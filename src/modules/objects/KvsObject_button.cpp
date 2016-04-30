//=============================================================================
//
//   File : KvsObject_button.cpp
//   Creation date : Wed 13 Sep 2000 02:42:05 CEST by Krzysztof Godlewski
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000 Krzysztof Godlewski
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_debug.h"
#include "KviLocale.h"
#include "KviError.h"
#include "KviIconManager.h"
#include "KviFile.h"

#include "KvsObject_button.h"
#include "KvsObject_pixmap.h"

#include <QIcon>
#include <QPushButton>

/*
	@doc:button
	@title:
		button class
	@type:
		class
	@short:
		Button widget.
	@inherits:
		[class]object[/class]
		[class]button[/class]
	@description:
		This widget provides a push button
	@functions:
		!fn: $setText([<text:string>])
		Set the button text.[br]
		See also [classfnc]$text[/classfnc]().
		!fn: <string> $text()
		Return the button text.[br]
		See also [classfnc]$setText[/classfnc]().
		!fn: $setImage(<image_id_or_pixmap_object>)
		Sets the icon for this button.
		See the [doc:image_id]image identifier[/doc] documentation for the explanation	of the <image_id> parameter.
		!fn: $clickEvent()
		Called by KVIrc when the mouse button is clicked.
		The default implementation emits the [classfnc]$clicked[/classfnc]()signal.
	@signals:
		!sg: $clicked()
		This signal is emitted by the default implementation of [classfnc]$clickEvent[/classfnc]().
	@properties:
		!pr: $scaledContents()
		This property holds whether the label will scale its contents to fill all available space.
*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_button, "button", "widget")

KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_button, setText)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_button, text)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_button, clickEvent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_button, setImage)

KVSO_END_REGISTERCLASS(KvsObject_button)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_button, KvsObject_widget)

KVSO_END_CONSTRUCTOR(KvsObject_button)

KVSO_BEGIN_DESTRUCTOR(KvsObject_button)

KVSO_END_DESTRUCTOR(KvsObject_button)

bool KvsObject_button::init(KviKvsRunTimeContext *, KviKvsVariantList *)
{
	SET_OBJECT(QPushButton);
	connect(widget(), SIGNAL(clicked()), this, SLOT(slotClicked()));
	return true;
}

KVSO_CLASS_FUNCTION(button, text)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setString(((QPushButton *)widget())->text());
	return true;
}

KVSO_CLASS_FUNCTION(button, setText)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("text", KVS_PT_STRING, 0, szText)
	KVSO_PARAMETERS_END(c)
	((QPushButton *)widget())->setText(szText);
	return true;
}

KVSO_CLASS_FUNCTION(button, setImage)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsVariant * pIcon;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("icon_or_hobject", KVS_PT_VARIANT, 0, pIcon)
	KVSO_PARAMETERS_END(c)
	if(!pIcon)
	{
		c->warning(__tr2qs_ctx("Image parameter missing", "object"));
		return true;
	}
	if(pIcon->isHObject())
	{
		kvs_hobject_t hObj;
		pIcon->asHObject(hObj);
		KviKvsObject * pObject = KviKvsKernel::instance()->objectController()->lookupObject(hObj);
		if(!pObject)
		{
			c->warning(__tr2qs_ctx("Pixmap parameter is not an object!", "objects"));
			return true;
		}
		if(pObject->inheritsClass("pixmap"))
			((QPushButton *)widget())->setIcon(QIcon(*((KvsObject_pixmap *)pObject)->getPixmap()));
		else
			c->warning(__tr2qs_ctx("Object pixmap required!", "object"));
		return true;
	}
	QString szIcon;
	pIcon->asString(szIcon);
	QPixmap * pix = g_pIconManager->getImage(szIcon);
	if(pix)
		((QPushButton *)widget())->setIcon(*pix);
	else
		((QPushButton *)widget())->setIcon(QIcon());
	return true;
}
KVSO_CLASS_FUNCTION(button, clickEvent)
{
	emitSignal("clicked", c);
	return true;
}

// slots
void KvsObject_button::slotClicked()
{
	KviKvsVariantList * params = nullptr;
	callFunction(this, "clickEvent", params);
}
