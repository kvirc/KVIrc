#ifndef _KVI_TRAYICON_H_
#define _KVI_TRAYICON_H_
//=============================================================================
//
//   File : libkvitrayicon.h
//   Creation date : Tue Jan 02 2001 14:34:12 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2007 Alexey Uzhva (wizard at opendoor dot ru)
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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

#include "kvi_settings.h"
#include "KviMainWindow.h"
#include "KviDockExtension.h"

#include <QSystemTrayIcon>
#include <QLabel>

class QPixmap;
class QTimer;
class KviTalPopupMenu;
class KviDynamicToolTip;
//class KviMainWindow;

class TrayIcon : public QSystemTrayIcon, public KviDockExtension
{
	Q_OBJECT
public:
	TrayIcon(KviMainWindow * win);
	~TrayIcon();
protected:
	KviMainWindow               * m_pFrm;
	KviDynamicToolTip      * m_pTip;
	KviTalPopupMenu        * m_pContextPopup;
	KviTalPopupMenu        * m_pAwayPopup;
	QLabel                 * m_pTitleLabel;

	QAction* m_pToggleFrame;
	QAction* m_pAwayMenuId;

	bool m_bFlashed;

	QPixmap m_CurrentPixmap;
	QTimer* m_pFlashingTimer;
	unsigned int m_iConsoles;
	unsigned int m_iChannels;
	unsigned int m_iQueries;
	unsigned int m_iOther;
public:
	KviMainWindow * frame(){ return m_pFrm; };
public:
	virtual void refresh();
	virtual void die();
	void updateIcon();
protected:
	void grabActivityInfo();
	virtual bool event(QEvent * e);
protected slots:
	void fillContextPopup();
	void toggleParentFrame();
	void doAway(bool);
	void flashingTimerShot();
	void activatedSlot( QSystemTrayIcon::ActivationReason reason );
	void executeInternalCommand(bool);
};

#endif //_KVI_TRAYICON_H_
