#ifndef _KVI_BUILDINFO_H_
#define _KVI_BUILDINFO_H_
//=============================================================================
//
//   File : KviBuildInfo.h
//   Creation date : Sat 19 Apr 2008 17:01:57 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2008 Szymon Stefanek <s.stefanek at libero dot it>
//   Copyright (C) 2008 Elvio Basello <hellvis69 at netsons dot org>
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

/**
* \file KviBuildInfo.h
* \author Szymon Stefanek
* \brief Useful functions which returns compilation and runtime info
*/

#include "kvi_settings.h"
#include "kvi_sourcesdate.h"

class QString;
class QStringList;

extern KVILIB_API const QStringList feature_list;

/**
* \namespace KviBuildInfo
* \brief This namespace contains information gathered at KVIrc build time.
*
* This, in fact, is the last time that "cmake" ran and successfully generated the
* kvilib Makefile. Since kvilib is generally built with the KVIrc
* executable, then you can assume that the information returned
* from this namespace strictly applies also to KVIrc.
*/
namespace KviBuildInfo
{
	/**
	* \brief Retrieve the latest kvilib/kvirc build date.
	* \return QString
	*/
	extern KVILIB_API QString buildDate();

	/**
	* \brief Returns the date tag applied to the currently compiled Git sources.
	* \return QString
	*/
	extern KVILIB_API QString buildSourcesDate();

	/**
	* \brief Returns the build command used by cmake to create the KVIrc executable.
	* \return QString
	*/
	extern KVILIB_API QString buildCommand();

	/**
	* \brief Returns the build flags
	*
	* The build flags were worked out by CMake at compilation time.
	* \return QString
	*/
	extern KVILIB_API QString buildFlags();

	/**
	* \brief Returns the name part of the system
	*
	* The system name refers to the one used to build the KVIrc executable.
	* \return QString
	*/
	extern KVILIB_API QString buildSystemName();

	/**
	* \brief Returns a description of the CPU
	*
	* The CPU refers to the one used to build the KVIrc executable.
	* \return QString
	*/
	extern KVILIB_API QString buildCPU();

	/**
	* \brief Returns a description of the compiler
	*
	* The compiler refers to the one used to build the KVIrc executable.
	* \return QString
	*/
	extern KVILIB_API QString buildCompiler();

	/**
	* \brief Returns a description of the compiler flags
	*
	* The flags refer to the ones used to build the KVIrc executable.
	* \return QString
	*/
	extern KVILIB_API QString buildCompilerFlags();

	/**
	* \brief Returns the type of build
	* \return const char *
	*/
	extern KVILIB_API const char * buildType();

	/**
	* \brief Returns the Git revision number of the local repository
	* \return QString
	*/
	extern KVILIB_API QString buildRevision();

	/**
	* \brief Returns the Qt version used to build the KVIrc executable.
	* \return QString
	*/
	extern KVILIB_API QString qtVersion();

	/**
	* \brief Returns the list of features compiled in the KVIrc executable.
	* \return QString
	*/
	extern KVILIB_API QString features();
}

#endif //!_KVI_BUILDINFO_H_
