//=============================================================================
//
//   File : libkvilanguage.cpp
//   Creation date : Fri 25 Dec 20:16:49 2004 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2004 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_module.h"
#include "kvi_app.h"
#include "kvi_locale.h"

#include <QFileInfo>

#include "langdetector.h"

/*
	@doc: language.detect
	@type:
		command
	@title:
		language.detect
	@short:
		(Tries to) detect the language and encoding of a text file
	@syntax:
		language.detect [document: string]
	@description:
		Tries to detect the language and encoding of the file specified
		as [document]. The file has to be preferably a simple text file,
		or the detector can be deceived by the file format (eg. html files
		contains english words inside html tags).
		[document] can be an absolute path.
		This command is exported by the "language" module.
*/


static bool language_kvs_cmd_detect(KviKvsModuleCommandCall * c)
{
	QString doc;

	KVSM_PARAMETERS_BEGIN(c)
		KVSM_PARAMETER("document",KVS_PT_STRING,KVS_PF_OPTIONAL,doc)
	KVSM_PARAMETERS_END(c)

	if(doc.isEmpty()){
		c->warning(__tr2qs("No file given, no detection occured."));
		return true;
	}

	QFileInfo * f= new QFileInfo(doc);
	qDebug("Path %s",doc.toUtf8().data());
	if(f)
	{
		if(!f->exists())
		{
			c->warning(__tr2qs("The file you specified doesn't exists, can't detect its language."));
			return true;
		}
	}

	KviLangDetetector *w = new KviLangDetetector(doc);

	return true;
}

static bool language_module_init(KviModule * m)
{
	KVSM_REGISTER_SIMPLE_COMMAND(m,"detect",language_kvs_cmd_detect);
	return true;
}

static bool language_module_cleanup(KviModule *m)
{
	return true;
}

KVIRC_MODULE(
	"Language",                                              // module name
	"4.0.0",                                                // module version
	"Copyright (C) 2004 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
	"A simple statistical language/encoding detector",
	language_module_init,
	0,
	0,
	language_module_cleanup
)
