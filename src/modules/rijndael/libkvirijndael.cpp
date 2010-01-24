//=============================================================================
//
//   File : libkvirijndael.cpp
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

#include "libkvirijndael.h"
#include "rijndael.h"

#include "kvi_module.h"
#include "kvi_debug.h"
#include "kvi_locale.h"
#include "kvi_mirccntrl.h"
#include "kvi_time.h"

//#warning "Other engines: mircStrip koi2win colorizer lamerizer etc.."

/*
	@doc: rijndael
	@type:
		module
	@short:
		The Rijndael cryptographic engines
	@title:
		The rijndael module
	@body:
		The rijndael module exports six [doc:crypt_engines]cryptographic engines[/doc] based
		on the Advanced Encryptiong Standard algorithm called Rijndael. Rijndael was
		originally written by Joan Daemen and Vincent Rijmen. The original Rijndael
		description is available at http://www.esat.kuleuven.ac.be/~rijmen/rijndael/.[br]
		It is a private key block cipher that has been designed to replace
		the widely used DES, and it should provide at leas a decent security agains
		common attacks. Theoretically the best attack that one can perform on this cipher
		is the "brute force" attack that requires a really massive parallel computation:
		actually out of the possibilities of a common "hacker".[br]
		My implementation allows the usage of 128, 192 and 256 bit keys
		on 128 bit data blocks. The encrypted binary data buffer is then converted
		into an ascii-string by using the base64 conversion or hex-digit-string rappresentation.
		The six engines are the six possible combinations of the key lengths and ascii-string
		conversions.
*/



#if defined(COMPILE_CRYPT_SUPPORT) || defined(Q_MOC_RUN)
	#include "kvi_memmove.h"
	#include "kvi_malloc.h"
	#include "kvi_pointerlist.h"

	static KviPointerList<KviCryptEngine> * g_pEngineList = 0;


	KviRijndaelEngine::KviRijndaelEngine()
	: KviCryptEngine()
	{
		g_pEngineList->append(this);
		m_pEncryptCipher = 0;
		m_pDecryptCipher = 0;
	}

	KviRijndaelEngine::~KviRijndaelEngine()
	{
		g_pEngineList->removeRef(this);
		if(m_pEncryptCipher)delete m_pEncryptCipher;
		if(m_pDecryptCipher)delete m_pDecryptCipher;
	}

	bool KviRijndaelEngine::init(const char *encKey,int encKeyLen,const char *decKey,int decKeyLen)
	{
		if(m_pEncryptCipher)
		{
			delete m_pEncryptCipher;
			m_pEncryptCipher = 0;
		}
		if(m_pDecryptCipher)
		{
			delete m_pDecryptCipher;
			m_pDecryptCipher = 0;
		}

		if(encKey && (encKeyLen > 0))
		{
			if(!(decKey && (decKeyLen > 0)))
			{
				decKey = encKey;
				decKeyLen = encKeyLen;
			} // else all
		} else {
			// no encrypt key specified...
			if(decKey && decKeyLen)
			{
				encKey = decKey;
				encKeyLen = decKeyLen;
			} else {
				// both keys missing
				setLastError(__tr("Missing both encrypt and decrypt key: at least one is needed"));
				return false;
			}
		}

		int defLen = getKeyLen();

		char * encryptKey = (char *)kvi_malloc(defLen);
		char * decryptKey = (char *)kvi_malloc(defLen);

		if(encKeyLen > defLen)encKeyLen = defLen;
		kvi_memmove(encryptKey,encKey,encKeyLen);
		if(encKeyLen < defLen)kvi_memset(encryptKey + encKeyLen,'0',defLen - encKeyLen);

		if(decKeyLen > defLen)decKeyLen = defLen;
		kvi_memmove(decryptKey,decKey,decKeyLen);
		if(decKeyLen < defLen)kvi_memset(decryptKey + decKeyLen,'0',defLen - decKeyLen);

		m_pEncryptCipher = new Rijndael();
		int retVal = m_pEncryptCipher->init(Rijndael::CBC,Rijndael::Encrypt,(unsigned char *)encryptKey,getKeyLenId());
		kvi_free(encryptKey);
		if(retVal != RIJNDAEL_SUCCESS)
		{
			kvi_free(decryptKey);
			delete m_pEncryptCipher;
			m_pEncryptCipher = 0;
			setLastErrorFromRijndaelErrorCode(retVal);
			return false;
		}

		m_pDecryptCipher = new Rijndael();
		retVal = m_pDecryptCipher->init(Rijndael::CBC,Rijndael::Decrypt,(unsigned char *)decryptKey,getKeyLenId());
		kvi_free(decryptKey);
		if(retVal != RIJNDAEL_SUCCESS)
		{
			delete m_pEncryptCipher;
			m_pEncryptCipher = 0;
			delete m_pDecryptCipher;
			m_pDecryptCipher = 0;
			setLastErrorFromRijndaelErrorCode(retVal);
			return false;
		}

		return true;
	}

	void KviRijndaelEngine::setLastErrorFromRijndaelErrorCode(int errCode)
	{
		switch(errCode)
		{
			case RIJNDAEL_SUCCESS: setLastError(__tr("Error 0: Success ?")); break;
			case RIJNDAEL_UNSUPPORTED_MODE: setLastError(__tr("Unsupported crypt mode")); break;
			case RIJNDAEL_UNSUPPORTED_DIRECTION: setLastError(__tr("Unsupported direction")); break;
			case RIJNDAEL_UNSUPPORTED_KEY_LENGTH: setLastError(__tr("Unsupported key length")); break;
			case RIJNDAEL_BAD_KEY: setLastError(__tr("Bad key data")); break;
			case RIJNDAEL_NOT_INITIALIZED: setLastError(__tr("Engine not initialized")); break;
			case RIJNDAEL_BAD_DIRECTION: setLastError(__tr("Invalid direction for this engine")); break;
			case RIJNDAEL_CORRUPTED_DATA: setLastError(__tr("Corrupted message data or invalid decrypt key")); break;
			default: setLastError(__tr("Unknown error")); break;
		}
	}

	KviCryptEngine::EncryptResult KviRijndaelEngine::encrypt(const char * plainText,KviStr &outBuffer)
	{
		if(!m_pEncryptCipher)
		{
			setLastError(__tr("Ops...encrypt cipher not initialized"));
			return KviCryptEngine::EncryptError;
		}
		int len = (int)kvi_strLen(plainText);
		char * buf = (char *)kvi_malloc(len + 16);

		int retVal = m_pEncryptCipher->padEncrypt((const unsigned char *)plainText,len,(unsigned char *)buf);
		if(retVal < 0)
		{
			kvi_free(buf);
			setLastErrorFromRijndaelErrorCode(retVal);
			return KviCryptEngine::EncryptError;
		}

		if(!binaryToAscii(buf,retVal,outBuffer))
		{
			kvi_free(buf);
			return KviCryptEngine::EncryptError;
		}
		kvi_free(buf);

		if(outBuffer.len() > maxEncryptLen())
		{
			if(maxEncryptLen() > 0)
			{
				setLastError(__tr("Data buffer too long"));
				return KviCryptEngine::EncryptError;
			}
		}
		outBuffer.prepend(KVI_TEXT_CRYPTESCAPE);
		return KviCryptEngine::Encrypted;
	}

	KviCryptEngine::DecryptResult KviRijndaelEngine::decrypt(const char * inBuffer,KviStr &plainText)
	{
		if(!m_pDecryptCipher)
		{
			setLastError(__tr("Ops...decrypt cipher not initialized"));
			return KviCryptEngine::DecryptError;
		}

		if(*inBuffer != KVI_TEXT_CRYPTESCAPE)
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

		if(!asciiToBinary(inBuffer,&len,&binary))return KviCryptEngine::DecryptError;

		char * buf = (char *)kvi_malloc(len + 1);

		int retVal = m_pDecryptCipher->padDecrypt((const unsigned char *)binary,len,(unsigned char *)buf);
		kvi_free(binary);

		if(retVal < 0)
		{
			kvi_free(buf);
			setLastErrorFromRijndaelErrorCode(retVal);
			return KviCryptEngine::DecryptError;
		}

		buf[retVal] = '\0';

		plainText = buf;

		kvi_free(buf);
		return KviCryptEngine::DecryptOkWasEncrypted;
	}

	bool KviRijndaelHexEngine::binaryToAscii(const char * inBuffer,int len,KviStr &outBuffer)
	{
		outBuffer.bufferToHex(inBuffer,len);
		return true;
	}

	bool KviRijndaelHexEngine::asciiToBinary(const char * inBuffer,int * len,char ** outBuffer)
	{
		KviStr hex(inBuffer);
		char * tmpBuf;
		*len = hex.hexToBuffer(&tmpBuf,false);
		if(*len < 0)
		{
			setLastError(__tr("The message is not a hexadecimal string: this is not my stuff"));
			return false;
		} else {
			if(len > 0)
			{
				*outBuffer = (char *)kvi_malloc(*len);
				kvi_memmove(*outBuffer,tmpBuf,*len);
				KviStr::freeBuffer(tmpBuf);
			}
		}
		return true;
	}

	bool KviRijndaelBase64Engine::binaryToAscii(const char * inBuffer,int len,KviStr &outBuffer)
	{
		outBuffer.bufferToBase64(inBuffer,len);
		return true;
	}

	bool KviRijndaelBase64Engine::asciiToBinary(const char * inBuffer,int * len,char ** outBuffer)
	{
		KviStr base64(inBuffer);
		char * tmpBuf;
		*len = base64.base64ToBuffer(&tmpBuf,false);
		if(*len < 0)
		{
			setLastError(__tr("The message is not a base64 string: this is not my stuff"));
			return false;
		} else {
			if(len > 0)
			{
				*outBuffer = (char *)kvi_malloc(*len);
				kvi_memmove(*outBuffer,tmpBuf,*len);
				KviStr::freeBuffer(tmpBuf);
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

	#include "ablowfish.h"

	KviMircryptionEngine::KviMircryptionEngine()
	: KviCryptEngine()
	{
		g_pEngineList->append(this);
	}

	KviMircryptionEngine::~KviMircryptionEngine()
	{
		g_pEngineList->removeRef(this);
	}

	bool KviMircryptionEngine::init(const char * encKey,int encKeyLen,const char * decKey,int decKeyLen)
	{
		if(encKey && (encKeyLen > 0))
		{
			if(!(decKey && (decKeyLen > 0)))
			{
				decKey = encKey;
				decKeyLen = encKeyLen;
			} // else all
		} else {
			// no encrypt key specified...
			if(decKey && decKeyLen)
			{
				encKey = decKey;
				encKeyLen = decKeyLen;
			} else {
				// both keys missing
				setLastError(__tr("Missing both encrypt and decrypt key: at least one is needed"));
				return false;
			}
		}
		m_szEncryptKey = KviStr(encKey,encKeyLen);
		m_szDecryptKey = KviStr(decKey,decKeyLen);
		if(kvi_strEqualCIN("cbc:",m_szEncryptKey.ptr(),4) && (m_szEncryptKey.len() > 4))
			m_szEncryptKey.cutLeft(4);
		else
			m_bEncryptCBC = false;
		if(kvi_strEqualCIN("cbc:",m_szDecryptKey.ptr(),4) && (m_szDecryptKey.len() > 4))
			m_szDecryptKey.cutLeft(4);
		else
			m_bDecryptCBC = false;
		return true;
	}

	KviCryptEngine::EncryptResult KviMircryptionEngine::encrypt(const char * plainText,KviStr &outBuffer)
	{
		KviStr szPlain = plainText;
		outBuffer = "";
		if(m_bEncryptCBC)
		{
			if(!doEncryptCBC(szPlain,outBuffer))return KviCryptEngine::EncryptError;
		} else {
			if(!doEncryptECB(szPlain,outBuffer))return KviCryptEngine::EncryptError;
		}
		outBuffer.prepend("+OK ");

		if(outBuffer.len() > maxEncryptLen())
		{
			if(maxEncryptLen() > 0)
			{
				setLastError(__tr("Data buffer too long"));
				return KviCryptEngine::EncryptError;
			}
		}

		//outBuffer = MCPS2_STARTTAG;
		//outBuffer += MCPS2_ENDTAG;
		return KviCryptEngine::Encrypted;
	}

	KviCryptEngine::DecryptResult KviMircryptionEngine::decrypt(const char * inBuffer,KviStr &plainText)
	{
		plainText = "";
		KviStr szIn = inBuffer;
		// various old versions
		if(kvi_strEqualCSN(inBuffer,"mcps ",5))
			szIn.cutLeft(5);
		else if(kvi_strEqualCSN(inBuffer,"+OK ",4))
			szIn.cutLeft(4);
		else {
			plainText = szIn;
			return KviCryptEngine::DecryptOkWasPlainText;
		}

		if(m_bDecryptCBC)return doDecryptCBC(szIn,plainText) ? KviCryptEngine::DecryptOkWasEncrypted : KviCryptEngine::DecryptError;
		return doDecryptECB(szIn,plainText) ? KviCryptEngine::DecryptOkWasEncrypted : KviCryptEngine::DecryptError;

		/*
		int len1 = kvi_strLen(MCPS2_STARTTAG);
		int len2 = kvi_strLen(MCPS2_ENDTAG);
		while(szIn.len() > 0)
		{
			int idx = szIn.findFirstIdx(MCPS2_STARTTAG);
			if(idx == -1)
			{
				// no more encrypted stuff
				plainText += szIn;
				return true;
			}
			if(idx > 0) // a non encrypted block
				plainText += szIn.left(idx);
			szIn.cutLeft(idx + len1);

			idx = szIn.findFirstIdx(MCPS2_ENDTAG);
			if(idx != -1)
			{
				KviStr toDecrypt = szIn.left(idx);
				if(!doDecrypt(toDecrypt,plainText))return false;
			}
			szIn.cutLeft(idx + len2);
		}
		*/
	}

	static unsigned char fake_base64[]="./0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

	unsigned int fake_base64dec(unsigned char c)
	{
		static char base64unmap[255];
		static bool didinit=false;
		int i;

		if(!didinit)
		{
			// initialize base64unmap
			for (i=0;i<255;++i)base64unmap[i]=0;
			for (i=0;i<64;++i)base64unmap[fake_base64[i]]=i;
			didinit=true;
		}

		return base64unmap[c];
	}

	static void byteswap_buffer(unsigned char * p,int len)
	{
		while(len > 0)
		{
			unsigned char aux = p[0];
			p[0] = p[3];
			p[3] = aux;
			aux = p[1];
			p[1] = p[2];
			p[2] = aux;
			p += 4;
			len -= 4;
		}
	}

	bool KviMircryptionEngine::doEncryptECB(KviStr &plain,KviStr &encoded)
	{
		// make sure it is a multiple of 8 bytes (eventually pad with zeroes)
		if(plain.len() % 8)
		{
			int oldL = plain.len();
			plain.setLength(plain.len() + (8 - (plain.len() % 8)));
			char * padB = plain.ptr() + oldL;
			char * padE = plain.ptr() + plain.len();
			while(padB < padE)*padB++ = 0;
		}

		//byteswap_buffer((unsigned char *)plain.ptr(),plain.len());

		unsigned char * out =(unsigned char *)kvi_malloc(plain.len()); // we use this to avoid endiannes problems

		BlowFish bf((unsigned char *)m_szEncryptKey.ptr(),m_szEncryptKey.len());
		bf.ResetChain();
		bf.Encrypt((unsigned char *)plain.ptr(),out,plain.len(),BlowFish::ECB);

		// FIXME: this is probably needed only on LittleEndian machines!
		byteswap_buffer((unsigned char *)out,plain.len());

		// da uglybase64 encoding
		unsigned char * outb = out;
		unsigned char * oute = out + plain.len();

		int ll = (plain.len() * 3) / 2;
		encoded.setLength(ll);

		unsigned char * p = (unsigned char *)encoded.ptr();
		while(outb < oute)
		{
			quint32 * dd1 = (quint32 *)outb;
			outb += 4;
			quint32 * dd2 = (quint32 *)outb;
			outb += 4;
			*p++ = fake_base64[*dd2 & 0x3f]; *dd2 >>= 6;
			*p++ = fake_base64[*dd2 & 0x3f]; *dd2 >>= 6;
			*p++ = fake_base64[*dd2 & 0x3f]; *dd2 >>= 6;
			*p++ = fake_base64[*dd2 & 0x3f]; *dd2 >>= 6;
			*p++ = fake_base64[*dd2 & 0x3f]; *dd2 >>= 6;
			*p++ = fake_base64[*dd2 & 0x3f];

			*p++ = fake_base64[*dd1 & 0x3f]; *dd1 >>= 6;
			*p++ = fake_base64[*dd1 & 0x3f]; *dd1 >>= 6;
			*p++ = fake_base64[*dd1 & 0x3f]; *dd1 >>= 6;
			*p++ = fake_base64[*dd1 & 0x3f]; *dd1 >>= 6;
			*p++ = fake_base64[*dd1 & 0x3f]; *dd1 >>= 6;
			*p++ = fake_base64[*dd1 & 0x3f];
		}

		kvi_free(out);
		return true;
	}

	bool KviMircryptionEngine::doDecryptECB(KviStr &encoded,KviStr &plain)
	{
		// encoded is in this strange base64...
		// make sure its length is multiple of 12 (eventually pad with zeroes)
		if(encoded.len() % 12)
		{
			int oldL = encoded.len();
			encoded.setLength(encoded.len() + (12 - (encoded.len() % 12)));
			char * padB = encoded.ptr() + oldL;
			char * padE = encoded.ptr() + encoded.len();
			while(padB < padE)*padB++ = 0;
		}

		// a fake base64 decoding algo, use a different character set
		// and stuff 6 bytes at a time into a 32 bit long...
		int ll = (encoded.len() * 2) / 3;

		unsigned char * buf = (unsigned char *)kvi_malloc(ll);
		unsigned char * p = (unsigned char *)encoded.ptr();
		unsigned char * e = p + encoded.len();
		int i;
		unsigned char * bufp = buf;
		while(p < e)
		{
			quint32 * dw1 = (quint32 *)bufp;
			bufp += 4;
			quint32 * dw2 = (quint32 *)bufp;
			bufp += 4;
			*dw2 = 0;
			for(i=0;i < 6;i++)*dw2 |= (fake_base64dec(*p++)) << (i * 6);
			*dw1 = 0;
			for(i=0;i < 6;i++)*dw1 |= (fake_base64dec(*p++)) << (i * 6);
		}

		// FIXME: this is probably needed only on LittleEndian machines!
		byteswap_buffer((unsigned char *)buf,ll);

		plain.setLength(ll);
		BlowFish bf((unsigned char *)m_szDecryptKey.ptr(),m_szDecryptKey.len());
		bf.ResetChain();
		bf.Decrypt(buf,(unsigned char *)plain.ptr(),ll,BlowFish::ECB);

		//byteswap_buffer((unsigned char *)plain.ptr(),ll);

		kvi_free(buf);

		return true;
	}

	bool KviMircryptionEngine::doEncryptCBC(KviStr &plain,KviStr &encoded)
	{
		// make sure it is a multiple of 8 bytes (eventually pad with zeroes)
		if(plain.len() % 8)
		{
			int oldL = plain.len();
			plain.setLength(plain.len() + (8 - (plain.len() % 8)));
			char * padB = plain.ptr() + oldL;
			char * padE = plain.ptr() + plain.len();
			while(padB < padE)*padB++ = 0;
		}

		int ll = plain.len() + 8;
		unsigned char * in = (unsigned char *)kvi_malloc(ll);

		// choose an IV
		static bool bDidInit = false;

		int t = (int)kvi_unixTime();

		if(!bDidInit)
		{
			srand(t);
			bDidInit = true;
		}

		for(int i=0;i<8;i++)in[i] = (unsigned char)(rand() % 256);

		kvi_fastmove(in+8,plain.ptr(),plain.len());

		// encrypt
		unsigned char * out = (unsigned char *)kvi_malloc(ll);
		BlowFish bf((unsigned char *)m_szEncryptKey.ptr(),m_szEncryptKey.len());
		bf.ResetChain();
		bf.Encrypt(in,out,ll,BlowFish::CBC);
		kvi_free(in);

		encoded.bufferToBase64((const char *)out,ll);
		kvi_free(out);

		encoded.prepend('*'); // prepend the signature

		return true;
	}

	bool KviMircryptionEngine::doDecryptCBC(KviStr &encoded,KviStr &plain)
	{
		if(*(encoded.ptr()) != '*')
		{
			debug("WARNING: Specified a CBC key but the incoming message doesn't seem to be a CBC one");
			return doDecryptECB(encoded,plain);
		}
		encoded.cutLeft(1);

		char * tmpBuf;
		int len = encoded.base64ToBuffer(&tmpBuf,false);
		if(len < 0)
		{
			setLastError(__tr("The message is not a base64 string: this is not my stuff"));
			return false;
		}
		if((len < 8) || (len % 8))
		{
			setLastError(__tr("The message doesn't seem to be encoded with CBC Mircryption"));
			if(len > 0)KviStr::freeBuffer(tmpBuf);
			return false;
		}

		plain.setLength(len);
		BlowFish bf((unsigned char *)m_szDecryptKey.ptr(),m_szDecryptKey.len());
		bf.ResetChain();
		bf.Decrypt((unsigned char *)tmpBuf,(unsigned char *)plain.ptr(),len,BlowFish::CBC);

		// kill the first 8 bytes (random IV)
		plain.cutLeft(8);

		KviStr::freeBuffer(tmpBuf);

		return true;
	}


	static KviCryptEngine * allocMircryptionEngine()
	{
		return new KviMircryptionEngine();
	}

#endif


// =======================================
// module routines
// =======================================
static bool rijndael_module_init(KviModule * m)
{
#ifdef COMPILE_CRYPT_SUPPORT
	g_pEngineList = new KviPointerList<KviCryptEngine>;
	g_pEngineList->setAutoDelete(false);

	KviStr format = __tr("Cryptographic engine based on the\n" \
		"Advanced Encryption Standard (AES)\n" \
		"algorithm called Rijndael.\n" \
		"The text is first encrypted with rijndael\n" \
		"and then converted to %s notation.\n" \
		"The keys used are %d bit long and will be padded\n" \
		"with zeros if you provide shorter ones.\n" \
		"If only one key is provided, this engine\n" \
		"will use it for both encrypting and decrypting.\n" \
		"See the rijndael module documentation\n" \
		"for more info on the algorithm used.\n");

	// FIXME: Maybe convert this repeated code to a function eh ?

	KviCryptEngineDescription * d = new KviCryptEngineDescription;
	d->szName = "Rijndael128Hex";
	d->szAuthor = "Szymon Stefanek";
	d->szDescription.sprintf(format.ptr(),__tr("hexadecimal"),128);
	d->iFlags = KVI_CRYPTENGINE_CAN_ENCRYPT | KVI_CRYPTENGINE_CAN_DECRYPT |
			KVI_CRYPTENGINE_WANT_ENCRYPT_KEY | KVI_CRYPTENGINE_WANT_DECRYPT_KEY;
	d->allocFunc = allocRijndael128HexEngine;
	d->deallocFunc = deallocRijndaelCryptEngine;
	m->registerCryptEngine(d);

	d = new KviCryptEngineDescription;
	d->szName = "Rijndael192Hex";
	d->szAuthor = "Szymon Stefanek";
	d->szDescription.sprintf(format.ptr(),__tr("hexadecimal"),192);
	d->iFlags = KVI_CRYPTENGINE_CAN_ENCRYPT | KVI_CRYPTENGINE_CAN_DECRYPT |
			KVI_CRYPTENGINE_WANT_ENCRYPT_KEY | KVI_CRYPTENGINE_WANT_DECRYPT_KEY;
	d->allocFunc = allocRijndael192HexEngine;
	d->deallocFunc = deallocRijndaelCryptEngine;
	m->registerCryptEngine(d);

	d = new KviCryptEngineDescription;
	d->szName = "Rijndael256Hex";
	d->szAuthor = "Szymon Stefanek";
	d->szDescription.sprintf(format.ptr(),__tr("hexadecimal"),256);
	d->iFlags = KVI_CRYPTENGINE_CAN_ENCRYPT | KVI_CRYPTENGINE_CAN_DECRYPT |
			KVI_CRYPTENGINE_WANT_ENCRYPT_KEY | KVI_CRYPTENGINE_WANT_DECRYPT_KEY;
	d->allocFunc = allocRijndael256HexEngine;
	d->deallocFunc = deallocRijndaelCryptEngine;
	m->registerCryptEngine(d);

	d = new KviCryptEngineDescription;
	d->szName = "Rijndael128Base64";
	d->szAuthor = "Szymon Stefanek";
	d->szDescription.sprintf(format.ptr(),__tr("base64"),128);
	d->iFlags = KVI_CRYPTENGINE_CAN_ENCRYPT | KVI_CRYPTENGINE_CAN_DECRYPT |
			KVI_CRYPTENGINE_WANT_ENCRYPT_KEY | KVI_CRYPTENGINE_WANT_DECRYPT_KEY;
	d->allocFunc = allocRijndael128Base64Engine;
	d->deallocFunc = deallocRijndaelCryptEngine;
	m->registerCryptEngine(d);

	d = new KviCryptEngineDescription;
	d->szName = "Rijndael192Base64";
	d->szAuthor = "Szymon Stefanek";
	d->szDescription.sprintf(format.ptr(),__tr("base64"),192);
	d->iFlags = KVI_CRYPTENGINE_CAN_ENCRYPT | KVI_CRYPTENGINE_CAN_DECRYPT |
			KVI_CRYPTENGINE_WANT_ENCRYPT_KEY | KVI_CRYPTENGINE_WANT_DECRYPT_KEY;
	d->allocFunc = allocRijndael192Base64Engine;
	d->deallocFunc = deallocRijndaelCryptEngine;
	m->registerCryptEngine(d);

	d = new KviCryptEngineDescription;
	d->szName = "Rijndael256Base64";
	d->szAuthor = "Szymon Stefanek";
	d->szDescription.sprintf(format.ptr(),__tr("base64"),256);
	d->iFlags = KVI_CRYPTENGINE_CAN_ENCRYPT | KVI_CRYPTENGINE_CAN_DECRYPT |
			KVI_CRYPTENGINE_WANT_ENCRYPT_KEY | KVI_CRYPTENGINE_WANT_DECRYPT_KEY;
	d->allocFunc = allocRijndael256Base64Engine;
	d->deallocFunc = deallocRijndaelCryptEngine;
	m->registerCryptEngine(d);


	d = new KviCryptEngineDescription;
	d->szName = "Mircryption";
	d->szAuthor = "Szymon Stefanek";
	d->szDescription = __tr("Popular cryptographic engine based on the\n" \
		"old Blowfish encryption algorithm.\n" \
		"The text is first encrypted with Blowfish \n" \
		"and then converted to base64 notation.\n" \
		"The keys used have variable length and\n" \
		"are specified as character strings.\n" \
		"You can specify keys long up to 56 bytes (448 bits).\n" \
		"If only one key is provided, this engine\n" \
		"will use it for both encrypting and decrypting.\n" \
		"This engine works in ECB mode by default:\n" \
		"if you want to use CBC mode you must prefix\n" \
		"your key(s) with \"cbc:\".\n");
	d->iFlags = KVI_CRYPTENGINE_CAN_ENCRYPT | KVI_CRYPTENGINE_CAN_DECRYPT |
			KVI_CRYPTENGINE_WANT_ENCRYPT_KEY | KVI_CRYPTENGINE_WANT_DECRYPT_KEY;
	d->allocFunc = allocMircryptionEngine;
	d->deallocFunc = deallocRijndaelCryptEngine;
	m->registerCryptEngine(d);


	return true;
#else
	return false;
#endif
}

static bool rijndael_module_cleanup(KviModule *m)
{
#ifdef COMPILE_CRYPT_SUPPORT
	while(g_pEngineList->first())delete g_pEngineList->first();
	delete g_pEngineList;
	g_pEngineList = 0;
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

// =======================================
// plugin definition structure
// =======================================
KVIRC_MODULE(
	"Rijndael crypt engine",
	"4.0.0",
	"Szymon Stefanek <pragma at kvirc dot net>" ,
	"Exports the rijndael crypt engine",
	rijndael_module_init ,
	rijndael_module_can_unload,
	0,
	rijndael_module_cleanup,
	0
)

#ifdef COMPILE_CRYPT_SUPPORT
	#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
		#include "libkvirijndael.moc"
	#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
#endif
