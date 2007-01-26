#ifndef _KVI_TAL_LISTBOX_QT3_H_
#define _KVI_TAL_LISTBOX_QT3_H_

//=============================================================================
//
//   File : kvi_tal_listbox_qt3.h
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

#include "kvi_settings.h"
#include "kvi_qstring.h"
#include <qlistbox.h>

class KviTalListBoxItem;

class KVILIB_API KviTalListBox : public QListBox
{
	Q_OBJECT
public:
	KviTalListBox(QWidget * pParent,Qt::WFlags f = 0);
	virtual ~KviTalListBox() {};
public:
	KviTalListBoxItem * firstItem() const { return (KviTalListBoxItem *)QListBox::firstItem(); };
	KviTalListBoxItem * selectedItem() const { return (KviTalListBoxItem *)QListBox::selectedItem(); };
	KviTalListBoxItem * item(int iIndex) const { return (KviTalListBoxItem *)QListBox::item(iIndex); };
signals:
	void highlighted(KviTalListBoxItem *);
	void selected(KviTalListBoxItem *);
	void selectionChanged(KviTalListBoxItem * item);
	void currentChanged(KviTalListBoxItem * item);
	void clicked(KviTalListBoxItem * item);
	void clicked(KviTalListBoxItem * item,const QPoint & pnt);
	void pressed(KviTalListBoxItem * item);
	void pressed(KviTalListBoxItem * item,const QPoint & pnt);
	void doubleClicked(KviTalListBoxItem * item);
	void returnPressed(KviTalListBoxItem * item);
	void rightButtonClicked(KviTalListBoxItem *, const QPoint &);
	void rightButtonPressed(KviTalListBoxItem *, const QPoint &);
	void mouseButtonPressed(int button,KviTalListBoxItem * item,const QPoint & pos);
	void mouseButtonClicked(int button,KviTalListBoxItem * item,const QPoint & pos);
	void contextMenuRequested(KviTalListBoxItem * item,const QPoint & pos);
	void onItem(KviTalListBoxItem * i);
protected slots:
	void redirect_highlighted(QListBoxItem *item);
	void redirect_selected(QListBoxItem *item);
	void redirect_selectionChanged(QListBoxItem * item);
	void redirect_currentChanged(QListBoxItem * item);
	void redirect_clicked(QListBoxItem * item);
	void redirect_clicked(QListBoxItem * item,const QPoint & pnt);
	void redirect_pressed(QListBoxItem * item);
	void redirect_pressed(QListBoxItem * item,const QPoint & pnt);
	void redirect_doubleClicked(QListBoxItem * item);
	void redirect_returnPressed(QListBoxItem * item);
	void redirect_rightButtonClicked(QListBoxItem * item, const QPoint &pnt);
	void redirect_rightButtonPressed(QListBoxItem * item, const QPoint &pnt);
	void redirect_mouseButtonPressed(int button,QListBoxItem * item,const QPoint & pos);
	void redirect_mouseButtonClicked(int button,QListBoxItem * item,const QPoint & pos);
	void redirect_contextMenuRequested(QListBoxItem * item,const QPoint & pos);
	void redirect_onItem(QListBoxItem * i);
};

class KVILIB_API KviTalListBoxItem : public QListBoxItem
{
public:
	KviTalListBoxItem()
	: QListBoxItem() {};
	KviTalListBoxItem(KviTalListBox * pParent)
	: QListBoxItem(pParent) {};
	KviTalListBoxItem(KviTalListBox * pParent,KviTalListBoxItem * pAfter)
	: QListBoxItem(pParent,pAfter) {};
	virtual ~KviTalListBoxItem() {};
public:
	KviTalListBoxItem * next() const { return (KviTalListBoxItem *)QListBoxItem::next(); };
	KviTalListBoxItem * prev() const { return (KviTalListBoxItem *)QListBoxItem::prev(); };
	KviTalListBox * listBox() const { return (KviTalListBox *)QListBoxItem::listBox(); };
	virtual int height(const KviTalListBox *) const { return 0; };
	int height(const QListBox *lb) const { return height((KviTalListBox *)lb); };
	virtual int width(const KviTalListBox *) const { return 0; };
	int width(const QListBox *lb) const { return width((KviTalListBox *)lb); };
};

class KVILIB_API KviTalListBoxText : public KviTalListBoxItem
{
public:
    KviTalListBoxText( KviTalListBox* listbox, const QString & text=QString::null );
    KviTalListBoxText( const QString & text=QString::null );
    KviTalListBoxText( KviTalListBox* listbox, const QString & text, KviTalListBoxItem *after );
   ~KviTalListBoxText();

    int	 height( const KviTalListBox * ) const;
    int	 width( const KviTalListBox * )  const;

    int rtti() const;
    static int RTTI;

protected:
    virtual void  paint( QPainter * );

private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    KviTalListBoxText( const KviTalListBoxText & );
    KviTalListBoxText &operator=( const KviTalListBoxText & );
#endif
};


class KVILIB_API KviTalListBoxPixmap : public KviTalListBoxItem
{
public:
    KviTalListBoxPixmap( KviTalListBox* listbox, const QPixmap & );
    KviTalListBoxPixmap( const QPixmap & );
    KviTalListBoxPixmap( KviTalListBox* listbox, const QPixmap & pix, KviTalListBoxItem *after );
    KviTalListBoxPixmap( KviTalListBox* listbox, const QPixmap &, const QString& );
    KviTalListBoxPixmap( const QPixmap &, const QString& );
    KviTalListBoxPixmap( KviTalListBox* listbox, const QPixmap & pix, const QString&, KviTalListBoxItem *after );
   ~KviTalListBoxPixmap();

    const QPixmap *pixmap() const { return &pm; }

    int	 height( const KviTalListBox * ) const;
    int	 width( const KviTalListBox * )  const;

    int rtti() const;
    static int RTTI;

protected:
    virtual void paint( QPainter * );

private:
    QPixmap pm;
private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    KviTalListBoxPixmap( const KviTalListBoxPixmap & );
    KviTalListBoxPixmap &operator=( const KviTalListBoxPixmap & );
#endif
};


#endif // _KVI_TAL_LISTBOX_QT3_H_
