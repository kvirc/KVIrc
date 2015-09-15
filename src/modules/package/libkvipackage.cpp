//=============================================================================
//
//   File : libkvipackage.cpp
//   Creation date : Tue 7 Dec 00:05:59 2002 GMT by Szymon Stefanek
//
//   This toolbar is part of the KVIrc irc client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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
//=============================================================================

#include "KviModule.h"
#include "KviPackageReader.h"
#include "KviLocale.h"

#include "KviKvsHash.h"
#include "KviKvsArray.h"

#include "KviFileUtils.h"

#include "KviApplication.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

#include <stdlib.h>

static QString createRandomDir()
{
	QString szDirName;
	char chars[] = {
		'A','B','C','D','E','F','G','H',
		'I','J','K','L','M','N','O','P',
		'Q','R','S','T','U','V','W','X',
		'Y','Z','a','b','c','d','e','f',
		'g','h','i','j','k','l','m','n',
		'o','p','q','r','s','t','u','v',
		'w','x','y','z','-','_','.'
	};

	// Generate dir name
	for(int i=0;i<10;i++)
	{
		int n = rand() % sizeof(chars);
		szDirName.append(chars[n]);
	}

	return szDirName;
}


/*
	@doc: package.info
	@type:
		function
	@title:
		$package.info
	@short:
		Returns informations about a KVIrc package file
	@syntax:
		<hash> $package.info(<package_path:string>)
	@description:
		Returns a set of informations about a KVIrc package file (*.kvt or *.kva).
		The set is a structured hash.
		If the specified path does not lead to a valid KVIrc package
		the returned hash will contain a single element with key "error".
*/

static bool package_kvs_fnc_info(KviKvsModuleFunctionCall * c)
{
	QString szPath;
	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("package_path",KVS_PT_NONEMPTYSTRING,0,szPath)
	KVSM_PARAMETERS_END(c)

	KviKvsHash * pHash = new KviKvsHash();
	
	c->returnValue()->setHash(pHash);

	// Create a random extraction dir
	QString szTmpPath, szUnpackPath;
	QString szRandomDir = createRandomDir();

	g_pApp->getLocalKvircDirectory(szTmpPath,KviApplication::Tmp);
	KviQString::ensureLastCharIs(szTmpPath,QChar(KVI_PATH_SEPARATOR_CHAR));
	szUnpackPath = szTmpPath + szRandomDir;
	QDir szTmpDir(szUnpackPath);

	// Check for dir existence
	while(szTmpDir.exists())
	{
		szRandomDir = createRandomDir();
		szUnpackPath = szTmpPath + szRandomDir;
		szTmpDir = QDir(szUnpackPath);
	}

	KviPackageReader r;

	// Unpack addon package into the random tmp dir
	if(!r.unpack(szPath,szUnpackPath))
	{
		pHash->set("error",new KviKvsVariant(r.lastError()));
		return true;
	}

	KviKvsHash * pInfoHash = new KviKvsHash();
	
	pHash->set("info",new KviKvsVariant(pInfoHash));
	
	QFileInfo inf(szPath);
	
	pInfoHash->set("path",new KviKvsVariant(szPath));
	pInfoHash->set("name",new KviKvsVariant(inf.fileName()));
	pInfoHash->set("size",new KviKvsVariant((kvs_int_t)inf.size()));

	KviKvsHash * pMetadataHash = new KviKvsHash();
	
	pHash->set("metadata",new KviKvsVariant(pMetadataHash));

	KviPointerHashTable<QString,QString> * pInfoFields1 = r.stringInfoFields();
	KviPointerHashTableIterator<QString,QString> it(*pInfoFields1);


	while(it.current())
	{
		pMetadataHash->set(it.currentKey(),new KviKvsVariant(*(it.current())));
		it.moveNext();
	}

	KviPointerHashTable<QString,QByteArray> * pInfoFields2 = r.binaryInfoFields();
	KviPointerHashTableIterator<QString,QByteArray> it2(*pInfoFields2);
	
	while(it2.current())
	{
		pMetadataHash->set(it2.currentKey(),new KviKvsVariant(QString("<binary data, %1 bytes>").arg(it2.current()->size())));
		it2.moveNext();
	}

	KviKvsArray * pFilesArray = new KviKvsArray();
	
	pHash->set("files",new KviKvsVariant(pFilesArray));

	QStringList sl = KviFileUtils::getFileListing(szUnpackPath);

	Q_FOREACH(QString fn,sl)
		pFilesArray->append(new KviKvsVariant(fn));

	// delete the random tmp dir
	if(!KviFileUtils::deleteDir(szUnpackPath))
		qDebug("Could not delete temporary directory %s",szUnpackPath.toUtf8().data());

	return true;
}


static bool package_module_init(KviModule *m)
{
	KVSM_REGISTER_FUNCTION(m,"info",package_kvs_fnc_info);
	return true;
}

static bool package_module_cleanup(KviModule *)
{
	return true;
}

KVIRC_MODULE(
	"package",                                                      // module name
	"1.0.0",                                                        // module version
	"Copyright (C) 2015 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
	"Interface to KVIrc package files",
	package_module_init,
	0,
	0,
	package_module_cleanup,
	0
)
