//=============================================================================
//
//   File : KviTalGroupBox.cpp
//   Creation date : Mon Jan 22 2007 11:25:08 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2007-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviTalGroupBox.h"

#include <QChildEvent>

KviTalGroupBox::KviTalGroupBox(QWidget * pParent, char * pcName)
    : QGroupBox(pParent)
{
	setObjectName(pcName);
	m_pLayout = new QHBoxLayout(this);
	m_pLayout->setMargin(4);
	m_pLayout->setContentsMargins(8, 8, 8, 8);
	m_pLayout->setSpacing(6);
	QGroupBox::setLayout(m_pLayout);
}

KviTalGroupBox::KviTalGroupBox(QWidget * pParent)
    : QGroupBox(pParent)
{
	m_pLayout = nullptr;
}

KviTalGroupBox::KviTalGroupBox(const QString & szTitle, QWidget * pParent)
    : QGroupBox(szTitle, pParent)
{
	m_pLayout = nullptr;
}

KviTalGroupBox::KviTalGroupBox(Qt::Orientation orientation, QWidget * pParent)
    : QGroupBox(pParent)
{
	if(orientation == Qt::Vertical)
		m_pLayout = new QHBoxLayout(this);
	else
		m_pLayout = new QVBoxLayout(this);
	m_pLayout->setMargin(4);
	m_pLayout->setContentsMargins(8, 8, 8, 8);
	m_pLayout->setSpacing(6);
	QGroupBox::setLayout(m_pLayout);
}

KviTalGroupBox::KviTalGroupBox(Qt::Orientation orientation, const QString & szTitle, QWidget * pParent)
    : QGroupBox(szTitle, pParent)
{
	mOrientation = orientation;
	if(orientation == Qt::Vertical)
		m_pLayout = new QHBoxLayout(this);
	else
		m_pLayout = new QVBoxLayout(this);
	m_pLayout->setMargin(4);
	m_pLayout->setContentsMargins(8, 8, 8, 8);
	m_pLayout->setSpacing(6);
	QGroupBox::setLayout(m_pLayout);
}

KviTalGroupBox::~KviTalGroupBox()
    = default;

void KviTalGroupBox::childEvent(QChildEvent * e)
{
	if(!e->child()->isWidgetType())
		return;
	if(e->child()->parent() != this)
		return;
	if(!m_pLayout)
		return;
	if(m_pLayout != layout())
		return;

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

void KviTalGroupBox::addSpace(int iSpace)
{
	if(m_pLayout && m_pLayout == layout())
	{
		if(mOrientation == Qt::Vertical)
			((QHBoxLayout *)m_pLayout)->addSpacing(iSpace);
		else
			((QVBoxLayout *)m_pLayout)->addSpacing(iSpace);
	}
}

void KviTalGroupBox::setOrientation(Qt::Orientation orientation)
{
	if(m_pLayout)
	{
		delete m_pLayout;
		m_pLayout = nullptr;
	}

	mOrientation = orientation;

	if(orientation == Qt::Vertical)
		m_pLayout = new QHBoxLayout(this);
	else
		m_pLayout = new QVBoxLayout(this);
	QGroupBox::setLayout(m_pLayout);
}

void KviTalGroupBox::setLayout(QLayout * layout)
{
	if(m_pLayout)
	{
		delete m_pLayout;
		m_pLayout = nullptr;
	}

	QGroupBox::setLayout(layout);
}
