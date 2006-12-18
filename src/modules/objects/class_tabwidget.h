#ifndef	_CLASS_TABWIDGET_H_
#define	_CLASS_TABWIDGET_H_
//mdm:
//   File : class_tabwidget.h
//   Creation date : Fri Feb 11 10:21:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2005 Szymon Stefanek (pragma at kvirc dot net)
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
#include <qtabwidget.h>
#include "class_widget.h"

#include "object_macros.h"

class KviKvsObject_tabwidget : public KviKvsObject_widget
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_tabwidget)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	virtual bool init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams);

	bool functionaddTab(KviKvsObjectFunctionCall *c);
	bool functioninsertTab(KviKvsObjectFunctionCall *c);
	bool functionsetTabToolTip(KviKvsObjectFunctionCall *c);
	bool functionremoveTabToolTip(KviKvsObjectFunctionCall *c);
	bool functionsetTabLabel(KviKvsObjectFunctionCall *c);
	bool functionsetCurrentPage(KviKvsObjectFunctionCall *c);
	bool functioncurrentPageIndex(KviKvsObjectFunctionCall *c);
	bool functiontabLabel(KviKvsObjectFunctionCall *c);
	bool functioncurrentTabLabel(KviKvsObjectFunctionCall *c);
	bool functionsetMargin(KviKvsObjectFunctionCall *c);
	bool functionmargin(KviKvsObjectFunctionCall *c);
	bool functioncount(KviKvsObjectFunctionCall *c);
	bool functionchangeTab(KviKvsObjectFunctionCall *c);
	bool functionCurrentChangedEvent(KviKvsObjectFunctionCall *c);
	bool functionremovePage(KviKvsObjectFunctionCall *c);
	bool functionsetTabPosition(KviKvsObjectFunctionCall *c);

protected slots:
	void slotCurrentChanged(int);
};

#endif	//!_CLASS_TABWIDGET_H_
