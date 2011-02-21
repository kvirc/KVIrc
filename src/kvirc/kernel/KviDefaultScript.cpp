//=============================================================================
//
//   File : KviDefaultScript.cpp
//   Creation date : Fri Oct 01 2010 19:14:48 by Elvio Basello
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2010 Elvio Basello (hell at hellvis69 dot netsons dot org)
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

#include "KviDefaultScript.h"

#include "KviApplication.h"
#include "KviKvs.h"
#include "KviConfigurationFile.h"
#include "KviLocale.h"
#include "KviMiscUtils.h"
#include "KviActionManager.h"
#include "KviIconManager.h"
#include "KviCustomToolBarManager.h"

#include <QFile>
#include <QDate>
#include <QGroupBox>
#include <QCheckBox>
#include <QPushButton>
#include <QMessageBox>

KviDefaultScriptManager * KviDefaultScriptManager::m_pSelf = NULL;
unsigned int KviDefaultScriptManager::m_uCount = 0;

KviDefaultScriptManager::KviDefaultScriptManager()
: QObject()
{
	m_bNoNeedToRestore = false;

	// Check if versions' file exists in personal settings
	QString szLocal;
	g_pApp->getLocalKvircDirectory(szLocal,KviApplication::Config,"default.kvc");

	if(!QFile::exists(szLocal))
	{
		QString szGlobal;
		g_pApp->getGlobalKvircDirectory(szGlobal,KviApplication::DefScript,"default.kvc");
		QFile::copy(szGlobal,szLocal);
	}
}

KviDefaultScriptManager::~KviDefaultScriptManager()
{
}

void KviDefaultScriptManager::init()
{
	if((!m_pSelf) && (m_pSelf->count() == 0))
	{
		m_pSelf = new KviDefaultScriptManager();
		m_uCount++;
	}
}

void KviDefaultScriptManager::done()
{
	m_uCount--;
	if(m_pSelf->count() == 0)
		delete m_pSelf;
}

bool KviDefaultScriptManager::isDefscriptUpToDate()
{
	QString szConfig, szTmp;
	g_pApp->getGlobalKvircDirectory(szConfig,KviApplication::DefScript,"default.kvc");
	KviConfigurationFile oConfig(szConfig,KviConfigurationFile::Read);

	szTmp = oConfig.readEntry("Date");
	QDate cfgDate = QDate::fromString(szTmp,"yyyy-MM-dd");
	QDate userDate = QDate::fromString(m_szDate,"yyyy-MM-dd");

	return (cfgDate == userDate);
}

void KviDefaultScriptManager::restore()
{
	QString szGlobal, szError, szLocal;
	g_pApp->getGlobalKvircDirectory(szGlobal,KviApplication::DefScript,"default.kvc");

	m_pDialog = new KviDefaultScriptDialog();
	if(m_pDialog->exec() == QDialog::Rejected)
	{
		delete m_pDialog;
		return;
	}

	// Check data
	if(!compareVersions(szGlobal,&szError))
	{
		QMessageBox::warning(0,__tr2qs("Restore Default Script - Error"),szError,QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return;
	}

	if(m_bNoNeedToRestore)
	{
		if(QMessageBox::warning(0,__tr2qs("Restore Default Script - Warning"),szError,QMessageBox::Yes, QMessageBox::No,QMessageBox::Cancel) != QMessageBox::Yes)
			return;
	}

	restoreInternal();
}

void KviDefaultScriptManager::restoreInternal()
{
	QString szConfig, szTmp;
	g_pApp->getGlobalKvircDirectory(szConfig,KviApplication::DefScript,"default.kvc");
	KviConfigurationFile oConfig(szConfig,KviConfigurationFile::Read);

	m_szDate = oConfig.readEntry("Date");

	// Select elements to restore
	if((m_pDialog->m_pAction->isEnabled() && m_pDialog->m_pAction->isChecked()) || m_pDialog->m_pAll->isChecked())
	{
		if(m_pDialog->m_pData->isChecked())
			KviActionManager::instance()->killAllKvsUserActions();
		// No need to load here since we haven't a default actions script yet
		//g_pApp->loadDefaultScript("action");
		m_szAction = oConfig.readEntry("ActionVersion");
	}

	if((m_pDialog->m_pAddon->isEnabled() && m_pDialog->m_pAddon->isChecked()) || m_pDialog->m_pAll->isChecked())
	{
		if(m_pDialog->m_pData->isChecked())
			KviKvs::clearScriptAddons();
		// No need to load here since we haven't a default actions script yet
		//g_pApp->loadDefaultScript("addons");
		m_szAddon = oConfig.readEntry("AddonVersion");
	}

	if((m_pDialog->m_pAlias->isEnabled() && m_pDialog->m_pAlias->isChecked()) || m_pDialog->m_pAll->isChecked())
	{
		if(m_pDialog->m_pData->isChecked())
			KviKvs::clearAliases();
		g_pApp->loadDefaultScript("aliases");
		m_szAlias = oConfig.readEntry("AliasVersion");
	}

	if((m_pDialog->m_pClass->isEnabled() && m_pDialog->m_pClass->isChecked()) || m_pDialog->m_pAll->isChecked())
	{
		// TODO: clear classes
		g_pApp->loadDefaultScript("classes");
		m_szClass = oConfig.readEntry("ClassVersion");
	}

	if((m_pDialog->m_pEvent->isEnabled() && m_pDialog->m_pEvent->isChecked()) || m_pDialog->m_pAll->isChecked())
	{
		if(m_pDialog->m_pData->isChecked())
			KviKvs::clearAppEvents();
		g_pApp->loadDefaultScript("events");
		m_szEvent = oConfig.readEntry("EventVersion");
	}

	if((m_pDialog->m_pPopup->isEnabled() && m_pDialog->m_pPopup->isChecked()) || m_pDialog->m_pAll->isChecked())
	{
		if(m_pDialog->m_pData->isChecked())
			KviKvs::clearPopups();
		g_pApp->loadDefaultScript("popups");
		m_szPopup = oConfig.readEntry("PopupVersion");
	}

	if((m_pDialog->m_pRaw->isEnabled() && m_pDialog->m_pRaw->isChecked()) || m_pDialog->m_pAll->isChecked())
	{
		if(m_pDialog->m_pData->isChecked())
			KviKvs::clearRawEvents();
		// No need to load here since we haven't a default raw events script yet
		//g_pApp->loadDefaultScript("raws");
		m_szRaw = oConfig.readEntry("RawVersion");
	}

	if((m_pDialog->m_pToolbar->isEnabled() && m_pDialog->m_pToolbar->isChecked()) || m_pDialog->m_pAll->isChecked())
	{
		if(m_pDialog->m_pData->isChecked())
			KviCustomToolBarManager::instance()->clear();
		g_pApp->loadDefaultScript("toolbars");
		m_szToolbar = oConfig.readEntry("ToolbarVersion");
	}
}

#define CHECK(__name,__global,__local) \
	if(KviMiscUtils::compareVersions(__global,__local) == 1) \
	{ \
		*pszError = __tr2qs("There's something wrong in your personal data: my '%1' is %2, while yours is %3").arg(__name,__global,__local); \
		return false; \
	}

bool KviDefaultScriptManager::compareVersions(QString & szConfig, QString * pszError)
{
	QString szTmp, szTmp2;

	if(pszError)
		*pszError = "";

	KviConfigurationFile oConfig(szConfig,KviConfigurationFile::Read);

	szTmp = "Date";
	szTmp2 = oConfig.readEntry(szTmp);
	QDate cfgDate = QDate::fromString(szTmp2,"yyyy-MM-dd");
	QDate userDate = QDate::fromString(m_szDate,"yyyy-MM-dd");

	if(cfgDate <= userDate)
	{
		m_bNoNeedToRestore = true;
		*pszError = __tr2qs("Your default script is up to date.\nDo you want to restore it anyway?");
		return true;
	}

	szTmp = "Version";
	szTmp2 = oConfig.readEntry(szTmp);
	CHECK(szTmp,szTmp2,m_szVersion)

	szTmp = "ActionVersion";
	szTmp2 = oConfig.readEntry(szTmp);
	CHECK(szTmp,szTmp2,m_szAction)

	szTmp = "AddonVersion";
	szTmp2 = oConfig.readEntry(szTmp);
	CHECK(szTmp,szTmp2,m_szAction)

	szTmp = "AliasVersion";
	szTmp2 = oConfig.readEntry(szTmp);
	CHECK(szTmp,szTmp2,m_szAlias)

	szTmp = "ClassVersion";
	szTmp2 = oConfig.readEntry(szTmp);
	CHECK(szTmp,szTmp2,m_szClass)

	szTmp = "EventVersion";
	szTmp2 = oConfig.readEntry(szTmp);
	CHECK(szTmp,szTmp2,m_szEvent)

	szTmp = "PopupVersion";
	szTmp2 = oConfig.readEntry(szTmp);
	CHECK(szTmp,szTmp2,m_szPopup)

	szTmp = "RawVersion";
	szTmp2 = oConfig.readEntry(szTmp);
	CHECK(szTmp,szTmp2,m_szRaw)

	szTmp = "ToolbarVersion";
	szTmp2 = oConfig.readEntry(szTmp);
	CHECK(szTmp,szTmp2,m_szToolbar)

	return true;
}

void KviDefaultScriptManager::load(const QString & szConfigFile)
{
	KviConfigurationFile cfg(szConfigFile,KviConfigurationFile::Read);
	loadInternal(&cfg);
}

#define IS_EMPTY(__var,__default) \
	if(__var.isEmpty()) \
		__var = __default;

void KviDefaultScriptManager::loadInternal(KviConfigurationFile * pCfg)
{
	QString szTmp;
	QString szDate = QDate::currentDate().toString("yyyy-MM-dd");

	szTmp = "Version";
	m_szVersion = pCfg->readEntry(szTmp,KVI_VERSION);
	IS_EMPTY(m_szVersion,KVI_VERSION)

	szTmp = "Date";
	m_szDate = pCfg->readEntry(szTmp,szDate);
	IS_EMPTY(m_szDate,szDate)

	szTmp = "ActionVersion";
	m_szAction = pCfg->readEntry(szTmp,KVI_VERSION);
	IS_EMPTY(m_szAction,KVI_VERSION)

	szTmp = "AddonVersion";
	m_szAddon = pCfg->readEntry(szTmp,KVI_VERSION);
	IS_EMPTY(m_szAddon,KVI_VERSION)

	szTmp = "AliasVersion";
	m_szAlias = pCfg->readEntry(szTmp,KVI_VERSION);
	IS_EMPTY(m_szAlias,KVI_VERSION)

	szTmp = "ClassVersion";
	m_szClass = pCfg->readEntry(szTmp,KVI_VERSION);
	IS_EMPTY(m_szClass,KVI_VERSION)

	szTmp = "EventVersion";
	m_szEvent = pCfg->readEntry(szTmp,KVI_VERSION);
	IS_EMPTY(m_szEvent,KVI_VERSION)

	szTmp = "PopupVersion";
	m_szPopup = pCfg->readEntry(szTmp,KVI_VERSION);
	IS_EMPTY(m_szPopup,KVI_VERSION)

	szTmp = "RawVersion";
	m_szRaw = pCfg->readEntry(szTmp,KVI_VERSION);
	IS_EMPTY(m_szRaw,KVI_VERSION)

	szTmp = "ToolbarVersion";
	m_szToolbar = pCfg->readEntry(szTmp,KVI_VERSION);
	IS_EMPTY(m_szToolbar,KVI_VERSION)
}

void KviDefaultScriptManager::save(const QString & szConfigFile)
{
	KviConfigurationFile cfg(szConfigFile,KviConfigurationFile::Write);
	cfg.clear();
	saveInternal(&cfg);
}

void KviDefaultScriptManager::saveInternal(KviConfigurationFile * pCfg)
{
	QString szTmp;

	szTmp = "Version";
	pCfg->writeEntry(szTmp,m_szVersion);

	szTmp = "Date";
	pCfg->writeEntry(szTmp,m_szDate);

	szTmp = "ActionVersion";
	pCfg->writeEntry(szTmp,m_szAction);

	szTmp = "AddonVersion";
	pCfg->writeEntry(szTmp,m_szAddon);

	szTmp = "AliasVersion";
	pCfg->writeEntry(szTmp,m_szAlias);

	szTmp = "ClassVersion";
	pCfg->writeEntry(szTmp,m_szClass);

	szTmp = "EventVersion";
	pCfg->writeEntry(szTmp,m_szEvent);

	szTmp = "PopupVersion";
	pCfg->writeEntry(szTmp,m_szPopup);

	szTmp = "RawVersion";
	pCfg->writeEntry(szTmp,m_szRaw);

	szTmp = "ToolbarVersion";
	pCfg->writeEntry(szTmp,m_szToolbar);
}


KviDefaultScriptDialog::KviDefaultScriptDialog()
: QDialog()
{
	setObjectName("restore_default_script");
	setWindowTitle(__tr2qs("Restore Default Script - KVIrc"));
	setWindowModality(Qt::WindowModal);
	setModal(true);

	QGridLayout * pLayout = new QGridLayout(this);

	QLabel * pLabel = new QLabel(this);
	pLabel->setText(__tr2qs("You are about to restore the default script.\nAny script changes you have made will be erased if you\nrestore all stuff or \"Clear personal data\" is selected.\n\nDo you wish to proceed?\n"));
	pLayout->addWidget(pLabel,0,0,1,4);

	// Construct the advanced widget
	m_pAdvanced = new QGroupBox(__tr2qs("Select default script's elements to restore"),this);
	m_pAdvanced->hide();

	QGridLayout * pAdvLayout = new QGridLayout(m_pAdvanced);
	pAdvLayout->setContentsMargins(0,10,0,0);
	pLayout->addWidget(m_pAdvanced,1,0,1,4);

	m_pAll = new QCheckBox(__tr2qs("All"),m_pAdvanced);
	m_pAll->setChecked(true);
	pAdvLayout->addWidget(m_pAll,0,0);
	connect(m_pAll,SIGNAL(clicked(bool)),this,SLOT(toggleAll(bool)));

	m_pData = new QCheckBox(__tr2qs("Clear custom scripts"),m_pAdvanced);
	m_pData->setChecked(false);
	m_pData->setEnabled(true);
	pAdvLayout->addWidget(m_pData,0,1);

	pLabel = new QLabel(m_pAdvanced);
	pAdvLayout->addWidget(pLabel,1,1,1,2);

	m_pAction = new QCheckBox(__tr2qs("Actions"),m_pAdvanced);
	m_pAction->setChecked(true);
	m_pAction->setEnabled(false);
	pAdvLayout->addWidget(m_pAction,2,0);

	m_pAddon = new QCheckBox(__tr2qs("Addons"),m_pAdvanced);
	m_pAddon->setChecked(true);
	m_pAddon->setEnabled(false);
	pAdvLayout->addWidget(m_pAddon,2,1);

	m_pAlias = new QCheckBox(__tr2qs("Aliases"),m_pAdvanced);
	m_pAlias->setChecked(true);
	m_pAlias->setEnabled(false);
	pAdvLayout->addWidget(m_pAlias,3,0);

	m_pClass = new QCheckBox(__tr2qs("Classes"),m_pAdvanced);
	m_pClass->setChecked(true);
	m_pClass->setEnabled(false);
	pAdvLayout->addWidget(m_pClass,3,1);

	m_pEvent = new QCheckBox(__tr2qs("Events"),m_pAdvanced);
	m_pEvent->setChecked(true);
	m_pEvent->setEnabled(false);
	pAdvLayout->addWidget(m_pEvent,4,0);

	m_pPopup = new QCheckBox(__tr2qs("Popups"),m_pAdvanced);
	m_pPopup->setChecked(true);
	m_pPopup->setEnabled(false);
	pAdvLayout->addWidget(m_pPopup,4,1);

	m_pRaw = new QCheckBox(__tr2qs("Raw Events"),m_pAdvanced);
	m_pRaw->setChecked(true);
	m_pRaw->setEnabled(false);
	pAdvLayout->addWidget(m_pRaw,5,0);

	m_pToolbar = new QCheckBox(__tr2qs("Toolbars"),m_pAdvanced);
	m_pToolbar->setChecked(true);
	m_pToolbar->setEnabled(false);
	pAdvLayout->addWidget(m_pToolbar,5,1);

	m_pAdvanced->setLayout(pAdvLayout);

	QPixmap * pImage = g_pIconManager->getSmallIcon(262);
	QPushButton * pAdvanced = new QPushButton(*pImage,__tr2qs("Advanced..."),this);
	connect(pAdvanced,SIGNAL(clicked()),this,SLOT(advanced()));
	pLayout->addWidget(pAdvanced,2,0,1,1);

	pImage = g_pIconManager->getSmallIcon(44);
	QPushButton * pCancel = new QPushButton(*pImage,__tr2qs("Cancel"),this);
	pCancel->setDefault(true);
	connect(pCancel,SIGNAL(clicked()),this,SLOT(reject()));
	pLayout->addWidget(pCancel,2,2,1,1);

	pImage = g_pIconManager->getSmallIcon(43);
	QPushButton * pAccept = new QPushButton(*pImage,__tr2qs("Restore"),this);
	connect(pAccept,SIGNAL(clicked()),this,SLOT(accept()));
	pLayout->addWidget(pAccept,2,3,1,1);
}

KviDefaultScriptDialog::~KviDefaultScriptDialog()
{
}

void KviDefaultScriptDialog::toggleAll(bool)
{
	m_pAction->setEnabled(!m_pAction->isEnabled());
	m_pAddon->setEnabled(!m_pAddon->isEnabled());
	m_pAlias->setEnabled(!m_pAlias->isEnabled());
	m_pClass->setEnabled(!m_pClass->isEnabled());
	m_pEvent->setEnabled(!m_pEvent->isEnabled());
	m_pPopup->setEnabled(!m_pPopup->isEnabled());
	m_pRaw->setEnabled(!m_pRaw->isEnabled());
	m_pToolbar->setEnabled(!m_pToolbar->isEnabled());
}

void KviDefaultScriptDialog::advanced()
{
	m_pAdvanced->isVisible() ? m_pAdvanced->hide() : m_pAdvanced->show();
}

void KviDefaultScriptDialog::accept()
{
	QDialog::accept();
}

void KviDefaultScriptDialog::reject()
{
	QDialog::reject();
}
