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

#include "HelpIndex.h"

#include <QFile>
#include <QDir>
#include <QStringList>
#include <QApplication>
#include <QByteArray>
#include <QTextStream>
#include <QUrl>
#include <QTextCodec>
#include <cctype>
#include <QTextDocument>
#include <QTimer>

#include <algorithm>
#include <utility>

QT_BEGIN_NAMESPACE

struct Term
{
	Term() = default;
	Term(QString t, int f, QVector<Document> l)
	    : term(std::move(t))
	    , frequency(f)
	    , documents(std::move(l))
	{
	}
	QString term;
	int frequency = -1;
	QVector<Document> documents;
	bool operator<(const Term & i2) const { return frequency < i2.frequency; }
};

QDataStream & operator>>(QDataStream & s, Document & l)
{
	s >> l.docNumber;
	s >> l.frequency;
	return s;
}

QDataStream & operator<<(QDataStream & s, const Document & l)
{
	s << (qint16)l.docNumber;
	s << (qint16)l.frequency;
	return s;
}

HelpIndex::HelpIndex(QString dp, const QString & /* hp */)
    : QObject(nullptr)
    , docPath(std::move(dp))
{
	alreadyHaveDocList = false;

	connect(qApp, SIGNAL(lastWindowClosed()), this, SLOT(setLastWinClosed()));

	m_pTimer = new QTimer(this);
	m_pTimer->setSingleShot(true);
	m_pTimer->setInterval(0);
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(filterNext()));
}

HelpIndex::HelpIndex(QStringList dl, const QString & /* hp */)
    : QObject(nullptr)
    , docList{ std::move(dl) }
{
	alreadyHaveDocList = true;

	connect(qApp, SIGNAL(lastWindowClosed()), this, SLOT(setLastWinClosed()));
}

void HelpIndex::setLastWinClosed()
{
	lastWindowClosed = true;
}

void HelpIndex::setDictionaryFile(const QString & f)
{
	dictFile = f;
}

void HelpIndex::setDocListFile(const QString & f)
{
	docListFile = f;
}

void HelpIndex::setDocList(const QStringList & lst)
{
	docList = lst;
}

void HelpIndex::makeIndex()
{
	if(!alreadyHaveDocList)
		setupDocumentList();

	lastWindowClosed = false;
	emit indexingStart(docList.count());
	dict.clear();
	m_iCurItem = 0;
	m_pTimer->start(); //singleshot
}

void HelpIndex::filterNext()
{
	if(m_iCurItem < docList.count() && !lastWindowClosed)
	{
		QUrl url(docList.at(m_iCurItem));
		parseDocument(url.toLocalFile(), m_iCurItem);
		emit indexingProgress(m_iCurItem);
		m_iCurItem++;
		m_pTimer->start(); //singleshot
	}
	else
	{
		emit indexingEnd();
	}
}

void HelpIndex::setupDocumentList()
{
	docList.clear();
	titleList.clear();
	QDir d(docPath);
	QStringList filters{QLatin1String("*.html")};
	QStringList lst = d.entryList(filters);
	for(auto&& item : lst)
	{
		QString filename = QLatin1String("file:///") + docPath + QLatin1String("/") + item;
		docList.append(filename);
		titleList.append(getDocumentTitle(filename));
	}
}

void HelpIndex::insertInDict(const QString & str, int docNum)
{
	if(str == QLatin1String("amp") || str == QLatin1String("nbsp"))
		return;
	Entry * e = nullptr;
	if(dict.count())
		e = dict[str];

	if(e)
	{
		if(e->documents.last().docNumber != docNum)
			e->documents.append(Document(docNum, 1));
		else
			e->documents.last().frequency++;
	}
	else
	{
		dict.insert(str, new Entry(docNum));
	}
}

QString HelpIndex::getCharsetForDocument(QFile * file)
{
	QTextStream s(file);
	QString contents = s.readAll();

	QString encoding;
	int start = contents.indexOf(QLatin1String("<meta"), 0, Qt::CaseInsensitive);
	if(start > 0)
	{
		int end = contents.indexOf(QLatin1String(">"), start);
		QString meta = contents.mid(start + 5, end - start);
		meta = meta.toLower();
		QRegExp r(QLatin1String("charset=([^\"\\s]+)"));
		if(r.indexIn(meta) != -1)
			encoding = r.cap(1);
	}

	file->seek(0);
	if(encoding.isEmpty())
		return QLatin1String("utf-8");
	return encoding;
}

void HelpIndex::parseDocument(const QString & filename, int docNum)
{
	QFile file(filename);
	if(!file.open(QFile::ReadOnly))
	{
		qWarning("Can't open file %s", qPrintable(filename));
		return;
	}

	QTextStream s(&file);
	QString en = getCharsetForDocument(&file);
	s.setCodec(QTextCodec::codecForName(en.toLatin1().constData()));

	QString text = s.readAll();
	if(text.isNull())
		return;

	bool valid = true;
	const QChar * buf = text.unicode();
	QChar str[64];
	QChar c = buf[0];
	int j = 0;
	int i = 0;
	while(j < text.length())
	{
		if(c == QLatin1Char('<') || c == QLatin1Char('&'))
		{
			valid = false;
			if(i > 1)
				insertInDict(QString(str, i), docNum);
			i = 0;
			c = buf[++j];
			continue;
		}
		if((c == QLatin1Char('>') || c == QLatin1Char(';')) && !valid)
		{
			valid = true;
			c = buf[++j];
			continue;
		}
		if(!valid)
		{
			c = buf[++j];
			continue;
		}
		if((c.isLetterOrNumber() || c == QLatin1Char('_')) && i < 63)
		{
			str[i] = c.toLower();
			++i;
		}
		else
		{
			if(i > 1)
				insertInDict(QString(str, i), docNum);
			i = 0;
		}
		c = buf[++j];
	}
	if(i > 1)
		insertInDict(QString(str, i), docNum);
	file.close();
}

void HelpIndex::writeDict()
{
	QFile f(dictFile);
	qDebug("Write dict to %s", f.fileName().toUtf8().data());
	if(!f.open(QFile::WriteOnly))
		return;
	QDataStream s(&f);
	for(auto it = dict.cbegin(); it != dict.cend(); ++it)
	{
		s << it.key();
		s << it.value()->documents.count();
		s << it.value()->documents;
	}
	f.close();
	writeDocumentList();
}

void HelpIndex::writeDocumentList()
{
	QFile f(docListFile);
	if(!f.open(QFile::WriteOnly))
		return;
	QDataStream s(&f);
	s << docList;

	QFile f1(docListFile + ".titles");
	if(!f1.open(QFile::WriteOnly))
		return;
	QDataStream s1(&f1);
	s1 << titleList;
}

void HelpIndex::readDict()
{
	QFile f(dictFile);
	if(!f.open(QFile::ReadOnly))
		return;

	dict.clear();
	QDataStream s(&f);
	QString key;
	int numOfDocs;
	QVector<Document> docs;
	while(!s.atEnd())
	{
		s >> key;
		s >> numOfDocs;
		docs.resize(numOfDocs);
		s >> docs;
		dict.insert(key, new Entry(docs));
	}
	f.close();
	readDocumentList();
}

void HelpIndex::readDocumentList()
{
	QFile f(docListFile);
	if(!f.open(QFile::ReadOnly))
		return;
	QDataStream s(&f);
	s >> docList;
	QFile f1(docListFile + ".titles");
	if(!f1.open(QFile::ReadOnly))
		return;
	QDataStream s1(&f1);
	s1 >> titleList;
}

QStringList HelpIndex::query(const QStringList & terms, const QStringList & termSeq, const QStringList & seqWords)
{
	QList<Term> termList;
	for(const auto & term : terms)
	{
		if(term.contains(QLatin1Char('*')))
		{
			QVector<Document> wcts = setupDummyTerm(getWildcardTerms(term));
			termList.append(Term(QLatin1String("dummy"), wcts.count(), wcts));
		}
		else if(dict[term])
		{
			auto e = dict[term];
			termList.append(Term(term, e->documents.count(), e->documents));
		}
		else
		{
			return QStringList();
		}
	}
	if(!termList.count())
		return QStringList();
	std::sort(termList.begin(), termList.end());

	QVector<Document> minDocs = termList.takeFirst().documents;
	for(const auto & term : termList)
	{
		QVector<Document> docs = term.documents;
		for(auto minDoc_it = minDocs.begin(); minDoc_it != minDocs.end();)
		{
			bool found = false;
			for(auto&& doc : docs)
			{
				if(minDoc_it->docNumber == doc.docNumber)
				{
					minDoc_it->frequency += doc.frequency;
					found = true;
					break;
				}
			}
			if(!found)
				minDoc_it = minDocs.erase(minDoc_it);
			else
				++minDoc_it;
		}
	}

	QStringList results;
	std::sort(minDocs.begin(), minDocs.end());
	if(termSeq.isEmpty())
	{
		for(auto & minDoc : minDocs)
			results << docList.at((int)minDoc.docNumber);
		return results;
	}

	for(const auto & minDoc : minDocs)
	{
		auto fileName = docList[(int)minDoc.docNumber];
		if(searchForPattern(termSeq, seqWords, fileName))
			results << fileName;
	}
	return results;
}

QString HelpIndex::getDocumentTitle(const QString & fullFileName)
{
	QUrl url(fullFileName);
	QString fileName = url.toLocalFile();

	if(documentTitleCache.contains(fileName))
		return documentTitleCache.value(fileName);

	QFile file(fileName);
	if(!file.open(QFile::ReadOnly))
	{
		qWarning("Can't open file %s", qPrintable(fileName));
		return fileName;
	}
	QTextStream s(&file);
	QString text = s.readAll();

	int start = text.indexOf(QLatin1String("<title>"), 0, Qt::CaseInsensitive) + 7;
	int end = text.indexOf(QLatin1String("</title>"), start, Qt::CaseInsensitive);

	QString title = tr("Untitled");
	if(end > start)
	{
		title = text.mid(start, end - start);
		if(Qt::mightBeRichText(title))
		{
			QTextDocument doc;
			doc.setHtml(title);
			title = doc.toPlainText();
		}
	}
	documentTitleCache.insert(fileName, title);
	return title;
}

QStringList HelpIndex::getWildcardTerms(const QString & term)
{
	QStringList lst;
	QStringList terms = split(term);

	for(auto it = dict.begin(); it != dict.end(); ++it)
	{
		int index = 0;
		bool found = false;
		QString text(it.key());
		for(auto iter = terms.cbegin(); iter != terms.cend(); ++iter)
		{
			if(*iter == QLatin1String("*"))
			{
				found = true;
				continue;
			}
			if(iter == terms.cbegin() && (*iter)[0] != text[0])
			{
				found = false;
				break;
			}
			index = text.indexOf(*iter, index);
			if(*iter == terms.last() && index != text.length() - 1)
			{
				index = text.lastIndexOf(*iter);
				if(index != text.length() - iter->length())
				{
					found = false;
					break;
				}
			}
			if(index != -1)
			{
				found = true;
				index += iter->length();
				continue;
			}
			else
			{
				found = false;
				break;
			}
		}
		if(found)
			lst << text;
	}

	return lst;
}

QStringList HelpIndex::split(const QString & str)
{
	QStringList lst;
	int j = 0;
	int i = str.indexOf(QLatin1Char('*'), j);

	if(str.startsWith(QLatin1String("*")))
		lst << QLatin1String("*");

	while(i != -1)
	{
		if(i > j && i <= (int)str.length())
		{
			lst << str.mid(j, i - j);
			lst << QLatin1String("*");
		}
		j = i + 1;
		i = str.indexOf(QLatin1Char('*'), j);
	}

	int l = str.length() - 1;
	if(str.mid(j, l - j + 1).length() > 0)
		lst << str.mid(j, l - j + 1);

	return lst;
}

QVector<Document> HelpIndex::setupDummyTerm(const QStringList & terms)
{
	QList<Term> termList;
	for(const auto & term : terms)
	{
		if(dict[term])
		{
			auto e = dict[term];
			termList.append(Term(term, e->documents.count(), e->documents));
		}
	}
	if(!termList.count())
		return QVector<Document>();
	std::sort(termList.begin(), termList.end());

	auto maxList = termList.takeLast().documents;
	for(const auto & term : termList)
		for(const auto & doc : term.documents)
			if(maxList.indexOf(doc) == -1)
				maxList.append(doc);

	return maxList;
}

void HelpIndex::buildMiniDict(const QString & str)
{
	if(miniDict[str])
		miniDict[str]->positions.append(wordNum);
	++wordNum;
}

bool HelpIndex::searchForPattern(const QStringList & patterns, const QStringList & words, const QString & fileName)
{
	QUrl url(fileName);
	QString fName = url.toLocalFile();
	QFile file(fName);
	if(!file.open(QFile::ReadOnly))
	{
		qWarning("Can't open file %s", qPrintable(fName));
		return false;
	}

	wordNum = 3;
	miniDict.clear();
	for(auto&& word : words)
		miniDict.insert(word, new PosEntry(0));

	QTextStream s(&file);
	QString text = s.readAll();
	bool valid = true;
	const QChar * buf = text.unicode();
	QChar str[64];
	QChar c = buf[0];
	int j = 0;
	int i = 0;
	while(j < text.length())
	{
		if(c == QLatin1Char('<') || c == QLatin1Char('&'))
		{
			valid = false;
			if(i > 1)
				buildMiniDict(QString(str, i));
			i = 0;
			c = buf[++j];
			continue;
		}
		if((c == QLatin1Char('>') || c == QLatin1Char(';')) && !valid)
		{
			valid = true;
			c = buf[++j];
			continue;
		}
		if(!valid)
		{
			c = buf[++j];
			continue;
		}
		if((c.isLetterOrNumber() || c == QLatin1Char('_')) && i < 63)
		{
			str[i] = c.toLower();
			++i;
		}
		else
		{
			if(i > 1)
				buildMiniDict(QString(str, i));
			i = 0;
		}
		c = buf[++j];
	}
	if(i > 1)
		buildMiniDict(QString(str, i));
	file.close();

	QStringList wordLst;
	QList<uint> a;
	for(auto&& pattern : patterns)
	{
		wordLst = pattern.split(QLatin1Char(' '));
		a = miniDict[wordLst[0]]->positions;
		for(int j = 1; j < (int)wordLst.count(); ++j)
		{
			auto b = miniDict[wordLst[j]]->positions;
			auto aIt = a.begin();
			while(aIt != a.end())
			{
				if(b.contains(*aIt + 1))
				{
					(*aIt)++;
					++aIt;
				}
				else
				{
					aIt = a.erase(aIt);
				}
			}
		}
	}
	if(a.count())
		return true;
	return false;
}

QT_END_NAMESPACE
