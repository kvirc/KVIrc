#ifndef _OPTW_TOPICLABEL_H
#define _OPTW_TOPICLABEL_H
//=============================================================================
//
//   File : OptionsWidget_topicLabel.h
//   Creation date : Mon Sep 24 02:44:55 CEST 2001 by Krzysztof Godlewski
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001 Krzysztof Godlewski
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

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_topicLabel KviIconManager::Topic
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_topicLabel __tr2qs_no_lookup("Labels")
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_topicLabel __tr2qs_no_lookup("theme,topic")
#define KVI_OPTIONS_WIDGET_GROUP_OptionsWidget_topicLabel "theme"

class OptionsWidget_topicLabel : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_topicLabel(QWidget *);
	~OptionsWidget_topicLabel();
};

#endif // _OPTW_TOPICLABEL_H
