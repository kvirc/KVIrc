#ifndef _KVI_KVS_TREENODE_INSTRUCTION_H_
#define _KVI_KVS_TREENODE_INSTRUCTION_H_
//=============================================================================
//
//   File : KviKvsTreeNodeInstruction.h
//   Creation date : Mon 06 Oct 2003 03.46 CEST by Szymon Stefanek
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

/**
* \file KviKvsTreeNode.h
* \author Szymon Stefanek
* \brief Treenode instruction handling
*/

#include "KviQString.h"
#include "KviKvsTreeNodeBase.h"

class KviKvsRunTimeContext;

/**
* \class KviKvsTreeNodeInstruction
* \brief Treenode class to handle instructions
*/
class KVIRC_API KviKvsTreeNodeInstruction : public KviKvsTreeNode
{
public:
	/**
	* \brief Constructs the treenode instruction object
	* \param pLocation The location char of the instruction
	* \return KviKvsTreeNodeInstruction
	*/
	KviKvsTreeNodeInstruction(const QChar * pLocation)
	    : KviKvsTreeNode(pLocation){};

	/**
	* \brief Destroys the treenode instruction object
	*/
	~KviKvsTreeNodeInstruction(){};

public:
	/**
	* \brief Sets the buffer as Instruction
	* \param szBuffer The buffer :)
	* \return void
	*/
	virtual void contextDescription(QString & szBuffer);

	/**
	* \brief Dumps the instructions tree
	* \param prefix The prefix of the instruction
	* \return void
	*/
	virtual void dump(const char * prefix);

	/**
	* \brief This is the main entry point to the tree visit
	*
	* It returns false when the tree visiting has to be stopped and returns
	* true when it has been successfully completed. A false return value may
	* signal an error but is also used by break, halt, return and similar.
	* The error condition is signaled by KviKvsRunTimeContext::error().
	* \param c The source context
	* \return bool
	*/
	virtual bool execute(KviKvsRunTimeContext * c) = 0;
};

#endif //_KVI_KVS_TREENODE_H_
