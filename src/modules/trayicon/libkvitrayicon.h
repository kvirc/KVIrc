#ifndef _KVI_TRAYICON_H_
#define _KVI_TRAYICON_H_
//
//   This file is part of the KVirc irc client distribution
//   Creation date : Tue 02 Jan 2001 14:28:25 by Szymon Stefanek
//   Copyright (C) 2000-2001 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"
#include "kvi_frame.h"

#include <QSystemTrayIcon>

class KviTalPopupMenu;
class QPixmap;
class QTimer;
class KviDynamicToolTip;
//class KviFrame;

class KviTrayIcon : public QSystemTrayIcon, public KviDockExtension
{
	Q_OBJECT
public:
	KviTrayIcon(KviFrame * win);
	~KviTrayIcon();
protected:
	KviFrame               * m_pFrm;
	KviDynamicToolTip      * m_pTip;
	KviTalPopupMenu        * m_pContextPopup;
	KviTalPopupMenu        * m_pAwayPopup;

	int m_iToggleFrame;
	int m_iAwayMenuId;

	bool m_bFlashed;
	
	QPixmap m_CurrentPixmap;
	QTimer* m_pFlashingTimer;
	int m_iConsoles;
	int m_iChannels;
	int m_iQueries;
	int m_iOther;
public:
	KviFrame * frame(){ return m_pFrm; };
public:
	virtual void refresh();
	virtual void die();
	void updateIcon();
protected:
	void grabActivityInfo();

protected slots:
	void fillContextPopup();
	void toggleParentFrame();
	void tipRequest(KviDynamicToolTip *tip,const QPoint &pnt);
	void doAway(int);
	void flashingTimerShot();

	void activatedSlot( QSystemTrayIcon::ActivationReason reason );
};

#endif //_KVI_TRAYICON_H_
