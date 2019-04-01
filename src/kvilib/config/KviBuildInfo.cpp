//=============================================================================
//
//   File : KviBuildInfo.cpp
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

#include "KviBuildInfo.h"
#include "kvi_sourcesdate.h"
#include "kvi_sysbuildinfo.h"

#include <QString>
#include <QStringList>

const QStringList feature_list{
	"IRC",
#ifdef COMPILE_DEBUG_MODE
	"Debug",
#endif
#ifdef COMPILE_IPV6_SUPPORT
	"IPv6",
#endif
#ifdef COMPILE_CRYPT_SUPPORT
	"Crypt",
#endif
#ifdef COMPILE_SSL_SUPPORT
	"SSL",
#endif
#ifdef COMPILE_GET_INTERFACE_ADDRESS
	"IfAddr",
#endif
#ifndef COMPILE_NO_IPC
	"IPC",
#endif
#ifdef COMPILE_KDE_SUPPORT
	"KDE",
#endif
#ifdef COMPILE_OSS_SUPPORT
	"OSS",
#endif
#ifdef COMPILE_ESD_SUPPORT
	"ESD",
#endif
#ifdef COMPILE_AUDIOFILE_SUPPORT
	"Audiofile",
#endif
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	"Transparency",
#endif
#ifdef COMPILE_PHONON_SUPPORT
	"Phonon",
#endif
#ifdef COMPILE_WEBKIT_SUPPORT
	"Webkit",
#endif
#ifndef COMPILE_DISABLE_DCC_VIDEO
	"DCCVideo",
#endif
#ifndef COMPILE_DISABLE_DCC_VOICE
	"DCCVoice",
#endif
#ifdef COMPILE_DCC_CANVAS
	"DCCCanvas",
#endif
#ifdef COMPILE_PERL_SUPPORT
	"Perl",
#endif
#ifdef COMPILE_PYTHON_SUPPORT
	"Python",
#endif
#ifdef COMPILE_ENCHANT_SUPPORT
	"Enchant",
#endif
#ifdef COMPILE_ENABLE_GTKSTYLE
	"GTK",
#endif
	"Qt5",
	"KVS"
};

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

	QString buildSystemName()
	{
#ifdef COMPILE_ON_WINDOWS
		return QString();
#else
		return QString(KVIRC_BUILD_SYSTEM_NAME);
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
		if(flags.isEmpty())
			return QString("N/A");
		else
			return flags;
	}

	const char * buildType()
	{
#ifdef COMPILE_DEBUG_MODE
		return "debug";
#else
		return "release";
#endif
	}

	QString buildRevision()
	{
#ifdef KVIRC_BUILD_REVISION
		return QString(KVIRC_BUILD_REVISION);
#else
		return QString();
#endif
	}

	QString qtVersion()
	{
		return QString(QT_VERSION_STR);
	}

	QString features()
	{
		return feature_list.join(", ");
	}
}
