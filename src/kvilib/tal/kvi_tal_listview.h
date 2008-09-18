#ifndef _KVI_TAL_LISTVIEW_H_
#define _KVI_TAL_LISTVIEW_H_

//=============================================================================
//
//   File : kvi_tal_listview.h
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


//
// This is the only reasonable CROSS-QT3-QT4-COMPATIBLE implementation
// of QListView I've been able to find.
// Note that using macros for the items will NOT work since moc
// doesn't expand them. Note also that KviTalCheckListItem must
// be fully reimplemented and not be inherited from QCheckListItem
// to build up a consistent item object hierarchy. To complete
// the obscenity, we need TWO COMPLETE implementations: one for Qt3
// and one for Qt4... bleah :D
//
// The code for KviTalCheckListItem is adapted from qlistview.h/cpp
// present in qt 3.3.6 AND in qt 4.1.2.
//

#include "kvi_settings.h"
#include "kvi_qstring.h"

#include <q3listview.h>

class KviTalListViewItem;


class KVILIB_API KviTalListView : public Q3ListView
{
	friend class KviTalCheckListItem;
	Q_OBJECT
public:
	KviTalListView(QWidget * pParent);
	virtual ~KviTalListView() {};
public:
	// Shadow the internal Qt methods
	KviTalListViewItem * firstChild() const { return (KviTalListViewItem *)Q3ListView::firstChild(); };
	KviTalListViewItem * lastItem() const { return (KviTalListViewItem *)Q3ListView::lastItem(); };
	KviTalListViewItem * selectedItem() const { return (KviTalListViewItem *)Q3ListView::selectedItem(); };
	KviTalListViewItem * currentItem() const { return (KviTalListViewItem *)Q3ListView::currentItem(); };
	KviTalListViewItem * itemAt(const QPoint &pnt) const { return (KviTalListViewItem *)Q3ListView::itemAt(pnt); };
signals:
	void selectionChanged(KviTalListViewItem * pItem);
	void currentChanged(KviTalListViewItem * pItem);
	void clicked(KviTalListViewItem * pItem);
	void clicked(KviTalListViewItem * pItem,const QPoint &pnt,int uColumn);
	void pressed(KviTalListViewItem * pItem);
	void pressed(KviTalListViewItem * pItem,const QPoint &pnt,int uColumn);
	void doubleClicked(KviTalListViewItem * pItem);
	void doubleClicked(KviTalListViewItem * pItem,const QPoint &pnt,int uColumn);
	void returnPressed(KviTalListViewItem * pItem);
	void spacePressed(KviTalListViewItem * pItem);
	void rightButtonClicked(KviTalListViewItem * pItem,const QPoint &pnt,int uColumn);
	void rightButtonPressed(KviTalListViewItem * pItem,const QPoint &pnt,int uColumn);
	void mouseButtonClicked(int iButton,KviTalListViewItem * pItem,const QPoint &pnt,int uColumn);
	void mouseButtonPressed(int iButton,KviTalListViewItem * pItem,const QPoint &pnt,int uColumn);
	void contextMenuRequested(KviTalListViewItem * pItem,const QPoint &pnt,int uColumn);
	void onItem(KviTalListViewItem * pItem);
	void expanded(KviTalListViewItem * pItem);
	void collapsed(KviTalListViewItem * pItem);
protected slots:
	void redirect_selectionChanged(Q3ListViewItem * pItem);
	void redirect_currentChanged(Q3ListViewItem * pItem);
	void redirect_clicked(Q3ListViewItem * pItem);
	void redirect_clicked(Q3ListViewItem * pItem,const QPoint &pnt,int uColumn);
	void redirect_pressed(Q3ListViewItem * pItem);
	void redirect_pressed(Q3ListViewItem * pItem,const QPoint &pnt,int uColumn);
	void redirect_doubleClicked(Q3ListViewItem * pItem);
	void redirect_doubleClicked(Q3ListViewItem * pItem,const QPoint &pnt,int uColumn);
	void redirect_returnPressed(Q3ListViewItem * pItem);
	void redirect_spacePressed(Q3ListViewItem * pItem);
	void redirect_rightButtonClicked(Q3ListViewItem * pItem,const QPoint &pnt,int uColumn);
	void redirect_rightButtonPressed(Q3ListViewItem * pItem,const QPoint &pnt,int uColumn);
	void redirect_mouseButtonClicked(int iButton,Q3ListViewItem * pItem,const QPoint &pnt,int uColumn);
	void redirect_mouseButtonPressed(int iButton,Q3ListViewItem * pItem,const QPoint &pnt,int uColumn);
	void redirect_contextMenuRequested(Q3ListViewItem * pItem,const QPoint &pnt,int uColumn);
	void redirect_onItem(Q3ListViewItem * pItem);
	void redirect_expanded(Q3ListViewItem * pItem);
	void redirect_collapsed(Q3ListViewItem * pItem);

};


class KVILIB_API KviTalListViewItem : public Q3ListViewItem
{
public:
	KviTalListViewItem(KviTalListView * pParent)
	: Q3ListViewItem(pParent) {};
	KviTalListViewItem(KviTalListViewItem * pParent)
	: Q3ListViewItem(pParent) {};
	KviTalListViewItem(KviTalListView * pParent,KviTalListViewItem * pAfter)
	: Q3ListViewItem(pParent,pAfter) {};
	KviTalListViewItem(KviTalListViewItem * pParent,KviTalListViewItem * pAfter)
	: Q3ListViewItem(pParent,pAfter) {};
	KviTalListViewItem(KviTalListView * pParent,const QString &szLabel1,const QString &szLabel2 = KviQString::empty,const QString &szLabel3 = KviQString::empty,const QString &szLabel4 = KviQString::empty,const QString &szLabel5 = KviQString::empty)
	: Q3ListViewItem(pParent,szLabel1,szLabel2,szLabel3,szLabel4,szLabel5) {};
	KviTalListViewItem(KviTalListView * pParent,KviTalListViewItem * pAfter,const QString &szLabel1,const QString &szLabel2 = KviQString::empty,const QString &szLabel3 = KviQString::empty,const QString &szLabel4 = KviQString::empty,const QString &szLabel5 = KviQString::empty)
	: Q3ListViewItem(pParent,pAfter,szLabel1,szLabel2,szLabel3,szLabel4,szLabel5) {};
	KviTalListViewItem(KviTalListViewItem * pParent,const QString &szLabel1,const QString &szLabel2 = KviQString::empty,const QString &szLabel3 = KviQString::empty,const QString &szLabel4 = KviQString::empty)
	: Q3ListViewItem(pParent,szLabel1,szLabel2,szLabel3,szLabel4) {};
	KviTalListViewItem(KviTalListViewItem * pParent,KviTalListViewItem * pAfter,const QString &szLabel1,const QString &szLabel2 = KviQString::empty,const QString &szLabel3 = KviQString::empty,const QString &szLabel4 = KviQString::empty)
	: Q3ListViewItem(pParent,pAfter,szLabel1,szLabel2,szLabel3,szLabel4) {};
public:
	// Shadow the internal Qt methods
	KviTalListViewItem * firstChild() const { return (KviTalListViewItem *)Q3ListViewItem::firstChild(); };
	KviTalListViewItem * nextSibling() const { return (KviTalListViewItem *)Q3ListViewItem::nextSibling(); };
	KviTalListViewItem * parent() const { return (KviTalListViewItem *)Q3ListViewItem::parent(); };
	KviTalListView * listView() const { return (KviTalListView *)Q3ListViewItem::listView(); };
	KviTalListViewItem * itemAbove() { return (KviTalListViewItem *)Q3ListViewItem::itemAbove(); };
	KviTalListViewItem * itemBelow() { return (KviTalListViewItem *)Q3ListViewItem::itemBelow(); };
};

struct KviTalCheckListItemPrivate;

class KVILIB_API KviTalCheckListItem : public KviTalListViewItem
{
public:
	enum Type {
		RadioButton,
		CheckBox,
		Controller,
		RadioButtonController=Controller,
		CheckBoxController
	};

	enum ToggleState { Off, NoChange, On };

	KviTalCheckListItem(KviTalCheckListItem *parent, const QString &text,
			Type = RadioButtonController);
	KviTalCheckListItem(KviTalCheckListItem *parent, KviTalListViewItem *after,
			const QString &text, Type = RadioButtonController);
	KviTalCheckListItem(KviTalListViewItem *parent, const QString &text,
			Type = RadioButtonController);
	KviTalCheckListItem(KviTalListViewItem *parent, KviTalListViewItem *after,
			const QString &text, Type = RadioButtonController);
	KviTalCheckListItem(KviTalListView *parent, const QString &text,
			Type = RadioButtonController);
	KviTalCheckListItem(KviTalListView *parent, KviTalListViewItem *after,
			const QString &text, Type = RadioButtonController);
	KviTalCheckListItem(KviTalListViewItem *parent, const QString &text,
			const QPixmap &);
	KviTalCheckListItem(KviTalListView *parent, const QString &text,
			const QPixmap &);
	~KviTalCheckListItem();

	void paintCell(QPainter *, const QColorGroup & cg,
			int column, int width, int alignment);
	virtual void paintFocus(QPainter *, const QColorGroup &cg,
				const QRect & r);
	int width(const QFontMetrics&, const KviTalListView*, int column) const;
	void setup();

	virtual void setOn(bool);
	bool isOn() const { return on; }
	Type type() const { return myType; }
	QString text() const { return KviTalListViewItem::text(0); }
	QString text(int n) const { return KviTalListViewItem::text(n); }

	void setTristate(bool);
	bool isTristate() const;
	ToggleState state() const;
	void setState(ToggleState s);

	int rtti() const;
	enum { RTTI = 1 };

protected:
	void activate();
	void turnOffChild();
	virtual void stateChange(bool);

private:
	void init();
	ToggleState internalState() const;
	void setStoredState(ToggleState newState, KviTalCheckListItem *key);
	ToggleState storedState(KviTalCheckListItem *key) const;
	void stateChange(ToggleState s);
	void restoreState(KviTalCheckListItem *key, int depth = 0);
	void updateController(bool update = true , bool store = false);
	void updateStoredState(KviTalCheckListItem *key);
	void setState(ToggleState s, bool update, bool store);
	void setCurrentState(ToggleState s);

	Type myType;
	bool on;
	KviTalCheckListItemPrivate *d;
};

#define KviTalListViewItemIterator Q3ListViewItemIterator

#endif // _KVI_TAL_LISTVIEW_H_
