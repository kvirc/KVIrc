#ifndef _KVI_DEBUGWINDOW_H_
#define _KVI_DEBUGWINDOW_H_
//=============================================================================
//
//   File : kvi_debugwindow.h
//   Creation date : Sun Jul 18 2005 14:14:00 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2005-2008 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_window.h"
#include "kvi_string.h"

class QSplitter;

class KVIRC_API KviDebugWindow : public KviWindow
{
	Q_OBJECT
public:
	KviDebugWindow();
	~KviDebugWindow();
protected:
	static KviDebugWindow * m_pInstance;
public:
	static KviDebugWindow * instance(){ return m_pInstance; };
	static KviDebugWindow * getInstance();
protected:
	virtual QPixmap * myIconPtr();
	virtual void fillCaptionBuffers();
	virtual void resizeEvent(QResizeEvent *e);
	virtual void loadProperties(KviConfig * cfg);
	virtual void saveProperties(KviConfig * cfg);
	virtual void getBaseLogFileName(QString &buffer);
	virtual QSize sizeHint() const;
};

#endif //_KVI_DEBUGWINDOW_H_
