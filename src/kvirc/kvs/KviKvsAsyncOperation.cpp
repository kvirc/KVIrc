//=============================================================================
//
//   File : KviKvsAsyncOperation.cpp
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

#include "KviKvsAsyncOperation.h"
#include "KviKvsKernel.h"
#include "KviWindow.h"

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

KviKvsAsyncOperationManager::~KviKvsAsyncOperationManager()
{
	while(!m_pOperationList.empty())
		delete *m_pOperationList.begin();
}

void KviKvsAsyncOperationManager::registerOperation(KviKvsAsyncOperation * o)
{
	m_pOperationList.insert(o);
}

void KviKvsAsyncOperationManager::unregisterOperation(KviKvsAsyncOperation * o)
{
	m_pOperationList.erase(o);
}
