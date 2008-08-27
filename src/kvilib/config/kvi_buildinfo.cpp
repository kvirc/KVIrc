//=============================================================================
//
//   File : kvi_buildinfo.cpp
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

#include "kvi_buildinfo.h"
#include "kvi_sourcesdate.h"

//MSVC can use it!
#include "kvi_sysbuildinfo.h"

//#if (defined(_OS_WIN32_) || defined(Q_OS_WIN32) || defined(Q_OS_WIN32_)) && !defined(MINGW)
//	#include "kvi_wincfg.h"
//#else
//	#include "kvi_sysbuildinfo.h"
//#endif

namespace KviBuildInfo
{
	QString buildDate()
	{
		return QString(KVIRC_BUILD_DATE);
	}

	QString buildSourcesDate()
	{
		return QString(KVI_SOURCES_DATE);
	}

	QString buildCommand()
	{
		return QString(KVIRC_BUILD_COMMAND);
	}

	QString buildFlags()
	{
		return QString(KVIRC_BUILD_FLAGS);
	}

	QString buildSystem()
	{
		return QString(KVIRC_BUILD_SYSTEM);
	}

	QString buildSystemName()
	{
		#ifdef COMPILE_ON_WINDOWS
			return QString();
		#else
			return QString(KVIRC_BUILD_SYSTEM_NAME);
		#endif
	}

	QString buildSystemVersion()
	{
		#ifdef COMPILE_ON_WINDOWS
			return QString();
		#else
			return QString(KVIRC_BUILD_SYSTEM_VERSION);
		#endif
	}

	QString buildCPU()
	{
		return QString(KVIRC_BUILD_CPU);
	}

	QString buildCompiler()
	{
		return QString(KVIRC_BUILD_COMPILER);
	}

	QString buildCompilerFlags()
	{
		QString flags = QString(KVIRC_BUILD_COMPILER_FLAGS);
		if(flags.isEmpty()) return "N/A";
		else return flags;
	}

	QString buildRevision()
	{
		#ifdef KVIRC_BUILD_REVISION
		QString rev = QString(KVIRC_BUILD_REVISION);
		#else
		QString rev;
		#endif
		return rev;
	}
};
