//=============================================================================
//
//   File : KviDynamicToolTip.cpp
//   Creation date : Wed Nov 01 2000 15:25:11 CEST by Szymon Stefanek
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

#include "KviDynamicToolTip.h"

KviDynamicToolTipHelper::KviDynamicToolTipHelper(QWidget * parent, KviDynamicToolTip * parToolTip)
    : KviTalToolTip(parent)
{
	m_pParentToolTip = parToolTip;
}

KviDynamicToolTipHelper::~KviDynamicToolTipHelper()
    = default;

void KviDynamicToolTipHelper::maybeTip(const QPoint & pnt)
{
	m_pParentToolTip->maybeTip(pnt);
}

KviDynamicToolTip::KviDynamicToolTip(QWidget * parent, const char * name)
    : QObject(parent)
{
	setObjectName(name);
	m_pHelper = new KviDynamicToolTipHelper(parent, this);
}

KviDynamicToolTip::~KviDynamicToolTip()
{
	delete m_pHelper;
}

void KviDynamicToolTip::maybeTip(const QPoint & pnt)
{
	emit tipRequest(this, pnt);
}

void KviDynamicToolTip::tip(const QRect & rct, const QString & text)
{
	m_pHelper->tip(rct, text);
}
