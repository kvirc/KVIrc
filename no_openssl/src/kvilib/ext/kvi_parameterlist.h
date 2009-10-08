#ifndef _KVI_PARAMETERLIST_H_
#define _KVI_PARAMETERLIST_H_
//=============================================================================
//
//   File : kvi_parameterlist.h
//   Creation date : Tue Sep 12 2000 18:00:01 by Szymon Stefanek
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
#include "kvi_heapobject.h"
#include "kvi_string.h"
#include "kvi_pointerlist.h"

#include <QRect>
#include <QPoint>
#include <QSize>

class KVILIB_API KviParameterList : public KviPointerList<KviStr>, public KviHeapObject
{
public:
	KviParameterList();
	KviParameterList(KviStr *p1);
	KviParameterList(KviStr *p1,KviStr *p2);
	KviParameterList(KviStr *p1,KviStr *p2,KviStr *p3);
	KviParameterList(KviStr *p1,KviStr *p2,KviStr *p3,KviStr *p4);
	KviParameterList(KviStr *p1,KviStr *p2,KviStr *p3,KviStr *p4,KviStr *p5);
	KviParameterList(KviStr *p1,KviStr *p2,KviStr *p3,KviStr *p4,KviStr *p5,KviStr *p6);
	KviParameterList(KviStr *p1,KviStr *p2,KviStr *p3,KviStr *p4,KviStr *p5,KviStr *p6,KviStr *p7);
	KviParameterList(const char * paramBuffer);
	virtual ~KviParameterList();
private:
	KviStr m_szEmpty;
public:
	void         init(){ (void)first(); };
	// These functions have to be called when the
	// current() points to the FIRST item that has
	// to be interpreted as Bool,Int,UInt,Rect etc...
	// At the call exit the current() points
	// to the first item that was NOT used by the call
	bool         getBool();
	int          getInt(bool * bOk = 0);
	unsigned int getUInt(bool * bOk = 0);
	QRect        getRect(bool * bOk = 0);
	QPoint       getPoint(bool * bOk = 0);
	QSize        getSize(bool * bOk = 0);
	KviStr *     safeFirst();
	KviStr *     safeNext();
	const char * safeFirstParam(){ return safeFirst()->ptr(); };
	const char * safeNextParam(){ return safeNext()->ptr(); };

};



#endif //_KVI_PARAMETERLIST_H_
