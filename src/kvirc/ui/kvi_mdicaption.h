#ifndef _KVI_MDICAPTION_H_
#define _KVI_MDICAPTION_H_
//=================================================================================================
//
//   File : kvi_mdicaption.h
//   Creation date : Tue Sep 2 2003 02:35:45 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2003 Szymon Stefanek (pragma at kvirc dot net)
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
//=================================================================================================

#include "kvi_settings.h"

#include <qtoolbutton.h>
#include <qlabel.h>
#include "kvi_styled_controls.h"

class KviMdiManager;
class KviMdiChild;


class KVIRC_API KviMdiCaptionButton : public QToolButton
{
	Q_OBJECT
public:
	KviMdiCaptionButton(const QPixmap &pix,QWidget * parent,const char * name);
	~KviMdiCaptionButton();
protected:
	virtual void drawButton(QPainter * p);
#ifdef COMPILE_USE_QT4
	virtual void paintEvent(QPaintEvent *e);
#endif
};


class KviMenuBarToolButton : public KviStyledToolButton
{
	Q_OBJECT
public:
	KviMenuBarToolButton(QWidget * par,const QPixmap &img, const char * name);
	~KviMenuBarToolButton();
public:
	virtual QSize sizeHint() const;
};


class KVIRC_API KviMdiCaption : public QWidget
{
	friend class KviMdiChild;
	Q_OBJECT
public:
	KviMdiCaption(KviMdiChild * parent,const char * name);
	~KviMdiCaption();
protected:
	QPoint                m_lastMousePos;
	bool                  m_bMouseGrabbed;
	int                   m_iLineSpacing;
	bool                  m_bActive;
	KviMdiCaptionButton * m_pSystemButton;
	KviMdiCaptionButton * m_pMinimizeButton;
	KviMdiCaptionButton * m_pMaximizeButton;
	KviMdiCaptionButton * m_pCloseButton;
public:
	int heightHint();
	void setActive(bool bActive);
	bool active(){ return m_bActive; };
	void setSystemIcon(const QPixmap &pix){ m_pSystemButton->setPixmap(pix); };
	const QPixmap * systemIcon(){ return m_pSystemButton->pixmap(); };
	void enableClose(bool bEnable){ m_pCloseButton->setEnabled(bEnable); };
	bool closeEnabled(){ return m_pCloseButton->isEnabled(); };
	virtual void setFocusProxy(QWidget * w);
protected:
	void calcLineSpacing();
	virtual void fontChange(const QFont &old);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseDoubleClickEvent(QMouseEvent *e);
	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);
	virtual void paintEvent(QPaintEvent * e);
	virtual void resizeEvent(QResizeEvent * e);
	void reloadImages();
};


#endif //_KVI_MDICAPTION_H_
