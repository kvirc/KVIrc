//=============================================================================
//
//   File : KvsObject_webView.cpp
//   Creation date : Wed Mar 9 23:34:48 CEST 2011
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
//

#include "kvi_settings.h"
#ifdef COMPILE_WEBKIT_SUPPORT
#include <QtWebKit/QWebView>
#include <QWebElement>
#include <QPixmap>
#include <QSize>
#include "KvsObject_webView.h"
#include "KvsObject_pixmap.h"
#include "KviError.h"
#include "kvi_debug.h"
#include "KviLocale.h"

/*
	@doc:	webview
	@keyterms:
		An embedded webview widget,
	@title:

	@type:
		class
	@short:
		Provides web support in a widget using webkit.
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:

	@functions:
		!fn: $load()

*/


KVSO_BEGIN_REGISTERCLASS(KvsObject_webView,"webview","widget")
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,load)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,frames)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,firstChild)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,findAll)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,findFirst)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,pushCurrentToStack)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,popCurrentFromStack)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,clearElementStack)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,parentElement)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,nextSibling)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,currentElementTagName)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,moveToQueryResultsAt)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,elementStackCount)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,queryResultsCount)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,getDocumentElement)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,attributeNames)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,attribute)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,makePreview)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,toPlainText)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,setAttribute)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,loadFinishedEvent)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,loadProgressEvent)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,loadStartedEvent)
KVSO_END_REGISTERCLASS(KvsObject_webView)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_webView,KviKvsObject)

KVSO_END_CONSTRUCTOR(KvsObject_webView)


KVSO_BEGIN_DESTRUCTOR(KvsObject_webView)
lStack.clear();
KVSO_END_CONSTRUCTOR(KvsObject_webView)

bool KvsObject_webView::init(KviKvsRunTimeContext * ,KviKvsVariantList *)
{
    SET_OBJECT(QWebView);
    // fixme: this is for testing purpose only
    ((QWebView *)widget())->settings()->setAttribute(QWebSettings::PluginsEnabled,true);
    //
    connect(((QWebView *)widget()),SIGNAL(loadStarted()),this,SLOT(slotLoadStarted()));
    connect(((QWebView *)widget()),SIGNAL(loadFinished(bool)),this,SLOT(slotLoadFinished(bool)));
    connect(((QWebView *)widget()),SIGNAL(loadProgress(int)),this,SLOT(slotLoadProgress(int)));
    return true;
}
void KvsObject_webView::getFrames(QWebFrame *pFrame,KviKvsArray *pArray, kvs_uint_t &uIdx)
{
    m_dictFrames.insert(pFrame->title(),pFrame);
    pArray->set(uIdx,new KviKvsVariant(pFrame->title()));
    uIdx++;
    QList<QWebFrame *> lFrames = pFrame->childFrames();
    for(int i=0;i<lFrames.count();i++)
    {
	QWebFrame *pChildFrame = lFrames.at(i);
	if (pChildFrame->childFrames().count()) getFrames(pChildFrame,pArray,uIdx);
	else
	{
	    pArray->set(uIdx,new KviKvsVariant(pChildFrame->title()));
	    uIdx++;
	}
    }
}

KVSO_CLASS_FUNCTION(webView,load)
{
    CHECK_INTERNAL_POINTER(widget())
    QString szUrl;
    KVSO_PARAMETERS_BEGIN(c)
    	KVSO_PARAMETER("text",KVS_PT_STRING,0,szUrl)
    KVSO_PARAMETERS_END(c)
    ((QWebView *)widget())->load(QUrl(szUrl));
    return true;
}
KVSO_CLASS_FUNCTION(webView,makePreview)
{
    CHECK_INTERNAL_POINTER(widget())
    QSize size=((QWebView *)widget())->page()->mainFrame()->contentsSize();
    qDebug("contents size %d - %d",size.width(),size.height());
    QImage image(212,142, QImage::Format_RGB32);
    QWebFrame  *pFrame=((QWebView *)widget())->page()->mainFrame();
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setClipRect(0,0,size.width(),768);
    painter.scale(0.20,0.34);
    pFrame->documentElement().render(&painter);
    painter.end();
    QPixmap *pPixmap=new QPixmap();
    *pPixmap=pPixmap->fromImage(image);
    KviKvsObjectClass * pClass = KviKvsKernel::instance()->objectController()->lookupClass("pixmap");
    KviKvsVariantList params;
    KviKvsObject * pObject = pClass->allocateInstance(0,"internalpixmap",c->context(),&params);
    ((KvsObject_pixmap *)pObject)->setInternalPixmap(pPixmap);
    c->returnValue()->setHObject(pObject->handle());
    return true;
}
KVSO_CLASS_FUNCTION(webView,findAll)
{
    CHECK_INTERNAL_POINTER(widget())
    QString szQuery;
    KVSO_PARAMETERS_BEGIN(c)
    	KVSO_PARAMETER("query",KVS_PT_STRING,0,szQuery)
    KVSO_PARAMETERS_END(c)
    m_webElementCollection=m_currentElement.findAll(szQuery);
    return true;
}
KVSO_CLASS_FUNCTION(webView,findFirst)
{
    QString szQuery;
    KVSO_PARAMETERS_BEGIN(c)
    	KVSO_PARAMETER("query",KVS_PT_STRING,0,szQuery)
    KVSO_PARAMETERS_END(c)
    QWebElement tempElement=m_currentElement.findFirst(szQuery);
    if(!tempElement.isNull())
    {
	m_currentElement=tempElement;
	c->returnValue()->setBoolean(true);
    }
    else c->returnValue()->setBoolean(false);

    return true;
}
KVSO_CLASS_FUNCTION(webView,moveToQueryResultsAt)
{
    CHECK_INTERNAL_POINTER(widget())
    kvs_int_t iIdx;
    KVSO_PARAMETERS_BEGIN(c)
    	KVSO_PARAMETER("index",KVS_PT_INTEGER,0,iIdx)
    KVSO_PARAMETERS_END(c)
    m_currentElement=m_webElementCollection.at(iIdx);
    return true;
}
KVSO_CLASS_FUNCTION(webView,elementStackCount)
{
    CHECK_INTERNAL_POINTER(widget())
    c->returnValue()->setInteger((kvs_int_t) lStack.count());
    return true;
}
KVSO_CLASS_FUNCTION(webView,clearElementStack)
{
    Q_UNUSED(c);
    lStack.clear();
    return true;
}

KVSO_CLASS_FUNCTION(webView,queryResultsCount)
{
    CHECK_INTERNAL_POINTER(widget())
    c->returnValue()->setInteger((kvs_int_t) m_webElementCollection.count());
    return true;
}
KVSO_CLASS_FUNCTION(webView,pushCurrentToStack)
{
    Q_UNUSED(c);
    lStack.append(m_currentElement);
    return true;
}
KVSO_CLASS_FUNCTION(webView,popCurrentFromStack)
{
    if(!lStack.isEmpty())
	m_currentElement = lStack.takeLast();
    else c->warning(__tr2qs_ctx("Stack is empty","objects"));
    return true;
}
KVSO_CLASS_FUNCTION(webView,frames)
{
    CHECK_INTERNAL_POINTER(widget())
    m_dictFrames.clear();
    QWebFrame *pFrame=((QWebView *)widget())->page()->mainFrame();
    KviKvsArray *pArray=new KviKvsArray();
    kvs_uint_t uIdx=0;
    getFrames(pFrame,pArray,uIdx);
    c->returnValue()->setArray(pArray);
    return true;
}
KVSO_CLASS_FUNCTION(webView,currentElementTagName)
{
    CHECK_INTERNAL_POINTER(widget())
    if(m_currentElement.isNull())
    {
	c->warning(__tr2qs_ctx("Document element is null: you must call getDocumentElement first","objects"));
	return true;
    }
    c->returnValue()->setString(m_currentElement.tagName());
    return true;
}

KVSO_CLASS_FUNCTION(webView,setAttribute)
{
    CHECK_INTERNAL_POINTER(widget())
    QString szName,szValue;
    KVSO_PARAMETERS_BEGIN(c)
    	KVSO_PARAMETER("name",KVS_PT_NONEMPTYSTRING,0,szName)
	KVSO_PARAMETER("value",KVS_PT_STRING,0,szValue)
    KVSO_PARAMETERS_END(c)
    m_currentElement.setAttribute(szName,szValue);
    return true;
}
KVSO_CLASS_FUNCTION(webView,attribute)
{
    CHECK_INTERNAL_POINTER(widget())
    QString szName;
    KVSO_PARAMETERS_BEGIN(c)
    	KVSO_PARAMETER("name",KVS_PT_NONEMPTYSTRING,0,szName)
    KVSO_PARAMETERS_END(c)
    c->returnValue()->setString(m_currentElement.attribute(szName));
    return true;
}
KVSO_CLASS_FUNCTION(webView,attributeNames)
{
    CHECK_INTERNAL_POINTER(widget())
    if(m_currentElement.isNull())
    {
	c->warning(__tr2qs_ctx("Document element is null: you must call getDocumentElement first","objects"));
	return true;
    }
    QString szAttributeNames;
    szAttributeNames=m_currentElement.attributeNames().join(",");
    c->returnValue()->setString(szAttributeNames);
    return true;
}

KVSO_CLASS_FUNCTION(webView,toPlainText)
{
    CHECK_INTERNAL_POINTER(widget())
    if(m_currentElement.isNull())
    {
	c->warning(__tr2qs_ctx("Document element is null: you must call getDocumentElement first","objects"));
	return true;
    }
    c->returnValue()->setString(m_currentElement.toPlainText());
    return true;
}

KVSO_CLASS_FUNCTION(webView,firstChild)
{
    CHECK_INTERNAL_POINTER(widget())
    if(m_currentElement.isNull())
    {
	c->warning(__tr2qs_ctx("Document element is null: you must call getDocumentElement first","objects"));
	return true;
    }
    if (!m_currentElement.firstChild().isNull())
    {
	m_currentElement=m_currentElement.firstChild();
	c->returnValue()->setBoolean(true);
	return true;
    }
    c->returnValue()->setBoolean(false);
    return true;
}
KVSO_CLASS_FUNCTION(webView,parentElement)
{
    CHECK_INTERNAL_POINTER(widget())
    if(m_currentElement.isNull())
    {
	c->warning(__tr2qs_ctx("Document element is null: you must call getDocumentElement first","objects"));
	return true;
    }
    QWebElement element=m_currentElement.parent();
    if (!element.isNull())
    {
	m_currentElement=element;
	c->returnValue()->setBoolean(true);
	return true;
    }
    c->returnValue()->setBoolean(false);
    return true;
}
KVSO_CLASS_FUNCTION(webView,nextSibling)
{
    CHECK_INTERNAL_POINTER(widget())
    if(m_currentElement.isNull())
    {
	c->warning(__tr2qs_ctx("Document element is null: you must call getDocumentElement first","objects"));
	return true;
    }
    QWebElement element=m_currentElement.nextSibling();
    if (!element.isNull())
    {
	m_currentElement=element;
	c->returnValue()->setBoolean(true);
	return true;
    }
    c->returnValue()->setBoolean(false);
    return true;
}


KVSO_CLASS_FUNCTION(webView,getDocumentElement)
{
    CHECK_INTERNAL_POINTER(widget())
    QString szFrameName;
    KVSO_PARAMETERS_BEGIN(c)
	    KVSO_PARAMETER("frame_name",KVS_PT_STRING,KVS_PF_OPTIONAL,szFrameName)
    KVSO_PARAMETERS_END(c)
    QWebFrame *pFrame;
    if(szFrameName.isEmpty())
	pFrame=((QWebView *)widget())->page()->mainFrame();
    else{
	pFrame=m_dictFrames.value(szFrameName);
	if(!pFrame)
	{
	    c->warning(__tr2qs_ctx("Unknown frame '%Q'","objects"),&szFrameName);
	    return true;
	}
    }
    m_currentElement=pFrame->documentElement();
  //  m_documentElement=m_currentElement;
    return true;
}



KVSO_CLASS_FUNCTION(webView,loadFinishedEvent)
{
	emitSignal("loadFinished",c,c->params());
	return true;
}

KVSO_CLASS_FUNCTION(webView,loadProgressEvent)
{
	emitSignal("loadProgress",c,c->params());
	return true;
}
KVSO_CLASS_FUNCTION(webView,loadStartedEvent)
{
	emitSignal("loadStarted",c);
	return true;
}

// slots
void KvsObject_webView::slotLoadFinished(bool bOk)
{
	if (bOk)
	    m_currentElement=((QWebView *)widget())->page()->mainFrame()->documentElement();
	KviKvsVariantList params(new KviKvsVariant(bOk));
	callFunction(this,"loadFinishedEvent",&params);
}

void KvsObject_webView::slotLoadStarted()
{
	KviKvsVariantList *lParams=0;
	callFunction(this,"loadStartedEvent",lParams);
}
void KvsObject_webView::slotLoadProgress(int iProgress)
{
	KviKvsVariantList params(new KviKvsVariant((kvs_int_t) iProgress));
	callFunction(this,"loadProgressEvent" ,&params);
}

#endif

