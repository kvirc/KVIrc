//=============================================================================
//
//   File : kvi_fileutils.cpp
//   Creation date : Fri Dec 25 1998 18:26:48 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1998-2007 Szymon Stefanek (pragma at kvirc dot net)
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

#define __KVILIB__


#define _KVI_FILEUTLIS_CPP_
#include "kvi_fileutils.h"
#include "kvi_qstring.h"
#include "kvi_file.h"
#include "kvi_malloc.h"

#include <QDir>
#include <QFileInfo>
#include <qglobal.h>
#include <QTextCodec>
#include <QTextStream>


namespace KviFileUtils
{
	/*
		WORKING CODE BUT UNUSED FOR NOW
	bool readLine(QFile * f,QString &szBuffer,bool bClearBuffer)
	{
		// FIXME: Should this assume UTF8 encoding ?
		char tmp_buf[256];
		int cur_len = 0;
		//char *cur_ptr = tmp_buf;
		if(bClearBuffer)szBuffer = "";
		int ch = f->getch();
	
		while((ch != -1)&&(ch != '\n')&&(ch != 0))
		{
			tmp_buf[cur_len] = ch;
			cur_len++;
			if(cur_len > 255)
			{
				if(tmp_buf[255] == '\r')cur_len--; //Ignore CR...
				int lastlen = szBuffer.length();
				szBuffer.resize(lastlen + cur_len);
				QChar *p1 = szBuffer.unicode() + lastlen;
				char * p2 = tmp_buf;
				for(int i=0;i<cur_len;i++)*p1++ = *p2++;
				cur_len = 0;
			}
			ch = f->getch();
		}
		if(ch == 0)
		{
			debug("Warning : %s is not an ascii file",f->name().latin1());
		}
		if(cur_len > 0)
		{
			if(tmp_buf[cur_len - 1] == '\r')cur_len--; //Ignore CR...
			int lastlen = szBuffer.length();
			szBuffer.resize(lastlen + cur_len);
			QChar *p1 = szBuffer.unicode() + lastlen;
			char * p2 = tmp_buf;
			for(int i=0;i<cur_len;i++)*p1++ = *p2++;
		}
		return (ch == '\n'); //more data to read else a broken file or EOF
	}

	bool loadFileStripCR(const QString &szPath,QString &szBuffer)
	{
		QFile f(szPath);
		if(!f.open(IO_ReadOnly))return false;
		szBuffer = "";
		while(readLine(&f,szBuffer,false))
		{
			szBuffer.append('\n'); // readLine returned true...last char was a newline
		}
		// readLine returned false , no ending newline encountered
		return true;
	}
	*/

	bool makeDir(const QString &szPath)
	{
		QDir d;
		QString dir = KviQString::trimmed(szPath);
		adjustFilePath(dir);
		QString createdDir;

#ifdef COMPILE_ON_WINDOWS
		int idx = dir.indexOf(':');
		if(idx == 1)
		{
			createdDir = dir.left(2);
			dir.remove(0,2);
		}
#endif

		KviQString::stripLeft(dir,KVI_PATH_SEPARATOR_CHAR);
		while(!dir.isEmpty())
		{
			createdDir += KVI_PATH_SEPARATOR;
			createdDir += KviQString::getToken(dir,KVI_PATH_SEPARATOR_CHAR);
			if(!directoryExists(createdDir))
			{
				if(!d.mkdir(createdDir))
				{
					debug("Can't create directory %s",KviQString::toUtf8(createdDir).data());
					return false;
				}
			}
			KviQString::stripLeft(dir,KVI_PATH_SEPARATOR_CHAR);
		}
		return true;
	}
	
	bool makeDir(const char* path)
	{
		QString szPath=QString::fromUtf8(path);
		return makeDir(szPath);
	}

	bool renameFile(const QString &szSrc,const QString &szDst)
	{
		QDir d;
		return d.rename(szSrc,szDst);
	}

	bool renameFile(const char* path,const char* path2)
	{
		QString szPath=QString::fromUtf8(path);
		QString szPath2=QString::fromUtf8(path2);
		return renameFile(szPath,szPath2);
	}

	bool copyFile(const QString &szSrc,const QString &szDst)
	{
		KviFile f1(szSrc);
		if(!f1.openForReading())return false;
		KviFile f2(szDst);
		if(!f2.openForWriting())
		{
			f1.close();
			return false;
		}
		char buffer[1024];
		while(!f1.atEnd())
		{
			int len = f1.readBlock(buffer,1024);
			if(len <= 0)
			{
				f1.close();
				f2.close();
				return false; //"serious error"
			}
			f2.writeBlock(buffer,len);
		}
		f1.close();
		f2.close();
		return true;
	}

	bool copyFile(const char* path,const char* path2)
	{
		QString szPath=QString::fromUtf8(path);
		QString szPath2=QString::fromUtf8(path2);
		return copyFile(szPath,szPath2);
	}

	bool loadFile(const QString &szPath,QString &szBuffer,bool bUtf8)
	{
		KviFile f(szPath);
		if(!f.openForReading())return false;
		if(bUtf8)
		{
			QByteArray ba = f.readAll();
			szBuffer = QString::fromUtf8(ba.data(),ba.size());
			//debug("BUFFERLEN: %d",szBuffer.length());
		} else {
			szBuffer = QString(f.readAll());
		}
		return true;
	}
	
	bool loadFile(const char* path,QString &szBuffer,bool bUtf8)
	{
		QString szPath=QString::fromUtf8(path);
		return loadFile(szPath,szBuffer,bUtf8);
	}	
	
	void adjustFilePath(QString &szPath)
	{
#ifdef COMPILE_ON_WINDOWS
		szPath.replace('/',"\\");
		szPath.replace("\\\\","\\");
		// FIXME: Use the default drive here ?
		if(szPath.startsWith("\\"))szPath.prepend("C:");
#else
		szPath.replace('\\',"/");
		szPath.replace("//","/");
		// deal with windows paths
		if((szPath.length() > 2) && (szPath.at(0) != QChar('/')))
		{
			if((szPath.at(1) == QChar(':')) && (szPath.at(2) == QChar('/')))
			{
				szPath.remove(0,2);
			}
		}
		szPath=QDir::cleanPath(szPath);
#endif
		
	}
	
	bool directoryExists(const QString &szPath)
	{
		QFileInfo f(szPath);
		return (f.exists() && f.isDir());
	}

	bool directoryExists(const char* path)
	{	
		QString szPath=QString::fromUtf8(path);
		QFileInfo f(szPath);
		return (f.exists() && f.isDir());
	}

	bool fileExists(const QString &szPath)
	{
		QFileInfo f(szPath);
		return (f.exists() && f.isFile());
	}
	
	bool fileExists(const char* path)
	{	
		QString szPath=QString::fromUtf8(path);
		return fileExists(szPath);
	}
	
	bool removeFile(const QString &szPath)
	{
		QDir d;
		return d.remove(szPath);
	}

	bool removeFile(const char* path)
	{
		QString szPath=QString::fromUtf8(path);
		return removeFile(szPath);
	}

	bool removeDir(const QString &szPath)
	{
		QDir d;
		return d.rmdir(szPath);
	}
	
	bool removeDir(const char* path)
	{
		QString szPath=QString::fromUtf8(path);
		return removeDir(szPath);
	}

	bool deleteDir(const QString &szPath)
	{
		QDir d(szPath);
		QStringList sl = d.entryList(QDir::Dirs);
		QStringList::Iterator it;
		for(it=sl.begin();it != sl.end();it++)
		{
			QString szSubdir = *it;
			if(!(KviQString::equalCS(szSubdir,"..") || KviQString::equalCS(szSubdir,".")))
			{
				QString szSubPath = szPath;
				KviQString::ensureLastCharIs(szSubPath,QChar(KVI_PATH_SEPARATOR_CHAR));
				szSubPath += szSubdir;
				if(!KviFileUtils::deleteDir(szSubPath))
					return false;
			}
		}

		sl = d.entryList(QDir::Files);
		for(it=sl.begin();it != sl.end();it++)
		{
			QString szFilePath = szPath;
			KviQString::ensureLastCharIs(szFilePath,QChar(KVI_PATH_SEPARATOR_CHAR));
			szFilePath += *it;
			if(!KviFileUtils::removeFile(szFilePath))
				return false;
		}

		return KviFileUtils::removeDir(szPath);
	}
	
	bool writeFile(const QString &szPath,const QString &szData,bool bAppend)
	{
		KviFile f(szPath);
		if(!f.openForWriting(bAppend))return false;
		KviQCString szTmp = KviQString::toUtf8(szData);
		if(!szTmp.data())return true;
		if(f.writeBlock(szTmp.data(),szTmp.length()) != ((int)(szTmp.length())))return false;
		return true;
	}

	bool writeFile(const char* path,const QString &szData,bool bAppend)
	{
		QString szPath=QString::fromUtf8(path);
		return writeFile(szPath,szData,bAppend);
	}

	bool writeFileLocal8Bit(const QString &szPath,const QString &szData,bool bAppend)
	{
		KviFile f(szPath);
		if(!f.openForWriting(bAppend))return false;
		KviQCString szTmp = QTextCodec::codecForLocale()->fromUnicode(szData);
		if(!szTmp.data())return true;
		if(f.writeBlock(szTmp.data(),szTmp.length()) != ((int)(szTmp.length())))return false;
		return true;
	}
	
	bool writeFileLocal8Bit(const char* path,const QString &szData,bool bAppend)
	{
		QString szPath=QString::fromUtf8(path);
		return writeFileLocal8Bit(szPath,szData,bAppend);
	}

	bool readFile(const QString &szPath,QString &szBuffer,unsigned int uMaxSize)
	{
		KviFile f(szPath);
		if(!f.openForReading())return false;
		if(f.size() < 1)
		{
			szBuffer = "";
			f.close();
			return true;
		}
		if(f.size() > uMaxSize)return false;
		char * buf = new char[f.size() + 1];
		if(f.readBlock(buf,f.size()) != ((long int)f.size()))
		{
			delete buf;
            buf = 0;
			return false;
		}
		buf[f.size()] = '\0';
		szBuffer = QString::fromUtf8(buf);
		delete[] buf;
		return true;
	}

	bool readFile(const char* path,QString &szBuffer,unsigned int uMaxSize)
	{
		QString szPath=QString::fromUtf8(path);
		return readFile(szPath,szBuffer,uMaxSize);
	}

	QString extractFileName(const QString &szFileNameWithPath)
	{
		return QFileInfo(szFileNameWithPath).fileName();
	}

	QString extractFilePath(const QString &szFileNameWithPath)
	{
		return QFileInfo(szFileNameWithPath).dirPath(true);
	}

	bool readLine(QFile * f,QString &szBuffer,bool bUtf8)
	{
		QTextStream stream(f);
		stream.setEncoding(bUtf8 ? QTextStream::UnicodeUTF8 : QTextStream::Locale);
		szBuffer=stream.readLine();
		return !szBuffer.isNull();
	}
	
	bool readLines(QFile * f,QStringList &buffer,int iStartLine, int iCount, bool bUtf8)
	{
		QTextStream stream( f );
		stream.setEncoding(bUtf8 ? QTextStream::UnicodeUTF8 : QTextStream::Locale);
		for(int i=0;i<iStartLine;i++)
			stream.readLine();

		if(iCount>0)
		{
			for(; (iCount>0 && !stream.atEnd()) ; iCount-- )
				buffer.append(stream.readLine());
		} else {
			while(!stream.atEnd()) {
				buffer.append(stream.readLine());
			}
		}
		return buffer.count()!= 0;
	}

	bool isReadable(const QString &szFname)
	{
		QFileInfo f(szFname);
		return (f.exists() && f.isFile() && f.isReadable());
	}
	
	bool isReadable(const char* path)
	{
		QString szPath=QString::fromUtf8(path);
		return isReadable(szPath);
	}
	
	bool isAbsolutePath(const QString &szPath)
	{
		QFileInfo f(szPath);
		return !f.isRelative();
	}
};

static char hexchars[16] = { '0' , '1' , '2' , '3' , '4' , '5' , '6' , '7' , '8' , '9' , 'A' , 'B' , 'C' , 'D' , 'E' , 'F' };


void kvi_encodeFileName(KviStr & path)
{
	QString szPath(path.ptr());
	kvi_encodeFileName(szPath);
	path=szPath;
}

void kvi_encodeFileName(QString & path)
{
	QString src(path);
	path="";
	for(int i=0;i<src.length();i++)
	{
		QChar cur=src[i];
		if( ! (cur.isLetter() || cur.isDigit() || cur==' ' || cur=='_' || cur=='.' || cur=='#' || cur=='%') )
		{
			if(cur.row()!=0)
			{
				path+='%';
				path+=hexchars[cur.row() >> 4];
				path+=hexchars[cur.row() & 15];
			}
			path+='%';
			path+=hexchars[cur.cell() >> 4];
			path+=hexchars[cur.cell() & 15];
		} else if (cur=='%')
		{
			path+="%%";
		} else {
			path+=cur;
		}
	}
}

//================ kvi_isAbsolutePath ===============//

bool kvi_isAbsolutePath(const char *path)
{
	if(*path == '/')return true;
	if(isalpha(*path))
	{
		if((*(path + 1)) == ':')return true;
	}
	return false;
}

//=================== kvi_readLine =====================//

bool kvi_readLine(QFile *f,KviStr &str)
{
	QTextStream stream(f);
	QString szBuff=stream.readLine();
	str=szBuff;
	return szBuff.isNull() ? 1 : 0;
}
