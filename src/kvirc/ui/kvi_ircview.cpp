//=============================================================================
//
//   File : kvi_ircview.cpp
//   Creation date : Tue Jul 6 1999 14:45:20 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2004 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#define __KVIRC__
// Damn complex class ...but it works :)
// #include <brain.h>
//
// #define HOPE_THAT_IT_WILL_NEVER_NEED_TO_BE_MODIFIED :)

// 07 May 1999 ,
//      Already forgot how this damn thing works ,
//      and spent 1 hour over a stupid bug.
//      I had to recreate the whole thing in my mind......ooooouh...
//      How did I wrote it ?
//      Just take a look to paintEvent() or to calculateLineWraps()...
//      Anyway...I've solved the bug.

// 23 Nov 1999 ,
//      Well , not so bad...I seem to still remember how it works
//      So just for fun , complicated the things a little bit more.
//      Added precaclucaltion of the text blocks and word wrapping
//      and a fast scrolling mode (3 lines at once) for consecutive
//      appendText() calls.
//      Now the code becomes really not understandable...:)

// 29 Jun 2000 21:02 ,
//      Here we go again... I have to adjust this stuff for 3.0.0
//      Will I make this thingie work ?
// 01 Jul 2000 04:20 (AM!) ,
//      Yes....I got it to work just now
//      and YES , complicated the things yet more.
//      This time made some paint event code completely unreadable
//      by placing two monster macros...
//      I hope that you have a smart compiler (such as gcc is).

// 09 Dec 2000
//      This is my C-asm-optimisation-hack playground
//      Expect Bad Programming(tm) , Ugly Code(tm) , Unreadable Macros (tm)
//      and massive usage of the Evil(tm) goto.

// 25 Sep 2001
//      This stuff is going to be ported to Windoze
//      A conditionally compiled code will use only Qt calls...let's see :)
//


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Here we go... a huge set of includes
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "kvi_ircview.h"
#include "kvi_ircviewtools.h"
#include "kvi_ircviewprivate.h"
#include "kvi_styled_controls.h"
#include "kvi_debug.h"
#include "kvi_app.h"
#include "kvi_settings.h"
#include "kvi_options.h"
#include "kvi_mirccntrl.h"
#include "kvi_defaults.h"
#include "kvi_window.h"
#include "kvi_locale.h"
#include "kvi_frame.h"
#include "kvi_malloc.h"
#include "kvi_memmove.h"
#include "kvi_iconmanager.h"
#include "kvi_out.h"
#include "kvi_parameterlist.h"
#include "kvi_console.h"
#include "kvi_ircuserdb.h"
#include "kvi_channel.h"
#include "kvi_topicw.h"
#include "kvi_query.h"
#include "kvi_filedialog.h"
#include "kvi_msgbox.h"
#include "kvi_texticonmanager.h"
#include "kvi_ircconnection.h"
#include "kvi_ircconnectiontarget.h"
#include "kvi_mdimanager.h"
#include "kvi_userinput.h"
#include "kvi_kvs_eventtriggers.h"
#include "kvi_doublebuffer.h"
#include "kvi_ircurl.h"
#include "kvi_draganddrop.h"
#include "kvi_qcstring.h"
// FIXME: #warning "There should be an option to preserve control codes in copied text (clipboard) (mIrc = CTRL+Copy->with colors)"

#include <qbitmap.h>
#include <qpainter.h>
#include <qregexp.h>
#include <qfile.h>
#include <qtoolbutton.h>
#include <qfontmetrics.h> // needed
#include <qapplication.h>
#include "kvi_tal_popupmenu.h"
#include <qmessagebox.h>
#include <qtextcodec.h>
#include <qdatetime.h>
#include <qevent.h>

//#include <qcolor.h>   // needed 

// FIXME: #warning "There are problems with the selection and wrapped lines: you can select something on the first line and get the second highlighted"
// FIXME: #warning "This hack is temporary...later remove it"

#if QT_VERSION >= 300
	#ifndef QT_CLEAN_NAMESPACE
		#define QT_CLEAN_NAMESPACE
		#include <qcursor.h>
		#undef QT_CLEAN_NAMESPACE
	#else
		#include <qcursor.h>
	#endif
#else
	#include <qcursor.h>
#endif

#include <qclipboard.h>
#include <qdatetime.h>
#include <qmessagebox.h>
#include <qscrollbar.h>
#include <qfontdialog.h>

#include <time.h>

#ifdef COMPILE_USE_QT4
	#include <q3mimefactory.h>
	#define QMimeSourceFactory Q3MimeSourceFactory
#endif


#ifdef COMPILE_ON_WINDOWS
	#pragma warning(disable: 4102)
#endif

#ifdef __STRICT_ANSI__
	#ifdef COMPILE_USE_DYNAMIC_LABELS
		// incompatible with -ansi
		#undef COMPILE_USE_DYNAMIC_LABELS
	#endif
#endif

#ifdef COMPILE_ZLIB_SUPPORT
	#include <zlib.h>
#endif

#define KVI_DEF_BACK 200

// FIXME: #warning "The scrollbar should NOT have a fixed size : the KDE styles can configure the size (sizeHint() ?)"

//
// FIXME: PgUp and PgDn scrolls a fixed number of lines!
//        Make it view height dependant
//
// FIXME: This widget is quite slow on a 300 MHz processor
//


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Globals
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Stuff declared in kvi_app.cpp and managed by KviApp class


#ifdef COMPILE_PSEUDO_TRANSPARENCY
	extern QPixmap       * g_pShadedChildGlobalDesktopBackground;
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Internal constants
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Maximum size of the internal buffer for each window
// This is the default value
//#define KVI_IRCVIEW_MAX_LINES 1024
// Borders...just do not set it to 0
#define KVI_IRCVIEW_HORIZONTAL_BORDER 4
#define KVI_IRCVIEW_VERTICAL_BORDER 4
// A little bit more than the scroll-bar...
// Qt+X have strange interactions that I can not understand when I try to move the splitter
// to the maximum on the left , Maybe the cache pixmap size becomes negative ? (I don't think so)
// Anyway , when the scroll bar position becomes negative (or the IrcView has smaller width than
// the scroll bar) X aborts with a funny
// X Error: BadDrawable (invalid Pixmap or Window parameter) 9
//   Major opcode:  55
// Program received signal SIGABRT, Aborted.
// Do not change unless you're sure that it will not happen :)
#define KVI_IRCVIEW_MINIMUM_WIDTH 22
//16+4+(2*4) * Do not change
#define KVI_IRCVIEW_PIXMAP_AND_SEPARATOR 20
#define KVI_IRCVIEW_PIXMAP_SEPARATOR_AND_DOUBLEBORDER_WIDTH 28
#define KVI_IRCVIEW_SELECT_REPAINT_INTERVAL 100
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

// FIXME: Get rid of this!!!!!!!!!
#define WSTRINGCONFIG_SAFE_TO_MEMCPY_QCHAR

#define _WSTRING_WMEMCPY(_dst,_src,_len) kvi_fastmoveodd((void *)(_dst),(const void *)(_src),sizeof(kvi_wchar_t) * (_len))

void kvi_appendWCharToQStringWithLength(QString * qstrptr,const kvi_wchar_t * ptr,kvi_wslen_t len)
{
	kvi_wslen_t oldLen = qstrptr->length();
	qstrptr->setLength(oldLen + len);
	#ifdef WSTRINGCONFIG_SAFE_TO_MEMCPY_QCHAR
		_WSTRING_WMEMCPY(qstrptr->unicode() + oldLen,ptr,len);
	#else // !WSTRINGCONFIG_SAFE_TO_MEMCPY_QCHAR
		QChar * c = (qstrptr->unicode() + oldLen);
		while(*ptr)
		{
			c->unicode() = *ptr;
			ptr++;
			c++;
		}
	#endif // !WSTRINGCONFIG_SAFE_TO_MEMCPY_QCHAR
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Info about escape syntax
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// escape commands:
//
//  <cr>!<escape_command><cr><visible parameters<cr>
//
//  <escape_command> ::= u        <--- url link
//  <escape_command> ::= n        <--- nick link
//  <escape_command> ::= s        <--- server link
//  <escape_command> ::= h        <--- host link
//  <escape_command> ::= [...     <--- generic escape "rbt" | "mbt" | "dbl" | "txt"
//


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// The IrcView : construct and destroy
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

KviIrcView::KviIrcView(QWidget *parent,KviFrame *pFrm,KviWindow *pWnd)
: QWidget(parent,"irc_view") 
{
	// Ok...here we go
	// initialize the initializable

#ifdef COMPILE_USE_QT4
	setAttribute(Qt::WA_NoSystemBackground); // This disables automatic qt double buffering
	setAttribute(Qt::WA_OpaquePaintEvent);
	//setAttribute(Qt::WA_PaintOnScreen); // disable qt backing store (that would force us to trigger repaint() instead of the 10 times faster paintEvent(0))
#endif

	m_iFlushTimer = 0;
	m_pToolsPopup = 0;
	m_pFirstLine               = 0;
	m_pCurLine                 = 0;
	m_pLastLine                = 0;
	m_pCursorLine              = 0;
	m_uLineMarkLineIndex       = KVI_IRCVIEW_INVALID_LINE_MARK_INDEX;
	m_bHaveUnreadedHighlightedMessages = false;
	m_bHaveUnreadedMessages = false;
	m_iNumLines                = 0;
	m_iMaxLines                = KVI_OPTION_UINT(KviOption_uintIrcViewMaxBufferSize);

	m_uNextLineIndex           = 0;

	if(m_iMaxLines < 32)
	{
		m_iMaxLines = 32;
		KVI_OPTION_UINT(KviOption_uintIrcViewMaxBufferSize) = 32;
	} 

	m_bMouseIsDown               = false;

	//m_bShowImages              = KVI_OPTION_BOOL(KviOption_boolIrcViewShowImages);

	m_iSelectTimer             = 0;
	m_iMouseTimer		   = 0;
	//m_iTipTimer                = 0;
	//m_bTimestamp               = KVI_OPTION_BOOL(KviOption_boolIrcViewTimestamp);

	m_bAcceptDrops             = false;
	m_pPrivateBackgroundPixmap = 0;
	m_bSkipScrollBarRepaint    = false;
	m_pLogFile                 = 0;
	m_pKviWindow               = pWnd;
	m_pFrm                     = pFrm;

	m_iUnprocessedPaintEventRequests = 0;
	m_bPostedPaintEventPending = false;

	m_pLastLinkUnderMouse      = 0;
	m_iLastLinkRectTop         = -1;
	m_iLastLinkRectHeight      = -1;

	m_pMasterView              = 0;

	m_pToolWidget              = 0;
	
	m_pWrappedBlockSelectionInfo  = new KviIrcViewWrappedBlockSelectionInfo;
	

	m_pMessagesStoppedWhileSelecting = new KviPointerList<KviIrcViewLine>;
	m_pMessagesStoppedWhileSelecting->setAutoDelete(false);

	// say qt to avoid erasing on repaint
#ifdef COMPILE_USE_QT4
	setAutoFillBackground(false);
#else
	setBackgroundMode(NoBackground);
#endif
	
	m_pFm = 0; // will be updated in the first paint event
	
	m_pToolTip = new KviIrcViewToolTip(this);

	// Create the scroll bar
#ifdef COMPILE_USE_QT4
	m_pScrollBar               = new QScrollBar(0,0,1,10,0,Qt::Vertical,this,"irc_view_scrollbar");
#else
	m_pScrollBar               = new QScrollBar(0,0,1,10,0,QScrollBar::Vertical,this,"irc_view_scrollbar");
#endif
	m_pScrollBar->setTracking(true);
	m_pScrollBar->show();

	m_pScrollBar->setFocusProxy(this);


	m_pToolsButton = new KviStyledToolButton(this,"btntools");
#ifdef COMPILE_USE_QT4
	QIcon is1(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_POPUPMENU)));
	m_pToolsButton->setAutoRaise(true);
#else
	QIconSet is1(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_POPUPMENU)),QIconSet::Small);
#endif
	m_pToolsButton->setIconSet(is1);

	KviTalToolTip::add(m_pToolsButton,__tr2qs("Search tools"));
	m_pToolsButton->setFocusProxy(this);

	connect(m_pToolsButton,SIGNAL(clicked()),this,SLOT(showToolsPopup()));
	m_pToolsButton->show();

	connect(m_pScrollBar,SIGNAL(valueChanged(int)),this,SLOT(scrollBarPositionChanged(int)));
	m_iLastScrollBarValue      = 0;

	// set the minimum width
	setMinimumWidth(KVI_IRCVIEW_MINIMUM_WIDTH);
	// and catch all mouse events
	setMouseTracking(true);
	// let's go!
	applyOptions();
	
	if(KVI_OPTION_UINT(KviOption_uintAutoFlushLogs)) //m_iFlushTimer
	{
		m_iFlushTimer = startTimer(KVI_OPTION_UINT(KviOption_uintAutoFlushLogs)*60*1000);
	}
	
//	if(pWnd->input()) setFocusProxy(pWnd->input());

}

static inline void delete_text_line(KviIrcViewLine * l)
{
	for(unsigned int i=0;i<l->uChunkCount;i++)
	{
		if((l->pChunks[i].type == KVI_TEXT_ESCAPE) || (l->pChunks[i].type == KVI_TEXT_ICON))
		{
			if( (l->pChunks[i].type == KVI_TEXT_ICON) && (l->pChunks[i].szPayload!=l->pChunks[i].szSmileId) )
				kvi_free(l->pChunks[i].szSmileId);
			kvi_free(l->pChunks[i].szPayload);
		}
	}
	kvi_free(l->pChunks);                        //free attributes data
	if(l->iBlockCount)kvi_free(l->pBlocks);
	delete l;
}

KviIrcView::~KviIrcView()
{
	// kill any pending timer
	if(m_iFlushTimer) killTimer(m_iFlushTimer);
	if(m_iSelectTimer)killTimer(m_iSelectTimer);
	if(m_iMouseTimer)killTimer(m_iMouseTimer);
	// and close the log file (flush!)
	stopLogging();
	if(m_pToolWidget)delete m_pToolWidget;
	// don't forget the bacgkround pixmap!
	if(m_pPrivateBackgroundPixmap)delete m_pPrivateBackgroundPixmap;
	// and to remove all the text lines
	emptyBuffer(false);
	// the pending ones too!
	while(KviIrcViewLine * l = m_pMessagesStoppedWhileSelecting->first())
	{
		m_pMessagesStoppedWhileSelecting->removeFirst();
		delete_text_line(l);
	}
	delete m_pMessagesStoppedWhileSelecting;
	if(m_pFm)delete m_pFm;
	delete m_pToolTip;
	delete m_pWrappedBlockSelectionInfo;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// The IrcView : options
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void KviIrcView::setFont(const QFont &f)
{
	if(m_pFm)
	{
		// force an update to the font variables
		delete m_pFm;
		m_pFm = 0;
	}
	KviIrcViewLine * l = m_pFirstLine;
	while(l)
	{
		l->iMaxLineWidth = -1;
		l = l->pNext;
	}
	QWidget::setFont(f);
	update();
}

void KviIrcView::applyOptions()
{
	flushLog();
	setFont(KVI_OPTION_FONT(KviOption_fontIrcView));
	if(m_iFlushTimer) killTimer(m_iFlushTimer);
	if(KVI_OPTION_UINT(KviOption_uintAutoFlushLogs))
	{
		m_iFlushTimer = startTimer(KVI_OPTION_UINT(KviOption_uintAutoFlushLogs)*60*1000);
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// The IrcView : DnD   //2005.Resurection by Grifisx & Noldor
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void KviIrcView::enableDnd(bool bEnable) 
{
	setAcceptDrops(bEnable);
	m_bAcceptDrops = bEnable;
}

void KviIrcView::dragEnterEvent(QDragEnterEvent *e)
{
	if(!m_bAcceptDrops)return;
	e->accept(KviUriDrag::canDecode(e));
	emit dndEntered();
}

void KviIrcView::dropEvent(QDropEvent *e)
{
	if(!m_bAcceptDrops)return;
	QStringList list;
	if(KviUriDrag::decodeLocalFiles(e,list))
	{
		if(!list.isEmpty())
		{
			QStringList::ConstIterator it = list.begin(); //kewl ! :)
			for( ; it != list.end(); ++it )
			{
				QString tmp = *it; //wow :)
				#ifndef COMPILE_ON_WINDOWS
					if(tmp[0] != '/')tmp.prepend("/"); //HACK HACK HACK for Qt bug (?!?)
				#endif
				emit fileDropped(tmp);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// The IrcView : Logging
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void KviIrcView::stopLogging()
{
	if(m_pLogFile)
	{
		QString szLogEnd;
		szLogEnd.sprintf(__tr2qs("### Log session terminated at %s ###"),QDateTime::currentDateTime().toString().utf8().data());
		
		add2Log(szLogEnd);	
		m_pLogFile->close();
#ifdef COMPILE_ZLIB_SUPPORT
		if(KVI_OPTION_BOOL(KviOption_boolGzipLogs))
		{
			if(m_pLogFile->open(IO_ReadOnly))
			{
				QByteArray bytes;
				bytes=m_pLogFile->readAll();
				m_pLogFile->close();
				QFileInfo fi(*m_pLogFile);
				QString szFname=fi.dirPath(true)+QString("/")+fi.baseName(true);
				gzFile file=gzopen(QTextCodec::codecForLocale()->fromUnicode(szFname).data(),"ab9");
				if(file)
				{
					gzwrite(file,bytes.data(),bytes.size());
					gzclose(file);
					m_pLogFile->remove();
				} else {
					debug("Cannot open compressed stream");
				}
			}
		}
#endif
		delete m_pLogFile;
		m_pLogFile = 0;
	}
}

void KviIrcView::getLogFileName(KviStr &buffer)
{
	if(m_pLogFile)buffer.append(m_pLogFile->name());
}

void KviIrcView::getLogFileName(QString &buffer)
{
	if(m_pLogFile) buffer=m_pLogFile->name();
}

void KviIrcView::getTextBuffer(QString &buffer)
{
	// FIXME: #warning "This does not merge the KviChannel::m_pMessageView buffer!"
	buffer = "";
	if(!m_pLastLine)return;
	for(KviIrcViewLine *l=m_pFirstLine;l;l=l->pNext)
	{
		buffer.append(l->szText);
		buffer.append("\n");
	}
}

void KviIrcView::flushLog()
{
	if(m_pLogFile) {
#ifdef COMPILE_ZLIB_SUPPORT
		if(KVI_OPTION_BOOL(KviOption_boolGzipLogs))
		{
			m_pLogFile->close();
			if(m_pLogFile->open(IO_ReadOnly))
			{
				QByteArray bytes;
				bytes=m_pLogFile->readAll();
				m_pLogFile->close();
				QFileInfo fi(*m_pLogFile);
				QString szFname=fi.dirPath(true)+QString("/")+fi.baseName(true);
				gzFile file=gzopen(QTextCodec::codecForLocale()->fromUnicode(szFname).data(),"ab9");
				if(file)
				{
					gzwrite(file,bytes.data(),bytes.size());
					gzclose(file);
					m_pLogFile->remove();
				} else {
					debug("Cannot open compressed stream");
				}
			}
			m_pLogFile->open(IO_Append|IO_WriteOnly);
		} else 
#endif
		m_pLogFile->flush();
	}	
	else if(m_pMasterView)m_pMasterView->flushLog();
}

const QString & KviIrcView::lastMessageText()
{
	KviIrcViewLine * pCur=m_pLastLine;
	while(pCur)
	{
		switch(pCur->iMsgType)
		{
			case KVI_OUT_CHANPRIVMSG:
			case KVI_OUT_CHANPRIVMSGCRYPTED:
			case KVI_OUT_CHANNELNOTICE:
			case KVI_OUT_CHANNELNOTICECRYPTED:
			case KVI_OUT_ACTION:
			case KVI_OUT_OWNPRIVMSG:
			case KVI_OUT_OWNPRIVMSGCRYPTED:
			case KVI_OUT_HIGHLIGHT:
				return pCur->szText;
		}
		pCur=pCur->pPrev;
	}
	return KviQString::empty;
}

const QString & KviIrcView::lastLineOfText()
{
	if(!m_pLastLine)return KviQString::empty;
	return m_pLastLine->szText;
}

//void KviIrcView::toggleLogging()
//{
//	if(isLogging())stopLogging();
//	else {
//#warning "FIX THIS COMMENTED STUFF"
//
//		KviStr tmp;
//		m_pKviWindow->getDefaultLogName(tmp);
//		startLogging(tmp.ptr());
//
//	}
//}

void KviIrcView::setMasterView(KviIrcView * v)
{
	if(m_pMasterView)disconnect(this,SLOT(masterDead()));
	m_pMasterView = v;
	if(m_pMasterView)connect(m_pMasterView,SIGNAL(destroyed()),this,SLOT(masterDead()));
}

void KviIrcView::masterDead()
{
	m_pMasterView = 0;
}

bool KviIrcView::startLogging(const QString& fname,bool bPrependCurBuffer)
{
	stopLogging();
	QString szFname(fname);
	
	if(fname.isEmpty())
	{
		if(!m_pKviWindow)return false;
		m_pKviWindow->getDefaultLogFileName(szFname);
	}

#ifdef COMPILE_ZLIB_SUPPORT
	if(KVI_OPTION_BOOL(KviOption_boolGzipLogs))
		szFname+=".tmp";
#endif
	
	m_pLogFile = new QFile(szFname);

	if(m_pLogFile->exists())
	{
		if(!m_pLogFile->open(IO_Append|IO_WriteOnly))
		{
			delete m_pLogFile;
			m_pLogFile = 0;
			return false;
		}
	} else {
		if(!m_pLogFile->open(IO_WriteOnly))
		{
			delete m_pLogFile;
			m_pLogFile = 0;
			return false;
		}
	}

	QString szLogStart;
	szLogStart.sprintf(__tr2qs("### Log session started at %s ###"),QDateTime::currentDateTime().toString().utf8().data());
	add2Log(szLogStart);
	if(bPrependCurBuffer)
	{
		add2Log(__tr2qs("### Existing data buffer:"));
		QString buffer;
		getTextBuffer(buffer);
		add2Log(buffer);
		add2Log(__tr2qs("### End of existing data buffer."));
		m_pLogFile->flush();
	}

	return true;
}

void KviIrcView::add2Log(const QString &szBuffer,int iMsgType)
{
	QString szToWrite=QString("%1 %2\n").arg(iMsgType).arg(szBuffer);
	KviQCString szTmp = KviQString::toUtf8(szToWrite);
	if(m_pLogFile->writeBlock(szTmp.data(),szTmp.length())==-1) debug("WARNING : Can not write to the log file.");
}

//=============================================================================
//
// Some slots
//

//void KviIrcView::saveBufferToFile()
//{
//	// Yeah....this is powerful! :)
////	KviStr cmd = "/DIALOG (savefile,Choose a file name,$deflogfile($window).savebuf,$window) "
////		"if(\"$dialogresult\" != \"\")window $dialogmagic savebuffer $dialogresult";
////	m_pFrm->m_pUserParser->parseUserCommand(cmd,m_pKviWindow);
//}
/*
void KviIrcView::toggleTimestamp()
{
	setTimestamp(!timestamp());
}

void KviIrcView::toggleImages()
{
	setShowImages(!imagesVisible());
}*/

void KviIrcView::clearBuffer()
{
	emptyBuffer(true);
}

bool KviIrcView::saveBuffer(const char *filename)
{
	QFile f(QString::fromUtf8(filename));
	if(!f.open(IO_WriteOnly|IO_Truncate))return false;
	QString tmp;
	getTextBuffer(tmp);
	KviQCString tmpx = KviQString::toUtf8(tmp);
	f.writeBlock(tmpx.data(),tmpx.length());
	f.close();
	return true;
}

void KviIrcView::prevLine(){ m_pScrollBar->subtractLine(); }
void KviIrcView::nextLine(){ m_pScrollBar->addLine(); }
void KviIrcView::prevPage(){ m_pScrollBar->subtractPage(); }
void KviIrcView::nextPage(){ m_pScrollBar->addPage(); }

void KviIrcView::setPrivateBackgroundPixmap(const QPixmap &pixmap,bool bRepaint)
{
	if(m_pPrivateBackgroundPixmap)
	{
		delete m_pPrivateBackgroundPixmap;
		m_pPrivateBackgroundPixmap=0;
	}
	if(!pixmap.isNull())m_pPrivateBackgroundPixmap = new QPixmap(pixmap);

	if(bRepaint)
		update();
}

void KviIrcView::emptyBuffer(bool bRepaint)
{
	while(m_pLastLine != 0)removeHeadLine();
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

void KviIrcView::checkLogDate()
{
	QDate::currentDate();
}

void KviIrcView::clearUnreaded()
{
	m_bHaveUnreadedHighlightedMessages = false;
	m_bHaveUnreadedMessages = false;
	
	if(m_pFrm)
		if(m_pFrm->dockExtension())
			m_pFrm->dockExtension()->refresh();
}

void KviIrcView::setMaxBufferSize(int maxBufSize,bool bRepaint)
{
	if(maxBufSize < 32)maxBufSize = 32;
	m_iMaxLines = maxBufSize;
	while(m_iNumLines > m_iMaxLines)removeHeadLine();
	m_pScrollBar->setRange(0,m_iNumLines);
	if(bRepaint)
		update();
}

/*
void KviIrcView::setShowImages(bool bShow,bool bRepaint)
{
	if(m_bShowImages!=bShow)
	{
		m_bShowImages=bShow;
		if(bRepaint)paintEvent(0);
	}
}
*/
/*
void KviIrcView::setTimestamp(bool bTimestamp)
{
	m_bTimestamp = bTimestamp;


// STATS FOR A BUFFER FULL OF HIGHLY COLORED STRINGS , HIGHLY WRAPPED
//
// Lines = 1024 (322425 bytes - 314 KB) (avg 314 bytes per line) , well :)
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
//	debug("\n\nLines = %u (%u bytes - %u KB) (avg %u bytes per line)",nLines,nAlloc,nAlloc / 1024,nLines ? (nAlloc / nLines) : 0);
//	debug("string bytes = %u (%u KB)",nStringBytes,nStringBytes / 1024);
//	debug("attributes = %u (%u bytes - %u KB)",nAttributes,nAttrBytes,nAttrBytes / 1024);
//	debug("blocks = %u (%u bytes - %u KB)\n",nBlocks,nBlockBytes,nBlockBytes / 1024);

}
*/
void KviIrcView::scrollBarPositionChanged(int newValue)
{
	if(!m_pCurLine)return;
	int diff = 0;
	if(newValue > m_iLastScrollBarValue)
	{
		while(newValue > m_iLastScrollBarValue)
		{
			if(m_pCurLine->pNext)
			{
				m_pCurLine=m_pCurLine->pNext;
				diff++;
			}
			m_iLastScrollBarValue++;
		}
	} else {
		while(newValue < m_iLastScrollBarValue)
		{
			if(m_pCurLine->pPrev)m_pCurLine=m_pCurLine->pPrev;
			m_iLastScrollBarValue--;
		}
	}
	if(!m_bSkipScrollBarRepaint)
		repaint();
		//update();
//	if(!m_bSkipScrollBarRepaint)postUpdateEvent();
}

bool KviIrcView::event(QEvent *e)
{
	if(e->type() == QEvent::User)
	{
		__range_valid(m_bPostedPaintEventPending);
		if(m_iUnprocessedPaintEventRequests)
			repaint();
		// else we just had a pointEvent that did the job
		m_bPostedPaintEventPending = false;
		return true;
	}
	return QWidget::event(e);
}

void KviIrcView::wheelEvent(QWheelEvent *e)
{
#ifdef COMPILE_USE_QT4
	static bool bHere = false;
	if(bHere)return;
	bHere = true; // Qt4 tends to jump into infinite recursion here
#endif
	g_pApp->sendEvent(m_pScrollBar,e);
#ifdef COMPILE_USE_QT4
	bHere = false;
#endif
}


void KviIrcView::postUpdateEvent()
{
	// This will post a QEvent with a full repaint request
	if(!m_bPostedPaintEventPending)
	{
		m_bPostedPaintEventPending = true;
		QEvent *e = new QEvent(QEvent::User);
		g_pApp->postEvent(this,e); // queue a repaint
	}

	m_iUnprocessedPaintEventRequests++; // paintEvent() will set it to 0

	if(m_iUnprocessedPaintEventRequests == 3)
	{
		// Three unprocessed paint events...do it now
#ifdef COMPILE_PSEUDO_TRANSPARENCY
		if(! ((KVI_OPTION_PIXMAP(KviOption_pixmapIrcViewBackground).pixmap()) || m_pPrivateBackgroundPixmap || g_pShadedChildGlobalDesktopBackground))
			fastScroll(3);
#else
		if(! ((KVI_OPTION_PIXMAP(KviOption_pixmapIrcViewBackground).pixmap()) || m_pPrivateBackgroundPixmap))
			fastScroll(3);
#endif
		else
			repaint();
	}
}

void KviIrcView::appendLine(KviIrcViewLine *ptr,bool bRepaint)
{
	//This one appends a KviIrcViewLine to
	//the buffer list (at the end)
	if(m_bMouseIsDown)
	{
		// Do not move the view!
		// So we append the text line to a temp queue
		// and then we'll add it when the mouse button is released
		m_pMessagesStoppedWhileSelecting->append(ptr);
		return;
	}

	// First log the line and assign the index
	// Don't use add2log here!...we must go as fast as possible, so we avoid some push and pop calls, and also a couple of branches
	if(m_pLogFile && KVI_OPTION_BOOL(KviOption_boolStripControlCodesInLogs))
	{
		// a slave view has no log files!
		if(KVI_OPTION_MSGTYPE(ptr->iMsgType).logEnabled())
		{
			add2Log(ptr->szText,ptr->iMsgType);
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
	} else {
		// no log: we could have master view!
		if(m_pMasterView)
		{
			if(m_pMasterView->m_pLogFile && KVI_OPTION_BOOL(KviOption_boolStripControlCodesInLogs))
			{
				if(KVI_OPTION_MSGTYPE(ptr->iMsgType).logEnabled())
				{
					m_pMasterView->add2Log(ptr->szText,ptr->iMsgType);
				}
			}
			ptr->uIndex = m_pMasterView->m_uNextLineIndex;
			m_pMasterView->m_uNextLineIndex++;
		} else {
			ptr->uIndex = m_uNextLineIndex;
			m_uNextLineIndex++;
		}
	}

	if(m_pLastLine)
	{
		// There is at least one line in the view
		m_pLastLine->pNext=ptr;
		ptr->pPrev  =m_pLastLine;
		ptr->pNext  =0;
		m_iNumLines++;

		if(m_iNumLines > m_iMaxLines)
		{
			// Too many lines in the view...remove one
			removeHeadLine();
			if(m_pCurLine==m_pLastLine)
			{
				m_pCurLine=ptr;
				if(bRepaint)
					postUpdateEvent();
			} else {
				// the cur line remains the same
				// the scroll bar must move up one place to be in sync
				m_bSkipScrollBarRepaint = true;
				if(m_pScrollBar->value() > 0)
				{
					m_iLastScrollBarValue--;
					__range_valid(m_iLastScrollBarValue >= 0);
					m_pScrollBar->subtractLine();
				} // else will stay in sync
				m_bSkipScrollBarRepaint = false;
			}
		} else {
			// Just append
			m_pScrollBar->setRange(0,m_iNumLines);
			if(m_pCurLine==m_pLastLine)
			{
				m_bSkipScrollBarRepaint = true;
				m_pScrollBar->addLine();
				m_bSkipScrollBarRepaint = false;
				if(bRepaint)
					postUpdateEvent();
			}
		}
		m_pLastLine=ptr;
	} else {
		//First line
		m_pLastLine    = ptr;
		m_pFirstLine   = ptr;
		m_pCurLine     = ptr;
		ptr->pPrev = 0;
		ptr->pNext = 0;
		m_iNumLines    = 1;
		m_pScrollBar->setRange(0,1);
		m_pScrollBar->addLine();
		if(bRepaint)
			postUpdateEvent();
	}
}

//============== removeHeadLine ===============//

void KviIrcView::removeHeadLine(bool bRepaint)
{
	//Removes the first line of the text buffer
	if(!m_pLastLine)return;
	if(m_pFirstLine == m_pCursorLine)m_pCursorLine = 0;

	if(m_pFirstLine->pNext)
	{
		KviIrcViewLine *aux_ptr=m_pFirstLine->pNext;     //get the next line
		aux_ptr->pPrev=0;                                    //becomes the first
		if(m_pFirstLine==m_pCurLine)m_pCurLine=aux_ptr;      //move the cur line if necessary
		delete_text_line(m_pFirstLine);                      //delete the struct
		m_pFirstLine=aux_ptr;                                //set the last
		m_iNumLines--;                                       //and decrement the count
	} else { //unique line
		m_pCurLine   = 0;
		delete_text_line(m_pFirstLine);
		m_pFirstLine = 0;
		m_iNumLines  = 0;
		m_pLastLine  = 0;
	}
	if(bRepaint)
		repaint();
}

void KviIrcView::splitMessagesTo(KviIrcView *v)
{
	v->emptyBuffer(false);
	
	KviIrcViewLine * l = m_pFirstLine;
	KviIrcViewLine * tmp;
	while(l)
		switch(l->iMsgType)
	{
		case KVI_OUT_CHANPRIVMSG:
		case KVI_OUT_CHANPRIVMSGCRYPTED:
		case KVI_OUT_CHANNELNOTICE:
		case KVI_OUT_CHANNELNOTICECRYPTED:
		case KVI_OUT_ACTION:
		case KVI_OUT_OWNPRIVMSG:
		case KVI_OUT_OWNPRIVMSGCRYPTED:
		case KVI_OUT_HIGHLIGHT:
		{
			m_iNumLines--;
			v->m_iNumLines++;

			if(l->pNext)l->pNext->pPrev = l->pPrev;
			if(l->pPrev)l->pPrev->pNext = l->pNext;
			if(l == m_pFirstLine)m_pFirstLine = l->pNext;
			if(l == m_pLastLine)m_pLastLine = l->pPrev;
			if(v->m_pLastLine)
			{
				v->m_pLastLine->pNext = l;
				l->pPrev = v->m_pLastLine;
				v->m_pLastLine = l;
			} else {
				v->m_pFirstLine = l;
				l->pPrev = 0;
				v->m_pLastLine = l;
			}
			tmp = l->pNext;
			l->pNext = 0;
			l = tmp;
		}
		break;
		default:
			l = l->pNext;
		break;
	}
	v->m_pCurLine = v->m_pLastLine;
	m_pCurLine = m_pLastLine;

	v->m_pCursorLine = 0;
	m_pCursorLine = 0;

	m_iLastScrollBarValue = m_iNumLines;
	m_pScrollBar->setRange(0,m_iNumLines);
	m_pScrollBar->setValue(m_iNumLines);
	
	repaint();

	v->m_iLastScrollBarValue = v->m_iNumLines;
	v->m_pScrollBar->setRange(0,v->m_iNumLines);
	v->m_pScrollBar->setValue(v->m_iNumLines);
	v->repaint();

}

void KviIrcView::appendMessagesFrom(KviIrcView *v)
{
	if(!m_pLastLine)
	{
		m_pFirstLine = v->m_pFirstLine;
	} else {
		m_pLastLine->pNext = v->m_pFirstLine;
		v->m_pFirstLine->pPrev = m_pLastLine;
	}
	m_pLastLine  = v->m_pLastLine;
	m_pCurLine = m_pLastLine;
	m_pCursorLine = 0;
	v->m_pFirstLine = 0;
	v->m_pLastLine = 0;
	v->m_pCurLine = 0;
	v->m_pCursorLine = 0;
	m_iNumLines += v->m_iNumLines;
	v->m_iNumLines = 0;
//	v->m_pScrollBar->setRange(0,0);
//	v->m_pScrollBar->setValue(0);
	m_iLastScrollBarValue = m_iNumLines;
	m_pScrollBar->setRange(0,m_iNumLines);
	m_pScrollBar->setValue(m_iNumLines);
	
	repaint();
}

void KviIrcView::joinMessagesFrom(KviIrcView *v)
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
				if(l1->pPrev)l1->pPrev->pNext = l2;
				else m_pFirstLine = l2;
				l1->pPrev = l2;
				tmp = l2->pNext;
				l2->pNext = l1;
				l2 = tmp;
			} else {
				// the external message is younger than the current internal one
				l1 = l1->pNext;
			}
		} else {
			// There is no current internal message (ran over the end)
			// merge at the end then
			if(m_pFirstLine)
			{
				m_pLastLine->pNext = l2;
				l2->pPrev = m_pLastLine;
			} else {
				m_pFirstLine = l2;
				l2->pPrev = 0;
			}
			tmp = l2->pNext;
			l2->pNext = 0;
			m_pLastLine  = l2;
			l2 = tmp;
		}
	}

	m_pCurLine = m_pLastLine;
	m_pCursorLine = 0;
	v->m_pFirstLine = 0;
	v->m_pLastLine = 0;
	v->m_pCurLine = 0;
	v->m_pCursorLine = 0;
	m_iNumLines += v->m_iNumLines;
	v->m_iNumLines = 0;
//	v->m_pScrollBar->setRange(0,0);
//	v->m_pScrollBar->setValue(0);

	m_iLastScrollBarValue = m_iNumLines;
	m_pScrollBar->setRange(0,m_iNumLines);
	m_pScrollBar->setValue(m_iNumLines);
	
	repaint();
}

void KviIrcView::appendText(int iMsgType,const kvi_wchar_t *data_ptr,int iFlags)
{
	//appends a text string to the buffer list
	//splits the lines
	__range_valid(data_ptr);
	m_pLastLinkUnderMouse = 0;

	while(*data_ptr)
	{                                         //Have more data
		KviIrcViewLine *line_ptr=new KviIrcViewLine;  //create a line struct
		line_ptr->iMsgType=iMsgType;
		line_ptr->iMaxLineWidth=-1;
		line_ptr->iBlockCount=0;
		
		if(!KVI_OPTION_BOOL(KviOption_boolStripControlCodesInLogs))
		{
			QString szBuffer;
			kvi_appendWCharToQStringWithLength(&szBuffer,data_ptr,kvi_wstrlen(data_ptr));
			szBuffer.prepend(QDateTime::currentDateTime().toString("[h:mm:ss] "));
			if(m_pLogFile && KVI_OPTION_MSGTYPE(iMsgType).logEnabled())
			{
				add2Log(szBuffer,iMsgType);
			} else if(m_pMasterView) {
				if(m_pMasterView->m_pLogFile && KVI_OPTION_MSGTYPE(iMsgType).logEnabled())
				{
					m_pMasterView->add2Log(szBuffer,iMsgType);
				}
			}
		}
	
		data_ptr=getTextLine(iMsgType,data_ptr,line_ptr,!(iFlags & NoTimestamp));
		appendLine(line_ptr,!(iFlags & NoRepaint));
		if(iFlags & SetLineMark)
		{
			if(KVI_OPTION_BOOL(KviOption_boolTrackLastReadTextViewLine))
			{
				m_uLineMarkLineIndex = line_ptr->uIndex;
				iFlags &= ~SetLineMark;
			}
			m_bHaveUnreadedHighlightedMessages = m_bHaveUnreadedHighlightedMessages || iMsgType == KVI_OUT_HIGHLIGHT;
			m_bHaveUnreadedMessages = m_bHaveUnreadedMessages ||
				iMsgType == KVI_OUT_CHANPRIVMSG ||
				iMsgType == KVI_OUT_CHANPRIVMSGCRYPTED ||
				iMsgType == KVI_OUT_CHANNELNOTICE ||
				iMsgType == KVI_OUT_CHANNELNOTICECRYPTED ||
				iMsgType == KVI_OUT_ACTION ||
				iMsgType == KVI_OUT_OWNPRIVMSGCRYPTED ||
				iMsgType == KVI_OUT_HIGHLIGHT;
		}
	}
}


void KviIrcView::getLinkEscapeCommand(QString &buffer,const QString &szPayload,const QString &escape_label)
{
	if(szPayload.isEmpty())return;

	int idx = szPayload.find(escape_label);
	if(idx == -1)return;
	idx += escape_label.length();
	
	int idx2 = szPayload.find("[!",idx);
	int len = idx2 == -1 ? szPayload.length() - idx : idx2 - idx;

	buffer = szPayload.mid(idx,len);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// The IrcView : Get text line
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static kvi_wchar_t case_xtx_XX[256] =
{
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000
};

static kvi_wchar_t case_ltu_00[256] =
{
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0130, 0x004A, 0x212A, 0x004C, 0x004D, 0x004E, 0x004F,
	0x0050, 0x0051, 0x0052, 0x017F, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x212B, 0x00C6, 0x00C7, 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
	0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x0000, 0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x0178
};


static kvi_wchar_t case_ltu_01[256] =
{
	0x0000, 0x0100, 0x0000, 0x0102, 0x0000, 0x0104, 0x0000, 0x0106, 0x0000, 0x0108, 0x0000, 0x010A, 0x0000, 0x010C, 0x0000, 0x010E,
	0x0000, 0x0110, 0x0000, 0x0112, 0x0000, 0x0114, 0x0000, 0x0116, 0x0000, 0x0118, 0x0000, 0x011A, 0x0000, 0x011C, 0x0000, 0x011E,
	0x0000, 0x0120, 0x0000, 0x0122, 0x0000, 0x0124, 0x0000, 0x0126, 0x0000, 0x0128, 0x0000, 0x012A, 0x0000, 0x012C, 0x0000, 0x012E,
	0x0000, 0x0049, 0x0000, 0x0132, 0x0000, 0x0134, 0x0000, 0x0136, 0x0000, 0x0000, 0x0139, 0x0000, 0x013B, 0x0000, 0x013D, 0x0000,
	0x013F, 0x0000, 0x0141, 0x0000, 0x0143, 0x0000, 0x0145, 0x0000, 0x0147, 0x0000, 0x0000, 0x014A, 0x0000, 0x014C, 0x0000, 0x014E,
	0x0000, 0x0150, 0x0000, 0x0152, 0x0000, 0x0154, 0x0000, 0x0156, 0x0000, 0x0158, 0x0000, 0x015A, 0x0000, 0x015C, 0x0000, 0x015E,
	0x0000, 0x0160, 0x0000, 0x0162, 0x0000, 0x0164, 0x0000, 0x0166, 0x0000, 0x0168, 0x0000, 0x016A, 0x0000, 0x016C, 0x0000, 0x016E,
	0x0000, 0x0170, 0x0000, 0x0172, 0x0000, 0x0174, 0x0000, 0x0176, 0x0000, 0x0000, 0x0179, 0x0000, 0x017B, 0x0000, 0x017D, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0182, 0x0000, 0x0184, 0x0000, 0x0000, 0x0187, 0x0000, 0x0000, 0x0000, 0x018B, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0191, 0x0000, 0x0000, 0x01F6, 0x0000, 0x0000, 0x0000, 0x0198, 0x0000, 0x0000, 0x0000, 0x0000, 0x0220, 0x0000,
	0x0000, 0x01A0, 0x0000, 0x01A2, 0x0000, 0x01A4, 0x0000, 0x0000, 0x01A7, 0x0000, 0x0000, 0x0000, 0x0000, 0x01AC, 0x0000, 0x0000,
	0x01AF, 0x0000, 0x0000, 0x0000, 0x01B3, 0x0000, 0x01B5, 0x0000, 0x0000, 0x01B8, 0x0000, 0x0000, 0x0000, 0x01BC, 0x0000, 0x01F7,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x01C5, 0x0000, 0x0000, 0x01C8, 0x0000, 0x0000, 0x01CB, 0x0000, 0x01CD, 0x0000,
	0x01CF, 0x0000, 0x01D1, 0x0000, 0x01D3, 0x0000, 0x01D5, 0x0000, 0x01D7, 0x0000, 0x01D9, 0x0000, 0x01DB, 0x018E, 0x0000, 0x01DE,
	0x0000, 0x01E0, 0x0000, 0x01E2, 0x0000, 0x01E4, 0x0000, 0x01E6, 0x0000, 0x01E8, 0x0000, 0x01EA, 0x0000, 0x01EC, 0x0000, 0x01EE,
	0x0000, 0x0000, 0x0000, 0x01F2, 0x0000, 0x01F4, 0x0000, 0x0000, 0x0000, 0x01F8, 0x0000, 0x01FA, 0x0000, 0x01FC, 0x0000, 0x01FE
};


static kvi_wchar_t case_ltu_02[256] =
{
	0x0000, 0x0200, 0x0000, 0x0202, 0x0000, 0x0204, 0x0000, 0x0206, 0x0000, 0x0208, 0x0000, 0x020A, 0x0000, 0x020C, 0x0000, 0x020E,
	0x0000, 0x0210, 0x0000, 0x0212, 0x0000, 0x0214, 0x0000, 0x0216, 0x0000, 0x0218, 0x0000, 0x021A, 0x0000, 0x021C, 0x0000, 0x021E,
	0x0000, 0x0000, 0x0000, 0x0222, 0x0000, 0x0224, 0x0000, 0x0226, 0x0000, 0x0228, 0x0000, 0x022A, 0x0000, 0x022C, 0x0000, 0x022E,
	0x0000, 0x0230, 0x0000, 0x0232, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0181, 0x0186, 0x0000, 0x0189, 0x018A, 0x0000, 0x018F, 0x0000, 0x0190, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0193, 0x0000, 0x0000, 0x0194, 0x0000, 0x0000, 0x0000, 0x0000, 0x0197, 0x0196, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x019C,
	0x0000, 0x0000, 0x019D, 0x0000, 0x0000, 0x019F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x01A6, 0x0000, 0x0000, 0x01A9, 0x0000, 0x0000, 0x0000, 0x0000, 0x01AE, 0x0000, 0x01B1, 0x01B2, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x01B7, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};


static kvi_wchar_t case_ltu_03[256] =
{
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0386, 0x0388, 0x0389, 0x038A,
	0x0000, 0x0391, 0x03D0, 0x0393, 0x0394, 0x03F5, 0x0396, 0x0397, 0x03F4, 0x1FBE, 0x03F0, 0x039B, 0x039C, 0x039D, 0x039E, 0x039F,
	0x03D6, 0x03F1, 0x0000, 0x03F2, 0x03A4, 0x03A5, 0x03D5, 0x03A7, 0x03A8, 0x2126, 0x03AA, 0x03AB, 0x038C, 0x038E, 0x038F, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x03D8, 0x0000, 0x03DA, 0x0000, 0x03DC, 0x0000, 0x03DE,
	0x0000, 0x03E0, 0x0000, 0x03E2, 0x0000, 0x03E4, 0x0000, 0x03E6, 0x0000, 0x03E8, 0x0000, 0x03EA, 0x0000, 0x03EC, 0x0000, 0x03EE,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};


static kvi_wchar_t case_ltu_04[256] =
{
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417, 0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F,
	0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427, 0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,
	0x0400, 0x0401, 0x0402, 0x0403, 0x0404, 0x0405, 0x0406, 0x0407, 0x0408, 0x0409, 0x040A, 0x040B, 0x040C, 0x040D, 0x040E, 0x040F,
	0x0000, 0x0460, 0x0000, 0x0462, 0x0000, 0x0464, 0x0000, 0x0466, 0x0000, 0x0468, 0x0000, 0x046A, 0x0000, 0x046C, 0x0000, 0x046E,
	0x0000, 0x0470, 0x0000, 0x0472, 0x0000, 0x0474, 0x0000, 0x0476, 0x0000, 0x0478, 0x0000, 0x047A, 0x0000, 0x047C, 0x0000, 0x047E,
	0x0000, 0x0480, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x048A, 0x0000, 0x048C, 0x0000, 0x048E,
	0x0000, 0x0490, 0x0000, 0x0492, 0x0000, 0x0494, 0x0000, 0x0496, 0x0000, 0x0498, 0x0000, 0x049A, 0x0000, 0x049C, 0x0000, 0x049E,
	0x0000, 0x04A0, 0x0000, 0x04A2, 0x0000, 0x04A4, 0x0000, 0x04A6, 0x0000, 0x04A8, 0x0000, 0x04AA, 0x0000, 0x04AC, 0x0000, 0x04AE,
	0x0000, 0x04B0, 0x0000, 0x04B2, 0x0000, 0x04B4, 0x0000, 0x04B6, 0x0000, 0x04B8, 0x0000, 0x04BA, 0x0000, 0x04BC, 0x0000, 0x04BE,
	0x0000, 0x0000, 0x04C1, 0x0000, 0x04C3, 0x0000, 0x04C5, 0x0000, 0x04C7, 0x0000, 0x04C9, 0x0000, 0x04CB, 0x0000, 0x04CD, 0x0000,
	0x0000, 0x04D0, 0x0000, 0x04D2, 0x0000, 0x04D4, 0x0000, 0x04D6, 0x0000, 0x04D8, 0x0000, 0x04DA, 0x0000, 0x04DC, 0x0000, 0x04DE,
	0x0000, 0x04E0, 0x0000, 0x04E2, 0x0000, 0x04E4, 0x0000, 0x04E6, 0x0000, 0x04E8, 0x0000, 0x04EA, 0x0000, 0x04EC, 0x0000, 0x04EE,
	0x0000, 0x04F0, 0x0000, 0x04F2, 0x0000, 0x04F4, 0x0000, 0x0000, 0x0000, 0x04F8, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};


static kvi_wchar_t case_ltu_05[256] =
{
	0x0000, 0x0500, 0x0000, 0x0502, 0x0000, 0x0504, 0x0000, 0x0506, 0x0000, 0x0508, 0x0000, 0x050A, 0x0000, 0x050C, 0x0000, 0x050E,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0531, 0x0532, 0x0533, 0x0534, 0x0535, 0x0536, 0x0537, 0x0538, 0x0539, 0x053A, 0x053B, 0x053C, 0x053D, 0x053E, 0x053F,
	0x0540, 0x0541, 0x0542, 0x0543, 0x0544, 0x0545, 0x0546, 0x0547, 0x0548, 0x0549, 0x054A, 0x054B, 0x054C, 0x054D, 0x054E, 0x054F,
	0x0550, 0x0551, 0x0552, 0x0553, 0x0554, 0x0555, 0x0556, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};


static kvi_wchar_t case_ltu_1E[256] =
{
	0x0000, 0x1E00, 0x0000, 0x1E02, 0x0000, 0x1E04, 0x0000, 0x1E06, 0x0000, 0x1E08, 0x0000, 0x1E0A, 0x0000, 0x1E0C, 0x0000, 0x1E0E,
	0x0000, 0x1E10, 0x0000, 0x1E12, 0x0000, 0x1E14, 0x0000, 0x1E16, 0x0000, 0x1E18, 0x0000, 0x1E1A, 0x0000, 0x1E1C, 0x0000, 0x1E1E,
	0x0000, 0x1E20, 0x0000, 0x1E22, 0x0000, 0x1E24, 0x0000, 0x1E26, 0x0000, 0x1E28, 0x0000, 0x1E2A, 0x0000, 0x1E2C, 0x0000, 0x1E2E,
	0x0000, 0x1E30, 0x0000, 0x1E32, 0x0000, 0x1E34, 0x0000, 0x1E36, 0x0000, 0x1E38, 0x0000, 0x1E3A, 0x0000, 0x1E3C, 0x0000, 0x1E3E,
	0x0000, 0x1E40, 0x0000, 0x1E42, 0x0000, 0x1E44, 0x0000, 0x1E46, 0x0000, 0x1E48, 0x0000, 0x1E4A, 0x0000, 0x1E4C, 0x0000, 0x1E4E,
	0x0000, 0x1E50, 0x0000, 0x1E52, 0x0000, 0x1E54, 0x0000, 0x1E56, 0x0000, 0x1E58, 0x0000, 0x1E5A, 0x0000, 0x1E5C, 0x0000, 0x1E5E,
	0x0000, 0x1E9B, 0x0000, 0x1E62, 0x0000, 0x1E64, 0x0000, 0x1E66, 0x0000, 0x1E68, 0x0000, 0x1E6A, 0x0000, 0x1E6C, 0x0000, 0x1E6E,
	0x0000, 0x1E70, 0x0000, 0x1E72, 0x0000, 0x1E74, 0x0000, 0x1E76, 0x0000, 0x1E78, 0x0000, 0x1E7A, 0x0000, 0x1E7C, 0x0000, 0x1E7E,
	0x0000, 0x1E80, 0x0000, 0x1E82, 0x0000, 0x1E84, 0x0000, 0x1E86, 0x0000, 0x1E88, 0x0000, 0x1E8A, 0x0000, 0x1E8C, 0x0000, 0x1E8E,
	0x0000, 0x1E90, 0x0000, 0x1E92, 0x0000, 0x1E94, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x1EA0, 0x0000, 0x1EA2, 0x0000, 0x1EA4, 0x0000, 0x1EA6, 0x0000, 0x1EA8, 0x0000, 0x1EAA, 0x0000, 0x1EAC, 0x0000, 0x1EAE,
	0x0000, 0x1EB0, 0x0000, 0x1EB2, 0x0000, 0x1EB4, 0x0000, 0x1EB6, 0x0000, 0x1EB8, 0x0000, 0x1EBA, 0x0000, 0x1EBC, 0x0000, 0x1EBE,
	0x0000, 0x1EC0, 0x0000, 0x1EC2, 0x0000, 0x1EC4, 0x0000, 0x1EC6, 0x0000, 0x1EC8, 0x0000, 0x1ECA, 0x0000, 0x1ECC, 0x0000, 0x1ECE,
	0x0000, 0x1ED0, 0x0000, 0x1ED2, 0x0000, 0x1ED4, 0x0000, 0x1ED6, 0x0000, 0x1ED8, 0x0000, 0x1EDA, 0x0000, 0x1EDC, 0x0000, 0x1EDE,
	0x0000, 0x1EE0, 0x0000, 0x1EE2, 0x0000, 0x1EE4, 0x0000, 0x1EE6, 0x0000, 0x1EE8, 0x0000, 0x1EEA, 0x0000, 0x1EEC, 0x0000, 0x1EEE,
	0x0000, 0x1EF0, 0x0000, 0x1EF2, 0x0000, 0x1EF4, 0x0000, 0x1EF6, 0x0000, 0x1EF8, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};


static kvi_wchar_t case_ltu_1F[256] =
{
	0x1F08, 0x1F09, 0x1F0A, 0x1F0B, 0x1F0C, 0x1F0D, 0x1F0E, 0x1F0F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x1F18, 0x1F19, 0x1F1A, 0x1F1B, 0x1F1C, 0x1F1D, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x1F28, 0x1F29, 0x1F2A, 0x1F2B, 0x1F2C, 0x1F2D, 0x1F2E, 0x1F2F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x1F38, 0x1F39, 0x1F3A, 0x1F3B, 0x1F3C, 0x1F3D, 0x1F3E, 0x1F3F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x1F48, 0x1F49, 0x1F4A, 0x1F4B, 0x1F4C, 0x1F4D, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x1F59, 0x0000, 0x1F5B, 0x0000, 0x1F5D, 0x0000, 0x1F5F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x1F68, 0x1F69, 0x1F6A, 0x1F6B, 0x1F6C, 0x1F6D, 0x1F6E, 0x1F6F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x1FBA, 0x1FBB, 0x1FC8, 0x1FC9, 0x1FCA, 0x1FCB, 0x1FDA, 0x1FDB, 0x1FF8, 0x1FF9, 0x1FEA, 0x1FEB, 0x1FFA, 0x1FFB, 0x0000, 0x0000,
	0x1F88, 0x1F89, 0x1F8A, 0x1F8B, 0x1F8C, 0x1F8D, 0x1F8E, 0x1F8F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x1F98, 0x1F99, 0x1F9A, 0x1F9B, 0x1F9C, 0x1F9D, 0x1F9E, 0x1F9F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x1FA8, 0x1FA9, 0x1FAA, 0x1FAB, 0x1FAC, 0x1FAD, 0x1FAE, 0x1FAF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x1FB8, 0x1FB9, 0x0000, 0x1FBC, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x1FCC, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x1FD8, 0x1FD9, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x1FE8, 0x1FE9, 0x0000, 0x0000, 0x0000, 0x1FEC, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x1FFC, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};


static kvi_wchar_t case_ltu_21[256] =
{
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x2160, 0x2161, 0x2162, 0x2163, 0x2164, 0x2165, 0x2166, 0x2167, 0x2168, 0x2169, 0x216A, 0x216B, 0x216C, 0x216D, 0x216E, 0x216F,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};


static kvi_wchar_t case_ltu_24[256] =
{
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x24B6, 0x24B7, 0x24B8, 0x24B9, 0x24BA, 0x24BB, 0x24BC, 0x24BD, 0x24BE, 0x24BF, 0x24C0, 0x24C1, 0x24C2, 0x24C3, 0x24C4, 0x24C5,
	0x24C6, 0x24C7, 0x24C8, 0x24C9, 0x24CA, 0x24CB, 0x24CC, 0x24CD, 0x24CE, 0x24CF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};


static kvi_wchar_t case_ltu_FF[256] =
{
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0xFF21, 0xFF22, 0xFF23, 0xFF24, 0xFF25, 0xFF26, 0xFF27, 0xFF28, 0xFF29, 0xFF2A, 0xFF2B, 0xFF2C, 0xFF2D, 0xFF2E, 0xFF2F,
	0xFF30, 0xFF31, 0xFF32, 0xFF33, 0xFF34, 0xFF35, 0xFF36, 0xFF37, 0xFF38, 0xFF39, 0xFF3A, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};


kvi_wchar_t * kvirc_case_map_ltu[256] =
{
	case_ltu_00, case_ltu_01, case_ltu_02, case_ltu_03, case_ltu_04, case_ltu_05, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_ltu_1E, case_ltu_1F,
	case_xtx_XX, case_ltu_21, case_xtx_XX, case_xtx_XX, case_ltu_24, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_ltu_FF
};


static kvi_wchar_t case_utl_00[256] =
{
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0131, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
	0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x03BC, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7, 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
	0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x0000, 0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};


static kvi_wchar_t case_utl_01[256] =
{
	0x0101, 0x0000, 0x0103, 0x0000, 0x0105, 0x0000, 0x0107, 0x0000, 0x0109, 0x0000, 0x010B, 0x0000, 0x010D, 0x0000, 0x010F, 0x0000,
	0x0111, 0x0000, 0x0113, 0x0000, 0x0115, 0x0000, 0x0117, 0x0000, 0x0119, 0x0000, 0x011B, 0x0000, 0x011D, 0x0000, 0x011F, 0x0000,
	0x0121, 0x0000, 0x0123, 0x0000, 0x0125, 0x0000, 0x0127, 0x0000, 0x0129, 0x0000, 0x012B, 0x0000, 0x012D, 0x0000, 0x012F, 0x0000,
	0x0069, 0x0000, 0x0133, 0x0000, 0x0135, 0x0000, 0x0137, 0x0000, 0x0000, 0x013A, 0x0000, 0x013C, 0x0000, 0x013E, 0x0000, 0x0140,
	0x0000, 0x0142, 0x0000, 0x0144, 0x0000, 0x0146, 0x0000, 0x0148, 0x0000, 0x0000, 0x014B, 0x0000, 0x014D, 0x0000, 0x014F, 0x0000,
	0x0151, 0x0000, 0x0153, 0x0000, 0x0155, 0x0000, 0x0157, 0x0000, 0x0159, 0x0000, 0x015B, 0x0000, 0x015D, 0x0000, 0x015F, 0x0000,
	0x0161, 0x0000, 0x0163, 0x0000, 0x0165, 0x0000, 0x0167, 0x0000, 0x0169, 0x0000, 0x016B, 0x0000, 0x016D, 0x0000, 0x016F, 0x0000,
	0x0171, 0x0000, 0x0173, 0x0000, 0x0175, 0x0000, 0x0177, 0x0000, 0x00FF, 0x017A, 0x0000, 0x017C, 0x0000, 0x017E, 0x0000, 0x0073,
	0x0000, 0x0253, 0x0183, 0x0000, 0x0185, 0x0000, 0x0254, 0x0188, 0x0000, 0x0256, 0x0257, 0x018C, 0x0000, 0x0000, 0x01DD, 0x0259,
	0x025B, 0x0192, 0x0000, 0x0260, 0x0263, 0x0000, 0x0269, 0x0268, 0x0199, 0x0000, 0x0000, 0x0000, 0x026F, 0x0272, 0x0000, 0x0275,
	0x01A1, 0x0000, 0x01A3, 0x0000, 0x01A5, 0x0000, 0x0280, 0x01A8, 0x0000, 0x0283, 0x0000, 0x0000, 0x01AD, 0x0000, 0x0288, 0x01B0,
	0x0000, 0x028A, 0x028B, 0x01B4, 0x0000, 0x01B6, 0x0000, 0x0292, 0x01B9, 0x0000, 0x0000, 0x0000, 0x01BD, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x01C6, 0x01C6, 0x0000, 0x01C9, 0x01C9, 0x0000, 0x01CC, 0x01CC, 0x0000, 0x01CE, 0x0000, 0x01D0,
	0x0000, 0x01D2, 0x0000, 0x01D4, 0x0000, 0x01D6, 0x0000, 0x01D8, 0x0000, 0x01DA, 0x0000, 0x01DC, 0x0000, 0x0000, 0x01DF, 0x0000,
	0x01E1, 0x0000, 0x01E3, 0x0000, 0x01E5, 0x0000, 0x01E7, 0x0000, 0x01E9, 0x0000, 0x01EB, 0x0000, 0x01ED, 0x0000, 0x01EF, 0x0000,
	0x0000, 0x01F3, 0x01F3, 0x0000, 0x01F5, 0x0000, 0x0195, 0x01BF, 0x01F9, 0x0000, 0x01FB, 0x0000, 0x01FD, 0x0000, 0x01FF, 0x0000
};


static kvi_wchar_t case_utl_02[256] =
{
	0x0201, 0x0000, 0x0203, 0x0000, 0x0205, 0x0000, 0x0207, 0x0000, 0x0209, 0x0000, 0x020B, 0x0000, 0x020D, 0x0000, 0x020F, 0x0000,
	0x0211, 0x0000, 0x0213, 0x0000, 0x0215, 0x0000, 0x0217, 0x0000, 0x0219, 0x0000, 0x021B, 0x0000, 0x021D, 0x0000, 0x021F, 0x0000,
	0x019E, 0x0000, 0x0223, 0x0000, 0x0225, 0x0000, 0x0227, 0x0000, 0x0229, 0x0000, 0x022B, 0x0000, 0x022D, 0x0000, 0x022F, 0x0000,
	0x0231, 0x0000, 0x0233, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};


static kvi_wchar_t case_utl_03[256] =
{
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x03B9, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x03AC, 0x0000, 0x03AD, 0x03AE, 0x03AF, 0x0000, 0x03CC, 0x0000, 0x03CD, 0x03CE,
	0x0000, 0x03B1, 0x03B2, 0x03B3, 0x03B4, 0x03B5, 0x03B6, 0x03B7, 0x03B8, 0x03B9, 0x03BA, 0x03BB, 0x03BC, 0x03BD, 0x03BE, 0x03BF,
	0x03C0, 0x03C1, 0x0000, 0x03C3, 0x03C4, 0x03C5, 0x03C6, 0x03C7, 0x03C8, 0x03C9, 0x03CA, 0x03CB, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x03C3, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x03B2, 0x03B8, 0x0000, 0x0000, 0x0000, 0x03C6, 0x03C0, 0x0000, 0x03D9, 0x0000, 0x03DB, 0x0000, 0x03DD, 0x0000, 0x03DF, 0x0000,
	0x03E1, 0x0000, 0x03E3, 0x0000, 0x03E5, 0x0000, 0x03E7, 0x0000, 0x03E9, 0x0000, 0x03EB, 0x0000, 0x03ED, 0x0000, 0x03EF, 0x0000,
	0x03BA, 0x03C1, 0x03C3, 0x0000, 0x03B8, 0x03B5, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};


static kvi_wchar_t case_utl_04[256] =
{
	0x0450, 0x0451, 0x0452, 0x0453, 0x0454, 0x0455, 0x0456, 0x0457, 0x0458, 0x0459, 0x045A, 0x045B, 0x045C, 0x045D, 0x045E, 0x045F,
	0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437, 0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
	0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447, 0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0461, 0x0000, 0x0463, 0x0000, 0x0465, 0x0000, 0x0467, 0x0000, 0x0469, 0x0000, 0x046B, 0x0000, 0x046D, 0x0000, 0x046F, 0x0000,
	0x0471, 0x0000, 0x0473, 0x0000, 0x0475, 0x0000, 0x0477, 0x0000, 0x0479, 0x0000, 0x047B, 0x0000, 0x047D, 0x0000, 0x047F, 0x0000,
	0x0481, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x048B, 0x0000, 0x048D, 0x0000, 0x048F, 0x0000,
	0x0491, 0x0000, 0x0493, 0x0000, 0x0495, 0x0000, 0x0497, 0x0000, 0x0499, 0x0000, 0x049B, 0x0000, 0x049D, 0x0000, 0x049F, 0x0000,
	0x04A1, 0x0000, 0x04A3, 0x0000, 0x04A5, 0x0000, 0x04A7, 0x0000, 0x04A9, 0x0000, 0x04AB, 0x0000, 0x04AD, 0x0000, 0x04AF, 0x0000,
	0x04B1, 0x0000, 0x04B3, 0x0000, 0x04B5, 0x0000, 0x04B7, 0x0000, 0x04B9, 0x0000, 0x04BB, 0x0000, 0x04BD, 0x0000, 0x04BF, 0x0000,
	0x0000, 0x04C2, 0x0000, 0x04C4, 0x0000, 0x04C6, 0x0000, 0x04C8, 0x0000, 0x04CA, 0x0000, 0x04CC, 0x0000, 0x04CE, 0x0000, 0x0000,
	0x04D1, 0x0000, 0x04D3, 0x0000, 0x04D5, 0x0000, 0x04D7, 0x0000, 0x04D9, 0x0000, 0x04DB, 0x0000, 0x04DD, 0x0000, 0x04DF, 0x0000,
	0x04E1, 0x0000, 0x04E3, 0x0000, 0x04E5, 0x0000, 0x04E7, 0x0000, 0x04E9, 0x0000, 0x04EB, 0x0000, 0x04ED, 0x0000, 0x04EF, 0x0000,
	0x04F1, 0x0000, 0x04F3, 0x0000, 0x04F5, 0x0000, 0x0000, 0x0000, 0x04F9, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};


static kvi_wchar_t case_utl_05[256] =
{
	0x0501, 0x0000, 0x0503, 0x0000, 0x0505, 0x0000, 0x0507, 0x0000, 0x0509, 0x0000, 0x050B, 0x0000, 0x050D, 0x0000, 0x050F, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0561, 0x0562, 0x0563, 0x0564, 0x0565, 0x0566, 0x0567, 0x0568, 0x0569, 0x056A, 0x056B, 0x056C, 0x056D, 0x056E, 0x056F,
	0x0570, 0x0571, 0x0572, 0x0573, 0x0574, 0x0575, 0x0576, 0x0577, 0x0578, 0x0579, 0x057A, 0x057B, 0x057C, 0x057D, 0x057E, 0x057F,
	0x0580, 0x0581, 0x0582, 0x0583, 0x0584, 0x0585, 0x0586, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};


static kvi_wchar_t case_utl_1E[256] =
{
	0x1E01, 0x0000, 0x1E03, 0x0000, 0x1E05, 0x0000, 0x1E07, 0x0000, 0x1E09, 0x0000, 0x1E0B, 0x0000, 0x1E0D, 0x0000, 0x1E0F, 0x0000,
	0x1E11, 0x0000, 0x1E13, 0x0000, 0x1E15, 0x0000, 0x1E17, 0x0000, 0x1E19, 0x0000, 0x1E1B, 0x0000, 0x1E1D, 0x0000, 0x1E1F, 0x0000,
	0x1E21, 0x0000, 0x1E23, 0x0000, 0x1E25, 0x0000, 0x1E27, 0x0000, 0x1E29, 0x0000, 0x1E2B, 0x0000, 0x1E2D, 0x0000, 0x1E2F, 0x0000,
	0x1E31, 0x0000, 0x1E33, 0x0000, 0x1E35, 0x0000, 0x1E37, 0x0000, 0x1E39, 0x0000, 0x1E3B, 0x0000, 0x1E3D, 0x0000, 0x1E3F, 0x0000,
	0x1E41, 0x0000, 0x1E43, 0x0000, 0x1E45, 0x0000, 0x1E47, 0x0000, 0x1E49, 0x0000, 0x1E4B, 0x0000, 0x1E4D, 0x0000, 0x1E4F, 0x0000,
	0x1E51, 0x0000, 0x1E53, 0x0000, 0x1E55, 0x0000, 0x1E57, 0x0000, 0x1E59, 0x0000, 0x1E5B, 0x0000, 0x1E5D, 0x0000, 0x1E5F, 0x0000,
	0x1E61, 0x0000, 0x1E63, 0x0000, 0x1E65, 0x0000, 0x1E67, 0x0000, 0x1E69, 0x0000, 0x1E6B, 0x0000, 0x1E6D, 0x0000, 0x1E6F, 0x0000,
	0x1E71, 0x0000, 0x1E73, 0x0000, 0x1E75, 0x0000, 0x1E77, 0x0000, 0x1E79, 0x0000, 0x1E7B, 0x0000, 0x1E7D, 0x0000, 0x1E7F, 0x0000,
	0x1E81, 0x0000, 0x1E83, 0x0000, 0x1E85, 0x0000, 0x1E87, 0x0000, 0x1E89, 0x0000, 0x1E8B, 0x0000, 0x1E8D, 0x0000, 0x1E8F, 0x0000,
	0x1E91, 0x0000, 0x1E93, 0x0000, 0x1E95, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1E61, 0x0000, 0x0000, 0x0000, 0x0000,
	0x1EA1, 0x0000, 0x1EA3, 0x0000, 0x1EA5, 0x0000, 0x1EA7, 0x0000, 0x1EA9, 0x0000, 0x1EAB, 0x0000, 0x1EAD, 0x0000, 0x1EAF, 0x0000,
	0x1EB1, 0x0000, 0x1EB3, 0x0000, 0x1EB5, 0x0000, 0x1EB7, 0x0000, 0x1EB9, 0x0000, 0x1EBB, 0x0000, 0x1EBD, 0x0000, 0x1EBF, 0x0000,
	0x1EC1, 0x0000, 0x1EC3, 0x0000, 0x1EC5, 0x0000, 0x1EC7, 0x0000, 0x1EC9, 0x0000, 0x1ECB, 0x0000, 0x1ECD, 0x0000, 0x1ECF, 0x0000,
	0x1ED1, 0x0000, 0x1ED3, 0x0000, 0x1ED5, 0x0000, 0x1ED7, 0x0000, 0x1ED9, 0x0000, 0x1EDB, 0x0000, 0x1EDD, 0x0000, 0x1EDF, 0x0000,
	0x1EE1, 0x0000, 0x1EE3, 0x0000, 0x1EE5, 0x0000, 0x1EE7, 0x0000, 0x1EE9, 0x0000, 0x1EEB, 0x0000, 0x1EED, 0x0000, 0x1EEF, 0x0000,
	0x1EF1, 0x0000, 0x1EF3, 0x0000, 0x1EF5, 0x0000, 0x1EF7, 0x0000, 0x1EF9, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};


static kvi_wchar_t case_utl_1F[256] =
{
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1F00, 0x1F01, 0x1F02, 0x1F03, 0x1F04, 0x1F05, 0x1F06, 0x1F07,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1F10, 0x1F11, 0x1F12, 0x1F13, 0x1F14, 0x1F15, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1F20, 0x1F21, 0x1F22, 0x1F23, 0x1F24, 0x1F25, 0x1F26, 0x1F27,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1F30, 0x1F31, 0x1F32, 0x1F33, 0x1F34, 0x1F35, 0x1F36, 0x1F37,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1F40, 0x1F41, 0x1F42, 0x1F43, 0x1F44, 0x1F45, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1F51, 0x0000, 0x1F53, 0x0000, 0x1F55, 0x0000, 0x1F57,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1F60, 0x1F61, 0x1F62, 0x1F63, 0x1F64, 0x1F65, 0x1F66, 0x1F67,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1F80, 0x1F81, 0x1F82, 0x1F83, 0x1F84, 0x1F85, 0x1F86, 0x1F87,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1F90, 0x1F91, 0x1F92, 0x1F93, 0x1F94, 0x1F95, 0x1F96, 0x1F97,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1FA0, 0x1FA1, 0x1FA2, 0x1FA3, 0x1FA4, 0x1FA5, 0x1FA6, 0x1FA7,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1FB0, 0x1FB1, 0x1F70, 0x1F71, 0x1FB3, 0x0000, 0x03B9, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1F72, 0x1F73, 0x1F74, 0x1F75, 0x1FC3, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1FD0, 0x1FD1, 0x1F76, 0x1F77, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1FE0, 0x1FE1, 0x1F7A, 0x1F7B, 0x1FE5, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1F78, 0x1F79, 0x1F7C, 0x1F7D, 0x1FF3, 0x0000, 0x0000, 0x0000
};


static kvi_wchar_t case_utl_21[256] =
{
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x03C9, 0x0000, 0x0000, 0x0000, 0x006B, 0x00E5, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x2170, 0x2171, 0x2172, 0x2173, 0x2174, 0x2175, 0x2176, 0x2177, 0x2178, 0x2179, 0x217A, 0x217B, 0x217C, 0x217D, 0x217E, 0x217F,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};


static kvi_wchar_t case_utl_24[256] =
{
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x24D0, 0x24D1, 0x24D2, 0x24D3, 0x24D4, 0x24D5, 0x24D6, 0x24D7, 0x24D8, 0x24D9,
	0x24DA, 0x24DB, 0x24DC, 0x24DD, 0x24DE, 0x24DF, 0x24E0, 0x24E1, 0x24E2, 0x24E3, 0x24E4, 0x24E5, 0x24E6, 0x24E7, 0x24E8, 0x24E9,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};


static kvi_wchar_t case_utl_FF[256] =
{
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0xFF41, 0xFF42, 0xFF43, 0xFF44, 0xFF45, 0xFF46, 0xFF47, 0xFF48, 0xFF49, 0xFF4A, 0xFF4B, 0xFF4C, 0xFF4D, 0xFF4E, 0xFF4F,
	0xFF50, 0xFF51, 0xFF52, 0xFF53, 0xFF54, 0xFF55, 0xFF56, 0xFF57, 0xFF58, 0xFF59, 0xFF5A, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};


kvi_wchar_t * kvirc_case_map_utl[256] =
{
	case_utl_00, case_utl_01, case_utl_02, case_utl_03, case_utl_04, case_utl_05, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_utl_1E, case_utl_1F,
	case_xtx_XX, case_utl_21, case_xtx_XX, case_xtx_XX, case_utl_24, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX,
	case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_xtx_XX, case_utl_FF
};


// Case mapping tables are 13824 bytes long



kvi_wchar_t kvi_wtolower(kvi_wchar_t c)
{
	kvi_wchar_t l = kvirc_case_map_utl[c >> 8][c & 0xff];
	return l ? l : c;
}

bool kvi_hstrEqualCIN(const kvi_wchar_t *str1,const char *str2,int len)
{
	while(len-- && *str1)if(kvi_wtolower(*str1++) != (kvi_wchar_t)tolower(*str2++))return false;
	return (len < 0);
}


const kvi_wchar_t * KviIrcView::getTextLine(int iMsgType,
					const kvi_wchar_t * data_ptr,
					KviIrcViewLine *line_ptr,
					bool bEnableTimeStamp)
{
	const kvi_wchar_t* pUnEscapeAt = 0;
	// Splits the text data in lines (separated by '\n')

	// NOTE: This function may be NOT reentrant
	//   ... no function in this file is supposed to be thread safe anyway

	int iTextIdx  = 0;       //we're at the beginning in the buffer
	int iCurChunk = 0;
	int blockLen;

	register const kvi_wchar_t *p= data_ptr;

	//Alloc the first attribute
	line_ptr->uChunkCount = 1;
	line_ptr->pChunks = (KviIrcViewLineChunk *)kvi_malloc(sizeof(KviIrcViewLineChunk));
	//And fill it up
	line_ptr->pChunks[0].type = KVI_TEXT_COLOR;
	line_ptr->pChunks[0].iTextStart = 0;
	line_ptr->pChunks[0].colors.back = KVI_OPTION_MSGTYPE(iMsgType).back();
	line_ptr->pChunks[0].colors.fore = KVI_OPTION_MSGTYPE(iMsgType).fore();
	line_ptr->pChunks[0].customFore=QColor();
	
	if(bEnableTimeStamp && KVI_OPTION_BOOL(KviOption_boolIrcViewTimestamp))
	{
		QString szTimestamp;
		szTimestamp=QDateTime::currentDateTime ( 
				KVI_OPTION_BOOL(KviOption_boolIrcViewTimestampUTC) ? Qt::UTC : Qt::LocalTime ).toString(
					KVI_OPTION_STRING(KviOption_stringIrcViewTimestampFormat) );
		szTimestamp.append(' ');
		int iTimeStampLength=szTimestamp.length();
		
		if(KVI_OPTION_BOOL(KviOption_boolUseSpecialColorForTimestamp))
		{
			// we need three chunks: the first one uses the default colors
			// for the message type, the second one the special colors
			// of the timestamp and the third one goes back to the defaults
			line_ptr->pChunks[0].iTextLen = 0;

			line_ptr->uChunkCount=3;
			line_ptr->pChunks=(KviIrcViewLineChunk *)kvi_realloc((void *)line_ptr->pChunks,3 * sizeof(KviIrcViewLineChunk));

			line_ptr->pChunks[1].type = KVI_TEXT_COLOR;
			line_ptr->pChunks[1].iTextStart = 0;
			line_ptr->pChunks[1].iTextLen = iTimeStampLength-1;
			line_ptr->pChunks[1].colors.back = KVI_OPTION_UINT(KviOption_uintTimeStampBackground);
			line_ptr->pChunks[1].colors.fore = KVI_OPTION_UINT(KviOption_uintTimeStampForeground);

			line_ptr->pChunks[2].type = KVI_TEXT_COLOR;
			line_ptr->pChunks[2].iTextStart = iTimeStampLength-1;
			line_ptr->pChunks[2].iTextLen = 1;
			line_ptr->pChunks[2].colors.back = KVI_OPTION_MSGTYPE(iMsgType).back();
			line_ptr->pChunks[2].colors.fore = KVI_OPTION_MSGTYPE(iMsgType).fore();
			line_ptr->pChunks[2].customFore=QColor();
			iCurChunk+=2;
		} else {
			// only one chunk
			line_ptr->pChunks[0].iTextLen = iTimeStampLength;
		}

		// We need the timestamp string to be added
		// alloc the necessary space
		line_ptr->szText.setLength(iTimeStampLength);

		iTextIdx = iTimeStampLength;                     // the rest of the string will begin 11 chars later

		// throw away const: we WANT to set the chars :D
		register QChar * data_ptr_aux = (QChar *)line_ptr->szText.unicode();
		register QChar * stamp_ptr_aux = (QChar *)szTimestamp.unicode();
		
		for(int i=0;i<iTimeStampLength;i++)
			*data_ptr_aux++  = *stamp_ptr_aux++;
	} else {
		// Timestamp not needed... but we don't want null strings floating around
		line_ptr->szText = "";
		line_ptr->pChunks[0].iTextLen = 0;
	}

	//
	// Ok... a couple of macros that occur really frequently
	// in the following code...
	// these could work well as functions too...but the macros are a lot faster :)
	//

#define APPEND_LAST_TEXT_BLOCK(__data_ptr,__data_len) \
	blockLen = (__data_len); \
	line_ptr->pChunks[iCurChunk].iTextLen += blockLen; \
	kvi_appendWCharToQStringWithLength(&(line_ptr->szText),__data_ptr,__data_len); \
	iTextIdx+=blockLen;

#define APPEND_LAST_TEXT_BLOCK_HIDDEN_FROM_NOW(__data_ptr,__data_len) \
	blockLen = (__data_len); \
	kvi_appendWCharToQStringWithLength(&(line_ptr->szText),__data_ptr,__data_len); \
	iTextIdx+=blockLen;


#define APPEND_ZERO_LENGTH_BLOCK(__data_ptr) /* does nothing */

#define NEW_LINE_CHUNK(_chunk_type) \
	line_ptr->uChunkCount++; \
	line_ptr->pChunks=(KviIrcViewLineChunk *)kvi_realloc((void *)line_ptr->pChunks, \
			line_ptr->uChunkCount * sizeof(KviIrcViewLineChunk)); \
	iCurChunk++; \
	line_ptr->pChunks[iCurChunk].type = _chunk_type; \
	line_ptr->pChunks[iCurChunk].iTextStart = iTextIdx; \
	line_ptr->pChunks[iCurChunk].iTextLen = 0; \
	line_ptr->pChunks[iCurChunk].customFore=iCurChunk ? line_ptr->pChunks[iCurChunk-1].customFore : QColor(); 

	// EOF Macros

	int partLen;

#ifdef COMPILE_USE_DYNAMIC_LABELS

	// Herezy :)

	// This is not only usage of the *Evil Goto(tm)*
	// This is also a *rather unclear* use of the *Really Evil Goto(tm)*
	// char_to_check_jump_table is a table of dynamic label addresses...
	// we use it to jump to the proper check
	// loop_begin is a dynamic label, and we use it to
	// return to the appropriate loop
	// This is again BAD PROGRAMMING(TM) :).... but it is faster than
	// the version with no dynamic gotos, and really faster
	// that any version without gotos that comed into my mind...
	// 
	// This code will prolly work only with GCC...(and even needs a "smart" one)

	// Again did two versions... the first was:
	//
	//  if(void * jmp_address = char_to_check_jump_table[*((unsigned char *)p)])goto *jmp_address;
	//    18a3:	8b 55 f0             	movl   0xfffffff0(%ebp),%edx
	//    18a6:	31 c0                	xorl   %eax,%eax
	//    18a8:	8a 02                	movb   (%edx),%al
	//    18aa:	8b 04 85 20 00 00 00 	movl   0x20(,%eax,4),%eax
	//    18b1:	85 c0                	testl  %eax,%eax
	//    18b3:	74 02                	je     18b7 <KviIrcView::getTextLine(int, char const *, KviIrcViewLine *)+0x1f3>
	//    18b5:	ff e0                	jmp    *%eax
	//
	// I even had a nicer version:
	//
	//  goto *(char_to_check_jump_table[*((unsigned char *)p)]);
	//    18a3:	8b 55 f0             	movl   0xfffffff0(%ebp),%edx
	//    18a6:	31 c0                	xorl   %eax,%eax
	//    18a8:	8a 02                	movb   (%edx),%al
	//    18aa:	ff 24 85 20 00 00 00 	jmp    *0x20(,%eax,4)
	//
	// but sth tells me that "jmp *0x20(,%eax,4)" takes a loooooot of clock ticks...
	// ...we have less instructions , but the code takes longer to execute (7-8% longer)
	// it might be also due to pipeline tricks, jump "next instruction precalculation" stuff...

	// So we end up using the fist version here

	void * loop_begin;

	static void * char_to_check_jump_table[256]=
	{
		&&found_end_of_buffer  ,0                      ,&&found_mirc_escape    ,&&found_color_escape   ,
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,&&found_end_of_line    ,0                      ,
		0                      ,&&found_command_escape ,0                      ,&&found_mirc_escape    ,
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,&&found_mirc_escape    ,0                      ,
		0                      ,0                      ,0                      ,0                      ,
		0                      ,&&found_icon_escape    ,0                      ,&&found_mirc_escape    , // 000-031
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                      , // 032-047
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,&&check_emoticon_char  ,&&check_emoticon_char  ,
		0                      ,&&check_emoticon_char  ,0                      ,0                      , // 048-063 // 61='=' , 59=';' , 58=':'
		0                      ,0                      ,0                      ,0                      ,
		0                      ,&&check_e2k_url        ,&&check_file_or_ftp_url,0                      ,
		&&check_http_url       ,&&check_irc_url        ,0                      ,0                      ,
		0                      ,&&check_mailto_url     ,0                      ,0                      , // 064-079  // 070==F 072==H 073==I  077==M
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,&&check_www_url        ,
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                      , // 080-095  // 087==W
		0                      ,0                      ,0                      ,0                      ,
		0                      ,&&check_e2k_url        ,&&check_file_or_ftp_url,0                      ,
		&&check_http_url       ,&&check_irc_url        ,0                      ,0                      ,
		0                      ,&&check_mailto_url     ,0                      ,0                      , // 096-111  // 101=e 102=f 104=h 105=i 109==m
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,&&check_www_url        ,
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                      , // 112-127  // 119==w
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                      , // 128-133
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                      , // 134-159
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                      , // 160-175
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                      , // 176-191
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                      , // 192-207
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                      , // 208-223
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                      , // 224-239
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                      ,
		0                      ,0                      ,0                      ,0                        // 240-255
	};

	if(KVI_OPTION_BOOL(KviOption_boolIrcViewUrlHighlighting))
	{
		loop_begin = &&highlighting_check_loop;               // get the address of the return label
		// forewer loop
highlighting_check_loop:
		// yet more optimized
		if(*((unsigned short *)p) < 0xff)
			if(void * jmp_address = char_to_check_jump_table[*((unsigned short *)p)])goto *jmp_address;
			// goto *(char_to_check_jump_table[*((unsigned char *)p)]); <--- replace 0 with &nothing_found
//nothing_found:
		p++;
		goto highlighting_check_loop;
		// newer here
	} else {
		loop_begin = &&escape_check_loop;                     // get the address of the return label
		// forever loop
escape_check_loop:
		while(*((unsigned short *)p) > 31)p++;
		goto check_escape_switch;                             // returns to escape_check_loop or returns from the function at all
		// newer here
	}
	// newer here


#else // !COMPILE_USE_DYNAMIC_LABELS

	// No way to have a jump table, nor a dynamic return jump
	// Anyway...we can have sth similar to a jump table...
	// Note: this could be substituted by a compiler generated jump table
	//       for a switch command... but this is STILL faster

	static char char_to_check_table[256]=
	{
		1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, // 000-015
		1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, // 016-031
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, // 032-047
		0,0,0,0,0,0,0,0, 0,0,7,7,0,7,0,0, // 048-063
		0,0,0,0,0,8,3,0, 2,5,0,0,0,6,0,0, // 064-079  // 070==F 072==H 073==I 077==M
		0,0,0,0,0,0,0,4, 0,0,0,0,0,0,0,0, // 080-095  // 087==W
		0,0,0,0,0,8,3,0, 2,5,0,0,0,6,0,0, // 096-111  // 102==f 104==h 105==i 109==m
		0,0,0,0,0,0,0,4, 0,0,0,0,0,0,0,0, // 112-127  // 119==w
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, // 128-133
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, // 134-159
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, // 160-175
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, // 176-191
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, // 192-207
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, // 208-223
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, // 224-239
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0  // 240-255
	};

check_char_loop:
	if(KVI_OPTION_BOOL(KviOption_boolIrcViewUrlHighlighting))
	{
		for(;;)
		{
			if(*((unsigned short *)p) < 0xff)
				if(unsigned int chk = char_to_check_table[*((unsigned short *)p)])
			{
				switch(chk)
				{
					case 1: goto check_escape_switch;      break; // returns to check_char_loop or returns from the function at all
					case 2: goto check_http_url;           break; // returns to check_char_loop
					case 3: goto check_file_or_ftp_url;    break; // returns to check_char_loop
					case 4: goto check_www_url;            break; // returns to check_char_loop
					case 5: goto check_irc_url;            break; // returns to check_char_loop
					case 6: goto check_mailto_url;         break; // returns to check_char_loop
					case 7: goto check_emoticon_char;      break; // returns to check_char_loop
					case 8: goto check_e2k_url;            break;
				}
			}
			p++;
		}
	} else {
		while(((unsigned short)*p) > 31)p++;
		goto check_escape_switch;                                 // returns to check_char_loop
	}

#endif // !COMPILE_USE_DYNAMIC_LABELS

check_escape_switch:
	switch(*p)
	{
		case '\0':
#ifdef COMPILE_USE_DYNAMIC_LABELS
found_end_of_buffer:
#endif //COMPILE_USE_DYNAMIC_LABELS
			APPEND_LAST_TEXT_BLOCK(data_ptr,p - data_ptr)
			return p;
			break;
		case '\n':
#ifdef COMPILE_USE_DYNAMIC_LABELS
found_end_of_line:
#endif //COMPILE_USE_DYNAMIC_LABELS
			// Found the end of a line
			APPEND_LAST_TEXT_BLOCK(data_ptr,p - data_ptr)
			// terminate the string
			// move the current pointer to the next character...
			// if it is '\0' we will simply stop
			p++;
			return p;
			break;
		case '\r':
#ifdef COMPILE_USE_DYNAMIC_LABELS
found_command_escape:
#endif //COMPILE_USE_DYNAMIC_LABELS
			if (p==pUnEscapeAt) {
				APPEND_LAST_TEXT_BLOCK(data_ptr,p - data_ptr);
				NEW_LINE_CHUNK(KVI_TEXT_UNESCAPE);
				pUnEscapeAt = 0;
				p++;
				data_ptr=p;
				break;
			}
			// Command escape sequence
			// \r!<escape_cmd>\r<visible parameters string>\r
			p++;
			if(*p == '!')
			{
				const kvi_wchar_t * next_cr = p;
				// lookup the next carriage return
				while(*next_cr && (*next_cr != '\r'))next_cr++;
				if(*next_cr)
				{
					const kvi_wchar_t * term_cr = next_cr;
					term_cr++;
					while(*term_cr && (*term_cr != '\r'))term_cr++;
					if(*term_cr)
					{
						// ok....the format is right:
						//  \r!    ... \r ...    \r
						//    ^p        ^next_cr  ^term_cr
						p--;
						//  \r!    ... \r ...    \r
						//   ^p         ^next_cr  ^term_cr
						APPEND_LAST_TEXT_BLOCK(data_ptr,p - data_ptr)
						NEW_LINE_CHUNK(KVI_TEXT_ESCAPE)

						p+=2; //point after \r!

						blockLen = (next_cr - p);
						line_ptr->pChunks[iCurChunk].szPayload = (kvi_wchar_t *)kvi_malloc(((next_cr - p) + 1) * sizeof(kvi_wchar_t));
						kvi_fastmoveodd((void *)(line_ptr->pChunks[iCurChunk].szPayload),p,blockLen * sizeof(kvi_wchar_t));
						
						line_ptr->pChunks[iCurChunk].szPayload[blockLen] = 0;

						++next_cr; //point after the middle \r
						
						pUnEscapeAt = term_cr;
						
						
						bool bColorSetted=false;
						if(line_ptr->pChunks[iCurChunk].szPayload[0]=='n' && KVI_OPTION_BOOL(KviOption_boolUseUserListColorsAsNickColors))
						{
							if(m_pKviWindow->type()==KVI_WINDOW_TYPE_CHANNEL && m_pKviWindow)
							{
								if(line_ptr->pChunks[iCurChunk].szPayload[1]=='c' && ((KviChannel*)m_pKviWindow)->userListView())
								{
									KviUserListEntry *e = ((KviChannel*)m_pKviWindow)->userListView()->findEntry(QString((QChar*)next_cr,term_cr-next_cr));
									if(e)
									{
										line_ptr->pChunks[iCurChunk].colors.fore = KVI_COLOR_CUSTOM; e->color(line_ptr->pChunks[iCurChunk].customFore);
										bColorSetted=true;
									}
								}
							}
							else if(m_pKviWindow->type()==KVI_WINDOW_TYPE_QUERY && m_pKviWindow && line_ptr->pChunks[iCurChunk].szPayload[1]=='c')
							{
								QString m_szNick = QString((QChar*)next_cr,term_cr-next_cr);
								if(m_szNick==m_pKviWindow->connection()->currentNickName()) {
									line_ptr->pChunks[iCurChunk].colors.fore = KVI_COLOR_OWN;
									bColorSetted=true;
								}
							}
						}
						if(!bColorSetted)
						{
							line_ptr->pChunks[iCurChunk].colors.fore=KVI_NOCHANGE;
						}
						
						/*APPEND_LAST_TEXT_BLOCK(next_cr,term_cr - next_cr)
						NEW_LINE_CHUNK(KVI_TEXT_UNESCAPE)*/

						p=next_cr;
						data_ptr=p;
					}
				}
			}
			break;
		case KVI_TEXT_COLOR:
#ifdef COMPILE_USE_DYNAMIC_LABELS
found_color_escape:
#endif //COMPILE_USE_DYNAMIC_LABELS
			//Color control code...need a new attribute struct
			APPEND_LAST_TEXT_BLOCK(data_ptr,p - data_ptr)
			NEW_LINE_CHUNK(*p)
			p++;
			p=getColorBytesW(p,&(line_ptr->pChunks[iCurChunk].colors.fore),
							&(line_ptr->pChunks[iCurChunk].colors.back));
			data_ptr=p;
			break;
		case KVI_TEXT_ICON:
#ifdef COMPILE_USE_DYNAMIC_LABELS
found_icon_escape:
#endif //COMPILE_USE_DYNAMIC_LABELS
			p++;
			if(*p > 32)
			{
				// Icon control word... need a new attribute struct
				const kvi_wchar_t * beginPtr = p - 1;
				const kvi_wchar_t * icon_name = p;
				while(*p > 32)p++;
				int datalen = p - icon_name;
				kvi_wchar_t save = *p;
				// throw away constness!
				*((kvi_wchar_t *)p) = 0;
				// FIXME: this has to be changed! : lookupTextIcon must use wide characters!
				QString tmpQ;
				tmpQ.setUnicodeCodes(icon_name,datalen);
				KviTextIcon * icon = g_pTextIconManager->lookupTextIcon(tmpQ);
				// throw away constness!
				*((kvi_wchar_t *)p) = save;
				//if(*p == KVI_TEXT_ICON)p++; // ending delimiter
				if(icon)
				{
					APPEND_LAST_TEXT_BLOCK(data_ptr,beginPtr - data_ptr)
					NEW_LINE_CHUNK(KVI_TEXT_ICON)
					line_ptr->pChunks[iCurChunk].szPayload = (kvi_wchar_t *)kvi_malloc((datalen + 1) * sizeof(kvi_wchar_t));
					kvi_fastmoveodd((void *)(line_ptr->pChunks[iCurChunk].szPayload),icon_name,datalen * sizeof(kvi_wchar_t));
					line_ptr->pChunks[iCurChunk].szPayload[datalen] = 0;
					line_ptr->pChunks[iCurChunk].szSmileId=line_ptr->pChunks[iCurChunk].szPayload;
					
					APPEND_LAST_TEXT_BLOCK_HIDDEN_FROM_NOW(icon_name,datalen)
					
					data_ptr = p;
					NEW_LINE_CHUNK(KVI_TEXT_UNICON)
				}
			}
			break;
		case KVI_TEXT_BOLD:
		case KVI_TEXT_UNDERLINE:
		case KVI_TEXT_REVERSE:
		case KVI_TEXT_RESET:
#ifdef COMPILE_USE_DYNAMIC_LABELS
found_mirc_escape:
#endif //COMPILE_USE_DYNAMIC_LABELS
			APPEND_LAST_TEXT_BLOCK(data_ptr,p - data_ptr)
			NEW_LINE_CHUNK(*p)
			data_ptr=++p;
			break;
		default:
			p++;
			break;
	}
#ifdef COMPILE_USE_DYNAMIC_LABELS
	goto *loop_begin;
#else // !COMPILE_USE_DYNAMIC_LABELS
	goto check_char_loop;
#endif // !COMPILE_USE_DYNAMIC_LABELS

check_http_url:
	p++;
	if((*p == 't') || (*p == 'T'))
	{
		p--;
		if(kvi_hstrEqualCIN(p,"http://",7))
		{
			partLen = 7;
			goto got_url;
		}
		if(kvi_hstrEqualCIN(p,"https://",8))
		{
			partLen = 8;
			goto got_url;
		}
		p++;
	}
#ifdef COMPILE_USE_DYNAMIC_LABELS
	goto *loop_begin;
#else // !COMPILE_USE_DYNAMIC_LABELS
	goto check_char_loop;
#endif // !COMPILE_USE_DYNAMIC_LABELS


check_file_or_ftp_url:
	p++;
	if((*p == 'i') || (*p == 'I'))
	{
		p--;
		if(kvi_hstrEqualCIN(p,"file://",7))
		{
			partLen = 7;
			goto got_url;
		}
		p++;
	} else if((*p == 't') || (*p == 'T'))
	{
		p--;
		if(kvi_hstrEqualCIN(p,"ftp://",6))
		{
			partLen = 6;
			goto got_url;
		}
		if(kvi_hstrEqualCIN(p,"ftp.",4))
		{
			partLen = 4;
			goto got_url;
		}
		p++;
	}

#ifdef COMPILE_USE_DYNAMIC_LABELS
	goto *loop_begin;
#else // !COMPILE_USE_DYNAMIC_LABELS
	goto check_char_loop;
#endif // !COMPILE_USE_DYNAMIC_LABELS

check_e2k_url:
	p++;
	if((*p == 'd') || (*p == 'D'))
	{
		p--;
		if(kvi_hstrEqualCIN(p,"ed2k://",7))
		{
			partLen = 7;
			goto got_url;
		}
		p++;
	}

#ifdef COMPILE_USE_DYNAMIC_LABELS
	goto *loop_begin;
#else // !COMPILE_USE_DYNAMIC_LABELS
	goto check_char_loop;
#endif // !COMPILE_USE_DYNAMIC_LABELS

check_www_url:
	p++;
	if((*p == 'w') || (*p == 'W'))
	{
		p--;
		if(kvi_hstrEqualCIN(p,"www.",4))
		{
			partLen = 4;
			goto got_url;
		}
		p++;
	}

#ifdef COMPILE_USE_DYNAMIC_LABELS
	goto *loop_begin;
#else // !COMPILE_USE_DYNAMIC_LABELS
	goto check_char_loop;
#endif // !COMPILE_USE_DYNAMIC_LABELS

check_irc_url:
	p++;
	if((*p == 'r') || (*p == 'R'))
	{
		p--;
		if(kvi_hstrEqualCIN(p,"irc://",6))
		{
			partLen = 6;
			goto got_url;
		}
		if(kvi_hstrEqualCIN(p,"irc6://",7))
		{
			partLen = 7;
			goto got_url;
		}
		if(kvi_hstrEqualCIN(p,"ircs://",7))
		{
			partLen = 7;
			goto got_url;
		}
		if(kvi_hstrEqualCIN(p,"ircs6://",8))
		{
			partLen = 8;
			goto got_url;
		}
		p++;
	}

#ifdef COMPILE_USE_DYNAMIC_LABELS
	goto *loop_begin;
#else // !COMPILE_USE_DYNAMIC_LABELS
	goto check_char_loop;
#endif // !COMPILE_USE_DYNAMIC_LABELS

check_mailto_url:
	p++;
	if((*p == 'a') || (*p == 'A'))
	{
		p--;
		if(kvi_hstrEqualCIN(p,"mailto:",7))
		{
			partLen = 7;
			goto got_url;
		}
		p++;
	}
#ifdef COMPILE_USE_DYNAMIC_LABELS
	goto *loop_begin;
#else // !COMPILE_USE_DYNAMIC_LABELS
	goto check_char_loop;
#endif // !COMPILE_USE_DYNAMIC_LABELS



got_url:
	//Url highlighting block
	if(*(p + partLen) < 47)
	{
		p+=partLen;
		APPEND_LAST_TEXT_BLOCK(data_ptr,p - data_ptr)
	} else {
	APPEND_LAST_TEXT_BLOCK(data_ptr,p - data_ptr)
	NEW_LINE_CHUNK(KVI_TEXT_ESCAPE)
// FIXME: #warning "Option for the URL escape...double click and right button!!!"
//	int urlLen = KVI_OPTION_STRING(KviOption_stringUrlLinkCommand).len() + 1;
	line_ptr->pChunks[iCurChunk].szPayload = (kvi_wchar_t *)kvi_malloc(2 * sizeof(kvi_wchar_t));
	line_ptr->pChunks[iCurChunk].szPayload[0] = 'u';
	line_ptr->pChunks[iCurChunk].szPayload[1] = 0x0;
	line_ptr->pChunks[iCurChunk].colors.fore = KVI_OPTION_MSGTYPE(KVI_OUT_URL).fore();
	//and run until the presumed end of the url
	data_ptr=p;
	p+=partLen;
	// Question : What characters are NOT allowed in an URL ?
	// I assume [] () {} 'and chars below 33 (space too , and negative chars too! (for signed char systems))
	// [] and () are used in ed2k links often

	// These characters are "{", "}", "|", "\", "^", "~", "[", "]", and "`". (RFC1738)
	while((*p > 32) && (*p != '[') && (*p != '|') && (*p != '{') && (*p != '\'') && (*p != '>') &&
			(*p != ']')  && (*p != '}') && (*p != '<') && (*p != '"'))p++;

	if(m_pKviWindow)
	{
		QString tmp;
		tmp.setUnicodeCodes(data_ptr,p-data_ptr);
		KVS_TRIGGER_EVENT_1(KviEvent_OnUrl,m_pKviWindow,tmp);
	}

	APPEND_LAST_TEXT_BLOCK(data_ptr,p - data_ptr)
	NEW_LINE_CHUNK(KVI_TEXT_UNESCAPE)

	}
	data_ptr=p;

#ifdef COMPILE_USE_DYNAMIC_LABELS
	goto *loop_begin;
#else // !COMPILE_USE_DYNAMIC_LABELS
	goto check_char_loop;
#endif // !COMPILE_USE_DYNAMIC_LABELS

	//FIXME #warning: Add more emoticons, and more intelligent code to detect when they're not really emoticons

check_emoticon_char:
	// What about this ?

	const kvi_wchar_t * begin = p;
	p++;
	if(KVI_OPTION_BOOL(KviOption_boolDrawEmoticons))
		switch(iMsgType)
	{
		case KVI_OUT_CHANPRIVMSG:
		case KVI_OUT_ACTION:
		case KVI_OUT_OWNPRIVMSG:
		case KVI_OUT_QUERYPRIVMSG:
		case KVI_OUT_QUERYPRIVMSGCRYPTED:
		case KVI_OUT_QUERYNOTICE:
		case KVI_OUT_QUERYNOTICECRYPTED:
		case KVI_OUT_CHANPRIVMSGCRYPTED:
		case KVI_OUT_CHANNELNOTICE:
		case KVI_OUT_CHANNELNOTICECRYPTED:
		case KVI_OUT_OWNPRIVMSGCRYPTED:
		case KVI_OUT_HIGHLIGHT:
	{
		// Pragma: 31.05.2002 : I had to kill the 8 prefix
		// It happens really too often to have an 8 followed by a parenthesis
		// that is not an emoticon

		// *begin can be one of ':' , ';' , '='
		if(*p == '-')p++; // FIXME: we could handle also 'o' as a nose ??? (extreme: also '+' ?)
		// FIXME: use a "jump-like-check-table" here ? .... it would be surely faster
		// FIXME: handle also '[',']','\\','p','@','#','<','>','|' ???
		switch(*p)
		{
			case ')':
			case '(':
			case '/':
			case 'D':
			case 'P':
			case 'S':
			case 'O':
			case '*':
			case '|':
			case 176: // 'Â°' -> alt 176 : teardrop
			{
				const kvi_wchar_t * item = p;
				const kvi_wchar_t * item2 = 0;
				p++;
				while(*p == *item)p++;
				int count = (p - item) - 1;
				if(*item == 176)
				{
					if(*p == ')')
					{
						item2 = p;
						p++;
					}
				}
				if(!*p || (*p == ' '))
				{
					// ok! this is an emoticon (sequence) !
					// We lookup simplified versions of the emoticons...
	
					// FIXME: this sould become UNICODE!!!
					QString lookupstring;
					kvi_wchar_t ng[3];
					ng[0] = *begin;
					ng[1] = *item;
					if(item2)ng[2] = *item2;
					lookupstring.setUnicodeCodes(ng,item2 ? 3 : 2);

					KviTextIcon * icon  = g_pTextIconManager->lookupTextIcon(lookupstring);
					// do we have that emoticon-icon association ?
					if(icon)
					{
						// we got an icon for this emoticon
						// the tooltip will carry the original emoticon source text
						APPEND_LAST_TEXT_BLOCK(data_ptr,begin - data_ptr)
						NEW_LINE_CHUNK(KVI_TEXT_ICON)
						
						int emolen = p - begin;
						int reallen=item2 ? 3 : 2;
						
						line_ptr->pChunks[iCurChunk].szPayload = (kvi_wchar_t *)kvi_malloc((emolen + 1) * sizeof(kvi_wchar_t));
						kvi_fastmoveodd(line_ptr->pChunks[iCurChunk].szPayload,begin,emolen * sizeof(kvi_wchar_t));
						line_ptr->pChunks[iCurChunk].szPayload[emolen] = 0;
						
						line_ptr->pChunks[iCurChunk].szSmileId = (kvi_wchar_t *)kvi_malloc((reallen + 1) * sizeof(kvi_wchar_t));
						kvi_fastmoveodd(line_ptr->pChunks[iCurChunk].szSmileId,ng,reallen * sizeof(kvi_wchar_t));
						line_ptr->pChunks[iCurChunk].szSmileId[reallen] = 0;
						
						APPEND_LAST_TEXT_BLOCK_HIDDEN_FROM_NOW(begin,emolen)
						data_ptr = p;
						// let's also handle thingies like :DDDD
						item++;
						while(count > 0)
						{
							NEW_LINE_CHUNK(KVI_TEXT_ICON)
							line_ptr->pChunks[iCurChunk].szPayload = (kvi_wchar_t *)kvi_malloc((emolen + 1) * sizeof(kvi_wchar_t));
							kvi_fastmoveodd(line_ptr->pChunks[iCurChunk].szPayload,begin,emolen * sizeof(kvi_wchar_t));
							line_ptr->pChunks[iCurChunk].szPayload[emolen] = 0;
							
							line_ptr->pChunks[iCurChunk].szSmileId = (kvi_wchar_t *)kvi_malloc((reallen + 1) * sizeof(kvi_wchar_t));
							kvi_fastmoveodd(line_ptr->pChunks[iCurChunk].szSmileId,ng,reallen * sizeof(kvi_wchar_t));
							line_ptr->pChunks[iCurChunk].szSmileId[reallen] = 0;
							
							APPEND_ZERO_LENGTH_BLOCK(data_ptr)
							count--;
						}
						NEW_LINE_CHUNK(KVI_TEXT_UNICON)
					} // we don't even need to skip back... the text eventually parsed is ok to be in a single block for sure
				} // we don't even need to skip back... the text eventually parsed is ok to be in a single block for sure
			} // we don't even need to skip back... the text eventually parsed is ok to be in a single block for sure
			break;
		} // switch(*p)
	} break;

	}


#ifdef COMPILE_USE_DYNAMIC_LABELS
	goto *loop_begin;
#else // !COMPILE_USE_DYNAMIC_LABELS
	goto check_char_loop;
#endif // !COMPILE_USE_DYNAMIC_LABELS

	// never here
	return p;

}

void KviIrcView::fastScroll(int lines)
{
	m_iUnprocessedPaintEventRequests = 0;

	if(!isVisible())return;

	if(!m_pFm)
	{
		// We must get the metrics from a real paint event :/
		// must do a full repaint to get them...
		repaint();
		return;
	}

	// Ok...the current line is the last one here
	// It is the only one that needs to be repainted
	int widgetWidth  = width()-m_pScrollBar->width();
	if(widgetWidth < KVI_IRCVIEW_PIXMAP_SEPARATOR_AND_DOUBLEBORDER_WIDTH+10)return; //can't show stuff here
	int widgetHeight = height();
	int maxLineWidth = widgetWidth;
	int defLeftCoord=KVI_IRCVIEW_HORIZONTAL_BORDER;
	if(KVI_OPTION_BOOL(KviOption_boolIrcViewShowImages))
	{
		maxLineWidth -= KVI_IRCVIEW_PIXMAP_SEPARATOR_AND_DOUBLEBORDER_WIDTH;
		defLeftCoord+=KVI_IRCVIEW_PIXMAP_AND_SEPARATOR;
	}


	int heightToPaint = 1;
	KviIrcViewLine * l = m_pCurLine;
	while(lines > 0)
	{
		if(l)
		{
			if(maxLineWidth != l->iMaxLineWidth)calculateLineWraps(l,maxLineWidth);
			heightToPaint += l->uLineWraps * m_iFontLineSpacing;
			heightToPaint += (m_iFontLineSpacing + m_iFontDescent);
			lines--;
			l = l->pPrev;
		} else lines = 0;
	}

#ifdef COMPILE_USE_QT4
	scroll(0,-(heightToPaint-1),QRect(1,1,widgetWidth-2,widgetHeight-2));
#else
	bitBlt(this,1,1,this,1,heightToPaint,widgetWidth -2,widgetHeight - (heightToPaint + KVI_IRCVIEW_VERTICAL_BORDER));

	QRect r(0,widgetHeight - (heightToPaint + KVI_IRCVIEW_VERTICAL_BORDER),
			widgetWidth,heightToPaint + KVI_IRCVIEW_VERTICAL_BORDER);

	QPaintEvent * e = new QPaintEvent(r);
	paintEvent(e);
	delete e;
#endif

	if(m_iLastLinkRectHeight > -1)
	{
		// need to kill the last highlighted link
		m_iLastLinkRectTop -= heightToPaint;
		if(m_iLastLinkRectTop < 0)
		{
			m_iLastLinkRectHeight += m_iLastLinkRectTop;
			m_iLastLinkRectTop = 0;
		}
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// The IrcView : THE paint event
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void KviIrcView::paintEvent(QPaintEvent *p)
{
	//
	// THIS FUNCTION IS A MONSTER
	//

	int scrollbarWidth = m_pScrollBar->width();
	int widgetWidth  = width() - scrollbarWidth;
	if(!isVisible() || (widgetWidth < 20))
	{
		m_iUnprocessedPaintEventRequests = 0; // assume a full repaint when this widget is shown...
		return; //can't show stuff here
	}

	// if the mdiManager is in SDI mode
	// and this window is attacched but is not the toplevel one
	// then it is hidden completely behind the other windows
	// and we can avoid to paint it :)
	if(g_pFrame->mdiManager()->isInSDIMode() &&
		(m_pKviWindow->mdiParent() != g_pFrame->mdiManager()->topChild()) &&
		(m_pKviWindow->mdiParent()))
	{
		m_iUnprocessedPaintEventRequests = 0; // assume a full repaint when this widget is shown...
		return; // totally hidden behind other windows
	}

	int widgetHeight = height();

	static QRect r; // static: avoid calling constructor and destructor every time...

	if(p)
	{
		r=p->rect(); // app triggered , or self triggered from fastScroll (in that case m_iUnprocessedPaintEventRequests is set to 0 there)
		if(r == rect())
			m_iUnprocessedPaintEventRequests = 0; // only full repaints reset
	} else {
		// A self triggered event
		m_iUnprocessedPaintEventRequests = 0; // only full repaints reset
		r = rect();
	}

	int rectLeft   = r.x();
	int rectTop    = r.y();
	int rectHeight = r.height();
	int rectBottom = rectTop + rectHeight;
	int rectWidth  = r.width();
	if(rectWidth > widgetWidth)rectWidth = widgetWidth;

#ifdef COMPILE_USE_QT4
	QPainter pa(this); // we use qt4 double buffering
#else
	KviDoubleBuffer doublebuffer(width(),height());
	QPixmap * pDoubleBufferPixmap = doublebuffer.pixmap();
	
	QPainter pa(pDoubleBufferPixmap);
#endif
	SET_ANTI_ALIASING(pa);

	pa.setFont(font());
	if(!m_pFm)
	{
		// note that QFontMetrics(pa.font()) may be not the same as QFontMetrics(font())
		// because the painter might effectively use an approximation of the QFont specified
		// by font().
		recalcFontVariables(QFontMetrics(pa.font()),pa.fontInfo());
	}

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt = mapToGlobal(QPoint(rectLeft,rectTop));
		pa.drawTiledPixmap(rectLeft,rectTop,rectWidth,rectHeight,*g_pShadedChildGlobalDesktopBackground,pnt.x(),pnt.y());
	} else {
#endif
		QPixmap * pix = m_pPrivateBackgroundPixmap;

		if(!pix)
			pix = KVI_OPTION_PIXMAP(KviOption_pixmapIrcViewBackground).pixmap();
	
		pa.fillRect(rectLeft,rectTop,rectWidth,rectHeight,KVI_OPTION_COLOR(KviOption_colorIrcViewBackground));
		if(pix)
			KviPixmapUtils::drawPixmapWithPainter(&pa,pix,KVI_OPTION_UINT(KviOption_uintIrcViewPixmapAlign),r,widgetWidth,widgetHeight);
#ifdef COMPILE_PSEUDO_TRANSPARENCY	
	}
#endif
	
	//Have lines visible
	int curBottomCoord = widgetHeight - KVI_IRCVIEW_VERTICAL_BORDER;
	int maxLineWidth   = widgetWidth;
	int defLeftCoord   = KVI_IRCVIEW_HORIZONTAL_BORDER;
	int lineWrapsHeight;

	if(KVI_OPTION_BOOL(KviOption_boolIrcViewShowImages))
	{
		maxLineWidth -= KVI_IRCVIEW_PIXMAP_SEPARATOR_AND_DOUBLEBORDER_WIDTH;
		defLeftCoord += KVI_IRCVIEW_PIXMAP_AND_SEPARATOR;
	}

	KviIrcViewLine *pCurTextLine = m_pCurLine;

	if(m_bMouseIsDown)
	{
		m_szLastSelectionLine = "";
		m_szLastSelection = "";
	}

	//Make sure that we have enough space to paint something...
	if(maxLineWidth < m_iMinimumPaintWidth)pCurTextLine=0;

	bool bLineMarkPainted = !KVI_OPTION_BOOL(KviOption_boolTrackLastReadTextViewLine);


	//And loop thru lines until we not run over the upper bound of the view
	while((curBottomCoord >= KVI_IRCVIEW_VERTICAL_BORDER) && pCurTextLine)
	{
		//Paint pCurTextLine
		if(maxLineWidth != pCurTextLine->iMaxLineWidth)
		{
			// Width of the widget or the font has been changed
			// from the last time that this line was painted
			calculateLineWraps(pCurTextLine,maxLineWidth);
		}

		// the evil multiplication
		// in an i486 it can get up to 42 clock cycles
		lineWrapsHeight  = (pCurTextLine->uLineWraps) * m_iFontLineSpacing;
		curBottomCoord  -= lineWrapsHeight;

		if((curBottomCoord - m_iFontLineSpacing) > rectBottom)
		{
			// not in update rect... skip
			curBottomCoord -= (m_iFontLineSpacing + m_iFontDescent);
			pCurTextLine = pCurTextLine->pPrev;
			continue;
		}

		if(KVI_OPTION_BOOL(KviOption_boolIrcViewShowImages))
		{
			//Paint the pixmap first
			//Calculate the position of the image
			//imageYPos = curBottomCoord - (pixmapHeight(16) + ((m_iFontLineSpacing - 16)/2) );
			int imageYPos = curBottomCoord - m_iRelativePixmapY;
			//Set the mask if needed
			int iPixId = KVI_OPTION_MSGTYPE(pCurTextLine->iMsgType).pixId();
			if(iPixId > 0)
				pa.drawPixmap(KVI_IRCVIEW_HORIZONTAL_BORDER,imageYPos,*(g_pIconManager->getSmallIcon(iPixId)));
		}

		if(m_pToolWidget)
		{
			if(!m_pToolWidget->messageEnabled(pCurTextLine->iMsgType))
			{
				// not in update rect... skip
				curBottomCoord -= (m_iFontLineSpacing + m_iFontDescent);
				pCurTextLine = pCurTextLine->pPrev;
				continue;
			}
		}

		// Initialize for drawing this line of text
		// The first block is always an attribute block
		char defaultBack  = pCurTextLine->pBlocks->pChunk->colors.back;
		char defaultFore  = pCurTextLine->pBlocks->pChunk->colors.fore;
		bool curBold      = false;
		bool curUnderline = false;
		char foreBeforeEscape= KVI_BLACK;
		bool curLink      = false;
		bool bacWasTransp = false;
		char curFore      = defaultFore;
		char curBack      = defaultBack;
		int  curLeftCoord = defLeftCoord;
		curBottomCoord   -= m_iFontDescent; //rise up the text...

		//
		// Single text line loop (paint all text blocks)
		// (May correspond to more physical lines on the display if the text is wrapped)
		//

		for(int i=0;i < pCurTextLine->iBlockCount;i++)
		{
			register KviIrcViewWrappedBlock * block = &(pCurTextLine->pBlocks[i]);

			// Play with the attributes
			if(block->pChunk)
			{
				//normal block
				switch(block->pChunk->type)
				{
					case KVI_TEXT_COLOR:
						if(block->pChunk->colors.fore != KVI_NOCHANGE)
						{
							curFore = block->pChunk->colors.fore;
							if(block->pChunk->colors.back != KVI_NOCHANGE)
								curBack = block->pChunk->colors.back;
						} else {
							// only a CTRL+K... reset
							curFore = defaultFore;
							curBack = defaultBack;
						}
						// Begin Edit by GMC-jimmy: Added
						// When KVIrc encounters a CTRL+K code without any trailing numbers, we then use KVIrc's default color value
						// defined by the user in the Options dialog.
						// This is to allow KVIrc to handle mIRC color codes in a similar fashion to most other modern irc clients.
						// See also kvi_input.cpp

						// Pragma: optimized: moved the code above (avoided duplicate if())
						// Pragma(05.03.2003): fixed again: reset ONLY if CTRL+K without numbers
						// otherwise leave the background unchanged 

						//if(block->pChunk->colors.fore == KVI_NOCHANGE)
						//	curFore = defaultFore;
						//if(block->pChunk->colors.back == KVI_NOCHANGE)
						//	curBack = defaultBack;
						// End Edit
						break;
					case KVI_TEXT_ESCAPE:
						foreBeforeEscape      = curFore;
						if(block->pChunk->colors.fore != KVI_NOCHANGE)
							curFore = block->pChunk->colors.fore;
						if(m_pLastLinkUnderMouse == block)curLink = true;
						break;
					case KVI_TEXT_UNESCAPE:
						curLink            = false;
						curFore            = foreBeforeEscape;
						break;
					case KVI_TEXT_BOLD:
						curBold            = !curBold;
						break;
					case KVI_TEXT_UNDERLINE:
						curUnderline       = !curUnderline;
						break;
					case KVI_TEXT_RESET:
						curBold            = false;
						curUnderline       = false;
						curFore            = defaultFore;
						curBack            = defaultBack;
						break;
					case KVI_TEXT_REVERSE: //Huh ?
						char aux       = curBack;
						if(bacWasTransp == true)
						{
							curBack = KVI_TRANSPARENT;
						} else {
							curBack = curFore;
						}
						if(aux == KVI_TRANSPARENT)
						{
							curFore = (char)KVI_DEF_BACK;
						} else {
							curFore = aux;
						}
						bacWasTransp = (aux == KVI_TRANSPARENT);
/*						if(curBack != KVI_TRANSPARENT)
						{
							char aux       =curFore;
							curFore        = curBack;
							curBack        = aux;
						} else {
							curBack = curFore;
							switch(curBack)
							{
								case KVI_WHITE:
								case KVI_ORANGE:
								case KVI_YELLOW:
								case KVI_LIGHTGREEN:
								case KVI_BLUEMARINE:
								case KVI_LIGHTBLUE:
								case KVI_LIGHTVIOLET:
								case KVI_LIGHTGRAY:
									curFore=KVI_BLACK;
									break;
								default: //transparent too
									curFore=KVI_LIGHTGREEN;
									break;
							}
						}
*/
						break;
					//case KVI_TEXT_ICON:
					//case KVI_TEXT_UNICON:
						// does nothing
						//debug("Have a block with ICON/UNICON attr");
						//break;
				}

			} else {
				// no attributes , it is a line wrap
				curLeftCoord = defLeftCoord;
				if(KVI_OPTION_BOOL(KviOption_boolIrcViewWrapMargin))curLeftCoord+=m_iWrapMargin;
				curBottomCoord += m_iFontLineSpacing;
			}

//
// Here we run really out of bounds :)))))
// A couple of macros that could work well as functions...
// but since there are really many params to be passed
// and push & pop calls take clock cycles
// my paranoic mind decided to go for the macro way.
// This is NOT good programming
//

#define SET_PEN(_color,_custom)\
	if( ((unsigned char)_color) < 16 )\
	{\
		pa.setPen(KVI_OPTION_MIRCCOLOR((unsigned char)_color));\
	} else {\
		switch((unsigned char)_color)\
		{\
			case KVI_COLOR_EXT_USER_OP:\
				pa.setPen(KVI_OPTION_COLOR(KviOption_colorUserListViewOpForeground));\
				break;\
			case KVI_COLOR_EXT_USER_HALFOP:\
				pa.setPen(KVI_OPTION_COLOR(KviOption_colorUserListViewHalfOpForeground));\
				break;\
			case KVI_COLOR_EXT_USER_ADMIN:\
				pa.setPen(KVI_OPTION_COLOR(KviOption_colorUserListViewChanAdminForeground));\
				break;\
			case KVI_COLOR_EXT_USER_OWNER:\
				pa.setPen(KVI_OPTION_COLOR(KviOption_colorUserListViewChanOwnerForeground));\
				break;\
			case KVI_COLOR_EXT_USER_VOICE:\
				pa.setPen(KVI_OPTION_COLOR(KviOption_colorUserListViewVoiceForeground));\
				break;\
			case KVI_COLOR_EXT_USER_USEROP:\
				pa.setPen(KVI_OPTION_COLOR(KviOption_colorUserListViewUserOpForeground));\
				break;\
			case KVI_COLOR_EXT_USER_NORMAL:\
				pa.setPen(KVI_OPTION_COLOR(KviOption_colorUserListViewNormalForeground));\
				break;\
			case KVI_DEF_BACK :\
				pa.setPen(KVI_OPTION_COLOR(KviOption_colorIrcViewBackground));\
				break;\
			case KVI_COLOR_CUSTOM :\
				pa.setPen(_custom);\
				break;\
			case KVI_COLOR_OWN :\
				pa.setPen(KVI_OPTION_COLOR(KviOption_colorUserListViewOwnForeground));\
				break;\
		}\
	}

#define DRAW_SELECTED_TEXT(_text_str,_text_idx,_text_len,_text_width) \
	SET_PEN(KVI_OPTION_MSGTYPE(KVI_OUT_SELECT).fore(),block->pChunk ? block->pChunk->customFore : QColor()); \
	{ \
		int theWdth = _text_width; \
		if(theWdth < 0) \
			theWdth=width()-(curLeftCoord+KVI_IRCVIEW_HORIZONTAL_BORDER+scrollbarWidth); \
		pa.fillRect(curLeftCoord,curBottomCoord - m_iFontLineSpacing + m_iFontDescent,theWdth,m_iFontLineSpacing,KVI_OPTION_MIRCCOLOR(KVI_OPTION_MSGTYPE(KVI_OUT_SELECT).back())); \
	} \
	pa.drawText(curLeftCoord,curBottomCoord,_text_str,_text_idx,_text_len); \
	m_szLastSelectionLine.append(_text_str.mid(_text_idx,_text_len)); \
	curLeftCoord += _text_width;

#define DRAW_NORMAL_TEXT(_text_str,_text_idx,_text_len,_text_width) \
	SET_PEN(curFore,block->pChunk ? block->pChunk->customFore : QColor()); \
	if(curBack != KVI_TRANSPARENT){ \
		int theWdth = _text_width; \
		if(theWdth < 0) \
			theWdth=width()-(curLeftCoord+KVI_IRCVIEW_HORIZONTAL_BORDER+scrollbarWidth); \
		pa.fillRect(curLeftCoord,curBottomCoord - m_iFontLineSpacing + m_iFontDescent,theWdth,m_iFontLineSpacing,KVI_OPTION_MIRCCOLOR((unsigned char)curBack)); \
	} \
	pa.drawText(curLeftCoord,curBottomCoord,_text_str,_text_idx,_text_len); \
	if(curBold)pa.drawText(curLeftCoord+1,curBottomCoord,_text_str,_text_idx,_text_len); \
	if(curUnderline){ \
		int theWdth = _text_width; \
		if(theWdth < 0) \
			theWdth=width()-(curLeftCoord+KVI_IRCVIEW_HORIZONTAL_BORDER+scrollbarWidth); \
		pa.drawLine(curLeftCoord,curBottomCoord+2,curLeftCoord+theWdth,curBottomCoord+2); \
	} \
	curLeftCoord += _text_width;


// EOF macro declarations

			if(m_bMouseIsDown)
			{
				//Check if the block or a part of it is selected
				if(checkSelectionBlock(pCurTextLine,curLeftCoord,curBottomCoord,i))
				{
					//Selected in some way
					//__range_valid(g_pOptions->m_cViewOutSeleFore != KVI_TRANSPARENT);
					//__range_valid(g_pOptions->m_cViewOutSeleBack != KVI_TRANSPARENT);
					
					if(m_bShiftPressed && i && block->pChunk &&
						((m_pWrappedBlockSelectionInfo->selection_type == KVI_IRCVIEW_BLOCK_SELECTION_TOTAL) ||
						(m_pWrappedBlockSelectionInfo->selection_type == KVI_IRCVIEW_BLOCK_SELECTION_LEFT))
					)
					{
						switch(block->pChunk->type)
						{
							case KVI_TEXT_BOLD:
							case KVI_TEXT_UNDERLINE:
							case KVI_TEXT_REVERSE:
							case KVI_TEXT_RESET:
								m_szLastSelectionLine.append(QChar(block->pChunk->type));
							break;
							case KVI_TEXT_COLOR:
								m_szLastSelectionLine.append(QChar(block->pChunk->type));
								if((block->pChunk->colors.fore != KVI_NOCHANGE) && (block->pChunk->colors.fore != KVI_TRANSPARENT))
								{
									if(curFore > 9)m_szLastSelectionLine.append(QChar('1'));
									m_szLastSelectionLine.append(QChar((curFore%10)+'0'));
								}
								if((block->pChunk->colors.back != KVI_NOCHANGE) && (block->pChunk->colors.back != KVI_TRANSPARENT) )
								{
									m_szLastSelectionLine.append(QChar(','));
									if(curBack > 9)m_szLastSelectionLine.append(QChar('1'));
									m_szLastSelectionLine.append(QChar((curBack%10)+'0'));
								}
							break;
						}
					}

					switch(m_pWrappedBlockSelectionInfo->selection_type)
					{
						case KVI_IRCVIEW_BLOCK_SELECTION_TOTAL:
							DRAW_SELECTED_TEXT(pCurTextLine->szText,block->block_start,
								block->block_len,block->block_width)
						break;
						case KVI_IRCVIEW_BLOCK_SELECTION_LEFT:
							DRAW_SELECTED_TEXT(pCurTextLine->szText,block->block_start,
								m_pWrappedBlockSelectionInfo->part_1_length,
								m_pWrappedBlockSelectionInfo->part_1_width)
							DRAW_NORMAL_TEXT(pCurTextLine->szText,block->block_start+m_pWrappedBlockSelectionInfo->part_1_length,
								m_pWrappedBlockSelectionInfo->part_2_length,
								m_pWrappedBlockSelectionInfo->part_2_width)
						break;
						case KVI_IRCVIEW_BLOCK_SELECTION_RIGHT:
							DRAW_NORMAL_TEXT(pCurTextLine->szText,block->block_start,
								m_pWrappedBlockSelectionInfo->part_1_length,
								m_pWrappedBlockSelectionInfo->part_1_width)
							DRAW_SELECTED_TEXT(pCurTextLine->szText,block->block_start+m_pWrappedBlockSelectionInfo->part_1_length,
								m_pWrappedBlockSelectionInfo->part_2_length,
								m_pWrappedBlockSelectionInfo->part_2_width)
						break;
						case KVI_IRCVIEW_BLOCK_SELECTION_CENTRAL:
							DRAW_NORMAL_TEXT(pCurTextLine->szText,block->block_start,
								m_pWrappedBlockSelectionInfo->part_1_length,
								m_pWrappedBlockSelectionInfo->part_1_width)
							DRAW_SELECTED_TEXT(pCurTextLine->szText,block->block_start+m_pWrappedBlockSelectionInfo->part_1_length,
								m_pWrappedBlockSelectionInfo->part_2_length,
								m_pWrappedBlockSelectionInfo->part_2_width)
							DRAW_NORMAL_TEXT(pCurTextLine->szText,block->block_start+m_pWrappedBlockSelectionInfo->part_1_length+m_pWrappedBlockSelectionInfo->part_2_length,
								m_pWrappedBlockSelectionInfo->part_3_length,
								m_pWrappedBlockSelectionInfo->part_3_width)
						break;
						case KVI_IRCVIEW_BLOCK_SELECTION_ICON:
						{
							int theWdth = block->block_width;
							if(theWdth < 0)theWdth=width()-(curLeftCoord+KVI_IRCVIEW_HORIZONTAL_BORDER+scrollbarWidth);
							pa.fillRect(curLeftCoord,curBottomCoord - m_iFontLineSpacing + m_iFontDescent,theWdth,m_iFontLineSpacing,KVI_OPTION_MIRCCOLOR(KVI_OPTION_MSGTYPE(KVI_OUT_SELECT).back()));
							kvi_wslen_t the_len = kvi_wstrlen(block->pChunk->szPayload);
							m_szLastSelectionLine.append(QChar(block->pChunk->type));
							QString tmp;
							tmp.setUnicodeCodes(block->pChunk->szPayload,the_len);
							m_szLastSelectionLine.append(tmp);
							goto no_selection_paint;
						}
						break;
					}
				} else {
					if(block->pChunk && block->pChunk->type == KVI_TEXT_ICON)goto no_selection_paint;
					int wdth = block->block_width;
					if(wdth == 0)
					{
						// Last block before a word wrap , or a zero characters attribute block ?
						if(i < (pCurTextLine->iBlockCount - 1))
						{
							// There is another block...
							// Check if it is a wrap...
							if(pCurTextLine->pBlocks[i+1].pChunk == 0)wdth = widgetWidth-(curLeftCoord+KVI_IRCVIEW_HORIZONTAL_BORDER);
							// else simply a zero characters block
						}
						// else simply a zero characters block
					}
					DRAW_NORMAL_TEXT(pCurTextLine->szText,block->block_start,block->block_len,wdth)
				}
			} else {
				//No selection ...go fast!
no_selection_paint:
				if(block->pChunk && block->pChunk->type == KVI_TEXT_ICON)
				{
					int wdth = block->block_width;
					if(wdth < 0)wdth = widgetWidth - (curLeftCoord + KVI_IRCVIEW_HORIZONTAL_BORDER);
					int imageYPos = curBottomCoord - m_iRelativePixmapY;
					//Set the mask if needed
					if(curBack != KVI_TRANSPARENT && curBack < 16)
					{
						pa.fillRect(curLeftCoord,curBottomCoord - m_iFontLineSpacing + m_iFontDescent,wdth,m_iFontLineSpacing,KVI_OPTION_MIRCCOLOR((unsigned char)curBack));
					}
					QString tmpQ;
					tmpQ.setUnicodeCodes(block->pChunk->szSmileId,kvi_wstrlen(block->pChunk->szSmileId));
					QPixmap * daIcon =0;
					KviTextIcon* pIcon = g_pTextIconManager->lookupTextIcon(tmpQ);
					if(pIcon)
					{
						daIcon = pIcon->pixmap();
					}
					if(!daIcon)
					{
						// this should never happen since we do a check
						// when building the text icon block , but.. better safe than sorry:
						// so... we lost some icons ? wrong associations ?
						// recover it by displaying the "question mark" icon
						daIcon = g_pIconManager->getSmallIcon(KVI_SMALLICON_HELP); // must be there, eventually null pixmap :D
					}
					int moredown = 1; //used to center imager vertically (pixels which the image is moved more down)
					moredown += ((m_iFontLineSpacing - daIcon->height()) / 2);
					pa.drawPixmap(curLeftCoord + m_iIconSideSpacing,imageYPos + moredown,*(daIcon));

					//debug("SHifting by %d",block->block_width);
					curLeftCoord += block->block_width;
				} else {

					int wdth = block->block_width;
					if(wdth < 0)wdth = widgetWidth - (curLeftCoord + KVI_IRCVIEW_HORIZONTAL_BORDER);
	
					// FIXME: We could avoid this XSetForeground if the curFore was not changed....
					
					SET_PEN(curFore,block->pChunk ? block->pChunk->customFore : QColor());
	
					if(curBack != KVI_TRANSPARENT && curBack < 16 )
					{
						pa.fillRect(curLeftCoord,curBottomCoord - m_iFontLineSpacing + m_iFontDescent,wdth,m_iFontLineSpacing,KVI_OPTION_MIRCCOLOR((unsigned char)curBack));
					}
					
					if(curLink)
					{
						SET_PEN(KVI_OPTION_MSGTYPE(KVI_OUT_LINK).fore(),block->pChunk ? block->pChunk->customFore : QColor());
						pa.drawText(curLeftCoord,curBottomCoord,pCurTextLine->szText,block->block_start,block->block_len);
						pa.drawText(curLeftCoord+1,curBottomCoord,pCurTextLine->szText,block->block_start,block->block_len);
						pa.drawLine(curLeftCoord,curBottomCoord+2,curLeftCoord+wdth,curBottomCoord+2);
					} else if(curBold) {
						//Draw doubled font (simulate bold)
						pa.drawText(curLeftCoord,curBottomCoord,pCurTextLine->szText,block->block_start,block->block_len);
						pa.drawText(curLeftCoord + 1,curBottomCoord,pCurTextLine->szText,block->block_start,block->block_len);
					} else {
						pa.drawText(curLeftCoord,curBottomCoord,pCurTextLine->szText,block->block_start,block->block_len);
					}

					if(curUnderline)
					{ 
						//Draw a line under the text block....
						pa.drawLine(curLeftCoord,curBottomCoord+2,curLeftCoord+wdth,curBottomCoord+2);
					}
					curLeftCoord += block->block_width;
				}
			}
		}

		if(pCurTextLine == m_pCursorLine)
		{
			// paint the cursor line
			int iH = lineWrapsHeight + m_iFontLineSpacing;
#ifdef COMPILE_USE_QT4
			pa.setCompositionMode(QPainter::CompositionMode_SourceOut);
#else
			pa.setRasterOp(NotROP);
#endif
			pa.fillRect(0,curBottomCoord - iH,widgetWidth,iH + (m_iFontDescent << 1),Qt::black);
#ifdef COMPILE_USE_QT4
			pa.setCompositionMode(QPainter::CompositionMode_SourceOver);
#else
			pa.setRasterOp(CopyROP);
#endif
		}

		if(m_bMouseIsDown)
		{
			if(!m_szLastSelectionLine.isEmpty())
			{
				if(!m_szLastSelection.isEmpty())m_szLastSelection.prepend("\n");
				m_szLastSelection.prepend(m_szLastSelectionLine);
				m_szLastSelectionLine = "";
			}
		}

		curBottomCoord -= (lineWrapsHeight + m_iFontLineSpacing);

		if(pCurTextLine->uIndex == m_uLineMarkLineIndex)
		{
			if((curBottomCoord >= KVI_IRCVIEW_VERTICAL_BORDER) && !bLineMarkPainted)
			{
				// visible!
				bLineMarkPainted = true;
				//pa.setRasterOp(NotROP);
#ifdef COMPILE_USE_QT4
				pa.setPen(QPen(KVI_OPTION_COLOR(KviOption_colorIrcViewMarkLine),1,Qt::DotLine));
#else
				pa.setPen(QPen(KVI_OPTION_COLOR(KviOption_colorIrcViewMarkLine),1,QPen::DotLine));
#endif
				pa.drawLine(0,curBottomCoord,widgetWidth,curBottomCoord);
				//pa.setRasterOp(CopyROP);
			} // else was partially visible only
		}

		pCurTextLine    = pCurTextLine->pPrev;
	}
	
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
			lineWrapsHeight  = (pCurTextLine->uLineWraps) * m_iFontLineSpacing;
			curBottomCoord  -= lineWrapsHeight + m_iFontLineSpacing + m_iFontDescent;
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
			} else {
				// no next line ? hm... compare to the not visible one.. but this should never happen
				if(pCurTextLine->uIndex >= m_uLineMarkLineIndex)
					bLineMarkPainted = true; // yes, its somewhere before or on this line
			}
			if(bLineMarkPainted)
			{
				// need to mark it!
				//pa.setRasterOp(NotROP);
				//pa.setPen(Qt::black);
#ifdef COMPILE_USE_QT4
				pa.setPen(QPen(KVI_OPTION_COLOR(KviOption_colorIrcViewMarkLine),1,Qt::DotLine));
#else
				pa.setPen(QPen(KVI_OPTION_COLOR(KviOption_colorIrcViewMarkLine),1,QPen::DotLine));
#endif
				int x = widgetWidth - 8;
				int y = KVI_IRCVIEW_VERTICAL_BORDER;
				pa.drawLine(x,y,x,y);
				y++; pa.drawLine(x-1,y,x+1,y);
				y++; pa.drawLine(x-2,y,x+2,y);
				y++; pa.drawLine(x-3,y,x+3,y);
				y++; pa.drawLine(x-4,y,x+4,y);
				//pa.setRasterOp(CopyROP);
			}
		}
	}
	
	//Need to draw the sunken rect around the view now...
	pa.setPen(colorGroup().dark());
	pa.drawLine(0,0,widgetWidth,0);
	pa.drawLine(0,0,0,widgetHeight);
	pa.setPen(colorGroup().light());
	widgetWidth--;
	pa.drawLine(1,widgetHeight-1,widgetWidth,widgetHeight-1);
	pa.drawLine(widgetWidth,1,widgetWidth,widgetHeight);

	// COPY TO THE DISPLAY
#ifndef COMPILE_USE_QT4
	bitBlt(this,rectLeft,rectTop,pDoubleBufferPixmap,rectLeft,rectTop,rectWidth,rectHeight,Qt::CopyROP);
#endif
// else we use the Qt4 native double buffering
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// The IrcView : calculate line wraps
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define IRCVIEW_WCHARWIDTH(__c) (((__c).unicode() < 0xff) ? m_iFontCharacterWidth[(__c).unicode()] : m_pFm->width(__c))

void KviIrcView::calculateLineWraps(KviIrcViewLine *ptr,int maxWidth)
{
	//
	// Another monster
	//
	
	if(maxWidth<=0) return;

	if(ptr->iBlockCount != 0)kvi_free(ptr->pBlocks); // free any previous wrap blocks
	ptr->pBlocks      = (KviIrcViewWrappedBlock *)kvi_malloc(sizeof(KviIrcViewWrappedBlock)); // alloc one block
	ptr->iMaxLineWidth       = maxWidth; // calculus for this width
	ptr->iBlockCount      = 0;        // it will be ++
	ptr->uLineWraps           = 0;        // no line wraps yet

	unsigned int curAttrBlock = 0;        // Current attribute block
	int curLineWidth          = 0;

	// init the first block
	ptr->pBlocks->block_start = 0;
	ptr->pBlocks->block_len   = 0;
	ptr->pBlocks->block_width = 0;
	ptr->pBlocks->pChunk  = &(ptr->pChunks[0]); // always an attribute block

	int maxBlockLen = ptr->pChunks->iTextLen; // ptr->pChunks[0].iTextLen

	const QChar * unicode = ptr->szText.unicode();

	for(;;)
	{
		//Calculate the block_width
		register const QChar * p = unicode + ptr->pBlocks[ptr->iBlockCount].block_start;
		int curBlockLen   = 0;
		int curBlockWidth = 0;

		if(ptr->pChunks[curAttrBlock].type == KVI_TEXT_ICON)
		{
			curBlockWidth = m_iIconWidth;
		} else {
			while(curBlockLen < maxBlockLen)
			{
				// FIXME: this is ugly :/
				curBlockWidth += IRCVIEW_WCHARWIDTH(*p);
				curBlockLen++;
				p++;
			}
		}
		//Check the length
		curLineWidth += curBlockWidth;

		if(curLineWidth < maxWidth)
		{
			//debug("Block of %d pix and len %d with type %d",ptr->pBlocks[ptr->iBlockCount].block_width,ptr->pBlocks[ptr->iBlockCount].block_len,ptr->pChunks[curAttrBlock].type);
			//Ok....proceed to next block
			ptr->pBlocks[ptr->iBlockCount].block_len   = curBlockLen;
			ptr->pBlocks[ptr->iBlockCount].block_width = curBlockWidth;
			curAttrBlock++;
			ptr->iBlockCount++;
			//Process the next block of data in the next loop or return if have no more blocks
			if(curAttrBlock < ptr->uChunkCount)
			{
				ptr->pBlocks = (KviIrcViewWrappedBlock *)kvi_realloc(ptr->pBlocks,(ptr->iBlockCount + 1) * sizeof(KviIrcViewWrappedBlock));
				ptr->pBlocks[ptr->iBlockCount].block_start = ptr->pChunks[curAttrBlock].iTextStart;
				ptr->pBlocks[ptr->iBlockCount].block_len = 0;
				ptr->pBlocks[ptr->iBlockCount].block_width = 0;
				ptr->pBlocks[ptr->iBlockCount].pChunk  = &(ptr->pChunks[curAttrBlock]);
				maxBlockLen = ptr->pBlocks[ptr->iBlockCount].pChunk->iTextLen;
			} else return;
		} else {
			//Need word wrap
			//First go back to an admissible width
			while((curLineWidth >= maxWidth) && curBlockLen)
			{
				p--;
				curBlockLen--;
				curLineWidth-=IRCVIEW_WCHARWIDTH(*p);
			}
			//Now look for a space
			while((*p != ' ') && curBlockLen)
			{ 
				p--;
				curBlockLen--;
				curLineWidth-=IRCVIEW_WCHARWIDTH(*p);
			}

			//If we ran up to the beginning of the block....
			if(curBlockLen == 0)
			{
				if(ptr->pChunks[curAttrBlock].type == KVI_TEXT_ICON)
				{
					// This is an icon block: needs to be wrapped differently:
					// The wrap block goes BEFORE the icon itself
					ptr->pBlocks[ptr->iBlockCount].pChunk  = 0;
					ptr->pBlocks[ptr->iBlockCount].block_width = 0;
					ptr->iBlockCount++;
					ptr->pBlocks = (KviIrcViewWrappedBlock *)kvi_realloc(ptr->pBlocks,(ptr->iBlockCount + 1) * sizeof(KviIrcViewWrappedBlock));
					ptr->pBlocks[ptr->iBlockCount].block_start = p - unicode;
					ptr->pBlocks[ptr->iBlockCount].block_len   = 0;
					ptr->pBlocks[ptr->iBlockCount].block_width = 0;
					ptr->pBlocks[ptr->iBlockCount].pChunk  = &(ptr->pChunks[curAttrBlock]);
					goto wrap_line;
				}
				//Don't like it....forced wrap here...
				//Go ahead up to the biggest possible string
				if(maxBlockLen > 0)
				{
					do
					{
						curBlockLen++;
						p++;
						curLineWidth+=IRCVIEW_WCHARWIDTH(*p);
					} while((curLineWidth < maxWidth) && (curBlockLen < maxBlockLen));
					//Now overrunned , go back 1 char
					p--;
					curBlockLen--;
				}
				//K...wrap
			} else {
				//found a space...
				//include it in the first block
				p++;
				curBlockLen++;
			}

			ptr->pBlocks[ptr->iBlockCount].block_len = curBlockLen;
			ptr->pBlocks[ptr->iBlockCount].block_width = -1; // word wrap --> negative block_width
			maxBlockLen-=curBlockLen;
			ptr->iBlockCount++;
			ptr->pBlocks = (KviIrcViewWrappedBlock *)kvi_realloc(ptr->pBlocks,(ptr->iBlockCount + 1) * sizeof(KviIrcViewWrappedBlock));
			ptr->pBlocks[ptr->iBlockCount].block_start = p - unicode;
			ptr->pBlocks[ptr->iBlockCount].block_len   = 0;
			ptr->pBlocks[ptr->iBlockCount].block_width = 0;
			ptr->pBlocks[ptr->iBlockCount].pChunk  = 0;
wrap_line:
			curLineWidth = 0;
			ptr->uLineWraps++;
			if(ptr->uLineWraps == 1)
			{
				if(KVI_OPTION_BOOL(KviOption_boolIrcViewWrapMargin))maxWidth-=m_iWrapMargin;
				if(maxWidth<=0) return;
			}
		}
	}

	ptr->iBlockCount++;
}

//================= calculateSelectionBounds ==================//

void KviIrcView::calculateSelectionBounds()
{
	if(m_mousePressPos.y() < m_mouseCurrentPos.y())
	{
		m_iSelectionTop     = m_mousePressPos.y();
		m_iSelectionBottom  = m_mouseCurrentPos.y();
		m_iSelectionBegin   = m_mousePressPos.x();
		m_iSelectionEnd     = m_mouseCurrentPos.x();
	} else {
		m_iSelectionTop     = m_mouseCurrentPos.y();
		m_iSelectionBottom  = m_mousePressPos.y();
		m_iSelectionBegin   = m_mouseCurrentPos.x();
		m_iSelectionEnd     = m_mousePressPos.x();
	}

	if(m_iSelectionBegin < m_iSelectionEnd)
	{
		m_iSelectionLeft    = m_iSelectionBegin;
		m_iSelectionRight   = m_iSelectionEnd;
	} else {
		m_iSelectionLeft    = m_iSelectionEnd;
		m_iSelectionRight   = m_iSelectionBegin;
	}
}


//=============== checkSelectionBlock ===============//

bool KviIrcView::checkSelectionBlock(KviIrcViewLine * line,int left,int bottom,int bufIndex)
{
	//
	// Yahoo!!!!
	//
	const QChar * unicode = line->szText.unicode();
	register const QChar * p = unicode + line->pBlocks[bufIndex].block_start;

	int top = bottom-m_iFontLineSpacing;
	int right  = ((line->pBlocks[bufIndex].block_width >= 0) ? \
					left+line->pBlocks[bufIndex].block_width : width()-(KVI_IRCVIEW_HORIZONTAL_BORDER + m_pScrollBar->width()));
	if(bottom < m_iSelectionTop)return false; //The selection starts under this line
	if(top > m_iSelectionBottom)return false; //The selection ends over this line
	if((top >= m_iSelectionTop)&&(bottom < m_iSelectionBottom))
	{
		//Whole line selected
		if(line->pBlocks[bufIndex].pChunk && line->pBlocks[bufIndex].pChunk->type == KVI_TEXT_ICON)
			m_pWrappedBlockSelectionInfo->selection_type = KVI_IRCVIEW_BLOCK_SELECTION_ICON;
		else
			m_pWrappedBlockSelectionInfo->selection_type = KVI_IRCVIEW_BLOCK_SELECTION_TOTAL;
		return true;
	}
	if((top < m_iSelectionTop) && (bottom >= m_iSelectionBottom))
	{
		//Selection begins and ends in this line
		if(right < m_iSelectionLeft)return false;
		if(left  > m_iSelectionRight)return false;
		if(line->pBlocks[bufIndex].pChunk && line->pBlocks[bufIndex].pChunk->type == KVI_TEXT_ICON)
		{
			m_pWrappedBlockSelectionInfo->selection_type = KVI_IRCVIEW_BLOCK_SELECTION_ICON;
			return true;
		}
		if((right <= m_iSelectionRight) && (left > m_iSelectionLeft))
		{
			//Whole line selected
			m_pWrappedBlockSelectionInfo->selection_type = KVI_IRCVIEW_BLOCK_SELECTION_TOTAL;
			return true;
		}
		if((right > m_iSelectionRight) && (left <= m_iSelectionLeft))
		{
			//Selection ends and begins in THIS BLOCK!
			m_pWrappedBlockSelectionInfo->selection_type = KVI_IRCVIEW_BLOCK_SELECTION_CENTRAL;
			m_pWrappedBlockSelectionInfo->part_1_length = 0;
			m_pWrappedBlockSelectionInfo->part_1_width  = 0;
			while((left <= m_iSelectionLeft) && (m_pWrappedBlockSelectionInfo->part_1_length < line->pBlocks[bufIndex].block_len)){
				int www = IRCVIEW_WCHARWIDTH(*p);
				left += www;
				m_pWrappedBlockSelectionInfo->part_1_width += www;
				p++;
				m_pWrappedBlockSelectionInfo->part_1_length++;
			}
			//Need to include the first character
			if(m_pWrappedBlockSelectionInfo->part_1_length > 0)
			{
				m_pWrappedBlockSelectionInfo->part_1_length--;
				p--;
				int www = IRCVIEW_WCHARWIDTH(*p);
				left -= www;
				m_pWrappedBlockSelectionInfo->part_1_width -= www;
			}
			int maxLenNow = line->pBlocks[bufIndex].block_len-m_pWrappedBlockSelectionInfo->part_1_length;
			int maxWidthNow = line->pBlocks[bufIndex].block_width-m_pWrappedBlockSelectionInfo->part_1_width;
			m_pWrappedBlockSelectionInfo->part_2_length = 0;
			m_pWrappedBlockSelectionInfo->part_2_width  = 0;
			while((left < m_iSelectionRight) && (m_pWrappedBlockSelectionInfo->part_2_length < maxLenNow))
			{
				int www = IRCVIEW_WCHARWIDTH(*p);
				left += www;
				m_pWrappedBlockSelectionInfo->part_2_width += www;
				p++;
				m_pWrappedBlockSelectionInfo->part_2_length++;
			}		
			m_pWrappedBlockSelectionInfo->part_3_length = maxLenNow-m_pWrappedBlockSelectionInfo->part_2_length;
			m_pWrappedBlockSelectionInfo->part_3_width  = maxWidthNow-m_pWrappedBlockSelectionInfo->part_2_width;
			return true;
		}
		if(right > m_iSelectionRight)
		{
			//Selection ends in THIS BLOCK!
			m_pWrappedBlockSelectionInfo->selection_type = KVI_IRCVIEW_BLOCK_SELECTION_LEFT;
			m_pWrappedBlockSelectionInfo->part_1_length = 0;
			m_pWrappedBlockSelectionInfo->part_1_width  = 0;
			while((left < m_iSelectionRight) && (m_pWrappedBlockSelectionInfo->part_1_length < line->pBlocks[bufIndex].block_len))
			{
				int www = IRCVIEW_WCHARWIDTH(*p);
				left += www;
				m_pWrappedBlockSelectionInfo->part_1_width += www;
				p++;
				m_pWrappedBlockSelectionInfo->part_1_length++;
			}
			m_pWrappedBlockSelectionInfo->part_2_length = line->pBlocks[bufIndex].block_len-m_pWrappedBlockSelectionInfo->part_1_length;
			m_pWrappedBlockSelectionInfo->part_2_width  = line->pBlocks[bufIndex].block_width-m_pWrappedBlockSelectionInfo->part_1_width;
			//debug("%d",m_pWrappedBlockSelectionInfo->part_2_width);
			return true;
		}
		//Selection begins in THIS BLOCK!
		m_pWrappedBlockSelectionInfo->selection_type = KVI_IRCVIEW_BLOCK_SELECTION_RIGHT;
		m_pWrappedBlockSelectionInfo->part_1_length = 0;
		m_pWrappedBlockSelectionInfo->part_1_width  = 0;
		while((left <= m_iSelectionLeft) && (m_pWrappedBlockSelectionInfo->part_1_length < line->pBlocks[bufIndex].block_len))
		{
				int www = IRCVIEW_WCHARWIDTH(*p);
				left += www;
				m_pWrappedBlockSelectionInfo->part_1_width += www;
			p++;
			m_pWrappedBlockSelectionInfo->part_1_length++;
		}
		//Need to include the first character
		if(m_pWrappedBlockSelectionInfo->part_1_length > 0)
		{
			m_pWrappedBlockSelectionInfo->part_1_length--;
			p--;
			int www = IRCVIEW_WCHARWIDTH(*p);
			left -= www;
			m_pWrappedBlockSelectionInfo->part_1_width -= www;
		}
		m_pWrappedBlockSelectionInfo->part_2_length = line->pBlocks[bufIndex].block_len-m_pWrappedBlockSelectionInfo->part_1_length;
		m_pWrappedBlockSelectionInfo->part_2_width  = line->pBlocks[bufIndex].block_width-m_pWrappedBlockSelectionInfo->part_1_width;
		return true;
	}

	if(top < m_iSelectionTop)
	{
		//Selection starts in this line
		if(right < m_iSelectionBegin)return false;
		if(line->pBlocks[bufIndex].pChunk && line->pBlocks[bufIndex].pChunk->type == KVI_TEXT_ICON)
		{
			m_pWrappedBlockSelectionInfo->selection_type = KVI_IRCVIEW_BLOCK_SELECTION_ICON;
			return true;
		}
		if(left > m_iSelectionBegin)
		{
			//Whole block selected
			m_pWrappedBlockSelectionInfo->selection_type = KVI_IRCVIEW_BLOCK_SELECTION_TOTAL;
			return true;
		}
		//Selection begins in THIS BLOCK!
		m_pWrappedBlockSelectionInfo->selection_type = KVI_IRCVIEW_BLOCK_SELECTION_RIGHT;
		m_pWrappedBlockSelectionInfo->part_1_length = 0;
		m_pWrappedBlockSelectionInfo->part_1_width  = 0;
		while((left <= m_iSelectionBegin) && (m_pWrappedBlockSelectionInfo->part_1_length < line->pBlocks[bufIndex].block_len))
		{
			int www = IRCVIEW_WCHARWIDTH(*p);
			left += www;
			m_pWrappedBlockSelectionInfo->part_1_width += www;
			p++;
			m_pWrappedBlockSelectionInfo->part_1_length++;
		}
		//Need to include the first character
		if(m_pWrappedBlockSelectionInfo->part_1_length > 0)
		{
			m_pWrappedBlockSelectionInfo->part_1_length--;
			p--;
			int www = IRCVIEW_WCHARWIDTH(*p);
			left -= www;
			m_pWrappedBlockSelectionInfo->part_1_width -= www;
		}
		m_pWrappedBlockSelectionInfo->part_2_length = line->pBlocks[bufIndex].block_len-m_pWrappedBlockSelectionInfo->part_1_length;
		m_pWrappedBlockSelectionInfo->part_2_width  = line->pBlocks[bufIndex].block_width-m_pWrappedBlockSelectionInfo->part_1_width;
		return true;
	}
	//Selection ends in this line
	if(left  > m_iSelectionEnd)return false;
	if(line->pBlocks[bufIndex].pChunk && line->pBlocks[bufIndex].pChunk->type == KVI_TEXT_ICON)
	{
		m_pWrappedBlockSelectionInfo->selection_type = KVI_IRCVIEW_BLOCK_SELECTION_ICON;
		return true;
	}
	if(right < m_iSelectionEnd)
	{
		//Whole block selected
		m_pWrappedBlockSelectionInfo->selection_type = KVI_IRCVIEW_BLOCK_SELECTION_TOTAL;
		return true;
	}
	//Selection ends in THIS BLOCK!
	m_pWrappedBlockSelectionInfo->selection_type = KVI_IRCVIEW_BLOCK_SELECTION_LEFT;
	m_pWrappedBlockSelectionInfo->part_1_length = 0;
	m_pWrappedBlockSelectionInfo->part_1_width  = 0;
	while((left < m_iSelectionEnd) && (m_pWrappedBlockSelectionInfo->part_1_length < line->pBlocks[bufIndex].block_len))
	{
		int www = IRCVIEW_WCHARWIDTH(*p);
		left += www;
		m_pWrappedBlockSelectionInfo->part_1_width += www;
		p++;
		m_pWrappedBlockSelectionInfo->part_1_length++;
	}
	m_pWrappedBlockSelectionInfo->part_2_length = line->pBlocks[bufIndex].block_len-m_pWrappedBlockSelectionInfo->part_1_length;
	m_pWrappedBlockSelectionInfo->part_2_width  = line->pBlocks[bufIndex].block_width-m_pWrappedBlockSelectionInfo->part_1_width;
	return true;
}

//============ recalcFontVariables ==============//

void KviIrcView::recalcFontVariables(const QFontMetrics &fm,const QFontInfo &fi)
{
	// FIXME: #warning "OPTIMIZE THIS: GLOBAL VARIABLES"
	if(m_pFm)delete m_pFm;
	m_pFm = new QFontMetrics(fm);
	m_iFontLineSpacing = m_pFm->lineSpacing();
	if(m_iFontLineSpacing < KVI_IRCVIEW_PIXMAP_SIZE && KVI_OPTION_BOOL(KviOption_boolIrcViewShowImages))
	{
		m_iFontLineSpacing = KVI_IRCVIEW_PIXMAP_SIZE;
	}
	m_iFontDescent     =m_pFm->descent();
	m_iFontLineWidth   =m_pFm->lineWidth();
	// cache the first 256 characters
	for(unsigned short i=0;i<256;i++)
	{
		m_iFontCharacterWidth[i]=m_pFm->width(QChar(i));
	}
	if(m_iFontLineWidth==0)m_iFontLineWidth=1;
	m_iWrapMargin = m_pFm->width("wwww");
	//for(int i=0;i<256;i++)m_iFontCharacterWidth[i]=fm.width((char)i);
	m_iMinimumPaintWidth = (m_pFm->width('w') << 1)+m_iWrapMargin;
	m_iRelativePixmapY = (m_iFontLineSpacing + KVI_IRCVIEW_PIXMAP_SIZE) >> 1;
	m_iIconWidth = m_pFm->width("w");

	if(fi.fixedPitch() && (m_iIconWidth > 0))
	{
		while(m_iIconWidth < 18)m_iIconWidth += m_iIconWidth;
		m_iIconSideSpacing = (m_iIconWidth - 16) >> 1;
	} else {
		m_iIconWidth = 18;
		m_iIconSideSpacing = 1;
	}
}

//================ resizeEvent ===============//

void KviIrcView::resizeEvent(QResizeEvent *)
{
	int iScr = m_pScrollBar->sizeHint().width();
	int iLeft = width()-iScr;
	m_pToolsButton->setGeometry(iLeft,0,iScr,iScr);
	m_pScrollBar->setGeometry(iLeft,iScr,iScr,height() - iScr);

	if(m_pToolWidget)
	{
		if( ((m_pToolWidget->x() + m_pToolWidget->width()) > (iLeft - 1)) ||
			((m_pToolWidget->y() + m_pToolWidget->height()) > (height() - 1)))
		{
			m_pToolWidget->move(10,10);
		}
	}
}

QSize KviIrcView::sizeHint() const
{
	QSize ret(KVI_IRCVIEW_SIZEHINT_WIDTH,KVI_IRCVIEW_SIZEHINT_HEIGHT);
	return ret;
}

void KviIrcView::showToolsPopup()
{
	if(!m_pToolsPopup)
		m_pToolsPopup = new KviTalPopupMenu(this);

	m_pToolsPopup->clear();

	if(m_pToolWidget)
		m_pToolsPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SEARCH)),__tr2qs("Hide Find Window"),this,SLOT(toggleToolWidget()));
	else
		m_pToolsPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SEARCH)),__tr2qs("Show Find Window"),this,SLOT(toggleToolWidget()));
	m_pToolsPopup->insertSeparator();
	m_pToolsPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_PLUS)),__tr2qs("Zoom In"),this,SLOT(increaseFontSize()));
	m_pToolsPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MINUS)),__tr2qs("Zoom Out"),this,SLOT(decreaseFontSize()));
	m_pToolsPopup->insertItem(__tr2qs("Choose Temporary Font..."),this,SLOT(chooseFont()));
	m_pToolsPopup->insertItem(__tr2qs("Choose Temporary Background..."),this,SLOT(chooseBackground()));
	int id = m_pToolsPopup->insertItem(__tr2qs("Reset Temporary Background"),this,SLOT(resetBackground()));
	m_pToolsPopup->setItemEnabled(id,m_pPrivateBackgroundPixmap != 0);
	m_pToolsPopup->insertSeparator();
	m_pToolsPopup->insertItem(__tr2qs("Clear Buffer"),this,SLOT(clearBuffer()));
	
	QSize s = m_pToolsPopup->sizeHint();

	m_pToolsPopup->popup(m_pToolsButton->mapToGlobal(QPoint(m_pToolsButton->width() - s.width(),m_pToolsButton->height())));
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
	if(p > 2)p--;
	f.setPointSize(p);
	setFont(f);
}

void KviIrcView::chooseFont()
{
	bool bOk;
	QFont f = QFontDialog::getFont(&bOk,font(),this);
	if(!bOk)return;
	setFont(f);
}

void KviIrcView::chooseBackground()
{
	QString f;
	if(!KviFileDialog::askForOpenFileName(f,__tr2qs("Choose the background image...")))return;
	if(f.isEmpty())return;
	QPixmap p(f);
	if(p.isNull())
	{
		QMessageBox::information(this,__tr2qs("Invalid image"),__tr2qs("Failed to load the selected image"),__tr2qs("Ok"));
		return;
	}
	setPrivateBackgroundPixmap(p);
}

void KviIrcView::resetBackground()
{
	setPrivateBackgroundPixmap(0);
}

void KviIrcView::toggleToolWidget()
{
	if(m_pToolWidget)
	{
		delete m_pToolWidget;
		m_pToolWidget = 0;
		m_pCursorLine = 0;
		repaint();

	} else {
		m_pToolWidget = new KviIrcViewToolWidget(this);
		int w = m_pToolWidget->sizeHint().width();
		m_pToolWidget->move(width() - (w + 40),10);
		m_pToolWidget->show();
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// The IrcView : find
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void KviIrcView::setCursorLine(KviIrcViewLine * l)
{
	m_pCursorLine = l;
	if(m_pCursorLine == m_pCurLine)
	{
	
	repaint();

		return;
	}
	int sc = m_pScrollBar->value();
	l = m_pCurLine;
	if(m_pCursorLine->uIndex > m_pCurLine->uIndex)
	{
		// The cursor line is below the current line
		while(l && (l != m_pCursorLine))
		{
			l = l->pNext;
			sc++;
		}
		if(!l)return;
		if(sc != m_pScrollBar->value())
		{
			m_pCurLine = m_pCursorLine;
			m_iLastScrollBarValue = sc;
			m_pScrollBar->setValue(sc);
		} else {
			repaint();
		}
	} else {
		// The cursor line is over the current line
		// Here we're in trouble :D
		int curBottomCoord = height() - KVI_IRCVIEW_VERTICAL_BORDER;
		int maxLineWidth   = width();
		if(KVI_OPTION_BOOL(KviOption_boolIrcViewShowImages))maxLineWidth -= KVI_IRCVIEW_PIXMAP_SEPARATOR_AND_DOUBLEBORDER_WIDTH;
		//Make sure that we have enough space to paint something...
		if(maxLineWidth < m_iMinimumPaintWidth)return; // ugh
		//And loop thru lines until we not run over the upper bound of the view
		KviIrcViewLine * curLine = m_pCurLine;
		while(l)
		{
			if(maxLineWidth != l->iMaxLineWidth)calculateLineWraps(l,maxLineWidth);
			curBottomCoord -= (l->uLineWraps + 1) * m_iFontLineSpacing;
			while(curLine && (curBottomCoord < KVI_IRCVIEW_VERTICAL_BORDER))
			{
				if(curLine->iMaxLineWidth != maxLineWidth)calculateLineWraps(curLine,maxLineWidth);
				curBottomCoord += ((curLine->uLineWraps + 1) * m_iFontLineSpacing) + m_iFontDescent;
				curLine = curLine->pPrev;
				sc--;
			}
			if(l == m_pCursorLine)break;
			curBottomCoord -= m_iFontDescent;
			l = l->pPrev;
		}
		if(!curLine)return;
		if(sc != m_pScrollBar->value())
		{
			m_pCurLine = curLine;
			m_iLastScrollBarValue = sc;
			m_pScrollBar->setValue(sc);
		} else {
			repaint();
		}
	}
}

void KviIrcView::findNext(const QString& szText,bool bCaseS,bool bRegExp,bool bExtended)
{
	KviIrcViewLine * l = m_pCursorLine;
	if(!l)l = m_pCurLine;
	if(l)
	{
		l = l->pNext;
		if(!l)l = m_pFirstLine;
		KviIrcViewLine * start = l;

		int idx = -1;

		do{
			if(m_pToolWidget)
			{
				if(!(m_pToolWidget->messageEnabled(l->iMsgType)))goto do_pNext;
			}

			if(bRegExp)
			{
				QRegExp re(szText,bCaseS,!bExtended);
#if QT_VERSION >= 300
				idx = re.search(l->szText,0);
#else
				idx = re.find(l->szText,0);
#endif
			} else {
				QString tmp = l->szText;
				idx = tmp.find(szText,0,bCaseS);
			}

			if(idx != -1)
			{
				setCursorLine(l);
				if(m_pToolWidget)
				{
					QString tmp;
					KviQString::sprintf(tmp,__tr2qs("Pos %d"),idx);
					m_pToolWidget->setFindResult(tmp);
				}
				return;
			}

do_pNext:

			l = l->pNext;
			if(!l)l = m_pFirstLine;

		} while(l != start);
	}
	m_pCursorLine = 0;
	repaint();
	if(m_pToolWidget)m_pToolWidget->setFindResult(__tr2qs("Not found"));
}


void KviIrcView::findPrev(const QString& szText,bool bCaseS,bool bRegExp,bool bExtended)
{
	KviIrcViewLine * l = m_pCursorLine;
	if(!l)l = m_pCurLine;
	if(l)
	{
		l = l->pPrev;
		if(!l)l = m_pLastLine;
		KviIrcViewLine * start = l;

		int idx = -1;

		do{

			if(m_pToolWidget)
			{
				if(!(m_pToolWidget->messageEnabled(l->iMsgType)))goto do_pPrev;
			}

			if(bRegExp)
			{
				QRegExp re(szText,bCaseS,!bExtended);
#if QT_VERSION >= 300
				idx = re.search(l->szText,0);
#else
				idx = re.find(l->szText,0);
#endif
			} else {
				QString tmp = l->szText;
				idx = tmp.find(szText,0,bCaseS);;
			}

			if(idx != -1)
			{
				setCursorLine(l);
				if(m_pToolWidget)
				{
					QString tmp;
					KviQString::sprintf(tmp,__tr2qs("Pos %d"),idx);
					m_pToolWidget->setFindResult(tmp);
				}
				return;
			}

do_pPrev:

			l = l->pPrev;
			if(!l)l = m_pLastLine;

		} while(l != start);
	}
	m_pCursorLine = 0;

	repaint();
	if(m_pToolWidget)m_pToolWidget->setFindResult(__tr2qs("Not found"));
}

/*
void KviIrcView::findClosestPositionInText(int xCursorPos,int yCursorPos,KviIrcViewPositionInText &pos)
{
	pos.pLine = getVisibleLineAt(xCursorPos,uCursorPos);
}
*/


KviIrcViewLine * KviIrcView::getVisibleLineAt(int xPos,int yPos)
{
	KviIrcViewLine * l = m_pCurLine;
	int iTop = height() + m_iFontDescent - KVI_IRCVIEW_VERTICAL_BORDER;

	while(iTop > yPos)
	{
		if(l)
		{
			iTop -= ((l->uLineWraps + 1) * m_iFontLineSpacing) + m_iFontDescent;
			if(iTop <= yPos)return l;
			l = l->pPrev;
		} else return 0;
	}
	return 0;
}

KviIrcViewWrappedBlock * KviIrcView::getLinkUnderMouse(int xPos,int yPos,QRect * pRect,QString * linkCmd,QString * linkText)
{
	KviIrcViewLine * l = m_pCurLine;
	int iTop = height() + m_iFontDescent - KVI_IRCVIEW_VERTICAL_BORDER;

	while(iTop > yPos)
	{
		if(l)
		{
			iTop -= ((l->uLineWraps + 1) * m_iFontLineSpacing) + m_iFontDescent;
			if(iTop <= yPos)
			{
				// got the right KviIrcViewLine
				int iLeft = KVI_IRCVIEW_HORIZONTAL_BORDER;
				if(KVI_OPTION_BOOL(KviOption_boolIrcViewShowImages))iLeft += KVI_IRCVIEW_PIXMAP_AND_SEPARATOR;
				int firstRowTop = iTop;
				int i = 0;

				for(;;)
				{
					if(yPos <= iTop + m_iFontLineSpacing)
					{
						// this row!!!
						if(iTop != firstRowTop)
							if(KVI_OPTION_BOOL(KviOption_boolIrcViewWrapMargin))iLeft+=m_iWrapMargin;
						if(xPos < iLeft)return 0;
						int iBlockWidth = 0;
						int iLastEscapeBlock = -1;
						int iLastEscapeBlockTop = -1;
						for(;;)
						{
							int iLastLeft = iLeft;
							if(i >= l->iBlockCount)return 0;
							if(l->pBlocks[i].pChunk)
								if(l->pBlocks[i].pChunk->type == KVI_TEXT_ESCAPE)
								{
								 iLastEscapeBlock=i;
								 iLastEscapeBlockTop=iTop;
								}
							if(l->pBlocks[i].pChunk)
								if(l->pBlocks[i].pChunk->type == KVI_TEXT_UNESCAPE) iLastEscapeBlock=-1;
							if(l->pBlocks[i].block_width > 0)
							{
								iBlockWidth = l->pBlocks[i].block_width;
								iLeft += iBlockWidth;
							} else {
								if(i < (l->iBlockCount - 1))
								{
									// There is another block...
									// Check if it is a wrap...
									if(l->pBlocks[i+1].pChunk == 0)
									{
										iBlockWidth = width() - iLastLeft;
										iLeft = width();
									}
									// else simply a zero characters block
								}
							}
							if(xPos < iLeft)
							{
								// Got it!
								// link ?
								bool bHadWordWraps = false;
								while(l->pBlocks[i].pChunk == 0)
								{
									// word wrap ?
									if(i >= 0)
									{
										i--;
										bHadWordWraps = true;
									} else return 0; // all word wraps ?!!!
								}
								if(iLastEscapeBlock != -1)
								{
 									int iLeftBorder=iLeft;
									int k;
									for(k = i ; k>=iLastEscapeBlock ; k--)
										iLeftBorder-=l->pBlocks[k].block_width;
									int iRightBorder=0;
									unsigned int uLineWraps = 0;
									for(k = iLastEscapeBlock;; k++)
									{
										if(l->pBlocks[k].pChunk)
											if(l->pBlocks[k].pChunk->type != KVI_TEXT_UNESCAPE)
												iRightBorder+=l->pBlocks[k].block_width;
											else
												break;
										else
										{
											uLineWraps++;
											bHadWordWraps=1;
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
										linkCmd->setUnicodeCodes(l->pBlocks[iLastEscapeBlock].pChunk->szPayload,kvi_wstrlen(l->pBlocks[iLastEscapeBlock].pChunk->szPayload));
										linkCmd->stripWhiteSpace();
										if((*linkCmd)=="nc") (*linkCmd)="n";
									}
									if(linkText)
									{
										QString szLink;
										int iEndOfLInk = iLastEscapeBlock;
										while(1)
										{
											if(l->pBlocks[iEndOfLInk].pChunk)
												if(l->pBlocks[iEndOfLInk].pChunk->type != KVI_TEXT_UNESCAPE)
												{
													switch(l->pBlocks[iEndOfLInk].pChunk->type)
													{
														case KVI_TEXT_BOLD:
														case KVI_TEXT_UNDERLINE:
														case KVI_TEXT_REVERSE:
														case KVI_TEXT_RESET:
															szLink.append(QChar(l->pBlocks[iEndOfLInk].pChunk->type));
														break;
														case KVI_TEXT_COLOR:
															szLink.append(QChar(KVI_TEXT_COLOR));
															if(l->pBlocks[iEndOfLInk].pChunk->colors.fore != KVI_NOCHANGE)
															{
																szLink.append(QString("%1").arg((int)(l->pBlocks[iEndOfLInk].pChunk->colors.fore)));
															}
															if(l->pBlocks[iEndOfLInk].pChunk->colors.back != KVI_NOCHANGE)
															{
																szLink.append(QChar(','));
																szLink.append(QString("%1").arg((int)(l->pBlocks[iEndOfLInk].pChunk->colors.back)));
															}
														break;
													}
													szLink.append(l->szText.mid(l->pBlocks[iEndOfLInk].block_start,l->pBlocks[iEndOfLInk].block_len));
												} else
													break;
											iEndOfLInk++;
											
										}
										*linkText=szLink;
										// grab the rest of the link visible string
										// Continue while we do not find a non word wrap block block
										for(int bufIndex = (i + 1);bufIndex < l->iBlockCount;bufIndex++)
										{
											if(l->pBlocks[bufIndex].pChunk ) break; //finished : not a word wrap
											else {
												linkText->append(l->szText.mid(l->pBlocks[bufIndex].block_start,l->pBlocks[bufIndex].block_len));
											}
										}
									}
									return &(l->pBlocks[iLastEscapeBlock]);
								}
								if(l->pBlocks[i].pChunk->type == KVI_TEXT_ICON)
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
										tmp.setUnicodeCodes(l->pBlocks[i].pChunk->szPayload,kvi_wstrlen(l->pBlocks[i].pChunk->szPayload));
										linkCmd->append(tmp);
										linkCmd->stripWhiteSpace();
									}
									if(linkText)
									{
										*linkText = "";
									}
									return &(l->pBlocks[i]);
								}
								return 0;
							}
							i++;
						}
					} else {
						// run until a word wrap block
						i++; //at least one block!
						while(i < l->iBlockCount)
						{
							// still ok to run right
							if(l->pBlocks[i].pChunk == 0)
							{
//								i++;
								break;
							} else i++;
						}
						if(i >= l->iBlockCount)return 0;
						iTop += m_iFontLineSpacing;
					}
				}
			} else l = l->pPrev;
		} else return 0;
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mouse handling routines
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
	@doc: escape_sequences
	@title:
		Escape sequences and clickable links
	@type:
		generic
	@body:
		The KVIrc view widgets support clickable links.[br]
		The links can be created using special escape sequences in the text
		passed to the [cmd]echo[/cmd] command.[br]
		KVIrc uses some escape sequences in the text "echoed" internally.[br]
		The simplest way to explain it is to use an example:[br]
		[example]
			[cmd]echo[/cmd] This is a [fnc]$cr[/fnc]![!dbl][cmd]echo[/cmd] You have clicked it![fnc]$cr[/fnc]\clickable link$cr !
		[/example]
		The example above will show the following text line: "This is a clickable link".
		If you move the mouse over the words "clickable link", you will see the text highlighted.[br]
		Once you double-click one of that words, the command "[cmd]echo[/cmd] You have clicked it!" will be executed.[br]
		The format looks complex ?... it is not...just read on.[br]

		<cr>!<link_type><cr><visible text><cr>
		<cr>!<escape_command><cr><visible text><cr>

		[big]Escape format[/big]
		The whole escape sequence format is the following:[br]
		[b]<cr>!<escape_command><cr><visible text><cr>[/b][br]
		<cr> is the carriage return character. You can obtain it by using the [fnc]]$cr[/fnc] function.[br]
		<visible text> is the text that will appear as "link" when you move the mouse over it.[br]
		<escape_command> is the description of the actions to be taken when the user interacts with the link.[br]
		<escape_command> has the two following syntactic forms:[br]
		[b]<escape_command> ::= <user_defined_commands>[/b][br]
		[b]<escape_command> ::= <builtin_link_description>[/b]

		[big]User defined links[/big][br]
		The user defined links allow you to perform arbitrary commands when the user interacts with the link.[br]
		The commands are specified in the <escape_command> part by using the following syntax:[br]
		<escape_command> ::= <user_defined_commands>[br]
		<user_defined_commands> ::= <command_rule> [<user_defined_commands>][br]
		<command_rule> ::= <action_tag><command>[br]
		<action_tag> ::= "[!" <action> "]"[br]
		<action> ::= "rbt" | "mbt" | "dbl" | "txt"[br]
		<command> ::= any kvirc command (see notes below)[br]
		
		[big]Builtin links[/big][br]
		The builtin links have builtin actions performed when the user interact with the link.[br]
		These links are used internally in KVIrc , but you can use them too.[br]
		The <escape_command> is a single letter this time: it defines the type of the link.[br]
		Currently KVIrc uses six types of builtin links : 'n' for nickname links, 'u' for url links,
		'c' for channel links, 'h' for hostname links, 'm' for mask links and 's' for server links.[br]
		Theoretically you can also use your own link types: just use any other letter or digit (you can't use ']' and <cr>),
		but probably you will prefer a completely user defined link in that case anyway.[br]
		Once the user interacts with the link , kvirc executes the predefined events:[br]
		On right-click the event OnBuiltinLinkRightClicked is triggered: the first parameter is the link type,
		the second parameter is the <visible text> (as a single parameter!!!)[br]
		On middle-click the event OnBuiltinLinkMiddleClicked is triggered: the parameters are similar to the previous one.[br]
		In the same way you have OnBuiltinLinkDoubleClicked.[br]

		[big]A shortcut[/big]
		You may have a look at the [fnc]$fmtlink[/fnc] function: it does automatically some of the job explained
		in this document.[br]
		
*/

// FIXME: #warning "Finish the doc above!! Maybe some examples ?!"


void KviIrcView::mouseDoubleClickEvent(QMouseEvent *e)
{
	QString cmd;
	QString linkCmd;
	QString linkText;

	if(m_iMouseTimer) 
	{
		killTimer(m_iMouseTimer);
		m_iMouseTimer=0;
		delete m_pLastEvent;
		m_pLastEvent = 0;
	}
	
	getLinkUnderMouse(e->pos().x(),e->pos().y(),0,&linkCmd,&linkText);

	if(linkCmd.isEmpty())
	{
		KVS_TRIGGER_EVENT_0(KviEvent_OnTextViewDoubleClicked,m_pKviWindow);
		return;
	}
	
	QString szCmd(linkCmd);
	szCmd.remove(0,1);

	KviKvsVariantList * pParams = new KviKvsVariantList();
	if(!szCmd.isEmpty()) pParams->append(szCmd);
	else pParams->append(linkText);
	pParams->append(linkText);
	pParams->append(szCmd);
	

	switch(linkCmd[0].unicode())
	{
		case 'n':
		{
			bool bTrigger = false;
			switch(m_pKviWindow->type())
			{
				case KVI_WINDOW_TYPE_CHANNEL:
					if(((KviChannel *)m_pKviWindow)->isOn(linkText))
					{
						KVS_TRIGGER_EVENT(KviEvent_OnChannelNickDefaultActionRequest,m_pKviWindow,pParams);
					} else bTrigger = true;
				break;
				case KVI_WINDOW_TYPE_QUERY:
					if(KviQString::equalCI(((KviQuery *)m_pKviWindow)->windowName(),linkText))
					{
						KVS_TRIGGER_EVENT(KviEvent_OnQueryNickDefaultActionRequest,m_pKviWindow,pParams);
					} else bTrigger = true;
				break;
				default:
					bTrigger = true;
				break;
			}
			if(bTrigger)
			{
				if(console())
				{
					KVS_TRIGGER_EVENT(KviEvent_OnNickLinkDefaultActionRequest,m_pKviWindow,pParams);
				}
			}
		}
		break;
		case 'm':
			if((linkCmd.length() > 2) && (m_pKviWindow->type() == KVI_WINDOW_TYPE_CHANNEL))
			{
				if(((KviChannel *)m_pKviWindow)->isMeOp())
				{
					QChar plmn = linkCmd[1];
					if((plmn.unicode() == '+') || (plmn.unicode() == '-'))
					{
						QString target(m_pKviWindow->windowName());
						target.replace("\\","\\\\");
						target.replace("\"","\\\"");
						target.replace(";","\\;");
						target.replace("$","\\$");
						target.replace("%","\\%");
						QChar flag = linkCmd[2];
						switch(flag.unicode())
						{
							case 'o':
							case 'v':
								// We can do nothing here...
							break;

							case 'b':
							case 'I':
							case 'e':
							case 'k':
								KviQString::sprintf(cmd,"mode %Q %c%c $0",&target,plmn.latin1(),flag.latin1());
							break;
							default:
								KviQString::sprintf(cmd,"mode %Q %c%c",&target,plmn.latin1(),flag.latin1());
							break;
						}
					}
				}
			}
		break;
		case 'h':
			m_pKviWindow->output(KVI_OUT_HOSTLOOKUP,__tr2qs("Looking up host %Q..."),&linkText);
			cmd = "host -a $0";
		break;
		case 'u':
			{
				QString urlText;
				if(!szCmd.isEmpty()) urlText=szCmd;
				else urlText=linkText;
				if(
					!KviQString::cmpCIN(urlText,"irc://",6) ||
					!KviQString::cmpCIN(urlText,"irc6://",7) ||
					!KviQString::cmpCIN(urlText,"ircs://",7) ||
					!KviQString::cmpCIN(urlText,"ircs6://",8)
					)
				{
					KviIrcUrl::run(urlText,KviIrcUrl::TryCurrentContext | KviIrcUrl::DoNotPartChans, console());
				} else {
					cmd = "openurl $0";
				}
			}
		break;
		case 'c':
			{
				if(console() && console()->connection())
				{
					QString szChan=linkText;
					if(szCmd.length()>0) szChan=szCmd;
					if(KviChannel * c = console()->connection()->findChannel(szChan))
					{
						// FIXME: #warning "Is this ok ?"
						c->raise();
						c->setFocus();
					} else {
						cmd = QString("join %1").arg(szChan);
					}
				}
			}
		break;
		case 's':
			cmd = "motd $0";
		break;
		default:
		{
			getLinkEscapeCommand(cmd,linkCmd,"[!dbl]");
			if(cmd.isEmpty())
			{
				KVS_TRIGGER_EVENT_0(KviEvent_OnTextViewDoubleClicked,m_pKviWindow);
			}
		}
		break;
	}
	if(!cmd.isEmpty())
	{
		KviKvsScript::run(cmd,m_pKviWindow,pParams);
	}
	delete pParams;
}

void KviIrcView::mousePressEvent(QMouseEvent *e)
{
	if(m_pKviWindow->input()) m_pKviWindow->input()->setFocus();
	
	if(e->button() & Qt::LeftButton)
	{
		// This is the beginning of a selection...
		// We just set the mouse to be "down" and
		// await mouseMove events...

		if(m_pToolWidget)
		{
			m_pCursorLine = getVisibleLineAt(e->pos().x(),e->pos().y());
			repaint();
		}

		m_mousePressPos   = e->pos();
		m_mouseCurrentPos = e->pos();

		m_bMouseIsDown = true;

		m_bShiftPressed = (e->state() & Qt::ShiftButton);

		calculateSelectionBounds();
	}
	
	if(e->button() & Qt::LeftButton)
	{
		if(m_iMouseTimer) 
		{
			killTimer(m_iMouseTimer);
			m_iMouseTimer=0;
			delete m_pLastEvent;
			m_pLastEvent = 0;
		} else {
			m_iMouseTimer = startTimer(QApplication::doubleClickInterval());
			m_pLastEvent = new QMouseEvent(*e);
		}
	} else {
		mouseRealPressEvent(e);
	}
}

void KviIrcView::mouseRealPressEvent(QMouseEvent *e)
{
	QString linkCmd;
	QString linkText;
	getLinkUnderMouse(e->pos().x(),e->pos().y(),0,&linkCmd,&linkText);
	
	QString szCmd(linkCmd);
	szCmd.remove(0,1);

	KviKvsVariantList * pParams = new KviKvsVariantList();
	if(!szCmd.isEmpty()) pParams->append(szCmd);
	else pParams->append(linkText);
	pParams->append(linkText);
	pParams->append(szCmd);
	

	if(!(e->state() & Qt::ControlButton))//(e->button() & Qt::RightButton) && (
	{
		if(!linkCmd.isEmpty())
		{
			switch(linkCmd[0].unicode())
			{
				case 'n':
					{
						bool bTrigger = false;
						switch(m_pKviWindow->type())
						{
							case KVI_WINDOW_TYPE_CHANNEL:
								if(((KviChannel *)m_pKviWindow)->isOn(linkText))
								{
									if(e->button() & Qt::RightButton)
										KVS_TRIGGER_EVENT(KviEvent_OnChannelNickPopupRequest,m_pKviWindow,pParams);
									if(e->button() & Qt::LeftButton) {
										KVS_TRIGGER_EVENT(KviEvent_OnChannelNickLinkClick,m_pKviWindow,pParams);
										if(m_pKviWindow)
										{
											if(m_pKviWindow->inherits("KviChannel")) {
												KviChannel *c = (KviChannel*)m_pKviWindow;
												QString nick;
												if(pParams->firstAsString(nick))
													c->userListView()->select(nick);
											}
										}
									}
								} else bTrigger = true;
							break;
							case KVI_WINDOW_TYPE_QUERY:
								if(KviQString::equalCI(((KviQuery *)m_pKviWindow)->windowName(),linkText))
								{
									if(e->button() & Qt::RightButton)
										KVS_TRIGGER_EVENT(KviEvent_OnQueryNickPopupRequest,m_pKviWindow,pParams);
									if(e->button() & Qt::LeftButton)
										KVS_TRIGGER_EVENT(KviEvent_OnQueryNickLinkClick,m_pKviWindow,pParams);
								} else bTrigger = true;
							break;
							default:
								bTrigger = true;
						break;
						}
						if(bTrigger)
						{
							if(console())
							{
								if(e->button() & Qt::RightButton)
									KVS_TRIGGER_EVENT(KviEvent_OnNickLinkPopupRequest,m_pKviWindow,pParams);
								if(e->button() & Qt::LeftButton)
									KVS_TRIGGER_EVENT(KviEvent_OnConsoleNickLinkClick,m_pKviWindow,pParams);
							} else emit rightClicked();
						}
					}
				break;
				case 'h':
					if(e->button() & Qt::RightButton)
						KVS_TRIGGER_EVENT(KviEvent_OnHostLinkPopupRequest,m_pKviWindow,pParams);
					if(e->button() & Qt::LeftButton)
						KVS_TRIGGER_EVENT(KviEvent_OnHostLinkClick,m_pKviWindow,pParams);
				break;
				case 'u':
					if(e->button() & Qt::RightButton)
						KVS_TRIGGER_EVENT(KviEvent_OnUrlLinkPopupRequest,m_pKviWindow,pParams);
					if(e->button() & Qt::LeftButton)
						KVS_TRIGGER_EVENT(KviEvent_OnUrlLinkClick,m_pKviWindow,pParams);
				break;
				case 'c':
					if(e->button() & Qt::RightButton)
						KVS_TRIGGER_EVENT(KviEvent_OnChannelLinkPopupRequest,m_pKviWindow,pParams);
					if(e->button() & Qt::LeftButton)
						KVS_TRIGGER_EVENT(KviEvent_OnChannelLinkClick,m_pKviWindow,pParams);
				break;
				case 's':
					if(e->button() & Qt::RightButton)
						KVS_TRIGGER_EVENT(KviEvent_OnServerLinkPopupRequest,m_pKviWindow,pParams);
					if(e->button() & Qt::LeftButton)
						KVS_TRIGGER_EVENT(KviEvent_OnServerLinkClick,m_pKviWindow,pParams);
				break;
				default:
				{
					if(e->button() & Qt::RightButton)
					{
						QString tmp;
						getLinkEscapeCommand(tmp,linkCmd,"[!rbt]");
						if(!tmp.isEmpty())
						{
							KviKvsScript::run(tmp,m_pKviWindow,pParams);
						} else emit rightClicked();
					}
				}
				break;
			}
		} else if(e->button() & Qt::RightButton) emit rightClicked();
		
	} else if((e->button() & Qt::MidButton) || ((e->button() & Qt::RightButton) && (e->state() & Qt::ControlButton)))
	{
		QString tmp;
		getLinkEscapeCommand(tmp,linkCmd,QString("[!mbt]"));
		if(!tmp.isEmpty())
		{
			KviKvsScript::run(tmp,m_pKviWindow,pParams);
		} else {
			KVS_TRIGGER_EVENT_0(KviEvent_OnWindowPopupRequest,m_pKviWindow);
		}
	}
	delete pParams;
}

//================ mouseReleaseEvent ===============//

void KviIrcView::mouseReleaseEvent(QMouseEvent *)
{
	if(m_iSelectTimer)
	{
		killTimer(m_iSelectTimer);
		m_iSelectTimer = 0;
		QClipboard * c = QApplication::clipboard();
		if(c)
		{
			// copy to both!
			c->setText(m_szLastSelection,QClipboard::Clipboard);
			if(c->supportsSelection())
				c->setText(m_szLastSelection,QClipboard::Selection);
		}
	}

	if(m_bMouseIsDown)
	{
		m_bMouseIsDown = false;
		m_bShiftPressed = false;
		// Insert the lines blocked while selecting
		while(KviIrcViewLine * l = m_pMessagesStoppedWhileSelecting->first())
		{
			m_pMessagesStoppedWhileSelecting->removeFirst();
			appendLine(l,false);
		}
		repaint();
	}
}

// FIXME: #warning "The tooltip timeout should be small, because the view scrolls!"

void KviIrcView::mouseMoveEvent(QMouseEvent *e)
{
//	debug("Pos : %d,%d",e->pos().x(),e->pos().y());
	if(m_bMouseIsDown && (e->state() & Qt::LeftButton)) // m_bMouseIsDown MUST BE true...(otherwise the mouse entered the window with the button pressed ?)
	{
		
		if(m_iSelectTimer == 0)m_iSelectTimer = startTimer(KVI_IRCVIEW_SELECT_REPAINT_INTERVAL);
		
		/*if(m_iMouseTimer)
		{
			killTimer(m_iMouseTimer);
			m_iMouseTimer=0;
			mouseRealPressEvent(m_pLastEvent);
			delete m_pLastEvent;
			m_pLastEvent=0;	
		}*/
	} else {
		if(m_iSelectTimer)
		{
			killTimer(m_iSelectTimer);
			m_iSelectTimer = 0;
		}

		int yPos = e->pos().y();
		int rectTop;
		int rectHeight;
		QRect rctLink;
		KviIrcViewWrappedBlock * newLinkUnderMouse = getLinkUnderMouse(e->pos().x(),yPos,&rctLink);

		rectTop = rctLink.y();
		rectHeight = rctLink.height();

		if(newLinkUnderMouse != m_pLastLinkUnderMouse)
		{
			//abortTip();
			//m_iTipTimer = startTimer(KVI_OPTION_UINT(KviOption_uintIrcViewToolTipTimeoutInMsec));
			m_pLastLinkUnderMouse = newLinkUnderMouse;
			if(m_pLastLinkUnderMouse)
			{
				setCursor(Qt::PointingHandCursor);
				if(rectTop < 0)rectTop = 0;
				if((rectTop + rectHeight) > height())rectHeight = height() - rectTop;

				if(m_iLastLinkRectHeight > -1)
				{
					// prev link
					int top = (rectTop < m_iLastLinkRectTop) ? rectTop : m_iLastLinkRectTop;
					int lastBottom = m_iLastLinkRectTop + m_iLastLinkRectHeight;
					int thisBottom = rectTop + rectHeight;
					QRect r(0,top,width(),((lastBottom > thisBottom) ? lastBottom : thisBottom) - top);
					repaint(r);
				} else {
					// no prev link
					QRect r(0,rectTop,width(),rectHeight);
					repaint(r);
				}
				m_iLastLinkRectTop = rectTop;
				m_iLastLinkRectHeight = rectHeight;
			} else {
				setCursor(Qt::ArrowCursor);
				if(m_iLastLinkRectHeight > -1)
				{
					// There was a previous bottom rect
					QRect r(0,m_iLastLinkRectTop,width(),m_iLastLinkRectHeight);
					repaint(r);
					m_iLastLinkRectTop = -1;
					m_iLastLinkRectHeight = -1;
				}
			}

		}
	}
}

KviConsole * KviIrcView::console()
{
	return m_pKviWindow->console();
}

void KviIrcView::doLinkToolTip(const QRect &rct,QString &linkCmd,QString &linkText)
{
	if(linkCmd.isEmpty())return;
	
	QString szCmd(linkCmd);
	szCmd.remove(0,1);
	
	QString tip;

	switch(linkCmd[0].unicode())
	{
		case 'u': // url link
		{
			tip = "<table width=\"100%\">" \
				"<tr><td valign=\"center\"><img src=\"url_icon\"> <u><font color=\"blue\"><nowrap>";
			if(linkText.length() > 50)
			{
				tip += linkText.left(47);
				tip += "...";
			} else {
				tip += linkText;
			}
			tip+="</nowrap></font></u></td></tr><tr><td>";
			QMimeSourceFactory::defaultFactory()->setPixmap("url_icon",*(g_pIconManager->getSmallIcon(KVI_SMALLICON_URL)));
			tip += __tr2qs("Double-click to open this link");
			tip += "</td></tr></table>";
		}
		break;
		case 'h': // host link
		{
			tip = "<table width=\"100%\">" \
				"<tr><td valign=\"center\"><img src=\"host_icon\"> <u><font color=\"blue\"><nowrap>";
			if(linkText.length() > 50)
			{
				tip += linkText.left(47);
				tip += "...";
			} else {
				tip += linkText;
			}
			tip+="</nowrap></font></u></td></tr><tr><td>";
			QMimeSourceFactory::defaultFactory()->setPixmap("host_icon",*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SERVER)));
			
			if(linkText.find('*') != -1)
			{
				if(linkText.length() > 1)tip += __tr2qs("Unable to look it up hostname: Hostname appears to be masked");
				else tip += __tr2qs("Unable to look it up hostname: Unknown host");
			} else {
				tip += __tr2qs("Double-click to look up this hostname<br>Right-click to view other options");
			}
			tip += "</td></tr></table>";
		}
		break;
		case 's': // server link
		{
			// FIXME: #warning "Spit out some server info...hub ?...registered ?"
			
			tip = "<table width=\"100%\">" \
				"<tr><td valign=\"center\"><img src=\"server_icon\"> <u><font color=\"blue\"><nowrap>";
			QMimeSourceFactory::defaultFactory()->setPixmap("server_icon",*(g_pIconManager->getSmallIcon(KVI_SMALLICON_IRC)));
			if(linkText.length() > 50)
			{
				tip += linkText.left(47);
				tip += "...";
			} else {
				tip += linkText;
			}
			tip+="</nowrap></font></u></td></tr><tr><td>";
			
			if(linkText.find('*') != -1)
			{
				if(linkText.length() > 1)tip += __tr2qs("Server appears to be a network hub<br>");
				else tip += __tr2qs("Unknown server<br>"); // might happen...
			}

			tip.append(__tr2qs("Double-click to read the MOTD<br>Right-click to view other options"));
			tip += "</td></tr></table>";
		}
		break;
		case 'm': // mode link
		{
			if((linkCmd.length() > 2) && (m_pKviWindow->type() == KVI_WINDOW_TYPE_CHANNEL))
			{
				if(((KviChannel *)m_pKviWindow)->isMeOp())
				{
					QChar plmn = linkCmd[1];
					if((plmn.unicode() == '+') || (plmn.unicode() == '-'))
					{
						tip = __tr2qs("Double-click to set<br>");
						QChar flag = linkCmd[2];
						switch(flag.unicode())
						{
							case 'o':
							case 'v':
								// We can do nothing here...
								tip = "";
							break;
							case 'b':
							case 'I':
							case 'e':
							case 'k':
								KviQString::appendFormatted(tip,QString("<b>mode %Q %c%c %Q</b>"),&(m_pKviWindow->windowName()),plmn.latin1(),flag.latin1(),&linkText);
							break;
							default:
								KviQString::appendFormatted(tip,QString("<b>mode %Q %c%c</b>"),&(m_pKviWindow->windowName()),plmn.latin1(),flag.latin1());
							break;
						}
					}
				} else {
					// I'm not op...no way
					tip = __tr2qs("You're not an operator: You may not change channel modes");
				}
			}
		}
		break;
		case 'n': // nick link
		{
			if(console())
			{
				if(console()->connection())
				{
					KviIrcUserEntry * e = console()->connection()->userDataBase()->find(linkText);
					if(e)
					{
						QString buffer;
						console()->getUserTipText(linkText,e,buffer);
						tip = buffer;
					} else KviQString::sprintf(tip,__tr2qs("Nothing known about %Q"),&linkText);
				} else KviQString::sprintf(tip,__tr2qs("Nothing known about %Q (no connection)"),&linkText);
			}
		}
		break;
		case 'c': // channel link
		{
			if(console() && console()->connection())
			{
				QString szChan = linkText;
				QString buf;
				tip = "<img src=\"chan_icon\"> ";
				QMimeSourceFactory::defaultFactory()->setPixmap("chan_icon",*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CHANNEL)));
				
				if(szCmd.length()>0) szChan=szCmd;
				KviChannel * c = console()->connection()->findChannel(szChan);
				QString szUrl;
				if(c)
				{
					QString chanMode;
					c->getChannelModeString(chanMode);
					QString topic = KviMircCntrl::stripControlBytes(c->topicWidget()->topic());
					topic.replace("<","&lt;");
					topic.replace(">","&gt;");
					KviIrcUrl::join(szUrl,console()->connection()->target()->server());
					szUrl.append(szChan);
					KviQString::sprintf(buf,__tr2qs("<b>%Q</b> (<u><font color=\"blue\"><nowrap>"
						"%Q</nowrap></font></u>): <br><nowrap>+%Q (%u users)<hr>%Q</nowrap>"),&szChan,&szUrl,&chanMode,
						c->count(),&topic);
				} else {
					KviIrcUrl::join(szUrl,console()->connection()->target()->server());
					szUrl.append(szChan);
					KviQString::sprintf(buf,__tr2qs("<b>%Q</b> (<u><font color=\"blue\"><nowrap>"
						"%Q</nowrap></font></u>)<hr>Double-click to join %Q<br>Right click to view other options"),&szChan,&szUrl,&szChan);
				}
				
				tip += buf;
			}
		}
		break;
		default:
		{
			QString dbl,rbt,txt,mbt;
			getLinkEscapeCommand(dbl,linkCmd,"[!dbl]");
			getLinkEscapeCommand(rbt,linkCmd,"[!rbt]");
			getLinkEscapeCommand(txt,linkCmd,"[!txt]");
			getLinkEscapeCommand(mbt,linkCmd,"[!mbt]");

			if(!txt.isEmpty())tip = txt;
			if(tip.isEmpty() && (!dbl.isEmpty()))
			{
				if(!tip.isEmpty())tip.append("<hr>");
				KviQString::appendFormatted(tip,__tr2qs("<b>Double-click:</b><br>%Q"),&dbl);
			}
			if(tip.isEmpty() && (!mbt.isEmpty()))
			{
				if(!tip.isEmpty())tip.append("<hr>");
				KviQString::appendFormatted(tip,__tr2qs("<b>Middle-click:</b><br>%Q"),&mbt);
			}
			if(tip.isEmpty() && (!rbt.isEmpty()))
			{
				if(!tip.isEmpty())tip.append("<hr>");
				KviQString::appendFormatted(tip,__tr2qs("<b>Right-click:</b><br>%Q"),&rbt);
			}
		}
		break;
	}

	if(tip.isEmpty())return;

	m_pToolTip->doTip(rct,tip);
}
void KviIrcView::leaveEvent ( QEvent * ) 
{ 
	if(m_pLastLinkUnderMouse)  
    {
		 m_pLastLinkUnderMouse=0;
		 update(); 
	}  
}
void KviIrcView::timerEvent(QTimerEvent *e)
{
	m_mouseCurrentPos = mapFromGlobal(QCursor::pos());

	if(e->timerId() == m_iSelectTimer)
	{
		calculateSelectionBounds();
		repaint();
	}
	if(e->timerId() == m_iMouseTimer)
	{
		killTimer(m_iMouseTimer);
		m_iMouseTimer=0;
		mouseRealPressEvent(m_pLastEvent);
		delete m_pLastEvent;
		m_pLastEvent=0;	
	}
	if(e->timerId() == m_iFlushTimer)
	{
		flushLog();
	}
}

void KviIrcView::keyPressEvent(QKeyEvent *e)
{
	switch(e->key())
	{
		case Qt::Key_PageUp:
			prevPage();
			e->accept();
			break;
		case Qt::Key_PageDown:
			nextPage();
			e->accept();
			break;
		default:
			e->ignore();
	}
}

void KviIrcView::maybeTip(const QPoint &pnt)
{
	QString linkCmd;
	QString linkText;

	QRect rctLink;

	KviIrcViewWrappedBlock * linkUnderMouse = getLinkUnderMouse(pnt.x(),pnt.y(),&rctLink,&linkCmd,&linkText);

	if((linkUnderMouse == m_pLastLinkUnderMouse) && linkUnderMouse)doLinkToolTip(rctLink,linkCmd,linkText);
	else m_pLastLinkUnderMouse = 0; //
}

#include "kvi_ircview.moc"
