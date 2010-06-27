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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

/**
* \file kvi_mdichild.h
* \brief MDI subwindow stuff
* \author Szymon Stefanek
*/

#include "kvi_settings.h"
#include "kvi_string.h"

#include <QMdiSubWindow>
#include <QWindowStateChangeEvent>

class KviMdiManager;

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
	* \param pPar The mdi-manager the mdi-child belongs to.
	* \param pcName The name of the mdi-child
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
	enum MdiChildState
	{
		Maximized = 0, /**< The child window is maximized */
		Minimized,     /**< The child window is minimized */
		Normal         /**< The child window is normal */
	};
protected:
	/// Contains the according KviMdiManager object.
	KviMdiManager * m_pManager;
private:
	/// The widget docked into our MDI window.
	QWidget * m_pClient;
	/// Contains the current window Icon
	QPixmap m_pIcon;
	/// Caption as plain text.
	QString m_szPlainCaption;
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
	* \warning  The widget has to inherit of KviWindow!
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
	*
	* For details see the MdiChildState declaration
	*/
	MdiChildState state();

	/**
	* \brief Returns plain text caption
	* \return const QString &
	*/
	const QString & plainCaption() { return m_szPlainCaption; };

	/**
	* \brief Sets the window title
	* \param szPlain Plain text of caption
	* \return void
	*/
	void setWindowTitle(const QString & szPlain);

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
	* \brief Returns the current KviMdiManager of the subwindow
	* \return KviMdiManager *
	*/
	inline KviMdiManager * manager(){ return m_pManager; };

	/**
	* \brief Activates this subwindow
	* \return void
	*/
	void activate();
protected:
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

	/**
	* \brief This is an ugly woraround for qt 4.6.1 that otherway will display the first console window as transparent
	* \return void
	*/
	void transparencyWorkaround();
	
};

#endif //_KVI_MDICHILD_H_
