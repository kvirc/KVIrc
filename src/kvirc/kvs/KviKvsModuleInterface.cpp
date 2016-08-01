//=============================================================================
//
//   File : KviKvsModuleInterface.cpp
//   Creation date : Tue 16 Dec 2003 00:27:54 by Szymon Stefanek
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

#include "KviKvsModuleInterface.h"
#include "KviKvsEventManager.h"
#include "KviModule.h"
#include "KviModuleManager.h"
#include "KviLocale.h"
#include "KviKvsTreeNodeData.h"
#include "KviKvsTreeNodeDataList.h"

bool KviKvsModuleCallbackCommandCall::getParameterCode(unsigned int uParamIdx, QString & szParamBuffer)
{
	if(!m_pParameterDataList)
		return false;
	KviKvsTreeNodeData * d = m_pParameterDataList->item(uParamIdx);
	if(!d)
		return false;
	const QChar * pBegin = d->location();
	const QChar * pEnd = d->endingLocation();
	if(!(pBegin && pEnd))
		return false;
	szParamBuffer.setUnicode(pBegin, pEnd - pBegin);
	szParamBuffer = szParamBuffer.trimmed();
	return true;
}

KviKvsModuleInterface::KviKvsModuleInterface()
{
	m_pModuleSimpleCommandExecRoutineDict = new KviPointerHashTable<QString, KviKvsModuleSimpleCommandExecRoutine>(17, false);
	m_pModuleSimpleCommandExecRoutineDict->setAutoDelete(true);
	m_pModuleFunctionExecRoutineDict = new KviPointerHashTable<QString, KviKvsModuleFunctionExecRoutine>(17, false);
	m_pModuleFunctionExecRoutineDict->setAutoDelete(true);
	m_pModuleCallbackCommandExecRoutineDict = new KviPointerHashTable<QString, KviKvsModuleCallbackCommandExecRoutine>(17, false);
	m_pModuleCallbackCommandExecRoutineDict->setAutoDelete(true);
}

KviKvsModuleInterface::~KviKvsModuleInterface()
{
	kvsUnregisterAllEventHandlers();
	delete m_pModuleSimpleCommandExecRoutineDict;
	delete m_pModuleFunctionExecRoutineDict;
	delete m_pModuleCallbackCommandExecRoutineDict;
}

#define COMPLETE_WORD_BY_DICT(word, list, type, dict)         \
	{                                                           \
		KviPointerHashTableIterator<QString, type> it(*dict);     \
		int l = word.length();                                    \
		while(it.current())                                       \
		{                                                         \
			if(KviQString::equalCIN(word, it.currentKey(), l))      \
				list.push_back(it.currentKey());                      \
			++it;                                                   \
		}                                                         \
	}

void KviKvsModuleInterface::completeCommand(const QString & szCommandBegin, std::vector<QString> & pMatches)
{
	COMPLETE_WORD_BY_DICT(szCommandBegin, pMatches, KviKvsModuleSimpleCommandExecRoutine, m_pModuleSimpleCommandExecRoutineDict)
	COMPLETE_WORD_BY_DICT(szCommandBegin, pMatches, KviKvsModuleCallbackCommandExecRoutine, m_pModuleCallbackCommandExecRoutineDict)
}

void KviKvsModuleInterface::completeFunction(const QString & szFunctionBegin, std::vector<QString> & pMatches)
{
	COMPLETE_WORD_BY_DICT(szFunctionBegin, pMatches, KviKvsModuleFunctionExecRoutine, m_pModuleFunctionExecRoutineDict)
}

void KviKvsModuleInterface::kvsRegisterSimpleCommand(const QString & szCommand, KviKvsModuleSimpleCommandExecRoutine r)
{
	m_pModuleSimpleCommandExecRoutineDict->replace(szCommand, new KviKvsModuleSimpleCommandExecRoutine(r));
}

void KviKvsModuleInterface::kvsRegisterCallbackCommand(const QString & szCommand, KviKvsModuleCallbackCommandExecRoutine r)
{
	m_pModuleCallbackCommandExecRoutineDict->replace(szCommand, new KviKvsModuleCallbackCommandExecRoutine(r));
}

void KviKvsModuleInterface::kvsRegisterFunction(const QString & szFunction, KviKvsModuleFunctionExecRoutine r)
{
	m_pModuleFunctionExecRoutineDict->replace(szFunction, new KviKvsModuleFunctionExecRoutine(r));
}

bool KviKvsModuleInterface::kvsRegisterAppEventHandler(unsigned int iEventIdx, KviKvsModuleEventHandlerRoutine r)
{
	KviKvsModuleEventHandler * h = new KviKvsModuleEventHandler(this, r);
	if(!KviKvsEventManager::instance()->addAppHandler(iEventIdx, h))
	{
		delete h;
		return false;
	}
	return true;
}

bool KviKvsModuleInterface::kvsRegisterRawEventHandler(unsigned int iRawIdx, KviKvsModuleEventHandlerRoutine r)
{
	KviKvsModuleEventHandler * h = new KviKvsModuleEventHandler(this, r);
	if(!KviKvsEventManager::instance()->addRawHandler(iRawIdx, h))
	{
		delete h;
		return false;
	}
	return true;
}

void KviKvsModuleInterface::kvsUnregisterAppEventHandler(unsigned int uEventIdx)
{
	KviKvsEventManager::instance()->removeModuleAppHandler(uEventIdx, this);
}

void KviKvsModuleInterface::kvsUnregisterRawEventHandler(unsigned int uRawIdx)
{
	KviKvsEventManager::instance()->removeModuleRawHandler(uRawIdx, this);
}

void KviKvsModuleInterface::kvsUnregisterAllAppEventHandlers()
{
	KviKvsEventManager::instance()->removeAllModuleAppHandlers(this);
}

void KviKvsModuleInterface::kvsUnregisterAllRawEventHandlers()
{
	KviKvsEventManager::instance()->removeAllModuleRawHandlers(this);
}

void KviKvsModuleInterface::kvsUnregisterAllEventHandlers()
{
	kvsUnregisterAllAppEventHandlers();
	kvsUnregisterAllRawEventHandlers();
}

static bool default_module_kvs_cmd_load(KviKvsModuleCommandCall *)
{
	return true;
}

static bool default_module_kvs_cmd_unload(KviKvsModuleCommandCall * c)
{
	if(c->module()->isLocked())
	{
		if(!c->switches()->find('f', "force"))
		{
			c->warning(__tr2qs_ctx("Can't unload the module: it has locked itself in memory", "kvs"));
			return true;
		}
	}
	g_pModuleManager->unloadModule(c->module()->name());
	return true;
}

void KviKvsModuleInterface::registerDefaultCommands()
{
	kvsRegisterSimpleCommand("load", default_module_kvs_cmd_load);
	kvsRegisterSimpleCommand("unload", default_module_kvs_cmd_unload);
}
void KviKvsModuleInterface::getAllFunctionsCommandsModule(QStringList * list, QString & szModuleName)
{
	KviPointerHashTableIterator<QString, KviKvsModuleFunctionExecRoutine> it(*m_pModuleFunctionExecRoutineDict);
	while(it.current())
	{
		QString szF("$" + szModuleName + ".");
		szF.append(it.currentKey());
		list->append(szF);
		++it;
	}
	KviPointerHashTableIterator<QString, KviKvsModuleSimpleCommandExecRoutine> it2(*m_pModuleSimpleCommandExecRoutineDict);
	while(it2.current())
	{
		QString szF(szModuleName + ".");
		szF.append(it2.currentKey());
		list->append(szF);
		++it2;
	}
}
