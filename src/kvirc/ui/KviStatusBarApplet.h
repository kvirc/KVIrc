#ifndef _KVI_STATUSBARAPPLET_H_
#define _KVI_STATUSBARAPPLET_H_
//=============================================================================
//
//   File : KviStatusBarApplet.h
//   Creation date : Tue 07 Sep 2004 03:56:46 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004 Szymon Stefanek <pragma at kvirc dot net>
//   Copyright (C) 2008 Elvio Basello <hellvis69 at netsons dor org>
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
#include "KviPointerList.h"
#include "KviHeapObject.h"
#include "KviStatusBar.h"
#include "KviPointerHashTable.h"
#include "KviHttpRequest.h"

#include <QString>
#include <QStatusBar>
#include <QLabel>
#include <QPixmap>

class QTimer;
class QLabel;
class QMenu;
class KviConfigurationFile;
class KviMainWindow;
class KviIrcContext;
class KviIrcConnection;
class KviStatusBarApplet;
class KviStatusBarAppletDescriptor;

typedef KviStatusBarApplet * (*CreateAppletCallback)(KviStatusBar *, KviStatusBarAppletDescriptor *);

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
	KviPointerList<KviStatusBarApplet> * m_pAppletList;
public:
	KviStatusBarAppletDescriptor(const QString & szVisibleName,
					const QString & szInternalName,
					CreateAppletCallback pProc,
					const QString & szPreloadModule = QString(),
					const QPixmap & pixIcon = QPixmap());
	virtual ~KviStatusBarAppletDescriptor();
public:
	const QString & visibleName(){ return m_szVisibleName; };
	const QString & internalName(){ return m_szInternalName; };
	const QString & preloadModule(){ return m_szPreloadModule; };
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
	int mIndex;
public:
	KviStatusBarApplet(KviStatusBar * pParent, KviStatusBarAppletDescriptor * pDescriptor);
	virtual ~KviStatusBarApplet();
public:
	KviStatusBar * statusBar(){ return m_pStatusBar; };
	KviMainWindow * frame(){ return m_pStatusBar->frame(); };
	KviStatusBarAppletDescriptor * descriptor(){ return m_pDescriptor; };
	inline void setIndex(int i){ mIndex=i; };
	inline int index() const { return mIndex; };
protected:
	virtual void fillContextPopup(QMenu *){};
	virtual void loadState(const char *, KviConfigurationFile *){};
	virtual void saveState(const char *, KviConfigurationFile *){};
	virtual QString tipText(const QPoint &);
};

//this is needed to sort a KviPointerList<KviStatusBarApplet *>
inline int kvi_compare(const KviStatusBarApplet * p1, const KviStatusBarApplet * p2)
{
	return p1->index() > p2->index();
}


class KviStatusBarClock : public KviStatusBarApplet
{
	Q_OBJECT
public:
	KviStatusBarClock(KviStatusBar * pParent, KviStatusBarAppletDescriptor * pDescriptor);
	virtual ~KviStatusBarClock();
	enum Format { HMS, HM };
protected:
	bool m_bUtc;
	bool m_b24h;
	int  m_iType;
public:
	static void selfRegister(KviStatusBar * pBar);
protected:
	virtual void fillContextPopup(QMenu * p);
	virtual void timerEvent(QTimerEvent * e);

	virtual void loadState(const char * pcPrefix, KviConfigurationFile * pCfg);
	virtual void saveState(const char * pcPrefix, KviConfigurationFile * pCfg);
	void adjustMinWidth();
protected slots:
	void toggleUtc();
	void toggle24h();
	void changeFormat(QAction *);
};


class KviStatusBarConnectionTimer : public KviStatusBarApplet
{
	Q_OBJECT
public:
	KviStatusBarConnectionTimer(KviStatusBar * pParent, KviStatusBarAppletDescriptor * pDescriptor);
	virtual ~KviStatusBarConnectionTimer();
protected:
	bool m_bTotal;
protected:
	virtual void timerEvent(QTimerEvent * e);
	virtual void fillContextPopup(QMenu * p);
	virtual void loadState(const char * pcPrefix, KviConfigurationFile * pCfg);
	virtual void saveState(const char * pcPrefix, KviConfigurationFile * pCfg);
public:
	static void selfRegister(KviStatusBar * pBar);
protected slots:
	void toggleTotal();
};


class KviStatusBarSeparator : public KviStatusBarApplet
{
	Q_OBJECT
public:
	KviStatusBarSeparator(KviStatusBar * pParent, KviStatusBarAppletDescriptor * pDescriptor);
	virtual ~KviStatusBarSeparator();
public:
	static void selfRegister(KviStatusBar * pBar);
};


class KviStatusBarAwayIndicator : public KviStatusBarApplet
{
	Q_OBJECT
public:
	KviStatusBarAwayIndicator(KviStatusBar * pParent, KviStatusBarAppletDescriptor * pDescriptor);
	virtual ~KviStatusBarAwayIndicator();
public:
	static void selfRegister(KviStatusBar * pBar);
protected:
	bool m_bAwayOnAllContexts;
protected:
	virtual void mouseDoubleClickEvent(QMouseEvent * e);
	virtual QString tipText(const QPoint &);
	virtual void fillContextPopup(QMenu * p);
	virtual void loadState(const char * pcPrefix, KviConfigurationFile * pCfg);
	virtual void saveState(const char * pcPrefix, KviConfigurationFile * pCfg);
protected slots:
	void updateDisplay();
	void toggleContext();
};


class KviStatusBarLagIndicator : public KviStatusBarApplet
{
	Q_OBJECT
public:
	KviStatusBarLagIndicator(KviStatusBar * pParent, KviStatusBarAppletDescriptor * pDescriptor);
	virtual ~KviStatusBarLagIndicator();
public:
	static void selfRegister(KviStatusBar * pBar);
protected:
	virtual void mouseDoubleClickEvent(QMouseEvent * e);
	virtual QString tipText(const QPoint &);
protected slots:
	void updateDisplay();
};


class KviStatusBarUpdateIndicator : public KviStatusBarApplet
{
	Q_OBJECT
public:
	KviStatusBarUpdateIndicator(KviStatusBar * pParent, KviStatusBarAppletDescriptor * pDescriptor);
	virtual ~KviStatusBarUpdateIndicator();
	static void selfRegister(KviStatusBar * pBar);
protected:
	virtual void mouseDoubleClickEvent(QMouseEvent * e);
	virtual QString tipText(const QPoint &);
	virtual void fillContextPopup(QMenu * p);
	virtual void loadState(const char * pcPrefix, KviConfigurationFile * pCfg);
	virtual void saveState(const char * pcPrefix, KviConfigurationFile * pCfg);
	void checkVersion();
	void getNewVersion();
protected slots:
	void updateDisplay();
	void toggleStartup();
	void toggleRevision();
	void responseReceived(const QString &);
	void requestCompleted(bool status);
	void binaryDataReceived(const KviDataBuffer &);
private:
	KviHttpRequest * m_pHttpRequest;
	QString          m_szHttpResponse;
	QString          m_szNewVersion;
	bool             m_bCheckDone;
	bool             m_bCheckFailed;
	bool             m_bUpdateStatus;
	bool             m_bUpdateOnStartup;
	bool             m_bUpdateRevision;
};

#endif //_KVI_STATUSBARAPPLET_H_
