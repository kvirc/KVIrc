#ifndef _OBJECT_MACROS_H_
#define _OBJECT_MACROS_H_
//=============================================================================
//
//   File : object_macros.h
//   Creation date : Sat Jun 18 2005 15:32:01 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviKvsKernel.h"
#include "KviKvsObject.h"
#include "KviKvsObjectClass.h"
#include "KviKvsObjectController.h"
#include "KviKvsObjectFunctionCall.h"

#define KVSO_DECLARE_OBJECT(__className)                                                     \
	__className(KviKvsObjectClass * pClass, KviKvsObject * pParent, const QString & szName); \
	~__className();                                                                          \
	static void registerSelf();                                                              \
	static void unregisterSelf();

#define SET_OBJECT(__className)                                \
	__className * obj = new __className(parentScriptWidget()); \
	obj->setObjectName(getName());                             \
	setObject(obj, true);

#define KVSO_REGISTER_HANDLER(__className, __szName, __proc) \
	g_pKvs##__className##Class->registerFunctionHandler(__szName, (KviKvsObjectFunctionHandlerProc)(KVI_PTR2MEMBER(__className::__proc)));

// TEST
#define KVSO_REGISTER_HANDLER_BY_NAME(__className, __szName) \
	g_pKvs##__className##Class->registerFunctionHandler(#__szName, (KviKvsObjectFunctionHandlerProc)(KVI_PTR2MEMBER(__className::__szName)));

#define KVSO_REGISTER_STANDARD_NOTHINGRETURN_HANDLER(__className, __szName) \
	g_pKvs##__className##Class->registerStandardNothingReturnFunctionHandler(__szName);

#define KVSO_REGISTER_STANDARD_TRUERETURN_HANDLER(__className, __szName) \
	g_pKvs##__className##Class->registerStandardTrueReturnFunctionHandler(__szName);

#define KVSO_REGISTER_STANDARD_FALSERETURN_HANDLER(__className, __szName) \
	g_pKvs##__className##Class->registerStandardFalseReturnFunctionHandler(__szName);

#define KVSO_BEGIN_REGISTERCLASS(__className, __stringName, __baseClass)                                                                 \
	static KviKvsObjectClass * g_pKvs##__className##Class = nullptr;                                                                     \
	static KviKvsObject * kvs_##__className##_createInstance(KviKvsObjectClass * pClass, KviKvsObject * pParent, const QString & szName) \
	{                                                                                                                                    \
		return new __className(pClass, pParent, szName);                                                                                 \
	}                                                                                                                                    \
	void __className::unregisterSelf()                                                                                                   \
	{                                                                                                                                    \
		delete g_pKvs##__className##Class;                                                                                               \
		g_pKvs##__className##Class = nullptr;                                                                                            \
	}                                                                                                                                    \
	void __className::registerSelf()                                                                                                     \
	{                                                                                                                                    \
		KviKvsObjectClass * base = KviKvsKernel::instance()->objectController()->lookupClass(__baseClass);                               \
		g_pKvs##__className##Class = new KviKvsObjectClass(base, __stringName, kvs_##__className##_createInstance, true);

#define KVSO_END_REGISTERCLASS(__className) \
	}

#define KVSO_BEGIN_CONSTRUCTOR(__className, __baseClass)                                                 \
	__className::__className(KviKvsObjectClass * pClass, KviKvsObject * pParent, const QString & szName) \
	    : __baseClass(pClass, pParent, szName)                                                           \
	{

#define KVSO_END_CONSTRUCTOR(__className) \
	}

#define KVSO_BEGIN_DESTRUCTOR(__className) \
	__className::~__className()            \
	{

#define KVSO_END_DESTRUCTOR(__className) \
	}

#define KVSO_CLASS_FUNCTION(__className, __functionName) \
	bool KvsObject_##__className::__functionName([[maybe_unused]] KviKvsObjectFunctionCall * c)

#define CHECK_INTERNAL_POINTER(__pointer)                                                     \
	if(!__pointer)                                                                            \
	{                                                                                         \
		c->error(__tr2qs_ctx("Internal error: no valid pointer for this object", "objects")); \
		return false;                                                                         \
	}
#define CHECK_HOBJECT_IS_WIDGET(__pObject)                                            \
	if(!__pObject)                                                                    \
	{                                                                                 \
		c->warning(__tr2qs_ctx("Widget parameter is not an object", "objects"));      \
		return true;                                                                  \
	}                                                                                 \
	if(!__pObject->object())                                                          \
	{                                                                                 \
		c->warning(__tr2qs_ctx("Widget parameter is not a valid object", "objects")); \
		return true;                                                                  \
	}                                                                                 \
	if(!__pObject->object()->isWidgetType())                                          \
	{                                                                                 \
		c->warning(__tr2qs_ctx("Widget object required", "objects"));                 \
		return true;                                                                  \
	}
#endif
