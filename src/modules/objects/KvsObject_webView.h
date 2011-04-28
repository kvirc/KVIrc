#ifndef _CLASS_WEVBIEW_H_
#define _CLASS_WEVBIEW_H_
//=============================================================================
//
//   File : KvsObject_webView.h
//   Creation date : Wed Mar 9 23:45:48 CEST 2011
//   by Alessandro Carbone(Noldor)
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2011 Alessandro Carbone (elfonol at gmail dot com)
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

#include "kvi_settings.h"
#if defined(COMPILE_WEBKIT_SUPPORT)
#include "object_macros.h"
#include <QFile>
#include <QNetworkAccessManager>
#include <QWebView>
#include <QWebFrame>
#include <QWebElement>

class KvsObject_webView : public KviKvsObject
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KvsObject_webView)
protected:
	KviKvsRunTimeContext          * m_pContext;
	QHash<QString,QWebFrame *>      m_dictFrames;
	QHash<QString,QWebElement *>    m_dictCache;
	KviPointerList<QNetworkReply> * m_pReplyList;
	QNetworkAccessManager         * m_pNetworkManager;
	QWebElementCollection           m_webElementCollection;
	QWebElement                     m_currentElement;
public:
	QWidget * widget(){ return (QWidget *)object(); }
protected:
	void getFrames(QWebFrame * pFrame, KviKvsArray * pArray, kvs_uint_t & uIdx);
	virtual bool init(KviKvsRunTimeContext * pContext, KviKvsVariantList * pParams);

	bool removeFromDocument(KviKvsObjectFunctionCall * c);
	bool makePreview(KviKvsObjectFunctionCall * c);

	bool removeClass(KviKvsObjectFunctionCall * c);
	bool classes(KviKvsObjectFunctionCall * c);

	bool rememberCurrent(KviKvsObjectFunctionCall * c);
	bool moveTo(KviKvsObjectFunctionCall * c);

	bool firstChild(KviKvsObjectFunctionCall * c);
	bool parentElement(KviKvsObjectFunctionCall * c);
	bool nextSibling(KviKvsObjectFunctionCall * c);

	bool toPlainText(KviKvsObjectFunctionCall * c);
	bool load(KviKvsObjectFunctionCall * c);
	bool getDocumentElement(KviKvsObjectFunctionCall * c);
	bool currentElementTagName(KviKvsObjectFunctionCall * c);
	bool findAll(KviKvsObjectFunctionCall * c);
	bool findFirst(KviKvsObjectFunctionCall * c);

	bool moveToQueryResultsAt(KviKvsObjectFunctionCall * c);
	bool queryResultsCount(KviKvsObjectFunctionCall * c);

	bool attributeNames(KviKvsObjectFunctionCall * c);
	bool setLinkDelegationPolicy(KviKvsObjectFunctionCall * c);
	bool setAttribute(KviKvsObjectFunctionCall * c);
	bool setWebSetting(KviKvsObjectFunctionCall * c);
	bool attribute(KviKvsObjectFunctionCall * c);
	bool frames(KviKvsObjectFunctionCall * c);

	bool loadStartedEvent(KviKvsObjectFunctionCall * c);
	bool linkClickedEvent(KviKvsObjectFunctionCall * c);
	bool loadFinishedEvent(KviKvsObjectFunctionCall * c);
	bool loadProgressEvent(KviKvsObjectFunctionCall * c);
	bool downloadCompletedEvent(KviKvsObjectFunctionCall * c);
	bool downloadProgressEvent(KviKvsObjectFunctionCall * c);
	bool downloadRequestEvent(KviKvsObjectFunctionCall * c);
protected slots:
	void slotLoadFinished(bool);
	void slotLoadProgress(int);
	void slotLoadStarted();
	void slotDownloadRequest(const QNetworkRequest &);
	void slotLinkClicked(const QUrl &);
};

class KviKvsDownloadHandler : public QObject
{
	Q_OBJECT
public:
	KviKvsDownloadHandler(KvsObject_webView * pParent, QFile * pFile, QNetworkReply * pNetReply, int iId);

	virtual ~KviKvsDownloadHandler();
protected:
	KvsObject_webView * m_pParentScript;
	QFile             * m_pFile;
	QNetworkReply     * m_pReply;
	int                 m_Id;
protected slots:
	void slotReadyRead();
	void slotReplyFinished();
};

#endif // COMPILE_WEBKIT_SUPPORT
#endif // _CLASS_WEBVIEW_H_
