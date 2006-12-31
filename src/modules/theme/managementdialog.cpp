//=============================================================================
//
//   File : managementdialog.cpp
//   Created on Sat 30 Dec 2006 14:54:56 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2006 Szymon Stefanek <pragma at kvirc dot net>
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
//=============================================================================

#define LVI_ICON_SIZE 32
#define LVI_BORDER 4
#define LVI_SPACING 8
#define LVI_MINIMUM_TEXT_WIDTH 300
#define LVI_MINIMUM_CELL_WIDTH (LVI_MINIMUM_TEXT_WIDTH + LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING + LVI_BORDER)

#include "managementdialog.h"
#include "kvi_xmladdonloader.h"
#include "kvi_iconmanager.h"
#include "kvi_msgbox.h"

#include <qlayout.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qregexp.h>
#include <qmessagebox.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qdatetime.h>
#include <qfiledialog.h>
#include <qpopupmenu.h>
#include <qcombobox.h>
#include <qpainter.h>
#include <qtooltip.h>
#include <qimage.h>
#include <qtextedit.h>
#include <qmultilineedit.h>
#include <qbuffer.h>

#include "kvi_options.h"
#include "kvi_locale.h"
#include "kvi_config.h"
#include "kvi_fileutils.h"
#include "kvi_app.h"
#include "kvi_frame.h"
#include "kvi_iconmanager.h"
#include "kvi_internalcmd.h"
#include "kvi_styled_controls.h"
#include "kvi_doublebuffer.h"
#include "kvi_packagefile.h"
#include "kvi_fileextensions.h"
#include "kvi_filedialog.h"

#include <stdlib.h> // rand & srand

extern QRect g_rectManagementDialogGeometry;

KviSaveThemeDialog::KviSaveThemeDialog(QWidget * par)
: QDialog(par)
{
	setCaption(__tr2qs_ctx("Save Current Theme","theme"));

	QGridLayout * g = new QGridLayout(this,8,2,4,4);

	QLabel * l;

	l = new QLabel(__tr2qs_ctx("Theme name:","theme"),this);
	g->addWidget(l,0,0);

	m_pThemeNameEdit = new QLineEdit(this);
	g->addMultiCellWidget(m_pThemeNameEdit,0,0,1,2);
	connect(m_pThemeNameEdit,SIGNAL(textChanged(const QString &)),this,SLOT(themeNameChanged(const QString &)));


	l = new QLabel(__tr2qs_ctx("Version:","theme"),this);
	g->addWidget(l,1,0);

	m_pVersionEdit = new QLineEdit(this);
	g->addMultiCellWidget(m_pVersionEdit,1,1,1,2);


	l = new QLabel(__tr2qs_ctx("Author:","theme"),this);
	g->addWidget(l,2,0);

	m_pAuthorEdit = new QLineEdit(this);
	g->addMultiCellWidget(m_pAuthorEdit,2,2,1,2);

	l = new QLabel(__tr2qs_ctx("Description:","theme"),this);
	g->addWidget(l,3,0);

	m_pDescriptionEdit = new QTextEdit(this);
	g->addMultiCellWidget(m_pDescriptionEdit,3,4,1,2);
	
	m_pIncludeMsgcolors = new KviStyledCheckBox( __tr2qs_ctx("Include message colors information","theme"), this );
	m_pIncludeMsgcolors->setChecked(TRUE);
	g->addMultiCellWidget(m_pIncludeMsgcolors,5,5,0,1);
	
//	l = new QLabel(__tr2qs_ctx("Package type:","theme"),this);
//	g->addWidget(l,6,0);

//	m_pFormatCombo = new QComboBox(this);
//	m_pFormatCombo->insertItem(__tr2qs_ctx("KVIrc Package","theme"),0);
//	m_pFormatCombo->insertItem(__tr2qs_ctx("New multi-file format","theme"),1);
//	m_pFormatCombo->insertItem(__tr2qs_ctx("Dublicate Theme","theme"),1);
//	g->addWidget(m_pFormatCombo,6,1);

	QHBox * h = new QHBox(this);
	h->setSpacing(7);
	g->addWidget(h,7,2);
	m_pOkButton = new QPushButton(__tr2qs_ctx("OK","theme"),h);
	m_pOkButton->setEnabled(false);
	connect(m_pOkButton,SIGNAL(clicked()),this,SLOT(saveTheme()));
	m_pOkButton->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)));

	QPushButton * pb = new QPushButton(__tr2qs_ctx("Cancel","theme"),h);
	connect(pb,SIGNAL(clicked()),this,SLOT(reject()));
	pb->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DISCARD)));

}

KviSaveThemeDialog::~KviSaveThemeDialog()
{

}

void KviSaveThemeDialog::themeNameChanged(const QString &txt)
{
	m_pOkButton->setEnabled(!txt.isEmpty());
}

void KviSaveThemeDialog::saveThemeInOldFormat(KviThemeInfo& sto)
{
	if(!KviFileUtils::directoryExists(sto.szAbsoluteDirectory))
	{
		g_pApp->getLocalKvircDirectory(sto.szAbsoluteDirectory,KviApp::Themes,sto.szSubdirectory,true);
		if(!KviFileUtils::makeDir(sto.szAbsoluteDirectory))
		{
			QMessageBox::critical(this,__tr2qs_ctx("Save Theme - KVIrc","theme"),__tr2qs_ctx("Unable to create theme directory.","theme"),
				QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
			return;
		}
	}
	g_pApp->saveTheme(sto);
	// write down the images

	QString szMsg = __tr2qs_ctx("Theme saved successfully to ","theme");
	szMsg += sto.szAbsoluteDirectory;

	QMessageBox::information(this,__tr2qs_ctx("Save Theme - KVIrc","theme"),szMsg,QMessageBox::Ok,
		QMessageBox::NoButton,QMessageBox::NoButton);
}

void KviSaveThemeDialog::saveThemeInNewFormat(KviThemeInfo& sto)
{
	QString szBaseDir;
	QFileDialog * d = new QFileDialog(this);
	d->setCaption(__tr2qs_ctx("Choose save directory","theme"));
	d->setMode(QFileDialog::DirectoryOnly);
	if(d->exec() == QDialog::Accepted)
	{
		szBaseDir = d->selectedFile();
		KviFileUtils::adjustFilePath(szBaseDir);
		delete d;
		if(szBaseDir.isEmpty())
		{
			QMessageBox::critical(this,__tr2qs_ctx("Save Theme - KVIrc","theme"),__tr2qs_ctx("You must select save directory","theme"),
				QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
			return;
		}
	} else {
		delete d;
		QMessageBox::critical(this,__tr2qs_ctx("Save Theme - KVIrc","theme"),__tr2qs_ctx("You must select save directory","theme"),
			QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return;	
	}
	KviQString::ensureLastCharIs(szBaseDir,KVI_PATH_SEPARATOR_CHAR);
	sto.szAbsoluteDirectory=szBaseDir+sto.szSubdirectory;
	KviQString::ensureLastCharIs(sto.szAbsoluteDirectory,KVI_PATH_SEPARATOR_CHAR);
	QString szThemeDir=sto.szAbsoluteDirectory;
	sto.szAbsoluteDirectory+="files";
	
	if(!KviFileUtils::makeDir(sto.szAbsoluteDirectory))
	{
		QMessageBox::critical(this,__tr2qs_ctx("Save Theme - KVIrc","theme"),__tr2qs_ctx("Unable to create theme files directory.","theme"),
			QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return;
	}
	
	g_pApp->saveTheme(sto);

	if(!KviFileUtils::makeDir(szThemeDir+"utils"))
	{
		QMessageBox::critical(this,__tr2qs_ctx("Save Theme - KVIrc","theme"),__tr2qs_ctx("Unable to create theme utils directory.","theme"),
			QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return;
	}
	
	QString szInstallerScript;
	g_pApp->getGlobalKvircDirectory(szInstallerScript,KviApp::DefScript,"installer.kvs");
	if(!KviFileUtils::fileExists(szInstallerScript))
	{
		QMessageBox::critical(this,__tr2qs_ctx("Save Theme - KVIrc","theme"),__tr2qs_ctx("Cannot find installer.kvs.","theme"),
			QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return;
	}
	
	if(!KviFileUtils::copyFile(szInstallerScript,szThemeDir+"utils/installer.kvs"))
	{
		QMessageBox::critical(this,__tr2qs_ctx("Save Theme - KVIrc","theme"),__tr2qs_ctx("Cannot write installer.kvs.","theme"),
			QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return;
	}
	
	g_pApp->getGlobalKvircDirectory(szInstallerScript,KviApp::DefScript,"theme-install.kvs");
	QString szMainScript=szThemeDir+"install.kvs";
	if(!KviFileUtils::copyFile(szInstallerScript,szMainScript))
	{
		QMessageBox::critical(this,__tr2qs_ctx("Save Theme - KVIrc","theme"),__tr2qs_ctx("Cannot write install.kvs.","theme"),
			QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return;
	}
	QFile installFile(szMainScript);
	if(!installFile.open(IO_ReadOnly))
	{
		QMessageBox::critical(this,__tr2qs_ctx("Save Theme - KVIrc","theme"),__tr2qs_ctx("Cannot open install.kvs for reading.","theme"),
			QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return;
	}
	
	QCString szFileContents;
	szFileContents=installFile.readAll();
	installFile.close();
	
	sto.szName.replace("\"","\\\"");

	szFileContents.replace("@THEMENAME@",sto.szName.utf8().data());
//		szFileContents.replace("@THEMENAME@",sto.szName.utf8().data());
	sto.szDescription.replace("\"","\\\"");
	if(sto.szDescription.isEmpty()) sto.szDescription="empty";
	szFileContents.replace("@THEMEDESCRIPTION@",sto.szDescription.utf8().data());
	sto.szVersion.replace("\"","\\\"");
	szFileContents.replace("@THEMEVERSION@",sto.szVersion.utf8().data());
	szFileContents.replace("@SUBDIR@",sto.szSubdirectory.utf8().data());
	
	QString szAliasName="";
	QString m_szId;
	
	struct timeval tv;
	kvi_gettimeofday(&tv,0);
	
	KviQString::sprintf(m_szId,"theme_%d%d%d",tv.tv_usec,rand() % 1000,rand() % 1000);
	szAliasName.prepend(m_szId);
	szFileContents.replace("@ALIASNAME@",szAliasName.utf8().data());
	if(!installFile.open(IO_WriteOnly))
	{
		QMessageBox::critical(this,__tr2qs_ctx("Save Theme - KVIrc","theme"),__tr2qs_ctx("Cannot open install.kvs for writing.","theme"),
			QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return;
	}
	installFile.writeBlock(szFileContents.data(),szFileContents.length());
	installFile.close();
	
	QString szMsg = __tr2qs_ctx("Theme saved successfully to ","theme");
	szMsg += szThemeDir;

	QMessageBox::information(this,__tr2qs_ctx("Save Theme - KVIrc","theme"),szMsg,QMessageBox::Ok,
		QMessageBox::NoButton,QMessageBox::NoButton);
}

void KviSaveThemeDialog::saveThemeInXmlFormat(KviThemeInfo& sto)
{
	QString szFilename;
	QFileDialog * d = new QFileDialog(this);
	d->setCaption(__tr2qs_ctx("Choose filaname","theme"));
	d->setMode(QFileDialog::AnyFile);
	d->setFilter( "KVIrc Packages (*.kvp)" );
	if(d->exec() == QDialog::Accepted)
	{
		szFilename = d->selectedFile();
		KviFileUtils::adjustFilePath(szFilename);
		delete d;
		if(szFilename.isEmpty())
		{
			QMessageBox::critical(this,__tr2qs_ctx("Save Theme - KVIrc","theme"),__tr2qs_ctx("You must select filename","theme"),
				QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
			return;
		}
	} else {
		delete d;
		QMessageBox::critical(this,__tr2qs_ctx("Save Theme - KVIrc","theme"),__tr2qs_ctx("You must select filename","theme"),
			QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return;	
	}
	if(QFileInfo(szFilename).extension()!="kvp") szFilename.append(".kvp");
	g_pApp->saveXmlTheme(sto,szFilename);
}

void KviSaveThemeDialog::saveTheme()
{
	KviThemeInfo sto;
	sto.szName = m_pThemeNameEdit->text();
	if(sto.szName.isEmpty())
	{
		QMessageBox::critical(this,__tr2qs_ctx("Save Theme - KVIrc","theme"),__tr2qs_ctx("You must choose a theme name!","theme"),QMessageBox::Ok,
			QMessageBox::NoButton,QMessageBox::NoButton);
		return;
	}

	sto.szAuthor = m_pAuthorEdit->text();
	sto.szDescription = m_pDescriptionEdit->text();
	sto.szDate = QDateTime::currentDateTime().toString();
	sto.szVersion = m_pVersionEdit->text();
	sto.szKvircVersion = KVI_VERSION;
	sto.bIncludeMsgColors=m_pIncludeMsgcolors->isChecked();
	
	if(sto.szVersion.isEmpty())sto.szVersion = "1.0.0";

	sto.szSubdirectory = sto.szName + QString("-") + sto.szVersion;
	sto.szSubdirectory.replace(QRegExp("[ \\\\/:][ \\\\/:]*"),"_");

/*
	switch(m_pFormatCombo->currentItem())
	{
		case 0:
			saveThemeInXmlFormat(sto);
			break;
		case 1:
			saveThemeInNewFormat(sto);
			break;
		case 2:*/
			saveThemeInOldFormat(sto);
/*			break;
	}
*/
	accept();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


KviPackThemeDialog::KviPackThemeDialog(QWidget * pParent,KviPtrList<KviThemeInfo> * pThemeInfoList)
: QWizard(pParent)
{
	m_pThemeInfoList = pThemeInfoList;

	KviThemeInfo * pThemeInfo;
	QString szPackageName;
	QString szPackageAuthor;
	QString szPackageDescription;
	QString szPackageVersion;
	if(m_pThemeInfoList->count() > 1)
	{
		szPackageName = "MyThemes";
		szPackageAuthor = __tr2qs_ctx("Your name here","theme");
		szPackageVersion = "1.0";
		szPackageDescription = __tr2qs_ctx("Put a package description here...","theme");
	} else {
		if(m_pThemeInfoList->count() > 0)
		{
			pThemeInfo = m_pThemeInfoList->first();
			szPackageName = pThemeInfo->szSubdirectory;
			szPackageAuthor = pThemeInfo->szAuthor;
			szPackageDescription = pThemeInfo->szDescription;
			szPackageVersion = pThemeInfo->szVersion;
		}
	}


	setCaption(__tr2qs_ctx("Export Theme - KVIrc","theme"));
	setMinimumSize(400,350);

	// welcome page ==================================================================================
	QWidget * pPage = new QWidget(this);
	QGridLayout * pLayout = new QGridLayout(pPage,2,1,4,4);
	
	QLabel * pLabel = new QLabel(pPage);
	QString szText = "<p>";
	szText += __tr2qs_ctx("This procedure allows you to export the selected themes to a single package. It is useful when you want to distribute your themes to the public.","theme");
	szText += "</p><p>";
	szText += __tr2qs_ctx("You will be asked to provide a package name, a description and, if you want, an icon/screenshot.","theme");
	szText += "</p><p>";
	szText += __tr2qs_ctx("Hit the \"Next\" button to begin.","theme");
	szText += "<p>";
	
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
	pLayout = new QGridLayout(pPage,2,1,4,4);

	pLabel = new QLabel(pPage);
	pLabel->setText(__tr2qs_ctx("This is the information list for the themes you're packaging. If it looks OK press \"Next\" to continue, otherwise press \"Cancel\" and rewiew your themes first.","theme"));
	pLabel->setTextFormat(Qt::RichText);
	pLayout->addWidget(pLabel,0,0);

	QString szAuthor = __tr2qs_ctx("Author","theme");
	QString szCreatedAt = __tr2qs_ctx("Created at","theme");
	QString szCreatedOn = __tr2qs_ctx("Created on","theme");
	QString szSubdirectory = __tr2qs_ctx("Subdirectory","theme");

	QString szThemesDescription = "<html><body bgcolor=\"#ffffff\">";

	int iIdx = 0;
	for(pThemeInfo = m_pThemeInfoList->first();pThemeInfo;pThemeInfo = m_pThemeInfoList->next())
	{
		QString szThemeDescription;
		KviQString::sprintf(
			szThemeDescription,
			"<p>" \
				"<h2>%Q %Q</h2>" \
			"</p>" \
			"<p>" \
				"<i>%Q</i>" \
			"</p>" \
			"<p>" \
				"%Q: <b>%Q</b><br>" \
				"%Q: <b>%Q</b><br>" \
			"</p>" \
			"<p>" \
				"<font color=\"#808080\">" \
					"%Q: KVIrc %Q<br>" \
					"%Q: %Q<br>" \
				"</font>" \
			"</p>",
			&(pThemeInfo->szName),
			&(pThemeInfo->szVersion),
			&(pThemeInfo->szDescription),
			&szAuthor,
			&(pThemeInfo->szAuthor),
			&szCreatedAt,
			&(pThemeInfo->szDate),
			&szCreatedOn,
			&(pThemeInfo->szKvircVersion),
			&szSubdirectory,
			&(pThemeInfo->szSubdirectory)
		);
		if(iIdx > 0)
			szThemesDescription += "<hr>";
		szThemesDescription += szThemeDescription;
		iIdx++;
	}
	
	szThemesDescription += "</body></html>";

	QTextEdit * pTextEdit = new QTextEdit(pPage);
	pTextEdit->setPaper(QBrush(QColor(255,255,255)));
	pTextEdit->setReadOnly(true);
	pTextEdit->setText(szThemesDescription);
	pLayout->addWidget(pTextEdit,1,0);
	pLayout->setRowStretch(1,1);

	addPage(pPage,__tr2qs_ctx("Theme Data","theme"));
	setBackEnabled(pPage,true);
	setHelpEnabled(pPage,false);
	setNextEnabled(pPage,true);
	setFinishEnabled(pPage,false);

	// packager informations ================================================================================

	pPage = new QWidget(this);
	pLayout = new QGridLayout(pPage,5,2,4,4);

	pLabel = new QLabel(pPage);
	pLabel->setText(__tr2qs_ctx("Here you need to provide informations about you (the packager) and a short description of the package you're creating.","theme"));
	pLabel->setTextFormat(Qt::RichText);
	pLayout->addMultiCellWidget(pLabel,0,0,0,1);
	
	pLabel = new QLabel(pPage);
	pLabel->setText(__tr2qs_ctx("Package Name:","theme"));
	pLayout->addWidget(pLabel,1,0);
	
	m_pPackageNameEdit = new QLineEdit(pPage);
	m_pPackageNameEdit->setText(szPackageName);
	pLayout->addWidget(m_pPackageNameEdit,1,1);

	pLabel = new QLabel(pPage);
	pLabel->setText(__tr2qs_ctx("Version:","theme"));
	pLayout->addWidget(pLabel,2,0);
	
	m_pPackageVersionEdit = new QLineEdit(pPage);
	m_pPackageVersionEdit->setText(szPackageVersion);
	pLayout->addWidget(m_pPackageVersionEdit,2,1);

	pLabel = new QLabel(pPage);
	pLabel->setText(__tr2qs_ctx("Description:","theme"));
	pLayout->addWidget(pLabel,3,0);
	
	m_pPackageDescriptionEdit = new QMultiLineEdit(pPage);
	m_pPackageDescriptionEdit->setText(szPackageDescription);
	pLayout->addWidget(m_pPackageDescriptionEdit,3,1);

	pLabel = new QLabel(pPage);
	pLabel->setText(__tr2qs_ctx("Package Author:","theme"));
	pLayout->addWidget(pLabel,4,0);
	
	m_pPackagerNameEdit = new QLineEdit(pPage);
	m_pPackagerNameEdit->setText(szPackageAuthor);
	pLayout->addWidget(m_pPackagerNameEdit,4,1);


	pLayout->setRowStretch(3,1);
	pLayout->setColStretch(1,1);

	addPage(pPage,__tr2qs_ctx("Package Informations","theme"));
	setBackEnabled(pPage,true);
	setHelpEnabled(pPage,false);
	setNextEnabled(pPage,true);
	setFinishEnabled(pPage,false);

	// screenshot/logo/icon ================================================================================

	pPage = new QWidget(this);
	pLayout = new QGridLayout(pPage,3,1,4,4);

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
	pLayout = new QGridLayout(pPage,4,1,4,4);

	pLabel = new QLabel(pPage);
	pLabel->setText(__tr2qs_ctx("Here you must choose the file name for the theme package. It should have a *.%1 extension.","theme").arg(KVI_FILEEXTENSION_THEMEPACKAGE));
	pLabel->setTextFormat(Qt::RichText);
	pLayout->addWidget(pLabel,0,0);

	m_szPackagePath = QDir::homeDirPath();
	KviQString::ensureLastCharIs(m_szPackagePath,QChar(KVI_PATH_SEPARATOR_CHAR));
	
	m_szPackagePath += szPackageName;
	m_szPackagePath += "-";
	m_szPackagePath += szPackageVersion;
	m_szPackagePath += ".";
	m_szPackagePath += KVI_FILEEXTENSION_THEMEPACKAGE;

	szFilter = "*.";
	szFilter += KVI_FILEEXTENSION_THEMEPACKAGE;
	m_pPathSelector = new KviFileSelector(pPage,"",&m_szPackagePath,true,KviFileSelector::ChooseSaveFileName,szFilter);
	pLayout->addWidget(m_pPathSelector,1,0);

	pLabel = new QLabel(pPage);
	pLabel->setText(__tr2qs_ctx("Finally hit the \"Finish\" button to complete the packaging operation.","theme"));
	pLabel->setTextFormat(Qt::RichText);
	pLayout->addWidget(pLabel,3,0);

	pLayout->setRowStretch(2,1);

	addPage(pPage,__tr2qs_ctx("Package Path","theme"));
	setBackEnabled(pPage,true);
	setHelpEnabled(pPage,false);
	setNextEnabled(pPage,false);
	setFinishEnabled(pPage,true);

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
			out.convertFromImage(pix.smoothScale(300,225,QImage::ScaleMin));
		else
			out.convertFromImage(pix);
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
	QWizard::accept();
}


bool KviPackThemeDialog::packTheme()
{
	m_pImageSelector->commit();
	m_pPathSelector->commit();

	QString szPackageAuthor = m_pPackagerNameEdit->text();
	QString szPackageName = m_pPackageNameEdit->text();
	QString szPackageDescription = m_pPackageDescriptionEdit->text();
	QString szPackageVersion = m_pPackageVersionEdit->text();

	QImage pix(m_szImagePath);
	QPixmap out;
	if(!pix.isNull())
	{
		if(pix.width() > 300 || pix.height() > 225)
			out.convertFromImage(pix.smoothScale(300,225,QImage::ScaleMin));
		else
			out.convertFromImage(pix);
	} else {
		if(!m_szImagePath.isEmpty())
		{
			QMessageBox::critical(this,__tr2qs_ctx("Export Theme - KVIrc","theme"),__tr2qs_ctx("Failed to load the selected image: please fix it","theme"),
				QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
			showPage(m_pImageSelectionPage);
			return false;
		}
	}

	KviPackageWriter f;

	QString szTmp = QDateTime::currentDateTime().toString();

	f.addInfoField("PackageType","ThemePack");
	f.addInfoField("ThemePackVersion","1");
	f.addInfoField("Name",szPackageName);
	f.addInfoField("Version",szPackageVersion);
	f.addInfoField("Author",szPackageAuthor);
	f.addInfoField("Description",szPackageDescription);
	f.addInfoField("Date",szTmp);
	f.addInfoField("KVIrcVersion",KVI_VERSION);

	if(!out.isNull())
	{
		QByteArray * pba = new QByteArray();
		QBuffer buffer(*pba);
		buffer.open(IO_WriteOnly);
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
		f.addInfoField(szTmp,pInfo->szName);
		KviQString::sprintf(szTmp,"Theme%dVersion",iIdx);
		f.addInfoField(szTmp,pInfo->szVersion);
		KviQString::sprintf(szTmp,"Theme%dDescription",iIdx);
		f.addInfoField(szTmp,pInfo->szDescription);
		KviQString::sprintf(szTmp,"Theme%dDate",iIdx);
		f.addInfoField(szTmp,pInfo->szDate);
		KviQString::sprintf(szTmp,"Theme%dSubdirectory",iIdx);
		f.addInfoField(szTmp,pInfo->szSubdirectory);
		KviQString::sprintf(szTmp,"Theme%dAuthor",iIdx);
		f.addInfoField(szTmp,pInfo->szAuthor);
		KviQString::sprintf(szTmp,"Theme%dKVIrcVersion",iIdx);
		f.addInfoField(szTmp,pInfo->szKvircVersion);
		if(!f.addDirectory(pInfo->szAbsoluteDirectory,pInfo->szSubdirectory))
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


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



KviThemeListBoxItem::KviThemeListBoxItem(QListBox * box,KviThemeInfo * inf)
: QListBoxText(box)
{
	m_pThemeInfo = inf;
	QString t;
	t = "<nobr><b>";
	t += inf->szName;
	t += "</b>";
	
	if(!inf->szVersion.isEmpty()) {
		t += "[";
		t += inf->szVersion;
		t += "]";
	}
	
	if(!inf->szAuthor.isEmpty()) {
		t += " <font color=\"#a0a0a0\"> ";
		t += __tr2qs_ctx("by","theme");
		t += " ";
		t += inf->szAuthor;
		t += "</font>";
	}

	t += "</nobr>";
	t += "<br><nobr><font size=\"-1\">";
	t += inf->szDescription;
	t += "</font></nobr>";
	m_pText = new QSimpleRichText(t,box->font());
	int iWidth = box->visibleWidth();
	if(iWidth < LVI_MINIMUM_CELL_WIDTH)iWidth = LVI_MINIMUM_CELL_WIDTH;
	iWidth -= LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING + LVI_BORDER;
	m_pText->setWidth(iWidth);
}

KviThemeListBoxItem::~KviThemeListBoxItem()
{
	delete m_pThemeInfo;
	delete m_pText;
}

void KviThemeListBoxItem::paint ( QPainter * p )
{
	QListBoxText::paint(p);
	p->drawPixmap(LVI_BORDER,LVI_BORDER, *(g_pIconManager->getBigIcon(QString(KVI_BIGICON_THEME))) );
	int afterIcon = LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING;
	int www = p->window().width() - (afterIcon + LVI_BORDER);
	m_pText->setWidth(www);
	m_pText->draw(p,afterIcon,LVI_BORDER,QRect(afterIcon,LVI_BORDER,www,p->window().height() - (LVI_BORDER * 2)),listBox()->viewport()->colorGroup());
}

int KviThemeListBoxItem::height ( const QListBox * lb ) const 
{
	int iHeight = m_pText->height() + (2 * LVI_BORDER);
	if(iHeight < (LVI_ICON_SIZE + (2 * LVI_BORDER)))iHeight = LVI_ICON_SIZE + (2 * LVI_BORDER);
	return iHeight;
}

KviThemeManagementDialog * KviThemeManagementDialog::m_pInstance = 0;


KviThemeManagementDialog::KviThemeManagementDialog(QWidget * parent)
: QDialog(parent,"theme_options_widget")
{
	setCaption(__tr2qs_ctx("Manage Themes - KVIrc","theme"));
	setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_THEME)));

	setModal(true);

	m_pInstance = this;

	m_pSaveThemeDialog = 0;

	QGridLayout * g = new QGridLayout(this,3,2,4,4);

	QHBox *hb = new QHBox(this);
	g->addMultiCellWidget(hb,0,0,0,1);

	KviStyledToolButton * tb;
	QFrame * sep;

	tb = new KviStyledToolButton(hb);
	tb->setIconSet(*(g_pIconManager->getBigIcon(KVI_BIGICON_SAVE)));
	tb->setUsesBigPixmap(true);
	QToolTip::add(tb,__tr2qs_ctx("Save Current Theme...","theme"));
	connect(tb,SIGNAL(clicked()),this,SLOT(saveCurrentTheme()));

	sep = new QFrame(hb);
	sep->setFrameStyle(QFrame::VLine | QFrame::Sunken);
	sep->setMinimumWidth(12);
	
	m_pPackThemeButton = new KviStyledToolButton(hb);
	m_pPackThemeButton->setIconSet(*(g_pIconManager->getBigIcon(KVI_BIGICON_PACK)));
	m_pPackThemeButton->setUsesBigPixmap(true);
	QToolTip::add(m_pPackThemeButton,__tr2qs_ctx("Export Selected Themes to a Distributable Package","theme"));
	connect(m_pPackThemeButton,SIGNAL(clicked()),this,SLOT(packTheme()));

	m_pDeleteThemeButton = new KviStyledToolButton(hb);
	m_pDeleteThemeButton->setIconSet(*(g_pIconManager->getBigIcon(KVI_BIGICON_REMOVE)));
	m_pDeleteThemeButton->setUsesBigPixmap(true);
	QToolTip::add(m_pDeleteThemeButton,__tr2qs_ctx("Delete Selected Themes","theme"));
	connect(m_pDeleteThemeButton,SIGNAL(clicked()),this,SLOT(deleteTheme()));

	sep = new QFrame(hb);
	sep->setFrameStyle(QFrame::VLine | QFrame::Sunken);
	sep->setMinimumWidth(12);

	tb = new KviStyledToolButton(hb);
	tb->setIconSet(*(g_pIconManager->getBigIcon(KVI_BIGICON_OPEN)));
	tb->setUsesBigPixmap(true);
	QToolTip::add(tb,__tr2qs_ctx("Install Theme Package From Disk","theme"));
	connect(tb,SIGNAL(clicked()),this,SLOT(installFromXml()));

	tb = new KviStyledToolButton(hb);
	tb->setIconSet(*(g_pIconManager->getBigIcon(KVI_BIGICON_WWW)));
	tb->setUsesBigPixmap(true);
	QToolTip::add(tb,__tr2qs_ctx("Get More Themes...","theme"));
	connect(tb,SIGNAL(clicked()),this,SLOT(getMoreThemes()));

	QWidget *w= new QWidget(hb);
	w->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);

	m_pListBox = new QListBox(this);
	m_pListBox->setMinimumHeight(400);
	m_pListBox->setMinimumWidth(400);
	m_pListBox->setSelectionMode(QListBox::Extended);
	connect(m_pListBox,SIGNAL(doubleClicked(QListBoxItem *)),this,SLOT(applyTheme(QListBoxItem *)));
	connect(m_pListBox,SIGNAL(contextMenuRequested(QListBoxItem *,const QPoint &)),
		this,SLOT(contextMenuRequested(QListBoxItem *,const QPoint &)));
	connect(m_pListBox,SIGNAL(selectionChanged()),this,SLOT(enableDisableButtons()));
	g->addMultiCellWidget(m_pListBox,1,1,0,1);


	QPushButton * b = new QPushButton(__tr2qs("Close"),this);
	connect(b,SIGNAL(clicked()),this,SLOT(closeClicked()));
	g->addWidget(b,2,1);

	g->setRowStretch(1,0);
	g->setColStretch(0,1);

	fillThemeBox();
	m_pContextPopup = new QPopupMenu(this);

	if(g_rectManagementDialogGeometry.y() < 5)
	{
		g_rectManagementDialogGeometry.setY(5);
	}
	resize(g_rectManagementDialogGeometry.width(),
		g_rectManagementDialogGeometry.height());
	move(g_rectManagementDialogGeometry.x(),
		g_rectManagementDialogGeometry.y());
}

KviThemeManagementDialog::~KviThemeManagementDialog()
{
	g_rectManagementDialogGeometry = QRect(pos().x(),pos().y(),size().width(),size().height());

	if(m_pSaveThemeDialog)
	{
		delete m_pSaveThemeDialog;
		m_pSaveThemeDialog = 0;
	}
	
	m_pInstance = 0;
}

void KviThemeManagementDialog::closeClicked()
{
	delete this;
}


void KviThemeManagementDialog::display()
{
	if(m_pInstance)return;
	m_pInstance = new KviThemeManagementDialog(g_pFrame);
	m_pInstance->show();
}

void KviThemeManagementDialog::cleanup()
{
	if(!m_pInstance)return;
	delete m_pInstance;
	m_pInstance = 0;
}

void KviThemeManagementDialog::packTheme()
{
	KviPtrList<KviThemeInfo> dl;
	dl.setAutoDelete(false);
	for(KviThemeListBoxItem * it = (KviThemeListBoxItem *)m_pListBox->firstItem();it;it = (KviThemeListBoxItem *)it->next())
	{
		if(it->isSelected())
			dl.append(it->themeInfo());
	}
	if(dl.isEmpty())return;

	KviPackThemeDialog * pDialog = new KviPackThemeDialog(this,&dl);
	pDialog->exec();
	delete pDialog;

}

void KviThemeManagementDialog::contextMenuRequested ( QListBoxItem * it, const QPoint & pos )
{
	if(it)
	{
		m_pListBox->setCurrentItem(it);
		m_pContextPopup->clear();
		m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MINUS)),__tr2qs_ctx("&Remove Theme","theme"),this,SLOT(deleteTheme()));
		m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)),__tr2qs_ctx("&Apply Theme","theme"),this,SLOT(applyCurrentTheme()));
		m_pContextPopup->popup(pos);
	}
}

void KviThemeManagementDialog::applyTheme ( QListBoxItem * it)
{
	if(it) m_pListBox->setCurrentItem(it);
	applyCurrentTheme();
}

void KviThemeManagementDialog::applyCurrentTheme()
{
	KviThemeListBoxItem * it = (KviThemeListBoxItem *)m_pListBox->item(m_pListBox->currentItem());
	if(!it)return;

	if(KviMessageBox::yesNo(__tr2qs_ctx("Apply theme - KVIrc","theme"),
		__tr2qs_ctx("Do you wish to apply theme \"%Q\" (version %Q)?","theme"),
		&(it->themeInfo()->szName),&(it->themeInfo()->szVersion))) {
		
			QString szPath = it->themeInfo()->szAbsoluteDirectory;
			if(szPath.isEmpty())return;

			g_pApp->loadTheme(szPath);
	}
}

void KviThemeManagementDialog::deleteTheme()
{
	for(KviThemeListBoxItem * pItem = (KviThemeListBoxItem *)m_pListBox->firstItem();pItem;pItem = (KviThemeListBoxItem *)pItem->next())
	{
		if(pItem->isSelected())
		{
			if(!KviMessageBox::yesNo(__tr2qs_ctx("Delete Theme - KVIrc","theme"),
				__tr2qs_ctx("Do you really wish to delete theme \"%Q\" (version %Q)?","theme"),
					&(pItem->themeInfo()->szName),&(pItem->themeInfo()->szVersion)))goto jump_out;

			KviFileUtils::deleteDir(pItem->themeInfo()->szAbsoluteDirectory);
		}
	}
jump_out:
	fillThemeBox();
}

void KviThemeManagementDialog::installFromXml()
{
	QString szFileName;
	QDict<QString> * pInfoFields;
	QString * pValue;
	bool bInstall;
	QPixmap pix;
	QByteArray * pByteArray;

	const char * check_fields[] = { "Name", "Version", "Author", "Description", "Date", "KVIrcVersion" };
	
	if(!KviFileDialog::askForOpenFileName(szFileName,__tr2qs_ctx("Open Theme - KVIrc","theme"),QString::null,"*.kvt"))
		return;

	// check if it is a valid theme file
	KviPackageReader r;
	if(!r.readHeader(szFileName))
	{
		QString szErr = r.lastError();
		KviMessageBox::information(__tr2qs_ctx("The selected file does not seem to be a valid KVIrc package: %Q","theme"),&szErr);
		return;
	}

	pInfoFields = r.stringInfoFields();

	pValue = pInfoFields->find("PackageType");
	if(!pValue)goto not_a_valid_theme_package;
	if(!KviQString::equalCI(*pValue,"ThemePack"))goto not_a_valid_theme_package;
	pValue = pInfoFields->find("ThemePackVersion");
	if(!pValue)goto not_a_valid_theme_package;
	if(!KviQString::equalCI(*pValue,"1"))goto not_a_valid_theme_package;
	
	// make sure the default fields exist
	for(int i=0;i<6;i++)
	{
		pValue = pInfoFields->find(check_fields[i]);
		if(!pValue)goto not_a_valid_theme_package;
	}

	pValue = pInfoFields->find("ThemeCount");
	if(!pValue)goto not_a_valid_theme_package;
	bool bOk;
	int iThemeCount = pValue->toInt(&bOk);
	if(!bOk)goto not_a_valid_theme_package;
	if(iThemeCount < 1)goto not_a_valid_theme_package;

	// ok.. it should be really valid at this point
	
	// load its picture
	pByteArray = r.binaryInfoFields()->find("Image");
	if(pByteArray)
	{
		pix.loadFromData(*pByteArray,0,0);
	}
	
	if(pix.isNull())
	{
		// load the default icon
	}
	
	/*
	KviUnpackThemeDialog * pDialog = new KviUnpackThemeDialog(this,szFileName);
	bool bInstall = pDialog->exec() == QDialog::Accepted;
	delete pDialog;
	*/
	
	bInstall = true;
	
	if(bInstall)
	{
		QString szUnpackPath;
		g_pApp->getLocalKvircDirectory(szUnpackPath,KviApp::Themes);
		if(!r.unpack(szFileName,szUnpackPath))
		{
			QString szErr2 = r.lastError();
			KviMessageBox::information(__tr2qs_ctx("Failed to unpack the selected file: %Q","theme"),&szErr2);
			return;
		}
	}

	/*
	KviXmlAddonLoader * pLoader = new KviXmlAddonLoader(szFilename,this);
	pLoader->exec();
	*/
	fillThemeBox();
	return;
not_a_valid_theme_package:
	KviMessageBox::information(__tr2qs_ctx("The selected file does not seem to be a valid KVIrc theme package","theme"));
	return;
}

void KviThemeManagementDialog::getMoreThemes()
{
	if(!g_pFrame)return;
	g_pFrame->executeInternalCommand(KVI_INTERNALCOMMAND_OPENURL_KVIRC_THEMES);
}

void KviThemeManagementDialog::saveCurrentTheme()
{
	if(m_pSaveThemeDialog)return;
	m_pSaveThemeDialog = new KviSaveThemeDialog(this);
	m_pSaveThemeDialog->exec();
	if(m_pSaveThemeDialog)
	{
		delete m_pSaveThemeDialog;
		m_pSaveThemeDialog = 0;
		fillThemeBox();
	}
}

void KviThemeManagementDialog::fillThemeBox(const QString &szDir)
{
	QDir d(szDir);

	QStringList sl = d.entryList(QDir::Dirs);

	for(QStringList::Iterator it = sl.begin();it != sl.end();++it)
	{
		if(*it == ".")continue;
		if(*it == "..")continue;

		QString szTest = szDir;
		szTest += KVI_PATH_SEPARATOR_CHAR;
		szTest += *it;

		KviThemeInfo * inf = new KviThemeInfo;
		inf->szSubdirectory = *it;
		if(g_pApp->loadThemeInfo(szTest,*inf))
		{
			KviThemeListBoxItem * item = 0;
			item = new KviThemeListBoxItem(m_pListBox,inf);
		} else {
			delete inf;
		}
	}
}

void KviThemeManagementDialog::fillThemeBox()
{
	m_pListBox->clear();

//	KviThemeInfo * ti = new KviThemeInfo;
//	ti->szName = __tr2qs_ctx("Current Theme","theme");

//	KviThemeListBoxItem * itewrq = new KviThemeListBoxItem(m_pListBox,ti);

	QString szDir;
	g_pApp->getGlobalKvircDirectory(szDir,KviApp::Themes);
	fillThemeBox(szDir);
	g_pApp->getLocalKvircDirectory(szDir,KviApp::Themes);
	fillThemeBox(szDir);

//	m_pListBox->setCurrentItem(itewrq);
	enableDisableButtons();
}

bool KviThemeManagementDialog::hasSelectedItems()
{
	for(QListBoxItem * it = m_pListBox->firstItem();it;it = it->next())
	{
		if(it->isSelected())return true;
	}
	return false;
}


void KviThemeManagementDialog::enableDisableButtons()
{
	bool b = hasSelectedItems();
	m_pPackThemeButton->setEnabled(b);
	m_pDeleteThemeButton->setEnabled(b);
}


