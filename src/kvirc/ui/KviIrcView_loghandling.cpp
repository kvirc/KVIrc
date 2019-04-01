//===========================================================================
//
//   File : KviIrcView_loghandling.cpp
//   Creation date : Wed Oct 1 2008 17:18:20 by Fabio Bas
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Fabio Bas (ctrlaltca at gmail dot com)
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
//===========================================================================

//
// This file was originally part of KviIrcView.cpp
//

#include "KviIrcView.h"
#include "KviIrcView_private.h"
#include "KviLocale.h"
#include "KviOptions.h"
#include "kvi_out.h"
#include "KviQString.h"
#include "KviWindow.h"

#ifdef COMPILE_ZLIB_SUPPORT
#include <zlib.h>
#endif

#include <QFileInfo>
#include <QFile>
#include <QDateTime>
#include <QTextCodec>
#include <QLocale>

void KviIrcView::stopLogging()
{
	if(m_pLogFile)
	{
		QDateTime date = QDateTime::currentDateTime();
		QString szLogEnd = QString(__tr2qs("### Log session terminated ###"));
		add2Log(szLogEnd, date, KVI_OUT_LOG, true);
		m_pLogFile->close();
#ifdef COMPILE_ZLIB_SUPPORT
		if(KVI_OPTION_BOOL(KviOption_boolGzipLogs))
		{
			if(m_pLogFile->open(QIODevice::ReadOnly))
			{
				QByteArray bytes;
				bytes = m_pLogFile->readAll();
				m_pLogFile->close();
				QFileInfo fi(*m_pLogFile);
				QString szFname = fi.absolutePath() + QString("/") + fi.completeBaseName();
				gzFile file = gzopen(QTextCodec::codecForLocale()->fromUnicode(szFname).data(), "ab9");
				if(file)
				{
					gzwrite(file, bytes.data(), bytes.size());
					gzclose(file);
					m_pLogFile->remove();
				}
				else
				{
					qDebug("Can't open compressed stream");
				}
			}
		}
#endif
		delete m_pLogFile;
		m_pLogFile = nullptr;
	}
}

void KviIrcView::getLogFileName(QString & buffer)
{
	if(m_pLogFile)
		buffer = m_pLogFile->fileName();
}

void KviIrcView::getTextBuffer(QString & buffer)
{
	// FIXME: #warning "This does not merge the KviChannelWindow::m_pMessageView buffer!"
	buffer = "";
	if(!m_pLastLine)
		return;
	for(KviIrcViewLine * l = m_pFirstLine; l; l = l->pNext)
	{
		buffer.append(l->szText);
		buffer.append("\n");
	}
}

void KviIrcView::flushLog()
{
	if(m_pLogFile)
	{
#ifdef COMPILE_ZLIB_SUPPORT
		if(KVI_OPTION_BOOL(KviOption_boolGzipLogs))
		{
			m_pLogFile->close();
			if(m_pLogFile->open(QIODevice::ReadOnly))
			{
				QByteArray bytes;
				bytes = m_pLogFile->readAll();
				m_pLogFile->close();
				QFileInfo fi(*m_pLogFile);
				QString szFname = fi.absolutePath() + QString("/") + fi.completeBaseName();
				gzFile file = gzopen(QTextCodec::codecForLocale()->fromUnicode(szFname).data(), "ab9");
				if(file)
				{
					gzwrite(file, bytes.data(), bytes.size());
					gzclose(file);
					m_pLogFile->remove();
				}
				else
				{
					qDebug("Can't open compressed stream");
				}
			}
			m_pLogFile->open(QIODevice::Append | QIODevice::WriteOnly);
		}
		else
#endif
			m_pLogFile->flush();
	}
	else if(m_pMasterView)
		m_pMasterView->flushLog();
}

const QString & KviIrcView::lastMessageText()
{
	KviIrcViewLine * pCur = m_pLastLine;
	while(pCur)
	{
		switch(pCur->iMsgType)
		{
			case KVI_OUT_CHANPRIVMSG:
			case KVI_OUT_CHANPRIVMSGCRYPTED:
			case KVI_OUT_CHANNELNOTICE:
			case KVI_OUT_CHANNELNOTICECRYPTED:
			case KVI_OUT_ACTION:
			case KVI_OUT_ACTIONCRYPTED:
			case KVI_OUT_OWNACTION:
			case KVI_OUT_OWNACTIONCRYPTED:
			case KVI_OUT_OWNPRIVMSG:
			case KVI_OUT_OWNPRIVMSGCRYPTED:
			case KVI_OUT_HIGHLIGHT:
				return pCur->szText;
		}
		pCur = pCur->pPrev;
	}
	return KviQString::Empty;
}

const QString & KviIrcView::lastLineOfText()
{
	if(!m_pLastLine)
		return KviQString::Empty;
	return m_pLastLine->szText;
}

void KviIrcView::setMasterView(KviIrcView * v)
{
	if(m_pMasterView)
		disconnect(this, SLOT(masterDead()));
	m_pMasterView = v;
	if(m_pMasterView)
		connect(m_pMasterView, SIGNAL(destroyed()), this, SLOT(masterDead()));
}

void KviIrcView::masterDead()
{
	m_pMasterView = nullptr;
}

bool KviIrcView::startLogging(const QString & fname, bool bPrependCurBuffer)
{
	stopLogging();
	QString szFname(fname);

	if(fname.isEmpty())
	{
		if(!m_pKviWindow)
			return false;
		m_pKviWindow->getDefaultLogFileName(szFname);
	}

#ifdef COMPILE_ZLIB_SUPPORT
	if(KVI_OPTION_BOOL(KviOption_boolGzipLogs))
		szFname += ".tmp";
#endif

	m_pLogFile = new QFile(szFname);

	if(m_pLogFile->exists())
	{
		if(!m_pLogFile->open(QIODevice::Append | QIODevice::WriteOnly))
		{
			delete m_pLogFile;
			m_pLogFile = nullptr;
			return false;
		}
	}
	else
	{
		if(!m_pLogFile->open(QIODevice::WriteOnly))
		{
			delete m_pLogFile;
			m_pLogFile = nullptr;
			return false;
		}
	}

	QDateTime date = QDateTime::currentDateTime();
	QString szLogStart = QString(__tr2qs("### Log session started ###"));
	add2Log(szLogStart, date, KVI_OUT_LOG, true);
	if(bPrependCurBuffer)
	{
		add2Log(__tr2qs("### Existing data buffer:"), date, KVI_OUT_LOG, true);
		QString buffer;
		getTextBuffer(buffer);
		add2Log(buffer, date, -1, false);
		add2Log(__tr2qs("### End of existing data buffer."), date, KVI_OUT_LOG, true);
		m_pLogFile->flush();
	}

	return true;
}

void KviIrcView::add2Log(const QString & szBuffer, const QDateTime & aDate, int iMsgType, bool bPrependDate)
{
	QByteArray tmp;

	if(iMsgType >= 0 && !KVI_OPTION_BOOL(KviOption_boolStripMsgTypeInLogs))
	{
		QString szMessageType = QString("%1 ").arg(iMsgType);

		tmp = szMessageType.toUtf8();

		if(m_pLogFile->write(tmp.data(), tmp.length()) == -1)
			qDebug("WARNING: can't write to the log file.");
	}

	if(bPrependDate)
	{
		QString szDate;
		QDateTime date = aDate.isValid() ? aDate : QDateTime::currentDateTime();
		switch(KVI_OPTION_UINT(KviOption_uintOutputDatetimeFormat))
		{
			case 0:
				szDate = date.toString("[hh:mm:ss] ");
				break;
			case 1:
				szDate = date.toString(Qt::ISODate);
				if (date.timeSpec() == Qt::LocalTime)
				{
					// Log milliseconds. QDateTime.fromString can parse them already.
					// However, the format is more complicated if a timezone is present,
					// so only log them for local time.
					szDate += date.toString(".zzz");
				}
				szDate += " ";
				break;
			case 2:
				szDate = date.toString(Qt::SystemLocaleShortDate);
				szDate += " ";
				break;
		}

		tmp = szDate.toUtf8();

		if(m_pLogFile->write(tmp.data(), tmp.length()) == -1)
			qDebug("WARNING: can't write to the log file.");
	}

	tmp = szBuffer.toUtf8();
	tmp.append('\n');

	if(m_pLogFile->write(tmp.data(), tmp.length()) == -1)
		qDebug("WARNING: can't write to the log file.");
}
