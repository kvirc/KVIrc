#ifndef _KVI_QUERY_H_
#define _KVI_QUERY_H_
//=============================================================================
//
//   File : KviQueryWindow.h
//   Creation date : Mon Aug 7 2000 14:19:00 by Szymon Stefanek
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
* \file KviQueryWindow.h
* \author Szymon Stefanek
* \brief Query widget: abstraction of an IRC query
*/

#include "kvi_settings.h"
#include "KviWindow.h"
#include "KviIrcUserDataBase.h"
#include "KviUserListView.h"
#include "KviThemedLabel.h"
#include "KviPointerHashTable.h"

class KviConfigurationFile;
class KviConsoleWindow;
class KviWindowToolPageButton;
class QString;

/**
* \class KviQueryWindow
* \brief The class which manages a query
*/
class KVIRC_API KviQueryWindow : public KviWindow
{
	Q_OBJECT
public:
	/**
	* \enum Flag
	* \brief Holds the flags of a query
	*/
	enum Flag
	{
		Dead = 1
	};

	/**
	* \brief Constructs the query object
	* \param lpConsole The associated console
	* \param szNick The nickname with who we open a query
	* \return KviQueryWindow
	*/
	KviQueryWindow(KviConsoleWindow * lpConsole, const QString & szNick);

	/**
	* \brief Destroys the query object
	*/
	~KviQueryWindow();

protected:
	KviUserListView * m_pUserListView;
	KviWindowToolPageButton * m_pListViewButton;
	int m_iFlags;
	KviThemedLabel * m_pLabel;
	QFrame * m_pButtonGrid;
	QList<int> m_SplitterSizesList;

public:
	/**
	* \brief Returns the userlist of the query (the remote peer)
	* \return KviUserListView *
	*/
	KviUserListView * userListView() { return m_pUserListView; };

	/**
	* \brief Sets the status of the query as dead
	* \return void
	*/
	void setDeadQuery();

	/**
	* \brief Sets the status of the query as alive
	* \return void
	*/
	void setAliveQuery();

	/**
	* \brief Returns true if the query is dead
	* \return bool
	*/
	bool isDeadQuery() { return m_iFlags & Dead; };

	/**
	* \brief Returns the size of the query object
	* \return QSize
	*/
	QSize sizeHint() const override;

	/**
	* \brief Returns the name of the query target
	* \return const QString &
	*/
	const QString & target() override { return windowName(); }

	/**
	* \brief Sets the target of the query
	* \param szNick The nickname of the user
	* \param szUser The username of the user
	* \param szHost The hostname of the user
	* \return KviUserListEntry *
	*/
	KviUserListEntry * setTarget(const QString & szNick, const QString & szUser, const QString & szHost);

	/**
	* \brief Returns the button container
	* \return QFrame *
	*/
	QFrame * buttonContainer() override { return (QFrame *)m_pButtonGrid; }

	/**
	* \brief Applies the options
	* \return void
	*/
	void applyOptions() override;

	/**
	* \brief Called when a user performs an action
	* \param user The source user
	* \param uActionType The type of the action
	* \return void
	*/
	void userAction(KviIrcMask * user, unsigned int uActionType);

	/**
	* \brief Called when a user performs an action
	* \param szNick The nickname of the user
	* \param uActionType The type of the action
	* \return void
	*/
	void userAction(const QString & szNick, unsigned int uActionType);

	/**
	* \brief Called when a user performs an action
	* \param szNick The nickname of the user
	* \param szUser The username of the user
	* \param szHost The hostname of the user
	* \param uActionType The type of the action
	* \return void
	*/
	void userAction(const QString & szNick, const QString & szUser, const QString & szHost, unsigned int uActionType);

	/**
	* \brief Called when the target changes the nick
	* \param szOldNick The old nickname of the user
	* \param szNewNick The new nickname of the user
	* \return bool
	*/
	bool nickChange(const QString & szOldNick, const QString & szNewNick);

	/**
	* \brief Called when we send a message to the query target
	* \param szBuffer The buffer where to store the data
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
	* \brief Returns the number of selected users in the userlist
	* \return int
	*/
	int selectedCount() { return m_pUserListView->selectedCount(); };

	/**
	* \brief Called when the target changes the avatar
	* \param szNick The nickname of the user
	* \return bool
	*/
	bool avatarChanged(const QString & szNick) { return m_pUserListView->avatarChanged(szNick); };

	/**
	* \brief Notify us if we have common channels with the target
	* \param szNick The nickname of the user
	* \param szUser The username of the user
	* \param szHost The hostname of the user
	* \param iChans The number of common channels
	* \param szChans The common channels
	* \return void
	*/
	void notifyCommonChannels(const QString & szNick, const QString & szUser, const QString & szHost, int iChans, const QString & szChans);

	/**
	* \brief Shows the userlist view
	* \param bShow Whether to show or hide the view
	* \param bIgnoreSizeChange Whether to ignore the size change
	* \return void
	*/
	void showListView(bool bShow, bool bIgnoreSizeChange = false);

	/**
	* \brief Merges a query into the one we're using
	* \param pQuery The source query
	* \return void
	*/
	void mergeQuery(KviQueryWindow * pQuery);

	/**
	* \brief Updates the text of the tooltip
	* \return void
	*/
	void updateLabelText();

protected:
	/**
	* \brief Called to notify us that the target of the query is changed
	* \param szOldNick The old nickname of the target
	* \param szOldUser The old username of the target
	* \param szOldHost The old hostname of the target
	* \param szNick The new nickname of the target
	* \param szUser The new username of the target
	* \param szHost The new hostname of the target
	* \return void
	*/
	void notifyTargetChange(const QString & szOldNick, const QString & szOldUser, const QString & szOldHost, const QString & szNick, const QString & szUser, const QString & szHost);

	/**
	* \brief Gets the info of the text in the upper part of the query
	* \return QString
	*/
	QString getInfoLabelText();

	/**
	* \brief Gets the window list and info label tooltip text
	* \return QString
	*/
	void getWindowListTipText(QString & szBuffer) override;

	/**
	* \brief Returns the icon associated to the query
	* \return QPixmap *
	*/
	QPixmap * myIconPtr() override;

	/**
	* \brief Fills in the caption buffers
	* \return void
	*/
	void fillCaptionBuffers() override;

	/**
	* \brief Loads the properties from file
	* \param pCfg The configuration file
	* \return void
	*/
	void loadProperties(KviConfigurationFile * pCfg) override;

	/**
	* \brief Saves the properties to file
	* \param pCfg The configuration file
	* \return void
	*/
	void saveProperties(KviConfigurationFile * pCfg) override;

	/**
	* \brief Gets the base of the log file name
	* \param szBuffer The buffer where to save the info
	* \return void
	*/
	void getBaseLogFileName(QString & szBuffer) override;

	/**
	* \brief Trigger the OnQueryWindowCreated event
	* \return void
	*/
	void triggerCreationEvents() override;

	void resizeEvent(QResizeEvent *) override;
protected slots:
	/**
	* \brief Triggers the OnQueryPopupRequest event
	* \return void
	*/
	void textViewRightClicked();

	/**
	* \brief Toggles the userlist view
	* \return void
	*/
	void toggleListView();

	/**
	* \brief Triggers the OnQueryFileDropped event
	* \param szFile The file dropped in the query
	* \return void
	*/
	void slotDndEvents(const QString & szFile);
};

#endif //_KVI_CHANNEL_H_
