//============================================================================
//
//   File : packaddondialog.cpp
//   Creation date : Sat 03 May 2008 01:40:44 by Elvio Basello
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2008 Elvio Basello <hellvis69 at netsons dot org>
//   Copyright (C) 2008 Alessandro Carbone <elfonol at gmail dot com>
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
//============================================================================

#include "packaddondialog.h"
#include "addonfunctions.h"

#include "kvi_options.h"
#include "kvi_locale.h"
#include "kvi_config.h"
#include "kvi_fileutils.h"
#include "kvi_app.h"
#include "kvi_frame.h"
#include "kvi_iconmanager.h"
#include "kvi_packagefile.h"
#include "kvi_fileextensions.h"
#include "kvi_filedialog.h"
#include "kvi_msgbox.h"
#include "kvi_selectors.h"
#include "kvi_miscutils.h"
#include "kvi_sourcesdate.h"
#include "kvi_tal_textedit.h"

#include <QLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QRegExp>
#include <QMessageBox>
#include <QDir>
#include <QComboBox>
#include <QPainter>
#include <QToolTip>
#include <QImage>
#include <QDateTime>
#include <QBuffer>
#include <QLabel>
#include <QString>


KviPackAddonDialog::KviPackAddonDialog(QWidget * pParent)
: KviTalWizard(pParent)
{
	setWindowTitle(__tr2qs_ctx("Create Addon Package - KVIrc","addon"));
	setMinimumSize(400,350);

	// Welcome page
	QWidget * pPage = new QWidget(this);
	QGridLayout * pLayout = new QGridLayout(pPage);

	QLabel * pLabel = new QLabel(pPage);
	QString szText = "<p>";
	szText += __tr2qs_ctx("This procedure allows you to export the selected addon to a single package. It is useful when you want to distribute your addon to the public.","addon");
	szText += "</p><p>";
	szText += __tr2qs_ctx("You will be asked to provide a package name, a version and a description.","addon");
	szText += "</p><p>";
	szText += __tr2qs_ctx("Hit the \"Next\" button to begin.","addon");
	szText += "<p>";

	pLabel->setText(szText);
	pLayout->addWidget(pLabel,0,0);
	pLayout->setRowStretch(1,1);

	addPage(pPage,__tr2qs_ctx("Welcome","addon"));

	setBackEnabled(pPage,false);
	setNextEnabled(pPage,true);
	setHelpEnabled(pPage,false);
	setFinishEnabled(pPage,false);

	// Packager informations
	m_pPackAddonInfoCreateWidget=new KviPackAddonCreateInfoPackageWidget(this);
	addPage(m_pPackAddonInfoCreateWidget,__tr2qs_ctx("Package Informations","addon"));
	setBackEnabled(m_pPackAddonInfoCreateWidget,true);
	setHelpEnabled(m_pPackAddonInfoCreateWidget,false);
	setNextEnabled(m_pPackAddonInfoCreateWidget,true);
	setFinishEnabled(m_pPackAddonInfoCreateWidget,false);

	// File selection
	m_pPackAddonFileSelectionWidget = new KviPackAddonFileSelectionWidget(this);
	addPage(m_pPackAddonFileSelectionWidget,__tr2qs_ctx("Package Files","addon"));
	setBackEnabled(m_pPackAddonFileSelectionWidget,true);
	setHelpEnabled(m_pPackAddonFileSelectionWidget,false);
	setNextEnabled(m_pPackAddonFileSelectionWidget,true);
	setFinishEnabled(m_pPackAddonFileSelectionWidget,false);

	// Save selection
	m_pPackAddonSaveSelectionWidget = new KviPackAddonSaveSelectionWidget(this);
	addPage(m_pPackAddonSaveSelectionWidget,__tr2qs_ctx("Save Package","addon"));
	setBackEnabled(m_pPackAddonFileSelectionWidget,true);
	setHelpEnabled(m_pPackAddonFileSelectionWidget,false);
	setNextEnabled(m_pPackAddonFileSelectionWidget,true);
	setFinishEnabled(m_pPackAddonFileSelectionWidget,false);

	// Final results
	m_pPackAddonInfoWidget=new KviPackAddonInfoWidget(this);
	addPage(m_pPackAddonInfoWidget,__tr2qs_ctx("Final Informations","addon"));
	setBackEnabled(m_pPackAddonInfoWidget,true);
	setHelpEnabled(m_pPackAddonInfoWidget,false);
	setNextEnabled(m_pPackAddonInfoWidget,false);
	setFinishEnabled(m_pPackAddonInfoWidget,true);
}

void KviPackAddonDialog::accept()
{
	if(!packAddon())return;
	KviTalWizard::accept();
}

bool KviPackAddonDialog::packAddon()
{
	// Let's create the addon package
	QString szPackageAuthor = m_pPackAddonInfoCreateWidget->authorName();
	QString szPackageName = m_pPackAddonInfoCreateWidget->packageName();
	QString szPackageVersion = m_pPackAddonInfoCreateWidget->packageVersion();
	QString szPackageDescription = m_pPackAddonInfoCreateWidget->packageDescription();
	QString szSourcePath = m_pPackAddonFileSelectionWidget->sourcePath();
	QString szConfigPath = m_pPackAddonFileSelectionWidget->configPath();
	QString szImagePath = m_pPackAddonFileSelectionWidget->imagePath();
	QString szLocalePath = m_pPackAddonFileSelectionWidget->localePath();
	QString szHelpPath = m_pPackAddonFileSelectionWidget->helpPath();
	QString szSoundPath = m_pPackAddonFileSelectionWidget->soundPath();
	QString szInstallPath = m_pPackAddonFileSelectionWidget->installerPath();
	QString szPackagePath = m_pPackAddonSaveSelectionWidget->savePath();

	// We need mandatory unix like path separator
	szSourcePath.replace('\\',"/");
	szConfigPath.replace('\\',"/");
	szImagePath.replace('\\',"/");
	szLocalePath.replace('\\',"/");
	szHelpPath.replace('\\',"/");
	szSoundPath.replace('\\',"/");
	szInstallPath.replace('\\',"/");
	szPackagePath.replace('\\',"/");

	QString szTmp = QDateTime::currentDateTime().toString();

	KviPackageWriter pw;
	pw.addInfoField("PackageType","AddonPack");
	pw.addInfoField("AddonPackVersion",KVI_CURRENT_ADDONS_ENGINE_VERSION);
	pw.addInfoField("Name",szPackageName);
	pw.addInfoField("Version",szPackageVersion);
	pw.addInfoField("Author",szPackageAuthor);
	pw.addInfoField("Description",szPackageDescription);
	pw.addInfoField("Date",szTmp);
	pw.addInfoField("Application","KVIrc " KVI_VERSION "." KVI_SOURCES_DATE);

	// Add source dir
	if(!pw.addDirectory(szSourcePath,"src/"))
	{
		szTmp = __tr2qs_ctx("Packaging failed","addon");
		szTmp += ": ";
		szTmp += pw.lastError();
		QMessageBox::critical(this,__tr2qs_ctx("Export Addon - KVIrc","addon"),szTmp,QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
	}

	// Add config dir
	if(!pw.addDirectory(szConfigPath,"config/scripts/"))
	{
		szTmp = __tr2qs_ctx("Packaging failed","addon");
		szTmp += ": ";
		szTmp += pw.lastError();
		QMessageBox::critical(this,__tr2qs_ctx("Export Addon - KVIrc","addon"),szTmp,QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
	}

	// Add image dir
	if(!pw.addDirectory(szImagePath,"pics/"))
	{
		szTmp = __tr2qs_ctx("Packaging failed","addon");
		szTmp += ": ";
		szTmp += pw.lastError();
		QMessageBox::critical(this,__tr2qs_ctx("Export Addon - KVIrc","addon"),szTmp,QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
	}

	// Add localization dir
	if(!pw.addDirectory(szLocalePath,"locale/"))
	{
		szTmp = __tr2qs_ctx("Packaging failed","addon");
		szTmp += ": ";
		szTmp += pw.lastError();
		QMessageBox::critical(this,__tr2qs_ctx("Export Addon - KVIrc","addon"),szTmp,QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
	}

	// Add help dir
	if(!pw.addDirectory(szImagePath,"help/en/"))
	{
		szTmp = __tr2qs_ctx("Packaging failed","addon");
		szTmp += ": ";
		szTmp += pw.lastError();
		QMessageBox::critical(this,__tr2qs_ctx("Export Addon - KVIrc","addon"),szTmp,QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
	}

	// Add sound dir
	if(!pw.addDirectory(szImagePath,"audio/"))
	{
		szTmp = __tr2qs_ctx("Packaging failed","addon");
		szTmp += ": ";
		szTmp += pw.lastError();
		QMessageBox::critical(this,__tr2qs_ctx("Export Addon - KVIrc","addon"),szTmp,QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
	}

	// Add installer script
	if(!pw.addFile(szInstallPath,"install.kvs"))
	{
		szTmp = __tr2qs_ctx("Packaging failed","addon");
		szTmp += ": ";
		szTmp += pw.lastError();
		QMessageBox::critical(this,__tr2qs_ctx("Export Addon - KVIrc","addon"),szTmp,QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
	}

	// Create the addon package
	if(szPackagePath.isEmpty())
	{
		szPackagePath = QDir::homePath();
		KviQString::ensureLastCharIs(szPackagePath,QChar(KVI_PATH_SEPARATOR_CHAR));
		szPackagePath += szPackageName;
		szPackagePath += "-";
		szPackagePath += szPackageVersion;
		szPackagePath += ".";
		szPackagePath += KVI_FILEEXTENSION_ADDONPACKAGE;
		qDebug("Addon name used: %s",szPackagePath.toUtf8().data());
	}

	if(!pw.pack(szPackagePath))
	{
		szTmp = __tr2qs_ctx("Packaging failed","addon");
		szTmp += ": ";
		szTmp += pw.lastError();
		QMessageBox::critical(this,__tr2qs_ctx("Export Addon - KVIrc","addon"),szTmp,QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return false;
	}

	QMessageBox::information(this,__tr2qs_ctx("Export Addon - KVIrc","addon"),__tr2qs("Package saved successfully in ") + szPackagePath,QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);

	return true;
}

KviPackAddonCreateInfoPackageWidget::KviPackAddonCreateInfoPackageWidget(KviPackAddonDialog *pParent)
:QWidget(pParent)
{
	QString szPackageName = "MyAddon";
	QString szPackageAuthor = __tr2qs_ctx("Your name here","addon");
	QString szPackageDescription = __tr2qs_ctx("Put a package description here...","addon");
	QString szPackageVersion = "1.0.0";

	QGridLayout *pLayout = new QGridLayout(this);

	QLabel *pLabel = new QLabel(this);
	pLabel->setText(__tr2qs_ctx("Here you need to provide informations about you (the packager) and a short description of the package you're creating.","addon"));
	pLabel->setWordWrap(true);
	pLabel->setTextFormat(Qt::RichText);
	pLayout->addWidget(pLabel,0,0,1,2);

	pLabel = new QLabel(this);
	pLabel->setText(__tr2qs_ctx("Package Author:","addon"));
	pLayout->addWidget(pLabel,1,0);

	m_pAuthorNameEdit = new QLineEdit(this);
	m_pAuthorNameEdit->setText(szPackageAuthor);
	pLayout->addWidget(m_pAuthorNameEdit,1,1);

	pLabel = new QLabel(this);
	pLabel->setText(__tr2qs_ctx("Package Name:","addon"));
	pLayout->addWidget(pLabel,2,0);

	m_pPackageNameEdit = new QLineEdit(this);
	m_pPackageNameEdit->setText(szPackageName);
	pLayout->addWidget(m_pPackageNameEdit,2,1);

	pLabel = new QLabel(this);
	pLabel->setText(__tr2qs_ctx("Package Version:","addon"));
	pLayout->addWidget(pLabel,3,0);

	m_pPackageVersionEdit = new QLineEdit(this);
	m_pPackageVersionEdit->setText(szPackageVersion);
	pLayout->addWidget(m_pPackageVersionEdit,3,1);

	pLabel = new QLabel(this);
	pLabel->setText(__tr2qs_ctx("Package Description:","addon"));
	pLayout->addWidget(pLabel,4,0);

	m_pPackageDescriptionEdit = new KviTalTextEdit(this);
	m_pPackageDescriptionEdit->setBackgroundRole(QPalette::Window);
	m_pPackageDescriptionEdit->setText(szPackageDescription);
	pLayout->addWidget(m_pPackageDescriptionEdit,4,1,1,2);
	pLayout->setRowStretch(1,1);
}

KviPackAddonFileSelectionWidget::KviPackAddonFileSelectionWidget(KviPackAddonDialog *pParent)
: QWidget(pParent)
{
	QGridLayout * pLayout = new QGridLayout(this);

	QLabel * pLabel = new QLabel(this);
	pLabel->setText(__tr2qs_ctx("Here you need to provide the real informations for the addon.","addon"));
	pLayout->addWidget(pLabel,0,0);

	// Select installer script
	m_pInstallPathSelector = new KviFileSelector(this,__tr2qs_ctx("Select installer script:","addon"),&szInstallPath,true,0,"*.kvs");
	pLayout->addWidget(m_pInstallPathSelector,1,0);

	// Select script dir
	m_pSourcePathSelector = new KviDirectorySelector(this,__tr2qs_ctx("Select scripts directory:","addon"),&szSourcePath,true);
	pLayout->addWidget(m_pSourcePathSelector,2,0);

	// Select config dir
	m_pConfigPathSelector = new KviDirectorySelector(this,__tr2qs_ctx("Select config directory:","addon"),&szConfigPath,true);
	pLayout->addWidget(m_pConfigPathSelector,3,0);

	// Select image dir
	m_pImagePathSelector = new KviDirectorySelector(this,__tr2qs_ctx("Select images directory:","addon"),&szImagePath,true);
	pLayout->addWidget(m_pImagePathSelector,4,0);

	// Select localization dir
	m_pLocalePathSelector = new KviDirectorySelector(this,__tr2qs_ctx("Select localization directory:","addon"),&szLocalePath,true);
	pLayout->addWidget(m_pLocalePathSelector,5,0);

	// Select help dir
	m_pHelpPathSelector = new KviDirectorySelector(this,__tr2qs_ctx("Select help directory:","addon"),&szHelpPath,true);
	pLayout->addWidget(m_pHelpPathSelector,6,0);

	// Select sound dir
	m_pSoundPathSelector = new KviDirectorySelector(this,__tr2qs_ctx("Select sounds directory:","addon"),&szSoundPath,true);
	pLayout->addWidget(m_pSoundPathSelector,7,0);
}

KviPackAddonSaveSelectionWidget::KviPackAddonSaveSelectionWidget(KviPackAddonDialog *pParent)
: QWidget(pParent)
{
	QGridLayout * pLayout = new QGridLayout(this);

	QLabel * pLabel = new QLabel(this);
	pLabel->setText(__tr2qs_ctx("Here you need to provide the path where to save the addon package","addon"));
	pLayout->addWidget(pLabel,0,0);

	// Create addon name
	KviPackAddonCreateInfoPackageWidget * pCreateWidget=pParent->m_pPackAddonInfoCreateWidget;

	szSavePath = QDir::homePath();
	KviQString::ensureLastCharIs(szSavePath,QChar(KVI_PATH_SEPARATOR_CHAR));
	szSavePath += pCreateWidget->packageName();
	szSavePath += "-";
	szSavePath += pCreateWidget->packageVersion();
	szSavePath += ".";
	szSavePath += KVI_FILEEXTENSION_ADDONPACKAGE;
	qDebug("Addon name selected: %s",szSavePath.toUtf8().data());

	// Setting dialog filter
	QString szFilter = "*.";
	szFilter += KVI_FILEEXTENSION_ADDONPACKAGE;

	// Select save path
	m_pSavePathSelector = new KviFileSelector(this,__tr2qs_ctx("Select save path:","addon"),&szSavePath,true,KviFileSelector::ChooseSaveFileName,szFilter);
	pLayout->addWidget(m_pSavePathSelector,1,0);
}

KviPackAddonInfoWidget::KviPackAddonInfoWidget(KviPackAddonDialog *pParent)
:QWidget(pParent)
{
	m_pParent=pParent;
	QGridLayout *pLayout = new QGridLayout(this);

	QLabel *pLabel = new QLabel(this);
	pLabel->setText(__tr2qs_ctx("Here there are the informations you provided. If they are correct, hit the \"Finish\" button to complete the packaging operations.","addon"));
	pLabel->setWordWrap(true);
	pLayout->addWidget(pLabel,0,0);

	m_pLabelInfo = new QLabel(this);
	pLayout->addWidget(m_pLabelInfo,1,0);
}

void KviPackAddonInfoWidget::showEvent(QShowEvent *)
{
	KviPackAddonCreateInfoPackageWidget * pCreateWidget=m_pParent->m_pPackAddonInfoCreateWidget;
	KviPackAddonFileSelectionWidget * pFileWidget = m_pParent->m_pPackAddonFileSelectionWidget;
	KviPackAddonSaveSelectionWidget * pSaveWidget = m_pParent->m_pPackAddonSaveSelectionWidget;

	QString szText = __tr2qs_ctx("This is what I will check for","addon") + "<br>";
	szText += "<b>" + __tr2qs_ctx("Package Author","addon") + ":</b> " + pCreateWidget->authorName() + "<br>";
	szText += "<b>" + __tr2qs_ctx("Package Name","addon") + ":</b> " + pCreateWidget->packageName() + "<br>";
	szText += "<b>" + __tr2qs_ctx("Package Version","addon") + ":</b> " + pCreateWidget->packageVersion() + "<br>";
	szText += "<b>" + __tr2qs_ctx("Package Description","addon") + ":</b> " + pCreateWidget->packageDescription() + "<br><br>";
	szText += "<b>" + __tr2qs_ctx("Installer Script","addon") + ":</b> " + pFileWidget->installerPath() + "<br>";
	szText += "<b>" + __tr2qs_ctx("Source Directory","addon") + ":</b> " + pFileWidget->sourcePath() + "<br>";
	szText += "<b>" + __tr2qs_ctx("Configuration Directory","addon") + ":</b> " + pFileWidget->configPath() + "<br>";
	szText += "<b>" + __tr2qs_ctx("Image Directory","addon") + ":</b> " + pFileWidget->imagePath() + "<br>";
	szText += "<b>" + __tr2qs_ctx("Localization Directory","addon") + ":</b> " + pFileWidget->localePath() + "<br>";
	szText += "<b>" + __tr2qs_ctx("Help Directory","addon") + ":</b> " + pFileWidget->helpPath() + "<br>";
	szText += "<b>" + __tr2qs_ctx("Sound Directory","addon") + ":</b> " + pFileWidget->soundPath() + "<br>";
	szText += "<b>" + __tr2qs_ctx("Save Path","addon") + ":</b> " + pSaveWidget->savePath();
	m_pLabelInfo->setText(szText);
}
