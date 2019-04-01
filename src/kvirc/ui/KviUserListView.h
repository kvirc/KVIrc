#ifndef _KVI_USERLISTVIEW_H_
#define _KVI_USERLISTVIEW_H_
//=============================================================================
//
//   File : KviUserListView.h
//   Creation date : Tue Aug 1 2000 21:03:41 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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
* \file KviUserListView.h
* \author Szymon Stefanek
* \brief User list view management
*
* \def KVI_USERLISTVIEW_GRIDTYPE_3DGRID Whether to have a 3D grid
* \def KVI_USERLISTVIEW_GRIDTYPE_3DBUTTONS Whether to have 3D buttons
* \def KVI_USERLISTVIEW_GRIDTYPE_PLAINGRID Whether to have a plain grid
* \def KVI_USERLISTVIEW_GRIDTYPE_DOTGRID Whether to have a dotted grid
* \def KVI_USERLISTVIEW_GRIDTYPE_MAXIMUM
* \def KVI_USERLISTVIEW_GRIDTYPE_DEFAULT
*/

#include "kvi_settings.h"
#include "KviPointerHashTable.h"
#include "KviWindowToolWidget.h"
#include "KviCString.h"
#include "KviIrcUserDataBase.h"
#include "KviIrcMask.h"
#include "KviTimeUtils.h"
#include "KviTalToolTip.h"

#include <time.h>
#include <vector>

#include <QWidget>

class QLabel;
class QScrollBar;
class KviUserListView;
class KviUserListViewArea;
class KviConsoleWindow;
class KviWindow;

#define KVI_USERLISTVIEW_GRIDTYPE_3DGRID 0
#define KVI_USERLISTVIEW_GRIDTYPE_3DBUTTONS 1
#define KVI_USERLISTVIEW_GRIDTYPE_PLAINGRID 2
#define KVI_USERLISTVIEW_GRIDTYPE_DOTGRID 3
#define KVI_USERLISTVIEW_GRIDTYPE_MAXIMUM 3
#define KVI_USERLISTVIEW_GRIDTYPE_DEFAULT 0

/**
* \struct _KviUserListViewUserStats
* \brief A struct to hold user statistics
*/
struct KviUserListViewUserStats
{
	unsigned int uTotal;     /**< total users on the channel */
	unsigned int uActive;    /**< active users in the last 10 mins */
	unsigned int uActiveOp;  /**< active operators in the last 10 minutes */
	unsigned int uHot;       /**< hot active users */
	unsigned int uHotOp;     /**< hot operators */
	unsigned int uIrcOp;     /**< total ircops */
	unsigned int uChanOwner; /**< total channel owners */
	unsigned int uChanAdmin; /**< total channel administrators */
	unsigned int uOp;        /**< total operators */
	unsigned int uHalfOp;    /**< total halfops */
	unsigned int uVoiced;    /**< total voiced users */
	unsigned int uUserOp;    /**< total userops (uops) */
	int iAvgTemperature;     /**< average user temperature */
};

/**
* \class KviUserListToolTip
* \brief A class to manage userlist tooltips
*/
class KVIRC_API KviUserListToolTip : public KviTalToolTip
{
public:
	/**
	* \brief Constructs the userlist tooltip
	* \param pView The listview to manage tooltips
	* \param pArea The parent widget area
	* \return KviUserListToolTip
	*/
	KviUserListToolTip(KviUserListView * pView, KviUserListViewArea * pArea);

	/**
	* \brief Destroys the userlist tooltip
	*/
	~KviUserListToolTip();

private:
	KviUserListView * m_pListView;

public:
	/**
	* \brief Checks if user activated tooltips in the option panel
	* \param pnt The point where to show to tooltip
	* \return void
	*/
	void maybeTip(const QPoint & pnt) override;

	/**
	* \brief Shows the tooltip
	* \param rect The rectangle where to show the tooltip
	* \param szStr The string to show
	* \return void
	*/
	void doTip(const QRect & rect, const QString & szStr) { tip(rect, szStr); };
};

/**
* \class KviUserListEntry
* \brief Userlist entry management class
*/
class KVIRC_API KviUserListEntry : public QObject
{
	Q_OBJECT
	friend class KviUserListView;
	friend class KviUserListViewArea;
public:
	/**
	* \brief Constructs the user list entry object
	* \param pParent The parent listview
	* \param szNick The nick of the entry
	* \param pEntry The entry in the irc user db
	* \param iFlags The flags of the user
	* \param bJoinTimeUnknown Whether the join time is unknown or not
	* \return KviUserListEntry
	*/
	KviUserListEntry(KviUserListView * pParent, const QString & szNick, KviIrcUserEntry * pEntry, short int iFlags, bool bJoinTimeUnknown = true);

	/**
	* \brief Destroys the user list entry object
	*/
	~KviUserListEntry();

protected:
	KviUserListView * m_pListView;
	QString m_szNick;
	KviIrcUserEntry * m_pGlobalData;
	short int m_iFlags;
	short int m_iTemperature; // user temperature : 0 = neutral
	kvi_time_t m_lastActionTime;
	kvi_time_t m_joinTime;

	int m_iHeight;
	bool m_bSelected;
	KviUserListEntry * m_pNext;
	KviUserListEntry * m_pPrev;
	KviAnimatedPixmap * m_pAvatarPixmap;

public:
	/**
	* \brief Returns the flags of the user
	* \return short int
	*/
	short int flags() const { return m_iFlags; };

	/**
	* \brief Returns the irc user db entry
	* \return KviIrcUserEntry *
	*/
	KviIrcUserEntry * globalData() { return m_pGlobalData; };

	/**
	* \brief Returns the nick of the entry
	* \return const QString &
	*/
	const QString & nick() const { return m_szNick; };

	/**
	* \brief Returns the next entry in the list
	* \return KviUserListEntry *
	*/
	KviUserListEntry * next() { return m_pNext; };

	/**
	* \brief Returns the color of the entry
	* \param color The color of the entry
	* \return bool
	*/
	bool color(QColor & color);

	/**
	* \brief Updates the avatar data
	* \return void
	*/
	void updateAvatarData();
	void detachAvatarData();

protected:
	/**
	* \brief Recalculates the size height for the entry
	* \return void
	*/
	void recalcSize();

private slots:
	void avatarFrameChanged();
	void avatarDestroyed();
};

/**
* \class KviUserListView
* \brief User list view management class
*/
class KVIRC_API KviUserListView : public KviWindowToolWidget
{
	friend class KviUserListEntry;
	friend class KviUserListViewArea;
	friend class KviUserListToolTip;
	friend class KviConsoleWindow;
	friend class KviChannelWindow;
	friend class KviQueryWindow;
	Q_OBJECT
public:
	/**
	* \brief Constructs the user list view object
	* \param pParent The parent widget
	* \param pButton The tool page button
	* \param pDb The irc users database
	* \param pWnd The window associated to the userlist
	* \param iDictSize The size of the internal database
	* \param szTextLabel The label for the tooltip
	* \param pName The name of the users list
	* \return KviUserListView
	*/
	KviUserListView(QWidget * pParent, KviWindowToolPageButton * pButton, KviIrcUserDataBase * pDb, KviWindow * pWnd, int iDictSize = 5, const QString & szTextLabel = QString(), const char * pName = "userslistview");

	/**
	* \brief Destroys the user list view object
	*/
	~KviUserListView();

protected:
	KviPointerHashTable<QString, KviUserListEntry> * m_pEntryDict;
	KviUserListEntry * m_pTopItem;
	KviUserListEntry * m_pHeadItem;
	KviUserListEntry * m_pTailItem;
	KviUserListEntry * m_pIterator;
	QLabel * m_pUsersLabel;
	KviUserListViewArea * m_pViewArea;
	KviIrcUserDataBase * m_pIrcUserDataBase;
	int m_iSelectedCount;
	int m_iIrcOpCount;
	int m_iChanOwnerCount;
	int m_iChanAdminCount;
	int m_iOpCount;
	int m_iHalfOpCount;
	int m_iVoiceCount;
	int m_iUserOpCount;
	int m_iTotalHeight;
	int m_iFontHeight;
	KviUserListToolTip * m_pToolTip;
	int m_ibEntries;
	int m_ieEntries;
	int m_iIEntries;
	KviWindow * m_pKviWindow;

public:
	/**
	* \brief Updates the list view area
	* \return void
	*/
	void updateArea();

	/**
	* \brief Selects a nickname in the list
	* \param szNick The nickname selected
	* \return void
	*/
	void select(const QString & szNick);

	/**
	* \brief Ensures that a nickname is visible
	* \param szNick The nickname
	* \return void
	*/
	void ensureVisible(const QString & szNick);

	/**
	* \brief Applies the options
	* \return void
	*/
	void applyOptions();

	/**
	* \brief Returns the entry in the list
	* \return KviPointerHashTable<QString,KviUserListEntry> *
	*/
	KviPointerHashTable<QString, KviUserListEntry> * entryDict() { return m_pEntryDict; };

	/**
	* \brief Returns the first item of the user list
	* \return KviUserListEntry *
	*/
	KviUserListEntry * firstItem() { return m_pHeadItem; };

	/**
	* \brief Returns the item at the given position
	* \param pnt The point where to search
	* \param pRect The rectangle where to search
	* \return KviUserListEntry *
	*/
	KviUserListEntry * itemAt(const QPoint & pnt, QRect * pRect = nullptr);

	/**
	* \brief Returns true if the item in the entry is visible
	* \param pEntry The entry in the user list
	* \return bool
	*/
	bool itemVisible(KviUserListEntry * pEntry);

	/**
	* \brief Returns the window associated to the userlist
	* \return KviWindow *
	*/
	KviWindow * window() { return m_pKviWindow; };

	/**
	* \brief Returns the number of the entries in the userlist
	* \return unsigned int
	*/
	unsigned int count() { return m_pEntryDict->count(); };

	/**
	* \brief Returns the number of selected entries in the userlist
	* \return int
	*/
	int selectedCount() { return m_iSelectedCount; };

	/**
	* \brief Returns the number of ircops
	* \return int
	*/
	int ircOpCount() { return m_iIrcOpCount; };

	/**
	* \brief Returns the number of chan owners
	* \return int
	*/
	int chanOwnerCount() { return m_iChanOwnerCount; };

	/**
	* \brief Returns the number of chan admins
	* \return int
	*/
	int chanAdminCount() { return m_iChanAdminCount; };

	/**
	* \brief Returns the number of operators
	* \return int
	*/
	int opCount() { return m_iOpCount; };

	/**
	* \brief Returns the number of half operators
	* \return int
	*/
	int halfOpCount() { return m_iHalfOpCount; };

	/**
	* \brief Returns the number of voices
	* \return int
	*/
	int voiceCount() { return m_iVoiceCount; };

	/**
	* \brief Returns the number of user operators
	* \return int
	*/
	int userOpCount() { return m_iUserOpCount; };

	/**
	* \brief Searches an entry in the userlist
	*
	* If the nick is not found, it returns 0
	* \param szNick The nickname to find
	* \return KviUserListEntry *
	*/
	KviUserListEntry * findEntry(const QString & szNick) { return szNick.isEmpty() ? 0 : m_pEntryDict->find(szNick); };

	/**
	* \brief Appends the selected nicknames to the buffer
	* \param szBuffer The buffer to use
	* \return void
	*/
	void appendSelectedNicknames(QString & szBuffer);

	/**
	* \brief Returns the first selected nickname in the list
	* \return QString *
	*/
	QString * firstSelectedNickname();

	/**
	* \brief Returns the next selected nickname in the list
	* \return QString *
	*/
	QString * nextSelectedNickname();

	/**
	* \brief Parts from all channels
	* \return void
	*/
	void partAll();

	/**
	* \brief Parts from all channels but the given one
	* \param szWhoNot The channel to not part
	* \return void
	*/
	void partAllButOne(const QString & szWhoNot);

	/**
	* \brief Gets the user statistics and store them in the user stats pointer
	* \param pStats The pointer of the user statistics
	* \return void
	*/
	void userStats(KviUserListViewUserStats * pStats);

	/**
	* \brief Returns the level of the user mode
	*
	* Levels:
	* - chanowner: 60
	* - chanadmin: 50
	* - op:        40
	* - halfop:    30
	* - voice:     20
	* - userop:    10
	* \param szNick The nickname of the user to get level
	* \return int
	*/
	int getUserModeLevel(const QString & szNick);

	/**
	* \brief Returns the join time of a user
	* \param szNick The nickname of the user
	* \return kvi_time_t
	*/
	kvi_time_t getUserJoinTime(const QString & szNick);

	/**
	* \brief Returns the time of the last action of a user
	* \param szNick The nickname of the user
	* \return kvi_time_t
	*/
	kvi_time_t getUserLastActionTime(const QString & szNick);

	/**
	* \brief Returns the flags of a user
	* \param pEntry The user in the userlist
	* \return char
	*/
	char getUserFlag(KviUserListEntry * pEntry);

	/**
	* \brief Returns the flags of a user
	* \param szNick The nickname of the user
	* \return char
	*/
	char getUserFlag(const QString & szNick) { return getUserFlag(m_pEntryDict->find(szNick)); };

	/**
	* \brief Check if a user parts from a channel
	* \param szNick The nickname to check
	* \return bool
	*/
	bool part(const QString & szNick) { return partInternal(szNick, true); };

	/**
	* \brief Prepends the user flags before the nickname
	* \param szNick The nickname of the user
	* \param szBuffer The buffer where to store the data
	* \return void
	*/
	void prependUserFlag(const QString & szNick, QString & szBuffer);

	/**
	* \brief Returns the flags of a user
	* \param szNick The nickname of the user
	* \return int
	*/
	int flags(const QString & szNick);

	/**
	* \brief Returns true if the user is a chan owner
	* \param szNick The nick to moderate
	* \param bAtLeast Whether the user is at least a chan owner
	* \return bool
	*/
	bool isChanOwner(const QString & szNick, bool bAtLeast = false);

	/**
	* \brief Returns true if the user is a chan admin
	* \param szNick The nick to moderate
	* \param bAtLeast Whether the user is at least a chan admin
	* \return bool
	*/
	bool isChanAdmin(const QString & szNick, bool bAtLeast = false);

	/**
	* \brief Returns true if the user is an operator
	* \param szNick The nick to moderate
	* \param bAtLeast Whether the user is at least an operator
	* \return bool
	*/
	bool isOp(const QString & szNick, bool bAtLeast = false);

	/**
	* \brief Returns true if the user is an half operator
	* \param szNick The nick to moderate
	* \param bAtLeast Whether the user is at least an half operator
	* \return bool
	*/
	bool isHalfOp(const QString & szNick, bool bAtLeast = false);

	/**
	* \brief Returns true if the user is a voice
	* \param szNick The nick to moderate
	* \param bAtLeast Whether the user is at least a voice
	* \return bool
	*/
	bool isVoice(const QString & szNick, bool bAtLeast = false);

	/**
	* \brief Returns true if the user is a user operator
	* \param szNick The nick to moderate
	* \param bAtLeast Whether the user is at least a user operator
	* \return bool
	*/
	bool isUserOp(const QString & szNick, bool bAtLeast = false);

	/**
	* \brief Sets the chan owner mode
	* \param szNick The nick to moderate
	* \param bChanOwner Whether to moderate the user
	* \return bool
	*/
	bool setChanOwner(const QString & szNick, bool bChanOwner);

	/**
	* \brief Sets the chan admin mode
	* \param szNick The nick to moderate
	* \param bChanAdmin Whether to moderate the user
	* \return bool
	*/
	bool setChanAdmin(const QString & szNick, bool bChanAdmin);

	/**
	* \brief Sets the operator mode
	* \param szNick The nick to moderate
	* \param bOp Whether to moderate the user
	* \return bool
	*/
	bool setOp(const QString & szNick, bool bOp);

	/**
	* \brief Sets the half operator mode
	* \param szNick The nick to moderate
	* \param bHalfOp Whether to moderate the user
	* \return bool
	*/
	bool setHalfOp(const QString & szNick, bool bHalfOp);

	/**
	* \brief Sets the voice mode
	* \param szNick The nick to moderate
	* \param bVoice Whether to moderate the user
	* \return bool
	*/
	bool setVoice(const QString & szNick, bool bVoice);

	/**
	* \brief Sets the user operator mode
	* \param szNick The nick to moderate
	* \param bUserOp Whether to moderate the user
	* \return bool
	*/
	bool setUserOp(const QString & szNick, bool bUserOp);

	/**
	* \brief Verifies the mask for a user action
	*
	* This is called when a user "acts" in some visible way on the channel,
	* so we can keep track of his channel idle time.
	* This particular function version of userAction will return false if
	* there was a user or hostname change (unless they were not known at all)
	* This will also update the username and hostname if needed.
	* \param szNick The nickname of the user
	* \param szUser The username of the user
	* \param szHost The hostname of the user
	* \param iActionTemperature The temperature of the action performed
	* \param szOldUser The old username of the user
	* \param szOldHost The old hostname of the user
	* \return bool
	*/
	bool userActionVerifyMask(const QString & szNick, const QString & szUser, const QString & szHost, int iActionTemperature, QString & szOldUser, QString & szOldHost);

	/**
	* \brief Verifies a user action
	*
	* This is called when a user "acts" in some visible way on the channel
	* In this way we can keep track of his channeel idle time. This will
	* also update the username and hostname if needed.
	* \param pUser The user by the irc mask
	* \param iActionTemperature The temperature of the action performed
	* \return void
	*/
	void userAction(KviIrcMask * pUser, int iActionTemperature);

	/**
	* \brief Verifies a user action
	*
	* This is called when a user "acts" in some visible way on the channel
	* In this way we can keep track of his channeel idle time. This will
	* also update the username and hostname if needed.
	* \param szNick The nickname of the user
	* \param szUser The username of the user
	* \param szHost The hostname of the user
	* \param iActionTemperature The temperature of the action performed
	* \return void
	*/
	void userAction(const QString & szNick, const QString & szUser, const QString & szHost, int iActionTemperature);

	/**
	* \brief Verifies a user action
	*
	* This is called when a user "acts" in some visible way on the channel
	* In this way we can keep track of his channeel idle time. This will
	* also update the username and hostname if needed.
	* \param szNick The nickname of the user
	* \param iActionTemperature The temperature of the action performed
	* \return void
	*/
	void userAction(const QString & szNick, int iActionTemperature);

	/**
	* \brief Changes a nickname
	* \param szOldNick The old nickname to change
	* \param szNewNick The new nickname
	* \return bool
	*/
	bool nickChange(const QString & szOldNick, const QString & szNewNick);

	/**
	* \brief Called when a user joins the channel
	* \param szNick The nickname of the user
	* \param szUser The username of the user
	* \param szHost The hostname of the user
	* \param iFlags The flags of the user
	* \return KviUserListEntry *
	*/
	KviUserListEntry * join(const QString & szNick, const QString & szUser = QString(), const QString & szHost = QString(), int iFlags = 0);

	/**
	* \brief Returns true if the avatar of a user is changed
	* \param szNick The nickname of the user
	* \return bool
	*/
	bool avatarChanged(const QString & szNick);

	/**
	* \brief Enables the updates on the view list area
	* \param bEnable Whether or not to enable updates
	* \return void
	*/
	void enableUpdates(bool bEnable);

	/**
	* \brief Sets the mask entries number for a channel
	*
	* The mask types can be:
	* - +b: ban
	* - +e: exception; a user can join a +b channel (ban) if there's a +e
	*       exception on a mask which the user matches
	* - +I: invite; a user can join a +i channel (invite) if there's a +I
	-       exception on a mask which the user matches
	* \param cType The type of the mask
	* \param iNum The number of the masks
	* \return void
	*/
	void setMaskEntries(char cType, int iNum);

	/**
	* \brief Emits the right click signal
	* \return void
	*/
	void emitRightClick();

	/**
	* \brief Emits the double click signal
	* \return void
	*/
	void emitDoubleClick();

	/**
	* \brief Completes the nick prioritizing last active first
	*/
	bool completeNickLastAction(const QString & szBegin, const QString & szSkipAfter, QString & szBuffer, bool bAppendMask);

	/**
	* \brief Completes the nick in normal behaviour
	*
	* It looks for the letters typed, if it found at least a result, it
	* cycles through the completed nicknames
	* \param szBegin The starting of the nick
	* \param szSkipAfter The string to skip after the nick is completed
	* \param szBuffer The buffer where to store the data
	* \param bAppendMask Whether to append the complete mask
	* \return bool
	*/
	bool completeNickStandard(const QString & szBegin, const QString & szSkipAfter, QString & szBuffer, bool bAppendMask);

	/**
	* \brief Completes the nick in Bash-like behaviour
	*
	* It looks for the letters typed, if it found only a result, it completes
	* the nickname; if it found more than a result, it will wrap will all the
	* possible results
	* \param szBegin The starting of the nick
	* \param pList The list of nicknames
	* \param bAppendMask Whether to append the complete mask
	* \return void
	*/
	void completeNickBashLike(const QString & szBegin, std::vector<QString> & pList, bool bAppendMask);

protected:
	/**
	* \brief Checks if user activated tooltips over the userlist
	* \param pTip The source tooltip to show
	* \param pnt The point where the mouse is
	* \return void
	*/
	void maybeTip(KviUserListToolTip * pTip, const QPoint & pnt);

	/**
	* \brief Updates the view list
	*
	* This function will updates the scrollbar range, the users label, and the
	* view area
	* \return void
	*/
	void triggerUpdate();

	/**
	* \brief Updates the users label
	* \return void
	*/
	void updateUsersLabel();

	/**
	* \brief Inserts a user into the users list
	* \param szNick The nickname of the user
	* \param pEntry The entry in the users list
	* \return void
	*/
	void insertUserEntry(const QString & szNick, KviUserListEntry * pEntry);

	/**
	* \brief Clears all channels entries
	*
	* All counters will be reset
	* \return void
	*/
	void removeAllEntries();

	/**
	* \brief Called when a user parts a channel
	* \param szNick The nickname of the user
	* \param bRemove Whether to remove the user from the irc user db
	* \return bool
	*/
	bool partInternal(const QString & szNick, bool bRemove = true);

	/**
	* \brief Sets the user database
	* \param pDb The source user database
	* \return void
	*/
	void setUserDataBase(KviIrcUserDataBase * pDb) { m_pIrcUserDataBase = pDb; };

	/**
	* \brief Updates the scrollbar range
	* \return void
	*/
	void updateScrollBarRange();

	void resizeEvent(QResizeEvent * e) override;

public slots:
	/**
	* \brief Called when an animated avatar is updated (every frame)
	* \return void
	*/
	void animatedAvatarUpdated(KviUserListEntry * e);
};

/**
* \class KviUserListViewArea
* \brief User list area management
*/
class KVIRC_API KviUserListViewArea : public QWidget
{
	friend class KviUserListView;

	Q_OBJECT
	Q_PROPERTY(int TransparencyCapable READ dummyRead)
public:
	/**
	* \brief Constructs the user listview area object
	* \param pPar The parent widget
	* \return KviUserListViewArea
	*/
	KviUserListViewArea(KviUserListView * pPar);

	/**
	* \brief Destroys the user listview are object
	*/
	~KviUserListViewArea();

public:
	/**
	* \brief A dummy function that always returns 0
	* \return int
	*/
	int dummyRead() const { return 0; };

protected:
	KviUserListView * m_pListView;
	KviUserListEntry * m_pLastEntryUnderMouse;
	QScrollBar * m_pScrollBar;
	int m_iLastScrollBarVal;
	int m_iTopItemOffset;
	bool m_bIgnoreScrollBar;

protected:
	void paintEvent(QPaintEvent * e) override;
	void resizeEvent(QResizeEvent * e) override;
	void mousePressEvent(QMouseEvent * e) override;
	void mouseMoveEvent(QMouseEvent * e) override;
	void mouseReleaseEvent(QMouseEvent * e) override;
	void mouseDoubleClickEvent(QMouseEvent * e) override;
	void wheelEvent(QWheelEvent * e) override;
	void keyPressEvent(QKeyEvent * e) override;
protected slots:

	/**
	* \brief Called when the scrollbar is moved
	* \param iNewVal The new value of the scrollbar
	* \return void
	*/
	void scrollBarMoved(int iNewVal);
};

#endif //_KVI_USERLISTVIEW_H_
