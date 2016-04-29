#ifndef _KVI_KVS_TREENODE_ALIASFUNCTIONCALL_H_
#define _KVI_KVS_TREENODE_ALIASFUNCTIONCALL_H_
//=============================================================================
//
//   File : KviKvsTreeNodeAliasFunctionCall.h
//   Creation date : Tue 07 Oct 2003 03:21:23 by Szymon Stefanek
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
* \file KviKvsTreeNodeAliasFunctionCall.h
* \author Szymon Stefanek
* \brief Treenode alias function call handling
*/

#include "kvi_settings.h"
#include "KviQString.h"
#include "KviKvsTreeNodeFunctionCall.h"
#include "KviKvsTreeNodeDataList.h"

class KviKvsRunTimeContext;

/**
* \class KviKvsTreeNodeAliasFunctionCall
* \brief Treenode alias function call class
*/
class KVIRC_API KviKvsTreeNodeAliasFunctionCall : public KviKvsTreeNodeFunctionCall
{
public:
	/**
	* \brief Constructs the treenode alias function call object
	* \param pLocation The location char of the instruction
	* \param szAliasName The name of the alias
	* \param pParams The parameters' list
	* \return KviKvsTreeNodeAliasFunctionCall
	*/
	KviKvsTreeNodeAliasFunctionCall(const QChar * pLocation, const QString & szAliasName, KviKvsTreeNodeDataList * pParams);

	/**
	* \brief Destroys the treenode alias function call object
	*/
	~KviKvsTreeNodeAliasFunctionCall();

public:
	/**
	* \brief Dumps the tree
	* \param prefix The prefix of the instruction
	* \return void
	*/
	virtual void dump(const char * prefix);

	/**
	* \brief Sets the buffer as Alias Function Call
	* \param szBuffer The buffer :)
	* \return void
	*/
	virtual void contextDescription(QString & szBuffer);

	/**
	* \brief Evaluates the script in read-only mode
	* \param c The context where the alias is bound to
	* \param pBuffer The data buffer
	* \return bool
	*/
	virtual bool evaluateReadOnly(KviKvsRunTimeContext * c, KviKvsVariant * pBuffer);
};

#endif //_KVI_KVS_TREENODE_ALIASFUNCTIONCALL_H_
