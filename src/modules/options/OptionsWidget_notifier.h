#ifndef _OPTW_NOTIFIER_H_
#define _OPTW_NOTIFIER_H_
//=============================================================================
//
//   File : OptionsWidget_notifier.h
//   Creation date : Thu Jul 09 2009 10:54:39 CEST by Fabio Bas
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

#include <QComboBox>

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_notifierLook KviIconManager::Notifier
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_notifierLook __tr2qs_no_lookup("Notifier")
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_notifierLook __tr2qs_no_lookup("theme,colors,text")
#define KVI_OPTIONS_WIDGET_GROUP_OptionsWidget_notifierLook "theme"

class OptionsWidget_notifierLook : public KviOptionsWidget
{
	Q_OBJECT
private:
	QComboBox * m_pHorizontalAlign;
	QComboBox * m_pVerticalAlign;

public:
	OptionsWidget_notifierLook(QWidget * parent);
	~OptionsWidget_notifierLook();

	virtual void commit();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_notifier KviIconManager::Notifier
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_notifier __tr2qs_no_lookup("Notifier")
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_notifier __tr2qs_no_lookup("popup")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_notifier OptionsWidget_interfaceFeatures

class OptionsWidget_notifier : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_notifier(QWidget * parent);
	~OptionsWidget_notifier();

private:
#ifdef COMPILE_KDE_SUPPORT
	KviBoolSelector * m_pKdeNotifier;
#endif
#ifdef COMPILE_DBUS_SUPPORT
	KviBoolSelector * m_pDBusNotifier;
#endif
#ifdef COMPILE_KDE_SUPPORT
private slots:
	void toggleNotifierProtocol(bool);
#endif
};

#endif //!_OPTW_NOTIFIER_H_
