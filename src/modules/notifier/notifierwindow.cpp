//=============================================================================
//
//   File : notifierwindow.cpp
//   Creation date : Tue Jul 6 2004 20:25:12 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2004-2008 Iacopo Palazzi < iakko(at)siena.linux.it >
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================
#include "kvi_settings.h"

#include "notifiermessage.h"
#include "notifierwindow.h"
#include "notifierwindowborder.h"
#include "notifierwindowtab.h"

#include "kvi_iconmanager.h"
#include "kvi_config.h"
#include "kvi_window.h"
#include "kvi_locale.h"
#include "kvi_frame.h"
#include "kvi_mirccntrl.h"
#include "kvi_options.h"
#include "kvi_userinput.h"
#include "kvi_themedlineedit.h"

#include <QApplication>
#include <QImage>
#include <QDesktopWidget>
#include <QToolTip>
#include <QEvent>
#include <QPen>
#include <QFontMetrics>
#include <QRegExp>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>

extern KviNotifierWindow * g_pNotifierWindow;

KviNotifierWindow::KviNotifierWindow()
: QWidget(0,
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
			Qt::FramelessWindowHint |
			Qt::Tool |
			Qt::WindowStaysOnTopHint)
#else
			Qt::FramelessWindowHint |
#ifndef COMPILE_ON_MAC
			Qt::Tool |
			Qt::X11BypassWindowManagerHint |
#endif
			Qt::WindowStaysOnTopHint)
#endif
{
	setObjectName("kvirc_notifier_window");

	g_pNotifierWindow = this;

	m_eState = Hidden;
	m_dOpacity = 0.0;
	m_pShowHideTimer = 0;
	m_pBlinkTimer = 0;
	m_tAutoHideAt = 0;
	m_tStartedAt=0;
	m_pAutoHideTimer = 0;

	m_pWndBorder = new KviNotifierWindowBorder();

	setFocusPolicy(Qt::NoFocus);
	setMouseTracking(true);
	setAutoFillBackground(true);

	hide();

	m_bBlinkOn = false;

	m_bCloseDown = false;
	m_bPrevDown = false;
	m_bNextDown = false;
	m_bWriteDown = false;

	m_bLeftButtonIsPressed = false;
	m_bDiagonalResizing = false;
	m_bResizing = false;

	m_pContextPopup = 0;
	m_pDisablePopup = 0;

	m_bDragging = false;

	m_bDisableHideOnMainWindowGotAttention = false;

	// Positioning the notifier bottom-right
	QDesktopWidget * w = QApplication::desktop();
	QRect r = w->availableGeometry(w->primaryScreen());

	m_wndRect.setRect(	r.x() + r.width() - (WDG_MIN_WIDTH + SPACING),
				r.y() + r.height() - (WDG_MIN_HEIGHT + SPACING),
				WDG_MIN_WIDTH,
				WDG_MIN_HEIGHT );

	m_pWndTabs = new QTabWidget(this);
	m_pWndTabs->setUsesScrollButtons(true);
	m_pWndTabs->setTabsClosable(true);
	connect(m_pWndTabs,SIGNAL(tabCloseRequested(int)),this,SLOT(slotTabCloseRequested(int)));

	m_pProgressBar = new QProgressBar(this);
	m_pProgressBar->setOrientation(Qt::Vertical);
	m_pProgressBar->setRange(0, 100);
	m_pProgressBar->setTextVisible(false);
	m_pProgressBar->setMaximumWidth(8);
	m_pProgressBar->installEventFilter(this);

	m_pLineEdit = new KviThemedLineEdit(this, 0, "notifier_lineedit");
	QPalette palette=m_pLineEdit->palette();
	palette.setColor(m_pLineEdit->backgroundRole(), Qt::transparent);
	m_pLineEdit->setPalette(palette);
	m_pLineEdit->installEventFilter(this);
	connect(m_pLineEdit,SIGNAL(returnPressed()),this,SLOT(returnPressed()));

	QGridLayout *layout = new QGridLayout;
	layout->addWidget(m_pProgressBar, 0,0,2,1);
	layout->addWidget(m_pWndTabs,0,1,1,1);
	layout->addWidget(m_pLineEdit,1,1,1,1);
	layout->setSpacing(SPACING);

	layout->setGeometry(m_pWndBorder->bodyRect());
	layout->setContentsMargins(5,25,5,5);
	setLayout(layout);

	connect(g_pApp,SIGNAL(updateNotifier()),this,SLOT(updateGui()));
	QTimer::singleShot(0,this,SLOT(updateGui()));
}

KviNotifierWindow::~KviNotifierWindow()
{
	stopShowHideTimer();
	stopBlinkTimer();
	stopAutoHideTimer();
	delete m_pWndBorder;
	m_pWndTabs->deleteLater();
}

void KviNotifierWindow::updateGui()
{
	setFont(KVI_OPTION_FONT(KviOption_fontNotifier));
	QPalette palette=m_pLineEdit->palette();
	palette.setColor(m_pLineEdit->foregroundRole(), KVI_OPTION_COLOR(KviOption_colorNotifierForeground));
	m_pLineEdit->setPalette(palette);
	m_pLineEdit->setFont(KVI_OPTION_FONT(KviOption_fontNotifier));
	
	for(int i=0; i<m_pWndTabs->count();++i)
	{
		((KviNotifierWindowTab*)m_pWndTabs->widget(i))->updateGui();
	}

}

void KviNotifierWindow::addMessage(KviWindow * pWnd,const QString &szImageId,const QString &szText,unsigned int uMessageTime)
{
	QPixmap * pIcon;
	QString szMessage=szText;
	szMessage.replace( QRegExp("\r([^\r])*\r([^\r])+\r"), "\\2" );
	if(szImageId.isEmpty())pIcon = 0;
	else pIcon = g_pIconManager->getImage(szImageId);

	KviNotifierMessage * m = new KviNotifierMessage(pIcon ? new QPixmap(*pIcon) : 0,szMessage);

	//search for an existing tab
	KviNotifierWindowTab *tab=0, *tmp=0;
	for(int i=0; i<m_pWndTabs->count();++i)
	{
		tmp=(KviNotifierWindowTab*)m_pWndTabs->widget(i);
		if(tmp->wnd() == pWnd)
		{
			tab=tmp;
			break;
		}
	}

	if(!tab)
	{
		tab = new KviNotifierWindowTab(pWnd, m_pWndTabs);
	}

	//if the notifier is already visible, don't steal the focus from the current tab!
	//the user could be writing a message on it (bug #678)
	if(!isVisible())
		m_pWndTabs->setCurrentWidget(tab);

	tab->appendMessage(m);

	if(!isActiveWindow())
		startBlinking();

	if(uMessageTime > 0)
	{
		kvi_time_t tAutoHide = kvi_unixTime() + uMessageTime;
		if(m_tAutoHideAt < tAutoHide)
		{
			m_tAutoHideAt = tAutoHide;
			if(m_eState == Visible)
				startAutoHideTimer();
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

	if(isVisible())
		update();
}

void KviNotifierWindow::stopShowHideTimer()
{
	if(!m_pShowHideTimer)return;
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
	if(!m_pAutoHideTimer)return;
	delete m_pAutoHideTimer;
	m_pAutoHideTimer = 0;
	m_pProgressBar->setValue(0);
}

// FIXME: The code for detecting fullscreen window does NOT take into account multi-screen setups.
//        We also lack the code for MacOSX and Qt-only-X11 compilation.

#if COMPILE_KDE_SUPPORT

	#include <kwindowsystem.h>
	
	static bool active_window_is_full_screen()
	{
		WId activeId = KWindowSystem::activeWindow();
		KWindowInfo wi = KWindowSystem::windowInfo(activeId, NET::WMState);
		return (wi.valid() && wi.hasState(NET::FullScreen));
	}

#else //!COMPILE_KDE_SUPPORT
	#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	
		#include <windows.h>
		
		static bool active_window_is_full_screen()
		{
			HWND hForeground = ::GetForegroundWindow();
			if(!hForeground)
				return false;

			HWND hDesktop = ::GetDesktopWindow();
			if(hForeground == hDesktop)
				return false;

			HWND hShell = ::GetShellWindow();
			if(hForeground == hShell)
				return false;

			RECT rct;
			::GetWindowRect(hForeground,&rct);

			if((rct.right - rct.left) < GetSystemMetrics(SM_CXSCREEN))
				return false;
			
			if((rct.bottom - rct.top) < GetSystemMetrics(SM_CYSCREEN))
				return false;

			return true;
		}
	
	#endif //COMPILE_ON_WINDOWS || COMPILE_ON_MINGW
#endif //!COMPILE_KDE_SUPPORT


void KviNotifierWindow::doShow(bool bDoAnimate)
{
	if(!KVI_OPTION_BOOL(KviOption_boolEnableNotifier))
		return;

	kvi_time_t tNow = kvi_unixTime();
	if(g_tNotifierDisabledUntil > tNow)return;
	g_tNotifierDisabledUntil = 0;

#if defined(COMPILE_KDE_SUPPORT) || defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)

	if(KVI_OPTION_BOOL(KviOption_boolDontShowNotifierIfActiveWindowIsFullScreen))
	{
		// check if the active window is full screen
		if(active_window_is_full_screen())
			return;
	}

#endif //COMPILE_KDE_SUPPORT || COMPILE_ON_WINDOWS || COMPILE_ON_MINGW

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
			if(bDoAnimate)
			{
				m_pShowHideTimer = new QTimer();
				connect(m_pShowHideTimer,SIGNAL(timeout()),this,SLOT(heartbeat()));
				m_dOpacity = OPACITY_STEP;
				m_eState = Showing;
				m_bCrashShowWorkAround=true;
				setWindowOpacity(m_dOpacity);
				show();
				m_pShowHideTimer->start(40);
				m_bCrashShowWorkAround=false;
			} else {
				m_dOpacity = 1.0;
				m_eState = Visible;
				show();
				startBlinking();
				startAutoHideTimer();
			}
		break;
		default:
		break;
	}
}

bool KviNotifierWindow::shouldHideIfMainWindowGotAttention()
{
	if(m_bDisableHideOnMainWindowGotAttention)return false;
	KviNotifierWindowTab * t = (KviNotifierWindowTab*) m_pWndTabs->currentWidget();
	if(!t)return false;
	if(!t->wnd())return false;
	return t->wnd()->hasAttention();
}

void KviNotifierWindow::heartbeat()
{
	bool bIncreasing;
	double targetOpacity = 0;
	switch(m_eState)
	{
		case Hidden:
			hideNow();
		break;
		case Visible:
			stopShowHideTimer();
			m_dOpacity = 1.0;
			if(!isVisible())show();
			else update();
		break;
		case Showing:
			// if the main window got attention while
			// showing up then just hide now
			if(shouldHideIfMainWindowGotAttention())
			{
				m_eState = Hiding;
			} else {
				m_dOpacity += OPACITY_STEP;
				targetOpacity = isActiveWindow() ? KVI_OPTION_UINT(KviOption_uintNotifierActiveTransparency) : KVI_OPTION_UINT(KviOption_uintNotifierInactiveTransparency);

				targetOpacity/=100;
				if(m_dOpacity >= targetOpacity)
				{
					m_dOpacity = targetOpacity;
					m_eState = Visible;
					stopShowHideTimer();
					startBlinking();
					startAutoHideTimer();
				}

				if(!isVisible())show(); //!!!
				setWindowOpacity(m_dOpacity);
				update();

			}
		break;
		case FocusingOn:
			targetOpacity = KVI_OPTION_UINT(KviOption_uintNotifierActiveTransparency);
			targetOpacity/=100;
			bIncreasing = targetOpacity>m_dOpacity;
			m_dOpacity += bIncreasing?
				OPACITY_STEP : -(OPACITY_STEP);
			if( (bIncreasing && (m_dOpacity >= targetOpacity) ) ||
				(!bIncreasing && (m_dOpacity <= targetOpacity) )
				)
			{
				m_dOpacity = targetOpacity;
				m_eState = Visible;
				stopShowHideTimer();
			}

			setWindowOpacity(m_dOpacity);
			break;
		case FocusingOff:
			targetOpacity = KVI_OPTION_UINT(KviOption_uintNotifierInactiveTransparency);
			targetOpacity/=100;
			bIncreasing = targetOpacity>m_dOpacity;
			m_dOpacity += bIncreasing ? OPACITY_STEP : -(OPACITY_STEP);
			//debug("%f %f %i %i",m_dOpacity,targetOpacity,bIncreasing,(m_dOpacity >= targetOpacity));
			if( (bIncreasing && (m_dOpacity >= targetOpacity) ) ||
				(!bIncreasing && (m_dOpacity <= targetOpacity) )
				)
			{
				m_dOpacity = targetOpacity;
				m_eState = Visible;
				stopShowHideTimer();
			}

			setWindowOpacity(m_dOpacity);
			break;
		case Hiding:
			m_dOpacity -= OPACITY_STEP;
			setWindowOpacity(m_dOpacity);
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
	hide();
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
		if((!bDoAnimate) || (x() != m_pWndBorder->x()) || (y() != m_pWndBorder->y()))
		{

			//debug ("just hide quickly with notifier x() %d and notifier y() % - WBorderx() %d and WBordery() %d and bDoanimate %d",x(),y(),m_pWndBorder->x(),m_pWndBorder->y(),bDoAnimate);
			// the user asked to not animate or
			// the window has been moved and the animation would suck anyway
			// just hide quickly
			hideNow();
		} else {
			//debug ("starting hide animation notifier x() %d and notifier y() % - WBorderx() %d and WBordery() %d and bDoanimate %d",x(),y(),m_pWndBorder->x(),m_pWndBorder->y(),bDoAnimate);
			m_pShowHideTimer = new QTimer();
			connect(m_pShowHideTimer,SIGNAL(timeout()),this,SLOT(heartbeat()));
			m_dOpacity = 1.0 - OPACITY_STEP;
			m_eState = Hiding;
			setWindowOpacity(m_dOpacity);
			update();
			m_pShowHideTimer->start(40);
		}
		break;
		default:
		break;
	}
}

void KviNotifierWindow::showEvent(QShowEvent *)
{
 	setGeometry(m_wndRect);
}

void KviNotifierWindow::hideEvent(QHideEvent *)
{
	if (m_bCrashShowWorkAround) return;
	stopBlinkTimer();
	stopShowHideTimer();
	stopAutoHideTimer();
	m_eState = Hidden; // make sure it's true
	m_tAutoHideAt = 0; // make sure it's true
	m_bDisableHideOnMainWindowGotAttention = false;
}

void KviNotifierWindow::startBlinking()
{
	stopBlinkTimer();
	m_bBlinkOn = false;

	if(KVI_OPTION_BOOL(KviOption_boolNotifierFlashing))
	{
		m_pBlinkTimer = new QTimer();
		connect(m_pBlinkTimer,SIGNAL(timeout()),this,SLOT(blink()));
		m_iBlinkCount = 0;
		m_pBlinkTimer->start(1000);
	}
}

void KviNotifierWindow::startAutoHideTimer()
{
	stopAutoHideTimer();
	m_tStartedAt = kvi_unixTime();
	if(m_tAutoHideAt <= m_tStartedAt)return;

	int nSecs = m_tAutoHideAt - m_tStartedAt;
	if(nSecs < 5)nSecs = 5;
	m_pAutoHideTimer = new QTimer();

	connect(m_pAutoHideTimer,SIGNAL(timeout()),this,SLOT(progressUpdate()));
	m_pAutoHideTimer->start(100);
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

void KviNotifierWindow::paintEvent(QPaintEvent *e)
{
	QPainter *p = new QPainter(this);

	if(m_wndRect.width()!=m_pWndBorder->width() || m_wndRect.height()!=m_pWndBorder->height())
		m_pWndBorder->resize( m_wndRect.size() );

	if(m_bBlinkOn)
	{
		m_pWndBorder->draw(p,true);
	} else {
		m_pWndBorder->draw(p);
	}

	p->setPen(KVI_OPTION_COLOR(KviOption_colorNotifierTitleForeground));
	p->setFont(KVI_OPTION_FONT(KviOption_fontNotifierTitle));

	QString title = "KVIrc - ";
	KviNotifierWindowTab *tab = (KviNotifierWindowTab *)m_pWndTabs->currentWidget();
	if(tab)
	{
		if(tab->wnd())
		{
			title += tab->wnd()->plainTextCaption();
		} else {
			title += "notifier";
		}
	} else {
			title += "notifier";
	}
	p->drawText(m_pWndBorder->titleRect(),Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine,title);
	
	delete p;
	e->ignore();
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
		/* ### END ### */

sartelo:
	update();

	}

	if(m_bDragging)
	{
		setCursor(Qt::SizeAllCursor);

		int w = m_wndRect.width();
		int h = m_wndRect.height();

		m_wndRect.setX( m_pntPos.x() + cursor().pos().x() - m_pntDrag.x() );
		m_wndRect.setY( m_pntPos.y() + cursor().pos().y() - m_pntDrag.y() );

		m_wndRect.setWidth(w);
		m_wndRect.setHeight(h);

		setGeometry(m_wndRect);
	} else if (m_bResizing)
	{
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

	activateWindow();
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

	if(m_bDragging)
	{
		m_bDragging = false;
		if(QApplication::overrideCursor()) QApplication::restoreOverrideCursor();
		return;
	}

	if(m_pWndBorder->captionRect().contains(e->pos())) {

		if(m_pWndBorder->closeRect().contains(e->pos())) {
			//debug ("hide now from release event");
			hideNow();
		} else {
			update();
		}

	}

	setCursor(-1);
}

void KviNotifierWindow::keyPressEvent(QKeyEvent * e)
{
	if (e->key()==Qt::Key_Escape) {
		hideNow();
		return;
	}
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
}

void KviNotifierWindow::resize(QPoint , bool)
{
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

inline void KviNotifierWindow::setCursor(int cur)
{
	if (m_cursor.shape() != cur) {
		if(QApplication::overrideCursor()) QApplication::restoreOverrideCursor();
		m_cursor.setShape((Qt::CursorShape)cur);
		QApplication::setOverrideCursor(m_cursor);
	} else if (cur==-1)
		if(QApplication::overrideCursor()) QApplication::restoreOverrideCursor();
}

void KviNotifierWindow::enterEvent(QEvent *)
{
	if(!m_pShowHideTimer) {
		m_pShowHideTimer = new QTimer();
		connect(m_pShowHideTimer,SIGNAL(timeout()),this,SLOT(heartbeat()));
	}
	m_eState = FocusingOn;
	m_pShowHideTimer->start(40);
}

void KviNotifierWindow::leaveEvent(QEvent *)
{
	// Leaving the widget area, restore default cursor
	m_pWndBorder->resetIcons();
	if (!m_bResizing) setCursor(-1);
	if(!m_pShowHideTimer) {
		m_pShowHideTimer = new QTimer();
		connect(m_pShowHideTimer,SIGNAL(timeout()),this,SLOT(heartbeat()));
	}
	if (m_eState!=Hidden)
	{
		m_eState = FocusingOff;
		m_pShowHideTimer->start(40);
	}
}

void KviNotifierWindow::contextPopup(const QPoint &pos)
{
	if(!m_pContextPopup)
	{
		m_pContextPopup = new KviTalPopupMenu(this);
		connect(m_pContextPopup,SIGNAL(aboutToShow()),this,SLOT(fillContextPopup()));
		m_pDisablePopup = new KviTalPopupMenu(this);
	}

	m_pContextPopup->popup(pos);
}

void KviNotifierWindow::fillContextPopup()
{
	m_pContextPopup->clear();
	m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_EDITOR)),__tr2qs_ctx("Show/Hide input line","notifier"),this,SLOT(toggleLineEdit()));
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
	KVI_OPTION_BOOL(KviOption_boolEnableNotifier) = false;
	hideNow();
}

void KviNotifierWindow::toggleLineEdit()
{
	showLineEdit(!m_pLineEdit->isVisible());
}

void KviNotifierWindow::showLineEdit(bool bShow)
{
	if(bShow)
	{
		if(m_pLineEdit->isVisible())return;
		if(!((KviNotifierWindowTab *)m_pWndTabs->currentWidget())->wnd())return;
		m_pLineEdit->setToolTip("");;
		QString tip = __tr2qs_ctx("Write text or commands to window","notifier");
		tip += " \"";
		tip += ((KviNotifierWindowTab *)m_pWndTabs->currentWidget())->wnd()->plainTextCaption();
		tip += "\"";
		m_pLineEdit->setToolTip(tip);
		m_pLineEdit->show();
		m_pLineEdit->setFocus();
		activateWindow();
	} else {
		if(!m_pLineEdit->isVisible())return;
		m_pLineEdit->hide();
		setFocus();
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
		activateWindow();
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

	KviNotifierWindowTab * tab = (KviNotifierWindowTab *) m_pWndTabs->currentWidget();
	if(!tab)return;
	if(!tab->wnd())return;

	QString txt = m_pLineEdit->text();
	if(txt.isEmpty())return;
	QString html = txt;
	html.replace("<","&lt;");
	html.replace(">","&gt;");
	KviStr tmp(KviStr::Format,"%d",KVI_SMALLICON_OWNPRIVMSG);

	addMessage(tab->wnd(),tmp.ptr(),html,0);
	m_pLineEdit->setText("");
	KviUserInput::parse(txt,tab->wnd(),QString(),1);
}

void KviNotifierWindow::progressUpdate()
{
	kvi_time_t now = kvi_unixTime();
	int iProgress = (int) (100/(m_tAutoHideAt - m_tStartedAt)*(now - m_tStartedAt));
	m_pProgressBar->setValue(iProgress);
	if(now>=m_tAutoHideAt)
	{
		m_tAutoHideAt = 0;
		stopAutoHideTimer();
		doHide(true);
	}
}

void KviNotifierWindow::slotTabCloseRequested(int index)
{
	if(m_pWndTabs)
	{
		KviNotifierWindowTab *tab = (KviNotifierWindowTab*) m_pWndTabs->widget(index);
		m_pWndTabs->removeTab(index);
		if(tab)
			tab->deleteLater();

		if(m_pWndTabs->count()==0)
			hideNow();
	}
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_notifierwindow.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
