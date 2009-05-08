//=============================================================================
//
//   File : optw_protection.cpp
//   Creation date : Wed Ott 19 16:15:16 2005 GMT by Grifisx & Noldor
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2005 Grifisx & Noldor
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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "optw_protection.h"

#include <qlayout.h>

#include "kvi_settings.h"
#include "kvi_options.h"
#include "kvi_locale.h"

KviProtectionOptionsWidget::KviProtectionOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent,"protection_options_widget")
{
	
	createLayout(1,1);

	QLabel * l;
	l = addLabel(0,0,0,0,__tr2qs_ctx("This section contains irc protection tools<br>"\
		"<p>like<b> flood, ignore, antispam. </b> " \
		"</p>","options"));

}

KviProtectionOptionsWidget::~KviProtectionOptionsWidget()
{
}

#include "m_optw_protection.moc"
