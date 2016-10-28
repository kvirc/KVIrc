#ifndef _KVI_MEDIAMANAGER_H_
#define _KVI_MEDIAMANAGER_H_
//=============================================================================
//
//   File : KviMediaManager.h
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

// This file was originally part of KviMediaType.h

#include "kvi_settings.h"
#include "KviMediaType.h"
#include "KviPointerList.h"
#include "KviThread.h"

class KVILIB_API KviMediaManager : public KviMutex
{
public:
	KviMediaManager();
	~KviMediaManager();

protected:
	KviPointerList<KviMediaType> * m_pMediaTypeList;

private:
	KviMediaType * findMediaTypeForRegularFile(const char * pcFullPath, const char * pcFileName, bool bCheckMagic);

public:
	KviPointerList<KviMediaType> * mediaTypeList() { return m_pMediaTypeList; };
	KviMediaType * findMediaTypeByFileMask(const char * pcFilemask);
	KviMediaType * findMediaTypeByIanaType(const char * pcIanaType);
	bool removeMediaType(KviMediaType * pType) { return m_pMediaTypeList->removeRef(pType); };
	void clear() { m_pMediaTypeList->clear(); };
	void insertMediaType(KviMediaType * pType);
	KviMediaType * findMediaType(const char * pcFilename, bool bCheckMagic = true);
	static void copyMediaType(KviMediaType * pDst, KviMediaType * pSrc);

	void load(const QString & szFilename);
	void save(const QString & szFilename);
};

#endif // _KVI_MEDIAMANAGER_H_
