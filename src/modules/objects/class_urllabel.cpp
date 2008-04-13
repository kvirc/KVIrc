//vim: ts=8
//   File : class_urllabel.cpp
//   Creation date : Thu Feb 18 1:27:44 CEST 2001 by Krzysztof Godlewski
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Krzysztof Godlewski
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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


#include "class_urllabel.h"

#include <QLabel>
#include "kvi_debug.h"
#include "kvi_locale.h"
#include "kvi_error.h"

#include "kvi_app.h"

#include "kvi_window.h"
#include "kvi_kvs_script.h"
#include <QMouseEvent>
#define QT_LEFT_BUTTON Qt::LeftButton
//#warning "Functions to change colors for different states"
//#warning "$setUnderline(), $underline()"

//extern KviApp * g_pApp;
/*static KviScriptObjectClass * g_pUrlLabelClass = 0;

static KviScriptObject * urlLabelClassCreateInstance(KviScriptObjectClass * c,
	KviScriptObject * p, const char * n)
{
	return new KviScriptUrlLabelObject(c, p, n);
}

KviScriptUrlLabelObject::KviScriptUrlLabelObject(KviScriptObjectClass * c, \
KviScriptObject * p, const char *n) : KviScriptLabelObject(c, p, n)
{
	m_normalClr = QColor("black");
	m_enterClr = QColor("blue");
	m_activeClr = QColor("red");
	m_useSingleClick = true;
//	((QLabel *)widget())->setAutoResize(true);
	//widget()->setCursor(Qt::pointingHandCursor);
	//((KviScriptWidgetObject: * )p)->widget()->setCursor(Qt::pointingHandCursor);
}

KviScriptUrlLabelObject::~KviScriptUrlLabelObject()
{
}

bool KviScriptUrlLabelObject::eventFilter(QObject * o, QEvent * e)
{
	QPalette pal = ((QLabel *)widget())->palette();

	switch(e->type())
	{
		case QEvent::Enter:
			pal.setColor(QColorGroup::Foreground, m_enterClr);
			break;
		case QEvent::Leave:
			// this doesn't work...
			if(((QMouseEvent *)e)->state() & QEvent::LeftButton)
				pal.setColor(QColorGroup::Foreground, \
					m_activeClr);
			else
				pal.setColor(QColorGroup::Foreground, \
					m_normalClr);
			break;
		case QEvent::MouseButtonRelease:
			pal.setColor(QColorGroup::Foreground, m_enterClr);
			// perform actions here
			if(!m_lastClickWasDbl && !m_useSingleClick)
				break;
			if(!m_action.isEmpty())
			{
					
			}
			break;
		case QEvent::MouseButtonPress:
			m_lastClickWasDbl = false;
			if(!((QMouseEvent * )e)->button() & \
			  QEvent::LeftButton || !m_useSingleClick)
				break;
			pal.setColor(QColorGroup::Foreground, m_activeClr);
			break;
		case QEvent::MouseButtonDblClick:
			m_lastClickWasDbl = true;
			if(m_useSingleClick)
				break;
			pal.setColor(QColorGroup::Foreground, m_activeClr);
			break;
		default: // make gcc happy
		break;
	}
	
	((QLabel *)widget())->setPalette(pal);

	return false;
}
*/
//#warning "Docs for urllabel"

/*
	@doc:	urllabel
	@keyterms:
		display url, link
	@title:
		urllabel class
	@type:
		class
	@short:
		Displays an URI allowing to perform actions on click
	@inherits:
		[class]object[/class]
		[class]widget[/class]
		[class]label[/class]
	@description:
		This widget allows you to bind specific action on click. It can
		open an url, or execute any KVI++ code. The text color reacts on
		cursor movements and clicks.
	@functions:
		!fn: $setUrl(<text:string>)
		Sets the url to be opened when the links is activated. The way
		the label opens the url is defined in KVirc config dialog.
		!fn: <string> $url()
		Returns the currently set url.
		!fn: $setAction(<text:string>)
		Sets the action to be performed when the link is triggered. It
		can be any valid KVI++ code. Note that all KVirc commands must
		be prefixed with a '/', just as if you typed them in the input
		widget.
		!fn: <string> $action()
		Returns currently set action string.
		!fn: $setText(<text>)
		Sets the label's text. Reimplemented from internal reasons.
		!fn: $setCursorChange(<bEnabled:boolean>)
		Tells the label wheather to change or not the shape of cursor,
		when it is above the widget. Default is false.
		!fn: <boolean> $cursorChange()
		Returns true if the cursor changes over the label, false if not.
		!fn: $setUseSingleClick(<bEnabled:boolean>)
		If <bEnabled> is true, the label will react on single clicks,
		otherwise only double clicks will trigger the link.
		!fn: <boolean> $useSingleClick()
		Returns true if the label reacts on single clicks, false if it
		reacts only on double clicks.
*/

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_urlabel,"urllabel","label")

	KVSO_REGISTER_HANDLER(KviKvsObject_urlabel,"setUrl",functionsetUrl)
	KVSO_REGISTER_HANDLER(KviKvsObject_urlabel,"url",functionurl)
	KVSO_REGISTER_HANDLER(KviKvsObject_urlabel,"setAction",functionsetAction)
	KVSO_REGISTER_HANDLER(KviKvsObject_urlabel,"action",functionaction)
	KVSO_REGISTER_HANDLER(KviKvsObject_urlabel,"setText",functionsetText)
	KVSO_REGISTER_HANDLER(KviKvsObject_urlabel,"setCursorChange",functionsetCursorChange)
	KVSO_REGISTER_HANDLER(KviKvsObject_urlabel,"cursorChange",functioncursorChange)
	KVSO_REGISTER_HANDLER(KviKvsObject_urlabel,"setUseSingleClick",functionsetUseSingleClick)
	KVSO_REGISTER_HANDLER(KviKvsObject_urlabel,"useSingleClick",functionuseSingleClick)
	
KVSO_END_REGISTERCLASS(KviKvsObject_urlabel)


KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_urlabel,KviKvsObject_label)

	m_normalClr = QColor("black");
	m_enterClr = QColor("blue");
	m_activeClr = QColor("red");
	m_useSingleClick = true;
	
KVSO_END_CONSTRUCTOR(KviKvsObject_urlabel)

KVSO_BEGIN_DESTRUCTOR(KviKvsObject_urlabel)

KVSO_END_CONSTRUCTOR(KviKvsObject_urlabel)

bool KviKvsObject_urlabel::eventFilter(QObject * o, QEvent * e)
{
	QPalette pal = ((QLabel *)widget())->palette();

	switch(e->type())
	{
		case QEvent::Enter:
			pal.setColor(QColorGroup::Foreground, m_enterClr);
			break;
		case QEvent::Leave:
			// this doesn't work...
			if(((QMouseEvent *)e)->state() & QT_LEFT_BUTTON)
				pal.setColor(QColorGroup::Foreground, \
					m_activeClr);
			else
				pal.setColor(QColorGroup::Foreground, \
					m_normalClr);
			break;
		case QEvent::MouseButtonRelease:
			pal.setColor(QColorGroup::Foreground, m_enterClr);
			// perform actions here
			if(!m_lastClickWasDbl && !m_useSingleClick)
				break;
			if(!m_action.isEmpty())
			{
				KviKvsScript kvs("commandline",m_action);
				kvs.run(g_pActiveWindow,0,0/*,KviKvsScript::AssumeLocals*/);
					
			} else if(!m_url.isEmpty()) {
				KviKvsScript kvs("commandline","openurl "+m_url);
				kvs.run(g_pActiveWindow,0,0/*,KviKvsScript::AssumeLocals*/);
			}
			break;
		case QEvent::MouseButtonPress:
			m_lastClickWasDbl = false;
			if(!((QMouseEvent * )e)->button() & \
			  QT_LEFT_BUTTON || !m_useSingleClick)
				break;
			pal.setColor(QColorGroup::Foreground, m_activeClr);
			break;
		case QEvent::MouseButtonDblClick:
			m_lastClickWasDbl = true;
			if(m_useSingleClick)
				break;
			pal.setColor(QColorGroup::Foreground, m_activeClr);
			break;
		default: // make gcc happy
		break;
	}
	
	((QLabel *)widget())->setPalette(pal);

	return false;
}
bool KviKvsObject_urlabel::functionsetUrl(KviKvsObjectFunctionCall *c)
{
	QString szUrl;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("url",KVS_PT_STRING,0,szUrl)
	KVSO_PARAMETERS_END(c)
	if(!widget()) return true;
	if(szUrl.isEmpty()){
		c->error(__tr2qs("Not Enough Parameters"));
		return false;
		}
	m_url = szUrl;
	return true;
}
bool KviKvsObject_urlabel::functionurl(KviKvsObjectFunctionCall *c)
{
	c->returnValue()->setString(m_url);
	return true;
}
bool KviKvsObject_urlabel::functionsetAction(KviKvsObjectFunctionCall *c)
{
	QString szAction;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("action",KVS_PT_STRING,0,szAction)
	KVSO_PARAMETERS_END(c)
	if(!widget()) return true;
	if(szAction.isEmpty()){
		c->error(__tr2qs("Not Enough Parameters"));
		return false;
		}
	m_action = szAction;
	return true;
}
bool KviKvsObject_urlabel::functionsetText(KviKvsObjectFunctionCall *c)
{
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("text",KVS_PT_STRING,0,szText)
	KVSO_PARAMETERS_END(c)
	if(!widget()) return true;
	((QLabel *)widget())->setText(szText);
	((QLabel *)widget())->setFixedSize(((QLabel *)widget())->sizeHint());
	return true;
}
bool KviKvsObject_urlabel::functionaction(KviKvsObjectFunctionCall *c)
{
	c->returnValue()->setString(m_action);
	return true;
}
bool KviKvsObject_urlabel::functionsetCursorChange(KviKvsObjectFunctionCall *c)
{
	bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOLEAN,0,bFlag)
	KVSO_PARAMETERS_END(c)
	if(!widget())
		return true;
	if(m_changeCursor = bFlag)
		widget()->setCursor(Qt::pointingHandCursor);
	else
		widget()->setCursor(Qt::arrowCursor);
	return true;
}
bool KviKvsObject_urlabel::functionsetUseSingleClick(KviKvsObjectFunctionCall *c)
{
	bool bFlag;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("bEnabled",KVS_PT_BOOLEAN,0,bFlag)
	KVSO_PARAMETERS_END(c)
	m_useSingleClick = bFlag;
	return true;
}
bool KviKvsObject_urlabel::functionuseSingleClick(KviKvsObjectFunctionCall *c)
{
	c->returnValue()->setBoolean(m_useSingleClick);
	return true;
}
bool KviKvsObject_urlabel::functioncursorChange(KviKvsObjectFunctionCall *c)
{
	c->returnValue()->setBoolean(m_changeCursor);
	return true;
}
