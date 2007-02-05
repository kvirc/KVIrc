#ifndef _KVI_HISTORYWIN_H_
#define _KVI_HISTORYWIN_H_
//
//   File : kvi_historywin.h
//   Creation date : Mon Aug 19 01:34:46 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_string.h"

#include "kvi_tal_listbox.h"

class KviInput;

#define KVI_HISTORY_WIN_HEIGHT 130

class KVIRC_API KviHistoryWindow : public KviTalListBox
{
	Q_OBJECT
public:
	KviHistoryWindow();
	~KviHistoryWindow();
private:
	KviInput      * m_pOwner;
	int             m_iTimerId;
public:
	void popup(KviInput *owner);
//	KviInput * owner(){ return m_pOwner; };
	void doHide();
private:
	virtual void show();
//	bool findTypedSeq(); // returns true if it is a complete word
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void timerEvent(QTimerEvent *);
	void fill();
public slots:
	void ownerDead();
	void itemSelected(const QString &str);
};

#endif //_KVI_HISTORYWIN_H_
