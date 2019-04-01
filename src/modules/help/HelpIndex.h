/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
**
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file.  Please review the following information
** to ensure GNU General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.  In addition, as a special
** exception, Nokia gives you certain additional rights. These rights
** are described in the Nokia Qt GPL Exception version 1.3, included in
** the file GPL_EXCEPTION.txt in this package.
**
** Qt for Windows(R) Licensees
** As a special exception, Nokia, as the sole copyright holder for Qt
** Designer, grants users of the Qt/Eclipse Integration plug-in the
** right for the Qt/Eclipse Integration to link to functionality
** provided by Qt Designer and its related libraries.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
****************************************************************************/

#ifndef INDEX_H
#define INDEX_H

#include <QStringList>
#include <QHash>
#include <QDataStream>
#include <QObject>
#include <QList>
#include <QFile>
#include <QVector>

class QTimer;

QT_BEGIN_NAMESPACE

struct Document
{
	Document() = default;
	Document(int d, int f) : docNumber(d), frequency(f) {}
	bool operator==(const Document & doc) const
	{
		return docNumber == doc.docNumber;
	}
	bool operator<(const Document & doc) const
	{
		return frequency > doc.frequency;
	}
	bool operator<=(const Document & doc) const
	{
		return frequency >= doc.frequency;
	}
	bool operator>(const Document & doc) const
	{
		return frequency < doc.frequency;
	}
	qint16 docNumber = -1;
	qint16 frequency = 0;
};

QDataStream & operator>>(QDataStream & s, Document & l);
QDataStream & operator<<(QDataStream & s, const Document & l);

class HelpIndex : public QObject
{
	Q_OBJECT
public:
	struct Entry
	{
		Entry(int d) { documents.append(Document(d, 1)); }
		Entry(QVector<Document> l) : documents(l) {}
		QVector<Document> documents;
	};
	struct PosEntry
	{
		PosEntry(int p) { positions.append(p); }
		QList<uint> positions;
	};

	HelpIndex(QString dp, const QString & hp);
	HelpIndex(QStringList dl, const QString & hp);
	void writeDict();
	void readDict();
	void makeIndex();
	QStringList query(const QStringList &, const QStringList &, const QStringList &);
	QString getDocumentTitle(const QString &);
	void setDictionaryFile(const QString &);
	void setDocListFile(const QString &);
	void setDocList(const QStringList &);

	const QStringList & documentList() const { return docList; };
	const QStringList & titlesList() const { return titleList; };

signals:
	void indexingStart(int);
	void indexingProgress(int);
	void indexingEnd();

private slots:
	void setLastWinClosed();
	void filterNext();

private:
	void setupDocumentList();
	void parseDocument(const QString &, int);
	void insertInDict(const QString &, int);
	void writeDocumentList();
	void readDocumentList();
	QStringList getWildcardTerms(const QString &);
	QStringList split(const QString &);
	QVector<Document> setupDummyTerm(const QStringList &);
	bool searchForPattern(const QStringList &, const QStringList &, const QString &);
	void buildMiniDict(const QString &);

	QString getCharsetForDocument(QFile *);
	QStringList docList;
	QStringList titleList;
	QHash<QString, Entry *> dict;
	QHash<QString, PosEntry *> miniDict;
	uint wordNum = 0;
	QString docPath;
	QString dictFile;
	QString docListFile;
	bool alreadyHaveDocList;
	bool lastWindowClosed = false;
	QHash<QString, QString> documentTitleCache;
	QTimer * m_pTimer = nullptr;
	int m_iCurItem = 0;
};

#endif

QT_END_NAMESPACE
