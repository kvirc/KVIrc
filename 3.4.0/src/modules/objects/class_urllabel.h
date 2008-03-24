#ifndef _CLASS_URLLABEL_H_
#define _CLASS_URLLABEL_H_
//vim: ts=8
//   File : class_urllabel.h
//   Creation date : Thu Feb 18 1:28:23 CEST 2001 by Krzysztof Godlewski
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Krzysztof Godlewski
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



#include "class_label.h"
#include "object_macros.h"

class KviKvsObject_urlabel : public KviKvsObject_label
{
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_urlabel)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	virtual bool eventFilter(QObject *, QEvent *);

    bool functionsetUrl(KviKvsObjectFunctionCall *c);
	bool functionsetAction(KviKvsObjectFunctionCall *c);
	bool functionurl(KviKvsObjectFunctionCall *c);
	bool functionaction(KviKvsObjectFunctionCall *c);
	bool functionsetText(KviKvsObjectFunctionCall *c);
	bool functionsetCursorChange(KviKvsObjectFunctionCall *c);
	bool functioncursorChange(KviKvsObjectFunctionCall *c);
	bool functionsetUseSingleClick(KviKvsObjectFunctionCall *c);
	bool functionuseSingleClick(KviKvsObjectFunctionCall *c);

	QString m_url;
	QString m_action;

	QColor m_normalClr;
	QColor m_enterClr;
	QColor m_activeClr;

	bool m_changeCursor;
	bool m_useSingleClick;

	bool m_lastClickWasDbl;

};

#endif	// !_CLASS_URLLABEL_H_
