#ifndef _KVI_KVS_OBJECT_CONTROLLER_H_
#define _KVI_KVS_OBJECT_CONTROLLER_H_
//=============================================================================
//
//   File : kvi_kvs_object_controller.h
//   Created on Sun 24 Apr 2005 05:23:04 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005 Szymon Stefanek <pragma at kvirc dot net>
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
//=============================================================================


#include "kvi_settings.h"

#include "kvi_qstring.h"
#include "kvi_list.h"

#include "kvi_dict.h"
#include "kvi_ptrdict.h"

#include "kvi_kvs_object.h"
#include "kvi_kvs_object_class.h"

class KVIRC_API KviKvsObjectController
{
	friend class KviKvsObject;
	friend class KviKvsObjectClass;
public:
	KviKvsObjectController();
	~KviKvsObjectController();
protected:
	KviPtrList<KviKvsObject>      * m_pTopLevelObjectList;
	KviPtrDict<KviKvsObject>        * m_pObjectDict;
	KviDict<KviKvsObjectClass>      * m_pClassDict;
	KviKvsObjectClass             * m_pObjectClass; //base class
protected:
	// the classes and the objects register themselves with the controller
	void registerObject(KviKvsObject *pObject);
	void unregisterObject(KviKvsObject *pObject);
	void registerClass(KviKvsObjectClass *pClass);
	void unregisterClass(KviKvsObjectClass *pClass);
public:
	void init();
	void deleteClass(KviKvsObjectClass * pClass);
	void clearInstances();
	void clearUserClasses();
	void flushUserClasses();
	void killAllObjectsWithClass(KviKvsObjectClass * pClass);
	KviKvsObjectClass * lookupClass(const QString &szClass,bool bBuiltinOnly = false);
	KviKvsObject * lookupObject(kvs_hobject_t hObject){ return m_pObjectDict->find(hObject); };
	KviPtrDict<KviKvsObject> * objectDict(){ return m_pObjectDict; };
	KviDict<KviKvsObjectClass> * classDict(){ return m_pClassDict; };
};

#endif //!_KVI_KVS_OBJECT_CONTROLLER_H_
