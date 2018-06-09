//=============================================================================
//
//   File : SlowPasteController.cpp
//   Creation date : Thu Apr 30 2002 17:13:12 GMT by Juanjo Álvarez
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002 Juanjo Álvarez (juanjux@yahoo.es)
//   Copyright (C) 2002-2010 Szymon Stefanek (kvirc@tin.it)
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

#include "SlowPasteController.h"

#include "KviWindow.h"
#include "KviConsoleWindow.h"
#include "KviControlCodes.h"
#include "KviApplication.h"
#include "KviOptions.h"

#include <QTimer>
#include <QString>
#include <QStringList>
#include <QClipboard>

extern KviPointerList<SlowPasteController> * g_pControllerList;

SlowPasteController::SlowPasteController(KviWindow * w, int id)
    : m_pClipBuff(nullptr), m_pFile(nullptr), m_pId(id), m_pWindow(w)
{
	g_pControllerList->append(this);
	//m_pWindow = w;
	m_pTimer = new QTimer(this);
}

SlowPasteController::~SlowPasteController()
{
	g_pControllerList->removeRef(this);
	if(m_pFile)
	{
		m_pFile->close();
		delete m_pFile;
	}
	if(m_pTimer)
	{
		m_pTimer->stop();
		delete m_pTimer;
	}
	if(m_pClipBuff)
		delete m_pClipBuff;
}

bool SlowPasteController::pasteFileInit(QString & fileName)
{
	if(m_pClipBuff)
		return false; // can't paste a file while pasting the clipboard
	if(m_pFile)
		return false; // can't paste two files at a time
	m_pFile = new QFile(fileName);
	if(!m_pFile->open(QIODevice::ReadOnly))
		return false;
	//avoid double connection
	disconnect(m_pTimer, SIGNAL(timeout()), nullptr, nullptr);
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(pasteFile()));
	//avoid timer reset if always active
	if(!m_pTimer->isActive())
		m_pTimer->start(KVI_OPTION_UINT(KviOption_uintPasteDelay));
	return true;
}

bool SlowPasteController::pasteClipboardInit()
{
	if(m_pFile)
		return false; // can't paste clipboard while pasting a file
	QString tmp(g_pApp->clipboard()->text());
	if(m_pClipBuff)
	{
		(*m_pClipBuff) += tmp.isEmpty() ? QStringList() : tmp.split("\n", QString::KeepEmptyParts);
	}
	else
	{
		m_pClipBuff = new QStringList(tmp.isEmpty() ? QStringList() : tmp.split("\n", QString::KeepEmptyParts));
	}
	//avoid double connection
	disconnect(m_pTimer, SIGNAL(timeout()), nullptr, nullptr);
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(pasteClipboard()));
	//avoid timer reset if always active
	if(!m_pTimer->isActive())
		m_pTimer->start(KVI_OPTION_UINT(KviOption_uintPasteDelay));
	return true;
}

void SlowPasteController::pasteFile()
{
	QString line;
	char data[1024];
	if(m_pFile->readLine(data, sizeof(data)) != -1)
	{
		line = data;
		if(line.isEmpty())
			line = QChar(KviControlCodes::Reset);

		line.replace('\t', QString(KVI_OPTION_UINT(KviOption_uintSpacesToExpandTabulationInput), ' ')); //expand tabs to spaces

		if(!g_pApp->windowExists(m_pWindow))
		{
			m_pFile->close();
			delete this;
		}
		else
		{
			m_pWindow->ownMessage(line.toLatin1());
		}
	}
	else
	{ //File finished
		m_pFile->close();
		delete this;
	}
}

void SlowPasteController::pasteClipboard()
{
	if(m_pClipBuff->isEmpty() || !g_pApp->windowExists(m_pWindow))
	{
		delete this;
	}
	else
	{
		QString line = m_pClipBuff->takeFirst();
		line.replace('\t', QString(KVI_OPTION_UINT(KviOption_uintSpacesToExpandTabulationInput), ' ')); //expand tabs to spaces
		m_pWindow->ownMessage(line);
	}
}
