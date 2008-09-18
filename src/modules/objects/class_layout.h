#ifndef _CLASS_LAYOUT_H_
#define _CLASS_LAYOUT_H_
//=============================================================================
//
//   File : class_layout.h
//   Creation date : Fri Now 22 2002 00:50:01 by Szymon Stefanek
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#include "object_macros.h"

class KviKvsObject_layout : public KviKvsObject
{
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_layout)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	virtual bool init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams);

	bool functionAddWidget(KviKvsObjectFunctionCall *c);
	bool functionAddMultiCellWidget(KviKvsObjectFunctionCall *c);
	bool functionSetRowStretch(KviKvsObjectFunctionCall *c);
	bool functionSetColumnStretch(KviKvsObjectFunctionCall *c);
	bool functionSetMargin(KviKvsObjectFunctionCall *c);
	bool functionSetSpacing(KviKvsObjectFunctionCall *c);
	bool functionAddRowSpacing(KviKvsObjectFunctionCall *c);
	bool functionAddColSpacing(KviKvsObjectFunctionCall *c);
	bool functionSetResizeMode(KviKvsObjectFunctionCall *c);
	bool functionsetAlignment(KviKvsObjectFunctionCall *c);


};


#endif //_CLASS_LAYOUT_H_
