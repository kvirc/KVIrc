//==================================================================================
//
//   File : notifierwindow.cpp
//   Creation date : Tue Jul 6 2004 20:25:12 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2004 Iacopo Palazzi < iakko(at)siena.linux.it >
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the linkss of the GNU General Public License
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
//==================================================================================
#include "kvi_settings.h"

#ifdef COMPILE_USE_QT4
	#define QPopupMenu Q3PopupMenu
	#define QSimpleRichText Q3SimpleRichText

	#include <q3popupmenu.h>
	#include <q3simplerichtext.h>
#else
	#include <qpopupmenu.h>
	#include <qsimplerichtext.h>
#endif

	#include <qpainter.h>

#include "notifiermessage.h"
#include "notifierwindow.h"

#include "kvi_iconmanager.h"
#include "kvi_config.h"
#include "kvi_app.h"
#include "kvi_window.h"

#include "kvi_locale.h"
#include "kvi_frame.h"
#include "kvi_mirccntrl.h"
#include "kvi_options.h"
#include "kvi_userinput.h"

#include <qapplication.h>
#include <qimage.h>
#include <qdesktopwidget.h>
#include <qtooltip.h>
#include <qevent.h>
#include <qpen.h>
#include <qfontmetrics.h>    
#include <qregexp.h> 

#include "notifierwindowbody.h"
#include "notifierwindowborder.h"
#include "notifierwindowprogressbar.h"
#include "notifierwindowtabs.h"

extern KviNotifierWindow * g_pNotifierWindow;

KviNotifierWindow::KviNotifierWindow()
: QWidget(0,"kvirc_notifier_window",
#ifdef COMPILE_ON_WINDOWS
			Qt::WStyle_Customize |
			Qt::WStyle_NoBorder |
			Qt::WStyle_Tool |
			Qt::WStyle_StaysOnTop)
#else
			Qt::WStyle_Customize |
			Qt::WStyle_NoBorder |
#ifndef Q_OS_MACX
			Qt::WStyle_Tool |
			Qt::WX11BypassWM |
#endif
			Qt::WStyle_StaysOnTop)
#endif
{

	g_pNotifierWindow = this;

	m_eState = Hidden;
	m_dOpacity = 0.0;
	m_pShowHideTimer = 0;
	m_pBlinkTimer = 0;
	m_tAutoHideAt = 0;
	m_tStartedAt=0;
	m_pAutoHideTimer = 0;
	m_pProgressTimer = 0;
	
	m_pWndBorder = new KviNotifierWindowBorder();

	reloadImages();

	KviStr buffer;
	g_pApp->getReadOnlyConfigPath(buffer,"libkvinotifier.kvc",KviApp::ConfigPlugins,true);

	KviConfig cfg(buffer.ptr(),KviConfig::Read);

	cfg.setGroup("NotifierSkin");
	
	QString szFamily = cfg.readEntry("TextFontFamily","Arial");
	m_pDefaultFont = new QFont(szFamily,cfg.readIntEntry("TextFontSize",9));
	szFamily = cfg.readEntry("TitleFontFamily","Arial");
	m_pTitleFont = new QFont(szFamily,cfg.readIntEntry("TitleFontSize",9));

	m_clrCurText = cfg.readColorEntry("CurTextColor",QColor(40,0,0));
	m_clrHistoricText = cfg.readColorEntry("HistoricTextColor",QColor(125,125,125));
	m_clrTitle = QColor(255,255,255); //cfg.readColorEntry("TitleColor",QColor(0,0,0));
	for(int i=0;i<NUM_OLD_COLORS;i++)
	{
		KviStr szKey;
		szKey.sprintf("OldTextColor%d",i + 1);
		m_clrOldText[i] = cfg.readColorEntry(szKey.ptr(),QColor(90,90,90));
	}
	
	m_iBlinkTimeout = cfg.readIntEntry("BlinkTimeout",650);
	if(m_iBlinkTimeout < 100)m_iBlinkTimeout = 100;
	m_iInputHeight = cfg.readIntEntry("InputHeight",20);
	if(m_iInputHeight < 10)m_iInputHeight = 10;

	setBackgroundMode(Qt::NoBackground);
#ifdef COMPILE_USE_QT4
	setFocusPolicy(Qt::NoFocus);
#else
	setFocusPolicy(QWidget::NoFocus);
#endif
	setMouseTracking(true);
	//setCursor(m_cursor);
	
	hide();
	m_pLineEdit = new QLineEdit(this);
	m_pLineEdit->setGeometry(0,0,0,0);
	m_pLineEdit->hide();
	m_pLineEdit->installEventFilter(this);
	connect(m_pLineEdit,SIGNAL(returnPressed()),this,SLOT(returnPressed()));

	szFamily = cfg.readEntry("InputFontFamily","Arial");
	m_pLineEdit->setFont(QFont(szFamily,cfg.readIntEntry("InputFontSize",9)));

	m_bBlinkOn = false;

	m_bCloseDown = false;
	m_bPrevDown = false;
	m_bNextDown = false;
	m_bWriteDown = false;
	
	m_bKeyShiftPressed = false;
	
	m_bLeftButtonIsPressed = false;
	m_bDiagonalResizing = false;
	m_bResizing = false;
	
	m_pContextPopup = 0;
	m_pDisablePopup = 0;

	m_bDragging = false;

	m_bDisableHideOnMainWindowGotAttention = false;

	// Positioning the notifier bottom-right
	int iWidth = m_pixBackground.width();
	int iHeight = m_pixBackground.height();
	QDesktopWidget * w = QApplication::desktop();
	QRect r = w->availableGeometry(w->primaryScreen()); //w->screenGeometry(w->primaryScreen());
	
	/*debug("r.x(),r.y(): %d,%d",r.x(),r.y());
	debug("r.width(),r.height(): %d,%d",r.width(),r.height());*/

	m_wndRect.setRect( r.x() + r.width() - (iWidth + SPACING), r.y() + r.height() - (iHeight + SPACING), iWidth, iHeight );
	
	//m_pWndBorder = new KviNotifierWindowBorder();
	m_pWndTabs = new KviNotifierWindowTabs(m_pWndBorder->tabsRect());
	m_pWndBody = new KviNotifierWindowBody(m_pWndBorder->bodyRect());
	
	m_pProgressBar = new KviNotifierWindowProgressBar(m_pWndBorder->progressRect());
	
	// FIXME: Ugly global-font initialization..
	QFontMetrics titleFontMetrics(*m_pTitleFont);
	m_pWndBorder->centerTitle(titleFontMetrics.height());
	
	connect(g_pApp,SIGNAL(reloadImages()),this,SLOT(reloadImages()));
}

KviNotifierWindow::~KviNotifierWindow()
{
	stopShowHideTimer();
	stopBlinkTimer();
	stopAutoHideTimer();
	delete m_pDefaultFont;
	delete m_pTitleFont;
	delete m_pWndBorder;
	delete m_pWndTabs;
	delete m_pWndBody;
}

int KviNotifierWindow::countTabs() {
	return m_pWndTabs->count();
}

void KviNotifierWindow::reloadImages()
{
	m_pixBackground.resize(WDG_MIN_WIDTH,WDG_MIN_HEIGHT);
	m_pixBackground.fill();
	m_pixBackgroundHighlighted.resize(m_pixBackground.size());
	m_pixBackgroundHighlighted.fill();

	m_pWndBorder->resize(m_pixBackground.size());
}


void KviNotifierWindow::addMessage(KviWindow * pWnd,const QString &szImageId,const QString &szText,unsigned int uMessageTime)
{
	QPixmap * pIcon;
	QString szMessage=szText;
	szMessage.replace( QRegExp("\r([^\r])*\r([^\r])+\r"), "\\2" );
	if(szImageId.isEmpty())pIcon = 0;
	else pIcon = g_pIconManager->getImage(szImageId);

	KviNotifierMessage * m = new KviNotifierMessage(this,pIcon ? new QPixmap(*pIcon) : 0,szMessage);

	m_pWndTabs->addMessage(pWnd, m); // Adds a tab about the sender pWnd

	if(!isActiveWindow())
		startBlinking();

	if(uMessageTime > 0)
	{
		kvi_time_t tAutoHide = kvi_unixTime() + uMessageTime;
		if(m_tAutoHideAt < tAutoHide)
		{
			m_tAutoHideAt = tAutoHide;
			if(m_eState == Visible)startAutoHideTimer();
		}
	} else {
		// never hide
		stopAutoHideTimer();
		m_tAutoHideAt = 0;
	}

	if(pWnd)
	{
		if(pWnd->hasAttention())m_bDisableHideOnMainWindowGotAttention = true;
	}

	//m_pWndTabs->setCurrentMessage(m);
	if(isVisible())update();
	// else the line edit was hidden
}
	

int KviNotifierWindow::textWidth() {
	return m_pWndBody->textRect().width() - 20;
};

void KviNotifierWindow::stopShowHideTimer()
{
	if(!m_pShowHideTimer)return;
	debug ("Deleting show/hide timer");
	delete m_pShowHideTimer;
	m_pShowHideTimer = 0;
}

void KviNotifierWindow::stopBlinkTimer()
{
	if(!m_pBlinkTimer)return;
	delete m_pBlinkTimer;
	m_pBlinkTimer = 0;
}

void KviNotifierWindow::stopAutoHideTimer()
{
	if(m_pProgressTimer) delete m_pProgressTimer;
	m_pProgressTimer = 0;
	
	if(!m_pAutoHideTimer)return;
	delete m_pAutoHideTimer;
	m_pAutoHideTimer = 0;
	m_pProgressBar->setProgress(0);
}

void KviNotifierWindow::computeRect()
{
	int iWidth = m_pixBackground.width();
	int iHeight = m_pixBackground.height();
	m_pWndBorder->setGeometry(QPoint(x(),y()), QSize(iWidth,iHeight));
	update();
}

void KviNotifierWindow::doShow(bool bDoAnimate)
{
	if(KVI_OPTION_BOOL(KviOption_boolForciblyDisableNotifier))return;

	kvi_time_t tNow = kvi_unixTime();
	if(g_tNotifierDisabledUntil > tNow)return;
	g_tNotifierDisabledUntil = 0;

	switch(m_eState)
	{
		case Showing:
			// already showing up
			return;
		break;
		case Visible:
			// already visible
			return;
		break;
		case Hiding:
			// ops.. hiding!
			m_eState = Showing;
		break;
		case Hidden:
			stopShowHideTimer();
			stopBlinkTimer();
		
			m_bDragging = false;
			m_bCloseDown = false;
			m_bPrevDown = false;
			m_bNextDown = false;
			m_bWriteDown = false;
			m_bBlinkOn = false;
		#if (!defined COMPILE_USE_QT4) || !(defined(COMPILE_USE_QT4) && (defined(COMPILE_ON_WINDOWS) || defined(Q_OS_MACX)))
			m_imgDesktop = QPixmap::grabWindow(
						QApplication::desktop()->winId(),
						m_wndRect.x(),
						m_wndRect.y(),
						m_wndRect.width(),
						m_wndRect.height()).convertToImage();
		#endif
			// QPixmap tmp = QPixmap::grabWindow( QApplication::desktop()->winId(),m_wndRect.x(), m_wndRect.y(), m_wndRect.width(), m_wndRect.height());
			// tmp.save("/root/pix.png","PNG");
			m_pixForeground.resize(m_pixBackground.size());
			m_imgBuffer.create(m_pixBackground.width(),m_pixBackground.height(),32);
			if(bDoAnimate)
			{
				m_pShowHideTimer = new QTimer();
				connect(m_pShowHideTimer,SIGNAL(timeout()),this,SLOT(heartbeat()));
				m_dOpacity = OPACITY_STEP;
				m_eState = Showing;
				m_bCrashShowWorkAround=true;
				#if defined(COMPILE_USE_QT4) && (defined(COMPILE_ON_WINDOWS) || defined(Q_OS_MACX)) 
				setWindowOpacity(m_dOpacity);
				#endif	
				show();
				#if defined(COMPILE_USE_QT4) && (defined(COMPILE_ON_WINDOWS) || defined(Q_OS_MACX)) 
				m_pShowHideTimer->start(40);
				#else
				m_pShowHideTimer->start(100);
				#endif
				computeRect();
				m_bCrashShowWorkAround=false;
				//show();

			} else {
				m_dOpacity = 1.0;
				m_eState = Visible;
				show();
				startBlinking();
				startAutoHideTimer();
			}
		break;
	}
}

bool KviNotifierWindow::shouldHideIfMainWindowGotAttention()
{
	if(m_bDisableHideOnMainWindowGotAttention)return false;
	KviNotifierWindowTab * t = m_pWndTabs->currentTab();
	if(!t)return false;
	if(!t->window())return false;
	return t->window()->hasAttention();
}


void KviNotifierWindow::heartbeat()

{
	switch(m_eState)
	{
		case Hidden:
			hideNow();
		break;
		case Visible:
			stopShowHideTimer();
			m_dOpacity = 1.0;
			if(!isVisible())show(); //!!!
		break;
		case Showing:
			// if the main window got attention while
			// showing up then just hide now
			if(shouldHideIfMainWindowGotAttention())
			{
				m_eState = Hiding;
			} else {
				m_dOpacity += OPACITY_STEP;
				if(m_dOpacity >= 1.0)
				{
					m_dOpacity = 1.0;
					m_eState = Visible;
					stopShowHideTimer();
					startBlinking();
					startAutoHideTimer();
				}

				if(!isVisible())show(); //!!!
				if(m_pLineEdit->isVisible())m_pLineEdit->hide();
				#if defined(COMPILE_USE_QT4) && (defined(COMPILE_ON_WINDOWS) || defined(Q_OS_MACX)) 
				setWindowOpacity(m_dOpacity);
				#endif
				update();

			}
		break;
		case Hiding:
			m_dOpacity -= OPACITY_STEP;
			#if defined(COMPILE_USE_QT4) && (defined(COMPILE_ON_WINDOWS) || defined(Q_OS_MACX)) 
				setWindowOpacity(m_dOpacity);
			#endif
			if(m_pLineEdit->isVisible())m_pLineEdit->hide();
			if(m_dOpacity <= 0.0)hideNow();
			else update();
		break;
	}
}

void KviNotifierWindow::hideNow()
{
	stopBlinkTimer();
	stopShowHideTimer();
	stopAutoHideTimer();
	m_eState = Hidden;
	m_dOpacity = 0.0;
	m_tAutoHideAt = 0;
	
	if(m_pLineEdit->isVisible())m_pLineEdit->hide();
	hide();//if(isVisible())hide();
}

void KviNotifierWindow::doHide(bool bDoAnimate)
{
	stopAutoHideTimer();
	switch(m_eState)
	{
	case Hiding:
		// already hiding up
		if(!bDoAnimate)hideNow();
		return;
		break;
	case Hidden:
		// already hidden
		if(isVisible())hideNow(); // !!!
		return;
		break;
	case Showing:
		// ops.. hiding!
		if(!bDoAnimate)hideNow();
		else {
			// continue animating, but hide
			m_eState = Hiding;
		}
		break;
	case Visible:
		stopBlinkTimer();
		stopShowHideTimer();
		if(m_pLineEdit->isVisible())m_pLineEdit->hide();
		if((!bDoAnimate) || (x() != m_pWndBorder->x()) || (y() != m_pWndBorder->y()))
		{

			debug ("just hide quickly with notifier x() %d and notifier y() % - WBorderx() %d and WBordery() %d and bDoanimate %d",x(),y(),m_pWndBorder->x(),m_pWndBorder->y(),bDoAnimate);
			// the user asked to not animate or
			// the window has been moved and the animation would suck anyway
			// just hide quickly
			hideNow();
		} else {
			debug ("starting hide animation notifier x() %d and notifier y() % - WBorderx() %d and WBordery() %d and bDoanimate %d",x(),y(),m_pWndBorder->x(),m_pWndBorder->y(),bDoAnimate);
			m_pShowHideTimer = new QTimer();
			connect(m_pShowHideTimer,SIGNAL(timeout()),this,SLOT(heartbeat()));
			m_dOpacity = 1.0 - OPACITY_STEP;
			m_eState = Hiding;
			#if defined(COMPILE_USE_QT4) && (defined(COMPILE_ON_WINDOWS) || defined(Q_OS_MACX)) 
			setWindowOpacity(m_dOpacity);
			update();
			m_pShowHideTimer->start(40);
			#else
			update();		
			m_pShowHideTimer->start(100);
			#endif
			

		}
		break;
	}
}

void KviNotifierWindow::showEvent(QShowEvent *e)
{
	computeRect();
	setGeometry(m_wndRect); // w_win = w_wndRect
	
}

void KviNotifierWindow::hideEvent(QHideEvent * e)
{
	if (m_bCrashShowWorkAround) return;
	stopBlinkTimer();
	stopShowHideTimer();
	stopAutoHideTimer();
	m_eState = Hidden; // make sure it's true
	m_tAutoHideAt = 0; // make sure it's true
	markAllMessagesAsHistoric();
	m_bDisableHideOnMainWindowGotAttention = false;
}

void KviNotifierWindow::startBlinking()
{
	stopBlinkTimer();
	m_bBlinkOn = false;
	

	if(!KVI_OPTION_BOOL(KviOption_boolDisableNotifierFlashing))
	{
		m_pBlinkTimer = new QTimer();
		connect(m_pBlinkTimer,SIGNAL(timeout()),this,SLOT(blink()));
		m_iBlinkCount = 0;
		m_pBlinkTimer->start(m_iBlinkTimeout);
	}
}

void KviNotifierWindow::startAutoHideTimer()
{
	stopAutoHideTimer();
	m_tStartedAt = kvi_unixTime();
	m_qtStartedAt.start();
	if(m_tAutoHideAt <= m_tStartedAt)return;

	int nSecs = m_tAutoHideAt - m_tStartedAt;
	if(nSecs < 5)nSecs = 5;
	m_pAutoHideTimer = new QTimer();
	connect(m_pAutoHideTimer,SIGNAL(timeout()),this,SLOT(autoHide()));
	m_pAutoHideTimer->start(nSecs * 1000);
	
	m_pProgressTimer= new QTimer();
	connect(m_pProgressTimer,SIGNAL(timeout()),this,SLOT(progressUpdate()));
	m_pProgressTimer->start(60);
}

void KviNotifierWindow::autoHide()
{
	m_tAutoHideAt = 0;
	stopAutoHideTimer();
	doHide(true);
}

void KviNotifierWindow::blink()
{
	m_iBlinkCount++;
	m_bBlinkOn = !m_bBlinkOn;

	if(m_iBlinkCount > 100)
	{
		// stop blinking at a certain point and remain highlighted
		m_bBlinkOn = true;
		stopBlinkTimer();
	} else {
		// if the main window got attention while
		// showing up then just hide now
		if(shouldHideIfMainWindowGotAttention())
		{
			doHide(false); // don't animate: the background has almost surely changed
			return;
		}
	}
	update(); 
}

void KviNotifierWindow::markAllMessagesAsHistoric()
{
	if (!m_pWndTabs) return;
	m_pWndTabs->markAllMessagesAsHistoric();
}


static void blend_images(QImage &buffer,QImage &background,QImage &foreground,double dValue)
{
	for(int y = 0;y < buffer.height();y++)
	{
		QRgb * dst = (QRgb *)buffer.scanLine(y);
		QRgb * end = dst + buffer.width();
		QRgb * bkg = (QRgb *)background.scanLine(y);
		QRgb * fgn = (QRgb *)foreground.scanLine(y);
	
		double dRemaining = 1.0 - dValue;

		while(dst < end)
		{
			*dst = qRgb(
						(int)((qRed(*bkg) * dRemaining) + (qRed(*fgn) * dValue)),
						(int)((qGreen(*bkg) * dRemaining) + (qGreen(*fgn) * dValue)),
						(int)((qBlue(*bkg) * dRemaining) + (qBlue(*fgn) * dValue))
					);
			dst++;
			bkg++;
			fgn++;
		}

	}
}

void KviNotifierWindow::paintEvent(QPaintEvent * e)
{

/*
	if(m_bBlinkOn)
		bitBlt(&m_pixForeground,QPoint(0,0),&m_pixBackgroundHighlighted);
	else
		bitBlt(&m_pixForeground,QPoint(0,0),&m_pixBackground);

	QPainter p(&m_pixForeground);
*/
	redrawWindow();
	redrawText();
	if(m_dOpacity < 1.0)
	{
			QPainter px(this);
	
	#if defined(COMPILE_USE_QT4) && (defined(COMPILE_ON_WINDOWS) || defined(Q_OS_MACX))
		px.drawPixmap(0,0,m_pixForeground);
	#else
		QImage temp_image = m_pixForeground.convertToImage();
		blend_images(m_imgBuffer,m_imgDesktop,temp_image,m_dOpacity);
		px.drawImage(0,0,m_imgBuffer);
	#endif
		px.end();
	} else {
#ifdef COMPILE_USE_QT4
		QPainter px(this);
		px.drawPixmap(0,0,m_pixForeground);
#else
		bitBlt(this,QPoint(0,0),&m_pixForeground);
#endif
	}
}

void KviNotifierWindow::redrawText()
{
	QPainter p(&m_pixForeground);

	// the current tab
	KviNotifierWindowTab * tab = m_pWndTabs->currentTab();
	if(!tab)return;
	
	// it's message list
	KviPtrList<KviNotifierMessage> * l = tab->messageList();
	if(!l)return;
	if(l->isEmpty())return;
	
	// the current message
	KviNotifierMessage * cur = tab->currentMessage();
	if(!cur)cur = l->last();

	// this is useful in several places
	KviNotifierMessage * last = l->last();

	// make sure that we can loop
	int iIdx = l->findRef(cur);
	if(iIdx == -1)
	{
		tab->setCurrentMessage(last);
		cur = last;
		iIdx = l->findRef(cur);
	}

	int y = m_pWndBody->textRect().y() + m_pWndBody->textRect().height();

	if(m_pLineEdit->isVisible())y -= (m_pLineEdit->height() + 4);

	QColorGroup grp = colorGroup();

	int idx = iIdx;
	KviNotifierMessage * m = cur;
	while(m && (y > m_pWndBody->textRect().y()))
	{
		int iHeight = m->text()->height();
		if(iHeight < 18)iHeight = 18;
		y -= iHeight;
		if(m->historic())
		{
			grp.setColor(QColorGroup::Text,m_clrHistoricText);
		} else {
			if(m == last)
				grp.setColor(QColorGroup::Text,m_clrCurText);
			else {
				int iClrIdx = l->count() - (idx + 2);
				if(iClrIdx < 0)iClrIdx = 0;
				else if(iClrIdx >= NUM_OLD_COLORS)iClrIdx = (NUM_OLD_COLORS - 1);
				grp.setColor(QColorGroup::Text,m_clrOldText[iClrIdx]);
			}
		}
		int iMaxY = y > m_pWndBody->textRect().y() ? y : m_pWndBody->textRect().y();
		QRect clip(m_pWndBody->textRect().x() + 20,iMaxY,m_pWndBody->textRect().width() - 20,iHeight);

		m->text()->draw(&p,m_pWndBody->textRect().x() + 20,y,clip,grp); //
		if(y > m_pWndBody->textRect().y())
		{
			if(m->image())
				p.drawPixmap(m_pWndBody->textRect().x() + 1,y + 1,*(m->image()),0,0,16,16);
		}
		m = l->prev();
		idx--;
	}

	p.setPen(QPen(m_clrTitle));
	p.setFont(*m_pTitleFont);
	QString title;
	KviQString::sprintf(title,"[%d/%d]",iIdx + 1,l->count());
	
	if(tab->window())
	{
		title += " ";
		title += tab->window()->plainTextCaption();
	}
	p.drawText(m_pWndBorder->titleRect(),Qt::AlignLeft | Qt::SingleLine,title);
	p.end();
}

void KviNotifierWindow::mouseMoveEvent(QMouseEvent * e)
{

	//debug ("move on x,y: %d,%d", e->pos().x(), e->pos().y());

	if (!m_bLeftButtonIsPressed) {
	
		if(checkResizing(e->pos())) goto sartelo;
	
		/* ### MOUSE-MOVE-EVENT -> m_pWndBorder ### */
		if (m_pWndBorder->captionRect().contains(e->pos())) {
			if (m_pWndBorder->closeRect().contains(e->pos())) {
				m_pWndBorder->setCloseIcon(WDG_ICON_OVER);
			} else {
				m_pWndBorder->setCloseIcon(WDG_ICON_OUT);
			}
			goto sartelo;
		}
	
		if(m_pWndTabs->currentTab())
		{
			/* ### MOUSE-MOVE-EVENT -> m_pWndTabs ### */ 
			if (m_pWndTabs->rect().contains(e->pos())) {
				m_pWndTabs->mouseMoveEvent(e);
				goto sartelo;
			}
		
			/* ### MOUSE-MOVE-EVENT -> m_pWndBody ### */
			if (m_pWndBody->rect().contains(e->pos())) {
				if (m_pWndBody->rctWriteIcon().contains(e->pos())) {
					m_pWndBody->setWriteIcon(WDG_ICON_ON);
				} else {
					m_pWndBody->setWriteIcon(WDG_ICON_OFF);
				}
				goto sartelo;
			}
		}
		/* ### END ### */
		
sartelo:
		
		update();
	
	}

	if(m_bDragging) {
		setCursor(Qt::SizeAllCursor);
		
		int w = m_wndRect.width();
		int h = m_wndRect.height();
		
		m_wndRect.setX( m_pntPos.x() + cursor().pos().x() - m_pntDrag.x() );
		m_wndRect.setY( m_pntPos.y() + cursor().pos().y() - m_pntDrag.y() );
		
		m_wndRect.setWidth(w);
		m_wndRect.setHeight(h);
		
		setGeometry(m_wndRect);
		
		return;
	} else if (m_bResizing) {
		resize(e->pos());
	}

}

void KviNotifierWindow::mousePressEvent(QMouseEvent * e)
{

	// stop blinking first of all :)
	bool bWasBlinkOn = m_bBlinkOn;
	m_bBlinkOn = false;
	stopBlinkTimer();
	m_tAutoHideAt = 0;
	stopAutoHideTimer();

	setActiveWindow();
	setFocus();

	m_pntClick = e->pos();

	if(e->button() == Qt::RightButton)
	{
		contextPopup(mapToGlobal(e->pos()));
		return;
	} else if(e->button() == Qt::LeftButton) {
		m_bLeftButtonIsPressed = true;
	}

	if(checkResizing(m_pntClick)) goto sartelo;

	if(m_pWndTabs->rect().contains(e->pos()))
	{
		m_pWndTabs->mousePressEvent(e);
		goto sartelo;
	}

	if(m_pWndBorder->captionRect().contains(e->pos())) {
		//debug ("Clicked on m_pWndBorder->rect()");
		if(m_pWndBorder->closeRect().contains(e->pos()))	{
			//debug ("\tClicked on m_pWndBorder->closeRect()");
			m_bCloseDown = true;
			m_pWndBorder->setCloseIcon(WDG_ICON_CLICKED);
			goto sartelo;
		}

		if( !m_bResizing) {
			m_bDragging = true;
			m_pntDrag = cursor().pos();
			m_pntPos = pos();
			goto sartelo;
		}
	}

	//debug ("x,y: %d,%d - width,height: %d,%d", m_pWndBorder->rect().x(),m_pWndBorder->rect().y(),m_pWndBorder->rect().width(),m_pWndBorder->rect().height());

	if (m_pWndBorder->rect().contains(e->pos())) {
		if(m_pWndTabs->currentTab())
		{
			//debug ("Clicked on m_pWndBody->textRect()");
			if(m_pWndBody->rctWriteIcon().contains(e->pos()))
			{
				m_pWndBody->setWriteIcon(WDG_ICON_CLICKED);
				showLineEdit(!(m_pLineEdit->isVisible()));
			}	
			
			if(m_pWndBody->rctPrevIcon().contains(e->pos()) && m_pWndBody->prevIconState()!=WDG_ICON_OFF)
			{
				m_bPrevDown = true;
				m_pWndBody->setPrevIcon(WDG_ICON_CLICKED);
				prevButtonClicked();
			}
		
			if(m_pWndBody->rctNextIcon().contains(e->pos()) && m_pWndBody->nextIconState()!=WDG_ICON_OFF)
			{
				m_bNextDown = true;
				m_pWndBody->setNextIcon(WDG_ICON_CLICKED);
				nextButtonClicked();
			}
		}
		goto sartelo;		
	}

	if(bWasBlinkOn) goto sartelo; else return;
	
sartelo:
	update();
	
}

void KviNotifierWindow::mouseReleaseEvent(QMouseEvent * e)
{
	m_bWriteDown = false;
	m_bNextDown = false;
	m_bPrevDown = false;
	m_bCloseDown = false;
	m_bLeftButtonIsPressed = false;
	m_bResizing = false;

	//m_pWndBody->setWriteIcon(WDG_ICON_ON);
	m_pWndBody->setNextIcon(WDG_ICON_ON);
	m_pWndBody->setPrevIcon(WDG_ICON_ON);
	m_pWndTabs->setCloseTabIcon(WDG_ICON_OFF);

	if(m_bDragging)
	{
		m_bDragging = false;
		if(QApplication::overrideCursor()) QApplication::restoreOverrideCursor();
		return;
	}

	if(m_pWndBorder->captionRect().contains(e->pos())) {
	
		if(m_pWndBorder->closeRect().contains(e->pos())) {
			debug ("hide now from release event");
			hideNow();
		} else {
			update();
		}
	
	}

	if(m_pWndTabs->rect().contains(e->pos()))
	{
		m_pWndTabs->mouseReleaseEvent(e);
	}
	
	setCursor(-1);
	
}

void KviNotifierWindow::mouseDoubleClickEvent(QMouseEvent * e)
{
	if(!m_pWndBody->textRect().contains(e->pos()))
	{
		QWidget::mouseDoubleClickEvent(e);
		return;
	}
	
	KviNotifierWindowTab * tab = m_pWndTabs->currentTab();
	if(!tab)return;

	if(!tab->window())return;
	
	hideNow();
	delayedRaise(tab->window());
}

void KviNotifierWindow::keyPressEvent(QKeyEvent * e)
{
	if (e->key()==Qt::Key_Shift) {
		m_bKeyShiftPressed = true;
		return;
	}
	
	if (e->key()==Qt::Key_Left && m_bKeyShiftPressed) {
		m_pWndTabs->prev();
		return;
	}
	if (e->key()==Qt::Key_Right && m_bKeyShiftPressed) {
		m_pWndTabs->next();
		return;
	}
	
	if (e->key()==Qt::Key_Escape) {
		hideNow();		
		return;
	}
}

void KviNotifierWindow::keyReleaseEvent(QKeyEvent * e)
{
	if (e->key()==Qt::Key_Shift) {
		m_bKeyShiftPressed = false;
	}
}

void KviNotifierWindow::delayedRaise(KviWindow * pWnd)
{
	m_pWindowToRaise = pWnd;
	QTimer::singleShot(0,this,SLOT(delayedRaiseSlot()));
}

void KviNotifierWindow::delayedRaiseSlot()
{
	if(!m_pWindowToRaise)return;
	if(!g_pApp->windowExists(m_pWindowToRaise))return;

	if(m_pWindowToRaise->mdiParent())
	{
		if(!m_pWindowToRaise->frame()->isVisible())
			m_pWindowToRaise->frame()->show();

		m_pWindowToRaise->frame()->raise();
		((QWidget *)(m_pWindowToRaise->frame()))->setActiveWindow();
		m_pWindowToRaise->frame()->setFocus();
	}

	m_pWindowToRaise->delayedAutoRaise();
}

bool KviNotifierWindow::checkResizing(QPoint e)
{
	if (e.y()<WDG_BORDER_THICKNESS) {
		if (e.x()<WDG_BORDER_THICKNESS) {
			//// UP LEFT CORNER ////
			setCursor(Qt::SizeFDiagCursor);
			if (m_bLeftButtonIsPressed) {
				m_bResizing = true;
				m_whereResizing = WDG_UPSX;
			}				
		} else if (e.x() > (size().width()-WDG_BORDER_THICKNESS)) {
			//// UP RIGHT CORNER ////
			setCursor(Qt::SizeBDiagCursor);
			if (m_bLeftButtonIsPressed) {
				m_bResizing = true;
				m_whereResizing = WDG_UPDX;
			}
		} else {
			//// UP SIDE ////
			setCursor(Qt::SizeVerCursor);
			if (m_bLeftButtonIsPressed) {
				m_bResizing = true;
				m_whereResizing = WDG_UP;
			}			}
	} else if (e.y() > (size().height()-WDG_BORDER_THICKNESS)) {
		if (e.x()<WDG_BORDER_THICKNESS) {
			//// DOWN LEFT CORNER ////
			setCursor(Qt::SizeBDiagCursor);
			if (m_bLeftButtonIsPressed) {
				m_bResizing = true;
				m_whereResizing = WDG_DWNSX;
			}
		} else if (e.x() > (size().width()-WDG_BORDER_THICKNESS)) {
			//// DOWN RIGHT CORNER ////
			setCursor(Qt::SizeFDiagCursor);
			if (m_bLeftButtonIsPressed) {
				m_bResizing = true;
				m_whereResizing = WDG_DWNDX;
			}
		} else {
			//// DOWN SIDE ////
			setCursor(Qt::SizeVerCursor);
			if (m_bLeftButtonIsPressed) {
				m_bResizing = true;
				m_whereResizing = WDG_DWN;
			}
		}
	} else {
		if (e.x()<WDG_BORDER_THICKNESS) {
			//// LEFT SIZE ////
			setCursor(Qt::SizeHorCursor);
			if (m_bLeftButtonIsPressed) {
				m_bResizing = true;
				m_whereResizing = WDG_SX;
			}				
		} else if (e.x() > (size().width()-WDG_BORDER_THICKNESS)) {
			//// RIGHT SIZE ////
			setCursor(Qt::SizeHorCursor);
			if (m_bLeftButtonIsPressed) {
				m_bResizing = true;
				m_whereResizing = WDG_DX;
			}
		} else {
			//// ELSEWHERE ////
			m_whereResizing = 0;
			m_bResizing = false;
			setCursor(-1);
		}
	}
	return m_bResizing;
};



void KviNotifierWindow::resize(QPoint p, bool up)
{
	//QPoint aux = mapToGlobal(p);

	if (m_whereResizing==WDG_UPSX || m_whereResizing==WDG_SX || m_whereResizing==WDG_DWNSX) {
		if ((x() + width() - cursor().pos().x()) < WDG_MIN_WIDTH)
			m_wndRect.setLeft(x() + width() - WDG_MIN_WIDTH);
		else
			m_wndRect.setLeft(cursor().pos().x());
	}
	
	if (m_whereResizing == WDG_UPSX || m_whereResizing == WDG_UP || m_whereResizing == WDG_UPDX) {
		if (y()+height()-cursor().pos().y() < WDG_MIN_HEIGHT)
			m_wndRect.setTop(y() + height() - WDG_MIN_HEIGHT);
		else
			m_wndRect.setTop(cursor().pos().y());
	}

	if (m_whereResizing == WDG_DX || m_whereResizing == WDG_UPDX || m_whereResizing == WDG_DWNDX) {
		if ((cursor().pos().x() - x()) > WDG_MIN_WIDTH)
			m_wndRect.setRight(cursor().pos().x());
		else
			m_wndRect.setRight(x() + WDG_MIN_WIDTH);
	}

	if (m_whereResizing == WDG_DWN || m_whereResizing == WDG_DWNDX || m_whereResizing == WDG_DWNSX) {
		if ((cursor().pos().y() - y()) > WDG_MIN_HEIGHT)
			m_wndRect.setBottom(cursor().pos().y());
		else
			m_wndRect.setBottom(y() + WDG_MIN_HEIGHT);
	}

	showLineEdit(m_pLineEdit->isVisible());
	setGeometry(m_wndRect);
}

void KviNotifierWindow::redrawWindow()
{
	if(m_pixBackground.size()!=m_wndRect.size())
	{
		// Redraw only if size was modified..
		m_pixBackground.resize(m_wndRect.size());
		m_pixForeground.resize(m_wndRect.size());
		m_pixBackgroundHighlighted.resize(m_wndRect.size());
		
		m_pWndBorder->resize( m_wndRect.size() );
		m_pWndTabs->setWidth( m_pWndBorder->tabsRect().width() );
		m_pWndBody->resize( m_pWndBorder->bodyRect() );
		m_pProgressBar->setHeight(m_pWndBorder->bodyRect().height() + m_pWndBorder->tabsRect().height());
	}
	
	QPainter paint;
	if(m_bBlinkOn)
	{
		m_pWndBorder->touch();
		m_pWndTabs->touch();
		m_pWndBody->touch();
		paint.begin(&m_pixBackgroundHighlighted);
			m_pWndBorder->draw(&paint,true);
			m_pWndTabs->draw(&paint);
			m_pWndBody->draw(&paint);
			m_pProgressBar->draw(&paint);
		paint.end();
		bitBlt(&m_pixForeground, 0,0, &m_pixBackgroundHighlighted);
		m_pWndBorder->touch();
		m_pWndTabs->touch();
		m_pWndBody->touch();
	} else {
		paint.begin(&m_pixBackground);
			m_pWndBorder->draw(&paint);
			m_pWndTabs->draw(&paint);
			m_pWndBody->draw(&paint);
			m_pProgressBar->draw(&paint);
		paint.end();
		bitBlt(&m_pixForeground, 0,0, &m_pixBackground);
	}
	
}

inline void KviNotifierWindow::setCursor(int cur) {
	if (m_cursor.shape() != cur) {
		if(QApplication::overrideCursor()) QApplication::restoreOverrideCursor();
		m_cursor.setShape((Qt::CursorShape)cur);
		QApplication::setOverrideCursor(m_cursor);
	} else if (cur==-1)
		if(QApplication::overrideCursor()) QApplication::restoreOverrideCursor();
}

void KviNotifierWindow::leaveEvent(QEvent * e)
{
	// Leaving the widget area, restore default cursor
	m_pWndBorder->resetIcons();
	m_pWndTabs->resetIcons();
	if (!m_bResizing)
		setCursor(-1);
}

void KviNotifierWindow::contextPopup(const QPoint &pos)
{
	if(!m_pContextPopup)
	{
		m_pContextPopup = new QPopupMenu(this);
		connect(m_pContextPopup,SIGNAL(aboutToShow()),this,SLOT(fillContextPopup()));
		m_pDisablePopup = new QPopupMenu(this);
	}

	m_pContextPopup->popup(pos);
}

void KviNotifierWindow::fillContextPopup()
{
	m_pContextPopup->clear();

	m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CLOSE)),__tr2qs_ctx("Hide","notifier"),this,SLOT(hideNow()));
	m_pContextPopup->insertSeparator();
	m_pDisablePopup->clear();
	m_pDisablePopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TIME)),__tr2qs_ctx("1 Minute","notifier"),this,SLOT(disableFor1Minute()));
	m_pDisablePopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TIME)),__tr2qs_ctx("5 Minutes","notifier"),this,SLOT(disableFor5Minutes()));
	m_pDisablePopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TIME)),__tr2qs_ctx("15 Minutes","notifier"),this,SLOT(disableFor15Minutes()));
	m_pDisablePopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TIME)),__tr2qs_ctx("30 Minutes","notifier"),this,SLOT(disableFor30Minutes()));
	m_pDisablePopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TIME)),__tr2qs_ctx("1 Hour","notifier"),this,SLOT(disableFor60Minutes()));
	m_pDisablePopup->insertSeparator();
	m_pDisablePopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_QUIT)),__tr2qs_ctx("Until KVIrc is Restarted","notifier"),this,SLOT(disableUntilKVIrcRestarted()));
	m_pDisablePopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_QUIT)),__tr2qs_ctx("Permanently (Until Explicitly Enabled)","notifier"),this,SLOT(disablePermanently()));

	m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_QUIT)),__tr2qs_ctx("Disable","notifier"),m_pDisablePopup);
}

void KviNotifierWindow::disableFor15Minutes()
{
	kvi_time_t tNow = kvi_unixTime();
	g_tNotifierDisabledUntil = tNow + (60 * 15);

	hideNow();
}

void KviNotifierWindow::disableFor5Minutes()
{
	kvi_time_t tNow = kvi_unixTime();
	g_tNotifierDisabledUntil = tNow + (60 * 5);

	hideNow();
}


void KviNotifierWindow::disableFor1Minute()
{
	kvi_time_t tNow = kvi_unixTime();
	g_tNotifierDisabledUntil = tNow + 60;

	hideNow();
}

void KviNotifierWindow::disableFor30Minutes()
{
	kvi_time_t tNow = kvi_unixTime();
	g_tNotifierDisabledUntil = tNow + (60 * 30);

	hideNow();
}

void KviNotifierWindow::disableFor60Minutes()
{
	kvi_time_t tNow = kvi_unixTime();
	g_tNotifierDisabledUntil = tNow + (60 * 60);

	hideNow();
}

void KviNotifierWindow::disableUntilKVIrcRestarted()
{
	kvi_time_t tNow = kvi_unixTime();
	// in fact we just disable for one year
	g_tNotifierDisabledUntil = tNow + (3600 * 24 * 360); // one year :D
	// this will NOT work if the module is forcibly unloaded with /notifier.unload
	// but the user should never do it :D
	hideNow();
}

void KviNotifierWindow::disablePermanently()
{
	KVI_OPTION_BOOL(KviOption_boolForciblyDisableNotifier) = true;
	hideNow();
}

void KviNotifierWindow::wheelEvent(QWheelEvent * e)
{
	if(e->delta() > 0)prevButtonClicked();
	else nextButtonClicked();
}


void KviNotifierWindow::prevButtonClicked()
{
	KviNotifierWindowTab * tab = m_pWndTabs->currentTab();
	if(!tab)return;
	tab->setPrevMessageAsCurrent();
	update();
}

void KviNotifierWindow::nextButtonClicked()
{
	KviNotifierWindowTab * tab = m_pWndTabs->currentTab();
	if(!tab)return;
	tab->setNextMessageAsCurrent();
	update();
}

void KviNotifierWindow::showLineEdit(bool bShow)
{
	if(bShow)
	{
		//if(m_pLineEdit->isVisible())return;
		if(!m_pWndTabs->currentMessage())return;
		if(!m_pWndTabs->currentTab()->window())return;
		QToolTip::remove(m_pLineEdit);
		QString tip = __tr2qs_ctx("Write text or commands to window","notifier");
		tip += " \"";
		tip += m_pWndTabs->currentTab()->window()->plainTextCaption();
		tip += "\"";
		QToolTip::add(m_pLineEdit,tip);
		m_pLineEdit->setGeometry(m_pWndBody->textRect().x(),m_pWndBody->textRect().y() + m_pWndBody->textRect().height() - m_iInputHeight,m_pWndBody->textRect().width(),m_iInputHeight);
		m_pLineEdit->show();
		m_pLineEdit->setFocus();
		redrawWindow();
		setActiveWindow();
	} else {
		if(!m_pLineEdit->isVisible())return;
		m_pLineEdit->hide();
		setFocus();
		KviNotifierWindowTab * tab = m_pWndTabs->currentTab();
		if(tab)tab->setLastMessageAsCurrent();
		update();
	}
}

bool KviNotifierWindow::eventFilter(QObject * pEdit,QEvent * e)
{
	if(pEdit != (QObject *)m_pLineEdit)return false; // huh ?
	if(!m_pLineEdit->isVisible())return false;
	if(e->type() == QEvent::MouseButtonPress)
	{
		bool bWasBlinkOn = m_bBlinkOn;
		m_bBlinkOn = false;
		m_tAutoHideAt = 0;
		stopAutoHideTimer();
		stopBlinkTimer();
		setActiveWindow();
		m_pLineEdit->setFocus();
		if(bWasBlinkOn)update();
		return true;
	}
	if(e->type() == QEvent::KeyPress)
	{
		if(((QKeyEvent *)e)->key() == Qt::Key_Escape)
		{
			hideNow();
			return true;
		}
	}
	return false;
}

void KviNotifierWindow::returnPressed()
{
	if(!m_pLineEdit->isVisible())return;

	KviNotifierWindowTab * tab = m_pWndTabs->currentTab();
	if(!tab)return;
	if(!tab->currentMessage())return;
	if(!tab->window())return;

	QString txt = m_pLineEdit->text();
	if(txt.isEmpty())return;
	QString html = txt;
	html.replace("<","&lt;");
	html.replace(">","&gt;");
	KviStr tmp(KviStr::Format,"%d",KVI_SMALLICON_OWNPRIVMSG);

	addMessage(tab->window(),tmp.ptr(),html,0); //m_pCurrentFocusedWindowTab on place of m_pCurrentMessage->window()
	m_pLineEdit->setText("");
	KviUserInput::parse(txt,tab->window(),QString::null,1);
}

void KviNotifierWindow::progressUpdate()
{
	double dProgress = m_qtStartedAt.elapsed()/(m_tAutoHideAt - m_tStartedAt);
	dProgress/=1000;
	m_pProgressBar->setProgress(dProgress);
	QPainter p(this);
	m_pProgressBar->draw(&p);
}
#include "m_notifierwindow.moc"

