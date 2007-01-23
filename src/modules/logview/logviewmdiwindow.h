#ifndef _LOGVIEWMDIWINDOW_H_
#define _LOGVIEWMDIWINDOW_H_
//
//   File : logviewmdiwindow.h
//   Creation date : Tue Apr 23 2002 18:05:59 by Juanjo �varez
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2002 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_window.h"
#include "kvi_moduleextension.h"

#include "kvi_tal_vbox.h"
#include <qtabwidget.h>

class QListView;
class QStringList;
class QListViewItem;
class KviLogViewWidget;

class KviLogViewMDIWindow : public KviWindow , public KviModuleExtension
{
    Q_OBJECT
public: 
	KviLogViewMDIWindow(KviModuleExtensionDescriptor * d,KviFrame * lpFrm);
	~KviLogViewMDIWindow();
protected:
	QListView       * m_pListView;
	QStringList     * m_pFileNames;
	QString           m_szLogDirectory;
	QTabWidget    * m_pTabWidget;
	KviTalVBox         * m_pIndexTab;
	KviTalVBox         * m_pSearchTab;
protected:
	QStringList getFileNames();
	
	void oneTimeSetup();

	virtual QPixmap * myIconPtr();
	virtual void resizeEvent(QResizeEvent *e);
	virtual void fillCaptionBuffers();
	virtual void die();
	virtual QSize sizeHint() const;
protected slots:
	void rightButtonClicked ( QListViewItem *, const QPoint &, int );
	void itemSelected(QListViewItem * it);
	void deleteCurrent();
};

#endif //_LOGVIEWMDIWINDOW_H_
