#ifndef _KVI_PARAMETERLIST_H_
#define _KVI_PARAMETERLIST_H_
//=============================================================================
//
//   File : KviParameterList.h
//   Creation date : Tue Sep 12 2000 18:00:01 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#include "KviHeapObject.h"
#include "KviCString.h"
#include "KviPointerList.h"

#include <QRect>
#include <QPoint>
#include <QSize>

class KVILIB_API KviParameterList : public KviPointerList<KviCString>, public KviHeapObject
{
public:
	KviParameterList();
	KviParameterList(KviCString *p1);
	KviParameterList(KviCString *p1,KviCString *p2);
	KviParameterList(KviCString *p1,KviCString *p2,KviCString *p3);
	KviParameterList(KviCString *p1,KviCString *p2,KviCString *p3,KviCString *p4);
	KviParameterList(KviCString *p1,KviCString *p2,KviCString *p3,KviCString *p4,KviCString *p5);
	KviParameterList(KviCString *p1,KviCString *p2,KviCString *p3,KviCString *p4,KviCString *p5,KviCString *p6);
	KviParameterList(KviCString *p1,KviCString *p2,KviCString *p3,KviCString *p4,KviCString *p5,KviCString *p6,KviCString *p7);
	KviParameterList(const char * paramBuffer);
	virtual ~KviParameterList();
private:
	KviCString m_szEmpty;
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
	KviCString *     safeFirst();
	KviCString *     safeNext();
	const char * safeFirstParam(){ return safeFirst()->ptr(); };
	const char * safeNextParam(){ return safeNext()->ptr(); };

};



#endif //_KVI_PARAMETERLIST_H_
