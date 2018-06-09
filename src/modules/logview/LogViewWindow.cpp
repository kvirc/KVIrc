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
#include <QTextStream>
#include <QTabWidget>
#include <QCheckBox>
#include <QMenu>

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
			m_logList.append(new LogFile(info.filePath()));
		}
	}
}

void LogViewWindow::setupItemList()
{
	if(m_logList.isEmpty())
		return;

	m_pFilterButton->setEnabled(false);
	m_pListView->clear();

	m_bAborted = false;
	m_pBottomLayout->setVisible(true);
	m_pProgressBar->setRange(0, m_logList.count());
	m_pProgressBar->setValue(0);

	m_pLastCategory = nullptr;
	m_pLastGroupItem = nullptr;
	m_logList.first();
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
	LogFile * pFile = m_logList.current();
	if(!pFile)
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
	pFile = m_logList.next();

filter_last:
	if(pFile && !m_bAborted)
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
		//pPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::UserList)),__tr2qs_ctx("Export all log files to","log"),m_pExportLogPopup);
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
			       __tr2qs_ctx("Do you really wish to delete this log?", "log"),
			       __tr2qs("Yes"), __tr2qs("No"), nullptr, 1)
			    != 0)
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
	       __tr2qs_ctx("Do you really wish to delete all these logs?", "log"),
	       __tr2qs("Yes"), __tr2qs("No"), nullptr, 1)
	    != 0)
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
		exportLog(pAction->data().toInt());
	else
		qDebug("LogViewWindow::exportLog called with invalid pAction");
}

void LogViewWindow::exportLog(int iId)
{
	LogListViewItem * pItem = (LogListViewItem *)(m_pListView->currentItem());
	if(!pItem)
		return;

	if(!pItem->childCount())
	{
		// Export the log
		createLog(pItem->log(), iId);
		return;
	}

	// We selected a node in the log list, scan the children
	KviPointerList<LogListViewItem> logList;
	logList.setAutoDelete(false);
	for(int i = 0; i < pItem->childCount(); i++)
	{
		if(!pItem->child(i)->childCount())
		{
			// The child is a log file, append it to the list
			logList.append((LogListViewItem *)pItem->child(i));
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
			logList.append((LogListViewItem *)pChild->child(j));
		}
	}

	// Scan the list
	for(unsigned int u = 0; u < logList.count(); u++)
	{
		LogListViewItem * pCurItem = logList.at(u);
		createLog(pCurItem->log(), iId);
	}
}

void LogViewWindow::createLog(LogFile * pLog, int iId, QString * pszFile)
{
	if(!pLog)
		return;

	QRegExp rx;
	QString szLog, szLogDir, szInputBuffer, szOutputBuffer, szLine, szTmp;
	QString szDate = pLog->date().toString("yyyy.MM.dd");

	/* Fetching previous export path and concatenating with generated filename
	 * adjustFilePath is for file paths not directory paths */
	szLog = KVI_OPTION_STRING(KviOption_stringLogsExportPath).trimmed();
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

	/* Save export directory - this directory path is also used in the HTML export
	 * and info is used when working with pszFile */
	QFileInfo info(szLog);
	szLogDir = info.absoluteDir().absolutePath();
	KVI_OPTION_STRING(KviOption_stringLogsExportPath) = szLogDir;

	/* Reading in log file - LogFiles are read in as bytes, so '\r' isn't
	 * sanitised by default */
	pLog->getText(szInputBuffer);
	QStringList lines = szInputBuffer.replace('\r', "").split('\n');

	switch(iId)
	{
		case LogFile::PlainText:
		{
			/* Only append extension if it isn't there already (e.g. a specific
			 * file is to be overwritten) */
			if(!szLog.endsWith(".txt"))
				szLog += ".txt";

			// Scan the file
			for(auto & line : lines)
			{
				szTmp = line;
				szLine = KviControlCodes::stripControlBytes(szTmp);

				// Remove icons' code
				rx.setPattern("^\\d{1,3}\\s");
				szLine.replace(rx, "");

				// Remove link from a user speaking, deal with (and keep) various ranks
				// e.g.: <!ncHelLViS69>  -->  <HelLViS69>
				rx.setPattern("\\s<([+%@&~!]?)!nc");
				szLine.replace(rx, " <\\1");

				// Remove link from a nick in a mask
				// e.g.: !nFoo [~bar@!hfoo.bar]  -->  Foo [~bar@!hfoo.bar]
				rx.setPattern("\\s!n");
				szLine.replace(rx, " ");

				// Remove link from a host in a mask
				// e.g.: Foo [~bar@!hfoo.bar]  -->  Foo [~bar@foo.bar]
				rx.setPattern("@!h");
				szLine.replace(rx, "@");

				// Remove link from a channel
				// e.g.: !c#KVIrc  -->  #KVIrc
				rx.setPattern("!c#");
				szLine.replace(rx, "#");

				szOutputBuffer += szLine;
				szOutputBuffer += "\n";
			}

			break;
		}
		case LogFile::HTML:
		{
			/* Only append extension if it isn't there already (e.g. a specific
			 * file is to be overwritten) */
			if(!szLog.endsWith(".html"))
				szLog += ".html";

			szTmp = QString("KVIrc %1 %2").arg(KVI_VERSION).arg(KVI_RELEASE_NAME);
			QString szNick = "";
			bool bFirstLine = true;

			QString szTitle;
			switch(pLog->type())
			{
				case LogFile::Channel:
					szTitle = __tr2qs_ctx("Channel %1 on %2", "log").arg(pLog->name(), pLog->network());
					break;
				case LogFile::Console:
					szTitle = __tr2qs_ctx("Console on %1", "log").arg(pLog->network());
					break;
				case LogFile::Query:
					szTitle = __tr2qs_ctx("Query with: %1 on %2", "log").arg(pLog->name(), pLog->network());
					break;
				case LogFile::DccChat:
					szTitle = __tr2qs_ctx("DCC Chat with: %1", "log").arg(pLog->name());
					break;
				case LogFile::Other:
					szTitle = __tr2qs_ctx("Something on: %1", "log").arg(pLog->network());
					break;
			}

			// Prepare HTML document
			szOutputBuffer += "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n";
			szOutputBuffer += "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\">\n";
			szOutputBuffer += "<head>\n";
			szOutputBuffer += "\t<meta http-equiv=\"content-type\" content=\"application/xhtml+xml; charset=utf-8\" />\n";
			szOutputBuffer += "\t<meta name=\"author\" content=\"" + szTmp + "\" />\n";
			szOutputBuffer += "\t<title>" + szTitle + "</title>\n";
			szOutputBuffer += "</head>\n<body>\n";
			szOutputBuffer += "<h2>" + szTitle + "</h2>\n<h3>Date: " + szDate + "</h3>\n";

			// Scan the file
			for(auto & line : lines)
			{
				szTmp = line;

				// Find who has talked
				QString szTmpNick = szTmp.section(" ", 2, 2);
				if((szTmpNick.left(1) != "<") && (szTmpNick.right(1) != ">"))
					szTmpNick = "";

				// locate msgtype
				QString szNum = szTmp.section(' ', 0, 0);
				bool bOk;
				int iMsgType = szNum.toInt(&bOk);

				// only human text for now...
				if(iMsgType != 24 && iMsgType != 25 && iMsgType != 26)
					continue;

				// remove msgtype tag
				szTmp = szTmp.remove(0, szNum.length() + 1);

				szTmp = KviHtmlGenerator::convertToHtml(szTmp, true);

				// insert msgtype icon at start of the current text line
				KviMessageTypeSettings msg(KVI_OPTION_MSGTYPE(iMsgType));
				QString szIcon = g_pIconManager->getSmallIconResourceName((KviIconManager::SmallIcon)msg.pixId());
				szTmp.prepend("<img src=\"" + szIcon + R"(" alt="" /> )");

				/*
				 * Check if the nick who has talked is the same of the above line.
				 * If so, we have to put the line as it is, otherwise we have to
				 * open a new paragraph
				 */
				if(szTmpNick != szNick)
				{
					/*
					 * People is not the same, close the paragraph opened
					 * before and open a new one
					 */
					if(!bFirstLine)
						szOutputBuffer += "</p>\n";
					szTmp.prepend("<p>");

					szNick = szTmpNick;
				}
				else
				{
					// Break the line
					szTmp.prepend("<br />\n");
				}

				szOutputBuffer += szTmp;
				bFirstLine = false;
			}

			// Close the last paragraph
			szOutputBuffer += "</p>\n";

			// regexp to search all embedded icons
			rx.setPattern("<img src=\"smallicons:([^\"]+)");
			int iIndex = szOutputBuffer.indexOf(rx);
			QStringList szImagesList;

			// search for icons
			while(iIndex >= 0)
			{
				int iLength = rx.matchedLength();
				QString szCap = rx.cap(1);

				// if the icon isn't in the images list then add
				if(szImagesList.indexOf(szCap) == -1)
					szImagesList.append(szCap);
				iIndex = szOutputBuffer.indexOf(rx, iIndex + iLength);
			}

			// get current theme path
			QString szCurrentThemePath;
			g_pApp->getLocalKvircDirectory(szCurrentThemePath, KviApplication::Themes, KVI_OPTION_STRING(KviOption_stringIconThemeSubdir));
			szCurrentThemePath += KVI_PATH_SEPARATOR_CHAR;

			// current coresmall path
			szCurrentThemePath += "coresmall";
			szCurrentThemePath += KVI_PATH_SEPARATOR_CHAR;

			// check if coresmall exists in current theme
			if(!KviFileUtils::directoryExists(szCurrentThemePath))
			{
				// get global coresmall path
				g_pApp->getGlobalKvircDirectory(szCurrentThemePath, KviApplication::Pics, "coresmall");
				KviQString::ensureLastCharIs(szCurrentThemePath, QChar(KVI_PATH_SEPARATOR_CHAR));
			}

			// copy all icons to the log destination folder
			for(int i = 0; i < szImagesList.count(); i++)
			{
				QString szSourceFile = szCurrentThemePath + szImagesList.at(i);
				QString szDestFile = szLogDir + szImagesList.at(i);
				KviFileUtils::copyFile(szSourceFile, szDestFile);
			}

			// remove internal tags
			rx.setPattern("<qt>|</qt>|smallicons:");
			szOutputBuffer.replace(rx, "");
			szOutputBuffer.replace(">!nc", ">");
			szOutputBuffer.replace("@!nc", "@");
			szOutputBuffer.replace("%!nc", "%");

			// Close the document
			szOutputBuffer += "</body>\n</html>\n";

			break;
		}
	}

	// File overwriting already dealt with when file path was obtained
	QFile log(szLog);
	if(!log.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	if(pszFile)
	{
		*pszFile = "";
		*pszFile = info.filePath();
	}

	// Ensure we're writing in UTF-8
	QTextStream output(&log);
	output.setCodec("UTF-8");
	output << szOutputBuffer;

	// Close file descriptors
	log.close();
}
