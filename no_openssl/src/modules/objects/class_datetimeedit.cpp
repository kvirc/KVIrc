//=============================================================================
//
//   File : class_datetimeedit.cpp
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

#include "kvi_debug.h"
#include "kvi_locale.h"
#include "kvi_error.h"
#include "kvi_iconmanager.h"
#include "kvi_file.h"

#include "class_datetimeedit.h"
#include "class_pixmap.h"

 #include <QDateTimeEdit>

/*
        @doc:datetimeedit
	@title:
                datetimeedit class
	@type:
		class
	@short:
                Datetimeedit widget.
	@inherits:
		[class]object[/class]
                [class]widget[/class]
	@description:
                This widget provides a datetimeedit.
	@functions:
                !fn: $setDate(<text:string>,<format:string>)
                Sets the date of the widget using the format <format>. Possible values for format are:
                d the day as number without a leading zero (1 to 31)
                dd the day as number with a leading zero (01 to 31)
                ddd the abbreviated localized day name (e.g. 'Mon' to 'Sun'). Uses QDate::shortDayName().
                dddd the long localized day name (e.g. 'Monday' to 'Qt::Sunday'). Uses QDate::longDayName().
                M the month as number without a leading zero (1-12)
                MM the month as number with a leading zero (01-12)
                MMM the abbreviated localized month name (e.g. 'Jan' to 'Dec'). Uses QDate::shortMonthName().
                MMMM the long localized month name (e.g. 'January' to 'December'). Uses QDate::longMonthName().
                yy the year as two digit number (00-99)
                yyyy the year as four digit number
                !fn: <string> $date(<format:string>)
                Returns the widget date according to format.
                !fn: $setTime(<text:string>,<format:string>)
                Sets the date of the widget using the format <format>. Possibile values for format are:
                h the hour without a leading zero
                hh the hour with a leading zero
                m the minute without a leading zero
                mm the minute with a leading zero
                s the second without a leading zero
                ss the second with a leading zero
                z the milliseconds without leading zeroes
                zzz the milliseconds with leading zeroes
                AP(or ap) use AM/PM display. AP will be replaced by either "AM" or "PM".
                !fn: $time(<format:string>)
                Returns the widget time according to format.
*/


KVSO_BEGIN_REGISTERCLASS(KviKvsObject_datetimeedit,"datetimeedit","widget")

        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_datetimeedit,date)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_datetimeedit,setDate)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_datetimeedit,setTime)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_datetimeedit,time)

KVSO_END_REGISTERCLASS(KviKvsObject_datetimeedit)

KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_datetimeedit,KviKvsObject_widget)

KVSO_END_CONSTRUCTOR(KviKvsObject_datetimeedit)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_datetimeedit)

KVSO_END_DESTRUCTOR(KviKvsObject_datetimeedit)

bool KviKvsObject_datetimeedit::init(KviKvsRunTimeContext *,KviKvsVariantList *)
{
        QDateTimeEdit *obj=new QDateTimeEdit(QDate::currentDate(),parentScriptWidget());
        obj->setObjectName(getName());
        setObject(obj,true);
	return true;
}

KVSO_CLASS_FUNCTION(datetimeedit,date)
{

        CHECK_INTERNAL_POINTER(widget())
        QString szFormat;
        KVSO_PARAMETERS_BEGIN(c)
                KVSO_PARAMETER("format",KVS_PT_STRING,0,szFormat)
        KVSO_PARAMETERS_END(c)

        c->returnValue()->setString(((QDateTimeEdit *)widget())->date().toString(szFormat));

        return true;
}
KVSO_CLASS_FUNCTION(datetimeedit,setDate)
{

        CHECK_INTERNAL_POINTER(widget())
        QString szFormat,szDate;
        KVSO_PARAMETERS_BEGIN(c)
                KVSO_PARAMETER("format",KVS_PT_STRING,0,szFormat)
                KVSO_PARAMETER("date",KVS_PT_STRING,0,szDate)
        KVSO_PARAMETERS_END(c)
        QDate date;
        date=date.fromString(szDate,szFormat);
        ((QDateTimeEdit *)widget())->setDate(date);
        return true;
}
KVSO_CLASS_FUNCTION(datetimeedit,time)
{

        CHECK_INTERNAL_POINTER(widget())
        QString szFormat;
        KVSO_PARAMETERS_BEGIN(c)
                KVSO_PARAMETER("format",KVS_PT_STRING,0,szFormat)
        KVSO_PARAMETERS_END(c)

        c->returnValue()->setString(((QDateTimeEdit *)widget())->time().toString(szFormat));

        return true;
}
KVSO_CLASS_FUNCTION(datetimeedit,setTime)
{

        CHECK_INTERNAL_POINTER(widget())
        QString szFormat,szTime;
        KVSO_PARAMETERS_BEGIN(c)
                KVSO_PARAMETER("format",KVS_PT_STRING,0,szFormat)
                KVSO_PARAMETER("date",KVS_PT_STRING,0,szTime)
        KVSO_PARAMETERS_END(c)
        QTime time;
        time=time.fromString(szTime,szFormat);
        ((QDateTimeEdit *)widget())->setTime(time);
        return true;
}
#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_class_datetimeedit.moc"
#endif //COMPILE_USE_STANDALONE_MOC_SOURCES
