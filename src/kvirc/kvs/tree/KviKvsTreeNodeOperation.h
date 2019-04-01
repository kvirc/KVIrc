#ifndef _KVI_KVS_TREENODE_OPERATION_H_
#define _KVI_KVS_TREENODE_OPERATION_H_
//=============================================================================
//
//   File : KviKvsTreeNodeOperation.h
//   Creation date : Thu 18 Dec 2003 22:36:00 by Szymon Stefanek
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
#include "KviKvsTreeNodeInstruction.h"

class KviKvsTreeNodeData;
class KviKvsRunTimeContext;

class KVIRC_API KviKvsTreeNodeOperation : public KviKvsTreeNodeInstruction
{
public:
	KviKvsTreeNodeOperation(const QChar * pLocation);
	~KviKvsTreeNodeOperation();

protected:
	KviKvsTreeNodeData * m_pTargetData = nullptr; // can't be null
public:
	void setTargetVariableReference(KviKvsTreeNodeData * r);
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
};

class KviKvsTreeNodeOperationAssignment : public KviKvsTreeNodeOperation
{
public:
	KviKvsTreeNodeOperationAssignment(const QChar * pLocation, KviKvsTreeNodeData * pRightSide);
	~KviKvsTreeNodeOperationAssignment();

protected:
	KviKvsTreeNodeData * m_pRightSide; // can't be 0
public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c);
};

class KviKvsTreeNodeOperationDecrement : public KviKvsTreeNodeOperation
{
public:
	KviKvsTreeNodeOperationDecrement(const QChar * pLocation);
	~KviKvsTreeNodeOperationDecrement();

public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c);
};

class KviKvsTreeNodeOperationIncrement : public KviKvsTreeNodeOperation
{
public:
	KviKvsTreeNodeOperationIncrement(const QChar * pLocation);
	~KviKvsTreeNodeOperationIncrement();

public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c);
};

class KviKvsTreeNodeOperationSelfAnd : public KviKvsTreeNodeOperation
{
public:
	KviKvsTreeNodeOperationSelfAnd(const QChar * pLocation, KviKvsTreeNodeData * pRightSide);
	~KviKvsTreeNodeOperationSelfAnd();

protected:
	KviKvsTreeNodeData * m_pRightSide; // can't be 0
public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c);
};

class KviKvsTreeNodeOperationSelfDivision : public KviKvsTreeNodeOperation
{
public:
	KviKvsTreeNodeOperationSelfDivision(const QChar * pLocation, KviKvsTreeNodeData * pRightSide);
	~KviKvsTreeNodeOperationSelfDivision();

protected:
	KviKvsTreeNodeData * m_pRightSide; // can't be 0
public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c);
};

class KviKvsTreeNodeOperationSelfModulus : public KviKvsTreeNodeOperation
{
public:
	KviKvsTreeNodeOperationSelfModulus(const QChar * pLocation, KviKvsTreeNodeData * pRightSide);
	~KviKvsTreeNodeOperationSelfModulus();

protected:
	KviKvsTreeNodeData * m_pRightSide; // can't be 0
public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c);
};

class KviKvsTreeNodeOperationSelfMultiplication : public KviKvsTreeNodeOperation
{
public:
	KviKvsTreeNodeOperationSelfMultiplication(const QChar * pLocation, KviKvsTreeNodeData * pRightSide);
	~KviKvsTreeNodeOperationSelfMultiplication();

protected:
	KviKvsTreeNodeData * m_pRightSide; // can't be 0
public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c);
};

class KviKvsTreeNodeOperationSelfOr : public KviKvsTreeNodeOperation
{
public:
	KviKvsTreeNodeOperationSelfOr(const QChar * pLocation, KviKvsTreeNodeData * pRightSide);
	~KviKvsTreeNodeOperationSelfOr();

protected:
	KviKvsTreeNodeData * m_pRightSide; // can't be 0
public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c);
};

class KviKvsTreeNodeOperationSelfShl : public KviKvsTreeNodeOperation
{
public:
	KviKvsTreeNodeOperationSelfShl(const QChar * pLocation, KviKvsTreeNodeData * pRightSide);
	~KviKvsTreeNodeOperationSelfShl();

protected:
	KviKvsTreeNodeData * m_pRightSide; // can't be 0
public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c);
};

class KviKvsTreeNodeOperationSelfShr : public KviKvsTreeNodeOperation
{
public:
	KviKvsTreeNodeOperationSelfShr(const QChar * pLocation, KviKvsTreeNodeData * pRightSide);
	~KviKvsTreeNodeOperationSelfShr();

protected:
	KviKvsTreeNodeData * m_pRightSide; // can't be 0
public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c);
};

class KviKvsTreeNodeOperationSelfSubtraction : public KviKvsTreeNodeOperation
{
public:
	KviKvsTreeNodeOperationSelfSubtraction(const QChar * pLocation, KviKvsTreeNodeData * pRightSide);
	~KviKvsTreeNodeOperationSelfSubtraction();

protected:
	KviKvsTreeNodeData * m_pRightSide; // can't be 0
public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c);
};

class KviKvsTreeNodeOperationSelfSum : public KviKvsTreeNodeOperation
{
public:
	KviKvsTreeNodeOperationSelfSum(const QChar * pLocation, KviKvsTreeNodeData * pRightSide);
	~KviKvsTreeNodeOperationSelfSum();

protected:
	KviKvsTreeNodeData * m_pRightSide; // can't be 0
public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c);
};

class KviKvsTreeNodeOperationSelfXor : public KviKvsTreeNodeOperation
{
public:
	KviKvsTreeNodeOperationSelfXor(const QChar * pLocation, KviKvsTreeNodeData * pRightSide);
	~KviKvsTreeNodeOperationSelfXor();

protected:
	KviKvsTreeNodeData * m_pRightSide; // can't be 0
public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c);
};

class KviKvsTreeNodeOperationStringAppend : public KviKvsTreeNodeOperation
{
public:
	KviKvsTreeNodeOperationStringAppend(const QChar * pLocation, KviKvsTreeNodeData * pRightSide);
	~KviKvsTreeNodeOperationStringAppend();

protected:
	KviKvsTreeNodeData * m_pRightSide; // can't be 0
public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c);
};

class KviKvsTreeNodeOperationArrayAppend : public KviKvsTreeNodeOperation
{
public:
	KviKvsTreeNodeOperationArrayAppend(const QChar * pLocation, KviKvsTreeNodeData * pRightSide);
	~KviKvsTreeNodeOperationArrayAppend();

protected:
	KviKvsTreeNodeData * m_pRightSide; // can't be 0
public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c);
};

class KviKvsTreeNodeOperationStringAppendWithComma : public KviKvsTreeNodeOperation
{
public:
	KviKvsTreeNodeOperationStringAppendWithComma(const QChar * pLocation, KviKvsTreeNodeData * pRightSide);
	~KviKvsTreeNodeOperationStringAppendWithComma();

protected:
	KviKvsTreeNodeData * m_pRightSide; // can't be 0
public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c);
};

class KviKvsTreeNodeOperationStringAppendWithSpace : public KviKvsTreeNodeOperation
{
public:
	KviKvsTreeNodeOperationStringAppendWithSpace(const QChar * pLocation, KviKvsTreeNodeData * pRightSide);
	~KviKvsTreeNodeOperationStringAppendWithSpace();

protected:
	KviKvsTreeNodeData * m_pRightSide; // can't be 0
public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c);
};

class KviKvsTreeNodeOperationStringTransliteration : public KviKvsTreeNodeOperation
{
public:
	KviKvsTreeNodeOperationStringTransliteration(const QChar * pLocation, KviKvsTreeNodeData * pLeft, KviKvsTreeNodeData * pRight, KviKvsTreeNodeData * pFlags);
	~KviKvsTreeNodeOperationStringTransliteration();

protected:
	KviKvsTreeNodeData * m_pLeft;  // can't be 0
	KviKvsTreeNodeData * m_pRight; // can't be 0
	KviKvsTreeNodeData * m_pFlags; // can't be 0
public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c);
};

class KviKvsTreeNodeOperationStringSubstitution : public KviKvsTreeNodeOperation
{
public:
	KviKvsTreeNodeOperationStringSubstitution(const QChar * pLocation, KviKvsTreeNodeData * pLeft, KviKvsTreeNodeData * pRight, KviKvsTreeNodeData * pFlags);
	~KviKvsTreeNodeOperationStringSubstitution();

protected:
	KviKvsTreeNodeData * m_pLeft;  // can't be 0
	KviKvsTreeNodeData * m_pRight; // can't be 0
	KviKvsTreeNodeData * m_pFlags; // can't be 0
public:
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c);
};

#endif //!_KVI_KVS_TREENODE_OPERATION_H_
