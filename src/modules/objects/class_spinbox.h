#ifndef	_CLASS_SPINBOX_H_
#define	_CLASS_SPINBOX_H_
//
//   File : class_progressbar.h
//   Creation date : Fri Jan 30 14:21:48 CEST 2005
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



#include <qspinbox.h>
#include "class_widget.h"

#include "object_macros.h"

class KviKvsObject_spinbox : public KviKvsObject_widget
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_spinbox)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	virtual bool init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams);

	bool functionsetTracking(KviKvsObjectFunctionCall *c);

	bool functionsetValue(KviKvsObjectFunctionCall *c);
	bool functionsetMinValue(KviKvsObjectFunctionCall *c);
	bool functionsetMaxValue(KviKvsObjectFunctionCall *c);
	bool functionsetLineStep(KviKvsObjectFunctionCall *c);
	bool functionsetPrefix(KviKvsObjectFunctionCall *c);
	bool functionsetSuffix(KviKvsObjectFunctionCall *c);
	bool functionvalue(KviKvsObjectFunctionCall *c);
	bool functionminValue(KviKvsObjectFunctionCall *c);
	bool functionmaxValue(KviKvsObjectFunctionCall *c);
	bool functionlineStep(KviKvsObjectFunctionCall *c);
	bool functionsetSpecialValueText(KviKvsObjectFunctionCall *c);
	bool functionspecialValueText(KviKvsObjectFunctionCall *c);
	
	bool functionvalueChangedEvent(KviKvsObjectFunctionCall *c);
	
protected slots:
	void valueChanged(int);
};

#endif	//!_CLASS_SPINBOX_H_
