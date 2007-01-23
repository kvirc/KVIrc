#ifndef _KVI_CONSOLE_H_
#define _KVI_CONSOLE_H_
//=============================================================================
//
//   File : kvi_console.h
//   Creation date : Sun Jun 25 2000 15:00:20 by Szymon Stefanek
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#include "kvi_settings.h"
#include "kvi_ircconnection.h"
#include "kvi_window.h"
#include "kvi_ircsocket.h"
#include "kvi_string.h"
#include "kvi_irccontext.h"

#include "kvi_list.h"

#include <time.h>
#include <qcombobox.h>


class QToolBar;
class QToolButton;

class KviAvatar;
class KviDns;
class KviIrcUserDataBase;
class KviIrcUserEntry;
class KviIrcServer;
class KviIrcNetwork;
class KviProxy;
#ifndef COMPILE_ON_WINDOWS
	class KviChannel;
	class KviQuery;
#else
	// windoze wants it to compile QList<KviChannel> and QList<KviQuery>
	#include "kvi_channel.h"
	#include "kvi_query.h"
#endif
class KviUserListView;
class KviNotifyListManager;
class KviRegisteredUser;
class KviWindowToolPageButton;



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
	KviUserListView                  * m_pNotifyListView;
	KviWindowToolPageButton          * m_pNotifyViewButton;
	QComboBox						 * m_pAddressEdit;
	QString                            m_szStatusString; // nick (flags) on server | not connected
protected:
	// UI
	virtual QPixmap * myIconPtr();
	virtual void fillCaptionBuffers();
	virtual void resizeEvent(QResizeEvent *e);
	virtual void closeEvent(QCloseEvent *e);
	virtual void getBaseLogFileName(KviStr &buffer);
	virtual void getBaseLogFileName(QString &buffer);
	virtual void getTaskBarTipText(QString &buffer);
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

public:
	// UI
	KviUserListView * notifyListView(){ return m_pNotifyListView; };
	int selectedCount();


	//
	// State
	//
	KviIrcContext::State state(){ return context()->state(); };
	KVI_DEPRECATED KviIrcContext * ircContext(){ return context(); };
	unsigned int ircContextId() KVI_DEPRECATED;
	//
	// Sock state
	//
	KviIrcSocket * socket() KVI_DEPRECATED;
	
	// these should disappear!
	bool isConnected(){ return context()->isConnected(); };
	bool isIpV6Connection();
	bool isNotConnected();
	bool connectionInProgress();
	//
	// This connection info
	//
	QString currentNetworkName();
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
		const QString &user,const QString &host,const QString &msg,int iFlags = 0,const QString &prefix = QString::null,const QString &suffix = QString::null);
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
	// if he has choosen a valid avatar in the options dialog
	// otherwise returns 0.
	// The avatar is allocated with new and must be deleted
	// when no longer needed.
	KviAvatar * defaultAvatarFromOptions();

	void terminateConnectionRequest(bool bForce = false,const char * quitMsg = 0);

	// Status string (usermode + nick) (connection related too)
	const QString & statusString(){ return m_szStatusString; };
	
	// forwarders from KviIrcConnection
	KVI_DEPRECATED KviPtrList<KviChannel> * channelList(){ return connection() ? connection()->channelList() : 0; };
	KVI_DEPRECATED KviPtrList<KviQuery> * queryList(){ return connection() ? connection()->queryList() : 0; };
	KVI_DEPRECATED unsigned int channelCount(){ return (connection() ? connection()->channelList()->count() : 0); };
	KVI_DEPRECATED unsigned int queryCount(){ return (connection() ? connection()->queryList()->count() : 0); };
	
	// Window management
	//KVI_DEPRECATED KviChannel * findChannel(const char * name){ return connection() ? connection()->findChannel(name) : 0; };
	//KVI_DEPRECATED KviQuery * findQuery(const char * nick){ return connection() ? connection()->findQuery(nick) : 0; };

	KviWindow * activeWindow();
	// User db, connection related
	void completeChannel(const QString &word,KviPtrList<QString> * matches);
	void completeServer(const QString &word,KviPtrList<QString> * matches);
	void connectionAttached();
	void connectionDetached();
public slots:
	void updateUri();
	void executeInternalCommand(int index);
	void recentUrlsChanged();
protected slots:
	void ircUriChanged(const QString & );
	void toggleNotifyView();
	void textViewRightClicked();
};

#endif //_KVI_CONSOLE_H_
