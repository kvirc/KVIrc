#ifndef _KVI_AVATAR_H_
#define _KVI_AVATAR_H_
//=============================================================================
//
//   File : KviAvatar.h
//   Creation date : Fri Dec 01 2000 13:54:04 CEST by Szymon Stefanek
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

/**
* \file KviAvatar.h
* \author Szymon Stefanek
* \brief Avatar handling
*/

#include "kvi_settings.h"
#include "KviAnimatedPixmap.h"
#include "KviHeapObject.h"

#include <QHash>
#include <QPixmap>
#include <QSize>
#include <QString>

/**
* \brief Returns an hash based on avatar dimensions
* \param s
* \return uint
*/
inline uint qHash(const QSize & s) { return uint(s.width() | s.height()); }

/**
* \class KviAvatar
* \brief Avatar class
*/
class KVILIB_API KviAvatar : public KviHeapObject
{
public:
	/**
	* \brief Constructs the avatar object
	* \param szLocalPath The local path of the avatar
	* \param szName The name of the avatar
	* \param scaleOnLoad If we want to scale the avatar on load
	* \return KviAvatar
	*/
	KviAvatar(const QString & szLocalPath,
	    const QString & szName,
	    const QSize & scaleOnLoad = QSize());

	/**
	* \brief Destroys the avatar object
	*/
	~KviAvatar();

private:
	QString m_szLocalPath;
	QString m_szName;
	bool m_bRemote;

	KviAnimatedPixmap * m_pPixmap;
	QHash<QSize, KviAnimatedPixmap *> m_scaledPixmapsCache;

public:
	/**
	* \brief Returns true, if avatar is remote.
	* \return bool
	*/
	bool isRemote() const { return m_bRemote; }

	/**
	* \brief Returns filepath
	* \return const QString &
	*/
	const QString & localPath() const { return m_szLocalPath; }

	/**
	* \brief Returns fiename.
	*
	* For remote avatars, remote address will be returned.
	* \return const QString &
	*/
	const QString & name() const { return m_szName; }

	/**
	* \brief Returns true, if underlying pixmap contains more then one frame.
	* \return bool
	*/
	bool isAnimated() const { return (m_pPixmap->framesCount() > 1); }

	/**
	* \brief Returns original pixmap's size
	* \return const QSize &
	*/
	const QSize & size() const { return m_pPixmap->size(); }

	/**
	* \brief Returns unscaled original frame.
	* \return QPixmap *
	*/
	QPixmap * pixmap() const { return m_pPixmap->pixmap(); }

	/**
	* \brief Returns true if pixmap has at least one loaded frame.
	*
	* Returns false otherwise.
	* \return bool
	*/
	bool isValid() const { return m_pPixmap->isValid(); }

	/**
	* \brief Returns original animated pixmap.
	* \return KviAnimatedPixmap *
	*/
	KviAnimatedPixmap * animatedPixmap() const { return m_pPixmap; }

	/**
	* \brief Returns animated pixmap, scaled to the requisted size.
	*
	* Scaling automagically keeps aspect ratio.
	* Scaled image cached.
	*
	* KviAvatar takes ownership on it, and will automatically delete it
	* in destructor.
	*
	* It is safe to connect returned KviAnimatedPixmap signals to external
	* slots.
	* \param size The size of the avatar
	* \return KviAnimatedPixmap *
	*/
	KviAnimatedPixmap * forSize(const QSize & size);

	/**
	* \brief Returns animated pixmap, scaled to the requisted size.
	*
	* Scaling automagically keeps aspect ratio.
	* Scaled image cached.
	*
	* KviAvatar takes ownership on it, and will automatically delete it
	* in destructor.
	*
	* It is safe to connect returned KviAnimatedPixmap signals to external
	* slots.
	* \param uWidth The width of the avatar
	* \param uHeight The height of the avatar
	* \return KviAnimatedPixmap *
	*/
	KviAnimatedPixmap * forSize(unsigned int uWidth, unsigned int uHeight) { return forSize(QSize(uWidth, uHeight)); }

	/**
	* \brief Returns the string that uniquely identifies this avatar.
	*
	* For remote avatars that have name starting with http:// the name is used.
	* For local avatars the localPath is used instead
	* \return const QString &
	*/
	const QString & identificationString() { return m_bRemote ? m_szName : m_szLocalPath; };
};

#endif //_KVI_AVATAR_H_
