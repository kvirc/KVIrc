//=============================================================================
//
//   File : savethemedialog.cpp
//   Creation date : Wed 03 Jan 2007 03:01:34 by Szymon Stefanek
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

#include "savethemedialog.h"
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

#include <QTextEdit>
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




KviSaveThemeDialog::KviSaveThemeDialog(QWidget * pParent)
: KviTalWizard(pParent)
{
	setWindowTitle(__tr2qs_ctx("Save Current Theme - KVIrc","theme"));
	setMinimumSize(400,350);

	// welcome page ==================================================================================
	QWidget * pPage = new QWidget(this);
	QGridLayout * pLayout = new QGridLayout(pPage);

	QLabel * pLabel = new QLabel(pPage);
	pLabel->setWordWrap(true);
	QString szText = "<p>";
	szText += __tr2qs_ctx("This procedure allows you to save the current theme settings to a single directory. It is useful if you want to apply other themes or play with the theme settings and later come back to this theme with a single click. It will also allow you to manually modify the theme settings and later export them to a distributable package.","theme");
	szText += "</p><p>";
	szText += __tr2qs_ctx("You will be asked to provide a theme name, a description and, if you want, a screenshot.","theme");
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

	// packager information ================================================================================

	pPage = new QWidget(this);
	pLayout = new QGridLayout(pPage);

	pLabel = new QLabel(pPage);
	pLabel->setText(__tr2qs_ctx("Here you need to provide information about you (the author) and a short description of the theme you're creating.","theme"));
	pLabel->setWordWrap(true);
	pLabel->setTextFormat(Qt::RichText);
	pLayout->addWidget(pLabel,0,0,1,2);

	pLabel = new QLabel(pPage);
	pLabel->setText(__tr2qs_ctx("Theme Name:","theme"));
	pLayout->addWidget(pLabel,1,0);

	m_pThemeNameEdit = new QLineEdit(pPage);
	//m_pThemeNameEdit->setText(szThemeName);
	pLayout->addWidget(m_pThemeNameEdit,1,1);

	pLabel = new QLabel(pPage);
	pLabel->setText(__tr2qs_ctx("Version:","theme"));
	pLayout->addWidget(pLabel,2,0);

	m_pThemeVersionEdit = new QLineEdit(pPage);
	//m_pThemeVersionEdit->setText(szThemeVersion);
	pLayout->addWidget(m_pThemeVersionEdit,2,1);

	pLabel = new QLabel(pPage);
	pLabel->setText(__tr2qs_ctx("Description:","theme"));
	pLayout->addWidget(pLabel,3,0);

	m_pThemeDescriptionEdit = new QTextEdit(pPage);
	//m_pThemeDescriptionEdit->setText(szThemeDescription);
	pLayout->addWidget(m_pThemeDescriptionEdit,3,1);

	pLabel = new QLabel(pPage);
	pLabel->setText(__tr2qs_ctx("Theme Author:","theme"));
	pLayout->addWidget(pLabel,4,0);

	m_pAuthorNameEdit = new QLineEdit(pPage);
	//m_pAuthorNameEdit->setText(szThemeAuthor);
	pLayout->addWidget(m_pAuthorNameEdit,4,1);


	pLayout->setRowStretch(3,1);
	pLayout->setColumnStretch(1,1);

	addPage(pPage,__tr2qs_ctx("Theme Information","theme"));
	setBackEnabled(pPage,true);
	setHelpEnabled(pPage,false);
	setNextEnabled(pPage,true);
	setFinishEnabled(pPage,false);

	// screenshot/logo/icon ================================================================================

	pPage = new QWidget(this);
	pLayout = new QGridLayout(pPage);

	pLabel = new QLabel(pPage);
	pLabel->setText(__tr2qs_ctx("Here you can either choose a screenshot image from disk or make one now. The screenshot will be displayed in the tooltips of the theme management dialog and will be also visible in the package installation dialog if you will export the theme to a distributable package.","theme"));
	pLabel->setWordWrap(true);
	pLabel->setTextFormat(Qt::RichText);
	pLayout->addWidget(pLabel,0,0);

	m_pImageLabel = new QLabel(pPage);
	m_pImageLabel->setFrameStyle(QFrame::Sunken | QFrame::Panel);
	m_pImageLabel->setMinimumSize(300,225);
	m_pImageLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
	pLayout->addWidget(m_pImageLabel,1,0);

	m_pImageSelector = new KviFileSelector(pPage,"",&m_szScreenshotPath,true,0,KVI_FILTER_IMAGE);
	connect(m_pImageSelector,SIGNAL(selectionChanged(const QString &)),this,SLOT(imageSelectionChanged(const QString &)));
	pLayout->addWidget(m_pImageSelector,2,0);

	QPushButton * pButton = new QPushButton(pPage);
	pButton->setText(__tr2qs_ctx("Make Screenshot Now","theme"));
	connect(pButton,SIGNAL(clicked()),this,SLOT(makeScreenshot()));
	pLayout->addWidget(pButton,3,0);

	pLayout->setRowStretch(1,1);

	m_pImageSelectionPage = pPage;
	addPage(pPage,__tr2qs_ctx("Screenshot","theme"));
	setBackEnabled(pPage,true);
	setHelpEnabled(pPage,false);
	setNextEnabled(pPage,true);
	setFinishEnabled(pPage,true);
}

KviSaveThemeDialog::~KviSaveThemeDialog()
{
}

void KviSaveThemeDialog::imageSelectionChanged(const QString &szImagePath)
{
	QImage pix(szImagePath);
	if(!pix.isNull())
	{
		QPixmap out;
		if(pix.width() > 300 || pix.height() > 225)
			out = QPixmap::fromImage(pix.scaled(300,225,Qt::KeepAspectRatio));
		else
			out = QPixmap::fromImage(pix);
		m_pImageLabel->setPixmap(out);
		return;
	}

	QMessageBox::critical(this,__tr2qs_ctx("Save Current Theme - KVIrc","theme"),__tr2qs_ctx("Failed to load the selected image","theme"),
		QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);

	m_pImageSelector->setSelection("");
	m_pImageLabel->setPixmap(QPixmap());
}

void KviSaveThemeDialog::accept()
{
	if(!saveTheme())return;
	KviTalWizard::accept();
}

void KviSaveThemeDialog::makeScreenshot()
{
	QString szFileName;
	g_pApp->getTmpFileName(szFileName,"screenshot.png");
	if(!KviThemeFunctions::makeKVIrcScreenshot(szFileName))
	{
		QMessageBox::critical(this,__tr2qs_ctx("Acquire Screenshot - KVIrc","theme"),__tr2qs_ctx("Failed to make screenshot","theme"),
			QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return;
	}
	m_pImageSelector->setSelection(szFileName);
	imageSelectionChanged(szFileName);
}

bool KviSaveThemeDialog::saveTheme()
{
	m_pImageSelector->commit();

	KviThemeInfo sto;
	sto.setName(m_pThemeNameEdit->text());
	if(sto.name().isEmpty())
	{
		QMessageBox::critical(this,__tr2qs_ctx("Save Current Theme - KVIrc","theme"),__tr2qs_ctx("You must choose a theme name!","theme"),QMessageBox::Ok,
			QMessageBox::NoButton,QMessageBox::NoButton);
		return false;
	}

	sto.setAuthor(m_pAuthorNameEdit->text());
	sto.setDescription(m_pThemeDescriptionEdit->toPlainText());
	
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
	
	sto.setDate(szTmp);
	sto.setVersion(m_pThemeVersionEdit->text());
	sto.setApplication("KVIrc " KVI_VERSION "." KVI_SOURCES_DATE);

	if(sto.version().isEmpty())sto.setVersion("1.0.0");

	QString szSubdir = sto.name() + QString("-") + sto.version();
	szSubdir.replace(QRegExp("[ \\\\/:][ \\\\/:]*"),"_");
	sto.setSubdirectory(szSubdir);

	QString szAbsDir;
	g_pApp->getLocalKvircDirectory(szAbsDir,KviApp::Themes,sto.subdirectory(),true);
	if(!KviFileUtils::makeDir(szAbsDir))
	{
		QMessageBox::critical(this,__tr2qs_ctx("Save Current Theme - KVIrc","theme"),__tr2qs_ctx("Unable to create theme directory.","theme"),
			QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return false;
	}

	sto.setAbsoluteDirectory(szAbsDir);

	if(!KviTheme::save(sto))
	{
		QString szMsg2;
		QString szErr = sto.lastError();
		KviQString::sprintf(szMsg2,__tr2qs_ctx("Unable to save theme: %Q","theme"),&szErr);
		QMessageBox::critical(this,__tr2qs_ctx("Save Current Theme - KVIrc","theme"),szMsg2,
			QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return false;
	}
	// write down the screenshot, if needed

	if(!m_szScreenshotPath.isEmpty())
	{
		if(!KviTheme::saveScreenshots(sto,m_szScreenshotPath))
		{
			QMessageBox::critical(this,__tr2qs_ctx("Save Current Theme - KVIrc","theme"),__tr2qs_ctx("Failed to load the selected screenshot image: please fix it","theme"),
				QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
			setCurrentPage(m_pImageSelectionPage);
			return false;
		}
	}

	QString szMsg = __tr2qs_ctx("Theme saved successfully to ","theme");
	szMsg += sto.absoluteDirectory();

	QMessageBox::information(this,__tr2qs_ctx("Save Theme - KVIrc","theme"),szMsg,QMessageBox::Ok,
		QMessageBox::NoButton,QMessageBox::NoButton);

	return true;
}
