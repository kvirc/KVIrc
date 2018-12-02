//===========================================================================
//
//   File : KviIrcView.cpp
//   Creation date : Tue Jul 6 1999 14:45:20 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek (pragma at kvirc dot net)
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
//===========================================================================

// Damn complex class ...but it works :)
// #include <brain.h>
//
// #define HOPE_THAT_IT_WILL_NEVER_NEED_TO_BE_MODIFIED :)

// 07 May 1999,
//      Already forgot how this damn thing works,
//      and spent 1 hour over a stupid bug.
//      I had to recreate the whole thing in my mind......ooooouh...
//      How did I wrote it ?
//      Just take a look to paintEvent() or to calculateLineWraps()...
//      Anyway...I've solved the bug.

// 23 Nov 1999,
//      Well, not so bad...I seem to still remember how it works
//      So just for fun, complicated the things a little bit more.
//      Added pre-calculation of the text blocks and word wrapping
//      and a fast scrolling mode (3 lines at once) for consecutive
//      appendText() calls.
//      Now the code becomes really not understandable...:)

// 29 Jun 2000 21:02,
//      Here we go again... I have to adjust this stuff for 3.0.0
//      Will I make this thingie work?
// 01 Jul 2000 04:20 (AM!),
//      Yes....I got it to work just now
//      and YES, complicated the things yet more.
//      This time made some paint event code completely unreadable
//      by placing two monster macros...
//      I hope that you have a smart compiler (such as gcc is).

// 09 Dec 2000
//      This is my C-asm-optimisation-hack playground
//      Expect Bad Programming(tm), Ugly Code(tm), Unreadable Macros (tm)
//      and massive usage of the Evil(tm) goto.

// 25 Sep 2001
//      This stuff is going to be ported to Windoze
//      A conditionally compiled code will use only Qt calls...let's see :)
//

//
// Here we go... a huge set of includes
//

#include "KviIrcView.h"
#include "KviIrcView_tools.h"
#include "KviIrcView_private.h"
#include "kvi_debug.h"
#include "KviApplication.h"
#include "kvi_settings.h"
#include "KviOptions.h"
#include "KviControlCodes.h"
#include "kvi_defaults.h"
#include "KviWindow.h"
#include "KviLocale.h"
#include "KviMainWindow.h"
#include "KviMemory.h"
#include "KviIconManager.h"
#include "kvi_out.h"
#include "KviConsoleWindow.h"
#include "KviIrcUserDataBase.h"
#include "KviChannelWindow.h"
#include "KviFileDialog.h"
#include "KviMessageBox.h"
#include "KviTextIconManager.h"
#include "KviIrcConnection.h"
#include "KviWindowStack.h"
#include "KviUserInput.h"
#include "KviAnimatedPixmap.h"
#include "KviPixmapUtils.h"
#include "KviTrayIcon.h"

#include <QPainter>
#include <QRegExp>
#include <QFontMetrics>
#include <QMessageBox>
#include <QPaintEvent>
#include <QDateTime>
#include <QScrollBar>
#include <QFontDialog>
#include <QByteArray>
#include <QMenu>
#include <QWindow>

#include <ctime>

#ifdef COMPILE_ON_WINDOWS
#pragma warning(disable : 4102)
#endif

#ifdef __STRICT_ANSI__
#ifdef COMPILE_USE_DYNAMIC_LABELS
// incompatible with -ansi

#endif
#endif
//#undef COMPILE_USE_DYNAMIC_LABELS

#define KVI_DEF_BACK 200

//
// Globals
//

// Stuff declared in KviApplication.cpp and managed by KviApplication class

#ifdef COMPILE_PSEUDO_TRANSPARENCY
extern QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif

//
// Internal constants
//

// Maximum size of the internal buffer for each window
// This is the default value
//#define KVI_IRCVIEW_MAX_LINES 1024
// A little bit more than the scroll-bar...
// Qt+X have strange interactions that I can not understand when I try to move the splitter
// to the maximum on the left, Maybe the cache pixmap size becomes negative ? (I don't think so)
// Anyway, when the scroll bar position becomes negative (or the IrcView has smaller width than
// the scroll bar) X aborts with a funny
// X Error: BadDrawable (invalid Pixmap or Window parameter) 9
//   Major opcode:  55
// Program received signal SIGABRT, Aborted.
// Do not change unless you're sure that it will not happen :)
#define KVI_IRCVIEW_MINIMUM_WIDTH 22
// 16+4+(2*4) * Do not change
// this is mostly needed to avoid collapsing in slit view
#define KVI_IRCVIEW_MINIMUM_HEIGHT 22
#define KVI_IRCVIEW_PIXMAP_AND_SEPARATOR 20
#define KVI_IRCVIEW_DOUBLEBORDER_WIDTH 8
#define KVI_IRCVIEW_SIZEHINT_WIDTH 150
#define KVI_IRCVIEW_SIZEHINT_HEIGHT 150

#define KVI_IRCVIEW_BLOCK_SELECTION_TOTAL 0
#define KVI_IRCVIEW_BLOCK_SELECTION_LEFT 1
#define KVI_IRCVIEW_BLOCK_SELECTION_RIGHT 2
#define KVI_IRCVIEW_BLOCK_SELECTION_CENTRAL 3
#define KVI_IRCVIEW_BLOCK_SELECTION_ICON 4

#define KVI_IRCVIEW_PIXMAP_SIZE 16

#define KVI_IRCVIEW_ESCAPE_TAG_URLLINK 'u'
#define KVI_IRCVIEW_ESCAPE_TAG_NICKLINK 'n'
#define KVI_IRCVIEW_ESCAPE_TAG_SERVERLINK 's'
#define KVI_IRCVIEW_ESCAPE_TAG_HOSTLINK 'h'
#define KVI_IRCVIEW_ESCAPE_TAG_GENERICESCAPE '['

//
// Info about escape syntax
//

// escape commands:
//
//  <cr>!<escape_command><cr><visible parameters<cr>
//
//  <escape_command> ::= u        <--- URL link
//  <escape_command> ::= n        <--- nick link
//  <escape_command> ::= s        <--- server link
//  <escape_command> ::= h        <--- host link
//  <escape_command> ::= [...     <--- generic escape "rbt" | "mbt" | "dbl" | "txt"
//

//
// The IrcView : construct and destroy
//

KviIrcView::KviIrcView(QWidget * parent, KviWindow * pWnd)
    : QWidget(parent)
{
	setObjectName("irc_view");
	// Ok...here we go
	// initialize the initializable

	setAttribute(Qt::WA_NoSystemBackground); // This disables automatic qt double buffering
	                                         // 	setAttribute(Qt::WA_OpaquePaintEvent);
	                                         // 	setAttribute(Qt::WA_PaintOnScreen); // disable qt backing store (that would force us to trigger repaint() instead of the 10 times faster paintEvent(0))

	m_iFlushTimer = 0;
	m_pToolsPopup = nullptr;
	m_pFirstLine = nullptr;
	m_pCurLine = nullptr;
	m_pLastLine = nullptr;
	m_pCursorLine = nullptr;
	m_uLineMarkLineIndex = KVI_IRCVIEW_INVALID_LINE_MARK_INDEX;
	m_bHaveUnreadedHighlightedMessages = false;
	m_bHaveUnreadedMessages = false;
	m_iNumLines = 0;
	m_iMaxLines = KVI_OPTION_UINT(KviOption_uintIrcViewMaxBufferSize);

	m_uNextLineIndex = 0;
	m_pSelectionInitLine = nullptr;
	m_pSelectionEndLine = nullptr;
	m_iSelectionInitCharIndex = 0;
	m_iSelectionEndCharIndex = 0;
	m_iSelectTimer = 0;

	if(m_iMaxLines < 32)
	{
		m_iMaxLines = 32;
		KVI_OPTION_UINT(KviOption_uintIrcViewMaxBufferSize) = 32;
	}

	m_bMouseIsDown = false;

	//m_bShowImages            = KVI_OPTION_BOOL(KviOption_boolIrcViewShowImages);

	m_iMouseTimer = 0;
	m_pLastEvent = nullptr;
	m_iLastMouseClickTime = QDateTime::currentMSecsSinceEpoch();

	m_bAcceptDrops = false;
	m_pPrivateBackgroundPixmap = nullptr;
	m_bSkipScrollBarRepaint = false;
	m_pLogFile = nullptr;
	m_pKviWindow = pWnd;

	m_iUnprocessedPaintEventRequests = 0;
	m_bPostedPaintEventPending = false;

	m_pLastLinkUnderMouse = nullptr;
	m_iLastLinkRectTop = -1;
	m_iLastLinkRectHeight = -1;

	m_pMasterView = nullptr;

	m_pToolWidget = nullptr;

	m_pWrappedBlockSelectionInfo = new KviIrcViewWrappedBlockSelectionInfo;

	// say qt to avoid erasing on repaint
	setAutoFillBackground(false);

	m_pFm = nullptr; // will be updated in the first paint event
	m_iFontDescent = 0;
	m_iFontLineSpacing = 0;
	m_iFontLineWidth = 0;

	m_pToolTip = new KviIrcViewToolTip(this);

	// Create the scroll bar
	m_pScrollBar = new QScrollBar(Qt::Vertical, this);
	m_pScrollBar->setAutoFillBackground(true);
	m_pScrollBar->setMaximum(0);
	m_pScrollBar->setMinimum(0);
	m_pScrollBar->setSingleStep(1);
	m_pScrollBar->setPageStep(10);
	m_pScrollBar->setValue(0);
	m_pScrollBar->setObjectName("irc_view_scrollbar");
	m_pScrollBar->setTracking(true);
	m_pScrollBar->show();
	m_pScrollBar->setFocusProxy(this);

	m_pToolsButton = new QToolButton(this);
	m_pToolsButton->setObjectName("btntools");
	m_pToolsButton->setAutoFillBackground(true);

	QIcon is1(*(g_pIconManager->getSmallIcon(KviIconManager::PopupMenu)));
	m_pToolsButton->setAutoRaise(true);
	m_pToolsButton->setIcon(is1);

	KviTalToolTip::add(m_pToolsButton, __tr2qs("Output view tools"));
	m_pToolsButton->setFocusProxy(this);

	connect(m_pToolsButton, SIGNAL(clicked()), this, SLOT(showToolsPopup()));
	m_pToolsButton->show();

	connect(m_pScrollBar, SIGNAL(valueChanged(int)), this, SLOT(scrollBarPositionChanged(int)));
	m_iLastScrollBarValue = 0;

	// set the minimum size
	setMinimumSize(KVI_IRCVIEW_MINIMUM_WIDTH + m_pScrollBar->sizeHint().width(), KVI_IRCVIEW_MINIMUM_HEIGHT);
	// and catch all mouse events
	setMouseTracking(true);
	// let's go!
	applyOptions();

	if(KVI_OPTION_UINT(KviOption_uintAutoFlushLogs)) //m_iFlushTimer
	{
		m_iFlushTimer = startTimer(KVI_OPTION_UINT(KviOption_uintAutoFlushLogs) * 60 * 1000);
	}

	//	if(pWnd->input()) setFocusProxy(pWnd->input());

	QSizePolicy oSizePolicy = sizePolicy();
	oSizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
	setSizePolicy(oSizePolicy);
}

static inline void delete_text_line(KviIrcViewLine * line, QHash<KviIrcViewLine *, KviAnimatedPixmap *> * animatedSmiles)
{
	QMultiHash<KviIrcViewLine *, KviAnimatedPixmap *>::iterator it = animatedSmiles->find(line);
	while(it != animatedSmiles->end() && it.key() == line)
	{
		it = animatedSmiles->erase(it);
	}
	for(size_t i{}; i < line->uChunkCount; i++)
	{
		if((line->pChunks[i].type == KviControlCodes::Escape) || (line->pChunks[i].type == KviControlCodes::Icon))
		{
			if((line->pChunks[i].type == KviControlCodes::Icon) && (line->pChunks[i].szPayload != line->pChunks[i].szSmileId))
				KviMemory::free(line->pChunks[i].szSmileId);
			KviMemory::free(line->pChunks[i].szPayload);
		}
	}
	KviMemory::free(line->pChunks); // free attributes data
	if(line->iBlockCount)
		KviMemory::free(line->pBlocks);
	delete line;
}

KviIrcView::~KviIrcView()
{
	// kill any pending timer
	if(m_iFlushTimer)
		killTimer(m_iFlushTimer);
	if(m_iSelectTimer)
		killTimer(m_iSelectTimer);
	if(m_iMouseTimer)
		killTimer(m_iMouseTimer);

	// and close the log file (flush!)
	stopLogging();

	if(m_pToolWidget)
		delete m_pToolWidget;

	// don't forget the background pixmap!
	if(m_pPrivateBackgroundPixmap)
		delete m_pPrivateBackgroundPixmap;

	// and to remove all the text lines
	emptyBuffer(false);

	// the pending ones too!
	for(const auto & l : m_pMessagesStoppedWhileSelecting)
		delete_text_line(l, &m_hAnimatedSmiles);

	m_pMessagesStoppedWhileSelecting.clear();

	if(m_pFm)
		delete m_pFm;

	delete m_pToolTip;
	delete m_pWrappedBlockSelectionInfo;
}

void KviIrcView::showEvent(QShowEvent * e)
{
	QWindow * pWin = topLevelWidget()->windowHandle();
	if(!pWin)
		return; // huh ?

	QObject::disconnect(pWin,SIGNAL(screenChanged(QScreen *)),this,SLOT(screenChanged(QScreen *)));
	QObject::connect(pWin,SIGNAL(screenChanged(QScreen *)),this,SLOT(screenChanged(QScreen *)));
}

void KviIrcView::screenChanged(QScreen *)
{
	// Changing screen can change DPI. Reset font so metrics are recomputed.
	setFont(font());
}


//
// The IrcView : options
//

void KviIrcView::setFont(const QFont & f)
{
	if(m_pFm)
	{
		// force an update to the font variables
		delete m_pFm;
		m_pFm = nullptr;
	}
	KviIrcViewLine * l = m_pFirstLine;
	while(l)
	{
		l->iMaxLineWidth = -1;
		l = l->pNext;
	}

	QFont newFont(f);
	newFont.setKerning(false);
	newFont.setStyleStrategy(QFont::StyleStrategy(newFont.styleStrategy() | QFont::ForceIntegerMetrics));
	QWidget::setFont(newFont);
	update();
}

void KviIrcView::applyOptions()
{
	flushLog();

	if(m_iFlushTimer)
		killTimer(m_iFlushTimer);

	if(KVI_OPTION_UINT(KviOption_uintAutoFlushLogs))
		m_iFlushTimer = startTimer(KVI_OPTION_UINT(KviOption_uintAutoFlushLogs) * 60 * 1000);

	// Will do nothing if the view is still empty (i.e. called from the constructor)
	reapplyMessageColors();

	setFont(KVI_OPTION_FONT(KviOption_fontIrcView));
}

//
// The IrcView : DnD - 2005.Resurrection by Grifisx & Noldor
//

void KviIrcView::enableDnd(bool bEnable)
{
	setAcceptDrops(bEnable);
	m_bAcceptDrops = bEnable;
}

void KviIrcView::clearBuffer()
{
	emptyBuffer(true);
	m_pScrollBar->setRange(0, m_iNumLines);
}

bool KviIrcView::saveBuffer(const char * pcFilename)
{
	QFile f(QString::fromUtf8(pcFilename));
	if(!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return false;
	QString szTmp;
	getTextBuffer(szTmp);
	QByteArray tmpx = szTmp.toUtf8();
	f.write(tmpx.data(), tmpx.length());
	f.close();
	return true;
}

void KviIrcView::prevLine() { m_pScrollBar->triggerAction(QAbstractSlider::SliderSingleStepSub); }
void KviIrcView::nextLine() { m_pScrollBar->triggerAction(QAbstractSlider::SliderSingleStepAdd); }
void KviIrcView::prevPage() { m_pScrollBar->triggerAction(QAbstractSlider::SliderPageStepSub); }
void KviIrcView::nextPage() { m_pScrollBar->triggerAction(QAbstractSlider::SliderPageStepAdd); }
void KviIrcView::scrollTop() { m_pScrollBar->triggerAction(QAbstractSlider::SliderToMinimum); }
void KviIrcView::scrollBottom() { m_pScrollBar->triggerAction(QAbstractSlider::SliderToMaximum); }

void KviIrcView::setPrivateBackgroundPixmap(const QPixmap & pixmap, bool bRepaint)
{
	if(m_pPrivateBackgroundPixmap)
	{
		delete m_pPrivateBackgroundPixmap;
		m_pPrivateBackgroundPixmap = nullptr;
	}
	if(!pixmap.isNull())
		m_pPrivateBackgroundPixmap = new QPixmap(pixmap);

	if(bRepaint)
		update();
}

void KviIrcView::emptyBuffer(bool bRepaint)
{
	while(m_pLastLine != nullptr)
		removeHeadLine();
	if(bRepaint)
		update();
}

void KviIrcView::clearLineMark(bool bRepaint)
{
	m_uLineMarkLineIndex = KVI_IRCVIEW_INVALID_LINE_MARK_INDEX;
	clearUnreaded();
	if(bRepaint)
		update();
}

void KviIrcView::clearUnreaded()
{
	m_bHaveUnreadedHighlightedMessages = false;
	m_bHaveUnreadedMessages = false;

	if(g_pMainWindow->trayIcon())
		g_pMainWindow->trayIcon()->refresh();
}

void KviIrcView::setMaxBufferSize(int maxBufSize, bool bRepaint)
{
	if(maxBufSize < 32)
		maxBufSize = 32;
	m_iMaxLines = maxBufSize;
	while(m_iNumLines > m_iMaxLines)
		removeHeadLine();
	m_pScrollBar->setRange(0, m_iNumLines);
	if(bRepaint)
		update();
}

/*
void KviIrcView::setTimestamp(bool bTimestamp)
{
	m_bTimestamp = bTimestamp;


// STATS FOR A BUFFER FULL OF HIGHLY COLORED STRINGS, HIGHLY WRAPPED
//
// Lines = 1024 (322425 bytes - 314 KB) (avg 314 bytes per line), well :)
// string bytes = 87745 (85 KB)
// attributes = 3576 (42912 bytes - 41 KB)
// blocks = 12226 (146712 bytes - 143 KB)
//
//	unsigned long int nAlloc = 0;
//	unsigned long int nLines = 0;
//	unsigned long int nStringBytes = 0;
//	unsigned long int nAttrBytes = 0;
//	unsigned long int nBlockBytes = 0;
//	unsigned long int nBlocks = 0;
//	unsigned long int nAttributes = 0;
//	KviIrcViewLine * l=m_pFirstLine;
//	while(l){
//		nLines++;
//		nAlloc += sizeof(KviIrcViewLine);
//		nStringBytes += l->data_len + 1;
//		nAlloc += l->data_len + 1;
//		nAlloc += (l->uChunkCount * sizeof(KviIrcViewLineChunk));
//		nAttrBytes +=(l->uChunkCount * sizeof(KviIrcViewLineChunk));
//		nAlloc += (l->iBlockCount * sizeof(KviIrcViewLineChunk));
//		nBlockBytes += (l->iBlockCount * sizeof(KviIrcViewLineChunk));
//		nBlocks += (l->iBlockCount);
//		nAttributes += (l->uChunkCount);
//		l = l->pNext;
//	}
//	qDebug("\n\nLines = %u (%u bytes - %u KB) (avg %u bytes per line)",nLines,nAlloc,nAlloc / 1024,nLines ? (nAlloc / nLines) : 0);
//	qDebug("string bytes = %u (%u KB)",nStringBytes,nStringBytes / 1024);
//	qDebug("attributes = %u (%u bytes - %u KB)",nAttributes,nAttrBytes,nAttrBytes / 1024);
//	qDebug("blocks = %u (%u bytes - %u KB)\n",nBlocks,nBlockBytes,nBlockBytes / 1024);

}
*/
void KviIrcView::scrollBarPositionChanged(int newValue)
{
	if(!m_pCurLine)
		return;
	if(newValue > m_iLastScrollBarValue)
	{
		while(newValue > m_iLastScrollBarValue)
		{
			if(m_pCurLine->pNext)
			{
				m_pCurLine = m_pCurLine->pNext;
			}
			m_iLastScrollBarValue++;
		}
	}
	else
	{
		while(newValue < m_iLastScrollBarValue)
		{
			if(m_pCurLine->pPrev)
				m_pCurLine = m_pCurLine->pPrev;
			m_iLastScrollBarValue--;
		}
	}
	if(!m_bSkipScrollBarRepaint)
		repaint();
}

void KviIrcView::postUpdateEvent()
{
	// This will post a QEvent with a full repaint request
	if(!m_bPostedPaintEventPending)
	{
		m_bPostedPaintEventPending = true;
		QEvent * e = new QEvent(QEvent::User);
		g_pApp->postEvent(this, e); // queue a repaint
	}

	m_iUnprocessedPaintEventRequests++; // paintEvent() will set it to 0

	if(m_iUnprocessedPaintEventRequests == 3)
	{
// Three unprocessed paint events...do it now
#ifdef COMPILE_PSEUDO_TRANSPARENCY
		if(!((KVI_OPTION_PIXMAP(KviOption_pixmapIrcViewBackground).pixmap()) || m_pPrivateBackgroundPixmap || g_pShadedChildGlobalDesktopBackground || KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency)))
			fastScroll(3);
#else
		if(!((KVI_OPTION_PIXMAP(KviOption_pixmapIrcViewBackground).pixmap()) || m_pPrivateBackgroundPixmap))
			fastScroll(3);
#endif
		else
			repaint();
	}
}

void KviIrcView::appendLine(KviIrcViewLine * ptr, const QDateTime & date, bool bRepaint)
{
	// This one appends a KviIrcViewLine to
	// the buffer list (at the end)

	if(m_bMouseIsDown)
	{
		// Do not move the view!
		// So we append the text line to a temp queue
		// and then we'll add it when the mouse button is released
		m_pMessagesStoppedWhileSelecting.push_back(ptr);
		return;
	}

	// First log the line and assign the index
	// Don't use add2log here!...we must go as fast as possible, so we avoid some push and pop calls, and also a couple of branches
	if(m_pLogFile && KVI_OPTION_BOOL(KviOption_boolStripControlCodesInLogs))
	{
		// a slave view has no log files!
		if(KVI_OPTION_MSGTYPE(ptr->iMsgType).logEnabled())
		{
			add2Log(ptr->szText, date, ptr->iMsgType, false);
			// If we fail...this has been already reported!
		}

		// mmh.. when this overflows... we have problems (find doesn't work anymore :()
		// but it overflows at 2^32 lines... 2^32 = 4.294.967.296 lines
		// to spit it out in a year you'd need to print 1360 lines per second... that's insane :D
		// a really fast but reasonable rate of printed lines might be 10 per second
		// thus 429.496.730 seconds would be needed to make this var overflow
		// that means more or less 13 years of text spitting at full rate :D
		// I think that we can safely assume that this will NOT overflow ... your cpu (or you)
		// will get mad before. Well.. it is not that dangerous after all...
		ptr->uIndex = m_uNextLineIndex;
		m_uNextLineIndex++;
	}
	else
	{
		// no log: we could have master view!
		if(m_pMasterView)
		{
			if(m_pMasterView->m_pLogFile && KVI_OPTION_BOOL(KviOption_boolStripControlCodesInLogs))
			{
				if(KVI_OPTION_MSGTYPE(ptr->iMsgType).logEnabled())
					m_pMasterView->add2Log(ptr->szText, date, ptr->iMsgType, false);
			}
			ptr->uIndex = m_pMasterView->m_uNextLineIndex;
			m_pMasterView->m_uNextLineIndex++;
		}
		else
		{
			ptr->uIndex = m_uNextLineIndex;
			m_uNextLineIndex++;
		}
	}

	if(m_pLastLine)
	{
		// There is at least one line in the view
		m_pLastLine->pNext = ptr;
		ptr->pPrev = m_pLastLine;
		ptr->pNext = nullptr;
		m_iNumLines++;

		if(m_iNumLines > m_iMaxLines)
		{
			// Too many lines in the view...remove one
			removeHeadLine();
			if(m_pCurLine == m_pLastLine)
			{
				m_pCurLine = ptr;
				if(bRepaint)
					postUpdateEvent();
			}
			else
			{
				// the cur line remains the same
				// the scroll bar must move up one place to be in sync
				m_bSkipScrollBarRepaint = true;
				if(m_pScrollBar->value() > 0)
				{
					m_iLastScrollBarValue--;
					KVI_ASSERT(m_iLastScrollBarValue >= 0);
					m_pScrollBar->triggerAction(QAbstractSlider::SliderSingleStepSub);
				} // else will stay in sync
				m_bSkipScrollBarRepaint = false;
			}
		}
		else
		{
			// Just append
			m_pScrollBar->setRange(0, m_iNumLines);
			if(m_pCurLine == m_pLastLine)
			{
				m_bSkipScrollBarRepaint = true;
				m_pScrollBar->triggerAction(QAbstractSlider::SliderSingleStepAdd);
				m_bSkipScrollBarRepaint = false;
				if(bRepaint)
					postUpdateEvent();
			}
		}
		m_pLastLine = ptr;
	}
	else
	{
		//First line
		m_pLastLine = ptr;
		m_pFirstLine = ptr;
		m_pCurLine = ptr;
		ptr->pPrev = nullptr;
		ptr->pNext = nullptr;
		m_iNumLines = 1;
		m_pScrollBar->setRange(0, 1);
		m_pScrollBar->triggerAction(QAbstractSlider::SliderSingleStepAdd);
		if(bRepaint)
			postUpdateEvent();
	}
}

//
// removeHeadLine
//

void KviIrcView::removeHeadLine(bool bRepaint)
{
	//Removes the first line of the text buffer
	if(!m_pLastLine)
		return;
	if(m_pFirstLine == m_pCursorLine)
		m_pCursorLine = nullptr;

	if(m_pFirstLine->pNext)
	{
		KviIrcViewLine * aux_ptr = m_pFirstLine->pNext; // get the next line
		aux_ptr->pPrev = nullptr;                       // becomes the first
		if(m_pFirstLine == m_pCurLine)
			m_pCurLine = aux_ptr;                       // move the cur line if necessary
		delete_text_line(m_pFirstLine, &m_hAnimatedSmiles); // delete the struct
		m_pFirstLine = aux_ptr;                             // set the last
		m_iNumLines--;                                      // and decrement the count
	}
	else
	{	// unique line
		m_pCurLine = nullptr;
		delete_text_line(m_pFirstLine, &m_hAnimatedSmiles);
		m_pFirstLine = nullptr;
		m_iNumLines = 0;
		m_pLastLine = nullptr;
	}
	if(bRepaint)
		repaint();
}

bool KviIrcView::messageShouldGoToMessageView(int iMsgType)
{
	switch(iMsgType)
	{
		case KVI_OUT_CHANPRIVMSG:
		case KVI_OUT_CHANPRIVMSGCRYPTED:
		case KVI_OUT_CHANNELNOTICE:
		case KVI_OUT_CHANNELNOTICECRYPTED:
		case KVI_OUT_ACTION:
		case KVI_OUT_ACTIONCRYPTED:
		case KVI_OUT_OWNACTION:
		case KVI_OUT_OWNACTIONCRYPTED:
		case KVI_OUT_OWNPRIVMSG:
		case KVI_OUT_OWNPRIVMSGCRYPTED:
		case KVI_OUT_HIGHLIGHT:
			return true;
			break;
		default:
			break; // fall down
	}

	return false;
}

void KviIrcView::splitMessagesTo(KviIrcView * v)
{
	v->emptyBuffer(false);

	KviIrcViewLine * l = m_pFirstLine;
	KviIrcViewLine * tmp;
	while(l)
	{
		if(messageShouldGoToMessageView(l->iMsgType))
		{
			m_iNumLines--;
			v->m_iNumLines++;

			if(l->pNext)
				l->pNext->pPrev = l->pPrev;
			if(l->pPrev)
				l->pPrev->pNext = l->pNext;
			if(l == m_pFirstLine)
				m_pFirstLine = l->pNext;
			if(l == m_pLastLine)
				m_pLastLine = l->pPrev;
			if(v->m_pLastLine)
			{
				v->m_pLastLine->pNext = l;
				l->pPrev = v->m_pLastLine;
				v->m_pLastLine = l;
			}
			else
			{
				v->m_pFirstLine = l;
				l->pPrev = nullptr;
				v->m_pLastLine = l;
			}
			tmp = l->pNext;
			l->pNext = nullptr;
			l = tmp;
		}
		else
		{
			l = l->pNext;
		}
	}

	v->m_pCurLine = v->m_pLastLine;
	m_pCurLine = m_pLastLine;

	v->m_pCursorLine = nullptr;
	m_pCursorLine = nullptr;

	m_iLastScrollBarValue = m_iNumLines;
	m_pScrollBar->setRange(0, m_iNumLines);
	m_pScrollBar->setValue(m_iNumLines);

	repaint();

	v->m_iLastScrollBarValue = v->m_iNumLines;
	v->m_pScrollBar->setRange(0, v->m_iNumLines);
	v->m_pScrollBar->setValue(v->m_iNumLines);
	v->repaint();
}

void KviIrcView::appendMessagesFrom(KviIrcView * v)
{
	if(!m_pLastLine)
	{
		m_pFirstLine = v->m_pFirstLine;
	}
	else
	{
		m_pLastLine->pNext = v->m_pFirstLine;
		v->m_pFirstLine->pPrev = m_pLastLine;
	}
	m_pLastLine = v->m_pLastLine;
	m_pCurLine = m_pLastLine;
	m_pCursorLine = nullptr;
	v->m_pFirstLine = nullptr;
	v->m_pLastLine = nullptr;
	v->m_pCurLine = nullptr;
	v->m_pCursorLine = nullptr;
	m_iNumLines += v->m_iNumLines;
	v->m_iNumLines = 0;
	//	v->m_pScrollBar->setRange(0,0);
	//	v->m_pScrollBar->setValue(0);
	m_iLastScrollBarValue = m_iNumLines;
	m_pScrollBar->setRange(0, m_iNumLines);
	m_pScrollBar->setValue(m_iNumLines);

	repaint();
}

void KviIrcView::joinMessagesFrom(KviIrcView * v)
{
	KviIrcViewLine * l1 = m_pFirstLine;
	KviIrcViewLine * l2 = v->m_pFirstLine;
	KviIrcViewLine * tmp;

	while(l2)
	{
		if(l1)
		{
			if(l2->uIndex < l1->uIndex)
			{
				// the external message is older than the current internal one
				l2->pPrev = l1->pPrev;
				if(l1->pPrev)
					l1->pPrev->pNext = l2;
				else
					m_pFirstLine = l2;
				l1->pPrev = l2;
				tmp = l2->pNext;
				l2->pNext = l1;
				l2 = tmp;
			}
			else
			{
				// the external message is younger than the current internal one
				l1 = l1->pNext;
			}
		}
		else
		{
			// There is no current internal message (ran over the end)
			// merge at the end then
			if(m_pFirstLine)
			{
				m_pLastLine->pNext = l2;
				l2->pPrev = m_pLastLine;
			}
			else
			{
				m_pFirstLine = l2;
				l2->pPrev = nullptr;
			}
			tmp = l2->pNext;
			l2->pNext = nullptr;
			m_pLastLine = l2;
			l2 = tmp;
		}
	}

	m_pCurLine = m_pLastLine;
	m_pCursorLine = nullptr;
	v->m_pFirstLine = nullptr;
	v->m_pLastLine = nullptr;
	v->m_pCurLine = nullptr;
	v->m_pCursorLine = nullptr;
	m_iNumLines += v->m_iNumLines;
	v->m_iNumLines = 0;
	//	v->m_pScrollBar->setRange(0,0);
	//	v->m_pScrollBar->setValue(0);
	m_iLastScrollBarValue = m_iNumLines;
	m_pScrollBar->setRange(0, m_iNumLines);
	m_pScrollBar->setValue(m_iNumLines);

	repaint();
}

void KviIrcView::getLinkEscapeCommand(QString & buffer, const QString & szPayload, const QString & escape_label)
{
	if(szPayload.isEmpty())
		return;

	int idx = szPayload.indexOf(escape_label, Qt::CaseInsensitive);
	if(idx == -1)
		return;
	idx += escape_label.length();

	int idx2 = szPayload.indexOf("[!", idx, Qt::CaseInsensitive);
	int len = idx2 == -1 ? szPayload.length() - idx : idx2 - idx;

	buffer = szPayload.mid(idx, len);
}

void KviIrcView::fastScroll(int lines)
{
	m_iUnprocessedPaintEventRequests = 0;

#ifdef COMPILE_ON_MAC
	// fastScroll() is currently broken for macosx, ticket #791
	update();
	return;
#endif

	if(!isVisible())
		return;

	if(!m_pFm)
	{
		repaint(); // We must get the metrics from a real paint event :/
		return;	   // must do a full repaint to get them..
	}

	// Ok...the current line is the last one here
	// It is the only one that needs to be repainted
	int widgetWidth = width() - m_pScrollBar->width();

	if(widgetWidth < KVI_IRCVIEW_PIXMAP_AND_SEPARATOR + KVI_IRCVIEW_DOUBLEBORDER_WIDTH + 10)
		return; // can't show stuff here

	int widgetHeight = height();
	int maxLineWidth = widgetWidth - KVI_IRCVIEW_DOUBLEBORDER_WIDTH;

	if(KVI_OPTION_BOOL(KviOption_boolIrcViewShowImages))
	{
		maxLineWidth -= KVI_IRCVIEW_PIXMAP_AND_SEPARATOR;
	}

	int heightToPaint = 1;
	KviIrcViewLine * l = m_pCurLine;
	while(lines > 0)
	{
		if(l)
		{
			if(maxLineWidth != l->iMaxLineWidth)
				calculateLineWraps(l, maxLineWidth);
			heightToPaint += l->uLineWraps * m_iFontLineSpacing;
			heightToPaint += (m_iFontLineSpacing + m_iFontDescent);
			lines--;
			l = l->pPrev;
		}
		else
			lines = 0;
	}

	scroll(0, -(heightToPaint - 1), QRect(1, 1, widgetWidth - 2, widgetHeight - 2));

	if(m_iLastLinkRectHeight > -1)
	{	// need to kill the last highlighted link
		m_iLastLinkRectTop -= heightToPaint;
		if(m_iLastLinkRectTop < 0)
		{
			m_iLastLinkRectHeight += m_iLastLinkRectTop;
			m_iLastLinkRectTop = 0;
		}
	}
}

//
// The IrcView : THE paint event
//

void KviIrcView::paintEvent(QPaintEvent * p)
{
	// THIS FUNCTION IS A MONSTER

	/*
	 * Profane description: this is ircview's most important function. It takes a lot of cpu cycles to complete, so we want to be sure
	 * it's well optimized. First, we want to skip this method every time it's useless: it we're too short or we're covered by other windows.
	 */

	if(!isVisible())
	{
		m_iUnprocessedPaintEventRequests = 0; // assume a full repaint when this widget is shown...
		return;                               // can't show stuff here
	}

	int scrollbarWidth = m_pScrollBar->width();
	int toolWidgetHeight = (m_pToolWidget && m_pToolWidget->isVisible()) ? m_pToolWidget->sizeHint().height() : 0;
	int widgetWidth = width() - scrollbarWidth;
	int widgetHeight = height() - toolWidgetHeight;

	static QRect r; // static: avoid calling constructor and destructor every time...

	if(p)
	{
		r = p->rect(); // app triggered, or self triggered from fastScroll (in that case m_iUnprocessedPaintEventRequests is set to 0 there)
		if(r == rect())
			m_iUnprocessedPaintEventRequests = 0; // only full repaints reset
	}
	else
	{
		// A self triggered event
		m_iUnprocessedPaintEventRequests = 0; // only full repaints reset
		r = rect();
	}

	/*
	 * Profane description: we start the real paint here: set some geometry, a font, and paint the background
	 */
	int rectTop = r.y();
	int rectHeight = r.height();
	int rectBottom = rectTop + rectHeight;

	QFont newFont;

	QPainter pa(this);

	SET_ANTI_ALIASING(pa);

	pa.setFont(font());
	QFont::Style normalFontStyle = pa.font().style();

	if(!m_pFm)
	{
		// note that QFontMetrics(pa.font()) may be not the same as QFontMetrics(font())
		// because the painter might effectively use an approximation of the QFont specified
		// by font().
		recalcFontVariables(pa.font(), pa.fontInfo());
	}

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency) && g_pApp->supportsCompositing())
	{
		pa.save();
		pa.setCompositionMode(QPainter::CompositionMode_Source);
		QColor col = KVI_OPTION_COLOR(KviOption_colorGlobalTransparencyFade);
		col.setAlphaF((float)((float)KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) / (float)100));
		pa.fillRect(rect(), col);
		pa.restore();
	}
	else if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt = m_pKviWindow->isDocked() ? mapTo(g_pMainWindow, r.topLeft()) : mapTo(m_pKviWindow, r.topLeft());
		pa.drawTiledPixmap(r, *(g_pShadedChildGlobalDesktopBackground), pnt);
	}
	else
	{
#endif
		pa.fillRect(r, KVI_OPTION_COLOR(KviOption_colorIrcViewBackground));

		QPixmap * pix = m_pPrivateBackgroundPixmap;

		if(!pix)
			pix = KVI_OPTION_PIXMAP(KviOption_pixmapIrcViewBackground).pixmap();
		if(pix)
			KviPixmapUtils::drawPixmapWithPainter(&pa, pix, KVI_OPTION_UINT(KviOption_uintIrcViewPixmapAlign), r, widgetWidth, widgetHeight);
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	}
#endif

	if(widgetWidth < 20)
	{
		m_iUnprocessedPaintEventRequests = 0; // assume a full repaint when this widget is shown...
		return;                               // can't show stuff here
	}

	/*
	 * Profane description: after the background, start to paint the contents (a list of text lines with "dynamic contents", correctly
	 * wrapped at the right edge of this control).
	 */

	// Have lines visible
	int curBottomCoord = widgetHeight - KVI_IRCVIEW_VERTICAL_BORDER;
	int maxLineWidth = widgetWidth - KVI_IRCVIEW_DOUBLEBORDER_WIDTH;
	int defLeftCoord = KVI_IRCVIEW_HORIZONTAL_BORDER;
	int lineWrapsHeight;

	// if we draw an icon as a line preamble, we have to change borders geometry accordingly
	if(KVI_OPTION_BOOL(KviOption_boolIrcViewShowImages))
	{
		maxLineWidth -= KVI_IRCVIEW_PIXMAP_AND_SEPARATOR;
		defLeftCoord += KVI_IRCVIEW_PIXMAP_AND_SEPARATOR;
	}

	KviIrcViewLine * pCurTextLine = m_pCurLine;

	// Make sure that we have enough space to paint something...
	if(maxLineWidth < m_iMinimumPaintWidth)
		pCurTextLine = nullptr;

	bool bLineMarkPainted = !KVI_OPTION_BOOL(KviOption_boolTrackLastReadTextViewLine);
	int iLinesPerPage = 0;

	// And loop through lines until we not run over the upper bound of the view
	while((curBottomCoord >= KVI_IRCVIEW_VERTICAL_BORDER) && pCurTextLine)
	{
		// Paint pCurTextLine
		if(maxLineWidth != pCurTextLine->iMaxLineWidth)
		{
			// Width of the widget or the font has been changed
			// from the last time that this line was painted
			calculateLineWraps(pCurTextLine, maxLineWidth);
		}

		// the evil multiplication
		// in an i486 it can get up to 42 clock cycles
		lineWrapsHeight = (pCurTextLine->uLineWraps) * m_iFontLineSpacing;
		curBottomCoord -= lineWrapsHeight;

		if((curBottomCoord - m_iFontLineSpacing) > rectBottom)
		{
			// not in update rect... skip
			curBottomCoord -= (m_iFontLineSpacing + m_iFontDescent);
			pCurTextLine = pCurTextLine->pPrev;
			continue;
		}

		if(KVI_OPTION_BOOL(KviOption_boolIrcViewShowImages))
		{
			// Paint the pixmap first
			// Calculate the position of the image
			//imageYPos = curBottomCoord - (pixmapHeight(16) + ((m_iFontLineSpacing - 16)/2) );
			int imageYPos = curBottomCoord - m_iRelativePixmapY;
			//Set the mask if needed
			int iPixId = KVI_OPTION_MSGTYPE(pCurTextLine->iMsgType).pixId();
			if(iPixId > 0)
				pa.drawPixmap(KVI_IRCVIEW_HORIZONTAL_BORDER, imageYPos, *(g_pIconManager->getSmallIcon(iPixId)));
		}

		// Initialize for drawing this line of text
		// The first block is always an attribute block
		char defaultBack = pCurTextLine->pBlocks->pChunk->colors.back;
		char defaultFore = pCurTextLine->pBlocks->pChunk->colors.fore;
		bool curBold = false;
		bool curItalic = false;
		bool curUnderline = false;
		char foreBeforeEscape = KviControlCodes::Black;
		bool curLink = false;
		bool bacWasTransp = false;
		char curFore = defaultFore;
		char curBack = defaultBack;
		int curLeftCoord = defLeftCoord;
		curBottomCoord -= m_iFontDescent; //rise up the text...

		//
		// Single text line loop (paint all text blocks)
		// (May correspond to more physical lines on the display if the text is wrapped)
		//

		for(int i = 0; i < pCurTextLine->iBlockCount; i++)
		{
			KviIrcViewWrappedBlock * block = &(pCurTextLine->pBlocks[i]);

			// Play with the attributes
			if(block->pChunk)
			{
				// normal block
				switch(block->pChunk->type)
				{
					case KviControlCodes::Color:
						if(block->pChunk->colors.fore != KviControlCodes::NoChange)
						{
							curFore = block->pChunk->colors.fore;
							if(block->pChunk->colors.back != KviControlCodes::NoChange)
								curBack = block->pChunk->colors.back;
						}
						else
						{
							/*
							* When KVIrc encounters a Ctrl+K code without any trailing numbers, we then
							* use KVIrc's default color value defined by the user in the Options dialog.
							*/
							curFore = defaultFore;
							curBack = defaultBack;
						}
						break;
					case KviControlCodes::Escape:
						foreBeforeEscape = curFore;
						if(block->pChunk->colors.fore != KviControlCodes::NoChange)
							curFore = block->pChunk->colors.fore;
						if(m_pLastLinkUnderMouse == block)
							curLink = true;
						break;
					case KviControlCodes::UnEscape:
						curLink = false;
						curFore = foreBeforeEscape;
						break;
					case KviControlCodes::Bold:
						curBold = !curBold;
						break;
					case KviControlCodes::Italic:
						curItalic = !curItalic;
						break;
					case KviControlCodes::Underline:
						curUnderline = !curUnderline;
						break;
					case KviControlCodes::Reset:
						curBold = false;
						curItalic = false;
						curUnderline = false;
						bacWasTransp = false;
						curFore = defaultFore;
						curBack = defaultBack;
						break;
					case KviControlCodes::Reverse:
						// this should be "reversed colors"
						char aux = curBack;
						if(bacWasTransp)
						{
							curBack = KviControlCodes::Transparent;
						}
						else
						{
							curBack = curFore;
						}
						if(aux == KviControlCodes::Transparent)
						{
							curFore = (char)KVI_DEF_BACK;
						}
						else
						{
							curFore = aux;
						}
						bacWasTransp = (aux == KviControlCodes::Transparent);
						break;
					//case KviControlCodes::Icon:
					//case KviControlCodes::UnIcon:
						// does nothing
						//qDebug("Have a block with ICON/UNICON attr");
						//break;
				}
			}
			else
			{
				// no attributes, it is a line wrap
				curLeftCoord = defLeftCoord;
				if(KVI_OPTION_BOOL(KviOption_boolIrcViewWrapMargin))
					curLeftCoord += m_iWrapMargin;
				curBottomCoord += m_iFontLineSpacing;
			}

//
// Here we run really out of bounds :)))))
// A couple of macros that could work well as functions...
// but since there are really many params to be passed
// and push & pop calls take clock cycles
// my paranoid mind decided to go for the macro way.
// This is NOT good programming
//

#define SET_PEN(_color, _custom)                                                             \
	if(((unsigned char)_color) <= KVI_EXTCOLOR_MAX)                                          \
	{                                                                                        \
		pa.setPen(getMircColor((unsigned char)_color));                                      \
	}                                                                                        \
	else                                                                                     \
	{                                                                                        \
		switch((unsigned char)_color)                                                        \
		{                                                                                    \
			case KVI_COLOR_EXT_USER_OP:                                                      \
				pa.setPen(KVI_OPTION_COLOR(KviOption_colorUserListViewOpForeground));        \
				break;                                                                       \
			case KVI_COLOR_EXT_USER_HALFOP:                                                  \
				pa.setPen(KVI_OPTION_COLOR(KviOption_colorUserListViewHalfOpForeground));    \
				break;                                                                       \
			case KVI_COLOR_EXT_USER_ADMIN:                                                   \
				pa.setPen(KVI_OPTION_COLOR(KviOption_colorUserListViewChanAdminForeground)); \
				break;                                                                       \
			case KVI_COLOR_EXT_USER_OWNER:                                                   \
				pa.setPen(KVI_OPTION_COLOR(KviOption_colorUserListViewChanOwnerForeground)); \
				break;                                                                       \
			case KVI_COLOR_EXT_USER_VOICE:                                                   \
				pa.setPen(KVI_OPTION_COLOR(KviOption_colorUserListViewVoiceForeground));     \
				break;                                                                       \
			case KVI_COLOR_EXT_USER_USEROP:                                                  \
				pa.setPen(KVI_OPTION_COLOR(KviOption_colorUserListViewUserOpForeground));    \
				break;                                                                       \
			case KVI_COLOR_EXT_USER_NORMAL:                                                  \
				pa.setPen(KVI_OPTION_COLOR(KviOption_colorUserListViewNormalForeground));    \
				break;                                                                       \
			case KVI_DEF_BACK:                                                               \
				pa.setPen(KVI_OPTION_COLOR(KviOption_colorIrcViewBackground));               \
				break;                                                                       \
			case KVI_COLOR_CUSTOM:                                                           \
				pa.setPen(_custom);                                                          \
				break;                                                                       \
			case KVI_COLOR_OWN:                                                              \
				pa.setPen(KVI_OPTION_COLOR(KviOption_colorUserListViewOwnForeground));       \
				break;                                                                       \
		}                                                                                    \
	}

#define DRAW_SELECTED_TEXT(_text_str, _text_idx, _text_len, _text_width)                                                                                                               \
	SET_PEN(KVI_OPTION_MSGTYPE(KVI_OUT_SELECT).fore(), block->pChunk ? block->pChunk->customFore : QColor());                                                                          \
	{                                                                                                                                                                                  \
		int theWdth = _text_width;                                                                                                                                                     \
		if(theWdth < 0)                                                                                                                                                                \
			theWdth = width() - (curLeftCoord + KVI_IRCVIEW_HORIZONTAL_BORDER + scrollbarWidth);                                                                                       \
		pa.fillRect(curLeftCoord, curBottomCoord - m_iFontLineSpacing + m_iFontDescent, theWdth, m_iFontLineSpacing, getMircColor(KVI_OPTION_MSGTYPE(KVI_OUT_SELECT).back())); \
	}                                                                                                                                                                                  \
	pPenFont.setStyle(curItalic ^ (normalFontStyle != QFont::StyleNormal) ? QFont::StyleItalic : QFont::StyleNormal);                                                 \
	if (m_bUseRealBold)                                                                                                                                             \
		pPenFont.setBold(curBold);                                                                                                                                  \
	pa.setFont(pPenFont);                                                                                                                                           \
	pa.drawText(curLeftCoord, curBottomCoord, _text_str.mid(_text_idx, _text_len));                                                                                 \
	if(curBold && !m_bUseRealBold)                                                                                                                                  \
		pa.drawText(curLeftCoord + 1, curBottomCoord, _text_str.mid(_text_idx, _text_len));                                                                         \
	curLeftCoord += _text_width;

#define DRAW_NORMAL_TEXT(_text_str, _text_idx, _text_len, _text_width)                                                                                              \
	SET_PEN(curFore, block->pChunk ? block->pChunk->customFore : QColor());                                                                                         \
	if(curBack != KviControlCodes::Transparent)                                                                                                                     \
	{                                                                                                                                                               \
		int theWdth = _text_width;                                                                                                                                  \
		if(theWdth < 0)                                                                                                                                             \
			theWdth = width() - (curLeftCoord + KVI_IRCVIEW_HORIZONTAL_BORDER + scrollbarWidth);                                                                    \
		pa.fillRect(curLeftCoord, curBottomCoord - m_iFontLineSpacing + m_iFontDescent, theWdth, m_iFontLineSpacing, getMircColor((unsigned char)curBack));         \
	}                                                                                                                                                               \
	pPenFont.setStyle(curItalic ^ (normalFontStyle != QFont::StyleNormal) ? QFont::StyleItalic : QFont::StyleNormal);                                                 \
	if (m_bUseRealBold)                                                                                                                                             \
	    pPenFont.setBold(curBold);                                                                                                                                  \
	pa.setFont(pPenFont);                                                                                                                                           \
	pa.drawText(curLeftCoord, curBottomCoord, _text_str.mid(_text_idx, _text_len));                                                                                 \
	if(curBold && !m_bUseRealBold)                                                                                                                                  \
		pa.drawText(curLeftCoord + 1, curBottomCoord, _text_str.mid(_text_idx, _text_len));                                                                         \
	if(curUnderline)                                                                                                                                                \
	{                                                                                                                                                               \
		int theWdth = _text_width;                                                                                                                                  \
		if(theWdth < 0)                                                                                                                                             \
			theWdth = width() - (curLeftCoord + KVI_IRCVIEW_HORIZONTAL_BORDER + scrollbarWidth);                                                                    \
		pa.drawLine(curLeftCoord, curBottomCoord + 2, curLeftCoord + theWdth, curBottomCoord + 2);                                                                  \
	}                                                                                                                                                               \
	curLeftCoord += _text_width;

	// EOF macro declarations

			if(pCurTextLine == m_pCursorLine)
			{
				// this line is currently highlighted by the ircview "find" method.
				curBack = KVI_OPTION_MSGTYPE(KVI_OUT_SEARCH).back();
				curFore = KVI_OPTION_MSGTYPE(KVI_OUT_SEARCH).fore();
			}

			if(m_bMouseIsDown)
			{
				QFont pPenFont = pa.font();
				// Check if the block or a part of it is selected
				if(checkSelectionBlock(pCurTextLine, i))
				{
					switch(m_pWrappedBlockSelectionInfo->selection_type)
					{
						case KVI_IRCVIEW_BLOCK_SELECTION_TOTAL:
							DRAW_SELECTED_TEXT(pCurTextLine->szText, block->block_start,
							    block->block_len, block->block_width)
							break;
						case KVI_IRCVIEW_BLOCK_SELECTION_LEFT:
							DRAW_SELECTED_TEXT(pCurTextLine->szText, block->block_start,
							    m_pWrappedBlockSelectionInfo->part_1_length,
							    m_pWrappedBlockSelectionInfo->part_1_width)
							DRAW_NORMAL_TEXT(pCurTextLine->szText, block->block_start + m_pWrappedBlockSelectionInfo->part_1_length,
							    m_pWrappedBlockSelectionInfo->part_2_length,
							    m_pWrappedBlockSelectionInfo->part_2_width)
							break;
						case KVI_IRCVIEW_BLOCK_SELECTION_RIGHT:
							DRAW_NORMAL_TEXT(pCurTextLine->szText, block->block_start,
							    m_pWrappedBlockSelectionInfo->part_1_length,
							    m_pWrappedBlockSelectionInfo->part_1_width)
							DRAW_SELECTED_TEXT(pCurTextLine->szText, block->block_start + m_pWrappedBlockSelectionInfo->part_1_length,
							    m_pWrappedBlockSelectionInfo->part_2_length,
							    m_pWrappedBlockSelectionInfo->part_2_width)
							break;
						case KVI_IRCVIEW_BLOCK_SELECTION_CENTRAL:
							DRAW_NORMAL_TEXT(pCurTextLine->szText, block->block_start,
							    m_pWrappedBlockSelectionInfo->part_1_length,
							    m_pWrappedBlockSelectionInfo->part_1_width)
							DRAW_SELECTED_TEXT(pCurTextLine->szText, block->block_start + m_pWrappedBlockSelectionInfo->part_1_length,
							    m_pWrappedBlockSelectionInfo->part_2_length,
							    m_pWrappedBlockSelectionInfo->part_2_width)
							DRAW_NORMAL_TEXT(pCurTextLine->szText, block->block_start + m_pWrappedBlockSelectionInfo->part_1_length + m_pWrappedBlockSelectionInfo->part_2_length,
							    m_pWrappedBlockSelectionInfo->part_3_length,
							    m_pWrappedBlockSelectionInfo->part_3_width)
							break;
						case KVI_IRCVIEW_BLOCK_SELECTION_ICON:
						{
							int theWdth = block->block_width;
							if(theWdth < 0)
								theWdth = width() - (curLeftCoord + KVI_IRCVIEW_HORIZONTAL_BORDER + scrollbarWidth);
							pa.fillRect(curLeftCoord, curBottomCoord - m_iFontLineSpacing + m_iFontDescent, theWdth, m_iFontLineSpacing,
								getMircColor(KVI_OPTION_MSGTYPE(KVI_OUT_SELECT).back()));
							goto no_selection_paint;
						}
						break;
					}
				}
				else
				{
					if(block->pChunk && block->pChunk->type == KviControlCodes::Icon)
						goto no_selection_paint;
					int wdth = block->block_width;
					if(wdth == 0)
					{
						// Last block before a word wrap, or a zero characters attribute block ?
						if(i < (pCurTextLine->iBlockCount - 1))
						{
							// There is another block...
							// Check if it is a wrap...
							if(pCurTextLine->pBlocks[i + 1].pChunk == nullptr)
								wdth = widgetWidth - (curLeftCoord + KVI_IRCVIEW_HORIZONTAL_BORDER);
						}
						// else simply a zero characters block
					}
					DRAW_NORMAL_TEXT(pCurTextLine->szText, block->block_start, block->block_len, wdth)
				}
			}
			else
			{
			// No selection ...go fast!
			no_selection_paint:
				if(block->pChunk && block->pChunk->type == KviControlCodes::Icon)
				{
					int wdth = block->block_width;
					if(wdth < 0)
						wdth = widgetWidth - (curLeftCoord + KVI_IRCVIEW_HORIZONTAL_BORDER);
					int imageYPos = curBottomCoord - m_iRelativePixmapY;
					// Set the mask if needed
					if(curBack != KviControlCodes::Transparent && curBack <= KVI_EXTCOLOR_MAX)
					{
						pa.fillRect(curLeftCoord, curBottomCoord - m_iFontLineSpacing + m_iFontDescent, wdth, m_iFontLineSpacing, getMircColor((unsigned char)curBack));
					}
					QString tmpQ;
					tmpQ.setUtf16(block->pChunk->szSmileId, kvi_wstrlen(block->pChunk->szSmileId));
					QPixmap * daIcon = nullptr;
					KviTextIcon * pIcon = g_pTextIconManager->lookupTextIcon(tmpQ);
					if(pIcon)
					{
						daIcon = pIcon->animatedPixmap() ? pIcon->animatedPixmap()->pixmap() : pIcon->pixmap();
					}
					if(!daIcon)
					{
						// this should never happen since we do a check
						// when building the text icon block, but.. better safe than sorry:
						// so... we lost some icons ? wrong associations ?
						// recover it by displaying the "question mark" icon
						daIcon = g_pIconManager->getSmallIcon(KviIconManager::Help); // must be there, eventually null pixmap :D
					}
					int moredown = 1; // used to center image vertically (pixels which the image is moved more down)
					moredown += ((m_iFontLineSpacing - daIcon->height()) / 2);
					pa.drawPixmap(curLeftCoord + m_iIconSideSpacing, imageYPos + moredown, *(daIcon));

					//qDebug("SHifting by %d",block->block_width);
					curLeftCoord += block->block_width;
				}
				else
				{
					int wdth = block->block_width;
					if(wdth < 0)
						wdth = widgetWidth - (curLeftCoord + KVI_IRCVIEW_HORIZONTAL_BORDER);

					// FIXME: We could avoid this XSetForeground if the curFore was not changed....

					SET_PEN(curFore, block->pChunk ? block->pChunk->customFore : QColor());

					if(curBack != KviControlCodes::Transparent && curBack <= KVI_EXTCOLOR_MAX)
					{
						pa.fillRect(curLeftCoord, curBottomCoord - m_iFontLineSpacing + m_iFontDescent, wdth, m_iFontLineSpacing, getMircColor((unsigned char)curBack));
					}

					bool bBold = curBold || curLink;

					newFont = pa.font();
					newFont.setStyle(curItalic ^ (normalFontStyle != QFont::StyleNormal) ? QFont::StyleItalic : QFont::StyleNormal);
					if (m_bUseRealBold)
						newFont.setBold(bBold);
					pa.setFont(newFont);

					if(curLink)
					{
						SET_PEN(KVI_OPTION_MSGTYPE(KVI_OUT_LINK).fore(), block->pChunk ? block->pChunk->customFore : QColor());
						pa.drawLine(curLeftCoord, curBottomCoord + 2, curLeftCoord + wdth, curBottomCoord + 2);
					}

					pa.drawText(curLeftCoord, curBottomCoord, pCurTextLine->szText.mid(block->block_start, block->block_len));

					if (bBold && !m_bUseRealBold)
					{
						// Draw doubled font (simulate bold)
						pa.drawText(curLeftCoord + 1, curBottomCoord, pCurTextLine->szText.mid(block->block_start, block->block_len));
					}
					if(curUnderline)
					{
						// Draw a line under the text block....
						pa.drawLine(curLeftCoord, curBottomCoord + 2, curLeftCoord + wdth, curBottomCoord + 2);
					}
					curLeftCoord += block->block_width;
				}
			}
		}

		curBottomCoord -= (lineWrapsHeight + m_iFontLineSpacing);

		// paint the "last read line marker"
		if(pCurTextLine->uIndex == m_uLineMarkLineIndex)
		{
			if((curBottomCoord >= KVI_IRCVIEW_VERTICAL_BORDER) && !bLineMarkPainted)
			{
				// visible!
				bLineMarkPainted = true;
				//pa.setRasterOp(NotROP);

				// Pen setup for marker line
				QPen pen(KVI_OPTION_COLOR(KviOption_colorIrcViewMarkLine), KVI_OPTION_UINT(KviOption_uintIrcViewMarkerSize));

				switch(KVI_OPTION_UINT(KviOption_uintIrcViewMarkerStyle))
				{
					case 1:
						pen.setStyle(Qt::DashLine);
						break;
					case 2:
						pen.setStyle(Qt::SolidLine);
						break;
					case 3:
						pen.setStyle(Qt::DashDotLine);
						break;
					case 4:
						pen.setStyle(Qt::DashDotDotLine);
						break;
					default:
						pen.setStyle(Qt::DotLine);
				}

				pa.setPen(pen);
				pa.drawLine(0, curBottomCoord, widgetWidth, curBottomCoord);
				//pa.setRasterOp(CopyROP);
			}       // else was partially visible only
		}

		pCurTextLine = pCurTextLine->pPrev;
		iLinesPerPage++;
	}

	/* REMINDER
	 * Try to get the current number of KviIrcViewLines from the paintEvent and set the m_pScrollBar's
	 * pageStep accordingly; the calculated value is valid only:
	 * if there are enough lines to fill up at least a page (pCurTextLine)
	 * if the value is safe (iLinesPerPage > 0)
	 * if it's different from the actual one (iLinesPerPage != m_pScrollBar->pageStep())
	 * only on full repaints: rectHeight == rect().height()
	 */
	if(pCurTextLine && iLinesPerPage > 0 && iLinesPerPage != m_pScrollBar->pageStep() && rectHeight == rect().height())
		m_pScrollBar->setPageStep(iLinesPerPage);

	if(!bLineMarkPainted && pCurTextLine && (rectTop <= (KVI_IRCVIEW_VERTICAL_BORDER + 5)))
	{
		// the line mark hasn't been painted yet
		// need to find out if the mark is above the display
		// the mark might be somewhere before the current text line
		// find the first line that can't be painted in the view at all
		while((curBottomCoord >= KVI_IRCVIEW_VERTICAL_BORDER) && pCurTextLine)
		{
			// the line wraps for the visible lines MUST have been already calculated
			// for this view width
			lineWrapsHeight = (pCurTextLine->uLineWraps) * m_iFontLineSpacing;
			curBottomCoord -= lineWrapsHeight + m_iFontLineSpacing + m_iFontDescent;
			pCurTextLine = pCurTextLine->pPrev;
		}

		if(pCurTextLine)
		{
			// this is the first NOT visible
			// so pCurTextLine->pNext is the last visible one
			if(pCurTextLine->pNext)
			{
				if(pCurTextLine->pNext->uIndex >= m_uLineMarkLineIndex)
					bLineMarkPainted = true; // yes, its somewhere before or on this line
			}
			else
			{
				// no next line ? hm... compare to the not visible one.. but this should never happen
				if(pCurTextLine->uIndex >= m_uLineMarkLineIndex)
					bLineMarkPainted = true; // yes, its somewhere before or on this line
			}
			if(bLineMarkPainted)
			{
				// need to mark it!
				//pa.setRasterOp(NotROP);
				pa.setPen(QPen(KVI_OPTION_COLOR(KviOption_colorIrcViewMarkLine), 1, Qt::DotLine));

				// Marker icon
				// 16(width) + 5(border) = 21
				int x = widgetWidth - 21;
				int y = KVI_IRCVIEW_VERTICAL_BORDER;
				/*
				* Old icon... what a lame code :D
				* pa.drawLine(x,y,x,y);
				* y++; pa.drawLine(x-1,y,x+1,y);
				* y++; pa.drawLine(x-2,y,x+2,y);
				* y++; pa.drawLine(x-3,y,x+3,y);
				* y++; pa.drawLine(x-4,y,x+4,y);
				*/
				QPixmap * pIcon = g_pIconManager->getSmallIcon(KviIconManager::UnreadText);
				m_lineMarkArea = QRect(x, y, 16, 16);
				pa.drawPixmap(x, y, 16, 16, *pIcon);
				//pa.setRasterOp(CopyROP);
			}
			else
			{
				m_lineMarkArea = QRect();
			}
		}
	}

	// Need to draw the sunken rect around the view now...
	pa.setPen(palette().dark().color());
	pa.drawLine(0, 0, widgetWidth, 0);
	pa.drawLine(0, 0, 0, widgetHeight);
	pa.setPen(palette().light().color());
	widgetWidth--;
	pa.drawLine(1, widgetHeight - 1, widgetWidth, widgetHeight - 1);
	pa.drawLine(widgetWidth, 1, widgetWidth, widgetHeight);
}

//
// The IrcView : calculate line wraps
//

#define IRCVIEW_WCHARWIDTH(c) (((c).unicode() < 0xff) ? m_iFontCharacterWidth[(c).unicode()] : m_pFm->width(c))

void KviIrcView::calculateLineWraps(KviIrcViewLine * ptr, int maxWidth)
{
	// Another monster
	if(maxWidth <= m_iIconWidth)
		return;

	if(ptr->iBlockCount != 0)
		KviMemory::free(ptr->pBlocks); // free any previous wrap blocks

	ptr->pBlocks = (KviIrcViewWrappedBlock *)KviMemory::allocate(sizeof(KviIrcViewWrappedBlock)); // alloc one block
	ptr->iMaxLineWidth = maxWidth;                                                                // calculus for this width
	ptr->iBlockCount = 0;                                                                         // it will be ++
	ptr->uLineWraps = 0;                                                                          // no line wraps yet

	unsigned int curAttrBlock = 0; // Current attribute block
	int curLineWidth = 0;

	// init the first block
	ptr->pBlocks->block_start = 0;
	ptr->pBlocks->block_len = 0;
	ptr->pBlocks->block_width = 0;
	ptr->pBlocks->pChunk = &(ptr->pChunks[0]); // always an attribute block

	int maxBlockLen = ptr->pChunks->iTextLen; // ptr->pChunks[0].iTextLen

	const QChar * unicode = ptr->szText.unicode();

	for(;;)
	{
		// Calculate the block_width
		const QChar * p = unicode + ptr->pBlocks[ptr->iBlockCount].block_start;

		int curBlockLen = 0;
		int curBlockWidth = 0;

		if(ptr->pChunks[curAttrBlock].type == KviControlCodes::Icon)
		{
			curBlockWidth = m_iIconWidth;
		}
		else
		{
			while(curBlockLen < maxBlockLen)
			{
				// FIXME: this is ugly :/
				curBlockWidth += IRCVIEW_WCHARWIDTH(*p);
				curBlockLen++;
				p++;
			}
		}

		// Check the length
		curLineWidth += curBlockWidth;

		if(curLineWidth < maxWidth)
		{
			// Ok....proceed to next block
			ptr->pBlocks[ptr->iBlockCount].block_len = curBlockLen;
			ptr->pBlocks[ptr->iBlockCount].block_width = curBlockWidth;
			curAttrBlock++;
			ptr->iBlockCount++;

			// if we have no more blocks, return (with is ok)
			if(curAttrBlock >= ptr->uChunkCount)
				return;

			// Process the next block of data in the next loop
			ptr->pBlocks = (KviIrcViewWrappedBlock *)KviMemory::reallocate(ptr->pBlocks, (ptr->iBlockCount + 1) * sizeof(KviIrcViewWrappedBlock));
			ptr->pBlocks[ptr->iBlockCount].block_start = ptr->pChunks[curAttrBlock].iTextStart;
			ptr->pBlocks[ptr->iBlockCount].block_len = 0;
			ptr->pBlocks[ptr->iBlockCount].block_width = 0;
			ptr->pBlocks[ptr->iBlockCount].pChunk = &(ptr->pChunks[curAttrBlock]);
			maxBlockLen = ptr->pBlocks[ptr->iBlockCount].pChunk->iTextLen;

			continue;
		}

		// Need word wrap
		// First go back to an admissible width
		while((curLineWidth >= maxWidth) && (curBlockLen > 0))
		{
			p--;
			curBlockLen--;
			curLineWidth -= IRCVIEW_WCHARWIDTH(*p);
		}

		// Now look for a space (or a tabulation)
		while((p->unicode() != ' ') && (p->unicode() != '\t') && (curBlockLen > 0))
		{
			p--;
			curBlockLen--;
			curLineWidth -= IRCVIEW_WCHARWIDTH(*p);
		}

		if(curBlockLen == 0)
		{
			// ran up to the beginning of the block....
			if(ptr->pChunks[curAttrBlock].type == KviControlCodes::Icon)
			{
				// FIXME what if the icon curBlockWidth is > maxWidth ? => endless loop
				// This is an icon block: needs to be wrapped differently:
				// The wrap block goes BEFORE the icon itself
				ptr->pBlocks[ptr->iBlockCount].pChunk = nullptr;
				ptr->pBlocks[ptr->iBlockCount].block_width = 0;
				ptr->iBlockCount++;
				ptr->pBlocks = (KviIrcViewWrappedBlock *)KviMemory::reallocate(ptr->pBlocks, (ptr->iBlockCount + 1) * sizeof(KviIrcViewWrappedBlock));
				ptr->pBlocks[ptr->iBlockCount].block_start = p - unicode;
				ptr->pBlocks[ptr->iBlockCount].block_len = 0;
				ptr->pBlocks[ptr->iBlockCount].block_width = 0;
				ptr->pBlocks[ptr->iBlockCount].pChunk = &(ptr->pChunks[curAttrBlock]);
				goto wrap_line;
			}
			// Don't like it....forced wrap here...
			// Go ahead up to the biggest possible string
			if(maxBlockLen > 0)
			{
				// avoid a loop when IRCVIEW_WCHARWIDTH(*p) > maxWidth
				uint uLoopedChars = 0;
				do
				{
					curBlockLen++;
					p++;
					curLineWidth += IRCVIEW_WCHARWIDTH(*p);
					uLoopedChars++;
				} while((curLineWidth < maxWidth) && (curBlockLen < maxBlockLen));
				// Now overrun, go back 1 char (if we ran over at least 2 chars)
				if(uLoopedChars > 1)
				{
					p--;
					curBlockLen--;
				}
			}
			//K...wrap
		}
		else
		{
			p++;           // found a space...
			curBlockLen++; // include it in the first block
		}

		ptr->pBlocks[ptr->iBlockCount].block_len = curBlockLen;
		ptr->pBlocks[ptr->iBlockCount].block_width = -1; // word wrap --> negative block_width
		maxBlockLen -= curBlockLen;
		ptr->iBlockCount++;
		ptr->pBlocks = (KviIrcViewWrappedBlock *)KviMemory::reallocate(ptr->pBlocks, (ptr->iBlockCount + 1) * sizeof(KviIrcViewWrappedBlock));
		ptr->pBlocks[ptr->iBlockCount].block_start = p - unicode;
		ptr->pBlocks[ptr->iBlockCount].block_len = 0;
		ptr->pBlocks[ptr->iBlockCount].block_width = 0;
		ptr->pBlocks[ptr->iBlockCount].pChunk = nullptr;

	wrap_line:
		curLineWidth = 0;
		ptr->uLineWraps++;

		if(ptr->uLineWraps == 1)
		{
			if(KVI_OPTION_BOOL(KviOption_boolIrcViewWrapMargin))
				maxWidth -= m_iWrapMargin;
			if(maxWidth <= m_iIconWidth)
				return;
		}
		else if(ptr->uLineWraps > 128)
		{	// oops.. this is looping endlessly: it may happen in certain insane window width / font size configurations...
			return;
		}
	}

	ptr->iBlockCount++;
}

//
// checkSelectionBlock
//

bool KviIrcView::checkSelectionBlock(KviIrcViewLine * line, int bufIndex)
{
	// Checks if the specified chunk in the specified ircviewline is part of the current selection
	const QChar * unicode = line->szText.unicode();
	const QChar * p = unicode + line->pBlocks[bufIndex].block_start;

	if(!m_pSelectionInitLine || !m_pSelectionEndLine)
		return false;

	// check if selection is bottom to top or vice-versa
	KviIrcViewLine *init, *end;
	if(m_pSelectionInitLine->uIndex <= m_pSelectionEndLine->uIndex)
	{
		init = m_pSelectionInitLine;
		end = m_pSelectionEndLine;
	}
	else
	{
		end = m_pSelectionInitLine;
		init = m_pSelectionEndLine;
	}

	// line is between the first selected line and the last selected one
	if(line->uIndex > init->uIndex && line->uIndex < end->uIndex)
	{
		if(line->pBlocks[bufIndex].pChunk && line->pBlocks[bufIndex].pChunk->type == KviControlCodes::Icon)
			m_pWrappedBlockSelectionInfo->selection_type = KVI_IRCVIEW_BLOCK_SELECTION_ICON;
		else
			m_pWrappedBlockSelectionInfo->selection_type = KVI_IRCVIEW_BLOCK_SELECTION_TOTAL;
		return true;
	}

	if(line->uIndex == init->uIndex && line->uIndex == end->uIndex)
	{
		// Selection begins and ends in this line
		int initChar, endChar;

		// check if the selection is rtol or ltor
		if(m_iSelectionInitCharIndex <= m_iSelectionEndCharIndex)
		{
			initChar = m_iSelectionInitCharIndex;
			endChar = m_iSelectionEndCharIndex;
		}
		else
		{
			endChar = m_iSelectionInitCharIndex;
			initChar = m_iSelectionEndCharIndex;
		}

		// quick check if we're outside the selection bounds
		if(line->pBlocks[bufIndex].block_start > endChar)
			return false;
		if(line->pBlocks[bufIndex].block_start + line->pBlocks[bufIndex].block_len < initChar)
			return false;

		// checks if this is an icon block
		if(line->pBlocks[bufIndex].pChunk && line->pBlocks[bufIndex].pChunk->type == KviControlCodes::Icon)
		{
			m_pWrappedBlockSelectionInfo->selection_type = KVI_IRCVIEW_BLOCK_SELECTION_ICON;
			return true;
		}
		if(line->pBlocks[bufIndex].block_start >= initChar && (line->pBlocks[bufIndex].block_start + line->pBlocks[bufIndex].block_len) <= endChar)
		{
			// Whole chunk selected
			m_pWrappedBlockSelectionInfo->selection_type = KVI_IRCVIEW_BLOCK_SELECTION_TOTAL;
			return true;
		}
		if(line->pBlocks[bufIndex].block_start <= initChar && (line->pBlocks[bufIndex].block_start + line->pBlocks[bufIndex].block_len) >= endChar)
		{
			// Selection ends and begins in THIS BLOCK!
			m_pWrappedBlockSelectionInfo->selection_type = KVI_IRCVIEW_BLOCK_SELECTION_CENTRAL;
			m_pWrappedBlockSelectionInfo->part_1_length = initChar - line->pBlocks[bufIndex].block_start;
			m_pWrappedBlockSelectionInfo->part_1_width = 0;
			m_pWrappedBlockSelectionInfo->part_2_length = endChar - initChar;
			m_pWrappedBlockSelectionInfo->part_3_length = line->pBlocks[bufIndex].block_start + line->pBlocks[bufIndex].block_len - endChar;
			m_pWrappedBlockSelectionInfo->part_2_width = 0;
			for(int i = 0; i < m_pWrappedBlockSelectionInfo->part_1_length; i++)
			{
				int www = IRCVIEW_WCHARWIDTH(*p);
				m_pWrappedBlockSelectionInfo->part_1_width += www;
				p++;
			}
			for(int i = 0; i < m_pWrappedBlockSelectionInfo->part_2_length; i++)
			{
				int www = IRCVIEW_WCHARWIDTH(*p);
				m_pWrappedBlockSelectionInfo->part_2_width += www;
				p++;
			}
			m_pWrappedBlockSelectionInfo->part_3_width = line->pBlocks[bufIndex].block_width - m_pWrappedBlockSelectionInfo->part_1_width - m_pWrappedBlockSelectionInfo->part_2_width;
			return true;
		}

		if(line->pBlocks[bufIndex].block_start > initChar && (line->pBlocks[bufIndex].block_start + line->pBlocks[bufIndex].block_len) > endChar)
		{
			// Selection ends in THIS BLOCK!
			m_pWrappedBlockSelectionInfo->selection_type = KVI_IRCVIEW_BLOCK_SELECTION_LEFT;
			m_pWrappedBlockSelectionInfo->part_1_length = endChar - line->pBlocks[bufIndex].block_start;
			m_pWrappedBlockSelectionInfo->part_1_width = 0;
			for(int i = 0; i < m_pWrappedBlockSelectionInfo->part_1_length; i++)
			{
				int www = IRCVIEW_WCHARWIDTH(*p);
				m_pWrappedBlockSelectionInfo->part_1_width += www;
				p++;
			}
			m_pWrappedBlockSelectionInfo->part_2_length = line->pBlocks[bufIndex].block_len - m_pWrappedBlockSelectionInfo->part_1_length;
			m_pWrappedBlockSelectionInfo->part_2_width = line->pBlocks[bufIndex].block_width - m_pWrappedBlockSelectionInfo->part_1_width;
			return true;
		}

		if(line->pBlocks[bufIndex].block_start < initChar && (line->pBlocks[bufIndex].block_start + line->pBlocks[bufIndex].block_len) < endChar)
		{
			// Selection begins in THIS BLOCK!
			m_pWrappedBlockSelectionInfo->selection_type = KVI_IRCVIEW_BLOCK_SELECTION_RIGHT;
			m_pWrappedBlockSelectionInfo->part_1_length = initChar - line->pBlocks[bufIndex].block_start;
			m_pWrappedBlockSelectionInfo->part_1_width = 0;
			for(int i = 0; i < m_pWrappedBlockSelectionInfo->part_1_length; i++)
			{
				int www = IRCVIEW_WCHARWIDTH(*p);
				m_pWrappedBlockSelectionInfo->part_1_width += www;
				p++;
			}
			m_pWrappedBlockSelectionInfo->part_2_length = line->pBlocks[bufIndex].block_len - m_pWrappedBlockSelectionInfo->part_1_length;
			m_pWrappedBlockSelectionInfo->part_2_width = line->pBlocks[bufIndex].block_width - m_pWrappedBlockSelectionInfo->part_1_width;
			return true;
		}
		return false;
	}

	if(line->uIndex == init->uIndex)
	{
		// Selection begins in this line
		int initChar;

		// check if the selection is uptobottom or bottomtoup
		if(m_pSelectionInitLine->uIndex <= m_pSelectionEndLine->uIndex)
		{
			initChar = m_iSelectionInitCharIndex;
		}
		else
		{
			initChar = m_iSelectionEndCharIndex;
		}
		// icon chunk
		if(line->pBlocks[bufIndex].pChunk && line->pBlocks[bufIndex].pChunk->type == KviControlCodes::Icon)
		{
			m_pWrappedBlockSelectionInfo->selection_type = KVI_IRCVIEW_BLOCK_SELECTION_ICON;
			return true;
		}
		if(line->pBlocks[bufIndex].block_start >= initChar)
		{	// Whole chunk selected
			m_pWrappedBlockSelectionInfo->selection_type = KVI_IRCVIEW_BLOCK_SELECTION_TOTAL;
			return true;
		}

		if(line->pBlocks[bufIndex].block_start < initChar && (line->pBlocks[bufIndex].block_start + line->pBlocks[bufIndex].block_len) > initChar)
		{	// Selection begins in THIS BLOCK!
			m_pWrappedBlockSelectionInfo->selection_type = KVI_IRCVIEW_BLOCK_SELECTION_RIGHT;
			m_pWrappedBlockSelectionInfo->part_1_length = initChar - line->pBlocks[bufIndex].block_start;
			m_pWrappedBlockSelectionInfo->part_1_width = 0;
			for(int i = 0; i < m_pWrappedBlockSelectionInfo->part_1_length; i++)
			{
				int www = IRCVIEW_WCHARWIDTH(*p);
				m_pWrappedBlockSelectionInfo->part_1_width += www;
				p++;
			}
			m_pWrappedBlockSelectionInfo->part_2_length = line->pBlocks[bufIndex].block_len - m_pWrappedBlockSelectionInfo->part_1_length;
			m_pWrappedBlockSelectionInfo->part_2_width = line->pBlocks[bufIndex].block_width - m_pWrappedBlockSelectionInfo->part_1_width;
			return true;
		}
		return false;
	}

	if(line->uIndex == end->uIndex)
	{
		// Selection ends in this line
		int endChar;

		// check if the selection is uptobottom or bottomtoup
		if(m_pSelectionInitLine->uIndex <= m_pSelectionEndLine->uIndex)
		{
			endChar = m_iSelectionEndCharIndex;
		}
		else
		{
			endChar = m_iSelectionInitCharIndex;
		}

		// icon chunk
		if(line->pBlocks[bufIndex].pChunk && line->pBlocks[bufIndex].pChunk->type == KviControlCodes::Icon)
		{
			m_pWrappedBlockSelectionInfo->selection_type = KVI_IRCVIEW_BLOCK_SELECTION_ICON;
			return true;
		}
		if((line->pBlocks[bufIndex].block_start + line->pBlocks[bufIndex].block_len) <= endChar)
		{	// Whole chunk selected
			m_pWrappedBlockSelectionInfo->selection_type = KVI_IRCVIEW_BLOCK_SELECTION_TOTAL;
			return true;
		}

		if(line->pBlocks[bufIndex].block_start < endChar && (line->pBlocks[bufIndex].block_start + line->pBlocks[bufIndex].block_len) > endChar)
		{	// Selection ends in THIS BLOCK!
			m_pWrappedBlockSelectionInfo->selection_type = KVI_IRCVIEW_BLOCK_SELECTION_LEFT;
			m_pWrappedBlockSelectionInfo->part_1_length = endChar - line->pBlocks[bufIndex].block_start;
			m_pWrappedBlockSelectionInfo->part_1_width = 0;
			for(int i = 0; i < m_pWrappedBlockSelectionInfo->part_1_length; i++)
			{
				int www = IRCVIEW_WCHARWIDTH(*p);
				m_pWrappedBlockSelectionInfo->part_1_width += www;
				p++;
			}
			m_pWrappedBlockSelectionInfo->part_2_length = line->pBlocks[bufIndex].block_len - m_pWrappedBlockSelectionInfo->part_1_length;
			m_pWrappedBlockSelectionInfo->part_2_width = line->pBlocks[bufIndex].block_width - m_pWrappedBlockSelectionInfo->part_1_width;
			return true;
		}
		return false;
	}
	return false;
}

//
// recalcFontVariables
//

void KviIrcView::recalcFontVariables(const QFont & font, const QFontInfo & fi)
{
	if(m_pFm)
		delete m_pFm;

	m_pFm = new QFontMetrics(font);

	m_iFontLineSpacing = m_pFm->lineSpacing();

	if((m_iFontLineSpacing < KVI_IRCVIEW_PIXMAP_SIZE) && KVI_OPTION_BOOL(KviOption_boolIrcViewShowImages))
		m_iFontLineSpacing = KVI_IRCVIEW_PIXMAP_SIZE;

	m_iFontDescent = m_pFm->descent();
	m_iFontLineWidth = m_pFm->lineWidth();

	// cache the first 256 characters
	for(unsigned short i = 0; i < 256; i++)
		m_iFontCharacterWidth[i] = m_pFm->width(QChar(i));

	// Currently KviIrcView requires that the bold font variant has the same metrics as the non-bold one.
	// To ensure this, we check if the width of the bold and non-bold variants of the first 256 characters match.
	// If it does, use real bold; otherwise, use faux-bold by drawing the text again at (x,y+1).
	{
		m_bUseRealBold = true;
		QFont fontBold = QFont(font);
		fontBold.setBold(true);
		QFontMetrics fmBold = QFontMetrics(fontBold);
		for(unsigned short i = 32; i < 256; i++)
			if (m_iFontCharacterWidth[i] && fmBold.width(QChar(i)) != m_iFontCharacterWidth[i])
			{
				//printf("Char %d: %d != %d\n", i, fmBold.width(QChar(i)), m_iFontCharacterWidth[i]);
				m_bUseRealBold = false;
				break;
			}
		//printf("m_bUseRealBold = %d\n", m_bUseRealBold);
	}

	// fix for #489 (horizontal tabulations)
	m_iFontCharacterWidth[9] = m_pFm->width("\t");

	if(m_iFontLineWidth < 1)
		m_iFontLineWidth = 1;

	if(KVI_OPTION_BOOL(KviOption_boolIrcViewTimestamp))
	{
		QString szTimestamp;
		QDateTime datetime = KVI_OPTION_BOOL(KviOption_boolIrcViewTimestampUTC) ? QDateTime::currentDateTime().toUTC() : QDateTime::currentDateTime();
		szTimestamp = datetime.toString(KVI_OPTION_STRING(KviOption_stringIrcViewTimestampFormat));
		szTimestamp.append(' ');
		m_iWrapMargin = m_pFm->width(szTimestamp);
	}
	else
	{
		m_iWrapMargin = m_pFm->width("wwww");
	}

	m_iMinimumPaintWidth = (((int)(m_pFm->width('w'))) << 1);
	if(KVI_OPTION_BOOL(KviOption_boolIrcViewWrapMargin))
		m_iMinimumPaintWidth += m_iWrapMargin;

	m_iRelativePixmapY = (int)(m_iFontLineSpacing + KVI_IRCVIEW_PIXMAP_SIZE) >> 1;

	m_iIconWidth = (int)m_pFm->width("w");

	if(fi.fixedPitch() && (m_iIconWidth > 0))
	{
		while(m_iIconWidth < 18)
			m_iIconWidth += m_iIconWidth;
		m_iIconSideSpacing = (m_iIconWidth - 16) >> 1;
	}
	else
	{
		m_iIconWidth = 18;
		m_iIconSideSpacing = 1;
	}
}

//
// resizeEvent
//

void KviIrcView::resizeEvent(QResizeEvent *)
{
	int iScr = m_pScrollBar->sizeHint().width();
	int iLeft = width() - iScr;
	m_pToolsButton->setGeometry(iLeft, 0, iScr, iScr);
	m_pScrollBar->setGeometry(iLeft, iScr, iScr, height() - iScr);

	if(m_pToolWidget && m_pToolWidget->isVisible())
	{
		int h = m_pToolWidget->sizeHint().height();
		m_pToolWidget->setGeometry(0, height() - h, width() - iScr, h);
	}
}

QSize KviIrcView::sizeHint() const
{
	QSize ret(KVI_IRCVIEW_SIZEHINT_WIDTH, KVI_IRCVIEW_SIZEHINT_HEIGHT);
	return ret;
}

void KviIrcView::showToolsPopup()
{
	if(!m_pToolsPopup)
	{
		m_pToolsPopup = new QMenu(this);

		m_pToolsPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Search)), __tr2qs("Toggle Search"), this, SLOT(toggleToolWidget()));

		m_pToolsPopup->addSeparator();

		m_pToolsPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Plus)), __tr2qs("Zoom In"), this, SLOT(increaseFontSize())); // We let fly "in" as a capitalized preposition.
		m_pToolsPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Minus)), __tr2qs("Zoom Out"), this, SLOT(decreaseFontSize()));

		m_pToolsPopup->addSeparator();

		m_pToolsPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Topic)), __tr2qs("Choose Temporary Font..."), this, SLOT(chooseFont()));
		m_pToolsPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Canvas)), __tr2qs("Choose Temporary Background..."), this, SLOT(chooseBackground()));

		m_pToolsPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Discard)), __tr2qs("Reset Font"), this, SLOT(resetDefaultFont()));
		m_pToolsPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Discard)), __tr2qs("Reset Background"), this, SLOT(resetBackground()));

		m_pToolsPopup->addSeparator();

		m_pToolsPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Remove)), __tr2qs("Clear Buffer"), this, SLOT(clearBuffer()));
	}
	QSize s = m_pToolsPopup->sizeHint();

	m_pToolsPopup->popup(m_pToolsButton->mapToGlobal(QPoint(m_pToolsButton->width() - s.width(), m_pToolsButton->height())));
}

void KviIrcView::increaseFontSize()
{
	QFont f = font();
	f.setPointSize(f.pointSize() + 1);
	setFont(f);
}

void KviIrcView::decreaseFontSize()
{
	QFont f = font();
	int p = f.pointSize();
	if(p > 2)
		p--;
	f.setPointSize(p);
	setFont(f);
}

void KviIrcView::resetDefaultFont()
{
	setFont(KVI_OPTION_FONT(KviOption_fontIrcView));
}

void KviIrcView::chooseFont()
{
	bool bOk;
#ifdef COMPILE_ON_MAC
	// The native font dialog makes Qt 4.6 go into a strange modal infinite loop (the font dialog is never properly closed).
	// FIXME: Re-check it with future releases of Qt.
	QFont f = QFontDialog::getFont(&bOk, font(), this, __tr("Choose Font"), QFontDialog::DontUseNativeDialog);
#else  //!COMPILE_ON_MAC
	QFont f = QFontDialog::getFont(&bOk, font(), this, __tr("Choose Font"));
#endif //!COMPILE_ON_MAC
	if(!bOk)
		return;
	setFont(f);
}

void KviIrcView::chooseBackground()
{
	QString f;
	if(
	    !KviFileDialog::askForOpenFileName(
	        f,
	        __tr2qs("Choose the background image..."),
	        QString(),
	        QString(),
	        false,
	        true,
	        this))
		return;

	if(f.isEmpty())
		return;

	QPixmap p(f);
	if(p.isNull())
	{
		QMessageBox::information(this, __tr2qs("Invalid Image - KVIrc"), __tr2qs("Failed to load the selected image!"), __tr2qs("OK"));
		return;
	}

	setPrivateBackgroundPixmap(p);
}

void KviIrcView::resetBackground()
{
	QPixmap dummy;
	setPrivateBackgroundPixmap(dummy);
}

void KviIrcView::toggleToolWidget()
{
	if(m_pToolWidget && m_pToolWidget->isVisible())
	{
		m_pToolWidget->setVisible(false);
		m_pCursorLine = nullptr;

		// When the tool widget is hidden, ensure the input is focussed (otherwise text is still entered into the 'string to find' widget...)
		if(m_pKviWindow && m_pKviWindow->input())
			m_pKviWindow->input()->setFocus();
	}
	else
	{
		if(!m_pToolWidget)
			m_pToolWidget = new KviIrcViewToolWidget(this);

		int h = m_pToolWidget->sizeHint().height();
		int iScr = m_pScrollBar->sizeHint().width();
		m_pToolWidget->setGeometry(0, height() - h, width() - iScr, h);
		m_pToolWidget->setVisible(true);

		// Ensure the 'string to find' widget is always in focus when the search functionality is requested
		m_pToolWidget->focusStringToFind();

		m_pToolWidget->m_pStringToFind->selectAll();
	}

	repaint();
}

//
// The IrcView : find
//

void KviIrcView::ensureLineVisible(KviIrcViewLine * pLineToShow)
{
	if(!pLineToShow)
		return;

	if(pLineToShow == m_pCurLine)
	{
		// nothing to do, just repaint for safety sake
		repaint();
		return;
	}

	// need to scroll
	int sc = m_pScrollBar->value();

	if(pLineToShow->uIndex > m_pCurLine->uIndex)
	{
		// The cursor line is below the current line
		// Go down counting scroll steps (and verify if the line is really there)
		KviIrcViewLine * pLine = m_pCurLine;

		while(pLine && (pLine != pLineToShow))
		{
			pLine = pLine->pNext;
			sc++;
		}

		if(!pLine)
			return; // oops.. line not found ?

		if(sc != m_pScrollBar->value())
		{
			m_pCurLine = pLine;
			m_iLastScrollBarValue = sc;
			m_pScrollBar->setValue(sc);
		}
		else
		{
			repaint();
		}
		return;
	}

	// The cursor line is over the current line
	// Here we're in trouble :D
	int toolWidgetHeight = (m_pToolWidget && m_pToolWidget->isVisible()) ? m_pToolWidget->sizeHint().height() : 0;
	int scrollbarWidth = m_pScrollBar->width();
	int curBottomCoord = height() - toolWidgetHeight - KVI_IRCVIEW_VERTICAL_BORDER;
	int maxLineWidth = width() - scrollbarWidth - KVI_IRCVIEW_DOUBLEBORDER_WIDTH;

	if(KVI_OPTION_BOOL(KviOption_boolIrcViewShowImages))
		maxLineWidth -= KVI_IRCVIEW_PIXMAP_AND_SEPARATOR;
	// Make sure that we have enough space to paint something...
	if(maxLineWidth < m_iMinimumPaintWidth)
		return; // ugh
	// And loop through lines until we not run over the upper bound of the view
	KviIrcViewLine * pLine = m_pCurLine;
	KviIrcViewLine * pCurLine = m_pCurLine;
	while(pLine)
	{
		if(maxLineWidth != pLine->iMaxLineWidth)
			calculateLineWraps(pLine, maxLineWidth);
		curBottomCoord -= (pLine->uLineWraps + 1) * m_iFontLineSpacing;
		while(pCurLine && (curBottomCoord < KVI_IRCVIEW_VERTICAL_BORDER))
		{
			if(pCurLine->iMaxLineWidth != maxLineWidth)
				calculateLineWraps(pCurLine, maxLineWidth);
			curBottomCoord += ((pCurLine->uLineWraps + 1) * m_iFontLineSpacing) + m_iFontDescent;
			pCurLine = pCurLine->pPrev;
			sc--;
		}
		if(pLine == pLineToShow)
			break;
		curBottomCoord -= m_iFontDescent;
		pLine = pLine->pPrev;
	}

	if(!pCurLine)
		return; // ooops.. line not found :D

	if(sc != m_pScrollBar->value())
	{
		m_pCurLine = pCurLine;
		m_iLastScrollBarValue = sc;
		m_pScrollBar->setValue(sc);
	}
	else
	{
		repaint();
	}
}

void KviIrcView::setCursorLine(KviIrcViewLine * l)
{
	m_pCursorLine = l;
	ensureLineVisible(l);
}

void KviIrcView::findNext(const QString & szText, bool bCaseS, bool bRegExp, bool bExtended)
{
	KviIrcViewLine * l = m_pCursorLine;
	if(!l)
		l = m_pCurLine;
	if(l)
	{
		l = l->pNext;
		if(!l)
			l = m_pFirstLine;
		KviIrcViewLine * start = l;

		int idx = -1;

		do
		{
			if(m_pToolWidget)
			{
				if(!(m_pToolWidget->messageEnabled(l->iMsgType)))
					goto do_pNext;
			}

			if(bRegExp)
			{
				QRegExp re(szText, bCaseS ? Qt::CaseSensitive : Qt::CaseInsensitive, bExtended ? QRegExp::RegExp : QRegExp::Wildcard);
				idx = re.indexIn(l->szText, 0);
			}
			else
			{
				QString tmp = l->szText;
				idx = tmp.indexOf(szText, 0, bCaseS ? Qt::CaseSensitive : Qt::CaseInsensitive);
			}

			if(idx != -1)
			{
				setCursorLine(l);
				if(m_pToolWidget)
				{
					QString szTmp = QString(__tr2qs("Pos %1")).arg(idx);
					m_pToolWidget->setFindResult(szTmp);
				}
				return;
			}

		do_pNext:

			l = l->pNext;
			if(!l)
				l = m_pFirstLine;

		} while(l != start);
	}
	m_pCursorLine = nullptr;
	repaint();
	if(m_pToolWidget)
		m_pToolWidget->setFindResult(__tr2qs("Not found"));
}

void KviIrcView::findPrev(const QString & szText, bool bCaseS, bool bRegExp, bool bExtended)
{
	KviIrcViewLine * l = m_pCursorLine;
	if(!l)
		l = m_pCurLine;
	if(l)
	{
		l = l->pPrev;
		if(!l)
			l = m_pLastLine;
		KviIrcViewLine * start = l;

		int idx = -1;

		do
		{

			if(m_pToolWidget)
			{
				if(!(m_pToolWidget->messageEnabled(l->iMsgType)))
					goto do_pPrev;
			}

			if(bRegExp)
			{
				QRegExp re(szText, bCaseS ? Qt::CaseSensitive : Qt::CaseInsensitive, bExtended ? QRegExp::RegExp : QRegExp::Wildcard);
				idx = re.indexIn(l->szText, 0);
			}
			else
			{
				QString tmp = l->szText;
				idx = tmp.indexOf(szText, 0, bCaseS ? Qt::CaseSensitive : Qt::CaseInsensitive);
			}

			if(idx != -1)
			{
				setCursorLine(l);
				if(m_pToolWidget)
				{
					QString szTmp = QString(__tr2qs("Pos %1")).arg(idx);
					m_pToolWidget->setFindResult(szTmp);
				}
				return;
			}

		do_pPrev:

			l = l->pPrev;
			if(!l)
				l = m_pLastLine;

		} while(l != start);
	}
	m_pCursorLine = nullptr;

	repaint();
	if(m_pToolWidget)
		m_pToolWidget->setFindResult(__tr2qs("Not found"));
}

KviIrcViewLine * KviIrcView::getVisibleLineAt(int yPos)
{
	KviIrcViewLine * l = m_pCurLine;
	int toolWidgetHeight = (m_pToolWidget && m_pToolWidget->isVisible()) ? m_pToolWidget->sizeHint().height() : 0;
	int iTop = height() + m_iFontDescent - KVI_IRCVIEW_VERTICAL_BORDER - toolWidgetHeight;

	while(iTop > yPos)
	{
		if(l)
		{
			iTop -= ((l->uLineWraps + 1) * m_iFontLineSpacing) + m_iFontDescent;
			if(iTop <= yPos)
				return l;
			l = l->pPrev;
		}
		else
			return nullptr;
	}
	return nullptr;
}

int KviIrcView::getVisibleCharIndexAt(KviIrcViewLine *, int xPos, int yPos)
{
	/*
	 * Profane description: this functions sums up most of the complications involved in the ircview. We got a mouse position and have
	 * to identify if there's a link inside the KviIrcViewLine at that position.
	 * l contains the current KviIrcViewLine we're checking, iTop is the y coordinate of the
	 * that line. We go from the bottom to the top: l is the last line and iTop is the y coordinate of the end of that line (imagine it
	 * as the beginning of the "next" line that have to come.
	 */

	KviIrcViewLine * l = m_pCurLine;
	int toolWidgetHeight = (m_pToolWidget && m_pToolWidget->isVisible()) ? m_pToolWidget->sizeHint().height() : 0;
	int iTop = height() + m_iFontDescent - KVI_IRCVIEW_VERTICAL_BORDER - toolWidgetHeight;

	// our current line begins after the mouse position... go on
	while(iTop > yPos)
	{
		// no lines, go away
		if(!l)
			return -1;

		// subtract from iTop the height of the current line (aka go to the end of the previous / start of the current point)
		iTop -= ((l->uLineWraps + 1) * m_iFontLineSpacing) + m_iFontDescent;

		// we're still below the mouse position.. go on
		if(iTop > yPos)
		{
			// next round, try with the previous line
			l = l->pPrev;
			continue;
		}

		/*
		 * Profane description: if we are here we have found the right line where our mouse is over; l is the KviIrcViewLine *,
		 * iTop is the line start y coordinate. Now we have to go through this line's text and find the exact text under the mouse.
		 * The line start x posistion is iLeft; we save iTop to firstRowTop (many rows can be part of this lingle line of text)
		 */
		int iLeft = KVI_IRCVIEW_HORIZONTAL_BORDER;

		if(KVI_OPTION_BOOL(KviOption_boolIrcViewShowImages))
			iLeft += KVI_IRCVIEW_PIXMAP_AND_SEPARATOR;

		int firstRowTop = iTop;
		int i = 0;

		for(;;)
		{
			// if the mouse position is > start_of_this_row + row_height, move on to the next row of this line
			if(yPos > iTop + m_iFontLineSpacing)
			{
				// run until a word wrap block (aka a new line); move at least one block forward
				i++;
				while(i < l->iBlockCount)
				{
					if(l->pBlocks[i].pChunk == nullptr)
						break; // word wrap found
					else
						i++;
				}
				if(i >= l->iBlockCount)
					return -1; // we reached the last chunk... there's something wrong, return
				else
					iTop += m_iFontLineSpacing; // we found a word wrap, check the next row.
			}
			else
			{
			/*
			 * Profane description: Once we get here, we know the correct line l, the correct row top coordinate iTop and
			 * the index of the first chunk in this line i.
			 * Calculate the left border of this row: if this is not the first one, add any margin.
			 * Note: iLeft will contain the right border position of the current chunk.
			 */
				// this is not the first row of this line and the margin option is enabled?
				if(iTop != firstRowTop)
					if(KVI_OPTION_BOOL(KviOption_boolIrcViewWrapMargin))
						iLeft += m_iWrapMargin;

				if(xPos < iLeft)
					return 0; // Mouse is out of this row boundaries

				if(i >= l->iBlockCount)
					return l->szText.size();

				// run up to the chunk containing the mouse position
				for(; iLeft + l->pBlocks[i].block_width < xPos;)
				{
					if(l->pBlocks[i].block_width > 0)
						iLeft += l->pBlocks[i].block_width;
					else if(i < (l->iBlockCount - 1))
					{
						// There is another block, check if it is a wrap (we reached the end of the row)
						if(l->pBlocks[i + 1].pChunk == nullptr)
							break;
						// else simply a zero characters block
					}
					i++;
					if(i >= l->iBlockCount)
						return l->szText.size();
				}
				// now, get the right character inside the block
				int retValue = 0, oldIndex = 0, oldLeft = iLeft;
				QChar curChar;
				// add the width of each single character until we get the right one
				while(iLeft < xPos && retValue < l->pBlocks[i].block_len)
				{
					oldIndex = retValue; oldLeft = iLeft;

					curChar = l->szText.at(l->pBlocks[i].block_start + retValue);
					if (curChar >= 0xD800 && curChar <= 0xDC00) // Surrogate pair
					{
						iLeft += m_pFm->width(l->szText.mid(retValue), 2);
						retValue+=2;
					}
					else
					{
						iLeft += (curChar < 0xff) ? m_iFontCharacterWidth[curChar.unicode()] : m_pFm->width(curChar);
						retValue++;
					}
				}

				// Make the horizontal point delimiting which characters are to be selected be in the middle of the character,
				// i.e. if the user starts selecting from the left half of a character, select that character too.
				// This mimics the behavior of most other GUI applications.
				int iMid = (oldLeft + iLeft) / 2;
				if (xPos <= iMid)
					retValue = oldIndex;

				//printf("%d\n",l->pBlocks[i].block_start+retValue);
				return l->pBlocks[i].block_start + retValue;
			}
		}
	}
	return -1;
}

KviIrcViewWrappedBlock * KviIrcView::getLinkUnderMouse(int xPos, int yPos, QRect * pRect, QString * linkCmd, QString * linkText)
{	/*
	 * Profane description: this functions sums up most of the complications involved in the ircview. We got a mouse position and have
	 * to identify if there's a link inside the KviIrcViewLine at that position.
	 * l contains the current KviIrcViewLine we're checking, iTop is the y coordinate of the
	 * that line. We go from the bottom to the top: l is the last line and iTop is the y coordinate of the end of that line (imagine it
	 * as the beginning of the "next" line that have to come.
	 */

	KviIrcViewLine * l = m_pCurLine;
	int toolWidgetHeight = (m_pToolWidget && m_pToolWidget->isVisible()) ? m_pToolWidget->sizeHint().height() : 0;
	int iTop = height() + m_iFontDescent - KVI_IRCVIEW_VERTICAL_BORDER - toolWidgetHeight;

	// our current line begins after the mouse position... go on
	while(iTop > yPos)
	{
		// no lines, go away
		if(!l)
			return nullptr;

		// subtract from iTop the height of the current line (aka go to the end of the previous / start of the current point)
		iTop -= ((l->uLineWraps + 1) * m_iFontLineSpacing) + m_iFontDescent;

		// we're still below the mouse position.. go on
		if(iTop > yPos)
		{
			// next round, try with the previous line
			l = l->pPrev;
			continue;
		}

		/*
		 * Profane description: if we are here we have found the right line where our mouse is over; l is the KviIrcViewLine *,
		 * iTop is the line start y coordinate. Now we have to go through this line's text and find the exact text under the mouse.
		 * The line start x posistion is iLeft; we save iTop to firstRowTop (many rows can be part of this lingle line of text)
		 */
		int iLeft = KVI_IRCVIEW_HORIZONTAL_BORDER;
		if(KVI_OPTION_BOOL(KviOption_boolIrcViewShowImages))
			iLeft += KVI_IRCVIEW_PIXMAP_AND_SEPARATOR;
		int firstRowTop = iTop;
		int i = 0;

		int iLastEscapeBlock = -1;
		int iLastEscapeBlockTop = -1;

		for(;;)
		{
			// if the mouse position is > start_of_this_row + row_height, move on to the next row of this line
			if(yPos > iTop + m_iFontLineSpacing)
			{
				// run until a word wrap block (aka a new line); move at least one block forward
				i++;
				while(i < l->iBlockCount)
				{
					if(l->pBlocks[i].pChunk == nullptr)
					{
						break; // word wrap found
					}
					else
					{
						// still ok to run right, but check if we find a url
						if(i >= l->iBlockCount)
							break;
						// we try to save the position of the last "text escape" tag we find
						if(l->pBlocks[i].pChunk)
							if(l->pBlocks[i].pChunk->type == KviControlCodes::Escape)
							{
								iLastEscapeBlock = i;
								iLastEscapeBlockTop = iTop;
							}
						// we reset the position of the last "text escape" tag if we find a "unescape"
						if(l->pBlocks[i].pChunk)
							if(l->pBlocks[i].pChunk->type == KviControlCodes::UnEscape)
								iLastEscapeBlock = -1;

						i++;
					}
				}
				if(i >= l->iBlockCount)
					return nullptr; // we reached the last chunk... there's something wrong, return
				else
					iTop += m_iFontLineSpacing; // we found a word wrap, check the next row.
			}
			else
			{
			/*
			 * Profane description: Once we get here, we know the correct line l, the correct row top coordinate iTop and
			 * the index of the first chunk in this line i.
			 * Calculate the left border of this row: if this is not the first one, add any margin.
			 * Note: iLeft will contain the right border position of the current chunk.
			 */
				int iBlockWidth = 0;

				// this is not the first row of this line and the margin option is enabled?
				if(iTop != firstRowTop)
					if(KVI_OPTION_BOOL(KviOption_boolIrcViewWrapMargin))
						iLeft += m_iWrapMargin;

				if(xPos < iLeft)
					return nullptr; // Mouse is out of this row boundaries
				for(;;)
				{
					int iLastLeft = iLeft;
					// we've run till the end of the line, go away
					if(i >= l->iBlockCount)
						return nullptr;
					// we try to save the position of the last "text escape" tag we find
					if(l->pBlocks[i].pChunk)
						if(l->pBlocks[i].pChunk->type == KviControlCodes::Escape)
						{
							iLastEscapeBlock = i;
							iLastEscapeBlockTop = iTop;
						}
					// we reset the position of the last "text escape" tag if we find a "unescape"
					if(l->pBlocks[i].pChunk)
						if(l->pBlocks[i].pChunk->type == KviControlCodes::UnEscape)
							iLastEscapeBlock = -1;
					// if the block width is > 0, update iLeft
					if(l->pBlocks[i].block_width > 0)
					{
						iBlockWidth = l->pBlocks[i].block_width;
						iLeft += iBlockWidth;
					}
					else
					{
						if(i < (l->iBlockCount - 1))
						{	// There is another block, check if it is a wrap (we reached the end of the row)
							if(l->pBlocks[i + 1].pChunk == nullptr)
							{
								iBlockWidth = width() - iLastLeft;
								iLeft = width();
							}
							// else simply a zero characters block
						}
					}
					/*
					 * Profane description: mouse was not under the last chunk, try with this one..
					 */
					if(xPos < iLeft)
					{
						// Got it!
						// link ?
						bool bHadWordWraps = false;
						while(l->pBlocks[i].pChunk == nullptr)
						{
							// word wrap ?
							if(i >= 0)
							{
								i--;
								bHadWordWraps = true;
							}
							else
								return nullptr; // all word wraps ?!!!
						}
						if(iLastEscapeBlock != -1)
						{
							int iLeftBorder = iLeft;
							int k;
							for(k = i; k >= iLastEscapeBlock; k--)
								iLeftBorder -= l->pBlocks[k].block_width;
							int iRightBorder = 0;
							unsigned int uLineWraps = 0;
							for(k = iLastEscapeBlock;; k++)
							{
								if(l->pBlocks[k].pChunk)
								{
									if(l->pBlocks[k].pChunk->type != KviControlCodes::UnEscape)
										iRightBorder += l->pBlocks[k].block_width;
									else
										break;
								}
								else
								{
									uLineWraps++;
									bHadWordWraps = true;
								}
							}
							if(pRect)
							{
								*pRect = QRect(iLeftBorder,
								    bHadWordWraps ? iLastEscapeBlockTop : iTop,
								    iRightBorder,
								    ((uLineWraps + 1) * m_iFontLineSpacing) + m_iFontDescent);
							}
							if(linkCmd)
							{
								linkCmd->setUtf16(l->pBlocks[iLastEscapeBlock].pChunk->szPayload, kvi_wstrlen(l->pBlocks[iLastEscapeBlock].pChunk->szPayload));
								*linkCmd = linkCmd->trimmed();
								if((*linkCmd) == "nc")
									(*linkCmd) = "n";
							}
							if(linkText)
							{
								QString szLink;
								int iEndOfLInk = iLastEscapeBlock;
								while(true)
								{
									if(l->pBlocks[iEndOfLInk].pChunk)
									{
										if(l->pBlocks[iEndOfLInk].pChunk->type != KviControlCodes::UnEscape)
										{
											switch(l->pBlocks[iEndOfLInk].pChunk->type)
											{
												case KviControlCodes::Bold:
												case KviControlCodes::Italic:
												case KviControlCodes::Underline:
												case KviControlCodes::Reverse:
												case KviControlCodes::Reset:
													szLink.append(QChar(l->pBlocks[iEndOfLInk].pChunk->type));
													break;
												case KviControlCodes::Color:
													szLink.append(QChar(KviControlCodes::Color));
													if(l->pBlocks[iEndOfLInk].pChunk->colors.fore != KviControlCodes::NoChange)
													{
														szLink.append(QString("%1").arg((int)(l->pBlocks[iEndOfLInk].pChunk->colors.fore)));
													}
													if(l->pBlocks[iEndOfLInk].pChunk->colors.back != KviControlCodes::NoChange)
													{
														szLink.append(QChar(','));
														szLink.append(QString("%1").arg((int)(l->pBlocks[iEndOfLInk].pChunk->colors.back)));
													}
													break;
											}
											szLink.append(l->szText.mid(l->pBlocks[iEndOfLInk].block_start, l->pBlocks[iEndOfLInk].block_len));
										}
										else
										{
											break;
										}
									}
									iEndOfLInk++;
								}
								*linkText = szLink;
								// grab the rest of the link visible string
								// Continue while we do not find a non word wrap block block
								for(int bufIndex = (i + 1); bufIndex < l->iBlockCount; bufIndex++)
								{
									if(l->pBlocks[bufIndex].pChunk)
										break; // finished : not a word wrap
									else
									{
										linkText->append(l->szText.mid(l->pBlocks[bufIndex].block_start, l->pBlocks[bufIndex].block_len));
									}
								}
							}
							return &(l->pBlocks[iLastEscapeBlock]);
						}
						if(l->pBlocks[i].pChunk->type == KviControlCodes::Icon)
						{
							if(pRect)
							{
								*pRect = QRect(iLastLeft,
								    bHadWordWraps ? firstRowTop : iTop,
								    iBlockWidth,
								    ((l->uLineWraps + 1) * m_iFontLineSpacing) + m_iFontDescent);
							}
							if(linkCmd)
							{
								*linkCmd = "[!txt]";
								QString tmp;
								tmp.setUtf16(l->pBlocks[i].pChunk->szPayload, kvi_wstrlen(l->pBlocks[i].pChunk->szPayload));
								linkCmd->append(tmp);
								*linkCmd = linkCmd->trimmed();
							}
							if(linkText)
							{
								*linkText = "";
							}
							return &(l->pBlocks[i]);
						}
						return nullptr;
					}
					i++;
				}
			}
		}
	}
	return nullptr;
}

KviConsoleWindow * KviIrcView::console()
{
	return m_pKviWindow->console();
}

void KviIrcView::animatedIconChange()
{
	update();
}

void KviIrcView::scrollToMarker()
{
	KviIrcViewLine * pLine = m_pCurLine;

	while(pLine && (pLine->uIndex != m_uLineMarkLineIndex))
		pLine = pLine->pPrev;

	if(pLine == nullptr)
	{	// The buffer has already cleaned the marker line
		ensureLineVisible(m_pFirstLine);
	}
	else
	{
		ensureLineVisible(pLine);
	}
}
