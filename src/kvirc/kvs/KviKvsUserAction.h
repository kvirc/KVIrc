#ifndef _KVI_KVS_USERACTION_H_
#define _KVI_KVS_USERACTION_H_
//=============================================================================
//
//   File : KviKvsUserAction.h
//   Creation date : Tue 07 Dec 2004 02:00:24 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek <pragma at kvirc dot net>
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
#include "KviKvsAction.h"

class KviConfigurationFile;
class KviKvsScript;

class KVIRC_API KviKvsUserAction : public KviKvsAction
{
	friend class KviActionManager;
	Q_OBJECT
public:
	KviKvsUserAction(
	    QObject * pParent,
	    const QString & szName,
	    const QString & szScriptCode,
	    const QString & szVisibleNameCode,
	    const QString & szDescriptionCode,
	    const QString & szCategory,
	    const QString & szBigIconId,
	    const QString & szSmallIconId,
	    unsigned int uFlags,
	    const QString & szKeySequence = QString());
	KviKvsUserAction(QObject * pParent);
	~KviKvsUserAction();

	// Static allocator function.
	// This MUST be used by the modules to allocate action structures
	// instead of the new operator.
	// See KviHeapObject.cpp for an explanation.
	static KviKvsUserAction * createInstance(
	    QObject * pParent,
	    const QString & szName,
	    const QString & szScriptCode,
	    const QString & szVisibleNameCode,
	    const QString & szDescriptionCode,
	    const QString & szCategory,
	    const QString & szBigIconId,
	    const QString & szSmallIcon,
	    unsigned int uFlags,
	    const QString & szKeySequence = QString());
	void suicide() { delete this; };
protected:
	QString m_szCategory;
	KviKvsScript * m_pDescriptionScript;
	KviKvsScript * m_pVisibleNameScript;

protected:
	virtual bool isKviUserActionNeverOverrideThis();
	bool load(KviConfigurationFile * pCfg);
	void save(KviConfigurationFile * pCfg);

public:
	virtual const QString & visibleName();
	virtual const QString & description();
	const QString & visibleNameCode();
	const QString & descriptionCode();
	const QString & category() { return m_szCategory; };
	static void exportToKvs(
	    QString & szBuffer,
	    const QString & szName,
	    const QString & szScriptCode,
	    const QString & szVisibleName,
	    const QString & szDescription,
	    const QString & szCategory,
	    const QString & szBigIcon,
	    const QString & szSmallIcon,
	    unsigned int uFlags,
	    const QString & szKeySequence);
	void exportToKvs(QString & szBuffer);
};

#endif // _KVI_KVS_USERACTION_H_
