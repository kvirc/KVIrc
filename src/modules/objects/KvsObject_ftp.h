#ifndef _CLASS_FTP_H_
#define _CLASS_FTP_H_
//=============================================================================
//
//   File : KvsObject_ftp.h
//   Creation date : Mon Sep 1 08:13:45 2008 GMT by Carbone Alesssandro
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2009 Alessandro Carbone (elfonol at gmail dot com)
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

#include "object_macros.h"

class QFtp;
class QUrlInfo;

#include <QHash>
#include <QFile>

class KvsObject_ftp : public KviKvsObject
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KvsObject_ftp)
protected:
	QFtp * m_pFtp;

protected:
	bool functionConnect(KviKvsObjectFunctionCall * c);
	bool get(KviKvsObjectFunctionCall * c);
	bool put(KviKvsObjectFunctionCall * c);
	bool login(KviKvsObjectFunctionCall * c);
	bool cd(KviKvsObjectFunctionCall * c);
	bool list(KviKvsObjectFunctionCall * c);
	bool dataTransferProgressEvent(KviKvsObjectFunctionCall * c);
	bool listInfoEvent(KviKvsObjectFunctionCall * c);
	bool commandFinishedEvent(KviKvsObjectFunctionCall * c);
	bool stateChangedEvent(KviKvsObjectFunctionCall * c);
	bool abort(KviKvsObjectFunctionCall * c);
	bool close(KviKvsObjectFunctionCall * c);
protected slots:
	void slotCommandFinished(int id, bool error);
	void slotCommandStarted(int id);
	void slotDataTransferProgress(qint64 done, qint64 total);
	void slotDone(bool error);
	void slotListInfo(const QUrlInfo & i);
	void slotRawCommandReply(int replyCode, const QString & detail);
	void slotReadyRead();
	void slotStateChanged(int state);
};

#endif //_CLASS_FTP_H_
