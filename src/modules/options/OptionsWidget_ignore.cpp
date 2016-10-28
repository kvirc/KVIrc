//=============================================================================
//
//   File : OptionsWidget_ignore.cpp
//   Creation date : Thu Feb 12 15:11:29 2002 GMT by Juanjo Álvarez
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002 Juanjo Álvarez
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "OptionsWidget_ignore.h"

#include "KviOptions.h"
#include "KviLocale.h"

#include <QLayout>

OptionsWidget_ignore::OptionsWidget_ignore(QWidget * parent)
    : KviOptionsWidget(parent)
{
	setObjectName("ignore_options_widget");
	createLayout();

	KviTalGroupBox * g = addGroupBox(0, 0, 0, 0, Qt::Horizontal, __tr2qs_ctx("Enable Ignore for", "options"));
	m_pIgnorePrivmsg = addBoolSelector(g, __tr2qs_ctx("Private/Channel messages", "options"), KviOption_boolEnableIgnoreOnPrivMsg);
	m_pIgnoreNotice = addBoolSelector(g, __tr2qs_ctx("Private/Channel notices", "options"), KviOption_boolEnableIgnoreOnNotice);

	connect(m_pIgnorePrivmsg, SIGNAL(toggled(bool)), this, SLOT(enableVerbose(bool)));

	m_pVerboseIgnore = addBoolSelector(0, 1, 0, 1, __tr2qs_ctx("Use verbose ignore (show messages in console)", "options"),
	    KviOption_boolVerboseIgnore,
	    KVI_OPTION_BOOL(KviOption_boolEnableIgnoreOnPrivMsg) || KVI_OPTION_BOOL(KviOption_boolEnableIgnoreOnNotice));

	addRowSpacer(0, 2, 0, 2);
}

OptionsWidget_ignore::~OptionsWidget_ignore()
    = default;

void OptionsWidget_ignore::enableVerbose(bool)
{
	m_pVerboseIgnore->setEnabled(m_pIgnorePrivmsg->isChecked() || m_pIgnoreNotice->isChecked());
}
