#ifndef	_CLASS_GROUPBOX_H_
#define	_CLASS_GROUPBOX_H_
//
//   File : class_groupbox.h
//   Creation date : Fri Jan 28 14:21:48 CEST 2005
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



#include <kvi_tal_groupbox.h>
#include "class_widget.h"
#include "object_macros.h"

class KviKvsObject_groupbox : public KviKvsObject_widget
{
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_groupbox)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	virtual bool init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams);

	
	bool functionSetTitle(KviKvsObjectFunctionCall *c);
	bool functionTitle(KviKvsObjectFunctionCall *c);
	bool functionSetFlat(KviKvsObjectFunctionCall *c);
	bool functionIsFlat(KviKvsObjectFunctionCall *c);
	bool functionSetCheckable(KviKvsObjectFunctionCall *c);
	bool functionIsCheckable(KviKvsObjectFunctionCall *c);
	bool functionSetInsideMargin(KviKvsObjectFunctionCall *c);
	bool functionInsideMargin(KviKvsObjectFunctionCall *c);
	bool functionSetInsideSpacing(KviKvsObjectFunctionCall *c);
	bool functionInsideSpacing(KviKvsObjectFunctionCall *c);
	bool functionSetColumns(KviKvsObjectFunctionCall *c);
	bool functionColumns(KviKvsObjectFunctionCall *c);
	bool functionAddSpace(KviKvsObjectFunctionCall *c);
	bool functionAlignment(KviKvsObjectFunctionCall *c);
	bool functionSetAlignment(KviKvsObjectFunctionCall *c);
	bool functionSetOrientation(KviKvsObjectFunctionCall *c);
	bool functionIsChecked(KviKvsObjectFunctionCall *c);
	bool functionSetChecked(KviKvsObjectFunctionCall *c);
	bool functionSetColumnLayout(KviKvsObjectFunctionCall *c);

};
#endif	//!_CLASS_GROUPBOX_H_

