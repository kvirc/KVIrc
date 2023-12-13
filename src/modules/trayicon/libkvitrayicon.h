#ifndef _KVI_TRAYICON_H_
#define _KVI_TRAYICON_H_
//=============================================================================
//
//   File : libkvitrayicon.h
//   Creation date : Tue Jan 02 2001 14:34:12 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2007 Alexey Uzhva (wizard at opendoor dot ru)
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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
#include "KviMainWindow.h"
#include "KviTrayIcon.h"

#include <QObject>
#include <QLabel>
#include <QMenu>
#include <QString>
#include <QTimer>

#ifdef COMPILE_KDE_SUPPORT
#include <KStatusNotifierItem>
#else
#include <QSystemTrayIcon>
#endif

class QPixmap;

class KviTrayIconWidget final
#ifdef COMPILE_KDE_SUPPORT
: public KStatusNotifierItem, public KviTrayIcon
#else
: public QSystemTrayIcon, public KviTrayIcon
#endif
{
	Q_OBJECT
public:
	KviTrayIconWidget();
	~KviTrayIconWidget() override;

private:
	QMenu * m_pContextPopup;
	QMenu m_awayPopup;
#ifndef COMPILE_ON_MAC
	QLabel * m_pTitleLabel;
#endif

	QAction * m_pToggleFrame;
	QAction * m_pAwayMenuId;

	bool m_bFlashed;
	bool m_bHidden;

	QPixmap m_CurrentPixmap;

	QTimer m_flashingTimer;
	unsigned int m_iConsoles;
	unsigned int m_iChannels;
	unsigned int m_iQueries;
	unsigned int m_iOther;

public:
	void refresh() override;
	void updateIcon();
#ifdef COMPILE_KDE_SUPPORT
	void show();
#endif

private:
	void grabActivityInfo();
	const QString getToolTipText(bool bHtml);
private slots:
	void fillContextPopup();
	void toggleParentFrame();
	void doAway(bool);
	void flashingTimerShot();
#ifndef COMPILE_KDE_SUPPORT
	void activatedSlot(QSystemTrayIcon::ActivationReason reason);
#endif
	void executeInternalCommand(bool);
	void disableTrayIcon();
};

#endif //_KVI_TRAYICON_H_
