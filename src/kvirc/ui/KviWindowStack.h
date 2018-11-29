#ifndef _KviWindowStack_h_
#define _KviWindowStack_h_
//=============================================================================
//
//   File : KviWindowStack.h
//   Creation date : Wed Jun 21 2000 17:28:04 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2008 TheXception (kvirc at thexception dot net)
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
* \file KviWindowStack.h
* \brief The window manager
* \author Szymon Stefanek
*/

#include "kvi_settings.h"

#include <QStackedWidget>

class KviMainWindow;
class KviWindow;
class QMenu;

/**
* \class KviWindowStack
* \brief Handles all docked KviWindow windows
*/
class KVIRC_API KviWindowStack : public QStackedWidget
{
	Q_OBJECT
public:
	/**
	* \brief Constructs the Mdi manager object
	* \param pParent The parent widget
	* \param pcName The name of the manager
	* \return KviWindowStack
	*/
	KviWindowStack(QWidget * pParent, const char * pcName);

	/**
	* \brief Destroys the Mdi manager object
	*/
	~KviWindowStack();

	/**
	* \brief Filters out some events: (de)activation events, window switching
	* \return bool
	*/
	//bool eventFilter(QObject *obj, QEvent *event) override;
protected:
	/// Holds the specialized window popup
	QMenu * m_pWindowPopup;

public:
	/**
	* \brief Add an KviWindow to the area
	* \param pWnd The KviWindow
	* \return void
	*/
	void addWindow(KviWindow * pWnd);

	/**
	* \brief Show the KviWindow and bring it to the front
	* \param pWnd The KviWindow which will be shown.
	* \return void
	*/
	void showAndActivate(KviWindow * pWnd);

	/**
	* \brief Returns the window popup
	* \return QMenu *
	*/
	QMenu * windowPopup() const { return m_pWindowPopup; }

	/**
	* \brief Remove and delete the subwindow
	* \param pWnd The KviWindow which will be destroyed.
 	* \return void
 	*/
	void destroyWindow(KviWindow * pWnd);

protected slots:
	void currentWindowChanged(int iIdx);
	void menuActivated(QAction * pAction);
	void fillWindowPopup();
};

#endif //_KviWindowStack_h_
