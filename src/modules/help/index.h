#ifndef INDEX_H
#define INDEX_H
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

#include "kvi_pointerhashtable.h"
#include "kvi_pointerlist.h"

#include <QList>
#include <QDataStream>
#include <QObject>
#include <QStringList>

struct Document {
	Document( int d, int f ) : docNumber( d ), frequency( f ) {}
	Document() : docNumber( -1 ), frequency( 0 ) {}
	bool operator==( const Document &doc ) const {
		return docNumber == doc.docNumber;
	}
	bool operator<( const Document &doc ) const {
		return frequency > doc.frequency;
	}
	bool operator<=( const Document &doc ) const {
		return frequency >= doc.frequency;
	}
	bool operator>( const Document &doc ) const {
		return frequency < doc.frequency;
	}
	qint16 docNumber;
	qint16 frequency;
};

QDataStream &operator>>( QDataStream &s, Document &l );
QDataStream &operator<<( QDataStream &s, const Document &l );

class Index : public QObject
{
	Q_OBJECT
public:
	struct Entry {
		Entry( int d ) { documents.append( Document( d, 1 ) ); }
		Entry( QList<Document> l ) : documents( l ) {}
		QList<Document> documents;
	};
	struct PosEntry {
		PosEntry( int p ) { positions.append( p ); }
		QList<uint> positions;
	};
	Index( const QString &dp, const QString &hp );
	Index( const QStringList &dl, const QString &hp );
	void writeDict();
	void readDict();
	int makeIndex();
	QStringList query( const QStringList&, const QStringList&, const QStringList& );
	QString getDocumentTitle( const QString& );
	void setDictionaryFile( const QString& );
	void setDocListFile( const QString& );
	void writeDocumentList();
	void readDocumentList();
	void setupDocumentList();
	const QStringList& documentList() { return docList; };
	const QStringList& titlesList() { return titleList; };
signals:
	void indexingProgress( int );
private slots:
	void setLastWinClosed();
private:
	void parseDocument( const QString&, int );
	void insertInDict( const QString&, int );
	QStringList getWildcardTerms( const QString& );
	QStringList split( const QString& );
	QList<Document> setupDummyTerm( const QStringList& );
	bool searchForPattern( const QStringList&, const QStringList&, const QString& );
	void buildMiniDict( const QString& );
	QStringList docList;
	QStringList titleList;
	KviPointerHashTable<QString,Entry> dict;
	KviPointerHashTable<QString,PosEntry> miniDict;
	uint wordNum;
	QString docPath;
	QString dictFile, docListFile;
	bool alreadyHaveDocList;
	bool lastWindowClosed;
};

struct Term {
	Term( const QString &t, int f, QList<Document> l )
	: term( t ), frequency( f ), documents( l ) {}
	QString term;
	int frequency;
	QList<Document>documents;
};

#define TermList KviPointerList<Term>

#endif
