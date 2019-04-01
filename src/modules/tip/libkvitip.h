#ifndef _LIBKVITIP_H_
#define _LIBKVITIP_H_
//=============================================================================
//
//   File : libkvitip.h
//   Creation date : Thu May 10 2001 13:49:12 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviCString.h"
#include "KviConfigurationFile.h"

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QCheckBox>

class TipFrame : public QFrame
{
	Q_OBJECT
public:
	TipFrame(QWidget * par);
	~TipFrame();

private:
	QLabel * m_pLabel1;
	QLabel * m_pLabel2;

public:
	void setText(const QString & text);
};

class TipWindow : public QWidget
{
	Q_OBJECT
public:
	TipWindow();
	~TipWindow();

protected:
	TipFrame * m_pTipFrame;
	QCheckBox * m_pShowAtStartupCheck;
	KviConfigurationFile * m_pConfig;
	QString m_szConfigFileName; // no path!
protected:
	void showEvent(QShowEvent * e) override;

public:
	bool openConfig(QString filename, bool bEnsureExists = true);
	void closeConfig();
public slots:
	void nextTip();
	void prevTip();
};

#endif //_LIBKVITIP_H_
