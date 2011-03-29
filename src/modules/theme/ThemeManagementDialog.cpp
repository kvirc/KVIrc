//=============================================================================
//
//   File : ThemeManagementDialog.cpp
//   Creation date : Sat 30 Dec 2006 14:54:56 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2006-2010 Szymon Stefanek <pragma at kvirc dot net>
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
#include "kvi_settings.h"

#if defined(COMPILE_WEBKIT_SUPPORT)
	#include "WebThemeInterfaceDialog.h"
#endif

#include <QVBoxLayout>
#include "ThemeManagementDialog.h"
#include "PackThemeDialog.h"
#include "SaveThemeDialog.h"
#include "ThemeFunctions.h"

#include "KviOptions.h"
#include "KviLocale.h"
#include "KviConfigurationFile.h"
#include "KviFileUtils.h"
#include "KviApplication.h"
#include "KviMainWindow.h"
#include "KviIconManager.h"
#include "KviInternalCommand.h"
#include "KviPackageReader.h"
#include "kvi_fileextensions.h"
#include "KviFileDialog.h"
#include "KviDynamicToolTip.h"
#include "KviIconManager.h"
#include "KviMessageBox.h"
#include "KviTalPopupMenu.h"
#include "KviTalListWidget.h"


#include <QLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QRegExp>
#include <QMessageBox>
#include <QDir>
#include <QDesktopWidget>
#include <QStringList>
#include <QDateTime>
#include <QFileDialog>
#include <QComboBox>
#include <QPainter>
#include <QToolTip>
#include <QImage>
#include <QBuffer>
#include <QCloseEvent>
#include <QSplitter>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

extern QRect g_rectManagementDialogGeometry;

#define BUILTIN_THEMES 1

#define USER_DEFINED_THEMES 0

ThemeListWidgetItem::ThemeListWidgetItem(KviTalListWidget * box,KviThemeInfo * inf)
: KviTalListWidgetItem(box)
{
	m_pThemeInfo = inf;
	QString t;
	t = "<nobr><b>";
	t += inf->name();
	t += "</b>";

	if(!inf->version().isEmpty()) {
		t += "[";
		t += inf->version();
		t += "]";
	}

	if(!inf->author().isEmpty()) {
		t += " <font color=\"#a0a0a0\"> ";
		t += __tr2qs_ctx("by","theme");
		t += " ";
		t += inf->author();
		t += "</font>";
	}

	t += "</nobr>";
	t += "<br><nobr><font size=\"-1\">";
	t += inf->description();
	t += "</font></nobr>";
	setText(t);
}

ThemeListWidgetItem::~ThemeListWidgetItem()
{
	delete m_pThemeInfo;

}

ThemeManagementDialog * ThemeManagementDialog::m_pInstance = 0;


ThemeManagementDialog::ThemeManagementDialog(QWidget * parent)
: QWidget(parent)
{
	m_pItemDelegate = NULL;
#ifdef COMPILE_WEBKIT_SUPPORT
	m_pWebThemeInterfaceDialog = NULL;
#endif
	setObjectName("theme_options_widget");
	setWindowTitle(__tr2qs_ctx("Manage Themes - KVIrc","theme"));
	setWindowIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Theme)));

	m_pInstance = this;

	//QGridLayout * g = new QGridLayout(this);
	QVBoxLayout * vBox = new QVBoxLayout(this);
	KviTalHBox *hb = new KviTalHBox(this);
	hb->setMargin(1);
	hb->setSpacing(1);
	//g->addWidget(hb,0,0);
	vBox->addWidget(hb);
	QToolButton * tb;
	QFrame * sep;

	tb = new QToolButton(hb);
	tb->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_SAVE)));
	tb->setIconSize(QSize(32,32));
	tb->setToolTip(__tr2qs_ctx("Save Current Theme...","theme"));
	connect(tb,SIGNAL(clicked()),this,SLOT(saveCurrentTheme()));

	sep = new QFrame(hb);
	sep->setFrameStyle(QFrame::VLine | QFrame::Sunken);
	sep->setMinimumWidth(12);

	m_pPackThemeButton = new QToolButton(hb);
	m_pPackThemeButton->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_PACK)));

	m_pPackThemeButton->setIconSize(QSize(32,32));
	m_pPackThemeButton->setToolTip(__tr2qs_ctx("Export Selected Themes to a Distributable Package","theme"));
	connect(m_pPackThemeButton,SIGNAL(clicked()),this,SLOT(packTheme()));

	m_pDeleteThemeButton = new QToolButton(hb);
	m_pDeleteThemeButton->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_REMOVE)));
	m_pDeleteThemeButton->setIconSize(QSize(32,32));
	m_pDeleteThemeButton->setToolTip(__tr2qs_ctx("Delete Selected Themes","theme"));
	connect(m_pDeleteThemeButton,SIGNAL(clicked()),this,SLOT(deleteTheme()));

	sep = new QFrame(hb);
	sep->setFrameStyle(QFrame::VLine | QFrame::Sunken);
	sep->setMinimumWidth(12);

	tb = new QToolButton(hb);
	tb->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_OPEN)));
	tb->setIconSize(QSize(32,32));
	tb->setToolTip(__tr2qs_ctx("Install Theme Package From Disk","theme"));
	connect(tb,SIGNAL(clicked()),this,SLOT(installFromFile()));

	tb = new QToolButton(hb);
	tb->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_WWW)));
	tb->setIconSize(QSize(32,32));
	tb->setToolTip(__tr2qs_ctx("Get More Themes...","theme"));
	connect(tb,SIGNAL(clicked()),this,SLOT(getMoreThemes()));

	QWidget *w= new QWidget(hb);
	w->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);

	m_pListWidget = new KviTalListWidget(this);
	m_pListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	m_pItemDelegate = new KviTalIconAndRichTextItemDelegate(m_pListWidget);
	m_pItemDelegate->setDefaultIcon(g_pIconManager->getBigIcon(QString(KVI_BIGICON_THEME)));
	m_pListWidget->setItemDelegate(m_pItemDelegate);
	m_pListWidget->setMinimumHeight(400);
	m_pListWidget->setMinimumWidth(420);

	m_pListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_pListWidget->setSortingEnabled(true);
	connect(m_pListWidget,SIGNAL(itemDoubleClicked(QListWidgetItem *)),this,SLOT(applyTheme(QListWidgetItem *)));


	//FIXME tooltip
	//connect(m_pListWidget,SIGNAL(tipRequest(QListWidgetItem *,const QPoint &)),this,SLOT(tipRequest(QListWidgetItem *,const QPoint &)));
	connect(m_pListWidget,SIGNAL(customContextMenuRequested(const QPoint &)),
		this,SLOT(contextMenuRequested(const QPoint &)));
	connect(m_pListWidget,SIGNAL(itemSelectionChanged()),this,SLOT(enableDisableButtons()));

	sep = new QFrame(this);
	sep->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	sep->setMinimumWidth(300);
	sep->setMinimumHeight(8);
	vBox->addWidget(sep);

	//g->addWidget(sep,2,0);

	m_pCurrentInstalledThemeLabel = new QLabel(this);
	m_pCurrentInstalledThemeLabel->setText(__tr2qs_ctx("<b><u>Current Installed Theme:</u> ","theme")+KVI_OPTION_STRING(KviOption_stringIconThemeSubdir)+"</b>");
//	g->addWidget(pLabel,2,0);
	vBox->addWidget(m_pCurrentInstalledThemeLabel);

	sep = new QFrame(this);
	sep->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	sep->setMinimumWidth(300);

//	g->addWidget(sep,3,0);
	vBox->addWidget(sep);

//	g->addWidget(m_pListWidget,4,0);
	vBox->addWidget(m_pListWidget);

//	KviDynamicToolTip * tip = new KviDynamicToolTip(m_pListWidget);
//	connect(tip,SIGNAL(tipRequest(KviDynamicToolTip *,const QPoint &)),this,SLOT(tipRequest(KviDynamicToolTip *,const QPoint &)));

	QPushButton * b = new QPushButton(__tr2qs("Close"),this);
	b->setMaximumSize(b->sizeHint().width(),b->sizeHint().height());
	connect(b,SIGNAL(clicked()),this,SLOT(closeClicked()));
//	g->addWidget(b,5,0);
	vBox->addWidget(b);

//	g->setMargin(1);
//	g->setSpacing(1);
	vBox->setAlignment(b,Qt::AlignRight);
	fillThemeBox();
	m_pContextPopup = new KviTalPopupMenu(this);

	if(g_rectManagementDialogGeometry.y() < 5)
	{
		g_rectManagementDialogGeometry.setY(5);
	}
	resize(g_rectManagementDialogGeometry.width(),
		g_rectManagementDialogGeometry.height());

	QRect rect = g_pApp->desktop()->screenGeometry(g_pMainWindow);
	move(rect.x() + ((rect.width() - g_rectManagementDialogGeometry.width())/2),rect.y() + ((rect.height() - g_rectManagementDialogGeometry.height())/2));
}

ThemeManagementDialog::~ThemeManagementDialog()
{
	if (m_pItemDelegate) delete m_pItemDelegate;
	g_rectManagementDialogGeometry = QRect(pos().x(),pos().y(),size().width(),size().height());
	m_pInstance = 0;
#ifdef COMPILE_WEBKIT_SUPPORT
	if(m_pWebThemeInterfaceDialog)
	{
		delete m_pWebThemeInterfaceDialog;
		m_pWebThemeInterfaceDialog = NULL;
	}
#endif //COMPILE_WEBKIT_SUPPORT
}

void ThemeManagementDialog::closeClicked()
{
	delete this;
	m_pInstance = 0;
}

void ThemeManagementDialog::display(bool bTopLevel)
{
	if(m_pInstance)
	{
		if(bTopLevel)
		{
			if(m_pInstance->parent())
			{
				m_pInstance->setParent(0);
			}
		} else {
			if(m_pInstance->parent() != g_pMainWindow->splitter())
			{
				m_pInstance->setParent(g_pMainWindow->splitter());
			}
		}
	} else {
		if(bTopLevel)
		{
			m_pInstance = new ThemeManagementDialog(0);
		} else {
			m_pInstance = new ThemeManagementDialog(g_pMainWindow->splitter());
		}
	}
	m_pInstance->show();
	m_pInstance->raise();
	m_pInstance->setFocus();
}

void ThemeManagementDialog::cleanup()
{
	if(!m_pInstance)return;
	delete m_pInstance;
	m_pInstance = 0;
}

void ThemeManagementDialog::packTheme()
{
	KviPointerList<KviThemeInfo> dl;
	dl.setAutoDelete(false);
	QList<QListWidgetItem*> itemsSelected = m_pListWidget->selectedItems ();
	for(int i=0;i<itemsSelected.count();i++)
		dl.append(((ThemeListWidgetItem *)itemsSelected.at(i))->themeInfo());
	if(dl.isEmpty())return;

	PackThemeDialog * pDialog = new PackThemeDialog(this,&dl);
	pDialog->exec();
	delete pDialog;
}

void ThemeManagementDialog::contextMenuRequested(const QPoint & pos)
{
	if(m_pListWidget->itemAt(pos)!=0)
	{
		m_pListWidget->setCurrentItem(m_pListWidget->itemAt(pos));
		m_pContextPopup->clear();
		m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::Minus)),__tr2qs_ctx("&Remove Theme","theme"),this,SLOT(deleteTheme()));
		m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::Accept)),__tr2qs_ctx("&Apply Theme","theme"),this,SLOT(applyCurrentTheme()));
		m_pContextPopup->popup(m_pListWidget->viewport()->mapToGlobal(pos));
	}
}

void ThemeManagementDialog::applyTheme ( QListWidgetItem * it)
{
	if(it)m_pListWidget->setCurrentItem(it);
	applyCurrentTheme();
}

void ThemeManagementDialog::applyCurrentTheme()
{
	ThemeListWidgetItem * it = (ThemeListWidgetItem *)m_pListWidget->currentItem();
	if(!it)return;

	if(KviMessageBox::yesNo(__tr2qs_ctx("Apply theme - KVIrc","theme"),
		__tr2qs_ctx("Do you wish to apply theme \"%Q\" (version %Q)?","theme"),
		&(it->themeInfo()->name()),&(it->themeInfo()->version())))
	{
		QString szPath = it->themeInfo()->dirName();
		if(szPath.isEmpty())return;

		KviThemeInfo out;
		if(!KviTheme::load(szPath,out,it->themeInfo()->isBuiltin()))
		{
			QString szErr = out.lastError();
			QString szMsg = QString(__tr2qs_ctx("Failed to apply the specified theme: %1","theme")).arg(szErr);
			QMessageBox::critical(this,__tr2qs_ctx("Apply theme - KVIrc","theme"),szMsg,
				QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);

		}
		else m_pCurrentInstalledThemeLabel->setText(__tr2qs_ctx("<b><u>Current Installed Theme:</u> ","theme")+KVI_OPTION_STRING(KviOption_stringIconThemeSubdir)+"</b>");
		m_pItemDelegate->setDefaultIcon(g_pIconManager->getBigIcon(QString(KVI_BIGICON_THEME)));
	}
}

void ThemeManagementDialog::deleteTheme()
{
	QList<QListWidgetItem*> itemsSelected = m_pListWidget->selectedItems ();
	for(int i=0;i<itemsSelected.count();i++)
	{
			if(!KviMessageBox::yesNo(__tr2qs_ctx("Delete Theme - KVIrc","theme"),
				__tr2qs_ctx("Do you really wish to delete theme \"%Q\" (version %Q)?","theme"),
					&(((ThemeListWidgetItem *)itemsSelected.at(i))->themeInfo()->name()),&(((ThemeListWidgetItem *)itemsSelected.at(i))->themeInfo()->version())))goto jump_out;
			QString szThemePath;
			((ThemeListWidgetItem *)itemsSelected.at(i))->themeInfo()->getCompleteDirPath(szThemePath);
			KviFileUtils::deleteDir(szThemePath);
	}
jump_out:
	fillThemeBox();
}

void ThemeManagementDialog::installFromFile()
{
	QString szFileName;
	QString szError;

	if(!KviFileDialog::askForOpenFileName(szFileName,__tr2qs_ctx("Open Theme - KVIrc","theme"),QString(),KVI_FILTER_THEME))
		return;
	if(!ThemeFunctions::installThemePackage(szFileName,szError,this))
	{
		KviMessageBox::information(szError);
		return;
	}
	fillThemeBox();
}

void ThemeManagementDialog::getMoreThemes()
{
#ifdef COMPILE_WEBKIT_SUPPORT
	if(m_pWebThemeInterfaceDialog)
	{
		m_pWebThemeInterfaceDialog->show();
	} else {
		m_pWebThemeInterfaceDialog = new WebThemeInterfaceDialog();
		QObject::connect(m_pWebThemeInterfaceDialog,SIGNAL(destroyed()),this,SLOT(webThemeInterfaceDialogDestroyed()));
		m_pWebThemeInterfaceDialog->show();
	}
#else
	if(!g_pMainWindow)
		return;
	g_pMainWindow->executeInternalCommand(KVI_INTERNALCOMMAND_OPENURL_KVIRC_THEMES);
#endif
}

void ThemeManagementDialog::webThemeInterfaceDialogDestroyed()
{
#ifdef COMPILE_WEBKIT_SUPPORT
	m_pWebThemeInterfaceDialog = NULL;
#endif
	fillThemeBox();
}


void ThemeManagementDialog::saveCurrentTheme()
{
	SaveThemeDialog * pSaveThemeDialog = new SaveThemeDialog(this);
	pSaveThemeDialog->exec();
	delete pSaveThemeDialog;
	fillThemeBox();
}

void ThemeManagementDialog::fillThemeBox(bool bBuiltin)
{
	QStringList slThemes;
	KviTheme::installedThemes(slThemes,bBuiltin);

	for(int i=0;i<slThemes.count();i++)
	{
		KviThemeInfo * inf = new KviThemeInfo();
		if(inf->load(slThemes.at(i),bBuiltin))
		{
			inf->setSubdirectory(slThemes.at(i));
			ThemeListWidgetItem *it=new ThemeListWidgetItem(m_pListWidget,inf);
			QPixmap pixmap=inf->smallScreenshot();
			    if(!pixmap.isNull()) it->setIcon(pixmap.scaled(32,32));
		} else {
			delete inf;
		}
	}
}

void ThemeManagementDialog::fillThemeBox()
{
	m_pListWidget->clear();

	fillThemeBox(BUILTIN_THEMES);
	fillThemeBox(USER_DEFINED_THEMES);

	enableDisableButtons();
}

bool ThemeManagementDialog::hasSelectedItems()
{
	QList<QListWidgetItem*> itemsSelected = m_pListWidget->selectedItems ();
	return itemsSelected.count()?true:false;

}

void ThemeManagementDialog::enableDisableButtons()
{
	bool b = hasSelectedItems();
	m_pPackThemeButton->setEnabled(b);
	m_pDeleteThemeButton->setEnabled(b);
}

void ThemeManagementDialog::closeEvent(QCloseEvent * e)
{
	e->ignore();
	delete this;
}

void ThemeManagementDialog::tipRequest(QListWidgetItem *,const QPoint &)
{
	// FIXME
	/*
	//ThemeListWidgetItem * it = (ThemeListWidgetItem *)(m_pListWidget->itemAt(pnt));

	//if(!it)return;

	KviThemeInfo * pThemeInfo = ((ThemeListWidgetItem *)it)->themeInfo();

	QString szThemeDescription;

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
		pThemeInfo->mediumScreenshot(),
		0
	);
	QLabel *wid=new QLabel(0);
	wid->setWindowFlags(Qt::Popup);
	wid->setText("Prova del tooltip");
	wid->move(pnt);
	wid->show();
//	pTip->tip(m_pListWidget->visualItemRect(it),szThemeDescription);
*/
}

