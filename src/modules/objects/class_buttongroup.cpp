//
//   File : class_buttongroup.cpp
//   Creation date : Fri Jan 28 14:21:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2005 Szymon Stefanek (pragma at kvirc dot net)
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

#include <qbuttongroup.h>
#include "class_buttongroup.h"
#include <qradiobutton.h>
#include "kvi_error.h"
#include "kvi_debug.h"

#include "kvi_locale.h"
#include "kvi_iconmanager.h"


/*
	@doc:	buttongroup
	@keyterms:
		buttongroup object class,
	@title:
		buttongroup class
	@type:
		class
	@short:
		Provides a buttongroup bar.
	@inherits:
		[class]groupbox[/class]
		[class]object[/class]
		[class]widget[/class]
	@description:
		This widget organizes buttons in a group.
		It will be usually a parent for other child controls.
		You can either use a child layout to manage the children geometries
		or use $setColumnLayout function to manage the layout automatically.
		The class ineriths groupbox.

	@functions:
	
*/

KVSO_BEGIN_REGISTERCLASS(KviKvsObject_buttongroup,"buttongroup","groupbox")

KVSO_END_REGISTERCLASS(KviKvsObject_buttongroup)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_buttongroup,KviKvsObject_groupbox)

KVSO_END_CONSTRUCTOR(KviKvsObject_buttongroup)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_buttongroup)

KVSO_END_CONSTRUCTOR(KviKvsObject_buttongroup)

// CHECK ME
bool KviKvsObject_buttongroup::init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams)
{
	#ifdef COMPILE_USE_QT4
		QButtonGroup *group=new QButtonGroup(parentScriptWidget());
		group->setObjectName(name());
		setObject(group,true);
	#else
		setObject(new QButtonGroup(name(),parentScriptWidget()), true);
	#endif	


	return true;
}


