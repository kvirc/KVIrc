//=============================================================================
//
//   File : class_tbrowser.cpp
//   Creation date : Fri Mar 18 14:30:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2005-2008 Alessandro Carbone (elfonol at gmail dot com)
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


#include "class_tbrowser.h"
#include "kvi_error.h"
#include "kvi_debug.h"
#include "kvi_locale.h"
#include "kvi_iconmanager.h"

#include <QTextBrowser>
#include <QFile>
#include <QUrl>
/*
	@doc: textbrowser
	@keyterms:
		textbrowser browser widget class,
	@title:
		textbrowser class
	@type:
		class
	@short:
		Provides a class that can display html pages.
	@inherits:
		[class]widget[/class]
	@description:
		Provides a class that can display html pages with hypertext navigation.
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

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_textbrowser,"textbrowser","multilineedit")
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_textbrowser,setSource)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_textbrowser,forward)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_textbrowser,backward)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_textbrowser,home)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_textbrowser,reload)
	KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_textbrowser,linkClickedEvent)
KVSO_END_REGISTERCLASS(KviKvsObject_textbrowser)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_textbrowser,KviKvsObject_textedit)

KVSO_END_CONSTRUCTOR(KviKvsObject_textbrowser)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_textbrowser)

KVSO_END_CONSTRUCTOR(KviKvsObject_textbrowser)

bool KviKvsObject_textbrowser::init(KviKvsRunTimeContext *,KviKvsVariantList *)
{
	SET_OBJECT(QTextBrowser);
	connect(obj,SIGNAL(anchorClicked(const QUrl &)),this,SLOT(anchorClicked(const QUrl &)));
	return true;
}

KVSO_CLASS_FUNCTION(textbrowser,setSource)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szFile;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("file_name",KVS_PT_STRING,0,szFile)
	KVSO_PARAMETERS_END(c)
	if(!QFile::exists(szFile))
	{
		c->warning(__tr2qs_ctx("I can't find the specified file '%Q'.","objects"),&szFile);
		return true;
	}
	QUrl url;
	url.setPath(szFile);
	((QTextBrowser *)widget())->setSource(url);
	return true;
}

KVSO_CLASS_FUNCTION(textbrowser,forward)
{
	CHECK_INTERNAL_POINTER(widget())
	((QTextBrowser *)widget())->forward();
	return true;
}

KVSO_CLASS_FUNCTION(textbrowser,backward)
{
	CHECK_INTERNAL_POINTER(widget())
	((QTextBrowser *)widget())->backward();
	return true;
}

KVSO_CLASS_FUNCTION(textbrowser,home)
{
	CHECK_INTERNAL_POINTER(widget())
	((QTextBrowser *)widget())->home();
	return true;
}

KVSO_CLASS_FUNCTION(textbrowser,reload)
{
	CHECK_INTERNAL_POINTER(widget())
	((QTextBrowser *)widget())->reload();
	return true;
}

void KviKvsObject_textbrowser::anchorClicked(const QUrl &url)
{
	KviKvsVariantList parms(new KviKvsVariant(url.path()));
	callFunction(this,"linkClickedEvent",0,&parms);
}

KVSO_CLASS_FUNCTION(textbrowser,linkClickedEvent)
{
	emitSignal("linkClicked",c,c->params());
	return true;
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_tbrowser.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

