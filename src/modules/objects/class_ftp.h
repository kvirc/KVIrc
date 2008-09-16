#ifndef _CLASS_FTP_H_
#define _CLASS_FTP_H_
//
//   File : class_ftp.h
//   Creation date : Mon Sep 1 08:13:45 2008 GMT by Carbone Alesssandro
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
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




//===============================================================
#include "object_macros.h"
#include <QFtp>
#include <QFile>

class KviKvsObject_ftp : public KviKvsObject
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_ftp)
protected:
	QFtp *m_pFtp;
	QFile *m_pFile;
protected:
	bool functionConnect(KviKvsObjectFunctionCall *c);
	bool functionGet(KviKvsObjectFunctionCall *c);
	bool functionLogin(KviKvsObjectFunctionCall *c);
	bool functionCd(KviKvsObjectFunctionCall *c);
	bool functionList(KviKvsObjectFunctionCall *c);
	bool function_dataTransferProgressEvent(KviKvsObjectFunctionCall *c);
	bool function_listInfoEvent(KviKvsObjectFunctionCall *c);
	bool function_commandFinishedEvent(KviKvsObjectFunctionCall *c);
	bool function_stateChangedEvent(KviKvsObjectFunctionCall *c);
	bool functionAbort(KviKvsObjectFunctionCall *c);

protected slots:
	void slotCommandFinished ( int id, bool error );
	void slotCommandStarted ( int id );
	void slotDataTransferProgress ( qint64 done, qint64 total );
	void slotDone ( bool error );
	void slotListInfo ( const QUrlInfo & i );
	void slotRawCommandReply ( int replyCode, const QString & detail );
	void slotReadyRead ();
	void slotStateChanged ( int state);
};
#endif //_CLASS_FTP_H_
