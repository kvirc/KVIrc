//============================================================================
//
//   File : KviInputEditor.cpp
//   Creation date : Fri Sep 5 2008 17:26:34 by Elvio Basello
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your option) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//============================================================================

//
//   This file was originally part of KviInput.h
//

#include "kvi_fileextensions.h"
#include "kvi_out.h"
#include "KviApplication.h"
#include "KviChannelWindow.h"
#include "KviColorSelectionWindow.h"
#include "KviConsoleWindow.h"
#include "KviControlCodes.h"
#include "KviMainWindow.h"
#include "KviInput.h"
#include "KviInputEditor.h"
#include "KviInputHistory.h"
#include "KviIrcView.h"
#include "KviKvsArrayCast.h"
#include "KviKvsEventTriggers.h"
#include "KviKvsKernel.h"
#include "KviKvsScript.h"
#include "KviLocale.h"
#include "KviOptions.h"
#include "KviPixmapUtils.h"
#include "KviQString.h"
#include "KviShortcut.h"
#include "KviTextIconManager.h"
#include "KviTextIconWindow.h"
#include "KviUserInput.h"
#include "KviUserListView.h"
#include "KviWindowStack.h"

#include <QClipboard>
#include <QDragEnterEvent>
#include <QFileDialog>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QLabel>
#include <QMimeData>
#include <QPainter>
#include <QPixmap>
#include <QRegExp>
#include <QStyle>
#include <QStyleOption>
#include <QTextBoundaryFinder>
#include <QUrl>
#include <QWidgetAction>

#include <qdrawutil.h> // qDrawShadePanel

#include <algorithm>
#include <functional>
#include <utility>

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
#include <windows.h>
#endif //COMPILE_ON_WINDOWS || COMPILE_ON_MINGW

// from KviApplication.cpp
extern QMenu * g_pInputPopup;
extern KviTextIconWindow * g_pTextIconWindow;
extern KviColorWindow * g_pColorWindow;

#ifdef COMPILE_PSEUDO_TRANSPARENCY
extern QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif

//static members initialization
int KviInputEditor::g_iInputInstances = 0;
QFontMetricsF * KviInputEditor::g_pLastFontMetrics = nullptr;
int KviInputEditor::g_iCachedHeight = 0;

#define KVI_INPUT_MAX_UNDO_SIZE 256

class KviInputEditorTextBlock
{
public:
	enum Flags
	{
		IsControlBlock = 1,
		IsBold = 2,
		IsItalic = 4,
		IsUnderline = 8,
		IsSpellingMistake = 16,
		IsSelected = 32
	};

public:
	QString szText;
	qreal fWidth;
	int iLength;
	unsigned int uFlags;
	unsigned char uForeground;
	unsigned char uBackground;

public:
	KviInputEditorTextBlock(const QChar * p, int len)
	    : szText(p, len)
	{
	}

	KviInputEditorTextBlock(QString text)
	    : szText(std::move(text))
	{
	}
};

class KviInputEditorPrivate
{
public:
	QList<KviInputEditorTextBlock *> lTextBlocks;
	bool bTextBlocksDirty;
	qreal fXOffset; // positive (but shifts text to the left). Does NOT include margin.
	QString szFontElision;
	qreal fFontElisionWidth;
};

KviInputEditor::KviInputEditor(QWidget * pPar, KviWindow * pWnd, KviUserListView * pView)
    : QWidget(pPar)
{
	m_p = new KviInputEditorPrivate();
	m_p->bTextBlocksDirty = true;
	m_p->fXOffset = 0;
	m_p->szFontElision = QString::fromUtf8("…"); // DANGER: make sure this file is saved as utf8

	g_iInputInstances++;
	setObjectName("input_widget");

	m_pIconMenu = nullptr;
	m_pInputParent = pPar;
	m_iMaxBufferSize = KVI_INPUT_MAX_BUFFER_SIZE;
	m_iCursorPosition = 0;       //Index of the char AFTER the cursor
	m_iSelectionBegin = -1;      //Index of the first char in the selection
	m_iSelectionEnd = -1;        //Index of the last char in the selection
	m_bIMComposing = false;      //Whether the input method is active (composing).
	// for input method support
	m_iIMStart = 0;              //Index of the start of the preedit string.
	m_iIMLength = 0;             //Length of the preedit string.
	m_iIMSelectionBegin = 0;     //Index of the start of the selection in preedit string.
	m_iIMSelectionLength = 0;    //Length of the selection in preedit string.
	m_p->bTextBlocksDirty = true;

	m_bCursorOn = false;         //Cursor state
	m_iCursorTimer = 0;          //Timer that inverts the cursor state
	m_iDragTimer = 0;            //Timer for drag selection updates
	m_iSelectionAnchorChar = -1; //Character clicked at the beginning of the selection process
	m_iCurHistoryIdx = -1;       //No data in the history
	m_bUpdatesEnabled = true;
	m_pKviWindow = pWnd;
	m_pUserListView = pView;
	m_bReadOnly = false;
	m_bSpSlowFlag = false;       //Slow paste status flag

	setAttribute(Qt::WA_InputMethodEnabled, true);

	setAutoFillBackground(false);

	setFocusPolicy(Qt::StrongFocus);
	setAcceptDrops(true);

	m_pIconMenu = new QMenu();
	connect(m_pIconMenu, SIGNAL(triggered(QAction *)), this, SLOT(iconPopupActivated(QAction *)));

	setCursor(Qt::IBeamCursor);

	setContentsMargins(KVI_INPUT_MARGIN, KVI_INPUT_MARGIN, KVI_INPUT_MARGIN, KVI_INPUT_MARGIN);
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

	//set the font and font metrics; only the first inputeditor refresh the cached font metrics
	applyOptions(g_iInputInstances == 1);

	installShortcuts();
}

KviInputEditor::~KviInputEditor()
{
	--g_iInputInstances;
	if(g_iInputInstances == 0 && g_pLastFontMetrics)
	{
		//last instance, delete shared resources
		delete g_pLastFontMetrics;
		g_pLastFontMetrics = nullptr;
	}

	delete m_pIconMenu;

	if(m_iCursorTimer)
		killTimer(m_iCursorTimer);
	killDragTimer();

	qDeleteAll(m_p->lTextBlocks);
	delete m_p;
}

void KviInputEditor::applyOptions(bool bRefreshCachedMetrics)
{
	//set the font
	QFont newFont(KVI_OPTION_FONT(KviOption_fontInput));
	newFont.setKerning(false);
	newFont.setStyleStrategy(QFont::StyleStrategy(newFont.styleStrategy() | QFont::ForceIntegerMetrics));
	setFont(newFont);

	//set cursor custom width
	if(KVI_OPTION_BOOL(KviOption_boolEnableCustomCursorWidth))
	{
		m_iCursorWidth = KVI_OPTION_UINT(KviOption_uintCustomCursorWidth);
	}
	else
	{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		SystemParametersInfo(SPI_GETCARETWIDTH, 0, &m_iCursorWidth, 0);
#else //COMPILE_ON_WINDOWS || COMPILE_ON_MINGW
		m_iCursorWidth = 1;
#endif
	}

	if(bRefreshCachedMetrics)
	{
		//then, let font metrics be updated in lazy fashion
		if(g_pLastFontMetrics)
		{
			delete g_pLastFontMetrics;
			g_pLastFontMetrics = nullptr;
		}

		getLastFontMetrics(font());
	}
}

void KviInputEditor::dragEnterEvent(QDragEnterEvent * e)
{
	if(e->mimeData()->hasUrls())
		e->acceptProposedAction();
}

void KviInputEditor::dropEvent(QDropEvent * e)
{
	QList<QUrl> list;

	if(!e->mimeData()->hasUrls())
		return;

	list = e->mimeData()->urls();

	if(list.isEmpty())
		return;

	for(auto url : list)
	{
		QString szPath = url.toLocalFile();
		if(szPath.endsWith(KVI_FILEEXTENSION_SCRIPT, Qt::CaseInsensitive))
		{
			//script, parse it
			KviQString::escapeKvs(&szPath, KviQString::EscapeSpace);
			szPath.prepend("PARSE ");
			if(m_pKviWindow)
				KviKvsScript::run(szPath, m_pKviWindow);
		}
		else
		{
			//other file, paste link
			szPath.append(" ");
			insertText(szPath);
		}
	}
}

QFontMetricsF * KviInputEditor::getLastFontMetrics(const QFont & font)
{
	if(g_pLastFontMetrics)
		return g_pLastFontMetrics;

	g_pLastFontMetrics = new QFontMetricsF(font);

	m_p->fFontElisionWidth = g_pLastFontMetrics->width(m_p->szFontElision);

	//height calculation

	int h = qMax(g_pLastFontMetrics->height(), 14.0) + 2 * (KVI_INPUT_MARGIN + KVI_INPUT_XTRAPADDING);
	int w = 100;
	QStyleOptionFrame option;
	option.initFrom(this);
	option.rect = rect();
	option.lineWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, &option, this);
	option.midLineWidth = 0;

	option.state |= QStyle::State_Sunken;
	option.features = QStyleOptionFrame::None;

	QSize dummySize = style()->sizeFromContents(QStyle::CT_LineEdit, &option, QSize(w, h).expandedTo(QApplication::globalStrut()), this);

	g_iCachedHeight = dummySize.height();
	return g_pLastFontMetrics;
}

int KviInputEditor::heightHint() const
{
	return g_iCachedHeight;
}

bool KviInputEditor::checkWordSpelling(const QString & szWord)
{
#ifdef COMPILE_ENCHANT_SUPPORT
	if(szWord.isEmpty())
		return true;

	if(m_pKviWindow && m_pKviWindow->isChannel())
	{
		KviChannelWindow * c = static_cast<KviChannelWindow *>(m_pKviWindow);
		if(c->isOn(szWord))
		{
			// a nickname on channel
			return true;
		}
	}

	KviKvsVariant bCorrect;
	KviKvsVariantList params(new KviKvsVariant(szWord));
	KviKvsScript::evaluate("$spellchecker.check($0)", nullptr, &params, &bCorrect);

	return bCorrect.asBoolean();
#else
	return true; // assume correct
#endif
}

#define ADD_SPELLCHECKER_BLOCK(_lBuffer, _szText, _iStart, _bSpellCheckable, _bCorrect)   \
	do                                                                                    \
	{                                                                                     \
		KviInputEditorSpellCheckerBlock * pBlock = new KviInputEditorSpellCheckerBlock(); \
		pBlock->szText = _szText;                                                         \
		pBlock->iStart = _iStart;                                                         \
		pBlock->iLength = pBlock->szText.length();                                        \
		pBlock->bSpellCheckable = _bSpellCheckable;                                       \
		pBlock->bCorrect = _bCorrect;                                                     \
		_lBuffer.push_back(pBlock);                                                       \
	} while(0)

void KviInputEditor::splitTextIntoSpellCheckerBlocks(const QString & szText, std::vector<KviInputEditorSpellCheckerBlock *> & lBuffer)
{
#ifdef COMPILE_ENCHANT_SUPPORT
	if(szText.isEmpty())
		return;

	const QChar * const pBufferBegin = szText.unicode();
	const QChar * p = pBufferBegin;
	const QChar * e = pBufferBegin + szText.length();

	// first of all skip any spaces
	while(p < e)
	{
		if(!p->isSpace())
			break;
		p++;
	}

	if(p > pBufferBegin)
		ADD_SPELLCHECKER_BLOCK(lBuffer, QString(pBufferBegin, p - pBufferBegin), 0, false, false);

	// check if the buffer starts with a command
	if((p < e) && (p->unicode() == '/'))
	{
		const QChar * pCommandBlockBegin = p;

		// a command.
		p++;
		// skip spaces
		while(p < e)
		{
			if(!p->isSpace())
				break;
			p++;
		}

		const QChar * pCommandWordBegin = p;

		// skip command word
		while(p < e)
		{
			if(p->isSpace())
				break;
			p++;
		}

		if(p > pCommandWordBegin)
		{
			QString szCommand = QString(pCommandWordBegin, p - pCommandWordBegin).toLower();

			// common commands that may be spellcheckable
			static QString szMe("me");
			static QString szMsg("msg");
			static QString szSay("msg");
			static QString szPart("part");
			static QString szQuit("quit");

			if(
			    (szCommand != szMe) && (szCommand != szMsg) && (szCommand != szSay) && (szCommand != szPart) && (szCommand != szQuit))
			{
				// the command parameters usually have no spellcheckable text
				ADD_SPELLCHECKER_BLOCK(lBuffer, QString(pCommandBlockBegin, e - pCommandBlockBegin), 0, false, false);
				return;
			}
		}

		ADD_SPELLCHECKER_BLOCK(lBuffer, QString(pCommandBlockBegin, p - pCommandBlockBegin), pCommandBlockBegin - pBufferBegin, false, false);
	}

	const QChar * pNonWordBegin = nullptr;
	const QChar * pWordBegin = nullptr;

	while(p < e)
	{
		switch(p->category())
		{
			case QChar::Letter_Uppercase:
			case QChar::Letter_Lowercase:
			case QChar::Letter_Titlecase:
			case QChar::Letter_Modifier:
			case QChar::Letter_Other:
				if(!pWordBegin)
				{
					// a word starts here
					if(pNonWordBegin)
					{
						ADD_SPELLCHECKER_BLOCK(lBuffer, QString(pNonWordBegin, p - pNonWordBegin), pNonWordBegin - pBufferBegin, false, false);
						pNonWordBegin = nullptr;
					}
					pWordBegin = p;
				}
				else
				{
					// we're already in a word, go on
				}
				break;
			case QChar::Number_DecimalDigit:
			case QChar::Number_Letter:
			case QChar::Number_Other:
				if(!pWordBegin && !pNonWordBegin)
					pNonWordBegin = p; // start a non-word
				                           // else just go ahead keeping current state
				break;
			default:
				if(!pNonWordBegin)
				{
					if(p->unicode() == '\'')
					{
						// special case for the ' character which can be part of a word in many languages
						if(!pWordBegin)            // word not started
							pNonWordBegin = p; // then start a nonwod
						                           // else keep current state
					}
					else
					{
						if(pWordBegin)
						{
							// a word to spellcheck
							if((p - 1)->unicode() == '\'') // exclude the trailing ' from the word
								p--;                   // go back one char, so it will become a non-word

							QString szWord(pWordBegin, p - pWordBegin);
							ADD_SPELLCHECKER_BLOCK(lBuffer, szWord, pWordBegin - pBufferBegin, true, checkWordSpelling(szWord));
							pWordBegin = nullptr;
						}
						pNonWordBegin = p;
					}
				}
				else
				{
					// already in a non-word
				}
				break;
		}

		p++;
	}

	if(pWordBegin)
	{
		// last word to spellcheck
		QString szWord(pWordBegin, p - pWordBegin);
		ADD_SPELLCHECKER_BLOCK(lBuffer, szWord, pWordBegin - pBufferBegin, true, checkWordSpelling(szWord));
	}
	else if(pNonWordBegin)
	{
		// last non-word
		ADD_SPELLCHECKER_BLOCK(lBuffer, QString(pNonWordBegin, p - pNonWordBegin), pNonWordBegin - pBufferBegin, false, false);
	}
#endif
}

void KviInputEditor::rebuildTextBlocks()
{
	qDeleteAll(m_p->lTextBlocks);
	m_p->lTextBlocks.clear();

	std::vector<KviInputEditorSpellCheckerBlock *> lSpellCheckerBlocks;

#ifdef COMPILE_ENCHANT_SUPPORT
	splitTextIntoSpellCheckerBlocks(m_szTextBuffer, lSpellCheckerBlocks);
#else
	ADD_SPELLCHECKER_BLOCK(lSpellCheckerBlocks, m_szTextBuffer, 0, false, true);
#endif

	m_p->bTextBlocksDirty = false;

	if(lSpellCheckerBlocks.empty()) // should never happen, but well...
		return;                   // nothing to do

#define NOT_CONTROL_CHAR() \
	(                      \
	    (c > 32) || ((c != KviControlCodes::Color) && (c != KviControlCodes::Bold) && (c != KviControlCodes::Italic) && (c != KviControlCodes::Underline) && (c != KviControlCodes::Reset) && (c != KviControlCodes::Reverse) && (c != KviControlCodes::CryptEscape) && (c != KviControlCodes::Icon)))

	// FIXME: get rid of getLastFontMetrics() ?
	QFontMetricsF * fm = getLastFontMetrics(font());

	unsigned int uFlags = 0;
	unsigned char uCurFore = KVI_INPUT_DEF_FORE;
	unsigned char uCurBack = KVI_INPUT_DEF_BACK;

	KviInputEditorTextBlock * pBlock;

	for(auto spb : lSpellCheckerBlocks)
	{
		if(spb->bSpellCheckable && !spb->bCorrect)
			uFlags |= KviInputEditorTextBlock::IsSpellingMistake;
		else
			uFlags &= ~KviInputEditorTextBlock::IsSpellingMistake;

		const QChar * pBuffer = spb->szText.unicode();
		const QChar * p = pBuffer;
		const QChar * e = p + spb->szText.length();

		while(p < e)
		{
			ushort c = p->unicode();

			if(NOT_CONTROL_CHAR())
			{
				const QChar * b = p;
				p++;
				while(p < e)
				{
					c = p->unicode();
					if(!NOT_CONTROL_CHAR())
						break;
					p++;
				}

				pBlock = new KviInputEditorTextBlock(b, p - b);
				pBlock->uForeground = uCurFore;
				pBlock->uBackground = uCurBack;
				pBlock->uFlags = uFlags;
				pBlock->fWidth = fm->width(pBlock->szText);
				pBlock->iLength = pBlock->szText.length();
				m_p->lTextBlocks.append(pBlock);
				continue;
			}

			// control char
			switch(c)
			{
				case KviControlCodes::Bold:
					if(uFlags & KviInputEditorTextBlock::IsBold)
						uFlags &= ~KviInputEditorTextBlock::IsBold;
					else
						uFlags |= KviInputEditorTextBlock::IsBold;
					break;
				case KviControlCodes::Italic:
					if(uFlags & KviInputEditorTextBlock::IsItalic)
						uFlags &= ~KviInputEditorTextBlock::IsItalic;
					else
						uFlags |= KviInputEditorTextBlock::IsItalic;
					break;
				case KviControlCodes::Underline:
					if(uFlags & KviInputEditorTextBlock::IsUnderline)
						uFlags &= ~KviInputEditorTextBlock::IsUnderline;
					else
						uFlags |= KviInputEditorTextBlock::IsUnderline;
					break;
				case KviControlCodes::Reset:
					uCurFore = KVI_INPUT_DEF_FORE;
					uCurBack = KVI_INPUT_DEF_BACK;
					uFlags = 0;
					break;
				case KviControlCodes::Reverse:
				{
					char cAuxClr = uCurFore;
					uCurFore = uCurBack;
					uCurBack = cAuxClr;
				}
				break;
				case KviControlCodes::CryptEscape:
				case KviControlCodes::Icon:
					// makes a single block
					break;
				case KviControlCodes::Color:
				{
					p++;
					if(p < e)
					{
						unsigned char uFore;
						unsigned char uBack;
						/* int iNextChar = */ KviControlCodes::getUnicodeColorBytes(spb->szText, p - spb->szText.unicode(), &uFore, &uBack);
						if(uFore != KviControlCodes::NoChange)
						{
							uCurFore = uFore;
							if(uBack != KviControlCodes::NoChange)
								uCurBack = uBack;
						}
						else
						{
							// ONLY a Ctrl+K
							uCurBack = KVI_INPUT_DEF_BACK;
							uCurFore = KVI_INPUT_DEF_FORE;
						}
					}
					p--; // there is a p++ below
				}
				break;
				default:
					Q_ASSERT(false);
					break;
			}

			QChar cSubstitute = getSubstituteChar(c);
			pBlock = new KviInputEditorTextBlock(QString(cSubstitute));
			pBlock->fWidth = fm->width(pBlock->szText) + 4;
			pBlock->uFlags = uFlags | KviInputEditorTextBlock::IsControlBlock;
			pBlock->uForeground = uCurFore;
			pBlock->uBackground = uCurBack;
			pBlock->iLength = pBlock->szText.length();
			m_p->lTextBlocks.append(pBlock);

			p++;
		}
	}

	// apply selection marks too
	if(
	    (m_iSelectionEnd <= m_iSelectionBegin) || (m_iSelectionEnd == -1) || (m_iSelectionBegin == -1) || (m_iSelectionBegin > m_szTextBuffer.length()))
	{
		m_iSelectionEnd = -1;
		m_iSelectionBegin = -1;
	}
	else
	{

		int iCurStart = 0;
		int cnt = m_p->lTextBlocks.count();

		for(int idx = 0; idx < cnt; idx++)
		{
			pBlock = m_p->lTextBlocks.at(idx);

			if(m_iSelectionEnd <= iCurStart)
				break; // finished

			int iCurEnd = iCurStart + pBlock->iLength;

			if(pBlock->iLength < 1)
			{
				iCurStart = iCurEnd;
				continue; // zero length blocks don't matter (hm.. do we have them?)
			}

			if(m_iSelectionBegin < iCurEnd)
			{
				// selection begins before block end
				if(m_iSelectionBegin < iCurStart)
				{
					// selection begins before this block
					pBlock->uFlags |= KviInputEditorTextBlock::IsSelected;
				}
				else if(m_iSelectionBegin == iCurStart)
				{
					// selection begins exactly at this block
					pBlock->uFlags |= KviInputEditorTextBlock::IsSelected;
				}
				else
				{
					// selection begins in the middle of the block
					int iLeft = m_iSelectionBegin - iCurStart;
					KviInputEditorTextBlock * pBlock2 = new KviInputEditorTextBlock(pBlock->szText.left(iLeft));
					pBlock2->uForeground = pBlock->uForeground;
					pBlock2->uBackground = pBlock->uBackground;
					pBlock2->uFlags = pBlock->uFlags;
					pBlock2->fWidth = fm->width(pBlock2->szText);
					pBlock2->iLength = iLeft;
					m_p->lTextBlocks.insert(idx, pBlock2);
					pBlock->szText.remove(0, iLeft);
					pBlock->fWidth = fm->width(pBlock->szText);
					pBlock->iLength -= iLeft;
					pBlock->uFlags |= KviInputEditorTextBlock::IsSelected;
					iCurStart += iLeft;
					idx++;
					cnt++;
				}

				if(m_iSelectionEnd >= iCurEnd)
				{
				}	// selection ends after or at the end of this block
				else
				{
					// selection ends in the middle of the block
					int iLeft = m_iSelectionEnd - iCurStart;

					KviInputEditorTextBlock * pBlock2 = new KviInputEditorTextBlock(pBlock->szText.left(iLeft));
					pBlock2->uForeground = pBlock->uForeground;
					pBlock2->uBackground = pBlock->uBackground;
					pBlock2->uFlags = pBlock->uFlags;
					pBlock2->fWidth = fm->width(pBlock2->szText);
					pBlock2->iLength = iLeft;
					m_p->lTextBlocks.insert(idx, pBlock2);
					pBlock->szText.remove(0, iLeft);
					pBlock->fWidth = fm->width(pBlock->szText);
					pBlock->iLength = pBlock->szText.length();
					pBlock->uFlags &= ~KviInputEditorTextBlock::IsSelected;
					break;
				}
			}

			iCurStart = iCurEnd;
		}
	}
}

void KviInputEditor::ensureCursorVisible()
{
	if(m_iCursorPosition < 0)
		m_iCursorPosition = 0;
	else if(m_iCursorPosition > m_szTextBuffer.length())
		m_iCursorPosition = m_szTextBuffer.length();

	qreal fCursorXAbsolute = xPositionFromCharIndex(m_iCursorPosition) - KVI_INPUT_MARGIN;
	qreal fCursorXInText = fCursorXAbsolute + m_p->fXOffset;
	qreal fEndXAbsolute = xPositionFromCharIndex(m_szTextBuffer.length()) - KVI_INPUT_MARGIN;

	bool bNeedRepaint = false;

	qreal fAvailableWidth = width() - KVI_INPUT_MARGIN - KVI_INPUT_MARGIN;

	if(fAvailableWidth > fEndXAbsolute)
	{
		m_p->fXOffset += fEndXAbsolute - fAvailableWidth;
		if(m_p->fXOffset < 0.0)
			m_p->fXOffset = 0.0;
		bNeedRepaint = true;
	}
	else if(fCursorXAbsolute < 0.0)
	{
		m_p->fXOffset = fCursorXInText;
		bNeedRepaint = true;
	}
	else if(fCursorXAbsolute > fAvailableWidth)
	{
		m_p->fXOffset = fCursorXInText - fAvailableWidth;
		if(m_p->fXOffset < 0.0)
			m_p->fXOffset = 0.0;
		bNeedRepaint = true;
	}

	if(bNeedRepaint)
		update();
}

void KviInputEditor::paintEvent(QPaintEvent *)
{
	QPainter p(this);

	QRect cr = rect();

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency) && g_pApp->supportsCompositing())
	{
		p.save();
		p.setCompositionMode(QPainter::CompositionMode_Source);
		QColor col = KVI_OPTION_COLOR(KviOption_colorGlobalTransparencyFade);
		col.setAlphaF((float)((float)KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) / (float)100));
		p.fillRect(cr, col);
		p.restore();
	}
	else if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt = m_pKviWindow->isDocked() ? mapTo(g_pMainWindow, cr.topLeft()) : mapTo(m_pKviWindow, cr.topLeft());
		p.drawTiledPixmap(cr, *(g_pShadedChildGlobalDesktopBackground), pnt);
	}
	else
	{
#endif
		p.fillRect(cr, KVI_OPTION_COLOR(KviOption_colorInputBackground));

		QPixmap * pix = KVI_OPTION_PIXMAP(KviOption_pixmapInputBackground).pixmap();
		if(pix)
			KviPixmapUtils::drawPixmapWithPainter(&p, pix, KVI_OPTION_UINT(KviOption_uintInputPixmapAlign), cr, cr.width(), cr.height());
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	}
#endif

	// In Qt5 QStyle::drawPrimitive seems to always overwrite the background, no matter what.
	qDrawShadePanel(&p, 0, 0, width(), height(), palette(), true, 1, nullptr);

	QRect r(1, 1, width() - 1, height() - 1);

	p.setClipRect(r);

	//p.translate(r.topLeft());
	drawContents(&p);
}

void KviInputEditor::drawContents(QPainter * p)
{
	QRect rect = p->clipRegion().boundingRect();
	QFontMetricsF * fm = getLastFontMetrics(font());

	if(m_p->bTextBlocksDirty)
		rebuildTextBlocks();

	qreal fCurX = -m_p->fXOffset + KVI_INPUT_MARGIN;

	KviInputEditorTextBlock * pBlock;

	qreal fWidth = width();

	int iTop = KVI_INPUT_MARGIN + KVI_INPUT_XTRAPADDING;
	int iBottom = rect.height() - KVI_INPUT_MARGIN - KVI_INPUT_XTRAPADDING;

	int iTextBaseline = iBottom - fm->descent();

	QFont::Style normalFontStyle = p->font().style();

	// When m_bIMComposing is true, the text between m_iIMStart and m_iIMStart+m_iIMLength should be highlighted to show that this is the active
	// preedit area for the input method, and the text outside cannot be edited while
	// composing. Maybe this can be implemented similarly as painting the selection?
	// Also notice that inside the preedit, there can also be a selection, given by
	// m_iSelectionBegin and m_iSelectionLength, and the widget needs to highlight that
	// while in IM composition mode
	if(m_bIMComposing && m_iIMLength > 0)
	{
		int iIMSelectionStart = m_iIMSelectionBegin;

		qreal iXIMSelectionLeft = xPositionFromCharIndex(iIMSelectionStart);
		qreal iXIMSelectionRight = xPositionFromCharIndex(iIMSelectionStart + m_iIMSelectionLength);
		p->fillRect(QRectF(iXIMSelectionLeft, iTop, iXIMSelectionRight - iXIMSelectionLeft, iBottom), KVI_OPTION_COLOR(KviOption_colorInputSelectionBackground));

		// highlight the IM selection
		int iIMStart = m_iIMStart;
		int xIMLeft = xPositionFromCharIndex(iIMStart);
		int xIMRight = xPositionFromCharIndex(iIMStart + m_iIMLength);

		// underline the IM preedit
		// Maybe should be put in drawTextBlock, similar to drawing underlined text
		p->drawLine(xIMLeft, iBottom, xIMRight, iBottom);
	}

	foreach(pBlock, m_p->lTextBlocks)
	{
		qreal fNextX = fCurX + pBlock->fWidth;
		if((fNextX > 0) && (pBlock->fWidth > 0))
		{
			// draw this block
			bool bIsSelected = pBlock->uFlags & KviInputEditorTextBlock::IsSelected;

			if(bIsSelected)
				p->fillRect(QRectF(fCurX, iTop - 2, pBlock->fWidth, iBottom - iTop + 4), KVI_OPTION_COLOR(KviOption_colorInputSelectionBackground));

			if(pBlock->uFlags & KviInputEditorTextBlock::IsControlBlock)
			{
				p->setPen(KVI_OPTION_COLOR(KviOption_colorInputControl));
				// the block width is 4 pixels more than the actual character
				p->drawText(QPointF(fCurX + 2, iTextBaseline), pBlock->szText);
				p->drawRect(QRectF(fCurX, iTop, pBlock->fWidth, iBottom - iTop));
			}
			else
			{
				if(pBlock->uForeground == KVI_INPUT_DEF_FORE)
					p->setPen(bIsSelected ? KVI_OPTION_COLOR(KviOption_colorInputSelectionForeground) : KVI_OPTION_COLOR(KviOption_colorInputForeground));
				else if(pBlock->uForeground > KVI_EXTCOLOR_MAX)
					p->setPen(KVI_OPTION_COLOR(KviOption_colorInputBackground));
				else
					p->setPen(getMircColor(pBlock->uForeground));

				if(pBlock->uBackground != KVI_INPUT_DEF_BACK)
				{
					if(pBlock->uBackground > KVI_EXTCOLOR_MAX)
						p->fillRect(QRectF(fCurX, iTop, pBlock->fWidth, iBottom - iTop), KVI_OPTION_COLOR(KviOption_colorInputForeground));
					else
						p->fillRect(QRectF(fCurX, iTop, pBlock->fWidth, iBottom - iTop), getMircColor(pBlock->uBackground));
				}


				if (pBlock->uFlags & KviInputEditorTextBlock::IsItalic)
				{
					QFont newFont = p->font();
					newFont.setStyle(normalFontStyle == QFont::StyleNormal ? QFont::StyleItalic : QFont::StyleNormal);
					p->setFont(newFont);
				}

				p->drawText(QPointF(fCurX, iTextBaseline), pBlock->szText);

				if(pBlock->uFlags & KviInputEditorTextBlock::IsBold)
					p->drawText(QPointF(fCurX + 1, iTextBaseline), pBlock->szText);

				if(pBlock->uFlags & KviInputEditorTextBlock::IsUnderline)
					p->drawLine(QPointF(fCurX, iTextBaseline + fm->descent()), QPointF(fCurX + pBlock->fWidth, iTextBaseline + fm->descent()));

				if(pBlock->uFlags & KviInputEditorTextBlock::IsSpellingMistake)
				{
					p->setPen(QPen(Qt::red, 1)); // red overlay
					int iY = iTextBaseline + fm->descent() - 1;
					p->fillRect(QRectF(fCurX, iTop, pBlock->fWidth, iY - iTop), QColor(255, 0, 0, 30)); // alpha = 30
					p->drawLine(QPointF(fCurX, iY), QPointF(fCurX + pBlock->fWidth, iY));
				}

				if (pBlock->uFlags & KviInputEditorTextBlock::IsItalic)
				{
					QFont newFont = p->font();
					newFont.setStyle(normalFontStyle);
					p->setFont(newFont);
				}
			}
		}
		fCurX = fNextX;
		if(fCurX > fWidth)
			break;
	}

	qreal fCursorX = xPositionFromCharIndex(m_iCursorPosition);

	if(m_bCursorOn)
	{
		p->setClipping(false);
		p->setPen(KVI_OPTION_COLOR(KviOption_colorInputCursor));
		p->fillRect(QRectF(fCursorX, iTop, m_iCursorWidth, iBottom - iTop), KVI_OPTION_COLOR(KviOption_colorInputCursor));
		p->setClipping(true);
	}
}

QChar KviInputEditor::getSubstituteChar(unsigned short uControlCode)
{
	switch(uControlCode)
	{
		case KviControlCodes::Color:
			return QChar('K');
			break;
		case KviControlCodes::Bold:
			return QChar('B');
			break;
		case KviControlCodes::Italic:
			return QChar('I');
			break;
		case KviControlCodes::Reset:
			return QChar('O');
			break;
		case KviControlCodes::Reverse:
			return QChar('R');
			break;
		case KviControlCodes::Underline:
			return QChar('U');
			break;
		case KviControlCodes::CryptEscape:
			return QChar('P');
			break;
		case KviControlCodes::Icon:
			return QChar('E');
			break;
		default:
			return { uControlCode };
			break;
	}
}

void KviInputEditor::mouseDoubleClickEvent(QMouseEvent * e)
{
	//select clicked word
	if(!(e->button() & Qt::LeftButton))
		return;

	if(m_szTextBuffer.length() < 1)
		return;

	int iCursor = charIndexFromXPosition(e->pos().x());
	int iLen = m_szTextBuffer.length() - 1;
	if(iCursor > iLen)
		iCursor = iLen;

	if(!m_szTextBuffer.at(iCursor).isLetterOrNumber())
		return;

	//search word init
	m_iSelectionBegin = iCursor;
	while(m_iSelectionBegin > 0)
	{
		if(!m_szTextBuffer.at(m_iSelectionBegin - 1).isLetterOrNumber())
			break;
		m_iSelectionBegin--;
	}

	//search word end
	m_iSelectionEnd = iCursor;
	while(m_iSelectionEnd < iLen + 1)
	{
		if(!m_szTextBuffer.at(m_iSelectionEnd).isLetterOrNumber())
			break;
		m_iSelectionEnd++;
	}

	m_p->bTextBlocksDirty = true;

	moveCursorTo(m_iSelectionEnd, true);
	killDragTimer();
}

void KviInputEditor::showContextPopup(const QPoint & pos)
{
	int iType = m_pKviWindow->type();

	//Popup menu
	g_pInputPopup->clear();

	QString szClip;

	QClipboard * pClip = QApplication::clipboard();
	if(pClip)
	{
		szClip = pClip->text(QClipboard::Clipboard);

		int iOcc = szClip.count(QChar('\n'));

		if(!szClip.isEmpty())
		{
			// Prevent too many newlines from spamming the context menu
			szClip.replace(QRegExp("^((?:[^\n]*\n){6}).*"), "\\1...");

			if(szClip.length() > 60)
			{
				szClip.truncate(60);
				szClip.append("...");
			}

			szClip.replace(QChar('&'), "&amp;");
			szClip.replace(QChar('<'), "&lt;");
			szClip.replace(QChar('>'), "&gt;");
			szClip.replace(QChar('\n'), "<br>");

			QString szLabel = "<html><body>";

			// Title
			szLabel += "<table width=\"100%\">";
			szLabel += START_TABLE_BOLD_ROW;
			szLabel += "<center><b>";
			szLabel += __tr2qs("Clipboard");
			szLabel += "</b></center>";
			szLabel += END_TABLE_BOLD_ROW;
			szLabel += "</table>";

			// Clipboard contents (truncated for display)
			szLabel += szClip;

			// Line breaks count
			szLabel += "<br><b><center>";
			szLabel += QString::number(iOcc);
			szLabel += QChar(' ');
			szLabel += (iOcc == 1) ? __tr2qs("line break") : __tr2qs("line breaks");
			szLabel += "</b></center>";

			szLabel += "</body></html>";

			QLabel * pLabel = new QLabel(szLabel, g_pInputPopup);
			pLabel->setFrameStyle(QFrame::Raised | QFrame::StyledPanel);
			pLabel->setMargin(5);

			QWidgetAction * pAction = new QWidgetAction(g_pInputPopup);
			pAction->setDefaultWidget(pLabel);

			g_pInputPopup->addAction(pAction);
		}
	}

	QAction * pAction = g_pInputPopup->addAction(__tr2qs("&Undo") + ACCEL_KEY(Z), this, SLOT(undo()));
	pAction->setEnabled(isUndoAvailable());
	pAction = g_pInputPopup->addAction(__tr2qs("&Redo") + ACCEL_KEY(Y), this, SLOT(redo()));
	pAction->setEnabled(isRedoAvailable());

	g_pInputPopup->addSeparator();

	pAction = g_pInputPopup->addAction(__tr2qs("Cu&t") + ACCEL_KEY(X), this, SLOT(cut()));
	pAction->setEnabled(hasSelection());
	pAction = g_pInputPopup->addAction(__tr2qs("&Copy") + ACCEL_KEY(C), this, SLOT(copyToClipboard()));
	pAction->setEnabled(hasSelection());
	pAction = g_pInputPopup->addAction(__tr2qs("&Paste") + ACCEL_KEY(V), this, SLOT(pasteClipboardWithConfirmation()));
	pAction->setEnabled(!szClip.isEmpty() && !m_bReadOnly);
	pAction = g_pInputPopup->addAction(__tr2qs("Paste Slowly"), this, SLOT(pasteSlow()));
	if((iType == KviWindow::Channel) || (iType == KviWindow::Query) || (iType == KviWindow::DccChat))
		pAction->setEnabled(!szClip.isEmpty() && !m_bReadOnly);
	else
		pAction->setEnabled(false);
	pAction = g_pInputPopup->addAction(__tr2qs("Paste &File") + ACCEL_KEY(L), this, SLOT(pasteFile()));
	if((iType != KviWindow::Channel) && (iType != KviWindow::Query) && (iType != KviWindow::DccChat))
		pAction->setEnabled(false);
	else
		pAction->setEnabled(!m_bReadOnly);
	if(m_bSpSlowFlag)
		pAction = g_pInputPopup->addAction(__tr2qs("Stop Paste"), this, SLOT(stopPasteSlow())); /*G&N 2005*/

	g_pInputPopup->addSeparator();

	pAction = g_pInputPopup->addAction(__tr2qs("Clear"), this, SLOT(clear()));
	pAction->setEnabled(!m_szTextBuffer.isEmpty() && !m_bReadOnly);

	g_pInputPopup->addSeparator();

	pAction = g_pInputPopup->addAction(__tr2qs("Select All"), this, SLOT(selectAll()));
	pAction->setEnabled((!m_szTextBuffer.isEmpty()));

	g_pInputPopup->addSeparator();

	pAction = g_pInputPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::BigGrin)), __tr2qs("Insert Icon"), this, SLOT(popupTextIconWindow()));
	pAction->setEnabled(!m_bReadOnly);

#ifdef COMPILE_ENCHANT_SUPPORT
	// check if the cursor is in a spell-checkable block

	std::vector<KviInputEditorSpellCheckerBlock *> lBuffer;
	splitTextIntoSpellCheckerBlocks(m_szTextBuffer, lBuffer);

	KviInputEditorSpellCheckerBlock * pCurrentBlock = findSpellCheckerBlockAtCursor(lBuffer);

	if(pCurrentBlock && pCurrentBlock->bSpellCheckable && (!pCurrentBlock->bCorrect))
	{
		g_pInputPopup->addSeparator();

		fillSpellCheckerCorrectionsPopup();
		m_SpellCheckerPopup.setTitle(__tr2qs("Correct Spelling of '%1'").arg(pCurrentBlock->szText));
		m_SpellCheckerPopup.setEnabled(!m_bReadOnly);
		g_pInputPopup->addMenu(&m_SpellCheckerPopup);
	}
	else
	{
		pAction = g_pInputPopup->addAction(__tr2qs("Correct Spelling"));
		pAction->setEnabled(false);
	}
#endif

/*
 * With Qt5 the use of input method composing works, but we are unable to query the list of
 * available ims and change the active one. By now Qt5's QInputMethod lacks several methods,
 * check it again on newer qt versions!
 */
	// Qt4 code, needs to be re-implemented in >= Qt5
	// See above for more info.
	//
	// QInputContext * qic = g_pApp->inputContext();
	// if(qic)
	// {
	// 	QList<QAction *> imActions = qic->actions();
	// 	for(int i = 0; i < imActions.size(); ++i)
	// 		g_pInputPopup->addAction(imActions.at(i));
	// }

	g_pInputPopup->popup(pos);
}

void KviInputEditor::showContextPopupHere()
{
	qreal fXPos = xPositionFromCharIndex(m_iCursorPosition);
	int iBottom = heightHint() - KVI_INPUT_MARGIN - KVI_INPUT_XTRAPADDING; // not exact but easy
	showContextPopup(mapToGlobal(QPoint(fXPos, iBottom)));
}

KviInputEditorSpellCheckerBlock * KviInputEditor::findSpellCheckerBlockAtCursor(std::vector<KviInputEditorSpellCheckerBlock *> & lBlocks)
{
	KviInputEditorSpellCheckerBlock * pCurrentBlock = nullptr;

	for(auto pBlock : lBlocks)
	{
		if(m_iCursorPosition <= (pBlock->iStart + pBlock->iLength))
		{
			pCurrentBlock = pBlock;
			break;
		}
	}

	return pCurrentBlock;
}

void KviInputEditor::fillSpellCheckerCorrectionsPopup()
{
	m_SpellCheckerPopup.clear();

	QWidgetAction * pWidgetAction = new QWidgetAction(g_pInputPopup);
	QLabel * pLabel = new QLabel(&m_SpellCheckerPopup);
	pLabel->setFrameStyle(QFrame::Raised | QFrame::StyledPanel);
	pLabel->setMargin(5);

	pWidgetAction->setDefaultWidget(pLabel);
	m_SpellCheckerPopup.addAction(pWidgetAction);

#ifdef COMPILE_ENCHANT_SUPPORT
	// check if the cursor is in a spellcheckable block

	std::vector<KviInputEditorSpellCheckerBlock *> lBuffer;
	splitTextIntoSpellCheckerBlocks(m_szTextBuffer, lBuffer);

	KviInputEditorSpellCheckerBlock * pCurrentBlock = findSpellCheckerBlockAtCursor(lBuffer);

	if(!pCurrentBlock)
		return; // doh?

	if(!pCurrentBlock->bSpellCheckable)
	{
		pLabel->setText(__tr2qs("Spell Checker"));
		m_SpellCheckerPopup.addAction(__tr2qs("No Suggestions Available"))->setEnabled(false);
	}
	else
	{

		pLabel->setText(__tr2qs("Spelling Suggestions for '%1'").arg(pCurrentBlock->szText));
		m_SpellCheckerPopup.addAction(pWidgetAction);

		KviKvsVariant aRet;
		KviKvsVariantList params(new KviKvsVariant(pCurrentBlock->szText));
		KviKvsScript::evaluate("$spellchecker.suggestions($0)", nullptr, &params, &aRet);

		KviKvsArrayCast aCast;
		aRet.castToArray(&aCast);

		KviKvsArray * pArray = aCast.array();

		int s = pArray->size();

		for(int i = 0; i < s; i++)
		{
			KviKvsVariant * v = pArray->at(i);
			if(!v)
				continue;

			QString szWord;
			v->asString(szWord);

			if(szWord.isEmpty())
				continue;

			m_SpellCheckerPopup.addAction(szWord, this, SLOT(spellCheckerPopupCorrectionActionTriggered()));
		}
	}

#else

	pLabel->setText(__tr2qs("Spelling Checker"));

	m_SpellCheckerPopup.addAction(__tr2qs("Not Supported"))->setEnabled(false);

#endif
}

void KviInputEditor::showSpellCheckerCorrectionsPopup()
{
	fillSpellCheckerCorrectionsPopup();

	qreal fXPos = xPositionFromCharIndex(m_iCursorPosition);
	if(fXPos > 24)
		fXPos -= 24;

	QSize sh = m_SpellCheckerPopup.sizeHint();

	m_SpellCheckerPopup.popup(mapToGlobal(QPoint(fXPos, -sh.height())));
}

void KviInputEditor::spellCheckerPopupCorrectionActionTriggered()
{
	QObject * pSender = sender();
	if(!pSender)
		return;
	if(!pSender->inherits("QAction"))
		return;

	QAction * pAction = (QAction *)pSender;
	QString szWord = pAction->text();

	if(szWord.isEmpty())
		return;

	std::vector<KviInputEditorSpellCheckerBlock *> lBuffer;
	splitTextIntoSpellCheckerBlocks(m_szTextBuffer, lBuffer);

	KviInputEditorSpellCheckerBlock * pCurrentBlock = findSpellCheckerBlockAtCursor(lBuffer);

	if(!pCurrentBlock)
		return; // doh?

	m_iSelectionBegin = pCurrentBlock->iStart;
	m_iSelectionEnd = pCurrentBlock->iStart + pCurrentBlock->iLength;

	m_p->bTextBlocksDirty = true;

	insertText(szWord);
}

void KviInputEditor::mousePressEvent(QMouseEvent * e)
{
	if(e->button() & Qt::LeftButton)
	{
		m_iCursorPosition = std::min(charIndexFromXPosition(e->pos().x()), m_szTextBuffer.length());
		m_iSelectionAnchorChar = m_iCursorPosition;
		clearSelection();
		repaintWithCursorOn();
		killDragTimer();
		m_iDragTimer = startTimer(KVI_INPUT_DRAG_TIMEOUT);
		return;
	}

	if(e->button() & Qt::RightButton)
	{
		showContextPopup(mapToGlobal(e->pos()));
		return;
	}

	pasteSelectionWithConfirmation();
}

void KviInputEditor::iconPopupActivated(QAction * pAction)
{
	if(m_bReadOnly)
		return;

	QString szText = pAction->text();
	if(!szText.isEmpty())
	{
		szText.prepend(KviControlCodes::Icon);
		szText.append(' ');
		insertText(szText);
	}
}

bool KviInputEditor::hasSelection()
{
	return ((m_iSelectionBegin != -1) && (m_iSelectionEnd != -1));
}

void KviInputEditor::copyToClipboard()
{
	if(!hasSelection())
		return;

	QClipboard * pClip = QApplication::clipboard();
	if(!pClip)
		return;

	QString szTxt = m_szTextBuffer.mid(m_iSelectionBegin, m_iSelectionEnd - m_iSelectionBegin);

	pClip->setText(szTxt, QClipboard::Clipboard);

	repaintWithCursorOn();
}

void KviInputEditor::copyToSelection(bool bDonNotCopyToClipboard)
{
	if(!hasSelection())
		return;

	QClipboard * pClip = QApplication::clipboard();
	if(!pClip)
		return;

	QString szTxt = m_szTextBuffer.mid(m_iSelectionBegin, m_iSelectionEnd - m_iSelectionBegin);

	if(pClip->supportsSelection())
		pClip->setText(szTxt, QClipboard::Selection);
	else if(!bDonNotCopyToClipboard)
		pClip->setText(szTxt, QClipboard::Clipboard);

	repaintWithCursorOn();
}

void KviInputEditor::moveCursorTo(int iIdx, bool bRepaint)
{
	if(iIdx < 0)
		iIdx = 0;
	if(iIdx > ((int)(m_szTextBuffer.length())))
		iIdx = m_szTextBuffer.length();

	if(iIdx > m_iCursorPosition)
	{
		while(m_iCursorPosition < iIdx)
			m_iCursorPosition++;
	}
	else
	{
		m_iCursorPosition = iIdx;
	}

	ensureCursorVisible();
	if(bRepaint)
		repaintWithCursorOn();
}

void KviInputEditor::removeSelected()
{
	if(!hasSelection())
		return;

	addUndo(new EditCommand(EditCommand::RemoveText, m_szTextBuffer.mid(m_iSelectionBegin, m_iSelectionEnd - m_iSelectionBegin), m_iSelectionBegin));

	m_szTextBuffer.remove(m_iSelectionBegin, m_iSelectionEnd - m_iSelectionBegin);
	m_p->bTextBlocksDirty = true;
	moveCursorTo(m_iSelectionBegin, false);
	clearSelection();
	repaintWithCursorOn();
}

void KviInputEditor::cut()
{
	if(!hasSelection())
		return;

	QClipboard * pClip = QApplication::clipboard();

	if(!pClip)
		return;

	pClip->setText(m_szTextBuffer.mid(m_iSelectionBegin, m_iSelectionEnd - m_iSelectionBegin), QClipboard::Clipboard);

	addUndo(new EditCommand(EditCommand::RemoveText, m_szTextBuffer.mid(m_iSelectionBegin, m_iSelectionEnd - m_iSelectionBegin), m_iSelectionBegin));

	m_szTextBuffer.remove(m_iSelectionBegin, m_iSelectionEnd - m_iSelectionBegin);
	m_p->bTextBlocksDirty = true;
	moveCursorTo(m_iSelectionBegin, false);
	clearSelection();
	repaintWithCursorOn();
}

void KviInputEditor::insertText(const QString & szTxt)
{
	QString szText = szTxt; // crop away constness
	if(szText.isEmpty())
		return;

	szText.replace('\t', QString(KVI_OPTION_UINT(KviOption_uintSpacesToExpandTabulationInput), ' ')); //expand tabs to spaces

	m_bUpdatesEnabled = false;
	removeSelected();
	m_bUpdatesEnabled = true;

	if(szText.indexOf('\n') == -1)
	{
		addUndo(new EditCommand(EditCommand::InsertText, szText, m_iCursorPosition));

		m_szTextBuffer.insert(m_iCursorPosition, szText);
		m_szTextBuffer.truncate(m_iMaxBufferSize);
		m_p->bTextBlocksDirty = true;
		moveCursorTo(m_iCursorPosition + szText.length());
	}
	else
	{
		//Multiline paste...do not execute commands here
		QString szBlock;
		while(!szText.isEmpty())
		{
			int iIdx = szText.indexOf('\n');
			if(iIdx != -1)
			{
				szBlock = szText.left(iIdx);
				//else szBlock = QChar(KviControlCodes::Reset);
				szText.remove(0, iIdx + 1);
			}
			else
			{
				szBlock = szText;
				szText = "";
			}

			m_szTextBuffer.insert(m_iCursorPosition, szBlock);
			m_szTextBuffer.truncate(m_iMaxBufferSize);
			m_p->bTextBlocksDirty = true;

			int iPos = 0;
			while((iPos < ((int)(m_szTextBuffer.length()))) && (m_szTextBuffer[iPos] < 33))
				iPos++;
			if((iPos < ((int)(m_szTextBuffer.length()))) && (m_szTextBuffer[iPos] == QChar('/')))
				m_szTextBuffer.insert(iPos, "\\");

			returnPressed(iIdx != -1);
		}
	}
}

int KviInputEditor::replaceSegment(int iStart, int iLength, const QString & szText)
{
	addUndo(new EditCommand(EditCommand::InsertText, m_szTextBuffer.mid(iStart, iLength), iStart));
	m_szTextBuffer.remove(iStart, iLength);
	addUndo(new EditCommand(EditCommand::InsertText, szText, iStart));
	m_szTextBuffer.insert(iStart, szText);
	m_szTextBuffer.truncate(m_iMaxBufferSize);
	m_p->bTextBlocksDirty = true;
	repaintWithCursorOn();

	int iInsertedLength = szText.length();
	int iMaxInsertedLength = m_iMaxBufferSize - iStart;
	if(iInsertedLength > iMaxInsertedLength)
		return iMaxInsertedLength;
	return iInsertedLength;
}

void KviInputEditor::pasteClipboardWithConfirmation()
{
	QClipboard * pClip = QApplication::clipboard();
	if(!pClip)
		return;
	QString szText = pClip->text(QClipboard::Clipboard);

	if(KVS_TRIGGER_EVENT_2_HALTED(KviEvent_OnInputBarPaste, m_pKviWindow, m_pKviWindow->id(), szText))
		return;

	if(szText.contains(QChar('\n')))
	{
		if(m_pInputParent->inherits("KviInput"))
			((KviInput *)(m_pInputParent))->multiLinePaste(szText);
	}
	else
	{
		insertText(szText);
	}
}

void KviInputEditor::pasteSelectionWithConfirmation()
{
	QClipboard * pClip = QApplication::clipboard();
	if(!pClip)
		return;
	QString szText = pClip->text(pClip->supportsSelection() ? QClipboard::Selection : QClipboard::Clipboard);

	if(KVS_TRIGGER_EVENT_2_HALTED(KviEvent_OnInputBarPaste, m_pKviWindow, m_pKviWindow->id(), szText))
		return;

	if(szText.contains(QChar('\n')))
	{
		if(m_pInputParent->inherits("KviInput"))
			((KviInput *)(m_pInputParent))->multiLinePaste(szText);
	}
	else
	{
		insertText(szText);
	}
}

void KviInputEditor::pasteSlow()
{
	KviKvsScript::run("spaste.clipboard", g_pActiveWindow);
	m_bSpSlowFlag = true;
}

void KviInputEditor::stopPasteSlow()
{
	KviKvsScript::run("spaste.stop", g_pActiveWindow);
	m_bSpSlowFlag = false;
}

void KviInputEditor::pasteFile()
{
	QString szTmp = QFileDialog::getOpenFileName(this, "Select a File - KVIrc", "", ""); //this is window title
	if(!szTmp.isEmpty())
	{
		KviQString::escapeKvs(&szTmp, KviQString::EscapeSpace);
		szTmp.prepend("spaste.file ");
		KviKvsScript::run(szTmp, g_pActiveWindow);
		m_bSpSlowFlag = true;
	}
}

void KviInputEditor::selectAll()
{
	if(m_szTextBuffer.length() > 0)
	{
		m_iSelectionBegin = 0;
		m_iSelectionEnd = m_szTextBuffer.length();
		m_p->bTextBlocksDirty = true;
	}
	end();
}

void KviInputEditor::clear()
{
	addUndo(new EditCommand(EditCommand::RemoveText, m_szTextBuffer, 0));
	m_szTextBuffer = "";
	m_p->bTextBlocksDirty = true;
	clearSelection();
	home();
}

void KviInputEditor::setText(const QString szText)
{
	addUndo(new EditCommand(EditCommand::RemoveText, m_szTextBuffer, 0));
	addUndo(new EditCommand(EditCommand::RemoveText, szText, 0));
	m_szTextBuffer = szText;
	m_szTextBuffer.truncate(m_iMaxBufferSize);
	m_p->bTextBlocksDirty = true;
	clearSelection();
	end();
}

void KviInputEditor::mouseReleaseEvent(QMouseEvent *)
{
	if(m_iDragTimer)
	{
		m_iSelectionAnchorChar = -1;
		//releaseMouse();
		killDragTimer();
	}
	if(hasSelection())
		copyToSelection();
}

void KviInputEditor::killDragTimer()
{
	if(!m_iDragTimer)
		return;

	killTimer(m_iDragTimer);
	m_iDragTimer = 0;
}

void KviInputEditor::timerEvent(QTimerEvent * e)
{
	if(e->timerId() == m_iCursorTimer)
	{
		if(!hasFocus() || !isVisible())
		{
			killTimer(m_iCursorTimer);
			m_iCursorTimer = 0;
			m_bCursorOn = false;
		}
		else
			m_bCursorOn = !m_bCursorOn;
		update();
	}
	else
	{
		//Drag timer
		handleDragSelection();
	}
}

void KviInputEditor::handleDragSelection()
{
	if(m_iSelectionAnchorChar == -1)
		return;

	if(m_iSelectionAnchorChar > m_szTextBuffer.length()) // may happen if hitting backspace or del while dragging
		m_iSelectionAnchorChar = m_szTextBuffer.length();

	QPoint pnt = mapFromGlobal(QCursor::pos());

	m_iCursorPosition = std::min(charIndexFromXPosition(pnt.x()), m_szTextBuffer.length());

	if(m_iCursorPosition == m_iSelectionAnchorChar)
		clearSelection();
	else
	{
		if(m_iCursorPosition > m_iSelectionAnchorChar)
		{
			m_iSelectionBegin = m_iSelectionAnchorChar;
			m_iSelectionEnd = m_iCursorPosition;
		}
		else
		{
			m_iSelectionBegin = m_iCursorPosition;
			m_iSelectionEnd = m_iSelectionAnchorChar;
		}
	}

	ensureCursorVisible();
	m_p->bTextBlocksDirty = true;
	repaintWithCursorOn();
}

void KviInputEditor::finishInput()
{
	if(!m_szTextBuffer.isEmpty())
	{
		if(m_pInputParent->inherits("KviInput"))
			KviInputHistory::instance()->add(m_szTextBuffer);

		addToHistory(m_szTextBuffer);
	}

	//ensure the color window is hidden (bug #835)
	if(g_pColorWindow && g_pColorWindow->isVisible())
		g_pColorWindow->hide();

	m_iCurHistoryIdx = -1;
}

void KviInputEditor::returnPressed(bool)
{
	finishInput();

	emit enterPressed();
}

void KviInputEditor::clearUndoStack()
{
	m_UndoStack.clear();
	m_RedoStack.clear();
}

void KviInputEditor::focusInEvent(QFocusEvent * e)
{
	if(!m_iCursorTimer)
	{
		m_iCursorTimer = startTimer(KVI_INPUT_BLINK_TIME);
		m_bCursorOn = true;
		update();
	}
	e->accept();
}

void KviInputEditor::focusOutEvent(QFocusEvent * e)
{
	if(m_iCursorTimer)
		killTimer(m_iCursorTimer);
	m_iCursorTimer = 0;
	m_bCursorOn = false;
	update();
	e->accept();
}

void KviInputEditor::internalCursorRight(bool bShift)
{
	if(m_iCursorPosition >= ((int)(m_szTextBuffer.length())))
	{
		clearSelection();
		ensureCursorVisible();
		return;
	}

	//Grow the selection if needed
	if(bShift)
	{
		if((m_iSelectionBegin != -1) && (m_iSelectionEnd != -1))
		{
			if(m_iSelectionEnd <= m_iCursorPosition)
				m_iSelectionEnd = m_iCursorPosition + 1;
			else if(m_iSelectionBegin >= m_iCursorPosition)
				m_iSelectionBegin = m_iCursorPosition + 1;
			else
			{
				m_iSelectionBegin = m_iCursorPosition;
				m_iSelectionEnd = m_iCursorPosition + 1;
			}
		}
		else
		{
			m_iSelectionBegin = m_iCursorPosition;
			m_iSelectionEnd = m_iCursorPosition + 1;
		}
		m_iCursorPosition++;
	}
	else
	{
		m_iCursorPosition++;
		clearSelection();
	}

	m_p->bTextBlocksDirty = true;
	ensureCursorVisible();
}

void KviInputEditor::internalCursorLeft(bool bShift)
{
	if(m_iCursorPosition <= 0)
	{
		clearSelection();
		ensureCursorVisible();
		return;
	}

	if(bShift)
	{
		if((m_iSelectionBegin != -1) && (m_iSelectionEnd != -1))
		{
			if(m_iSelectionBegin >= m_iCursorPosition)
				m_iSelectionBegin = m_iCursorPosition - 1;
			else if(m_iSelectionEnd <= m_iCursorPosition)
				m_iSelectionEnd = m_iCursorPosition - 1;
			else
			{
				m_iSelectionEnd = m_iCursorPosition;
				m_iSelectionBegin = m_iCursorPosition - 1;
			}
		}
		else
		{
			m_iSelectionEnd = m_iCursorPosition;
			m_iSelectionBegin = m_iCursorPosition - 1;
		}
		m_iCursorPosition--;
	}
	else
	{
		m_iCursorPosition--;
		clearSelection();
	}

	m_p->bTextBlocksDirty = true;
	ensureCursorVisible();
}

QVariant KviInputEditor::inputMethodQuery(Qt::InputMethodQuery query) const
{
	switch(query)
	{
		case Qt::ImEnabled:
			return QVariant(true);
			break;
		case Qt::ImHints:
			return QVariant(0);
			break;
		default:
			// fall down
			break;
	}

	return QWidget::inputMethodQuery(query);
}

void KviInputEditor::inputMethodEvent(QInputMethodEvent * e)
{
	if(m_bReadOnly)
	{
		e->ignore();
		return;
	}

	if(!m_bIMComposing)
	{
		removeSelected();
		m_iIMStart = m_iIMSelectionBegin = m_iCursorPosition;
		m_iIMLength = 0;
		m_bIMComposing = true;
	}

	m_bUpdatesEnabled = false;

	m_iIMLength = replaceSegment(m_iIMStart, m_iIMLength, e->commitString());

	// update selection inside the pre-edit
	m_iIMSelectionBegin = m_iIMStart + e->replacementStart();
	m_iIMSelectionLength = e->replacementLength();
	moveCursorTo(m_iIMSelectionBegin);

	if(e->commitString().isEmpty())
	{
		if(e->preeditString().isEmpty())
		{
			m_bIMComposing = false;
			m_iIMStart = 0;
			m_iIMLength = 0;
		}
		else
		{
			// replace the preedit area with the IM result text
			m_iIMLength = replaceSegment(m_iIMStart, m_iIMLength, e->preeditString());
			// move cursor to after the IM result text
			moveCursorTo(m_iIMStart + m_iIMLength);
		}
	}
	else
	{
		// replace the preedit area with the IM result text
		m_iIMLength = replaceSegment(m_iIMStart, m_iIMLength, e->commitString());
		// move cursor to after the IM result text
		moveCursorTo(m_iIMStart + m_iIMLength);
		// reset data
		m_bIMComposing = false;
		m_iIMStart = 0;
		m_iIMLength = 0;
	}

	// repaint
	m_bUpdatesEnabled = true;

	repaintWithCursorOn();
}

void KviInputEditor::installShortcuts()
{
	KviShortcut::create(KVI_SHORTCUTS_INPUT_HISTORY, this, SLOT(openHistory()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_HISTORY_PREV, this, SLOT(historyPrev()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_HISTORY_NEXT, this, SLOT(historyNext()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_PREV_CHAR, this, SLOT(previousChar()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_NEXT_CHAR, this, SLOT(nextChar()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_PREV_CHAR_SELECT, this, SLOT(previousCharSelection()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_NEXT_CHAR_SELECT, this, SLOT(nextCharSelection()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_PREV_WORD, this, SLOT(previousWord()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_NEXT_WORD, this, SLOT(nextWord()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_PREV_WORD_SELECT, this, SLOT(previousWordSelection()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_NEXT_WORD_SELECT, this, SLOT(nextWordSelection()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_BOLD, this, SLOT(insertBold()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_ITALIC, this, SLOT(insertItalic()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_RESET, this, SLOT(insertReset()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_UNDERLINE, this, SLOT(insertUnderline()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_REVERSE, this, SLOT(insertReverse()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_PLAINTEXT, this, SLOT(insertPlainText()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_ICON, this, SLOT(insertIcon()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_COLOR, this, SLOT(insertColor()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_COPY, this, SLOT(copyInternal()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_CUT, this, SLOT(cutInternal()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_PASTE, this, SLOT(pasteInternal()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_UNDO, this, SLOT(undoInternal()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_REDO, this, SLOT(redoInternal()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_SELECT_ALL, this, SLOT(selectAllInternal()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_DELETE_PREV_WORD, this, SLOT(deletePreviousWord()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_DELETE_NEXT_WORD, this, SLOT(deleteNextWord()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_WIN_PREV_LINE, this, SLOT(previousLine()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_WIN_NEXT_LINE, this, SLOT(nextLine()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_WIN_PREV_PAGE, this, SLOT(previousPage()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_WIN_NEXT_PAGE, this, SLOT(nextPage()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_WIN_HOME, this, SLOT(scrollTop()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_WIN_END, this, SLOT(scrollBottom()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_WIN_SEARCH, this, SLOT(search()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_WIN_SCROLL_TO_LAST_READ_LINE, this, SLOT(scrollToLastReadLine()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_SEND_PLAIN, this, SLOT(sendPlain()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_SEND_PLAIN_2, this, SLOT(sendPlain()), nullptr, Qt::WidgetShortcut);
	//KviShortcut::create(KVI_SHORTCUTS_INPUT_SEND_KVS,this,SLOT(sendKvs()),nullptr,Qt::WidgetShortcut);
	//KviShortcut::create(KVI_SHORTCUTS_INPUT_SEND_KVS_2,this,SLOT(sendKvs()),nullptr,Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_HOME, this, SLOT(homeInternal()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_END, this, SLOT(endInternal()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_HOME_SELECT, this, SLOT(homeInternalSelection()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_END_SELECT, this, SLOT(endInternalSelection()), nullptr, Qt::WidgetShortcut);
	//KviShortcut::create(KVI_SHORTCUTS_INPUT_RETURN,this,SLOT(returnHit()),nullptr,Qt::WidgetShortcut);
	//KviShortcut::create(KVI_SHORTCUTS_INPUT_RETURN_2,this,SLOT(returnHit()),nullptr,Qt::WidgetShortcut);
	//KviShortcut::create(KVI_SHORTCUTS_INPUT_BACKSPACE,this,SLOT(backspaceHit()),nullptr,Qt::WidgetShortcut);
	//KviShortcut::create(KVI_SHORTCUTS_INPUT_BACKSPACE_2,this,SLOT(backspaceHit()),nullptr,Qt::WidgetShortcut);
	//KviShortcut::create(KVI_SHORTCUTS_INPUT_DELETE,this,SLOT(deleteHit()),nullptr,Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_ESCAPE, this, SLOT(escapeHit()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_COMMANDLINE, this, SLOT(toggleCommandMode()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_DUMMY, this, SLOT(dummy()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_WIN_ZOOM_IN, this, SLOT(zoomIn()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_WIN_ZOOM_OUT, this, SLOT(zoomOut()), nullptr, Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_CORRECT_SPELLING, this, SLOT(showSpellCheckerCorrectionsPopup()), nullptr, Qt::WidgetShortcut);
	// this is currently ambiguous, since we're using it for scripting, too
	KviShortcut::create(KVI_SHORTCUTS_WIN_ZOOM_DEFAULT, this, SLOT(zoomDefault()), SLOT(zoomDefault()), Qt::WidgetShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_MENU, this, SLOT(showContextPopupHere()), nullptr, Qt::WidgetShortcut);
}

void KviInputEditor::zoomIn()
{
	if(m_pKviWindow && m_pKviWindow->lastClickedView())
		m_pKviWindow->lastClickedView()->increaseFontSize();
}

void KviInputEditor::zoomOut()
{
	if(m_pKviWindow && m_pKviWindow->lastClickedView())
		m_pKviWindow->lastClickedView()->decreaseFontSize();
}

void KviInputEditor::zoomDefault()
{
	if(m_pKviWindow && m_pKviWindow->lastClickedView())
		m_pKviWindow->lastClickedView()->resetDefaultFont();
}

void KviInputEditor::keyPressEvent(QKeyEvent * e)
{
	// disable the keyPress handling when IM is in composition.
	if(m_bIMComposing)
	{
		e->ignore();
		return;
	}

	// completion thingies
	if(!m_bReadOnly)
	{
		if((e->key() == Qt::Key_Tab) || (e->key() == Qt::Key_Backtab))
		{
			completion(e->modifiers() & Qt::ShiftModifier);
			return;
		}

		m_bLastCompletionFinished = true;
	}

	switch(e->key())
	{
		case Qt::Key_Backspace:
			if(!m_bReadOnly)
				backspaceHit();
			return;
			break;
		case Qt::Key_Delete:
			if(!m_bReadOnly)
				deleteHit();
			return;
			break;
		case Qt::Key_Return:
		case Qt::Key_Enter:
			if(!m_bReadOnly)
				returnHit();
			return;
			break;
	}

	if(e->modifiers() & Qt::ControlModifier)
	{
		switch(e->key())
		{
			case Qt::Key_J:
			{
				break; //avoid Ctrl+J from inserting a linefeed
			}
			default:
				if(!m_bReadOnly)
					insertText(e->text());
				break;
		}
		return;
	}
#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
	if((e->modifiers() & Qt::AltModifier) && (e->modifiers() & Qt::KeypadModifier))
	{
		// Qt::Key_Meta seems to substitute Qt::Key_Alt on some keyboards
		if((e->key() == Qt::Key_Alt) || (e->key() == Qt::Key_Meta))
		{
			m_szAltKeyCode = "";
			return;
		}
		else if((e->text().unicode()->toLatin1() >= '0') && (e->text().unicode()->toLatin1() <= '9'))
		{
			m_szAltKeyCode += e->text().unicode()->toLatin1();
			return;
		}
		else if((e->key() >= Qt::Key_0) && (e->key() <= Qt::Key_9))
		{
			m_szAltKeyCode += e->key();
			return;
		}

		if(!m_bReadOnly)
			insertText(e->text());

		return;
	}
#endif
	if(!e->text().isEmpty() && !m_bReadOnly)
		insertText(e->text());
}

void KviInputEditor::keyReleaseEvent(QKeyEvent * e)
{
#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
	if((e->key() == Qt::Key_Alt) || (e->key() == Qt::Key_Meta))
	{
		if(m_szAltKeyCode.hasData())
		{
			bool bOk;
			unsigned short uCh = m_szAltKeyCode.toUShort(&bOk);
			if(bOk && uCh != 0)
			{
				insertChar(QChar(uCh));
				e->accept();
			}
		}
		m_szAltKeyCode = "";
	}
#endif
	e->ignore();
}

QString KviInputEditor::textBeforeCursor() const
{
	if(m_szTextBuffer.isEmpty() || m_iCursorPosition <= 0)
		return {};

	return m_szTextBuffer.left(m_iCursorPosition);
}

void KviInputEditor::getWordBeforeCursor(QString & szBuffer, bool * bIsFirstWordInLine)
{
	if(m_szTextBuffer.isEmpty() || m_iCursorPosition <= 0)
	{
		szBuffer = "";
		return;
	}

	szBuffer = m_szTextBuffer.left(m_iCursorPosition);

	int iIdx = szBuffer.lastIndexOf(' ');
	int iIdx2 = szBuffer.lastIndexOf(','); // This is for comma separated lists...
	int iIdx3 = szBuffer.lastIndexOf('(');
	int iIdx4 = szBuffer.lastIndexOf('"');

	iIdx = std::max({ iIdx, iIdx2, iIdx3, iIdx4 });

	*bIsFirstWordInLine = false;
	if(iIdx > -1)
		szBuffer.remove(0, iIdx + 1);
	else
		*bIsFirstWordInLine = true;
}

void KviInputEditor::completionEscapeUnsafeToken(QString & szToken)
{
	if(!((KviInput *)(m_pInputParent))->isUserFriendly())
		KviQString::escapeKvs(&szToken, KviQString::EscapeSpace);
}

void KviInputEditor::completion(bool bShift)
{
	// FIXME: Spaces in directory completion can mess everything completely
	//        On windows the characters are breaking everything...
	//        Well.... :D

	QString szWord;
	bool bFirstWordInLine;

	bool bInCommand = m_szTextBuffer.trimmed().indexOf('/') == 0;

	getWordBeforeCursor(szWord, &bFirstWordInLine);

	if(szWord.isEmpty())
	{
		if(m_szLastCompletedNick.isEmpty())
			return; // nothing to complete

		// this is standard nick completion continued
		standardNickCompletion(bShift, szWord, bFirstWordInLine, bInCommand);
		repaintWithCursorOn();
		return;
	}

	int iOffset{0};
	if(KviQString::equalCI(m_szTextBuffer.left(5), "/help"))
		iOffset = 1;

	std::vector<QString> tmp;

	bool bIsCommand = false;
	bool bIsFunction = false;
	bool bIsDir = false;
	bool bIsNick = false;

	QString szMatch;

	unsigned short uc = szWord[0].unicode();

	if(uc == '/' || iOffset)
	{
		if(szWord[1 - iOffset].unicode() == '$')
		{
			// function/identifier completion
			szWord.remove(0, 2 - iOffset);
			if(szWord.isEmpty())
				return;
			KviKvsKernel::instance()->completeFunction(szWord, tmp);
			// function names don't need to be escaped
			bIsFunction = true;
		}
		else if(bFirstWordInLine || iOffset)
		{
			// command completion
			szWord.remove(0, 1 - iOffset);
			if(szWord.isEmpty())
				return;
			KviKvsKernel::instance()->completeCommand(szWord, tmp);
			// commands don't need to be escaped
			bIsCommand = true;
		}
		else
		{
			// directory completion attempt
			g_pApp->completeDirectory(szWord, tmp);
			bIsDir = true;
		}
	}
	else if(uc == '$')
	{
		// function/identifier completion
		szWord.remove(0, 1);
		if(szWord.isEmpty())
			return;
		KviKvsKernel::instance()->completeFunction(szWord, tmp);
		bIsFunction = true;
	}
	else if(uc == '#' || uc == '&' || uc == '!')
	{
		if(m_pKviWindow)
		{
			if((szWord.length() == 1) && (m_pKviWindow->windowName()[0].unicode() == uc))
			{
				szMatch = m_pKviWindow->windowName();
				completionEscapeUnsafeToken(szMatch);
				szMatch.append(" ");
				replaceWordBeforeCursor(szWord, szMatch, false);
				repaintWithCursorOn();
				return;
			}
			if(m_pKviWindow->console())
				m_pKviWindow->console()->completeChannel(szWord, tmp);
		}
	}
	else if(KviQString::equalCIN(szWord, "irc.", 4))
	{
		//FIXME: Complete also on irc:// starting strings, not only irc.?

		// irc server name
		if(m_pKviWindow && m_pKviWindow->console())
			m_pKviWindow->console()->completeServer(szWord, tmp);
	}
	else
	{
		// empty word will end up here
		if(m_pUserListView)
		{
			if(KVI_OPTION_BOOL(KviOption_boolZshLikeNickCompletion))
			{
				if(m_szLastCompletedNick.isEmpty())
				{
					//first round of zsh completion
					m_pUserListView->completeNickBashLike(szWord, tmp, bShift);
					bIsNick = true;
					m_szLastCompletedNick = szWord;
				}
				else
				{
					standardNickCompletion(bShift, szWord, bFirstWordInLine, bInCommand);
					repaintWithCursorOn();
					return;
				}
			}
			else if(KVI_OPTION_BOOL(KviOption_boolBashLikeNickCompletion))
			{
				m_pUserListView->completeNickBashLike(szWord, tmp, bShift);
				bIsNick = true;
			}
			else
			{
				standardNickCompletion(bShift, szWord, bFirstWordInLine, bInCommand);
				repaintWithCursorOn();
				return;
			}
		}
	}

	// Lookup the longest exact match
	if(tmp.size() > 0)
	{
		if(tmp.size() == 1)
		{
			szMatch = tmp.front();
			if(szMatch.left(1) == '$' && szMatch != '$')
				szMatch.remove(0, 1);
			if(bIsCommand && !iOffset && szMatch.right(1) != '.')
				szMatch.append(' ');
			else if(bIsFunction && !iOffset && szMatch.right(1) != '.')
				szMatch.append('(');
			else if(bIsNick)
			{
				if(!KVI_OPTION_STRING(KviOption_stringNickCompletionPostfix).isEmpty())
				{
					if(bFirstWordInLine || (!KVI_OPTION_BOOL(KviOption_boolUseNickCompletionPostfixForFirstWordOnly)))
						szMatch.append(KVI_OPTION_STRING(KviOption_stringNickCompletionPostfix));
				}
			}

			if(bInCommand && !bIsCommand && !bIsFunction)
				completionEscapeUnsafeToken(szMatch); // escape crazy things like Nick\nquit
		}
		else
		{
			QString szAll;
			szMatch = tmp.front();
			auto predicate =
				[bIsDir](const QChar & a, const QChar & b)
				{
					return bIsDir ? (a.unicode() == b.unicode()) : (a.toLower().unicode() == b.toLower().unicode());
				};

			for(auto szTmp : tmp)
			{
				szMatch.truncate(std::mismatch(szMatch.data(), szMatch.data() + std::min(szMatch.size(), szTmp.size()), szTmp.data(), predicate).first - szMatch.data());

				if(!szAll.isEmpty())
					szAll.append(", ");
				szAll.append(szTmp);
			}

			if(m_pKviWindow)
				m_pKviWindow->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("%d matches: %Q"), tmp.size(), &szAll);

			if(szMatch.left(1) == '$')
				szMatch.remove(0, 1);
		}
	}
	else if(m_pKviWindow)
		m_pKviWindow->outputNoFmt(KVI_OUT_SYSTEMMESSAGE, __tr2qs("No matches"));

	if(!szMatch.isEmpty())
		replaceWordBeforeCursor(szWord, szMatch, false);

	repaintWithCursorOn();
}

void KviInputEditor::replaceWordBeforeCursor(const QString & szWord, const QString & szReplacement, bool bRepaint)
{
	clearSelection();
	m_iCursorPosition -= szWord.length();
	m_szTextBuffer.remove(m_iCursorPosition, szWord.length());
	m_szTextBuffer.insert(m_iCursorPosition, szReplacement);
	m_szTextBuffer.truncate(m_iMaxBufferSize);
	m_p->bTextBlocksDirty = true;
	moveCursorTo(m_iCursorPosition + szReplacement.length());
	if(bRepaint)
		repaintWithCursorOn();
}

void KviInputEditor::standardNickCompletionInsertCompletedText(const QString & szReplacedWord, QString szCompletedText, bool bFirstWordInLine, bool bInCommand)
{
	if(!KVI_OPTION_STRING(KviOption_stringNickCompletionPostfix).isEmpty())
	{
		if(bFirstWordInLine || (!KVI_OPTION_BOOL(KviOption_boolUseNickCompletionPostfixForFirstWordOnly)))
			szCompletedText.append(KVI_OPTION_STRING(KviOption_stringNickCompletionPostfix));
	}

	if(bInCommand)
		completionEscapeUnsafeToken(szCompletedText); // escape crazy things like Nick\nquit

	replaceWordBeforeCursor(szReplacedWord, szCompletedText, false);
}

void KviInputEditor::standardNickCompletion(bool bAddMask, QString & szWord, bool bFirstWordInLine, bool bInCommand)
{
	// FIXME: this could be really simplified...
	if(!m_pUserListView)
		return;

	clearSelection();

	QString szBuffer;
	if(m_szLastCompletedNick.isEmpty())
	{
		// New completion session: we NEED sth to complete
		if(szWord.isEmpty())
			return;

		if(m_pUserListView->completeNickStandard(szWord, m_szLastCompletedNick, szBuffer, bAddMask))
		{
			// completed: save the buffer
			m_szLastCompletionBuffer = m_szTextBuffer;
			m_iLastCompletionCursorPosition = m_iCursorPosition;
			m_szLastCompletedNick = szBuffer;
			standardNickCompletionInsertCompletedText(szWord, szBuffer, bFirstWordInLine, bInCommand);
			m_bLastCompletionFinished = false;
			// REPAINT CALLED FROM OUTSIDE!
		} // else no match at all

		return;
	}

	if(!m_bLastCompletionFinished)
	{
		// Old session
		// swap the buffers
		m_szTextBuffer = m_szLastCompletionBuffer;
		m_iCursorPosition = m_iLastCompletionCursorPosition;
		m_p->bTextBlocksDirty = true;
		// re-extract
		//word = m_szTextBuffer.left(m_iCursorPosition);

		getWordBeforeCursor(szWord, &bFirstWordInLine);
		if(szWord.isEmpty())
			return;

		if(m_pUserListView->completeNickStandard(szWord, m_szLastCompletedNick, szBuffer, bAddMask))
		{
			// completed
			m_szLastCompletedNick = szBuffer;
			standardNickCompletionInsertCompletedText(szWord, szBuffer, bFirstWordInLine, bInCommand);
			m_bLastCompletionFinished = false;
			// REPAINT CALLED FROM OUTSIDE!
		}
		else
		{
			m_bLastCompletionFinished = true;
			m_szLastCompletedNick = "";
		}

		return;
	}

	// Old session finished
	// re-extract
	//word = m_szTextBuffer.left(m_iCursorPosition);
	//getWordBeforeCursor(word,&bFirstWordInLine);
	if(szWord.isEmpty())
		return;
	if(m_pUserListView->completeNickStandard(szWord, "", szBuffer, bAddMask))
	{
		// completed
		m_szLastCompletionBuffer = m_szTextBuffer;
		m_iLastCompletionCursorPosition = m_iCursorPosition;
		m_szLastCompletedNick = szBuffer;
		standardNickCompletionInsertCompletedText(szWord, szBuffer, bFirstWordInLine, bInCommand);
		m_bLastCompletionFinished = false;
		// REPAINT CALLED FROM OUTSIDE!
	}
	else
	{
		m_bLastCompletionFinished = true;
		m_szLastCompletedNick = "";
	}
}

//Funky helpers
void KviInputEditor::end()
{
	m_iCursorPosition = m_szTextBuffer.length();
	ensureCursorVisible();
	repaintWithCursorOn();
}

void KviInputEditor::home()
{
	m_iCursorPosition = 0;
	ensureCursorVisible();
	repaintWithCursorOn();
}

void KviInputEditor::insertChar(QChar c)
{
	if(m_szTextBuffer.length() >= m_iMaxBufferSize)
		return;

	// Kill the selection
	if((m_iSelectionBegin > -1) || (m_iSelectionEnd > -1))
	{
		if((m_iCursorPosition >= m_iSelectionBegin) && (m_iCursorPosition <= m_iSelectionEnd))
		{
			m_bUpdatesEnabled = false;
			removeSelected();
			m_bUpdatesEnabled = true;
		}
	}
	clearSelection();
	m_szTextBuffer.insert(m_iCursorPosition, c);
	m_p->bTextBlocksDirty = true;

	addUndo(new EditCommand(EditCommand::InsertText, c, m_iCursorPosition));

	m_iCursorPosition++;
	ensureCursorVisible();
	repaintWithCursorOn();
}

void KviInputEditor::repaintWithCursorOn()
{
	if(!m_bUpdatesEnabled)
		return;

	m_bCursorOn = true;
	update();
}

int KviInputEditor::charIndexFromXPosition(qreal fXPos)
{
	if(m_p->bTextBlocksDirty)
		rebuildTextBlocks();

	fXPos -= KVI_INPUT_MARGIN;

	qreal fCurX = -m_p->fXOffset;
	int iCurChar = 0;

	if(m_p->lTextBlocks.isEmpty())
		return 0;

	if(fXPos <= fCurX)
		return 0;

	KviInputEditorTextBlock * pBlock = nullptr;

	foreach(pBlock, m_p->lTextBlocks)
	{
		qreal fNextX = fCurX + pBlock->fWidth;
		if(fNextX >= fXPos)
			break;
		fCurX = fNextX;
		iCurChar += pBlock->iLength;
	}

	if(!pBlock)
		return iCurChar;

	// This is very tricky. Qt does not provide a simple means to figure out the cursor position
	// from an x position on the text. We use QFontMetrics::elidedText() to guess it.

	// Additionally Qt::ElideNone does not work as expected (see QTBUG-40315): it just ignores clipping altogether.
	// So we use Qt::ElideRight here but we must take into account the width of the elision

	qreal fWidth = fXPos - fCurX;

	QFontMetricsF * fm = getLastFontMetrics(font());

	QString szPart = fm->elidedText(pBlock->szText, Qt::ElideRight, fWidth + m_p->fFontElisionWidth);

	if(szPart.endsWith(m_p->szFontElision))
		szPart.truncate(szPart.length() - 1); // kill the elision

	// OK, now we have a good starting point

	qreal fPrevWidth = fm->width(szPart);
	int iBlockLength = pBlock->szText.length();

	if(fPrevWidth <= fWidth)
	{
		// move up adding characters
		for(;;)
		{
			int iPartLength = szPart.length();
			if(iPartLength == iBlockLength)
				return iCurChar + iBlockLength;

			szPart = pBlock->szText.left(iPartLength + 1);

			qreal fNextWidth = fm->width(szPart);

			if(fNextWidth >= fWidth)
			{
				// gotcha.
				qreal fMiddle = (fPrevWidth + fNextWidth) / 2.0;

				if(fWidth < fMiddle)
					return iCurChar + iPartLength;

				return iCurChar + iPartLength + 1;
			}

			fPrevWidth = fNextWidth;
		}
	}
	else
	{
		// move down removing characters
		for(;;)
		{
			int iPartLength = szPart.length();
			if(iPartLength == 0)
				return iCurChar;

			szPart = pBlock->szText.left(iPartLength - 1);

			qreal fNextWidth = fm->width(szPart);

			if(fNextWidth <= fWidth)
			{
				// gotcha.
				qreal fMiddle = (fPrevWidth + fNextWidth) / 2.0;

				if(fWidth < fMiddle)
					return iCurChar + iPartLength - 1;

				return iCurChar + iPartLength;
			}

			fPrevWidth = fNextWidth;
		}
	}

	Q_ASSERT(false); // not reached
}

qreal KviInputEditor::xPositionFromCharIndex(int iChIdx)
{
	if(m_p->bTextBlocksDirty)
		rebuildTextBlocks();

	qreal fCurX = -m_p->fXOffset + KVI_INPUT_MARGIN;

	if(m_p->lTextBlocks.isEmpty())
		return fCurX;

	QFontMetricsF * fm = getLastFontMetrics(font());
	int iCurChar = 0;

	foreach(KviInputEditorTextBlock * pBlock, m_p->lTextBlocks)
	{
		int iNextChar = iCurChar + pBlock->iLength;
		if(iNextChar > iChIdx)
		{
			if(iChIdx == iCurChar)
				return fCurX;
			return fCurX + fm->width(pBlock->szText.left(iChIdx - iCurChar));
		}
		iCurChar = iNextChar;
		fCurX += pBlock->fWidth;
	}

	return fCurX;
}

void KviInputEditor::undo()
{
	if(!isUndoAvailable())
		return;

	if(m_UndoStack.empty())
		return; // this should be ensured by isUndoAvailable() but well...

	EditCommand * pCommand = m_UndoStack.back();
	m_UndoStack.pop_back();

	Q_ASSERT(pCommand); // should be true: we delete the empty undo stack

	m_iSelectionBegin = -1;
	m_iSelectionEnd = -1;

	switch(pCommand->type())
	{
		case EditCommand::InsertText:
			m_szTextBuffer.remove(pCommand->startPosition(), pCommand->text().length());
			m_p->bTextBlocksDirty = true;
			moveCursorTo(pCommand->startPosition());
			break;
		case EditCommand::RemoveText:
			m_szTextBuffer.insert(pCommand->startPosition(), pCommand->text());
			m_p->bTextBlocksDirty = true;
			moveCursorTo(pCommand->startPosition() + pCommand->text().length());
			break;
		default:
			Q_ASSERT_X(false, "KviInputEditor::undo", "Unexpected EditCommand type");
			delete pCommand; // argh
			return;
			break;
	}

	m_RedoStack.push_back(pCommand);
	if(m_RedoStack.size() > KVI_INPUT_MAX_UNDO_SIZE)
		m_RedoStack.erase(m_RedoStack.begin()); // will delete it
}

void KviInputEditor::redo()
{
	if(!isRedoAvailable())
		return;

	if(m_RedoStack.empty())
		return; // this should be ensured by isUndoAvailable() but well...

	EditCommand * pCommand = m_RedoStack.back();
	m_RedoStack.pop_back();

	Q_ASSERT(pCommand); // should be true: we delete the empty redo stack

	m_iSelectionBegin = -1;
	m_iSelectionEnd = -1;

	switch(pCommand->type())
	{
		case EditCommand::InsertText:
			m_szTextBuffer.insert(pCommand->startPosition(), pCommand->text());
			m_p->bTextBlocksDirty = true;
			moveCursorTo(pCommand->startPosition() + pCommand->text().length());
			break;
		case EditCommand::RemoveText:
			m_szTextBuffer.remove(pCommand->startPosition(), pCommand->text().length());
			m_p->bTextBlocksDirty = true;
			moveCursorTo(pCommand->startPosition());
			break;
		default:
			Q_ASSERT_X(false, "KviInputEditor::redo", "Unexpected EditCommand type");
			delete pCommand; // argh
			return;
			break;
	}

	m_UndoStack.push_back(pCommand);
	if(m_UndoStack.size() > KVI_INPUT_MAX_UNDO_SIZE)
		m_UndoStack.erase(m_UndoStack.begin()); // will delete it
}

void KviInputEditor::addUndo(EditCommand * pCommand)
{
	m_UndoStack.push_back(pCommand);

	if(m_UndoStack.size() > KVI_INPUT_MAX_UNDO_SIZE)
		m_UndoStack.erase(m_UndoStack.begin()); // will delete it
}

void KviInputEditor::openHistory()
{
	if(!KVI_OPTION_BOOL(KviOption_boolEnableInputHistory))
		return;
	if(m_pInputParent->inherits("KviInput"))
		((KviInput *)(m_pInputParent))->historyButtonClicked();
}

void KviInputEditor::toggleMultiLineEditor()
{
	if(m_pInputParent->inherits("KviInput"))
	{
		((KviInput *)(m_pInputParent))->multiLinePaste(m_szTextBuffer);
		clear();
	}
}

void KviInputEditor::previousChar()
{
	internalCursorLeft(false);
	repaintWithCursorOn();
}

void KviInputEditor::nextChar()
{
	internalCursorRight(false);
	repaintWithCursorOn();
}

void KviInputEditor::previousCharSelection()
{
	if(m_iCursorPosition <= 0)
		return;

	internalCursorLeft(true);
	repaintWithCursorOn();
}

void KviInputEditor::nextCharSelection()
{
	if(m_iCursorPosition >= m_szTextBuffer.length())
		return;

	internalCursorRight(true);
	repaintWithCursorOn();
}

void KviInputEditor::previousWord()
{
	if(m_iCursorPosition <= 0)
		return;

	// skip whitespace
	while(m_iCursorPosition > 0)
	{
		if(!m_szTextBuffer.at(m_iCursorPosition - 1).isSpace())
			break;
		internalCursorLeft(false);
	}
	// skip nonwhitespace
	while(m_iCursorPosition > 0)
	{
		if(m_szTextBuffer.at(m_iCursorPosition - 1).isSpace())
			break;
		internalCursorLeft(false);
	}
	repaintWithCursorOn();
}

void KviInputEditor::nextWord()
{
	if(m_iCursorPosition >= m_szTextBuffer.length())
		return;

	// skip whitespace
	while(m_iCursorPosition < m_szTextBuffer.length())
	{
		if(!m_szTextBuffer.at(m_iCursorPosition).isSpace())
			break;
		internalCursorRight(false);
	}
	// skip nonwhitespace
	while(m_iCursorPosition < m_szTextBuffer.length())
	{
		if(m_szTextBuffer.at(m_iCursorPosition).isSpace())
			break;
		internalCursorRight(false);
	}
	repaintWithCursorOn();
}

void KviInputEditor::previousWordSelection()
{
	if(m_iCursorPosition <= 0)
		return;

	// skip whitespace
	while(m_iCursorPosition > 0)
	{
		if(!m_szTextBuffer.at(m_iCursorPosition - 1).isSpace())
			break;
		internalCursorLeft(true);
	}
	// skip nonwhitespace
	while(m_iCursorPosition > 0)
	{
		if(m_szTextBuffer.at(m_iCursorPosition - 1).isSpace())
			break;
		internalCursorLeft(true);
	}
	repaintWithCursorOn();
}

void KviInputEditor::nextWordSelection()
{
	if(m_iCursorPosition >= m_szTextBuffer.length())
		return;

	// skip whitespace
	while(m_iCursorPosition < m_szTextBuffer.length())
	{
		if(!m_szTextBuffer.at(m_iCursorPosition).isSpace())
			break;
		internalCursorRight(true);
	}
	// skip nonwhitespace
	while(m_iCursorPosition < m_szTextBuffer.length())
	{
		if(m_szTextBuffer.at(m_iCursorPosition).isSpace())
			break;
		internalCursorRight(true);
	}
	repaintWithCursorOn();
}

void KviInputEditor::insertBold()
{
	if(!m_bReadOnly)
		insertChar(KviControlCodes::Bold);
}

void KviInputEditor::insertItalic()
{
	if(!m_bReadOnly)
		insertChar(KviControlCodes::Italic);
}

void KviInputEditor::insertReset()
{
	if(!m_bReadOnly)
		insertChar(KviControlCodes::Reset);
}

void KviInputEditor::insertUnderline()
{
	if(!m_bReadOnly)
		insertChar(KviControlCodes::Underline);
}

void KviInputEditor::insertReverse()
{
	if(!m_bReadOnly)
		insertChar(KviControlCodes::Reverse);
}

void KviInputEditor::insertPlainText()
{
	if(!m_bReadOnly)
		insertChar(KviControlCodes::CryptEscape); // DO NOT CRYPT THIS STUFF
}

void KviInputEditor::insertIcon()
{
	if(m_bReadOnly)
		return;

	insertChar(KviControlCodes::Icon); // THE NEXT WORD IS AN ICON NAME

	popupTextIconWindow();
}

void KviInputEditor::popupTextIconWindow()
{
	if(m_bReadOnly)
		return;

	qreal fXPos = xPositionFromCharIndex(m_iCursorPosition);
	if(fXPos > 24)
		fXPos -= 24;
	if(!g_pTextIconWindow)
		g_pTextIconWindow = new KviTextIconWindow();

	if(fXPos + g_pTextIconWindow->width() > width())
		fXPos = width() - (g_pTextIconWindow->width() + 2);
	g_pTextIconWindow->move(mapToGlobal(QPoint(fXPos, -KVI_TEXTICON_WIN_HEIGHT)));
	g_pTextIconWindow->popup(this, false);
}

void KviInputEditor::insertIconCode(const QString & szCode)
{
	if(m_iCursorPosition == 0)
	{
		insertChar(KviControlCodes::Icon);
		insertText(szCode);
		insertChar(' ');
		return;
	}

	int idx = m_szTextBuffer.lastIndexOf(KviControlCodes::Icon, m_iCursorPosition - 1);
	int idx2 = m_szTextBuffer.lastIndexOf(QChar(' '), m_iCursorPosition - 1);
	if((idx == -1) || (idx2 > idx))
	{
		insertChar(KviControlCodes::Icon);
		insertText(szCode);
		insertChar(' ');
		return;
	}

	QString szPart = m_szTextBuffer.mid(idx + 1, m_iCursorPosition - idx + 1);

	if(!szCode.startsWith(szPart))
	{
		insertChar(KviControlCodes::Icon);
		insertText(szCode);
		insertChar(' ');
		return;
	}

	QString szMissing = szCode.mid(szPart.length());
	if(!szMissing.isEmpty())
		insertText(szMissing);
	insertChar(' ');
}

void KviInputEditor::insertColor()
{
	if(m_bReadOnly)
		return;
	insertChar(KviControlCodes::Color);
	int xPos = xPositionFromCharIndex(m_iCursorPosition);
	if(xPos > 24)
		xPos -= 24;
	if(!g_pColorWindow)
		g_pColorWindow = new KviColorWindow();
	if(xPos + g_pColorWindow->width() > width())
		xPos = width() - (g_pColorWindow->width() + 2);
	g_pColorWindow->move(mapToGlobal(QPoint(xPos, -35)));
	g_pColorWindow->popup(this);
}

void KviInputEditor::copyInternal()
{
	copyToClipboard();
}

void KviInputEditor::cutInternal()
{
	if(!m_bReadOnly)
		cut();
}

void KviInputEditor::pasteInternal()
{
	if(!m_bReadOnly)
		pasteClipboardWithConfirmation();
}

void KviInputEditor::undoInternal()
{
	if(!m_bReadOnly)
		undo();
}

void KviInputEditor::redoInternal()
{
	if(!m_bReadOnly)
		redo();
}

void KviInputEditor::selectAllInternal()
{
	m_iSelectionBegin = 0;
	m_iSelectionEnd = m_szTextBuffer.length();
	m_iCursorPosition = m_szTextBuffer.length();
	m_p->bTextBlocksDirty = true;
	repaintWithCursorOn();
}

void KviInputEditor::deletePreviousWord()
{
	if(m_bReadOnly)
		return;

	if(hasSelection())
	{
		removeSelected();
	}
	else
	{
		if(m_iCursorPosition <= 0)
			return;

		// skip whitespace
		while(m_iCursorPosition > 0)
		{
			if(!m_szTextBuffer.at(m_iCursorPosition - 1).isSpace())
				break;
			m_szTextBuffer.remove(m_iCursorPosition - 1, 1);
			m_p->bTextBlocksDirty = true;
			m_iCursorPosition--;
		}
		// skip nonwhitespace
		while(m_iCursorPosition > 0)
		{
			if(m_szTextBuffer.at(m_iCursorPosition - 1).isSpace())
				break;
			m_szTextBuffer.remove(m_iCursorPosition - 1, 1);
			m_p->bTextBlocksDirty = true;
			m_iCursorPosition--;
		}
	}
	repaintWithCursorOn();
}

void KviInputEditor::deleteNextWord()
{
	if(m_bReadOnly)
		return;

	if(hasSelection())
	{
		removeSelected();
	}
	else
	{
		if(m_iCursorPosition >= m_szTextBuffer.length())
			return;

		// skip whitespace
		while(m_iCursorPosition < m_szTextBuffer.length())
		{
			if(!m_szTextBuffer.at(m_iCursorPosition).isSpace())
				break;
			m_szTextBuffer.remove(m_iCursorPosition, 1);
			m_p->bTextBlocksDirty = true;
		}
		// skip nonwhitespace
		while(m_iCursorPosition < m_szTextBuffer.length())
		{
			if(m_szTextBuffer.at(m_iCursorPosition).isSpace())
				break;
			m_szTextBuffer.remove(m_iCursorPosition, 1);
			m_p->bTextBlocksDirty = true;
		}
	}
	repaintWithCursorOn();
}

void KviInputEditor::previousLine()
{
	if(m_pKviWindow && m_pKviWindow->lastClickedView())
		m_pKviWindow->lastClickedView()->prevLine();
	return;
}

void KviInputEditor::nextLine()
{
	if(m_pKviWindow && m_pKviWindow->lastClickedView())
		m_pKviWindow->lastClickedView()->nextLine();
	return;
}

void KviInputEditor::previousPage()
{
	if(m_pKviWindow && m_pKviWindow->lastClickedView())
		m_pKviWindow->lastClickedView()->prevPage();
}

void KviInputEditor::nextPage()
{
	if(m_pKviWindow && m_pKviWindow->lastClickedView())
		m_pKviWindow->lastClickedView()->nextPage();
}

void KviInputEditor::scrollTop()
{
	if(m_pKviWindow && m_pKviWindow->lastClickedView())
		m_pKviWindow->lastClickedView()->scrollTop();
}

void KviInputEditor::scrollBottom()
{
	if(m_pKviWindow && m_pKviWindow->lastClickedView())
		m_pKviWindow->lastClickedView()->scrollBottom();
}

void KviInputEditor::search()
{
	if(m_pKviWindow && m_pKviWindow->lastClickedView())
		m_pKviWindow->lastClickedView()->toggleToolWidget();
}

void KviInputEditor::scrollToLastReadLine()
{
	if(m_pKviWindow && m_pKviWindow->lastClickedView() && m_pKviWindow->lastClickedView()->hasLineMark())
		m_pKviWindow->lastClickedView()->scrollToMarker();
}

void KviInputEditor::sendPlain()
{
	if(!m_pInputParent->inherits("KviInput"))
		return;

	//ensure the color window is hidden (bug #835)
	if(g_pColorWindow && g_pColorWindow->isVisible())
		g_pColorWindow->hide();

	QString szBuffer(m_szTextBuffer);
	m_szTextBuffer = "";
	m_p->bTextBlocksDirty = true;
	m_iSelectionBegin = -1;
	m_iSelectionEnd = -1;
	m_iCursorPosition = 0;
	ensureCursorVisible();
	repaintWithCursorOn();
	KviUserInput::parseNonCommand(szBuffer, m_pKviWindow);
	if(!szBuffer.isEmpty())
	{
		KviInputHistory::instance()->add(szBuffer);
		addToHistory(szBuffer);
	}

	m_iCurHistoryIdx = -1;
}

void KviInputEditor::sendKvs()
{
	if(!m_pInputParent->inherits("KviInput"))
		return;

	//ensure the color window is hidden (bug #835)
	if(g_pColorWindow && g_pColorWindow->isVisible())
		g_pColorWindow->hide();

	QString szBuffer(m_szTextBuffer), szTmp(m_szTextBuffer);
	m_szTextBuffer = "";
	m_p->bTextBlocksDirty = true;
	m_iSelectionBegin = -1;
	m_iSelectionEnd = -1;
	m_iCursorPosition = 0;
	repaintWithCursorOn();

	if(szTmp.startsWith(QChar('/')))
		szTmp.remove(0, 1);
	KviUserInput::parseCommand(szTmp, m_pKviWindow, QString(), false);

	if(!szBuffer.isEmpty())
	{
		KviInputHistory::instance()->add(szBuffer);
		addToHistory(szBuffer);
	}

	m_iCurHistoryIdx = -1;
}

void KviInputEditor::clearSelection()
{
	if((m_iSelectionBegin == -1) || (m_iSelectionEnd == -1))
		return;

	m_iSelectionBegin = -1;
	m_iSelectionEnd = -1;
	m_p->bTextBlocksDirty = true;
}

void KviInputEditor::homeInternal()
{
	clearSelection();

	if(m_iCursorPosition <= 0)
		return;

	home();
}

void KviInputEditor::homeInternalSelection()
{
	if(m_iCursorPosition <= 0)
		return;

	if((m_iSelectionBegin == -1) && (m_iSelectionEnd == -1))
	{
		m_iSelectionEnd = m_iCursorPosition; // There is no selection
	}
	else
	{
		// There is selection
		if(m_iCursorPosition == m_iSelectionEnd)
			m_iSelectionEnd = m_iSelectionBegin;
	}

	m_iSelectionBegin = 0;

	m_p->bTextBlocksDirty = true;

	home();
}

void KviInputEditor::endInternal()
{
	clearSelection();
	end();
}

void KviInputEditor::endInternalSelection()
{
	if((m_iSelectionBegin == -1) && (m_iSelectionEnd == -1))
	{
		m_iSelectionBegin = m_iCursorPosition; // There is no selection
	}
	else
	{
		m_iSelectionBegin = m_iSelectionEnd; // There is no selection
	}

	m_iSelectionEnd = m_szTextBuffer.length();
	m_p->bTextBlocksDirty = true;
	end();
}

void KviInputEditor::historyPrev()
{
	if(m_bReadOnly)
		return;

	if(m_History.size() < 1)
		return;

	if(m_iCurHistoryIdx < 0)
	{
		m_szSaveTextBuffer = m_szTextBuffer;
		m_szTextBuffer = m_History[0];
		m_p->bTextBlocksDirty = true;
		m_iCurHistoryIdx = 0;
	}
	else if(m_iCurHistoryIdx >= (int)(m_History.size() - 1))
	{
		m_szTextBuffer = m_szSaveTextBuffer;
		m_p->bTextBlocksDirty = true;
		m_iCurHistoryIdx = -1;
	}
	else
	{
		m_iCurHistoryIdx++;
		m_szTextBuffer = m_History[m_iCurHistoryIdx];
		m_p->bTextBlocksDirty = true;
	}
	clearSelection();
	if(KVI_OPTION_BOOL(KviOption_boolInputHistoryCursorAtEnd))
		end();
	else
		home();
}

void KviInputEditor::historyNext()
{
	if(m_bReadOnly)
		return;

	if(m_History.size() < 1)
		return;

	if(m_iCurHistoryIdx < 0)
	{
		m_szSaveTextBuffer = m_szTextBuffer;
		m_szTextBuffer = m_History[m_History.size() - 1];
		m_iCurHistoryIdx = m_History.size() - 1;
	}
	else if(m_iCurHistoryIdx == 0)
	{
		m_szTextBuffer = m_szSaveTextBuffer;
		m_iCurHistoryIdx = -1;
	}
	else
	{
		m_iCurHistoryIdx--;
		m_szTextBuffer = m_History[m_iCurHistoryIdx];
	}
	m_p->bTextBlocksDirty = true;
	clearSelection();
	if(KVI_OPTION_BOOL(KviOption_boolInputHistoryCursorAtEnd))
		end();
	else
		home();
}

void KviInputEditor::returnHit()
{
	returnPressed();
}

void KviInputEditor::backspaceHit()
{
	if(m_bReadOnly)
		return;

	if(hasSelection() && (m_iSelectionEnd >= m_iCursorPosition) && (m_iSelectionBegin <= m_iCursorPosition))
	{
		//remove the selection
		addUndo(new EditCommand(EditCommand::RemoveText, m_szTextBuffer.mid(m_iSelectionBegin, m_iSelectionEnd - m_iSelectionBegin), m_iSelectionBegin));
		m_szTextBuffer.remove(m_iSelectionBegin, m_iSelectionEnd - m_iSelectionBegin);
		m_p->bTextBlocksDirty = true;
		m_iCursorPosition = m_iSelectionBegin;
	}
	else if(m_iCursorPosition > 0)
	{
		m_iCursorPosition--;
		addUndo(new EditCommand(EditCommand::RemoveText, m_szTextBuffer.mid(m_iCursorPosition, 1), m_iCursorPosition));
		m_szTextBuffer.remove(m_iCursorPosition, 1);
		m_p->bTextBlocksDirty = true;
	}

	clearSelection();
	ensureCursorVisible();
	repaintWithCursorOn();
}

void KviInputEditor::deleteHit()
{
	if(m_bReadOnly)
		return;

	if(hasSelection())
	{
		removeSelected();
		return;
	}

	if(m_iCursorPosition < m_szTextBuffer.length())
	{
		m_szTextBuffer.remove(m_iCursorPosition, 1);
		m_p->bTextBlocksDirty = true;
		clearSelection();
		ensureCursorVisible();
		repaintWithCursorOn();
	}
}

void KviInputEditor::escapeHit()
{
	finishInput();
	setText("");
	emit escapePressed();
}

void KviInputEditor::toggleCommandMode()
{
	if(m_pInputParent->inherits("KviInput"))
		((KviInput *)(m_pInputParent))->setUserFriendly(!((KviInput *)(m_pInputParent))->isUserFriendly());
}

void KviInputEditor::dummy()
{
} // this function does nothing. check the header file for explanation

void KviInputEditor::addToHistory(const QString &szString)
{
	if(!m_History.empty() && m_History.front() == szString)
		return;

	m_History.insert(m_History.begin(), szString);

	if(m_History.size() > KVI_INPUT_MAX_LOCAL_HISTORY_ENTRIES)
		m_History.pop_back();
}
