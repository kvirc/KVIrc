#ifndef _CLASS_CHECKBOX_H_
#define _CLASS_CHECKBOX_H_
//=============================================================================
//
//   File : KvsObject_checkBox.h
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

#include "KvsObject_button.h"
#include "object_macros.h"

class KvsObject_checkBox : public KvsObject_button
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KvsObject_checkBox)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	bool init(KviKvsRunTimeContext * pContext, KviKvsVariantList * pParams) override;
	bool setChecked(KviKvsObjectFunctionCall * c);
	bool isChecked(KviKvsObjectFunctionCall * c);
	bool toggleEvent(KviKvsObjectFunctionCall * c);
	bool setText(KviKvsObjectFunctionCall * c);
protected slots:
	void toggled(bool b);
};
#endif // !_CLASS_CHECKBOX_H_
