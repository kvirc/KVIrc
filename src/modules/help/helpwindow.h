#ifndef _HELPWINDOW_H_
#define _HELPWINDOW_H_
//
//   File : helpwindow.h
//   Creation date : Fri Aug 11 2000 18:05:59 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_string.h"
#include <qvbox.h>
#include <qtabwidget.h> 
#include <qlistbox.h>
#include <qlineedit.h>
#include <qprogressdialog.h>

class KviHelpWidget;
class QTextBrowser;

class KviHelpWindow : public KviWindow
{
	Q_OBJECT
public:
	KviHelpWindow(KviFrame * lpFrm,const char * name);
	~KviHelpWindow();
protected:
	KviHelpWidget * m_pHelpWidget;
	QVBox	      * m_pToolBar;
	QTabWidget    * m_pTabWidget;
	QVBox         * m_pIndexTab;
	QVBox         * m_pSearchTab;
	
	QListBox     * m_pIndexListBox;
	QLineEdit    * m_pIndexSearch;
	QStringList    m_foundDocs;
	QStringList    m_terms;
	QListBox     * m_pResultBox;
	QLineEdit    * m_pTermsEdit;
public:
	KviHelpWidget * helpWidget(){ return m_pHelpWidget; };
protected:
	virtual QPixmap * myIconPtr();
	virtual void fillCaptionBuffers();
	virtual void resizeEvent(QResizeEvent *e);
	virtual void saveProperties(KviConfig * cfg);
	virtual void loadProperties(KviConfig * cfg);
public:
	virtual QSize sizeHint() const;
	QTextBrowser * textBrowser();
public slots:
	void indexSelected ( int );
	void searchInIndex( const QString &s );
	void showIndexTopic();
	void startSearch();
	void searchSelected ( int index );
	void refreshIndex();
};

#endif //_KVI_HELPWINDOW_H_
