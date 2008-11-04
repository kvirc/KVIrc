#ifndef _KVI_MDICHILD_H_
#define _KVI_MDICHILD_H_
//=============================================================================
//
//   File : kvi_mdichild.h
//   Creation date : Wed Jun 21 2000 17:35:04 by Szymon Stefanek
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

/**
* \file kvi_mdichild.h
* \brief MDI subwindow stuff
*/

#include "kvi_settings.h"
#include "kvi_string.h"

#include <QFrame>
#include <QLabel>
#include <QLabel>
#include <QToolButton>
#include <QMdiSubWindow>
#include <QWidget>
#include <QWindowStateChangeEvent>
#include <QTimer>

#include "kvi_tal_popupmenu.h"

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
	* \brief Constructor
	* \param par The mdi-manager the mdi-child belongs to.
	* \param name The name of the mdi-child
	*/
	KviMdiChild(KviMdiManager* par,const char * name = 0);
	~KviMdiChild();
public:
	enum MdiChildState { Maximized = 0 , Minimized , Normal };
protected:
	/// Contains the according KviMdiManager object.
	KviMdiManager              * m_pManager;
private:
	/// The widget docked into our MDI window.
	QWidget						* m_pClient;
	/// Saves the geometry of the window for restoring it's position
	QRect						  m_restoredGeometry;
	/// Saves the last state of our window
	MdiChildState				  m_State;
	/// Saves the last state before window is minimized
	MdiChildState				  m_LastState;
	/// Contains the current window Icon
	QPixmap						  m_pIcon;
	/// XML-Version of active Caption.
	QString						  m_szXmlActiveCaption;
	/// XML-Version of inactive Caption.
	QString						  m_szXmlInactiveCaption;
	/// Caption as plain text.
	QString						  m_szPlainCaption;
	/// If window can be closed or not
	bool						  m_bCloseEnabled;
private:
	void updateCaption();
public:
	/// Read the current geometry
	QRect restoredGeometry();

	/// Set new geomentry
	/// \param r The geometry the window will take when it will be restored
	void setRestoredGeometry(const QRect &r);

	/// Set the client widget which is shown in the subwindow
	/// \param w The new client widget which is docked into our KviMdiChild
	void setClient(QWidget * w);

	/// Returns the currently in the window visible widget
	QWidget * client() { return m_pClient; };

	/// Removes the client widget
	void unsetClient();

	/// Get the current window state
	MdiChildState state();

	/// Return plain text caption
	const QString & plainCaption() { return m_szPlainCaption; };

	/// Return xml-caption string for active caption
	const QString & xmlActiveCaption() { return m_szXmlActiveCaption; };

	/// Return xml-caption string for inactive caption
	const QString & xmlInactiveCaption() { return m_szXmlInactiveCaption; };

	/** Set the window title
	*	\param plain Plain text of caption
	*	\param xmlActive not used
	*	\param xmlInactive not used
	*/
	void setWindowTitle(const QString & plain,const QString & xmlActive,const QString & xmlInactive);

	/// Set the window icon
	/// \param pix The new icon
	void setIcon(QPixmap pix);

	/// Return current window icon
	const QPixmap * icon();

	/// Enable or disable the close button
	/// \param bEnable Activate/Deactivate close button
	void enableClose(bool bEnable);

	/// Return if close is enabled
	bool closeEnabled();

	/// Returns the current KviMdiManager of the subwindow
	KviMdiManager * manager(){ return m_pManager; };

	/// Activate this subwindow
	void activate(bool bSetFocus);

	/// Since Qt sometimes does not like changing window states while being in another event
	void queuedMinimize();
	/// Since Qt sometimes does not like changing window states while being in another event
	void queuedRestore();
	/// Since Qt sometimes does not like changing window states while being in another event
	void queuedMaximize();

public slots:
	/// Maximize this window
	void maximize();
	/// Minimize this window
	void minimize();
	/// Restore it's latest postition/state
	void restore();

	// TODO: ???
	void systemPopupSlot();

	/// Event is used to catch minmize event of the KviMdiSubWindow
	void windowStateChangedEvent( Qt::WindowStates oldState, Qt::WindowStates newState );
private slots:
	void updateSystemPopup();
signals:
	/// Signal is emitted when user clicks on the window icon
	void systemPopupRequest(const QPoint & pnt);
	/// Signal is used to minimize the window
	void minimizeSignal();
	void maximizeSignal();
	void restoreSignal();
	void hideSignal();
protected:
	// TODO: ???
	virtual void setBackgroundRole(QPalette::ColorRole);
	// TODO: ???
	virtual void focusInEvent(QFocusEvent *);

	/// Updates the widget background when moving
	virtual void moveEvent(QMoveEvent * e);

	/// Hook into close event to make user decisions possible
	virtual void closeEvent(QCloseEvent * e);

	// TODO: ???
	void emitSystemPopupRequest(const QPoint & pnt) { emit systemPopupRequest(pnt); };
};


#endif //_KVI_MDICHILD_H_
