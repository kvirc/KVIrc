#ifndef _KVI_MODEW_H_
#define _KVI_MODEW_H_

//============================================================================
//
//   File : kvi_modew.h
//   Creation date : Sat Nov 12 2005 23:50:12 by Alexey Uzhva
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2005-2008 Alexey Uzhva (wizard at opendoor dot ru)
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
//============================================================================

#include "kvi_themedlineedit.h"

class KviChannel;

class KVIRC_API KviModeWidget : public KviThemedLineEdit
{
	Q_OBJECT

public:
	KviModeWidget(QWidget * par,KviChannel* chan,const char * name=0);
	~KviModeWidget();
	void reset();
	void refreshModes();
private:
	KviChannel            *  m_pChannel;
protected:
	void mouseDoubleClickEvent(QMouseEvent * e);
	void keyReleaseEvent (QKeyEvent * e);
public slots:
	void editorReturnPressed();
	void editorTextEdited( const QString & );
};

#endif //_KVI_MODEW_H_
