#ifndef	_CLASS_WORKSPACE_H_
#define	_CLASS_WORKSPACE_H_
//mdm:
//   File : class_workspace.h
//   Creation date : Sat Feb 12 23:05:58 CEST 2005
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




#include <qworkspace.h>
#include "class_widget.h"

class KviKvsObject_workspace : public KviKvsObject_widget
{
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_workspace)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:

	virtual bool init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams);

	bool functionactiveWindow(KviKvsObjectFunctionCall *c);
	bool functionscrollBarsEnabled(KviKvsObjectFunctionCall *c);
	bool functionsetscrollBarsEnabled(KviKvsObjectFunctionCall *c);
	bool functioncascade(KviKvsObjectFunctionCall *c);
	bool functiontile(KviKvsObjectFunctionCall *c);
	bool functioncloseActiveWindow(KviKvsObjectFunctionCall *c);
	bool functioncloseAllWindows(KviKvsObjectFunctionCall *c);
	bool functionactivateNextWindow(KviKvsObjectFunctionCall *c);
	bool functionactivatePrevWindow(KviKvsObjectFunctionCall *c);
};

#endif	//!_CLASS_WORKSPACE_H_
