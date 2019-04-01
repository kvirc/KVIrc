//=============================================================================
//
//   File : KviKvsTreeNodeSpecialCommandSwitch.cpp
//   Creation date : Fri 02 Jan 2004 14:09:00 by Szymon Stefanek
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

#include "KviKvsTreeNodeSpecialCommandSwitch.h"
#include "KviKvsTreeNodeExpression.h"
#include "KviKvsTreeNodeInstruction.h"
#include "KviKvsRunTimeContext.h"
#include "KviLocale.h"

#include <QRegExp>

KviKvsTreeNodeSpecialCommandSwitchLabel::KviKvsTreeNodeSpecialCommandSwitchLabel(const QChar * pLocation)
    : KviKvsTreeNode(pLocation)
{
	m_pParameter = nullptr;
	m_pInstruction = nullptr;
	m_bHasTerminatingBreak = false;
}

KviKvsTreeNodeSpecialCommandSwitchLabel::~KviKvsTreeNodeSpecialCommandSwitchLabel()
{
	if(m_pParameter)
		delete m_pParameter;
	if(m_pInstruction)
		delete m_pInstruction;
}

void KviKvsTreeNodeSpecialCommandSwitchLabel::setParameter(KviKvsTreeNodeData * pParameter)
{
	if(m_pParameter)
		delete m_pParameter;
	m_pParameter = pParameter;
	if(m_pParameter)
		m_pParameter->setParent(this);
}

void KviKvsTreeNodeSpecialCommandSwitchLabel::setInstruction(KviKvsTreeNodeInstruction * pInstruction)
{
	if(m_pInstruction)
		delete m_pInstruction;
	m_pInstruction = pInstruction;
	if(m_pInstruction)
		m_pInstruction->setParent(this);
}

KviKvsTreeNodeSpecialCommandSwitchLabelCase::KviKvsTreeNodeSpecialCommandSwitchLabelCase(const QChar * pLocation)
    : KviKvsTreeNodeSpecialCommandSwitchLabel(pLocation)
{
}

KviKvsTreeNodeSpecialCommandSwitchLabelCase::~KviKvsTreeNodeSpecialCommandSwitchLabelCase()
    = default;

void KviKvsTreeNodeSpecialCommandSwitchLabelCase::contextDescription(QString & szBuffer)
{
	szBuffer = R"(Label "case" for Special Command "switch")";
}

void KviKvsTreeNodeSpecialCommandSwitchLabelCase::dump(const char * prefix)
{
	qDebug("%s SpecialCommandSwitchLabelCase", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	if(m_pParameter)
		m_pParameter->dump(tmp.toUtf8().data());
	if(m_pInstruction)
		m_pInstruction->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeSpecialCommandSwitchLabelCase::execute(KviKvsRunTimeContext * c, KviKvsVariant * pRealParameter, bool * bPassThrough)
{
	if(!(*bPassThrough))
	{
		KviKvsVariant v;
		if(!m_pParameter->evaluateReadOnly(c, &v))
			return false;

		KviKvsNumber num;
		if(pRealParameter->asNumber(num))
		{
			KviKvsNumber num2;
			if(!v.asNumber(num2))
				return true; // a number and a non number can't match
			if(num.isInteger())
			{
				if(num2.isInteger())
				{
					if(num.integer() != num2.integer())
						return true;
				}
				else
				{
					if(((double)(num.integer())) != num2.real())
						return true;
				}
			}
			else
			{
				if(num2.isInteger())
				{
					if(num.real() != ((double)(num2.integer())))
						return true;
				}
				else
				{
					if(num.real() != num2.real())
						return true;
				}
			}
		}
		else
		{
			// string comparison, case insensitive
			QString reg;
			v.asString(reg);

			QString val;
			pRealParameter->asString(val);

			if(reg.toLower() != val.toLower())
				return true;
		}
	}

	*bPassThrough = true;

	if(m_pInstruction)
	{
		if(!m_pInstruction->execute(c))
			return false; // might be a break too
	}
	if(m_bHasTerminatingBreak)
	{
		c->setBreakPending();
		return false;
	}
	return true;
}

KviKvsTreeNodeSpecialCommandSwitchLabelMatch::KviKvsTreeNodeSpecialCommandSwitchLabelMatch(const QChar * pLocation)
    : KviKvsTreeNodeSpecialCommandSwitchLabel(pLocation)
{
}

KviKvsTreeNodeSpecialCommandSwitchLabelMatch::~KviKvsTreeNodeSpecialCommandSwitchLabelMatch()
    = default;

void KviKvsTreeNodeSpecialCommandSwitchLabelMatch::contextDescription(QString & szBuffer)
{
	szBuffer = R"(Label "match" for Special Command "switch")";
}

void KviKvsTreeNodeSpecialCommandSwitchLabelMatch::dump(const char * prefix)
{
	qDebug("%s SpecialCommandSwitchLabelMatch", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	if(m_pParameter)
		m_pParameter->dump(tmp.toUtf8().data());
	if(m_pInstruction)
		m_pInstruction->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeSpecialCommandSwitchLabelMatch::execute(KviKvsRunTimeContext * c, KviKvsVariant * pRealParameter, bool * bPassThrough)
{
	if(!(*bPassThrough))
	{
		KviKvsVariant v;
		if(!m_pParameter->evaluateReadOnly(c, &v))
			return false;

		QString reg;
		v.asString(reg);

		//QRegExp rx(reg,false,true);
		QRegExp rx(reg, Qt::CaseInsensitive, QRegExp::Wildcard);
		QString val;
		pRealParameter->asString(val);

		if(!rx.exactMatch(val))
			return true; // no match
	}

	*bPassThrough = true;

	if(m_pInstruction)
	{
		if(!m_pInstruction->execute(c))
			return false; // might be a break too
	}
	if(m_bHasTerminatingBreak)
	{
		c->setBreakPending();
		return false;
	}
	return true;
}

KviKvsTreeNodeSpecialCommandSwitchLabelRegexp::KviKvsTreeNodeSpecialCommandSwitchLabelRegexp(const QChar * pLocation)
    : KviKvsTreeNodeSpecialCommandSwitchLabel(pLocation)
{
}

KviKvsTreeNodeSpecialCommandSwitchLabelRegexp::~KviKvsTreeNodeSpecialCommandSwitchLabelRegexp()
    = default;

void KviKvsTreeNodeSpecialCommandSwitchLabelRegexp::contextDescription(QString & szBuffer)
{
	szBuffer = R"(Label "regexp" for Special Command "switch")";
}

void KviKvsTreeNodeSpecialCommandSwitchLabelRegexp::dump(const char * prefix)
{
	qDebug("%s SpecialCommandSwitchLabelRegexp", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	if(m_pParameter)
		m_pParameter->dump(tmp.toUtf8().data());
	if(m_pInstruction)
		m_pInstruction->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeSpecialCommandSwitchLabelRegexp::execute(KviKvsRunTimeContext * c, KviKvsVariant * pRealParameter, bool * bPassThrough)
{
	if(!(*bPassThrough))
	{
		KviKvsVariant v;
		if(!m_pParameter->evaluateReadOnly(c, &v))
			return false;

		QString reg;
		v.asString(reg);

		//	QRegExp rx(reg,false,false);
		QRegExp rx(reg, Qt::CaseInsensitive, QRegExp::RegExp);
		QString val;
		pRealParameter->asString(val);

		if(!rx.exactMatch(val))
			return true; // no match
	}

	*bPassThrough = true;

	if(m_pInstruction)
	{
		if(!m_pInstruction->execute(c))
			return false; // might be a break too
	}
	if(m_bHasTerminatingBreak)
	{
		c->setBreakPending();
		return false;
	}
	return true;
}

KviKvsTreeNodeSpecialCommandSwitchLabelDefault::KviKvsTreeNodeSpecialCommandSwitchLabelDefault(const QChar * pLocation)
    : KviKvsTreeNodeSpecialCommandSwitchLabel(pLocation)
{
}

KviKvsTreeNodeSpecialCommandSwitchLabelDefault::~KviKvsTreeNodeSpecialCommandSwitchLabelDefault()
    = default;

void KviKvsTreeNodeSpecialCommandSwitchLabelDefault::contextDescription(QString & szBuffer)
{
	szBuffer = R"(Label "default" for Special Command "switch")";
}

void KviKvsTreeNodeSpecialCommandSwitchLabelDefault::dump(const char * prefix)
{
	qDebug("%s SpecialCommandSwitchLabelDefault", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	if(m_pInstruction)
		m_pInstruction->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeSpecialCommandSwitchLabelDefault::execute(KviKvsRunTimeContext * c, KviKvsVariant *, bool * bPassThrough)
{
	*bPassThrough = true;

	if(m_pInstruction)
	{
		if(!m_pInstruction->execute(c))
			return false; // might be a break too
	}
	if(m_bHasTerminatingBreak)
	{
		c->setBreakPending();
		return false;
	}
	return true;
}

KviKvsTreeNodeSpecialCommandSwitch::KviKvsTreeNodeSpecialCommandSwitch(const QChar * pLocation, KviKvsTreeNodeExpression * e)
    : KviKvsTreeNodeSpecialCommand(pLocation, "switch")
{
	m_pExpression = e;
	m_pExpression->setParent(this);
	m_pLabels = new KviPointerList<KviKvsTreeNodeSpecialCommandSwitchLabel>;
	m_pLabels->setAutoDelete(true);
}

KviKvsTreeNodeSpecialCommandSwitch::~KviKvsTreeNodeSpecialCommandSwitch()
{
	delete m_pExpression;
	delete m_pLabels;
}

void KviKvsTreeNodeSpecialCommandSwitch::addLabel(KviKvsTreeNodeSpecialCommandSwitchLabel * l)
{
	m_pLabels->append(l);
	l->setParent(this);
}

void KviKvsTreeNodeSpecialCommandSwitch::contextDescription(QString & szBuffer)
{
	szBuffer = "Special Command \"switch\"";
}

void KviKvsTreeNodeSpecialCommandSwitch::dump(const char * prefix)
{
	qDebug("%s SpecialCommandSwitch", prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pExpression->dump(tmp.toUtf8().data());
	for(KviKvsTreeNodeSpecialCommandSwitchLabel * l = m_pLabels->first(); l; l = m_pLabels->next())
		l->dump(tmp.toUtf8().data());
}

bool KviKvsTreeNodeSpecialCommandSwitch::execute(KviKvsRunTimeContext * c)
{
	KviKvsVariant v;
	if(!m_pExpression->evaluateReadOnly(c, &v))
		return false;

	KviKvsSwitchList swl;
	if(m_pSwitches)
	{
		if(!(m_pSwitches->evaluate(c, &swl)))
			return false;
	}

	bool bUsePassThrough = swl.find('p', "--passthrough");
	bool bPassThrough = false;
	for(KviKvsTreeNodeSpecialCommandSwitchLabel * l = m_pLabels->first(); l; l = m_pLabels->next())
	{
		if(!l->execute(c, &v, &bPassThrough))
		{
			if(c->error())
				return false;
			// break ?
			if(c->breakPending())
			{
				c->handleBreak();
				return true;
			}
			return false;
		}
		if(!bUsePassThrough)
			bPassThrough = false;
	}
	return true;
}
