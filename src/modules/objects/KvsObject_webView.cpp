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
#include "kvi_debug.h"
#include "KvsObject_webView.h"
#include "KvsObject_pixmap.h"
#include "KviError.h"
#include "KviLocale.h"

#if defined(COMPILE_WEBKIT_SUPPORT)
#include <QWebView>
#include <QWebSettings>
#include <QWebElement>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QPixmap>
#include <QFile>
#include <QSize>

static int g_iDownloadId = 1;
KviKvsDownloadHandler::KviKvsDownloadHandler(KvsObject_webView * pParent,QFile *pFile,QNetworkReply *pNetReply,int iId)
:QObject(pParent)
{
	m_Id = iId;
	m_pParentScript = pParent;
	m_pReply = pNetReply;
	m_pFile = pFile;
	connect(m_pReply,SIGNAL(finished()),this,SLOT(slotReplyFinished()));
	connect(m_pReply,SIGNAL(readyRead()),this,SLOT(slotReadyRead()));
}

void KviKvsDownloadHandler::slotReplyFinished()
{
	KviKvsVariantList params(new KviKvsVariant((kvs_int_t)m_Id));
	m_pParentScript->callFunction(m_pParentScript,"downloadCompletedEvent",&params);
	m_pFile->close();
	delete m_pFile;
	m_pFile = 0;
	m_pReply->deleteLater();
	m_pReply = 0;
	this->deleteLater();
}

KviKvsDownloadHandler::~KviKvsDownloadHandler()
{
	if(m_pFile)
	{
		m_pFile->close();
		delete m_pFile;
		m_pFile = 0;
	}
	if(m_pReply)
	{
		delete m_pReply;
		m_pReply = 0;
	}
}
void KviKvsDownloadHandler::slotReadyRead()
{
	QVariant vSize = m_pReply->header(QNetworkRequest::ContentLengthHeader);
	int iSize = 0;
	if(!vSize.isNull())
	{
		bool bOk;
		iSize = vSize.toInt(&bOk);
	}

	QByteArray bytes = m_pReply->readAll();
	KviKvsVariantList params(new KviKvsVariant((kvs_int_t)bytes.count()),\
		new KviKvsVariant((kvs_int_t)m_Id),new KviKvsVariant((kvs_int_t)iSize));
	m_pParentScript->callFunction(m_pParentScript,"downloadProgressEvent",&params);
	m_pFile->write(bytes);
}

const char * const webattributes_tbl[] = {
	"JavascriptEnabled",
#if (QT_VERSION >= 0x040700)
	"PluginsEnabled",
#endif
	"JavascriptCanOpenWindows",
	"JavascriptCanAccessClipboard",
	"ZoomTextOnly"
#if (QT_VERSION >= 0x040700)
	,
	"LocalContentCanAccessFileUrls"
#endif
};

const QWebSettings::WebAttribute webattributes_cod[] = {
	QWebSettings::JavascriptEnabled,
#if (QT_VERSION >= 0x040700)
	QWebSettings::PluginsEnabled,
#endif
	QWebSettings::JavascriptCanOpenWindows,
	QWebSettings::JavascriptCanAccessClipboard,
	QWebSettings::ZoomTextOnly
#if (QT_VERSION >= 0x040700)
	,
	QWebSettings::LocalContentCanAccessFileUrls
#endif
};

const char * const findflag_tbl[] = {
	"FindBackward",
	"FindCaseSensitively",
	"FindWrapsAroundDocument",
	"HighlightAllOccurrences"};

const QWebPage::FindFlag findflag_cod[] = {
	QWebPage::FindBackward,
	QWebPage::FindCaseSensitively,
	QWebPage::FindWrapsAroundDocument,
	QWebPage::HighlightAllOccurrences};

#define webattributes_num (sizeof(webattributes_tbl) / sizeof(webattributes_tbl[0]))
#define findflag_num (sizeof(findflag_tbl) / sizeof(findflag_tbl[0]))

/*
	@doc:	webview
	@keyterms:
		An embedded webview widget,
	@title:
		webview
	@type:
		class
	@short:
		Provides web support in a widget using webkit.
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		Provides an embedded web browser using webkit.
	@functions:
		!fn: $load(<url:string>)
		Sets the current url for the webView and starts loading it
		!fn: $findText(<txt:string>,[flag 1,flag 2,..;string])
		Finds the specified string, in the page, using the given options.
		Valid flags are:
		[pre]
		FindBackward		- Searches backwards instead of forwards;
		FindCaseSensitively	- Changes the behaviour to a case sensitive find operation.
		FindWrapsAroundDocument	- Restart from the beginning of the document if the end was reached and the text was not found.
		HighlightAllOccurrences	- Highlights all existing occurrences.
		[/pre]
		!fn: array $frames()
		Returns an array containing the names of the document frames.

		!fn: $firstChild()
		Sets the first child of the current element as the new current element.
		Returns true if the current element has been changed.

		!fn: $findAll(<query:string>)
		Searches for all the elements named <query> and stores them in a collection

		!fn: $findFirst(<query:string>)
		Searches for the first element named <query>; returns true if an element have been found, false if not.

		!fn: $parentElement()
		Sets the parent of the current element as the new current element.
		Returns true if the current element has been changed.
		
		!fn: $nextSibling()
		Sets the element just after the current one as the new current element.
		Returns true if the current element has been changed.
		!fn: $currentElementTagName()
		Returns the tag name of the current element
		!fn: $moveToQueryResultsAt(<index:int>)
		Changes the current element to the <index>th element of the collection
		!fn: int $queryResultsCount()
		Returns the number of elements in the collection
		!fn: $getDocumentElement([frame_name:string])
		Sets the document element of the frame [frame_name] as the current element.
		If no value has been specified for [frame_name] or [frame_name] is empty, the main frame of the page will be used.
		!fn: string $attributeNames()
		Returns a comma-separated list of the attribute names set on the current element
		!fn: string $attribute(<name:string>)
		Returns the value of the attribute <name> for the current element.
		!fn: pixmap $makePreview()
		Returns a 212x142 thumbnail of the current webView contants.
		The returned object is an instance of the pixmap class.
		!fn: string $rememberCurrent()
		Returns the id of the current web element and stores it in a cache
		!fn: $moveTo(<id:string>)
		Changes the current element to <id>
		!fn: string $toPlainText()
		Returns the string representation of the current element.
		!fn: string $setPlainText()
		Set the string representation of the current element
		!fn: $setAttribute(<name:string>,<value:string>)
		Sets the attribute <name> with value <value> to the current element
		!fn: $setWebSetting(<name:string>,<value:bool>)
		Enables or disables the <name> setting depending on <value>.
		Valid settings name: JavascriptEnabled, PluginsEnabled, JavascriptCanOpenWindows, JavascriptCanAccessClipboard, ZoomTextOnly, LocalContentCanAccessFileUrls.
		!fn: $removeFromDocument()
		Removes the current element from the document. The parent element is set as the new current element.
		!fn: $removeClass(<class_name:string>)
		Removes a class from the current element
		!fn: string $classes()
		Returns a comma-separated list of classes set on the current element
		!fn: $setLinkDelegationPolicy(<policy:string>)
		Sets the link delegation policy: what happens when the users click on a link. Valid values:
		[br] DontDelegateLinks: No links are delegated. Instead, webView tries to handle them all.
		[br] DelegateExternalLinks: When activating links that point to documents not stored on the local filesystem or an equivalent then $linkClickedEvent() is executed.
		[br] DelegateAllLinks: Whenever a link is activated the $linkClickedEvent() is executed.	
		!fn: $linkClickedEvent()
		This function can be called when the user clicks on a link, depending no the current link delegation policy.
		The argument of the function is the url that has been clicked.[br]
		The default implementation emits the [classfnc:webview]$linkClicked[/classfnc]() signal.
		!fn: $loadStartedEvent()
		This function is called when the load of the page has started.
		The default implementation emits the [classfnc:webview]$loadStarted[/classfnc]() signal.
		!fn: $loadProgressEvent()
		This function can be called during the page load progress.
		The argument of the function is an int value that represent the loading progress status, ranging from 0 to 100.[br]
		The default implementation emits the [classfnc:webview]$loadProgress[/classfnc]() signal.
		!fn: $loadFinishedEvent()
		This function is called when the load of the page has finished.
		The argument of the function is a bool value that is true if the page has been loaded successfully, false otherwise.[br]
		The default implementation emits the [classfnc:webview]$loadFinished[/classfnc]() signal.
		!fn: $downloadRequestEvent()
		This function is called when the user tries to download a file.
		The argument of the function is the url of the file.[br]
		You should return a valid path in the filesystem where to save the file.[br]
		The default implementation emits the [classfnc:webview]$downloadRequest[/classfnc]() signal.
		!fn: $downloadProgressEvent()
		This function can be called during the download of a file.
		Three integer arguments are passed to this function: the number of downloaded bytes, the download id, the size of the remove file (if known).
		The default implementation emits the [classfnc:webview]$downloadProgress[/classfnc]() signal.
		!fn: $downloadCompletedEvent()
		This function can be called when a file download finishes.
		The argument of the function is the an integer value containing the download id.
		The default implementation emits the [classfnc:webview]$downloadCompleted[/classfnc]() signal.
	@signals:
		!sg: linkClicked()
		This signal is emitted by the default implementation of [classfnc:webview]linkClickedEvent[/classfnc]().

		!sg: loadStarted()
		This signal is emitted by the default implementation of [classfnc:webview]loadStartedEvent[/classfnc]().

		!sg: loadProgress()
		This signal is emitted by the default implementation of [classfnc:webview]loadProgressEvent[/classfnc]().

		!sg: loadFinished()
		This signal is emitted by the default implementation of [classfnc:webview]loadFinishedEvent[/classfnc]().

		!sg: downloadRequest()
		This signal is emitted by the default implementation of [classfnc:webview]downloadRequestEvent[/classfnc]().

		!sg: downloadProgress()
		This signal is emitted by the default implementation of [classfnc:webview]downloadProgressEvent[/classfnc]().

		!sg: downloadCompleted()
		This signal is emitted by the default implementation of [classfnc:webview]downloadCompletedEvent[/classfnc]().
*/


KVSO_BEGIN_REGISTERCLASS(KvsObject_webView,"webview","widget")
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,load)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,frames)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,firstChild)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,findAll)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,findFirst)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,findText)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,parentElement)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,nextSibling)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,currentElementTagName)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,moveToQueryResultsAt)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,queryResultsCount)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,getDocumentElement)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,attributeNames)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,attribute)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,makePreview)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,rememberCurrent)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,moveTo)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,toPlainText)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,setPlainText)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,setAttribute)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,setWebSetting)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,removeFromDocument)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,removeClass)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,classes)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,setLinkDelegationPolicy)

	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,linkClickedEvent)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,loadFinishedEvent)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,loadProgressEvent)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,loadStartedEvent)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,downloadCompletedEvent)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,downloadRequestEvent)
	KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView,downloadProgressEvent)
KVSO_END_REGISTERCLASS(KvsObject_webView)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_webView,KviKvsObject)

KVSO_END_CONSTRUCTOR(KvsObject_webView)


KVSO_BEGIN_DESTRUCTOR(KvsObject_webView)
qDeleteAll(m_dictCache);
KVSO_END_CONSTRUCTOR(KvsObject_webView)

bool KvsObject_webView::init(KviKvsRunTimeContext *c ,KviKvsVariantList *)
{
	SET_OBJECT(QWebView);
	m_pContext = c;
	m_pNetworkManager = new QNetworkAccessManager(this);
	/* m_pReplyList=new KviPointerList<QNetworkReply>;
	m_pReplyList->setAutoDelete(false);*/
	QWebPage *pPage = ((QWebView *)widget())->page();
	connect(((QWebView *)widget()),SIGNAL(loadStarted()),this,SLOT(slotLoadStarted()));
	connect(((QWebView *)widget()),SIGNAL(loadFinished(bool)),this,SLOT(slotLoadFinished(bool)));
	connect(((QWebView *)widget()),SIGNAL(loadProgress(int)),this,SLOT(slotLoadProgress(int)));
	connect(pPage,SIGNAL(linkClicked(const QUrl &)),this,SLOT(slotLinkClicked(const QUrl &)));
	connect(pPage,SIGNAL(downloadRequested(const QNetworkRequest &)),this,SLOT(slotDownloadRequest(const QNetworkRequest &)));
	return true;
}

void KvsObject_webView::getFrames(QWebFrame *pFrame,KviKvsArray *pArray, kvs_uint_t &uIdx)
{
	m_dictFrames.insert(pFrame->title(),pFrame);
	pArray->set(uIdx,new KviKvsVariant(pFrame->title()));
	uIdx++;
	QList<QWebFrame *> lFrames = pFrame->childFrames();
	for(int i=0; i < lFrames.count(); i++)
	{
		QWebFrame *pChildFrame = lFrames.at(i);
		if(pChildFrame->childFrames().count())
			getFrames(pChildFrame,pArray,uIdx);
		else
		{
			pArray->set(uIdx,new KviKvsVariant(pChildFrame->title()));
			uIdx++;
		}
	}
}

KVSO_CLASS_FUNCTION(webView,setLinkDelegationPolicy)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szPolicy;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("policy",KVS_PT_STRING,0,szPolicy)
	KVSO_PARAMETERS_END(c)
	QWebPage::LinkDelegationPolicy policy = QWebPage::DontDelegateLinks;
	if(KviQString::equalCI(szPolicy,"DontDelegateLinks"))
		policy=QWebPage::DontDelegateLinks;
	else if(KviQString::equalCI(szPolicy,"DelegateExternalLinks"))
		policy=QWebPage::DelegateExternalLinks;
	else if(KviQString::equalCI(szPolicy,"DelegateAllLinks"))
		policy=QWebPage::DelegateAllLinks;
	else
		c->warning(__tr2qs_ctx("Unknown delegation policy '%Q'- Switch do default dontDelegateLinks","objects"),&szPolicy);
	((QWebView *)widget())->page()->setLinkDelegationPolicy(policy);
	return true;
}

KVSO_CLASS_FUNCTION(webView,load)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szUrl;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("url",KVS_PT_STRING,0,szUrl)
	KVSO_PARAMETERS_END(c)
	((QWebView *)widget())->load(QUrl(szUrl));
	return true;
}

KVSO_CLASS_FUNCTION(webView,makePreview)
{
	CHECK_INTERNAL_POINTER(widget())
	QSize size=((QWebView *)widget())->page()->mainFrame()->contentsSize();
	QImage *pImage=new QImage(212,142, QImage::Format_RGB32);
	QWebFrame  *pFrame=((QWebView *)widget())->page()->mainFrame();
	QPainter painter(pImage);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setRenderHint(QPainter::TextAntialiasing, true);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	double dWScale = size.width() > 0 ? (212.0 / (double)(size.width())) : 0;
	double dHScale = dWScale;
	if((size.height() * dWScale) < 142)
	{
		// solution 1 is to fill the missing bacgkround
		painter.fillRect(0,0,212,142,QColor(255,255,255));
		// solution 2 is to stretch the contents, but this doesn't work well because
		// the frame reports a size that doesn't exactly match the document
		//dHScale = 142.0 / (double)(size.height());
	}
	painter.scale(dWScale,dHScale);
	pFrame->documentElement().render(&painter);
	painter.end();
	KviKvsObjectClass * pClass = KviKvsKernel::instance()->objectController()->lookupClass("pixmap");
	KviKvsVariantList params;
	KviKvsObject * pObject = pClass->allocateInstance(0,"internalpixmap",c->context(),&params);
	((KvsObject_pixmap *)pObject)->setInternalImage(pImage);
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

KVSO_CLASS_FUNCTION(webView,rememberCurrent)
{
	QString adr=QString::number((long)&m_currentElement,16).toUpper();
	QWebElement *pElement = new QWebElement(m_currentElement);
	m_dictCache.insert(adr,pElement);
	c->returnValue()->setString(adr);
	return true;
}

KVSO_CLASS_FUNCTION(webView,moveTo)
{
	QString szId;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("id",KVS_PT_STRING,0,szId)
	KVSO_PARAMETERS_END(c)
	if(m_dictCache.value(szId))
		m_currentElement=*m_dictCache.value(szId);
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

KVSO_CLASS_FUNCTION(webView,removeFromDocument)
{
	Q_UNUSED(c);
	QWebElement element=m_currentElement.parent();
	m_currentElement.removeFromDocument();
	m_currentElement=element;
	return true;
}

KVSO_CLASS_FUNCTION(webView,queryResultsCount)
{
	CHECK_INTERNAL_POINTER(widget())
	c->returnValue()->setInteger((kvs_int_t) m_webElementCollection.count());
	return true;
}

KVSO_CLASS_FUNCTION(webView,frames)
{
	CHECK_INTERNAL_POINTER(widget())
	m_dictFrames.clear();
	QWebFrame *pFrame=((QWebView *)widget())->page()->mainFrame();

	//dd.toAscii().toHex()
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

KVSO_CLASS_FUNCTION(webView,removeClass)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szClassName;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("class_name",KVS_PT_NONEMPTYSTRING,0,szClassName)
	KVSO_PARAMETERS_END(c)
	m_currentElement.removeClass(szClassName);
	return true;
}

KVSO_CLASS_FUNCTION(webView,setWebSetting)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szName;
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("name",KVS_PT_NONEMPTYSTRING,0,szName)
		KVSO_PARAMETER("value",KVS_PT_BOOLEAN,0,bEnabled)
	KVSO_PARAMETERS_END(c)
	bool found=false;
	unsigned int j = 0;
	for(; j < webattributes_num; j++)
	{
		if(KviQString::equalCI(szName,webattributes_tbl[j]))
		{
			found=true;
			break;
		}
	}
	if (found)
		((QWebView *)widget())->settings()->setAttribute(webattributes_cod[j],bEnabled);
	else
		c->warning(__tr2qs_ctx("Unknown web setting '%Q'","objects"),&szName);
	return true;
}
KVSO_CLASS_FUNCTION(webView,findText)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szName;
	QStringList szFindFlag;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("find_text",KVS_PT_NONEMPTYSTRING,0,szName)
		KVSO_PARAMETER("find_flag",KVS_PT_STRINGLIST,KVS_PF_OPTIONAL,szFindFlag)
	KVSO_PARAMETERS_END(c)
	int findflag=0;
	int sum=0;
	for ( QStringList::Iterator it = szFindFlag.begin(); it != szFindFlag.end(); ++it )
	{
		findflag = 0;
		for(unsigned int j = 0; j < findflag_num; j++)
		{
			if(KviQString::equalCI((*it), findflag_tbl[j]))
			{
				findflag=findflag_cod[j];
				break;
			}
		}
		if(findflag)
			sum = sum | findflag;
		else
			c->warning(__tr2qs_ctx("Unknown findflag  '%Q'","objects"),&(*it));

	}
	((QWebView *)widget())->findText(szName,(QWebPage::FindFlags)findflag);
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
KVSO_CLASS_FUNCTION(webView,classes)
{
	CHECK_INTERNAL_POINTER(widget())
	if(m_currentElement.isNull())
	{
		c->warning(__tr2qs_ctx("Document element is null: you must call getDocumentElement first","objects"));
		return true;
	}
	QString szClasses;
	szClasses=m_currentElement.classes().join(",");
	c->returnValue()->setString(szClasses);
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
KVSO_CLASS_FUNCTION(webView,setPlainText)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szText;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("plaintext",KVS_PT_STRING,0,szText)
	KVSO_PARAMETERS_END(c)
	if(m_currentElement.isNull())
	{
		c->warning(__tr2qs_ctx("Document element is null: you must call getDocumentElement first","objects"));
		return true;
	}
	m_currentElement.setPlainText(szText);
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
	if(!element.isNull())
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

KVSO_CLASS_FUNCTION(webView,downloadCompletedEvent)
{
	emitSignal("downloadCompleted",c,c->params());
	return true;
}

KVSO_CLASS_FUNCTION(webView,downloadProgressEvent)
{
	emitSignal("downloadProgress",c,c->params());
	return true;
}

KVSO_CLASS_FUNCTION(webView,linkClickedEvent)
{
	emitSignal("linkClicked",c,c->params());
	return true;
}

KVSO_CLASS_FUNCTION(webView,downloadRequestEvent)
{
	emitSignal("downloadRequest",c,c->params());
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

void KvsObject_webView::slotLinkClicked(const QUrl &url)
{
	QString szUrl=url.toString();
	KviKvsVariantList params(new KviKvsVariant(szUrl));
	callFunction(this,"linkClickedEvent" ,&params);
}

void KvsObject_webView::slotDownloadRequest(const QNetworkRequest &r)
{
	QNetworkReply *pReply=m_pNetworkManager->get(r);
	QString szFilePath="";
	KviKvsVariant *filepathret=new KviKvsVariant(szFilePath);
	KviKvsVariantList params(new KviKvsVariant(r.url().toString()));
	callFunction(this,"downloadRequestEvent",filepathret,&params);
	filepathret->asString(szFilePath);
	if (!szFilePath.isEmpty())
	{
		QFile *pFile=new QFile(szFilePath);
		if(!pFile->open(QIODevice::WriteOnly))
		{
			m_pContext->warning(__tr2qs_ctx("Invalid file path '%Q'","objects"),&szFilePath);
			pReply->abort();
			pReply->deleteLater();
			return;
		}
		KviKvsDownloadHandler *pHandler = new KviKvsDownloadHandler(this,pFile,pReply,g_iDownloadId);
		Q_UNUSED(pHandler);
		g_iDownloadId++;
	}
}

#endif // COMPILE_WEBKIT_SUPPORT
