//=============================================================================
//
//   File : LogViewWindow.cpp
//   Creation date : Tue Apr 23 2002 18:08:22 by Juanjo Alvarez
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002 Juanjo Alvarez
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2011 Elvio Basello (hellvis69 at gmail dot com)
//   Copyright (C) 2014 OmegaPhil (OmegaPhil at startmail dot com)
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

#include "LogViewWindow.h"
#include "LogViewWidget.h"

#include "KviHtmlGenerator.h"
#include "KviIconManager.h"
#include "KviLocale.h"
#include "KviModule.h"
#include "KviOptions.h"
#include "KviMainWindow.h"
#include "KviIrcView.h"
#include "KviQString.h"
#include "KviApplication.h"
#include "KviFileUtils.h"
#include "KviFileDialog.h"
#include "KviControlCodes.h"

#include "ExportOperation.h"

#include <QList>
#include <QFileInfo>
#include <QDir>
#include <QCursor>
#include <QHeaderView>
#include <QPushButton>
#include <QDateEdit>
#include <QLineEdit>
#include <QLabel>
#include <QMouseEvent>
#include <QMessageBox>
#include <QProgressBar>
#include <QProgressDialog>
#include <QTextStream>
#include <QTabWidget>
#include <QCheckBox>
#include <QMenu>
#include <QtConcurrent>

#include <climits> //for INT_MAX

extern LogViewWindow * g_pLogViewWindow;

LogViewListView::LogViewListView(QWidget * pParent)
    : QTreeWidget(pParent)
{
	header()->setSortIndicatorShown(true);
	setColumnCount(1);
	setHeaderLabel(__tr2qs_ctx("Log File", "log"));
	setSelectionMode(QAbstractItemView::SingleSelection);
	setSortingEnabled(true);
	setRootIsDecorated(true);
	setAnimated(true);
}

void LogViewListView::mousePressEvent(QMouseEvent * pEvent)
{
	if(pEvent->button() == Qt::RightButton)
	{
		QTreeWidgetItem * pItem = itemAt(pEvent->pos());
		if(pItem)
			emit rightButtonPressed(pItem, QCursor::pos());
	}
	QTreeWidget::mousePressEvent(pEvent);
}

LogViewWindow::LogViewWindow()
    : KviWindow(KviWindow::LogView, "log")
{
	g_pLogViewWindow = this;
	//m_pLogViewWidget = new KviLogViewWidget(this);

	m_pSplitter = new KviTalSplitter(Qt::Horizontal, this);
	m_pSplitter->setObjectName("main_splitter");
	m_pSplitter->setChildrenCollapsible(false);

	m_pLeftLayout = new KviTalVBox(m_pSplitter);
	m_pTabWidget = new QTabWidget(m_pLeftLayout);
	m_pBottomLayout = new KviTalHBox(m_pLeftLayout);
	m_pProgressBar = new QProgressBar(m_pBottomLayout);

	m_pCancelButton = new QPushButton(m_pBottomLayout);
	m_pCancelButton->setText(__tr2qs_ctx("Cancel", "log"));
	connect(m_pCancelButton, SIGNAL(clicked()), this, SLOT(abortFilter()));
	m_pBottomLayout->setVisible(false);

	m_pIndexTab = new KviTalVBox(m_pTabWidget);
	m_pTabWidget->addTab(m_pIndexTab, __tr2qs_ctx("Index", "log"));

	m_pListView = new LogViewListView(m_pIndexTab);

	connect(m_pListView, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(itemSelected(QTreeWidgetItem *, QTreeWidgetItem *)));
	connect(m_pListView, SIGNAL(rightButtonPressed(QTreeWidgetItem *, QPoint)), this, SLOT(rightButtonClicked(QTreeWidgetItem *, QPoint)));

	m_pSearchTab = new QWidget(m_pTabWidget);
	m_pTabWidget->addTab(m_pSearchTab, __tr2qs_ctx("Filter", "log"));

	QGridLayout * pLayout = new QGridLayout(m_pSearchTab);

	m_pShowChannelsCheck = new QCheckBox(__tr2qs_ctx("Show channel logs", "log"), m_pSearchTab);
	m_pShowChannelsCheck->setChecked(true);
	pLayout->addWidget(m_pShowChannelsCheck, 0, 0, 1, 2);

	m_pShowQueryesCheck = new QCheckBox(__tr2qs_ctx("Show query logs", "log"), m_pSearchTab);
	m_pShowQueryesCheck->setChecked(true);
	pLayout->addWidget(m_pShowQueryesCheck, 1, 0, 1, 2);

	m_pShowConsolesCheck = new QCheckBox(__tr2qs_ctx("Show console logs", "log"), m_pSearchTab);
	m_pShowConsolesCheck->setChecked(true);
	pLayout->addWidget(m_pShowConsolesCheck, 2, 0, 1, 2);

	m_pShowDccChatCheck = new QCheckBox(__tr2qs_ctx("Show DCC chat logs", "log"), m_pSearchTab);
	m_pShowDccChatCheck->setChecked(true);
	pLayout->addWidget(m_pShowDccChatCheck, 3, 0, 1, 2);

	m_pShowOtherCheck = new QCheckBox(__tr2qs_ctx("Show other logs", "log"), m_pSearchTab);
	m_pShowOtherCheck->setChecked(true);
	pLayout->addWidget(m_pShowOtherCheck, 4, 0, 1, 2);

	QLabel * pLabel;
	pLabel = new QLabel(__tr2qs_ctx("Contents Filter", "log"), m_pSearchTab);
	pLayout->addWidget(pLabel, 5, 0, 1, 2);

	pLabel = new QLabel(__tr2qs_ctx("Log name mask:", "log"), m_pSearchTab);
	m_pFileNameMask = new QLineEdit(m_pSearchTab);
	pLayout->addWidget(pLabel, 6, 0);
	pLayout->addWidget(m_pFileNameMask, 6, 1);
	connect(m_pFileNameMask, SIGNAL(returnPressed()), this, SLOT(applyFilter()));

	pLabel = new QLabel(__tr2qs_ctx("Log contents mask:", "log"), m_pSearchTab);
	m_pContentsMask = new QLineEdit(m_pSearchTab);
	pLayout->addWidget(pLabel, 7, 0);
	pLayout->addWidget(m_pContentsMask, 7, 1);
	connect(m_pContentsMask, SIGNAL(returnPressed()), this, SLOT(applyFilter()));

	m_pEnableFromFilter = new QCheckBox(__tr2qs_ctx("Only older than:", "log"), m_pSearchTab);
	m_pFromDateEdit = new QDateEdit(m_pSearchTab);
	m_pFromDateEdit->setDate(QDate::currentDate());
	m_pFromDateEdit->setEnabled(false);
	pLayout->addWidget(m_pEnableFromFilter, 8, 0);
	pLayout->addWidget(m_pFromDateEdit, 8, 1);
	connect(m_pEnableFromFilter, SIGNAL(toggled(bool)), m_pFromDateEdit, SLOT(setEnabled(bool)));

	m_pEnableToFilter = new QCheckBox(__tr2qs_ctx("Only newer than:", "log"), m_pSearchTab);
	m_pToDateEdit = new QDateEdit(m_pSearchTab);
	m_pToDateEdit->setDate(QDate::currentDate());
	m_pToDateEdit->setEnabled(false);
	pLayout->addWidget(m_pEnableToFilter, 9, 0);
	pLayout->addWidget(m_pToDateEdit, 9, 1);
	connect(m_pEnableToFilter, SIGNAL(toggled(bool)), m_pToDateEdit, SLOT(setEnabled(bool)));

	m_pFilterButton = new QPushButton(__tr2qs_ctx("Apply Filter", "log"), m_pSearchTab);
	pLayout->addWidget(m_pFilterButton, 10, 1);
	connect(m_pFilterButton, SIGNAL(clicked()), this, SLOT(applyFilter()));

	QWidget * pWidget = new QWidget(m_pSearchTab);
	pWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	pLayout->addWidget(pWidget, 11, 1);

	m_pIrcView = new KviIrcView(m_pSplitter, this);
	m_pIrcView->setMaxBufferSize(INT_MAX);
	m_pIrcView->setFocusPolicy(Qt::ClickFocus);

	QList<int> li;
	li.append(110);
	li.append(width() - 110);
	m_pSplitter->setSizes(li);

	// Using setData to track the option ordinal that used to be passed as id
	m_pExportLogPopup = new QMenu("exportlog", this);
	QAction * pAction = m_pExportLogPopup->addAction(__tr2qs_ctx("Plain Text File", "log"));
	pAction->setData(LogFile::PlainText);
	pAction = m_pExportLogPopup->addAction(__tr2qs_ctx("HTML Archive", "log"));
	pAction->setData(LogFile::HTML);
	//m_pExportLogPopup->addAction(__tr2qs_ctx("XML file","log"));
	//m_pExportLogPopup->addAction(__tr2qs_ctx("database file","log"));

	// 18.11.14: Originally this hooked activated, but Qt4 kept sending bullshit IDs here
	// https://github.com/kvirc/KVIrc/issues/934#issuecomment-124933890
	connect(m_pExportLogPopup, SIGNAL(triggered(QAction *)), this, SLOT(exportLog(QAction *)));

	m_pTimer = new QTimer(this);
	m_pTimer->setSingleShot(true);
	m_pTimer->setInterval(0);
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(filterNext()));
	//avoid to execute the long time-consuming procedure of log indexing here:
	//we could still be inside the context of the "Browse log files" QAction
	QTimer::singleShot(0, this, SLOT(cacheFileList()));
}

LogViewWindow::~LogViewWindow()
{
	g_pLogViewWindow = nullptr;
}

void LogViewWindow::keyPressEvent(QKeyEvent * pEvent)
{
	if((pEvent->modifiers() & Qt::ControlModifier) || (pEvent->modifiers() & Qt::AltModifier) || (pEvent->modifiers() & Qt::MetaModifier))
	{
		if(pEvent->key() == Qt::Key_F)
		{
			m_pIrcView->toggleToolWidget();
			return;
		}
	}
	KviWindow::keyPressEvent(pEvent);
}

QPixmap * LogViewWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KviIconManager::Log);
}

void LogViewWindow::resizeEvent(QResizeEvent *)
{
	m_pSplitter->setGeometry(0, 0, width(), height());
}

void LogViewWindow::fillCaptionBuffers()
{
	m_szPlainTextCaption = __tr2qs_ctx("Log Viewer", "log");
}

void LogViewWindow::die()
{
	close();
}

QSize LogViewWindow::sizeHint() const
{
	QSize ret(m_pSplitter->sizeHint().width(), m_pIrcView->sizeHint().height());
	return ret;
}

void LogViewWindow::recurseDirectory(const QString & szDir)
{
	QDir dir(szDir);
	QFileInfoList list = dir.entryInfoList();
	for(int i = 0; i < list.count(); i++)
	{
		QFileInfo info = list[i];
		if(info.isDir())
		{
			// recursive
			if((info.fileName() != "..") && (info.fileName() != "."))
				recurseDirectory(info.filePath());
		}
		else if((info.suffix() == "gz") || (info.suffix() == "log"))
		{
			m_logList.emplace_back(new LogFile(info.filePath()));
		}
	}
}

void LogViewWindow::setupItemList()
{
	if(m_logList.empty())
		return;

	m_pFilterButton->setEnabled(false);
	m_pListView->clear();

	m_bAborted = false;
	m_pBottomLayout->setVisible(true);
	m_pProgressBar->setRange(0, m_logList.size());
	m_pProgressBar->setValue(0);

	m_pLastCategory = nullptr;
	m_pLastGroupItem = nullptr;
	m_currentLog = m_logList.begin();
	m_pTimer->start(); //singleshot
}

void LogViewWindow::applyFilter()
{
	setupItemList();
}

void LogViewWindow::abortFilter()
{
	m_bAborted = true;
}

void LogViewWindow::filterNext()
{
	QString szCurGroup;
	std::shared_ptr<LogFile> pFile = *m_currentLog;

	if(m_currentLog == m_logList.end())
		goto filter_last;

	if(pFile->type() == LogFile::Channel && !m_pShowChannelsCheck->isChecked())
		goto filter_next;
	if(pFile->type() == LogFile::Console && !m_pShowConsolesCheck->isChecked())
		goto filter_next;
	if(pFile->type() == LogFile::DccChat && !m_pShowDccChatCheck->isChecked())
		goto filter_next;
	if(pFile->type() == LogFile::Other && !m_pShowOtherCheck->isChecked())
		goto filter_next;
	if(pFile->type() == LogFile::Query && !m_pShowQueryesCheck->isChecked())
		goto filter_next;

	if(m_pEnableFromFilter->isChecked())
		if(pFile->date() > m_pFromDateEdit->date())
			goto filter_next;

	if(m_pEnableToFilter->isChecked())
		if(pFile->date() < m_pToDateEdit->date())
			goto filter_next;

	if(!m_pFileNameMask->text().isEmpty())
		if(!KviQString::matchString(m_pFileNameMask->text(), pFile->name()))
			goto filter_next;

	if(!m_pContentsMask->text().isEmpty())
	{
		QString szBuffer;
		pFile->getText(szBuffer);
		if(!KviQString::matchString(m_pContentsMask->text(), szBuffer))
			goto filter_next;
	}

	if(m_pLastCategory)
	{
		if(m_pLastCategory->m_eType != pFile->type())
		{
			m_pLastCategory = nullptr;
			for(int i = 0; i < m_pListView->topLevelItemCount(); ++i)
			{
				LogListViewItemType * pTmp = (LogListViewItemType *)m_pListView->topLevelItem(i);
				if(pTmp->m_eType == pFile->type())
				{
					m_pLastCategory = pTmp;
					break;
				}
			}
			if(!m_pLastCategory)
				m_pLastCategory = new LogListViewItemType(m_pListView, pFile->type());
		}
	}
	else
	{
		m_pLastCategory = new LogListViewItemType(m_pListView, pFile->type());
	}

	szCurGroup = __tr2qs_ctx("%1 on %2", "log").arg(pFile->name(), pFile->network());

	if(m_szLastGroup != szCurGroup)
	{
		m_szLastGroup = szCurGroup;
		m_pLastGroupItem = nullptr;
		for(int i = 0; i < m_pLastCategory->childCount(); ++i)
		{
			LogListViewItemFolder * pTmp = (LogListViewItemFolder *)m_pLastCategory->child(i);
			if(pTmp->text(0) == m_szLastGroup)
			{
				m_pLastGroupItem = pTmp;
				break;
			}
		}

		if(!m_pLastGroupItem)
			m_pLastGroupItem = new LogListViewItemFolder(m_pLastCategory, m_szLastGroup);
	}

	new LogListViewLog(m_pLastGroupItem, pFile->type(), pFile);

filter_next:
	++m_currentLog;

filter_last:
	if((m_currentLog != m_logList.end()) && !m_bAborted)
	{
		m_pProgressBar->setValue(m_pProgressBar->value() + 1);
		m_pTimer->start(); //singleshot
	}
	else
	{
		m_pBottomLayout->setVisible(false);
		m_pListView->sortItems(0, Qt::AscendingOrder);
		m_pProgressBar->setValue(0);
		m_pFilterButton->setEnabled(true);

		// Reset m_szLastGroup for next search
		m_szLastGroup = "";
	}
}

void LogViewWindow::cacheFileList()
{
	QString szLogPath;
	g_pApp->getLocalKvircDirectory(szLogPath, KviApplication::Log);
	recurseDirectory(szLogPath);

	setupItemList();
}

void LogViewWindow::itemSelected(QTreeWidgetItem * it, QTreeWidgetItem *)
{
	//A parent node
	m_pIrcView->clearBuffer();
	if(!it || !it->parent() || !(((LogListViewItem *)it)->m_pFileData))
		return;

	QString szText;
	((LogListViewItem *)it)->m_pFileData->getText(szText);

	QStringList lines = szText.split('\n');
	bool bOk;
	int iMsgType;
	for(auto & line : lines)
	{
		QString szNum = line.section(' ', 0, 0);
		iMsgType = szNum.toInt(&bOk);
		if(iMsgType < 0 || iMsgType > (KVI_NUM_MSGTYPE_OPTIONS - 1))
			iMsgType = 0;
		if(bOk)
			outputNoFmt(iMsgType, line.section(' ', 1), KviIrcView::NoRepaint | KviIrcView::NoTimestamp);
		else
			outputNoFmt(0, line, KviIrcView::NoRepaint | KviIrcView::NoTimestamp);
	}
	m_pIrcView->repaint();
}

void LogViewWindow::rightButtonClicked(QTreeWidgetItem * pItem, const QPoint &)
{
	if(!pItem)
		return;
	m_pListView->setCurrentItem(pItem);

	QMenu * pPopup = new QMenu(this);
	if(((LogListViewItem *)pItem)->childCount())
	{
		pPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Save)), __tr2qs_ctx("Export All Log Files to", "log"))->setMenu(m_pExportLogPopup);
		pPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Discard)), __tr2qs_ctx("Remove All Log Files Within This Folder", "log"), this, SLOT(deleteCurrent()));
	}
	else
	{
		pPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Save)), __tr2qs_ctx("Export Log File to", "log"))->setMenu(m_pExportLogPopup);
		pPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Discard)), __tr2qs_ctx("Remove Log File", "log"), this, SLOT(deleteCurrent()));
	}

	pPopup->exec(QCursor::pos());
}

void LogViewWindow::deleteCurrent()
{
	LogListViewItem * pItem = dynamic_cast<LogListViewItem *>(m_pListView->currentItem());
	if(!pItem)
		return;

	if(!pItem->childCount())
	{
		if(!pItem->fileName().isNull())
		{
			if(QMessageBox::question(
			       this,
			       __tr2qs_ctx("Confirm Current User Log Deletion", "log"),
			       __tr2qs_ctx("Do you really wish to delete this log?", "log"))
			    != QMessageBox::Yes)
				return;

			KviFileUtils::removeFile(pItem->fileName());
			if(!pItem->parent()->childCount())
				delete pItem->parent();

			delete pItem;
			m_pIrcView->clearBuffer();
		}
		return;
	}

	if(QMessageBox::question(
	       this,
	       __tr2qs_ctx("Confirm Current User Logs Deletion", "log"),
	       __tr2qs_ctx("Do you really wish to delete all these logs?", "log"))
	    != QMessageBox::Yes)
		return;
	KviPointerList<LogListViewItem> itemsList;
	itemsList.setAutoDelete(false);
	for(int i = 0; i < pItem->childCount(); i++)
	{
		if(!pItem->child(i)->childCount())
		{
			itemsList.append((LogListViewItem *)pItem->child(i));
			continue;
		}
		LogListViewItem * pChild = (LogListViewItem *)pItem->child(i);
		for(int j = 0; j < pChild->childCount(); j++)
		{
			if(!(LogListViewItem *)pChild->child(j))
			{
				qDebug("Null pointer in logviewitem");
				continue;
			}
			itemsList.append((LogListViewItem *)pChild->child(j));
		}
	}
	for(unsigned int u = 0; u < itemsList.count(); u++)
	{
		LogListViewItem * pCurItem = itemsList.at(u);
		if(!pCurItem->fileName().isNull())
			KviFileUtils::removeFile(pCurItem->fileName());
	}
	delete pItem;
}

void LogViewWindow::exportLog(QAction * pAction)
{
	/* This slot is required as Qt4 has started to screw up the menu ordinal
	 * sent with the old activated signal - the ordinal is now stored as
	 * QAction user data */
	if(pAction)
		exportLog(static_cast<LogFile::ExportType>(pAction->data().toInt()));
	else
		qDebug("LogViewWindow::exportLog called with invalid pAction");
}

void LogViewWindow::exportLog(LogFile::ExportType exportType)
{
	LogListViewItem * pItem = (LogListViewItem *)(m_pListView->currentItem());
	if(!pItem)
		return;

	if(!pItem->childCount())
	{
		std::shared_ptr<LogFile> pLog { pItem->log() };

		QString szDate = pLog->date().toString("yyyy.MM.dd");

		QString szLog = KVI_OPTION_STRING(KviOption_stringLogsExportPath).trimmed();
		if(!szLog.isEmpty())
			szLog += KVI_PATH_SEPARATOR_CHAR;
		szLog += QString("%1_%2.%3_%4").arg(pLog->typeString(), pLog->name(), pLog->network(), szDate);
		KviFileUtils::adjustFilePath(szLog);

		// Getting output file path from the user, with overwrite confirmation
		if(!KviFileDialog::askForSaveFileName(
		       szLog,
		       __tr2qs_ctx("Export Log - KVIrc", "log"),
		       szLog,
		       QString(),
		       false,
		       true,
		       true,
		       this))
			return;

		// Export the log
		pLog->createLog(exportType, szLog);
		return;
	}

	// We selected a node in the log list, scan the children
	std::vector<std::shared_ptr<LogFile>> logList;
	for(int i = 0; i < pItem->childCount(); i++)
	{
		if(!pItem->child(i)->childCount())
		{
			// The child is a log file, append it to the list
			LogListViewItem * pViewItem = static_cast<LogListViewItem *>(pItem->child(i));
			std::shared_ptr<LogFile> pLog { pViewItem->log() };
			logList.push_back(pLog);
			continue;
		}

		// The child is a node, scan it
		LogListViewItem * pChild = (LogListViewItem *)pItem->child(i);
		for(int j = 0; j < pChild->childCount(); j++)
		{
			if(!(LogListViewItem *)pChild->child(j))
			{
				qDebug("Null pointer in logviewitem");
				continue;
			}

			// Add the child to the list
			LogListViewItem * pViewItem = static_cast<LogListViewItem *>(pItem->child(j));
			std::shared_ptr<LogFile> pLog { pViewItem->log() };
			logList.push_back(pLog);
		}
	}

	// Select output directory
	QString szDir = KVI_OPTION_STRING(KviOption_stringLogsExportPath).trimmed();
	if(!KviFileDialog::askForDirectoryName(
	       szDir,
	       __tr2qs_ctx("Export Log - KVIrc", "log"),
	       szDir,
	       QString(),
	       false,
	       true,
	       this))
		return;
	KVI_OPTION_STRING(KviOption_stringLogsExportPath) = szDir;

	// Begin asynchronously writing the logs to persistent storage
	ExportOperation * worker = new ExportOperation(logList, exportType, szDir);
	worker->start();
}
