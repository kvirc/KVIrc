#ifndef _HELPWINDOW_H_
#define _HELPWINDOW_H_
//=============================================================================
//
//   File : helpwindow.h
//   Creation date : Fri Aug 11 2000 18:05:59 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_window.h"
#include "kvi_string.h"
#include "kvi_tal_vbox.h"
#include "kvi_tal_listwidget.h"

#include <QTabWidget>
#include <QLineEdit>

class QTextBrowser;
class KviHelpWidget;

class KviHelpWindow : public KviWindow
{
	Q_OBJECT
public:
	KviHelpWindow(KviFrame * lpFrm,const char * name);
	~KviHelpWindow();
protected:
	KviHelpWidget * m_pHelpWidget;
	KviTalVBox    * m_pToolBar;
	QTabWidget    * m_pTabWidget;
	KviTalVBox    * m_pIndexTab;
	KviTalVBox    * m_pSearchTab;

	KviTalListWidget* m_pIndexListWidget;
	QLineEdit     * m_pIndexSearch;
	QStringList     m_foundDocs;
	QStringList     m_terms;
	KviTalListWidget* m_pResultBox;
	QLineEdit     * m_pTermsEdit;
public:
	KviHelpWidget * helpWidget(){ return m_pHelpWidget; };
protected:
	virtual QPixmap * myIconPtr();
	virtual void fillCaptionBuffers();
	virtual void resizeEvent(QResizeEvent *e);
	virtual void saveProperties(KviConfig * cfg);
	virtual void loadProperties(KviConfig * cfg);
public:
	QTextBrowser * textBrowser();
public slots:
	void indexSelected ( QListWidgetItem * );
	void searchInIndex( const QString &s );
	void showIndexTopic();
	void startSearch();
	void searchSelected ( QListWidgetItem * );
	void refreshIndex();
};

#endif //_KVI_HELPWINDOW_H_
