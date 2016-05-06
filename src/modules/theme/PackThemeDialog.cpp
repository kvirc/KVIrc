//=============================================================================
//
//   File : PackThemeDialog.cpp
//   Creation date : Wed 03 Jan 2007 01:11:44 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2007-2010 Szymon Stefanek <pragma at kvirc dot net>
//   Copyright (C) 2010 Elvio Basello <hell at hellvis69 dot netsons dot org>
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

#include "PackThemeDialog.h"
#include "ThemeFunctions.h"

#include "KviOptions.h"
#include "KviLocale.h"
#include "KviConfigurationFile.h"
#include "KviFileUtils.h"
#include "KviApplication.h"
#include "KviMainWindow.h"
#include "KviIconManager.h"
#include "KviPackageWriter.h"
#include "kvi_fileextensions.h"
#include "KviSelectors.h"
#include "KviMiscUtils.h"
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

PackThemeDialog::PackThemeDialog(QWidget * pParent, KviPointerList<KviThemeInfo> * pThemeInfoList)
    : QWizard(pParent)
{
	setWindowTitle(__tr2qs_ctx("Export Theme - KVIrc", "theme"));
	setMinimumSize(400, 350);
	setDefaultProperty("QTextEdit", "plainText", SIGNAL(textChanged()));
	m_pThemeInfoList = pThemeInfoList;

	QPixmap * pSide = g_pIconManager->getBigIcon("kvi_setup_label.png");
	QPixmap * pLogo = g_pIconManager->getBigIcon("kvi_bigicon_addons.png");

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
#if(_WIN32_WINNT >= 0x0600)
	// We are on a Windows Vista / Seven
	// It will fallback to XP if alpha compositing is disabled
	setWizardStyle(QWizard::AeroStyle);
#else
	// Windows XP
	setWizardStyle(QWizard::ModernStyle);
	setPixmap(QWizard::WatermarkPixmap, *pSide);
	setPixmap(QWizard::LogoPixmap, *pLogo);
#endif
#elif defined(COMPILE_ON_MAC)
	setWizardStyle(QWizard::MacStyle);
	setPixmap(QWizard::BackgroundPixmap, *pSide);
#else
	// All other systems
	setWizardStyle(QWizard::ClassicStyle);
	setPixmap(QWizard::WatermarkPixmap, *pSide);
	setPixmap(QWizard::LogoPixmap, *pLogo);
#endif

	// Add a default property for file selectors
	setDefaultProperty("KviFileSelector", "tmpFile", SIGNAL(selectionChanged(const QString &)));

	// Welcome page
	QWizardPage * pPage = new QWizardPage(this);
	QVBoxLayout * pLayout = new QVBoxLayout(pPage);

	pPage->setLayout(pLayout);
	pPage->setTitle(__tr2qs_ctx("Welcome", "theme"));

	QString szText = "<p>";
	szText += __tr2qs_ctx("This procedure allows you to export the selected themes to a single package. It is useful when you want to distribute your themes to the public.", "theme");
	szText += "</p><p>";
	szText += __tr2qs_ctx("You will be asked to provide a package name, a description and, if you want, an icon/screenshot.", "theme");
	szText += "</p><p>";
	szText += __tr2qs_ctx("Hit the \"Next\" button to begin.", "theme");
	szText += "<p>";

	QLabel * pLabel = new QLabel(pPage);
	pLabel->setWordWrap(true);
	pLabel->setText(szText);
	pLayout->addWidget(pLabel);
	addPage(pPage);

	// Theme data
	m_pPackThemeDataWidget = new PackThemeDataWidget(this);
	addPage(m_pPackThemeDataWidget);

	// Packager information
	m_pPackThemeInfoWidget = new PackThemeInfoWidget(this);
	addPage(m_pPackThemeInfoWidget);

	// Screenshot/logo/icon
	m_pPackThemeImageWidget = new PackThemeImageWidget(this);
	addPage(m_pPackThemeImageWidget);

	// Save file name
	m_pPackThemeSaveWidget = new PackThemeSaveWidget(this);
	addPage(m_pPackThemeSaveWidget);

	//last thing to do before starting
	m_pPackThemeDataWidget->parseThemes(m_pThemeInfoList);
}

PackThemeDataWidget::PackThemeDataWidget(PackThemeDialog * pParent)
    : QWizardPage(pParent)
{
	setObjectName("theme_package_data_page");
	setTitle(__tr2qs_ctx("Theme Data", "theme"));
	setSubTitle(__tr2qs_ctx("This is the information list for the themes you're packaging. If it looks OK press \"Next\" to continue, otherwise press \"Cancel\" and review your themes first.", "theme"));
}

void PackThemeDataWidget::parseThemes(KviPointerList<KviThemeInfo> * pThemeInfoList)
{
	QString szPackageName;
	QString szPackageAuthor;
	QString szPackageDescription;
	QString szPackageVersion;
	KviThemeInfo * pThemeInfo = nullptr;
	bool bPackagePathSet = false;

	QString szPackagePath = QDir::homePath();
	KviQString::ensureLastCharIs(szPackagePath, QChar(KVI_PATH_SEPARATOR_CHAR));

	if(pThemeInfoList->count() > 1)
	{
		szPackageName = "MyThemes";
		szPackageAuthor = __tr2qs_ctx("Your name here", "theme");
		szPackageVersion = "1.0.0";
		szPackageDescription = __tr2qs_ctx("Enter a package description here...", "theme");
	}
	else
	{
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

		ThemeFunctions::getThemeHtmlDescription(
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
		    iIdx);

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

	setField("packageName", QVariant(szPackageName));
	setField("packageVersion", szPackageVersion);
	setField("packageDescription", szPackageDescription);
	setField("packageAuthor", szPackageAuthor);
}

PackThemeDataWidget::~PackThemeDataWidget()
    = default;

PackThemeInfoWidget::PackThemeInfoWidget(PackThemeDialog * pParent)
    : QWizardPage(pParent)
{
	setObjectName("theme_package_info_page");
	setTitle(__tr2qs_ctx("Package Information", "theme"));
	setSubTitle(__tr2qs_ctx("Here you need to provide information about you (the packager) and a short description of the package you're creating.", "theme"));

	QGridLayout * pLayout = new QGridLayout(this);

	QLabel * pLabel = new QLabel(this);
	pLabel->setText(__tr2qs_ctx("Package name:", "theme"));
	pLayout->addWidget(pLabel, 1, 0);

	m_pPackageNameEdit = new QLineEdit(this);

	pLabel->setBuddy(m_pPackageNameEdit);
	pLayout->addWidget(m_pPackageNameEdit, 1, 1);

	pLabel = new QLabel(this);
	pLabel->setText(__tr2qs_ctx("Version:", "theme"));
	pLayout->addWidget(pLabel, 2, 0);

	m_pPackageVersionEdit = new QLineEdit(this);

	pLabel->setBuddy(m_pPackageVersionEdit);
	pLayout->addWidget(m_pPackageVersionEdit, 2, 1);

	pLabel = new QLabel(this);
	pLabel->setText(__tr2qs_ctx("Description:", "theme"));
	pLayout->addWidget(pLabel, 3, 0);

	m_pPackageDescriptionEdit = new QTextEdit(this);

	pLabel->setBuddy(m_pPackageDescriptionEdit);
	pLayout->addWidget(m_pPackageDescriptionEdit, 3, 1);

	pLabel = new QLabel(this);
	pLabel->setText(__tr2qs_ctx("Package author:", "theme"));
	pLayout->addWidget(pLabel, 4, 0);

	m_pPackageAuthorEdit = new QLineEdit(this);

	pLabel->setBuddy(m_pPackageAuthorEdit);
	pLayout->addWidget(m_pPackageAuthorEdit, 4, 1);

	pLayout->setRowStretch(3, 1);
	pLayout->setColumnStretch(1, 1);

	// Store data in the fields
	registerField("packageName*", m_pPackageNameEdit);
	registerField("packageVersion*", m_pPackageVersionEdit);
	registerField("packageDescription*", m_pPackageDescriptionEdit);
	registerField("packageAuthor*", m_pPackageAuthorEdit);
}
void PackThemeInfoWidget::initializePage()
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

PackThemeInfoWidget::~PackThemeInfoWidget()
    = default;

PackThemeImageWidget::PackThemeImageWidget(PackThemeDialog * pParent)
    : QWizardPage(pParent)
{
	setObjectName("theme_package_image_page");
	setTitle(__tr2qs_ctx("Icon/Screenshot", "theme"));
	setSubTitle(__tr2qs_ctx("Here you can choose the image that will appear in the installation dialog for your theme package. It can be an icon, a logo or a screenshot and it should be not larger than 300x225. If you don't provide an image a simple default icon will be used at installation stage.", "theme"));

	QVBoxLayout * pLayout = new QVBoxLayout(this);

	m_pImageLabel = new QLabel(this);
	m_pImageLabel->setFrameStyle(QFrame::Sunken | QFrame::Panel);
	m_pImageLabel->setMinimumSize(300, 225);
	m_pImageLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
	pLayout->addWidget(m_pImageLabel);

	m_pImageSelector = new KviFileSelector(this, "", &m_szImagePath, true, 0, KVI_FILTER_IMAGE);
	connect(m_pImageSelector, SIGNAL(selectionChanged(const QString &)), this, SLOT(imageSelectionChanged(const QString &)));
	pLayout->addWidget(m_pImageSelector);
	// Store data in the fields
	registerField("packageImagePath*", m_pImageSelector);
}

PackThemeImageWidget::~PackThemeImageWidget()
    = default;

void PackThemeImageWidget::imageSelectionChanged(const QString & szImagePath)
{
	QImage pix(szImagePath);
	if(!pix.isNull())
	{
		QPixmap out;
		if(pix.width() > 300 || pix.height() > 225)
		{
			pix = pix.scaled(300, 225, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			out = out.fromImage(pix);
		}
		else
		{
			out = out.fromImage(pix);
		}
		m_pImageLabel->setPixmap(out);
		return;
	}

	QMessageBox::critical(this, __tr2qs_ctx("Export Theme - KVIrc", "theme"), __tr2qs_ctx("Failed to load the selected image!", "theme"),
	    QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);

	m_pImageSelector->setSelection("");
	m_pImageLabel->setPixmap(QPixmap());
}

PackThemeSaveWidget::PackThemeSaveWidget(PackThemeDialog * pParent)
    : QWizardPage(pParent)
{
	setObjectName("theme_package_save_page");
	setTitle(__tr2qs_ctx("Package Path", "theme"));
	setSubTitle(__tr2qs_ctx("Here you must choose the file name for the theme package. It should have a *%1 extension.", "theme").arg(KVI_FILEEXTENSION_THEMEPACKAGE));

	QVBoxLayout * pLayout = new QVBoxLayout(this);

	QString szFilter = "*";
	szFilter += KVI_FILEEXTENSION_THEMEPACKAGE;
	m_pSavePathSelector = new KviFileSelector(this, "", &m_szPackagePath, true, KviFileSelector::ChooseSaveFileName, szFilter);
	pLayout->addWidget(m_pSavePathSelector);

	QLabel * pLabel = new QLabel(this);
	pLabel->setWordWrap(true);
	pLabel->setText(__tr2qs_ctx("Finally hit the \"Finish\" button to complete the packaging operation.", "theme"));
	pLayout->addWidget(pLabel);

	// Store data in the fields
	registerField("packageSavePath*", m_pSavePathSelector);
}

PackThemeSaveWidget::~PackThemeSaveWidget()
    = default;

void PackThemeSaveWidget::initializePage()
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

PackThemeDialog::~PackThemeDialog()
    = default;

void PackThemeDialog::accept()
{
	if(!packTheme())
		return;
	QWizard::accept();
}

bool PackThemeDialog::packTheme()
{
	// Get data from registered fields
	m_szAuthor = field("packageAuthor").toString();
	m_szName = field("packageName").toString();
	m_szVersion = field("packageVersion").toString();
	m_szDescription = field("packageDescription").toString();
	m_szImagePath = field("packageImagePath").toString();
	m_szPackagePath = field("packageSavePath").toString();

	QString szError;

	if(!ThemeFunctions::packageThemes(
	       m_szPackagePath,
	       m_szName,
	       m_szVersion,
	       m_szDescription,
	       m_szAuthor,
	       m_szImagePath,
	       *m_pThemeInfoList,
	       szError))
	{
		QMessageBox::critical(this,
		    __tr2qs_ctx("Export Theme - KVIrc", "theme"),
		    szError,
		    QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
		return false;
	}

	QMessageBox::information(this,
	    __tr2qs_ctx("Exporting Theme - KVIrc", "theme"),
	    __tr2qs("Theme package saved successfully."),
	    QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);

	return true;
}
