//=============================================================================
//
//   File : kvi_parameterlist.cpp
//   Creation date : Tue Sep 12 2000 18:14:01 by Szymon Stefanek
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

#include "kvi_parameterlist.h"

KviParameterList::KviParameterList()
: KviPointerList<KviStr>()
{
	setAutoDelete(true);
}

KviParameterList::KviParameterList(KviStr *p1)
: KviPointerList<KviStr>()
{
	setAutoDelete(true);
	append(p1);
}

KviParameterList::KviParameterList(KviStr *p1,KviStr *p2)
: KviPointerList<KviStr>()
{
	setAutoDelete(true);
	append(p1);
	append(p2);
}

KviParameterList::KviParameterList(KviStr *p1,KviStr *p2,KviStr *p3)
: KviPointerList<KviStr>()
{
	setAutoDelete(true);
	append(p1);
	append(p2);
	append(p3);
}

KviParameterList::KviParameterList(KviStr *p1,KviStr *p2,KviStr *p3,KviStr *p4)
: KviPointerList<KviStr>()
{
	setAutoDelete(true);
	append(p1);
	append(p2);
	append(p3);
	append(p4);
}

KviParameterList::KviParameterList(KviStr *p1,KviStr *p2,KviStr *p3,KviStr *p4,KviStr *p5)
: KviPointerList<KviStr>()
{
	setAutoDelete(true);
	append(p1);
	append(p2);
	append(p3);
	append(p4);
	append(p5);
}

KviParameterList::KviParameterList(KviStr *p1,KviStr *p2,KviStr *p3,KviStr *p4,KviStr *p5,KviStr *p6)
: KviPointerList<KviStr>()
{
	setAutoDelete(true);
	append(p1);
	append(p2);
	append(p3);
	append(p4);
	append(p5);
	append(p6);
}


KviParameterList::KviParameterList(KviStr *p1,KviStr *p2,KviStr *p3,KviStr *p4,KviStr *p5,KviStr *p6,KviStr *p7)
: KviPointerList<KviStr>()
{
	setAutoDelete(true);
	append(p1);
	append(p2);
	append(p3);
	append(p4);
	append(p5);
	append(p6);
	append(p7);
}

KviParameterList::KviParameterList(const char *paramBuffer)
: KviPointerList<KviStr>()
{
	setAutoDelete(true);
	while(*paramBuffer)
	{
		KviStr * pStr = new KviStr();
		paramBuffer = kvi_extractToken(*pStr,paramBuffer);
		append(pStr);
	}
}

KviParameterList::~KviParameterList()
{
}

KviStr * KviParameterList::safeFirst()
{
	KviStr * f= first();
	return f ? f : &m_szEmpty;
}

KviStr * KviParameterList::safeNext()
{
	KviStr * f = next();
	return f ? f : &m_szEmpty;
}


bool KviParameterList::getBool()
{
	KviStr * par = current();
	(void)next();
	if(par)
	{
		if(kvi_strEqualCS(par->ptr(),"0"))return false;
	}
	return true;  // default
}

int KviParameterList::getInt(bool * bOk)
{
	KviStr * par = current();
	(void)next();
	if(par)
	{
		return par->toInt(bOk);
	}
	if(bOk)*bOk = false;
	return 0;
}

unsigned int KviParameterList::getUInt(bool * bOk)
{
	KviStr * par = current();
	(void)next();
	if(par)
	{
		return par->toUInt(bOk);
	}
	if(bOk)*bOk = false;
	return 0;
}

QRect KviParameterList::getRect(bool * bOk)
{
	int val[4];
	for(int i=0;i<4;i++)
	{
		KviStr * pszv = current();
		(void)next();
		if(!pszv)
		{
			if(bOk)*bOk = false;
			return QRect(); // invalid
		}
		bool mybOk;
		val[i] = pszv->toInt(&mybOk);
		if(!mybOk)
		{
			if(bOk)*bOk = false;
			return QRect(); // invalid
		}
	}
	if(bOk)*bOk = true;
	return QRect(val[0],val[1],val[2],val[3]);
}

QPoint KviParameterList::getPoint(bool * bOk)
{
	int val[2];
	for(int i=0;i<2;i++)
	{
		KviStr * pszv = current();
		(void)next();
		if(!pszv)
		{
			if(bOk)*bOk = false;
			return QPoint(); // invalid
		}
		bool mybOk;
		val[i] = pszv->toInt(&mybOk);
		if(!mybOk)
		{
			if(bOk)*bOk = false;
			return QPoint(); // invalid
		}
	}
	if(bOk)*bOk = true;
	return QPoint(val[0],val[1]);
}

QSize KviParameterList::getSize(bool * bOk)
{
	int val[2];
	for(int i=0;i<2;i++)
	{
		KviStr * pszv = current();
		(void)next();
		if(!pszv)
		{
			if(bOk)*bOk = false;
			return QSize(); // invalid
		}
		bool mybOk;
		val[i] = pszv->toInt(&mybOk);
		if(!mybOk)
		{
			if(bOk)*bOk = false;
			return QSize(); // invalid
		}
	}
	if(bOk)*bOk = true;
	return QSize(val[0],val[1]);
}

//#ifdef COMPILE_ON_WINDOWS
//
//	#include "kvi_malloc.h"
//
//	void * KviParameterList::operator new(size_t tSize)
//	{
//		return kvi_malloc(tSize);
//	}
//
//	void KviParameterList::operator delete(void * p)
//	{
//		kvi_free(p);
//	}
//#endif
