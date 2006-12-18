#ifndef _KVI_CTCPPAGEDIALOG_H_
#define _KVI_CTCPPAGEDIALOG_H_
//
//   File : kvi_ctcppagedialog.h
//   Creation date : Tue May 21 2002 22:09:45 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"

#include "kvi_string.h"

#include <qwidgetstack.h>
#include <qtabbar.h>
#include <qwidget.h>
#include <qpushbutton.h>

class KVIRC_API KviCtcpPageDialog : public QWidget
{
	Q_OBJECT
public:
	KviCtcpPageDialog();
	~KviCtcpPageDialog();
protected:
	QWidgetStack * m_pWidgetStack;
	QTabBar      * m_pTabBar;
	QPushButton  * m_pCloseButton;
public:
	void popup();
	void addPage(const QString &szNick,const QString &szUser,const QString &szHost,const QString &szMsg);
protected slots:
	void tabSelected(int id);
	void die();
protected:
	void center();
	virtual void showEvent(QShowEvent *e);
	virtual void closeEvent(QCloseEvent *);
};

#endif //_KVI_CTCPPAGEDIALOG_H_
