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

#include "kvi_settings.h"
#include "KviApplication.h"
#include "KviControlCodes.h"
#include "KviQString.h"
#include "KviCString.h"
#include "KviHtmlGenerator.h"
#include "KviIconManager.h"
#include "KviLocale.h"
#include "KviOptions.h"
#include "KviFileUtils.h"
#include "KviRegExp.h"

#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QLocale>
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
#include <QTextCodec>
#else
#include <QStringConverter>
#endif

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
			m_date = QLocale().toDate(szDate, QLocale::ShortFormat);
			if(!m_date.isValid())
			{
				// some locale date formats use '/' as a separator; we change them to '-'
				// when creating log files. Try to reverse that change here
				QString szUnescapedDate = szDate;
				szUnescapedDate.replace('-', KVI_PATH_SEPARATOR_CHAR);
				m_date = QLocale().toDate(szUnescapedDate, QLocale::ShortFormat);
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
				m_date = QLocale().toDate(szDate, QLocale::ShortFormat);
				if(!m_date.isValid())
				{
					// some locale date formats use '/' as a separator; we change them to '-'
					// when creating log files. Try to reverse that change here
					QString szUnescapedDate = szDate;
					szUnescapedDate.replace('-', KVI_PATH_SEPARATOR_CHAR);
					m_date = QLocale().toDate(szUnescapedDate, QLocale::ShortFormat);
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

void LogFile::getText(QString & szText) const
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

void LogFile::createLog(ExportType exportType, QString szLog, QString * pszFile) const
{
	KviRegExp rx;
	QString szLogDir, szInputBuffer, szOutputBuffer, szLine, szTmp;
	QString szDate = date().toString("yyyy.MM.dd");

	/* Save export directory - this directory path is also used in the HTML export
	 * and info is used when working with pszFile */
	QFileInfo info(szLog);
	szLogDir = info.absoluteDir().absolutePath() + QString(KVI_PATH_SEPARATOR_CHAR);

	/* Reading in log file - LogFiles are read in as bytes, so '\r' isn't
	 * sanitised by default */
	getText(szInputBuffer);
	QStringList lines = szInputBuffer.replace('\r', "").split('\n');

	switch(exportType)
	{
		case LogFile::PlainText:
		{
			/* Only append extension if it isn't there already (e.g. a specific
			 * file is to be overwritten) */
			if(!szLog.endsWith(".txt"))
				szLog += ".txt";

			// Scan the file
			for(auto & line : lines)
			{
				szTmp = line;
				szLine = KviControlCodes::stripControlBytes(szTmp);

				// Remove icons' code
				rx.setPattern("^\\d{1,3}\\s");
				szLine.replace(rx, "");

				// Remove link from a user speaking, deal with (and keep) various ranks
				// e.g.: <!ncHelLViS69>  -->  <HelLViS69>
				rx.setPattern("\\s<([+%@&~!]?)!nc");
				szLine.replace(rx, " <\\1");

				// Remove link from a nick in a mask
				// e.g.: !nFoo [~bar@!hfoo.bar]  -->  Foo [~bar@!hfoo.bar]
				rx.setPattern("\\s!n");
				szLine.replace(rx, " ");

				// Remove link from a host in a mask
				// e.g.: Foo [~bar@!hfoo.bar]  -->  Foo [~bar@foo.bar]
				rx.setPattern("@!h");
				szLine.replace(rx, "@");

				// Remove link from a channel
				// e.g.: !c#KVIrc  -->  #KVIrc
				rx.setPattern("!c#");
				szLine.replace(rx, "#");

				szOutputBuffer += szLine;
				szOutputBuffer += "\n";
			}

			break;
		}
		case LogFile::HTML:
		{
			/* Only append extension if it isn't there already (e.g. a specific
			 * file is to be overwritten) */
			if(!szLog.endsWith(".html"))
				szLog += ".html";

			szTmp = QString("KVIrc %1 %2").arg(KVI_VERSION).arg(KVI_RELEASE_NAME);
			QString szNick = "";
			bool bFirstLine = true;

			QString szTitle;
			switch(type())
			{
				case LogFile::Channel:
					szTitle = __tr2qs_ctx("Channel %1 on %2", "log").arg(name(), network());
					break;
				case LogFile::Console:
					szTitle = __tr2qs_ctx("Console on %1", "log").arg(network());
					break;
				case LogFile::Query:
					szTitle = __tr2qs_ctx("Query with: %1 on %2", "log").arg(name(), network());
					break;
				case LogFile::DccChat:
					szTitle = __tr2qs_ctx("DCC Chat with: %1", "log").arg(name());
					break;
				case LogFile::Other:
					szTitle = __tr2qs_ctx("Something on: %1", "log").arg(network());
					break;
			}

			// Prepare HTML document
			szOutputBuffer += "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n";
			szOutputBuffer += "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\">\n";
			szOutputBuffer += "<head>\n";
			szOutputBuffer += "\t<meta http-equiv=\"content-type\" content=\"application/xhtml+xml; charset=utf-8\" />\n";
			szOutputBuffer += "\t<meta name=\"author\" content=\"" + szTmp + "\" />\n";
			szOutputBuffer += "\t<title>" + szTitle + "</title>\n";
			szOutputBuffer += "</head>\n<body>\n";
			szOutputBuffer += "<h2>" + szTitle + "</h2>\n<h3>Date: " + szDate + "</h3>\n";

			// Scan the file
			for(auto & line : lines)
			{
				szTmp = line;

				// Find who has talked
				QString szTmpNick = szTmp.section(" ", 2, 2);
				if((szTmpNick.left(1) != "<") && (szTmpNick.right(1) != ">"))
					szTmpNick = "";

				// locate msgtype
				QString szNum = szTmp.section(' ', 0, 0);
				bool bOk;
				int iMsgType = szNum.toInt(&bOk);

				// only human text for now...
				if(iMsgType != 24 && iMsgType != 25 && iMsgType != 26)
					continue;

				// remove msgtype tag
				szTmp = szTmp.remove(0, szNum.length() + 1);

				szTmp = KviHtmlGenerator::convertToHtml(szTmp, true);

				// insert msgtype icon at start of the current text line
				KviMessageTypeSettings msg(KVI_OPTION_MSGTYPE(iMsgType));
				QString szIcon = g_pIconManager->getSmallIconResourceName((KviIconManager::SmallIcon)msg.pixId());
				szTmp.prepend("<img src=\"" + szIcon + R"(" alt="" /> )");

				/*
				 * Check if the nick who has talked is the same of the above line.
				 * If so, we have to put the line as it is, otherwise we have to
				 * open a new paragraph
				 */
				if(szTmpNick != szNick)
				{
					/*
					 * People is not the same, close the paragraph opened
					 * before and open a new one
					 */
					if(!bFirstLine)
						szOutputBuffer += "</p>\n";
					szTmp.prepend("<p>");

					szNick = szTmpNick;
				}
				else
				{
					// Break the line
					szTmp.prepend("<br />\n");
				}

				// remove internal tags before nickname
				rx.setPattern(">([+%@&~!]?)!nc");
				szTmp.replace(rx, ">\\1");
				szTmp.replace(">&amp;!nc", ">&amp;");

				szOutputBuffer += szTmp;
				bFirstLine = false;
			}

			// Close the last paragraph
			szOutputBuffer += "</p>\n";

			// regexp to search all embedded icons
			rx.setPattern("<img src=\"smallicons:([^\"]+)");
			int iIndex = szOutputBuffer.indexOf(rx);
			QStringList szImagesList;

			// search for icons
			while(iIndex >= 0)
			{
				int iLength = rx.matchedLength();
				QString szCap = rx.cap(1);

				// if the icon isn't in the images list then add
				if(szImagesList.indexOf(szCap) == -1)
					szImagesList.append(szCap);
				iIndex = szOutputBuffer.indexOf(rx, iIndex + iLength);
			}

			// get current theme path
			QString szCurrentThemePath;
			g_pApp->getLocalKvircDirectory(szCurrentThemePath, KviApplication::Themes, KVI_OPTION_STRING(KviOption_stringIconThemeSubdir));
			szCurrentThemePath += KVI_PATH_SEPARATOR_CHAR;

			// current coresmall path
			szCurrentThemePath += "coresmall";
			szCurrentThemePath += KVI_PATH_SEPARATOR_CHAR;

			// check if coresmall exists in current theme
			if(!KviFileUtils::directoryExists(szCurrentThemePath))
			{
				// get global coresmall path
				g_pApp->getGlobalKvircDirectory(szCurrentThemePath, KviApplication::Pics, "coresmall");
				KviQString::ensureLastCharIs(szCurrentThemePath, QChar(KVI_PATH_SEPARATOR_CHAR));
			}

			// copy all icons to the log destination folder
			for(int i = 0; i < szImagesList.count(); i++)
			{
				QString szSourceFile = szCurrentThemePath + szImagesList.at(i);
				QString szDestFile = szLogDir + szImagesList.at(i);
				KviFileUtils::copyFile(szSourceFile, szDestFile);
			}

			// remove internal tags
			rx.setPattern("<qt>|</qt>|smallicons:");
			szOutputBuffer.replace(rx, "");

			// Close the document
			szOutputBuffer += "</body>\n</html>\n";

			break;
		}
	}

	// File overwriting already dealt with when file path was obtained
	QFile log(szLog);
	if(!log.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	if(pszFile)
	{
		*pszFile = "";
		*pszFile = info.filePath();
	}

	// Ensure we're writing in UTF-8
	QTextStream output(&log);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    output.setCodec(QTextCodec::codecForMib(106));
#else
	output.setEncoding(QStringConverter::Utf8);
#endif
	output << szOutputBuffer;

	// Close file descriptors
	log.close();
}
