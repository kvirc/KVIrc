//=============================================================================
//
//   File : kvi_query.cpp
//   Creation date : Tue Aug 7 2000 14:23:22 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2004 Szymon Stefanek (pragma at kvirc dot net)
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

#define __KVIRC__
#include "kvi_query.h"
#include "kvi_console.h"
#include "kvi_iconmanager.h"
#include "kvi_ircview.h"
#include "kvi_input.h"
#include "kvi_options.h"
#include "kvi_locale.h"
#include "kvi_settings.h"
#include "kvi_ircsocket.h"
#include "kvi_out.h"
#include "kvi_taskbar.h"
#include "kvi_config.h"
#include "kvi_mirccntrl.h"
#include "kvi_settings.h"
#include "kvi_themedlabel.h"
#include "kvi_useraction.h"
#include "kvi_parameterlist.h"
#include "kvi_ircconnection.h"
#include "kvi_ircconnectionuserinfo.h"
#include "kvi_sparser.h"
#include "kvi_ircuserdb.h"
#include "kvi_mirccntrl.h"
#include "kvi_toolwindows_container.h"
#include "kvi_qcstring.h"

#ifdef COMPILE_CRYPT_SUPPORT
	#include "kvi_crypt.h"
	#include "kvi_cryptcontroller.h"
#endif

#include "kvi_kvs_eventtriggers.h"

#include <qpixmap.h>
#include <qsplitter.h>
#include "kvi_tal_hbox.h"
#include <qtoolbutton.h>
#include "kvi_draganddrop.h"
#include "kvi_valuelist.h"

KviQuery::KviQuery(KviFrame * lpFrm,KviConsole * lpConsole,const QString &nick)
: KviWindow(KVI_WINDOW_TYPE_QUERY,lpFrm,nick,lpConsole)
{

	m_iFlags = 0;
	connection()->registerQuery(this);

	//m_pTopSplitter = new QSplitter(QSplitter::Horizontal,this,"top_splitter");
	m_pButtonBox = new KviTalHBox(this);
	m_pLabel = new KviThemedLabel(m_pButtonBox,"query_label");
	m_pLabel->setAutoHeight(1);
	updateLabelText();
	m_pButtonBox->setStretchFactor(m_pLabel,1);

	// The button box on the right
	//KviTalHBox * box = new KviTalHBox(m_pTopSplitter,"button_box");
	if(KVI_OPTION_BOOL(KviOption_boolShowExtendedInfoInQueryLabel))
		m_pButtonGrid= new KviTalGrid(2,Qt::Vertical,m_pButtonBox);
	else
		m_pButtonGrid= new KviTalGrid(1,Qt::Vertical,m_pButtonBox);


	createTextEncodingButton(m_pButtonGrid);

#ifdef COMPILE_USE_QT4
	m_pSplitter = new QSplitter(Qt::Horizontal,this,"main_splitter");
#else
	m_pSplitter = new QSplitter(QSplitter::Horizontal,this,"main_splitter");
#endif
	m_pIrcView = new KviIrcView(m_pSplitter,lpFrm,this);
	connect(m_pIrcView,SIGNAL(rightClicked()),this,SLOT(textViewRightClicked()));
	//m_pEditorsContainer= new KviToolWindowsContainer(m_pSplitter);
		
		
	m_pListViewButton = new KviWindowToolPageButton(KVI_SMALLICON_HIDELISTVIEW,KVI_SMALLICON_SHOWLISTVIEW,__tr2qs("Show User List"),buttonContainer(),true,"list_view_button");
	connect(m_pListViewButton,SIGNAL(clicked()),this,SLOT(toggleListView()));
	
#ifdef COMPILE_CRYPT_SUPPORT
	createCryptControllerButton(m_pButtonGrid);
#endif
	
	m_pUserListView = new KviUserListView(m_pSplitter,m_pListViewButton,connection()->userDataBase(),this,7,__tr2qs("Query Targets"),"user_list_view");

	m_pInput   = new KviInput(this,m_pUserListView);

	if(KVI_OPTION_BOOL(KviOption_boolAutoLogQueries)) m_pIrcView->startLogging();
	// FIXME: #warning "Maybe tell the user all that we know about the remote end(s)....channels..."

	m_pIrcView->enableDnd(TRUE);
	connect(m_pIrcView,SIGNAL(fileDropped(const char *)),this,SLOT(slotDndEvents(const char *)));
	
	updateCaption();
}

KviQuery::~KviQuery()
{
	m_pUserListView->partAll();
	if(type() == KVI_WINDOW_TYPE_DEADQUERY)
		context()->unregisterDeadQuery(this);
	else
		connection()->unregisterQuery(this);
}

void KviQuery::updateLabelText()
{
	QString szText=getInfoLabelText();
	if(szText!=m_pLabel->text())
	{
		m_pLabel->setText(szText);
		KviTalToolTip::add(m_pLabel,getInfoLabelTipText());
	}
}

QString KviQuery::getInfoLabelTipText()
{
	QString txt;
	KviIrcUserEntry * e = connection()->userDataBase()->find(m_szName);
	if(e)
	{
		QString tmp;
		QString szMask;
		if(e->hasUser()) 
			szMask+=e->user();
		else 
			szMask+="*";
		szMask+="@";
		if(e->hasHost()) 
			szMask+=e->host();
		else 
			szMask+="*";
		tmp+="\n";
		QString szChans;
		connection()->getCommonChannels(m_szName,szChans,0);
		if(console()->connection())
		{
		
			txt = "<html>" \
					"<body>" \
						"<table width=\"100%\">";
		
			txt +=          "<tr><td bgcolor=\"#303030\">" \
								"<font color=\"#FFFFFF\"><b>";
			txt += __tr2qs("Query target:");
			txt +=              "</b></font>" \
							"</td></tr>";
			txt +=          "<tr><td>";
		
			if(e->hasRealName())
				tmp=__tr2qs("%1 is %2 (%3)").arg(m_szName).arg(szMask).arg(KviMircCntrl::stripControlBytes(e->realName()));
			else
				tmp=__tr2qs("%1 is %2").arg(m_szName).arg(szMask);
			
			tmp.replace('&',"&amp;");
			tmp.replace('<',"&lt;");
			tmp.replace('>',"&gt;");
			
			txt += tmp;
			
			txt +=          "</td></tr>";
			
			if(e->hasServer())
			{
				txt+="<tr><td>";
				if(e->hasHops())
					txt+=__tr2qs("%1 is using irc server: %2 (%3 hops)").arg(m_szName).arg(e->server()).arg(e->hops());
				else
					txt+=__tr2qs("%1 is using irc server: %2").arg(m_szName).arg(e->server());
				txt+="</td></tr>";
			}
			
			if(e->isAway())
			{
				txt+="<tr><td>";
				txt+=__tr2qs("%1 is probably away").arg(m_szName);
				txt+="</td></tr>";
			}
			
			txt+="<tr><td>";
			tmp=__tr2qs("Common channels with %1: %2").arg(m_szName).arg(szChans);
			
			tmp.replace('&',"&amp;");
			tmp.replace('<',"&lt;");
			tmp.replace('>',"&gt;");
			
			txt+=tmp;
			txt +="</td></tr>";	
			
			txt += "</table>" \
				"</body>" \
			"<html>";
		} else {
			txt=__tr2qs("[Dead Query]");
		}
	}
	return txt;
}

QString KviQuery::getInfoLabelText()
{
	QString tmp;
	if(KVI_OPTION_BOOL(KviOption_boolShowExtendedInfoInQueryLabel))
	{
		KviIrcUserEntry * e = connection()->userDataBase()->find(m_szName);
		if(e)
		{
			QString szMask;
			if(console()->connection())
			{
				if(e->hasUser()) 
					szMask+=e->user();
				else 
					szMask+="*";
				szMask+="@";
				if(e->hasHost()) 
					szMask+=e->host();
				else 
					szMask+="*";
				if(e->hasRealName())
					tmp=__tr2qs("Query with %1!%2 (%3)").arg(m_szName).arg(szMask).arg(KviMircCntrl::stripControlBytes(e->realName()));
				else
					tmp=__tr2qs("Query with %1!%2").arg(m_szName).arg(szMask);
				if(e->hasServer())
					tmp+=__tr2qs(", using server %1").arg(e->server());
				if(e->hasHops())
					tmp+=__tr2qs(" (%1 hops)").arg(e->hops());
				if(e->isAway())
					tmp+=__tr2qs(", probably away");
				tmp+="\n";
				QString szChans;
				connection()->getCommonChannels(m_szName,szChans,0);
				tmp+=__tr2qs("Common channels: %2").arg(szChans);
			} else {
				tmp=__tr2qs("[Dead Query]");
			}
		}
	}
	return tmp;
}
void KviQuery::slotDndEvents(const char *file)
{	
	KVS_TRIGGER_EVENT_1(KviEvent_OnQueryFileDropped,this,QString(file));
}

void KviQuery::triggerCreationEvents()
{
	if(!KVI_OPTION_STRING(KviOption_stringOnNewQueryOpenedSound).isEmpty()) KviKvsScript::run("snd.play $0",0,new KviKvsVariantList(new KviKvsVariant(KVI_OPTION_STRING(KviOption_stringOnNewQueryOpenedSound))));
	KVS_TRIGGER_EVENT_0(KviEvent_OnQueryWindowCreated,this);
}

void KviQuery::getBaseLogFileName(QString &buffer)
{
	if(console()->connection())
	{
		buffer=windowName();
		buffer += ".";
		buffer += console()->currentNetworkName();
	} else {
		buffer=windowName();
		buffer+=".";
		buffer+=console()->ircContextId();
	}
}

void KviQuery::mergeQuery(KviQuery * q)
{
	m_pIrcView->appendMessagesFrom(q->m_pIrcView);
	updateLabelText();
}

void KviQuery::textViewRightClicked()
{
	KVS_TRIGGER_EVENT_0(KviEvent_OnQueryPopupRequest,this);
}

void KviQuery::saveProperties(KviConfig *cfg)
{
	KviWindow::saveProperties(cfg);
	cfg->writeEntry("Splitter",m_pSplitter->sizes());
	cfg->writeEntry("UserListViewVisible",m_pUserListView->isVisible());
}

void KviQuery::loadProperties(KviConfig *cfg)
{
	int w = width();
	KviWindow::loadProperties(cfg);
	KviValueList<int> def;
	def.append((w * 80) / 100);
	def.append((w * 20) / 100);
	m_pSplitter->setSizes(cfg->readIntListEntry("Splitter",def));
	showListView(cfg->readBoolEntry("UserListViewVisible",false));
}

void KviQuery::notifyTargetChange(const QString &oldNick,const QString &oldUser,const QString &oldHost,const QString &nick,const QString &user,const QString &host)
{
	QString oldN = oldNick.isEmpty() ? QString("*") : oldNick;
	QString oldU = oldUser.isEmpty() ? QString("*") : oldUser;
	QString oldH = oldHost.isEmpty() ? QString("*") : oldHost;
	output(KVI_OUT_QUERYTRACE,
		__tr2qs("The target of this query has changed from \r!n\r%Q\r [%Q@\r!h\r%Q\r] to \r!n\r%Q\r [%Q@\r!h\r%Q\r]"),
		&oldN,&oldU,&oldH,&nick,&user,&host);
	updateLabelText();
}


void KviQuery::userAction(const QString &nick,const QString &user,const QString &host,unsigned int uActionType)
{
	int iTemperature = kvi_getUserActionTemperature(uActionType);
	if(KVI_OPTION_BOOL(KviOption_boolEnableQueryTracing))
	{
		QString oldUser,oldHost;
		if(!m_pUserListView->userActionVerifyMask(nick,user,host,iTemperature,oldUser,oldHost))
			notifyTargetChange(nick,oldUser,oldHost,nick,user,host);
	} else {
		m_pUserListView->userAction(nick,user,host,iTemperature);
	}
	updateLabelText();
}


void KviQuery::userAction(const QString &nick,unsigned int uActionType)
{
	int iTemperature = kvi_getUserActionTemperature(uActionType);
	m_pUserListView->userAction(nick,iTemperature);
	updateLabelText();
}

void KviQuery::userAction(KviIrcMask *user,unsigned int uActionType)
{
	int iTemperature = kvi_getUserActionTemperature(uActionType);
	if(KVI_OPTION_BOOL(KviOption_boolEnableQueryTracing))
	{
		QString oldUser,oldHost;
		if(!m_pUserListView->userActionVerifyMask(user->nick(),user->user(),user->host(),iTemperature,oldUser,oldHost))
			notifyTargetChange(user->nick(),oldUser,oldHost,user->nick(),user->user(),user->host());
	} else {
		m_pUserListView->userAction(user,iTemperature);
	}
	updateLabelText();
}


KviUserListEntry * KviQuery::setTarget(const QString &nick,const QString &user,const QString &host)
{
	KviUserListEntry * e = m_pUserListView->join(nick,user,host);
	if((!e->globalData()->avatar()) && (!user.isEmpty()) && (user != "*"))
		m_pConsole->checkDefaultAvatar(e->globalData(),nick,user,host);

	setWindowName(nick);
	updateCaption();

	if(KVI_OPTION_BOOL(KviOption_boolEnableQueryTracing))
	{
		QString szChans;
		int iChans = m_pConsole->connection()->getCommonChannels(nick,szChans);
		notifyCommonChannels(nick,user,host,iChans,szChans);
	}

	KVS_TRIGGER_EVENT_3(KviEvent_OnQueryTargetAdded,this,nick,user,host);
	updateLabelText();
	return e;	
}

void KviQuery::notifyCommonChannels(const QString &nick,const QString &user,const QString &host,int iChans,const QString &szChans)
{
	static QString star("*");
	if(iChans > 0)
	{
		output(KVI_OUT_QUERYTRACE,__tr2qs("Common channels for \r!n\r%Q\r [%Q@\r!h\r%Q\r]: %Q"),
			&nick,user.isEmpty() ? &star : &user,host.isEmpty() ? &star : &host,&szChans);
	} else {
		output(KVI_OUT_QUERYTRACE,__tr2qs("No common channels for \r!n\r%Q\r [%Q@\r!h\r%Q\r]"),
			&nick,user.isEmpty() ? &star : &user,host.isEmpty() ? &star : &host);
	}
	updateLabelText();
}




/*void KviQuery::updateTargets()
{
	QString szName;

	if(targetCount() > 0)
	{
		KviDictIterator<KviUserListEntry> it(*(m_pUserListView->entryDict()));
		if(it.current())szName = it.currentKey();
	}

	if(szName.isEmpty())szName = __tr2qs("[No targets]");

	setWindowName(szName);

	updateCaption();
}*/

void KviQuery::fillCaptionBuffers()
{
	static QString begin("<nobr><font color=\"");
	static QString boldbegin("\"><b>");
	static QString endofbold("</b></font> <font color=\"");
	static QString endoffont("\">");
	static QString end("</font></nobr>");

	if(!console()->connection())
	{
		QString dead = __tr2qs("[Dead query]");

		m_szPlainTextCaption = windowName();
		m_szPlainTextCaption += " : ";
		m_szPlainTextCaption += dead;

		m_szHtmlActiveCaption = begin;
		m_szHtmlActiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextActive).name();
		m_szHtmlActiveCaption += boldbegin;
		m_szHtmlActiveCaption += windowName();
		m_szHtmlActiveCaption += endofbold;
		m_szHtmlActiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextActive2).name();
		m_szHtmlActiveCaption += endoffont;
		m_szHtmlActiveCaption += dead;
		m_szHtmlActiveCaption += end;

		m_szHtmlInactiveCaption = begin;
		m_szHtmlInactiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextInactive).name();
		m_szHtmlInactiveCaption += boldbegin;
		m_szHtmlInactiveCaption += windowName();
		m_szHtmlInactiveCaption += endofbold;
		m_szHtmlInactiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextInactive2).name();
		m_szHtmlInactiveCaption += endoffont;
		m_szHtmlInactiveCaption += dead;
		m_szHtmlInactiveCaption += end;

		return;
	}

	QString szNickOnServer = QChar('[');
	szNickOnServer += connection()->currentNickName();
	szNickOnServer += __tr2qs(" on ");
	szNickOnServer += connection()->currentServerName();
	szNickOnServer += QChar(']');

	m_szPlainTextCaption = windowName();
	m_szPlainTextCaption += QChar(' ');
	m_szPlainTextCaption += szNickOnServer;

	m_szHtmlActiveCaption = begin;
	m_szHtmlActiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextActive).name();
	m_szHtmlActiveCaption += boldbegin;
	m_szHtmlActiveCaption += windowName();
	m_szHtmlActiveCaption += endofbold;
	m_szHtmlActiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextActive2).name();
	m_szHtmlActiveCaption += endoffont;
	m_szHtmlActiveCaption += szNickOnServer;
	m_szHtmlActiveCaption += end;

	m_szHtmlInactiveCaption = begin;
	m_szHtmlInactiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextInactive).name();
	m_szHtmlInactiveCaption += boldbegin;
	m_szHtmlInactiveCaption += windowName();
	m_szHtmlInactiveCaption += endofbold;
	m_szHtmlInactiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextInactive2).name();
	m_szHtmlInactiveCaption += endoffont;
	m_szHtmlInactiveCaption += szNickOnServer;
	m_szHtmlInactiveCaption += end;
}

bool KviQuery::nickChange(const QString &oldNick,const QString &newNick)
{
	bool bRet = m_pUserListView->nickChange(oldNick,newNick);
	if(!bRet)return false; // ugh!! ?
	setWindowName(newNick);
	updateCaption();
	updateLabelText();
	return bRet;
}

void KviQuery::showListView(bool bShow)
{
	if(!bShow)
	{
		m_pUserListView->hide();
		if(m_pListViewButton->isOn())m_pListViewButton->setOn(false);
	} else {
		m_pUserListView->show();
		if(!(m_pListViewButton->isOn()))m_pListViewButton->setOn(true);
	}
}

void KviQuery::toggleListView()
{
	showListView(!m_pUserListView->isVisible());
}

void KviQuery::setDeadQuery()
{
	m_iFlags |= KVI_QUERY_FLAG_DEAD;

	m_pUserListView->enableUpdates(false);
	m_pUserListView->partAll();
	m_pUserListView->enableUpdates(true);
	m_pUserListView->setUserDataBase(0);
	connection()->unregisterQuery(this);
	context()->registerDeadQuery(this);
	setType(KVI_WINDOW_TYPE_DEADQUERY);

	updateIcon();
	updateCaption();
	updateLabelText();
};

void KviQuery::setAliveQuery()
{
	m_iFlags &= ~KVI_QUERY_FLAG_DEAD;
	m_pUserListView->setUserDataBase(connection()->userDataBase());
	setType(KVI_WINDOW_TYPE_QUERY);
	context()->unregisterDeadQuery(this);
	connection()->registerQuery(this);
	// Update log file name
	if(m_pIrcView->isLogging()) m_pIrcView->startLogging();
	updateIcon();
	updateCaption();
	updateLabelText();
}

void KviQuery::applyOptions()
{
	m_pUserListView->applyOptions();
	updateLabelText();
	// this applies options for IrcView and Input and forces the window to relayout
	KviWindow::applyOptions();
}

QPixmap * KviQuery::myIconPtr()
{
	return g_pIconManager->getSmallIcon(isDeadQuery() ? KVI_SMALLICON_DEADQUERY : KVI_SMALLICON_QUERY);
}

void KviQuery::resizeEvent(QResizeEvent *e)
{
	int hght = m_pInput->heightHint();
	int hght2 = m_pButtonBox->sizeHint().height();
	m_pButtonBox->setGeometry(0,0,width(),hght2);
	m_pSplitter->setGeometry(0,hght2,width(),height() - (hght + hght2));
	m_pInput->setGeometry(0,height() - hght,width(),hght);
}

QSize KviQuery::sizeHint() const
{
	QSize ret(m_pSplitter->sizeHint().width(),m_pIrcView->sizeHint().height() + m_pInput->heightHint());
	return ret;
}

void KviQuery::ownMessage(const QString &buffer)
{
	if(!connection())
	{
		outputNoFmt(KVI_OUT_SYSTEMWARNING,__tr2qs("This query has no active targets, no message sent"));
		return;
	}
	
	KviQCString szName = connection()->encodeText(windowName());
	KviQCString szData = encodeText(buffer);

	const char * d = szData.data();
	if(!d)return;
	
#ifdef COMPILE_CRYPT_SUPPORT
	if(cryptSessionInfo())
	{
		if(cryptSessionInfo()->bDoEncrypt)
		{
			if(*d != KVI_TEXT_CRYPTESCAPE)
			{
				KviStr encrypted;
				cryptSessionInfo()->pEngine->setMaxEncryptLen(500 - szName.length());
				switch(cryptSessionInfo()->pEngine->encrypt(d,encrypted))
				{
					case KviCryptEngine::Encrypted:
						if(!connection()->sendFmtData("PRIVMSG %s :%s",szName.data(),encrypted.ptr()))return;
						m_pConsole->outputPrivmsg(this,KVI_OUT_OWNPRIVMSGCRYPTED,
							QString::null,QString::null,QString::null,buffer,KviConsole::NoNotifications);
					break;
					case KviCryptEngine::Encoded:
					{
						if(!connection()->sendFmtData("PRIVMSG %s :%s",szName.data(),encrypted.ptr()))return;
						// ugly ,but we must redecode here
						QString szRedecoded = decodeText(encrypted.ptr());
						m_pConsole->outputPrivmsg(this,KVI_OUT_OWNPRIVMSG,
							QString::null,QString::null,QString::null,szRedecoded,KviConsole::NoNotifications);
					}
					break;
					default: // also case KviCryptEngine::EncryptError
					{
						QString szEngineError = cryptSessionInfo()->pEngine->lastError();
						output(KVI_OUT_SYSTEMERROR,
							__tr2qs("The crypto engine was unable to encrypt the current message (%Q): %s, no data sent to the server"),
							&buffer,&szEngineError);
					}
					break;
				}
				userAction(connection()->currentNickName(),KVI_USERACTION_PRIVMSG);
				return;
			} else {
				d++; //eat the escape code
				QString tmp = buffer.right(buffer.length() - 1);
				if(!connection()->sendFmtData("PRIVMSG %s :%s",szName.data(),d))return;
				m_pConsole->outputPrivmsg(this,KVI_OUT_OWNPRIVMSG,QString::null,QString::null,QString::null,tmp,KviConsole::NoNotifications);
				userAction(connection()->currentNickName(),KVI_USERACTION_PRIVMSG);
				return;
			}
		}
	}
#endif

	if(!connection()->sendFmtData("PRIVMSG %s :%s",szName.data(),d))return;
	m_pConsole->outputPrivmsg(this,KVI_OUT_OWNPRIVMSG,QString::null,QString::null,QString::null,buffer,KviConsole::NoNotifications);
	userAction(connection()->currentNickName(),KVI_USERACTION_PRIVMSG);
}

void KviQuery::ownAction(const QString &buffer)
{
	if(!connection())
	{
		outputNoFmt(KVI_OUT_SYSTEMWARNING,__tr2qs("This query has no active targets, no message sent"));
	} else {
		KviQCString szBuffer = encodeText(buffer);
		if(!szBuffer.data())return;
		KviQCString sz = connection()->encodeText(windowName());
		if(sz.isEmpty())return;
		if(!connection()->sendFmtData("PRIVMSG %s :%cACTION %s%c",
			sz.data(),0x01,szBuffer.data(),0x01))return;
		output(KVI_OUT_ACTION,"\r!nc\r%Q\r %Q",&(connection()->currentNickName()),&buffer);
		m_pUserListView->userAction(connection()->currentNickName(),KVI_USERACTION_ACTION);
	}
}

#include "kvi_query.moc"
