//=============================================================================
//
//   File : kvi_pixmap.cpp
//   Creation date : Sat Jun 24 2000 14:00:27 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_pixmap.h"
#include "kvi_qstring.h"

#include <QPainter>

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


void KviPixmapUtils::drawPixmapWithPainter(QPainter* p,QPixmap * pix,int flags,const QRect& paintRect,int iWidgetWidth,int iWidgetHeight,int dx,int dy)
{
	if(!pix)return;
	if(!flags)
	{
		p->drawTiledPixmap(paintRect.left(),paintRect.top(),paintRect.width(),paintRect.height(),*pix,dx,dy);
		return;
	}

	int iPixWidth=pix->width();
	int iPixHeight=pix->height();
	int x=0;
	int y=0;

	if( !(flags & Qt::AlignHorizontal_Mask ))
		x=-1;
	else if ( flags & Qt::AlignRight )
		x=iWidgetWidth - iPixWidth;
	else if( flags & Qt::AlignHCenter )
		x=(iWidgetWidth - iPixWidth)/2;

	if( !(flags & Qt::AlignVertical_Mask ))
		y=-1;
	else if ( flags & Qt::AlignBottom )
		y=iWidgetHeight - iPixHeight;
	else if( flags & Qt::AlignVCenter )
		y=(iWidgetHeight - iPixHeight)/2;

	if(x==-1) {
		p->drawTiledPixmap(paintRect.left(),y,paintRect.width(),iPixHeight,*pix,dx,dy);
	} else if(y==-1) {
		p->drawTiledPixmap(x,paintRect.top(),iPixWidth,paintRect.height(),*pix,dx,dy);
	} else {
		p->drawPixmap(x,y,*pix);
	}
}
