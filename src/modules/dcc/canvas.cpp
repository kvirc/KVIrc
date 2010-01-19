//=============================================================================
//
//   File : canvas.cpp
//   Creation date : Sun Jul 29 07 2001 20:23:13 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#define QT_MODULE_CANVAS

#include "canvas.h"

#ifdef COMPILE_DCC_CANVAS

	#include "dialogs.h"
	#include "marshal.h"
	#include "canvaswidget.h"

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
	#include "kvi_ircconnection.h"

	#include <QSplitter>

	extern KviDccBroker * g_pDccBroker;


	KviDccCanvas::KviDccCanvas(KviFrame *pFrm,KviDccDescriptor * dcc,const char * name)
	: KviDccWindow(KVI_WINDOW_TYPE_DCCCANVAS,pFrm,name,dcc)
	{
		m_pSplitter = new QSplitter(QSplitter::Vertical,this,"splitter");

		m_pCanvas   = new KviCanvasWidget(m_pSplitter);

		m_pIrcView  = new KviIrcView(m_pSplitter,pFrm,this);
		m_pInput    = new KviInput(this);

	//	setFocusHandler(m_pInput,this);

		m_pMarshal = new KviDccMarshal(this);
		connect(m_pMarshal,SIGNAL(error(int)),this,SLOT(handleMarshalError(int)));
		connect(m_pMarshal,SIGNAL(connected()),this,SLOT(connected()));


		if(!(m_pDescriptor->bActive))
		{
			// PASSIVE CONNECTION
			output(KVI_OUT_DCCMSG,__tr2qs_ctx("Attempting a passive DCC CANVAS connection","dcc"));
			int ret = m_pMarshal->dccListen(dcc->szListenIp,dcc->szListenPort,m_pDescriptor->bDoTimeout);
			if(ret != KviError_success)handleMarshalError(ret);
			else {

				output(KVI_OUT_DCCMSG,__tr2qs_ctx("Listening on interface %Q port %Q","dcc"),
					&(m_pMarshal->localIp()),&(m_pMarshal->localPort()));

				if(dcc->bSendRequest)
				{
					QString ip     = !dcc->szFakeIp.isEmpty() ? dcc->szFakeIp : dcc->szListenIp;
					QString port   = !dcc->szFakePort.isEmpty() ? dcc->szFakePort.toUtf8().data() : m_pMarshal->localPort();
					//#warning "OPTION FOR SENDING 127.0.0.1 and so on (not an unsigned number)"
					struct in_addr a;
					if(kvi_stringIpToBinaryIp(ip.toUtf8().data(),&a))ip.setNum(htonl(a.s_addr));
					dcc->console()->connection()->sendFmtData("PRIVMSG %s :%cDCC CANVAS chat %Q %Q%c",
						dcc->console()->connection()->encodeText( dcc->szNick.toUtf8().data() ).data(),
						0x01,&ip,
						&port,
						0x01);
					output(KVI_OUT_DCCMSG,__tr2qs_ctx("Sent DCC CANVAS request to %Q, waiting for the remote client to connect...","dcc"),
						&(dcc->szNick));
				} else outputNoFmt(KVI_OUT_DCCMSG,__tr2qs_ctx("DCC CANVAS request not sent: awaiting manual connections","dcc"));
			}
		} else {
			// ACTIVE CONNECTION
			outputNoFmt(KVI_OUT_DCCMSG,__tr2qs_ctx("Attempting an active DCC CANVAS connection","dcc"));
			int ret = m_pMarshal->dccConnect(dcc->szIp.toUtf8().data(),dcc->szPort.toUtf8().data(),m_pDescriptor->bDoTimeout);
			if(ret != KviError_success)handleMarshalError(ret);
			else output(KVI_OUT_DCCMSG,__tr2qs_ctx("Contacting host %Q on port %Q","dcc"),&(dcc->szIp),&(dcc->szPort));
		}

	//	m_pSlaveThread = 0;
	}

	KviDccCanvas::~KviDccCanvas()
	{
		g_pDccBroker->unregisterDccWindow(this);
	//	if(m_pSlaveThread)
	//	{
	//		m_pSlaveThread->terminate();
	//		delete m_pSlaveThread;
	//		m_pSlaveThread = 0;
	//	}
		KviThreadManager::killPendingEvents(this);
	//	delete m_pDescriptor;
	//	delete m_pMarshal;
	}

	const QString & KviDccCanvas::target()
	{
		// This may change on the fly...
		m_szTarget.sprintf("%s@%s:%s",
			m_pDescriptor->szNick.toUtf8().data(),m_pDescriptor->szIp.toUtf8().data(),m_pDescriptor->szPort.toUtf8().data());
		return m_szTarget;
	}

	void KviDccCanvas::fillCaptionBuffers()
	{
		KviStr tmp(KviStr::Format,"DCC Canvas %s@%s:%s",
			m_pDescriptor->szNick.toUtf8().data(),m_pDescriptor->szIp.toUtf8().data(),m_pDescriptor->szPort.toUtf8().data());

		m_szPlainTextCaption = tmp;

		m_szHtmlActiveCaption.sprintf("<nobr><font color=\"%s\"><b>%s</b></font></nobr>",
			KVI_OPTION_COLOR(KviOption_colorCaptionTextActive).name().ascii(),tmp.ptr());
		m_szHtmlInactiveCaption.sprintf("<nobr><font color=\"%s\"><b>%s</b></font></nobr>",
			KVI_OPTION_COLOR(KviOption_colorCaptionTextInactive).name().ascii(),tmp.ptr());
	}

	QPixmap * KviDccCanvas::myIconPtr()
	{
		return g_pIconManager->getSmallIcon(KVI_SMALLICON_CANVAS);
	}

	void KviDccCanvas::getBaseLogFileName(KviStr &buffer)
	{
		buffer.sprintf("%s_%s_%s",m_pDescriptor->szNick.toUtf8().data(),m_pDescriptor->szIp.toUtf8().data(),m_pDescriptor->szPort.toUtf8().data());
	}

	void KviDccCanvas::ownMessage(const char * text)
	{
		KviStr buf(KviStr::Format,"%s\r\n",text);
	//	m_pSlaveThread->sendRawData(buf.ptr(),buf.len());
		m_pFrm->firstConsole()->outputPrivmsg(this,KVI_OUT_OWNPRIVMSG,
			m_pDescriptor->szLocalNick.toUtf8().data(),m_pDescriptor->szLocalUser.toUtf8().data(),
			m_pDescriptor->szLocalHost.toUtf8().data(),text);
	}

	void KviDccCanvas::ownAction(const char * text)
	{
		KviStr buf(KviStr::Format,"%cACTION %s%c\r\n",text);
	//	m_pSlaveThread->sendRawData(buf.ptr(),buf.len());
		output(KVI_OUT_ACTION,"%Q %s",&(m_pDescriptor->szLocalNick),text);
	}

	bool KviDccCanvas::event(QEvent *e)
	{
	//	if(e->type() == KVI_THREAD_EVENT)
	//	{
	//		switch(((KviThreadEvent *)e)->id())
	//		{
	//			case KVI_DCC_THREAD_EVENT_ERROR:
	//			{
	//				int * err = ((KviThreadDataEvent<int> *)e)->getData();
	//				output(KVI_OUT_DCCERROR,__tr("ERROR: %s"),kvi_getErrorString(*err));
	//				delete err;
	//				return true;
	//			}
	//			break;
	//			case KVI_DCC_THREAD_EVENT_DATA:
	//			{
	//				KviStr * d = ((KviThreadDataEvent<KviStr> *)e)->getData();
	//				if(d->firstCharIs(0x01))
	//				{
	//					d->cutLeft(1);
	//					if(d->lastCharIs(0x01))d->cutRight(1);
	//					if(kvi_strEqualCIN("ACTION",d->ptr(),6))d->cutLeft(6);
	//					d->stripLeftWhiteSpace();
	//					output(KVI_OUT_ACTION,"%s %s",m_pDescriptor->szNick.ptr(),d->ptr());
	//				} else {
	//
	//#ifdef COMPILE_CRYPT_SUPPORT
	//					if(KviCryptSessionInfo * cinf = cryptSessionInfo())
	//					{
	//						if(cinf->bDoDecrypt)
	//						{
	//							if(cinf->pEngine->isCryptographicEngine() && (*(d->ptr()) == KVI_TEXT_CRYPT))
	//							{
	//								KviStr decryptedStuff;
	//								if(cinf->pEngine->decrypt(d->ptr() + 1,decryptedStuff))
	//								{
	//									m_pFrm->firstConsole()->outputPrivmsg(this,KVI_OUT_DCCCHATMSGCRYPTED,
	//										m_pDescriptor->szNick.ptr(),m_pDescriptor->szUser.ptr(),
	//										m_pDescriptor->szHost.ptr(),decryptedStuff.ptr());
	//								} else {
	//									output(KVI_OUT_SYSTEMERROR,
	//										__tr("The following message looks like an encrypted one, but the crypting engine failed to decode it: %s"),
	//										cinf->pEngine->lastError());
	//									m_pFrm->firstConsole()->outputPrivmsg(this,KVI_OUT_DCCCHATMSG,
	//										m_pDescriptor->szNick.ptr(),m_pDescriptor->szUser.ptr(),
	//										m_pDescriptor->szHost.ptr(),d->ptr() + 1);
	//								}
	//								delete d;
	//								return true;
	//							} else {
	//								if(!(cinf->pEngine->isCryptographicEngine()))
	//								{
	//									KviStr decryptedStuff;
	//									if(cinf->pEngine->decrypt(d->ptr(),decryptedStuff))
	//									{
	//										m_pFrm->firstConsole()->outputPrivmsg(this,KVI_OUT_DCCCHATMSG,
	//											m_pDescriptor->szNick.ptr(),m_pDescriptor->szUser.ptr(),
	//											m_pDescriptor->szHost.ptr(),decryptedStuff.ptr());
	//										delete d;
	//										return true;
	//									} else {
	//										output(KVI_OUT_SYSTEMERROR,
	//											__tr("The following message looks like an encrypted one, but the crypting engine failed to decode it: %s"),
	//											cinf->pEngine->lastError());
	//									}
	//								}
	//							}
	//						}
	//					}
	//#endif
	//					m_pFrm->firstConsole()->outputPrivmsg(this,KVI_OUT_DCCCHATMSG,
	//						m_pDescriptor->szNick.ptr(),m_pDescriptor->szUser.ptr(),
	//						m_pDescriptor->szHost.ptr(),d->ptr());
	//				}
	//				delete d;
	//				return true;
	//			}
	//			break;
	//		}
	//	}
		return KviWindow::event(e);
	}

	void KviDccCanvas::resizeEvent(QResizeEvent *e)
	{
		int hght = m_pInput->heightHint();
	//	int hght2 = m_pTopSplitter->sizeHint().height();
	//	m_pTopSplitter->setGeometry(0,0,width(),hght2);
		m_pSplitter->setGeometry(0,0,width(),height() - hght);
		m_pInput->setGeometry(0,height() - hght,width(),hght);
	}

	QSize KviDccCanvas::sizeHint() const
	{
		QSize ret(m_pIrcView->sizeHint().width(),
			m_pIrcView->sizeHint().height() + m_pInput->heightHint());
		return ret;
	}

	void KviDccCanvas::handleMarshalError(int err)
	{
		QString sss = KviError::getDescription(err);
		output(KVI_OUT_DCCERROR,__tr2qs_ctx("DCC Failed: %Q","dcc"),&sss);
	}

	void KviDccCanvas::connected()
	{
		output(KVI_OUT_DCCMSG,__tr2qs_ctx("Connected to %Q:%Q","dcc"),
			&(m_pMarshal->remoteIp()),&(m_pMarshal->remotePort()));
		output(KVI_OUT_DCCMSG,__tr2qs_ctx("Local end is %Q:%Q","dcc"),
			&(m_pMarshal->localIp()),&(m_pMarshal->localPort()));
		if(!(m_pDescriptor->bActive))
		{
			// PASSIVE CONNECTION...Find out the remote end
			m_pDescriptor->szIp   = m_pMarshal->remoteIp();
			m_pDescriptor->szPort = m_pMarshal->remotePort();
			m_pDescriptor->szHost = m_pMarshal->remoteIp();
		}
		updateCaption();
	//	m_pSlaveThread = new KviDccCanvasThread(this,m_pMarshal->releaseSocket());
	//	m_pSlaveThread->start();
	}

	#include "m_canvas.moc"
#endif
