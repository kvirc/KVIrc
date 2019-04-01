//=============================================================================
//
//   File : HelpWindow.cpp
//   Creation date : Tue Aug 11 2000 18:08:22 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the m_terms of the GNU General Public License
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

#include "HelpWindow.h"
#include "HelpWidget.h"

#include "kvi_settings.h"
#include "KviApplication.h"
#include "KviIconManager.h"
#include "KviOptions.h"
#include "KviLocale.h"
#include "KviModule.h"
#include "KviConfigurationFile.h"
#include "kvi_sourcesdate.h"

#include <QList>
#include <QFileInfo>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRegExp>
#include <QToolTip>
#include <QTimer>

extern HelpIndex * g_pDocIndex;
extern KviPointerList<HelpWindow> * g_pHelpWindowList;
extern KviPointerList<HelpWidget> * g_pHelpWidgetList;

//bool g_bIndexingDone = false;
HelpWindow::HelpWindow(const char * name)
    : KviWindow(KviWindow::Help, name)
{
	g_pHelpWindowList->append(this);
	m_pSplitter = new KviTalSplitter(Qt::Horizontal, this);
	m_pSplitter->setObjectName("main_splitter");
	m_pSplitter->setChildrenCollapsible(false);

	m_pHelpWidget = new HelpWidget(m_pSplitter);

	m_pToolBar = new KviTalVBox(m_pSplitter);
	m_pTabWidget = new QTabWidget(m_pToolBar);

	m_pBottomLayout = new KviTalHBox(m_pToolBar);
	m_pProgressBar = new QProgressBar(m_pBottomLayout);
	m_pCancelButton = new QPushButton(m_pBottomLayout);
	m_pCancelButton->setText(__tr2qs_ctx("Cancel", "logview"));
	connect(m_pCancelButton, SIGNAL(clicked()), g_pDocIndex, SLOT(setLastWinClosed()));
	m_pBottomLayout->setVisible(false);

	m_pIndexTab = new KviTalVBox(m_pTabWidget);
	m_pTabWidget->addTab(m_pIndexTab, __tr2qs("Help Index"));

	KviTalHBox * pSearchBox = new KviTalHBox(m_pIndexTab);
	m_pIndexSearch = new QLineEdit(pSearchBox);
	connect(m_pIndexSearch, SIGNAL(textChanged(const QString &)), this, SLOT(searchInIndex(const QString &)));
	connect(m_pIndexSearch, SIGNAL(returnPressed()), this, SLOT(showIndexTopic()));

	m_pBtnRefreshIndex = new QPushButton(pSearchBox);
	m_pBtnRefreshIndex->setIcon(*g_pIconManager->getBigIcon(KVI_REFRESH_IMAGE_NAME));
	connect(m_pBtnRefreshIndex, SIGNAL(clicked()), this, SLOT(refreshIndex()));
	m_pBtnRefreshIndex->setToolTip(__tr2qs("Refresh index"));

	m_pIndexListWidget = new KviTalListWidget(m_pIndexTab);
	connect(m_pIndexListWidget, SIGNAL(itemActivated(QListWidgetItem *)), this, SLOT(indexSelected(QListWidgetItem *)));

	m_pSearchTab = new KviTalVBox(m_pTabWidget);
	m_pTabWidget->addTab(m_pSearchTab, __tr2qs("Search"));

	m_pTermsEdit = new QLineEdit(m_pSearchTab);

	connect(m_pTermsEdit, SIGNAL(returnPressed()), this, SLOT(startSearch()));

	m_pResultBox = new KviTalListWidget(m_pSearchTab);
	connect(m_pResultBox, SIGNAL(itemActivated(QListWidgetItem *)), this, SLOT(searchSelected(QListWidgetItem *)));

	QList<int> li;
	li.append(width() - 80);
	li.append(80);
	m_pSplitter->setSizes(li);

	connect(g_pDocIndex, SIGNAL(indexingStart(int)), this, SLOT(indexingStart(int)));
	connect(g_pDocIndex, SIGNAL(indexingProgress(int)), this, SLOT(indexingProgress(int)));
	connect(g_pDocIndex, SIGNAL(indexingEnd()), this, SLOT(indexingEnd()));

	QTimer::singleShot(0, this, SLOT(initialSetup()));
}

HelpWindow::~HelpWindow()
{
	g_pHelpWindowList->removeRef(this);
}

void HelpWindow::initialSetup()
{
	m_pIndexSearch->setFocus();

	QString szDoclist, szDict;

	g_pApp->getLocalKvircDirectory(szDoclist, KviApplication::Help, "help.doclist." KVI_SOURCES_DATE);
	g_pApp->getLocalKvircDirectory(szDict, KviApplication::Help, "help.dict." KVI_SOURCES_DATE);

	if(QFileInfo(szDoclist).exists() && QFileInfo(szDict).exists())
	{
		g_pDocIndex->readDict();
		m_pIndexListWidget->clear();
		QStringList docList = g_pDocIndex->titlesList();
		m_pIndexListWidget->addItems(docList);
		m_pIndexListWidget->sortItems();
		m_pBtnRefreshIndex->setEnabled(true);
	}
	else
		g_pDocIndex->makeIndex();
}

void HelpWindow::indexingStart(int iNum)
{
	m_pBtnRefreshIndex->setEnabled(false);
	m_pBottomLayout->setVisible(true);
	m_pProgressBar->setRange(0, iNum);
	m_pProgressBar->setValue(0);
}

void HelpWindow::indexingProgress(int iNum)
{
	m_pProgressBar->setValue(iNum);
}

void HelpWindow::indexingEnd()
{
	m_pProgressBar->setValue(0);
	m_pBottomLayout->setVisible(false);
	g_pDocIndex->writeDict();
	m_pIndexListWidget->clear();
	QStringList docList = g_pDocIndex->titlesList();
	m_pIndexListWidget->addItems(docList);
	m_pIndexListWidget->sortItems();
	m_pBtnRefreshIndex->setEnabled(true);
}

void HelpWindow::saveProperties(KviConfigurationFile * cfg)
{
	KviWindow::saveProperties(cfg);
	cfg->writeEntry("Splitter", m_pSplitter->sizes());
}

void HelpWindow::loadProperties(KviConfigurationFile * cfg)
{
	QList<int> def;
	int w = width();
	def.append((w * 82) / 100);
	def.append((w * 18) / 100);
	m_pSplitter->setSizes(cfg->readIntListEntry("Splitter", def));
	KviWindow::loadProperties(cfg);
}

void HelpWindow::refreshIndex()
{
	g_pDocIndex->makeIndex();
}

void HelpWindow::startSearch()
{
	QString str = m_pTermsEdit->text();
	str = str.replace("\'", "\"");
	str = str.replace("`", "\"");
	QString buf = str;
	str = str.replace("-", " ");
	str = str.replace(QRegExp(R"(\s[\S]?\s)"), " ");
	m_terms = str.split(" ", QString::SkipEmptyParts);
	QStringList termSeq;
	QStringList seqWords;
	QStringList::iterator it = m_terms.begin();
	for(; it != m_terms.end(); ++it)
	{
		(*it) = (*it).simplified();
		(*it) = (*it).toLower();
		(*it) = (*it).replace("\"", "");
	}
	if(str.contains('\"'))
	{
		if((str.count('\"')) % 2 == 0)
		{
			int beg = 0;
			int end = 0;
			QString s;
			beg = str.indexOf('\"', beg);
			while(beg != -1)
			{
				beg++;
				end = str.indexOf('\"', beg);
				s = str.mid(beg, end - beg);
				s = s.toLower();
				s = s.simplified();
				if(s.contains('*'))
				{
					QMessageBox::warning(this, __tr2qs("Full Text Search - KVIrc"),
					    __tr2qs("Using a wildcard within phrases is not allowed."));
					return;
				}
				seqWords += s.split(' ', QString::SkipEmptyParts);
				termSeq << s;
				beg = str.indexOf('\"', end + 1);
			}
		}
		else
		{
			QMessageBox::warning(this, __tr2qs("Full Text Search - KVIrc"),
			    __tr2qs("The closing quotation mark is missing."));
			return;
		}
	}
	setCursor(Qt::WaitCursor);
	m_foundDocs.clear();
	m_foundDocs = g_pDocIndex->query(m_terms, termSeq, seqWords);

	m_pResultBox->clear();
	for(it = m_foundDocs.begin(); it != m_foundDocs.end(); ++it)
		m_pResultBox->addItem(g_pDocIndex->getDocumentTitle(*it));

	m_terms.clear();
	bool isPhrase = false;
	QString s = "";
	for(const auto & i : buf)
	{
		if(i == '\"')
		{
			isPhrase = !isPhrase;
			s = s.simplified();
			if(!s.isEmpty())
				m_terms << s;
			s = "";
		}
		else if(i == ' ' && !isPhrase)
		{
			s = s.simplified();
			if(!s.isEmpty())
				m_terms << s;
			s = "";
		}
		else
			s += i;
	}
	if(!s.isEmpty())
		m_terms << s;

	setCursor(Qt::ArrowCursor);
}

#ifdef COMPILE_WEBKIT_SUPPORT
QWebView * HelpWindow::textBrowser()
#else
QTextBrowser * HelpWindow::textBrowser()
#endif
{
	return m_pHelpWidget->textBrowser();
}

void HelpWindow::showIndexTopic()
{
	if(m_pIndexSearch->text().isEmpty() || !m_pIndexListWidget->selectedItems().count())
		return;
	int i = g_pDocIndex->titlesList().indexOf(m_pIndexListWidget->selectedItems().at(0)->text());
#ifdef COMPILE_WEBKIT_SUPPORT
	textBrowser()->load(QUrl(g_pDocIndex->documentList()[i]));
#else
	textBrowser()->setSource(QUrl(g_pDocIndex->documentList()[i]));
#endif
}

void HelpWindow::searchInIndex(const QString & s)
{
	QListWidgetItem * item;
	QString sl = s.toLower();
	for(int i = 0; i < m_pIndexListWidget->count(); i++)
	{
		item = m_pIndexListWidget->item(i);
		QString t = item->text();
		if(t.length() >= sl.length() && item->text().left(s.length()).toLower() == sl)
		{
			m_pIndexListWidget->setCurrentItem(item);
			m_pIndexListWidget->scrollToItem(item, QAbstractItemView::PositionAtTop);
			break;
		}
	}
}

void HelpWindow::indexSelected(QListWidgetItem * item)
{
	if(!item)
		return;
	int i = g_pDocIndex->titlesList().indexOf(item->text());
#ifdef COMPILE_WEBKIT_SUPPORT
	textBrowser()->load(QUrl(g_pDocIndex->documentList()[i]));
#else
	textBrowser()->setSource(QUrl(g_pDocIndex->documentList()[i]));
#endif
}

void HelpWindow::searchSelected(QListWidgetItem * item)
{
	if(!item)
		return;
	int i = g_pDocIndex->titlesList().indexOf(item->text());
#ifdef COMPILE_WEBKIT_SUPPORT
	textBrowser()->load(QUrl(g_pDocIndex->documentList()[i]));
#else
	textBrowser()->setSource(QUrl(g_pDocIndex->documentList()[i]));
#endif
}

QPixmap * HelpWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KviIconManager::QuestionMark);
}

void HelpWindow::resizeEvent(QResizeEvent *)
{
	m_pSplitter->setGeometry(0, 0, width(), height());
}

void HelpWindow::fillCaptionBuffers()
{
	m_szPlainTextCaption = __tr2qs("Help Browser");
}
