#ifndef _KVI_MDICHILD_H_
#define _KVI_MDICHILD_H_
//=================================================================================================
//
//   File : kvi_mdichild.h
//   Creation date : Wed Jun 21 2000 17:35:04 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=================================================================================================

#include "kvi_settings.h"
#include "kvi_string.h"

#include <qframe.h>
#include <qtoolbutton.h>
#include <qlabel.h>


class KviMdiManager;
class KviMdiChild;
class KviMdiCaption;

class QCursor;



class KVIRC_API KviMdiChild : public QFrame
{
	friend class KviMdiManager;
	friend class KviMdiCaption;
	Q_OBJECT
public:
	KviMdiChild(KviMdiManager* par,const char * name = 0);
	~KviMdiChild();
public:
	enum MdiChildState { Maximized , Minimized , Normal };
protected:
	KviMdiManager              * m_pManager;
	KviMdiCaption              * m_pCaption;
private:
	int                          m_iResizeCorner;
	int                          m_iLastCursorCorner;
	bool                         m_bResizeMode;
	QWidget                    * m_pClient;
	MdiChildState                m_state;
	QRect                        m_restoredGeometry;

	QString                      m_szXmlActiveCaption;
	QString                      m_szXmlInactiveCaption;
	QString                      m_szPlainCaption;
public:
	QRect restoredGeometry();
	void setRestoredGeometry(const QRect &r){ m_restoredGeometry = r; };
	void setClient(QWidget * w);
	QWidget * client(){ return m_pClient; };
	void unsetClient();
	KviMdiCaption * captionLabel(){ return m_pCaption; };
	MdiChildState state(){ return m_state; };
	const QString & plainCaption(){ return m_szPlainCaption; };
	const QString & xmlActiveCaption(){ return m_szXmlActiveCaption; };
	const QString & xmlInactiveCaption(){ return m_szXmlInactiveCaption; };
	void setCaption(const QString & plain,const QString & xmlActive,const QString & xmlInactive);
	virtual QSize sizeHint();
	void setIcon(const QPixmap &pix);
	const QPixmap * icon();
	void enableClose(bool bEnable);
	bool closeEnabled();
	KviMdiManager * manager(){ return m_pManager; };
	void activate(bool bSetFocus);
	void reloadImages();
public slots:
	void maximize();
	void minimize();
	void restore();
	void systemPopupSlot();
	void closeRequest();
signals:
	void systemPopupRequest(const QPoint & pnt);
protected:
#ifdef COMPILE_USE_QT4
	virtual void setBackgroundRole(QPalette::ColorRole);
#else
	virtual void setBackgroundMode(QWidget::BackgroundMode bgmd);
#endif
	virtual void resizeEvent(QResizeEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);
	virtual void leaveEvent(QEvent *e);
	virtual void focusInEvent(QFocusEvent *);
	virtual void moveEvent(QMoveEvent *e);
//	bool eventFilter(QObject *o,QEvent *e);
	void emitSystemPopupRequest(const QPoint & pnt){ emit systemPopupRequest(pnt); };
private:
//	void linkChildren(QWidget *w);
//	void unlinkChildren(QWidget *w);
	QCursor getResizeCursor(int resizeCorner);
	void resizeWindowOpaque(int resizeCorner);
	int getResizeCorner(int ax,int ay);
	void calculateMinimumSize(int &minWidth,int &minHeight);
	void setResizeCursor(int resizeCorner);
	void calculateResizeRect(int resizeCorner,QPoint mousePos,QRect &resizeRect,int minWidth,int minHeight);
};


#endif //_KVI_MDICHILD_H_
