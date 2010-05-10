//===========================================================================
//
//   File : kvi_ircview_loghandling.cpp
//   Creation date : Wed Oct 1 2008 17:18:20 by Fabio Bas
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2008 Fabio Bas (ctrlaltca at gmail dot com)
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
//===========================================================================
//
// This file was originally part of kvi_ircview.cpp
//
//===========================================================================

#include "kvi_ircview.h"
#include "kvi_ircviewprivate.h"
#include "kvi_locale.h"
#include "kvi_options.h"
#include "kvi_out.h"
#include "kvi_qstring.h"
#include "kvi_window.h"

#ifdef COMPILE_ZLIB_SUPPORT
	#include <zlib.h>
#endif

#include <QFileInfo>
#include <QFile>
#include <QDateTime>
#include <QTextCodec>

void KviIrcView::stopLogging()
{
	if(m_pLogFile)
	{
		QString szDate;
		QDateTime date = QDateTime::currentDateTime();
		switch(KVI_OPTION_UINT(KviOption_uintOutputDatetimeFormat))
		{
			case 0:
				szDate = date.toString();
				break;
			case 1:
				szDate = date.toString(Qt::ISODate);
				break;
			case 2:
				szDate = date.toString(Qt::SystemLocaleDate);
				break;
		}
		
		QString szLogEnd = QString(__tr2qs("### Log session terminated at %1 ###")).arg(szDate);
		add2Log(szLogEnd);
		m_pLogFile->close();
#ifdef COMPILE_ZLIB_SUPPORT
		if(KVI_OPTION_BOOL(KviOption_boolGzipLogs))
		{
			if(m_pLogFile->open(QIODevice::ReadOnly))
			{
				QByteArray bytes;
				bytes=m_pLogFile->readAll();
				m_pLogFile->close();
				QFileInfo fi(*m_pLogFile);
				QString szFname=fi.absolutePath()+QString("/")+fi.completeBaseName();
				gzFile file=gzopen(QTextCodec::codecForLocale()->fromUnicode(szFname).data(),"ab9");
				if(file)
				{
					gzwrite(file,bytes.data(),bytes.size());
					gzclose(file);
					m_pLogFile->remove();
				} else {
					debug("Cannot open compressed stream");
				}
			}
		}
#endif
		delete m_pLogFile;
		m_pLogFile = 0;
	}
}

void KviIrcView::getLogFileName(QString &buffer)
{
	if(m_pLogFile) buffer=m_pLogFile->fileName();
}

void KviIrcView::getTextBuffer(QString &buffer)
{
	// FIXME: #warning "This does not merge the KviChannel::m_pMessageView buffer!"
	buffer = "";
	if(!m_pLastLine)return;
	for(KviIrcViewLine *l=m_pFirstLine;l;l=l->pNext)
	{
		buffer.append(l->szText);
		buffer.append("\n");
	}
}

void KviIrcView::flushLog()
{
	if(m_pLogFile) {
#ifdef COMPILE_ZLIB_SUPPORT
		if(KVI_OPTION_BOOL(KviOption_boolGzipLogs))
		{
			m_pLogFile->close();
			if(m_pLogFile->open(QIODevice::ReadOnly))
			{
				QByteArray bytes;
				bytes=m_pLogFile->readAll();
				m_pLogFile->close();
				QFileInfo fi(*m_pLogFile);
				QString szFname=fi.absolutePath()+QString("/")+fi.completeBaseName();
				gzFile file=gzopen(QTextCodec::codecForLocale()->fromUnicode(szFname).data(),"ab9");
				if(file)
				{
					gzwrite(file,bytes.data(),bytes.size());
					gzclose(file);
					m_pLogFile->remove();
				} else {
					debug("Cannot open compressed stream");
				}
			}
			m_pLogFile->open(QIODevice::Append|QIODevice::WriteOnly);
		} else
#endif
		m_pLogFile->flush();
	}
	else if(m_pMasterView)m_pMasterView->flushLog();
}

const QString & KviIrcView::lastMessageText()
{
	KviIrcViewLine * pCur=m_pLastLine;
	while(pCur)
	{
		switch(pCur->iMsgType)
		{
			case KVI_OUT_CHANPRIVMSG:
			case KVI_OUT_CHANPRIVMSGCRYPTED:
			case KVI_OUT_CHANNELNOTICE:
			case KVI_OUT_CHANNELNOTICECRYPTED:
			case KVI_OUT_ACTION:
			case KVI_OUT_OWNPRIVMSG:
			case KVI_OUT_OWNPRIVMSGCRYPTED:
			case KVI_OUT_HIGHLIGHT:
				return pCur->szText;
		}
		pCur=pCur->pPrev;
	}
	return KviQString::Empty;
}

const QString & KviIrcView::lastLineOfText()
{
	if(!m_pLastLine)return KviQString::Empty;
	return m_pLastLine->szText;
}

void KviIrcView::setMasterView(KviIrcView * v)
{
	if(m_pMasterView)disconnect(this,SLOT(masterDead()));
	m_pMasterView = v;
	if(m_pMasterView)connect(m_pMasterView,SIGNAL(destroyed()),this,SLOT(masterDead()));
}

void KviIrcView::masterDead()
{
	m_pMasterView = 0;
}

bool KviIrcView::startLogging(const QString& fname,bool bPrependCurBuffer)
{
	stopLogging();
	QString szFname(fname);

	if(fname.isEmpty())
	{
		if(!m_pKviWindow)return false;
		m_pKviWindow->getDefaultLogFileName(szFname);
	}

#ifdef COMPILE_ZLIB_SUPPORT
	if(KVI_OPTION_BOOL(KviOption_boolGzipLogs))
		szFname+=".tmp";
#endif

	m_pLogFile = new QFile(szFname);

	if(m_pLogFile->exists())
	{
		if(!m_pLogFile->open(QIODevice::Append|QIODevice::WriteOnly))
		{
			delete m_pLogFile;
			m_pLogFile = 0;
			return false;
		}
	} else {
		if(!m_pLogFile->open(QIODevice::WriteOnly))
		{
			delete m_pLogFile;
			m_pLogFile = 0;
			return false;
		}
	}

	QString szDate;
	QDateTime date = QDateTime::currentDateTime();
	switch(KVI_OPTION_UINT(KviOption_uintOutputDatetimeFormat))
	{
		case 0:
			szDate = date.toString();
			break;
		case 1:
			szDate = date.toString(Qt::ISODate);
			break;
		case 2:
			szDate = date.toString(Qt::SystemLocaleDate);
			break;
	}
	
	QString szLogStart = QString(__tr2qs("### Log session started at %1 ###")).arg(szDate);
	add2Log(szLogStart);
	if(bPrependCurBuffer)
	{
		add2Log(__tr2qs("### Existing data buffer:"));
		QString buffer;
		getTextBuffer(buffer);
		add2Log(buffer);
		add2Log(__tr2qs("### End of existing data buffer."));
		m_pLogFile->flush();
	}

	return true;
}


void KviIrcView::add2Log(const QString &szBuffer,int iMsgType,bool bPrependDate)
{
	QByteArray szTmp;

	if(!KVI_OPTION_BOOL(KviOption_boolStripMsgTypeInLogs))
	{
		QString szMessageType = QString("%1 ").arg(iMsgType);

		szTmp = KviQString::toUtf8(szMessageType);

		if(m_pLogFile->write(szTmp.data(),szTmp.length())==-1)
			debug("WARNING : Can not write to the log file.");
	}
	
	if(bPrependDate)
	{
		QString szDate;
		QDateTime date = QDateTime::currentDateTime();
		switch(KVI_OPTION_UINT(KviOption_uintOutputDatetimeFormat))
		{
			case 0:
				szDate = date.toString("[hh:mm:ss] ");
			break;
			case 1:
				szDate = date.toString(Qt::ISODate);
			break;
			case 2:
				szDate = date.toString(Qt::SystemLocaleDate);
			break;
		}

		szTmp = KviQString::toUtf8(szDate);

		if(m_pLogFile->write(szTmp.data(),szTmp.length())==-1)
			debug("WARNING : Can not write to the log file.");
	}
	
	szTmp = KviQString::toUtf8(szBuffer);
	szTmp.append('\n');

	if(m_pLogFile->write(szTmp.data(),szTmp.length())==-1)
		debug("WARNING : Can not write to the log file.");
}

