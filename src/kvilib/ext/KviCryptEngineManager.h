#ifndef _KVI_CRYPT_ENGINE_MANAGER_H_
#define _KVI_CRYPT_ENGINE_MANAGER_H_
//=============================================================================
//
//   File : KviCryptEngineManager.h
//   Creation date : Wed Dec 29 2010 22:10:05 CEST by Elvio Basello
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2010 Elvio Basello (hellvis69 at gmail dot com)
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

// this file was originally part of KviCryptEngine.h

#include "kvi_settings.h"
#include "KviPointerHashTable.h"

class KviCryptEngine;
class KviCryptEngineDescription;

#ifdef COMPILE_CRYPT_SUPPORT

class QString;

class KVILIB_API KviCryptEngineManager
{
public:
	KviCryptEngineManager();
	virtual ~KviCryptEngineManager();

private:
	KviPointerHashTable<QString, KviCryptEngineDescription> * m_pEngineDict;

public:
	const KviPointerHashTable<QString, KviCryptEngineDescription> * engineDict() { return m_pEngineDict; };
	void registerEngine(KviCryptEngineDescription * pDesc);
	void unregisterEngine(const QString & szName);
	void unregisterEngines(void * providerHandle);
	//
	// Allocates a crypt engine
	// Please note that the engine may be deleted from outside
	// so you'd better connect the "destroyed" signal
	//
	KviCryptEngine * allocateEngine(const QString & szName);
	void deallocateEngine(KviCryptEngine * pEngine);
};

#endif // COMPILE_CRYPT_SUPPORT
#endif // _KVI_CRYPT_ENGINE_MANAGER_H_
