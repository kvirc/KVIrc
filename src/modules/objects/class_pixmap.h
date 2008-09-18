#ifndef	_CLASS_PIXMAP_H_
#define	_CLASS_PIXMAP_H_
//=============================================================================
//
//   File : class_pixmap.h
//   Creation date : Fri Mar 18 14:30:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2005-2008 Alessandro Carbone (elfonol at gmail dot com)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================



#include <qpixmap.h>
#include <qimage.h>
#include <QMatrix>
#include "kvi_string.h"


#include "object_macros.h"


class KviKvsObject_pixmap : public KviKvsObject
{
	Q_OBJECT
//	Q_PROPERTY(QPixmap pixmap READ getPixmap)
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_pixmap);
	QPixmap  * getPixmap();
	QImage * getImage();
	void pixmapChanged(){bPixmapModified=true;}
	void imageChanged(){bImageModified=true;}

	QPixmap * pixmap(){ return m_pPixmap; }
protected:
	QPixmap * m_pPixmap;
	QImage * m_pImage;
	bool bImageModified;
	bool bPixmapModified;
signals:
	void aboutToDie();

protected:
	bool functionfill(KviKvsObjectFunctionCall *c);
	bool functionresize(KviKvsObjectFunctionCall *c);
	bool functionscale(KviKvsObjectFunctionCall *c);
	bool functionrotate(KviKvsObjectFunctionCall *c);
	bool functionload(KviKvsObjectFunctionCall *c);
	bool functionheight(KviKvsObjectFunctionCall *c);
	bool functionwidth(KviKvsObjectFunctionCall *c);
	bool functionsetOpacity(KviKvsObjectFunctionCall *c);

  };

#endif	// !_CLASS_PIXMAP_H_
