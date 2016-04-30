//=============================================================================
//
//   File : KviWindowToolWidget.cpp
//   Creation date : Tue Aug 1 2000 21:05:22 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviWindowToolWidget.h"
#include "KviTalVBox.h"

#include <QPainter>
#include <QApplication>
#include <QLayout>
#include <QObjectCleanupHandler>
#include <QIcon>

KviWindowToolWidget::KviWindowToolWidget(QWidget * pParent, KviWindowToolPageButton * pButton)
    : QWidget(pParent)
{
	m_pButton = pButton;
	if(m_pButton)
		m_pButton->setChecked(!isHidden());
}

KviWindowToolWidget::~KviWindowToolWidget()
    = default;

KviWindowToolPageButton::KviWindowToolPageButton(KviIconManager::SmallIcon eIconOff, KviIconManager::SmallIcon eIconOn, const QString & szText, QWidget * pParent, bool bOn)
    : QToolButton(pParent)
{
	setAutoRaise(true);
	setToolTip(szText);
	setObjectName("kvi_window_tool_button");
	QIcon icon;
	icon.addPixmap(*(g_pIconManager->getSmallIcon(eIconOff)), QIcon::Normal, QIcon::Off);
	icon.addPixmap(*(g_pIconManager->getSmallIcon(eIconOn)), QIcon::Active, QIcon::On);
	icon.addPixmap(*(g_pIconManager->getSmallIcon(eIconOn)), QIcon::Active);
	setIcon(icon);
	setCheckable(true);
	setChecked(bOn);
}

KviWindowToolPageButton::~KviWindowToolPageButton()
    = default;
