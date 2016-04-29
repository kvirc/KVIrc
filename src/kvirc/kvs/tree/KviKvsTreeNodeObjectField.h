#ifndef _KVI_KVS_TREENODE_OBJECTFIELD_H_
#define _KVI_KVS_TREENODE_OBJECTFIELD_H_
//=============================================================================
//
//   File : KviKvsTreeNodeObjectField.h
//   Creation date : Thu 28 Apr 2005 17:12:14 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2010 Szymon Stefanek <pragma at kvirc dot net>
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
#include "KviKvsTreeNodeVariable.h"

class KviKvsRunTimeContext;
class KviKvsObject;

class KVIRC_API KviKvsTreeNodeObjectField : public KviKvsTreeNodeVariable
{
public:
	KviKvsTreeNodeObjectField(const QChar * pLocation, const QString & szIdentifier);
	~KviKvsTreeNodeObjectField();

public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool canEvaluateInObjectScope();
	virtual bool evaluateReadOnlyInObjectScope(KviKvsObject * o, KviKvsRunTimeContext * c, KviKvsVariant * pResult);
	virtual KviKvsRWEvaluationResult * evaluateReadWriteInObjectScope(KviKvsObject * o, KviKvsRunTimeContext * c);
};

#endif //!_KVI_KVS_TREENODE_OBJECTFIELD_H_
