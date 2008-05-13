#ifndef _KVI_TAL_TREEWIDGET_H_
#define _KVI_TAL_TREEWIDGET_H_

//=============================================================================
//
//   File : kvi_tal_listview.h
//   Creation date : Mon Jan 22 2007 11:25:08 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2007 Szymon Stefanek (pragma at kvirc dot net)
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

#include <QTreeWidget>
#include <QAbstractItemView>

class KviTalTreeWidgetItem;


class KVILIB_API KviTalTreeWidget : public QTreeWidget
{
	//friend class KviTalCheckListItem;
	Q_OBJECT
public:
	KviTalTreeWidget(QWidget * pParent);
	virtual ~KviTalTreeWidget() {};
public:
	void addColumn(const QString label){setHeaderLabel(label);}; 
	// Shadow the internal Qt methods
	/*KviTalTreeWidgetItem * firstChild() const { return (KviTalTreeWidgetItem *)Q3ListView::firstChild(); };
	KviTalTreeWidgetItem * lastItem() const { return (KviTalTreeWidgetItem *)Q3ListView::lastItem(); };
	KviTalTreeWidgetItem * selectedItem() const { return (KviTalTreeWidgetItem *)Q3ListView::selectedItem(); };
	KviTalTreeWidgetItem * currentItem() const { return (KviTalTreeWidgetItem *)Q3ListView::currentItem(); };
	KviTalTreeWidgetItem * itemAt(const QPoint &pnt) const { return (KviTalTreeWidgetItem *)Q3ListView::itemAt(pnt); };

signals:
	void selectionChanged(KviTalTreeWidgetItem * pItem);
	void currentChanged(KviTalTreeWidgetItem * pItem);
	void clicked(KviTalTreeWidgetItem * pItem);
	void clicked(KviTalTreeWidgetItem * pItem,const QPoint &pnt,int uColumn);
	void pressed(KviTalTreeWidgetItem * pItem);
	void pressed(KviTalTreeWidgetItem * pItem,const QPoint &pnt,int uColumn);
	void doubleClicked(KviTalTreeWidgetItem * pItem);
	void doubleClicked(KviTalTreeWidgetItem * pItem,const QPoint &pnt,int uColumn);
	void returnPressed(KviTalTreeWidgetItem * pItem);
	void spacePressed(KviTalTreeWidgetItem * pItem);
	void rightButtonClicked(KviTalTreeWidgetItem * pItem,const QPoint &pnt,int uColumn);
	void rightButtonPressed(KviTalTreeWidgetItem * pItem,const QPoint &pnt,int uColumn);
	void mouseButtonClicked(int iButton,KviTalTreeWidgetItem * pItem,const QPoint &pnt,int uColumn);
	void mouseButtonPressed(int iButton,KviTalTreeWidgetItem * pItem,const QPoint &pnt,int uColumn);
	void contextMenuRequested(KviTalTreeWidgetItem * pItem,const QPoint &pnt,int uColumn);
	void onItem(KviTalTreeWidgetItem * pItem);
	void expanded(KviTalTreeWidgetItem * pItem);
	void collapsed(KviTalTreeWidgetItem * pItem);
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
*/
};


class KVILIB_API KviTalTreeWidgetItem : public QTreeWidgetItem
{
public:
	KviTalTreeWidgetItem(KviTalTreeWidget * pParent)
	: QTreeWidgetItem(pParent) {};
	KviTalTreeWidgetItem(KviTalTreeWidgetItem * pParent)
	: QTreeWidgetItem(pParent) {};
	KviTalTreeWidgetItem(KviTalTreeWidgetItem * pParent,const QString &szLabel)
	: QTreeWidgetItem(pParent) {
		setText(0,szLabel);
	};
	KviTalTreeWidgetItem(KviTalTreeWidget * pParent,const QString &szLabel)
		: QTreeWidgetItem(pParent) {
	setText(0,szLabel);
	};
	int compare ( KviTalTreeWidgetItem * i, int col, bool ascending ) const
	{
		QString key=text(col);
		return key.localeAwareCompare(i->text(col));
	};
	/*
	KviTalTreeWidgetItem(KviTalTreeWidget * pParent,KviTalTreeWidgetItem * pAfter)
	: Q3ListViewItem(pParent,pAfter) {};
	KviTalTreeWidgetItem(KviTalTreeWidgetItem * pParent,KviTalTreeWidgetItem * pAfter)
	: Q3ListViewItem(pParent,pAfter) {};
	const QString &szLabel2 = KviQString::empty,const QString &szLabel3 = KviQString::empty,const QString &szLabel4 = KviQString::empty,const QString &szLabel5 = KviQString::empty)
	: Q3ListViewItem(pParent,szLabel1,szLabel2,szLabel3,szLabel4,szLabel5) {};
	KviTalTreeWidgetItem(KviTalTreeWidget * pParent,KviTalTreeWidgetItem * pAfter,const QString &szLabel1,const QString &szLabel2 = KviQString::empty,const QString &szLabel3 = KviQString::empty,const QString &szLabel4 = KviQString::empty,const QString &szLabel5 = KviQString::empty)
	: Q3ListViewItem(pParent,pAfter,szLabel1,szLabel2,szLabel3,szLabel4,szLabel5) {};
	KviTalTreeWidgetItem(KviTalTreeWidgetItem * pParent,const QString &szLabel1,const QString &szLabel2 = KviQString::empty,const QString &szLabel3 = KviQString::empty,const QString &szLabel4 = KviQString::empty)
	: Q3ListViewItem(pParent,szLabel1,szLabel2,szLabel3,szLabel4) {};
	KviTalTreeWidgetItem(KviTalTreeWidgetItem * pParent,KviTalTreeWidgetItem * pAfter,const QString &szLabel1,const QString &szLabel2 = KviQString::empty,const QString &szLabel3 = KviQString::empty,const QString &szLabel4 = KviQString::empty)
	: Q3ListViewItem(pParent,pAfter,szLabel1,szLabel2,szLabel3,szLabel4) {};
public:
	// Shadow the internal Qt methods
	KviTalTreeWidgetItem * firstChild() const { return (KviTalTreeWidgetItem *)Q3ListViewItem::firstChild(); };
	KviTalTreeWidgetItem * nextSibling() const { return (KviTalTreeWidgetItem *)Q3ListViewItem::nextSibling(); };
	KviTalTreeWidgetItem * parent() const { return (KviTalTreeWidgetItem *)Q3ListViewItem::parent(); };
	KviTalTreeWidget * listView() const { return (KviTalTreeWidget *)Q3ListViewItem::listView(); };
	KviTalTreeWidgetItem * itemAbove() { return (KviTalTreeWidgetItem *)Q3ListViewItem::itemAbove(); };
	KviTalTreeWidgetItem * itemBelow() { return (KviTalTreeWidgetItem *)Q3ListViewItem::itemBelow(); };
	*/
};
/*
struct KviTalCheckListItemPrivate;

class KVILIB_API KviTalCheckListItem : public KviTalTreeWidgetItem
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
	KviTalCheckListItem(KviTalCheckListItem *parent, KviTalTreeWidgetItem *after,
			const QString &text, Type = RadioButtonController);
	KviTalCheckListItem(KviTalTreeWidgetItem *parent, const QString &text,
			Type = RadioButtonController);
	KviTalCheckListItem(KviTalTreeWidgetItem *parent, KviTalTreeWidgetItem *after,
			const QString &text, Type = RadioButtonController);
	KviTalCheckListItem(KviTalTreeWidget *parent, const QString &text,
			Type = RadioButtonController);
	KviTalCheckListItem(KviTalTreeWidget *parent, KviTalTreeWidgetItem *after,
			const QString &text, Type = RadioButtonController);
	KviTalCheckListItem(KviTalTreeWidgetItem *parent, const QString &text,
			const QPixmap &);
	KviTalCheckListItem(KviTalTreeWidget *parent, const QString &text,
			const QPixmap &);
	~KviTalCheckListItem();

	void paintCell(QPainter *, const QColorGroup & cg,
			int column, int width, int alignment);
	virtual void paintFocus(QPainter *, const QColorGroup &cg,
				const QRect & r);
	int width(const QFontMetrics&, const KviTalTreeWidget*, int column) const;
	void setup();

	virtual void setOn(bool);
	bool isOn() const { return on; }
	Type type() const { return myType; }
	QString text() const { return KviTalTreeWidgetItem::text(0); }
	QString text(int n) const { return KviTalTreeWidgetItem::text(n); }

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

#define KviTalTreeWidgetItemIterator Q3ListViewItemIterator
*/
#endif // _KVI_TAL_TREEWIDGETW_H_
