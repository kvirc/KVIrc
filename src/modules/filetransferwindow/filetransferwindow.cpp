//==================================================================================
//
//   File : filetransferwindow.cpp
//   Creation date : Mon Apr 21 2003 23:14:12 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2001 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//==================================================================================

#include "filetransferwindow.h"

#include "kvi_debug.h"
#include "kvi_iconmanager.h"
#include "kvi_ircview.h"
#include "kvi_out.h"
#include "kvi_options.h"
#include "kvi_locale.h"
#include "kvi_out.h"
#include "kvi_mirccntrl.h"
#include "kvi_themedlabel.h"
#include "kvi_input.h"
#include "kvi_qstring.h"
#include "kvi_tal_hbox.h"
#include "kvi_frame.h"

#include <QSplitter>
#include <QToolTip>
#include <QPainter>
#include <QMessageBox>
#include <QClipboard>
#include <QFileInfo>
#include <QFile>
#include <QLabel>
#include <QFontMetrics>
#include <QEvent>
#include <QKeyEvent>

// FIXME: Qt4 #include <QHeaderView>
#include <q3header.h>

#ifdef COMPILE_KDE_SUPPORT
	#include <kurl.h>
	#include <krun.h>
	#include <kmimetype.h>
	#include <kmimetypetrader.h>
	#include <kiconloader.h>
#endif //COMPILE_KDE_SUPPORT

extern KviFileTransferWindow * g_pFileTransferWindow;


KviFileTransferItem::KviFileTransferItem(KviTalListView * v,KviFileTransfer * t)
: KviTalListViewItem(v)
{
	m_pTransfer = t;
	m_pTransfer->setDisplayItem(this);
}

KviFileTransferItem::~KviFileTransferItem()
{
	m_pTransfer->setDisplayItem(0);
}

QString KviFileTransferItem::key(int column,bool bAcending) const
{
	QString ret;
	ret.setNum(m_pTransfer->id());
	if(ret.length() == 1)ret.prepend("0000000");
	else if(ret.length() == 2)ret.prepend("000000");
	else if(ret.length() == 3)ret.prepend("00000");
	else if(ret.length() == 4)ret.prepend("0000");
	else if(ret.length() == 5)ret.prepend("000");
	else if(ret.length() == 6)ret.prepend("00");
	else if(ret.length() == 7)ret.prepend("0");
	return ret;
}

void KviFileTransferItem::paintCell(QPainter * p,const QColorGroup &cg,int column,int width,int align)
{
	QPainter * newP;
	QPixmap * pix = 0;
	if(p->device() == listView()->viewport())
	{
		// ops.. there is no double buffering active ?
		// we'll do it then
		pix = g_pFileTransferWindow->memPixmap();
		if((pix->width() < width) || (pix->height() < height()))
		{
			pix->resize(width,height());
		} else {
			if((pix->width() > 500) || (pix->height() > 110))
			{
				if((pix->width() * pix->height()) > (width * height() * 4))
				{
					// this is getting too big
					pix->resize(width,height());
				}
			}
		}
		newP = new QPainter(pix);
	} else {
		newP = p;
	}

	p->setFont(listView()->font());

	newP->setPen(isSelected() ? cg.highlight() : cg.base());
	newP->drawRect(0,0,width,height());
	newP->setPen(m_pTransfer->active() ? QColor(180,180,180) : QColor(200,200,200));
	newP->drawRect(1,1,width - 2,height() - 2);
	newP->fillRect(2,2,width - 4,height() - 4,m_pTransfer->active() ? QColor(240,240,240) : QColor(225,225,225));

	m_pTransfer->displayPaint(newP,column,width,height());

	if(newP != p)
	{
		p->drawPixmap(0,0,*pix,0,0,width,height());
		delete newP;
	}
}

void KviFileTransferItem::setHeight(int h)
{
	KviTalListViewItem::setHeight(m_pTransfer->displayHeight(g_pFileTransferWindow->lineSpacing()));
}


KviFileTransferWindow::KviFileTransferWindow(KviModuleExtensionDescriptor * d,KviFrame * lpFrm)
: KviWindow(KVI_WINDOW_TYPE_TOOL,lpFrm,"file transfer window",0) , KviModuleExtension(d)
{
	g_pFileTransferWindow = this;

	m_pContextPopup = 0;
	m_pLocalFilePopup = 0;
	m_pOpenFilePopup = 0;

	m_pMemPixmap = new QPixmap(1,1);

	m_pTimer = new QTimer(this);
	connect(m_pTimer,SIGNAL(timeout()),this,SLOT(heartbeat()));

	m_pInput   = new KviInput(this,0);

	m_pSplitter = new QSplitter(Qt::Horizontal,this,"splitter");
	m_pVertSplitter = new QSplitter(Qt::Vertical,m_pSplitter,"vsplitter");

	m_pListView  = new KviTalListView(m_pVertSplitter);
	//m_pListView->header()->hide();
	m_pListView->setAllColumnsShowFocus(true);
	m_pListView->addColumn(__tr2qs_ctx("Type","filetransferwindow"),56);
	m_pListView->addColumn(__tr2qs_ctx("Information","filetransferwindow"),350);
	m_pListView->addColumn(__tr2qs_ctx("Progress","filetransferwindow"),350);

	KviDynamicToolTip * tp = new KviDynamicToolTip(m_pListView->viewport());
	connect(tp,SIGNAL(tipRequest(KviDynamicToolTip *,const QPoint &)),this,SLOT(tipRequest(KviDynamicToolTip *,const QPoint &)));

	//m_pListView->setFocusPolicy(NoFocus);
	//m_pListView->viewport()->setFocusPolicy(NoFocus);

	//connect(m_pListView,SIGNAL(rightButtonPressed(KviTalListViewItem *,const QPoint &,int)),
	// this,SLOT(showHostPopup(KviTalListViewItem *,const QPoint &,int)));

	QFontMetrics fm(font());
	m_iLineSpacing = fm.lineSpacing();

	m_pIrcView = new KviIrcView(m_pVertSplitter,lpFrm,this);

	m_pListView->installEventFilter(this);
	connect(m_pListView,SIGNAL(rightButtonPressed(KviTalListViewItem *,const QPoint &,int)),this,SLOT(rightButtonPressed(KviTalListViewItem *,const QPoint &,int)));
	connect(m_pListView,SIGNAL(doubleClicked(KviTalListViewItem *,const QPoint &,int)),this,SLOT(doubleClicked(KviTalListViewItem *,const QPoint &,int)));
	fillTransferView();

	connect(KviFileTransferManager::instance(),SIGNAL(transferRegistered(KviFileTransfer *)),this,SLOT(transferRegistered(KviFileTransfer *)));
	connect(KviFileTransferManager::instance(),SIGNAL(transferUnregistering(KviFileTransfer *)),this,SLOT(transferUnregistering(KviFileTransfer *)));

	KviFileTransferManager::instance()->setTransferWindow(this);

	//setFocusHandler(m_pInput,this);

	m_pTimer->start(2000);
}

KviFileTransferWindow::~KviFileTransferWindow()
{
	KviFileTransferManager::instance()->setTransferWindow(0);
	g_pFileTransferWindow = 0;
	delete m_pMemPixmap;
}

bool KviFileTransferWindow::eventFilter( QObject *obj, QEvent *ev )
{
	if( (obj==m_pListView) && ( ev->type() == QEvent::KeyPress ) )
	{
		QKeyEvent *keyEvent = (QKeyEvent*)ev;
		switch(keyEvent->key())
		{
			case Qt::Key_Delete:
				if(m_pListView->currentItem())
				{
					delete m_pListView->currentItem();
					return TRUE;
				}
				break;
		}
	}
	return KviWindow::eventFilter( obj, ev );
}

void KviFileTransferWindow::fontChange(const QFont &oldFont)
{
	QFontMetrics fm(font());
	m_iLineSpacing = fm.lineSpacing();
	KviWindow::fontChange(oldFont);
}

void KviFileTransferWindow::tipRequest(KviDynamicToolTip * tip,const QPoint &pnt)
{
	KviFileTransferItem * it = (KviFileTransferItem *)m_pListView->itemAt(pnt);
	if(!it)return;
	QString txt = it->transfer()->tipText();
	tip->tip(m_pListView->itemRect(it),txt);
}

void KviFileTransferWindow::fillTransferView()
{
	KviPointerList<KviFileTransfer> * l = KviFileTransferManager::instance()->transferList();
	if(!l)return;
	KviFileTransferItem * it;
	for(KviFileTransfer * t = l->first();t;t = l->next())
	{
		it = new KviFileTransferItem(m_pListView,t);
		t->setDisplayItem(it);
	}
}

KviFileTransferItem * KviFileTransferWindow::findItem(KviFileTransfer * t)
{
	KviFileTransferItem * it = (KviFileTransferItem *)m_pListView->firstChild();
	while(it)
	{
		if(it->transfer() == t)return it;
		it = (KviFileTransferItem *)(it->nextSibling());
	}
	return 0;
}

void KviFileTransferWindow::transferRegistered(KviFileTransfer * t)
{
	KviFileTransferItem * it = new KviFileTransferItem(m_pListView,t);
	//t->setDisplayItem(it);
}

void KviFileTransferWindow::transferUnregistering(KviFileTransfer * t)
{
	KviFileTransferItem * it = findItem(t);
	//t->setDisplayItem(0);
	if(it)delete it;
    it = 0;
}

void KviFileTransferWindow::doubleClicked(KviTalListViewItem *it,const QPoint &pnt,int col)
{
	if(it) openLocalFile();
}

void KviFileTransferWindow::rightButtonPressed(KviTalListViewItem *it,const QPoint &pnt,int col)
{
	if(!m_pContextPopup)m_pContextPopup = new KviTalPopupMenu(this);
	if(!m_pLocalFilePopup)m_pLocalFilePopup = new KviTalPopupMenu(this);
	if(!m_pOpenFilePopup)
	{
		m_pOpenFilePopup= new KviTalPopupMenu(this);
		connect(m_pOpenFilePopup,SIGNAL(activated(int)),this,SLOT(openFilePopupActivated(int)));
	}

	m_pContextPopup->clear();

	int id;

	if(it)
	{
		KviFileTransferItem * i = (KviFileTransferItem *)it;
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
					tmp += __tr2qs_ctx("Size: %1","filetransferwindow").arg(KviQString::makeSizeReadable(fi.size()));
					tmp += "</nobr><br>";
				}

#ifdef COMPILE_KDE_SUPPORT
				tmp += "<nobr>Mime: ";
				tmp += KMimeType::findByPath(szFile)->name();
				tmp += "</nobr>";
#endif //COMPILE_KDE_SUPPORT

				QLabel * l = new QLabel(tmp,m_pLocalFilePopup);
				l->setFrameStyle(QFrame::Raised | QFrame::Panel);
				m_pLocalFilePopup->insertItem(l);

#ifdef COMPILE_KDE_SUPPORT
				QString mimetype = KMimeType::findByPath(szFile)->name();
				KService::List offers = KMimeTypeTrader::self()->query(mimetype,"Application");

				id = m_pLocalFilePopup->insertItem(__tr2qs_ctx("&Open","filetransferwindow"),this,SLOT(openLocalFile()));
				m_pLocalFilePopup->setItemParameter(id,-1);

				m_pOpenFilePopup->clear();

				int id;
				int idx = 0;

				for(KService::List::Iterator itOffers = offers.begin();
	   				itOffers != offers.end(); ++itOffers)
				{
					id = m_pOpenFilePopup->insertItem(
							SmallIcon((*itOffers).data()->icon()),
							(*itOffers).data()->name()
						);
					m_pOpenFilePopup->setItemParameter(id,idx);
					idx++;
				}

				m_pOpenFilePopup->insertSeparator();

				id = m_pOpenFilePopup->insertItem(__tr2qs_ctx("&Other...","filetransferwindow"),this,SLOT(openLocalFileWith()));
				m_pOpenFilePopup->setItemParameter(id,-1);

				m_pLocalFilePopup->insertItem(__tr2qs_ctx("Open &With","filetransferwindow"),m_pOpenFilePopup);
				m_pLocalFilePopup->insertSeparator();
				m_pLocalFilePopup->insertItem(__tr2qs_ctx("Open &Location","filetransferwindow"),this,SLOT(openLocalFileFolder()));
				m_pLocalFilePopup->insertItem(__tr2qs_ctx("Terminal at Location","filetransferwindow"),this,SLOT(openLocalFileTerminal()));
				m_pLocalFilePopup->insertSeparator();
#endif //COMPILE_KDE_SUPPORT

//-| Grifisx & Noldor |-
#ifdef COMPILE_ON_WINDOWS
				id = m_pLocalFilePopup->insertItem(__tr2qs_ctx("&Open","filetransferwindow"),this,SLOT(openLocalFile()));
				m_pLocalFilePopup->setItemParameter(id,-1);
				m_pOpenFilePopup->insertSeparator();
				m_pLocalFilePopup->insertItem(__tr2qs_ctx("Open &With","filetransferwindow"),this,SLOT(openLocalFileWith()));
				m_pLocalFilePopup->insertSeparator();
				m_pLocalFilePopup->insertItem(__tr2qs_ctx("Open &Location","filetransferwindow"),this,SLOT(openLocalFileFolder()));
				m_pLocalFilePopup->insertItem(__tr2qs_ctx("MS-DOS Prompt at Location","filetransferwindow"),this,SLOT(openLocalFileTerminal()));
				m_pLocalFilePopup->insertSeparator();
#endif //COMPILE_ON_WINDOWS
// G&N end

				m_pLocalFilePopup->insertItem(__tr2qs_ctx("&Copy Path to Clipboard","filetransferwindow"),this,SLOT(copyLocalFileToClipboard()));

				id = m_pLocalFilePopup->insertItem(__tr2qs_ctx("&Delete file","filetransferwindow"),this,SLOT(deleteLocalFile()));
				m_pLocalFilePopup->setItemEnabled(id,i->transfer()->terminated());
				m_pContextPopup->insertItem(__tr2qs_ctx("Local &File","filetransferwindow"),m_pLocalFilePopup);


			}

			i->transfer()->fillContextPopup(m_pContextPopup,col);
			m_pContextPopup->insertSeparator();
		}
	}


	bool bHaveTerminated = false;
	KviFileTransferItem * item = (KviFileTransferItem *)m_pListView->firstChild();
	while(item)
	{
		if(item->transfer()->terminated())
		{
			bHaveTerminated = true;
			break;
		}
		item = (KviFileTransferItem *)item->nextSibling();
	}

	id = m_pContextPopup->insertItem(__tr2qs_ctx("&Clear Terminated","filetransferwindow"),this,SLOT(clearTerminated()));
	m_pContextPopup->setItemEnabled(id,bHaveTerminated);

	bool bAreTransfersActive = false;
	if(m_pListView->childCount() >= 1)
		bAreTransfersActive = true;

	id = m_pContextPopup->insertItem(__tr2qs_ctx("Clear &All","filetransferwindow"),this,SLOT(clearAll()));
	m_pContextPopup->setItemEnabled(id,bAreTransfersActive);

	m_pContextPopup->popup(pnt);
}


KviFileTransfer * KviFileTransferWindow::selectedTransfer()
{
	KviTalListViewItem * it = m_pListView->selectedItem();
	if(!it)return 0;
	KviFileTransferItem * i = (KviFileTransferItem *)it;
	return i->transfer();
}

void KviFileTransferWindow::openFilePopupActivated(int id)
{
#ifdef COMPILE_KDE_SUPPORT
	int ip = m_pOpenFilePopup->itemParameter(id);
	if(ip < 0)return;
	QString txt = m_pOpenFilePopup->text(id);

	KviFileTransfer * t = selectedTransfer();
	if(!t)return;
	QString tmp = t->localFileName();
	if(tmp.isEmpty())return;


	QString mimetype = KMimeType::findByPath(tmp)->name();
	KService::List offers = KMimeTypeTrader::self()->query(mimetype,"Application");

	for(KService::List::Iterator itOffers = offers.begin();
	   				itOffers != offers.end(); ++itOffers)
	{
		if(txt == (*itOffers).data()->name())
		{
			KUrl::List lst;
			KUrl url;
			url.setPath(tmp);
			lst.append(url);
			KRun::run(*((*itOffers).data()),lst,g_pFrame);
			break;
		}
	}
#endif //COMPILE_KDE_SUPPORT
}

void KviFileTransferWindow::openLocalFileTerminal()
{
//-| Grifisx & Noldor |-
#ifdef COMPILE_ON_WINDOWS
	KviFileTransfer * t = selectedTransfer();
	if(!t)return;
	QString tmp = t->localFileName();
	if(tmp.isEmpty())return;
	int idx = tmp.lastIndexOf("/");
	if(idx == -1)return;
	tmp = tmp.left(idx);
	tmp.append("\"");
	// FIXME: this is not a solution ...because if the drive isn't system's drive the command 'cd' naturaly doesn't work
	tmp.prepend("cmd.exe /k cd \"");
	system(tmp.toLocal8Bit().data());
#else //COMPILE_ON_WINDOWS
// G&N end
	#ifdef COMPILE_KDE_SUPPORT
		KviFileTransfer * t = selectedTransfer();
		if(!t)return;
		QString tmp = t->localFileName();
		if(tmp.isEmpty())return;

		int idx = tmp.lastIndexOf("/");
		if(idx == -1)return;
		tmp = tmp.left(idx);

		tmp.prepend("konsole --workdir=\"");
		tmp.append("\"");

		KRun::runCommand(tmp,g_pFrame);
	#endif //COMPILE_KDE_SUPPORT
#endif //!COMPILE_ON_WINDOWS
}

void KviFileTransferWindow::deleteLocalFile()
{
	KviFileTransfer * t = selectedTransfer();
	if(!t)return;

	QString fName = t->localFileName();
	QString tmp;

	KviQString::sprintf(tmp,__tr2qs_ctx("Do you really want to delete the file %Q?","filetransferwindow"),&fName);

	if(QMessageBox::warning(this,__tr2qs_ctx("Confirm delete","filetransferwindow"),
			tmp,__tr2qs_ctx("Yes","filetransferwindow"),__tr2qs_ctx("No","filetransferwindow")) != 0)
		return;

	if(!QFile::remove(fName))
		QMessageBox::warning(this,__tr2qs_ctx("Delete failed","filetransferwindow"),
			__tr2qs_ctx("Failed to remove the file","filetransferwindow"),
			__tr2qs_ctx("OK","filetransferwindow"));
}

void KviFileTransferWindow::openLocalFile()
{
//-| Grifisx & Noldor |-
#ifdef COMPILE_ON_WINDOWS

	KviFileTransfer * t = selectedTransfer();
	if(!t)return;
	QString tmp = t->localFileName();
	if(tmp.isEmpty())return;
	tmp.replace("/","\\");
	ShellExecute(0,"open",tmp.toLocal8Bit().data(),NULL,NULL,SW_SHOWNORMAL);  //You have to link the shell32.lib
#else //!COMPILE_ON_WINDOWS
// G&N end
	#ifdef COMPILE_KDE_SUPPORT
		KviFileTransfer * t = selectedTransfer();
		if(!t)return;
		QString tmp = t->localFileName();
		if(tmp.isEmpty())return;

		QString mimetype = KMimeType::findByPath(tmp)->name();  //KMimeType
		KService::Ptr offer = KMimeTypeTrader::self()->preferredService(mimetype,"Application");
		if(!offer)
		{
			openLocalFileWith();
			return;
		}

		KUrl::List lst;
		KUrl url;
		url.setPath(tmp);
		lst.append(url);

		KRun::run(*offer, lst, g_pFrame);
	#endif //COMPILE_KDE_SUPPORT
#endif //!COMPILE_ON_WINDOWS
}

void KviFileTransferWindow::openLocalFileWith()
{
//-| Grifisx & Noldor |-
#ifdef COMPILE_ON_WINDOWS
	KviFileTransfer * t = selectedTransfer();
	if(!t)return;
	QString tmp = t->localFileName();
	if(tmp.isEmpty())return;
	tmp.replace("/","\\");
	tmp.prepend("rundll32.exe shell32.dll,OpenAs_RunDLL "); // this if to show the 'open with...' window
	WinExec(tmp.toLocal8Bit().data(),SW_SHOWNORMAL);
#else //!COMPILE_ON_WINDOWS
// G&N end
	#ifdef COMPILE_KDE_SUPPORT
		KviFileTransfer * t = selectedTransfer();
		if(!t)return;
		QString tmp = t->localFileName();
		if(tmp.isEmpty())return;

		KUrl::List lst;
		KUrl url;
		url.setPath(tmp);
		lst.append(url);
		KRun::displayOpenWithDialog(lst,g_pFrame);
	#endif //COMPILE_KDE_SUPPORT
#endif //!COMPILE_ON_WINDOWS
}

void KviFileTransferWindow::copyLocalFileToClipboard()
{
	KviFileTransfer * t = selectedTransfer();
	if(!t)return;
	QString tmp = t->localFileName();
	if(tmp.isEmpty())return;
	QApplication::clipboard()->setText(tmp);
}

void KviFileTransferWindow::openLocalFileFolder()
{
//-| Grifisx & Noldor|-
#ifdef COMPILE_ON_WINDOWS
	KviFileTransfer * t = selectedTransfer();
	if(!t)return;
	QString tmp = t->localFileName();
	if(tmp.isEmpty())return;
	tmp=QFileInfo(tmp).dirPath(TRUE);
	tmp.replace('/','\\');
	tmp.prepend("explorer.exe ");
	WinExec(tmp.toLocal8Bit().data(), SW_MAXIMIZE);
#else //!COMPILE_ON_WINDOWS
// G&N end
	#ifdef COMPILE_KDE_SUPPORT
		KviFileTransfer * t = selectedTransfer();
		if(!t)return;
		QString tmp = t->localFileName();
		if(tmp.isEmpty())return;

		int idx = tmp.lastIndexOf("/");
		if(idx == -1)return;
		tmp = tmp.left(idx);

		QString mimetype = KMimeType::findByPath(tmp)->name(); // inode/directory
		KService::Ptr offer = KMimeTypeTrader::self()->preferredService(mimetype,"Application");
		if(!offer)return;

		KUrl::List lst;
		KUrl url;
		url.setPath(tmp);
		lst.append(url);
		KRun::run(*offer,lst,g_pFrame);
	#endif //COMPILE_KDE_SUPPORT
#endif //!COMPILE_ON_WINDOWS
}

void KviFileTransferWindow::heartbeat()
{
	if(m_pListView->childCount() < 1)return;

	KviTalListViewItem * i1;
	KviTalListViewItem * i2;

	i1 = m_pListView->itemAt(QPoint(1,1));
	if(!i1)
	{
		m_pListView->viewport()->update();
		return;
	}
	i2 = m_pListView->itemAt(QPoint(1,m_pListView->viewport()->height() - 2));
	if(i2)i2 = i2->nextSibling();

	while(i1 && (i1 != i2))
	{
		if(((KviFileTransferItem *)i1)->transfer()->active())
		{
			m_pListView->repaintItem(i1);
		}
		i1 = i1->nextSibling();
	}
}

void KviFileTransferWindow::clearAll()
{
	QString tmp;

	bool bHaveAllTerminated = true;
	KviFileTransferItem * item = (KviFileTransferItem *)m_pListView->firstChild();
	while(item)
	{
		if(!item->transfer()->terminated())
		{
			bHaveAllTerminated = false;
			break;
		}
		item = (KviFileTransferItem *)item->nextSibling();
	}

	KviQString::sprintf(tmp,__tr2qs_ctx("Clear all transfers, including any in progress?","filetransferwindow"));

	// If any transfer is active asks for confirm
	if(!bHaveAllTerminated)
		if(QMessageBox::warning(this,__tr2qs_ctx("Clear All Transfers? - KVIrc","filetransferwindow"), tmp,__tr2qs_ctx("Yes","filetransferwindow"),__tr2qs_ctx("No","filetransferwindow")) != 0)
			return;

	KviFileTransferManager::instance()->killAllTransfers();
}

void KviFileTransferWindow::clearTerminated()
{
	KviFileTransferManager::instance()->killTerminatedTransfers();
}

void KviFileTransferWindow::getBaseLogFileName(KviStr &buffer)
{
	buffer.sprintf("FILETRANSFER");
}

QPixmap * KviFileTransferWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KVI_SMALLICON_FILETRANSFER);
}

void KviFileTransferWindow::resizeEvent(QResizeEvent *e)
{
	int h = m_pInput->heightHint();
	m_pSplitter->setGeometry(0,0,width(),height() - h);
	m_pInput->setGeometry(0,height() - h,width(),h);
}

QSize KviFileTransferWindow::sizeHint() const
{
	return m_pSplitter->sizeHint();
}

void KviFileTransferWindow::fillCaptionBuffers()
{
	m_szPlainTextCaption = __tr2qs_ctx("File Transfers","filetransferwindow");

	m_szHtmlActiveCaption = "<nobr><font color=\"";
	m_szHtmlActiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextActive).name();
	m_szHtmlActiveCaption += "\"><b>";
	m_szHtmlActiveCaption += m_szPlainTextCaption;
	m_szHtmlActiveCaption += "</b></font></nobr>";

	m_szHtmlInactiveCaption = "<nobr><font color=\"";
	m_szHtmlInactiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextInactive).name();
	m_szHtmlInactiveCaption += "\"><b>";
	m_szHtmlInactiveCaption += m_szPlainTextCaption;
	m_szHtmlInactiveCaption += "</b></font></nobr>";
}

void KviFileTransferWindow::die()
{
	close();
}



//#warning "Load & save properties of this kind of window"

//void KviFileTransferWindow::saveProperties()
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
//void KviFileTransferWindow::setProperties(KviWindowProperty *p)
//{
//	QValueList<int> l;
//	l.append(p->splitWidth1);
//	l.append(p->splitWidth2);
//	m_pVertSplitter->setSizes(l);
//	m_pIrcView->setTimestamp(p->timestamp);
//	m_pIrcView->setShowImages(p->imagesVisible);
//}

void KviFileTransferWindow::applyOptions()
{
	m_pIrcView->applyOptions();
	KviWindow::applyOptions();
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_filetransferwindow.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
