//=============================================================================
//
//   File : HelpWidget.cpp
//   Creation date : Thu Aug 10 2000 17:42:12 by Szymon Stefanek
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

#include "HelpWidget.h"
#include "HelpWindow.h"

#include "KviMainWindow.h"
#include "KviIconManager.h"
#include "KviLocale.h"
#include "KviApplication.h"
#include "KviMessageBox.h"
#include "KviModule.h"
#include "KviFileUtils.h"

#include <QToolButton>
#include <QLineEdit>
#include <QToolTip>
#include <QTimer>
#include <QClipboard>
#include <QDir>
#include <QAction>

extern HelpIndex * g_pDocIndex;
extern KviPointerList<HelpWindow> * g_pHelpWindowList;
extern KviPointerList<HelpWidget> * g_pHelpWidgetList;

HelpWidget::HelpWidget(QWidget * par, bool bIsStandalone)
    : QWidget(par)
{
	setObjectName("help_widget");
	setMinimumWidth(80);
	if(bIsStandalone)
		g_pHelpWidgetList->append(this);
	m_bIsStandalone = bIsStandalone;

	// layout
	m_pLayout = new QVBoxLayout(this);
	m_pLayout->setContentsMargins(0, 0, 0, 0);
	m_pLayout->setSpacing(0);
	setLayout(m_pLayout);

	m_pTextBrowser = new QTextBrowser(this);
	m_pTextBrowser->setObjectName("text_browser");
	m_pTextBrowser->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	m_pTextBrowser->setStyleSheet("QTextBrowser { background-color:white; color:black; }");

	m_pToolBar = new QToolBar(this);
	m_pToolBar->addAction(*g_pIconManager->getBigIcon(KVI_BIGICON_HELPINDEX), __tr2qs("Show index"), this, SLOT(showIndex()));

	m_pBackAction = new QAction(*g_pIconManager->getBigIcon(KVI_BIGICON_HELPBACK), __tr2qs("Back"), this);
	m_pBackAction->setEnabled(false);
	connect(m_pBackAction, SIGNAL(triggered()), m_pTextBrowser, SLOT(backward()));
	m_pToolBar->addAction(m_pBackAction);

	m_pForwardAction = new QAction(*g_pIconManager->getBigIcon(KVI_BIGICON_HELPFORWARD), __tr2qs("Forward"), this);
	m_pForwardAction->setEnabled(false);
	connect(m_pForwardAction, SIGNAL(triggered()), m_pTextBrowser, SLOT(forward()));
	m_pToolBar->addAction(m_pForwardAction);

	if(bIsStandalone)
	{
		setAttribute(Qt::WA_DeleteOnClose);

		m_pToolBar->addAction(*g_pIconManager->getBigIcon(KVI_BIGICON_HELPCLOSE), __tr2qs("Close"), this, SLOT(close()));
	}

	m_pLayout->addWidget(m_pToolBar);
	m_pLayout->addWidget(m_pTextBrowser);

	connect(m_pTextBrowser, SIGNAL(backwardAvailable(bool)), m_pBackAction, SLOT(setEnabled(bool)));
	connect(m_pTextBrowser, SIGNAL(forwardAvailable(bool)), m_pForwardAction, SLOT(setEnabled(bool)));
}

HelpWidget::~HelpWidget()
{
	if(m_bIsStandalone)
		g_pHelpWidgetList->removeRef(this);
}

void HelpWidget::showIndex()
{
	QString szHelpDir;
	QDir dirHelp;

	g_pApp->getGlobalKvircDirectory(szHelpDir, KviApplication::Help);
	dirHelp = QDir(szHelpDir);
	m_pTextBrowser->setSource(QUrl::fromLocalFile(dirHelp.absoluteFilePath("index.html")));
}
