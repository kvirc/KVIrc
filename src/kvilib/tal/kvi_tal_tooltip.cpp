//=============================================================================
//
//   File : kvi_tal_tooltip.cpp
//   Creation date : Mon Jan 22 2007 11:25:08 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2007 Szymon Stefanek (pragma at kvirc dot net)
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
//=============================================================================

#define __KVILIB__
#include "kvi_tal_tooltip.h"

#include <QEvent>
#include <QHelpEvent>

KviTalToolTipHelper::KviTalToolTipHelper(KviTalToolTip * pToolTip,QWidget * pWidget)
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
	m_pToolTip = 0;
}

bool KviTalToolTipHelper::eventFilter(QObject * pObject,QEvent * pEvent)
{
	if((pEvent->type() == QEvent::ToolTip) && m_pToolTip)
	{
//		debug("TOOL TIP EVENT WITH POSITION %d,%d",((QHelpEvent *)pEvent)->pos().x(),((QHelpEvent *)pEvent)->pos().y());
		m_pToolTip->maybeTip(((QHelpEvent *)pEvent)->pos());
		return true;
	}
	return false;
}


KviTalToolTip::KviTalToolTip(QWidget * pParent)
{
	m_pHelper = new KviTalToolTipHelper(this,pParent);
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
	m_pHelper = 0;
}

void KviTalToolTip::add(QWidget * widget,const QString & text)
{
//	QToolTip::add(widget,text);
	widget->setToolTip(text);
}

void KviTalToolTip::remove(QWidget * widget)
{

	//QToolTip::remove(widget); 
}

void KviTalToolTip::tip(const QRect & rect,const QString & text)
{
//	debug("TOOL TIP AT %d,%d",rect.topLeft().x(),rect.topLeft().y());
	QToolTip::showText(m_pParent->mapToGlobal(rect.topLeft()),text);
}

void KviTalToolTip::maybeTip(const QPoint & p)
{
	// does nothing here.. and in Qt 4.x will even fail to work
}

#ifndef COMPILE_ON_WINDOWS
	#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
		#include "kvi_tal_tooltip.moc"
	#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
#endif
