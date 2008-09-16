//===============================================================================================
//
//   File : kvi_windowlist_tree.cpp
//   Last major modification : Sat Sep 13 2008 18:00:00 CEST by Fabio Bas
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_channel.h"
#include "kvi_console.h"
#include "kvi_dynamictooltip.h"
#include "kvi_frame.h"
#include "kvi_locale.h"
#include "kvi_options.h"
#include "kvi_tal_popupmenu.h"
#include "kvi_window.h"
#include "kvi_windowlist_tree.h"

#include <QHeaderView>
#include <QMouseEvent>
#include <QPainter>

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	extern QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif

extern QPixmap * g_pActivityMeterPixmap;

// KviTreeWindowListItem

KviTreeWindowListItem::KviTreeWindowListItem(KviTalTreeWidget * par,KviWindow * wnd)
: KviTalTreeWidgetItem(par) , KviWindowListItem(wnd)
{
	if(KVI_OPTION_BOOL(KviOption_boolUseWindowListIcons))
	{
		m_Layout.addWidget(&m_Icon);
	}

	if(wnd->type()==KVI_WINDOW_TYPE_CONSOLE)
	{
		QFont myfont(font(0));
		myfont.setBold(true);
		setFont(myfont);

		if(KVI_OPTION_BOOL(KviOption_boolUseWindowListIrcContextIndicator))
		{
			QPixmap pix(16,16);
			QPainter p(&pix);
			p.fillRect(0, 0, 16, 16, KVI_OPTION_ICCOLOR(wnd->console()->context()->id() % KVI_NUM_ICCOLOR_OPTIONS));
			setContextIcon(pix);
			m_Layout.addWidget(&m_Context);
		}

	}

	//not for consoles / other windows
	//m_Layout.addWidget(&m_Activity);

	m_Layout.addWidget(&m_Text);
	m_Layout.addStretch();
	m_Layout.setContentsMargins(0,0,0,0);

	m_pBox = new QWidget();
	m_pBox->setLayout(&m_Layout);
	treeWidget()->setItemWidget(this, 0, m_pBox);

	//sort the widget
	treeWidget()->sortItems(0,Qt::AscendingOrder);
}

KviTreeWindowListItem::KviTreeWindowListItem(KviTreeWindowListItem * par,KviWindow * wnd)
: KviTalTreeWidgetItem(par) , KviWindowListItem(wnd)
{
	if(KVI_OPTION_BOOL(KviOption_boolUseWindowListIcons))
	{
		m_Layout.addWidget(&m_Icon);
	}
	//used only in console windows
	// m_Layout.addWidget(&m_Context);

	if(KVI_OPTION_BOOL(KviOption_boolUseWindowListActivityMeter))
	{
		unsigned int uActivityValue;
		unsigned int uActivityTemperature;
		if(wnd->activityMeter(&uActivityValue,&uActivityTemperature))
		{
			QPixmap pix2(5,16);
			QPainter p2(&pix2);
			p2.drawPixmap(0,0,*g_pActivityMeterPixmap,uActivityValue * 5,uActivityTemperature * 16,5,16);
			setActivityIcon(pix2);
		}
		m_Layout.addWidget(&m_Activity);
	}

	m_Layout.addWidget(&m_Text);
	m_Layout.addStretch();
	m_Layout.setContentsMargins(0,0,0,0);

	m_pBox = new QWidget();
	m_pBox->setLayout(&m_Layout);
	treeWidget()->setItemWidget(this, 0, m_pBox);

	//sort the widget
	treeWidget()->sortItems(0,Qt::AscendingOrder);
}

void KviTreeWindowListItem::captionChanged()
{
	QString szText;

	switch(m_pWindow->type())
	{
		case KVI_WINDOW_TYPE_CONSOLE:
		{
			KviWindowListBase::getTextForConsole(szText,(KviConsole *)m_pWindow);
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

	if(m_pWindow->isMinimized())
		szText.prepend('(').append(')');

	setText(szText);

	if(KVI_OPTION_BOOL(KviOption_boolUseWindowListIcons))
	{
		setWindowIcon(*kviWindow()->myIconPtr());
	}

	//sort the widget
	treeWidget()->sortItems(0,Qt::AscendingOrder);
}

void KviTreeWindowListItem::refreshBrush()
{
	if(this == treeWidget()->currentItem())
	{
		setForeground(KVI_OPTION_COLOR(KviOption_colorTreeWindowListActiveForeground));
	} else {
		int iLevel;
		switch(m_iHighlightLevel)
		{
			case 1: iLevel = KviOption_colorTreeWindowListHighlight1Foreground; break;
			case 2: iLevel = KviOption_colorTreeWindowListHighlight2Foreground; break;
			case 3: iLevel = KviOption_colorTreeWindowListHighlight3Foreground; break;
			case 4: iLevel = KviOption_colorTreeWindowListHighlight4Foreground; break;
			case 5: iLevel = KviOption_colorTreeWindowListHighlight5Foreground; break;
			default: iLevel = KviOption_colorTreeWindowListForeground; break;
		}
		setForeground(KVI_OPTION_COLOR(iLevel));
	}
}

void KviTreeWindowListItem::refreshActivityIcon()
{
	if(KVI_OPTION_BOOL(KviOption_boolUseWindowListActivityMeter))
	{
		unsigned int uActivityValue;
		unsigned int uActivityTemperature;
		if(kviWindow()->activityMeter(&uActivityValue,&uActivityTemperature))
		{
			QPixmap pix2(5,16);
			QPainter p2(&pix2);
			p2.drawPixmap(0,0,*g_pActivityMeterPixmap,uActivityValue * 5,uActivityTemperature * 16,5,16);
			setActivityIcon(pix2);
		}
	}
}
void KviTreeWindowListItem::unhighlight()
{
	refreshActivityIcon();
	if(m_iHighlightLevel < 1)return;
	m_iHighlightLevel = 0;
	refreshBrush();
	if(g_pFrame->dockExtension())g_pFrame->dockExtension()->refresh();
}

void KviTreeWindowListItem::highlight(int iLevel)
{
	refreshActivityIcon();
	if(iLevel <= m_iHighlightLevel)return;
	if((this == treeWidget()->currentItem()) && g_pFrame->isActiveWindow())return;
	m_iHighlightLevel = iLevel;
	refreshBrush();
	if(g_pFrame->dockExtension())g_pFrame->dockExtension()->refresh();
}

QString KviTreeWindowListItem::key() const
{
	// This is the sorting function for KviTreeTaskBarItem
	// 1) window type (console, other window..) 2) unique id (to avoid bug #9) 3) windowname (for alphabetical sorting of childs) 3)
	QString ret;
	if(m_pWindow->type()==KVI_WINDOW_TYPE_CONSOLE)
	{
		ret.sprintf("%d%d",m_pWindow->type(),m_pWindow->numericId());
	} else {
		ret.sprintf("%d%s",m_pWindow->type(),m_pWindow->windowName().toLower().toUtf8().data());
	}
	return ret;
}

// KviTreeWindowListTreeWidget

KviTreeWindowListTreeWidget::KviTreeWindowListTreeWidget(QWidget * par)
: KviTalTreeWidget(par)
{
	setObjectName("tree_windowlist");
	setRootIsDecorated(true);
	//Animation creates problem with the background painting on expande/collapse
	//setAnimated(true);
	setAllColumnsShowFocus(true);
	setSelectionMode(QAbstractItemView::SingleSelection);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void KviTreeWindowListTreeWidget::mousePressEvent(QMouseEvent *e)
{

	KviTreeWindowListItem * it = (KviTreeWindowListItem *)itemAt(e->pos());
	if(it)
	{
		KviWindow* wnd = it->kviWindow();
		if(e->button() & Qt::LeftButton)
		{
			if(e->modifiers() & Qt::ShiftModifier)
			{
				wnd->delayedClose();
			} else {
				if((g_pActiveWindow != wnd) || (wnd->isMinimized()))
				{
					g_pFrame->setActiveWindow(wnd);
					KviTalTreeWidget::mousePressEvent(e);
				} else wnd->minimize();
			}

		} else if(e->button() & Qt::RightButton)
		{
			wnd->contextPopup();
		}

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

void KviTreeWindowListTreeWidget::sort()
{
	sortItems(0,Qt::AscendingOrder);
}

void KviTreeWindowListTreeWidget::reverseSort()
{
	sortItems(0,Qt::DescendingOrder);
}

void KviTreeWindowListTreeWidget::paintEvent(QPaintEvent * event)
{
	QPainter *p = new QPainter(viewport());
	QRect rect = event->rect();

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt = viewport()->mapToGlobal(rect.topLeft());
		p->drawTiledPixmap(rect,*g_pShadedChildGlobalDesktopBackground,pnt);
	} else {
#endif
		QPixmap * pix = KVI_OPTION_PIXMAP(KviOption_pixmapTreeWindowListBackground).pixmap();
		if(pix)
		{
			QPoint pnt = rect.topLeft();

			KviPixmapUtils::drawPixmapWithPainter(p,pix,KVI_OPTION_UINT(KviOption_uintTreeWindowListPixmapAlign),rect,viewport()->width(),viewport()->height(),pnt.x(),pnt.y());
		} else {
			p->fillRect(rect,KVI_OPTION_COLOR(KviOption_colorTreeWindowListBackground));
		}
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	}
#endif

	delete p;

	//call paint on all childrens
	KviTalTreeWidget::paintEvent(event);
}

// KviTreeWindowList

KviTreeWindowList::KviTreeWindowList()
: KviWindowListBase()
{
	m_pTreeWidget = new KviTreeWindowListTreeWidget(this);
	m_pTreeWidget->addColumn(__tr2qs("Window List"),135);
	setWidget(m_pTreeWidget);

	//minimun and maximun width of the treeview
	if(KVI_OPTION_UINT(KviOption_uintTreeWindowListMinimumWidth) < 48)
		KVI_OPTION_UINT(KviOption_uintTreeWindowListMinimumWidth) = 48;
	int iMin = m_pTreeWidget->minimumSize().width() + 4;
	if(((unsigned int)iMin) < KVI_OPTION_UINT(KviOption_uintTreeWindowListMinimumWidth))
		iMin = KVI_OPTION_UINT(KviOption_uintTreeWindowListMinimumWidth);
	setMinimumWidth(iMin);

	if(KVI_OPTION_UINT(KviOption_uintTreeWindowListMaximumWidth) < iMin)
		KVI_OPTION_UINT(KviOption_uintTreeWindowListMaximumWidth) = iMin;
	int iMax = 600;
	if(((unsigned int)iMax) > KVI_OPTION_UINT(KviOption_uintTreeWindowListMaximumWidth))
		iMax = KVI_OPTION_UINT(KviOption_uintTreeWindowListMaximumWidth);
	setMaximumWidth(iMax+4);

	//misc settings
	m_pTreeWidget->setFont(KVI_OPTION_FONT(KviOption_fontTreeWindowList));

	if(KVI_OPTION_BOOL(KviOption_boolShowTreeWindowListHeader))
	{
		m_pTreeWidget->header()->show();
	} else {
		m_pTreeWidget->header()->hide();
	}

	m_pTreeWidget->header()->setResizeMode(QHeaderView::Interactive);

	//tooltips
	m_pToolTip = new KviDynamicToolTip(m_pTreeWidget->viewport(),"tree_windowlist_tooltip");
	connect(m_pToolTip,SIGNAL(tipRequest(KviDynamicToolTip *,const QPoint &)),this,SLOT(tipRequest(KviDynamicToolTip *,const QPoint &)));
}

KviTreeWindowList::~KviTreeWindowList()
{
}

void KviTreeWindowList::updatePseudoTransparency()
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	m_pTreeWidget->viewport()->update();
#endif
}

void KviTreeWindowList::moveEvent(QMoveEvent *)
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	updatePseudoTransparency();
#endif
}

void KviTreeWindowList::tipRequest(KviDynamicToolTip *,const QPoint &pnt)
{
	if(KVI_OPTION_BOOL(KviOption_boolShowWindowListToolTips))
	{
		KviTalTreeWidgetItem * it = (KviTalTreeWidgetItem *)m_pTreeWidget->itemAt(pnt);
		if(it)
		{
			QString szText;
			((KviTreeWindowListItem *)it)->m_pWindow->getWindowListTipText(szText);
			m_pToolTip->tip(m_pTreeWidget->visualItemRect(it),szText);
		}
	}
}

KviWindowListItem * KviTreeWindowList::addItem(KviWindow * wnd)
{
	// complex insertion task
	if(wnd->console())
	{
		if(wnd->type() != KVI_WINDOW_TYPE_CONSOLE)
			return new KviTreeWindowListItem(((KviTreeWindowListItem *)(wnd->console()->m_pWindowListItem)),wnd);
	}

	// console , or a window that has no irc context
	return new KviTreeWindowListItem(m_pTreeWidget,wnd);
}

bool KviTreeWindowList::removeItem(KviWindowListItem * it)
{
	delete (KviTreeWindowListItem *)it;
	return true;
}

void KviTreeWindowList::setActiveItem(KviWindowListItem * i)
{
	if(i)
	{
		KviTreeWindowListItem *it = (KviTreeWindowListItem *)i;
		KviTreeWindowListItem *cur = (KviTreeWindowListItem *)m_pTreeWidget->currentItem();
		if(cur != it)
		{
			m_pTreeWidget->setCurrentItem(it);
			m_pTreeWidget->scrollToItem(it);
			it->setSelected(true);
			it->unhighlight();
			it->refreshBrush();
			if(cur)
			{
				cur->setSelected(false);
				cur->unhighlight();
				cur->refreshBrush();
			}
		}
		if(g_pFrame->dockExtension())g_pFrame->dockExtension()->refresh();
	}
}

void KviTreeWindowList::updateActivityMeter()
{
	if(KVI_OPTION_BOOL(KviOption_boolUseWindowListActivityMeter))
	{
		m_pTreeWidget->viewport()->update();
	}
}

KviWindowListItem * KviTreeWindowList::firstItem()
{
	m_pCurrentItem = (KviTreeWindowListItem *)m_pTreeWidget->topLevelItem(0);
	return m_pCurrentItem;
}

KviWindowListItem * KviTreeWindowList::nextItem()
{
	if(!m_pCurrentItem)return 0;
	return m_pCurrentItem = (KviTreeWindowListItem *) m_pTreeWidget->itemBelow(m_pCurrentItem);
}

KviWindowListItem * KviTreeWindowList::prevItem()
{
	if(!m_pCurrentItem)return 0;
	return m_pCurrentItem = (KviTreeWindowListItem *) m_pTreeWidget->itemAbove(m_pCurrentItem);
}

KviWindowListItem * KviTreeWindowList::lastItem()
{
	// first find last toplevel item
	int ctops, tops;

	tops = m_pTreeWidget->topLevelItemCount();
	if(tops > 0)
	{
		ctops = m_pTreeWidget->topLevelItem(tops - 1)->childCount();
		if(ctops) {
			return m_pCurrentItem = (KviTreeWindowListItem *) m_pTreeWidget->topLevelItem(tops - 1)->child(ctops -1);
		} else {
			return m_pCurrentItem = (KviTreeWindowListItem *) m_pTreeWidget->topLevelItem(tops - 1);
		}
	} else {
		return 0;
	}
}

bool KviTreeWindowList::setIterationPointer(KviWindowListItem * it)
{
	m_pCurrentItem = (KviTreeWindowListItem *)it;
	if(!it)return true;
	if(((KviTalTreeWidget *)m_pTreeWidget) == ((KviTreeWindowListItem *)it)->treeWidget())return true;
	m_pCurrentItem = 0;
	return false;
}

void KviTreeWindowList::applyOptions()
{
	if(!KVI_OPTION_BOOL(KviOption_boolShowTreeWindowListHeader))
	{
		m_pTreeWidget->header()->hide();
	} else {
		m_pTreeWidget->header()->show();
	}
	m_pTreeWidget->update();
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "kvi_windowlist_tree.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
