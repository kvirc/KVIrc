#ifndef _KVI_PIXMAP_H_
#define _KVI_PIXMAP_H_
//=============================================================================
//
//   File : KviPixmap.h
//   Creation date : Sat Jun 24 2000 13:59:04 by Szymon Stefanek
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

#include "kvi_settings.h"

#include <memory>
#include <QPixmap>

class QString;

class KVILIB_API KviPixmap
{
public:
	KviPixmap();
	KviPixmap(const char * path);
	KviPixmap(const KviPixmap & pix);

private:
	std::unique_ptr<QPixmap> m_pPix;
	QString m_szPath;

public:
	const QString & path() const { return m_szPath; };
	QPixmap * pixmap() const { return m_pPix.get(); };

	bool isNull() const { return m_pPix == nullptr; };

	bool load(const char * path);
	bool load(const QString & path);
	void set(const QPixmap & pix, const QString & szPath);

	KviPixmap & operator=(const KviPixmap & pix); // deep copy

	void setNull();
};

#endif //_KVI_PIXMAP_H_
