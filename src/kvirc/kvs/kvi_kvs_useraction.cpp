//=============================================================================
//
//   File : kvi_kvs_useraction.cpp
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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#define __KVIRC__

#include "kvi_kvs_useraction.h"
#include "kvi_kvs_script.h"
#include "kvi_config.h"
#include "kvi_iconmanager.h"
#include "kvi_actionmanager.h"
#include "kvi_kvs_script.h"
#include "kvi_window.h"
#include "kvi_cmdformatter.h"

KviKvsUserAction::KviKvsUserAction(QObject * pParent,
		const QString &szName,
		const QString &szScriptCode,
		const QString &szVisibleNameCode,
		const QString &szDescriptionCode,
		const QString &szCategory,
		const QString &szBigIcon,
		const QString &szSmallIcon,
		unsigned int uFlags,
		const QString &szKeySequence)
	: KviKvsAction(pParent,szName,szScriptCode,szVisibleNameCode,szDescriptionCode,0,szBigIcon,0,uFlags,szKeySequence)
{
	m_szSmallIcon = szSmallIcon;

	QString szKvsName = "action::";
	szKvsName += szName;

	QString szTmp = szKvsName;
	szTmp += "::description";
	m_pDescriptionScript = new KviKvsScript(szTmp,szDescriptionCode,KviKvsScript::Parameter);

	szTmp = szKvsName;
	szTmp += "::label";
	m_pVisibleNameScript = new KviKvsScript(szTmp,szVisibleNameCode,KviKvsScript::Parameter);

	m_szCategory = szCategory;
	m_pCategory = KviActionManager::instance()->category(szCategory);
}


KviKvsUserAction::KviKvsUserAction(QObject * pParent)
	: KviKvsAction(pParent,QString::null,QString::null,QString::null,QString::null)
{
	// needs loading!
	m_pDescriptionScript = 0;
	m_pVisibleNameScript = 0;
}

KviKvsUserAction::~KviKvsUserAction()
{
	if(m_pDescriptionScript)delete m_pDescriptionScript;
	if(m_pVisibleNameScript)delete m_pVisibleNameScript;
}

KviKvsUserAction * KviKvsUserAction::createInstance(QObject * pParent,
		const QString &szName,
		const QString &szScriptCode,
		const QString &szVisibleNameCode,
		const QString &szDescriptionCode,
		const QString &szCategory,
		const QString &szBigIcon,
		const QString &szSmallIcon,
		unsigned int uFlags,
		const QString &szKeySequence)
{
	return new KviKvsUserAction(pParent,szName,szScriptCode,szVisibleNameCode,szDescriptionCode,szCategory,szBigIcon,szSmallIcon,uFlags,szKeySequence);
}


void KviKvsUserAction::exportToKvs(QString &szBuffer)
{
	exportToKvs(szBuffer,m_szName,scriptCode(),visibleNameCode(),descriptionCode(),m_szCategory,m_szBigIcon,m_szSmallIcon,m_uFlags,m_szKeySequence);
}

void KviKvsUserAction::exportToKvs(QString &szBuffer,
		const QString &szName,
		const QString &szScriptCode,
		const QString &szVisibleName,
		const QString &szDescription,
		const QString &szCategory,
		const QString &szBigIcon,
		const QString &szSmallIcon,
		unsigned int uFlags,
		const QString &szKeySequence)
{
	szBuffer += "action.create";
	if(uFlags & NeedsContext)
	{
		szBuffer += " -i";
		if(uFlags & NeedsConnection)
		{
			szBuffer += " -c";
			if(uFlags & EnableAtLogin)szBuffer += " -l";
		}
	}
	if(uFlags & InternalWindowMask)
	{
		QString szWins;
		if(uFlags & WindowConsole)szWins += "x";
		if(uFlags & WindowChannel)szWins += "c";
		if(uFlags & WindowQuery)szWins += "q";
		if(uFlags & WindowDccChat)szWins += "d";
		szBuffer += " -w=";
		szBuffer += szWins;
		if(uFlags & (WindowConsole | WindowChannel | WindowQuery))
		{
			if(uFlags & WindowOnlyIfUsersSelected)
				szBuffer += " -s";
		}
	}
	if(!szCategory.isEmpty())
	{
		szBuffer += " -t=";
		szBuffer += szCategory;
	}
	if(!szKeySequence.isEmpty())
	{
		szBuffer += " -k=\"";
		szBuffer += szKeySequence;
		szBuffer += "\"";
	}
	szBuffer += " (\"";
	szBuffer += szName;
	szBuffer += "\",";
	QString tmp = szVisibleName;
	if(tmp.contains('$'))
	{
		szBuffer += tmp;
		szBuffer += ",";
	} else {
		tmp.replace("\"","\\\"");
		szBuffer += "\"";
		szBuffer += tmp;
		szBuffer += "\",";
	}
	tmp = szDescription;
	if(tmp.contains('$'))
	{
		szBuffer += tmp;
		szBuffer += ",\"";
	} else {
		tmp.replace("\"","\\\"");
		szBuffer += "\"";
		szBuffer += tmp;
		szBuffer += "\",\"";
	}
	szBuffer += szBigIcon;
	szBuffer += "\",\"";
	szBuffer += szSmallIcon;
	szBuffer += "\")\n";
	tmp = szScriptCode;
	KviCommandFormatter::blockFromBuffer(tmp);
	szBuffer += tmp;
	szBuffer += "\n";
}


// FIXME: Cache this stuff ??

const QString & KviKvsUserAction::visibleName()
{
	if(!m_pVisibleNameScript)return m_szVisibleName;
	if(!m_pVisibleNameScript->run(g_pActiveWindow,0,m_szVisibleName))m_szVisibleName = m_pVisibleNameScript->code();
	return m_szVisibleName;
}

const QString & KviKvsUserAction::description()
{
	if(!m_pDescriptionScript)return m_szDescription;
	if(!m_pDescriptionScript->run(g_pActiveWindow,0,m_szDescription))m_szDescription = m_pDescriptionScript->code();
	return m_szDescription;
}

const QString & KviKvsUserAction::visibleNameCode()
{
	if(!m_pVisibleNameScript)return m_szVisibleName;
	return m_pVisibleNameScript->code();
}

const QString & KviKvsUserAction::descriptionCode()
{
	if(!m_pDescriptionScript)return m_szDescription;
	return m_pDescriptionScript->code();
}

QPixmap * KviKvsUserAction::smallIcon()
{
	return g_pIconManager->getImage(m_szSmallIcon.utf8().data());
}

bool KviKvsUserAction::isKviUserActionNeverOverrideThis()
{
	return true;
}

bool KviKvsUserAction::load(KviConfig * cfg)
{
	m_szName = cfg->group();
	if(m_szName.isEmpty())return false;
	m_szVisibleName = cfg->readQStringEntry("VisibleName");
	if(m_szVisibleName.isEmpty())return false;
	QString szKvsName = "action::";
	szKvsName += m_szName;
	QString szTmp = szKvsName;
	szTmp += "::label";
	
	if(m_pVisibleNameScript)delete m_pVisibleNameScript;
	m_pVisibleNameScript = new KviKvsScript(szTmp,m_szVisibleName,KviKvsScript::Parameter);
	
	if(m_szVisibleName.isEmpty())return false;
	m_szDescription = cfg->readQStringEntry("Description");
	szTmp = szKvsName;
	szTmp += "::description";
	
	if(m_pDescriptionScript)delete m_pDescriptionScript;
	m_pDescriptionScript = new KviKvsScript(szTmp,m_szDescription,KviKvsScript::Parameter);


	m_szBigIcon = cfg->readQStringEntry("BigIcon");
	m_szSmallIcon = cfg->readQStringEntry("SmallIcon");
	m_szKeySequence = cfg->readQStringEntry("KeySequence");
	m_szCategory = cfg->readQStringEntry("Category");
	m_pCategory = KviActionManager::instance()->category(m_szCategory);
	m_uFlags = cfg->readUIntEntry("Flags",0);

	QString szCode = cfg->readQStringEntry("Code");
	if(szCode.isEmpty())return false;
	
	if(m_pScript)delete m_pScript;
	QString tmp = KVI_KVS_ACTION_SCRIPT_NAME_PREFIX;
	tmp += m_szName;
	m_pScript = new KviKvsScript(tmp,szCode);

	return true;
}

void KviKvsUserAction::save(KviConfig * cfg)
{
	cfg->setGroup(m_szName);
	if(m_pVisibleNameScript)cfg->writeEntry("VisibleName",m_pVisibleNameScript->code());
	if(m_pDescriptionScript)
	{
		if(!m_pDescriptionScript->code().isEmpty())cfg->writeEntry("Description",m_pDescriptionScript->code());
	}
	if(!m_szBigIcon.isEmpty())cfg->writeEntry("BigIcon",m_szBigIcon);
	if(!m_szSmallIcon.isEmpty())cfg->writeEntry("SmallIcon",m_szSmallIcon);
	if(!m_szCategory.isEmpty())cfg->writeEntry("Category",m_szCategory);
	if(!m_szKeySequence.isEmpty())cfg->writeEntry("KeySequence",m_szKeySequence);
	if(m_uFlags != 0)cfg->writeEntry("Flags",m_uFlags);
	cfg->writeEntry("Code",m_pScript->code());
}

