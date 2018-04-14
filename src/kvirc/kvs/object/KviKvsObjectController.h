#ifndef _KVI_KVS_OBJECT_CONTROLLER_H_
#define _KVI_KVS_OBJECT_CONTROLLER_H_
//=============================================================================
//
//   File : KviKvsObjectController.h
//   Creation date : Sun 24 Apr 2005 05:23:04 by Szymon Stefanek
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

#include "KviKvsObject.h"
#include "KviKvsObjectClass.h"

class KVIRC_API KviKvsObjectController
{
	friend class KviKvsObject;
	friend class KviKvsObjectClass;

public:
	KviKvsObjectController();
	~KviKvsObjectController();

protected:
	KviPointerList<KviKvsObject> * m_pTopLevelObjectList;
	KviPointerHashTable<void *, KviKvsObject> * m_pObjectDict;
	KviPointerHashTable<QString, KviKvsObjectClass> * m_pClassDict;
	KviKvsObjectClass * m_pObjectClass = nullptr; //base class
protected:
	// the classes and the objects register themselves with the controller
	void registerObject(KviKvsObject * pObject);
	void unregisterObject(KviKvsObject * pObject);
	void registerClass(KviKvsObjectClass * pClass);
	void unregisterClass(KviKvsObjectClass * pClass);

public:
	void init();
	void deleteClass(KviKvsObjectClass * pClass);
	void clearInstances();
	void clearUserClasses();
	void flushUserClasses();
	void killAllObjectsWithClass(KviKvsObjectClass * pClass);
	KviKvsObjectClass * lookupClass(const QString & szClass, bool bBuiltinOnly = false);
	KviKvsObject * lookupObject(kvs_hobject_t hObject) { return m_pObjectDict->find(hObject); };
	KviPointerHashTable<void *, KviKvsObject> * objectDict() { return m_pObjectDict; };
	KviPointerHashTable<QString, KviKvsObjectClass> * classDict() { return m_pClassDict; };
};

#endif //!_KVI_KVS_OBJECT_CONTROLLER_H_
