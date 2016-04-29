#ifndef _KVI_KVS_TREENODE_SPECIALCOMMAND_H_
#define _KVI_KVS_TREENODE_SPECIALCOMMAND_H_
//=============================================================================
//
//   File : KviKvsTreeNodeSpecialCommand.h
//   Creation date : Thu 09 Oct 2003 02:20:12 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek <pragma at kvirc dot net>
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
#include "KviKvsTreeNodeCommand.h"

class KVIRC_API KviKvsTreeNodeSpecialCommand : public KviKvsTreeNodeCommand
{
public:
	KviKvsTreeNodeSpecialCommand(const QChar * pLocation, const QString & szCmdName);
	~KviKvsTreeNodeSpecialCommand();

public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
};

#endif //!_KVI_KVS_TREENODE_SPECIALCOMMAND_H_
