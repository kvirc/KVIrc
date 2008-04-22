#ifndef _KVI_BUILDINFO_H_
#define _KVI_BUILDINFO_H_
//=============================================================================
//
//   File : kvi_buildinfo.h
//   Created on Sat 19 Apr 2008 17:01:57 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2008 Szymon Stefanek <s.stefanek at libero dot it>
//   Copyright (C) 2008 Elvio Basello <hellvis69 at netsons dot org>
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
#include "kvi_sourcesdate.h"

#include <QString>

///
/// This namespace contains informations gathered at kvirc build time.
/// This, in fact, is the last time that "cmake" ran and successfully generated the
/// kvilib Makefile. Since kvilib is generally built with the kvirc
/// executable, then you can assume that the information returned
/// from this namespace strictly applies also to kvirc.
///
namespace KviBuildInfo
{
	///
	/// Retrieve the latest kvilib/kvirc build date.
	///
	extern KVILIB_API QString buildDate();
	///
	/// Returns the date tag applied to the currently compiled SVN sources.
	///
	extern KVILIB_API QString buildSourcesDate();
	///
	/// Returns the build command used by cmake to create the current kvirc executable.
	///
	extern KVILIB_API QString buildCommand();
	///
	/// Returns some of the build flags that were worked out by cmake at
	/// compilation time.
	///
	extern KVILIB_API QString buildFlags();
	///
	/// Returns a description of the system that the currently running
	/// executable was built on.
	///
	extern KVILIB_API QString buildSystem();
	///
	/// Returns the name part of the system that the currently running
	/// executable was built on.
	///
	extern KVILIB_API QString buildSystemName();
	///
	/// Returns the version part of the system that the currently running
	/// executable was built on.
	///
	extern KVILIB_API QString buildSystemVersion();
	///
	/// Returns a description of the CPU that the currently running
	/// executable was built on.
	///
	extern KVILIB_API QString buildCPU();
	///
	/// Returns a description of the compiler that the currently running
	/// executable was built with.
	///
	extern KVILIB_API QString buildCompiler();
	///
	/// Returns a description of the compiler flags that the currently running
	/// executable was built with.
	///
	extern KVILIB_API QString buildCompilerFlags();
	///
	/// Returns the SVN revision number of the local repository
	///
	extern KVILIB_API QString buildRevision();
};

#endif //!_KVI_BUILDINFO_H_
