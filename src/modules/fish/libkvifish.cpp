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

#include <array>
#include <memory>

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
#include "KviPointerList.h"
#include "KviCryptEngine.h"
#include "KviCryptController.h"
// KviApplication.cpp
extern KVIRC_API KviCryptEngineManager * g_pCryptEngineManager;

#if defined(COMPILE_SSL_SUPPORT)

#include "KviSSL.h"
#include <openssl/bn.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
#include <openssl/core_names.h>
#include <openssl/param_build.h>
#else
#include <openssl/dh.h>
#endif

using FishKey = std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)>;
using FishKeyContext = std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)>;
using FishNumber = std::unique_ptr<BIGNUM, decltype(&BN_free)>;
static FishKey g_fishKey(nullptr, EVP_PKEY_free);

// ### new sophie-germain 1080bit prime number ###
// const char *g_fish_prime1080_bin="++ECLiPSE+is+proud+to+present+latest+FiSH+release+featuring+even+more+security+for+you+++shouts+go+out+to+TMG+for+helping+to+generate+this+cool+sophie+germain+prime+number++++/C32L";
const char * g_fish_prime1080_hex = "FBE1022E23D213E8ACFA9AE8B9DFADA3EA6B7AC7A7B7E95AB5EB2DF858921FEADE95E6AC7BE7DE6ADBAB8A783E7AF7A7FA6A2B7BEB1E72EAE2B72F9FA2BFB2A2EFBEFAC868BADB3E828FA8BADFADA3E4CC1BE7E8AFE85E9698A783EB68FA07A77AB6AD7BEB618ACF9CA2897EB28A6189EFA07AB99A8A7FA9AE299EFA7BA66DEAFEFBEFBF0B7D8B";
// const char *g_fish_prime1080_dec="12745216229761186769575009943944198619149164746831579719941140425076456621824834322853258804883232842877311723249782818608677050956745409379781245497526069657222703636504651898833151008222772087491045206203033063108075098874712912417029101508315117935752962862335062591404043092163187352352197487303798807791605274487594646923";
// The peer key and key generation must use the same DH1080 parameters.
static FishKey fish_DH1080_parameters(FishNumber publicKey)
{
	FishKey key(nullptr, EVP_PKEY_free);
	BIGNUM * rawPrime = nullptr;
	const int primeLength = BN_hex2bn(&rawPrime, g_fish_prime1080_hex);
	FishNumber prime(rawPrime, BN_free);
	FishNumber generator(BN_new(), BN_free);
	if(!primeLength || !generator || !BN_set_word(generator.get(), 2))
		return key;

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
	FishKeyContext context(EVP_PKEY_CTX_new_from_name(nullptr, "DH", nullptr), EVP_PKEY_CTX_free);
	std::unique_ptr<OSSL_PARAM_BLD, decltype(&OSSL_PARAM_BLD_free)> builder(OSSL_PARAM_BLD_new(), OSSL_PARAM_BLD_free);
	if(!context || !builder ||
	    !OSSL_PARAM_BLD_push_BN(builder.get(), OSSL_PKEY_PARAM_FFC_P, prime.get()) ||
	    !OSSL_PARAM_BLD_push_BN(builder.get(), OSSL_PKEY_PARAM_FFC_G, generator.get()) ||
	    (publicKey && !OSSL_PARAM_BLD_push_BN(builder.get(), OSSL_PKEY_PARAM_PUB_KEY, publicKey.get())))
		return key;

	std::unique_ptr<OSSL_PARAM, decltype(&OSSL_PARAM_free)> params(OSSL_PARAM_BLD_to_param(builder.get()), OSSL_PARAM_free);
	EVP_PKEY * rawKey = nullptr;
	const int selection = publicKey ? EVP_PKEY_PUBLIC_KEY : EVP_PKEY_KEY_PARAMETERS;
	if(params && EVP_PKEY_fromdata_init(context.get()) > 0 &&
	    EVP_PKEY_fromdata(context.get(), &rawKey, selection, params.get()) > 0)
		key.reset(rawKey);
#else
	std::unique_ptr<DH, decltype(&DH_free)> dh(DH_new(), DH_free);
	// The DH_set0_* functions take ownership of the BIGNUMs on success.
	// Release each unique_ptr only after success so DH_free() can free its BIGNUM.
	if(!dh || !DH_set0_pqg(dh.get(), prime.get(), nullptr, generator.get()))
		return key;
	prime.release();
	generator.release();
	if(publicKey)
	{
		if(!DH_set0_key(dh.get(), publicKey.get(), nullptr))
			return key;
		publicKey.release();
	}
	key.reset(EVP_PKEY_new());
	if(!key || !EVP_PKEY_set1_DH(key.get(), dh.get()))
		key.reset();
#endif
	return key;
}

static QByteArray fish_DH1080_secret(const QByteArray & peerBytes)
{
	if(peerBytes.isEmpty() || !g_fishKey)
		return {};
	FishNumber publicKey(BN_bin2bn(reinterpret_cast<const unsigned char *>(peerBytes.constData()), peerBytes.size(), nullptr), BN_free);
	if(!publicKey)
		return {};
	auto peer = fish_DH1080_parameters(std::move(publicKey));
	FishKeyContext context(EVP_PKEY_CTX_new(g_fishKey.get(), nullptr), EVP_PKEY_CTX_free);
	if(!peer || !context || EVP_PKEY_derive_init(context.get()) <= 0 || EVP_PKEY_derive_set_peer(context.get(), peer.get()) <= 0)
		return {};

	// FiSH hashes the unpadded shared secret.
	size_t length = 0;
	if(EVP_PKEY_derive(context.get(), nullptr, &length) <= 0)
		return {};
	QByteArray secret(static_cast<int>(length), Qt::Uninitialized);
	if(EVP_PKEY_derive(context.get(), reinterpret_cast<unsigned char *>(secret.data()), &length) <= 0)
		return {};
	secret.resize(static_cast<int>(length));
	return secret;
}
#endif

#define FISH_KEYLEN 180

static QByteArray fish_DH1080_gen()
{
#if defined(COMPILE_SSL_SUPPORT)
	if(!g_fishKey)
	{
		auto parameters = fish_DH1080_parameters(FishNumber(nullptr, BN_free));
		if(!parameters)
			return {};
		FishKeyContext context(EVP_PKEY_CTX_new(parameters.get(), nullptr), EVP_PKEY_CTX_free);
		EVP_PKEY * key = nullptr;
		if(!context || EVP_PKEY_keygen_init(context.get()) <= 0 || EVP_PKEY_keygen(context.get(), &key) <= 0)
			return {};
		g_fishKey.reset(key);
	}

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
	BIGNUM * number = nullptr;
	if(!EVP_PKEY_get_bn_param(g_fishKey.get(), OSSL_PKEY_PARAM_PUB_KEY, &number))
		return {};
	FishNumber publicKey(number, BN_free);
	const BIGNUM * pub = publicKey.get();
#else
	const BIGNUM * pub = nullptr;
	DH_get0_key(EVP_PKEY_get0_DH(g_fishKey.get()), &pub, nullptr);
#endif
	if(!pub)
		return {};
	QByteArray bytes(BN_num_bytes(pub), Qt::Uninitialized);
	BN_bn2bin(pub, reinterpret_cast<unsigned char *>(bytes.data()));
	return bytes;
#else
	qDebug("%s", __tr2qs("FiSH has been compiled without SSL support, unable to proceed").toUtf8().data());
	return {};
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

	const QByteArray szMyPubKey = fish_DH1080_gen();
	QByteArray szHisPubKey, szTmp;
	if(szMyPubKey.isEmpty())
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

		szTmp = szMyPubKey.toBase64();
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
	const QByteArray secret = fish_DH1080_secret(szHisPubKey);
	if(secret.isEmpty())
	{
		c->warning(__tr2qs("FiSH: error verifying peer public key (size=%1)").arg(szHisPubKey.size()));
		return false;
	}
	std::array<unsigned char, SHA256_DIGEST_LENGTH> hash;
	SHA256(reinterpret_cast<const unsigned char *>(secret.constData()), secret.size(), hash.data());
	szFinalKey.bufferToBase64(reinterpret_cast<const char *>(hash.data()), hash.size());
	szFinalKey.stripRight('=');
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

	const QByteArray szMyPubKey = fish_DH1080_gen();
	if(szMyPubKey.isEmpty())
		return false;

	const QByteArray szTmp = szMyPubKey.toBase64();
	c->window()->console()->connection()->sendFmtData("NOTICE %s :DH1080_INIT %sA",
	    c->window()->console()->connection()->encodeText(szTarget).data(),
	    szTmp.constData());

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
#if defined(COMPILE_CRYPT_SUPPORT) && defined(COMPILE_SSL_SUPPORT)
	g_fishKey.reset();
#endif
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
