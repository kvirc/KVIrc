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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================



#include <qpixmap.h>
#include <qimage.h>
#include <QMatrix>
#include <kvi_animatedpixmap.h>
#include "kvi_string.h"


#include "object_macros.h"


class KviKvsObject_pixmap : public KviKvsObject
{
	Q_OBJECT
//	Q_PROPERTY(QPixmap pixmap READ getPixmap)
public:
	KVSO_DECLARE_OBJECT(KviKvsObject_pixmap)
	QPixmap  * getPixmap();

            //QPixmap * pixmap(){ return m_pPixmap; }
protected:
            KviAnimatedPixmap * m_pAnimatedPixmap;
            QPixmap * m_pPixmap;
signals:
	void aboutToDie();
protected:
        bool fill(KviKvsObjectFunctionCall *c);
        bool resize(KviKvsObjectFunctionCall *c);
        bool scale(KviKvsObjectFunctionCall *c);
        //bool rotate(KviKvsObjectFunctionCall *c);
        bool save(KviKvsObjectFunctionCall *c);
        bool startAnimation(KviKvsObjectFunctionCall *c);
        bool stopAnimation(KviKvsObjectFunctionCall *c);
        bool loadAnimation(KviKvsObjectFunctionCall *c);
        bool load(KviKvsObjectFunctionCall *c);
        bool height(KviKvsObjectFunctionCall *c);
        bool width(KviKvsObjectFunctionCall *c);
        bool grabWidget(KviKvsObjectFunctionCall *c);
//	bool setOpacity(KviKvsObjectFunctionCall *c);
        bool loadFromMemoryBuffer(KviKvsObjectFunctionCall *c);

        bool frameChangedEvent(KviKvsObjectFunctionCall *c);

protected slots:
        void frameChanged();
  };

#endif	// !_CLASS_PIXMAP_H_
