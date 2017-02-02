#ifndef _LIBKVILAMERIZER_H_
#define _LIBKVILAMERIZER_H_
//=============================================================================
//
//   File : libkvilamerizer.h
//   Creation date : Sat Now 4 2000 15:41:41 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2000-2008 Till Bush (buti@geocities.com)
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

#ifdef COMPILE_CRYPT_SUPPORT

#include "KviCryptEngine.h"

class KviLamerizerEngine : public KviCryptEngine
{
	Q_OBJECT
public:
	KviLamerizerEngine(bool bLight);
	virtual ~KviLamerizerEngine();

protected:
	bool m_bLight;

public:
	bool init(const char * encKey, int encKeyLen, const char * decKey, int decKeyLen) override;
	KviCryptEngine::EncryptResult encrypt(const char * plainText, KviCString & outBuffer) override;
	KviCryptEngine::DecryptResult decrypt(const char * inBuffer, KviCString & plainText) override;
};

#endif // COMPILE_CRYPT_SUPPORT

#endif // _LIBKVILAMERIZER_H_
