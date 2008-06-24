//
//   File : helpwidget.cpp
//   Creation date : Thu Aug 10 2000 17:42:12 by Szymon Stefanek
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

#include "helpwidget.h"
#include "helpwindow.h"

#include "kvi_frame.h"
#include "kvi_iconmanager.h"
#include "kvi_locale.h"
#include "kvi_app.h"
#include "kvi_msgbox.h"
#include "kvi_module.h"
#include "kvi_styled_controls.h"
#include "kvi_fileutils.h"

#include <QToolButton>
#include <QLineEdit>
#include <QToolTip>
#include <QTimer>
#include <QClipboard>
#include <QDir>

extern Index        * g_pDocIndex;
extern KviPointerList<KviHelpWindow> * g_pHelpWindowList;
extern KviPointerList<KviHelpWidget> * g_pHelpWidgetList;

KviHelpWidget::KviHelpWidget(QWidget * par,KviFrame * lpFrm,bool bIsStandalone)
: QWidget(par)
{
	setObjectName("help_widget");
	if(bIsStandalone)g_pHelpWidgetList->append(this);
	m_bIsStandalone = bIsStandalone;

	m_pTextBrowser = new QTextBrowser(this);
	m_pTextBrowser->setObjectName("text_browser");
	m_pTextBrowser->setFrameStyle(QFrame::StyledPanel|QFrame::Sunken);
	m_pTextBrowser->setFocusPolicy(Qt::NoFocus);
	m_pToolBar = new KviTalHBox(this);

	m_pBtnIndex = new KviStyledToolButton(m_pToolBar);
	m_pBtnIndex->setIcon(*g_pIconManager->getBigIcon(KVI_BIGICON_HELPINDEX));
	connect(m_pBtnIndex,SIGNAL(clicked()),this,SLOT(showIndex()));
	//m_pBtnIndex->setUsesBigPixmap(true);

	m_pBtnBackward = new KviStyledToolButton(m_pToolBar);
	m_pBtnBackward->setIcon(*g_pIconManager->getBigIcon(KVI_BIGICON_HELPBACK));
	connect(m_pBtnBackward,SIGNAL(clicked()),m_pTextBrowser,SLOT(backward()));
	m_pBtnBackward->setEnabled(false);
	//m_pBtnBackward->setUsesBigPixmap(true);

	m_pBtnForward = new KviStyledToolButton(m_pToolBar);
	m_pBtnForward->setIcon(*g_pIconManager->getBigIcon(KVI_BIGICON_HELPFORWARD));
	connect(m_pBtnForward,SIGNAL(clicked()),m_pTextBrowser,SLOT(forward()));
	m_pBtnForward->setEnabled(false);
	//m_pBtnForward->setUsesBigPixmap(true);

	QWidget* pSpacer=new QWidget(m_pToolBar);

	if(bIsStandalone)
	{
		QToolButton * b = new KviStyledToolButton(m_pToolBar);
		b->setIcon(*g_pIconManager->getBigIcon(KVI_BIGICON_HELPCLOSE));
		connect(b,SIGNAL(clicked()),this,SLOT(doClose()));
		//b->setUsesBigPixmap(true);
	}

	m_pToolBar->setStretchFactor(pSpacer,1);
	connect(m_pTextBrowser,SIGNAL(backwardAvailable(bool)),m_pBtnBackward,SLOT(setEnabled(bool)));
	connect(m_pTextBrowser,SIGNAL(forwardAvailable(bool)),m_pBtnForward,SLOT(setEnabled(bool)));

	m_pTextBrowser->viewport()->installEventFilter(this);
}

KviHelpWidget::~KviHelpWidget()
{
	if(m_bIsStandalone)g_pHelpWidgetList->removeRef(this);
}

void KviHelpWidget::showIndex()
{
	QString szHelpDir;
        QDir dirHelp;

	g_pApp->getGlobalKvircDirectory(szHelpDir,KviApp::Help);
        dirHelp = QDir(szHelpDir);

	m_pTextBrowser->setSource(QUrl::fromLocalFile(dirHelp.absoluteFilePath("index.html")));
}

void KviHelpWidget::resizeEvent(QResizeEvent *e)
{
	int hght = m_pToolBar->sizeHint().height();
	if(hght < 40)hght = 40;
	m_pToolBar->setGeometry(0,0,width(),hght);
	m_pTextBrowser->setGeometry(0,hght,width(),height() - hght);
}

void KviHelpWidget::doClose()
{
	// hack needed to workaround "QToolBar::emulateButtonClicked()"
	// that refers to the "this" pointer after this slot has been
	// called (from the "too-small-toolbar-for-all-items-popup")
	QTimer::singleShot(0,this,SLOT(suicide()));
}

void KviHelpWidget::suicide()
{
	// goodbye cruel wolrd
	delete this;
}

QSize KviHelpWidget::sizeHint() const
{
	int wdth = m_pTextBrowser->sizeHint().width();
	if(m_pToolBar->sizeHint().width() > wdth)wdth = m_pToolBar->sizeHint().width();
	QSize s(wdth,m_pTextBrowser->sizeHint().height() + m_pToolBar->sizeHint().height());
	return s;
}

bool KviHelpWidget::eventFilter(QObject * o, QEvent *e)
{
	QClipboard *cb = QApplication::clipboard();

	if(e->type() == QEvent::MouseButtonRelease) {
		if(m_pTextBrowser->textCursor().hasSelection()) {
			cb->setText(m_pTextBrowser->textCursor().selectedText());
		}
	}

	return QWidget::eventFilter(o,e);
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "helpwidget.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
