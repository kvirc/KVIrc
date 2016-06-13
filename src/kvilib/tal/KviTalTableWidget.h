#ifndef _KVI_TAL_TABLEWIDGET_H_
#define _KVI_TAL_TABLEWIDGET_H_
//=============================================================================
//
//   File : KviTalTableWidget.h
//   Creation date : Fri Jun 27 2008 10:00:08 by Fabio Bas
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Fabio Bas (ctrlaltca at gmail dot com)
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

#include "kvi_settings.h"
#include "KviQString.h"

#include <QTableWidget>
#include <QHeaderView>

class KviTalTableWidgetItem;

class KVILIB_API KviTalTableWidget : public QTableWidget
{
	friend class KviTalTableWidgetItem;
	Q_OBJECT
public:
	KviTalTableWidget(QWidget * pParent);
	virtual ~KviTalTableWidget(){};

public:
};

class KVILIB_API KviTalTableWidgetItem : public QTableWidgetItem
{
public:
	KviTalTableWidgetItem(KviTalTableWidget * pParent)
	    : QTableWidgetItem()
	{
		pParent->insertRow(pParent->rowCount());
		pParent->setItem(pParent->rowCount() - 1, 0, this);
	};

	KviTalTableWidgetItem(KviTalTableWidget * pParent, int row, int column)
	    : QTableWidgetItem()
	{
		pParent->setItem(row, column, this);
	};

	KviTalTableWidgetItem(const KviTalTableWidgetItem & other)
	    : QTableWidgetItem(other){};
};

class KVILIB_API KviTalTableWidgetItemEx : public KviTalTableWidgetItem
{
public:
	KviTalTableWidgetItemEx(KviTalTableWidget * pParent)
	    : KviTalTableWidgetItem(pParent){};

	KviTalTableWidgetItemEx(KviTalTableWidget * pParent, int row, int column)
	    : KviTalTableWidgetItem(pParent, row, column){};

	KviTalTableWidgetItemEx(const KviTalTableWidgetItem & other)
	    : KviTalTableWidgetItem(other){};
	virtual void displayUpdate() = 0;
};

#endif // _KVI_TAL_TABLEWIDGET_H_
