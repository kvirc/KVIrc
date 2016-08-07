//=============================================================================
//
//   File : KvsObject_textBrowser.cpp
//   Creation date : Fri Mar 18 14:30:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2008 Alessandro Carbone (elfonol at gmail dot com)
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

#include "KvsObject_textBrowser.h"
#include "KviError.h"
#include "kvi_debug.h"
#include "KviLocale.h"
#include "KviIconManager.h"

#include <QTextBrowser>
#include <QFile>
#include <QUrl>

/*
	@doc: textBrowser
	@keyterms:
		textBrowser browser widget class,
	@title:
		textBrowser class
	@type:
		class
	@short:
		Provides a class that can display HTML pages.
	@inherits:
		[class]widget[/class]
	@description:
		Provides a class that can display HTML pages with hypertext navigation.
	@functions:
		!fn: $setSource(<file_name:string>)
		With this you can display the page at szFile location (szFile include the path).
		!fn: $backward()
		Changes the document displayed to the previous document in the list of documents built by navigating links.
		!fn: $forward()
		Changes the document displayed to the next document in the list of documents built by navigating links.
		!fn: $home()
		Changes the document displayed to be the first document the browser displayed.
		!fn: $reload()
		Reloads the current set source.
		!fn: $linkClickedEvent(<link_href:string>)
		Triggered when a link is clicked by the user. The default implementation of this function
		emits the "linkClicked" signal.
	@signals:
		!sg: $linkClicked(<link_href:string>)
		This signal is emitted by the default implementation of the [classfnc]$linkClickedEvent[/classfnc]() function.
*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_textBrowser, "textBrowser", "multilineedit")
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_textBrowser, setSource)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_textBrowser, forward)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_textBrowser, backward)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_textBrowser, home)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_textBrowser, reload)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_textBrowser, linkClickedEvent)
KVSO_END_REGISTERCLASS(KvsObject_textBrowser)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_textBrowser, KvsObject_textedit)

KVSO_END_CONSTRUCTOR(KvsObject_textBrowser)

KVSO_BEGIN_DESTRUCTOR(KvsObject_textBrowser)

KVSO_END_CONSTRUCTOR(KvsObject_textBrowser)

bool KvsObject_textBrowser::init(KviKvsRunTimeContext *, KviKvsVariantList *)
{
	SET_OBJECT(QTextBrowser);
	connect(obj, SIGNAL(anchorClicked(const QUrl &)), this, SLOT(anchorClicked(const QUrl &)));
	return true;
}

KVSO_CLASS_FUNCTION(textBrowser, setSource)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szFile;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("file_name", KVS_PT_STRING, 0, szFile)
	KVSO_PARAMETERS_END(c)
	if(!QFile::exists(szFile))
	{
		c->warning(__tr2qs_ctx("I can't find the specified file '%Q'.", "objects"), &szFile);
		return true;
	}
	((QTextBrowser *)widget())->setSource(QUrl::fromLocalFile(szFile));
	return true;
}

KVSO_CLASS_FUNCTION(textBrowser, forward)
{
	CHECK_INTERNAL_POINTER(widget())
	((QTextBrowser *)widget())->forward();
	return true;
}

KVSO_CLASS_FUNCTION(textBrowser, backward)
{
	CHECK_INTERNAL_POINTER(widget())
	((QTextBrowser *)widget())->backward();
	return true;
}

KVSO_CLASS_FUNCTION(textBrowser, home)
{
	CHECK_INTERNAL_POINTER(widget())
	((QTextBrowser *)widget())->home();
	return true;
}

KVSO_CLASS_FUNCTION(textBrowser, reload)
{
	CHECK_INTERNAL_POINTER(widget())
	((QTextBrowser *)widget())->reload();
	return true;
}

void KvsObject_textBrowser::anchorClicked(const QUrl & url)
{
	KviKvsVariantList parms(new KviKvsVariant(url.path()));
	callFunction(this, "linkClickedEvent", nullptr, &parms);
}

KVSO_CLASS_FUNCTION(textBrowser, linkClickedEvent)
{
	emitSignal("linkClicked", c, c->params());
	return true;
}
