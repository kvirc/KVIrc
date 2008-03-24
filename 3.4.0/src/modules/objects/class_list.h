#ifndef _CLASS_LIST_H_
#define _CLASS_LIST_H_
//=========================================================================================
//
//   File : class_list.h
//   Creation date : Wed Sep 09 2000 21:07:01 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2005 Szymon Stefanek (pragma at kvirc dot net)
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
//=========================================================================================

#include "kvi_settings.h"
#include "kvi_string.h"
#include "kvi_pointerlist.h"
#include "kvi_kvs_variant.h"
#include "object_macros.h"


class KviKvsObject_list : public KviKvsObject
{
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_list)
protected:
	KviPointerList<KviKvsVariant> * m_pDataList;
protected:
	KviPointerList<KviKvsVariant> * dataList() const { return m_pDataList; };
public:
	bool function_count(KviKvsObjectFunctionCall *c);
	bool function_isEmpty(KviKvsObjectFunctionCall *c);
	bool function_clear(KviKvsObjectFunctionCall *c);
	bool function_append(KviKvsObjectFunctionCall *c);
	bool function_prepend(KviKvsObjectFunctionCall *c);
	bool function_insert(KviKvsObjectFunctionCall *c);
	bool function_at(KviKvsObjectFunctionCall *c);
	bool function_remove(KviKvsObjectFunctionCall *c);
	bool function_removeFirst(KviKvsObjectFunctionCall *c);
	bool function_removeLast(KviKvsObjectFunctionCall *c);
	bool function_removeCurrent(KviKvsObjectFunctionCall *c);
	bool function_moveFirst(KviKvsObjectFunctionCall *c);
	bool function_moveNext(KviKvsObjectFunctionCall *c);
	bool function_movePrev(KviKvsObjectFunctionCall *c);
	bool function_moveLast(KviKvsObjectFunctionCall *c);
	bool function_eof(KviKvsObjectFunctionCall *c);
	bool function_current(KviKvsObjectFunctionCall *c);
	bool function_sort(KviKvsObjectFunctionCall *c);
};


#endif //_CLASS_LIST_H_
