#ifndef _LOGVIEWMDIWINDOW_H_
#define _LOGVIEWMDIWINDOW_H_
//
//   File : logviewmdiwindow.h
//   Creation date : Tue Apr 23 2002 18:05:59 by Juanjo �varez
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2007 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//

#include "kvi_settings.h"
#include "kvi_window.h"
#include "kvi_moduleextension.h"

#include "kvi_tal_vbox.h"
#include "kvi_tal_listview.h"
#include "kvi_list.h"
#include <qtabwidget.h>
#include <qdatetime.h> 

class KviTalListView;
class QStringList;
class KviLogViewWidget;
class QCheckBox;
class QLineEdit;
class QDateEdit;

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

	const QString & fileName() { return m_szFilename; };
	const QString & name() { return m_szName; };
	const QString & network() { return m_szNetwork; };
	const QDate   & date() { return m_date; };

	void getText(QString & text,const QString& logDir);

	KviLogTypes type() { return m_type; };
private:
	KviLogTypes  m_type;
	QString      m_szFilename;
	bool         m_bCompressed;
	QString      m_szName;
	QString      m_szNetwork;
	QDate        m_date;
};

class KviLogViewMDIWindow : public KviWindow , public KviModuleExtension
{
    Q_OBJECT
public: 
	KviLogViewMDIWindow(KviModuleExtensionDescriptor * d,KviFrame * lpFrm);
	~KviLogViewMDIWindow();
protected:
	KviPtrList<KviLogFile> m_logList;

	KviTalListView     * m_pListView;

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
protected:
	QStringList getFileNames();
	
	void setupItemList();
	void cacheFileList();

	virtual QPixmap * myIconPtr();
	virtual void resizeEvent(QResizeEvent *e);
	virtual void fillCaptionBuffers();
	virtual void die();
	virtual QSize sizeHint() const;
protected slots:
	void rightButtonClicked ( KviTalListViewItem *, const QPoint &, int );
	void itemSelected(KviTalListViewItem * it);
	void deleteCurrent();
	void applyFilter();
};

#endif //_LOGVIEWMDIWINDOW_H_

