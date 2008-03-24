#ifndef _KVI_KVS_USERACTION_H_
#define _KVI_KVS_USERACTION_H_
//=============================================================================
//
//   File : kvi_kvs_useraction.h
//   Created on Tue 07 Dec 2004 02:00:24 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004 Szymon Stefanek <pragma at kvirc dot net>
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#include "kvi_settings.h"
#include "kvi_kvs_action.h"

class KviConfig;
class KviKvsScript;

class KVIRC_API KviKvsUserAction : public KviKvsAction
{
	friend class KviActionManager;
	Q_OBJECT
public:
	KviKvsUserAction(QObject * pParent,
		const QString &szName,
		const QString &szScriptCode,
		const QString &szVisibleNameCode,
		const QString &szDescriptionCode,
		const QString &szCategory,
		const QString &szBigIcon,
		const QString &szSmallIcon,
		unsigned int uFlags,
		const QString &szKeySequence = QString::null);
	KviKvsUserAction(QObject * pParent);
	~KviKvsUserAction();

	// Static allocator function.
	// This MUST be used by the modules to allocate action structures
	// instead of the new operator.
	// See kvi_heapobject.cpp for an explaination.
	static KviKvsUserAction * createInstance(QObject * pParent,
		const QString &szName,
		const QString &szScriptCode,
		const QString &szVisibleNameCode,
		const QString &szDescriptionCode,
		const QString &szCategory,
		const QString &szBigIcon,
		const QString &szSmallIcon,
		unsigned int uFlags,
		const QString &szKeySequence = QString::null);
	void suicide() { delete this; };
protected:
	QString m_szSmallIcon;
	QString m_szCategory;
	KviKvsScript * m_pDescriptionScript;
	KviKvsScript * m_pVisibleNameScript;
protected:
	virtual bool isKviUserActionNeverOverrideThis();
	bool load(KviConfig * cfg);
	void save(KviConfig * cfg);
public:
	virtual const QString & visibleName();
	virtual const QString & description();
	const QString & visibleNameCode();
	const QString & descriptionCode();
	virtual QPixmap * smallIcon();
	const QString & category(){ return m_szCategory; };
	const QString & smallIconString(){ return m_szSmallIcon; };
	static void exportToKvs(QString &szBuffer,
		const QString &szName,
		const QString &szScriptCode,
		const QString &szVisibleName,
		const QString &szDescription,
		const QString &szCategory,
		const QString &szBigIcon,
		const QString &szSmallIcon,
		unsigned int uFlags,
		const QString &szKeySequence);
	void exportToKvs(QString &szBuffer);
};


#endif //!_KVI_KVS_USERACTION_H_
