#ifndef _CLASS_MENUBAR_H_
#define _CLASS_MENUBAR_H_
//=============================================================================
//
//   File : class_menubar.h
//   Creation date : Tue Now 26 13:15:48 CEST 2002 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include <qmenubar.h>
#include "class_widget.h"
#include "object_macros.h"

class KviKvsObject_menubar : public KviKvsObject_widget
{
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_menubar)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	virtual bool init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams);

	bool insertItem(KviKvsObjectFunctionCall *c);
};
#endif	// !_CLASS_MENUBAR_H_
