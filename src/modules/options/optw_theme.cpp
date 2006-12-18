//=============================================================================
//
//   File : optw_theme.cpp
//   Creation date : Sun May 25 2003 18:00:00 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2003 Szymon Stefanek (pragma at kvirc dot net)
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

#include "optw_theme.h"
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

#include <stdlib.h> // rand & srand


KviSaveThemeDialog::KviSaveThemeDialog(QWidget * par)
: QDialog(par)
{
	setCaption(__tr2qs_ctx("Save Current Theme","options"));

	QGridLayout * g = new QGridLayout(this,8,2,4,4);

	QLabel * l;

	l = new QLabel(__tr2qs_ctx("Theme name:","options"),this);
	g->addWidget(l,0,0);

	m_pThemeNameEdit = new QLineEdit(this);
	g->addMultiCellWidget(m_pThemeNameEdit,0,0,1,2);
	connect(m_pThemeNameEdit,SIGNAL(textChanged(const QString &)),this,SLOT(themeNameChanged(const QString &)));


	l = new QLabel(__tr2qs_ctx("Version:","options"),this);
	g->addWidget(l,1,0);

	m_pVersionEdit = new QLineEdit(this);
	g->addMultiCellWidget(m_pVersionEdit,1,1,1,2);


	l = new QLabel(__tr2qs_ctx("Author:","options"),this);
	g->addWidget(l,2,0);

	m_pAuthorEdit = new QLineEdit(this);
	g->addMultiCellWidget(m_pAuthorEdit,2,2,1,2);

	l = new QLabel(__tr2qs_ctx("Description:","options"),this);
	g->addWidget(l,3,0);

	m_pDescriptionEdit = new QTextEdit(this);
	g->addMultiCellWidget(m_pDescriptionEdit,3,4,1,2);
	
	m_pIncludeMsgcolors = new KviStyledCheckBox( __tr2qs_ctx("Include message colors information","options"), this );
	m_pIncludeMsgcolors->setChecked(TRUE);
	g->addMultiCellWidget(m_pIncludeMsgcolors,5,5,0,1);
	
	l = new QLabel(__tr2qs_ctx("Package type:","options"),this);
	g->addWidget(l,6,0);

	m_pFormatCombo = new QComboBox(this);
	m_pFormatCombo->insertItem(__tr2qs_ctx("KVIrc Package","options"),0);
//	m_pFormatCombo->insertItem(__tr2qs_ctx("New multi-file format","options"),1);
	m_pFormatCombo->insertItem(__tr2qs_ctx("Dublicate Theme","options"),1);
	g->addWidget(m_pFormatCombo,6,1);

	QHBox * h = new QHBox(this);
	h->setSpacing(7);
	g->addWidget(h,7,2);
	m_pOkButton = new QPushButton(__tr2qs_ctx("OK","options"),h);
	m_pOkButton->setEnabled(false);
	connect(m_pOkButton,SIGNAL(clicked()),this,SLOT(saveTheme()));
	m_pOkButton->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)));

	QPushButton * pb = new QPushButton(__tr2qs_ctx("Cancel","options"),h);
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
			QMessageBox::critical(this,__tr2qs_ctx("Save Theme - KVIrc","options"),__tr2qs_ctx("Unable to create theme directory.","options"),
				QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
			return;
		}
	}
	g_pApp->saveTheme(sto);
	// write down the images

	QString szMsg = __tr2qs_ctx("Theme saved successfully to ","options");
	szMsg += sto.szAbsoluteDirectory;

	QMessageBox::information(this,__tr2qs_ctx("Save Theme - KVIrc","options"),szMsg,QMessageBox::Ok,
		QMessageBox::NoButton,QMessageBox::NoButton);
}

void KviSaveThemeDialog::saveThemeInNewFormat(KviThemeInfo& sto)
{
	QString szBaseDir;
	QFileDialog * d = new QFileDialog(this);
	d->setCaption(__tr2qs_ctx("Choose save directory","options"));
	d->setMode(QFileDialog::DirectoryOnly);
	if(d->exec() == QDialog::Accepted)
	{
		szBaseDir = d->selectedFile();
		KviFileUtils::adjustFilePath(szBaseDir);
		delete d;
		if(szBaseDir.isEmpty())
		{
			QMessageBox::critical(this,__tr2qs_ctx("Save Theme - KVIrc","options"),__tr2qs_ctx("You must select save directory","options"),
				QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
			return;
		}
	} else {
		delete d;
		QMessageBox::critical(this,__tr2qs_ctx("Save Theme - KVIrc","options"),__tr2qs_ctx("You must select save directory","options"),
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
		QMessageBox::critical(this,__tr2qs_ctx("Save Theme - KVIrc","options"),__tr2qs_ctx("Unable to create theme files directory.","options"),
			QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return;
	}
	
	g_pApp->saveTheme(sto);

	if(!KviFileUtils::makeDir(szThemeDir+"utils"))
	{
		QMessageBox::critical(this,__tr2qs_ctx("Save Theme - KVIrc","options"),__tr2qs_ctx("Unable to create theme utils directory.","options"),
			QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return;
	}
	
	QString szInstallerScript;
	g_pApp->getGlobalKvircDirectory(szInstallerScript,KviApp::DefScript,"installer.kvs");
	if(!KviFileUtils::fileExists(szInstallerScript))
	{
		QMessageBox::critical(this,__tr2qs_ctx("Save Theme - KVIrc","options"),__tr2qs_ctx("Cannot find installer.kvs.","options"),
			QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return;
	}
	
	if(!KviFileUtils::copyFile(szInstallerScript,szThemeDir+"utils/installer.kvs"))
	{
		QMessageBox::critical(this,__tr2qs_ctx("Save Theme - KVIrc","options"),__tr2qs_ctx("Cannot write installer.kvs.","options"),
			QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return;
	}
	
	g_pApp->getGlobalKvircDirectory(szInstallerScript,KviApp::DefScript,"theme-install.kvs");
	QString szMainScript=szThemeDir+"install.kvs";
	if(!KviFileUtils::copyFile(szInstallerScript,szMainScript))
	{
		QMessageBox::critical(this,__tr2qs_ctx("Save Theme - KVIrc","options"),__tr2qs_ctx("Cannot write install.kvs.","options"),
			QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return;
	}
	QFile installFile(szMainScript);
	if(!installFile.open(IO_ReadOnly))
	{
		QMessageBox::critical(this,__tr2qs_ctx("Save Theme - KVIrc","options"),__tr2qs_ctx("Cannot open install.kvs for reading.","options"),
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
		QMessageBox::critical(this,__tr2qs_ctx("Save Theme - KVIrc","options"),__tr2qs_ctx("Cannot open install.kvs for writing.","options"),
			QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
		return;
	}
	installFile.writeBlock(szFileContents.data(),szFileContents.length());
	installFile.close();
	
	QString szMsg = __tr2qs_ctx("Theme saved successfully to ","options");
	szMsg += szThemeDir;

	QMessageBox::information(this,__tr2qs_ctx("Save Theme - KVIrc","options"),szMsg,QMessageBox::Ok,
		QMessageBox::NoButton,QMessageBox::NoButton);
}

void KviSaveThemeDialog::saveThemeInXmlFormat(KviThemeInfo& sto)
{
	QString szFilename;
	QFileDialog * d = new QFileDialog(this);
	d->setCaption(__tr2qs_ctx("Choose filaname","options"));
	d->setMode(QFileDialog::AnyFile);
	d->setFilter( "KVIrc Packages (*.kvp)" );
	if(d->exec() == QDialog::Accepted)
	{
		szFilename = d->selectedFile();
		KviFileUtils::adjustFilePath(szFilename);
		delete d;
		if(szFilename.isEmpty())
		{
			QMessageBox::critical(this,__tr2qs_ctx("Save Theme - KVIrc","options"),__tr2qs_ctx("You must select filename","options"),
				QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
			return;
		}
	} else {
		delete d;
		QMessageBox::critical(this,__tr2qs_ctx("Save Theme - KVIrc","options"),__tr2qs_ctx("You must select filename","options"),
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
		QMessageBox::critical(this,__tr2qs_ctx("Save Theme - KVIrc","options"),__tr2qs_ctx("You must choose a theme name!","options"),QMessageBox::Ok,
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

	switch(m_pFormatCombo->currentItem())
	{
		case 0:
			saveThemeInXmlFormat(sto);
			break;
		case 1:
/*			saveThemeInNewFormat(sto);
			break;
		case 2:*/
			saveThemeInOldFormat(sto);
			break;
	}

	accept();
}




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
		t += __tr2qs_ctx("by","options");
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

KviThemeOptionsWidget::KviThemeOptionsWidget(QWidget * parent)
: KviOptionsWidget(parent,"theme_options_widget")
{
	m_pSaveThemeDialog = 0;

	createLayout(2,1);

	QHBox *hb = new QHBox(this);
	layout()->addWidget(hb,0,0);

	KviStyledToolButton * tb = new KviStyledToolButton(hb);
	tb->setIconSet(*(g_pIconManager->getBigIcon(KVI_BIGICON_OPEN)));
	QToolTip::add(tb,__tr2qs_ctx("Install Theme Package","options"));
	connect(tb,SIGNAL(clicked()),this,SLOT(installFromXml()));

	tb = new KviStyledToolButton(hb);
	tb->setIconSet(*(g_pIconManager->getBigIcon(KVI_BIGICON_SAVE)));
	QToolTip::add(tb,__tr2qs_ctx("Save Current Theme...","options"));
	connect(tb,SIGNAL(clicked()),this,SLOT(saveCurrentTheme()));
	
	tb = new KviStyledToolButton(hb);
	tb->setIconSet(*(g_pIconManager->getBigIcon(KVI_BIGICON_REMOVE)));
	QToolTip::add(tb,__tr2qs_ctx("Delete Selected Theme","options"));
	connect(tb,SIGNAL(clicked()),this,SLOT(deleteTheme()));

	tb = new KviStyledToolButton(hb);
	tb->setIconSet(*(g_pIconManager->getBigIcon(KVI_BIGICON_WWW)));
	QToolTip::add(tb,__tr2qs_ctx("Get More Themes...","options"));
	connect(tb,SIGNAL(clicked()),this,SLOT(getMoreThemes()));

	QWidget *w= new QWidget(hb);
	w->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);

	m_pListBox = new QListBox(this);
	m_pListBox->setMinimumHeight(400);
	m_pListBox->setMinimumWidth(400);
	connect(m_pListBox,SIGNAL(doubleClicked ( QListBoxItem * )),this,SLOT(applyTheme ( QListBoxItem * )));
	connect(m_pListBox,SIGNAL(contextMenuRequested(QListBoxItem *,const QPoint &)),
		this,SLOT(contextMenuRequested(QListBoxItem *,const QPoint &)));
	layout()->addWidget(m_pListBox,1,0);

	layout()->setRowStretch(1,0);
	layout()->setColStretch(1,0);

	fillThemeBox();
	m_pContextPopup = new QPopupMenu(this); 
}

KviThemeOptionsWidget::~KviThemeOptionsWidget()
{
	if(m_pSaveThemeDialog)
	{
		delete m_pSaveThemeDialog;
		m_pSaveThemeDialog = 0;
	}
}

void KviThemeOptionsWidget::contextMenuRequested ( QListBoxItem * it, const QPoint & pos )
{
	if(it)
	{
		m_pListBox->setCurrentItem(it);
		m_pContextPopup->clear();
		m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MINUS)),__tr2qs_ctx("&Remove Theme","options"),this,SLOT(deleteTheme()));
		m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)),__tr2qs_ctx("&Apply Theme","options"),this,SLOT(applyCurrentTheme()));
		m_pContextPopup->popup(pos);
	}
}

void KviThemeOptionsWidget::applyTheme ( QListBoxItem * it)
{
	if(it) m_pListBox->setCurrentItem(it);
	applyCurrentTheme();
}

void KviThemeOptionsWidget::applyCurrentTheme()
{
	KviThemeListBoxItem * it = (KviThemeListBoxItem *)m_pListBox->item(m_pListBox->currentItem());
	if(!it)return;

	if(KviMessageBox::yesNo(__tr2qs_ctx("Apply theme - KVIrc","options"),
		__tr2qs_ctx("Do you wish to apply theme \"%Q\" (version %Q)?","options"),
		&(it->themeInfo()->szName),&(it->themeInfo()->szVersion))) {
		
			QString szPath = it->themeInfo()->szAbsoluteDirectory;
			if(szPath.isEmpty())return;

			g_pApp->loadTheme(szPath);
		}
}

void KviThemeOptionsWidget::deleteTheme()
{
	KviThemeListBoxItem* pItem=(KviThemeListBoxItem* )(m_pListBox->selectedItem());
	if(pItem)
	{
		KviThemeInfo * pInfo=pItem->themeInfo();
		QDir d(pInfo->szAbsoluteDirectory);
		KviQString::ensureLastCharIs(pInfo->szAbsoluteDirectory,KVI_PATH_SEPARATOR_CHAR);
		QStringList sl = d.entryList(QDir::Files);
		for(QStringList::Iterator it=sl.begin();it != sl.end();it++)
		{
			KviFileUtils::removeFile(pInfo->szAbsoluteDirectory+*it);
		}
		KviFileUtils::removeDir(pInfo->szAbsoluteDirectory);
		fillThemeBox();
	}
}

void KviThemeOptionsWidget::installFromXml()
{
	QString szFilename;
	QFileDialog * d = new QFileDialog(this);
	d->setCaption(__tr2qs_ctx("Choose filaname","options"));
	d->setMode(QFileDialog::ExistingFile);
	d->setFilter( "KVIrc Packages (*.kvp, *.kvxml)" );
	if(d->exec() == QDialog::Accepted)
	{
		szFilename = d->selectedFile();
		KviFileUtils::adjustFilePath(szFilename);
		delete d;
		if(szFilename.isEmpty())
		{
			QMessageBox::critical(this,__tr2qs_ctx("Open Theme - KVIrc","options"),__tr2qs_ctx("You must select file","options"),
				QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
			return;
		}
	} else {
		delete d;
		return;
	}
	KviXmlAddonLoader * pLoader = new KviXmlAddonLoader(szFilename,this);
	pLoader->exec();
	fillThemeBox();
}

void KviThemeOptionsWidget::getMoreThemes()
{
	if(!g_pFrame)return;
	g_pFrame->executeInternalCommand(KVI_INTERNALCOMMAND_OPENURL_KVIRC_THEMES);
}

void KviThemeOptionsWidget::saveCurrentTheme()
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

void KviThemeOptionsWidget::fillThemeBox(const QString &szDir)
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
		if(g_pApp->loadThemeInfo(szTest,*inf))
		{
			KviThemeListBoxItem * item = 0;
			item = new KviThemeListBoxItem(m_pListBox,inf);
		} else {
			delete inf;
		}
	}
}

void KviThemeOptionsWidget::fillThemeBox()
{
	m_pListBox->clear();

	KviThemeInfo * ti = new KviThemeInfo;
	ti->szName = __tr2qs_ctx("Current Theme","options");

	KviThemeListBoxItem * itewrq = new KviThemeListBoxItem(m_pListBox,ti);

	QString szDir;
	g_pApp->getGlobalKvircDirectory(szDir,KviApp::Themes);
	fillThemeBox(szDir);
	g_pApp->getLocalKvircDirectory(szDir,KviApp::Themes);
	fillThemeBox(szDir);

	m_pListBox->setCurrentItem(itewrq);
}

void KviThemeOptionsWidget::commit()
{
	KviOptionsWidget::commit();
}

#include "m_optw_theme.moc"
