#ifndef _ADDONFUNCTIONS_H_
#define _ADDONFUNCTIONS_H_
//=============================================================================
//
//   File : AddonFunctions.h
//   Creation date : Fri 02 May 2008 17:36:07 by Elvio Basello
//
//   This file is part of the KVIrc IRC Client distribution
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

#include "kvi_settings.h"
#include "KviQString.h"

#include <QWidget>
#include <QPixmap>

#define KVI_CURRENT_ADDONS_ENGINE_VERSION "2.0.0"

class AddonInfo
{
public:
	QString szAuthor;
	QString szName;
	QString szVersion;
	QString szDescription;
	QString szMinVersion;
	QString szImage;
	QString szDirPath;
	QString szSavePath;
};

namespace AddonFunctions
{
	bool checkDirTree(const QString & szDirPath, QString * pszError);
	bool pack(AddonInfo & info, QString & szError);
	bool notAValidAddonPackage(QString & szError);
	bool installAddonPackage(const QString & szAddonPackageFileName, QString & szError, QWidget * pDialogParent = nullptr);
	QString createRandomDir();
}

#endif //!_ADDONFUNCTIONS_H_
