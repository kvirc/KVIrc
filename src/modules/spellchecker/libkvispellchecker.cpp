//=============================================================================
//
//   File : libkvispellchecker.cpp
//   Creation date : Thu Dec 26 2014 15:12:12 GMT by Alexey Sokolov
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright © 2014 Alexey Sokolov <sokolov@google.com>
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

#include "KviModule.h"
#include "KviOptions.h"

#include <enchant++.h>

static enchant::Broker * g_pEnchantBroker = nullptr;
static KviPointerList<enchant::Dict> * g_pEnchantDicts = nullptr;

/*
	@doc: spellchecker.available_dictionaries
	@type:
		function
	@title:
		$spellchecker.available_dictionaries
	@short:
		Return available dictionaries.
	@syntax:
		<hash> $spellchecker.available_dictionaries
	@description:
		This function returns a hash. For every supported dictionary, key is the language code,
		value is name of provider for that language (e.g. Aspell).
*/

static void spellchecker_enumerate_dicts(
    const char * szLang,
    const char * /*szName*/,
    const char * szDesc,
    const char * /*szFile*/,
    void * pData)
{
	KviKvsHash * pHash = reinterpret_cast<KviKvsHash *>(pData);
	pHash->set(szLang, new KviKvsVariant(szDesc));
}

static bool spellchecker_kvs_available_dictionaries(KviKvsModuleFunctionCall * c)
{
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETERS_END(c)
	KviKvsHash * pHash = new KviKvsHash;
	g_pEnchantBroker->list_dicts(spellchecker_enumerate_dicts, pHash);
	c->returnValue()->setHash(pHash);
	return true;
}

/*
	@doc: spellchecker.check
	@type:
		function
	@title:
		$spellchecker.check
	@short:
		Check a single work for spelling mistakes.
	@syntax:
		<bool> $spellchecker.check(<word:string>)
	@description:
		This function returns true if the word is spelled correctly.
*/

static bool spellchecker_kvs_check(KviKvsModuleFunctionCall * c)
{
	QString szWord;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("word", KVS_PT_STRING, 0, szWord)
	KVSM_PARAMETERS_END(c)

	bool bResult = g_pEnchantDicts->isEmpty();
	KviPointerListIterator<enchant::Dict> it(*g_pEnchantDicts);
	for(bool b = it.moveFirst(); b; b = it.moveNext())
		bResult |= (*it)->check(szWord.toStdString());

	c->returnValue()->setBoolean(bResult);
	return true;
}

/*
	@doc: spellchecker.suggestions
	@type:
		function
	@title:
		$spellchecker.suggestions
	@short:
		Get spelling suggestions for a single word
	@syntax:
		<array> $spellchecker.suggestions(<word:string>)
	@description:
		This function returns the suggestions for the specified word.
		If the word seems to be spelled correctly or there are no dictionaries
		selected then the function returns an empty array.
*/

static bool spellchecker_kvs_suggestions(KviKvsModuleFunctionCall * c)
{
	QString szWord;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("word", KVS_PT_STRING, 0, szWord)
	KVSM_PARAMETERS_END(c)

	QHash<QString, int> hAllSuggestions;

	if(!g_pEnchantDicts->isEmpty())
	{
		std::vector<std::string> suggestions;

		KviPointerListIterator<enchant::Dict> it(*g_pEnchantDicts);
		for(bool b = it.moveFirst(); b; b = it.moveNext())
		{
			(*it)->suggest(szWord.toStdString(), suggestions);
			for (auto & suggestion : suggestions) {
				hAllSuggestions.insert(QString::fromStdString(suggestion), 1);
			}
		}
	}

	KviKvsArray * pArray = new KviKvsArray();

	for(const auto & szSuggestion : hAllSuggestions.keys())
		pArray->append(new KviKvsVariant(szSuggestion));

	c->returnValue()->setArray(pArray);
	return true;
}

static void spellchecker_reload_dicts()
{
	while(!g_pEnchantDicts->isEmpty())
		delete g_pEnchantDicts->takeFirst();

	const QStringList & wantedDictionaries = KVI_OPTION_STRINGLIST(KviOption_stringlistSpellCheckerDictionaries);
	foreach(QString szLang, wantedDictionaries)
	{
		if(szLang.isEmpty())
			continue;

		try {
			enchant::Dict * pDict = g_pEnchantBroker->request_dict(szLang.toUtf8().data());
			if(pDict)
			{
				g_pEnchantDicts->append(pDict);
			}
		} catch (enchant::Exception e) {
			qDebug("Can't load spellchecker dictionary %s: %s", szLang.toUtf8().data(), e.what());
		}
	}
}

/*
	@doc: spellchecker.reloadDictionaries
	@type:
		command
	@title:
		spellchecker.reloadDictionaries
	@short:
		Reload spell checker dictionaries
	@syntax:
		spellchecker.reloadDictionaries
	@description:
		Reload spell checker dictionaries
*/

static bool spellchecker_kvs_reload_dictionaries(KviKvsModuleCommandCall * c)
{
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETERS_END(c)
	spellchecker_reload_dicts();
	return true;
}

static bool spellchecker_module_init(KviModule * m)
{
	g_pEnchantBroker = new enchant::Broker();
	g_pEnchantDicts = new KviPointerList<enchant::Dict>(/* bAutoDelete = */ false);

	spellchecker_reload_dicts();

	KVSM_REGISTER_SIMPLE_COMMAND(m, "reloadDictionaries", spellchecker_kvs_reload_dictionaries);
	KVSM_REGISTER_FUNCTION(m, "availableDictionaries", spellchecker_kvs_available_dictionaries);
	KVSM_REGISTER_FUNCTION(m, "check", spellchecker_kvs_check);
	KVSM_REGISTER_FUNCTION(m, "suggestions", spellchecker_kvs_suggestions);
	return true;
}

static bool spellchecker_module_cleanup(KviModule *)
{
	while(!g_pEnchantDicts->isEmpty())
		delete g_pEnchantDicts->takeFirst();

	delete g_pEnchantDicts;
	g_pEnchantDicts = nullptr;
	delete g_pEnchantBroker;
	g_pEnchantBroker = nullptr;
	return true;
}

static bool spellchecker_module_can_unload(KviModule *)
{
	return false;
}

KVIRC_MODULE(
    "SpellChecker",                                                  // module name
    "4.0.0",                                                         // module version
    "Copyright (C) 2014 Alexey Sokolov (sokolov at google dot com)", // author & (C)
    "Spell checker",
    spellchecker_module_init,
    spellchecker_module_can_unload,
    0,
    spellchecker_module_cleanup,
    0)
