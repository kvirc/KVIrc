//=============================================================================
//
//   File : KviPixmap.cpp
//   Creation date : Sat Jun 24 2000 14:00:27 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================




#include "kvi_debug.h"
#include "KviPixmap.h"
#include "KviQString.h"

KviPixmap::KviPixmap()
{
	m_pPix = 0;
}

KviPixmap::KviPixmap(const char * path)
{
	m_pPix = 0;
	load(path);
}

KviPixmap::KviPixmap(const KviPixmap &pix)
{
	m_pPix   = 0;
	m_szPath = pix.path();

	if(!m_szPath.isEmpty())
	{
		if(pix.pixmap())
		{
			m_pPix = new QPixmap(*(pix.pixmap()));
		}
	}
}

KviPixmap::~KviPixmap()
{
	if(m_pPix)
	{
		delete m_pPix;
		m_pPix = 0; // just to be sure :)
	}
}

bool KviPixmap::load(const char * path)
{
	if(m_pPix)
	{
		delete m_pPix;
		m_pPix   = 0;
	}
	m_szPath = path;
	if(m_szPath.isEmpty())return false;

	m_pPix = new QPixmap(m_szPath);

	if(m_pPix->isNull())
	{
		delete m_pPix;
		m_pPix = 0;
		m_szPath = "";
		return false;
	}
	return true;
}

bool KviPixmap::load(const QString& path)
{
	if(m_pPix)
	{
		delete m_pPix;
		m_pPix   = 0;
	}
	m_szPath = path;
	if(m_szPath.isEmpty())return false;

	m_pPix = new QPixmap(m_szPath);

	if(m_pPix->isNull())
	{
		delete m_pPix;
		m_pPix = 0;
		m_szPath = "";
		return false;
	}
	return true;
}

void KviPixmap::set(const QPixmap &pix,const QString &szPath)
{
	if(pix.isNull())
	{
		setNull();
		return;
	}

	if(m_pPix)
		delete m_pPix;
	m_pPix = new QPixmap(pix);
	m_szPath = szPath;
}


void KviPixmap::setNull()
{
	if(m_pPix)
	{
		delete m_pPix;
		m_pPix = 0;
	}
	m_szPath = "";
}

KviPixmap & KviPixmap::operator=(const KviPixmap &pix)
{
	if(m_pPix == pix.m_pPix)return (*this); // self assignment (!!!)
	if(KviQString::equalCI(m_szPath,pix.path()))return (*this); // same pix

	if(m_pPix)
	{
		delete m_pPix;
		m_pPix   = 0;
	}
	m_szPath = pix.path();

	if(!m_szPath.isEmpty())
	{
		if(pix.pixmap())
		{
			m_pPix = new QPixmap(*(pix.pixmap()));
		}
	}
	return (*this);
}
