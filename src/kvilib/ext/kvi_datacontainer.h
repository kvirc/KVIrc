#ifndef _KVI_VARCACHE_H_
#define _KVI_VARCACHE_H_

//
//   File : kvi_varcache.h
//   Creation date : Mon Feb 15 1999 01:10:34 by Szymon Stefanek
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

#include "kvi_settings.h"
#include "kvi_heapobject.h"

#include <qasciidict.h>

#include "kvi_string.h"
#include "kvi_stringarray.h"

typedef QAsciiDict<KviStr> KviDictionary;
typedef QAsciiDictIterator<KviStr> KviDictionaryIterator;

class KVILIB_API KviDataContainer : public KviHeapObject
{
public:
	KviDataContainer(bool bGlobal = false);
	~KviDataContainer();
protected:
	QAsciiDict<KviStr>          * m_pVariableDict;
	QAsciiDict<KviDictionary>   * m_pDictionaryDict;
	QAsciiDict<KviStringArray>  * m_pArrayDict;
public:
	KviStr * lookupVariable(const char * varName,bool bReadOnly);
	void removeVariable(const char * varName);

	KviDictionary * lookupDictionary(const char * dictName,bool bReadOnly);
	KviStr * lookupDictionaryVariable(const char * dictName,const char * varName,bool bReadOnly);
	void insertDictionary(const char * dictName,KviDictionary * dict);
	void removeDictionary(const char * dictName);
	void removeDictionaryNoDelete(const char * dictName);
	void removeDictionaryVariable(const char * dictName,const char * varName);

	KviStringArray * lookupArray(const char * arrayName,bool bReadOnly);
	KviStr * lookupArrayVariable(const char * arrayName,unsigned int uIdx,bool bReadOnly);
	void removeArrayNoDelete(const char * arrayName);
	void removeArray(const char * arrayName);
	void removeArrayVariable(const char * arrayName,unsigned int uIdx);
	void insertArray(const char * arrayName,KviStringArray * a);
};


#endif //!_KVI_VARCACHE_H_INCLUDED_
