//=============================================================================
//
//   File : KviKvsTreeNodeAliasSimpleCommand.cpp
//   Creation date : Thu 09 Oct 2003 02:16:42 by Szymon Stefanek
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

#include "KviKvsTreeNodeAliasSimpleCommand.h"
#include "KviKvsTreeNodeDataList.h"
#include "KviKvsTreeNodeSwitchList.h"
#include "KviKvsAliasManager.h"
#include "KviLocale.h"
#include "KviOptions.h"
#include "KviIrcContext.h"
#include "KviWindow.h"
#include "KviIrcConnection.h"
#include "kvi_out.h"

#include <QByteArray>

KviKvsTreeNodeAliasSimpleCommand::KviKvsTreeNodeAliasSimpleCommand(const QChar * pLocation, const QString & szCmdName, KviKvsTreeNodeDataList * params)
    : KviKvsTreeNodeSimpleCommand(pLocation, szCmdName, params)
{
}

KviKvsTreeNodeAliasSimpleCommand::~KviKvsTreeNodeAliasSimpleCommand()
    = default;

void KviKvsTreeNodeAliasSimpleCommand::contextDescription(QString & szBuffer)
{
	szBuffer = QString("Alias Simple Command \"%1\"").arg(m_szCmdName);
}

void KviKvsTreeNodeAliasSimpleCommand::dump(const char * prefix)
{
	qDebug("%s AliasSimpleCommand(%s)", prefix, m_szCmdName.toUtf8().data());
	dumpSwitchList(prefix);
	dumpParameterList(prefix);
}

bool KviKvsTreeNodeAliasSimpleCommand::execute(KviKvsRunTimeContext * c)
{
	KviKvsVariantList l;
	if(!m_pParams->evaluate(c, &l))
		return false;

	KviKvsSwitchList swl;
	if(m_pSwitches)
	{
		if(!(m_pSwitches->evaluate(c, &swl)))
			return false;
	}

	const KviKvsScript * s = KviKvsAliasManager::instance()->lookup(m_szCmdName);
	if(!s)
	{
		if(KVI_OPTION_BOOL(KviOption_boolSendUnknownCommandsAsRaw))
		{
			QString szAll;
			l.allAsString(szAll);

			if(!szAll.isEmpty())
				szAll.prepend(" ");
			szAll.prepend(m_szCmdName);

			QByteArray szData;

			if(!c->window()->context())
				goto no_way_to_send_as_raw;
			if(!c->window()->connection())
				goto no_way_to_send_as_raw;

			szData = c->window()->connection()->encodeText(szAll);

			if(!c->window()->connection()->sendData(szData.data()))
				goto no_way_to_send_as_raw;

			c->window()->output(KVI_OUT_RAW, __tr2qs_ctx("[RAW]: %Q", "kvs"), &szAll);
			return true;
		no_way_to_send_as_raw:
			c->warning(this, __tr2qs_ctx("Failed to send an unknown command as /RAW", "kvs"));
			c->error(this, __tr2qs_ctx("Call to undefined command '%Q'", "kvs"), &m_szCmdName);
			return false;
		}
		else
		{
			c->error(this, __tr2qs_ctx("Call to undefined command '%Q'", "kvs"), &m_szCmdName);
			return false;
		}
	}

	KviKvsScript copy(*s); // quick reference
	// FIXME: the ExtRTData could be a member structure
	//        it would avoid the constructor call each time
	KviKvsExtendedRunTimeData extData(&swl);

	if(!copy.run(c->window(), &l, nullptr, KviKvsScript::PreserveParams, &extData))
	{
		c->error(this, __tr2qs_ctx("Error in inner alias command call '%Q', called from this context", "kvs"), &m_szCmdName);
		return false;
	}
	return true;
}
