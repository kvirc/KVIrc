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

#ifdef COMPILE_WEBKIT_SUPPORT

#include <QWidget>

class QToolBar;
class QVBoxLayout;
class QWebView;
class QFile;
class QProgressBar;
class QUrl;

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
	QVBoxLayout * m_pLayout;
	QWebView * m_pWebView;
	bool m_bBusy;
	QProgressBar * m_pProgressBar;
	QString m_szPackagePageUrl;
	QString m_szLocalTemporaryPath;

protected:
	void setPackagePageUrl(const QString & szUrl);
	void showEvent(QShowEvent * e) override;

	virtual bool packageIsInstalled(const QString & szId, const QString & szVersion) = 0;
	virtual bool installPackage(const QString & szPath, QString & szError) = 0;

protected slots:

	void slotLoadFinished(bool ok);
	void slotLoadProgress(int iProgress);
	void slotDataTransferProgress(qint64 iDone, qint64 iTotal);
	void slotCommandFinished();
	void slotLinkClicked(const QUrl & url);

}; // class KviWebPackageManagementDialog

#endif //COMPILE_WEBKIT_SUPPORT

#endif //!_KviWebPackageManagementDialog_h_
