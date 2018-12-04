#ifndef _KVI_CONSOLE_H_
#define _KVI_CONSOLE_H_
//=============================================================================
//
//   File : KviConsoleWindow.h
//   Creation date : Sun Jun 25 2000 15:00:20 by Szymon Stefanek
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

#include "kvi_settings.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionTarget.h"
#include "KviWindow.h"
#include "KviIrcSocket.h"
#include "KviIrcServer.h"
#include "KviCString.h"
#include "KviIrcContext.h"
#include "KviUserListView.h"
#include "KviThemedComboBox.h"

#include <time.h>
#include <vector>

#include <QToolButton>

class QToolBar;

class KviAvatar;
class KviDnsResolver;
class KviIrcUserDataBase;
class KviIrcUserEntry;
class KviIrcServer;
class KviIrcNetwork;
class KviProxy;
class KviUserListView;
class KviNotifyListManager;
class KviRegisteredUser;
class KviWindowToolPageButton;

#ifdef COMPILE_ON_WINDOWS
// windoze wants it to compile QList<KviChannelWindow> and QList<KviQueryWindow>
#include "KviChannelWindow.h"
#include "KviQueryWindow.h"
#else
class KviChannelWindow;
class KviQueryWindow;
#endif

//
// The console window
//

#define KVI_CONSOLE_FLAG_FIRSTINAPP 1
#define KVI_CONSOLE_FLAG_FIRSTINFRAME 2

class KVIRC_API KviConsoleWindow : public KviWindow
{
	friend class KviMainWindow;
	friend class KviIrcSocket;
	friend class KviChannelWindow;
	friend class KviQueryWindow;
	friend class KviIrcContext;
	Q_OBJECT
protected:
	KviConsoleWindow(int iFlags);

public:
	~KviConsoleWindow();

protected:
	int m_iFlags; // FIXME: make this a KviWindow property ?
	// UI
	KviUserListView * m_pNotifyListView;
	KviWindowToolPageButton * m_pNotifyViewButton;
	KviThemedComboBox * m_pAddressEdit;
	QString m_szStatusString; // nick (flags) on server | not connected
	QString m_szOwnSmartColor;
	QStringList * m_pTmpHighLightedChannels;
	KviIrcContext * m_pContext;
	QList<int> m_SplitterSizesList;

protected:
	// UI
	QPixmap * myIconPtr() override;
	void fillCaptionBuffers() override;
	void resizeEvent(QResizeEvent * e) override;
	void closeEvent(QCloseEvent * e) override;
	void getBaseLogFileName(QString & buffer) override;
	void getWindowListTipText(QString & buffer) override;
	QSize sizeHint() const override;
	void applyOptions() override;
	void triggerCreationEvents() override;
	void fillStatusString();
	//void socketError(int iError);
	//void socketStateChange();
	//void registerLinkMonitor(KviIrcSocketMonitor * m);
	//void unregisterLinkMonitor(KviIrcSocketMonitor * m);
	void loadProperties(KviConfigurationFile * cfg) override;
	void saveProperties(KviConfigurationFile * cfg) override;

	void destroyConnection();
	// internal helper for applyHighlighting
	int triggerOnHighlight(KviWindow * wnd, int type, const QString & nick, const QString & user, const QString & host, const QString & szMsg, const QString & trigger);

	void showNotifyList(bool bShow, bool bIgnoreSizeChange = false);
	static int getSmartColorHashForNick(QString * szNick);

public:
	KviIrcContext * context() { return m_pContext; };

	// UI
	KviUserListView * notifyListView() const { return m_pNotifyListView; }
	int selectedCount();

	//
	// State
	//
	KviIrcContext::State state() { return context()->state(); }

	// these should disappear!
	bool isConnected() { return context()->isConnected(); }
	bool isIPv6Connection();
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
	void getUserTipText(const QString & nick, KviIrcUserEntry * e, QString & buffer);
	enum OutputPrivmsgFlags
	{
		NoWindowFlashing = 1, // has meaning only if NoHighlighting is NOT given, otherwise it is implied
		NoNotifier = 2,       // has meaning only if NoHighlighitng is NOT given, otherwise it is implied
		NoHighlighting = 4,
		NoNotifications = 7 // this is 1|2|4 implies NoWindowFlashing and NoNotifier
	};
	void outputPrivmsg(KviWindow * wnd, int type, const QString & nick,
	    const QString & user, const QString & host, const QString & msg, int iFlags = 0, const QString & prefix = QString(), const QString & suffix = QString(), const QDateTime & datetime = QDateTime());
	// this applies highlighting to the specified message
	// and triggers the OnHighlight event.
	// it returns KVI_OUT_HIGHLIGHT if highlighting was applied
	// and -1 if OnHighlight called halt
	// otherwise it returns <type>
	int applyHighlighting(KviWindow * wnd, int type, const QString & nick, const QString & user, const QString & host, const QString & szMsg);
	// Avatar helpers (connection related)
	void resetAvatarForMatchingUsers(KviRegisteredUser * u);
	// this should be protected at least
	void avatarChangedUpdateWindows(const QString & nick, const QString & textLine);
	void avatarChanged(KviAvatar * avatar, const QString & nick, const QString & user, const QString & host, const QString & textLine);
	KviAvatar * setAvatar(const QString & nick, const QString & user, const QString & host, const QString & szLocalPath, const QString & szName);
	void checkDefaultAvatar(KviIrcUserEntry * e, const QString & nick, const QString & user, const QString & host);
	void setAvatarFromOptions();

	// This returns the default avatar for the current KVIrc user
	// if he has chosen a valid avatar in the options dialog
	// otherwise returns 0.
	// The avatar is allocated with new and must be deleted
	// when no longer needed.
	KviAvatar * defaultAvatarFromOptions();

	void terminateConnectionRequest(bool bForce = false, const char * quitMsg = nullptr);

	// Status string (usermode + nick) (connection related too)
	const QString & statusString() const { return m_szStatusString; }

	KviWindow * activeWindow();
	// User db, connection related
	void completeChannel(const QString & word, std::vector<QString> & matches);
	void completeServer(const QString & word, std::vector<QString> & matches);
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
	bool isHighlightedChannel(const QString & szChan) { return m_pTmpHighLightedChannels->contains(szChan, Qt::CaseInsensitive); };

public slots:
	void updateUri();
	void executeInternalCommand(int index);
	void recentUrlsChanged();
protected slots:
	void ircUriChanged(const QString &);
	void toggleNotifyView();
	void textViewRightClicked();
};

inline int KviConsoleWindow::selectedCount()
{
	return m_pNotifyListView->selectedCount();
}

inline bool KviConsoleWindow::isIPv6Connection()
{
	KVI_ASSERT(connection());
	return connection()->target()->server()->isIPv6();
}

inline bool KviConsoleWindow::isNotConnected()
{
	return (context()->state() == KviIrcContext::Idle);
}

inline QString KviConsoleWindow::currentNetworkName()
{
	return (connection() ? connection()->currentNetworkName() : QString());
}

#endif //_KVI_CONSOLE_H_
