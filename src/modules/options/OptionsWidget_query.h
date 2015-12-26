#ifndef _OPTW_QUERY_H_
#define _OPTW_QUERY_H_
//=============================================================================
//
//   File : OptionsWidget_query.h
//   Creation date : Tue Mar 05 22:31:18 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma@kvirc.net)
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

#include "KviOptionsWidget.h"

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_query KviIconManager::Query
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_query __tr2qs_no_lookup("Query")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_query OptionsWidget_irc
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_query __tr2qs_no_lookup("chat")
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_query 80000
#define KVI_OPTIONS_WIDGET_NOTCONTAINED_OptionsWidget_query true

class OptionsWidget_query : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_query(QWidget * parent);
	~OptionsWidget_query();
};

#endif //_OPTW_QUERY_H_
