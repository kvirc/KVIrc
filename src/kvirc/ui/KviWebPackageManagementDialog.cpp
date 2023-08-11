//=============================================================================
//
//   File : KviWebPackageManagementDialog.cpp
//   Creation date : Thu 17 Mar 2011 15:21:06
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2011 Alessandro Carbone <elfonol at gmail dot com>
//   Copyright (C) 2011 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviWebPackageManagementDialog.h"

#ifdef COMPILE_WEBKIT_SUPPORT

#include "KviLocale.h"
#include "KviApplication.h"
#include "KviMessageBox.h"
#include "KviModule.h"
#include "KviFileUtils.h"
#include "KviIconManager.h"
#include "KviMainWindow.h"
#include "KviNetworkAccessManager.h"

#include <QScreen>
#include <QToolButton>
#include <QLineEdit>
#include <QToolTip>
#include <QTimer>
#include <QClipboard>
#include <QHBoxLayout>
#include <QWebEngineView>
#include <QVBoxLayout>
#include <QToolBar>
#include <QFile>
#include <QProgressBar>
#include <QDir>
#include <QUrl>
#include <QShowEvent>

KviWebPackageManagementDialog::KviWebPackageManagementDialog(QWidget * pParent)
    : QWidget(pParent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setMinimumSize(600, 500);
	setWindowIcon(QIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_WWW))));

	m_pLayout = new QVBoxLayout(this);
	m_pLayout->setMargin(3);
	m_pLayout->setSpacing(2);

	setLayout(m_pLayout);
	m_bBusy = false;

	QWidget * pStatus = new QWidget(this);
	m_pLayout->addWidget(pStatus);

	QHBoxLayout * hbox = new QHBoxLayout(pStatus);

	m_pToolBar = new QToolBar(pStatus);
	m_pProgressBar = new QProgressBar(pStatus);
	m_pProgressBar->setMaximumWidth(220);
	m_pProgressBar->setMinimumWidth(220);
	hbox->addWidget(m_pToolBar);
	hbox->addWidget(m_pProgressBar);
	hbox->setAlignment(m_pProgressBar, Qt::AlignRight);

	m_pWebView = new QWebEngineView(this);
	// we managing the links
	m_pWebView->setPage(new KviWebPackagePage());

	m_pLayout->addWidget(m_pWebView);
	m_pLayout->setStretchFactor(m_pWebView, 10); // must take most space
	// disable context menu
	m_pWebView->setContextMenuPolicy(Qt::NoContextMenu);
	m_pToolBar->addAction(m_pWebView->pageAction(QWebEnginePage::Back));
	m_pToolBar->addAction(m_pWebView->pageAction(QWebEnginePage::Forward));

	//connect(pWebView,SIGNAL(loadStarted()),this,SLOT(slotLoadStarted()));
	connect(m_pWebView, SIGNAL(loadFinished(bool)), this, SLOT(slotLoadFinished(bool)));
	connect(m_pWebView, SIGNAL(loadProgress(int)), this, SLOT(slotLoadProgress(int)));
	connect(m_pWebView->page(), SIGNAL(linkClicked(const QUrl &)), this, SLOT(slotLinkClicked(const QUrl &)));
}

KviWebPackageManagementDialog::~KviWebPackageManagementDialog()
{
	KviWebPackagePage *tmp = (KviWebPackagePage*) m_pWebView->page();
	m_pWebView->setPage(nullptr);
	tmp->deleteLater();
}

void KviWebPackageManagementDialog::setPackagePageUrl(const QString & szUrl)
{
	m_szPackagePageUrl = szUrl;
	m_pWebView->load(QUrl(m_szPackagePageUrl));
}

void KviWebPackageManagementDialog::slotLoadFinished(bool bOk)
{
	if(!bOk)
	{
		qDebug("Error loading page"); // fixme: add warning dialog
		return;
	}

	m_pProgressBar->hide();
	m_pProgressBar->setValue(0);

	if(!isVisible())
		show();

	m_pWebView->page()->runJavaScript("var items = "" \
		Array.from(document.querySelectorAll(\"div.item_entry\")).forEach( \
			function(element, index, array) { \
				items += index + \"|\" + element.querySelector(\"span.item_id\").innerText + \"|\" + element.querySelector(\"span.item_version\").innerText + \"\\n\" \
			})",
		[this](const QVariant &items) {
			QStringList elementscollection = items.toString().split("\n", Qt::SkipEmptyParts);
			foreach(const QString element, elementscollection)
			{
				QStringList tmp = element.split("|");
				if(tmp.size() != 3)
					continue;

				// web element 'title'
				QString szIndex = tmp.at(0);
				// string title
				QString szId = tmp.at(1);
				// number version
				QString szVersion = tmp.at(2);
				// is the theme installed?
				if(packageIsInstalled(szId, szVersion))
				{
					// change the background color to highlight the item
					m_pWebView->page()->runJavaScript(QString("var item = document.querySelectorAll(\"div.item_entry\")[%1]; \
						item.querySelector(\"div.item_identification\").style.background = \"none repeat scroll 0 0 #3cd543\"; \
						item.querySelector(\"a.item_download_link\").innerText = \"Reinstall\";").arg(szIndex));
				}

			}
		}
	);
}
void KviWebPackageManagementDialog::slotLinkClicked(const QUrl & url)
{
	// check if one download is running
	if(m_bBusy)
		return;
	QString szScheme = url.scheme();
	// it's an ftp link?
	if(KviQString::equalCI(szScheme, "ftp"))
	{
		// one download at once
		m_bBusy = true;

		QNetworkRequest req(url);
		QNetworkReply * pReply = KviNetworkAccessManager::getInstance()->get(req);
		connect(pReply, SIGNAL(finished()), this, SLOT(slotCommandFinished()));
		connect(pReply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(slotDataTransferProgress(qint64, qint64)));

		m_pProgressBar->show();
		return;
	}

	m_pProgressBar->show();
	m_pWebView->load(url);
}

void KviWebPackageManagementDialog::slotDataTransferProgress(qint64 iDone, qint64 iTotal)
{
	m_pProgressBar->setMaximum(iTotal);
	m_pProgressBar->setValue(iDone);
	m_pProgressBar->setFormat(__tr2qs("Downloading: %p%"));
}

void KviWebPackageManagementDialog::slotLoadProgress(int iProgress)
{
	m_pProgressBar->setMaximum(100);
	m_pProgressBar->setValue(iProgress);
	m_pProgressBar->setFormat(__tr2qs("Loading: %p%"));
}

void KviWebPackageManagementDialog::slotCommandFinished()
{
	QNetworkReply * reply = qobject_cast<QNetworkReply *>(sender());

	m_pProgressBar->hide();
	m_pProgressBar->setValue(0);

	if(reply)
	{
		if(reply->error() == QNetworkReply::NoError)
		{
			//read data from reply
			QString szUrl = reply->url().toString();
			int iIdx = szUrl.lastIndexOf("/");
			QString szFile = szUrl.right(szUrl.length() - iIdx - 1);
			QFile tmpFile;
			g_pApp->getLocalKvircDirectory(m_szLocalTemporaryPath, KviApplication::Tmp, szFile);
			tmpFile.setFileName(m_szLocalTemporaryPath);
			tmpFile.open(QIODevice::ReadWrite);
			tmpFile.write(reply->readAll());
			tmpFile.close();

			QString szError;
			if(!installPackage(m_szLocalTemporaryPath, szError))
			{
				KviMessageBox::information(szError);
			}
			else
			{
				m_pWebView->load(QUrl(m_szPackagePageUrl));
			}
			QFileInfo info(m_szLocalTemporaryPath);

			if(info.exists())
				KviFileUtils::removeFile(m_szLocalTemporaryPath);
			m_bBusy = false;
		}
		else
		{
			//get http status code
			int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
			KviMessageBox::information(__tr2qs("Download failed: %1").arg(reply->errorString()));
		}
		reply->deleteLater();
	}
}

void KviWebPackageManagementDialog::showEvent(QShowEvent *)
{
	m_pProgressBar->hide();
	QRect rect = g_pMainWindow->screen()->availableGeometry();
	move(rect.x() + ((rect.width() - width()) / 2), rect.y() + ((rect.height() - height()) / 2));
}

#endif //COMPILE_WEBKIT_SUPPORT
