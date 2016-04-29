//=============================================================================
//
//   File : LogFile.cpp
//   Creation date : Thu Apr 14 2011 19:21:59 by Elvio Basello
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2011 Elvio Basello (hellvis69 at gmail dot com)
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

#include "LogFile.h"

#include "KviQString.h"
#include "KviCString.h"
#include "KviOptions.h"
#include "KviFileUtils.h"

#include <QFileInfo>

#ifdef COMPILE_ZLIB_SUPPORT
#include <zlib.h>
#endif

LogFile::LogFile(const QString & szName)
{
	m_szFilename = szName;

	QFileInfo fi(m_szFilename);
	QString szTmpName = fi.fileName();

	m_bCompressed = (fi.suffix() == "gz");
	if(m_bCompressed)
	{
		//removes trailing dot and extension
		szTmpName.chop(3);
	}
	QString szTypeToken = szTmpName.section('_', 0, 0);
	// Ignore non-logs files, this includes '.' and '..'
	if(KviQString::equalCI(szTypeToken, "channel") || KviQString::equalCI(szTypeToken, "deadchannel"))
	{
		m_szType = "channel";
		m_eType = Channel;
	}
	else if(KviQString::equalCI(szTypeToken, "console"))
	{
		m_szType = "console";
		m_eType = Console;
	}
	else if(KviQString::equalCI(szTypeToken, "query"))
	{
		m_szType = "query";
		m_eType = Query;
	}
	else if(KviQString::equalCI(szTypeToken, "dccchat"))
	{
		m_szType = "dccchat";
		m_eType = DccChat;
	}
	else
	{
		m_szType = "";
		m_eType = Other;
	}

	KviCString szUndecoded = szTmpName.section('.', 0, 0);
	szUndecoded.cutToFirst('_');
	m_szName = szUndecoded.hexDecode(szUndecoded.ptr()).ptr();

	szUndecoded = szTmpName.section('.', 1).section('_', 0, -2);
	m_szNetwork = szUndecoded.hexDecode(szUndecoded.ptr()).ptr();

	QString szDate = szTmpName.section('_', -1).section('.', 0, -2);

	switch(KVI_OPTION_UINT(KviOption_uintOutputDatetimeFormat))
	{
		case 1:
			m_date = QDate::fromString(szDate, Qt::ISODate);
			break;
		case 2:
			m_date = QDate::fromString(szDate, Qt::SystemLocaleShortDate);
			if(!m_date.isValid())
			{
				// some locale date formats use '/' as a separator; we change them to '-'
				// when creating log files. Try to reverse that change here
				QString szUnescapedDate = szDate;
				szUnescapedDate.replace('-', KVI_PATH_SEPARATOR_CHAR);
				m_date = QDate::fromString(szUnescapedDate, Qt::SystemLocaleShortDate);
				if(m_date.isValid())
				{
					//qt4 defaults to 1900 for years. So "11" means "1911" instead of "2011".. what a pity
					if(m_date.year() < 1990)
						m_date = m_date.addYears(100);
				}
			}
			break;
		case 0:
		default:
			m_date = QDate::fromString(szDate, "yyyy.MM.dd");
			break;
	}
	if(!m_date.isValid())
	{
		// probably the log has been created when the OutputDatetimeFormat option
		// was set to a different value. Try to guess its format
		m_date = QDate::fromString(szDate, "yyyy.MM.dd");
		if(!m_date.isValid())
		{
			m_date = QDate::fromString(szDate, Qt::ISODate);
			if(!m_date.isValid())
			{
				m_date = QDate::fromString(szDate, Qt::SystemLocaleShortDate);
				if(!m_date.isValid())
				{
					// some locale date formats use '/' as a separator; we change them to '-'
					// when creating log files. Try to reverse that change here
					QString szUnescapedDate = szDate;
					szUnescapedDate.replace('-', KVI_PATH_SEPARATOR_CHAR);
					m_date = QDate::fromString(szUnescapedDate, Qt::SystemLocaleShortDate);
					if(m_date.isValid())
					{
						//qt4 defaults to 1900 for years. So "11" means "1911" instead of "2011".. what a pity
						if(m_date.year() < 1990)
							m_date = m_date.addYears(100);
					}
				}
				// if the date is still not valid, we give up
			}
		}
	}
}

void LogFile::getText(QString & szText)
{
	QString szLogName = fileName();
	QFile logFile;
#ifdef COMPILE_ZLIB_SUPPORT
	if(m_bCompressed)
	{
		gzFile file = gzopen(szLogName.toLocal8Bit().data(), "rb");
		if(file)
		{
			char cBuff[1025];
			int iLen;
			QByteArray data;
			//QCString data;
			iLen = gzread(file, cBuff, 1024);
			while(iLen > 0)
			{
				cBuff[iLen] = 0;
				data.append(cBuff);
				iLen = gzread(file, cBuff, 1024);
			}
			gzclose(file);
			szText = QString::fromUtf8(data);
		}
		else
		{
			qDebug("Can't open compressed file %s", szLogName.toLocal8Bit().data());
		}
	}
	else
	{
#endif
		logFile.setFileName(szLogName);

		if(!logFile.open(QIODevice::ReadOnly))
			return;

		QByteArray bytes;
		bytes = logFile.readAll();
		szText = QString::fromUtf8(bytes.data(), bytes.size());
		logFile.close();
#ifdef COMPILE_ZLIB_SUPPORT
	}
#endif
}
