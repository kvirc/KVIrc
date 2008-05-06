//=============================================================================
//
//   File : managementdialog.cpp
//   Created on Fri 08 Apr 2005 14:54:56 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2005 Szymon Stefanek <pragma at kvirc dot net>
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

#include "managementdialog.h"

#include "kvi_app.h"
#include "kvi_locale.h"
#include "kvi_frame.h"
#include "kvi_iconmanager.h"
#include "kvi_kvs_scriptaddonmanager.h"
#include "kvi_window.h"
#include "kvi_filedialog.h"
#include "kvi_fileutils.h"
#include "kvi_kvs_script.h"
#include "kvi_sourcesdate.h"
#include "kvi_draganddrop.h"
#include "kvi_tal_listwidget.h"

#include <QPainter>
#include <QPixmap>
#include <QMessageBox>
#include <QEvent>
#include <QCloseEvent>
#include <QPushButton>
#include <QToolButton>
#include <QLayout>
#include <QApplication>
#include <QToolTip>
#include <QLineEdit>
#include <QLabel>
#include <QFrame>
#include <QAbstractTextDocumentLayout>




KviScriptManagementDialog * KviScriptManagementDialog::m_pInstance = 0;
extern QRect g_rectManagementDialogGeometry;

KviScriptAddonListViewItem::KviScriptAddonListViewItem(KviTalListWidget *v,KviKvsScriptAddon * a)
: KviTalListWidgetItem(v)
{
	m_pAddon = new KviKvsScriptAddon(*a);
	m_pListWidget = v;
	QString t = "<nobr><b>";
	t += a->visibleName();
	t += "</b> [";
	t += a->version();
	t += "]";
	t += " <font color=\"#a0a0a0\">[";
	t += a->name();
	t += "]</font></nobr>";
	t += "<br><nobr><font size=\"-1\">";
	t += a->description();
	t += "</font></nobr>";

	setText(t);
	QPixmap * p = a->icon();
	if (p) setIcon(*p); 
}

KviScriptAddonListViewItem::~KviScriptAddonListViewItem()
{
	delete m_pAddon;
}


KviScriptManagementDialog::KviScriptManagementDialog(QWidget * p)
: QDialog(p/*,WType_TopLevel | WStyle_Customize | WStyle_Title | WStyle_StaysOnTop | WStyle_DialogBorder*/)
{
	/* old manager
	setWindowTitle(__tr2qs("Manage Script-Based Addons"));
	setObjectName("Addon manager");
	setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ADDONS)));
	setModal(true);
	m_pInstance = this;
	QGridLayout * g = new QGridLayout(this);
	
	//	QPixmap * pix = g_pIconManager->getImage("kvi_dialog_addons.png");
	m_pListWidget = new KviTalListWidget(this);
	KviTalIconAndRichTextItemDelegate *itemDelegate=new KviTalIconAndRichTextItemDelegate(m_pListWidget);

	m_pListWidget->setItemDelegate(itemDelegate);
	m_pListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pListWidget->setSortingEnabled(true);
	m_pListWidget->setMinimumHeight(400);
	m_pListWidget->setMinimumWidth(400);

	QString szPic;
	g_pApp->getGlobalKvircDirectory(szPic,KviApp::Pics);
	// FIXME
	szPic += "/kvi_dialog_addons.png";
	QString style("QListWidget {background-image: url(" + szPic + ");}");
	debug("pic: %s",szPic.toUtf8().data());
	debug("style: %s",style.toUtf8().data());
	m_pListWidget->setStyleSheet(style);
	g->addMultiCellWidget(m_pListWidget,0,10,1,1);



	fillListView();
	
	currentChanged(0,0);
	connect(m_pListWidget,SIGNAL(currentItemChanged(QListWidgetItem *,QListWidgetItem *)),this,SLOT(currentChanged(QListWidgetItem *,QListWidgetItem *)));
	m_pListWidget->setCurrentItem(m_pListWidget->item(0));
	if(g_rectManagementDialogGeometry.y() < 5)
	{
		g_rectManagementDialogGeometry.setY(5);
	}
	resize(g_rectManagementDialogGeometry.width(),
		g_rectManagementDialogGeometry.height());
	move(g_rectManagementDialogGeometry.x(),
		g_rectManagementDialogGeometry.y());
	*/

	/* new manager */
	setWindowTitle(__tr2qs("Manage Script-Based Addons"));
	setObjectName("Addon manager");
	setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ADDONS)));
	setModal(true);
	m_pInstance = this;
	QGridLayout * g = new QGridLayout(this);

	KviTalHBox *hb = new KviTalHBox(this);
	hb->setMargin(1);
	hb->setSpacing(1);
	g->addWidget(hb,0,0);

	QToolButton * tb;
	QFrame * sep;

	m_pConfigureButton = new QToolButton(hb);
	m_pConfigureButton->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_ADDONS)));
	m_pConfigureButton->setIconSize(QSize(32,32));
	QToolTip::add(m_pConfigureButton,__tr2qs_ctx("Configure Addon...","addon"));
	connect(m_pConfigureButton,SIGNAL(clicked()),this,SLOT(configureScript()));

	m_pHelpButton = new QToolButton(hb);
	m_pHelpButton->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_HELP)));
	m_pHelpButton->setIconSize(QSize(32,32));
	QToolTip::add(m_pHelpButton,__tr2qs_ctx("Show Help","addon"));
	connect(m_pHelpButton,SIGNAL(clicked()),this,SLOT(showScriptHelp()));

	sep = new QFrame(hb);
	sep->setFrameStyle(QFrame::VLine | QFrame::Sunken);
	sep->setMinimumWidth(12);
	
	m_pPackButton = new QToolButton(hb);
	m_pPackButton->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_PACK)));
	m_pPackButton->setIconSize(QSize(32,32));
	QToolTip::add(m_pPackButton,__tr2qs_ctx("Export Selected Themes to a Distributable Package","theme"));
	connect(m_pPackButton,SIGNAL(clicked()),this,SLOT(packScript()));

	m_pUninstallButton = new QToolButton(hb);
	m_pUninstallButton->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_REMOVE)));
	m_pUninstallButton->setIconSize(QSize(32,32));
	QToolTip::add(m_pUninstallButton,__tr2qs_ctx("Delete Selected Themes","theme"));
	connect(m_pUninstallButton,SIGNAL(clicked()),this,SLOT(uninstallScript()));

	sep = new QFrame(hb);
	sep->setFrameStyle(QFrame::VLine | QFrame::Sunken);
	sep->setMinimumWidth(12);

	tb = new QToolButton(hb);
	tb->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_OPEN)));
	tb->setIconSize(QSize(32,32));
	QToolTip::add(tb,__tr2qs_ctx("Install Theme Package From Disk","theme"));
	connect(tb,SIGNAL(clicked()),this,SLOT(installScript()));

	tb = new QToolButton(hb);
	tb->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_WWW)));
	tb->setIconSize(QSize(32,32));
	QToolTip::add(tb,__tr2qs_ctx("Get More Themes...","theme"));
	connect(tb,SIGNAL(clicked()),this,SLOT(getMoreScripts()));

	QWidget *w= new QWidget(hb);
	w->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);

	//	QPixmap * pix = g_pIconManager->getImage("kvi_dialog_addons.png");
	m_pListWidget = new KviTalListWidget(this);
	KviTalIconAndRichTextItemDelegate *itemDelegate=new KviTalIconAndRichTextItemDelegate(m_pListWidget);

	m_pListWidget->setItemDelegate(itemDelegate);
	m_pListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pListWidget->setSortingEnabled(true);
	m_pListWidget->setMinimumHeight(400);
	m_pListWidget->setMinimumWidth(400);

	QString szPic;
	g_pApp->getGlobalKvircDirectory(szPic,KviApp::Pics);
	szPic += "/kvi_dialog_addons.png";
	QString style("QListWidget {background-image: url(" + szPic + ");}");
	m_pListWidget->setStyleSheet(style);
	g->addWidget(m_pListWidget,1,0);

	fillListView();
	
	currentChanged(0,0);
	connect(m_pListWidget,SIGNAL(currentItemChanged(QListWidgetItem *,QListWidgetItem *)),this,SLOT(currentChanged(QListWidgetItem *,QListWidgetItem *)));
	m_pListWidget->setCurrentItem(m_pListWidget->item(0));
	if(g_rectManagementDialogGeometry.y() < 5)
	{
		g_rectManagementDialogGeometry.setY(5);
	}
	resize(g_rectManagementDialogGeometry.width(),
		g_rectManagementDialogGeometry.height());
	move(g_rectManagementDialogGeometry.x(),
		g_rectManagementDialogGeometry.y());
}

KviScriptManagementDialog::~KviScriptManagementDialog()
{
	g_rectManagementDialogGeometry = QRect(pos().x(),pos().y(),size().width(),size().height());
	m_pInstance = 0;
}

void KviScriptManagementDialog::fillListView()
{
	m_pListWidget->clear();
	KviPointerHashTable<QString,KviKvsScriptAddon> * d = KviKvsScriptAddonManager::instance()->addonDict();
	if(!d)return;
	KviPointerHashTableIterator<QString,KviKvsScriptAddon> it(*d);
	KviScriptAddonListViewItem * item;
	while(KviKvsScriptAddon * a = it.current())
	{
		item = new KviScriptAddonListViewItem(m_pListWidget,a);
		++it;
	}
}

void KviScriptManagementDialog::currentChanged(QListWidgetItem *item,QListWidgetItem *)
{
	KviScriptAddonListViewItem * it = (KviScriptAddonListViewItem *)item;
	if(!it)
	{
		m_pConfigureButton->setEnabled(false);
		m_pUninstallButton->setEnabled(false);
		m_pHelpButton->setEnabled(false);
		m_pPackButton->setEnabled(false);
	} else {
		m_pConfigureButton->setEnabled(!(it->addon()->configureCallbackCode().isEmpty()));
		m_pHelpButton->setEnabled(!(it->addon()->helpCallbackCode().isEmpty()));
		m_pUninstallButton->setEnabled(true);
	}
}

void KviScriptManagementDialog::showScriptHelp()
{
	KviScriptAddonListViewItem * it = (KviScriptAddonListViewItem *)m_pListWidget->currentItem();
	if(!it)return;
	if(it->addon()->helpCallbackCode().isEmpty())return;
	it->addon()->executeHelpCallback(g_pActiveWindow);
}

void KviScriptManagementDialog::configureScript()
{
	KviScriptAddonListViewItem * it = (KviScriptAddonListViewItem *)m_pListWidget->currentItem();
	if(!it)return;
	if(it->addon()->configureCallbackCode().isEmpty())return;
	it->addon()->executeConfigureCallback(g_pActiveWindow);
}

void KviScriptManagementDialog::packScript()
{
	/*
	KviPointerList<KviThemeInfo> dl;
	dl.setAutoDelete(false);
	QList<QListWidgetItem*> itemsSelected = m_pListWidget->selectedItems ();
	for(int i=0;i<itemsSelected.count();i++)
		dl.append(((KviThemeListBoxItem *)itemsSelected.at(i))->themeInfo());
	if(dl.isEmpty())return;

	KviPackAddonDialog * pDialog = new KviPackAddonDialog(this,&dl);
	pDialog->exec();
	delete pDialog;
	*/
}

void KviScriptManagementDialog::uninstallScript()
{
	KviScriptAddonListViewItem * it = (KviScriptAddonListViewItem *)m_pListWidget->currentItem();
	if(!it)return;

	QString txt = "<p>";
	txt += __tr2qs("Do you really want to uninstall the addon \"%1\" ?").arg(it->addon()->visibleName());
	txt += "</p>";
	
	if(QMessageBox::question(this,
		__tr2qs("Confirm addon uninstallation"),txt,__tr2qs("&Yes"),__tr2qs("&No"),0,1) != 0)return;

	KviKvsScriptAddonManager::instance()->unregisterAddon(it->addon()->name(),g_pActiveWindow);
	
	fillListView();
	currentChanged(0,0);
}

void KviScriptManagementDialog::getMoreScripts()
{
	KviKvsScript::run("openurl http://www.kvirc.net/?id=addons&version=" KVI_VERSION "." KVI_SOURCES_DATE,g_pActiveWindow);
}

void KviScriptManagementDialog::installScript()
{
	QString buffer;

	if(!KviFileDialog::askForOpenFileName(buffer,__tr2qs("Please select the addon installation file"),QString::null,"install.kvs",false,true))return;

	buffer.replace("\\","\\\\");

	QString szCmd = "parse \"";
	szCmd += buffer;
	szCmd += "\"";

	KviKvsScript::run(szCmd,g_pActiveWindow);

	fillListView();
	currentChanged(0,0);

	//m_pListWidget->publicUpdateContents();
	//m_pListWidget->triggerUpdate();
}

void KviScriptManagementDialog::showEvent(QShowEvent * e)
{
//	QRect r = parentWidget() ? parentWidget()->rect() : QApplication::desktop()->rect();
//	int x = (r.width() - width()) / 2;
//	int y = (r.height() - height()) / 2;
//	move(x,y);
}

void KviScriptManagementDialog::closeClicked()
{
	delete this;
}

void KviScriptManagementDialog::cleanup()
{
	if(!m_pInstance)return;
	delete m_pInstance;
	m_pInstance = 0;
}

void KviScriptManagementDialog::display()
{
	if(m_pInstance)return;
	m_pInstance = new KviScriptManagementDialog(g_pFrame);
	m_pInstance->show();
}

void KviScriptManagementDialog::closeEvent(QCloseEvent * e)
{
	e->ignore();
	delete this;
}
