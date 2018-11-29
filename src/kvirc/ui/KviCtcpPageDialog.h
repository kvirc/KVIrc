#ifndef _KVI_CTCPPAGEDIALOG_H_
#define _KVI_CTCPPAGEDIALOG_H_
//=============================================================================
//
//   File : KviCtcpPageDialog.h
//   Creation date : Tue May 21 2002 22:09:45 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include <QWidget>

class QTabWidget;
class QPushButton;
class QStackedWidget;

class KVIRC_API KviCtcpPageDialog : public QWidget
{
	Q_OBJECT
public:
	KviCtcpPageDialog();
	~KviCtcpPageDialog();

protected:
	QTabWidget * m_pTabBar;
	QPushButton * m_pCloseButton;

public:
	void popup();
	void addPage(const QString & szNick, const QString & szUser, const QString & szHost, const QString & szMsg);

protected:
	void center();

protected:
	void showEvent(QShowEvent * e) override;
	void closeEvent(QCloseEvent * e) override;
protected slots:
	void die();
};

#endif //_KVI_CTCPPAGEDIALOG_H_
