//=============================================================================
//
//   File : mp_xmmsinterface.cpp
//   Created on Fri 25 Mar 2005 20:04:54 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005 Szymon Stefanek <pragma at kvirc dot net>
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

#include "mp_xmmsinterface.h"

#ifndef COMPILE_ON_WINDOWS

#include "kvi_library.h"
#include "kvi_locale.h"

MP_IMPLEMENT_DESCRIPTOR(
	KviXmmsInterface,
	"xmms",
	__tr2qs_ctx(
		"An interface to the popular UNIX xmms media player.\n" \
		"Download it from http://www.xmms.org\n"
		,
		"mediaplayer"
	)
)


static kvi_library_t g_hXmmsLib = 0;

static inline kvi_library_t load_xmms_lib()
{
	if(g_hXmmsLib)return g_hXmmsLib;
	g_hXmmsLib = kvi_library_load("libxmms.so");
	if(!g_hXmmsLib)
	{
		g_hXmmsLib = kvi_library_load("libxmms.so.1");
		if(!g_hXmmsLib)
		{
			g_hXmmsLib = kvi_library_load("/usr/lib/libxmms.so");
			if(!g_hXmmsLib)
			{
				g_hXmmsLib = kvi_library_load("/usr/lib/libxmms.so.1");
				if(!g_hXmmsLib)
				{
					g_hXmmsLib = kvi_library_load("/usr/local/lib/libxmms.so");
					if(!g_hXmmsLib)
					{
						g_hXmmsLib = kvi_library_load("/usr/local/lib/libxmms.so.1");
					}
				}
			}
		}
	}
	return g_hXmmsLib;
}

static inline void * lookup_xmms_symbol(KviXmmsInterface * i,const char * sym)
{
	if(!load_xmms_lib())
	{
		i->setLastError(__tr2qs_ctx("Can't load the xmms library (libxmms.so)","mediaplayer"));
		return 0;
	}
	void * symptr = kvi_library_symbol(g_hXmmsLib,sym);
	if(!symptr)
	{
		QString tmp;
		KviQString::sprintf(tmp,__tr2qs_ctx("Can't find symbol %s in libxmms.so","mediaplayer"),sym);
		i->setLastError(tmp);
	}
	return symptr;
}



KviXmmsInterface::KviXmmsInterface()
: KviMediaPlayerInterface()
{
}

KviXmmsInterface::~KviXmmsInterface()
{
	if(g_hXmmsLib)
	{
		kvi_library_close(g_hXmmsLib);
		g_hXmmsLib = 0;
	}
}

int KviXmmsInterface::detect(bool bStart)
{
	void * sym = lookup_xmms_symbol(this,"xmms_remote_play");
	return sym ? 80 : 0;
}

#define XMMS_SIMPLE_CALL(__symname) \
	void (*sym)(int) = (void (*)(int))lookup_xmms_symbol(this,__symname); \
	if(!sym)return false; \
	sym(0); \
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

bool KviXmmsInterface::jumpTo(int &iPos)
{
	void (*sym)(int,int) = (void (*)(int,int))lookup_xmms_symbol(this,"xmms_remote_jump_to_time");
	if(!sym)return false;
	sym(0,iPos);
	return true;
}

bool KviXmmsInterface::setVol(int &iVol)
{
	void (*sym)(int,int) = (void (*)(int,int))lookup_xmms_symbol(this,"xmms_remote_set_main_volume");
	if(!sym)return false;
	sym(0,100*iVol/255);
	return true;
}

int KviXmmsInterface::getVol()
{
	int (*sym)(int) = (int (*)(int))lookup_xmms_symbol(this,"xmms_remote_get_main_volume");
	if(!sym)return -1;
	int iVol = sym(0);
	return iVol * 255 /100;
}

bool KviXmmsInterface::getRepeat()
{
	bool (*sym)(int) = (bool (*)(int))lookup_xmms_symbol(this,"xmms_remote_is_repeat");
	if(!sym)return false;
	bool ret = sym(0);
	return ret;
}

bool KviXmmsInterface::setRepeat(bool &bVal)
{
	bool (*sym1)(int) = (bool (*)(int))lookup_xmms_symbol(this,"xmms_remote_is_repeat");
	if(!sym1)return false;
	bool bNow = sym1(0);
	if(bNow != bVal)
	{
		void (*sym2)(int) = (void (*)(int))lookup_xmms_symbol(this,"xmms_remote_toggle_repeat");
		if(!sym2)return false;
		sym2(0);
	}
	return true;
}

bool KviXmmsInterface::getShuffle()
{
	bool (*sym)(int) = (bool (*)(int))lookup_xmms_symbol(this,"xmms_remote_is_shuffle");
	if(!sym)return false;
	bool ret = sym(0);
	return ret;
}

bool KviXmmsInterface::setShuffle(bool &bVal)
{
	bool (*sym1)(int) = (bool (*)(int))lookup_xmms_symbol(this,"xmms_remote_is_shuffle");
	if(!sym1)return false;
	bool bNow = sym1(0);
	if(bNow != bVal)
	{
		void (*sym2)(int) = (void (*)(int))lookup_xmms_symbol(this,"xmms_remote_toggle_shuffle");
		if(!sym2)return false;
		sym2(0);
	}
	return true;
}

KviMediaPlayerInterface::PlayerStatus KviXmmsInterface::status()
{
	bool (*sym1)(int) = (bool (*)(int))lookup_xmms_symbol(this,"xmms_remote_is_paused");
	if(sym1)
	{
		if(sym1(0))return KviMediaPlayerInterface::Paused;
		bool (*sym2)(int) = (bool (*)(int))lookup_xmms_symbol(this,"xmms_remote_is_playing");
		if(sym2)
		{
			if(sym2(0))return KviMediaPlayerInterface::Playing;
			else return KviMediaPlayerInterface::Stopped;
		}
	}

	return KviMediaPlayerInterface::Unknown;
}

bool KviXmmsInterface::playMrl(const QString &mrl)
{
	void (*sym)(int,char *) = (void (*)(int,char *))lookup_xmms_symbol(this,"xmms_remote_playlist_add_url_string");
	QCString tmp = mrl.local8Bit();
	if(!tmp.isEmpty())
	{
		if(sym)
		{
			sym(0,tmp.data());
			int (*sym1)(int) = (int (*)(int))lookup_xmms_symbol(this,"xmms_remote_get_playlist_length");
			if(sym1)
			{
				int len = sym1(0);
				if(len > 0)
				{
					void (*sym2)(int,int) = (void (*)(int,int))lookup_xmms_symbol(this,"xmms_remote_set_playlist_pos");
					if(sym2)
					{
						sym2(0,len - 1);
					} else return false;
				} else return false;
			} else return false;
		} else return false;
	}
	return true;
}

QString KviXmmsInterface::nowPlaying()
{
	int (*sym)(int) = (int (*)(int))lookup_xmms_symbol(this,"xmms_remote_get_playlist_pos");
	if(!sym)return QString::null;
	int pos = sym(0);
	char * (*sym2)(int,int) = (char * (*)(int,int))lookup_xmms_symbol(this,"xmms_remote_get_playlist_title");
	if(!sym2)return QString::null;
	return QString::fromLocal8Bit(sym2(0,pos));
}

QString KviXmmsInterface::mrl()
{
	int (*sym)(int) = (int (*)(int))lookup_xmms_symbol(this,"xmms_remote_get_playlist_pos");
	if(!sym)return QString::null;
	int pos = sym(0);
	char * (*sym2)(int,int) = (char * (*)(int,int))lookup_xmms_symbol(this,"xmms_remote_get_playlist_file");
	if(!sym2)return QString::null;
	QString ret = QString::fromLocal8Bit(sym2(0,pos));
	if(ret.length() > 1)
		if(ret[0] == '/')ret.prepend("file://");
	return ret;
}

int KviXmmsInterface::position()
{
	int (*sym)(int) = (int (*)(int))lookup_xmms_symbol(this,"xmms_remote_get_playlist_pos");
	if(!sym)return -1;
	int pos = sym(0);
	int (*sym2)(int,int) = (int (*)(int,int))lookup_xmms_symbol(this,"xmms_remote_get_output_time");
	if(!sym2)return -1;
	return sym2(0,pos);
}

int KviXmmsInterface::length()
{
	int (*sym)(int) = (int (*)(int))lookup_xmms_symbol(this,"xmms_remote_get_playlist_pos");
	if(!sym)return -1;
	int pos = sym(0);
	int (*sym2)(int,int) = (int (*)(int,int))lookup_xmms_symbol(this,"xmms_remote_get_playlist_time");
	if(!sym2)return -1;
	return sym2(0,pos);
}

int KviXmmsInterface::getPlayListPos()
{
	int (*sym)(int) = (int (*)(int))lookup_xmms_symbol(this,"xmms_remote_get_playlist_pos");
	if(!sym)return -1;
	return sym(0);
}



#endif //!COMPILE_ON_WINDOWS
