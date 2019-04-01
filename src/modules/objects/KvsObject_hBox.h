#ifndef _CLASS_HBOX_H_
#define _CLASS_HBOX_H_
//=============================================================================
//
//   File : KvsObject_hBox.h
//   Creation date : Wed Mar 01 2005 23:00:01 by Alessandro Carbone & Tonino Imbesi
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2009 Alessandro Carbone (elfonol at gmail dot com)
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

#include "object_macros.h"
#include "KvsObject_widget.h"

class KvsObject_hBox : public KvsObject_widget
{
public:
	KVSO_DECLARE_OBJECT(KvsObject_hBox)
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
