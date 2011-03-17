//=============================================================================
//
//   File : WebThemeInterfaceDialog.cpp
//   Creation date : Mon Mar 14 2011 15:10:20 by Alessandro Carbone
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2011 Alessandro Carbone(elfonol at gmail dot com)
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

#define KVI_URL_THEMES "http://www.kvirc.de/app/themes.php"


#if defined(COMPILE_WEBKIT_SUPPORT) && (QT_VERSION >= 0x040600)

#include "WebThemeInterfaceDialog.h"
#include "ThemeFunctions.h"

#include "KviLocale.h"
#include "KviApplication.h"
#include "KviMessageBox.h"
#include "KviModule.h"
#include "KviFileUtils.h"
#include "KviIconManager.h"

#include <QDesktopWidget>
#include <QToolButton>
#include <QLineEdit>
#include <QToolTip>
#include <QTimer>
#include <QClipboard>
#include <QHBoxLayout>
#include <QDir>

WebThemeInterfaceDialog::WebThemeInterfaceDialog(QWidget * par)
: QWidget (par)
{
	m_pLayout = new QVBoxLayout(this);
	setWindowTitle(__tr2qs_ctx("KVIrc Web Theme Interface","theme"));
	setWindowIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_WWW)));
	m_pFile = 0;
	m_bBusy = false;
	QWidget *pStatus = new QWidget(this);
	m_pLayout->addWidget(pStatus);
	QHBoxLayout *hbox= new QHBoxLayout(pStatus);
	//  m_pLayout = new QVBoxLayout(this);
	m_pFtp = 0;
	m_pLayout->setMargin(3);
	m_pLayout->setSpacing(2);
	setLayout(m_pLayout);
	m_pToolBar= new QToolBar(pStatus);
	m_pProgressBar = new QProgressBar(pStatus);
	m_pProgressBar->setMaximumWidth(220);
	m_pProgressBar->setMinimumWidth(220);
	hbox->addWidget(m_pToolBar);
	hbox->addWidget(m_pProgressBar);
	hbox->setAlignment(m_pProgressBar,Qt::AlignRight);

	m_pWebView = new QWebView(this);
	m_pLayout->addWidget(m_pWebView);
	// disable context menu
	m_pWebView->setContextMenuPolicy(Qt::NoContextMenu);
	m_pToolBar->addAction(m_pWebView->pageAction(QWebPage::Back));
	m_pToolBar->addAction(m_pWebView->pageAction(QWebPage::Forward));

	//connect(pWebView,SIGNAL(loadStarted()),this,SLOT(slotLoadStarted()));
	connect(m_pWebView,SIGNAL(loadFinished(bool)),this,SLOT(slotLoadFinished(bool)));
	connect(m_pWebView,SIGNAL(loadProgress(int)),this,SLOT(slotLoadProgress(int)));
	connect(m_pWebView->page(),SIGNAL(linkClicked(const QUrl &)),this,SLOT(slotLinkClicked(const QUrl &)));

	// we managing the links
	m_pWebView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
	m_pWebView->load(QUrl(KVI_URL_THEMES));

}
WebThemeInterfaceDialog::~WebThemeInterfaceDialog()
{
	if(m_pFile)
		delete m_pFile;
	if(m_pFtp)
		delete m_pFtp;
}

void WebThemeInterfaceDialog::slotLoadFinished(bool bOk)
{
	if(!bOk)
	{
		qDebug("Error loading page");// fixme: add warning dialog		return;
		return;
	}

	m_pProgressBar->hide();
	m_pProgressBar->setValue(0);

	if(!isVisible())
		show(); 

	// main frame
	QWebFrame *pFrame=m_pWebView->page()->mainFrame();	
	/*QWebElement style=pFrame->documentElement().findFirst("style");
	QString szCss=style.toPlainText();
	szCss.append("div.item_entry:hover {background-color: rgba(0,0,0,0.9)}");
	style.setPlainText(szCss);*/
	// search for all item_entry div
	QWebElementCollection elementscollection=pFrame->documentElement().findAll("div.item_entry");

	// local dir for user defined themes
	QString szLocalThemesPath;
	g_pApp->getLocalKvircDirectory(szLocalThemesPath,KviApplication::Themes);
	szLocalThemesPath += KVI_PATH_SEPARATOR_CHAR;

	// global dir for builtin themes
	QString szGlobalThemesPath;
	g_pApp->getGlobalKvircDirectory(szGlobalThemesPath,KviApplication::Themes);
	szGlobalThemesPath += KVI_PATH_SEPARATOR_CHAR;

	// if this is null something is wrong
	if(elementscollection.count())
	{  
		foreach(QWebElement element,elementscollection)
		{
			// web element 'title'
			QWebElement eTitle=element.findFirst("span.item_name");
			// string title
			QString szTitle=eTitle.toPlainText();
			// number version
			QString szVersion=element.findFirst("span.item_version").toPlainText();
			//QString t=szLocalThemesPath+szTitle+"-"+szVersion;
			// is the theme installed?
			if(KviFileUtils::fileExists(szGlobalThemesPath+szTitle+"-"+szVersion)|| KviFileUtils::fileExists(szLocalThemesPath+szTitle+"-"+szVersion))
			{
				// yeah: change the background color for highlight the item
				element.findFirst("div.item_id").setStyleProperty("background","none repeat scroll 0 0 #3cd543");
				// then change the 'install' text into 'uninstall'
				QWebElement eDownload=element.findFirst("a.item_download_link");
				eDownload.setPlainText("Uninstall");
				// why use a c++ var when we can use the web page for store it? :-D
				eDownload.setAttribute("Installed",QString("1"));
			}
		}
	}
}
void WebThemeInterfaceDialog::slotLinkClicked(const QUrl &url)
{
	// check if one download is running
	if(m_bBusy) return;
	QString szScheme=url.scheme();
	// it's an ftp link?
	if(!KviQString::equalCI(szScheme,"ftp"))
	{
		m_pProgressBar->show();
		m_pWebView->load(url);
		return;
	}
	
	// let's search for the 'a' tag
	QWebElementCollection elementscollection=m_pWebView->page()->mainFrame()->findAllElements("a.item_download_link");
	if(elementscollection.count() == 0)
		return;

	foreach(QWebElement element,elementscollection)
	{
		// check the href to find the item
		if(!KviQString::equalCI(element.attribute("href"),url.toString()))
			continue;

		// check the hidden attribute for installed item
		if(element.attribute("Installed").isEmpty())
		{
			// one download at once
			m_bBusy = true;
			if(!m_pFtp)
			{
				m_pFtp = new QFtp();
				connect(m_pFtp,SIGNAL(commandFinished(int,bool)),this,SLOT(slotCommandFinished(int,bool)));
				connect(m_pFtp,SIGNAL(dataTransferProgress(qint64,qint64)),this,SLOT(slotDataTransferProgress(qint64,qint64)));
			}
			m_pProgressBar->show();
			QString szUrl = url.toString();
			int iIdx=szUrl.lastIndexOf("/");
			QString szFile = szUrl.right(szUrl.length()-iIdx-1);
			m_pFile = new QFile(m_pFtp);
			g_pApp->getLocalKvircDirectory(m_szLocalTmpThemeCompletePath,KviApplication::Tmp,szFile);
			m_pFile->setFileName(m_szLocalTmpThemeCompletePath);
			m_pFile->open(QIODevice::ReadWrite);
			m_pFtp->connectToHost(url.host());
			m_pFtp->login("anonymous","anonymous");
			m_pFtp->get(url.path(),m_pFile);
		} else {
			qDebug("uninstall theme");//to be continued
		}
	}
}

void WebThemeInterfaceDialog::slotDataTransferProgress(qint64 iDone,qint64 iTotal)
{
	m_pProgressBar->setMaximum(iTotal);
	m_pProgressBar->setValue(iDone);
	m_pProgressBar->setFormat(__tr2qs_ctx("Downloading ","theme")+"%p");
}

void WebThemeInterfaceDialog::slotLoadProgress(int iProgress)
{
	m_pProgressBar->setMaximum(100);
	m_pProgressBar->setValue(iProgress);
	m_pProgressBar->setFormat(__tr2qs_ctx("Loading ","theme")+"%p");
}

void WebThemeInterfaceDialog::slotCommandFinished(int id,bool error)
{
	if(m_pFtp->currentCommand()==QFtp::Get)
	{
		m_pProgressBar->hide();
		m_pProgressBar->setValue(0);
		if(!error)
		{
			QString szError;
			m_pFile->close();
			if(!ThemeFunctions::installThemePackage(m_szLocalTmpThemeCompletePath,szError,this))
			{
				KviMessageBox::information(szError);
			} else {
				m_pWebView->load(QUrl(KVI_URL_THEMES));
			}
			QFileInfo info(m_szLocalTmpThemeCompletePath);

			delete m_pFile;
			m_pFile = 0;
			if (info.exists()) KviFileUtils::removeFile(m_szLocalTmpThemeCompletePath);
			m_bBusy = false;
			m_pFtp->close();
		}
	}
	id=0; //to be continued
}
void WebThemeInterfaceDialog::showEvent(QShowEvent *)
{
	m_pProgressBar->hide();
	QRect rect = g_pApp->desktop()->screenGeometry(g_pApp->desktop()->primaryScreen());
	move((rect.width() - width())/2,(rect.height() - height())/2);
}

#endif

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "WebThemeInterfaceDialog.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

