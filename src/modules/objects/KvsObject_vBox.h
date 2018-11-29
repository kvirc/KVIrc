#ifndef _CLASS_VBOX_H_
#define _CLASS_VBOX_H_
//=============================================================================
//
//   File : KvsObject_vBox.h
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

#include "KviCString.h"
#include "KvsObject_widget.h"
#include "object_macros.h"

class KvsObject_vBox : public KvsObject_widget
{
public:
	KVSO_DECLARE_OBJECT(KvsObject_vBox)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	bool init(KviKvsRunTimeContext * pContext, KviKvsVariantList * pParams) override;
	bool setMargin(KviKvsObjectFunctionCall * c);
	bool setSpacing(KviKvsObjectFunctionCall * c);
	bool setStretchFactor(KviKvsObjectFunctionCall * c);
	bool addStretch(KviKvsObjectFunctionCall * c);
	bool setAlignment(KviKvsObjectFunctionCall * c);
};

#endif //_CLASS_HBOX_H_
