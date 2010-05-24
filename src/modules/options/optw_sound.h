#ifndef _OPTW_SOUND_H_
#define _OPTW_SOUND_H_
//=============================================================================
//
//   File : optw_sound.h
//   Creation date : Fri Sep  6 02:18:21 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_optionswidget.h"

#include <QComboBox>
#include <QPushButton>

#define KVI_OPTIONS_WIDGET_ICON_KviSoundOptionsWidget KVI_SMALLICON_SOUND
#define KVI_OPTIONS_WIDGET_NAME_KviSoundOptionsWidget __tr2qs_no_lookup("Sound")
#define KVI_OPTIONS_WIDGET_PARENT_KviSoundOptionsWidget KviGeneralOptOptionsWidget
#define KVI_OPTIONS_WIDGET_CONTAINER_KviSoundOptionsWidget true


class KviSoundOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviSoundOptionsWidget(QWidget * parent);
	~KviSoundOptionsWidget();
};

#define KVI_OPTIONS_WIDGET_ICON_KviSoundGeneralOptionsWidget KVI_SMALLICON_SOUND
#define KVI_OPTIONS_WIDGET_NAME_KviSoundGeneralOptionsWidget __tr2qs_no_lookup("Sound System")
#define KVI_OPTIONS_WIDGET_PARENT_KviSoundGeneralOptionsWidget KviSoundOptionsWidget
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviSoundGeneralOptionsWidget __tr2qs_no_lookup("sound,xmms,audio,play,amarok")
#define KVI_OPTIONS_WIDGET_PRIORITY_KviSoundGeneralOptionsWidget 70000

class KviSoundGeneralOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviSoundGeneralOptionsWidget(QWidget * parent);
	~KviSoundGeneralOptionsWidget();
protected:
	QComboBox * m_pSoundSystemBox;
	QPushButton * m_pSoundTestButton;
	QPushButton * m_pSoundAutoDetectButton;
	QComboBox * m_pMediaPlayerBox;
	QPushButton * m_pMediaTestButton;
	QPushButton * m_pMediaAutoDetectButton;

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	QComboBox * m_pWinampEncodingCombo;
#endif
	QComboBox * m_pTagsEncodingCombo;
	bool m_bFirstShow;
protected:
	void soundFillBox();
	void mediaFillBox();
	virtual void commit();
	virtual void showEvent(QShowEvent *e);
protected slots:
	void soundTest();
	void soundAutoDetect();
	void mediaTest();
	void mediaAutoDetect();
};

#define KVI_OPTIONS_WIDGET_ICON_KviSoundsOptionsWidget KVI_SMALLICON_SOUND
#define KVI_OPTIONS_WIDGET_NAME_KviSoundsOptionsWidget __tr2qs_no_lookup("Sounds")
#define KVI_OPTIONS_WIDGET_PARENT_KviSoundsOptionsWidget KviSoundOptionsWidget
#define KVI_OPTIONS_WIDGET_KEYWORDS_KviSoundsOptionsWidget __tr2qs_no_lookup("sound,events")
#define KVI_OPTIONS_WIDGET_PRIORITY_KviSoundsOptionsWidget 80000

class KviSoundsOptionsWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviSoundsOptionsWidget(QWidget * parent);
	~KviSoundsOptionsWidget();
};

#endif //_OPTW_SOUND_H_
