//=============================================================================
//
//   File : packthemedialog.cpp
//   Creation date : Wed 03 Jan 2007 01:11:44 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2007-2008 Szymon Stefanek <pragma at kvirc dot net>
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

#include "packthemedialog.h"
#include "themefunctions.h"

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



KviPackThemeDialog::KviPackThemeDialog(QWidget * pParent,KviPointerList<KviThemeInfo> * pThemeInfoList)
: KviTalWizard(pParent)
{
	m_pThemeInfoList = pThemeInfoList;

	KviThemeInfo * pThemeInfo;
	QString szPackageName;
	QString szPackageAuthor;
	QString szPackageDescription;
	QString szPackageVersion;

	m_szPackagePath = QDir::homePath();
	KviQString::ensureLastCharIs(m_szPackagePath,QChar(KVI_PATH_SEPARATOR_CHAR));

	bool bPackagePathSet = false;

	if(m_pThemeInfoList->count() > 1)
	{
		szPackageName = "MyThemes";
		szPackageAuthor = __tr2qs_ctx("Your name here","theme");
		szPackageVersion = "1.0.0";
		szPackageDescription = __tr2qs_ctx("Put a package description here...","theme");
	} else {
		if(m_pThemeInfoList->count() > 0)
		{
			pThemeInfo = m_pThemeInfoList->first();
			szPackageName = pThemeInfo->subdirectory();
			szPackageAuthor = pThemeInfo->author();
			szPackageDescription = pThemeInfo->description();
			szPackageVersion = pThemeInfo->version();

			m_szPackagePath += pThemeInfo->subdirectory();
			if(m_szPackagePath.indexOf(QRegExp("[0-9]\\.[0-9]")) == -1)
			{
				m_szPackagePath += "-";
				m_szPackagePath += szPackageVersion;
			}
			m_szPackagePath += ".";
			m_szPackagePath += KVI_FILEEXTENSION_THEMEPACKAGE;

			bPackagePathSet = true;
		}
	}

	if(!bPackagePathSet)
	{
		m_szPackagePath += szPackageName;
		m_szPackagePath += "-";
		m_szPackagePath += szPackageVersion;
		m_szPackagePath += ".";
		m_szPackagePath += KVI_FILEEXTENSION_THEMEPACKAGE;
	}

	setWindowTitle(__tr2qs_ctx("Export Theme - KVIrc","theme"));
	setMinimumSize(400,350);

	// welcome page ==================================================================================
	QWidget * pPage = new QWidget(this);
	QGridLayout * pLayout = new QGridLayout(pPage);

	QLabel * pLabel = new QLabel(pPage);
	QString szText = "<p>";
	szText += __tr2qs_ctx("This procedure allows you to export the selected themes to a single package. It is useful when you want to distribute your themes to the public.","theme");
	szText += "</p><p>";
	szText += __tr2qs_ctx("You will be asked to provide a package name, a description and, if you want, an icon/screenshot.","theme");
	szText += "</p><p>";
	szText += __tr2qs_ctx("Hit the \"Next\" button to begin.","theme");
	szText += "<p>";

	pLabel->setWordWrap(true);
	pLabel->setText(szText);
	pLayout->addWidget(pLabel,0,0);
	pLayout->setRowStretch(1,1);

	addPage(pPage,__tr2qs_ctx("Welcome","theme"));
	setBackEnabled(pPage,false);
	setNextEnabled(pPage,true);
	setHelpEnabled(pPage,false);
	setFinishEnabled(pPage,false);

	// theme data name ================================================================================

	pPage = new QWidget(this);
	pLayout = new QGridLayout(pPage);

	pLabel = new QLabel(pPage);
	pLabel->setWordWrap(true);
	pLabel->setText(__tr2qs_ctx("This is the information list for the themes you're packaging. If it looks OK press \"Next\" to continue, otherwise press \"Cancel\" and rewiew your themes first.","theme"));
	pLabel->setTextFormat(Qt::RichText);
	pLayout->addWidget(pLabel,0,0);

	QString szThemesDescription = "<html><body bgcolor=\"#ffffff\">";

	int iIdx = 0;
	QPixmap pixScreenshot;
	QString szScreenshotPath;

	for(pThemeInfo = m_pThemeInfoList->first();pThemeInfo;pThemeInfo = m_pThemeInfoList->next())
	{
		QString szThemeDescription;

		if(pixScreenshot.isNull())
		{
			pixScreenshot = pThemeInfo->smallScreenshot();
			if(!pixScreenshot.isNull())
				szScreenshotPath = pThemeInfo->smallScreenshotPath();
		}

		KviThemeFunctions::getThemeHtmlDescription(
			szThemeDescription,
			pThemeInfo->name(),
			pThemeInfo->version(),
			pThemeInfo->description(),
			pThemeInfo->subdirectory(),
			pThemeInfo->application(),
			pThemeInfo->author(),
			pThemeInfo->date(),
			pThemeInfo->themeEngineVersion(),
			pThemeInfo->smallScreenshot(),
			iIdx
		);

		if(iIdx > 0)
			szThemesDescription += "<hr>";
		szThemesDescription += szThemeDescription;
		iIdx++;
	}

	szThemesDescription += "</body></html>";

	KviTalTextEdit * pTextEdit = new KviTalTextEdit(pPage);
	pTextEdit->setBackgroundRole(QPalette::Window);
	pTextEdit->setReadOnly(true);
	QTextDocument *doc=new QTextDocument(pTextEdit);
	debug("Create dialog html");
	doc->setHtml(szThemesDescription);
	pTextEdit->setDocument(doc);
	//pTextEdit->setText(szThemesDescription);
	pLayout->addWidget(pTextEdit,1,0);
	pLayout->setRowStretch(1,1);

	addPage(pPage,__tr2qs_ctx("Theme Data","theme"));
	setBackEnabled(pPage,true);
	setHelpEnabled(pPage,false);
	setNextEnabled(pPage,true);
	setFinishEnabled(pPage,false);

	// packager informations ================================================================================

	pPage = new QWidget(this);
	pLayout = new QGridLayout(pPage);

	pLabel = new QLabel(pPage);
	pLabel->setWordWrap(true);
	pLabel->setText(__tr2qs_ctx("Here you need to provide informations about you (the packager) and a short description of the package you're creating.","theme"));
	pLabel->setTextFormat(Qt::RichText);
	pLayout->addWidget(pLabel,0,0,1,2);

	pLabel = new QLabel(pPage);
	pLabel->setWordWrap(true);
	pLabel->setText(__tr2qs_ctx("Package Name:","theme"));
	pLayout->addWidget(pLabel,1,0);

	m_pPackageNameEdit = new QLineEdit(pPage);
	m_pPackageNameEdit->setText(szPackageName);
	pLayout->addWidget(m_pPackageNameEdit,1,1);

	pLabel = new QLabel(pPage);
	pLabel->setWordWrap(true);
	pLabel->setText(__tr2qs_ctx("Version:","theme"));
	pLayout->addWidget(pLabel,2,0);

	m_pPackageVersionEdit = new QLineEdit(pPage);
	m_pPackageVersionEdit->setText(szPackageVersion);
	pLayout->addWidget(m_pPackageVersionEdit,2,1);

	pLabel = new QLabel(pPage);
	pLabel->setText(__tr2qs_ctx("Description:","theme"));
	pLayout->addWidget(pLabel,3,0);

	m_pPackageDescriptionEdit = new KviTalTextEdit(pPage);
	m_pPackageDescriptionEdit->setText(szPackageDescription);
	pLayout->addWidget(m_pPackageDescriptionEdit,3,1);

	pLabel = new QLabel(pPage);
	pLabel->setText(__tr2qs_ctx("Package Author:","theme"));
	pLayout->addWidget(pLabel,4,0);

	m_pPackagerNameEdit = new QLineEdit(pPage);
	m_pPackagerNameEdit->setText(szPackageAuthor);
	pLayout->addWidget(m_pPackagerNameEdit,4,1);


	pLayout->setRowStretch(3,1);
	pLayout->setColumnStretch(1,1);

	addPage(pPage,__tr2qs_ctx("Package Informations","theme"));
	setBackEnabled(pPage,true);
	setHelpEnabled(pPage,false);
	setNextEnabled(pPage,true);
	setFinishEnabled(pPage,false);

	// screenshot/logo/icon ================================================================================

	pPage = new QWidget(this);
	pLayout = new QGridLayout(pPage);

	pLabel = new QLabel(pPage);
	pLabel->setText(__tr2qs_ctx("Here you can choose the image that will appear in the installation dialog for your theme package. It can be an icon, a logo or a screenshot and it should be not larger than 300x225. If you don't provide an image a simple default icon will be used at installation stage.","theme"));
	pLabel->setTextFormat(Qt::RichText);
	pLayout->addWidget(pLabel,0,0);

	m_pImageLabel = new QLabel(pPage);
	m_pImageLabel->setFrameStyle(QFrame::Sunken | QFrame::Panel);
	m_pImageLabel->setMinimumSize(300,225);
	m_pImageLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
	pLayout->addWidget(m_pImageLabel,1,0);

	QString szFilter = "*.png *.jpg *.xpm";
	m_pImageSelector = new KviFileSelector(pPage,"",&m_szImagePath,true,0,szFilter);
	connect(m_pImageSelector,SIGNAL(selectionChanged(const QString &)),this,SLOT(imageSelectionChanged(const QString &)));
	pLayout->addWidget(m_pImageSelector,2,0);
	pLayout->setRowStretch(1,1);

	m_pImageSelectionPage = pPage;
	addPage(pPage,__tr2qs_ctx("Icon/Screenshot","theme"));
	setBackEnabled(pPage,true);
	setHelpEnabled(pPage,false);
	setNextEnabled(pPage,true);
	setFinishEnabled(pPage,false);

	// save file name ================================================================================

	pPage = new QWidget(this);
	pLayout = new QGridLayout(pPage);

	pLabel = new QLabel(pPage);
	pLabel->setText(__tr2qs_ctx("Here you must choose the file name for the theme package. It should have a *.%1 extension.","theme").arg(KVI_FILEEXTENSION_THEMEPACKAGE));
		pLabel->setWordWrap(true);
	pLabel->setTextFormat(Qt::RichText);
	pLayout->addWidget(pLabel,0,0);

	szFilter = "*.";
	szFilter += KVI_FILEEXTENSION_THEMEPACKAGE;
	m_pPathSelector = new KviFileSelector(pPage,"",&m_szPackagePath,true,KviFileSelector::ChooseSaveFileName,szFilter);
	pLayout->addWidget(m_pPathSelector,1,0);

	pLabel = new QLabel(pPage);
	pLabel->setText(__tr2qs_ctx("Finally hit the \"Finish\" button to complete the packaging operation.","theme"));
		pLabel->setWordWrap(true);
	pLabel->setTextFormat(Qt::RichText);
	pLayout->addWidget(pLabel,3,0);

	pLayout->setRowStretch(2,1);

	addPage(pPage,__tr2qs_ctx("Package Path","theme"));
	setBackEnabled(pPage,true);
	setHelpEnabled(pPage,false);
	setNextEnabled(pPage,false);
	setFinishEnabled(pPage,true);

	if(!szScreenshotPath.isEmpty())
	{
		m_pImageSelector->setSelection(szScreenshotPath);
		imageSelectionChanged(szScreenshotPath);
	}
}

KviPackThemeDialog::~KviPackThemeDialog()
{
}

void KviPackThemeDialog::imageSelectionChanged(const QString &szImagePath)
{
	QImage pix(szImagePath);
	if(!pix.isNull())
	{
		QPixmap out;
		if(pix.width() > 300 || pix.height() > 225)
			out.fromImage(pix.scaled(300,225,Qt::KeepAspectRatio,Qt::SmoothTransformation));
		else
			out.fromImage(pix);
		m_pImageLabel->setPixmap(out);
		return;
	}

	QMessageBox::critical(this,__tr2qs_ctx("Export Theme - KVIrc","theme"),__tr2qs_ctx("Failed to load the selected image","theme"),
		QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);

	m_pImageSelector->setSelection("");
	m_pImageLabel->setPixmap(QPixmap());
}

void KviPackThemeDialog::accept()
{
	if(!packTheme())return;
	KviTalWizard::accept();
}

bool KviPackThemeDialog::packTheme()
{
	m_pImageSelector->commit();
	m_pPathSelector->commit();

	QString szPackageAuthor = m_pPackagerNameEdit->text();
	QString szPackageName = m_pPackageNameEdit->text();
	QString szPackageDescription = m_pPackageDescriptionEdit->toPlainText();
	QString szPackageVersion = m_pPackageVersionEdit->text();

	QImage pix(m_szImagePath);
	QPixmap out;
	if(!pix.isNull())
	{
		if(pix.width() > 300 || pix.height() > 225)
			out.fromImage(pix.scaled(300,225,Qt::KeepAspectRatio));
		else
			out.fromImage(pix);
	} else {
		if(!m_szImagePath.isEmpty())
		{
			QMessageBox::critical(this,__tr2qs_ctx("Export Theme - KVIrc","theme"),__tr2qs_ctx("Failed to load the selected image: please fix it","theme"),
				QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
			setCurrentPage(m_pImageSelectionPage);
			return false;
		}
	}

	KviPackageWriter f;

	QString szTmp = QDateTime::currentDateTime().toString();

	f.addInfoField("PackageType","ThemePack");
	f.addInfoField("ThemePackVersion",KVI_CURRENT_THEME_ENGINE_VERSION);
	f.addInfoField("Name",szPackageName);
	f.addInfoField("Version",szPackageVersion);
	f.addInfoField("Author",szPackageAuthor);
	f.addInfoField("Description",szPackageDescription);
	f.addInfoField("Date",szTmp);
	f.addInfoField("Application","KVIrc " KVI_VERSION "." KVI_SOURCES_DATE);

	if(!out.isNull())
	{
		QByteArray * pba = new QByteArray();
		QBuffer buffer(pba,0);
		buffer.open(QIODevice::WriteOnly);
		out.save(&buffer,"PNG");
		buffer.close();
		f.addInfoField("Image",pba); // cool :) [no disk access needed]
	}

	szTmp.setNum(m_pThemeInfoList->count());
	f.addInfoField("ThemeCount",szTmp);

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
		QPixmap pixScreenshot = pInfo->smallScreenshot();
		if(!pixScreenshot.isNull())
		{
			KviQString::sprintf(szTmp,"Theme%dScreenshot",iIdx);
			QByteArray * pba = new QByteArray();

			QBuffer bufferz(pba,0);
			bufferz.open(QIODevice::WriteOnly);
			pixScreenshot.save(&bufferz,"PNG");
			bufferz.close();
			f.addInfoField(szTmp,pba);
		}

		if(!f.addDirectory(pInfo->absoluteDirectory(),pInfo->subdirectory()))
		{
			szTmp = __tr2qs_ctx("Packaging failed","theme");
			szTmp += ": ";
			szTmp += f.lastError();
			QMessageBox::critical(this,__tr2qs_ctx("Export Theme - KVIrc","theme"),szTmp,
					QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		}

		iIdx++;
	}

	if(!f.pack(m_szPackagePath))
	{
		szTmp = __tr2qs_ctx("Packaging failed","theme");
		szTmp += ": ";
		szTmp += f.lastError();
		QMessageBox::critical(this,__tr2qs_ctx("Export Theme - KVIrc","theme"),szTmp,
				QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return false;
	}

	//KviPackageReader r;
	//r.unpack("/root/test.kvt","/root/unpacked_test_kvt");

	QMessageBox::information(this,__tr2qs_ctx("Export Theme - KVIrc","theme"),__tr2qs("Package saved succesfully"),
				QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);

	return true;
}
