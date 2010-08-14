//=============================================================================
//
//   File : chat.cpp
//   Creation date : Tue Sep 20 09 2000 15:13:13 by Szymon Stefanek
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "chat.h"
#include "marshal.h"
#include "broker.h"

#ifdef COMPILE_ON_WINDOWS
	// Ugly Windoze compiler...
	#include "dialogs.h"
#endif

#define _KVI_DEBUG_CHECK_RANGE_

#include "kvi_debug.h"
#include "kvi_options.h"
#include "kvi_input.h"
#include "kvi_ircview.h"
#include "kvi_iconmanager.h"
#include "kvi_locale.h"
#include "kvi_error.h"
#include "kvi_out.h"
#include "kvi_netutils.h"
#include "kvi_console.h"
#include "kvi_frame.h"
#include "kvi_malloc.h"
#include "kvi_memmove.h"
#include "kvi_thread.h"
#include "kvi_ircsocket.h"
#include "kvi_settings.h"
#include "kvi_socket.h"
#include "kvi_app.h"
#include "kvi_parameterlist.h"
#include "kvi_ircconnection.h"
#include "kvi_ircconnectionuserinfo.h"
#include "kvi_kvs_eventtriggers.h"
#include "kvi_mirccntrl.h"
#include "kvi_tal_vbox.h"

#include <QEvent>
#include <QResizeEvent>
#include <QByteArray>
#include <QTextDocument> //for Qt::escape

#ifdef COMPILE_CRYPT_SUPPORT
	#include "kvi_crypt.h"
	#include "kvi_cryptcontroller.h"
#endif

#ifdef COMPILE_SSL_SUPPORT
	#include "kvi_sslmaster.h"
#endif

extern KviDccBroker * g_pDccBroker;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////
////// WINDOW
//////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

KviDccChat::KviDccChat(KviFrame *pFrm,KviDccDescriptor * dcc,const char * name)
: KviDccWindow(KVI_WINDOW_TYPE_DCCCHAT,pFrm,name,dcc)
{
	m_pButtonBox = new KviTalHBox(this);

	m_pLabel = new KviThemedLabel(m_pButtonBox, this, "dcc_chat_label");
	m_pLabel->setText(name);
	m_pButtonBox->setStretchFactor(m_pLabel,1);

	m_pButtonContainer= new KviTalHBox(m_pButtonBox);
	createTextEncodingButton(m_pButtonContainer);

#ifdef COMPILE_CRYPT_SUPPORT
	createCryptControllerButton(m_pButtonContainer);
#endif

	m_pSplitter = new KviTalSplitter(Qt::Horizontal,this);
	m_pSplitter->setObjectName("dcc_chat_splitter");
	m_pSplitter->setChildrenCollapsible(false);

	m_pIrcView  = new KviIrcView(m_pSplitter,pFrm,this);
	connect(m_pIrcView,SIGNAL(rightClicked()),this,SLOT(textViewRightClicked()));
	m_pInput    = new KviInput(this);

	//setFocusHandler(m_pInput,this);

	m_pSlaveThread = 0;

	if(KVI_OPTION_BOOL(KviOption_boolAutoLogDccChat))m_pIrcView->startLogging();

	m_pMarshal = new KviDccMarshal(this);
	connect(m_pMarshal,SIGNAL(error(int)),this,SLOT(handleMarshalError(int)));
	connect(m_pMarshal,SIGNAL(connected()),this,SLOT(connected()));
	connect(m_pMarshal,SIGNAL(inProgress()),this,SLOT(connectionInProgress()));
#ifdef COMPILE_SSL_SUPPORT
	connect(m_pMarshal,SIGNAL(startingSSLHandshake()),this,SLOT(startingSSLHandshake()));
	connect(m_pMarshal,SIGNAL(sslError(const char *)),this,SLOT(sslError(const char *)));
#endif

	m_pSlaveThread = 0;

	startConnection();
}

KviDccChat::~KviDccChat()
{
	g_pDccBroker->unregisterDccWindow(this);
	if(m_pSlaveThread)
	{
		m_pSlaveThread->terminate();
		delete m_pSlaveThread;
		m_pSlaveThread = 0;
	}
	KviThreadManager::killPendingEvents(this);
}

void KviDccChat::textViewRightClicked()
{
	KVS_TRIGGER_EVENT_1(KviEvent_OnDCCChatPopupRequest,this,m_pDescriptor->idString());
}

void KviDccChat::triggerCreationEvents()
{
	KVS_TRIGGER_EVENT_1(KviEvent_OnDCCChatWindowCreated,this,m_pDescriptor->idString());
}

void KviDccChat::triggerDestructionEvents()
{
	KVS_TRIGGER_EVENT_1(KviEvent_OnDCCChatWindowClosing,this,m_pDescriptor->idString());
}

void KviDccChat::startConnection()
{
	if(!(m_pDescriptor->bActive))
	{
		// PASSIVE CONNECTION
		output(KVI_OUT_DCCMSG,__tr2qs_ctx("Attempting a passive DCC %s connection","dcc"),m_pDescriptor->szType.toUtf8().data());
#ifdef COMPILE_SSL_SUPPORT
		int ret = m_pMarshal->dccListen(m_pDescriptor->szListenIp,m_pDescriptor->szListenPort,m_pDescriptor->bDoTimeout,m_pDescriptor->bIsSSL);
#else
		int ret = m_pMarshal->dccListen(m_pDescriptor->szListenIp,m_pDescriptor->szListenPort,m_pDescriptor->bDoTimeout);
#endif
		if(ret != KviError_success)handleMarshalError(ret);
	} else {
		// ACTIVE CONNECTION
		output(KVI_OUT_DCCMSG,__tr2qs_ctx("Attempting an active DCC %s connection","dcc"),m_pDescriptor->szType.toUtf8().data());
#ifdef COMPILE_SSL_SUPPORT
		int ret = m_pMarshal->dccConnect(m_pDescriptor->szIp.toUtf8().data(),m_pDescriptor->szPort.toUtf8().data(),m_pDescriptor->bDoTimeout,m_pDescriptor->bIsSSL);
#else
		int ret = m_pMarshal->dccConnect(m_pDescriptor->szIp.toUtf8().data(),m_pDescriptor->szPort.toUtf8().data(),m_pDescriptor->bDoTimeout);
#endif
		if(ret != KviError_success)handleMarshalError(ret);
	}
}

void KviDccChat::connectionInProgress()
{
	if(m_pDescriptor->bActive)
	{
		output(KVI_OUT_DCCMSG,__tr2qs_ctx("Contacting host %Q on port %Q","dcc"),&(m_pDescriptor->szIp),&(m_pDescriptor->szPort));
	} else {
		output(KVI_OUT_DCCMSG,__tr2qs_ctx("Listening on interface %Q port %Q","dcc"),&(m_pMarshal->localIp()),&(m_pMarshal->localPort()));

		if(m_pDescriptor->bSendRequest)
		{
			KviStr ip;
			if(!m_pDescriptor->szFakeIp.isEmpty())
			{
				ip = m_pDescriptor->szFakeIp;
			} else {
				ip = m_pDescriptor->szListenIp;

				if(KVI_OPTION_BOOL(KviOption_boolDccGuessIpFromServerWhenLocalIsUnroutable))
				{
					if(!kvi_isRoutableIpString(ip.ptr()))
					{
						// try to get the IP that the IRC server can see
						if(m_pDescriptor->console())
						{
							KviStr tmp = m_pDescriptor->console()->connection() ? m_pDescriptor->console()->connection()->userInfo()->hostIp().toUtf8().data() : "";
							if(tmp.hasData())
							{
								ip = tmp;
								output(KVI_OUT_DCCMSG,__tr2qs_ctx("The local IP address is private, determining from IRC server: %s","dcc"),ip.ptr());
							} else {
								output(KVI_OUT_DCCMSG,__tr2qs_ctx("The local IP address is private, but unable to determine it from the IRC server","dcc"));
							}
						} else {
							output(KVI_OUT_DCCMSG,__tr2qs_ctx("The local IP address is private, but have no IRC server to determine it from","dcc"));
						}
					}
				}
			}

			QString port   = !m_pDescriptor->szFakePort.isEmpty() ? m_pDescriptor->szFakePort : QString(m_pMarshal->localPort());

			//FIXME: #warning "OPTION FOR SENDING 127.0.0.1 and so on (not an unsigned number)"
			struct in_addr a;
			if(KviNetUtils::stringIpToBinaryIp(ip.ptr(),&a))ip.setNum(htonl(a.s_addr));

			QString szReq = QString("PRIVMSG %1 :%2DCC %3 chat %4 %5").arg(m_pDescriptor->szNick, QChar(0x01), m_pDescriptor->szType, ip.ptr(), port);

			if(m_pDescriptor->isZeroPortRequest())
			{
				szReq.append(" ");
				szReq+=m_pDescriptor->zeroPortRequestTag();
			}
			szReq.append((char)(0x01));

			m_pDescriptor->console()->connection()->sendData(m_pDescriptor->console()->connection()->encodeText(szReq).data());
			output(KVI_OUT_DCCMSG,__tr2qs_ctx("Sent DCC %Q request to %Q, waiting for the remote client to connect...","dcc"),
				&(m_pDescriptor->szType),&(m_pDescriptor->szNick));
			//qDebug(m_pDescriptor->szNick);
		} else output(KVI_OUT_DCCMSG,__tr2qs_ctx("DCC %Q request not sent, awaiting manual connection","dcc"),&(m_pDescriptor->szType));
	}
	KVS_TRIGGER_EVENT_1(KviEvent_OnDCCChatConnectionInProgress,this,m_pDescriptor->idString());
}

void KviDccChat::startingSSLHandshake()
{
#ifdef COMPILE_SSL_SUPPORT
	outputNoFmt(KVI_OUT_SSL,__tr2qs_ctx("Low-level transport connection established","dcc"));
	outputNoFmt(KVI_OUT_SSL,__tr2qs_ctx("Starting Secure Socket Layer handshake","dcc"));
#endif
}

void KviDccChat::sslError(const char * msg)
{
#ifdef COMPILE_SSL_SUPPORT
	if(!KVS_TRIGGER_EVENT_2_HALTED(KviEvent_OnDCCChatError,this,QString(msg),m_pDescriptor->idString()))
		output(KVI_OUT_DCCERROR,__tr2qs_ctx("[SSL ERROR]: %s","dcc"),msg);
#endif
}

const QString & KviDccChat::target()
{
	// This may change on the fly...
	m_szTarget = m_pDescriptor->szNick;
	m_szTarget += "@";
	m_szTarget += m_pDescriptor->szIp;
	m_szTarget += ":";
	m_szTarget += m_pDescriptor->szPort;
	return m_szTarget;
}

void KviDccChat::fillCaptionBuffers()
{
	QString tmp = QString("DCC %1 %2@%3:%4").arg(
#ifdef COMPILE_SSL_SUPPORT
		m_pDescriptor->bIsSSL ? "SChat" : "Chat", 
#else
		"Chat", 
#endif
		m_pDescriptor->szNick, m_pDescriptor->szIp, m_pDescriptor->szPort);

	m_szPlainTextCaption = tmp;
}

QPixmap * KviDccChat::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KVI_SMALLICON_DCCMSG);
}

void KviDccChat::getBaseLogFileName(QString &buffer)
{
	buffer.sprintf("%s_%s_%s",m_pDescriptor->szNick.toUtf8().data(),m_pDescriptor->szIp.toUtf8().data(),m_pDescriptor->szPort.toUtf8().data());
}

void KviDccChat::ownMessage(const QString &text)
{
	if(!m_pSlaveThread)
	{
		output(KVI_OUT_SYSTEMWARNING,__tr2qs_ctx("Cannot send data: No active connection","dcc"));
		return;
	}

	QByteArray szData = encodeText(text);
	const char * d = szData.data();
	if(!d)return;

#ifdef COMPILE_CRYPT_SUPPORT
	if(cryptSessionInfo())
	{
		if(cryptSessionInfo()->m_bDoEncrypt)
		{
			if(*d != KVI_TEXT_CRYPTESCAPE)
			{
				KviStr encrypted;
				cryptSessionInfo()->m_pEngine->setMaxEncryptLen(-1);
				switch(cryptSessionInfo()->m_pEngine->encrypt(d,encrypted))
				{
					case KviCryptEngine::Encrypted:
					{
						KviStr buf(KviStr::Format,"%s\r\n",encrypted.ptr());
						m_pSlaveThread->sendRawData(buf.ptr(),buf.len());
						m_pFrm->firstConsole()->outputPrivmsg(this,KVI_OUT_OWNPRIVMSGCRYPTED,
							m_pDescriptor->szLocalNick.toUtf8().data(),m_pDescriptor->szLocalUser.toUtf8().data(),
							m_pDescriptor->szLocalHost.toUtf8().data(),text,KviConsole::NoNotifications);
					}
					break;
					case KviCryptEngine::Encoded:
					{
						KviStr buf(KviStr::Format,"%s\r\n",encrypted.ptr());
						m_pSlaveThread->sendRawData(buf.ptr(),buf.len());
						QString encr = decodeText(encrypted.ptr());
						m_pFrm->firstConsole()->outputPrivmsg(this,KVI_OUT_OWNPRIVMSG,
							m_pDescriptor->szLocalNick.toUtf8().data(),m_pDescriptor->szLocalUser.toUtf8().data(),
							m_pDescriptor->szLocalHost.toUtf8().data(),encr,KviConsole::NoNotifications);
					}
					break;
					default: // also case KviCryptEngine::EncryptError
					{
						QString szErr = cryptSessionInfo()->m_pEngine->lastError();
						output(KVI_OUT_SYSTEMERROR,
							__tr2qs_ctx("The crypto engine was not able to encrypt the current message (%Q): %Q, no data was sent to the remote end","dcc"),
							&text,&szErr);
					}
					break;
				}
				return;
			} else {
				d++; //eat the escape code
				KviStr buf(KviStr::Format,"%s\r\n",d);
				QString tmp = text.right(text.length() - 1);
				m_pSlaveThread->sendRawData(buf.ptr(),buf.len());
				m_pFrm->firstConsole()->outputPrivmsg(this,KVI_OUT_OWNPRIVMSG,
					m_pDescriptor->szLocalNick.toUtf8().data(),m_pDescriptor->szLocalUser.toUtf8().data(),
					m_pDescriptor->szLocalHost.toUtf8().data(),tmp,KviConsole::NoNotifications);
				return;
			}
		}
	}
#endif
	KviStr buf(KviStr::Format,"%s\r\n",d);
	m_pSlaveThread->sendRawData(buf.ptr(),buf.len());
	m_pFrm->firstConsole()->outputPrivmsg(this,KVI_OUT_OWNPRIVMSG,
		m_pDescriptor->szLocalNick.toUtf8().data(),m_pDescriptor->szLocalUser.toUtf8().data(),
		m_pDescriptor->szLocalHost.toUtf8().data(),text,KviConsole::NoNotifications);
}

const QString & KviDccChat::localNick()
{
	// FIXME: This is just a complete HACK
	m_szLocalNick = m_pDescriptor->szLocalNick;
	return m_szLocalNick;
}

void KviDccChat::ownAction(const QString &text)
{
	if(m_pSlaveThread)
	{
		QString szTmpBuffer;
		//see bug ticket #220
		if(KVI_OPTION_BOOL(KviOption_boolStripMircColorsInUserMessages))
		{
			szTmpBuffer = KviMircCntrl::stripControlBytes(text);
		} else {
			szTmpBuffer = text;
		}

		QByteArray szData = encodeText(szTmpBuffer);
		const char * d = szData.data();
		if(!d)return;
		KviStr buf(KviStr::Format,"%cACTION %s%c\r\n",0x01,d,0x01);
		m_pSlaveThread->sendRawData(buf.ptr(),buf.len());
		output(KVI_OUT_ACTION,"%Q %Q",&(m_pDescriptor->szLocalNick),&szTmpBuffer);
	} else {
		output(KVI_OUT_SYSTEMWARNING,__tr2qs_ctx("Cannot send data: No active connection","dcc"));
	}
}

bool KviDccChat::event(QEvent *e)
{
	if(e->type() == KVI_THREAD_EVENT)
	{
		switch(((KviThreadEvent *)e)->id())
		{
			case KVI_DCC_THREAD_EVENT_ERROR:
			{
				int * err = ((KviThreadDataEvent<int> *)e)->getData();
				QString szErr = KviError::getDescription(*err);
				if(!KVS_TRIGGER_EVENT_2_HALTED(KviEvent_OnDCCChatError,this,szErr,m_pDescriptor->idString()))
					output(KVI_OUT_DCCERROR,__tr2qs_ctx("ERROR: %Q","dcc"),&szErr);
				KVS_TRIGGER_EVENT_1(KviEvent_OnDCCChatDisconnected,this,m_pDescriptor->idString());
				delete err;
				return true;
			}
			break;
			case KVI_DCC_THREAD_EVENT_DATA:
			{
				KviStr * encoded = ((KviThreadDataEvent<KviStr> *)e)->getData();
				KviStr d=KviStr(decodeText(encoded->ptr()));
				if(d.firstCharIs(0x01))
				{
					d.cutLeft(1);
					if(d.lastCharIs(0x01))d.cutRight(1);
					if(kvi_strEqualCIN("ACTION",d.ptr(),6))d.cutLeft(6);
					d.stripLeftWhiteSpace();
					output(KVI_OUT_ACTION,"%Q %s",&(m_pDescriptor->szNick),d.ptr());
					if(!hasAttention())
					{
						if(KVI_OPTION_BOOL(KviOption_boolFlashDccChatWindowOnNewMessages))
						{
							demandAttention();
						}
						if(KVI_OPTION_BOOL(KviOption_boolPopupNotifierOnNewDccChatMessages))
						{
							QString szMsg = "<b>";
							szMsg += m_pDescriptor->szNick;
							szMsg += "</b> ";
							szMsg += Qt::escape(QString(d.ptr()));
							//debug("kvi_sp_ctcp.cpp:975 debug: %s",szMsg.data());
							g_pApp->notifierMessage(this,KVI_OPTION_MSGTYPE(KVI_OUT_ACTION).pixId(),szMsg,KVI_OPTION_UINT(KviOption_uintNotifierAutoHideTime));
						}
					}
				} else {

#ifdef COMPILE_CRYPT_SUPPORT
					if(KviCryptSessionInfo * cinf = cryptSessionInfo())
					{
						if(cinf->m_bDoDecrypt)
						{
							KviStr decryptedStuff;
							switch(cinf->m_pEngine->decrypt(d.ptr(),decryptedStuff))
							{
								case KviCryptEngine::DecryptOkWasEncrypted:
								case KviCryptEngine::DecryptOkWasEncoded:
								case KviCryptEngine::DecryptOkWasPlainText:
									if(!KVS_TRIGGER_EVENT_2_HALTED(KviEvent_OnDCCChatMessage,this,QString(decryptedStuff.ptr()),m_pDescriptor->idString()))
									{
										m_pFrm->firstConsole()->outputPrivmsg(this,KVI_OUT_DCCCHATMSG,
											m_pDescriptor->szNick.toUtf8().data(),m_pDescriptor->szUser.toUtf8().data(),
											m_pDescriptor->szHost.toUtf8().data(),decryptedStuff.ptr());
									}
									delete encoded;
									return true;
								break;

								default: // also case KviCryptEngine::DecryptError
								{
									QString szErr = cinf->m_pEngine->lastError();
									output(KVI_OUT_SYSTEMERROR,
										__tr2qs_ctx("The following message appears to be encrypted, but the crypto engine failed to decode it: %Q","dcc"),
										&szErr);
								}
								break;
							}
						}
					} else {
#endif
						// FIXME!
						if(!KVS_TRIGGER_EVENT_2_HALTED(KviEvent_OnDCCChatMessage,this,QString(d.ptr()),m_pDescriptor->idString()))
						{
							m_pFrm->firstConsole()->outputPrivmsg(this,KVI_OUT_DCCCHATMSG,
								m_pDescriptor->szNick.toUtf8().data(),m_pDescriptor->szUser.toUtf8().data(),
								m_pDescriptor->szHost.toUtf8().data(),d.ptr());
							if(!hasAttention())
							{
								if(KVI_OPTION_BOOL(KviOption_boolFlashDccChatWindowOnNewMessages))
								{
									demandAttention();
								}
								if(KVI_OPTION_BOOL(KviOption_boolPopupNotifierOnNewDccChatMessages))
								{
									QString szMsg = Qt::escape(QString(d.ptr()));
									g_pApp->notifierMessage(this,KVI_SMALLICON_DCCCHATMSG,szMsg,KVI_OPTION_UINT(KviOption_uintNotifierAutoHideTime));
								}
							}
						}
#ifdef COMPILE_CRYPT_SUPPORT
					}
#endif
				}
				delete encoded;
				return true;
			}
			break;
		}
	}
	return KviWindow::event(e);
}

void KviDccChat::resizeEvent(QResizeEvent *)
{
	int hght = m_pInput->heightHint();
	int hght2 = m_pButtonBox->sizeHint().height();
	m_pButtonBox->setGeometry(0,0,width(),hght2);
	m_pSplitter->setGeometry(0,hght2,width(),height() - (hght + hght2));
	m_pInput->setGeometry(0,height() - hght,width(),hght);
}

QSize KviDccChat::sizeHint() const
{
	QSize ret(m_pIrcView->sizeHint().width(),
		m_pIrcView->sizeHint().height() + m_pInput->heightHint());
	return ret;
}

void KviDccChat::handleMarshalError(int err)
{
	QString szErr = KviError::getDescription(err);
	if(!KVS_TRIGGER_EVENT_2_HALTED(KviEvent_OnDCCChatError,this,szErr,m_pDescriptor->idString()))
		output(KVI_OUT_DCCERROR,__tr2qs_ctx("DCC %Q failed: %Q","dcc"),&(m_pDescriptor->szType),&szErr);
}

void KviDccChat::connected()
{
	if(!(m_pDescriptor->bActive))
	{
		// PASSIVE CONNECTION...Find out the remote end
		m_pDescriptor->szIp   = m_pMarshal->remoteIp();
		m_pDescriptor->szPort = m_pMarshal->remotePort();
		m_pDescriptor->szHost = m_pMarshal->remoteIp();
	}

	updateCaption();

	m_pSlaveThread = new KviDccChatThread(this,m_pMarshal->releaseSocket());
	
#ifdef COMPILE_SSL_SUPPORT
	KviSSL * s = m_pMarshal->releaseSSL();
	if(s)
	{
		KviSSLMaster::printSSLConnectionInfo(this,s);
		m_pSlaveThread->setSSL(s);
	}
#endif
	m_pSlaveThread->start();

	if(!KVS_TRIGGER_EVENT_1_HALTED(KviEvent_OnDCCChatConnected,this,m_pDescriptor->idString()))
	{
		output(KVI_OUT_DCCMSG,__tr2qs_ctx("Connected to %Q:%Q","dcc"),
			&(m_pMarshal->remoteIp()),&(m_pMarshal->remotePort()));
		output(KVI_OUT_DCCMSG,__tr2qs_ctx("Local end is %Q:%Q","dcc"),
			&(m_pMarshal->localIp()),&(m_pMarshal->localPort()));
#ifdef COMPILE_SSL_SUPPORT
		QString tmp = QString("dcc: %1 %2@%3:%4").arg(m_pDescriptor->bIsSSL ? "SChat" : "Chat", m_pDescriptor->szNick, m_pDescriptor->szIp, m_pDescriptor->szPort);
#else
		QString tmp = QString("dcc: %1 %2@%3:%4").arg("Chat", m_pDescriptor->szNick, m_pDescriptor->szIp, m_pDescriptor->szPort);
#endif
		m_pLabel->setText(tmp);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////
////// THREAD
//////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


KviDccChatThread::KviDccChatThread(KviWindow *wnd,kvi_socket_t fd)
: KviDccThread(wnd,fd)
{
	m_pOutBuffers = new KviPointerList<KviDataBuffer>;
	m_pOutBuffers->setAutoDelete(true);
}

KviDccChatThread::~KviDccChatThread()
{
	if(m_pOutBuffers)delete m_pOutBuffers;
}

void KviDccChatThread::run()
{
	KviDccThreadIncomingData data;
	data.iLen   = 0;
	data.buffer = 0;

	for(;;)
	{
		// Dequeue events
		while(KviThreadEvent * e = dequeueEvent())
		{
			if(e->id() == KVI_THREAD_EVENT_TERMINATE)
			{
				delete e;
				goto out_of_the_loop;
			} else {
				// Other events are senseless to us
				delete e;
			}
		}

		bool bCanRead;
		bool bCanWrite;
		if(kvi_select(m_fd,&bCanRead,&bCanWrite))
		{
			if(bCanWrite)
			{
				if(!tryFlushOutBuffers())goto out_of_the_loop;
			}
			if(bCanRead)
			{
				data.buffer = (char *) kvi_realloc(data.buffer,(data.iLen + 512) * sizeof(char));
				int readLen;
#ifdef COMPILE_SSL_SUPPORT
				if(m_pSSL)
				{
					readLen = m_pSSL->read(data.buffer + data.iLen,512);
				} else {
#endif
					readLen = kvi_socket_recv(m_fd,data.buffer + data.iLen,512);
#ifdef COMPILE_SSL_SUPPORT
				}
#endif
				if(readLen > 0)
				{
					data.iLen += readLen;
					data.buffer = (char *)kvi_realloc(data.buffer,data.iLen * sizeof(char));
					if(!handleIncomingData(&data,false))break; // non critical...
				} else {

#ifdef COMPILE_SSL_SUPPORT
					if(m_pSSL)
					{
						// ssl error....?
						switch(m_pSSL->getProtocolError(readLen))
						{
							case KviSSL::ZeroReturn:
								readLen = 0;
							break;
							case KviSSL::WantRead:
							case KviSSL::WantWrite:
								// hmmm...
							break;
							case KviSSL::SyscallError:
							{
								int iE = m_pSSL->getLastError(true);
								if(iE != 0)
								{
									raiseSSLError();
									postErrorEvent(KviError_SSLError);
									goto out_of_the_loop;
								}
							}
							break;
							case KviSSL::SSLError:
							{
								raiseSSLError();
								postErrorEvent(KviError_SSLError);
								goto out_of_the_loop;
							}
							break;
							default:
								// Raise unknown SSL ERROR
								postErrorEvent(KviError_SSLError);
								goto out_of_the_loop;
							break;
						}
					}
#endif
					if(data.iLen > 0)
					{
						data.buffer = (char *)kvi_realloc(data.buffer,data.iLen * sizeof(char));
					} else {
						kvi_free(data.buffer);
						data.buffer = 0;
					}

					if(!handleInvalidSocketRead(readLen))
					{
						if(data.iLen)handleIncomingData(&data,true); // critical
						__range_invalid(data.iLen);
						break; // error
					}
				}
			}
			msleep(100);
		}
	}

out_of_the_loop:


	if(data.iLen)kvi_free(data.buffer);

#ifdef COMPILE_SSL_SUPPORT
	if(m_pSSL)
	{
		KviSSLMaster::freeSSL(m_pSSL);
		m_pSSL = 0;
	}
#endif

	if(m_fd != KVI_INVALID_SOCKET)::kvi_socket_close(m_fd);
	m_fd = KVI_INVALID_SOCKET;
}

bool KviDccChatThread::handleIncomingData(KviDccThreadIncomingData * data,bool bCritical)
{
	__range_valid(data->iLen);
	__range_valid(data->buffer);
	char * aux = data->buffer;
	char * end = data->buffer + data->iLen;
	while(aux != end)
	{
		if((*aux == '\n') || (*aux == '\0'))
		{
			KviThreadDataEvent<KviStr> * e = new KviThreadDataEvent<KviStr>(KVI_DCC_THREAD_EVENT_DATA);
			// The left part is len chars long
			int len = aux - data->buffer;
//			debug("LEN = %d, iLen = %d",len,data->iLen);
//#warning "DO IT BETTER (the \r cutting)"
			KviStr * s = new KviStr(data->buffer,len);
			if(s->lastCharIs('\r'))s->cutRight(1);
			e->setData(s);
			// but we cut also \n (or \0)
			++aux;
			// so len += 1; --> new data->iLen -= len;
			data->iLen -= (len + 1);
//			debug("iLen now = %d",data->iLen);
			__range_valid(data->iLen >= 0);
			if(data->iLen > 0)
			{
				// memmove the remaining part to the beginning
				// aux points after \n or \0
				kvi_memmove(data->buffer,aux,data->iLen);
				data->buffer = (char *)kvi_realloc(data->buffer,data->iLen);
				end = data->buffer + data->iLen;
				aux = data->buffer;
			} else {
				// no more data in the buffer
				__range_valid(data->iLen == 0);
				kvi_free(data->buffer);
				data->buffer = end = aux = 0;
			}
			postEvent(parent(),e);
		} else aux++;
//		debug("PASSING CHAR %c",*aux);
	}
	// now aux == end
	if(bCritical)
	{
		// need to flush everything...
		if(data->iLen > 0)
		{
			// in the last part there are no NULL and \n chars
			KviThreadDataEvent<KviStr> * e = new KviThreadDataEvent<KviStr>(KVI_DCC_THREAD_EVENT_DATA);
			KviStr * s = new KviStr(data->buffer,data->iLen);
			if(s->lastCharIs('\r'))s->cutRight(1);
			e->setData(s);
			data->iLen = 0;
			kvi_free(data->buffer);
			data->buffer = 0;
			postEvent(parent(),e);
		}
	}
	return true;
}

void KviDccChatThread::sendRawData(const void * buffer,int len)
{
	m_pMutex->lock();
	m_pOutBuffers->append(new KviDataBuffer((unsigned int)len,(const unsigned char *)buffer));
	m_pMutex->unlock();
}

bool KviDccChatThread::tryFlushOutBuffers()
{
	bool bRet = true;
	m_pMutex->lock();
	while(KviDataBuffer * b = m_pOutBuffers->first())
	{
		int sentLen;
#ifdef COMPILE_SSL_SUPPORT
		if(m_pSSL)
		{
			sentLen = m_pSSL->write((const char *)b->data(),b->size());
		} else {
#endif
			sentLen = kvi_socket_send(m_fd,b->data(),b->size());
#ifdef COMPILE_SSL_SUPPORT
		}
#endif
		if(sentLen > 0)
		{
			if(sentLen == b->size())m_pOutBuffers->removeFirst();
			else {
				// just a part
				b->remove((unsigned int)sentLen);
				break;
			}
		} else {
#ifdef COMPILE_SSL_SUPPORT
			if(m_pSSL)
			{
				// ops...might be an SSL error
				switch(m_pSSL->getProtocolError(sentLen))
				{
					case KviSSL::WantWrite:
					case KviSSL::WantRead:
						// Async continue...
						goto handle_system_error;
					break;
					case KviSSL::SyscallError:
						if(sentLen == 0)
						{
							raiseSSLError();
							postErrorEvent(KviError_remoteEndClosedConnection);
							bRet = false;
							goto out_of_the_loop;
						} else {
							int iSSLErr = m_pSSL->getLastError(true);
							if(iSSLErr != 0)
							{
								raiseSSLError();
								postErrorEvent(KviError_SSLError);
								bRet = false;
								goto out_of_the_loop;
							} else {
								goto handle_system_error;
							}
						}
					break;
					case KviSSL::SSLError:
						raiseSSLError();
						postErrorEvent(KviError_SSLError);
						bRet = false;
						goto out_of_the_loop;
					break;
					default:
						postErrorEvent(KviError_SSLError);
						bRet = false;
						goto out_of_the_loop;
					break;
				}
			}
#endif
handle_system_error:
			if(sentLen < 0)
			{
				int err = kvi_socket_error();
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
				if((err != EAGAIN) || (err != EINTR) || (err != WSAEWOULDBLOCK))
#else
				if((err != EAGAIN)||(err != EINTR))
#endif
				{
					postErrorEvent(KviError::translateSystemError(err));
					bRet = false;
					goto out_of_the_loop;
				}
			}
			break; // send error
		}
	}
out_of_the_loop:
	m_pMutex->unlock();
	return bRet;
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_chat.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
