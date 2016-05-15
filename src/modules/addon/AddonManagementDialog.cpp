//=============================================================================
//
//   File : AddonManagementDialog.cpp
//   Creation date : Fri 08 Apr 2005 14:54:56 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2005-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "AddonManagementDialog.h"
#include "PackAddonDialog.h"
#include "AddonFunctions.h"

#include "KviApplication.h"
#include "KviLocale.h"
#include "KviMainWindow.h"
#include "KviIconManager.h"
#include "KviKvsScriptAddonManager.h"
#include "KviWindow.h"
#include "KviFileDialog.h"
#include "KviFileUtils.h"
#include "kvi_fileextensions.h"
#include "KviKvsScript.h"
#include "kvi_sourcesdate.h"
#include "KviTalListWidget.h"
#include "KviTalToolTip.h"

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
#include <QShortcut>

#ifdef COMPILE_WEBKIT_SUPPORT
#include "WebAddonInterfaceDialog.h"
#endif //COMPILE_WEBKIT_SUPPORT

AddonManagementDialog * AddonManagementDialog::m_pInstance = nullptr;
extern QRect g_rectManagementDialogGeometry;

AddonListViewItem::AddonListViewItem(KviTalListWidget * v, KviKvsScriptAddon * a)
    : KviTalListWidgetItem(v)
{
	m_pAddon = new KviKvsScriptAddon(*a);
	m_pListWidget = v;
	QString t = "<b>";
	t += a->visibleName();
	t += "</b> [";
	t += a->version();
	t += "]";
	t += " <font color=\"#454545\">[";
	t += a->name();
	t += "]";
	t += "<br><font size=\"-1\">";
	t += a->description();
	t += "</font>";

	setText(t);
	QPixmap * p = a->icon();
	if(p)
		setIcon(*p);
}

AddonListViewItem::~AddonListViewItem()
{
	delete m_pAddon;
}

AddonManagementDialog::AddonManagementDialog(QWidget * p)
    : QWidget(p)
{
	setWindowTitle(__tr2qs_ctx("Manage Addons - KVIrc", "addon"));
	setObjectName("Addon manager");
	setWindowIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Addons)));

#ifdef COMPILE_WEBKIT_SUPPORT
	m_pWebInterfaceDialog = nullptr;
#endif //COMPILE_WEBKIT_SUPPORT

	m_pInstance = this;
	QGridLayout * g = new QGridLayout(this);

	KviTalHBox * hb = new KviTalHBox(this);
	hb->setMargin(1);
	hb->setSpacing(1);
	g->addWidget(hb, 0, 0);

	QToolButton * tb;
	QFrame * sep;

	m_pConfigureButton = new QToolButton(hb);
	m_pConfigureButton->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_ADDONS)));
	m_pConfigureButton->setIconSize(QSize(32, 32));
	KviTalToolTip::add(m_pConfigureButton, __tr2qs_ctx("Configure addon...", "addon"));
	connect(m_pConfigureButton, SIGNAL(clicked()), this, SLOT(configureScript()));

	m_pHelpButton = new QToolButton(hb);
	m_pHelpButton->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_HELP)));
	m_pHelpButton->setIconSize(QSize(32, 32));
	KviTalToolTip::add(m_pHelpButton, __tr2qs_ctx("Show help", "addon"));
	connect(m_pHelpButton, SIGNAL(clicked()), this, SLOT(showScriptHelp()));

	m_pUninstallButton = new QToolButton(hb);
	m_pUninstallButton->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_REMOVE)));
	m_pUninstallButton->setIconSize(QSize(32, 32));
	KviTalToolTip::add(m_pUninstallButton, __tr2qs_ctx("Delete selected addon", "addon"));
	connect(m_pUninstallButton, SIGNAL(clicked()), this, SLOT(uninstallScript()));

	sep = new QFrame(hb);
	sep->setFrameStyle(QFrame::VLine | QFrame::Sunken);
	sep->setMinimumWidth(12);

	m_pPackButton = new QToolButton(hb);
	m_pPackButton->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_PACK)));
	m_pPackButton->setIconSize(QSize(32, 32));
	KviTalToolTip::add(m_pPackButton, __tr2qs_ctx("Create an addon as a distributable package", "addon"));
	connect(m_pPackButton, SIGNAL(clicked()), this, SLOT(packScript()));

	sep = new QFrame(hb);
	sep->setFrameStyle(QFrame::VLine | QFrame::Sunken);
	sep->setMinimumWidth(12);

	tb = new QToolButton(hb);
	tb->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_OPEN)));
	tb->setIconSize(QSize(32, 32));
	KviTalToolTip::add(tb, __tr2qs_ctx("Install addon package from disk", "addon"));
	connect(tb, SIGNAL(clicked()), this, SLOT(installScript()));

	tb = new QToolButton(hb);
	tb->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_WWW)));
	tb->setIconSize(QSize(32, 32));
	KviTalToolTip::add(tb, __tr2qs_ctx("Get more addons...", "addon"));
	connect(tb, SIGNAL(clicked()), this, SLOT(getMoreScripts()));

	QWidget * w = new QWidget(hb);
	w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

	m_pListWidget = new KviTalListWidget(this);
	KviTalIconAndRichTextItemDelegate * itemDelegate = new KviTalIconAndRichTextItemDelegate(m_pListWidget);

	m_pListWidget->setItemDelegate(itemDelegate);
	m_pListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pListWidget->setSortingEnabled(true);
	m_pListWidget->setMinimumHeight(400);
	m_pListWidget->setMinimumWidth(380);
	g->addWidget(m_pListWidget, 1, 0);

	fillListView();

	currentChanged(nullptr, nullptr);
	connect(m_pListWidget, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(currentChanged(QListWidgetItem *, QListWidgetItem *)));
	m_pListWidget->setCurrentItem(m_pListWidget->item(0));

	QPushButton * pCloseBtn = new QPushButton(__tr2qs_ctx("Close", "addon"), this);
	pCloseBtn->setMaximumSize(pCloseBtn->sizeHint().width(), pCloseBtn->sizeHint().height());
	connect(pCloseBtn, SIGNAL(clicked()), this, SLOT(closeClicked()));
	g->addWidget(pCloseBtn, 2, 0);

	g->setMargin(5);
	g->setSpacing(5);
	g->setAlignment(pCloseBtn, Qt::AlignRight);

	if(g_rectManagementDialogGeometry.y() < 5)
	{
		g_rectManagementDialogGeometry.setY(5);
	}
	resize(g_rectManagementDialogGeometry.width(),
	    g_rectManagementDialogGeometry.height());

	QRect rect = g_pApp->desktop()->screenGeometry(g_pMainWindow);
	move(rect.x() + ((rect.width() - g_rectManagementDialogGeometry.width()) / 2), rect.y() + ((rect.height() - g_rectManagementDialogGeometry.height()) / 2));

	new QShortcut(Qt::Key_Escape, this, SLOT(closeClicked()));
}

AddonManagementDialog::~AddonManagementDialog()
{
#ifdef COMPILE_WEBKIT_SUPPORT
	if(m_pWebInterfaceDialog)
		delete m_pWebInterfaceDialog;
#endif //COMPILE_WEBKIT_SUPPORT
	g_rectManagementDialogGeometry = QRect(pos().x(), pos().y(), size().width(), size().height());
	m_pInstance = nullptr;
}

void AddonManagementDialog::fillListView()
{
	m_pListWidget->clear();
	KviPointerHashTable<QString, KviKvsScriptAddon> * d = KviKvsScriptAddonManager::instance()->addonDict();
	if(!d)
		return;
	KviPointerHashTableIterator<QString, KviKvsScriptAddon> it(*d);

	while(KviKvsScriptAddon * a = it.current())
	{
		new AddonListViewItem(m_pListWidget, a);
		++it;
	}
}

void AddonManagementDialog::currentChanged(QListWidgetItem * item, QListWidgetItem *)
{
	AddonListViewItem * it = (AddonListViewItem *)item;
	if(!it)
	{
		m_pConfigureButton->setEnabled(false);
		m_pUninstallButton->setEnabled(false);
		m_pHelpButton->setEnabled(false);
	}
	else
	{
		m_pConfigureButton->setEnabled(!(it->addon()->configureCallbackCode().isEmpty()));
		m_pHelpButton->setEnabled(!(it->addon()->helpCallbackCode().isEmpty()));
		m_pUninstallButton->setEnabled(true);
	}
}

void AddonManagementDialog::showScriptHelp()
{
	AddonListViewItem * it = (AddonListViewItem *)m_pListWidget->currentItem();
	if(!it)
		return;
	if(it->addon()->helpCallbackCode().isEmpty())
		return;
	it->addon()->executeHelpCallback(g_pActiveWindow);
}

void AddonManagementDialog::configureScript()
{
	AddonListViewItem * it = (AddonListViewItem *)m_pListWidget->currentItem();
	if(!it)
		return;
	if(it->addon()->configureCallbackCode().isEmpty())
		return;
	it->addon()->executeConfigureCallback(g_pActiveWindow);
}

void AddonManagementDialog::packScript()
{
	PackAddonDialog * pDialog = new PackAddonDialog(this);
	pDialog->exec();
	delete pDialog;
}

void AddonManagementDialog::uninstallScript()
{
	AddonListViewItem * it = (AddonListViewItem *)m_pListWidget->currentItem();
	if(!it)
		return;

	QString txt = "<p>";
	txt += __tr2qs_ctx("Do you really want to uninstall the addon \"%1\"?", "addon").arg(it->addon()->visibleName());
	txt += "</p>";

	if(QMessageBox::question(
	       this,
	       __tr2qs_ctx("Confirm Addon Uninstallation - KVIrc", "addon"),
	       txt, __tr2qs_ctx("Yes", "addon"), __tr2qs_ctx("No", "addon"), nullptr, 1)
	    != 0)
		return;

	KviKvsScriptAddonManager::instance()->unregisterAddon(it->addon()->name(), g_pActiveWindow);

	fillListView();
	currentChanged(nullptr, nullptr);
}

void AddonManagementDialog::getMoreScripts()
{
#ifdef COMPILE_WEBKIT_SUPPORT
	if(m_pWebInterfaceDialog)
		return;
	m_pWebInterfaceDialog = new WebAddonInterfaceDialog();
#else  //!COMPILE_WEBKIT_SUPPORT
	// If change this introducing not-fixed text, remember to escape this using KviQString::escapeKvs()!
	KviKvsScript::run("openurl http://www.kvirc.net/?id=addons&version=" KVI_VERSION "." KVI_SOURCES_DATE, g_pActiveWindow);
#endif //!COMPILE_WEBKIT_SUPPORT
}

void AddonManagementDialog::installScript()
{
	QString szFileName, szError;

	if(!KviFileDialog::askForOpenFileName(
	       szFileName,
	       __tr2qs_ctx("Select a Installation File - KVIrc", "addon"),
	       QString(), KVI_FILTER_ADDON, false, true, this))
		return;

	szFileName.replace("\\", "\\\\");

	// Sanity check
	if(szFileName.endsWith(".kva"))
	{
		if(!AddonFunctions::installAddonPackage(szFileName, szError, this))
		{
			QMessageBox::critical(
			    this,
			    __tr2qs_ctx("Install Addon - KVIrc", "addon"),
			    szError,
			    QMessageBox::Ok,
			    QMessageBox::NoButton,
			    QMessageBox::NoButton);
			return;
		}
	}
	else
	{
		// Just for sanity check. We should NEVER enter here
		qDebug("Entered sanity check");
		AddonFunctions::notAValidAddonPackage(szError);
		QMessageBox::critical(
		    this,
		    __tr2qs_ctx("Install Addon - KVIrc", "addon"),
		    szError,
		    QMessageBox::Ok,
		    QMessageBox::NoButton,
		    QMessageBox::NoButton);
	}

	fillListView();
	currentChanged(nullptr, nullptr);

	//m_pListWidget->publicUpdateContents();
	//m_pListWidget->triggerUpdate();
}

void AddonManagementDialog::closeClicked()
{
	delete this;
}

void AddonManagementDialog::cleanup()
{
	if(!m_pInstance)
		return;
	delete m_pInstance;
	m_pInstance = nullptr;
}

void AddonManagementDialog::display(bool bTopLevel)
{
	if(m_pInstance)
	{
		if(bTopLevel)
		{
			if(m_pInstance->parent())
			{
				m_pInstance->setParent(nullptr);
			}
		}
		else
		{
			if(m_pInstance->parent() != g_pMainWindow->splitter())
			{
				m_pInstance->setParent(g_pMainWindow->splitter());
			}
		}
	}
	else
	{
		if(bTopLevel)
		{
			m_pInstance = new AddonManagementDialog(nullptr);
		}
		else
		{
			m_pInstance = new AddonManagementDialog(g_pMainWindow->splitter());
		}
	}
	m_pInstance->show();
	m_pInstance->raise();
	m_pInstance->setFocus();
}

void AddonManagementDialog::closeEvent(QCloseEvent * e)
{
	e->ignore();
	delete this;
}

void AddonManagementDialog::reject()
{
	cleanup();
}
