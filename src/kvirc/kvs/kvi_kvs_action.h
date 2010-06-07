#ifndef _KVI_KVSACTION_H_
#define _KVI_KVSACTION_H_
//=============================================================================
//
//   File : kvi_kvs_action.h
//   Creation date : Sat 04 Dec 2004 04:22:12 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2008 Szymon Stefanek <pragma at kvirc dot net>
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
#include "kvi_action.h"

class KviKvsScript;

class KVIRC_API KviKvsAction : public KviAction
{
	Q_OBJECT
protected:
	QString m_szScript;
public:
	KviKvsAction(
			QObject * pParent,
			const QString &szName,
			const QString &szScriptCode,
			const QString &szVisibleName,
			const QString &szDescription,
			KviActionCategory * pCategory = NULL,
			const QString &szBigIconId = QString(),
			const QString &szSmallIconId = QString(),
			unsigned int uFlags = 0,
			const QString &szKeySequence = QString()
		);
	~KviKvsAction();
public:
	const QString & scriptCode();
	virtual void activate();
};

#endif //!_KVI_KVSACTION_H_
