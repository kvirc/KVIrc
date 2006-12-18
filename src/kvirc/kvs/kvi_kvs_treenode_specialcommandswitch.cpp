//=============================================================================
//
//   File : kvi_kvs_treenode_specialcommandswitch.cpp
//   Created on Fri 02 Jan 2004 14:09:00 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004 Szymon Stefanek <pragma at kvirc dot net>
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your opinion) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#define __KVIRC__

#include "kvi_kvs_treenode_specialcommandswitch.h"
#include "kvi_kvs_treenode_expression.h"
#include "kvi_kvs_treenode_instruction.h"
#include "kvi_kvs_runtimecontext.h"
#include "kvi_locale.h"

#include <qregexp.h>

KviKvsTreeNodeSpecialCommandSwitchLabel::KviKvsTreeNodeSpecialCommandSwitchLabel(const QChar * pLocation)
: KviKvsTreeNode(pLocation)
{
#ifdef COMPILE_NEW_KVS
	m_pParameter = 0;
	m_pInstruction = 0;
	m_bHasTerminatingBreak = false;
#endif
}

KviKvsTreeNodeSpecialCommandSwitchLabel::~KviKvsTreeNodeSpecialCommandSwitchLabel()
{
#ifdef COMPILE_NEW_KVS
	if(m_pParameter)delete m_pParameter;
	if(m_pInstruction)delete m_pInstruction;
#endif
}

void KviKvsTreeNodeSpecialCommandSwitchLabel::setParameter(KviKvsTreeNodeData * pParameter)
{
#ifdef COMPILE_NEW_KVS
	if(m_pParameter)delete m_pParameter;
	m_pParameter = pParameter;
	if(m_pParameter)m_pParameter->setParent(this);
#endif
}

void KviKvsTreeNodeSpecialCommandSwitchLabel::setInstruction(KviKvsTreeNodeInstruction * pInstruction)
{
#ifdef COMPILE_NEW_KVS
	if(m_pInstruction)delete m_pInstruction;
	m_pInstruction = pInstruction;
	if(m_pInstruction)m_pInstruction->setParent(this);
#endif
}





KviKvsTreeNodeSpecialCommandSwitchLabelCase::KviKvsTreeNodeSpecialCommandSwitchLabelCase(const QChar * pLocation)
: KviKvsTreeNodeSpecialCommandSwitchLabel(pLocation)
{
}

KviKvsTreeNodeSpecialCommandSwitchLabelCase::~KviKvsTreeNodeSpecialCommandSwitchLabelCase()
{
}

void KviKvsTreeNodeSpecialCommandSwitchLabelCase::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Label \"case\" For Special Command \"switch\"";
#endif
}

void KviKvsTreeNodeSpecialCommandSwitchLabelCase::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s SpecialCommandSwitchLabelCase",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	if(m_pParameter)m_pParameter->dump(tmp.utf8().data());
	if(m_pInstruction)m_pInstruction->dump(tmp.utf8().data());
#endif
}

bool KviKvsTreeNodeSpecialCommandSwitchLabelCase::execute(KviKvsRunTimeContext * c,KviKvsVariant * pRealParameter)
{
#ifdef COMPILE_NEW_KVS
	KviKvsVariant v;
	if(!m_pParameter->evaluateReadOnly(c,&v))return false;

	KviKvsNumber num;
	if(pRealParameter->asNumber(num))
	{
		KviKvsNumber num2;
		if(!v.asNumber(num2))return true; // a number an a non number can't match
		if(num.isInteger())
		{
			if(num2.isInteger())
			{
				if(num.integer() != num2.integer())return true;
			} else {
				if(((double)(num.integer())) != num2.real())return true;
			}
		} else {
			if(num2.isInteger())
			{
				if(num.real() != ((double)(num2.integer())))return true;
			} else {
				if(num.real() != num2.real())return true;
			}
		}
	} else {
		// string comparision, case insensitive
		QString reg;
		v.asString(reg);
	
		QString val;
		pRealParameter->asString(val);
	
		if(reg.lower() != val.lower())return true;
	}

	if(m_pInstruction)
	{
		if(!m_pInstruction->execute(c))return false; // might be a break too
	}
	if(m_bHasTerminatingBreak)
	{
		c->setBreakPending();
		return false;
	}
#endif
	return true;
}




KviKvsTreeNodeSpecialCommandSwitchLabelMatch::KviKvsTreeNodeSpecialCommandSwitchLabelMatch(const QChar * pLocation)
: KviKvsTreeNodeSpecialCommandSwitchLabel(pLocation)
{
}

KviKvsTreeNodeSpecialCommandSwitchLabelMatch::~KviKvsTreeNodeSpecialCommandSwitchLabelMatch()
{
}

void KviKvsTreeNodeSpecialCommandSwitchLabelMatch::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Label \"match\" For Special Command \"switch\"";
#endif
}


void KviKvsTreeNodeSpecialCommandSwitchLabelMatch::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s SpecialCommandSwitchLabelMatch",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	if(m_pParameter)m_pParameter->dump(tmp.utf8().data());
	if(m_pInstruction)m_pInstruction->dump(tmp.utf8().data());
#endif
}

bool KviKvsTreeNodeSpecialCommandSwitchLabelMatch::execute(KviKvsRunTimeContext * c,KviKvsVariant * pRealParameter)
{
#ifdef COMPILE_NEW_KVS
	KviKvsVariant v;
	if(!m_pParameter->evaluateReadOnly(c,&v))return false;

	QString reg;
	v.asString(reg);

	QRegExp rx(reg,false,true);

	QString val;
	pRealParameter->asString(val);

	if(!rx.exactMatch(val))return true; // no match

	if(m_pInstruction)
	{
		if(!m_pInstruction->execute(c))return false; // might be a break too
	}
	if(m_bHasTerminatingBreak)
	{
		c->setBreakPending();
		return false;
	}
#endif
	return true;
}




KviKvsTreeNodeSpecialCommandSwitchLabelRegexp::KviKvsTreeNodeSpecialCommandSwitchLabelRegexp(const QChar * pLocation)
: KviKvsTreeNodeSpecialCommandSwitchLabel(pLocation)
{
}

KviKvsTreeNodeSpecialCommandSwitchLabelRegexp::~KviKvsTreeNodeSpecialCommandSwitchLabelRegexp()
{
}

void KviKvsTreeNodeSpecialCommandSwitchLabelRegexp::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Label \"regexp\" For Special Command \"switch\"";
#endif
}


void KviKvsTreeNodeSpecialCommandSwitchLabelRegexp::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s SpecialCommandSwitchLabelRegexp",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	if(m_pParameter)m_pParameter->dump(tmp.utf8().data());
	if(m_pInstruction)m_pInstruction->dump(tmp.utf8().data());
#endif
}

bool KviKvsTreeNodeSpecialCommandSwitchLabelRegexp::execute(KviKvsRunTimeContext * c,KviKvsVariant * pRealParameter)
{
#ifdef COMPILE_NEW_KVS
	KviKvsVariant v;
	if(!m_pParameter->evaluateReadOnly(c,&v))return false;

	QString reg;
	v.asString(reg);

	QRegExp rx(reg,false,false);

	QString val;
	pRealParameter->asString(val);

	if(!rx.exactMatch(val))return true; // no match

	if(m_pInstruction)
	{
		if(!m_pInstruction->execute(c))return false; // might be a break too
	}
	if(m_bHasTerminatingBreak)
	{
		c->setBreakPending();
		return false;
	}
#endif
	return true;
}






KviKvsTreeNodeSpecialCommandSwitchLabelDefault::KviKvsTreeNodeSpecialCommandSwitchLabelDefault(const QChar * pLocation)
: KviKvsTreeNodeSpecialCommandSwitchLabel(pLocation)
{
}

KviKvsTreeNodeSpecialCommandSwitchLabelDefault::~KviKvsTreeNodeSpecialCommandSwitchLabelDefault()
{
}

void KviKvsTreeNodeSpecialCommandSwitchLabelDefault::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Label \"default\" For Special Command \"switch\"";
#endif
}


void KviKvsTreeNodeSpecialCommandSwitchLabelDefault::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s SpecialCommandSwitchLabelDefault",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	if(m_pInstruction)m_pInstruction->dump(tmp.utf8().data());
#endif
}

bool KviKvsTreeNodeSpecialCommandSwitchLabelDefault::execute(KviKvsRunTimeContext * c,KviKvsVariant * pRealParameter)
{
#ifdef COMPILE_NEW_KVS
	if(m_pInstruction)
	{
		if(!m_pInstruction->execute(c))return false; // might be a break too
	}
	if(m_bHasTerminatingBreak)
	{
		c->setBreakPending();
		return false;
	}
#endif
	return true;
}




KviKvsTreeNodeSpecialCommandSwitch::KviKvsTreeNodeSpecialCommandSwitch(const QChar * pLocation,KviKvsTreeNodeExpression * e)
: KviKvsTreeNodeSpecialCommand(pLocation,"switch")
{
#ifdef COMPILE_NEW_KVS
	m_pExpression = e;
	m_pExpression->setParent(this);
	m_pLabels = new KviPtrList<KviKvsTreeNodeSpecialCommandSwitchLabel>;
	m_pLabels->setAutoDelete(true);
#endif
}

KviKvsTreeNodeSpecialCommandSwitch::~KviKvsTreeNodeSpecialCommandSwitch()
{
#ifdef COMPILE_NEW_KVS
	delete m_pExpression;
	delete m_pLabels;
#endif
}

void KviKvsTreeNodeSpecialCommandSwitch::addLabel(KviKvsTreeNodeSpecialCommandSwitchLabel * l)
{
#ifdef COMPILE_NEW_KVS
	m_pLabels->append(l);
	l->setParent(this);
#endif
}

void KviKvsTreeNodeSpecialCommandSwitch::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Special Command \"switch\"";
#endif
}


void KviKvsTreeNodeSpecialCommandSwitch::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s SpecialCommandSwitch",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pExpression->dump(tmp.utf8().data());
	for(KviKvsTreeNodeSpecialCommandSwitchLabel * l = m_pLabels->first();l;l = m_pLabels->next())
		l->dump(tmp.utf8().data());
#endif
}

bool KviKvsTreeNodeSpecialCommandSwitch::execute(KviKvsRunTimeContext * c)
{
#ifdef COMPILE_NEW_KVS
	KviKvsVariant v;
	if(!m_pExpression->evaluateReadOnly(c,&v))return false;

	for(KviKvsTreeNodeSpecialCommandSwitchLabel * l = m_pLabels->first();l;l = m_pLabels->next())
	{
		if(!l->execute(c,&v))
		{
			if(c->error())return false;
			// break ?
			if(c->breakPending())
			{
				c->handleBreak();
				return true;
			}
			return false;
		}
	}
#endif
	return true;
}
