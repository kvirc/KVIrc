//=============================================================================
//
//   File : kvi_windowlist_tree.cpp
//   Creation date : Thu Jan 7 1999 03:56:50 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2008 Fabio Bas (ctrlaltca at gmail dot com)
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
#include <QTimer>

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	extern QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif

extern QPixmap * g_pActivityMeterPixmap;

#define KVI_NUM_STEPS 20
#define KVI_TIMER_DELAY 18


// KviTreeWindowListItem

KviTreeWindowListItem::KviTreeWindowListItem(KviTalTreeWidget * par,KviWindow * wnd)
: KviTalTreeWidgetItem(par) , KviWindowListItem(wnd)
{
	m_iStepNumber=0;
	m_bIncreasing=0;
	m_pInternal=new KviTreeWindowListItemInternal(this);
	m_pAnimTimer=new QTimer();
	QObject::connect( m_pAnimTimer, SIGNAL(timeout()), m_pInternal, SLOT(timerShot()));
	applyOptions();
	//sort the widget
	treeWidget()->sortItems(0,Qt::AscendingOrder);
}

KviTreeWindowListItem::KviTreeWindowListItem(KviTreeWindowListItem * par,KviWindow * wnd)
: KviTalTreeWidgetItem(par) , KviWindowListItem(wnd)
{
	m_iStepNumber=0;
	m_bIncreasing=0;
	m_pInternal=new KviTreeWindowListItemInternal(this);
	m_pAnimTimer=new QTimer();
	QObject::connect( m_pAnimTimer, SIGNAL(timeout()), m_pInternal, SLOT(timerShot()));
	applyOptions();
	//sort the widget
	treeWidget()->sortItems(0,Qt::AscendingOrder);
}

KviTreeWindowListItem::~KviTreeWindowListItem()
{
	KviTalTreeWidget* pView=(KviTalTreeWidget *)treeWidget();
	if(pView)
		if(((KviTreeWindowListTreeWidget*)(pView))->m_pPrevItem==this) ((KviTreeWindowListTreeWidget*)(treeWidget()))->m_pPrevItem=0;
	delete m_pAnimTimer;
	delete m_pInternal;
}

void KviTreeWindowListItem::applyOptions()
{
	setData(0, KVI_TTBID_REDDIFF, (KVI_OPTION_COLOR(KviOption_colorTreeWindowListActiveBackground).red()-KVI_OPTION_COLOR(KviOption_colorTreeWindowListBackground).red())/KVI_NUM_STEPS);
	setData(0, KVI_TTBID_GREENDIFF, (KVI_OPTION_COLOR(KviOption_colorTreeWindowListActiveBackground).green()-KVI_OPTION_COLOR(KviOption_colorTreeWindowListBackground).green())/KVI_NUM_STEPS);
	setData(0, KVI_TTBID_BLUEDIFF, (KVI_OPTION_COLOR(KviOption_colorTreeWindowListActiveBackground).blue()-KVI_OPTION_COLOR(KviOption_colorTreeWindowListBackground).blue())/KVI_NUM_STEPS);
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

	//sort the widget
	treeWidget()->sortItems(0,Qt::AscendingOrder);

	setData(0, Qt::DisplayRole, szText);
}

void KviTreeWindowListItem::unhighlight()
{
	if(m_iHighlightLevel < 1)return;
	m_iHighlightLevel = 0;
	setData(0, KVI_TTBID_HIGHLIGHT, m_iHighlightLevel);

	if(g_pFrame->dockExtension())g_pFrame->dockExtension()->refresh();
}

void KviTreeWindowListItem::highlight(int iLevel)
{
	if(iLevel <= m_iHighlightLevel)return;
	if(isSelected() && g_pFrame->isActiveWindow())return;
	m_iHighlightLevel = iLevel;
	setData(0, KVI_TTBID_HIGHLIGHT, m_iHighlightLevel);

	if(g_pFrame->dockExtension())g_pFrame->dockExtension()->refresh();
	if(isSelected())return;
}

void KviTreeWindowListItem::setProgress(int progress)
{
	if(progress == m_iProgress)return;
	m_iProgress = progress;
	setData(0, KVI_TTBID_PROGRESS, m_iProgress);
}

void KviTreeWindowListItem::setActive(bool bActive)
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

void KviTreeWindowListItem::timerShot()
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

	QRect rect = treeWidget()->visualItemRect(this);
 	treeWidget()->update(QRect(0, rect.top(), rect.left()+rect.width(), rect.height()));
}

void KviTreeWindowListItem::mouseEnter()
{
	if(KVI_OPTION_BOOL(KviOption_boolEnableVisualEffects))
	{
		m_bIncreasing=true;
		if(!m_pAnimTimer->isActive()) m_pAnimTimer->start(KVI_TIMER_DELAY);
	}
}

void KviTreeWindowListItem::mouseLeave()
{
	if(KVI_OPTION_BOOL(KviOption_boolEnableVisualEffects))
	{
		m_bIncreasing=false;
		if(!m_pAnimTimer->isActive()) m_pAnimTimer->start(KVI_TIMER_DELAY);
	}
}

// KviTreeWindowListTreeWidget

KviTreeWindowListTreeWidget::KviTreeWindowListTreeWidget(QWidget * par)
: KviTalTreeWidget(par)
{
	setObjectName("tree_windowlist");
	setRootIsDecorated(true);
	//Animation creates problem with the background painting on expande/collapse
	//setAnimated(true);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setFont(KVI_OPTION_FONT(KviOption_fontTreeWindowList));
	setFocusPolicy(Qt::NoFocus);
	setFrameShape(NoFrame);

	if(KVI_OPTION_BOOL(KviOption_boolShowTreeWindowListHeader))
	{
		header()->show();
	} else {
		header()->hide();
	}

	header()->setResizeMode(QHeaderView::Interactive);

	viewport()->setAutoFillBackground(false);
	viewport()->setMouseTracking(TRUE);

	m_pPrevItem=0;
}

KviTreeWindowListTreeWidget::~KviTreeWindowListTreeWidget()
{
}

void KviTreeWindowListTreeWidget::mouseMoveEvent ( QMouseEvent * e )
{
	KviTreeWindowListItem* pCur=(KviTreeWindowListItem*)(itemAt(e->pos()));
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

void KviTreeWindowListTreeWidget::leaveEvent(QEvent *)
{
	if(m_pPrevItem) m_pPrevItem->mouseLeave();
	m_pPrevItem=0;
	setCursor(Qt::ArrowCursor);
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

void KviTreeWindowListTreeWidget::mouseDoubleClickEvent(QMouseEvent * e)
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
	QStyleOptionViewItem option = viewOptions();
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

	if(KVI_OPTION_UINT(KviOption_uintTreeWindowListMaximumWidth) < iMin)
		KVI_OPTION_UINT(KviOption_uintTreeWindowListMaximumWidth) = iMin;
	int iMax = 600;
	if(((unsigned int)iMax) > KVI_OPTION_UINT(KviOption_uintTreeWindowListMaximumWidth))
		iMax = KVI_OPTION_UINT(KviOption_uintTreeWindowListMaximumWidth);
	setMaximumWidth(iMax+4);

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
		{
			((KviTreeWindowListItem *)(wnd->console()->m_pWindowListItem))->setExpanded(true);
			return new KviTreeWindowListItem(((KviTreeWindowListItem *)(wnd->console()->m_pWindowListItem)),wnd);
		}
	}

	// console , or a window that has no irc context
	return new KviTreeWindowListItem(m_pTreeWidget,wnd);
}

bool KviTreeWindowList::removeItem(KviWindowListItem * it)
{
	delete (KviTreeWindowListItem *)it;
	return true;
}

void KviTreeWindowList::setActiveItem(KviWindowListItem * it)
{
	if(it)
	{
		KviTreeWindowListItem * cur = (KviTreeWindowListItem *)m_pTreeWidget->currentItem();
		if(cur && (cur != (KviTreeWindowListItem *)it))
		{
			cur->setActive(false);
		}
		if(((KviTreeWindowListItem *)it)->parent())
		{
			if(!((KviTreeWindowListItem *)it)->parent()->isExpanded())((KviTreeWindowListItem *)it)->parent()->setExpanded(true);
		}

		//default selectionModel doesn't enforce singleselection if done from the code
		m_pTreeWidget->selectionModel()->clearSelection();
		((KviTreeWindowListItem *)it)->setActive(true);
		((KviTreeWindowListItem *)it)->setSelected(true); // this MUST go after it->setActive()
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

// KviTreeWindowListItemDelegate

int KviTreeWindowListItemDelegate::calculateColor(int col1,int col2, int iStepNumber) const
{
	int result=col1+(col2-col1)/KVI_NUM_STEPS*iStepNumber;
	return result<255 ? result :255;
}

void KviTreeWindowListItemDelegate::paint(QPainter * p, const QStyleOptionViewItem & option, const QModelIndex & index) const
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
	KviTreeWindowListTreeWidget* treeWidget = (KviTreeWindowListTreeWidget*)parent();
	KviTreeWindowListItem* item = (KviTreeWindowListItem*) treeWidget->itemFromIndex(index);
	KviWindow* pWindow = item->kviWindow();

	//paint cell background
	if (option.state & QStyle::State_Selected)
	{
		//selection colored background
		p->fillRect(fullRect,KVI_OPTION_COLOR(KviOption_colorTreeWindowListActiveBackground));
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
				//background image (if defined)
				QPixmap * pix = KVI_OPTION_PIXMAP(KviOption_pixmapTreeWindowListBackground).pixmap();
				if(pix)
				{
					KviPixmapUtils::drawPixmapWithPainter(p,pix,KVI_OPTION_UINT(KviOption_uintTreeWindowListPixmapAlign),fullRect,fullRect.width(),fullRect.height());
				} else {
					p->fillRect(fullRect,KVI_OPTION_COLOR(KviOption_colorTreeWindowListBackground));
				}
#ifdef COMPILE_PSEUDO_TRANSPARENCY
			}
#endif
		} else {
			// paint mouse over effect
			p->fillRect(fullRect,
				QColor(KVI_OPTION_COLOR(KviOption_colorTreeWindowListBackground).red()+iRedDiff*iStepNumber,
					KVI_OPTION_COLOR(KviOption_colorTreeWindowListBackground).green()+iGreenDiff*iStepNumber,
					KVI_OPTION_COLOR(KviOption_colorTreeWindowListBackground).blue()+iBlueDiff*iStepNumber
					)
				);
		}
	}

	//tree branches
	treeWidget->drawBranches(p, branchRect, index);

	//draw window icon, irc context indicator (a colored square), set font properties for text
	int im = rect.left();
	int yPixmap = (rect.top() + (rect.height() / 2 - 8));

	QRect cRect(im + 3,rect.top(),rect.width(),rect.height());

	switch(pWindow->type())
	{
		case KVI_WINDOW_TYPE_CONSOLE:
		{
			if(KVI_OPTION_BOOL(KviOption_boolUseWindowListIrcContextIndicator))
			{
				QColor base = option.palette.window().color();
				QColor cntx = KVI_OPTION_ICCOLOR(pWindow->console()->context()->id() % KVI_NUM_ICCOLOR_OPTIONS);
				base.setRgb((base.red() + cntx.red()) >> 1,(base.green() + cntx.green()) >> 1,
					(base.blue() + cntx.blue()) >> 1);
				p->fillRect(im + 2,yPixmap,14,15,base);
				if(KVI_OPTION_BOOL(KviOption_boolUseWindowListIcons))
				{
					p->drawPixmap(im + 20,yPixmap,*(pWindow->myIconPtr()));
					cRect.setLeft(cRect.left() + 37);
				} else {
					cRect.setLeft(cRect.left() + 20);
				}
			} else {
				if(KVI_OPTION_BOOL(KviOption_boolUseWindowListIcons))
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
			if(KVI_OPTION_BOOL(KviOption_boolUseWindowListIcons))
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
	if(KVI_OPTION_BOOL(KviOption_boolUseWindowListActivityMeter))
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
		p->fillRect(cRect.x(),cRect.y(),wdth,cRect.height(),KVI_OPTION_COLOR(KviOption_colorTreeWindowListProgress));
	}

	//choose window name font color (highlighting)
	if (option.state & QStyle::State_Selected)
	{
		p->setPen(KVI_OPTION_COLOR(KviOption_colorTreeWindowListActiveForeground));
	} else {
		int iLevel;
		switch(iHighlightLevel)
		{
			case 0: iLevel = KviOption_colorTreeWindowListForeground; break;
			case 1: iLevel = KviOption_colorTreeWindowListHighlight1Foreground; break;
			case 2: iLevel = KviOption_colorTreeWindowListHighlight2Foreground; break;
			case 3: iLevel = KviOption_colorTreeWindowListHighlight3Foreground; break;
			case 4: iLevel = KviOption_colorTreeWindowListHighlight4Foreground; break;
			default: iLevel = KviOption_colorTreeWindowListHighlight5Foreground; break;
		}
		p->setPen(
			QColor(
				calculateColor(KVI_OPTION_COLOR(iLevel).red(),KVI_OPTION_COLOR(KviOption_colorTreeWindowListActiveForeground).red(),iStepNumber),
				calculateColor(KVI_OPTION_COLOR(iLevel).green(),KVI_OPTION_COLOR(KviOption_colorTreeWindowListActiveForeground).green(),iStepNumber),
				calculateColor(KVI_OPTION_COLOR(iLevel).blue(),KVI_OPTION_COLOR(KviOption_colorTreeWindowListActiveForeground).blue(),iStepNumber)
				)
			);
	}

	//draw window name
	p->drawText(cRect,Qt::AlignLeft | Qt::AlignVCenter,szText);
}

QSize KviTreeWindowListItemDelegate::sizeHint( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	KviTreeWindowListTreeWidget* treeWidget = (KviTreeWindowListTreeWidget*)parent();

	return QSize(treeWidget->viewport()->size().width(), qMax(20, treeWidget->fontMetrics().xHeight() * 3));
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "kvi_windowlist_tree.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
