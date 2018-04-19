//=============================================================================
//
//   File : KviSharedFilesManager.cpp
//   Creation date : Wed Aug 27 2000 10:33:11 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
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

#include "KviSharedFilesManager.h"
#include "KviConfigurationFile.h"
#include "KviFileUtils.h"
#include "KviIrcMask.h"

#include <QFileInfo>
#include <QTimer>

// TODO: Match servers that the file requests come from
// TODO: Max number of downloads ?

// FIXME: MD5SUM ?

/*
	@doc: shared_files
	@title:
		Sharing files with KVIrc
	@type:
		generic
	@short:
		Automatically sharing your files with other IRC users
	@keyterms:
		file sharing
	@body:
		[big]What is this?[/big]
		The [i]file offers[/i] are a simple way to share your files with other IRC users.[br]
		Basically, you setup an offer by selecting a local file, choosing a [i]visible name[/i] for it.
		Remote users will be able to request you the file and download it automatically by
		issuing a simple DCC GET request.
		[big]Details[/big]
		Each offer refers to an existing file on one of your locally mounted file systems.
		The offer is given a visible name that the remote users will effectively request.
		To share the file /usr/arch/mp3/SonataArctica_SingInSilence_Live.mp3 you will add a file offer
		with /usr/arch/mp3/SonataArctica_SingInSilence_Live.mp3 as real file path, something like
		[i]SonataArctica_SingInSilence.mp3[/i]. A remote user will then request you a DCC GET SonataArctica_SingInSilence.mp3
		and KVIrc will automatically send the file.[br]
		Each file offer has an [i]user mask[/i] that the requesting remote users must match to
		obtain the file: *!*@* matches any user, Pragma!*@* matches any user with nickname pragma,
		*!*@*.omnikron.net matches any user coming from the omnikron.net domain.[br]
		Each offer can have an expire time: the offer will be automatically removed after
		a defined number of seconds. An expire time of '0' seconds means that the offer should never expire.[br]
		If you have two file offers with the same name and different file, the remote user can
		use an additional [i]size[/i] parameter in the DCC GET request.
		[big]Security issues[/big]
		This is a nice but unsecure method of sharing files.[br]
		The user mask is a good protection but you have to use it properly![br]
		Setting the user mask to Nick!*@* can be easily exploited (just by making a user disconnect
		in one of the well known ways and then by using his nickname).[br]
		On the other side, the remote end must know exactly the visible name of the offer to request
		and no one but you will tell him that name.[br]
		In sum:[br]
		Don't share any really important files: this *might* be like putting it on your webpage :D[br]
		Please don't send complaints if someone steals your /etc/passwd : it is because you have permitted that.[br]
*/

KviSharedFilesManager::KviSharedFilesManager()
    : QObject()
{
	m_pSharedListDict = new KviPointerHashTable<QString, KviSharedFileList>();
	m_pSharedListDict->setAutoDelete(true);
	m_pCleanupTimer = new QTimer();
	connect(m_pCleanupTimer, SIGNAL(timeout()), this, SLOT(cleanup()));
}

KviSharedFilesManager::~KviSharedFilesManager()
{
	if(m_pCleanupTimer->isActive())
		m_pCleanupTimer->stop();
	delete m_pCleanupTimer;
	delete m_pSharedListDict;
}

void KviSharedFilesManager::cleanup()
{
	KviPointerHashTableIterator<QString, KviSharedFileList> it(*m_pSharedListDict);
	time_t curTime = time(nullptr);

	bool bOtherStuffToCleanup = false;

	KviPointerList<QString> lDying;
	lDying.setAutoDelete(true);

	while(KviSharedFileList * l = it.current())
	{
		KviPointerList<KviSharedFile> tmp;
		tmp.setAutoDelete(false);
		for(KviSharedFile * o = l->first(); o; o = l->next())
		{
			if(o->expireTime() > 0)
			{
				if(((int)o->expireTime()) <= ((int)curTime))
				{
					tmp.append(o);
				}
				else
				{
					bOtherStuffToCleanup = true;
				}
			}
		}
		for(KviSharedFile * fo = tmp.first(); fo; fo = tmp.next())
		{
			l->removeRef(fo);
			emit sharedFileRemoved(fo);
		}
		if(l->count() == 0)
			lDying.append(new QString(it.currentKey()));

		++it;
	}

	for(QString * pDyingKey = lDying.first(); pDyingKey; pDyingKey = lDying.next())
		m_pSharedListDict->remove(*pDyingKey);

	if(!bOtherStuffToCleanup)
		m_pCleanupTimer->stop();
}

void KviSharedFilesManager::clear()
{
	m_pSharedListDict->clear();
	emit sharedFilesChanged();
}

void KviSharedFilesManager::doInsert(KviSharedFileList * l, KviSharedFile * o)
{
	int index = 0;
	for(KviSharedFile * fo = l->first(); fo; fo = l->next())
	{
		if(o->wildcardCount() > 0)
		{
			// the new mask has wildcards... if the current one has none, skip it
			if(fo->wildcardCount() > 0)
			{
				// the one in the list has wildcards too...
				// the ones with more non-wild chars go first...
				if(fo->nonWildcardCount() < o->nonWildcardCount())
				{
					// ok...the new one has more non-wildcards, insert
					l->insert(index, o);
					return;
				}
				else
				{
					if(o->nonWildcardCount() == fo->nonWildcardCount())
					{
						// the same number of non-wildcards
						// let the number of wildcards decide (it will be eventually equal)
						if(o->wildcardCount() < fo->wildcardCount())
						{
							// the new one has less wildcards... goes first
							l->insert(index, o);
							return;
						} // else the same number of wildcards and non-wildcards...skip
					}     // else the existing one has more non-wildcards...skip
				}
			} // else the current has no wildcards...skip
		}
		else
		{
			// the new mask has no wildcards....
			if(fo->wildcardCount() > 0)
			{
				// current one has wildcards...insert
				l->insert(index, o);
				return;
			}
			// the current one has no wildcards...
			// the longer masks go first....
			if(fo->maskLength() < o->maskLength())
			{
				// the current one is shorter than the new one...insert
				l->insert(index, o);
				return;
			} // else current one is longer...skip
		}
		index++;
	}
	l->append(o);
}

void KviSharedFilesManager::addSharedFile(KviSharedFile * f)
{
	// First find the list
	KviSharedFileList * l = m_pSharedListDict->find(f->name());
	if(!l)
	{
		l = new KviSharedFileList;
		l->setAutoDelete(true);
		m_pSharedListDict->replace(f->name(), l);
	}

	doInsert(l, f);

	if(((int)f->expireTime()) > 0)
	{
		if(!m_pCleanupTimer->isActive())
			m_pCleanupTimer->start(60000);
	}

	emit sharedFileAdded(f);
}

KviSharedFile * KviSharedFilesManager::addSharedFile(const QString & szName, const QString & szAbsPath, const QString & szMask, int timeoutInSecs)
{
	QFileInfo inf(szAbsPath);
	if(inf.exists() && inf.isFile() && inf.isReadable() && (inf.size() > 0))
	{
		// First find the list
		KviSharedFileList * l = m_pSharedListDict->find(szName);
		if(!l)
		{
			l = new KviSharedFileList;
			l->setAutoDelete(true);
			m_pSharedListDict->replace(szName, l);
		}

		// Now insert
		KviSharedFile * o = new KviSharedFile(szName, szAbsPath, szMask, timeoutInSecs > 0 ? (((int)(time(nullptr))) + timeoutInSecs) : 0, inf.size());

		doInsert(l, o);

		if(((int)o->expireTime()) > 0)
		{
			if(!m_pCleanupTimer->isActive())
				m_pCleanupTimer->start(60000);
		}

		emit sharedFileAdded(o);

		return o;
	}
	else
	{
		qDebug("File %s unreadable: can't add offer", szAbsPath.toUtf8().data());
		return nullptr;
	}
}

KviSharedFile * KviSharedFilesManager::lookupSharedFile(const QString & szName, KviIrcMask * mask, unsigned int uFileSize)
{
	KviSharedFileList * l = m_pSharedListDict->find(szName);
	if(!l)
		return nullptr;

	for(KviSharedFile * o = l->first(); o; o = l->next())
	{
		bool bMatch;
		if(mask)
		{
			KviIrcMask umask(o->userMask());
			bMatch = mask->matchedBy(umask);
		}
		else
			bMatch = KviQString::equalCS(o->userMask(), "*!*@*");
		if(bMatch)
		{
			if(uFileSize > 0)
			{
				if(uFileSize == o->fileSize())
					return o;
			}
			else
				return o;
		}
	}

	return nullptr;
}
bool KviSharedFilesManager::removeSharedFile(const QString & szName, const QString & szMask, unsigned int uFileSize)
{
	KviSharedFileList * l = m_pSharedListDict->find(szName);
	if(!l)
		return false;
	for(KviSharedFile * o = l->first(); o; o = l->next())
	{
		if(KviQString::equalCI(szMask, o->userMask()))
		{
			bool bMatch = uFileSize > 0 ? uFileSize == o->fileSize() : true;
			if(bMatch)
			{
				QString save = szName; // <-- szName MAY Be a pointer to o->name()
				l->removeRef(o);
				if(l->count() == 0)
					m_pSharedListDict->remove(save);
				emit sharedFileRemoved(o);
				return true;
			}
		}
	}
	return false;
}

bool KviSharedFilesManager::removeSharedFile(const QString & szName, KviSharedFile * off)
{
	KviSharedFileList * l = m_pSharedListDict->find(szName);
	if(!l)
		return false;
	for(KviSharedFile * o = l->first(); o; o = l->next())
	{
		if(off == o)
		{
			QString save = szName; // <-- szName MAY Be a pointer to o->name()
			l->removeRef(o);
			if(l->count() == 0)
				m_pSharedListDict->remove(save);
			emit sharedFileRemoved(off);
			return true;
		}
	}
	return false;
}

void KviSharedFilesManager::load(const QString & szFilename)
{
	KviConfigurationFile cfg(szFilename, KviConfigurationFile::Read);
	cfg.setGroup("PermanentFileOffers");
	int iNum = cfg.readIntEntry("NEntries", 0);
	for(int i = 0; i < iNum; i++)
	{
		QString szTmp;
		szTmp = QString("%1FName").arg(i);
		QString szName = cfg.readEntry(szTmp, "");
		szTmp = QString("%1FilePath").arg(i);
		QString szPath = cfg.readEntry(szTmp, "");
		szTmp = QString("%1UserMask").arg(i);
		QString szMask = cfg.readEntry(szTmp, "");
		if(!szMask.isEmpty() && !szPath.isEmpty() && !szName.isEmpty())
			addSharedFile(szName, szPath, szMask, 0);
	}
}

void KviSharedFilesManager::save(const QString & szFilename)
{
	KviConfigurationFile cfg(szFilename, KviConfigurationFile::Write);
	cfg.clear();
	cfg.setGroup("PermanentFileOffers");

	KviPointerHashTableIterator<QString, KviSharedFileList> it(*m_pSharedListDict);
	int i = 0;
	while(KviSharedFileList * pList = it.current())
	{
		for(KviSharedFile * pFile = pList->first(); pFile; pFile = pList->next())
		{
			if(((int)(pFile->expireTime())) == 0)
			{
				QString szTmp;
				szTmp = QString("%1FName").arg(i);
				cfg.writeEntry(szTmp, it.currentKey());
				szTmp = QString("%1FilePath").arg(i);
				cfg.writeEntry(szTmp, pFile->absFilePath());
				szTmp = QString("%1UserMask").arg(i);
				cfg.writeEntry(szTmp, pFile->userMask());
				++i;
			}
		}
		++it;
	}
	cfg.writeEntry("NEntries", i);
}
