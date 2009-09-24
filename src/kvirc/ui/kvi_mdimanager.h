#ifndef _KVI_MDIMANAGER_H_
#define _KVI_MDIMANAGER_H_
//=============================================================================
//
//   File : kvi+_mdimanager.h
//   Creation date : Wed Jun 21 2000 17:28:04 by Szymon Stefanek
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
* \file kvi_mdimanager.h
* \brief The MDI-manager
* \author Szymon Stefanek
*/

#include "kvi_settings.h"
#include "kvi_pointerlist.h"
//#include "kvi_tal_scrollview.h"

#include <QWidget>
#include <QFrame>
#include <QPixmap>
#include <QMdiArea>

#ifdef COMPILE_ON_WINDOWS
	#include "kvi_mdichild.h"
#else
	class KviMdiChild;
#endif

class KviFrame;
class KviTalPopupMenu;
class KviTalHBox;
class KviSdiButtonBox;

/**
* \def KVI_MDICHILD_BORDER Defines the border of the child
* \def KVI_MDICHILD_SPACING Defines the spacing of the child
* \def KVI_MDICHILD_MIN_WIDTH Defines the minimum width of the child
* \def KVI_MDICHILD_MIN_HEIGHT Defines the minimum height of the child
* \def KVI_MDICHILD_HIDDEN_EDGE Defines the hidden edge of the child
*/
#define KVI_MDICHILD_BORDER 4
#define KVI_MDICHILD_SPACING 2
#define KVI_MDICHILD_MIN_WIDTH 100
#define KVI_MDICHILD_MIN_HEIGHT 40
#define KVI_MDICHILD_HIDDEN_EDGE 3

/**
* \class KviMdiManager
* \brief Handles all MDI windows
*
* This MDI-manager is based on QMdiArea of Qt 4. It handles all docked windows and does their tiling.
* Basicly all windows are managed by the KviFrame, which also gives the order to the MDI-manager to dock or undock a window.
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
	KviMdiManager(QWidget * pParent, KviFrame * pFrm, const char * pcName);

	/**
	* \brief Destroys the Mdi manager object
	*/
	~KviMdiManager();
private:
	bool m_bInSDIMode;
protected:
	/// Holds the specialized window popup
	KviTalPopupMenu * m_pWindowPopup;
	/// Holds the tiling popup
	KviTalPopupMenu * m_pTileMethodPopup;
	/// The frame where this KviMdiManager belongs to
	KviFrame        * m_pFrm;
public:
	/**
	* \brief Get the currently active window
	* \return KviMdiChild *
	*/
	KviMdiChild * topChild();

	/**
	* \brief
	* \return KviMdiChild *
	*/
	KviMdiChild * highestChildExcluding(KviMdiChild * pChild);

	/**
	* \brief Add an KviMdiChild to the area
	* \param lpC The KviMdiChild
	* \param bCascade Cascade window or not.
	* \param setGeom Sets the windows geometry before shown
	* \return void
	*/
	void manageChild(KviMdiChild * lpC, bool bCascade = true, QRect * setGeom = 0);

	/**
	* \brief Bring the KviMdiChild to the foreground
	* \param lpC The KviMdiChild
	* \return void
	*/
	void setTopChild(KviMdiChild * lpC);

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
	KviTalPopupMenu * windowPopup() { return m_pWindowPopup; };

	/**
	* \brief Move the focus the the top window
	* \return void
	*/
	void focusTopChild();

	/**
	* \brief Remove and delete the subwindow
	* \param lpC The KviMdiChild which will be destroyed.
	* \param bFocusTopChild Defines if the next window which will appear afterwards will get the focus or not.
	* \return void
	*/
	void destroyChild(KviMdiChild * lpC, bool bFocusTopChild = true);

	/**
	* \brief Get all visible subwindows
	* \return int
	*/
	int getVisibleChildCount();

	/**
	* \brief Check if we are in SDI mode
	* \return bool
	*/
	bool isInSDIMode();
protected:
	/**
	 * \brief Reorders widgets on KviMdiChild
	 * \return void
	 * \deprecated Was used by old MDI system
	 */
	void updateContentsSize();

	/**
	 * \brief This has to be called if a KviMdiChild gets maximized
	 * \param lpC The target KviMdiChild
	 * \return void
	 *
	 * This updates if we are in SDI or in MDI mode.
	 */
	void childMaximized(KviMdiChild * lpC);

	/**
	 * \brief This has to be called if a KviMdiChild gets minimized
	 * \param lpC The target KviMdiChild
	 * \param bWasMaximized Tell the KviMdiManager if the KviMdiChild was maximized before minimizing
	 * \return void
	 *
	 * Focus the next availiable window
	 */
	void childMinimized(KviMdiChild * lpC, bool bWasMaximized);

	/**
	 * \brief This has to be called if a KviMdiChild gets restored
	 * \param lpC The target KviMdiChild
	 * \param bWasMaximized Tell the KviMdiManager if the KviMdiChild was maximized before restoring
	 * \return void
	 *
	 * Update SDI/MDI mode. In this case we leave SDI mode.
	 */
	void childRestored(KviMdiChild * lpC, bool bWasMaximized);

	/**
	 *  \brief This has to be called if a KviMdiChild has been moved
	 *  \param lpC The target KviMdiChild
	 *  \return void
	 *  \deprecated Was used by old MDI system
	 */
	void childMoved(KviMdiChild * lpC);

	/**
	 * \brief Open the window popup menu
	 */
	virtual void mousePressEvent(QMouseEvent * e);

	/**
	 * \brief Repaints the transparent backgrounds if activated
	 */
	virtual void paintEvent(QPaintEvent * e);

	/**
	 * \brief Passes this to the toplevel window
	 */
	virtual bool focusNextPrevChild(bool pNext);
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
	/**
	 * \brief
	 * \return QPoint
	 */
	QPoint getCascadePoint(int indexOfWindow);
public slots:
	void relayoutMenuButtons();
	void cascadeWindows();
	void cascadeMaximized();
	void expandVertical();
	void expandHorizontal();
	void minimizeAll();
	void restoreAll(); //<-- this does nothing (not working?)
	void tile();
	void toggleAutoTile();
	void tileAnodine();
	void reloadImages();
protected slots:
	void minimizeActiveChild();
	void restoreActiveChild();
	void closeActiveChild();
	void activeChildSystemPopup();
	void menuActivated(int id);
	void tileMethodMenuActivated(int id);
	void fillWindowPopup();
};

#endif //_KVI_MDIMANAGER_H_
