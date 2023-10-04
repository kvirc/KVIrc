#ifndef _CLASS_WEVBIEW_H_
#define _CLASS_WEVBIEW_H_
//=============================================================================
//
//   File : KvsObject_webView.h
//   Creation date : Wed Mar 9 23:45:48 CEST 2011
//   by Alessandro Carbone(Noldor)
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2011 Alessandro Carbone (elfonol at gmail dot com)
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

#include "kvi_settings.h"
#if defined(COMPILE_WEBENGINE_SUPPORT)
#include "object_macros.h"
#include <QFile>
#include <QNetworkAccessManager>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QMouseEvent>
#include <QContextMenuEvent>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#define DOWNLOAD_CLASS_NAME QWebEngineDownloadItem
#else
#define DOWNLOAD_CLASS_NAME QWebEngineDownloadRequest
#endif

class DOWNLOAD_CLASS_NAME;

class KvsObject_webView;

class KviKvsWebView : public QWebEngineView
{
	Q_OBJECT
public:
	KviKvsWebView(QWidget * par, const char * name, KvsObject_webView *);
	//void accept();
	//void reject();
	~KviKvsWebView();

protected:
	KvsObject_webView * m_pParentScript;

protected:
	void mouseMoveEvent(QMouseEvent * ev) override;
	void contextMenuEvent(QContextMenuEvent *) override;
	bool event(QEvent * e) override;
	/*protected slots:
	void slotNextClicked();
	void slotBackClicked();
	*/
};

class KviKvsWebViewPage : public QWebEnginePage
{
    Q_OBJECT
public:
	enum LinkDelegationPolicy
	{
		DontDelegateLinks,
		DelegateAllLinks
	};

    KviKvsWebViewPage(QObject* parent = 0) : QWebEnginePage(parent){}

    bool acceptNavigationRequest(const QUrl & url, QWebEnginePage::NavigationType type, bool)
    {
        if (type == QWebEnginePage::NavigationTypeLinkClicked)
        {
   			emit linkClicked(url);
	    	switch(m_ePolicy)
	    	{
	    		case DelegateAllLinks:
	    			return false;
	    		case DontDelegateLinks:
	    		default:
	    			return true;
	    	}
        }
        return true;
    }

    void setLinkDelegationPolicy(LinkDelegationPolicy policy)
    {
		m_ePolicy = policy;
    }

protected:
	LinkDelegationPolicy m_ePolicy = DontDelegateLinks;
signals:
    void linkClicked(const QUrl&);

};

class KvsObject_webView : public KviKvsObject
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KvsObject_webView)
protected:
	KviKvsRunTimeContext * m_pContext = nullptr;
	KviPointerList<QNetworkReply> * m_pReplyList = nullptr;
	QNetworkAccessManager * m_pNetworkManager = nullptr;

public:
	QWidget * widget() { return (QWidget *)object(); }
protected:
	bool init(KviKvsRunTimeContext * pContext, KviKvsVariantList * pParams) override;
	bool makePreview(KviKvsObjectFunctionCall * c);
	bool load(KviKvsObjectFunctionCall * c);
	bool setHtml(KviKvsObjectFunctionCall * c);
	bool findText(KviKvsObjectFunctionCall * c);
	bool setLinkDelegationPolicy(KviKvsObjectFunctionCall * c);
	bool setWebSetting(KviKvsObjectFunctionCall * c);
	bool appendWebViewActionToMenu(KviKvsObjectFunctionCall * c);

	bool loadStartedEvent(KviKvsObjectFunctionCall * c);
	bool linkClickedEvent(KviKvsObjectFunctionCall * c);
	bool loadFinishedEvent(KviKvsObjectFunctionCall * c);
	bool loadProgressEvent(KviKvsObjectFunctionCall * c);
	bool downloadCompletedEvent(KviKvsObjectFunctionCall * c);
	bool downloadProgressEvent(KviKvsObjectFunctionCall * c);
	bool downloadRequestEvent(KviKvsObjectFunctionCall * c);
	bool addToJavaScriptWindowObject(KviKvsObjectFunctionCall * c);
	bool evaluateJavaScript(KviKvsObjectFunctionCall * c);

	//
	bool jsChangeEvent(KviKvsObjectFunctionCall * c);
	bool jsSubmitEvent(KviKvsObjectFunctionCall * c);
	bool jsClickEvent(KviKvsObjectFunctionCall * c);
	bool jsMouseOverEvent(KviKvsObjectFunctionCall * c);
	bool jsMouseOutEvent(KviKvsObjectFunctionCall * c);
protected slots:
	void slotLoadFinished(bool);
	void slotLoadProgress(int);
	void slotLoadStarted();
	void slotDownloadRequest(DOWNLOAD_CLASS_NAME *);
	void slotLinkClicked(const QUrl &);

	void slotOnChange(QString);
	void slotOnSubmit(QString);
	void slotOnClick(QString);
	void slotOnMouseOver(QString);
	void slotOnMouseOut(QString);
};

class KviKvsDownloadHandler : public QObject
{
	Q_OBJECT
public:
	KviKvsDownloadHandler(KvsObject_webView * pParent, QString &szFilePath, DOWNLOAD_CLASS_NAME * pDownload, int iId);

	~KviKvsDownloadHandler();

protected:
	KvsObject_webView * m_pParentScript;
	DOWNLOAD_CLASS_NAME * m_pDownload;
	int m_Id;
protected slots:
	void slotDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
	void slotFinished();
};

#endif // COMPILE_WEBENGINE_SUPPORT
#endif // _CLASS_WEBVIEW_H_
