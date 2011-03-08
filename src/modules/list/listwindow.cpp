//=============================================================================
//
//   File : listwindow.cpp
//   Creation date : Thu Oct 7 2001 13:27:55 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2009 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the lists of the GNU General Public License
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

#include "listwindow.h"

#include "kvi_debug.h"
#include "kvi_iconmanager.h"
#include "kvi_ircview.h"
#include "kvi_out.h"
#include "kvi_options.h"
#include "kvi_locale.h"
#include "kvi_out.h"
#include "kvi_mirccntrl.h"
#include "kvi_themedlabel.h"
#include "kvi_options.h"
#include "kvi_ircconnection.h"
#include "kvi_ircconnection.h"
#include "kvi_qstring.h"
#include "kvi_topicw.h"
#include "kvi_config.h"
#include "kvi_filedialog.h"
#include "kvi_msgbox.h"
#include "kvi_fileextensions.h"
#include "kvi_tal_hbox.h"
#include "kvi_tal_tooltip.h"

#include <QTimer>
#include <QPainter>
#include <QFontMetrics>
#include <QToolTip>
#include <QDateTime>
#include <QByteArray>

extern KviPointerList<KviListWindow> * g_pListWindowList;

KviChannelTreeWidgetItemData::KviChannelTreeWidgetItemData(const QString & szChan, const QString & szUsers, const QString & szTopic)
{
	m_szChan = szChan;
	m_szUsers = szUsers;
	m_szTopic = szTopic;
}

KviChannelTreeWidgetItemData::~KviChannelTreeWidgetItemData()
{
}


KviChannelTreeWidgetItem::KviChannelTreeWidgetItem(KviChannelTreeWidgetItemData * pData)
: QTreeWidgetItem(), m_pData(pData)
{
}

KviChannelTreeWidgetItem::~KviChannelTreeWidgetItem()
{
}

int KviChannelTreeWidgetItem::width(const QFontMetrics & fm, const QTreeWidget *, int iColumn) const
{
	switch(iColumn)
	{
		case 0:
			//channel
			return fm.width(m_pData->m_szChan);
			break;
		case 1:
			//users
			return fm.width(m_pData->m_szUsers.toInt());
			break;
		case 2:
		default:
			//topic
			return fm.width(KviMircCntrl::stripControlBytes(m_pData->m_szTopic));
			break;
	}
}

bool KviChannelTreeWidgetItem::operator<(const QTreeWidgetItem & other) const
{
	switch(treeWidget()->sortColumn())
	{
		case 0:
			//channel
			return m_pData->m_szChan.toUpper() < ((KviChannelTreeWidgetItem*)&other)->itemData()->m_szChan.toUpper();
			break;
		case 1:
			//users
			return m_pData->m_szUsers.toInt() < ((KviChannelTreeWidgetItem*)&other)->itemData()->m_szUsers.toInt();
			break;
		case 2:
		default:
			//topic
			return KviMircCntrl::stripControlBytes(m_pData->m_szTopic.toUpper()) < KviMircCntrl::stripControlBytes(((KviChannelTreeWidgetItem*)&other)->itemData()->m_szTopic.toUpper());
			break;
	}
}

KviChannelTreeWidgetItemDelegate::KviChannelTreeWidgetItemDelegate(QTreeWidget * pWidget)
: QItemDelegate(pWidget)
{
}

KviChannelTreeWidgetItemDelegate::~KviChannelTreeWidgetItemDelegate()
{
}

void KviChannelTreeWidgetItemDelegate::paint(QPainter * p, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	KviChannelTreeWidgetItem* obj = static_cast<KviChannelTreeWidgetItem*>(index.internalPointer());

	if (option.state & QStyle::State_Selected)
		p->fillRect(option.rect, option.palette.brush( QPalette::Highlight ) );

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
			KviTopicWidget::paintColoredText(p,obj->itemData()->m_szTopic,option.palette,option.rect);
			break;
	}
}


KviListWindow::KviListWindow(KviFrame * lpFrm, KviConsole * lpConsole)
: KviWindow(KVI_WINDOW_TYPE_LIST,lpFrm,"list",lpConsole), KviExternalServerDataParser()
{
	g_pListWindowList->append(this);

	m_pFlushTimer = 0;

	m_pItemList = new KviPointerList<KviChannelTreeWidgetItemData>;
	m_pItemList->setAutoDelete(false);

	m_pSplitter = new KviTalSplitter(Qt::Horizontal,this);
	m_pSplitter->setObjectName("splitter");
	m_pTopSplitter = new KviTalSplitter(Qt::Horizontal,this);
	m_pTopSplitter->setObjectName("top_splitter");
	m_pVertSplitter = new KviTalSplitter(Qt::Vertical,m_pSplitter);
	m_pVertSplitter->setObjectName("vsplitter");

	KviTalHBox * pBox = new KviTalHBox(m_pTopSplitter);
	pBox->setSpacing(1);
	pBox->setMargin(0);

	m_pOpenButton = new QToolButton(pBox);
	m_pOpenButton->setObjectName("import_list");
	m_pOpenButton->setIconSize(QSize(16,16));
	m_pOpenButton->setIcon(QIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_OPEN))));
	KviTalToolTip::add(m_pOpenButton,__tr2qs("Import List"));
	connect(m_pOpenButton,SIGNAL(clicked()),this,SLOT(importList()));

	m_pSaveButton = new QToolButton(pBox);
	m_pSaveButton->setObjectName("export_list");
	m_pSaveButton->setIconSize(QSize(16,16));
	m_pSaveButton->setIcon(QIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_SAVE))));
	KviTalToolTip::add(m_pSaveButton,__tr2qs("Export List"));
	connect(m_pSaveButton,SIGNAL(clicked()),this,SLOT(exportList()));

	m_pRequestButton = new QToolButton(pBox);
	m_pRequestButton->setObjectName("request_button");
	m_pRequestButton->setIconSize(QSize(16,16));
	m_pRequestButton->setIcon(QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_LIST))));
	KviTalToolTip::add(m_pRequestButton,__tr2qs("Request List"));
	connect(m_pRequestButton,SIGNAL(clicked()),this,SLOT(requestList()));

	m_pStopListDownloadButton = new QToolButton(pBox);
	m_pStopListDownloadButton->setObjectName("stoplistdownload_button");
	m_pStopListDownloadButton->setIconSize(QSize(16,16));
	m_pStopListDownloadButton->setIcon(QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NICKNAMEPROBLEM))));
	KviTalToolTip::add(m_pStopListDownloadButton,__tr2qs("Stop list download"));
	connect(m_pStopListDownloadButton,SIGNAL(clicked()),this,SLOT(stoplistdownload()));

	m_pParamsEdit = new QLineEdit(pBox);
	pBox->setStretchFactor(m_pParamsEdit,1);
	KviTalToolTip::add(m_pParamsEdit,__tr2qs("<center><b>/LIST command parameters:</b><br>Many servers accept special parameters that " \
		"allow you to filter the returned entries.<br>" \
		"Commonly, masked channel names (*kvirc*) are accepted as parameters, as well as strings " \
		"like <b>c&lt;n</b> or <b>c&gt;n</b> where <b>n</b> is the minimum or maximum of users on the channel.</center>"));

	connect(m_pParamsEdit,SIGNAL(textEdited(const QString &)),this,SLOT(liveSearch(const QString &)));

	m_pInfoLabel = new KviThemedLabel(m_pTopSplitter, this, "info_label");

	m_pTreeWidget  = new QTreeWidget(m_pVertSplitter);
	m_pTreeWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pTreeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pTreeWidget->setItemDelegate(new KviChannelTreeWidgetItemDelegate(m_pTreeWidget));
	QStringList columnLabels;
	columnLabels.append(__tr2qs("Channel"));
	columnLabels.append(__tr2qs("Users"));
	columnLabels.append(__tr2qs("Topic"));
	m_pTreeWidget->setColumnCount(3);
	m_pTreeWidget->setHeaderLabels(columnLabels);
	m_pTreeWidget->setAllColumnsShowFocus(TRUE);
	m_pTreeWidget->setSortingEnabled(TRUE);
	m_pTreeWidget->sortItems(0,Qt::AscendingOrder);
	connect(m_pTreeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),this,SLOT(itemDoubleClicked(QTreeWidgetItem *, int)));

	m_pIrcView = new KviIrcView(m_pVertSplitter,lpFrm,this);

	m_pConsole->context()->setListWindowPointer(this);
	connect(m_pConsole->context(),SIGNAL(stateChanged()),this,SLOT(connectionStateChange()));

	connectionStateChange();
}

KviListWindow::~KviListWindow()
{
	g_pListWindowList->removeRef(this);
	m_pConsole->context()->setListWindowPointer(0);

	if(m_pFlushTimer)
		delete m_pFlushTimer;
	m_pItemList->setAutoDelete(true);
	delete m_pItemList;
}

void KviListWindow::getBaseLogFileName(QString & szBuffer)
{
	szBuffer.sprintf("LIST_%d",context()->id());
}

void KviListWindow::requestList()
{
	if(m_pConsole->isConnected())
	{
		KviStr parms = m_pParamsEdit->text();
		if(parms.isEmpty())
			m_pConsole->connection()->sendFmtData("list");
		else
			m_pConsole->connection()->sendFmtData("list %s",m_pConsole->connection()->encodeText(parms.ptr()).data());

		outputNoFmt(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Sent list request, waiting for reply..."));
		m_pRequestButton->setEnabled(false);
	} else {
		outputNoFmt(KVI_OUT_SYSTEMERROR,__tr2qs("Cannot request list: No active connection"));
	}
}

void KviListWindow::stoplistdownload()
{
	if(m_pConsole->isConnected())
	{
		m_pConsole->connection()->sendFmtData("list stoplistdownloadnow");
		outputNoFmt(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Stopping the list download...")); //G&N mar 2005
	} else {
		outputNoFmt(KVI_OUT_SYSTEMERROR,__tr2qs("Cannot stop list download, no active connection."));
	}
}

void KviListWindow::connectionStateChange()
{
	KviIrcContext::State st = m_pConsole->context()->state();
	m_pRequestButton->setEnabled(st == KviIrcContext::Connected);
	if(st == KviIrcContext::Connected)
	{
		QString szTmp;
		KviQString::sprintf(szTmp,
				__tr2qs("Connected to %s (%s)"),
				m_pConsole->connection()->currentServerName().toUtf8().data(),
				m_pConsole->currentNetworkName().toUtf8().data());
		m_pInfoLabel->setText(szTmp);
	} else {
		m_pInfoLabel->setText(__tr2qs("List cannot be requested: Not connected to a server"));
	}
}

QPixmap * KviListWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KVI_SMALLICON_LIST);
}

void KviListWindow::resizeEvent(QResizeEvent *)
{
	int iHeight = m_pTopSplitter->sizeHint().height();
	m_pTopSplitter->setGeometry(0,0,width(),iHeight);
	m_pSplitter->setGeometry(0,iHeight,width(),height() - iHeight);
}

QSize KviListWindow::sizeHint() const
{
	return m_pSplitter->sizeHint();
}

void KviListWindow::fillCaptionBuffers()
{
	KviQString::sprintf(m_szPlainTextCaption,__tr2qs("Channel List [IRC Context %u]"),m_pConsole->context()->id());
}

void KviListWindow::die()
{
	close();
}

void KviListWindow::exportList()
{
	if(!m_pTreeWidget->topLevelItemCount())
	{
		KviMessageBox::warning(__tr2qs("You cannot export an empty list"));
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
				szDate = date.toString("d MMM yyyy hh-mm");
				break;
			case 1:
				szDate = date.toString(Qt::ISODate);
				break;
			case 2:
				szDate = date.toString(Qt::SystemLocaleDate);
				break;
		}
		KviQString::sprintf(szFile,__tr2qs("Channel list for %Q - %Q"),
			&(connection()->networkName()),&(szDate));
	} else {
		szFile = __tr2qs("Channel list");
	}

	if(KviFileDialog::askForSaveFileName(szFile,__tr2qs("Choose filename"),szFile,__tr2qs("Configuration files (*.kvc)"),false,false,true,this))
	{
		if(QFileInfo(szFile).completeSuffix() != "kvc")
			szFile.append(".kvc");

		KviConfig cfg(szFile,KviConfig::Write);
		cfg.clear();

		KviChannelTreeWidgetItem * it;
		for(int i=0; i < m_pTreeWidget->topLevelItemCount(); i++)
		{
			it = (KviChannelTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
			cfg.setGroup(it->itemData()->m_szChan);
			// Write properties
			cfg.writeEntry("topic",it->itemData()->m_szTopic);
			cfg.writeEntry("users",it->itemData()->m_szUsers);
		}
	}
}

void KviListWindow::importList()
{
	QString szFile;

	if(KviFileDialog::askForOpenFileName(szFile,__tr2qs("Choose filename"),QString(),KVI_FILTER_CONFIG,false,false,this))
	{

		m_pItemList->setAutoDelete(true);
		m_pItemList->clear();
		m_pItemList->setAutoDelete(false);

		m_pTreeWidget->clear();


		KviConfig cfg(szFile,KviConfig::Read);
		KviConfigIterator it(*cfg.dict());
		while(it.current())
		{
			cfg.setGroup(it.currentKey());
			m_pItemList->append(
					new KviChannelTreeWidgetItemData(
						it.currentKey(),
						cfg.readQStringEntry("users","0"),
						cfg.readQStringEntry("topic","")
					)
				);
			++it;
		}
		flush();
	}
}

void KviListWindow::control(int iMsg)
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

void KviListWindow::reset()
{
	outputNoFmt(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Reset"));
}

void KviListWindow::endOfList()
{
	if(m_pFlushTimer)
	{
		delete m_pFlushTimer;
		m_pFlushTimer = 0;
	}
	m_pRequestButton->setEnabled(true);
	outputNoFmt(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Channels list download finished"));
	flush(); // give it the last kick
}

void KviListWindow::startOfList()
{
	m_pItemList->setAutoDelete(true);
	m_pItemList->clear();
	m_pItemList->setAutoDelete(false);

	m_pTreeWidget->clear();

	m_pRequestButton->setEnabled(false);
}

void KviListWindow::liveSearch(const QString & szText)
{
	QRegExp res(szText,Qt::CaseInsensitive,QRegExp::Wildcard);
	
	KviChannelTreeWidgetItem * pItem=0;
	for(int i=0; i<m_pTreeWidget->topLevelItemCount(); i++)
	{
		pItem = (KviChannelTreeWidgetItem*) m_pTreeWidget->topLevelItem(i);
		if(pItem->itemData()->m_szChan.contains(res) ||
			pItem->itemData()->m_szTopic.contains(res))
		{
			pItem->setHidden(false);
		} else {
			pItem->setHidden(true);
		}
	}
}

void KviListWindow::processData(KviIrcMessage * pMsg)
{
	if(!m_pFlushTimer)
	{
		m_pFlushTimer = new QTimer(this);
		connect(m_pFlushTimer,SIGNAL(timeout()),this,SLOT(flush()));
		m_pFlushTimer->start(1000);
		m_pRequestButton->setEnabled(false);
	}

	if(m_pParamsEdit->text().isEmpty())
	{
		m_pItemList->append(
			new KviChannelTreeWidgetItemData(
				pMsg->connection()->decodeText(pMsg->safeParam(1)),
				pMsg->connection()->decodeText(pMsg->safeParam(2)),
				pMsg->connection()->decodeText(pMsg->safeTrailing()))
		);
	} else {
		//rfc2812 permits wildcards here (section 3.2.6)
		QRegExp res(m_pParamsEdit->text(),Qt::CaseInsensitive,QRegExp::Wildcard);
		if(
				res.exactMatch(pMsg->connection()->decodeText(pMsg->safeParam(1))) ||
				res.exactMatch(pMsg->connection()->decodeText(pMsg->safeTrailing()))
			)
		{
			m_pItemList->append(
				new KviChannelTreeWidgetItemData(
					pMsg->connection()->decodeText(pMsg->safeParam(1)),
					pMsg->connection()->decodeText(pMsg->safeParam(2)),
					pMsg->connection()->decodeText(pMsg->safeTrailing())
				)
			);
		}
	}

	if(_OUTPUT_VERBOSE)
	{
		QString szTmp = pMsg->connection()->decodeText(pMsg->allParams());
		output(KVI_OUT_LIST,__tr2qs("Processing list: %Q"),&szTmp);
	}
}

void KviListWindow::flush()
{
	m_pTreeWidget->setUpdatesEnabled(false); /* for v_scroolbar */
	while(KviChannelTreeWidgetItemData * d = m_pItemList->first())
	{
		m_pTreeWidget->addTopLevelItem(new KviChannelTreeWidgetItem(d)); 

		m_pItemList->removeFirst();
	}
	m_pTreeWidget->setUpdatesEnabled(true);
	m_pTreeWidget->viewport()->update();
}

void KviListWindow::itemDoubleClicked(QTreeWidgetItem * it, int)
{
	QString szText = ((KviChannelTreeWidgetItem*)it)->itemData()->m_szChan;

	if(szText.isEmpty()) return;
	if(!connection()) return;

	QByteArray dat = connection()->encodeText(szText);
	if(!dat.data()) return;

	m_pConsole->connection()->sendFmtData("join %s",dat.data());
}


//
//#warning "Load & save properties of this kind of window"

//void KviListWindow::saveProperties()
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
//void KviListWindow::setProperties(KviWindowProperty *p)
//{
//	QValueList<int> l;
//	l.append(p->splitWidth1);
//	l.append(p->splitWidth2);
//	m_pVertSplitter->setSizes(l);
//	m_pIrcView->setTimestamp(p->timestamp);
//	m_pIrcView->setShowImages(p->imagesVisible);
//}

void KviListWindow::applyOptions()
{
	m_pIrcView->applyOptions();
	KviWindow::applyOptions();
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "listwindow.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
