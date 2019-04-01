#ifndef _PERLCOREINTERFACE_H_
#define _PERLCOREINTERFACE_H_
//=============================================================================
//
//   File : perlcoreinterface.h
//   Creation date : Tue Jul 13 13:03:31 2004 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#define KVI_PERLCORECTRLCOMMAND_EXECUTE "execute"

struct KviPerlCoreCtrlCommand_execute
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

#define KVI_PERLCORECTRLCOMMAND_DESTROY "destroy"

struct KviPerlCoreCtrlCommand_destroy
{
	unsigned int uSize;
	QString szContext;
};

#endif // !_PERLCOREINTERFACE_H_
