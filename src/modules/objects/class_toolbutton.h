#ifndef        _CLASS_TOOLBUTTON_H_
#define        _CLASS_TOOLBUTTON_H_

//mdm:
//   File : class_toolbutton.h
//   Creation date : Wed Feb 23 16:39:48 CEST 2005
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



#include "class_widget.h"
#include "object_macros.h"

class KviKvsObject_toolbutton : public KviKvsObject_widget
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_toolbutton)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	virtual bool init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams);

	bool functionsetImage(KviKvsObjectFunctionCall *c);
	bool functionsetUsesBigPixmap(KviKvsObjectFunctionCall *c);
	bool functionusesBigPixmap(KviKvsObjectFunctionCall *c);
	bool functionsetUsesTextLabel(KviKvsObjectFunctionCall *c);
	bool functionusesTextLabel(KviKvsObjectFunctionCall *c);
	bool functionsetTextLabel(KviKvsObjectFunctionCall *c);
	bool functiontextLabel(KviKvsObjectFunctionCall *c);
	bool functionsetPopup(KviKvsObjectFunctionCall *c);
	bool functionopenPopup(KviKvsObjectFunctionCall *c);
	bool functionsetOn(KviKvsObjectFunctionCall *c);
	bool functionsetToggleButton(KviKvsObjectFunctionCall *c);
	bool functiontoggle(KviKvsObjectFunctionCall *c);
	bool functionsetPopupDelay(KviKvsObjectFunctionCall *c);
	bool functionpopupDelay(KviKvsObjectFunctionCall *c);
	bool functionsetTextPosition(KviKvsObjectFunctionCall *c);
	bool functiontextPosition(KviKvsObjectFunctionCall *c);

	bool function_setAutoRaise(KviKvsObjectFunctionCall *c);
	bool function_autoRaise(KviKvsObjectFunctionCall *c);
	bool function_clickEvent(KviKvsObjectFunctionCall *c);
signals:
	void clicked();
protected slots:
	void slotClicked();
};


#endif        //!_CLASS_TOOLBUTTON_H_
