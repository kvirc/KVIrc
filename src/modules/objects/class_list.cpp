//=============================================================================
//
//   File : class_list.cpp
//   Creation date : Wed Sep 09 2000 21:07:55 by Szymon Stefanek
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
//============================================================================

#include "kvi_debug.h"

#include "kvi_malloc.h"
#include "kvi_locale.h"

#include "class_list.h"

#include <stdlib.h>


/*
	@doc: list
	@keyterms:
		list object class, sorted list
	@title:
		list class
	@type:
		class
	@short:
		Abstracts a double linked list of strings
	@inherits:
		[class]object[/class]
	@description:


		This object class abstracts a double linked list of strings.
		You can insert/remove items by zero based position, at the end (tail) or at the
		beginning (head). The list incorporates an efficient iteration
		method by using the [classfnc:list]$moveFirst[/classfnc](),[classfnc:list]$moveLast[/classfnc](),
		[classfnc:list]$moveNext[/classfnc](),[classfnc:list]$movePrev[/classfnc](),
		[classfnc:list]$current[/classfnc]() and [classfnc:list]$eof[/classfnc]() functions.
	@functions:
		!fn: <integer> $count()
		Returns the number of items in the list

		!fn: <boolean> $isEmpty()
		Returns $true if the list is empty, $false otherwise.

		!fn: $clear()
		Removes all the items from the list.This is the same as
		[classfnc:list]$removeAll[/classfnc]().

		!fn: $removeAll()
		Removes all the items from the list. This is the same as
		[classfnc:list]$clear[/classfnc]().

		!fn: $append(<item:variant>)
		Inserts the <item> at the end (tail) of the list.

		!fn: $prepend(<item:variant>)
		Inserts the <item> at the beginning (head) of the list.

		!fn: $insert(<index:uint>,<item:variant>)
		Inserts the <item> at zero-based position <index> in the list.
		If <index> is greater or equal to [classfnc:list]$count[/classfnc]()
		then the item is simply appended to the end of the list.

		!fn: $add(<index:uint>,<item:variant>)
		This is exactly the same as [classfnc:list]$insert[/classfnc]().

		!fn: <variant> $item(<index:uint>)
		Returns the item at zero-based <index>. If <index> is greater
		or equal to [classfnc:list]$count[/classfnc]() (beyond the end of the list)
		then [fnc]$nothing[/fnc] is returned.

		!fn: <boolean> $remove(<index:uint>)
		Removes the item at zero-based <index>. Returns $true
		if the item was succesfully removed and $false otherwise (i.e. <index>
		pointed beyond the end of the list).

		!fn: <boolean> $removeFirst()
		Removes the first item from the list. Returns $true
		if the item was succesfully removed (the list was not empty)
		and $false otherwise.

		!fn: <boolean> $removeLast()
		Removes the last item from the list. Returns $true
		if the item was succesfully removed (the list was not empty)
		and $false otherwise.

		!fn: <boolean> $removeCurrent()
		Removes the current item from the list. Returns $true
		if the item was succesfully removed or $false otherwise.
		Invalidates any iteration operation.

		!fn: <boolean> $moveFirst()
		Moves the iterator to the first item in the list and returns
		$true if the move was succesfull (i.e., the list is not empty)
		and $false otherwise.

		!fn: <boolean> $moveLast()
		Moves the iterator to the last item in the list and returns
		$true if the move was succesfull (i.e., the list is not empty)
		and $false otherwise.

		!fn: <boolean> $movePrev()
		Moves the iterator to the previous item and returns $true
		if the move was succesfull (i.e., there IS a previous item)
		and $false otherwise.

		!fn: <boolean> $moveNext()
		Moves the iterator to the next item and returns $true
		if the move was succesfull (i.e., there IS a next item)
		and $false otherwise.

		!fn: <boolean> $eof()
		Returns $true if the iterator points to a valid
		item in the list (and thus [classfnc:list]$current[/classfnc]()
		would return a valid value) and $false otherwise.

		!fn: <boolean> $current()
		Returns the item pointed by the current iterator
		or [fnc]$nothing[/fnc] is the iterator is not valid (points
		beyond the end of the list).
*/


KVSO_BEGIN_REGISTERCLASS(KviKvsObject_list,"list","object")
	KVSO_REGISTER_HANDLER(KviKvsObject_list,"count",function_count)
	KVSO_REGISTER_HANDLER(KviKvsObject_list,"isEmpty",function_isEmpty)
	KVSO_REGISTER_HANDLER(KviKvsObject_list,"clear",function_clear)
	KVSO_REGISTER_HANDLER(KviKvsObject_list,"removeAll",function_clear)
	KVSO_REGISTER_HANDLER(KviKvsObject_list,"append",function_append)
	KVSO_REGISTER_HANDLER(KviKvsObject_list,"prepend",function_prepend)
	KVSO_REGISTER_HANDLER(KviKvsObject_list,"insert",function_insert)
	KVSO_REGISTER_HANDLER(KviKvsObject_list,"add",function_insert)
	KVSO_REGISTER_HANDLER(KviKvsObject_list,"at",function_at)
	KVSO_REGISTER_HANDLER(KviKvsObject_list,"item",function_at)
	KVSO_REGISTER_HANDLER(KviKvsObject_list,"remove",function_remove)
	KVSO_REGISTER_HANDLER(KviKvsObject_list,"removeFirst",function_removeFirst)
	KVSO_REGISTER_HANDLER(KviKvsObject_list,"removeLast",function_removeLast)
	KVSO_REGISTER_HANDLER(KviKvsObject_list,"removeCurrent",function_removeCurrent)
	KVSO_REGISTER_HANDLER(KviKvsObject_list,"moveFirst",function_moveFirst)
	KVSO_REGISTER_HANDLER(KviKvsObject_list,"moveNext",function_moveNext)
	KVSO_REGISTER_HANDLER(KviKvsObject_list,"movePrev",function_movePrev)
	KVSO_REGISTER_HANDLER(KviKvsObject_list,"moveLast",function_moveLast)
	KVSO_REGISTER_HANDLER(KviKvsObject_list,"sort",function_sort)
	KVSO_REGISTER_HANDLER(KviKvsObject_list,"eof",function_eof)
	KVSO_REGISTER_HANDLER(KviKvsObject_list,"current",function_current)
KVSO_END_REGISTERCLASS(KviKvsObject_list)


KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_list,KviKvsObject)
	m_pDataList = new KviPointerList<KviKvsVariant>;
	m_pDataList->setAutoDelete(true);
KVSO_END_CONSTRUCTOR(KviKvsObject_list)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_list)
	delete m_pDataList;
	m_pDataList = 0;
KVSO_END_CONSTRUCTOR(KviKvsObject_list)

bool KviKvsObject_list::function_current(KviKvsObjectFunctionCall *c)
{
	if(!m_pDataList)
	{
		c->returnValue()->setNothing();
		return true;
	}
	KviKvsVariant * v = m_pDataList->safeCurrent();
	if(v)c->returnValue()->copyFrom(*v);
	else c->returnValue()->setNothing();
	return true;
}

bool KviKvsObject_list::function_eof(KviKvsObjectFunctionCall *c)
{
	if(!m_pDataList)
	{
		c->returnValue()->setBoolean(true);
		return true;
	}
	c->returnValue()->setBoolean(m_pDataList->current() != 0);
	return true;
}

bool KviKvsObject_list::function_moveLast(KviKvsObjectFunctionCall *c)
{
	if(!m_pDataList)
	{
		c->returnValue()->setBoolean(false);
		return true;
	}
	c->returnValue()->setBoolean(m_pDataList->last() != 0);
	return true;
}

bool KviKvsObject_list::function_movePrev(KviKvsObjectFunctionCall *c)
{
	if(!m_pDataList)
	{
		c->returnValue()->setBoolean(false);
		return true;
	}
	c->returnValue()->setBoolean(m_pDataList->prev() != 0);
	return true;
}

bool KviKvsObject_list::function_moveNext(KviKvsObjectFunctionCall *c)
{
	if(!m_pDataList)
	{
		c->returnValue()->setBoolean(false);
		return true;
	}
	c->returnValue()->setBoolean(m_pDataList->next() != 0);
	return true;
}

bool KviKvsObject_list::function_moveFirst(KviKvsObjectFunctionCall *c)
{
	if(!m_pDataList)
	{
		c->returnValue()->setBoolean(false);
		return true;
	}
	c->returnValue()->setBoolean(m_pDataList->first() != 0);
	return true;
}

bool KviKvsObject_list::function_removeLast(KviKvsObjectFunctionCall *c)
{
	if(!m_pDataList)
	{
		c->returnValue()->setBoolean(false);
		return true;
	}
	c->returnValue()->setBoolean(m_pDataList->removeLast());
	return true;
}

bool KviKvsObject_list::function_removeCurrent(KviKvsObjectFunctionCall *c)
{
	if(!m_pDataList)
	{
		c->returnValue()->setBoolean(false);
		return true;
	}
	if(m_pDataList->current())
	{
		m_pDataList->removeCurrent();
		c->returnValue()->setBoolean(true);
	} else {
		c->returnValue()->setBoolean(false);
	}
	return true;
}

bool KviKvsObject_list::function_removeFirst(KviKvsObjectFunctionCall *c)
{
	if(!m_pDataList)
	{
		c->returnValue()->setBoolean(false);
		return true;
	}
	c->returnValue()->setBoolean(m_pDataList->removeFirst());
	return true;
}

bool KviKvsObject_list::function_remove(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t uIndex;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("index",KVS_PT_UINT,0,uIndex)
	KVSO_PARAMETERS_END(c)
	if(!m_pDataList)
	{
		c->returnValue()->setBoolean(false);
		return true;
	}
	c->returnValue()->setBoolean(m_pDataList->remove(uIndex));
	return true;
}

bool KviKvsObject_list::function_at(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t uIndex;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("index",KVS_PT_UINT,0,uIndex)
	KVSO_PARAMETERS_END(c)
	if(!m_pDataList)
	{
		c->returnValue()->setNothing();
		return true;
	}
	KviKvsVariant * v = m_pDataList->at(uIndex);
	if(v)c->returnValue()->copyFrom(*v);
	else c->returnValue()->setNothing();
	return true;
}

bool KviKvsObject_list::function_insert(KviKvsObjectFunctionCall *c)
{
	kvs_uint_t uIndex;
	KviKvsVariant * pVar;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("index",KVS_PT_UINT,0,uIndex)
		KVSO_PARAMETER("item",KVS_PT_VARIANT,0,pVar)
	KVSO_PARAMETERS_END(c)
	if(!m_pDataList)return true;
	m_pDataList->insert(uIndex,new KviKvsVariant(*pVar));
	return true;
}

bool KviKvsObject_list::function_prepend(KviKvsObjectFunctionCall *c)
{
	KviKvsVariant * pVar;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("item",KVS_PT_VARIANT,0,pVar)
	KVSO_PARAMETERS_END(c)
	if(!m_pDataList)return true;
	m_pDataList->prepend(new KviKvsVariant(*pVar));
	return true;
}

bool KviKvsObject_list::function_append(KviKvsObjectFunctionCall *c)
{
	KviKvsVariant * pVar;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("item",KVS_PT_VARIANT,0,pVar)
	KVSO_PARAMETERS_END(c)
	if(!m_pDataList)return true;
	m_pDataList->append(new KviKvsVariant(*pVar));
	if (m_pDataList->count()) m_pDataList->first();
	return true;
}

bool KviKvsObject_list::function_clear(KviKvsObjectFunctionCall *c)
{
	if(!m_pDataList)return true;
	m_pDataList->clear();
	return true;
}

inline int kvi_compare(const KviKvsVariant * p1,const KviKvsVariant * p2)
{
	return p1->compare(p2);
}

bool KviKvsObject_list::function_sort(KviKvsObjectFunctionCall *c)
{
	if(!m_pDataList)return true;
	m_pDataList->sort();
	return true;
}

bool KviKvsObject_list::function_isEmpty(KviKvsObjectFunctionCall *c)
{
	if(!m_pDataList)
	{
		c->returnValue()->setBoolean(true);
		return true;
	}
	c->returnValue()->setBoolean(m_pDataList->isEmpty());
	return true;
}

bool KviKvsObject_list::function_count(KviKvsObjectFunctionCall *c)
{
	if(!m_pDataList)
	{
		c->returnValue()->setInteger(0);
		return true;
	}
	c->returnValue()->setInteger(m_pDataList->count());
	return true;
}

