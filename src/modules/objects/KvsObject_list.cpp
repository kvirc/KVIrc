//=============================================================================
//
//   File : KvsObject_list.cpp
//   Creation date : Wed Sep 09 2000 21:07:55 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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
//============================================================================

#include "kvi_debug.h"
#include "KviMemory.h"
#include "KviLocale.h"
#include "KvsObject_list.h"
#include <cstdlib>

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
		!fn: $sort(<bReverse:bool>)
		Sorts items in the list alphabetically ($true) or in reverse order ($false); Default to $false.
		[example]
			%list=$new(list);[br]
			%list->$append('Foo');[br]
			%list->$append('Bar');[br]
			%list->$append('Dummy');[br]
			%list->$append('Aria');[br]
			[br]
			%list->$sort(true);[br]
			%list->$moveFirst();[br]
			while(%list->$eof())[br]
			{[br]
			  echo %list->$current();[br]
			  %list->$moveNext();[br]
			}[br]
		[/example]
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
		if the item was successfully removed and $false otherwise (i.e. <index>
		pointed beyond the end of the list).
		!fn: <boolean> $removeFirst()
		Removes the first item from the list. Returns $true
		if the item was successfully removed (the list was not empty)
		and $false otherwise.
		!fn: <boolean> $removeLast()
		Removes the last item from the list. Returns $true
		if the item was successfully removed (the list was not empty)
		and $false otherwise.
		!fn: <boolean> $removeCurrent()
		Removes the current item from the list. Returns $true
		if the item was successfully removed or $false otherwise.
		Invalidates any iteration operation.
		!fn: <boolean> $moveFirst()
		Moves the iterator to the first item in the list and returns
		$true if the move was successful (i.e., the list is not empty)
		and $false otherwise.
		!fn: <boolean> $moveLast()
		Moves the iterator to the last item in the list and returns
		$true if the move was successful (i.e., the list is not empty)
		and $false otherwise.
		!fn: <boolean> $movePrev()
		Moves the iterator to the previous item and returns $true
		if the move was successful (i.e., there IS a previous item)
		and $false otherwise.
		!fn: <boolean> $moveNext()
		Moves the iterator to the next item and returns $true
		if the move was successful (i.e., there IS a next item)
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

KVSO_BEGIN_REGISTERCLASS(KvsObject_list, "list", "object")
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_list, count)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_list, isEmpty)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_list, clear)
KVSO_REGISTER_HANDLER(KvsObject_list, "removeAll", clear)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_list, append)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_list, prepend)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_list, insert)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_list, insert)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_list, at)
KVSO_REGISTER_HANDLER(KvsObject_list, "item", at)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_list, remove)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_list, removeFirst)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_list, removeLast)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_list, removeCurrent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_list, moveFirst)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_list, moveNext)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_list, movePrev)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_list, moveLast)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_list, sort)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_list, eof)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_list, current)
KVSO_END_REGISTERCLASS(KvsObject_list)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_list, KviKvsObject)
m_pDataList = new KviPointerList<KviKvsVariant>;
m_pDataList->setAutoDelete(true);
m_pDataList->first();
KVSO_END_CONSTRUCTOR(KvsObject_list)

KVSO_BEGIN_DESTRUCTOR(KvsObject_list)
delete m_pDataList;
m_pDataList = nullptr;
KVSO_END_CONSTRUCTOR(KvsObject_list)

KVSO_CLASS_FUNCTION(list, current)
{
	CHECK_INTERNAL_POINTER(m_pDataList)
	if(!m_pDataList->count())
	{
		c->returnValue()->setNothing();
		return true;
	}
	KviKvsVariant * v = m_pDataList->safeCurrent();
	if(v)
		c->returnValue()->copyFrom(*v);
	else
		c->returnValue()->setNothing();
	return true;
}

KVSO_CLASS_FUNCTION(list, eof)
{
	CHECK_INTERNAL_POINTER(m_pDataList)
	c->returnValue()->setBoolean(m_pDataList->safeCurrent() != nullptr);
	return true;
}

KVSO_CLASS_FUNCTION(list, moveLast)
{
	CHECK_INTERNAL_POINTER(m_pDataList)
	c->returnValue()->setBoolean(m_pDataList->last() != nullptr);
	return true;
}

KVSO_CLASS_FUNCTION(list, movePrev)
{
	CHECK_INTERNAL_POINTER(m_pDataList)
	c->returnValue()->setBoolean(m_pDataList->prev() != nullptr);
	return true;
}

KVSO_CLASS_FUNCTION(list, moveNext)
{
	CHECK_INTERNAL_POINTER(m_pDataList)
	c->returnValue()->setBoolean(m_pDataList->next() != nullptr);
	return true;
}

KVSO_CLASS_FUNCTION(list, moveFirst)
{
	CHECK_INTERNAL_POINTER(m_pDataList)
	c->returnValue()->setBoolean(m_pDataList->first() != nullptr);
	return true;
}

KVSO_CLASS_FUNCTION(list, removeLast)
{
	CHECK_INTERNAL_POINTER(m_pDataList)
	c->returnValue()->setBoolean(m_pDataList->removeLast());
	return true;
}

KVSO_CLASS_FUNCTION(list, removeCurrent)
{
	CHECK_INTERNAL_POINTER(m_pDataList)
	if(m_pDataList->count())
	{
		m_pDataList->removeCurrent();
		c->returnValue()->setBoolean(true);
	}
	else
	{
		c->returnValue()->setBoolean(false);
	}
	return true;
}

KVSO_CLASS_FUNCTION(list, removeFirst)
{
	CHECK_INTERNAL_POINTER(m_pDataList)
	c->returnValue()->setBoolean(m_pDataList->removeFirst());
	return true;
}

KVSO_CLASS_FUNCTION(list, remove)
{
	CHECK_INTERNAL_POINTER(m_pDataList)
	kvs_uint_t uIndex;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("index", KVS_PT_UINT, 0, uIndex)
	KVSO_PARAMETERS_END(c)
	c->returnValue()->setBoolean(m_pDataList->remove(uIndex));
	return true;
}

KVSO_CLASS_FUNCTION(list, at)
{
	CHECK_INTERNAL_POINTER(m_pDataList)
	kvs_uint_t uIndex;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("index", KVS_PT_UINT, 0, uIndex)
	KVSO_PARAMETERS_END(c)
	KviKvsVariant * v = m_pDataList->at(uIndex);
	if(v)
		c->returnValue()->copyFrom(*v);
	else
		c->returnValue()->setNothing();
	return true;
}

KVSO_CLASS_FUNCTION(list, insert)
{
	CHECK_INTERNAL_POINTER(m_pDataList)
	kvs_uint_t uIndex;
	KviKvsVariant * pVar;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("index", KVS_PT_UINT, 0, uIndex)
	KVSO_PARAMETER("item", KVS_PT_VARIANT, 0, pVar)
	KVSO_PARAMETERS_END(c)
	m_pDataList->insert(uIndex, new KviKvsVariant(*pVar));
	return true;
}

KVSO_CLASS_FUNCTION(list, prepend)
{
	CHECK_INTERNAL_POINTER(m_pDataList)
	KviKvsVariant * pVar;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("item", KVS_PT_VARIANT, 0, pVar)
	KVSO_PARAMETERS_END(c)
	m_pDataList->prepend(new KviKvsVariant(*pVar));
	return true;
}

KVSO_CLASS_FUNCTION(list, append)
{
	CHECK_INTERNAL_POINTER(m_pDataList)
	KviKvsVariant * pVar;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("item", KVS_PT_VARIANT, 0, pVar)
	KVSO_PARAMETERS_END(c)
	m_pDataList->append(new KviKvsVariant(*pVar));
	if(m_pDataList->count() == 1)
		m_pDataList->first();
	return true;
}

KVSO_CLASS_FUNCTION(list, clear)
{
	CHECK_INTERNAL_POINTER(m_pDataList)
	m_pDataList->clear();
	return true;
}

int kvi_compare(const KviKvsVariant * p1, const KviKvsVariant * p2)
{
	return p1->compare(p2);
}

KVSO_CLASS_FUNCTION(list, sort)
{
	CHECK_INTERNAL_POINTER(m_pDataList)
	bool bReverse;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bReverse", KVS_PT_BOOL, KVS_PF_OPTIONAL, bReverse)
	KVSO_PARAMETERS_END(c)
	m_pDataList->sort();
	if(bReverse)
		m_pDataList->invert();
	return true;
}

KVSO_CLASS_FUNCTION(list, isEmpty)
{
	CHECK_INTERNAL_POINTER(m_pDataList)
	c->returnValue()->setBoolean(m_pDataList->isEmpty());
	return true;
}

KVSO_CLASS_FUNCTION(list, count)
{
	CHECK_INTERNAL_POINTER(m_pDataList)
	c->returnValue()->setInteger(m_pDataList->count());
	return true;
}
