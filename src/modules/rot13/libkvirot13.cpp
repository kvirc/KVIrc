//=============================================================================
//
//   File : libkvirot13.cpp
//   Creation date : Fri Jan 30 2002 09:25:15 GMT by Aeriana
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2009 Aeriana (aeriana at quasarnet dot org)
//   Copyright (C) 2002-2009 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "libkvirot13.h"

#include "kvi_module.h"
#include "kvi_debug.h"
#include "kvi_locale.h"



/*
	@doc: rot13
	@type:
		module
	@short:
		The ROT13 text transformation engine
	@title:
		The ROT13 module
	@body:
		Performs the "rotate by 13 places" substitution.
*/



#ifdef COMPILE_CRYPT_SUPPORT

	#include "kvi_memmove.h"
	#include "kvi_malloc.h"

	#include "kvi_pointerlist.h"

	static KviPointerList<KviCryptEngine> * g_pEngineList = 0;

	KviRot13Engine::KviRot13Engine()
	: KviCryptEngine()
	{
		g_pEngineList->append(this);
	}

	KviRot13Engine::~KviRot13Engine()
	{
		g_pEngineList->removeRef(this);
	}

	bool KviRot13Engine::init(const char *,int,const char *,int)
	{
		return true;
	}

	KviCryptEngine::EncryptResult KviRot13Engine::encrypt(const char * plainText,KviStr &outBuffer)
	{
		outBuffer = plainText;
		unsigned char * aux = (unsigned char *)outBuffer.ptr();
		while(*aux)
		{
			if(isalpha(*aux))
			{
				char l = toupper(*aux);
				if (l >= 'N' && l <= 'Z')
					*aux = *aux - 13;
				else if (l >= 'A' && l <= 'M')
					*aux = *aux + 13;
			}
			aux++;
		}

		return KviCryptEngine::Encoded;
	}

	KviCryptEngine::DecryptResult KviRot13Engine::decrypt(const char * inBuffer,KviStr &plainText)
	{
		plainText = inBuffer;
		return KviCryptEngine::DecryptOkWasPlainText;
	}

	static KviCryptEngine * allocRot13Engine()
	{
		return new KviRot13Engine();
	}

	static void deallocRot13Engine(KviCryptEngine * e)
	{
		delete e;
	}

#endif


// =======================================
// module routines
// =======================================
static bool rot13_module_init(KviModule * m)
{
#ifdef COMPILE_CRYPT_SUPPORT
	g_pEngineList = new KviPointerList<KviCryptEngine>;
	g_pEngineList->setAutoDelete(false);

	KviCryptEngineDescription * d = new KviCryptEngineDescription;
	d->m_szName = "ROT13";
	d->m_szAuthor = "Aeriana";
	d->m_szDescription = __tr2qs("The simple Caesar-cypher encryption that replaces each letter with the one 13 places forward or back along the alphabet; it is used to enclose the text in a sealed wrapper that the reader must choose to open - e.g. for posting things that might offend some readers, or spoilers.");
	d->m_iFlags = KVI_CRYPTENGINE_CAN_ENCRYPT;
	d->m_allocFunc = allocRot13Engine;
	d->m_deallocFunc = deallocRot13Engine;
	m->registerCryptEngine(d);

	return true;
#else
	return false;
#endif
}

static bool rot13_module_cleanup(KviModule *m)
{
#ifdef COMPILE_CRYPT_SUPPORT
	while(g_pEngineList->first())
		delete g_pEngineList->first();
	delete g_pEngineList;
	g_pEngineList = 0;
	m->unregisterCryptEngines();
	return true;
#else
	return false;
#endif
}

static bool rot13_module_can_unload(KviModule *)
{
#ifdef COMPILE_CRYPT_SUPPORT
	return g_pEngineList->isEmpty();
#else
	return true;
#endif
}

// =======================================
// plugin definition structure
// =======================================
KVIRC_MODULE(
	"ROT13 crypt engine",
	"4.0.0",
	"Aeriana <aeriana at quasarnet dot org>" ,
	"Exports the ROT13 text transformation engine",
	rot13_module_init ,
	rot13_module_can_unload,
	0,
	rot13_module_cleanup,
	0
)

#ifdef COMPILE_CRYPT_SUPPORT
	#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
		#include "libkvirot13.moc"
	#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
#endif
