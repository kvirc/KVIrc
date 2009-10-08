#ifndef _KVI_MODULEEXTENSION_H_
#define _KVI_MODULEEXTENSION_H_
//=============================================================================
//
//   File : kvi_moduleextension.h
//   Creation date : Tue Sep 10 01:16:24 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2008 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_string.h"
#include "kvi_pointerlist.h"
#include "kvi_heapobject.h"
#include "kvi_pointerhashtable.h"
#include "kvi_pointerhashtable.h"

#include <QPixmap>
#include <QVariant>

class KviWindow;
class KviModule;
class KviModuleExtension;
class KviModuleExtensionDescriptor;

typedef struct _KviModuleExtensionAllocStructTag
{
	KviModuleExtensionDescriptor * pDescriptor; // module extension that this alloc routine refers to
	KviWindow                    * pWindow;     // may be 0!
	KviPointerHashTable<QString,QVariant>              * pParams;     // parameter dict (may be 0!)
	void                         * pSpecial;    // special parameter passed to the alloc routine, may be 0
} KviModuleExtensionAllocStruct;

typedef KviModuleExtension * (*KviModuleExtensionAllocRoutine)(KviModuleExtensionAllocStruct *);

class KVIRC_API KviModuleExtensionDescriptor
{
	friend class KviModuleExtension;
	friend class KviModuleExtensionManager; // only the manager allocates and deallocates these structures
protected:
	KviModuleExtensionDescriptor(KviModule * m,const KviStr &szType,const KviStr &szName,const QString &szVisibleName,KviModuleExtensionAllocRoutine r,const QPixmap &pix);
public: // this has to be public because of QPtrList... but should be protected
	~KviModuleExtensionDescriptor();
private:
	int                              m_iId;            // unique id assigned at creation time
	KviStr                           m_szType;         // name of the service (toolbar , crypt engine...)
	KviStr                           m_szName;         // name of the extension
	QString                          m_szVisibleName;  // name that is VISIBLE and possibly translated
	KviStr                           m_szAuthor;       // Author (visible)
	KviStr                           m_szDescription;  // Description (visible!)
	int                              m_iFlags;         // Flags (0 if not applicable)
	QPixmap                        * m_pIcon;          // Icon (may be null!)
	KviModuleExtensionAllocRoutine   m_allocRoutine;
	KviPointerList<KviModuleExtension> * m_pObjectList;

	KviModule                      * m_pModule;        // module pointer
public:
	// pParams ownership is NOT taken
	KviModuleExtension * allocate(KviWindow * pWnd = 0,KviPointerHashTable<QString,QVariant> * pParams = 0,void * pSpecial = 0);

	int id(){ return m_iId; };
	KviModule * module(){ return m_pModule; };
	const KviStr &type(){ return m_szType; };
	const KviStr &name(){ return m_szName; };
	const QString &visibleName(){ return m_szVisibleName; };
	const KviStr &author(){ return m_szAuthor; };
	const KviStr &description(){ return m_szDescription; };
	const QPixmap * icon(){ return m_pIcon; };
	int flags(){ return m_iFlags; };

	void setAuthor(const KviStr &szAuthor){ m_szAuthor = szAuthor; };
	void setDescription(const KviStr &szDescription){ m_szDescription = szDescription; };
	void setVisibleName(const KviStr &szVisibleName){ m_szVisibleName = szVisibleName; };
	void setFlags(int iFlags){ m_iFlags = iFlags; };
	void setIcon(const QPixmap &pix);

protected:
	void registerObject(KviModuleExtension * e);
	void unregisterObject(KviModuleExtension * e);
};

typedef KviPointerList<KviModuleExtensionDescriptor> KviModuleExtensionDescriptorList;

class KviModuleExtensionManager;

extern KVIRC_API KviModuleExtensionManager * g_pModuleExtensionManager;

class KVIRC_API KviModuleExtensionManager
{
	friend class KviModule;
	friend class KviApp;
protected:
	KviModuleExtensionManager(); // KviApp calls this
	~KviModuleExtensionManager(); // and this
protected:
	KviPointerHashTable<const char *,KviModuleExtensionDescriptorList> * m_pExtensionDict;
protected:
	// Only KviModule can call this
	KviModuleExtensionDescriptor * registerExtension(KviModule * m,const KviStr &szType,const KviStr &szName,const QString &szVisibleName,KviModuleExtensionAllocRoutine r,const QPixmap &icon);
	void unregisterExtensionsByModule(KviModule * m);
public:
	KviModuleExtensionDescriptor * findExtensionDescriptor(const KviStr &szType,const KviStr &szName);
	static KviModuleExtensionManager * instance(){ return g_pModuleExtensionManager; };
	KviModuleExtensionDescriptorList * getExtensionList(const KviStr &szType);
	KviModuleExtension * allocateExtension(const KviStr &szType,const KviStr &szName,KviWindow * pWnd = 0,KviPointerHashTable<QString,QVariant> * pParams = 0,void * pSpecial = 0,const QString &preloadModule = QString());
	KviModuleExtension * allocateExtension(const KviStr &szType,int id,KviWindow * pWnd = 0,KviPointerHashTable<QString,QVariant> * pParams = 0,void * pSpecial = 0,const QString &preloadModule = QString());
private:
	KviModuleExtensionDescriptorList * allocateExtensionGetDescriptorList(const KviStr &szType,const QString &preloadModule);
};



class KVIRC_API KviModuleExtension : public KviHeapObject
{
public:
	KviModuleExtension(KviModuleExtensionDescriptor * d);
	virtual ~KviModuleExtension();
private:
	KviModuleExtensionDescriptor * m_pDescriptor;
public:
	static KviModuleExtensionManager * manager(){ return g_pModuleExtensionManager; };
	KviModuleExtensionDescriptor * descriptor(){ return m_pDescriptor; };
	// A module extension MUST implement die() as "delete this" <-- FIXME: this should be no longer necessary with KviHeapObject
	virtual void die() = 0;
};



#endif //_KVI_MODULEEXTENSION_H_
