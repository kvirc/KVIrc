#ifndef _CLASS_datetimeedit_H_
#define _CLASS_datetimeedit_H_
//=============================================================================
//
//   File : class_datetimeedit.h
//   Creation date : Wed 30 Gen 2009 09:30:05 CEST by Carbone Alessandro
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2009 Carbone Alessandro (elfonol at gmail dot com)
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================



#include "class_widget.h"
#include "object_macros.h"

class KviKvsObject_datetimeedit : public KviKvsObject_widget
{
	Q_OBJECT
public:
        KVSO_DECLARE_OBJECT(KviKvsObject_datetimeedit)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	virtual bool init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams);

        bool date(KviKvsObjectFunctionCall *c);
        bool setDate(KviKvsObjectFunctionCall *c);
        bool time(KviKvsObjectFunctionCall *c);
        bool setTime(KviKvsObjectFunctionCall *c);

        /*	bool text(KviKvsObjectFunctionCall *c);

	bool setImage(KviKvsObjectFunctionCall *c);
	bool clickEvent(KviKvsObjectFunctionCall *c);
*/
protected slots:
        //void slotClicked();
};

#endif	// !_CLASS_datetimeedit_H_
