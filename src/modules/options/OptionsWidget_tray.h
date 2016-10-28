#ifndef _OPTW_TRAY_H_
#define _OPTW_TRAY_H_
//=============================================================================
//
//   File : OptionsWidget_tray.h
//   Creation date : Mon 14 Aug 2006 by Alexey Uzhva
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2006-2008 Alexey Uzhva (wizard at opendoor dot ru)
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

#include <QComboBox>

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_tray KviIconManager::TrayIcon
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_tray __tr2qs_no_lookup("Tray Icon")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_tray OptionsWidget_interfaceFeatures
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_tray __tr2qs_no_lookup("tray,TRAYICON")

class OptionsWidget_tray : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_tray(QWidget * parent);
	~OptionsWidget_tray();

protected:
	KviBoolSelector * m_pEnable;
	KviBoolSelector * m_pEnableFlashing;
	KviBoolSelector * m_pLevelBasedNotify;
#ifndef COMPILE_ON_MAC
	KviBoolSelector * m_pMinimizeInTray;
#endif
	KviBoolSelector * m_pCloseInTray;
	KviTalGroupBox * m_pLevelsGroup;
protected slots:
	void setEnabled(bool);
};

#endif //_OPTW_TRAY_H_
