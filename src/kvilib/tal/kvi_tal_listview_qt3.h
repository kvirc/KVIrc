#ifndef _KVI_TAL_LISTVIEW_QT3_H_
#define _KVI_TAL_LISTVIEW_QT3_H_

//=============================================================================
//
//   File : kvi_tal_listview_qt3.h
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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "kvi_settings.h"

#include "kvi_qstring.h"

#include <qlistview.h>

class KviTalListViewItem;

class KVILIB_API KviTalListView : public QListView
{
	friend class KviTalCheckListItem;
	Q_OBJECT
public:
	KviTalListView(QWidget * pParent);
	virtual ~KviTalListView() {};
public:
	// Shadow the internal Qt methods
	KviTalListViewItem * firstChild() const { return (KviTalListViewItem *)QListView::firstChild(); };
	KviTalListViewItem * lastItem() const { return (KviTalListViewItem *)QListView::lastItem(); };
	KviTalListViewItem * selectedItem() const { return (KviTalListViewItem *)QListView::selectedItem(); };
	KviTalListViewItem * currentItem() const { return (KviTalListViewItem *)QListView::currentItem(); };
	KviTalListViewItem * itemAt(const QPoint &pnt) const { return (KviTalListViewItem *)QListView::itemAt(pnt); };
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
	void redirect_selectionChanged(QListViewItem * pItem);
	void redirect_currentChanged(QListViewItem * pItem);
	void redirect_clicked(QListViewItem * pItem);
	void redirect_clicked(QListViewItem * pItem,const QPoint &pnt,int uColumn);
	void redirect_pressed(QListViewItem * pItem);
	void redirect_pressed(QListViewItem * pItem,const QPoint &pnt,int uColumn);
	void redirect_doubleClicked(QListViewItem * pItem);
	void redirect_doubleClicked(QListViewItem * pItem,const QPoint &pnt,int uColumn);
	void redirect_returnPressed(QListViewItem * pItem);
	void redirect_spacePressed(QListViewItem * pItem);
	void redirect_rightButtonClicked(QListViewItem * pItem,const QPoint &pnt,int uColumn);
	void redirect_rightButtonPressed(QListViewItem * pItem,const QPoint &pnt,int uColumn);
	void redirect_mouseButtonClicked(int iButton,QListViewItem * pItem,const QPoint &pnt,int uColumn);
	void redirect_mouseButtonPressed(int iButton,QListViewItem * pItem,const QPoint &pnt,int uColumn);
	void redirect_contextMenuRequested(QListViewItem * pItem,const QPoint &pnt,int uColumn);
	void redirect_onItem(QListViewItem * pItem);
	void redirect_expanded(QListViewItem * pItem);
	void redirect_collapsed(QListViewItem * pItem);
};


class KVILIB_API KviTalListViewItem : public QListViewItem
{
public:
	KviTalListViewItem(KviTalListView * pParent)
	: QListViewItem(pParent) {};
	KviTalListViewItem(KviTalListViewItem * pParent)
	: QListViewItem(pParent) {};
	KviTalListViewItem(KviTalListView * pParent,KviTalListViewItem * pAfter)
	: QListViewItem(pParent,pAfter) {};
	KviTalListViewItem(KviTalListViewItem * pParent,KviTalListViewItem * pAfter)
	: QListViewItem(pParent,pAfter) {};
	KviTalListViewItem(KviTalListView * pParent,const QString &szLabel1,const QString &szLabel2 = KviQString::empty,const QString &szLabel3 = KviQString::empty,const QString &szLabel4 = KviQString::empty,const QString &szLabel5 = KviQString::empty)
	: QListViewItem(pParent,szLabel1,szLabel2,szLabel3,szLabel4,szLabel5) {};
	KviTalListViewItem(KviTalListView * pParent,KviTalListViewItem * pAfter,const QString &szLabel1,const QString &szLabel2 = KviQString::empty,const QString &szLabel3 = KviQString::empty,const QString &szLabel4 = KviQString::empty,const QString &szLabel5 = KviQString::empty)
	: QListViewItem(pParent,pAfter,szLabel1,szLabel2,szLabel3,szLabel4,szLabel5) {};
	KviTalListViewItem(KviTalListViewItem * pParent,const QString &szLabel1,const QString &szLabel2 = KviQString::empty,const QString &szLabel3 = KviQString::empty,const QString &szLabel4 = KviQString::empty)
	: QListViewItem(pParent,szLabel1,szLabel2,szLabel3,szLabel4) {};
	KviTalListViewItem(KviTalListViewItem * pParent,KviTalListViewItem * pAfter,const QString &szLabel1,const QString &szLabel2 = KviQString::empty,const QString &szLabel3 = KviQString::empty,const QString &szLabel4 = KviQString::empty)
	: QListViewItem(pParent,pAfter,szLabel1,szLabel2,szLabel3,szLabel4) {};
	virtual ~KviTalListViewItem() {};
public:
	// Shadow the internal Qt methods
	KviTalListViewItem * firstChild() const { return (KviTalListViewItem *)QListViewItem::firstChild(); };
	KviTalListViewItem * nextSibling() const { return (KviTalListViewItem *)QListViewItem::nextSibling(); };
	KviTalListViewItem * parent() const { return (KviTalListViewItem *)QListViewItem::parent(); };
	KviTalListView * listView() const { return (KviTalListView *)QListViewItem::listView(); };
	KviTalListViewItem * itemAbove() { return (KviTalListViewItem *)QListViewItem::itemAbove(); };
	KviTalListViewItem * itemBelow() { return (KviTalListViewItem *)QListViewItem::itemBelow(); };
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

	KviTalCheckListItem(KviTalCheckListItem *parent, const QString &text,Type = RadioButtonController);
	KviTalCheckListItem(KviTalCheckListItem *parent, KviTalListViewItem *after,const QString &text, Type = RadioButtonController);
	KviTalCheckListItem( KviTalListViewItem *parent, const QString &text,Type = RadioButtonController );
	KviTalCheckListItem( KviTalListViewItem *parent, KviTalListViewItem *after,const QString &text, Type = RadioButtonController );
	KviTalCheckListItem( KviTalListView *parent, const QString &text,Type = RadioButtonController );
	KviTalCheckListItem( KviTalListView *parent, KviTalListViewItem *after,const QString &text, Type = RadioButtonController );
	KviTalCheckListItem( KviTalListViewItem *parent, const QString &text,const QPixmap & );
	KviTalCheckListItem( KviTalListView *parent, const QString &text,const QPixmap & );
	~KviTalCheckListItem();

	void paintCell( QPainter *,const QColorGroup & cg,int column, int width, int alignment );
    virtual void paintFocus( QPainter *, const QColorGroup & cg,
			     const QRect & r );
    int width( const QFontMetrics&, const KviTalListView*, int column) const;
    void setup();

    virtual void setOn( bool ); // ### should be replaced by setChecked in ver4
    bool isOn() const { return on; }
    Type type() const { return myType; }
    QString text() const { return KviTalListViewItem::text( 0 ); }
    QString text( int n ) const { return KviTalListViewItem::text( n ); }

    void setTristate( bool );
    bool isTristate() const;
    ToggleState state() const;
    void setState( ToggleState s);

    int rtti() const;
    static int RTTI;

protected:
    void activate();
    void turnOffChild();
    virtual void stateChange( bool );

private:
    void init();
    ToggleState internalState() const;
    void setStoredState( ToggleState newState, void *key );
    ToggleState storedState( void *key ) const;
    void stateChange( ToggleState s );
    void restoreState( void *key, int depth = 0 );
    void updateController( bool update = TRUE , bool store = FALSE );
    void updateStoredState( void *key );
    void setState( ToggleState s, bool update, bool store );
    void setCurrentState( ToggleState s );

    Type myType;
    bool on; // ### remove in ver4
    KviTalCheckListItemPrivate *d;
};


#define KviTalListViewItemIterator QListViewItemIterator 

#endif // _KVI_TAL_LISTVIEW_QT3_H_
