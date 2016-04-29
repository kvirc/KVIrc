#ifndef _BROKER_H_
#define _BROKER_H_
//=============================================================================
//
//   File : DccBroker.h
//   Creation date : Tue Sep 19 09 2000 10:20:01 by Szymon Stefanek
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

#include "kvi_settings.h"
#include "KviCString.h"
#include "KviPointerList.h"
#include "KviIrcServerParser.h"
#include "KviPointerHashTable.h"

#include <QDateTime>
#include <QObject>

class KviConsoleWindow;
class DccBroker;
class KviWindow;
class DccDialog;

// #define COMPILE_DCC_CANVAS

class KviDccZeroPortTag
{
public:
	QDateTime m_tTimestamp;
	QString m_szTag;
	unsigned long m_uFileSize;       // outgoing file size, valid only for file transfers obviously
	unsigned long m_uResumePosition; // if 0 = no resume, valid only for file transfers obviously
};

class DccBroker : public QObject
{
	Q_OBJECT
public:
	DccBroker();
	~DccBroker();

protected:
	KviPointerList<DccDialog> * m_pBoxList;
	KviPointerList<KviWindow> * m_pDccWindowList;
	KviPointerHashTable<QString, KviDccZeroPortTag> * m_pZeroPortTags;

public:
	KviDccZeroPortTag * addZeroPortTag();
	KviDccZeroPortTag * findZeroPortTag(const QString & szTag);
	void removeZeroPortTag(const QString & szTag);

	unsigned int dccWindowsCount() { return m_pDccWindowList->count(); };
	unsigned int dccBoxCount();

	void unregisterDccBox(DccDialog * box);
	void unregisterDccWindow(KviWindow * dcc);

	void rsendManage(DccDescriptor * dcc);
	void rsendAskForFileName(DccDescriptor * dcc);

	void handleChatRequest(DccDescriptor * dcc);

#ifdef COMPILE_DCC_CANVAS
	void activeCanvasManage(DccDescriptor * dcc);
	void passiveCanvasExecute(DccDescriptor * dcc);
#endif

	void activeVoiceManage(DccDescriptor * dcc);
	void passiveVoiceExecute(DccDescriptor * dcc);

	void activeVideoManage(DccDescriptor * dcc);
	void passiveVideoExecute(DccDescriptor * dcc);

	void recvFileManage(DccDescriptor * dcc);
	void sendFileManage(DccDescriptor * dcc);

	bool handleResumeAccepted(const char * filename, const char * port, const char * szZeroPortTag);
	bool handleResumeRequest(KviDccRequest * dcc, const char * filename, const char * port, unsigned long filePos, const char * szZeroPortTag);

public slots:
	void rsendExecute(DccDialog * box, DccDescriptor * dcc);
	void rsendExecute(DccDescriptor * dcc);

	//	void activeChatExecute(DccDialog * box,DccDescriptor * dcc);
	void executeChat(DccDialog * box, DccDescriptor * dcc);

	void activeCanvasExecute(DccDialog * box, DccDescriptor * dcc);
	void activeVoiceExecute(DccDialog * box, DccDescriptor * dcc);
	void activeVideoExecute(DccDialog * box, DccDescriptor * dcc);

	void sendFileExecute(DccDialog * box, DccDescriptor * dcc);
	void recvFileExecute(DccDialog * box, DccDescriptor * dcc);

	void chooseSaveFileName(DccDialog * box, DccDescriptor * dcc);
	void renameOverwriteResume(DccDialog * box, DccDescriptor * dcc);
	void renameDccSendFile(DccDialog * box, DccDescriptor * dcc);

	void cancelDcc(DccDialog * box, DccDescriptor * dcc);
	void cancelDcc(DccDescriptor * dcc);

public:
	bool canUnload();
};

#endif
