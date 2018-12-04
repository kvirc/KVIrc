#ifndef _MP_INTERFACE_H_
#define _MP_INTERFACE_H_
//=============================================================================
//
//   File : MpInterface.h
//   Creation date : Fri Mar 25 20:01:25 2005 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"
#include "KviQString.h"
#include "KviKvsTypes.h"

class MpInterface
{
public:
	// don't do any initialization in the constructor
	// implement lazy initialization in each function instead
	MpInterface(){};
	virtual ~MpInterface(){};

protected:
	QString m_szLastError;

public:
	const QString & lastError() const { return m_szLastError; };

	//
	// mandatory interface
	//

	// This should attempt to detect if the player is available on the user's system
	// and returning a score from 0 to 100 depending on how likely the player will work
	// and how many of its functions are available. 100 means that the interface
	// is absolutely sure that the player will start and play stuff.
	// If bStart is true then the function is allowed to explicitly start the player,
	// otherwise a startup attempt should not be made and different methods should be used.
	// This function is used for auto-detection and is called twice: the first time
	// with bStart set to false to determine the players that potentially could run.
	// If no player returns a reasonably high value by just guessing, then
	// a second loop may be attempted with the "bStart" parameter set to true.
	// A level of 0 is considered total failure: in this case detect() should
	// also set the last error to a descriptive string.
	virtual int detect(bool bStart) = 0;

	// play previous media, return false only on communication failure
	virtual bool prev() = 0;
	// play next media, return false only on communication failure
	virtual bool next() = 0;
	// start playback now, return false only on communication failure (i.e. return true when already playing)
	virtual bool play() = 0;
	// stop playback now, return false only on communication failure (i.e. return true when already stopped)
	virtual bool stop() = 0;
	// pause playback now (do NOT toggle pause, just pause), return false only on communication failure
	virtual bool pause() = 0;

	// current media related

	// currently played media: it should include AT least the title
	// but may also include other information.
	// this string MUST be non-empty when the player is playing something.
	// If it is empty then either the player is not playing or there are
	// communication errors.
	virtual QString nowPlaying() = 0;
	// currently played media resource locator
	// file://<filepath> for local files, dvb:// for dvb media, dvd:// for dvd's
	// http:// for audio streams etc...
	// empty if player is not playing (or in the tragic case that the player
	// can't determine the url).
	virtual QString mrl() = 0;

	// optional interface

	// this should play the specified mrl
	// the mrl may be (or may be not) added to the player's playlist
	// the function should return false if the player doesn't support
	// this function or there is a communication error
	virtual bool playMrl(const QString & mrl);
	// what is this ? :D
	virtual bool amipExec(const QString & cmd);
	virtual QString amipEval(const QString & cmd);
	// this is functions to hide,show and minimize the player interface
	virtual bool hide();
	virtual bool show();
	virtual bool minimize();
	// set the volume of mediaplayer (0-255)
	virtual bool setVol(kvs_int_t & iVol);
	// get the pvolume value(0-255)
	virtual int getVol();
	//mute the volume
	virtual bool mute();
	// should quit the player if it's running
	// return false only on communication failure
	virtual bool quit();
	// return the current player status
	enum PlayerStatus
	{
		Unknown,
		Stopped,
		Playing,
		Paused
	};
	virtual MpInterface::PlayerStatus status();
	// current position in the media (msecs)
	// 0 if the player isn't playing anything and -1 if unknown
	virtual int position();
	// total length of the media (msecs)
	// 0 if the player isn't playing anything and -1 if unknown (e.g. a stream)
	virtual int length();
	// jump to position
	virtual bool jumpTo(kvs_int_t & iPos);
	// interface with a default implementation for certain types of media (read for mp3)
	// reimplement only if the player knows better

	// currently played media title (it's player choice if the title
	// is to be derived from the media file name or from the information
	// stored inside the file like the ID3 tag...)
	// If the player is not playing, the returned string should be empty
	virtual QString title();
	// currently played media artist's name
	// If the player is not playing, the returned string should be empty
	// If the player can't guess the artist it should return the string "unknown"
	virtual QString artist();
	// currently plaued media genre
	// If the player is not playing, the returned string should be empty
	// If the player can't guess the genre it should return the string "unknown"
	virtual QString genre();
	// currently played media comment.
	// Empty string if the player isn't playing anything or there is no comment
	virtual QString comment();
	// currently played media year
	// Empty string if the player isn't playing anything or the year is unknown
	virtual QString year();
	// currently played media album
	// Empty string if the player isn't playing anything
	// If the player can't guess the album/collection then this string should be "unknown"
	virtual QString album();
	// bit rate in bits/sec, 0 if unknown
	virtual int bitRate();
	// sample rate in samples/sec (Hz), 0 if unknown
	virtual int sampleRate();
	// number of channels
	virtual int channels();
	// frequency
	// the type of the media (MPEG Layer 3, MPEG Layer 4, OGG Vorbis, AVI Stream etc..)
	virtual QString mediaType();
	// get the position in the playlist
	virtual int getPlayListPos();
	// set the position in the playlist
	virtual bool setPlayListPos(kvs_int_t & iPos);
	// return the list's length
	virtual int getListLength();
	// return the Eq(number) value
	virtual int getEqData(kvs_int_t & i_val);
	// set the  Eq(iPos) to Eq(iVal) value
	virtual bool setEqData(kvs_int_t & iPos, kvs_int_t & iVal);
	// get the   Repeat bool value
	virtual bool getRepeat();
	// get the  shuffle bool value
	virtual bool getShuffle();
	// set the   Repeat bool value
	virtual bool setRepeat(bool & bVal);
	// set the  shuffle bool value
	virtual bool setShuffle(bool & bVal);
	void setLastError(const QString & szLastError) { m_szLastError = szLastError; };
protected:
	void notImplemented();
	QString getLocalFile();
};

class MpInterfaceDescriptor
{
public:
	MpInterfaceDescriptor(){};
	virtual ~MpInterfaceDescriptor(){};

public:
	virtual const QString & name() = 0;
	virtual const QString & description() = 0;
	virtual MpInterface * instance() = 0;
};

#define MP_DECLARE_DESCRIPTOR(_interfaceclass)                       \
	class _interfaceclass##Descriptor : public MpInterfaceDescriptor \
	{                                                                \
	public:                                                          \
		_interfaceclass##Descriptor();                               \
		virtual ~_interfaceclass##Descriptor();                      \
                                                                     \
	protected:                                                       \
		_interfaceclass * m_pInstance;                               \
		QString m_szName;                                            \
		QString m_szDescription;                                     \
                                                                     \
	public:                                                          \
		virtual const QString & name();                              \
		virtual const QString & description();                       \
		virtual MpInterface * instance();                            \
	};

#define MP_IMPLEMENT_DESCRIPTOR(_interfaceclass, _name, _description) \
	_interfaceclass##Descriptor::_interfaceclass##Descriptor()        \
	    : MpInterfaceDescriptor()                                     \
	{                                                                 \
		m_pInstance = nullptr;                                        \
		m_szName = _name;                                             \
		m_szDescription = _description;                               \
	}                                                                 \
	_interfaceclass##Descriptor::~_interfaceclass##Descriptor()       \
	{                                                                 \
		if(m_pInstance)                                               \
			delete m_pInstance;                                       \
	}                                                                 \
	const QString & _interfaceclass##Descriptor::name()               \
	{                                                                 \
		return m_szName;                                              \
	}                                                                 \
	const QString & _interfaceclass##Descriptor::description()        \
	{                                                                 \
		return m_szDescription;                                       \
	}                                                                 \
	MpInterface * _interfaceclass##Descriptor::instance()             \
	{                                                                 \
		if(!m_pInstance)                                              \
			m_pInstance = new _interfaceclass();                      \
		return m_pInstance;                                           \
	}

#define MP_CREATE_DESCRIPTOR(_interfaceclass) \
	new _interfaceclass##Descriptor()

#endif //!_MP_INTERFACE_H_
