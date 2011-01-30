//=============================================================================
//
//   File : libkvifish.cpp
//   Creation date : Sun Jan 30 2011 12:11:41 CEST by Fabio Bas
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2011 Fabio Bas (ctrlaltca at gmail dot com)
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

#include "libkvifish.h"

#include "KviModule.h"
#include "KviModuleManager.h"
#include "KviKvsEventTable.h"
#include "KviLocale.h"
#include "KviConsoleWindow.h"
#include "KviQueryWindow.h"
#include "KviCString.h"
#include "kvi_out.h"

/*
	@doc: fish
	@type:
		module
	@short:
		A FISH implementation
	@title:
		The FISH module
	@body:
		A FISH implementation
*/

#ifdef COMPILE_CRYPT_SUPPORT
	#include "KviMemory.h"
	#include "KviPointerList.h"
	#include "KviCryptEngine.h"
	#include "KviCryptController.h"
	// KviApplication.cpp
	extern KVIRC_API KviCryptEngineManager * g_pCryptEngineManager;
	
	#ifdef COMPILE_SSL_SUPPORT

		#include <KviSSL.h>
		#include <openssl/blowfish.h>
		#include <openssl/bn.h>
		#include <openssl/dh.h>
		#include <openssl/err.h>
		#include <openssl/ssl.h>
		#include <openssl/sha.h>

		DH * g_fish_dh = 0;

	#endif

	#define FISH_KEYLEN 180
		
	// ### new sophie-germain 1080bit prime number ###
	// const char *g_fish_prime1080_bin="++ECLiPSE+is+proud+to+present+latest+FiSH+release+featuring+even+more+security+for+you+++shouts+go+out+to+TMG+for+helping+to+generate+this+cool+sophie+germain+prime+number++++/C32L";
	const char *g_fish_prime1080_hex="FBE1022E23D213E8ACFA9AE8B9DFADA3EA6B7AC7A7B7E95AB5EB2DF858921FEADE95E6AC7BE7DE6ADBAB8A783E7AF7A7FA6A2B7BEB1E72EAE2B72F9FA2BFB2A2EFBEFAC868BADB3E828FA8BADFADA3E4CC1BE7E8AFE85E9698A783EB68FA07A77AB6AD7BEB618ACF9CA2897EB28A6189EFA07AB99A8A7FA9AE299EFA7BA66DEAFEFBEFBF0B7D8B";
	// const char *g_fish_prime1080_dec="12745216229761186769575009943944198619149164746831579719941140425076456621824834322853258804883232842877311723249782818608677050956745409379781245497526069657222703636504651898833151008222772087491045206203033063108075098874712912417029101508315117935752962862335062591404043092163187352352197487303798807791605274487594646923";
	const char *g_fish_generator="2";

	static bool fish_DH1080_gen(KviCString * szPrivKey, KviCString * szPubKey)
	{
	#ifdef COMPILE_SSL_SUPPORT
		if(!g_fish_dh)
		{
			BIGNUM * dhp = BN_new();
			BN_init(dhp);
			if(!BN_hex2bn(&dhp,g_fish_prime1080_hex))
				return false;

			BIGNUM * dhg = BN_new();
			BN_init(dhg);
			if(!BN_hex2bn(&dhg,g_fish_generator))
				return false;

			g_fish_dh = DH_new();
			g_fish_dh->p = dhp;
			g_fish_dh->g = dhg;

			DH_generate_key(g_fish_dh);
		}

		int iSize=BN_num_bytes(g_fish_dh->pub_key);
		unsigned char * pPubKey = (unsigned char *) KviMemory::allocate(iSize);
		BN_bn2bin(g_fish_dh->pub_key, pPubKey);
		szPubKey->setStr((char *)pPubKey, iSize);

		iSize=BN_num_bytes(g_fish_dh->priv_key);
		unsigned char * pPrivKey = (unsigned char *) KviMemory::allocate(iSize);
		BN_bn2bin(g_fish_dh->priv_key, pPrivKey);
		szPrivKey->setStr((char *)pPrivKey, iSize);
		return true;
	#else
		c->warning(__tr2qs("FiSH has been compiled without ssl support, unable to proceed"));
		return false;
	#endif
	}

	bool fish_event_onQueryNotice(KviKvsModuleEventCall * c)
	{
		/*	EVENT("OnQueryNotice", \
			"$0 = source nick\n" \
			"$1 = source username\n" \
			"$2 = source host\n" \
			"$3 = message"),
			*/
		KviKvsVariantList * pParams = c->params();
		if(!pParams)
			return true;

		QString szNick, szMessage, szKey;

		if(!pParams->firstAsString(szNick))
			return true;
		
		pParams->next();
		pParams->next();

		if(!pParams->nextAsString(szMessage))
			return true;

		if(!szMessage.startsWith("DH1080_", Qt::CaseSensitive))
			return true;

		KviCString szMyPrivkey, szMyPubkey, szHisPubKey, szTmp;
		if(!fish_DH1080_gen(&szMyPrivkey, &szMyPubkey))
			return false;

		if(szMessage.startsWith("DH1080_INIT ", Qt::CaseSensitive))
		{
			c->window()->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("FiSH: Received DH1080 public key from %1, sending mine...").arg(szNick));
			// fish appends an 'A' to all base64 coded strings
			char * tmpBuf;
			int len;
			szHisPubKey.setStr(szMessage.mid(12).toAscii().data(), FISH_KEYLEN);
			len = szHisPubKey.base64ToBuffer(&tmpBuf,false);
			if(len > 0)
				szHisPubKey.setStr(tmpBuf, len);

			szTmp.bufferToBase64(szMyPubkey.ptr(), szMyPubkey.len());
			c->window()->console()->connection()->sendFmtData("NOTICE %s :DH1080_FINISH %sA",
				c->window()->console()->connection()->encodeText(szNick).data(),
				szTmp.ptr()
				);
		}
		
		if(szMessage.startsWith("DH1080_FINISH ", Qt::CaseSensitive))
		{
			// fish appends an 'A' to all base64 coded strings
			char * tmpBuf;
			int len;
			szHisPubKey.setStr(szMessage.mid(14).toAscii().data(), FISH_KEYLEN);
			len = szHisPubKey.base64ToBuffer(&tmpBuf,false);
			if(len > 0)
				szHisPubKey.setStr(tmpBuf, len);
		}

		if(!c->window()->connection())
			return false;
		
		KviQueryWindow * pWnd = c->window()->connection()->findQuery(szNick);
		if(!pWnd)
		{
			pWnd = c->window()->connection()->createQuery(szNick);
			QString user;
			QString host;
			KviIrcUserDataBase * db = c->window()->connection()->userDataBase();
			if(db)
			{
				KviIrcUserEntry * e = db->find(szNick);
				if(e)
				{
					user = e->user();
					host = e->host();
				}
			}
			pWnd->setTarget(szNick,user,host);
		}
		if(!pWnd)
		{
			c->warning(__tr2qs("Query not found: %Q"),&szNick);
			return false;
		}

		unsigned char * secret=(unsigned char *) KviMemory::allocate(DH_size(g_fish_dh));
		int secretLen;
		BIGNUM *bn = BN_bin2bn((unsigned char*) szHisPubKey.ptr(), szHisPubKey.len(),NULL);
		if(-1 == (secretLen = DH_compute_key(secret, bn, g_fish_dh)))
		{
			c->warning(__tr2qs("FiSH: error verificating peer public key (size=%1)").arg(szHisPubKey.len()));
			return false;
		}
		BN_zero(bn);
		BN_free(bn);

		/// SHA256 hash = 32 byte
		unsigned char * hashedSecret=(unsigned char *) KviMemory::allocate(32);
		#ifdef COMPILE_SSL_SUPPORT
			SHA256(secret, secretLen, hashedSecret);
		#else
			c->warning(__tr2qs("FiSH has been compiled without ssl support, unable to proceed"));
			return false;
		#endif

		KviMemory::free(secret);

		KviCString szFinalKey;
		szFinalKey.bufferToBase64((char *) hashedSecret, 32);
		//strip the trailing =
		szFinalKey.stripRight('=');
		
		KviMemory::free(hashedSecret);

		(void)g_pModuleManager->loadModulesByCaps("crypt");

		QString szEngine="Mircryption";
		KviCryptEngine * e = g_pCryptEngineManager->allocateEngine(szEngine);
		if(e)
		{
			int iFinalKeyLen=sizeof(szFinalKey);
			if(e->init(szFinalKey,iFinalKeyLen,szFinalKey,iFinalKeyLen))
			{
				KviCryptSessionInfo * inf = KviCryptController::allocateCryptSessionInfo();
				inf->m_pEngine = e;
				inf->m_szEngineName = szEngine;
				inf->m_bDoEncrypt = true;
				inf->m_bDoDecrypt = true;
			
				pWnd->setCryptSessionInfo(inf);
			} else {
				c->warning(__tr2qs("Failed to initialize the Mircryption engine: %Q"),&(e->lastError()));
				g_pCryptEngineManager->deallocateEngine(e);
			}
		} else {
			c->warning(__tr2qs("The Mircryption crypt engine does not exist"));
		}

		c->window()->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("FiSH: Key for %1 successfully set!").arg(szNick));
		return true;
	}

	static bool fish_cmd_keyx(KviKvsModuleCommandCall * c)
	{
		QString szTarget;
		KVSM_PARAMETERS_BEGIN(c)
			KVSM_PARAMETER("target",KVS_PT_NONEMPTYSTRING,0,szTarget)
		KVSM_PARAMETERS_END(c)

		KviCString szMyPrivkey, szMyPubkey, szTmp;
		if(!fish_DH1080_gen(&szMyPrivkey, &szMyPubkey))
			return false;

		szTmp.bufferToBase64(szMyPubkey.ptr(), szMyPubkey.len());
		c->window()->console()->connection()->sendFmtData("NOTICE %s :DH1080_INIT %sA",
			c->window()->console()->connection()->encodeText(szTarget).data(),
			szTmp.ptr()
			);

		c->window()->output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("FiSH: Sent my DH1080 public key to %1, waiting for reply ...").arg(szTarget));
		return true;
	}
#endif //COMPILE_CRYPT_SUPPORT
// =======================================
// module routines
// =======================================
static bool fish_module_init(KviModule * m)
{
#ifdef COMPILE_CRYPT_SUPPORT
	#ifdef COMPILE_SSL_SUPPORT
		KviSSL::globalSSLInit();
	#endif
	m->kvsRegisterAppEventHandler(KviEvent_OnQueryNotice,fish_event_onQueryNotice);
		
	KVSM_REGISTER_SIMPLE_COMMAND(m,"keyx",fish_cmd_keyx);

	return true;
#else 
	return false
#endif
}

static bool fish_module_cleanup(KviModule *m)
{
	return true;
}

static bool fish_module_can_unload(KviModule *)
{
	return false;
}

// =======================================
// plugin definition structure
// =======================================
KVIRC_MODULE(
	"FISH crypt engine",
	"4.0.0",
	"Fabio Bas <ctrlaltca at gmail dot com>",
	"A FISH implementation",
	fish_module_init,
	fish_module_can_unload,
	0,
	fish_module_cleanup,
	0
)

#ifdef COMPILE_CRYPT_SUPPORT
	#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
		#include "libkvifish.moc"
	#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
#endif
