//=============================================================================
//
//   File : DccChatWindow.cpp
//   Creation date : Tue Sep 20 09 2000 15:13:13 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "DccChatWindow.h"
#include "DccMarshal.h"
#include "DccBroker.h"

#ifdef COMPILE_ON_WINDOWS
// Ugly Windoze compiler...
#include "DccDialog.h"
#endif

#include "kvi_debug.h"
#include "KviOptions.h"
#include "KviInput.h"
#include "KviIrcView.h"
#include "KviIconManager.h"
#include "KviLocale.h"
#include "kvi_out.h"
#include "KviNetUtils.h"
#include "KviConsoleWindow.h"
#include "KviMainWindow.h"
#include "KviMemory.h"
#include "KviThread.h"
#include "KviIrcSocket.h"
#include "kvi_settings.h"
#include "kvi_socket.h"
#include "KviApplication.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionUserInfo.h"
#include "KviKvsEventTriggers.h"
#include "KviControlCodes.h"
#include "KviTalVBox.h"

#include <QEvent>
#include <QResizeEvent>
#include <QByteArray>

#ifdef COMPILE_CRYPT_SUPPORT
#include "KviCryptEngine.h"
#include "KviCryptController.h"
#endif

#ifdef COMPILE_SSL_SUPPORT
#include "KviSSLMaster.h"
#endif

extern DccBroker * g_pDccBroker;

//
// WINDOW
//

DccChatWindow::DccChatWindow(DccDescriptor * dcc, const char * name)
    : DccWindow(KviWindow::DccChat, name, dcc)
{
	m_pButtonBox = new KviTalHBox(this);

	m_pLabel = new KviThemedLabel(m_pButtonBox, this, "dcc_chat_label");
	m_pLabel->setText(name);
	m_pButtonBox->setStretchFactor(m_pLabel, 1);

	m_pButtonContainer = new KviTalHBox(m_pButtonBox);
	createTextEncodingButton(m_pButtonContainer);

#ifdef COMPILE_CRYPT_SUPPORT
	createCryptControllerButton(m_pButtonContainer);
#endif

	m_pSplitter = new KviTalSplitter(Qt::Horizontal, this);
	m_pSplitter->setObjectName("dcc_chat_splitter");
	m_pSplitter->setChildrenCollapsible(false);

	m_pIrcView = new KviIrcView(m_pSplitter, this);
	connect(m_pIrcView, SIGNAL(rightClicked()), this, SLOT(textViewRightClicked()));
	m_pInput = new KviInput(this);

	//setFocusHandler(m_pInput,this);

	m_pSlaveThread = nullptr;

	if(KVI_OPTION_BOOL(KviOption_boolAutoLogDccChat))
		m_pIrcView->startLogging();

	m_pMarshal = new DccMarshal(this);
	connect(m_pMarshal, SIGNAL(error(KviError::Code)), this, SLOT(handleMarshalError(KviError::Code)));
	connect(m_pMarshal, SIGNAL(connected()), this, SLOT(connected()));
	connect(m_pMarshal, SIGNAL(inProgress()), this, SLOT(connectionInProgress()));
#ifdef COMPILE_SSL_SUPPORT
	connect(m_pMarshal, SIGNAL(startingSSLHandshake()), this, SLOT(startingSSLHandshake()));
	connect(m_pMarshal, SIGNAL(sslError(const char *)), this, SLOT(sslError(const char *)));
#endif

	m_pSlaveThread = nullptr;

	startConnection();
}

DccChatWindow::~DccChatWindow()
{
	g_pDccBroker->unregisterDccWindow(this);
	if(m_pSlaveThread)
	{
		m_pSlaveThread->terminate();
		delete m_pSlaveThread;
		m_pSlaveThread = nullptr;
	}
	KviThreadManager::killPendingEvents(this);
}

void DccChatWindow::textViewRightClicked()
{
	KVS_TRIGGER_EVENT_1(KviEvent_OnDCCChatPopupRequest, this, m_pDescriptor->idString());
}

void DccChatWindow::triggerCreationEvents()
{
	KVS_TRIGGER_EVENT_1(KviEvent_OnDCCChatWindowCreated, this, m_pDescriptor->idString());
}

void DccChatWindow::triggerDestructionEvents()
{
	KVS_TRIGGER_EVENT_1(KviEvent_OnDCCChatWindowClosing, this, m_pDescriptor->idString());
}

void DccChatWindow::startConnection()
{
	if(!(m_pDescriptor->bActive))
	{
		// PASSIVE CONNECTION
		output(KVI_OUT_DCCMSG, __tr2qs_ctx("Attempting a passive DCC %s connection", "dcc"), m_pDescriptor->szType.toUtf8().data());
#ifdef COMPILE_SSL_SUPPORT
		KviError::Code eError = m_pMarshal->dccListen(m_pDescriptor->szListenIp, m_pDescriptor->szListenPort, m_pDescriptor->bDoTimeout, m_pDescriptor->bIsSSL);
#else
		KviError::Code eError = m_pMarshal->dccListen(m_pDescriptor->szListenIp, m_pDescriptor->szListenPort, m_pDescriptor->bDoTimeout);
#endif
		if(eError != KviError::Success)
			handleMarshalError(eError);
	}
	else
	{
		// ACTIVE CONNECTION
		output(KVI_OUT_DCCMSG, __tr2qs_ctx("Attempting an active DCC %s connection", "dcc"), m_pDescriptor->szType.toUtf8().data());
#ifdef COMPILE_SSL_SUPPORT
		KviError::Code eError = m_pMarshal->dccConnect(m_pDescriptor->szIp.toUtf8().data(), m_pDescriptor->szPort.toUtf8().data(), m_pDescriptor->bDoTimeout, m_pDescriptor->bIsSSL);
#else
		KviError::Code eError = m_pMarshal->dccConnect(m_pDescriptor->szIp.toUtf8().data(), m_pDescriptor->szPort.toUtf8().data(), m_pDescriptor->bDoTimeout);
#endif
		if(eError != KviError::Success)
			handleMarshalError(eError);
	}
}

void DccChatWindow::connectionInProgress()
{
	if(m_pDescriptor->bActive)
	{
		output(KVI_OUT_DCCMSG, __tr2qs_ctx("Contacting host %Q on port %Q", "dcc"), &(m_pDescriptor->szIp), &(m_pDescriptor->szPort));
	}
	else
	{
		output(KVI_OUT_DCCMSG, __tr2qs_ctx("Listening on interface %Q port %Q", "dcc"), &(m_pMarshal->localIp()), &(m_pMarshal->localPort()));

		if(m_pDescriptor->bSendRequest)
		{
			KviCString ip;
			if(!m_pDescriptor->szFakeIp.isEmpty())
			{
				ip = m_pDescriptor->szFakeIp;
			}
			else
			{
				ip = m_pDescriptor->szListenIp;

				if(KVI_OPTION_BOOL(KviOption_boolDccGuessIpFromServerWhenLocalIsUnroutable))
				{
					if(!kvi_isRoutableIpString(ip.ptr()))
					{
						// try to get the IP that the IRC server can see
						if(m_pDescriptor->console())
						{
							KviCString tmp = m_pDescriptor->console()->connection() ? m_pDescriptor->console()->connection()->userInfo()->hostIp().toUtf8().data() : "";
							if(tmp.hasData())
							{
								ip = tmp;
								output(KVI_OUT_DCCMSG, __tr2qs_ctx("The local IP address is private, determining from IRC server: %s", "dcc"), ip.ptr());
							}
							else
							{
								output(KVI_OUT_DCCMSG, __tr2qs_ctx("The local IP address is private, but unable to determine it from the IRC server", "dcc"));
							}
						}
						else
						{
							output(KVI_OUT_DCCMSG, __tr2qs_ctx("The local IP address is private, but have no IRC server to determine it from", "dcc"));
						}
					}
				}
			}

			QString port = !m_pDescriptor->szFakePort.isEmpty() ? m_pDescriptor->szFakePort : QString(m_pMarshal->localPort());

			//FIXME: #warning "OPTION FOR SENDING 127.0.0.1 and so on (not an unsigned number)"
			struct in_addr a;
			if(KviNetUtils::stringIpToBinaryIp(ip.ptr(), &a))
				ip.setNum(htonl(a.s_addr));

			QString szReq = QString("PRIVMSG %1 :%2DCC %3 chat %4 %5").arg(m_pDescriptor->szNick, QChar(0x01), m_pDescriptor->szType, ip.ptr(), port);

			if(m_pDescriptor->isZeroPortRequest())
			{
				szReq.append(" ");
				szReq += m_pDescriptor->zeroPortRequestTag();
			}
			szReq.append((char)(0x01));

			m_pDescriptor->console()->connection()->sendData(m_pDescriptor->console()->connection()->encodeText(szReq).data());
			output(KVI_OUT_DCCMSG, __tr2qs_ctx("Sent DCC %Q request to %Q, waiting for the remote client to connect...", "dcc"),
			    &(m_pDescriptor->szType), &(m_pDescriptor->szNick));
			//qDebug(m_pDescriptor->szNick);
		}
		else
			output(KVI_OUT_DCCMSG, __tr2qs_ctx("DCC %Q request not sent, awaiting manual connection", "dcc"), &(m_pDescriptor->szType));
	}
	KVS_TRIGGER_EVENT_1(KviEvent_OnDCCChatConnectionInProgress, this, m_pDescriptor->idString());
}

void DccChatWindow::startingSSLHandshake()
{
#ifdef COMPILE_SSL_SUPPORT
	outputNoFmt(KVI_OUT_SSL, __tr2qs_ctx("Low-level transport connection established", "dcc"));
	outputNoFmt(KVI_OUT_SSL, __tr2qs_ctx("Starting Secure Socket Layer handshake", "dcc"));
#endif
}

void DccChatWindow::sslError(const char * msg)
{
#ifdef COMPILE_SSL_SUPPORT
	if(!KVS_TRIGGER_EVENT_2_HALTED(KviEvent_OnDCCChatError, this, QString(msg), m_pDescriptor->idString()))
		output(KVI_OUT_DCCERROR, __tr2qs_ctx("[SSL ERROR]: %s", "dcc"), msg);
#endif
}

const QString & DccChatWindow::target()
{
	// This may change on the fly...
	m_szTarget = m_pDescriptor->szNick;
	m_szTarget += "@";
	m_szTarget += m_pDescriptor->szIp;
	m_szTarget += ":";
	m_szTarget += m_pDescriptor->szPort;
	return m_szTarget;
}

void DccChatWindow::fillCaptionBuffers()
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

QPixmap * DccChatWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KviIconManager::DccMsg);
}

void DccChatWindow::getBaseLogFileName(QString & buffer)
{
	buffer.sprintf("%s_%s_%s", m_pDescriptor->szNick.toUtf8().data(), m_pDescriptor->szIp.toUtf8().data(), m_pDescriptor->szPort.toUtf8().data());
}

void DccChatWindow::ownMessage(const QString & text, bool bUserFeedback)
{
	if(!m_pSlaveThread)
	{
		output(KVI_OUT_SYSTEMWARNING, __tr2qs_ctx("Can't send data: no active connection", "dcc"));
		return;
	}

	QByteArray szData = encodeText(text);
	const char * d = szData.data();
	if(!d)
		return;

#ifdef COMPILE_CRYPT_SUPPORT
	if(cryptSessionInfo())
	{
		if(cryptSessionInfo()->m_bDoEncrypt)
		{
			if(*d != KviControlCodes::CryptEscape)
			{
				KviCString encrypted;
				cryptSessionInfo()->m_pEngine->setMaxEncryptLen(-1);
				switch(cryptSessionInfo()->m_pEngine->encrypt(d, encrypted))
				{
					case KviCryptEngine::Encrypted:
					{
						KviCString buf(KviCString::Format, "%s\r\n", encrypted.ptr());
						m_pSlaveThread->sendRawData(buf.ptr(), buf.len());
						if(bUserFeedback)
							g_pMainWindow->firstConsole()->outputPrivmsg(this, KVI_OUT_OWNPRIVMSGCRYPTED,
							    m_pDescriptor->szLocalNick.toUtf8().data(), m_pDescriptor->szLocalUser.toUtf8().data(),
							    m_pDescriptor->szLocalHost.toUtf8().data(), text, KviConsoleWindow::NoNotifications);
					}
					break;
					case KviCryptEngine::Encoded:
					{
						KviCString buf(KviCString::Format, "%s\r\n", encrypted.ptr());
						m_pSlaveThread->sendRawData(buf.ptr(), buf.len());
						if(bUserFeedback)
						{
							QString encr = decodeText(encrypted.ptr());
							g_pMainWindow->firstConsole()->outputPrivmsg(this, KVI_OUT_OWNPRIVMSG,
							    m_pDescriptor->szLocalNick.toUtf8().data(), m_pDescriptor->szLocalUser.toUtf8().data(),
							    m_pDescriptor->szLocalHost.toUtf8().data(), encr, KviConsoleWindow::NoNotifications);
						}
					}
					break;
					default: // also case KviCryptEngine::EncryptError
					{
						QString szErr = cryptSessionInfo()->m_pEngine->lastError();
						output(KVI_OUT_SYSTEMERROR,
						    __tr2qs_ctx("The encryption engine was not able to encrypt the current message (%Q): %Q, no data was sent to the remote end", "dcc"),
						    &text, &szErr);
					}
					break;
				}
				return;
			}
			else
			{
				d++; //eat the escape code
				KviCString buf(KviCString::Format, "%s\r\n", d);
				QString tmp = text.right(text.length() - 1);
				m_pSlaveThread->sendRawData(buf.ptr(), buf.len());

				if(bUserFeedback)
					g_pMainWindow->firstConsole()->outputPrivmsg(this, KVI_OUT_OWNPRIVMSG,
					    m_pDescriptor->szLocalNick.toUtf8().data(), m_pDescriptor->szLocalUser.toUtf8().data(),
					    m_pDescriptor->szLocalHost.toUtf8().data(), tmp, KviConsoleWindow::NoNotifications);
				return;
			}
		}
	}
#endif
	KviCString buf(KviCString::Format, "%s\r\n", d);
	m_pSlaveThread->sendRawData(buf.ptr(), buf.len());

	if(bUserFeedback)
		g_pMainWindow->firstConsole()->outputPrivmsg(this, KVI_OUT_OWNPRIVMSG,
		    m_pDescriptor->szLocalNick.toUtf8().data(), m_pDescriptor->szLocalUser.toUtf8().data(),
		    m_pDescriptor->szLocalHost.toUtf8().data(), text, KviConsoleWindow::NoNotifications);
}

const QString & DccChatWindow::localNick()
{
	// FIXME: This is just a complete HACK
	m_szLocalNick = m_pDescriptor->szLocalNick;
	return m_szLocalNick;
}

void DccChatWindow::ownAction(const QString & text)
{
	if(m_pSlaveThread)
	{
		QString szTmpBuffer;
		//see bug ticket #220
		if(KVI_OPTION_BOOL(KviOption_boolStripMircColorsInUserMessages))
		{
			szTmpBuffer = KviControlCodes::stripControlBytes(text);
		}
		else
		{
			szTmpBuffer = text;
		}

		QByteArray szData = encodeText(szTmpBuffer);
		const char * d = szData.data();
		if(!d)
			return;
		KviCString buf(KviCString::Format, "%cACTION %s%c\r\n", 0x01, d, 0x01);
		m_pSlaveThread->sendRawData(buf.ptr(), buf.len());
		output(KVI_OUT_OWNACTION, "%Q %Q", &(m_pDescriptor->szLocalNick), &szTmpBuffer);
	}
	else
	{
		output(KVI_OUT_SYSTEMWARNING, __tr2qs_ctx("Can't send data: no active connection", "dcc"));
	}
}

bool DccChatWindow::event(QEvent * e)
{
	if(e->type() == KVI_THREAD_EVENT)
	{
		switch(((KviThreadEvent *)e)->id())
		{
			case KVI_DCC_THREAD_EVENT_ERROR:
			{
				KviError::Code * pError = ((KviThreadDataEvent<KviError::Code> *)e)->getData();
				QString szErr = KviError::getDescription(*pError);
				if(!KVS_TRIGGER_EVENT_2_HALTED(KviEvent_OnDCCChatError, this, szErr, m_pDescriptor->idString()))
					output(KVI_OUT_DCCERROR, __tr2qs_ctx("ERROR: %Q", "dcc"), &szErr);
				KVS_TRIGGER_EVENT_1(KviEvent_OnDCCChatDisconnected, this, m_pDescriptor->idString());
				delete pError;
				return true;
			}
			break;
			case KVI_DCC_THREAD_EVENT_DATA:
			{
				KviCString * encoded = ((KviThreadDataEvent<KviCString> *)e)->getData();
				KviCString d = KviCString(decodeText(encoded->ptr()));
				if(d.firstCharIs(0x01))
				{
					d.cutLeft(1);
					if(d.lastCharIs(0x01))
						d.cutRight(1);
					if(kvi_strEqualCIN("ACTION", d.ptr(), 6))
						d.cutLeft(6);
					d.stripLeftWhiteSpace();
					output(KVI_OUT_ACTION, "%Q %s", &(m_pDescriptor->szNick), d.ptr());
					if(!hasAttention(KviWindow::MainWindowIsVisible))
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
							szMsg += KviQString::toHtmlEscaped(QString(d.ptr()));
							//qDebug("KviIrcServerParser_ctcp.cpp:975 debug: %s",szMsg.data());
							g_pApp->notifierMessage(this, KVI_OPTION_MSGTYPE(KVI_OUT_ACTION).pixId(), szMsg, KVI_OPTION_UINT(KviOption_uintNotifierAutoHideTime));
						}
					}
				}
				else
				{

#ifdef COMPILE_CRYPT_SUPPORT
					if(KviCryptSessionInfo * cinf = cryptSessionInfo())
					{
						if(cinf->m_bDoDecrypt)
						{
							KviCString decryptedStuff;
							switch(cinf->m_pEngine->decrypt(d.ptr(), decryptedStuff))
							{
								case KviCryptEngine::DecryptOkWasEncrypted:
								case KviCryptEngine::DecryptOkWasEncoded:
								case KviCryptEngine::DecryptOkWasPlainText:
									if(!KVS_TRIGGER_EVENT_2_HALTED(KviEvent_OnDCCChatMessage, this, QString(decryptedStuff.ptr()), m_pDescriptor->idString()))
									{
										g_pMainWindow->firstConsole()->outputPrivmsg(this, KVI_OUT_DCCCHATMSG,
										    m_pDescriptor->szNick.toUtf8().data(), m_pDescriptor->szUser.toUtf8().data(),
										    m_pDescriptor->szHost.toUtf8().data(), decryptedStuff.ptr());
									}
									delete encoded;
									return true;
									break;

								default: // also case KviCryptEngine::DecryptError
								{
									QString szErr = cinf->m_pEngine->lastError();
									output(KVI_OUT_SYSTEMERROR,
									    __tr2qs_ctx("The following message appears to be encrypted, but the encryption engine failed to decode it: %Q", "dcc"),
									    &szErr);
								}
								break;
							}
						}
					}
					else
					{
#endif
						// FIXME!
						if(!KVS_TRIGGER_EVENT_2_HALTED(KviEvent_OnDCCChatMessage, this, QString(d.ptr()), m_pDescriptor->idString()))
						{
							g_pMainWindow->firstConsole()->outputPrivmsg(this, KVI_OUT_DCCCHATMSG,
							    m_pDescriptor->szNick.toUtf8().data(), m_pDescriptor->szUser.toUtf8().data(),
							    m_pDescriptor->szHost.toUtf8().data(), d.ptr());
							if(!hasAttention(KviWindow::MainWindowIsVisible))
							{
								if(KVI_OPTION_BOOL(KviOption_boolFlashDccChatWindowOnNewMessages))
								{
									demandAttention();
								}
								if(KVI_OPTION_BOOL(KviOption_boolPopupNotifierOnNewDccChatMessages))
								{
									QString szMsg = KviQString::toHtmlEscaped(QString(d.ptr()));
									g_pApp->notifierMessage(this, KviIconManager::DccChatMsg, szMsg, KVI_OPTION_UINT(KviOption_uintNotifierAutoHideTime));
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

void DccChatWindow::resizeEvent(QResizeEvent *)
{
	int hght = m_pInput->heightHint();
	int hght2 = m_pButtonBox->sizeHint().height();
	m_pButtonBox->setGeometry(0, 0, width(), hght2);
	m_pSplitter->setGeometry(0, hght2, width(), height() - (hght + hght2));
	m_pInput->setGeometry(0, height() - hght, width(), hght);
}

QSize DccChatWindow::sizeHint() const
{
	QSize ret(m_pIrcView->sizeHint().width(),
	    m_pIrcView->sizeHint().height() + m_pInput->heightHint());
	return ret;
}

void DccChatWindow::handleMarshalError(KviError::Code eError)
{
	QString szErr = KviError::getDescription(eError);
	if(!KVS_TRIGGER_EVENT_2_HALTED(KviEvent_OnDCCChatError, this, szErr, m_pDescriptor->idString()))
		output(KVI_OUT_DCCERROR, __tr2qs_ctx("DCC %Q failed: %Q", "dcc"), &(m_pDescriptor->szType), &szErr);
}

void DccChatWindow::connected()
{
	if(!(m_pDescriptor->bActive))
	{
		// PASSIVE CONNECTION...Find out the remote end
		m_pDescriptor->szIp = m_pMarshal->remoteIp();
		m_pDescriptor->szPort = m_pMarshal->remotePort();
		m_pDescriptor->szHost = m_pMarshal->remoteIp();
	}

	updateCaption();

	m_pSlaveThread = new DccChatThread(this, m_pMarshal->releaseSocket());

#ifdef COMPILE_SSL_SUPPORT
	KviSSL * s = m_pMarshal->releaseSSL();
	if(s)
	{
		KviSSLMaster::printSSLConnectionInfo(this, s);
		m_pSlaveThread->setSSL(s);
	}
#endif
	m_pSlaveThread->start();

	if(!KVS_TRIGGER_EVENT_1_HALTED(KviEvent_OnDCCChatConnected, this, m_pDescriptor->idString()))
	{
		output(KVI_OUT_DCCMSG, __tr2qs_ctx("Connected to %Q:%Q", "dcc"),
		    &(m_pMarshal->remoteIp()), &(m_pMarshal->remotePort()));
		output(KVI_OUT_DCCMSG, __tr2qs_ctx("Local end is %Q:%Q", "dcc"),
		    &(m_pMarshal->localIp()), &(m_pMarshal->localPort()));
#ifdef COMPILE_SSL_SUPPORT
		QString tmp = QString("DCC: %1 %2@%3:%4").arg(m_pDescriptor->bIsSSL ? "SChat" : "Chat", m_pDescriptor->szNick, m_pDescriptor->szIp, m_pDescriptor->szPort);
#else
		QString tmp = QString("DCC: %1 %2@%3:%4").arg("Chat", m_pDescriptor->szNick, m_pDescriptor->szIp, m_pDescriptor->szPort);
#endif
		m_pLabel->setText(tmp);
	}
}

//
// THREAD
//

DccChatThread::DccChatThread(KviWindow * wnd, kvi_socket_t fd)
    : DccThread(wnd, fd)
{
}

void DccChatThread::run()
{
	KviDccThreadIncomingData data;
	data.iLen = 0;
	data.buffer = nullptr;

	for(;;)
	{
		// Dequeue events
		while(KviThreadEvent * e = dequeueEvent())
		{
			if(e->id() == KVI_THREAD_EVENT_TERMINATE)
			{
				delete e;
				goto out_of_the_loop;
			}
			else
			{
				// Other events are senseless to us
				delete e;
			}
		}

		bool bCanRead;
		bool bCanWrite;
		if(kvi_select(m_fd, &bCanRead, &bCanWrite))
		{
			if(bCanWrite)
			{
				if(!tryFlushOutBuffers())
					goto out_of_the_loop;
			}
			if(bCanRead)
			{
				data.buffer = (char *)KviMemory::reallocate(data.buffer, (data.iLen + 512) * sizeof(char));
				int readLen;
#ifdef COMPILE_SSL_SUPPORT
				if(m_pSSL)
				{
					readLen = m_pSSL->read(data.buffer + data.iLen, 512);
				}
				else
				{
#endif
					readLen = kvi_socket_recv(m_fd, data.buffer + data.iLen, 512);
#ifdef COMPILE_SSL_SUPPORT
				}
#endif
				if(readLen > 0)
				{
					data.iLen += readLen;
					data.buffer = (char *)KviMemory::reallocate(data.buffer, data.iLen * sizeof(char));
					if(!handleIncomingData(&data, false))
						break; // non critical...
				}
				else
				{

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
									postErrorEvent(KviError::SSLError);
									goto out_of_the_loop;
								}
							}
							break;
							case KviSSL::SSLError:
							{
								raiseSSLError();
								postErrorEvent(KviError::SSLError);
								goto out_of_the_loop;
							}
							break;
							default:
								// Raise unknown SSL ERROR
								postErrorEvent(KviError::SSLError);
								goto out_of_the_loop;
								break;
						}
					}
#endif
					if(data.iLen > 0)
					{
						data.buffer = (char *)KviMemory::reallocate(data.buffer, data.iLen * sizeof(char));
					}
					else
					{
						KviMemory::free(data.buffer);
						data.buffer = nullptr;
					}

					if(!handleInvalidSocketRead(readLen))
					{
						if(data.iLen)
							handleIncomingData(&data, true); // critical
						KVI_ASSERT(!data.iLen);
						break; // error
					}
				}
			}
			msleep(100);
		}
	}

out_of_the_loop:

	if(data.iLen)
		KviMemory::free(data.buffer);

#ifdef COMPILE_SSL_SUPPORT
	if(m_pSSL)
	{
		KviSSLMaster::freeSSL(m_pSSL);
		m_pSSL = nullptr;
	}
#endif

	if(m_fd != KVI_INVALID_SOCKET)
		::kvi_socket_close(m_fd);
	m_fd = KVI_INVALID_SOCKET;
}

bool DccChatThread::handleIncomingData(KviDccThreadIncomingData * data, bool bCritical)
{
	KVI_ASSERT(data->iLen);
	KVI_ASSERT(data->buffer);
	char * aux = data->buffer;
	char * end = data->buffer + data->iLen;
	while(aux != end)
	{
		if((*aux == '\n') || (*aux == '\0'))
		{
			KviThreadDataEvent<KviCString> * e = new KviThreadDataEvent<KviCString>(KVI_DCC_THREAD_EVENT_DATA);
			// The left part is len chars long
			int len = aux - data->buffer;
			//			qDebug("LEN = %d, iLen = %d",len,data->iLen);
			//#warning "DO IT BETTER (the \r cutting)"
			KviCString * s = new KviCString(data->buffer, len);
			if(s->lastCharIs('\r'))
				s->cutRight(1);
			e->setData(s);
			// but we cut also \n (or \0)
			++aux;
			// so len += 1; --> new data->iLen -= len;
			data->iLen -= (len + 1);
			//			qDebug("iLen now = %d",data->iLen);
			KVI_ASSERT(data->iLen >= 0);
			if(data->iLen > 0)
			{
				// memmove the remaining part to the beginning
				// aux points after \n or \0
				KviMemory::move(data->buffer, aux, data->iLen);
				data->buffer = (char *)KviMemory::reallocate(data->buffer, data->iLen);
				end = data->buffer + data->iLen;
				aux = data->buffer;
			}
			else
			{
				// no more data in the buffer
				KVI_ASSERT(data->iLen == 0);
				KviMemory::free(data->buffer);
				data->buffer = end = aux = nullptr;
			}
			postEvent(parent(), e);
		}
		else
			aux++;
		//		qDebug("PASSING CHAR %c",*aux);
	}
	// now aux == end
	if(bCritical)
	{
		// need to flush everything...
		if(data->iLen > 0)
		{
			// in the last part there are no NULL and \n chars
			KviThreadDataEvent<KviCString> * e = new KviThreadDataEvent<KviCString>(KVI_DCC_THREAD_EVENT_DATA);
			KviCString * s = new KviCString(data->buffer, data->iLen);
			if(s->lastCharIs('\r'))
				s->cutRight(1);
			e->setData(s);
			data->iLen = 0;
			KviMemory::free(data->buffer);
			data->buffer = nullptr;
			postEvent(parent(), e);
		}
	}
	return true;
}

void DccChatThread::sendRawData(const void * buffer, int len)
{
	m_pMutex->lock();
	m_pOutBuffers.emplace_back(new KviDataBuffer((unsigned int)len, (const unsigned char *)buffer));
	m_pMutex->unlock();
}

bool DccChatThread::tryFlushOutBuffers()
{
	bool bRet = true;
	m_pMutex->lock();
	while(!m_pOutBuffers.empty())
	{
		auto & b = m_pOutBuffers.front();
		int sentLen;

#ifdef COMPILE_SSL_SUPPORT
		if(m_pSSL)
		{
			sentLen = m_pSSL->write((const char *)b->data(), b->size());
		}
		else
		{
#endif
			sentLen = kvi_socket_send(m_fd, b->data(), b->size());
#ifdef COMPILE_SSL_SUPPORT
		}
#endif
		if(sentLen > 0)
		{
			if(sentLen == b->size())
				m_pOutBuffers.pop_front();
			else
			{
				// just a part
				b->remove((unsigned int)sentLen);
				break;
			}
		}
		else
		{
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
							postErrorEvent(KviError::RemoteEndClosedConnection);
							bRet = false;
							goto out_of_the_loop;
						}
						else
						{
							int iSSLErr = m_pSSL->getLastError(true);
							if(iSSLErr != 0)
							{
								raiseSSLError();
								postErrorEvent(KviError::SSLError);
								bRet = false;
								goto out_of_the_loop;
							}
							else
							{
								goto handle_system_error;
							}
						}
						break;
					case KviSSL::SSLError:
						raiseSSLError();
						postErrorEvent(KviError::SSLError);
						bRet = false;
						goto out_of_the_loop;
						break;
					default:
						postErrorEvent(KviError::SSLError);
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
				if((err != EAGAIN) && (err != EINTR) && (err != WSAEWOULDBLOCK))
#else
				if((err != EAGAIN) && (err != EINTR))
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
