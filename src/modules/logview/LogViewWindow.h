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

#include "kvi_settings.h"
#include "KviWindow.h"
#include "KviModuleExtension.h"
#include "KviTalVBox.h"
#include "KviPointerList.h"

#include <QTreeWidget>
#include <QTabWidget>
#include <QDateTime>
#include <QCheckBox>

class KviLogViewWidget;
class LogListViewItem;
class LogListViewItemFolder;
class QProgressBar;
class QStringList;
class QLineEdit;
class QDateEdit;

/**
* \class LogFile
* \brief The LogFile class which handle any log file
*/
class LogFile
{
public:
	/**
	* \enum Type
	* \brief Holds the type of the log
	*/
	enum Type {
		Channel,   /**< the log file of a channel */
		Console,   /**< the log file of a console */
		Query,     /**< the log file of a query */
		DccChat,   /**< the log file of a dcc chat */
		Other      /**< any other log file */
	};

	/**
	* \brief Constructs the log file object
	* \param szName The name of the log
	* \return LogFile
	*/
	LogFile(const QString & szName);
private:
	Type    m_type;
	QString m_szFilename;
	bool    m_bCompressed;
	QString m_szName;
	QString m_szNetwork;
	QDate   m_date;
public:
	/**
	* \brief Returns the type of the log
	* \return Type
	*/
	Type type() const { return m_type; };

	/**
	* \brief Returns the filename of the log
	* \return const QString &
	*/
	const QString & fileName() const { return m_szFilename; };

	/**
	* \brief Returns the name of the log
	* \return const QString &
	*/
	const QString & name() const { return m_szName; };

	/**
	* \brief Returns the network of the log
	* \return const QString &
	*/
	const QString & network() const { return m_szNetwork; };

	/**
	* \brief Returns the date of the log
	* \return const QDate &
	*/
	const QDate & date() const { return m_date; };

	/**
	* \brief Returns the text of the log file
	* \param szText The buffer where to save the contents of the log
	* \return void
	*/
	void getText(QString & szText);
};

class LogViewListView : public QTreeWidget
{
	Q_OBJECT
public:
	LogViewListView(QWidget*);
	~LogViewListView(){};
protected:
	void mousePressEvent (QMouseEvent *e);
signals:
	void rightButtonPressed(QTreeWidgetItem *,QPoint);
};

class LogViewWindow : public KviWindow, public KviModuleExtension
{
	Q_OBJECT
public:
	LogViewWindow(KviModuleExtensionDescriptor * d,KviMainWindow * lpFrm);
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
protected:
	void recurseDirectory(const QString& sDir);

	void setupItemList();
	virtual QPixmap * myIconPtr();
	virtual void resizeEvent(QResizeEvent *e);
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void fillCaptionBuffers();
	virtual void die();
	virtual QSize sizeHint() const;
protected slots:
	void rightButtonClicked ( QTreeWidgetItem *, const QPoint &);
	void itemSelected(QTreeWidgetItem * it, QTreeWidgetItem *);
	void deleteCurrent();
	void applyFilter();
	void abortFilter();
	void cacheFileList();
	void filterNext();
};

#endif //_LOGVIEWWINDOW_H_
