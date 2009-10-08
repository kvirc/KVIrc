#ifndef _KVI_GARBAGE_H_
#define _KVI_GARBAGE_H_
//=============================================================================
//
//   File : kvi_garbage.h
//   Creation date : Mon Dec  3 16:49:13 2001 GMT by Szymon Stefanek
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
#include "kvi_pointerlist.h"

#include <QObject>
#include <QTimer>

/**
 * FIXME: this class is an obsolete.
 *
 * We can safelly use QObject::deleteLater() with the same functionality,
 * and much easier.
 */

/*
class KVILIB_API KviGarbageCollector : public QObject
{
	Q_OBJECT
public:
	KviGarbageCollector();
	~KviGarbageCollector();
protected:
	KviPointerList<QObject> * m_pGarbageList;
	QTimer              * m_pCleanupTimer;
	bool                  m_bForceCleanupNow;
public:
	void collect(QObject * g);
protected:
	void triggerCleanup(int iTimeout);
protected slots:
	void cleanup();
	void garbageSuicide();
};
*/
#endif //_KVI_GARBAGE_H_
