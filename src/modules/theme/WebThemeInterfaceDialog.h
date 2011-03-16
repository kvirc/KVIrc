#ifndef _WEBTHEMEINTERFACEDIALOG_H_
#define _WEBTHEMEINTERFACEDIALOG_H_
//=============================================================================
//
//   File : WebThemeInterfaceDialog.h
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
#if defined(COMPILE_WEBKIT_SUPPORT)
#include <QDialog>
#include <QtWebKit/QWebView>
#include <QtWebKit/QWebFrame>
#include <QWebElement>
#include <QToolBar>
#include <QFtp>
#include <QUrl>
#include <QFile>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QShowEvent>


class WebThemeInterfaceDialog : public QWidget
{
	Q_OBJECT
public:
	WebThemeInterfaceDialog(QWidget *par=0);
	~WebThemeInterfaceDialog();
private:
	QToolBar   * m_pToolBar;
	QString m_szLocalTmpThemeCompletePath;
	QVBoxLayout *m_pLayout;
	QWebView * m_pWebView;
	QFile *m_pFile;
	QFtp *m_pFtp;
	bool m_bBusy;
	QProgressBar *m_pProgressBar;
protected:
	virtual void showEvent(QShowEvent *e);
protected slots:

	void slotLoadFinished(bool ok);
	void slotLoadProgress(int iProgress);
	void slotDataTransferProgress(qint64 iDone,qint64 iTotal);
	void slotCommandFinished(int id,bool error);
	void slotLinkClicked(const QUrl &url);
};
	
#endif
	

#endif //_WEBTHEMEINTERFACEDIALOG_H_

