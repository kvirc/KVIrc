#ifndef _CLASS_HBOX_H_
#define _CLASS_HBOX_H_
//
//   File : class_hbox.h
//   Creation date : Wed Mar 01 2005 23:00:01 by Alessandro Carbone & Tonino Imbesi
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2006 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_string.h"

#include "kvi_tal_hbox.h"
#include "object_macros.h"
#include "class_widget.h"

class KviKvsObject_hbox : public KviKvsObject_widget
{
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_hbox)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	virtual bool init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams);

	bool functionsetMargin(KviKvsObjectFunctionCall *c);
	bool functionsetSpacing(KviKvsObjectFunctionCall *c);
	bool functionsetStretchFactor(KviKvsObjectFunctionCall *c);


};


#endif //_CLASS_HBOX_H_
