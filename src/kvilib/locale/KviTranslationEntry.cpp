//=============================================================================
//
//   File : KviTranslationEntry.cpp
//   Creation date : Wed 23 Mar 2011 03:32:12
//   Originally in KviLocale.cpp
//      Created on: Sat Jan 16 1999 19:08:41 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviTranslationEntry.h"

#include <QString>

KviTranslationEntry::KviTranslationEntry(char * pcKey, int iKeyLen, char * pcTr, int iTrLen)
    : m_szKey(pcKey, iKeyLen), m_szEncodedTranslation(pcTr, iTrLen)
{
	m_pTranslation = nullptr;
}

KviTranslationEntry::KviTranslationEntry(const char * pcKeyAndTr)
    : m_szKey(pcKeyAndTr), m_szEncodedTranslation(pcKeyAndTr)
{
	m_pTranslation = nullptr;
}

KviTranslationEntry::~KviTranslationEntry()
{
	if(m_pTranslation)
		delete m_pTranslation;
}
