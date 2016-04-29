#ifndef _KVI_KVS_TREENODE_BASE_H_
#define _KVI_KVS_TREENODE_BASE_H_
//=============================================================================
//
//   File : KviKvsTreeNodeBase.h
//   Creation date : Tue 07 Oct 2003 02:00:28 by Szymon Stefanek
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
* \file KviKvsTreeNodeBase.h
* \author Szymon Stefanek
* \brief Treenode handling
*/

#include "kvi_settings.h"
#include "KviQString.h"

/**
* \class KviKvsTreeNode
* \brief Treenode class
*/
class KVIRC_API KviKvsTreeNode
{
	friend class KviKvsParser;

public:
	/**
	* \brief Constructs the treenode object
	* \param pLocation The location char of the instruction
	* \return KviKvsTreeNode
	*/
	KviKvsTreeNode(const QChar * pLocation);

	/**
	* \brief Destroys the treenode object
	*/
	virtual ~KviKvsTreeNode(){};

protected:
	KviKvsTreeNode * m_pParent;
	const QChar * m_pLocation;

public:
	/**
	* \brief Sets the parent object
	* \param p The parent treenode
	* \return void
	*/
	void setParent(KviKvsTreeNode * p) { m_pParent = p; };

	/**
	* \brief Returns the parent object
	* \return KviKvsTreeNode *
	*/
	KviKvsTreeNode * parent() { return m_pParent; };

	/**
	* \brief Returns the location char
	* \return const QChar *
	*/
	const QChar * location() { return m_pLocation; };

	/**
	* \brief Dumps the tree
	* \param prefix The prefix of the instruction
	* \return void
	*/
	virtual void dump(const char * prefix) = 0;

	/**
	* \brief Sets the buffer
	* \param szBuffer The buffer :)
	* \return void
	*/
	virtual void contextDescription(QString & szBuffer) = 0;

protected:
	/**
	* \brief Sets the location char
	*
	* This is called only by KviKvsParser
	* \param pLocation The location char
	* \return void
	*/
	void setLocation(const QChar * pLocation) { m_pLocation = pLocation; };
};

#endif //_KVI_KVS_TREENODE_BASE_H_
