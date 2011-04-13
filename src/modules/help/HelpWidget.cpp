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

#define HIGHLIGHT_FLAGS QWebPage::HighlightAllOccurrences

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

	m_pToolBar->addAction(*g_pIconManager->getSmallIcon(KviIconManager::Unrecognized), __tr2qs("Reset"), this, SLOT(slotResetFind()));
	m_pToolBar->addAction(*g_pIconManager->getSmallIcon(KviIconManager::Part), __tr2qs("Find previous"), this, SLOT(slotFindPrev()));
	m_pToolBar->addAction(*g_pIconManager->getSmallIcon(KviIconManager::Join), __tr2qs("Find next"), this, SLOT(slotFindNext()));

	m_pTextBrowser->setObjectName("text_browser");
	m_pTextBrowser->setStyleSheet("QTextBrowser { background-color:white; color:black; }");

	QLabel *pBrowsingLabel = new QLabel();
	pBrowsingLabel->setText(__tr2qs("Browsing: "));
	m_pToolBar->addWidget(pBrowsingLabel);

	m_pToolBar->addAction(*g_pIconManager->getBigIcon(KVI_BIGICON_HELPINDEX), __tr2qs("Show index"), this, SLOT(showIndex()));

	m_pToolBar->addAction(m_pTextBrowser->pageAction(QWebPage::Back));
	m_pToolBar->addAction(m_pTextBrowser->pageAction(QWebPage::Forward));

	m_pToolBar->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Plus)), __tr2qs("Zoom in"), this, SLOT(slotZoomIn()));
	m_pToolBar->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Minus)), __tr2qs("Zoom out"), this, SLOT(slotZoomOut()));

	if(bIsStandalone)
	{
		setAttribute(Qt::WA_DeleteOnClose);
		m_pToolBar->addAction(*g_pIconManager->getBigIcon(KVI_BIGICON_HELPCLOSE), __tr2qs("Close"), this, SLOT(close()));
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
		setAttribute(Qt::WA_DeleteOnClose);
		QToolButton * b = new QToolButton(m_pToolBar);
		b->setIcon(*g_pIconManager->getBigIcon(KVI_BIGICON_HELPCLOSE));
		connect(b,SIGNAL(clicked()),this,SLOT(close()));
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
