#ifndef _HELPWINDOW_H_
#define _HELPWINDOW_H_
//=============================================================================
//
//   File : HelpWindow.h
//   Creation date : Fri Aug 11 2000 18:05:59 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviWindow.h"
#include "KviCString.h"
#include "KviTalVBox.h"
#include "KviTalListWidget.h"

#include <QTabWidget>
#include <QLineEdit>

class QProgressBar;
class QPushButton;
class QTextBrowser;
class HelpWidget;

class HelpWindow : public KviWindow
{
	Q_OBJECT
public:
	HelpWindow(KviMainWindow * lpFrm,const char * name);
	~HelpWindow();
protected:
	HelpWidget * m_pHelpWidget;
	KviTalVBox    * m_pToolBar;
	QTabWidget    * m_pTabWidget;
	KviTalVBox    * m_pIndexTab;
	KviTalVBox    * m_pSearchTab;
	KviTalHBox    * m_pBottomLayout;
	QPushButton   * m_pCancelButton;
	QProgressBar  * m_pProgressBar;

	KviTalListWidget* m_pIndexListWidget;
	QLineEdit     * m_pIndexSearch;
	QStringList     m_foundDocs;
	QStringList     m_terms;
	KviTalListWidget* m_pResultBox;
	QLineEdit     * m_pTermsEdit;
	QPushButton   * m_pBtnRefreshIndex;
public:
	HelpWidget * helpWidget(){ return m_pHelpWidget; };
protected:
	virtual QPixmap * myIconPtr();
	virtual void fillCaptionBuffers();
	virtual void resizeEvent(QResizeEvent *e);
	virtual void saveProperties(KviConfigurationFile * cfg);
	virtual void loadProperties(KviConfigurationFile * cfg);
public:
	QTextBrowser * textBrowser();
public slots:
	void indexSelected ( QListWidgetItem * );
	void searchInIndex( const QString &s );
	void showIndexTopic();
	void startSearch();
	void searchSelected ( QListWidgetItem * );
	void refreshIndex();
	
	void initialSetup();
	void indexingStart( int iNum );
	void indexingProgress( int iNum );
	void indexingEnd();
};

#endif //_KVI_HELPWINDOW_H_
