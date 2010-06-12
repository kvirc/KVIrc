#ifndef _KVINOTIFIERWINDOWTAB_H_
#define _KVINOTIFIERWINDOWTAB_H_
//=============================================================================
//
//   File : notifierwindowtab.h
//   Creation date : Tue 07 Jul 2009 10:28 by Fabio Bas
//
//   This file is part of the KVIrc distribution
//   Copyright (C) 2009 Fabio Bas < ctrlaltca at gmail dot com >
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

#include "notifiermessage.h"

#include "kvi_settings.h"

#include <QTabWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

class QPainter;
class KviWindow;

class KviNotifierWindowTab : public QScrollArea // this class defines an object for every single tab about the tabs area
{
	Q_OBJECT
public:
	KviNotifierWindowTab(KviWindow *, QTabWidget*);
	~KviNotifierWindowTab();
private:
	QString		  m_label;
	KviWindow	* m_pWnd;
	QTabWidget	* m_pParent;
	QVBoxLayout	* m_pVBox;
	QWidget		* m_pVWidget;
public:
	void appendMessage(KviNotifierMessage * m);
	void updateGui();
	inline QString label() const { return m_label; };
	inline KviWindow * wnd() const { return m_pWnd; };
protected:
	virtual void mouseDoubleClickEvent(QMouseEvent * e);
	virtual void resizeEvent(QResizeEvent * e);
	virtual void paintEvent(QPaintEvent * e);
private slots:
	void scrollRangeChanged(int, int);
	void labelChanged();
	void closeMe();
};

#endif //!_KVINOTIFIERWINDOWTAB_H_
