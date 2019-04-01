#ifndef _CLASS_WORKSPACE_H_
#define _CLASS_WORKSPACE_H_
//=============================================================================
//
//   File : KvsObject_workspace.h
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

#include "KvsObject_widget.h"
#include <QHash>
#include <QMdiSubWindow>
class KvsObject_workspace : public KvsObject_widget
{
public:
	KVSO_DECLARE_OBJECT(KvsObject_workspace)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	QHash<kvs_hobject_t, QMdiSubWindow *> * pWidgetDict;
	bool init(KviKvsRunTimeContext * pContext, KviKvsVariantList * pParams) override;

	bool addSubWindow(KviKvsObjectFunctionCall * c);
	bool removeSubWindow(KviKvsObjectFunctionCall * c);
	bool cascade(KviKvsObjectFunctionCall * c);
	bool tile(KviKvsObjectFunctionCall * c);
	bool closeActiveWindow(KviKvsObjectFunctionCall * c);
	bool closeAllWindows(KviKvsObjectFunctionCall * c);
	bool activateNextWindow(KviKvsObjectFunctionCall * c);
	bool activatePrevWindow(KviKvsObjectFunctionCall * c);
	bool scrollBarsEnabled(KviKvsObjectFunctionCall * c);
	bool setscrollBarsEnabled(KviKvsObjectFunctionCall * c);

	bool activeWindow(KviKvsObjectFunctionCall * c);
};

#endif //!_CLASS_WORKSPACE_H_
