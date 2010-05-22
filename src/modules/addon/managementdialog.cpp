//=============================================================================
//
//   File : managementdialog.cpp
//   Creation date : Fri 08 Apr 2005 14:54:56 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2005-2008 Szymon Stefanek <pragma at kvirc dot net>
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

#include "managementdialog.h"
#include "packaddondialog.h"
#include "addonfunctions.h"

#include "kvi_app.h"
#include "kvi_locale.h"
#include "kvi_frame.h"
#include "kvi_iconmanager.h"
#include "kvi_kvs_scriptaddonmanager.h"
#include "kvi_window.h"
#include "kvi_filedialog.h"
#include "kvi_fileutils.h"
#include "kvi_fileextensions.h"
#include "kvi_kvs_script.h"
#include "kvi_sourcesdate.h"
#include "kvi_tal_listwidget.h"
#include "kvi_tal_tooltip.h"

#include <QPainter>
#include <QPixmap>
#include <QMessageBox>
#include <QEvent>
#include <QCloseEvent>
#include <QPushButton>
#include <QLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QLineEdit>
#include <QLabel>
#include <QFrame>
#include <QDir>
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
: QDialog(p)
{
	setWindowTitle(__tr2qs_ctx("Manage Script-Based Addons","addon"));
	setObjectName("Addon manager");
	setWindowIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ADDONS)));

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
	KviTalToolTip::add(m_pConfigureButton,__tr2qs_ctx("Configure Addon...","addon"));
	connect(m_pConfigureButton,SIGNAL(clicked()),this,SLOT(configureScript()));

	m_pHelpButton = new QToolButton(hb);
	m_pHelpButton->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_HELP)));
	m_pHelpButton->setIconSize(QSize(32,32));
	KviTalToolTip::add(m_pHelpButton,__tr2qs_ctx("Show Help","addon"));
	connect(m_pHelpButton,SIGNAL(clicked()),this,SLOT(showScriptHelp()));

	m_pUninstallButton = new QToolButton(hb);
	m_pUninstallButton->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_REMOVE)));
	m_pUninstallButton->setIconSize(QSize(32,32));
	KviTalToolTip::add(m_pUninstallButton,__tr2qs_ctx("Delete Selected Addons","addon"));
	connect(m_pUninstallButton,SIGNAL(clicked()),this,SLOT(uninstallScript()));

	sep = new QFrame(hb);
	sep->setFrameStyle(QFrame::VLine | QFrame::Sunken);
	sep->setMinimumWidth(12);

	m_pPackButton = new QToolButton(hb);
	m_pPackButton->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_PACK)));
	m_pPackButton->setIconSize(QSize(32,32));
	KviTalToolTip::add(m_pPackButton,__tr2qs_ctx("Create an Addon as a Distributable Package","addon"));
	connect(m_pPackButton,SIGNAL(clicked()),this,SLOT(packScript()));

	sep = new QFrame(hb);
	sep->setFrameStyle(QFrame::VLine | QFrame::Sunken);
	sep->setMinimumWidth(12);

	tb = new QToolButton(hb);
	tb->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_OPEN)));
	tb->setIconSize(QSize(32,32));
	KviTalToolTip::add(tb,__tr2qs_ctx("Install Addon Package From Disk","addon"));
	connect(tb,SIGNAL(clicked()),this,SLOT(installScript()));

	tb = new QToolButton(hb);
	tb->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_WWW)));
	tb->setIconSize(QSize(32,32));
	KviTalToolTip::add(tb,__tr2qs_ctx("Get More Addons...","addon"));
	connect(tb,SIGNAL(clicked()),this,SLOT(getMoreScripts()));

	QWidget *w= new QWidget(hb);
	w->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);

	m_pListWidget = new KviTalListWidget(this);
	KviTalIconAndRichTextItemDelegate *itemDelegate=new KviTalIconAndRichTextItemDelegate(m_pListWidget);

	m_pListWidget->setItemDelegate(itemDelegate);
	m_pListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pListWidget->setSortingEnabled(true);
	m_pListWidget->setMinimumHeight(400);
	m_pListWidget->setMinimumWidth(380);
	g->addWidget(m_pListWidget,1,0);

	fillListView();

	currentChanged(0,0);
	connect(m_pListWidget,SIGNAL(currentItemChanged(QListWidgetItem *,QListWidgetItem *)),this,SLOT(currentChanged(QListWidgetItem *,QListWidgetItem *)));
	m_pListWidget->setCurrentItem(m_pListWidget->item(0));

	QPushButton * pCloseBtn = new QPushButton(__tr2qs_ctx("Close","addon"),this);
	pCloseBtn->setMaximumSize(pCloseBtn->sizeHint().width(),pCloseBtn->sizeHint().height());
	connect(pCloseBtn,SIGNAL(clicked()),this,SLOT(closeClicked()));
	g->addWidget(pCloseBtn,2,0);

	g->setMargin(5);
	g->setSpacing(5);
	g->setAlignment(pCloseBtn,Qt::AlignRight);

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
	KviPackAddonDialog * pDialog = new KviPackAddonDialog(this);
	pDialog->exec();
	delete pDialog;
}

void KviScriptManagementDialog::uninstallScript()
{
	KviScriptAddonListViewItem * it = (KviScriptAddonListViewItem *)m_pListWidget->currentItem();
	if(!it)return;

	QString txt = "<p>";
	txt += __tr2qs_ctx("Do you really want to uninstall the addon \"%1\" ?","addon").arg(it->addon()->visibleName());
	txt += "</p>";

	if(QMessageBox::question(
		this,
		__tr2qs_ctx("Confirm addon uninstallation","addon"),
		txt, __tr2qs_ctx("Yes","addon"), __tr2qs_ctx("No","addon"),0,1
		) != 0) return;

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
	QString szFileName, szError;

	if(!KviFileDialog::askForOpenFileName(
		szFileName,
		__tr2qs_ctx("Please select the addon installation file","addon"),
		QString(),KVI_FILTER_ADDON,false,true
		)) return;

	szFileName.replace("\\","\\\\");

	// Sanity check
	if(szFileName.endsWith(".kva"))
	{
		if(!KviAddonFunctions::installAddonPackage(szFileName,szError,this))
		{
			QMessageBox::critical(
				this,
				__tr2qs_ctx("Install Addon - KVIrc","addon"),
				szError,
				QMessageBox::Ok,
				QMessageBox::NoButton,
				QMessageBox::NoButton
			);
			return;
		}
	} else {
		// Just for sanity check. We should NEVER enter here
		qDebug("Entered sanity check");
		KviAddonFunctions::notAValidAddonPackage(szError);
		QMessageBox::critical(
			this,
			__tr2qs_ctx("Install Addon - KVIrc","addon"),
			szError,
			QMessageBox::Ok,
			QMessageBox::NoButton,
			QMessageBox::NoButton
		);
	}

	fillListView();
	currentChanged(0,0);

	//m_pListWidget->publicUpdateContents();
	//m_pListWidget->triggerUpdate();
}

void KviScriptManagementDialog::showEvent(QShowEvent * e)
{
	QRect rect = g_pApp->desktop()->screenGeometry(g_pApp->desktop()->primaryScreen());
	move((rect.width() - width())/2,(rect.height() - height())/2);

	QDialog::showEvent(e);
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

void KviScriptManagementDialog::reject()
{
	cleanup();
}
