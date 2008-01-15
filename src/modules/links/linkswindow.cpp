//
//   File : linkswindow.cpp
//   Creation date : Thu Dec 21 2001 12:41:18 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2001 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
#include "linkswindow.h"

#include "kvi_debug.h"
#include "kvi_iconmanager.h"
#include "kvi_ircview.h"
#include "kvi_out.h"
#include "kvi_options.h"
#include "kvi_locale.h"
#include "kvi_out.h"
#include "kvi_mirccntrl.h"
#include "kvi_themedlabel.h"
#include "kvi_irccontext.h"
#include "kvi_ircconnection.h"

#include <qsplitter.h>
#include <qtooltip.h>
#include "kvi_tal_hbox.h"
#include <qlabel.h>

extern KviPointerList<KviLinksWindow> * g_pLinksWindowList;

KviLinksWindow::KviLinksWindow(KviFrame * lpFrm,KviConsole * lpConsole)
: KviWindow(KVI_WINDOW_TYPE_LINKS,lpFrm,"links",lpConsole) , KviExternalServerDataParser()
{
	g_pLinksWindowList->append(this);

	m_pTopSplitter = new QSplitter(Qt::Horizontal,this,"top_splitter");

	// The button box on the left
	KviTalHBox * box = new KviTalHBox(m_pTopSplitter);

	m_pRequestButton = new QToolButton(box,"request_button");
	m_pRequestButton->setUsesBigPixmap(false);
	m_pRequestButton->setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_LINKS)));
	connect(m_pRequestButton,SIGNAL(clicked()),this,SLOT(requestLinks()));
	QToolTip::add(m_pRequestButton,__tr2qs("Request Links"));

	QLabel * l = new QLabel(box,"");
	box->setStretchFactor(l,1);
	m_pInfoLabel = new KviThemedLabel(m_pTopSplitter,"info_label");

	connect(lpConsole->context(),SIGNAL(stateChanged()),
		this,SLOT(connectionStateChange()));

	m_pSplitter = new QSplitter(Qt::Horizontal,this,"splitter");
	m_pVertSplitter = new QSplitter(Qt::Vertical,m_pSplitter,"vsplitter");

	m_pListView  = new KviTalListView(m_pVertSplitter);
	m_pListView->addColumn(__tr2qs("Link"));
	m_pListView->addColumn(__tr2qs("Hops"));
	m_pListView->addColumn(__tr2qs("Description"));
	m_pListView->setRootIsDecorated(true);
	m_pListView->setAllColumnsShowFocus(true);
	connect(m_pListView,SIGNAL(rightButtonPressed(KviTalListViewItem *,const QPoint &,int)),
			this,SLOT(showHostPopup(KviTalListViewItem *,const QPoint &,int)));

	m_pIrcView = new KviIrcView(m_pVertSplitter,lpFrm,this);

	m_pLinkList = new KviPointerList<KviLink>;
	m_pLinkList->setAutoDelete(true);

	m_pHostPopup = new KviTalPopupMenu();
	connect(m_pHostPopup,SIGNAL(activated(int)),this,SLOT(hostPopupClicked(int)));

	connectionStateChange();

	m_pConsole->ircContext()->setLinksWindowPointer(this);

	m_szRootServer = __tr2qs("(None)");
}

KviLinksWindow::~KviLinksWindow()
{
	g_pLinksWindowList->removeRef(this);
	m_pConsole->ircContext()->setLinksWindowPointer(0);
	delete m_pLinkList;
	delete m_pHostPopup;
}

void KviLinksWindow::getBaseLogFileName(KviStr &buffer)
{
	buffer.sprintf("LINKS_%d",console()->ircContextId());
}


void KviLinksWindow::requestLinks()
{
	if(m_pConsole->isConnected())
	{
		m_pConsole->connection()->sendFmtData("links");
		outputNoFmt(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Sent links request, waiting for reply..."));
		m_pRequestButton->setEnabled(false);
	} else {
		outputNoFmt(KVI_OUT_SYSTEMERROR,__tr2qs("Cannot request links: No active connection"));
	}
}

void KviLinksWindow::connectionStateChange()
{
	KviIrcContext::State st = m_pConsole->context()->state();
	m_pRequestButton->setEnabled(st == KviIrcContext::Connected);
	if(st == KviIrcContext::Connected)
	{
		KviStr tmp(KviStr::Format,__tr2qs("Connected to %s (%s)"),m_pConsole->connection()->currentServerName().utf8().data(),m_pConsole->currentNetworkName().utf8().data());
		m_pInfoLabel->setText(tmp.ptr());
	} else {
		m_pInfoLabel->setText(__tr2qs("Links cannot be requested: Not connected to a server"));
	}
}

QPixmap * KviLinksWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KVI_SMALLICON_LINKS);
}

void KviLinksWindow::resizeEvent(QResizeEvent *e)
{
	int hght2 = m_pTopSplitter->sizeHint().height();
	m_pTopSplitter->setGeometry(0,0,width(),hght2);
	m_pSplitter->setGeometry(0,hght2,width(),height() - hght2);
}

QSize KviLinksWindow::sizeHint() const
{
	QSize ret(m_pSplitter->sizeHint().width(),
		m_pSplitter->sizeHint().height() + m_pTopSplitter->sizeHint().height());
	return ret;
}

void KviLinksWindow::fillCaptionBuffers()
{
	KviQString::sprintf(m_szPlainTextCaption,__tr2qs("Links for %Q [IRC Context %u]"),&m_szRootServer,m_pConsole->ircContextId());

	KviQString::sprintf(m_szHtmlActiveCaption,
		__tr2qs("<nobr><font color=\"%s\"><b>Links for %Q</b></font> " \
			"<font color=\"%s\">[IRC Context %u]</font></nobr>"),
		KVI_OPTION_COLOR(KviOption_colorCaptionTextActive).name().ascii(),
		&m_szRootServer,
		KVI_OPTION_COLOR(KviOption_colorCaptionTextActive2).name().ascii(),
		m_pConsole->ircContextId());

	KviQString::sprintf(m_szHtmlInactiveCaption,
		__tr2qs("<nobr><font color=\"%s\"><b>Links for %Q</b></font> " \
			"<font color=\"%s\">[IRC Context %u]</font></nobr>"),
		KVI_OPTION_COLOR(KviOption_colorCaptionTextInactive).name().ascii(),
		&m_szRootServer,
		KVI_OPTION_COLOR(KviOption_colorCaptionTextInactive2).name().ascii(),
		m_pConsole->ircContextId());
}

void KviLinksWindow::die()
{
	close();
}

void KviLinksWindow::control(int message)
{
	switch(message)
	{
		case EXTERNAL_SERVER_DATA_PARSER_CONTROL_RESET:     reset(); break;
		case EXTERNAL_SERVER_DATA_PARSER_CONTROL_ENDOFDATA: endOfLinks(); break;
	}
}

void KviLinksWindow::endOfLinks()
{
	m_pRequestButton->setEnabled(true);

	m_pListView->clear();
	m_pListView->setSorting(-1);

	outputNoFmt(KVI_OUT_SYSTEMMESSAGE,"======================");
	outputNoFmt(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Received end of links."));
	outputNoFmt(KVI_OUT_SYSTEMMESSAGE,"======================");

	KviTalListViewItem * it   = 0;
	KviTalListViewItem * root = 0;

	int totalHosts  = 0;
	int totalHops   = 0;
	int maxHops     = 0;
	int avgHops     = 0;
	int directLinks = 0;
	int nearLinks   = 0;
	int brokenLinks = 0;
	int maxLinks    = 0;
	int farLinks    = 0;
	int wildServers = 0;

	KviStr szMaxHop,szMaxLinks;

	m_pListView->setUpdatesEnabled(false);
	for(KviLink *l=m_pLinkList->first();l;l=m_pLinkList->next()){
		totalHosts++;
		if(l->hops == 0)root = new KviTalListViewItem(m_pListView,QString(l->host.ptr()),"0",QString(l->description.ptr()));
		else {
			totalHops += l->hops;
			if(l->hops < 4){
				nearLinks++;
				if(l->hops == 1)directLinks++;
			} else {
				if(l->hops >= 7)farLinks++;
			}
			if(l->hops == maxHops){
				szMaxHop.append(',');
				szMaxHop.append(l->host);
			} else if(l->hops > maxHops){
				maxHops = l->hops;
				szMaxHop = l->host;
			}
			if(l->host.contains('*'))wildServers++;
			it = insertLink(l);
			if(!it){
				output(KVI_OUT_SYSTEMERROR,__tr2qs("Broken link: Missing parent (%s) for %s (%d hops): %s (used /LINKS <mask> ?)"),
					l->parent.ptr(),l->host.ptr(),l->hops,l->description.ptr());
				brokenLinks++;
				KviStr tmp(KviStr::Format,__tr2qs("%s: Parent link %s"),l->description.ptr(),l->parent.ptr());
				KviStr tmp2(KviStr::Format,"%d",l->hops);
				if(root)it = new KviTalListViewItem(m_pListView,root,QString(l->host.ptr()),QString(tmp2.ptr()),QString(tmp.ptr()));
				else {
					outputNoFmt(KVI_OUT_SYSTEMERROR,__tr2qs("Warning: No root link was sent by the server, the stats may be invalid."));
					it = new KviTalListViewItem(m_pListView,QString(l->host.ptr()),QString(tmp2.ptr()),QString(tmp.ptr()));
				}
			} else {
				it = it->parent();
				if(it)
				{
					int links = it->childCount() + 1;
					if(links == maxLinks)
					{
						szMaxLinks.append(',');
						szMaxLinks.append(it->text(0));
					} else if(links > maxLinks)
					{
						maxLinks = links;
						szMaxLinks = it->text(0);
					}
				}
			}
		}
	}
	
	avgHops = ((totalHosts > 0) ? ((totalHops * 100) / totalHosts) : 0 );
	int nearProcent = ((totalHosts > 0) ? ((nearLinks * 10000) / totalHosts) : 0);
	int directProcent = ((totalHosts > 0) ? ((directLinks * 10000) / totalHosts) : 0);
	int midLinks = totalHosts - (farLinks + nearLinks + 1 + brokenLinks);
	if(midLinks < 0)midLinks = 0;
	int midProcent = ((totalHosts > 0) ? ((midLinks * 10000) / totalHosts) : 0);
	int farProcent = ((totalHosts > 0) ? ((farLinks * 10000) / totalHosts) : 0);

	outputNoFmt(KVI_OUT_LINKS,"======================");

//	if(!root)root = m_pListView->firstChild();
	if(root)
	{
		m_szRootServer = root->text(0);
		output(KVI_OUT_LINKS,__tr2qs("%c%cLinks for %Q"),KVI_TEXT_BOLD,KVI_TEXT_UNDERLINE,&m_szRootServer);
		outputNoFmt(KVI_OUT_LINKS,"======================");
		QString tmpo = wildServers ? __tr2qs("Total hosts listed") : __tr2qs("Total hosts in the network");
		output(KVI_OUT_LINKS,"%Q: %d",&tmpo,totalHosts);
		if(wildServers)output(KVI_OUT_LINKS,__tr2qs("Wildcard servers (hubs?): %d"),wildServers);
		output(KVI_OUT_LINKS,__tr2qs("Direct links: %d (~%d.%d %)"),directLinks,directProcent / 100, directProcent % 100);
		output(KVI_OUT_LINKS,__tr2qs("Close links (1 <= hops <= 3): %d (~%d.%d %)"),nearLinks,nearProcent / 100,nearProcent % 100);
		output(KVI_OUT_LINKS,__tr2qs("Mid-range links (4 <= hops  <= 6): %d (~%d.%d %)"),midLinks,midProcent / 100,midProcent % 100);
		output(KVI_OUT_LINKS,__tr2qs("Distant links (7 <= hops): %d (~%d.%d %)"),farLinks,farProcent / 100,farProcent % 100);
		output(KVI_OUT_LINKS,__tr2qs("Broken (unknown) links: %d"),brokenLinks);
		output(KVI_OUT_LINKS,__tr2qs("Maximum links per host: %d [%s]"),maxLinks,szMaxLinks.ptr());
		output(KVI_OUT_LINKS,__tr2qs("Total links: %d"),totalHops);
		output(KVI_OUT_LINKS,__tr2qs("Maximum hops: %d [%s]"),maxHops,szMaxHop.ptr());
		output(KVI_OUT_LINKS,__tr2qs("Average hops: ~%d.%d"),avgHops / 100,avgHops % 100);
	} else {
		m_szRootServer = __tr2qs("(Unknown)");
		outputNoFmt(KVI_OUT_LINKS,__tr2qs("Incomplete LINKS result, no stats available"));
	}
	outputNoFmt(KVI_OUT_LINKS,"======================");

	updateCaption();


	while(!m_pLinkList->isEmpty())m_pLinkList->removeFirst();

	m_pListView->setUpdatesEnabled(true);
	m_pListView->repaint();
}

KviTalListViewItem * KviLinksWindow::insertLink(KviLink *l)
{
	__range_valid(l->hops > 0);
	KviTalListViewItem * i = getItemByHost(l->parent.ptr(),0);
	KviTalListViewItem * it = 0;
	if(!i)return 0;
	else {
		KviStr hops(KviStr::Format,"%d",l->hops);
		it = new KviTalListViewItem(i,QString(l->host.ptr()),QString(hops.ptr()),QString(l->description.ptr()));
		i->setOpen(true);
	}
	return it;
}

KviTalListViewItem * KviLinksWindow::getItemByHost(const char *host,KviTalListViewItem * par)
{
	KviTalListViewItem * i = (par ? par->firstChild() : m_pListView->firstChild());
	if(!i)return 0;
	while(i){
		KviStr tmp = i->text(0);
		if(kvi_strEqualCI(tmp.ptr(),host))return i;
		KviTalListViewItem * ch = getItemByHost(host,i);
		if(ch)return ch;
		i = i->nextSibling();
	}
	return 0;
}

void KviLinksWindow::showHostPopup(KviTalListViewItem *i,const QPoint &p,int)
{
	if(!i)return;
	KviStr host=i->text(0);
	if(host.isEmpty())return;
	m_pHostPopup->clear();
	KviStr tmp(KviStr::Format,"LINKS %s *",host.ptr());
	m_pHostPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_LINKS)),tmp.ptr());
	m_pHostPopup->insertSeparator();
	tmp.sprintf("TIME %s",host.ptr());
	m_pHostPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TIME)),tmp.ptr());
	tmp.sprintf("ADMIN %s",host.ptr());
	m_pHostPopup->insertItem(tmp.ptr());
	tmp.sprintf("INFO %s",host.ptr());
	m_pHostPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SERVERINFO)),tmp.ptr());
	tmp.sprintf("MOTD %s",host.ptr());
	m_pHostPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MOTD)),tmp.ptr());
	tmp.sprintf("VERSION %s",host.ptr());
	m_pHostPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_KVIRC)),tmp.ptr());
	tmp.sprintf("TRACE %s",host.ptr());
	m_pHostPopup->insertItem(tmp.ptr());
	tmp.sprintf("USERS %s",host.ptr());
	m_pHostPopup->insertItem(tmp.ptr());
	m_pHostPopup->insertSeparator();
	tmp.sprintf("STATS c %s",host.ptr());
	m_pHostPopup->insertItem(tmp.ptr());
	tmp.sprintf("STATS d %s",host.ptr());
	m_pHostPopup->insertItem(tmp.ptr());
	tmp.sprintf("STATS h %s",host.ptr());
	m_pHostPopup->insertItem(tmp.ptr());
	tmp.sprintf("STATS i %s",host.ptr());
	m_pHostPopup->insertItem(tmp.ptr());
	tmp.sprintf("STATS k %s",host.ptr());
	m_pHostPopup->insertItem(tmp.ptr());
	tmp.sprintf("STATS l %s",host.ptr());
	m_pHostPopup->insertItem(tmp.ptr());
	tmp.sprintf("STATS m %s",host.ptr());
	m_pHostPopup->insertItem(tmp.ptr());
	tmp.sprintf("STATS o %s",host.ptr());
	m_pHostPopup->insertItem(tmp.ptr());
	tmp.sprintf("STATS t %s",host.ptr());
	m_pHostPopup->insertItem(tmp.ptr());
	tmp.sprintf("STATS u %s",host.ptr());
	m_pHostPopup->insertItem(tmp.ptr());
	tmp.sprintf("STATS y %s",host.ptr());
	m_pHostPopup->insertItem(tmp.ptr());
	tmp.sprintf("STATS z %s",host.ptr());
	m_pHostPopup->insertItem(tmp.ptr());
	m_pHostPopup->popup(p);
}

void KviLinksWindow::hostPopupClicked(int id)
{
	KviStr tmp = m_pHostPopup->text(id);
	if(tmp.hasData())
	{
		if(!connection())output(KVI_OUT_SYSTEMERROR,__tr2qs("You're not connected to a server"));
		m_pConsole->connection()->sendData(tmp.ptr());
	}
}

void KviLinksWindow::reset()
{
	outputNoFmt(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Reset"));
	while(!m_pLinkList->isEmpty())m_pLinkList->removeFirst();
}

void KviLinksWindow::processData(KviIrcMessage *msg)
{
	output(KVI_OUT_SYSTEMMESSAGE,__tr2qs("Processing link: %s"),msg->allParams());
	KviLink * l = new KviLink;

	l->host = msg->safeParam(1);
	l->parent = msg->safeParam(2);

	const char * tr = msg->safeTrailing();

	if(isdigit(*tr))
	{
		const char *aux = tr;
		while(*tr && (isdigit(*tr)))tr++;
		KviStr tmp(aux,tr - aux);
		l->hops = tmp.toInt();
	} else {
		outputNoFmt(KVI_OUT_SYSTEMERROR,__tr2qs("Broken message syntax, can't extract hops number, assuming 0"));
		l->hops = 0;
	}
	while(*tr && (*tr == ' '))tr++;
	l->description = tr;
	uint idx=0;
	for(KviLink *m=m_pLinkList->first();m;m=m_pLinkList->next())
	{
		if(m->hops >= l->hops)
		{
			m_pLinkList->insert(idx,l);
			return;
		}
		idx++;
	}
	m_pLinkList->append(l);
}

//#warning "Load & save properties of this kind of window"

//void KviLinksWindow::saveProperties()
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
//void KviLinksWindow::setProperties(KviWindowProperty *p)
//{
//	QValueList<int> l;
//	l.append(p->splitWidth1);
//	l.append(p->splitWidth2);
//	m_pVertSplitter->setSizes(l);
//	m_pIrcView->setTimestamp(p->timestamp);
//	m_pIrcView->setShowImages(p->imagesVisible);
//}

void KviLinksWindow::applyOptions()
{
	m_pIrcView->applyOptions();
	KviWindow::applyOptions();
}

#include "m_linkswindow.moc"
