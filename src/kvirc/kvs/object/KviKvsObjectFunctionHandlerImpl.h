#ifndef _KVI_KVS_OBJECT_FUNCTIONHANDLERIMPL_H_
#define _KVI_KVS_OBJECT_FUNCTIONHANDLERIMPL_H_
//=============================================================================
//
//   File : KviKvsObjectFunctionHandlerImpl.h
//   Creation date : Sun 24 Apr 2005 04:01:14 by Szymon Stefanek
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
#include "KviKvsObject.h"
#include "KviKvsObjectClass.h"
#include "KviKvsObjectFunctionHandler.h"
#include "KviQString.h"

class KviKvsObjectClass;
class KviKvsScript;
class KviKvsObjectFunctionCall;

class KVIRC_API KviKvsObjectCoreCallFunctionHandler : public KviKvsObjectFunctionHandler
{
	friend class KviKvsObject;
	friend class KviKvsObjectClass;

protected:
	KviKvsObjectCoreCallFunctionHandler(KviKvsObjectFunctionHandlerProc pProc, unsigned int uFlags = 0);
	KviKvsObjectCoreCallFunctionHandler(const KviKvsObjectCoreCallFunctionHandler & h);

public:
	virtual ~KviKvsObjectCoreCallFunctionHandler();

protected:
	KviKvsObjectFunctionHandlerProc m_pProc;

protected:
	virtual KviKvsObjectFunctionHandler * clone();
	virtual bool call(KviKvsObject * pObject, KviKvsObjectFunctionCall * pCall);
};

class KVIRC_API KviKvsObjectScriptFunctionHandler : public KviKvsObjectFunctionHandler
{
	friend class KviKvsObject;
	friend class KviKvsObjectClass;

protected:
	KviKvsObjectScriptFunctionHandler(const QString & szScriptContext, const QString & szCode, const QString & szReminder, unsigned int uFlags = 0);
	KviKvsObjectScriptFunctionHandler(const KviKvsObjectScriptFunctionHandler & h);

public:
	virtual ~KviKvsObjectScriptFunctionHandler();

protected:
	KviKvsScript * m_pHandler;
	QString m_szReminder;

protected:
	virtual bool isScriptHandler();
	virtual QString scriptHandlerCode();
	virtual QString reminder();
	virtual void setReminder(const QString & szReminder) { m_szReminder = szReminder; };

	virtual KviKvsObjectFunctionHandler * clone();
	virtual bool call(KviKvsObject * pObject, KviKvsObjectFunctionCall * pCall);
};

class KVIRC_API KviKvsObjectStandardTrueReturnFunctionHandler : public KviKvsObjectFunctionHandler
{
	friend class KviKvsObject;
	friend class KviKvsObjectClass;

protected:
	KviKvsObjectStandardTrueReturnFunctionHandler()
	    : KviKvsObjectFunctionHandler(){};

public:
	virtual ~KviKvsObjectStandardTrueReturnFunctionHandler(){};

protected:
	virtual KviKvsObjectFunctionHandler * clone();
	virtual bool call(KviKvsObject * pObject, KviKvsObjectFunctionCall * pCall);
};

class KVIRC_API KviKvsObjectStandardFalseReturnFunctionHandler : public KviKvsObjectFunctionHandler
{
	friend class KviKvsObject;
	friend class KviKvsObjectClass;

protected:
	KviKvsObjectStandardFalseReturnFunctionHandler()
	    : KviKvsObjectFunctionHandler(){};

public:
	virtual ~KviKvsObjectStandardFalseReturnFunctionHandler(){};

protected:
	virtual KviKvsObjectFunctionHandler * clone();
	virtual bool call(KviKvsObject * pObject, KviKvsObjectFunctionCall * pCall);
};

class KVIRC_API KviKvsObjectStandardNothingReturnFunctionHandler : public KviKvsObjectFunctionHandler
{
	friend class KviKvsObject;
	friend class KviKvsObjectClass;

protected:
	KviKvsObjectStandardNothingReturnFunctionHandler()
	    : KviKvsObjectFunctionHandler(){};

public:
	virtual ~KviKvsObjectStandardNothingReturnFunctionHandler(){};

protected:
	virtual KviKvsObjectFunctionHandler * clone();
	virtual bool call(KviKvsObject * pObject, KviKvsObjectFunctionCall * pCall);
};

#endif //!_KVI_KVS_OBJECT_FUNCTIONHANDLERIMPL_H_
