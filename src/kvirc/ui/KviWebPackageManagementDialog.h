#ifndef _KviWebPackageManagementDialog_h_
#define _KviWebPackageManagementDialog_h_
//=============================================================================
//
//   File : KviWebPackageManagementDialog.h
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

#include "kvi_settings.h"

#include <QWidget>
#include <QJsonObject>
#include "KviTalListWidget.h"

class QToolBar;
class QToolButton;
class QFile;
class QProgressBar;
class QUrl;
class QTableWidget;
class QMehu;

class KviWebPackageListItem : public QObject, public KviTalListWidgetItem
{
	Q_OBJECT

public:
	KviWebPackageListItem(KviTalListWidget * pBox, QJsonObject obj, const QString & szBaseUrl);
	~KviWebPackageListItem() {};

protected:
	QString m_szName;
	QString m_szVersion;
	QString m_szAuthor;
	QString m_szDesc;
	QString m_szScreen;
	QString m_szDownload;

	void downloadIcon(const QString & szIconUrl);
public:
	const QString & name() { return m_szName; };
	const QString & version() { return m_szVersion; };
	const QString & download() { return m_szDownload; };
public slots:
	void showPopupImage();
};

///
/// \class KviWebPackageManagementDialog
/// \brief The KviWebPackageManagementDialog class
///
/// This class...
///
class KVIRC_API KviWebPackageManagementDialog : public QWidget
{
	Q_OBJECT

public:
	///
	/// Creates an instance of KviWebPackageManagementDialog
	///
	KviWebPackageManagementDialog(QWidget * pParent = nullptr);

	///
	/// Destroys the instance of KviWebPackageManagementDialog
	/// and frees all the relevant resources
	///
	~KviWebPackageManagementDialog();

private:
	QToolBar * m_pToolBar;
	KviTalIconAndRichTextItemDelegate * m_pItemDelegate;
	KviTalListWidget * m_pListWidget;
	bool m_bBusy;
	QProgressBar * m_pProgressBar;
	QString m_szPackagePageUrl;
	QString m_szLocalTemporaryPath;
	QMenu * m_pContextPopup;
	QToolButton * m_pPreviewButton;
	QToolButton * m_pDeleteButton;

protected:
	void setPackagePageUrl(const QString & szUrl);
	void showEvent(QShowEvent * e) override;

	virtual bool packageIsInstalled(const QString & szId, const QString & szVersion) = 0;
	virtual bool installPackage(const QString & szPath, QString & szError) = 0;

protected slots:
	void enableDisableButtons();
	void contextMenuRequested(const QPoint & pos);
	void showItemPreview();
	void downloadItem();
	void slotDataTransferProgress(qint64 iDone, qint64 iTotal);
	void slotDownloadFinished();
}; // class KviWebPackageManagementDialog

#endif //!_KviWebPackageManagementDialog_h_
