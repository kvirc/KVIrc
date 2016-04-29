#ifndef _MP_XMMSINTERFACE_H_
#define _MP_XMMSINTERFACE_H_
//=============================================================================
//
//   File : MpXmmsInterface.h
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

#include "MpInterface.h"

#include "kvi_settings.h"

#include <QLibrary>

#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
class KviXmmsInterface : public MpInterface
{
public:
	KviXmmsInterface();
	virtual ~KviXmmsInterface();

protected:
	QLibrary * m_pPlayerLibrary;
	QString m_szPlayerLibraryName;
	const char ** m_pLibraryPaths;

public:
	virtual int detect(bool bStart);
	virtual bool prev();
	virtual bool next();
	virtual bool play();
	virtual bool stop();
	virtual bool pause();
	virtual bool quit();
	virtual bool jumpTo(kvs_int_t & iPos);
	virtual bool setVol(kvs_int_t & iVol);
	virtual int getVol();
	virtual MpInterface::PlayerStatus status();
	virtual QString nowPlaying();
	virtual bool playMrl(const QString & mrl);
	virtual QString mrl();
	virtual int getPlayListPos();
	virtual int position();
	virtual int length();
	virtual bool getRepeat();
	virtual bool getShuffle();
	virtual bool setRepeat(bool & bVal);
	virtual bool setShuffle(bool & bVal);

protected:
	bool loadPlayerLibrary();
	void * lookupSymbol(const char * szSymbolName);
};

MP_DECLARE_DESCRIPTOR(KviXmmsInterface)

class KviAudaciousClassicInterface : public KviXmmsInterface
{
public:
	KviAudaciousClassicInterface();
	virtual ~KviAudaciousClassicInterface();
};

MP_DECLARE_DESCRIPTOR(KviAudaciousClassicInterface)

#endif //COMPILE_ON_WINDOWS

#endif //_MP_XMMSINTERFACE_H_
