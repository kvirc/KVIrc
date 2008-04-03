//===============================================================================================
//
//   File : kvi_taskbar.cpp
//   Last major modification : Thu Jan 7 1999 03:59:43 CEST by Szymon Stefanek
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
//===============================================================================================

#define __KVIRC__
//#define KVI_TASKBARBUTTON_MIN_WIDTH 100
#define KVI_TASKBARBUTTON_CONTEXTINDICATORHEIGHT 6
#define KVI_TASKBAR_MIN_WIDTH 120

#define KVI_NUM_STEPS 20
#define KVI_TIMER_DELAY 18


#define _KVI_DEBUG_CHECK_RANGE_
#include "kvi_debug.h"
#include "kvi_taskbar.h"
#include "kvi_frame.h"
#include "kvi_window.h"
#include "kvi_string.h"
#include "kvi_options.h"
#include "kvi_console.h"
#include "kvi_locale.h"
#include "kvi_dynamictooltip.h"
#include "kvi_settings.h"
#include "kvi_channel.h"
#include "kvi_ircconnection.h"
#include "kvi_doublebuffer.h"
#include "kvi_tal_popupmenu.h"

// FIXME: #warning "The tree taskbar min width should be configurable"
#include <QTimer>
#include <QFontMetrics>
#include <QPainter>
#include <QPixmap>
#include <QImage>
#include <QCursor>
#include <QPaintEvent>
#include <QEvent>

// FIXME: Qt4 #include <QHeaderView>
#include <q3header.h>


#ifdef COMPILE_PSEUDO_TRANSPARENCY
	extern QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif

extern QPixmap * g_pActivityMeterPixmap;

// FIXME: This should be renamed to "Window List"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KviTaskBarBase
//

KviTaskBarBase::KviTaskBarBase()
: QDockWidget(__tr2qs("Taskbar"),g_pFrame)
{
	// FIXME: this timer should be started only if KVI_OPTION_BOOL(KviOption_boolUseTaskBarActivityMeter)
	setObjectName(__tr2qs("taskbar"));
	setFeatures(QDockWidget::DockWidgetMovable);
	m_pActivityMeterTimer = new QTimer();
	connect(m_pActivityMeterTimer,SIGNAL(timeout()),this,SLOT(updateActivityMeter()));
	m_pActivityMeterTimer->start(5000);
}

KviTaskBarBase::~KviTaskBarBase()
{
	delete m_pActivityMeterTimer;
}

void KviTaskBarBase::updateActivityMeter()
{
}

void KviTaskBarBase::getTextForConsole(QString &szText,KviConsole * pConsole)
{
	if(pConsole->isConnected())
	{
		if(KVI_OPTION_BOOL(KviOption_boolShowNetworkNameForConsoleTaskBarEntry))
		{
			// FIXME: Should never show "Standalone Servers" or "orphan_servers".
			//        It would also be nice to have a number appended to
			//        multiple entries with the same server name...but this costs too much.
			szText = pConsole->connection()->networkName();
			if(szText.isEmpty())
				szText = pConsole->connection()->currentServerName();
		} else {
			szText = pConsole->connection()->currentServerName();
		}
	} else {
		szText = pConsole->statusString();
	}
}


KviTaskBarItem *  KviTaskBarBase::item(int number)
{
	KviTaskBarItem * it = firstItem();
	if(!setIterationPointer(it))return 0;

	while(it && (number > 0))
	{
		it = nextItem();
		number--;
	}
	return it;
}

void KviTaskBarBase::switchWindow(bool bNext,bool bInContextOnly)
{
	if(!g_pActiveWindow)return;

	KviConsole * cons = g_pActiveWindow->console();
	KviTaskBarItem * cur = g_pActiveWindow->taskBarItem();
	KviTaskBarItem * it = cur;

	if(!setIterationPointer(cur))return;

	while(it)
	{
		it = bNext ? nextItem() : prevItem();
		if(!it)it = bNext ? firstItem() : lastItem();
		if(it)
		{
			if(bInContextOnly)
			{
				if(it->kviWindow()->console() == cons)
				{
					g_pFrame->setActiveWindow(it->kviWindow());
					return;
				}
			} else {
				g_pFrame->setActiveWindow(it->kviWindow());
				return;
			}
		}
		if(it == cur)return; // did a complete loop.... no window to switch to
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KviTaskBarItem
//

KviTaskBarItem::KviTaskBarItem(KviWindow * wnd)
{
	m_pWindow = wnd;
	m_iHighlightLevel = 0;
	m_iProgress = -1;
}

KviTaskBarItem::~KviTaskBarItem()
{
	m_pWindow->m_pTaskBarItem = 0;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KviTaskBarButton
//


KviTaskBarButton::KviTaskBarButton(QWidget * par,KviWindow * wnd,const char * name)
: QPushButton(par,name) , KviTaskBarItem(wnd)
{
	m_bActive         = false;
	m_pTip            = new KviDynamicToolTip(this);
	connect(m_pTip,SIGNAL(tipRequest(KviDynamicToolTip *,const QPoint &)),this,SLOT(tipRequest(KviDynamicToolTip *,const QPoint &)));
//	setBackgroundMode(QWidget::NoBackground);
	setToggleButton (true);
	setFlat ( KVI_OPTION_BOOL(KviOption_boolUseFlatClassicTaskbarButtons) );
}

KviTaskBarButton::~KviTaskBarButton()
{
	delete m_pTip; //not necessary ?
}

void KviTaskBarButton::tipRequest(KviDynamicToolTip *,const QPoint &pnt)
{
	if(KVI_OPTION_BOOL(KviOption_boolShowTaskBarToolTips))
	{
		QString szText;
		m_pWindow->getTaskBarTipText(szText);
		m_pTip->tip(rect(),szText);
	}
}

void KviTaskBarButton::mousePressEvent(QMouseEvent *e)
{
	if(e->button() & Qt::LeftButton)
	{
		if(e->state() & Qt::ShiftButton)
		{
			m_pWindow->delayedClose();
		} else {
			if((g_pActiveWindow != m_pWindow)  || (m_pWindow->isMinimized()))g_pFrame->setActiveWindow(m_pWindow);
			else m_pWindow->minimize();
		}
	} else m_pWindow->contextPopup();
}

void KviTaskBarButton::contextMenuEvent(QContextMenuEvent *e)
{
	m_pWindow->contextPopup();
	e->accept();
}

void KviTaskBarButton::setActive(bool bActive)
{
	if(bActive)
	{
		m_bActive         = true;
		m_iHighlightLevel = 0;
	} else {
		if(m_bActive)
		{
			// was active... unset the highlighting if it was silently turned on while being active...
			m_iHighlightLevel = 0;
		}
		m_bActive      = false;
	}
	setOn(bActive);
	update();
}

void KviTaskBarButton::paintEvent(QPaintEvent * e)
{
	QPainter p(this);
	QStyleOption opt;
	opt.initFrom(this);
	if(isOn())
		opt.state = QStyle::State_On | QStyle::State_Active;
	style()->drawPrimitive(QStyle::PE_PanelButtonTool,&opt,&p,this);
	drawButtonLabel(&p);
}

void KviTaskBarButton::drawButtonLabel(QPainter * painter)
{
	QRect distRect = painter->window();
	int iHeight = distRect.height();
	int iWidth = distRect.width();

	QPainter * pPainter;
	pPainter = painter;

	if(KVI_OPTION_BOOL(KviOption_boolUseTaskBarIrcContextIndicator))
	{
		iHeight -= KVI_TASKBARBUTTON_CONTEXTINDICATORHEIGHT;
		QColor base = colorGroup().background();
		if(m_pWindow->console())
		{
			QColor cntx = KVI_OPTION_ICCOLOR(m_pWindow->console()->ircContextId() % KVI_NUM_ICCOLOR_OPTIONS);
			base.setRgb((base.red() + cntx.red()) >> 1,(base.green() + cntx.green()) >> 1,
				(base.blue() + cntx.blue()) >> 1);
			pPainter->fillRect(2,iHeight,iWidth - 4,KVI_TASKBARBUTTON_CONTEXTINDICATORHEIGHT - 2,base);
		} else {
			pPainter->fillRect(2,iHeight,iWidth - 4,KVI_TASKBARBUTTON_CONTEXTINDICATORHEIGHT - 2,colorGroup().brush(QColorGroup::Background));
		}
	}

	int daX = 3;

	if(KVI_OPTION_BOOL(KviOption_boolUseTaskBarIcons))
	{
		pPainter->drawPixmap(3,3,*(m_pWindow->myIconPtr()));
		daX = 20;
	}

	if(KVI_OPTION_BOOL(KviOption_boolUseTaskBarActivityMeter))
	{
		unsigned int uActivityValue;
		unsigned int uActivityTemperature;
		if(m_pWindow->activityMeter(&uActivityValue,&uActivityTemperature))
		{
			pPainter->drawPixmap(daX,3,*g_pActivityMeterPixmap,uActivityValue * 5,uActivityTemperature * 16,5,16);
			daX = 27;
		}
	}

	QRect cRect(daX,3,iWidth - (20 + daX),iHeight - 6);

	if(m_iProgress >= 0)
	{
		// paint the progress bar
		int wdth = (m_iProgress * cRect.width()) / 100;
		pPainter->setPen(KVI_OPTION_COLOR(KviOption_colorTaskBarProgressBar));
		pPainter->drawRect(cRect);
		pPainter->fillRect(daX,3,wdth,cRect.height(),KVI_OPTION_COLOR(KviOption_colorTaskBarProgressBar));
	}

	QRect bRect;
	QString szText;

	bool bMinimized = m_pWindow->isMinimized();

	if(m_bActive)
	{
		pPainter->setPen(KVI_OPTION_COLOR(KviOption_colorTaskBarNormalText));
	} else {
		int iLevel;
	
		switch(m_iHighlightLevel)
		{
			case 0: iLevel = bMinimized ? KviOption_colorTaskBarMinimizedText : KviOption_colorTaskBarNormalText; break;
			case 1: iLevel = KviOption_colorTaskBarHighlight1Text; break;
			case 2: iLevel = KviOption_colorTaskBarHighlight2Text; break;
			case 3: iLevel = KviOption_colorTaskBarHighlight3Text; break;
			case 4: iLevel = KviOption_colorTaskBarHighlight4Text; break;
			default: iLevel = KviOption_colorTaskBarHighlight5Text; break;
		}
		pPainter->setPen(KVI_OPTION_COLOR(iLevel));
	}

	pPainter->setFont(KVI_OPTION_FONT(KviOption_fontTaskbar));

	switch(m_pWindow->type())
	{
		case KVI_WINDOW_TYPE_CONSOLE:
		{
			QFont f = QFont(KVI_OPTION_FONT(KviOption_fontTaskbar));
			f.setBold(true);
			pPainter->setFont(f);
			KviTaskBarBase::getTextForConsole(szText,(KviConsole *)m_pWindow);
		}
		break;
		case KVI_WINDOW_TYPE_CHANNEL:
		case KVI_WINDOW_TYPE_DEADCHANNEL:
			szText = ((KviChannel *)m_pWindow)->nameWithUserFlag();
		break;
		case KVI_WINDOW_TYPE_QUERY:
		case KVI_WINDOW_TYPE_DEADQUERY:
			szText = m_pWindow->windowName();
		break;
		default:
			szText = m_pWindow->plainTextCaption();
		break;
	}

	if(bMinimized)
	{
		QString tmp = QChar('(');
		tmp += szText;
		tmp += QChar(')');
		pPainter->drawText(cRect,Qt::AlignLeft | Qt::AlignTop,tmp,-1,&bRect);
	} else {
		pPainter->drawText(cRect,Qt::AlignLeft | Qt::AlignTop,szText,-1,&bRect);
	}

	if(bRect.width() > cRect.width())
	{
		pPainter->setClipRect(cRect.right(),cRect.y(),10,cRect.height());
		QColor base = pPainter->pen().color();
		QColor bg   = colorGroup().color(QColorGroup::Background);
		base.setRgb((base.red() + bg.red()) / 2,(base.green() + bg.green()) / 2,(base.blue() + bg.blue()) / 2);
		pPainter->setPen(base);
		cRect.setWidth(cRect.width() + 10);
		pPainter->drawText(cRect,Qt::AlignLeft | Qt::AlignTop,szText,-1);
		pPainter->setClipRect(cRect.right(),cRect.y(),5,cRect.height());
		base.setRgb((base.red() + bg.red()) / 2,(base.green() + bg.green()) / 2,(base.blue() + bg.blue()) / 2);
		pPainter->setPen(base);
		cRect.setWidth(cRect.width() + 10);
		pPainter->drawText(cRect,Qt::AlignLeft | Qt::AlignTop,szText,-1);
	}
}

void KviTaskBarButton::captionChanged()
{
	update();
}

void KviTaskBarButton::setProgress(int progress)
{
	if(progress == m_iProgress)return;
	m_iProgress = progress;
	update(); // repaint(false) ?
}

void KviTaskBarButton::unhighlight()
{
	if(m_iHighlightLevel < 1)return;
	m_iHighlightLevel = 0;
	if(g_pFrame->dockExtension())g_pFrame->dockExtension()->refresh();
	update();
}

void KviTaskBarButton::highlight(int iLevel)
{
	if(iLevel <= m_iHighlightLevel)return;
	if(m_bActive && g_pFrame->isActiveWindow())return;
	m_iHighlightLevel = iLevel;
	if(g_pFrame->dockExtension())g_pFrame->dockExtension()->refresh();
	if(m_bActive)return;
	update(); // repaint(false) ?
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KviTaskBarToolTip
//
/*
KviTaskBarToolTip::KviTaskBarToolTip(KviTaskBarButton * b)
: KviTalToolTip(b,0)
{
	m_pTaskBarButton = b;
}

KviTaskBarToolTip::~KviTaskBarToolTip()
{
}

void KviTaskBarToolTip::maybeTip(const QPoint &pnt)
{
	tip(m_pTaskBarButton->rect(),m_pTaskBarButton->m_pWindow->plainTextCaption());
}
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KviClasicTaskBar
//


KviClassicTaskBar::KviClassicTaskBar()
: KviTaskBarBase()
{
	m_pButtonList = new KviPointerList<KviTaskBarButton>;
	m_pButtonList->setAutoDelete(true);

	calcButtonHeight();

	m_pBase = new QWidget(this);
//	m_pBase->setBackgroundMode(NoBackground);
	setWidget(m_pBase);

	m_pBase->setMinimumWidth(KVI_TASKBAR_MIN_WIDTH);
	setMinimumWidth(KVI_TASKBAR_MIN_WIDTH);
	//m_pBase->setMinimumWidth(KVI_TASKBAR_MIN_WIDTH);
	//setMinimumWidth(KVI_TASKBAR_MIN_WIDTH);

	m_pBase->setMinimumHeight(m_iButtonHeight+5);
	setMinimumHeight(m_iButtonHeight+5);
}

KviClassicTaskBar::~KviClassicTaskBar()
{
	delete m_pButtonList;
	m_pButtonList = 0;
}

void KviClassicTaskBar::orientationChangedSlot(Qt::Orientation o)
{
	doLayout();
}

/*
void KviClassicTaskBar::fontChange(const QFont &old)
{
	calcButtonHeight();
	doLayout();
	QToolBar::fontChange(old);
}
*/

void KviClassicTaskBar::updateActivityMeter()
{
	if(KVI_OPTION_BOOL(KviOption_boolUseTaskBarActivityMeter))
	{
		for(KviTaskBarButton * btn = m_pButtonList->first();btn;btn = m_pButtonList->next())
			btn->update();
	}
}

void KviClassicTaskBar::calcButtonHeight()
{
	QFontMetrics fm(KVI_OPTION_FONT(KviOption_fontTaskbar));
	m_iButtonHeight = fm.lineSpacing() + 6;
	if(m_iButtonHeight < 22)m_iButtonHeight = 22;
	if(KVI_OPTION_BOOL(KviOption_boolUseTaskBarIrcContextIndicator))
		m_iButtonHeight += KVI_TASKBARBUTTON_CONTEXTINDICATORHEIGHT;
}

void KviClassicTaskBar::insertButton(KviTaskBarButton * b)
{
	int idx = 0;
//	if(KVI_OPTION_BOOL(KviOption_boolSortTaskbarButtons))
//	{
		// first sort by irc context
		for(KviTaskBarButton * btn = m_pButtonList->first();btn;btn = m_pButtonList->next())
		{
			if(btn->kviWindow()->console() == b->kviWindow()->console())
			{
				// same irc context (or none)
				// sort by type now
				for(;btn;btn = m_pButtonList->next())
				{
					if((btn->kviWindow()->type() > b->kviWindow()->type()) ||
						(btn->kviWindow()->console() != b->kviWindow()->console()))
					{
						// greater type or another irc context
						m_pButtonList->insert(idx,b);
						return;
					} else if(btn->kviWindow()->type() == b->kviWindow()->type())
					{
						// same type!
						// sort by name
						if(!KVI_OPTION_BOOL(KviOption_boolSortTaskBarItemsByName) || (KviQString::cmpCI(btn->kviWindow()->windowName(),b->kviWindow()->windowName()) > 0))
						{
							// got a "higher one"
							m_pButtonList->insert(idx,b);
							return;
						}
					}
					idx++;
				}
				// ran out of buttons
				m_pButtonList->append(b);
				return;
			} else {
				if(!(btn->kviWindow()->console()) && b->kviWindow()->console())
				{
					// this must be a new console...insert before the contextless windows
					__range_valid(b->kviWindow()->console() == b->kviWindow());
					m_pButtonList->insert(idx,b);
					return;
				} else idx++; // wrong irc contet...go on searching
			}
		}
//	}
	// no sorting selected , or no match for this irc context
	m_pButtonList->append(b);
}


KviTaskBarItem * KviClassicTaskBar::addItem(KviWindow * wnd)
{
	KviTaskBarButton * b = new KviTaskBarButton(m_pBase,wnd,"");
	insertButton(b);
	b->show();
	doLayout();
	if(g_pFrame->dockExtension())g_pFrame->dockExtension()->refresh();
/*	if(b->width() < m_pBase->width()) m_pBase->setMinimumWidth(b->width());
	if(b->height() < m_pBase->height()) m_pBase->setMinimumWidth(b->height());*/
	return b;
}

bool KviClassicTaskBar::removeItem(KviTaskBarItem * it)
{
	if(it)
	{
		m_pButtonList->removeRef((KviTaskBarButton *)it);
		doLayout();
		if(g_pFrame->dockExtension())g_pFrame->dockExtension()->refresh();
	}
	return true;
}

void KviClassicTaskBar::setActiveItem(KviTaskBarItem * it)
{
	if(it)
	{
		for(KviTaskBarButton * b = m_pButtonList->first();b;b = m_pButtonList->next())
		{
			b->setActive(((KviTaskBarButton *)it) == b);
		}
		if(g_pFrame->dockExtension())g_pFrame->dockExtension()->refresh();
	}
}


void KviClassicTaskBar::doLayout()
{
	if(!m_pButtonList->count())return;

	if(!m_pBase->isVisible())
	{
		// handle a windows bug: sometimes this ugly thing happens
		// this shouldn't hurt on other platforms
		if(isVisible())m_pBase->show();
	}

	int baseWidth = m_pBase->width();
	int btnsInRow = baseWidth / KVI_OPTION_UINT(KviOption_uintTaskBarButtonMinWidth);
	int totCount = m_pButtonList->count();
	//int btnsInRow = totCount * totCount * btnWidth * btnWidth / (wdth * wdth);
	if(btnsInRow < 1)btnsInRow = 1;

	int rows = 0;
	while(totCount > 0)
	{
		rows++;
		totCount -= btnsInRow;
	}

	if(isFloating() || ((g_pFrame->dockWidgetArea(this) != Qt::BottomDockWidgetArea) && (g_pFrame->dockWidgetArea(this) != Qt::TopDockWidgetArea)))
	{
		QDockWidget::DockWidgetFeatures f = features();
		if(f & QDockWidget::DockWidgetVerticalTitleBar)
		{
			f &= ~QDockWidget::DockWidgetVerticalTitleBar;
			setFeatures(f);
		}
	} else {
		QDockWidget::DockWidgetFeatures f = features();
		if(!(f & QDockWidget::DockWidgetVerticalTitleBar))
		{
			f |= QDockWidget::DockWidgetVerticalTitleBar;
			setFeatures(f);
		}
	}

	if ((width() > height()) && 
		(((unsigned int)rows) > m_pBase->height() / m_iButtonHeight ))
	{
		rows = m_pBase->height() / m_iButtonHeight;
	}
	if(rows==0) rows=1;
	totCount = m_pButtonList->count();
	btnsInRow = totCount / rows;
	if(totCount % rows)btnsInRow++;

	//m_pBase->setMinimumHeight(rows * m_iButtonHeight);

	int theWidth      = 0;
	int theX          = 0;
	int theY          = -m_iButtonHeight;
	int btnIdx        = 0;
	int btnInRow      = 1;
	for(KviTaskBarButton * b = m_pButtonList->first();b;b = m_pButtonList->next())
	{
		if((btnIdx % btnsInRow) == 0)
		{
			int inRow = ((totCount < btnsInRow ) ? totCount : btnsInRow);
			theWidth = baseWidth / (inRow ? inRow : 1);
			theX     = 0;
			theY    += m_iButtonHeight;
			btnInRow = 1;
		} else {
			if((btnInRow == btnsInRow) || (totCount == 1))theWidth = baseWidth - theX;
		}
		
		if( KVI_OPTION_BOOL(KviOption_boolClassicTaskBarSetMaximumButtonWidth) && (theWidth > KVI_OPTION_UINT(KviOption_uintClassicTaskBarMaximumButtonWidth)) && (width() > height())
			)
				theWidth = KVI_OPTION_UINT(KviOption_uintClassicTaskBarMaximumButtonWidth);

		b->setGeometry(theX,theY,theWidth,m_iButtonHeight);

		if(btnInRow != btnsInRow)
		{
			theX += theWidth;
			btnInRow++;
		}
		btnIdx++;
		totCount--;
	}
}

void KviClassicTaskBar::applyOptions()
{
	for(KviTaskBarButton * b = m_pButtonList->first();b;b = m_pButtonList->next())
	{
		b->setFlat(KVI_OPTION_BOOL(KviOption_boolUseFlatClassicTaskbarButtons));
	}
	doLayout();
}

void KviClassicTaskBar::resizeEvent(QResizeEvent *e)
{
/*
	if(orientation() == Qt::Horizontal)
	{
		int iRows = height()/m_iButtonHeight;
		if(!iRows) iRows=1;
		debug("%i %i",height(),iRows);
		resize(width(),iRows*m_iButtonHeight);
	}
*/
	KviTaskBarBase::resizeEvent(e);
	doLayout();
}

KviTaskBarItem * KviClassicTaskBar::firstItem()
{
	return m_pButtonList->first();
}

KviTaskBarItem * KviClassicTaskBar::lastItem(void)
{
	return m_pButtonList->last();
}

KviTaskBarItem * KviClassicTaskBar::nextItem()
{
	return m_pButtonList->next();
}

KviTaskBarItem * KviClassicTaskBar::prevItem(void)
{
	return m_pButtonList->prev();
}

bool KviClassicTaskBar::setIterationPointer(KviTaskBarItem * it)
{
	return (m_pButtonList->findRef((const KviTaskBarButton *)it) != -1);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KviTreeTaskBarItem
//


KviTreeTaskBarItem::KviTreeTaskBarItem(KviTalListView * par,KviWindow * wnd)
: KviTalListViewItem(par) , KviTaskBarItem(wnd)
{
	m_iStepNumber=0;
	m_bIncreasing=0;
	m_pInternal=new KviTreeTaskBarItemInternal(this);
	m_pAnimTimer=new QTimer();
	QObject::connect( m_pAnimTimer, SIGNAL(timeout()), m_pInternal, SLOT(timerShot()));
	applyOptions();
}

KviTreeTaskBarItem::KviTreeTaskBarItem(KviTreeTaskBarItem * par,KviWindow * wnd)
: KviTalListViewItem(par) , KviTaskBarItem(wnd)
{
	m_iStepNumber=0;
	m_bIncreasing=0;
	m_pInternal=new KviTreeTaskBarItemInternal(this);
	m_pAnimTimer=new QTimer();
	QObject::connect( m_pAnimTimer, SIGNAL(timeout()), m_pInternal, SLOT(timerShot()));
	applyOptions();
}

int KviTreeTaskBarItem::calculateColor(int col1,int col2)
{
	int result=col1+(col2-col1)/KVI_NUM_STEPS*m_iStepNumber;
	return result<255 ? result :255;
}

KviTreeTaskBarItem::~KviTreeTaskBarItem()
{
	KviTalListView* pView=(KviTalListView *)listView();
	if(pView)
		if(((KviTreeTaskBarListView*)(pView))->m_pPrevItem==this) ((KviTreeTaskBarListView*)(listView()))->m_pPrevItem=0;
	delete m_pAnimTimer;
	delete m_pInternal;
}

void KviTreeTaskBarItem::applyOptions()
{
	m_iRedDiff=(KVI_OPTION_COLOR(KviOption_colorTreeTaskBarActiveBackground).red()-KVI_OPTION_COLOR(KviOption_colorTreeTaskBarBackground).red())/KVI_NUM_STEPS;
	m_iGreenDiff=(KVI_OPTION_COLOR(KviOption_colorTreeTaskBarActiveBackground).green()-KVI_OPTION_COLOR(KviOption_colorTreeTaskBarBackground).green())/KVI_NUM_STEPS;
	m_iBlueDiff=(KVI_OPTION_COLOR(KviOption_colorTreeTaskBarActiveBackground).blue()-KVI_OPTION_COLOR(KviOption_colorTreeTaskBarBackground).blue())/KVI_NUM_STEPS;
}

void KviTreeTaskBarItem::captionChanged()
{
	// FIXME: can we do better ?
	repaint();
}

void KviTreeTaskBarItem::unhighlight()
{
	if(m_iHighlightLevel < 1)return;
	m_iHighlightLevel = 0;
	if(g_pFrame->dockExtension())g_pFrame->dockExtension()->refresh();
	repaint();
}

void KviTreeTaskBarItem::highlight(int iLevel)
{
	if(iLevel <= m_iHighlightLevel)return;
	if(isSelected() && g_pFrame->isActiveWindow())return;
	m_iHighlightLevel = iLevel;
	if(g_pFrame->dockExtension())g_pFrame->dockExtension()->refresh();
	if(isSelected())return;
	repaint(); // repaint(false) ?
}

void KviTreeTaskBarItem::setProgress(int progress)
{
	if(progress == m_iProgress)return;
	m_iProgress = progress;
	repaint(); // repaint(false) ?
}

void KviTreeTaskBarItem::setActive(bool bActive)
{
	if(bActive)
	{
//		m_bHighlighted = false;
//		m_bAltColor    = false;
		m_iHighlightLevel = 0;
		// was not selected: the listView will repaint it
	} else {
		if(isSelected())
		{
			// was active... unset the highlighting if it was silently turned on while being active...
//			m_bHighlighted = false;
//			m_bAltColor = false;
			m_iHighlightLevel = 0;
			// was selected: the list view will repaint it
		}
	}
}

void KviTreeTaskBarItem::paintBranches(QPainter *p,const QColorGroup &,int w,int y,int h)
{
	SET_ANTI_ALIASING(*p);
	((KviTreeTaskBarListView *)listView())->paintEmptyArea(p,QRect(0,y,w,totalHeight() - height()));
}

void KviTreeTaskBarItem::paintCell(QPainter *painter,const QColorGroup &cg,int column,int width,int)
{
	KviDoubleBuffer db(width,height());
	QPixmap * pMemBuffer = db.pixmap();
	QPainter p(pMemBuffer);
	SET_ANTI_ALIASING(p);
	
	if(isSelected())
	{
		p.fillRect(0,0,width,height(),KVI_OPTION_COLOR(KviOption_colorTreeTaskBarActiveBackground));
	} else {
		if(!m_iStepNumber)
		{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
			if(g_pShadedChildGlobalDesktopBackground)
			{
				QPoint pnt = listView()->viewport()->mapToGlobal(QPoint(int(painter->worldMatrix().dx()),int(painter->worldMatrix().dy())));
				p.drawTiledPixmap(0,0,width,height(),*g_pShadedChildGlobalDesktopBackground,pnt.x(),pnt.y());
			} else {
#endif
				p.fillRect(0,0,width,height(),KVI_OPTION_COLOR(KviOption_colorTreeTaskBarBackground));
#ifdef COMPILE_PSEUDO_TRANSPARENCY
			}
#endif		
			QPixmap * pix = KVI_OPTION_PIXMAP(KviOption_pixmapTreeTaskBarBackground).pixmap();
			if(pix)
			{
				QPoint pnt = listView()->viewportToContents(QPoint(int(painter->worldMatrix().dx()),int(painter->worldMatrix().dy())));
				//p.drawTiledPixmap(0,0,width,height(),*pix,pnt.x(),pnt.y());
//				debug("%i %i",pnt.x(),pnt.y());
				p.translate(-pnt.x(),-pnt.y());
				KviPixmapUtils::drawPixmapWithPainter(&p,pix,KVI_OPTION_UINT(KviOption_uintTreeTaskBarPixmapAlign),QRect(pnt.x(),pnt.y(),width,height()),listView()->width(),listView()->height());
				p.translate(pnt.x(),pnt.y());
			}
		} else {
			p.fillRect(0,0,width,height(),
				QColor(KVI_OPTION_COLOR(KviOption_colorTreeTaskBarBackground).red()+m_iRedDiff*m_iStepNumber,
					KVI_OPTION_COLOR(KviOption_colorTreeTaskBarBackground).green()+m_iGreenDiff*m_iStepNumber,
					KVI_OPTION_COLOR(KviOption_colorTreeTaskBarBackground).blue()+m_iBlueDiff*m_iStepNumber
					)
				);
			
		}
	}

	int h = height();
	int im = listView()->itemMargin();
	int yPixmap = (h - 16) >> 1;

	QString szText;

	QRect cRect(im + 3,0,width - (im << 1),height());

	switch(m_pWindow->type())
	{
		case KVI_WINDOW_TYPE_CONSOLE:
		{
			if(KVI_OPTION_BOOL(KviOption_boolUseTaskBarIrcContextIndicator))
			{
				QColor base = cg.background();
				QColor cntx = KVI_OPTION_ICCOLOR(m_pWindow->console()->ircContextId() % KVI_NUM_ICCOLOR_OPTIONS);
				base.setRgb((base.red() + cntx.red()) >> 1,(base.green() + cntx.green()) >> 1,
					(base.blue() + cntx.blue()) >> 1);
				p.fillRect(im + 2,yPixmap + 1,14,15,base);
				//draw_frame_helper(&p,im + 1,yPixmap,im + 15,yPixmap + 15,base.light(180),base.dark());
				if(KVI_OPTION_BOOL(KviOption_boolUseTaskBarIcons))
				{
					p.drawPixmap(im + 20,yPixmap,*(m_pWindow->myIconPtr()));
					cRect.setLeft(cRect.left() + 37);
				} else {
					cRect.setLeft(cRect.left() + 20);
				}
			} else {
				if(KVI_OPTION_BOOL(KviOption_boolUseTaskBarIcons))
				{
					p.drawPixmap(im,yPixmap,*(m_pWindow->myIconPtr()));
					cRect.setLeft(cRect.left() + 17);
				}
			}
			QFont f = QFont();
			f.setBold(true);
			p.setFont(f);
			KviTaskBarBase::getTextForConsole(szText,(KviConsole *)m_pWindow);
		}
		break;
		case KVI_WINDOW_TYPE_CHANNEL:
		case KVI_WINDOW_TYPE_DEADCHANNEL:
			szText = ((KviChannel *)m_pWindow)->nameWithUserFlag();
			if(KVI_OPTION_BOOL(KviOption_boolUseTaskBarIcons))
			{
				p.drawPixmap(im,yPixmap,*(m_pWindow->myIconPtr()));
				cRect.setLeft(cRect.left() + 17);
			}
		break;
		case KVI_WINDOW_TYPE_QUERY:
		case KVI_WINDOW_TYPE_DEADQUERY:
			szText = m_pWindow->windowName();
			if(KVI_OPTION_BOOL(KviOption_boolUseTaskBarIcons))
			{
				p.drawPixmap(im,yPixmap,*(m_pWindow->myIconPtr()));
				cRect.setLeft(cRect.left() + 17);
			}
		break;
		default:
			szText = m_pWindow->plainTextCaption();
			if(KVI_OPTION_BOOL(KviOption_boolUseTaskBarIcons))
			{
				p.drawPixmap(im,yPixmap,*(m_pWindow->myIconPtr()));
				cRect.setLeft(cRect.left() + 17);
			}
		break;
	}

	if(KVI_OPTION_BOOL(KviOption_boolUseTaskBarActivityMeter))
	{
		unsigned int uActivityValue;
		unsigned int uActivityTemperature;
		if(m_pWindow->activityMeter(&uActivityValue,&uActivityTemperature))
		{
			p.drawPixmap(cRect.left(),yPixmap,*g_pActivityMeterPixmap,uActivityValue * 5,uActivityTemperature * 16,5,16);
			cRect.setLeft(cRect.left() + 7);
		}
	}

	if(m_iProgress >= 0)
	{
		// paint the progress bar
		int wdth = (m_iProgress * cRect.width()) / 100;
		p.fillRect(cRect.x(),cRect.y(),wdth,cRect.height(),KVI_OPTION_COLOR(KviOption_colorTreeTaskBarProgress));
	}

	if(isSelected())
	{
		p.setPen(KVI_OPTION_COLOR(KviOption_colorTreeTaskBarActiveForeground));
	} else {
		int iLevel;
		switch(m_iHighlightLevel)
		{
			case 0: iLevel = KviOption_colorTreeTaskBarForeground; break;
			case 1: iLevel = KviOption_colorTreeTaskBarHighlight1Foreground; break;
			case 2: iLevel = KviOption_colorTreeTaskBarHighlight2Foreground; break;
			case 3: iLevel = KviOption_colorTreeTaskBarHighlight3Foreground; break;
			case 4: iLevel = KviOption_colorTreeTaskBarHighlight4Foreground; break;
			default: iLevel = KviOption_colorTreeTaskBarHighlight5Foreground; break;
		}
		p.setPen(
			QColor(
				calculateColor(KVI_OPTION_COLOR(iLevel).red(),KVI_OPTION_COLOR(KviOption_colorTreeTaskBarActiveForeground).red()),
				calculateColor(KVI_OPTION_COLOR(iLevel).green(),KVI_OPTION_COLOR(KviOption_colorTreeTaskBarActiveForeground).green()),
				calculateColor(KVI_OPTION_COLOR(iLevel).blue(),KVI_OPTION_COLOR(KviOption_colorTreeTaskBarActiveForeground).blue())
				)
			);
	}

	if(m_pWindow->isMinimized())
	{
		QString tmp = QChar('(');
		tmp += szText;
		tmp += QChar(')');
		p.drawText(cRect,Qt::AlignLeft | Qt::AlignVCenter,tmp,-1,0);
	} else {
		p.drawText(cRect,Qt::AlignLeft | Qt::AlignVCenter,szText,-1,0);
	}
	
	painter->drawPixmap(0,0,*pMemBuffer,0,0,width,height());
	//bitBlt(painter->pixmap(),0,0,pMemBuffer,0,0,width,height(),Qt::CopyROP,false);
}

QString KviTreeTaskBarItem::key(int,bool) const
{
	QString ret = m_pWindow->typeString();
	ret.append(m_pWindow->windowName());
	return ret;
}

void KviTreeTaskBarItem::timerShot() 
{
	if(m_bIncreasing)
		m_iStepNumber++;
	else
		m_iStepNumber--;
	
	if((m_iStepNumber>=KVI_NUM_STEPS) && m_bIncreasing)
	{
		m_pAnimTimer->stop();
		m_iStepNumber=KVI_NUM_STEPS; //make shure, that we cannot get out of range
	} else if((m_iStepNumber<=0) && !m_bIncreasing) {
		m_pAnimTimer->stop();
		m_iStepNumber=0; //make shure, that we cannot get out of range
	}
	repaint();
}

void KviTreeTaskBarItem::mouseEnter()
{
	if(KVI_OPTION_BOOL(KviOption_boolEnableVisualEffects))
	{
		m_bIncreasing=true;
		if(!m_pAnimTimer->isActive()) m_pAnimTimer->start(KVI_TIMER_DELAY);
 	}
}

void KviTreeTaskBarItem::mouseLeave()
{
	if(KVI_OPTION_BOOL(KviOption_boolEnableVisualEffects))
	{
		m_bIncreasing=false;
		if(!m_pAnimTimer->isActive()) m_pAnimTimer->start(KVI_TIMER_DELAY);
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KviTreeTaskBarListView
//


KviTreeTaskBarListView::KviTreeTaskBarListView(QWidget * par)
: KviTalListView(par)
{
	//setSorting(0);
	setShowSortIndicator(true);
	setItemMargin(2);
	setFrameShape(NoFrame);
	viewport()->setMouseTracking(TRUE);
	m_pPrevItem=0;
	setHScrollBarMode(KviTalListView::AlwaysOff);
}

KviTreeTaskBarListView::~KviTreeTaskBarListView()
{
}

void KviTreeTaskBarListView::contentsMouseMoveEvent ( QMouseEvent * e ) 
{
	if(!e) return;
	KviTreeTaskBarItem* pCur=(KviTreeTaskBarItem*)(itemAt(contentsToViewport(e->pos())));
	if(pCur!=m_pPrevItem)
	{
		if(m_pPrevItem)m_pPrevItem->mouseLeave();
		if(pCur) pCur->mouseEnter();
		setCursor(Qt::PointingHandCursor);
		m_pPrevItem=pCur;
	} else if(!pCur) {
		setCursor(Qt::ArrowCursor);
	}
}
void KviTreeTaskBarListView::leaveEvent(QEvent *)
{
	if(m_pPrevItem) m_pPrevItem->mouseLeave();
	m_pPrevItem=0;
	setCursor(Qt::ArrowCursor);
}


void KviTreeTaskBarListView::contentsMousePressEvent(QMouseEvent *e)
{
	KviTalListViewItem * it = (KviTalListViewItem *)itemAt(contentsToViewport(e->pos()));
	if(it)
	{
		if(e->button() & Qt::LeftButton)emit leftMousePress(it);
		else if(e->button() & Qt::RightButton)emit rightMousePress(it);
	} else {
		if(e->button() & Qt::RightButton)
		{
			KviTalPopupMenu* pPopup=new KviTalPopupMenu();
			pPopup->insertItem(__tr2qs("Sort"),this,SLOT(sort()));
			pPopup->insertItem(__tr2qs("Reverse Sort"),this,SLOT(reverseSort()));
			pPopup->popup(QCursor::pos());
		}
	}
}

void KviTreeTaskBarListView::sort()
{
	setSorting(0,TRUE);
}

void KviTreeTaskBarListView::reverseSort()
{
	setSorting(0,FALSE);
}

void KviTreeTaskBarListView::resizeEvent(QResizeEvent *e)
{
	KviTalListView::resizeEvent(e);
	setColumnWidth(0,viewport()->width());
	resizeContents(viewport()->width(),contentsHeight());
}

//void KviTreeTaskBarListView::paintEmptyAreaInternal(QPainter * p,const QRect &viewportRect,const QRect &painterRect)
//{
//
//}

void KviTreeTaskBarListView::paintEmptyArea(QPainter * p,const QRect &rct)
{
	SET_ANTI_ALIASING(*p);
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt = viewport()->mapToGlobal(QPoint(rct.x() + int(p->worldMatrix().dx()),rct.y() + int(p->worldMatrix().dy())));
		p->drawTiledPixmap(rct.x(),rct.y(),rct.width(),rct.height(),*g_pShadedChildGlobalDesktopBackground,pnt.x(),pnt.y());
	} else {
#endif
		p->fillRect(rct.x(),rct.y(),rct.width(),rct.height(),KVI_OPTION_COLOR(KviOption_colorTreeTaskBarBackground));
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	}
#endif
	
	QPixmap * pix = KVI_OPTION_PIXMAP(KviOption_pixmapTreeTaskBarBackground).pixmap();
	if(pix)
	{
		QPoint pnt = viewportToContents(QPoint(rct.x() + int(p->worldMatrix().dx()),rct.y() + int(p->worldMatrix().dy())));
		//p->drawTiledPixmap(rct.x(),rct.y(),rct.width(),rct.height(),*pix,pnt.x(),pnt.y());
		KviPixmapUtils::drawPixmapWithPainter(p,pix,KVI_OPTION_UINT(KviOption_uintTreeTaskBarPixmapAlign),rct,viewport()->width(),viewport()->height(),pnt.x(),pnt.y());
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KviTreeTaskBar
//

KviTreeTaskBar::KviTreeTaskBar()
: KviTaskBarBase()
{
	m_pListView = new KviTreeTaskBarListView(this);
	m_pListView->addColumn(__tr2qs("Window List"),135);
	m_pListView->setAllColumnsShowFocus(true);
	m_pListView->setMultiSelection(false);
	setWidget(m_pListView);

	// FIXME: this code is useless ?
	if(KVI_OPTION_UINT(KviOption_uintTreeTaskBarMinimumWidth) < 48)
		KVI_OPTION_UINT(KviOption_uintTreeTaskBarMinimumWidth) = 48;
	int iMin = m_pListView->minimumSize().width() + 4;
	if(((unsigned int)iMin) < KVI_OPTION_UINT(KviOption_uintTreeTaskBarMinimumWidth))
		iMin = KVI_OPTION_UINT(KviOption_uintTreeTaskBarMinimumWidth);
	setMinimumWidth(iMin);

	// this is surely useful :)
	m_pListView->setMinimumWidth(4);

	//setMaximumWidth(KVI_OPTION_UINT(KviOption_uintTreeTaskBarMaximumWidth));
	//m_pListView->setMinimumWidth(KVI_OPTION_UINT(KviOption_uintTreeTaskBarMinimumWidth));
	//m_pListView->setMaximumWidth(KVI_OPTION_UINT(KviOption_uintTreeTaskBarMaximumWidth));
	m_pListView->setFocusPolicy(Qt::NoFocus);
	m_pListView->setStaticBackground(true);
	m_pListView->viewport()->setAutoFillBackground(false);

	if(!KVI_OPTION_BOOL(KviOption_boolShowTreeTaskbarHeader))
	{
		m_pListView->header()->hide();
	}
	
	m_pListView->header()->setResizeEnabled(true);

	setMaximumWidth(600);

	m_pListView->viewport()->installEventFilter(this);

	m_pToolTip = new KviDynamicToolTip(m_pListView->viewport(),"tree_taskbar_tooltip");
	connect(m_pToolTip,SIGNAL(tipRequest(KviDynamicToolTip *,const QPoint &)),this,SLOT(tipRequest(KviDynamicToolTip *,const QPoint &)));
}

KviTreeTaskBar::~KviTreeTaskBar()
{
}

void KviTreeTaskBar::updatePseudoTransparency()
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	m_pListView->viewport()->update();
#endif
}

void KviTreeTaskBar::moveEvent(QMoveEvent *)
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	updatePseudoTransparency();
#endif
}

void KviTreeTaskBar::tipRequest(KviDynamicToolTip *,const QPoint &pnt)
{
	if(KVI_OPTION_BOOL(KviOption_boolShowTaskBarToolTips))
	{
		KviTalListViewItem * it = (KviTalListViewItem *)m_pListView->itemAt(pnt);
		if(it)
		{
			QString szText;
			((KviTreeTaskBarItem *)it)->m_pWindow->getTaskBarTipText(szText);
			m_pToolTip->tip(m_pListView->itemRect(it),szText);
		}
	}
}

bool KviTreeTaskBar::eventFilter(QObject * o,QEvent *e)
{
	if(o == m_pListView->viewport())
	{
		if(e->type() == QEvent::MouseButtonPress)
		{
			QMouseEvent * ev = (QMouseEvent *)e;
			KviTreeTaskBarItem * it = (KviTreeTaskBarItem *)m_pListView->itemAt(ev->pos());
			if(!it)return false;
			KviWindow * wnd = it->kviWindow();
			if(wnd)
			{
				if(ev->button() & Qt::LeftButton)
				{
					if(ev->state() & Qt::ShiftButton)
					{
						wnd->delayedClose();
					} else {
						if((g_pActiveWindow != wnd) || (wnd->isMinimized()))g_pFrame->setActiveWindow(wnd);
						else wnd->minimize();
					}
				} else {
					wnd->contextPopup();
				}
			}
			return true;
		}
	}
	return false;
}


KviTaskBarItem * KviTreeTaskBar::addItem(KviWindow * wnd)
{
	// complex insertion task
	if(wnd->console())
	{
		if(wnd->type() != KVI_WINDOW_TYPE_CONSOLE)
		{
			((KviTreeTaskBarItem *)(wnd->console()->m_pTaskBarItem))->setOpen(true);
			return new KviTreeTaskBarItem(((KviTreeTaskBarItem *)(wnd->console()->m_pTaskBarItem)),wnd);
		}
	}

	// console , or a window that has no irc context
	return new KviTreeTaskBarItem(m_pListView,wnd);
}

bool KviTreeTaskBar::removeItem(KviTaskBarItem * it)
{
	delete (KviTreeTaskBarItem *)it;
	return true;
}

void KviTreeTaskBar::setActiveItem(KviTaskBarItem * it)
{
	if(it)
	{
		KviTreeTaskBarItem * cur = (KviTreeTaskBarItem *)m_pListView->currentItem();
		if(cur && (cur != (KviTreeTaskBarItem *)it))
		{
			cur->setActive(false);
		}
		if(((KviTreeTaskBarItem *)it)->parent())
		{
			if(!((KviTreeTaskBarItem *)it)->parent()->isOpen())((KviTreeTaskBarItem *)it)->parent()->setOpen(true);
		}
		((KviTreeTaskBarItem *)it)->setActive(true);
		m_pListView->setSelected(((KviTreeTaskBarItem *)it),true); // this MUST go after it->setActive()
		if(g_pFrame->dockExtension())g_pFrame->dockExtension()->refresh();
	}
}

void KviTreeTaskBar::updateActivityMeter()
{
	if(KVI_OPTION_BOOL(KviOption_boolUseTaskBarActivityMeter))
	{
		m_pListView->viewport()->update();
	}
}

KviTaskBarItem * KviTreeTaskBar::firstItem()
{
	m_pCurrentItem = (KviTreeTaskBarItem *)m_pListView->firstChild();
	return m_pCurrentItem;
}

KviTaskBarItem * KviTreeTaskBar::nextItem()
{
	if(!m_pCurrentItem)return 0;

	if(m_pCurrentItem->firstChild())
	{
		m_pCurrentItem = (KviTreeTaskBarItem *)m_pCurrentItem->firstChild();
	} else {
		// this item has no children: try the next sibling
		if(m_pCurrentItem->nextSibling())
		{
			m_pCurrentItem = (KviTreeTaskBarItem *)m_pCurrentItem->nextSibling();
		} else {
			if(m_pCurrentItem->parent())
			{
				// child with not siblings : try the sibling of the parent
				m_pCurrentItem = (KviTreeTaskBarItem *)m_pCurrentItem->parent()->nextSibling();
			} else {
				m_pCurrentItem = 0; // toplevel with no siblings
			}
		}
	}
	return m_pCurrentItem;
}

KviTaskBarItem * KviTreeTaskBar::prevItem()
{	
	KviTreeTaskBarItem * it;

	if(!m_pCurrentItem)return 0;

	if(m_pCurrentItem->parent())
	{
		// a child item
		it = (KviTreeTaskBarItem *)m_pCurrentItem->parent()->firstChild();

		while(it)
		{
			if(((KviTreeTaskBarItem *)it->nextSibling()) == m_pCurrentItem)break;
			else it = ((KviTreeTaskBarItem *)(it->nextSibling()));
		}
		if(!it)
		{
			it = (KviTreeTaskBarItem *)m_pCurrentItem->parent();
		}

	} else {
		// a toplevel one
		it = (KviTreeTaskBarItem *)m_pListView->firstChild();
		while(it)
		{
			if(((KviTreeTaskBarItem *)it->nextSibling()) == m_pCurrentItem)break;
			else it = ((KviTreeTaskBarItem *)(it->nextSibling()));
		}
		if(it)
		{
			if(it->firstChild())
			{
				it = ((KviTreeTaskBarItem *)(it->firstChild()));
				while(it->nextSibling())it = ((KviTreeTaskBarItem *)(it->nextSibling()));
			}
		}
	}

	m_pCurrentItem = it;
	return it;
}

KviTaskBarItem * KviTreeTaskBar::lastItem()
{
	// first find last toplevel item
	m_pCurrentItem = (KviTreeTaskBarItem *)m_pListView->firstChild();
	for(;;)
	{
		if(m_pCurrentItem->nextSibling())
		{
			m_pCurrentItem = (KviTreeTaskBarItem *)m_pCurrentItem->nextSibling();
		} else if(m_pCurrentItem->firstChild())
		{
			m_pCurrentItem = (KviTreeTaskBarItem *)m_pCurrentItem->firstChild();
		} else return m_pCurrentItem;
	}
	return 0;
}

bool KviTreeTaskBar::setIterationPointer(KviTaskBarItem * it)
{
	m_pCurrentItem = (KviTreeTaskBarItem *)it;
	if(!it)return true;
	if(((KviTalListView *)m_pListView) == ((KviTreeTaskBarItem *)it)->listView())return true;
	m_pCurrentItem = 0;
	return false;
}

void KviTreeTaskBar::applyOptions()
{
	m_pListView->update();
	if(!KVI_OPTION_BOOL(KviOption_boolShowTreeTaskbarHeader))
	{
		m_pListView->header()->hide();
	} else {
		m_pListView->header()->show();
	}
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "kvi_taskbar.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
