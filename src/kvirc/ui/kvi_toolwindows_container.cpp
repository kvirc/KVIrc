//=============================================================================
//
//   File : kvi_toolwindows_container.cpp
//   Creation date : Tue Aug 1 2000 21:05:22 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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



#include "kvi_toolwindows_container.h"
#include "kvi_iconmanager.h"
#include "kvi_tal_vbox.h"

#include <QPainter>
#include <QApplication>
#include <QLayout>
#include <QObjectCleanupHandler>
#include <QIcon>


KviWindowToolWidget::KviWindowToolWidget(QWidget * parent, KviWindowToolPageButton* button)
:QWidget(parent)
{
	m_pButton=button;
	if(m_pButton) m_pButton->setChecked(!isHidden());
}

KviWindowToolWidget::~KviWindowToolWidget()
{
}

KviWindowToolPageButton::KviWindowToolPageButton ( int pixoff,int pixon, const QString & text, QWidget * parent,bool bOn, const char *)
:QToolButton(parent)
{
	setAutoRaise(true);
	setToolTip(text);
	setObjectName("kvi_window_tool_button");
	QIcon icon;
	icon.addPixmap(*(g_pIconManager->getSmallIcon(pixoff)),QIcon::Normal,QIcon::Off);
	icon.addPixmap(*(g_pIconManager->getSmallIcon(pixon)),QIcon::Active,QIcon::On);
	icon.addPixmap(*(g_pIconManager->getSmallIcon(pixon)),QIcon::Active);
	setIcon(icon);
	setCheckable(true);
	setChecked(bOn);
}

KviWindowToolPageButton::~KviWindowToolPageButton()
{
}

