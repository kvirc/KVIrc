#ifndef _KVI_SSLMASTER_H_
#define _KVI_SSLMASTER_H_
//=============================================================================
//
//   File : KviSSLMaster.h
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

#include "kvi_settings.h"

#ifdef COMPILE_SSL_SUPPORT

#include "KviSSL.h"
#include "kvi_sockettype.h"

class KviWindow;
class KviKvsVariant;

namespace KviSSLMaster
{
	extern KVIRC_API void printSSLCipherInfo(KviWindow * wnd, const char * description, KviSSLCipherInfo * c);
	extern KVIRC_API void printSSLCertificate(KviWindow * wnd, const char * description, KviSSLCertificate * c);

	extern KVIRC_API void printSSLConnectionInfo(KviWindow * wnd, KviSSL * s);

	extern KVIRC_API KviSSL * allocSSL(KviWindow * wnd, kvi_socket_t sock, KviSSL::Method m, const char * contextString = nullptr);
	extern KVIRC_API void freeSSL(KviSSL * s);

	extern KVIRC_API bool getSSLCertInfo(KviSSLCertificate * pCert, QString szQuery, QString szOptionalParam, KviKvsVariant * pRetBuffer);
}

#endif

#endif //_KVI_SSLMASTER_H_
