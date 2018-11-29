#ifndef _CLASS_datetimeedit_H_
#define _CLASS_datetimeedit_H_
//=============================================================================
//
//   File : KvsObject_dateTimeEdit.h
//   Creation date : Wed 30 Gen 2009 09:30:05 CEST by Carbone Alessandro
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2009 Carbone Alessandro (elfonol at gmail dot com)
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

class QDateTime;
class QDate;
class QTime;

class KvsObject_dateTimeEdit : public KvsObject_widget
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KvsObject_dateTimeEdit)
public:
	QWidget * widget() { return (QWidget *)object(); };
protected:
	bool init(KviKvsRunTimeContext * pContext, KviKvsVariantList * pParams) override;

	bool date(KviKvsObjectFunctionCall * c);
	bool setDate(KviKvsObjectFunctionCall * c);
	bool time(KviKvsObjectFunctionCall * c);
	bool setTime(KviKvsObjectFunctionCall * c);

	bool timeChangedEvent(KviKvsObjectFunctionCall * c);
	bool dateTimeChangedEvent(KviKvsObjectFunctionCall * c);
	bool dateChangedEvent(KviKvsObjectFunctionCall * c);

protected slots:
	void slotDateTimeChanged(const QDateTime &);
	void slotDateChanged(const QDate &);
	void slotTimeChanged(const QTime &);
};

#endif // !_CLASS_datetimeedit_H_
