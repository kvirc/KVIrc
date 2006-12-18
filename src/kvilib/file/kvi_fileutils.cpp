//=============================================================================
//
//   File : kvi_fileutils.cpp
//   Creation date : Fri Dec 25 1998 18:26:48 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1998-2004 Szymon Stefanek (pragma at kvirc dot net)
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


#define _KVI_DEBUG_CHECK_RANGE_
#include "kvi_debug.h"

#define _KVI_FILEUTLIS_CPP_
#include "kvi_fileutils.h"
#include "kvi_qstring.h"

#include "kvi_malloc.h"

#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qglobal.h>
#include <qtextcodec.h>


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
				szBuffer.setLength(lastlen + cur_len);
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
			szBuffer.setLength(lastlen + cur_len);
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
		szPath.stripWhiteSpace();
		QString dir = szPath;
		adjustFilePath(dir);
		QString createdDir;

#ifdef COMPILE_ON_WINDOWS
		int idx = dir.find(':');
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
					debug("Can't create directory %s",createdDir.utf8().data());
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
		QFile f1(szSrc);
		if(!f1.open(IO_ReadOnly))return false;
		QFile f2(szDst);
		if(!f2.open(IO_WriteOnly|IO_Truncate))
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
		QFile f(szPath);
		if(!f.open(IO_ReadOnly))return false;
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
		while(szPath.find("\\\\") != -1)szPath.replace("\\\\","\\");
		// FIXME: Use the default drive here ?
		if(szPath.startsWith("\\"))szPath.prepend("C:");
#else
		szPath.replace('\\',"/");
		while(szPath.find("//") != -1)szPath.replace("//","/");
		// deal with windows paths
		if((szPath.length() > 2) && (szPath.at(0) != QChar('/')))
		{
			if((szPath.at(1) == QChar(':')) && (szPath.at(2) == QChar('/')))
			{
				szPath.remove(0,2);
			}
		}
		szPath=QDir::cleanDirPath(szPath);
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
		return d.remove(szPath,true);
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

	
	bool writeFile(const QString &szPath,const QString &szData,bool bAppend)
	{
		QFile f(szPath);
		int iFlags = IO_WriteOnly;
		if(bAppend)iFlags |= IO_Append;
		else iFlags |= IO_Truncate;
		if(!f.open(iFlags))return false;
		QCString szTmp = szData.utf8();
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
		QFile f(szPath);
		int iFlags = IO_WriteOnly;
		if(bAppend)iFlags |= IO_Append;
		else iFlags |= IO_Truncate;
		if(!f.open(iFlags))return false;
		QCString szTmp = QTextCodec::codecForLocale()->fromUnicode(szData);
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
		QFile f(szPath);
		if(!f.open(IO_ReadOnly))return false;
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
		return QFileInfo(szFileNameWithPath).absFilePath();
	}

	bool readLine(QFile * f,QString &szBuffer,bool bUtf8)
	{
		QTextStream stream(f);
		szBuffer=stream.readLine();
		return !szBuffer.isNull();
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

//================ kvi_askFor*Name ============//
//
//	THIS STUFF IS DEAD: NOBODY USES IT
//#include <qfiledialog.h>
//#warning "These functions should die in favr of g_pApp->askFor*Name"
//
//QString kvi_askForDirectoryName(const char *basePath)
//{
//	return QFileDialog::getExistingDirectory(basePath);
//}
//
//QString kvi_askForOpenFileName(const char *basePath,const char *filter)
//{
//	return QFileDialog::getOpenFileName(basePath,filter);
//}
//
//QStringList kvi_askForOpenFileNames(const char *basePath,const char *filter)
//{
//	return QFileDialog::getOpenFileNames(filter,basePath);
//}
//
//QString kvi_askForSaveFileName(const char *baseFileName)
//{
//	return QFileDialog::getSaveFileName(baseFileName);
//}
//

void kvi_adjustFilePath(KviStr & path)
{
#ifdef COMPILE_ON_WINDOWS
	path.replaceAll("/","\\");
	while(path.findFirstIdx("\\\\") != -1)path.replaceAll("\\\\","\\");
	if(path.firstCharIs('\\'))path.prepend("C:"); // use the default drive
#else
	path.replaceAll("\\","/");
	while(path.findFirstIdx("//") != -1)path.replaceAll("//","/");
	if((*(path.ptr()) != '/') && (path.len() > 2))
	{
		if((*(path.ptr() + 1) == ':') && (*(path.ptr() + 2) == '/'))
		{
			path.cutLeft(2);
		}
	}
#endif
}

void kvi_adjustFilePath(QString & path)
{
#ifdef COMPILE_ON_WINDOWS
	path.replace("/","\\");
	while(path.find("\\\\") != -1)path.replace("\\\\","\\");
	if( path[0]== ('\\') )path.prepend("C:"); // use the default drive
#else
	path.replace("\\","/");
	while(path.find("//") != -1)path.replace("//","/");
	if((*(path.utf8().data()) != '/') && (path.length() > 2))
	{
		if((*(path.utf8().data() + 1) == ':') && (*(path.utf8().data() + 2) == '/'))
		{
			path = path.right(path.length()-2);
		}
	}
#endif
}

static Q_UINT32 replacetable[8] = 
{
	0xFFFFFFFF,
	0xD4008424,
	0x10000000,
	0x10000000,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF
};

static char hexchars[16] = { '0' , '1' , '2' , '3' , '4' , '5' , '6' , '7' , '8' , '9' , 'A' , 'B' , 'C' , 'D' , 'E' , 'F' };

/*void kvi_encodeFileName(KviStr & path)
{
	char * aux = path.ptr();

	char * repl = (char *)kvi_malloc((sizeof(char) * path.len() * 3) + 1);

	char * dst = repl;

	while(*aux)
	{
		//if(replacetable[*((unsigned char *)aux) / 32] & ((Q_UINT32)(1 << (*((unsigned char *)aux) % 32))))
		if( (*aux)<32 || !QChar(*aux).isPrint() )
		{
			*dst++ = '%';
			*dst++ = hexchars[*((unsigned char *)aux) >> 4];
			*dst++ = hexchars[*((unsigned char *)aux) & 15];
		} else {
			*dst++ = tolower(*aux);
		}

		aux++;
	}

	*dst = '\0';

	path = repl;

	kvi_free(repl);
}

void kvi_encodeFileName(QString & path)
{
	char * aux = path.utf8().data();

	char * repl = (char *)kvi_malloc((sizeof(char) * path.length() * 3) + 1);

	char * dst = repl;

	while(*aux)
	{
		//if(replacetable[*((unsigned char *)aux) / 32] & ((Q_UINT32)(1 << (*((unsigned char *)aux) % 32))))
		if( (*aux)<32 || !QChar(*aux).isPrint() )
		{
			*dst++ = '%';
			*dst++ = hexchars[*((unsigned char *)aux) >> 4];
			*dst++ = hexchars[*((unsigned char *)aux) & 15];
		} else {
			*dst++ = tolower(*aux);
		}

		aux++;
	}

	*dst = '\0';

	path = repl;

	kvi_free(repl);
}*/

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
		if( ! (cur.isLetter() || cur.isDigit() || cur==" " || cur=="_" || cur=="." || cur=="#" || cur=="%") )
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
		} else if (cur=="%"){
			path+="%%";
		} else {
			path+=cur;
		}
	}
}

//================ kvi_isAbsolutePath ===============//

bool kvi_isAbsolutePath(const char *path)
{
	__range_valid(path);
	if(*path == '/')return true;
	if(isalpha(*path))
	{
		if((*(path + 1)) == ':')return true;
	}
	return false;
}

//============ kvi_directoryExists ================//

/*bool kvi_directoryExists(const char *path)
{
	__range_valid(path);
	QFileInfo f(QTextCodec::codecForLocale()->fromUnicode(path));
	return (f.exists() && f.isDir());
}*/

//============ kvi_fileExists ================//

/*bool kvi_fileExists(const char *path)
{
	__range_valid(path);
	QFileInfo f(QTextCodec::codecForLocale()->fromUnicode(path));
	return (f.exists() && f.isFile());
}*/

/*bool kvi_removeFile(const char * path)
{
	__range_valid(path);
	QDir d;
	return d.remove(QTextCodec::codecForLocale()->fromUnicode(path),true);
}*/

/*
bool kvi_removeDir(const char * path)
{
	__range_valid(path);
	QDir d;
	return d.rmdir(path);
}
*/

//============= kvi_fileIsReadable =============//

/*bool kvi_fileIsReadable(const char *path)
{
	__range_valid(path);
	QFileInfo f(QTextCodec::codecForLocale()->fromUnicode(path));
	return (f.exists() && f.isFile() && f.isReadable());
}*/

//================ kvi_makeDir ================//
/*
bool kvi_makeDir(const char *path)
{
	__range_valid(path);
	QDir d;
	KviStr dir(path);
	dir.stripWhiteSpace();
	kvi_adjustFilePath(dir);
	KviStr createdDir("");
#ifndef COMPILE_ON_WINDOWS
	dir.stripLeft(KVI_PATH_SEPARATOR_CHAR);
#else
	int idx = dir.findFirstIdx(":");
	if(idx == 1)
	{
		KviStr drive = dir.left(2);
		dir.cutLeft(2);
		createdDir = drive;
	}
	dir.stripLeft(KVI_PATH_SEPARATOR_CHAR);
#endif
	while(dir.hasData())
	{
		createdDir+=KVI_PATH_SEPARATOR;
		createdDir+=dir.getToken(KVI_PATH_SEPARATOR_CHAR);
		if(!kvi_directoryExists(createdDir.m_ptr))
		{
			if(!d.mkdir(createdDir.m_ptr)){
				debug("Can't create directory %s",createdDir.m_ptr);
				return false;
			}
		}
		dir.stripLeft(KVI_PATH_SEPARATOR_CHAR);
	}
	return true;
}
*/
//=================== kvi_readLine =====================//

bool kvi_readLine(QFile *f,KviStr &str)
{
	QTextStream stream(f);
	QString szBuff=stream.readLine();
	str=szBuff;
	return szBuff.isNull() ? 1 : 0;
}

//==================== kvi_readStrippedLine =====================//

bool kvi_readStrippedLine(QFile *f,KviStr &str)
{
	bool bRet = kvi_readLine(f,str);
	str.stripWhiteSpace();
	return bRet;
}

//==================== kvi_readFirstNonEmptyStrippedLine =========================//

// FIXME: #warning "This is used only by kvi_config.cpp (1 occurence) and opw_servers.cpp (1 occurence)"
// FIXME: #warning "Maybe remove it ? (It is a bad concept at all)"

bool kvi_readFirstNonEmptyStrippedLine(QFile *f,KviStr &str)
{
	bool bContinue;
	do{
		bContinue = kvi_readStrippedLine(f,str);
	} while(bContinue && str.isEmpty());
	return bContinue;
}

//=================== kvi_writeLine =======================//

bool kvi_writeLine(QFile *f,const char *line)
{
	bool bRet = (f->writeBlock(line,(Q_ULONG)strlen(line)) != -1);
	return bRet ? (f->putch('\n') != -1) : false;
}

//=================== kvi_loadFile =======================//

/*bool kvi_loadFile(const char *path,KviStr &buffer)
{
	QFile f(QTextCodec::codecForLocale()->fromUnicode(path));
	if(!f.open(IO_ReadOnly))return false;
	buffer = "";
	KviStr tmp;
	while(kvi_readLine(&f,tmp))
	{
		buffer.append(tmp);
		buffer.append('\n'); // kvi_readLine returned true...last char was a newline
	}
	// kvi_readLine returned false , no ending newline encountered
	// but tmp may still contain some data
	if(tmp.hasData())buffer.append(tmp);
	return true;
}*/

/*bool kvi_writeFile(const char *path,KviStr &buffer,bool bAppend)
{
	QFile f(QTextCodec::codecForLocale()->fromUnicode(path));
	int iFlags = IO_WriteOnly;
	if(bAppend)iFlags |= IO_Append;
	else iFlags |= IO_Truncate;
	if(!f.open(iFlags))return false;
	if(f.writeBlock(buffer.ptr(),buffer.len()) != buffer.len())return false;
	f.close();
	return true;
}*/

/*
bool kvi_copyFile(const char *src,const char *dst)
{
	QFile f1(src);
	if(!f1.open(IO_ReadOnly))return false;
	QFile f2(dst);
	if(!f2.open(IO_WriteOnly|IO_Truncate)){
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

bool kvi_renameFile(const char *src,const char *dst)
{
	QDir d;
	return d.rename(QString(src),QString(dst));
}
*/
