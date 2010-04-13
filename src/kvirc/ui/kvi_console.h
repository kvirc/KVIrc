#ifndef _KVI_CONSOLE_H_
#define _KVI_CONSOLE_H_
//=============================================================================
//
//   File : kvi_console.h
//   Creation date : Sun Jun 25 2000 15:00:20 by Szymon Stefanek
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

#include "kvi_settings.h"
#include "kvi_ircconnection.h"
#include "kvi_ircconnectiontarget.h"
#include "kvi_window.h"
#include "kvi_ircsocket.h"
#include "kvi_ircserver.h"
#include "kvi_string.h"
#include "kvi_irccontext.h"
#include "kvi_pointerlist.h"
#include "kvi_userlistview.h"
#include "kvi_themedcombobox.h"

#include <time.h>

#include <QToolButton>

class QToolBar;

class KviAvatar;
class KviDns;
class KviIrcUserDataBase;
class KviIrcUserEntry;
class KviServer;
class KviNetwork;
class KviProxy;
class KviUserListView;
class KviNotifyListManager;
class KviRegisteredUser;
class KviWindowToolPageButton;

#ifdef COMPILE_ON_WINDOWS
	// windoze wants it to compile QList<KviChannel> and QList<KviQuery>
	#include "kvi_channel.h"
	#include "kvi_query.h"
#else
	class KviChannel;
	class KviQuery;
#endif


//=================================================================================================
//
// The console window
//
//=================================================================================================

#define KVI_CONSOLE_FLAG_FIRSTINAPP 1
#define KVI_CONSOLE_FLAG_FIRSTINFRAME 2


class KVIRC_API KviConsole : public KviWindow
{
	friend class KviFrame;
	friend class KviIrcSocket;
	friend class KviChannel;
	friend class KviQuery;
	friend class KviIrcContext;
	Q_OBJECT
protected:
	KviConsole(KviFrame * lpFrm,int iFlags);
public:
	~KviConsole();
protected:
	int                                m_iFlags; // FIXME: make this a KviWindow property ?
	// UI
	KviUserListView         * m_pNotifyListView;
	KviWindowToolPageButton * m_pNotifyViewButton;
	KviThemedComboBox       * m_pAddressEdit;
	QString                   m_szStatusString; // nick (flags) on server | not connected
	QString                   m_szOwnSmartColor;
	QStringList             * m_pTmpHighLightedChannels;
	KviIrcContext           * m_pContext;
protected:
	// UI
	virtual QPixmap * myIconPtr();
	virtual void fillCaptionBuffers();
	virtual void resizeEvent(QResizeEvent *e);
	virtual void closeEvent(QCloseEvent *e);
	virtual void getBaseLogFileName(QString &buffer);
	virtual void getWindowListTipText(QString &buffer);
	virtual void fillContextPopup(KviTalPopupMenu * p);
	virtual QSize sizeHint() const;
	virtual void applyOptions();
	virtual void triggerCreationEvents();
	void fillStatusString();
	//void socketError(int iError);
	//void socketStateChange();
	//void registerLinkMonitor(KviIrcSocketMonitor * m);
	//void unregisterLinkMonitor(KviIrcSocketMonitor * m);
	virtual void loadProperties(KviConfig * cfg);
	virtual void saveProperties(KviConfig * cfg);

	void destroyConnection();
	// internal helper for applyHighlighting
	int triggerOnHighlight(KviWindow *wnd,int type,const QString &nick,const QString &user,const QString &host,const QString &szMsg,const QString &trigger);

	void showNotifyList(bool bShow);
	static int getSmartColorHashForNick(QString *szNick);
public:
	KviIrcContext * context(){ return m_pContext; };

	// UI
	inline KviUserListView * notifyListView(){ return m_pNotifyListView; };
	inline int selectedCount();


	//
	// State
	//
	inline KviIrcContext::State state(){ return context()->state(); };

	// these should disappear!
	inline bool isConnected(){ return context()->isConnected(); };
	inline bool isIPv6Connection();
	inline bool isNotConnected();
	bool connectionInProgress();
	//
	// This connection info
	//
	inline QString currentNetworkName();
	KviAvatar * currentAvatar();
	//
	// IRC Context wide helpers (connection related)
	//
	void getUserTipText(const QString &nick,KviIrcUserEntry *e,QString &buffer);
	enum OutputPrivmsgFlags {
		NoWindowFlashing = 1, // has meaning only if NoHighlighting is NOT given, otherwise it is implied
		NoNotifier = 2, // has meaning only if NoHighlighitng is NOT given, otherwise it is implied
		NoHighlighting = 4,
		NoNotifications = 7 // this is 1|2|4 implies NoWindowFlashing and NoNotifier
	};
	void outputPrivmsg(KviWindow *wnd,int type,const QString &nick,
		const QString &user,const QString &host,const QString &msg,int iFlags = 0,const QString &prefix = QString(),const QString &suffix = QString());
	// this applies highlighting to the specified message
	// and triggers the OnHighlight event.
	// it returns KVI_OUT_HIGHLIGHT if highlighting was applied
	// and -1 if OnHighlight called halt
	// otherwise it returns <type>
	int applyHighlighting(KviWindow *wnd,int type,const QString &nick,const QString &user,const QString &host,const QString &szMsg);
	// Avatar helpers (conneciton related)
	void resetAvatarForMatchingUsers(KviRegisteredUser * u);
	// this should be protected at least
	void avatarChangedUpdateWindows(const QString &nick,const QString &textLine);
	void avatarChanged(KviAvatar * avatar,const QString &nick,const QString &user,const QString &host,const QString &textLine);
	KviAvatar * setAvatar(const QString &nick,const QString &user,const QString &host,const QString &szLocalPath,const QString &szName);
	void checkDefaultAvatar(KviIrcUserEntry *e,const QString &nick,const QString &user,const QString &host);
	void setAvatarFromOptions();

	// This returns the default avatar for the current KVIrc user
	// if he has chosen a valid avatar in the options dialog
	// otherwise returns 0.
	// The avatar is allocated with new and must be deleted
	// when no longer needed.
	KviAvatar * defaultAvatarFromOptions();

	void terminateConnectionRequest(bool bForce = false,const char * quitMsg = 0);

	// Status string (usermode + nick) (connection related too)
	inline const QString & statusString(){ return m_szStatusString; };

	KviWindow * activeWindow();
	// User db, connection related
	void completeChannel(const QString &word,KviPointerList<QString> * matches);
	void completeServer(const QString &word,KviPointerList<QString> * matches);
	void connectionAttached();
	void connectionDetached();

	/**
	* \brief Adds a channel to the highlight list
	* \param szChan The channel name
	* \return void
	*/
	void addHighlightedChannel(const QString & szChan);

	/**
	* \brief Removes a channel from the highlight list
	* \param szChan The channel name
	* \return void
	*/
	void removeHighlightedChannel(const QString & szChan);

	/**
	* \brief Returns true if the channel is highlighted
	* \param szChan The channel name
	* \return bool
	*/
	bool isHighlightedChannel(const QString & szChan) { return m_pTmpHighLightedChannels->contains(szChan,Qt::CaseInsensitive); };

public slots:
	void updateUri();
	void executeInternalCommand(int index);
	void recentUrlsChanged();
protected slots:
	void ircUriChanged(const QString & );
	void toggleNotifyView();
	void textViewRightClicked();
};

inline int KviConsole::selectedCount()
{
	return m_pNotifyListView->selectedCount();
}

inline bool KviConsole::isIPv6Connection()
{
	__range_valid(connection());
	return connection()->target()->server()->isIPv6();
}

inline bool KviConsole::isNotConnected()
{
	return (context()->state() == KviIrcContext::Idle);
}

inline QString KviConsole::currentNetworkName()
{
	return (connection() ? connection()->networkName() : QString());
}

#endif //_KVI_CONSOLE_H_
