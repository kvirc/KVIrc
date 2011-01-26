//=============================================================================
//
//   File : LogViewWindow.cpp
//   Creation date : Tue Apr 23 2002 18:08:22 by Juanjo Alvarez
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2002 Juanjo Alvarez
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "LogViewWindow.h"
#include "LogViewWidget.h"

#include "KviIconManager.h"
#include "KviLocale.h"
#include "KviModule.h"
#include "KviOptions.h"
#include "KviMainWindow.h"
#include "KviIrcView.h"
#include "KviQString.h"
#include "KviApplication.h"
#include "KviFileUtils.h"
#include "KviTalPopupMenu.h"

#include <QList>
#include <QPixmap>
#include <QToolButton>
#include <QFileInfo>
#include <QDir>
#include <QCursor>
#include <QHeaderView>
#include <QLayout>
#include <QPushButton>
#include <QTextCodec>
#include <QDateTimeEdit>
#include <QLineEdit>
#include <QLabel>
#include <QShortcut>
#include <QMouseEvent>
#include <QByteArray>
#include <QMessageBox>
#include <QSplitter>
#include <QProgressBar>

#ifdef COMPILE_ZLIB_SUPPORT
	#include <zlib.h>
#endif

#include <limits.h> //for INT_MAX

extern LogViewWindow * g_pLogViewWindow;

LogViewWindow::LogViewWindow(KviModuleExtensionDescriptor * d,KviMainWindow * lpFrm)
: KviWindow(KviWindow::LogView,lpFrm,"logview"), KviModuleExtension(d)
{
	g_pLogViewWindow = this;
//	m_pLogViewWidget = new KviLogViewWidget(this);

	m_pSplitter = new KviTalSplitter(Qt::Horizontal,this);
	m_pSplitter->setObjectName("main_splitter");
	m_pSplitter->setChildrenCollapsible(false);

	m_pLeftLayout = new KviTalVBox(m_pSplitter);
	m_pTabWidget = new QTabWidget(m_pLeftLayout);
	m_pBottomLayout = new KviTalHBox(m_pLeftLayout);
	m_pProgressBar = new QProgressBar(m_pBottomLayout);
	
	m_pCancelButton = new QPushButton(m_pBottomLayout);
	m_pCancelButton->setText(__tr2qs_ctx("Cancel","logview"));
	connect(m_pCancelButton,SIGNAL(clicked()),this,SLOT(abortFilter()));
	m_pBottomLayout->setVisible(false);

	m_pIndexTab  = new KviTalVBox(m_pTabWidget);
	m_pTabWidget->addTab(m_pIndexTab,__tr2qs_ctx("HelpIndex","logview"));

	m_pListView = new LogViewListView(m_pIndexTab);

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

	m_pIrcView = new KviIrcView(m_pSplitter,g_pMainWindow,this);
	m_pIrcView->setMaxBufferSize(INT_MAX);
	m_pIrcView->setFocusPolicy(Qt::ClickFocus);

	QList<int> li;
	li.append(110);
	li.append(width()-110);
	m_pSplitter->setSizes(li);

	g_pApp->getLocalKvircDirectory(m_szLogDirectory,KviApplication::Log);
	KviQString::ensureLastCharIs(m_szLogDirectory,'/'); // Does this work on Windows?

	m_pTimer = new QTimer(this);
	m_pTimer->setSingleShot(true);
	m_pTimer->setInterval(0);
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(filterNext()));
	//avoid to execute the long time-consuming procedure of log indexing here:
	//we could still be inside the context of the "Browse log files" QAction
	QTimer::singleShot( 0, this, SLOT(cacheFileList()) );
}

LogViewWindow::~LogViewWindow()
{
	g_pLogViewWindow = 0;
}

void LogViewWindow::keyPressEvent(QKeyEvent *e)
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

void LogViewWindow::applyFilter()
{
	setupItemList();
}

QPixmap * LogViewWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KviIconManager::Log);
}

void LogViewWindow::resizeEvent(QResizeEvent *)
{
	m_pSplitter->setGeometry(0,0,width(),height());
}

void LogViewWindow::fillCaptionBuffers()
{
	m_szPlainTextCaption = __tr2qs_ctx("Log Viewer","logview");
}

void LogViewWindow::die()
{
	close();
}

QSize LogViewWindow::sizeHint() const
{
	QSize ret(m_pSplitter->sizeHint().width(),m_pIrcView->sizeHint().height());
	return ret;
}

void LogViewWindow::setupItemList()
{
	if(m_logList.isEmpty())
		return;

	m_pFilterButton->setEnabled(false);
	m_pListView->clear();

	m_bAborted=false;
	m_pBottomLayout->setVisible(true);
	m_pProgressBar->setRange(0, m_logList.count());
	m_pProgressBar->setValue(0);

	m_pLastCategory=0;
	m_pLastGroupItem=0;
	m_logList.first();
	m_pTimer->start(); //singleshot
}

void LogViewWindow::abortFilter()
{
	m_bAborted=true;
}

void LogViewWindow::filterNext()
{
	QString szCurGroup;
	LogFile * pFile=m_logList.current();
	if(!pFile)
		goto filter_last;

	if(pFile->type()==LogFile::Channel && !m_pShowChannelsCheck->isChecked())
		goto filter_next;
	if(pFile->type()==LogFile::Console && !m_pShowConsolesCheck->isChecked())
		goto filter_next;
	if(pFile->type()==LogFile::DccChat && !m_pShowDccChatCheck->isChecked())
		goto filter_next;
	if(pFile->type()==LogFile::Other && !m_pShowOtherCheck->isChecked())
		goto filter_next;
	if(pFile->type()==LogFile::Query && !m_pShowQueryesCheck->isChecked())
		goto filter_next;

	if(m_pEnableFromFilter->isChecked())
		if(pFile->date()>m_pFromDateEdit->date())
			goto filter_next;

	if(m_pEnableToFilter->isChecked())
		if(pFile->date()<m_pToDateEdit->date())
			goto filter_next;

	if(!m_pFileNameMask->text().isEmpty())
		if(!KviQString::matchString(m_pFileNameMask->text(),pFile->name()))
			goto filter_next;

	if(!m_pContentsMask->text().isEmpty())
	{
		QString textBuffer;
		pFile->getText(textBuffer,m_szLogDirectory);
		if(!KviQString::matchString(m_pContentsMask->text(),textBuffer))
			goto filter_next;
	}

	if(m_pLastCategory)
	{
		if(m_pLastCategory->m_type!=pFile->type())
		{
			m_pLastCategory=0;
			for(int i=0;i<m_pListView->topLevelItemCount(); ++i)
			{
				LogListViewItemType * pTmp = (LogListViewItemType*) m_pListView->topLevelItem(i);
				if(pTmp->m_type == pFile->type())
				{
					m_pLastCategory=pTmp;
					break;
				}
			}
			if(!m_pLastCategory)
				m_pLastCategory = new LogListViewItemType(m_pListView,pFile->type());
		}
	} else {
		m_pLastCategory = new LogListViewItemType(m_pListView,pFile->type());
	}

	szCurGroup = QString(__tr2qs_ctx("%1 on %2","logview")).arg(pFile->name(),pFile->network());

	if(m_szLastGroup!=szCurGroup)
	{
		m_szLastGroup=szCurGroup;
		m_pLastGroupItem=0;
		for(int i=0;i<m_pLastCategory->childCount(); ++i)
		{
			LogListViewItemFolder * pTmp = (LogListViewItemFolder*) m_pLastCategory->child(i);
			if(pTmp->text(0) == m_szLastGroup)
			{
				m_pLastGroupItem=pTmp;
				break;
			}
		}
		
		if(!m_pLastGroupItem)
			m_pLastGroupItem=new LogListViewItemFolder(m_pLastCategory,m_szLastGroup);
	}

	new LogListViewLog(m_pLastGroupItem,pFile->type(),pFile);

filter_next:
	pFile = m_logList.next();

filter_last:
	if(pFile && !m_bAborted)
	{
		m_pProgressBar->setValue(m_pProgressBar->value()+1);
		m_pTimer->start(); //singleshot
	} else {
		m_pBottomLayout->setVisible(false);
		m_pListView->sortItems(0, Qt::AscendingOrder);
		m_pProgressBar->setValue(0);
		m_pFilterButton->setEnabled(true);
	}
}

void LogViewWindow::cacheFileList()
{
	QString logPath;
	g_pApp->getLocalKvircDirectory(logPath,KviApplication::Log);
	recurseDirectory(logPath);

	setupItemList();
}

void LogViewWindow::recurseDirectory(const QString& sDir)
{
	QDir dir(sDir);
	QFileInfoList list = dir.entryInfoList();
	for (int iList=0;iList<list.count();iList++)
	{
		QFileInfo info = list[iList];
		if (info.isDir())
		{
			// recursive
			if (info.fileName()!=".." && info.fileName()!=".")
			{
				recurseDirectory(info.filePath());
			}
		} else if(info.suffix()=="gz" || info.suffix()=="log") {
			m_logList.append(new LogFile(info.filePath()));
		}
	}
}

void LogViewWindow::itemSelected(QTreeWidgetItem * it,QTreeWidgetItem *)
{
	//A parent node
	m_pIrcView->clearBuffer();
	if(!it || !it->parent() || !(((LogListViewItem *)it)->m_pFileData) )
	{
		return;
	}

	QString text;
	((LogListViewItem *)it)->m_pFileData->getText(text,m_szLogDirectory);

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

void LogViewWindow::rightButtonClicked ( QTreeWidgetItem * it, const QPoint &)
{
	if(!it) return;
	m_pListView->setCurrentItem(it);

	KviTalPopupMenu* popup = new KviTalPopupMenu(this);
	if(((LogListViewItem *)it)->childCount())
		popup->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::Quit)),__tr2qs_ctx("Remove all these channel/query log files","logview"),this,SLOT(deleteCurrent()));
	else popup->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::Quit)),__tr2qs_ctx("Remove file","logview"),this,SLOT(deleteCurrent()));

	popup->exec( QCursor::pos() );
}

void LogViewWindow::deleteCurrent()
{
	LogListViewItem* pItem = (LogListViewItem *)(m_pListView->currentItem());
	if(pItem)
	{
		if (pItem->childCount())
		{
			if(QMessageBox::question(
				this,
				__tr2qs("Confirm current user logs delete"),
			"Do you really wish to delete all these channel/query logs?", __tr2qs("&Yes"), __tr2qs("&No"),0,1
			) != 0) return;
			KviPointerList <LogListViewItem> itemsList;
			itemsList.setAutoDelete(false);
			for(int i=0;i<pItem->childCount();i++)
			{
				if (!pItem->child(i)->childCount())
				{
					itemsList.append((LogListViewItem*)pItem->child(i));
					continue;
				}
				LogListViewItem* pChild=(LogListViewItem*)pItem->child(i);
				for(int j=0;j<pChild->childCount();j++)
				{
					if (!(LogListViewItem*)pChild->child(j))
					{
						qDebug("Null pointer in logviewitem");
						continue;
					}
					itemsList.append((LogListViewItem*)pChild->child(j));
				}
			}
			for(unsigned int i=0;i<itemsList.count();i++)
			{
				LogListViewItem *pCurItem=itemsList.at(i);
				if(!pCurItem->fileName().isNull())
				{
					QString szFname;
					g_pApp->getLocalKvircDirectory(szFname,KviApplication::Log,pCurItem->fileName());
					KviFileUtils::removeFile(szFname);
				}
			}
			delete pItem;
			return;
		}
			if(!pItem->fileName().isNull())
		{
			QString szFname;
			g_pApp->getLocalKvircDirectory(szFname,KviApplication::Log,pItem->fileName());
			KviFileUtils::removeFile(szFname);
			delete pItem;
			m_pIrcView->clearBuffer();
			if (!pItem->parent()->childCount()) delete pItem->parent();
		}
	}
}

LogFile::LogFile(const QString & szName)
{
	/*
	Log is in the format
	$type_$nick.$network_$YYYY.$MM.$DD.log
	Examples:
	query_noldor.azzurra_2009.05.20.log
	channel_#slackware.azzurra_2009.11.03.log
	*/

	m_szFilename = szName;

	QFileInfo fi(m_szFilename);
	QString szTmpName = fi.fileName();

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

	KviCString szUndecoded = szTmpName.section('.',0,0);
	szUndecoded.cutToFirst('_');
	m_szName = szUndecoded.hexDecode(szUndecoded.ptr()).ptr();

	szUndecoded = szTmpName.section('.',1).section('_',0,-2);
	m_szNetwork = szUndecoded.hexDecode(szUndecoded.ptr()).ptr();

	QString szDate = szTmpName.section('_',-1).section('.',0,-2);
	int iYear = szDate.section('.',0,0).toInt();
	int iMonth = szDate.section('.',1,1).toInt();
	int iDay = szDate.section('.',2,2).toInt();
	m_date.setYMD(iYear,iMonth,iDay);

	//qDebug("type=%i, name=%s, net=%s, date=%i %i %i",m_type,m_szName.ascii(),m_szNetwork.ascii(),iYear,iMonth,iDay);
}

void LogFile::getText(QString & text,const QString&)
{
	QString logName = fileName();
	QFile logFile;
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
			qDebug("Cannot open compressed file %s",logName.toLocal8Bit().data());
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

LogViewListView::LogViewListView(QWidget * par)
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

void LogViewListView::mousePressEvent (QMouseEvent *e)
{
	if (e->button() == Qt::RightButton)
	{
		QTreeWidgetItem *i=itemAt(e->pos());
		if (i) emit rightButtonPressed(i,QCursor::pos());
	}
	QTreeWidget::mousePressEvent(e);
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "LogViewWindow.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
