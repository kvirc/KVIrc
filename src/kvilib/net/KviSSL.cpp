//=============================================================================
//
//   File : KviSSL.cpp
//   Creation date : Mon May 27 2002 21:36:12 CEST by Szymon Stefanek
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

#include "KviSSL.h"
#include "KviLocale.h"
#include "kvi_socket.h"

#ifdef COMPILE_SSL_SUPPORT

#include "KviThread.h"
#include "KviMemory.h"

#include <openssl/asn1.h>
#include <openssl/bn.h>
#include <openssl/err.h>
#include <openssl/dh.h>

#include <cstdio>

#if !(defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW))
// linux, mac
#include <csignal>
#endif

static bool g_bSSLInitialized = false;
static KviMutex * g_pSSLMutex = nullptr;

static inline void my_ssl_lock()
{
	g_pSSLMutex->lock();
}

static inline void my_ssl_unlock()
{
	g_pSSLMutex->unlock();
}

// THIS PART OF OpenSSL SUCKS

static DH * dh_512 = nullptr;
static DH * dh_1024 = nullptr;
static DH * dh_2048 = nullptr;
static DH * dh_4096 = nullptr;

static unsigned char dh512_p[] = {
	0x90, 0x86, 0xDD, 0x06, 0xE8, 0x0F, 0x10, 0x86, 0xF0, 0x91, 0xC5, 0x55,
	0x4D, 0x6B, 0xAF, 0x69, 0x4F, 0x01, 0xED, 0xF9, 0x57, 0x8F, 0x3B, 0xB8,
	0x9C, 0x87, 0xAE, 0x85, 0xC1, 0xBF, 0x57, 0xA5, 0xD5, 0xBA, 0x81, 0x24,
	0xE7, 0x99, 0xE3, 0xF6, 0xCD, 0xB4, 0x41, 0xB7, 0x7F, 0x6E, 0x7B, 0xB1,
	0xD2, 0xF3, 0xE9, 0x0F, 0xB9, 0x0E, 0x4D, 0xEB, 0x9D, 0xD4, 0xA9, 0xE5,
	0x03, 0x67, 0xA7, 0x27
};
static unsigned char dh512_g[] = { 0x05 };

static unsigned char dh1024_p[] = {
	0xA5, 0x4C, 0xB9, 0xB9, 0xC4, 0x35, 0x88, 0x68, 0x9B, 0x79, 0x48, 0x6C,
	0x21, 0xA7, 0x8E, 0xE2, 0x9C, 0xAF, 0x2F, 0x04, 0xBF, 0x45, 0xBC, 0xF5,
	0xAB, 0x35, 0x86, 0xC8, 0xBB, 0x9B, 0x75, 0x18, 0x7C, 0x9B, 0xAB, 0xE8,
	0x52, 0x7F, 0x57, 0x3E, 0xD8, 0x65, 0x7D, 0x2B, 0xE1, 0x6D, 0x3D, 0xA5,
	0x32, 0xE8, 0xA0, 0x2B, 0x7A, 0x58, 0x6B, 0x47, 0x16, 0x4E, 0xB1, 0xFC,
	0x09, 0xB7, 0x7C, 0xC6, 0xE9, 0x6E, 0xC7, 0xC7, 0xA1, 0x42, 0x0F, 0x4B,
	0x43, 0xFB, 0x58, 0xBA, 0xC7, 0x66, 0xD6, 0xCA, 0x6B, 0xC7, 0x45, 0x7C,
	0x99, 0xE4, 0x46, 0x02, 0x93, 0x3F, 0x28, 0xD2, 0xCE, 0x0C, 0x8A, 0xDD,
	0x6A, 0x22, 0x2E, 0xA9, 0x9A, 0xCA, 0x16, 0x48, 0x4E, 0x67, 0x4C, 0xE9,
	0xC8, 0x54, 0xCD, 0x18, 0xC9, 0xF3, 0x30, 0x3A, 0x74, 0xAB, 0xF9, 0xAF,
	0xE4, 0xA4, 0x0D, 0x56, 0x62, 0x28, 0x07, 0xBF
};
static unsigned char dh1024_g[] = { 0x05 };

static unsigned char dh2048_p[] = {
	0xBF, 0x67, 0x7B, 0x79, 0xA5, 0x22, 0xD3, 0xB5, 0x0C, 0x13, 0xE6, 0x92,
	0x54, 0xFD, 0x64, 0xBF, 0x57, 0x25, 0xBD, 0x02, 0x7C, 0xFD, 0x72, 0x97,
	0x82, 0xA4, 0xA6, 0x0A, 0xB9, 0xE6, 0x4B, 0xFA, 0xBD, 0xFA, 0x71, 0x8A,
	0x2E, 0x36, 0xF9, 0x03, 0x58, 0x1B, 0xB6, 0x3A, 0xFD, 0x15, 0xCC, 0x87,
	0x5D, 0x04, 0xF7, 0x45, 0xE0, 0xE2, 0x34, 0x7F, 0x54, 0x5F, 0x5D, 0x14,
	0xD3, 0xCA, 0x3E, 0xFD, 0x2A, 0x92, 0x10, 0x89, 0xA0, 0xB0, 0xB4, 0xE5,
	0x80, 0x05, 0x13, 0xBE, 0xA3, 0xD0, 0x42, 0x4B, 0x98, 0x44, 0x54, 0xB3,
	0xE0, 0x23, 0x26, 0xF5, 0x6B, 0x0E, 0x4D, 0x2A, 0x81, 0xB2, 0x8A, 0x06,
	0xC8, 0x00, 0x9E, 0xAB, 0x1B, 0x77, 0xDC, 0x87, 0x9C, 0x6C, 0xD5, 0xEE,
	0xB4, 0xB4, 0xDD, 0xDA, 0x3F, 0x40, 0xA3, 0xFA, 0xC1, 0x1E, 0xC0, 0xA2,
	0x9E, 0xB8, 0xAC, 0x31, 0xE8, 0x12, 0x93, 0x9C, 0x71, 0xF6, 0xE7, 0xF0,
	0x65, 0x7F, 0xA5, 0x20, 0xF7, 0x49, 0x3D, 0xD6, 0xF9, 0xD3, 0xF0, 0x3F,
	0xB3, 0xF0, 0xD0, 0x23, 0x22, 0x82, 0xA5, 0xDD, 0xFB, 0xD9, 0x9C, 0x7D,
	0xE7, 0xA0, 0x78, 0xE8, 0xF9, 0x02, 0x0C, 0x2F, 0x1D, 0x52, 0xC7, 0x61,
	0xED, 0xA0, 0xC9, 0x06, 0x14, 0xDF, 0xE7, 0xB1, 0x1E, 0x50, 0x98, 0x4F,
	0x10, 0xB9, 0x87, 0x4C, 0x1C, 0x9C, 0xB3, 0xD2, 0x98, 0x23, 0x7C, 0x47,
	0xD2, 0x3C, 0xC5, 0x29, 0x65, 0xC5, 0x67, 0x4E, 0xC0, 0x76, 0x0F, 0x43,
	0x27, 0x28, 0x89, 0x69, 0x30, 0x7D, 0x04, 0xFD, 0xF7, 0x89, 0xE5, 0xD6,
	0xE6, 0x97, 0x7D, 0xBB, 0x54, 0x5F, 0xB7, 0x94, 0x1D, 0xBC, 0x82, 0xAB,
	0x9A, 0xF5, 0x0A, 0x0C, 0x89, 0x68, 0xE7, 0x0A, 0x8C, 0x2D, 0x0D, 0x82,
	0x44, 0xA7, 0xB8, 0xF9, 0x0B, 0x8E, 0xCB, 0xA4, 0x6A, 0xA7, 0xEC, 0x5F,
	0x0A, 0xF8, 0x5F, 0xE7
};
static unsigned char dh2048_g[] = { 0x05 };

static unsigned char dh4096_p[] = {
	0xFA, 0x14, 0x72, 0x52, 0xC1, 0x4D, 0xE1, 0x5A, 0x49, 0xD4, 0xEF, 0x09,
	0x2D, 0xC0, 0xA8, 0xFD, 0x55, 0xAB, 0xD7, 0xD9, 0x37, 0x04, 0x28, 0x09,
	0xE2, 0xE9, 0x3E, 0x77, 0xE2, 0xA1, 0x7A, 0x18, 0xDD, 0x46, 0xA3, 0x43,
	0x37, 0x23, 0x90, 0x97, 0xF3, 0x0E, 0xC9, 0x03, 0x50, 0x7D, 0x65, 0xCF,
	0x78, 0x62, 0xA6, 0x3A, 0x62, 0x22, 0x83, 0xA1, 0x2F, 0xFE, 0x79, 0xBA,
	0x35, 0xFF, 0x59, 0xD8, 0x1D, 0x61, 0xDD, 0x1E, 0x21, 0x13, 0x17, 0xFE,
	0xCD, 0x38, 0x87, 0x9E, 0xF5, 0x4F, 0x79, 0x10, 0x61, 0x8D, 0xD4, 0x22,
	0xF3, 0x5A, 0xED, 0x5D, 0xEA, 0x21, 0xE9, 0x33, 0x6B, 0x48, 0x12, 0x0A,
	0x20, 0x77, 0xD4, 0x25, 0x60, 0x61, 0xDE, 0xF6, 0xB4, 0x4F, 0x1C, 0x63,
	0x40, 0x8B, 0x3A, 0x21, 0x93, 0x8B, 0x79, 0x53, 0x51, 0x2C, 0xCA, 0xB3,
	0x7B, 0x29, 0x56, 0xA8, 0xC7, 0xF8, 0xF4, 0x7B, 0x08, 0x5E, 0xA6, 0xDC,
	0xA2, 0x45, 0x12, 0x56, 0xDD, 0x41, 0x92, 0xF2, 0xDD, 0x5B, 0x8F, 0x23,
	0xF0, 0xF3, 0xEF, 0xE4, 0x3B, 0x0A, 0x44, 0xDD, 0xED, 0x96, 0x84, 0xF1,
	0xA8, 0x32, 0x46, 0xA3, 0xDB, 0x4A, 0xBE, 0x3D, 0x45, 0xBA, 0x4E, 0xF8,
	0x03, 0xE5, 0xDD, 0x6B, 0x59, 0x0D, 0x84, 0x1E, 0xCA, 0x16, 0x5A, 0x8C,
	0xC8, 0xDF, 0x7C, 0x54, 0x44, 0xC4, 0x27, 0xA7, 0x3B, 0x2A, 0x97, 0xCE,
	0xA3, 0x7D, 0x26, 0x9C, 0xAD, 0xF4, 0xC2, 0xAC, 0x37, 0x4B, 0xC3, 0xAD,
	0x68, 0x84, 0x7F, 0x99, 0xA6, 0x17, 0xEF, 0x6B, 0x46, 0x3A, 0x7A, 0x36,
	0x7A, 0x11, 0x43, 0x92, 0xAD, 0xE9, 0x9C, 0xFB, 0x44, 0x6C, 0x3D, 0x82,
	0x49, 0xCC, 0x5C, 0x6A, 0x52, 0x42, 0xF8, 0x42, 0xFB, 0x44, 0xF9, 0x39,
	0x73, 0xFB, 0x60, 0x79, 0x3B, 0xC2, 0x9E, 0x0B, 0xDC, 0xD4, 0xA6, 0x67,
	0xF7, 0x66, 0x3F, 0xFC, 0x42, 0x3B, 0x1B, 0xDB, 0x4F, 0x66, 0xDC, 0xA5,
	0x8F, 0x66, 0xF9, 0xEA, 0xC1, 0xED, 0x31, 0xFB, 0x48, 0xA1, 0x82, 0x7D,
	0xF8, 0xE0, 0xCC, 0xB1, 0xC7, 0x03, 0xE4, 0xF8, 0xB3, 0xFE, 0xB7, 0xA3,
	0x13, 0x73, 0xA6, 0x7B, 0xC1, 0x0E, 0x39, 0xC7, 0x94, 0x48, 0x26, 0x00,
	0x85, 0x79, 0xFC, 0x6F, 0x7A, 0xAF, 0xC5, 0x52, 0x35, 0x75, 0xD7, 0x75,
	0xA4, 0x40, 0xFA, 0x14, 0x74, 0x61, 0x16, 0xF2, 0xEB, 0x67, 0x11, 0x6F,
	0x04, 0x43, 0x3D, 0x11, 0x14, 0x4C, 0xA7, 0x94, 0x2A, 0x39, 0xA1, 0xC9,
	0x90, 0xCF, 0x83, 0xC6, 0xFF, 0x02, 0x8F, 0xA3, 0x2A, 0xAC, 0x26, 0xDF,
	0x0B, 0x8B, 0xBE, 0x64, 0x4A, 0xF1, 0xA1, 0xDC, 0xEE, 0xBA, 0xC8, 0x03,
	0x82, 0xF6, 0x62, 0x2C, 0x5D, 0xB6, 0xBB, 0x13, 0x19, 0x6E, 0x86, 0xC5,
	0x5B, 0x2B, 0x5E, 0x3A, 0xF3, 0xB3, 0x28, 0x6B, 0x70, 0x71, 0x3A, 0x8E,
	0xFF, 0x5C, 0x15, 0xE6, 0x02, 0xA4, 0xCE, 0xED, 0x59, 0x56, 0xCC, 0x15,
	0x51, 0x07, 0x79, 0x1A, 0x0F, 0x25, 0x26, 0x27, 0x30, 0xA9, 0x15, 0xB2,
	0xC8, 0xD4, 0x5C, 0xCC, 0x30, 0xE8, 0x1B, 0xD8, 0xD5, 0x0F, 0x19, 0xA8,
	0x80, 0xA4, 0xC7, 0x01, 0xAA, 0x8B, 0xBA, 0x53, 0xBB, 0x47, 0xC2, 0x1F,
	0x6B, 0x54, 0xB0, 0x17, 0x60, 0xED, 0x79, 0x21, 0x95, 0xB6, 0x05, 0x84,
	0x37, 0xC8, 0x03, 0xA4, 0xDD, 0xD1, 0x06, 0x69, 0x8F, 0x4C, 0x39, 0xE0,
	0xC8, 0x5D, 0x83, 0x1D, 0xBE, 0x6A, 0x9A, 0x99, 0xF3, 0x9F, 0x0B, 0x45,
	0x29, 0xD4, 0xCB, 0x29, 0x66, 0xEE, 0x1E, 0x7E, 0x3D, 0xD7, 0x13, 0x4E,
	0xDB, 0x90, 0x90, 0x58, 0xCB, 0x5E, 0x9B, 0xCD, 0x2E, 0x2B, 0x0F, 0xA9,
	0x4E, 0x78, 0xAC, 0x05, 0x11, 0x7F, 0xE3, 0x9E, 0x27, 0xD4, 0x99, 0xE1,
	0xB9, 0xBD, 0x78, 0xE1, 0x84, 0x41, 0xA0, 0xDF
};
static unsigned char dh4096_g[] = { 0x02 };

static DH * my_get_dh(int keylength)
{
	DH * dh = nullptr;
	unsigned char * p = nullptr;
	unsigned char * g = nullptr;
	int sp = 0;
	int sg = 0;
	BIGNUM *bp, *bg;
	switch(keylength)
	{
		case 512:
			dh = dh_512;
			p = dh512_p;
			g = dh512_g;
			sp = sizeof(dh512_p);
			sg = sizeof(dh512_g);
			break;
		case 1024:
			dh = dh_1024;
			p = dh1024_p;
			g = dh1024_g;
			sp = sizeof(dh1024_p);
			sg = sizeof(dh1024_g);
			break;
		case 2048:
			dh = dh_2048;
			p = dh2048_p;
			g = dh2048_g;
			sp = sizeof(dh2048_p);
			sg = sizeof(dh2048_g);
			break;
		case 4096:
			dh = dh_4096;
			p = dh4096_p;
			g = dh4096_g;
			sp = sizeof(dh4096_p);
			sg = sizeof(dh4096_g);
			break;
		default:
			// What the hell do you want from me ?
			qDebug("OpenSSL is asking for a DH param with keylen %d: no way :D", keylength);
			break;
	}

	if(dh)
		return dh;
	dh = DH_new();
	if(!dh)
		return nullptr;
	bp = BN_bin2bn(p, sp, nullptr);
	bg = BN_bin2bn(g, sg, nullptr);
	if((p == nullptr) || (g == nullptr))
	{
		BN_free(bp);
		BN_free(bg);
		DH_free(dh);
		return nullptr;
	}
#if OPENSSL_VERSION_NUMBER >= 0x10100005L
	DH_set0_pqg(dh, bp, nullptr, bg);
#else
	dh->p = bp;
	dh->g = bg;
#endif
	return dh;
}

DH * my_ugly_dh_callback(SSL *, int, int keylength)
{
	my_ssl_lock();
	DH * dh = my_get_dh(keylength);
	my_ssl_unlock();
	return dh;
}

void KviSSL::globalInit()
{
	if(g_pSSLMutex)
		return;
	g_pSSLMutex = new KviMutex();
}

void KviSSL::globalDestroy()
{
	if(!g_pSSLMutex)
		return;
	if(dh_512)
		DH_free(dh_512);
	if(dh_1024)
		DH_free(dh_1024);
	if(dh_2048)
		DH_free(dh_2048);
	if(dh_4096)
		DH_free(dh_4096);
	globalSSLDestroy();
	delete g_pSSLMutex;
	g_pSSLMutex = nullptr;
}

void KviSSL::globalSSLInit()
{
	my_ssl_lock();
	if(!g_bSSLInitialized)
	{
		// FIXME: this should be done only if SSL is really needed
		SSL_library_init();
		SSL_load_error_strings();
		//needed to support SHA2 in < OpenSSL 0.9.8o and 1.0.0a
		OpenSSL_add_all_algorithms();
		g_bSSLInitialized = true;
	}
	my_ssl_unlock();
}

void KviSSL::globalSSLDestroy()
{
	my_ssl_lock();
	if(g_bSSLInitialized)
	{
		EVP_cleanup();
		g_bSSLInitialized = false;
	}
	my_ssl_unlock();
}

KviSSL::KviSSL()
{
	globalSSLInit();
	m_pSSL = nullptr;
	m_pSSLCtx = nullptr;
}

KviSSL::~KviSSL()
{
	shutdown();
}

#ifdef COMPILE_ON_WINDOWS

// On windows we need to override new and delete operators
// to ensure that always the right new/delete pair is called for an object instance
// This bug is present in all the classes exported by a module that
// can be instantiated/destroyed from external modules.
// (this is a well known bug described in Q122675 of MSDN)

void * KviSSL::operator new(size_t tSize)
{
	return KviMemory::allocate(tSize);
}

void KviSSL::operator delete(void * p)
{
	KviMemory::free(p);
}

#endif

void KviSSL::shutdown()
{
	if(m_pSSL)
	{
		//avoid to die on a SIGPIPE if the connection has close (SSL_shutdown can call send())
		//see bug #440

#if !(defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW))
		// ignore SIGPIPE
		signal(SIGPIPE, SIG_IGN);
		// At least attempt to shutdown the connection gracefully
		if(!SSL_in_init(m_pSSL))
			SSL_shutdown(m_pSSL);
		//restore normal SIGPIPE behaviour.
		signal(SIGPIPE, SIG_DFL);
#else
		// At least attempt to shutdown the connection gracefully
		if(!SSL_in_init(m_pSSL))
			SSL_shutdown(m_pSSL);
#endif
		SSL_free(m_pSSL);
		m_pSSL = nullptr;
	}
	if(m_pSSLCtx)
	{
		SSL_CTX_free(m_pSSLCtx);
		m_pSSLCtx = nullptr;
	}
}

/**
 * Used only during passive (server) mode
 * Check the client certificate, always valid, even if it not exists
 */
int verify_clientCallback(int, X509_STORE_CTX *)
{
	//int preverify_ok, X509_STORE_CTX *x509_ctx
	// From man page: If verify_callback returns 1, the verification process is continued. If verify_callback always returns 1,
	// the TLS/SSL handshake will not be terminated with respect to verification failures and the connection will be established.
	return 1;
}

bool KviSSL::initContext(Method m)
{
	if(m_pSSL)
		return false;
	m_pSSLCtx = SSL_CTX_new(m == Client ? SSLv23_client_method() : SSLv23_server_method());
	if(!m_pSSLCtx)
		return false;

	if(m == Server)
	{
		// we have to request the peer certificate, else only the client can see the peer identity, not the server
		SSL_CTX_set_verify(m_pSSLCtx, SSL_VERIFY_PEER, verify_clientCallback);
	}

	// we want all ciphers to be available here, except insecure ones, orderer by strength;
	// ADH are moved to the end since they are less secure, but they don't need a certificate
	// (so we can use secure dcc without a cert)
	// NOTE: see bug ticket #155
	SSL_CTX_set_cipher_list(m_pSSLCtx, "ALL:!eNULL:!EXP:!SSLv2:+ADH@STRENGTH");
	SSL_CTX_set_tmp_dh_callback(m_pSSLCtx, my_ugly_dh_callback);
	return true;
}

bool KviSSL::initSocket(kvi_socket_t fd)
{
	if(!m_pSSLCtx)
		return false;
	m_pSSL = SSL_new(m_pSSLCtx);
	if(!m_pSSL)
		return false;
	if(!SSL_set_fd(m_pSSL, fd))
		return false;
	return true;
}

static int cb(char * buf, int size, int, void * u)
{
	KviCString * p = (KviCString *)u;
	int len = p->len();
	if(len >= size)
		return 0;
	KviMemory::move(buf, p->ptr(), len + 1);
	//qDebug("PASS REQYESTED: %s",p->ptr());
	return len;
}

KviSSL::Result KviSSL::useCertificateFile(QString cert, QString pass)
{
	if(!m_pSSLCtx)
		return NotInitialized;
	m_szPass = pass.toUtf8().data();
	if(m_szPass.len() < 4)
		m_szPass.append("xxxx");
	X509 * x509 = nullptr;

#ifdef COMPILE_ON_WINDOWS
	FILE * f = _wfopen(cert.toStdWString().data(), L"r");
#else
	FILE * f = fopen(cert.toUtf8().data(), "r");
#endif
	if(!f)
		return FileIoError;

	//qDebug("READING CERTIFICATE %s",cert.Utf8().data());
	if(PEM_read_X509(f, &x509, cb, &m_szPass))
	{
		if(!SSL_CTX_use_certificate(m_pSSLCtx, x509))
		{
			X509_free(x509);
			fclose(f);
			return SSLError;
		}
		X509_free(x509);
	}

	fclose(f);
	return Success;
}

KviSSL::Result KviSSL::usePrivateKeyFile(QString key, QString pass)
{
	if(!m_pSSLCtx)
		return NotInitialized;
	m_szPass = pass.toUtf8().data();
	if(m_szPass.len() < 4)
		m_szPass.append("xxxx");

	EVP_PKEY * k = nullptr;

#ifdef COMPILE_ON_WINDOWS
	FILE * f = _wfopen(key.toStdWString().data(), L"r");
#else
	FILE * f = fopen(key.toUtf8().data(), "r");
#endif
	if(!f)
		return FileIoError;

	//qDebug("READING KEY %s",key.toUtf8().data());
	if(PEM_read_PrivateKey(f, &k, cb, &m_szPass))
	{
		if(!SSL_CTX_use_PrivateKey(m_pSSLCtx, k))
		{
			EVP_PKEY_free(k);
			fclose(f);
			return SSLError;
		}
		EVP_PKEY_free(k);
	}

	fclose(f);
	return Success;
}

unsigned long KviSSL::getLastError(bool bPeek)
{
	return bPeek ? ERR_peek_error() : ERR_get_error();
}

bool KviSSL::getLastErrorString(KviCString & buffer, bool bPeek)
{
	unsigned long uErr = getLastError(bPeek);
	if(uErr != 0)
	{
		const char * err = ERR_reason_error_string(uErr);
		buffer = err ? err : "Unknown error";
		return true;
	}
	return false;
}

KviSSL::Result KviSSL::connect()
{
	if(!m_pSSL)
		return NotInitialized;
	int ret = SSL_connect(m_pSSL);
	return connectOrAcceptError(ret);
}

KviSSL::Result KviSSL::accept()
{
	if(!m_pSSL)
		return NotInitialized;
	int ret = SSL_accept(m_pSSL);
	return connectOrAcceptError(ret);
}

KviSSL::Result KviSSL::connectOrAcceptError(int ret)
{
	switch(SSL_get_error(m_pSSL, ret))
	{
		case SSL_ERROR_NONE:
			return Success;
			break;
		case SSL_ERROR_WANT_READ:
			return WantRead;
			break;
		case SSL_ERROR_WANT_WRITE:
			return WantWrite;
			break;
		case SSL_ERROR_ZERO_RETURN:
			return RemoteEndClosedConnection;
			break;
		case SSL_ERROR_WANT_X509_LOOKUP:
			return ObscureError;
			break;
		case SSL_ERROR_SYSCALL:
		{
			if(getLastError(true) != 0)
				return SSLError;
			if(ret == 0)
				return RemoteEndClosedConnection;
			return SyscallError;
		}
		break;
		case SSL_ERROR_SSL:
			return SSLError;
			break;
		default:
			return UnknownError;
			break;
	}
	return UnknownError;
}

int KviSSL::read(char * buffer, int len)
{
	int iReceived = SSL_read(m_pSSL, buffer, len);
	g_uIncomingTraffic += iReceived;
	return iReceived;
}
int KviSSL::write(const char * buffer, int len)
{
	g_uOutgoingTraffic += len;
	return SSL_write(m_pSSL, buffer, len);
}

KviSSL::Result KviSSL::getProtocolError(int ret)
{
	if(!m_pSSL)
		return NotInitialized;
	switch(SSL_get_error(m_pSSL, ret))
	{
		case SSL_ERROR_NONE:
			return Success;
			break;
		case SSL_ERROR_WANT_READ:
			return WantRead;
			break;
		case SSL_ERROR_WANT_WRITE:
			return WantWrite;
			break;
		case SSL_ERROR_ZERO_RETURN:
			return ZeroReturn;
			break;
		case SSL_ERROR_WANT_X509_LOOKUP:
			return ObscureError;
			break;
		case SSL_ERROR_SYSCALL:
			return SyscallError;
			break;
		case SSL_ERROR_SSL:
			return SSLError;
			break;
		default:
			return UnknownError;
			break;
	}
	return UnknownError;
}

KviSSLCertificate * KviSSL::getPeerCertificate()
{
	if(!m_pSSL)
		return nullptr;
	X509 * x509 = SSL_get_peer_certificate(m_pSSL);
	if(!x509)
		return nullptr;
	return new KviSSLCertificate(x509);
}

KviSSLCertificate * KviSSL::getLocalCertificate()
{
	if(!m_pSSL)
		return nullptr;
	X509 * x509 = SSL_get_certificate(m_pSSL);
	if(!x509)
		return nullptr;
	return new KviSSLCertificate(x509);
}

KviSSLCipherInfo * KviSSL::getCurrentCipherInfo()
{
	if(!m_pSSL)
		return nullptr;
#if OPENSSL_VERSION_NUMBER >= 0x10000000L
	const SSL_CIPHER * c = SSL_get_current_cipher(m_pSSL);
#else
	SSL_CIPHER * c = SSL_get_current_cipher(m_pSSL);
#endif
	if(!c)
		return nullptr;
	return new KviSSLCipherInfo(c, m_pSSL);
}

KviSSLCertificate::KviSSLCertificate(X509 * x509)
{
	m_pSubject = new KviPointerHashTable<const char *, KviCString>(17);
	m_pSubject->setAutoDelete(true);
	m_pIssuer = new KviPointerHashTable<const char *, KviCString>(17);
	m_pIssuer->setAutoDelete(true);
	m_pX509 = nullptr;
	setX509(x509);
}

KviSSLCertificate::~KviSSLCertificate()
{
	X509_free(m_pX509);
	delete m_pSubject;
	delete m_pIssuer;
}

char * KviSSLCertificate::getX509Base64()
{
	BUF_MEM * bptr;
	BIO * mem = BIO_new(BIO_s_mem());
	PEM_write_bio_X509(mem, m_pX509);
	int iLen = BIO_get_mem_data(mem, &bptr);
	char * szTmp = (char *)KviMemory::allocate(iLen + 1);
	KviMemory::copy(szTmp, bptr, iLen);
	*(szTmp + iLen) = '\0';
	BIO_free_all(mem);
	return szTmp;
}

#ifdef COMPILE_ON_WINDOWS

// On windows we need to override new and delete operators
// to ensure that always the right new/delete pair is called for an object instance
// This bug is present in all the classes exported by a module that
// can be instantiated/destroyed from external modules.
// (this is a well known bug described in Q122675 of MSDN)

void * KviSSLCertificate::operator new(size_t tSize)
{
	return KviMemory::allocate(tSize);
}

void KviSSLCertificate::operator delete(void * p)
{
	KviMemory::free(p);
}

#endif

void KviSSLCertificate::setX509(X509 * x509)
{
	if(m_pX509)
		X509_free(m_pX509);
	m_pX509 = x509;
	m_iVersion = X509_get_version(x509);
	extractSubject();
	extractIssuer();
	extractPubKeyInfo();
	extractSerialNumber();
	extractSignature();
}

bool KviSSLCertificate::fingerprintIsValid()
{
	if(!m_pX509)
		return false;

	EVP_PKEY * pkey = X509_get_pubkey(m_pX509);
	int rv = X509_verify(m_pX509, pkey);

	// careful: https://support.ntp.org/bugs/show_bug.cgi?id=1127
	// quote: X509_verify is a call to ASN1_item_verify which can return both 0 and -1 for error cases.
	// In particular it can return -1 when the message digest type is not known, or memory allocation failed.
	return (rv > 0);
}

int KviSSLCertificate::fingerprintDigestId()
{
	if(!m_pX509)
		return -1;

	const X509_ALGOR * alg;
#if OPENSSL_VERSION_NUMBER >= 0x10100005L
	X509_get0_signature(nullptr, &alg, m_pX509);
#else
	alg = m_pX509->sig_alg;
#endif

	int NID = OBJ_obj2nid(alg->algorithm);
	if(NID == NID_undef)
	{
		return 0; // unknown digest function: it means the signature can't be verified: the certificate can't be trusted
	}

	const EVP_MD * mdType = nullptr;
	mdType = EVP_get_digestbyname(OBJ_nid2sn(NID));

	if(mdType == nullptr)
	{
		return 0; // Unknown digest
	}

	return EVP_MD_type(mdType);
}

const char * KviSSLCertificate::fingerprintDigestStr()
{
	int iDigestType = fingerprintDigestId();

	if(iDigestType == 0)
		return "";

	return OBJ_nid2ln(iDigestType);
}

const char * KviSSLCertificate::fingerprintContents(QString digestName)
{
	unsigned char bufferData[EVP_MAX_MD_SIZE];
	unsigned int bufferLen = 0;
	const char * pDigestName;
	if(digestName.isEmpty())
	{
		pDigestName = OBJ_nid2sn(fingerprintDigestId()); // use the one used to create the signature
	}
	else
	{
		pDigestName = digestName.toUtf8().data();
	}

	if(getFingerprint(bufferData, &bufferLen, pDigestName) != 0)
		return "";

	QByteArray digestByteArray = QByteArray::fromRawData((char *)bufferData, bufferLen);
	return digestByteArray.toHex().data();
}

int KviSSLCertificate::getFingerprint(unsigned char * bufferData, unsigned int * bufferLen, const char * digestName)
{
	//TODO if in the future we will want to check the return value, ensure this
	// doesn't collide with the one from openssl
	if(!m_pX509)
		return -99;

	const EVP_MD * mdType = nullptr;
	mdType = EVP_get_digestbyname(digestName);

	if(mdType == nullptr)
	{
		return -98; // Unknown digest
	}

	if(!X509_digest(m_pX509, mdType, bufferData, bufferLen))
	{
		return -97;
	}

	return 0;
}

void KviSSLCertificate::extractSubject()
{
	char buffer[1024];
	char * t = X509_NAME_oneline(X509_get_subject_name(m_pX509), buffer, 1024);
	if(!t)
		return;
	m_pSubject->clear();
	splitX509String(m_pSubject, t);
}

void KviSSLCertificate::extractIssuer()
{
	char buffer[1024];
	char * t = X509_NAME_oneline(X509_get_issuer_name(m_pX509), buffer, 1024);
	if(!t)
		return;
	m_pIssuer->clear();
	splitX509String(m_pIssuer, t);
}

void KviSSLCertificate::splitX509String(KviPointerHashTable<const char *, KviCString> * dict, const char * t)
{
	KviCString buf = t;
	int cnt;
	KviCString ** arr = buf.splitToArray('/', 50, &cnt);
	if(arr)
	{
		if(cnt > 0)
		{
			for(int i = 0; i < cnt; i++)
			{
				int idx = arr[i]->findFirstIdx('=');
				if(idx != -1)
				{
					KviCString szTok = arr[i]->left(idx);
					arr[i]->cutLeft(idx + 1);
					if(szTok.hasData() && arr[i]->hasData())
					{
						dict->replace(szTok.ptr(), new KviCString(arr[i]->ptr()));
					}
				}
			}
		}

		KviCString::freeArray(arr);
	}
}

const char * KviSSLCertificate::dictEntry(KviPointerHashTable<const char *, KviCString> * dict, const char * entry)
{
	KviCString * t = dict->find(entry);
	if(!t)
		return __tr("Unknown");
	return t->ptr();
}

void KviSSLCertificate::extractPubKeyInfo()
{
	EVP_PKEY * p = X509_get_pubkey(m_pX509);
	if(p)
	{
		int type;
#if OPENSSL_VERSION_NUMBER >= 0x10100005L
		type = EVP_PKEY_base_id(p);
#else
		type = EVP_PKEY_type(p->type);
#endif
		m_iPubKeyBits = EVP_PKEY_bits(p);
		m_szPubKeyType = (type == NID_undef) ? __tr("Unknown") : OBJ_nid2ln(type);
		//		getPKeyType(p->type,m_szPubKeyType);
	}
	else
	{
		m_iPubKeyBits = 0;
		m_szPubKeyType = "None";
	}
}

void KviSSLCertificate::extractSerialNumber()
{
	m_szSerialNumber.clear();
	ASN1_INTEGER * i = X509_get_serialNumber(m_pX509);
	if(i)
	{
		BIGNUM * bn = ASN1_INTEGER_to_BN(i, nullptr);
		if(bn)
		{
			char * str = BN_bn2dec(bn);
			if(str)
			{
				m_szSerialNumber = KviCString(str);
				OPENSSL_free(str);
			}
			BN_free(bn);
		}
	}
}

void KviSSLCertificate::extractSignature()
{
	static char hexdigits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

	const ASN1_BIT_STRING * sig;
	const X509_ALGOR * alg;
#if OPENSSL_VERSION_NUMBER >= 0x10100005L
	X509_get0_signature(&sig, &alg, m_pX509);
#else
	sig = m_pX509->signature;
	alg = m_pX509->sig_alg;
#endif

	int i = OBJ_obj2nid(alg->algorithm);
	m_szSignatureType = (i == NID_undef) ? __tr("Unknown") : OBJ_nid2ln(i);

	m_szSignatureContents = "";

	for(i = 0; i < sig->length; i++)
	{
		if(m_szSignatureContents.hasData())
			m_szSignatureContents.append(":");
		m_szSignatureContents.append(hexdigits[(sig->data[i] & 0xf0) >> 4]);
		m_szSignatureContents.append(hexdigits[(sig->data[i] & 0x0f)]);
	}
}

#if OPENSSL_VERSION_NUMBER >= 0x10000000L
KviSSLCipherInfo::KviSSLCipherInfo(const SSL_CIPHER * c, const SSL * s)
#else
KviSSLCipherInfo::KviSSLCipherInfo(SSL_CIPHER * c, SSL * s)
#endif
{
	m_szVersion = SSL_get_version(s);
	m_iNumBitsUsed = SSL_CIPHER_get_bits(c, &m_iNumBits);
	m_szName = SSL_CIPHER_get_name(c);
	char buf[1024];
	m_szDescription = SSL_CIPHER_description(c, buf, 1024);
}

KviSSLCipherInfo::~KviSSLCipherInfo()
    = default;

#ifdef COMPILE_ON_WINDOWS

// On windows we need to override new and delete operators
// to ensure that always the right new/delete pair is called for an object instance
// This bug is present in all the classes exported by a module that
// can be instantiated/destroyed from external modules.
// (this is a well known bug described in Q122675 of MSDN)

void * KviSSLCipherInfo::operator new(size_t tSize)
{
	return KviMemory::allocate(tSize);
}

void KviSSLCipherInfo::operator delete(void * p)
{
	KviMemory::free(p);
}

#endif

#endif //COMPILE_SSL_SUPPORT
