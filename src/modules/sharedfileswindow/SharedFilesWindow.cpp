//=============================================================================
//
//   File : SharedFilesWindow.cpp
//   Creation date : Mon Apr 21 2003 23:14:12 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the linkss of the GNU General Public License
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

#include "SharedFilesWindow.h"

#include "kvi_debug.h"
#include "KviIconManager.h"
#include "KviIrcView.h"
#include "kvi_out.h"
#include "KviOptions.h"
#include "KviLocale.h"
#include "kvi_out.h"
#include "KviThemedLabel.h"
#include "KviInput.h"
#include "KviFileDialog.h"
#include "KviTalHBox.h"
#include "KviTalVBox.h"

#include <QDateTimeEdit>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QLayout>
#include <QMessageBox>
#include <QFileInfo>

extern SharedFilesWindow * g_pSharedFilesWindow;
extern KVIRC_API KviSharedFilesManager * g_pSharedFilesManager;

SharedFileEditDialog::SharedFileEditDialog(QWidget * par,KviSharedFile * f)
: QDialog(par)//,"shared_file_editor",true)
{
	setModal(true);
	setObjectName("shared_file_editor");

	QGridLayout * g = new QGridLayout(this);

	setWindowTitle(__tr2qs_ctx("Edit Shared File - KVIrc","sharedfileswindow"));

	QLabel * l = new QLabel(__tr2qs_ctx("Share name:","sharedfileswindow"),this);
	g->addWidget(l, 0, 0 );

	m_pShareNameEdit = new QLineEdit(this);
	g->addWidget(m_pShareNameEdit,0,1,1,3);
	//g->addMultiCellWidget( m_pShareNameEdit, 0, 0, 1, 3 );

	l = new QLabel(__tr2qs_ctx("File path:","sharedfileswindow"),this);
	g->addWidget(l, 1, 0 );

	m_pFilePathEdit = new QLineEdit(this);
	g->addWidget(m_pFilePathEdit,1,1,1,2);
	//g->addMultiCellWidget( m_pFilePathEdit, 1, 1, 1, 2 );

	m_pBrowseButton = new QPushButton(__tr2qs_ctx("&Browse...","sharedfileswindow"),this);
	g->addWidget( m_pBrowseButton, 1, 3 );
	connect(m_pBrowseButton,SIGNAL(clicked()),this,SLOT(browse()));

	l = new QLabel(__tr2qs_ctx("User mask:","sharedfileswindow"),this);
	g->addWidget(l, 2, 0 );

	m_pUserMaskEdit = new QLineEdit(this);
	g->addWidget(m_pUserMaskEdit,2,1,1,3);
//	g->addMultiCellWidget( m_pUserMaskEdit, 2, 2, 1, 3 );

	m_pExpireCheckBox = new QCheckBox(__tr2qs_ctx("Expire at:","sharedfileswindow"),this);
	g->addWidget(m_pExpireCheckBox,3,0);

	m_pExpireDateTimeEdit = new QDateTimeEdit(this);
	g->addWidget(m_pExpireDateTimeEdit,3,1,1,3);
	//g->addMultiCellWidget(m_pExpireDateTimeEdit, 3, 3, 1, 3 );

	connect(m_pExpireCheckBox,SIGNAL(toggled(bool)),m_pExpireDateTimeEdit,SLOT(setEnabled(bool)));

	QPushButton * pb;

	pb = new QPushButton(__tr2qs_ctx("&OK","sharedfileswindow"),this);
	connect(pb,SIGNAL(clicked()),this,SLOT(okClicked()));
	pb->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Accept)));
	g->addWidget(pb,5,2);
	pb = new QPushButton(__tr2qs_ctx("Cancel","sharedfileswindow"),this);
	connect(pb,SIGNAL(clicked()),this,SLOT(reject()));
	pb->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Discard)));
	g->addWidget(pb,5,3);

	g->setRowStretch(4,1);
	g->setColumnStretch(0,1);

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
	} else {
		m_pExpireCheckBox->setChecked(false);
		m_pExpireDateTimeEdit->setDateTime(QDateTime::currentDateTime());
		m_pExpireDateTimeEdit->setEnabled(false);
	}

}

SharedFileEditDialog::~SharedFileEditDialog()
{

}

void SharedFileEditDialog::browse()
{
	QString szBuf;
	QString szTxt = m_pFilePathEdit->text();
	if(!KviFileDialog::askForOpenFileName(szBuf,__tr2qs("Choose the file to share"),szTxt))return;
	m_pFilePathEdit->setText(szBuf);
}

KviSharedFile * SharedFileEditDialog::getResult()
{
	QString szName = m_pShareNameEdit->text();
	QString szPath = m_pFilePathEdit->text();
	QString szMask = m_pUserMaskEdit->text();
	QDateTime dt = m_pExpireDateTimeEdit->dateTime();
	bool bExpires = m_pExpireCheckBox->isChecked();
	QFileInfo f(szPath);
	return new KviSharedFile(szName,szPath,szMask,bExpires ? dt.toTime_t() : (time_t)0,f.size());
}

void SharedFileEditDialog::okClicked()
{
	QString szName = m_pShareNameEdit->text();
	QString szPath = m_pFilePathEdit->text();
	QDateTime dt = m_pExpireDateTimeEdit->dateTime();
	bool bExpires = m_pExpireCheckBox->isChecked();
	if(bExpires && (dt <= QDateTime::currentDateTime()))
	{
		QMessageBox::warning(this,__tr2qs_ctx("Invalid expire time","sharedfileswindow"),
			__tr2qs_ctx("The expire date/time is in the past: please either remove the \"expires\"" \
					"check mark or specify a expire date/time in the future","sharedfileswindow"),
					__tr2qs_ctx("OK","sharedfileswindow"));
		return;
	}

	if(szName.isEmpty())
	{
		QMessageBox::warning(this,__tr2qs_ctx("Invalid share name","sharedfileswindow"),
			__tr2qs_ctx("The share name can't be empty, please correct it","sharedfileswindow"),
			__tr2qs_ctx("OK","sharedfileswindow"));
		return;
	}

	QFileInfo f(szPath);
	if(!(f.exists() && f.isFile() && f.isReadable()))
	{
		QMessageBox::warning(this,__tr2qs_ctx("Can't open the file","sharedfileswindow"),
			__tr2qs_ctx("The file doesn't exist or it is not readable, please check the path","sharedfileswindow"),
			__tr2qs_ctx("OK","sharedfileswindow"));
		return;
	}

	accept();
}


SharedFilesTreeWidgetItem::SharedFilesTreeWidgetItem(QTreeWidget * lv,KviSharedFile * f)
: QTreeWidgetItem(lv)
{
	setText(0,f->name());
	setText(1,f->absFilePath());
	setText(2,f->userMask());
	if(f->expires())
	{
		QDateTime dt;
		dt.setTime_t(f->expireTime());
		setText(3,dt.toString());
	} else {
		setText(3,__tr2qs_ctx("Never","sharedfileswindow"));
	}
	m_pSharedFilePointer = f;
}

SharedFilesTreeWidgetItem::~SharedFilesTreeWidgetItem()
{
}


SharedFilesWindow::SharedFilesWindow()
: KviWindow(KviWindow::Tool,"shared files window",0)
{
	g_pSharedFilesWindow = this;
	m_pSplitter = new KviTalSplitter(Qt::Horizontal,this);
	m_pSplitter->setObjectName("sharedfiles_splitter");

	KviTalVBox * vbox = new KviTalVBox(m_pSplitter);

	m_pTreeWidget  = new KviThemedTreeWidget(vbox, this, "sharedfileswindow_treewidget");
	//m_pTreeWidget->header()->hide();
	m_pTreeWidget->setAllColumnsShowFocus(true);
	QStringList columsLabels;
	columsLabels.append(__tr2qs_ctx("Name","sharedfileswindow"));
	columsLabels.append(__tr2qs_ctx("Filename","sharedfileswindow"));
	columsLabels.append(__tr2qs_ctx("Mask","sharedfileswindow"));

	columsLabels.append(__tr2qs_ctx("Expires","sharedfileswindow"));
	m_pTreeWidget->setHeaderLabels(columsLabels);
	m_pTreeWidget->setColumnWidth(0,200);
	m_pTreeWidget->setColumnWidth(0,300);
	m_pTreeWidget->setColumnWidth(0,300);
	m_pTreeWidget->setColumnWidth(0,200);

	m_pTreeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	connect(m_pTreeWidget,SIGNAL(itemSelectionChanged()),this,SLOT(enableButtons()));

	connect(g_pSharedFilesManager,SIGNAL(sharedFilesChanged()),this,SLOT(fillFileView()));
	connect(g_pSharedFilesManager,SIGNAL(sharedFileAdded(KviSharedFile *)),this,SLOT(sharedFileAdded(KviSharedFile *)));
	connect(g_pSharedFilesManager,SIGNAL(sharedFileRemoved(KviSharedFile *)),this,SLOT(sharedFileRemoved(KviSharedFile *)));

	KviTalHBox * b = new KviTalHBox(vbox);

	m_pAddButton = new QPushButton(__tr2qs_ctx("&Add...","sharedfileswindow"),b);
	connect(m_pAddButton,SIGNAL(clicked()),this,SLOT(addClicked()));
	m_pRemoveButton = new QPushButton(__tr2qs_ctx("Re&move","sharedfileswindow"),b);
	connect(m_pRemoveButton,SIGNAL(clicked()),this,SLOT(removeClicked()));
	m_pEditButton = new QPushButton(__tr2qs_ctx("&Edit","sharedfileswindow"),b);
	connect(m_pEditButton,SIGNAL(clicked()),this,SLOT(editClicked()));

	fillFileView();
}

SharedFilesWindow::~SharedFilesWindow()
{
	g_pSharedFilesWindow = 0;
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
	if(!it)return;

	g_pSharedFilesManager->removeSharedFile(it->readOnlySharedFilePointer()->name(),it->readOnlySharedFilePointer());
}

void SharedFilesWindow::addClicked()
{
	SharedFileEditDialog dlg(0);
	if(dlg.exec() != QDialog::Accepted)return;

	KviSharedFile * f = dlg.getResult();
	if(!f)return;
	g_pSharedFilesManager->addSharedFile(f);
}

void SharedFilesWindow::editClicked()
{
	SharedFilesTreeWidgetItem * it = (SharedFilesTreeWidgetItem *)m_pTreeWidget->currentItem();
	if(!it)return;

	SharedFileEditDialog dlg(0,it->readOnlySharedFilePointer());
	if(dlg.exec() != QDialog::Accepted)return;

	SharedFilesTreeWidgetItem * it2 = (SharedFilesTreeWidgetItem *)m_pTreeWidget->currentItem();
	if(it2 != it)return; // ooops ?
	KviSharedFile * f = dlg.getResult();
	if(!f)return; // ooops 2 ?

	g_pSharedFilesManager->removeSharedFile(it->readOnlySharedFilePointer()->name(),it->readOnlySharedFilePointer());
	g_pSharedFilesManager->addSharedFile(f);
}

void SharedFilesWindow::fillFileView()
{
	m_pTreeWidget->clear();
	KviPointerHashTableIterator<QString,KviSharedFileList> it(*(g_pSharedFilesManager->sharedFileListDict()));

	while(KviSharedFileList * l = it.current())
	{
		for(KviSharedFile * o = l->first();o;o = l->next())
			new SharedFilesTreeWidgetItem(m_pTreeWidget,o);

		++it;
	}
	enableButtons();
}

void SharedFilesWindow::sharedFileAdded(KviSharedFile * f)
{
	new SharedFilesTreeWidgetItem(m_pTreeWidget,f);
	enableButtons();
}

void SharedFilesWindow::sharedFileRemoved(KviSharedFile * f)
{
	QTreeWidgetItem * it;
	for (int i=0;i<m_pTreeWidget->topLevelItemCount();i++)
	{
		it=(QTreeWidgetItem *) m_pTreeWidget->topLevelItem(i);
		if(((SharedFilesTreeWidgetItem *)it)->readOnlySharedFilePointer() == f)
		{
			delete ((SharedFilesTreeWidgetItem *)it);
			return;
		}
	}
	enableButtons();
}



/*
void SharedFilesWindow::tipRequest(KviDynamicToolTip * tip,const QPoint &pnt)
{

	KviSharedFilesItem * it = (KviSharedFilesItem *)m_pTreeWidget->itemAt(pnt);
	if(!it)return;
	QString txt = it->transfer()->tipText();
	tip->tip(m_pTreeWidget->itemRect(it),txt);

}
*/
/*
void SharedFilesWindow::fillTransferView()
{
	KviPointerList<KviSharedFiles> * l = KviSharedFilesManager::instance()->transferList();
	if(!l)return;
	KviSharedFilesItem * it;
	for(KviSharedFiles * t = l->first();t;t = l->next())
	{
		it = new KviSharedFilesItem(m_pTreeWidget,t);
		t->setDisplayItem(it);
	}
}

KviSharedFilesItem * SharedFilesWindow::findItem(KviSharedFiles * t)
{
	KviSharedFilesItem * it = (KviSharedFilesItem *)m_pTreeWidget->firstChild();
	while(it)
	{
		if(it->transfer() == t)return it;
		it = (KviSharedFilesItem *)(it->nextSibling());
	}
	return 0;
}

void SharedFilesWindow::transferRegistered(KviSharedFiles * t)
{
	KviSharedFilesItem * it = new KviSharedFilesItem(m_pTreeWidget,t);
	//t->setDisplayItem(it);
}

void SharedFilesWindow::transferUnregistering(KviSharedFiles * t)
{
	KviSharedFilesItem * it = findItem(t);
	//t->setDisplayItem(0);
	if(it)delete it;
}

void SharedFilesWindow::rightButtonPressed(QTreeWidgetItem *it,const QPoint &pnt,int col)
{
	if(!m_pContextPopup)m_pContextPopup = new QMenu(this);
	if(!m_pLocalFilePopup)m_pLocalFilePopup = new QMenu(this);
	if(!m_pOpenFilePopup)
	{
		m_pOpenFilePopup= new QMenu(this);
		connect(m_pOpenFilePopup,SIGNAL(activated(int)),this,SLOT(openFilePopupActivated(int)));
	}

	m_pContextPopup->clear();

	int id;


	if(it)
	{
		KviSharedFilesItem * i = (KviSharedFilesItem *)it;
		if(i->transfer())
		{

			QString szFile = i->transfer()->localFileName();
			if(!szFile.isEmpty())
			{
				m_pLocalFilePopup->clear();

				QString tmp = "<b>file:/";
				tmp += szFile;
				tmp += "</b><br>";

				QFileInfo fi(szFile);
				if(fi.exists())
				{
					tmp += "<nobr>";
					tmp += __tr2qs_ctx("Size: %1 bytes","filetransferwindow").arg(fi.size());
					tmp += "</nobr><br>";
				}

#ifdef COMPILE_KDE_SUPPORT
				tmp += "<nobr>Mime: ";
				tmp += KMimeType::findByPath(szFile)->name();
				tmp += "</nobr>";
#endif //COMPILE_KDE_SUPPORT

				QLabel * l = new QLabel(tmp,m_pLocalFilePopup);
				QPalette p;
				m_pLabel->setStyleSheet("background-color: " + p.color(QPalette::Normal, QPalette::Mid).name());
				m_pLocalFilePopup->addAction(l);

#ifdef COMPILE_KDE_SUPPORT
				QString mimetype = KMimeType::findByPath(szFile)->name();
				KServiceTypeProfile::OfferList offers = KServiceTypeProfile::offers(mimetype,"Application");

				m_pOpenFilePopup->clear();

				int id;
				int idx = 0;

				for(KServiceTypeProfile::OfferList::Iterator itOffers = offers.begin();
	   				itOffers != offers.end(); ++itOffers)
				{
					id = m_pOpenFilePopup->addAction((*itOffers).service()->pixmap(KIcon::Small),(*itOffers).service()->name());
					m_pOpenFilePopup->setItemParameter(id,idx);
					idx++;
				}

				m_pOpenFilePopup->addSeparator();

				id = m_pOpenFilePopup->addAction(__tr2qs_ctx("Default application","filetransferwindow"),this,SLOT(openLocalFile()));
				m_pOpenFilePopup->setItemParameter(id,-1);
				id = m_pOpenFilePopup->addAction(__tr2qs_ctx("&Other...","filetransferwindow"),this,SLOT(openLocalFileWith()));
				m_pOpenFilePopup->setItemParameter(id,-1);

				m_pLocalFilePopup->addAction(__tr2qs_ctx("Open with","filetransferwindow"),m_pOpenFilePopup);
				m_pLocalFilePopup->addSeparator();
				m_pLocalFilePopup->addAction(__tr2qs_ctx("Open folder","filetransferwindow"),this,SLOT(openLocalFileFolder()));
				m_pLocalFilePopup->addAction(__tr2qs_ctx("Reach in terminal","filetransferwindow"),this,SLOT(openLocalFileTerminal()));
				m_pLocalFilePopup->addSeparator();
#endif //COMPILE_KDE_SUPPORT
				m_pLocalFilePopup->addAction(__tr2qs_ctx("Copy path to clipboard","filetransferwindow"),this,SLOT(copyLocalFileToClipboard()));

				m_pContextPopup->addAction(__tr2qs_ctx("Local file","filetransferwindow"),m_pLocalFilePopup);
			}

			i->transfer()->fillContextPopup(m_pContextPopup,col);
			m_pContextPopup->addSeparator();
		}
	}


	bool bHaveTerminated = false;
	KviSharedFilesItem * item = (KviSharedFilesItem *)m_pTreeWidget->firstChild();
	while(item)
	{
		if(item->transfer()->terminated())
		{
			bHaveTerminated = true;
			break;
		}
		item = (KviSharedFilesItem *)item->nextSibling();
	}

	id = m_pContextPopup->addAction(__tr2qs_ctx("Clear terminated","filetransferwindow"),this,SLOT(clearTerminated()));
	m_pContextPopup->setItemEnabled(id,bHaveTerminated);

	id = m_pContextPopup->addAction(__tr2qs_ctx("Clear all","filetransferwindow"),this,SLOT(clearAll()));
	m_pContextPopup->setItemEnabled(id,it);

	m_pContextPopup->popup(pnt);
}


KviSharedFiles * SharedFilesWindow::selectedTransfer()
{
	QTreeWidgetItem * it = m_pTreeWidget->selectedItem();
	if(!it)return 0;
	KviSharedFilesItem * i = (KviSharedFilesItem *)it;
	return i->transfer();
}

void SharedFilesWindow::openFilePopupActivated(int id)
{
#ifdef COMPILE_KDE_SUPPORT
	int ip = m_pOpenFilePopup->itemParameter(id);
	if(ip < 0)return;
	QString txt = m_pOpenFilePopup->text(id);

	KviSharedFiles * t = selectedTransfer();
	if(!t)return;
	QString tmp = t->localFileName();
	if(tmp.isEmpty())return;

	QString mimetype = KMimeType::findByPath(tmp)->name();
	KServiceTypeProfile::OfferList offers = KServiceTypeProfile::offers(mimetype,"Application");

	for(KServiceTypeProfile::OfferList::Iterator itOffers = offers.begin();
			itOffers != offers.end(); ++itOffers)
	{
		if(txt == (*itOffers).service()->name())
		{
			KURL::List lst;
			KURL url;
			url.setPath(tmp);
			lst.append(url);
			KRun::run(*((*itOffers).service()), lst);
			break;
		}
	}
#endif //COMPILE_KDE_SUPPORT
}

void SharedFilesWindow::openLocalFileTerminal()
{
#ifdef COMPILE_KDE_SUPPORT
	KviSharedFiles * t = selectedTransfer();
	if(!t)return;
	QString tmp = t->localFileName();
	if(tmp.isEmpty())return;

	int idx = tmp.findRev("/");
	if(idx == -1)return;
	tmp = tmp.left(idx);

	tmp.prepend("konsole --workdir=\"");
	tmp.append("\"");

	KRun::runCommand(tmp);
#endif //COMPILE_KDE_SUPPORT
}

void SharedFilesWindow::openLocalFile()
{
#ifdef COMPILE_KDE_SUPPORT
	KviSharedFiles * t = selectedTransfer();
	if(!t)return;
	QString tmp = t->localFileName();
	if(tmp.isEmpty())return;

	QString mimetype = KMimeType::findByPath(tmp)->name();
	KService::Ptr offer = KServiceTypeProfile::preferredService(mimetype,"Application");
	if(!offer)
	{
		openLocalFileWith();
		return;
	}

	KURL::List lst;
	KURL url;
	url.setPath(tmp);
	lst.append(url);
	KRun::run(*offer, lst);
#endif //COMPILE_KDE_SUPPORT
}

void SharedFilesWindow::openLocalFileWith()
{
#ifdef COMPILE_KDE_SUPPORT
	KviSharedFiles * t = selectedTransfer();
	if(!t)return;
	QString tmp = t->localFileName();
	if(tmp.isEmpty())return;

	KURL::List lst;
	KURL url;
	url.setPath(tmp);
	lst.append(url);
	KRun::displayOpenWithDialog(lst);
#endif //COMPILE_KDE_SUPPORT
}

void SharedFilesWindow::copyLocalFileToClipboard()
{
	KviSharedFiles * t = selectedTransfer();
	if(!t)return;
	QString tmp = t->localFileName();
	if(tmp.isEmpty())return;
	QApplication::clipboard()->setSelectionMode(false);
	QApplication::clipboard()->setText(tmp);
	QApplication::clipboard()->setSelectionMode(true);
	QApplication::clipboard()->setText(tmp);
}

void SharedFilesWindow::openLocalFileFolder()
{
#ifdef COMPILE_KDE_SUPPORT
	KviSharedFiles * t = selectedTransfer();
	if(!t)return;
	QString tmp = t->localFileName();
	if(tmp.isEmpty())return;

	int idx = tmp.findRev("/");
	if(idx == -1)return;
	tmp = tmp.left(idx);

	QString mimetype = KMimeType::findByPath(tmp)->name(); // inode/directory
	KService::Ptr offer = KServiceTypeProfile::preferredService(mimetype,"Application");
	if(!offer)return;

	KURL::List lst;
	KURL url;
	url.setPath(tmp);
	lst.append(url);
	KRun::run(*offer, lst);
#endif //COMPILE_KDE_SUPPORT
}

void SharedFilesWindow::heartbeat()
{
	if(m_pTreeWidget->childCount() < 1)return;

	QTreeWidgetItem * i1;
	QTreeWidgetItem * i2;

	i1 = m_pTreeWidget->itemAt(QPoint(1,1));
	if(!i1)
	{
		m_pTreeWidget->viewport()->update();
		return;
	}
	i2 = m_pTreeWidget->itemAt(QPoint(1,m_pTreeWidget->viewport()->height() - 2));
	if(i2)i2 = i2->nextSibling();

	while(i1 && (i1 != i2))
	{
		if(((KviSharedFilesItem *)i1)->transfer()->active())
		{
			m_pTreeWidget->repaintItem(i1);
		}
		i1 = i1->nextSibling();
	}
}

void SharedFilesWindow::clearAll()
{
	if(QMessageBox::information(this,__tr2qs_ctx("Clear all transfers","filetransferwindow"),
				__tr2qs_ctx("Remove all the transfers including the running ones ?","filetransferwindow"),
				__tr2qs_ctx("Yes","filetransferwindow"),__tr2qs_ctx("No","filetransferwindow")) != 0)return;

	KviSharedFilesManager::instance()->killAllTransfers();
}

void SharedFilesWindow::clearTerminated()
{
	KviSharedFilesManager::instance()->killTerminatedTransfers();
}
*/
void SharedFilesWindow::getBaseLogFileName(QString &buffer)
{
	buffer.sprintf("SHAREDFILES");
}

QPixmap * SharedFilesWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KviIconManager::SharedFiles);
}

void SharedFilesWindow::resizeEvent(QResizeEvent *)
{
	//int h = m_pInput->heightHint();
	m_pSplitter->setGeometry(0,0,width(),height());
	//m_pInput->setGeometry(0,height() - h,width(),h);
}

QSize SharedFilesWindow::sizeHint() const
{
	return m_pSplitter->sizeHint();
}

void SharedFilesWindow::fillCaptionBuffers()
{
	m_szPlainTextCaption = __tr2qs_ctx("Shared Files","sharedfileswindow");
}

void SharedFilesWindow::die()
{
	close();
}



//#warning "Load & save properties of this kind of window"

//void SharedFilesWindow::saveProperties()
//{
//	KviWindowProperty p;
//	p.rect = externalGeometry();
//	p.isDocked = isAttacched();
//	QValueList<int> l(m_pSplitter->sizes());
//	if(l.count() >= 1)p.splitWidth1 = *(l.at(0));
//	if(l.count() >= 2)p.splitWidth2 = *(l.at(1));
//	p.timestamp = m_pView->timestamp();
//	p.imagesVisible = m_pView->imagesVisible();
//	p.isMaximized = isAttacched() && isMaximized();
//	p.topSplitWidth1 = 0;
//	p.topSplitWidth2 = 0;
//	p.topSplitWidth3 = 0;
//	g_pOptions->m_pWinPropertiesList->setProperty(caption(),&p);
//}
//
//void SharedFilesWindow::setProperties(KviWindowProperty *p)
//{
//	QValueList<int> l;
//	l.append(p->splitWidth1);
//	l.append(p->splitWidth2);
//	m_pVertSplitter->setSizes(l);
//	m_pIrcView->setTimestamp(p->timestamp);
//	m_pIrcView->setShowImages(p->imagesVisible);
//}


#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_SharedFilesWindow.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
