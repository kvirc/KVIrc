#ifndef _KVI_MDIMANAGER_H_
#define _KVI_MDIMANAGER_H_
//=============================================================================
//
//   File : kvi_mdimanager.h
//   Creation date : Wed Jun 21 2000 17:28:04 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2004 Szymon Stefanek (pragma at kvirc dot net)
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


#include "kvi_settings.h"
#include "kvi_list.h"

#include <qwidget.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qtoolbutton.h>

#include "kvi_tal_scrollview.h"

#define KVI_MDICHILD_BORDER 4
#define KVI_MDICHILD_SPACING 2
#define KVI_MDICHILD_MIN_WIDTH 100
#define KVI_MDICHILD_MIN_HEIGHT 40
#define KVI_MDICHILD_HIDDEN_EDGE 3

#ifdef COMPILE_ON_WINDOWS
	#include "kvi_mdichild.h"
#else
	class KviMdiChild;
#endif
//class KviMdiCaptionButton;
class KviFrame;

class KviTalPopupMenu;

class KviSdiButtonBox;
class KviMenuBarToolButton;

class KVIRC_API KviMdiManager : public KviTalScrollView
{
	friend class KviMdiChild;
	friend class KviMdiCaption;
	Q_OBJECT
public:
	KviMdiManager(QWidget * parent,KviFrame * pFrm,const char * name);
	~KviMdiManager();
public:
	KviMdiChild * topChild(){ return m_pZ->last(); };
	KviMdiChild * highestChildExcluding(KviMdiChild * pChild);
	void manageChild(KviMdiChild * lpC,bool bCascade = true,QRect * setGeom = 0);
	void setTopChild(KviMdiChild *lpC,bool bSetFocus);
	void showAndActivate(KviMdiChild * lpC);
	KviTalPopupMenu * windowPopup(){ return m_pWindowPopup; };
	void focusTopChild();
	void destroyChild(KviMdiChild *lpC,bool bFocusTopChild = true);
	int getVisibleChildCount();
	bool isInSDIMode();
protected:
	KviPtrList<KviMdiChild>  * m_pZ; // topmost child is the last in the list

	KviMenuBarToolButton     * m_pSdiRestoreButton;
	KviMenuBarToolButton     * m_pSdiMinimizeButton;
	KviMenuBarToolButton     * m_pSdiCloseButton;
	KviMenuBarToolButton     * m_pSdiIconButton;

	int                        m_iSdiIconItemId;
	int                        m_iSdiRestoreItemId;
	int                        m_iSdiMinimizeItemId;
	int                        m_iSdiCloseItemId;

	KviTalPopupMenu               * m_pWindowPopup;
	KviTalPopupMenu               * m_pTileMethodPopup;
	KviFrame                 * m_pFrm;
protected:
	void updateContentsSize();
	//void childMaximized(KviMdiChild *lpC);
	void childMinimized(KviMdiChild *lpC,bool bWasMaximized);
	void childRestored(KviMdiChild *lpC,bool bWasMaximized);
	void childMoved(KviMdiChild * lpC);
	void maximizeChild(KviMdiChild * lpC);
	virtual void focusInEvent(QFocusEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void resizeEvent(QResizeEvent *e);
	virtual void drawContents(QPainter * p,int x,int y,int w,int h);
	virtual bool focusNextPrevChild(bool pNext);
public slots:
	void cascadeWindows();
	void cascadeMaximized();
	void expandVertical();
	void expandHorizontal();
	void minimizeAll();
//    void restoreAll(); <-- this does nothing
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
	void sdiMinimizeButtonDestroyed();
	void sdiRestoreButtonDestroyed();
	void sdiCloseButtonDestroyed();
	void sdiIconButtonDestroyed();
private:
	void ensureNoMaximized();
	void tileAllInternal(int maxWnds,bool bHorizontal);
	QPoint getCascadePoint(int indexOfWindow);
	void enterSDIMode(KviMdiChild *lpC);
	void leaveSDIMode();
	void updateSDIMode();
signals:
	void enteredSdiMode();
	void leftSdiMode();
};

#endif //_KVI_MDIMANAGER_H_
