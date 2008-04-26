//
//   File : helpwindow.cpp
//   Creation date : Tue Aug 11 2000 18:08:22 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//


#include "helpwindow.h"
#include "helpwidget.h"

#include "kvi_app.h"
#include "kvi_iconmanager.h"
#include "kvi_options.h"
#include "kvi_locale.h"
#include "kvi_module.h"
#include "kvi_config.h"
#include "kvi_valuelist.h"
#include "kvi_sourcesdate.h"
#include "kvi_styled_controls.h"

#include <QFileInfo>
#include <QSplitter>
#include <QLineEdit>
#include <QMessageBox>
#include <QRegExp>
#include <QToolTip>

extern Index        * g_pDocIndex;
extern KviPointerList<KviHelpWindow> * g_pHelpWindowList;
extern KviPointerList<KviHelpWidget> * g_pHelpWidgetList;

bool g_bIndexingDone = FALSE;
KviHelpWindow::KviHelpWindow(KviFrame * lpFrm,const char * name)
: KviWindow(KVI_WINDOW_TYPE_HELP,lpFrm,name)
{
	if(!g_bIndexingDone)
	{
		QString szDoclist,szDict;
		
		g_pApp->getLocalKvircDirectory(szDoclist,KviApp::Help,"help.doclist." KVI_SOURCES_DATE);
		g_pApp->getLocalKvircDirectory(szDict,KviApp::Help,"help.dict." KVI_SOURCES_DATE);
		
		if ( QFileInfo( szDoclist ).exists() && QFileInfo( szDict ).exists() ) {
			g_pDocIndex->readDict();
		} else {
			QProgressDialog* pProgressDialog = new QProgressDialog( __tr2qs("Indexing help files"), __tr2qs("Cancel"), 0,100 );
			connect(g_pDocIndex,SIGNAL(indexingProgress(int)), pProgressDialog, SLOT(setValue(int)) );
			g_pDocIndex->makeIndex();
			g_pDocIndex->writeDict();
			g_pDocIndex->writeDocumentList();
			delete pProgressDialog;
		}
		g_bIndexingDone=TRUE;
	}
	
	g_pHelpWindowList->append(this);
	m_pSplitter = new QSplitter(Qt::Horizontal,this,"main_splitter");
	m_pHelpWidget = new KviHelpWidget(m_pSplitter,lpFrm);

	m_pToolBar=new KviTalVBox(m_pSplitter);

	m_pTabWidget = new QTabWidget(m_pToolBar);
	
	m_pIndexTab  = new KviTalVBox(m_pTabWidget);
	m_pTabWidget->insertTab(m_pIndexTab,__tr2qs("Index"));
	
	KviTalHBox* pSearchBox = new KviTalHBox(m_pIndexTab);
	m_pIndexSearch = new QLineEdit(pSearchBox);
	connect( m_pIndexSearch, SIGNAL( textChanged(const QString&) ),
	     this, SLOT( searchInIndex(const QString&) ) );
	connect( m_pIndexSearch, SIGNAL( returnPressed() ),
	     this, SLOT( showIndexTopic() ) );
	
	KviStyledToolButton* pBtnRefreshIndex = new KviStyledToolButton(pSearchBox);
	pBtnRefreshIndex->setIconSet(*g_pIconManager->getBigIcon(KVI_REFRESH_IMAGE_NAME));
	connect(pBtnRefreshIndex,SIGNAL(clicked()),this,SLOT(refreshIndex()));
	QToolTip::add( pBtnRefreshIndex, __tr2qs("Refresh index") );
	
	m_pIndexListBox = new KviTalListBox(m_pIndexTab);
	QStringList docList=g_pDocIndex->titlesList();
	m_pIndexListBox->insertStringList(docList);
	connect(m_pIndexListBox,SIGNAL(selected(int)),this,SLOT(indexSelected ( int )));
	m_pIndexListBox->sort();
	
	m_pSearchTab  = new KviTalVBox(m_pTabWidget);
	m_pTabWidget->insertTab(m_pSearchTab,__tr2qs("Search"));
	
	m_pTermsEdit = new QLineEdit(m_pSearchTab);
/*	connect( m_pTermsEdit, SIGNAL( textChanged(const QString&) ),
	     this, SLOT( searchInIndex(const QString&) ) );*/
	connect( m_pTermsEdit, SIGNAL( returnPressed() ),
	     this, SLOT( startSearch() ) );
	     
	m_pResultBox = new KviTalListBox(m_pSearchTab);
	connect(m_pResultBox,SIGNAL(selected(int)),this,SLOT(searchSelected ( int )));
	
	KviValueList<int> li;
	li.append(width()-80);
	li.append(80);
	m_pSplitter->setSizes(li);

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
	KviValueList<int> def;
	int w = width();
	def.append((w * 82) / 100);
	def.append((w * 18) / 100);
	m_pSplitter->setSizes(cfg->readIntListEntry("Splitter",def));
	KviWindow::loadProperties(cfg);
}

void KviHelpWindow::refreshIndex()
{
	m_pIndexListBox->clear();
	QProgressDialog* pProgressDialog = new QProgressDialog( __tr2qs("Indexing help files"), __tr2qs("Cancel"), 0,100 );
	connect(g_pDocIndex,SIGNAL(indexingProgress(int)), pProgressDialog, SLOT(setValue(int)) );
	g_pDocIndex->makeIndex();
	g_pDocIndex->writeDict();
	g_pDocIndex->writeDocumentList();
	delete pProgressDialog;
	g_bIndexingDone=TRUE;
	QStringList docList=g_pDocIndex->titlesList();
	m_pIndexListBox->insertStringList(docList);
	m_pIndexListBox->sort();
}

void KviHelpWindow::startSearch()
{

	QString str = m_pTermsEdit->text();
	str = str.replace( "\'", "\"" );
	str = str.replace( "`", "\"" );
	QString buf = str;
	str = str.replace( "-", " " );
	str = str.replace( QRegExp( "\\s[\\S]?\\s" ), " " );
	m_terms = QStringList::split( " ", str );
	QStringList termSeq;
	QStringList seqWords;
	QStringList::iterator it = m_terms.begin();
	for ( ; it != m_terms.end(); ++it ) {
		(*it) = (*it).simplifyWhiteSpace();
		(*it) = (*it).lower();
		(*it) = (*it).replace( "\"", "" );
	}
	if ( str.contains( '\"' ) ) {
		if ( (str.count( '\"' ))%2 == 0 ) {
			int beg = 0;
			int end = 0;
			QString s;
			beg = str.find( '\"', beg );
			while ( beg != -1 ) {
				beg++;
				end = str.find( '\"', beg );
				s = str.mid( beg, end - beg );
				s = s.lower();
				s = s.simplifyWhiteSpace();
				if ( s.contains( '*' ) ) {
				QMessageBox::warning( this, tr( "Full Text Search" ),
					tr( "Using a wildcard within phrases is not allowed." ) );
				return;
				}
				seqWords += QStringList::split( ' ', s );
				termSeq << s;
				beg = str.find( '\"', end + 1);
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
		m_pResultBox->insertItem( g_pDocIndex->getDocumentTitle( *it ) );

	m_terms.clear();
	bool isPhrase = FALSE;
	QString s = "";
	for ( int i = 0; i < (int)buf.length(); ++i ) {
		if ( buf[i] == '\"' ) {
			isPhrase = !isPhrase;
			s = s.simplifyWhiteSpace();
			if ( !s.isEmpty() )
				m_terms << s;
			s = "";
		} else if ( buf[i] == ' ' && !isPhrase ) {
			s = s.simplifyWhiteSpace();
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
	if (m_pIndexSearch->text().isEmpty()|| !m_pIndexListBox->selectedItem()) return;
	int i=g_pDocIndex->titlesList().findIndex(m_pIndexListBox->selectedItem()->text());
	textBrowser()->setSource(QUrl(g_pDocIndex->documentList()[ i ]));
}

void KviHelpWindow::searchInIndex( const QString &s )
{
	KviTalListBoxItem *i = m_pIndexListBox->firstItem();
	QString sl = s.lower();
	while ( i ) {
		QString t = i->text();
		if ( t.length() >= sl.length() &&
		i->text().left(s.length()).lower() == sl ) {
			m_pIndexListBox->setCurrentItem( i );
			m_pIndexListBox->setTopItem(m_pIndexListBox->index(i));
			break;
		}
		i = i->next();
	}
}

void KviHelpWindow::indexSelected ( int index )
{
	int i=g_pDocIndex->titlesList().findIndex(m_pIndexListBox->text(index));
	textBrowser()->setSource(QUrl(g_pDocIndex->documentList()[ i ]));
}

void KviHelpWindow::searchSelected ( int index )
{
	int i=g_pDocIndex->titlesList().findIndex(m_pResultBox->text(index));
	textBrowser()->setSource(QUrl(g_pDocIndex->documentList()[ i ]));
}

QPixmap * KviHelpWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KVI_SMALLICON_MDIHELP);
}

void KviHelpWindow::resizeEvent(QResizeEvent *e)
{
	m_pSplitter->setGeometry(0,0,width(),height());
}

QSize KviHelpWindow::sizeHint() const
{
	return m_pHelpWidget->sizeHint();
}

void KviHelpWindow::fillCaptionBuffers()
{
	m_szPlainTextCaption = __tr2qs("Help Browser");

	m_szHtmlActiveCaption = "<nobr><font color=\"";
	m_szHtmlActiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextActive).name();
	m_szHtmlActiveCaption += "\"><b>";
	m_szHtmlActiveCaption += m_szPlainTextCaption;
	m_szHtmlActiveCaption += "</b></font></nobr>";

	m_szHtmlInactiveCaption = "<nobr><font color=\"";
	m_szHtmlInactiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextInactive).name();
	m_szHtmlInactiveCaption += "\"><b>";
	m_szHtmlInactiveCaption += m_szPlainTextCaption;
	m_szHtmlInactiveCaption += "</b></font></nobr>";
}


#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "helpwindow.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
