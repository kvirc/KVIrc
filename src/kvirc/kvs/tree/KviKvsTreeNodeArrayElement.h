#ifndef _KVI_KVS_TREENODE_ARRAYELEMENT_H_
#define _KVI_KVS_TREENODE_ARRAYELEMENT_H_
//=============================================================================
//
//   File : KviKvsTreeNodeArrayElement.h
//   Creation date : Tue 07 Oct 2003 02:58:41 by Szymon Stefanek
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
* \file KviKvsTreeNodeArrayElement.h
* \author Szymon Stefanek
* \brief Treenode array element handling
*/

#include "kvi_settings.h"
#include "KviQString.h"
#include "KviKvsTreeNodeExpression.h"
#include "KviKvsTreeNodeArrayOrHashElement.h"
#include "KviKvsTypes.h"

class KviKvsObject;
class KviKvsRunTimeContext;

/**
* \class KviKvsTreeNodeArrayElement
* \brief Treenode array element class
*/
class KVIRC_API KviKvsTreeNodeArrayElement : public KviKvsTreeNodeArrayOrHashElement
{
public:
	/**
	* \brief Constructs the treenode array element object
	* \param pLocation The location char of the instruction
	* \param pSource The source data
	* \param pIndex The index of the expression
	* \return KviKvsTreeNodeArrayElement
	*/
	KviKvsTreeNodeArrayElement(const QChar * pLocation, KviKvsTreeNodeData * pSource, KviKvsTreeNodeExpression * pIndex);

	/**
	* \brief Destroys the treenode array element object
	*/
	~KviKvsTreeNodeArrayElement();

protected:
	KviKvsTreeNodeExpression * m_pIndex; // can't be null
public:
	/**
	* \brief Sets the buffer as Array Element Evaluation
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
	* \brief Evaluates the array element in read-only mode
	* \param c The context where the alias is bound to
	* \param pBuffer The data buffer
	* \return bool
	*/
	virtual bool evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer);

	/**
	* \brief Evaluates the array element in read-write mode
	* \param c The context where the alias is bound to
	* \return KviKvsRWEvaluationResult *
	*/
	virtual KviKvsRWEvaluationResult * evaluateReadWrite(KviKvsRunTimeContext * c);

	/**
	* \brief Evaluates the array element in read-only mode in the object scope
	* \param o The source object to evaluate
	* \param c The context where the array is bound to
	* \param pBuffer The data buffer
	* \return bool
	*/
	virtual bool evaluateReadOnlyInObjectScope(KviKvsObject * o, KviKvsRunTimeContext * c, KviKvsVariant * pBuffer);

	/**
	* \brief Evaluates the array element in read-write mode in the object scope
	* \param o The source object to evaluate
	* \param c The context where the array is bound to
	* \return KviKvsRWEvaluationResult *
	*/
	virtual KviKvsRWEvaluationResult * evaluateReadWriteInObjectScope(KviKvsObject * o, KviKvsRunTimeContext * c);

protected:
	/**
	* \brief Evaluates the index of the array
	* \param c The context where the array is bound to
	* \param iVal The index of the array
	* \return bool
	*/
	bool evaluateIndex(KviKvsRunTimeContext * c, kvs_int_t & iVal);
};

#endif //_KVI_KVS_TREENODE_ARRAYELEMENT_H_
