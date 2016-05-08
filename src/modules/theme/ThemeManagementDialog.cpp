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
#include "kvi_settings.h"

#if defined(COMPILE_WEBKIT_SUPPORT)
#include "WebThemeInterfaceDialog.h"
#endif

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
#include "KviFileDialog.h"
#include "KviDynamicToolTip.h"
#include "KviMessageBox.h"
#include "KviTalListWidget.h"
#include "kvi_fileextensions.h"

#include <QVBoxLayout>
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
#include <QAbstractTextDocumentLayout>
#include <QShortcut>
#include <QMenu>

extern QRect g_rectManagementDialogGeometry;

ThemeListWidgetItem::ThemeListWidgetItem(KviTalListWidget * pBox, KviThemeInfo * pInfo)
    : KviTalListWidgetItem(pBox)
{
	m_pThemeInfo = pInfo;

	QString szText;
	szText = "<b>";
	szText += pInfo->name();
	szText += "</b>";

	if(!pInfo->version().isEmpty())
	{
		szText += "[";
		szText += pInfo->version();
		szText += "]";
	}

	if(!pInfo->author().isEmpty())
	{
		szText += " <font color=\"#454545\"> ";
		szText += __tr2qs_ctx("by", "theme");
		szText += " ";
		szText += pInfo->author();
		szText += "</font>";
	}

	szText += "<br><font size=\"-1\">";
	szText += pInfo->description();
	szText += "</font>";
	setText(szText);
}

ThemeListWidgetItem::~ThemeListWidgetItem()
{
	delete m_pThemeInfo;
}

ThemeManagementDialog * ThemeManagementDialog::m_pInstance = nullptr;

ThemeManagementDialog::ThemeManagementDialog(QWidget * parent)
    : QWidget(parent)
{
	m_pItemDelegate = nullptr;
#ifdef COMPILE_WEBKIT_SUPPORT
	m_pWebThemeInterfaceDialog = nullptr;
#endif
	setObjectName("theme_options_widget");
	setWindowTitle(__tr2qs_ctx("Manage Themes - KVIrc", "theme"));
	setWindowIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Theme)));

	m_pInstance = this;

	//QGridLayout * g = new QGridLayout(this);
	QVBoxLayout * pVBox = new QVBoxLayout(this);
	KviTalHBox * pHBox = new KviTalHBox(this);
	pHBox->setMargin(1);
	pHBox->setSpacing(1);
	//g->addWidget(pHBox,0,0);
	pVBox->addWidget(pHBox);
	QToolButton * pTool;
	QFrame * pSep;

	pTool = new QToolButton(pHBox);
	pTool->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_SAVE)));
	pTool->setIconSize(QSize(32, 32));
	pTool->setToolTip(__tr2qs_ctx("Save current theme...", "theme"));
	connect(pTool, SIGNAL(clicked()), this, SLOT(saveCurrentTheme()));

	pSep = new QFrame(pHBox);
	pSep->setFrameStyle(QFrame::VLine | QFrame::Sunken);
	pSep->setMinimumWidth(12);

	m_pPackThemeButton = new QToolButton(pHBox);
	m_pPackThemeButton->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_PACK)));

	m_pPackThemeButton->setIconSize(QSize(32, 32));
	m_pPackThemeButton->setToolTip(__tr2qs_ctx("Export selected themes to a distributable package", "theme"));
	connect(m_pPackThemeButton, SIGNAL(clicked()), this, SLOT(packTheme()));

	m_pDeleteThemeButton = new QToolButton(pHBox);
	m_pDeleteThemeButton->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_REMOVE)));
	m_pDeleteThemeButton->setIconSize(QSize(32, 32));
	m_pDeleteThemeButton->setToolTip(__tr2qs_ctx("Delete selected themes", "theme"));
	connect(m_pDeleteThemeButton, SIGNAL(clicked()), this, SLOT(deleteTheme()));

	pSep = new QFrame(pHBox);
	pSep->setFrameStyle(QFrame::VLine | QFrame::Sunken);
	pSep->setMinimumWidth(12);

	pTool = new QToolButton(pHBox);
	pTool->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_OPEN)));
	pTool->setIconSize(QSize(32, 32));
	pTool->setToolTip(__tr2qs_ctx("Install theme package from disk", "theme"));
	connect(pTool, SIGNAL(clicked()), this, SLOT(installFromFile()));

	pTool = new QToolButton(pHBox);
	pTool->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_WWW)));
	pTool->setIconSize(QSize(32, 32));
	pTool->setToolTip(__tr2qs_ctx("Get more themes...", "theme"));
	connect(pTool, SIGNAL(clicked()), this, SLOT(getMoreThemes()));

	QWidget * w = new QWidget(pHBox);
	w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

	m_pListWidget = new KviTalListWidget(this);
	m_pListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	m_pItemDelegate = new KviTalIconAndRichTextItemDelegate(m_pListWidget);
	m_pItemDelegate->setDefaultIcon(g_pIconManager->getBigIcon(QString(KVI_BIGICON_THEME))->scaled(64, 64, Qt::KeepAspectRatio)); // FIXME!
	m_pItemDelegate->setMinimumSize(QSize(64, 64));
	m_pItemDelegate->setIconSize(QSize(64, 64));
	m_pListWidget->setItemDelegate(m_pItemDelegate);
	m_pListWidget->setMinimumHeight(400);
	m_pListWidget->setMinimumWidth(520);

	m_pListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_pListWidget->setSortingEnabled(true);
	connect(m_pListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(applyTheme(QListWidgetItem *)));

	//FIXME tooltip
	connect(m_pListWidget, SIGNAL(customContextMenuRequested(const QPoint &)),
	    this, SLOT(contextMenuRequested(const QPoint &)));
	connect(m_pListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(enableDisableButtons()));

	pSep = new QFrame(this);
	pSep->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	pSep->setMinimumWidth(300);
	pSep->setMinimumHeight(8);
	pVBox->addWidget(pSep);

	m_pCurrentInstalledThemeLabel = new QLabel(this);
	m_pCurrentInstalledThemeLabel->setText(__tr2qs_ctx("<b><u>Current Installed Theme:</u> ", "theme") + " " + KVI_OPTION_STRING(KviOption_stringIconThemeSubdir) + "</b>");

	pVBox->addWidget(m_pCurrentInstalledThemeLabel);

	pSep = new QFrame(this);
	pSep->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	pSep->setMinimumWidth(400);

	pVBox->addWidget(pSep);

	pVBox->addWidget(m_pListWidget);

	QPushButton * b = new QPushButton(__tr2qs("Close"), this);
	b->setMaximumSize(b->sizeHint().width(), b->sizeHint().height());
	connect(b, SIGNAL(clicked()), this, SLOT(closeClicked()));

	pVBox->addWidget(b);

	pVBox->setAlignment(b, Qt::AlignRight);
	fillThemeBox();
	m_pContextPopup = new QMenu(this);

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

ThemeManagementDialog::~ThemeManagementDialog()
{
	if(m_pItemDelegate)
		delete m_pItemDelegate;
	g_rectManagementDialogGeometry = QRect(pos().x(), pos().y(), size().width(), size().height());
	m_pInstance = nullptr;
#ifdef COMPILE_WEBKIT_SUPPORT
	if(m_pWebThemeInterfaceDialog)
	{
		m_pWebThemeInterfaceDialog->deleteLater();
		m_pWebThemeInterfaceDialog = nullptr;
	}
#endif //COMPILE_WEBKIT_SUPPORT
}

void ThemeManagementDialog::closeClicked()
{
	deleteLater();
	m_pInstance = nullptr;
}

void ThemeManagementDialog::display(bool bTopLevel)
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
			m_pInstance = new ThemeManagementDialog(nullptr);
		}
		else
		{
			m_pInstance = new ThemeManagementDialog(g_pMainWindow->splitter());
		}
	}
	m_pInstance->show();
	m_pInstance->raise();
	m_pInstance->setFocus();
}

void ThemeManagementDialog::cleanup()
{
	if(!m_pInstance)
		return;
	delete m_pInstance;
	m_pInstance = nullptr;
}

void ThemeManagementDialog::packTheme()
{
	KviPointerList<KviThemeInfo> dl;
	dl.setAutoDelete(false);
	QList<QListWidgetItem *> itemsSelected = m_pListWidget->selectedItems();
	for(int i = 0; i < itemsSelected.count(); i++)
		dl.append(((ThemeListWidgetItem *)itemsSelected.at(i))->themeInfo());
	if(dl.isEmpty())
		return;

	PackThemeDialog * pDialog = new PackThemeDialog(this, &dl);
	pDialog->exec();
	pDialog->deleteLater();
}

void ThemeManagementDialog::contextMenuRequested(const QPoint & pos)
{
	ThemeListWidgetItem * pItem = dynamic_cast<ThemeListWidgetItem *>(m_pListWidget->itemAt(pos));
	if(pItem != nullptr)
	{
		m_pListWidget->setCurrentItem(pItem);
		m_pContextPopup->clear();

		KviThemeInfo * pInfo = pItem->themeInfo();
		if(!pInfo)
			return;

		if(!pInfo->isBuiltin())
			m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Discard)), __tr2qs_ctx("&Remove Theme", "theme"), this, SLOT(deleteTheme()));

		m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Accept)), __tr2qs_ctx("&Apply Theme", "theme"), this, SLOT(applyCurrentTheme()));
		m_pContextPopup->popup(m_pListWidget->viewport()->mapToGlobal(pos));
	}
}

void ThemeManagementDialog::applyTheme(QListWidgetItem * it)
{
	if(it)
		m_pListWidget->setCurrentItem(it);
	applyCurrentTheme();
}

void ThemeManagementDialog::applyCurrentTheme()
{
	ThemeListWidgetItem * it = (ThemeListWidgetItem *)m_pListWidget->currentItem();
	if(!it)
		return;

	if(!KviMessageBox::yesNo(__tr2qs_ctx("Apply Theme - KVIrc", "theme"),
	       __tr2qs_ctx("Do you wish to apply theme \"%Q\" (version %Q)?", "theme"),
	       &(it->themeInfo()->name()), &(it->themeInfo()->version())))
		return;

	KviThemeInfo out;

	//qDebug("Apply theme item %x, info %x, %s : %s at location %d",(long)it,(long)it->themeInfo(),it->themeInfo()->directory().toUtf8().data(),it->themeInfo()->subdirectory().toUtf8().data(),it->themeInfo()->location());
	if(!KviTheme::apply(it->themeInfo()->subdirectory(), it->themeInfo()->location(), out))
	{
		QString szErr = out.lastError();
		QString szMsg = QString(__tr2qs_ctx("Failed to apply the specified theme: %1", "theme")).arg(szErr);
		QMessageBox::critical(this, __tr2qs_ctx("Apply Theme - KVIrc", "theme"), szMsg,
		    QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
		return;
	}

	m_pCurrentInstalledThemeLabel->setText(QString(__tr2qs_ctx("<b><u>Current Installed Theme:</u> %1</b>", "theme")).arg(KVI_OPTION_STRING(KviOption_stringIconThemeSubdir)));
	m_pItemDelegate->setDefaultIcon(g_pIconManager->getBigIcon(QString(KVI_BIGICON_THEME))->scaled(64, 64, Qt::KeepAspectRatio));
}

void ThemeManagementDialog::deleteTheme()
{
	QList<QListWidgetItem *> itemsSelected = m_pListWidget->selectedItems();
	for(int i = 0; i < itemsSelected.count(); i++)
	{
		ThemeListWidgetItem * pItem = dynamic_cast<ThemeListWidgetItem *>(itemsSelected.at(i));
		if(!pItem)
			return;
		KviThemeInfo * pInfo = pItem->themeInfo();
		if(pInfo->isBuiltin())
			continue;

		if(!KviMessageBox::yesNo(
		       __tr2qs_ctx("Delete Selected Theme - KVIrc", "theme"),
		       __tr2qs_ctx("Do you really wish to delete theme \"%Q\" (version %Q)?", "theme"),
		       &(pInfo->name()), &(pInfo->version())))
			goto jump_out;

		QString szThemePath = ((ThemeListWidgetItem *)itemsSelected.at(i))->themeInfo()->directory();
		KviFileUtils::deleteDir(szThemePath);
	}
jump_out:
	fillThemeBox();
}

void ThemeManagementDialog::installFromFile()
{
	QString szFileName;
	QString szError;

	if(!KviFileDialog::askForOpenFileName(
	       szFileName,
	       __tr2qs_ctx("Select a Installation File - KVIrc", "theme"),
	       QString(),
	       KVI_FILTER_THEME,
	       false,
	       true,
	       this))
		return;

	if(!ThemeFunctions::installThemePackage(szFileName, szError, this))
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
	}
	else
	{
		m_pWebThemeInterfaceDialog = new WebThemeInterfaceDialog();
		QObject::connect(m_pWebThemeInterfaceDialog, SIGNAL(destroyed()), this, SLOT(webThemeInterfaceDialogDestroyed()));
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
	m_pWebThemeInterfaceDialog = nullptr;
#endif
	fillThemeBox();
}

void ThemeManagementDialog::saveCurrentTheme()
{
	SaveThemeDialog * pSaveThemeDialog = new SaveThemeDialog(this);
	pSaveThemeDialog->exec();
	pSaveThemeDialog->deleteLater();
	fillThemeBox();
}

void ThemeManagementDialog::fillThemeBox(bool bBuiltin)
{
	QStringList slThemes;
	KviTheme::installedThemeDirectories(slThemes, bBuiltin ? KviThemeInfo::Builtin : KviThemeInfo::User);

	for(int i = 0; i < slThemes.count(); i++)
	{
		//qDebug("Installed theme %s builtin %d",slThemes.at(i).toUtf8().data(),bBuiltin);
		KviThemeInfo * inf = new KviThemeInfo();
		if(inf->load(slThemes.at(i), bBuiltin ? KviThemeInfo::Builtin : KviThemeInfo::User))
		{
			ThemeListWidgetItem * it = new ThemeListWidgetItem(m_pListWidget, inf);

			//qDebug("Item %x, info %x, info2 %x, %s : %s at location %d",(long)it,(long)inf,(long)it->themeInfo(),it->themeInfo()->directory().toUtf8().data(),it->themeInfo()->subdirectory().toUtf8().data(),it->themeInfo()->location());
			QPixmap pixmap = inf->smallScreenshot();
			if(!pixmap.isNull())
				it->setIcon(pixmap.scaled(300, 280, Qt::KeepAspectRatio));
		}
		else
		{
			delete inf;
		}
	}
}

void ThemeManagementDialog::fillThemeBox()
{
	m_pListWidget->clear();

	fillThemeBox(true);
	fillThemeBox(false);

	enableDisableButtons();
}

void ThemeManagementDialog::enableDisableButtons()
{
	QList<QListWidgetItem *> itemsSelected = m_pListWidget->selectedItems();
	int iCount = itemsSelected.count();
	bool bHasItems = iCount ? true : false;

	m_pPackThemeButton->setEnabled(bHasItems);

	unsigned int u = 0;
	for(int i = 0; i < iCount; i++)
	{
		ThemeListWidgetItem * pItem = dynamic_cast<ThemeListWidgetItem *>(itemsSelected.at(i));
		if(!pItem)
			return;
		KviThemeInfo * pInfo = pItem->themeInfo();
		if(!pInfo->isBuiltin())
			u++;
	}

	m_pDeleteThemeButton->setEnabled(u >= 1);
}

void ThemeManagementDialog::closeEvent(QCloseEvent * e)
{
	e->ignore();
	deleteLater();
}

void ThemeManagementDialog::tipRequest(QListWidgetItem *, const QPoint &)
{
}
