//=============================================================================
//
//   File : KviTreeWindowList.cpp
//   Creation date : Thu Jan 7 1999 03:56:50 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2008 Fabio Bas (ctrlaltca at gmail dot com)
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

#include "KviChannelWindow.h"
#include "KviConsoleWindow.h"
#include "KviDynamicToolTip.h"
#include "KviMainWindow.h"
#include "KviLocale.h"
#include "KviOptions.h"
#include "KviWindow.h"
#include "KviTreeWindowList.h"
#include "KviPixmapUtils.h"
#include "KviTrayIcon.h"

#include <QHeaderView>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPainter>
#include <QTimer>
#include <QScrollBar>
#include <QStyleOptionViewItem>
#include <QStyle>
#include <QMenu>

#ifdef COMPILE_PSEUDO_TRANSPARENCY
extern QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif

extern QPixmap * g_pActivityMeterPixmap;

// KviTreeWindowListItem

KviTreeWindowListItem::KviTreeWindowListItem(QTreeWidget * par, KviWindow * wnd)
    : QTreeWidgetItem(par), KviWindowListItem(wnd)
{
	applyOptions();
	captionChanged();
}

KviTreeWindowListItem::KviTreeWindowListItem(KviTreeWindowListItem * par, KviWindow * wnd)
    : QTreeWidgetItem(par), KviWindowListItem(wnd)
{
	applyOptions();
	captionChanged();
}

KviTreeWindowListItem::~KviTreeWindowListItem()
    = default;

void KviTreeWindowListItem::applyOptions()
{
	//sort the widget [FIXME: expensive?]
	treeWidget()->sortItems(0, ((KviTreeWindowListTreeWidget *)treeWidget())->isReverseSort() ? Qt::DescendingOrder : Qt::AscendingOrder);
}

void KviTreeWindowListItem::captionChanged()
{
	QString szText;

	switch(m_pWindow->type())
	{
		case KviWindow::Console:
		{
			KviWindowListBase::getTextForConsole(szText, (KviConsoleWindow *)m_pWindow);
		}
		break;
		case KviWindow::Channel:
		case KviWindow::DeadChannel:
			if(KVI_OPTION_BOOL(KviOption_boolShowUserFlagForChannelsInWindowList))
				szText = ((KviChannelWindow *)m_pWindow)->nameWithUserFlag();
			else
				szText = ((KviChannelWindow *)m_pWindow)->target();
			break;
		case KviWindow::Query:
		case KviWindow::DeadQuery:
			szText = m_pWindow->windowName();
			break;
		default:
			szText = m_pWindow->plainTextCaption();
			break;
	}

	if(m_pWindow->isMinimized())
		szText.prepend('(').append(')');

	//sort the widget
	treeWidget()->sortItems(0, ((KviTreeWindowListTreeWidget *)treeWidget())->isReverseSort() ? Qt::DescendingOrder : Qt::AscendingOrder);

	setData(0, Qt::DisplayRole, szText);
}

void KviTreeWindowListItem::unhighlight()
{
	if(m_iHighlightLevel < 1)
		return;
	m_iHighlightLevel = 0;
	setData(0, KVI_TTBID_HIGHLIGHT, m_iHighlightLevel);

	if(g_pMainWindow->trayIcon())
		g_pMainWindow->trayIcon()->refresh();
}

void KviTreeWindowListItem::highlight(int iLevel)
{
	if(iLevel <= m_iHighlightLevel)
		return;
	if(treeWidget()->currentItem() == this && g_pMainWindow->isActiveWindow())
		return;
	m_iHighlightLevel = iLevel;
	setData(0, KVI_TTBID_HIGHLIGHT, m_iHighlightLevel);

	if(g_pMainWindow->trayIcon())
		g_pMainWindow->trayIcon()->refresh();
}

void KviTreeWindowListItem::setProgress(int progress)
{
	if(progress == m_iProgress)
		return;
	m_iProgress = progress;
	setData(0, KVI_TTBID_PROGRESS, m_iProgress);
}

void KviTreeWindowListItem::setActive(bool bActive)
{
	if(bActive)
	{
		unhighlight();
		treeWidget()->setCurrentItem(this);
		// 		treeWidget()->scrollToItem(this);
	}
}

QString KviTreeWindowListItem::key() const
{
	// This is the sorting function for KviTreeTaskBarItem
	// 1) window type (console, other window..) 2) unique id [to avoid bug #9] 3) windowname (for alphabetical sorting of children)
	QString ret;
	int iType = m_pWindow->type();

	// ensure dead/alive channels and queries stay in the same position
	if(iType == KviWindow::DeadChannel)
		iType = KviWindow::Channel;
	if(iType == KviWindow::DeadQuery)
		iType = KviWindow::Query;

	if(KVI_OPTION_BOOL(KviOption_boolSortWindowListItemsByName))
	{
		if(iType == KviWindow::Console)
		{
			QString szText;
			KviWindowListBase::getTextForConsole(szText, (KviConsoleWindow *)m_pWindow);

			ret.sprintf("%2d%s", iType, szText.toLower().toUtf8().data());
		}
		else
		{
			ret.sprintf("%2d%s", iType, m_pWindow->windowName().toLower().toUtf8().data());
		}
	}
	else
	{
		if(iType == KviWindow::Console)
		{
			ret.sprintf("%2d%4u", iType, ((KviConsoleWindow *)m_pWindow)->context() ? ((KviConsoleWindow *)m_pWindow)->context()->id() : 9999);
		}
		else
		{
			ret.sprintf("%2d%4d", iType, parent() ? parent()->indexOfChild((QTreeWidgetItem *)this) : 9999);
		}
	}
	return ret;
}

// KviTreeWindowListTreeWidget

KviTreeWindowListTreeWidget::KviTreeWindowListTreeWidget(QWidget * par)
    : QTreeWidget(par)
{
	setObjectName("tree_windowlist");
	bReverseSort = false;
	setRootIsDecorated(true);
	setSelectionMode(QAbstractItemView::NoSelection);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setFont(KVI_OPTION_FONT(KviOption_fontTreeWindowList));
	setFocusPolicy(Qt::NoFocus);
	setAutoFillBackground(false);
	viewport()->setAutoFillBackground(false);
	setFrameShape(QFrame::Box);
	setFrameStyle(QFrame::Panel | QFrame::Sunken);
}

KviTreeWindowListTreeWidget::~KviTreeWindowListTreeWidget()
    = default;

void KviTreeWindowListTreeWidget::mouseMoveEvent(QMouseEvent *)
{
	// dummy just to avoid bug #581:
	//   Clicking on item A and moving the mouse to item B selects item A first and then item B
	//   Hovewer item A remains current
}

void KviTreeWindowListTreeWidget::wheelEvent(QWheelEvent * e)
{
	// Mitigate bug #488:
	//   When there is a scroll bar the wheel scrolls up and down
	//   When there is no scroll bar the wheel changes selection in the tree

	if(KVI_OPTION_BOOL(KviOption_boolWheelScrollsWindowsList))
	{
		if(e->delta() < 0)
			((KviTreeWindowList *)parent())->switchWindow(true, false);
		else if(e->delta() > 0)
			((KviTreeWindowList *)parent())->switchWindow(false, false);
	}
	else
	{
		QScrollBar * pBar = verticalScrollBar();

		if(!pBar)
			return;
		if(!pBar->isVisible())
			return;

		if(
		    ((e->delta() < 0) && (pBar->value() < pBar->maximum())) || ((e->delta() > 0) && (pBar->value() > pBar->minimum())))
			QApplication::sendEvent(pBar, e);
	}
}

void KviTreeWindowListTreeWidget::mousePressEvent(QMouseEvent * e)
{
	KviTreeWindowListItem * it = (KviTreeWindowListItem *)itemAt(e->pos());
	if(it)
	{
		//clicked over an item
		KviWindow * wnd = it->kviWindow();
		if(e->button() & Qt::LeftButton)
		{
			if(e->modifiers() & Qt::ShiftModifier)
			{
				//shitf+left click: close window
				wnd->delayedClose();
			}
			else
			{
				//left click activate/deactivate window
				if(g_pActiveWindow != wnd)
					g_pMainWindow->setActiveWindow(wnd);

				// let our parent handle clicks on branches, etc
				QStyleOption option;
				option.initFrom(this);
				option.rect = visualItemRect(it);
				if(!style()->subElementRect(QStyle::SE_TreeViewDisclosureItem, &option, this).contains(e->pos()))
					QTreeWidget::mousePressEvent(e);
			}
		}
		else if(e->button() & Qt::RightButton)
		{
			//right click: open popup
			wnd->contextPopup();
		}
		else if(e->button() & Qt::MidButton)
		{
			//mid click: close window
			wnd->delayedClose();
		}
	}
	else
	{
		//clicked on empty space
		if(e->button() & Qt::RightButton)
		{
			QMenu * pPopup = new QMenu();
			pPopup->addAction(__tr2qs("Sort"), this, SLOT(sort()));
			pPopup->addAction(__tr2qs("Reverse Sort"), this, SLOT(reverseSort()));
			pPopup->popup(QCursor::pos());
		}
	}
}

void KviTreeWindowListTreeWidget::mouseDoubleClickEvent(QMouseEvent * e)
{
	QTreeWidgetItem * it = itemAt(e->pos());
	if(it)
	{
		if(e->button() & Qt::LeftButton)
		{
			it->setExpanded(!(it->isExpanded()));
		}
	}
}

void KviTreeWindowListTreeWidget::sort()
{
	bReverseSort = false;
	sortItems(0, Qt::AscendingOrder);
}

void KviTreeWindowListTreeWidget::reverseSort()
{
	bReverseSort = true;
	sortItems(0, Qt::DescendingOrder);
}

void KviTreeWindowListTreeWidget::paintEvent(QPaintEvent * event)
{
	QPainter * p = new QPainter(viewport());
	QStyleOptionViewItem option = viewOptions();
	QRect rect = event->rect();

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency) && g_pApp->supportsCompositing())
	{
		p->save();
		p->setCompositionMode(QPainter::CompositionMode_Source);
		QColor col = KVI_OPTION_COLOR(KviOption_colorGlobalTransparencyFade);
		col.setAlphaF((float)((float)KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) / (float)100));
		p->fillRect(rect, col);
		p->restore();
	}
	else if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt = mapTo(g_pMainWindow, rect.topLeft());
		p->drawTiledPixmap(rect, *(g_pShadedChildGlobalDesktopBackground), pnt);
	}
	else
	{
#endif
		QPixmap * pix = KVI_OPTION_PIXMAP(KviOption_pixmapTreeWindowListBackground).pixmap();
		if(pix)
		{
			KviPixmapUtils::drawPixmapWithPainter(p, pix, KVI_OPTION_UINT(KviOption_uintTreeWindowListPixmapAlign), rect, viewport()->width(), viewport()->height());
		}
		else
		{
			p->fillRect(rect, KVI_OPTION_COLOR(KviOption_colorTreeWindowListBackground));
		}
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	}
#endif

	delete p;

	//call paint on all childrens
	QTreeWidget::paintEvent(event);
}

// KviTreeWindowList

KviTreeWindowList::KviTreeWindowList()
    : KviWindowListBase()
{
	m_pTreeWidget = new KviTreeWindowListTreeWidget(this);
	m_pTreeWidget->setColumnWidth(0, 135);
	m_pTreeWidget->header()->hide();
	setWidget(m_pTreeWidget);

	//ad-hoc itemdelegate for this view
	m_pItemDelegate = new KviTreeWindowListItemDelegate(m_pTreeWidget);
	m_pTreeWidget->setItemDelegate(m_pItemDelegate);

	//minimun and maximun width of the treeview
	if(KVI_OPTION_UINT(KviOption_uintTreeWindowListMinimumWidth) < 48)
		KVI_OPTION_UINT(KviOption_uintTreeWindowListMinimumWidth) = 48;
	int iMin = m_pTreeWidget->minimumSize().width() + 4;
	if(((unsigned int)iMin) < KVI_OPTION_UINT(KviOption_uintTreeWindowListMinimumWidth))
		iMin = KVI_OPTION_UINT(KviOption_uintTreeWindowListMinimumWidth);
	setMinimumWidth(iMin);

	//tooltips
	m_pToolTip = new KviDynamicToolTip(m_pTreeWidget->viewport(), "tree_windowlist_tooltip");
	connect(m_pToolTip, SIGNAL(tipRequest(KviDynamicToolTip *, const QPoint &)), this, SLOT(tipRequest(KviDynamicToolTip *, const QPoint &)));

	applyOptions();
}

KviTreeWindowList::~KviTreeWindowList()
    = default;

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

void KviTreeWindowList::tipRequest(KviDynamicToolTip *, const QPoint & pnt)
{
	if(KVI_OPTION_BOOL(KviOption_boolShowWindowListToolTips))
	{
		QTreeWidgetItem * it = m_pTreeWidget->itemAt(pnt);
		if(it)
		{
			QString szText;
			((KviTreeWindowListItem *)it)->m_pWindow->getWindowListTipText(szText);
			m_pToolTip->tip(QRect(pnt, QSize(16, 16)), szText);
		}
	}
}

KviWindowListItem * KviTreeWindowList::addItem(KviWindow * wnd)
{
	// complex insertion task
	if(wnd->console())
	{
		if(wnd->type() != KviWindow::Console)
		{
			((KviTreeWindowListItem *)(wnd->console()->m_pWindowListItem))->setExpanded(true);
			return new KviTreeWindowListItem(((KviTreeWindowListItem *)(wnd->console()->m_pWindowListItem)), wnd);
		}
	}

	// console, or a window that has no irc context
	return new KviTreeWindowListItem(m_pTreeWidget, wnd);
}

bool KviTreeWindowList::removeItem(KviWindowListItem * it)
{
	if(it)
	{
		KviTreeWindowListItem * item = (KviTreeWindowListItem *)it;
		if(m_pTreeWidget)
		{
			int index = m_pTreeWidget->indexOfTopLevelItem(item);
			if(index >= 0)
			{
				delete(KviTreeWindowListItem *)m_pTreeWidget->takeTopLevelItem(index);
			}
			else
			{
				index = item->parent()->indexOfChild(item);
				if(index >= 0)
					delete(KviTreeWindowListItem *)item->parent()->takeChild(index);
			}
		}
	}
	return true;
}

void KviTreeWindowList::setActiveItem(KviWindowListItem * it)
{
	if(it)
	{
		if(((KviTreeWindowListItem *)it)->parent())
		{
			if(!((KviTreeWindowListItem *)it)->parent()->isExpanded())
				((KviTreeWindowListItem *)it)->parent()->setExpanded(true);
		}

		((KviTreeWindowListItem *)it)->setActive(true);
		if(g_pMainWindow->trayIcon())
			g_pMainWindow->trayIcon()->refresh();
	}
}

void KviTreeWindowList::updateActivityMeter()
{
	if(KVI_OPTION_BOOL(KviOption_boolUseWindowListActivityMeter))
	{
		m_pTreeWidget->viewport()->update();
	}
}

void KviTreeWindowList::wheelEvent(QWheelEvent * e)
{
	// Override KviWindowListBase::wheelEvent.
	// Mouse wheel handling is done in
	// KviTreeWindowListTreeWidget::wheelEvent.
}

KviWindowListItem * KviTreeWindowList::firstItem()
{
	m_pCurrentItem = (KviTreeWindowListItem *)m_pTreeWidget->topLevelItem(0);
	return m_pCurrentItem;
}

KviWindowListItem * KviTreeWindowList::nextItem()
{
	if(!m_pCurrentItem)
		return nullptr;
	return m_pCurrentItem = (KviTreeWindowListItem *)m_pTreeWidget->itemBelow(m_pCurrentItem);
}

KviWindowListItem * KviTreeWindowList::prevItem()
{
	if(!m_pCurrentItem)
		return nullptr;
	return m_pCurrentItem = (KviTreeWindowListItem *)m_pTreeWidget->itemAbove(m_pCurrentItem);
}

KviWindowListItem * KviTreeWindowList::lastItem()
{
	// first find last toplevel item
	int tops = m_pTreeWidget->topLevelItemCount();
	if(tops > 0)
	{
		int ctops = m_pTreeWidget->topLevelItem(tops - 1)->childCount();
		if(ctops)
		{
			return m_pCurrentItem = (KviTreeWindowListItem *)m_pTreeWidget->topLevelItem(tops - 1)->child(ctops - 1);
		}
		else
		{
			return m_pCurrentItem = (KviTreeWindowListItem *)m_pTreeWidget->topLevelItem(tops - 1);
		}
	}
	else
	{
		return nullptr;
	}
}

bool KviTreeWindowList::setIterationPointer(KviWindowListItem * it)
{
	m_pCurrentItem = (KviTreeWindowListItem *)it;
	if(!m_pCurrentItem)
		return true;
	if(m_pTreeWidget == m_pCurrentItem->treeWidget())
		return true;
	m_pCurrentItem = nullptr;
	return false;
}

// KviTreeWindowListItemDelegate

void KviTreeWindowListItemDelegate::paint(QPainter * p, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	QString szText = index.data(Qt::DisplayRole).toString();
	int iProgress = index.data(KVI_TTBID_PROGRESS).toInt();

	//FIXME not exactly model/view coding style.. but we need to access data on the associated window
	KviTreeWindowListTreeWidget * treeWidget = (KviTreeWindowListTreeWidget *)parent();
	KviTreeWindowListItem * item = (KviTreeWindowListItem *)treeWidget->itemFromIndex(index);
	KviWindow * pWindow = item->kviWindow();

	QStyleOptionViewItem opt(option);
	opt.state = opt.state | QStyle::State_Selected;
	opt.showDecorationSelected = true;
	//paint cell background
	if(treeWidget->currentItem() == item)
	{
		//selection colored background
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		if(treeWidget->style()->inherits("QStyleSheetStyle") || treeWidget->style()->inherits("QWindowsVistaStyle"))
		{
			// The Windows style does not honor our colors. It uses the system ones instead.
			// We can't accept it.
			p->fillRect(opt.rect, KVI_OPTION_COLOR(KviOption_colorTreeWindowListActiveBackground));
		}
		else
		{
#endif
			opt.palette.setColor(QPalette::Highlight, KVI_OPTION_COLOR(KviOption_colorTreeWindowListActiveBackground));
			treeWidget->style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, p, treeWidget);
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		}
#endif
#ifndef COMPILE_ON_MAC
	}
	else
	{
		if(KVI_OPTION_BOOL(KviOption_boolEnableVisualEffects) && option.state & QStyle::State_MouseOver)
		{
			// paint mouse over effect
			QColor col(KVI_OPTION_COLOR(KviOption_colorTreeWindowListActiveBackground));
			col.setAlpha(127);

	#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
			if(treeWidget->style()->inherits("QStyleSheetStyle") || treeWidget->style()->inherits("QWindowsVistaStyle"))
			{
				// The Windows style does not honor our colors. It uses the system ones instead.
				// We can't accept it.
				p->fillRect(opt.rect, col);
			}
			else
			{
	#endif
				opt.palette.setColor(QPalette::Highlight, col);
				treeWidget->style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, p, treeWidget);
	#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
			}
	#endif
		}
#endif
	}
	//draw window icon, irc context indicator (a colored square), set font properties for text
	int im = option.rect.left() + 2;
	int yPixmap = (option.rect.top() + (option.rect.height() / 2 - 8));

	QRect cRect(im + 3, option.rect.top(), option.rect.width(), option.rect.height());

	switch(pWindow->type())
	{
		case KviWindow::Console:
		{
			if(KVI_OPTION_BOOL(KviOption_boolUseWindowListIrcContextIndicator))
			{
				QColor base = option.palette.window().color();
				QColor cntx = KVI_OPTION_ICCOLOR(pWindow->console()->context()->id() % KVI_NUM_ICCOLOR_OPTIONS);
				base.setRgb((base.red() + cntx.red()) >> 1, (base.green() + cntx.green()) >> 1,
				    (base.blue() + cntx.blue()) >> 1);
				p->fillRect(im + 2, yPixmap, 14, 15, base);
				if(KVI_OPTION_BOOL(KviOption_boolUseWindowListIcons))
				{
					p->drawPixmap(im + 20, yPixmap, *(pWindow->myIconPtr()));
					cRect.setLeft(cRect.left() + 37);
				}
				else
				{
					cRect.setLeft(cRect.left() + 20);
				}
			}
			else
			{
				if(KVI_OPTION_BOOL(KviOption_boolUseWindowListIcons))
				{
					p->drawPixmap(im, yPixmap, *(pWindow->myIconPtr()));
					cRect.setLeft(cRect.left() + 17);
				}
			}
			//console window: bold font
			QFont f = p->font();
			f.setBold(true);
			p->setFont(f);
		}
		break;
		case KviWindow::Channel:
		case KviWindow::DeadChannel:
		case KviWindow::Query:
		case KviWindow::DeadQuery:
		default:
			if(KVI_OPTION_BOOL(KviOption_boolUseWindowListIcons))
			{
				p->drawPixmap(im, yPixmap, *(pWindow->myIconPtr()));
				cRect.setLeft(cRect.left() + 17);
			}
			//channels, other windows: normal font
			QFont f = p->font();
			f.setBold(false);
			p->setFont(f);
			break;
	}

	// paint the channel activity meter
	if(KVI_OPTION_BOOL(KviOption_boolUseWindowListActivityMeter))
	{
		unsigned int uActivityValue;
		unsigned int uActivityTemperature;
		if(pWindow->activityMeter(&uActivityValue, &uActivityTemperature))
		{
			p->drawPixmap(cRect.left(), yPixmap, *g_pActivityMeterPixmap, uActivityValue * 5, uActivityTemperature * 16, 5, 16);
			cRect.setLeft(cRect.left() + 7);
		}
	}

	// paint the progress bar
	if(iProgress >= 0)
	{
		int wdth = (iProgress * cRect.width()) / 100;
		p->fillRect(cRect.x(), cRect.y(), wdth, cRect.height(), KVI_OPTION_COLOR(KviOption_colorTreeWindowListProgress));
	}

	//choose window name font color (highlighting)
	if(treeWidget->currentItem() == item)
	{
		p->setPen(KVI_OPTION_COLOR(KviOption_colorTreeWindowListActiveForeground));
	}
	else
	{
		int iHighlightLevel = index.data(KVI_TTBID_HIGHLIGHT).toInt();
		int iLevel;

		switch(iHighlightLevel)
		{
			case 0:
				iLevel = KviOption_colorTreeWindowListForeground;
				break;
			case 1:
				iLevel = KviOption_colorTreeWindowListHighlight1Foreground;
				break;
			case 2:
				iLevel = KviOption_colorTreeWindowListHighlight2Foreground;
				break;
			case 3:
				iLevel = KviOption_colorTreeWindowListHighlight3Foreground;
				break;
			case 4:
				iLevel = KviOption_colorTreeWindowListHighlight4Foreground;
				break;
			default:
				iLevel = KviOption_colorTreeWindowListHighlight5Foreground;
				break;
		}

		p->setPen(KVI_OPTION_COLOR(iLevel));
	}

	//qDebug("Draw text in %d,%d,%d,%d (%s)",cRect.left(),cRect.top(),cRect.width(),cRect.height(),szText.toUtf8().data());

	//draw window name
	p->drawText(cRect, Qt::AlignLeft | Qt::AlignVCenter, szText);
}

QSize KviTreeWindowListItemDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const
{
	KviTreeWindowListTreeWidget * treeWidget = (KviTreeWindowListTreeWidget *)parent();
	int iHeight = treeWidget->fontMetrics().lineSpacing();

	iHeight += iHeight / 3;
	if((KVI_OPTION_BOOL(KviOption_boolUseWindowListIrcContextIndicator) || KVI_OPTION_BOOL(KviOption_boolUseWindowListIcons) || KVI_OPTION_BOOL(KviOption_boolUseWindowListActivityMeter)) && iHeight < 20)
		iHeight = 20;

	return { treeWidget->viewport()->size().width(), iHeight };
}
