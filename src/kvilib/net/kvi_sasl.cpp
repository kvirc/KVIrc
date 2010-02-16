//=============================================================================
//
//   File : kvi_sasl.cpp
//   Creation date : Mon Feb 14 2010 19:36:12 CEST by Fabio Bas
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2010 Fabio Bas (ctrlaltca at gmail dot com)
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



#include "kvi_sasl.h"

#include "kvi_malloc.h"

#ifdef COMPILE_SSL_SUPPORT

#include <openssl/blowfish.h>
#include <openssl/bn.h>
#include <openssl/dh.h>
#include <openssl/err.h>
#include <kvi_netutils.h>

#endif


namespace KviSASL
{

	bool plainMethod(KviStr & szIn, KviStr & szOut, QByteArray & baNick, QByteArray & baPass)
	{
		if(szIn=="+")
		{
			int answerLen = 3 + (2 * baNick.size()) + baPass.size();
			char * answer = (char *) kvi_malloc(answerLen);
			char * answer2 = answer;

			memcpy(answer, baNick.data(), baNick.size());
			answer+=baNick.size();
			memset(answer, 0, 1);
			answer++;

			memcpy(answer, baNick.data(), baNick.size());
			answer+=baNick.size();
			memset(answer, 0, 1);
			answer++;

			memcpy(answer, baPass.data(), baPass.size());
			answer+=baPass.size();
			memset(answer, 0, 1);
			answer++;

			szOut.bufferToBase64(answer2,answerLen);
			kvi_free(answer2);

			return true;
		}
		return false;
	}

#ifdef COMPILE_SSL_SUPPORT
	bool dh_blowfishMethod(KviStr & szIn, KviStr & szOut, QByteArray & baNick, QByteArray & baPass)
	{
		/*
		 * The format of the auth token is quite complex; the server sends us 3 strings:
		 * p - a prime number
		 * g - a generator number, tipically 2 or 5 are used.
		 * y - the server-generated public key
		 * These 3 strings are null-terminated and codified as pascal strings (they are prefixed
		 * with a 16-bit lenght identifiedr in "network" big-endian order)
		 * Then, the 3 strings are concatenated and base-64 encoded.
		 */

		BF_KEY key;
		quint16 size, pKlen;
		int secretLen;
		unsigned char *secret = NULL, *pubKey = NULL;
		char * tmpBuf;
		DH * dh = DH_new();
		int len = szIn.base64ToBuffer(&tmpBuf,false);

		if(len < 7) return false;

		// extract p
		size = ntohs(*(unsigned int*)tmpBuf);
		tmpBuf+=2;
		len-=2;
		if(size > len) return false;

		if(!(dh->p = BN_bin2bn((unsigned char*) tmpBuf, size, NULL)))
			return false;

		tmpBuf+=size;
		len-=size;

		// extract g
		size = ntohs(*(unsigned int*)tmpBuf);
		tmpBuf+=2;
		len-=2;
		if(size > len) return false;

		if(!(dh->g = BN_bin2bn((unsigned char*) tmpBuf, size, NULL)))
			return false;

		tmpBuf+=size;
		len-=size;

		// extract y
		size = ntohs(*(unsigned int*)tmpBuf);
		tmpBuf+=2;
		len-=2;
		if(size > len) return false;

		// create our keys and extract shared secret
		// note: any memory checking tool (as valgrind) will complain on this call. blame openssl
		if(!DH_generate_key(dh))
			return false;

		secret=(unsigned char *) kvi_malloc(DH_size(dh));
		// note: any memory checking tool (as valgrind) will complain on this call. blame openssl
		if(-1 == (secretLen = DH_compute_key(secret, BN_bin2bn((unsigned char*) tmpBuf, size, NULL), dh)))
			return false;

		pKlen=BN_num_bytes(dh->pub_key);
		pubKey = (unsigned char *) kvi_malloc(pKlen);
		BN_bn2bin(dh->pub_key, pubKey);

		//create crypto buffers
		int passLen = baPass.size() + ((8 -( baPass.size() % 8)) % 8);
		int passC = 0;
		unsigned char *passIn = (unsigned char *) kvi_malloc(passLen);
		unsigned char *passOut = (unsigned char *) kvi_malloc(passLen);

		memset(passIn, 0, passLen);
		memset(passOut, 0, passLen);
		memcpy(passIn, baPass.data(), baPass.size());

		// crypt our password
		BF_set_key(&key, secretLen, secret);

		for (passC=0; passC < passLen; passC += 8)
			BF_ecb_encrypt(passIn + passC,  passOut + passC, &key, BF_ENCRYPT);

		/*
		 * Build up the answer
		 * The format of the auth answer is quite complex, and formed by the byte concatenation of:
		 * 1) a 16 bit integer containing the byte length of our public key
		 * 2) our public key
		 * 3) our username (nickname), null-terminated
		 * 4) our password, crypted using blowfish in ecb mode and the dh secret as the blowfish key
		 * Then, the answer is to be base64 encoded.
		 */
		
		int answerLen = 2 + pKlen + baNick.size() + 1 + passLen;
		char * answer = (char *) malloc(answerLen);
		char * answer2 = answer;
		*((unsigned int *)answer) = htons(pKlen);
		answer+=2;
		memcpy(answer, pubKey, pKlen);
		answer+=pKlen;
		memcpy(answer, baNick.data(), baNick.size());
		answer+=baNick.size();
		memset(answer, 0, 1);
		answer++;
		memcpy(answer, passOut, passLen);
		szOut.bufferToBase64(answer2,answerLen);

		//clean up
		kvi_free(secret);
		kvi_free(pubKey);
		kvi_free(passIn);
		kvi_free(passOut);

		return true;
	}
#else
	bool dh_blowfishMethod(KviStr & szIn, KviStr & szOut, QByteArray & baNick, QByteArray & baPass)
	{
		return false;
	}
#endif

};
