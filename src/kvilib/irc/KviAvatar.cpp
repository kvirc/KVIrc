//=============================================================================
//
//   File : KviAvatar.cpp
//   Creation date : Fri Dec 01 2000 13:58:12 CEST by Szymon Stefanek
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

#include "KviAvatar.h"
#include "KviQString.h"

#include <QImage>
#include <QImageReader>

/*
	@doc: ctcp_avatar
	@title:
		The AVATAR idea
	@short:
		Extending IRC fun: the AVATAR idea
	@body:
		[big]Introduction[/big]
		Starting from version 3.0.0 KVIrc supports the AVATAR protocol.
		The AVATAR term dictionary definitions include:[br]
		- Hindu mythology incarnation of a God[br]
		- Embodiment of a concept or philosophy[br]
		- In [b]cyberspace communities[/b], the representation of an
		user in a shared virtual reality.[br]
		The AVATAR protocol attempts to improve the IRC communication
		by adding a method for associating a graphical representation
		to an IRC user.[br]
		Since this may involve binary data transfers between users,
		the protocol is intended to be client based.
		[big]The terms[/big]
		The user that wants to offer a digital representation of himself
		will be called [i]source user[/i]. The ones that will receive the notification
		will be called [i]target users[/i].
		Every IRC user can be either a source user or target user.
		[big]The idea[/big]
		Every IRC user has a client-side property called AVATAR. Let's say that there
		are two users: A and B.[br]
		When user A wants to see the B's avatar he simply sends a CTCP AVATAR request
		to B (the request is sent through a PRIVMSG IRC command).[br]
		User B replies with a CTCP AVATAR notification (sent through a NOTICE IRC command)
		with the name or URL of his avatar.[br]
		The actual syntax for the notification is:[br]
		[b]AVATAR <avatar_file> [<filesize>][/b]
		The <avatar_file> may be either the name of a B's local image file or a URL
		pointing to an image on some web server.[br]
		The optional <filesize> parameter is sent only if <avatar_file> is
		stored on the B's machine and there will be more info on that later.[br]
		Anyway, after A has received the notification he tries to locate the avatar
		file in its local cache (yes, <filesize> may help here and more on this later).
		If the file can be found
		and loaded then it is simply displayed in some way near the B's nickname
		otherwise A must download the avatar from some place.
		If the <avatar_file> contains a leading URL prefix (http://) then
		A fetches the image from the specified URL and after that displays
		it near the B's nickname. If the <avatar_file> does not contain the
		leading URL prefix then it is assumed that B offers this file for
		downloading via DCC from his machine. In this case A may also avoid
		requesting the file if the <filesize> is too large and the transfer
		would occupy too much bandwidth (for example).
		The DCC download is initiated by issuing a DCC GET <avatar_file> request to B.
		B may then reply with a standard DCC SEND or a DCC RSEND (KVIrc's extension).[br]
		The implementation of the DCC GET protocol is defined by other documents here around :).[br]
		[br]
		The CTCP AVATAR messages can be sent to a single user, a set of users or a channel:
		this depends only on the source user and how many clients he wants to reach.
		[br]
		There should be a convention on the image sizes: not a protocol limit.
		For example, the convention could be that all the images should be smaller than
		129x129 pixels. The preferred image format is [i]png[/i] (Portable Network Graphics)
		since it allows good compression rates without compromising the image quality.
		Other formats may be accepted as well (Theoretically this protocol could be
		extended to allow movies or 3D images).
		The preferred image size may grow with time, as the network transmission speed grows.
*/

KviAvatar::KviAvatar(
    const QString & szLocalPath,
    const QString & szName,
    const QSize & scaleOnLoad)
{
	m_bRemote = KviQString::equalCIN("http://", szName, 7) || KviQString::equalCIN("https://", szName, 8);

	m_szLocalPath = szLocalPath;
	m_szName = szName;

	m_pPixmap = new KviAnimatedPixmap(szLocalPath);

	if(scaleOnLoad.isValid() && (scaleOnLoad.height() < size().height() || scaleOnLoad.width() < size().width()))
	{
		m_pPixmap->resize(scaleOnLoad, Qt::KeepAspectRatio);
	}

	m_scaledPixmapsCache.insert(m_pPixmap->size(), m_pPixmap);
}

KviAvatar::~KviAvatar()
{
	foreach(KviAnimatedPixmap * pix, m_scaledPixmapsCache)
		delete pix;
}

KviAnimatedPixmap * KviAvatar::forSize(const QSize & size)
{
	if(m_scaledPixmapsCache.contains(size))
		return m_scaledPixmapsCache[size];

	KviAnimatedPixmap * scaledPixmap = new KviAnimatedPixmap(*m_pPixmap);
	scaledPixmap->resize(size, Qt::KeepAspectRatio);

	m_scaledPixmapsCache.insert(size, scaledPixmap);
	return scaledPixmap;
}
