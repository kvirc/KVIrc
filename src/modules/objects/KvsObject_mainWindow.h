#ifndef _CLASS_MAINWINDOW_H_
#define _CLASS_MAINWINDOW_H_
//=============================================================================
//
//   File : KvsObject_mainWindow.h
//   Creation date : Mon Feb 28 14:21:48 CEST 2005
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
#include "object_macros.h"

class KvsObject_mainWindow : public KvsObject_widget
{
public:
	KVSO_DECLARE_OBJECT(KvsObject_mainWindow)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	bool init(KviKvsRunTimeContext * pContext, KviKvsVariantList * pParams) override;

	bool setCentralWidget(KviKvsObjectFunctionCall * c);
};

#endif //!_CLASS_MAINWINDOW_H_
