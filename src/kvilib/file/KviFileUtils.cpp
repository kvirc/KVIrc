//=============================================================================
//
//   File : KviFileUtils.cpp
//   Creation date : Fri Dec 25 1998 18:26:48 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1998-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#define _KVI_FILEUTLIS_CPP_

#include "KviFileUtils.h"
#include "KviQString.h"
#include "KviFile.h"
#include "KviMemory.h"

#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <QtGlobal>
#include <QTextCodec>
#include <QTextStream>

namespace KviFileUtils
{
	static char cHexChars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

	bool makeDir(const QString & szPath)
	{
		QDir d;
		QString szDir = szPath.trimmed();
		adjustFilePath(szDir);
		QString szCreatedDir;

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		int idx = szDir.indexOf(':');
		if(idx == 1)
		{
			szCreatedDir = szDir.left(2);
			szDir.remove(0, 2);
		}
#endif

		KviQString::stripLeft(szDir, KVI_PATH_SEPARATOR_CHAR);
		while(!szDir.isEmpty())
		{
			szCreatedDir += KVI_PATH_SEPARATOR;
			szCreatedDir += KviQString::getToken(szDir, KVI_PATH_SEPARATOR_CHAR);
			if(!directoryExists(szCreatedDir))
			{
				if(!d.mkdir(szCreatedDir))
				{
					qDebug("Can't create directory %s", szCreatedDir.toUtf8().data());
					return false;
				}
			}
			KviQString::stripLeft(szDir, KVI_PATH_SEPARATOR_CHAR);
		}
		return true;
	}

	bool makeDir(const char * pcPath)
	{
		QString szPath = QString::fromUtf8(pcPath);
		return makeDir(szPath);
	}

	bool renameFile(const QString & szSrc, const QString & szDst)
	{
		QDir d;
		return d.rename(szSrc, szDst);
	}

	bool renameFile(const char * pcSrc, const char * pcDst)
	{
		QString szPath = QString::fromUtf8(pcSrc);
		QString szPath2 = QString::fromUtf8(pcDst);
		return renameFile(szPath, szPath2);
	}

	bool copyFile(const QString & szSrc, const QString & szDst)
	{
		KviFile f1(szSrc);
		if(!f1.open(QFile::ReadOnly))
			return false;
		KviFile f2(szDst);
		if(!f2.open(QFile::WriteOnly | QFile::Truncate))
		{
			f1.close();
			return false;
		}
		char cBuffer[1024];
		while(!f1.atEnd())
		{
			int iLen = f1.read(cBuffer, 1024);
			if(iLen <= 0)
			{
				f1.close();
				f2.close();
				return false; //"serious error"
			}
			f2.write(cBuffer, iLen);
		}
		f1.close();
		f2.close();
		return true;
	}

	bool copyFile(const char * pcSrc, const char * pcDst)
	{
		QString szPath = QString::fromUtf8(pcSrc);
		QString szPath2 = QString::fromUtf8(pcDst);
		return copyFile(szPath, szPath2);
	}

	bool loadFile(const QString & szPath, QString & szBuffer, bool bUtf8)
	{
		KviFile f(szPath);
		if(!f.open(QFile::ReadOnly))
			return false;
		if(bUtf8)
		{
			QByteArray ba = f.readAll();
			szBuffer = QString::fromUtf8(ba.data(), ba.size());
			//qDebug("BUFFERLEN: %d",szBuffer.length());
		}
		else
		{
			szBuffer = QString(f.readAll());
		}
		return true;
	}

	void adjustFilePath(QString & szPath)
	{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		szPath.replace('/', "\\");
		szPath.replace("\\\\", "\\");
		// FIXME: Use the default drive here ?
		if(szPath.startsWith("\\"))
			szPath.prepend("C:");
#else
		szPath.replace('\\', "/");
		szPath.replace("//", "/");
		// deal with windows paths
		if((szPath.length() > 2) && (szPath.at(0) != QChar('/')))
		{
			if((szPath.at(1) == QChar(':')) && (szPath.at(2) == QChar('/')))
			{
				szPath.remove(0, 2);
			}
		}
		szPath = QDir::cleanPath(szPath);
#endif
	}

	bool removeFile(const QString & szPath)
	{
		QDir d;
		return d.remove(szPath);
	}

	bool removeFile(const char * pcPath)
	{
		QString szPath = QString::fromUtf8(pcPath);
		return removeFile(szPath);
	}

	bool removeDir(const QString & szPath)
	{
		QDir d;
		return d.rmdir(szPath);
	}

	bool removeDir(const char * pcPath)
	{
		QString szPath = QString::fromUtf8(pcPath);
		return removeDir(szPath);
	}

	bool deleteDir(const QString & szPath)
	{
		QDir d(szPath);

		QFileInfoList lFileInfo = d.entryInfoList(QDir::Files | QDir::Dirs | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot);

		for(const auto & inf : lFileInfo)
		{
			// just to be sure check that we're not deleting ..
			if(KviQString::equalCS(inf.fileName(), "..") || KviQString::equalCS(inf.fileName(), "."))
				continue;

			if(inf.isDir())
			{
				if(!deleteDir(inf.absoluteFilePath()))
					return false;
				continue;
			}

			if(!KviFileUtils::removeFile(inf.absoluteFilePath()))
			{
				qDebug("Failed to remove file %s", inf.absoluteFilePath().toUtf8().data());
				return false;
			}
		}

		if(!KviFileUtils::removeDir(szPath))
		{
			qDebug("Failed to remove directory %s", szPath.toUtf8().data());
			return false;
		}

		return true;
	}

	bool writeFile(const QString & szPath, const QByteArray & oData, bool bAppend)
	{
		KviFile f(szPath);
		if(!f.open(QFile::WriteOnly | (bAppend ? QFile::Append : QFile::Truncate)))
			return false;
		if(oData.length() < 1)
			return true;
		if(f.write(oData.data(), oData.length()) != ((unsigned int)(oData.length())))
			return false;
		return true;
	}

	bool writeFile(const QString & szPath, const QString & szData, bool bAppend)
	{
		KviFile f(szPath);
		if(!f.open(QFile::WriteOnly | (bAppend ? QFile::Append : QFile::Truncate)))
			return false;
		QByteArray szTmp = szData.toUtf8();
		if(f.write(szTmp.data(), szTmp.length()) != ((unsigned int)(szTmp.length())))
			return false;
		return true;
	}

	bool writeFile(const char * pcPath, const QString & szData, bool bAppend)
	{
		QString szPath = QString::fromUtf8(pcPath);
		return writeFile(szPath, szData, bAppend);
	}

	bool writeFileLocal8Bit(const QString & szPath, const QString & szData, bool bAppend)
	{
		KviFile f(szPath);
		if(!f.open(QFile::WriteOnly | (bAppend ? QFile::Append : QFile::Truncate)))
			return false;
		QByteArray szTmp = QTextCodec::codecForLocale()->fromUnicode(szData);
		if(f.write(szTmp.data(), szTmp.length()) != ((unsigned int)(szTmp.length())))
			return false;
		return true;
	}

	bool writeFileLocal8Bit(const char * pcPath, const QString & szData, bool bAppend)
	{
		QString szPath = QString::fromUtf8(pcPath);
		return writeFileLocal8Bit(szPath, szData, bAppend);
	}

	bool readFile(const QString & szPath, QString & szBuffer, unsigned int uMaxSize)
	{
		KviFile f(szPath);
		if(!f.open(QFile::ReadOnly))
			return false;
		if(f.size() < 1)
		{
			szBuffer = "";
			f.close();
			return true;
		}
		if(f.size() > uMaxSize)
			return false;
		char * pcBuf = new char[f.size() + 1];
		if(f.read(pcBuf, f.size()) != ((long int)f.size()))
		{
			delete[] pcBuf;
			pcBuf = nullptr;
			return false;
		}
		pcBuf[f.size()] = '\0';
		szBuffer = QString::fromUtf8(pcBuf);
		delete[] pcBuf;
		pcBuf = nullptr;
		return true;
	}

	bool readFile(const char * pcPath, QString & szBuffer, unsigned int uMaxSize)
	{
		QString szPath = QString::fromUtf8(pcPath);
		return readFile(szPath, szBuffer, uMaxSize);
	}

	QString extractFileName(const QString & szFileNameWithPath, bool bAllowEmpty)
	{
		if(!bAllowEmpty)
		{
			if(szFileNameWithPath.endsWith(KVI_PATH_SEPARATOR))
				return QFileInfo(szFileNameWithPath.left(szFileNameWithPath.length() - 1)).fileName();
		}
		return QFileInfo(szFileNameWithPath).fileName();
	}

	QString extractFilePath(const QString & szFileNameWithPath)
	{
		return QFileInfo(szFileNameWithPath).absolutePath();
	}

	bool readLine(QFile * pFile, QString & szBuffer, bool bUtf8)
	{
		QTextStream stream(pFile);
		// By default, QTextCodec::codecForLocale() is used, and automatic unicode detection is enabled.
		if(bUtf8)
			stream.setCodec(QTextCodec::codecForMib(106));
		/*
		* Name: UTF-8
		* MIBenum: 106
		* from: http://www.iana.org/assignments/character-sets
		*/
		szBuffer = stream.readLine();
		return !szBuffer.isNull();
	}

	bool readLines(QFile * pFile, QStringList & buffer, int iStartLine, int iCount, bool bUtf8)
	{
		QTextStream stream(pFile);
		//please read comments in the readLine(...) method
		if(bUtf8)
			stream.setCodec(QTextCodec::codecForMib(106));

		for(int i = 0; i < iStartLine; i++)
			stream.readLine();

		if(iCount > 0)
		{
			for(; (iCount > 0 && !stream.atEnd()); iCount--)
				buffer.append(stream.readLine());
		}
		else
		{
			while(!stream.atEnd())
			{
				buffer.append(stream.readLine());
			}
		}
		return buffer.count() != 0;
	}

	bool directoryExists(const QString & szPath)
	{
		QFileInfo f(szPath);
		return (f.exists() && f.isDir());
	}

	bool isReadable(const QString & szFname)
	{
		QFileInfo f(szFname);
		return (f.exists() && f.isFile() && f.isReadable());
	}

	bool isReadable(const char * pcPath)
	{
		QString szPath = QString::fromUtf8(pcPath);
		return isReadable(szPath);
	}

	bool isAbsolutePath(const QString & szPath)
	{
		QFileInfo f(szPath);
		return !f.isRelative();
	}

	void encodeFileName(QString & szPath)
	{
		QString szSrc(szPath);
		szPath = "";
		for(const auto & i : szSrc)
		{
			QChar cur = i;
			if(!(cur.isLetter() || cur.isDigit() || cur == ' ' || cur == '_' || cur == '.' || cur == '#' || cur == '%'))
			{
				if(cur.row() != 0)
				{
					szPath += '%';
					szPath += cHexChars[cur.row() >> 4];
					szPath += cHexChars[cur.row() & 15];
				}
				szPath += '%';
				szPath += cHexChars[cur.cell() >> 4];
				szPath += cHexChars[cur.cell() & 15];
			}
			else if(cur == '%')
			{
				szPath += "%%";
			}
			else
			{
				szPath += cur;
			}
		}
	}

	void cleanFileName(QString & szPath)
	{
		QString szSrc(szPath);
		szPath = "";
		for(const auto & i : szSrc)
		{
			QChar cur = i;
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
			if(cur.unicode() < 32 || cur == '<' || cur == '>' || cur == ':' || cur == '"' || cur == '/' || cur == '\\' || cur == '|' || cur == '?' || cur == '*')
#else
			if(cur.unicode() == 0 || cur == '/')
#endif
			{
				szPath += '-';
			}
			else
			{
				szPath += cur;
			}
		}
	}

	static QStringList getFileListingInternal(const QString & szPath, const QString & szPrefix)
	{
		//qDebug("Scan %s",szPath.toUtf8().data());
		QStringList sl;

		QDir d(szPath);

		QFileInfoList fl = d.entryInfoList(QStringList(), QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot, QDir::Name);

		for(const auto & inf : fl)
		{
			QString szName = szPrefix.isEmpty() ? inf.fileName() : QString("%1" KVI_PATH_SEPARATOR "%2").arg(szPrefix).arg(inf.fileName());
			if(inf.isDir())
			{
				QStringList sub = getFileListingInternal(inf.canonicalFilePath(), szName);
				sl.append(sub);
			}
			else
			{
				sl.append(szName);
			}
		}

		return sl;
	}

	QStringList getFileListing(const QString & szPath)
	{
		return getFileListingInternal(szPath, QString());
	}
}
