#ifndef _KVI_POINTERLIST_H_
#define _KVI_POINTERLIST_H_
//=============================================================================
//
//   File : KviPointerList.h
//   Creation date : Tue Jul 6 1999 14:52:20 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek (pragma at kvirc dot net)
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

//
//   C++ Template based double linked pointer list class
//   Original ss_list.h Created on 10 Dec 2001
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
//   Added to KVIrc on 02 Jan 2008.
//

/**
* \file KviPointerList.h
* \author Szymon Stefanek
* \brief C++ Template based double linked pointer list class
*
* Qt changes the collection classes too much and too frequently.
* I think we need to be independent of that to the maximum degree possible.
* That's why we have our own fast pointer list class.
* This does not depend on Qt AT ALL and has an interface similar
* to the Qt<=3.x series. The pointer lists with the autodelete
* feature was great and I don't completely understand why they have
* been removed from Qt4 in favor of the value based non-autodeleting
* lists... anyway: here we go :)
*
* \def KviPointerListBase Defines KviPointerListBase as KviPointerList
*/

#include "kvi_settings.h"

template <typename T>
class KviPointerList;
template <typename T>
class KviPointerListIterator;

/**
* \class KviPointerListNode
* \brief A KviPointerList node pointers.
*/
class KviPointerListNode
{
public:
	KviPointerListNode * m_pPrev;
	void * m_pData;
	KviPointerListNode * m_pNext;
};

/**
* \class KviPointerListIterator
* \brief A fast KviPointerList iterator.
*
* This class allows traversing the list sequentially.
* Multilpe iterators can traverse the list at the same time.
*
* Iteration example 1:
*
* \verbatim
* KviPointerListIterator<T> it(list);
* for(bool b = it.moveFirst(); b; b = it.moveNext())
* {
*     T * pData = it.data();
*     doSomethingWithData(pData);
* }
* \endverbatim
*
* Iteration example 2:
*
* \verbatim
* KviPointerListIterator<T> it(list);
* if(it.moveFirst())
* {
*     do {
*         T * pData = it.data();
*         doSomethingWithData(pData);
*     } while(it.moveNext());
* }
* \endverbatim
*
* Iteration example 3:
*
* \verbatim
* KviPointerListIterator<T> it(list.iteratorAt(10));
* if(it.isValid())
* {
*    do {
*         T * pData = it.data();
*         doSomethingWithData(pData);
*    while(it.movePrev());
* }
* \endverbatim
*
* Please note that you must NOT remove any item from
* the list when using the iterators. An iterator pointing
* to a removed item will crash your application if you use it.
* The following code will NOT work (and crash):
*
* \verbatim
* KviPointerList<T> l;
* l.append(new KviCString("x"));
* l.append(new KviCString("y"));
* KviPointerListIterator<T> it(l);
* it.moveFirst();
* l.removeFirst();
* KviCString * tmp = it.data(); <-- this will crash
* \endverbatim
*
* In the rare cases in that you need to remove items
* while traversing the list you should put them
* in a temporary list and remove them after the iteration.
*
* I've chosen this way because usually you don't modify
* the list while traversing it and a fix for this
* would add a constant overhead to several list operation.
* You just must take care of it yourself.
*
* \warning This class is not thread safe by itself.
*/
template <typename T>
class KviPointerListIterator
{
protected:
	KviPointerList<T> * m_pList;
	KviPointerListNode * m_pNode;

public:
	/**
	* \brief Creates an iterator copy.
	*
	* The new iterator points exactly to the item pointed by src.
	* \param src The source item to point to
	* \return KviPointerListIterator
	*/
	KviPointerListIterator(const KviPointerListIterator<T> & src)
	{
		m_pList = src.m_pList;
		m_pNode = src.m_pNode;
	}

	/**
	* \brief Creates an iterator for the list l.
	*
	* The iterator points to the first list item, if any.
	* \param l The source list to point to
	* \return KviPointerListIterator
	*/
	KviPointerListIterator(KviPointerList<T> & l)
	{
		m_pList = (KviPointerList<T> *)&l;
		m_pNode = m_pList->m_pHead;
	}

	/**
	* \brief Creates an iterator for the list l.
	*
	* The iterator points to the specified list node.
	* \param l The source list
	* \param pNode The list node to point to
	* \return KviPointerListIterator
	*/
	KviPointerListIterator(KviPointerList<T> & l, KviPointerListNode * pNode)
	{
		m_pList = (KviPointerList<T> *)&l;
		m_pNode = pNode;
	}

	/**
	* \brief Creates an iterator copy.
	*
	* The new iterator points exactly to the item pointed by src.
	* \param src The source item to copy
	* \return void
	*/
	void operator=(const KviPointerListIterator<T> & src)
	{
		m_pList = src.m_pList;
		m_pNode = src.m_pNode;
	}

public:
	/**
	* \brief Moves the iterator to the first element of the list.
	*
	* Returns true in case of success or false if the list is empty.
	* \return bool
	*/
	bool moveFirst()
	{
		m_pNode = m_pList->m_pHead;
		return m_pNode != nullptr;
	}

	/**
	* \brief Moves the iterator to the last element of the list.
	*
	* Returns true in case of success or false if the list is empty.
	* \return bool
	*/
	bool moveLast()
	{
		m_pNode = m_pList->m_pTail;
		return m_pNode != nullptr;
	}

	/**
	* \brief Moves the iterator to the next element of the list.
	*
	* The iterator must be actually valid for this function to work.
	* Returns true in case of success or false if there is no next item.
	* \return bool
	*/
	bool moveNext()
	{
		if(!m_pNode)
			return false;
		m_pNode = m_pNode->m_pNext;
		return m_pNode != nullptr;
	}

	/**
	* \brief Moves the iterator to the next element of the list.
	*
	* The iterator must be actually valid for this operator to work.
	* Returns true in case of success or false if there is no next item.
	* This is just a convenient alias to moveNext().
	* \return bool
	*/
	bool operator++()
	{
		if(!m_pNode)
			return false;
		m_pNode = m_pNode->m_pNext;
		return m_pNode != nullptr;
	}

	/**
	* \brief Moves the iterator to the previous element of the list.
	*
	* The iterator must be actually valid for this function to work.
	* Returns true in case of success or false if there is no previous
	* item.
	* \return bool
	*/
	bool movePrev()
	{
		if(!m_pNode)
			return false;
		m_pNode = m_pNode->m_pPrev;
		return m_pNode != nullptr;
	}

	/**
	* \brief Moves the iterator to the previous element of the list.
	*
	* The iterator must be actually valid for this operator to work.
	* Returns true in case of success or false if there is no previous
	* item.
	* This is just a convenient alias to movePrev().
	* \return bool
	*/
	bool operator--()
	{
		if(!m_pNode)
			return false;
		m_pNode = m_pNode->m_pPrev;
		return m_pNode != nullptr;
	}

	/**
	* \brief Returns the value pointed by the iterator.
	*
	* If the iterator is not valid, returns nullptr.
	* \return T *
	*/
	T * current()
	{
		return m_pNode ? (T *)(m_pNode->m_pData) : nullptr;
	}

	/**
	* \brief Returns the value pointed by the iterator.
	*
	* If the iterator is not valid, returns nullptr.
	* This is just an alias to current().
	* \return T *
	*/
	T * operator*()
	{
		return m_pNode ? (T *)(m_pNode->m_pData) : nullptr;
	}

	/**
	* \brief Returns true if this iterator points to a valid element
	*
	* Returns false otherwise.
	* \return bool
	*/
	bool isValid()
	{
		return m_pNode != nullptr;
	}
};

/**
* \class KviPointerList
* \brief A template double linked list of pointers.
*
* The main advantage of this type of list is speed.
* Insertion of pointers is very fast when compared to the typical "copy
* constructor" call used in the "plain type" template list implementations.
*
* Iterating over pointers is also very fast and this class contains an
* internal iterator that allows to write loops in a compact and clean way.
* See the first(), next(), current() and findRef() functions for the
* description of this feature.
*
* There is also a non-const external iterator that you can use to traverse
* the list concurrently.
* There is no const iterator (and no const access methods) since the list
* provides the autoDelete() method which vould implicitly violate
* constness.
* If you have to deal with const objects then you need to use a QList
* instead.
*
* Your objects also do not need to support copy constructors or >=
* operators.
* This class will work fine without them as opposed to a plain QList.
*
* This class also supports automatic deletion of the inseted items.
* See the setAutoDelete() and autoDelete() members for the description of
* the feature.
*
* Typcal usage:
*
* \verbatim
*   KviPointerList<MyClass> list();
*   list.append(new MyClass());
*   list.append(new MyClass());
*   ...
*   for(MyClass * c = list.first(); c; c = list.next()) doSomethingWith(c);
*   delete list; // autodelete is set to true in the constructor
* \endverbatim
*
* \warning This class is absolutely NOT thread safe. You must protect
* concurrent access from multiple threads by using an external
* synchronization tool (such as KviMutex).
*/
template <typename T>
class KviPointerList
{
	friend class KviPointerListIterator<T>;

protected:
	bool m_bAutoDelete; //< do we automatically delete items when they are removed ?

	KviPointerListNode * m_pHead; //< our list head pointer (nullptr if there are no items in the list)
	KviPointerListNode * m_pTail; //< our list tail
	KviPointerListNode * m_pAux;  //< our iteration pointer

	unsigned int m_uCount; //< the count of items in the list
protected:
	/**
	* \brief Inserts the item d before the item ref
	*
	* If ref is not found in the list, it inserts d at the beginning
	* Also sets the current iteration pointer to the newly inserted
	* item
	* \param ref The source list node
	* \param d The item to insert
	* \return void
	*/
	void insertBeforeSafe(KviPointerListNode * ref, const T * d)
	{
		m_pAux = ref;
		KviPointerListNode * n = new KviPointerListNode;
		n->m_pPrev = m_pAux->m_pPrev;
		n->m_pNext = m_pAux;
		if(m_pAux->m_pPrev)
		{
			m_pAux->m_pPrev->m_pNext = n;
		}
		else
		{
			m_pHead = n;
		}
		m_pAux->m_pPrev = n;
		n->m_pData = (void *)d;
		m_uCount++;
	}

	/**
	* \brief Grabs the first element from the list src and puts it as the first element of this list.
	* \param src The source list
	* \return void
	*/
	void grabFirstAndPrepend(KviPointerList<T> * src)
	{
		KviPointerListNode * pNewHead = src->m_pHead;
		if(!pNewHead)
			return;

		if(pNewHead->m_pNext)
		{
			src->m_pHead = pNewHead->m_pNext;
			src->m_pHead->m_pPrev = nullptr;
		}
		else
		{
			src->m_pHead = nullptr;
			src->m_pTail = nullptr;
		}

		if(m_pHead)
		{
			m_pHead->m_pPrev = pNewHead;
			pNewHead->m_pNext = m_pHead;
			m_pHead = pNewHead;
		}
		else
		{
			m_pHead = pNewHead;
			m_pTail = pNewHead;
			m_pHead->m_pNext = nullptr;
		}
		m_uCount++;
		src->m_uCount--;
	}

	/**
	* \brief Removes the current iteration item assuming that it is valid.
	* \return void
	*/
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
		const T * pAuxData = (const T *)(m_pAux->m_pData);
		delete m_pAux;
		m_pAux = nullptr;
		m_uCount--;
		if(m_bAutoDelete)
			delete pAuxData; // this can cause recursion, so do it at the end
	}

public:
	/**
	* \brief Inserts the list src inside this list
	*
	* It respects the sort order.
	* The src list elements are removed.
	* \param src The list to insert
	* \return void
	*/
	void merge(KviPointerList<T> * src)
	{
		m_pAux = m_pHead;
		KviPointerListNode * n = src->m_pHead;
		m_uCount += src->m_uCount;
		while(m_pAux && n)
		{
			if(kvi_compare((const T *)(m_pAux->m_pData), (const T *)(n->m_pData)) > 0)
			{
				// our element is greater, n->m_pData goes first
				KviPointerListNode * pNext = n->m_pNext;
				n->m_pPrev = m_pAux->m_pPrev; // his prev becomes
				n->m_pNext = m_pAux;
				if(m_pAux->m_pPrev)
					m_pAux->m_pPrev->m_pNext = n;
				else
					m_pHead = n;
				m_pAux->m_pPrev = n;
				n = pNext;
			}
			else
			{
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
			}
			else
			{
				m_pHead = n;
				m_pTail = n;
				n->m_pPrev = nullptr;
			}
			m_pTail = src->m_pTail;
		}

		src->m_pHead = nullptr;
		src->m_pTail = nullptr;
		src->m_uCount = 0;
	}

	/**
	* \brief Swap the lists
	* \param src The list to swap with
	* \return void
	*/
	void swap(KviPointerList<T> * src)
	{
		KviPointerListNode * n = m_pHead;
		m_pHead = src->m_pHead;
		src->m_pHead = n;
		n = m_pTail;
		m_pTail = src->m_pTail;
		src->m_pTail = n;
		unsigned int uCount = m_uCount;
		m_uCount = src->m_uCount;
		src->m_uCount = uCount;
	}

	/**
	* \brief Sorts this list in ascending order.
	*
	* There must be an int kvi_compare(const T *p1, const T *p2)
	* function which returns a value less than, equal to or greater
	* than zero when the item p1 is considered lower than, equal to or
	* greater than p2.
	* \return void
	*/
	void sort()
	{
		if(m_uCount < 2)
			return;

		KviPointerList<T> carry;
		KviPointerList<T> tmp[64];
		KviPointerList * fill = &tmp[0];
		KviPointerList * counter;

		do
		{
			carry.grabFirstAndPrepend(this);

			for(counter = &tmp[0]; counter != fill && !counter->isEmpty(); ++counter)
			{
				counter->merge(&carry);
				carry.swap(counter);
			}
			carry.swap(counter);
			if(counter == fill)
				++fill;
		} while(m_uCount > 0);

		for(counter = &tmp[1]; counter != fill; ++counter)
			counter->merge(counter - 1);
		swap(fill - 1);
	}

	/**
	* \brief Inserts the item respecting the sorting order inside the list.
	*
	* The list itself must be already sorted for this to work
	* correctly.
	* There must be a int kvi_compare(const T *p1, const T * p2) that
	* returns a value less than, equal to or greater than zero when the
	* item p1 is considered lower than, equal to or greater than p2.
	* \param t The item to insert
	* \return void
	*/
	void inSort(T * t)
	{
		KviPointerListNode * x = m_pHead;
		while(x && (kvi_compare(((T *)x->m_pData), t) > 0))
			x = x->m_pNext;
		if(!x)
			append(t);
		else
			insertBeforeSafe(x, t);
	}

	/**
	* \brief Returns true if the list is empty
	* \return bool
	*/
	bool isEmpty() const
	{
		return (m_pHead == nullptr);
	}

	/**
	* \brief Returns the count of the items in the list
	* \return unsigned int
	*/
	unsigned int count() const
	{
		return m_uCount;
	}

	/**
	* \brief Returns the first item in the list
	*
	* Sets the iteration pointer to the first item in the list and
	* returns that item (or 0 if the list is empty)
	* \return T *
	*/
	T * first()
	{
		if(!m_pHead)
		{
			m_pAux = nullptr;
			return nullptr;
		}
		m_pAux = m_pHead;
		return (T *)(m_pAux->m_pData);
	}

	/**
	* \brief Removes the first element from the list
	*
	* It returns the item to the caller. This function obviously never
	* deletes the item (regadless of autoDeletion()).
	* \return T *
	*/
	T * takeFirst()
	{
		if(!m_pHead)
			return nullptr;
		T * pData = (T *)m_pHead->m_pData;
		if(m_pHead->m_pNext)
		{
			m_pHead = m_pHead->m_pNext;
			delete m_pHead->m_pPrev;
			m_pHead->m_pPrev = nullptr;
		}
		else
		{
			delete m_pHead;
			m_pHead = nullptr;
			m_pTail = nullptr;
		}
		m_pAux = nullptr;
		m_uCount--;
		return pData;
	}

	/**
	* \brief Removes the last item (if any) and returns it.  This function obviously never
	* deletes the item (regadless of autoDeletion()).
	*/
	T * takeLast()
	{
		if(!m_pTail)
			return nullptr;
		T * pData = (T *)m_pTail->m_pData;
		if(m_pTail->m_pPrev)
		{
			m_pTail = m_pTail->m_pPrev;
			delete m_pTail->m_pNext;
			m_pTail->m_pNext = nullptr;
		}
		else
		{
			delete m_pTail;
			m_pHead = nullptr;
			m_pTail = nullptr;
		}
		m_pAux = nullptr;
		m_uCount--;
		return pData;
	}

	/**
	* \brief Returns an iterator pointing to the first item of the list.
	* \return KviPointerListIterator<T>
	*/
	KviPointerListIterator<T> iteratorAtFirst()
	{
		return KviPointerListIterator<T>(*this, m_pHead);
	}

	/**
	* \brief Returns the last item in the list
	*
	* Sets the iteration pointer to the last item in the list
	* and returns that item (or 0 if the list is empty)
	* \return T *
	*/
	T * last()
	{
		if(!m_pTail)
		{
			m_pAux = nullptr;
			return nullptr;
		}
		m_pAux = m_pTail;
		return (T *)(m_pAux->m_pData);
	}

	/**
	* \brief Returns an iterator pointing to the first item of the list.
	* \return KviPointerListIterator<T>
	*/
	KviPointerListIterator<T> iteratorAtLast()
	{
		return KviPointerListIterator<T>(*this, m_pTail);
	}

	/**
	* \brief Returns the current iteration item
	*
	* A call to this function MUST be preceded by a call to
	* first(),last(),at() or findRef()
	* \return T *
	*/
	T * current()
	{
		return (T *)(m_pAux->m_pData);
	}

	/**
	* \brief Returns the current iteration item
	*
	* A call to this function should be preceded by a call to
	* first(),last(),at() or findRef().
	* This function will return a nullptr if the current item has
	* been invalidated due to a remove operation.
	* \return T *
	*/
	T * safeCurrent()
	{
		return m_pAux ? (T *)(m_pAux->m_pData) : nullptr;
	}

	/**
	* \brief Returns an iterator pointing to the current item in the list.
	*
	* A call to this function MUST be preceded by a call to
	* first(),last(),at() or findRef()
	* \return KviPointerListIterator<T>
	*/
	KviPointerListIterator<T> iteratorAtCurrent()
	{
		return KviPointerListIterator<T>(*this, m_pAux);
	}

	/**
	* \brief Returns the next item in the list
	*
	* Sets the iteration pointer to the next item in the list and
	* returns that item (or nullptr if the end of the list has been reached)
	* A call to this function MUST be preceded by a _successfull_ call
	* to first(),last(),at() or findRef().
	* \return T *
	*/
	T * next()
	{
		if(!m_pAux)
			return nullptr;
		m_pAux = m_pAux->m_pNext;
		if(m_pAux)
			return (T *)(m_pAux->m_pData);
		return nullptr;
	}

	/**
	* \brief Returns the previous item in the list
	*
	* Sets the iteration pointer to the previous item in the list and
	* returns that item (or 0 if the beginning of the list has been
	* reached).
	* A call to this function MUST be preceded by a _successfull_ call
	* to first(),last(),at() or findRef()
	* \return T *
	*/
	T * prev()
	{
		if(!m_pAux)
			return nullptr;
		m_pAux = m_pAux->m_pPrev;
		if(m_pAux)
			return (T *)(m_pAux->m_pData);
		return nullptr;
	}

	/**
	* \brief Returns the item at index position
	*
	* Sets the iteration pointer to the nTh item in the list
	* and returns that item (or 0 if the index is out of range)
	* \param idx The index of the element to return
	* \return T *
	*/
	T * at(int idx)
	{
		T * t = first();
		int cnt = 0;
		while(t)
		{
			if(idx == cnt)
				return t;
			t = next();
			cnt++;
		}
		return nullptr;
	}

	/**
	* \brief Returns an iterator pointing to the item at the specified index.
	* \param idx The index of the element to return
	* \return KviPointerListIterator<T>
	*/
	KviPointerListIterator<T> iteratorAt(int idx)
	{
		KviPointerListNode * n = m_pHead;
		int cnt = 0;
		while(n)
		{
			if(idx == cnt)
				return KviPointerListIterator<T>(*this, n);
			n = n->m_pNext;
			cnt++;
		}
		return KviPointerListIterator<T>(*this, nullptr);
	}

	/**
	* \brief Returns the position of an item
	*
	* Sets the iteration pointer to the item with pointer d
	* and returns its position (zero based index) in the list or -1 if
	* the item cannot be found
	* \param d The element to find
	* \return int
	*/
	int findRef(const T * d)
	{
		int ret = 0;
		for(T * t = first(); t; t = next())
		{
			if(t == d)
				return ret;
			ret++;
		}
		return -1;
	}

	/**
	* \brief Returns an iterator pointing to the item with pointer d.
	* \param d The element to find
	* \return KviPointerListIterator<T>
	*/
	KviPointerListIterator<T> iteratorAtRef(const T * d)
	{
		KviPointerListNode * n = m_pHead;
		while(n)
		{
			if(n->m_pData == d)
				return KviPointerListIterator<T>(*this, n);
			n = n->m_pNext;
		}
		return KviPointerListIterator<T>(*this, nullptr);
	}

	/**
	* \brief Appends an item at the end of the list
	* \param d The item to append
	* \return void
	*/
	void append(const T * d)
	{
		if(!m_pHead)
		{
			m_pHead = new KviPointerListNode;
			m_pHead->m_pPrev = nullptr;
			m_pHead->m_pNext = nullptr;
			m_pHead->m_pData = (void *)d;
			m_pTail = m_pHead;
		}
		else
		{
			m_pTail->m_pNext = new KviPointerListNode;
			m_pTail->m_pNext->m_pPrev = m_pTail;
			m_pTail->m_pNext->m_pNext = nullptr;
			m_pTail->m_pNext->m_pData = (void *)d;
			m_pTail = m_pTail->m_pNext;
		}
		m_uCount++;
	}

	/**
	* \brief Appends all the items from the list l to this list
	* \param l The source list where to get items
	* \return void
	*/
	void append(KviPointerList<T> * l)
	{
		for(T * t = l->first(); t; t = l->next())
			append(t);
	}

	/**
	* \brief Prepends all the items from the list l to this list
	* \param l The source list where to get items
	* \return void
	*/
	void prepend(KviPointerList<T> * l)
	{
		for(T * t = l->last(); t; t = l->prev())
			prepend(t);
	}

	/**
	* \brief Inserts the item d in the head position
	* \param d The element to insert
	* \return void
	*/
	void prepend(const T * d)
	{
		if(!m_pHead)
		{
			m_pHead = new KviPointerListNode;
			m_pHead->m_pPrev = nullptr;
			m_pHead->m_pNext = nullptr;
			m_pHead->m_pData = (void *)d;
			m_pTail = m_pHead;
		}
		else
		{
			m_pHead->m_pPrev = new KviPointerListNode;
			m_pHead->m_pPrev->m_pNext = m_pHead;
			m_pHead->m_pPrev->m_pPrev = nullptr;
			m_pHead->m_pPrev->m_pData = (void *)d;
			m_pHead = m_pHead->m_pPrev;
			m_uCount++;
		}
	}

	/**
	* \brief Inserts the item d at the position specified by iIndex.
	*
	* The position is zero-based. If the specified position is out of
	* the list then the item is appended.
	* Note that this function costs O(n).
	* It's really better to use insertAfter() or insertBefore(), if
	* possible.
	* \param iIndex The index where to insert the item
	* \param d The item to insert
	* \return void
	*/
	void insert(int iIndex, const T * d)
	{
		m_pAux = m_pHead;
		while(m_pAux && iIndex > 0)
		{
			iIndex--;
			m_pAux = m_pAux->m_pNext;
		}
		if(m_pAux)
			insertBeforeSafe(m_pAux, d);
		else
			append(d);
	}

	/**
	* \brief Removes the first item (if any)
	*
	* The item is deleted if autoDelete() is set to true
	* \return bool
	*/
	bool removeFirst()
	{
		if(!m_pHead)
			return false;
		const T * pAuxData;
		if(m_pHead->m_pNext)
		{
			m_pHead = m_pHead->m_pNext;
			pAuxData = (const T *)(m_pHead->m_pPrev->m_pData);
			delete m_pHead->m_pPrev;
			m_pHead->m_pPrev = nullptr;
		}
		else
		{
			pAuxData = (const T *)(m_pHead->m_pData);
			delete m_pHead;
			m_pHead = nullptr;
			m_pTail = nullptr;
		}
		m_pAux = nullptr;
		m_uCount--;
		if(m_bAutoDelete)
			delete pAuxData;
		return true;
	}

	/**
	* \brief Removes the firstitem (if any)
	*
	* The item is deleted if autoDelete() is set to true
	* \return bool
	*/
	bool removeLast()
	{
		if(!m_pTail)
			return false;
		const T * pAuxData;
		if(m_pTail->m_pPrev)
		{
			m_pTail = m_pTail->m_pPrev;
			pAuxData = (const T *)(m_pTail->m_pNext->m_pData);
			delete m_pTail->m_pNext;
			m_pTail->m_pNext = nullptr;
		}
		else
		{
			pAuxData = (const T *)(m_pTail->m_pData);
			delete m_pTail;
			m_pHead = nullptr;
			m_pTail = nullptr;
		}
		m_pAux = nullptr;
		m_uCount--;
		if(m_bAutoDelete)
			delete pAuxData;
		return true;
	}

	/**
	* \brief Removes the item at zero-based position iIndex.
	*
	* Does nothing and returns false if iIndex is out of the list.
	* Please note that this function costs O(n).
	* \param iIndex The index where to remove the item
	* \return bool
	*/
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

	/**
	* \brief Sets the autodelete flag
	*
	* When this flag is on (default), all the items are deleted when
	* removed from the list (or when the list is destroyed or cleared
	* explicitly)
	* \param bAutoDelete The state of the autoDelete()
	* \return void
	*/
	void setAutoDelete(bool bAutoDelete)
	{
		m_bAutoDelete = bAutoDelete;
	}

	/**
	* \brief Returns the autodelete flag.
	* \return bool
	*/
	bool autoDelete()
	{
		return m_bAutoDelete;
	};

	/**
	* \brief Removes all the items from the list
	*
	* The items are deleted if the autoDelete() flag is set to true
	* \return void
	*/
	void clear()
	{
		while(m_pHead)
			removeFirst();
	}

	/**
	* \brief Removes the current iteration item.
	*
	* Returns true if the current iteration item was valid (and was
	* removed) and false otherwise.
	* \return bool
	*/
	bool removeCurrent()
	{
		if(!m_pAux)
			return false;
		removeCurrentSafe();
		return true;
	}

	/**
	* \brief Removes the item pointed by d (if found in the list)
	*
	* The item is deleted if the autoDelete() flag is set to true)
	* Returns true if the item was in the list and false otherwise.
	* \param d The pointer to the item to delete
	* \return bool
	*/
	bool removeRef(const T * d)
	{
		if(findRef(d) == -1)
			return false;
		removeCurrentSafe();
		return true;
	}

	/**
	* \brief Inserts the item d after the item ref
	*
	* If ref is not found in the list, the item is inserted at the end
	* Also sets the current iteration pointer to the newly inserted
	* item
	* \param ref The index item
	* \param d The item to insert after ref
	* \return void
	*/
	void insertAfter(const T * ref, const T * d)
	{
		if(findRef(ref) == -1)
		{
			append(d);
			return;
		}
		KviPointerListNode * n = new KviPointerListNode;
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

	/**
	* \brief Inserts the item d before the item ref
	*
	* If ref is not found in the list, the item is inserted at the
	* beginning.
	* Also sets the current iteration pointer to the newly inserted
	* item
	* \param ref The index item
	* \param d The item to insert before ref
	* \return void
	*/
	void insertBefore(const T * ref, const T * d)
	{
		if(findRef(ref) == -1)
		{
			prepend(d);
			return;
		}
		KviPointerListNode * n = new KviPointerListNode;
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

	/**
	* \brief Inverts the elements in the list.
	* \return void
	*/
	void invert()
	{
		if(!m_pHead)
			return;
		KviPointerListNode * oldHead = m_pHead;
		KviPointerListNode * oldTail = m_pTail;
		KviPointerListNode * n = m_pHead;
		while(n)
		{
			KviPointerListNode * next = n->m_pNext;
			n->m_pNext = n->m_pPrev;
			n->m_pPrev = next;
			n = next;
		}
		m_pTail = oldHead;
		m_pHead = oldTail;
	}

	/**
	* \brief Clears the list and inserts all the items from the list l
	* \param l The source list to copy from
	* \return void
	*/
	void copyFrom(KviPointerList<T> * l)
	{
		clear();
		for(T * t = l->first(); t; t = l->next())
			append(t);
	}

	/**
	* \brief Clears the list and inserts all the items from the list l
	*
	* This is just an alias to copyFrom(l)
	* \param l The source list to copy from
	* \return KviPointerList<T> &
	*/
	KviPointerList<T> & operator=(KviPointerList<T> & l)
	{
		copyFrom(&l);
		return *this;
	}

	/**
	* \brief Creates a template list
	* \param bAutoDelete The state of autoDelete()
	* \return KviPointerList<T>
	*/
	KviPointerList<T>(bool bAutoDelete = true)
	{
		m_bAutoDelete = bAutoDelete;
		m_pHead = nullptr;
		m_pTail = nullptr;
		m_uCount = 0;
		m_pAux = nullptr;
	};

	/**
	* \brief Destroys the list
	*
	* If autoDelete() is set to true, all the items are deleted
	*/
	virtual ~KviPointerList<T>()
	{
		clear();
	};
};

#define KviPointerListBase KviPointerList

// BROKEN MSVC LINKER
#ifdef COMPILE_ON_WINDOWS
#include "KviCString.h"
template class KVILIB_API KviPointerList<KviCString>;
#endif

// Provide a default implementation of kvi_compare()
template <typename T>
int kvi_compare(const T * p1, const T * p2)
{
	return p1 > p2; // just compare pointers
}

#endif //_KVI_POINTERLIST_H_
