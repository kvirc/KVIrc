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

#include <QHeaderView>

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

void KviTaskBarBase::wheelEvent(QWheelEvent *e)
{
	if ( e->delta() > 0 )
		switchWindow(false, false);
	else
		switchWindow(true, false);
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
: QPushButton(par) , KviTaskBarItem(wnd)
{
	setObjectName(name);
	m_bActive         = false;
	m_pTip            = new KviDynamicToolTip(this);
	connect(m_pTip,SIGNAL(tipRequest(KviDynamicToolTip *,const QPoint &)),this,SLOT(tipRequest(KviDynamicToolTip *,const QPoint &)));
	setCheckable(true); //setToggleButton (true);
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
		if(e->modifiers() & Qt::ShiftModifier)
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
	setChecked(bActive);
	update();
}

void KviTaskBarButton::paintEvent(QPaintEvent * e)
{
	QPainter p(this);
	QStyleOption opt;
	opt.initFrom(this);
	if(isChecked())
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
		QColor base = palette().color(QPalette::Background);
		if(m_pWindow->console())
		{
			QColor cntx = KVI_OPTION_ICCOLOR(m_pWindow->console()->ircContextId() % KVI_NUM_ICCOLOR_OPTIONS);
			base.setRgb((base.red() + cntx.red()) >> 1,(base.green() + cntx.green()) >> 1,
				(base.blue() + cntx.blue()) >> 1);
			pPainter->fillRect(2,iHeight,iWidth - 4,KVI_TASKBARBUTTON_CONTEXTINDICATORHEIGHT - 2,base);
		} else {
			pPainter->fillRect(2,iHeight,iWidth - 4,KVI_TASKBARBUTTON_CONTEXTINDICATORHEIGHT - 2,palette().brush(QPalette::Background));
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
		szText.prepend('(');
		szText.append(')');
	}

	pPainter->setClipRect(cRect);
	pPainter->drawText(cRect,Qt::AlignLeft | Qt::AlignTop,szText,&bRect);

	if(bRect.width() > cRect.width())
	{
		pPainter->setClipRect(cRect.right(),cRect.y(),10,cRect.height());
		QColor base = pPainter->pen().color();
		QColor bg   = palette().color(QPalette::Background);
		base.setRgb((base.red() + bg.red()) / 2,(base.green() + bg.green()) / 2,(base.blue() + bg.blue()) / 2);
		pPainter->setPen(base);
		cRect.setWidth(cRect.width() + 10);
		pPainter->drawText(cRect,Qt::AlignLeft | Qt::AlignTop,szText);
		pPainter->setClipRect(cRect.right(),cRect.y(),5,cRect.height());
		base.setRgb((base.red() + bg.red()) / 2,(base.green() + bg.green()) / 2,(base.blue() + bg.blue()) / 2);
		pPainter->setPen(base);
		cRect.setWidth(cRect.width() + 10);
		pPainter->drawText(cRect,Qt::AlignLeft | Qt::AlignTop,szText);
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
}

void KviTaskBarButton::unhighlight()
{
	if(m_iHighlightLevel < 1)return;
	m_iHighlightLevel = 0;
	if(g_pFrame->dockExtension())g_pFrame->dockExtension()->refresh();
}

void KviTaskBarButton::highlight(int iLevel)
{
	if(iLevel <= m_iHighlightLevel)return;
	if(m_bActive && g_pFrame->isActiveWindow())return;
	m_iHighlightLevel = iLevel;
	if(g_pFrame->dockExtension())g_pFrame->dockExtension()->refresh();
	if(m_bActive)return;
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


// KviTreeTaskBarItem

KviTreeTaskBarItem::KviTreeTaskBarItem(KviTalTreeWidget * par,KviWindow * wnd)
: KviTalTreeWidgetItem(par) , KviTaskBarItem(wnd)
{
	m_iStepNumber=0;
	m_bIncreasing=0;
	m_pInternal=new KviTreeTaskBarItemInternal(this);
	m_pAnimTimer=new QTimer();
	QObject::connect( m_pAnimTimer, SIGNAL(timeout()), m_pInternal, SLOT(timerShot()));
	applyOptions();
	//sort the widget
	//TODO should we implement an operator< to fine tune our sorting?
	treeWidget()->sortItems(0,Qt::AscendingOrder);
}

KviTreeTaskBarItem::KviTreeTaskBarItem(KviTreeTaskBarItem * par,KviWindow * wnd)
: KviTalTreeWidgetItem(par) , KviTaskBarItem(wnd)
{
	m_iStepNumber=0;
	m_bIncreasing=0;
	m_pInternal=new KviTreeTaskBarItemInternal(this);
	m_pAnimTimer=new QTimer();
	QObject::connect( m_pAnimTimer, SIGNAL(timeout()), m_pInternal, SLOT(timerShot()));
	applyOptions();
	//sort the widget
	treeWidget()->sortItems(0,Qt::AscendingOrder);
}

KviTreeTaskBarItem::~KviTreeTaskBarItem()
{
	KviTalTreeWidget* pView=(KviTalTreeWidget *)treeWidget();
	if(pView)
		if(((KviTreeTaskBarTreeWidget*)(pView))->m_pPrevItem==this) ((KviTreeTaskBarTreeWidget*)(treeWidget()))->m_pPrevItem=0;
	delete m_pAnimTimer;
	delete m_pInternal;
}

void KviTreeTaskBarItem::applyOptions()
{
	setData(0, KVI_TTBID_REDDIFF, (KVI_OPTION_COLOR(KviOption_colorTreeTaskBarActiveBackground).red()-KVI_OPTION_COLOR(KviOption_colorTreeTaskBarBackground).red())/KVI_NUM_STEPS);
	setData(0, KVI_TTBID_GREENDIFF, (KVI_OPTION_COLOR(KviOption_colorTreeTaskBarActiveBackground).green()-KVI_OPTION_COLOR(KviOption_colorTreeTaskBarBackground).green())/KVI_NUM_STEPS);
	setData(0, KVI_TTBID_BLUEDIFF, (KVI_OPTION_COLOR(KviOption_colorTreeTaskBarActiveBackground).blue()-KVI_OPTION_COLOR(KviOption_colorTreeTaskBarBackground).blue())/KVI_NUM_STEPS);
}

QString KviTreeTaskBarItem::currentCaption() const
{
	QString szText;

	switch(m_pWindow->type())
	{
		case KVI_WINDOW_TYPE_CONSOLE:
		{
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

	return szText;
}

void KviTreeTaskBarItem::captionChanged()
{
	QString szText=currentCaption();
	if(m_pWindow->isMinimized())
		szText.prepend('(').append(')');

	//sort the widget
	treeWidget()->sortItems(0,Qt::AscendingOrder);

	setData(0, Qt::DisplayRole, szText);
}

void KviTreeTaskBarItem::unhighlight()
{
	if(m_iHighlightLevel < 1)return;
	m_iHighlightLevel = 0;
	setData(0, KVI_TTBID_HIGHLIGHT, m_iHighlightLevel);
	
	if(g_pFrame->dockExtension())g_pFrame->dockExtension()->refresh();
}

void KviTreeTaskBarItem::highlight(int iLevel)
{
	if(iLevel <= m_iHighlightLevel)return;
	if(isSelected() && g_pFrame->isActiveWindow())return;
	m_iHighlightLevel = iLevel;
	setData(0, KVI_TTBID_HIGHLIGHT, m_iHighlightLevel);

	if(g_pFrame->dockExtension())g_pFrame->dockExtension()->refresh();
	if(isSelected())return;
}

void KviTreeTaskBarItem::setProgress(int progress)
{
	if(progress == m_iProgress)return;
	m_iProgress = progress;
	setData(0, KVI_TTBID_PROGRESS, m_iProgress);
}

void KviTreeTaskBarItem::setActive(bool bActive)
{
	if(bActive)
	{
		m_iHighlightLevel = 0;
	} else {
		if(isSelected())
			m_iHighlightLevel = 0;
	}

	setData(0, KVI_TTBID_HIGHLIGHT, m_iHighlightLevel);
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
		if(m_pAnimTimer->isActive()) m_pAnimTimer->stop();
		m_iStepNumber=KVI_NUM_STEPS; //make shure, that we cannot get out of range
	} else if((m_iStepNumber<=0) && !m_bIncreasing) {
		if(m_pAnimTimer->isActive()) m_pAnimTimer->stop();
		m_iStepNumber=0; //make shure, that we cannot get out of range
	}
	setData(0, KVI_TTBID_STEPNUM, m_iStepNumber);
	treeWidget()->viewport()->update();
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

// KviTreeTaskBarTreeWidget

KviTreeTaskBarTreeWidget::KviTreeTaskBarTreeWidget(QWidget * par)
: KviTalTreeWidget(par)
{
	header()->setSortIndicatorShown(false);
	setFrameShape(NoFrame);
	viewport()->setMouseTracking(TRUE);
	m_pPrevItem=0;
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setSelectionBehavior(QAbstractItemView::SelectItems);
	setSelectionMode(QAbstractItemView::SingleSelection);
}

KviTreeTaskBarTreeWidget::~KviTreeTaskBarTreeWidget()
{
}

void KviTreeTaskBarTreeWidget::mouseMoveEvent ( QMouseEvent * e )
{
	if(!e) return;
	KviTreeTaskBarItem* pCur=(KviTreeTaskBarItem*)(itemAt(e->pos()));
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

void KviTreeTaskBarTreeWidget::leaveEvent(QEvent *)
{
	if(m_pPrevItem) m_pPrevItem->mouseLeave();
	m_pPrevItem=0;
	setCursor(Qt::ArrowCursor);
}

void KviTreeTaskBarTreeWidget::mousePressEvent(QMouseEvent *e)
{
	KviTalTreeWidgetItem * it = (KviTalTreeWidgetItem *)itemAt(e->pos());
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

void KviTreeTaskBarTreeWidget::mouseDoubleClickEvent(QMouseEvent * e)
{
	KviTalTreeWidgetItem * it = (KviTalTreeWidgetItem *)itemAt(e->pos());
	if(it)
	{
		if(e->button() & Qt::LeftButton)
		{
			it->setExpanded(!(it->isExpanded()));
		}
	}
}

void KviTreeTaskBarTreeWidget::sort()
{
	sortItems(0,Qt::AscendingOrder);
}

void KviTreeTaskBarTreeWidget::reverseSort()
{
	sortItems(0,Qt::DescendingOrder);
}

void KviTreeTaskBarTreeWidget::resizeEvent(QResizeEvent *e)
{
	KviTalTreeWidget::resizeEvent(e);
	setColumnWidth(0,viewport()->width());
	resize(viewport()->width(),viewport()->height());
}

void KviTreeTaskBarTreeWidget::paintEvent(QPaintEvent * event)
{
	QPainter *p = new QPainter(viewport());
	QStyleOptionViewItem option = viewOptions();
	QRect rect = event->rect();

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt = viewport()->mapToGlobal(rect.topLeft());
		p->drawTiledPixmap(rect,*g_pShadedChildGlobalDesktopBackground,pnt);
	} else {
#endif
		p->fillRect(rect,KVI_OPTION_COLOR(KviOption_colorTreeTaskBarBackground));
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	}
#endif
	
	QPixmap * pix = KVI_OPTION_PIXMAP(KviOption_pixmapTreeTaskBarBackground).pixmap();
	if(pix)
	{
		QPoint pnt = rect.topLeft();

		KviPixmapUtils::drawPixmapWithPainter(p,pix,KVI_OPTION_UINT(KviOption_uintTreeTaskBarPixmapAlign),rect,viewport()->width(),viewport()->height(),pnt.x(),pnt.y());
	}
	delete p;

	//call paint on all childrens
	KviTalTreeWidget::paintEvent(event);
}

// KviTreeTaskBar

KviTreeTaskBar::KviTreeTaskBar()
: KviTaskBarBase()
{
	m_pTreeWidget = new KviTreeTaskBarTreeWidget(this);
	m_pTreeWidget->addColumn(__tr2qs("Window List"),135);
	setWidget(m_pTreeWidget);

	//ad-hoc itemdelegate for this view
	m_pItemDelegate = new KviTreeTaskBarItemDelegate(m_pTreeWidget);
	m_pTreeWidget->setItemDelegate(m_pItemDelegate);

	//minimun and maximun width of the treeview
	if(KVI_OPTION_UINT(KviOption_uintTreeTaskBarMinimumWidth) < 48)
		KVI_OPTION_UINT(KviOption_uintTreeTaskBarMinimumWidth) = 48;
	int iMin = m_pTreeWidget->minimumSize().width() + 4;
	if(((unsigned int)iMin) < KVI_OPTION_UINT(KviOption_uintTreeTaskBarMinimumWidth))
		iMin = KVI_OPTION_UINT(KviOption_uintTreeTaskBarMinimumWidth);
	setMinimumWidth(iMin);

	if(KVI_OPTION_UINT(KviOption_uintTreeTaskBarMaximumWidth) < iMin)
		KVI_OPTION_UINT(KviOption_uintTreeTaskBarMaximumWidth) = iMin;
	int iMax = 600;
	if(((unsigned int)iMax) > KVI_OPTION_UINT(KviOption_uintTreeTaskBarMaximumWidth))
		iMax = KVI_OPTION_UINT(KviOption_uintTreeTaskBarMaximumWidth);
	setMaximumWidth(iMax+4);

	//misc settings
	m_pTreeWidget->setFocusPolicy(Qt::NoFocus);
	m_pTreeWidget->viewport()->setAutoFillBackground(false);

	if(KVI_OPTION_BOOL(KviOption_boolShowTreeTaskbarHeader))
	{
		m_pTreeWidget->header()->show();
	} else {
		m_pTreeWidget->header()->hide();
	} 
	
	m_pTreeWidget->header()->setResizeMode(QHeaderView::Interactive);
	m_pTreeWidget->viewport()->installEventFilter(this);

	//tooltips
	m_pToolTip = new KviDynamicToolTip(m_pTreeWidget->viewport(),"tree_taskbar_tooltip");
	connect(m_pToolTip,SIGNAL(tipRequest(KviDynamicToolTip *,const QPoint &)),this,SLOT(tipRequest(KviDynamicToolTip *,const QPoint &)));
}

KviTreeTaskBar::~KviTreeTaskBar()
{
}

void KviTreeTaskBar::updatePseudoTransparency()
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	m_pTreeWidget->viewport()->update();
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
		KviTalTreeWidgetItem * it = (KviTalTreeWidgetItem *)m_pTreeWidget->itemAt(pnt);
		if(it)
		{
			QString szText;
			((KviTreeTaskBarItem *)it)->m_pWindow->getTaskBarTipText(szText);
			m_pToolTip->tip(m_pTreeWidget->visualItemRect(it),szText);
		}
	}
}

bool KviTreeTaskBar::eventFilter(QObject * o,QEvent *e)
{
	if(o == m_pTreeWidget->viewport())
	{
		if(e->type() == QEvent::MouseButtonPress)
		{
			QMouseEvent * ev = (QMouseEvent *)e;
			KviTreeTaskBarItem * it = (KviTreeTaskBarItem *)m_pTreeWidget->itemAt(ev->pos());
			if(!it)return false;
			KviWindow * wnd = it->kviWindow();
			if(wnd)
			{
				if(ev->button() & Qt::LeftButton)
				{
					if(ev->modifiers() & Qt::ShiftModifier)
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
			((KviTreeTaskBarItem *)(wnd->console()->m_pTaskBarItem))->setExpanded(true);
			return new KviTreeTaskBarItem(((KviTreeTaskBarItem *)(wnd->console()->m_pTaskBarItem)),wnd);
		}
	}

	// console , or a window that has no irc context
	return new KviTreeTaskBarItem(m_pTreeWidget,wnd);
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
		KviTreeTaskBarItem * cur = (KviTreeTaskBarItem *)m_pTreeWidget->currentItem();
		if(cur && (cur != (KviTreeTaskBarItem *)it))
		{
			cur->setActive(false);
		}
		if(((KviTreeTaskBarItem *)it)->parent())
		{
			if(!((KviTreeTaskBarItem *)it)->parent()->isExpanded())((KviTreeTaskBarItem *)it)->parent()->setExpanded(true);
		}

		//default selectionModel doesn't enforce singleselection if done from the code
		m_pTreeWidget->selectionModel()->clearSelection();
		((KviTreeTaskBarItem *)it)->setActive(true);
		((KviTreeTaskBarItem *)it)->setSelected(true); // this MUST go after it->setActive()
		if(g_pFrame->dockExtension())g_pFrame->dockExtension()->refresh();
	}
}

void KviTreeTaskBar::updateActivityMeter()
{
	if(KVI_OPTION_BOOL(KviOption_boolUseTaskBarActivityMeter))
	{
		m_pTreeWidget->viewport()->update();
	}
}

KviTaskBarItem * KviTreeTaskBar::firstItem()
{
	m_pCurrentItem = (KviTreeTaskBarItem *)m_pTreeWidget->topLevelItem(0);
	return m_pCurrentItem;
}

KviTaskBarItem * KviTreeTaskBar::nextItem()
{
	if(!m_pCurrentItem)return 0;
	return m_pCurrentItem = (KviTreeTaskBarItem *) m_pTreeWidget->itemBelow(m_pCurrentItem);
}

KviTaskBarItem * KviTreeTaskBar::prevItem()
{	
	if(!m_pCurrentItem)return 0;
	return m_pCurrentItem = (KviTreeTaskBarItem *) m_pTreeWidget->itemAbove(m_pCurrentItem);
}

KviTaskBarItem * KviTreeTaskBar::lastItem()
{
	// first find last toplevel item
	int ctops, tops;

	tops = m_pTreeWidget->topLevelItemCount();
	if(tops > 0)
	{
		ctops = m_pTreeWidget->topLevelItem(tops - 1)->childCount();
		if(ctops) {
			return m_pCurrentItem = (KviTreeTaskBarItem *) m_pTreeWidget->topLevelItem(tops - 1)->child(ctops -1);
		} else {
			return m_pCurrentItem = (KviTreeTaskBarItem *) m_pTreeWidget->topLevelItem(tops - 1);
		}
	} else {
		return 0;
	}
}

bool KviTreeTaskBar::setIterationPointer(KviTaskBarItem * it)
{
	m_pCurrentItem = (KviTreeTaskBarItem *)it;
	if(!it)return true;
	if(((KviTalTreeWidget *)m_pTreeWidget) == ((KviTreeTaskBarItem *)it)->treeWidget())return true;
	m_pCurrentItem = 0;
	return false;
}

void KviTreeTaskBar::applyOptions()
{
	if(!KVI_OPTION_BOOL(KviOption_boolShowTreeTaskbarHeader))
	{
		m_pTreeWidget->header()->hide();
	} else {
		m_pTreeWidget->header()->show();
	}
	m_pTreeWidget->update();
}

// KviTreeTaskBarItemDelegate

int KviTreeTaskBarItemDelegate::calculateColor(int col1,int col2, int iStepNumber) const
{
	int result=col1+(col2-col1)/KVI_NUM_STEPS*iStepNumber;
	return result<255 ? result :255;
}

void KviTreeTaskBarItemDelegate::paint(QPainter * p, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	QRect rect=option.rect;
	QRect branchRect = QRect(0, rect.top(), rect.left(), rect.height());
	QRect fullRect = QRect(0, rect.top(), rect.left()+rect.width(), rect.height());

	QString szText=index.data(Qt::DisplayRole).toString();
	int iStepNumber=index.data(KVI_TTBID_STEPNUM).toInt();
	int iRedDiff=index.data(KVI_TTBID_REDDIFF).toInt();
	int iGreenDiff=index.data(KVI_TTBID_GREENDIFF).toInt();
	int iBlueDiff=index.data(KVI_TTBID_BLUEDIFF).toInt();
	int iHighlightLevel=index.data(KVI_TTBID_HIGHLIGHT).toInt();
	int iProgress=index.data(KVI_TTBID_PROGRESS).toInt();

	//FIXME not exactly model/view coding style.. but we need to access data on the associated window
	KviTreeTaskBarTreeWidget* treeWidget = (KviTreeTaskBarTreeWidget*)parent();
	KviTreeTaskBarItem* item = (KviTreeTaskBarItem*) treeWidget->itemFromIndex(index);
	KviWindow* pWindow = item->kviWindow();

	//paint cell background	
	if (option.state & QStyle::State_Selected)
	{
		//selection colored background
		p->fillRect(fullRect,KVI_OPTION_COLOR(KviOption_colorTreeTaskBarActiveBackground));
	} else {
		if(!iStepNumber)
		{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
			//pseudo transparent background
			if(g_pShadedChildGlobalDesktopBackground)
			{
				QPoint pnt = treeWidget->viewport()->mapToGlobal(fullRect.topLeft());
				p->drawTiledPixmap(fullRect,*g_pShadedChildGlobalDesktopBackground,pnt);
			} else {
#endif
				p->fillRect(rect,KVI_OPTION_COLOR(KviOption_colorTreeTaskBarBackground));
#ifdef COMPILE_PSEUDO_TRANSPARENCY
			}
#endif
			//background image (if defined)
			QPixmap * pix = KVI_OPTION_PIXMAP(KviOption_pixmapTreeTaskBarBackground).pixmap();
			if(pix)
			{
				KviPixmapUtils::drawPixmapWithPainter(p,pix,KVI_OPTION_UINT(KviOption_uintTreeTaskBarPixmapAlign),fullRect,fullRect.width(),fullRect.height());
			}
		} else {
			// paint mouse over effect
			p->fillRect(fullRect,
				QColor(KVI_OPTION_COLOR(KviOption_colorTreeTaskBarBackground).red()+iRedDiff*iStepNumber,
					KVI_OPTION_COLOR(KviOption_colorTreeTaskBarBackground).green()+iGreenDiff*iStepNumber,
					KVI_OPTION_COLOR(KviOption_colorTreeTaskBarBackground).blue()+iBlueDiff*iStepNumber
					)
				);
		}
	}

	//tree branches
	treeWidget->drawBranches(p, branchRect, index);

	//draw window icon, irc context indicator (a colored square), set font properties for text
	int im = rect.left();
	int yPixmap = (rect.top() + rect.height() - 16);

	QRect cRect(im + 3,rect.top(),rect.width(),rect.height());

	switch(pWindow->type())
	{
		case KVI_WINDOW_TYPE_CONSOLE:
		{
			if(KVI_OPTION_BOOL(KviOption_boolUseTaskBarIrcContextIndicator))
			{
				QColor base = option.palette.window().color();
				QColor cntx = KVI_OPTION_ICCOLOR(pWindow->console()->ircContextId() % KVI_NUM_ICCOLOR_OPTIONS);
				base.setRgb((base.red() + cntx.red()) >> 1,(base.green() + cntx.green()) >> 1,
					(base.blue() + cntx.blue()) >> 1);
				p->fillRect(im + 2,yPixmap,14,15,base);
				if(KVI_OPTION_BOOL(KviOption_boolUseTaskBarIcons))
				{
					p->drawPixmap(im + 20,yPixmap,*(pWindow->myIconPtr()));
					cRect.setLeft(cRect.left() + 37);
				} else {
					cRect.setLeft(cRect.left() + 20);
				}
			} else {
				if(KVI_OPTION_BOOL(KviOption_boolUseTaskBarIcons))
				{
					p->drawPixmap(im,yPixmap,*(pWindow->myIconPtr()));
					cRect.setLeft(cRect.left() + 17);
				}
			}
			//console window: bold font
			QFont f = p->font();
			f.setBold(true);
			p->setFont(f);
		}
		break;
		case KVI_WINDOW_TYPE_CHANNEL:
		case KVI_WINDOW_TYPE_DEADCHANNEL:
		case KVI_WINDOW_TYPE_QUERY:
		case KVI_WINDOW_TYPE_DEADQUERY:
		default:
			if(KVI_OPTION_BOOL(KviOption_boolUseTaskBarIcons))
			{
				p->drawPixmap(im,yPixmap,*(pWindow->myIconPtr()));
				cRect.setLeft(cRect.left() + 17);
			}
			//channels, other windows: normal font
			QFont f = p->font();
			f.setBold(false);
			p->setFont(f);
		break;
	}

	// paint the channel activity meter
	if(KVI_OPTION_BOOL(KviOption_boolUseTaskBarActivityMeter))
	{
		unsigned int uActivityValue;
		unsigned int uActivityTemperature;
		if(pWindow->activityMeter(&uActivityValue,&uActivityTemperature))
		{
			p->drawPixmap(cRect.left(),yPixmap,*g_pActivityMeterPixmap,uActivityValue * 5,uActivityTemperature * 16,5,16);
			cRect.setLeft(cRect.left() + 7);
		}
	}

	// paint the progress bar
	if(iProgress >= 0)
	{
		int wdth = (iProgress * cRect.width()) / 100;
		p->fillRect(cRect.x(),cRect.y(),wdth,cRect.height(),KVI_OPTION_COLOR(KviOption_colorTreeTaskBarProgress));
	}

	//choose window name font color (highlighting)
	if (option.state & QStyle::State_Selected)
	{
		p->setPen(KVI_OPTION_COLOR(KviOption_colorTreeTaskBarActiveForeground));
	} else {
		int iLevel;
		switch(iHighlightLevel)
		{
			case 0: iLevel = KviOption_colorTreeTaskBarForeground; break;
			case 1: iLevel = KviOption_colorTreeTaskBarHighlight1Foreground; break;
			case 2: iLevel = KviOption_colorTreeTaskBarHighlight2Foreground; break;
			case 3: iLevel = KviOption_colorTreeTaskBarHighlight3Foreground; break;
			case 4: iLevel = KviOption_colorTreeTaskBarHighlight4Foreground; break;
			default: iLevel = KviOption_colorTreeTaskBarHighlight5Foreground; break;
		}
		p->setPen(
			QColor(
				calculateColor(KVI_OPTION_COLOR(iLevel).red(),KVI_OPTION_COLOR(KviOption_colorTreeTaskBarActiveForeground).red(),iStepNumber),
				calculateColor(KVI_OPTION_COLOR(iLevel).green(),KVI_OPTION_COLOR(KviOption_colorTreeTaskBarActiveForeground).green(),iStepNumber),
				calculateColor(KVI_OPTION_COLOR(iLevel).blue(),KVI_OPTION_COLOR(KviOption_colorTreeTaskBarActiveForeground).blue(),iStepNumber)
				)
			);
	}

	//draw window name
	p->drawText(cRect,Qt::AlignLeft | Qt::AlignVCenter,szText);
}

QSize KviTreeTaskBarItemDelegate::sizeHint( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	QString text=index.data(Qt::DisplayRole).toString();
	// FIXME fixed width: 20; compute from font size
	return QSize(((KviTreeTaskBarTreeWidget*)parent())->viewport()->size().width(), 20);
}
#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "kvi_taskbar.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
