#ifndef _OPTW_QUERY_H_
#define _OPTW_QUERY_H_
//
//   File : optw_query.h
//   Creation date : Tue Mar 05 22:31:18 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Szymon Stefanek (pragma@kvirc.net)
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

#include "kvi_optionswidget.h"

#define KVI_OPTIONS_WIDGET_ICON_KviQueryOptionsWidget KVI_SMALLICON_QUERY
#define KVI_OPTIONS_WIDGET_NAME_KviQueryOptionsWidget __tr2qs_no_lookup("Query")
#define KVI_OPTIONS_WIDGET_PARENT_KviQueryOptionsWidget KviIrcOptionsWidget
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviQueryOptionsWidget __tr2qs_no_lookup("chat")
#define KVI_OPTIONS_WIDGET_PRIORITY_KviQueryOptionsWidget 80000
#define KVI_OPTIONS_WIDGET_NOTCONTAINED_KviQueryOptionsWidget true

class KviQueryOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviQueryOptionsWidget(QWidget * parent);
	~KviQueryOptionsWidget();
protected: 
};

#endif //_OPTW_away_H_
