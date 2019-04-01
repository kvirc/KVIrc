#ifndef _KVI_PTR_LIST_ITERATOR_H_
#define _KVI_PTR_LIST_ITERATOR_H_
//=============================================================================
//
//   File : KviPtrListIterator.h
//   Creation date : Tue July 30 2016 06:18:52 by Matt Ullman
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2016 Matt Ullman (staticfox at staticfox dot net)
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

#include "KviPointerList.h"

template<typename T>
class KviPtrListIterator {

	KviPointerList<T> * ptrList;
	T * c;

public:

	explicit KviPtrListIterator(KviPointerList<T> * ptrList, T *c)
	: ptrList(ptrList),
	  c(c)
	{
	}

	T &operator*() const
	{
		return *this->c;
	}

	KviPtrListIterator &operator++()
	{
		this->c = this->ptrList->next();
		return *this;
	}

	bool operator!=(const KviPtrListIterator & other) const
	{
		return this->c != other.c;
	}

};

template<typename T>
inline KviPtrListIterator<T> begin(KviPointerList<T> * ptrList)
{
	return KviPtrListIterator<T>(ptrList, ptrList->first());
}

template<typename T>
inline KviPtrListIterator<T> end(KviPointerList<T> * ptrList)
{
	return KviPtrListIterator<T>(ptrList, nullptr);
}

#endif
