#ifndef _KVI_ABOUT_H_
#define _KVI_ABOUT_H_
//=============================================================================
//
//   File : libkviabout.h
//   Creation date : Tue Jul 6 1999 03:54:25 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2000 Till Bush (buti@geocities.com)
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

/*
#include <qdialog.h>
#include <qtimer.h>
#include <qscrollview.h>
#include <qpixmap.h>
#include <qlabel.h>

class KviDlgAbout;

class KviAboutLabel : public QLabel
{
	Q_OBJECT
public:
	KviAboutLabel(KviDlgAbout * par);
	~KviAboutLabel();
public:
	KviDlgAbout * m_pAboutDialog;
	QPixmap     * m_pMemPixmap;
public:
	void drawContents(QPainter *p) override;
	void resizeEvent(QResizeEvent *e) override;
};



class KviDlgAbout : public QDialog
{
	Q_OBJECT
public:
	KviDlgAbout();
	~KviDlgAbout();
//signals:		// this is never used in the source	(Kristoff)
//	void closed();
private slots:
	void close();
	void closeEvent(QCloseEvent *) override;
//	void paintEvent(QPaintEvent *) override;
	void scrollText();
public:
	int              m_posy;
	char           * m_text;
	KviAboutLabel  * m_pfield;
	QPixmap        * m_ppix_text;
	QPixmap        * m_pBackground;
};

class KviDlgBuildInfo : public QDialog
{
	Q_OBJECT
public:
	KviDlgBuildInfo();
	~KviDlgBuildInfo();
private slots:
	void close();
private:
	void closeEvent(QCloseEvent *) override;

	QLabel		* m_pPicLabel;
	QLabel		* m_pTextLabel;
	QPushButton	* m_pButton;
};
*/
#endif //_KVI_ABOUT_H_
