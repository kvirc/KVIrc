//=============================================================================
//
//   File : KviPixmap.cpp
//   Creation date : Sat Jun 24 2000 14:00:27 by Szymon Stefanek
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

#include "kvi_debug.h"
#include "KviPixmap.h"
#include "KviQString.h"

#include <QString>
#include <memory>

KviPixmap::KviPixmap()
    = default;

KviPixmap::KviPixmap(const char * path)
{
	load(path);
}

KviPixmap::KviPixmap(const KviPixmap & pix)
{
	if(!pix.path().isEmpty() && !pix.isNull())
		set(*(pix.pixmap()), pix.path());
}

bool KviPixmap::load(const char * path)
{
	return load(QString(path));
}

bool KviPixmap::load(const QString & path)
{
	if(path.isEmpty())
	{
		setNull();
		return false;
	}

	m_pPix = std::make_unique<QPixmap>(path);

	if(m_pPix->isNull())
	{
		setNull();
		return false;
	}

	m_szPath = path;

	return true;
}

void KviPixmap::set(const QPixmap & pix, const QString & szPath)
{
	if(pix.isNull())
	{
		setNull();
		return;
	}

	m_pPix = std::make_unique<QPixmap>(pix);
	m_szPath = szPath;
}

void KviPixmap::setNull()
{
	m_pPix.reset();
	m_szPath = "";
}

KviPixmap & KviPixmap::operator=(const KviPixmap & pix)
{
	if(this == &pix)
		return *this; // self assignment
	if(KviQString::equalCI(m_szPath, pix.path()))
		return *this; // same pix

	if(!pix.path().isEmpty() && !pix.isNull())
	{
		m_szPath = pix.path();
		m_pPix = std::make_unique<QPixmap>(*(pix.pixmap()));
	}
	else
		setNull();

	return *this;
}
