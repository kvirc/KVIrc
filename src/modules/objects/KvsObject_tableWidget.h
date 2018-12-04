#ifndef _CLASS_TABLEWIDGET_H_
#define _CLASS_TABLEWIDGET_H_
//=============================================================================
//
//   File : KvsObject_tableWidget.h
//   Creation date : Wed 4 Feb 2009 09:30:05 CEST by Carbone Alessandro
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000 Krzysztof Godlewski
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KvsObject_widget.h"
#include "object_macros.h"

#include "KviKvsRunTimeContext.h"

#include <QTableWidgetItem>
#include <QItemDelegate>
#include <QAbstractItemView>

class KvsObject_tableWidget;

class KviCellItemDelegate : public QItemDelegate
{
public:
	KviCellItemDelegate(QAbstractItemView * pWidget = nullptr, KvsObject_tableWidget * pParent = nullptr);
	~KviCellItemDelegate();

protected:
	KvsObject_tableWidget * m_pParentScript;

public:
	QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const override;
	void paint(QPainter * pPainter, const QStyleOptionViewItem & option, const QModelIndex & index) const override;
};

class KvsObject_tableWidget : public KvsObject_widget
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KvsObject_tableWidget)
public:
	QWidget * widget() { return (QWidget *)object(); };
	bool paint(QPainter * pPainter, const QStyleOptionViewItem & option, const QModelIndex & index);

protected:
	KviKvsRunTimeContext * m_pContext = nullptr;
	KviCellItemDelegate * m_pCellItemDelegate = nullptr;

protected:
	bool init(KviKvsRunTimeContext * pContext, KviKvsVariantList * pParams) override;

	bool setText(KviKvsObjectFunctionCall * c);
	bool setForeground(KviKvsObjectFunctionCall * c);
	bool setNumber(KviKvsObjectFunctionCall * c);
	bool setToolTip(KviKvsObjectFunctionCall * c);
	bool text(KviKvsObjectFunctionCall * c);
	bool setIcon(KviKvsObjectFunctionCall * c);
	bool setItemFlags(KviKvsObjectFunctionCall * c);

	bool setColumnCount(KviKvsObjectFunctionCall * c);
	bool columnCount(KviKvsObjectFunctionCall * c);
	bool setRowCount(KviKvsObjectFunctionCall * c);
	bool currentRow(KviKvsObjectFunctionCall * c);
	bool currentColumn(KviKvsObjectFunctionCall * c);
	bool itemRowColAt(KviKvsObjectFunctionCall * c);

	bool rowCount(KviKvsObjectFunctionCall * c);

	bool insertRow(KviKvsObjectFunctionCall * c);
	bool insertColumn(KviKvsObjectFunctionCall * c);
	bool removeRow(KviKvsObjectFunctionCall * c);
	bool removeColumn(KviKvsObjectFunctionCall * c);

	bool setHorizontalHeaderLabels(KviKvsObjectFunctionCall * c);
	bool setVerticalHeaderLabels(KviKvsObjectFunctionCall * c);
	bool setCellWidget(KviKvsObjectFunctionCall * c);

	bool hideHorizontalHeader(KviKvsObjectFunctionCall * c);
	bool showHorizontalHeader(KviKvsObjectFunctionCall * c);
	bool hideVerticalHeader(KviKvsObjectFunctionCall * c);
	bool showVerticalHeader(KviKvsObjectFunctionCall * c);
	bool clear(KviKvsObjectFunctionCall * c);
	bool itemEnteredEvent(KviKvsObjectFunctionCall * c);
	bool cellActivatedEvent(KviKvsObjectFunctionCall * c);
	bool cellDoubleClickedEvent(KviKvsObjectFunctionCall * c);

	bool hideColumn(KviKvsObjectFunctionCall * c);
	bool showColumn(KviKvsObjectFunctionCall * c);
	bool hideRow(KviKvsObjectFunctionCall * c);
	bool showRow(KviKvsObjectFunctionCall * c);
	bool resizeRowsToContents(KviKvsObjectFunctionCall * c);
	bool resizeColumnsToContents(KviKvsObjectFunctionCall * c);
protected slots:
	void slotItemEntered(QTableWidgetItem *);
	void cellActivated(int iRow, int iCol);
	void cellDoubleClicked(int iRow, int iCol);
};

#endif // !_CLASS_TABLEWIDGET_H_
