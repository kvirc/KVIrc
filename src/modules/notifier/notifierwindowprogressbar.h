#ifndef _NOTIFIERWINDOWPROGRESSBAR_H_
#define _NOTIFIERWINDOWPROGRESSBAR_H_
//=============================================================================
//
//   File : notifierwindowprogressbar.h
//   Creation date : Tue 17 Apr 2007 10:24:01 by Iacopo Palazzi
//
//   This file is part of the KVIrc distribution
//   Copyright (C) 2007-2008 Iacopo Palazzi < iakko(at)siena.linux.it >
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

#include <QRect>
#include <QPainter>

class KviNotifierWindowProgressBar
{
public:
	KviNotifierWindowProgressBar(QRect r);
	~KviNotifierWindowProgressBar();

// ================================
// Put members declaration below...
// ================================
private:

public:

protected:
	QRect 	m_rect;
	double	m_dProgress;

// ================================
// Put methods declaration below...
// ================================
private:

public:
	int x() { return m_rect.x(); };
	int y() { return m_rect.y(); };
	int width() { return m_rect.width(); };
	int height() { return m_rect.height(); };

	void setHeight(int p_iHeight) { m_rect.setHeight(p_iHeight); };
	void setProgress(double progress) { m_dProgress=progress; };
	void draw(QPainter *);
	
protected:
};

#endif //!_NOTIFIERWINDOWPROGRESSBAR_H_
