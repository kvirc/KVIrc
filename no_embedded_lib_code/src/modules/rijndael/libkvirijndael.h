#ifndef _LIBKVIRIJNDAEL_H_
#define _LIBKVIRIJNDAEL_H_
//=============================================================================
//
//   File : libkvirijndael.h
//   Creation date : Sat Now 4 2000 15:33:12 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000 Till Bush (buti@geocities.com)
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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

	#include "kvi_crypt.h"
	#include "rijndael.h"

	class KviRijndaelEngine : public KviCryptEngine
	{
		Q_OBJECT
	public:
		KviRijndaelEngine();
		virtual ~KviRijndaelEngine();
	private:
		Rijndael * m_pEncryptCipher;
		Rijndael * m_pDecryptCipher;
	public:
		virtual bool init(const char *encKey,int encKeyLen,const char *decKey,int decKeyLen);
		virtual KviCryptEngine::EncryptResult encrypt(const char * plainText,KviStr &outBuffer);
		virtual KviCryptEngine::DecryptResult decrypt(const char * inBuffer,KviStr &plainText);
	protected:
		virtual bool binaryToAscii(const char *,int ,KviStr &){ return false; };
		virtual bool asciiToBinary(const char *,int *,char **){ return false; };
		virtual int getKeyLen(){ return 32; };
		virtual Rijndael::KeyLength getKeyLenId(){ return Rijndael::Key32Bytes; };
	private:
		void setLastErrorFromRijndaelErrorCode(int errCode);
	};

	class KviRijndaelHexEngine : public KviRijndaelEngine
	{
		Q_OBJECT
	public:
		KviRijndaelHexEngine() : KviRijndaelEngine() {};
		virtual ~KviRijndaelHexEngine(){};
	protected:
		virtual bool binaryToAscii(const char * inBuffer,int len,KviStr &outBuffer);
		virtual bool asciiToBinary(const char * inBuffer,int * len,char ** outBuffer);
	};

	class KviRijndael128HexEngine : public KviRijndaelHexEngine
	{
		Q_OBJECT
	public:
		KviRijndael128HexEngine() : KviRijndaelHexEngine() {};
		virtual ~KviRijndael128HexEngine(){};
	protected:
		virtual int getKenLen(){ return 16; };
		virtual Rijndael::KeyLength getKeyLenId(){ return Rijndael::Key16Bytes; };
	};

	class KviRijndael192HexEngine : public KviRijndaelHexEngine
	{
		Q_OBJECT
	public:
		KviRijndael192HexEngine() : KviRijndaelHexEngine() {};
		virtual ~KviRijndael192HexEngine(){};
	protected:
		virtual int getKenLen(){ return 24; };
		virtual Rijndael::KeyLength getKeyLenId(){ return Rijndael::Key24Bytes; };
	};

	class KviRijndael256HexEngine : public KviRijndaelHexEngine
	{
		Q_OBJECT
	public:
		KviRijndael256HexEngine() : KviRijndaelHexEngine() {};
		virtual ~KviRijndael256HexEngine(){};
	protected:
		virtual int getKenLen(){ return 32; };
	};

	class KviRijndaelBase64Engine : public KviRijndaelEngine
	{
		Q_OBJECT
	public:
		KviRijndaelBase64Engine() : KviRijndaelEngine() {};
		virtual ~KviRijndaelBase64Engine(){};
	protected:
		virtual bool binaryToAscii(const char * inBuffer,int len,KviStr &outBuffer);
		virtual bool asciiToBinary(const char * inBuffer,int * len,char ** outBuffer);
	};

	class KviRijndael128Base64Engine : public KviRijndaelBase64Engine
	{
		Q_OBJECT
	public:
		KviRijndael128Base64Engine() : KviRijndaelBase64Engine() {};
		virtual ~KviRijndael128Base64Engine(){};
	protected:
		virtual int getKenLen(){ return 16; };
		virtual Rijndael::KeyLength getKeyLenId(){ return Rijndael::Key16Bytes; };
	};

	class KviRijndael192Base64Engine : public KviRijndaelBase64Engine
	{
		Q_OBJECT
	public:
		KviRijndael192Base64Engine() : KviRijndaelBase64Engine() {};
		virtual ~KviRijndael192Base64Engine(){};
	protected:
		virtual int getKenLen(){ return 24; };
		virtual Rijndael::KeyLength getKeyLenId(){ return Rijndael::Key24Bytes; };
	};

	class KviRijndael256Base64Engine : public KviRijndaelBase64Engine
	{
		Q_OBJECT
	public:
		KviRijndael256Base64Engine() : KviRijndaelBase64Engine() {};
		virtual ~KviRijndael256Base64Engine(){};
	protected:
		virtual int getKenLen(){ return 32; };
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
		KviStr m_szEncryptKey;
		bool m_bEncryptCBC;
		KviStr m_szDecryptKey;
		bool m_bDecryptCBC;
	public:
		virtual bool isCryptographicEngine(){ return false; }; // we need to return false since it doesn't use the Qt::CTRL+P escape
		virtual bool init(const char *encKey,int encKeyLen,const char *decKey,int decKeyLen);
		virtual KviCryptEngine::EncryptResult encrypt(const char * plainText,KviStr &outBuffer);
		virtual KviCryptEngine::DecryptResult decrypt(const char * inBuffer,KviStr &plainText);
	protected:
		bool doDecryptECB(KviStr &encoded,KviStr &plain);
		bool doDecryptCBC(KviStr &encoded,KviStr &plain);
		bool doEncryptECB(KviStr &plain,KviStr &encoded);
		bool doEncryptCBC(KviStr &plain,KviStr &encoded);
	};

#endif // COMPILE_CRYPT_SUPPORT

#endif // _LIBKVIRIJNDAEL_H_
