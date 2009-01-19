//=============================================================================
//
//   File : kvi_console.cpp
//   Creation date : Sun Jun 25 2000 15:01:34 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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
//=============================================================================



#include "kvi_ircurl.h"
#include "kvi_app.h"
#include "kvi_console.h"
#include "kvi_frame.h"
#include "kvi_iconmanager.h"
#include "kvi_options.h"
#include "kvi_locale.h"
#include "kvi_ircview.h"
#include "kvi_mirccntrl.h"
#include "kvi_input.h"
#include "kvi_error.h"
#include "kvi_proxydb.h"
#include "kvi_netutils.h"
#include "kvi_ircserver.h"
#include "kvi_ircserverdb.h"
#include "kvi_dns.h"
#include "kvi_defaults.h"
#include "kvi_ircuserdb.h"
#include "kvi_channel.h"
#include "kvi_query.h"
#include "kvi_parameterlist.h"
#include "kvi_regusersdb.h"
#include "kvi_userlistview.h"
#include "kvi_out.h"
#include "kvi_config.h"
#include "kvi_irctoolbar.h"
#include "kvi_internalcmd.h"
#include "kvi_sparser.h"
#include "kvi_themedlabel.h"
//#include "kvi_garbage.h"
#include "kvi_modulemanager.h"
#include "kvi_fileutils.h"
#include "kvi_time.h"
#include "kvi_mexlinkfilter.h"
#include "kvi_avatarcache.h"
#include "kvi_ircconnection.h"
#include "kvi_ircconnectionuserinfo.h"
#include "kvi_ircconnectionserverinfo.h"
#include "kvi_ircconnectionstatedata.h"
#include "kvi_ircconnectiontarget.h"
#include "kvi_ircconnectionstatistics.h"
#include "kvi_asynchronousconnectiondata.h"
#include "kvi_ircdatastreammonitor.h"
#include "kvi_toolwindows_container.h"
#include "kvi_msgbox.h"
#include "kvi_kvs_script.h"
#include "kvi_kvs_eventtriggers.h"
#include "kvi_tal_hbox.h"
#include "kvi_tal_popupmenu.h"

#ifdef COMPILE_SSL_SUPPORT
	#include "kvi_sslmaster.h"
#endif

#include <QSplitter>
#include <QToolBar>
#include <QTimer>
#include <QMessageBox>
#include <QStringList>
#include <QCloseEvent>
#include <QTextDocument>
#include <QRegExp>
#include <QDebug>

#define __KVI_DEBUG__
#include "kvi_debug.h"

extern KVIRC_API KviServerDataBase           * g_pServerDataBase;
extern KVIRC_API KviProxyDataBase               * g_pProxyDataBase;
//extern KVIRC_API KviGarbageCollector            * g_pGarbageCollector;

KviConsole::KviConsole(KviFrame * lpFrm,int iFlags)
#ifdef COMPILE_ON_WINDOWS
: KviWindow(KVI_WINDOW_TYPE_CONSOLE,lpFrm,"CONSOLE",0)
#else
: KviWindow(KVI_WINDOW_TYPE_CONSOLE,lpFrm,"CONSOLE",this)
#endif
{
	m_pConsole = this;
	m_pContext = new KviIrcContext(this);

	m_iFlags = iFlags;
	if(m_pContext->id() == 1)
	{
		m_iFlags |= KVI_CONSOLE_FLAG_FIRSTINAPP;
	}

	m_pButtonBox = new KviTalHBox(this);
	m_pButtonBox->setSpacing(0);
	m_pButtonBox->setMargin(0);
	new QLabel(__tr2qs("Address:"),m_pButtonBox);
	m_pAddressEdit = new QComboBox(m_pButtonBox);
	m_pAddressEdit->setObjectName("url_editor");
	m_pAddressEdit->setAutoCompletion(true);
	m_pAddressEdit->setDuplicatesEnabled(false);
	m_pAddressEdit->setEditable(true);
	m_pAddressEdit->addItem(QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_URL))),"");
	recentUrlsChanged();
	//m_pAddressEdit->setCurrentText("");
	m_pAddressEdit->setInsertPolicy(QComboBox::NoInsert);
	m_pAddressEdit->setMinimumHeight(24); //icon is 16px, + margins
	m_pButtonBox->setStretchFactor(m_pAddressEdit,1);
	m_pButtonBox->setObjectName( QLatin1String( "kvi_window_button_box" ) );

	KviTalToolTip::add(m_pAddressEdit,__tr2qs("Current IRC URI"));
	connect(m_pAddressEdit,SIGNAL(activated(const QString & )),this,SLOT(ircUriChanged(const QString & )));
	connect(g_pApp,SIGNAL(recentUrlsChanged()),this,SLOT(recentUrlsChanged()));

	m_pSplitter = new QSplitter(Qt::Horizontal,this);
	m_pSplitter->setObjectName("console_splitter");
	m_pIrcView = new KviIrcView(m_pSplitter,lpFrm,this);
	connect(m_pIrcView,SIGNAL(rightClicked()),this,SLOT(textViewRightClicked()));

	// FIXME: #warning "If notify list is disabled avoid to show this"
	// FIXME: #warning "Button to show/hide the notifyListView (NOT DELETE RE_CREATE!)"
	// The userlist on the right
	//m_pEditorsContainer= new KviToolWindowsContainer(m_pSplitter);
	m_pNotifyViewButton = new KviWindowToolPageButton(KVI_SMALLICON_HIDELISTVIEW,KVI_SMALLICON_SHOWLISTVIEW,__tr2qs("Notify List"),buttonContainer(),true,"list_view_button");
	connect(m_pNotifyViewButton,SIGNAL(clicked()),this,SLOT(toggleNotifyView()));

	m_pNotifyListView  = new KviUserListView(m_pSplitter,m_pNotifyViewButton,0,this,19,__tr2qs("Notify List"),"notify_list_view");

	m_pInput   = new KviInput(this,m_pNotifyListView);

	if(KVI_OPTION_BOOL(KviOption_boolAutoLogConsole))m_pIrcView->startLogging();
}

void KviConsole::recentUrlsChanged(){
	QString cur = m_pAddressEdit->currentText();
	m_pAddressEdit->clear();
	for (
		QStringList::Iterator it = KVI_OPTION_STRINGLIST(KviOption_stringlistRecentIrcUrls).begin();
		it != KVI_OPTION_STRINGLIST(KviOption_stringlistRecentIrcUrls).end();
		++it
			) {
		m_pAddressEdit->addItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_URL)),*it);
	}
//	m_pAddressEdit->setCurrentText(cur);
	int i = m_pAddressEdit->findText(cur);
    if (i != -1)m_pAddressEdit->setCurrentIndex(i);
	else
	{
		if (m_pAddressEdit->isEditable())m_pAddressEdit->setEditText(cur);
		else
	    m_pAddressEdit->setItemText(m_pAddressEdit->currentIndex(), cur);
	}
}

bool KviConsole::connectionInProgress()
{
	if(context()->asynchronousConnectionData() != 0)return true;
	if(context()->state() != KviIrcContext::Idle)return true;
	return false;
}

KviConsole::~KviConsole()
{
	// FIXME: #warning "WARNING : THIS SHOULD BECOME A COMMAND /QUIT $option() so the idents are parsed!"

	// Force connection close: it will just return if no connection is present
	context()->terminateConnectionRequest(true);

	KVS_TRIGGER_EVENT_0(KviEvent_OnIrcContextDestroyed,this);

	//if(m_pLastIrcServer)delete m_pLastIrcServer;

	delete m_pContext;
	m_pContext = 0;
}

void KviConsole::triggerCreationEvents()
{
	if(m_iFlags & KVI_CONSOLE_FLAG_FIRSTINAPP) // this is the first context in the application
	{
		KVS_TRIGGER_EVENT_0(KviEvent_OnKVIrcStartup,this);

		if(KVI_OPTION_BOOL(KviOption_boolShowTipAtStartup))
			g_pFrame->executeInternalCommand(KVI_INTERNALCOMMAND_TIP_OPEN);
	}

	if(m_iFlags & KVI_CONSOLE_FLAG_FIRSTINFRAME)
	{
		KVS_TRIGGER_EVENT_0(KviEvent_OnFrameWindowCreated,this);
	}

	KVS_TRIGGER_EVENT_0(KviEvent_OnIrcContextCreated,this);
}

void KviConsole::fillContextPopup(KviTalPopupMenu * p)
{
	int id;
	int cc = 0;
	int qc = 0;
	// FIXME: add items to close dead queries and channels ?
	if(connection())
	{
		cc = connection()->channelList()->count();
		qc = connection()->queryList()->count();
		p->insertSeparator();
		id = p->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CHANNEL)),__tr2qs("Part All Channels"),connection(),SLOT(partAllChannels()));
		if(!cc)p->setItemEnabled(id,false);
		id = p->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_QUERY)),__tr2qs("Close All Queries"),connection(),SLOT(closeAllQueries()));
		if(!qc)p->setItemEnabled(id,false);
	}

	p->insertSeparator();
	p->insertItem(__tr2qs("Unhighlight All Windows"),context(),SLOT(unhighlightAllWindows()));
	if(connection())
	{
		id = p->insertItem(__tr2qs("Unhighlight All Channels"),connection(),SLOT(unhighlightAllChannels()));
		if(!cc)p->setItemEnabled(id,false);
		id = p->insertItem(__tr2qs("Unhighlight All Queries"),connection(),SLOT(unhighlightAllQueries()));
		if(!qc)p->setItemEnabled(id,false);
	}
}

void KviConsole::completeChannel(const QString &word,KviPointerList<QString> * matches)
{
	// FIXME: first look in our context ?
	/*
	if(!connection())return;
	for(KviChannel * c = connection()->channelList()->first();c;c = connection()->channelList()->next())
	{
		if(kvi_strEqualCIN(c->windowName(),word.ptr(),word.len()))matches->append(new KviStr((*it)
	}
	*/
	QStringList *pList = g_pApp->getRecentChannels(currentNetworkName());
	if(pList)
	{
		for(QStringList::Iterator it = pList->begin(); it != pList->end(); ++it)
		{
			if(KviQString::equalCIN((*it),word,word.length()))matches->append(new QString(*it));
		}
	}
}

void KviConsole::completeServer(const QString &word, KviPointerList<QString> * matches)
{
	for(QStringList::Iterator it = KVI_OPTION_STRINGLIST(KviOption_stringlistRecentServers).begin(); it != KVI_OPTION_STRINGLIST(KviOption_stringlistRecentServers).end(); ++it)
	{
		QString srv((*it));

		KviQString::cutToFirst(srv,'/');
		while(srv.startsWith("/"))srv.remove(0,1);
		KviQString::cutFromLast(srv,':');
		//We should have a full server name here, without the irc:// and without the port
		if(KviQString::equalCIN(srv,word,word.length()))
		{
			matches->append(new QString(srv));
		}
	}
}

void KviConsole::getUserTipText(const QString &nick,KviIrcUserEntry *e,QString &buffer)
{
	KviRegisteredMask *u = g_pRegisteredUserDataBase->findMatchingMask(nick,e->user(),e->host());

	buffer = "<table width=\"100%\">" \
			"<tr><td bgcolor=\"#303030\">" \
			"<center><font color=\"#FFFFFF\"><b>";

	buffer += nick;
	buffer += "!";
	buffer += e->user().isEmpty() ? QString("*") : e->user();
	buffer += "@";
	buffer += e->host().isEmpty() ? QString("*") : e->host();

	buffer += "</b></font></center></td></tr>";
	if(u)
	{
		QString szComment=u->user()->getProperty("comment");
		if(!szComment.isEmpty())
		{
			buffer += "<tr bgcolor=\"#F0F0F0\"><td><center><font size=\"-1\">(";
			buffer += szComment;
			buffer += ")</font></center></td></tr>";
		}
	}

	if(e->avatar())
	{
		buffer += QString("<tr><td><center><img src=\"%1\" width=\"%2\"></center></td></tr>").arg(e->avatar()->localPath()).arg(e->avatar()->size().width());
	}

	if(e->hasRealName())
	{
		buffer += "<tr><td><center><b>";
		buffer += KviMircCntrl::stripControlBytes(e->realName());
		buffer += "</b></center></td></tr>";
	}

	if(e->gender()!=KviIrcUserEntry::Unknown)
	{
		buffer += "<tr><td>";
		buffer += __tr2qs("Gender:");
		buffer += " ";
		buffer += (e->gender()==KviIrcUserEntry::Male) ? __tr2qs("Male") : __tr2qs("Female");
		buffer += "</td></tr>";
	}

	if(u)
	{
		QString mask;
		u->mask()->mask(mask);
		buffer += "<tr bgcolor=\"#F0F0F0\"><td><font color=\"#000000\">";
		buffer += __tr2qs("Registered as");
		buffer += " <b>";
		buffer += u->user()->name();
		buffer += "</b>; Group ";
		buffer += u->user()->group();
		buffer += "</font></td></tr><tr bgcolor=\"#F0F0F0\"><td><font size=\"-1\" color=\"#000000\">";
		buffer += __tr2qs("(Matched by");
		buffer += " ";
		buffer += mask;
		buffer += ")</font></td></tr>";
	}

	if(connection())
	{
		QString chans;
		if(connection()->getCommonChannels(nick,chans,false))
		{
			buffer += "<tr><td bgcolor=\"#F0F0F0\"><font color=\"#000000\">";
			buffer += __tr2qs("On <b>");
			buffer += chans;
			buffer += "</b></font></td></tr>";
		}
	}

	if(e->hasServer())
	{
		buffer += "<tr><td bgcolor=\"#F0F0F0\"><nobr><font color=\"#000000\">";
		buffer += __tr2qs("Using server <b>%1</b>").arg(e->server());

		if(e->hasHops())
		{
			buffer += " (";
			buffer += __tr2qs("%1 hops").arg(e->hops());
			buffer += ")</font></nobr></td></tr>";
		} else {
			buffer += "</font></nobr></td></tr></table>";
		}
	}

	if(e->isAway())
	{
		buffer += "<tr><td bgcolor=\"#F0F0F0\"><font color=\"#000000\">";
		buffer += __tr2qs("Probably Away");
		buffer += "</font></td></tr>";
	}
}

void KviConsole::toggleNotifyView()
{
	showNotifyList(!m_pNotifyListView->isVisible());
}

void KviConsole::executeInternalCommand(int index)
{
	KviKvsScript::run(kvi_getInternalCommandBuffer(index),this);
}

void KviConsole::saveProperties(KviConfig *cfg)
{
	KviWindow::saveProperties(cfg);
	cfg->writeEntry("Splitter",m_pSplitter->sizes());
	cfg->writeEntry("NotifyListViewVisible",m_pNotifyListView->isVisible());

}

void KviConsole::getBaseLogFileName(QString &buffer)
{
	buffer=QString("CONSOLE%1").arg(context()->id());
}

void KviConsole::showNotifyList(bool bShow)
{
	if(!bShow)
	{
		m_pNotifyListView->hide();
		if(m_pNotifyViewButton->isChecked())m_pNotifyViewButton->setChecked(false);
	} else {
		m_pNotifyListView->show();
		if(!(m_pNotifyViewButton->isChecked()))m_pNotifyViewButton->setChecked(true);
	}
}

void KviConsole::loadProperties(KviConfig *cfg)
{
	int w = width();
	QList<int> def;
	def.append((w * 85) / 100);
	def.append((w * 15) / 100);
	QList<int> cur = cfg->readIntListEntry("Splitter",def);
	// check the size correctness
	if(cur.count() != 2)cur = def;
	else {
		int i1 = cur[0];
		int i2 = cur[1];
		if(i1 < i2)cur = def;
	}
	m_pSplitter->setSizes(cur);
	KviWindow::loadProperties(cfg);
	showNotifyList(cfg->readBoolEntry("NotifyListViewVisible",false));
}

void KviConsole::textViewRightClicked()
{
	KVS_TRIGGER_EVENT_0(KviEvent_OnConsolePopupRequest,this);
}


KviWindow * KviConsole::activeWindow()
{
	if(!g_pActiveWindow)return this;
	if(g_pActiveWindow->console() == this)return g_pActiveWindow;
	return this;
}

void KviConsole::ircUriChanged(const QString & text){
	if(KviIrcUrl::run(text,KviIrcUrl::CurrentContext,this) & KviIrcUrl::InvalidProtocol)
	{
		KviMessageBox::warning(__tr2qs("KVIrc can accept only irc://, irc6://, ircs:// or irc6s:// URL's\n"
				"Your URL is invalid. Check spelling and try again"));
	}
}

void KviConsole::updateUri()
{
	QString uri;
	if(connection())
	{
		KviServer* server = connection()->target()->server();
		if(server)
		{
			KviIrcUrl::join(uri,server);
			KviChannel * last =connection()->channelList()->last();
			for(KviChannel * c = connection()->channelList()->first();c;c = connection()->channelList()->next())
			{
				uri.append(c->target());
				if(c->hasChannelKey()) {
					uri.append("?");
					uri.append(c->channelKey());
				}
				if(c!=last) uri.append(",");
			}
		}
	}
//	m_pAddressEdit->setCurrentText(uri);
	int i = m_pAddressEdit->findText(uri);
    if (i != -1)m_pAddressEdit->setCurrentIndex(i);
	else
	{
		if (m_pAddressEdit->isEditable())m_pAddressEdit->setEditText(uri);
		else
	    m_pAddressEdit->setItemText(m_pAddressEdit->currentIndex(), uri);
	}
}

void KviConsole::connectionAttached()
{
	//need to update URI
	connect(m_pContext->connection(),SIGNAL(chanListChanged()),this,SLOT(updateUri()));
	updateUri();
	m_pNotifyListView->setUserDataBase(connection()->userDataBase());
}

void KviConsole::connectionDetached()
{
	//need to update URI?
	m_pNotifyListView->partAll();
	m_pNotifyListView->setUserDataBase(0); // this is rather for crash tests
}

void KviConsole::closeEvent(QCloseEvent *e)
{
	if(g_pFrame->consoleCount() > 1)
	{
		// there are other consoles beside this one
		if(context()->state() == KviIrcContext::Connected)
		{
			if(!KVI_OPTION_BOOL(KviOption_boolAlwaysDisconnectClosingConnectedConsole))
			{
				switch(QMessageBox::warning(this,
					__tr2qs("Confirmation - KVIrc"),
					__tr2qs("You have just attempted to close a console window with an active connection inside.\n" \
						"Are you sure you wish to terminate the connection?"),
					__tr2qs("&Always"),
					__tr2qs("&Yes"),
					__tr2qs("&No"),
					2,2))
				{
					case 0:
						KVI_OPTION_BOOL(KviOption_boolAlwaysDisconnectClosingConnectedConsole) = true;
					break;
					case 1:
						// nothing here
					break;
					default: // 2 = no
						e->ignore();
						return;
					break;
				}
			}
			// ask the context to terminate the connection gracefully
			context()->terminateConnectionRequest(false);
			// the close event will recall terminateConnectionRequest()
			// to brutally interrupt it in a while
		}

		// just close
		KviWindow::closeEvent(e);
		return;
	}

	// this is the only console... ask if the user really wants to quit KVirc
	if(!KVI_OPTION_BOOL(KviOption_boolAlwaysQuitKVIrcClosingLastConsole))
	{
		switch(QMessageBox::warning(this,
				__tr2qs("Confirmation - KVIrc"),
				__tr2qs("You have just attempted to close the last console window.\nAre you sure you wish to quit KVIrc?"),
				__tr2qs("&Always"),
				__tr2qs("&Yes"),
				__tr2qs("&No"),
				2,2))
		{
			case 0:
				KVI_OPTION_BOOL(KviOption_boolAlwaysQuitKVIrcClosingLastConsole) = true;
			break;
			case 1:
				// nothing here
			break;
			default: // 2 = no
				e->ignore();
				return;
			break;
		}
	}

	g_pApp->quit();
}

// internal helper for applyHighlighting
int KviConsole::triggerOnHighlight(KviWindow *wnd,int type,const QString &nick,const QString &user,const QString &host,const QString &szMsg,const QString &trigger)
{
	if(!KVI_OPTION_STRING(KviOption_stringOnHighlightedMessageSound).isEmpty() && wnd!=g_pActiveWindow) KviKvsScript::run("snd.play $0",0,new KviKvsVariantList(new KviKvsVariant(KVI_OPTION_STRING(KviOption_stringOnHighlightedMessageSound))));
	QString szMessageType;
	KviQString::sprintf(szMessageType,"%d",type);
	if(KVS_TRIGGER_EVENT_7_HALTED(KviEvent_OnHighlight,
					wnd,nick,user,host,
					szMsg,trigger,
					szMessageType,(type == KVI_OUT_ACTION)))
		return -1;
	return KVI_OUT_HIGHLIGHT;
}

// if it returns -1 you should just return and not display the message
int KviConsole::applyHighlighting(KviWindow *wnd,int type,const QString &nick,const QString &user,const QString &host,const QString &szMsg)
{
	QString szPattern=KVI_OPTION_STRING(KviOption_stringWordSplitters);
	QString szSource;
	QString szStripMsg=KviMircCntrl::stripControlBytes(szMsg);
	QRegExp rgxHlite;
	if(KVI_OPTION_BOOL(KviOption_boolAlwaysHighlightNick) && connection())
	{
		if(!szPattern.isEmpty())
			rgxHlite.setPattern(
				QString("(?:[%1]|\\s|^)%2(?:[%1]|\\s|$)").arg(
					QRegExp::escape(szPattern), QRegExp::escape(connection()->userInfo()->nickName())
				)
			);
		else
			rgxHlite.setPattern(
				QString("(?:\\s|^)%1(?:\\s|$)").arg(
					QRegExp::escape(connection()->userInfo()->nickName())
				)
			);
		rgxHlite.setCaseSensitivity(Qt::CaseInsensitive);
		if(szStripMsg.contains(rgxHlite))
			return triggerOnHighlight(wnd,type,nick,user,host,szMsg,connection()->userInfo()->nickName());
	}

	if(KVI_OPTION_BOOL(KviOption_boolUseWordHighlighting))
	{
		for(QStringList::Iterator it = KVI_OPTION_STRINGLIST(KviOption_stringlistHighlightWords).begin();
				it != KVI_OPTION_STRINGLIST(KviOption_stringlistHighlightWords).end() ; ++it)
		{
			if((*it).isEmpty())
				continue;
			if(!szPattern.isEmpty())
				rgxHlite.setPattern(
				QString("(?:[%1]|\\s|^)%2(?:[%1]|\\s|$)").arg(
					QRegExp::escape(szPattern), QRegExp::escape(*it)
					)
				);
			else
				rgxHlite.setPattern(
				QString("(?:\\s|^)%1(?:\\s|$)").arg(
					QRegExp::escape(*it)
					)
				);
			rgxHlite.setCaseSensitivity(Qt::CaseInsensitive);
			if(szStripMsg.contains(rgxHlite))
			{
				return triggerOnHighlight(wnd,type,nick,user,host,szMsg,*it);
			}
		}
	}

	if(wnd->type() == KVI_WINDOW_TYPE_CHANNEL)
	{
		if(((KviChannel *)wnd)->isHighlightedUser(nick))
			return triggerOnHighlight(wnd,type,nick,user,host,szMsg,nick);

		// FIXME: this is for userhighlighing
		//        maybe mark the users as highlighted in the console user database
		//        and then lookup them there ? this would be potentially a lot faster
		KviRegisteredUser * u = connection()->userDataBase()->registeredUser(nick,user,host);

		// note that we're highlighting users only in channels since
		// in a query (or DCC) highlighting the remote end is senseless.
		if(u)
		{
			if(u->getBoolProperty("highlight"))
				return triggerOnHighlight(wnd,type,nick,user,host,szMsg,nick);
		}
	}

	return type;
}

#define KVI_NUM_NICK_COLORS 95

static const char * g_nickColors[KVI_NUM_NICK_COLORS]=
{
	"0,1"  ,"0,2"  ,"0,3"  ,"0,4"  ,"0,5"  ,"0,6"  ,"0,10" ,"0,12" ,"0,14" , //9
	"1,0"  ,"1,4"  ,"1,7"  ,"1,8"  ,"1,9"  ,"1,11" ,"1,15" ,  //7
	"2,0"  ,"2,4"  ,"2,7"  ,"2,8"  ,"2,9"  ,"2,11" ,"2,15" ,  //7
	"3,8"  ,"3,9"  ,"3,0"  ,"3,15" , //4
	"4,0"  ,"4,1"  ,"4,8"  ,"4,9"  ,"4,11" ,"4,15" , //6
	"5,0"  ,"5,7"  ,"5,8"  ,"5,15" , //4
	"6,0"  ,"6,7"  ,"6,8"  ,"6,9"  ,"6,10" ,"6,11" ,"6,15" , //7
	"7,1"  ,"7,2"  ,"7,5"  ,"7,6"  ,"7,14" , //5
	"8,1"  ,"8,2"  ,"8,3"  ,"8,4"  ,"8,5"  ,"8,6"  ,"8,7"  ,"8,10" ,"8,12" ,"8,14" , //10
	"9,1"  ,"9,2"  ,"9,3"  ,"9,5"  ,"9,6"  ,"9,14" , //6
	"10,1" ,"10,2" , //2
	"11,1" ,"11,2" ,"11,3" ,"11,5" ,"11,6" ,"11,14", //6
	"12,0" ,"12,7" ,"12,8" ,"12,9" ,"12,10","12,11","12,15", //7
	"13,0" ,"13,1" ,"13,6" ,"13,8" ,"13,11","13,15", //6
	"14,0" ,"14,8" ,"14,11","14,15", //4
	"15,1" ,"15,2" ,"15,3" ,"15,6" ,"15,14" //5
};

void KviConsole::outputPrivmsg(KviWindow *wnd,
	int type,
	const QString &daNick,
	const QString &daUser,
	const QString &daHost,
	const QString &msg,
	int iFlags,
	const QString &prefix,
	const QString &suffix)
{
	// FIXME: #warning "THIS IS USED BY WINDOWS THAT ARE NOT BOUND TO THIS IRC CONTEXT"
	// FIXME: #warning "REMEMBER IT IN ESCAPE COMMANDS"
	// __range_valid(wnd);

	bool bIsChan = (wnd->type() == KVI_WINDOW_TYPE_CHANNEL);
	bool bIsNotice = ((type == KVI_OUT_CHANNELNOTICE)||(type == KVI_OUT_CHANNELNOTICECRYPTED) \
		   ||(type == KVI_OUT_QUERYNOTICE)||(type == KVI_OUT_QUERYNOTICECRYPTED));

	QString nick = daNick; // not that beautiful.. :/
	QString user = daUser;
	QString host = daHost;

	if(connection())
	{
		if(nick.isEmpty())nick = connection()->userInfo()->nickName();
		if(user.isEmpty())user = connection()->userInfo()->userName();
		if(host.isEmpty())host = connection()->userInfo()->hostName();
	}

	QString szDecodedMessage = msg; // shallow copy

	if(KVI_OPTION_BOOL(KviOption_boolStripMircColorsInUserMessages))
		szDecodedMessage = KviMircCntrl::stripControlBytes(szDecodedMessage);

	if(!(iFlags & NoHighlighting))
	{
		// HIGHLIGHTING BLOCK
		int iSaveType = type;
		type = applyHighlighting(wnd,type,nick,user,host,szDecodedMessage);
		if(type < 0)return; // event stopped the message!
		if(type == KVI_OUT_HIGHLIGHT)
		{
			if(!wnd->hasAttention())
			{
				if(KVI_OPTION_BOOL(KviOption_boolFlashWindowOnHighlightedMessages) &&
					(!(iFlags & NoWindowFlashing)))
				{
					wnd->demandAttention();
				}
				if(KVI_OPTION_BOOL(KviOption_boolPopupNotifierOnHighlightedMessages) &&
					(!(iFlags & NoNotifier)))
				{
					QString szMsg = "<b>&lt;";
					szMsg += nick;
					szMsg += "&gt;</b> ";
					szMsg += Qt::escape(szDecodedMessage);
					//debug("kvi_console.cpp:629 debug: %s",szMsg.data());
					g_pApp->notifierMessage(wnd,KVI_OPTION_MSGTYPE(iSaveType).pixId(),szMsg,KVI_OPTION_UINT(KviOption_uintNotifierAutoHideTime));
				}
			}
		}
	}

	// <PREFIX>nick[!user@host]<POSTFIX>This is a test message

	QString szNick;
	KviQString::sprintf(szNick,"\r!nc\r%Q\r",&nick);

	if(KVI_OPTION_BOOL(KviOption_boolShowUserAndHostInPrivmsgView))
		KviQString::appendFormatted(szNick,"!%Q@\r!h\r%Q\r",&user,&host);

	if(bIsChan && KVI_OPTION_BOOL(KviOption_boolShowChannelUserFlagInPrivmsgView))
		((KviChannel *)wnd)->prependUserFlag(nick,szNick);

	if(KVI_OPTION_BOOL(KviOption_boolColorNicks))
	{
		if(KVI_OPTION_BOOL(KviOption_boolUseSpecifiedSmartColorForOwnNick) && \
		(QString::compare(nick,connection()->userInfo()->nickName(),Qt::CaseSensitive)==0))
		{
			QString szOwnColor;
			int back = KVI_OPTION_UINT(KviOption_uintUserIrcViewOwnBackground);
			if(back==KVI_TRANSPARENT)
			{
				KviQString::sprintf(szOwnColor,"%d", \
					KVI_OPTION_UINT(KviOption_uintUserIrcViewOwnForeground));
			} else {
				KviQString::sprintf(szOwnColor,"%d,%d", \
					KVI_OPTION_UINT(KviOption_uintUserIrcViewOwnForeground), \
					back);
			}
			szNick.prepend(szOwnColor);
		} else {
			int sum = 0;
			int i = nick.length();
			const QChar * aux = nick.unicode();
			// FIXME: Shouldn't this be case insensitive ?
			while(i > 0)
			{
				sum += aux->unicode();
				aux++;
				i--;
			}
			szNick.prepend(g_nickColors[sum % KVI_NUM_NICK_COLORS]);
		}
		szNick.prepend(KVI_TEXT_COLOR);
		szNick.append(KVI_TEXT_COLOR);
	}
/*	if(KVI_OPTION_BOOL(KviOption_boolUseUserListColorsAsNickColors) && bIsChan)
	{
		if(((KviChannel*)wnd)->userListView())
		{
			KviUserListEntry *e = ((KviChannel*)wnd)->userListView()->findEntry(nick);
			if(e)
			{
				int sum = 0;
				int i = nick.length();
				const QChar * aux = nick.unicode();
				// FIXME: Shouldn't this be case insensitive ?
				while(i > 0)
				{
					sum += aux->unicode();
					aux++;
					i--;
				}
				int color;
				if(e->flags() == 0)
				{
					color = KVI_COLOR_EXT_USER_NORMAL;
				} else {
					color = (e->flags() & KVI_USERFLAG_CHANOWNER) ? \
						KVI_COLOR_EXT_USER_OWNER : ((e->flags() & KVI_USERFLAG_CHANADMIN) ? \
						KVI_COLOR_EXT_USER_ADMIN : ((e->flags() & KVI_USERFLAG_OP) ? \
						KVI_COLOR_EXT_USER_OP : ((e->flags() & KVI_USERFLAG_HALFOP) ? \
						KVI_COLOR_EXT_USER_HALFOP : ((e->flags() & KVI_USERFLAG_VOICE) ? \
						KVI_COLOR_EXT_USER_VOICE : KVI_COLOR_EXT_USER_USEROP))));
				}
				szNick.prepend(QString("%1").arg(color));
				szNick.prepend(KVI_TEXT_COLOR);
				szNick.append(KVI_TEXT_COLOR);
			}
		}
	}*/
	if(KVI_OPTION_BOOL(KviOption_boolBoldedNicks))
	{
		szNick.prepend(KVI_TEXT_BOLD);
		szNick.append(KVI_TEXT_BOLD);
	}

	QString szMessage;

	if(KVI_OPTION_BOOL(KviOption_boolUseExtendedPrivmsgView))
	{
		szMessage = prefix.isEmpty() ? KVI_OPTION_STRING(KviOption_stringExtendedPrivmsgPrefix) : prefix;
		szMessage += szNick;
		szMessage += suffix.isEmpty() ? KVI_OPTION_STRING(KviOption_stringExtendedPrivmsgPostfix) : suffix;
	} else {
		if(bIsNotice)
		{
			static QString pre1("*");
			static QString suf1("* ");
			szMessage = prefix.isEmpty() ? pre1 : prefix;
			szMessage += szNick;
			szMessage += suffix.isEmpty() ? suf1 : suffix;
		} else {
			static QString pre2("<");
			static QString suf2("> ");
			szMessage = prefix.isEmpty() ? pre2 : prefix;
			szMessage += szNick;
			szMessage += suffix.isEmpty() ? suf2 : suffix;
		}
	}

	szMessage += szDecodedMessage;

	if(bIsChan)((KviChannel *)wnd)->outputMessage(type,szMessage);
	else wnd->outputNoFmt(type,szMessage);
}

void KviConsole::avatarChangedUpdateWindows(const QString &nick,const QString &textLine)
{
	if(!connection())return; //ops...

	// in quiet mode avoid bugging the user about avatar changes
	bool bOut = ((!textLine.isEmpty()) && (!(_OUTPUT_QUIET)));

	for(KviChannel * c = connection()->channelList()->first();c;c = connection()->channelList()->next())
	{
		if(c->avatarChanged(nick))
		{
			if(bOut)c->outputNoFmt(KVI_OUT_AVATAR,textLine);
		}
	}
	for(KviQuery * q = connection()->queryList()->first();q;q = connection()->queryList()->next())
	{
		if(q->avatarChanged(nick))
		{
			if(bOut)q->outputNoFmt(KVI_OUT_AVATAR,textLine);
		}
	}
	m_pNotifyListView->avatarChanged(nick); // recalc the item height here too!
}

void KviConsole::avatarChanged(KviAvatar * avatar,const QString &nick,const QString &user,const QString &host,const QString &textLine)
{
	if(!connection())return; //ops...

	bool bRegisteredStuff = false;

	if(KVI_OPTION_BOOL(KviOption_boolSetLastAvatarAsDefaultForRegisteredUsers))
	{
		// Don't even try to do it for myself
		if(!KviQString::equalCI(nick,connection()->userInfo()->nickName()))
		{
			KviRegisteredUser * u = connection()->userDataBase()->registeredUser(nick,user,host);
			if(u)
			{
				if(avatar)u->setProperty("avatar",avatar->identificationString());
				else u->setProperty("avatar",QString());
				bRegisteredStuff = true;
			}
		}
	}

	if(!bRegisteredStuff)
	{
		// cache it
		if(avatar)
			KviAvatarCache::instance()->replace(avatar->identificationString(),KviIrcMask(nick,user,host),currentNetworkName().toUtf8().data());
		else
			KviAvatarCache::instance()->remove(KviIrcMask(nick,user,host),currentNetworkName().toUtf8().data());
	}

	avatarChangedUpdateWindows(nick,textLine);
}

void KviConsole::checkDefaultAvatar(KviIrcUserEntry *e,const QString &nick,const QString &user,const QString &host)
{
	// look it up in the cache
	QString szAvatar = KviAvatarCache::instance()->lookup(KviIrcMask(nick,user,host),currentNetworkName().toUtf8().data());
	if(!szAvatar.isEmpty())
	{
		// got a cache hit... is it on disk ?
		KviAvatar * avatar = g_pIconManager->getAvatar(QString(),szAvatar);
		if(avatar)
		{
			// cached image on disk
			e->setAvatar(avatar);
			avatarChangedUpdateWindows(nick,QString());
			return;
		} else {
			// no cached image on disk.. will need to requery it anyway
			// remove from cache
			KviAvatarCache::instance()->remove(KviIrcMask(nick,user,host),currentNetworkName().toUtf8().data());
		}
	}

	// registered ?
	KviRegisteredUser * u = connection()->userDataBase()->registeredUser(nick,user,host);
	if(u)
	{
		// the user is registered...
		QString szAvatar;
		if(u->getProperty("avatar",szAvatar))
		{
			// the user has a default avatar...
			KviAvatar * avatar = g_pIconManager->getAvatar(QString(),szAvatar);
			if(avatar)
			{
				e->setAvatar(avatar);
				avatarChangedUpdateWindows(nick,QString());
				return;
			}
		}
	}
}

void KviConsole::resetAvatarForMatchingUsers(KviRegisteredUser * u)
{
	if(!connection())return;

	QString szAvatar;
	if(!u->getProperty("avatar",szAvatar))return;

	KviPointerHashTableIterator<QString,KviIrcUserEntry> it(*(connection()->userDataBase()->dict()));
	while(KviIrcUserEntry * e = it.current())
	{
		if(e->hasHost())
		{
			if(u->matchesFixed(it.currentKey(),e->user(),e->host()))
			{
				KviAvatar * a = g_pIconManager->getAvatar(QString(),szAvatar);
				e->setAvatar(a);
				avatarChangedUpdateWindows(it.currentKey(),QString());
			}
		}
		++it;
	}
}

KviAvatar * KviConsole::setAvatar(const QString &nick,const QString &user,const QString &host,const QString &szLocalPath,const QString &szName)
{
	if(!connection())return 0;
	KviIrcUserEntry * e = connection()->userDataBase()->find(nick);
	if(e)
	{
		// User and host must match
		if((!user.isEmpty()) && e->hasUser())
		{
			if(!KviQString::equalCI(user,e->user()))return 0;
		}

		if((!host.isEmpty()) && e->hasHost())
		{
			if(!KviQString::equalCI(host,e->host()))return 0;
		}

		// Ok...got it
		KviAvatar * avatar = g_pIconManager->getAvatar(szLocalPath,szName);
		if(avatar)
		{
			e->setAvatar(avatar);
			avatarChanged(avatar,nick,user,host,QString());
			return avatar;
		} else {
			if(_OUTPUT_PARANOIC)
				output(KVI_OUT_VERBOSE,__tr2qs("Failed to load avatar with name \"%Q\" and local path \"%Q\""),&szName,&szLocalPath);
		}
	}
	return 0;
}

KviAvatar * KviConsole::defaultAvatarFromOptions()
{
	QPixmap * avatar = KVI_OPTION_PIXMAP(KviOption_pixmapMyAvatar).pixmap();
	if(!avatar)return 0;
	if(avatar->isNull())return 0;
	if(KVI_OPTION_STRING(KviOption_stringMyAvatar).isEmpty())return 0;
	KviAvatar * loadedAvatar = new KviAvatar(KVI_OPTION_PIXMAP(KviOption_pixmapMyAvatar).path(),KVI_OPTION_STRING(KviOption_stringMyAvatar));
	if(loadedAvatar->isValid())
	{
		return loadedAvatar;
	} else {
		delete loadedAvatar;
		return 0;
	}
}

KviAvatar * KviConsole::currentAvatar()
{
	if(!connection())return 0;
	KviIrcUserEntry * e = connection()->userDataBase()->find(connection()->userInfo()->nickName());
	if(!e)return 0;
	KviAvatar * a = e->avatar();
	if(!a)
	{
		a = defaultAvatarFromOptions();
		if(a)
		{
			e->setAvatar(a);
			avatarChanged(a,connection()->userInfo()->nickName(),QString(),QString(),QString());
		}
	}
	return a;
}

void KviConsole::setAvatarFromOptions()
{
	if(!connection())return;
	KviIrcUserEntry * e = connection()->userDataBase()->find(connection()->userInfo()->nickName());
	if(!e)return;
	KviAvatar * a = defaultAvatarFromOptions();
	if(a)
	{
		e->setAvatar(a);
		avatarChanged(a,connection()->userInfo()->nickName(),QString(),QString(),QString());
	}
}

void KviConsole::applyOptions()
{
	m_pNotifyListView->applyOptions();
	m_pInput->applyOptions();
	m_pIrcView->applyOptions();

	KviWindow::applyOptions();

	// trick
	resize(width() - 1,height() - 1);
	resize(width() + 1,height() + 1);
}

void KviConsole::resizeEvent(QResizeEvent *)
{
	int hght = m_pInput->heightHint();
	int hght2 = m_pButtonBox->sizeHint().height();
	m_pButtonBox->setGeometry(0,0,width(),hght2);
	m_pSplitter->setGeometry(0,hght2,width(),height() - (hght + hght2));
	m_pInput->setGeometry(0,height() - hght,width(),hght);
}

QSize KviConsole::sizeHint() const
{
	QSize ret(m_pIrcView->sizeHint().height(),m_pIrcView->sizeHint().height() + m_pInput->heightHint());
	return ret;
}

void KviConsole::fillStatusString()
{
	switch(context()->state())
	{
		case KviIrcContext::Idle:
			m_szStatusString = __tr2qs("No connection");
		break;
		case KviIrcContext::Connecting:
			m_szStatusString = __tr2qs("Connection in progress...");
		break;
		case KviIrcContext::LoggingIn:
			m_szStatusString = __tr2qs("Login in progress...");
		break;
		case KviIrcContext::Connected:
			m_szStatusString = connection()->userInfo()->nickName();
			if(!connection()->userInfo()->userMode().isEmpty())
			{
				m_szStatusString += " (+";
				m_szStatusString += connection()->userInfo()->userMode();

				if(connection()->userInfo()->isAway())
				{
					m_szStatusString += QChar(' ');
					m_szStatusString += __tr2qs("away");
				}
				m_szStatusString += QChar(')');
			} else {
				if(connection()->userInfo()->isAway())
				{
					m_szStatusString += " (";
					m_szStatusString += __tr2qs("away");
					m_szStatusString += QChar(')');
				}
			}

			m_szStatusString += __tr2qs(" on ");
			m_szStatusString += connection()->serverInfo()->name();
		break;
	}
}

void KviConsole::fillCaptionBuffers()
{
	fillStatusString();

	static QString begin("<nobr><font color=\"");
	static QString boldbegin("\"><b>");
	static QString endofbold("</b></font> <font color=\"");
	static QString endoffont("\">");
	static QString end("</font></nobr>");

	m_szPlainTextCaption = windowName();
	m_szPlainTextCaption += " [";
	m_szPlainTextCaption += m_szStatusString;
	m_szPlainTextCaption += QChar(']');

	m_szHtmlActiveCaption = begin;
	m_szHtmlActiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextActive).name();
	m_szHtmlActiveCaption += boldbegin;
	m_szHtmlActiveCaption += windowName();
	m_szHtmlActiveCaption += endofbold;
	m_szHtmlActiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextActive2).name();
	m_szHtmlActiveCaption += endoffont;
	m_szHtmlActiveCaption += m_szStatusString;
	m_szHtmlActiveCaption += end;

	m_szHtmlInactiveCaption = begin;
	m_szHtmlInactiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextInactive).name();
	m_szHtmlInactiveCaption += boldbegin;
	m_szHtmlInactiveCaption += windowName();
	m_szHtmlInactiveCaption += endofbold;
	m_szHtmlInactiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextInactive2).name();
	m_szHtmlInactiveCaption += endoffont;
	m_szHtmlInactiveCaption += m_szStatusString;
	m_szHtmlInactiveCaption += end;
}

QPixmap * KviConsole::myIconPtr()
{
	return g_pIconManager->getSmallIcon(isConnected() ? KVI_SMALLICON_LINKS : KVI_SMALLICON_CONSOLE);
}

void KviConsole::getWindowListTipText(QString &buffer)
{
	fillStatusString();

	static QString html_bold("<b>");
	static QString html_tab("&nbsp;&nbsp;");
	static QString html_eofbold("</b>");
	static QString html_hrbr("<br><hr>");
	static QString html_channel(__tr2qs("channel"));
	static QString html_channels(__tr2qs("channels"));
	static QString html_query(__tr2qs("query"));
	static QString html_queries(__tr2qs("queries"));
	static QString html_space(" ");
	static QString html_commaspace(", ");
	static QString html_br("<br>");
	static QString html_spaceparopen(" (");
	static QString html_spaceparclosed(")");

	buffer = "<table width=\"100%\">" \
		START_TABLE_BOLD_ROW;
	buffer += m_szStatusString;
	buffer += END_TABLE_BOLD_ROW;
	if((context()->state() == KviIrcContext::Connected) && connection())
	{
		QString num;

		unsigned int uD;
		unsigned int uH;

		uD = connection()->channelList()->count();
		uH = connection()->queryList()->count();

		if(uD || uH > 0)
		{
			buffer += "<tr><td>";
			buffer += html_tab;

			if(uD > 0)
			{
				num.setNum(uD);

				buffer += html_bold;
				buffer += num;
				buffer += html_eofbold;
				buffer += html_space;
				buffer += uD > 1 ? html_channels : html_channel;
				if(uH > 0)
					buffer += html_commaspace;
			}

			if(uH > 0)
			{
				num.setNum(uH);
				buffer += html_bold;
				buffer += num;
				buffer += html_eofbold;
				buffer += html_space;
				buffer += uH > 1 ? html_queries : html_query;
			}
			buffer += "</td></tr>";
		}

		QDateTime dt;
		dt.setTime_t(connection()->statistics()->connectionStartTime());

		buffer += START_TABLE_NORMAL_ROW;

		buffer += __tr2qs("Connected since");
		buffer += html_space;
		buffer += html_br;
		buffer += html_tab;
		buffer += html_bold;
		buffer += dt.toString();
		buffer += html_eofbold;
		buffer += html_br;

		//buffer += html_spaceparopen;

		QString tspan = KviTimeUtils::formatTimeInterval((unsigned int)(kvi_secondsSince(connection()->statistics()->connectionStartTime())),
			KviTimeUtils::NoLeadingEmptyIntervals | KviTimeUtils::NoLeadingZeroes);

		buffer += __tr2qs("Online for");
		buffer += html_space;
		buffer += html_bold;
		buffer += tspan;
		buffer += html_eofbold;
		//buffer += html_spaceparclosed;

		buffer += "</td></tr><tr><td bgcolor=\"#F0F0F0\">";

		tspan = KviTimeUtils::formatTimeInterval((unsigned int)(kvi_secondsSince(connection()->statistics()->lastMessageTime())),
			KviTimeUtils::NoLeadingEmptyIntervals | KviTimeUtils::NoLeadingZeroes);

		buffer += __tr2qs("Server idle for");
		buffer += html_space;
		buffer += html_bold;
		buffer += tspan;
		buffer += "</b></td></tr>";
	}

	buffer += "</table>";
}
#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "kvi_console.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
