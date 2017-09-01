//============================================================================
//
//   File : PackAddonDialog.cpp
//   Creation date : Sat 03 May 2008 01:40:44 by Elvio Basello
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2008-2009 Elvio Basello <hellvis69 at netsons dot org>
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
//============================================================================

#include "PackAddonDialog.h"
#include "AddonFunctions.h"

#include "KviLocale.h"
#include "KviFileUtils.h"
#include "KviApplication.h"
#include "KviIconManager.h"
#include "KviPackageWriter.h"
#include "kvi_fileextensions.h"
#include "kvi_sourcesdate.h"
#include "KviOptions.h"

#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>
#include <QDir>
#include <QDirIterator>

PackAddonDialog::PackAddonDialog(QWidget * pParent)
    : QWizard(pParent)
{
	setMinimumSize(400, 350);
	setObjectName("addon_package_wizard");
	setOption(QWizard::IndependentPages, true);
	setWindowTitle(__tr2qs_ctx("Create Addon Package - KVIrc", "addon"));

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
	pPage->setTitle(__tr2qs_ctx("Welcome", "addon"));

	QLabel * pLabel = new QLabel(pPage);
	pLabel->setWordWrap(true);
	pLabel->setText(__tr2qs_ctx("This procedure allows you to export the selected addon to a single package.<br>It is useful when you want to distribute your addon to the public.", "addon"));
	pLayout->addWidget(pLabel);

	pLabel = new QLabel(pPage);
	pLabel->setWordWrap(true);
	QString szText;
	szText += __tr2qs_ctx("You will be asked to provide some information like the package name, the version, a description and so on.", "addon");
	szText += "<br><br>";
	szText += __tr2qs_ctx("Hit the \"Next\" button to begin.", "addon");
	pLabel->setText(szText);
	pLayout->addWidget(pLabel);

	pPage->setMinimumWidth(350);

	addPage(pPage);

	// Packager information
	m_pPackAddonInfoPackageWidget = new PackAddonInfoPackageWidget(this);
	addPage(m_pPackAddonInfoPackageWidget);

	// File selection
	m_pPackAddonFileSelectionWidget = new PackAddonFileSelectionWidget(this);
	addPage(m_pPackAddonFileSelectionWidget);

	// Save selection
	m_pPackAddonSaveSelectionWidget = new PackAddonSaveSelectionWidget(this);
	addPage(m_pPackAddonSaveSelectionWidget);

	// Summary info
	m_pPackAddonSummaryInfoWidget = new PackAddonSummaryInfoWidget(this);
	addPage(m_pPackAddonSummaryInfoWidget);
}

PackAddonDialog::~PackAddonDialog()
    = default;

void PackAddonDialog::accept()
{
	if(!packAddon())
		return;
	QWizard::accept();
}

bool PackAddonDialog::packAddon()
{
	// Get data from registered fields
	AddonInfo info;
	info.szAuthor = field("packageAuthor").toString();
	info.szName = field("packageName").toString();
	info.szVersion = field("packageVersion").toString();
	info.szDescription = field("packageDescription").toString();
	info.szMinVersion = field("packageMinVersion").toString();
	info.szImage = field("packageImage").toString();
	info.szDirPath = field("packageDirPath").toString();
	info.szSavePath = field("packageSavePath").toString();

	if(info.szSavePath.isEmpty())
	{
		info.szSavePath = QDir::homePath();
		KviQString::ensureLastCharIs(info.szSavePath, QChar(KVI_PATH_SEPARATOR_CHAR));
		info.szSavePath += m_szName;
		info.szSavePath += "-";
		info.szSavePath += m_szVersion;
		info.szSavePath += KVI_FILEEXTENSION_ADDONPACKAGE;
	}

	if(QFile::exists(info.szSavePath))
	{
		if(QMessageBox::question(
		       this,
		       __tr2qs_ctx("Exporting Addon Confirmation - KVIrc", "addon"),
		       __tr2qs_ctx("File %1 already exists. Do you want to overwrite it?", "addon").arg(info.szSavePath),
		       QMessageBox::Yes,
		       QMessageBox::No)
		    == QMessageBox::No)
			return false;
	}

	// Raise the files summary dialog
	m_pPackAddonSummaryFilesWidget = new PackAddonSummaryFilesWidget(this);
	m_pPackAddonSummaryFilesWidget->setPath(info.szDirPath);
	if(m_pPackAddonSummaryFilesWidget->exec() == QDialog::Rejected)
	{
		delete m_pPackAddonSummaryFilesWidget;
		return false;
	}

	QString szError;

	if(!AddonFunctions::pack(info, szError))
	{
		QMessageBox::critical(this,
		    __tr2qs_ctx("Addon Packaging - KVIrc", "addon"),
		    szError, QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
		return false;
	}

	QMessageBox::information(this, __tr2qs_ctx("Exporting Addon Completed - KVIrc", "addon"), __tr2qs_ctx("The package was saved successfully in %1", "addon").arg(info.szSavePath), QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);

	return true;
}

PackAddonInfoPackageWidget::PackAddonInfoPackageWidget(PackAddonDialog * pParent)
    : QWizardPage(pParent)
{
	setObjectName("addon_package_info_page");
	setTitle(__tr2qs_ctx("Package Information", "addon"));
	setSubTitle(__tr2qs_ctx("Here you need to provide information about you (the packager) and a short description of the package you're creating.", "addon"));

	QGridLayout * pLayout = new QGridLayout(this);

	QLabel * pLabel = new QLabel(this);
	pLabel->setText(__tr2qs_ctx("Package &author:", "addon"));
	pLayout->addWidget(pLabel, 0, 0);

	m_pPackageAuthorEdit = new QLineEdit(this);
	pLabel->setBuddy(m_pPackageAuthorEdit);
	pLayout->addWidget(m_pPackageAuthorEdit, 0, 1);

	pLabel = new QLabel(this);
	pLabel->setText(__tr2qs_ctx("Package &name:", "addon"));
	pLayout->addWidget(pLabel, 1, 0);

	m_pPackageNameEdit = new QLineEdit(this);
	m_pPackageNameEdit->setPlaceholderText(__tr2qs_ctx("No spaces allowed", "addon"));
	pLabel->setBuddy(m_pPackageNameEdit);
	pLayout->addWidget(m_pPackageNameEdit, 1, 1);

	pLabel = new QLabel(this);
	pLabel->setText(__tr2qs_ctx("Package &version:", "addon"));
	pLayout->addWidget(pLabel, 2, 0);

	m_pPackageVersionEdit = new QLineEdit(this);
	m_pPackageVersionEdit->setPlaceholderText("1.0");
	pLabel->setBuddy(m_pPackageVersionEdit);
	pLayout->addWidget(m_pPackageVersionEdit, 2, 1);

	pLabel = new QLabel(this);
	pLabel->setText(__tr2qs_ctx("Package &description:", "addon"));
	pLayout->addWidget(pLabel, 3, 0);

	m_pPackageDescriptionEdit = new QLineEdit(this);
	m_pPackageDescriptionEdit->setText(QString());
	pLabel->setBuddy(m_pPackageDescriptionEdit);
	pLayout->addWidget(m_pPackageDescriptionEdit, 3, 1);

	pLabel = new QLabel(this);
	pLabel->setText(__tr2qs_ctx("Minimum &KVIrc version:", "addon"));
	pLayout->addWidget(pLabel, 4, 0);

	m_pPackageMinVersionEdit = new QLineEdit(this);
	m_pPackageMinVersionEdit->setText(KVI_VERSION);
	pLabel->setBuddy(m_pPackageMinVersionEdit);
	pLayout->addWidget(m_pPackageMinVersionEdit, 4, 1);

	// Store data in the fields
	registerField("packageAuthor*", m_pPackageAuthorEdit);
	registerField("packageName*", m_pPackageNameEdit);
	registerField("packageVersion*", m_pPackageVersionEdit);
	registerField("packageDescription*", m_pPackageDescriptionEdit);
	registerField("packageMinVersion", m_pPackageMinVersionEdit);
}

PackAddonInfoPackageWidget::~PackAddonInfoPackageWidget()
    = default;

PackAddonFileSelectionWidget::PackAddonFileSelectionWidget(PackAddonDialog * pParent)
    : QWizardPage(pParent)
{
	setObjectName("addon_package_file_page");
	setTitle(__tr2qs_ctx("Package Files", "addon"));
	setSubTitle(__tr2qs_ctx("Here you need to select the directory where the addon files to be packed are located.", "addon"));

	QVBoxLayout * pLayout = new QVBoxLayout(this);

	// Select source directory
	m_pDirPathSelector = new KviDirectorySelector(this, __tr2qs_ctx("Select the source directory:", "addon"), &m_szDirPath, true, KviFileSelector::VerticalLayout);
	pLayout->addWidget(m_pDirPathSelector);

	// Select addon icon
	m_pPackageImageEdit = new KviFileSelector(this, __tr2qs_ctx("Select the image file:", "addon"), &m_szPackageImage, true, KviFileSelector::VerticalLayout, KVI_FILTER_IMAGE);
	pLayout->addWidget(m_pPackageImageEdit);

	// Store data in the fields
	registerField("packageDirPath*", m_pDirPathSelector);
	registerField("packageImage*", m_pPackageImageEdit);
}

PackAddonFileSelectionWidget::~PackAddonFileSelectionWidget()
    = default;

PackAddonSaveSelectionWidget::PackAddonSaveSelectionWidget(PackAddonDialog * pParent)
    : QWizardPage(pParent)
{
	setObjectName("addon_package_save_page");
	setTitle(__tr2qs_ctx("Save Package", "addon"));
	setSubTitle(__tr2qs_ctx("Here you need to provide the path where to save the created addon package", "addon"));

	QVBoxLayout * pLayout = new QVBoxLayout(this);

	// Select save path
	m_pSavePathSelector = new KviFileSelector(
	    this,
	    __tr2qs_ctx("Select save path:", "addon"),
	    &m_szFilePath,
	    true,
	    KviFileSelector::ChooseSaveFileName | KviFileSelector::VerticalLayout,
	    KVI_FILTER_ADDON);
	pLayout->addWidget(m_pSavePathSelector);

	// Store data in the fields
	registerField("packageSavePath*", m_pSavePathSelector);
}

PackAddonSaveSelectionWidget::~PackAddonSaveSelectionWidget()
    = default;

void PackAddonSaveSelectionWidget::initializePage()
{
	// Get data from registered fields
	QString szName = field("packageName").toString();
	QString szVersion = field("packageVersion").toString();

	// Create addon name
	QString szSavePath = QDir::homePath();
	KviQString::ensureLastCharIs(szSavePath, QChar(KVI_PATH_SEPARATOR_CHAR));
	szSavePath += szName;
	szSavePath += "-";
	szSavePath += szVersion;
	szSavePath += KVI_FILEEXTENSION_ADDONPACKAGE;
	m_pSavePathSelector->setTmpFile(szSavePath);
}

PackAddonSummaryInfoWidget::PackAddonSummaryInfoWidget(PackAddonDialog * pParent)
    : QWizardPage(pParent)
{
	setObjectName("addon_package_summary_info_page");
	setTitle(__tr2qs_ctx("Final Information", "addon"));
	setSubTitle(__tr2qs_ctx("Here are the details you provided. If this information is correct, hit the \"Finish\" button to complete the packaging operations.", "addon"));

	QVBoxLayout * pLayout = new QVBoxLayout(this);
	m_pLabelInfo = new QLabel(this);
	pLayout->addWidget(m_pLabelInfo);
}

PackAddonSummaryInfoWidget::~PackAddonSummaryInfoWidget()
    = default;

void PackAddonSummaryInfoWidget::initializePage()
{
	// Get data from registered fields
	QString szAuthor = field("packageAuthor").toString();
	QString szName = field("packageName").toString();
	QString szVersion = field("packageVersion").toString();
	QString szDescription = field("packageDescription").toString();
	QString szMinVersion = field("packageMinVersion").toString();
	QString szImage = field("packageImage").toString();
	QString szDirPath = field("packageDirPath").toString();
	QString szSavePath = field("packageSavePath").toString();

	QString szText = __tr2qs_ctx("This is what I will check for:", "addon");
	szText += "<br><br><b>";
	szText += __tr2qs_ctx("Package author", "addon");
	szText += ":</b> ";
	szText += szAuthor;
	szText += "<br><b>";
	szText += __tr2qs_ctx("Package name", "addon");
	szText += ":</b> ";
	szText += szName;
	szText += "<br><b>";
	szText += __tr2qs_ctx("Package version", "addon");
	szText += ":</b> ";
	szText += szVersion;
	szText += "<br><b>";
	szText += __tr2qs_ctx("Package description", "addon");
	szText += ":</b> ";
	szText += szDescription;
	szText += "<br><b>";
	szText += __tr2qs_ctx("Minimum KVIrc version", "addon");
	szText += ":</b> ";
	szText += szMinVersion;
	szText += "<br><b>";
	szText += __tr2qs_ctx("Image file", "addon");
	szText += ":</b> ";
	szText += szImage;
	szText += "<br><b>";
	szText += __tr2qs_ctx("Source directory", "addon");
	szText += ":</b> ";
	szText += szDirPath;
	szText += "<br><b>";
	szText += __tr2qs_ctx("Save path", "addon");
	szText += ":</b> ";
	szText += szSavePath;

	m_pLabelInfo->setText(szText);
}

PackAddonSummaryFilesWidget::PackAddonSummaryFilesWidget(PackAddonDialog * pParent)
    : QDialog(pParent)
{
	setObjectName("addon_package_summary_file_dialog");
	setWindowTitle(__tr2qs_ctx("File Summary Review - KVIrc", "addon"));
	setWindowModality(Qt::WindowModal);
	setModal(true);

	QVBoxLayout * pLayout = new QVBoxLayout(this);

	QLabel * pLabel = new QLabel(this);
	pLabel->setText(__tr2qs_ctx("Here are the files found in the directories you provided.\nIf the files listed below are correct, hit the \"Finish\" button to complete the packaging operation.", "addon"));
	pLayout->addWidget(pLabel);

	m_pFiles = new QTextEdit(this);
	m_pFiles->setReadOnly(true);
	pLayout->addWidget(m_pFiles);

	KviTalHBox * pBox = new KviTalHBox(this);
	QPushButton * pCancel = new QPushButton(pBox);
	pCancel->setText(__tr2qs_ctx("Cancel", "addon"));
	connect(pCancel, SIGNAL(clicked()), this, SLOT(reject()));

	QPushButton * pAccept = new QPushButton(pBox);
	pAccept->setText(__tr2qs_ctx("Finish", "addon"));
	connect(pAccept, SIGNAL(clicked()), this, SLOT(accept()));
	pLayout->addWidget(pBox);
}

PackAddonSummaryFilesWidget::~PackAddonSummaryFilesWidget()
    = default;

void PackAddonSummaryFilesWidget::accept()
{
	QDialog::accept();
}

void PackAddonSummaryFilesWidget::reject()
{
	QDialog::reject();
}

void PackAddonSummaryFilesWidget::showEvent(QShowEvent *)
{
	QStringList list;

	QDirIterator it(m_szPath, QDir::AllEntries | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

	// Iterate through the addon dir and its subdirs
	while(it.hasNext())
	{
		QString szStr = it.next();
		if(!szStr.contains("/."))
			list.append(szStr);
	}

	// QDirIterator does not support sorting, so do it manually
	list.sort();

	m_pFiles->setPlainText(list.join("\n"));
}
