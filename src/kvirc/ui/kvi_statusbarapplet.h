#ifndef _KVI_STATUSBARAPPLET_H_
#define _KVI_STATUSBARAPPLET_H_
//=============================================================================
//
//   File : kvi_statusbarapplet.h
//   Created on Tue 07 Sep 2004 03:56:46 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004 Szymon Stefanek <pragma at kvirc dot net>
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
#include "kvi_list.h"
#include "kvi_heapobject.h"
#include "kvi_statusbar.h"

#include <qstring.h>
#include <qdict.h>
#include <qstatusbar.h>
#include <qlabel.h>
#include <qpixmap.h>

class QTimer;
class QLabel;
class QPopupMenu;

class KviConfig;
class KviFrame;
class KviIrcContext;
class KviIrcConnection;
class KviStatusBarApplet;
class KviStatusBarAppletDescriptor;

typedef KviStatusBarApplet * (*CreateAppletCallback)(KviStatusBar *,KviStatusBarAppletDescriptor *);

class KVIRC_API KviStatusBarAppletDescriptor : public KviHeapObject
{
	friend class KviStatusBar;
	friend class KviStatusBarApplet;
protected:
	QString                          m_szVisibleName;       // visible name of the applet
	QString                          m_szInternalName;      // UNIQUE name of the applet
	QString                          m_szPreloadModule;     // name of the module that must be preloaded for this applet
	CreateAppletCallback             m_pProc;               // creation callback
	QPixmap                        * m_pIcon;
	int                              m_iId;
	KviPtrList<KviStatusBarApplet> * m_pAppletList;
public:
	KviStatusBarAppletDescriptor(const QString &szVisibleName,
					const QString &szInternalName,
					CreateAppletCallback pProc,
					const QString &szPreloadModule = QString::null,
					const QPixmap &pixIcon = QPixmap());
	virtual ~KviStatusBarAppletDescriptor();
public:
	const QString &visibleName(){ return m_szVisibleName; };
	const QString &internalName(){ return m_szInternalName; };
	const QString &preloadModule(){ return m_szPreloadModule; };
	int id(){ return m_iId; };
	QPixmap * icon(){ return m_pIcon; };
protected:
	KviStatusBarApplet * create(KviStatusBar * pBar);
	void registerApplet(KviStatusBarApplet * a);
	void unregisterApplet(KviStatusBarApplet * a);
};


class KVIRC_API KviStatusBarApplet : public QLabel
{
	friend class KviStatusBar;
	Q_OBJECT
protected:
	KviStatusBar                 * m_pStatusBar;
	KviStatusBarAppletDescriptor * m_pDescriptor;
	bool                           m_bSelected;
public:
	KviStatusBarApplet(KviStatusBar * pParent,KviStatusBarAppletDescriptor *pDescriptor);
	virtual ~KviStatusBarApplet();
public:
	KviStatusBar * statusBar(){ return m_pStatusBar; };
	KviFrame * frame(){ return m_pStatusBar->frame(); };
	KviStatusBarAppletDescriptor * descriptor(){ return m_pDescriptor; };
	void select(bool bSelect = true);
	bool isSelected(){ return m_bSelected; };
protected:
	virtual void paintEvent(QPaintEvent *e);
	virtual void fillContextPopup(QPopupMenu *p){};
	virtual void loadState(const char * prefix,KviConfig *cfg){};
	virtual void saveState(const char * prefix,KviConfig *cfg){};
	virtual QString tipText(const QPoint &);
};



class KviStatusBarClock : public KviStatusBarApplet
{
	Q_OBJECT
public:
	KviStatusBarClock(KviStatusBar * pParent,KviStatusBarAppletDescriptor *pDescriptor);
	virtual ~KviStatusBarClock();
protected:
	bool m_bUtc;
public:
	static void selfRegister(KviStatusBar * pBar);
protected:
	virtual void fillContextPopup(QPopupMenu * p);
	virtual void timerEvent(QTimerEvent * e);
	virtual void loadState(const char * prefix,KviConfig *cfg);
	virtual void saveState(const char * prefix,KviConfig *cfg);
protected slots:
	void toggleUtc();
};

class KviStatusBarConnectionTimer : public KviStatusBarApplet
{
	Q_OBJECT
public:
	KviStatusBarConnectionTimer(KviStatusBar * pParent,KviStatusBarAppletDescriptor *pDescriptor);
	virtual ~KviStatusBarConnectionTimer();
protected:
	bool m_bTotal;
protected:
	virtual void timerEvent(QTimerEvent * e);
	virtual void fillContextPopup(QPopupMenu *p);
	virtual void loadState(const char * prefix,KviConfig *cfg);
	virtual void saveState(const char * prefix,KviConfig *cfg);
public:
	static void selfRegister(KviStatusBar * pBar);
protected slots:
	void toggleTotal();
};

class KviStatusBarSeparator : public KviStatusBarApplet
{
	Q_OBJECT
public:
	KviStatusBarSeparator(KviStatusBar * pParent,KviStatusBarAppletDescriptor *pDescriptor);
	virtual ~KviStatusBarSeparator();
public:
	static void selfRegister(KviStatusBar * pBar);
};

class KviStatusBarAwayIndicator : public KviStatusBarApplet
{
	Q_OBJECT
public:
	KviStatusBarAwayIndicator(KviStatusBar * pParent,KviStatusBarAppletDescriptor *pDescriptor);
	virtual ~KviStatusBarAwayIndicator();
public:
	static void selfRegister(KviStatusBar * pBar);
protected:
	bool m_bAwayOnAllContexts;
protected:
	virtual void mouseDoubleClickEvent(QMouseEvent *e);
	virtual QString tipText(const QPoint &);
	virtual void fillContextPopup(QPopupMenu *p);
	virtual void loadState(const char * prefix,KviConfig *cfg);
	virtual void saveState(const char * prefix,KviConfig *cfg);
protected slots:
	void updateDisplay();
	void toggleContext();
};

class KviStatusBarLagIndicator : public KviStatusBarApplet
{
	Q_OBJECT
public:
	KviStatusBarLagIndicator(KviStatusBar * pParent,KviStatusBarAppletDescriptor *pDescriptor);
	virtual ~KviStatusBarLagIndicator();
public:
	static void selfRegister(KviStatusBar * pBar);
protected:
	virtual void mouseDoubleClickEvent(QMouseEvent *e);
	virtual QString tipText(const QPoint &);
protected slots:
	void updateDisplay();
};


#endif //!_KVI_STATUSBARAPPLET_H_
