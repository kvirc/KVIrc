//=============================================================================
//
//   File : listwindow.cpp
//   Creation date : Thu Oct 7 2001 13:27:55 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2004 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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
#include "kvi_mirccntrl.h"
#include "kvi_ircconnection.h"
#include "kvi_ircconnection.h"
#include "kvi_qstring.h"
#include "kvi_qcstring.h"
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
#include <QSplitter>
#include <QToolTip>
#include <QDateTime>

extern KviPointerList<KviListWindow> * g_pListWindowList;

// kvi_ircview.cpp
//extern KVIRC_API const char * getColorBytes(const char *data_ptr,unsigned char *byte_1,unsigned char *byte_2);


KviChannelTreeWidgetItemData::KviChannelTreeWidgetItemData(const QString &szChan,const QString &szUsers,const QString &szTopic)
{
	m_szChan = szChan;
	m_szUsers = szUsers;
	m_szTopic = szTopic;
	m_szUsersKey = szUsers;
	//setText(0,szChan.upper());
	while(m_szUsersKey.length() < 6)m_szUsersKey.prepend("0");
}

KviChannelTreeWidgetItemData::~KviChannelTreeWidgetItemData()
{
}


KviChannelTreeWidgetItem::KviChannelTreeWidgetItem(KviTalTreeWidget * v,KviChannelTreeWidgetItemData * pData)
: KviTalTreeWidgetItem(v)
{
	m_pData = pData;
}


KviChannelTreeWidgetItem::~KviChannelTreeWidgetItem()
{
	delete m_pData;
}

int KviChannelTreeWidgetItem::width ( const QFontMetrics & fm, const KviTalTreeWidget * lv, int column ) const
{
	debug("width request");
	QString szText;

	switch(column)
	{
		case 0:  szText = m_pData->m_szChan;   break;
		case 1:  szText = m_pData->m_szUsers;  break;
		default: szText = m_pData->m_szTopic;  break;
	}
	if(column==2) return fm.width(KviMircCntrl::stripControlBytes(szText));
	else return fm.width(szText);
}

#define KVI_LABEL_DEF_BACK 100
#define KVI_LABEL_DEF_FORE 101

void KviChannelTreeWidgetItemDelegate::paint( QPainter * p, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	QString szText;
	QPalette cg;
	KviChannelTreeWidgetItem *item=static_cast<KviChannelTreeWidgetItem *>(index.internalPointer());
	switch(index.column())
	{
		case 0:  szText = item->channelData()->m_szChan;   break;
		case 1:  szText = item->channelData()->m_szUsers;  break;
		default: szText = item->channelData()->m_szTopic;  break;
	}

	KviTalTreeWidget* lv = (KviTalTreeWidget *)item->treeWidget();
	// FIXME
	int marg = 5;//lv->itemMargin();
	int r = marg;

	p->fillRect( option.rect.x(), option.rect.y(), option.rect.width(), option.rect.height(), cg.brush(lv->viewport()->backgroundRole()) );
	int width=option.rect.width();

	if ((option.state & QStyle::State_Selected) && (index.column() == 0 || lv->allColumnsShowFocus()) ) {
		p->fillRect( r - marg, 0, width - r + marg, option.rect.height(),
		cg.brush( QPalette::Highlight ) );

		if ( (option.state & QStyle::State_Enabled) || !lv )
			p->setPen( cg.highlightedText().color() );
		else if ( !(option.state & QStyle::State_Enabled) && lv)
			// FIXME
			// p->setPen( lv->palette().currentColorGroup().inactive().highlightedText() );
			p->setPen( lv->palette().highlightedText().color() );
	} else {
		if ( (option.state & QStyle::State_Enabled) || !lv )
			p->setPen( cg.text().color() );
		else if ( !(option.state & QStyle::State_Enabled) && lv)
			//p->setPen( lv->palette().disabled().text() );
			p->setPen(lv->palette().text().color());
	}

	KviTopicWidget::paintColoredText(p,szText,cg,option.rect);
}

QString KviChannelTreeWidgetItem::key(int col,bool) const
{
	switch(col)
	{
		case 0:
			return m_pData->m_szChan;
		break;
		case 1:
			return m_pData->m_szUsersKey;
		break;
		case 2:
			return m_pData->m_szTopic;
		break;
	}
	QString ret;
	return ret;
}


KviListWindow::KviListWindow(KviFrame * lpFrm,KviConsole * lpConsole)
: KviWindow(KVI_WINDOW_TYPE_LIST,lpFrm,"list",lpConsole) , KviExternalServerDataParser()
{
	g_pListWindowList->append(this);

	m_pFlushTimer = 0;

	m_pItemList = new KviPointerList<KviChannelTreeWidgetItemData>;
	m_pItemList->setAutoDelete(false);

	m_pSplitter = new QSplitter(Qt::Horizontal,this);
	m_pSplitter->setObjectName("splitter");
	m_pTopSplitter = new QSplitter(Qt::Horizontal,this);
	m_pTopSplitter->setObjectName("top_splitter");
	m_pVertSplitter = new QSplitter(Qt::Vertical,m_pSplitter);
	m_pVertSplitter->setObjectName("vsplitter");

	KviTalHBox * box = new KviTalHBox(m_pTopSplitter);
	box->setSpacing(1);
	box->setMargin(0);
	m_pOpenButton = new QToolButton(box);
	m_pOpenButton->setIcon(QIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_OPEN))));
	connect(m_pOpenButton,SIGNAL(clicked()),this,SLOT(importList()));

	m_pSaveButton = new QToolButton(box);
	m_pSaveButton->setIcon(QIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_SAVE))));
	connect(m_pSaveButton,SIGNAL(clicked()),this,SLOT(exportList()));

	m_pRequestButton = new QToolButton(box,"request_button");
	m_pRequestButton->setIconSize(QSize(16,16));
	m_pRequestButton->setIcon(QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_LIST))));
	connect(m_pRequestButton,SIGNAL(clicked()),this,SLOT(requestList()));
	KviTalToolTip::add(m_pRequestButton,__tr2qs("Request List"));

	m_pStopListDownloadButton = new QToolButton(box,"stoplistdownload_button");
	m_pStopListDownloadButton->setIconSize(QSize(16,16));
	m_pStopListDownloadButton->setIcon(QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NICKNAMEPROBLEM))));
	connect(m_pStopListDownloadButton,SIGNAL(clicked()),this,SLOT(stoplistdownload()));
	KviTalToolTip::add(m_pStopListDownloadButton,__tr2qs("Stop list download"));

	m_pParamsEdit = new QLineEdit(box);
	box->setStretchFactor(m_pParamsEdit,1);
	KviTalToolTip::add(m_pParamsEdit,__tr2qs("<center><b>/LIST command parameters:</b><br>Many servers accept special parameters that " \
						"allow you to filter the returned entries.<br>" \
						"Commonly, masked channel names (*kvirc*) are accepted as parameters, as well as strings " \
						"like <b>c&lt;n</b> or <b>c&gt;n</b> where <b>n</b> is the minimum or maximum of users on the channel.</center>"));

	m_pInfoLabel = new KviThemedLabel(m_pTopSplitter,"info_label");

	m_pTreeWidget  = new KviTalTreeWidget(m_pVertSplitter);
	m_pTreeWidget->setItemDelegate(new KviChannelTreeWidgetItemDelegate(m_pTreeWidget));
	QStringList columnLabels;
	columnLabels.append(__tr2qs("Channel"));
	columnLabels.append(__tr2qs("Users"));
	columnLabels.append(__tr2qs("Topic"));
	m_pTreeWidget->setColumnCount(3);
	m_pTreeWidget->setHeaderLabels(columnLabels);
	m_pTreeWidget->setAllColumnsShowFocus(TRUE);
	//m_pTreeWidget->setColumnWidthMode(2,KviTalTreeWidget::Maximum);
	//m_pTreeWidget->setColumnWidthMode(3,KviTalTreeWidget::Maximum);
	//m_pTreeWidget->setSorting(100);
	m_pTreeWidget->sortItems(0,Qt::AscendingOrder);
	connect(m_pTreeWidget,SIGNAL(itemDoubleClicked(KviTalTreeWidgetItem *)),this,SLOT(itemDoubleClicked(KviTalTreeWidgetItem *)));

	m_pIrcView = new KviIrcView(m_pVertSplitter,lpFrm,this);

	m_pConsole->ircContext()->setListWindowPointer(this);

	connect(m_pConsole->context(),SIGNAL(stateChanged()),
		this,SLOT(connectionStateChange()));

	connectionStateChange();

}

KviListWindow::~KviListWindow()
{
	g_pListWindowList->removeRef(this);
	m_pConsole->ircContext()->setListWindowPointer(0);
	if(m_pFlushTimer)delete m_pFlushTimer;
	m_pItemList->setAutoDelete(true);
	delete m_pItemList;
}

void KviListWindow::getBaseLogFileName(KviStr &buffer)
{
	buffer.sprintf("LIST_%d",console()->ircContextId());
}

void KviListWindow::requestList()
{
	if(m_pConsole->isConnected())
	{
		KviStr parms = m_pParamsEdit->text();
		if(parms.isEmpty())m_pConsole->connection()->sendFmtData("list");
		else m_pConsole->connection()->sendFmtData("list %s",m_pConsole->connection()->encodeText(parms.ptr()).data());
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
		QString tmp;
		KviQString::sprintf(tmp,
				__tr2qs("Connected to %s (%s)"),
				m_pConsole->connection()->currentServerName().toUtf8().data(),
				m_pConsole->currentNetworkName().toUtf8().data());
		m_pInfoLabel->setText(tmp);
	} else {
		m_pInfoLabel->setText(__tr2qs("List cannot be requested: Not connected to a server"));
	}
}

QPixmap * KviListWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KVI_SMALLICON_LIST);
}

void KviListWindow::resizeEvent(QResizeEvent *e)
{
	int hght = m_pTopSplitter->sizeHint().height();
	m_pTopSplitter->setGeometry(0,0,width(),hght);
	m_pSplitter->setGeometry(0,hght,width(),height() - hght);
}

QSize KviListWindow::sizeHint() const
{
	return m_pSplitter->sizeHint();
}

void KviListWindow::fillCaptionBuffers()
{
	KviQString::sprintf(m_szPlainTextCaption,__tr2qs("Channel List [IRC Context %u]"),m_pConsole->ircContextId());

	KviQString::sprintf(m_szHtmlActiveCaption,
		__tr2qs("<nobr><font color=\"%s\"><b>Channel List</b></font> " \
			"<font color=\"%s\">[IRC Context %u]</font></nobr>"),
		KVI_OPTION_COLOR(KviOption_colorCaptionTextActive).name().toAscii().data(),
		KVI_OPTION_COLOR(KviOption_colorCaptionTextActive2).name().toAscii().data(),
		m_pConsole->ircContextId());

	KviQString::sprintf(m_szHtmlInactiveCaption,
		__tr2qs("<nobr><font color=\"%s\"><b>Channel list</b></font> " \
			"<font color=\"%s\">[IRC Context %u]</font></nobr>"),
		KVI_OPTION_COLOR(KviOption_colorCaptionTextInactive).name().toAscii().data(),
		KVI_OPTION_COLOR(KviOption_colorCaptionTextInactive2).name().toAscii().data(),
		m_pConsole->ircContextId());
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
		QString szDate = QDateTime::currentDateTime().toString("d MMM yyyy hh-mm");
		KviQString::sprintf(szFile,__tr2qs("Channel list for %Q - %Q"),
			&(connection()->networkName()),&(szDate));
	} else {
		szFile = __tr2qs("Channel list");
	}
	if(KviFileDialog::askForSaveFileName(szFile,__tr2qs("Choose filename"),szFile,
		__tr2qs("Configuration files (*.kvc)"),false,
		false,true,this))
	{
		if(QFileInfo(szFile).completeSuffix()!="kvc")
			szFile.append(".kvc");
		KviConfig cfg(szFile,KviConfig::Write);
		cfg.clear();
		//KviTalTreeWidgetItemIterator it(m_pTreeWidget);

		//while(it.current())
		KviChannelTreeWidgetItem *it;
		for (int i=0;i<m_pTreeWidget->topLevelItemCount();i++)
		{
			it=(KviChannelTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
			KviChannelTreeWidgetItemData* pData= it->m_pData;
			cfg.setGroup(pData->m_szChan);
			// Write properties
			cfg.writeEntry("topic",pData->m_szTopic);
			cfg.writeEntry("users",pData->m_szUsers);
//			cfg.writeEntry("usersKey",pData->m_szUsersKey);
		//	++it;
		}
	}
}

void KviListWindow::importList()
{

	QString szFile;
	if(KviFileDialog::askForOpenFileName(szFile,__tr2qs("Choose filename"),QString::null,KVI_FILTER_CONFIG,false,
		false,this))
	{
		if(m_pConsole->isConnected())
		{
			m_pConsole->connection()->sendFmtData("list stoplistdownloadnow");
			outputNoFmt(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Stopping the list download...")); //G&N mar 2005
		}

		m_pItemList->clear();

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

void KviListWindow::control(int message)
{
	switch(message)
	{
		case EXTERNAL_SERVER_DATA_PARSER_CONTROL_RESET:       reset();       break;
		case EXTERNAL_SERVER_DATA_PARSER_CONTROL_STARTOFDATA: startOfList(); break;
		case EXTERNAL_SERVER_DATA_PARSER_CONTROL_ENDOFDATA:   endOfList();   break;
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

void KviListWindow::processData(KviIrcMessage *msg)
{
	if(!m_pFlushTimer)
	{
		m_pFlushTimer = new QTimer(this);
		connect(m_pFlushTimer,SIGNAL(timeout()),this,SLOT(flush()));
		m_pFlushTimer->start(1000);
		m_pRequestButton->setEnabled(false);
	}

	QString sz2 = msg->connection()->decodeText(msg->safeParam(2));

	m_pItemList->append(
		new KviChannelTreeWidgetItemData(
			msg->connection()->decodeText(msg->safeParam(1)),
			msg->connection()->decodeText(msg->safeParam(2)),
			msg->connection()->decodeText(msg->safeTrailing()))
	);

	if(_OUTPUT_VERBOSE)
	{
		QString zzz = msg->connection()->decodeText(msg->allParams());
		output(KVI_OUT_LIST,__tr2qs("Processing list: %Q"),&zzz);
	}
}

void KviListWindow::flush()
{
	m_pTreeWidget->setUpdatesEnabled(true); /* for v_scroolbar */
	while(KviChannelTreeWidgetItemData * d = m_pItemList->first())
	{
		(void)new KviChannelTreeWidgetItem(m_pTreeWidget,d);
		m_pItemList->removeFirst();
	}
	m_pTreeWidget->setUpdatesEnabled(true);
	m_pTreeWidget->viewport()->update();
}

void KviListWindow::itemDoubleClicked(KviTalTreeWidgetItem *it)
{
	QString sz = ((KviChannelTreeWidgetItem *)it)->channel();
	if(sz.isEmpty())return;
	if(!connection())return;
	KviQCString dat = connection()->encodeText(sz);
	if(!dat.data())return;
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
