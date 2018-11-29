#ifndef _OPTW_SOUND_H_
#define _OPTW_SOUND_H_
//=============================================================================
//
//   File : OptionsWidget_sound.h
//   Creation date : Fri Sep  6 02:18:21 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviOptionsWidget.h"

#include <QComboBox>
#include <QPushButton>

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_sound KviIconManager::Sound
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_sound __tr2qs_no_lookup("Sound")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_sound OptionsWidget_general
#define KVI_OPTIONS_WIDGET_CONTAINER_OptionsWidget_sound true

class OptionsWidget_sound : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_sound(QWidget * parent);
	~OptionsWidget_sound();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_soundGeneral KviIconManager::Sound
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_soundGeneral __tr2qs_no_lookup("Sound System")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_soundGeneral OptionsWidget_sound
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_soundGeneral __tr2qs_no_lookup("sound,xmms,audio,play,amarok")
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_soundGeneral 70000

class OptionsWidget_soundGeneral : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_soundGeneral(QWidget * parent);
	~OptionsWidget_soundGeneral();

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
	void commit() override;
	void showEvent(QShowEvent * e) override;
protected slots:
	void soundTest();
	void soundAutoDetect();
	void mediaTest();
	void mediaAutoDetect();
};

#define KVI_OPTIONS_WIDGET_ICON_OptionsWidget_sounds KviIconManager::Sound
#define KVI_OPTIONS_WIDGET_NAME_OptionsWidget_sounds __tr2qs_no_lookup("Sounds")
#define KVI_OPTIONS_WIDGET_PARENT_OptionsWidget_sounds OptionsWidget_sound
#define KVI_OPTIONS_WIDGET_KEYWORDS_OptionsWidget_sounds __tr2qs_no_lookup("sound,events")
#define KVI_OPTIONS_WIDGET_PRIORITY_OptionsWidget_sounds 80000

class OptionsWidget_sounds : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsWidget_sounds(QWidget * parent);
	~OptionsWidget_sounds();
};

#endif //_OPTW_SOUND_H_
