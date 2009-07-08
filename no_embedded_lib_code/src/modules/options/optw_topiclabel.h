#ifndef _OPTW_TOPICLABEL_H
#define _OPTW_TOPICLABEL_H
//=============================================================================
//
//   File : optw_topiclabel.h
//   Creation date : Mon Sep 24 02:44:55 CEST 2001 by Krzysztof Godlewski
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Krzysztof Godlewski
//   Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
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


#include "kvi_optionswidget.h"

#define KVI_OPTIONS_WIDGET_ICON_KviTopicLabelLookOptionsWidget KVI_SMALLICON_TOPIC
#define KVI_OPTIONS_WIDGET_NAME_KviTopicLabelLookOptionsWidget __tr2qs_no_lookup("Labels")
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviTopicLabelLookOptionsWidget __tr2qs_no_lookup("theme,topic")
#define KVI_OPTIONS_WIDGET_GROUP_KviTopicLabelLookOptionsWidget "theme"

class KviTopicLabelLookOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviTopicLabelLookOptionsWidget(QWidget *);
	~KviTopicLabelLookOptionsWidget();
};

#endif // _OPTW_TOPICLABEL_H
