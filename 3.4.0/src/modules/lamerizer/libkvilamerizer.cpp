//=============================================================================
//
//   File : libkvilamerizer.cpp
//   Creation date : Sat Jan 20 2002 17:06:12 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2005 Szymon Stefanek (pragma at kvirc dot net)
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

#include "libkvilamerizer.h"

#include "kvi_module.h"
#include "kvi_debug.h"
#include "kvi_locale.h"



/*
	@doc: lamerizer
	@type:
		module
	@short:
		The Lamerizer text transformation engine
	@title:
		The lamerizer module
	@body:
		This is an example of what you should NOT do with a text transformation
		engine. Do NOT use it! :D
*/



#ifdef COMPILE_CRYPT_SUPPORT
	
	#include "kvi_memmove.h"
	#include "kvi_malloc.h"

	#include "kvi_pointerlist.h"

	static KviPointerList<KviCryptEngine> * g_pEngineList = 0;

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

	bool KviLamerizerEngine::init(const char *,int,const char *,int)
	{
		return true;
	}

	static char subst_table[256] =
	{
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 007
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 015
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 023
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 031
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 039 : '
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 047 : /
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 055 : 7
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 063 : ?
		0   , '4' , '8' , 'C' , 'D' , '3' , 'F' , 'G' , // 071 : G
		'H' , '|' , 'J' , 'K' , 'L' , 'm' , 'N' , 'O' , // 079 : O
		'P' , 'q' , 'R' , '5' , '7' , 'U' , 'V' , 'W' , // 087 : W
		'x' , 'Y' , '2' , 0   , 0   , 0   , 0   , 0   , // 095 : _
		0   , '4' , 'b' , 'c' , 'd' , '3' , 'f' , '9' , // 103 : g
		'h' , '|' , 'j' , 'k' , '1' , 'm' , 'n' , '0' ,
		'p' , 'q' , 'r' , '5' , '7' , 'u' , 'v' , 'w' , // 119 : w
		'x' , 'y' , '2' , 0   , 0   , 0   , 0   , 0   , // 127 : 
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 135 : á
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 143 : è
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 151 : ó
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 159 : ü
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 167 : ß
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 175 : Ø
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 183 : ∑
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 191 : ø
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 199 : «
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 215 : ◊
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 223 : ﬂ
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 231 : Á
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 239 : Ô
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 247 : ˜
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0
	};

	static char subst_table_light[256] =
	{
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 007
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 015
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 023
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 031
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 039 : '
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 047 : /
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 055 : 7
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 063 : ?
		0   , '4' , 'B' , 'C' , 'D' , '3' , 'F' , 'G' , // 071 : G
		'H' , '|' , 'J' , 'K' , 'L' , 'm' , 'N' , 'O' , // 079 : O
		'P' , 'q' , 'R' , '5' , '7' , 'U' , 'V' , 'W' , // 087 : W
		'x' , 'Y' , 'Z' , 0   , 0   , 0   , 0   , 0   , // 095 : _
		0   , '4' , 'b' , 'c' , 'd' , '3' , 'f' , 'G' , // 103 : g
		'h' , '|' , 'j' , 'k' , '1' , 'm' , 'n' , '0' ,
		'p' , 'q' , 'r' , '5' , '7' , 'u' , 'v' , 'w' , // 119 : w
		'x' , 'y' , 'z' , 0   , 0   , 0   , 0   , 0   , // 127 : 
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 135 : á
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 143 : è
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 151 : ó
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 159 : ü
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 167 : ß
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 175 : Ø
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 183 : ∑
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 191 : ø
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 199 : «
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 215 : ◊
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 223 : ﬂ
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 231 : Á
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 239 : Ô
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , // 247 : ˜
		0   , 0   , 0   , 0   , 0   , 0   , 0   , 0
	};
	
	
	KviCryptEngine::EncryptResult KviLamerizerEngine::encrypt(const char * plainText,KviStr &outBuffer)
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
		} else {
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
			outBuffer.replaceAll("F","Ph");
			outBuffer.replaceAll("V","\\/");
		}

		return KviCryptEngine::Encoded;
	}

	KviCryptEngine::DecryptResult KviLamerizerEngine::decrypt(const char * inBuffer,KviStr &plainText)
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


// =======================================
// module routines
// =======================================
static bool lamerizer_module_init(KviModule * m)
{
#ifdef COMPILE_CRYPT_SUPPORT
	g_pEngineList = new KviPointerList<KviCryptEngine>;
	g_pEngineList->setAutoDelete(false);

	// FIXME: Maybe convert this repeated code to a function eh ?

	KviCryptEngineDescription * d = new KviCryptEngineDescription;
	d->szName = "Lamerizer";
	d->szAuthor = "Szymon Stefanek and Jan Wagner";
	d->szDescription = __tr2qs("A really lame text transformation engine :D");
	d->iFlags = KVI_CRYPTENGINE_CAN_ENCRYPT;
	d->allocFunc = allocLamerizerEngine;
	d->deallocFunc = deallocLamerizerEngine;
	m->registerCryptEngine(d);


	d = new KviCryptEngineDescription;
	d->szName = "LamerizerLight";
	d->szAuthor = "Szymon Stefanek and Jan Wagner";
	d->szDescription = __tr2qs("A really lame text transformation engine: Light Version.");
	d->iFlags = KVI_CRYPTENGINE_CAN_ENCRYPT;
	d->allocFunc = allocLightLamerizerEngine;
	d->deallocFunc = deallocLamerizerEngine;
	m->registerCryptEngine(d);

	return true;
#else
	return false;
#endif
}

static bool lamerizer_module_cleanup(KviModule *m)
{
#ifdef COMPILE_CRYPT_SUPPORT
	while(g_pEngineList->first())delete g_pEngineList->first();
	delete g_pEngineList;
	g_pEngineList = 0;
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

// =======================================
// plugin definition structure
// =======================================
KVIRC_MODULE(
	"Lamerizer crypt engine",
	"1.0.1",
	"Szymon Stefanek <pragma at kvirc dot net> \n Jan Wagner <istari@kvirc.net>" ,
	"Exports the lamerizer text transformation engine",
	lamerizer_module_init ,
	lamerizer_module_can_unload,
	0,
	lamerizer_module_cleanup
)

#ifdef COMPILE_CRYPT_SUPPORT
	#include "libkvilamerizer.moc"
#endif
