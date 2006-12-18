//
//   File : kvi_varcache.cpp
//   Creation date : Mon Feb 15 1999 01:11:40 by Szymon Stefanek
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

#define __KVILIB__


#include "kvi_datacontainer.h"

//#define _KVI_DEBUG_CHECK_RANGE_
#include "kvi_debug.h"



KviDataContainer::KviDataContainer(bool bGlobal)
{
// #warning "HAS IT TO BE CASE INSENSITIVE ?"
	m_pVariableDict = new QAsciiDict<KviStr>(bGlobal ? 43 : 11,false,true);
	m_pVariableDict->setAutoDelete(true);
	m_pDictionaryDict = new QAsciiDict<KviDictionary>(bGlobal ? 17 : 3,false,true);
	m_pDictionaryDict->setAutoDelete(true);
	m_pArrayDict = new QAsciiDict<KviStringArray>(bGlobal ? 31 : 5,false,true);
	m_pArrayDict->setAutoDelete(false);
}


KviDataContainer::~KviDataContainer()
{
	delete m_pArrayDict;
	delete m_pVariableDict;
	delete m_pDictionaryDict;
}

KviStr * KviDataContainer::lookupVariable(const char * varName,bool bReadOnly)
{
	KviStr * ret = m_pVariableDict->find(varName);
	if(!ret)
	{
		if(bReadOnly)return 0;
		ret = new KviStr();
		m_pVariableDict->insert(varName,ret);
	}
	return ret;
}

void KviDataContainer::removeVariable(const char * varName)
{
	m_pVariableDict->remove(varName);
}

KviDictionary * KviDataContainer::lookupDictionary(const char * dictName,bool bReadOnly)
{
	KviDictionary * ret = m_pDictionaryDict->find(dictName);
	if(!ret)
	{
		if(bReadOnly)return 0;
		ret = new KviDictionary(17,false,true);
		ret->setAutoDelete(true);
		m_pDictionaryDict->insert(dictName,ret);
	}
	return ret;
}


void KviDataContainer::insertDictionary(const char * dictName,KviDictionary * dict)
{
	m_pDictionaryDict->replace(dictName,dict);
}

KviStr * KviDataContainer::lookupDictionaryVariable(const char * dictName,const char * varName,bool bReadOnly)
{
	KviDictionary * dict = lookupDictionary(dictName,bReadOnly);
	if(!dict)return 0;

	KviStr * ret = dict->find(varName);
	if(!ret)
	{
		if(bReadOnly)return 0;
		ret = new KviStr();
		dict->insert(varName,ret);
	}
	return ret;
}

void KviDataContainer::removeDictionaryVariable(const char * dictName,const char * varName)
{
	KviDictionary * dict = m_pDictionaryDict->find(dictName);
	if(!dict)return;
	dict->remove(varName);
	if(dict->count() == 0)m_pDictionaryDict->remove(dictName);
}

void KviDataContainer::removeDictionary(const char * dictName)
{
	m_pDictionaryDict->remove(dictName);
}

void KviDataContainer::removeDictionaryNoDelete(const char * dictName)
{
	m_pDictionaryDict->setAutoDelete(false);
	m_pDictionaryDict->remove(dictName);
	m_pDictionaryDict->setAutoDelete(true);
}


KviStringArray * KviDataContainer::lookupArray(const char * arrayName,bool bReadOnly)
{
	KviStringArray * ret = m_pArrayDict->find(arrayName);
	if(!ret)
	{
		if(bReadOnly)return 0;
		ret = new KviStringArray();
		m_pArrayDict->insert(arrayName,ret);
	}
	return ret;
}

void KviDataContainer::removeArrayNoDelete(const char * arrayName)
{
	m_pArrayDict->setAutoDelete(false);
	m_pArrayDict->remove(arrayName);
	m_pArrayDict->setAutoDelete(true);
}

KviStr * KviDataContainer::lookupArrayVariable(const char * arrayName,unsigned int uIdx,bool bReadOnly)
{
	KviStringArray * a = lookupArray(arrayName,bReadOnly);
	if(!a)return 0;
	KviStr * r = a->at(uIdx);
	if(r)return r;
	if(bReadOnly)return 0;
	r = new KviStr();
	a->insert(uIdx,r);
	return r;
}

void KviDataContainer::insertArray(const char * arrayName,KviStringArray * a)
{
	m_pArrayDict->replace(arrayName,a);
}

void KviDataContainer::removeArray(const char * arrayName)
{
	m_pArrayDict->remove(arrayName);
}


void KviDataContainer::removeArrayVariable(const char * arrayName,unsigned int uIdx)
{
	KviStringArray * a = m_pArrayDict->find(arrayName);
	if(!a)return;
	a->remove(uIdx);
	if(a->isEmpty())m_pArrayDict->remove(arrayName);
}
