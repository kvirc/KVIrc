//=============================================================================
//
//   File : KviSSLMaster.cpp
//   Creation date : Mon Jun 17 2002 20:34:00 by Szymon Stefanek
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

#include "KviSSLMaster.h"

#ifdef COMPILE_SSL_SUPPORT

#include "KviWindow.h"
#include "KviOptions.h"
#include "kvi_out.h"
#include "KviControlCodes.h"
#include "KviLocale.h"
#include "KviKvsVariant.h"

#ifdef COMPILE_ON_WINDOWS
//
// Since OpenSSL 0.9.8 we need to link in a piece of code from the OpenSSL source.
// This is their own hack to avoid runtime problems when linking to an OpenSSL library
// that has been compiled with a different runtime (different options or compiler at all).
// This file is either installed in ${OPENSSL_INSTALL_ROOT}/include/openssl or
// in ${OPENSSL_INSTALL_ROOT}/ms/.
//
// Failing to include this file manifests itself with an abort when using a certificate.
// Just before the abort a message box complains about "no OPENSSL_Applink" which is
// implemented inside applink.c
//
#include <applink.c>
#endif //COMPILE_ON_WINDOWS

namespace KviSSLMaster
{

	KVIRC_API void printSSLCipherInfo(KviWindow * wnd, const char * description, KviSSLCipherInfo * c)
	{
		wnd->output(KVI_OUT_SSL, __tr2qs("[SSL]: %c%s"), KviControlCodes::Bold, description);
		wnd->output(KVI_OUT_SSL, __tr2qs("[SSL]:  Cipher: %c%s"), KviControlCodes::Bold, c->name());
		wnd->output(KVI_OUT_SSL, __tr2qs("[SSL]:  Version: %c%s"), KviControlCodes::Bold, c->version());
		wnd->output(KVI_OUT_SSL, __tr2qs("[SSL]:  Bits: %c%d (%d used)"), KviControlCodes::Bold, c->bits(), c->bitsUsed());
		//	wnd->output(KVI_OUT_SSL,__tr2qs("[SSL]:  Description: %c%s"),KviControlCodes::Bold,c->description());
	}

	KVIRC_API void printSSLCertificate(KviWindow * wnd, const char * description, KviSSLCertificate * c)
	{
		wnd->output(KVI_OUT_SSL, __tr2qs("[SSL]: %c%s"), KviControlCodes::Bold, description);
		wnd->output(KVI_OUT_SSL, __tr2qs("[SSL]:  Version: %c%d"), KviControlCodes::Bold, c->version());
		wnd->output(KVI_OUT_SSL, __tr2qs("[SSL]:  Serial number: %c%s"), KviControlCodes::Bold, c->serialNumber());
		wnd->output(KVI_OUT_SSL, __tr2qs("[SSL]:  Subject:"));
		wnd->output(KVI_OUT_SSL, __tr2qs("[SSL]:     Common name: %c%s"), KviControlCodes::Bold, c->subjectCommonName());
		wnd->output(KVI_OUT_SSL, __tr2qs("[SSL]:     Organization: %c%s"), KviControlCodes::Bold, c->subjectOrganization());
		wnd->output(KVI_OUT_SSL, __tr2qs("[SSL]:     Organizational unit: %c%s"), KviControlCodes::Bold, c->subjectOrganizationalUnit());
		wnd->output(KVI_OUT_SSL, __tr2qs("[SSL]:     Country: %c%s"), KviControlCodes::Bold, c->subjectCountry());
		wnd->output(KVI_OUT_SSL, __tr2qs("[SSL]:     State or province: %c%s"), KviControlCodes::Bold, c->subjectStateOrProvince());
		wnd->output(KVI_OUT_SSL, __tr2qs("[SSL]:     Locality: %c%s"), KviControlCodes::Bold, c->subjectLocality());
		wnd->output(KVI_OUT_SSL, __tr2qs("[SSL]:   Issuer:"));
		wnd->output(KVI_OUT_SSL, __tr2qs("[SSL]:     Common name: %c%s"), KviControlCodes::Bold, c->issuerCommonName());
		wnd->output(KVI_OUT_SSL, __tr2qs("[SSL]:     Organization: %c%s"), KviControlCodes::Bold, c->issuerOrganization());
		wnd->output(KVI_OUT_SSL, __tr2qs("[SSL]:     Organizational unit: %c%s"), KviControlCodes::Bold, c->issuerOrganizationalUnit());
		wnd->output(KVI_OUT_SSL, __tr2qs("[SSL]:     Country: %c%s"), KviControlCodes::Bold, c->issuerCountry());
		wnd->output(KVI_OUT_SSL, __tr2qs("[SSL]:     State or province: %c%s"), KviControlCodes::Bold, c->issuerStateOrProvince());
		wnd->output(KVI_OUT_SSL, __tr2qs("[SSL]:     Locality: %c%s"), KviControlCodes::Bold, c->issuerLocality());
		wnd->output(KVI_OUT_SSL, __tr2qs("[SSL]:   Public key: %c%s (%d bits)"), KviControlCodes::Bold, c->publicKeyType(), c->publicKeyBits());
		wnd->output(KVI_OUT_SSL, __tr2qs("[SSL]:   Signature type: %c%s"), KviControlCodes::Bold, c->signatureType());
		KviCString tmp = c->signatureContents();
		if(tmp.len() > 40)
		{
			tmp.cutRight(tmp.len() - 40);
			tmp.append("...");
		}
		wnd->output(KVI_OUT_SSL, __tr2qs("[SSL]:   Signature contents: %c%s"), KviControlCodes::Bold, tmp.ptr());
	}

	KVIRC_API void printSSLConnectionInfo(KviWindow * wnd, KviSSL * s)
	{
		KviSSLCertificate * sc = s->getPeerCertificate();
		if(sc)
		{
			QString tmp(__tr2qs("Peer X509 certificate"));
			KviSSLMaster::printSSLCertificate(wnd, tmp.toLocal8Bit().data(), sc);
			delete sc;
		}
		else
			wnd->outputNoFmt(KVI_OUT_SSL, __tr2qs("[SSL]: The peer didn't provide a certificate"));
		KviSSLCipherInfo * ci = s->getCurrentCipherInfo();
		if(ci)
		{
			QString tmp(__tr2qs("Current transmission cipher"));
			KviSSLMaster::printSSLCipherInfo(wnd, tmp.toLocal8Bit().data(), ci);
			delete ci;
		}
		else
			wnd->outputNoFmt(KVI_OUT_SSL, __tr2qs("[SSL]: Can't find out the current cipher info"));
	}

	KVIRC_API KviSSL * allocSSL(KviWindow * wnd, kvi_socket_t sock, KviSSL::Method m, const char * contextString)
	{
		KviSSL * s = new KviSSL();
		if(!s->initContext(m))
		{
			delete s;
			return nullptr;
		}

		if(!contextString)
			contextString = KviCString::emptyString().ptr();

		if(KVI_OPTION_BOOL(KviOption_boolUseSSLCertificate))
		{
			switch(s->useCertificateFile(
			    KVI_OPTION_STRING(KviOption_stringSSLCertificatePath),
			    KVI_OPTION_STRING(KviOption_stringSSLCertificatePass)))
			{
				case KviSSL::Success:
					if(wnd)
						wnd->output(KVI_OUT_SSL, __tr2qs("[%s]: [SSL]: Using certificate file %s"), contextString, KVI_OPTION_STRING(KviOption_stringSSLCertificatePath).toUtf8().data());
					break;
				case KviSSL::FileIoError:
					if(wnd)
						wnd->output(KVI_OUT_SSL, __tr2qs("[%s]: [SSL ERROR]: File I/O error while trying to use the certificate file %s"), contextString, KVI_OPTION_STRING(KviOption_stringSSLCertificatePath).toUtf8().data());
					break;
				default:
				{
					KviCString buffer;
					while(s->getLastErrorString(buffer))
					{
						if(wnd)
							wnd->output(KVI_OUT_SSL, __tr2qs("[%s]: [SSL ERROR]: %s"), contextString, buffer.ptr());
					}
				}
				break;
			}
		}
		if(KVI_OPTION_BOOL(KviOption_boolUseSSLPrivateKey))
		{
			switch(s->usePrivateKeyFile(
			    KVI_OPTION_STRING(KviOption_stringSSLPrivateKeyPath),
			    KVI_OPTION_STRING(KviOption_stringSSLPrivateKeyPass)))
			{
				case KviSSL::Success:
					if(wnd)
						wnd->output(KVI_OUT_SSL, __tr2qs("[%s]: [SSL]: Using private key file %s"), contextString, KVI_OPTION_STRING(KviOption_stringSSLPrivateKeyPath).toUtf8().data());
					break;
				case KviSSL::FileIoError:
					if(wnd)
						wnd->output(KVI_OUT_SSL, __tr2qs("[%s]: [SSL ERROR]: File I/O error while trying to use the private key file %s"), contextString, KVI_OPTION_STRING(KviOption_stringSSLPrivateKeyPath).toUtf8().data());
					break;
				default:
				{
					KviCString buffer;
					while(s->getLastErrorString(buffer))
					{
						if(wnd)
							wnd->output(KVI_OUT_SSL, __tr2qs("[%s]: [SSL ERROR]: %s"), contextString, buffer.ptr());
					}
				}
				break;
			}
		}

		if(!s->initSocket(sock))
		{
			delete s;
			return nullptr;
		}

		return s;
	}

	KVIRC_API void freeSSL(KviSSL * s)
	{
		delete s;
	}

/*
 * Used for $certificate() and $dcc.getSSLCertInfo() function
 */
	KVIRC_API bool getSSLCertInfo(KviSSLCertificate * pCert, QString szQuery, QString szOptionalParam, KviKvsVariant * pRetBuffer)
	{
		pRetBuffer->setString("");

		if(szQuery.compare("signatureType") == 0)
		{
			pRetBuffer->setString(pCert->signatureType());
			return true;
		}
		if(szQuery.compare("signatureContents") == 0)
		{
			pRetBuffer->setString(pCert->signatureContents());
			return true;
		}
		if(szQuery.compare("subjectCountry") == 0)
		{
			pRetBuffer->setString(pCert->subjectCountry());
			return true;
		}
		if(szQuery.compare("subjectStateOrProvince") == 0)
		{
			pRetBuffer->setString(pCert->subjectStateOrProvince());
			return true;
		}
		if(szQuery.compare("subjectLocality") == 0)
		{
			pRetBuffer->setString(pCert->subjectLocality());
			return true;
		}
		if(szQuery.compare("subjectOrganization") == 0)
		{
			pRetBuffer->setString(pCert->subjectOrganization());
			return true;
		}
		if(szQuery.compare("subjectOrganizationalUnit") == 0)
		{
			pRetBuffer->setString(pCert->subjectOrganizationalUnit());
			return true;
		}
		if(szQuery.compare("subjectCommonName") == 0)
		{
			pRetBuffer->setString(pCert->subjectCommonName());
			return true;
		}
		if(szQuery.compare("issuerCountry") == 0)
		{
			pRetBuffer->setString(pCert->issuerCountry());
			return true;
		}
		if(szQuery.compare("issuerStateOrProvince") == 0)
		{
			pRetBuffer->setString(pCert->issuerStateOrProvince());
			return true;
		}
		if(szQuery.compare("issuerLocality") == 0)
		{
			pRetBuffer->setString(pCert->issuerLocality());
			return true;
		}
		if(szQuery.compare("issuerOrganization") == 0)
		{
			pRetBuffer->setString(pCert->issuerOrganization());
			return true;
		}
		if(szQuery.compare("issuerOrganizationalUnit") == 0)
		{
			pRetBuffer->setString(pCert->issuerOrganizationalUnit());
			return true;
		}
		if(szQuery.compare("issuerCommonName") == 0)
		{
			pRetBuffer->setString(pCert->issuerCommonName());
			return true;
		}
		if(szQuery.compare("publicKeyBits") == 0)
		{
			pRetBuffer->setInteger(pCert->publicKeyBits());
			return true;
		}
		if(szQuery.compare("publicKeyType") == 0)
		{
			pRetBuffer->setString(pCert->publicKeyType());
			return true;
		}
		if(szQuery.compare("serialNumber") == 0)
		{
			pRetBuffer->setString(pCert->serialNumber());
			return true;
		}
		if(szQuery.compare("pemBase64") == 0)
		{
			char * szTmp = pCert->getX509Base64();
			QString szBase64(szTmp);
			pRetBuffer->setString(szBase64);
			KviMemory::free(szTmp);
			return true;
		}
		if(szQuery.compare("version") == 0)
		{
			pRetBuffer->setInteger(pCert->version());
			return true;
		}
		if(szQuery.compare("fingerprintIsValid") == 0)
		{
			pRetBuffer->setBoolean(pCert->fingerprintIsValid());
			return true;
		}
		if(szQuery.compare("fingerprintDigestId") == 0)
		{
			pRetBuffer->setInteger(pCert->fingerprintDigestId());
			return true;
		}
		if(szQuery.compare("fingerprintDigestStr") == 0)
		{
			pRetBuffer->setString(pCert->fingerprintDigestStr());
			return true;
		}
		if(szQuery.compare("fingerprintContents") == 0)
		{
			pRetBuffer->setString(pCert->fingerprintContents(szOptionalParam));
			return true;
		}

		// the warning is displayed in the caller
		return false;
	}
};
#endif
