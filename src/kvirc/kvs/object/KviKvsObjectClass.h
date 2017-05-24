#ifndef _KVI_KVS_OBJECTCLASS_H_
#define _KVI_KVS_OBJECTCLASS_H_
//=============================================================================
//
//   File : KviKvsObjectClass.h
//   Creation date : Sat 23 Apr 2005 20:31:32 by Szymon Stefanek
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
#include "KviPointerList.h"

#include "KviPointerHashTable.h"

#include "KviKvsObjectFunctionHandler.h"

class KviKvsObject;
class KviKvsObjectClass;
class KviKvsRunTimeContext;
class KviKvsVariantList;
class KviKvsVariant;
// Object allocation function
// parameters are: the class, the parent object (eventually 0), the object name (eventually empty)
typedef KviKvsObject * (*KviKvsObjectAllocateInstanceProc)(KviKvsObjectClass *, KviKvsObject *, const QString &);

// An object function callback
typedef bool (KviKvsObject::*KviKvsObjectFunctionHandlerProc)(KviKvsObjectFunctionCall * pCall);

// The descriptor of a kvirc object class

class KVIRC_API KviKvsObjectClass
{
	friend class KviKvsObject;
	friend class KviKvsObjectController;

public:
	KviKvsObjectClass(
	    KviKvsObjectClass * pParent,           // parent class
	    const QString & szName,                // class name
	    KviKvsObjectAllocateInstanceProc proc, // instance allocation proc
	    bool bBuiltin = true                   // this is a builtin or script based class ?
	    );
	~KviKvsObjectClass();

protected:
	KviKvsObjectClass * m_pParentClass;                                              // the parent (base) class
	QString m_szName;                                                                // the class name
	bool m_bBuiltin;                                                                 // is this a builtin or script based class ?
	KviPointerHashTable<QString, KviKvsObjectFunctionHandler> * m_pFunctionHandlers; // all our function handlers
	KviPointerList<KviKvsObjectClass> * m_pChildClasses;                             //
	KviKvsObjectAllocateInstanceProc m_allocProc;
	bool m_bDirty; // not yet flushed to disk (only for not builtin classes)
protected:
	void registerChildClass(KviKvsObjectClass * pClass);
	void unregisterChildClass(KviKvsObjectClass * pClass);
	KviPointerHashTable<QString, KviKvsObjectFunctionHandler> * functionHandlers() { return m_pFunctionHandlers; };
public:
	void clearDirtyFlag() { m_bDirty = false; };
	bool isDirty() { return m_bDirty; };
	bool isBuiltin() { return m_bBuiltin; };
	bool isScriptHandler(const QString & szFunctionName)
	{
		KviKvsObjectFunctionHandler * pFunctionHandler = m_pFunctionHandlers->find(szFunctionName);
		if(pFunctionHandler)
			return pFunctionHandler->isScriptHandler();
		else
			return false;
	};
	void setReminder(const QString & szReminder, KviKvsObjectFunctionHandler * h);
	QString reminder(KviKvsObjectFunctionHandler * h);

	const QString & name() { return m_szName; };
	KviKvsObjectClass * parentClass() { return m_pParentClass; };
	// pProc CAN'T be zero here!
	void registerFunctionHandler(const QString & szFunctionName, KviKvsObjectFunctionHandlerProc pProc, unsigned int uFlags = 0);
	void registerFunctionHandler(const QString & szFunctionName, const QString & szBuffer, const QString & szReminder, unsigned int uFlags = 0);

	// registers an empty handler that returns "nothing"
	void registerStandardNothingReturnFunctionHandler(const QString & szFunc);
	// registers an empty handler that returns $true
	void registerStandardTrueReturnFunctionHandler(const QString & szFunc);
	// registers an empty handler that returns $false
	void registerStandardFalseReturnFunctionHandler(const QString & szFunc);

	KviKvsObjectFunctionHandler * lookupFunctionHandler(const QString & szFunc) { return m_pFunctionHandlers->find(szFunc); };
	KviKvsObject * allocateInstance(KviKvsObject * pParent, const QString & szName, KviKvsRunTimeContext * pContext, KviKvsVariantList * pParams);

	bool save(const QString & szFileName);
	static bool load(const QString & szFileName);
	void getFunctionCode(QString & szCode, KviKvsObjectFunctionHandler & h);
	KviPointerHashTable<QString, KviKvsObjectFunctionHandler> * getHandlers() { return m_pFunctionHandlers; };
};

#endif //!_KVI_KVS_OBJECTCLASS_H_
