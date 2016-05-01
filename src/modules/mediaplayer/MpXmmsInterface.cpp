//=============================================================================
//
//   File : MpXmmsInterface.cpp
//   Creation date : Fri 25 Mar 2005 20:04:54 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2010 Szymon Stefanek <pragma at kvirc dot net>
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
//   Thnx to Alexander Stillich <torque at pltn dot org> for Audacious
//   media player interface hints :)
//
//=============================================================================

#include "MpXmmsInterface.h"

#if(!defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MAC) && !defined(COMPILE_ON_MINGW))

#include "KviLocale.h"

MP_IMPLEMENT_DESCRIPTOR(
    KviXmmsInterface,
    "xmms",
    __tr2qs_ctx(
        "An interface for the UNIX XMMS media player.\n"
        "Download it from http://legacy.xmms2.org\n",
        "mediaplayer"))

MP_IMPLEMENT_DESCRIPTOR(
    KviAudaciousClassicInterface,
    "audacious classic",
    __tr2qs_ctx(
        "An interface for the UNIX Audacious media player.\n"
        "Download it from http://audacious-media-player.org\n",
        "mediaplayer"))

static const char * xmms_lib_names[] = {
	"libxmms.so",
	"libxmms.so.1",
	"/usr/lib/libxmms.so",
	"/usr/lib/libxmms.so.1",
	"/usr/local/lib/libxmms.so",
	"/usr/local/lib/libxmms.so.1",
	nullptr
};

static const char * audacious_lib_names[] = {
	"libaudacious.so",
	"libaudacious.so.4",
	"/usr/lib/libaudacious.so",
	"/usr/lib/libaudacious.so.4",
	"/usr/local/lib/libaudacious.so",
	"/usr/local/lib/libaudacious.so.4",
	nullptr
};

KviXmmsInterface::KviXmmsInterface()
    : MpInterface()
{
	m_pPlayerLibrary = nullptr;
	m_szPlayerLibraryName = "libxmms.so";
	m_pLibraryPaths = xmms_lib_names;
}

KviXmmsInterface::~KviXmmsInterface()
{
	if(m_pPlayerLibrary)
	{
		m_pPlayerLibrary->unload();
		delete m_pPlayerLibrary;
		m_pPlayerLibrary = nullptr;
	}
}

KviAudaciousClassicInterface::KviAudaciousClassicInterface()
    : KviXmmsInterface()
{
	m_szPlayerLibraryName = "libaudacious.so";
	m_pLibraryPaths = audacious_lib_names;
}

KviAudaciousClassicInterface::~KviAudaciousClassicInterface()
    = default;

bool KviXmmsInterface::loadPlayerLibrary()
{
	if(m_pPlayerLibrary && m_pPlayerLibrary->isLoaded())
		return true;

	const char ** lib_name = m_pLibraryPaths;
	while(*lib_name)
	{
		m_pPlayerLibrary = new QLibrary(*lib_name);
		if(m_pPlayerLibrary->load())
		{
			m_szPlayerLibraryName = *lib_name;
			return true;
		}
		delete m_pPlayerLibrary;
		m_pPlayerLibrary = nullptr;
		lib_name++;
	}
	return false;
}

void * KviXmmsInterface::lookupSymbol(const char * szSymbolName)
{
	if(!m_pPlayerLibrary)
	{
		if(!loadPlayerLibrary())
		{
			QString szTmp = QString(__tr2qs_ctx("Can't load the player library (%1)", "mediaplayer")).arg(m_szPlayerLibraryName);
			setLastError(szTmp);
			return nullptr;
		}
	}
	void * symptr = (void *)m_pPlayerLibrary->resolve(szSymbolName);
	if(!symptr)
	{
		QString szTmp = QString(__tr2qs_ctx("Can't find symbol %1 in %2", "mediaplayer")).arg(szSymbolName, m_szPlayerLibraryName);
		setLastError(szTmp);
	}
	return symptr;
}

int KviXmmsInterface::detect(bool)
{
	void * sym = lookupSymbol("xmms_remote_play");
	return sym ? 80 : 0;
}

#define XMMS_SIMPLE_CALL(__symname)                            \
	void (*sym)(int) = (void (*)(int))lookupSymbol(__symname); \
	if(!sym)                                                   \
		return false;                                          \
	sym(0);                                                    \
	return true;

bool KviXmmsInterface::prev()
{
	XMMS_SIMPLE_CALL("xmms_remote_playlist_prev")
}

bool KviXmmsInterface::next()
{
	XMMS_SIMPLE_CALL("xmms_remote_playlist_next")
}

bool KviXmmsInterface::play()
{
	XMMS_SIMPLE_CALL("xmms_remote_play")
}

bool KviXmmsInterface::stop()
{
	XMMS_SIMPLE_CALL("xmms_remote_stop")
}

bool KviXmmsInterface::pause()
{
	XMMS_SIMPLE_CALL("xmms_remote_pause")
}

bool KviXmmsInterface::quit()
{
	XMMS_SIMPLE_CALL("xmms_remote_quit")
}

bool KviXmmsInterface::jumpTo(kvs_int_t & iPos)
{
	void (*sym)(int, int) = (void (*)(int, int))lookupSymbol("xmms_remote_jump_to_time");
	if(!sym)
		return false;
	sym(0, iPos);
	return true;
}

bool KviXmmsInterface::setVol(kvs_int_t & iVol)
{
	void (*sym)(int, int) = (void (*)(int, int))lookupSymbol("xmms_remote_set_main_volume");
	if(!sym)
		return false;
	sym(0, 100 * iVol / 255);
	return true;
}

int KviXmmsInterface::getVol()
{
	int (*sym)(int) = (int (*)(int))lookupSymbol("xmms_remote_get_main_volume");
	if(!sym)
		return -1;
	int iVol = sym(0);
	return iVol * 255 / 100;
}

bool KviXmmsInterface::getRepeat()
{
	bool (*sym)(int) = (bool (*)(int))lookupSymbol("xmms_remote_is_repeat");
	if(!sym)
		return false;
	bool ret = sym(0);
	return ret;
}

bool KviXmmsInterface::setRepeat(bool & bVal)
{
	bool (*sym1)(int) = (bool (*)(int))lookupSymbol("xmms_remote_is_repeat");
	if(!sym1)
		return false;
	bool bNow = sym1(0);
	if(bNow != bVal)
	{
		void (*sym2)(int) = (void (*)(int))lookupSymbol("xmms_remote_toggle_repeat");
		if(!sym2)
			return false;
		sym2(0);
	}
	return true;
}

bool KviXmmsInterface::getShuffle()
{
	bool (*sym)(int) = (bool (*)(int))lookupSymbol("xmms_remote_is_shuffle");
	if(!sym)
		return false;
	bool ret = sym(0);
	return ret;
}

bool KviXmmsInterface::setShuffle(bool & bVal)
{
	bool (*sym1)(int) = (bool (*)(int))lookupSymbol("xmms_remote_is_shuffle");
	if(!sym1)
		return false;
	bool bNow = sym1(0);
	if(bNow != bVal)
	{
		void (*sym2)(int) = (void (*)(int))lookupSymbol("xmms_remote_toggle_shuffle");
		if(!sym2)
			return false;
		sym2(0);
	}
	return true;
}

MpInterface::PlayerStatus KviXmmsInterface::status()
{
	bool (*sym1)(int) = (bool (*)(int))lookupSymbol("xmms_remote_is_paused");
	if(sym1)
	{
		if(sym1(0))
			return MpInterface::Paused;
		bool (*sym2)(int) = (bool (*)(int))lookupSymbol("xmms_remote_is_playing");
		if(sym2)
		{
			if(sym2(0))
				return MpInterface::Playing;
			else
				return MpInterface::Stopped;
		}
	}

	return MpInterface::Unknown;
}

bool KviXmmsInterface::playMrl(const QString & mrl)
{
	void (*sym)(int, char *) = (void (*)(int, char *))lookupSymbol("xmms_remote_playlist_add_url_string");
	QByteArray tmp = mrl.toLocal8Bit();
	if(!tmp.isEmpty())
	{
		if(sym)
		{
			sym(0, tmp.data());
			int (*sym1)(int) = (int (*)(int))lookupSymbol("xmms_remote_get_playlist_length");
			if(sym1)
			{
				int len = sym1(0);
				if(len > 0)
				{
					void (*sym2)(int, int) = (void (*)(int, int))lookupSymbol("xmms_remote_set_playlist_pos");
					if(sym2)
					{
						sym2(0, len - 1);
					}
					else
						return false;
				}
				else
					return false;
			}
			else
				return false;
		}
		else
			return false;
	}
	return true;
}

QString KviXmmsInterface::nowPlaying()
{
	int (*sym)(int) = (int (*)(int))lookupSymbol("xmms_remote_get_playlist_pos");
	if(!sym)
		return QString();
	int pos = sym(0);
	char * (*sym2)(int, int) = (char * (*)(int, int))lookupSymbol("xmms_remote_get_playlist_title");
	if(!sym2)
		return QString();
	return QString::fromLocal8Bit(sym2(0, pos));
}

QString KviXmmsInterface::mrl()
{
	int (*sym)(int) = (int (*)(int))lookupSymbol("xmms_remote_get_playlist_pos");
	if(!sym)
		return QString();
	int pos = sym(0);
	char * (*sym2)(int, int) = (char * (*)(int, int))lookupSymbol("xmms_remote_get_playlist_file");
	if(!sym2)
		return QString();
	QString ret = QString::fromLocal8Bit(sym2(0, pos));
	if(ret.length() > 1)
		if(ret[0] == '/')
			ret.prepend("file://");
	return ret;
}

int KviXmmsInterface::position()
{
	int (*sym)(int) = (int (*)(int))lookupSymbol("xmms_remote_get_playlist_pos");
	if(!sym)
		return -1;
	int pos = sym(0);
	int (*sym2)(int, int) = (int (*)(int, int))lookupSymbol("xmms_remote_get_output_time");
	if(!sym2)
		return -1;
	return sym2(0, pos);
}

int KviXmmsInterface::length()
{
	int (*sym)(int) = (int (*)(int))lookupSymbol("xmms_remote_get_playlist_pos");
	if(!sym)
		return -1;
	int pos = sym(0);
	int (*sym2)(int, int) = (int (*)(int, int))lookupSymbol("xmms_remote_get_playlist_time");
	if(!sym2)
		return -1;
	return sym2(0, pos);
}

int KviXmmsInterface::getPlayListPos()
{
	int (*sym)(int) = (int (*)(int))lookupSymbol("xmms_remote_get_playlist_pos");
	if(!sym)
		return -1;
	return sym(0);
}

#endif //!COMPILE_ON_WINDOWS
