//=============================================================================
//
//   File : libkvifish.cpp
//   Creation date : Sun Jan 30 2011 12:11:41 CEST by Fabio Bas
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2011 Fabio Bas (ctrlaltca at gmail dot com)
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

#if defined(COMPILE_SSL_SUPPORT)

#include "KviSSL.h"
#include <openssl/blowfish.h>
#include <openssl/bn.h>
#include <openssl/dh.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/sha.h>

DH * g_fish_dh = nullptr;

// ### new sophie-germain 1080bit prime number ###
// const char *g_fish_prime1080_bin="++ECLiPSE+is+proud+to+present+latest+FiSH+release+featuring+even+more+security+for+you+++shouts+go+out+to+TMG+for+helping+to+generate+this+cool+sophie+germain+prime+number++++/C32L";
const char * g_fish_prime1080_hex = "FBE1022E23D213E8ACFA9AE8B9DFADA3EA6B7AC7A7B7E95AB5EB2DF858921FEADE95E6AC7BE7DE6ADBAB8A783E7AF7A7FA6A2B7BEB1E72EAE2B72F9FA2BFB2A2EFBEFAC868BADB3E828FA8BADFADA3E4CC1BE7E8AFE85E9698A783EB68FA07A77AB6AD7BEB618ACF9CA2897EB28A6189EFA07AB99A8A7FA9AE299EFA7BA66DEAFEFBEFBF0B7D8B";
// const char *g_fish_prime1080_dec="12745216229761186769575009943944198619149164746831579719941140425076456621824834322853258804883232842877311723249782818608677050956745409379781245497526069657222703636504651898833151008222772087491045206203033063108075098874712912417029101508315117935752962862335062591404043092163187352352197487303798807791605274487594646923";
const char * g_fish_generator = "2";
#endif

#define FISH_KEYLEN 180
#define SHA256_LEN 32

static bool fish_DH1080_gen(unsigned char ** szPubKey, int * iLen)
{
#if defined(COMPILE_SSL_SUPPORT)
	if(!g_fish_dh)
	{
		BIGNUM * dhp = BN_new();
		if(!BN_hex2bn(&dhp, g_fish_prime1080_hex))
			return false;

		BIGNUM * dhg = BN_new();
		if(!BN_hex2bn(&dhg, g_fish_generator))
			return false;

		g_fish_dh = DH_new();
#if OPENSSL_VERSION_NUMBER >= 0x10100005L
		DH_set0_pqg(g_fish_dh, dhp, nullptr, dhg);
#else
		g_fish_dh->p = dhp;
		g_fish_dh->g = dhg;
#endif

		DH_generate_key(g_fish_dh);
	}

	const BIGNUM* pub_key;
#if OPENSSL_VERSION_NUMBER >= 0x10100005L
	DH_get0_key(g_fish_dh, &pub_key, nullptr);
#else
	pub_key = g_fish_dh->pub_key;
#endif

	*iLen = BN_num_bytes(pub_key);
	*szPubKey = (unsigned char *)KviMemory::allocate(*iLen);
	BN_bn2bin(pub_key, *szPubKey);

	return true;
#else
	qDebug("%s", __tr2qs("FiSH has been compiled without SSL support, unable to proceed").toUtf8().data());
	return false;
#endif
}

bool fish_event_onQueryNotice(KviKvsModuleEventCall * c)
{
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

	unsigned char * szMyPubKey = nullptr;
	QByteArray szHisPubKey, szTmp;
	int iMyPubKeyLen, *pMyPubKeyLen = &iMyPubKeyLen;
	if(!fish_DH1080_gen(&szMyPubKey, pMyPubKeyLen))
	{
#if !defined(COMPILE_SSL_SUPPORT)
		c->warning(__tr2qs("FiSH has been compiled without SSL support, unable to proceed"));
		return false;
#endif
		return false;
	}

	if(szMessage.startsWith("DH1080_INIT ", Qt::CaseSensitive) || szMessage.startsWith("DH1080_INIT_cbc ", Qt::CaseSensitive))
	{
		c->window()->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("FiSH: received DH1080 public key from %1, sending mine...").arg(szNick));
		// fish appends an 'A' to all base64 coded strings
		if(szMessage.startsWith("DH1080_INIT_cbc ", Qt::CaseSensitive))
			szHisPubKey = szMessage.mid(16).toLatin1();
		else
			szHisPubKey = szMessage.mid(12).toLatin1();
		szHisPubKey.truncate(FISH_KEYLEN);
		szHisPubKey = QByteArray::fromBase64(szHisPubKey);

		szTmp = QByteArray((char *)szMyPubKey, iMyPubKeyLen).toBase64();
		if(szMessage.startsWith("DH1080_INIT_cbc ", Qt::CaseSensitive))
		{
			c->window()->console()->connection()->sendFmtData("NOTICE %s :DH1080_FINISH_cbc %sA",
			    c->window()->console()->connection()->encodeText(szNick).data(),
			    szTmp.data());
		}
		else
		{
			// for compatibility with fish10
			if(szMessage.endsWith(" CBC", Qt::CaseSensitive))
				c->window()->console()->connection()->sendFmtData("NOTICE %s :DH1080_FINISH %sA CBC",
					c->window()->console()->connection()->encodeText(szNick).data(),
					szTmp.data());
			else
				c->window()->console()->connection()->sendFmtData("NOTICE %s :DH1080_FINISH %sA",
					c->window()->console()->connection()->encodeText(szNick).data(),
					szTmp.data());
		}
	}

	if(szMessage.startsWith("DH1080_FINISH ", Qt::CaseSensitive))
	{
		// fish appends an 'A' to all base64 coded strings
		szHisPubKey = szMessage.mid(14).toLatin1();
		szHisPubKey.truncate(FISH_KEYLEN);
		szHisPubKey = QByteArray::fromBase64(szHisPubKey);
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
		pWnd->setTarget(szNick, user, host);
	}
	if(!pWnd)
	{
		c->warning(__tr2qs("Query not found: %Q"), &szNick);
		return false;
	}

	KviCString szFinalKey;

#if defined(COMPILE_SSL_SUPPORT)
	unsigned char * secret = (unsigned char *)KviMemory::allocate(DH_size(g_fish_dh));
	int secretLen;
	BIGNUM * bn = BN_bin2bn((unsigned char *)szHisPubKey.data(), szHisPubKey.size(), nullptr);
	if(-1 == (secretLen = DH_compute_key(secret, bn, g_fish_dh)))
	{
		c->warning(__tr2qs("FiSH: error verifying peer public key (size=%1)").arg(szHisPubKey.size()));
		return false;
	}
	BN_zero(bn);
	BN_free(bn);

	unsigned char * hashedSecret = (unsigned char *)KviMemory::allocate(SHA256_LEN);
	SHA256(secret, secretLen, hashedSecret);

	KviMemory::free(secret);

	szFinalKey.bufferToBase64((char *)hashedSecret, SHA256_LEN);
	//strip the trailing =
	szFinalKey.stripRight('=');

	KviMemory::free(hashedSecret);
#else
	c->warning(__tr2qs("FiSH has been compiled without SSL support, unable to proceed"));
	return false;
#endif

	(void)g_pModuleManager->loadModulesByCaps("crypt");

	QString szEngine = "Mircryption";
	KviCryptEngine * e = g_pCryptEngineManager->allocateEngine(szEngine);
	if(e)
	{
		if(e->init(szFinalKey.ptr(), szFinalKey.len(), szFinalKey.ptr(), szFinalKey.len()))
		{
			KviCryptSessionInfo * inf = KviCryptController::allocateCryptSessionInfo();
			inf->m_pEngine = e;
			inf->m_szEngineName = szEngine;
			inf->m_bDoEncrypt = true;
			inf->m_bDoDecrypt = true;

			pWnd->setCryptSessionInfo(inf);
		}
		else
		{
			c->warning(__tr2qs("Failed to initialize the Mircryption engine: %Q"), &(e->lastError()));
			g_pCryptEngineManager->deallocateEngine(e);
		}
	}
	else
	{
		c->warning(__tr2qs("The Mircryption encryption engine doesn't exist"));
	}

	c->window()->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("FiSH: key for %1 successfully set!").arg(szNick));
	return true;
}

static bool fish_cmd_keyx(KviKvsModuleCommandCall * c)
{
	QString szTarget;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("target", KVS_PT_NONEMPTYSTRING, 0, szTarget)
	KVSM_PARAMETERS_END(c)

	unsigned char * szMyPubKey = nullptr;
	KviCString szHisPubKey, szTmp;
	int iMyPubKeyLen, *pMyPubKeyLen = &iMyPubKeyLen;
	if(!fish_DH1080_gen(&szMyPubKey, pMyPubKeyLen))
		return false;

	szTmp.bufferToBase64((char *)szMyPubKey, iMyPubKeyLen);
	c->window()->console()->connection()->sendFmtData("NOTICE %s :DH1080_INIT %sA",
	    c->window()->console()->connection()->encodeText(szTarget).data(),
	    szTmp.ptr());

	c->window()->output(KVI_OUT_SYSTEMMESSAGE, __tr2qs("FiSH: sent my DH1080 public key to %1, waiting for reply...").arg(szTarget));
	return true;
}
#endif //COMPILE_CRYPT_SUPPORT

//
// module routines
//

static bool fish_module_init(KviModule * m)
{
#ifdef COMPILE_CRYPT_SUPPORT
#if defined(COMPILE_SSL_SUPPORT)
	KviSSL::globalSSLInit();
#endif
	m->kvsRegisterAppEventHandler(KviEvent_OnQueryNotice, fish_event_onQueryNotice);

	KVSM_REGISTER_SIMPLE_COMMAND(m, "keyx", fish_cmd_keyx);

	return true;
#else
	return false;
#endif
}

static bool fish_module_cleanup(KviModule *)
{
	return true;
}

static bool fish_module_can_unload(KviModule *)
{
	return false;
}

//
// plugin definition structure
//

KVIRC_MODULE(
    "FISH crypt engine",
    "4.0.0",
    "Fabio Bas <ctrlaltca at gmail dot com>",
    "A FISH implementation",
    fish_module_init,
    fish_module_can_unload,
    0,
    fish_module_cleanup,
    0)
