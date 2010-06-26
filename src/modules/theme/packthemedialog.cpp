//=============================================================================
//
//   File : packthemedialog.cpp
//   Creation date : Wed 03 Jan 2007 01:11:44 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2007-2008 Szymon Stefanek <pragma at kvirc dot net>
//   Copyright (C) 2010 Elvio Basello <hell at hellvis69 dot netsons dot org>
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
//#include "kvi_msgbox.h"
#include "kvi_selectors.h"
#include "kvi_miscutils.h"
#include "kvi_sourcesdate.h"

#include <QLineEdit>
#include <QLabel>
#include <QTextEdit>

#include <QRegExp>
#include <QMessageBox>
#include <QDir>
#include <QDateTime>
#include <QPainter>
#include <QImage>
#include <QBuffer>
//#include <QComboBox>
//#include <QToolTip>
//#include <QPushButton>



KviPackThemeDialog::KviPackThemeDialog(QWidget * pParent, KviPointerList<KviThemeInfo> * pThemeInfoList)
: QWizard(pParent)
{
	setWindowTitle(__tr2qs_ctx("Export Theme - KVIrc","theme"));
	setMinimumSize(400,350);
	setDefaultProperty("QTextEdit","plainText",SIGNAL(textChanged()));
	m_pThemeInfoList = pThemeInfoList;
	
	QPixmap * pSide = g_pIconManager->getBigIcon("kvi_setup_label.png");
	QPixmap * pLogo = g_pIconManager->getBigIcon("kvi_bigicon_addons.png");

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	#if (_WIN32_WINNT >= 0x0600)
		// We are on a Windows Vista / Seven
		// It will fallback to XP if alpha compositing is disabled
		setWizardStyle(QWizard::AeroStyle);
	#else
		// Windows XP
		setWizardStyle(QWizard::ModernStyle);
		setPixmap(QWizard::WatermarkPixmap,*pSide);
		setPixmap(QWizard::LogoPixmap,*pLogo);
	#endif
#elif defined(COMPILE_ON_MAC)
	setWizardStyle(QWizard::MacStyle);
	setPixmap(QWizard::BackgroundPixmap,*pSide);
#else
	// All other systems
	setWizardStyle(QWizard::ClassicStyle);
	setPixmap(QWizard::WatermarkPixmap,*pSide);
	setPixmap(QWizard::LogoPixmap,*pLogo);
#endif

	// Add a default property for file selectors
	setDefaultProperty("KviFileSelector","tmpFile",SIGNAL(selectionChanged(const QString &)));

	// Welcome page
	QWizardPage * pPage = new QWizardPage(this);
	QVBoxLayout * pLayout = new QVBoxLayout(pPage);

	pPage->setLayout(pLayout);
	pPage->setTitle(__tr2qs_ctx("Welcome","theme"));
	
	QString szText = "<p>";
	szText += __tr2qs_ctx("This procedure allows you to export the selected themes to a single package. It is useful when you want to distribute your themes to the public.","theme");
	szText += "</p><p>";
	szText += __tr2qs_ctx("You will be asked to provide a package name, a description and, if you want, an icon/screenshot.","theme");
	szText += "</p><p>";
	szText += __tr2qs_ctx("Hit the \"Next\" button to begin.","theme");
	szText += "<p>";

	QLabel * pLabel = new QLabel(pPage);
	pLabel->setWordWrap(true);
	pLabel->setText(szText);
	pLayout->addWidget(pLabel);
	addPage(pPage);

	// Theme data
	m_pPackThemeDataWidget = new KviPackThemeDataWidget(this);
	addPage(m_pPackThemeDataWidget);

	// Packager information
	m_pPackThemeInfoWidget = new KviPackThemeInfoWidget(this);
	addPage(m_pPackThemeInfoWidget);
	
	// Screenshot/logo/icon
	m_pPackThemeImageWidget = new KviPackThemeImageWidget(this);
	addPage(m_pPackThemeImageWidget);
	
	// Save file name
	m_pPackThemeSaveWidget = new KviPackThemeSaveWidget(this);
	addPage(m_pPackThemeSaveWidget);

	//last thing to do before starting
	m_pPackThemeDataWidget->parseThemes(m_pThemeInfoList);
}

KviPackThemeDataWidget::KviPackThemeDataWidget(KviPackThemeDialog * pParent)
: QWizardPage(pParent)
{
	setObjectName("theme_package_data_page");
	setTitle(__tr2qs_ctx("Theme Data","theme"));
	setSubTitle(__tr2qs_ctx("This is the information list for the themes you're packaging. If it looks OK press \"Next\" to continue, otherwise press \"Cancel\" and rewiew your themes first.","theme"));
}

void KviPackThemeDataWidget::parseThemes(KviPointerList<KviThemeInfo> * pThemeInfoList)
{
	QString szPackageName;
	QString szPackageAuthor;
	QString szPackageDescription;
	QString szPackageVersion;
	KviThemeInfo * pThemeInfo = 0;
	bool bPackagePathSet = false;

	
	QString szPackagePath = QDir::homePath();
	KviQString::ensureLastCharIs(szPackagePath,QChar(KVI_PATH_SEPARATOR_CHAR));
	
	if(pThemeInfoList->count() > 1)
	{
		szPackageName = "MyThemes";
		szPackageAuthor = __tr2qs_ctx("Your name here","theme");
		szPackageVersion = "1.0.0";
		szPackageDescription = __tr2qs_ctx("Put a package description here...","theme");
	} else {
		if(pThemeInfoList->count() > 0)
		{
			pThemeInfo = pThemeInfoList->first();
			szPackageName = pThemeInfo->subdirectory();
			szPackageAuthor = pThemeInfo->author();
			szPackageDescription = pThemeInfo->description();
			szPackageVersion = pThemeInfo->version();

			szPackagePath += pThemeInfo->subdirectory();
			if(szPackagePath.indexOf(QRegExp("[0-9]\\.[0-9]")) == -1)
			{
				szPackagePath += "-";
				szPackagePath += szPackageVersion;
			}
			szPackagePath += KVI_FILEEXTENSION_THEMEPACKAGE;

			bPackagePathSet = true;
		}
	}
	
	if(!bPackagePathSet)
	{
		szPackagePath += szPackageName;
		szPackagePath += "-";
		szPackagePath += szPackageVersion;
		szPackagePath += KVI_FILEEXTENSION_THEMEPACKAGE;
	}

	QVBoxLayout * pLayout = new QVBoxLayout(this);

	QString szThemesDescription = "<html><body bgcolor=\"#ffffff\">";

	int iIdx = 0;
	QPixmap pixScreenshot;
	QString szScreenshotPath;

	for(pThemeInfo = pThemeInfoList->first(); pThemeInfo; pThemeInfo = pThemeInfoList->next())
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

	QTextEdit * pTextEdit = new QTextEdit(this);
	pTextEdit->setBackgroundRole(QPalette::Window);
	pTextEdit->setReadOnly(true);
	QTextDocument * pDoc = new QTextDocument(pTextEdit);
	pDoc->setHtml(szThemesDescription);
	pTextEdit->setDocument(pDoc);
	pLayout->addWidget(pTextEdit);

	setField("packageName",QVariant(szPackageName));
	setField("packageVersion",szPackageVersion);
	setField("packageDescription",szPackageDescription);
	setField("packageAuthor",szPackageAuthor);
}

KviPackThemeDataWidget::~KviPackThemeDataWidget()
{
}

KviPackThemeInfoWidget::KviPackThemeInfoWidget(KviPackThemeDialog * pParent)
: QWizardPage(pParent)
{
	setObjectName("theme_package_info_page");
	setTitle(__tr2qs_ctx("Package Information","theme"));
	setSubTitle(__tr2qs_ctx("Here you need to provide information about you (the packager) and a short description of the package you're creating.","theme"));
	

	QGridLayout * pLayout = new QGridLayout(this);

	QLabel * pLabel = new QLabel(this);
	pLabel->setText(__tr2qs_ctx("Package Name:","theme"));
	pLayout->addWidget(pLabel,1,0);

	m_pPackageNameEdit = new QLineEdit(this);

	pLabel->setBuddy(m_pPackageNameEdit);
	pLayout->addWidget(m_pPackageNameEdit,1,1);

	pLabel = new QLabel(this);
	pLabel->setText(__tr2qs_ctx("Version:","theme"));
	pLayout->addWidget(pLabel,2,0);

	m_pPackageVersionEdit = new QLineEdit(this);

	pLabel->setBuddy(m_pPackageVersionEdit);
	pLayout->addWidget(m_pPackageVersionEdit,2,1);

	pLabel = new QLabel(this);
	pLabel->setText(__tr2qs_ctx("Description:","theme"));
	pLayout->addWidget(pLabel,3,0);

	m_pPackageDescriptionEdit = new QTextEdit(this);

	pLabel->setBuddy(m_pPackageDescriptionEdit);
	pLayout->addWidget(m_pPackageDescriptionEdit,3,1);

	pLabel = new QLabel(this);
	pLabel->setText(__tr2qs_ctx("Package Author:","theme"));
	pLayout->addWidget(pLabel,4,0);

	m_pPackageAuthorEdit = new QLineEdit(this);

	pLabel->setBuddy(m_pPackageAuthorEdit);
	pLayout->addWidget(m_pPackageAuthorEdit,4,1);

	pLayout->setRowStretch(3,1);
	pLayout->setColumnStretch(1,1);
	
	// Store data in the fields
	registerField("packageName*",m_pPackageNameEdit);
	registerField("packageVersion*",m_pPackageVersionEdit);
	registerField("packageDescription*",m_pPackageDescriptionEdit);
	registerField("packageAuthor*",m_pPackageAuthorEdit);

}
void KviPackThemeInfoWidget::initializePage()
{
	QString szPackageName = field("packageName").toString();
	QString szPackageVersion = field("packageVersion").toString();
	QString szPackageDescription = field("packageDescription").toString();
	QString szPackageAuthor = field("packageAuthor").toString();
	m_pPackageNameEdit->setText(szPackageName);
	m_pPackageVersionEdit->setText(szPackageVersion);
	m_pPackageDescriptionEdit->setText(szPackageDescription);
	m_pPackageAuthorEdit->setText(szPackageAuthor);
}

KviPackThemeInfoWidget::~KviPackThemeInfoWidget()
{
}

KviPackThemeImageWidget::KviPackThemeImageWidget(KviPackThemeDialog * pParent)
: QWizardPage(pParent)
{
	setObjectName("theme_package_image_page");
	setTitle(__tr2qs_ctx("Icon/Screenshot","theme"));
	setSubTitle(__tr2qs_ctx("Here you can choose the image that will appear in the installation dialog for your theme package. It can be an icon, a logo or a screenshot and it should be not larger than 300x225. If you don't provide an image a simple default icon will be used at installation stage.","theme"));

	QVBoxLayout * pLayout = new QVBoxLayout(this);

	m_pImageLabel = new QLabel(this);
	m_pImageLabel->setFrameStyle(QFrame::Sunken | QFrame::Panel);
	m_pImageLabel->setMinimumSize(300,225);
	m_pImageLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
	pLayout->addWidget(m_pImageLabel);

	m_pImageSelector = new KviFileSelector(this,"",&m_szImagePath,true,0,KVI_FILTER_IMAGE);
	connect(m_pImageSelector,SIGNAL(selectionChanged(const QString &)),this,SLOT(imageSelectionChanged(const QString &)));
	pLayout->addWidget(m_pImageSelector);
	// Store data in the fields
	registerField("packageImagePath*",m_pImageSelector);

	//m_pImageSelectionPage = pPage;
}

KviPackThemeImageWidget::~KviPackThemeImageWidget()
{
}

void KviPackThemeImageWidget::imageSelectionChanged(const QString & szImagePath)
{
	QImage pix(szImagePath);
	if(!pix.isNull())
	{
		QPixmap out;
		if(pix.width() > 300 || pix.height() > 225)
		{
			pix=pix.scaled(300,225,Qt::KeepAspectRatio,Qt::SmoothTransformation);
			out=out.fromImage(pix);
		} else {
			out=out.fromImage(pix);
		}
		m_pImageLabel->setPixmap(out);
		return;
	}

	QMessageBox::critical(this,__tr2qs_ctx("Export Theme - KVIrc","theme"),__tr2qs_ctx("Failed to load the selected image","theme"),
		QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);

	m_pImageSelector->setSelection("");
	m_pImageLabel->setPixmap(QPixmap());
}

KviPackThemeSaveWidget::KviPackThemeSaveWidget(KviPackThemeDialog * pParent)
: QWizardPage(pParent)
{
	setObjectName("theme_package_save_page");
	setTitle(__tr2qs_ctx("Package Path","theme"));
	setSubTitle(__tr2qs_ctx("Here you must choose the file name for the theme package. It should have a *%1 extension.","theme").arg(KVI_FILEEXTENSION_THEMEPACKAGE));

	QVBoxLayout * pLayout = new QVBoxLayout(this);

	QString szFilter = "*";
	szFilter += KVI_FILEEXTENSION_THEMEPACKAGE;
	m_pSavePathSelector = new KviFileSelector(this,"",&m_szPackagePath,true,KviFileSelector::ChooseSaveFileName,szFilter);
	pLayout->addWidget(m_pSavePathSelector);

	QLabel * pLabel = new QLabel(this);
	pLabel->setWordWrap(true);
	pLabel->setText(__tr2qs_ctx("Finally hit the \"Finish\" button to complete the packaging operation.","theme"));
	pLayout->addWidget(pLabel);
	
	// Store data in the fields
	registerField("packageSavePath*",m_pSavePathSelector);
}

KviPackThemeSaveWidget::~KviPackThemeSaveWidget()
{
}

void KviPackThemeSaveWidget::initializePage()
{
	m_szPackagePath = field("packageSavePath").toString();
}
#if 0
	if(!szScreenshotPath.isEmpty())
	{
		m_pImageSelector->setSelection(szScreenshotPath);
		imageSelectionChanged(szScreenshotPath);
	}
}
#endif

KviPackThemeDialog::~KviPackThemeDialog()
{
}

void KviPackThemeDialog::accept()
{
	if(!packTheme()) return;
	QWizard::accept();
}

bool KviPackThemeDialog::packTheme()
{
	//m_pImageSelector->commit();
	//m_pPathSelector->commit();

	// Get data from registered fields
	m_szAuthor = field("packageAuthor").toString();
	m_szName = field("packageName").toString();
	m_szVersion = field("packageVersion").toString();
	m_szDescription = field("packageDescription").toString();
	m_szImagePath = field("packageImagePath").toString();
	m_szPackagePath = field("packageSavePath").toString();
	//m_szSavePath = field("packageSavePath").toString();

	//return false;

	QImage pix(m_szImagePath);
	QPixmap out;
	if(!pix.isNull())
	{
		if((pix.width() > 300) || (pix.height() > 225))
		{
			out=out.fromImage(pix.scaled(300,225,Qt::KeepAspectRatio));
		} else {
			out=out.fromImage(pix);
		}
	} else {
		if(!m_szImagePath.isEmpty())
		{
			QMessageBox::critical(this,
				__tr2qs_ctx("Export Theme - KVIrc","theme"),
				__tr2qs_ctx("Failed to load the selected image: please fix it","theme"),
				QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton
				);
			//setCurrentPage(m_pImageSelectionPage);
			return false;
		}
	}

	KviPackageWriter f;

	QString szTmp;
	QDateTime date = QDateTime::currentDateTime();
	
	switch(KVI_OPTION_UINT(KviOption_uintOutputDatetimeFormat))
	{
		case 0:
			szTmp = date.toString();
			break;
		case 1:
			szTmp = date.toString(Qt::ISODate);
			break;
		case 2:
			szTmp = date.toString(Qt::SystemLocaleDate);
			break;
	}

	f.addInfoField("PackageType","ThemePack");
	f.addInfoField("ThemePackVersion",KVI_CURRENT_THEME_ENGINE_VERSION);
	f.addInfoField("Name",m_szName);
	f.addInfoField("Version",m_szVersion);
	f.addInfoField("Author",m_szAuthor);
	f.addInfoField("Description",m_szDescription);
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
	for(KviThemeInfo * pInfo = m_pThemeInfoList->first(); pInfo; pInfo = m_pThemeInfoList->next())
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
			QMessageBox::critical(this,
				__tr2qs_ctx("Export Theme - KVIrc","theme"),
				szTmp,
				QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton
				);
		}

		iIdx++;
	}

	if(!f.pack(m_szPackagePath))
	{
		szTmp = __tr2qs_ctx("Packaging failed","theme");
		szTmp += ": ";
		szTmp += f.lastError();
		QMessageBox::critical(this,
			__tr2qs_ctx("Export Theme - KVIrc","theme"),
			szTmp,
			QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton
			);
			
		return false;
	}

	//KviPackageReader r;
	//r.unpack("/root/test.kvt","/root/unpacked_test_kvt");

	QMessageBox::information(this,
		__tr2qs_ctx("Export Theme - KVIrc","theme"),
		__tr2qs("Package saved successfully"),
		QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton
		);
		
	return true;
}
