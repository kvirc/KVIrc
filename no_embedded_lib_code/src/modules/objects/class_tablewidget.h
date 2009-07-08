#ifndef _CLASS_TABLEWIDGET_H_
#define _CLASS_TABLEWIDGET_H_
//=============================================================================
//
//   File : class_tablewidget.h
//   Creation date : Wed 4 Feb 2009 09:30:05 CEST by Carbone Alessandro
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000 Krzysztof Godlewski
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================



#include "class_widget.h"
#include "object_macros.h"
#include <QtSql>

class KviKvsObject_tablewidget : public KviKvsObject_widget
{
	Q_OBJECT
public:
        KVSO_DECLARE_OBJECT(KviKvsObject_tablewidget)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
            QSqlQueryModel *model;
protected:
	virtual bool init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams);

            bool setText(KviKvsObjectFunctionCall *c);
            bool text(KviKvsObjectFunctionCall *c);
            bool setIcon(KviKvsObjectFunctionCall *c);

            bool setColumnCount(KviKvsObjectFunctionCall *c);
            bool columnCount(KviKvsObjectFunctionCall *c);
            bool setRowCount(KviKvsObjectFunctionCall *c);
            bool rowCount(KviKvsObjectFunctionCall *c);
            bool setHorizontalHeaderLabels(KviKvsObjectFunctionCall *c);
            bool setVerticalHeaderLabels(KviKvsObjectFunctionCall *c);
            bool setCellWidget(KviKvsObjectFunctionCall *c);

            bool hideHorizontalHeader(KviKvsObjectFunctionCall *c);
            bool showHorizontalHeader(KviKvsObjectFunctionCall *c);
            bool hideVerticalHeader(KviKvsObjectFunctionCall *c);
            bool showVerticalHeader(KviKvsObjectFunctionCall *c);
            bool clear(KviKvsObjectFunctionCall *c);
            //bool clickEvent(KviKvsObjectFunctionCall *c);

protected slots:
        //void slotClicked();
};

#endif	// !_CLASS_TABLEWIDGET_H_
