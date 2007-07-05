//   File : controller.cpp
//   Creation date : Thu Apr 30 2002 17:13:12 GMT by Juanjo Álvarez
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Juanjo Álvarez (juanjux@yahoo.es)
//   Copyright (C) 2002 Szymon Stefanek (kvirc@tin.it)
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

#include "controller.h"
#include "kvi_window.h"
#include "kvi_console.h"
#include "kvi_mirccntrl.h"
#include "kvi_app.h"
#include "kvi_options.h"

#include <qtimer.h>
#include <qstringlist.h>
#include <qclipboard.h>

extern KviPtrList<SPasteController> * g_pControllerList;

SPasteController::SPasteController(KviWindow * w,int id)
	: m_pClipBuff(NULL),m_pFile(NULL),m_pId(id),m_pWindow(w)
{
    g_pControllerList->append(this);
    //m_pWindow = w;
    m_pTimer = new QTimer(this);
}

SPasteController::~SPasteController()
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

bool SPasteController::pasteFileInit(QString &fileName)
{
	if(m_pClipBuff)return false; // can't paste a file while pasting the clipboard
	if(m_pFile)return false; // can't paste two files at a time
	m_pFile = new QFile(fileName);
	if(!m_pFile->open(IO_ReadOnly))return false;
	connect(m_pTimer,SIGNAL(timeout()),this,SLOT(pasteFile()));
	m_pTimer->start(KVI_OPTION_UINT(KviOption_uintPasteDelay));
	return true;
}

bool SPasteController::pasteClipboardInit(void)
{
	if(m_pFile)return false; // can't paste clipboard while pasting a file
	QString tmp(g_pApp->clipboard()->text());
	if(m_pClipBuff)
	{
		(*m_pClipBuff) += QStringList::split("\n",tmp,true);
	} else {
		m_pClipBuff = new QStringList(QStringList::split("\n",tmp,true));
		m_clipBuffIterator = m_pClipBuff->begin();
	}
	connect(m_pTimer,SIGNAL(timeout()),this,SLOT(pasteClipboard()));
	m_pTimer->start(KVI_OPTION_UINT(KviOption_uintPasteDelay));
	return true;
}

void SPasteController::pasteFile(void)
{
#ifndef COMPILE_USE_QT4
    QString line;
    if(m_pFile->readLine(line,999) != -1)
    {
	if(line.isEmpty())
		line = QChar(KVI_TEXT_RESET);
        if( (!g_pApp->windowExists(m_pWindow)) || m_pWindow->console()->isNotConnected() )
        { 
            m_pFile->close();
            delete this;
        } else m_pWindow->ownMessage(line.ascii());
    } else { //File finished
        m_pFile->close();
        delete this;
    }
#endif
}

void SPasteController::pasteClipboard(void)
{
	if(m_clipBuffIterator != m_pClipBuff->end()) 
	{
		if((!g_pApp->windowExists(m_pWindow)) || m_pWindow->console()->isNotConnected() )
		  	delete this;
		else {
			if((*m_clipBuffIterator).isEmpty())(*m_clipBuffIterator) = QChar(KVI_TEXT_RESET);
			m_pWindow->ownMessage((*m_clipBuffIterator).ascii()); // <-- not good :/
			++m_clipBuffIterator;
		}
	} else delete this;//Clipboard finished
}

#include "controller.moc"
