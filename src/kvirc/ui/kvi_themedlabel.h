#ifndef _KVI_THEMEDLABEL_H_
#define _KVI_THEMEDLABEL_H_
//=============================================================================
//
//   File : kvi_themedlabel.h
//   Creation date : Tue Aug 29 2000 21:12:12 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include <QFrame>

class KVIRC_API KviThemedLabel : public QFrame
{
	Q_OBJECT
	Q_PROPERTY(int TransparencyCapable READ dummyRead)
public:
	KviThemedLabel(QWidget * par,const char * name);
	~KviThemedLabel();
private:
	QString m_szText;
	bool    m_bAutoHeight;
protected:
	virtual void paintEvent ( QPaintEvent * event );
	virtual void mouseDoubleClickEvent(QMouseEvent *e);
public:
	int dummyRead() const { return 0; };
	void setText(const char * text);
	void setText(const QString& text);
	QString text() { return m_szText; };
	void setAutoHeight(bool value) { m_bAutoHeight=value; };
	void applyOptions();
	
//	QSize sizeHint();
signals:
	void doubleClicked();
};

#endif //_KVI_THEMEDLABEL_H_
