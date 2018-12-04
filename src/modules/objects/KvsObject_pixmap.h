#ifndef _CLASS_PIXMAP_H_
#define _CLASS_PIXMAP_H_
//=============================================================================
//
//   File : KvsObject_pixmap.h
//   Creation date : Fri Mar 18 14:30:48 CEST 2005
//   by Tonino Imbesi(Grifisx) and Alessandro Carbone(Noldor)
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2008 Alessandro Carbone (elfonol at gmail dot com)
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

#include <qpixmap.h>
#include <qimage.h>
#include <KviAnimatedPixmap.h>
#include "KviCString.h"

#include "object_macros.h"

class KvsObject_pixmap : public KviKvsObject
{
	Q_OBJECT
	//	Q_PROPERTY(QPixmap pixmap READ getPixmap)
public:
	KVSO_DECLARE_OBJECT(KvsObject_pixmap)
	enum Type
	{
		Pixmap,
		AnimatedPixmap,
		Image
	};
	Type m_currentType;
	void setInternalPixmap(QPixmap * pPixmap);
	void setInternalImage(QImage * pImage);

	QImage * getImage()
	{
		if(!m_pImage)
			m_pImage = new QImage();
		if(m_currentType == Pixmap)
		{
			if(m_pPixmap)
			{
				*m_pImage = m_pPixmap->toImage();
				delete m_pPixmap;
				m_pPixmap = nullptr;
			}
		}
		m_currentType = Image;
		return m_pImage;
	}

	QPixmap * getPixmap()
	{
		if(!m_pPixmap)
			m_pPixmap = new QPixmap();
		if(m_currentType == Image)
		{
			if(m_pImage)
			{
				*m_pPixmap = m_pPixmap->fromImage(*m_pImage);
				delete m_pImage;
				m_pImage = nullptr;
			}
		}
		m_currentType = Pixmap;
		return m_pPixmap;
	}

protected:
	KviAnimatedPixmap * m_pAnimatedPixmap;
	QPixmap * m_pPixmap;
	QImage * m_pImage;
signals:
	void aboutToDie();

protected:
	bool fill(KviKvsObjectFunctionCall * c);
	bool resize(KviKvsObjectFunctionCall * c);
	bool scale(KviKvsObjectFunctionCall * c);
	//bool rotate(KviKvsObjectFunctionCall *c);
	bool save(KviKvsObjectFunctionCall * c);
	bool startAnimation(KviKvsObjectFunctionCall * c);
	bool stopAnimation(KviKvsObjectFunctionCall * c);
	bool loadAnimation(KviKvsObjectFunctionCall * c);
	bool load(KviKvsObjectFunctionCall * c);
	bool height(KviKvsObjectFunctionCall * c);
	bool width(KviKvsObjectFunctionCall * c);
	bool mirrored(KviKvsObjectFunctionCall * c);
	bool grabWidget(KviKvsObjectFunctionCall * c);
	bool loadFromMemoryBuffer(KviKvsObjectFunctionCall * c);
	bool rotate(KviKvsObjectFunctionCall * c);
	bool toGrayScale(KviKvsObjectFunctionCall * c);
	bool setPixel(KviKvsObjectFunctionCall * c);
	bool pixel(KviKvsObjectFunctionCall * c);
	bool frameChangedEvent(KviKvsObjectFunctionCall * c);
protected slots:
	void frameChanged();
};

#endif // !_CLASS_PIXMAP_H_
