//=============================================================================
//
//   File : SharedFilesWindow.cpp
//   Creation date : Mon Apr 21 2003 23:14:12 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "SharedFilesWindow.h"

#include "kvi_debug.h"
#include "kvi_out.h"
#include "KviFileDialog.h"
#include "KviIconManager.h"
#include "KviInput.h"
#include "KviIrcView.h"
#include "KviLocale.h"
#include "KviOptions.h"
#include "KviTalHBox.h"
#include "KviTalVBox.h"
#include "KviThemedLabel.h"

#include <QCheckBox>
#include <QDateTimeEdit>
#include <QFileInfo>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

extern SharedFilesWindow * g_pSharedFilesWindow;
extern KVIRC_API KviSharedFilesManager * g_pSharedFilesManager;

SharedFileEditDialog::SharedFileEditDialog(QWidget * par, KviSharedFile * f)
    : QDialog(par) //,"shared_file_editor",true)
{
	setModal(true);
	setObjectName("shared_file_editor");

	QGridLayout * g = new QGridLayout(this);

	setWindowTitle(__tr2qs_ctx("Shared File Configuration - KVIrc", "sharedfileswindow"));

	QLabel * l = new QLabel(__tr2qs_ctx("Share name:", "sharedfileswindow"), this);
	g->addWidget(l, 0, 0);

	m_pShareNameEdit = new QLineEdit(this);
	g->addWidget(m_pShareNameEdit, 0, 1, 1, 3);

	l = new QLabel(__tr2qs_ctx("File path:", "sharedfileswindow"), this);
	g->addWidget(l, 1, 0);

	m_pFilePathEdit = new QLineEdit(this);
	g->addWidget(m_pFilePathEdit, 1, 1, 1, 2);

	m_pBrowseButton = new QPushButton(__tr2qs_ctx("&Browse...", "sharedfileswindow"), this);
	g->addWidget(m_pBrowseButton, 1, 3);
	connect(m_pBrowseButton, SIGNAL(clicked()), this, SLOT(browse()));

	l = new QLabel(__tr2qs_ctx("User mask:", "sharedfileswindow"), this);
	g->addWidget(l, 2, 0);

	m_pUserMaskEdit = new QLineEdit(this);
	g->addWidget(m_pUserMaskEdit, 2, 1, 1, 3);

	m_pExpireCheckBox = new QCheckBox(__tr2qs_ctx("Expire at:", "sharedfileswindow"), this);
	g->addWidget(m_pExpireCheckBox, 3, 0);

	m_pExpireDateTimeEdit = new QDateTimeEdit(this);
	g->addWidget(m_pExpireDateTimeEdit, 3, 1, 1, 3);

	connect(m_pExpireCheckBox, SIGNAL(toggled(bool)), m_pExpireDateTimeEdit, SLOT(setEnabled(bool)));

	QPushButton * pb;

	pb = new QPushButton(__tr2qs_ctx("&OK", "sharedfileswindow"), this);
	connect(pb, SIGNAL(clicked()), this, SLOT(okClicked()));
	pb->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Accept)));
	g->addWidget(pb, 5, 2);
	pb = new QPushButton(__tr2qs_ctx("Cancel", "sharedfileswindow"), this);
	connect(pb, SIGNAL(clicked()), this, SLOT(reject()));
	pb->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Discard)));
	g->addWidget(pb, 5, 3);

	g->setRowStretch(4, 1);
	g->setColumnStretch(0, 1);

	if(f)
	{
		m_pShareNameEdit->setText(f->name());
		m_pFilePathEdit->setText(f->absFilePath());
		m_pUserMaskEdit->setText(f->userMask());
		QDateTime dt;
		dt.setTime_t(f->expireTime());
		m_pExpireDateTimeEdit->setDateTime(dt);
		m_pExpireCheckBox->setChecked(f->expires());
		m_pExpireDateTimeEdit->setEnabled(f->expires());
	}
	else
	{
		m_pExpireCheckBox->setChecked(false);
		m_pExpireDateTimeEdit->setDateTime(QDateTime::currentDateTime());
		m_pExpireDateTimeEdit->setEnabled(false);
	}
}

SharedFileEditDialog::~SharedFileEditDialog()
    = default;

void SharedFileEditDialog::browse()
{
	QString szBuf;
	QString szTxt = m_pFilePathEdit->text();
	if(!KviFileDialog::askForOpenFileName(szBuf, __tr2qs("Select a File - KVIrc"), szTxt, QString(), false, true, this))
		return;
	m_pFilePathEdit->setText(szBuf);
}

KviSharedFile * SharedFileEditDialog::getResult() const
{
	QString szName = m_pShareNameEdit->text();
	QString szPath = m_pFilePathEdit->text();
	QString szMask = m_pUserMaskEdit->text();
	QDateTime dt = m_pExpireDateTimeEdit->dateTime();
	bool bExpires = m_pExpireCheckBox->isChecked();
	QFileInfo f(szPath);
	return new KviSharedFile(szName, szPath, szMask, bExpires ? dt.toTime_t() : (time_t)0, f.size());
}

void SharedFileEditDialog::okClicked()
{
	QString szName = m_pShareNameEdit->text();
	QString szPath = m_pFilePathEdit->text();
	QDateTime dt = m_pExpireDateTimeEdit->dateTime();
	bool bExpires = m_pExpireCheckBox->isChecked();
	if(bExpires && (dt <= QDateTime::currentDateTime()))
	{
		QMessageBox::warning(this, __tr2qs_ctx("Invalid Expiry Time - KVIrc", "sharedfileswindow"),
		    __tr2qs_ctx("The expiry date/time is in the past: please either remove the \"Expires at\""
		                "check mark or specify a expiry date/time in the future",
		                         "sharedfileswindow"),
		    __tr2qs_ctx("OK", "sharedfileswindow"));
		return;
	}

	if(szName.isEmpty())
	{
		QMessageBox::warning(this, __tr2qs_ctx("Invalid Share Name - KVIrc", "sharedfileswindow"),
		    __tr2qs_ctx("The share name can't be empty, please correct it", "sharedfileswindow"),
		    __tr2qs_ctx("OK", "sharedfileswindow"));
		return;
	}

	QFileInfo f(szPath);
	if(!(f.exists() && f.isFile() && f.isReadable()))
	{
		QMessageBox::warning(this, __tr2qs_ctx("Error Opening File - KVIrc", "sharedfileswindow"),
		    __tr2qs_ctx("The file doesn't exist or it is not readable, please check the path", "sharedfileswindow"),
		    __tr2qs_ctx("OK", "sharedfileswindow"));
		return;
	}

	accept();
}

SharedFilesTreeWidgetItem::SharedFilesTreeWidgetItem(QTreeWidget * lv, KviSharedFile * f)
    : QTreeWidgetItem(lv)
{
	setText(0, f->name());
	setText(1, f->absFilePath());
	setText(2, f->userMask());
	if(f->expires())
	{
		QDateTime dt;
		dt.setTime_t(f->expireTime());
		setText(3, dt.toString());
	}
	else
	{
		setText(3, __tr2qs_ctx("Never", "sharedfileswindow"));
	}
	m_pSharedFilePointer = f;
}

SharedFilesTreeWidgetItem::~SharedFilesTreeWidgetItem()
    = default;

SharedFilesWindow::SharedFilesWindow()
    : KviWindow(KviWindow::Tool, "shared files window", nullptr)
{
	g_pSharedFilesWindow = this;
	m_pSplitter = new KviTalSplitter(Qt::Horizontal, this);
	m_pSplitter->setObjectName("sharedfiles_splitter");

	KviTalVBox * vbox = new KviTalVBox(m_pSplitter);

	m_pTreeWidget = new KviThemedTreeWidget(vbox, this, "sharedfileswindow_treewidget");
	//m_pTreeWidget->header()->hide();
	m_pTreeWidget->setAllColumnsShowFocus(true);
	QStringList columsLabels;
	columsLabels.append(__tr2qs_ctx("Name", "sharedfileswindow"));
	columsLabels.append(__tr2qs_ctx("Filename", "sharedfileswindow"));
	columsLabels.append(__tr2qs_ctx("Mask", "sharedfileswindow"));

	columsLabels.append(__tr2qs_ctx("Expires", "sharedfileswindow"));
	m_pTreeWidget->setHeaderLabels(columsLabels);
	m_pTreeWidget->setColumnWidth(0, 200);
	m_pTreeWidget->setColumnWidth(0, 300);
	m_pTreeWidget->setColumnWidth(0, 300);
	m_pTreeWidget->setColumnWidth(0, 200);

	m_pTreeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	connect(m_pTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(enableButtons()));

	connect(g_pSharedFilesManager, SIGNAL(sharedFilesChanged()), this, SLOT(fillFileView()));
	connect(g_pSharedFilesManager, SIGNAL(sharedFileAdded(KviSharedFile *)), this, SLOT(sharedFileAdded(KviSharedFile *)));
	connect(g_pSharedFilesManager, SIGNAL(sharedFileRemoved(KviSharedFile *)), this, SLOT(sharedFileRemoved(KviSharedFile *)));

	KviTalHBox * b = new KviTalHBox(vbox);

	m_pAddButton = new QPushButton(__tr2qs_ctx("&Add...", "sharedfileswindow"), b);
	connect(m_pAddButton, SIGNAL(clicked()), this, SLOT(addClicked()));
	m_pRemoveButton = new QPushButton(__tr2qs_ctx("Re&move", "sharedfileswindow"), b);
	connect(m_pRemoveButton, SIGNAL(clicked()), this, SLOT(removeClicked()));
	m_pEditButton = new QPushButton(__tr2qs_ctx("&Edit", "sharedfileswindow"), b);
	connect(m_pEditButton, SIGNAL(clicked()), this, SLOT(editClicked()));

	fillFileView();
}

SharedFilesWindow::~SharedFilesWindow()
{
	g_pSharedFilesWindow = nullptr;
}

void SharedFilesWindow::enableButtons()
{
	QTreeWidgetItem * it = (QTreeWidgetItem *)m_pTreeWidget->currentItem();
	m_pEditButton->setEnabled(it);
	m_pRemoveButton->setEnabled(it);
}

void SharedFilesWindow::removeClicked()
{
	SharedFilesTreeWidgetItem * it = (SharedFilesTreeWidgetItem *)m_pTreeWidget->currentItem();
	if(!it)
		return;

	g_pSharedFilesManager->removeSharedFile(it->readOnlySharedFilePointer()->name(), it->readOnlySharedFilePointer());
}

void SharedFilesWindow::addClicked()
{
	SharedFileEditDialog dlg(nullptr);
	if(dlg.exec() != QDialog::Accepted)
		return;

	KviSharedFile * f = dlg.getResult();
	if(!f)
		return;
	g_pSharedFilesManager->addSharedFile(f);
}

void SharedFilesWindow::editClicked()
{
	SharedFilesTreeWidgetItem * it = (SharedFilesTreeWidgetItem *)m_pTreeWidget->currentItem();
	if(!it)
		return;

	SharedFileEditDialog dlg(nullptr, it->readOnlySharedFilePointer());
	if(dlg.exec() != QDialog::Accepted)
		return;

	SharedFilesTreeWidgetItem * it2 = (SharedFilesTreeWidgetItem *)m_pTreeWidget->currentItem();
	if(it2 != it)
		return; // ooops ?
	KviSharedFile * f = dlg.getResult();
	if(!f)
		return; // ooops 2 ?

	g_pSharedFilesManager->removeSharedFile(it->readOnlySharedFilePointer()->name(), it->readOnlySharedFilePointer());
	g_pSharedFilesManager->addSharedFile(f);
}

void SharedFilesWindow::fillFileView()
{
	m_pTreeWidget->clear();
	KviPointerHashTableIterator<QString, KviSharedFileList> it(*(g_pSharedFilesManager->sharedFileListDict()));

	while(KviSharedFileList * l = it.current())
	{
		for(KviSharedFile * o = l->first(); o; o = l->next())
			new SharedFilesTreeWidgetItem(m_pTreeWidget, o);

		++it;
	}
	enableButtons();
}

void SharedFilesWindow::sharedFileAdded(KviSharedFile * f)
{
	new SharedFilesTreeWidgetItem(m_pTreeWidget, f);
	enableButtons();
}

void SharedFilesWindow::sharedFileRemoved(KviSharedFile * f)
{
	QTreeWidgetItem * it;
	for(int i = 0; i < m_pTreeWidget->topLevelItemCount(); i++)
	{
		it = (QTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		if(((SharedFilesTreeWidgetItem *)it)->readOnlySharedFilePointer() == f)
		{
			delete((SharedFilesTreeWidgetItem *)it);
			return;
		}
	}
	enableButtons();
}

void SharedFilesWindow::getBaseLogFileName(QString & buffer)
{
	buffer.sprintf("SHAREDFILES");
}

QPixmap * SharedFilesWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KviIconManager::SharedFiles);
}

void SharedFilesWindow::resizeEvent(QResizeEvent *)
{
	m_pSplitter->setGeometry(0, 0, width(), height());
}

QSize SharedFilesWindow::sizeHint() const
{
	return m_pSplitter->sizeHint();
}

void SharedFilesWindow::fillCaptionBuffers()
{
	m_szPlainTextCaption = __tr2qs_ctx("Shared Files", "sharedfileswindow");
}

void SharedFilesWindow::die()
{
	close();
}
