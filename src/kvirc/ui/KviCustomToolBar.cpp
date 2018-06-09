//=============================================================================
//
//   File : KviCustomToolBar.cpp
//   Creation date : Sun 21 Nov 2004 05:28:57 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviCustomToolBar.h"
#include "KviMainWindow.h"
#include "KviLocale.h"
#include "KviApplication.h"
#include "KviActionManager.h"
#include "KviCustomToolBarDescriptor.h"

#include <QCursor>
#include <QDrag>
#include <QLayout>
#include <QPixmap>
#include <QPainter>
#include <QStyle>
#include <QEvent>
#include <QStyleOption>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QMenu>

static QAction * g_pDraggedAction = nullptr;

KviCustomToolBar::KviCustomToolBar(KviCustomToolBarDescriptor * pDesc, const QString & szLabel, Qt::ToolBarArea type, const char * pcName)
    : KviToolBar(szLabel, type, pcName)
{
	m_pDescriptor = pDesc;
	m_pFilteredChildren = nullptr;
	setAcceptDrops(true);
	// if the user removes all the items from this toolbar, keep a minimum size to permit dropping new item
	setMinimumSize(16, 16);

	connect(KviActionManager::instance(), SIGNAL(beginCustomizeToolBars()), this, SLOT(beginCustomize()));
	connect(KviActionManager::instance(), SIGNAL(endCustomizeToolBars()), this, SLOT(endCustomize()));

	pDesc->registerToolBar(this);
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
			KviActionManager::instance()->setCurrentToolBar(nullptr);
	}

	if(m_pFilteredChildren)
		delete m_pFilteredChildren;
}

void KviCustomToolBar::paintEvent(QPaintEvent * e)
{
	KviToolBar::paintEvent(e);

	if(KviActionManager::customizingToolBars() && (KviActionManager::currentToolBar() == this))
	{
		QPainter * p = new QPainter(this);
		QPalette pal = palette();
		QColor col = pal.highlight().color();
		col.setAlpha(127);
		p->fillRect(rect(), QBrush(col));
		delete p;
	}
}

void KviCustomToolBar::filteredChildDestroyed()
{
	if(!m_pFilteredChildren)
		return;
	const QObject * o = sender();
	m_pFilteredChildren->remove((void *)o);
}

void KviCustomToolBar::filterChild(QObject * o)
{
	if(m_pFilteredChildren)
		m_pFilteredChildren->insert(o, new bool(((QWidget *)o)->isEnabled()));

	o->installEventFilter(this);
	connect(o, SIGNAL(destroyed()), this, SLOT(filteredChildDestroyed()));
}

void KviCustomToolBar::unfilterChild(QObject * o)
{
	if(m_pFilteredChildren)
	{
		bool * pBool = m_pFilteredChildren->find(o);
		if(pBool)
		{
			o->removeEventFilter(this);
			disconnect(o, SIGNAL(destroyed()), this, SLOT(filteredChildDestroyed()));
		}
	}
}

void KviCustomToolBar::beginCustomize()
{
	if(m_pFilteredChildren)
		delete m_pFilteredChildren;

	m_pFilteredChildren = new KviPointerHashTable<void *, bool>;
	m_pFilteredChildren->setAutoDelete(true);
	// filter the events for all the children
	QList<QObject *> list = children();
	for(auto & it : list)
	{
		if(it->isWidgetType())
			filterChild(it);
	}
}

void KviCustomToolBar::endCustomize()
{
	// stop filtering events
	QList<QObject *> list = children();
	for(auto & it : list)
	{
		if(it->isWidgetType())
			unfilterChild(it);
	}

	if(m_pFilteredChildren)
	{
		delete m_pFilteredChildren;
		m_pFilteredChildren = nullptr;
	}
	syncDescriptor();
}

void KviCustomToolBar::syncDescriptor()
{
	// store the item order in the descriptor
	// There was boxLayouts
	m_pDescriptor->actions()->clear();
	foreach(QAction * pAction, actions())
	{
		m_pDescriptor->actions()->append(new QString(pAction->objectName()));
	}
}

void KviCustomToolBar::childEvent(QChildEvent * e)
{
	if(KviActionManager::customizingToolBars())
	{
		// this is useful for dropped and dragged-out children
		if(e->type() == QEvent::ChildAdded)
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

void KviCustomToolBar::dragEnterEvent(QDragEnterEvent * e)
{
	e->ignore();

	if(!KviActionManager::customizingToolBars())
		return;

	KviActionManager::instance()->setCurrentToolBar(this);

	if(e->mimeData()->hasText())
	{
		if(e->mimeData()->text().isEmpty())
			return;
		if(g_pDraggedAction && !g_pDraggedAction->objectName().compare(e->mimeData()->text()))
		{
			// moving a qaction from a toolbar to another
			e->acceptProposedAction();
		}
		else
		{
			// moving a kviaction from the toolbar editor to a toolbar
			KviAction * pAction = KviActionManager::instance()->getAction(e->mimeData()->text());
			if(pAction)
			{
				g_pDraggedAction = pAction->addToCustomToolBar(this);
				e->acceptProposedAction();
			}
		}
	}
}

void KviCustomToolBar::dragMoveEvent(QDragMoveEvent * e)
{
	if(!g_pDraggedAction)
		return;

	QAction * pActionToMove = actionAt(e->pos());
	if(pActionToMove == g_pDraggedAction)
		return; // hmmm

	if(actions().contains(g_pDraggedAction))
		removeAction(g_pDraggedAction);
	insertAction(pActionToMove, g_pDraggedAction);
}

void KviCustomToolBar::dragLeaveEvent(QDragLeaveEvent *)
{
	if(!g_pDraggedAction)
		return;

	if(actions().contains(g_pDraggedAction))
		removeAction(g_pDraggedAction);
}

void KviCustomToolBar::dropEvent(QDropEvent * e)
{
	if(!g_pDraggedAction || !e->mimeData()->hasText())
		return;

	if(!g_pDraggedAction->objectName().compare(e->mimeData()->text()))
	{
		g_pDraggedAction = nullptr;
		e->acceptProposedAction();
	}
}

QAction * KviCustomToolBar::actionForWidget(QWidget * pWidget)
{
	return actionAt(pWidget->x() + 1, pWidget->y() + 1);
}

bool KviCustomToolBar::eventFilter(QObject * o, QEvent * e)
{
	if(!KviActionManager::customizingToolBars())
		return KviToolBar::eventFilter(o, e); // anything here is done when customizing only

	if(e->type() == QEvent::MouseButtonPress)
	{
		KviActionManager::instance()->setCurrentToolBar(this);
		QMouseEvent * pEvent = (QMouseEvent *)e;
		if(pEvent->button() & Qt::LeftButton)
		{
			if(o->isWidgetType())
			{
				if(
				    o->inherits("KviToolBarGraphicalApplet") || o->inherits("QToolButton"))
				{
					QWidget * pMovedWidget = (QWidget *)o;
					g_pDraggedAction = actionForWidget(pMovedWidget);
					if(!g_pDraggedAction)
						return KviToolBar::eventFilter(o, e);
					// allow resizing of children

					if(o->inherits("KviToolBarGraphicalApplet") && pMovedWidget->width() > 20) // might be an applet
					{
						if(pEvent->pos().x() > (pMovedWidget->width() - 4))
						{
							g_pDraggedAction = nullptr;
							return KviToolBar::eventFilter(o, e); // let the applet handle the event it
						}
					}

					// drag out!
					QDrag * pDrag = new QDrag(this);
					QMimeData * pMime = new QMimeData();
					pMime->setText(g_pDraggedAction->objectName());
					pDrag->setMimeData(pMime);
					KviAction * act = KviActionManager::instance()->getAction(g_pDraggedAction->objectName());
					if(act)
					{
						QPixmap * pixie = act->bigIcon();
						if(pixie)
						{
							pDrag->setPixmap(*pixie);
							pDrag->setHotSpot(QPoint(3, 3));
						}
					}

					// search for the action after the moved one: we'll need it
					// to reinsert m_pMovedChild in the right position if necessary
					QAction * pAfterAction = nullptr;
					bool found = false;
					foreach(QAction * pTmp, actions())
					{
						if(found)
						{
							pAfterAction = pTmp;
							break;
						}

						if(pTmp == g_pDraggedAction)
							found = true;
					}

					removeAction(g_pDraggedAction);

					QEvent ev(QEvent::LayoutRequest);
					QApplication::sendEvent(this, &ev);
					if(pDrag->exec(Qt::MoveAction) != Qt::MoveAction)
					{
						// the user has probably failed to remove the action from the toolbar
						// flash the trashcan in the customize toolbars dialog
						KviActionManager::instance()->emitRemoveActionsHintRequest();
						insertAction(pAfterAction, g_pDraggedAction);
					}
					g_pDraggedAction = nullptr;
					return true;
				}
			}
		}
	}
	return KviToolBar::eventFilter(o, e);
}

KviCustomToolBarSeparator::KviCustomToolBarSeparator(KviCustomToolBar * pParent, const char * pcName)
    : QWidget(pParent)
{
	setObjectName(pcName);
	m_pToolBar = pParent;
	setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
}

QSize KviCustomToolBarSeparator::sizeHint() const
{
	QStyleOption opt;
	opt.initFrom(this);
	int iExtent = style()->pixelMetric(QStyle::PM_ToolBarSeparatorExtent, &opt, this);

	if(m_pToolBar->orientation() == Qt::Horizontal)
		return { iExtent, 0 };
	else
		return QSize(0, iExtent);
}

void KviCustomToolBarSeparator::paintEvent(QPaintEvent *)
{
	QPainter p(this);
	QStyleOption opt;
	opt.initFrom(this);
	if(m_pToolBar->orientation() == Qt::Horizontal)
		opt.state |= QStyle::State_Horizontal;
	style()->drawPrimitive(QStyle::PE_IndicatorToolBarSeparator, &opt, &p, this);
}
