#ifndef _KVI_KVS_TREENODE_PARAMETERRETURN_H_
#define _KVI_KVS_TREENODE_PARAMETERRETURN_H_
//=============================================================================
//
//   File : KviKvsTreeNodeParameterReturn.h
//   Creation date : Fri 30 Jan 2004 01:31:01 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek <pragma at kvirc dot net>
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
#include "KviKvsTreeNodeInstruction.h"

class KviKvsTreeNodeDataList;

class KVIRC_API KviKvsTreeNodeParameterReturn : public KviKvsTreeNodeInstruction
{
public:
	KviKvsTreeNodeParameterReturn(const QChar * pLocation, KviKvsTreeNodeDataList * pDataList);
	~KviKvsTreeNodeParameterReturn();

protected:
	KviKvsTreeNodeDataList * m_pDataList;

public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c);
};

#endif //!_KVI_KVS_TREENODE_PARAMETERRETURN_H_
