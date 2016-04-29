#ifndef _KVI_KVS_TREENODE_ARRAYREFERENCEASSERT_H_
#define _KVI_KVS_TREENODE_ARRAYREFERENCEASSERT_H_
//=============================================================================
//
//   File : KviKvsTreeNodeArrayReferenceAssert.h
//   Creation date : Thu 16 Oct 2003 23:46:12 by Szymon Stefanek
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

/**
* \file KviKvsTreeNodeArrayReferenceAssert.h
* \author Szymon Stefanek
* \brief Treenode array reference assert handling
*/

#include "kvi_settings.h"
#include "KviQString.h"
#include "KviKvsTreeNodeIndirectData.h"

class KviKvsRunTimeContext;
class KviKvsVariant;
class KviKvsRWEvaluationResult;
class KviKvsObject;

/**
* \class KviKvsTreeNodeArrayReferenceAssert
* \brief Treenode array reference assert class
*/
class KVIRC_API KviKvsTreeNodeArrayReferenceAssert : public KviKvsTreeNodeIndirectData
{
public:
	/**
	* \brief Constructs the treenode array reference assert object
	* \param pLocation The location char of the instruction
	* \param pSource The source data
	* \return KviKvsTreeNodeArrayReferenceAssert
	*/
	KviKvsTreeNodeArrayReferenceAssert(const QChar * pLocation, KviKvsTreeNodeData * pSource);

	/**
	* \brief Destroys the treenode array reference assert object
	* \return KviKvsTreeNodeArrayElement
	*/
	~KviKvsTreeNodeArrayReferenceAssert();

protected:
public:
	/**
	* \brief Returns true if the array reference is read-only
	* \return bool
	*/
	virtual bool isReadOnly();

	/**
	* \brief Sets the buffer as Array Reference Assert
	* \param szBuffer The buffer :)
	* \return void
	*/
	virtual void contextDescription(QString & szBuffer);

	/**
	* \brief Dumps the tree
	* \param prefix The prefix of the instruction
	* \return void
	*/
	virtual void dump(const char * prefix);

	/**
	* \brief Evaluates the array in read-only mode
	* \param c The context where the alias is bound to
	* \param pBuffer The data buffer
	* \return bool
	*/
	virtual bool evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer);

	/**
	* \brief Evaluates the array in read-write mode
	* \param c The context where the alias is bound to
	* \return KviKvsRWEvaluationResult *
	*/
	virtual KviKvsRWEvaluationResult * evaluateReadWrite(KviKvsRunTimeContext * c);

	/**
	* \brief Evaluates the array in read-only mode in the object scope
	* \param o The source object to evaluate
	* \param c The context where the array is bound to
	* \param pBuffer The data buffer
	* \return bool
	*/
	virtual bool evaluateReadOnlyInObjectScope(KviKvsObject * o, KviKvsRunTimeContext * c, KviKvsVariant * pBuffer);

	/**
	* \brief Evaluates the array in read-write mode in the object scope
	* \param o The source object to evaluate
	* \param c The context where the array is bound to
	* \return KviKvsRWEvaluationResult *
	*/
	virtual KviKvsRWEvaluationResult * evaluateReadWriteInObjectScope(KviKvsObject * o, KviKvsRunTimeContext * c);
};

#endif //_KVI_KVS_TREENODE_ARRAYREFERENCEASSERT_H_
