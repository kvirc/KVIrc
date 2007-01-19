//=============================================================================
//
//   File : kvi_kvs_kernel.cpp
//   Creation date : Tue 30 Sep 2003 05.12 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2003 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_kvs_kernel.h"
#include "kvi_kvs_parser.h"
#include "kvi_kvs_hash.h"
#include "kvi_kvs_aliasmanager.h"
#include "kvi_kvs_coresimplecommands.h"
#include "kvi_kvs_corefunctions.h"
#include "kvi_kvs_corecallbackcommands.h"
#include "kvi_kvs_switchlist.h"
#include "kvi_kvs_variantlist.h"
#include "kvi_kvs_script.h"
#include "kvi_kvs_object_controller.h"
#include "kvi_kvs_asyncoperation.h"
#include "kvi_modulemanager.h"

KviKvsKernel * KviKvsKernel::m_pKvsKernel = 0;

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION AND DESTRUCTION

KviKvsKernel::KviKvsKernel()
{
	m_pKvsKernel = this;

	m_pSpecialCommandParsingRoutineDict = new KviDict<KviKvsSpecialCommandParsingRoutine>(17,false);
	m_pSpecialCommandParsingRoutineDict->setAutoDelete(true);
	m_pCoreSimpleCommandExecRoutineDict = new KviDict<KviKvsCoreSimpleCommandExecRoutine>(51,false);
	m_pCoreSimpleCommandExecRoutineDict->setAutoDelete(true);
	m_pCoreFunctionExecRoutineDict = new KviDict<KviKvsCoreFunctionExecRoutine>(51,false);
	m_pCoreFunctionExecRoutineDict->setAutoDelete(true);
	m_pCoreCallbackCommandExecRoutineDict = new KviDict<KviKvsCoreCallbackCommandExecRoutine>(17,false);
	m_pCoreCallbackCommandExecRoutineDict->setAutoDelete(true);

	m_pGlobalVariables = new KviKvsHash();
	m_pEmptyParameterList = new KviKvsVariantList();
	m_pObjectController = new KviKvsObjectController();
	m_pObjectController->init();
	m_pAsyncOperationManager = new KviKvsAsyncOperationManager();
	
	KviKvsParser::init();

	KviKvsCoreSimpleCommands::init();
	KviKvsCoreFunctions::init();
	KviKvsCoreCallbackCommands::init();
}

KviKvsKernel::~KviKvsKernel()
{
	delete m_pAsyncOperationManager;
	delete m_pObjectController;
	delete m_pEmptyParameterList;
	delete m_pGlobalVariables;

	delete m_pSpecialCommandParsingRoutineDict;
	delete m_pCoreSimpleCommandExecRoutineDict;
	delete m_pCoreFunctionExecRoutineDict;
	delete m_pCoreCallbackCommandExecRoutineDict;
}

////////////////////////////////////////////////////////////////////////////////
// INSTANCE MANAGEMENT

void KviKvsKernel::init()
{
	if(!m_pKvsKernel)m_pKvsKernel = new KviKvsKernel();
}

void KviKvsKernel::done()
{
	if(m_pKvsKernel)
	{
		delete m_pKvsKernel;
		m_pKvsKernel = 0;
	}
}

#define COMPLETE_COMMAND_BY_DICT(__type,__dict) \
	{ \
		KviDictIterator<__type> it(*__dict); \
		int l = szCommandBegin.length(); \
		while(it.current()) \
		{ \
			if(KviQString::equalCIN(szCommandBegin,it.currentKey(),l)) \
				pMatches->append(new QString(it.currentKey())); \
			++it; \
		} \
	}


void KviKvsKernel::completeCommand(const QString &szCommandBegin,KviPtrList<QString> * pMatches)
{
	int idx = szCommandBegin.find(QChar('.'));
	if(idx == -1)
	{
		// no module name inside
		COMPLETE_COMMAND_BY_DICT(KviKvsCoreSimpleCommandExecRoutine,m_pCoreSimpleCommandExecRoutineDict)
		COMPLETE_COMMAND_BY_DICT(KviKvsSpecialCommandParsingRoutine,m_pSpecialCommandParsingRoutineDict)
		COMPLETE_COMMAND_BY_DICT(KviKvsCoreCallbackCommandExecRoutine,m_pCoreCallbackCommandExecRoutineDict)

		KviPtrList<QString> lModules;
		lModules.setAutoDelete(true);
		g_pModuleManager->completeModuleNames(szCommandBegin,&lModules);
		QString szEmpty = "";
		for(QString * pszModuleName = lModules.first();pszModuleName;pszModuleName = lModules.next())
			completeModuleCommand(*pszModuleName,szEmpty,pMatches);

		KviKvsAliasManager::instance()->completeCommand(szCommandBegin,pMatches);
	} else {
		// contains a module name
		QString szModuleName = szCommandBegin.left(idx);
		QString szRight = szCommandBegin.right(szCommandBegin.length() - (idx+1));
		completeModuleCommand(szModuleName,szRight,pMatches);
	}
}

void KviKvsKernel::completeModuleCommand(const QString &szModuleName,const QString &szCommandBegin,KviPtrList<QString> * pMatches)
{
	KviModule * pModule = g_pModuleManager->getModule(szModuleName.latin1());
	if(!pModule)return;

	KviPtrList<QString> lModuleMatches;
	lModuleMatches.setAutoDelete(true);
	pModule->completeCommand(szCommandBegin,&lModuleMatches);
	for(QString * pszModuleMatch = lModuleMatches.first();pszModuleMatch;pszModuleMatch = lModuleMatches.next())
	{
		QString * pszMatch = new QString(*pszModuleMatch);
		pszMatch->prepend(".");
		pszMatch->prepend(szModuleName);
		pMatches->append(pszMatch);
	}
}

void KviKvsKernel::completeFunction(const QString &szFunctionBegin,KviPtrList<QString> * pMatches)
{
	int idx = szFunctionBegin.find(QChar('.'));
	if(idx == -1)
	{
		// no module name inside

		KviDictIterator<KviKvsCoreFunctionExecRoutine> it(*m_pCoreFunctionExecRoutineDict);
		int l = szFunctionBegin.length();
		while(it.current())
		{
			if(KviQString::equalCIN(szFunctionBegin,it.currentKey(),l))
			{
				QString * pMatch = new QString(it.currentKey());
				//pMatch->prepend("$");
				pMatches->append(pMatch);
			}
			++it;
		}

		KviPtrList<QString> lModules;
		lModules.setAutoDelete(true);
		g_pModuleManager->completeModuleNames(szFunctionBegin,&lModules);
		QString szEmpty = "";
		for(QString * pszModuleName = lModules.first();pszModuleName;pszModuleName = lModules.next())
			completeModuleFunction(*pszModuleName,szEmpty,pMatches);

		KviPtrList<QString> lAliases;
		lAliases.setAutoDelete(true);

		KviKvsAliasManager::instance()->completeCommand(szFunctionBegin,&lAliases);
		for(QString * pszAlias = lAliases.first();pszAlias;pszAlias = lAliases.next())
		{
			QString * pszAliasMatch = new QString(*pszAlias);
			//pszAliasMatch->prepend("$");
			pMatches->append(pszAliasMatch);
		}
	} else {
		// contains a module name
		QString szModuleName = szFunctionBegin.left(idx);
		QString szRight = szFunctionBegin.right(szFunctionBegin.length() - (idx+1));
		completeModuleFunction(szModuleName,szRight,pMatches);
	}

	
}

void KviKvsKernel::completeModuleFunction(const QString &szModuleName,const QString &szCommandBegin,KviPtrList<QString> * pMatches)
{
	KviModule * pModule = g_pModuleManager->getModule(szModuleName.latin1());
	if(!pModule)return;

	KviPtrList<QString> lModuleMatches;
	lModuleMatches.setAutoDelete(true);
	pModule->completeFunction(szCommandBegin,&lModuleMatches);
	for(QString * pszModuleMatch = lModuleMatches.first();pszModuleMatch;pszModuleMatch = lModuleMatches.next())
	{
		QString * pszMatch = new QString(*pszModuleMatch);
		pszMatch->prepend(".");
		pszMatch->prepend(szModuleName);
		//pszMatch->prepend("$");
		pMatches->append(pszMatch);
	}
}
