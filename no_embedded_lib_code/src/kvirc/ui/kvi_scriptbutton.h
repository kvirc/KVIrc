#ifndef _KVI_SCRIPTBUTTON_H_
#define _KVI_SCRIPTBUTTON_H_
//=============================================================================
//
//   File : kvi_scriptbutton.h
//   Creation date : Wed Nov 14 15:43:39 2001 GMT by Szymon Stefanek
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

#include "kvi_settings.h"
#include "kvi_string.h"

#include <QToolButton>
#include <QToolBar>

class KviKvsScript;
class KviWindow;

class KVIRC_API KviScriptUserButton : public QToolButton
{
	Q_OBJECT
public:
	KviScriptUserButton(QWidget * par,const char * name);
	~KviScriptUserButton();
protected:
	KviKvsScript * m_pScript;
public:
	void setButtonCode(KviKvsScript * pScript);
	virtual void setButtonPixmap(const QPixmap & pix){ setIcon(pix); };
	void setButtonText(const QString &text){ setText(text); };
	virtual KviWindow * window();
protected slots:
	void btnClicked();
};

class KVIRC_API KviWindowScriptButton : public KviScriptUserButton
{
	Q_OBJECT
public:
	KviWindowScriptButton(QWidget * p,KviWindow * wnd,const char * name);
	~KviWindowScriptButton();
protected:
	KviWindow * m_pWnd;
public:
	virtual void setButtonPixmap(const QPixmap & pix){ setIcon(QIcon(pix)); setIconSize(QSize(22,22)); };
	virtual KviWindow * window(){ return m_pWnd; };
};

#endif //_KVI_SCRIPTBUTTON_H_
