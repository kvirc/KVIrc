//=============================================================================
//
//   File : optw_ignore.h
//   Creation date : Thu Feb 12 15:11:29 2002 GMT by Juanjo Álvarez
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Juanjo Álvarez
//   Copyright (C) 2002-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "optw_ignore.h"

#include "kvi_options.h"
#include "kvi_locale.h"

#include <QLayout>


KviIgnoreOptionsWidget::KviIgnoreOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("ignore_options_widget");
	createLayout();

	KviTalGroupBox *g = addGroupBox(0,0,0,0,Qt::Horizontal,__tr2qs_ctx("Enable Ignore For","options"));
	m_pIgnorePrivmsg = addBoolSelector(g,__tr2qs_ctx("Private/channel messages","options"),KviOption_boolEnableIgnoreOnPrivMsg);
	m_pIgnoreNotice  = addBoolSelector(g,__tr2qs_ctx("Private/channel notices","options"),KviOption_boolEnableIgnoreOnNotice);

	connect(m_pIgnorePrivmsg,SIGNAL(toggled(bool)),this,SLOT(enableVerbose(bool)));

	m_pVerboseIgnore = addBoolSelector(0,1,0,1,__tr2qs_ctx("Use verbose ignore (show messages in console)","options"),
				KviOption_boolVerboseIgnore,
				KVI_OPTION_BOOL(KviOption_boolEnableIgnoreOnPrivMsg) || KVI_OPTION_BOOL(KviOption_boolEnableIgnoreOnNotice));

	addRowSpacer(0,2,0,2);
}

KviIgnoreOptionsWidget::~KviIgnoreOptionsWidget()
{
}

void KviIgnoreOptionsWidget::enableVerbose(bool)
{
	m_pVerboseIgnore->setEnabled(m_pIgnorePrivmsg->isChecked() || m_pIgnoreNotice->isChecked());
}



#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_optw_ignore.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
