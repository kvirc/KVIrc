#ifndef _CHAT_H_
#define _CHAT_H_
//=============================================================================
//
//   File : DccChatWindow.h
//   Creation date : Tue Sep 20 09 2000 15:11:12 by Szymon Stefanek
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

#include "DccDescriptor.h"
#include "DccThread.h"
#include "DccWindow.h"

#include "KviDataBuffer.h"
#include "KviThemedLabel.h"
#include "KviCString.h"
#include "KviWindow.h"
#include "KviError.h"

#include <deque>
#include <memory>

#ifdef COMPILE_SSL_SUPPORT
class KviSSL;
#endif

class DccMarshal;
class QSplitter;

class DccChatThread : public DccThread
{
public:
	DccChatThread(KviWindow * wnd, kvi_socket_t fd);

protected:
	std::deque<std::unique_ptr<KviDataBuffer>> m_pOutBuffers;

protected:
	virtual void run();
	bool tryFlushOutBuffers();
	// This should handle the incoming data buffer
	// must "eat" some data from data.buffer, memmove the remaining part
	// to the beginning, kvi_realloc data.buffer and update data.iLen
	// If bCritical is true, it should handle the whole data buffer
	// since the thread is going to die
	// It should return true if the handing was successful
	// or false if the thread should be stopped
	bool handleIncomingData(KviDccThreadIncomingData * data, bool bCritical);

public:
	void sendRawData(const void * buffer, int len); // mutex (m_pOutBuffers usage)
};

class DccChatWindow : public DccWindow
{
	Q_OBJECT
public:
	DccChatWindow(DccDescriptor * dcc, const char * name);
	~DccChatWindow();
	QFrame * buttonContainer() override { return (QFrame *)m_pButtonContainer; }
protected:
	DccChatThread * m_pSlaveThread;
	QString m_szTarget;
	QString m_szLocalNick;
	KviThemedLabel * m_pLabel;
	KviTalHBox * m_pButtonBox;
	KviTalHBox * m_pButtonContainer;

protected:
	const QString & target() override;
	void fillCaptionBuffers() override;
	void getBaseLogFileName(QString & buffer) override;
	QPixmap * myIconPtr() override;
	void resizeEvent(QResizeEvent * e) override;
	QSize sizeHint() const override;
	const QString & localNick() override;
	bool event(QEvent * e) override;
	void ownMessage(const QString & text, bool bUserFeedback = true) override;
	void ownAction(const QString & text) override;
	void triggerCreationEvents() override;
	void triggerDestructionEvents() override;
	void startConnection();
	DccThread * getSlaveThread() override { return m_pSlaveThread; }
protected slots:
	void handleMarshalError(KviError::Code eError);
	void connected();
	void sslError(const char * msg);
	void connectionInProgress();
	void startingSSLHandshake();
	void textViewRightClicked();
};

#endif //_CHAT_H_
