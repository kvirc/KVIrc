//=============================================================================
//
//   File : kvi_customtoolbar.cpp
//   Created on Sun 21 Nov 2004 05:28:57 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2004 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_customtoolbar.h"
#include "kvi_frame.h"
#include "kvi_locale.h"
#include "kvi_app.h"
#include "kvi_actionmanager.h"
#include "kvi_customtoolbardescriptor.h"

#include <qcursor.h>
#include "kvi_tal_popupmenu.h"
#include <qlayout.h>
#include <qpixmap.h>
#include <qcursor.h>
#include <qtoolbutton.h>
#include <qpainter.h>
#include <qstyle.h>

#ifdef COMPILE_USE_QT4
	#include <qevent.h>
	#include <q3dragobject.h>
	
	#define QDragObject Q3DragObject
	#define QTextDrag Q3TextDrag
	#define QIconDrag Q3IconDrag
	
	#include <qstyleoption.h>
#else
	#include <qobjectlist.h>
	#include <qdragobject.h>
#endif



KviCustomToolBarSeparator::KviCustomToolBarSeparator(KviCustomToolBar *pParent,const char * name)
: QWidget(pParent,name)
{
	m_pToolBar = pParent;
	setBackgroundMode(pParent->backgroundMode());
	setBackgroundOrigin(ParentOrigin);
	setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));
}

QSize KviCustomToolBarSeparator::sizeHint() const
{
#ifdef COMPILE_USE_QT4
	QStyleOption opt;
	opt.initFrom(this);
	int extent = style()->pixelMetric(QStyle::PM_ToolBarSeparatorExtent,&opt,this);
#else
	int extent = style().pixelMetric(QStyle::PM_DockWindowSeparatorExtent,this);
#endif
	if(m_pToolBar->orientation() == Qt::Horizontal)return QSize(extent,0);
	else return QSize(0,extent);
}

void KviCustomToolBarSeparator::paintEvent(QPaintEvent *)
{
	QPainter p(this);
#ifdef COMPILE_USE_QT4
	QStyleOption opt;
	opt.initFrom(this);
	style()->drawPrimitive(QStyle::PE_Q3DockWindowSeparator,&opt,&p,this);
#else
	QStyle::SFlags flags = QStyle::Style_Default;
	if(m_pToolBar->orientation() == Qt::Horizontal)flags |= QStyle::Style_Horizontal;
	style().drawPrimitive(QStyle::PE_DockWindowSeparator,&p,rect(),colorGroup(),flags);
#endif
}


KviCustomToolBar::KviCustomToolBar(KviCustomToolBarDescriptor * d,const QString &label,QT_TOOLBARDOCK_TYPE dock,bool bNewLine,const char * nam)
: KviToolBar(label,dock,bNewLine,nam)
{
	m_pDescriptor = d;
	m_pMovedChild = 0;
	m_pDraggedChild = 0;
	m_pFilteredChildren = 0;
	setAcceptDrops(true);
	connect(KviActionManager::instance(),SIGNAL(beginCustomizeToolBars()),this,SLOT(beginCustomize()));
	connect(KviActionManager::instance(),SIGNAL(endCustomizeToolBars()),this,SLOT(endCustomize()));
	setMinimumSize(20,20);
	d->registerToolBar(this);
	if(KviActionManager::customizingToolBars())
		beginCustomize(); // because we will not get the signal
}

KviCustomToolBar::~KviCustomToolBar()
{
	if(KviActionManager::customizingToolBars())
		syncDescriptor(); // because we will not get endCustomize()
	m_pDescriptor->unregisterToolBar(this);
	if(KviActionManager::customizingToolBars())
	{
		if(KviActionManager::currentToolBar() == this)
			KviActionManager::instance()->setCurrentToolBar(0);
	}
	if(m_pFilteredChildren)delete m_pFilteredChildren;
}

void KviCustomToolBar::paintEvent(QPaintEvent * e)
{
	KviToolBar::paintEvent(e);
	if(KviActionManager::customizingToolBars() && (KviActionManager::currentToolBar() == this))
	{
		QPainter p(this);
		p.setPen(Qt::red);
		p.drawRect(0,0,width(),height());
	}
}

void KviCustomToolBar::filteredChildDestroyed()
{
	if(!m_pFilteredChildren)return;
	const QObject * o = sender();
	m_pFilteredChildren->remove((void *)o);
}

void KviCustomToolBar::filterChild(QObject * o)
{
	bool * b = new bool(((QWidget *)o)->isEnabled());
	if(m_pFilteredChildren)
		m_pFilteredChildren->insert(o,b);
	if(!*b)((QWidget *)o)->setEnabled(true);
	o->installEventFilter(this);
	connect(o,SIGNAL(destroyed()),this,SLOT(filteredChildDestroyed()));
}

void KviCustomToolBar::unfilterChild(QObject * o)
{
	if(m_pFilteredChildren)
	{
		bool * b = m_pFilteredChildren->find(o);
		if(b)
		{
			if(!*b)((QWidget *)o)->setEnabled(false);
			o->removeEventFilter(this);
			disconnect(o,SIGNAL(destroyed()),this,SLOT(filteredChildDestroyed()));
		}
	}
}

void KviCustomToolBar::beginCustomize()
{
	QString szClassName;
	if(m_pFilteredChildren)delete m_pFilteredChildren;
	m_pFilteredChildren = new KviPointerHashTable<void *,bool>;
	m_pFilteredChildren->setAutoDelete(true);
	// filter the events for all the children
#ifdef COMPILE_USE_QT4
	QList<QObject*> l = children();
	for(QList<QObject*>::Iterator it = l.begin();it != l.end();++it)
	{
		if((*it)->isWidgetType())
		{
			szClassName = (*it)->className();
			//we need to filter only some classes: eg. we don't want the QTitleBar of the QToolbar to be removeable (see bug report #210)
			if(szClassName.compare("KviStyledToolButton") == 0 || szClassName.compare("KviCustomToolBarSeparator") == 0 )
				filterChild(*it);
		}
	}
#else
	const QObjectList * l = children();
	QObjectListIterator it(*l);
	while(QObject * o = it.current())
	{
		if(o->isWidgetType())
		{
			szClassName = o->className();
			//we need to filter only some classes: eg. we don't want the QTitleBar of the QToolbar to be removeable (see bug report #210)
			if(szClassName.compare("KviStyledToolButton") == 0 || szClassName.compare("KviCustomToolBarSeparator") == 0 )
				filterChild(o);
		}
		++it;
	}
#endif
}

void KviCustomToolBar::endCustomize()
{
	// stop filtering events
#ifdef COMPILE_USE_QT4
	QList<QObject*> l = children();
	for(QList<QObject*>::Iterator it = l.begin();it != l.end();++it)
	{
		if((*it)->isWidgetType())
			unfilterChild(*it);
	}
#else
	const QObjectList * l = children();
	QObjectListIterator it(*l);
	while(QObject * o = it.current())
	{
		if(o->isWidgetType())
			unfilterChild(o);
		++it;
	}
#endif
	// FIXME: We SHOULD MAKE SURE that the children are re-enabled...
	// this could be done by calling setEnabled(isEnabled()) on each action ?
	if(m_pFilteredChildren)
	{
		delete m_pFilteredChildren;
		m_pFilteredChildren = 0;
	}
	syncDescriptor();
}

void KviCustomToolBar::syncDescriptor()
{
	// store the item order in the descriptor
	QBoxLayout * lay = boxLayout();
	QLayoutIterator iter = lay->iterator();
	QLayoutItem * i;
	m_pDescriptor->actions()->clear();
	while((i = iter.current()))
	{
		if(QWidget * w = i->widget())
			m_pDescriptor->actions()->append(new QString(w->name()));
		++iter;
	}
}

void KviCustomToolBar::childEvent(QChildEvent *e)
{
	if(KviActionManager::customizingToolBars())
	{
		// this is useful for droppped and dragged-out children
		if(e->type() == QEvent::ChildInserted)
		{
			if(e->child()->isWidgetType())
				filterChild(e->child());
			goto done;
		}
		
		if(e->type() == QEvent::ChildRemoved)
		{
			if(e->child()->isWidgetType())
				unfilterChild(e->child());
			goto done;
		}
	}
done:
	KviToolBar::childEvent(e);
}

void KviCustomToolBar::dragEnterEvent(QDragEnterEvent *e)
{
	if(!KviActionManager::customizingToolBars())return;
	KviActionManager::instance()->setCurrentToolBar(this);
	QString text;
	if(QTextDrag::decode(e,text))
	{
		if(!text.isEmpty())
		{
			KviAction * a = KviActionManager::instance()->getAction(text);
			if(a)
			{
				e->accept(true);
				int idx = dropIndexAt(mapFromGlobal(QCursor::pos()),0,0);
				m_pDraggedChild = a->addToCustomToolBar(this);
#ifdef COMPILE_USE_QT4
				QWidget * pWidgetToMove = widgetAt(idx);
				bool bDone = false;
				QAction * a;
				if(pWidgetToMove)
				{
					a = actionForWidget(pWidgetToMove);
					if(a)
					{
						bDone = true;
						a = insertWidget(a,m_pDraggedChild);
					}
				}
				if(!bDone)
					a = addWidget(m_pDraggedChild);
				a->setVisible(true);
#else
				boxLayout()->remove(m_pDraggedChild); // in case it was already added
				boxLayout()->insertWidget(idx,m_pDraggedChild);
#ifdef COMPILE_KDE_SUPPORT
				// bleah ://///
				insertWidget(-1,m_pDraggedChild->sizeHint().width(),m_pDraggedChild,idx);
#endif
#endif
				QEvent ev(QEvent::LayoutHint);
				QApplication::sendEvent(this,&ev);
			} else e->accept(false);
		} else e->accept(false);
	} else e->accept(false);
}

void KviCustomToolBar::dragMoveEvent(QDragMoveEvent *e)
{
	if(!m_pDraggedChild)return;
	drag(m_pDraggedChild,mapFromGlobal(QCursor::pos()));
}

void KviCustomToolBar::dragLeaveEvent(QDragLeaveEvent *e)
{
	if(m_pDraggedChild)
	{
		if(m_pFilteredChildren)
			m_pFilteredChildren->remove(m_pDraggedChild); // just to be sure
		delete m_pDraggedChild;
		m_pDraggedChild = 0;
	}
}

void KviCustomToolBar::dropEvent(QDropEvent *e)
{
	if(!m_pDraggedChild)return;
	m_pDraggedChild = 0;
	e->accept();
	// nuthin :)
}

int KviCustomToolBar::dropIndexAt(const QPoint &pnt,QWidget * exclude,int * excludeIdx)
{
	// find the widget at the current poisition
	// treating exclude as if it was going to be removed
	// find also the exclude index if needed
#ifdef COMPILE_USE_QT4
	QLayout * l = layout();
#else
	QBoxLayout * l = boxLayout();
#endif
	QLayoutItem * i = 0;
	if(excludeIdx)*excludeIdx = -1;
	int idx = 0;

	if(!l)return 0;
	QLayoutIterator it = l->iterator();
	
	// find the children with minimum distance
	int iMinDistIdx = -1;
	QWidget * pMinDistW = 0;
	unsigned int uMinDist = 0xffffffff;
	int iExcludeIdx = -1;
	QPoint pntExclude;
	QWidget * w = 0;

	while((i = it.current()))
	{
		if((w = i->widget()))
		{
			if(uMinDist != 0)
			{
				int iRight = w->x() + w->width();
				int iBottom = w->y() + w->height();
				if((pnt.x() >= w->x()) && (pnt.y() >= w->y()) && (pnt.x() <= iRight) && (pnt.y() <= iBottom))
				{
					// inside the widget
					// distance 0
					pMinDistW = w;
					iMinDistIdx = idx;
					uMinDist = 0;
				} else {
					// outside the widget
					// compute the distance
					unsigned int uXDist = (pnt.x() < w->x()) ? (w->x() - pnt.x()) : (pnt.x() > iRight ? (pnt.x() - iRight) : 0);
					unsigned int uYDist = (pnt.y() < w->y()) ? (w->y() - pnt.y()) : (pnt.y() > iBottom ? (pnt.y() - iBottom) : 0);
					if((uXDist < 8192) && (uYDist < 8192))
					{
						// it is in reasonable rect
						unsigned int uDist = (uXDist * uXDist) + (uYDist * uYDist);
						if(uDist < uMinDist)
						{
							// min distance for now
							uMinDist = uDist;
							pMinDistW = w;
							iMinDistIdx = idx;
						}
					} // else the user has a really HUUUGE screen
				}
			} // else the minimum distance widget has already been found
			if(w == exclude)
			{
				iExcludeIdx = idx;
				pntExclude = w->pos();
			}
		}
		idx++;
		++it;
	}
	
	if(!pMinDistW)
	{
		// ops.. not found at all (empty toolbar or really far from any button)
		if(orientation() == Qt::Horizontal)
		{
			if(pnt.x() < (width() / 2))iMinDistIdx = 0; // insert at position 0
			else iMinDistIdx = idx;
			// else insert at the last position found
		} else {
			if(pnt.y() < (height() / 2))iMinDistIdx = 0; // insert at position 0
			else iMinDistIdx = idx;
			// else insert at the last position found
		}
	} else {
		// got it, check for the exclude idx
		if((iExcludeIdx == -1) || (iExcludeIdx != iMinDistIdx))
		{
			// would not put it over exclude idx
			// check if we have to stay on right or left of the widget found
			if(orientation() == Qt::Horizontal)
			{
				if(pnt.x() > (pMinDistW->x() + (pMinDistW->width() / 2)))
					iMinDistIdx++; // need to put it on the right
			} else {
				if(pnt.y() > (pMinDistW->y() + (pMinDistW->height() / 2)))
					iMinDistIdx++; // need to put it below
			}
		}
		
		// ok , check again (we might have moved exactly over exclude idx now!)
		if((iExcludeIdx != -1) && (iExcludeIdx != iMinDistIdx))
		{
			// got the exclude idx by the way and wouldn't put exactly over it
			// check if exclude idx is "before" the current possible insert position
			// if it is , then lower down the index by one
			if(orientation() == Qt::Horizontal)
			{
				if(pnt.x() > pntExclude.x())
					iMinDistIdx--; // removing exclude will move everything one step back
			} else {
				if(pnt.y() > pntExclude.y())
					iMinDistIdx--; // removing exclude will move everything one step back
			}
		}
	}

	if(iMinDistIdx < 0)iMinDistIdx = 0;
	if(excludeIdx)*excludeIdx = iExcludeIdx;

	return iMinDistIdx;

}

#ifdef COMPILE_USE_QT4
QWidget * KviCustomToolBar::widgetAt(int index)
{
	QLayout * l = layout();
	if(!l)
		return NULL;
	QLayoutItem * it = l->itemAt(index);
	if(!it)
		return NULL;
	return it->widget();
}

QAction * KviCustomToolBar::actionForWidget(QWidget * pWidget)
{
	return actionAt(pWidget->x() + 1,pWidget->y() + 1);
}

#endif

void KviCustomToolBar::drag(QWidget * child,const QPoint &pnt)
{
	int me = -1;
	int idx = dropIndexAt(pnt,child,&me);
	debug("DROP INDEX IS %d, ME IS %d",idx,me);
	if(idx == me)
		return; // would move over itself
#ifdef COMPILE_USE_QT4
	QWidget * pWidgetToMove = widgetAt(idx > me ? idx-1 : idx);
	debug("SEARCHING FOR WIDGET TO MOVE AT %d AND FOUND %x (ME=%x)",idx > me ? idx-1 : idx,pWidgetToMove,child);
	if(pWidgetToMove == child)
		return; // hmmm
	bool bDone = false;
	QAction * pMyOwnAction = actionForWidget(child);
	if(!pMyOwnAction)
		return;
	QAction * a;
	removeAction(pMyOwnAction);
	if(pWidgetToMove)
	{
		a = actionForWidget(pWidgetToMove);
		if(a)
		{
			debug("AND GOT ACTION FOR THAT WIDGET");

			bDone = true;
			a = insertWidget(a,child);
		}
	} else {
		addAction(a);
	}
	if(!bDone)
		a = addWidget(child);
	a->setVisible(true);
#else
	boxLayout()->remove(child);
	boxLayout()->insertWidget(idx,child);
#ifdef COMPILE_KDE_SUPPORT
	// bleah ://///
	insertWidget(-1,child->width(),child,idx);
#endif
#endif
	QEvent ev(QEvent::LayoutHint);
	QApplication::sendEvent(this,&ev);
}

void KviCustomToolBar::mousePressEvent(QMouseEvent * e)
{
	if(KviActionManager::customizingToolBars())
		KviActionManager::instance()->setCurrentToolBar(this);
	KviToolBar::mousePressEvent(e);
}

bool KviCustomToolBar::eventFilter(QObject *o,QEvent *e)
{
	if(!KviActionManager::customizingToolBars())goto unhandled; // anything here is done when customizing only
	if(e->type() == QEvent::Enter)
	{
		if(m_pMovedChild)return true; // kill it while moving other children
	}
	if(e->type() == QEvent::Leave)
	{
		if(m_pMovedChild)return true; // kill it while moving other children
	}
	if(e->type() == QEvent::MouseButtonPress)
	{
		KviActionManager::instance()->setCurrentToolBar(this);
		QMouseEvent * ev = (QMouseEvent *)e;
		if(ev->button() & Qt::LeftButton)
		{
			if(o->isWidgetType())
			{
				if(!(
					o->inherits("KviTalPopupMenu") ||
					o->inherits("QToolBarHandle") ||
					o->inherits("QDockWindowHandle") ||
					o->inherits("QDockWindowResizeHandle") ||
					o->inherits("QToolBarExtensionWidget")
					))
				{
					m_pMovedChild = (QWidget *)o;
					// allow resizing of children
					// FIXME: do it only if the child is really resizable
					if(m_pMovedChild->width() > 20) // might be an applet
					{
						if(ev->pos().x() > (m_pMovedChild->width() - 4))
						{
							m_pMovedChild = 0;
							goto unhandled; // let the applet handle the event it
						}
					}
					g_pApp->setOverrideCursor(Qt::sizeAllCursor);
					return true;
				}
			}
		}
		goto unhandled;
	}
	if(e->type() == QEvent::MouseButtonRelease)
	{
		if(m_pMovedChild)
		{
			g_pApp->restoreOverrideCursor();
			m_pMovedChild = 0;
			return true;
		}
		goto unhandled;
	}
	if(e->type() == QEvent::MouseMove)
	{
		if(m_pMovedChild)
		{
			QMouseEvent * ev = (QMouseEvent *)e;

			QPoint pnt = mapFromGlobal(m_pMovedChild->mapToGlobal(ev->pos()));
			if((pnt.y() < 0) || (pnt.y() > height()) || (pnt.x() < 0) || (pnt.x() > width()))
			{
				// drag out!
// FIXME: This is screwed up in Qt4.... :/
#ifdef COMPILE_USE_QT4
				QDrag * d = new QDrag(this);
				QMimeData * m = new QMimeData();
				m->setText(m_pMovedChild->name());
				d->setMimeData(m);
#else
				QDragObject * d = new QTextDrag(m_pMovedChild->name(),this);
#endif
				KviAction * act = KviActionManager::instance()->getAction(m_pMovedChild->name());
				if(act)
				{
					QPixmap * pixie = act->bigIcon();
#ifdef COMPILE_USE_QT4
					if(pixie)
					{
						d->setPixmap(*pixie);
						d->setHotSpot(QPoint(3,3));
					}
#else
					if(pixie)d->setPixmap(*pixie,QPoint(3,3));
#endif
				}
				//d->setPixmap(QPixmap::grabWidget(m_pMovedChild),QPoint(m_pMovedChild->width() / 2,m_pMovedChild->height() / 2));
				// throw it somewhere else for now
				if(m_pFilteredChildren)
					unfilterChild(m_pMovedChild);
#ifdef COMPILE_USE_QT4
				QAction * pActionForMovedChild = actionForWidget(m_pMovedChild);
				if(pActionForMovedChild)
					pActionForMovedChild->setVisible(false);
				m_pMovedChild->hide();
#else
				m_pMovedChild->hide();
				m_pMovedChild->reparent(g_pFrame,QPoint(-1000,-1000),false);
#endif
				QEvent ev(QEvent::LayoutHint);
				QApplication::sendEvent(this,&ev);
#ifdef COMPILE_USE_QT4
				if(!d->exec(Qt::MoveAction) != Qt::MoveAction)
#else
				if(!d->dragMove())
#endif
				{
					// the user has probably failed to remove the action from the toolbar
					// flash the trashcan in the customize toolbars dialog
					KviActionManager::instance()->emitRemoveActionsHintRequest();
					// will filter it as ChildInserted
#ifndef COMPILE_USE_QT4
					m_pMovedChild->reparent(this,QPoint(0,0),false);
#endif
#ifdef COMPILE_USE_QT4
					QAction * pActionForMovedChild = actionForWidget(m_pMovedChild);
					if(pActionForMovedChild)
						pActionForMovedChild->setVisible(false);
#else
					boxLayout()->insertWidget(0,m_pMovedChild);
					m_pMovedChild->show();
#ifdef COMPILE_KDE_SUPPORT
					// bleah ://///
					insertWidget(-1,m_pMovedChild->width(),m_pMovedChild,0);
#endif
#endif
					QEvent ev(QEvent::LayoutHint);
					QApplication::sendEvent(this,&ev);
				} else {
					QApplication::sendPostedEvents(m_pMovedChild,0);
					m_pMovedChild->deleteLater();
					m_pMovedChild = 0;
				}
				return true;
			}

			drag(m_pMovedChild,pnt);
			return true;
		}
		goto unhandled;
	}
unhandled:
	return KviToolBar::eventFilter(o,e);
}
