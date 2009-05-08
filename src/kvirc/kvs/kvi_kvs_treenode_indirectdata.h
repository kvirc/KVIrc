#ifndef _KVI_KVS_TREENODE_INDIRECTDATA_H_
#define _KVI_KVS_TREENODE_INDIRECTDATA_H_
//=============================================================================
//
//   File : kvi_kvs_treenode_indirectdata.h
//   Creation date : Thu 16 Oct 2003 22:49:46 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2008 Szymon Stefanek <pragma at kvirc dot net>
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "kvi_settings.h"
#include "kvi_qstring.h"
#include "kvi_kvs_treenode_data.h"

class KVIRC_API KviKvsTreeNodeIndirectData : public KviKvsTreeNodeData
{
public:
	KviKvsTreeNodeIndirectData(const QChar * pLocation,KviKvsTreeNodeData * pSource);
	~KviKvsTreeNodeIndirectData();
protected:
	KviKvsTreeNodeData * m_pSource; // can't be null!
public:
	virtual bool canEvaluateInObjectScope();
};

#endif //!_KVI_KVS_TREENODE_INDIRECTDATA_H_
