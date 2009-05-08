#ifndef	_CLASS_RADIOBUTTON_H_
#define	_CLASS_RADIOBUTTON_H_
//mdm:
//   File : class_radiobutton.h
//   Creation date :Thu Feb 08 14:21:48 CEST 2005
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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
#include <qradiobutton.h>
#include "class_widget.h"


#include "object_macros.h"

class KviKvsObject_radiobutton : public KviKvsObject_widget
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_radiobutton)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	virtual bool init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams);

	bool functionSetText(KviKvsObjectFunctionCall *c);
	bool functionIsChecked(KviKvsObjectFunctionCall *c);
	bool functionSetChecked(KviKvsObjectFunctionCall *c);
	bool functionSetImage(KviKvsObjectFunctionCall *c);
	bool function_toggleEvent(KviKvsObjectFunctionCall *c);
protected slots:
	void toggled(bool b);

};

#endif	//!_CLASS_RADIOBUTTON_H_
