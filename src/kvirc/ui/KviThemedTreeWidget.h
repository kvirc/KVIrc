#ifndef _KVI_THEMEDTREEWIDGET_H_
#define _KVI_THEMEDTREEWIDGET_H_
//=============================================================================
//
//   File : KviThemedTreeWidget.h
//   Creation date : Sun Jan 10 2010 12:17:00 by Fabio Bas
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2010 Fabio Bas (ctrlaltca at gmail dot com)
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

#include <QTreeWidget>
#include <QPainter>

class KviWindow;

class KVIRC_API KviThemedTreeWidget : public QTreeWidget
{
	Q_OBJECT
	Q_PROPERTY(int TransparencyCapable READ dummyRead)
public:
	KviThemedTreeWidget(QWidget * par, KviWindow * pWindow, const char * name);
	~KviThemedTreeWidget();

protected:
	KviWindow * m_pKviWindow;

protected:
	virtual void paintEvent(QPaintEvent * event);

public:
	int dummyRead() const { return 0; };
	void applyOptions();
};

#endif //_KVI_THEMEDTREEWIDGET_H_
