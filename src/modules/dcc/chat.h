#ifndef _CHAT_H_
#define _CHAT_H_
//=============================================================================
//
//   File : chat.h
//   Creation date : Tue Sep 20 09 2000 15:11:12 by Szymon Stefanek
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

#include "descriptor.h"
#include "thread.h"
#include "window.h"

#include "kvi_databuffer.h"
#include "kvi_pointerlist.h"
#include "kvi_themedlabel.h"
#include "kvi_string.h"
#include "kvi_window.h"

#ifdef COMPILE_SSL_SUPPORT
	class KviSSL;
#endif

class KviDccMarshal;
class QSplitter;

class KviDccChatThread : public KviDccThread
{
public:
	KviDccChatThread(KviWindow * wnd,kvi_socket_t fd);
	~KviDccChatThread();
protected:
	KviPointerList<KviDataBuffer>    * m_pOutBuffers;
protected:
	virtual void run();
	bool tryFlushOutBuffers();
	// This should handle the incoming data buffer
	// must "eat" some data from data.buffer, memmove the remaining part
	// to the beginning , kvi_realloc data.buffer and update data.iLen
	// If bCritical is true , it should handle the whole data buffer
	// since the thread is going to die
	// It should return true if the handing was succesfull
	// or false if the thread should be stopped
	virtual bool handleIncomingData(KviDccThreadIncomingData *data,bool bCritical);
public:
	virtual void sendRawData(const void * buffer,int len); // mutex (m_pOutBuffers usage)
};


class KviDccChat : public KviDccWindow
{
	Q_OBJECT
public:
	KviDccChat(KviFrame *pFrm,KviDccDescriptor * dcc,const char * name);
	~KviDccChat();
	QFrame * buttonContainer() { return (QFrame*)m_pButtonContainer; };
protected:
	KviDccChatThread       * m_pSlaveThread;
	QString                  m_szTarget;
	QString                  m_szLocalNick;
	KviThemedLabel         * m_pLabel;
	KviTalHBox             * m_pButtonBox;
	KviTalHBox             * m_pButtonContainer;
protected:
	virtual const QString & target();
	virtual void fillCaptionBuffers();
	virtual void getBaseLogFileName(QString &buffer);
	virtual QPixmap * myIconPtr();
	virtual void resizeEvent(QResizeEvent *e);
	virtual QSize sizeHint() const;
	virtual const QString & localNick();
	virtual bool event(QEvent *e);
	virtual void ownMessage(const QString &text);
	virtual void ownAction(const QString &text);
	virtual void triggerCreationEvents();
	virtual void triggerDestructionEvents();
	void startConnection();
protected slots:
	void handleMarshalError(int err);
	void connected();
	void sslError(const char * msg);
	void connectionInProgress();
	void startingSSLHandshake();
	void textViewRightClicked();
};

#endif //_CHAT_H_
