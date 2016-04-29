#ifndef _KVI_KVS_TYPES_H_
#define _KVI_KVS_TYPES_H_
//=============================================================================
//
//   File : KviKvsTypes.h
//   Creation date : : Sun 29 May 2005 05.25 CEST by Szymon Stefanek
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

#include "kvi_settings.h"
#include "kvi_inttypes.h"

// we want to have 64 bit arithmetics, if possible...

typedef void * kvs_hobject_t;
typedef kvi_i64_t kvs_int_t;
typedef kvi_u64_t kvs_uint_t;
typedef double kvs_real_t;

#endif //!_KVI_KVS_TYPES_H_
