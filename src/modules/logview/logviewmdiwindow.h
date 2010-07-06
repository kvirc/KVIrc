#ifndef _LOGVIEWMDIWINDOW_H_
#define _LOGVIEWMDIWINDOW_H_
//=============================================================================
//
//   File : logviewmdiwindow.h
//   Creation date : Tue Apr 23 2002 18:05:59 by Juanjo Alvarez
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Juanjo Alvarez
//   Copyright (C) 2002-2008 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_window.h"
#include "kvi_moduleextension.h"
#include "kvi_tal_vbox.h"
#include <QTreeWidget>
#include "kvi_pointerlist.h"

#include <QTabWidget>
#include <QDateTime>
#include <QCheckBox>

class QStringList;
class QLineEdit;
class QDateEdit;
class KviLogViewWidget;


class KviLogFile {

public:

	enum KviLogTypes {
		Channel,
		Console,
		Query,
		DccChat,
		Other
	};

	KviLogFile(const QString& name);

	const QString & fileName() const { return m_szFilename; };
	const QString & name() const { return m_szName; };
	const QString & network() const { return m_szNetwork; };
	const QDate   & date() const { return m_date; };

	void getText(QString & text,const QString& logDir);

	KviLogTypes type() const { return m_type; };
private:
	KviLogTypes  m_type;
	QString      m_szFilename;
	bool         m_bCompressed;
	QString      m_szName;
	QString      m_szNetwork;
	QDate        m_date;
};

class KviLogViewListView : public QTreeWidget
{
	Q_OBJECT
public:
	KviLogViewListView(QWidget*);
	~KviLogViewListView(){};
protected:
	void mousePressEvent (QMouseEvent *e);
signals:
	void rightButtonPressed(QTreeWidgetItem *,QPoint);
};

class KviLogViewMDIWindow : public KviWindow , public KviModuleExtension
{
    Q_OBJECT
public:
	KviLogViewMDIWindow(KviModuleExtensionDescriptor * d,KviFrame * lpFrm);
	~KviLogViewMDIWindow();
protected:
	KviPointerList<KviLogFile> m_logList;

	KviLogViewListView     * m_pListView;

	// Type filter
	QCheckBox          * m_pShowChannelsCheck;
	QCheckBox          * m_pShowQueryesCheck;
	QCheckBox          * m_pShowConsolesCheck;
	QCheckBox          * m_pShowOtherCheck;
	QCheckBox          * m_pShowDccChatCheck;

	// Content filter
	QLineEdit          * m_pFileNameMask;
	QLineEdit          * m_pContentsMask;

	// Date/time mask
	QCheckBox          * m_pEnableFromFilter;
	QCheckBox          * m_pEnableToFilter;
	QDateEdit          * m_pFromDateEdit;
	QDateEdit          * m_pToDateEdit;

	QStringList        * m_pFileNames;
	QString              m_szLogDirectory;
	QTabWidget         * m_pTabWidget;
	KviTalVBox         * m_pIndexTab;
	QWidget            * m_pSearchTab;
	QPushButton        * m_pFilterButton;
protected:
	QStringList getFileNames();

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
	void cacheFileList();
};

#endif //_LOGVIEWMDIWINDOW_H_
