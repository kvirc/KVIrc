//=============================================================================
//
//   File : KvsObject_webView.cpp
//   Creation date : Wed Mar 9 23:34:48 CEST 2011
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
//

#include "kvi_settings.h"
#include "kvi_debug.h"
#include "KvsObject_webView.h"
#include "KvsObject_pixmap.h"
#include "KviError.h"
#include "KviLocale.h"

#include <QMenu>

#if defined(COMPILE_WEBENGINE_SUPPORT)
#include <QWebEngineProfile>
#include <QWebEngineView>
#include <QWebEngineSettings>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QWebEngineDownloadItem>
#else
#include <QWebEngineDownloadRequest>
#endif
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QPixmap>
#include <QFileInfo>
#include <QSize>
#include <QPoint>
#include <QVariant>
#include <QToolTip>

static int g_iDownloadId = 1;
KviKvsDownloadHandler::KviKvsDownloadHandler(KvsObject_webView * pParent, QString &szFilePath, DOWNLOAD_CLASS_NAME * pDownload, int iId)
    : QObject(pParent)
{
	m_Id = iId;
	m_pParentScript = pParent;
	m_pDownload = pDownload;
	connect(m_pDownload, SIGNAL(finished()), this, SLOT(slotFinished()));
	connect(m_pDownload, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(slotDownloadProgress(qint64, qint64)));

	QFileInfo oFi(szFilePath);
	m_pDownload->setDownloadDirectory(oFi.absolutePath());
	m_pDownload->setDownloadFileName(oFi.fileName());
	m_pDownload->accept();
}

void KviKvsDownloadHandler::slotFinished()
{
	KviKvsVariantList params(new KviKvsVariant((kvs_int_t)m_Id));
	m_pParentScript->callFunction(m_pParentScript, "downloadCompletedEvent", &params);
	m_pDownload->deleteLater();
	m_pDownload = nullptr;
	this->deleteLater();
}

KviKvsDownloadHandler::~KviKvsDownloadHandler()
{
	if(m_pDownload)
	{
		delete m_pDownload;
		m_pDownload = nullptr;
	}
}
void KviKvsDownloadHandler::slotDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	KviKvsVariantList params(new KviKvsVariant((kvs_int_t)bytesReceived),
	    new KviKvsVariant((kvs_int_t)m_Id), new KviKvsVariant((kvs_int_t)bytesTotal));
	m_pParentScript->callFunction(m_pParentScript, "downloadProgressEvent", &params);
}

const char * const webattributes_tbl[] = {
	"AutoLoadImages",
	"JavascriptEnabled",
	"JavascriptCanOpenWindows",
	"JavascriptCanAccessClipboard",
	"LinksIncludedInFocusChain",
	"LocalStorageEnabled",
	"LocalContentCanAccessRemoteUrls",
	"XSSAuditingEnabled",
	"SpatialNavigationEnabled",
	"LocalContentCanAccessFileUrls",
	"HyperlinkAuditingEnabled",
	"ScrollAnimatorEnabled",
	"ErrorPageEnabled",
	"PluginsEnabled",
	"FullScreenSupportEnabled",
	"ScreenCaptureEnabled",
	"WebGLEnabled",
	"Accelerated2dCanvasEnabled",
	"AutoLoadIconsForPage",
	"TouchIconsEnabled",
	"FocusOnNavigationEnabled",
	"PrintElementBackgrounds",
	"AllowRunningInsecureContent",
	"AllowGeolocationOnInsecureOrigins",
	"AllowWindowActivationFromJavaScript",
	"ShowScrollBars",
	"PlaybackRequiresUserGesture",
	"JavascriptCanPaste",
	"WebRTCPublicInterfacesOnly",
	"DnsPrefetchEnabled",
	"PdfViewerEnabled"
};

const QWebEngineSettings::WebAttribute webattributes_cod[] = {
	QWebEngineSettings::AutoLoadImages,
	QWebEngineSettings::JavascriptEnabled,
	QWebEngineSettings::JavascriptCanOpenWindows,
	QWebEngineSettings::JavascriptCanAccessClipboard,
	QWebEngineSettings::LinksIncludedInFocusChain,
	QWebEngineSettings::LocalStorageEnabled,
	QWebEngineSettings::LocalContentCanAccessRemoteUrls,
	QWebEngineSettings::XSSAuditingEnabled,
	QWebEngineSettings::SpatialNavigationEnabled,
	QWebEngineSettings::LocalContentCanAccessFileUrls,
	QWebEngineSettings::HyperlinkAuditingEnabled,
	QWebEngineSettings::ScrollAnimatorEnabled,
	QWebEngineSettings::ErrorPageEnabled,
	QWebEngineSettings::PluginsEnabled,
	QWebEngineSettings::FullScreenSupportEnabled,
	QWebEngineSettings::ScreenCaptureEnabled,
	QWebEngineSettings::WebGLEnabled,
	QWebEngineSettings::Accelerated2dCanvasEnabled,
	QWebEngineSettings::AutoLoadIconsForPage,
	QWebEngineSettings::TouchIconsEnabled,
	QWebEngineSettings::FocusOnNavigationEnabled,
	QWebEngineSettings::PrintElementBackgrounds,
	QWebEngineSettings::AllowRunningInsecureContent,
	QWebEngineSettings::AllowGeolocationOnInsecureOrigins,
	QWebEngineSettings::AllowWindowActivationFromJavaScript,
	QWebEngineSettings::ShowScrollBars,
	QWebEngineSettings::PlaybackRequiresUserGesture,
	QWebEngineSettings::JavascriptCanPaste,
	QWebEngineSettings::WebRTCPublicInterfacesOnly,
	QWebEngineSettings::DnsPrefetchEnabled,
	QWebEngineSettings::PdfViewerEnabled
};

const char * const findflag_tbl[] = {
	"FindBackward",
	"FindCaseSensitively"
};

const QWebEnginePage::FindFlag findflag_cod[] = {
	QWebEnginePage::FindBackward,
	QWebEnginePage::FindCaseSensitively
};

const QWebEnginePage::WebAction actions_cod[] = {
	QWebEnginePage::NoWebAction,
	QWebEnginePage::Back,
	QWebEnginePage::Forward,
	QWebEnginePage::Stop,
	QWebEnginePage::Reload,
	QWebEnginePage::ReloadAndBypassCache,
	QWebEnginePage::Cut,
	QWebEnginePage::Copy,
	QWebEnginePage::Paste,
	QWebEnginePage::Undo,
	QWebEnginePage::Redo,
	QWebEnginePage::SelectAll,
	QWebEnginePage::PasteAndMatchStyle,
	QWebEnginePage::OpenLinkInThisWindow,
	QWebEnginePage::OpenLinkInNewWindow,
	QWebEnginePage::OpenLinkInNewTab,
	QWebEnginePage::OpenLinkInNewBackgroundTab,
	QWebEnginePage::CopyLinkToClipboard,
	QWebEnginePage::CopyImageToClipboard,
	QWebEnginePage::CopyImageUrlToClipboard,
	QWebEnginePage::CopyMediaUrlToClipboard,
	QWebEnginePage::ToggleMediaControls,
	QWebEnginePage::ToggleMediaLoop,
	QWebEnginePage::ToggleMediaPlayPause,
	QWebEnginePage::ToggleMediaMute,
	QWebEnginePage::DownloadLinkToDisk,
	QWebEnginePage::DownloadImageToDisk,
	QWebEnginePage::DownloadMediaToDisk,
	QWebEnginePage::InspectElement,
	QWebEnginePage::ExitFullScreen,
	QWebEnginePage::RequestClose,
	QWebEnginePage::Unselect,
	QWebEnginePage::SavePage,
	QWebEnginePage::ViewSource,
	QWebEnginePage::ToggleBold,
	QWebEnginePage::ToggleItalic,
	QWebEnginePage::ToggleUnderline,
	QWebEnginePage::ToggleStrikethrough,
	QWebEnginePage::AlignLeft,
	QWebEnginePage::AlignCenter,
	QWebEnginePage::AlignRight,
	QWebEnginePage::AlignJustified,
	QWebEnginePage::Indent,
	QWebEnginePage::Outdent,
	QWebEnginePage::InsertOrderedList,
	QWebEnginePage::InsertUnorderedList
};

const char * const actions_tbl[] = {
	"NoWebAction",
	"Back",
	"Forward",
	"Stop",
	"Reload",
	"ReloadAndBypassCache",
	"Cut",
	"Copy",
	"Paste",
	"Undo",
	"Redo",
	"SelectAll",
	"PasteAndMatchStyle",
	"OpenLinkInThisWindow",
	"OpenLinkInNewWindow",
	"OpenLinkInNewTab",
	"OpenLinkInNewBackgroundTab",
	"CopyLinkToClipboard",
	"CopyImageToClipboard",
	"CopyImageUrlToClipboard",
	"CopyMediaUrlToClipboard",
	"ToggleMediaControls",
	"ToggleMediaLoop",
	"ToggleMediaPlayPause",
	"ToggleMediaMute",
	"DownloadLinkToDisk",
	"DownloadImageToDisk",
	"DownloadMediaToDisk",
	"InspectElement",
	"ExitFullScreen",
	"RequestClose",
	"Unselect",
	"SavePage",
	"ViewSource",
	"ToggleBold",
	"ToggleItalic",
	"ToggleUnderline",
	"ToggleStrikethrough",
	"AlignLeft",
	"AlignCenter",
	"AlignRight",
	"AlignJustified",
	"Indent",
	"Outdent",
	"InsertOrderedList",
	"InsertUnorderedList"
};

#define webattributes_num (sizeof(webattributes_tbl) / sizeof(webattributes_tbl[0]))
#define findflag_num (sizeof(findflag_tbl) / sizeof(findflag_tbl[0]))
#define actions_num (sizeof(actions_tbl) / sizeof(actions_tbl[0]))

/*
	@doc:	webview
	@keyterms:
		An embedded webview widget,
	@title:
		webview
	@type:
		class
	@short:
		Provides web support in a widget using webengine.
	@inherits:
		[class]object[/class]
		[class]widget[/class]
	@description:
		Provides an embedded web browser using webengine. Page structure can be managed by web element's unique identifiers.
	@functions:
		!fn: $load(<url:string>)
		Sets the current URL for the webView and starts loading it
		!fn: $setHtml(<html_code:string>,[<baseurl:string>])
		Sets the content of the web view to the specified <html_code>.
		External objects such as stylesheets or images referenced in the HTML document are located relative to <baseurl>.
		The html is loaded immediately; external objects are loaded asynchronously.
		!fn: $findText(<txt:string>,[flag 1,flag 2,..;string])
		Finds the specified string, in the page, using the given options.
		Valid flags are:
		[pre]
			FindBackward		- Searches backwards instead of forwards;
			FindCaseSensitively	- Changes the behaviour to a case sensitive find operation.
		[/pre]
		!fn: appendWebViewActionToMenu(<menu:h_object>,<webview_action:string>,[<icon_identifier>])
		!fn: pixmap $makePreview()
		Returns a 212x142 thumbnail of the current webView constants.
		The returned object is an instance of the pixmap class.
		!fn: $setWebSetting(<name:string>,<value:bool>)
		Enables or disables the <name> setting depending on <value>.
		Valid settings name: JavascriptEnabled, PluginsEnabled, JavascriptCanOpenWindows, JavascriptCanAccessClipboard, ZoomTextOnly, LocalContentCanAccessFileUrls.
		!fn: $setLinkDelegationPolicy(<policy:string>)
		Sets the link delegation policy: what happens when the users click on a link. Valid values:
		[br] DontDelegateLinks: No links are delegated. Instead, webView tries to handle them all.
		[br] DelegateAllLinks: Whenever a link is activated the $linkClickedEvent() is executed.
		!fn: $linkClickedEvent()
		This function can be called when the user clicks on a link, depending no the current link delegation policy.
		The argument of the function is the URL that has been clicked.[br]
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
		The argument of the function is the URL of the file.[br]
		You should return a valid path in the filesystem where to save the file.[br]
		The default implementation emits the [classfnc:webview]$downloadRequest[/classfnc]() signal.
		!fn: $downloadProgressEvent()
		This function can be called during the download of a file.
		Three integer arguments are passed to this function: the number of downloaded bytes, the download ID, the size of the remove file (if known).
		The default implementation emits the [classfnc:webview]$downloadProgress[/classfnc]() signal.
		!fn: $downloadCompletedEvent()
		This function can be called when a file download finishes.
		The argument of the function is the an integer value containing the download ID.
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

KVSO_BEGIN_REGISTERCLASS(KvsObject_webView, "webview", "widget")

// page related
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView, load)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView, setHtml)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView, makePreview)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView, appendWebViewActionToMenu)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView, setLinkDelegationPolicy)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView, findText)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView, evaluateJavaScript)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView, setWebSetting)

// events
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView, jsChangeEvent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView, jsSubmitEvent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView, jsClickEvent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView, jsMouseOverEvent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView, jsMouseOutEvent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView, linkClickedEvent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView, loadFinishedEvent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView, loadProgressEvent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView, loadStartedEvent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView, downloadCompletedEvent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView, downloadRequestEvent)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_webView, downloadProgressEvent)

KVSO_END_REGISTERCLASS(KvsObject_webView)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_webView, KviKvsObject)

KVSO_END_CONSTRUCTOR(KvsObject_webView)

KVSO_BEGIN_DESTRUCTOR(KvsObject_webView)

KVSO_END_CONSTRUCTOR(KvsObject_webView)

bool KvsObject_webView::init(KviKvsRunTimeContext * c, KviKvsVariantList *)
{
	//SET_OBJECT(KviKvsWebview);
	KviKvsWebView *pView = new KviKvsWebView(parentScriptWidget(), getName().toUtf8().data(), this);
	setObject(pView, true);
	KviKvsWebViewPage *pPage = new KviKvsWebViewPage(pView);
	pView->setPage(pPage);
	m_pContext = c;
	m_pNetworkManager = new QNetworkAccessManager(this);
	connect(pView, SIGNAL(loadStarted()), this, SLOT(slotLoadStarted()));
	connect(pView, SIGNAL(loadFinished(bool)), this, SLOT(slotLoadFinished(bool)));
	connect(pView, SIGNAL(loadProgress(int)), this, SLOT(slotLoadProgress(int)));
	connect(pPage, SIGNAL(linkClicked(const QUrl &)), this, SLOT(slotLinkClicked(const QUrl &)));
	connect(QWebEngineProfile::defaultProfile(), SIGNAL(downloadRequested(DOWNLOAD_CLASS_NAME*)),
                this, SLOT(slotDownloadRequest(DOWNLOAD_CLASS_NAME*)));
	return true;
}

KVSO_CLASS_FUNCTION(webView, setLinkDelegationPolicy)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szPolicy;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("policy", KVS_PT_STRING, 0, szPolicy)
	KVSO_PARAMETERS_END(c)
	KviKvsWebViewPage::LinkDelegationPolicy policy = KviKvsWebViewPage::DontDelegateLinks;
	if(KviQString::equalCI(szPolicy, "DontDelegateLinks"))
		policy = KviKvsWebViewPage::DontDelegateLinks;
	else if(KviQString::equalCI(szPolicy, "DelegateAllLinks"))
		policy = KviKvsWebViewPage::DelegateAllLinks;
	else
		c->warning(__tr2qs_ctx("Unknown delegation policy '%Q'- Switch do default dontDelegateLinks", "objects"), &szPolicy);

	((KviKvsWebViewPage *)((QWebEngineView *)widget())->page())->setLinkDelegationPolicy(policy);
	return true;
}

KVSO_CLASS_FUNCTION(webView, load)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szUrl;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("url", KVS_PT_STRING, 0, szUrl)
	KVSO_PARAMETERS_END(c)
	((QWebEngineView *)widget())->load(QUrl::fromUserInput(szUrl));
	return true;
}
KVSO_CLASS_FUNCTION(webView, setHtml)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szHtml, szUrlBase;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("szHtml", KVS_PT_STRING, 0, szHtml)
	KVSO_PARAMETER("urlbase", KVS_PT_STRING, KVS_PF_OPTIONAL, szUrlBase)
	KVSO_PARAMETERS_END(c)
	((QWebEngineView *)widget())->setHtml(szHtml, QUrl(szUrlBase));
	return true;
}
KVSO_CLASS_FUNCTION(webView, makePreview)
{
	CHECK_INTERNAL_POINTER(widget())
	//QSizeF size = ((QWebEngineView *)widget())->page()->contentsSize();
	QImage * pImage = new QImage(212, 142, QImage::Format_RGB32);
	((QWebEngineView *)widget())->render(pImage);
	KviKvsObjectClass * pClass = KviKvsKernel::instance()->objectController()->lookupClass("pixmap");
	KviKvsVariantList params;
	KviKvsObject * pObject = pClass->allocateInstance(nullptr, "internalpixmap", c->context(), &params);
	((KvsObject_pixmap *)pObject)->setInternalImage(pImage);
	c->returnValue()->setHObject(pObject->handle());
	return true;
}

KVSO_CLASS_FUNCTION(webView, evaluateJavaScript)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szScript;
	kvs_int_t iEleId;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("script_code", KVS_PT_STRING, 0, szScript)
	KVSO_PARAMETERS_END(c)
	((QWebEngineView *)widget())->page()->runJavaScript(szScript, [](const QVariant &vRes) {
		/*
		 * FIXME NEEDS A CALLBACK
		if(vRes.type() == QVariant::String)
		{
			QString szVal = vRes.toString();
			c->returnValue()->setString(szVal);
		}
		else
			c->warning(__tr2qs_ctx("Unsupported datatype", "objects"));
		*/
	});
	return true;
}

KVSO_CLASS_FUNCTION(webView, setWebSetting)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szName;
	bool bEnabled;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("name", KVS_PT_NONEMPTYSTRING, 0, szName)
	KVSO_PARAMETER("value", KVS_PT_BOOLEAN, 0, bEnabled)
	KVSO_PARAMETERS_END(c)
	bool found = false;
	unsigned int j = 0;
	for(; j < webattributes_num; j++)
	{
		if(KviQString::equalCI(szName, webattributes_tbl[j]))
		{
			found = true;
			break;
		}
	}
	if(found)
		((QWebEngineView *)widget())->settings()->setAttribute(webattributes_cod[j], bEnabled);
	else
		c->warning(__tr2qs_ctx("Unknown web setting '%Q'", "objects"), &szName);
	return true;
}
KVSO_CLASS_FUNCTION(webView, findText)
{
	CHECK_INTERNAL_POINTER(widget())
	QString szName;
	QStringList szFindFlag;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("find_text", KVS_PT_NONEMPTYSTRING, 0, szName)
	KVSO_PARAMETER("find_flag", KVS_PT_STRINGLIST, KVS_PF_OPTIONAL, szFindFlag)
	KVSO_PARAMETERS_END(c)
	int findflag = 0;
	int sum = 0;
	for(auto & it : szFindFlag)
	{
		findflag = 0;
		for(unsigned int j = 0; j < findflag_num; j++)
		{
			if(KviQString::equalCI(it, findflag_tbl[j]))
			{
				findflag = findflag_cod[j];
				break;
			}
		}
		if(findflag)
			sum = sum | findflag;
		else
			c->warning(__tr2qs_ctx("Unknown findflag  '%Q'", "objects"), &it);
	}
	((QWebEngineView *)widget())->findText(szName, (QWebEnginePage::FindFlags)findflag);
	return true;
}

KVSO_CLASS_FUNCTION(webView, appendWebViewActionToMenu)
{
	CHECK_INTERNAL_POINTER(widget())
	KviKvsObject * ob;
	kvs_hobject_t hObject;
	QString szActionName, szIcon;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("popup", KVS_PT_HOBJECT, 0, hObject)
	KVSO_PARAMETER("action_name", KVS_PT_NONEMPTYSTRING, 0, szActionName)
	KVSO_PARAMETER("icon", KVS_PT_STRING, KVS_PF_OPTIONAL, szIcon)
	KVSO_PARAMETERS_END(c)
	ob = KviKvsKernel::instance()->objectController()->lookupObject(hObject);
	if(!ob)
	{
		c->warning(__tr2qs_ctx("Widget parameter is not an object", "objects"));
		return true;
	}
	if(!ob->object())
	{
		c->warning(__tr2qs_ctx("Widget parameter is not a valid object", "objects"));
		return true;
	}
	if(!ob->inheritsClass("popupmenu"))
	{
		c->warning(__tr2qs_ctx("Can't add a non-popupmenu object", "objects"));
		return true;
	}
	QAction * pAction = nullptr;
	for(unsigned int i = 0; i < actions_num; i++)
	{
		if(KviQString::equalCI(szActionName, actions_tbl[i]))
		{
			pAction = ((QWebEngineView *)widget())->pageAction(actions_cod[i]);
			break;
		}
	}
	if(!pAction)
	{
		c->warning(__tr2qs_ctx("Unknown action '%Q':", "objects"), &szActionName);
		return true;
	}
	if(!szIcon.isEmpty())
	{
		QPixmap * pix;
		pix = g_pIconManager->getImage(szIcon);
		if(pix)
			pAction->setIcon(*pix);
		else
			c->warning(__tr2qs_ctx("Icon '%Q' doesn't exist", "objects"), &szIcon);
	}
	((QMenu *)(ob->object()))->addAction(pAction);
	return true;
}

KVSO_CLASS_FUNCTION(webView, loadFinishedEvent)
{
	emitSignal("loadFinished", c, c->params());
	return true;
}

KVSO_CLASS_FUNCTION(webView, loadProgressEvent)
{
	emitSignal("loadProgress", c, c->params());
	return true;
}

KVSO_CLASS_FUNCTION(webView, loadStartedEvent)
{
	emitSignal("loadStarted", c);
	return true;
}

KVSO_CLASS_FUNCTION(webView, downloadCompletedEvent)
{
	emitSignal("downloadCompleted", c, c->params());
	return true;
}

KVSO_CLASS_FUNCTION(webView, downloadProgressEvent)
{
	emitSignal("downloadProgress", c, c->params());
	return true;
}

KVSO_CLASS_FUNCTION(webView, linkClickedEvent)
{
	emitSignal("linkClicked", c, c->params());
	return true;
}

KVSO_CLASS_FUNCTION(webView, downloadRequestEvent)
{
	emitSignal("downloadRequest", c, c->params());
	return true;
}

KVSO_CLASS_FUNCTION(webView, jsSubmitEvent)
{
	emitSignal("jssubmit", c, c->params());
	return true;
}
KVSO_CLASS_FUNCTION(webView, jsChangeEvent)
{
	emitSignal("jschange", c, c->params());
	return true;
}
KVSO_CLASS_FUNCTION(webView, jsClickEvent)
{
	emitSignal("jsclick", c, c->params());
	return true;
}
KVSO_CLASS_FUNCTION(webView, jsMouseOverEvent)
{
	emitSignal("jsmouseover", c, c->params());
	return true;
}
KVSO_CLASS_FUNCTION(webView, jsMouseOutEvent)
{
	emitSignal("jsmouseout", c, c->params());
	return true;
}
void KvsObject_webView::slotOnChange(QString szParam)
{
	//qDebug("change");
	KviKvsVariantList params(new KviKvsVariant(szParam));
	callFunction(this, "jsChangeEvent", &params);
}

void KvsObject_webView::slotOnSubmit(QString szParam)
{
	// ,QString szParam2,QString szParam3
	//qDebug("slotonsubmit");
	KviKvsVariantList params(new KviKvsVariant(szParam));
	callFunction(this, "jsSubmitEvent", &params);
}

void KvsObject_webView::slotOnClick(QString szParam)
{
	KviKvsVariantList params(new KviKvsVariant(szParam));
	callFunction(this, "jsClickEvent", &params);
}
void KvsObject_webView::slotOnMouseOver(QString szParam)
{
	KviKvsVariantList params(new KviKvsVariant(szParam));
	callFunction(this, "jsMouseOverEvent", &params);
}
void KvsObject_webView::slotOnMouseOut(QString szParam)
{
	KviKvsVariantList params(new KviKvsVariant(szParam));
	callFunction(this, "jsMouseOutEvent", &params);
}

// slots
void KvsObject_webView::slotLoadFinished(bool bOk)
{
	KviKvsVariantList params(new KviKvsVariant(bOk));
	callFunction(this, "loadFinishedEvent", &params);
}

void KvsObject_webView::slotLoadStarted()
{
	KviKvsVariantList * lParams = nullptr;
	callFunction(this, "loadStartedEvent", lParams);
}

void KvsObject_webView::slotLoadProgress(int iProgress)
{
	KviKvsVariantList params(new KviKvsVariant((kvs_int_t)iProgress));
	callFunction(this, "loadProgressEvent", &params);
}

void KvsObject_webView::slotLinkClicked(const QUrl & url)
{
	QString szUrl = url.toString();
	KviKvsVariantList params(new KviKvsVariant(szUrl));
	callFunction(this, "linkClickedEvent", &params);
}

void KvsObject_webView::slotDownloadRequest(DOWNLOAD_CLASS_NAME *pDownload)
{
	QString szFilePath = "";
	KviKvsVariant * filepathret = new KviKvsVariant(szFilePath);
	KviKvsVariantList params(new KviKvsVariant(pDownload->url().toString()));
	callFunction(this, "downloadRequestEvent", filepathret, &params);
	filepathret->asString(szFilePath);
	if(!szFilePath.isEmpty())
	{
		(void)new KviKvsDownloadHandler(this, szFilePath, pDownload, g_iDownloadId);
		g_iDownloadId++;
	}
}

KviKvsWebView::KviKvsWebView(QWidget * par, const char * name, KvsObject_webView * parent)
    : QWebEngineView(par)
{
	m_pParentScript = parent;
	setObjectName(name);
}

void KviKvsWebView::mouseMoveEvent(QMouseEvent * ev)
{
	KviKvsVariant vRetValue;
	KviKvsVariantList lParams;
#if(QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
	lParams.append(new KviKvsVariant((kvs_int_t)ev->x()));
	lParams.append(new KviKvsVariant((kvs_int_t)ev->y()));
#else
	lParams.append(new KviKvsVariant((kvs_int_t)ev->position().x()));
	lParams.append(new KviKvsVariant((kvs_int_t)ev->position().y()));
#endif
	if(!m_pParentScript->callFunction(m_pParentScript, "mouseMoveEvent", &vRetValue, &lParams))
		QWebEngineView::mouseMoveEvent(ev); // ignore results of a broken event handler
	else
	{
		if(!vRetValue.asBoolean())
			QWebEngineView::mouseMoveEvent(ev);
	}
}
void KviKvsWebView::contextMenuEvent(QContextMenuEvent * ev)
{
	KviKvsVariant vRetValue;
	KviKvsVariantList lParams;
	lParams.append(new KviKvsVariant((kvs_int_t)ev->x()));
	lParams.append(new KviKvsVariant((kvs_int_t)ev->y()));
	if(!m_pParentScript->callFunction(m_pParentScript, "customContextMenuRequestedEvent", &vRetValue, &lParams))
		QWebEngineView::contextMenuEvent(ev); // ignore results of a broken event handler
	else
	{
		if(!vRetValue.asBoolean())
			QWebEngineView::contextMenuEvent(ev);
	}
}
bool KviKvsWebView::event(QEvent * e)
{

	if(e->type() == QEvent::ToolTip)
	{
		QHelpEvent * helpEvent = static_cast<QHelpEvent *>(e);
		QString szTooltip = "";
		KviKvsVariant * tipret = new KviKvsVariant(szTooltip);
		KviKvsVariantList params(new KviKvsVariant((kvs_int_t)helpEvent->x()), new KviKvsVariant((kvs_int_t)helpEvent->y()));
		m_pParentScript->callFunction(m_pParentScript, "maybeTipEvent", tipret, &params);
		tipret->asString(szTooltip);
		if(!szTooltip.isEmpty())
		{
			QToolTip::showText(helpEvent->globalPos(), szTooltip);
			return false;
		}
	}
	return QWebEngineView::event(e);
}
KviKvsWebView::~KviKvsWebView()
    = default;
#endif // COMPILE_WEBENGINE_SUPPORT
