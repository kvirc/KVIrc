#ifndef _CODETESTER_H_
#define _CODETESTER_H_
//=============================================================================
//
//   File : eventeditor.h
//   Creation date : Mon Dec 23 2002 20:24:55 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2008 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the linkss of the GNU General Public License
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

#include "kvi_window.h"
#include "kvi_string.h"

#include <QLineEdit>

class QPushButton;
class QLabel;
class KviScriptEditor;


class KviCodeTester : public QWidget
{
	Q_OBJECT
public:
	KviCodeTester(QWidget * par);
	~KviCodeTester();
private:
	KviScriptEditor * m_pEditor;
	QLineEdit       * m_pParams;
	QPushButton     * m_pExecuteButton;
	QLabel          * m_pModeLabel;
private slots:
	void execute();
};

class KviCodeTesterWindow : public KviWindow
{
	Q_OBJECT
public:
	KviCodeTesterWindow(KviFrame * lpFrm);
	~KviCodeTesterWindow();
protected:
	KviCodeTester * m_pTester;
protected:
	virtual QPixmap * myIconPtr();
	virtual void fillCaptionBuffers();
	virtual void resizeEvent(QResizeEvent *e);
	virtual void getConfigGroupName(QString &szName);
	virtual void saveProperties(KviConfig *);
	virtual void loadProperties(KviConfig *);
};

#endif //_CODETESTER_H_
