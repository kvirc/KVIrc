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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#include "optw_protection.h"

#include "kvi_settings.h"
#include "kvi_options.h"
#include "kvi_locale.h"

#include <QLayout>

KviProtectionOptionsWidget::KviProtectionOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("protection_options_widget");

	createLayout();

	QLabel * l;
	l = addLabel(0,0,0,0,__tr2qs_ctx("This section contains irc protection tools<br>"\
		"<p>like<b> flood, ignore, antispam. </b> " \
		"</p>","options"));

}

KviProtectionOptionsWidget::~KviProtectionOptionsWidget()
{
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_optw_protection.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
