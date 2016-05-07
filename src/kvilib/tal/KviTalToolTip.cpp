//=============================================================================
//
//   File : KviTalToolTip.cpp
//   Creation date : Mon May 05 2008 11:25:08 by Alessandro Carbone
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Alessandro Carbone (elfonol at gmail dot com)
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

#include "KviTalToolTip.h"

#include <QEvent>
#include <QHelpEvent>

KviTalToolTipHelper::KviTalToolTipHelper(KviTalToolTip * pToolTip, QWidget * pWidget)
    : QObject(pWidget)
{
	m_pToolTip = pToolTip;
	pWidget->installEventFilter(this);
}

KviTalToolTipHelper::~KviTalToolTipHelper()
{
	if(m_pToolTip)
	{
		m_pToolTip->helperDying();
		delete m_pToolTip;
	}
}

void KviTalToolTipHelper::toolTipDying()
{
	m_pToolTip = nullptr;
}

bool KviTalToolTipHelper::eventFilter(QObject *, QEvent * pEvent)
{
	if((pEvent->type() == QEvent::ToolTip) && m_pToolTip)
	{
		// qDebug("TOOL TIP EVENT WITH POSITION %d,%d",((QHelpEvent *)pEvent)->pos().x(),((QHelpEvent *)pEvent)->pos().y());
		m_pToolTip->maybeTip(((QHelpEvent *)pEvent)->pos());
		return true;
	}
	return false;
}

KviTalToolTip::KviTalToolTip(QWidget * pParent)
{
	m_pHelper = new KviTalToolTipHelper(this, pParent);
	m_pParent = pParent;
}

KviTalToolTip::~KviTalToolTip()
{
	if(m_pHelper)
	{
		m_pHelper->toolTipDying();
		delete m_pHelper;
	}
}

void KviTalToolTip::helperDying()
{
	m_pHelper = nullptr;
}

void KviTalToolTip::add(QWidget * widget, const QString & text)
{
	widget->setToolTip(text);
}

void KviTalToolTip::remove(QWidget * widget)
{

	widget->setToolTip("");
}

void KviTalToolTip::tip(const QRect & rect, const QString & text)
{
	// qDebug("TOOL TIP AT %d,%d",rect.topLeft().x(),rect.topLeft().y());
	QPoint pnt = m_pParent->mapToGlobal(rect.topLeft());
	pnt.setX(pnt.x() + 16);
	pnt.setY(pnt.y() + 16);
	QToolTip::showText(m_pParent->mapToGlobal(rect.topLeft()), text, m_pParent, rect);
}

void KviTalToolTip::maybeTip(const QPoint &)
{
	// does nothing here..
}
