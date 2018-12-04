#ifndef _KVI_IRCTOOLBAR_H_
#define _KVI_IRCTOOLBAR_H_
//=============================================================================
//
//   File : KviIrcToolBar.h
//   Creation date : Thu Oct 29 2000 14:09:11 CEST by Szymon Stefanek
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

#include "KviCString.h"
#include "KviToolBar.h"
#include "KviConsoleWindow.h"
#include "KviPointerList.h"

#include <QWidget>
#include <QToolButton>
#include <QLabel>

class QMenu;

//
// Basic graphical applet
//
// Thinks about the "common" background (handles transparency stuff & co.)
// Can be a child of any AppletContainer
//

class KVIRC_API KviToolBarGraphicalApplet : public QWidget
{
	Q_OBJECT
public:
	KviToolBarGraphicalApplet(QWidget * par, const char * name = nullptr);
	~KviToolBarGraphicalApplet();

private:
	bool m_bResizeMode;

public:
	QSize sizeHint() const override;

protected:
	void mouseMoveEvent(QMouseEvent * e) override;
	void mousePressEvent(QMouseEvent * e) override;
	void mouseReleaseEvent(QMouseEvent * e) override;

	void paintEvent(QPaintEvent * e) override;
	virtual void drawContents(QPainter * p);
};

class KviDynamicToolTip;

class KVIRC_API KviIrcContextDisplay : public KviToolBarGraphicalApplet
{
	Q_OBJECT
public:
	KviIrcContextDisplay(QWidget * par, const char * name = nullptr);
	~KviIrcContextDisplay();

protected:
	virtual void drawContents(QPainter * p);
protected slots:
	void tipRequest(KviDynamicToolTip * tip, const QPoint &);
};

#endif //_KVI_IRCTOOLBAR_H_
