#ifndef _KVI_KVS_TREENODE_PARAMETERCOUNT_H_
#define _KVI_KVS_TREENODE_PARAMETERCOUNT_H_
//=============================================================================
//
//   File : KviKvsTreeNodeParameterCount.h
//   Creation date : Thu 21 Dec 2011 06:13:11 by Szymon Tomasz Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2011 Szymon Stefanek <pragma at kvirc dot net>
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
#include "KviKvsTreeNodeData.h"

class KviKvsRunTimeContext;
class KviKvsVariant;

class KVIRC_API KviKvsTreeNodeParameterCount : public KviKvsTreeNodeData
{
public:
	KviKvsTreeNodeParameterCount(const QChar * pLocation);
	~KviKvsTreeNodeParameterCount();

public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);

	virtual bool canEvaluateToObjectReference();
	virtual bool evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer);
};

#endif //!_KVI_KVS_TREENODE_PARAMETERCOUNT_H_
