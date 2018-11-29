#ifndef _CLASS_LAYOUT_H_
#define _CLASS_LAYOUT_H_
//=============================================================================
//
//   File : KvsObject_layout.h
//   Creation date : Fri Now 22 2002 00:50:01 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your option) any later version.
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

#include "object_macros.h"

class KvsObject_layout : public KviKvsObject
{
public:
	KVSO_DECLARE_OBJECT(KvsObject_layout)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	bool init(KviKvsRunTimeContext * pContext, KviKvsVariantList * pParams) override;

	bool addWidget(KviKvsObjectFunctionCall * c);
	bool addMultiCellWidget(KviKvsObjectFunctionCall * c);
	bool setRowStretch(KviKvsObjectFunctionCall * c);
	bool setColumnStretch(KviKvsObjectFunctionCall * c);
	bool setMargin(KviKvsObjectFunctionCall * c);
	bool setSpacing(KviKvsObjectFunctionCall * c);
	bool addRowSpacing(KviKvsObjectFunctionCall * c);
	bool addColSpacing(KviKvsObjectFunctionCall * c);
	bool setResizeMode(KviKvsObjectFunctionCall * c);
	bool setAlignment(KviKvsObjectFunctionCall * c);
};

#endif //_CLASS_LAYOUT_H_
