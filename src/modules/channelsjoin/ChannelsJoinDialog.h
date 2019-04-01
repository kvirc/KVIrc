#ifndef _CHANNELSJOIN_H_
#define _CHANNELSJOIN_H_
//=============================================================================
//
//   File : ChannelsJoinDialog.h
//   Creation date : Thu Nov 06 2001 12:30:25 CEST by Juan Alvarez
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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
* \file ChannelsJoinDialog.h
* \author Juan Alvarez
* \brief Channels join window
*/

#include "KviWindow.h"

#include <QDialog>
#include <QMenu>
#include <QTreeWidget>

class KviConsoleWindow;
class QCheckBox;
class QGroupBox;
class QLineEdit;
class QPushButton;
class QString;

/**
* \class ChannelsJoinDialogTreeWidget
* \brief Channels join window tree widget
*/
class ChannelsJoinDialogTreeWidget : public QTreeWidget
{
	Q_OBJECT
public:
	ChannelsJoinDialogTreeWidget(QWidget * par)
	    : QTreeWidget(par){};

	~ChannelsJoinDialogTreeWidget()
	{
		if(m_pJoinPopup)
			delete m_pJoinPopup;
	};

protected:
	QMenu * m_pJoinPopup = nullptr;
	/**
	* \brief Called when the user clicks on the list
	* \param e mouse event descriptor
	* \return void
	*/
	void mousePressEvent(QMouseEvent * e) override;
	void mouseDoubleClickEvent(QMouseEvent * e) override;
};

/**
* \class ChannelsJoinDialog
* \brief Channels join window class
*/
class ChannelsJoinDialog : public QDialog
{
	Q_OBJECT
	friend class ChannelsJoinDialogTreeWidget;

public:
	/**
	* \brief Constructs the channels join window
	* \param name The name of the window
	* \return ChannelsJoinDialog
	*/
	ChannelsJoinDialog(const char * name);

	/**
	* \brief Destroys the channels join window
	*/
	~ChannelsJoinDialog();

protected:
	enum ItemTypes
	{
		HeaderItem,
		RecentChannelItem,
		RegisteredChannelItem
	};
	QLineEdit * m_pChannelEdit = nullptr;
	ChannelsJoinDialogTreeWidget * m_pTreeWidget = nullptr;
	QGroupBox * m_pGroupBox = nullptr;
	QLineEdit * m_pPass = nullptr;
	QCheckBox * m_pShowAtStartupCheck = nullptr;
	QCheckBox * m_pCloseAfterJoinCheck = nullptr;
	QPushButton * m_pJoinButton = nullptr;
	QPushButton * m_pRegButton = nullptr;
	QPushButton * m_pClearButton = nullptr;
	KviConsoleWindow * m_pConsole = nullptr;

public:
	void closeEvent(QCloseEvent * e) override;

	/**
	* \brief Fills in the servers' list
	* \return void
	*/
	void fillListView();

	/**
	* \brief Enables join and register button if channel is found
	* \return void
	*/
	void enableJoin();

	/**
	* \brief Sets the console and fills in the servers' list
	* \param pConsole The source console
	* \return void
	*/
	void setConsole(KviConsoleWindow * pConsole);

	/**
	* \brief Called when the user choose an item from the list
	* \return void
	*/
	void itemSelected();
protected slots:
	/**
	* \brief Called when the text changes
	* \return void
	*/
	void editTextChanged(const QString &);

	/**
	* \brief Called when the return is pressed
	*
	* It joins the selected channel
	* \return void
	*/
	void editReturnPressed();

	/**
	* \brief Called when the cancel button is pressed
	*
	* It destroys the window object
	* \return void
	*/
	void cancelClicked();

	/**
	* \brief Called when the join button is pressed
	*
	* It joins the selected channel
	* \return void
	*/
	void joinClicked();

	/**
	* \brief Called when the delete button is pressed
	*
	* It deletes the selected channel
	* \return void
	*/
	void deleteClicked();

	/**
	* \brief Called when the register button is pressed
	*
	* It registers the selected channel in the database
	* \return void
	*/
	void regClicked();

	/**
	* \brief Called when the clear button is pressed
	*
	* It clears the list of recent channels
	* \return void
	*/
	void clearClicked();

	//void whoClicked();
	//void namesClicked();
};

#endif //_CHANNELSJOINSWINDOW_H_
