#ifndef _KVI_KVS_SCRIPTADDONMANAGER_H_
#define _KVI_KVS_SCRIPTADDONMANAGER_H_
//=============================================================================
//
//   File : KviKvsScriptAddonManager.h
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

#include "kvi_settings.h"
#include "KviHeapObject.h"
#include "KviQString.h"
#include "KviPointerHashTable.h"

#include <QStringList>

class KviConfigurationFile;
class KviKvsScript;
class KviWindow;
class QPixmap;

class KVIRC_API KviKvsScriptAddon : public KviHeapObject
{
	friend class KviKvsScriptAddonManager;

protected:
	KviKvsScriptAddon();
	KviKvsScriptAddon(
	    QString szName,
	    QString szVersion,
	    const QString & szVisibleNameCode,
	    const QString & szDescriptionCode,
	    const QString & szUninstallCallbackCode,
	    QString szIconId);

public:
	KviKvsScriptAddon(const KviKvsScriptAddon & a);

public: // KviDict wants that... how to restrict the deletion to KviKvsScriptAddonManager only ?
	~KviKvsScriptAddon();

protected:
	QString m_szName;                    // the short name of the addon
	QString m_szVersion;                 // x.y.z
	KviKvsScript * m_pVisibleNameScript; // the visible name, possibly translated
	KviKvsScript * m_pDescriptionScript; // the description, possibly translated
	KviKvsScript * m_pUninstallCallback; // uninstall callback
	KviKvsScript * m_pConfigureCallback; // configure callback
	KviKvsScript * m_pHelpCallback;      // help callback
	QString m_szDescription;             // parsed description
	QString m_szVisibleName;             // parsed visible name
	QString m_szIconId;                  // the id of the associated icon
	QStringList m_lInstalledFiles;       // the list of the installed files
public:
	const QStringList & installedFiles() const { return m_lInstalledFiles; };
	const QString & name() const { return m_szName; };
	const QString & version() const { return m_szVersion; };
	const QString & visibleName();
	const QString & description();
	const QString & visibleNameCode();
	const QString & descriptionCode();
	const QString & uninstallCallbackCode();
	const QString & configureCallbackCode();
	const QString & helpCallbackCode();
	const QString & iconId() { return m_szIconId; };
	QPixmap * icon();
	///
	/// Register a file to be uninstalled after the uninstall callback.
	/// The file name *MUST* be relative to the local kvirc directory.
	///
	void addInstalledFile(const QString & szFileName);
	void setConfigureCallback(const QString & szConfigureCallbackCode);
	void setHelpCallback(const QString & szHelpCallbackCode);
	void executeConfigureCallback(KviWindow * pWnd);
	void executeHelpCallback(KviWindow * pWnd);

protected:
	bool load(KviConfigurationFile * cfg, const QString & szName);
	void save(KviConfigurationFile * cfg);
	void executeUninstallCallback(KviWindow * pWnd);
	// this assumes that the script pointers are clean (i.e. not needing to be freed!)
	void allocateScripts(const QString & sVisibleNameCode, const QString & szDescriptionCode, const QString & szUninstallCallbackCode);
};

class KVIRC_API KviKvsScriptAddonRegistrationData
{
public:
	QString szName;                    // the addon name
	QString szVersion;                 // the addon version in form x.y.z
	QString szVisibleNameScript;       // the code that evaluates to the visible name, possibly translated
	QString szDescriptionScript;       // the code that evaluates to the description, possibly translated
	QString szUninstallCallbackScript; // the uninstall callback code, will be executed at uninstallation
	QString szIconId;                  // the icon identifier (scaled to 32x32 atm)
};

class KVIRC_API KviKvsScriptAddonManager
{
public:
	KviKvsScriptAddonManager();
	~KviKvsScriptAddonManager();

protected:
	static KviKvsScriptAddonManager * m_pInstance;
	// this class implements delayed loading
	QString m_szFileName;                                           // the file name that we will load from
	bool m_bLoaded;                                                 // have we loaded stuff from disk yet ?
	KviPointerHashTable<QString, KviKvsScriptAddon> * m_pAddonDict; // all the registered addons
public:
	static KviKvsScriptAddonManager * instance() { return m_pInstance; };
	static void init(); // called by KviKvs::init()
	static void done(); // called by KviKvs::done()

	bool registerAddon(KviKvsScriptAddonRegistrationData * d);
	KviKvsScriptAddon * findAddon(const QString & szName);
	bool unregisterAddon(const QString & szName, KviWindow * pWnd, bool bExecuteUninstallCallback = true, bool bUninstallFiles = true);
	KviPointerHashTable<QString, KviKvsScriptAddon> * addonDict();

	void clear();
	void load(const QString & szFileName); // called in the KviKvs namespace
	void save(const QString & szFileName); // called in the KViKvs namespace
protected:
	void delayedLoad();
};

#endif //!_KVI_KVS_SCRIPTADDONMANAGER_H_
