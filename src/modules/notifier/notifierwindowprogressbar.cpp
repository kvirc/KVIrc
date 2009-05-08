//=============================================================================
//
//   File : notifierwindowprogressbar.cpp
//   Created on Tue 17 Apr 2007 10:24:01 by Iacopo Palazzi
//
//   This file is part of the KVIrc distribution
//   Copyright (C) 2007 Iacopo Palazzi < iakko(at)siena.linux.it >
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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "notifierwindowprogressbar.h"

KviNotifierWindowProgressBar::KviNotifierWindowProgressBar(QRect r)
: m_rect(r)
{

}

KviNotifierWindowProgressBar::~KviNotifierWindowProgressBar()
{
	m_dProgress = 0;
}

void KviNotifierWindowProgressBar::draw(QPainter *p)
{
	p->fillRect(m_rect, QColor(236, 233, 216));
	p->fillRect(
		QRect(
			m_rect.x()+1,
			m_rect.y()+1 + m_rect.height() * (1 - m_dProgress),
			m_rect.width() - 2,
			(m_rect.height() - 2) * m_dProgress
			),
		QColor(175, 28, 95)
		);
	p->drawRect(m_rect);
}
