#ifndef _HELPWINDOW_H_
#define _HELPWINDOW_H_
//=============================================================================
//
//   File : HelpWindow.h
//   Creation date : Fri Aug 11 2000 18:05:59 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviWindow.h"
#include "KviCString.h"
#include "KviTalVBox.h"
#include "KviTalListWidget.h"
#include "kvi_settings.h"

#include <QTabWidget>
#ifdef COMPILE_WEBKIT_SUPPORT
#include <QtWebKitWidgets/QWebView>
#else
class QTextBrowser;
#endif

#include <QLineEdit>

class QProgressBar;
class QPushButton;

class HelpWidget;

class HelpWindow : public KviWindow
{
	Q_OBJECT
public:
	HelpWindow(const char * name);
	~HelpWindow();

protected:
	HelpWidget * m_pHelpWidget;
	KviTalVBox * m_pToolBar;
	QTabWidget * m_pTabWidget;
	KviTalVBox * m_pIndexTab;
	KviTalVBox * m_pSearchTab;
	KviTalHBox * m_pBottomLayout;
	QPushButton * m_pCancelButton;
	QProgressBar * m_pProgressBar;

	KviTalListWidget * m_pIndexListWidget;
	QLineEdit * m_pIndexSearch;
	QStringList m_foundDocs;
	QStringList m_terms;
	KviTalListWidget * m_pResultBox;
	QLineEdit * m_pTermsEdit;
	QPushButton * m_pBtnRefreshIndex;

public:
	HelpWidget * helpWidget() { return m_pHelpWidget; };
protected:
	QPixmap * myIconPtr() override;
	void fillCaptionBuffers() override;
	void resizeEvent(QResizeEvent * e) override;
	void saveProperties(KviConfigurationFile * cfg) override;
	void loadProperties(KviConfigurationFile * cfg) override;

public:
#ifdef COMPILE_WEBKIT_SUPPORT
	QWebView * textBrowser();
#else
	QTextBrowser * textBrowser();
#endif
public slots:
	void indexSelected(QListWidgetItem *);
	void searchInIndex(const QString & s);
	void showIndexTopic();
	void startSearch();
	void searchSelected(QListWidgetItem *);
	void refreshIndex();

	void initialSetup();
	void indexingStart(int iNum);
	void indexingProgress(int iNum);
	void indexingEnd();
};

#endif //_KVI_HELPWINDOW_H_
