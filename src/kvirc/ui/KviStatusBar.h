#ifndef _KVI_STATUSBAR_H_
#define _KVI_STATUSBAR_H_
//=============================================================================
//
//   File : KviStatusBar.h
//   Creation date : Tue 07 Sep 2004 03:56:46 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek <pragma at kvirc dot net>
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
* \file KviStatusBar.h
* \author Szymon Stefanek
* \brief KVIrc Status Bar management
*
* \def VMARGIN Vertical margin
* \def HMARGIN Horizontal margin
* \def SPACING Spacing between applets
* \def RICHTEXTLABELTRICK
*/

#include "kvi_settings.h"
#include "KviHeapObject.h"
#include "KviPointerHashTable.h"
#include "KviPointerList.h"
#include "KviTalHBox.h"

#include <QStatusBar>
#include <QString>

class KviDynamicToolTip;
class KviIrcContext;
class KviMainWindow;
class KviStatusBarApplet;
class KviStatusBarAppletDescriptor;
class QHelpEvent;
class QLabel;
class QMenu;
class QTimer;

#define VMARGIN 3
//#define HMARGIN 4
//#define SPACING 3
#define RICHTEXTLABELTRICK 2

/**
* \class KviStatusBarMessage
* \brief A class that hold the statusbar message
*/
class KVIRC_API KviStatusBarMessage : public KviHeapObject
{
	friend class KviStatusBar;

protected:
	QString m_szText;
	unsigned int m_uTimeout;
	unsigned int m_uPriority;

public:
	/**
	* \brief Constructs the statusbar message object
	* \param szText The text of the message
	* \param uTimeout The timeout of the message
	* \param uPriority The priority of the message
	* \return KviStatusBarMessage
	*/
	KviStatusBarMessage(const QString & szText, unsigned int uTimeout = 8000, unsigned int uPriority = 0)
	    : KviHeapObject(), m_szText(szText), m_uTimeout(uTimeout), m_uPriority(uPriority){};

	/**
	* \brief Destroys the statusbar message object
	*/
	~KviStatusBarMessage(){};

public:
	/**
	* \brief Returns the text of the message
	* \return const QString &
	*/
	const QString & text() { return m_szText; };

	/**
	* \brief Returns the timeout of the message
	* \return unsigned int
	*/
	unsigned int timeout() { return m_uTimeout; };

	/**
	* \brief Returns the priority of the message
	* \return unsigned int
	*/
	unsigned int priority() { return m_uPriority; };
};

/**
* \class KviStatusBar
* \brief Status Bar class
*/
class KVIRC_API KviStatusBar : public QStatusBar
{
	friend class KviStatusBarApplet;
	friend class KviMainWindow;
	Q_OBJECT
public:
	/**
	* \brief Constructs the statusbar object
	* \param pFrame The parent frame class
	* \return KviStatusBar
	*/
	KviStatusBar(KviMainWindow * pFrame);

	/**
	* \brief Destroys the statusbar object
	*/
	~KviStatusBar();

protected:
	KviMainWindow * m_pFrame;
	KviPointerList<KviStatusBarMessage> * m_pMessageQueue;
	QTimer * m_pMessageTimer;
	QLabel * m_pMessageLabel;
	KviPointerList<KviStatusBarApplet> * m_pAppletList;
	KviPointerHashTable<QString, KviStatusBarAppletDescriptor> * m_pAppletDescriptors;
	QMenu * m_pContextPopup;
	QMenu * m_pAppletsPopup;
	KviStatusBarApplet * m_pClickedApplet;
	int m_iLastMinimumHeight;
	bool m_bStopLayoutOnAddRemove;

public:
	/**
	* \brief Returns the frame pointer
	* \return KviMainWindow *
	*/
	KviMainWindow * frame() { return m_pFrame; };

	/**
	* \brief Returns true if the applet exists, false otherwise
	* \param pApplet The pointer to the applet to check existence
	* \return bool
	*/
	bool appletExists(KviStatusBarApplet * pApplet);

	/**
	* \brief Returns the applet at the given point
	*
	* If bBestMatch is specified and no applets are found, the last applet
	* will be returned
	* \param pnt The source point
	* \param bBestMatch Whether to return the best match
	* \return KviStatusBarApplet *
	* \warning pnt is global!
	*/
	KviStatusBarApplet * appletAt(const QPoint & pnt, bool bBestMatch = false);

	//QMenu * contextPopup();

	/**
	* \brief Queue a statusbar message in the stack
	* \param pMsg The pointer to the message to queue
	* \return void
	* \warning It takes the ownership of pMsg
	*/
	void queueMessage(KviStatusBarMessage * pMsg);

	/**
	* \brief Registers the applet descriptor
	* \param d The applet description to register
	* \return void
	*/
	void registerAppletDescriptor(KviStatusBarAppletDescriptor * d);

	/**
	* \brief Inserts an applet at the end of the statusbar as a permanent widget and returns its index.
	* \param widget The widget to insert
	* \param stretch Stretch factor used to compute widget size when the statusbar gets resized. Defaults to 0.
	* \return int
	*/
	int insertPermanentWidgetAtTheEnd(QWidget * widget, int stretch = 0);

	//void addApplet(KviStatusBarApplet * pApplet);
	//void removeApplet(KviStatusBarApplet * pApplet);
protected:
	/**
	* \brief Shows the first message in the queue stack
	* \return void
	*/
	void showFirstMessageInQueue();

	/**
	* \brief Registers the given applet in the statusbar
	* \param pApplet The pointer to the applet
	* \return void
	*/
	void registerApplet(KviStatusBarApplet * pApplet);

	/**
	* \brief Unegisters the given applet in the statusbar
	* \param pApplet The pointer to the applet
	* \return void
	*/
	void unregisterApplet(KviStatusBarApplet * pApplet);

	/**
	* \brief Recalculates the minimum height of the status bar
	* \return void
	*/
	void recalcMinimumHeight();

	/**
	* \brief Saves the statusbar stack
	* \return void
	*/
	void save();

	/**
	* \brief Saves the statusbar stack
	* \return void
	*/
	void load();

	/**
	* \brief Create an applet
	*
	* It searches for the given name and if found in the applet
	* descriptor stack, it will passes creates the applet
	* \param szInternalName The internal name of the applet
	* \return KviStatusBarApplet *
	*/
	KviStatusBarApplet * createApplet(const QString & szInternalName);

	/**
	* \brief Shows the common help for every applet
	* \return void
	*/
	void showLayoutHelp();

	/**
	* \brief Shows the right tooltip
	*
	* If an applet is found at the position of the triggered event, the
	* right tooltip will be show, otherwise a common one will be show
	* \param e The event triggered
	* \return void
	*/
	void tipRequest(QHelpEvent * e);
protected slots:
	/**
	* \brief Called when the timer timeouts
	*
	* Remove the first item from the stack and sets the second one as
	* permanent
	* \return void
	*/
	void messageTimerFired();

	/**
	* \brief Called when the user requests a context menu
	* \param pos The position of the mouse
	* \return void
	*/
	void contextMenuRequested(const QPoint & pos);

	/**
	* \brief Shows the applet common popup menu
	* \return void
	*/
	void contextPopupAboutToShow();

	/**
	* \brief Shows the per-applet popup menu
	* \return void
	*/
	void appletsPopupAboutToShow();

	/**
	* \brief Called when the user activates the per-applet popup
	* \param iId The id of the applet
	* \return void
	*/
	void appletsPopupActivated(QAction * pAction);

	/**
	* \brief Called when the user removes an applet from the statusbar
	* \return void
	*/
	void removeClickedApplet();

	/**
	* \brief Sets the permanent message to be shown
	*
	* The message varies upon the connection status
	* \return void
	*/
	void setPermanentMessage();

protected:
	void mousePressEvent(QMouseEvent * e) override;
	void mouseDoubleClickEvent(QMouseEvent * e) override;
	bool event(QEvent * e) override;
	void dropEvent(QDropEvent * de) override;
	void dragMoveEvent(QDragMoveEvent * de) override;
	void dragEnterEvent(QDragEnterEvent * event) override;
};

#endif // _KVI_STATUSBAR_H_
