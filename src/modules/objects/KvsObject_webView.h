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
#if defined(COMPILE_WEBKIT_SUPPORT)
#include "object_macros.h"
#include <QFile>
#include <QNetworkAccessManager>
#include <QWebView>
#include <QWebFrame>
#include <QWebElement>
#include <QMouseEvent>
#include <QContextMenuEvent>

class KvsObject_webView;
class KviKvsWebView : public QWebView
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

class KvsObject_webView : public KviKvsObject
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KvsObject_webView)
protected:
	KviKvsRunTimeContext * m_pContext = nullptr;
	int elementMapId = 1;
	int insertElement(const QWebElement & ele);
	QWebElement getElement(int iIdx);
	int getElementId(const QWebElement &);
	QHash<int, QWebElement> m_elementMapper;
	KviPointerList<KviKvsObject> * lWebelement = nullptr;
	QHash<QString, QWebElement *> m_dictCache;
	KviPointerList<QNetworkReply> * m_pReplyList = nullptr;
	QNetworkAccessManager * m_pNetworkManager = nullptr;
	QWebElementCollection m_webElementCollection;
	QWebElement m_currentElement;

public:
	QWidget * widget() { return (QWidget *)object(); }
protected:
	void getFrames(QWebFrame * pCurFrame, QStringList & szFramesNames);
	QWebFrame * findFrame(QWebFrame * pCurFrame, QString & szFrameName);
	bool init(KviKvsRunTimeContext * pContext, KviKvsVariantList * pParams) override;
	bool removeFromDocument(KviKvsObjectFunctionCall * c);
	bool makePreview(KviKvsObjectFunctionCall * c);

	bool removeClass(KviKvsObjectFunctionCall * c);
	bool addClass(KviKvsObjectFunctionCall * c);

	bool classes(KviKvsObjectFunctionCall * c);

	bool firstChild(KviKvsObjectFunctionCall * c);
	bool lastChild(KviKvsObjectFunctionCall * c);
	bool parentElement(KviKvsObjectFunctionCall * c);
	bool nextSibling(KviKvsObjectFunctionCall * c);

	bool toPlainText(KviKvsObjectFunctionCall * c);
	bool setPlainText(KviKvsObjectFunctionCall * c);
	bool load(KviKvsObjectFunctionCall * c);
	bool setHtml(KviKvsObjectFunctionCall * c);
	bool getDocumentElement(KviKvsObjectFunctionCall * c);
	bool elementTagName(KviKvsObjectFunctionCall * c);
	bool findAll(KviKvsObjectFunctionCall * c);
	bool appendInside(KviKvsObjectFunctionCall * c);
	bool appendOutside(KviKvsObjectFunctionCall * c);

	bool findFirst(KviKvsObjectFunctionCall * c);
	bool findText(KviKvsObjectFunctionCall * c);

	bool hitTestContent(KviKvsObjectFunctionCall * c);

	bool elementAttributeNames(KviKvsObjectFunctionCall * c);
	bool setLinkDelegationPolicy(KviKvsObjectFunctionCall * c);
	bool setElementAttribute(KviKvsObjectFunctionCall * c);
	bool setWebSetting(KviKvsObjectFunctionCall * c);
	bool elementAttribute(KviKvsObjectFunctionCall * c);
	bool frames(KviKvsObjectFunctionCall * c);

	bool setStyleProperty(KviKvsObjectFunctionCall * c);
	bool styleProperty(KviKvsObjectFunctionCall * c);
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
	bool setEventFilter(KviKvsObjectFunctionCall * c);
	bool jsChangeEvent(KviKvsObjectFunctionCall * c);
	bool jsSubmitEvent(KviKvsObjectFunctionCall * c);
	bool jsClickEvent(KviKvsObjectFunctionCall * c);
	bool jsMouseOverEvent(KviKvsObjectFunctionCall * c);
	bool jsMouseOutEvent(KviKvsObjectFunctionCall * c);
protected slots:
	void slotLoadFinished(bool);
	void slotLoadProgress(int);
	void slotLoadStarted();
	void slotDownloadRequest(const QNetworkRequest &);
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
	KviKvsDownloadHandler(KvsObject_webView * pParent, QFile * pFile, QNetworkReply * pNetReply, int iId);

	~KviKvsDownloadHandler();

protected:
	KvsObject_webView * m_pParentScript;
	QFile * m_pFile;
	QNetworkReply * m_pReply;
	int m_Id;
protected slots:
	void slotReadyRead();
	void slotReplyFinished();
};

#endif // COMPILE_WEBKIT_SUPPORT
#endif // _CLASS_WEBVIEW_H_
