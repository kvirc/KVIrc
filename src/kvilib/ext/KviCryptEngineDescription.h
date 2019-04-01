#ifndef _KVI_CRYPT_ENGINE_DESC_H_
#define _KVI_CRYPT_ENGINE_DESC_H_
//=============================================================================
//
//   File : KviCryptEngineDescription.h
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

//
// this file was originally part of KviCryptEngine.h
//

#include "kvi_settings.h"

#ifdef COMPILE_CRYPT_SUPPORT

#include "KviCryptEngine.h"
#include "KviHeapObject.h"

#include <QString>

class KVILIB_API KviCryptEngineDescription : public KviHeapObject
{
public:
        KviCryptEngineDescription() = default;
        virtual ~KviCryptEngineDescription() = default;

public:
	QString m_szName;                                      /**< engine name */
	QString m_szDescription;                               /**< details */
	QString m_szAuthor;                                    /**< algorithm author */
	int m_iFlags = 0;                                      /**< properties */
	crypt_engine_allocator_func m_allocFunc = nullptr;     /**< engine allocator */
	crypt_engine_deallocator_func m_deallocFunc = nullptr; /**< deallocation function (if called from outside the origin module) */
	void * m_providerHandle = nullptr;                     /**< used to identify the provider module */
};

#endif //COMPILE_CRYPT_SUPPORT
#endif // _KVI_CRYPT_ENGINE_DESC_H_
