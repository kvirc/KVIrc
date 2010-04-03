//=============================================================================
//
//   File : logviewmdiwindow.cpp
//   Creation date : Tue Apr 23 2002 18:08:22 by Juanjo Alvarez
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Juanjo Alvarez
//   Copyright (C) 2002-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "logviewmdiwindow.h"
#include "logviewwidget.h"

#include "kvi_iconmanager.h"
#include "kvi_locale.h"
#include "kvi_module.h"
#include "kvi_options.h"
#include "kvi_frame.h"
#include "kvi_ircview.h"
#include "kvi_qstring.h"
#include "kvi_app.h"
#include "kvi_fileutils.h"
#include "kvi_tal_popupmenu.h"

#include <QList>
#include <QPixmap>
#include <QToolButton>
#include <QFileInfo>
#include <QDir>
#include <QCursor>
#include <QHeaderView>
#include <QLayout>
#include <QPushButton>
#include <QProgressDialog>
#include <QTextCodec>
#include <QDateTimeEdit>
#include <QLineEdit>
#include <QLabel>
#include <QShortcut>
#include <QMouseEvent>
#include <QByteArray>
#include <QMessageBox>

#ifdef COMPILE_ZLIB_SUPPORT
	#include <zlib.h>
#endif

#include <limits.h> //for INT_MAX

extern KviLogViewMDIWindow * g_pLogViewWindow;

KviLogViewMDIWindow::KviLogViewMDIWindow(KviModuleExtensionDescriptor * d,KviFrame * lpFrm)
: KviWindow(KVI_WINDOW_TYPE_LOGVIEW,lpFrm,"logview"), KviModuleExtension(d)
{
	g_pLogViewWindow = this;
//	m_pLogViewWidget = new KviLogViewWidget(this);

	m_pSplitter = new KviTalSplitter(Qt::Horizontal,this);
	m_pSplitter->setObjectName("main_splitter");
	m_pTabWidget = new QTabWidget(m_pSplitter);

	m_pIndexTab  = new KviTalVBox(m_pTabWidget);
	m_pTabWidget->addTab(m_pIndexTab,__tr2qs_ctx("Index","logview"));

	m_pListView = new KviLogViewListView(m_pIndexTab);

	connect(m_pListView,SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)),this,SLOT(itemSelected(QTreeWidgetItem *,QTreeWidgetItem *)));
	connect(m_pListView,SIGNAL(rightButtonPressed(QTreeWidgetItem *,QPoint)),
		this,SLOT(rightButtonClicked(QTreeWidgetItem *,QPoint)));

	m_pSearchTab  = new QWidget(m_pTabWidget);
	m_pTabWidget->addTab(m_pSearchTab,__tr2qs_ctx("Filter","logview"));

	QGridLayout *layout = new QGridLayout(m_pSearchTab);

	m_pShowChannelsCheck = new QCheckBox(__tr2qs_ctx("Show channel logs","logview"),m_pSearchTab);
	m_pShowChannelsCheck->setChecked(true);
	layout->addWidget(m_pShowChannelsCheck,0,0,1,2);

	m_pShowQueryesCheck  = new QCheckBox(__tr2qs_ctx("Show query logs","logview"),m_pSearchTab);
	m_pShowQueryesCheck->setChecked(true);
	layout->addWidget(m_pShowQueryesCheck,1,0,1,2);

	m_pShowConsolesCheck = new QCheckBox(__tr2qs_ctx("Show console logs","logview"),m_pSearchTab);
	m_pShowConsolesCheck->setChecked(true);
	layout->addWidget(m_pShowConsolesCheck,2,0,1,2);

	m_pShowDccChatCheck  = new QCheckBox(__tr2qs_ctx("Show DCC chat logs","logview"),m_pSearchTab);
	m_pShowDccChatCheck->setChecked(true);
	layout->addWidget(m_pShowDccChatCheck,3,0,1,2);

	m_pShowOtherCheck   = new QCheckBox(__tr2qs_ctx("Show other logs","logview"),m_pSearchTab);
	m_pShowOtherCheck->setChecked(true);
	layout->addWidget(m_pShowOtherCheck,4,0,1,2);

	QLabel *l;
	l = new QLabel(__tr2qs_ctx("Contents filter","logview"),m_pSearchTab);
	layout->addWidget(l,5,0,1,2);

	l = new QLabel(__tr2qs_ctx("Log name mask:","logview"),m_pSearchTab);
	m_pFileNameMask = new QLineEdit(m_pSearchTab);
	connect(m_pFileNameMask,SIGNAL(returnPressed()),this,SLOT(applyFilter()));
	layout->addWidget(l,6,0);
	layout->addWidget(m_pFileNameMask,6,1);

	l = new QLabel(__tr2qs_ctx("Log contents mask:","logview"),m_pSearchTab);
	m_pContentsMask = new QLineEdit(m_pSearchTab);
	connect(m_pContentsMask,SIGNAL(returnPressed()),this,SLOT(applyFilter()));
	layout->addWidget(l,7,0);
	layout->addWidget(m_pContentsMask,7,1);

	m_pEnableFromFilter = new QCheckBox(__tr2qs_ctx("Only older than","logview"),m_pSearchTab);
	m_pFromDateEdit = new QDateEdit(m_pSearchTab);
	m_pFromDateEdit->setDate(QDate::currentDate());
	layout->addWidget(m_pEnableFromFilter,8,0);
	layout->addWidget(m_pFromDateEdit,8,1);
	connect(m_pEnableFromFilter,SIGNAL(toggled(bool)),m_pFromDateEdit,SLOT(setEnabled(bool)));
	m_pFromDateEdit->setEnabled(false);

	m_pEnableToFilter = new QCheckBox(__tr2qs_ctx("Only newier than","logview"),m_pSearchTab);
	m_pToDateEdit = new QDateEdit(m_pSearchTab);
	m_pToDateEdit->setDate(QDate::currentDate());
	layout->addWidget(m_pEnableToFilter,9,0);
	layout->addWidget(m_pToDateEdit,9,1);
	connect(m_pEnableToFilter,SIGNAL(toggled(bool)),m_pToDateEdit,SLOT(setEnabled(bool)));
	m_pToDateEdit->setEnabled(false);

	m_pFilterButton = new QPushButton(__tr2qs_ctx("Apply filter","logview"),m_pSearchTab);
	connect(m_pFilterButton,SIGNAL(clicked()),this,SLOT(applyFilter()));
	layout->addWidget(m_pFilterButton,10,1);

	QWidget *w = new QWidget(m_pSearchTab);
	w->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
	layout->addWidget(w,11,1);

	m_pIrcView = new KviIrcView(m_pSplitter,g_pFrame,this);
	m_pIrcView->setMaxBufferSize(INT_MAX);
	m_pIrcView->setFocusPolicy(Qt::ClickFocus);

	QList<int> li;
	li.append(110);
	li.append(width()-110);
	m_pSplitter->setSizes(li);

	g_pApp->getLocalKvircDirectory(m_szLogDirectory,KviApp::Log);
	KviQString::ensureLastCharIs(m_szLogDirectory,'/'); // Does this work on Windows?

	//avoid to execute the long time-consuming procedure of log indexing here:
	//we could still be inside the context of the "Browse log files" QAction
	QTimer::singleShot( 0, this, SLOT(cacheFileList()) );
}

KviLogViewMDIWindow::~KviLogViewMDIWindow()
{
	g_pLogViewWindow = 0;
}

void KviLogViewMDIWindow::keyPressEvent(QKeyEvent *e)
{
//Make CtrlKey and CommandKey ("Apple") behave equally on MacOSX.
//This way typical X11 and Apple shortcuts can be used simultanously within the input line.
	if((e->modifiers() & Qt::ControlModifier) || (e->modifiers() & Qt::AltModifier) || (e->modifiers() & Qt::MetaModifier))
	{
		if(e->key() == Qt::Key_F)
		{
			m_pIrcView->toggleToolWidget();
			return;
		}
	}
	KviWindow::keyPressEvent(e);
}

void KviLogViewMDIWindow::applyFilter()
{
	setupItemList();
}

QPixmap * KviLogViewMDIWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KVI_SMALLICON_LOG);
}

void KviLogViewMDIWindow::resizeEvent(QResizeEvent *)
{
	m_pSplitter->setGeometry(0,0,width(),height());
}

void KviLogViewMDIWindow::fillCaptionBuffers()
{
	m_szPlainTextCaption = __tr2qs_ctx("Log Viewer","logview");
}

void KviLogViewMDIWindow::die()
{
	close();
}

QSize KviLogViewMDIWindow::sizeHint() const
{
	QSize ret(m_pSplitter->sizeHint().width(),m_pIrcView->sizeHint().height());
	return ret;
}

void KviLogViewMDIWindow::setupItemList()
{
	m_pFilterButton->setEnabled(false);
	m_pListView->clear();
	KviLogFile *pFile;
	KviLogListViewItem *pLastCategory=0;
	KviLogListViewItemFolder *pLastGroupItem=0;
	QString szLastGroup;
	QString szCurGroup;
	const bool bShowChannel=m_pShowChannelsCheck->isChecked();
	const bool bShowQuery=m_pShowQueryesCheck->isChecked();
	const bool bShowConsole=m_pShowConsolesCheck->isChecked();
	const bool bShowOther=m_pShowOtherCheck->isChecked();
	const bool bShowDccChat=m_pShowDccChatCheck->isChecked();

	const bool filterFromDate=m_pEnableFromFilter->isChecked();
	const bool filterToDate=m_pEnableToFilter->isChecked();

	const QString nameFilterText = m_pFileNameMask->text();
	const bool enableNameFilter = !nameFilterText.isEmpty();

	const QString contentFilterText = m_pContentsMask->text();
	const bool enableContentFilter = !contentFilterText.isEmpty();

	QDate fromDate = m_pFromDateEdit->date();
	QDate toDate   = m_pToDateEdit->date();

	QString textBuffer;
	QProgressDialog progress( __tr2qs_ctx("Filtering files...","logview"),
		__tr2qs_ctx("Abort filtering","logview"), 0, m_logList.count(),
		this);
	progress.setObjectName("progress");

	int i=0;
	for(pFile=m_logList.first();pFile;pFile=m_logList.next())
	{
		progress.setValue( i );
		i++;
		g_pApp->processEvents();

		if ( progress.wasCanceled() )
			break;

		if(pFile->type()==KviLogFile::Channel && !bShowChannel)
			continue;
		if(pFile->type()==KviLogFile::Console && !bShowConsole)
			continue;
		if(pFile->type()==KviLogFile::DccChat && !bShowDccChat)
			continue;
		if(pFile->type()==KviLogFile::Other && !bShowOther)
			continue;
		if(pFile->type()==KviLogFile::Query && !bShowQuery)
			continue;

		if(filterFromDate)
			if(pFile->date()>fromDate)
				continue;

		if(filterToDate)
			if(pFile->date()<toDate)
				continue;

		if(enableNameFilter)
			if(!KviQString::matchString(nameFilterText,pFile->name()))
				continue;

		if(enableContentFilter)
		{
			pFile->getText(textBuffer,m_szLogDirectory);
			if(!KviQString::matchString(contentFilterText,textBuffer))
				continue;
		}

		if(pLastCategory)
		{
			if(pLastCategory->m_type!=pFile->type())
				pLastCategory = new KviLogListViewItemType(m_pListView,pFile->type());
		} else {
			pLastCategory = new KviLogListViewItemType(m_pListView,pFile->type());
		}

		KviQString::sprintf(szCurGroup,__tr2qs_ctx("%Q on %Q","logview"),&(pFile->name()),
			&(pFile->network())
			);

		if(szLastGroup!=szCurGroup) {
			szLastGroup=szCurGroup;
			pLastGroupItem=new KviLogListViewItemFolder(pLastCategory,szLastGroup);
		}
		new KviLogListViewLog(pLastGroupItem,pFile->type(),pFile);
	}
	progress.setValue( m_logList.count() );
	m_pListView->sortItems(0, Qt::AscendingOrder);
	m_pFilterButton->setEnabled(true);
}

void KviLogViewMDIWindow::cacheFileList()
{
	QStringList m_pFileNames = getFileNames();
	m_pFileNames.sort();
	QString szFname;

	for(QStringList::Iterator it = m_pFileNames.begin(); it != m_pFileNames.end(); ++it)
	{
		szFname=(*it);
		QFileInfo fi(szFname);
		if(fi.suffix()=="gz" || fi.suffix()=="log")
			m_logList.append(new KviLogFile(szFname));
	}

	setupItemList();
}

void KviLogViewMDIWindow::itemSelected(QTreeWidgetItem * it,QTreeWidgetItem *)
{
	//A parent node
	m_pIrcView->clearBuffer();
	if(!it || !it->parent() || !(((KviLogListViewItem *)it)->m_pFileData) )
	{
		return;
	}

	QString text;
	((KviLogListViewItem *)it)->m_pFileData->getText(text,m_szLogDirectory);

	QStringList lines= text.split('\n');
	bool bOk;
	int iMsgType;
	for ( QStringList::Iterator it = lines.begin(); it != lines.end(); ++it ) {
		QString num=(*it).section(' ',0,0);
		iMsgType=num.toInt(&bOk);
		if(bOk)
			outputNoFmt(iMsgType,(*it).section(' ',1),KviIrcView::NoRepaint | KviIrcView::NoTimestamp);
		else
			outputNoFmt(0,*it,KviIrcView::NoRepaint | KviIrcView::NoTimestamp);
	}
	m_pIrcView->repaint();
}

QStringList KviLogViewMDIWindow::getFileNames()
{
	QString logPath;
	g_pApp->getLocalKvircDirectory(logPath,KviApp::Log);
	QString qPath(logPath);
	QDir logDir(qPath);
	return logDir.entryList();
}

void KviLogViewMDIWindow::rightButtonClicked ( QTreeWidgetItem * it, const QPoint &)
{
	if(!it) return;
	m_pListView->setCurrentItem(it);

	KviTalPopupMenu* popup = new KviTalPopupMenu(this);
	if(((KviLogListViewItem *)it)->childCount())
		popup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_QUIT)),__tr2qs_ctx("Remove all these channel/query log files","logview"),this,SLOT(deleteCurrent()));
	else popup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_QUIT)),__tr2qs_ctx("Remove file","logview"),this,SLOT(deleteCurrent()));

	popup->exec( QCursor::pos() );
}

void KviLogViewMDIWindow::deleteCurrent()
{
	KviLogListViewItem* pItem = (KviLogListViewItem *)(m_pListView->currentItem());
	if(pItem)
	{
		if (pItem->childCount())
		{
			if(QMessageBox::question(
				this,
				__tr2qs("Confirm current user logs delete"),
			"Do you really wish to delete all these channel/query logs?", __tr2qs("&Yes"), __tr2qs("&No"),0,1
			) != 0) return;
			KviPointerList <KviLogListViewItem> itemsList;
			itemsList.setAutoDelete(false);
			for(int i=0;i<pItem->childCount();i++)
			{
				if (!pItem->child(i)->childCount())
				{
					itemsList.append((KviLogListViewItem*)pItem->child(i));
					continue;
				}
				KviLogListViewItem* pChild=(KviLogListViewItem*)pItem->child(i);
				for(int j=0;j<pChild->childCount();j++)
				{
					if (!(KviLogListViewItem*)pChild->child(j))
					{
						debug ("Null pointer in logviewitem");
						continue;
					}
					itemsList.append((KviLogListViewItem*)pChild->child(j));
				}
			}
			for(unsigned int i=0;i<itemsList.count();i++)
			{
				KviLogListViewItem *pCurItem=itemsList.at(i);
				if(!pCurItem->fileName().isNull())
				{
					QString szFname;
					g_pApp->getLocalKvircDirectory(szFname,KviApp::Log,pCurItem->fileName());
					KviFileUtils::removeFile(szFname);
				}
			}
			delete pItem;
			return;
		}
			if(!pItem->fileName().isNull())
		{
			QString szFname;
			g_pApp->getLocalKvircDirectory(szFname,KviApp::Log,pItem->fileName());
			KviFileUtils::removeFile(szFname);
			delete pItem;
			m_pIrcView->clearBuffer();
			if (!pItem->parent()->childCount()) delete pItem->parent();
		}
	}
}

KviLogFile::KviLogFile(const QString & szName)
{
	/*
	Log is in the format
	$type_$nick.$network_$YYYY.$MM.$DD.log
	Examples:
	query_noldor.azzurra_2009.05.20.log
	channel_#slackware.azzurra_2009.11.03.log
	*/

	QString szTmpName = szName;
	m_szFilename = szName;

	QFileInfo fi(m_szFilename);
	m_bCompressed = (fi.suffix() == "gz");
	if(m_bCompressed)
	{
		//removes trailing dot and extension
		szTmpName.chop(3);
	}
	QString szTypeToken = szTmpName.section('_',0,0);
	// Ignore non-logs files, this includes '.' and '..'
	if(KviQString::equalCI(szTypeToken,"channel"))
		m_type = Channel;
	else if(KviQString::equalCI(szTypeToken,"console"))
		m_type = Console;
	else if(KviQString::equalCI(szTypeToken,"dccchat"))
		m_type = DccChat;
	else if(KviQString::equalCI(szTypeToken,"query"))
		m_type = Query;
	else
		m_type = Other;

	KviStr szUndecoded = szTmpName.section('.',0,0);
	szUndecoded.cutToFirst('_');
	m_szName = szUndecoded.hexDecode(szUndecoded.ptr()).ptr();

	szUndecoded = szTmpName.section('.',1).section('_',0,-2);
	m_szNetwork = szUndecoded.hexDecode(szUndecoded.ptr()).ptr();

	QString szDate = szTmpName.section('_',-1).section('.',0,-2);
	int iYear = szDate.section('.',0,0).toInt();
	int iMonth = szDate.section('.',1,1).toInt();
	int iDay = szDate.section('.',2,2).toInt();
	m_date.setYMD(iYear,iMonth,iDay);

	//debug("type=%i, name=%s, net=%s, date=%i %i %i",m_type,m_szName.ascii(),m_szNetwork.ascii(),iYear,iMonth,iDay);
}

void KviLogFile::getText(QString & text,const QString& logDir){
	QString logName = logDir;
	QFile logFile;
	logName.append(fileName());
#ifdef COMPILE_ZLIB_SUPPORT
	if(m_bCompressed)
	{
		gzFile file=gzopen(logName.toLocal8Bit().data(),"rb");
		if(file)
		{
			char buff[1025];
			int len;
			QByteArray data;
			//QCString data;
			len=gzread(file,buff,1024);
			while(len>0)
			{
				buff[len]=0;
				data.append(buff);
				len=gzread(file,buff,1024);
			}
			gzclose(file);
			text = QString::fromUtf8(data);
		} else {
			debug("Cannot open compressed file %s",logName.toLocal8Bit().data());
		}
	} else {
#endif
		logFile.setFileName(logName);

		if(!logFile.open(QIODevice::ReadOnly))
		return;

		QByteArray bytes;
		bytes=logFile.readAll();
		text = QString::fromUtf8(bytes.data(), bytes.size());
		logFile.close();
#ifdef COMPILE_ZLIB_SUPPORT
	}
#endif
}

KviLogViewListView::KviLogViewListView(QWidget * par)
: QTreeWidget(par)
{
	header()->setSortIndicatorShown(true);
	setColumnCount(1);
	setHeaderLabel(__tr2qs_ctx("Log File","logview"));
	setSelectionMode(QAbstractItemView::SingleSelection);
	setSortingEnabled(true);
	setRootIsDecorated(true);
	setAnimated(true);
}

void KviLogViewListView::mousePressEvent (QMouseEvent *e)
{
	if (e->button() == Qt::RightButton)
	{
		QTreeWidgetItem *i=itemAt(e->pos());
		if (i) emit rightButtonPressed(i,QCursor::pos());
	}
	QTreeWidget::mousePressEvent(e);
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "logviewmdiwindow.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
