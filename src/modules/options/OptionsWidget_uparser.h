#ifndef _OPTW_UPARSER_H_
#define _OPTW_UPARSER_H_
//=============================================================================
//
//   File : OptionsWidget_uparser.h
//   Creation date : Sat Oct 27 16:32:27 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_uparser KviIconManager::ScriptCenter;
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_uparser __tr2qs_no_lookup("Scripting Engine")
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_uparser __tr2qs_no_lookup("scripting,kvs,debug")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_uparser OptionsWidget_general

class OptionsWidget_uparser : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_uparser(QWidget * parent);
	~OptionsWidget_uparser();
};

#endif //_OPTW_UPARSER_H_
