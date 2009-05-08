#ifndef _KVI_TAL_LISTBOX_QT4_H_
#define _KVI_TAL_LISTBOX_QT4_H_

//=============================================================================
//
//   File : kvi_tal_listbox_qt4.h
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

#include <q3listbox.h>

class KviTalListBoxItem;


class KVILIB_API KviTalListBox : public Q3ListBox
{
	Q_OBJECT
public:
	KviTalListBox(QWidget * pParent,Qt::WFlags f = 0);
	virtual ~KviTalListBox() {};
public:
	KviTalListBoxItem * firstItem() const { return (KviTalListBoxItem *)Q3ListBox::firstItem(); };
	KviTalListBoxItem * selectedItem() const { return (KviTalListBoxItem *)Q3ListBox::selectedItem(); };
	KviTalListBoxItem * item(int iIndex) const { return (KviTalListBoxItem *)Q3ListBox::item(iIndex); };
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
	void redirect_highlighted(Q3ListBoxItem *);
	void redirect_selected(Q3ListBoxItem *);
	void redirect_selectionChanged(Q3ListBoxItem * item);
	void redirect_currentChanged(Q3ListBoxItem * item);
	void redirect_clicked(Q3ListBoxItem * item);
	void redirect_clicked(Q3ListBoxItem * item,const QPoint & pnt);
	void redirect_pressed(Q3ListBoxItem * item);
	void redirect_pressed(Q3ListBoxItem * item,const QPoint & pnt);
	void redirect_doubleClicked(Q3ListBoxItem * item);
	void redirect_returnPressed(Q3ListBoxItem *);
	void redirect_rightButtonClicked(Q3ListBoxItem *, const QPoint &);
	void redirect_rightButtonPressed(Q3ListBoxItem *, const QPoint &);
	void redirect_mouseButtonPressed(int button,Q3ListBoxItem * item,const QPoint & pos);
	void redirect_mouseButtonClicked(int button,Q3ListBoxItem * item,const QPoint & pos);
	void redirect_contextMenuRequested(Q3ListBoxItem * item,const QPoint & pos);
	void redirect_onItem(Q3ListBoxItem * i);
};

class KVILIB_API KviTalListBoxItem : public Q3ListBoxItem
{
public:
	KviTalListBoxItem()
	: Q3ListBoxItem() {};
	KviTalListBoxItem(KviTalListBox * pParent)
	: Q3ListBoxItem(pParent) {};
	KviTalListBoxItem(KviTalListBox * pParent,KviTalListBoxItem * pAfter)
	: Q3ListBoxItem(pParent,pAfter) {};
	virtual ~KviTalListBoxItem() {};
public:
	KviTalListBoxItem * next() const { return (KviTalListBoxItem *)Q3ListBoxItem::next(); };
	KviTalListBoxItem * prev() const { return (KviTalListBoxItem *)Q3ListBoxItem::prev(); };
	KviTalListBox * listBox() const { return (KviTalListBox *)Q3ListBoxItem::listBox(); };
	virtual int height(const KviTalListBox *) const { return 0; };
	int height(const Q3ListBox *lb) const { return height((KviTalListBox *)lb); };
	virtual int width(const KviTalListBox *) const { return 0; };
	int width(const Q3ListBox *lb) const { return width((KviTalListBox *)lb); };
};


class KVILIB_API KviTalListBoxText : public KviTalListBoxItem
{
public:
    KviTalListBoxText(KviTalListBox* listbox, const QString & text=QString());
    KviTalListBoxText(const QString & text=QString());
    KviTalListBoxText(KviTalListBox* listbox, const QString & text, KviTalListBoxItem *after);
   ~KviTalListBoxText();

    int         height(const KviTalListBox *) const;
    int         width(const KviTalListBox *)  const;

    int rtti() const;
    enum { RTTI = 1 };

protected:
    virtual void  paint(QPainter *);

private:
    Q_DISABLE_COPY(KviTalListBoxText)
};


class KVILIB_API KviTalListBoxPixmap : public KviTalListBoxItem
{
public:
    KviTalListBoxPixmap(KviTalListBox* listbox, const QPixmap &);
    KviTalListBoxPixmap(const QPixmap &);
    KviTalListBoxPixmap(KviTalListBox* listbox, const QPixmap & pix, KviTalListBoxItem *after);
    KviTalListBoxPixmap(KviTalListBox* listbox, const QPixmap &, const QString&);
    KviTalListBoxPixmap(const QPixmap &, const QString&);
    KviTalListBoxPixmap(KviTalListBox* listbox, const QPixmap & pix, const QString&, KviTalListBoxItem *after);
   ~KviTalListBoxPixmap();

    const QPixmap *pixmap() const { return &pm; }

    int         height(const KviTalListBox *) const;
    int         width(const KviTalListBox *)  const;

    int rtti() const;
    enum { RTTI = 2 };

protected:
    virtual void paint(QPainter *);

private:
    Q_DISABLE_COPY(KviTalListBoxPixmap)

    QPixmap pm;
};

#endif // _KVI_TAL_LISTBOX_QT4_H_
