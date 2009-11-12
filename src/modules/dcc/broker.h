#ifndef _BROKER_H_
#define _BROKER_H_
//=============================================================================
//
//   File : broker.h
//   Creation date : Tue Sep 19 09 2000 10:20:01 by Szymon Stefanek
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

#include "kvi_settings.h"
#include "kvi_string.h"
#include "kvi_pointerlist.h"
#include "kvi_sparser.h"
#include "kvi_pointerhashtable.h"

#include <QDateTime>
#include <QObject>

class KviConsole;
class KviDccBroker;
class KviWindow;
class KviDccBox;

// #define COMPILE_DCC_CANVAS


class KviDccZeroPortTag
{
public:
	QDateTime      m_tTimestamp;
	QString        m_szTag;
	unsigned long  m_uFileSize; // outgoing file size, valid only for file transfers obviously
	unsigned long  m_uResumePosition; // if 0 = no resume, valid only for file transfers obviously
};

class KviDccBroker : public QObject
{
	Q_OBJECT
public:
	KviDccBroker();
	~KviDccBroker();
protected:
	KviPointerList<KviDccBox>         * m_pBoxList;
	KviPointerList<KviWindow>         * m_pDccWindowList;
	KviPointerHashTable<QString,KviDccZeroPortTag>      * m_pZeroPortTags;
public:
	KviDccZeroPortTag * addZeroPortTag();
	KviDccZeroPortTag * findZeroPortTag(const QString &szTag);
	void removeZeroPortTag(const QString &szTag);

	unsigned int dccWindowsCount(){ return m_pDccWindowList->count(); };
	unsigned int dccBoxCount();

	void unregisterDccBox(KviDccBox * box);
	void unregisterDccWindow(KviWindow *dcc);

	void rsendManage(KviDccDescriptor * dcc);
	void rsendAskForFileName(KviDccDescriptor * dcc);

	void handleChatRequest(KviDccDescriptor * dcc);

#ifdef COMPILE_DCC_CANVAS
	void activeCanvasManage(KviDccDescriptor * dcc);
	void passiveCanvasExecute(KviDccDescriptor * dcc);
#endif

	void activeVoiceManage(KviDccDescriptor * dcc);
	void passiveVoiceExecute(KviDccDescriptor * dcc);

	void activeVideoManage(KviDccDescriptor * dcc);
	void passiveVideoExecute(KviDccDescriptor * dcc);

	void recvFileManage(KviDccDescriptor * dcc);
	void sendFileManage(KviDccDescriptor * dcc);

	bool handleResumeAccepted(const char * filename,const char * port,const char * szZeroPortTag);
	bool handleResumeRequest(KviDccRequest * dcc,const char * filename,const char * port,unsigned long filePos,const char * szZeroPortTag);

public slots:
	void rsendExecute(KviDccBox * box,KviDccDescriptor * dcc);
	void rsendExecute(KviDccDescriptor * dcc);

//	void activeChatExecute(KviDccBox * box,KviDccDescriptor * dcc);
	void executeChat(KviDccBox * box,KviDccDescriptor * dcc);

	void activeCanvasExecute(KviDccBox * box,KviDccDescriptor * dcc);
	void activeVoiceExecute(KviDccBox * box,KviDccDescriptor * dcc);
	void activeVideoExecute(KviDccBox * box,KviDccDescriptor * dcc);

	void sendFileExecute(KviDccBox * box,KviDccDescriptor * dcc);
	void recvFileExecute(KviDccBox * box,KviDccDescriptor * dcc);

	void chooseSaveFileName(KviDccBox *box,KviDccDescriptor * dcc);
	void renameOverwriteResume(KviDccBox *box,KviDccDescriptor * dcc);
	void renameDccSendFile(KviDccBox *box,KviDccDescriptor * dcc);

	void cancelDcc(KviDccBox *box,KviDccDescriptor * dcc);
	void cancelDcc(KviDccDescriptor * dcc);

public:
	bool canUnload();
};

#endif
