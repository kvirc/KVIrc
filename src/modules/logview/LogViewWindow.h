#ifndef _LOGVIEWWINDOW_H_
#define _LOGVIEWWINDOW_H_
//=============================================================================
//
//   File : LogViewWindow.h
//   Creation date : Tue Apr 23 2002 18:05:59 by Juanjo Alvarez
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2002 Juanjo Alvarez
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2011 Elvio Basello (hellvis69 at gmail dot com)
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

#include "LogFile.h"

#include "kvi_settings.h"
#include "KviWindow.h"
#include "KviModuleExtension.h"
#include "KviTalVBox.h"
#include "KviPointerList.h"

#include <QTreeWidget>

class KviLogViewWidget;
class LogListViewItem;
class LogListViewItemFolder;
class QProgressBar;
class QStringList;
class QLineEdit;
class QDateEdit;
class QTabWidget;
class QCheckBox;

class LogViewListView : public QTreeWidget
{
	Q_OBJECT
public:
	LogViewListView(QWidget *);
	~LogViewListView(){};
protected:
	void mousePressEvent(QMouseEvent * pEvent);
signals:
	void rightButtonPressed(QTreeWidgetItem *, QPoint);
};

class LogViewWindow : public KviWindow
{
	Q_OBJECT
public:
	LogViewWindow();
	~LogViewWindow();
protected:
	KviPointerList<LogFile> m_logList;

	LogViewListView       * m_pListView;

	// Type filter
	QCheckBox             * m_pShowChannelsCheck;
	QCheckBox             * m_pShowQueryesCheck;
	QCheckBox             * m_pShowConsolesCheck;
	QCheckBox             * m_pShowOtherCheck;
	QCheckBox             * m_pShowDccChatCheck;

	// Content filter
	QLineEdit             * m_pFileNameMask;
	QLineEdit             * m_pContentsMask;

	// Date/time mask
	QCheckBox             * m_pEnableFromFilter;
	QCheckBox             * m_pEnableToFilter;
	QDateEdit             * m_pFromDateEdit;
	QDateEdit             * m_pToDateEdit;

	QStringList           * m_pFileNames;
	QTabWidget            * m_pTabWidget;
	KviTalVBox            * m_pIndexTab;
	KviTalVBox            * m_pLeftLayout;
	QWidget               * m_pSearchTab;
	QPushButton           * m_pFilterButton;
	QPushButton           * m_pCancelButton;
	KviTalHBox            * m_pBottomLayout;
	QProgressBar          * m_pProgressBar;
	LogListViewItem       * m_pLastCategory;
	LogListViewItemFolder * m_pLastGroupItem;
	QString                 m_szLastGroup;
	bool                    m_bAborted;
	QTimer                * m_pTimer;
	QMenu       * m_pExportLogPopup;
public:
	/**
	* \brief Exports the log and creates the file in the selected format
	* \param pLog The log file associated to the item selected in the popup
	* \param iId The id of the item in the popup
	* \param pszFile The buffer where to store the exported log name
	* \return void
	*/
	void createLog(LogFile * pLog, int iId, QString * pszFile = 0);
protected:
	void recurseDirectory(const QString & szDir);
	void setupItemList();

	virtual QPixmap * myIconPtr();
	virtual void resizeEvent(QResizeEvent * pEvent);
	virtual void keyPressEvent(QKeyEvent * pEvent);
	virtual void fillCaptionBuffers();
	virtual void die();
	virtual QSize sizeHint() const;
protected slots:
	void rightButtonClicked(QTreeWidgetItem *, const QPoint &);
	void itemSelected(QTreeWidgetItem * pItem, QTreeWidgetItem *);
	void deleteCurrent();
	void applyFilter();
	void abortFilter();
	void cacheFileList();
	void filterNext();
	void exportLog(int iId);
};

#endif //_LOGVIEWWINDOW_H_
