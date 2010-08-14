//=============================================================================
//
//   File : libkvirijndael_cryptopp.cpp
//   Creation date : Fri, 17 Jul 2009 22:04:54 +0200 by Kai Wasserbäch
//      based on libkvirijndael.cpp as this aims to be a drop-in replacement
//
//   This file is part of the KVirc irc client distribution
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

#include "libkvirijndael_cryptopp.h"

#include <cryptopp/aes.h>
#include <cryptopp/blowfish.h>
#include <cryptopp/modes.h>
#include <cryptopp/base64.h>
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>
#include <string>

#include "kvi_module.h"
#include "kvi_debug.h"
#include "kvi_locale.h"
#include "kvi_mirccntrl.h"

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
the widely used DES, and it should provide at least a decent security against
common attacks. Theoretically the best attack that one can perform on this cipher
is the "brute force" attack that requires a really massive parallel computation:
actually out of the possibilities of a common "hacker".[br]
My implementation allows the usage of 128, 192 and 256 bit keys
on 128 bit data blocks. The encrypted binary data buffer is then converted
into an ascii-string by using the base64 conversion or hex-digit-string rappresentation.
The six engines are the six possible combinations of the key lengths and ascii-string
conversions.
*/



#include "kvi_pointerlist.h"

static KviPointerList<KviCryptEngine> * g_pEngineList = 0;

// The following lines contain a compatible (to the embedded version)
// implementation of the KviCryptEngine for Rijndael. Therefore the output
// gets encoded the same way (Hex or ASCII) as with the embedded version.
KviRijndaelEngine::KviRijndaelEngine()
: KviCryptEngine()
{
    g_pEngineList->append(this);
    m_szEncKey.clear();
    m_szDecKey.clear();
}

KviRijndaelEngine::~KviRijndaelEngine()
{
    g_pEngineList->removeRef(this);
    m_szEncKey.clear();
    m_szDecKey.clear();
}

bool KviRijndaelEngine::init(const char *encKey, int encKeyLen, const char *decKey, int decKeyLen)
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
            setLastError(__tr("Missing encryption and decryption key: at least one is needed"));
            return false;
        }
    }

    m_szEncKey = encKey;
    m_szDecKey = decKey;
    
    if(encKeyLen > getKeyLen()) {
        m_szEncKey = m_szEncKey.substr(0,getKeyLen());
    }
    
    if(encKeyLen < getKeyLen()) {
        m_szEncKey.resize(getKeyLen());
    }
    
    if(decKeyLen > getKeyLen()) {
        m_szDecKey = m_szDecKey.substr(0,getKeyLen());
    }
    
    if(decKeyLen < getKeyLen()) {
        m_szDecKey.resize(getKeyLen());
    }
    
    if(m_szEncKey.empty() || m_szDecKey.empty()) {
        return false;
    }
    else {
        return true;
    }
}

KviCryptEngine::EncryptResult KviRijndaelEngine::encrypt(const char * plainText,KviStr &outBuffer)
{
    std::string cipher;
    // byte is a typedef by Crypto++ for unsigned char.
    byte key[m_szEncKey.size()],iv[CryptoPP::AES::BLOCKSIZE];
    
    for(unsigned int i=0;i<m_szEncKey.size();i++) {
        key[i] = m_szEncKey[i];
    }

    // The following is absolute shit, but the module we replace does it like
    // that, so we hardcode the IV to 0 in the required length...
    for(unsigned int i=0;i<sizeof(iv);i++) {
            iv[i] = 0x00;
    }
    
    // We hardcode CBC mode here, as the embedded code is always called in
    // CBC mode too (see above, line 148), no need to use the external
    // cipher object.
    // The padding mode is enforced to be
    // CryptoPP::BlockPaddingSchemeDef::ZEROS_PADDING, as the embedded code
    // does only that.
    try {
        CryptoPP::CBC_Mode< CryptoPP::AES >::Encryption encryptor( key, sizeof(key), iv );
        
        if(getEncoding() == KVI_RIJNDAEL_HEX) {
            CryptoPP::StringSource(static_cast<std::string>(plainText), true,
                                    new CryptoPP::StreamTransformationFilter(
                                    encryptor, new CryptoPP::HexEncoder(
                                    new CryptoPP::StringSink( cipher )
                                    ), CryptoPP::BlockPaddingSchemeDef::ZEROS_PADDING
                                    )
                                    );
        }
        else if(getEncoding() == KVI_RIJNDAEL_B64) {
            CryptoPP::StringSource(static_cast<std::string>(plainText), true,
                                    new CryptoPP::StreamTransformationFilter(
                                    encryptor, new CryptoPP::Base64Encoder(
                                    new CryptoPP::StringSink( cipher )
                                    ), CryptoPP::BlockPaddingSchemeDef::ZEROS_PADDING
                                    )
                                    );
        }
        else {
            setLastError(__tr("Unable to determine output encoding (Hex or Base64)."));
            return KviCryptEngine::EncryptError;
        }
    }
    catch(CryptoPP::Exception e) {
        QString staticErrTxt =
        __tr("Crypto++ threw the following exception: ");
        setLastError(staticErrTxt.append(QString(e.what())));
        return KviCryptEngine::EncryptError;
    }
   
    outBuffer = cipher.c_str();
    outBuffer.prepend(KVI_TEXT_CRYPTESCAPE);
    return KviCryptEngine::Encrypted;
}

KviCryptEngine::DecryptResult KviRijndaelEngine::decrypt(const char * inBuffer,KviStr &plainText)
{
    std::string plain;
    byte key[m_szDecKey.size()],iv[CryptoPP::AES::BLOCKSIZE];
    std::string szIn = inBuffer;
    
    for(unsigned int i=0;i<m_szDecKey.size();i++) {
        key[i] = m_szDecKey[i];
    }
    
    // The following is absolute shit, but the module we replace does it like
    // that, so we hardcode the IV to 0 in the required length...
    for(unsigned int i=0;i<sizeof(iv);i++) {
        iv[i] = 0x00;
    }

    if(static_cast<int>(szIn[0]) != KVI_TEXT_CRYPTESCAPE)
    {
        plainText = inBuffer;
        return KviCryptEngine::DecryptOkWasPlainText;
    }

    szIn = szIn.erase(0,1);
    
    if(szIn.empty())
    {
        plainText = inBuffer;
        return KviCryptEngine::DecryptOkWasPlainText; // empty buffer
    }
    
    try {
        CryptoPP::CBC_Mode< CryptoPP::AES >::Decryption decryptor( key, sizeof(key), iv );
        
        if(getEncoding() == KVI_RIJNDAEL_HEX) {
            CryptoPP::StringSource(szIn, true,
                                    new CryptoPP::HexDecoder(
                                    new CryptoPP::StreamTransformationFilter(
                                    decryptor, new CryptoPP::StringSink( plain ),
                                    CryptoPP::BlockPaddingSchemeDef::ZEROS_PADDING )
                                    )
                                    );
        }
        else if(getEncoding() == KVI_RIJNDAEL_B64) {
            CryptoPP::StringSource(szIn, true,
                                    new CryptoPP::Base64Decoder(
                                    new CryptoPP::StreamTransformationFilter(
                                    decryptor, new CryptoPP::StringSink( plain ),
                                    CryptoPP::BlockPaddingSchemeDef::ZEROS_PADDING )
                                    )
                                    );
        }
        else {
            setLastError(__tr("Unable to determine input encoding (Hex or Base64)."));
            return KviCryptEngine::DecryptError;
        }
    }
    catch(CryptoPP::Exception e) {
        QString staticErrTxt =
        __tr("Crypto++ threw the following exception: ");
        setLastError(staticErrTxt.append(QString(e.what())));
        return KviCryptEngine::DecryptError;
    }

    plainText = plain.c_str();
    return KviCryptEngine::DecryptOkWasEncrypted;
}

// The following is needed in either case
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

// Again a compatible (to the embedded version) implementation of the
// KviCryptEngine, this time for Blowfish.
KviMircryptionEngine::KviMircryptionEngine()
: KviCryptEngine()
{
    g_pEngineList->append(this);
    m_szEncKey.clear();
    m_szDecKey.clear();
}

KviMircryptionEngine::~KviMircryptionEngine()
{
    g_pEngineList->removeRef(this);
    m_szEncKey.clear();
    m_szDecKey.clear();
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
            setLastError(__tr("Missing encryption and decryption key: at least one is needed"));
            return false;
        }
    }
    
    m_szEncKey = std::string(encKey,encKeyLen);
    m_szDecKey = std::string(decKey,decKeyLen);
    if((m_szEncKey.find("cbc:",0,4) != std::string::npos) && (m_szEncKey.size() > 4)) {
        m_szEncKey = m_szEncKey.substr(4);
        m_bEncryptCBC = true;
    }
    else {
        m_bEncryptCBC = false;
    }
    
    if((m_szDecKey.find("cbc:",0,4) != std::string::npos) && (m_szDecKey.size() > 4)) {
        m_szDecKey = m_szDecKey.substr(4);
        m_bDecryptCBC = true;
    }
    else {
        m_bDecryptCBC = false;
    }
    
    return true;
}

KviCryptEngine::EncryptResult KviMircryptionEngine::encrypt(const char * plainText,KviStr &outBuffer)
{
    std::string cipher;
    byte key[m_szEncKey.size()], iv[CryptoPP::Blowfish::BLOCKSIZE];
    
    for(unsigned int i=0;i<m_szEncKey.size();i++) {
        key[i] = m_szEncKey.at(i);
    }
    
    if(m_bEncryptCBC)
    {
        try {
            for(unsigned int i=0;i<sizeof(iv);i++) {
                iv[i] = 0x00;
            }

            CryptoPP::CBC_Mode< CryptoPP::Blowfish >::Encryption encryptor( key, sizeof(key), iv );
            CryptoPP::StringSource(static_cast<std::string>(plainText), true,
                                    new CryptoPP::StreamTransformationFilter(
                                    encryptor, new CryptoPP::Base64Encoder(
                                    new CryptoPP::StringSink( cipher )
                                    ), CryptoPP::BlockPaddingSchemeDef::ZEROS_PADDING
                                    )
                                    );
        }
        catch(CryptoPP::Exception e) {
            QString staticErrTxt =
            __tr("Crypto++ threw the following exception: ");
            setLastError(staticErrTxt.append(QString(e.what())));
            return KviCryptEngine::EncryptError;
        }
    } else {
        try {
            CryptoPP::ECB_Mode< CryptoPP::Blowfish >::Encryption encryptor( key, sizeof(key) );
            CryptoPP::StringSource(static_cast<std::string>(plainText), true,
                                    new CryptoPP::StreamTransformationFilter(
                                    encryptor, new CryptoPP::Base64Encoder(
                                    new CryptoPP::StringSink( cipher )
                                    ), CryptoPP::BlockPaddingSchemeDef::ZEROS_PADDING
                                    )
                                    );
        }
        catch(CryptoPP::Exception e) {
            QString staticErrTxt =
            __tr("Crypto++ threw the following exception: ");
            setLastError(staticErrTxt.append(QString(e.what())));
            return KviCryptEngine::EncryptError;
        }
    }
    
    outBuffer = "+OK ";
    outBuffer.append(cipher.c_str());
    
    return KviCryptEngine::Encrypted;
}

KviCryptEngine::DecryptResult KviMircryptionEngine::decrypt(const char * inBuffer,KviStr &plainText)
{
    std::string plain;
    std::string szIn = inBuffer;
    byte key[m_szDecKey.size()], iv[CryptoPP::Blowfish::BLOCKSIZE];
    
    // various old versions
    if(szIn.find("mcps ",0,5) != std::string::npos) {
        szIn = szIn.substr(5);
    }
    else if(szIn.find("+OK ",0,4) != std::string::npos) {
        szIn = szIn.substr(4);
    }
    // some servers seem to strip the + at the beginning...
    else if(szIn.find("OK ",0,3) != std::string::npos) {
        szIn = szIn.substr(3);
    }
    else {
        plainText = szIn.c_str();
        return KviCryptEngine::DecryptOkWasPlainText;
    }
    
    for(unsigned int i=0;i<m_szDecKey.size();i++) {
        key[i] = m_szDecKey.at(i);
    }
    
    if(m_bDecryptCBC)
    {
        try {
            for(unsigned int i=0;i<sizeof(iv);i++) {
                iv[i] = 0x00;
            }

            CryptoPP::CBC_Mode< CryptoPP::Blowfish >::Decryption decryptor( key, sizeof(key), iv );
            CryptoPP::StringSource(szIn, true,
                            new CryptoPP::Base64Decoder(
                                new CryptoPP::StreamTransformationFilter(
                                    decryptor,
                                    new CryptoPP::StringSink( plain ),
                                    CryptoPP::BlockPaddingSchemeDef::ZEROS_PADDING
                                    )
                                )
                            );
        }
        catch(CryptoPP::Exception e) {
            QString staticErrTxt =
            __tr("Crypto++ threw the following exception: ");
            setLastError(staticErrTxt.append(QString(e.what())));
            return KviCryptEngine::DecryptError;
        }
    } else {
        try {
            CryptoPP::ECB_Mode< CryptoPP::Blowfish >::Decryption decryptor( key, sizeof(key) );
            CryptoPP::StringSource(szIn, true,
                            new CryptoPP::Base64Decoder(
                                new CryptoPP::StreamTransformationFilter(
                                    decryptor, new CryptoPP::StringSink( plain ),
                                    CryptoPP::BlockPaddingSchemeDef::ZEROS_PADDING )
                                )
                            );
        }
        catch(CryptoPP::Exception e) {
            QString staticErrTxt =
            __tr("Crypto++ threw the following exception: ");
            setLastError(staticErrTxt.append(QString(e.what())));
            return KviCryptEngine::DecryptError;
        }
    }
    
    plainText = plain.c_str();
    return KviCryptEngine::DecryptOkWasEncrypted;
}

static KviCryptEngine * allocMircryptionEngine()
{
    return new KviMircryptionEngine();
}

// =======================================
// module routines
// =======================================
static bool rijndael_module_init(KviModule * m)
{
    g_pEngineList = new KviPointerList<KviCryptEngine>;
    g_pEngineList->setAutoDelete(false);
    
    KviStr format = __tr("Cryptographic engine based on the\n" \
    "Advanced Encryption Standard (AES)\n" \
    "algorithm called Rijndael.\n" \
    "The text is first encrypted with Rijndael\n" \
    "and then converted to %s notation.\n" \
    "The keys used are %d bit long and will be padded\n" \
    "with zeros if you provide shorter ones.\n" \
    "If only one key is provided, this engine\n" \
    "will use it for both encryption and decryption.\n\n" \
    "This is the Crypto++ implementation which could do\n" \
    "a lot more than the current default engine, but as\n" \
    "this should be a drop-in replacement, it acts just\n" \
    "like the original one.\n" \
    "Information about the algorithm can be found at\n" \
    "<http://www.cryptolounge.org/wiki/AES>\n");
    
    // FIXME: Maybe convert this repeated code to a function eh ?
    
    KviCryptEngineDescription * d = new KviCryptEngineDescription;
    d->szName = "Rijndael128Hex";
    d->szAuthor = "Kai Wasserbäch";
    d->szDescription.sprintf(format.ptr(),__tr("hexadecimal"),128);
    d->iFlags = KVI_CRYPTENGINE_CAN_ENCRYPT | KVI_CRYPTENGINE_CAN_DECRYPT |
    KVI_CRYPTENGINE_WANT_ENCRYPT_KEY | KVI_CRYPTENGINE_WANT_DECRYPT_KEY;
    d->allocFunc = allocRijndael128HexEngine;
    d->deallocFunc = deallocRijndaelCryptEngine;
    m->registerCryptEngine(d);
    
    d = new KviCryptEngineDescription;
    d->szName = "Rijndael192Hex";
    d->szAuthor = "Kai Wasserbäch";
    d->szDescription.sprintf(format.ptr(),__tr("hexadecimal"),192);
    d->iFlags = KVI_CRYPTENGINE_CAN_ENCRYPT | KVI_CRYPTENGINE_CAN_DECRYPT |
    KVI_CRYPTENGINE_WANT_ENCRYPT_KEY | KVI_CRYPTENGINE_WANT_DECRYPT_KEY;
    d->allocFunc = allocRijndael192HexEngine;
    d->deallocFunc = deallocRijndaelCryptEngine;
    m->registerCryptEngine(d);
    
    d = new KviCryptEngineDescription;
    d->szName = "Rijndael256Hex";
    d->szAuthor = "Kai Wasserbäch";
    d->szDescription.sprintf(format.ptr(),__tr("hexadecimal"),256);
    d->iFlags = KVI_CRYPTENGINE_CAN_ENCRYPT | KVI_CRYPTENGINE_CAN_DECRYPT |
    KVI_CRYPTENGINE_WANT_ENCRYPT_KEY | KVI_CRYPTENGINE_WANT_DECRYPT_KEY;
    d->allocFunc = allocRijndael256HexEngine;
    d->deallocFunc = deallocRijndaelCryptEngine;
    m->registerCryptEngine(d);
    
    d = new KviCryptEngineDescription;
    d->szName = "Rijndael128Base64";
    d->szAuthor = "Kai Wasserbäch";
    d->szDescription.sprintf(format.ptr(),__tr("base64"),128);
    d->iFlags = KVI_CRYPTENGINE_CAN_ENCRYPT | KVI_CRYPTENGINE_CAN_DECRYPT |
    KVI_CRYPTENGINE_WANT_ENCRYPT_KEY | KVI_CRYPTENGINE_WANT_DECRYPT_KEY;
    d->allocFunc = allocRijndael128Base64Engine;
    d->deallocFunc = deallocRijndaelCryptEngine;
    m->registerCryptEngine(d);
    
    d = new KviCryptEngineDescription;
    d->szName = "Rijndael192Base64";
    d->szAuthor = "Kai Wasserbäch";
    d->szDescription.sprintf(format.ptr(),__tr("base64"),192);
    d->iFlags = KVI_CRYPTENGINE_CAN_ENCRYPT | KVI_CRYPTENGINE_CAN_DECRYPT |
    KVI_CRYPTENGINE_WANT_ENCRYPT_KEY | KVI_CRYPTENGINE_WANT_DECRYPT_KEY;
    d->allocFunc = allocRijndael192Base64Engine;
    d->deallocFunc = deallocRijndaelCryptEngine;
    m->registerCryptEngine(d);
    
    d = new KviCryptEngineDescription;
    d->szName = "Rijndael256Base64";
    d->szAuthor = "Kai Wasserbäch";
    d->szDescription.sprintf(format.ptr(),__tr("base64"),256);
    d->iFlags = KVI_CRYPTENGINE_CAN_ENCRYPT | KVI_CRYPTENGINE_CAN_DECRYPT |
    KVI_CRYPTENGINE_WANT_ENCRYPT_KEY | KVI_CRYPTENGINE_WANT_DECRYPT_KEY;
    d->allocFunc = allocRijndael256Base64Engine;
    d->deallocFunc = deallocRijndaelCryptEngine;
    m->registerCryptEngine(d);
    
    
    d = new KviCryptEngineDescription;
    d->szName = "Mircryption";
    d->szAuthor = "Kai Wasserbäch";
    d->szDescription = __tr("Popular cryptographic engine based on the\n" \
    "old Blowfish encryption algorithm.\n" \
    "The text is first encrypted with Blowfish \n" \
    "and then base64 encoded.\n" \
    "Keys of variable length can be used and\n" \
    "are specified as character strings.\n" \
    "Keys can be up to 56 bytes (448 bits) long.\n" \
    "If only one key is provided, this engine\n" \
    "will use it for encryption and decryption.\n" \
    "This engine works in ECB mode by default,\n" \
    "if you want to use CBC mode you must prefix\n" \
    "your key(s) with \"cbc:\".\n\n" \
    "This is the Crypto++ based version, not relying on embedded code\n");
    d->iFlags = KVI_CRYPTENGINE_CAN_ENCRYPT | KVI_CRYPTENGINE_CAN_DECRYPT |
    KVI_CRYPTENGINE_WANT_ENCRYPT_KEY | KVI_CRYPTENGINE_WANT_DECRYPT_KEY;
    d->allocFunc = allocMircryptionEngine;
    d->deallocFunc = deallocRijndaelCryptEngine;
    m->registerCryptEngine(d);
    
    
    return true;
}

static bool rijndael_module_cleanup(KviModule *m)
{
    while(g_pEngineList->first())delete g_pEngineList->first();
    delete g_pEngineList;
    g_pEngineList = 0;
    m->unregisterCryptEngines();
    return true;
}

static bool rijndael_module_can_unload(KviModule *)
{
    return g_pEngineList->isEmpty();
}

// =======================================
// plugin definition structure
// =======================================
KVIRC_MODULE(
"Rijndael crypt engine",
             "4.0.1",
             "Copyright ©      2009 Kai Wasserbäch <debian@carbon-project.org>" \
             "          © 2000-2008 Szymon Stefanek <pragma at kvirc dot net>" ,
             "Exports the rijndael crypt engine",
             rijndael_module_init ,
             rijndael_module_can_unload,
             0,
             rijndael_module_cleanup,
	     0
             )
