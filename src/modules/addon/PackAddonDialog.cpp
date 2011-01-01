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
#include <QDateTime>
#include <QBuffer>


PackAddonDialog::PackAddonDialog(QWidget * pParent)
: QWizard(pParent)
{
	setMinimumSize(400,350);
	setObjectName("addon_package_wizard");
	setOption(QWizard::IndependentPages,true);
	setWindowTitle(__tr2qs_ctx("Create Addon Package - KVIrc","addon"));

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
	pPage->setTitle(__tr2qs_ctx("Welcome","addon"));

	QLabel * pLabel = new QLabel(pPage);
	pLabel->setWordWrap(true);
	pLabel->setText(__tr2qs_ctx("This procedure allows you to export the selected addon to a single package. It is useful when you want to distribute your addon to the public.","addon"));
	pLayout->addWidget(pLabel);

	pLabel = new QLabel(pPage);
	pLabel->setWordWrap(true);
	QString szText;
	szText += __tr2qs_ctx("You will be asked to provide some information like the package name, the version, a description and so on.","addon");
	szText += "<br><br>";
	szText += __tr2qs_ctx("Hit the \"Next\" button to begin.","addon");
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
{
}

void PackAddonDialog::accept()
{
	if(!packAddon())
		return;
	QWizard::accept();
}

bool PackAddonDialog::checkDirTree(QString * pszError, QString * pszWarning)
{
	if(pszError) *pszError = "";
	if(pszWarning) *pszWarning = "";

	QDir addon(m_szDirPath);
	if(!addon.exists())
	{
		*pszError = __tr2qs_ctx("The selected directory does not exist.","addon");
		return false;
	}

	QFileInfo init(m_szDirPath + "/install.kvs");
	if(!init.exists())
	{
		*pszError = __tr2qs_ctx("The initialization script (install.kvs) does not exist.","addon");
		return false;
	}

	return true;
}

#if 0
bool PackAddonDialog::createInstaller(QString * pszError)
{
	if(pszError)
		*pszError = "";

	// Start creating install.kvs: header
	QString szTmp;
	szTmp += QString("# This file is generated automatically. Do NOT touch unless you know what are you doing\n#\n");
	szTmp += QString("# %1 %2\n# Written by %3\n# %4\n\n").arg(m_szName, m_szVersion, m_szAuthor, m_szDescription);
	szTmp += "# Register the script: this must be the first instruction executed\n# since it will abort with an error when a greater version is already installed\n";

	// install.kvs: addon registration
	/* FIXME: re-add in KVIrc 4.1!
	 * implement a backend with full automatic cleaning for aliases, classes, popups,
	 * actions, ... via snapshot/diff of kvirc state before/after installing a package

	szTmp += QString("addon.register(\"%1\",\"%2\",\"%1\",\"%3\",\"%4\",\"%5\")\n").arg(m_szName, m_szVersion, m_szDescription, m_szMinVersion, m_szIcon);
	szTmp += "{\n";
	szTmp += "\t# This is our uninstall callback: it will be called by KVIrc when addon.uninstall is invoked\n";
	szTmp += QString("\t%1::uninstall::uninstall\n").arg(m_szName);
	szTmp += QString("\t%1::uninstall::uninstallfiles\n").arg(m_szName);
	szTmp += QString("\t%1::uninstall::uninstallaliases\n").arg(m_szName);
	szTmp += "}\n\n";
	szTmp += "# Ok, addon.register succeeded. We can go on with the installation.\n\n";
	*/

	// install.kvs: run path
	szTmp += "# Get the path that this script was launched from\n";
	szTmp += "%mypath = $file.extractPath($0)\n\n";

	// install.kvs: create an instance of the installer class
	szTmp += "# The installer will copy our files and generate automatically an uninstallation\n# alias for them\n";
	szTmp += "%installer = $new(installer,0,myaddon)\n\n";

	// install.kvs: copy files
	szTmp += "# Copy files in each subdirectory\n# the pics\n";

	szTmp += QString("%installer->$copyFiles(\"%mypath/pics\",\"*.png\",$file.localdir(\"pics\"))\n\n");
	szTmp += "# the translations\n";
	szTmp += "%installer->$copyFiles(\"%mypath/locale\",\"*.mo\",$file.localdir(\"locale\"))\n\n";

	szTmp += "# the documentation\n";
	szTmp += QString("%installer->$copyFiles(\"%mypath/help/\",\"*.html\",$file.localdir(\"help/\"))\n\n");

	// install.kvs: generate uninstall alias
	szTmp += "# Generate the uninstall alias\n";
	szTmp += QString("%installer->$generateUninstallAlias(\"%1::uninstallfiles\")\n\n") \
		.arg(m_szName);

	// kviinstall.kvs: kill the installer class
	szTmp += "# Kill the installer helper\n";
	szTmp += "delete %installer\n\n";

#if 0
	// install.kvs: fetch the complete script
	szTmp += "# Fetch the complete script\n";
	szTmp += "%files[] = $file.ls($file.extractpath($0)/src,f)\n";
	szTmp += QString("%alias = \"alias(%1::uninstall::uninstallaliases){$lf\"\n") \
		.arg(m_szName);
	szTmp += "for(%i=0; %i<$length(%files[]); %i++)\n{\n";
        szTmp += "\tinclude $file.extractpath($0)/src/%files[%i]\n";
        szTmp += "\t%flt = $str.left(%files[%i],$($str.len(%files[%i])-4))\n";
        szTmp += "\t%file = $str.replace(%flt,\"_\",\"::\")\n";
        szTmp += "\t%alias .= \"alias(%file){};$lf\"\n}\n";
        szTmp += QString("\t%alias .= \"alias(%1::uninstall::uninstallfiles){};$lf\"\n") \
		.arg(m_szName);
        szTmp += QString("\t%alias .= \"alias(%1::uninstall::uninstallaliases){};$lf\"\n") \
                .arg(m_szName);
	szTmp += "%alias .= \"}\"\n";
	szTmp += "eval %alias\n\n";
#endif

	// install.kvs: include initialization file
	szTmp += "# Include initialization file\n";
	szTmp += "include \"init.kvs\" %mypath\n\n";

	// Put the install.kvs in the buffer
	QByteArray buffer;
	buffer.append(szTmp.toUtf8());

	// We don't need to check dir existance since it was checked just before
	QDir addon(m_szDirPath);

	// Open file for writing
	QFile installer(addon.filePath("install.kvs"));
	if(!installer.open(QIODevice::WriteOnly))
	{
		*pszError = __tr2qs_ctx("Cannot open file for writing.","addon");
		return false;
	}

	// Write the buffer to the file descriptor
	if(installer.write(buffer) == -1)
	{
		*pszError = __tr2qs_ctx("Cannot write to the file descriptor.","addon");
		return false;
	}

	return true;
}
#endif


bool PackAddonDialog::packAddon()
{
	// Get data from registered fields
	m_szAuthor = field("packageAuthor").toString();
	m_szName = field("packageName").toString();
	m_szVersion = field("packageVersion").toString();
	m_szDescription = field("packageDescription").toString();
	m_szMinVersion = field("packageMinVersion").toString();
	m_szIcon = field("packageIcon").toString();
	m_szDirPath = field("packageDirPath").toString();
	m_szSavePath = field("packageSavePath").toString();

	// Check the addon tree
	QString szError, szWarning;
	
	if(!checkDirTree(&szError,&szWarning))
	{
		QMessageBox::critical(this,
			__tr2qs_ctx("Addon Packaging Error","addon"),
			szError,QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton
		);
		return false;
	}
	
	if(szWarning != "")
	{
		QMessageBox::warning(this,
			__tr2qs_ctx("Addon Packaging Warning","addon"),
			szWarning,QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton
		);
	}
	
	// Raise the files summary dialog
	m_pPackAddonSummaryFilesWidget = new PackAddonSummaryFilesWidget(this);
	m_pPackAddonSummaryFilesWidget->setPath(m_szDirPath);
	if(m_pPackAddonSummaryFilesWidget->exec() == QDialog::Rejected)
	{
		delete m_pPackAddonSummaryFilesWidget;
		return false;
	}


	QString szTmp;
	szTmp = QDateTime::currentDateTime().toString();

	KviPackageWriter pw;
	pw.addInfoField("PackageType","AddonPack");
	pw.addInfoField("AddonPackVersion",KVI_CURRENT_ADDONS_ENGINE_VERSION);
	pw.addInfoField("Name",m_szName);
	pw.addInfoField("Version",m_szVersion);
	pw.addInfoField("Author",m_szAuthor);
	pw.addInfoField("Description",m_szDescription);
	pw.addInfoField("Date",szTmp);
	pw.addInfoField("Application","KVIrc " KVI_VERSION "." KVI_SOURCES_DATE);

	QPixmap pix(m_szIcon);
	if(!pix.isNull())
	{
		QByteArray * pba = new QByteArray();
		QBuffer bufferz(pba,0);

		bufferz.open(QIODevice::WriteOnly);
		pix.save(&bufferz,"PNG");
		bufferz.close();
		pw.addInfoField("Image",pba);
	}

	QDir dir(m_szDirPath);
	QFileInfoList ls = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::Readable | QDir::NoSymLinks | QDir::NoDotAndDotDot);

	if(ls.isEmpty())
	{
		szTmp = __tr2qs_ctx("Packaging failed","addon");
		szTmp += ": ";
		szTmp += __tr2qs_ctx("The package file list is empty","addon");
		QMessageBox::critical(this,__tr2qs_ctx("Export Addon - KVIrc","addon"),szTmp,QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return false;
	}

	for(QFileInfoList::Iterator it = ls.begin();it != ls.end();++it)
	{
		const QFileInfo &inf = *it;

		if(inf.isDir())
		{
			if(!pw.addDirectory(inf.absoluteFilePath(),QString("%1/").arg(inf.fileName())))
			{
				QString szTmp = __tr2qs_ctx("Packaging failed","addon");
				szTmp += ": ";
				szTmp += pw.lastError();
				QMessageBox::critical(this,__tr2qs_ctx("Export Addon - KVIrc","addon"),szTmp,QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
				return false;
			}

			continue;
		}

		// must be a file
		if(!pw.addFile(inf.absoluteFilePath(),inf.fileName()))
		{
			szTmp = __tr2qs_ctx("Packaging failed","addon");
			szTmp += ": ";
			szTmp += pw.lastError();
			QMessageBox::critical(this,__tr2qs_ctx("Export Addon - KVIrc","addon"),szTmp,QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
			return false;
		}
	}


	// Create the addon package
	if(m_szSavePath.isEmpty())
	{
		m_szSavePath = QDir::homePath();
		KviQString::ensureLastCharIs(m_szSavePath,QChar(KVI_PATH_SEPARATOR_CHAR));
		m_szSavePath += m_szName;
		m_szSavePath += "-";
		m_szSavePath += m_szVersion;
		m_szSavePath += KVI_FILEEXTENSION_ADDONPACKAGE;
	}

	if(!pw.pack(m_szSavePath))
	{
		szTmp = __tr2qs_ctx("Packaging failed","addon");
		szTmp += ": ";
		szTmp += pw.lastError();
		QMessageBox::critical(this,__tr2qs_ctx("Export Addon - KVIrc","addon"),szTmp,QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return false;
	}


	QMessageBox::information(this,__tr2qs_ctx("Export Addon - KVIrc","addon"),__tr2qs("Package saved successfully in ") + m_szSavePath,QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);

	return true;
}


PackAddonInfoPackageWidget::PackAddonInfoPackageWidget(PackAddonDialog * pParent)
: QWizardPage(pParent)
{
	setObjectName("addon_package_info_page");
	setTitle(__tr2qs_ctx("Package Information","addon"));
	setSubTitle(__tr2qs_ctx("Here you need to provide information about you (the packager) and a short description of the package you're creating.","addon"));

	QGridLayout * pLayout = new QGridLayout(this);

	QLabel * pLabel = new QLabel(this);
	pLabel->setText(__tr2qs_ctx("Package &Author:","addon"));
	pLayout->addWidget(pLabel,0,0);
	
	m_pPackageAuthorEdit = new QLineEdit(this);
	m_pPackageAuthorEdit->setText(__tr2qs_ctx("Your name here","addon"));
	pLabel->setBuddy(m_pPackageAuthorEdit);
	pLayout->addWidget(m_pPackageAuthorEdit,0,1);

	pLabel = new QLabel(this);
	pLabel->setText(__tr2qs_ctx("Package &Name:","addon"));
	pLayout->addWidget(pLabel,1,0);
	
	m_pPackageNameEdit = new QLineEdit(this);
	m_pPackageNameEdit->setText(__tr2qs_ctx("No spaces allowed, like MyAddon","addon"));
	pLabel->setBuddy(m_pPackageNameEdit);
	pLayout->addWidget(m_pPackageNameEdit,1,1);

	pLabel = new QLabel(this);
	pLabel->setText(__tr2qs_ctx("Package &Version:","addon"));
	pLayout->addWidget(pLabel,2,0);
	
	m_pPackageVersionEdit = new QLineEdit(this);
	m_pPackageVersionEdit->setText(__tr2qs_ctx("Version in the form x.y[.z], like 1.0 or 2.0.3","addon"));
	pLabel->setBuddy(m_pPackageVersionEdit);
	pLayout->addWidget(m_pPackageVersionEdit,2,1);

	pLabel = new QLabel(this);
	pLabel->setText(__tr2qs_ctx("Package &Description:","addon"));
	pLayout->addWidget(pLabel,3,0);

	m_pPackageDescriptionEdit = new QLineEdit(this);
	m_pPackageDescriptionEdit->setText(QString());
	pLabel->setBuddy(m_pPackageDescriptionEdit);
	pLayout->addWidget(m_pPackageDescriptionEdit,3,1);

	pLabel = new QLabel(this);
	pLabel->setText(__tr2qs_ctx("Minimum &KVIrc Version:","addon"));
	pLayout->addWidget(pLabel,4,0);

	m_pPackageMinVersionEdit = new QLineEdit(this);
	m_pPackageMinVersionEdit->setText(KVI_VERSION);
	pLabel->setBuddy(m_pPackageMinVersionEdit);
	pLayout->addWidget(m_pPackageMinVersionEdit,4,1);

	// Store data in the fields
	registerField("packageAuthor*",m_pPackageAuthorEdit);
	registerField("packageName*",m_pPackageNameEdit);
	registerField("packageVersion*",m_pPackageVersionEdit);
	registerField("packageDescription*",m_pPackageDescriptionEdit);
	registerField("packageMinVersion",m_pPackageMinVersionEdit);
}

PackAddonInfoPackageWidget::~PackAddonInfoPackageWidget()
{
}


PackAddonFileSelectionWidget::PackAddonFileSelectionWidget(PackAddonDialog * pParent)
: QWizardPage(pParent)
{
	setObjectName("addon_package_file_page");
	setTitle(__tr2qs_ctx("Package Files","addon"));
	setSubTitle(__tr2qs_ctx("Here you need to select the directory where the addon files are.","addon"));

	QVBoxLayout * pLayout = new QVBoxLayout(this);

	// Select source directory
	m_pDirPathSelector = new KviDirectorySelector(this,__tr2qs_ctx("Select the source directory:","addon"),&m_szDirPath,true,KviFileSelector::VerticalLayout);
	pLayout->addWidget(m_pDirPathSelector);

	// Select addon icon
	m_pPackageIconEdit = new KviFileSelector(this,__tr2qs_ctx("Select the icon file:","addon"),&m_szPackageIcon,true,KviFileSelector::VerticalLayout,KVI_FILTER_IMAGE);
	pLayout->addWidget(m_pPackageIconEdit);

	// Store data in the fields
	registerField("packageDirPath*",m_pDirPathSelector);
	registerField("packageIcon*",m_pPackageIconEdit);
}

PackAddonFileSelectionWidget::~PackAddonFileSelectionWidget()
{
}


PackAddonSaveSelectionWidget::PackAddonSaveSelectionWidget(PackAddonDialog * pParent)
: QWizardPage(pParent)
{
	setObjectName("addon_package_save_page");
	setTitle(__tr2qs_ctx("Save Package","addon"));
	setSubTitle(__tr2qs_ctx("Here you need to provide the path where to save the addon package","addon"));

	QVBoxLayout * pLayout = new QVBoxLayout(this);

	// Select save path
	m_pSavePathSelector = new KviFileSelector(
			this,
			__tr2qs_ctx("Select save path:","addon"),
			&m_szFilePath,
			true,
			KviFileSelector::ChooseSaveFileName | KviFileSelector::VerticalLayout,
			KVI_FILTER_ADDON
		);
	pLayout->addWidget(m_pSavePathSelector);

	// Store data in the fields
	registerField("packageSavePath*",m_pSavePathSelector);
}

PackAddonSaveSelectionWidget::~PackAddonSaveSelectionWidget()
{
}

void PackAddonSaveSelectionWidget::initializePage()
{
	// Get data from registered fields
	QString szName = field("packageName").toString();
	QString szVersion = field("packageVersion").toString();

	// Create addon name
	QString szSavePath = QDir::homePath();
	KviQString::ensureLastCharIs(szSavePath,QChar(KVI_PATH_SEPARATOR_CHAR));
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
	setTitle(__tr2qs_ctx("Final Information","addon"));
	setSubTitle(__tr2qs_ctx("Here there are the information you provided. If these information are correct, hit the \"Finish\" button to complete the packaging operations.","addon"));

	QVBoxLayout * pLayout = new QVBoxLayout(this);
	m_pLabelInfo = new QLabel(this);
	pLayout->addWidget(m_pLabelInfo);
}

PackAddonSummaryInfoWidget::~PackAddonSummaryInfoWidget()
{
}

void PackAddonSummaryInfoWidget::initializePage()
{
	// Get data from registered fields
	QString szAuthor = field("packageAuthor").toString();
	QString szName = field("packageName").toString();
	QString szVersion = field("packageVersion").toString();
	QString szDescription = field("packageDescription").toString();
	QString szMinVersion = field("packageMinVersion").toString();
	QString szIcon = field("packageIcon").toString();
	QString szDirPath = field("packageDirPath").toString();
	QString szSavePath = field("packageSavePath").toString();

	QString szText = __tr2qs_ctx("This is what I will check for:","addon");
	szText += "<br><br><b>";
	szText += __tr2qs_ctx("Package Author","addon");
	szText += ":</b> ";
	szText += szAuthor;
	szText += "<br><b>";
	szText += __tr2qs_ctx("Package Name","addon");
	szText += ":</b> ";
	szText += szName;
	szText += "<br><b>";
	szText += __tr2qs_ctx("Package Version","addon");
	szText += ":</b> ";
	szText += szVersion;
	szText += "<br><b>";
	szText += __tr2qs_ctx("Package Description","addon");
	szText += ":</b> ";
	szText += szDescription;
	szText += "<br><b>";
	szText += __tr2qs_ctx("Minimum KVIrc Version","addon");
	szText += ":</b> ";
	szText += szMinVersion;
	szText += "<br><b>";
	szText += __tr2qs_ctx("Icon File","addon");
	szText += ":</b> ";
	szText += szIcon;
	szText += "<br><b>";
	szText += __tr2qs_ctx("Source Directory","addon");
	szText += ":</b> ";
	szText += szDirPath;
	szText += "<br><b>";
	szText += __tr2qs_ctx("Save Path","addon");
	szText += ":</b> ";
	szText += szSavePath;

	m_pLabelInfo->setText(szText);
}


PackAddonSummaryFilesWidget::PackAddonSummaryFilesWidget(PackAddonDialog * pParent)
: QDialog(pParent)
{
	setObjectName("addon_package_summary_file_dialog");
	setWindowTitle(__tr2qs_ctx("File Summary","addon"));
	setWindowModality(Qt::WindowModal);
	setModal(true);

	QVBoxLayout * pLayout = new QVBoxLayout(this);

	QLabel * pLabel = new QLabel(this);
	pLabel->setText(__tr2qs_ctx("Here there are the files I found in the directories you provided.\nIf these and the information showed in the previous page are correct, hit the \"Finish\" button to complete\nthe packaging operations.","addon"));
	pLayout->addWidget(pLabel);

	m_pFiles = new QTextEdit(this);
	m_pFiles->setReadOnly(true);
	pLayout->addWidget(m_pFiles);

	KviTalHBox * pBox = new KviTalHBox(this);
	QPushButton * pCancel = new QPushButton(pBox);
	pCancel->setText(__tr2qs_ctx("Cancel","addon"));
	connect(pCancel,SIGNAL(clicked()),this,SLOT(reject()));

	QPushButton * pAccept = new QPushButton(pBox);
	pAccept->setText(__tr2qs_ctx("Finish","addon"));
	connect(pAccept,SIGNAL(clicked()),this,SLOT(accept()));
	pLayout->addWidget(pBox);
}

PackAddonSummaryFilesWidget::~PackAddonSummaryFilesWidget()
{
}

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

	QDirIterator it(m_szPath,QDir::AllEntries | QDir::NoDotAndDotDot,QDirIterator::Subdirectories);

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
