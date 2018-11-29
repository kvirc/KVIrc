#ifndef _CODETESTER_H_
#define _CODETESTER_H_
//=============================================================================
//
//   File : CodeTesterWindow.h
//   Creation date : Mon Dec 23 2002 20:24:55 CEST by Szymon Stefanek
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

#include "KviWindow.h"
#include "KviCString.h"

#include <QLineEdit>

class QPushButton;
class QLabel;
class KviScriptEditor;

class CodeTesterWindow : public KviWindow
{
	Q_OBJECT
public:
	CodeTesterWindow();
	~CodeTesterWindow();

private:
	KviTalSplitter * m_pSplitter;
	KviScriptEditor * m_pEditor;
	QPushButton * m_pExecuteButton;
	QLabel * m_pModeLabel;
	QLineEdit * m_pParams;

private slots:
	void execute();

protected:
	QPixmap * myIconPtr() override;
	void fillCaptionBuffers() override;
	void resizeEvent(QResizeEvent * e) override;
	void getConfigGroupName(QString & szName) override;
	void saveProperties(KviConfigurationFile *) override {}
	void loadProperties(KviConfigurationFile *) override {}
};

#endif //_CODETESTER_H_
