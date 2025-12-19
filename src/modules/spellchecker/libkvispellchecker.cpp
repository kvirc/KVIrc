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

#ifdef COMPILE_ENCHANT_SUPPORT
#include <enchant++.h>

static enchant::Broker * g_pEnchantBroker = nullptr;
static KviPointerList<enchant::Dict> * g_pEnchantDicts = nullptr;
#endif

#ifdef COMPILE_WINSPELLCHECKER_SUPPORT
#include <spellcheck.h>
#include <winerror.h>

static ISpellCheckerFactory *g_pWinSpellFactory = nullptr;
static KviPointerList<ISpellChecker> * g_pWinSpellDicts = nullptr;
#endif

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

#ifdef COMPILE_ENCHANT_SUPPORT
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
#endif

static bool spellchecker_kvs_available_dictionaries(KviKvsModuleFunctionCall * c)
{
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETERS_END(c)
	KviKvsHash * pHash = new KviKvsHash;

#ifdef COMPILE_ENCHANT_SUPPORT
	g_pEnchantBroker->list_dicts(spellchecker_enumerate_dicts, pHash);
#endif

#ifdef COMPILE_WINSPELLCHECKER_SUPPORT
	IEnumString *enumDicts;

	if(SUCCEEDED(g_pWinSpellFactory->get_SupportedLanguages(&enumDicts))) {
		HRESULT hr = S_OK;
		while (S_OK == hr) {
			LPOLESTR string = nullptr;
			hr = enumDicts->Next(1, &string, nullptr);
			if (S_OK == hr) {
				pHash->set(QString::fromWCharArray(string), new KviKvsVariant(QStringLiteral("Windows Spellchecker")));
				CoTaskMemFree(string);
			}
		}
		enumDicts->Release();
	}
#endif

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

	bool bResult = false;

#ifdef COMPILE_ENCHANT_SUPPORT
	bResult = g_pEnchantDicts->isEmpty();
	KviPointerListIterator<enchant::Dict> it(*g_pEnchantDicts);
	for(bool b = it.moveFirst(); b; b = it.moveNext())
		bResult |= (*it)->check(szWord.toStdString());
#endif

#ifdef COMPILE_WINSPELLCHECKER_SUPPORT
	bResult = g_pWinSpellDicts->isEmpty();

	IEnumSpellingError *errors = nullptr;
	ISpellingError *error = nullptr;

	KviPointerListIterator<ISpellChecker> it(*g_pWinSpellDicts);
	for(bool b = it.moveFirst(); b; b = it.moveNext()) {
		if(SUCCEEDED((*it)->Check(szWord.toStdWString().c_str(), &errors))) {
			if (errors->Next(&error) == S_OK) {
				error->Release();
			} else {
				// no error found for this dict
				bResult = true;
			}
			errors->Release();
		}
	}
#endif

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

#ifdef COMPILE_ENCHANT_SUPPORT
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
#endif

#ifdef COMPILE_WINSPELLCHECKER_SUPPORT
	if(!g_pWinSpellDicts->isEmpty())
	{
		IEnumString *suggestions;

		KviPointerListIterator<ISpellChecker> it(*g_pWinSpellDicts);
		for(bool b = it.moveFirst(); b; b = it.moveNext())
		{
			if(SUCCEEDED((*it)->Suggest(szWord.toStdWString().c_str(), &suggestions))) {
				HRESULT hr = S_OK;
				while (S_OK == hr) {
					LPOLESTR suggestion = nullptr;
					hr = suggestions->Next(1, &suggestion, nullptr);
					if (S_OK == hr) {
						hAllSuggestions.insert(QString::fromWCharArray(suggestion), 1);
						CoTaskMemFree(suggestion);
					}
				}
				suggestions->Release();
			}
		}
	}
#endif

	KviKvsArray * pArray = new KviKvsArray();

	for(const auto & szSuggestion : hAllSuggestions.keys())
		pArray->append(new KviKvsVariant(szSuggestion));

	c->returnValue()->setArray(pArray);
	return true;
}

static void spellchecker_reload_dicts()
{
#ifdef COMPILE_ENCHANT_SUPPORT
	while(!g_pEnchantDicts->isEmpty())
		delete g_pEnchantDicts->takeFirst();
#endif
#ifdef COMPILE_WINSPELLCHECKER_SUPPORT
	while(!g_pWinSpellDicts->isEmpty()) {
		auto pDict = g_pWinSpellDicts->takeFirst();
		pDict->Release();
		delete pDict;
	}
#endif

	const QStringList & wantedDictionaries = KVI_OPTION_STRINGLIST(KviOption_stringlistSpellCheckerDictionaries);
	foreach(QString szLang, wantedDictionaries)
	{
		if(szLang.isEmpty())
			continue;

#ifdef COMPILE_ENCHANT_SUPPORT
		try {
			enchant::Dict * pDict = g_pEnchantBroker->request_dict(szLang.toUtf8().data());
			if(pDict)
			{
				g_pEnchantDicts->append(pDict);
			}
		} catch (enchant::Exception e) {
			qDebug("Can't load spellchecker dictionary %s: %s", szLang.toUtf8().data(), e.what());
		}
#endif
#ifdef COMPILE_WINSPELLCHECKER_SUPPORT
		ISpellChecker *pDict;
		if(SUCCEEDED(g_pWinSpellFactory->CreateSpellChecker(szLang.toStdWString().c_str(), &pDict))) {
			g_pWinSpellDicts->append(pDict);
		} else {
			qDebug("Can't load spellchecker dictionary %s", szLang.toUtf8().data());
		}
#endif
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
#ifdef COMPILE_ENCHANT_SUPPORT
	g_pEnchantBroker = new enchant::Broker();
	g_pEnchantDicts = new KviPointerList<enchant::Dict>(/* bAutoDelete = */ false);
#endif

#ifdef COMPILE_WINSPELLCHECKER_SUPPORT
	if (FAILED(CoCreateInstance(__uuidof(SpellCheckerFactory), nullptr,
			CLSCTX_INPROC_SERVER, IID_PPV_ARGS (&g_pWinSpellFactory)))) {
		return false;
	}
	g_pWinSpellDicts = new KviPointerList<ISpellChecker>(/* bAutoDelete = */ false);
#endif

	spellchecker_reload_dicts();

	KVSM_REGISTER_SIMPLE_COMMAND(m, "reloadDictionaries", spellchecker_kvs_reload_dictionaries);
	KVSM_REGISTER_FUNCTION(m, "availableDictionaries", spellchecker_kvs_available_dictionaries);
	KVSM_REGISTER_FUNCTION(m, "check", spellchecker_kvs_check);
	KVSM_REGISTER_FUNCTION(m, "suggestions", spellchecker_kvs_suggestions);
	return true;
}

static bool spellchecker_module_cleanup(KviModule *)
{
#ifdef COMPILE_ENCHANT_SUPPORT
	while(!g_pEnchantDicts->isEmpty())
		delete g_pEnchantDicts->takeFirst();

	delete g_pEnchantDicts;
	g_pEnchantDicts = nullptr;
	delete g_pEnchantBroker;
	g_pEnchantBroker = nullptr;
#endif

#ifdef COMPILE_WINSPELLCHECKER_SUPPORT
	while(!g_pWinSpellDicts->isEmpty()) {
		auto pDict = g_pWinSpellDicts->takeFirst();
		pDict->Release();
		delete pDict;
	}

	g_pWinSpellFactory->Release();
	delete g_pWinSpellFactory;
	g_pWinSpellFactory = nullptr;
#endif

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
