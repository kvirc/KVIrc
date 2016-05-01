//=============================================================================
//
//   File : KviKvsAction.cpp
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

#include "KviKvsAction.h"
#include "KviKvsScript.h"
#include "KviWindow.h"

KviKvsAction::KviKvsAction(
    QObject * pParent,
    const QString & szName,
    const QString & szScriptCode,
    const QString & szVisibleName,
    const QString & szDescription,
    KviActionCategory * pCategory,
    const QString & szBigIconId,
    const QString & szSmallIconId,
    unsigned int uFlags,
    const QString & szKeySequence)
    : KviAction(
          pParent,
          szName,
          szVisibleName,
          szDescription,
          pCategory,
          szBigIconId,
          szSmallIconId,
          uFlags,
          szKeySequence)
{
	m_szScript = QString(szScriptCode);
}

KviKvsAction::KviKvsAction(
    QObject * pParent,
    const QString & szName,
    const QString & szScriptCode,
    const QString & szVisibleName,
    const QString & szDescription,
    KviActionCategory * pCategory,
    const QString & szBigIconId,
    KviIconManager::SmallIcon eSmallIcon,
    unsigned int uFlags,
    const QString & szKeySequence)
    : KviAction(
          pParent,
          szName,
          szVisibleName,
          szDescription,
          pCategory,
          szBigIconId,
          eSmallIcon,
          uFlags,
          szKeySequence)
{
	m_szScript = QString(szScriptCode);
}

KviKvsAction::~KviKvsAction()
    = default;

const QString & KviKvsAction::scriptCode()
{
	return m_szScript;
}

void KviKvsAction::activate()
{
	if(!isEnabled())
		return; // no way
	KviKvsScript::run(m_szScript, g_pActiveWindow);
}
