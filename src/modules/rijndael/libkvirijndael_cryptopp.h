#ifndef _LIBKVIRIJNDAEL_H_
#define _LIBKVIRIJNDAEL_H_
//=============================================================================
//
//   File : libkvirijndael_cryptopp.h
//   Creation date : Fri, 17 Jul 2009 22:04:54 +0200 by Kai Wasserbäch
//      based on libkvirjndael.h as this is aimed as drop-in replacement.
//
//   This file is part of the KVIrc irc client distribution
//   Copyright ©        2009 Kai Wasserbäch <debian@carbon-project.org>
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

//base class
#include "KviCryptEngine.h"

// class definitions for the libcrypto++ case
#include <cryptopp/aes.h>
#include <cryptopp/blowfish.h>
#include <string>

// The following two defines are to indicate the in-/output encoding (hex
// or Base64).
#define KVI_RIJNDAEL_HEX 0
#define KVI_RIJNDAEL_B64 1

// AES/Rijndael
class KviRijndaelEngine : public KviCryptEngine
{
	Q_OBJECT
public:
	KviRijndaelEngine();
	virtual ~KviRijndaelEngine();
protected:
	virtual int getKeyLen(){ return CryptoPP::AES::DEFAULT_KEYLENGTH; };
	virtual int getEncoding(){ return KVI_RIJNDAEL_HEX; };
private:
	std::string m_szEncKey;
	std::string m_szDecKey;
public:
	virtual bool init(const char * pcEncKey, int iEncKeyLen, const char * pcDecKey, int iDecKeyLen);
	virtual KviCryptEngine::EncryptResult encrypt(const char * pcPlainText, KviCString & szOutBuffer);
	virtual KviCryptEngine::DecryptResult decrypt(const char * pcInBuffer, KviCString & szPlainText);
};

class KviRijndael128HexEngine : public KviRijndaelEngine
{
	Q_OBJECT
public:
	KviRijndael128HexEngine() : KviRijndaelEngine() {};
	virtual ~KviRijndael128HexEngine(){};
protected:
	virtual int getKeyLen(){ return CryptoPP::AES::MIN_KEYLENGTH; };
	virtual int getEncoding(){ return KVI_RIJNDAEL_HEX; };
};

class KviRijndael192HexEngine : public KviRijndaelEngine
{
	Q_OBJECT
public:
	KviRijndael192HexEngine() : KviRijndaelEngine() {};
	virtual ~KviRijndael192HexEngine(){};
protected:
	virtual int getKeyLen(){ return (CryptoPP::AES::MIN_KEYLENGTH + CryptoPP::AES::KEYLENGTH_MULTIPLE); };
	virtual int getEncoding() { return KVI_RIJNDAEL_HEX; };
};

class KviRijndael256HexEngine : public KviRijndaelEngine
{
	Q_OBJECT
public:
	KviRijndael256HexEngine() : KviRijndaelEngine() {};
	virtual ~KviRijndael256HexEngine(){};
protected:
	virtual int getKeyLen(){ return CryptoPP::AES::MAX_KEYLENGTH; };
	virtual int getEncoding() { return KVI_RIJNDAEL_HEX; };
};

class KviRijndael128Base64Engine : public KviRijndaelEngine
{
	Q_OBJECT
public:
	KviRijndael128Base64Engine() : KviRijndaelEngine() {};
	virtual ~KviRijndael128Base64Engine(){};
protected:
	virtual int getKeyLen(){ return CryptoPP::AES::MIN_KEYLENGTH; };
	virtual int getEncoding() { return KVI_RIJNDAEL_B64; };
};

class KviRijndael192Base64Engine : public KviRijndaelEngine
{
	Q_OBJECT
public:
	KviRijndael192Base64Engine() : KviRijndaelEngine() {};
	virtual ~KviRijndael192Base64Engine(){};
protected:
	virtual int getKeyLen(){ return (CryptoPP::AES::MIN_KEYLENGTH + CryptoPP::AES::KEYLENGTH_MULTIPLE); };
	virtual int getEncoding() { return KVI_RIJNDAEL_B64; };
};

class KviRijndael256Base64Engine : public KviRijndaelEngine
{
	Q_OBJECT
public:
	KviRijndael256Base64Engine() : KviRijndaelEngine() {};
	virtual ~KviRijndael256Base64Engine(){};
protected:
	virtual int getKeyLen(){ return CryptoPP::AES::MAX_KEYLENGTH; };
	virtual int getEncoding() { return KVI_RIJNDAEL_B64; };
};

// Blowfish (named »Mircryption« to be a real drop in replacement, hopefully)
class KviMircryptionEngine : public KviCryptEngine
{
	Q_OBJECT
public:
	KviMircryptionEngine();
	~KviMircryptionEngine();
	
	virtual bool init(const char * pcEncKey, int iEncKeyLen, const char * pcDecKey, int iDecKeyLen);
	virtual KviCryptEngine::EncryptResult encrypt(const char * pcPlainText, KviCString & szOutBuffer);
	virtual KviCryptEngine::DecryptResult decrypt(const char * pcInBuffer, KviCString & szPlainText);
private:
	std::string m_szEncKey;
	std::string m_szDecKey;
	bool m_bEncryptCBC;
	bool m_bDecryptCBC;
};

#endif // _LIBKVIRIJNDAEL_H_
