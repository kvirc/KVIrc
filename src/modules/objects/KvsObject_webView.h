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
#ifdef COMPILE_WEBKIT_SUPPORT
#include "object_macros.h"
#include <QtWebKit/QWebView>
#include <QtWebKit/QWebFrame>
#include <QWebElement>
class KvsObject_webView : public KviKvsObject
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KvsObject_webView)
public:
	QWidget * widget() { return (QWidget *)object(); }
protected:
	QWebPage * m_pWebPage;
	QHash<QString,QWebFrame *> m_dictFrames;
	QHash<QString,QWebElement *> m_dictCache;

	QList<QWebElement> lStack;
	QWebElementCollection m_webElementCollection;
	QWebElement m_currentElement;
	void getFrames(QWebFrame *pFrame,KviKvsArray *n, kvs_uint_t & uIdx);
	virtual bool init(KviKvsRunTimeContext * pContext,KviKvsVariantList *pParams);

	bool removeFromDocument(KviKvsObjectFunctionCall *c);
	bool makePreview(KviKvsObjectFunctionCall *c);

	bool rememberCurrent(KviKvsObjectFunctionCall *c);
	bool moveTo(KviKvsObjectFunctionCall *c);


	bool firstChild(KviKvsObjectFunctionCall *c);
	bool parentElement(KviKvsObjectFunctionCall *c);
	bool nextSibling(KviKvsObjectFunctionCall *c);

	bool toPlainText(KviKvsObjectFunctionCall *c);
	bool load(KviKvsObjectFunctionCall *c);
	bool getDocumentElement(KviKvsObjectFunctionCall *c);
	bool currentElementTagName(KviKvsObjectFunctionCall *c);
	bool findAll(KviKvsObjectFunctionCall *c);
	bool findFirst(KviKvsObjectFunctionCall *c);

	bool moveToQueryResultsAt(KviKvsObjectFunctionCall *c);
	bool queryResultsCount(KviKvsObjectFunctionCall *c);

	bool attributeNames(KviKvsObjectFunctionCall *c);
	bool setAttribute(KviKvsObjectFunctionCall *c);
	bool setWebSetting(KviKvsObjectFunctionCall *c);
	bool attribute(KviKvsObjectFunctionCall *c);
	bool frames(KviKvsObjectFunctionCall *c);

	bool loadStartedEvent(KviKvsObjectFunctionCall *c);
	bool loadFinishedEvent(KviKvsObjectFunctionCall *c);
	bool loadProgressEvent(KviKvsObjectFunctionCall *c);


protected slots:
	void slotLoadFinished(bool);
	void slotLoadProgress(int);
	void slotLoadStarted();
};
#endif
#endif	//!_CLASS_WEBVIEW_H_
