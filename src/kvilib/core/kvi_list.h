#ifndef _KVI_LIST_H_
#define _KVI_LIST_H_
//=================================================================================================
//
//   File : kvi_list.h
//   Creation date : Tue Jul 6 1999 14:52:20 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2007 Szymon Stefanek (pragma at kvirc dot net)
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
//=================================================================================================
//=============================================================================
//
//   C++ Template based double linked pointer list class
//   Original ss_list.h Created on 10 Dec 2001
//   Copyright (C) 2001-2007 Szymon Stefanek (pragma at kvirc dot net)
//   Added to KVIrc on 02 Jan 2008.
//
//=============================================================================

// Qt changes the collection classes too much and too frequently.
// I think we need to be independent of that to the maximum degree possible.
// That's why we have our own fast pointer list class.
// This does not depend on Qt AT ALL and has an interface similar
// to the Qt<=3.x series. The pointer lists with the autodelete
// feature was great and I don't completly understand why they have
// been removed from Qt4 in favor of the value based non-autodeleting
// lists... anyway: here we go :)

#include "kvi_settings.h"

template<typename T> class KviPtrList;
template<typename T> class KviPtrListIterator;

#ifndef NULL
	#define NULL 0
#endif

///
/// \internal
///
class KviPtrListNode
{
public:
	KviPtrListNode * m_pPrev;
	void               * m_pData;
	KviPtrListNode * m_pNext;
};

///
/// \class KviPtrListIterator
/// \brief A fast KviPtrList iterator.
///
/// This class allows traversing the list sequentially.
/// Multilpe iterators can traverse the list at the same time.
/// 
/// Iteration example 1:
///
/// \verbatim
/// KviPtrListIterator<T> it(list);
/// for(bool b = it.moveFirst();b;b = it.moveNext())
/// {
///     T * pData = it.data();
///     doSomethingWithData(pData);
/// }
/// \endverbatim
///
/// Iteration example 2:
///
/// \verbatim
/// KviPtrListIterator<T> it(list);
/// if(it.moveFirst())
/// {
///     do {
///         T * pData = it.data();
///         doSomethingWithData(pData);
///     } while(it.moveNext());
/// }
/// \endverbatim
///
/// Iteration example 3:
///
/// \verbatim
/// KviPtrListIterator<T> it(list.iteratorAt(10));
/// if(it.isValid())
/// {
///    do {
///         T * pData = it.data();
///         doSomethingWithData(pData);
///    while(it.movePrev());
/// }
/// \endverbatim
///
/// Please note that you must NOT remove any item from
/// the list when using the iterators. An iterator pointing
/// to a removed item will crash your application if you use it.
/// The following code will NOT work (and crash):
///
/// \verbatim
/// KviPtrList<T> l;
/// l.append(new KviStr("x"));
/// l.append(new KviStr("y"));
/// KviPtrListIterator<T> it(l);
/// it.moveFirst();
/// l.removeFirst();
/// KviStr * tmp = it.data(); <-- this will crash
/// \endverbatim
///
/// In the rare cases in that you need to remove items
/// while traversing the list you should put them
/// in a temporary list and remove them after the iteration.
///
/// I've choosen this way because usually you don't modify
/// the list while traversing it and a fix for this
/// would add a constant overhead to several list operation.
/// You just must take care of it yourself.
///
/// \warning This class is not thread safe by itself.
///
template<typename T> class KviPtrListIterator
{
protected:
	KviPtrList<T>                 * m_pList;
	KviPtrListNode                * m_pNode;
public:
	///
	/// Creates an iterator copy.
	/// The new iterator points exactly to the item pointed by src.
	///
	KviPtrListIterator(const KviPtrListIterator<T> &src)
	{
		m_pList = src.m_pList;
		m_pNode = src.m_pNode;
	}

	///
	/// Creates an iterator for the list l.
	/// The iterator points to the first list item, if any.
	///
	KviPtrListIterator(KviPtrList<T> &l)
	{
		m_pList = (KviPtrList<T> *)&l;
		m_pNode = m_pList->m_pHead;
	}

	///
	/// Creates an iterator for the list l.
	/// The iterator points to the specified list node.
	///
	KviPtrListIterator(KviPtrList<T> &l,KviPtrListNode * pNode)
	{
		m_pList = (KviPtrList<T> *)&l;
		m_pNode = pNode;
	}

	///
	/// Creates an iterator copy.
	/// The new iterator points exactly to the item pointed by src.
	///
	void operator = (const KviPtrListIterator<T> &src)
	{
		m_pList = src.m_pList;
		m_pNode = src.m_pNode;
	}
public:
	///
	/// Moves the iterator to the first element of the list.
	/// Returns true in case of success or false if the list is empty.
	///
	bool moveFirst()
	{
		m_pNode = m_pList->m_pHead;
		return m_pNode != NULL;
	}
	
	///
	/// Moves the iterator to the last element of the list.
	/// Returns true in case of success or false if the list is empty.
	///
	bool moveLast()
	{
		m_pNode = m_pList->m_pTail;
		return m_pNode != NULL;
	}

	///
	/// Moves the iterator to the next element of the list.
	/// The iterator must be actually valid for this function to work.
	/// Returns true in case of success or false if there is no next item.
	///
	bool moveNext()
	{
		if(!m_pNode)return false;
		m_pNode = m_pNode->m_pNext;
		return m_pNode != NULL;
	}

	///
	/// Moves the iterator to the next element of the list.
	/// The iterator must be actually valid for this operator to work.
	/// Returns true in case of success or false if there is no next item.
	/// This is just a convenient alias to moveNext().
	///
	bool operator ++()
	{
		if(!m_pNode)return false;
		m_pNode = m_pNode->m_pNext;
		return m_pNode != NULL;
	}

	///
	/// Moves the iterator to the previous element of the list.
	/// The iterator must be actually valid for this function to work.
	/// Returns true in case of success or false if there is no previous item.
	///
	bool movePrev()
	{
		if(!m_pNode)return false;
		m_pNode = m_pNode->m_pPrev;
		return m_pNode != NULL;
	}

	///
	/// Moves the iterator to the previous element of the list.
	/// The iterator must be actually valid for this operator to work.
	/// Returns true in case of success or false if there is no previous item.
	/// This is just a convenient alias to movePrev().
	///
	bool operator --()
	{
		if(!m_pNode)return false;
		m_pNode = m_pNode->m_pPrev;
		return m_pNode != NULL;
	}

	///
	/// Returs the value pointed by the iterator
	/// or NULL if the iterator is not valid.
	///
	T * current()
	{
		return m_pNode ? (T *)(m_pNode->m_pData) : NULL;
	}

	///
	/// Returs the value pointed by the iterator
	/// or NULL if the iterator is not valid.
	/// This is just an alias to current().
	///
	T * operator *()
	{
		return m_pNode ? (T *)(m_pNode->m_pData) : NULL;
	}

	///
	/// Returns true if this iterator points to a valid
	/// element of the list and false otherwise.
	///
	bool isValid()
	{
		return m_pNode != NULL;
	}
};

///
/// \class KviPtrList
/// \brief A template double linked list of pointers.
///
/// The main advantage of this type of list is speed.
/// Insertion of pointers is very fast when compared
/// to the typical "copy constructor" call used
/// in the "plain type" template list implementations.
///
/// Iterating over pointers is also very fast and this
/// class contains an internal iterator that allows to
/// write loops in a compact and clean way.
/// See the first(), next(), current() and findRef()
/// functions for the description of this feature.
///
/// There is also a non-const external iterator
/// that you can use to traverse the list concurrently.
/// There is no const iterator (and no const access methods)
/// since the list provides the autoDelete() method
/// which vould implicitly violate constness.
/// If you have to deal with const objects then
/// you need to use a QList instead.
///
/// Your objects also do not need to support copy constructors
/// or >= operators. This class will work fine without them
/// as opposed to a plain QList.
///
/// This class also supports automatic deletion of the inseted items.
/// See the setAutoDelete() and autoDelete() members for the
/// description of the feature.
///
/// Typcal usage:
///
/// \verbatim
///   KviPtrList<MyClass> list();
///   list.append(new MyClass());
///   list.append(new MyClass());
///   ...
///   for(MyClass * c = list.first();c;c = list.next())doSomethingWith(c);
///   delete list; // autodelete is set to true in the constructor
/// \endverbatim
///
/// \warning This class is absolutely NOT thread safe. You must
/// protect concurrent access from multiple threads by
/// using an external synchronization tool (such as KviMutex).
///
template<typename T> class KviPtrList
{
	friend class KviPtrListIterator<T>;
protected:
	bool   m_bAutoDelete;   //< do we automatically delete items when they are removed ?

	KviPtrListNode * m_pHead;         //< our list head pointer (NULL if there are no items in the list)
	KviPtrListNode * m_pTail;         //< our list tail
	KviPtrListNode * m_pAux;          //< our iteration pointer

	unsigned int m_uCount;  //< the count of items in the list
protected:
	///
	/// \internal
	///
	/// inserts the item d before the item ref or at the beginning
	/// if ref is not found in the list
	/// also sets the current iteration pointer to the newly inserted item
	///
	void insertBeforeSafe(KviPtrListNode * ref,const T * d)
	{
		m_pAux = ref;
		KviPtrListNode * n = new KviPtrListNode;
		n->m_pPrev = m_pAux->m_pPrev;
		n->m_pNext = m_pAux;
		if(m_pAux->m_pPrev)
		{
			m_pAux->m_pPrev->m_pNext = n;
		} else {
			m_pHead = n;
		}
		m_pAux->m_pPrev = n;
		n->m_pData = (void *)d;
		m_uCount++;
	}

	///
	/// \internal
	///
	/// Grabs the first element from the list src
	/// and puts it as the first element of this list.
	///
	void grabFirstAndPrepend(KviPtrList<T> * src)
	{
		KviPtrListNode * pNewHead = src->m_pHead;
		if(!pNewHead)
			return;

		if(pNewHead->m_pNext)
		{
			src->m_pHead = pNewHead->m_pNext;
			src->m_pHead->m_pPrev = NULL;
		} else {
			src->m_pHead = NULL;
			src->m_pTail = NULL;
		}

		if(m_pHead)
		{
			m_pHead->m_pPrev = pNewHead;
			pNewHead->m_pNext = m_pHead;
			m_pHead = pNewHead;
		} else {
			m_pHead = pNewHead;
			m_pTail = pNewHead;
			m_pHead->m_pNext = NULL;
		}
		m_uCount++;
		src->m_uCount--;
	}

	///
	/// \internal
	///
	/// Removes the current iteration item assuming that it is valid.
	///
	void removeCurrentSafe()
	{
		if(m_pAux->m_pPrev)
			m_pAux->m_pPrev->m_pNext = m_pAux->m_pNext;
		else
			m_pHead = m_pAux->m_pNext;
		if(m_pAux->m_pNext)
			m_pAux->m_pNext->m_pPrev = m_pAux->m_pPrev;
		else
			m_pTail = m_pAux->m_pPrev;
		if(m_bAutoDelete)
			delete ((const T *)(m_pAux->m_pData));
		delete m_pAux;
		m_pAux = NULL;
		m_uCount--;
	}

public:
	///
	/// Inserts the list src inside this list
	/// by respecting the sort order.
	/// The src list elements are removed.
	///
	void merge(KviPtrList<T> * src)
	{
		m_pAux = m_pHead;
		KviPtrListNode * n = src->m_pHead;
		m_uCount += src->m_uCount;
		while(m_pAux && n)
		{
			if(kvi_compare((const T *)(m_pAux->m_pData),(const T *)(n->m_pData)) > 0)
			{
				// our element is greater, n->m_pData goes first
				KviPtrListNode * pNext = n->m_pNext;
				n->m_pPrev = m_pAux->m_pPrev; // his prev becomes 
				n->m_pNext = m_pAux;
				if(m_pAux->m_pPrev)
					m_pAux->m_pPrev->m_pNext = n;
				else
					m_pHead = n;
				m_pAux->m_pPrev = n;
				n = pNext;
			} else {
				// that element is greater
				m_pAux = m_pAux->m_pNext;
			}
		}
		if(n)
		{
			// last items to append
			if(m_pTail)
			{
				m_pTail->m_pNext = n;
				n->m_pPrev = m_pTail;
			} else {
				m_pHead = n;
				m_pTail = n;
				n->m_pPrev = NULL;
			}
			m_pTail = src->m_pTail;
		}

		src->m_pHead = NULL;
		src->m_pTail = NULL;
		src->m_uCount = 0;
	}
	
	void swap(KviPtrList<T> * src)
	{
		KviPtrListNode * n = m_pHead;
		m_pHead = src->m_pHead;
		src->m_pHead = n;
		n = m_pTail;
		m_pTail = src->m_pTail;
		src->m_pTail = n;
		unsigned int uCount = m_uCount;
		m_uCount = src->m_uCount;
		src->m_uCount = uCount;
	}


	///
	/// Sorts this list in ascending order.
	/// There must be an int kvi_compare(const T *p1,const T *p2) function
	/// which returns a value less than, equal to
	/// or greater than zero when the item p1 is considered lower than,
	/// equal to or greater than p2.
	///
	void sort()
	{
		if(m_uCount < 2)return;

		KviPtrList<T> carry;
		KviPtrList<T> tmp[64];
		KviPtrList * fill = &tmp[0];
		KviPtrList * counter;

		do {
			carry.grabFirstAndPrepend(this);
			
			for(counter = &tmp[0];counter != fill && !counter->isEmpty();++counter)
			{
				counter->merge(&carry);
				carry.swap(counter);
			}
			carry.swap(counter);
			if(counter == fill)
				++fill;
		} while(m_uCount > 0);

		for(counter = &tmp[1];counter != fill;++counter)
			counter->merge(counter-1);
		swap(fill-1);
	}

	///
	/// Inserts the item respecting the sorting order inside the list.
	/// The list itself must be already sorted for this to work correctly.
	/// There must be a int kvi_compare(const T *p1,const T * p2)
	/// that returns a value less than, equal to
	/// or greater than zero when the item p1 is considered lower than,
	/// equal to or greater than p2. 
	///
	void inSort(T * t)
	{
		KviPtrListNode * x = m_pHead;
		while(x && (kvi_compare(((T *)x->m_pData),t) > 0))x = x->m_pNext;
		if(!x)append(t);
		else insertBeforeSafe(x,t);
	}

	///
	/// Returns true if the list is empty
	///
	bool isEmpty() const
	{
		return (m_pHead == NULL);
	}

	///
	/// Returns the count of the items in the list
	///
	unsigned int count() const
	{
		return m_uCount;
	}

	///
	/// Sets the iteration pointer to the first item in the list
	/// and returns that item (or 0 if the list is empty)
	///
	T * first()
	{
		if(!m_pHead)return NULL;
		m_pAux = m_pHead;
		return (T *)(m_pAux->m_pData);
	}

	///
	/// Removes the first element from the list
	/// and returns it to the caller. This function
	/// obviously never deletes the item (regadless of autoDeletion()).
	///
	T * takeFirst()
	{
		if(!m_pHead)return NULL;
		T * pData = (T *)m_pHead->m_pData;
		if(m_pHead->m_pNext)
		{
			m_pHead = m_pHead->m_pNext;
			delete m_pHead->m_pPrev;
			m_pHead->m_pPrev = NULL;
		} else {
			delete m_pHead;
			m_pHead = NULL;
			m_pTail = NULL;
		}
		m_uCount--;
		return pData;
	}

	///
	/// Returns an iterator pointing to the first item of the list.
	///
	KviPtrListIterator<T> iteratorAtFirst()
	{
		return KviPtrListIterator<T>(*this,m_pHead);
	}

	///
	/// Sets the iteration pointer to the last item in the list
	/// and returns that item (or 0 if the list is empty)
	///
	T * last()
	{
		if(!m_pTail)return NULL;
		m_pAux = m_pTail;
		return (T *)(m_pAux->m_pData);
	}

	///
	/// Returns an iterator pointing to the first item of the list.
	///
	KviPtrListIterator<T> iteratorAtLast()
	{
		return KviPtrListIterator<T>(*this,m_pTail);
	}

	///
	/// Returns the current iteration item
	/// A call to this function MUST be preceded by a call to
	/// first(),last(),at() or findRef()
	///
	T * current()
	{
		return (T *)(m_pAux->m_pData);
	}

	///
	/// Returns the current iteration item
	/// A call to this function should be preceded by a call to
	/// first(),last(),at() or findRef().
	/// This function will return a NULL pointer if the current
	/// item has been invalidated due to a remove operation.
	///
	T * safeCurrent()
	{
		return m_pAux ? (T *)(m_pAux->m_pData) : NULL;
	}


	///
	/// Returns an iterator pointing to the current item in the list.
	/// A call to this function MUST be preceded by a call to
	/// first(),last(),at() or findRef()
	///
	KviPtrListIterator<T> iteratorAtCurrent()
	{
		return KviPtrListIterator<T>(*this,m_pAux);
	}

	///
	/// Sets the iteration pointer to the next item in the list
	/// and returns that item (or 0 if the end of the list has been reached)
	/// A call to this function MUST be preceded by a call to
	/// first(),last(),at() or findRef()
	///
	T * next()
	{
		m_pAux = m_pAux->m_pNext;
		if(m_pAux)return (T *)(m_pAux->m_pData);
		return NULL;
	}

	///
	/// Sets the iteration pointer to the previous item in the list
	/// and returns that item (or 0 if the beginning of the list has been reached)
	/// A call to this function MUST be preceded by a call to
	/// first(),last(),at() or findRef()
	///
	T * prev()
	{
		m_pAux = m_pAux->m_pPrev;
		if(m_pAux)return (T *)(m_pAux->m_pData);
		return NULL;
	}

	///
	/// Sets the iteration pointer to the nTh item in the list
	/// and returns that item (or 0 if the index is out of range)
	///
	T * at(int idx)
	{
		T * t = first();
		int cnt = 0;
		while(t)
		{
			if(idx == cnt)return t;
			t = next();
			cnt++;
		}
		return 0;
	}

	///
	/// Returns an iterator pointing to the item at the specified index.
	///
	KviPtrListIterator<T> iteratorAt(int idx)
	{
		KviPtrListNode * n = m_pHead;
		int cnt = 0;
		while(n)
		{
			if(idx == cnt)
				return KviPtrListIterator<T>(*this,n);
			n = n->m_pNext;
			cnt++;
		}
		return KviPtrListIterator<T>(*this,NULL);
	}

	///
	/// Sets the iteration pointer to the item with pointer d
	/// and returns its position (zero based index) in the list or -1 if the
	/// item cannot be found
	///
	int findRef(const T * d)
	{
		int ret = 0;
		for(T * t = first();t;t = next())
		{
			if(t == d)return ret;
			ret++;
		}
		return -1;
	}

	///
	/// Returns an iterator pointing to the item with pointer d.
	///
	KviPtrListIterator<T> iteratorAtRef(const T * d)
	{
		KviPtrListNode * n = m_pHead;
		while(n)
		{
			if(n->m_pData == d)
				return KviPtrListIterator<T>(*this,n);
			n = n->m_pNext;
		}
		return KviPtrListIterator<T>(*this,NULL);
	}

	///
	/// Appends an item at the end of the list
	///
	void append(const T * d)
	{
		if(!m_pHead)
		{
			m_pHead = new KviPtrListNode;
			m_pHead->m_pPrev = NULL;
			m_pHead->m_pNext = NULL;
			m_pHead->m_pData = (void *)d;
			m_pTail = m_pHead;
		} else {
			m_pTail->m_pNext = new KviPtrListNode;
			m_pTail->m_pNext->m_pPrev = m_pTail;
			m_pTail->m_pNext->m_pNext = NULL;
			m_pTail->m_pNext->m_pData = (void *)d;
			m_pTail = m_pTail->m_pNext;
		}
		m_uCount++;
	}

	///
	/// Appends all the items from the list l to this list
	///
	void append(KviPtrList<T> * l)
	{
		for(T * t = l->first();t;t = l->next())append(t);
	}

	///
	/// Prepends (inserts in head position) all the items from
	/// the list l to this list
	///
	void prepend(KviPtrList<T> * l)
	{
		for(T * t = l->last();t;t = l->prev())prepend(t);
	}

	///
	/// Inserts the item d in the head position
	///
	void prepend(const T * d)
	{
		if(!m_pHead)
		{
			m_pHead = new KviPtrListNode;
			m_pHead->m_pPrev = NULL;
			m_pHead->m_pNext = NULL;
			m_pHead->m_pData = (void *)d;
			m_pTail = m_pHead;
		} else {
			m_pHead->m_pPrev = new KviPtrListNode;
			m_pHead->m_pPrev->m_pNext = m_pHead;
			m_pHead->m_pPrev->m_pPrev = NULL;
			m_pHead->m_pPrev->m_pData = (void *)d;
			m_pHead =  m_pHead->m_pPrev;
			m_uCount++;
		}
	}

	///
	/// Inserts the item d at the zero-based position
	/// specified by iIndex. If the specified position
	/// is out of the list then the item is appended.
	/// Note that this function costs O(n).
	/// It's really better to use insertAfter() or
	/// insertBefore(), if possible.
	///
	void insert(int iIndex,const T * d)
	{
		m_pAux = m_pHead;
		while(m_pAux && iIndex > 0)
		{
			iIndex--;
			m_pAux = m_pAux->m_pNext;
		}
		if(m_pAux)
			insertBeforeSafe(m_pAux,d);
		else
			append(d);
	}

	///
	/// Removes the firstitem (if any)
	/// the item is deleted if autoDelete() is set to true
	///
	bool removeFirst()
	{
		if(!m_pHead)return false;
		if(m_pHead->m_pNext)
		{
			m_pHead = m_pHead->m_pNext;
			if(m_bAutoDelete)
				delete ((const T *)(m_pHead->m_pPrev->m_pData));
			delete m_pHead->m_pPrev;
			m_pHead->m_pPrev = NULL;
		} else {
			if(m_bAutoDelete)
				delete ((const T *)(m_pHead->m_pData));
			delete m_pHead;
			m_pHead = NULL;
			m_pTail = NULL;
		}
		m_pAux = NULL;
		m_uCount--;
		return true;
	}

	///
	/// Removes the firstitem (if any)
	/// the item is deleted if autoDelete() is set to true
	///
	bool removeLast()
	{
		if(!m_pTail)return false;
		if(m_pTail->m_pPrev)
		{
			m_pTail = m_pTail->m_pPrev;
			if(m_bAutoDelete)
				delete ((const T *)(m_pTail->m_pNext->m_pData));
			delete m_pTail->m_pNext;
			m_pTail->m_pNext = NULL;
		} else {
			if(m_bAutoDelete)
				delete ((const T *)(m_pTail->m_pData));
			delete m_pTail;
			m_pHead = NULL;
			m_pTail = NULL;
		}
		m_pAux = NULL;
		m_uCount--;
		return true;
	}

	///
	/// Removes the item at zero-based position iIndex.
	/// Does nothing and returns false if iIndex is out of the list.
	/// Please note that this function costs O(n).
	///
	bool remove(int iIndex)
	{
		m_pAux = m_pHead;
		while(m_pAux && iIndex > 0)
		{
			iIndex--;
			m_pAux = m_pAux->m_pNext;
		}
		if(!m_pAux)
			return false;
		removeCurrentSafe();
		return true;
	}

	///
	/// Sets the autodelete flag
	/// When this flag is on (default) , all the items
	/// are deleted when removed from the list (or when the list is destroyed
	/// or cleared explicitly)
	///
	void setAutoDelete(bool bAutoDelete)
	{
		m_bAutoDelete = bAutoDelete;
	}

	///
	/// Returns the autodelete flag.
	///
	bool autoDelete()
	{
		return m_bAutoDelete;
	};

	///
	/// Removes all the items from the list 
	/// (the items are deleted if the autoDelete() flag is set to true)
	///
	void clear()
	{
		while(m_pHead)removeFirst();
	}

	///
	/// Removes the current iteration item.
	/// Returns true if the current iteration item was valid (and was removed)
	/// and false otherwise.
	///
	bool removeCurrent()
	{
		if(!m_pAux)
			return false;
		removeCurrentSafe();
		return true;
	}

	///
	/// Removes the item pointed by d (if found in the list)
	/// the item is deleted if the autoDelete() flag is set to true)
	/// Returns true if the item was in the list and false otherwise.
	///
	bool removeRef(const T * d)
	{
		if(findRef(d) == -1)return false;
		removeCurrentSafe();
		return true;
	}

	///
	/// inserts the item d after the item ref or at the end
	/// if ref is not found in the list
	/// also sets the current iteration pointer to the newly inserted item
	///
	void insertAfter(const T * ref,const T * d)
	{
		if(findRef(ref) == -1)
		{
			append(d);
			return;
		}
		KviPtrListNode * n = new KviPtrListNode;
		n->m_pPrev = m_pAux;
		n->m_pNext = m_pAux->m_pNext;
		if(m_pAux->m_pNext)
			m_pAux->m_pNext->m_pPrev = n;
		else
			m_pTail = n;
		m_pAux->m_pNext = n;
		n->m_pData = (void *)d;
		m_uCount++;
	}

	///
	/// inserts the item d before the item ref or at the beginning
	/// if ref is not found in the list
	/// also sets the current iteration pointer to the newly inserted item
	///
	void insertBefore(const T * ref,const T * d)
	{
		if(findRef(ref) == -1)
		{
			prepend(d);
			return;
		}
		KviPtrListNode * n = new KviPtrListNode;
		n->m_pPrev = m_pAux->m_pPrev;
		n->m_pNext = m_pAux;
		if(m_pAux->m_pPrev)
			m_pAux->m_pPrev->m_pNext = n;
		else
			m_pHead = n;
		m_pAux->m_pPrev = n;
		n->m_pData = (void *)d;
		m_uCount++;
	}

	///
	/// Inverts the elements in the list.
	///
	void invert()
	{
		if(!m_pHead)return;
		KviPtrListNode * oldHead = m_pHead;
		KviPtrListNode * oldTail = m_pTail;
		KviPtrListNode * n = m_pHead;
		while(n)
		{
			KviPtrListNode * next = n->m_pNext;
			n->m_pNext = n->m_pPrev;
			n->m_pPrev = next;
			n = next;
		}
		m_pTail = oldHead;
		m_pHead = oldTail;
	}

	///
	/// clears the list and inserts all the items from the list l
	///
	void copyFrom(KviPtrList<T> * l)
	{
		clear();
		for(T * t = l->first();t;t = l->next())append(t);
	}

	///
	/// equivalent to copyFrom(l)
	///
	KviPtrList<T> & operator = (KviPtrList<T> &l)
	{
		copyFrom(&l);
		return *this;
	}

	///
	/// creates a template list
	///
	KviPtrList<T>(bool bAutoDelete = true)
	{
		m_bAutoDelete = bAutoDelete;
		m_pHead = NULL;
		m_pTail = NULL;
		m_uCount = 0;
		m_pAux	= NULL;
	};

	///
	/// destroys the list
	/// if autoDelete() is set to true, all the items are deleted
	///
	virtual ~KviPtrList<T>()
	{
		clear();
	};
};

#define KviPtrListBase KviPtrList

// BROKEN MSVC LINKER
#ifdef COMPILE_ON_WINDOWS
	#include "kvi_string.h"
	template class KVILIB_API KviPtrList<KviStr>;
#endif

#endif //_KVI_LIST_H_
