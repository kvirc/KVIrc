//=============================================================================
//
//   File : kvi_kvs_action.cpp
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



#include "kvi_kvs_action.h"
#include "kvi_kvs_script.h"
#include "kvi_window.h"

KviKvsAction::KviKvsAction(QObject * pParent,
		const QString &szName,
		const QString &szScriptCode,
		const QString &szVisibleName,
		const QString &szDescription,
		KviActionCategory * pCategory,
		const QString &szBigIcon,
		int iSmallIconId,
		unsigned int uFlags,
		const QString &szKeySequence)
	: KviAction(pParent,szName,szVisibleName,szDescription,pCategory,szBigIcon,iSmallIconId,uFlags,szKeySequence)
{
	QString tmp = KVI_KVS_ACTION_SCRIPT_NAME_PREFIX;
	tmp += szName;
	m_pScript = new KviKvsScript(tmp,szScriptCode);
}


KviKvsAction::~KviKvsAction()
{
	unregisterAccelerator();
	delete m_pScript;
}

const QString & KviKvsAction::scriptCode()
{
	return m_pScript->code();
}

void KviKvsAction::activate()
{
	// at the moment it executes externally!
	// FIXME!
	if(!isEnabled())return; // no way
	m_pScript->run(g_pActiveWindow); // FIXME: complain more in case of error ?
}
