//=============================================================================
//
//   File : MpSpotifyInterface.cpp
//   Creation date : Sat 09 Apr 2011 10:38:12 by David López
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2011 David López <grannost at gmail dot com>
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

#include "MpSpotifyInterface.h"
#include "KviOptions.h"

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)

#include <QTextCodec>

#include "KviLocale.h"
#include "KviModule.h"

#include <windows.h>

static HWND find_spotify(KviSpotifyInterface * i)
{
	HWND hWnd = FindWindow("SpotifyMainWindow",NULL);
	if(!hWnd)
	{
		// try to start the process ?
		i->setLastError(__tr2qs_ctx("Spotify is not running","mediaplayer"));
	}
	return hWnd;
}

MP_IMPLEMENT_DESCRIPTOR(
	KviSpotifyInterface,
	"spotify",
	__tr2qs_ctx(
		"An interface to the Spotify music streaming service.\n" \
		"It is only available for certain countries in Europe.\n" \
		"You can download it from http://www.spotify.com.\n"
		,
		"mediaplayer"
	)
)

KviSpotifyInterface::KviSpotifyInterface()
: MpInterface()
{
}

KviSpotifyInterface::~KviSpotifyInterface()
{
}

QString KviSpotifyInterface::get_window_title() {
	HWND hSpotify;
	QString szReturn;
	if (!(hSpotify = find_spotify(this)))return QString("");

	QByteArray nowPlaying = QByteArray((GetWindowTextLength(hSpotify) + 1)
		* sizeof(wchar_t), 0);

	if (GetWindowTextW(hSpotify, (LPWSTR)nowPlaying.data(), nowPlaying.size())) {
		szReturn = QString::fromWCharArray((wchar_t *)nowPlaying.data());
		if (szReturn == "Spotify")
			return QString("");
		return szReturn.replace(QString("Spotify - "), QString(""));
	} else
		szReturn = QString("");
	
	return szReturn;
}

MpInterface::PlayerStatus KviSpotifyInterface::status() {
	if (get_window_title() == QString(""))
		return MpInterface::Stopped;
	else
		return MpInterface::Playing;
}

int KviSpotifyInterface::detect(bool bStart)
{
	if(find_spotify(this))return 100;
	return 0;
}

QString KviSpotifyInterface::nowPlaying()
{
	return get_window_title().replace(QString("\u2013"), QString("-"));
}

QString KviSpotifyInterface::artist()
{
	QString title = get_window_title();
	int pos = title.indexOf(QString(" \u2013 "));
	if (pos != 1)return title.left(pos);
	else return title;
}

QString KviSpotifyInterface::title()
{
	QString title = get_window_title();
	int pos = title.indexOf(QString(" \u2013 "));
	if (pos != 1)return title.right(title.length() - (pos + 3));
	else return title;
}

bool KviSpotifyInterface::show()
{
	HWND hSpotify = find_spotify(this);
	if(hSpotify)
	{
		ShowWindow(hSpotify, SW_SHOW);
		return true;
	}
	return false;
}

bool KviSpotifyInterface::minimize()
{
	HWND hSpotify = find_spotify(this);
	if(hSpotify)
	{
		ShowWindow(hSpotify, SW_SHOWMINIMIZED);
		return true;
	}
	return false;
}

bool KviSpotifyInterface::hide()
{
	HWND hSpotify = find_spotify(this);
	if(hSpotify)
	{
		ShowWindow(hSpotify, SW_HIDE);
		return true;
	}
	return false;
}

#endif //COMPILE_ON_WINDOWS
