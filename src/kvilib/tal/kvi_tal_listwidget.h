#ifndef _KVI_TAL_LISTWIDGET_H_
#define _KVI_TAL_LISTWIDGET_H_

//=============================================================================
//
//   File : kvi_tal_listwidget.h
//   Creation date : Mon May 05 2008 11:25:08 by Carbone Alessandro
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
#include "kvi_tal_itemdelegates.h"


#include <QListWidget>

class KviTalListWidgetItem;


class KVILIB_API KviTalListWidget : public QListWidget
{
	Q_OBJECT
public:
	KviTalListWidget(QWidget * pParent,QString name,Qt::WFlags f = 0);
	KviTalListWidget(QWidget * pParent)
		: QListWidget (pParent)/*, m_pDelegate(0)*/{};
	virtual ~KviTalListWidget() {	/*if (m_pDelegate) delete m_pDelegate;*/ };
	/*void setDelegate(QItemDelegate *itemdelegate)
	{	m_pDelegate=itemdelegate;
		setItemDelegate(itemdelegate);
	}
	*/
protected:
	//QItemDelegate *m_pDelegate;

/*public:
	KviTalListWidgetItem * firstItem() const { return (KviTalListWidgetItem *)Q3ListBox::firstItem(); };
	KviTalListWidgetItem * selectedItem() const { return (KviTalListWidgetItem *)Q3ListBox::selectedItem(); };
	KviTalListWidgetItem * item(int iIndex) const { return (KviTalListWidgetItem *)Q3ListBox::item(iIndex); };
signals:
	void highlighted(KviTalListWidgetItem *);
	void selected(KviTalListWidgetItem *);
	void selectionChanged(KviTalListWidgetItem * item);
	void currentChanged(KviTalListWidgetItem * item);
	void clicked(KviTalListWidgetItem * item);
	void clicked(KviTalListWidgetItem * item,const QPoint & pnt);
	void pressed(KviTalListWidgetItem * item);
	void pressed(KviTalListWidgetItem * item,const QPoint & pnt);
	void doubleClicked(KviTalListWidgetItem * item);
	void returnPressed(KviTalListWidgetItem * item);
	void rightButtonClicked(KviTalListWidgetItem *, const QPoint &);
	void rightButtonPressed(KviTalListWidgetItem *, const QPoint &);
	void mouseButtonPressed(int button,KviTalListWidgetItem * item,const QPoint & pos);
	void mouseButtonClicked(int button,KviTalListWidgetItem * item,const QPoint & pos);
	void contextMenuRequested(KviTalListWidgetItem * item,const QPoint & pos);
	void onItem(KviTalListWidgetItem * i);
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
	*/
};

class KVILIB_API KviTalListWidgetItem : public QListWidgetItem 
{
public:
	KviTalListWidgetItem()
	: QListWidgetItem() {};
	KviTalListWidgetItem(KviTalListWidget * pParent)
	: QListWidgetItem(pParent){};
	
	virtual ~KviTalListWidgetItem() {};

};



#endif // _KVI_TAL_LISTWIDGET_H_
