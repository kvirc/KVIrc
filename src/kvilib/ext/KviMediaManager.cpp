//=============================================================================
//
//   File : KviMediaManager.cpp
//   Creation date : Wed Dec 29 2010 00:37:56 CEST by Elvio basello
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2010 Elvio Basello (hellvis69 at gmail dot com)
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

//#define _KVI_DEBUG_CHECK_RANGE_

#include "KviMediaManager.h"
#include "KviConfigurationFile.h"
#include "KviFile.h"
#include "KviFileUtils.h"
#include "KviLocale.h"
#include "kvi_debug.h"

#include <QDir>
#include <QRegExp>

#include <sys/types.h>
#include <sys/stat.h>

#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
#include <unistd.h>
#include "KviMemory.h"
#endif

#ifndef S_ISDIR
#define S_ISDIR(__f) (__f & _S_IFDIR)
#endif

#ifndef S_ISFIFO
#define S_ISFIFO(__f) (__f & _S_IFIFO)
#endif

#ifndef S_ISREG
#define S_ISREG(__f) (__f & _S_IFREG)
#endif

#ifndef S_ISCHR
#define S_ISCHR(__f) (__f & _S_IFCHR)
#endif

#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
#include <dirent.h>
#else
#include "KviMemory.h"
#endif

KviMediaManager::KviMediaManager()
    : KviMutex()
{
	m_pMediaTypeList = new KviPointerList<KviMediaType>;
	m_pMediaTypeList->setAutoDelete(true);
}

KviMediaManager::~KviMediaManager()
{
	delete m_pMediaTypeList;
}

KviMediaType * KviMediaManager::findMediaTypeByIanaType(const char * ianaType)
{
	KVI_ASSERT(locked());
	for(KviMediaType * mt = m_pMediaTypeList->first(); mt; mt = m_pMediaTypeList->next())
	{
		if(kvi_strEqualCI(mt->szIanaType.ptr(), ianaType))
			return mt;
	}

	return nullptr;
}

KviMediaType * KviMediaManager::findMediaTypeByFileMask(const char * filemask)
{
	KVI_ASSERT(locked());
	for(KviMediaType * mt = m_pMediaTypeList->first(); mt; mt = m_pMediaTypeList->next())
	{
		// FIXME: #warning "Should this be case sensitive ?"
		if(kvi_strEqualCI(mt->szFileMask.ptr(), filemask))
			return mt;
	}

	return nullptr;
}

void KviMediaManager::copyMediaType(KviMediaType * dst, KviMediaType * src)
{
	dst->szFileMask = src->szFileMask;
	dst->szMagicBytes = src->szMagicBytes;
	dst->szIanaType = src->szIanaType;
	dst->szDescription = src->szDescription;
	dst->szSavePath = src->szSavePath;
	dst->szCommandline = src->szCommandline;
	dst->szRemoteExecCommandline = src->szRemoteExecCommandline;
	dst->szIcon = src->szIcon;
}

void KviMediaManager::insertMediaType(KviMediaType * m)
{
	KVI_ASSERT(locked());
	int iWildCount = m->szFileMask.occurrences('*');
	int iNonWildCount = m->szFileMask.len() - iWildCount;

	// The masks with no wildcards go first in the list
	// then we insert the ones with more non-wild chars

	int index = 0;
	for(KviMediaType * mt = m_pMediaTypeList->first(); mt; mt = m_pMediaTypeList->next())
	{
		if(iWildCount)
		{
			// the new mask has wildcards... if the current one has none, skip it
			int iWildCountExisting = mt->szFileMask.occurrences('*');
			if(iWildCountExisting)
			{
				// the one in the list has wildcards too...
				// the ones with more non-wild chars go first...
				int iNonWildCountExisting = mt->szFileMask.len() - iWildCountExisting;
				if(iNonWildCountExisting < iNonWildCount)
				{
					// ok...the new one has more non-wildcards, insert
					m_pMediaTypeList->insert(index, m);
					return;
				}
				else
				{
					if(iNonWildCount == iNonWildCountExisting)
					{
						// the same number of non-wildcards
						// let the number of wildcards decide (it will be eventually equal)
						if(iWildCount < iWildCountExisting)
						{
							// the new one has less wildcards... goes first
							m_pMediaTypeList->insert(index, m);
							return;
						} // else the same number of wildcards and non-wildcards...skip
					}     // else the existing one has more non-wildcards...skip
				}
			} // else the current has no wildcards...skip
		}
		else
		{
			// the new mask has no wildcards....
			if(mt->szFileMask.contains('*'))
			{
				// current one has wildcards...insert
				m_pMediaTypeList->insert(index, m);
				return;
			}
			// the current one has no wildcards...
			// the longer masks go first....
			if(mt->szFileMask.len() < m->szFileMask.len())
			{
				// the current one is shorter than the new one...insert
				m_pMediaTypeList->insert(index, m);
				return;
			} // else current one is longer...skip
		}
		index++;
	}
	m_pMediaTypeList->append(m);
}

KviMediaType * KviMediaManager::findMediaType(const char * filename, bool bCheckMagic)
{
	// FIXME: This should be ported at least to QString....
	KVI_ASSERT(locked());

	KviCString szFullPath = filename;
	if(!KviFileUtils::isAbsolutePath(szFullPath.ptr()))
	{
		KviCString tmp = QDir::currentPath();
		tmp.ensureLastCharIs('/');
		szFullPath.prepend(tmp);
	}

	KviCString szFile = filename;
	szFile.cutToLast('/', true);

// first of all, lstat() the file
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	struct _stat st;
	if(_stat(szFullPath.ptr(), &st) != 0)
#else
	struct stat st;
	if(lstat(szFullPath.ptr(), &st) != 0)
#endif
	{
		//qDebug("Problems while stating file %s",szFullPath.ptr());
		// We do just the pattern matching
		// it's better to avoid magic checks
		// if the file is a device, we would be blocked while attempting to read data
		return findMediaTypeForRegularFile(szFullPath.ptr(), szFile.ptr(), false);
	}
	else
	{
// If it is a link, stat() the link target
#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
		if(S_ISLNK(st.st_mode))
		{
			if(stat(szFullPath.ptr(), &st) != 0)
			{
				qDebug("Problems while stating() target for link %s", szFullPath.ptr());
				// Same as above
				return findMediaTypeForRegularFile(szFullPath.ptr(), szFile.ptr(), false);
			}
		}
#endif
	}

#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
	if(S_ISSOCK(st.st_mode))
	{
		// Socket : return default media type
		KviMediaType * mtd = findMediaTypeByIanaType("inode/socket");
		if(!mtd)
		{
			// Add it
			mtd = new KviMediaType;
			mtd->szIanaType = "inode/socket";
			mtd->szDescription = __tr("Socket");
			mtd->szIcon = "kvi_dbsocket.png"; // hardcoded ?
			insertMediaType(mtd);
		}
		return mtd;
	}
#endif

	if(S_ISFIFO(st.st_mode))
	{
		// Fifo: return default media type
		KviMediaType * mtd = findMediaTypeByIanaType("inode/fifo");
		if(!mtd)
		{
			// Add it
			mtd = new KviMediaType;
			mtd->szIanaType = "inode/fifo";
			mtd->szDescription = __tr("Fifo");
			mtd->szIcon = "kvi_dbfifo.png"; // hardcoded ?
			insertMediaType(mtd);
		}
		return mtd;
	}

#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
	if(S_ISBLK(st.st_mode))
	{
		// Block device: return default media type
		KviMediaType * mtd = findMediaTypeByIanaType("inode/blockdevice");
		if(!mtd)
		{
			// Add it
			mtd = new KviMediaType;
			mtd->szIanaType = "inode/blockdevice";
			mtd->szDescription = __tr("Block device");
			mtd->szIcon = "kvi_dbblockdevice.png"; // hardcoded ?
			insertMediaType(mtd);
		}
		return mtd;
	}
#endif

	if(S_ISCHR(st.st_mode))
	{
		// Char device: return default media type
		KviMediaType * mtd = findMediaTypeByIanaType("inode/chardevice");
		if(!mtd)
		{
			// Add it
			mtd = new KviMediaType;
			mtd->szIanaType = "inode/chardevice";
			mtd->szDescription = __tr("Char device");
			mtd->szIcon = "kvi_dbchardevice.png"; // hardcoded ?
			insertMediaType(mtd);
		}
		return mtd;
	}

	// this is a regular file (or at least it looks like one)
	return findMediaTypeForRegularFile(szFullPath.ptr(), szFile.ptr(), bCheckMagic);
}

KviMediaType * KviMediaManager::findMediaTypeForRegularFile(const char * szFullPath, const char * szFileName, bool bCheckMagic)
{
	char buffer[17];
	int len = 0;

	if(bCheckMagic)
	{
		QString szTmp = QString::fromUtf8(szFullPath);
		KviFile f(szTmp);
		if(f.open(QFile::ReadOnly))
		{
			len = f.read(buffer, 16);
			if(len > 0)
			{
				buffer[len] = '\0';
				if(buffer[0] == 0)
					len = 0; // no way to match it
			}
			f.close();
		}
	}

	for(KviMediaType * m = m_pMediaTypeList->first(); m; m = m_pMediaTypeList->next())
	{
		// FIXME: #warning "Should this be case sensitive ?"
		if(kvi_matchWildExpr(m->szFileMask.ptr(), szFileName))
		{
			if(len && m->szMagicBytes.hasData())
			{
				QRegExp re(m->szMagicBytes.ptr());
				// It looks like they can't decide the name for this function :D
				// ... well, maybe the latest choice is the best one.
				if(re.indexIn(buffer) > -1)
					return m; // matched!

				// else magic failed...not a match
			}
			else
				return m; // matched! (no magic check)
		}
	}

	KviMediaType * mtd = findMediaTypeByIanaType("application/octet-stream");
	if(!mtd)
	{
		// Add it
		mtd = new KviMediaType;
		mtd->szIanaType = "application/octet-stream";
		mtd->szDescription = __tr("Octet stream (unknown)");
		mtd->szCommandline = "editor.open $0";
		mtd->szIcon = "kvi_dbunknown.png"; // hardcoded ?
		insertMediaType(mtd);
	}

	return mtd;
}

// FIXME : add proper default handlers for Windows
// e.g. a KVS function which calls https://msdn.microsoft.com/en-us/library/windows/desktop/bb762153(v=vs.85).aspx

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
static KviDefaultMediaType g_defMediaTypes[] = {
	{ "*.jpg", "^\\0330\\0377", "image/jpeg", "JPEG image", "run cmd.exe /c start $0" },
	{ "*.jpeg", "^\\0330\\0377", "image/jpeg", "JPEG image", "run cmd.exe /c start $0" },
	{ "*.png", "", "image/png", "PNG image", "run cmd.exe /c start $0" },
	{ "*.mp3", "", "audio/mpeg", "MPEG audio", "run cmd.exe /c start $0" },
	{ "*.gif", "", "image/gif", "GIF image", "run cmd.exe /c start $0" },
	{ "*.mpeg", "", "video/mpeg", "MPEG video", "run cmd.exe /c start $0" },
	{ "*.zip", "^PK\\0003\\0004", "application/zip", "ZIP archive", "run cmd.exe /c start $0" },
	{ "*.exe", "", "application/x-executable-file", "Executable file", "run $0" },
	{ "*.wav", "", "audio/wav", "Wave audio", "run cmd.exe /c start $0" },
	{ nullptr, nullptr, nullptr, nullptr, nullptr }
};
#else
#ifdef COMPILE_ON_MAC
static KviDefaultMediaType g_defMediaTypes[] = {
	{ "*.jpg", "^\\0330\\0377", "image/jpeg", "JPEG image", "run open $0" },
	{ "*.jpeg", "^\\0330\\0377", "image/jpeg", "JPEG image", "run open $0" },
	{ "*.png", "", "image/png", "PNG image", "run open $0" },
	{ "*.mp3", "", "audio/mpeg", "MPEG audio", "run open $0" },
	{ "*.gif", "", "image/gif", "GIF image", "run open $0" },
	{ "*.mpeg", "", "video/mpeg", "MPEG video", "run open $0" },
	{ "*.zip", "^PK\\0003\\0004", "application/zip", "ZIP archive", "run open $0" },
	{ "*.tar.gz", "", "application/x-gzip", "GZipped tarball", "run open $0" },
	{ "*.tar.bz2", "", "application/x-bzip2", "BZipped tarball", "run open $0" },
	{ "*.tgz", "", "application/x-gzip", "GZipped tarball", "run open $0" },
	{ "*.wav", "", "audio/wav", "Wave audio", "run open $0" },
	{ nullptr, nullptr, nullptr, nullptr, nullptr }
};
#else
static KviDefaultMediaType g_defMediaTypes[] = {
	{ "*.jpg", "^\\0330\\0377", "image/jpeg", "JPEG image", "run xdg-open $0" },
	{ "*.jpeg", "^\\0330\\0377", "image/jpeg", "JPEG image", "run xdg-open $0" },
	{ "*.png", "", "image/png", "PNG image", "run xdg-open $0" },
	{ "*.mp3", "", "audio/mpeg", "MPEG audio", "run xdg-open $0" },
	{ "*.gif", "", "image/gif", "GIF image", "run xdg-open $0" },
	{ "*.mpeg", "", "video/mpeg", "MPEG video", "run xdg-open $0" },
	{ "*.zip", "^PK\\0003\\0004", "application/zip", "ZIP archive", "run xdg-open $0" },
	{ "*.tar.gz", "", "application/x-gzip", "GZipped tarball", "run xdg-open $0" },
	{ "*.tar.bz2", "", "application/x-bzip2", "BZipped tarball", "run xdg-open $0" },
	{ "*.tgz", "", "application/x-gzip", "GZipped tarball", "run xdg-open $0" },
	{ "*.wav", "", "audio/wav", "Wave audio", "run xdg-open $0" },
	{ nullptr, nullptr, nullptr, nullptr, nullptr }
};
#endif
#endif

void KviMediaManager::load(const QString & filename)
{
	KVI_ASSERT(locked());

	KviConfigurationFile cfg(filename, KviConfigurationFile::Read);
	cfg.setGroup("MediaTypes");
	unsigned int nEntries = cfg.readUIntEntry("NEntries", 0);
	for(unsigned int i = 0; i < nEntries; i++)
	{
		KviMediaType * m = new KviMediaType;
		KviCString tmp(KviCString::Format, "%dFileMask", i);
		m->szFileMask = cfg.readEntry(tmp.ptr(), "");
		tmp.sprintf("%dMagicBytes", i);
		m->szMagicBytes = cfg.readEntry(tmp.ptr(), "");
		tmp.sprintf("%dIanaType", i);
		m->szIanaType = cfg.readEntry(tmp.ptr(), "application/unknown");
		tmp.sprintf("%dDescription", i);
		m->szDescription = cfg.readEntry(tmp.ptr(), "");
		tmp.sprintf("%dSavePath", i);
		m->szSavePath = cfg.readEntry(tmp.ptr(), "");
		tmp.sprintf("%dCommandline", i);
		m->szCommandline = cfg.readEntry(tmp.ptr(), "");
		tmp.sprintf("%dRemoteExecCommandline", i);
		m->szRemoteExecCommandline = cfg.readEntry(tmp.ptr(), "");
		tmp.sprintf("%dIcon", i);
		m->szIcon = cfg.readEntry(tmp.ptr(), "");
		insertMediaType(m);
	}

	for(int u = 0; g_defMediaTypes[u].filemask; u++)
	{
		if(!findMediaTypeByFileMask(g_defMediaTypes[u].filemask))
		{
			KviMediaType * m = new KviMediaType;
			m->szFileMask = g_defMediaTypes[u].filemask;
			m->szMagicBytes = g_defMediaTypes[u].magicbytes;
			m->szIanaType = g_defMediaTypes[u].ianatype;
			m->szDescription = g_defMediaTypes[u].description;
			m->szCommandline = g_defMediaTypes[u].commandline;
			insertMediaType(m);
		}
	}
}

void KviMediaManager::save(const QString & filename)
{
	KVI_ASSERT(locked());
	KviConfigurationFile cfg(filename, KviConfigurationFile::Write);

	cfg.clear();
	cfg.setGroup("MediaTypes");
	cfg.writeEntry("NEntries", m_pMediaTypeList->count());
	int index = 0;
	for(KviMediaType * m = m_pMediaTypeList->first(); m; m = m_pMediaTypeList->next())
	{
		KviCString tmp(KviCString::Format, "%dFileMask", index);
		cfg.writeEntry(tmp.ptr(), m->szFileMask.ptr());
		tmp.sprintf("%dMagicBytes", index);
		cfg.writeEntry(tmp.ptr(), m->szMagicBytes.ptr());
		tmp.sprintf("%dIanaType", index);
		cfg.writeEntry(tmp.ptr(), m->szIanaType.ptr());
		tmp.sprintf("%dDescription", index);
		cfg.writeEntry(tmp.ptr(), m->szDescription.ptr());
		tmp.sprintf("%dSavePath", index);
		cfg.writeEntry(tmp.ptr(), m->szSavePath.ptr());
		tmp.sprintf("%dCommandline", index);
		cfg.writeEntry(tmp.ptr(), m->szCommandline.ptr());
		tmp.sprintf("%dRemoteExecCommandline", index);
		cfg.writeEntry(tmp.ptr(), m->szRemoteExecCommandline.ptr());
		tmp.sprintf("%dIcon", index);
		cfg.writeEntry(tmp.ptr(), m->szIcon.ptr());
		++index;
	}
}
