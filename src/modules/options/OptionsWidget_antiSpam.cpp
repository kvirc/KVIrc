//=============================================================================
//
//   File : OptionsWidget_antiSpam.cpp
//   Creation date : Tue Oct 16 15:47:02 2001 GMT by Szymon Stefanek
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

#include "OptionsWidget_antiSpam.h"

#include "KviOptions.h"
#include "KviLocale.h"

#include <QLayout>
#include <QLabel>

OptionsWidget_antispam::OptionsWidget_antispam(QWidget * parent)
    : KviOptionsWidget(parent)
{
	setObjectName("antispam_options_widget");
	createLayout();

	KviTalGroupBox * g = addGroupBox(0, 0, 0, 0, Qt::Horizontal, __tr2qs_ctx("Enable Anti-spam for", "options"));
	m_b1 = addBoolSelector(g, __tr2qs_ctx("Private messages", "options"), KviOption_boolUseAntiSpamOnPrivmsg);
	connect(m_b1, SIGNAL(toggled(bool)), this, SLOT(reenableStuff(bool)));
	m_b2 = addBoolSelector(g, __tr2qs_ctx("Private notices", "options"), KviOption_boolUseAntiSpamOnNotice);
	connect(m_b2, SIGNAL(toggled(bool)), this, SLOT(reenableStuff(bool)));

	m_b3 = addBoolSelector(0, 1, 0, 1, __tr2qs_ctx("Silent anti-spam (no warnings)", "options"), KviOption_boolSilentAntiSpam);
	m_sl = addStringListSelector(0, 2, 0, 2, __tr2qs_ctx("Words considered spam:", "options"), KviOption_stringlistSpamWords);

	reenableStuff(true);

	layout()->setRowStretch(2, 1);
}

OptionsWidget_antispam::~OptionsWidget_antispam()
    = default;

void OptionsWidget_antispam::reenableStuff(bool)
{
	m_b3->setEnabled(m_b1->isChecked() || m_b2->isChecked());
	m_sl->setEnabled(m_b1->isChecked() || m_b2->isChecked());
}
