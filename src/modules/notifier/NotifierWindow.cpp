//=============================================================================
//
//   File : NotifierWindow.cpp
//   Creation date : Tue Jul 6 2004 20:25:12 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2008 Iacopo Palazzi < iakko(at)siena.linux.it >
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
//=============================================================================
#include "kvi_settings.h"

#include "NotifierMessage.h"
#include "NotifierWindow.h"
#include "NotifierWindowBorder.h"
#include "NotifierWindowTab.h"

#include "KviIconManager.h"
#include "KviConfigurationFile.h"
#include "KviWindow.h"
#include "KviLocale.h"
#include "KviMainWindow.h"
#include "KviOptions.h"
#include "KviUserInput.h"
#include "KviThemedLineEdit.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QEvent>
#include <QFontMetrics>
#include <QImage>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QRegExp>
#include <QToolTip>

extern NotifierWindow * g_pNotifierWindow;

NotifierWindow::NotifierWindow()
    : QWidget(nullptr, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
          | Qt::Tool)
#else
#ifndef COMPILE_ON_MAC
              | Qt::Tool | Qt::X11BypassWindowManagerHint
#endif
          )
#endif
{
	setObjectName("kvirc_notifier_window");

	g_pNotifierWindow = this;

	m_pWndBorder = new NotifierWindowBorder();

	setFocusPolicy(Qt::NoFocus);
	setMouseTracking(true);
	setAutoFillBackground(true);

	hide();

	// Positioning the notifier bottom-right
	QDesktopWidget * pDesktop = QApplication::desktop();
	QRect r = pDesktop->availableGeometry(pDesktop->primaryScreen());

	m_wndRect.setRect(
	    r.x() + r.width() - (WDG_MIN_WIDTH + SPACING),
	    r.y() + r.height() - (WDG_MIN_HEIGHT + SPACING),
	    WDG_MIN_WIDTH,
	    WDG_MIN_HEIGHT);

	m_pWndTabs = new QTabWidget(this);
	m_pWndTabs->setUsesScrollButtons(true);
	m_pWndTabs->setTabsClosable(true);
	connect(m_pWndTabs, SIGNAL(tabCloseRequested(int)), this, SLOT(slotTabCloseRequested(int)));

	m_pProgressBar = new QProgressBar(this);
	m_pProgressBar->setOrientation(Qt::Vertical);
	m_pProgressBar->setRange(0, 100);
	m_pProgressBar->setTextVisible(false);
	m_pProgressBar->setMaximumWidth(8);
	m_pProgressBar->installEventFilter(this);

	m_pLineEdit = new KviThemedLineEdit(this, nullptr, "notifier_lineedit");
	QPalette palette = m_pLineEdit->palette();
	palette.setColor(m_pLineEdit->backgroundRole(), Qt::transparent);
	m_pLineEdit->setPalette(palette);
	m_pLineEdit->installEventFilter(this);
	connect(m_pLineEdit, SIGNAL(returnPressed()), this, SLOT(returnPressed()));

	QGridLayout * layout = new QGridLayout;
	layout->addWidget(m_pProgressBar, 0, 0, 2, 1);
	layout->addWidget(m_pWndTabs, 0, 1, 1, 1);
	layout->addWidget(m_pLineEdit, 1, 1, 1, 1);
	layout->setSpacing(SPACING);

	layout->setGeometry(m_pWndBorder->bodyRect());
	layout->setContentsMargins(5, 25, 5, 5);
	setLayout(layout);

	connect(g_pApp, SIGNAL(updateNotifier()), this, SLOT(updateGui()));
	QTimer::singleShot(0, this, SLOT(updateGui()));
}

NotifierWindow::~NotifierWindow()
{
	stopShowHideTimer();
	stopBlinkTimer();
	stopAutoHideTimer();
	delete m_pWndBorder;
	m_pWndTabs->deleteLater();
}

void NotifierWindow::updateGui()
{
	setFont(KVI_OPTION_FONT(KviOption_fontNotifier));
	QPalette palette = m_pLineEdit->palette();
	palette.setColor(m_pLineEdit->foregroundRole(), KVI_OPTION_COLOR(KviOption_colorNotifierForeground));
	m_pLineEdit->setPalette(palette);
	m_pLineEdit->setFont(KVI_OPTION_FONT(KviOption_fontNotifier));

	for(int i = 0; i < m_pWndTabs->count(); ++i)
		((NotifierWindowTab *)m_pWndTabs->widget(i))->updateGui();
}

void NotifierWindow::addMessage(KviWindow * pWnd, const QString & szImageId, const QString & szText, unsigned int uMessageTime)
{
	QPixmap * pIcon = nullptr;
	QString szMessage = szText;
	szMessage.replace(QRegExp("\r([^\r])*\r([^\r])+\r"), "\\2");
	if(!szImageId.isEmpty())
		pIcon = g_pIconManager->getImage(szImageId);

	NotifierMessage * pMessage = new NotifierMessage(pIcon ? new QPixmap(*pIcon) : nullptr, szMessage);

	//search for an existing tab
	NotifierWindowTab *pTab = nullptr, *pTmp = nullptr;
	for(int i = 0; i < m_pWndTabs->count(); ++i)
	{
		pTmp = (NotifierWindowTab *)m_pWndTabs->widget(i);
		if(pTmp->wnd() == pWnd)
		{
			pTab = pTmp;
			break;
		}
	}

	if(!pTab)
		pTab = new NotifierWindowTab(pWnd, m_pWndTabs);

	//if the notifier is already visible, don't steal the focus from the current tab!
	//the user could be writing a message on it (bug #678)
	if(!isVisible())
		m_pWndTabs->setCurrentWidget(pTab);

	pTab->appendMessage(pMessage);

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
	}
	else
	{
		// never hide
		stopAutoHideTimer();
		m_tAutoHideAt = 0;
	}

	if(pWnd && pWnd->hasAttention(KviWindow::MainWindowIsVisible))
		m_bDisableHideOnMainWindowGotAttention = true;

	if(isVisible())
		update();
}

void NotifierWindow::stopShowHideTimer()
{
	if(!m_pShowHideTimer)
		return;
	delete m_pShowHideTimer;
	m_pShowHideTimer = nullptr;
}

void NotifierWindow::stopBlinkTimer()
{
	if(!m_pBlinkTimer)
		return;
	delete m_pBlinkTimer;
	m_pBlinkTimer = nullptr;
}

void NotifierWindow::stopAutoHideTimer()
{
	if(!m_pAutoHideTimer)
		return;
	delete m_pAutoHideTimer;
	m_pAutoHideTimer = nullptr;
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
#else // COMPILE_KDE_SUPPORT
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)

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
	::GetWindowRect(hForeground, &rct);

	if((rct.right - rct.left) < GetSystemMetrics(SM_CXSCREEN))
		return false;

	if((rct.bottom - rct.top) < GetSystemMetrics(SM_CYSCREEN))
		return false;

	return true;
}

#endif //COMPILE_ON_WINDOWS || COMPILE_ON_MINGW
#endif // COMPILE_KDE_SUPPORT

void NotifierWindow::doShow(bool bDoAnimate)
{
	if(!KVI_OPTION_BOOL(KviOption_boolEnableNotifier))
		return;

	kvi_time_t tNow = kvi_unixTime();
	if(g_tNotifierDisabledUntil > tNow)
		return;
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
				connect(m_pShowHideTimer, SIGNAL(timeout()), this, SLOT(heartbeat()));
				m_dOpacity = OPACITY_STEP;
				m_eState = Showing;
				m_bCrashShowWorkAround = true;
				setWindowOpacity(m_dOpacity);
				show();
				m_pShowHideTimer->start(40);
				m_bCrashShowWorkAround = false;
			}
			else
			{
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

bool NotifierWindow::shouldHideIfMainWindowGotAttention()
{
	if(m_bDisableHideOnMainWindowGotAttention)
		return false;
	NotifierWindowTab * pTab = (NotifierWindowTab *)m_pWndTabs->currentWidget();
	if(!pTab)
		return false;
	if(!pTab->wnd())
		return false;
	return pTab->wnd()->hasAttention();
}

void NotifierWindow::heartbeat()
{
	switch(m_eState)
	{
		case Hidden:
			hideNow();
			break;
		case Visible:
			stopShowHideTimer();
			m_dOpacity = 1.0;
			if(!isVisible())
				show();
			else
				update();
			break;
		case Showing:
			// if the main window got attention while
			// showing up then just hide now
			if(shouldHideIfMainWindowGotAttention())
			{
				m_eState = Hiding;
			}
			else
			{
				m_dOpacity += OPACITY_STEP;
				double targetOpacity = isActiveWindow() ? KVI_OPTION_UINT(KviOption_uintNotifierActiveTransparency) : KVI_OPTION_UINT(KviOption_uintNotifierInactiveTransparency);

				targetOpacity /= 100;
				if(m_dOpacity >= targetOpacity)
				{
					m_dOpacity = targetOpacity;
					m_eState = Visible;
					stopShowHideTimer();
					startBlinking();
					startAutoHideTimer();
				}

				if(!isVisible())
					show();
				setWindowOpacity(m_dOpacity);
				update();
			}
			break;
		case FocusingOn:
		{
			double targetOpacity = KVI_OPTION_UINT(KviOption_uintNotifierActiveTransparency);
			targetOpacity /= 100;
			bool bIncreasing = targetOpacity > m_dOpacity;
			m_dOpacity += bIncreasing ? OPACITY_STEP : -(OPACITY_STEP);
			if((bIncreasing && (m_dOpacity >= targetOpacity)) || (!bIncreasing && (m_dOpacity <= targetOpacity)))
			{
				m_dOpacity = targetOpacity;
				m_eState = Visible;
				stopShowHideTimer();
			}

			setWindowOpacity(m_dOpacity);
		}
			break;
		case FocusingOff:
		{
			double targetOpacity = KVI_OPTION_UINT(KviOption_uintNotifierInactiveTransparency);
			targetOpacity /= 100;
			bool bIncreasing = targetOpacity > m_dOpacity;
			m_dOpacity += bIncreasing ? OPACITY_STEP : -(OPACITY_STEP);

			if((bIncreasing && (m_dOpacity >= targetOpacity)) || (!bIncreasing && (m_dOpacity <= targetOpacity)))
			{
				m_dOpacity = targetOpacity;
				m_eState = Visible;
				stopShowHideTimer();
			}

			setWindowOpacity(m_dOpacity);
		}
			break;
		case Hiding:
			m_dOpacity -= OPACITY_STEP;
			setWindowOpacity(m_dOpacity);
			if(m_dOpacity <= 0.0)
				hideNow();
			else
				update();
			break;
	}
}

void NotifierWindow::hideNow()
{
	stopBlinkTimer();
	stopShowHideTimer();
	stopAutoHideTimer();
	m_eState = Hidden;
	m_dOpacity = 0.0;
	m_tAutoHideAt = 0;
	hide();
}

void NotifierWindow::doHide(bool bDoAnimate)
{
	stopAutoHideTimer();
	switch(m_eState)
	{
		case Hiding:
			// already hiding up
			if(!bDoAnimate)
				hideNow();
			return;
			break;
		case Hidden:
			// already hidden
			if(isVisible())
				hideNow();
			return;
			break;
		case Showing:
			// ops.. hiding!
			if(!bDoAnimate)
				hideNow();
			else
			{
				// continue animating, but hide
				m_eState = Hiding;
			}
			break;
		case Visible:
			stopBlinkTimer();
			stopShowHideTimer();
			if((!bDoAnimate) || (x() != m_pWndBorder->x()) || (y() != m_pWndBorder->y()))
			{

				// the user asked to not animate or
				// the window has been moved and the animation would suck anyway
				// just hide quickly
				hideNow();
			}
			else
			{
				m_pShowHideTimer = new QTimer();
				connect(m_pShowHideTimer, SIGNAL(timeout()), this, SLOT(heartbeat()));
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

void NotifierWindow::showEvent(QShowEvent *)
{
	setGeometry(m_wndRect);
}

void NotifierWindow::hideEvent(QHideEvent *)
{
	if(m_bCrashShowWorkAround)
		return;
	stopBlinkTimer();
	stopShowHideTimer();
	stopAutoHideTimer();
	m_eState = Hidden; // make sure it's true
	m_tAutoHideAt = 0; // make sure it's true
	m_bDisableHideOnMainWindowGotAttention = false;
}

void NotifierWindow::startBlinking()
{
	stopBlinkTimer();
	m_bBlinkOn = false;

	if(KVI_OPTION_BOOL(KviOption_boolNotifierFlashing))
	{
		m_pBlinkTimer = new QTimer();
		connect(m_pBlinkTimer, SIGNAL(timeout()), this, SLOT(blink()));
		m_iBlinkCount = 0;
		m_pBlinkTimer->start(1000);
	}
}

void NotifierWindow::startAutoHideTimer()
{
	stopAutoHideTimer();
	m_tStartedAt = kvi_unixTime();
	if(m_tAutoHideAt <= m_tStartedAt)
		return;

	int iSecs = m_tAutoHideAt - m_tStartedAt;
	if(iSecs < 5)
		iSecs = 5;
	m_pAutoHideTimer = new QTimer();

	connect(m_pAutoHideTimer, SIGNAL(timeout()), this, SLOT(progressUpdate()));
	m_pAutoHideTimer->start(100);
}

void NotifierWindow::blink()
{
	m_iBlinkCount++;
	m_bBlinkOn = !m_bBlinkOn;

	if(m_iBlinkCount > 100)
	{
		// stop blinking at a certain point and remain highlighted
		m_bBlinkOn = true;
		stopBlinkTimer();
	}
	else
	{
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

void NotifierWindow::paintEvent(QPaintEvent * e)
{
	QPainter * pPaint = new QPainter(this);

	if(width() != m_pWndBorder->width() || height() != m_pWndBorder->height())
		m_pWndBorder->resize(size());

	m_pWndBorder->draw(pPaint, m_bBlinkOn);

	pPaint->setPen(KVI_OPTION_COLOR(KviOption_colorNotifierTitleForeground));
	pPaint->setFont(KVI_OPTION_FONT(KviOption_fontNotifierTitle));

	QString szTitle = "KVIrc - ";
	NotifierWindowTab * pTab = (NotifierWindowTab *)m_pWndTabs->currentWidget();
	if(pTab && pTab->wnd())
		szTitle += pTab->wnd()->plainTextCaption();
	else
		szTitle += "notifier";

	pPaint->drawText(m_pWndBorder->titleRect(), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, szTitle);

	delete pPaint;
	e->ignore();
}

void NotifierWindow::mouseMoveEvent(QMouseEvent * e)
{
	if(!m_bLeftButtonIsPressed)
	{
		if(!checkResizing(e->pos()) && m_pWndBorder->captionRect().contains(e->pos()))
		{
			if(m_pWndBorder->closeRect().contains(e->pos()))
				m_pWndBorder->setCloseIcon(WDG_ICON_OVER);
			else
				m_pWndBorder->setCloseIcon(WDG_ICON_OUT);
		}

		update();
	}

	if(m_bDragging)
	{
		setCursor(Qt::SizeAllCursor);

		int iW = m_wndRect.width();
		int iH = m_wndRect.height();

		m_wndRect.setX(m_pntPos.x() + cursor().pos().x() - m_pntDrag.x());
		m_wndRect.setY(m_pntPos.y() + cursor().pos().y() - m_pntDrag.y());

		m_wndRect.setWidth(iW);
		m_wndRect.setHeight(iH);

		setGeometry(m_wndRect);
	}
	else if(m_bResizing)
	{
		resize(e->pos());
	}
}

void NotifierWindow::mousePressEvent(QMouseEvent * e)
{
	// stop blinking first of all :)
	bool bWasBlinkOn = m_bBlinkOn;
	m_bBlinkOn = false;
	stopBlinkTimer();
	m_tAutoHideAt = 0;
	stopAutoHideTimer();

	activateWindow();
	if(m_pLineEdit->isVisible())
		m_pLineEdit->setFocus();
	else
		setFocus();

	m_pntClick = e->pos();

	if(e->button() == Qt::RightButton)
	{
		contextPopup(mapToGlobal(e->pos()));
		return;
	}
	else if(e->button() == Qt::LeftButton)
	{
		m_bLeftButtonIsPressed = true;
	}

	if(checkResizing(m_pntClick))
	{
		update();
		return;
	}

	if(m_pWndBorder->captionRect().contains(e->pos()))
	{
		if(m_pWndBorder->closeRect().contains(e->pos()))
		{
			m_bCloseDown = true;
			m_pWndBorder->setCloseIcon(WDG_ICON_CLICKED);
			update();
			return;
		}

		if(!m_bResizing)
		{
			m_bDragging = true;
			m_pntDrag = cursor().pos();
			m_pntPos = pos();
			update();
			return;
		}
	}

	if(m_pWndBorder->rect().contains(e->pos()) || bWasBlinkOn)
		update();
}

void NotifierWindow::mouseReleaseEvent(QMouseEvent * e)
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
		if(QApplication::overrideCursor())
			QApplication::restoreOverrideCursor();
		return;
	}

	if(m_pWndBorder->captionRect().contains(e->pos()))
	{
		if(m_pWndBorder->closeRect().contains(e->pos()))
			hideNow();
		else
			update();
	}

	setCursor(-1);
}

void NotifierWindow::keyPressEvent(QKeyEvent * e)
{
	if(e->key() == Qt::Key_Escape)
	{
		hideNow();
		return;
	}
}

bool NotifierWindow::checkResizing(QPoint e)
{
	if(e.y() < WDG_BORDER_THICKNESS)
	{
		if(e.x() < WDG_BORDER_THICKNESS)
		{
			//// UP LEFT CORNER ////
			setCursor(Qt::SizeFDiagCursor);
			if(m_bLeftButtonIsPressed)
			{
				m_bResizing = true;
				m_whereResizing = WDG_UPSX;
			}
		}
		else if(e.x() > (size().width() - WDG_BORDER_THICKNESS))
		{
			//// UP RIGHT CORNER ////
			setCursor(Qt::SizeBDiagCursor);
			if(m_bLeftButtonIsPressed)
			{
				m_bResizing = true;
				m_whereResizing = WDG_UPDX;
			}
		}
		else
		{
			//// UP SIDE ////
			setCursor(Qt::SizeVerCursor);
			if(m_bLeftButtonIsPressed)
			{
				m_bResizing = true;
				m_whereResizing = WDG_UP;
			}
		}
	}
	else if(e.y() > (size().height() - WDG_BORDER_THICKNESS))
	{
		if(e.x() < WDG_BORDER_THICKNESS)
		{
			//// DOWN LEFT CORNER ////
			setCursor(Qt::SizeBDiagCursor);
			if(m_bLeftButtonIsPressed)
			{
				m_bResizing = true;
				m_whereResizing = WDG_DWNSX;
			}
		}
		else if(e.x() > (size().width() - WDG_BORDER_THICKNESS))
		{
			//// DOWN RIGHT CORNER ////
			setCursor(Qt::SizeFDiagCursor);
			if(m_bLeftButtonIsPressed)
			{
				m_bResizing = true;
				m_whereResizing = WDG_DWNDX;
			}
		}
		else
		{
			//// DOWN SIDE ////
			setCursor(Qt::SizeVerCursor);
			if(m_bLeftButtonIsPressed)
			{
				m_bResizing = true;
				m_whereResizing = WDG_DWN;
			}
		}
	}
	else
	{
		if(e.x() < WDG_BORDER_THICKNESS)
		{
			//// LEFT SIZE ////
			setCursor(Qt::SizeHorCursor);
			if(m_bLeftButtonIsPressed)
			{
				m_bResizing = true;
				m_whereResizing = WDG_SX;
			}
		}
		else if(e.x() > (size().width() - WDG_BORDER_THICKNESS))
		{
			//// RIGHT SIZE ////
			setCursor(Qt::SizeHorCursor);
			if(m_bLeftButtonIsPressed)
			{
				m_bResizing = true;
				m_whereResizing = WDG_DX;
			}
		}
		else
		{
			//// ELSEWHERE ////
			m_whereResizing = 0;
			m_bResizing = false;
			setCursor(-1);
		}
	}
	return m_bResizing;
}

void NotifierWindow::resize(QPoint, bool)
{
	if(m_whereResizing == WDG_UPSX || m_whereResizing == WDG_SX || m_whereResizing == WDG_DWNSX)
	{
		if((x() + width() - cursor().pos().x()) < WDG_MIN_WIDTH)
			m_wndRect.setLeft(x() + width() - WDG_MIN_WIDTH);
		else
			m_wndRect.setLeft(cursor().pos().x());
	}

	if(m_whereResizing == WDG_UPSX || m_whereResizing == WDG_UP || m_whereResizing == WDG_UPDX)
	{
		if(y() + height() - cursor().pos().y() < WDG_MIN_HEIGHT)
			m_wndRect.setTop(y() + height() - WDG_MIN_HEIGHT);
		else
			m_wndRect.setTop(cursor().pos().y());
	}

	if(m_whereResizing == WDG_DX || m_whereResizing == WDG_UPDX || m_whereResizing == WDG_DWNDX)
	{
		if((cursor().pos().x() - x()) > WDG_MIN_WIDTH)
			m_wndRect.setRight(cursor().pos().x());
		else
			m_wndRect.setRight(x() + WDG_MIN_WIDTH);
	}

	if(m_whereResizing == WDG_DWN || m_whereResizing == WDG_DWNDX || m_whereResizing == WDG_DWNSX)
	{
		if((cursor().pos().y() - y()) > WDG_MIN_HEIGHT)
			m_wndRect.setBottom(cursor().pos().y());
		else
			m_wndRect.setBottom(y() + WDG_MIN_HEIGHT);
	}

	showLineEdit(m_pLineEdit->isVisible());
	setGeometry(m_wndRect);
}

void NotifierWindow::setCursor(int iCur)
{
	if(m_cursor.shape() != iCur)
	{
		if(QApplication::overrideCursor())
			QApplication::restoreOverrideCursor();
		m_cursor.setShape((Qt::CursorShape)iCur);
		QApplication::setOverrideCursor(m_cursor);
	}
	else if(iCur == -1)
	{
		if(QApplication::overrideCursor())
			QApplication::restoreOverrideCursor();
	}
}

void NotifierWindow::enterEvent(QEvent *)
{
	if(!m_pShowHideTimer)
	{
		m_pShowHideTimer = new QTimer();
		connect(m_pShowHideTimer, SIGNAL(timeout()), this, SLOT(heartbeat()));
	}
	m_eState = FocusingOn;
	m_pShowHideTimer->start(40);
}

void NotifierWindow::leaveEvent(QEvent *)
{
	// Leaving the widget area, restore default cursor
	m_pWndBorder->resetIcons();
	if(!m_bResizing)
		setCursor(-1);

	if(!m_pShowHideTimer)
	{
		m_pShowHideTimer = new QTimer();
		connect(m_pShowHideTimer, SIGNAL(timeout()), this, SLOT(heartbeat()));
	}

	if(m_eState != Hidden)
	{
		m_eState = FocusingOff;
		m_pShowHideTimer->start(40);
	}
}

void NotifierWindow::contextPopup(const QPoint & pos)
{
	if(!m_pContextPopup)
	{
		m_pContextPopup = new QMenu(this);
		connect(m_pContextPopup, SIGNAL(aboutToShow()), this, SLOT(fillContextPopup()));
		m_pDisablePopup = new QMenu(this);
	}

	m_pContextPopup->popup(pos);
}

void NotifierWindow::fillContextPopup()
{
	m_pContextPopup->clear();
	m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Editor)), __tr2qs_ctx("Show/Hide input line", "notifier"), this, SLOT(toggleLineEdit()));
	m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Close)), __tr2qs_ctx("Hide", "notifier"), this, SLOT(hideNow()));
	m_pContextPopup->addSeparator();
	m_pDisablePopup->clear();
	m_pDisablePopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Time)), __tr2qs_ctx("1 Minute", "notifier"), this, SLOT(disableFor1Minute()));
	m_pDisablePopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Time)), __tr2qs_ctx("5 Minutes", "notifier"), this, SLOT(disableFor5Minutes()));
	m_pDisablePopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Time)), __tr2qs_ctx("15 Minutes", "notifier"), this, SLOT(disableFor15Minutes()));
	m_pDisablePopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Time)), __tr2qs_ctx("30 Minutes", "notifier"), this, SLOT(disableFor30Minutes()));
	m_pDisablePopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Time)), __tr2qs_ctx("1 Hour", "notifier"), this, SLOT(disableFor60Minutes()));
	m_pDisablePopup->addSeparator();
	m_pDisablePopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Quit)), __tr2qs_ctx("Until KVIrc is Restarted", "notifier"), this, SLOT(disableUntilKVIrcRestarted()));
	m_pDisablePopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Quit)), __tr2qs_ctx("Permanently (Until Explicitly Enabled)", "notifier"), this, SLOT(disablePermanently()));

	m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Quit)), __tr2qs_ctx("Disable", "notifier"))->setMenu(m_pDisablePopup);
}

void NotifierWindow::disableFor15Minutes()
{
	kvi_time_t tNow = kvi_unixTime();
	g_tNotifierDisabledUntil = tNow + (60 * 15);

	hideNow();
}

void NotifierWindow::disableFor5Minutes()
{
	kvi_time_t tNow = kvi_unixTime();
	g_tNotifierDisabledUntil = tNow + (60 * 5);

	hideNow();
}

void NotifierWindow::disableFor1Minute()
{
	kvi_time_t tNow = kvi_unixTime();
	g_tNotifierDisabledUntil = tNow + 60;

	hideNow();
}

void NotifierWindow::disableFor30Minutes()
{
	kvi_time_t tNow = kvi_unixTime();
	g_tNotifierDisabledUntil = tNow + (60 * 30);

	hideNow();
}

void NotifierWindow::disableFor60Minutes()
{
	kvi_time_t tNow = kvi_unixTime();
	g_tNotifierDisabledUntil = tNow + (60 * 60);

	hideNow();
}

void NotifierWindow::disableUntilKVIrcRestarted()
{
	kvi_time_t tNow = kvi_unixTime();
	// in fact we just disable for one year
	g_tNotifierDisabledUntil = tNow + (3600 * 24 * 360); // one year :D
	// this will NOT work if the module is forcibly unloaded with /notifier.unload
	// but the user should never do it :D
	hideNow();
}

void NotifierWindow::disablePermanently()
{
	KVI_OPTION_BOOL(KviOption_boolEnableNotifier) = false;
	hideNow();
}

void NotifierWindow::toggleLineEdit()
{
	showLineEdit(!m_pLineEdit->isVisible());
}

void NotifierWindow::showLineEdit(bool bShow)
{
	if(bShow)
	{
		if(m_pLineEdit->isVisible())
			return;
		if(!((NotifierWindowTab *)m_pWndTabs->currentWidget())->wnd())
			return;
		m_pLineEdit->setToolTip("");

		QString szTip = __tr2qs_ctx("Write text or commands to window", "notifier");
		szTip += " \"";
		szTip += ((NotifierWindowTab *)m_pWndTabs->currentWidget())->wnd()->plainTextCaption();
		szTip += "\"";
		m_pLineEdit->setToolTip(szTip);
		m_pLineEdit->show();
		m_pLineEdit->setFocus();
		activateWindow();
	}
	else
	{
		if(!m_pLineEdit->isVisible())
			return;
		m_pLineEdit->hide();
		setFocus();
		update();
	}
}

bool NotifierWindow::eventFilter(QObject * pEdit, QEvent * e)
{
	if(pEdit != (QObject *)m_pLineEdit)
		return false; // huh ?
	if(!m_pLineEdit->isVisible())
		return false;
	if(e->type() == QEvent::MouseButtonPress)
	{
		bool bWasBlinkOn = m_bBlinkOn;
		m_bBlinkOn = false;
		m_tAutoHideAt = 0;
		stopAutoHideTimer();
		stopBlinkTimer();
		activateWindow();
		m_pLineEdit->setFocus();
		if(bWasBlinkOn)
			update();
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

void NotifierWindow::returnPressed()
{
	if(!m_pLineEdit->isVisible())
		return;

	NotifierWindowTab * pTab = (NotifierWindowTab *)m_pWndTabs->currentWidget();
	if(!pTab)
		return;
	if(!pTab->wnd())
		return;

	QString szTxt = m_pLineEdit->text();
	if(szTxt.isEmpty())
		return;
	QString szHtml = szTxt;
	szHtml.replace("<", "&lt;");
	szHtml.replace(">", "&gt;");
	KviCString szTmp(KviCString::Format, "%d", KviIconManager::OwnPrivMsg);

	addMessage(pTab->wnd(), szTmp.ptr(), szHtml, 0);
	m_pLineEdit->setText("");
	KviUserInput::parse(szTxt, pTab->wnd(), QString(), true);
}

void NotifierWindow::progressUpdate()
{
	kvi_time_t now = kvi_unixTime();
	int iProgress = (int)(100 / (m_tAutoHideAt - m_tStartedAt) * (now - m_tStartedAt));
	m_pProgressBar->setValue(iProgress);
	if(now >= m_tAutoHideAt)
	{
		m_tAutoHideAt = 0;
		stopAutoHideTimer();
		doHide(true);
	}
}

void NotifierWindow::slotTabCloseRequested(int iIndex)
{
	if(m_pWndTabs)
	{
		NotifierWindowTab * pTab = (NotifierWindowTab *)m_pWndTabs->widget(iIndex);
		m_pWndTabs->removeTab(iIndex);
		if(pTab)
			pTab->deleteLater();

		if(m_pWndTabs->count() == 0)
			hideNow();
	}
}
