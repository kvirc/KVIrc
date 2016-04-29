#ifndef _KVI_KVS_RWEVALUATIONRESULT_H_
#define _KVI_KVS_RWEVALUATIONRESULT_H_
//=============================================================================
//
//   File : KviKvsRWEvaluationResult.h
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

#include "kvi_settings.h"
#include "KviKvsVariant.h"

class KVIRC_API KviKvsRWEvaluationResult
{
public:
	// this result becomes the owner of the parent result!!!!
	KviKvsRWEvaluationResult(KviKvsRWEvaluationResult * pParent, KviKvsVariant * pVariant);
	// the destructor must do (in order):
	//    check if variant is empty and if it is then remove it from the container (destroy it)
	//    delete the m_pParent result
	virtual ~KviKvsRWEvaluationResult();

protected:
	KviKvsRWEvaluationResult * m_pParent;
	KviKvsVariant * m_pVariant;

public:
	KviKvsVariant * result() { return m_pVariant; };
};

class KviKvsArray;

class KVIRC_API KviKvsArrayElement : public KviKvsRWEvaluationResult
{
public:
	KviKvsArrayElement(KviKvsRWEvaluationResult * pParent, KviKvsVariant * pVariant, KviKvsArray * pArray, int iKey);
	~KviKvsArrayElement();

protected:
	KviKvsArray * m_pArray;
	int m_iKey;
};

class KviKvsHash;

class KVIRC_API KviKvsHashElement : public KviKvsRWEvaluationResult
{
public:
	KviKvsHashElement(KviKvsRWEvaluationResult * pParent, KviKvsVariant * pVariant, KviKvsHash * pHash, const QString & szKey);
	~KviKvsHashElement();

protected:
	KviKvsHash * m_pHash;
	QString m_szKey;
};

#endif //!_KVI_KVS_RWEVALUATIONRESULT_H_
