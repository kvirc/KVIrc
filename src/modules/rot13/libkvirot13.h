#ifndef _LIBKVIROT13_H_
#define _LIBKVIROT13_H_
//=============================================================================
//
//   File : libkvirot13.h
//   Creation date : Sat Now 4 2000 15:41:41 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2009 Aeriana (aeriana at quasarnet dot org)
//   Copyright (C) 2000 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"

#ifdef COMPILE_CRYPT_SUPPORT

	#include "KviCryptEngine.h"

	class KviRot13Engine : public KviCryptEngine
	{
		Q_OBJECT
	public:
		KviRot13Engine();
		~KviRot13Engine();
	public:
		virtual bool init(const char *encKey,int encKeyLen,const char *decKey,int decKeyLen);
		virtual KviCryptEngine::EncryptResult encrypt(const char * plainText,KviCString &outBuffer);
		virtual KviCryptEngine::DecryptResult decrypt(const char * inBuffer,KviCString &plainText);
	};

#endif // COMPILE_CRYPT_SUPPORT

#endif // _LIBKVIROT13_H_
