//=============================================================================
//
//   File : libkvilanguage.cpp
//   Creation date : Fri 25 Dec 20:16:49 2004 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#include "KviApplication.h"
#include "KviLocale.h"

#include <QFileInfo>

#include "detector.h"

/*
	@doc: language.detect
	@type:
		function
	@title:
		$language.detect
	@short:
		Tries to detect the language and encoding of a string
	@syntax:
		<hash> language.detect(<text: string>)
	@description:
		Tries to detect the language and encoding of the string specified
		as [text]. The accuracy of the detection mainly depends on the length
		of the supplied text. Good results can be achieved with some thousands
		characters.[br]
		This command is exported by the [i]language[/i] module.
	@examples:
		[example]
			%dati = $language.detect("I'm a lord and I speak perfect English.");
			if(%dati{"error"} != "")echo "Language detection failed: " %dati{"error"};
				%count = %dati{"matchcount"};
				for(%i=0;%i<%count;%i++)
			{
				echo "LANGUAGE " %i " : " %dati{"matches"}[%i]{"language"};
				echo "ENCODING " %i " : " %dati{"matches"}[%i]{"encoding"};
				echo "SCORE " %i " : "%dati{"matches"}[%i]{"score"};
			}
		echo "ACCURACY: " %dati{"accuracy"};
		[/example]
*/

static bool language_kvs_cmd_detect(KviKvsModuleFunctionCall * c)
{
	QString text, error;
	KviKvsArray * matches;
	KviKvsHash *match, *ret;
	LanguageAndEncodingResult r;
	int matchcount = DLE_NUM_BEST_MATCHES;

	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("text", KVS_PT_STRING, KVS_PF_OPTIONAL, text)
	KVSM_PARAMETERS_END(c)

	//on error we return an empty array
	matches = new KviKvsArray();

	if(text.isEmpty())
	{
		error = "err_notext";
		matchcount = 0;
	}
	else
	{
		detect_language_and_encoding(text.toUtf8().data(), &r, 0);

		for(int i = 0; i < matchcount; i++)
		{
			match = new KviKvsHash();
			match->set("language", new KviKvsVariant(QString(r.match[i].szLanguage)));
			match->set("encoding", new KviKvsVariant(r.match[i].szEncoding));
			match->set("score", new KviKvsVariant((kvs_real_t)r.match[i].dScore));
			matches->set(i, new KviKvsVariant(match));
		}
	}

	ret = new KviKvsHash();
	ret->set("matches", new KviKvsVariant(matches));
	ret->set("matchcount", new KviKvsVariant((kvs_int_t)matchcount));
	ret->set("error", new KviKvsVariant(error));
	ret->set("accuracy", new KviKvsVariant((kvs_real_t)r.dAccuracy));

	c->returnValue()->setHash(ret);

	return true;
}

static bool language_module_init(KviModule * m)
{
	KVSM_REGISTER_FUNCTION(m, "detect", language_kvs_cmd_detect);
	return true;
}

static bool language_module_cleanup(KviModule *)
{
	return true;
}

KVIRC_MODULE(
    "Language",                                                     // module name
    "4.0.0",                                                        // module version
    "Copyright (C) 2004 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
    "A simple statistical language/encoding detector",
    language_module_init,
    0,
    0,
    language_module_cleanup,
    0)
