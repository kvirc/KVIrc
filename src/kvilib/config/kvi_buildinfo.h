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

namespace KviBuildInfo
{
	QString buildDate();
	QString buildSourcesDate();
	QString buildCommand();
	QString buildFlags();
	QString buildSystem();
	QString buildCPU();
	QString buildCompiler();
	QString buildCompilerFlags();
};

#endif //!_KVI_BUILDINFO_H_
