//=============================================================================
//
//   File : KvsObject_buttonGroup.cpp
//   Creation date : Fri Jan 28 14:21:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
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

#include "KvsObject_buttonGroup.h"
#include "KviError.h"
#include "kvi_debug.h"
#include "KviLocale.h"
#include "KviIconManager.h"

#include <QButtonGroup>
#include <QRadioButton>

/*
	@doc:	buttongroup
	@keyterms:
		buttongroup object class,
	@title:
		buttongroup class
	@type:
		class
	@short:
		Provides a buttongroup control.
	@inherits:
		[class]object[/class]
	@description:
		This object organizes buttons in a group.
	@functions:
		!fn: <id:integer> $addButton(<checkbutton or radiobutton:object>)
		Adds the given button to the button group and returns an unique identifier.
		!fn: <object> $checkedButton()
		Returns the button group's checked button, or [b]0[/b] if no buttons are checked.
*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_buttonGroup, "buttongroup", "object")
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_buttonGroup, addButton)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_buttonGroup, checkedButton)
KVSO_END_REGISTERCLASS(KvsObject_buttonGroup)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_buttonGroup, KviKvsObject)
m_iId = 0;
btnDict.setAutoDelete(false);
m_pButtonGroup = new QButtonGroup();
KVSO_END_CONSTRUCTOR(KvsObject_buttonGroup)

KVSO_BEGIN_DESTRUCTOR(KvsObject_buttonGroup)
btnDict.clear();
delete m_pButtonGroup;
KVSO_END_CONSTRUCTOR(KvsObject_buttonGroup)

KVSO_CLASS_FUNCTION(buttonGroup, addButton)
{
	CHECK_INTERNAL_POINTER(m_pButtonGroup)
	KviKvsObject * pObject;
	kvs_hobject_t hObject;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("button", KVS_PT_HOBJECT, 0, hObject)
	KVSO_PARAMETERS_END(c)
	pObject = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	CHECK_HOBJECT_IS_WIDGET(pObject)
	if(pObject->inheritsClass("radiobutton") || pObject->inheritsClass("checkbox"))
	{
		m_pButtonGroup->addButton(((QRadioButton *)(pObject->object())), m_iId);
		c->returnValue()->setInteger(m_iId);
		btnDict.insert(m_iId, pObject);
		m_iId++;
	}
	else
	{
		c->warning(__tr2qs_ctx("Buttongroup supports only checkbox and radiobox object", "objects"));
		return true;
	}
	return true;
}
KVSO_CLASS_FUNCTION(buttonGroup, checkedButton)
{
	CHECK_INTERNAL_POINTER(m_pButtonGroup)
	int id = m_pButtonGroup->checkedId();
	if(id != -1)
		c->returnValue()->setHObject(btnDict.find(id)->handle());
	else
		c->returnValue()->setNothing();
	return true;
}
