#ifndef _KVI_KVS_TREENODE_ALIASSIMPLECOMMAND_H_
#define _KVI_KVS_TREENODE_ALIASSIMPLECOMMAND_H_
//=============================================================================
//
//   File : KviKvsTreeNodeAliasSimpleCommand.h
//   Creation date : Thu 09 Oct 2003 02:16:42 by Szymon Stefanek
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
* \file KviKvsTreeNodeAliasSimpleCommand.h
* \author Szymon Stefanek
* \brief Treenode alias simple command call handling
*/

#include "kvi_settings.h"
#include "KviQString.h"
#include "KviKvsTreeNodeSimpleCommand.h"

class KviKvsTreeNodeDataList;
class KviKvsRunTimeContext;

/**
* \class KviKvsTreeNodeAliasSimpleCommand
* \brief Treenode alias simple command call class
*/
class KVIRC_API KviKvsTreeNodeAliasSimpleCommand : public KviKvsTreeNodeSimpleCommand
{
public:
	/**
	* \brief Constructs the treenode alias simple command call object
	* \param pLocation The location char of the instruction
	* \param szCmdName The name of the command
	* \param pParams The parameters' list
	* \return KviKvsTreeNodeAliasSimpleCommand
	*/
	KviKvsTreeNodeAliasSimpleCommand(const QChar * pLocation, const QString & szCmdName, KviKvsTreeNodeDataList * pParams);

	/**
	* \brief Destroys the treenode alias simple command call object
	*/
	~KviKvsTreeNodeAliasSimpleCommand();

public:
	/**
	* \brief Sets the buffer as Alias Simple Command
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
	* \brief Evaluates the command
	* \param c The context where the command is bound to
	* \return bool
	*/
	virtual bool execute(KviKvsRunTimeContext * c);
};

#endif //_KVI_KVS_TREENODE_ALIASSIMPLECOMMAND_H_
