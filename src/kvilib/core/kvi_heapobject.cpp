//=============================================================================
//
//   File : kvi_heapobject.cpp
//   Created on Wed 24 Mar 2004 04:45:17 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2006 Szymon Stefanek <pragma at kvirc dot net>
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



#include "kvi_heapobject.h"
#include "kvi_malloc.h"

// On windows we need to override new and delete operators
// to ensure that always the right new/delete pair is called for an object instance
// This bug jumps out because windows uses a local heap for each
// executable module (exe or dll).
// (this is a well known bug described in Q122675 of MSDN)

// on Linux it is not needed: there is a single global heap



// 05.02.2005 : scalar/vector deleting destructors in modules
//
// There are also other issues involving the MSVC compiler.
// When the operator new is called on an object with a virtual
// destructor the compiler generates a helper function
// called "vector deleting destructor" that is used to both
// free the object's memory and call the object's destructor.
// (In fact there is also a "scalar deleting destructor" but
// MSVC seems to call the vector version also for scalar deletes ?!?)
// The problem arises when operator new is called in a module:
// the helper function gets stuffed in one of the module's sections
// and when the module is unloaded any attempt to delete
// the object will simply jump into no man's land.

// An "unhandled exception" in a "call [%eax]" corresponding
// to a delete <pointer> may be a symptom of this problem.

// I haven't been able to find a solution nicer than having
// a static allocation function in each class that can be
// created from inside a module and destroyed anywhere else
// and has a virtual destructor.

#ifdef COMPILE_ON_WINDOWS
	void * KviHeapObject::operator new(size_t uSize)
	{
		return kvi_malloc(uSize);
	}

	void KviHeapObject::operator delete(void * pData)
	{
		kvi_free(pData);
	}

	void * KviHeapObject::operator new[](size_t uSize)
	{
		return kvi_malloc(uSize);
	}

	void KviHeapObject::operator delete[](void * pData)
	{
		kvi_free(pData);
	}

	// these are the debug versions...
	void * KviHeapObject::operator new(size_t uSize,const char *,int)
	{
		return kvi_malloc(uSize);
	}

	void KviHeapObject::operator delete(void * pData,const char *,int)
	{
		kvi_free(pData);
	}
#endif
