#ifndef _KVI_AVATAR_H_
#define _KVI_AVATAR_H_

//=============================================================================
//
//   File : kvi_avatar.h
//   Creation date : Fri Dec 01 2000 13:54:04 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2004 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_heapobject.h"
#include "kvi_settings.h"

#include "kvi_animatedpixmap.h"

#include <QPixmap>
#include <QHash>

inline uint qHash(const QSize& s) { return uint(s.width() | s.height()); }

class KVILIB_API KviAvatar : public KviHeapObject
{
public:
	KviAvatar(const QString &szLocalPath,
			const QString &szName,
			const QSize& scaleOnLoad = QSize());
	~KviAvatar();

private:
	QString                             m_szLocalPath;
	QString                             m_szName;
	bool                                m_bRemote;

	KviAnimatedPixmap                 * m_pPixmap;
	QHash<QSize,KviAnimatedPixmap*>     m_scaledPixmapsCache;

public:

	/*
	 * Returns true, if avatar is remote.
	 */
	inline bool isRemote() {
		return m_bRemote;
	}

	/*
	 * Returns filepath
	 */
	inline const QString &localPath() {
		return m_szLocalPath;
	}

	/*
	 * Returns fiename. For remote avatars, remote address will be returned.
	 */
	inline const QString &name() {
		return m_szName;
	}

	/*
	 * Returns true, if underlying pixmap contains more, then one frame.
	 */
	inline bool isAnimated() {
		return (m_pPixmap->framesCount() > 1);
	}

	/*
	 * Returns original pixmap's size
	 */
	inline const QSize& size() {
		return m_pPixmap->size();
	}

	/*
	 * Returns unscaled original frame.
	 */
	inline QPixmap* pixmap() {
		return m_pPixmap->pixmap();
	}

	/*
	 * Returns true if pixmap has at least one loaded frame.
	 * Returns false otherways.
	 */
	inline bool isValid()
	{
		return m_pPixmap->isValid();
	}

	/*
	 * Returns original animated pixmap.
	 */
	inline KviAnimatedPixmap * animatedPixmap() {
		return m_pPixmap;
	}

	/*
	 * Returns animated pixmap, scaled to the requisted size.
	 * Scaling automagically keeps aspect ratio.
	 * Scaled image cached.
	 *
	 * KviAvatar takes ownership on it, and will automatically delete it
	 * in destructor.
	 *
	 * It is safe to connect returned KviAnimatedPixmap signals to external
	 * slots.
	 */

	KviAnimatedPixmap * forSize(const QSize& size);
	inline KviAnimatedPixmap * forSize(unsigned int w, unsigned int h) {
		return forSize(QSize(w,h));
	}

	// string that uniquely identifies this avatar
	// for remote avatars that have name starting with http://
	// the name is used.
	// for local avatars the localPath is used instead
	const QString &identificationString(){ return m_bRemote ? m_szName : m_szLocalPath; };


	// if name is http://xxxx
	// then identification is the name
	// if name is xxx.png
	// then identification is the local path


	// name : visible name of the avatar : url or filename
	//        ex: http://www.kvirc.net/img/pragma.png
	//        ex: pragma.png
	// local path : local path
	//        ex: /home/pragma/.kvirc/avatars/http.www.kvirc.net.img.pragma.png
	//        ex: /home/pragma/.kvirc/avatars/pragma.png

	// local path->name : strip leading path informations
	// name->local path : replace : / and

};

#endif //_KVI_AVATAR_H_
