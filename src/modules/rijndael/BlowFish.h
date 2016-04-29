#ifndef _BLOWFISH_H_
#define _BLOWFISH_H_
//=============================================================================
//
//   File : BlowFish.h
//   Creation date : Wed Jan 13 2005 02:04:10 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2010 Szymon Stefanek (pragma at kvirc dot net)
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

//
// This code is adapted from the MircCryption engine suite by Dark Raichu
// Original comments follow.
//

//---------------------------------------------------------------------------
//  The implementation of blowfish for CBC mode is from
//    http://www.thecodeproject.com/cpp/blowfish.asp by George Anescu
//  I removed exception handling, so it is callers responsibility to insure
//  that strings are length multiples of 8 bytes
//  -mouser 1/08/05
//---------------------------------------------------------------------------

//
//    BLOWFISH ENCRYPTION ALGORITHM
//
//    Implementation of Bruce Schneier's BLOWFISH algorithm from "Applied
//    Cryptography", Second Edition.
//
//    Encryption and Decryption of Byte Strings using the Blowfish Encryption Algorithm.
//    Blowfish is a block cipher that encrypts data in 8-byte blocks. The algorithm consists
//    of two parts: a key-expansion part and a data-ancryption part. Key expansion converts a
//    variable key of at least 1 and at most 56 bytes into several subkey arrays totaling
//    4168 bytes. Blowfish has 16 rounds. Each round consists of a key-dependent permutation,
//    and a key and data-dependent substitution. All operations are XORs and additions on 32-bit words.
//    The only additional operations are four indexed array data lookups per round.
//    Blowfish uses a large number of subkeys. These keys must be precomputed before any data
//    encryption or decryption. The P-array consists of 18 32-bit subkeys: P0, P1,...,P17.
//    There are also four 32-bit S-boxes with 256 entries each: S0,0, S0,1,...,S0,255;
//    S1,0, S1,1,...,S1,255; S2,0, S2,1,...,S2,255; S3,0, S3,1,...,S3,255;
//
//    The Electronic Code Book (ECB), Cipher Block Chaining (CBC) and Cipher Feedback modes
//    are used:
//
//    In ECB mode if the same block is encrypted twice with the same key, the resulting
//    ciphertext blocks are the same.
//
//    In CBC Mode a ciphertext block is obtained by first xoring the
//    plaintext block with the previous ciphertext block, and encrypting the resulting value.
//
//    In CFB mode a ciphertext block is obtained by encrypting the previous ciphertext block
//    and xoring the resulting value with the plaintext
//
//    The previous ciphertext block is usually stored in an Initialization Vector (IV).
//    An Initialization Vector of zero is commonly used for the first block, though other
//    arrangements are also in use.
//

#include "kvi_settings.h"

#if defined(COMPILE_CRYPT_SUPPORT) || defined(Q_MOC_RUN)

//Block Structure
struct SBlock
{
	//Constructors
	SBlock(unsigned int l = 0, unsigned int r = 0) : m_uil(l), m_uir(r) {}
	//Copy Constructor
	SBlock(const SBlock & roBlock) : m_uil(roBlock.m_uil), m_uir(roBlock.m_uir) {}
	SBlock & operator^=(SBlock & b)
	{
		m_uil ^= b.m_uil;
		m_uir ^= b.m_uir;
		return *this;
	}
	unsigned int m_uil, m_uir;
};

class BlowFish
{
public:
	enum
	{
		ECB = 0,
		CBC = 1,
		CFB = 2
	};

	//Constructor - Initialize the P and S boxes for a given Key
	BlowFish(unsigned char * ucKey, unsigned int n, const SBlock & roChain = SBlock(0UL, 0UL));

	//Resetting the chaining block
	void ResetChain() { m_oChain = m_oChain0; }

	// Encrypt/Decrypt Buffer in Place
	void Encrypt(unsigned char * buf, unsigned int n, int iMode = ECB);
	void Decrypt(unsigned char * buf, unsigned int n, int iMode = ECB);

	// Encrypt/Decrypt from Input Buffer to Output Buffer
	void Encrypt(const unsigned char * in, unsigned char * out, unsigned int n, int iMode = ECB);
	void Decrypt(const unsigned char * in, unsigned char * out, unsigned int n, int iMode = ECB);

	//Private Functions
private:
	unsigned int F(unsigned int ui);
	void Encrypt(SBlock &);
	void Decrypt(SBlock &);

private:
	//The Initialization Vector, by default {0, 0}
	SBlock m_oChain0;
	SBlock m_oChain;
	unsigned int m_auiP[18];
	unsigned int m_auiS[4][256];
	static const unsigned int scm_auiInitP[18];
	static const unsigned int scm_auiInitS[4][256];
};

//Extract low order byte
inline unsigned char Byte(unsigned int ui)
{
	return (unsigned char)(ui & 0xff);
}

//Function F
inline unsigned int BlowFish::F(unsigned int ui)
{
	return ((m_auiS[0][Byte(ui >> 24)] + m_auiS[1][Byte(ui >> 16)]) ^ m_auiS[2][Byte(ui >> 8)]) + m_auiS[3][Byte(ui)];
}

#endif //COMPILE_CRYPT_SUPPORT

#endif //!_BLOWFISH_H_
