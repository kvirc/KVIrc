#ifndef _KVI_KVS_ASYNCOPERATION_H_
#define _KVI_KVS_ASYNCOPERATION_H_
//=============================================================================
//
//   File : KviKvsAsyncOperation.h
//   Creation date : Sat 09 Jul 2005 04:54:24 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2005-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include <QObject>
#include <unordered_set>

class KviWindow;

class KVIRC_API KviKvsAsyncOperation : public QObject
{
	Q_OBJECT
public:
	KviKvsAsyncOperation(KviWindow * pWnd);
	virtual ~KviKvsAsyncOperation();

protected:
	KviWindow * m_pWnd;

public:
	KviWindow * window() { return m_pWnd; };
};

class KVIRC_API KviKvsAsyncOperationManager
{
	friend class KviKvsAsyncOperation;

public:
	~KviKvsAsyncOperationManager();

protected:
	std::unordered_set<KviKvsAsyncOperation *> m_pOperationList;

protected:
	void registerOperation(KviKvsAsyncOperation * o);
	void unregisterOperation(KviKvsAsyncOperation * o);
};

#endif //!_KVI_KVS_ASYNCOPERATION_H_
