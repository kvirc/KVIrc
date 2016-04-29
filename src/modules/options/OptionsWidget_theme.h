#ifndef _OPTW_INTERFACELOOKGLOBAL_H_
#define _OPTW_INTERFACELOOKGLOBAL_H_
//=============================================================================
//
//   File : OptionsWidget_theme.h
//   Creation date : Sun Jun 3 2001 20:49:02 CEST by Szymon Stefanek
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

#include "kvi_settings.h"
#include "KviOptionsWidget.h"
#include "KviSelectors.h"

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_theme KviIconManager::Gui
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_theme __tr2qs_no_lookup("General")
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_theme __tr2qs_no_lookup("theme")
#define KVI_OPTIONS_WIDGET_GROUP_OptionsWidget_theme "theme"

class OptionsWidget_theme : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_theme(QWidget * parent);
	~OptionsWidget_theme();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_themeTransparency KviIconManager::Transparent
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_themeTransparency __tr2qs_no_lookup("Transparency")
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_themeTransparency __tr2qs_no_lookup("theme,background")
#define KVI_OPTIONS_WIDGET_GROUP_OptionsWidget_themeTransparency "theme"

class OptionsWidget_themeTransparency : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_themeTransparency(QWidget * parent);
	~OptionsWidget_themeTransparency();

protected:
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	KviPixmapSelector * m_pGlobalBackgroundPixmapSelector;
	KviBoolSelector * m_pUseTransparencyBoolSelector;
#ifdef COMPILE_X11_SUPPORT
	KviBoolSelector * m_pUseCompositingForTransparencyBoolSelector;
#elif defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	KviBoolSelector * m_pUseWindowsFakeDesktopTransparencyBoolSelector;
#endif
#endif
protected slots:
	void enableGlobalBackgroundPixmapSelector(bool);
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_themeToolBarApplets KviIconManager::Applet
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_themeToolBarApplets __tr2qs_no_lookup("Toolbar Applets")
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_themeToolBarApplets __tr2qs_no_lookup("theme,toolbar")
#define KVI_OPTIONS_WIDGET_GROUP_OptionsWidget_themeToolBarApplets "theme"

class OptionsWidget_themeToolBarApplets : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_themeToolBarApplets(QWidget * parent);
	~OptionsWidget_themeToolBarApplets();
};

#endif //!_OPTW_INTERFACELOOKGLOBAL_H_
