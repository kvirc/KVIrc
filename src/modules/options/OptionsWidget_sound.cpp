//=============================================================================
//
//   File : OptionsWidget_sound.cpp
//   Creation date : Fri Sep  6 02:18:23 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVsound sound client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvsound dot net)
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

#include "OptionsWidget_sound.h"

#include "kvi_settings.h"
#include "KviLocale.h"
#include "KviOptions.h"
#include "KviModuleManager.h"
#include "KviPointerList.h"
#include "KviCString.h"
#include "KviTalHBox.h"
#include "KviTalToolTip.h"
#include "KviApplication.h"

#include <QLabel>
#include <QPushButton>

// FIXME: This module doesn't ???? properly when auto-detection is performed!

OptionsWidget_sound::OptionsWidget_sound(QWidget * pParent)
    : KviOptionsWidget(pParent)
{
}

OptionsWidget_sound::~OptionsWidget_sound()
    = default;

OptionsWidget_soundGeneral::OptionsWidget_soundGeneral(QWidget * parent)
    : KviOptionsWidget(parent)
{
	m_bFirstShow = true;

	setObjectName("sound_system_options_widget");
	createLayout();

	KviTalGroupBox * g = addGroupBox(0, 0, 0, 0, Qt::Horizontal, __tr2qs_ctx("Sound System", "options"), true);

	KviTalToolTip::add(g, __tr2qs_ctx("This allows you to select the sound system to be used with KVIrc.", "options"));

	KviTalHBox * h = new KviTalHBox(g);

	m_pSoundSystemBox = new QComboBox(h);

	m_pSoundAutoDetectButton = new QPushButton(__tr2qs_ctx("Auto-detect", "options"), h);
	connect(m_pSoundAutoDetectButton, SIGNAL(clicked()), this, SLOT(soundAutoDetect()));

	m_pSoundTestButton = new QPushButton(__tr2qs_ctx("Test", "options"), h);
	connect(m_pSoundTestButton, SIGNAL(clicked()), this, SLOT(soundTest()));

	g = addGroupBox(0, 1, 0, 1, Qt::Horizontal, __tr2qs_ctx("Media Player", "options"), true);

	KviTalToolTip::add(g, __tr2qs_ctx("This allows you to select the preferred media player to be used with "
	                                  "the mediaplayer.* module commands and functions.",
	                          "options"));

	h = new KviTalHBox(g);

	m_pMediaPlayerBox = new QComboBox(h);

	m_pMediaAutoDetectButton = new QPushButton(__tr2qs_ctx("Auto-detect", "options"), h);
	connect(m_pMediaAutoDetectButton, SIGNAL(clicked()), this, SLOT(mediaAutoDetect()));

	m_pMediaTestButton = new QPushButton(__tr2qs_ctx("Test", "options"), h);
	connect(m_pMediaTestButton, SIGNAL(clicked()), this, SLOT(mediaTest()));

	m_pMediaTestButton->setEnabled(false);

	g = addGroupBox(0, 2, 0, 2, Qt::Horizontal, __tr2qs_ctx("ID3 Tags Encoding", "options"), true);

	KviTalToolTip::add(g, __tr2qs_ctx("This allows you to select encoding of mp3 tags.", "options"));

	h = new KviTalHBox(g);

	m_pTagsEncodingCombo = new QComboBox(h);
	m_pTagsEncodingCombo->addItem(__tr2qs_ctx("Use Language Encoding", "options"));

	int i = 0;
	int iMatch = 0;

	KviLocale::EncodingDescription * d = KviLocale::instance()->encodingDescription(i);
	while(d->pcName)
	{
		if(KviQString::equalCI(d->pcName, KVI_OPTION_STRING(KviOption_stringMp3TagsEncoding)))
			iMatch = i + 1;
		m_pTagsEncodingCombo->insertItem(m_pTagsEncodingCombo->count(), d->pcName);
		i++;
		d = KviLocale::instance()->encodingDescription(i);
	}
	m_pTagsEncodingCombo->setCurrentIndex(iMatch);

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	g = addGroupBox(0, 3, 0, 3, Qt::Horizontal, __tr2qs_ctx("Winamp messages encoding", "options"), true);

	KviTalToolTip::add(g, __tr2qs_ctx("This allows you to select encoding of winamp messages.", "options"));

	h = new KviTalHBox(g);

	m_pWinampEncodingCombo = new QComboBox(h);

	m_pWinampEncodingCombo->addItem(__tr2qs_ctx("Use Language Encoding", "options"));
	i = 0;
	iMatch = 0;

	d = KviLocale::instance()->encodingDescription(i);
	while(d->pcName)
	{
		if(KviQString::equalCI(d->pcName, KVI_OPTION_STRING(KviOption_stringWinampTextEncoding)))
			iMatch = i + 1;
		m_pWinampEncodingCombo->insertItem(m_pWinampEncodingCombo->count(), d->pcName);
		i++;
		d = KviLocale::instance()->encodingDescription(i);
	}
	m_pWinampEncodingCombo->setCurrentIndex(iMatch);

	addRowSpacer(0, 4, 0, 4);
#else
	addRowSpacer(0, 3, 0, 3);
#endif
}

OptionsWidget_soundGeneral::~OptionsWidget_soundGeneral()
    = default;

void OptionsWidget_soundGeneral::showEvent(QShowEvent *)
{
	if(!m_bFirstShow)
		return;
	// We fill these boxes only before the first show since the soundFillBox()
	// is likely to trigger sound system-detection which may take time...

	g_pApp->setOverrideCursor(Qt::WaitCursor);

	soundFillBox();
	mediaFillBox();

	g_pApp->restoreOverrideCursor();

	m_bFirstShow = false;
}

void OptionsWidget_soundGeneral::soundTest()
{
	QString szSavedSoundSystem = KVI_OPTION_STRING(KviOption_stringSoundSystem);
	KVI_OPTION_STRING(KviOption_stringSoundSystem) = m_pSoundSystemBox->currentText();

	KviModule * m = g_pModuleManager->getModule("snd");

	if(!m)
		return; // doh

	QString szFileName;

	if(!g_pApp->findAudioFile(szFileName, QString::fromUtf8("jingle.wav")))
	{
		qDebug("Can't find the jingle.wav file: was it shipped with your KVIrc installation?");
		return;
	}

	m->ctrl("play", &szFileName);

	KVI_OPTION_STRING(KviOption_stringSoundSystem) = szSavedSoundSystem;
}

void OptionsWidget_soundGeneral::mediaTest()
{
}

void OptionsWidget_soundGeneral::soundAutoDetect()
{
	KviModule * m = g_pModuleManager->getModule("snd");
	if(!m)
		return;

	g_pApp->setOverrideCursor(Qt::WaitCursor);

	m->ctrl("detectSoundSystem", nullptr);
	soundFillBox();

	g_pApp->restoreOverrideCursor();
}

void OptionsWidget_soundGeneral::mediaAutoDetect()
{
	KviModule * m = g_pModuleManager->getModule("mediaplayer");
	if(!m)
		return;

	g_pApp->setOverrideCursor(Qt::WaitCursor);

	m->ctrl("detectMediaPlayer", nullptr);
	mediaFillBox();

	g_pApp->restoreOverrideCursor();
}

void OptionsWidget_soundGeneral::soundFillBox()
{
	QStringList l;
	unsigned int cnt, i;
	KviModule * m = g_pModuleManager->getModule("snd");

	if(!m || !m->ctrl("getAvailableSoundSystems", &l))
		goto disable;

	m_pSoundSystemBox->clear();

	for(const auto& it : l)
		m_pSoundSystemBox->addItem(it);

	cnt = m_pSoundSystemBox->count();
	for(i = 0; i < cnt; i++)
	{
		QString t = m_pSoundSystemBox->itemText(i);
		if(KviQString::equalCI(t, KVI_OPTION_STRING(KviOption_stringSoundSystem)))
		{
			m_pSoundSystemBox->setCurrentIndex(i);
			break;
		}
	}

	return;
disable:
	m_pSoundSystemBox->clear();
	m_pSoundSystemBox->setEnabled(false);
	m_pSoundTestButton->setEnabled(false);
	m_pSoundAutoDetectButton->setEnabled(false);
}

void OptionsWidget_soundGeneral::mediaFillBox()
{
	QStringList l;
	unsigned int cnt, i;
	KviModule * m = g_pModuleManager->getModule("mediaplayer");

	if(!m || !m->ctrl("getAvailableMediaPlayers", &l))
		goto disable;

	m_pMediaPlayerBox->clear();

	for(const auto& it : l)
		m_pMediaPlayerBox->addItem(it);

	cnt = m_pMediaPlayerBox->count();
	for(i = 0; i < cnt; i++)
	{
		QString t = m_pMediaPlayerBox->itemText(i);
		if(KviQString::equalCI(t, KVI_OPTION_STRING(KviOption_stringPreferredMediaPlayer)))
		{
			m_pMediaPlayerBox->setCurrentIndex(i);
			break;
		}
	}

	return;
disable:
	m_pMediaPlayerBox->clear();
	m_pMediaPlayerBox->setEnabled(false);
	m_pMediaTestButton->setEnabled(false);
	m_pMediaAutoDetectButton->setEnabled(false);
}

void OptionsWidget_soundGeneral::commit()
{
	// avoid to commit if we've never been shown (and initialized)
	if(m_bFirstShow)
		return;

	KviOptionsWidget::commit();
	KVI_OPTION_STRING(KviOption_stringSoundSystem) = m_pSoundSystemBox->currentText();
	KVI_OPTION_STRING(KviOption_stringPreferredMediaPlayer) = m_pMediaPlayerBox->currentText();

	int idx = m_pTagsEncodingCombo->currentIndex();
	if(idx <= 0)
	{
		// guess from locale
		KVI_OPTION_STRING(KviOption_stringMp3TagsEncoding) = "";
	}
	else
	{
		KVI_OPTION_STRING(KviOption_stringMp3TagsEncoding) = m_pTagsEncodingCombo->itemText(idx);
	}

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	idx = m_pWinampEncodingCombo->currentIndex();
	if(idx <= 0)
	{
		// guess from locale
		KVI_OPTION_STRING(KviOption_stringWinampTextEncoding) = "";
	}
	else
	{
		KVI_OPTION_STRING(KviOption_stringWinampTextEncoding) = m_pWinampEncodingCombo->itemText(idx);
	}
#endif
}

OptionsWidget_sounds::OptionsWidget_sounds(QWidget * parent)
    : KviOptionsWidget(parent)
{
	setObjectName("sound_options_widget");
	createLayout();
	addLabel(0, 0, 0, 0, __tr2qs_ctx("New query opened:", "options"));
	addSoundSelector(1, 0, 1, 0, "", KviOption_stringOnNewQueryOpenedSound);

	addLabel(0, 1, 0, 1, __tr2qs_ctx("New message in inactive query:", "options"));
	addSoundSelector(1, 1, 1, 1, "", KviOption_stringOnQueryMessageSound);

	addLabel(0, 2, 0, 2, __tr2qs_ctx("Highlighted message in inactive window:", "options"));
	addSoundSelector(1, 2, 1, 2, "", KviOption_stringOnHighlightedMessageSound);

	addLabel(0, 3, 0, 3, __tr2qs_ctx("When I am kicked:", "options"));
	addSoundSelector(1, 3, 1, 3, "", KviOption_stringOnMeKickedSound);

	addRowSpacer(0, 4, 1, 4);
}

OptionsWidget_sounds::~OptionsWidget_sounds()
    = default;
