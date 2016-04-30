//=============================================================================
//
//   File : KviKvsRWEvaluationResult.cpp
//   Creation date : Thu 11 Dec 2003 22:51:46 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviKvsRWEvaluationResult.h"
#include "KviKvsHash.h"
#include "KviKvsArray.h"

KviKvsRWEvaluationResult::KviKvsRWEvaluationResult(KviKvsRWEvaluationResult * pParent, KviKvsVariant * pVariant)
{
	m_pParent = pParent;
	m_pVariant = pVariant;
}

KviKvsRWEvaluationResult::~KviKvsRWEvaluationResult()
    = default;

KviKvsArrayElement::KviKvsArrayElement(KviKvsRWEvaluationResult * pParent, KviKvsVariant * pVariant, KviKvsArray * pArray, int iKey)
    : KviKvsRWEvaluationResult(pParent, pVariant)
{
	m_pArray = pArray;
	m_iKey = iKey;
}

KviKvsArrayElement::~KviKvsArrayElement()
{
	if(m_pVariant->isEmpty())
		m_pArray->unset(m_iKey);
	if(m_pParent)
		delete m_pParent;
}

KviKvsHashElement::KviKvsHashElement(KviKvsRWEvaluationResult * pParent, KviKvsVariant * pVariant, KviKvsHash * pHash, const QString & szKey)
    : KviKvsRWEvaluationResult(pParent, pVariant)
{
	m_pHash = pHash;
	m_szKey = szKey;
}

KviKvsHashElement::~KviKvsHashElement()
{
	if(m_pVariant->isEmpty())
		m_pHash->unset(m_szKey);
	if(m_pParent)
		delete m_pParent;
}
