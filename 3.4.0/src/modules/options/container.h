#ifndef _CONTAINER_H_
#define _CONTAINER_H_
//
//   File : container.h
//   Creation date : Wed Nov 21 17:09:50 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
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


#include "kvi_optionswidget.h"
#include <qdialog.h>

class QPushButton;

class KviOptionsWidgetContainer : public QDialog
{
	Q_OBJECT
public:
	KviOptionsWidgetContainer(QWidget * par,bool bModal = false);
	~KviOptionsWidgetContainer();
protected:
	KviOptionsWidget * m_pOptionsWidget;
	QPushButton      * m_pCancel;
public:
	void setup(KviOptionsWidget * w);
protected:
	virtual void closeEvent(QCloseEvent *e);
	virtual void showEvent(QShowEvent *e);
	virtual void reject();
protected slots:
	void okClicked();
	void cancelClicked();
};



#endif //_CONTAINER_H_
