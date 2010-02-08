#ifndef _OPTW_INTERFACELOOKGLOBAL_H_
#define _OPTW_INTERFACELOOKGLOBAL_H_
//=============================================================================
//
//   File : optw_interfacelookglobal.h
//   Creation date : Sun Jun 3 2001 20:49:02 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"
#include "kvi_optionswidget.h"
#include "kvi_selectors.h"


#define KVI_OPTIONS_WIDGET_ICON_KviThemeGeneralOptionsWidget KVI_SMALLICON_GUI
#define KVI_OPTIONS_WIDGET_NAME_KviThemeGeneralOptionsWidget __tr2qs_no_lookup("General")
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviThemeGeneralOptionsWidget __tr2qs_no_lookup("theme")
#define KVI_OPTIONS_WIDGET_GROUP_KviThemeGeneralOptionsWidget "theme"


class KviThemeGeneralOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviThemeGeneralOptionsWidget(QWidget * parent);
	~KviThemeGeneralOptionsWidget();
};


#define KVI_OPTIONS_WIDGET_ICON_KviThemeTransparencyOptionsWidget KVI_SMALLICON_TRANSPARENT
#define KVI_OPTIONS_WIDGET_NAME_KviThemeTransparencyOptionsWidget __tr2qs_no_lookup("Transparency")
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviThemeTransparencyOptionsWidget __tr2qs_no_lookup("theme,background")
#define KVI_OPTIONS_WIDGET_GROUP_KviThemeTransparencyOptionsWidget "theme"


class KviThemeTransparencyOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviThemeTransparencyOptionsWidget(QWidget * parent);
	~KviThemeTransparencyOptionsWidget();
protected:
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	KviPixmapSelector * m_pGlobalBackgroundPixmapSelector;
	KviBoolSelector * m_pUseTransparencyBoolSelector;
	#ifdef COMPILE_X11_SUPPORT
		KviBoolSelector * m_pUseCompositingForTransparencyBoolSelector;
	#elif defined(COMPILE_ON_MINGW) // || defined(COMPILE_ON_WINDOWS)
		KviBoolSelector * m_pUseWindowsFakeDesktopTransparencyBoolSelector;
	#endif
#endif
protected slots:
	void enableGlobalBackgroundPixmapSelector(bool);
};

#define KVI_OPTIONS_WIDGET_ICON_KviThemeMdiAreaOptionsWidget KVI_SMALLICON_APPLET
#define KVI_OPTIONS_WIDGET_NAME_KviThemeMdiAreaOptionsWidget __tr2qs_no_lookup("Workspace")
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviThemeMdiAreaOptionsWidget __tr2qs_no_lookup("theme,mdi")
#define KVI_OPTIONS_WIDGET_GROUP_KviThemeMdiAreaOptionsWidget "theme"

class KviThemeMdiAreaOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviThemeMdiAreaOptionsWidget(QWidget * parent);
	~KviThemeMdiAreaOptionsWidget();
};

#define KVI_OPTIONS_WIDGET_ICON_KviThemeToolBarAppletsOptionsWidget KVI_SMALLICON_APPLET
#define KVI_OPTIONS_WIDGET_NAME_KviThemeToolBarAppletsOptionsWidget __tr2qs_no_lookup("ToolBar Applets")
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviThemeToolBarAppletsOptionsWidget __tr2qs_no_lookup("theme,toolbar")
#define KVI_OPTIONS_WIDGET_GROUP_KviThemeToolBarAppletsOptionsWidget "theme"


class KviThemeToolBarAppletsOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviThemeToolBarAppletsOptionsWidget(QWidget * parent);
	~KviThemeToolBarAppletsOptionsWidget();
};

#endif //!_OPTW_INTERFACELOOKGLOBAL_H_
