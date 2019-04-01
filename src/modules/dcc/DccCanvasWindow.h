#ifndef _CANVAS_H_
#define _CANVAS_H_
//=============================================================================
//
//   File : DccCanvasWindow.h
//   Creation date : Sun Jul 29 07 2001 20:17:12 by Szymon Stefanek
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

#include "DccBroker.h"

#ifdef COMPILE_DCC_CANVAS
#include "DccDescriptor.h"
#include "DccWindow.h"
#include "DccThread.h"

#include "KviWindow.h"
#include "KviCString.h"
#include "KviPointerList.h"

class DccMarshal;
class DccCanvasWidget;
class QSplitter;

class DccCanvasWindow : public DccWindow
{
	Q_OBJECT
public:
	DccCanvasWindow(DccDescriptor * dcc, const char * name);
	~DccCanvasWindow();

protected:
	//	KviDccCanvasThread     * m_pSlaveThread;
	DccCanvasWidget * m_pCanvas;
	//	QSplitter              * m_pTopSplitter;
	QString m_szTarget;

protected:
	const QString & target() override;
	void fillCaptionBuffers() override;
	void getBaseLogFileName(KviCString & buffer) override;
	QPixmap * myIconPtr() override;
	void resizeEvent(QResizeEvent * e) override;
	QSize sizeHint() const override;
	bool event(QEvent * e) override;
	void ownMessage(const QString & text, bool bUserFeedback = true) override;
	void ownAction(const QString & text) override;
protected slots:
	void handleMarshalError(int err);
	void connected();
};
#endif

#endif // _CANVAS_H_
