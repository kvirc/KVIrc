//=============================================================================
//
//   File : libkvilamerizer.cpp
//   Creation date : Sat Jan 20 2002 17:06:12 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "libkvilamerizer.h"

#include "KviModule.h"
#include "kvi_debug.h"
#include "KviLocale.h"

/*
	@doc: lamerizer
	@type:
		module
	@short:
		The Lamerizer text transformation engine
	@title:
		The lamerizer module
	@body:
		This is an example of what you should [b]not[/b] do with a text transformation
		engine. Do [b]not[/b] use it! :D
*/

#ifdef COMPILE_CRYPT_SUPPORT

#include "KviMemory.h"
#include "KviPointerList.h"
#include "KviCryptEngineDescription.h"

static KviPointerList<KviCryptEngine> * g_pEngineList = nullptr;

KviLamerizerEngine::KviLamerizerEngine(bool bLight)
    : KviCryptEngine()
{
	m_bLight = bLight;
	g_pEngineList->append(this);
}

KviLamerizerEngine::~KviLamerizerEngine()
{
	g_pEngineList->removeRef(this);
}

bool KviLamerizerEngine::init(const char *, int, const char *, int)
{
	return true;
}

static char subst_table[256] = {
	0, 0, 0, 0, 0, 0, 0, 0,                 // 007
	0, 0, 0, 0, 0, 0, 0, 0,                 // 015
	0, 0, 0, 0, 0, 0, 0, 0,                 // 023
	0, 0, 0, 0, 0, 0, 0, 0,                 // 031
	0, 0, 0, 0, 0, 0, 0, 0,                 // 039 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 047 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 055 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 063 :
	0, '4', '8', 'C', 'D', '3', 'F', 'G',   // 071 :
	'H', '|', 'J', 'K', 'L', 'm', 'N', 'O', // 079 :
	'P', 'q', 'R', '5', '7', 'U', 'V', 'W', // 087 :
	'x', 'Y', '2', 0, 0, 0, 0, 0,           // 095 :
	0, '4', 'b', 'c', 'd', '3', 'f', '9',   // 103 :
	'h', '|', 'j', 'k', '1', 'm', 'n', '0',
	'p', 'q', 'r', '5', '7', 'u', 'v', 'w', // 119 :
	'x', 'y', '2', 0, 0, 0, 0, 0,           // 127 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 135 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 143 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 151 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 159 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 167 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 175 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 183 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 191 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 199 :
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,                 // 215 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 223 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 231 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 239 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 247 :
	0, 0, 0, 0, 0, 0, 0, 0
};

static char subst_table_light[256] = {
	0, 0, 0, 0, 0, 0, 0, 0,                 // 007
	0, 0, 0, 0, 0, 0, 0, 0,                 // 015
	0, 0, 0, 0, 0, 0, 0, 0,                 // 023
	0, 0, 0, 0, 0, 0, 0, 0,                 // 031
	0, 0, 0, 0, 0, 0, 0, 0,                 // 039 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 047 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 055 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 063 :
	0, '4', 'B', 'C', 'D', '3', 'F', 'G',   // 071 :
	'H', '|', 'J', 'K', 'L', 'm', 'N', 'O', // 079 :
	'P', 'q', 'R', '5', '7', 'U', 'V', 'W', // 087 :
	'x', 'Y', 'Z', 0, 0, 0, 0, 0,           // 095 :
	0, '4', 'b', 'c', 'd', '3', 'f', 'G',   // 103 :
	'h', '|', 'j', 'k', '1', 'm', 'n', '0',
	'p', 'q', 'r', '5', '7', 'u', 'v', 'w', // 119 :
	'x', 'y', 'z', 0, 0, 0, 0, 0,           // 127 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 135 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 143 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 151 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 159 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 167 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 175 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 183 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 191 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 199 :
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,                 // 215 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 223 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 231 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 239 :
	0, 0, 0, 0, 0, 0, 0, 0,                 // 247 :
	0, 0, 0, 0, 0, 0, 0, 0
};

KviCryptEngine::EncryptResult KviLamerizerEngine::encrypt(const char * plainText, KviCString & outBuffer)
{
	outBuffer = plainText;
	unsigned char * aux = (unsigned char *)outBuffer.ptr();
	if(m_bLight)
	{
		while(*aux)
		{
			if(subst_table_light[*aux])
			{
				*aux = subst_table_light[*aux];
			}
			aux++;
		}
	}
	else
	{
		while(*aux)
		{
			if(subst_table[*aux])
			{
				*aux = subst_table[*aux];
			}
			aux++;
		}
	}

	if(!m_bLight)
	{
		outBuffer.replaceAll("F", "Ph");
		outBuffer.replaceAll("V", "\\/");
	}

	return KviCryptEngine::Encoded;
}

KviCryptEngine::DecryptResult KviLamerizerEngine::decrypt(const char * inBuffer, KviCString & plainText)
{
	plainText = inBuffer;
	return KviCryptEngine::DecryptOkWasPlainText;
}

static KviCryptEngine * allocLamerizerEngine()
{
	return new KviLamerizerEngine(false);
}

static KviCryptEngine * allocLightLamerizerEngine()
{
	return new KviLamerizerEngine(true);
}

static void deallocLamerizerEngine(KviCryptEngine * e)
{
	delete e;
}

#endif

//
// module routines
//

static bool lamerizer_module_init(KviModule * m)
{
#ifdef COMPILE_CRYPT_SUPPORT
	g_pEngineList = new KviPointerList<KviCryptEngine>;
	g_pEngineList->setAutoDelete(false);

	// FIXME: Maybe convert this repeated code to a function eh ?

	KviCryptEngineDescription * d = new KviCryptEngineDescription;
	d->m_szName = "Lamerizer";
	d->m_szAuthor = "Szymon Stefanek and Jan Wagner";
	d->m_szDescription = __tr2qs("A really lame text transformation engine :D");
	d->m_iFlags = KviCryptEngine::CanEncrypt;
	d->m_allocFunc = allocLamerizerEngine;
	d->m_deallocFunc = deallocLamerizerEngine;
	m->registerCryptEngine(d);

	d = new KviCryptEngineDescription;
	d->m_szName = "LamerizerLight";
	d->m_szAuthor = "Szymon Stefanek and Jan Wagner";
	d->m_szDescription = __tr2qs("A really lame text transformation engine: Light Version.");
	d->m_iFlags = KviCryptEngine::CanEncrypt;
	d->m_allocFunc = allocLightLamerizerEngine;
	d->m_deallocFunc = deallocLamerizerEngine;
	m->registerCryptEngine(d);

	return true;
#else
	return false;
#endif
}

static bool lamerizer_module_cleanup(KviModule * m)
{
#ifdef COMPILE_CRYPT_SUPPORT
	while(g_pEngineList->first())
		delete g_pEngineList->first();
	delete g_pEngineList;
	g_pEngineList = nullptr;
	m->unregisterCryptEngines();
	return true;
#else
	return false;
#endif
}

static bool lamerizer_module_can_unload(KviModule *)
{
#ifdef COMPILE_CRYPT_SUPPORT
	return g_pEngineList->isEmpty();
#else
	return true;
#endif
}

KVIRC_MODULE(
    "Lamerizer crypt engine",
    "4.0.0",
    "Szymon Stefanek <pragma at kvirc dot net> \n Jan Wagner <istari@kvirc.net>",
    "Exports the lamerizer text transformation engine",
    lamerizer_module_init,
    lamerizer_module_can_unload,
    0,
    lamerizer_module_cleanup,
    0)
