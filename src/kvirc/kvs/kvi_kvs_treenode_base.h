#ifndef _KVI_KVS_TREENODE_BASE_H_
#define _KVI_KVS_TREENODE_BASE_H_
//=============================================================================
//
//   File : kvi_kvs_treenode_base.h
//   Created on Tue 07 Oct 2003 02:00:28 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003 Szymon Stefanek <pragma at kvirc dot net>
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your opinion) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#include "kvi_settings.h"
#include "kvi_qstring.h"

class KVIRC_API KviKvsTreeNode
{
	friend class KviKvsParser;
public:
	KviKvsTreeNode(const QChar * pLocation);
	virtual ~KviKvsTreeNode(){};
protected:
	KviKvsTreeNode * m_pParent;
	const QChar    * m_pLocation;
public:
	void setParent(KviKvsTreeNode * p){ m_pParent = p; };
protected:
	// this is called only by KviKvsParser
	void setLocation(const QChar * pLocation){ m_pLocation = pLocation; };
public:
	KviKvsTreeNode * parent(){ return m_pParent; };
	const QChar * location(){ return m_pLocation; };
	virtual void dump(const char * prefix) = 0;
	virtual void contextDescription(QString &szBuffer) = 0;
};

#endif //!_KVI_KVS_TREENODE_BASE_H_
