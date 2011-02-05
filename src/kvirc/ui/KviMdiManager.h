#ifndef _KVI_MDIMANAGER_H_
#define _KVI_MDIMANAGER_H_
//=============================================================================
//
//   File : kvi+_mdimanager.h
//   Creation date : Wed Jun 21 2000 17:28:04 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
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
* \file KviMdiManager.h
* \brief The MDI-manager
* \author Szymon Stefanek
*/

#include "kvi_settings.h"
#include "KviMdiChild.h"

#include <QMdiArea>
#include <QScrollBar>

class KviMainWindow;
class KviTalPopupMenu;

/**
* \class KviMdiManager
* \brief Handles all MDI windows
*
* This MDI-manager is based on QMdiArea of Qt 4. It handles all docked windows and does their tiling.
* Basicly all windows are managed by the KviMainWindow, which also gives the order to the MDI-manager to dock or undock a window.
* What does that mean?
* All windows in KVIrc are of the type QWidget which can be standalone windows. These are packed into a QMdiSubWindow and added to the MDI-manager.
* So the KviMdiManager gets a KviMdiChild which will be set as main widget of our QMdiSubWindow. When it'll be undocked QMdiSubWindow will be removed and our KviWindow is "free" again.
*/
class KVIRC_API KviMdiManager : public QMdiArea
{
	friend class KviMdiChild;
	Q_OBJECT
public:
	/**
	* \brief Constructs the Mdi manager object
	* \param pParent The parent widget
	* \param pFrm The frame container
	* \param pcName The name of the manager
	* \return KviMdiManager
	*/
	KviMdiManager(QWidget * pParent, KviMainWindow * pFrm, const char * pcName);

	/**
	* \brief Destroys the Mdi manager object
	*/
	~KviMdiManager();

	/**
	* \brief Filters out some events: (de)activation events, window switching
	* \return bool
	*/
	bool eventFilter(QObject *obj, QEvent *event);
private:
	bool m_bInSDIMode;
protected:
	/// Holds the specialized window popup
	KviTalPopupMenu * m_pWindowPopup;
	/// Holds the tiling popup
	KviTalPopupMenu * m_pTileMethodPopup;
	/// The frame where this KviMdiManager belongs to
	KviMainWindow        * m_pFrm;
public:
	/**
	* \brief Get the currently active window
	* \return KviMdiChild *
	*/
	KviMdiChild * topChild();
 
	/**
	* \brief Add an KviMdiChild to the area
	* \param lpC The KviMdiChild
	* \param bCascade Cascade window or not.
	* \param setGeom Sets the windows geometry before shown
	* \return void
	*/
	void manageChild(KviMdiChild * lpC, bool bCascade = true, QRect * setGeom = 0);

	/**
	* \brief Show the KviMdiChild and bring it to the front
	* \param lpC The KviMdiChild which will be shown.
	* \return void
	*/
	void showAndActivate(KviMdiChild * lpC);

	/**
	* \brief Returns the window popup
	* \return KviTalPopupMenu *
	*/
	inline KviTalPopupMenu * windowPopup() { return m_pWindowPopup; };

	/**
	* \brief Move the focus the the previous active window
	* \return void
	*/
	void focusPreviousTopChild(KviMdiChild * pExcludeThis = NULL);

	/**
	* \brief Remove and delete the subwindow
	* \param lpC The KviMdiChild which will be destroyed.
 	* \return void
 	*/
	void destroyChild(KviMdiChild * lpC);

	/**
	* \brief Hides the specified child and focuses some other child
	* \param lpC The KviMdiChild which will be hidden.
 	* \return void
 	*/
	void hideChild(KviMdiChild * lpC);

	/**
	* \brief Get all visible subwindows
	* \return int
	*/
	int getVisibleChildCount();

	/**
	* \brief Check if we are in SDI mode
	* \return bool
	*/
	inline bool isInSDIMode() { return m_bInSDIMode; };

	/**
	* \brief Returns the current scrollbar offsets
	* \return QPoint
	*/
	inline QPoint scrollBarsOffset() { return QPoint(horizontalScrollBar()->value(), verticalScrollBar()->value()); };
protected:
	/**
	* \brief Sets if we are in SDI mode
	*/
	void setIsInSDIMode(bool bMode);

	/**
	 * \brief Repaints the transparent backgrounds if activated
	 */
	virtual void paintEvent(QPaintEvent * e);

private:
	/**
 	 * \brief Make sure that no KviMdiChild is maximized
	 * \return void
	 *
	 * Goes trough all KviMdiChild objects and checks for maximized mode and restores that windows
	 */
	void ensureNoMaximized();

	/**
	 * \brief The internal member which provides the tiling methods
	 * \param maxWnds The maximum number of windows
	 * \param bHorizontal If the windows will be tiled horizontal or vertical
	 * \return void
	 */
	void tileAllInternal(int maxWnds,bool bHorizontal);

public slots:
	void cascadeWindows();
	void cascadeMaximized();
	void expandVertical();
	void expandHorizontal();
	void minimizeAll();
	void restoreAll();
	void tile();
	void toggleAutoTile();
	void tileAnodine();
protected slots:
	void menuActivated(int id);
	void tileMethodMenuActivated(int id);
	void fillWindowPopup();
	void slotSubWindowActivated(QMdiSubWindow * pMdiChild);
};

#endif //_KVI_MDIMANAGER_H_
