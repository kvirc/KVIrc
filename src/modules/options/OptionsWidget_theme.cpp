//=============================================================================
//
//   File : OptionsWidget_theme.cpp
//   Creation date : Sun Jun 3 2001 20:50:13 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "OptionsWidget_theme.h"

#include "kvi_settings.h"
#include "KviOptions.h"
#include "KviLocale.h"
#include "KviApplication.h"
#include "KviTalToolTip.h"

#include <QLayout>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OptionsWidget_theme::OptionsWidget_theme(QWidget * parent)
: KviOptionsWidget(parent)
{
	createLayout();

	KviBoolSelector * b = addBoolSelector(0,1,1,1,__tr2qs_ctx("Use global application font","options"),KviOption_boolUseGlobalApplicationFont);
	KviTalToolTip::add(b,__tr2qs_ctx("This option will force a static font for every widget that has no specific font setting","options"));
	addBoolSelector(0,0,1,0,__tr2qs_ctx("Show icons in popup menus","options"),KviOption_boolShowIconsInPopupMenus);
	KviFontSelector * f = addFontSelector(0,2,1,2,__tr2qs_ctx("Global application font:","options"),KviOption_fontApplication,
			KVI_OPTION_BOOL(KviOption_boolUseGlobalApplicationFont));

	connect(b,SIGNAL(toggled(bool)),f,SLOT(setEnabled(bool)));

	addRowSpacer(0,3,1,3);
}

OptionsWidget_theme::~OptionsWidget_theme()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


OptionsWidget_themeTransparency::OptionsWidget_themeTransparency(QWidget * parent)
: KviOptionsWidget(parent)
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY

	createLayout();

	KviUIntSelector * u = addUIntSelector(0,0,1,0,__tr2qs_ctx("Global window opacity percent:","options"),KviOption_uintGlobalWindowOpacityPercent,
			50,100,100, true);

	m_pUseTransparencyBoolSelector = addBoolSelector(0,1,1,1,__tr2qs_ctx("Enable transparency","options"),KviOption_boolUseGlobalPseudoTransparency);
	#ifdef COMPILE_X11_SUPPORT
		mergeTip(m_pUseTransparencyBoolSelector,
			__tr2qs_ctx("<center>This option makes all KVIrc windows look " \
				"transparent.<br>You must choose a blending " \
				"background image to below or check the " \
				"\"Use compositing for real transparency\" option.</center>","options"));
	#elif defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		mergeTip(m_pUseTransparencyBoolSelector,
			__tr2qs_ctx("<center>This option makes all KVIrc windows look " \
				"transparent.<br>You must choose a blending " \
				"background image to below or check the " \
				"\"Use desktop background for transparency\" option.</center>","options"));
	#else
		mergeTip(m_pUseTransparencyBoolSelector,
			__tr2qs_ctx("<center>This option makes all KVIrc windows look " \
				"like transparent.<br>You must choose a blending " \
				"background image to below.</center>","options"));
	#endif

	u = addUIntSelector(0,2,1,2,__tr2qs_ctx("Child window opacity:","options"),KviOption_uintGlobalTransparencyChildFadeFactor,
			0,100,35,KVI_OPTION_BOOL(KviOption_boolUseGlobalPseudoTransparency));
	connect(m_pUseTransparencyBoolSelector,SIGNAL(toggled(bool)),u,SLOT(setEnabled(bool)));

	u = addUIntSelector(0,3,1,3,__tr2qs_ctx("Parent window opacity:","options"),KviOption_uintGlobalTransparencyParentFadeFactor,
			0,100,10,KVI_OPTION_BOOL(KviOption_boolUseGlobalPseudoTransparency));
	connect(m_pUseTransparencyBoolSelector,SIGNAL(toggled(bool)),u,SLOT(setEnabled(bool)));

	KviColorSelector * c = addColorSelector(0,4,1,4,__tr2qs_ctx("Blend color:","options"),KviOption_colorGlobalTransparencyFade,
			KVI_OPTION_BOOL(KviOption_boolUseGlobalPseudoTransparency));
	connect(m_pUseTransparencyBoolSelector,SIGNAL(toggled(bool)),c,SLOT(setEnabled(bool)));

	#ifdef COMPILE_X11_SUPPORT
		m_pUseCompositingForTransparencyBoolSelector = addBoolSelector(0,5,1,5,__tr2qs_ctx("Use compositing for real transparency","options"),KviOption_boolUseCompositingForTransparency, KVI_OPTION_BOOL(KviOption_boolUseGlobalPseudoTransparency));

		m_pGlobalBackgroundPixmapSelector = addPixmapSelector(0,6,1,6,__tr2qs_ctx("Transparency blend image:","options"),KviOption_pixmapGlobalTransparencyBackground, KVI_OPTION_BOOL(KviOption_boolUseGlobalPseudoTransparency) && !KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency));
		layout()->setRowStretch(7,1);

		if(g_pApp->supportsCompositing())
		{
			connect(m_pUseTransparencyBoolSelector,SIGNAL(toggled(bool)),m_pUseCompositingForTransparencyBoolSelector,SLOT(setEnabled(bool)));
			connect(m_pUseCompositingForTransparencyBoolSelector,SIGNAL(toggled(bool)),this,SLOT(enableGlobalBackgroundPixmapSelector(bool)));
		} else {
			m_pUseCompositingForTransparencyBoolSelector->setEnabled(false);
			m_pUseCompositingForTransparencyBoolSelector->setChecked(false);
			enableGlobalBackgroundPixmapSelector(true);
		}

	#elif defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		m_pUseWindowsFakeDesktopTransparencyBoolSelector = addBoolSelector(0,5,1,5,__tr2qs_ctx("Use desktop background for transparency","options"),KviOption_boolUseWindowsDesktopForTransparency, KVI_OPTION_BOOL(KviOption_boolUseGlobalPseudoTransparency));

		m_pGlobalBackgroundPixmapSelector = addPixmapSelector(0,6,1,6,__tr2qs_ctx("Transparency blend image:","options"),KviOption_pixmapGlobalTransparencyBackground, KVI_OPTION_BOOL(KviOption_boolUseGlobalPseudoTransparency) && !KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency));
		layout()->setRowStretch(7,1);

		connect(m_pUseTransparencyBoolSelector,SIGNAL(toggled(bool)),m_pUseWindowsFakeDesktopTransparencyBoolSelector,SLOT(setEnabled(bool)));
		connect(m_pUseWindowsFakeDesktopTransparencyBoolSelector,SIGNAL(toggled(bool)),this,SLOT(enableGlobalBackgroundPixmapSelector(bool)));

	#else
		m_pGlobalBackgroundPixmapSelector = addPixmapSelector(0,5,1,5,__tr2qs_ctx("Transparency blend image:","options"),KviOption_pixmapGlobalTransparencyBackground, KVI_OPTION_BOOL(KviOption_boolUseGlobalPseudoTransparency));
		layout()->setRowStretch(6,1);
	#endif //!COMPILE_X11_SUPPORT

	connect(m_pUseTransparencyBoolSelector,SIGNAL(toggled(bool)),this,SLOT(enableGlobalBackgroundPixmapSelector(bool)));
#else //!COMPILE_PSEUDO_TRANSPARENCY
	createLayout();
	addRowSpacer(0,0,0,0);
#endif
}

OptionsWidget_themeTransparency::~OptionsWidget_themeTransparency()
{
}

void OptionsWidget_themeTransparency::enableGlobalBackgroundPixmapSelector(bool)
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	#ifdef COMPILE_X11_SUPPORT
		m_pGlobalBackgroundPixmapSelector->setEnabled(m_pUseTransparencyBoolSelector->isChecked() && !(m_pUseCompositingForTransparencyBoolSelector->isChecked()));
	#elif defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		m_pGlobalBackgroundPixmapSelector->setEnabled(m_pUseTransparencyBoolSelector->isChecked() && !(m_pUseWindowsFakeDesktopTransparencyBoolSelector->isChecked()));
        #else
		m_pGlobalBackgroundPixmapSelector->setEnabled(m_pUseTransparencyBoolSelector->isChecked());
	#endif
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


OptionsWidget_themeMdiArea::OptionsWidget_themeMdiArea(QWidget * parent)
: KviOptionsWidget(parent)
{
	createLayout();

	addColorSelector(0,0,0,0,__tr2qs_ctx("Background","options"),KviOption_colorMdiBackground);
	addPixmapSelector(0,1,0,1,__tr2qs_ctx("Background Image","options"),KviOption_pixmapMdiBackground);

	layout()->setRowStretch(1,1);
}

OptionsWidget_themeMdiArea::~OptionsWidget_themeMdiArea()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


OptionsWidget_themeToolBarApplets::OptionsWidget_themeToolBarApplets(QWidget * parent)
: KviOptionsWidget(parent)
{
	createLayout();

	addFontSelector(0,0,0,0,__tr2qs_ctx("Font","options"),KviOption_fontIrcToolBarApplet);

	addColorSelector(0,1,0,1,__tr2qs_ctx("Background","options"),KviOption_colorIrcToolBarAppletBackground);
	addColorSelector(0,2,0,2,__tr2qs_ctx("Low-Contrast Foreground","options"),KviOption_colorIrcToolBarAppletForegroundLowContrast);
	addColorSelector(0,3,0,3,__tr2qs_ctx("Mid-Contrast Foreground","options"),KviOption_colorIrcToolBarAppletForegroundMidContrast);
	addColorSelector(0,4,0,4,__tr2qs_ctx("Active High-Contrast Foreground (Primary)","options"),KviOption_colorIrcToolBarAppletForegroundHighContrastActive1);
	addColorSelector(0,5,0,5,__tr2qs_ctx("Active High-Contrast Foreground (Secondary)","options"),KviOption_colorIrcToolBarAppletForegroundHighContrastActive2);
	addColorSelector(0,6,0,6,__tr2qs_ctx("Inactive High-Contrast Foreground (Primary)","options"),KviOption_colorIrcToolBarAppletForegroundHighContrastInactive1);
	addColorSelector(0,7,0,7,__tr2qs_ctx("Inactive High-Contrast Foreground (Secondary)","options"),KviOption_colorIrcToolBarAppletForegroundHighContrastInactive2);

	addPixmapSelector(0,8,0,8,__tr2qs_ctx("Background Image","options"),KviOption_pixmapIrcToolBarAppletBackground);

	layout()->setRowStretch(8,1);
}

OptionsWidget_themeToolBarApplets::~OptionsWidget_themeToolBarApplets()
{
}


#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_OptionsWidget_theme.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
