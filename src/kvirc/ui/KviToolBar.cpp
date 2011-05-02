//=============================================================================
//
//   File : KviToolBar.cpp
//   Creation date : Tue Sep 17 02:00:17 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviToolBar.h"
#include "KviMainWindow.h"
#include "KviLocale.h"
#include "KviApplication.h"
#include "KviTalPopupMenu.h"
#include "KviOptions.h"

#include <QCursor>
#include <QEvent>
#include <QMouseEvent>

static KviTalPopupMenu * g_pToolBarContextPopup     = 0;
static KviTalPopupMenu * g_pToolBarWindowsPopup     = 0;
static KviTalPopupMenu * g_pToolBarIconSizesPopup   = 0;
static KviTalPopupMenu * g_pToolBarButtonStylePopup = 0;
static uint              uToolBarInstances          = 0;

KviToolBar::KviToolBar(const QString & szLabel, Qt::ToolBarArea type, const char * pcName)
: QToolBar(szLabel,g_pMainWindow)
{
	uToolBarInstances++;

	unsigned int uIconSize = KVI_OPTION_UINT(KviOption_uintToolBarIconSize);
	setIconSize(QSize(uIconSize,uIconSize));
	setToolButtonStyle((Qt::ToolButtonStyle) KVI_OPTION_UINT(KviOption_uintToolBarButtonStyle));

	setObjectName(pcName);
	g_pMainWindow->addToolBar(type,this);
}

KviToolBar::~KviToolBar()
{
	uToolBarInstances--;

	// the last instance cleans up the cached popups
	if(!uToolBarInstances)
	{
		if(g_pToolBarButtonStylePopup)
			delete g_pToolBarButtonStylePopup;
		if(g_pToolBarIconSizesPopup)
			delete g_pToolBarIconSizesPopup;
		if(g_pToolBarWindowsPopup)
			delete g_pToolBarWindowsPopup;
		if(g_pToolBarContextPopup)
			delete g_pToolBarContextPopup;

		g_pToolBarButtonStylePopup = 0;
		g_pToolBarIconSizesPopup   = 0;
		g_pToolBarWindowsPopup     = 0;
		g_pToolBarContextPopup     = 0;
	}
}

#define VALID_ICONSIZES_NUM 4
static KviToolBar::IconSizes valid_iconsizes[VALID_ICONSIZES_NUM] =
{
	{ 16, "Small (16x16)"  },
	{ 22, "Medium (22x22)" },
	{ 32, "Large (32x32)"  },
	{ 48, "Huge (48x48)"   }
};

#define VALID_BUTTONSTYLES_NUM 5
static KviToolBar::ButtonStyles valid_buttonstyles[VALID_BUTTONSTYLES_NUM] =
{
	{ Qt::ToolButtonIconOnly       , "Icon Only"  },
	{ Qt::ToolButtonTextOnly       , "Text Only" },
	{ Qt::ToolButtonTextBesideIcon , "Text Beside Icon"  },
	{ Qt::ToolButtonTextUnderIcon  , "Text Under Icon"   },
	{ Qt::ToolButtonFollowStyle    , "Use System Style" }
};

void KviToolBar::mousePressEvent(QMouseEvent * e)
{
	if(!(e->button() & Qt::RightButton))
	{
		QToolBar::mousePressEvent(e);
		return;
	}

	if(!g_pToolBarContextPopup)
	{
		g_pToolBarContextPopup = new KviTalPopupMenu();
		g_pToolBarIconSizesPopup = new KviTalPopupMenu();
		g_pToolBarWindowsPopup = new KviTalPopupMenu();
		g_pToolBarButtonStylePopup = new KviTalPopupMenu();

		g_pToolBarContextPopup->insertItem(__tr2qs("Toolbars"),g_pToolBarWindowsPopup);
		g_pToolBarContextPopup->insertItem(__tr2qs("Icon Size"),g_pToolBarIconSizesPopup);
		g_pToolBarContextPopup->insertItem(__tr2qs("Button Style"),g_pToolBarButtonStylePopup);

		// fill toolbars menu
		g_pMainWindow->fillToolBarsPopup(g_pToolBarWindowsPopup);

		// fill icon size menu
		QActionGroup * pIconSizeGroup = new QActionGroup(g_pToolBarIconSizesPopup);

		QAction * pTmp = 0;
		IconSizes iconSize;
		for(uint i = 0; i < VALID_ICONSIZES_NUM; i++)
		{
			iconSize = valid_iconsizes[i];

			pTmp = pIconSizeGroup->addAction(g_pToolBarIconSizesPopup->addAction(__tr2qs(iconSize.pcName)));
			pTmp->setData((uint)iconSize.uSize);
			pTmp->setCheckable(true);
			if(iconSize.uSize == KVI_OPTION_UINT(KviOption_uintToolBarIconSize))
				pTmp->setChecked(true);
		}

		connect(pIconSizeGroup,SIGNAL(triggered(QAction*)),g_pMainWindow,SLOT(iconSizePopupSelected(QAction*)));

		// fill button style menu
		QActionGroup * pButtonStyleGroup = new QActionGroup(g_pToolBarButtonStylePopup);

		pTmp = 0;
		ButtonStyles buttonStyle;
		for(uint i = 0; i < VALID_BUTTONSTYLES_NUM; i++)
		{
			buttonStyle = valid_buttonstyles[i];

			pTmp = pButtonStyleGroup->addAction(g_pToolBarButtonStylePopup->addAction(__tr2qs(buttonStyle.pcName)));
			pTmp->setData((uint)buttonStyle.uStyle);
			pTmp->setCheckable(true);
			if(buttonStyle.uStyle == KVI_OPTION_UINT(KviOption_uintToolBarButtonStyle))
				pTmp->setChecked(true);
		}

		connect(pButtonStyleGroup,SIGNAL(triggered(QAction*)),g_pMainWindow,SLOT(buttonStylePopupSelected(QAction*)));
		

	}

	g_pToolBarContextPopup->popup(QCursor::pos());
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "KviToolBar.moc"
#endif //COMPILE_USE_STANDALONE_MOC_SOURCES
