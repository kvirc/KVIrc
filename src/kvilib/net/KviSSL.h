#ifndef _KVI_SSL_H_
#define _KVI_SSL_H_
//=============================================================================
//
//   File : KviSSL.h
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

#include "kvi_settings.h"

#ifdef COMPILE_SSL_SUPPORT

#include "KviCString.h"
#include "KviPointerHashTable.h"
#include "kvi_sockettype.h"

// Apple deprecated openssl since osx 10.7:

#ifdef DEPRECATED_IN_MAC_OS_X_VERSION_10_7_AND_LATER
#undef DEPRECATED_IN_MAC_OS_X_VERSION_10_7_AND_LATER
#define DEPRECATED_IN_MAC_OS_X_VERSION_10_7_AND_LATER
#endif

#include <openssl/ssl.h>

class KVILIB_API KviSSLCertificate
{
public:
	KviSSLCertificate(X509 * x509);
	~KviSSLCertificate();

protected:
	X509 * m_pX509;
	KviPointerHashTable<const char *, KviCString> * m_pSubject;
	KviPointerHashTable<const char *, KviCString> * m_pIssuer;
	int m_iPubKeyBits;
	KviCString m_szPubKeyType;
	KviCString m_szSerialNumber;
	int m_iVersion;
	KviCString m_szSignatureType;
	KviCString m_szSignatureContents;

private:
	void extractSubject();
	void extractIssuer();
	void extractPubKeyInfo();
	void extractSerialNumber();
	void extractSignature();
	const char * dictEntry(KviPointerHashTable<const char *, KviCString> * dict, const char * entry);
	void splitX509String(KviPointerHashTable<const char *, KviCString> * dict, const char * t);
	int getFingerprint(unsigned char * bufferData, unsigned int * bufferLen, const char * digestName);

public:
	void setX509(X509 * x509);
	char * getX509Base64(); //not owned, you'll need to free this

	const char * signatureType() { return m_szSignatureType.ptr(); };
	const char * signatureContents() { return m_szSignatureContents.ptr(); };

	const char * subjectCountry() { return dictEntry(m_pSubject, "C"); };
	const char * subjectStateOrProvince() { return dictEntry(m_pSubject, "ST"); };
	const char * subjectLocality() { return dictEntry(m_pSubject, "L"); };
	const char * subjectOrganization() { return dictEntry(m_pSubject, "O"); };
	const char * subjectOrganizationalUnit() { return dictEntry(m_pSubject, "OU"); };
	const char * subjectCommonName() { return dictEntry(m_pSubject, "CN"); };

	const char * issuerCountry() { return dictEntry(m_pIssuer, "C"); };
	const char * issuerStateOrProvince() { return dictEntry(m_pIssuer, "ST"); };
	const char * issuerLocality() { return dictEntry(m_pIssuer, "L"); };
	const char * issuerOrganization() { return dictEntry(m_pIssuer, "O"); };
	const char * issuerOrganizationalUnit() { return dictEntry(m_pIssuer, "OU"); };
	const char * issuerCommonName() { return dictEntry(m_pIssuer, "CN"); };

	int publicKeyBits() { return m_iPubKeyBits; };
	const char * publicKeyType() { return m_szPubKeyType.ptr(); };

	const char * serialNumber() { return m_szSerialNumber.len() ? m_szSerialNumber.ptr() : nullptr; };

	int version() { return m_iVersion; };

	bool fingerprintIsValid();
	int fingerprintDigestId();
	const char * fingerprintDigestStr();
	const char * fingerprintContents(QString digestName = "");

#ifdef COMPILE_ON_WINDOWS
	// On windows we need to override new and delete operators
	// to ensure that always the right new/delete pair is called for an object instance
	// This bug is present in all the classes exported by a module that
	// can be instantiated/destroyed from external modules.
	// (this is a well known bug described in Q122675 of MSDN)
	void * operator new(size_t tSize);
	void operator delete(void * p);
#endif
};

class KVILIB_API KviSSLCipherInfo
{
public:
#if OPENSSL_VERSION_NUMBER >= 0x10000000L
	KviSSLCipherInfo(const SSL_CIPHER * c, const SSL * s);
#else
	KviSSLCipherInfo(SSL_CIPHER * c, SSL * s);
#endif
	~KviSSLCipherInfo();

protected:
	KviCString m_szVersion;
	int m_iNumBits;
	int m_iNumBitsUsed;
	KviCString m_szName;
	KviCString m_szDescription;

public:
	const char * name() { return m_szName.ptr(); };
	const char * description() { return m_szDescription.ptr(); };
	int bits() { return m_iNumBits; };
	int bitsUsed() { return m_iNumBitsUsed; };
	const char * version() { return m_szVersion.ptr(); };
#ifdef COMPILE_ON_WINDOWS
	// On windows we need to override new and delete operators
	// to ensure that always the right new/delete pair is called for an object instance
	// This bug is present in all the classes exported by a module that
	// can be instantiated/destroyed from external modules.
	// (this is a well known bug described in Q122675 of MSDN)
	void * operator new(size_t tSize);
	void operator delete(void * p);
#endif
};

#ifdef Success
#undef Success
#endif

class KVILIB_API KviSSL
{
public:
	enum Method
	{
		Client,
		Server
	};
	enum Result
	{
		Success,
		NotInitialized,
		WantRead,
		WantWrite,
		ZeroReturn,
		FileIoError,
		UnknownError,
		ObscureError,
		SSLError,
		SyscallError,
		RemoteEndClosedConnection
	};

public:
	KviSSL();
	~KviSSL();

public:
	SSL * m_pSSL;
	SSL_CTX * m_pSSLCtx;
	KviCString m_szPass;

public:
	static void globalInit();
	static void globalDestroy();
	static void globalSSLInit();
	static void globalSSLDestroy();

public:
	bool initSocket(kvi_socket_t fd);
	bool initContext(KviSSL::Method m);
	void shutdown();
	KviSSL::Result connect();
	KviSSL::Result accept();
	int read(char * buffer, int len);
	int write(const char * buffer, int len);
	// SSL ERRORS
	unsigned long getLastError(bool bPeek = false);
	bool getLastErrorString(KviCString & buffer, bool bPeek = false);
	// Protocol error
	KviSSL::Result getProtocolError(int ret);
	KviSSLCertificate * getPeerCertificate();
	KviSSLCertificate * getLocalCertificate();
	KviSSLCipherInfo * getCurrentCipherInfo();
	KviSSL::Result useCertificateFile(QString cert, QString pass);
	KviSSL::Result usePrivateKeyFile(QString key, QString pass);
#ifdef COMPILE_ON_WINDOWS
	// On windows we need to override new and delete operators
	// to ensure that always the right new/delete pair is called for an object instance
	// This bug is present in all the classes exported by a module that
	// can be instantiated/destroyed from external modules.
	// (this is a well known bug described in Q122675 of MSDN)
	void * operator new(size_t tSize);
	void operator delete(void * p);
#endif
private:
	KviSSL::Result connectOrAcceptError(int ret);
};

#endif //COMPILE_SSL_SUPPORT

#endif //_KVI_SSL_H_
