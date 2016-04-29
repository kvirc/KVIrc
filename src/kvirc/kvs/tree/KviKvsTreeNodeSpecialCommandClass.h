#ifndef _KVI_KVS_TREENODE_SPECIALCOMMANDCLASS_H_
#define _KVI_KVS_TREENODE_SPECIALCOMMANDCLASS_H_
//=============================================================================
//
//   File : KviKvsTreeNodeSpecialCommandClass.h
//   Creation date : Fri 12 Aug 2005 03:23:31 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
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
#include "KviKvsTreeNodeSpecialCommand.h"
#include "KviKvsTreeNodeBase.h"
#include "KviKvsTreeNodeDataList.h"

class KviKvsRunTimeContext;

class KVIRC_API KviKvsTreeNodeSpecialCommandClassFunctionDefinition : public KviKvsTreeNode
{
public:
	KviKvsTreeNodeSpecialCommandClassFunctionDefinition(const QChar * pLocation, const QString & szName, const QString & szBuffer, const QString & szReminder, unsigned int uHandlerFlags);
	virtual ~KviKvsTreeNodeSpecialCommandClassFunctionDefinition(){};

protected:
	QString m_szName;
	QString m_szReminder;
	QString m_szBuffer;
	unsigned int m_uHandlerFlags;

public:
	unsigned int handlerFlags() { return m_uHandlerFlags; };
	const QString & name() { return m_szName; };
	const QString & reminder() { return m_szReminder; };
	const QString & buffer() { return m_szBuffer; };
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
};

class KVIRC_API KviKvsTreeNodeSpecialCommandClass : public KviKvsTreeNodeSpecialCommand
{
public:
	KviKvsTreeNodeSpecialCommandClass(const QChar * pLocation, KviKvsTreeNodeDataList * pParams);
	virtual ~KviKvsTreeNodeSpecialCommandClass();

protected:
	KviKvsTreeNodeDataList * m_pParams;
	KviPointerList<KviKvsTreeNodeSpecialCommandClassFunctionDefinition> * m_pFunctions;

public:
	void addFunctionDefinition(KviKvsTreeNodeSpecialCommandClassFunctionDefinition * pDef);
	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);
	virtual bool execute(KviKvsRunTimeContext * c);
};

#endif //!_KVI_KVS_TREENODE_SPECIALCOMMANDCLASS_H_
