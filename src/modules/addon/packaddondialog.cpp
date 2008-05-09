//============================================================================
//
//   File : packaddondialog.cpp
//   Created on Sat 03 May 2008 01:40:44 by Elvio Basello
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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
#include "kvi_styled_controls.h"
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
	setWindowTitle(__tr2qs_ctx("Export Addon - KVIrc","addon"));
	setMinimumSize(400,350);

	// welcome page
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

	// packager informations
	m_pPackAddonInfoCreateWidget=new KviPackAddonCreateInfoPackageWidget(this);
	addPage(m_pPackAddonInfoCreateWidget,__tr2qs_ctx("Package Informations","addon"));
	setBackEnabled(m_pPackAddonInfoCreateWidget,true);
	setHelpEnabled(m_pPackAddonInfoCreateWidget,false);
	setNextEnabled(m_pPackAddonInfoCreateWidget,true);
	setFinishEnabled(m_pPackAddonInfoCreateWidget,false);

	// file selection
	m_pPackAddonFileSelectionWidget = new KviPackAddonFileSelectionWidget(this);
	addPage(m_pPackAddonFileSelectionWidget,__tr2qs_ctx("Package Files","addon"));
	setBackEnabled(m_pPackAddonFileSelectionWidget,true);
	setHelpEnabled(m_pPackAddonFileSelectionWidget,false);
	setNextEnabled(m_pPackAddonFileSelectionWidget,true);
	setFinishEnabled(m_pPackAddonFileSelectionWidget,false);

	// final results
	m_pPackAddonInfoWidget=new KviPackAddonInfoWidget(this);
	addPage(m_pPackAddonInfoWidget,__tr2qs_ctx("Package Path","addon"));
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
	//m_pPathSelector->commit();

	QString szPackageAuthor = m_pPackAddonInfoCreateWidget->authorName();
	QString szPackageName = m_pPackAddonInfoCreateWidget->packageName();
	QString szPackageVersion = m_pPackAddonInfoCreateWidget->packageVersion();
	QString szPackageDescription = m_pPackAddonInfoCreateWidget->packageDescription();

	QString szTmp = QDateTime::currentDateTime().toString();
	KviPackageWriter f;
	f.addInfoField("PackageType","AddonPack");
	f.addInfoField("AddonPackVersion","1");
	f.addInfoField("Name",szPackageName);
	f.addInfoField("Version",szPackageVersion);
	f.addInfoField("Author",szPackageAuthor);
	f.addInfoField("Description",szPackageDescription);
	f.addInfoField("Date",szTmp);
	f.addInfoField("Application","KVIrc " KVI_VERSION "." KVI_SOURCES_DATE);

	// create addon name
	m_szPackagePath = QDir::homeDirPath();
	KviQString::ensureLastCharIs(m_szPackagePath,QChar(KVI_PATH_SEPARATOR_CHAR));
	m_szPackagePath += szPackageName;
	m_szPackagePath += "-";
	m_szPackagePath += szPackageVersion;
	m_szPackagePath += ".";
	m_szPackagePath += KVI_FILEEXTENSION_ADDONPACKAGE;
	qDebug("Addon name: %s",m_szPackagePath.toUtf8().data());

/*
	szTmp.setNum(m_pThemeInfoList->count());
	f.addInfoField("AddonCount",szTmp);

	int iIdx = 0;
	for(KviThemeInfo * pInfo = m_pThemeInfoList->first();pInfo;pInfo = m_pThemeInfoList->next())
	{
		KviQString::sprintf(szTmp,"Theme%dName",iIdx);
		f.addInfoField(szTmp,pInfo->name());
		KviQString::sprintf(szTmp,"Theme%dVersion",iIdx);
		f.addInfoField(szTmp,pInfo->version());
		KviQString::sprintf(szTmp,"Theme%dDescription",iIdx);
		f.addInfoField(szTmp,pInfo->description());
		KviQString::sprintf(szTmp,"Theme%dDate",iIdx);
		f.addInfoField(szTmp,pInfo->date());
		KviQString::sprintf(szTmp,"Theme%dSubdirectory",iIdx);
		f.addInfoField(szTmp,pInfo->subdirectory());
		KviQString::sprintf(szTmp,"Theme%dAuthor",iIdx);
		f.addInfoField(szTmp,pInfo->author());
		KviQString::sprintf(szTmp,"Theme%dApplication",iIdx);
		f.addInfoField(szTmp,pInfo->application());
		KviQString::sprintf(szTmp,"Theme%dThemeEngineVersion",iIdx);
		f.addInfoField(szTmp,pInfo->themeEngineVersion());

		if(!f.addDirectory(pInfo->absoluteDirectory(),pInfo->subdirectory()))
		{
			szTmp = __tr2qs_ctx("Packaging failed","addon");
			szTmp += ": ";
			szTmp += f.lastError();
			QMessageBox::critical(this,__tr2qs_ctx("Export Addon - KVIrc","addon"),szTmp,QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		}

		iIdx++;
	}

	if(!f.pack(m_szPackagePath))
	{
		szTmp = __tr2qs_ctx("Packaging failed","addon");
		szTmp += ": ";
		szTmp += f.lastError();
		QMessageBox::critical(this,__tr2qs_ctx("Export Addon - KVIrc","addon"),szTmp,QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return false;
	}
	*/

	QMessageBox::information(this,__tr2qs_ctx("Export Addon - KVIrc","addon"),__tr2qs("Package saved succesfully"),QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);

	return true;
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

	QString szText = __tr2qs_ctx("Package Author:","addon") + " " + pCreateWidget->authorName() + "<br>";
	szText += __tr2qs_ctx("Package Name:","addon") + " " + pCreateWidget->packageName() + "<br>";
	szText += __tr2qs_ctx("Package Version:","addon") + " " + pCreateWidget->packageVersion() + "<br>";
	szText +=__tr2qs_ctx("Package Description:","addon") + " " + pCreateWidget->packageDescription() + "<br>";
	szText += __tr2qs_ctx("Source Directory:","addon") + " " + pFileWidget->sourcePath() + "<br>";
	szText += __tr2qs_ctx("Image Directory:","addon") + " " + pFileWidget->imagePath() + "<br>";
	szText += __tr2qs_ctx("Help Directory:","addon") + " " + pFileWidget->helpPath() + "<br>";
	szText += __tr2qs_ctx("Sound Directory:","addon") + " " + pFileWidget->soundPath();
	m_pLabelInfo->setText(szText);
}

KviPackAddonInfoWidget::~KviPackAddonInfoWidget()
{
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

KviPackAddonCreateInfoPackageWidget::~KviPackAddonCreateInfoPackageWidget()
{
}

KviPackAddonFileSelectionWidget::KviPackAddonFileSelectionWidget(KviPackAddonDialog *pParent)
: QWidget(pParent)
{
	QGridLayout * pLayout = new QGridLayout(this);

	QLabel * pLabel = new QLabel(this);
	pLabel->setText(__tr2qs_ctx("Here you need to provide the real informations for the addon.","addon"));
	pLayout->addWidget(pLabel,0,0);

	// select script dir
	QString * szOption = new QString();
	/* FIXME: this is an UGLY hack
                  should be better to fix KviDirectorySelector passing a QString as parameter and then setting the correct ptr to the value of the QString passed?
	*/

	const QString szScriptDir = __tr2qs_ctx("Select scripts directory:","addon");
	m_pSourcePath = new KviDirectorySelector(this,szScriptDir,szOption,true);
	pLayout->addWidget(m_pSourcePath,1,0);

	// select image dir
	const QString szImageDir = __tr2qs_ctx("Select images directory:","addon");
	m_pImagePath = new KviDirectorySelector(this,szImageDir,szOption,true);
	pLayout->addWidget(m_pImagePath,2,0);

	// select help dir
	const QString szHelpDir = __tr2qs_ctx("Select help directory:","addon");
	m_pHelpPath = new KviDirectorySelector(this,szHelpDir,szOption,true);
	pLayout->addWidget(m_pHelpPath,3,0);

	// select sound dir
	const QString szSoundDir = __tr2qs_ctx("Select sounds directory:","addon");
	m_pSoundPath = new KviDirectorySelector(this,szSoundDir,szOption,true);
	pLayout->addWidget(m_pSoundPath,4,0);
}

KviPackAddonFileSelectionWidget::~KviPackAddonFileSelectionWidget()
{
}
