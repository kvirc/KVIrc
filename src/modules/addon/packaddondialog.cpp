//============================================================================
//
//   File : packaddondialog.cpp
//   Created on Sat 03 May 2008 01:40:44 by Elvio Basello
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2008 Elvio Basello <hellvis69 at netsons dot org>
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
	QString szPackageName = "MyAddon";
	QString szPackageAuthor = __tr2qs_ctx("Your name here","addon");
	QString szPackageDescription = __tr2qs_ctx("Put a package description here...","addon");
	QString szPackageVersion = "1.0.0";

	m_szPackagePath = QDir::homeDirPath();
	KviQString::ensureLastCharIs(m_szPackagePath,QChar(KVI_PATH_SEPARATOR_CHAR));
	/*
	m_szPackagePath += szPackageName;
	m_szPackagePath += "-";
	m_szPackagePath += szPackageVersion;
	m_szPackagePath += ".";
	m_szPackagePath += KVI_FILEEXTENSION_ADDONPACKAGE;
	*/

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
	pPage = new QWidget(this);
	pLayout = new QGridLayout(pPage);

	pLabel = new QLabel(pPage);
	pLabel->setText(__tr2qs_ctx("Here you need to provide informations about you (the packager) and a short description of the package you're creating.","addon"));
	pLabel->setTextFormat(Qt::RichText);
	pLayout->addWidget(pLabel,0,0,1,2);

	pLabel = new QLabel(pPage);
	pLabel->setText(__tr2qs_ctx("Package Author:","addon"));
	pLayout->addWidget(pLabel,1,0);
	
	m_pPackagerNameEdit = new QLineEdit(pPage);
	m_pPackagerNameEdit->setText(szPackageAuthor);
	pLayout->addWidget(m_pPackagerNameEdit,1,1);

	pLabel = new QLabel(pPage);
	pLabel->setText(__tr2qs_ctx("Package Name:","addon"));
	pLayout->addWidget(pLabel,2,0);
	
	m_pPackageNameEdit = new QLineEdit(pPage);
	m_pPackageNameEdit->setText(szPackageName);
	pLayout->addWidget(m_pPackageNameEdit,2,1);

	pLabel = new QLabel(pPage);
	pLabel->setText(__tr2qs_ctx("Package Version:","addon"));
	pLayout->addWidget(pLabel,3,0);
	
	m_pPackageVersionEdit = new QLineEdit(pPage);
	m_pPackageVersionEdit->setText(szPackageVersion);
	pLayout->addWidget(m_pPackageVersionEdit,3,1);

	pLabel = new QLabel(pPage);
	pLabel->setText(__tr2qs_ctx("Package Description:","addon"));
	pLayout->addWidget(pLabel,4,0);
	
	m_pPackageDescriptionEdit = new KviTalTextEdit(pPage);
	m_pPackageDescriptionEdit->setBackgroundRole(QPalette::Window);
	m_pPackageDescriptionEdit->setText(szPackageDescription);
	pLayout->addWidget(m_pPackageDescriptionEdit,4,1,1,2);
	pLayout->setRowStretch(1,1);

	addPage(pPage,__tr2qs_ctx("Package Informations","addon"));
	setBackEnabled(pPage,true);
	setHelpEnabled(pPage,false);
	setNextEnabled(pPage,true);
	setFinishEnabled(pPage,false);

	// select script files

	// select image dir

	// select help dir

	// select sound dir



	// final results
	pPage = new QWidget(this);
	pLayout = new QGridLayout(pPage);

	pLabel = new QLabel(pPage);
	pLabel->setText(__tr2qs_ctx("Here there are the informations you provided. If they are correct, hit the \"Finish\" button to complete the packaging operations.","addon"));
	pLayout->addWidget(pLabel,0,0,1,2);

	pLabel = new QLabel(pPage);
	pLabel->setText(__tr2qs_ctx("Package Author:","addon"));
	pLayout->addWidget(pLabel,1,0);

	pLabel = new QLabel(pPage);
	pLabel->setText(m_pPackagerNameEdit->text());
	pLayout->addWidget(pLabel,1,1);

	pLabel = new QLabel(pPage);
	pLabel->setText(__tr2qs_ctx("Package Name:","addon"));
	pLayout->addWidget(pLabel,2,0);

	pLabel = new QLabel(pPage);
	pLabel->setText(m_pPackageNameEdit->text());
	pLayout->addWidget(pLabel,2,1);

	pLabel = new QLabel(pPage);
	pLabel->setText(__tr2qs_ctx("Package Version:","addon"));
	pLayout->addWidget(pLabel,3,0);

	pLabel = new QLabel(pPage);
	pLabel->setText(m_pPackageVersionEdit->text());
	pLayout->addWidget(pLabel,3,1);

	pLabel = new QLabel(pPage);
	pLabel->setText(__tr2qs_ctx("Package Description:","addon"));
	pLayout->addWidget(pLabel,4,0);

	pLabel = new QLabel(pPage);
	pLabel->setText(m_pPackageDescriptionEdit->text());
	pLayout->addWidget(pLabel,4,1,1,2);
	pLayout->setRowStretch(1,1);

	addPage(pPage,__tr2qs_ctx("Package Path","addon"));
	setBackEnabled(pPage,true);
	setHelpEnabled(pPage,false);
	setNextEnabled(pPage,false);
	setFinishEnabled(pPage,true);
}

KviPackAddonDialog::~KviPackAddonDialog()
{
}

void KviPackAddonDialog::accept()
{
	if(!packAddon())return;
	KviTalWizard::accept();
}

bool KviPackAddonDialog::packAddon()
{
	//m_pPathSelector->commit();

	QString szPackageAuthor = m_pPackagerNameEdit->text();
	QString szPackageName = m_pPackageNameEdit->text();
	QString szPackageDescription = m_pPackageDescriptionEdit->text();
	QString szPackageVersion = m_pPackageVersionEdit->text();

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
