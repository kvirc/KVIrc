#ifndef _KVI_WINDOWLIST_H_
#define _KVI_WINDOWLIST_H_
//=============================================================================
//
//   File : KviWindowListBase.h
//   Creation date : Thu Jan 7 1999 03:56:50 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"
#include "KviPointerList.h"
#include "KviTalToolTip.h"

#include <QPushButton>
#include <QFrame>
#include <QDockWidget>

class QPixmap;
class KviWindow;
class KviMainWindow;
class KviDynamicToolTip;
class KviConsoleWindow;

//
// KviWindowListItem
//
// The base class for the WindowList items
// this is the only interface to an item visible to external classes
//

class KVIRC_API KviWindowListItem
{
public:
	KviWindowListItem(KviWindow * wnd);
	virtual ~KviWindowListItem();
protected:
	KviWindow * m_pWindow;
	int         m_iHighlightLevel;
	int         m_iProgress;
public:
	KviWindow * kviWindow(){ return m_pWindow; };
	virtual void captionChanged(){};
	virtual void highlight(int){};
	virtual void setProgress(int){};
	virtual bool active(){ return false; };
	virtual void unhighlight(){};
	int progress(){ return m_iProgress; };
	int highlightLevel(){ return m_iHighlightLevel; };
};

//
// KviWindowListBase
//
// The base class for the WindowList implementations
// This is the basic interface that all the external classes should see
//

class KVIRC_API KviWindowListBase : public QDockWidget
{
	Q_OBJECT
public:
	KviWindowListBase();
	virtual ~KviWindowListBase();
protected:
	KviMainWindow * m_pFrm;
	QTimer   * m_pActivityMeterTimer;
	QWidget  * m_pTitleWidget;
	Qt::DockWidgetArea currentArea;
public:
	virtual KviWindowListItem * addItem(KviWindow *){ return 0; };
	virtual bool removeItem(KviWindowListItem *){ return false; };
	virtual void setActiveItem(KviWindowListItem *){};
	virtual KviWindowListItem * firstItem(){ return 0; };
	virtual KviWindowListItem * lastItem(void) { return 0; }
	virtual KviWindowListItem * nextItem(){ return 0; };
	virtual KviWindowListItem * prevItem(void) { return 0; }
	virtual KviWindowListItem * item(int number);
	virtual bool setIterationPointer(KviWindowListItem *){ return false; };
	virtual void switchWindow(bool bNext,bool bInContextOnly,bool bHighlightedOnly=false);
	virtual void updatePseudoTransparency(){};
	virtual void applyOptions();
	virtual void wheelEvent(QWheelEvent *e);
	static void getTextForConsole(QString &szText,KviConsoleWindow * pConsole);
	Qt::DockWidgetArea currentDockArea() { return currentArea; };
protected slots:
	virtual void updateActivityMeter();
	void updateDockLocation(Qt::DockWidgetArea newArea);
};

//
// Implementation details: the following classes should be
// never used directly (with just the exception of KviMainWindow
// that creates the WindowList)
//

class KviClassicWindowList;

class KVIRC_API KviWindowListButton : public QPushButton, KviWindowListItem
{
	friend class KviClassicWindowList;
	Q_OBJECT
public:
	KviWindowListButton(QWidget * par,KviWindow * wnd,const char * name);
	~KviWindowListButton();
protected:
	bool                m_bActive;
	KviDynamicToolTip * m_pTip;
protected:
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void contextMenuEvent(QContextMenuEvent *e);
	virtual void drawButtonLabel(QPainter *p);
	virtual void paintEvent(QPaintEvent * e);
public:
	virtual bool active(){ return m_bActive; };
	virtual void highlight(int iLevel = 1);
	virtual void unhighlight();
	virtual void setProgress(int progress);
	virtual void captionChanged();
protected:
	void setActive(bool bActive);
protected slots:
	void tipRequest(KviDynamicToolTip *tip,const QPoint &pnt);
};


class KVIRC_API KviClassicWindowList : public KviWindowListBase
{
	Q_OBJECT
public:
	KviClassicWindowList();
	~KviClassicWindowList();
protected:
	KviPointerList<KviWindowListButton> * m_pButtonList;
	int                       m_iButtonHeight;
	QWidget                 * m_pBase;
protected:
	void calcButtonHeight();
	void insertButton(KviWindowListButton * b);
public:
//	virtual void fontChange(const QFont & old);
	virtual void resizeEvent(QResizeEvent *e);
public:
	virtual KviWindowListItem * addItem(KviWindow *);
	virtual bool removeItem(KviWindowListItem *);
	virtual void setActiveItem(KviWindowListItem *);
	virtual KviWindowListItem * firstItem();
	virtual KviWindowListItem * lastItem(void);
	virtual KviWindowListItem * nextItem();
	virtual KviWindowListItem * prevItem(void);
	virtual bool setIterationPointer(KviWindowListItem * it);
	virtual void updateActivityMeter();
	virtual void applyOptions();
protected slots:
	void orientationChangedSlot(Qt::Orientation o);
	void doLayout();
};

class KVIRC_API KviWindowListTitleWidget : public QWidget
{
	Q_OBJECT
public:
	KviWindowListTitleWidget(KviWindowListBase * parent) { m_pParent = parent; };
	~KviWindowListTitleWidget() {};
private:
	KviWindowListBase* m_pParent;
public:
	QSize sizeHint() const;
	void paintEvent(QPaintEvent *);
};

#endif //_KVI_WINDOWLIST_H_
