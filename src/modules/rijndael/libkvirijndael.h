#ifndef _LIBKVIRIJNDAEL_H_
#define _LIBKVIRIJNDAEL_H_
//=============================================================================
//
//   File : libkvirijndael.h
//   Creation date : Sat Now 4 2000 15:33:12 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000 Till Bush (buti@geocities.com)
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#include "KviCString.h"

#ifdef COMPILE_CRYPT_SUPPORT

#include "KviCryptEngine.h"
#include "Rijndael.h"

class KviRijndaelEngine : public KviCryptEngine
{
	Q_OBJECT
public:
	KviRijndaelEngine();
	~KviRijndaelEngine();

private:
	enum OperationalMode
	{
		OldCBC = 1, /** Legacy (pre-4.2) mode: CBC with empty IV **/
		CBC = 2,    /** CBC mode **/
		ECB = 3     /** ECB mode **/
	};
	Rijndael * m_pEncryptCipher;
	Rijndael * m_pDecryptCipher;
	OperationalMode m_bEncryptMode;
	OperationalMode m_bDecryptMode;

public:
	bool init(const char * encKey, int encKeyLen, const char * decKey, int decKeyLen) override;
	KviCryptEngine::EncryptResult encrypt(const char * plainText, KviCString & outBuffer) override;
	KviCryptEngine::DecryptResult decrypt(const char * inBuffer, KviCString & plainText) override;

protected:
	virtual bool binaryToAscii(const char *, int, KviCString &) { return false; }
	virtual bool asciiToBinary(const char *, int *, char **) { return false; }
	virtual int getKeyLen() const { return 32; }
	virtual Rijndael::KeyLength getKeyLenId() const { return Rijndael::Key32Bytes; }
private:
	void setLastErrorFromRijndaelErrorCode(int errCode);
};

class KviRijndaelHexEngine : public KviRijndaelEngine
{
	Q_OBJECT
public:
	KviRijndaelHexEngine() : KviRijndaelEngine(){}
	~KviRijndaelHexEngine() = default;

protected:
	bool binaryToAscii(const char * inBuffer, int len, KviCString & outBuffer) override;
	bool asciiToBinary(const char * inBuffer, int * len, char ** outBuffer) override;
};

class KviRijndael128HexEngine : public KviRijndaelHexEngine
{
	Q_OBJECT
public:
	KviRijndael128HexEngine() : KviRijndaelHexEngine(){}
	~KviRijndael128HexEngine() = default;

protected:
	int getKeyLen() const override { return 16; }
	Rijndael::KeyLength getKeyLenId() const override { return Rijndael::Key16Bytes; }
};

class KviRijndael192HexEngine : public KviRijndaelHexEngine
{
	Q_OBJECT
public:
	KviRijndael192HexEngine() : KviRijndaelHexEngine(){}
	~KviRijndael192HexEngine() = default;

protected:
	int getKeyLen() const override { return 24; }
	Rijndael::KeyLength getKeyLenId() const override { return Rijndael::Key24Bytes; }
};

class KviRijndael256HexEngine : public KviRijndaelHexEngine
{
	Q_OBJECT
public:
	KviRijndael256HexEngine() : KviRijndaelHexEngine(){}
	~KviRijndael256HexEngine() = default;

protected:
	int getKeyLen() const override { return 32; }
};

class KviRijndaelBase64Engine : public KviRijndaelEngine
{
	Q_OBJECT
public:
	KviRijndaelBase64Engine() : KviRijndaelEngine(){}
	~KviRijndaelBase64Engine() = default;

protected:
	bool binaryToAscii(const char * inBuffer, int len, KviCString & outBuffer) override;
	bool asciiToBinary(const char * inBuffer, int * len, char ** outBuffer) override;
};

class KviRijndael128Base64Engine : public KviRijndaelBase64Engine
{
	Q_OBJECT
public:
	KviRijndael128Base64Engine() : KviRijndaelBase64Engine(){}
	~KviRijndael128Base64Engine() = default;

protected:
	int getKeyLen() const override { return 16; }
	Rijndael::KeyLength getKeyLenId() const override { return Rijndael::Key16Bytes; }
};

class KviRijndael192Base64Engine : public KviRijndaelBase64Engine
{
	Q_OBJECT
public:
	KviRijndael192Base64Engine() : KviRijndaelBase64Engine(){}
	~KviRijndael192Base64Engine() = default;

protected:
	int getKeyLen() const override { return 24; }
	Rijndael::KeyLength getKeyLenId() const override { return Rijndael::Key24Bytes; }
};

class KviRijndael256Base64Engine : public KviRijndaelBase64Engine
{
	Q_OBJECT
public:
	KviRijndael256Base64Engine() : KviRijndaelBase64Engine(){}
	~KviRijndael256Base64Engine() = default;

protected:
	int getKeyLen() const override { return 32; }
};

// Mircyption stuff
#define MCPS2_STARTTAG "\xABm\xAB"
#define MCPS2_ENDTAG "\xBBm\xBB"

class KviMircryptionEngine : public KviCryptEngine
{
	Q_OBJECT
public:
	KviMircryptionEngine();
	~KviMircryptionEngine();

protected:
	KviCString m_szEncryptKey;
	bool m_bEncryptCBC;
	KviCString m_szDecryptKey;
	bool m_bDecryptCBC;

public:
	bool init(const char * encKey, int encKeyLen, const char * decKey, int decKeyLen) override;
	KviCryptEngine::EncryptResult encrypt(const char * plainText, KviCString & outBuffer) override;
	KviCryptEngine::DecryptResult decrypt(const char * inBuffer, KviCString & plainText) override;

protected:
	bool doDecryptECB(KviCString & encoded, KviCString & plain);
	bool doDecryptCBC(KviCString & encoded, KviCString & plain);
	bool doEncryptECB(KviCString & plain, KviCString & encoded);
	bool doEncryptCBC(KviCString & plain, KviCString & encoded);
};

#endif // COMPILE_CRYPT_SUPPORT

#endif // _LIBKVIRIJNDAEL_H_
