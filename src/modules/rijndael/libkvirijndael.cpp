//=============================================================================
//
//   File : libkvirijndael.cpp
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

#include "libkvirijndael.h"
#include "Rijndael.h"

#include "KviModule.h"
#include "kvi_debug.h"
#include "KviLocale.h"
#include "KviControlCodes.h"
#include "UglyBase64.h"
#include "InitVectorEngine.h"

//#warning "Other engines: mircStrip koi2win colorizer lamerizer etc.."

/*
	@doc: rijndael
	@type:
		module
	@short:
		The Rijndael cryptographic engines
	@title:
		The Rijndael module
	@body:
		The Rijndael module exports six [doc:crypt_engines]cryptographic engines[/doc] based
		on the Advanced Encryption Standard algorithm called Rijndael. Rijndael was
		originally written by Joan Daemen and Vincent Rijmen. The original Rijndael
		description is available at http://www.esat.kuleuven.ac.be/~rijmen/rijndael/.[br][br]
		It is a private key block cipher that has been designed to replace
		the widely used DES, and it should provide at least decent security against
		common attacks. Theoretically the best attack that one can perform on this cipher
		is the "brute force" attack that requires a really massive parallel computation:
		outside the capability of the common "hacker".[br][br]
		My implementation allows the usage of 128, 192 and 256 bit keys
		on 128 bit data blocks. The encrypted binary data buffer is then converted
		into an ASCII-string by using the base64 conversion or hex-digit-string representation.[br][br]
		The six engines are the six possible combinations of the key lengths and ASCII-string
		conversions.
*/

#if defined(COMPILE_CRYPT_SUPPORT) || defined(Q_MOC_RUN)
#include "KviMemory.h"
#include "KviPointerList.h"
#include "KviCryptEngineDescription.h"

static KviPointerList<KviCryptEngine> * g_pEngineList = nullptr;

KviRijndaelEngine::KviRijndaelEngine()
    : KviCryptEngine()
{
	g_pEngineList->append(this);
	m_pEncryptCipher = nullptr;
	m_pDecryptCipher = nullptr;
}

KviRijndaelEngine::~KviRijndaelEngine()
{
	g_pEngineList->removeRef(this);
	if(m_pEncryptCipher)
		delete m_pEncryptCipher;
	if(m_pDecryptCipher)
		delete m_pDecryptCipher;
}

bool KviRijndaelEngine::init(const char * encKey, int encKeyLen, const char * decKey, int decKeyLen)
{
	if(m_pEncryptCipher)
	{
		delete m_pEncryptCipher;
		m_pEncryptCipher = nullptr;
	}
	if(m_pDecryptCipher)
	{
		delete m_pDecryptCipher;
		m_pDecryptCipher = nullptr;
	}

	if(encKey && (encKeyLen > 0))
	{
		if(!(decKey && (decKeyLen > 0)))
		{
			decKey = encKey;
			decKeyLen = encKeyLen;
		} // else all
	}
	else
	{
		// no encrypt key specified...
		if(decKey && decKeyLen)
		{
			encKey = decKey;
			encKeyLen = decKeyLen;
		}
		else
		{
			// both keys missing
			setLastError(__tr2qs("Missing both encryption and decryption key: at least one is needed"));
			return false;
		}
	}

	KviCString szTmpEncryptKey = KviCString(encKey, encKeyLen);
	KviCString szTmpDecryptKey = KviCString(decKey, decKeyLen);

	m_bEncryptMode = CBC; // default mode
	m_bDecryptMode = CBC; // default mode

	if(kvi_strEqualCIN("ecb:", szTmpEncryptKey.ptr(), 4) && (szTmpEncryptKey.len() > 4))
	{
		szTmpEncryptKey.cutLeft(4);
		m_bEncryptMode = ECB;
	}
	else if(kvi_strEqualCIN("old:", szTmpEncryptKey.ptr(), 4) && (szTmpEncryptKey.len() > 4))
	{
		szTmpEncryptKey.cutLeft(4);
		m_bEncryptMode = OldCBC;
	}
	else if(kvi_strEqualCIN("cbc:", szTmpEncryptKey.ptr(), 4) && (szTmpEncryptKey.len() > 4))
	{
		szTmpEncryptKey.cutLeft(4);
	}

	if(kvi_strEqualCIN("ecb:", szTmpDecryptKey.ptr(), 4) && (szTmpDecryptKey.len() > 4))
	{
		szTmpDecryptKey.cutLeft(4);
		m_bDecryptMode = ECB;
	}
	else if(kvi_strEqualCIN("old:", szTmpDecryptKey.ptr(), 4) && (szTmpDecryptKey.len() > 4))
	{
		szTmpDecryptKey.cutLeft(4);
		m_bDecryptMode = OldCBC;
	}
	else if(kvi_strEqualCIN("cbc:", szTmpDecryptKey.ptr(), 4) && (szTmpDecryptKey.len() > 4))
	{
		szTmpDecryptKey.cutLeft(4);
	}

	int defLen = getKeyLen();

	szTmpEncryptKey.padRight(defLen);
	szTmpDecryptKey.padRight(defLen);

	m_pEncryptCipher = new Rijndael();

	int retVal = m_pEncryptCipher->init(
	    (m_bEncryptMode == ECB) ? Rijndael::ECB : Rijndael::CBC,
	    Rijndael::Encrypt,
	    (unsigned char *)szTmpEncryptKey.ptr(),
	    getKeyLenId());
	if(retVal != RIJNDAEL_SUCCESS)
	{
		delete m_pEncryptCipher;
		m_pEncryptCipher = nullptr;
		setLastErrorFromRijndaelErrorCode(retVal);
		return false;
	}

	m_pDecryptCipher = new Rijndael();
	retVal = m_pDecryptCipher->init(
	    (m_bEncryptMode == ECB) ? Rijndael::ECB : Rijndael::CBC,
	    Rijndael::Decrypt,
	    (unsigned char *)szTmpDecryptKey.ptr(),
	    getKeyLenId());
	if(retVal != RIJNDAEL_SUCCESS)
	{
		delete m_pEncryptCipher;
		m_pEncryptCipher = nullptr;
		delete m_pDecryptCipher;
		m_pDecryptCipher = nullptr;
		setLastErrorFromRijndaelErrorCode(retVal);
		return false;
	}

	return true;
}

void KviRijndaelEngine::setLastErrorFromRijndaelErrorCode(int errCode)
{
	switch(errCode)
	{
		case RIJNDAEL_SUCCESS:
			setLastError(__tr2qs("Error 0: success?"));
			break;
		case RIJNDAEL_UNSUPPORTED_MODE:
			setLastError(__tr2qs("Unsupported encryption mode"));
			break;
		case RIJNDAEL_UNSUPPORTED_DIRECTION:
			setLastError(__tr2qs("Unsupported direction"));
			break;
		case RIJNDAEL_UNSUPPORTED_KEY_LENGTH:
			setLastError(__tr2qs("Unsupported key length"));
			break;
		case RIJNDAEL_BAD_KEY:
			setLastError(__tr2qs("Bad key data"));
			break;
		case RIJNDAEL_NOT_INITIALIZED:
			setLastError(__tr2qs("Engine not initialized"));
			break;
		case RIJNDAEL_BAD_DIRECTION:
			setLastError(__tr2qs("Invalid direction for this engine"));
			break;
		case RIJNDAEL_CORRUPTED_DATA:
			setLastError(__tr2qs("Corrupted message data or invalid decryption key"));
			break;
		default:
			setLastError(__tr2qs("Unknown error"));
			break;
	}
}

KviCryptEngine::EncryptResult KviRijndaelEngine::encrypt(const char * plainText, KviCString & outBuffer)
{
	if(!m_pEncryptCipher)
	{
		setLastError(__tr2qs("Oops! Encryption cipher not initialized"));
		return KviCryptEngine::EncryptError;
	}
	int len = (int)kvi_strLen(plainText);
	char * buf = (char *)KviMemory::allocate(len + 16); // needed for the eventual padding
	unsigned char * iv = nullptr;
	if(m_bEncryptMode == CBC)
	{
		iv = (unsigned char *)KviMemory::allocate(MAX_IV_SIZE);
		InitVectorEngine::fillRandomIV(iv, MAX_IV_SIZE);
	}

	int retVal = m_pEncryptCipher->padEncrypt((const unsigned char *)plainText, len, (unsigned char *)buf, iv);
	if(retVal < 0)
	{
		if(m_bEncryptMode == CBC)
			KviMemory::free(iv);
		KviMemory::free(buf);
		setLastErrorFromRijndaelErrorCode(retVal);
		return KviCryptEngine::EncryptError;
	}

	if(m_bEncryptMode == CBC)
	{
		// prepend the iv to the cyphered text
		buf = (char *)KviMemory::reallocate(buf, retVal + MAX_IV_SIZE);
		KviMemory::move(buf + MAX_IV_SIZE, buf, retVal);
		KviMemory::move(buf, iv, MAX_IV_SIZE);
		KviMemory::free(iv);
		retVal += MAX_IV_SIZE;
	}

	if(!binaryToAscii(buf, retVal, outBuffer))
	{
		KviMemory::free(buf);
		return KviCryptEngine::EncryptError;
	}
	KviMemory::free(buf);

	if(outBuffer.len() > maxEncryptLen())
	{
		if(maxEncryptLen() > 0)
		{
			setLastError(__tr2qs("Data buffer too long"));
			return KviCryptEngine::EncryptError;
		}
	}
	outBuffer.prepend(KviControlCodes::CryptEscape);
	return KviCryptEngine::Encrypted;
}

KviCryptEngine::DecryptResult KviRijndaelEngine::decrypt(const char * inBuffer, KviCString & plainText)
{
	if(!m_pDecryptCipher)
	{
		setLastError(__tr2qs("Oops! Decryption cipher not initialized"));
		return KviCryptEngine::DecryptError;
	}

	if(*inBuffer != KviControlCodes::CryptEscape)
	{
		plainText = inBuffer;
		return KviCryptEngine::DecryptOkWasPlainText;
	}

	inBuffer++;

	if(!*inBuffer)
	{
		plainText = inBuffer;
		return KviCryptEngine::DecryptOkWasPlainText; // empty buffer
	}

	int len;
	char * binary;

	if(!asciiToBinary(inBuffer, &len, &binary))
		return KviCryptEngine::DecryptError;

	char * buf = (char *)KviMemory::allocate(len + 1);
	unsigned char * iv = nullptr;
	if(m_bEncryptMode == CBC)
	{
		// extract the IV from the cyphered string
		len -= MAX_IV_SIZE;
		iv = (unsigned char *)KviMemory::allocate(MAX_IV_SIZE);
		KviMemory::move(iv, binary, MAX_IV_SIZE);
		KviMemory::move(binary, binary + MAX_IV_SIZE, len);
		binary = (char *)KviMemory::reallocate(binary, len);
	}

	int retVal = m_pDecryptCipher->padDecrypt((const unsigned char *)binary, len, (unsigned char *)buf, iv);
	KviMemory::free(binary);
	KviMemory::free(iv);

	if(retVal < 0)
	{
		KviMemory::free(buf);
		setLastErrorFromRijndaelErrorCode(retVal);
		return KviCryptEngine::DecryptError;
	}

	buf[retVal] = '\0';

	plainText = buf;

	KviMemory::free(buf);
	return KviCryptEngine::DecryptOkWasEncrypted;
}

bool KviRijndaelHexEngine::binaryToAscii(const char * inBuffer, int len, KviCString & outBuffer)
{
	outBuffer.bufferToHex(inBuffer, len);
	return true;
}

bool KviRijndaelHexEngine::asciiToBinary(const char * inBuffer, int * len, char ** outBuffer)
{
	KviCString hex(inBuffer);
	char * tmpBuf;
	*len = hex.hexToBuffer(&tmpBuf, false);
	if(*len < 0)
	{
		setLastError(__tr2qs("The message is not a hexadecimal string: this is not my stuff"));
		return false;
	}
	else
	{
		if(*len > 0)
		{
			*outBuffer = (char *)KviMemory::allocate(*len);
			KviMemory::move(*outBuffer, tmpBuf, *len);
			KviCString::freeBuffer(tmpBuf);
		}
	}
	return true;
}

bool KviRijndaelBase64Engine::binaryToAscii(const char * inBuffer, int len, KviCString & outBuffer)
{
	outBuffer.bufferToBase64(inBuffer, len);
	return true;
}

bool KviRijndaelBase64Engine::asciiToBinary(const char * inBuffer, int * len, char ** outBuffer)
{
	KviCString base64(inBuffer);
	char * tmpBuf;
	*len = base64.base64ToBuffer(&tmpBuf, false);
	if(*len < 0)
	{
		setLastError(__tr2qs("The message is not a base64 string: this is not my stuff"));
		return false;
	}
	else
	{
		if(*len > 0)
		{
			*outBuffer = (char *)KviMemory::allocate(*len);
			KviMemory::move(*outBuffer, tmpBuf, *len);
			KviCString::freeBuffer(tmpBuf);
		}
	}
	return true;
}

static KviCryptEngine * allocRijndael128HexEngine()
{
	return new KviRijndael128HexEngine();
}

static KviCryptEngine * allocRijndael192HexEngine()
{
	return new KviRijndael192HexEngine();
}

static KviCryptEngine * allocRijndael256HexEngine()
{
	return new KviRijndael256HexEngine();
}

static KviCryptEngine * allocRijndael128Base64Engine()
{
	return new KviRijndael128Base64Engine();
}

static KviCryptEngine * allocRijndael192Base64Engine()
{
	return new KviRijndael192Base64Engine();
}

static KviCryptEngine * allocRijndael256Base64Engine()
{
	return new KviRijndael256Base64Engine();
}

static void deallocRijndaelCryptEngine(KviCryptEngine * e)
{
	delete e;
}

// Mircryption stuff

#include "BlowFish.h"

KviMircryptionEngine::KviMircryptionEngine()
    : KviCryptEngine()
{
	g_pEngineList->append(this);
}

KviMircryptionEngine::~KviMircryptionEngine()
{
	g_pEngineList->removeRef(this);
}

bool KviMircryptionEngine::init(const char * encKey, int encKeyLen, const char * decKey, int decKeyLen)
{
	if(encKey && (encKeyLen > 0))
	{
		if(!(decKey && (decKeyLen > 0)))
		{
			decKey = encKey;
			decKeyLen = encKeyLen;
		} // else all
	}
	else
	{
		// no encrypt key specified...
		if(decKey && decKeyLen)
		{
			encKey = decKey;
			encKeyLen = decKeyLen;
		}
		else
		{
			// both keys missing
			setLastError(__tr2qs("Missing both encryption and decryption key: at least one is needed"));
			return false;
		}
	}
	m_szEncryptKey = KviCString(encKey, encKeyLen);
	m_szDecryptKey = KviCString(decKey, decKeyLen);

	m_bEncryptCBC = true;
	m_bDecryptCBC = true;

	if((kvi_strEqualCIN("ecb:", m_szEncryptKey.ptr(), 4) || kvi_strEqualCIN("old:", m_szEncryptKey.ptr(), 4))
	    && (m_szEncryptKey.len() > 4))
	{
		m_bEncryptCBC = false;
		m_szEncryptKey.cutLeft(4);
	}
	else if(kvi_strEqualCIN("cbc:", m_szEncryptKey.ptr(), 4) && (m_szEncryptKey.len() > 4))
	{
		m_szEncryptKey.cutLeft(4);
	}

	if((kvi_strEqualCIN("ecb:", m_szDecryptKey.ptr(), 4) || kvi_strEqualCIN("old:", m_szDecryptKey.ptr(), 4))
	    && (m_szDecryptKey.len() > 4))
	{
		m_bDecryptCBC = false;
		m_szDecryptKey.cutLeft(4);
	}
	else if(kvi_strEqualCIN("cbc:", m_szDecryptKey.ptr(), 4) && (m_szDecryptKey.len() > 4))
	{
		m_szDecryptKey.cutLeft(4);
	}

	return true;
}

KviCryptEngine::EncryptResult KviMircryptionEngine::encrypt(const char * plainText, KviCString & outBuffer)
{
	KviCString szPlain = plainText;
	outBuffer = "";
	if(m_bEncryptCBC)
	{
		if(!doEncryptCBC(szPlain, outBuffer))
			return KviCryptEngine::EncryptError;
	}
	else
	{
		if(!doEncryptECB(szPlain, outBuffer))
			return KviCryptEngine::EncryptError;
	}
	outBuffer.prepend("+OK ");

	if(outBuffer.len() > maxEncryptLen())
	{
		if(maxEncryptLen() > 0)
		{
			setLastError(__tr2qs("Data buffer too long"));
			return KviCryptEngine::EncryptError;
		}
	}

	//outBuffer = MCPS2_STARTTAG;
	//outBuffer += MCPS2_ENDTAG;
	return KviCryptEngine::Encrypted;
}

KviCryptEngine::DecryptResult KviMircryptionEngine::decrypt(const char * inBuffer, KviCString & plainText)
{
	plainText = "";
	KviCString szIn = inBuffer;
	// various old versions
	if(kvi_strEqualCSN(inBuffer, "mcps ", 5))
		szIn.cutLeft(5);
	else if(kvi_strEqualCSN(inBuffer, "+OK ", 4))
		szIn.cutLeft(4);
	else if(kvi_strEqualCSN(inBuffer, "OK ", 3)) // some servers strip out the +
		szIn.cutLeft(3);
	else
	{
		plainText = szIn;
		return KviCryptEngine::DecryptOkWasPlainText;
	}

	if(m_bDecryptCBC)
		return doDecryptCBC(szIn, plainText) ? KviCryptEngine::DecryptOkWasEncrypted : KviCryptEngine::DecryptError;
	return doDecryptECB(szIn, plainText) ? KviCryptEngine::DecryptOkWasEncrypted : KviCryptEngine::DecryptError;
}

bool KviMircryptionEngine::doEncryptECB(KviCString & plain, KviCString & encoded)
{
	// make sure it is a multiple of 8 bytes (eventually pad with zeroes)
	if(plain.len() % 8)
	{
		int oldL = plain.len();
		plain.setLen(plain.len() + (8 - (plain.len() % 8)));
		char * padB = plain.ptr() + oldL;
		char * padE = plain.ptr() + plain.len();
		while(padB < padE)
			*padB++ = 0;
	}

	unsigned char * out = (unsigned char *)KviMemory::allocate(plain.len()); // we use this to avoid endiannes problems

	BlowFish bf((unsigned char *)m_szEncryptKey.ptr(), m_szEncryptKey.len());
	bf.ResetChain();
	bf.Encrypt((unsigned char *)plain.ptr(), out, plain.len(), BlowFish::ECB);

	UglyBase64::encode(out, plain.len(), encoded);
	KviMemory::free(out);
	return true;
}

bool KviMircryptionEngine::doDecryptECB(KviCString & encoded, KviCString & plain)
{
	unsigned char * buf = nullptr;
	int len;
	UglyBase64::decode(encoded, &buf, &len);

	plain.setLen(len);
	BlowFish bf((unsigned char *)m_szDecryptKey.ptr(), m_szDecryptKey.len());
	bf.ResetChain();
	bf.Decrypt(buf, (unsigned char *)plain.ptr(), len, BlowFish::ECB);

	KviMemory::free(buf);

	return true;
}

bool KviMircryptionEngine::doEncryptCBC(KviCString & plain, KviCString & encoded)
{
	// make sure it is a multiple of 8 bytes (eventually pad with zeroes)
	if(plain.len() % 8)
	{
		int oldL = plain.len();
		plain.setLen(plain.len() + (8 - (plain.len() % 8)));
		char * padB = plain.ptr() + oldL;
		char * padE = plain.ptr() + plain.len();
		while(padB < padE)
			*padB++ = 0;
	}

	int ll = plain.len() + 8;
	unsigned char * in = (unsigned char *)KviMemory::allocate(ll);

	InitVectorEngine::fillRandomIV(in, 8);

	KviMemory::copy(in + 8, plain.ptr(), plain.len());

	// encrypt
	unsigned char * out = (unsigned char *)KviMemory::allocate(ll);
	BlowFish bf((unsigned char *)m_szEncryptKey.ptr(), m_szEncryptKey.len());
	bf.ResetChain();
	bf.Encrypt(in, out, ll, BlowFish::CBC);
	KviMemory::free(in);

	encoded.bufferToBase64((const char *)out, ll);
	KviMemory::free(out);

	encoded.prepend('*'); // prepend the signature

	return true;
}

bool KviMircryptionEngine::doDecryptCBC(KviCString & encoded, KviCString & plain)
{
	if(*(encoded.ptr()) != '*')
	{
		qDebug("WARNING: specified a CBC key but the incoming message doesn't seem to be a CBC one");
		return doDecryptECB(encoded, plain);
	}
	encoded.cutLeft(1);

	char * tmpBuf;
	int len = encoded.base64ToBuffer(&tmpBuf, false);
	if(len < 0)
	{
		setLastError(__tr2qs("The message is not a base64 string: this is not my stuff"));
		return false;
	}
	if((len < 8) || (len % 8))
	{
		setLastError(__tr2qs("The message doesn't seem to be encoded with CBC Mircryption"));
		if(len > 0)
			KviCString::freeBuffer(tmpBuf);
		return false;
	}

	plain.setLen(len);
	BlowFish bf((unsigned char *)m_szDecryptKey.ptr(), m_szDecryptKey.len());
	bf.ResetChain();
	bf.Decrypt((unsigned char *)tmpBuf, (unsigned char *)plain.ptr(), len, BlowFish::CBC);

	// kill the first 8 bytes (random IV)
	plain.cutLeft(8);

	KviCString::freeBuffer(tmpBuf);

	return true;
}

static KviCryptEngine * allocMircryptionEngine()
{
	return new KviMircryptionEngine();
}

#endif

///////////////////////////////////////////////////////////////////////////////
// module routines
///////////////////////////////////////////////////////////////////////////////

static bool rijndael_module_init(KviModule * m)
{
#ifdef COMPILE_CRYPT_SUPPORT
	g_pEngineList = new KviPointerList<KviCryptEngine>;
	g_pEngineList->setAutoDelete(false);

	QString szFormat = __tr2qs("Cryptographic engine based on the Advanced Encryption Standard (AES) algorithm called Rijndael. "
	                           "<br/>The text is first encrypted with Rijndael and then converted to %1 notation. "
	                           "The keys used are %2 bit long and will be padded with zeros if you provide shorter ones. "
	                           "If only one key is provided, this engine will use it for both encrypting and decrypting. "
	                           "See the Rijndael module documentation for more info on the algorithm used. "
	                           "<br/>This engine works in CBC mode by default: other modes are considered INSECURE and should be avoided. "
	                           "The old pseudo-CBC mode used in KVIrc &lt; 4.2 is still available prefixing your key(s) with \"old:\"; "
	                           "if you want to use ECB mode you must prefix your key(s) with \"ecb:\".");

	// FIXME: Maybe convert this repeated code to a function eh ?

	KviCryptEngineDescription * d = new KviCryptEngineDescription;
	d->m_szName = "Rijndael128Hex";
	d->m_szAuthor = "Szymon Stefanek";
	d->m_szDescription = QString(szFormat).arg(__tr2qs("hexadecimal")).arg(128);
	d->m_iFlags = KviCryptEngine::CanEncrypt | KviCryptEngine::CanDecrypt | KviCryptEngine::WantEncryptKey | KviCryptEngine::WantDecryptKey;
	d->m_allocFunc = allocRijndael128HexEngine;
	d->m_deallocFunc = deallocRijndaelCryptEngine;
	m->registerCryptEngine(d);

	d = new KviCryptEngineDescription;
	d->m_szName = "Rijndael192Hex";
	d->m_szAuthor = "Szymon Stefanek";
	d->m_szDescription = QString(szFormat).arg(__tr2qs("hexadecimal")).arg(192);
	d->m_iFlags = KviCryptEngine::CanEncrypt | KviCryptEngine::CanDecrypt | KviCryptEngine::WantEncryptKey | KviCryptEngine::WantDecryptKey;
	d->m_allocFunc = allocRijndael192HexEngine;
	d->m_deallocFunc = deallocRijndaelCryptEngine;
	m->registerCryptEngine(d);

	d = new KviCryptEngineDescription;
	d->m_szName = "Rijndael256Hex";
	d->m_szAuthor = "Szymon Stefanek";
	d->m_szDescription = QString(szFormat).arg(__tr2qs("hexadecimal")).arg(256);
	d->m_iFlags = KviCryptEngine::CanEncrypt | KviCryptEngine::CanDecrypt | KviCryptEngine::WantEncryptKey | KviCryptEngine::WantDecryptKey;
	d->m_allocFunc = allocRijndael256HexEngine;
	d->m_deallocFunc = deallocRijndaelCryptEngine;
	m->registerCryptEngine(d);

	d = new KviCryptEngineDescription;
	d->m_szName = "Rijndael128Base64";
	d->m_szAuthor = "Szymon Stefanek";
	d->m_szDescription = QString(szFormat).arg(__tr2qs("base64")).arg(128);
	d->m_iFlags = KviCryptEngine::CanEncrypt | KviCryptEngine::CanDecrypt | KviCryptEngine::WantEncryptKey | KviCryptEngine::WantDecryptKey;
	d->m_allocFunc = allocRijndael128Base64Engine;
	d->m_deallocFunc = deallocRijndaelCryptEngine;
	m->registerCryptEngine(d);

	d = new KviCryptEngineDescription;
	d->m_szName = "Rijndael192Base64";
	d->m_szAuthor = "Szymon Stefanek";
	d->m_szDescription = QString(szFormat).arg(__tr2qs("base64")).arg(192);
	d->m_iFlags = KviCryptEngine::CanEncrypt | KviCryptEngine::CanDecrypt | KviCryptEngine::WantEncryptKey | KviCryptEngine::WantDecryptKey;
	d->m_allocFunc = allocRijndael192Base64Engine;
	d->m_deallocFunc = deallocRijndaelCryptEngine;
	m->registerCryptEngine(d);

	d = new KviCryptEngineDescription;
	d->m_szName = "Rijndael256Base64";
	d->m_szAuthor = "Szymon Stefanek";
	d->m_szDescription = QString(szFormat).arg(__tr2qs("base64")).arg(256);
	d->m_iFlags = KviCryptEngine::CanEncrypt | KviCryptEngine::CanDecrypt | KviCryptEngine::WantEncryptKey | KviCryptEngine::WantDecryptKey;
	d->m_allocFunc = allocRijndael256Base64Engine;
	d->m_deallocFunc = deallocRijndaelCryptEngine;
	m->registerCryptEngine(d);

	d = new KviCryptEngineDescription;
	d->m_szName = "Mircryption";
	d->m_szAuthor = "Szymon Stefanek";
	d->m_szDescription = __tr2qs("Popular cryptographic engine based on the Blowfish encryption algorithm. "
	                             "<br/>The text is first encrypted with Blowfish and then converted to base64 notation. "
	                             "The keys used have variable length and are specified as character strings. "
	                             "You can specify keys with length up to 56 bytes (448 bits) by default or change this with /option uintMaximumBlowFishKeySize UINT. "
	                             "Blowfish allows for up to 72 bytes for keys, but it is not advised to use more than 56 bytes for security reasons. "
	                             "Only increase this value if you need compatibility with another client's Blowfish implementation. "
	                             "If only one key is provided, this engine will use it for both encrypting and decrypting. "
	                             "<br/>This engine works in CBC mode by default: if you want to use the old and INSECURE ECB mode you must prefix your key(s) with \"ecb:\" or \"old:\".");
	d->m_iFlags = KviCryptEngine::CanEncrypt | KviCryptEngine::CanDecrypt | KviCryptEngine::WantEncryptKey | KviCryptEngine::WantDecryptKey;
	d->m_allocFunc = allocMircryptionEngine;
	d->m_deallocFunc = deallocRijndaelCryptEngine;
	m->registerCryptEngine(d);

	return true;
#else
	return false;
#endif
}

static bool rijndael_module_cleanup(KviModule * m)
{
#ifdef COMPILE_CRYPT_SUPPORT
	while(g_pEngineList->first())
		delete g_pEngineList->first();
	delete g_pEngineList;
	g_pEngineList = nullptr;
	m->unregisterCryptEngines();
	return true;
#else
	return false;
#endif
}

static bool rijndael_module_can_unload(KviModule *)
{
#ifdef COMPILE_CRYPT_SUPPORT
	return g_pEngineList->isEmpty();
#else
	return true;
#endif
}

KVIRC_MODULE(
    "Rijndael crypt engine",
    "4.0.0",
    "Szymon Stefanek <pragma at kvirc dot net>\n"
    "Fabio Bas <ctrlaltca at gmail dot com>",
    "Exports the rijndael crypt engine",
    rijndael_module_init,
    rijndael_module_can_unload,
    0,
    rijndael_module_cleanup,
    0)
