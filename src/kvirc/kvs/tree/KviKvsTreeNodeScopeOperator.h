#ifndef _KVI_KVS_TREENODE_SCOPEOPERATOR_H_
#define _KVI_KVS_TREENODE_SCOPEOPERATOR_H_
//=============================================================================
//
//   File : KviKvsTreeNodeScopeOperator.h
//   Creation date : Tue 07 Oct 2003 02:33:01 by Szymon Stefanek
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
#include "KviKvsTreeNodeData.h"

class KviKvsObject;

class KVIRC_API KviKvsTreeNodeScopeOperator : public KviKvsTreeNodeData
{
public:
	KviKvsTreeNodeScopeOperator(const QChar * pLocation, KviKvsTreeNodeData * pObject, KviKvsTreeNodeData * pData);
	~KviKvsTreeNodeScopeOperator();

public:
	KviKvsTreeNodeData * m_pObjectReference; // can't be null
	KviKvsTreeNodeData * m_pRightSide;       // can't be null
public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool isReadOnly();                   // gets the m_pRightSide read only state
	virtual bool canEvaluateToObjectReference(); // gets the m_pRightSide result
	virtual bool isFunctionCall();               // gets the m_pRightSide result
	virtual bool canEvaluateInObjectScope();     // gets the m_pObjectReference result
	//virtual bool canReleaseResult(); // gets the m_pRightSide result

	virtual bool evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer);
	virtual KviKvsRWEvaluationResult * evaluateReadWrite(KviKvsRunTimeContext * c);
	virtual bool evaluateReadOnlyInObjectScope(KviKvsObject * o, KviKvsRunTimeContext * c, KviKvsVariant * pBuffer);
	virtual KviKvsRWEvaluationResult * evaluateReadWriteInObjectScope(KviKvsObject * o, KviKvsRunTimeContext * c);

protected:
	KviKvsObject * objectFromVariant(KviKvsRunTimeContext * c, KviKvsVariant * v);
};

#endif //!_KVI_KVS_TREENODE_SCOPEOPERATOR_H_
