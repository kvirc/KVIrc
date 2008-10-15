/**********************************************************************
** Copyright (C) 2000-2003 Trolltech AS.  All rights reserved.
**
** This file is part of the Qt Assistant.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "index.h"

#include "kvi_file.h"
#include "kvi_qstring.h"
#include "kvi_pointerhashtable.h"
#include "kvi_locale.h"

#include <ctype.h>

#include <QDir>
#include <QStringList>
#include <QApplication>
#include <QTextStream>
#include <QProgressDialog>

int kvi_compare(const Term * p1,const Term * p2)
{
	if(p1->frequency == p2->frequency) return 0;
	if(p1->frequency < p2->frequency) return -1;
	return 1;
}

QDataStream &operator>>( QDataStream &s, Document &l )
{
	s >> l.docNumber;
	s >> l.frequency;
	return s;
}

QDataStream &operator<<( QDataStream &s, const Document &l )
{
	s << (qint16)l.docNumber;
	s << (qint16)l.frequency;
	return s;
}

Index::Index( const QString &dp, const QString &hp )
: QObject( 0), dict( 8999 ), docPath( dp )
{
	alreadyHaveDocList = FALSE;
	lastWindowClosed = FALSE;
	connect(qApp,SIGNAL(lastWindowClosed()),this,SLOT(setLastWinClosed()));
}

Index::Index( const QStringList &dl, const QString &hp )
: QObject( 0), dict( 8999 )
{
	docList = dl;
	alreadyHaveDocList = TRUE;
	lastWindowClosed = FALSE;
	connect(qApp,SIGNAL(lastWindowClosed()),this,SLOT(setLastWinClosed()));
}

void Index::setLastWinClosed()
{
	lastWindowClosed = TRUE;
}

void Index::setDictionaryFile( const QString &f )
{
	dictFile = f;
}

void Index::setDocListFile( const QString &f )
{
	docListFile = f;
}

int Index::makeIndex()
{
	if ( !alreadyHaveDocList ) setupDocumentList();
	if ( docList.isEmpty() ) return 1;
	dict.clear();
	QStringList::Iterator it = docList.begin();

// 	QProgressDialog* pProgressDialog = new QProgressDialog( __tr2qs("Indexing help files"), __tr2qs("Cancel"), 0, docList.count() );
// 	pProgressDialog->setWindowTitle(__tr2qs("KVIrc"));
// 	pProgressDialog->setMinimumDuration(500);
// 	pProgressDialog->setWindowModality(Qt::WindowModal);
	for ( int i = 0; it != docList.end(); ++it, ++i ) {
// 		if (lastWindowClosed ||pProgressDialog->wasCanceled())
// 			break;
		parseDocument( *it, i );
// 		pProgressDialog->setValue(i);
	}
// 	delete pProgressDialog;
	return 0;
}

void Index::setupDocumentList()
{
	docList.clear();
	titleList.clear();
	QDir d( docPath );
	QString szCur;
	QStringList lst = d.entryList( QStringList("*.html") );
	QStringList::ConstIterator it = lst.begin();
	for ( ; it != lst.end(); ++it )
	{
		szCur=docPath + "/" + *it;
		docList.append( szCur );
		titleList.append(getDocumentTitle( szCur ));
	}
}

void Index::insertInDict( const QString &str, int docNum )
{
	if (KviQString::equalCI( str, "amp" ) || KviQString::equalCI( str, "nbsp" ) ) return;
	Entry *e = 0;
	if ( dict.count() ) e = dict[ str ];

	if ( e ) {
		if ( e->documents.first().docNumber != docNum )
		e->documents.prepend( Document( docNum, 1 ) );
		else
		e->documents.first().frequency++;
	} else {
		dict.insert( str, new Entry( docNum ) );
	}
}

void Index::parseDocument( const QString &filename, int docNum )
{
	KviFile file( filename );
	if ( !file.openForReading() ) {
		QString warn = "cannot open file " + filename;
		qWarning("%s", warn.toUtf8().data() );
		return;
	}
	QTextStream s( &file );
	QString text = s.readAll();
	if (text.isNull()) return;
	bool valid = TRUE;
	const QChar *buf = text.unicode();
	QChar str[64];
	QChar c = buf[0];
	int j = 0;
	int i = 0;
	while ( (uint)j < text.length() ) {
		if ( c == '<' || c == '&' ) {
			valid = FALSE;
			if ( i > 1 ) insertInDict( QString(str,i), docNum );
			i = 0;
			c = buf[++j];
			continue;
		}

		if ( ( c == '>' || c == ';' ) && !valid ) {
			valid = TRUE;
			c = buf[++j];
			continue;
		}

		if ( !valid ) {
			c = buf[++j];
			continue;
		}

		if ( ( c.isLetterOrNumber() || c == '_' ) && i < 63 ) {
			str[i] = c.toLower();
			++i;
		} else {
			if ( i > 1 ) insertInDict( QString(str,i), docNum );
			i = 0;
		}

		c = buf[++j];
	}

	if ( i > 1 ) insertInDict( QString(str,i), docNum );
	file.close();
}

void Index::writeDict()
{
	KviPointerHashTableIterator<QString,Entry> it( dict );
	KviFile f( dictFile );
	if ( !f.openForWriting() ) return;

	QDataStream s( &f );

	for( ; it.current(); ++it ) {
		Entry *e = it.current();
		s << it.currentKey();
		s << e->documents;
	}

	f.close();
	writeDocumentList();
}

void Index::writeDocumentList()
{
	KviFile f( docListFile );
	if ( !f.openForWriting() ) return;
	QTextStream s( &f );
	QString docs = docList.join("[#item#]");
	s << docs;

	KviFile f1( docListFile+".titles" );
	if ( !f1.openForWriting() ) return;
	QTextStream s1( &f1 );
	docs = titleList.join("[#item#]");
	s1 << docs;
}

void Index::readDict()
{
	KviFile f( dictFile );
	if ( !f.openForReading() ) return;
	dict.clear();
	QDataStream s( &f );
	QString key;
	QList<Document> docs;
	while ( !s.atEnd() ) {
		s >> key;
		s >> docs;
		dict.insert( key, new Entry( docs ) );
	}
	f.close();
	readDocumentList();
}

void Index::readDocumentList()
{
	//reading docs
	KviFile f( docListFile );
	if ( !f.openForReading() ) return;
	QTextStream s( &f );
	docList = s.readAll().split("[#item#]",QString::SkipEmptyParts,Qt::CaseInsensitive);

	//reading titles
	KviFile f1( docListFile+".titles" );
	if ( !f1.openForReading() ) return;
	QTextStream s1( &f1 );
	titleList = s1.readAll().split("[#item#]",QString::SkipEmptyParts,Qt::CaseInsensitive);
	//    qDebug(titleList);
}

QStringList Index::query( const QStringList &terms, const QStringList &termSeq, const QStringList &seqWords )
{
	TermList termList;
	QStringList::ConstIterator it = terms.begin();

	for ( it = terms.begin(); it != terms.end(); ++it ) {
		Entry *e = 0;
		if ( (*it).contains( '*' ) ) {
			QList<Document> wcts = setupDummyTerm( getWildcardTerms( *it ) );
			termList.append( new Term( "dummy", wcts.count(), wcts ) );
		} else if ( dict[ *it ] ) {
			e = dict[ *it ];
			termList.append( new Term( *it, e->documents.count(), e->documents ) );
		} else return QStringList();
	}

	termList.sort();

	Term *minTerm = termList.first();

	if ( !termList.count() ) return QStringList();

	termList.removeFirst();

	QList<Document> minDocs = minTerm->documents;
	QList<Document>::iterator C;
	QList<Document>::ConstIterator It;

	Term *t = termList.first();
	for ( ; t; t = termList.next() ) {

		QList<Document> docs = t->documents;

		C = minDocs.begin();

		while ( C != minDocs.end() ) {
			bool found = FALSE;

			for ( It = docs.begin(); It != docs.end(); ++It ) {
				if ( (*C).docNumber == (*It).docNumber ) {
					(*C).frequency += (*It).frequency;
					found = TRUE;
					break;
				}
			}

			if ( !found ) C = minDocs.erase( C );
			else ++C;
		}
	}

	QStringList results;
	if ( termSeq.isEmpty() ) {
		for ( C = minDocs.begin(); C != minDocs.end(); ++C )
			results << docList[ (int)(*C).docNumber ];

		return results;
	}

	QString fileName;
	for ( C = minDocs.begin(); C != minDocs.end(); ++C ) {
		fileName =  docList[ (int)(*C).docNumber ];

		if ( searchForPattern( termSeq, seqWords, fileName ) )
			results << fileName;
	}

	return results;
}

QString Index::getDocumentTitle( const QString &fileName )
{
	KviFile file( fileName );
	if ( !file.openForReading() ) {
		QString warn = "cannot open file " + fileName;
		qWarning("%s", warn.toUtf8().data() );
		return fileName;
	}

	QTextStream s( &file );
	QString text = s.readAll();

	int start = text.indexOf( "<title>", 0, Qt::CaseInsensitive ) + 7;
	int end = text.indexOf( "</title>", 0, Qt::CaseInsensitive);

	QString title = ( end - start <= 0 ? tr("Untitled") : text.mid( start, end - start ) );

	return title;
}

QStringList Index::getWildcardTerms( const QString &term )
{
	QStringList lst;
	QStringList terms = split( term );
	QStringList::Iterator iter;

	KviPointerHashTableIterator<QString,Entry> it( dict );
	for( ; it.current(); ++it ) {
		int index = 0;
		bool found = FALSE;
		QString text( it.currentKey() );

		for ( iter = terms.begin(); iter != terms.end(); ++iter ) {
			if ( *iter == "*" ) {
				found = TRUE;
				continue;
			}

			if ( iter == terms.begin() && (*iter)[0] != text[0] ) {
				found = FALSE;
				break;
			}

			index = text.indexOf( *iter, index,Qt::CaseInsensitive );

			if ( *iter == terms.last() && index != (int)text.length()-1 ) {
				index = text.lastIndexOf( *iter,-1,Qt::CaseInsensitive );

				if ( index != (int)text.length() - (int)(*iter).length() ) {
					found = FALSE;
					break;
				}

			}

			if ( index != -1 ) {
				found = TRUE;
				index += (*iter).length();
				continue;
			} else {
				found = FALSE;
				break;
			}
		}

		if ( found ) lst << text;
	}

	return lst;
}

QStringList Index::split( const QString &str )
{
	QStringList lst;
	int j = 0;
	int i = str.indexOf( '*', j );

	while ( i != -1 ) {
		if ( i > j && i <= (int)str.length() ) {
			lst << str.mid( j, i - j );
			lst << "*";
		}

		j = i + 1;
		i = str.indexOf( '*', j );
	}

	int l = str.length() - 1;

	if ( str.mid( j, l - j + 1 ).length() > 0 )
		lst << str.mid( j, l - j + 1 );

	return lst;
}

QList<Document> Index::setupDummyTerm( const QStringList &terms )
{
	TermList termList;
	QStringList::ConstIterator it = terms.begin();

	for ( ; it != terms.end(); ++it ) {
		Entry *e = 0;
		if ( dict[ *it ] ) {
			e = dict[ *it ];
			termList.append( new Term( *it, e->documents.count(), e->documents ) );
		}

	}

	termList.sort();

	QList<Document> maxList;
	if ( !termList.count() ) return maxList;

	maxList = termList.last()->documents;
	termList.removeLast();

	QList<Document>::iterator docIt;

	Term *t = termList.first();
	while ( t ) {
		QList<Document> docs = t->documents;
		for ( docIt = docs.begin(); docIt != docs.end(); ++docIt ) {
			if ( maxList.indexOf( *docIt ) == -1 )
				maxList.append( *docIt );
		}
		t = termList.next();
	}

	return maxList;
}

void Index::buildMiniDict( const QString &str )
{
	if ( miniDict[ str ] ) miniDict[ str ]->positions.append( wordNum );
	++wordNum;
}

bool Index::searchForPattern( const QStringList &patterns, const QStringList &words, const QString &fileName )
{
	KviFile file( fileName );

	if ( !file.openForReading() ) {
		QString warn = "cannot open file " + fileName;
		qWarning("%s", warn.toUtf8().data() );
		return FALSE;
	}
	else debug("Open file %s",fileName.toUtf8().data());
	debug("Patterns %s and words %s",patterns.join(",").toUtf8().data(),words.join(",").toUtf8().data());
	wordNum = 3;
	miniDict.clear();

	QStringList::ConstIterator cIt = words.begin();
	for ( ; cIt != words.end(); ++cIt )
		miniDict.insert( *cIt, new PosEntry( 0 ) );

	QTextStream s( &file );
	QString text = s.readAll();
	bool valid = TRUE;
	const QChar *buf = text.unicode();
	QChar str[64];
	QChar c = buf[0];
	int j = 0;
	int i = 0;
	while ( (uint)j < text.length() ) {
		if ( c == '<' || c == '&' ) {
			valid = FALSE;
			if ( i > 1 ) buildMiniDict( QString(str,i) );
			i = 0;
			c = buf[++j];
			continue;
		}

		if ( ( c == '>' || c == ';' ) && !valid ) {
			valid = TRUE;
			c = buf[++j];
			continue;
		}

		if ( !valid ) {
			c = buf[++j];
			continue;
		}

		if ( ( c.isLetterOrNumber() || c == '_' ) && i < 63 ) {
			str[i] = c.toLower();
			++i;
		} else {
			if ( i > 1 ) buildMiniDict( QString(str,i) );
			i = 0;
		}

		c = buf[++j];
	}

	if ( i > 1 ) buildMiniDict( QString(str,i) );

	file.close();

	QStringList::ConstIterator patIt = patterns.begin();
	QStringList wordLst;

	QList<uint> a, b;
	QList<uint>::iterator aIt;

	for ( ; patIt != patterns.end(); ++patIt ) {
		QString tmp=*patIt;
		wordLst = tmp.split( ' ');
		a = miniDict[ wordLst[0] ]->positions;

		for ( int j = 1; j < (int)wordLst.count(); ++j ) {
			b = miniDict[ wordLst[j] ]->positions;
			aIt = a.begin();

			while ( aIt != a.end() ) {
				if ( b.indexOf( *aIt + 1 ) != b.at(b.count()-1) ) {
					(*aIt)++;
					++aIt;
				} else {
					aIt = a.erase( aIt );
				}
			}
		}
	}

	if ( a.count() ) return TRUE;

	return FALSE;
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "index.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
