//=============================================================================
//
//   File : helpwindow.cpp
//   Creation date : Tue Aug 11 2000 18:08:22 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the m_terms of the GNU General Public License
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

#include "helpwindow.h"
#include "helpwidget.h"

#include "kvi_app.h"
#include "kvi_iconmanager.h"
#include "kvi_options.h"
#include "kvi_locale.h"
#include "kvi_module.h"
#include "kvi_config.h"
#include "kvi_sourcesdate.h"

#include <QList>
#include <QFileInfo>
#include <QLineEdit>
#include <QMessageBox>
#include <QRegExp>
#include <QToolTip>
#include <QTimer>

extern Index        * g_pDocIndex;
extern KviPointerList<KviHelpWindow> * g_pHelpWindowList;
extern KviPointerList<KviHelpWidget> * g_pHelpWidgetList;

bool g_bIndexingDone = FALSE;
KviHelpWindow::KviHelpWindow(KviFrame * lpFrm,const char * name)
: KviWindow(KVI_WINDOW_TYPE_HELP,lpFrm,name)
{
	if(!g_bIndexingDone)
	{
		g_bIndexingDone=TRUE;
		QString szDoclist,szDict;

		g_pApp->getLocalKvircDirectory(szDoclist,KviApp::Help,"help.doclist." KVI_SOURCES_DATE);
		g_pApp->getLocalKvircDirectory(szDict,KviApp::Help,"help.dict." KVI_SOURCES_DATE);

		if ( QFileInfo( szDoclist ).exists() && QFileInfo( szDict ).exists() ) {
			g_pDocIndex->readDict();
		} else {
			g_pDocIndex->makeIndex();
			g_pDocIndex->writeDict();
		}
	}

	g_pHelpWindowList->append(this);
	m_pSplitter = new KviTalSplitter(Qt::Horizontal,this);
	m_pSplitter->setObjectName("main_splitter");
	m_pSplitter->setChildrenCollapsible(false);

	m_pHelpWidget = new KviHelpWidget(m_pSplitter,lpFrm);

	m_pToolBar=new KviTalVBox(m_pSplitter);

	m_pTabWidget = new QTabWidget(m_pToolBar);

	m_pIndexTab  = new KviTalVBox(m_pTabWidget);
	m_pTabWidget->addTab(m_pIndexTab,__tr2qs("Index"));

	KviTalHBox* pSearchBox = new KviTalHBox(m_pIndexTab);
	m_pIndexSearch = new QLineEdit(pSearchBox);
	connect( m_pIndexSearch, SIGNAL( textChanged(const QString&) ),
	     this, SLOT( searchInIndex(const QString&) ) );
	connect( m_pIndexSearch, SIGNAL( returnPressed() ),
	     this, SLOT( showIndexTopic() ) );

	QToolButton* pBtnRefreshIndex = new QToolButton(pSearchBox);
	pBtnRefreshIndex->setIcon(*g_pIconManager->getBigIcon(KVI_REFRESH_IMAGE_NAME));
	connect(pBtnRefreshIndex,SIGNAL(clicked()),this,SLOT(refreshIndex()));
	pBtnRefreshIndex->setToolTip(__tr2qs("Refresh index") );

	m_pIndexListWidget = new KviTalListWidget(m_pIndexTab);
	QStringList docList=g_pDocIndex->titlesList();
	m_pIndexListWidget->addItems(docList);
	connect(m_pIndexListWidget,SIGNAL(itemActivated(QListWidgetItem *)),this,SLOT(indexSelected (QListWidgetItem * )));
	m_pIndexListWidget->sortItems();

	m_pSearchTab  = new KviTalVBox(m_pTabWidget);
	m_pTabWidget->addTab(m_pSearchTab,__tr2qs("Search"));

	m_pTermsEdit = new QLineEdit(m_pSearchTab);

	connect( m_pTermsEdit, SIGNAL( returnPressed() ),
	     this, SLOT( startSearch() ) );

	m_pResultBox = new KviTalListWidget(m_pSearchTab);
	connect(m_pResultBox,SIGNAL(itemActivated(QListWidgetItem *)),this,SLOT(searchSelected (QListWidgetItem *)));

	QList<int> li;
	li.append(width()-80);
	li.append(80);
	m_pSplitter->setSizes(li);

	QTimer::singleShot(0,m_pIndexSearch,SLOT(setFocus()));
}

KviHelpWindow::~KviHelpWindow()
{
	g_pHelpWindowList->removeRef(this);
}

void KviHelpWindow::saveProperties(KviConfig *cfg)
{
	KviWindow::saveProperties(cfg);
	cfg->writeEntry("Splitter",m_pSplitter->sizes());
}

void KviHelpWindow::loadProperties(KviConfig *cfg)
{
	QList<int> def;
	int w = width();
	def.append((w * 82) / 100);
	def.append((w * 18) / 100);
	m_pSplitter->setSizes(cfg->readIntListEntry("Splitter",def));
	KviWindow::loadProperties(cfg);
}

void KviHelpWindow::refreshIndex()
{
	m_pIndexListWidget->clear();
	g_pDocIndex->makeIndex();
	g_pDocIndex->writeDict();
	g_bIndexingDone=TRUE;
	QStringList docList=g_pDocIndex->titlesList();
	m_pIndexListWidget->addItems(docList);
	m_pIndexListWidget->sortItems();
}

void KviHelpWindow::startSearch()
{

	QString str = m_pTermsEdit->text();
	str = str.replace( "\'", "\"" );
	str = str.replace( "`", "\"" );
	QString buf = str;
	str = str.replace( "-", " " );
	str = str.replace( QRegExp( "\\s[\\S]?\\s" ), " " );
	m_terms = str.split(" ",QString::SkipEmptyParts);
	QStringList termSeq;
	QStringList seqWords;
	QStringList::iterator it = m_terms.begin();
	for ( ; it != m_terms.end(); ++it ) {
		(*it) = (*it).simplified();
		(*it) = (*it).toLower();
		(*it) = (*it).replace( "\"", "" );
	}
	if ( str.contains( '\"' ) ) {
		if ( (str.count( '\"' ))%2 == 0 ) {
			int beg = 0;
			int end = 0;
			QString s;
			beg = str.indexOf( '\"', beg );
			while ( beg != -1 ) {
				beg++;
				end = str.indexOf( '\"', beg );
				s = str.mid( beg, end - beg );
				s = s.toLower();
				s = s.simplified();
				if ( s.contains( '*' ) ) {
				QMessageBox::warning( this, tr( "Full Text Search" ),
					tr( "Using a wildcard within phrases is not allowed." ) );
				return;
				}
				seqWords += s.split( ' ', QString::SkipEmptyParts );
				termSeq << s;
				beg = str.indexOf( '\"', end + 1);
			}
		} else {
			QMessageBox::warning( this, tr( "Full Text Search" ),
				tr( "The closing quotation mark is missing." ) );
			return;
		}
	}
	setCursor( Qt::WaitCursor );
	m_foundDocs.clear();
	m_foundDocs = g_pDocIndex->query( m_terms, termSeq, seqWords );

	m_pResultBox->clear();
	for ( it = m_foundDocs.begin(); it != m_foundDocs.end(); ++it )
		m_pResultBox->addItem( g_pDocIndex->getDocumentTitle( *it ) );

	m_terms.clear();
	bool isPhrase = FALSE;
	QString s = "";
	for ( int i = 0; i < (int)buf.length(); ++i ) {
		if ( buf[i] == '\"' ) {
			isPhrase = !isPhrase;
			s = s.simplified();
			if ( !s.isEmpty() )
				m_terms << s;
			s = "";
		} else if ( buf[i] == ' ' && !isPhrase ) {
			s = s.simplified();
			if ( !s.isEmpty() )
				m_terms << s;
			s = "";
		} else
		s += buf[i];
	}
	if ( !s.isEmpty() ) m_terms << s;

	setCursor( Qt::ArrowCursor );
}

QTextBrowser * KviHelpWindow::textBrowser()
{
	return m_pHelpWidget->textBrowser();
}

void KviHelpWindow::showIndexTopic()
{
	if (m_pIndexSearch->text().isEmpty()|| !m_pIndexListWidget->selectedItems().count()) return;
	int i=g_pDocIndex->titlesList().indexOf(m_pIndexListWidget->selectedItems().at(0)->text());
	textBrowser()->setSource(QUrl(g_pDocIndex->documentList()[ i ]));
}

void KviHelpWindow::searchInIndex( const QString &s )
{
	QListWidgetItem *item ;
	QString sl = s.toLower();
	for(int i=0;i<m_pIndexListWidget->count();i++)
	{
		item=m_pIndexListWidget->item(i);
		QString t = item->text();
		if ( t.length() >= sl.length() &&
		item->text().left(s.length()).toLower() == sl ) {
			m_pIndexListWidget->setCurrentItem( item );
			m_pIndexListWidget->scrollToItem(item,QAbstractItemView::PositionAtTop);
			break;
		}
	}
}

void KviHelpWindow::indexSelected ( QListWidgetItem *item )
{
	if (!item) return;
	int i=g_pDocIndex->titlesList().indexOf(item->text());
	textBrowser()->setSource(QUrl(g_pDocIndex->documentList()[ i ]));
}

void KviHelpWindow::searchSelected ( QListWidgetItem *item )
{
	if (!item) return;
	int i=g_pDocIndex->titlesList().indexOf(item->text());
	textBrowser()->setSource(QUrl(g_pDocIndex->documentList()[ i ]));
}

QPixmap * KviHelpWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KVI_SMALLICON_MDIHELP);
}

void KviHelpWindow::resizeEvent(QResizeEvent *)
{
	m_pSplitter->setGeometry(0,0,width(),height());
}

void KviHelpWindow::fillCaptionBuffers()
{
	m_szPlainTextCaption = __tr2qs("Help Browser");
}


#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "helpwindow.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
