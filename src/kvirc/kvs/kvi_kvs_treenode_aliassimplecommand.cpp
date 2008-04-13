//=============================================================================
//
//   File : kvi_kvs_treenode_aliassimplecommand.cpp
//   Created on Thu 09 Oct 2003 02:16:42 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_kvs_treenode_aliassimplecommand.h"
#include "kvi_kvs_treenode_datalist.h"
#include "kvi_kvs_treenode_switchlist.h"
#include "kvi_kvs_aliasmanager.h"
#include "kvi_locale.h"
#include "kvi_options.h"
#include "kvi_irccontext.h"
#include "kvi_window.h"
#include "kvi_ircconnection.h"
#include "kvi_qcstring.h"
#include "kvi_out.h"


KviKvsTreeNodeAliasSimpleCommand::KviKvsTreeNodeAliasSimpleCommand(const QChar * pLocation,const QString &szCmdName,KviKvsTreeNodeDataList * params)
: KviKvsTreeNodeSimpleCommand(pLocation,szCmdName,params)
{
}


KviKvsTreeNodeAliasSimpleCommand::~KviKvsTreeNodeAliasSimpleCommand()
{
}

void KviKvsTreeNodeAliasSimpleCommand::contextDescription(QString &szBuffer)
{
	KviQString::sprintf(szBuffer,"Alias Simple Command \"%Q\"",&m_szCmdName);
}

void KviKvsTreeNodeAliasSimpleCommand::dump(const char * prefix)
{
	debug("%s AliasSimpleCommand(%s)",prefix,m_szCmdName.toUtf8().data());
	dumpSwitchList(prefix);
	dumpParameterList(prefix);
}

bool KviKvsTreeNodeAliasSimpleCommand::execute(KviKvsRunTimeContext * c)
{
	KviKvsVariantList l;
	if(!m_pParams->evaluate(c,&l))return false;

	KviKvsSwitchList swl;
	if(m_pSwitches)
	{
		if(!(m_pSwitches->evaluate(c,&swl)))return false;
	}

	const KviKvsScript * s = KviKvsAliasManager::instance()->lookup(m_szCmdName);
	if(!s)
	{
		if(KVI_OPTION_BOOL(KviOption_boolSendUnknownCommandsAsRaw))
		{
			QString szAll;
			l.allAsString(szAll);
			
			if(!szAll.isEmpty())szAll.prepend(" ");
			szAll.prepend(m_szCmdName);

			KviQCString szData;

			if(!c->window()->context())
				goto no_way_to_send_as_raw;
			if(!c->window()->connection())
				goto no_way_to_send_as_raw;

			szData = c->window()->connection()->encodeText(szAll);
			if(!szData.data())szData = "";
	
			if(!c->window()->connection()->sendData(szData.data()))
				goto no_way_to_send_as_raw;
	
			c->window()->output(KVI_OUT_RAW,__tr2qs("[RAW]: %Q"),&szAll);
			return true;
no_way_to_send_as_raw:
			c->warning(this,__tr2qs("Failed to send an unknown command as /raw"));
			c->error(this,__tr2qs("Call to undefined command '%Q'"),&m_szCmdName);
			return false;
		} else {
			c->error(this,__tr2qs("Call to undefined command '%Q'"),&m_szCmdName);
			return false;
		}
	}

	KviKvsScript copy(*s); // quick reference
	// FIXME: the ExtRTData could be a member structure
	//        it would avoid the constructor call each time
	KviKvsExtendedRunTimeData extData(&swl);

	if(!copy.run(c->window(),&l,0,KviKvsScript::PreserveParams,&extData))
	{
		c->error(this,__tr2qs("Error in inner alias command call '%Q', called from this context"),&m_szCmdName);
		return false;
	}
	return true;
}
