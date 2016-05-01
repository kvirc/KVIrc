//=============================================================================
//
//   File : OptionsWidget_topicLabel.cpp
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

#include "OptionsWidget_topicLabel.h"

#include "KviOptions.h"
#include "KviLocale.h"

OptionsWidget_topicLabel::OptionsWidget_topicLabel(QWidget * p) : KviOptionsWidget(p, "topic_label_look_options_widget")
{
	createLayout();

	addFontSelector(0, 0, 0, 0, __tr2qs_ctx("Font:", "options"), KviOption_fontLabel);
	addColorSelector(0, 1, 0, 1, __tr2qs_ctx("Foreground color:", "options"), KviOption_colorLabelForeground);
	addColorSelector(0, 2, 0, 2, __tr2qs_ctx("Background color:", "options"), KviOption_colorLabelBackground);
	addPixmapSelector(0, 3, 0, 3, __tr2qs_ctx("Background image:", "options"), KviOption_pixmapLabelBackground);

	layout()->setRowStretch(3, 1);
}

OptionsWidget_topicLabel::~OptionsWidget_topicLabel()
    = default;
