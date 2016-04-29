#ifndef _SPASTE_CONTROLLER_H_
#define _SPASTE_CONTROLLER_H_
//=============================================================================
//
//   File : SlowPasteController.h
//   Creation date : Thu Apr 30 2002 17:13:12 GMT by Juanjo Álvarez
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002 Juanjo Álvarez (juanjux@yahoo.es)
//   Copyright (C) 2002-2010 Szymon Stefanek (kvirc@tin.it)
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

#include <QObject>
#include <QStringList>
#include <QFile>

class SlowPasteController : public QObject
{
	Q_OBJECT
public:
	SlowPasteController(KviWindow * w, int id);
	~SlowPasteController();

	//bool pasteFileInit(KviCString * fileName);
	bool pasteFileInit(QString & fileName);
	bool pasteClipboardInit(void);
	int getId(void) { return m_pId; };
	KviWindow * window(void) { return m_pWindow; };
	//void stop(void);
protected slots:
	void pasteFile(void);
	void pasteClipboard(void);

private:
	QStringList * m_pClipBuff;
	QFile * m_pFile;
	int m_pId;
	KviWindow * m_pWindow;
	QTimer * m_pTimer;
};

#endif //SPASTE_CONTROLLER_H
