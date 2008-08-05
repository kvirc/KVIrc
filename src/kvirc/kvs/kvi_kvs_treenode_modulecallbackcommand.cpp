//=============================================================================
//
//   File : kvi_kvs_treenode_modulecallbackcommand.cpp
//   Created on Thu 09 Oct 2003 02:09:10 by Szymon Stefanek
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

#include "kvi_kvs_treenode_modulecallbackcommand.h"
#include "kvi_kvs_treenode_datalist.h"
#include "kvi_kvs_treenode_switchlist.h"

#include "kvi_modulemanager.h"
#include "kvi_locale.h"
#include "kvi_kvs_moduleinterface.h"
#include "kvi_kvs_runtimecontext.h"

KviKvsTreeNodeModuleCallbackCommand::KviKvsTreeNodeModuleCallbackCommand(const QChar * pLocation,const QString &szModuleName,const QString &szCmdName,KviKvsTreeNodeDataList * params,KviKvsScript * pCallback)
: KviKvsTreeNodeCallbackCommand(pLocation,szCmdName,params,pCallback)
{
	m_szModuleName = szModuleName;
}


KviKvsTreeNodeModuleCallbackCommand::~KviKvsTreeNodeModuleCallbackCommand()
{
}

void KviKvsTreeNodeModuleCallbackCommand::contextDescription(QString &szBuffer)
{
	szBuffer = "Module Callback Command \"";
	szBuffer += m_szModuleName;
	szBuffer += ".";
	szBuffer += m_szCmdName;
	szBuffer += "\"";
}

void KviKvsTreeNodeModuleCallbackCommand::dump(const char * prefix)
{
	qDebug("%s ModuleCallbackCommand(%s.%s)",prefix,m_szModuleName.toUtf8().data(),m_szCmdName.toUtf8().data());
	dumpSwitchList(prefix);
	dumpParameterList(prefix);
	dumpCallback(prefix);
}

bool KviKvsTreeNodeModuleCallbackCommand::execute(KviKvsRunTimeContext * c)
{
//#warning "FIXME: module names should be UNICODE!"
	KviModule * m = g_pModuleManager->getModule(m_szModuleName.toUtf8().data());
	if(!m)
	{
		QString szErr = g_pModuleManager->lastError().ptr(); // <-- fixme!
		c->error(this,__tr2qs("Module command call failed: can't load the module '%Q': %Q"),&m_szModuleName,&szErr);
		return false;
	}

	KviKvsModuleCallbackCommandExecRoutine * proc = m->kvsFindCallbackCommand(m_szCmdName);
	if(!proc)
	{
		c->error(this,__tr2qs("Module command call failed: the module '%Q' doesn't export a callback command named '%Q'"),&m_szModuleName,&m_szCmdName);
		return false;
	}

	KviKvsVariantList l;
	l.setAutoDelete(true);
	if(!(m_pParams->evaluate(c,&l)))return false;
	
	KviKvsSwitchList swl;
	if(m_pSwitches)
	{
		if(!(m_pSwitches->evaluate(c,&swl)))return false;
	}

	c->setDefaultReportLocation(this);
	
	KviKvsModuleCallbackCommandCall call(m,c,&l,&swl,m_pCallback,m_pParams);

	return (*proc)(&call);
}
