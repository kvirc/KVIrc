//=============================================================================
//
//   File : KvsObject_trayIcon.cpp
//   Creation date : Wed Ago 25 17:28:45 2010 GMT by Carbone Alesssandro
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2009 Alessandro Carbone (elfonol at gmail dot com)
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

#include "KvsObject_trayIcon.h"
#include "KvsObject_popupMenu.h"
#include "KviIconManager.h"
#include "kvi_debug.h"
#include "KviError.h"
#include "KviLocale.h"

#include <QSystemTrayIcon>

/*
	@doc: trayicon
	@keyterms:
		trayicon object class
	@title:
		trayicon class
	@type:
		class
	@short:
		An implementation of the the system tray icon.
	@inherits:
		[class]object[/class]
	@description:
		This class provides a standard trayicon functionality.[br]
	@functions:
		!fn: setIcon(<image_id>).
		Sets the tray icon.
		See the [doc:image_id]image identifier[/doc] documentation for the explanation of the <image_id> parameter.
		!fn: setTooltip(<tooltip:string>).
		Sets the tray icon tooltip.
		!fn: show()
		Shows the tray icon.
		!fn: hide()
		Hides the tray icon.
		!fn: isVisible()
		Returns '1' if the tray icon is currently visible.
		!fn: showMessage(<title:string>,<message:string>,<message_icon:string>,<timeout:integer>)
		Shows a balloon message for the entry with the given title, message and message_icon for the time specified in millisecondsTimeoutHint. title and message must be plain text strings.
		Message can be clicked by the user; the messageClickedEvent() will be triggered when this occurs.[br]
		Valid values for message_icon are:
		[pre]
			- NoIcon : No icon is shown.
			- Information : An information icon is shown.
			- Warning : A standard warning icon is shown.
			- Critical : A critical warning icon is shown.
		[/pre]
		!fn: setContextMenu(<popupmenu:hobject>).
		Associates the given <popupmenu> with the tray icon.
		!fn: activatedEvent(<reason:string>)
		This event is triggered when the user click the tray icon.
		If you reimplement this function the reason parameter will be passed as $0.
		Values for reason are:[br]
		[pre]
			- Unknown : Unknown reason.
			- Context : The context menu for the tray icon was requested.
			- DoubleClick : The tray icon was double clicked.
			- Trigger : The tray icon was clicked.
			- MiddleClick : The tray icon was clicked with the middle mouse button.
		[/pre]
		The default implementation emits the [classfnc]$activated[/classfnc]() signal.
		!fn: messageClickedEvent()
		This event is triggered when the message displayed using [classfnc]$showMessage[/classfnc]() was clicked by the user.
		The default implementation emits the [classfnc]$messageClicked[/classfnc]() signal.
	@signals:
		!sg: $activated()
		This signal is emitted by the default implementation of [classfnc]$activatedEvent[/classfnc].
		!sg: $messageClicked()
		This signal is emitted by the default implementation of [classfnc]$messageClickedEvent[/classfnc].
*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_trayIcon, "trayicon", "object")
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_trayIcon, show)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_trayIcon, hide)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_trayIcon, isVisible)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_trayIcon, setIcon)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_trayIcon, setTooltip)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_trayIcon, showMessage)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_trayIcon, setContextMenu)
// events
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_trayIcon, activatedEvent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_trayIcon, messageClickedEvent)
KVSO_END_REGISTERCLASS(KvsObject_trayIcon)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_trayIcon, KviKvsObject)
m_pTrayIcon = new QSystemTrayIcon(nullptr);
connect(m_pTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(slotActivated(QSystemTrayIcon::ActivationReason)));
connect(m_pTrayIcon, SIGNAL(messageClicked()), this, SLOT(slotMessageClicked()));

KVSO_END_CONSTRUCTOR(KvsObject_trayIcon)

KVSO_BEGIN_DESTRUCTOR(KvsObject_trayIcon)
delete m_pTrayIcon;
KVSO_END_DESTRUCTOR(KvsObject_trayIcon)

KVSO_CLASS_FUNCTION(trayIcon, setIcon)
{
	CHECK_INTERNAL_POINTER(m_pTrayIcon)
	QString szIcon;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("icon", KVS_PT_STRING, 0, szIcon)
	KVSO_PARAMETERS_END(c)
	QPixmap * pix = g_pIconManager->getImage(szIcon);
	if(pix)
		m_pTrayIcon->setIcon(*pix);
	return true;
}
KVSO_CLASS_FUNCTION(trayIcon, setTooltip)
{
	CHECK_INTERNAL_POINTER(m_pTrayIcon)
	QString szTooltip;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("icon", KVS_PT_STRING, 0, szTooltip)
	KVSO_PARAMETERS_END(c)
	m_pTrayIcon->setToolTip(szTooltip);
	return true;
}
KVSO_CLASS_FUNCTION(trayIcon, show)
{
	CHECK_INTERNAL_POINTER(m_pTrayIcon)
	m_pTrayIcon->show();
	return true;
}
KVSO_CLASS_FUNCTION(trayIcon, hide)
{
	CHECK_INTERNAL_POINTER(m_pTrayIcon)
	m_pTrayIcon->show();
	return true;
}
KVSO_CLASS_FUNCTION(trayIcon, isVisible)
{
	CHECK_INTERNAL_POINTER(m_pTrayIcon)
	c->returnValue()->setBoolean(m_pTrayIcon->isVisible());
	return true;
}

KVSO_CLASS_FUNCTION(trayIcon, showMessage)
{
	CHECK_INTERNAL_POINTER(m_pTrayIcon)
	QString szMessage, szTitle, szMessageIcon;
	kvs_uint_t uTimeout;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("title", KVS_PT_STRING, 0, szTitle)
	KVSO_PARAMETER("message", KVS_PT_STRING, 0, szMessage)
	KVSO_PARAMETER("icon", KVS_PT_STRING, 0, szMessageIcon)
	KVSO_PARAMETER("timeout", KVS_PT_UINT, 0, uTimeout)
	KVSO_PARAMETERS_END(c)
	QSystemTrayIcon::MessageIcon message = QSystemTrayIcon::NoIcon;
	if(KviQString::equalCI(szMessageIcon, "NoIcon"))
		message = QSystemTrayIcon::NoIcon;
	else if(KviQString::equalCI(szMessageIcon, "Information"))
		message = QSystemTrayIcon::Information;
	else if(KviQString::equalCI(szMessageIcon, "Warning"))
		message = QSystemTrayIcon::Warning;
	else if(KviQString::equalCI(szMessageIcon, "Critical"))
		message = QSystemTrayIcon::Critical;
	else
		c->warning(__tr2qs_ctx("Invalid message Icon: switch to default NoIcon '%Q'", "objects"), &szMessageIcon);
	m_pTrayIcon->showMessage(szTitle, szMessage, message, uTimeout);
	return true;
}

KVSO_CLASS_FUNCTION(trayIcon, setContextMenu)
{
	CHECK_INTERNAL_POINTER(m_pTrayIcon)
	KviKvsObject * ob;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("widget", KVS_PT_HOBJECT, 0, hObject)
	KVSO_PARAMETERS_END(c)
	ob = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!ob)
	{
		c->warning(__tr2qs_ctx("Widget parameter is not an object", "objects"));
		return true;
	}
	if(!ob->object())
	{
		c->warning(__tr2qs_ctx("Widget parameter is not a valid object", "objects"));
		return true;
	}
	if(!ob->inheritsClass("popupmenu"))
	{
		c->warning(__tr2qs_ctx("Can't add a non-popupmenu object", "objects"));
		return true;
	}
	m_pTrayIcon->setContextMenu(((QMenu *)(ob->object())));
	return true;
}
KVSO_CLASS_FUNCTION(trayIcon, activatedEvent)
{
	emitSignal("activated", c, c->params());
	return true;
}
KVSO_CLASS_FUNCTION(trayIcon, messageClickedEvent)
{
	emitSignal("messageClicked", c, c->params());
	return true;
}
void KvsObject_trayIcon::slotMessageClicked()
{
	KviKvsVariantList * lParams = nullptr;
	callFunction(this, "messageClickedEvent", nullptr, lParams);
}

void KvsObject_trayIcon::slotActivated(QSystemTrayIcon::ActivationReason reason)
{
	QString szReason;
	if(reason == QSystemTrayIcon::Unknown)
		szReason = "Unknown";
	else if(reason == QSystemTrayIcon::Context)
		szReason = "Context";
	else if(reason == QSystemTrayIcon::DoubleClick)
		szReason = "DoubleClick";
	else if(reason == QSystemTrayIcon::Trigger)
		szReason = "Trigger";
	else
		szReason = "MiddleClick";
	KviKvsVariantList lParams;
	lParams.append(new KviKvsVariant(szReason));
	callFunction(this, "activatedEvent", nullptr, &lParams);
}
