//=============================================================================
//
//   File : MpWinampInterface.cpp
//   Creation date : Mon 28 Mar 2005 23:41:50 by Szymon Stefanek
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
//=============================================================================

#include "MpWinampInterface.h"
#include "KviOptions.h"

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)

#include <QTextCodec>

#include "KviLocale.h"
#include "KviModule.h"

#define IPC_STARTPLAY 102
#define IPC_ISPLAYING 104

#define IPC_GETOUTPUTTIME 105
//	** int res = SendMessage(hwnd_winamp,WM_WA_IPC,mode,IPC_GETOUTPUTTIME);
//	**
//	** IPC_GETOUTPUTTIME returns the position in milliseconds of the
//	** current song (mode = 0), or the song length, in seconds (mode = 1).
//	** Returns -1 if not playing or error.

#define IPC_JUMPTOTIME 106
// ** SendMessage(hwnd_winamp,WM_WA_IPC,ms,IPC_JUMPTOTIME);
// ** IPC_JUMPTOTIME sets the position in milliseconds of the
// ** current song (approximately).

#define IPC_SETPLAYLISTPOS 121
//	** SendMessage(hwnd_winamp,WM_WA_IPC,position,IPC_SETPLAYLISTPOS)
//	** IPC_SETPLAYLISTPOS sets the playlsit position to 'position'.

#define IPC_SETVOLUME 122
// ** SendMessage(hwnd_winamp,WM_WA_IPC,volume,IPC_SETVOLUME);
// ** IPC_SETVOLUME sets the volume of Winamp (from 0-255).

#define IPC_GETLISTLENGTH 124
//	** int length = SendMessage(hwnd_winamp,WM_WA_IPC,0,IPC_GETLISTLENGTH);
//	** IPC_GETLISTLENGTH returns the length of the current playlist, in
//	** tracks.

#define IPC_GETEQDATA 127
// ** int data=SendMessage(hwnd_winamp,WM_WA_IPC,pos,IPC_GETEQDATA);
// ** IPC_GETEQDATA queries the status of the EQ.
// ** The value returned depends on what 'pos' is set to:
// ** Value      Meaning
// ** ------------------
// ** 0-9        The 10 bands of EQ data. 0-63 (+20db - -20db)
// ** 10         The preamp value. 0-63 (+20db - -20db)
// ** 11         Enabled. zero if disabled, nonzero if enabled.
// ** 12         Autoload. zero if disabled, nonzero if enabled.

#define IPC_SETEQDATA 128
// ** SendMessage(hwnd_winamp,WM_WA_IPC,pos,IPC_GETEQDATA);
// ** SendMessage(hwnd_winamp,WM_WA_IPC,value,IPC_SETEQDATA);
// ** IPC_SETEQDATA sets the value of the last position retrieved
// ** by IPC_GETEQDATA. This is pretty lame, and we should provide
// ** an extended version that lets you do a MAKELPARAM(pos,value).
// ** someday...

#define IPC_GETPLAYLISTFILE 211
//	** (requires Winamp 2.04+, only usable from plug-ins (not external apps))
//	** char *name=SendMessage(hwnd_winamp,WM_WA_IPC,index,IPC_GETPLAYLISTFILE);
//	** IPC_GETPLAYLISTFILE gets the filename of the playlist entry [index].
//	** returns a pointer to it. returns nullptr on error.

#define IPC_GETPLAYLISTTITLE 212
//	* (requires Winamp 2.04+, only usable from plug-ins (not external apps))
//	** char *name=SendMessage(hwnd_winamp,WM_WA_IPC,index,IPC_GETPLAYLISTTITLE);
//	** IPC_GETPLAYLISTTITLE gets the title of the playlist entry [index].
//	** returns a pointer to it. returns nullptr on error.

#define IPC_GET_SHUFFLE 250
//	** val=SendMessage(hwnd_winamp,WM_WA_IPC,0,IPC_GET_SHUFFLE);
//	**
//	** IPC_GET_SHUFFLE returns the status of the Shuffle option (1 if set)

#define IPC_GET_REPEAT 251
//	** val=SendMessage(hwnd_winamp,WM_WA_IPC,0,IPC_GET_REPEAT);
//	**
//	** IPC_GET_REPEAT returns the status of the Repeat option (1 if set)

#define IPC_SET_SHUFFLE 252
//	** SendMessage(hwnd_winamp,WM_WA_IPC,value,IPC_SET_SHUFFLE);
//	**
//	** IPC_SET_SHUFFLE sets the status of the Shuffle option (1 to turn it on)

#define IPC_SET_REPEAT 253
//	** SendMessage(hwnd_winamp,WM_WA_IPC,value,IPC_SET_REPEAT);
//	**
//	** IPC_SET_REPEAT sets the status of the Repeat option (1 to turn it on)

#define IPC_GETLISTPOS 125
//	** int pos=SendMessage(hwnd_winamp,WM_WA_IPC,0,IPC_GETLISTPOS);
//	** IPC_GETLISTPOS returns the playlist position. A lot like IPC_WRITEPLAYLIST
//	** only faster since it doesn't have to write out the list. Heh, silly me.

#define IPC_GETINFO 126
//	** int inf=SendMessage(hwnd_winamp,WM_WA_IPC,mode,IPC_GETINFO);
//	** IPC_GETINFO returns info about the current playing song. The value
//	** it returns depends on the value of 'mode'.
//	** Mode      Meaning
//	** ------------------
//	** 0         Samplerate (i.e. 44100)
//	** 1         Bitrate  (i.e. 128)
//	** 2         Channels (i.e. 2)

#define IPC_PLAYFILE 100
//	** COPYDATASTRUCT cds;
//	** cds.dwData = IPC_PLAYFILE;
//	** cds.lpData = (void *) "file.mp3";
//	** cds.cbData = strlen((char *) cds.lpData)+1; // include space for null char
//	** SendMessage(hwnd_winamp,WM_COPYDATA,(WPARAM)nullptr,(LPARAM)&cds);
//	**
//	** This will play the file "file.mp3".

#define IPC_ISPLAYING 104
//	** int res = SendMessage(hwnd_winamp,WM_WA_IPC,0,IPC_ISPLAYING);
//	** IPC_ISPLAYING returns the status of playback.
//	** If it returns 1, it is playing. if it returns 3, it is paused,
//	** if it returns 0, it is not playing.

// The following stuff needs PostMessage(hwnd_winamp,WM_COMMAND,id,0);
#define WINAMP_BUTTON1 40044
#define WINAMP_BUTTON2 40045
#define WINAMP_BUTTON3 40046
#define WINAMP_BUTTON4 40047
#define WINAMP_BUTTON5 40048

#define WINAMP_CMD_PREV WINAMP_BUTTON1
#define WINAMP_CMD_PLAY WINAMP_BUTTON2
#define WINAMP_CMD_PAUSE WINAMP_BUTTON3
#define WINAMP_CMD_STOP WINAMP_BUTTON4
#define WINAMP_CMD_NEXT WINAMP_BUTTON5

#define WINAMP_CMD_QUIT 40001

#define KVIRC_WM_USER 63112
#define KVIRC_WM_USER_CHECK 13123
#define KVIRC_WM_USER_CHECK_REPLY 13124
#define KVIRC_WM_USER_GETTITLE 5000
#define KVIRC_WM_USER_GETFILE 10000
#define KVIRC_WM_USER_TRANSFER 15000

static QTextCodec * mediaplayer_get_codec()
{
	QTextCodec * pCodec = QTextCodec::codecForName(KVI_OPTION_STRING(KviOption_stringWinampTextEncoding).toUtf8());

	if(!pCodec)
		pCodec = QTextCodec::codecForLocale();
	return pCodec;
}

static HWND find_winamp(KviWinampInterface * i)
{
	HWND hWnd = FindWindow(TEXT("Winamp v1.x"), nullptr);
	if(!hWnd)
	{
		// try to start the process ?
		i->setLastError(__tr2qs_ctx("Can't find a running Winamp window", "mediaplayer"));
	}
	return hWnd;
}

MP_IMPLEMENT_DESCRIPTOR(
    KviWinampInterface,
    "winamp",
    __tr2qs_ctx(
        "An interface for the Winamp media player.\n"
        "You can download it from http://www.winamp.com.\n"
        "To use all the features of this interface you must "
        "copy the gen_kvirc.dll plugin found in the KVIrc "
        "distribution directory to the Winamp plugins folder "
        "and restart winamp.",
        "mediaplayer"))

KviWinampInterface::KviWinampInterface()
    : MpInterface()
{
}

KviWinampInterface::~KviWinampInterface()
{
}

int KviWinampInterface::detect(bool bStart)
{
	if(find_winamp(this))
		return 80;
	// FIXME : check for Programs Folder\Winamp\Winamp.exe ?
	// FIXME : if bStart try to start winamp.exe ?
	return 50;
}

#define MP_WINAMP_SENDMESSAGE(__cmdname, __wmmsg, __lparam, __wparam) \
	bool KviWinampInterface::__cmdname()                              \
	{                                                                 \
		HWND hWinamp = find_winamp(this);                             \
		if(hWinamp)                                                   \
			SendMessage(hWinamp, __wmmsg, __lparam, __wparam);        \
		return hWinamp != 0;                                          \
	}

#define MP_WINAMP_WM_USER(__cmdname, _ipcmsg) MP_WINAMP_SENDMESSAGE(__cmdname, WM_USER, 0, _ipcmsg)
#define MP_WINAMP_WM_COMMAND(__cmdname, _cmdmsg) MP_WINAMP_SENDMESSAGE(__cmdname, WM_COMMAND, _cmdmsg, 0)

MP_WINAMP_WM_USER(play, IPC_STARTPLAY)
MP_WINAMP_WM_COMMAND(stop, WINAMP_CMD_STOP)
MP_WINAMP_WM_COMMAND(next, WINAMP_CMD_NEXT)
MP_WINAMP_WM_COMMAND(prev, WINAMP_CMD_PREV)
MP_WINAMP_WM_COMMAND(pause, WINAMP_CMD_PAUSE)
MP_WINAMP_WM_COMMAND(quit, WINAMP_CMD_QUIT)

int KviWinampInterface::length()
{
	int leninsecs = -1;
	HWND hWinamp = find_winamp(this);
	if(hWinamp)
		leninsecs = SendMessage(hWinamp, WM_USER, 1, IPC_GETOUTPUTTIME);
	return leninsecs * 1000;
}

int KviWinampInterface::position()
{
	int leninmsecs = -1;
	HWND hWinamp = find_winamp(this);
	if(hWinamp)
		leninmsecs = SendMessage(hWinamp, WM_USER, 0, IPC_GETOUTPUTTIME);
	return leninmsecs;
}

int KviWinampInterface::bitRate()
{
	int ret = -1;
	HWND hWinamp = find_winamp(this);
	if(hWinamp)
		ret = SendMessage(hWinamp, WM_USER, 1, IPC_GETINFO);
	return ret;
}

int KviWinampInterface::sampleRate()
{
	int ret = -1;
	HWND hWinamp = find_winamp(this);
	if(hWinamp)
		ret = SendMessage(hWinamp, WM_USER, 0, IPC_GETINFO);
	return ret;
}

int KviWinampInterface::channels()
{
	int ret = -1;
	HWND hWinamp = find_winamp(this);
	if(hWinamp)
		ret = SendMessage(hWinamp, WM_USER, 2, IPC_GETINFO);
	return ret;
}

MpInterface::PlayerStatus KviWinampInterface::status()
{
	HWND hWinamp = find_winamp(this);
	int ret = 1000;
	if(hWinamp)
		ret = SendMessage(hWinamp, WM_USER, 0, IPC_ISPLAYING);
	switch(ret)
	{
		case 0:
			return MpInterface::Stopped;
			break;
		case 3:
			return MpInterface::Paused;
			break;
		case 1:
			return MpInterface::Playing;
			break;
		default:
			return MpInterface::Unknown;
			break;
	}
	return MpInterface::Unknown;
}

QString KviWinampInterface::mrl()
{
	QString ret;
	HWND hWinamp = find_winamp(this);
	if(hWinamp)
	{
		int ret2 = SendMessage(hWinamp, WM_USER, KVIRC_WM_USER, KVIRC_WM_USER_CHECK);

		if(ret2 != KVIRC_WM_USER_CHECK_REPLY)
		{
			setLastError(__tr2qs_ctx("The Winamp plugin has not been installed properly. Check /help mediaplayer.nowplaying", "mediaplayer"));
			return ret;
		}

		int len = SendMessage(hWinamp, WM_USER, KVIRC_WM_USER, KVIRC_WM_USER_GETFILE);

		if(len < 4096)
		{
			char szBuffer[4096];

			for(int i = 0; i < len; i++)
			{
				szBuffer[i] = SendMessage(hWinamp, WM_USER, KVIRC_WM_USER, KVIRC_WM_USER_TRANSFER + i);
			}
			szBuffer[len] = '\0';
			QTextCodec * c = mediaplayer_get_codec();
			if(c)
				ret = c->toUnicode(szBuffer);
			else
				ret = szBuffer;
			if(!ret.startsWith("http://", Qt::CaseInsensitive))
				ret.prepend("file://");
		}
	}
	return ret;
}

QString KviWinampInterface::nowPlaying()
{
	QString ret;
	HWND hWinamp = find_winamp(this);
	if(hWinamp)
	{
		int retpippo = SendMessage(hWinamp, WM_USER, KVIRC_WM_USER, KVIRC_WM_USER_CHECK);
		if(retpippo != KVIRC_WM_USER_CHECK_REPLY)
		{
			setLastError(__tr2qs_ctx("The Winamp plugin has not been installed properly. Check /help mediaplayer.nowplaying", "mediaplayer"));
			return ret;
		}

		int len = SendMessage(hWinamp, WM_USER, KVIRC_WM_USER, KVIRC_WM_USER_GETTITLE);

		if(len < 4096)
		{
			char szBuffer[4096];

			for(int i = 0; i < len; i++)
			{
				szBuffer[i] = SendMessage(hWinamp, WM_USER, KVIRC_WM_USER, KVIRC_WM_USER_TRANSFER + i);
			}
			szBuffer[len] = '\0';

			QTextCodec * c = mediaplayer_get_codec();
			if(c)
				ret = c->toUnicode(szBuffer);
			else
				ret = szBuffer;
		}
	}
	return ret;
}

bool KviWinampInterface::playMrl(const QString & mrl)
{
	HWND hWinamp = find_winamp(this);
	if(hWinamp)
	{
		QTextCodec * c = mediaplayer_get_codec();
		KviCString szMrl = c ? c->fromUnicode(mrl) : mrl.toUtf8();
		COPYDATASTRUCT cds;
		cds.dwData = IPC_PLAYFILE;
		cds.lpData = (void *)szMrl.ptr();
		cds.cbData = szMrl.len() + 1; // include space for null char
		SendMessage(hWinamp, WM_COPYDATA, (WPARAM)nullptr, (LPARAM)&cds);
		return true;
	}
	return false;
}

bool KviWinampInterface::setVol(kvs_int_t & iVol)
{
	HWND hWinamp = find_winamp(this);
	if(hWinamp)
	{
		SendMessage(hWinamp, WM_USER, iVol, IPC_SETVOLUME);
		return true;
	}
	return false;
}

int KviWinampInterface::getVol()
{
	int ret = -1;
	HWND hWinamp = find_winamp(this);
#if defined(COMPILE_ON_MINGW)
	if(hWinamp)
		ret = SendMessage(hWinamp, WM_USER, 666, IPC_SETVOLUME);
#else
	if(hWinamp)
		ret = SendMessage(hWinamp, WM_USER, -666, IPC_SETVOLUME);
#endif
	return ret;
}

bool KviWinampInterface::jumpTo(kvs_int_t & iPos)
{
	HWND hWinamp = find_winamp(this);
	if(hWinamp)
	{
		SendMessage(hWinamp, WM_USER, iPos, IPC_JUMPTOTIME);
		return true;
	}
	return false;
}
bool KviWinampInterface::hide()
{
	HWND hWinamp = find_winamp(this);
	HWND hWinampPE = FindWindow(TEXT("Winamp PE"), nullptr);       /*Playlist*/
	HWND hWinampEQ = FindWindow(TEXT("Winamp EQ"), nullptr);       /*Equalizer*/
	HWND hWinampMB = FindWindow(TEXT("Winamp MB"), nullptr);       /*MiniBrowser*/
	HWND hWinampGen = FindWindow(TEXT("Winamp Gen"), nullptr);     /*Library*/
	HWND hWinampVideo = FindWindow(TEXT("Winamp Video"), nullptr); /*Video*/
	if(hWinamp)
	{
		ShowWindow(hWinamp, SW_HIDE);
		if(hWinampPE || hWinampEQ || hWinampMB || hWinampGen || hWinampVideo)
		{
			if(hWinampPE)
				ShowWindow(hWinampPE, SW_HIDE);
			if(hWinampEQ)
				ShowWindow(hWinampEQ, SW_HIDE);
			if(hWinampMB)
				ShowWindow(hWinampMB, SW_HIDE);
			if(hWinampGen)
				ShowWindow(hWinampGen, SW_HIDE);
			if(hWinampVideo)
				ShowWindow(hWinampVideo, SW_HIDE);
			return true;
		}
		return true;
	}
	return false;
}

bool KviWinampInterface::show()
{
	HWND hWinamp = find_winamp(this);
	if(hWinamp)
	{
		ShowWindow(hWinamp, SW_SHOW);
		return true;
	}
	return false;
}

bool KviWinampInterface::minimize()
{
	HWND hWinamp = find_winamp(this);
	if(hWinamp)
	{
		ShowWindow(hWinamp, SW_SHOWMINIMIZED);
		return true;
	}
	return false;
}

bool KviWinampInterface::setPlayListPos(kvs_int_t & iPos)
{
	HWND hWinamp = find_winamp(this);
	if(hWinamp)
	{
		SendMessage(hWinamp, WM_USER, iPos, IPC_SETPLAYLISTPOS);
		return true;
	}
	return false;
}

int KviWinampInterface::getPlayListPos()
{
	int ret = -1;
	HWND hWinamp = find_winamp(this);
	if(hWinamp)
		ret = SendMessage(hWinamp, WM_USER, 2, IPC_GETLISTPOS);
	return ret;
}

int KviWinampInterface::getListLength()
{
	int ret = -1;
	HWND hWinamp = find_winamp(this);
	if(hWinamp)
		ret = SendMessage(hWinamp, WM_USER, 2, IPC_GETLISTLENGTH);
	return ret;
}

bool KviWinampInterface::setEqData(kvs_int_t & iPos, kvs_int_t & iVal)
{
	HWND hWinamp = find_winamp(this);
	if(hWinamp)
	{
		SendMessage(hWinamp, WM_USER, iPos, IPC_GETEQDATA);
		SendMessage(hWinamp, WM_USER, iVal, IPC_SETEQDATA);
		return true;
	}
	return false;
}

int KviWinampInterface::getEqData(kvs_int_t & ival)
{
	HWND hWinamp = find_winamp(this);
	int ret = -1;
	if(hWinamp)
	{
		ret = SendMessage(hWinamp, WM_USER, ival, IPC_GETEQDATA);
		return ret;
	}
	return ret;
}

bool KviWinampInterface::getRepeat()
{
	HWND hWinamp = find_winamp(this);
	if(hWinamp)
	{
		bool bRepeat = SendMessage(hWinamp, WM_USER, 0, IPC_GET_REPEAT);
		return bRepeat;
	}
	return false;
}

bool KviWinampInterface::getShuffle()
{
	HWND hWinamp = find_winamp(this);
	if(hWinamp)
	{
		bool bShuffle = SendMessage(hWinamp, WM_USER, 0, IPC_GET_SHUFFLE);
		return bShuffle;
	}
	return false;
}

bool KviWinampInterface::setShuffle(bool & bVal)
{
	HWND hWinamp = find_winamp(this);
	if(hWinamp)
	{
		bool bRepeat = SendMessage(hWinamp, WM_USER, bVal, IPC_SET_SHUFFLE);
		return bRepeat;
	}
	return false;
}

bool KviWinampInterface::setRepeat(bool & bVal)
{
	HWND hWinamp = find_winamp(this);
	if(hWinamp)
	{
		bool bShuffle = SendMessage(hWinamp, WM_USER, bVal, IPC_SET_REPEAT);
		return bShuffle;
	}
	return false;
}
#endif //COMPILE_ON_WINDOWS
