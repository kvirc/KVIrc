#ifndef _LIBKVITIP_H_
#define _LIBKVITIP_H_
//=============================================================================
//
//   File : kvi_tip.cpp
//   Creation date : Thu May 10 2001 13:49:12 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_string.h"
#include "kvi_config.h"

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QCheckBox>

class KviTipFrame : public QFrame
{
	Q_OBJECT
public:
	KviTipFrame(QWidget * par);
	~KviTipFrame();
private:
	QLabel *m_pLabel1;
	QLabel *m_pLabel2;
public:
	void setText(const QString &text);
};

class KviTipWindow : public QWidget
{
	Q_OBJECT
public:
	KviTipWindow();
	~KviTipWindow();
protected:
	KviTipFrame * m_pTipFrame;
	QCheckBox   * m_pShowAtStartupCheck;
	KviConfig   * m_pConfig;
	QString        m_szConfigFileName; // no path!
protected:
	virtual void closeEvent(QCloseEvent *e);
	virtual void showEvent(QShowEvent *e);
public:
	bool openConfig(QString  filename,bool bEnsureExists = true);
	void closeConfig();
public slots:
	void nextTip();
	void prevTip();
};

#endif //_LIBKVITIP_H_
