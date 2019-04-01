#ifndef _CLASS_DOCKWINDOW_H_
#define _CLASS_DOCKWINDOW_H_
//=============================================================================
//
//   File : KvsObject_dockWindow.h
//   Creation date : Thu 29 Dec 2005 23:45:11 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2005-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_settings.h"
#include "object_macros.h"
#include "KvsObject_widget.h"

class KvsObject_dockWindow : public KvsObject_widget
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KvsObject_dockWindow)
protected:
	bool init(KviKvsRunTimeContext * pContext, KviKvsVariantList * pParams) override;

	bool addWidget(KviKvsObjectFunctionCall * c);
	bool setAllowedDockAreas(KviKvsObjectFunctionCall * c);
	bool dock(KviKvsObjectFunctionCall * c);
};

#endif //!_CLASS_DOCKWINDOW_H_
