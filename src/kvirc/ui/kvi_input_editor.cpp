//============================================================================
//
//   File : kvi_input_editor.cpp
//   Creation date : Fri Sep 5 2008 17:26:34 by Elvio Basello
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your opinion) any later version.
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
//   This file was originally part of kvi_input.h
//============================================================================

#include "kvi_app.h"
#include "kvi_colorwin.h"
#include "kvi_console.h"
#include "kvi_fileextensions.h"
#include "kvi_frame.h"
#include "kvi_input.h"
#include "kvi_input_editor.h"
#include "kvi_input_history.h"
#include "kvi_ircview.h"
#include "kvi_kvs_script.h"
#include "kvi_kvs_kernel.h"
#include "kvi_locale.h"
#include "kvi_mirccntrl.h"
#include "kvi_options.h"
#include "kvi_out.h"
#include "kvi_tal_popupmenu.h"
#include "kvi_texticonwin.h"
#include "kvi_texticonmanager.h"
#include "kvi_userinput.h"
#include "kvi_userlistview.h"
#include "kvi_shortcuts.h"

#include <QClipboard>
#include <QLabel>
#include <QUrl>
#include <QStyle>
#include <QStyleOption>
#include <QPainter>
#include <QPixmap>
#include <QFileDialog>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QDragEnterEvent>
#include <QInputContext>
#include <QShortcut>

// from kvi_app.cpp
extern KviTalPopupMenu         * g_pInputPopup;
extern KviTextIconWindow       * g_pTextIconWindow;
extern KviColorWindow          * g_pColorWindow;

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	extern QPixmap       * g_pShadedChildGlobalDesktopBackground;
#endif

//static members initialization
int            KviInputEditor::g_iInputInstances = 0;
int            KviInputEditor::g_iInputFontCharWidth[256];
QFontMetrics * KviInputEditor::g_pLastFontMetrics = 0;

KviInputEditor::KviInputEditor(QWidget * pPar, KviWindow * pWnd, KviUserListView * pView)
	: QWidget(pPar)
{
	++KviInputEditor::g_iInputInstances;
	setObjectName("input_widget");
	m_pIconMenu            = 0;
	m_pInputParent         = pPar;
	m_iMaxBufferSize       = KVI_INPUT_MAX_BUFFER_SIZE;
	m_iCursorPosition      = 0;                             //Index of the char AFTER the cursor
	m_iFirstVisibleChar    = 0;                             //Index of the first visible character
	m_iSelectionBegin      = -1;                            //Index of the first char in the selection
	m_iSelectionEnd        = -1;                            //Index of the last char in the selection
	m_bIMComposing         = false;                         //Whether the input method is active (composing).
	// for input method support
	m_iIMStart             = 0;                             //Index of the start of the preedit string.
	m_iIMLength            = 0;                             //Length of the preedit string.
	m_iIMSelectionBegin    = 0;                             //Index of the start of the selection in preedit string.
	m_iIMSelectionLength   = 0;                             //Length of the selection in preedit string.

	m_bCursorOn            = false;                         //Cursor state
	m_iCursorTimer         = 0;                             //Timer that iverts the cursor state
	m_iDragTimer           = 0;                             //Timer for drag selection updates
	m_iLastCursorXPosition = 0;                             //Calculated in paintEvent
	m_iSelectionAnchorChar = -1;                            //Character clicked at the beginning of the selection process
	m_iCurHistoryIdx       = -1;                            //No data in the history
	m_bUpdatesEnabled      = true;
	m_pKviWindow           = pWnd;
	m_pUserListView        = pView;
	m_pHistory             = new KviPointerList<QString>;
	m_pHistory->setAutoDelete(true);
	m_bReadOnly            = false;
	undoState              = 0;
	separator              = false;

	setAttribute(Qt::WA_InputMethodEnabled, true);

	setAutoFillBackground(false);

	setFocusPolicy(Qt::StrongFocus);
	setAcceptDrops(true);

	m_pIconMenu = new KviTalPopupMenu();
	connect(m_pIconMenu,SIGNAL(activated(int)),this,SLOT(iconPopupActivated(int)));

	setCursor(Qt::IBeamCursor);

	setContentsMargins(KVI_INPUT_MARGIN,KVI_INPUT_MARGIN,KVI_INPUT_MARGIN,KVI_INPUT_MARGIN);
	//set the font and font metrics
	applyOptions();
	
	installShortcuts();
}

KviInputEditor::~KviInputEditor()
{
	--KviInputEditor::g_iInputInstances;
	if(KviInputEditor::g_iInputInstances==0 && g_pLastFontMetrics)
	{
		//last instance, delete shared resources
		delete g_pLastFontMetrics;
		g_pLastFontMetrics = 0;
	}

	if(m_pIconMenu)
		delete m_pIconMenu;

	delete m_pHistory;

	if(m_iCursorTimer)
		killTimer(m_iCursorTimer);
	killDragTimer();
}

void KviInputEditor::applyOptions()
{
	//set the font
	QFont newFont(KVI_OPTION_FONT(KviOption_fontInput));
	newFont.setKerning(false);
	setFont(newFont);

	//then, let font metrics be updated in lazy fashion
	if(g_pLastFontMetrics)
		delete g_pLastFontMetrics;
	g_pLastFontMetrics = 0;

	//not that lazy, since we force an update :)
	update();
}

void KviInputEditor::dragEnterEvent(QDragEnterEvent * e)
{
	if(e->mimeData()->hasUrls())
		e->acceptProposedAction();
}

void KviInputEditor::dropEvent(QDropEvent * e)
{
	QList<QUrl> list;
	if(e->mimeData()->hasUrls())
	{
		list = e->mimeData()->urls();
		//debug("Local files decoded");
		if(!list.isEmpty())
		{
			//debug("List not empty");
			QList<QUrl>::Iterator it = list.begin();
			for( ; it != list.end(); ++it )
			{
				QUrl url = *it;
				QString szPath = url.toLocalFile();
				if(szPath.endsWith(KVI_FILEEXTENSION_SCRIPT,Qt::CaseInsensitive))
				{
					//script, parse it
					szPath.prepend("PARSE \"");
					szPath.append("\"");
					if(m_pKviWindow)
						KviKvsScript::run(szPath,m_pKviWindow);
				} else {
					//other file, paste link
					szPath.append(" ");
					insertText(szPath);
				}
			}
		}
	}
}

int  KviInputEditor::heightHint() const
{
	return sizeHint().height();
}

QSize KviInputEditor::sizeHint() const
{
	ensurePolished();
	QFontMetrics *fm = KviInputEditor::getLastFontMetrics(font());
	int h = qMax(fm->height(), 14) + 2*(KVI_INPUT_MARGIN + KVI_INPUT_PADDING + KVI_INPUT_XTRAPADDING);
	int w = fm->width(QLatin1Char('x')) * 17 + 2*(KVI_INPUT_MARGIN + KVI_INPUT_PADDING + KVI_INPUT_XTRAPADDING);

	QStyleOptionFrameV2 option;
	option.initFrom(this);
	option.rect = rect();
	option.lineWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, &option, this);
	option.midLineWidth = 0;

	option.state |= QStyle::State_Sunken;
	if(isReadOnly())
		option.state |= QStyle::State_ReadOnly;
	option.features = QStyleOptionFrameV2::None;

	return (style()->sizeFromContents(QStyle::CT_LineEdit, &option, QSize(w, h).
		expandedTo(QApplication::globalStrut()), this));
}

void KviInputEditor::paintEvent(QPaintEvent *)
{
	QPainter p(this);
	
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency) && g_pApp->supportsCompositing())
	{
		p.save();
		p.setCompositionMode(QPainter::CompositionMode_Source);
		QColor col=KVI_OPTION_COLOR(KviOption_colorGlobalTransparencyFade);
		col.setAlphaF((float)((float)KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) / (float)100));
		p.fillRect(contentsRect(), col);
		p.restore();
	} else if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt = m_pKviWindow->mdiParent() ? mapTo(g_pFrame, contentsRect().topLeft() + g_pFrame->mdiManager()->scrollBarsOffset()) : mapTo(m_pKviWindow, contentsRect().topLeft());
		p.drawTiledPixmap(contentsRect(),*(g_pShadedChildGlobalDesktopBackground), pnt);
	} else {
#endif
		p.fillRect(contentsRect(),KVI_OPTION_COLOR(KviOption_colorInputBackground));
		
		QPixmap * pix = KVI_OPTION_PIXMAP(KviOption_pixmapLabelBackground).pixmap();
		if(pix)
			KviPixmapUtils::drawPixmapWithPainter(&p,pix,KVI_OPTION_UINT(KviOption_uintTreeWindowListPixmapAlign),contentsRect(),contentsRect().width(),contentsRect().height());
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	}
#endif

	QStyleOptionFrameV2 option;

	option.initFrom(this);
	option.rect = rect();
	option.lineWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, &option, this);
	option.midLineWidth = 0;
	
	option.state |= QStyle::State_Sunken;
	if(isReadOnly())
		option.state |= QStyle::State_ReadOnly;
	
	//option.state &= ~(QStyle::State_HasFocus | QStyle::State_Active | QStyle::State_MouseOver); // kill any state that will cause an "active" frame to be painted
	option.features = QStyleOptionFrameV2::None;

	style()->drawPrimitive(QStyle::PE_FrameLineEdit, &option, &p, this);

	QRect r = style()->subElementRect(QStyle::SE_LineEditContents, &option, this);

	r.setX(r.x() + KVI_INPUT_MARGIN + KVI_INPUT_PADDING);
	r.setY(r.y() + KVI_INPUT_MARGIN + KVI_INPUT_PADDING);
	r.setRight(r.right() - KVI_INPUT_MARGIN - KVI_INPUT_PADDING);
	r.setBottom(r.bottom() - KVI_INPUT_MARGIN - KVI_INPUT_PADDING);
		
	p.setClipRect(r);

	p.translate(r.topLeft());
	drawContents(&p);
}

void KviInputEditor::drawContents(QPainter * p)
{
	QRect rect = p->clipRegion().boundingRect();
	QFontMetrics * fm = KviInputEditor::getLastFontMetrics(font());

	int iCurXPos     = 0;
	int iMaxXPos     = rect.width()-1;
	m_iCurBack       = KVI_INPUT_DEF_BACK; //transparent
	m_iCurFore       = KVI_INPUT_DEF_FORE; //normal fore color
	m_bCurBold       = false;
	m_bCurUnderline  = false;

	int iTop          = KVI_INPUT_XTRAPADDING;
	int iBottom       = rect.height() - KVI_INPUT_XTRAPADDING;

	int iTextBaseline = iBottom - fm->descent();

	runUpToTheFirstVisibleChar();

	int iCharIdx      = m_iFirstVisibleChar;

	//Control the selection state
	if((m_iSelectionEnd < m_iSelectionBegin) || (m_iSelectionEnd == -1) || (m_iSelectionBegin == -1))
	{
		m_iSelectionEnd = -1;
		m_iSelectionBegin = -1;
	}

	if((m_iSelectionBegin != -1) && (m_iSelectionEnd >= m_iFirstVisibleChar))
	{
		int iSelStart = m_iSelectionBegin;

		if(iSelStart < m_iFirstVisibleChar)
			iSelStart = m_iFirstVisibleChar;
		int iXLeft = xPositionFromCharIndex(iSelStart);
		int iXRight = xPositionFromCharIndex(m_iSelectionEnd + 1);

		p->fillRect(iXLeft,0,iXRight - iXLeft,rect.height(),KVI_OPTION_COLOR(KviOption_colorInputSelectionBackground));
	}

	// When m_bIMComposing is true, the text between m_iIMStart and m_iIMStart+m_iIMLength should be highlighted to show that this is the active
	// preedit area for the input method, and the text outside cannot be edited while
	// composing. Maybe this can be implemented similarly as painting the selection?
	// Also notice that inside the preedit, there can also be a selection, given by
	// m_iSelectionBegin and m_iSelectionLength, and the widget needs to highlight that
	// while in IM composition mode
	if(m_bIMComposing && m_iIMLength > 0)
	{
		int iIMSelectionStart = m_iIMSelectionBegin;
		if(iIMSelectionStart < m_iFirstVisibleChar)
			iIMSelectionStart = m_iFirstVisibleChar;

		int iXIMSelectionLeft = xPositionFromCharIndex(iIMSelectionStart);
		int iXIMSelectionRight = xPositionFromCharIndex(iIMSelectionStart + m_iIMSelectionLength);
		p->fillRect(iXIMSelectionLeft,iTop,iXIMSelectionRight - iXIMSelectionLeft, iBottom,KVI_OPTION_COLOR(KviOption_colorInputSelectionBackground));

		// highlight the IM selection
		int iIMStart = m_iIMStart;
		if(m_iIMStart < m_iFirstVisibleChar) m_iIMStart = m_iFirstVisibleChar;
		int xIMLeft = xPositionFromCharIndex(iIMStart);
		int xIMRight = xPositionFromCharIndex(iIMStart + m_iIMLength);

		// underline the IM preedit
		// Maybe should be put in drawTextBlock, similar to drawing underlined text
		p->drawLine(xIMLeft, iBottom, xIMRight, iBottom);
	}

	while((iCharIdx < ((int)(m_szTextBuffer.length()))) && (iCurXPos < iMaxXPos))
	{
		extractNextBlock(iCharIdx,fm,iCurXPos,iMaxXPos);

		if(m_bControlBlock)
		{
			p->setPen(KVI_OPTION_COLOR(KviOption_colorInputControl));

			QChar s = getSubstituteChar(m_szTextBuffer[iCharIdx].unicode());

			// the block width is 4 pixels more than the actual character

			p->drawText(iCurXPos + 2,iTextBaseline,s);

			p->drawRect(iCurXPos,iTop,m_iBlockWidth-1,iBottom-1);
		} else {
			if(m_iSelectionBegin!=-1)
			{
				int iBlockEnd = iCharIdx + m_iBlockLen;
				//block is selected (maybe partially)
				if( iBlockEnd > m_iSelectionBegin && iCharIdx <= m_iSelectionEnd )
				{
					int iSubStart,iSubLen;
					//in common it consists of 3 parts: unselected-selected-unselected
					//some of thst parts can be empty (for example block is fully selected)

					//first part start is always equal to the block start
					iSubStart = iCharIdx;
					iSubLen = m_iSelectionBegin > iCharIdx ? m_iSelectionBegin-iCharIdx : 0;

					if(iSubLen)
					{
						drawTextBlock(p,iTop,iBottom,iCurXPos,iTextBaseline,iSubStart,iSubLen,FALSE);
						iCurXPos += m_iBlockWidth;
						m_iBlockWidth=0;
					}

					//second one
					iSubStart += iSubLen;
					iSubLen = m_iSelectionEnd<iBlockEnd ? m_iSelectionEnd-iSubStart+1 : iBlockEnd-iSubStart;

					if(iSubLen)
					{
						drawTextBlock(p,iTop,iBottom,iCurXPos,iTextBaseline,iSubStart,iSubLen,TRUE);
						iCurXPos += m_iBlockWidth;
						m_iBlockWidth=0;
					}

					if(m_iSelectionEnd<(iBlockEnd-1))
					{
						iSubStart += iSubLen;
						iSubLen = iBlockEnd-iSubStart;
						drawTextBlock(p,iTop,iBottom,iCurXPos,iTextBaseline,iSubStart,iSubLen,FALSE);
					}
				} else {
					drawTextBlock(p,iTop,iBottom,iCurXPos,iTextBaseline,iCharIdx,m_iBlockLen);
				}
			} else {
				drawTextBlock(p,iTop,iBottom,iCurXPos,iTextBaseline,iCharIdx,m_iBlockLen);
			}
		}

		iCurXPos += m_iBlockWidth;
		iCharIdx += m_iBlockLen;
	}

	//Now the cursor
	m_iLastCursorXPosition = 0;
	m_iBlockLen = m_iFirstVisibleChar;

	while(m_iBlockLen < m_iCursorPosition)
	{
		QChar c = m_szTextBuffer.at(m_iBlockLen);
		m_iLastCursorXPosition+= (c.unicode() < 256) ? KviInputEditor::g_iInputFontCharWidth[c.unicode()] : fm->width(c);
		m_iBlockLen++;
	}

	if(m_bCursorOn)
	{
		p->setPen(KVI_OPTION_COLOR(KviOption_colorInputCursor));
		p->drawLine(m_iLastCursorXPosition,iTop,m_iLastCursorXPosition,iBottom);
	} else {
		p->setPen(KVI_OPTION_COLOR(KviOption_colorInputForeground));
	}
}

void KviInputEditor::drawTextBlock(QPainter * pa, int iTop, int iBottom, int iCurXPos, int iTextBaseline, int iIdx, int iLen, bool bSelected)
{
	QFontMetrics * fm = KviInputEditor::getLastFontMetrics(font());
	QString szTmp = m_szTextBuffer.mid(iIdx,iLen);
	m_iBlockWidth = fm->width(szTmp);

	if(m_iCurFore == KVI_INPUT_DEF_FORE)
	{
		pa->setPen( bSelected ? KVI_OPTION_COLOR(KviOption_colorInputSelectionForeground) : KVI_OPTION_COLOR(KviOption_colorInputForeground));
	} else {
		if(((unsigned char)m_iCurFore) > 16)
		{
			pa->setPen(KVI_OPTION_COLOR(KviOption_colorInputBackground));
		} else {
			pa->setPen(KVI_OPTION_MIRCCOLOR((unsigned char)m_iCurFore));
		}
	}

	if(m_iCurBack != KVI_INPUT_DEF_BACK)
	{
		if(((unsigned char)m_iCurBack) > 16)
		{
			pa->fillRect(iCurXPos,iTop,m_iBlockWidth,iBottom,KVI_OPTION_COLOR(KviOption_colorInputForeground));
		} else {
			pa->fillRect(iCurXPos,iTop,m_iBlockWidth,iBottom,KVI_OPTION_MIRCCOLOR((unsigned char)m_iCurBack));
		}
	}

	pa->drawText(iCurXPos,iTextBaseline,szTmp);

	if(m_bCurBold)
		pa->drawText(iCurXPos+1,iTextBaseline,szTmp);
	if(m_bCurUnderline)
		pa->drawLine(iCurXPos,iTextBaseline + fm->descent(),iCurXPos+m_iBlockWidth,iTextBaseline + fm->descent());
}

QChar KviInputEditor::getSubstituteChar(unsigned short uControlCode)
{
	switch(uControlCode)
	{
		case KVI_TEXT_COLOR:
			return QChar('K');
			break;
		case KVI_TEXT_BOLD:
			return QChar('B');
			break;
		case KVI_TEXT_RESET:
			return QChar('O');
			break;
		case KVI_TEXT_REVERSE:
			return QChar('R');
			break;
		case KVI_TEXT_UNDERLINE:
			return QChar('U');
			break;
		case KVI_TEXT_CRYPTESCAPE:
			return QChar('P');
			break;
		case KVI_TEXT_ICON:
			return QChar('I');
			break;
		default:
			return QChar(uControlCode);
			break;
	}
}

void KviInputEditor::extractNextBlock(int iIdx, QFontMetrics *fm, int iCurXPos, int iMaxXPos)
{
	m_iBlockLen = 0;
	m_iBlockWidth = 0;

	QChar c = m_szTextBuffer[iIdx];

	if((c.unicode() > 32) ||
		((c != QChar(KVI_TEXT_COLOR)) &&
		(c != QChar(KVI_TEXT_BOLD)) &&
		(c != QChar(KVI_TEXT_UNDERLINE)) &&
		(c != QChar(KVI_TEXT_RESET)) &&
		(c != QChar(KVI_TEXT_REVERSE)) &&
		(c != QChar(KVI_TEXT_CRYPTESCAPE)) &&
		(c != QChar(KVI_TEXT_ICON))))
	{
		m_bControlBlock = false;
		//Not a control code...run..
		while((iIdx < ((int)(m_szTextBuffer.length()))) && (iCurXPos < iMaxXPos))
		{
			c = m_szTextBuffer[iIdx];
			if((c.unicode() > 32) ||
				((c != QChar(KVI_TEXT_COLOR)) &&
				(c != QChar(KVI_TEXT_BOLD)) &&
				(c != QChar(KVI_TEXT_UNDERLINE)) &&
				(c != QChar(KVI_TEXT_RESET)) &&
				(c != QChar(KVI_TEXT_REVERSE)) &&
				(c != QChar(KVI_TEXT_CRYPTESCAPE)) &&
				(c != QChar(KVI_TEXT_ICON))))
			{
				m_iBlockLen++;
				int iXxx = (c.unicode() < 256) ? KviInputEditor::g_iInputFontCharWidth[c.unicode()] : fm->width(c);
				m_iBlockWidth += iXxx;
				iCurXPos      += iXxx;
				iIdx++;
			} else break;
		}
		return;
	} else {
		m_bControlBlock = true;
		m_iBlockLen = 1;
		m_iBlockWidth = KviInputEditor::g_iInputFontCharWidth[c.unicode()];
		//Control code
		switch(c.unicode())
		{
			case KVI_TEXT_BOLD:
				m_bCurBold = ! m_bCurBold;
				break;
			case KVI_TEXT_UNDERLINE:
				m_bCurUnderline = ! m_bCurUnderline;
				break;
			case KVI_TEXT_RESET:
				m_iCurFore = KVI_INPUT_DEF_FORE;
				m_iCurBack = KVI_INPUT_DEF_BACK;
				m_bCurBold = false;
				m_bCurUnderline = false;
				break;
			case KVI_TEXT_REVERSE:
			{
				char cAuxClr = m_iCurFore;
				m_iCurFore  = m_iCurBack;
				m_iCurBack  = cAuxClr;
			}
			break;
			case KVI_TEXT_CRYPTESCAPE:
			case KVI_TEXT_ICON:
				// makes a single block
				break;
			case KVI_TEXT_COLOR:
			{
				iIdx++;
				if(iIdx >= ((int)(m_szTextBuffer.length())))
					return;

				unsigned char uFore;
				unsigned char uBack;
				iIdx = getUnicodeColorBytes(m_szTextBuffer,iIdx,&uFore,&uBack);
				if(uFore != KVI_NOCHANGE)
				{
					m_iCurFore = uFore;
					if(uBack != KVI_NOCHANGE)
						m_iCurBack = uBack;
				} else {
					// ONLY a CTRL+K
					m_iCurBack = KVI_INPUT_DEF_BACK;
					m_iCurFore = KVI_INPUT_DEF_FORE;
				}
			}
			break;
			default:
				debug("Ops..");
				exit(0);
			break;
		}
	}
}

void KviInputEditor::runUpToTheFirstVisibleChar()
{
	register int iIdx = 0;
	while(iIdx < m_iFirstVisibleChar)
	{
		unsigned short uChar = m_szTextBuffer[iIdx].unicode();
		if(uChar < 32)
		{
			switch(uChar)
			{
				case KVI_TEXT_BOLD:
					m_bCurBold = ! m_bCurBold;
					break;
				case KVI_TEXT_UNDERLINE:
					m_bCurUnderline = ! m_bCurUnderline;
					break;
				case KVI_TEXT_RESET:
					m_iCurFore = KVI_INPUT_DEF_FORE;
					m_iCurBack = KVI_INPUT_DEF_BACK;
					m_bCurBold = false;
					m_bCurUnderline = false;
					break;
				case KVI_TEXT_REVERSE:
				{
					char cAuxClr = m_iCurFore;
					m_iCurFore = m_iCurBack;
					m_iCurBack = cAuxClr;
				}
				break;
				case KVI_TEXT_COLOR:
				{
					iIdx++;
					if(iIdx >= ((int)(m_szTextBuffer.length())))return;
					unsigned char uFore;
					unsigned char uBack;
					iIdx = getUnicodeColorBytes(m_szTextBuffer,iIdx,&uFore,&uBack);
					iIdx--;
					if(uFore != KVI_NOCHANGE) m_iCurFore = uFore;
					else m_iCurFore = KVI_INPUT_DEF_FORE;
					if(uBack != KVI_NOCHANGE) m_iCurBack = uBack;
					else m_iCurBack = KVI_INPUT_DEF_BACK;
				}
				break;
				case 0:
					debug("KviInputEditor::Encountered invisible end of the string!");
					exit(0);
				break;
			}
		}
		iIdx++;
	}
}

void KviInputEditor::mouseDoubleClickEvent(QMouseEvent * e)
{
	//select clicked word
	if(e->button() & Qt::LeftButton)
	{
		if(m_szTextBuffer.length()<1)
			return;
		int iCursor = charIndexFromXPosition(e->pos().x());
		int iLen=m_szTextBuffer.length()-1;
		if(iCursor>iLen)
			iCursor=iLen;
		if(!m_szTextBuffer.at(iCursor).isLetterOrNumber())
			return;
		//search word init
		m_iSelectionBegin = iCursor;
		while(m_iSelectionBegin > 0)
		{
			if(!m_szTextBuffer.at(m_iSelectionBegin-1).isLetterOrNumber())
					break;
			m_iSelectionBegin--;
		}
		//ensure that the begin of the selection is visible
		if(m_iFirstVisibleChar>m_iSelectionBegin)
			m_iFirstVisibleChar=m_iSelectionBegin;

		//search word end
		m_iSelectionEnd = iCursor;
		while(m_iSelectionEnd < iLen)
		{
			if(!m_szTextBuffer.at(m_iSelectionEnd+1).isLetterOrNumber())
					break;
			m_iSelectionEnd++;
		}
		if(m_iSelectionEnd==((int)(m_szTextBuffer.length())))
			m_iSelectionEnd--;
		//all-in-one: move cursor at the end of the selection, ensure it's visible and repaint
		moveCursorTo(m_iSelectionEnd, true);
		m_iCursorPosition++;
		killDragTimer();
	}
}

void KviInputEditor::mousePressEvent(QMouseEvent * e)
{
	if(e->button() & Qt::LeftButton)
	{
		m_iCursorPosition = charIndexFromXPosition(e->pos().x());
		//move the cursor to
		int iAnchorX = xPositionFromCharIndex(m_iCursorPosition);
		if(iAnchorX > width()) m_iFirstVisibleChar++;
		m_iSelectionAnchorChar = m_iCursorPosition;
		selectOneChar(-1);
		//grabMouse(QCursor(crossCursor));
		repaintWithCursorOn();
		killDragTimer();
		m_iDragTimer = startTimer(KVI_INPUT_DRAG_TIMEOUT);

	} else if(e->button() & Qt::RightButton)
	{
		int iType = g_pActiveWindow->type();

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
				if(szClip.length() > 60)
				{
					szClip.truncate(60);
					szClip.append("...");
				}
				szClip.replace(QChar('&'),"&amp;");
				szClip.replace(QChar('<'),"&lt;");
				szClip.replace(QChar('>'),"&gt;");
				szClip.replace(QChar('\n'),"<br>");

				QString szLabel = "<center><b>";
				szLabel += __tr2qs("Clipboard");
				szLabel += ":</b><br>";
				szLabel += szClip;
				szLabel += "<br><b>";

				QString szNum;
				szNum.setNum(iOcc);

				szLabel += szNum;
				szLabel += QChar(' ');
				szLabel += (iOcc == 1) ? __tr2qs("line break") : __tr2qs("line breaks");
				szLabel += "</b></center>";

				QLabel * pLabel = new QLabel(szLabel,g_pInputPopup);
				pLabel->setFrameStyle(QFrame::Raised | QFrame::StyledPanel);
				pLabel->setMargin(5);

				delete pLabel;
			}
		}

		int iId = g_pInputPopup->insertItem(__tr2qs("&Undo") + ACCEL_KEY(Z),this,SLOT(undo()));
		g_pInputPopup->setItemEnabled(iId,isUndoAvailable());
		iId = g_pInputPopup->insertItem(__tr2qs("&Redo") + ACCEL_KEY(Y),this,SLOT(redo()));
		g_pInputPopup->setItemEnabled(iId,isRedoAvailable());
		iId = g_pInputPopup->insertItem(__tr2qs("Cu&t") + ACCEL_KEY(X),this,SLOT(cut()));
		g_pInputPopup->setItemEnabled(iId,hasSelection());
		iId = g_pInputPopup->insertItem(__tr2qs("&Copy") + ACCEL_KEY(C),this,SLOT(copyToClipboard()));
		g_pInputPopup->setItemEnabled(iId,hasSelection());
		iId = g_pInputPopup->insertItem(__tr2qs("&Paste") + ACCEL_KEY(V),this,SLOT(pasteClipboardWithConfirmation()));
		g_pInputPopup->setItemEnabled(iId,!szClip.isEmpty() && !m_bReadOnly);
		iId = g_pInputPopup->insertItem(__tr2qs("Paste (Slowly)"),this,SLOT(pasteSlow()));
		if ((iType == KVI_WINDOW_TYPE_CHANNEL) || (iType == KVI_WINDOW_TYPE_QUERY) || (iType == KVI_WINDOW_TYPE_DCCCHAT))
			g_pInputPopup->setItemEnabled(iId,!szClip.isEmpty() && !m_bReadOnly);
		else
			g_pInputPopup->setItemEnabled(iId,false);
		iId = g_pInputPopup->insertItem(__tr2qs("Paste &File") + ACCEL_KEY(L),this,SLOT(pasteFile()));
		if ((iType != KVI_WINDOW_TYPE_CHANNEL) && (iType != KVI_WINDOW_TYPE_QUERY) && (iType != KVI_WINDOW_TYPE_DCCCHAT))
			g_pInputPopup->setItemEnabled(iId,false);
		else
			g_pInputPopup->setItemEnabled(iId,!m_bReadOnly);
		if(m_bSpSlowFlag ==true)
		{
			iId = g_pInputPopup->insertItem(__tr2qs("Stop Paste"),this,SLOT(stopPasteSlow())); /*G&N 2005*/
		}
		iId = g_pInputPopup->insertItem(__tr2qs("Clear"),this,SLOT(clear()));
		g_pInputPopup->setItemEnabled(iId,!m_szTextBuffer.isEmpty() && !m_bReadOnly);
		g_pInputPopup->insertSeparator();
		iId = g_pInputPopup->insertItem(__tr2qs("Select All"),this,SLOT(selectAll()));
		g_pInputPopup->setItemEnabled(iId,(!m_szTextBuffer.isEmpty()));

		g_pInputPopup->insertSeparator();
		m_pIconMenu->clear();

		KviPointerHashTable<QString,KviTextIcon> * d = g_pTextIconManager->textIconDict();
		KviPointerHashTableIterator<QString,KviTextIcon> it(*d);
		QStringList szList;

		while(it.current())
		{
			szList.append(it.currentKey());
			++it;
		}
		szList.sort();

		KviTextIcon * pIcon;
		QPixmap * pPix;

		for(QStringList::Iterator iter = szList.begin(); iter != szList.end(); ++iter)
		{
			pIcon = g_pTextIconManager->lookupTextIcon(*iter);
			if(pIcon)
			{
				pPix = pIcon->pixmap();
				if(pPix) m_pIconMenu->insertItem(*pPix,*iter);
			}
		}

		g_pInputPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_BIGGRIN)),__tr2qs("Insert Icon"),m_pIconMenu);

		QInputContext *qic = g_pApp->inputContext();
		if (qic) {
			QList<QAction *> imActions = qic->actions();
			for (int i = 0; i < imActions.size(); ++i)
				g_pInputPopup->addAction(imActions.at(i));
		}

		g_pInputPopup->popup(mapToGlobal(e->pos()));
	} else {
		pasteSelectionWithConfirmation();
	}
}

void KviInputEditor::iconPopupActivated(int iId)
{
	if(!m_bReadOnly)
	{
		QString szText = m_pIconMenu->text(iId);
		if(!szText.isEmpty())
		{
			szText.prepend(KVI_TEXT_ICON);
			szText.append(' ');
			insertText(szText);
		}
	}
}

bool KviInputEditor::hasSelection()
{
	return ((m_iSelectionBegin != -1) && (m_iSelectionEnd != -1));
}

void KviInputEditor::copyToClipboard()
{
	if(!hasSelection()) return;
	QClipboard * pClip = QApplication::clipboard();
	if(!pClip) return;
	QString szTxt = m_szTextBuffer.mid(m_iSelectionBegin,(m_iSelectionEnd-m_iSelectionBegin)+1);
	pClip->setText(szTxt,QClipboard::Clipboard);
	repaintWithCursorOn();
}

void KviInputEditor::copyToSelection(bool bDonNotCopyToClipboard)
{
	if(!hasSelection()) return;
	QClipboard * pClip = QApplication::clipboard();
	if(!pClip) return;
	QString szTxt = m_szTextBuffer.mid(m_iSelectionBegin,(m_iSelectionEnd-m_iSelectionBegin)+1);
	if(pClip->supportsSelection())
		pClip->setText(szTxt,QClipboard::Selection);
	else if(!bDonNotCopyToClipboard)
		pClip->setText(szTxt,QClipboard::Clipboard);
	repaintWithCursorOn();
}

void KviInputEditor::moveCursorTo(int iIdx, bool bRepaint)
{
	if(iIdx < 0) iIdx = 0;
	if(iIdx > ((int)(m_szTextBuffer.length()))) iIdx = m_szTextBuffer.length();
	if(iIdx > m_iCursorPosition)
	{
		while(m_iCursorPosition < iIdx)
		{
			moveRightFirstVisibleCharToShowCursor();
			m_iCursorPosition++;
		}
	} else {
		m_iCursorPosition = iIdx;
		if(m_iFirstVisibleChar > m_iCursorPosition)m_iFirstVisibleChar = m_iCursorPosition;
	}
	if(bRepaint) repaintWithCursorOn();
}

void KviInputEditor::removeSelected()
{
	if(!hasSelection()) return;

	addCommand(Command(SetSelection, m_iCursorPosition, 0, m_iSelectionBegin, m_iSelectionEnd));
	addCommand(Command(DeleteSelection, m_iCursorPosition, m_szTextBuffer.mid(m_iSelectionBegin, m_iSelectionEnd-m_iSelectionBegin+1), -1, -1));

	m_szTextBuffer.remove(m_iSelectionBegin,(m_iSelectionEnd-m_iSelectionBegin)+1);
	moveCursorTo(m_iSelectionBegin,false);
	selectOneChar(-1);
	repaintWithCursorOn();
}

void KviInputEditor::cut()
{
	if(!hasSelection()) return;
	QClipboard * pClip = QApplication::clipboard();
	if(!pClip) return;
	pClip->setText(m_szTextBuffer.mid(m_iSelectionBegin,(m_iSelectionEnd-m_iSelectionBegin)+1),QClipboard::Clipboard);

	addCommand(Command(SetSelection, m_iCursorPosition, 0, m_iSelectionBegin, m_iSelectionEnd));
	addCommand (Command(DeleteSelection, m_iSelectionBegin, m_szTextBuffer.mid(m_iSelectionBegin, m_iSelectionEnd-m_iSelectionBegin+1), -1, -1));

	m_szTextBuffer.remove(m_iSelectionBegin,(m_iSelectionEnd-m_iSelectionBegin)+1);
	moveCursorTo(m_iSelectionBegin,false);
	selectOneChar(-1);
	repaintWithCursorOn();
}

void KviInputEditor::insertText(const QString & szTxt)
{
	QString szText = szTxt; // crop away constness
	if(szText.isEmpty())return;

	szText.replace('\t',QString(KVI_OPTION_UINT(KviOption_uintSpacesToExpandTabulationInput),' ')); //expand tabs to spaces

	m_bUpdatesEnabled = false;
	removeSelected();
	m_bUpdatesEnabled = true;

	if(szText.indexOf('\n') == -1)
	{
		addCommand(Command(Insert, m_iCursorPosition, szText, -1, -1));
		m_szTextBuffer.insert(m_iCursorPosition,szText);
		m_szTextBuffer.truncate(m_iMaxBufferSize);
		moveCursorTo(m_iCursorPosition + szText.length());
	} else {
		//Multiline paste...do not execute commands here
		QString szBlock;
		while(!szText.isEmpty())
		{
			int iIdx = szText.indexOf('\n');
			if(iIdx != -1)
			{
				szBlock = szText.left(iIdx);
				//else szBlock = QChar(KVI_TEXT_RESET);
				szText.remove(0,iIdx+1);
			} else {
				szBlock = szText;
				szText  = "";
			}

			m_szTextBuffer.insert(m_iCursorPosition,szBlock);
			m_szTextBuffer.truncate(m_iMaxBufferSize);

			int iPos = 0;
			while((iPos < ((int)(m_szTextBuffer.length()))) && (m_szTextBuffer[iPos] < 33)) iPos++;
			if((iPos < ((int)(m_szTextBuffer.length()))) && (m_szTextBuffer[iPos] == QChar('/')))m_szTextBuffer.insert(iPos,"\\");

			returnPressed(iIdx != -1);
		}
	}
}

int KviInputEditor::replaceSegment(int iStart, int iLength, const QString & szText)
{
	m_szTextBuffer.remove(iStart, iLength);
	m_szTextBuffer.insert(iStart, szText);
	m_szTextBuffer.truncate(m_iMaxBufferSize);
	repaintWithCursorOn();

	int iInsertedLength = szText.length();
	int iMaxInsertedLength = m_iMaxBufferSize - iStart;
	if(iInsertedLength > iMaxInsertedLength) return iMaxInsertedLength;
	return iInsertedLength;
}

void KviInputEditor::pasteClipboardWithConfirmation()
{
	QClipboard * pClip = QApplication::clipboard();
	if(!pClip) return;
	QString szText = pClip->text(QClipboard::Clipboard);

	if(szText.contains(QChar('\n')) > 0)
	{
		if(m_pInputParent->inherits("KviInput"))
			((KviInput*)(m_pInputParent))->multiLinePaste(szText);
	} else {
		insertText(szText);
	}
}

void KviInputEditor::pasteSelectionWithConfirmation()
{
	QClipboard * pClip = QApplication::clipboard();
	if(!pClip) return;
	QString szText = pClip->text(pClip->supportsSelection() ? QClipboard::Selection : QClipboard::Clipboard);

	if(szText.contains(QChar('\n')) > 0)
	{
		if(m_pInputParent->inherits("KviInput"))
			((KviInput*)(m_pInputParent))->multiLinePaste(szText);
	} else {
		insertText(szText);
	}
}

void KviInputEditor::pasteSlow()
{
	KviKvsScript::run("spaste.clipboard",g_pActiveWindow);
	m_bSpSlowFlag = true;
}

void KviInputEditor::stopPasteSlow()
{
	KviKvsScript::run("spaste.stop",g_pActiveWindow);
	m_bSpSlowFlag = false;
}

void KviInputEditor::pasteFile()
{
	QString szTmp = QFileDialog::getOpenFileName(this,"Choose a file","","");
	if(szTmp != "")
	{
		szTmp.replace("\"", "\\\"");
		QString szTmp2 = QString("spaste.file \"%1\"").arg(szTmp);
		KviKvsScript::run(szTmp2,g_pActiveWindow);
		m_bSpSlowFlag = true;
	}
}

void KviInputEditor::selectAll()
{
	if(m_szTextBuffer.length() > 0)
	{
		m_iSelectionBegin = 0;
		m_iSelectionEnd = m_szTextBuffer.length()-1;
	}
	end();
}

void KviInputEditor::clear()
{
	m_szTextBuffer = "";
	selectOneChar(-1);
	home();
}

void KviInputEditor::setText(const QString szText)
{
	m_szTextBuffer = szText;
	m_szTextBuffer.truncate(m_iMaxBufferSize);
	selectOneChar(-1);
	end();
}

void KviInputEditor::mouseReleaseEvent(QMouseEvent *)
{
	if(m_iDragTimer)
	{
		m_iSelectionAnchorChar =-1;
		//releaseMouse();
		killDragTimer();
	}
	if(hasSelection()) copyToSelection();
}

void KviInputEditor::killDragTimer()
{
	if(m_iDragTimer)
	{
		killTimer(m_iDragTimer);
		m_iDragTimer = 0;
	}
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
		} else m_bCursorOn = ! m_bCursorOn;
		update();
	} else {
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

	if(pnt.x() <= 0)
	{
		//Left side dragging
		if(m_iFirstVisibleChar > 0)
			m_iFirstVisibleChar--;
		m_iCursorPosition = m_iFirstVisibleChar;
	} else if(pnt.x() >= width()) {
		//Right side dragging...add a single character to the selection on the right
		if(m_iCursorPosition < ((int)(m_szTextBuffer.length())))
		{
			moveRightFirstVisibleCharToShowCursor();
			m_iCursorPosition++;
		} //else at the end of the selection...don't move anything
	} else {
		//Inside the window...
		m_iCursorPosition = charIndexFromXPosition(pnt.x());
	}

	if(m_iCursorPosition == m_iSelectionAnchorChar) selectOneChar(-1);
	else {
		if(m_iCursorPosition > m_iSelectionAnchorChar)
		{
			m_iSelectionBegin = m_iSelectionAnchorChar;
			m_iSelectionEnd   = m_iCursorPosition-1;
		} else {
			m_iSelectionBegin = m_iCursorPosition;
			m_iSelectionEnd   = m_iSelectionAnchorChar-1;
		}
	}
	repaintWithCursorOn();
}

void KviInputEditor::returnPressed(bool)
{
	if (!m_szTextBuffer.isEmpty() /* && (!m_pHistory->current() || m_szTextBuffer.compare(*(m_pHistory->current())))*/)
	{
		if(m_pInputParent->inherits("KviInput"))
			KviInputHistory::instance()->add(new QString(m_szTextBuffer));

		m_pHistory->insert(0,new QString(m_szTextBuffer));
	}

	//ensure the color window is hidden (bug #835)
	if(g_pColorWindow)
		if(g_pColorWindow->isVisible())
			g_pColorWindow->hide();

	__range_valid(KVI_INPUT_MAX_LOCAL_HISTORY_ENTRIES > 1); //ABSOLUTELY NEEDED, if not, pHist will be destroyed...
	if(m_pHistory->count() > KVI_INPUT_MAX_LOCAL_HISTORY_ENTRIES)m_pHistory->removeLast();

	m_iCurHistoryIdx = -1;

	emit enterPressed();
}

void KviInputEditor::focusInEvent(QFocusEvent *)
{
	if(m_iCursorTimer==0)
	{
		m_iCursorTimer = startTimer(KVI_INPUT_BLINK_TIME);
		m_bCursorOn = true;
		update();
	}
}

void KviInputEditor::focusOutEvent(QFocusEvent *)
{
	if(m_iCursorTimer) killTimer(m_iCursorTimer);
	m_iCursorTimer = 0;
	m_bCursorOn = false;
	update();
}

void KviInputEditor::internalCursorRight(bool bShift)
{
	if(m_iCursorPosition >= ((int)(m_szTextBuffer.length()))) return;
	moveRightFirstVisibleCharToShowCursor();
	//Grow the selection if needed
	if(bShift)
	{
		if((m_iSelectionBegin > -1)&&(m_iSelectionEnd > -1))
		{
			if(m_iSelectionEnd == m_iCursorPosition-1)m_iSelectionEnd++;
			else if(m_iSelectionBegin == m_iCursorPosition)m_iSelectionBegin++;
			else selectOneChar(m_iCursorPosition);
		} else selectOneChar(m_iCursorPosition);
	} else selectOneChar(-1);
	m_iCursorPosition++;
}

void KviInputEditor::internalCursorLeft(bool bShift)
{
	if(m_iCursorPosition <= 0) return;

	if(bShift)
	{
		if((m_iSelectionBegin > -1) && (m_iSelectionEnd > -1))
		{
			if(m_iSelectionBegin == m_iCursorPosition)
				m_iSelectionBegin--;
			else if(m_iSelectionEnd == m_iCursorPosition-1)
				m_iSelectionEnd--;
			else selectOneChar(m_iCursorPosition - 1);
		} else selectOneChar(m_iCursorPosition - 1);
	} else selectOneChar(-1);

	m_iCursorPosition--;
	if(m_iFirstVisibleChar > m_iCursorPosition) m_iFirstVisibleChar--;
}

void KviInputEditor::inputMethodEvent(QInputMethodEvent * e)
{
	if (m_bReadOnly) {
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

	if (e->commitString().isEmpty())
	{
		if(e->preeditString().isEmpty())
		{
			m_bIMComposing = false;
			m_iIMStart = 0;
			m_iIMLength = 0;
		} else {
			// replace the preedit area with the IM result text
			m_iIMLength = replaceSegment(m_iIMStart, m_iIMLength, e->preeditString());
			// move cursor to after the IM result text
			moveCursorTo(m_iIMStart + m_iIMLength);
		}
	} else {
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
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_HISTORY),this,SLOT(openHistory()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_HISTORY_PREV),this,SLOT(historyPrev()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_HISTORY_NEXT),this,SLOT(historyNext()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_PREV_CHAR),this,SLOT(previousChar()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_NEXT_CHAR),this,SLOT(nextChar()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_PREV_CHAR_SELECT),this,SLOT(previousCharSelection()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_NEXT_CHAR_SELECT),this,SLOT(nextCharSelection()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_PREV_WORD),this,SLOT(previousWord()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_NEXT_WORD),this,SLOT(nextWord()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_PREV_WORD_SELECT),this,SLOT(previousWordSelection()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_NEXT_WORD_SELECT),this,SLOT(nextWordSelection()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_BOLD),this,SLOT(insertBold()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_RESET),this,SLOT(insertReset()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_UNDERLINE),this,SLOT(insertUnderline()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_REVERSE),this,SLOT(insertReverse()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_PLAINTEXT),this,SLOT(insertPlainText()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_ICON),this,SLOT(insertIcon()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_COLOR),this,SLOT(insertColor()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_COPY),this,SLOT(copyInternal()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_CUT),this,SLOT(cutInternal()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_PASTE),this,SLOT(pasteInternal()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_PASTE_2),this,SLOT(pasteInternal()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_UNDO),this,SLOT(undoInternal()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_REDO),this,SLOT(redoInternal()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_SELECT_ALL),this,SLOT(selectAllInternal()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_DELETE_WORD),this,SLOT(deleteWord()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_WIN_PREV_LINE),this,SLOT(previousLine()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_WIN_NEXT_LINE),this,SLOT(nextLine()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_WIN_PREV_PAGE),this,SLOT(previousPage()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_WIN_NEXT_PAGE),this,SLOT(nextPage()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_WIN_SEARCH),this,SLOT(search()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_SEND_PLAIN),this,SLOT(sendPlain()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_SEND_PLAIN_2),this,SLOT(sendPlain()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_HOME),this,SLOT(homeInternal()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_END),this,SLOT(endInternal()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_HOME_SELECT),this,SLOT(homeInternalSelection()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_END_SELECT),this,SLOT(endInternalSelection()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_RETURN),this,SLOT(returnHit()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_RETURN_2),this,SLOT(returnHit()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_RETURN_3),this,SLOT(returnHit()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_RETURN_4),this,SLOT(returnHit()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_BACKSPACE),this,SLOT(backspaceHit()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_BACKSPACE_2),this,SLOT(backspaceHit()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_DELETE),this,SLOT(deleteHit()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_ESCAPE),this,SLOT(escapeHit()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_COMMANDLINE),this,SLOT(toggleCommandMode()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_DUMMY),this,SLOT(dummy()),0,Qt::WidgetShortcut);
	new QShortcut(QKeySequence(KVI_SHORTCUTS_INPUT_DUMMY_2),this,SLOT(dummy()),0,Qt::WidgetShortcut);
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
		} else {
			m_bLastCompletionFinished=1;
		}
	}

	if(e->modifiers() & Qt::ControlModifier)
	{
		switch(e->key())
		{
			case Qt::Key_J:
			{
				//avoid Ctrl+J from inserting a linefeed
				break;
			}
			default:
				if(!m_bReadOnly) insertText(e->text());
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
		} else if((e->text().unicode()->toLatin1() >= '0') && (e->text().unicode()->toLatin1() <= '9')) {
			m_szAltKeyCode += e->text().unicode()->toLatin1();
			return;
		} else if((e->key() >= Qt::Key_0) && (e->key() <= Qt::Key_9)) {
			m_szAltKeyCode += e->key();
			return;
		}

		//debug("%c",e->ascii());
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
				//debug("INSERTING CHAR %d",ch);
				insertChar(QChar(uCh));
				e->accept();
			}
		}
		m_szAltKeyCode = "";
	}
#endif
	e->ignore();
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
	if(iIdx2 > iIdx) iIdx = iIdx2;
	if(iIdx3 > iIdx) iIdx = iIdx3;
	if(iIdx4 > iIdx) iIdx = iIdx4;
	*bIsFirstWordInLine = false;
	if(iIdx > -1) szBuffer.remove(0,iIdx+1);
	else *bIsFirstWordInLine = true;
}

void KviInputEditor::completion(bool bShift)
{
	// FIXME: Spaces in directory completion can mess everything completely
	//        On windows the characters are breaking everything...
	//        Well.... :D

	QString szWord;
	QString szMatch;
	bool bFirstWordInLine;

	bool bInCommand = m_szTextBuffer.trimmed().indexOf('/') == 0;

	getWordBeforeCursor(szWord,&bFirstWordInLine);
	if(szWord.isEmpty())
	{
		if(m_szLastCompletedNick.isEmpty())
			return; // nothing to complete

		// this is standard nick completion continued
		standardNickCompletion(bShift,szWord,bFirstWordInLine,bInCommand);
		repaintWithCursorOn();
		return;
	}
	int iOffset;
	if(KviQString::equalCI(m_szTextBuffer.left(5),"/help") || KviQString::equalCI(m_szTextBuffer.left(5),"/help.open")) iOffset=1;
	else iOffset=0;

	KviPointerList<QString> tmp;
	tmp.setAutoDelete(true);

	bool bIsCommand = false;
	bool bIsFunction = false;
	bool bIsDir = false;
	bool bIsNick = false;

	unsigned short uc = szWord[0].unicode();


	if(uc == '/' || iOffset)
	{
		if(szWord[1-iOffset].unicode()=='$')
		{
			// function/identifer completion
			szWord.remove(0,2-iOffset);
			if(szWord.isEmpty()) return;
			KviKvsKernel::instance()->completeFunction(szWord,&tmp);
			bIsFunction = true;
		}
		else if(bFirstWordInLine || iOffset)
		{
			// command completion
			szWord.remove(0,1-iOffset);
			if(szWord.isEmpty())return;
			KviKvsKernel::instance()->completeCommand(szWord,&tmp);
			bIsCommand = true;
		} else {
			// directory completion attempt
			g_pApp->completeDirectory(szWord,&tmp);
			bIsDir = true;
		}
	} else if(uc == '$')
	{
		// function/identifer completion
		szWord.remove(0,1);
		if(szWord.isEmpty()) return;
		KviKvsKernel::instance()->completeFunction(szWord,&tmp);
		bIsFunction = true;
	} else if(uc == '#' || uc == '&' || uc == '!')
	{
		if(m_pKviWindow)
		{
			if( (szWord.length()==1) && (m_pKviWindow->windowName()[0].unicode()==uc))
			{
				szMatch=m_pKviWindow->windowName();
				szMatch.append(" ");
				replaceWordBeforeCursor(szWord,szMatch,false);
				repaintWithCursorOn();
				return;
			} else {
				if(m_pKviWindow->console())
					m_pKviWindow->console()->completeChannel(szWord,&tmp);
			}
		}

	//FIXME: Complete also on irc:// starting strings, not only irc.?
	} else if(KviQString::equalCIN(szWord,"irc.",4))
	{
		// irc server name
		if(m_pKviWindow)
			if(m_pKviWindow->console())
				m_pKviWindow->console()->completeServer(szWord,&tmp);
	} else {
		// empty word will end up here
		if(m_pUserListView)
		{
			if(KVI_OPTION_BOOL(KviOption_boolZshLikeNickCompletion))
			{
				if(m_szLastCompletedNick.isEmpty())
				{
					//first round of zsh completion
					m_pUserListView->completeNickBashLike(szWord,&tmp,bShift);
					bIsNick = true;
					m_szLastCompletedNick=szWord;
				} else {
					standardNickCompletion(bShift,szWord,bFirstWordInLine,bInCommand);
					repaintWithCursorOn();
					return;
				}
			} else if(KVI_OPTION_BOOL(KviOption_boolBashLikeNickCompletion))
			{
				m_pUserListView->completeNickBashLike(szWord,&tmp,bShift);
				bIsNick = true;
			} else {
				standardNickCompletion(bShift,szWord,bFirstWordInLine,bInCommand);
				repaintWithCursorOn();
				return;
			}
		}
	}

	// Lookup the longest exact match
	if(tmp.count() > 0)
	{
		if(tmp.count() == 1)
		{
			szMatch = *(tmp.first());
			if (szMatch.left(1)=='$')szMatch.remove(0,1);
			if(bIsCommand && szMatch.right(1)!='.')szMatch.append(' ');
			else if(bIsFunction && szMatch.right(1)!='.')szMatch.append('(');
			else if(bIsNick)
			{
				if(!KVI_OPTION_STRING(KviOption_stringNickCompletionPostfix).isEmpty())
				{
					if(bFirstWordInLine || (!KVI_OPTION_BOOL(KviOption_boolUseNickCompletionPostfixForFirstWordOnly)))
						szMatch.append(KVI_OPTION_STRING(KviOption_stringNickCompletionPostfix));
				}
				if(bInCommand)
				{
					// escape crazy things like Nick\nquit
					szMatch.replace(QChar('\\'),QString::fromAscii("\\\\"));
				}
			}
		} else {
			QString szAll;
			QString * szTmp = tmp.first();
			szMatch = *szTmp;
			int iWLen = szWord.length();
			if (szMatch.left(1)=='$')szMatch.remove(0,1);
			for(; szTmp; szTmp = tmp.next())
			{
				if(szTmp->length() < szMatch.length())
					szMatch.remove(szTmp->length(),szMatch.length() - szTmp->length());
				// All the matches here have length >= word.len()!!!
				const QChar * b1 = KviQString::nullTerminatedArray(*szTmp) + iWLen;
				const QChar * b2 = KviQString::nullTerminatedArray(szMatch) + iWLen;
				const QChar * c1 = b1;
				const QChar * c2 = b2;
				if(bIsDir)while(c1->unicode() && (c1->unicode() == c2->unicode()))c1++,c2++;
				else while(c1->unicode() && (c1->toLower().unicode() == c2->toLower().unicode()))c1++,c2++;
				int iLen = iWLen + (c1 - b1);
				if(iLen < ((int)(szMatch.length())))szMatch.remove(iLen,szMatch.length() - iLen);
				if(!szAll.isEmpty())szAll.append(", ");
				szAll.append(*szTmp);
			}
			if(m_pKviWindow)
				m_pKviWindow->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("%d matches: %Q"),tmp.count(),&szAll);
		}
	} else
		if(m_pKviWindow)
			m_pKviWindow->outputNoFmt(KVI_OUT_SYSTEMMESSAGE,__tr2qs("No matches"));

	if(!szMatch.isEmpty())
	{
		//if(!bIsDir && !bIsNick)match = match.toLower(); <-- why? It is nice to have
		//						 $module.someFunctionName instad
		//						 of unreadable $module.somefunctionfame
		replaceWordBeforeCursor(szWord,szMatch,false);
	}

	repaintWithCursorOn();
}

void KviInputEditor::replaceWordBeforeCursor(const QString & szWord, const QString & szReplacement, bool bRepaint)
{
	selectOneChar(-1);
	m_iCursorPosition -= szWord.length();
	m_szTextBuffer.remove(m_iCursorPosition,szWord.length());
	m_szTextBuffer.insert(m_iCursorPosition,szReplacement);
	m_szTextBuffer.truncate(m_iMaxBufferSize);
	moveCursorTo(m_iCursorPosition + szReplacement.length());
	if(bRepaint)
		repaintWithCursorOn();
}

void KviInputEditor::standardNickCompletionInsertCompletedText(const QString &szReplacedWord,const QString &szCompletedText,bool bFirstWordInLine,bool bInCommand)
{
	QString szBuffer = szCompletedText;

	if(!KVI_OPTION_STRING(KviOption_stringNickCompletionPostfix).isEmpty())
	{
		if(bFirstWordInLine || (!KVI_OPTION_BOOL(KviOption_boolUseNickCompletionPostfixForFirstWordOnly)))
			szBuffer.append(KVI_OPTION_STRING(KviOption_stringNickCompletionPostfix));
	}
	if(bInCommand)
	{
		// escape crazy things like Nick\nquit
		szBuffer.replace(QChar('\\'),QString::fromAscii("\\\\"));
	}
	replaceWordBeforeCursor(szReplacedWord,szBuffer,false);
}

void KviInputEditor::standardNickCompletion(bool bAddMask,QString & szWord,bool bFirstWordInLine,bool bInCommand)
{
	// FIXME: this could be really simplified...
	if(!m_pUserListView)
		return;

	selectOneChar(-1);

	QString szBuffer;
	if(m_szLastCompletedNick.isEmpty())
	{
		// New completion session: we NEED sth to complete
		if(szWord.isEmpty())
			return;

		if(m_pUserListView->completeNickStandard(szWord,m_szLastCompletedNick,szBuffer,bAddMask))
		{
			// completed: save the buffer
			m_szLastCompletionBuffer          = m_szTextBuffer;
			m_iLastCompletionCursorPosition   = m_iCursorPosition;
			m_iLastCompletionCursorXPosition  = m_iLastCursorXPosition;
			m_iLastCompletionFirstVisibleChar = m_iFirstVisibleChar;
			m_szLastCompletedNick             = szBuffer;
			standardNickCompletionInsertCompletedText(szWord,szBuffer,bFirstWordInLine,bInCommand);
			m_bLastCompletionFinished=0;
			// REPAINT CALLED FROM OUTSIDE!
		} // else no match at all
		
		return;
	}
	
	if(!m_bLastCompletionFinished)
	{
		// Old session
		// swap the buffers
		m_szTextBuffer                        = m_szLastCompletionBuffer;
		m_iCursorPosition                     = m_iLastCompletionCursorPosition;
		m_iLastCursorXPosition                = m_iLastCompletionCursorXPosition;
		m_iFirstVisibleChar                   = m_iLastCompletionFirstVisibleChar;
		// re-extract
		//word = m_szTextBuffer.left(m_iCursorPosition);

		getWordBeforeCursor(szWord,&bFirstWordInLine);
		if(szWord.isEmpty())return;
		if(m_pUserListView->completeNickStandard(szWord,m_szLastCompletedNick,szBuffer,bAddMask))
		{
			// completed
			m_szLastCompletedNick             = szBuffer;
			standardNickCompletionInsertCompletedText(szWord,szBuffer,bFirstWordInLine,bInCommand);
			m_bLastCompletionFinished=0;
			// REPAINT CALLED FROM OUTSIDE!
		} else {
			m_bLastCompletionFinished=1;
			m_szLastCompletedNick = "";
		}
		
		return;
	}
	
	// Old session finished
	// re-extract
	//word = m_szTextBuffer.left(m_iCursorPosition);
	//getWordBeforeCursor(word,&bFirstWordInLine);
	if(szWord.isEmpty())return;
	if(m_pUserListView->completeNickStandard(szWord,"",szBuffer,bAddMask))
	{
		// completed
		m_szLastCompletionBuffer          = m_szTextBuffer;
		m_iLastCompletionCursorPosition   = m_iCursorPosition;
		m_iLastCompletionCursorXPosition  = m_iLastCursorXPosition;
		m_iLastCompletionFirstVisibleChar = m_iFirstVisibleChar;
		m_szLastCompletedNick             = szBuffer;
		standardNickCompletionInsertCompletedText(szWord,szBuffer,bFirstWordInLine,bInCommand);
		m_bLastCompletionFinished=0;
		// REPAINT CALLED FROM OUTSIDE!
	} else {
		m_bLastCompletionFinished=1;
		m_szLastCompletedNick = "";
	}
}

//Funky helpers
void KviInputEditor::end()
{
	m_iLastCursorXPosition = 0;
	m_iCursorPosition = 0;
	m_iFirstVisibleChar = 0;
	while(m_iCursorPosition < ((int)(m_szTextBuffer.length())))
	{
		moveRightFirstVisibleCharToShowCursor();
		m_iCursorPosition++;
	}
	repaintWithCursorOn();
}

void KviInputEditor::home()
{
	m_iFirstVisibleChar = 0;
	m_iCursorPosition   = 0;
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
	selectOneChar(-1);
	m_szTextBuffer.insert(m_iCursorPosition,c);

	addCommand(Command(Insert, m_iCursorPosition, c, -1, -1));

	moveRightFirstVisibleCharToShowCursor();
	m_iCursorPosition++;
	repaintWithCursorOn();
}

void KviInputEditor::moveRightFirstVisibleCharToShowCursor()
{
	// :)
	QFontMetrics * fm = KviInputEditor::getLastFontMetrics(font());

	QChar c = m_szTextBuffer.at(m_iCursorPosition);

	m_iLastCursorXPosition += (c.unicode() < 256) ? KviInputEditor::g_iInputFontCharWidth[c.unicode()] : fm->width(c);

	while(m_iLastCursorXPosition >= contentsRect().width()-2*KVI_INPUT_MARGIN && m_iFirstVisibleChar < m_szTextBuffer.size())
	{
		c = m_szTextBuffer.at(m_iFirstVisibleChar);

		m_iLastCursorXPosition -= (c.unicode() < 256) ? KviInputEditor::g_iInputFontCharWidth[c.unicode()] : fm->width(c);

		m_iFirstVisibleChar++;
	}
}

void KviInputEditor::repaintWithCursorOn()
{
	// :)
	if(m_bUpdatesEnabled)
	{
		m_bCursorOn = true;
		update();
	}
}

void KviInputEditor::selectOneChar(int iPos)
{
	separate();
	m_iSelectionBegin = iPos;
	m_iSelectionEnd   = iPos;
}

int KviInputEditor::charIndexFromXPosition(int iXPos)
{
	int iCurXPos = KVI_INPUT_MARGIN;
	int iCurChar = m_iFirstVisibleChar;
	int iBufLen  = m_szTextBuffer.length();

	QFontMetrics *fm = KviInputEditor::getLastFontMetrics(font());
	while(iCurChar < iBufLen)
	{
		QChar c = m_szTextBuffer.at(iCurChar);
		int iWidthCh = (c.unicode() < 256) ? KviInputEditor::g_iInputFontCharWidth[c.unicode()] : fm->width(c);

		if(iXPos < (iCurXPos+(iWidthCh/2)))
		{
			return iCurChar;
		} else {
			if(iXPos < (iCurXPos+iWidthCh))
			{
				return (iCurChar+1);
			}
		}

		iCurXPos += iWidthCh;
		iCurChar++;
	}
	return iCurChar;
}

int KviInputEditor::xPositionFromCharIndex(int iChIdx)
{
	QFontMetrics *fm = KviInputEditor::getLastFontMetrics(font());
	int iCurXPos = 0;
	int iCurChar = m_iFirstVisibleChar;

	if(m_szTextBuffer.isEmpty())
		return iCurXPos;
	
	while(iCurChar < iChIdx)
	{
		QChar c = m_szTextBuffer.at(iCurChar);
		iCurXPos += (c.unicode() < 256) ? KviInputEditor::g_iInputFontCharWidth[c.unicode()] : fm->width(c);
		iCurChar++;
	}

	return iCurXPos;
}

void KviInputEditor::undo(int iUntil)
{
	if (!isUndoAvailable())
		return;
	selectOneChar(-1);
	while (undoState && undoState > iUntil)
	{
		Command& cmd = history[--undoState];
		switch (cmd.type) {
			case Insert:
				m_szTextBuffer.remove(cmd.pos, cmd.us.size());
				moveCursorTo(cmd.pos);
				break;
			case SetSelection:
				m_iSelectionBegin = cmd.selStart;
				m_iSelectionEnd   = cmd.selEnd;
				moveCursorTo(cmd.pos);
				break;
			case Remove:
			case RemoveSelection:
				m_szTextBuffer.insert(cmd.pos, cmd.us);
				moveCursorTo(cmd.pos + cmd.us.size());
				break;
			case Delete:
			case DeleteSelection:
				m_szTextBuffer.insert(cmd.pos, cmd.us);
				moveCursorTo(cmd.pos);
				break;
			case Separator:
				continue;
		}
		if (iUntil < 0 && undoState)
		{
			Command& next = history[undoState-1];
			if (next.type != cmd.type && next.type < RemoveSelection && (cmd.type < RemoveSelection || next.type == Separator))
				break;
		}
	}
}

void KviInputEditor::redo()
{
	if (!isRedoAvailable())
		return;
	selectOneChar(-1);
	while (undoState < (int)history.size())
	{
		Command& cmd = history[undoState++];
		switch (cmd.type)
		{
			case Insert:
				m_szTextBuffer.insert(cmd.pos, cmd.us);
				moveCursorTo(cmd.pos+cmd.us.size());
				break;
			case SetSelection:
				m_iSelectionBegin = cmd.selStart;
				m_iSelectionEnd   = cmd.selEnd;
				moveCursorTo(cmd.pos);
				break;
			case Remove:
			case Delete:
			case RemoveSelection:
			case DeleteSelection:
				m_szTextBuffer.remove(cmd.pos, cmd.us.size());
				moveCursorTo(cmd.pos);
				break;
			case Separator:
				m_iSelectionBegin = cmd.selStart;
				m_iSelectionEnd   = cmd.selEnd;
				moveCursorTo(cmd.pos);
				break;
		}
		if (undoState < (int)history.size())
		{
			Command& next = history[undoState];
			if (next.type != cmd.type && cmd.type < RemoveSelection && next.type != Separator
				&& (next.type < RemoveSelection || cmd.type == Separator))
				break;
		}
	}
}

void KviInputEditor::addCommand(const Command & cmd)
{
	if (separator && undoState && history[undoState-1].type != Separator)
	{
		history.resize(undoState + 2);
		history[undoState++] = Command(Separator, m_iCursorPosition, 0, m_iSelectionBegin, m_iSelectionEnd);
	} else {
		history.resize(undoState + 1);
	}
	separator = false;
	history[undoState++] = cmd;
}

void KviInputEditor::openHistory()
{
	if(!KVI_OPTION_BOOL(KviOption_boolEnableInputHistory))
		return;
	if(m_pInputParent->inherits("KviInput"))
		((KviInput*)(m_pInputParent))->historyButtonClicked();
}

void KviInputEditor::toggleMultiLineEditor()
{
	if(m_pInputParent->inherits("KviInput"))
	{
		((KviInput*)(m_pInputParent))->multiLinePaste(m_szTextBuffer);
		clear();
		return;
	}
}

void KviInputEditor::previousChar()
{
	if(m_iCursorPosition > 0)
	{
		internalCursorLeft(false);
		repaintWithCursorOn();
	}
}

void KviInputEditor::nextChar()
{
	if(m_iCursorPosition < ((int)(m_szTextBuffer.length())))
	{
		internalCursorRight(false);
		repaintWithCursorOn();
	}
}

void KviInputEditor::previousCharSelection()
{
	if(m_iCursorPosition > 0)
	{
		internalCursorLeft(true);
		repaintWithCursorOn();
	}
}

void KviInputEditor::nextCharSelection()
{
	if(m_iCursorPosition < ((int)(m_szTextBuffer.length())))
	{
		internalCursorRight(true);
		repaintWithCursorOn();
	}
}

void KviInputEditor::previousWord()
{
	if(m_iCursorPosition > 0)
	{
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
}

void KviInputEditor::nextWord()
{
	if(m_iCursorPosition < ((int)(m_szTextBuffer.length())))
	{
		// skip whitespace
		while(m_iCursorPosition < ((int)(m_szTextBuffer.length())))
		{
			if(!m_szTextBuffer.at(m_iCursorPosition).isSpace())
				break;
			internalCursorRight(false);
		}
		// skip nonwhitespace
		while(m_iCursorPosition < ((int)(m_szTextBuffer.length())))
		{
			if(m_szTextBuffer.at(m_iCursorPosition).isSpace())
				break;
			internalCursorRight(false);
		}
		repaintWithCursorOn();
	}
}

void KviInputEditor::previousWordSelection()
{
	if(m_iCursorPosition > 0)
	{
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
}

void KviInputEditor::nextWordSelection()
{
	if(m_iCursorPosition < ((int)(m_szTextBuffer.length())))
	{
		// skip whitespace
		while(m_iCursorPosition < ((int)(m_szTextBuffer.length())))
		{
			if(!m_szTextBuffer.at(m_iCursorPosition).isSpace())
				break;
			internalCursorRight(true);
		}
		// skip nonwhitespace
		while(m_iCursorPosition < ((int)(m_szTextBuffer.length())))
		{
			if(m_szTextBuffer.at(m_iCursorPosition).isSpace())
				break;
			internalCursorRight(true);
		}
		repaintWithCursorOn();
	}
}

void KviInputEditor::insertBold()
{
	if(!m_bReadOnly) insertChar(KVI_TEXT_BOLD);
}

void KviInputEditor::insertReset()
{
	if(!m_bReadOnly) insertChar(KVI_TEXT_RESET);
}

void KviInputEditor::insertUnderline()
{
	if(!m_bReadOnly) insertChar(KVI_TEXT_UNDERLINE);
}

void KviInputEditor::insertReverse()
{
	if(!m_bReadOnly) insertChar(KVI_TEXT_REVERSE);
}

void KviInputEditor::insertPlainText()
{
	if(!m_bReadOnly) insertChar(KVI_TEXT_CRYPTESCAPE); // DO NOT CRYPT THIS STUFF
}

void KviInputEditor::insertIcon()
{
	if(!m_bReadOnly)
	{
		insertChar(KVI_TEXT_ICON); // THE NEXT WORD IS AN ICON NAME
		int iXPos = xPositionFromCharIndex(m_iCursorPosition);
		if(iXPos > 24)
			iXPos-=24;
		if(!g_pTextIconWindow)
			g_pTextIconWindow = new KviTextIconWindow();

		if(iXPos+g_pTextIconWindow->width() > width())
			iXPos = width()-(g_pTextIconWindow->width()+2);
		g_pTextIconWindow->move(mapToGlobal(QPoint(iXPos,-KVI_TEXTICON_WIN_HEIGHT)));
		g_pTextIconWindow->popup(this,false);
	}
}

void KviInputEditor::insertColor()
{
	if(!m_bReadOnly)
	{
		insertChar(KVI_TEXT_COLOR);
		int xPos = xPositionFromCharIndex(m_iCursorPosition);
		if(xPos > 24)xPos-=24;
		if(!g_pColorWindow)g_pColorWindow = new KviColorWindow();
		if(xPos+g_pColorWindow->width() > width())xPos = width()-(g_pColorWindow->width()+2);
		g_pColorWindow->move(mapToGlobal(QPoint(xPos,-35)));
		g_pColorWindow->popup(this);
	}
}

void KviInputEditor::copyInternal()
{
	copyToClipboard();
}

void KviInputEditor::cutInternal()
{
	if(!m_bReadOnly) cut();
}

void KviInputEditor::pasteInternal()
{
	if(!m_bReadOnly) pasteClipboardWithConfirmation();
}

void KviInputEditor::undoInternal()
{
	if(!m_bReadOnly) undo();
}

void KviInputEditor::redoInternal()
{
	if(!m_bReadOnly) redo();
}

void KviInputEditor::selectAllInternal()
{
	m_iSelectionBegin=0;
	m_iSelectionEnd=m_szTextBuffer.length()-1;
	m_iCursorPosition=m_szTextBuffer.length();
	repaintWithCursorOn();
}

void KviInputEditor::deleteWord()
{
	if(m_iCursorPosition > 0 && !m_bReadOnly && !hasSelection())
	{
		// skip whitespace
		while(m_iCursorPosition > 0)
		{
			if(!m_szTextBuffer.at(m_iCursorPosition - 1).isSpace())
				break;
			m_szTextBuffer.remove(m_iCursorPosition-1,1);
			m_iCursorPosition--;
			if(m_iFirstVisibleChar > m_iCursorPosition)
				m_iFirstVisibleChar--;
		}
		// skip nonwhitespace
		while(m_iCursorPosition > 0)
		{
			if(m_szTextBuffer.at(m_iCursorPosition - 1).isSpace())
				break;
			m_szTextBuffer.remove(m_iCursorPosition-1,1);
			m_iCursorPosition--;
			if(m_iFirstVisibleChar > m_iCursorPosition)
				m_iFirstVisibleChar--;
		}
		repaintWithCursorOn();
	}
}

void KviInputEditor::previousLine()
{
	if(m_pKviWindow)
		if(m_pKviWindow->view()) m_pKviWindow->view()->prevLine();
	return;
}

void KviInputEditor::nextLine()
{
	if(m_pKviWindow)
		if(m_pKviWindow->view()) m_pKviWindow->view()->nextLine();
	return;
}

void KviInputEditor::previousPage()
{
	if(m_pKviWindow)
		if(m_pKviWindow->view()) m_pKviWindow->view()->prevPage();
}

void KviInputEditor::nextPage()
{
	if(m_pKviWindow)
		if(m_pKviWindow->view()) m_pKviWindow->view()->nextPage();
}

void KviInputEditor::search()
{
	if(m_pKviWindow)
		if(m_pKviWindow->view()) m_pKviWindow->view()->toggleToolWidget();
}

void KviInputEditor::sendPlain()
{
	if(m_pInputParent->inherits("KviInput"))
	{
		//ensure the color window is hidden (bug #835)
		if(g_pColorWindow)
			if(g_pColorWindow->isVisible())
				g_pColorWindow->hide();

		QString szBuffer(m_szTextBuffer);
		m_szTextBuffer="";
		selectOneChar(-1);
		m_iCursorPosition = 0;
		m_iFirstVisibleChar = 0;
		repaintWithCursorOn();
		KviUserInput::parseNonCommand(szBuffer,m_pKviWindow);
		if (!szBuffer.isEmpty())
		{
			KviInputHistory::instance()->add(new QString(szBuffer));
			m_pHistory->insert(0,new QString(szBuffer));
		}

		__range_valid(KVI_INPUT_MAX_LOCAL_HISTORY_ENTRIES > 1); //ABSOLUTELY NEEDED, if not, pHist will be destroyed...
		if(m_pHistory->count() > KVI_INPUT_MAX_LOCAL_HISTORY_ENTRIES)m_pHistory->removeLast();

		m_iCurHistoryIdx = -1;
	}
}

void KviInputEditor::homeInternal()
{
	if(m_iCursorPosition > 0)
	{
		selectOneChar(-1);
		home();
	}
}

void KviInputEditor::homeInternalSelection()
{
	if(m_iCursorPosition > 0)
	{
		if((m_iSelectionBegin == -1) && (m_iSelectionEnd == -1))
		{
			// There is no selection
			m_iSelectionEnd = m_iCursorPosition - 1;
		} else {
			// There is selection
			m_iSelectionEnd = m_iSelectionBegin - 1;
		}
		
		m_iSelectionBegin = 0;
		home();
	}
}

void KviInputEditor::endInternal()
{
	selectOneChar(-1);
	end();
}

void KviInputEditor::endInternalSelection()
{
	if((m_iSelectionBegin == -1) && (m_iSelectionEnd == -1))
	{
		// There is no selection
		m_iSelectionBegin = m_iCursorPosition;
	} else {
		// There is selection
		m_iSelectionBegin = m_iSelectionEnd+1;
	}

	m_iSelectionEnd = m_szTextBuffer.length()-1;
	end();
}

void KviInputEditor::historyPrev()
{
	if(!m_bReadOnly)
	{
		if(m_pHistory->count() > 0)
		{
			if(m_iCurHistoryIdx < 0)
			{
				m_szSaveTextBuffer = m_szTextBuffer;
				m_szTextBuffer = *(m_pHistory->at(0));
				m_iCurHistoryIdx = 0;
			} else if(m_iCurHistoryIdx >= (int)(m_pHistory->count()-1))
			{
				m_szTextBuffer=m_szSaveTextBuffer;
				m_iCurHistoryIdx = -1;
			} else {
				m_iCurHistoryIdx++;
				m_szTextBuffer = *(m_pHistory->at(m_iCurHistoryIdx));
			}
			selectOneChar(-1);
			if(KVI_OPTION_BOOL(KviOption_boolInputHistoryCursorAtEnd))end();
			else home();
		}
	}
}

void KviInputEditor::historyNext()
{
	if(!m_bReadOnly)
	{
		if(m_pHistory->count() > 0)
		{
			if(m_iCurHistoryIdx < 0)
			{
				m_szSaveTextBuffer = m_szTextBuffer;
				m_szTextBuffer = *(m_pHistory->at(m_pHistory->count()-1));
				m_iCurHistoryIdx =m_pHistory->count()-1;
			} else if(m_iCurHistoryIdx == 0)
			{
				m_szTextBuffer=m_szSaveTextBuffer;
				m_iCurHistoryIdx = -1;
			} else {
				m_iCurHistoryIdx--;
				m_szTextBuffer = *(m_pHistory->at(m_iCurHistoryIdx));
			}
			selectOneChar(-1);
			if(KVI_OPTION_BOOL(KviOption_boolInputHistoryCursorAtEnd))end();
			else home();
		}
	}
}

void KviInputEditor::returnHit()
{
	returnPressed();
}

void KviInputEditor::backspaceHit()
{
	if(m_bReadOnly)
		return;
	if(hasSelection() && (m_iSelectionEnd >= m_iCursorPosition-1) && (m_iSelectionBegin <= m_iCursorPosition))
	{
		//remove the selection
		m_szTextBuffer.remove(m_iSelectionBegin,(m_iSelectionEnd-m_iSelectionBegin)+1);
		m_iCursorPosition = m_iSelectionBegin;
		if(m_iFirstVisibleChar > m_iCursorPosition)
			m_iFirstVisibleChar = m_iCursorPosition;
	} else if(m_iCursorPosition > 0)
	{
		m_iCursorPosition--;
		m_szTextBuffer.remove(m_iCursorPosition,1);
		if(m_iFirstVisibleChar > m_iCursorPosition)
			m_iFirstVisibleChar--;
	}
	selectOneChar(-1);
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
	
	if(m_iCursorPosition < (int)m_szTextBuffer.length())
	{
		m_szTextBuffer.remove(m_iCursorPosition,1);
		selectOneChar(-1);
		repaintWithCursorOn();
	}
}

void KviInputEditor::escapeHit()
{
	emit escapePressed();
	return;
}

void KviInputEditor::toggleCommandMode()
{
	if(m_pInputParent->inherits("KviInput"))
		((KviInput *)(m_pInputParent))->setUserFriendly(!((KviInput *)(m_pInputParent))->isUserFriendly());
}

void KviInputEditor::dummy()
{
	// this function does nothing. check the header file for explanation
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "kvi_input_editor.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
