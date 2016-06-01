#ifndef _OPTW_ANTISPAM_H_
#define _OPTW_ANTISPAM_H_
//=============================================================================
//
//   File : OptionsWidget_antiSpam.h
//   Creation date : Tue Oct 16 15:47:03 2001 GMT by Szymon Stefanek
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

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_antispam KviIconManager::Spam
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_antispam __tr2qs_no_lookup("Anti-spam")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_antispam OptionsWidget_protection

class OptionsWidget_antispam : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_antispam(QWidget * parent);
	~OptionsWidget_antispam();

protected:
	KviBoolSelector * m_b1;
	KviBoolSelector * m_b2;
	KviBoolSelector * m_b3;
	KviStringListSelector * m_sl;
protected slots:
	void reenableStuff(bool);
};

#endif //_OPTW_ANTISPAM_H_
