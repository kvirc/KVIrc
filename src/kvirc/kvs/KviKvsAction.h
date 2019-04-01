#ifndef _KVI_KVSACTION_H_
#define _KVI_KVSACTION_H_
//=============================================================================
//
//   File : KviKvsAction.h
//   Creation date : Sat 04 Dec 2004 04:22:12 by Szymon Stefanek
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

/**
* \file KviKvsAction.h
* \author Szymon Stefanek
* \brief Actions handling
*/

#include "kvi_settings.h"
#include "KviAction.h"
#include "KviIconManager.h"

class KviKvsScript;

/**
* \class KviKvsAction
* \brief This class handles the actions
*/
class KVIRC_API KviKvsAction : public KviAction
{
	Q_OBJECT
protected:
	QString m_szScript;

public:
	/**
	* \brief Constructs the action object
	* \param pParent The parent object
	* \param szName The name of the action
	* \param szScriptCode The code of the script contained in the action
	* \param szVisibleName Permanent visible name, visible at least in the action drawer
	* \param szDescription The description of the action
	* \param pCategory One of KviActionManager::category*() or 0 (default category)
	* \param szBigIconId The id of a big icon (32x32)
	* \param szSmallIconId The id of a small icon (16x16)
	* \param uFlags Flags of the action like connection needed, context needed, ...
	* \param szKeySequence The shortcut to activate the action
	* \return KviKvsAction
	*/
	KviKvsAction(
	    QObject * pParent,
	    const QString & szName,
	    const QString & szScriptCode,
	    const QString & szVisibleName,
	    const QString & szDescription,
	    KviActionCategory * pCategory = nullptr,
	    const QString & szBigIconId = QString(),
	    const QString & szSmallIconId = QString(),
	    unsigned int uFlags = 0,
	    const QString & szKeySequence = QString());

	/**
	* \brief Constructs the action object
	* \param pParent The parent object
	* \param szName The name of the action
	* \param szScriptCode The code of the script contained in the action
	* \param szVisibleName Permanent visible name, visible at least in the action drawer
	* \param szDescription The description of the action
	* \param pCategory One of KviActionManager::category*() or 0 (default category)
	* \param szBigIconId The id of a big icon (32x32)
	* \param szSmallIconId The id of a small icon (16x16)
	* \param uFlags Flags of the action like connection needed, context needed, ...
	* \param szKeySequence The shortcut to activate the action
	* \return KviKvsAction
	*/
	KviKvsAction(
	    QObject * pParent,
	    const QString & szName,
	    const QString & szScriptCode,
	    const QString & szVisibleName,
	    const QString & szDescription,
	    KviActionCategory * pCategory = nullptr,
	    const QString & szBigIconId = QString(),
	    KviIconManager::SmallIcon eSmallIcon = KviIconManager::None,
	    unsigned int uFlags = 0,
	    const QString & szKeySequence = QString());

	/**
	* \brief Destroys the action object
	*/
	virtual ~KviKvsAction();

public:
	/**
	* \brief Returns the code of the script contained in the action
	* \return const QString &
	*/
	const QString & scriptCode();

	/**
	* \brief Executes the action
	* \return void
	*/
	virtual void activate();
};

#endif // _KVI_KVSACTION_H_
