#ifndef _KVI_MDICHILD_H_
#define _KVI_MDICHILD_H_
//=============================================================================
//
//   File : kvi_mdichild.h
//   Creation date : Wed Jun 21 2000 17:35:04 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2008 TheXception (kvirc at thexception dot net)
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

/**
* \file kvi_mdichild.h
* \brief MDI subwindow stuff
* \author Szymon Stefanek
*/

#include "kvi_settings.h"
#include "kvi_string.h"
#include "kvi_tal_popupmenu.h"

#include <QFrame>
#include <QLabel>
#include <QLabel>
#include <QToolButton>
#include <QMdiSubWindow>
#include <QWidget>
#include <QWindowStateChangeEvent>
#include <QTimer>

class KviMdiManager;
class KviMdiChild;
class QCursor;

/**
* \class KviMdiChild
* \brief Class of an MDI  window
*/
class KVIRC_API KviMdiChild : public QMdiSubWindow
{
	friend class KviMdiManager;
	Q_OBJECT
public:
	/**
	* \brief Constructs the Mdi child object
	* \param par The mdi-manager the mdi-child belongs to.
	* \param name The name of the mdi-child
	* \return KviMdiChild
	*/
	KviMdiChild(KviMdiManager * pPar, const char * pcName = 0);

	/**
	* \brief Destroys the Mdi child object
	*/
	~KviMdiChild();
public:
	/**
	* \enum MdiChildState
	* \brief This is the state of the child window
	*/
	enum MdiChildState {
		Maximized = 0, /**< The child window is maximized */
		Minimized,     /**< The child window is minimized */
		Normal         /**< The child window is normal */
	};
protected:
	/// Contains the according KviMdiManager object.
	KviMdiManager * m_pManager;
private:
	/// The widget docked into our MDI window.
	QWidget       * m_pClient;
	/// Saves the geometry of the window for restoring it's position
	QRect           m_restoredGeometry;
	/// Saves the last state of our window
	MdiChildState   m_State;
	/// Saves the last state before window is minimized
	MdiChildState   m_LastState;
	/// Contains the current window Icon
	QPixmap         m_pIcon;
	/// XML-Version of active Caption.
	QString         m_szXmlActiveCaption;
	/// XML-Version of inactive Caption.
	QString         m_szXmlInactiveCaption;
	/// Caption as plain text.
	QString         m_szPlainCaption;
	/// If window can be closed or not
	bool            m_bCloseEnabled;
private:
	/**
	* \brief Updates the caption of the child
	* \return void
	*/
	void updateCaption();
public:
	/**
	* \brief Read the current geometry
	* \return QRect
	*/
	QRect restoredGeometry();

	/**
	* \brief Sets the new geomentry
	* \param r The geometry the window will take when it will be restored
	* \return void
	*/
	void setRestoredGeometry(const QRect & r);

	/**
	* \brief Sets the client widget which is shown in the subwindow
	* \param w The new client widget which is docked into our KviMdiChild
	* \return void
	*/
	void setClient(QWidget * w);

	/**
	* \brief Returns the currently in the window visible widget
	* \return QWidget *
	*/
	QWidget * client() { return m_pClient; };

	/**
	* \brief Removes the client widget
	* \return void
	*/
	void unsetClient();

	/**
	* \brief Gets the current window state
	* \return MdiChildState
	*/
	MdiChildState state();

	/**
	* \brief Returns plain text caption
	* \return const QString &
	*/
	const QString & plainCaption() { return m_szPlainCaption; };

	/**
	* \brief Returns XML-caption string for active caption
	* \return const QString &
	*/
	const QString & xmlActiveCaption() { return m_szXmlActiveCaption; };

	/**
	* \brief Returns XML-caption string for inactive caption
	* \return const QString &
	*/
	const QString & xmlInactiveCaption() { return m_szXmlInactiveCaption; };

	/**
	* \brief Sets the window title
	* \param plain Plain text of caption
	* \param xmlActive not used
	* \param xmlInactive not used
	* \return void
	*/
	void setWindowTitle(const QString & szPlain, const QString & szXmlActive, const QString & szXmlInactive);

	/**
	* \brief Sets the window icon
	* \param pix The new icon
	* \return void
	*/
	void setIcon(QPixmap pix);

	/**
	* \brief Returns current window icon
	* \return const QPixmap *
	*/
	const QPixmap * icon();

	/**
	* \brief Enables or disables the close button
	* \param bEnable Activate/Deactivate close button
	* \return void
	*/
	void enableClose(bool bEnable);

	/**
	* \brief Returns if close is enabled
	* \return bool
	*/
	bool closeEnabled();

	/**
	* \brief Returns the current KviMdiManager of the subwindow
	* \return KviMdiManager *
	*/
	KviMdiManager * manager(){ return m_pManager; };

	/**
	* \brief Activates this subwindow
	* \return void
	*/
	void activate(bool bSetFocus);

	/**
	* \brief Enqueue minimize status
	*
	* Since Qt sometimes does not like changing window states while being in
	* another event
	* \return void
	*/
	void queuedMinimize();

	/**
	* \brief Enqueue restore status
	*
	* Since Qt sometimes does not like changing window states while being in
	* another event
	* \return void
	*/
	void queuedRestore();

	/**
	* \brief Enqueue maximize status
	*
	* Since Qt sometimes does not like changing window states while being in
	* another event
	* \return void
	*/
	void queuedMaximize();
protected:
	// TODO: ???
	virtual void setBackgroundRole(QPalette::ColorRole);
	// TODO: ???
	virtual void focusInEvent(QFocusEvent *);

	/**
	* \brief Updates the widget background when moving
	* \return void
	*/
	virtual void moveEvent(QMoveEvent * e);

	/**
	* \brief Hooks into close event to make user decisions possible
	* \return void
	*/
	virtual void closeEvent(QCloseEvent * e);

	// TODO: ???
	/**
	* \brief Emits the systemPopupRequest signal
	* \param pnt The point where the popup has to be shown
	* \return void
	*/
	void emitSystemPopupRequest(const QPoint & pnt) { emit systemPopupRequest(pnt); };
public slots:
	/**
	* \brief Maximizes this window
	* \return void
	*/
	void maximize();

	/**
	* \brief Minimizes this window
	* \return void
	*/
	void minimize();

	/**
	* \brief Restore it's latest postition/state
	* \return void
	*/
	void restore();

	// TODO: ???
	void systemPopupSlot();

	/**
	* \brief Triggered when the state of a window changes
	*
	* This event is used to catch minmize event of the KviMdiSubWindow
	* \param oldState The old state of the window
	* \param newState The new state of the window
	* \return void
	*/
	void windowStateChangedEvent( Qt::WindowStates oldState, Qt::WindowStates newState );
private slots:
	/**
	* \brief Updates the system popup
	* \return void
	*/
	void updateSystemPopup();
signals:
	/**
	* \brief Signal is emitted when user clicks on the window icon
	* \param pnt The point where the user clicks
	* \return void
	*/
	void systemPopupRequest(const QPoint & pnt);

	/**
	* \brief This signal is used to minimize the window
	* \return void
	*/
	void minimizeSignal();

	/**
	* \brief This signal is used to maximize the window
	* \return void
	*/
	void maximizeSignal();

	/**
	* \brief This signal is used to restore the window
	* \return void
	*/
	void restoreSignal();

	/**
	* \brief This signal is used to hide the window
	* \return void
	*/
	void hideSignal();
};

#endif //_KVI_MDICHILD_H_
