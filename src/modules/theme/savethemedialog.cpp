//=============================================================================
//
//   File : savethemedialog.cpp
//   Created on Wed 03 Jan 2007 03:01:34 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2007 Szymon Stefanek <pragma at kvirc dot net>
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

#include "savethemedialog.h"

#include <qlayout.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qregexp.h>
#include <qmessagebox.h>
#include <qdir.h>
#include <qcombobox.h>
#include <qpainter.h>
#include <qtooltip.h>
#include <qimage.h>
#include <qtextedit.h>
#include <qmultilineedit.h>
#include <qbuffer.h>
#include <qlabel.h>


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
#include "kvi_sourcesdate.h"


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


void KviSaveThemeDialog::saveTheme()
{
	KviThemeInfo sto;
	sto.setName(m_pThemeNameEdit->text());
	if(sto.name().isEmpty())
	{
		QMessageBox::critical(this,__tr2qs_ctx("Save Theme - KVIrc","theme"),__tr2qs_ctx("You must choose a theme name!","theme"),QMessageBox::Ok,
			QMessageBox::NoButton,QMessageBox::NoButton);
		return;
	}

	sto.setAuthor(m_pAuthorEdit->text());
	sto.setDescription(m_pDescriptionEdit->text());
	sto.setDate(QDateTime::currentDateTime().toString());
	sto.setVersion(m_pVersionEdit->text());
	sto.setApplication("KVIrc " KVI_VERSION "." KVI_SOURCES_DATE);
	
	if(sto.version().isEmpty())sto.setVersion("1.0.0");

	QString szSubdir = sto.name() + QString("-") + sto.version();
	szSubdir.replace(QRegExp("[ \\\\/:][ \\\\/:]*"),"_");
	sto.setSubdirectory(szSubdir);

	QString szAbsDir;
	g_pApp->getLocalKvircDirectory(szAbsDir,KviApp::Themes,sto.subdirectory(),true);
	if(!KviFileUtils::makeDir(szAbsDir))
	{
		QMessageBox::critical(this,__tr2qs_ctx("Save Theme - KVIrc","theme"),__tr2qs_ctx("Unable to create theme directory.","theme"),
			QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return;
	}
	
	sto.setAbsoluteDirectory(szAbsDir);
	
	if(!KviTheme::save(sto))
	{
		QString szMsg2;
		QString szErr = sto.lastError();
		KviQString::sprintf(szMsg2,__tr2qs_ctx("Unable to save theme: %Q","theme"),&szErr);
		QMessageBox::critical(this,__tr2qs_ctx("Save Theme - KVIrc","theme"),szMsg2,
			QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return;
	}
	// write down the images

	QString szMsg = __tr2qs_ctx("Theme saved successfully to ","theme");
	szMsg += sto.absoluteDirectory();

	QMessageBox::information(this,__tr2qs_ctx("Save Theme - KVIrc","theme"),szMsg,QMessageBox::Ok,
		QMessageBox::NoButton,QMessageBox::NoButton);

	accept();
}

