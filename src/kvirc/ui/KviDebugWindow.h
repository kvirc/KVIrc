#ifndef _KVI_DEBUGWINDOW_H_
#define _KVI_DEBUGWINDOW_H_
//=============================================================================
//
//   File : KviDebugWindow.h
//   Creation date : Sun Jul 18 2005 14:14:00 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#include "KviWindow.h"
#include "KviCString.h"

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
	static KviDebugWindow * instance() { return m_pInstance; };
	static KviDebugWindow * getInstance();

protected:
	QPixmap * myIconPtr() override;
	void fillCaptionBuffers() override;
	void resizeEvent(QResizeEvent * e) override;
	void loadProperties(KviConfigurationFile * cfg) override;
	void saveProperties(KviConfigurationFile * cfg) override;
	void getBaseLogFileName(QString & buffer) override;
	QSize sizeHint() const override;
};

#endif //_KVI_DEBUGWINDOW_H_
