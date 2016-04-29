#ifndef _CLASS_HTTP_H_
#define _CLASS_HTTP_H_
//=============================================================================
//
//   File : KvsObject_http.h
//   Creation date : Fry Sep 5 18:13:45 2008 GMT by Carbone Alesssandro
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Alessandro Carbone (elfonol at gmail dot com)
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

//===============================================================
#include "object_macros.h"

class QHttp;
class QHttpResponseHeader;

#include <QFile>
#include <QHash>
#include <QSslError>

class KvsObject_http : public KviKvsObject
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KvsObject_http)
protected:
	QHttp * m_pHttp;
	QHash<int, QFile *> getDict;
	bool m_bAbort;
	bool m_bEnableForceRedirect;

protected:
	bool functionSetHost(KviKvsObjectFunctionCall * c);
	bool functionGet(KviKvsObjectFunctionCall * c);
	bool functionSetUser(KviKvsObjectFunctionCall * c);
	bool functionSetProxy(KviKvsObjectFunctionCall * c);
	bool functionRequestFinishedEvent(KviKvsObjectFunctionCall * c);
	bool functionRequestStartedEvent(KviKvsObjectFunctionCall * c);
	bool functionDataReadProgressEvent(KviKvsObjectFunctionCall * c);
	bool functionDataSendProgressEvent(KviKvsObjectFunctionCall * c);
	bool functionResponseHeaderReceivedEvent(KviKvsObjectFunctionCall * c);
	bool functionAbort(KviKvsObjectFunctionCall * c);
	bool functionDoneEvent(KviKvsObjectFunctionCall * c);
	bool functionStateChangedEvent(KviKvsObjectFunctionCall * c);
	bool functionReadAll(KviKvsObjectFunctionCall * c);
	bool functionReadyReadEvent(KviKvsObjectFunctionCall * c);
	bool functionErrorString(KviKvsObjectFunctionCall * c);
	bool functionPost(KviKvsObjectFunctionCall * c);
	bool functionFollowRedirect(KviKvsObjectFunctionCall * c);
	bool functionCurrentId(KviKvsObjectFunctionCall * c);
	void redirect(QString & file, const QHttpResponseHeader & r);

#ifndef QT_NO_OPENSSL
	bool functionIgnoreSslErrors(KviKvsObjectFunctionCall * c);
	bool functionSslErrorsEvent(KviKvsObjectFunctionCall * c);
#endif

protected slots:
	void slotRequestFinished(int id, bool error);
	void slotDone(bool error);
	void slotRequestStarted(int id);
	void slotDataSendProgress(int done, int total);
	void slotDataReadProgress(int done, int total);
#ifndef QT_NO_OPENSSL
	void slotSslErrors(QList<QSslError>);
#endif
	void slotResponseHeaderReceived(const QHttpResponseHeader & r);
	void slotReadyRead(const QHttpResponseHeader & r);
	void slotStateChanged(int state);
};

#endif //_CLASS_HTTP_H_
