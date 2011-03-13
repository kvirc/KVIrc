//=============================================================================
//
//   File : HelpWidget.cpp
//   Creation date : Thu Aug 10 2000 17:42:12 by Szymon Stefanek
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

extern HelpIndex        * g_pDocIndex;
extern KviPointerList<HelpWindow> * g_pHelpWindowList;
extern KviPointerList<HelpWidget> * g_pHelpWidgetList;

#ifdef COMPILE_WEBKIT_SUPPORT
#include <QtWebKit/QWebView>

#if QT_VERSION >= 0x040600
	#define HIGHLIGHT_FLAGS QWebPage::HighlightAllOccurrences
#else
	#define HIGHLIGHT_FLAGS 0
#endif

HelpWidget::HelpWidget(QWidget * par,KviMainWindow *,bool bIsStandalone)
: QWidget(par)
{
	m_pLayout = new QVBoxLayout(this);
	m_pLayout->setMargin(3);
	m_pLayout->setSpacing(2);
	setLayout(m_pLayout);
	m_pToolBar = new QToolBar(this);
	m_pLayout->addWidget(m_pToolBar);
	setObjectName("help_widget");
	setMinimumWidth(80);
	if(bIsStandalone)g_pHelpWidgetList->append(this);
	m_bIsStandalone = bIsStandalone;

	m_pTextBrowser = new QWebView(this);
	m_pLayout->addWidget(m_pTextBrowser);
	QLabel *pHighlightLabel = new QLabel();
	pHighlightLabel->setText(__tr2qs("Highlight: "));
	m_pToolBar->addWidget(pHighlightLabel);


	m_pFindText = new QLineEdit();
	m_pToolBar->addWidget(m_pFindText);
	connect(m_pFindText,SIGNAL(textChanged(const QString )),this,SLOT(slotTextChanged(const QString)));

	connect(m_pTextBrowser,SIGNAL(loadFinished(bool)),this,SLOT(slotLoadFinished(bool)));

	m_pBtnResetFind = new QToolButton();
	m_pBtnResetFind->setIcon(*g_pIconManager->getImage("41"));
	m_pToolBar->addWidget(m_pBtnResetFind);
	connect(m_pBtnResetFind ,SIGNAL(clicked()),this,SLOT(slotResetFind()));


	m_pBtnFindPrev = new QToolButton();
	m_pBtnFindPrev->setIcon(*g_pIconManager->getImage("40"));
	m_pToolBar->addWidget(m_pBtnFindPrev);
	connect(m_pBtnFindPrev,SIGNAL(clicked()),this,SLOT(slotFindPrev()));

	m_pBtnFindNext = new QToolButton();
	m_pBtnFindNext->setIcon(*g_pIconManager->getImage("39"));
	m_pToolBar->addWidget(m_pBtnFindNext);
	connect(m_pBtnFindNext,SIGNAL(clicked()),this,SLOT(slotFindNext()));

	m_pTextBrowser->setObjectName("text_browser");
	m_pTextBrowser->setStyleSheet("QTextBrowser { background-color:white; color:black; }");

	QLabel *pBrowsingLabel = new QLabel();
	pBrowsingLabel->setText(__tr2qs("Browsing: "));
	m_pToolBar->addWidget(pBrowsingLabel);

	m_pBtnIndex = new QToolButton();
	m_pBtnIndex->setIcon(*g_pIconManager->getBigIcon(KVI_BIGICON_HELPINDEX));

	connect(m_pBtnIndex,SIGNAL(clicked()),this,SLOT(showIndex()));
	m_pToolBar->addWidget(m_pBtnIndex);

	m_pToolBar->addAction(m_pTextBrowser->pageAction(QWebPage::Back));
	m_pToolBar->addAction(m_pTextBrowser->pageAction(QWebPage::Forward));

	m_pBtnZoomIn = new QToolButton();
	m_pBtnZoomIn->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Plus)));
	m_pToolBar->addWidget(m_pBtnZoomIn);

	connect(m_pBtnZoomIn,SIGNAL(clicked()),this,SLOT(slotZoomIn()));

	m_pBtnZoomOut = new QToolButton();
	m_pBtnZoomOut->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Minus)));
	m_pToolBar->addWidget(m_pBtnZoomOut);

	connect(m_pBtnZoomOut,SIGNAL(clicked()),this,SLOT(slotZoomOut()));
	if(bIsStandalone)
	{
		QToolButton * b = new QToolButton();
		m_pToolBar->addWidget(b);
		b->setIcon(*g_pIconManager->getBigIcon(KVI_BIGICON_HELPCLOSE));
		connect(b,SIGNAL(clicked()),this,SLOT(doClose()));
	}

}

void HelpWidget::slotLoadFinished(bool )
{
	m_pTextBrowser->findText(m_pFindText->text(),HIGHLIGHT_FLAGS);
}

void HelpWidget::slotTextChanged(const QString szFind)
{
	m_pTextBrowser->findText("",HIGHLIGHT_FLAGS);
	m_pTextBrowser->findText(szFind,HIGHLIGHT_FLAGS);
}

void HelpWidget::slotResetFind()
{
	m_pFindText->setText("");
	m_pTextBrowser->findText("",HIGHLIGHT_FLAGS);
}

void HelpWidget::slotFindPrev()
{
	m_pTextBrowser->findText(m_pFindText->text(),QWebPage::FindBackward);
}

void HelpWidget::slotFindNext()
{
	m_pTextBrowser->findText(m_pFindText->text());
}
void HelpWidget::slotZoomIn()
{
	kvs_real_t dZoom=m_pTextBrowser->zoomFactor();
	if(dZoom>=2) return;
	dZoom+= 0.05;
	m_pTextBrowser->setZoomFactor(dZoom);
}

void HelpWidget::slotZoomOut()
{
	kvs_real_t dZoom=m_pTextBrowser->zoomFactor();
	if(dZoom<=0.5) return;
	dZoom-= 0.05;
	m_pTextBrowser->setZoomFactor(dZoom);
}

#else

HelpWidget::HelpWidget(QWidget * par,KviMainWindow *,bool bIsStandalone)
: QWidget(par)
{
	setObjectName("help_widget");
	setMinimumWidth(80);
	if(bIsStandalone)g_pHelpWidgetList->append(this);
	m_bIsStandalone = bIsStandalone;

	m_pTextBrowser = new QTextBrowser(this);
	m_pTextBrowser->setObjectName("text_browser");
	m_pTextBrowser->setFrameStyle(QFrame::StyledPanel|QFrame::Sunken);
	m_pTextBrowser->setStyleSheet("QTextBrowser { background-color:white; color:black; }");
	m_pToolBar = new KviTalHBox(this);

	m_pBtnIndex = new QToolButton(m_pToolBar);
	m_pBtnIndex->setIcon(*g_pIconManager->getBigIcon(KVI_BIGICON_HELPINDEX));
	connect(m_pBtnIndex,SIGNAL(clicked()),this,SLOT(showIndex()));

	m_pBtnBackward = new QToolButton(m_pToolBar);
	m_pBtnBackward->setIcon(*g_pIconManager->getBigIcon(KVI_BIGICON_HELPBACK));
	connect(m_pBtnBackward,SIGNAL(clicked()),m_pTextBrowser,SLOT(backward()));
	m_pBtnBackward->setEnabled(false);

	m_pBtnForward = new QToolButton(m_pToolBar);
	m_pBtnForward->setIcon(*g_pIconManager->getBigIcon(KVI_BIGICON_HELPFORWARD));
	connect(m_pBtnForward,SIGNAL(clicked()),m_pTextBrowser,SLOT(forward()));
	m_pBtnForward->setEnabled(false);

	QWidget* pSpacer=new QWidget(m_pToolBar);

	if(bIsStandalone)
	{
		QToolButton * b = new QToolButton(m_pToolBar);
		b->setIcon(*g_pIconManager->getBigIcon(KVI_BIGICON_HELPCLOSE));
		connect(b,SIGNAL(clicked()),this,SLOT(doClose()));
	}

	m_pToolBar->setStretchFactor(pSpacer,1);
	connect(m_pTextBrowser,SIGNAL(backwardAvailable(bool)),m_pBtnBackward,SLOT(setEnabled(bool)));
	connect(m_pTextBrowser,SIGNAL(forwardAvailable(bool)),m_pBtnForward,SLOT(setEnabled(bool)));
}

#endif

HelpWidget::~HelpWidget()
{
	if(m_bIsStandalone)g_pHelpWidgetList->removeRef(this);
}

void HelpWidget::showIndex()
{
	QString szHelpDir;
        QDir dirHelp;

	g_pApp->getGlobalKvircDirectory(szHelpDir,KviApplication::Help);
        dirHelp = QDir(szHelpDir);
	#ifdef COMPILE_WEBKIT_SUPPORT
	m_pTextBrowser->load(QUrl::fromLocalFile(dirHelp.absoluteFilePath("index.html")));
	#else
	m_pTextBrowser->setSource(QUrl::fromLocalFile(dirHelp.absoluteFilePath("index.html")));
	#endif
}

void HelpWidget::resizeEvent(QResizeEvent *)
{
	int hght = m_pToolBar->sizeHint().height();
	if(hght < 40)hght = 40;
	m_pToolBar->setGeometry(0,0,width(),hght);
	m_pTextBrowser->setGeometry(0,hght,width(),height() - hght);
}

void HelpWidget::doClose()
{
	// hack needed to workaround "QToolBar::emulateButtonClicked()"
	// that refers to the "this" pointer after this slot has been
	// called (from the "too-small-toolbar-for-all-items-popup")
	QTimer::singleShot(0,this,SLOT(suicide()));
}

void HelpWidget::suicide()
{
	// goodbye cruel wolrd
	delete this;
}

QSize HelpWidget::sizeHint() const
{
	int wdth = m_pTextBrowser->sizeHint().width();
	if(m_pToolBar->sizeHint().width() > wdth)wdth = m_pToolBar->sizeHint().width();
	QSize s(wdth,m_pTextBrowser->sizeHint().height() + m_pToolBar->sizeHint().height());
	return s;
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "HelpWidget.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
