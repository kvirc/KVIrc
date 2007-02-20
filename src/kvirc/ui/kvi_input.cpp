//=============================================================================
//
//   File : kvi_input.cpp
//   Creation date : Sun Jan 3 1999 23:11:50 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2007 Szymon Stefanek (pragma at kvirc dot net)
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
#define _KVI_DEBUG_CHECK_RANGE_
#include "kvi_debug.h"

#define _KVI_INPUT_CPP_

#include "kvi_options.h"
#include "kvi_app.h"
#include "kvi_settings.h"
#include "kvi_defaults.h"
#include "kvi_colorwin.h"
#include "kvi_texticonwin.h"
#include "kvi_window.h"

#include "kvi_locale.h"
#include "kvi_mirccntrl.h"
#include "kvi_userlistview.h"
#include "kvi_ircview.h"
#include "kvi_console.h"
#include "kvi_out.h"
#include "kvi_iconmanager.h"
#include "kvi_scripteditor.h"
#include "kvi_config.h"
#include "kvi_historywin.h"
#include "kvi_input.h"
#include "kvi_userinput.h"
#include "kvi_kvs_script.h"
#include "kvi_kvs_kernel.h"
#include "kvi_doublebuffer.h"
#include "kvi_styled_controls.h"
#include "kvi_texticonmanager.h"
#include "kvi_draganddrop.h"

#include <qlabel.h>
#include <ctype.h>
#include <stdlib.h>
#include <qfiledialog.h>
#include "kvi_tal_popupmenu.h"
#include <qpainter.h>
#include <qclipboard.h>
#include <qstringlist.h>
#include "kvi_list.h"
#include <qapplication.h>
#include <qclipboard.h>
#include <qmessagebox.h>
#include "kvi_tal_hbox.h"
#include <qlayout.h> 
#include <qstyle.h>
#include <qevent.h>


#ifndef ACCEL_KEY
#define ACCEL_KEY(k) "\t" + QString(QKeySequence( Qt::CTRL | Qt::Key_ ## k ))
#endif

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



//This comes from kvi_app.cpp
extern KviColorWindow    * g_pColorWindow;
extern KviTextIconWindow * g_pTextIconWindow;
extern KviHistoryWindow  * g_pHistoryWindow;
extern KviTalPopupMenu        * g_pInputPopup;


#ifdef COMPILE_PSEUDO_TRANSPARENCY
	extern QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif


#define KVI_INPUT_MAX_GLOBAL_HISTORY_ENTRIES 100
#define KVI_INPUT_MAX_LOCAL_HISTORY_ENTRIES 20


extern KviInputHistory * g_pInputHistory;


KviInputHistory::KviInputHistory()
{
	m_pStringList = new KviPtrList<QString>;
	m_pStringList->setAutoDelete(true);
}

KviInputHistory::~KviInputHistory()
{
	delete m_pStringList;
}

void KviInputHistory::add(QString * s)
{
	m_pStringList->insert(0,s);
	if(m_pStringList->count() > KVI_INPUT_MAX_GLOBAL_HISTORY_ENTRIES)m_pStringList->removeLast();
}

void KviInputHistory::load(const char * filename)
{
	KviConfig c(filename,KviConfig::Read);

	int cnt = c.readIntEntry("Count",0);

	if(cnt > KVI_INPUT_MAX_GLOBAL_HISTORY_ENTRIES)cnt = KVI_INPUT_MAX_GLOBAL_HISTORY_ENTRIES;

	KviStr tmp;

	for(int i=0;i<cnt;i++)
	{
		tmp.sprintf("S%d",i);
		QString entry = c.readQStringEntry(tmp.ptr(),"");
		if(!entry.isEmpty())add(new QString(entry));
	}
}

void KviInputHistory::save(const char * filename)
{
	KviConfig c(filename,KviConfig::Write);
	c.clear();

	c.writeEntry("Count",m_pStringList->count());

	KviStr tmp;
	int idx = 0;

	for(QString * s = m_pStringList->first();s;s = m_pStringList->next())
	{
		if(!s->isEmpty())
		{
			tmp.sprintf("S%d",idx);
			c.writeEntry(tmp.ptr(),*s);
			idx++;
		}
	}
}

//=============== KviInputEditor ==============//

static int g_iInputFontCharWidth[256];
static bool g_bInputFontMetricsDirty = true;


KviInputEditor::KviInputEditor(QWidget * par,KviWindow *wnd,KviUserListView * view)
:QFrame(par,"input")
{
	m_pIconMenu            = 0;
	m_pInputParent         = par;
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
	m_iLastCursorXPosition = KVI_INPUT_MARGIN;              //Calculated in paintEvent
	m_iSelectionAnchorChar = -1;                            //Character clicked at the beginning of the selection process
	m_iCurHistoryIdx       = -1;                            //No data in the history
	m_bUpdatesEnabled      = true;
	m_pKviWindow           = wnd;
	m_pUserListView        = view;
	m_pHistory             = new KviPtrList<QString>;
	m_pHistory->setAutoDelete(true);
	m_bReadOnly = FALSE;
	
	setInputMethodEnabled(true);

#ifdef COMPILE_USE_QT4
	setAutoFillBackground(false);
	setFocusPolicy(Qt::StrongFocus);
#else
	setBackgroundMode(Qt::NoBackground);
	setFocusPolicy(QWidget::StrongFocus);
#endif
	setAcceptDrops(true);
	setFrameStyle( LineEditPanel );
	setFrameShadow( Plain );
	
	m_pIconMenu = new KviTalPopupMenu();
	connect(m_pIconMenu,SIGNAL(activated(int)),this,SLOT(iconPopupActivated(int)));

#ifdef COMPILE_USE_QT4
	setCursor(Qt::IBeamCursor);
#else
	setCursor(IbeamCursor);
#endif
}

KviInputEditor::~KviInputEditor()
{
	if(m_pIconMenu)delete m_pIconMenu;
	delete m_pHistory;
	if(m_iCursorTimer)killTimer(m_iCursorTimer);
	killDragTimer();
}

void KviInputEditor::recalcFontMetrics(QFontMetrics * pFm)
{
	QFontMetrics fm(KVI_OPTION_FONT(KviOption_fontInput));
	unsigned short i;
	for(i=1;i<32;i++)
	{
		QChar c = getSubstituteChar(i);
		g_iInputFontCharWidth[i] = fm.width(c);
		if(c != QChar(i))g_iInputFontCharWidth[i] += 4;
	}
	for(i=32;i<256;i++)
	{
		g_iInputFontCharWidth[i] = fm.width(QChar(i));
	}
	g_bInputFontMetricsDirty = false;
}

void KviInputEditor::applyOptions()
{
	g_bInputFontMetricsDirty = true;
	update();
}

void KviInputEditor::dragEnterEvent(QDragEnterEvent *e)
{
	if(KviUriDrag::canDecode(e))
	{
		e->accept(true);
// FIXME: #warning "FIX THIS COMMENTED STUFF"
/*
		m_pKviWindow->m_pFrm->m_pStatusBar->tempText(__tr("Drop the file to /PARSE it"),5000);
*/
	} else e->accept(false);
}

void KviInputEditor::dropEvent(QDropEvent *e)
{
	QStringList list;
	if(KviUriDrag::decodeLocalFiles(e,list))
	{
		//debug("Local files decoded");
		if(!list.isEmpty())
		{
			//debug("List not empty");
			QStringList::ConstIterator it = list.begin(); //kewl ! :)
    		for( ; it != list.end(); ++it )
			{
				QString tmp = *it; //wow :)
#ifndef COMPILE_ON_WINDOWS
				if(tmp.length() > 0)
				{
					if(tmp[0] != QChar('/'))tmp.prepend("/"); //HACK HACK HACK for Qt bug (?!?)
				}
#endif
				tmp.prepend("/PARSE \"");
				tmp.append("\"");
				if(m_pKviWindow)
					KviKvsScript::run(tmp,m_pKviWindow);
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
	//grabbed from qlineedit.cpp
	constPolish();
	QFontMetrics fm(KVI_OPTION_FONT(KviOption_fontInput));
	int h = QMAX(fm.lineSpacing(), 14) + 2*2; /* innerMargin */
	int w = fm.width( 'x' ) * 17; // "some"
	int m = frameWidth() * 2;
#ifdef COMPILE_USE_QT4
	QStyleOption opt;
	opt.initFrom(this);
	return (style()->sizeFromContents(QStyle::CT_LineEdit,&opt,
				     QSize( w + m, h + m ).
				     expandedTo(QApplication::globalStrut()),this));
#else
	return (style().sizeFromContents(QStyle::CT_LineEdit, this,
				     QSize( w + m, h + m ).
				     expandedTo(QApplication::globalStrut())));
#endif
}

#define KVI_INPUT_DEF_BACK 100
#define KVI_INPUT_DEF_FORE 101

#ifdef COMPILE_USE_QT4
void KviInputEditor::paintEvent(QPaintEvent *e)
{
	QPainter p(this);
	drawFrame(&p);
	drawContents(&p);
}
#endif

void KviInputEditor::drawContents(QPainter *p)
{
	if(!isVisible())return;

	QRect rect = contentsRect();
	int widgetWidth       = rect.width();
	int widgetHeight      = rect.height();

	KviDoubleBuffer doublebuffer(widgetWidth,widgetHeight);
	QPixmap * pDoubleBufferPixmap = doublebuffer.pixmap();

	QPainter pa(pDoubleBufferPixmap);

	pa.setFont(KVI_OPTION_FONT(KviOption_fontInput));

	QFontMetrics fm(pa.fontMetrics());
	if(g_bInputFontMetricsDirty)
		recalcFontMetrics(&fm);


#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt = mapToGlobal(rect.topLeft());
		pa.drawTiledPixmap(0,0,widgetWidth,widgetHeight,*g_pShadedChildGlobalDesktopBackground,pnt.x(),pnt.y());
	} else {
#endif
		QPixmap *pix=KVI_OPTION_PIXMAP(KviOption_pixmapInputBackground).pixmap();
		
		pa.fillRect(0,0,widgetWidth,widgetHeight,KVI_OPTION_COLOR(KviOption_colorInputBackground));
		if(pix)
			KviPixmapUtils::drawPixmapWithPainter(&pa,pix,KVI_OPTION_UINT(KviOption_uintInputPixmapAlign),rect,widgetWidth,widgetHeight);
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	}
#endif


	int curXPos      = KVI_INPUT_MARGIN;
	int maxXPos      = widgetWidth-2*KVI_INPUT_MARGIN;
	m_iCurBack       = KVI_INPUT_DEF_BACK; //transparent
	m_iCurFore       = KVI_INPUT_DEF_FORE; //normal fore color
	m_bCurBold       = false;
	m_bCurUnderline  = false;

	int bottom       = widgetHeight-(widgetHeight-fm.height())/2;
	int textBaseline = fm.ascent()+(widgetHeight-fm.height())/2;
	int top          = (widgetHeight-fm.height())/2;
	
	runUpToTheFirstVisibleChar();

	int charIdx      = m_iFirstVisibleChar;

	pa.setClipRect(0,0,widgetWidth,widgetHeight);

	//Control the selection state
	if((m_iSelectionEnd < m_iSelectionBegin) || (m_iSelectionEnd == -1) || (m_iSelectionBegin == -1))
	{
		m_iSelectionEnd = -1;
		m_iSelectionBegin = -1;
	}

	if((m_iSelectionBegin != -1) && (m_iSelectionEnd >= m_iFirstVisibleChar))
	{
		int iSelStart = m_iSelectionBegin;

    // TODO Refactor: write a function to combine this with the code determining iIMStart and iIMSelectionStart
		if(iSelStart < m_iFirstVisibleChar)iSelStart = m_iFirstVisibleChar;
		int xLeft = xPositionFromCharIndex(iSelStart,TRUE);
		int xRight = xPositionFromCharIndex(m_iSelectionEnd + 1,TRUE);

//		pa.setRasterOp(Qt::NotROP);
		pa.fillRect(xLeft,frameWidth(),xRight - xLeft,widgetWidth,KVI_OPTION_COLOR(KviOption_colorInputSelectionBackground));
//		pa.setRasterOp(Qt::CopyROP);
	}

	// When m_bIMComposing is true, the text between m_iIMStart and m_iIMStart+m_iIMLength should be highlighted to show that this is the active
	// preedit area for the input method, and the text outside cannot be edited while
	// composing. Maybe this can be implemented similarly as painting the selection?
	// Also notice that inside the preedit, there can also be a selection, given by
	// m_iSelectionBegin and m_iSelectionLength, and the widget needs to highlight that
	// while in IM composition mode
	if(m_bIMComposing && m_iIMLength > 0)
	{
		// TODO Write a function to combine IM selection drawing code. maybe the preedit area too.
		int iIMSelectionStart = m_iIMSelectionBegin;
		if(iIMSelectionStart < m_iFirstVisibleChar) iIMSelectionStart = m_iFirstVisibleChar;
		int xIMSelectionLeft = xPositionFromCharIndex(iIMSelectionStart,TRUE);
		int xIMSelectionRight = xPositionFromCharIndex(iIMSelectionStart + m_iIMSelectionLength,TRUE);
//		pa.setRasterOp(Qt::NotROP);
		pa.fillRect(xIMSelectionLeft,0,xIMSelectionRight - xIMSelectionLeft, widgetWidth,KVI_OPTION_COLOR(KviOption_colorInputSelectionBackground));
//		pa.setRasterOp(Qt::CopyROP);
 
		// highlight the IM selection
		int iIMStart = m_iIMStart;
		if(m_iIMStart < m_iFirstVisibleChar) m_iIMStart = m_iFirstVisibleChar;
		int xIMLeft = xPositionFromCharIndex(iIMStart,TRUE);
		int xIMRight = xPositionFromCharIndex(iIMStart + m_iIMLength,TRUE);

		// underline the IM preedit
		// Maybe should be put in drawTextBlock, similar to drawing underlined text
		pa.drawLine(xIMLeft, bottom, xIMRight, bottom);
	}

	pa.setClipping(false);

	while((charIdx < ((int)(m_szTextBuffer.length()))) && (curXPos < maxXPos))
	{
		extractNextBlock(charIdx,fm,curXPos,maxXPos);

		if(m_bControlBlock)
		{
			pa.setPen(KVI_OPTION_COLOR(KviOption_colorInputControl));

			QString s = getSubstituteChar(m_szTextBuffer[charIdx].unicode());

			// the block width is 4 pixels more than the actual character

			pa.drawText(curXPos + 2,textBaseline,s,1);

			pa.drawLine(curXPos,top,curXPos+m_iBlockWidth-1,top);
			pa.drawLine(curXPos,top,curXPos,bottom);
			pa.drawLine(curXPos,bottom,curXPos+m_iBlockWidth,bottom);
			pa.drawLine(curXPos+m_iBlockWidth-1,top,curXPos+m_iBlockWidth-1,bottom);
		} else {
			if(m_iSelectionBegin!=-1)
			{
				int iBlockEnd=charIdx+m_iBlockLen;
				//block is selected (maybe partially)
				if( iBlockEnd>m_iSelectionBegin && charIdx<=m_iSelectionEnd )
				{
					int iSubStart,iSubLen;
					//in common it consists of 3 parts: unselected-selected-unselected
					//some of thst parts can be empty (for example block is fully selected)
					
					//first part start is always equal to the block start
					iSubStart=charIdx;
					iSubLen = m_iSelectionBegin>charIdx ? m_iSelectionBegin-charIdx : 0;
					
				
					if(iSubLen)
					{
						drawTextBlock(&pa,fm,curXPos,textBaseline,iSubStart,iSubLen,FALSE);
						curXPos += m_iBlockWidth;
						m_iBlockWidth=0;
					}
					
					//second one
					iSubStart+=iSubLen;
					iSubLen=m_iSelectionEnd<iBlockEnd ? m_iSelectionEnd-iSubStart+1 : iBlockEnd-iSubStart;
					
					
					if(iSubLen)
					{
						drawTextBlock(&pa,fm,curXPos,textBaseline,iSubStart,iSubLen,TRUE);
						curXPos += m_iBlockWidth;
						m_iBlockWidth=0;
					}
					
					if( m_iSelectionEnd<(iBlockEnd-1))
					{
						iSubStart+=iSubLen;
						iSubLen=iBlockEnd-iSubStart;
						drawTextBlock(&pa,fm,curXPos,textBaseline,iSubStart,iSubLen,FALSE);
					}
				} else {
					drawTextBlock(&pa,fm,curXPos,textBaseline,charIdx,m_iBlockLen);
				}
			} else {
				drawTextBlock(&pa,fm,curXPos,textBaseline,charIdx,m_iBlockLen);
			}
		}

		curXPos += m_iBlockWidth;
		charIdx += m_iBlockLen;
	}
	
	//Now the cursor

	m_iLastCursorXPosition = KVI_INPUT_MARGIN;
	m_iBlockLen = m_iFirstVisibleChar;

	while(m_iBlockLen < m_iCursorPosition)
	{
		QChar c = m_szTextBuffer.at(m_iBlockLen);
#ifdef COMPILE_USE_QT4
		m_iLastCursorXPosition+= fm.width(c);
#else
		m_iLastCursorXPosition+= (c.unicode() < 256) ? g_iInputFontCharWidth[c.unicode()] : fm.width(c);
#endif
		m_iBlockLen++;
	}

//	m_iLastCursorXPosition = xPositionFromCharIndex(m_iCursorPosition);

	if(m_bCursorOn)
	{
		pa.setPen(KVI_OPTION_COLOR(KviOption_colorInputCursor));
		pa.drawLine(m_iLastCursorXPosition,0,m_iLastCursorXPosition,widgetHeight);
	} else {
		pa.setPen(KVI_OPTION_COLOR(KviOption_colorInputForeground));
	}

#ifdef COMPILE_USE_QT4
	// The other version of drawPixmap seems to be buggy
	p->drawPixmap(rect.x(),rect.y(),rect.width(),rect.height(),*pDoubleBufferPixmap,0,0,widgetWidth,widgetHeight);
#else
	p->drawPixmap(rect.x(),rect.y(),*pDoubleBufferPixmap,0,0,widgetWidth,widgetHeight);
#endif
}

void KviInputEditor::drawTextBlock(QPainter * pa,QFontMetrics & fm,int curXPos,int textBaseline,int charIdx,int len,bool bSelected)
{
	QString tmp = m_szTextBuffer.mid(charIdx,len);
	m_iBlockWidth = fm.width(tmp);
	
	QRect rect = contentsRect();
	int widgetHeight      = rect.height();
	
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
			pa->fillRect(curXPos,(widgetHeight-fm.height())/2,m_iBlockWidth,fm.height(),KVI_OPTION_COLOR(KviOption_colorInputForeground));
		} else {
			pa->fillRect(curXPos,(widgetHeight-fm.height())/2,m_iBlockWidth,fm.height(),KVI_OPTION_MIRCCOLOR((unsigned char)m_iCurBack));
		}
	}

	pa->drawText(curXPos,textBaseline,tmp);

	if(m_bCurBold)pa->drawText(curXPos+1,textBaseline,tmp);
	if(m_bCurUnderline)
	{
		pa->drawLine(curXPos,textBaseline + fm.descent(),curXPos+m_iBlockWidth,textBaseline + fm.descent());
	}
	
}

QChar KviInputEditor::getSubstituteChar(unsigned short control_code)
{
	switch(control_code)
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
			return QChar(control_code);
			break;
	}
}

void KviInputEditor::extractNextBlock(int idx,QFontMetrics & fm,int curXPos,int maxXPos)
{
	m_iBlockLen = 0;
	m_iBlockWidth = 0;

	QChar c = m_szTextBuffer[idx];

	if((c.unicode() > 32) ||
		((c != QChar(KVI_TEXT_COLOR)) &&
		(c != QChar(KVI_TEXT_BOLD)) && (c != QChar(KVI_TEXT_UNDERLINE)) &&
		(c != QChar(KVI_TEXT_RESET)) && (c != QChar(KVI_TEXT_REVERSE)) &&
		(c != QChar(KVI_TEXT_CRYPTESCAPE)) && (c != QChar(KVI_TEXT_ICON))))
	{
		m_bControlBlock = false;
		//Not a control code...run..
		while((idx < ((int)(m_szTextBuffer.length()))) && (curXPos < maxXPos))
		{
			c = m_szTextBuffer[idx];
			if((c.unicode() > 32) ||
				((c != QChar(KVI_TEXT_COLOR)) && (c != QChar(KVI_TEXT_BOLD)) &&
				(c != QChar(KVI_TEXT_UNDERLINE)) && (c != QChar(KVI_TEXT_RESET)) &&
				(c != QChar(KVI_TEXT_REVERSE)) && (c != QChar(KVI_TEXT_CRYPTESCAPE)) &&
				(c != QChar(KVI_TEXT_ICON))))
			{
				m_iBlockLen++;
#ifdef COMPILE_USE_QT4
				int xxx = fm.width(c);
#else
				int xxx = (c.unicode() < 256 ? g_iInputFontCharWidth[c.unicode()] : fm.width(c));
#endif
				m_iBlockWidth +=xxx;
				curXPos       +=xxx;
				idx++;
			} else break;
		}
		return;
	} else {
		m_bControlBlock = true;
		m_iBlockLen = 1;
		m_iBlockWidth = g_iInputFontCharWidth[c.unicode()];
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
				char auxClr = m_iCurFore;
				m_iCurFore  = m_iCurBack;
				m_iCurBack  = auxClr;
			}
			break;
			case KVI_TEXT_CRYPTESCAPE:
			case KVI_TEXT_ICON:
				// makes a single block
				break;
			case KVI_TEXT_COLOR:
			{
				idx++;
				if(idx >= ((int)(m_szTextBuffer.length())))return;
				unsigned char fore;
				unsigned char back;
				idx = getUnicodeColorBytes(m_szTextBuffer,idx,&fore,&back);
				if(fore != KVI_NOCHANGE)
				{
					m_iCurFore = fore;
					if(back != KVI_NOCHANGE)m_iCurBack = back;
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
	register int idx = 0;
	while(idx < m_iFirstVisibleChar)
	{
		unsigned short c = m_szTextBuffer[idx].unicode();
		if(c < 32)
		{
			switch(c)
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
					char auxClr = m_iCurFore;
					m_iCurFore  = m_iCurBack;
					m_iCurBack  = auxClr;
				}
				break;
				case KVI_TEXT_COLOR:
				{
					idx++;
					if(idx >= ((int)(m_szTextBuffer.length())))return;
					unsigned char fore;
					unsigned char back;
					idx = getUnicodeColorBytes(m_szTextBuffer,idx,&fore,&back);
					idx--;
					if(fore != KVI_NOCHANGE)m_iCurFore = fore;
					else m_iCurFore = KVI_INPUT_DEF_FORE;
					if(back != KVI_NOCHANGE)m_iCurBack = back;
					else m_iCurBack = KVI_INPUT_DEF_BACK;
				}
				break;
				case 0:
					debug("KviInputEditor::Encountered invisible end of the string!");
					exit(0);
				break;
			}
		}
		idx++;
	}
}


void KviInputEditor::mousePressEvent(QMouseEvent *e)
{
	if(e->button() & Qt::LeftButton)
	{
		m_iCursorPosition = charIndexFromXPosition(e->pos().x());
		//move the cursor to
		int anchorX =  xPositionFromCharIndex(m_iCursorPosition);
		if(anchorX > (width()-frameWidth()))m_iFirstVisibleChar++;
		m_iSelectionAnchorChar = m_iCursorPosition;
		selectOneChar(-1);
		//grabMouse(QCursor(crossCursor));
		repaintWithCursorOn();
		killDragTimer();
		m_iDragTimer = startTimer(KVI_INPUT_DRAG_TIMEOUT);

	} else if(e->button() & Qt::RightButton)
	{
        int type = g_pActiveWindow->type();

		//Popup menu
		g_pInputPopup->clear();
		
		QString szClip;

		QClipboard * c = QApplication::clipboard();
		if(c)
		{
			szClip = c->text(QClipboard::Clipboard);
			
#ifdef COMPILE_USE_QT4
			int occ = szClip.count(QChar('\n'));
#else
			int occ = szClip.contains(QChar('\n'));
#endif
	
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
	
				QString label = "<center><b>";
				label += __tr2qs("Clipboard");
				label += ":</b><br>";
				label += szClip;
				label += "<br><b>";
	
				QString num;
				num.setNum(occ);
	
				label += num;
				label += QChar(' ');
				label += (occ == 1) ? __tr2qs("line break") : __tr2qs("line breaks");
				label += "</b></center>";
	
				QLabel * l = new QLabel(label,g_pInputPopup);
				l->setFrameStyle(QFrame::Raised | QFrame::StyledPanel);
				l->setMargin(5);
				// FIXME: This does NOT work under Qt 4.x (they seem to consider it as bad UI design)
#ifndef COMPILE_USE_QT4
				g_pInputPopup->insertItem(l);
#else
				delete l;
#endif
			}
		}
		
		int id = g_pInputPopup->insertItem(__tr2qs("Cu&t") + ACCEL_KEY(X),this,SLOT(cut()));
		g_pInputPopup->setItemEnabled(id,hasSelection());
		id = g_pInputPopup->insertItem(__tr2qs("&Copy") + ACCEL_KEY(C),this,SLOT(copyToClipboard()));
		g_pInputPopup->setItemEnabled(id,hasSelection());
		id = g_pInputPopup->insertItem(__tr2qs("&Paste") + ACCEL_KEY(V),this,SLOT(pasteClipboardWithConfirmation()));
		g_pInputPopup->setItemEnabled(id,!szClip.isEmpty() && !m_bReadOnly);
		id = g_pInputPopup->insertItem(__tr2qs("Paste (Slowly)"),this,SLOT(pasteSlow()));
		if ((type == KVI_WINDOW_TYPE_CHANNEL) || (type == KVI_WINDOW_TYPE_QUERY) || (type == KVI_WINDOW_TYPE_DCCCHAT))
			g_pInputPopup->setItemEnabled(id,!szClip.isEmpty() && !m_bReadOnly);
		else 
			g_pInputPopup->setItemEnabled(id,false);
		id = g_pInputPopup->insertItem(__tr2qs("Paste &File") + ACCEL_KEY(F),this,SLOT(pasteFile()));
		if ((type != KVI_WINDOW_TYPE_CHANNEL) && (type != KVI_WINDOW_TYPE_QUERY) && (type != KVI_WINDOW_TYPE_DCCCHAT))
			g_pInputPopup->setItemEnabled(id,false);
		else
			g_pInputPopup->setItemEnabled(id,!m_bReadOnly);
		if(m_bSpSlowFlag ==true)
		{
			id = g_pInputPopup->insertItem(__tr2qs("Stop Paste"),this,SLOT(stopPasteSlow())); /*G&N 2005*/
		}
		id = g_pInputPopup->insertItem(__tr2qs("Clear"),this,SLOT(clear()));
		g_pInputPopup->setItemEnabled(id,!m_szTextBuffer.isEmpty() && !m_bReadOnly);
		g_pInputPopup->insertSeparator();
		id = g_pInputPopup->insertItem(__tr2qs("Select All"),this,SLOT(selectAll()));
		g_pInputPopup->setItemEnabled(id,(!m_szTextBuffer.isEmpty()));
		
		
		g_pInputPopup->insertSeparator();
		m_pIconMenu->clear();
		
		KviDict<KviTextIcon> * d = g_pTextIconManager->textIconDict();
		KviDictIterator<KviTextIcon> it(*d);
		QStringList strList;
		while(KviTextIcon * i = it.current())
		{
			strList.append(it.currentKey());
			++it;
		}
		strList.sort();
		KviTextIcon * icon;
		QPixmap *pix;
		
		for(QStringList::Iterator iter = strList.begin(); iter != strList.end(); ++iter)
		{
			icon=g_pTextIconManager->lookupTextIcon(*iter);
			if(icon)
			{
				pix = icon->pixmap();
				if(pix) m_pIconMenu->insertItem(*pix,*iter);
			}
		}
		
		g_pInputPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_BIGGRIN)),__tr2qs("Insert Icon"),m_pIconMenu);
		g_pInputPopup->popup(mapToGlobal(e->pos()));
	} else {
		pasteSelectionWithConfirmation();
	}
}
void KviInputEditor::iconPopupActivated(int id)
{
	if(!m_bReadOnly)
	{
		QString text = m_pIconMenu->text(id);
		if(!text.isEmpty())
		{
			text.prepend(KVI_TEXT_ICON);
			text.append(' ');
			insertText(text);
		}
	}
}

bool KviInputEditor::hasSelection()
{
	return ((m_iSelectionBegin != -1)&&(m_iSelectionEnd != -1));
}

void KviInputEditor::copyToClipboard()
{
	if(!hasSelection())return;
	QClipboard * c = QApplication::clipboard();
	if(!c)return;
	QString szTxt = m_szTextBuffer.mid(m_iSelectionBegin,(m_iSelectionEnd-m_iSelectionBegin)+1);
	c->setText(szTxt,QClipboard::Clipboard);
	repaintWithCursorOn();
}

void KviInputEditor::copyToSelection(bool bDonNotCopyToClipboard)
{
	if(!hasSelection())return;
	QClipboard * c = QApplication::clipboard();
	if(!c)return;
	QString szTxt = m_szTextBuffer.mid(m_iSelectionBegin,(m_iSelectionEnd-m_iSelectionBegin)+1);
	if(c->supportsSelection())
		c->setText(szTxt,QClipboard::Selection);
	else if(!bDonNotCopyToClipboard)
		c->setText(szTxt,QClipboard::Clipboard);
	repaintWithCursorOn();
}


void KviInputEditor::moveCursorTo(int idx,bool bRepaint)
{
	if(idx < 0)idx = 0;
	if(idx > ((int)(m_szTextBuffer.length())))idx = m_szTextBuffer.length();
	if(idx > m_iCursorPosition)
	{
		while(m_iCursorPosition < idx)
		{
			moveRightFirstVisibleCharToShowCursor();
			m_iCursorPosition++;
		}
	} else {
		m_iCursorPosition = idx;
		if(m_iFirstVisibleChar > m_iCursorPosition)m_iFirstVisibleChar = m_iCursorPosition;
	}
	if(bRepaint)repaintWithCursorOn();
}

void KviInputEditor::removeSelected()
{
	if(!hasSelection())return;
	m_szTextBuffer.remove(m_iSelectionBegin,(m_iSelectionEnd-m_iSelectionBegin)+1);
	moveCursorTo(m_iSelectionBegin,false);
	selectOneChar(-1);
	repaintWithCursorOn();
}

void KviInputEditor::cut()
{
	if(!hasSelection())return;
	QClipboard * c = QApplication::clipboard();
	if(!c)return;
	c->setText(m_szTextBuffer.mid(m_iSelectionBegin,(m_iSelectionEnd-m_iSelectionBegin)+1),QClipboard::Clipboard);
	m_szTextBuffer.remove(m_iSelectionBegin,(m_iSelectionEnd-m_iSelectionBegin)+1);
	moveCursorTo(m_iSelectionBegin,false);
	selectOneChar(-1);
	repaintWithCursorOn();
}

void KviInputEditor::insertText(const QString &text)
{
	QString szText = text; // crop away constness
	if(szText.isEmpty())return;

	//szText.replaceAll('\t'," "); //Do not paste tabs

	//szText.replace(QRegExp("\t")," "); // do not paste tabs

	m_bUpdatesEnabled = false;
	removeSelected();
	m_bUpdatesEnabled = true;

	if(szText.find('\n') == -1)
	{
		m_szTextBuffer.insert(m_iCursorPosition,szText);
		m_szTextBuffer.truncate(m_iMaxBufferSize);
		moveCursorTo(m_iCursorPosition + szText.length());
	} else {
		//Multiline paste...do not execute commands here
		QString szBlock;
		while(!szText.isEmpty())
		{
			int idx = szText.find('\n');
			if(idx != -1)
			{
				szBlock = szText.left(idx);
				//else szBlock = QChar(KVI_TEXT_RESET);
				szText.remove(0,idx+1);
			} else {
				szBlock = szText;
				szText  = "";
			}

			m_szTextBuffer.insert(m_iCursorPosition,szBlock);
			m_szTextBuffer.truncate(m_iMaxBufferSize);

			int pos = 0;
			while((pos < ((int)(m_szTextBuffer.length()))) && (m_szTextBuffer[pos] < 33))pos++;
			if((pos < ((int)(m_szTextBuffer.length()))) && (m_szTextBuffer[pos] == QChar('/')))m_szTextBuffer.insert(pos,"\\");

			returnPressed(idx != -1);
		}
	}
}

// Replace (length) characters in the buffer from (start) with (text), returns
// the length of the text inserted (different from text.length() only if the 
// buffer was truncated.
int KviInputEditor::replaceSegment(int start, int length, const QString &text)
{
	m_szTextBuffer.remove(start, length);
	m_szTextBuffer.insert(start, text);
	m_szTextBuffer.truncate(m_iMaxBufferSize);
	repaintWithCursorOn();

	int iInsertedLength = text.length();
	int iMaxInsertedLength = m_iMaxBufferSize - start;
	if(iInsertedLength > iMaxInsertedLength) return iMaxInsertedLength;
	return iInsertedLength;
}

void KviInputEditor::pasteClipboardWithConfirmation()
{
	QClipboard * c = QApplication::clipboard();
	if(!c)return;
	QString szText = c->text(QClipboard::Clipboard);

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
	QClipboard * c = QApplication::clipboard();
	if(!c)return;
	QString szText = c->text(c->supportsSelection() ? QClipboard::Selection : QClipboard::Clipboard);

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
	QString stmp = QFileDialog::getOpenFileName("","",this,"Paste File", "Choose a file" );
	if(stmp!="")
	{
		QString stmp1 = "spaste.file " + stmp ;
		KviKvsScript::run(stmp1,g_pActiveWindow);
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

void KviInputEditor::setText(const QString text)
{
	m_szTextBuffer = text;
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
	if(hasSelection())
		copyToSelection();
}

void KviInputEditor::killDragTimer()
{
	if(m_iDragTimer)
	{
		killTimer(m_iDragTimer);
		m_iDragTimer = 0;
	}
}

void KviInputEditor::timerEvent(QTimerEvent *e)
{
	if(e->timerId() == m_iCursorTimer)
	{
		if(!hasFocus() || !isVisibleToTLW())
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
	if(m_iSelectionAnchorChar == -1)return;

	QPoint pnt = mapFromGlobal(QCursor::pos());


	if(pnt.x() <= 0)
	{
		//Left side dragging
		if(m_iFirstVisibleChar > 0)m_iFirstVisibleChar--;
		m_iCursorPosition = m_iFirstVisibleChar;
	} else if(pnt.x() >= width())
	{
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
	if(m_iCursorPosition == m_iSelectionAnchorChar)selectOneChar(-1);
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

void KviInputEditor::returnPressed(bool bRepaint)
{
	if (!m_szTextBuffer.isEmpty() /* && (!m_pHistory->current() || m_szTextBuffer.compare(*(m_pHistory->current())))*/)
	{
		if(m_pInputParent->inherits("KviInput"))
			g_pInputHistory->add(new QString(m_szTextBuffer));

		m_pHistory->insert(0,new QString(m_szTextBuffer));
	}

	__range_valid(KVI_INPUT_MAX_LOCAL_HISTORY_ENTRIES > 1); //ABSOLUTELY NEEDED, if not, pHist will be destroyed...
	if(m_pHistory->count() > KVI_INPUT_MAX_LOCAL_HISTORY_ENTRIES)m_pHistory->removeLast();

	m_iCurHistoryIdx = -1;

	// FIXME: ALL THIS STUFF SHOULD BE CONVERTED TO QString
	/*
	if(m_pInputParent->inherits("KviInput"))
	{
		QString szBuffer(m_szTextBuffer);
		m_szTextBuffer="";
		selectOneChar(-1);
		m_iCursorPosition = 0;
		m_iFirstVisibleChar = 0;
		if(bRepaint)repaintWithCursorOn();
		KviUserInput::parse(szBuffer,m_pKviWindow);
	} else {
	*/
	emit enterPressed();
	/*
		return;
	}
	*/
}

void KviInputEditor::focusInEvent(QFocusEvent *)
{
	if(m_iCursorTimer==0)
	{
		m_iCursorTimer = startTimer(KVI_INPUT_BLINK_TIME);
		m_bCursorOn = true;
		update();
	}
	// XIM handling...
#ifndef COMPILE_USE_QT4
	// THIS SEEMS TO BE GONE IN Qt4.x ? (even if the documentation states that it *should* be there)
	setMicroFocusHint(1,1,width() - 2,height() - 2,true,0);
#endif
}

void KviInputEditor::focusOutEvent(QFocusEvent *)
{
	if(m_iCursorTimer)killTimer(m_iCursorTimer);
	m_iCursorTimer = 0;
	m_bCursorOn = false;
	update();
}


void KviInputEditor::internalCursorRight(bool bShift)
{
	if(m_iCursorPosition >= ((int)(m_szTextBuffer.length())))return;
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
	if(m_iCursorPosition <= 0)return;

	if(bShift)
	{
		if((m_iSelectionBegin > -1)&&(m_iSelectionEnd > -1))
		{
			if(m_iSelectionBegin == m_iCursorPosition)m_iSelectionBegin--;
			else if(m_iSelectionEnd == m_iCursorPosition-1)m_iSelectionEnd--;
			else selectOneChar(m_iCursorPosition - 1);
		} else selectOneChar(m_iCursorPosition - 1);
	} else selectOneChar(-1);

	m_iCursorPosition--;
	if(m_iFirstVisibleChar > m_iCursorPosition)m_iFirstVisibleChar--;
}

// remember the text before and after the cursor at this point, and put them
// before and after the text inserted by IM in imEndEvent.
//    hagabaka
void KviInputEditor::imStartEvent(QIMEvent *e)
{
	removeSelected();
	m_iIMStart = m_iIMSelectionBegin = m_iCursorPosition;
	m_iIMLength = 0;
	m_bIMComposing = true;
	e->accept();
}

// Whenever the IM's preedit changes, update the visuals and internal data. refer to <http://doc.trolltech.com/3.3/qimevent.html> */
//    hagabaka
void KviInputEditor::imComposeEvent(QIMEvent *e)
{
	// replace the old pre-edit string with e->text()
	m_bUpdatesEnabled = false;
#ifdef COMPILE_USE_QT4
	// Qt 4.x ??????????
	m_iIMLength = replaceSegment(m_iIMStart, m_iIMLength, e->commitString());

	// update selection inside the pre-edit
	m_iIMSelectionBegin = m_iIMStart + e->replacementStart();
	m_iIMSelectionLength = e->replacementLength();
	moveCursorTo(m_iIMSelectionBegin);

#else
	m_iIMLength = replaceSegment(m_iIMStart, m_iIMLength, e->text());

	// update selection inside the pre-edit
	m_iIMSelectionBegin = m_iIMStart + e->cursorPos();
	m_iIMSelectionLength = e->selectionLength();
	moveCursorTo(m_iIMSelectionBegin);
#endif


	// repaint
	m_bUpdatesEnabled = true;
	repaintWithCursorOn();
	e->accept();
}

// Input method is done; put its resulting text to where the preedit area was
//    hagabaka
void KviInputEditor::imEndEvent(QIMEvent *e)
{
	// replace the preedit area with the IM result text
	m_bUpdatesEnabled = false;
#ifdef COMPILE_USE_QT4
	// Qt 4.x ??????????
	m_iIMLength = replaceSegment(m_iIMStart, m_iIMLength, e->commitString());
#else
	m_iIMLength = replaceSegment(m_iIMStart, m_iIMLength, e->text());
#endif

	// move cursor to after the IM result text
	moveCursorTo(m_iIMStart + m_iIMLength);

	// repaint
	m_bUpdatesEnabled = true;
	repaintWithCursorOn();

	// reset data
	m_bIMComposing = false;
	e->accept();
}

// FIXME According to <http://www.kde.gr.jp/~asaki/how-to-support-input-method.html>, if the XIM
//  style used is OverTheTop, code needs to be added in keyPressEvent handler */
//    hagabaka
void KviInputEditor::keyPressEvent(QKeyEvent *e)
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
		if((e->key() == Qt::Key_Tab) || (e->key() == Qt::Key_BackTab))
		{
			completion(e->state() & Qt::ShiftButton);
			return;
		} else {
			m_bLastCompletionFinished=1;
		}
	}
	
	if(e->key() == Qt::Key_Escape)
	{
		emit escapePressed();
		return;
	}

//Make CtrlKey and CommandKey ("Apple") behave equally on MacOSX.
//This way typical X11 and Apple shortcuts can be used simultanously within the input line.
#ifndef Q_OS_MACX
	if(e->state() & Qt::ControlButton)
#else
	if((e->state() & Qt::ControlButton) || (e->state() & MetaButton))
#endif
	{
		switch(e->key())
		{
			case Qt::Key_Right:
				if(m_iCursorPosition < ((int)(m_szTextBuffer.length())))
				{
					// skip whitespace
					while(m_iCursorPosition < ((int)(m_szTextBuffer.length())))
					{
						if(!m_szTextBuffer.at(m_iCursorPosition).isSpace())break;
						internalCursorRight(e->state() & Qt::ShiftButton);
					}
					// skip nonwhitespace
					while(m_iCursorPosition < ((int)(m_szTextBuffer.length())))
					{
						if(m_szTextBuffer.at(m_iCursorPosition).isSpace())break;
						internalCursorRight(e->state() & Qt::ShiftButton);
					}
					repaintWithCursorOn();
				}
			break;
			case Qt::Key_Left:
				if(m_iCursorPosition > 0)
				{
					// skip whitespace
					while(m_iCursorPosition > 0)
					{
						if(!m_szTextBuffer.at(m_iCursorPosition - 1).isSpace())break;
						internalCursorLeft(e->state() & Qt::ShiftButton);
					}
					// skip nonwhitespace
					while(m_iCursorPosition > 0)
					{
						if(m_szTextBuffer.at(m_iCursorPosition - 1).isSpace())break;
						internalCursorLeft(e->state() & Qt::ShiftButton);
					}
					repaintWithCursorOn();
				}
			break;
			case Qt::Key_K:
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
			break;
			case Qt::Key_B:
				if(!m_bReadOnly) insertChar(KVI_TEXT_BOLD);
			break;
			case Qt::Key_O:
				if(!m_bReadOnly) insertChar(KVI_TEXT_RESET);
			break;
			case Qt::Key_U:
				if(!m_bReadOnly) insertChar(KVI_TEXT_UNDERLINE);
			break;
			case Qt::Key_R:
				if(!m_bReadOnly) insertChar(KVI_TEXT_REVERSE);
			break;
			case Qt::Key_P:
				if(!m_bReadOnly) insertChar(KVI_TEXT_CRYPTESCAPE); // DO NOT CRYPT THIS STUFF
			break;
			case Qt::Key_I:
			{
				if(!m_bReadOnly)
				{
					insertChar(KVI_TEXT_ICON); // THE NEXT WORD IS AN ICON NAME
					int xPos = xPositionFromCharIndex(m_iCursorPosition);
					if(xPos > 24)xPos-=24;
					if(!g_pTextIconWindow)g_pTextIconWindow = new KviTextIconWindow();
					if(xPos+g_pTextIconWindow->width() > width())xPos = width()-(g_pTextIconWindow->width()+2);
					g_pTextIconWindow->move(mapToGlobal(QPoint(xPos,-KVI_TEXTICON_WIN_HEIGHT)));
					g_pTextIconWindow->popup(this);
				}
			}
			break;
			case Qt::Key_C:
				copyToClipboard();
			break;
			case Qt::Key_X:
				if(!m_bReadOnly) cut();
			break;
			case Qt::Key_V:
				if(!m_bReadOnly) pasteClipboardWithConfirmation();
			break;
			case Qt::Key_Backspace:
			case Qt::Key_W:
				if(!m_bReadOnly && !hasSelection())
				{
					while(m_iCursorPosition>0 && !m_szTextBuffer[m_iCursorPosition-1].isSpace())
					{
						m_szTextBuffer.remove(m_iCursorPosition-1,1);
						m_iCursorPosition--;
						if(m_iFirstVisibleChar > m_iCursorPosition)m_iFirstVisibleChar--;
					}
					selectOneChar(-1);
					repaintWithCursorOn();
				}
			break;
			case Qt::Key_PageUp:
				if(KVI_OPTION_BOOL(KviOption_boolDisableInputHistory)) break;
				if(m_pInputParent->inherits("KviInput"))
					((KviInput*)(m_pInputParent))->historyButtonClicked();
			break;
			case Qt::Key_F:
				if(m_pKviWindow)
					if(m_pKviWindow->view())m_pKviWindow->view()->toggleToolWidget();
			break;
			case Qt::Key_A:
				m_iSelectionBegin=0;
				m_iSelectionEnd=m_szTextBuffer.length()-1;
				m_iCursorPosition=m_szTextBuffer.length();
				repaintWithCursorOn();
			break;
			case Qt::Key_Return:
			case Qt::Key_Enter:
				if(m_pInputParent->inherits("KviInput"))
				{
					QString szBuffer(m_szTextBuffer);
					m_szTextBuffer="";
					selectOneChar(-1);
					m_iCursorPosition = 0;
					m_iFirstVisibleChar = 0;
					repaintWithCursorOn();
					KviUserInput::parseNonCommand(szBuffer,m_pKviWindow);
					if (!szBuffer.isEmpty())
					{
						g_pInputHistory->add(new QString(szBuffer));
						m_pHistory->insert(0,new QString(szBuffer));
					}
				
					__range_valid(KVI_INPUT_MAX_LOCAL_HISTORY_ENTRIES > 1); //ABSOLUTELY NEEDED, if not, pHist will be destroyed...
					if(m_pHistory->count() > KVI_INPUT_MAX_LOCAL_HISTORY_ENTRIES)m_pHistory->removeLast();
				
					m_iCurHistoryIdx = -1;
				}
				break;	
			default:
				if(!m_bReadOnly) insertText(e->text());
			break;
		}
		return;
	}

	if((e->state() & Qt::AltButton) && (e->state() & Qt::Keypad))
	{
		// Qt::Key_Meta seems to substitute Qt::Key_Alt on some keyboards
		if((e->key() == Qt::Key_Alt) || (e->key() == Qt::Key_Meta))
		{
			m_szAltKeyCode = "";
			return;
		} else if((e->ascii() >= '0') && (e->ascii() <= '9'))
		{
			m_szAltKeyCode += e->ascii();
			return;
		}

		//debug("%c",e->ascii());
		if(!m_bReadOnly) insertText(e->text());
		return;
	}

	if(e->state() & Qt::ShiftButton)
	{
		switch(e->key())
		{
			case Qt::Key_Insert:
				if(!m_bReadOnly) pasteClipboardWithConfirmation();
				return;
			break;
			case Qt::Key_PageUp:
				if(m_pKviWindow)
					if(m_pKviWindow->view())m_pKviWindow->view()->prevLine();
				return;
			break;
			case Qt::Key_PageDown:
				if(m_pKviWindow)
					if(m_pKviWindow->view())m_pKviWindow->view()->nextLine();
				return;
			break;
		}
	}
	if(e->state() & Qt::AltButton)
	{
		switch(e->key())
		{
			case Qt::Key_Backspace:
				if(m_pInputParent->inherits("KviInput"))
				{
					((KviInput*)(m_pInputParent))->multiLinePaste(m_szTextBuffer);
					clear();
					return;
				}
				break;
		}
	}
	switch(e->key())
	{
		case Qt::Key_Right:
			if(m_iCursorPosition < ((int)(m_szTextBuffer.length())))
			{
				internalCursorRight(e->state() & Qt::ShiftButton);
				repaintWithCursorOn();
			}
			break;
		case Qt::Key_Left:
			if(m_iCursorPosition > 0)
			{
				internalCursorLeft(e->state() & Qt::ShiftButton);
				repaintWithCursorOn();
			}
			break;
		case Qt::Key_Backspace:
			if(!m_bReadOnly)
			{
				if(hasSelection() && (m_iSelectionEnd >= m_iCursorPosition-1) && (m_iSelectionBegin <= m_iCursorPosition))
				{
					//remove the selection
					m_szTextBuffer.remove(m_iSelectionBegin,(m_iSelectionEnd-m_iSelectionBegin)+1);
					m_iCursorPosition = m_iSelectionBegin;
					if(m_iFirstVisibleChar > m_iCursorPosition)m_iFirstVisibleChar = m_iCursorPosition;
				} else if(m_iCursorPosition > 0) {
					m_iCursorPosition--;
					m_szTextBuffer.remove(m_iCursorPosition,1);
					if(m_iFirstVisibleChar > m_iCursorPosition)m_iFirstVisibleChar--;
				}
				selectOneChar(-1);
				repaintWithCursorOn();
			}
			break;
		case Qt::Key_Delete:
			if(!m_bReadOnly)
			{
				if(hasSelection()) removeSelected();
				else if(m_iCursorPosition < (int)m_szTextBuffer.length())
				{
					m_szTextBuffer.remove(m_iCursorPosition,1);
					selectOneChar(-1);
					repaintWithCursorOn();
				}
			}
			break;
		case Qt::Key_Home:
			if(m_iCursorPosition > 0)
			{
				if(e->state() & Qt::ShiftButton)
				{
					if((m_iSelectionBegin == -1)&&(m_iSelectionEnd == -1))m_iSelectionEnd = m_iCursorPosition - 1;
					m_iSelectionBegin = 0;
				} else {
					selectOneChar(-1);
				}
				home();
			}
			break;
		case Qt::Key_End://we should call it even the cursor is at the end for deselecting
			if(e->state() & Qt::ShiftButton)
			{
				if((m_iSelectionBegin == -1)&&(m_iSelectionEnd == -1))m_iSelectionBegin = m_iCursorPosition;
				m_iSelectionEnd = m_szTextBuffer.length()-1;
			} else {
				selectOneChar(-1);
			}
			end();
			break;
		case Qt::Key_Up:
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
			break;
		case Qt::Key_Down:
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
			break;
		case Qt::Key_PageUp:
			if(m_pKviWindow)
				if(m_pKviWindow->view())m_pKviWindow->view()->prevPage();
		break;
		case Qt::Key_PageDown:
			if(m_pKviWindow)
				if(m_pKviWindow->view())m_pKviWindow->view()->nextPage();
		break;
		case Qt::Key_Return:
		case Qt::Key_Enter:
			returnPressed();
			break;
		case Qt::Key_Alt:
		case Qt::Key_Meta:
			m_szAltKeyCode = "";
			break;
		default:
			if(!e->text().isEmpty() && !m_bReadOnly)
				insertText(e->text());
		break;
	}
}

void KviInputEditor::keyReleaseEvent(QKeyEvent *e)
{
	if((e->key() == Qt::Key_Alt) || (e->key() == Qt::Key_Meta))
	{
		if(m_szAltKeyCode.hasData())
		{
			bool bOk;
			unsigned short ch = m_szAltKeyCode.toUShort(&bOk);
			if(bOk && ch != 0)
			{
				//debug("INSERTING CHAR %d",ch);
				insertChar(QChar(ch));
				e->accept();
			}
		}
		m_szAltKeyCode = "";
	}
	e->ignore();
}

void KviInputEditor::getWordBeforeCursor(QString &buffer,bool * bIsFirstWordInLine)
{
	if(m_szTextBuffer.isEmpty() || m_iCursorPosition <= 0)
	{
		buffer = "";
		return;
	}

	buffer = m_szTextBuffer.left(m_iCursorPosition);

	int idx = buffer.findRev(' ');
	int idx2 = buffer.findRev(','); // This is for comma separated lists...
	int idx3 = buffer.findRev('(');
	int idx4 = buffer.findRev('"');
	if(idx2 > idx)idx = idx2;
	if(idx3 > idx)idx = idx3;
	if(idx4 > idx)idx = idx4;
	*bIsFirstWordInLine = false;
	if(idx > -1)buffer.remove(0,idx+1);
	else *bIsFirstWordInLine = true;
}

void KviInputEditor::completion(bool bShift)
{
	// FIXME: Spaces in directory completion can mess everything completely
	//        On windows the KVI_PATH_SEPARATOR_CHARacters are breaking everything...
	//        Well.... :D

	QString word;
	QString match;

	bool bFirstWordInLine;
	getWordBeforeCursor(word,&bFirstWordInLine);
	if(word.isEmpty())
	{
		if(m_szLastCompletedNick.isEmpty())return; // nothing to complete
		else {
			// this is standard nick completion continued
			standardNickCompletion(bShift,word,bFirstWordInLine);
			repaintWithCursorOn();
			return;
		}
	}
	KviPtrList<QString> tmp;
	tmp.setAutoDelete(true);

	bool bIsCommand = false;
	bool bIsDir = false;
	bool bIsNick = false;

	unsigned short uc = word[0].unicode();

	if(uc == '/')
	{
		if(bFirstWordInLine)
		{
			// command completion
			word.remove(0,1);
			if(word.isEmpty())return;
			KviKvsKernel::instance()->completeCommand(word,&tmp);
			bIsCommand = true;
		} else {
			// directory completion attempt
			g_pApp->completeDirectory(word,&tmp);
			bIsDir = true;
		}
	} else if(uc == '$')
	{
		// function/identifer completion
		word.remove(0,1);
		if(word.isEmpty())return;
		KviKvsKernel::instance()->completeFunction(word,&tmp);
	} else if(uc == '#' || uc == '&' || uc == '!')
	{
		if(m_pKviWindow)
		{
			if( (word.length()==1) && (m_pKviWindow->windowName()[0].unicode()==uc))
			{
				match=m_pKviWindow->windowName();
				match.append(" ");
				replaceWordBeforeCursor(word,match,false);
				repaintWithCursorOn();
				return;
			} else {
				if(m_pKviWindow->console())
					m_pKviWindow->console()->completeChannel(word,&tmp);
			}
		}

	//FIXME: Complete also on irc:// starting strings, not only irc.?
	} else if(KviQString::equalCIN(word,"irc.",4))
	{
		// irc server name
		if(m_pKviWindow)
			if(m_pKviWindow->console())
				m_pKviWindow->console()->completeServer(word,&tmp);
	} else {
		// empty word will end up here
		if(m_pUserListView)
		{
			if(KVI_OPTION_BOOL(KviOption_boolBashLikeNickCompletion))
			{
				m_pUserListView->completeNickBashLike(word,&tmp,bShift);
				bIsNick = true;
			} else {
				standardNickCompletion(bShift,word,bFirstWordInLine);
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
			match = *(tmp.first());
			if(bIsCommand)match.append(' ');
			else if(bIsNick)
			{
				if(!KVI_OPTION_STRING(KviOption_stringNickCompletionPostfix).isEmpty())
				{
					if(bFirstWordInLine || (!KVI_OPTION_BOOL(KviOption_boolUseNickCompletionPostfixForFirstWordOnly)))
						match.append(KVI_OPTION_STRING(KviOption_stringNickCompletionPostfix));
				}
			}
		} else {
			QString all;
			QString * s = tmp.first();
			match = *s;
			int wLen = word.length();
			for(;s;s = tmp.next())
			{
				if(s->length() < match.length())
					match.remove(s->length(),match.length() - s->length());
				// All the matches here have length >= word.len()!!!
				const QChar * b1 = KviQString::nullTerminatedArray(*s) + wLen;
				const QChar * b2 = KviQString::nullTerminatedArray(match) + wLen;
				const QChar * c1 = b1;
				const QChar * c2 = b2;
				if(bIsDir)while(c1->unicode() && (c1->unicode() == c2->unicode()))c1++,c2++;
				else while(c1->unicode() && (c1->lower().unicode() == c2->lower().unicode()))c1++,c2++;
				int len = wLen + (c1 - b1);
				if(len < ((int)(match.length())))match.remove(len,match.length() - len);
				if(!all.isEmpty())all.append(", ");
				all.append(*s);
			}
			if(m_pKviWindow)
				m_pKviWindow->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("%d matches: %Q"),tmp.count(),&all);
		}
	} else 
		if(m_pKviWindow)
			m_pKviWindow->outputNoFmt(KVI_OUT_SYSTEMMESSAGE,__tr2qs("No matches"));

	if(!match.isEmpty())
	{
		//if(!bIsDir && !bIsNick)match = match.lower(); <-- why? It is nice to have
		//						 $module.someFunctionName instad 
		//						 of unreadable $module.somefunctionfame
		replaceWordBeforeCursor(word,match,false);
	}

	repaintWithCursorOn();
}

void KviInputEditor::replaceWordBeforeCursor(const QString &word,const QString &replacement,bool bRepaint)
{
	selectOneChar(-1);
	m_iCursorPosition -= word.length();
	m_szTextBuffer.remove(m_iCursorPosition,word.length());
	m_szTextBuffer.insert(m_iCursorPosition,replacement);
	m_szTextBuffer.truncate(m_iMaxBufferSize);
	moveCursorTo(m_iCursorPosition + replacement.length());
	if(bRepaint)repaintWithCursorOn();
}

void KviInputEditor::standardNickCompletion(bool bAddMask,QString &word,bool bFirstWordInLine)
{
	// FIXME: this could be really simplified...
	if(!m_pUserListView)return;
	selectOneChar(-1);

	QString buffer;
	if(m_szLastCompletedNick.isEmpty())
	{
		// New completion session: we NEED sth to complete
		if(word.isEmpty())return;
		if(m_pUserListView->completeNickStandard(word,m_szLastCompletedNick,buffer,bAddMask))
		{
			// completed: save the buffer
			m_szLastCompletionBuffer          = m_szTextBuffer;
			m_iLastCompletionCursorPosition   = m_iCursorPosition;
			m_iLastCompletionCursorXPosition  = m_iLastCursorXPosition;
			m_iLastCompletionFirstVisibleChar = m_iFirstVisibleChar;
			m_szLastCompletedNick             = buffer;
			if(!KVI_OPTION_STRING(KviOption_stringNickCompletionPostfix).isEmpty())
			{
				if(bFirstWordInLine || (!KVI_OPTION_BOOL(KviOption_boolUseNickCompletionPostfixForFirstWordOnly)))
					buffer.append(KVI_OPTION_STRING(KviOption_stringNickCompletionPostfix));
			}
			replaceWordBeforeCursor(word,buffer,false);
			m_bLastCompletionFinished=0;
			// REPAINT CALLED FROM OUTSIDE!
		} // else no match at all
	} else  if(!m_bLastCompletionFinished) {
		// Old session
		// swap the buffers
		m_szTextBuffer                        = m_szLastCompletionBuffer;
		m_iCursorPosition                     = m_iLastCompletionCursorPosition;
		m_iLastCursorXPosition                = m_iLastCompletionCursorXPosition;
		m_iFirstVisibleChar                   = m_iLastCompletionFirstVisibleChar;
		// re-extract
		//word = m_szTextBuffer.left(m_iCursorPosition);

		getWordBeforeCursor(word,&bFirstWordInLine);
		if(word.isEmpty())return;
		if(m_pUserListView->completeNickStandard(word,m_szLastCompletedNick,buffer,bAddMask))
		{
			// completed
			m_szLastCompletedNick             = buffer;
			if(!KVI_OPTION_STRING(KviOption_stringNickCompletionPostfix).isEmpty())
			{
				if(bFirstWordInLine || (!KVI_OPTION_BOOL(KviOption_boolUseNickCompletionPostfixForFirstWordOnly)))
					buffer.append(KVI_OPTION_STRING(KviOption_stringNickCompletionPostfix));
			}
			replaceWordBeforeCursor(word,buffer,false);
			m_bLastCompletionFinished=0;
			// REPAINT CALLED FROM OUTSIDE!
		} else {
			m_bLastCompletionFinished=1;
			m_szLastCompletedNick = "";
		}
	} else {
		// Old session finished
		// re-extract
		//word = m_szTextBuffer.left(m_iCursorPosition);
		//getWordBeforeCursor(word,&bFirstWordInLine);
		if(word.isEmpty())return;
		if(m_pUserListView->completeNickStandard(word,"",buffer,bAddMask))
		{
			// completed
			m_szLastCompletionBuffer          = m_szTextBuffer;
			m_iLastCompletionCursorPosition   = m_iCursorPosition;
			m_iLastCompletionCursorXPosition  = m_iLastCursorXPosition;
			m_iLastCompletionFirstVisibleChar = m_iFirstVisibleChar;
			m_szLastCompletedNick             = buffer;
			if(!KVI_OPTION_STRING(KviOption_stringNickCompletionPostfix).isEmpty())
			{
				if(bFirstWordInLine || (!KVI_OPTION_BOOL(KviOption_boolUseNickCompletionPostfixForFirstWordOnly)))
					buffer.append(KVI_OPTION_STRING(KviOption_stringNickCompletionPostfix));
			}
			replaceWordBeforeCursor(word,buffer,false);
			m_bLastCompletionFinished=0;
			// REPAINT CALLED FROM OUTSIDE!
		} else {
			m_bLastCompletionFinished=1;
			m_szLastCompletedNick = "";
		}
	}
}


//Funky helpers

void KviInputEditor::end()
{
	m_iLastCursorXPosition = frameWidth();
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
	if(m_szTextBuffer.length() >= m_iMaxBufferSize)return;

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
	moveRightFirstVisibleCharToShowCursor();
	m_iCursorPosition++;
	repaintWithCursorOn();
}

void KviInputEditor::moveRightFirstVisibleCharToShowCursor()
{
	// :)
	QFontMetrics fm(KVI_OPTION_FONT(KviOption_fontInput));

	QChar c = m_szTextBuffer.at(m_iCursorPosition);

#ifdef COMPILE_USE_QT4
	m_iLastCursorXPosition += fm.width(c);
#else
	m_iLastCursorXPosition += (c.unicode() < 256) ? g_iInputFontCharWidth[c.unicode()] : fm.width(c);
#endif
	while(m_iLastCursorXPosition >= contentsRect().width()-2*KVI_INPUT_MARGIN)
	{
		c = m_szTextBuffer.at(m_iFirstVisibleChar);
#ifdef COMPILE_USE_QT4
		m_iLastCursorXPosition -= fm.width(c);
#else
		m_iLastCursorXPosition -= (c.unicode() < 256) ? g_iInputFontCharWidth[c.unicode()] : fm.width(c);
#endif
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

void KviInputEditor::selectOneChar(int pos)
{
	m_iSelectionBegin = pos;
	m_iSelectionEnd   = pos;
}

int KviInputEditor::charIndexFromXPosition(int xPos)
{
	int curXPos = frameWidth()+KVI_INPUT_MARGIN;
	int curChar = m_iFirstVisibleChar;
	int bufLen  = m_szTextBuffer.length();

	QFontMetrics fm(KVI_OPTION_FONT(KviOption_fontInput));
	while(curChar < bufLen)
	{
		QChar c = m_szTextBuffer.at(curChar);
#ifdef COMPILE_USE_QT4
		int widthCh = fm.width(c);
#else
		int widthCh = (c.unicode() < 256) ? g_iInputFontCharWidth[c.unicode()] : fm.width(c);
#endif
		if(xPos < (curXPos+(widthCh/2)))return curChar;
		else if(xPos < (curXPos+widthCh))return (curChar+1);
		{
			curXPos+=widthCh;
			curChar++;
		}
	}
	return curChar;
}

int KviInputEditor::xPositionFromCharIndex(int chIdx,bool bContentsCoords)
{
	// FIXME: this could use fm.width(m_szTextBuffer,chIdx)
	int curXPos = bContentsCoords ? KVI_INPUT_MARGIN : frameWidth()+KVI_INPUT_MARGIN;
	int curChar = m_iFirstVisibleChar;
	QFontMetrics fm(KVI_OPTION_FONT(KviOption_fontInput));
	while(curChar < chIdx)
	{
		QChar c = m_szTextBuffer.at(curChar);
#ifdef COMPILE_USE_QT4
		curXPos += fm.width(c);
#else
		curXPos += (c.unicode() < 256) ? g_iInputFontCharWidth[c.unicode()] : fm.width(c);
#endif
		curChar++;
	}
	return curXPos;
}

/*
	@doc: texticons
	@type:
		generic
	@title:
		The KVIrc TextIcons extension
	@short:
		The KVIrc TextIcons extension
	@body:
		Starting from version 3.0.0 KVIrc supports the TextIcon extension
		to the standard IRC protocol. It is a mean for sending text enriched
		of small images without sending the images themselves.[br]
		The idea is quite simple: the IRC client (and it's user) associates
		some small images to text strings (called icon tokens) and the strings are sent
		in place of the images preceeded by a special escape character.[br]
		The choosen escape character is 29 (hex 0x1d) which corresponds
		to the ASCII group separator.[br]
		So for example if a client has the association of the icon token "rose" with a small
		icon containing a red rose flower then KVIrc could send the string
		"&lt;0x1d&gt;rose" in the message stream to ask the remote parties to
		display such an icon. If the remote parties don't have this association
		then they will simply strip the control code and display the string "rose",
		(eventually showing it in some enchanced way).[br]
		The icon tokens can't contain spaces
		so the receiving clients stop the extraction of the icon strings
		when a space, an icon escape or the message termination is encountered.
		[br]
		&lt;icon escape&gt; := character 0x1d (ASCII group separator)[br]
		&lt;icon token&gt; := any character with the exception of 0x1d, CR,LF and SPACE.[br]
		[br]
		Please note that this is a KVIrc extension and the remote clients
		that don't support this feature will not display the icon (and will
		eventually show the 0x1d character in the data stream).[br]
		If you like this feature please either convince the remote users
		to try KVIrc or tell them to write to their client developers asking
		for this simple feature to be implemented.[br]
*/


/*
	@doc: commandline
	@title:
		The Commandline Input Features
	@type:
		generic
	@short:
		Commandline input features
	@body:
		[big]Principles of operation[/big]
		[p]
		The idea is simple: anything that starts with a slash (/) character
		is interpreted as a command. Anything else is plain text that is
		sent to the target of the window (channel, query, dcc chat etc..).
		[/p]
		[big]The two operating modes[/big]
		[p]
		The commandline input has two operating modes: the "user friendly mode" and
		the "kvs mode". In the user friendly mode all the parameters of the commands
		are interpreted exactly like you type them. There is no special interpretation
		of $,%,-,( and ; characters. This allows you to type "/me is happy ;)", for example.
		In the kvs mode the full parameter interpretation is enabled and the commands
		work just like in any other script editor. This means that anything that
		starts with a $ is a function call, anything that starts with a % is a variable,
		the dash characters after command names are interpreted as switches and ; is the
		command separator. This in turn does NOT allow you to type "/me is happy ;)"
		because ; is the command separator and ) will be interpreted as the beginning
		of the next command. In KVS mode you obviously have to escape the ; character
		by typing "/me is happy \;)". The user friendly mode is good for everyday chatting
		and for novice users while the KVS mode is for experts that know that minimum about
		scripting languages. Please note that in the user-friendly mode you're not allowed
		to type multiple commands at once :).
		[/p]
		[big]Default Key Bindings:[/big][br]
		Ctrl+B: Inserts the 'bold' mIRC text control character<br>
		Ctrl+K: Inserts the 'color' mIRC text control character<br>
		Ctrl+R: Inserts the 'reverse' mIRC text control character<br>
		Ctrl+U: Inserts the 'underline' mIRC text control character<br>
		Ctrl+O: Inserts the 'reset' mIRC text control character<br>
		Ctrl+P: Inserts the 'non-crypt' (plain text) KVIrc control character used to disable encryption of the current text line<br>
		Ctrl+C: Copies the selected text to clipboard<br>
		Ctrl+X: Cuts the selected text<br>
		Ctrl+V: Pastes the clipboard contents (same as middle mouse click)<br>
		Ctrl+I: Inserts the 'icon' control code and pops up the icon list box<br>
		Ctrl+A: Select all<br>
		CursorUp: Moves backward in the command history<br>
		CursorDown: Moves forward in the command history<br>
		CursorRight: Moves the cursor to the right<br>
		CursorLeft: Moves the cursor to the left :)<br>
		Shift+CursorLeft: Moves the selection to the left<br>
		Shift+RightCursor: Moves the selection to the right<br>
		Ctrl+CursorLeft: Moves the cursor one word left<br>
		Ctrl+CursorRight: Moves the cursor one word right<br>
		Ctrl+Shift+CursorLeft: Moves the selection one word left<br>
		Ctrl+Shift+CursorRight: Moves the selection one word right<br>
		Tab: Nickname, function/command, or filename completion (see below)<br>
		Shift+Tab: Hostmask or function/command completion (see below)<br>
		Alt+&lt;numeric_sequence&gt;: Inserts the character by ASCII/Unicode code<br>
		<example>
		Alt+32: Inserts ASCII/Unicode character 32: ' ' (a space)
		Alt+00032: Same as above :)
		Alt+13: Inserts the Carriage Return (CR) control character
		Alt+77: Inserts ASCII/Unicode character 77: 'M'
		Alt+23566: Inserts Unicode character 23566 (an ideogram)
		</example>
		Also look at the <a href="shortcuts.kvihelp">global shortcuts</a> reference.<br>
		If you drop a file on this widget, a <a href="parse.kvihelp">/PARSE &lt;filename&gt;</a> will be executed.<br>
		You can enable word substitution in the preferences dialog.<br>
		For example, if you choose to substitute "afaik" with "As far as I know",<br>
		when you will type "afaik" somewhere in the command line, and then
		press Space or Return, that word will be replaced with "As far as I know".<br>
		Experiment with it :)<br>
		The Tab key activates the completion of the current word.<br>
		If a word is prefixed with a '/', it is treated as a command to be completed,
		if it begins with '$', it is treated as a function or identifier to be completed,
		otherwise it is treated as a nickname or filename to be completed.<br>
		<example>
			/ec&lt;Tab&gt; will produce /echo&lt;space&gt
			/echo $loca&lt;Tab&gt; will produce /echo $localhost
		</example>
		Multiple matches are listed in the view window and the word is completed
		to the common part of all the matches.<br>
		<example>
			$sel&lt;Tab;&gt; will find multiple matches and produce $selected
		</example>
		Experiment with that too :)
*/



KviInput::KviInput(KviWindow *par,KviUserListView * view)
: QWidget(par,"input")
{
	QBoxLayout* pLayout=new QHBoxLayout(this);
	pLayout->setAutoAdd(true);
	pLayout->setDirection(QBoxLayout::RightToLeft);

	pLayout->setMargin(0);
	pLayout->setSpacing(0);

	m_pWindow = par;
	m_pMultiLineEditor = 0;
	
	m_pHideToolsButton = new KviStyledToolButton(this,"hide_container_button");
	
	m_pHideToolsButton->setUsesBigPixmap(false);
	m_pHideToolsButton->setFixedWidth(10);

	if(g_pIconManager->getBigIcon("kvi_horizontal_left.png"))
		m_pHideToolsButton->setPixmap(*(g_pIconManager->getBigIcon("kvi_horizontal_left.png")));
	
	connect(m_pHideToolsButton,SIGNAL(clicked()),this,SLOT(toggleToolButtons()));
	
	m_pButtonContainer=new KviTalHBox(this);
	m_pButtonContainer->setSpacing(0);

#ifdef COMPILE_USE_QT4
	m_pButtonContainer->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Preferred));
//	if(m_pButtonContainer->layout())
//		m_pButtonContainer->layout()->setSizeConstraint(QLayout::SetMinimumSize);
#endif

	m_pHistoryButton = new KviStyledToolButton(m_pButtonContainer,"historybutton");
	m_pHistoryButton->setUsesBigPixmap(false);
	//m_pHistoryButton->setUpdatesEnabled(TRUE); ???
	QIconSet is1;
	if(!KVI_OPTION_BOOL(KviOption_boolDisableInputHistory))//G&N mar 2005
	{
		is1.setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TIME)),QIconSet::Small);
		m_pHistoryButton->setIconSet(is1);
		KviTalToolTip::add(m_pHistoryButton,__tr2qs("Show History<br>&lt;Ctrl+PageUp&gt;"));
		connect(m_pHistoryButton,SIGNAL(clicked()),this,SLOT(historyButtonClicked()));
	}
	else
	{
		is1.setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_QUITSPLIT)),QIconSet::Small);
		m_pHistoryButton->setIconSet(is1);
		KviTalToolTip::add(m_pHistoryButton,__tr2qs("Input History Disabled"));
	}

	m_pIconButton = new KviStyledToolButton(m_pButtonContainer,"iconbutton");
	m_pIconButton->setUsesBigPixmap(false);
	QIconSet is3;
	is3.setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_BIGGRIN)),QIconSet::Small);
	m_pIconButton->setIconSet(is3);
	KviTalToolTip::add(m_pIconButton,__tr2qs("Show Icons Popup<br>&lt;Ctrl+I&gt;<br>See also /help texticons"));

	connect(m_pIconButton,SIGNAL(clicked()),this,SLOT(iconButtonClicked()));


	m_pCommandlineModeButton = new KviStyledToolButton(m_pButtonContainer,"commandlinemodebutton");
	m_pCommandlineModeButton->setUsesBigPixmap(false);
	m_pCommandlineModeButton->setToggleButton(true);
	QIconSet is0;
	is0.setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SAYSMILE)),QIconSet::Small,QIconSet::Normal,QIconSet::On);
	is0.setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SAYKVS)),QIconSet::Small,QIconSet::Normal,QIconSet::Off);
	m_pCommandlineModeButton->setIconSet(is0);
	KviTalToolTip::add(m_pCommandlineModeButton,__tr2qs("User friendly commandline mode<br>See also /help commandline"));
	if(KVI_OPTION_BOOL(KviOption_boolCommandlineInUserFriendlyModeByDefault))
		m_pCommandlineModeButton->setOn(true);


	m_pMultiEditorButton = new KviStyledToolButton(m_pButtonContainer,"multieditorbutton");
	m_pMultiEditorButton->setToggleButton(true);
	m_pMultiEditorButton->setUsesBigPixmap(false);
	QIconSet is2;
	is2.setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TERMINAL)),QIconSet::Small,QIconSet::Normal,QIconSet::On);
	is2.setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TERMINAL)),QIconSet::Small,QIconSet::Normal,QIconSet::Off);
	m_pMultiEditorButton->setIconSet(is2);
	KviTalToolTip::add(m_pMultiEditorButton,__tr2qs("Multi-line Editor<br>&lt;Alt+Backspace&gt;"));

	connect(m_pMultiEditorButton,SIGNAL(toggled(bool)),this,SLOT(multilineEditorButtonToggled(bool)));
	
	m_pInputEditor = new KviInputEditor(this,par,view);
	connect(m_pInputEditor,SIGNAL(enterPressed()),this,SLOT(inputEditorEnterPressed()));
#ifdef COMPILE_USE_QT4
	m_pInputEditor->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Ignored));
#else
	m_pInputEditor->setSizePolicy(QSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored));
#endif
	
	pLayout->setStretchFactor(m_pInputEditor,100000);
	pLayout->setStretchFactor(m_pButtonContainer,0);
	pLayout->setStretchFactor(m_pHideToolsButton,0);
}

KviInput::~KviInput()
{
	if(m_pMultiLineEditor)KviScriptEditor::destroyInstance(m_pMultiLineEditor);
}

bool KviInput::isButtonsHidden()
{
	return m_pButtonContainer->isHidden();
}

void KviInput::setButtonsHidden(bool bHidden)
{
	if(!m_pHideToolsButton || !m_pButtonContainer) return;
	if(bHidden==m_pButtonContainer->isHidden()) return;
	m_pButtonContainer->setHidden(bHidden);
	QPixmap* pix= bHidden ? 
		g_pIconManager->getBigIcon("kvi_horizontal_right.png") :
		g_pIconManager->getBigIcon("kvi_horizontal_left.png");
	if(pix)
		m_pHideToolsButton->setPixmap(*pix);
}

void KviInput::toggleToolButtons()
{
	setButtonsHidden(!isButtonsHidden());
}

void KviInput::inputEditorEnterPressed()
{
	QString szText = m_pInputEditor->text();
	KviUserInput::parse(szText,m_pWindow,QString::null,m_pCommandlineModeButton->isOn());
	m_pInputEditor->setText("");
}

void KviInput::keyPressEvent(QKeyEvent *e)
{
	//debug("KviInput::keyPressEvent(key:%d,state:%d,text:%s)",e->key(),e->state(),e->text().isEmpty() ? "empty" : e->text().utf8().data());

	if(e->state() & Qt::ControlButton)
	{
		switch(e->key())
		{
			case Qt::Key_Backspace:
				if(m_pMultiLineEditor)
					multilineEditorButtonToggled(!m_pMultiLineEditor);
			break;
		}
	}

	if(e->state() & Qt::ControlButton)
	{
		switch(e->key())
		{
			case Qt::Key_Enter:
			case Qt::Key_Return:
			{
				if(m_pMultiLineEditor)
				{
					QString szText;
					m_pMultiLineEditor->getText(szText);
					if(szText.isEmpty())return;
					if(KVI_OPTION_BOOL(KviOption_boolWarnAboutPastingMultipleLines))
					{
						if(szText.length() > 256)
						{
							if(szText[0] != '/')
							{
#ifdef COMPILE_USE_QT4
								int nLines = szText.count('\n') + 1;
#else
								int nLines = szText.contains('\n') + 1;
#endif
								if(nLines > 15)
								{
									int nRet = QMessageBox::question(
										this,
										__tr2qs("Confirm Multiline Message"),
										__tr2qs("You're about to send a message with %1 lines of text.<br><br>" \
												"There is nothing wrong with it, this warning is<br>" \
												"here to prevent you from accidentally sending<br>" \
												"a really large message just because you didn't edit it<br>" \
												"properly after pasting text from the clipboard.<br><br>" \
												"Do you want the message to be sent?").arg(nLines),
										__tr2qs("Yes, always"),
										__tr2qs("Yes"),
										__tr2qs("No"),
										1,2);
									switch(nRet)
									{
										case 0:
											KVI_OPTION_BOOL(KviOption_boolWarnAboutPastingMultipleLines) = false;
										break;
										case 2:
											return;
										break;
										default: // also case 1
										break;
									}
								}
							}
						}
					}
					KviUserInput::parse(szText,m_pWindow,QString::null,m_pCommandlineModeButton->isOn());
					m_pMultiLineEditor->setText("");
				}
			}
			break;
			case Qt::Key_PageUp:
				historyButtonClicked();
			break;
		}
	}
}

void KviInput::multiLinePaste(const QString &text)
{
	if(!m_pMultiLineEditor)multilineEditorButtonToggled(true);
	m_pMultiLineEditor->setText(text);
}

void KviInput::multilineEditorButtonToggled(bool bOn)
{
	if(m_pMultiLineEditor)
	{
		if(bOn)return;
		KviScriptEditor::destroyInstance(m_pMultiLineEditor);
		m_pMultiLineEditor = 0;
		m_pInputEditor->show();
		m_pWindow->childrenTreeChanged(0);
		m_pInputEditor->setFocus();
		m_pMultiEditorButton->setOn(false);
	} else {
		if(!bOn)return;
		m_pMultiLineEditor = KviScriptEditor::createInstance(this);
		m_pMultiLineEditor->setFindText(__tr2qs("<Ctrl+Return>; submits, <Alt+Backspace>; hides this editor"));
		m_pMultiLineEditor->setFindLineeditReadOnly(true);
		m_pInputEditor->hide();
		m_pMultiLineEditor->show();
		m_pWindow->childrenTreeChanged(m_pMultiLineEditor);
		m_pMultiLineEditor->setFocus();
		m_pMultiEditorButton->setOn(true);
	}
}

void KviInput::iconButtonClicked()
{
	if(!g_pTextIconWindow)g_pTextIconWindow = new KviTextIconWindow();
	QPoint pnt = m_pIconButton->mapToGlobal(QPoint(m_pIconButton->width(),0));
	g_pTextIconWindow->move(pnt.x()-g_pTextIconWindow->width(),pnt.y() - g_pTextIconWindow->height());
	g_pTextIconWindow->popup(this,true);
}

void KviInput::historyButtonClicked()
{
	if(!g_pHistoryWindow)g_pHistoryWindow = new KviHistoryWindow();

	QPoint pnt = mapToGlobal(QPoint(0,0));

	g_pHistoryWindow->setGeometry(pnt.x(),pnt.y() - KVI_HISTORY_WIN_HEIGHT,width(),KVI_HISTORY_WIN_HEIGHT);
	g_pHistoryWindow->popup(this);
}

#define BUTTON_WIDTH 20

/*void KviInput::resizeEvent(QResizeEvent *e)
{
	//m_pButtonContainer
	m_pInputEditor->setGeometry(0,0,m_pButtonContainer->isVisible() ? width() - (BUTTON_WIDTH * 4)-10 : width() - 10,height());
	if(m_pMultiLineEditor)m_pMultiLineEditor->setGeometry(0,0,m_pButtonContainer->isVisible() ? width() - (BUTTON_WIDTH * 4)-10 : width() - 10,height());
	if(m_pButtonContainer->isVisible()) m_pButtonContainer->setGeometry(width() - (BUTTON_WIDTH * 4)-10,0,BUTTON_WIDTH*4,height());
		
	m_pHideToolsButton->setGeometry(width() - 10,0,10,height());
	
	QWidget::resizeEvent(e);
}*/

void KviInput::setFocus()
{
	// redirect setFocus() to the right children
	if(m_pMultiLineEditor)m_pMultiLineEditor->setFocus();
	else m_pInputEditor->setFocus();
}

void KviInput::focusInEvent(QFocusEvent * e)
{
	// if we get a focus in event , redirect the focus to the children
	if(m_pMultiLineEditor)m_pMultiLineEditor->setFocus();
	else m_pInputEditor->setFocus();
}


int KviInput::heightHint() const
{
	return m_pMultiLineEditor ? 120 : m_pInputEditor->heightHint();
}

void KviInput::setText(const QString &text)
{
	// FIXME: Latin1 -> QString ?
	if(m_pMultiLineEditor)m_pMultiLineEditor->setText(text);
	else m_pInputEditor->setText(text);
}

void KviInput::insertChar(char c)
{
	m_pInputEditor->insertChar(c);
}

void KviInput::insertText(const QString& text)
{
	m_pInputEditor->insertText(text);
}

void KviInput::applyOptions()
{
	if(KVI_OPTION_BOOL(KviOption_boolDisableInputHistory))//G&N mar 2005
	{
		QIconSet is1;
		is1.setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_QUITSPLIT)),QIconSet::Small);
		m_pHistoryButton->setIconSet(is1);
		KviTalToolTip::add(m_pHistoryButton,__tr2qs("Input History Disabled"));
		m_pHistoryButton->disconnect(SIGNAL(clicked()));
	}

	if(!KVI_OPTION_BOOL(KviOption_boolDisableInputHistory))
	{
		QIconSet is1;
		is1.setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TIME)),QIconSet::Small);
		m_pHistoryButton->setIconSet(is1);
		KviTalToolTip::add(m_pHistoryButton,__tr2qs("Show History<br>&lt;Ctrl+PageUp&gt;"));
		connect(m_pHistoryButton,SIGNAL(clicked()),this,SLOT(historyButtonClicked()));
	}

	m_pInputEditor->applyOptions();
}

void KviInput::setFocusProxy(QWidget *)
{
	/* do nothing */
}

//const QString & KviInput::text()
QString KviInput::text()
{
	QString szText;
	if(m_pMultiLineEditor)
		m_pMultiLineEditor->getText(szText);
	else
		szText=m_pInputEditor->text();
	return szText; 
}

#include "kvi_input.moc"