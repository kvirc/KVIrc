#ifndef _CLASS_BUTTONGROUP_H_
#define _CLASS_BUTTONGROUP_H_
//=============================================================================
//
//   File : KvsObject_buttonGroup.h
//   Creation date : Fri Jan 28 14:21:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2009 Alessandro Carbone (elfonol at gmail dot com)
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

#include <QButtonGroup>
#include "object_macros.h"

class KvsObject_buttonGroup : public KviKvsObject
{
public:
	KVSO_DECLARE_OBJECT(KvsObject_buttonGroup)
protected:
	QButtonGroup * m_pButtonGroup;
	int m_iId;
	KviPointerHashTable<int, KviKvsObject> btnDict;
	bool addButton(KviKvsObjectFunctionCall * c);
	bool checkedButton(KviKvsObjectFunctionCall * c);
};
#endif //!_CLASS_GROUPBOX_H_
