//=============================================================================
//
//   File : kvi_tal_grid.cpp
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
#include "kvi_tal_grid.h"
 
#include <QWidget>
#include <QLayout>
#include <QChildEvent>

 
KviTalGrid::KviTalGrid(int n,Qt::Orientation orient,QWidget * pParent)
: QWidget(pParent)
{
	if (orient==Qt::Horizontal)
		m_pLayout=new QHBoxLayout(this);
	else m_pLayout= new QVBoxLayout(this);
	m_pLayout->setSpacing(0);
	m_pLayout->setMargin(0);
	setLayout(m_pLayout);
}
void KviTalGrid::childEvent(QChildEvent * e)
{
	if(!e->child()->isWidgetType()) return;
	if(e->child()->parent() != this) return;

	switch(e->type())
	{
		case QEvent::ChildAdded:
			m_pLayout->addWidget((QWidget *)(e->child()));
			break;
		case QEvent::ChildRemoved:
			m_pLayout->removeWidget((QWidget *)(e->child()));
			break;
		default:
			break;
	}
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "kvi_tal_grid.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

