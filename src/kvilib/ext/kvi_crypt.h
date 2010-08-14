#ifndef _KVI_CRYPT_H_
#define _KVI_CRYPT_H_

//=============================================================================
//
//   File : kvi_crypt.h
//   Creation date : Fri Nov 03 2000 01:45:21 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
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


//
// Base class for all IRC crypt engines
// These intend to encrypt plain text into something
// that can be sent through the IRC protocol...
// so it should not contain NULL, CR , LF and other
// similar stuff...
//


#include "kvi_qstring.h"
#include "kvi_string.h"
#include "kvi_heapobject.h"
#include "kvi_pointerhashtable.h"

#include <QObject>

#ifdef COMPILE_CRYPT_SUPPORT
	class KviCryptEngine;

	typedef KviCryptEngine * (*crypt_engine_allocator_func)();
	typedef void (*crypt_engine_deallocator_func)(KviCryptEngine *);
#endif //COMPILE_CRYPT_SUPPORT

	// we must include this declaration to make moc happy even
	// if we're not compiling the crypt support

	class KVILIB_API KviCryptEngine : public QObject, public KviHeapObject
	{
		Q_OBJECT
		friend class KviCryptEngineManager;
	public:
		KviCryptEngine();
		virtual ~KviCryptEngine();

#ifdef COMPILE_CRYPT_SUPPORT
	private:
		crypt_engine_deallocator_func m_deallocFunc;    // this is accessed by KviCryptEngineManager only
		QString                       m_szLastError;
		int                           m_iMaxEncryptLen;
	public:
		void setMaxEncryptLen(int m){ m_iMaxEncryptLen = m; };
		int maxEncryptLen(){ return m_iMaxEncryptLen; };
		virtual bool init(const char * encKey,int encKeyLen,const char * decKey,int decKeyLen);
		//
		// Encrypts utf8 plainText and returns the encrypted
		// data in outBuffer. The encrypted data must be
		// suitable for sending thru an IRC (eventually DCC
		// that is less restrictive) connection and must be utf8 encoded: so
		// no NULL, CR and LF in the output.
		// 0x01 should be also avoided since
		// it is the CTCP delimiter.
		// Converting the result in a HEX string
		// is a good trick...also Base64 could be used.
		// Should return false in case of an error.
		// Theoretically we could allow NULLs in plainText
		// but this is not the case of KVIrc.
		//
		enum EncryptResult { Encrypted, Encoded, EncryptError };
		virtual EncryptResult encrypt(const char * plainText,KviStr &outBuffer);
		//
		// Decrypts the utf8 data in inBuffer and puts the decrypted utf8
		// stuff in plainText. inBuffer is the thingie
		// that we got from outBuffer of encrupt() so it
		// follows the same rules.
		// Should return false in case of error.
		//
		enum DecryptResult { DecryptOkWasEncrypted, DecryptOkWasEncoded, DecryptOkWasPlainText, DecryptError };
		virtual DecryptResult decrypt(const char * inBuffer,KviStr &plainText);
		//
		// Returns the string containing the description
		// of the last error or an empty string if there
		// was no error after the last init() call.
		//
		const QString &lastError(){ return m_szLastError; };
	protected:
		//
		// The following two should have clear meaning
		//
		void clearLastError(){ m_szLastError = ""; };
		void setLastError(const QString &err){ m_szLastError = err; };
#endif //COMPILE_CRYPT_SUPPORT
	};

#ifdef COMPILE_CRYPT_SUPPORT
	#define KVI_CRYPTENGINE_CAN_ENCRYPT 1
	#define KVI_CRYPTENGINE_CAN_DECRYPT 2
	#define KVI_CRYPTENGINE_WANT_ENCRYPT_KEY 4
	#define KVI_CRYPTENGINE_WANT_DECRYPT_KEY 8

	class KVILIB_API KviCryptEngineDescription : public KviHeapObject
	{
	public:
		KviCryptEngineDescription(){};
		virtual ~KviCryptEngineDescription(){};
	public:
		QString                       m_szName;           // engine name
		QString                       m_szDescription;    // details
		QString                       m_szAuthor;         // algorithm author
		int                           m_iFlags;           // properties
		crypt_engine_allocator_func   m_allocFunc;        // engine allocator
		crypt_engine_deallocator_func m_deallocFunc;      // deallocation function (if called from outside the origin module)
		void                        * m_providerHandle;   // used to identify the provider module
	};


	class KVILIB_API KviCryptEngineManager
	{
	public:
		KviCryptEngineManager();
		virtual ~KviCryptEngineManager();
	private:
		KviPointerHashTable<QString,KviCryptEngineDescription> * m_pEngineDict;
	public:
		const KviPointerHashTable<QString,KviCryptEngineDescription> * engineDict(){ return m_pEngineDict; };
		void registerEngine(KviCryptEngineDescription * d);
		void unregisterEngine(const QString &szName);
		void unregisterEngines(void * providerHandle);
		//
		// Allocates a crypt engine
		// Please note that the engine may be deleted from outside
		// so you'd better connect the "destroyed" signal
		//
		KviCryptEngine * allocateEngine(const QString &szName);
		void deallocateEngine(KviCryptEngine * e);
	};

#endif //COMPILE_CRYPT_SUPPORT


#endif //!_KVI_CRYPT_H_
