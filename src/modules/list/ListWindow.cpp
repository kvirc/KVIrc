//=============================================================================
//
//   File : ListWindow.cpp
//   Creation date : Thu Oct 7 2001 13:27:55 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the lists of the GNU General Public License
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

#include "ListWindow.h"

#include "kvi_debug.h"
#include "KviIconManager.h"
#include "KviIrcView.h"
#include "kvi_out.h"
#include "KviOptions.h"
#include "KviLocale.h"
#include "KviControlCodes.h"
#include "KviThemedLabel.h"
#include "KviIrcConnection.h"
#include "KviTopicWidget.h"
#include "KviConfigurationFile.h"
#include "KviFileDialog.h"
#include "kvi_fileextensions.h"
#include "KviTalHBox.h"
#include "KviHtmlGenerator.h"
#include "KviThemedLineEdit.h"
#include "KviIrcMessage.h"

#include <QTimer>
#include <QHeaderView>
#include <QPainter>
#include <QFontMetrics>
#include <QToolTip>
#include <QDateTime>
#include <QByteArray>
#include <QMessageBox>

extern KviPointerList<ListWindow> * g_pListWindowList;

ChannelTreeWidgetItemData::ChannelTreeWidgetItemData(const QString & szChan, const QString & szUsers, const QString & szTopic)
{
	m_szChan = szChan;
	m_szUsers = szUsers;
	m_szTopic = szTopic;
}

ChannelTreeWidgetItemData::~ChannelTreeWidgetItemData()
    = default;

ChannelTreeWidgetItem::ChannelTreeWidgetItem(ChannelTreeWidgetItemData * pData)
    : QTreeWidgetItem(), m_pData(pData)
{
	setToolTip(0, KviQString::toHtmlEscaped(m_pData->m_szChan));
	setToolTip(1, KviQString::toHtmlEscaped(m_pData->m_szUsers));
	setToolTip(2, KviHtmlGenerator::convertToHtml(KviQString::toHtmlEscaped(m_pData->m_szTopic)));
}

ChannelTreeWidgetItem::~ChannelTreeWidgetItem()
    = default;

bool ChannelTreeWidgetItem::operator<(const QTreeWidgetItem & other) const
{
	switch(treeWidget()->sortColumn())
	{
		case 0:
			//channel
			return m_pData->m_szChan.toUpper() < ((ChannelTreeWidgetItem *)&other)->itemData()->m_szChan.toUpper();
			break;
		case 1:
			//users
			return m_pData->m_szUsers.toInt() < ((ChannelTreeWidgetItem *)&other)->itemData()->m_szUsers.toInt();
			break;
		case 2:
		default:
			//topic
			return KviControlCodes::stripControlBytes(m_pData->m_szTopic.toUpper()) < KviControlCodes::stripControlBytes(((ChannelTreeWidgetItem *)&other)->itemData()->m_szTopic.toUpper());
			break;
	}
}

ChannelTreeWidgetItemDelegate::ChannelTreeWidgetItemDelegate(QTreeWidget * pWidget)
    : QItemDelegate(pWidget)
{
}

ChannelTreeWidgetItemDelegate::~ChannelTreeWidgetItemDelegate()
    = default;

#define BORDER 2

QSize ChannelTreeWidgetItemDelegate::sizeHint(const QStyleOptionViewItem & sovItem, const QModelIndex & index) const
{
	ChannelTreeWidget * treeWidget = (ChannelTreeWidget *)parent();

	int iHeight = treeWidget->fontMetrics().lineSpacing() + BORDER + BORDER;

	ChannelTreeWidgetItem * item = dynamic_cast<ChannelTreeWidgetItem *>(treeWidget->itemFromIndex(index));

	if(!item)
		return { 100, iHeight };

	QFontMetrics fm(sovItem.font);
	switch(index.column())
	{
		case 0:
			//channel
			return QSize(fm.width(item->itemData()->m_szChan), iHeight);
			break;
		case 1:
			//users
			return QSize(fm.width(item->itemData()->m_szUsers.toInt()), iHeight);
			break;
		case 2:
		default:
			//topic
			if(item->itemData()->m_szStrippedTopic.isEmpty())
				item->itemData()->m_szStrippedTopic = KviControlCodes::stripControlBytes(item->itemData()->m_szTopic);
			return QSize(fm.width(item->itemData()->m_szStrippedTopic), iHeight);
			break;
	}
	//make gcc happy
	return QSize();
}

void ChannelTreeWidgetItemDelegate::paint(QPainter * p, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	ChannelTreeWidgetItem * obj = static_cast<ChannelTreeWidgetItem *>(index.internalPointer());

	if(option.state & QStyle::State_Selected)
		p->fillRect(option.rect, option.palette.brush(QPalette::Highlight));

	//reset the color
	p->setPen(option.palette.text().color());

	switch(index.column())
	{
		case 0:
			//channel
			p->drawText(option.rect, obj->itemData()->m_szChan);
			break;
		case 1:
			//users
			p->drawText(option.rect, Qt::AlignHCenter, obj->itemData()->m_szUsers);
			break;
		case 2:
		default:
			//topic
			KviTopicWidget::paintColoredText(p, obj->itemData()->m_szTopic, option.palette, option.rect);
			break;
	}
}

ListWindow::ListWindow(KviConsoleWindow * lpConsole)
    : KviWindow(KviWindow::List, "list", lpConsole), KviExternalServerDataParser()
{
	g_pListWindowList->append(this);

	m_pFlushTimer = nullptr;

	m_pItemList = new KviPointerList<ChannelTreeWidgetItemData>;
	m_pItemList->setAutoDelete(false);

	m_pSplitter = new KviTalSplitter(Qt::Horizontal, this);
	m_pSplitter->setObjectName("splitter");
	m_pSplitter->setChildrenCollapsible(false);
	m_pTopSplitter = new KviTalSplitter(Qt::Horizontal, this);
	m_pTopSplitter->setObjectName("top_splitter");
	m_pTopSplitter->setChildrenCollapsible(false);
	m_pVertSplitter = new KviTalSplitter(Qt::Vertical, m_pSplitter);
	m_pVertSplitter->setObjectName("vsplitter");
	m_pVertSplitter->setChildrenCollapsible(false);

	KviTalHBox * pBox = new KviTalHBox(m_pTopSplitter);
	pBox->setSpacing(1);
	pBox->setMargin(0);

	m_pOpenButton = new QToolButton(pBox);
	m_pOpenButton->setObjectName("import_list");
	m_pOpenButton->setIconSize(QSize(16, 16));
	m_pOpenButton->setIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Folder))));
	KviTalToolTip::add(m_pOpenButton, __tr2qs("Import list"));
	connect(m_pOpenButton, SIGNAL(clicked()), this, SLOT(importList()));

	m_pSaveButton = new QToolButton(pBox);
	m_pSaveButton->setObjectName("export_list");
	m_pSaveButton->setIconSize(QSize(16, 16));
	m_pSaveButton->setIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Save))));
	KviTalToolTip::add(m_pSaveButton, __tr2qs("Export list"));
	connect(m_pSaveButton, SIGNAL(clicked()), this, SLOT(exportList()));

	m_pRequestButton = new QToolButton(pBox);
	m_pRequestButton->setObjectName("request_button");
	m_pRequestButton->setIconSize(QSize(16, 16));
	m_pRequestButton->setIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Update))));
	KviTalToolTip::add(m_pRequestButton, __tr2qs("Request list"));
	connect(m_pRequestButton, SIGNAL(clicked()), this, SLOT(requestList()));

	m_pStopListDownloadButton = new QToolButton(pBox);
	m_pStopListDownloadButton->setObjectName("stoplistdownload_button");
	m_pStopListDownloadButton->setIconSize(QSize(16, 16));
	m_pStopListDownloadButton->setIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Discard))));
	KviTalToolTip::add(m_pStopListDownloadButton, __tr2qs("Stop list download"));
	connect(m_pStopListDownloadButton, SIGNAL(clicked()), this, SLOT(stoplistdownload()));

	m_pParamsEdit = new KviThemedLineEdit(pBox, this, "lineedit");
	pBox->setStretchFactor(m_pParamsEdit, 1);
	KviTalToolTip::add(m_pParamsEdit, __tr2qs("<b>/LIST command parameters:</b><br>Many servers accept special parameters that "
	                                          "allow you to filter the returned entries.<br>"
	                                          "Commonly, masked channel names (*kvirc*) are accepted as parameters, as well as strings "
	                                          "like <b>c&lt;n</b> or <b>c&gt;n</b> where <b>n</b> is the minimum or maximum of users on the channel."));

	connect(m_pParamsEdit, SIGNAL(textEdited(const QString &)), this, SLOT(liveSearch(const QString &)));

	m_pInfoLabel = new KviThemedLabel(m_pTopSplitter, this, "info_label");

	m_pTreeWidget = new ChannelTreeWidget(m_pVertSplitter, this, "list_treewidget");
	m_pTreeWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pTreeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pTreeWidget->setItemDelegate(new ChannelTreeWidgetItemDelegate(m_pTreeWidget));
	QStringList columnLabels;
	columnLabels.append(__tr2qs("Channel"));
	columnLabels.append(__tr2qs("Users"));
	columnLabels.append(__tr2qs("Topic"));
	m_pTreeWidget->setColumnCount(3);
	m_pTreeWidget->setHeaderLabels(columnLabels);
	m_pTreeWidget->setAllColumnsShowFocus(true);
	m_pTreeWidget->setSortingEnabled(true);
	m_pTreeWidget->sortItems(0, Qt::AscendingOrder);
	m_pTreeWidget->setUniformRowHeights(true);

	m_pTreeWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	m_pTreeWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	m_pTreeWidget->header()->setStretchLastSection(false);
	m_pTreeWidget->header()->resizeSection(0, 150);
	m_pTreeWidget->header()->resizeSection(1, 80);
	m_pTreeWidget->header()->resizeSection(2, 450);
	//m_pTreeWidget->header()->setResizeMode(QHeaderView::ResizeToContents); <-- this is too heavy for single-core machines...

	connect(m_pTreeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem *, int)));

	m_pIrcView = new KviIrcView(m_pVertSplitter, this);

	m_pConsole->context()->setListWindowPointer(this);
	connect(m_pConsole->context(), SIGNAL(stateChanged()), this, SLOT(connectionStateChange()));

	connectionStateChange();
}

ListWindow::~ListWindow()
{
	g_pListWindowList->removeRef(this);
	m_pConsole->context()->setListWindowPointer(nullptr);

	if(m_pFlushTimer)
		delete m_pFlushTimer;
	m_pItemList->setAutoDelete(true);
	delete m_pItemList;
}

void ListWindow::getBaseLogFileName(QString & szBuffer)
{
	szBuffer.sprintf("LIST_%d", context()->id());
}

void ListWindow::requestList()
{
	if(m_pConsole->isConnected())
	{
		KviCString parms = m_pParamsEdit->text();
		if(parms.isEmpty())
			m_pConsole->connection()->sendFmtData("list");
		else
		{
			m_pParamsEdit->setText("");
			m_pConsole->connection()->sendFmtData("list %s", m_pConsole->connection()->encodeText(parms.ptr()).data());
		}

		outputNoFmt(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Sent list request: waiting for reply..."));
		m_pRequestButton->setEnabled(false);
	}
	else
	{
		outputNoFmt(KVI_OUT_SYSTEMERROR, __tr2qs("Can't request list: no active connection"));
	}
}

void ListWindow::stoplistdownload()
{
	if(m_pConsole->isConnected())
	{
		m_pConsole->connection()->sendFmtData("list stoplistdownloadnow");
		outputNoFmt(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Stopping the list download...")); //G&N mar 2005
	}
	else
	{
		outputNoFmt(KVI_OUT_SYSTEMERROR, __tr2qs("Can't stop list download: no active connection."));
	}
}

void ListWindow::connectionStateChange()
{
	KviIrcContext::State st = m_pConsole->context()->state();
	m_pRequestButton->setEnabled(st == KviIrcContext::Connected);
	if(st == KviIrcContext::Connected)
	{
		QString szTmp = QString(__tr2qs("Connected to %1 (%2)")).arg(m_pConsole->connection()->currentServerName(), m_pConsole->currentNetworkName());
		m_pInfoLabel->setText(szTmp);
	}
	else
	{
		m_pInfoLabel->setText(__tr2qs("List can't be requested: not connected to a server"));
	}
}

QPixmap * ListWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KviIconManager::List);
}

void ListWindow::resizeEvent(QResizeEvent *)
{
	int iHeight = m_pTopSplitter->sizeHint().height();
	m_pTopSplitter->setGeometry(0, 0, width(), iHeight);
	m_pSplitter->setGeometry(0, iHeight, width(), height() - iHeight);
}

QSize ListWindow::sizeHint() const
{
	return m_pSplitter->sizeHint();
}

void ListWindow::fillCaptionBuffers()
{
	//this both a tooltip and a window title, capitalization rules be dammed
	//should be separated titles and tooltips
	m_szPlainTextCaption = QString(__tr2qs("Channel List [IRC Context %1]")).arg(m_pConsole->context()->id());
}

void ListWindow::die()
{
	close();
}

void ListWindow::exportList()
{
	if(!m_pTreeWidget->topLevelItemCount())
	{
		QMessageBox::warning(nullptr, __tr2qs("Warning While Exporting - KVIrc"), __tr2qs("You can't export an empty list!"));
		return;
	}

	QString szFile;
	if(connection())
	{
		QString szDate;
		QDateTime date = QDateTime::currentDateTime();

		switch(KVI_OPTION_UINT(KviOption_uintOutputDatetimeFormat))
		{
			case 0:
				// this is the equivalent to an empty date.toString() call, but it's needed
				// to ensure qt4 will use the default() locale and not the system() one
				szDate = QLocale().toString(date, "ddd MMM d hh:mm:ss yyyy");
				break;
			case 1:
				szDate = date.toString(Qt::ISODate);
				break;
			case 2:
				szDate = date.toString(Qt::SystemLocaleShortDate);
				break;
		}
		szFile = QString(__tr2qs("Channel list for %1 - %2")).arg(connection()->currentNetworkName(), szDate);
	}
	else
	{
		szFile = __tr2qs("Channel list");
	}

	if(KviFileDialog::askForSaveFileName(szFile, __tr2qs("Enter a Filename - KVIrc"), szFile, __tr2qs("Configuration files (*.kvc)"), false, false, true, this))
	{
		if(QFileInfo(szFile).completeSuffix() != "kvc")
			szFile.append(".kvc");

		KviConfigurationFile cfg(szFile, KviConfigurationFile::Write);
		cfg.clear();

		ChannelTreeWidgetItem * it;
		for(int i = 0; i < m_pTreeWidget->topLevelItemCount(); i++)
		{
			it = (ChannelTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
			cfg.setGroup(it->itemData()->m_szChan);
			// Write properties
			cfg.writeEntry("topic", it->itemData()->m_szTopic);
			cfg.writeEntry("users", it->itemData()->m_szUsers);
		}
	}
}

void ListWindow::importList()
{
	QString szFile;

	if(KviFileDialog::askForOpenFileName(szFile, __tr2qs("Select a File - KVIrc"), QString(), KVI_FILTER_CONFIG, false, false, this))
	{

		m_pItemList->setAutoDelete(true);
		m_pItemList->clear();
		m_pItemList->setAutoDelete(false);

		m_pTreeWidget->clear();

		KviConfigurationFile cfg(szFile, KviConfigurationFile::Read);
		KviConfigurationFileIterator it(*cfg.dict());
		while(it.current())
		{
			cfg.setGroup(it.currentKey());
			m_pItemList->append(
			    new ChannelTreeWidgetItemData(
			        it.currentKey(),
			        cfg.readEntry("users", "0"),
			        cfg.readEntry("topic", "")));
			++it;
		}
		flush();
	}
}

void ListWindow::control(int iMsg)
{
	switch(iMsg)
	{
		case EXTERNAL_SERVER_DATA_PARSER_CONTROL_RESET:
			reset();
			break;
		case EXTERNAL_SERVER_DATA_PARSER_CONTROL_STARTOFDATA:
			startOfList();
			break;
		case EXTERNAL_SERVER_DATA_PARSER_CONTROL_ENDOFDATA:
			endOfList();
			break;
	}
}

void ListWindow::reset()
{
	outputNoFmt(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Reset"));
}

void ListWindow::endOfList()
{
	if(m_pFlushTimer)
	{
		delete m_pFlushTimer;
		m_pFlushTimer = nullptr;
	}
	m_pRequestButton->setEnabled(true);
	outputNoFmt(KVI_OUT_SYSTEMMESSAGE, __tr2qs("Channels list download finished"));
	flush(); // give it the last kick
}

void ListWindow::startOfList()
{
	m_pItemList->setAutoDelete(true);
	m_pItemList->clear();
	m_pItemList->setAutoDelete(false);

	m_pTreeWidget->clear();

	m_pRequestButton->setEnabled(false);
}

void ListWindow::liveSearch(const QString & szText)
{
	QRegExp res(szText, Qt::CaseInsensitive, QRegExp::Wildcard);

	ChannelTreeWidgetItem * pItem = nullptr;
	for(int i = 0; i < m_pTreeWidget->topLevelItemCount(); i++)
	{
		pItem = (ChannelTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		if(pItem->itemData()->m_szChan.contains(res) || pItem->itemData()->m_szTopic.contains(res))
		{
			pItem->setHidden(false);
		}
		else
		{
			pItem->setHidden(true);
		}
	}
}

void ListWindow::processData(KviIrcMessage * pMsg)
{
	if(!m_pFlushTimer)
	{
		m_pFlushTimer = new QTimer(this);
		connect(m_pFlushTimer, SIGNAL(timeout()), this, SLOT(flush()));
		m_pFlushTimer->start(1000);
		m_pRequestButton->setEnabled(false);
	}

	if(m_pParamsEdit->text().isEmpty())
	{
		m_pItemList->append(
		    new ChannelTreeWidgetItemData(
		        pMsg->connection()->decodeText(pMsg->safeParam(1)),
		        pMsg->connection()->decodeText(pMsg->safeParam(2)),
		        pMsg->connection()->decodeText(pMsg->safeTrailing())));
	}
	else
	{
		//rfc2812 permits wildcards here (section 3.2.6)
		QRegExp res(m_pParamsEdit->text(), Qt::CaseInsensitive, QRegExp::Wildcard);
		if(
		    res.exactMatch(pMsg->connection()->decodeText(pMsg->safeParam(1))) || res.exactMatch(pMsg->connection()->decodeText(pMsg->safeTrailing())))
		{
			m_pItemList->append(
			    new ChannelTreeWidgetItemData(
			        pMsg->connection()->decodeText(pMsg->safeParam(1)),
			        pMsg->connection()->decodeText(pMsg->safeParam(2)),
			        pMsg->connection()->decodeText(pMsg->safeTrailing())));
		}
	}

	if(_OUTPUT_VERBOSE)
	{
		QString szTmp = pMsg->connection()->decodeText(pMsg->allParams());
		output(KVI_OUT_LIST, __tr2qs("Processing list: %Q"), &szTmp);
	}
}

void ListWindow::flush()
{
	m_pTreeWidget->setUpdatesEnabled(false); /* for v_scroolbar */
	while(ChannelTreeWidgetItemData * d = m_pItemList->first())
	{
		// Model-View-Delegate-Crappiness...
		//
		// Don't use the constructor that takes QTreeWidget as parent
		// as it will insert the item and call the delegate's sizeHint()
		// function when the item is not yet fully constructed.

		m_pTreeWidget->addTopLevelItem(new ChannelTreeWidgetItem(d));

		m_pItemList->removeFirst();
	}
	m_pTreeWidget->setUpdatesEnabled(true);
	m_pTreeWidget->resizeColumnToContents(2);
	m_pTreeWidget->viewport()->update();
}

void ListWindow::itemDoubleClicked(QTreeWidgetItem * it, int)
{
	QString szText = ((ChannelTreeWidgetItem *)it)->itemData()->m_szChan;

	if(szText.isEmpty())
		return;
	if(!connection())
		return;

	QByteArray dat = connection()->encodeText(szText);
	if(!dat.data())
		return;

	m_pConsole->connection()->sendFmtData("join %s", dat.data());
}

void ListWindow::applyOptions()
{
	m_pTreeWidget->applyOptions();
	m_pIrcView->applyOptions();
	m_pParamsEdit->applyOptions();
	m_pInfoLabel->applyOptions();
	KviWindow::applyOptions();
}
