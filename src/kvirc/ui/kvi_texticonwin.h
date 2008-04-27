#ifndef _KVI_TEXTICONWIN_H_
#define _KVI_TEXTICONWIN_H_
//=============================================================================
//
//   File : kvi_texticonwin.h
//   Creation date : Fri May 17 2002 02:33:45 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2004 Szymon Stefanek (pragma at kvirc dot net)
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
//=============================================================================

#include "kvi_settings.h"
#include "kvi_string.h"
#include "kvi_tal_iconview.h"

#define KVI_TEXTICON_WIN_WIDTH 230
#define KVI_TEXTICON_WIN_HEIGHT 200

class KVIRC_API KviTextIconWindow : public KviTalIconView
{
	Q_OBJECT
public:
	KviTextIconWindow();
	~KviTextIconWindow();
private:
	QWidget       * m_pOwner;
	KviTalIconViewItem * m_pItem;
	QString         m_szTypedSeq;
	QString         m_szCurFullSeq;
	bool            m_bAltMode; // in alt mode the itemSelected() string contains
	                            // also the CTRL+I escape code
	int             m_iTimerId;
public:
	void popup(QWidget *owner,bool bAltMode = false);
private:
	void doHide();
	virtual void show();
	bool findTypedSeq(); // returns true if it is a complete word
	virtual void keyPressEvent(QKeyEvent *e);
	//virtual void mouseMoveEvent ( QMouseEvent * e )
	virtual void mousePressEvent(QMouseEvent *);
	virtual void timerEvent(QTimerEvent *);
public slots:
	void fill();
	void ownerDead();
	void currentItemChanged(KviTalIconViewItem * item,KviTalIconViewItem * prev);
	void itemSelected(KviTalIconViewItem * item);
};

#endif //_KVI_TEXTICONWIN_H_
