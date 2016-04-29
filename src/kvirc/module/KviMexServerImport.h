#ifndef _KVI_MEXSERVERIMPORT_H_
#define _KVI_MEXSERVERIMPORT_H_
//=============================================================================
//
//   File : KviMexServerImport.h
//   Creation date : Wed Apr 9 2003 04:53 by Szymon Stefanek
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

#include "kvi_settings.h"
#include "KviIrcServerDataBase.h"
#include "KviModuleExtension.h"

#include <QObject>

class KVIRC_API KviMexServerImport : public QObject, public KviModuleExtension
{
	Q_OBJECT
public:
	KviMexServerImport(KviModuleExtensionDescriptor * d);
	~KviMexServerImport();

public:
	virtual void start() = 0;
	virtual void die() = 0;
signals:
	void server(const KviIrcServer & s, const QString & network);
};

#endif //!_KVI_MEXSERVERIMPORT_H_
