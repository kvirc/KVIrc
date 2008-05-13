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
