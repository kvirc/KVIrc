#ifndef _CLASS_DIALOG_H_
#define _CLASS_DIALOG_H_
//=============================================================================
//
//   File : class_dialog.h
//   Creation date : Sun Apr 10 22:51:48 CEST 2002 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2009 Szymon Stefanek (pragma at kvirc dot net)
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

class KviKvsObject_dialog : public KviKvsObject_widget
{

public:
	KVSO_DECLARE_OBJECT(KviKvsObject_dialog)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	virtual bool init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams);
	bool setModal(KviKvsObjectFunctionCall *c);
};

#endif // !_CLASS_DIALOG_H_

