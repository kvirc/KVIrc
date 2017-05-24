#ifndef _HELPWIDGET_H_
#define _HELPWIDGET_H_
//=============================================================================
//
//   File : HelpWidget.h
//   Creation date : Thu Aug 10 2000 17:26:20 by Szymon Stefanek
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

#include "HelpIndex.h"
#include "kvi_settings.h"

#ifdef COMPILE_WEBKIT_SUPPORT
#include <QtWebKitWidgets/QWebView>
#else
#include <QTextBrowser>
#endif
#include <QToolBar>
#include <QVBoxLayout>
#include <QProgressBar>

class QToolButton;
class QLineEdit;
class KviMainWindow;

class HelpWidget : public QWidget
{
	Q_OBJECT
public:
	HelpWidget(QWidget * par, bool bIsStandalone = false);
	~HelpWidget();

private:
#ifdef COMPILE_WEBKIT_SUPPORT
	QVBoxLayout * m_pLayout;
	QToolBar * m_pToolBar;
	QToolBar * m_pToolBarHighlight;
	QLineEdit * m_pFindText;
	QWebView * m_pTextBrowser;
#else
	QVBoxLayout * m_pLayout;
	QToolBar * m_pToolBar;
	QAction * m_pBackAction;
	QAction * m_pForwardAction;
	QTextBrowser * m_pTextBrowser;
#endif
	bool m_bIsStandalone;

protected slots:
	void showIndex();
#ifdef COMPILE_WEBKIT_SUPPORT
	void slotLoadFinished(bool ok);
	void slotFindNext();
	void slotFindPrev();
	void slotZoomIn();
	void slotZoomOut();
	void slotTextChanged(const QString);
	void slotCopy();
	void slotShowHideFind();
#endif
public:
#ifdef COMPILE_WEBKIT_SUPPORT
	QWebView * textBrowser()
	{
		return m_pTextBrowser;
	}
#else
	QTextBrowser * textBrowser()
	{
		return m_pTextBrowser;
	}
#endif
};

#endif //_HELPWIDGET_H_
