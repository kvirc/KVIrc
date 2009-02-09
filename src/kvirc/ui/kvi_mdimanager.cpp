//=============================================================================
//
//   File : kvi_mdimanager.cpp
//   Creation date : Wed Jun 21 2000 17:28:04 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2008 TheXception (kvirc at thexception dot net)
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

/**
* \file kvi_mdimanager.cpp
* \brief The MDI-manager
*/


#include "kvi_debug.h"
#include "kvi_settings.h"
#include "kvi_mdimanager.h"
#include "kvi_mdichild.h"
#include "kvi_locale.h"
#include "kvi_options.h"
#include "kvi_iconmanager.h"
#include "kvi_frame.h"
#include "kvi_menubar.h"
#include "kvi_app.h"
#include "kvi_tal_popupmenu.h"
#include "kvi_tal_hbox.h"

#include <QMenuBar>
#include <QLayout>
#include <QPainter>
#include <QCursor>
#include <QEvent>
#include <QMouseEvent>
#include <QLabel>
#include <QToolButton>


#ifdef COMPILE_PSEUDO_TRANSPARENCY
	#include <QPixmap>
	extern QPixmap * g_pShadedParentGlobalDesktopBackground;
#endif

#include <math.h>


KviMdiManager::KviMdiManager(QWidget * parent,KviFrame * pFrm,const char *)
: QMdiArea(parent)
{
	setFrameShape(NoFrame);

	m_pFrm = pFrm;

	m_pWindowPopup = new KviTalPopupMenu(this);
	connect(m_pWindowPopup,SIGNAL(activated(int)),this,SLOT(menuActivated(int)));
	connect(m_pWindowPopup,SIGNAL(aboutToShow()),this,SLOT(fillWindowPopup()));
	m_pTileMethodPopup = new KviTalPopupMenu(this);
	connect(m_pTileMethodPopup,SIGNAL(activated(int)),this,SLOT(tileMethodMenuActivated(int)));

	viewport()->setAutoFillBackground(false);
//	setStaticBackground(true);

	//setFocusPolicy(Qt::NoFocus);
	//viewport()->setFocusPolicy(Qt::NoFocus);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	connect(g_pApp,SIGNAL(reloadImages()),this,SLOT(reloadImages()));
}

KviMdiManager::~KviMdiManager()
{
}

void KviMdiManager::reloadImages()
{
}

bool KviMdiManager::focusNextPrevChild(bool bNext)
{
	//bug("FFFFFF");
	// this is a QScrollView bug... it doesn't pass this
	// event to the toplevel window
	return m_pFrm->focusNextPrevChild(bNext);
}

void KviMdiManager::paintEvent(QPaintEvent * event)
{
	QPainter p(viewport());
	QPoint pnt = viewport()->mapToGlobal(event->rect().topLeft());
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(g_pShadedParentGlobalDesktopBackground)
	{
		p.drawTiledPixmap(event->rect(),*(g_pShadedParentGlobalDesktopBackground), pnt);
		return;
	}
#endif

	if(KVI_OPTION_PIXMAP(KviOption_pixmapMdiBackground).pixmap())
	{
		p.drawTiledPixmap(event->rect(),*(KVI_OPTION_PIXMAP(KviOption_pixmapMdiBackground).pixmap()),pnt);
	} else {
		p.fillRect(event->rect(),KVI_OPTION_COLOR(KviOption_colorMdiBackground));
	}
}

void KviMdiManager::manageChild(KviMdiChild * lpC, bool, QRect *)
{
	__range_valid(lpC);

	addSubWindow((QMdiSubWindow*)lpC);

	if(isInSDIMode()) lpC->queuedMaximize();

	if(KVI_OPTION_BOOL(KviOption_boolAutoTileWindows))tile();
}

void KviMdiManager::showAndActivate(KviMdiChild * lpC)
{
	lpC->show();
	setTopChild(lpC,true);
	if(KVI_OPTION_BOOL(KviOption_boolAutoTileWindows))tile();
}

KviMdiChild * KviMdiManager::topChild()
{
	return (KviMdiChild*)activeSubWindow();
};

void KviMdiManager::setTopChild(KviMdiChild *lpC,bool bSetFocus)
{
	__range_valid(lpC);
	// The following check fails safely at startup....

	QList<QMdiSubWindow *> tmp = subWindowList(QMdiArea::StackingOrder);

	if (tmp.last()->inherits("KviMdiChild"))
	{
		KviMdiChild * oldlpC = (KviMdiChild *) tmp.last();
		if (oldlpC && oldlpC->isMaximized()) lpC->queuedMaximize();
	}

	setActiveSubWindow((QMdiSubWindow*) lpC);

	if(bSetFocus)
	{
		if(!lpC->hasFocus())
		{
			lpC->setFocus();
		}
	}

}

void KviMdiManager::destroyChild(KviMdiChild *lpC,bool bFocusTopChild)
{
	removeSubWindow(lpC);

	if (lpC->isMaximized() && activeSubWindow())
	{
		if (!activeSubWindow()->inherits("KviMdiChild")) return;

		KviMdiChild * tmp = (KviMdiChild *) activeSubWindow();
		tmp->queuedMaximize();
	}

	delete lpC;

	if(bFocusTopChild)focusTopChild();

	if(KVI_OPTION_BOOL(KviOption_boolAutoTileWindows)) tile();
}

KviMdiChild * KviMdiManager::highestChildExcluding(KviMdiChild * pChild)
{
	QList<QMdiSubWindow *> tmp = subWindowList(QMdiArea::StackingOrder);
	QListIterator<QMdiSubWindow*> wl(tmp);
	wl.toBack();

	KviMdiChild * c;
	while (wl.hasPrevious())
	{
			c = (KviMdiChild*) wl.previous();
			if (!c->inherits("KviMdiChild")) continue;
			if (c == pChild) continue;
			return c;
	}
	return 0;
}

QPoint KviMdiManager::getCascadePoint(int indexOfWindow)
{
	QPoint pnt(0,0);
	if(indexOfWindow==0)return pnt;
	QList<QMdiSubWindow *> tmp = subWindowList(QMdiArea::StackingOrder);

	if (!tmp.last()->inherits("KviMdiChild"))
	{
		debug("QMdiSubWindow doesn't inherit from KviMdiChild!");
		return pnt;
	}

	KviMdiChild * lpC = (KviMdiChild *) tmp.last();

	int step=0;
	int availableHeight=viewport()->height()-(lpC ? lpC->minimumSize().height() : KVI_MDICHILD_MIN_HEIGHT);
	int availableWidth=viewport()->width()-(lpC ? lpC->minimumSize().width() : KVI_MDICHILD_MIN_WIDTH);
	int ax=0;
	int ay=0;
	for(int i=0;i<indexOfWindow;i++)
	{
		ax+=step;
		ay+=step;
		if(ax>availableWidth)ax=0;
		if(ay>availableHeight)ay=0;
	}
	pnt.setX(ax);
	pnt.setY(ay);
	return pnt;
}

void KviMdiManager::mousePressEvent(QMouseEvent * e)
{
	//Popup the window menu
	if(e->button() & Qt::RightButton) m_pWindowPopup->popup(mapToGlobal(e->pos()));
}

void KviMdiManager::childMoved(KviMdiChild *)
{

}

void KviMdiManager::childMaximized(KviMdiChild *)
{
	m_bInSDIMode = true;
}


void KviMdiManager::childMinimized(KviMdiChild * lpC, bool bWasMaximized)
{
	__range_valid(lpC);

	if(subWindowList().count() > 1)
	{
		if(!bWasMaximized)
		{
			if(KVI_OPTION_BOOL(KviOption_boolAutoTileWindows)) tile();
		}
		focusTopChild();
	} else {
		// Unique window minimized...it won't loose the focus...!!
		setFocus(); //Remove focus from the child
	}
}

void KviMdiManager::childRestored(KviMdiChild * lpC, bool bWasMaximized)
{
	if(bWasMaximized)
	{
		if(lpC != subWindowList().last()) return; // do nothing in this case
	}
	if(KVI_OPTION_BOOL(KviOption_boolAutoTileWindows)) tile();
	if (bWasMaximized) m_bInSDIMode = false;
}

void KviMdiManager::focusTopChild()
{
	if (!activeSubWindow()) return;
	if (!activeSubWindow()->inherits("KviMdiChild")) return;

	KviMdiChild * lpC = 0;

	QList<QMdiSubWindow *> tmp = subWindowList(QMdiArea::ActivationHistoryOrder);
	QListIterator<QMdiSubWindow*> wl(tmp);
	wl.toBack();

	while (wl.hasPrevious())
	{
			lpC = (KviMdiChild*) wl.previous();
			if (!lpC->inherits("KviMdiChild")) continue;

			if (lpC->state() != KviMdiChild::Minimized)
			{
				break;
			}
	}

	if(!lpC)return;
	if(isInSDIMode()) lpC->queuedMaximize();
	lpC->raise();
	if(!lpC->hasFocus())lpC->setFocus();
}

void KviMdiManager::minimizeActiveChild()
{
	if (!activeSubWindow()->inherits("KviMdiChild")) return;

	KviMdiChild * lpC = (KviMdiChild *) activeSubWindow();
	if(lpC->state() != KviMdiChild::Minimized) lpC->minimize();
}

void KviMdiManager::restoreActiveChild()
{
	if (!activeSubWindow()->inherits("KviMdiChild")) return;

	KviMdiChild * lpC = (KviMdiChild *) activeSubWindow();
	if(lpC->state() == KviMdiChild::Maximized)lpC->restore();
}

void KviMdiManager::closeActiveChild()
{
	if (!activeSubWindow()->inherits("KviMdiChild")) return;

	KviMdiChild * lpC = (KviMdiChild *) activeSubWindow();
	lpC->close();
}

void KviMdiManager::updateContentsSize()
{
}

void KviMdiManager::activeChildSystemPopup()
{
	if (!activeSubWindow()->inherits("KviMdiChild")) return;

	KviMdiChild * lpC = (KviMdiChild *) activeSubWindow();

	QPoint pnt;
	/*if(m_pSdiIconButton)
	{
		pnt = m_pSdiIconButton->mapToGlobal(QPoint(0,m_pSdiIconButton->height()));
	} else {
		pnt = QCursor::pos();
	}*/
	lpC->emitSystemPopupRequest(pnt);
}

bool KviMdiManager::isInSDIMode()
{
	return m_bInSDIMode;
}

void KviMdiManager::relayoutMenuButtons()
{
	// also force an activation of the top MdiChild since it probably didn't get it yet*/
	KviMdiChild * c = topChild();
	if(c) c->activate(false);
}

#define KVI_TILE_METHOD_ANODINE 0
#define KVI_TILE_METHOD_PRAGMA4HOR 1
#define KVI_TILE_METHOD_PRAGMA4VER 2
#define KVI_TILE_METHOD_PRAGMA6HOR 3
#define KVI_TILE_METHOD_PRAGMA6VER 4
#define KVI_TILE_METHOD_PRAGMA9HOR 5
#define KVI_TILE_METHOD_PRAGMA9VER 6

#define KVI_NUM_TILE_METHODS 7

void KviMdiManager::fillWindowPopup()
{
	m_pWindowPopup->clear();

	m_pWindowPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CASCADEWINDOWS)),(__tr2qs("&Cascade Windows")),this,SLOT(cascadeWindows()));
	m_pWindowPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CASCADEWINDOWS)),(__tr2qs("Cascade &Maximized")),this,SLOT(cascadeMaximized()));

	m_pWindowPopup->insertSeparator();
	m_pWindowPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TILEWINDOWS)),(__tr2qs("&Tile Windows")),this,SLOT(tile()));

	m_pTileMethodPopup->clear();
	int id = m_pTileMethodPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_AUTOTILEWINDOWS)),(__tr2qs("&Auto Tile")),this,SLOT(toggleAutoTile()));
	m_pTileMethodPopup->setItemChecked(id,KVI_OPTION_BOOL(KviOption_boolAutoTileWindows));
	m_pTileMethodPopup->setItemParameter(id,-1);
	m_pTileMethodPopup->insertSeparator();
	int ids[KVI_NUM_TILE_METHODS];
	ids[KVI_TILE_METHOD_ANODINE] = m_pTileMethodPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TILEWINDOWS)),(__tr2qs("Anodine's Full Grid")));
	m_pTileMethodPopup->setItemParameter(ids[KVI_TILE_METHOD_ANODINE],KVI_TILE_METHOD_ANODINE);
	ids[KVI_TILE_METHOD_PRAGMA4HOR] = m_pTileMethodPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TILEWINDOWS)),(__tr2qs("Pragma's Horizontal 4-Grid")));
	m_pTileMethodPopup->setItemParameter(ids[KVI_TILE_METHOD_PRAGMA4HOR],KVI_TILE_METHOD_PRAGMA4HOR);
	ids[KVI_TILE_METHOD_PRAGMA4VER] = m_pTileMethodPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TILEWINDOWS)),(__tr2qs("Pragma's Vertical 4-Grid")));
	m_pTileMethodPopup->setItemParameter(ids[KVI_TILE_METHOD_PRAGMA4VER],KVI_TILE_METHOD_PRAGMA4VER);
	ids[KVI_TILE_METHOD_PRAGMA6HOR] = m_pTileMethodPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TILEWINDOWS)),(__tr2qs("Pragma's Horizontal 6-Grid")));
	m_pTileMethodPopup->setItemParameter(ids[KVI_TILE_METHOD_PRAGMA6HOR],KVI_TILE_METHOD_PRAGMA6HOR);
	ids[KVI_TILE_METHOD_PRAGMA6VER] = m_pTileMethodPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TILEWINDOWS)),(__tr2qs("Pragma's Vertical 6-Grid")));
	m_pTileMethodPopup->setItemParameter(ids[KVI_TILE_METHOD_PRAGMA6VER],KVI_TILE_METHOD_PRAGMA6VER);
	ids[KVI_TILE_METHOD_PRAGMA9HOR] = m_pTileMethodPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TILEWINDOWS)),(__tr2qs("Pragma's Horizontal 9-Grid")));
	m_pTileMethodPopup->setItemParameter(ids[KVI_TILE_METHOD_PRAGMA9HOR],KVI_TILE_METHOD_PRAGMA9HOR);
	ids[KVI_TILE_METHOD_PRAGMA9VER] = m_pTileMethodPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TILEWINDOWS)),(__tr2qs("Pragma's Vertical 9-Grid")));
	m_pTileMethodPopup->setItemParameter(ids[KVI_TILE_METHOD_PRAGMA9VER],KVI_TILE_METHOD_PRAGMA9VER);

	if(KVI_OPTION_UINT(KviOption_uintTileMethod) >= KVI_NUM_TILE_METHODS)KVI_OPTION_UINT(KviOption_uintTileMethod) = KVI_TILE_METHOD_PRAGMA9HOR;
	m_pTileMethodPopup->setItemChecked(ids[KVI_OPTION_UINT(KviOption_uintTileMethod)],true);

	m_pWindowPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TILEWINDOWS)),(__tr2qs("Tile Met&hod")),m_pTileMethodPopup);

	m_pWindowPopup->insertSeparator();
	m_pWindowPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MAXVERTICAL)),(__tr2qs("Expand &Vertically")),this,SLOT(expandVertical()));
	m_pWindowPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MAXHORIZONTAL)),(__tr2qs("Expand &Horizontally")),this,SLOT(expandHorizontal()));

	m_pWindowPopup->insertSeparator();
	m_pWindowPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MINIMIZE)),(__tr2qs("Mi&nimize All")),this,SLOT(minimizeAll()));
    m_pWindowPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_RESTORE)),(__tr2qs("&Restore all")),this,SLOT(restoreAll()));

	m_pWindowPopup->insertSeparator();
	int i = 100;
	QString szItem;
	QString szCaption;
	QList<QMdiSubWindow*> tmp = subWindowList(QMdiArea::StackingOrder);
	QListIterator<QMdiSubWindow*> m_pZ(tmp);

	KviMdiChild * lpC;

	while (m_pZ.hasNext())
	{
		lpC = (KviMdiChild *) m_pZ.next();

		if (!lpC->inherits("KviMdiChild"))
		{
			i++;
			continue;
		}

		szItem.setNum(((uint)i)-99);
		szItem+=". ";

		szCaption = lpC->windowTitle();
		if(szCaption.length() > 30)
		{
			QString trail = szCaption.right(12);
			szCaption.truncate(12);
			szCaption+="...";
			szCaption+=trail;
		}

		if(lpC->state()==KviMdiChild::Minimized)
		{
			szItem+="(";
			szItem+=szCaption;
			szItem+=")";
		} else szItem+=szCaption;

		const QPixmap * pix = lpC->icon();

		if (pix && !(pix->isNull()))
		{
			m_pWindowPopup->insertItem(*pix, szItem,i);
		} else {
			m_pWindowPopup->insertItem(szItem);
		}

		//m_pWindowPopup->setItemChecked(i, ((uint)i) == (m_pZ->count()+99) ); <- ????
		i++;
	}
}

void KviMdiManager::menuActivated(int id)
{
	if(id<100)return;
	id-=100;
	QList<QMdiSubWindow *> tmp = subWindowList(QMdiArea::StackingOrder);

	__range_valid(((uint)id) < tmp.count());

	if (!tmp.at(id)->inherits("KviMdiChild")) return;
	KviMdiChild * lpC = (KviMdiChild *) tmp.at(id);

	if(!lpC) return;
	if(lpC->state() == KviMdiChild::Minimized) lpC->restore();

	setTopChild(lpC, true);
}

void KviMdiManager::ensureNoMaximized()
{
	QList<QMdiSubWindow *> tmp = subWindowList(QMdiArea::StackingOrder);

	KviMdiChild * lpC;

	for(int i = 0; i < tmp.count(); i++)
	{
		if (tmp.at(i)->inherits("KviMdiChild"))
		{
			lpC = (KviMdiChild *) tmp.at(i);
			if(lpC->state() == KviMdiChild::Maximized) lpC->restore();
		}
	}
}

void KviMdiManager::tileMethodMenuActivated(int id)
{
	int idx = m_pTileMethodPopup->itemParameter(id);

	if(idx < 0) idx = 0;
	if(idx >= KVI_NUM_TILE_METHODS) idx = KVI_TILE_METHOD_PRAGMA9VER;

	KVI_OPTION_UINT(KviOption_uintTileMethod) = idx;

	if(KVI_OPTION_BOOL(KviOption_boolAutoTileWindows)) tile();
}

void KviMdiManager::cascadeWindows()
{
}

void KviMdiManager::cascadeMaximized()
{
}

void KviMdiManager::expandVertical()
{
}

void KviMdiManager::expandHorizontal()
{
}

void KviMdiManager::minimizeAll()
{
	QList<QMdiSubWindow *> tmp = subWindowList(QMdiArea::StackingOrder);

	KviMdiChild * lpC;

	for(int i = 0; i < tmp.count(); i++)
	{
		if (tmp.at(i)->inherits("KviMdiChild"))
		{
			lpC = (KviMdiChild *) tmp.at(i);
			if(lpC->state() == KviMdiChild::Minimized) lpC->minimize();
		}
	}
}


void KviMdiManager::restoreAll()
{
	QList<QMdiSubWindow *> tmp = subWindowList(QMdiArea::StackingOrder);

	KviMdiChild * lpC;

	for(int i = 0; i < tmp.count(); i++)
	{
		if (tmp.at(i)->inherits("KviMdiChild"))
		{
			lpC = (KviMdiChild *) tmp.at(i);
			if(lpC->state() == KviMdiChild::Normal) lpC->restore();
		}
	}
}


int KviMdiManager::getVisibleChildCount()
{
	QList<QMdiSubWindow *> l = subWindowList();

	int cnt = 0;
	int i = 0;
	for(i = 0; i < l.count(); i++)
	{
		if(!l.at(i)->isHidden()) cnt++;
	}
	return cnt;
}

void KviMdiManager::tile()
{
	switch(KVI_OPTION_UINT(KviOption_uintTileMethod))
	{
		case KVI_TILE_METHOD_ANODINE:      tileAnodine(); break;
		case KVI_TILE_METHOD_PRAGMA4HOR:   tileAllInternal(4,true); break;
		case KVI_TILE_METHOD_PRAGMA4VER:   tileAllInternal(4,false); break;
		case KVI_TILE_METHOD_PRAGMA6HOR:   tileAllInternal(6,true); break;
		case KVI_TILE_METHOD_PRAGMA6VER:   tileAllInternal(6,false); break;
		case KVI_TILE_METHOD_PRAGMA9HOR:   tileAllInternal(9,true); break;
		case KVI_TILE_METHOD_PRAGMA9VER:   tileAllInternal(9,false); break;
		default:
			KVI_OPTION_UINT(KviOption_uintTileMethod) = KVI_TILE_METHOD_PRAGMA9HOR;
			tileAllInternal(9,true);
		break;
	}
}

void KviMdiManager::toggleAutoTile()
{
	if(KVI_OPTION_BOOL(KviOption_boolAutoTileWindows))
	{
		KVI_OPTION_BOOL(KviOption_boolAutoTileWindows) = false;
	} else {
		KVI_OPTION_BOOL(KviOption_boolAutoTileWindows) = true;
		tile();
	}
}


void KviMdiManager::tileAllInternal(int maxWnds, bool bHorizontal) //int maxWnds,bool bHorizontal
{

	//NUM WINDOWS =           1,2,3,4,5,6,7,8,9
	static int colstable[9]={ 1,1,1,2,2,2,3,3,3 }; //num columns
	static int rowstable[9]={ 1,2,3,2,3,3,3,3,3 }; //num rows
	static int lastwindw[9]={ 1,1,1,1,2,1,3,2,1 }; //last window multiplier
	static int colrecall[9]={ 0,0,0,3,3,3,6,6,6 }; //adjust self
	static int rowrecall[9]={ 0,0,0,0,4,4,4,4,4 }; //adjust self

	int * pColstable = bHorizontal ? colstable : rowstable;
	int * pRowstable = bHorizontal ? rowstable : colstable;
	int * pColrecall = bHorizontal ? colrecall : rowrecall;
	int * pRowrecall = bHorizontal ? rowrecall : colrecall;

	ensureNoMaximized();
	// this hack is needed to ensure that the scrollbars are hidden and the viewport()->width() and height() are correct
	//resizeContents(visibleWidth(),visibleHeight());
	// updateScrollBars();
	//g_pApp->sendPostedEvents();
	if (g_pApp->closingDown()) return;

	KviMdiChild * lpTop = topChild();
	if (!lpTop) return;

	int numVisible = getVisibleChildCount();

	if (numVisible < 1) return;

	int numToHandle = ((numVisible > maxWnds) ? maxWnds : numVisible);
	int xQuantum = viewport()->width() / pColstable[numToHandle-1];

	if(xQuantum < ((lpTop->minimumSize().width() > KVI_MDICHILD_MIN_WIDTH) ? lpTop->minimumSize().width() : KVI_MDICHILD_MIN_WIDTH))
	{
		if (pColrecall[numToHandle-1] == 0) debug("Tile : Not enouh space");
			else tileAllInternal(pColrecall[numToHandle-1], bHorizontal);
		return;
	}

	int yQuantum = viewport()->height() / pRowstable[numToHandle-1];

	if(yQuantum < ((lpTop->minimumSize().height() > KVI_MDICHILD_MIN_HEIGHT) ? lpTop->minimumSize().height() : KVI_MDICHILD_MIN_HEIGHT))
	{
		if (pRowrecall[numToHandle-1] == 0) debug("Tile : Not enough space");
			else tileAllInternal(pRowrecall[numToHandle-1], bHorizontal);
		return;
	}

	int curX = 0;
	int curY = 0;
	int curRow = 1;
	int curCol = 1;
	int curWin = 1;

	QList<QMdiSubWindow *> tmp = subWindowList(QMdiArea::StackingOrder);

	for(int i = 0; i < tmp.count(); i++)
	{
		KviMdiChild * lpC = (KviMdiChild*) tmp.at(i);

		if(lpC->state()!=KviMdiChild::Minimized)
		{
			if((curWin%numToHandle)==0)
			{
				lpC->move(curX, curY);
				lpC->resize(xQuantum * lastwindw[numToHandle-1], yQuantum);
			} else {
				lpC->move(curX, curY);
				lpC->resize(xQuantum, yQuantum);
			}
			//example : 12 windows : 3 cols 3 rows
			if (curCol < pColstable[numToHandle-1])
			{ //curCol < 3
				curX += xQuantum; //add a column in the same row
				curCol++;       //increase current column
			} else {
				curX = 0;         //new row
				curCol = 1;       //column 1
				if (curRow < pRowstable[numToHandle-1])
				{ //curRow < 3
					curY += yQuantum; //add a row
					curRow++;
				} else {
					curY = 0;         //restart from beginning
					curRow = 1;
				}
			}
			curWin++;
		}
	}
	if(lpTop)lpTop->setFocus();
	updateContentsSize();
}

void KviMdiManager::tileAnodine()
{
	this->tileSubWindows();
}
