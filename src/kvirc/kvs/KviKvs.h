#ifndef _KVI_KVS_H_
#define _KVI_KVS_H_
//=============================================================================
//
//   File : KviKvs.h
//   Creation date : Wed 1 Oct 2003 01.37 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek (pragma at kvirc dot net)
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

// this header contains ALL the stuff that the user should see
#include "KviKvsScript.h"

namespace KviKvs
{
	void init(); // this never fails
	void done(); // this never fails

	void loadAliases(const QString & szFileName);
	void saveAliases(const QString & szFileName);
	void clearAliases();

	void loadPopups(const QString & szFileName);
	void savePopups(const QString & szFileName);
	void clearPopups();

	void loadAppEvents(const QString & szFileName);
	void saveAppEvents(const QString & szFileName);
	void clearAppEvents();

	void loadRawEvents(const QString & szFileName);
	void saveRawEvents(const QString & szFileName);
	void clearRawEvents();

	void loadScriptAddons(const QString & szFileName);
	void saveScriptAddons(const QString & szFileName);
	void clearScriptAddons();

	void flushUserClasses();
};

#endif //!_KVI_KVS_H_
