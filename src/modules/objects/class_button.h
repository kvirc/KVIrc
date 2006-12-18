#ifndef _CLASS_BUTTON_H_
#define _CLASS_BUTTON_H_
//
//   File : class_button.h
//   Creation date : Wed 13 Sep 2000 02:42:05 CEST by Krzysztof Godlewski
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



#include <qpushbutton.h>
#include "class_widget.h"
#include "object_macros.h"

class KviKvsObject_button : public KviKvsObject_widget
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_button)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	virtual bool init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams);

	bool functionSetText(KviKvsObjectFunctionCall *c);
	bool functionIsChecked(KviKvsObjectFunctionCall *c);
	bool functionText(KviKvsObjectFunctionCall *c);
	bool functionSetAutoDefault(KviKvsObjectFunctionCall *c);
	bool functionSetToggleButton(KviKvsObjectFunctionCall *c);
	bool functionSetOn(KviKvsObjectFunctionCall *c);
	bool functionIsOn(KviKvsObjectFunctionCall *c);
	bool functionToggle(KviKvsObjectFunctionCall *c);
	bool functionSetIsMenuButton(KviKvsObjectFunctionCall *c);
	bool functionIsMenuButton(KviKvsObjectFunctionCall *c);
	bool functionSetImage(KviKvsObjectFunctionCall *c);
	bool functionclickEvent(KviKvsObjectFunctionCall *c);

protected slots:
	void slotClicked();
};

#endif	// !_CLASS_BUTTON_H_
