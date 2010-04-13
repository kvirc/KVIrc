#ifndef _KVI_CHANNEL_H_
#define _KVI_CHANNEL_H_
//=============================================================================
//
//   File : kvi_channel.h
//   Creation date : Tue Aug  1 2000 01:42:00 by Szymon Stefanek
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

/**
* \file kvi_channel.h
* \author Szymon Stefanek
* \brief Channel widget: abstraction of an IRC channel
*/

#include "kvi_settings.h"
#include "kvi_console.h"
#include "kvi_window.h"
#include "kvi_string.h"
#include "kvi_ircuserdb.h"
#include "kvi_pixmap.h"
#include "kvi_userlistview.h"
#include "kvi_time.h"
#include "kvi_modew.h"
#include "kvi_pointerhashtable.h"

#include <QList>
#include <QDateTime>
#include <QStringList>
#include <QToolButton>


class KviConsole;
class KviTopicWidget;
class KviIrcMask;
class KviThemedLabel;
class KviTalHBox;
class KviMaskEditor;
class KviModeEditor;

#ifdef COMPILE_ON_WINDOWS
	// windows compiler wants this instead of the forward decl
	#include "kvi_maskeditor.h"
#else
	typedef struct _KviMaskEntry KviMaskEntry; // kvi_maskeditor.h
#endif

/**
* \def KVI_CHANNEL_STATE_HAVEALLNAMES Flag for "have all names"
* \def KVI_CHANNEL_STATE_HAVEBANLIST Flag for "have ban list"
* \def KVI_CHANNEL_STATE_HAVEWHOLIST Flag for "have WHO list"
* \def KVI_CHANNEL_STATE_HAVEBANEXCEPTIONLIST Flag for "have ban exception list"
* \def KVI_CHANNEL_STATE_HAVEINVITELIST Flag for "have invite list"
* \def KVI_CHANNEL_STATE_HAVEQUIETBANLIST Flag for "have quiet ban list"
* \def KVI_CHANNEL_STATE_DEADCHAN Flag for "dead channel"
* \def KVI_CHANNEL_STATE_SENTBANLISTREQUEST Flag to set ban list request
* \def KVI_CHANNEL_STATE_SENTBANEXCEPTIONLISTREQUEST Flag to set ban exception list request
* \def KVI_CHANNEL_STATE_SENTINVITELISTREQUEST Flag to set invite list request
* \def KVI_CHANNEL_STATE_SENTQUIETBANLISTREQUEST Flag to set ban list request
* \def KVI_CHANNEL_STATE_SENTWHOREQUEST Flag to set WHO request
* \def KVI_CHANNEL_STATE_SENTPART Flag to set PART request
* \def KVI_CHANNEL_STATE_SYNCHRONIZED Flag to set SYNC request
* \def KVI_CHANNEL_STATE_NOCLOSEONPART Flag to set no close on part
* \def KVI_CHANNEL_STATE_SENTSYNCWHOREQUEST Flag for SYNC request
*/
#define KVI_CHANNEL_STATE_HAVEALLNAMES 1
#define KVI_CHANNEL_STATE_HAVEBANLIST (1 << 1)
#define KVI_CHANNEL_STATE_HAVEWHOLIST (1 << 2)
#define KVI_CHANNEL_STATE_HAVEBANEXCEPTIONLIST (1 << 3)
#define KVI_CHANNEL_STATE_HAVEINVITELIST (1 << 4)
#define KVI_CHANNEL_STATE_HAVEQUIETBANLIST (1 << 5)
#define KVI_CHANNEL_STATE_DEADCHAN (1 << 6)
#define KVI_CHANNEL_STATE_SENTBANLISTREQUEST (1 << 7)
#define KVI_CHANNEL_STATE_SENTBANEXCEPTIONLISTREQUEST (1 << 8)
#define KVI_CHANNEL_STATE_SENTINVITELISTREQUEST (1 << 9)
#define KVI_CHANNEL_STATE_SENTQUIETBANLISTREQUEST (1 << 10)
#define KVI_CHANNEL_STATE_SENTWHOREQUEST (1 << 11)
#define KVI_CHANNEL_STATE_SENTPART (1 << 12)
#define KVI_CHANNEL_STATE_SYNCHRONIZED (1 << 13)
#define KVI_CHANNEL_STATE_NOCLOSEONPART (1 << 14)
#define KVI_CHANNEL_STATE_SENTSYNCWHOREQUEST (1 << 15)

/**
* \def KVI_CHANACTIVITY_LIMIT_ICE The limit to be "ice"
* \def KVI_CHANACTIVITY_LIMIT_VERYCOLD The limit to be "very cold"
* \def KVI_CHANACTIVITY_LIMIT_COLD The limit to be "cold"
* \def KVI_CHANACTIVITY_LIMIT_UNDEFINED The limit to be "undefined"
* \def KVI_CHANACTIVITY_LIMIT_HOT The limit to be "hot"
* \def KVI_CHANACTIVITY_LIMIT_VERYHOT The limit to be "very hot"
*/
#define KVI_CHANACTIVITY_LIMIT_ICE 5
#define KVI_CHANACTIVITY_LIMIT_VERYCOLD 10
#define KVI_CHANACTIVITY_LIMIT_COLD 20
#define KVI_CHANACTIVITY_LIMIT_UNDEFINED 30
#define KVI_CHANACTIVITY_LIMIT_HOT 50
#define KVI_CHANACTIVITY_LIMIT_VERYHOT 70

/**
* \def AVERAGE_CHANNEL_USERS The average channel users of a channel
*/
#ifndef AVERAGE_CHANNEL_USERS
	#define AVERAGE_CHANNEL_USERS 101
#endif

/**
* \def KVI_CHANNEL_ACTION_HISTORY_MAX_COUNT Maximum actions count we keep in memory
* \def KVI_CHANNEL_ACTION_HISTORY_MAX_TIMESPAN Timespan of the oldest action we keep in memory (600 secs = 10 mins)
*/
#define KVI_CHANNEL_ACTION_HISTORY_MAX_COUNT 40
#define KVI_CHANNEL_ACTION_HISTORY_MAX_TIMESPAN 600

/**
* \typedef KviChannelAction
* \struct _KviChannelAction
* \brief A struct which holds the channel actions
*/
typedef struct _KviChannelAction
{
	QString      szNick;       // action source nick
	unsigned int uActionType;  // type of the action
	kvi_time_t   tTime;        // time of the action
	int          iTemperature; // temperature of the action
} KviChannelAction;

/**
* \typedef KviChannelActivityStats
* \struct _KviChannelActivityStats
* \brief A struct which holds the activity stats
*/
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

/**
* \class KviChannel
* \brief The class which manages a channel
*/
class KVIRC_API KviChannel : public KviWindow
{
	Q_OBJECT
public:
	/**
	* \brief Constructs the channel object
	* \param lpFrm The parent frame object
	* \param lpConsole The console of the context
	* \param szName The name of the channel
	* \return KviChannel
	*/
	KviChannel(KviFrame * lpFrm, KviConsole * lpConsole, const QString & szName);

	/**
	* \brief Destroys the channel object
	*/
	~KviChannel();
protected:
	KviTalSplitter                       * m_pTopSplitter;
	KviTalSplitter                       * m_pVertSplitter;
	QToolButton                          * m_pDoubleViewButton;
	KviWindowToolPageButton              * m_pListViewButton;
	KviWindowToolPageButton              * m_pBanEditorButton;
	KviWindowToolPageButton              * m_pBanExceptionEditorButton;
	KviWindowToolPageButton              * m_pInviteEditorButton;
	KviWindowToolPageButton              * m_pQuietBanEditorButton;
	KviWindowToolPageButton              * m_pModeEditorButton;
	KviMaskEditor                        * m_pBanEditor;
	KviMaskEditor                        * m_pBanExceptionEditor;
	KviMaskEditor                        * m_pInviteEditor;
	KviMaskEditor                        * m_pQuietBanEditor;
	KviModeEditor                        * m_pModeEditor;
	KviIrcView                           * m_pMessageView;
	KviTopicWidget                       * m_pTopicWidget;
	KviUserListView                      * m_pUserListView;
	KviModeWidget                        * m_pModeWidget;
	int                                    m_iStateFlags;
	QString                                m_szChannelMode;
	QString                                m_szChannelKey;
	QString                                m_szChannelLimit;
	KviPointerList<KviMaskEntry>         * m_pBanList;
	KviPointerList<KviMaskEntry>         * m_pBanExceptionList;
	KviPointerList<KviMaskEntry>         * m_pInviteList;
	KviPointerList<KviMaskEntry>         * m_pQuietBanList;
	KviPixmap                              m_privateBackground;
	QDateTime                              m_joinTime;
	QString                                m_szNameWithUserFlag;
	QStringList                          * m_pTmpHighLighted;
	unsigned int                           m_uActionHistoryHotActionCount;
	KviPointerList<KviChannelAction>     * m_pActionHistory;
	kvi_time_t                             m_tLastReceivedWhoReply;
	QList<int>                             m_VertSplitterSizesList;
	KviTalHBox                           * m_pButtonContainer;
public:
	/**
	* \brief Returns the user listview object
	* \return KviUserListView *
	*/
	KviUserListView * userListView(){ return m_pUserListView; };

	/**
	* \brief Returns the topic widget object
	* \return KviTopicWidget *
	*/
	KviTopicWidget * topicWidget(){ return m_pTopicWidget; };

	/**
	* \brief Returns the irc view object
	* \return KviIrcView *
	*/
	KviIrcView * messageView() const { return m_pMessageView; };

	/**
	* \brief Returns the button container object
	* \return QFrame *
	*/
	QFrame * buttonContainer() { return (QFrame*)m_pButtonContainer; };

	/**
	* \brief Returns the list of bans set
	* \return KviPointerList<KviMaskEntry> *
	*/
	KviPointerList<KviMaskEntry> * banList(){ return m_pBanList; };

	/**
	* \brief Returns the list of ban exceptions set
	* \return KviPointerList<KviMaskEntry> *
	*/
	KviPointerList<KviMaskEntry> * banExceptionList(){ return m_pBanExceptionList; };

	/**
	* \brief Returns the list of invites set
	* \return KviPointerList<KviMaskEntry> *
	*/
	KviPointerList<KviMaskEntry> * inviteList(){ return m_pInviteList; };

	/**
	* \brief Returns the list of quiet bans set
	* \return KviPointerList<KviMaskEntry> *
	*/
	KviPointerList<KviMaskEntry> * quietBanList(){ return m_pQuietBanList; };

	/**
	* \brief Returns the first selected nickname in the userlist
	* \return QString *
	*/
	QString * firstSelectedNickname(){ return m_pUserListView->firstSelectedNickname(); };

	/**
	* \brief Returns the next selected nickname in the userlist
	* \return QString *
	*/
	QString * nextSelectedNickname(){ return m_pUserListView->nextSelectedNickname(); };

	/**
	* \brief Returns the name of the channel
	* \return const QString &
	*/
	virtual const QString & target(){ return windowName(); };

	/**
	* \brief Returns the name of the channel with user flags
	* \return const QString &
	*/
	const QString & nameWithUserFlag(){ return m_szNameWithUserFlag; };

	/**
	* \brief Gets the channel activity stats and put them in the buffer
	* \param s The buffer where to store the data
	* \return void
	*/
	void getChannelActivityStats(KviChannelActivityStats * s);

	/**
	* \brief Returns the number of selected users
	* \return int
	*/
	int selectedCount(){ return m_pUserListView->selectedCount(); };

	/**
	* \brief Returns the number of users with chanowner status
	* \return int
	*/
	int chanOwnerCount(){ return m_pUserListView->chanOwnerCount(); };

	/**
	* \brief Returns the number of users with chanadmin status
	* \return int
	*/
	int chanAdminCount(){ return m_pUserListView->chanAdminCount(); };

	/**
	* \brief Returns the number of users with op status
	* \return int
	*/
	int opCount(){ return m_pUserListView->opCount(); };

	/**
	* \brief Returns the number of users with halfop status
	* \return int
	*/
	int halfOpCount(){ return m_pUserListView->halfOpCount(); };

	/**
	* \brief Returns the number of users with voice status
	* \return int
	*/
	int voiceCount(){ return m_pUserListView->voiceCount(); };

	/**
	* \brief Returns the number of users with userop status
	* \return int
	*/
	int userOpCount(){ return m_pUserListView->userOpCount(); };

	/**
	* \brief Returns the number of users
	* \return unsigned int
	*/
	unsigned int count(){ return m_pUserListView->count(); };

	/**
	* \brief Returns the number of ban masks
	* \return unsigned int
	*/
	unsigned int banCount(){ return m_pBanList->count(); };

	/**
	* \brief Returns the number of ban exceptions masks list
	* \return unsigned int
	*/
	unsigned int banExceptionCount(){ return m_pBanExceptionList->count(); };

	/**
	* \brief Returns the number of invite masks
	* \return unsigned int
	*/
	unsigned int inviteCount(){ return m_pInviteList->count(); };

	/**
	* \brief Returns the number of quiet ban masks
	* \return unsigned int
	*/
	unsigned int quietBanCount(){ return m_pQuietBanList->count(); };

	/**
	* \brief Called when someone sets a mask in the channel's lists
	*
	* The type of the mask can be:
	*  - b: ban
	*  - e: ban exception
	*  - I: invite
	*  - q: quiet ban
	* \param cFlag The type of the mask
	* \param szMask The mask set
	* \param bAdd Whether to add or remove the mask
	* \param szSetBy Who set the mask
	* \param uSetAt The datetime when the mask was set
	* \return void
	*/
	void setMask(char cFlag, const QString & szMask, bool bAdd, const QString & szSetBy, unsigned int uSetAt);

	/**
	* \brief Returns the time of the last received WHO reply
	* \return kvi_time_t
	*/
	kvi_time_t lastReceivedWhoReply(){ return m_tLastReceivedWhoReply; };

	/**
	* \brief Sets the time of the last received WHO reply
	* \param tTime The source time
	* \return void
	*/
	void setLastReceivedWhoReply(kvi_time_t tTime){ m_tLastReceivedWhoReply = tTime; };

	/**
	* \brief Returns true if we have sent the sync WHO request
	* \return bool
	*/
	bool sentSyncWhoRequest(){ return (m_iStateFlags & KVI_CHANNEL_STATE_SENTSYNCWHOREQUEST); };

	/**
	* \brief Sets the sync WHO request flag
	* \return void
	*/
	void setSentSyncWhoRequest(){ m_iStateFlags |= KVI_CHANNEL_STATE_SENTSYNCWHOREQUEST; };

	/**
	* \brief Clears the sync WHO request flag
	* \return void
	*/
	void clearSentSyncWhoRequest(){ m_iStateFlags ^= KVI_CHANNEL_STATE_SENTSYNCWHOREQUEST; };

	/**
	* \brief Returns true if we have sent the WHO request
	* \return bool
	*/
	bool sentWhoRequest(){ return (m_iStateFlags & KVI_CHANNEL_STATE_SENTWHOREQUEST); };

	/**
	* \brief Sets the WHO request flag
	* \return void
	*/
	void setSentWhoRequest(){ m_iStateFlags |= KVI_CHANNEL_STATE_SENTWHOREQUEST; };

	/**
	* \brief Returns true if we have sent the invite list request
	* \return bool
	*/
	bool sentInviteListRequest(){ return (m_iStateFlags & KVI_CHANNEL_STATE_SENTINVITELISTREQUEST); };

	/**
	* \brief Sets the invite request flag
	* \return void
	*/
	void setSentInviteListRequest(){ m_iStateFlags |= KVI_CHANNEL_STATE_SENTINVITELISTREQUEST; };

	/**
	* \brief Clears the invite request flag
	* \return void
	*/
	void setInviteListDone(){ m_iStateFlags ^= KVI_CHANNEL_STATE_SENTINVITELISTREQUEST; };

	/**
	* \brief Returns true if we have sent the ban list request
	* \return bool
	*/
	bool sentBanListRequest(){ return (m_iStateFlags & KVI_CHANNEL_STATE_SENTBANLISTREQUEST); };

	/**
	* \brief Sets the ban request flag
	* \return void
	*/
	void setSentBanListRequest(){ m_iStateFlags |= KVI_CHANNEL_STATE_SENTBANLISTREQUEST; };

	/**
	* \brief Clears the ban request flag
	* \return void
	*/
	void setBanListDone(){ m_iStateFlags ^= KVI_CHANNEL_STATE_SENTBANLISTREQUEST; };

	/**
	* \brief Returns true if we have sent the ban exception list request
	* \return bool
	*/
	bool sentBanExceptionListRequest(){ return (m_iStateFlags & KVI_CHANNEL_STATE_SENTBANEXCEPTIONLISTREQUEST); };

	/**
	* \brief Sets the ban exception request flag
	* \return void
	*/
	void setSentBanExceptionListRequest(){ m_iStateFlags |= KVI_CHANNEL_STATE_SENTBANEXCEPTIONLISTREQUEST; };

	/**
	* \brief Clears the ban exception request flag
	* \return void
	*/
	void setBanExceptionListDone(){ m_iStateFlags ^= KVI_CHANNEL_STATE_SENTBANEXCEPTIONLISTREQUEST; };

	/**
	* \brief Returns true if we have sent the quiet ban list request
	* \return bool
	*/
	bool sentQuietBanListRequest(){ return (m_iStateFlags & KVI_CHANNEL_STATE_SENTQUIETBANLISTREQUEST); };

	/**
	* \brief Sets the quiet ban request flag
	* \return void
	*/
	void setSentQuietBanListRequest(){ m_iStateFlags |= KVI_CHANNEL_STATE_SENTQUIETBANLISTREQUEST; };

	/**
	* \brief Clears the quiet ban request flag
	* \return void
	*/
	void setQuietBanListDone(){ m_iStateFlags ^= KVI_CHANNEL_STATE_SENTQUIETBANLISTREQUEST; };

	/**
	* \brief Returns true if the channel has all names
	* \return bool
	*/
	bool hasAllNames(){ return (m_iStateFlags & KVI_CHANNEL_STATE_HAVEALLNAMES); };

	/**
	* \brief Sets the existance of all names
	* \return void
	*/
	void setHasAllNames(){ m_iStateFlags |= KVI_CHANNEL_STATE_HAVEALLNAMES; checkChannelSync(); };

	/**
	* \brief Returns true if the channel has an invite list
	* \return bool
	*/
	bool hasInviteList(){ return (m_iStateFlags & KVI_CHANNEL_STATE_HAVEINVITELIST); checkChannelSync(); };

	/**
	* \brief Sets the existance of the invite list
	* \return void
	*/
	void setHasInviteList(){ m_iStateFlags |= KVI_CHANNEL_STATE_HAVEINVITELIST; };

	/**
	* \brief Returns true if the channel has a WHO list
	* \return bool
	*/
	bool hasWhoList(){ return (m_iStateFlags & KVI_CHANNEL_STATE_HAVEWHOLIST); };

	/**
	* \brief Sets the existance of the WHO list
	* \return void
	*/
	void setHasWhoList(){ m_iStateFlags |= KVI_CHANNEL_STATE_HAVEWHOLIST; checkChannelSync(); };

	/**
	* \brief Returns true if the channel has a ban list
	* \return bool
	*/
	bool hasBanList(){ return (m_iStateFlags & KVI_CHANNEL_STATE_HAVEBANLIST); };

	/**
	* \brief Sets the existance of the ban list
	* \return void
	*/
	void setHasBanList(){ m_iStateFlags |= KVI_CHANNEL_STATE_HAVEBANLIST; checkChannelSync(); };

	/**
	* \brief Returns true if the channel has a ban exception list
	* \return bool
	*/
	bool hasBanExceptionList(){ return (m_iStateFlags & KVI_CHANNEL_STATE_HAVEBANEXCEPTIONLIST); };

	/**
	* \brief Sets the existance of the ban exception list
	* \return void
	*/
	void setHasBanExceptionList(){ m_iStateFlags |= KVI_CHANNEL_STATE_HAVEBANEXCEPTIONLIST; checkChannelSync(); };

	/**
	* \brief Returns true if the channel has a quiet ban list
	* \return bool
	*/
	bool hasQuietBanList(){ return (m_iStateFlags & KVI_CHANNEL_STATE_HAVEQUIETBANLIST); };

	/**
	* \brief Sets the existance of the quiet ban list
	* \return void
	*/
	void setHasQuietBanList(){ m_iStateFlags |= KVI_CHANNEL_STATE_HAVEQUIETBANLIST; checkChannelSync(); };

	/**
	* \brief Returns true if the channel has to be closed on part
	* \return bool
	*/
	bool closeOnPart(){ return !(m_iStateFlags & KVI_CHANNEL_STATE_NOCLOSEONPART); };

	/**
	* \brief Called when we want to part a channel
	* \param bCloseOnPart Whether to leave channel open after part
	* \param bShowMessage Whether to show part message
	* \return void
	*/
	void partMessageSent(bool bCloseOnPart = true, bool bShowMessage = true);

	/**
	* \brief Colors the icon who tells how much activity there is
	* \param puActivityValue The value of the activity
	* \param puActivityTemperature The temperature of the activity
	* \return bool
	*/
	virtual bool activityMeter(unsigned int * puActivityValue, unsigned int * puActivityTemperature);

	/**
	* \brief Sets the channel as dead
	* \return void
	*/
	void setDeadChan();

	/**
	* \brief Returns true if the channel is dead
	* \return bool
	*/
	bool isDeadChan(){ return (m_iStateFlags & KVI_CHANNEL_STATE_DEADCHAN); };

	/**
	* \brief Sets the channel as alive
	* \return void
	*/
	void setAliveChan();

	/**
	* \brief Prepends the user flag to the nickname
	* \param szNick The nickname of the user
	* \param szBuffer The buffer :)
	* \return void
	*/
	void prependUserFlag(const QString & szNick, QString & szBuffer){ m_pUserListView->prependUserFlag(szNick,szBuffer); };

	/**
	* \brief Returns the flag of a user
	* \param szNick The nick of the user
	* \return char
	*/
	char getUserFlag(const QString & szNick){ return m_pUserListView->getUserFlag(szNick); };

	/**
	* \brief Returns the size of the channel
	* \return QSize
	*/
	virtual QSize sizeHint() const;

	/**
	* \brief Enables or disable the userlist updates
	* \param bEnable Whether to enable the updates
	* \return void
	*/
	void enableUserListUpdates(bool bEnable){ m_pUserListView->enableUpdates(bEnable); };

	/**
	* \brief Called when a user joins the channel
	* \param szNick The nickname of the user
	* \param szUser The username of the user
	* \param szHost The hostname of the user
	* \param iFlags The flags of the user
	* \return KviUserListEntry *
	*/
	KviUserListEntry * join(const QString & szNick, const QString & szUser = QString(), const QString & szHost = QString(), int iFlags = 0){ return m_pUserListView->join(szNick,szUser,szHost,iFlags); };

	/**
	* \brief Returns true if the avatar of a user is changed
	* \param szNick The nickname of the user
	* \return bool
	*/
	bool avatarChanged(const QString & szNick){ return m_pUserListView->avatarChanged(szNick); };

	/**
	* \brief Sets the chan owner mode
	* \param szNick The nick to set the mode on
	* \param bChanOwner Whether to set or unset the mode on the user
	* \return bool
	*/
	bool setChanOwner(const QString & szNick, bool bChanOwner){ return m_pUserListView->setChanOwner(szNick,bChanOwner); };

	/**
	* \brief Sets the chan admin mode
	* \param szNick The nick to set the mode on
	* \param bChanAdmin Whether to set or unset the mode on the user
	* \return bool
	*/
	bool setChanAdmin(const QString & szNick, bool bChanAdmin){ return m_pUserListView->setChanAdmin(szNick,bChanAdmin); };

	/**
	* \brief Sets the operator mode
	* \param szNick The nick to set the mode on
	* \param bOp Whether to set or unset the mode on the user
	* \return bool
	*/
	bool setOp(const QString & szNick, bool bOp){ return m_pUserListView->setOp(szNick,bOp); };

	/**
	* \brief Sets the half operator mode
	* \param szNick The nick to set the mode on
	* \param bHalfOp Whether to set or unset the mode on the user
	* \return bool
	*/
	bool setHalfOp(const QString & szNick, bool bHalfOp){ return m_pUserListView->setHalfOp(szNick,bHalfOp); };

	/**
	* \brief Sets the voice mode
	* \param szNick The nick to set the mode on
	* \param bVoice Whether to set or unset the mode on the user
	* \return bool
	*/
	bool setVoice(const QString & szNick, bool bVoice){ return m_pUserListView->setVoice(szNick,bVoice); };

	/**
	* \brief Sets the user operator mode
	* \param szNick The nick to set the mode on
	* \param bUserOp Whether to set or unset the mode on the user
	* \return bool
	*/
	bool setUserOp(const QString & szNick, bool bUserOp){ return m_pUserListView->setUserOp(szNick,bUserOp); };

	/**
	* \brief Returns true if the user is a chan owner
	* \param szNick The nickname of the user to check
	* \param bAtLeast Whether the user is at least a chan owner
	* \return bool
	*/
	bool isChanOwner(const QString & szNick, bool bAtLeast = false){ return m_pUserListView->isChanOwner(szNick,bAtLeast); };

	/**
	* \brief Returns true if the user is a chan admin
	* \param szNick The nickname of the user to check
	* \param bAtLeast Whether the user is at least a chan admin
	* \return bool
	*/
	bool isChanAdmin(const QString & szNick, bool bAtLeast = false){ return m_pUserListView->isChanAdmin(szNick,bAtLeast); };

	/**
	* \brief Returns true if the user is an operator
	* \param szNick The nickname of the user to check
	* \param bAtLeast Whether the user is at least an operator
	* \return bool
	*/
	bool isOp(const QString & szNick, bool bAtLeast = false){ return m_pUserListView->isOp(szNick,bAtLeast); };

	/**
	* \brief Returns true if the user is a half operator
	* \param szNick The nickname of the user to check
	* \param bAtLeast Whether the user is at least a half operator
	* \return bool
	*/
	bool isHalfOp(const QString & szNick, bool bAtLeast = false){ return m_pUserListView->isHalfOp(szNick,bAtLeast); };

	/**
	* \brief Returns true if the user is a voice
	* \param szNick The nickname of the user to check
	* \param bAtLeast Whether the user is at least a voice
	* \return bool
	*/
	bool isVoice(const QString & szNick, bool bAtLeast = false){ return m_pUserListView->isVoice(szNick,bAtLeast); };

	/**
	* \brief Returns true if the user is a user operator
	* \param szNick The nickname of the user to check
	* \param bAtLeast Whether the user is at least a user operator
	* \return bool
	*/
	bool isUserOp(const QString & szNick, bool bAtLeast = false){ return m_pUserListView->isUserOp(szNick,bAtLeast); };

	/**
	* \brief Returns true if we are an ircop
	* \param bAtLeast Whether we are at least an ircop
	* \return bool
	*/
	bool isMeIrcOp(bool bAtLeast = false);

	/**
	* \brief Returns true if we are a chan owner
	* \param bAtLeast Whether we are at least a chan owner
	* \return bool
	*/
	bool isMeChanOwner(bool bAtLeast = false);

	/**
	* \brief Returns true if we are a chan admin
	* \param bAtLeast Whether we are at least a chan admin
	* \return bool
	*/
	bool isMeChanAdmin(bool bAtLeast = false);

	/**
	* \brief Returns true if we are an operator
	* \param bAtLeast Whether we are at least an operator
	* \return bool
	*/
	bool isMeOp(bool bAtLeast = false);

	/**
	* \brief Returns true if we are a half operator
	* \param bAtLeast Whether we are at least a half operator
	* \return bool
	*/
	bool isMeHalfOp(bool bAtLeast = false);

	/**
	* \brief Returns true if we are a voice
	* \param bAtLeast Whether we are at least a voice
	* \return bool
	*/
	bool isMeVoice(bool bAtLeast = false);

	/**
	* \brief Returns true if we are a user operator
	* \param bAtLeast Whether we are at least a user operator
	* \return bool
	*/
	bool isMeUserOp(bool bAtLeast = false);

	/**
	* \brief Perform a user action in the channel
	* \param user The user who made the action
	* \param uActionType The type of the action
	* \return void
	*/
	void userAction(KviIrcMask * user, unsigned int uActionType);

	/**
	* \brief Perform a user action in the channel
	* \param szNick The nickname of the user who made the action
	* \param uActionType The type of the action
	* \return void
	*/
	void userAction(const QString & szNick, unsigned int uActionType);

	/**
	* \brief Perform a user action in the channel
	* \param szNick The nickname of the user who made the action
	* \param szUser The username of the user who made the action
	* \param szHost The hostname of the user who made the action
	* \param uActionType The type of the action
	* \return void
	*/
	void userAction(const QString & szNick, const QString & szUser, const QString & szHost, unsigned int uActionType);

	/**
	* \brief Called when someone perform an action in the channel
	* \param szNick The nickname of the user who made the action
	* \param uActionType The type of the action
	* \param iTemperature The temperature of the action
	* \return void
	*/
	void channelAction(const QString & szNick, unsigned int uActionType, int iTemperature);

	/**
	* \brief Called when someone changes his nick
	* \param szOldNick The old nickname of the user
	* \param szNewNick The new nickname of the user
	* \return bool
	*/
	bool nickChange(const QString & szOldNick, const QString & szNewNick);

	/**
	* \brief Called when someone parts the channel
	* \param szNick The nickname of the user who parts
	* \return bool
	*/
	bool part(const QString & szNick);

	/**
	* \brief Returns true if the user is on the channel
	* \param szNick The nickname of the user
	* \return bool
	*/
	bool isOn(const QString & szNick){ return (m_pUserListView->findEntry(szNick) != 0); };

	/**
	* \brief Searches for a user in the userlist
	* \param szNick The nickname of the user to find
	* \return KviUserListEntry *
	*/
	KviUserListEntry * findEntry(const QString & szNick){ return m_pUserListView->findEntry(szNick); };

	/**
	* \brief Returns our flags
	* \return int
	*/
	int myFlags();

	/**
	* \brief Updates the tooltip over the channel modes
	* \return void
	*/
	void updateModeLabel();

	/**
	* \brief Outputs a message to the channel window
	* \param iMsgType The type of the message
	* \param szMsg The message :)
	* \return void
	*/
	virtual void outputMessage(int iMsgType, const QString & szMsg);

	/**
	* \brief Called when we send a message
	* \param szBuffer The buffer :)
	* \return void
	*/
	void ownMessage(const QString & szBuffer);

	/**
	* \brief Called when we perform an action
	* \param szBuffer The buffer :)
	* \return void
	*/
	void ownAction(const QString & szBuffer);

	/**
	* \brief Sets the channel mode
	* \param cMode The mode to set
	* \param bAdd Whether to add or remove the mode
	* \return void
	*/
	void setChannelMode(char cMode, bool bAdd);

	/**
	* \brief Returns the channel mode
	* \return QString
	*/
	QString channelMode() { return m_szChannelMode; };

	/**
	* \brief Gets the channel mode string and saves it in the buffer
	* \param szBuffer The buffer :)
	* \return void
	*/
	void getChannelModeString(QString & szBuffer);

	/**
	* \brief Sets the limit for the channel
	* \param pcLimit The limit of the channel
	* \return void
	*/
	void setChannelLimit(const char * pcLimit);

	/**
	* \brief Returns true if the channel has a limit set
	* \return bool
	*/
	bool hasChannelLimit(){ return !m_szChannelLimit.isEmpty(); };

	/**
	* \brief Returns the limit number
	* \return QString &
	*/
	QString & channelLimit(){ return m_szChannelLimit; };

	/**
	* \brief Sets the key for the channel
	* \param pcKey The key of the channel
	* \return void
	*/
	void setChannelKey(const char * pcKey);

	/**
	* \brief Returns true if the channel has a key set
	* \return bool
	*/
	bool hasChannelKey() { return !m_szChannelKey.isEmpty(); };

	/**
	* \brief Returns the channel key
	* \return QString &
	*/
	QString & channelKey(){ return m_szChannelKey; };

	/**
	* \brief Adds a user to the highlight list
	* \param szNick The nickname of the user
	* \return void
	*/
	void addHighlightedUser(const QString & szNick);

	/**
	* \brief Removes a user from the highlight list
	* \param szNick The nickname of the user
	* \return void
	*/
	void removeHighlightedUser(const QString & szNick);

	/**
	* \brief Returns true if the user is highlighted
	* \param szNick The nickname of the user
	* \return bool
	*/
	bool isHighlightedUser(const QString & szNick) { return m_pTmpHighLighted->contains(szNick,Qt::CaseInsensitive); };

	/**
	* \brief Called when the channel losts the focus by the user
	* \return void
	*/
	virtual void lostUserFocus();

	/**
	* \brief Creates the tooltip over the channel treeview
	* \param szBuffer The buffer where to store the data
	* \return void
	*/
	virtual void getWindowListTipText(QString & szBuffer);

	/**
	* \brief Unhighlights the windowlist item
	* \return void
	*/
	void unhighlight();

	/**
	* \brief Pastes the most recent channel log into the buffer
	*
	* It searches for the most recent channel log of the channel we have just
	* joined, and pastes into the buffer the last N lines, where N is get from
	* the options
	* \return void
	*/
	void pasteLastLog();

	/**
	* \brief Loads a log file
	*
	* It opens a logfile, gzipped or not, and returns the content in a buffer
	* \param szFileName The filename of the log file
	* \param bGzip Whether the log file is gzipped
	* \return QByteArray
	*/
	QByteArray loadLogFile(const QString & szFileName, bool bGzip);
protected:
	/**
	* \brief Filters the events
	* \param o The object
	* \param e The event
	* \return bool
	*/
	bool eventFilter(QObject * o, QEvent * e);

	/**
	* \brief Returns the correct icon for the channel
	* \return QPixmap *
	*/
	virtual QPixmap * myIconPtr();

	/**
	* \brief Fills in the caption buffers
	* \return void
	*/
	virtual void fillCaptionBuffers();

	/**
	* \brief Gets the group name
	* \param szBuffer The buffer where to save the data
	* \return void
	*/
	virtual void getConfigGroupName(QString & szBuffer);

	/**
	* \brief Saves the properties to file
	* \param cfg The config file
	* \return void
	*/
	virtual void saveProperties(KviConfig * cfg);

	/**
	* \brief Loads the properties from file
	* \param cfg The config file
	* \return void
	*/
	virtual void loadProperties(KviConfig * cfg);

	/**
	* \brief Applies the options
	* \return void
	*/
	virtual void applyOptions();

	/**
	* \brief Gets the base name for log file
	* \param szBuffer The buffer where to save data
	* \return void
	*/
	virtual void getBaseLogFileName(QString & szBuffer);

	/**
	* \brief Trigger the OnChannelWindowCreated event
	* \return void
	*/
	virtual void triggerCreationEvents();

	/**
	* \brief Called when we toggle bans, quiet bans, exceptions or invites editor
	* \param ppEd The mask editor
	* \param ppBtn The toolpage button
	* \param l The masks list
	* \param cFlag The type of flag
	* \param pcEdName Tee editor name
	* \return void
	*/
	void toggleEditor(KviMaskEditor ** ppEd, KviWindowToolPageButton ** ppBtn, KviPointerList<KviMaskEntry> * l, char cFlag, const char * pcEdName);

	/**
	* \brief Called when someone sets a mask in the channel's lists
	* \param szMask The mask set
	* \param bAdd Whether to add or remove the mask
	* \param szSetBy Who set the mask
	* \param uSetAt The datetime when the mask was set
	* \param l The list of masks in the channel lists
	* \param ppEd The mask editor window
	* \return void
	*/
	void internalMask(const QString & szMask, bool bAdd, const QString & szSetBy, unsigned int uSetAt, KviPointerList<KviMaskEntry> * l, KviMaskEditor ** ppEd);

	/**
	* \brief Checks the channel synchronization time
	* \return void
	*/
	void checkChannelSync();

	/**
	* \brief Splits the channel view into two views
	*
	* The upper one will be used to show all system messages like join,
	* parts, quits, topics and so on, while the lower one will be used to
	* show the users messages.
	* \param bShow Whether to show the double view
	* \return void
	*/
	void showDoubleView(bool bShow);

	/**
	* \brief Fixes the action history
	* \return void
	*/
	void fixActionHistory();

	/**
	* \brief Gets the talking user stats to fill in the tooltip
	* \param szBuffer The buffer where to store the data
	* \param list The list of talking users
	* \param bPast Whether the activity is recent or in the past
	* \return void
	*/
	void getTalkingUsersStats(QString & szBuffer, QStringList & list, bool bPast);

	/**
	* \brief Preprocess message stripping control bytes
	* \param szMessage The message :)
	* \return void
	*/
	virtual void preprocessMessage(QString & szMessage);

	virtual void resizeEvent(QResizeEvent * e);
	virtual void closeEvent(QCloseEvent * e);
private slots:
	/**
	* \brief Toggles the double view mode
	* \return void
	*/
	void toggleDoubleView();

	/**
	* \brief Toggles the userlist view
	* \return void
	*/
	void toggleListView();

	/**
	* \brief Toggles the ban editor
	* \return void
	*/
	void toggleBanEditor();

	/**
	* \brief Toggles the ban exception editor
	* \return void
	*/
	void toggleBanExceptionEditor();

	/**
	* \brief Toggles the invite editor
	* \return void
	*/
	void toggleInviteEditor();

	/**
	* \brief Toggles the quiet ban editor
	* \return void
	*/
	void toggleQuietBanEditor();

	/**
	* \brief Toggles the mode editor
	* \return void
	*/
	void toggleModeEditor();

	/**
	* \brief Called when we close the mode editor
	*/
	void modeSelectorDone();

	/**
	* \brief Called when we select the topic
	* \param szTopic The topic :)
	* \return void
	*/
	void topicSelected(const QString & szTopic);

	/**
	* \brief Called when we select the modes from the mode editor
	* \param pcMode The modes selected
	* \return void
	*/
	void setMode(const char * pcMode);

	/**
	* \brief Called when we right-click the irc view.
	*
	* Trigger the OnChannelPopupRequest event
	* \return void
	*/
	void textViewRightClicked();

	/**
	* \brief Removes the masks from the mask editor window
	* \param pEd The mask editor
	* \param pList The list of masks to remove
	* \return void
	*/
	void removeMasks(KviMaskEditor * pEd, KviPointerList<KviMaskEntry> * pList);

	/**
	* \brief Toggles tool buttons widget over the listview
	* \return void
	*/
	void toggleToolButtons();
};

#endif //_KVI_CHANNEL_H_
