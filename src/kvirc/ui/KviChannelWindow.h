#ifndef _KVI_CHANNEL_H_
#define _KVI_CHANNEL_H_
//=============================================================================
//
//   File : KviChannelWindow.h
//   Creation date : Tue Aug  1 2000 01:42:00 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2011 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your option) any later version.
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
* \file KviChannelWindow.h
* \author Szymon Stefanek
* \brief Channel widget: abstraction of an IRC channel
*/

#include "kvi_settings.h"
#include "KviConsoleWindow.h"
#include "KviWindow.h"
#include "KviIrcUserDataBase.h"
#include "KviPixmap.h"
#include "KviUserListView.h"
#include "KviTimeUtils.h"
#include "KviModeWidget.h"

#include <QDateTime>
#include <QList>
#include <QString>
#include <QStringList>
#include <QToolButton>

#include <map>
#include <vector>

class KviConsoleWindow;
class KviIrcMask;
class KviMaskEditor;
class KviModeEditor;
class KviTalHBox;
class KviThemedLabel;
class KviTopicWidget;

#ifdef COMPILE_ON_WINDOWS
// windows compiler wants this instead of the forward decl
#include "KviMaskEditor.h"
#else
struct KviMaskEntry; // KviMaskEditor.h
#endif

/**
* \def KVI_CHANNEL_AVERAGE_USERS The average channel users of a channel
* \def KVI_CHANNEL_ACTION_HISTORY_MAX_COUNT Maximum actions count we keep in memory
* \def KVI_CHANNEL_ACTION_HISTORY_MAX_TIMESPAN Timespan of the oldest action we keep in memory (600 secs = 10 mins)
*/
#define KVI_CHANNEL_ACTION_HISTORY_MAX_COUNT 40
#define KVI_CHANNEL_ACTION_HISTORY_MAX_TIMESPAN 600

#ifndef KVI_CHANNEL_AVERAGE_USERS
#define KVI_CHANNEL_AVERAGE_USERS 101
#endif

/**
* \struct _KviChannelAction
* \brief A struct which holds the channel actions
*/
struct KviChannelAction
{
	QString szNick;           // action source nick
	unsigned int uActionType; // type of the action
	kvi_time_t tTime;         // time of the action
	int iTemperature;         // temperature of the action
};

/**
* \struct _KviChannelActivityStats
* \brief A struct which holds the activity stats
*/
struct KviChannelActivityStats
{
	unsigned int uActionCount;            // number of actions in the history
	bool bStatsInaccurate;                // the stats are inaccurate because we have just joined the chan
	unsigned int uLastActionTimeSpan;     // the timespan between the last action and now
	unsigned int uFirstActionTimeSpan;    // the time span between the first and the last action
	double dActionsPerMinute;             // average number of actions per minute in the lastActionTimeSpan
	unsigned int uActionsInTheLastMinute; // number of actions in the last minute
	int iAverageActionTemperature;        // the average chan temperature
	unsigned int uHotActionCount;
	unsigned int uHotActionPercent;
	QStringList lTalkingUsers; // users that seem to be talking NOW
	QStringList lWereTalkingUsers;
};

/**
* \class KviChannelWindow
* \brief The class which manages a channel
*/
class KVIRC_API KviChannelWindow : public KviWindow
{
	Q_OBJECT
public:
	/**
	* \enum StateFlag
	* \brief Holds the state flags of the channel
	*/
	enum StateFlag
	{
		HaveAllNames = 1,             /**< Flag for "have all names" */
		HaveWhoList = (1 << 2),       /**< Flag for "have WHO list" */
		DeadChan = (1 << 3),          /**< Flag for "dead channel" */
		SentWhoRequest = (1 << 4),    /**< Flag to set WHO request */
		SentPart = (1 << 5),          /**< Flag to set PART request */
		Synchronized = (1 << 6),      /**< Flag to set SYNC request */
		NoCloseOnPart = (1 << 7),     /**< Flag to set no close on part */
		SentSyncWhoRequest = (1 << 8) /**< Flag for SYNC request */
	};

	/**
	* \enum ActivityLimit
	* \brief Holds the limits of the activity in a channel
	*/
	enum ActivityLimit
	{
		Ice = 5,        /**< The limit to be "ice" */
		VeryCold = 10,  /**< The limit to be "very cold" */
		Cold = 20,      /**< The limit to be "cold" */
		Undefined = 30, /**< The limit to be "undefined" */
		Hot = 50,       /**< The limit to be "hot" */
		VeryHot = 70    /**< The limit to be "very hot" */
	};

	/**
	* \brief Constructs the channel object
	* \param lpConsole The console of the context
	* \param szName The name of the channel
	* \return KviChannelWindow
	*/
	KviChannelWindow(KviConsoleWindow * lpConsole, const QString & szName);

	/**
	* \brief Destroys the channel object
	*/
	~KviChannelWindow();

protected:
	KviTalSplitter * m_pTopSplitter;
	KviTalSplitter * m_pVertSplitter;
	QToolButton * m_pDoubleViewButton;
	KviWindowToolPageButton * m_pListViewButton;
	KviWindowToolPageButton * m_pModeEditorButton;
	std::map<char, KviWindowToolPageButton *> m_ListEditorButtons;
	std::map<char, KviMaskEditor *> m_ListEditors;
	KviModeEditor * m_pModeEditor;
	KviIrcView * m_pMessageView;
	KviTopicWidget * m_pTopicWidget;
	KviUserListView * m_pUserListView;
	KviModeWidget * m_pModeWidget;
	int m_iStateFlags;
	QString m_szSentModeRequests;
	QString m_szChannelMode;
	std::map<char, QString> m_szChannelParameterModes;
	std::map<char, std::vector<KviMaskEntry *>> m_ModeLists;
	KviPixmap m_privateBackground;
	QDateTime m_joinTime;
	QString m_szNameWithUserFlag;
	QStringList * m_pTmpHighLighted;
	unsigned int m_uActionHistoryHotActionCount;
	QList<KviChannelAction *> m_lActionHistory;
	kvi_time_t m_tLastReceivedWhoReply;
	QList<int> m_VertSplitterSizesList;
	QList<int> m_SplitterSizesList;
	KviTalHBox * m_pButtonContainer;

public:
	/**
	* \brief Returns the user listview object
	* \return KviUserListView *
	*/
	KviUserListView * userListView() { return m_pUserListView; };

	/**
	* \brief Returns the topic widget object
	* \return KviTopicWidget *
	*/
	KviTopicWidget * topicWidget() { return m_pTopicWidget; };

	/**
	* \brief Returns the irc view object
	* \return KviIrcView *
	*/
	KviIrcView * messageView() const { return m_pMessageView; };

	/**
	* \brief Returns the button container object
	* \return QFrame *
	*/
	QFrame * buttonContainer() override { return (QFrame *)m_pButtonContainer; }

	/**
	* \brief Returns a list of masks for a specific mode
	* \return const std::vector<KviMaskEntry *> &
	*/
	const std::vector<KviMaskEntry *> & modeMasks(char cMode) const
	{
		static const std::vector<KviMaskEntry *> EMPTY_VECTOR;
		const auto it = m_ModeLists.find(cMode);
		if(it != m_ModeLists.end())
			return it->second;
		return EMPTY_VECTOR;
	};

	/**
	* \brief Returns the first selected nickname in the userlist
	* \return QString *
	*/
	QString * firstSelectedNickname() { return m_pUserListView->firstSelectedNickname(); };

	/**
	* \brief Returns the next selected nickname in the userlist
	* \return QString *
	*/
	QString * nextSelectedNickname() { return m_pUserListView->nextSelectedNickname(); };

	/**
	* \brief Returns the name of the channel
	* \return const QString &
	*/
	const QString & target() override { return windowName(); }

	/**
	* \brief Returns the name of the channel with user flags
	* \return const QString &
	*/
	const QString & nameWithUserFlag() { return m_szNameWithUserFlag; };

	/**
	* \brief Gets the channel activity stats and put them in the buffer
	* \param pStats The buffer where to store the data
	* \return void
	*/
	void getChannelActivityStats(KviChannelActivityStats * pStats);

	/**
	* \brief Returns the number of selected users
	* \return int
	*/
	int selectedCount() { return m_pUserListView->selectedCount(); };

	/**
	* \brief Returns the number of users with chanowner status
	* \return int
	*/
	int chanOwnerCount() { return m_pUserListView->chanOwnerCount(); };

	/**
	* \brief Returns the number of users with chanadmin status
	* \return int
	*/
	int chanAdminCount() { return m_pUserListView->chanAdminCount(); };

	/**
	* \brief Returns the number of users with op status
	* \return int
	*/
	int opCount() { return m_pUserListView->opCount(); };

	/**
	* \brief Returns the number of users with halfop status
	* \return int
	*/
	int halfOpCount() { return m_pUserListView->halfOpCount(); };

	/**
	* \brief Returns the number of users with voice status
	* \return int
	*/
	int voiceCount() { return m_pUserListView->voiceCount(); };

	/**
	* \brief Returns the number of users with userop status
	* \return int
	*/
	int userOpCount() { return m_pUserListView->userOpCount(); };

	/**
	* \brief Returns the number of users
	* \return unsigned int
	*/
	unsigned int count() { return m_pUserListView->count(); };

	/**
	* \brief Returns the number of masks is a channel mode list
	* \return size_t
	*/
	size_t maskCount(char cMode) const { return this->modeMasks(cMode).size(); };

	/**
	* \brief Called when someone sets a channel mode that is stored in a list; these modes require a parameter that is tipically a mask
	*
	* Examples:
	*  - b: ban with mask
	*  - e: ban exception with mask
	*  - I: invite exception with mask
	*  - q: channel owner with nick (unreal) or quiet ban (ircd-seven)
	*  - g: spam filter word (inspircd)
	* \param cMode The type of the mask
	* \param szMask The mask set (more generically, the parameter)
	* \param bAdd Whether to add or remove the mask
	* \param szSetBy Who set the mask
	* \param uSetAt The datetime when the mask was set
	* \param szChangeMask If bAdd is false and this string is set, the mask will be updated instead that removed
	* \return void
	*/
	void setModeInList(char cMode, const QString & szMask, bool bAdd, const QString & szSetBy, unsigned int uSetAt, QString szChangeMask = QString());

	/**
	* \brief Returns the time of the last received WHO reply
	* \return kvi_time_t
	*/
	kvi_time_t lastReceivedWhoReply() { return m_tLastReceivedWhoReply; };

	/**
	* \brief Sets the time of the last received WHO reply
	* \param tTime The source time
	* \return void
	*/
	void setLastReceivedWhoReply(kvi_time_t tTime) { m_tLastReceivedWhoReply = tTime; };

	/**
	* \brief Returns true if we have sent the sync WHO request
	* \return bool
	*/
	bool sentSyncWhoRequest() { return (m_iStateFlags & SentSyncWhoRequest); };

	/**
	* \brief Sets the sync WHO request flag
	* \return void
	*/
	void setSentSyncWhoRequest() { m_iStateFlags |= SentSyncWhoRequest; };

	/**
	* \brief Clears the sync WHO request flag
	* \return void
	*/
	void clearSentSyncWhoRequest() { m_iStateFlags ^= SentSyncWhoRequest; };

	/**
	* \brief Returns true if we have sent the WHO request
	* \return bool
	*/
	bool sentWhoRequest() { return (m_iStateFlags & SentWhoRequest); };

	/**
	* \brief Sets the WHO request flag
	* \return void
	*/
	void setSentWhoRequest() { m_iStateFlags |= SentWhoRequest; };

	/**
	* \brief Returns true if we have sent a list request for a specific channel mode
	* \return bool
	*/
	bool sentListRequest(char cMode) { return m_szSentModeRequests.contains(cMode); };

	/**
	* \brief Sets the "sent request" flag for a specific channel mode
	* \return void
	*/
	void setSentListRequest(char cMode) { m_szSentModeRequests.append(cMode); };

	/**
	* \brief Clears the "sent request" flag for a specific chanel mode
	* \return void
	*/
	void setListRequestDone(char cMode)
	{
		m_szSentModeRequests.remove(cMode);
		checkChannelSync();
	};

	/**
	* \brief Returns true if the channel has all names
	* \return bool
	*/
	bool hasAllNames() { return (m_iStateFlags & HaveAllNames); };

	/**
	* \brief Sets the existence of all names
	* \return void
	*/
	void setHasAllNames()
	{
		m_iStateFlags |= HaveAllNames;
		checkChannelSync();
	};

	/**
	* \brief Returns true if the channel has an invite list
	* \return bool
	*/
	bool hasInviteList() { return m_ModeLists.count('I'); };

	/**
	* \brief Returns true if the channel has a WHO list
	* \return bool
	*/
	bool hasWhoList() { return (m_iStateFlags & HaveWhoList); };

	/**
	* \brief Sets the existence of the WHO list
	* \return void
	*/
	void setHasWhoList()
	{
		m_iStateFlags |= HaveWhoList;
		checkChannelSync();
	};

	/**
	* \brief Returns true if the channel has a ban list
	* \return bool
	*/
	bool hasBanList() { return m_ModeLists.count('b'); };

	/**
	* \brief Returns true if the channel has a ban exception list
	* \return bool
	*/
	bool hasBanExceptionList() { return m_ModeLists.count('e'); };

	/**
	* \brief Returns true if the channel has a quiet ban list
	* \return bool
	*/
	bool hasQuietBanList() { return m_ModeLists.count('q'); };

	/**
	* \brief Returns true if the channel has to be closed on part
	* \return bool
	*/
	bool closeOnPart() { return !(m_iStateFlags & NoCloseOnPart); };

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
	bool activityMeter(unsigned int * puActivityValue, unsigned int * puActivityTemperature) override;

	/**
	* \brief Sets the channel as dead
	* \return void
	*/
	void setDeadChan();

	/**
	* \brief Returns true if the channel is dead
	* \return bool
	*/
	bool isDeadChan() { return (m_iStateFlags & DeadChan); };

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
	void prependUserFlag(const QString & szNick, QString & szBuffer) { m_pUserListView->prependUserFlag(szNick, szBuffer); };

	/**
	* \brief Returns the flag of a user
	* \param szNick The nick of the user
	* \return char
	*/
	char getUserFlag(const QString & szNick) { return m_pUserListView->getUserFlag(szNick); };

	/**
	* \brief Returns the size of the channel
	* \return QSize
	*/
	QSize sizeHint() const override;

	/**
	* \brief Enables or disable the userlist updates
	* \param bEnable Whether to enable the updates
	* \return void
	*/
	void enableUserListUpdates(bool bEnable) { m_pUserListView->enableUpdates(bEnable); };

	/**
	* \brief Called when a user joins the channel
	* \param szNick The nickname of the user
	* \param szUser The username of the user
	* \param szHost The hostname of the user
	* \param iFlags The flags of the user
	* \return KviUserListEntry *
	*/
	KviUserListEntry * join(const QString & szNick, const QString & szUser = QString(), const QString & szHost = QString(), int iFlags = 0) { return m_pUserListView->join(szNick, szUser, szHost, iFlags); };

	/**
	* \brief Returns true if the avatar of a user is changed
	* \param szNick The nickname of the user
	* \return bool
	*/
	bool avatarChanged(const QString & szNick) { return m_pUserListView->avatarChanged(szNick); };

	/**
	* \brief Sets the chan owner mode
	* \param szNick The nick to set the mode on
	* \param bChanOwner Whether to set or unset the mode on the user
	* \return bool
	*/
	bool setChanOwner(const QString & szNick, bool bChanOwner) { return m_pUserListView->setChanOwner(szNick, bChanOwner); };

	/**
	* \brief Sets the chan admin mode
	* \param szNick The nick to set the mode on
	* \param bChanAdmin Whether to set or unset the mode on the user
	* \return bool
	*/
	bool setChanAdmin(const QString & szNick, bool bChanAdmin) { return m_pUserListView->setChanAdmin(szNick, bChanAdmin); };

	/**
	* \brief Sets the operator mode
	* \param szNick The nick to set the mode on
	* \param bOp Whether to set or unset the mode on the user
	* \param bIsMe Whether the user opped is us
	* \return bool
	*/
	bool setOp(const QString & szNick, bool bOp, bool bIsMe);

	/**
	* \brief Sets the half operator mode
	* \param szNick The nick to set the mode on
	* \param bHalfOp Whether to set or unset the mode on the user
	* \return bool
	*/
	bool setHalfOp(const QString & szNick, bool bHalfOp, bool) { return m_pUserListView->setHalfOp(szNick, bHalfOp); };

	/**
	* \brief Sets the voice mode
	* \param szNick The nick to set the mode on
	* \param bVoice Whether to set or unset the mode on the user
	* \return bool
	*/
	bool setVoice(const QString & szNick, bool bVoice, bool) { return m_pUserListView->setVoice(szNick, bVoice); };

	/**
	* \brief Sets the user operator mode
	* \param szNick The nick to set the mode on
	* \param bUserOp Whether to set or unset the mode on the user
	* \return bool
	*/
	bool setUserOp(const QString & szNick, bool bUserOp, bool) { return m_pUserListView->setUserOp(szNick, bUserOp); };

	/**
	* \brief Returns true if the user is a chan owner
	* \param szNick The nickname of the user to check
	* \param bAtLeast Whether the user is at least a chan owner
	* \return bool
	*/
	bool isChanOwner(const QString & szNick, bool bAtLeast = false) { return m_pUserListView->isChanOwner(szNick, bAtLeast); };

	/**
	* \brief Returns true if the user is a chan admin
	* \param szNick The nickname of the user to check
	* \param bAtLeast Whether the user is at least a chan admin
	* \return bool
	*/
	bool isChanAdmin(const QString & szNick, bool bAtLeast = false) { return m_pUserListView->isChanAdmin(szNick, bAtLeast); };

	/**
	* \brief Returns true if the user is an operator
	* \param szNick The nickname of the user to check
	* \param bAtLeast Whether the user is at least an operator
	* \return bool
	*/
	bool isOp(const QString & szNick, bool bAtLeast = false) { return m_pUserListView->isOp(szNick, bAtLeast); };

	/**
	* \brief Returns true if the user is a half operator
	* \param szNick The nickname of the user to check
	* \param bAtLeast Whether the user is at least a half operator
	* \return bool
	*/
	bool isHalfOp(const QString & szNick, bool bAtLeast = false) { return m_pUserListView->isHalfOp(szNick, bAtLeast); };

	/**
	* \brief Returns true if the user is a voice
	* \param szNick The nickname of the user to check
	* \param bAtLeast Whether the user is at least a voice
	* \return bool
	*/
	bool isVoice(const QString & szNick, bool bAtLeast = false) { return m_pUserListView->isVoice(szNick, bAtLeast); };

	/**
	* \brief Returns true if the user is a user operator
	* \param szNick The nickname of the user to check
	* \param bAtLeast Whether the user is at least a user operator
	* \return bool
	*/
	bool isUserOp(const QString & szNick, bool bAtLeast = false) { return m_pUserListView->isUserOp(szNick, bAtLeast); };

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
	bool isOn(const QString & szNick) { return (m_pUserListView->findEntry(szNick) != 0); };

	/**
	* \brief Searches for a user in the userlist
	* \param szNick The nickname of the user to find
	* \return KviUserListEntry *
	*/
	KviUserListEntry * findEntry(const QString & szNick) { return m_pUserListView->findEntry(szNick); };

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
	virtual void outputMessage(int iMsgType, const QString & szMsg, const QDateTime & datetime = QDateTime());

	/**
	* \brief Called when we send a message
	* \param szBuffer The buffer :)
	* \param bUserFeedback Whether to display the echo feedback to the user
	* \return void
	*/
	void ownMessage(const QString & szBuffer, bool bUserFeedback = true) override;

	/**
	* \brief Called when we perform an action
	* \param szBuffer The buffer :)
	* \return void
	*/
	void ownAction(const QString & szBuffer) override;

	/**
	* \brief Sets a plain (parameter-less) channel mode, (eg: +m)
	* \param cMode The mode to set
	* \param bAdd Whether to add or remove the mode
	* \return void
	*/
	void setChannelMode(char cMode, bool bAdd);

	/**
	* \brief Returns only the plain (parameter-less) channel modes (eg: mi)
	* \return QString
	*/
	QString plainChannelMode() { return m_szChannelMode; };

	/**
	* \brief Fills szBuffer with all set channel modes, but without any parameters (eg: lkmi)
	* \param szBuffer The buffer :)
	* \return void
	*/
	void getChannelModeString(QString & szBuffer);

	/**
	* \brief Fills szBuffer with all set channel modes and any parameters (eg: mi l:10 k:password)
	* \param szBuffer The buffer :)
	* \return void
	*/
	QString getChannelModeStringWithEmbeddedParams();

	/**
	* \brief Sets a channel mode with a parameter; an empty parameter unsets the mode (eg: +k password)
	* \param cMode The mode
	* \param szParam The parameter for the mode
	* \return void
	*/
	void setChannelModeWithParam(char cMode, QString & szParam);

	/**
	* \brief Returns true if the channel has a mode with parameter set (eg. mode k)
	* \param cMode The mode
	* \return bool
	*/
	bool hasChannelMode(char cMode) { return m_szChannelParameterModes.count(cMode); };

	/**
	* \brief Returns the value (parameter) for a channel mode (eg. the password for mode k)
	* \param cMode The mode
	* \return QString
	*/
	QString channelModeParam(char cMode) const
	{
		const auto it = m_szChannelParameterModes.find(cMode);
		if(it != m_szChannelParameterModes.end())
			return it->second;
		return QString();
	};

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
	bool isHighlightedUser(const QString & szNick) { return m_pTmpHighLighted->contains(szNick, Qt::CaseInsensitive); };

	/**
	* \brief Called when the channel losts the focus by the user
	* \return void
	*/
	void lostUserFocus() override;

	/**
	* \brief Creates the tooltip over the channel treeview
	* \param szBuffer The buffer where to store the data
	* \return void
	*/
	void getWindowListTipText(QString & szBuffer) override;

	/**
	* \brief Unhighlights the windowlist item
	* \return void
	*/
	void unhighlight();

	/**
	* \brief Gets the KviIrcConnectionServerInfo structure associated to the current connection
	* \return KviIrcConnectionServerInfo*
	*/
	KviIrcConnectionServerInfo * serverInfo();

	/**
	* \brief Checks the channel synchronization time
	* \return void
	*/
	void checkChannelSync();

	/**
	* \brief Returns the KviIrcView that was last clicked in this window
	*
	* Acts as view() except for split view windows
	* See also: view()
	* \return KviIrcView *
	*/
	KviIrcView * lastClickedView() const override;

protected:
	/**
	* \brief Filters the events
	* \param pObject The object
	* \param pEvent The event
	* \return bool
	*/
	bool eventFilter(QObject * pObject, QEvent * pEvent) override;

	/**
	* \brief Returns the correct icon for the channel
	* \return QPixmap *
	*/
	QPixmap * myIconPtr() override;

	/**
	* \brief Fills in the caption buffers
	* \return void
	*/
	void fillCaptionBuffers() override;

	/**
	* \brief Gets the group name
	* \param szBuffer The buffer where to save the data
	* \return void
	*/
	void getConfigGroupName(QString & szBuffer) override;

	/**
	* \brief Saves the properties to file
	* \param pCfg The config file
	* \return void
	*/
	void saveProperties(KviConfigurationFile * pCfg) override;

	/**
	* \brief Loads the properties from file
	* \param pCfg The config file
	* \return void
	*/
	void loadProperties(KviConfigurationFile * pCfg) override;

	/**
	* \brief Applies the options
	* \return void
	*/
	void applyOptions() override;

	/**
	* \brief Gets the base name for log file
	* \param szBuffer The buffer where to save data
	* \return void
	*/
	void getBaseLogFileName(QString & szBuffer) override;

	/**
	* \brief Trigger the OnChannelWindowCreated event
	* \return void
	*/
	void triggerCreationEvents() override;

	/**
	* \brief Called when someone sets a mask in the channel's lists
	* \param szMask The mask set
	* \param bAdd Whether to add or remove the mask
	* \param szSetBy Who set the mask
	* \param uSetAt The datetime when the mask was set
	* \param l The list of masks in the channel lists
	* \param ppEd The mask editor window
	* \param szChangeMask If bAdd is false and this string is set, the mask will be updated instead that removed
	* \return void
	*/
	void internalMask(const QString & szMask, bool bAdd, const QString & szSetBy, unsigned int uSetAt, std::vector<KviMaskEntry *> & l, KviMaskEditor ** ppEd, QString & szChangeMask);

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
	void preprocessMessage(QString & szMessage) override;

	void resizeEvent(QResizeEvent *) override;
	void closeEvent(QCloseEvent * pEvent) override;
public slots:
	/**
	* \brief Toggles the double view mode
	* \return void
	*/
	void toggleDoubleView();

private slots:
	/**
	* \brief Toggles the userlist view
	* \return void
	*/
	void toggleListView();

	/**
	* \brief Toggles a list mode editor
	* \return void
	*/
	void toggleListModeEditor();

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
	* \param szMode The modes selected, including any plus/minus sign and parameters
	* \return void
	*/
	void setMode(const QString & szMode);

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
	void removeMasks(KviMaskEditor * pEd, const std::vector<KviMaskEntry *> & pList);

	/**
	* \brief Toggles tool buttons widget over the listview
	* \return void
	*/
	void toggleToolButtons();

signals:
	/**
	* \brief Emitted when our op status change
	* \return void
	*/
	void opStatusChanged();
};

#endif //_KVI_CHANNEL_H_
