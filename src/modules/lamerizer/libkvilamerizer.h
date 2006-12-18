#ifndef _LIBKVILAMERIZER_H_
#define _LIBKVILAMERIZER_H_
//
//   File : libkvilamerizer.h
//   Creation date : Sat Now 4 2000 15:41:41 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Till Bush (buti@geocities.com)
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

#include "kvi_settings.h"

#ifdef COMPILE_CRYPT_SUPPORT

	#include "kvi_crypt.h"

	class KviLamerizerEngine : public KviCryptEngine
	{
		Q_OBJECT
	public:
		KviLamerizerEngine(bool bLight);
		~KviLamerizerEngine();
	protected:
		bool m_bLight;
	public:
		virtual bool init(const char *encKey,int encKeyLen,const char *decKey,int decKeyLen);
		virtual KviCryptEngine::EncryptResult encrypt(const char * plainText,KviStr &outBuffer);
		virtual KviCryptEngine::DecryptResult decrypt(const char * inBuffer,KviStr &plainText);
	};

#endif // COMPILE_CRYPT_SUPPORT

#endif // _LIBKVILAMERIZER_H_
