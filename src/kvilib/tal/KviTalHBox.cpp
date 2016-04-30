//=============================================================================
//
//   File : KviTalHBox.cpp
//   Creation date : Mon Jan 22 2007 11:25:08 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2007 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
//   Copyright (C) 2014 OmegaPhil (OmegaPhil@startmail.com)
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

#include "KviTalHBox.h"

#include <QChildEvent>

// Tables used in $setAlignment & $alignment
const char * const align_tbl[] = {
	"Left",
	"Right",
	"HCenter",
	"VCenter",
	"Center",
	"Top",
	"Bottom",
	"Justify"
};

const int align_cod[] = {
	Qt::AlignLeft,
	Qt::AlignRight,
	Qt::AlignHCenter,
	Qt::AlignVCenter,
	Qt::AlignCenter,
	Qt::AlignTop,
	Qt::AlignBottom,
	Qt::AlignJustify,
};

#define align_num (sizeof(align_tbl) / sizeof(align_tbl[0]))

KviTalHBox::KviTalHBox(QWidget * pParent, char * pcName)
    : QWidget(pParent)
{
	setObjectName(pcName);

	m_pLayout = new QHBoxLayout(this);
	m_pLayout->setMargin(0);
	m_pLayout->setSpacing(6);

	setLayout(m_pLayout);
}

KviTalHBox::~KviTalHBox()
    = default;

void KviTalHBox::childEvent(QChildEvent * e)
{
	if(!e->child()->isWidgetType())
		return;
	if(e->child()->parent() != this)
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

void KviTalHBox::setStretchFactor(QWidget * pChild, int iStretch)
{
	m_pLayout->setStretchFactor(pChild, iStretch);
}

void KviTalHBox::setSpacing(int iSpacing)
{
	m_pLayout->setSpacing(iSpacing);
}

void KviTalHBox::setMargin(int iMargin)
{
	m_pLayout->setContentsMargins(iMargin, iMargin, iMargin, iMargin);
}

void KviTalHBox::setAlignment(Qt::Alignment alignment)
{
	m_pLayout->setAlignment(this, alignment);
}

void KviTalHBox::setAlignment(QWidget * pChild, Qt::Alignment alignment)
{
	m_pLayout->setAlignment(pChild, alignment);
}

void KviTalHBox::addStretch(int iStretch)
{
	m_pLayout->addStretch(iStretch);
}

void KviTalHBox::addSpacing(int iSpacing)
{
	m_pLayout->addSpacing(iSpacing);
}
