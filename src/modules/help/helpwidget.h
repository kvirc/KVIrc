#ifndef _HELPWIDGET_H_
#define _HELPWIDGET_H_
//=============================================================================
//
//   File : helpwidget.h
//   Creation date : Thu Aug 10 2000 17:26:20 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "index.h"

#include "kvi_tal_hbox.h"

#include <QTextBrowser>
#include <QProgressBar>

class QToolButton;
class QLineEdit;
class KviFrame;
//class KviProcess;

class KviHelpWidget : public QWidget
{
	Q_OBJECT
public:
	KviHelpWidget(QWidget *par,KviFrame * lpFrm,bool bIsStandalone = false);
	~KviHelpWidget();
private:
	QToolButton  * m_pBtnIndex;
	QToolButton  * m_pBtnBackward;
	QToolButton  * m_pBtnForward;
	KviTalHBox   * m_pToolBar;
	QTextBrowser * m_pTextBrowser;
	bool           m_bIsStandalone;
protected:
	virtual void resizeEvent(QResizeEvent *e);
protected slots:
	void doClose();
	void showIndex();
	void suicide();
public:
	virtual QSize sizeHint() const;
	QTextBrowser * textBrowser() { return m_pTextBrowser; }
};


#endif //_HELPWIDGET_H_
