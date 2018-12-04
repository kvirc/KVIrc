//=============================================================================
//
//   File : MpAmipInterface.cpp
//   Creation date : Sun 27 Mar 2005 16:55:09 by Szymon Stefanek
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

#include "MpAmipInterface.h"
#include "KviOptions.h"

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)

#include <QTextCodec>

#include "KviLocale.h"
#include "KviModule.h"

enum ac_StartMode
{
	AC_START_ALL = 0,
	AC_START_CLIENT,
	AC_START_SERVER,
	AC_START_NONE
};

enum ac_ErrorCode
{
	AC_ERR_NOERROR = 0,
	AC_ERR_CLIENTISNULL,
	AC_ERR_EXCEPTION,
	AC_ERR_CONNECTIONFAILED,
	AC_ERR_SERVERNOTRUNNING
};

#define AC_BUFFER_SIZE 2048

static HINSTANCE amip_dll = nullptr;

#define MP_AC_DYNPTR(__rettype, __func, __args)        \
	typedef __rettype(CALLBACK * lp_##__func)(__args); \
	lp_##__func __func

#define MP_AC_FUNC(__func)                                   \
	__func = (lp_##__func)GetProcAddress(amip_dll, #__func); \
	if(!__func)                                              \
	{                                                        \
		FreeLibrary(amip_dll);                               \
		return false;                                        \
	}

#define COMMA() ,

MP_AC_DYNPTR(void, ac_init, int mode);
MP_AC_DYNPTR(void, ac_uninit, void);
MP_AC_DYNPTR(void, ac_getDestHost, char * out);
MP_AC_DYNPTR(int, ac_getDestPort, void);
MP_AC_DYNPTR(bool, ac_pingServer, const char * host COMMA() int port COMMA() int timeout);
MP_AC_DYNPTR(int, ac_exec, const char * cmd);
MP_AC_DYNPTR(int, ac_eval, const char * cmd COMMA() char * result);

static bool loadAmipDll()
{
	amip_dll = LoadLibrary(TEXT("ac.dll"));
	if(!amip_dll)
		return false;

	MP_AC_FUNC(ac_init);
	MP_AC_FUNC(ac_uninit);
	MP_AC_FUNC(ac_getDestHost);
	MP_AC_FUNC(ac_getDestPort);
	MP_AC_FUNC(ac_pingServer);
	MP_AC_FUNC(ac_eval);
	MP_AC_FUNC(ac_exec);

	return true;
};

static QTextCodec * mediaplayer_get_codec()
{
	QTextCodec * pCodec = nullptr;
	pCodec = QTextCodec::codecForName(KVI_OPTION_STRING(KviOption_stringWinampTextEncoding).toUtf8());

	if(!pCodec)
		pCodec = QTextCodec::codecForLocale();
	return pCodec;
}

MP_IMPLEMENT_DESCRIPTOR(
    MpAmipInterface,
    "amip",
    __tr2qs_ctx(
        "An interface for the AMIP plugin.\n"
        "You can download it from http://amip.tools-for.net\n"
        "To use this interface you must "
        "install AMIP plugin for your player.",
        "mediaplayer"))

MpAmipInterface::MpAmipInterface()
    : MpInterface()
{
	if(!amip_dll)
	{
		bool res = loadAmipDll();
		if(!res)
		{
			amip_dll = nullptr;
			return;
		}
		ac_init(AC_START_CLIENT);
	}
}

MpAmipInterface::~MpAmipInterface()
{
	if(!amip_dll)
		return;
	ac_uninit();
	FreeLibrary(amip_dll);
	amip_dll = nullptr;
}

int MpAmipInterface::detect(bool bStart)
{
	if(!amip_dll)
		return 0;
	char host[AC_BUFFER_SIZE];
	ac_getDestHost(host);
	if(ac_pingServer(host, ac_getDestPort(), 5000))
		return 99;
	return 1;
}

#define MP_AMIP_COMMAND(__cmdname, __acmd)          \
	bool MpAmipInterface::__cmdname()               \
	{                                               \
		return (ac_exec(__acmd) == AC_ERR_NOERROR); \
	}

MP_AMIP_COMMAND(play, "control play")
MP_AMIP_COMMAND(stop, "control stop")
MP_AMIP_COMMAND(next, "control >")
MP_AMIP_COMMAND(prev, "control <")
MP_AMIP_COMMAND(pause, "control pause")
MP_AMIP_COMMAND(quit, "control exit")

// helper function for evaluating variables returning integers
int eval_int(const char * var)
{
	if(!amip_dll)
		return -1;
	char buff[AC_BUFFER_SIZE];
	int res = -1;
	if(AC_ERR_NOERROR == ac_eval(var, buff))
		res = atoi(buff);
	return res;
}

QString eval_str(const char * var)
{
	QString res;
	if(!amip_dll)
		return res;
	char buff[AC_BUFFER_SIZE];
	if(AC_ERR_NOERROR == ac_eval(var, buff))
		res.append(buff);
	return res;
}

int MpAmipInterface::length()
{
	return eval_int("var_sl") * 1000;
}

int MpAmipInterface::position()
{
	return eval_int("var_psec") * 1000;
}

int MpAmipInterface::bitRate()
{
	return eval_int("var_br");
}

int MpAmipInterface::sampleRate()
{
	return eval_int("var_sr");
}

int MpAmipInterface::channels()
{
	int ret = -1;
	if(eval_str("var_typ").startsWith("Stereo"))
		ret = 2;
	else if(eval_str("var_typ").startsWith("Mono"))
		ret = 1;
	return ret;
}

MpInterface::PlayerStatus MpAmipInterface::status()
{
	int ret = eval_int("var_stat");
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

QString MpAmipInterface::mrl()
{
	QString szRet;
	QString szFn = eval_str("var_fn");
	QTextCodec * pCodec = mediaplayer_get_codec();
	if(pCodec)
	{
		szRet = pCodec->toUnicode(szFn.toUtf8());
	}
	else
	{
		szRet = szFn;
	}

	if(!szRet.startsWith("http://", Qt::CaseInsensitive))
		szRet.prepend("file://");

	return szRet;
}

QString getAmipString(const char * var)
{
	QString szRet;
	QString szString = eval_str(var);
	QTextCodec * pCodec = mediaplayer_get_codec();
	if(pCodec)
	{
		szRet = pCodec->toUnicode(szString.toUtf8());
	}
	else
	{
		szRet = szString;
	}
	return szRet;
}

QString MpAmipInterface::nowPlaying()
{
	return getAmipString("var_s");
}

QString MpAmipInterface::artist()
{
	return getAmipString("var_1");
}

QString MpAmipInterface::title()
{
	return getAmipString("var_2");
}

QString MpAmipInterface::album()
{
	return getAmipString("var_4");
}

QString MpAmipInterface::year()
{
	return getAmipString("var_5");
}

QString MpAmipInterface::comment()
{
	return getAmipString("var_6");
}

QString MpAmipInterface::genre()
{
	return getAmipString("var_7");
}

bool MpAmipInterface::setVol(kvs_int_t & iVol)
{
	if(!amip_dll)
		return false;
	char volcmd[AC_BUFFER_SIZE];
	sprintf(volcmd, "control vol %d", iVol);
	return (ac_exec(volcmd) == AC_ERR_NOERROR);
}

int MpAmipInterface::getVol()
{
	return eval_int("var_vol");
}

bool MpAmipInterface::jumpTo(kvs_int_t & iPos)
{
	if(!amip_dll)
		return false;
	char jmpcmd[AC_BUFFER_SIZE];
	sprintf(jmpcmd, "jumptotime %d", iPos / 1000);
	return (ac_exec(jmpcmd) == AC_ERR_NOERROR);
}
bool MpAmipInterface::hide()
{
	HWND hWinamp = (HWND)eval_int("var_phwnd");
	if(hWinamp && hWinamp != (HWND)-1)
	{
		ShowWindow(hWinamp, SW_HIDE);
		return true;
	}
	return false;
}

bool MpAmipInterface::show()
{
	HWND hWinamp = (HWND)eval_int("var_phwnd");
	if(hWinamp && hWinamp != (HWND)-1)
	{
		ShowWindow(hWinamp, SW_SHOW);
		return true;
	}
	return false;
}

bool MpAmipInterface::minimize()
{
	if(!amip_dll)
		return false;
	return (ac_exec("control mimimize") == AC_ERR_NOERROR);
}

bool MpAmipInterface::setPlayListPos(kvs_int_t & iPos)
{
	if(!amip_dll)
		return false;
	char jmpcmd[AC_BUFFER_SIZE];
	sprintf(jmpcmd, "setplpos %d", iPos + 1);
	return (ac_exec(jmpcmd) == AC_ERR_NOERROR);
}

int MpAmipInterface::getPlayListPos()
{
	return eval_int("var_pos");
}

int MpAmipInterface::getListLength()
{
	return eval_int("var_ll");
}

bool MpAmipInterface::getRepeat()
{
	return eval_str("var_repeat").startsWith("on");
}

bool MpAmipInterface::getShuffle()
{
	return eval_str("var_shuffle").startsWith("on");
}

bool MpAmipInterface::setShuffle(bool & bVal)
{
	if(!amip_dll)
		return false;
	bool res;
	if(bVal)
		res = (ac_exec("setshuffle on") == AC_ERR_NOERROR);
	else
		res = (ac_exec("setshuffle off") == AC_ERR_NOERROR);
	return res;
}

bool MpAmipInterface::setRepeat(bool & bVal)
{
	if(!amip_dll)
		return false;
	bool res;
	if(bVal)
		res = (ac_exec("setrepeat on") == AC_ERR_NOERROR);
	else
		res = (ac_exec("setrepeat off") == AC_ERR_NOERROR);
	return res;
}

bool MpAmipInterface::amipExec(const QString & cmd)
{
	if(!amip_dll)
		return false;
	QTextCodec * c = mediaplayer_get_codec();
	KviCString szCmd = c ? c->fromUnicode(cmd) : cmd.toUtf8();
	return (ac_exec(szCmd) == AC_ERR_NOERROR);
}

QString MpAmipInterface::amipEval(const QString & cmd)
{
	QString ret;
	if(!amip_dll)
		return ret;
	QTextCodec * c = mediaplayer_get_codec();
	KviCString szCmd = c ? c->fromUnicode(cmd) : cmd.toUtf8();
	char buff[AC_BUFFER_SIZE];
	if((ac_eval(szCmd, buff) == AC_ERR_NOERROR))
	{
		QString s = buff;
		QTextCodec * c = mediaplayer_get_codec();
		if(c)
			ret = c->toUnicode(s.toLatin1());
		else
			ret = s;
	}
	return ret;
}

#endif //COMPILE_ON_WINDOWS
