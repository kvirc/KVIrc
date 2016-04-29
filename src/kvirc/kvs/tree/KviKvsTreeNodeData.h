#ifndef _KVI_KVS_TREENODE_DATA_H_
#define _KVI_KVS_TREENODE_DATA_H_
//=============================================================================
//
//   File : KviKvsTreeNodeData.h
//   Creation date : Sun 05 Oct 2003 21.42 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviKvsTreeNodeBase.h"
#include "KviKvsVariant.h"
#include "KviKvsRunTimeContext.h"
#include "KviKvsRWEvaluationResult.h"

class KviKvsObject;

class KVIRC_API KviKvsTreeNodeData : public KviKvsTreeNode
{
public:
	KviKvsTreeNodeData(const QChar * pLocation);
	~KviKvsTreeNodeData();

protected:
	const QChar * m_pEndingLocation; // note that this MIGHT be not set (it's set for sure for anything parsed by parseCommaSeparatedParameter()
public:
	void setEndingLocation(const QChar * pEndingLocation) { m_pEndingLocation = pEndingLocation; };
	const QChar * endingLocation() { return m_pEndingLocation; };

	virtual bool evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer);
	virtual KviKvsRWEvaluationResult * evaluateReadWrite(KviKvsRunTimeContext * c); // error by default

	virtual bool evaluateReadOnlyInObjectScope(KviKvsObject * o, KviKvsRunTimeContext * c, KviKvsVariant * pBuffer); // error by default
	virtual KviKvsRWEvaluationResult * evaluateReadWriteInObjectScope(KviKvsObject * o, KviKvsRunTimeContext * c);   // error by default

	virtual void contextDescription(QString & szBuffer);
	virtual void dump(const char * prefix);

	virtual bool isReadOnly();                   // true by default
	virtual bool canEvaluateToObjectReference(); // no by default
	virtual bool isFunctionCall();               // no by default
	virtual bool canEvaluateInObjectScope();     // no by default

	virtual bool convertStringConstantToNumeric(); // this does nothing by default and is reimplemented only by KviKvsTreeNodeConstantData
};

#endif //!_KVI_KVS_TREENODE_DATA_H_
