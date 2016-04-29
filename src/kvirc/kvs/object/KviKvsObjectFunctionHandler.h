#ifndef _KVI_KVS_OBJECT_FUNCTIONHANDLER_H_
#define _KVI_KVS_OBJECT_FUNCTIONHANDLER_H_
//=============================================================================
//
//   File : KviKvsObjectFunctionHandler.h
//   Creation date : Sat 23 Apr 2005 21:53:23 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_settings.h"
#include "KviQString.h"

class KviKvsObjectClass;
class KviKvsObjectFunctionCall;
class KviKvsObject;

class KVIRC_API KviKvsObjectFunctionHandler
{
	friend class KviKvsObject;
	friend class KviKvsObjectClass;

public:
	enum Flags
	{
		Internal = 1
	};

protected:
	unsigned int m_uFlags;
	bool m_bClone;

protected:
	KviKvsObjectFunctionHandler(unsigned int uFlags = 0);

public:
	virtual ~KviKvsObjectFunctionHandler();

public:
	void setClone(bool bCl) { m_bClone = bCl; };
	bool isClone();
	unsigned int flags() { return m_uFlags; };
protected:
	virtual bool isScriptHandler();
	virtual QString scriptHandlerCode();
	virtual QString reminder();
	virtual void setReminder(const QString & szReminder);
	virtual KviKvsObjectFunctionHandler * clone() = 0;
	virtual bool call(KviKvsObject * pObject, KviKvsObjectFunctionCall * pCall) = 0;
};

#endif //!_KVI_KVS_OBJECT_FUNCTIONHANDLER_H_
