//=============================================================================
//
//   File : KviKvsKernel.cpp
//   Creation date : Tue 30 Sep 2003 05.12 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviKvsKernel.h"
#include "KviKvsParser.h"
#include "KviKvsHash.h"
#include "KviKvsAliasManager.h"
#include "KviKvsCoreSimpleCommands.h"
#include "KviKvsCoreFunctions.h"
#include "KviKvsCoreCallbackCommands.h"
#include "KviKvsSwitchList.h"
#include "KviKvsVariantList.h"
#include "KviKvsScript.h"
#include "KviKvsObjectController.h"
#include "KviKvsAsyncOperation.h"
#include "KviModuleManager.h"

#include <QDir>
KviKvsKernel * KviKvsKernel::m_pKvsKernel = nullptr;

//
// CONSTRUCTION AND DESTRUCTION
//

KviKvsKernel::KviKvsKernel()
{
	m_pKvsKernel = this;

	m_pSpecialCommandParsingRoutineDict = new KviPointerHashTable<QString, KviKvsSpecialCommandParsingRoutine>(17, false);
	m_pSpecialCommandParsingRoutineDict->setAutoDelete(true);
	m_pCoreSimpleCommandExecRoutineDict = new KviPointerHashTable<QString, KviKvsCoreSimpleCommandExecRoutine>(51, false);
	m_pCoreSimpleCommandExecRoutineDict->setAutoDelete(true);
	m_pCoreFunctionExecRoutineDict = new KviPointerHashTable<QString, KviKvsCoreFunctionExecRoutine>(51, false);
	m_pCoreFunctionExecRoutineDict->setAutoDelete(true);
	m_pCoreCallbackCommandExecRoutineDict = new KviPointerHashTable<QString, KviKvsCoreCallbackCommandExecRoutine>(17, false);
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

//
// INSTANCE MANAGEMENT
//

void KviKvsKernel::init()
{
	if(!m_pKvsKernel)
		m_pKvsKernel = new KviKvsKernel();
}

void KviKvsKernel::done()
{
	if(m_pKvsKernel)
	{
		delete m_pKvsKernel;
		m_pKvsKernel = nullptr;
	}
}

#define COMPLETE_COMMAND_BY_DICT(__type, __dict)                     \
	{                                                                  \
		KviPointerHashTableIterator<QString, __type> it(*__dict);        \
		int l = szCommandBegin.length();                                 \
		while(it.current())                                              \
		{                                                                \
			if(KviQString::equalCIN(szCommandBegin, it.currentKey(), l))   \
				pMatches.push_back(it.currentKey());                         \
			++it;                                                          \
		}                                                                \
	}

void KviKvsKernel::completeCommand(const QString & szCommandBegin, std::vector<QString> & pMatches)
{
	int idx = szCommandBegin.indexOf(QChar('.'));
	if(idx == -1)
	{
		// no module name inside
		COMPLETE_COMMAND_BY_DICT(KviKvsCoreSimpleCommandExecRoutine, m_pCoreSimpleCommandExecRoutineDict)
		COMPLETE_COMMAND_BY_DICT(KviKvsSpecialCommandParsingRoutine, m_pSpecialCommandParsingRoutineDict)
		COMPLETE_COMMAND_BY_DICT(KviKvsCoreCallbackCommandExecRoutine, m_pCoreCallbackCommandExecRoutineDict)

		g_pModuleManager->completeModuleNames(szCommandBegin, pMatches);

		KviKvsAliasManager::instance()->completeCommand(szCommandBegin, pMatches);
	}
	else
	{
		// contains a module name
		QString szModuleName = szCommandBegin.left(idx);
		QString szRight = szCommandBegin.right(szCommandBegin.length() - (idx + 1));
		completeModuleCommand(szModuleName, szRight, pMatches);
	}
}

void KviKvsKernel::completeModuleCommand(const QString & szModuleName, const QString & szCommandBegin, std::vector<QString> & pMatches)
{
	KviModule * pModule = g_pModuleManager->getModule(szModuleName);
	if(!pModule)
		return;

	std::vector<QString> lModuleMatches;

	pModule->completeCommand(szCommandBegin, lModuleMatches);
	for(auto & pszModuleMatch : lModuleMatches)
	{
		pszModuleMatch.prepend(".");
		pszModuleMatch.prepend(szModuleName);
		pMatches.push_back(std::move(pszModuleMatch));
	}
}

void KviKvsKernel::completeFunction(const QString & szFunctionBegin, std::vector<QString> & pMatches)
{
	int idx = szFunctionBegin.indexOf(QChar('.'));
	if(idx == -1)
	{
		// no module name inside

		KviPointerHashTableIterator<QString, KviKvsCoreFunctionExecRoutine> it(*m_pCoreFunctionExecRoutineDict);
		int l = szFunctionBegin.length();
		while(it.current())
		{
			if(KviQString::equalCIN(szFunctionBegin, it.currentKey(), l))
				pMatches.push_back(it.currentKey());
			++it;
		}

		g_pModuleManager->completeModuleNames(szFunctionBegin, pMatches);

		std::vector<QString> lAliases;

		KviKvsAliasManager::instance()->completeCommand(szFunctionBegin, lAliases);
		for(auto & pszAlias : lAliases)
			pMatches.push_back(std::move(pszAlias));
	}
	else
	{
		// contains a module name
		QString szModuleName = szFunctionBegin.left(idx);
		QString szRight = szFunctionBegin.right(szFunctionBegin.length() - (idx + 1));
		completeModuleFunction(szModuleName, szRight, pMatches);
	}
}

void KviKvsKernel::completeModuleFunction(const QString & szModuleName, const QString & szCommandBegin, std::vector<QString> & pMatches)
{
	KviModule * pModule = g_pModuleManager->getModule(szModuleName);
	if(!pModule)
		return;

	std::vector<QString> lModuleMatches;

	pModule->completeFunction(szCommandBegin, lModuleMatches);
	for(auto & pszModuleMatch : lModuleMatches)
	{
		pszModuleMatch.prepend(".");
		pszModuleMatch.prepend(szModuleName);
		pszModuleMatch.prepend("$");
		pMatches.push_back(std::move(pszModuleMatch));
	}
}

void KviKvsKernel::getAllFunctionsCommandsCore(QStringList * list)
{
	KviPointerHashTableIterator<QString, KviKvsCoreFunctionExecRoutine> it(*m_pCoreFunctionExecRoutineDict);
	while(it.current())
	{
		list->append("$" + it.currentKey());
		++it;
	}
	KviPointerHashTableIterator<QString, KviKvsCoreSimpleCommandExecRoutine> it2(*m_pCoreSimpleCommandExecRoutineDict);
	while(it2.current())
	{
		list->append(it2.currentKey());
		++it2;
	}
}
