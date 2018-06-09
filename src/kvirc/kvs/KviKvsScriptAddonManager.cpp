//=============================================================================
//
//   File : KviKvsScriptAddonManager.cpp
//   Creation date : Thu 31 Mar 2005 01:21:23 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviKvsScriptAddonManager.h"
#include "KviKvsScript.h"
#include "KviConfigurationFile.h"
#include "KviWindow.h"
#include "KviIconManager.h"
#include "KviFileUtils.h"
#include "KviApplication.h"

#include <QFileInfo>
#include <utility>

KviKvsScriptAddonManager * KviKvsScriptAddonManager::m_pInstance = nullptr;

KviKvsScriptAddon::KviKvsScriptAddon(
    QString szName,
    QString szVersion,
    const QString & szVisibleNameCode,
    const QString & szDescriptionCode,
    const QString & szUninstallCallbackCode,
    QString szIconId)
    : KviHeapObject()
    , m_szName(std::move(szName))
    , m_szVersion(std::move(szVersion))
    , m_szIconId(std::move(szIconId))
{
	allocateScripts(szVisibleNameCode, szDescriptionCode, szUninstallCallbackCode);
	m_pConfigureCallback = nullptr;
	m_pHelpCallback = nullptr;
}

KviKvsScriptAddon::KviKvsScriptAddon(const KviKvsScriptAddon & a)
    : KviHeapObject()
{
	m_szName = a.m_szName;
	m_szVersion = a.m_szVersion;
	m_szIconId = a.m_szIconId;
	allocateScripts(a.m_pVisibleNameScript->code(), a.m_pDescriptionScript->code(), a.m_pUninstallCallback ? a.m_pUninstallCallback->code() : QString());
	m_pConfigureCallback = nullptr;
	setConfigureCallback(a.m_pConfigureCallback ? a.m_pConfigureCallback->code() : QString());
	m_pHelpCallback = nullptr;
	setHelpCallback(a.m_pHelpCallback ? a.m_pHelpCallback->code() : QString());
}

KviKvsScriptAddon::KviKvsScriptAddon()
    : KviHeapObject()
{
	m_pVisibleNameScript = nullptr;
	m_pDescriptionScript = nullptr;
	m_pUninstallCallback = nullptr;
	m_pConfigureCallback = nullptr;
	m_pHelpCallback = nullptr;
}

KviKvsScriptAddon::~KviKvsScriptAddon()
{
	if(m_pVisibleNameScript)
		delete m_pVisibleNameScript;
	if(m_pDescriptionScript)
		delete m_pDescriptionScript;
	if(m_pUninstallCallback)
		delete m_pUninstallCallback;
	if(m_pConfigureCallback)
		delete m_pConfigureCallback;
	if(m_pHelpCallback)
		delete m_pHelpCallback;
}

QPixmap * KviKvsScriptAddon::icon()
{
	//qDebug("Addon requests icon %s",m_szIconId.toUtf8().data());
	return g_pIconManager->getBigIcon(m_szIconId.isEmpty() ? QString(KVI_BIGICON_KVS) : m_szIconId);
}

const QString & KviKvsScriptAddon::visibleName()
{
	if(!m_pVisibleNameScript)
		return m_szVisibleName;
	if(!m_pVisibleNameScript->run(g_pActiveWindow, nullptr, m_szVisibleName))
		m_szVisibleName = m_pVisibleNameScript->code();
	return m_szVisibleName;
}

const QString & KviKvsScriptAddon::description()
{
	if(!m_pDescriptionScript)
		return m_szDescription;
	if(!m_pDescriptionScript->run(g_pActiveWindow, nullptr, m_szDescription))
		m_szDescription = m_pDescriptionScript->code();
	return m_szDescription;
}

const QString & KviKvsScriptAddon::visibleNameCode()
{
	return m_pVisibleNameScript->code();
}

const QString & KviKvsScriptAddon::descriptionCode()
{
	return m_pDescriptionScript->code();
}

const QString & KviKvsScriptAddon::uninstallCallbackCode()
{
	return m_pUninstallCallback->code();
}

const QString & KviKvsScriptAddon::configureCallbackCode()
{
	if(m_pConfigureCallback)
		return m_pConfigureCallback->code();
	return KviQString::Empty;
}

const QString & KviKvsScriptAddon::helpCallbackCode()
{
	if(m_pHelpCallback)
		return m_pHelpCallback->code();
	return KviQString::Empty;
}

bool KviKvsScriptAddon::load(KviConfigurationFile * cfg, const QString & szName)
{
	m_szName = szName;
	cfg->setGroup(m_szName);
	m_szVersion = cfg->readEntry("Version");
	m_szIconId = cfg->readEntry("IconId");
	if(m_szVersion.isEmpty())
		return false;
	QString tmp1, tmp2, tmp3;
	tmp1 = cfg->readEntry("VisibleNameCode");
	tmp2 = cfg->readEntry("DescriptionCode");
	tmp3 = cfg->readEntry("UninstallCallback");
	if(tmp1.isEmpty())
		return false;
	allocateScripts(tmp1, tmp2, tmp3);
	tmp1 = cfg->readEntry("ConfigureCallback");
	if(!tmp1.isEmpty())
		setConfigureCallback(tmp1);
	tmp1 = cfg->readEntry("HelpCallback");
	if(!tmp1.isEmpty())
		setHelpCallback(tmp1);
	m_lInstalledFiles = cfg->readStringListEntry("InstalledFiles");
	return true;
}

void KviKvsScriptAddon::addInstalledFile(const QString & szFileName)
{
	if(m_lInstalledFiles.contains(szFileName))
		return;
	m_lInstalledFiles.append(szFileName);
}

void KviKvsScriptAddon::setConfigureCallback(const QString & szConfigureCallbackCode)
{
	if(m_pConfigureCallback)
		delete m_pConfigureCallback;
	if(szConfigureCallbackCode.isEmpty())
	{
		m_pConfigureCallback = nullptr;
		return;
	}

	QString szKvsName = "addon::";
	szKvsName += m_szName;

	QString szTmp;

	szTmp = szKvsName;
	szTmp += "::configure";
	m_pConfigureCallback = new KviKvsScript(szTmp, szConfigureCallbackCode, KviKvsScript::InstructionList);
}

void KviKvsScriptAddon::setHelpCallback(const QString & szHelpCallbackCode)
{
	if(m_pHelpCallback)
		delete m_pHelpCallback;
	if(szHelpCallbackCode.isEmpty())
	{
		m_pHelpCallback = nullptr;
		return;
	}

	QString szKvsName = "addon::";
	szKvsName += m_szName;

	QString szTmp;

	szTmp = szKvsName;
	szTmp += "::help";
	m_pHelpCallback = new KviKvsScript(szTmp, szHelpCallbackCode, KviKvsScript::InstructionList);
}

void KviKvsScriptAddon::allocateScripts(const QString & szVisibleNameCode, const QString & szDescriptionCode, const QString & szUninstallCallbackCode)
{
	QString szKvsName = "addon::";
	szKvsName += m_szName;

	QString szTmp;

	szTmp = szKvsName;
	szTmp += "::name";
	m_pVisibleNameScript = new KviKvsScript(szTmp, szVisibleNameCode, KviKvsScript::Parameter);
	szTmp = szKvsName;
	szTmp += "::description";
	m_pDescriptionScript = new KviKvsScript(szTmp, szDescriptionCode, KviKvsScript::Parameter);
	szTmp = szKvsName;
	szTmp += "::uninstall";
	m_pUninstallCallback = new KviKvsScript(szTmp, szUninstallCallbackCode, KviKvsScript::InstructionList);
}

void KviKvsScriptAddon::save(KviConfigurationFile * cfg)
{
	cfg->setGroup(m_szName);
	cfg->writeEntry("Version", m_szVersion);
	cfg->writeEntry("VisibleNameCode", visibleNameCode());
	cfg->writeEntry("DescriptionCode", descriptionCode());
	cfg->writeEntry("UninstallCallback", uninstallCallbackCode());
	cfg->writeEntry("ConfigureCallback", configureCallbackCode());
	cfg->writeEntry("HelpCallback", helpCallbackCode());
	cfg->writeEntry("IconId", m_szIconId);
	cfg->writeEntry("InstalledFiles", m_lInstalledFiles);
}

void KviKvsScriptAddon::executeUninstallCallback(KviWindow * pWnd)
{
	if(!m_pUninstallCallback)
		return;
	m_pUninstallCallback->run(pWnd);
}

void KviKvsScriptAddon::executeConfigureCallback(KviWindow * pWnd)
{
	if(!m_pConfigureCallback)
		return;
	m_pConfigureCallback->run(pWnd);
}

void KviKvsScriptAddon::executeHelpCallback(KviWindow * pWnd)
{
	if(!m_pHelpCallback)
		return;
	m_pHelpCallback->run(pWnd);
}

KviKvsScriptAddonManager::KviKvsScriptAddonManager()
{
	m_pInstance = this;
	m_bLoaded = false;
	m_pAddonDict = new KviPointerHashTable<QString, KviKvsScriptAddon>(17, false);
	m_pAddonDict->setAutoDelete(true);
}

KviKvsScriptAddonManager::~KviKvsScriptAddonManager()
{
	delete m_pAddonDict;
}

void KviKvsScriptAddonManager::init()
{
	if(KviKvsScriptAddonManager::instance())
	{
		qDebug("WARNING: trying to create the KviKvsScriptAddonManager twice!");
		return;
	}
	(void)new KviKvsScriptAddonManager();
}

void KviKvsScriptAddonManager::done()
{
	if(!KviKvsScriptAddonManager::instance())
	{
		qDebug("WARNING: trying to destroy the KviKvsScriptAddonManager twice!");
		return;
	}
	delete KviKvsScriptAddonManager::instance();
}

void KviKvsScriptAddonManager::load(const QString & szFileName)
{
	// in fact we implement delayed loading
	// so this function only stores the filename
	// from which we will load at the first request
	m_szFileName = szFileName;
	// this to make sure that we reload the addons
	// if someone explicitly requests a load after we have already loaded
	// (this does not happen in kvirc tough at the moment)
	m_bLoaded = false;
}

void KviKvsScriptAddonManager::save(const QString & szFileName)
{
	if(!m_bLoaded)
		return; // nothing to store anyway
	// we're stored here from now on...
	m_szFileName = szFileName;

	KviConfigurationFile cfg(szFileName, KviConfigurationFile::Write);

	cfg.clear();

	KviPointerHashTableIterator<QString, KviKvsScriptAddon> it(*m_pAddonDict);

	while(KviKvsScriptAddon * a = it.current())
	{
		cfg.setGroup(a->name());
		a->save(&cfg);
		++it;
	}
}

void KviKvsScriptAddonManager::delayedLoad()
{
	if(m_bLoaded)
		return; // already loaded
	m_bLoaded = true;
	// ::load() might be never called if we don't have
	// a scriptaddons.kvc file on disk, KviApplication checks that.
	// So finally m_szFileName may be empty here
	if(m_szFileName.isEmpty())
		return;

	KviConfigurationFile cfg(m_szFileName, KviConfigurationFile::Read);

	KviPointerHashTable<QString, KviConfigurationFileGroup> * d = cfg.dict();
	if(!d)
		return;

	KviPointerHashTableIterator<QString, KviConfigurationFileGroup> it(*d);
	while(it.current())
	{
		QString szName = it.currentKey();
		KviKvsScriptAddon * a = new KviKvsScriptAddon();
		if(a->load(&cfg, szName))
			m_pAddonDict->replace(szName, a);
		else
			delete a;
		++it;
	}
}

KviPointerHashTable<QString, KviKvsScriptAddon> * KviKvsScriptAddonManager::addonDict()
{
	if(!m_bLoaded)
		delayedLoad();
	return m_pAddonDict;
}

bool KviKvsScriptAddonManager::registerAddon(KviKvsScriptAddonRegistrationData * d)
{
	if(findAddon(d->szName))
		return false;
	KviKvsScriptAddon * a = new KviKvsScriptAddon(
	    d->szName,
	    d->szVersion,
	    d->szVisibleNameScript,
	    d->szDescriptionScript,
	    d->szUninstallCallbackScript,
	    d->szIconId);
	m_pAddonDict->replace(d->szName, a);
	return true;
}

KviKvsScriptAddon * KviKvsScriptAddonManager::findAddon(const QString & szName)
{
	if(!m_bLoaded)
		delayedLoad();
	return m_pAddonDict->find(szName);
}

bool KviKvsScriptAddonManager::unregisterAddon(const QString & szName, KviWindow * pWnd, bool bExecuteUninstallCallback, bool bUninstallFiles)
{
	KviKvsScriptAddon * a = findAddon(szName);
	if(!a)
		return false;

	// remove the addon before executing the uninstall callback
	// so the user effectively can't call addon.unregister on itself in the uninstall callback code :D
	m_pAddonDict->setAutoDelete(false);
	m_pAddonDict->remove(szName);
	m_pAddonDict->setAutoDelete(true);

	if(bExecuteUninstallCallback)
		a->executeUninstallCallback(pWnd);

	if(bUninstallFiles)
	{
		// uninstall files
		QStringList lFiles = a->installedFiles();
		foreach(QString szFileName, lFiles)
		{
			QString szPath;
			g_pApp->getLocalKvircDirectory(szPath, KviApplication::None, szFileName);

			//qDebug("Uninstalling %s",szPath.toUtf8().data());

			QFileInfo inf(szPath);
			if(!inf.exists())
				continue;

			KviFileUtils::removeFile(szPath);
		}
		// FIXME: remove empty directories!!
	}

	delete a;
	return true;
}

void KviKvsScriptAddonManager::clear()
{
	if(!m_bLoaded)
		delayedLoad();
	m_pAddonDict->clear();
}
