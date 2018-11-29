#ifndef _CLASS_WINDOW_H_
#define _CLASS_WINDOW_H_
//=============================================================================
//
//   File : KvsObject_window.h
//   Creation date : Tue Nov 26 21:37:48 CEST 2002 by Szymon Stefanek
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

#include "KvsObject_widget.h"
#include "KviWindow.h"
#include "object_macros.h"

class KviKvsScriptWindowWindow : public KviWindow
{
	Q_OBJECT
public:
	KviKvsScriptWindowWindow(const QString & szName);
	~KviKvsScriptWindowWindow();

protected:
	KvsObject_widget * m_pCentralWidgetObject = nullptr;
	QWidget * m_pCentralWidget = nullptr;
	QPixmap * m_pIcon = nullptr;

public:
	QPixmap * myIconPtr() override;
	void setIcon(QPixmap * pPixmap) { m_pIcon = pPixmap; };
	void setCentralWidget(KvsObject_widget * o, QWidget * w);
	void setWindowTitleString(const QString & s)
	{
		setFixedCaption(s);
		fillCaptionBuffers();
	};
	void resizeEvent(QResizeEvent * e) override;
protected slots:
	void centralWidgetObjectDestroyed();
	void centralWidgetDestroyed();
};

class KvsObject_window : public KvsObject_widget
{
public:
	KVSO_DECLARE_OBJECT(KvsObject_window)
public:
	QWidget * widget() { return (QWidget *)object(); }

protected:
	bool init(KviKvsRunTimeContext * pContext, KviKvsVariantList * pParams) override;

	bool setWindowTitle(KviKvsObjectFunctionCall * c);
	bool setIcon(KviKvsObjectFunctionCall * c);
	bool setCentralWidget(KviKvsObjectFunctionCall * c);
};

#endif // !_CLASS_WINDOW_H_
