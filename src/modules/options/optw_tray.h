#ifndef _OPTW_TRAY_H_
#define _OPTW_TRAY_H_
//
//   File : optw_tray.h
//   Creation date : Mon 14 Aug 2006 by Alexey Uzhva
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_optionswidget.h"
#include <qcombobox.h>

#define KVI_OPTIONS_WIDGET_ICON_KviTrayOptionsWidget KVI_SMALLICON_TRAYICON
#define KVI_OPTIONS_WIDGET_NAME_KviTrayOptionsWidget __tr2qs_no_lookup("Tray icon")
#define KVI_OPTIONS_WIDGET_PARENT_KviTrayOptionsWidget KviInterfaceFeaturesOptionsWidget
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviTrayOptionsWidget __tr2qs_no_lookup("tray,TRAYICON")

class KviTrayOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviTrayOptionsWidget(QWidget * parent);
	~KviTrayOptionsWidget();
protected:
	KviBoolSelector * m_pEnable;
	KviBoolSelector * m_pEnableFlashing;
	KviBoolSelector * m_pLevelBasedNotify;
	KviBoolSelector * m_pMinimizeInTray;
	KviBoolSelector * m_pCloseInTray;
	KviTalGroupBox * m_pLevelsGroup;
protected slots:
	void setEnabled(bool);
};

#endif //_OPTW_TRAY_H_
