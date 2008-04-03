#ifndef _KVI_USERLISTVIEW_H_
#define _KVI_USERLISTVIEW_H_

//=============================================================================
//
//   File : kvi_userlistview.h
//   Creation date : Tue Aug 1 2000 21:03:41 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2004 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_pointerhashtable.h"
#include "kvi_toolwindows_container.h"
#include "kvi_string.h"
#include "kvi_ircuserdb.h"
#include "kvi_ircmask.h"
#include "kvi_time.h"
#include "kvi_tal_tooltip.h"

#include <time.h>

#include <QWidget>

class QLabel;
class QScrollBar;
class KviUserListView;
class KviUserListViewArea;
class KviConsole;
class KviWindow;

#define KVI_USERLISTVIEW_GRIDTYPE_3DGRID 0
#define KVI_USERLISTVIEW_GRIDTYPE_3DBUTTONS 1
#define KVI_USERLISTVIEW_GRIDTYPE_PLAINGRID 2
#define KVI_USERLISTVIEW_GRIDTYPE_DOTGRID 3

#define KVI_USERLISTVIEW_GRIDTYPE_MAXIMUM 3
#define KVI_USERLISTVIEW_GRIDTYPE_DEFAULT 0

class KVIRC_API KviUserListToolTip : public KviTalToolTip
{
public:
	KviUserListToolTip(KviUserListView *v,KviUserListViewArea * a);
	virtual ~KviUserListToolTip();
private:
	KviUserListView * m_pListView;
public:
	virtual void maybeTip(const QPoint &pnt);
	void doTip(const QRect &rct,const QString &str){ tip(rct,str); };
};


class KVIRC_API KviUserListEntry
{
	friend class KviUserListView;
	friend class KviUserListViewArea;
public:
	KviUserListEntry(KviUserListView * parent,const QString &nick,KviIrcUserEntry * e,short int iFlags,bool bJoinTimeUnknown = true);
	~KviUserListEntry();
protected:
	KviUserListView  * m_pListView;
	QString            m_szNick;
	KviIrcUserEntry  * m_pGlobalData;
	short int          m_iFlags;
	short int          m_iTemperature;    // user temperature : 0 = neutral
	kvi_time_t         m_lastActionTime;
	kvi_time_t         m_joinTime;

	int                m_iHeight;
	bool               m_bSelected;
	KviUserListEntry * m_pNext;
	KviUserListEntry * m_pPrev;
public:
	short int flags() const { return m_iFlags; };
	KviIrcUserEntry * globalData(){ return m_pGlobalData; };
	const QString &nick() const { return m_szNick; };
	KviUserListEntry * next(){ return m_pNext; };
	bool color(QColor& color);
protected:
	void recalcSize();
};


typedef struct _KviUserListViewUserStats 
{
	unsigned int uTotal;           // total users on the channel
	unsigned int uActive;          // active users in the last 10 mins
	unsigned int uActiveOp;        // active operators in the last 10 minutes
	unsigned int uHot;             // hot active users
	unsigned int uHotOp;           // hot operators
	unsigned int uOp;              // total operators
	unsigned int uHalfOp;          // total halfops
	unsigned int uVoiced;          // total voiced users
	unsigned int uChanAdmin;       // total channel administrators
	unsigned int uChanOwner;       // total channel owners
	unsigned int uUserOp;          // total userops (uops)
	int iAvgTemperature;           // average user temperature
} KviUserListViewUserStats;


class KVIRC_API KviUserListView : public KviWindowToolWidget
{
	friend class KviUserListEntry;
	friend class KviUserListViewArea;
	friend class KviUserListToolTip;
	friend class KviConsole;
	friend class KviChannel;
	friend class KviQuery;
	Q_OBJECT
public:
	KviUserListView(QWidget * parent,KviWindowToolPageButton* button,KviIrcUserDataBase * db,KviWindow * pWnd,int dictSize = 5,const QString &label_text = QString::null,const char * name = 0);
	~KviUserListView();
protected:
	KviPointerHashTable<QString,KviUserListEntry>         * m_pEntryDict;
	KviUserListEntry                * m_pTopItem;
	KviUserListEntry                * m_pHeadItem;
	KviUserListEntry                * m_pTailItem;
	KviUserListEntry                * m_pIterator;
	QLabel                          * m_pUsersLabel;
	KviUserListViewArea             * m_pViewArea;
	KviIrcUserDataBase              * m_pIrcUserDataBase;
	int                               m_iSelectedCount;
	int                               m_iOpCount;
	int                               m_iVoiceCount;
	int                               m_iHalfOpCount;
	int                               m_iChanAdminCount;
	int                               m_iChanOwnerCount;
	int                               m_iUserOpCount;
	int                               m_iTotalHeight;
	int                               m_iFontHeight;
	KviUserListToolTip              * m_pToolTip;
	int                               m_ibEntries;
	int                               m_ieEntries;
	int                               m_iIEntries;
	KviWindow                       * m_pKviWindow;
protected:
	void maybeTip(KviUserListToolTip * tip,const QPoint &pnt);
	void triggerUpdate();
	void updateUsersLabel();
	void insertUserEntry(const QString  &nick,KviUserListEntry * e);
	void removeAllEntries();
	virtual void resizeEvent(QResizeEvent *);
	bool partInternal(const QString &nick,bool bRemove = true);
	void setUserDataBase(KviIrcUserDataBase * db){ m_pIrcUserDataBase = db; };
	void updateScrollBarRange();
public:
	void updateArea();
	void select(const QString&);
	void applyOptions();
	KviPointerHashTable<QString,KviUserListEntry> * entryDict(){ return m_pEntryDict; };
	KviUserListEntry * firstItem(){ return m_pHeadItem; };
	KviUserListEntry * itemAt(const QPoint &pnt,QRect * rct = 0);
	bool itemVisible(KviUserListEntry * e);
	KviWindow * window(){ return m_pKviWindow; };

	unsigned int count(){ return m_pEntryDict->count(); };
	int selectedCount(){ return m_iSelectedCount; };
	int opCount(){ return m_iOpCount; };
	int voiceCount(){ return m_iVoiceCount; };
	int chanOwnerCount(){ return m_iChanOwnerCount; };
	int chanAdminCount(){ return m_iChanAdminCount; };
	int halfOpCount(){ return m_iHalfOpCount; };
	int userOpCount(){ return m_iUserOpCount; };

	KviUserListEntry * findEntry(const QString &nick){ return nick.isEmpty() ? 0 : m_pEntryDict->find(nick); };
	void appendSelectedNicknames(QString &buffer);
	QString * firstSelectedNickname();
	QString * nextSelectedNickname();
	void partAll();
	void partAllButOne(const QString &whoNot);
	void userStats(KviUserListViewUserStats * s);
	int getUserModeLevel(const QString &nick);
	kvi_time_t getUserJoinTime(const QString &nick);
	kvi_time_t getUserLastActionTime(const QString &nick);
	char getUserFlag(KviUserListEntry * e);
	char getUserFlag(const QString &nick){ return getUserFlag(m_pEntryDict->find(nick)); };
	bool part(const QString &nick){ return partInternal(nick,true); };
	bool op(const QString &nick,bool bOp);
	void prependUserFlag(const QString &nick,QString &buffer);
	int flags(const QString &nick);
	bool isOp(const QString &nick,bool bAtLeast = false);
	bool isVoice(const QString &nick,bool bAtLeast = false);
	bool isHalfOp(const QString &nick,bool bAtLeast = false);
	bool isChanAdmin(const QString &nick,bool bAtLeast = false);
	bool isChanOwner(const QString &nick,bool bAtLeast = false);
	bool isUserOp(const QString &nick,bool bAtLeast = false);
	bool voice(const QString &nick,bool bVoice);
	bool setChanAdmin(const QString &nick,bool bChanAdmin);
	bool setChanOwner(const QString &nick,bool bChanOwner);
	bool halfop(const QString &nick,bool bHalfOp);
	bool userop(const QString &nick,bool bUserOp);
	void userAction(KviIrcMask *user,int actionTemperature);
	bool userActionVerifyMask(const QString &nick,const QString &user,const QString &host,int actionTemperature,QString &oldUser,QString &oldHost);
	void userAction(const QString &nick,const QString &user,const QString &host,int actionTemperature);
	void userAction(const QString &nick,int actionTemperature);
	bool nickChange(const QString &oldNick,const QString &newNick);
	//KviUserListEntry * join(const char *nick,const char * user,const char * host,bool bOp,bool bVoice,bool bHalfOp);
	KviUserListEntry * join(const QString &nick,const QString &user = QString::null,const QString &host = QString::null,int iFlags = 0);
	bool avatarChanged(const QString &nick);
	void enableUpdates(bool bEnable);
	void setMaskEntries(char type, int num);
	void emitRightClick();
	void emitDoubleClick();
	bool completeNickStandard(const QString &begin,const QString &skipAfter,QString &buffer,bool bAppendMask);
	void completeNickBashLike(const QString &begin,KviPointerList<QString> * l,bool bAppendMask);
};

class KVIRC_API KviUserListViewArea : public QWidget
{
	friend class KviUserListView;

	Q_OBJECT
	Q_PROPERTY(int TransparencyCapable READ dummyRead)
public:
	KviUserListViewArea(KviUserListView * par);
	~KviUserListViewArea();
public:
	int dummyRead() const { return 0; };
protected:
	KviUserListView  * m_pListView;
	KviUserListEntry * m_pLastEntryUnderMouse;
	QScrollBar       * m_pScrollBar;
	int                m_iLastScrollBarVal;
	int                m_iTopItemOffset;
	bool               m_bIgnoreScrollBar;
protected:
	virtual void paintEvent(QPaintEvent *);
	virtual void resizeEvent(QResizeEvent *);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void mouseDoubleClickEvent(QMouseEvent *e);
	virtual void wheelEvent(QWheelEvent *e);
	virtual void keyPressEvent( QKeyEvent * e );
protected slots:
	void scrollBarMoved(int newVal);
};


#endif //!_KVI_USERLISTVIEW_H_
