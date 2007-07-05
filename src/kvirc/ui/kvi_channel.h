#ifndef _KVI_CHANNEL_H_
#define _KVI_CHANNEL_H_
//=============================================================================
//
//   File : kvi_channel.h
//   Creation date : Tue Aug 1 2000 01:42:00 by Szymon Stefanek
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


#include "kvi_console.h"
#include "kvi_window.h"
#include "kvi_string.h"
#include "kvi_ircuserdb.h"
#include "kvi_pixmap.h"
#include "kvi_userlistview.h"
#include "kvi_time.h"
#include "kvi_modew.h"
#include "kvi_valuelist.h"

#include <qdatetime.h>
#include "kvi_asciidict.h"
#include <qstringlist.h>

class KviConsole;
class QSplitter;
class QToolButton;
class KviTopicWidget;
class KviIrcMask;
class KviThemedLabel;
class KviTalHBox;
class KviModeWidget;

#ifdef COMPILE_ON_WINDOWS
	// windows compiler wants this instead of the forward decl
	#include "kvi_maskeditor.h"
#else
	typedef struct _KviMaskEntry KviMaskEntry; // kvi_maskeditor.h
#endif
class KviMaskEditor;
class KviModeEditor;

#define KVI_CHANNEL_STATE_HAVEALLNAMES 1
#define KVI_CHANNEL_STATE_HAVEBANLIST (1 << 1)
#define KVI_CHANNEL_STATE_HAVEWHOLIST (1 << 2)
#define KVI_CHANNEL_STATE_HAVEBANEXCEPTIONLIST (1 << 3)
#define KVI_CHANNEL_STATE_HAVEINVITELIST (1 << 4)
#define KVI_CHANNEL_STATE_DEADCHAN (1 << 5)

#define KVI_CHANNEL_STATE_SENTBANLISTREQUEST (1 << 6)
#define KVI_CHANNEL_STATE_SENTBANEXCEPTIONLISTREQUEST (1 << 7)
#define KVI_CHANNEL_STATE_SENTINVITELISTREQUEST (1 << 8)
#define KVI_CHANNEL_STATE_SENTWHOREQUEST (1 << 9)

#define KVI_CHANNEL_STATE_SENTPART (1 << 10)
#define KVI_CHANNEL_STATE_SYNCHRONIZED (1 << 11)

#define KVI_CHANNEL_STATE_NOCLOSEONPART (1 << 12)

#define KVI_CHANNEL_STATE_SENTSYNCWHOREQUEST (1 << 13)

typedef struct _KviChannelAction
{
	QString      szNick;       // action source nick
	unsigned int uActionType;  // type of the action
	kvi_time_t   tTime;        // time of the action
	int          iTemperature; // temperature of the action
} KviChannelAction;

// this is the maximum count of actions that we keep in memory
#define KVI_CHANNEL_ACTION_HISTORY_MAX_COUNT 40
// this is the timespan of the oldest action that we keep in memory
// 600 secs = 10 mins
#define KVI_CHANNEL_ACTION_HISTORY_MAX_TIMESPAN 600

typedef struct _KviChannelActivityStats
{
	unsigned int        uActionCount;              // number of actions in the history
	bool                bStatsInaccurate;          // the stats are inaccurate because we have just joined the chan
	unsigned int        uLastActionTimeSpan;       // the timespan between the last action and now
	unsigned int        uFirstActionTimeSpan;      // the time span between the first and the last action
	double              dActionsPerMinute;         // average number of actions per minute in the lastActionTimeSpan
	unsigned int        uActionsInTheLastMinute;   // number of actions in the last minute
	int                 iAverageActionTemperature; // the average chan temperature
	unsigned int        uHotActionCount;
	unsigned int        uHotActionPercent;
	QStringList         lTalkingUsers;             // users that seem to be talking NOW
	QStringList         lWereTalkingUsers;
} KviChannelActivityStats;

class KVIRC_API KviChannel : public KviWindow
{
	Q_OBJECT
public:
	KviChannel(KviFrame * lpFrm,KviConsole * lpConsole,const char * name);
	~KviChannel();
protected:
	QSplitter                           * m_pTopSplitter;
	QSplitter                           * m_pVertSplitter;
	BUTTON_CLASS                        * m_pDoubleViewButton;
	KviWindowToolPageButton             * m_pListViewButton;
	KviWindowToolPageButton             * m_pBanEditorButton;
	KviWindowToolPageButton             * m_pBanExceptionEditorButton;
	KviWindowToolPageButton             * m_pInviteEditorButton;
	KviWindowToolPageButton             * m_pModeEditorButton;
	KviMaskEditor                       * m_pBanEditor;
	KviMaskEditor                       * m_pBanExceptionEditor;
	KviMaskEditor                       * m_pInviteEditor;
	KviModeEditor                       * m_pModeEditor;
	KviIrcView                          * m_pMessageView;
	KviTopicWidget                      * m_pTopicWidget;
	KviUserListView                     * m_pUserListView;
	KviModeWidget                       * m_pModeWidget;
	int                                   m_iStateFlags;
	QString                               m_szChannelMode;
	QString                               m_szChannelKey;
	KviStr                                m_szChannelLimit;
	KviPtrList<KviMaskEntry>            * m_pBanList;
	KviPtrList<KviMaskEntry>            * m_pBanExceptionList;
	KviPtrList<KviMaskEntry>            * m_pInviteList;
	KviPixmap                             m_privateBackground;
	QDateTime                             m_joinTime;
	QString                               m_szNameWithUserFlag;
	KviAsciiDict<QString>               * m_pTmpHighLighted;
	unsigned int                          m_uActionHistoryHotActionCount;
	KviPtrList<KviChannelAction>        * m_pActionHistory;
	kvi_time_t                            m_tLastReceivedWhoReply;
	KviValueList<int>                     m_VertSplitterSizesList;
	KviTalHBox                          * m_pButtonContainer;
protected:
	bool eventFilter(QObject *, QEvent *);
	virtual QPixmap * myIconPtr();
	virtual void fillCaptionBuffers();
	virtual void resizeEvent(QResizeEvent *e);
	virtual void closeEvent(QCloseEvent *e);
//	virtual void fillContextPopup(KviTalPopupMenu * p);
	virtual void getConfigGroupName(QString &buf);
	virtual void saveProperties(KviConfig * cfg);
	virtual void loadProperties(KviConfig * cfg);
	virtual void applyOptions();
	virtual void getBaseLogFileName(QString &buffer);
	virtual void triggerCreationEvents();
	void toggleEditor(KviMaskEditor ** ppEd,KviWindowToolPageButton ** ppBtn,KviPtrList<KviMaskEntry> *l,char flag,const char *edName);
	void internalMask(const QString &mask,bool bAdd,const QString &setBy,unsigned int setAt,KviPtrList<KviMaskEntry> *l,KviMaskEditor **ppEd);
	void checkChannelSync();
	void showDoubleView(bool bShow);
	void fixActionHistory();
	void getTalkingUsersStats(QString &buffer,QStringList &l,bool bPast);
public:
	void getChannelActivityStats(KviChannelActivityStats * s);
	//void getChannelActivityStatsDescription(QString &buffer);

	KviPtrList<KviMaskEntry> * banList(){ return m_pBanList; };
	KviPtrList<KviMaskEntry> * banExceptionList(){ return m_pBanExceptionList; };
	KviPtrList<KviMaskEntry> * inviteList(){ return m_pInviteList; };

	QString * firstSelectedNickname(){ return m_pUserListView->firstSelectedNickname(); };
	QString * nextSelectedNickname(){ return m_pUserListView->nextSelectedNickname(); };

	int selectedCount(){ return m_pUserListView->selectedCount(); };
	int opCount(){ return m_pUserListView->opCount(); };
	int voiceCount(){ return m_pUserListView->voiceCount(); };
	int halfOpCount(){ return m_pUserListView->halfOpCount(); };
	int userOpCount(){ return m_pUserListView->userOpCount(); };
	int chanAdminCount(){ return m_pUserListView->chanAdminCount(); };
	int chanOwnerCount(){ return m_pUserListView->chanOwnerCount(); };
	unsigned int count(){ return m_pUserListView->count(); };

	unsigned int banCount(){ return m_pBanList->count(); };
	unsigned int banExceptionCount(){ return m_pBanExceptionList->count(); };
	unsigned int inviteCount(){ return m_pInviteList->count(); };

	const QString & nameWithUserFlag(){ return m_szNameWithUserFlag; };
	virtual const QString & target(){ return windowName(); };
//	void appendSelectedNicknames(KviStr &buffer);
	void setMask(char flag, const QString &mask,bool bAdd,const QString &setBy,unsigned int setAt);

	kvi_time_t lastReceivedWhoReply(){ return m_tLastReceivedWhoReply; };
	void setLastReceivedWhoReply(kvi_time_t tTime){ m_tLastReceivedWhoReply = tTime; };
	void setSentSyncWhoRequest(){ m_iStateFlags |= KVI_CHANNEL_STATE_SENTSYNCWHOREQUEST; };
	void clearSentSyncWhoRequest(){ m_iStateFlags ^= KVI_CHANNEL_STATE_SENTSYNCWHOREQUEST; };
	bool sentSyncWhoRequest(){ return (m_iStateFlags & KVI_CHANNEL_STATE_SENTSYNCWHOREQUEST); };

	bool sentWhoRequest(){ return (m_iStateFlags & KVI_CHANNEL_STATE_SENTWHOREQUEST); };
	void setSentWhoRequest(){ m_iStateFlags |= KVI_CHANNEL_STATE_SENTWHOREQUEST; };
	bool sentInviteListRequest(){ return (m_iStateFlags & KVI_CHANNEL_STATE_SENTINVITELISTREQUEST); };
	void setSentInviteListRequest(){ m_iStateFlags |= KVI_CHANNEL_STATE_SENTINVITELISTREQUEST; };
	void setInviteListDone(){ m_iStateFlags ^= KVI_CHANNEL_STATE_SENTINVITELISTREQUEST; };
	bool sentBanListRequest(){ return (m_iStateFlags & KVI_CHANNEL_STATE_SENTBANLISTREQUEST); };
	void setSentBanListRequest(){ m_iStateFlags |= KVI_CHANNEL_STATE_SENTBANLISTREQUEST; };
	void setBanListDone(){ m_iStateFlags ^= KVI_CHANNEL_STATE_SENTBANLISTREQUEST; };
	bool sentBanExceptionListRequest(){ return (m_iStateFlags & KVI_CHANNEL_STATE_SENTBANEXCEPTIONLISTREQUEST); };
	void setSentBanExceptionListRequest(){ m_iStateFlags |= KVI_CHANNEL_STATE_SENTBANEXCEPTIONLISTREQUEST; };
	void setBanExceptionListDone(){ m_iStateFlags ^= KVI_CHANNEL_STATE_SENTBANEXCEPTIONLISTREQUEST; };

	bool hasAllNames(){ return (m_iStateFlags & KVI_CHANNEL_STATE_HAVEALLNAMES); };
	void setHasAllNames(){ m_iStateFlags |= KVI_CHANNEL_STATE_HAVEALLNAMES; checkChannelSync(); };
	bool hasInviteList(){ return (m_iStateFlags & KVI_CHANNEL_STATE_HAVEINVITELIST); checkChannelSync(); };
	void setHasInviteList(){ m_iStateFlags |= KVI_CHANNEL_STATE_HAVEINVITELIST; };
	bool hasWhoList(){ return (m_iStateFlags & KVI_CHANNEL_STATE_HAVEWHOLIST); };
	void setHasWhoList(){ m_iStateFlags |= KVI_CHANNEL_STATE_HAVEWHOLIST; checkChannelSync(); };
	bool hasBanList(){ return (m_iStateFlags & KVI_CHANNEL_STATE_HAVEBANLIST); };
	void setHasBanList(){ m_iStateFlags |= KVI_CHANNEL_STATE_HAVEBANLIST; checkChannelSync(); };
	bool hasBanExceptionList(){ return (m_iStateFlags & KVI_CHANNEL_STATE_HAVEBANEXCEPTIONLIST); };
	void setHasBanExceptionList(){ m_iStateFlags |= KVI_CHANNEL_STATE_HAVEBANEXCEPTIONLIST; checkChannelSync(); };

	bool closeOnPart(){ return !(m_iStateFlags & KVI_CHANNEL_STATE_NOCLOSEONPART); };
	void partMessageSent(bool bCloseOnPart = true,bool bShowMessage = true);
	
	virtual bool activityMeter(unsigned int * puActivityValue,unsigned int * puActivityTemperature);

	void setDeadChan();
	void setAliveChan();
	void prependUserFlag(const QString &nick,QString &buffer){ m_pUserListView->prependUserFlag(nick,buffer); };
	char getUserFlag(const QString &nick){ return m_pUserListView->getUserFlag(nick); };
	bool isDeadChan(){ return (m_iStateFlags & KVI_CHANNEL_STATE_DEADCHAN); };
	virtual QSize sizeHint() const;
	void enableUserListUpdates(bool bEnable){ m_pUserListView->enableUpdates(bEnable); };
	KviUserListEntry * join(const QString &nick,const QString &user = QString::null,const QString &host = QString::null,int iFlags = 0)
		{ return m_pUserListView->join(nick,user,host,iFlags); };
	bool op(const QString &nick,bool bOp){ return m_pUserListView->op(nick,bOp); };
	bool avatarChanged(const QString &nick){ return m_pUserListView->avatarChanged(nick); };
	bool voice(const QString &nick,bool bVoice){ return m_pUserListView->voice(nick,bVoice); };
	bool halfop(const QString &nick,bool bHalfOp){ return m_pUserListView->halfop(nick,bHalfOp); };
	bool userop(const QString &nick,bool bUserOp){ return m_pUserListView->userop(nick,bUserOp); };
	bool setChanOwner(const QString &nick,bool bChanOwner){ return m_pUserListView->setChanOwner(nick,bChanOwner); };
	bool setChanAdmin(const QString &nick,bool bChanAdmin){ return m_pUserListView->setChanAdmin(nick,bChanAdmin); };
	void userAction(KviIrcMask * user,unsigned int uActionType);
	void userAction(const QString &nick,unsigned int uActionType);
	void userAction(const QString &nick,const QString &user,const QString &host,unsigned int uActionType);
	bool nickChange(const QString &oldNick,const QString &newNick);
	void channelAction(const QString &nick,unsigned int uActionType,int iTemperature);
	bool part(const QString &nick);
	bool isOn(const QString &nick){ return (m_pUserListView->findEntry(nick) != 0); };
	KviUserListEntry * findEntry(const QString &nick){ return m_pUserListView->findEntry(nick); };
	KviUserListView * userListView(){ return m_pUserListView; };
	bool isMeOp(bool bAtLeast = false);
	bool isMeChanOwner(bool bAtLeast = false);
	bool isMeChanAdmin(bool bAtLeast = false);
	bool isMeVoice(bool bAtLeast = false);
	bool isMeHalfOp(bool bAtLeast = false);
	bool isMeUserOp(bool bAtLeast = false);
	bool isOp(const QString &nick,bool bAtLeast = false){ return m_pUserListView->isOp(nick,bAtLeast); };
	bool isVoice(const QString &nick,bool bAtLeast = false){ return m_pUserListView->isVoice(nick,bAtLeast); };
	bool isHalfOp(const QString &nick,bool bAtLeast = false){ return m_pUserListView->isHalfOp(nick,bAtLeast); };
	bool isUserOp(const QString &nick,bool bAtLeast = false){ return m_pUserListView->isUserOp(nick,bAtLeast); };
	bool isChanOwner(const QString &nick,bool bAtLeast = false){ return m_pUserListView->isChanOwner(nick,bAtLeast); };
	bool isChanAdmin(const QString &nick,bool bAtLeast = false){ return m_pUserListView->isChanAdmin(nick,bAtLeast); };
	int myFlags();
	void updateModeLabel();
	KviTopicWidget * topicWidget(){ return m_pTopicWidget; };
	virtual void outputMessage(int msg_type,const QString &msg);
	void ownMessage(const QString &buffer);
	void ownAction(const QString &buffer);
	void setChannelMode(char  mode,bool bAdd);
	void setChannelKey(const char * key);
	void setChannelLimit(const char * limit);

	void getChannelModeString(QString &buffer);

	KviStr & channelLimit(){ return m_szChannelLimit; };
	bool hasChannelLimit(){ return m_szChannelLimit.hasData(); };

	bool hasChannelKey() { return !m_szChannelKey.isEmpty(); };
	QString & channelKey(){ return m_szChannelKey; };

    void addHighlightedUser(const char * nick);
    void removeHighlightedUser(const char * nick);
    bool isHighlightedUser(const char * nick) { return m_pTmpHighLighted->find(nick); };
    KviIrcView * messageView() const { return m_pMessageView; };
	virtual void lostUserFocus();
	virtual void getTaskBarTipText(QString &buffer);
	QString channelMode() { return m_szChannelMode; };

	void unhighlight();
	
	QFrame * buttonContainer() { return (QFrame*)m_pButtonContainer; };
private slots:
	void toggleDoubleView();
	void toggleListView();
	void toggleBanEditor();
	void toggleBanExceptionEditor();
	void toggleInviteEditor();
	void toggleModeEditor();
	void topicSelected(const QString & topic);
	void setMode(const char * mode);
	void modeSelectorDone();
	void textViewRightClicked();
	void removeMasks(KviMaskEditor *ed,KviPtrList<KviMaskEntry> *l);
	void toggleToolButtons();
protected:
	virtual void preprocessMessage(QString & szMessage);
};

#endif //_KVI_CHANNEL_H_
