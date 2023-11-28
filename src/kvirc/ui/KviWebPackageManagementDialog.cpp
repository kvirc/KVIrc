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
#include <QToolTip>
#include <QClipboard>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>
#include <QFile>
#include <QProgressBar>
#include <QDir>
#include <QUrl>
#include <QShowEvent>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include <QMenu>

#define KVI_WEBPACK_INDEX "index.json"

KviWebPackageListItem::KviWebPackageListItem(KviTalListWidget * pBox, QJsonObject obj, const QString & szBaseUrl)
	: KviTalListWidgetItem(pBox)
{
	m_szName = obj.value("name").toString();
	m_szVersion = obj.value("version").toString();
	m_szAuthor = obj.value("author").toString();
	m_szDesc = obj.value("desc").toString();
	m_szScreen = obj.value("screen").toString();
	m_szDownload = obj.value("download").toString();

	QString szText;
	szText = "<b>";
	szText += m_szName;
	szText += "</b>";

	if(!m_szVersion.isEmpty())
	{
		szText += "[";
		szText += m_szVersion;
		szText += "]";
	}

	if(!m_szAuthor.isEmpty())
	{
		szText += " <font color=\"#454545\"> ";
		szText += __tr2qs("by");
		szText += " ";
		szText += m_szAuthor;
		szText += "</font>";
	}

	szText += "<br><font size=\"-1\">";
	szText += m_szDesc;
	szText += "</font>";
	setText(szText);

	if(!m_szScreen.isEmpty())
	{
		downloadIcon(szBaseUrl + m_szScreen);
	}
}

void KviWebPackageListItem::downloadIcon(const QString & szIconUrl)
{
	QNetworkRequest req(szIconUrl);
	QNetworkReply * pReply = KviNetworkAccessManager::getInstance()->get(req);
	connect(pReply, &QNetworkReply::finished, [this, pReply]() {
		if (pReply->error() == QNetworkReply::NoError) {
			QPixmap pixImage;
			if(pixImage.loadFromData(pReply->readAll()))
			{
				setIcon(pixImage);
			}
		}
		pReply->deleteLater();
	});
}

void KviWebPackageListItem::showPopupImage()
{
	QDialog dlg;
	QHBoxLayout * l = new QHBoxLayout(&dlg);
	QLabel * label = new QLabel;
	l->addWidget(label);
	label->setPixmap(icon().pixmap(800, 600));
	dlg.exec();
}

KviWebPackageManagementDialog::KviWebPackageManagementDialog(QWidget * pParent)
    : QWidget(pParent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setMinimumSize(600, 500);
	setWindowIcon(QIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_WWW))));

	m_bBusy = false;
	m_pContextPopup = new QMenu(this);

	QVBoxLayout * pVBox = new QVBoxLayout(this);
	KviTalHBox * pHBox = new KviTalHBox(this);
	pHBox->setContentsMargins(1, 1, 1, 1);
	pHBox->setSpacing(1);
	pVBox->addWidget(pHBox);

	m_pPreviewButton = new QToolButton(pHBox);
	m_pPreviewButton->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_THEME)));
	m_pPreviewButton->setIconSize(QSize(32, 32));
	m_pPreviewButton->setToolTip(__tr2qs("Show item preview"));
	connect(m_pPreviewButton, SIGNAL(clicked()), this, SLOT(showItemPreview()));

	m_pDeleteButton = new QToolButton(pHBox);
	m_pDeleteButton->setIcon(*(g_pIconManager->getBigIcon(KVI_BIGICON_PACK)));
	m_pDeleteButton->setIconSize(QSize(32, 32));
	m_pDeleteButton->setToolTip(__tr2qs("Download selected item"));
	connect(m_pDeleteButton, SIGNAL(clicked()), this, SLOT(downloadItem()));

	QWidget * w = new QWidget(pHBox);
	w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

	m_pProgressBar = new QProgressBar(this);
	pVBox->addWidget(m_pProgressBar);

	m_pListWidget = new KviTalListWidget(this);
	m_pListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	m_pItemDelegate = new KviTalIconAndRichTextItemDelegate(m_pListWidget);
	m_pItemDelegate->setDefaultIcon(g_pIconManager->getBigIcon(QString(KVI_BIGICON_THEME))->scaled(64, 64, Qt::KeepAspectRatio));
	m_pItemDelegate->setMinimumSize(QSize(64, 64));
	m_pItemDelegate->setIconSize(QSize(64, 64));
	m_pListWidget->setItemDelegate(m_pItemDelegate);
	m_pListWidget->setMinimumHeight(400);
	m_pListWidget->setMinimumWidth(520);

	m_pListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pListWidget->setSortingEnabled(true);
	// connect(m_pListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(applyTheme(QListWidgetItem *)));

	connect(m_pListWidget, SIGNAL(customContextMenuRequested(const QPoint &)),
	    this, SLOT(contextMenuRequested(const QPoint &)));
	connect(m_pListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(enableDisableButtons()));

	pVBox->addWidget(m_pListWidget);
}

KviWebPackageManagementDialog::~KviWebPackageManagementDialog()
{
	m_pItemDelegate->deleteLater();
	m_pListWidget->deleteLater();
}

void KviWebPackageManagementDialog::setPackagePageUrl(const QString & szUrl)
{
	m_szPackagePageUrl = szUrl;
	QNetworkRequest req(m_szPackagePageUrl + KVI_WEBPACK_INDEX);
	QNetworkReply * pReply = KviNetworkAccessManager::getInstance()->get(req);
	connect(pReply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(slotDataTransferProgress(qint64, qint64)));
	connect(pReply, &QNetworkReply::finished, [this, pReply]() {
		m_pProgressBar->hide();
		m_pProgressBar->setValue(0);

		if (pReply->error() == QNetworkReply::NoError) {
			QByteArray szJsonData = pReply->readAll();
			QJsonParseError jsonError{};
			QJsonDocument jsonResponse = QJsonDocument::fromJson(szJsonData, &jsonError);

			if (jsonError.error == QJsonParseError::NoError && jsonResponse.isArray()) {
				QJsonArray jsonArray = jsonResponse.array();

				foreach(QJsonValue jsonValue, jsonArray)
				{
					if(!jsonValue.isObject())
						continue;
					KviWebPackageListItem * item = new KviWebPackageListItem(m_pListWidget, jsonValue.toObject(), m_szPackagePageUrl);
				}
			}
		} else {
			QMessageBox::critical(this, __tr2qs("Error"), __tr2qs("Network error: %1.").arg(pReply->errorString()));
		}

		pReply->deleteLater();
		enableDisableButtons();
    });

	enableDisableButtons();
	m_pProgressBar->show();
}

void KviWebPackageManagementDialog::enableDisableButtons()
{
	QList<QListWidgetItem *> itemsSelected = m_pListWidget->selectedItems();
	if(itemsSelected.count() == 0)
	{
		m_pPreviewButton->setEnabled(false);
		m_pDeleteButton->setEnabled(false);
		return;
	}

	KviWebPackageListItem * pItem = dynamic_cast<KviWebPackageListItem *>(itemsSelected.at(0));

	m_pPreviewButton->setEnabled(!pItem->icon().isNull());
	m_pDeleteButton->setEnabled(!packageIsInstalled(pItem->name(), pItem->version()));
}

void KviWebPackageManagementDialog::contextMenuRequested(const QPoint & pos)
{
	KviWebPackageListItem * pItem = dynamic_cast<KviWebPackageListItem *>(m_pListWidget->itemAt(pos));
	if(pItem == nullptr)
		return;

	m_pListWidget->setCurrentItem(pItem);
	m_pContextPopup->clear();

	if(!pItem->icon().isNull())
	{
		m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Canvas)), __tr2qs("Show Preview"), pItem, SLOT(showPopupImage()));
	}

	if(!packageIsInstalled(pItem->name(), pItem->version()))
		m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Package)), __tr2qs("Download"), this, SLOT(downloadItem()));

	m_pContextPopup->popup(m_pListWidget->viewport()->mapToGlobal(pos));
}

void KviWebPackageManagementDialog::showItemPreview()
{
	QList<QListWidgetItem *> itemsSelected = m_pListWidget->selectedItems();
	if(itemsSelected.count() == 0)
		return;

	KviWebPackageListItem * pItem = dynamic_cast<KviWebPackageListItem *>(itemsSelected.at(0));
	pItem->showPopupImage();
}

void KviWebPackageManagementDialog::downloadItem()
{
	QList<QListWidgetItem *> itemsSelected = m_pListWidget->selectedItems();
	if(itemsSelected.count() == 0)
		return;

	KviWebPackageListItem * pItem = dynamic_cast<KviWebPackageListItem *>(itemsSelected.at(0));

	// check if one download is running
	if(m_bBusy)
		return;

	m_bBusy = true;

	QNetworkRequest req(m_szPackagePageUrl + pItem->download());
	QNetworkReply * pReply = KviNetworkAccessManager::getInstance()->get(req);
	connect(pReply, SIGNAL(finished()), this, SLOT(slotDownloadFinished()));
	connect(pReply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(slotDataTransferProgress(qint64, qint64)));

	m_pProgressBar->show();
}

void KviWebPackageManagementDialog::slotDataTransferProgress(qint64 iDone, qint64 iTotal)
{
	m_pProgressBar->setMaximum(iTotal);
	m_pProgressBar->setValue(iDone);
	m_pProgressBar->setFormat(__tr2qs("Downloading: %p%"));
}

void KviWebPackageManagementDialog::slotDownloadFinished()
{
	QNetworkReply * pReply = qobject_cast<QNetworkReply *>(sender());

	m_pProgressBar->hide();
	m_pProgressBar->setValue(0);

	if(pReply == nullptr)
		return;

	if(pReply->error() != QNetworkReply::NoError)
	{
		// get http status code
		int httpStatus = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		KviMessageBox::information(__tr2qs("Download failed: %1").arg(pReply->errorString()));
		pReply->deleteLater();
		return;
	}

	// read data from reply
	QString szUrl = pReply->url().toString();
	int iIdx = szUrl.lastIndexOf("/");
	QString szFile = szUrl.right(szUrl.length() - iIdx - 1);
	QFile tmpFile;
	g_pApp->getLocalKvircDirectory(m_szLocalTemporaryPath, KviApplication::Tmp, szFile);
	tmpFile.setFileName(m_szLocalTemporaryPath);
	tmpFile.open(QIODevice::ReadWrite);
	tmpFile.write(pReply->readAll());
	tmpFile.close();

	QString szError;
	if(!installPackage(m_szLocalTemporaryPath, szError))
	{
		KviMessageBox::information(szError);
	}
	QFileInfo info(m_szLocalTemporaryPath);

	if(info.exists())
		KviFileUtils::removeFile(m_szLocalTemporaryPath);

	m_bBusy = false;
	enableDisableButtons();
	pReply->deleteLater();
}

void KviWebPackageManagementDialog::showEvent(QShowEvent *)
{
	m_pProgressBar->hide();
	QRect rect = g_pMainWindow->screen()->availableGeometry();
	move(rect.x() + ((rect.width() - width()) / 2), rect.y() + ((rect.height() - height()) / 2));
}
