//=============================================================================
//
//   File : kvi_app_fs.cpp
//   Creation date : Thu Dec 24 1998 04:05:26 by Szymon Stefanek
//
//   This file is part of the Kvirc irc client distribution
//   Copyright (C) 1998-2006 Szymon Stefanek (pragma at kvirc dot net)
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

#define __KVIRC__

#include "kvi_debug.h"
#include "kvi_app.h"
#include "kvi_locale.h"
#include "kvi_fileutils.h"
#include "kvi_mediatype.h"
#include "kvi_env.h"
#include "kvi_time.h"
#include "kvi_options.h"
#include "kvi_defaults.h"
#include "kvi_iconmanager.h"

#include <qdir.h>
#include <qfileinfo.h>

extern KviMediaManager * g_pMediaManager;

//============ getGlobalKvircDirectory ================//

void KviApp::getGlobalKvircDirectory(QString &szData,KvircSubdir dir,const QString &appendFile)
{
	szData=m_szGlobalKvircDir;
	KviQString::ensureLastCharIs(szData,KVI_PATH_SEPARATOR_CHAR);

	switch(dir)
	{
		case None          :                                                                           break;
		case Pics          : szData.append("pics");                                                    break;
		case Modules       :
		case Plugins       : szData.append("modules");                                                 break;
		case ConfigPlugins : KviQString::appendFormatted(szData,"config%smodules",KVI_PATH_SEPARATOR); break;
		case ConfigScripts : KviQString::appendFormatted(szData,"config%sscripts",KVI_PATH_SEPARATOR); break;
		case Help          :
			szData.append("help");
			{
				// Localized help
				QString tmp(KviLocale::localeName().ptr());
				tmp.prepend(KVI_PATH_SEPARATOR_CHAR);
				tmp.prepend(szData);
				if(KviFileUtils::directoryExists(tmp))szData = tmp;
				else {
					tmp = szData; KviQString::appendFormatted(tmp,"%sen",KVI_PATH_SEPARATOR);
					if(KviFileUtils::directoryExists(tmp))szData = tmp;
				}
			}
		break;
		case HelpEN        : KviQString::appendFormatted(szData,"help%sen",KVI_PATH_SEPARATOR);        break;
		case HelpNoIntl    : szData.append("help");                                                    break;
		case Log           : debug("WARNING Global log directory requested!");                         break;
		case Incoming      : debug("WARNING Global incoming directory requested!");                    break;
		case Trash         : debug("WARNING Global trash directory requested!");                       break;
		case Config        : szData.append("config");                                                  break;
		case Audio         : szData.append("audio");                                                   break;
		case Scripts       : szData.append("scripts");                                                 break;
		case MsgColors     : szData.append("msgcolors");                                               break;
		case Charmaps      : szData.append("charmaps");                                                break;
		case Avatars       : szData.append("avatars");                                                 break;
		case DefScript     : szData.append("defscript");                                               break;
		case License       : szData.append("license");                                                 break;
		case Filters       : szData.append("filters");                                                 break;
		case Locale        : szData.append("locale");                                                  break;
		case Tmp           : debug("WARNING Global tmp directory requested!");                         break;
		case Themes        : szData.append("themes");                                                  break;
		case Classes       : szData.append("classes");                                                 break;
		case SmallIcons    : szData.append("pics"KVI_PATH_SEPARATOR KVI_SMALLICONS_SUBDIRECTORY);      break;
	}

	if(!appendFile.isEmpty())
	{
		KviQString::ensureLastCharIs(szData,KVI_PATH_SEPARATOR_CHAR);
		szData.append(appendFile);
	}

	KviFileUtils::adjustFilePath(szData);
}

void KviApp::getLocalKvircDirectory(QString &szData,KvircSubdir dir,const QString & appendFile,bool bCreateIfNeeded)
{
	szData=m_szLocalKvircDir;
	KviQString::ensureLastCharIs(szData,KVI_PATH_SEPARATOR_CHAR);

	switch(dir)
	{
		case None          :                                                                     break;
		case Pics          : szData.append("pics");                                              break;
		case Config        : szData.append("config");                                            break;
		case ConfigPlugins : szData.append("config"KVI_PATH_SEPARATOR"modules");                 break;
		case ConfigScripts : szData.append("config"KVI_PATH_SEPARATOR"scripts");                 break;
		case Log           : szData.append("log");                                               break;
		case Incoming      :
			szData = KVI_OPTION_STRING(KviOption_stringIncomingPath);
			// we should take special care here
			// the user is likely to mess the path behind our back
			// try to recover from most common problems
			while(KviQString::lastCharIs(szData,KVI_PATH_SEPARATOR_CHAR))
				szData = szData.remove(szData.length()-1,1);
			if(szData.isEmpty())
			{
				// unset ?
				KVI_OPTION_STRING(KviOption_stringIncomingPath) = QDir::homeDirPath();
				if(!KVI_OPTION_STRING(KviOption_stringIncomingPath).endsWith(QString(QChar(KVI_PATH_SEPARATOR_CHAR))))
					KVI_OPTION_STRING(KviOption_stringIncomingPath).append(KVI_PATH_SEPARATOR_CHAR);
				KVI_OPTION_STRING(KviOption_stringIncomingPath).append(KVI_DEFAULT_INCOMING_SUBDIRECTORY_NAME);
			}
		break;
		case Help          :
			szData.append("help");
			{
				// Localized help
				QString tmp = KviLocale::localeName().ptr();
				tmp.prepend(KVI_PATH_SEPARATOR_CHAR);
				tmp.prepend(szData);
				if(KviFileUtils::directoryExists(tmp))szData = tmp;
				else {
					tmp = szData; tmp.append(KVI_PATH_SEPARATOR"en");
					if(KviFileUtils::directoryExists(tmp))szData = tmp;
				}
			}
		break;
		case HelpEN        : szData.append("help"KVI_PATH_SEPARATOR"en");                        break;
		case HelpNoIntl    : szData.append("help");                                              break;
		case Audio         : szData.append("audio");                                             break;
		case Scripts       : szData.append("scripts");                                           break;
		case Modules       :
		case Plugins       : szData.append("modules");                                           break;
		case Trash         : szData.append("trash");                                             break;
		case MsgColors     : szData.append("msgcolors");                                         break;
		case Charmaps      : szData.append("charmaps");                                          break;
		case Avatars       : szData.append("avatars");                                           break;
		case License       : szData.append("license");                                           break;
		case Filters       : szData.append("filters");                                           break;
		case Tmp           : szData.append("tmp");                                               break;
		case Locale        : szData.append("locale");                                            break;
		case Themes        : szData.append("themes");                                            break;
		case DefScript     : szData.append("defscript");  /* should never happen! */             break;
		case Classes       : szData.append("classes");                                           break;
		case SmallIcons    : szData.append("pics"KVI_PATH_SEPARATOR KVI_SMALLICONS_SUBDIRECTORY); break;
	}
	if(bCreateIfNeeded)KviFileUtils::makeDir(szData);

	if(!appendFile.isEmpty())
	{
		KviQString::ensureLastCharIs(szData,KVI_PATH_SEPARATOR_CHAR);
		szData.append(appendFile);
	}

	KviFileUtils::adjustFilePath(szData);
}


void KviApp::getGlobalKvircDirectory(KviStr &szData,KvircSubdir dir,const QString &appendFile)
{
	QString szBuffer = szData.ptr();
	getGlobalKvircDirectory(szBuffer,dir,appendFile);
	szData=szBuffer;
	
}

//=============== getLocalKvircDirectory ================//

void KviApp::getLocalKvircDirectory(KviStr &szData,KvircSubdir dir,const QString &appendFile,bool bCreateIfNeeded)
{
	QString szBuffer = szData.ptr();
	getLocalKvircDirectory(szBuffer,dir,appendFile,bCreateIfNeeded);
	szData=szBuffer;
}

void KviApp::getTmpFileName(KviStr &szBuffer,const char * pattern)
{
	KviStr tmp;
	getLocalKvircDirectory(tmp,Tmp);
	tmp.ensureLastCharIs(KVI_PATH_SEPARATOR_CHAR);

	struct timeval tmv;
	kvi_gettimeofday(&tmv,0);

	do {
		szBuffer.append(KviStr::Format,"kvirc_%d_%s",tmv.tv_usec,pattern ? pattern : "file.tmp");
		tmv.tv_usec++;
	} while(KviFileUtils::fileExists(szBuffer.ptr()));
}

//====================== trashFile ====================//
//
// Moves a file to the local trash directory
//

bool KviApp::trashFile(const char *filename)
{
	// Exists ?
	QFileInfo fi = QFileInfo(QString::fromUtf8(filename));
	if( !fi.exists() ) return false; // nothing to trash

	// Build the filename
	KviStr lastPart = filename;
	lastPart.cutToLast(KVI_PATH_SEPARATOR_CHAR); // need only the filename
	lastPart.append(".bak_");
	QDateTime tm(QDateTime::currentDateTime());
	lastPart.append(tm.toString());
	lastPart.replaceAll(' ',"_");
	// Ok...have lastPart.bak_Dec_11_31_1999
	// Find the thrash directory (make it if necessary)
	KviStr trashFile;
	getLocalKvircDirectory(trashFile,Trash,lastPart.ptr(),true);
	// Check if a such file already exists
	fi.setFile(trashFile.ptr());
	while(fi.exists())
	{
		trashFile.append(".rnm");
		fi.setFile(trashFile.ptr());
	}
	// rename the file
	return KviFileUtils::renameFile(filename,trashFile.ptr());
}

void KviApp::completeDirectory(const QString &word,KviPtrList<QString> * matches)
{
	QString szDir = word;
	QString szFile = word;
	KviQString::cutToLast(szFile,KVI_PATH_SEPARATOR_CHAR);
	KviQString::cutFromLast(szDir,KVI_PATH_SEPARATOR_CHAR);
	KviFileUtils::adjustFilePath(szDir);
	KviQString::ensureLastCharIs(szDir,KVI_PATH_SEPARATOR_CHAR);
	QDir d(szDir);

	// FIXME: maybe check timestamps ? (old modules)
	QStringList sl = d.entryList(QDir::Dirs);
	QStringList::Iterator it;

	for(it = sl.begin();it != sl.end();++it)
	{
		QString * t = new QString(*it);
		if(KviQString::equalCS(*t,".") || KviQString::equalCS(*t,".."))
		{
			delete t;
		} else {
#ifdef COMPILE_ON_WINDOWS
			if(KviQString::equalCIN(szFile,*t,szFile.length()))
#else //!COMPILE_ON_WINDOWS
			if(KviQString::equalCSN(szFile,*t,szFile.length()))
#endif //COMPILE_ON_WINDOWS
			{
				t->prepend(szDir);
				t->append(KVI_PATH_SEPARATOR_CHAR);
				matches->append(t);
			} else {
				delete t;
			}
		}
	}

	sl = d.entryList(QDir::Files);

	for(it = sl.begin();it != sl.end();++it)
	{
		QString * t = new QString(*it);
#ifdef COMPILE_ON_WINDOWS
			if(KviQString::equalCIN(szFile,*t,szFile.length()))
#else //!COMPILE_ON_WINDOWS
			if(KviQString::equalCSN(szFile,*t,szFile.length()))
#endif //COMPILE_ON_WINDOWS
		{
			t->prepend(szDir);
			matches->append(t);
		} else {
			delete t;
		}
	}
}


//============= getChannelDumpLogFileName ==============//
//
// Returns a valid filename for the channel dump log
//

void KviApp::getChannelDumpLogFileName(KviStr &str)
{
	unsigned int logN = 0;
	KviStr fPrefix="kick_";
	do{
		KviStr fName(KviStr::Format,"%s%u.log",fPrefix.ptr(),logN);
		getLocalKvircDirectory(str,Log,fName.ptr());
		logN++;
		if(logN > 9999)
		{
			// Time to clear logs! :)
			fPrefix+="x";
			logN = 0;
		}
	} while(KviFileUtils::fileExists(str.ptr()));
	kvi_adjustFilePath(str);
}

bool KviApp::findFileByMediaType(KviStr &szRetPath,const char * filename)
{
	g_pMediaManager->lock();
	KviMediaType * m = g_pMediaManager->findMediaType(filename,false);

	if(m)
	{
		if(m->szSavePath.hasData())
		{
			szRetPath = m->szSavePath;
			szRetPath.ensureLastCharIs(KVI_PATH_SEPARATOR_CHAR);
			szRetPath.append(filename);
			if(KviFileUtils::fileExists(szRetPath.ptr()))
			{
				// check again the media type... check the magic too
				KviMediaType * mt = g_pMediaManager->findMediaType(szRetPath.ptr());
				if(mt == m)
				{
					g_pMediaManager->unlock();
					//if(retMediaType)*retMediaType = mt;
					kvi_adjustFilePath(szRetPath);
					return true;
				} // else mime type not matched...we should not be looking there!
			}
		}
	}

	g_pMediaManager->unlock();
	return false;
}

//===================== findUserFile =====================//
//
// Looks for an user file in the standard directories
//

bool KviApp::findUserFile(KviStr &szRetPath,const char *filename)
{
	static KviApp::KvircSubdir localsubdirs[5]={ Avatars , Incoming , Pics , Audio , Log };
	static KviApp::KvircSubdir globalsubdirs[3]={ Avatars , Pics , Audio };

	// Absolute paths can not be "found"... they are obvious
	if(kvi_isAbsolutePath(filename))
	{
		szRetPath=filename;
		kvi_adjustFilePath(szRetPath);
		return KviFileUtils::fileExists(filename);
	}

	// FIXME: #warning "Use the user defined path ?"

	// lookup the magic by name match only...

	if(findFileByMediaType(szRetPath,filename))return true;

	int i;

	for(i= 0;i<5; i++)
	{
		getLocalKvircDirectory(szRetPath,localsubdirs[i],filename);
		if(KviFileUtils::fileExists(szRetPath.ptr()))return true;
	}

	for(i= 0;i<3; i++)
	{
		getGlobalKvircDirectory(szRetPath,globalsubdirs[i],filename);
		if(KviFileUtils::fileExists(szRetPath.ptr()))return true;
	}

	if(findImageInImageSearchPath(szRetPath,filename))return true;

	// Last resort ...
	szRetPath = QDir::homeDirPath();
	szRetPath.ensureLastCharIs(KVI_PATH_SEPARATOR_CHAR);
	szRetPath.append(filename);
	if(KviFileUtils::fileExists(szRetPath.ptr()))return true;;

	szRetPath = filename;
	//if(retMediaType)*retMediaType = m;
	return false;

//gotit:
	//if(retMediaType)*retMediaType = g_pMediaManager->findMediaType(szRetPath.ptr());
//	return true;
}

//========================= findImage ===========================//
//
// Find a file inside the default "Image" directories
// This should be thread safe (and in fact IS used by other threads)
// This function should be used only for finding the INTERNAL
// pixmaps... (so we expect them to be in the pics directory or
// Eventually in user's $HOME
//

// FIXME: #warning "Check WHEN findImage is used and when findUserFile is used...we have a mess here"

bool KviApp::findImageInImageSearchPath(KviStr &szRetPath,const char * filename)
{
	// first lookup the user defined paths
	for(QStringList::Iterator it = KVI_OPTION_STRINGLIST(KviOption_stringlistImageSearchPaths).begin();
			it != KVI_OPTION_STRINGLIST(KviOption_stringlistImageSearchPaths).end();++it)
	{
		szRetPath = *it;
		szRetPath.ensureLastCharIs(KVI_PATH_SEPARATOR_CHAR);
		szRetPath.append(filename);
		//debug("LOOK FOR %s",szRetPath.ptr());
		if(KviFileUtils::fileExists(szRetPath.ptr()))return true;
	}

	return false;
}

bool KviApp::findImageInImageSearchPath(QString &szRetPath,const char * filename)
{
	// first lookup the user defined paths
	for(QStringList::Iterator it = KVI_OPTION_STRINGLIST(KviOption_stringlistImageSearchPaths).begin();
			it != KVI_OPTION_STRINGLIST(KviOption_stringlistImageSearchPaths).end();++it)
	{
		szRetPath = *it;
		KviQString::ensureLastCharIs(szRetPath,KVI_PATH_SEPARATOR_CHAR);
		szRetPath.append(filename);
		//debug("LOOK FOR %s",szRetPath.ptr());
		if(KviFileUtils::fileExists(szRetPath))return true;
	}

	return false;
}

static KviApp::KvircSubdir pics_localsubdirs[2]={ KviApp::Pics , KviApp::Avatars };
static KviApp::KvircSubdir pics_globalsubdirs[2]={ KviApp::Pics , KviApp::Avatars };

bool KviApp::mapImageFile(KviStr &szRetPath,const char * filename)
{
	QString buff;
	bool ret=mapImageFile(buff,filename);
	szRetPath=buff;
	return ret;
}

bool KviApp::mapImageFile(QString &szRetPath,const char * filename)
{
	szRetPath = filename;
	// can't map non absolute paths
	if(!kvi_isAbsolutePath(filename))return false;
	// can't map non existing paths
	if(!KviFileUtils::fileExists(filename))return false;

	QFileInfo fi = QFileInfo(QString::fromUtf8(filename));

	unsigned int size = fi.size();

	KviStr szBestMatch;

	while(szRetPath.find(KVI_PATH_SEPARATOR) != -1)
	{
		KviQString::cutToFirst(szRetPath,KVI_PATH_SEPARATOR);

		if(szRetPath.isEmpty())break;

		KviStr szBuffer;

		int i;

		for(i=0;i<2;i++)
		{
			getLocalKvircDirectory(szBuffer,pics_localsubdirs[i],szRetPath);
			if(KviFileUtils::fileExists(szBuffer.ptr()))
			{
				QFileInfo fi2(QString::fromUtf8(szBuffer.ptr()));
				if(size == fi2.size())
				{
					// probably the same file
					szBestMatch = szRetPath;
				}
			}
		}
	
		for(i=0;i<2;i++)
		{
			getGlobalKvircDirectory(szBuffer,pics_globalsubdirs[i],szRetPath);
			//debug("CHECK %s",szBuffer.ptr());
			if(KviFileUtils::fileExists(szBuffer.ptr()))
			{
				QFileInfo fi2(QString::fromUtf8(szBuffer.ptr()));
				if(size == fi2.size())
				{
					// probably the same file
					szBestMatch = szRetPath;
				}
			}
		}
	
		// Last resort
		szBuffer = QDir::homeDirPath();
		szBuffer.ensureLastCharIs(KVI_PATH_SEPARATOR_CHAR);
		szBuffer.append(szRetPath);
		if(KviFileUtils::fileExists(szBuffer.ptr()))
		{
			QFileInfo fi2(QString::fromUtf8(szBuffer.ptr()));
			if(size == fi2.size())
			{
				// prolly the same file
				szBestMatch = szRetPath;
			}
		}
	}

	if(szBestMatch.hasData())
	{
		szRetPath = szBestMatch;
		return true;
	}


	// no way to map it :/
	szRetPath = filename;
	return false;
}


bool KviApp::findImage(KviStr &szRetPath,const char *filename)
{
	QString buff;
	bool ret=findImage(buff,filename);
	szRetPath=buff;
	return ret;
}

bool KviApp::findImage(QString &szRetPath,const char *filename)
{
	// Find an user file...
	if(kvi_isAbsolutePath(filename))
	{
		szRetPath=filename;
		return KviFileUtils::fileExists(filename);
	}

	// if we have a theme, look it up as first
	if(!KVI_OPTION_STRING(KviOption_stringIconThemeSubdir).isEmpty())
	{
		QString szTmp = KVI_OPTION_STRING(KviOption_stringIconThemeSubdir);
		szTmp.append(KVI_PATH_SEPARATOR_CHAR);
		szTmp.append(filename);

		getLocalKvircDirectory(szRetPath,KviApp::Themes,szTmp);
		if(KviFileUtils::fileExists(szRetPath))return true;

		getGlobalKvircDirectory(szRetPath,KviApp::Themes,szTmp);
		if(KviFileUtils::fileExists(szRetPath))return true;
	}

	int i;

	for(i=0;i<2;i++)
	{
		getLocalKvircDirectory(szRetPath,pics_localsubdirs[i],filename);
		if(KviFileUtils::fileExists(szRetPath))return true;
	}

	for(i=0;i<2;i++)
	{
		getGlobalKvircDirectory(szRetPath,pics_globalsubdirs[i],filename);
		if(KviFileUtils::fileExists(szRetPath))return true;
	}

	if(findImageInImageSearchPath(szRetPath,filename))return true;

	// Last resort
	szRetPath = QDir::homeDirPath();
	KviQString::ensureLastCharIs(szRetPath,KVI_PATH_SEPARATOR_CHAR);
	szRetPath.append(filename);
	if(KviFileUtils::fileExists(szRetPath))return true;

	szRetPath = filename;

	return false;
}

bool KviApp::findImageThemeOnlyCompat(QString &szRetPath,const char *filename)
{
	// if we have a theme, look it up as first
	if(!KVI_OPTION_STRING(KviOption_stringIconThemeSubdir).isEmpty())
	{
		QString szTmp = KVI_OPTION_STRING(KviOption_stringIconThemeSubdir);
		szTmp.append(KVI_PATH_SEPARATOR_CHAR);
		szTmp.append(filename);

		getLocalKvircDirectory(szRetPath,KviApp::Themes,szTmp);
		if(KviFileUtils::fileExists(szRetPath))return true;

		getGlobalKvircDirectory(szRetPath,KviApp::Themes,szTmp);
		if(KviFileUtils::fileExists(szRetPath))return true;
	}

	getLocalKvircDirectory(szRetPath,KviApp::Pics,filename);
	if(KviFileUtils::fileExists(szRetPath))return true;


	if(findImageInImageSearchPath(szRetPath,filename))return true;

	szRetPath = filename;

	return false;
}

bool KviApp::findSmallIcon(QString &szRetPath,const char *filename)
{
	// this is a bit optimized for the small builtin icons
	// looks up less places.

	//if(kvi_isAbsolutePath(filename))
	//{
		// BUG!.. we should never
		//szRetPath=filename;
		//return KviFileUtils::fileExists(filename);
	//}

	// if we have a theme, look it up as first
	if(!KVI_OPTION_STRING(KviOption_stringIconThemeSubdir).isEmpty())
	{
		QString szTmp = KVI_OPTION_STRING(KviOption_stringIconThemeSubdir);
		szTmp.append(KVI_PATH_SEPARATOR_CHAR);
		szTmp.append(KVI_SMALLICONS_SUBDIRECTORY);
		szTmp.append(KVI_PATH_SEPARATOR_CHAR);
		szTmp.append(filename);

		getLocalKvircDirectory(szRetPath,KviApp::Themes,szTmp);
		if(KviFileUtils::fileExists(szRetPath))return true;

		getGlobalKvircDirectory(szRetPath,KviApp::Themes,szTmp);
		if(KviFileUtils::fileExists(szRetPath))return true;
	}

	int i;

	// unlikely
	getLocalKvircDirectory(szRetPath,KviApp::SmallIcons,filename);
	if(KviFileUtils::fileExists(szRetPath))return true;

	getLocalKvircDirectory(szRetPath,KviApp::Pics,filename);
	if(KviFileUtils::fileExists(szRetPath))return true;

	// likely
	getGlobalKvircDirectory(szRetPath,KviApp::SmallIcons,filename);
	if(KviFileUtils::fileExists(szRetPath))return true;

	if(findImageInImageSearchPath(szRetPath,filename))return true;

	// Last resort... bleah :/
	szRetPath = QDir::homeDirPath();
	KviQString::ensureLastCharIs(szRetPath,KVI_PATH_SEPARATOR_CHAR);
	szRetPath.append(filename);
	if(KviFileUtils::fileExists(szRetPath))return true;

	szRetPath = filename;

	return false;
}


//================== getReadOnlyConfigPath ====================//
//
// We want to READ a config file...find it
//

bool KviApp::getReadOnlyConfigPath(KviStr &buffer,const char *config_name,KvircSubdir sbd,bool bNoFail)
{
	// Take a look in the local directory....
	getLocalKvircDirectory(buffer,sbd,config_name);
	//debug("%s %s %i |%s| %i",__FILE__,__FUNCTION__,__LINE__,buffer.ptr(),KviFileUtils::fileExists(buffer.ptr()));
	if(!KviFileUtils::fileExists(buffer.ptr()))
	{
		// No saved config yet... check for defaults
		KviStr tmp;
		getGlobalKvircDirectory(tmp,sbd,config_name);
		if(!KviFileUtils::fileExists(tmp.ptr()))
		{
			// No defaults...no such config file at all.
			if(bNoFail)getLocalKvircDirectory(buffer,sbd,config_name);
			return false;
		} else buffer = tmp; // get the defaults this time
	} // else file exists...
	return true;
}
