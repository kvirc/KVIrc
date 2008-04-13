#ifndef	_CLASS_LABEL_H_
#define	_CLASS_LABEL_H_
//vim: ts=8
//   File : class_label.h
//   Creation date : Mon Sep 18 14:21:48 CEST 2000 by Krzysztof Godlewski
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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



#include "class_widget.h"
#include "object_macros.h"

class KviKvsObject_label : public KviKvsObject_widget
{
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_label)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	virtual bool init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams);

	bool functionSetTitle(KviKvsObjectFunctionCall *c);
	bool functionSetText(KviKvsObjectFunctionCall *c);
	bool functionText(KviKvsObjectFunctionCall *c);
	bool functionMargin(KviKvsObjectFunctionCall *c);
	bool functionSetMargin(KviKvsObjectFunctionCall *c);
	bool functionAutoResize(KviKvsObjectFunctionCall *c);
	bool functionSetAutoResize(KviKvsObjectFunctionCall *c);
	bool functionAlignment(KviKvsObjectFunctionCall *c);
	bool functionSetAlignment(KviKvsObjectFunctionCall *c);
	bool functionClear(KviKvsObjectFunctionCall *c);
	bool functionFrameStyle(KviKvsObjectFunctionCall *c);
	bool functionSetFrameStyle(KviKvsObjectFunctionCall *c);
	bool functionSetImage(KviKvsObjectFunctionCall *c);
};
#endif	// !_CLASS_LABEL_H_
