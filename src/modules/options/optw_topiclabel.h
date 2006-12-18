//
//   File : optw_topiclabel.cpp
//   Creation date : Mon Sep 24 02:44:55 CEST 2001 by Krzysztof Godlewski
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
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

#ifndef OPTW_TOPICLABEL_H
#define OPTW_TOPICLABEL_H

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

#endif	/* !OPTW_TOPICLABEL_H */
