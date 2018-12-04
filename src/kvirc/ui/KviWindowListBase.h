#ifndef _KVI_WINDOWLIST_H_
#define _KVI_WINDOWLIST_H_
//=============================================================================
//
//   File : KviWindowListBase.h
//   Creation date : Thu Jan 7 1999 03:56:50 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"
#include "KviPointerList.h"
#include "KviTalToolTip.h"

#include <QPushButton>
#include <QToolButton>
#include <QBoxLayout>
#include <QFrame>
#include <QDockWidget>

class QPixmap;
class KviWindow;
class KviMainWindow;
class KviDynamicToolTip;
class KviConsoleWindow;

//
// KviWindowListItem
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
	int m_iHighlightLevel;
	int m_iProgress;

public:
	KviWindow * kviWindow() { return m_pWindow; };
	virtual void captionChanged(){};
	virtual void highlight(int){};
	virtual void setProgress(int){};
	virtual bool active() { return false; };
	virtual void unhighlight(){};
	int progress() { return m_iProgress; };
	int highlightLevel() { return m_iHighlightLevel; };
};

//
// KviWindowListBase
// The base class for the WindowList implementations
// This is the basic interface that all the external classes should see
//

class KVIRC_API KviWindowListBase : public QDockWidget
{
	Q_OBJECT
public:
	KviWindowListBase();
	~KviWindowListBase();

protected:
	KviMainWindow * m_pFrm;
	QTimer * m_pActivityMeterTimer;
	QWidget * m_pTitleWidget;
	Qt::DockWidgetArea currentArea;

public:
	virtual KviWindowListItem * addItem(KviWindow *) { return nullptr; }
	virtual bool removeItem(KviWindowListItem *) { return false; };
	virtual void setActiveItem(KviWindowListItem *){};
	virtual KviWindowListItem * firstItem() { return nullptr; }
	virtual KviWindowListItem * lastItem(void) { return nullptr; }
	virtual KviWindowListItem * nextItem() { return nullptr; }
	virtual KviWindowListItem * prevItem(void) { return nullptr; }
	virtual KviWindowListItem * item(int number);
	virtual bool setIterationPointer(KviWindowListItem *) { return false; };
	virtual void switchWindow(bool bNext, bool bInContextOnly, bool bHighlightedOnly = false);
	virtual void updatePseudoTransparency(){};
	virtual void applyOptions();
	void wheelEvent(QWheelEvent * e) override;
	static void getTextForConsole(QString & szText, KviConsoleWindow * pConsole);
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

//
// KviWindowListButton
// Button to show/hide the window and containing the button to close it
//

class KVIRC_API KviWindowListButton : public QPushButton, KviWindowListItem
{
	friend class KviClassicWindowList;
	friend class KviClassicWindowListToolButton;
	Q_OBJECT
public:
	KviWindowListButton(QWidget * par, KviWindow * wnd, const char * name);
	~KviWindowListButton();

protected:
	bool m_bActive;
	QBoxLayout * m_pLayout;
	QToolButton * m_pTool;
	KviDynamicToolTip * m_pTip;

protected:
	void mousePressEvent(QMouseEvent * e) override;
	void contextMenuEvent(QContextMenuEvent * e) override;
	virtual void drawButtonLabel(QPainter * p);
	void paintEvent(QPaintEvent * e) override;

public:
	bool active() override { return m_bActive; }
	void highlight(int iLevel = 1) override;
	void unhighlight() override;
	void setProgress(int progress) override;
	void captionChanged() override;

protected:
	void setActive(bool bActive);
protected slots:
	void tipRequest(KviDynamicToolTip * tip, const QPoint & pnt);
};

//
// KviClassicWindowListToolButton
// Button to close the window
//

class KVIRC_API KviClassicWindowListToolButton : public QToolButton
{
	Q_OBJECT
protected:
	KviWindowListButton * m_pPar;

public:
	KviClassicWindowListToolButton(KviWindowListButton * par);
	~KviClassicWindowListToolButton() {};

protected:
	void mousePressEvent(QMouseEvent *e) override;

public:
	QSize sizeHint() const override;
};

class KVIRC_API KviClassicWindowList : public KviWindowListBase
{
	Q_OBJECT
public:
	KviClassicWindowList();
	~KviClassicWindowList();

protected:
	KviPointerList<KviWindowListButton> * m_pButtonList;
	int m_iButtonHeight;
	QWidget * m_pBase;

protected:
	void calcButtonHeight();
	void insertButton(KviWindowListButton * b);

public:
	void resizeEvent(QResizeEvent * e) override;

public:
	KviWindowListItem * addItem(KviWindow *) override;
	bool removeItem(KviWindowListItem *) override;
	void setActiveItem(KviWindowListItem *) override;
	KviWindowListItem * firstItem() override;
	KviWindowListItem * lastItem(void) override;
	KviWindowListItem * nextItem() override;
	KviWindowListItem * prevItem(void) override;
	bool setIterationPointer(KviWindowListItem * it) override;
	void updateActivityMeter() override;
	void applyOptions() override;
protected slots:
	void orientationChangedSlot(Qt::Orientation o);
	void doLayout();
};

class KVIRC_API KviWindowListTitleWidget : public QWidget
{
	Q_OBJECT
public:
	KviWindowListTitleWidget(KviWindowListBase * parent) { m_pParent = parent; };
	~KviWindowListTitleWidget(){};

private:
	KviWindowListBase * m_pParent;

public:
	QSize sizeHint() const override;
	void paintEvent(QPaintEvent *) override;
};

#endif //_KVI_WINDOWLIST_H_
