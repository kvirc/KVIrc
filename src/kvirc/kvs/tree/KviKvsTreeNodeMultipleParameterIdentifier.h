#ifndef _KVI_KVS_TREENODE_MULTIPLEPARAMETERIDENTIFIER_H_
#define _KVI_KVS_TREENODE_MULTIPLEPARAMETERIDENTIFIER_H_
//=============================================================================
//
//   File : KviKvsTreeNodeMultipleParameterIdentifier.h
//   Creation date : Tue 07 Oct 2003 03:49:31 by Szymon Stefanek
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
#include "KviKvsTreeNodeData.h"

class KviKvsRunTimeContext;
class KviKvsVariant;

class KVIRC_API KviKvsTreeNodeMultipleParameterIdentifier : public KviKvsTreeNodeData
{
public:
	KviKvsTreeNodeMultipleParameterIdentifier(const QChar * pLocation, int iStart, int iEnd);
	~KviKvsTreeNodeMultipleParameterIdentifier();

protected:
	int m_iStart;
	int m_iEnd; // the index of the last parameter wanted (may be -1: in that case all up to the end)
public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer);
};

#endif //!_KVI_KVS_TREENODE_MULTIPLEPARAMETERIDENTIFIER_H_
