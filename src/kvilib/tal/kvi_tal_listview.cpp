//=============================================================================
//
//   File : kvi_tal_listview.cpp
//   Creation date : Mon May 05 2008 11:25:08 by Alessandro Carbone
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2008 Alessandro Carbone (elfonol at gmail dot com)
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


#include "kvi_tal_listview.h"
#include "kvi_pointerhashtable.h"

#include <qpainter.h>
#include <qstyle.h>
#include <q3header.h>
#include <qapplication.h>
#include <qhash.h>
#include <qstyleoption.h>
#include <qaccessible.h>

static QStyleOptionQ3ListView getStyleOption(const Q3ListView *lv, const Q3ListViewItem *item)
{
	QStyleOptionQ3ListView opt;
	opt.init(lv);
	opt.subControls = QStyle::SC_None;
	opt.activeSubControls = QStyle::SC_None;
	QWidget *vp = lv->viewport();
	opt.viewportPalette = vp->palette();
	opt.viewportBGRole = vp->backgroundRole();
	opt.itemMargin = lv->itemMargin();
	opt.sortColumn = 0;
	opt.treeStepSize = lv->treeStepSize();
	opt.rootIsDecorated = lv->rootIsDecorated();
	bool firstItem = true;
	while (item) {
		QStyleOptionQ3ListViewItem lvi;
		lvi.height = item->height();
		lvi.totalHeight = item->totalHeight();
		lvi.itemY = item->itemPos();
		lvi.childCount = item->childCount();
		lvi.features = QStyleOptionQ3ListViewItem::None;
		lvi.state = QStyle::State_None;
		if (item->isEnabled())
			lvi.state |= QStyle::State_Enabled;
		if (item->isOpen())
			lvi.state |= QStyle::State_Open;
		if (item->isExpandable())
			lvi.features |= QStyleOptionQ3ListViewItem::Expandable;
		if (item->multiLinesEnabled())
			lvi.features |= QStyleOptionQ3ListViewItem::MultiLine;
		if (item->isVisible())
			lvi.features |= QStyleOptionQ3ListViewItem::Visible;
		if (item->parent() && item->parent()->rtti() == 1
			&& static_cast<Q3CheckListItem *>(item->parent())->type() == Q3CheckListItem::Controller)
			lvi.features |= QStyleOptionQ3ListViewItem::ParentControl;
		opt.items.append(lvi);
		if (!firstItem) {
			item = item->nextSibling();
		} else {
			firstItem = false;
			item = item->firstChild();
		}
	}
	return opt;
}

KviTalListView::KviTalListView(QWidget * pParent)
: Q3ListView(pParent)
{
	connect(this,SIGNAL(selectionChanged(Q3ListViewItem *)),this,SLOT(redirect_selectionChanged(Q3ListViewItem *)));
	connect(this,SIGNAL(currentChanged(Q3ListViewItem *)),this,SLOT(redirect_currentChanged(Q3ListViewItem *)));
	connect(this,SIGNAL(clicked(Q3ListViewItem *)),this,SLOT(redirect_clicked(Q3ListViewItem *)));
	connect(this,SIGNAL(clicked(Q3ListViewItem *,const QPoint &,int)),this,SLOT(redirect_clicked(Q3ListViewItem *,const QPoint &,int)));
	connect(this,SIGNAL(pressed(Q3ListViewItem *)),this,SLOT(redirect_pressed(Q3ListViewItem *)));
	connect(this,SIGNAL(pressed(Q3ListViewItem *,const QPoint &,int)),this,SLOT(redirect_pressed(Q3ListViewItem *,const QPoint &,int)));
	connect(this,SIGNAL(doubleClicked(Q3ListViewItem *)),this,SLOT(redirect_doubleClicked(Q3ListViewItem *)));
	connect(this,SIGNAL(doubleClicked(Q3ListViewItem *,const QPoint &,int)),this,SLOT(redirect_doubleClicked(Q3ListViewItem *,const QPoint &,int)));
	connect(this,SIGNAL(returnPressed(Q3ListViewItem *)),this,SLOT(redirect_returnPressed(Q3ListViewItem *)));
	connect(this,SIGNAL(spacePressed(Q3ListViewItem *)),this,SLOT(redirect_spacePressed(Q3ListViewItem *)));
	connect(this,SIGNAL(rightButtonClicked(Q3ListViewItem *,const QPoint &,int)),this,SLOT(redirect_rightButtonClicked(Q3ListViewItem *,const QPoint &,int)));
	connect(this,SIGNAL(rightButtonPressed(Q3ListViewItem *,const QPoint &,int)),this,SLOT(redirect_rightButtonPressed(Q3ListViewItem *,const QPoint &,int)));
	connect(this,SIGNAL(mouseButtonClicked(int,Q3ListViewItem *,const QPoint &,int)),this,SLOT(redirect_mouseButtonClicked(int,Q3ListViewItem *,const QPoint &,int)));
	connect(this,SIGNAL(mouseButtonPressed(int,Q3ListViewItem *,const QPoint &,int)),this,SLOT(redirect_mouseButtonPressed(int,Q3ListViewItem *,const QPoint &,int)));
	connect(this,SIGNAL(contextMenuRequested(Q3ListViewItem *,const QPoint &,int)),this,SLOT(redirect_contextMenuRequested(Q3ListViewItem *,const QPoint &,int)));
	connect(this,SIGNAL(onItem(Q3ListViewItem *)),this,SLOT(redirect_onItem(Q3ListViewItem *)));
	connect(this,SIGNAL(expanded(Q3ListViewItem *)),this,SLOT(redirect_expanded(Q3ListViewItem *)));
	connect(this,SIGNAL(collapsed(Q3ListViewItem *)),this,SLOT(redirect_collapsed(Q3ListViewItem *)));
}

void KviTalListView::redirect_selectionChanged(Q3ListViewItem * pItem)
{
	emit selectionChanged((KviTalListViewItem *)pItem);
}

void KviTalListView::redirect_currentChanged(Q3ListViewItem * pItem)
{
	emit currentChanged((KviTalListViewItem *)pItem);
}

void KviTalListView::redirect_clicked(Q3ListViewItem * pItem)
{
	emit clicked((KviTalListViewItem *)pItem);
}

void KviTalListView::redirect_clicked(Q3ListViewItem * pItem,const QPoint &pnt,int uColumn)
{
	emit clicked((KviTalListViewItem *)pItem,pnt,uColumn);
}

void KviTalListView::redirect_pressed(Q3ListViewItem * pItem)
{
	emit pressed((KviTalListViewItem *)pItem);
}

void KviTalListView::redirect_pressed(Q3ListViewItem * pItem,const QPoint &pnt,int uColumn)
{
	emit pressed((KviTalListViewItem *)pItem,pnt,uColumn);
}

void KviTalListView::redirect_doubleClicked(Q3ListViewItem * pItem)
{
	emit doubleClicked((KviTalListViewItem *)pItem);
}

void KviTalListView::redirect_doubleClicked(Q3ListViewItem * pItem,const QPoint &pnt,int uColumn)
{
	emit doubleClicked((KviTalListViewItem *)pItem,pnt,uColumn);
}

void KviTalListView::redirect_returnPressed(Q3ListViewItem * pItem)
{
	emit returnPressed((KviTalListViewItem *)pItem);
}

void KviTalListView::redirect_spacePressed(Q3ListViewItem * pItem)
{
	emit spacePressed((KviTalListViewItem *)pItem);
}

void KviTalListView::redirect_rightButtonClicked(Q3ListViewItem * pItem,const QPoint &pnt,int uColumn)
{
	emit rightButtonClicked((KviTalListViewItem *)pItem,pnt,uColumn);
}

void KviTalListView::redirect_rightButtonPressed(Q3ListViewItem * pItem,const QPoint &pnt,int uColumn)
{
	emit rightButtonPressed((KviTalListViewItem *)pItem,pnt,uColumn);
}

void KviTalListView::redirect_mouseButtonClicked(int iButton,Q3ListViewItem * pItem,const QPoint &pnt,int uColumn)
{
	emit mouseButtonClicked(iButton,(KviTalListViewItem *)pItem,pnt,uColumn);
}

void KviTalListView::redirect_mouseButtonPressed(int iButton,Q3ListViewItem * pItem,const QPoint &pnt,int uColumn)
{
	emit mouseButtonPressed(iButton,(KviTalListViewItem *)pItem,pnt,uColumn);
}

void KviTalListView::redirect_contextMenuRequested(Q3ListViewItem * pItem,const QPoint &pnt,int uColumn)
{
	emit contextMenuRequested((KviTalListViewItem *)pItem,pnt,uColumn);
}

void KviTalListView::redirect_onItem(Q3ListViewItem * pItem)
{
	emit onItem((KviTalListViewItem *)pItem);
}

void KviTalListView::redirect_expanded(Q3ListViewItem * pItem)
{
	emit expanded((KviTalListViewItem *)pItem);
}

void KviTalListView::redirect_collapsed(Q3ListViewItem * pItem)
{
	emit collapsed((KviTalListViewItem *)pItem);
}


struct KviTalCheckListItemPrivate
{
	KviTalCheckListItemPrivate():
	exclusive(0),
	currentState(KviTalCheckListItem::Off),
	tristate(false) {}

	KviTalCheckListItem *exclusive;
	KviTalCheckListItem::ToggleState currentState;
	QHash<KviTalCheckListItem *, KviTalCheckListItem::ToggleState> statesDict;
	bool tristate;
};

KviTalCheckListItem::KviTalCheckListItem(KviTalCheckListItem *parent, const QString &text,Type tt)
: KviTalListViewItem(parent, text, QString())
{
	myType = tt;
	init();
	if (myType == RadioButton) {
		if (parent->type() != RadioButtonController)
			qWarning("KviTalCheckListItem::KviTalCheckListItem(), radio button must be "
				"child of a controller");
		else
			d->exclusive = parent;
	}
}

KviTalCheckListItem::KviTalCheckListItem(KviTalCheckListItem *parent, KviTalListViewItem *after,const QString &text, Type tt)
: KviTalListViewItem(parent, after, text)
{
	myType = tt;
	init();
	if (myType == RadioButton) {
		if (parent->type() != RadioButtonController)
			qWarning("KviTalCheckListItem::KviTalCheckListItem(), radio button must be "
				"child of a controller");
		else
			d->exclusive = parent;
	}
}

KviTalCheckListItem::KviTalCheckListItem(KviTalListViewItem *parent, const QString &text,Type tt)
: KviTalListViewItem(parent, text, QString())
{
	myType = tt;
	if (myType == RadioButton) {
		qWarning("KviTalCheckListItem::KviTalCheckListItem(), radio button must be "
			"child of a KviTalCheckListItem");
	}
	init();
}

KviTalCheckListItem::KviTalCheckListItem(KviTalListViewItem *parent, KviTalListViewItem *after,const QString &text, Type tt)
: KviTalListViewItem(parent, after, text)
{
	myType = tt;
	if (myType == RadioButton) {
		qWarning("KviTalCheckListItem::KviTalCheckListItem(), radio button must be "
			"child of a KviTalCheckListItem");
	}
	init();
}

KviTalCheckListItem::KviTalCheckListItem(KviTalListView *parent, const QString &text,Type tt)
: KviTalListViewItem(parent, text)
{
	myType = tt;
	if (tt == RadioButton){
		qWarning("KviTalCheckListItem::KviTalCheckListItem(), radio button must be "
			"child of a KviTalCheckListItem");
	}
	init();
}

KviTalCheckListItem::KviTalCheckListItem(KviTalListView *parent, KviTalListViewItem *after,const QString &text, Type tt)
: KviTalListViewItem(parent, after, text)
{
	myType = tt;
	if (tt == RadioButton){
		qWarning("KviTalCheckListItem::KviTalCheckListItem(), radio button must be "
			"child of a KviTalCheckListItem");
	}
	init();
}

int KviTalCheckListItem::rtti() const
{
	return RTTI;
}

KviTalCheckListItem::KviTalCheckListItem(KviTalListView *parent, const QString &text,const QPixmap & p)
: KviTalListViewItem(parent, text)
{
	myType = RadioButtonController;
	setPixmap(0, p);
	init();
}

KviTalCheckListItem::KviTalCheckListItem(KviTalListViewItem *parent, const QString &text,const QPixmap & p)
: KviTalListViewItem(parent, text)
{
	myType = RadioButtonController;
	setPixmap(0, p);
	init();
}

void KviTalCheckListItem::init()
{
	d = new KviTalCheckListItemPrivate();
	on = false; // ### remove on ver 4
	// CheckBoxControllers by default have tristate set to true
	if (myType == CheckBoxController)
	setTristate(true);
}

KviTalCheckListItem::~KviTalCheckListItem()
{
	if (myType == RadioButton
		&& d->exclusive && d->exclusive->d
		&& d->exclusive->d->exclusive == this)
	d->exclusive->turnOffChild();
	d->exclusive = 0; // so the children won't try to access us.
	delete d;
	d = 0;
}

void KviTalCheckListItem::setTristate(bool b)
{
	if ((myType != CheckBoxController) && (myType != CheckBox)) {
		qWarning("KviTalCheckListItem::setTristate(), has no effect on RadioButton "
			"or RadioButtonController.");
		return;
	}
	d->tristate = b;
}

bool KviTalCheckListItem::isTristate() const
{
	return d->tristate;
}

KviTalCheckListItem::ToggleState KviTalCheckListItem::state() const
{
	if (!isTristate() && internalState() == NoChange)
	return Off;
	else
	return d->currentState;
}

KviTalCheckListItem::ToggleState KviTalCheckListItem::internalState() const
{
	return d->currentState;
}

void KviTalCheckListItem::setState(ToggleState s)
{
	if (myType == CheckBoxController && state() == NoChange)
	updateStoredState(this);
	setState(s, true, true);
}

void KviTalCheckListItem::setState(ToggleState s, bool update, bool store)
{

	if (s == internalState())
	return;

	if (myType == CheckBox) {
	setCurrentState(s);
	stateChange(state());
	if (update && parent() && parent()->rtti() == 1
		&& ((KviTalCheckListItem*)parent())->type() == CheckBoxController)
		((KviTalCheckListItem*)parent())->updateController(update, store);
	} else if (myType == CheckBoxController) {
		if (s == NoChange && childCount()) {
			restoreState(this);
		} else {
			KviTalListViewItem *item = firstChild();
			int childCount = 0;
			while(item) {
				if (item->rtti() == 1 &&
					(((KviTalCheckListItem*)item)->type() == CheckBox ||
					((KviTalCheckListItem*)item)->type() == CheckBoxController)) {
						KviTalCheckListItem *checkItem = (KviTalCheckListItem*)item;
						checkItem->setState(s, false, false);
						childCount++;
				}
				item = item->nextSibling();
			}
			if (update) {
				if (childCount > 0) {
					ToggleState oldState = internalState();
					updateController(false, false);
					if (oldState != internalState() &&
						parent() && parent()->rtti() == 1 &&
						((KviTalCheckListItem*)parent())->type() == CheckBoxController)
					((KviTalCheckListItem*)parent())->updateController(update, store);

					updateController(update, store);
				} else {
					// if there are no children we simply set the CheckBoxController and update its parent
					setCurrentState(s);
					stateChange(state());
					if (parent() && parent()->rtti() == 1
						&& ((KviTalCheckListItem*)parent())->type() == CheckBoxController)
					((KviTalCheckListItem*)parent())->updateController(update, store);
				}
			} else {
				setCurrentState(s);
				stateChange(state());
			}
		}
	} else if (myType == RadioButton) {
		if (s == On) {
			if (d->exclusive && d->exclusive->d->exclusive != this)
			d->exclusive->turnOffChild();
			setCurrentState(s);
			if (d->exclusive)
			d->exclusive->d->exclusive = this;
		} else {
			if (d->exclusive && d->exclusive->d->exclusive == this)
			d->exclusive->d->exclusive = 0;
			setCurrentState(Off);
		}
		stateChange(state());
	}
	repaint();
}

void KviTalCheckListItem::setCurrentState(ToggleState s)
{
	ToggleState old = d->currentState;
	d->currentState = s;
	if (d->currentState == On)
	on = true;
	else
	on = false;

//#ifndef QT_NO_ACCESSIBILITY
//    if (old != d->currentState && listView())
//        QAccessible::updateAccessibility(listView()->viewport(), indexOfItem(this), QAccessible::StateChanged);
//#else
//    Q_UNUSED(old);
//#endif
}

void KviTalCheckListItem::setStoredState(ToggleState newState, KviTalCheckListItem *key)
{
	if (myType == CheckBox || myType == CheckBoxController)
	d->statesDict[key] = newState;
}

KviTalCheckListItem::ToggleState KviTalCheckListItem::storedState(KviTalCheckListItem *key) const
{
	QHash<KviTalCheckListItem *, KviTalCheckListItem::ToggleState>::Iterator it = d->statesDict.find(key);
	if (it != d->statesDict.end())
	return it.value();
	else
	return Off;
}

void KviTalCheckListItem::turnOffChild()
{
	if (myType == RadioButtonController && d->exclusive)
	d->exclusive->setOn(false);
}

void KviTalCheckListItem::activate()
{
	KviTalListView * lv = listView();

	if (lv && !lv->isEnabled() || !isEnabled())
	return;

	QPoint pos;
	int boxsize = lv->style()->pixelMetric(QStyle::PM_CheckListButtonSize, 0, lv);
	if (activatedPos(pos)) {
	bool parentControl = false;
	if (parent() && parent()->rtti() == 1  &&
		((KviTalCheckListItem*) parent())->type() == RadioButtonController)
		parentControl = true;

	int x = parentControl ? 0 : 3;
	int align = lv->columnAlignment(0);
	int marg = lv->itemMargin();
	int y = 0;

	if (align & Qt::AlignVCenter)
		y = ((height() - boxsize) / 2) + marg;
	else
		y = (lv->fontMetrics().height() + 2 + marg - boxsize) / 2;

	QRect r(x, y, boxsize-3, boxsize-3);
	// columns might have been swapped
	r.moveBy(lv->header()->sectionPos(0), 0);
	if (!r.contains(pos))
		return;
	}
	if ((myType == CheckBox) || (myType == CheckBoxController))  {
		//lv->d->startEdit = FALSE;
		switch (internalState()) {
			case On:
				setState(Off);
				break;
			case Off:
				if (!isTristate() && myType == CheckBox) {
					setState(On);
				} else {
					setState(NoChange);
					if (myType == CheckBoxController && internalState() != NoChange)
						setState(On);
				}
				break;
			case NoChange:
				setState(On);
				break;
		}
		ignoreDoubleClick();
	} else if (myType == RadioButton) {
		setOn(true);
		ignoreDoubleClick();
	}
}

void KviTalCheckListItem::setOn(bool b )
{
	if (b)
	setState(On , true, true);
	else
	setState(Off , true, true);
}

void KviTalCheckListItem::stateChange(bool)
{
}

void KviTalCheckListItem::stateChange(ToggleState s)
{
	stateChange(s == On);
}

void KviTalCheckListItem::restoreState(KviTalCheckListItem *key, int depth)
{
	switch (type()) {
		case CheckBox:
			setCurrentState(storedState(key));
			stateChange(state());
			repaint();
			break;
		case CheckBoxController: {
			KviTalListViewItem *item = firstChild();
			int childCount = 0;
			while (item) {
				// recursively calling restoreState for children of type CheckBox and CheckBoxController
				if (item->rtti() == 1 &&
					(((KviTalCheckListItem*)item)->type() == CheckBox ||
					((KviTalCheckListItem*)item)->type() == CheckBoxController)) {
					((KviTalCheckListItem*)item)->restoreState(key , depth+1);
					childCount++;
				}
				item = item->nextSibling();
			}
			if (childCount > 0) {
				if (depth == 0)
				updateController(true);
				else
				updateController(false);
			} else {
				// if there are no children we retrieve the CheckBoxController state directly.
				setState(storedState(key), true, false);
			}
		}
			break;
		default:
			break;
	}
}

void KviTalCheckListItem::updateController(bool update , bool store)
{
	if (myType != CheckBoxController)
	return;

	KviTalCheckListItem *controller = 0;
	// checks if this CheckBoxController has another CheckBoxController as parent
	if (parent() && parent()->rtti() == 1
		&& ((KviTalCheckListItem*)parent())->type() == CheckBoxController)
	controller = (KviTalCheckListItem*)parent();

	ToggleState theState = Off;
	bool first = true;
	KviTalListViewItem *item = firstChild();
	while(item && theState != NoChange) {
		if (item->rtti() == 1 &&
			(((KviTalCheckListItem*)item)->type() == CheckBox ||
			((KviTalCheckListItem*)item)->type() == CheckBoxController)) {
			KviTalCheckListItem *checkItem = (KviTalCheckListItem*)item;
			if (first) {
				theState = checkItem->internalState();
				first = false;
			} else {
				if (checkItem->internalState() == NoChange ||
					theState != checkItem->internalState())
					theState = NoChange;
				else
					theState = checkItem->internalState();
			}
		}
		item = item->nextSibling();
	}
	if (internalState() != theState) {
		setCurrentState(theState);
		if (store && (internalState() == On || internalState() == Off))
			updateStoredState(this);
		stateChange(state());
		if (update && controller) {
			controller->updateController(update, store);
		}
		repaint();
	}
}

void KviTalCheckListItem::updateStoredState(KviTalCheckListItem *key)
{
	if (myType != CheckBoxController)
	return;

	KviTalListViewItem *item = firstChild();
	while(item) {
		if (item->rtti() == 1) {
			KviTalCheckListItem *checkItem = (KviTalCheckListItem*)item;
			if (checkItem->type() == CheckBox)
			checkItem->setStoredState(checkItem->internalState(), key);
			else if (checkItem->type() == CheckBoxController)
			checkItem->updateStoredState(key);
		}
		item = item->nextSibling();
	}
	// this state is only needed if the CheckBoxController has no CheckBox / CheckBoxController children.
	setStoredState(internalState() , key);
}

void KviTalCheckListItem::setup()
{
	KviTalListViewItem::setup();
	int h = height();
	KviTalListView *lv = listView();
	if (lv)
	h = qMax(lv->style()->pixelMetric(QStyle::PM_CheckListButtonSize, 0, lv),
			h);
	h = qMax(h, QApplication::globalStrut().height());
	setHeight(h);
}

int KviTalCheckListItem::width(const QFontMetrics& fm, const KviTalListView* lv, int column) const
{
	int r = KviTalListViewItem::width(fm, lv, column);
	if (column == 0) {
		r += lv->itemMargin();
		if (myType == RadioButtonController && pixmap(0)) {
			// r += 0;
		} else {
			r +=  lv->style()->pixelMetric(QStyle::PM_CheckListButtonSize, 0, lv) + 4;
		}
	}
	return qMax(r, QApplication::globalStrut().width());
}

void KviTalCheckListItem::paintCell(QPainter * p, const QColorGroup & cg, int column, int width, int align)
{
	if (!p)
	return;

	KviTalListView *lv = listView();
	if (!lv)
	return;

	const QPalette::ColorRole crole = lv->foregroundRole();
	if (cg.brush(crole) != lv->palette().brush(crole))
	p->fillRect(0, 0, width, height(), cg.brush(crole));
	else
	lv->paintEmptyArea(p, QRect(0, 0, width, height()));

	if (column != 0) {
		// The rest is text, or for subclasses to change.
		KviTalListViewItem::paintCell(p, cg, column, width, align);
		return;
	}

	bool parentControl = false;
	if (parent() && parent()->rtti() == 1  &&
		((KviTalCheckListItem*) parent())->type() == RadioButtonController)
	parentControl = true;

	QFontMetrics fm(lv->fontMetrics());
	int boxsize = lv->style()->pixelMetric(myType == RadioButtonController ? QStyle::PM_CheckListControllerSize :
		QStyle::PM_CheckListButtonSize, 0, lv);
	int marg = lv->itemMargin();
	int r = marg;

	// Draw controller / checkbox / radiobutton ---------------------
	QStyle::State styleflags = QStyle::State_None;
	if (internalState() == On) {
		styleflags |= QStyle::State_On;
	} else if (internalState() == NoChange) {
		if (myType == CheckBoxController && !isTristate())
			styleflags |= QStyle::State_Off;
		else
			styleflags |= QStyle::State_NoChange;
	} else {
		styleflags |= QStyle::State_Off;
	}
	if (isSelected())
	styleflags |= QStyle::State_Selected;
	if (isEnabled() && lv->isEnabled())
	styleflags |= QStyle::State_Enabled;
	if (lv->window()->isActiveWindow())
	styleflags |= QStyle::State_Active;

	if (myType == RadioButtonController) {
		int x = 0;
		if(!parentControl)
			x += 3;
		if (!pixmap(0)) {
			QStyleOptionQ3ListView opt = getStyleOption(lv, this);
			opt.rect.setRect(x, 0, boxsize, fm.height() + 2 + marg);
			opt.palette = cg;
			opt.state = styleflags;
			lv->style()->drawPrimitive(QStyle::PE_Q3CheckListController, &opt, p, lv);
			r += boxsize + 4;
		}
	} else {
		Q_ASSERT(lv); //###
		int x = 0;
		int y = 0;
		if (!parentControl)
			x += 3;
		if (align & Qt::AlignVCenter)
			y = ((height() - boxsize) / 2) + marg;
		else
			y = (fm.height() + 2 + marg - boxsize) / 2;

		QStyleOptionQ3ListView opt = getStyleOption(lv, this);
		opt.rect.setRect(x, y, boxsize, fm.height() + 2 + marg);
		opt.palette = cg;
		opt.state = styleflags;
		lv->style()->drawPrimitive((myType == CheckBox || myType == CheckBoxController)
			? QStyle::PE_Q3CheckListIndicator
			: QStyle::PE_Q3CheckListExclusiveIndicator, &opt, p, lv);
		r += boxsize + 4;
	}

	// Draw text ----------------------------------------------------
	p->translate(r, 0);
	p->setPen(QPen(cg.text()));
	KviTalListViewItem::paintCell(p, cg, column, width - r, align);
}

void KviTalCheckListItem::paintFocus(QPainter *p, const QColorGroup & cg,
const QRect & r)
{
	bool intersect = true;
	KviTalListView *lv = listView();
	if (lv && lv->header()->mapToActual(0) != 0) {
		int xdepth = lv->treeStepSize() * (depth() + (lv->rootIsDecorated() ? 1 : 0)) + lv->itemMargin();
		int p = lv->header()->cellPos(lv->header()->mapToActual(0));
		xdepth += p;
		intersect = r.intersects(QRect(p, r.y(), xdepth - p + 1, r.height()));
	}
	bool parentControl = false;
	if (parent() && parent()->rtti() == 1  &&
		((KviTalCheckListItem*) parent())->type() == RadioButtonController)
	parentControl = true;
	if (myType != RadioButtonController && intersect &&
		(lv->rootIsDecorated() || myType == RadioButton ||
		(myType == CheckBox && parentControl))) {
		QRect rect;
		int boxsize = lv->style()->pixelMetric(QStyle::PM_CheckListButtonSize, 0, lv);
		if (lv->columnAlignment(0) == Qt::AlignCenter) {
			QFontMetrics fm(lv->font());
			int bx = (lv->columnWidth(0) - (boxsize + fm.width(text())))/2 + boxsize;
			if (bx < 0) bx = 0;
			rect.setRect(r.x() + bx + 5, r.y(), r.width() - bx - 5,
					r.height());
		} else
			rect.setRect(r.x() + boxsize + 5, r.y(), r.width() - boxsize - 5,
					r.height());
		KviTalListViewItem::paintFocus(p, cg, rect);
	} else {
		KviTalListViewItem::paintFocus(p, cg, r);
	}
}


#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "kvi_tal_listview.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
