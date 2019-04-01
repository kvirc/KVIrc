#ifndef _PYTHONCOREINTERFACE_H_
#define _PYTHONCOREINTERFACE_H_
//=============================================================================
//
//   File : pythoncoreinterface.h
//   Creation date : Fri Nov 07 00:18:31 2008 GMT by Elvio Basello
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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
#include "KviQString.h"
#include "KviKvsRunTimeContext.h"

#include <QStringList>

#define KVI_PYTHONCORECTRLCOMMAND_EXECUTE "execute"

struct KviPythonCoreCtrlCommand_execute
{
	unsigned int uSize;
	KviKvsRunTimeContext * pKvsContext;
	QString szContext;
	QString szCode;
	bool bExitOk;
	QString szRetVal;
	QString szError;
	QStringList lWarnings;
	QStringList lArgs;
	bool bQuiet;
};

#define KVI_PYTHONCORECTRLCOMMAND_DESTROY "destroy"

struct KviPythonCoreCtrlCommand_destroy
{
	unsigned int uSize;
	QString szContext;
};

#endif // !_PYTHONCOREINTERFACE_H_
