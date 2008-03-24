//=============================================================================
//
//   File : kvi_kvs_asyncoperation.cpp
//   Created on Sat 09 Jul 2005 04:54:24 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2005 Szymon Stefanek <pragma at kvirc dot net>
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

#define __KVIRC__

#include "kvi_kvs_asyncoperation.h"
#include "kvi_kvs_kernel.h"
#include "kvi_window.h"

KviKvsAsyncOperation::KviKvsAsyncOperation(KviWindow * pWnd)
: QObject()
{
	m_pWnd = pWnd;
	KviKvsKernel::instance()->asyncOperationManager()->registerOperation(this);
}

KviKvsAsyncOperation::~KviKvsAsyncOperation()
{
	KviKvsKernel::instance()->asyncOperationManager()->unregisterOperation(this);
}


KviKvsAsyncOperationManager::KviKvsAsyncOperationManager()
{
	m_pOperationList = new KviPointerList<KviKvsAsyncOperation>;
	m_pOperationList->setAutoDelete(false);
}

KviKvsAsyncOperationManager::~KviKvsAsyncOperationManager()
{
	while(m_pOperationList->first())
		delete m_pOperationList->first();
	delete m_pOperationList;
}

void KviKvsAsyncOperationManager::registerOperation(KviKvsAsyncOperation * o)
{
	m_pOperationList->append(o);
}

void KviKvsAsyncOperationManager::unregisterOperation(KviKvsAsyncOperation * o)
{
	m_pOperationList->removeRef(o);
}

