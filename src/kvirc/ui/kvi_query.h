#ifndef _KVI_QUERY_H_
#define _KVI_QUERY_H_
//=============================================================================
//
//   File : kvi_query.h
//   Creation date : Mon Aug 7 2000 14:19:00 by Szymon Stefanek
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
* \file kvi_query.h
* \author Szymon Stefanek
* \brief Query widget: abstraction of an IRC query
*/

#include "kvi_settings.h"
#include "kvi_window.h"
#include "kvi_string.h"
#include "kvi_ircuserdb.h"
#include "kvi_userlistview.h"
#include "kvi_themedlabel.h"
#include "kvi_pointerhashtable.h"

class KviConsole;
class KviConfig;
class KviWindowToolPageButton;

/**
* \def KVI_QUERY_FLAG_DEAD When the query is dead
*/
#define KVI_QUERY_FLAG_DEAD 1

/**
* \class KviQuery
* \brief The class which manages a query
*/
class KVIRC_API KviQuery : public KviWindow
{
	Q_OBJECT
public:
	/**
	* \brief Constructs the query object
	* \param lpFrm The parent frame
	* \param lpConsole The associated console
	* \param szNick The nickname with who we open a query
	* \return KviQuery
	*/
	KviQuery(KviFrame * lpFrm, KviConsole * lpConsole, const QString & szNick);

	/**
	* \brief Destroys the query object
	*/
	~KviQuery();
protected:
	KviUserListView                     * m_pUserListView;
	KviWindowToolPageButton             * m_pListViewButton;
	int                                   m_iFlags;
	KviThemedLabel                      * m_pLabel;
	QFrame                              * m_pButtonGrid;
public:
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
	bool isDeadQuery(){ return m_iFlags & KVI_QUERY_FLAG_DEAD; };

	/**
	* \brief Returns the size of the query object
	* \return QSize
	*/
	virtual QSize sizeHint() const;

	/**
	* \brief Returns the name of the query target
	* \return const QString &
	*/
	virtual const QString & target(){ return windowName(); };

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
	QFrame * buttonContainer(){ return (QFrame*)m_pButtonGrid; };

	/**
	* \brief Applies the options
	* \return void
	*/
	virtual void applyOptions();

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
	* \brief Returns the number of selected users in the userlist
	* \return int
	*/
	int selectedCount(){ return m_pUserListView->selectedCount(); };

	/**
	* \brief Called when the target changes the avatar
	* \param szNick The nickname of the user
	* \return bool
	*/
	bool avatarChanged(const QString & szNick){ return m_pUserListView->avatarChanged(szNick); };

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
	* \return void
	*/
	void showListView(bool bShow);

	/**
	* \brief Merges a query into the one we're using
	* \param q The source query
	* \return void
	*/
	void mergeQuery(KviQuery * q);

	/**
	* \brief Updates the text of the tooltip
	* \return void
	*/
	void updateLabelText();

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
	* \brief Gets the info of the tooltip text
	* \return QString
	*/
	QString getInfoLabelTipText();

	/**
	* \brief Returns the icon associated to the query
	* \return QPixmap *
	*/
	virtual QPixmap * myIconPtr();

	/**
	* \brief Fills in the caption buffers
	* \return void
	*/
	virtual void fillCaptionBuffers();

	/**
	* \brief Loads the properties from file
	* \param cfg The configuration file
	* \return void
	*/
	virtual void loadProperties(KviConfig * cfg);

	/**
	* \brief Saves the properties to file
	* \param cfg The configuration file
	* \return void
	*/
	virtual void saveProperties(KviConfig * cfg);

	/**
	* \brief Gets the base of the log file name
	* \param szBuffer The buffer where to save the info
	* \return void
	*/
	virtual void getBaseLogFileName(QString & szBuffer);

	/**
	* \brief Trigger the OnQueryWindowCreated event
	* \return void
	*/
	virtual void triggerCreationEvents();

	virtual void resizeEvent(QResizeEvent * e);
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
	* \param pcFile The file dropped in the query
	* \return void
	*/
        void slotDndEvents(const QString &pcFile);
};

#endif //_KVI_CHANNEL_H_
