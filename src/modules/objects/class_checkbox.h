#ifndef	_CLASS_CHECKBOX_H_
#define	_CLASS_CHECKBOX_H_

//vim: ts=8
//   File : class_checkbox.h
//   Creation date : Wed Sep 20 23:02:11 CEST 2000 by Krzysztof Godlewski
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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



#include "class_button.h"
#include "object_macros.h"

class KviKvsObject_checkbox : public KviKvsObject_button
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_checkbox)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	virtual bool init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams);
	bool function_setChecked(KviKvsObjectFunctionCall *c);
	bool function_isChecked(KviKvsObjectFunctionCall *c);
	bool function_toggleEvent(KviKvsObjectFunctionCall *c);
	bool function_setText(KviKvsObjectFunctionCall *c);
protected slots:
	void toggled(bool b);
};
#endif	// !_CLASS_CHECKBOX_H_
