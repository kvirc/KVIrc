//=============================================================================
//
//   File : KviFileTransfer.cpp
//   Creation date : Mon Apr 21 22:14:31 2003 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviFileTransfer.h"
#include "KviKvsScript.h"
#include "KviModuleExtension.h"
#include "KviWindow.h"

#include "KviApplication.h"

#include <QMenu>

static KviFileTransferManager * g_pFileTransferManager = 0;


KviFileTransferManager::KviFileTransferManager()
: QObject()
{
	m_pTransferList = 0;
	m_pTransferWindow = 0;
}

KviFileTransferManager::~KviFileTransferManager()
{
	killAllTransfers();
}

KviFileTransferManager * KviFileTransferManager::instance()
{
	if(!g_pFileTransferManager) {
		g_pFileTransferManager = new KviFileTransferManager();
	}
	return g_pFileTransferManager;
}

void KviFileTransferManager::cleanup()
{
	if(g_pFileTransferManager) {
		delete g_pFileTransferManager;
		g_pFileTransferManager = 0;
	}
}

void KviFileTransferManager::killAllTransfers()
{
	while(m_pTransferList)
	{
		KviFileTransfer * t = m_pTransferList->first();
		t->die();
	}
}

void KviFileTransferManager::killTerminatedTransfers()
{
	if(m_pTransferList)
	{
		KviPointerList<KviFileTransfer> l;
		l.setAutoDelete(false);

		for(KviFileTransfer * f = m_pTransferList->first();f;f = m_pTransferList->next())
		{
			if(f->terminated()) {
				l.append(f);
			}
		}

		for(KviFileTransfer * d = l.first();d;d = l.next())
			d->die();
	}
}

void KviFileTransferManager::invokeTransferWindow(bool bCreateMinimized, bool bNoRaise)
{
	if(!m_pTransferWindow)
	{
		QString szScript("filetransferwindow.open");
		if(bCreateMinimized)
			szScript.append(" -m");
		if(bNoRaise)
			szScript.append(" -n");
		KviKvsScript::run(szScript,g_pActiveWindow);
	}
}

void KviFileTransferManager::registerTransfer(KviFileTransfer * t)
{
	if(!m_pTransferList)
	{
		m_pTransferList = new KviPointerList<KviFileTransfer>;
		m_pTransferList->setAutoDelete(false);
	}

	m_pTransferList->append(t);

	emit transferRegistered(t);
}

void KviFileTransferManager::unregisterTransfer(KviFileTransfer * t)
{
	if(!m_pTransferList)
	{
		qDebug("Ops: unregistering transfer with no transfer list!");
		return;
	}

	emit transferUnregistering(t);

	m_pTransferList->removeRef(t);
	if(m_pTransferList->isEmpty())
	{
		delete m_pTransferList;
		m_pTransferList = 0;
	}
}



KviFileTransfer::KviFileTransfer()
: QObject()
{
	m_pDisplayItem = 0;
	m_iId = g_pApp->getGloballyUniqueId();
	manager()->registerTransfer(this);
}

KviFileTransfer::~KviFileTransfer()
{
	manager()->unregisterTransfer(this);
}

KviWindow * KviFileTransfer::outputWindow()
{
	KviWindow * o = transferWindow();
	if(o)return o;
	return g_pActiveWindow;
}

void KviFileTransfer::invokeTransferWindow(bool bCreateMinimized, bool bNoRaise)
{
	manager()->invokeTransferWindow(bCreateMinimized,bNoRaise);
}

QString KviFileTransfer::localFileName()
{
	return QString();
}

QString KviFileTransfer::retryCommand()
{
	return QString();
}

bool KviFileTransfer::terminated()
{
	return !active();
}

QString KviFileTransfer::tipText()
{
	return QString();
}

int KviFileTransfer::displayHeight(int iLineSpacing)
{
	return iLineSpacing * 2;
}

void KviFileTransfer::displayUpdate()
{
	if(m_pDisplayItem)m_pDisplayItem->displayUpdate();
}

void KviFileTransfer::die()
{
	if(m_pDisplayItem)
		m_pDisplayItem->tableWidget()->removeRow(m_pDisplayItem->row());
	delete this;
}
