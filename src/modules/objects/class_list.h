#ifndef _CLASS_LIST_H_
#define _CLASS_LIST_H_
//=============================================================================
//
//   File : class_list.h
//   Creation date : Wed Sep 09 2000 21:07:01 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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
	bool count(KviKvsObjectFunctionCall *c);
	bool isEmpty(KviKvsObjectFunctionCall *c);
	bool clear(KviKvsObjectFunctionCall *c);
	bool append(KviKvsObjectFunctionCall *c);
	bool prepend(KviKvsObjectFunctionCall *c);
	bool insert(KviKvsObjectFunctionCall *c);
	bool at(KviKvsObjectFunctionCall *c);
	bool remove(KviKvsObjectFunctionCall *c);
	bool removeFirst(KviKvsObjectFunctionCall *c);
	bool removeLast(KviKvsObjectFunctionCall *c);
	bool removeCurrent(KviKvsObjectFunctionCall *c);
	bool moveFirst(KviKvsObjectFunctionCall *c);
	bool moveNext(KviKvsObjectFunctionCall *c);
	bool movePrev(KviKvsObjectFunctionCall *c);
	bool moveLast(KviKvsObjectFunctionCall *c);
	bool eof(KviKvsObjectFunctionCall *c);
	bool current(KviKvsObjectFunctionCall *c);
	bool sort(KviKvsObjectFunctionCall *c);
};


#endif //_CLASS_LIST_H_
