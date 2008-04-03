#ifndef _KVI_STATUSBAR_H_
#define _KVI_STATUSBAR_H_
//=============================================================================
//
//   File : kvi_statusbar.h
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
#include "kvi_pointerlist.h"
#include "kvi_heapobject.h"
#include "kvi_pointerhashtable.h"

#include <QString>
#include <QStatusBar>

class QTimer;
class QLabel;
class KviFrame;
class KviTalPopupMenu;
class KviStatusBarApplet;
class KviStatusBarAppletDescriptor;
class KviIrcContext;
class KviDynamicToolTip;

class KVIRC_API KviStatusBarMessage : public KviHeapObject
{
	friend class KviStatusBar;
protected:
	QString      m_szText;
	unsigned int m_uTimeout;
	unsigned int m_uPriority;
public:
	KviStatusBarMessage(const QString &szText,unsigned int uTimeout = 8000,unsigned int uPriority = 0)
		: KviHeapObject(), m_szText(szText), m_uTimeout(uTimeout), m_uPriority(uPriority) {};
	~KviStatusBarMessage(){};
public:
	const QString & text(){ return m_szText; };
	unsigned int timeout(){ return m_uTimeout; };
	unsigned int priority(){ return m_uPriority; };
};

class KVIRC_API KviStatusBar : public QStatusBar
{
	friend class KviStatusBarApplet;
	friend class KviFrame;
	Q_OBJECT
public:
	KviStatusBar(KviFrame * pFrame);
	~KviStatusBar();
protected:
	KviFrame                                 * m_pFrame;
	KviPointerList<KviStatusBarMessage>          * m_pMessageQueue;
	QTimer                                   * m_pMessageTimer;
	QLabel                                   * m_pMessageLabel;
	KviPointerList<KviStatusBarApplet>           * m_pAppletList;
	KviPointerHashTable<QString,KviStatusBarAppletDescriptor>      * m_pAppletDescriptors;
	KviTalPopupMenu                               * m_pContextPopup;
	KviTalPopupMenu                               * m_pAppletsPopup;
	KviStatusBarApplet                       * m_pClickedApplet;
	int                                        m_iLastMinimumHeight;
	bool                                       m_bStopLayoutOnAddRemove;
	KviDynamicToolTip                        * m_pToolTip;
public:
	KviFrame * frame(){ return m_pFrame; };
protected slots:
	void messageTimerFired();
protected:
	void showFirstMessageInQueue();
	virtual void paintEvent(QPaintEvent * e);
	virtual void mousePressEvent(QMouseEvent * e);
	virtual void mouseMoveEvent(QMouseEvent * e);
	virtual void mouseReleaseEvent(QMouseEvent * e);
	virtual void resizeEvent(QResizeEvent * e);
	virtual bool event(QEvent * e);
	void registerApplet(KviStatusBarApplet * a);
	void unregisterApplet(KviStatusBarApplet * a);
	void recalcMinimumHeight();
	void layoutChildren();
	void updateLayout(){ recalcMinimumHeight(); layoutChildren(); };
	void save();
	void load();
	KviStatusBarApplet * createApplet(const QString &szInternalName);
	void showLayoutHelp();
public:
	bool appletExists(KviStatusBarApplet * pApplet);
	// pnt is global!
	KviStatusBarApplet * appletAt(const QPoint &pnt,bool bBestMatch = false);

	KviTalPopupMenu * contextPopup();
	// takes the ownership of pMsg
	void queueMessage(KviStatusBarMessage * pMsg);
	// called by KviFrame
	void activeWindowChanged();
	
	void registerAppletDescriptor(KviStatusBarAppletDescriptor * d);

	//void addApplet(KviStatusBarApplet * pApplet);
	//void removeApplet(KviStatusBarApplet * pApplet);
protected slots:
	void contextPopupAboutToShow();
	void appletsPopupAboutToShow();
	void appletsPopupActivated(int id);
	void removeClickedApplet();
	void setPermanentMessage();
	void tipRequest(KviDynamicToolTip *pTip,const QPoint &pnt);
};

#endif //!_KVI_STATUSBAR_H_
