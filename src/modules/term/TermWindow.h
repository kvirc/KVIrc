#ifndef _TERMWINDOW_H_
#define _TERMWINDOW_H_
//=============================================================================
//
//   File : TermWindow.h
//   Creation date : Thu Aug 31 2000 15:00:00 by Szymon Stefanek
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

#include "kvi_settings.h"

#ifdef COMPILE_KDE4_SUPPORT
#include "KviWindow.h"
#include "KviCString.h"

class TermWidget;

class TermWindow : public KviWindow
{
	Q_OBJECT
public:
	TermWindow(const char * name);
	~TermWindow();

protected:
	TermWidget * m_pTermWidget;

protected:
	QPixmap * myIconPtr() override;
	void fillCaptionBuffers() override;
	void resizeEvent(QResizeEvent * e) override;

public:
	QSize sizeHint() const override;
};
#else
#include <QObject>

// Dummy, to make moc happy
class TermWindow : public QObject
{
	Q_OBJECT
public:
	TermWindow()
	    : QObject()
	{
	}
};
#endif

#endif //_KVI_HELPWINDOW_H_
