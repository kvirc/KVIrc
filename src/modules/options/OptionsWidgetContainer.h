#ifndef _CONTAINER_H_
#define _CONTAINER_H_
//=============================================================================
//
//   File : OptionsWidgetContainer.h
//   Creation date : Wed Nov 21 17:09:50 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2001-208 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviOptionsWidget.h"

#include <QWidget>

class QPushButton;
class QGridLayout;

class OptionsWidgetContainer : public QWidget
{
	Q_OBJECT
public:
	OptionsWidgetContainer(QWidget * par,bool bModal = false);
	~OptionsWidgetContainer();
protected:
	KviOptionsWidget * m_pOptionsWidget;
	QPushButton      * m_pCancel;
	QGridLayout      * m_pLayout;
public:
	void setup(KviOptionsWidget * w);
	void setLeftCornerWidget(QWidget * pWidget);
protected:
	virtual void closeEvent(QCloseEvent *e);
	virtual void showEvent(QShowEvent *e);
	virtual void childEvent(QChildEvent * e);
	virtual void reject();
protected slots:
	void okClicked();
	void cancelClicked();
	void optionsWidgetDestroyed();
};

#endif //_CONTAINER_H_
