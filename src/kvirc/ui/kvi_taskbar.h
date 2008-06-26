#ifndef _KVI_TASKBAR_H_
#define _KVI_TASKBAR_H_
//=============================================================================
//
//   File : kvi_taskbar.h
//   Creation date : Thu Jan 7 1999 03:56:50 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2004 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_pointerlist.h"
#include "kvi_tal_tooltip.h"
#include "kvi_tal_treewidget.h"
#include "kvi_tal_itemdelegates.h"

#include <QPushButton>
#include <QFrame>
#include <QDockWidget>
#include <QAbstractItemView>

class QPixmap;
class KviWindow;
class KviFrame;
class KviDynamicToolTip;
class KviConsole;

//
// KviTaskBarItem
//
// The base class for the taskBar items
// this is the only interface to an item visible to external classes
//

class KVIRC_API KviTaskBarItem
{
public:
	KviTaskBarItem(KviWindow * wnd);
	virtual ~KviTaskBarItem();
protected:
	KviWindow * m_pWindow;
	int         m_iHighlightLevel;
	int         m_iProgress;
public:
	KviWindow * kviWindow(){ return m_pWindow; };
	virtual void captionChanged(){};
	virtual void highlight(int iLevel = 1){};
	virtual void setProgress(int progress){};
	virtual bool active(){ return false; };
	virtual void unhighlight(){};
	int progress(){ return m_iProgress; };
	int highlightLevel(){ return m_iHighlightLevel; };
};

//
// KviTaskBarBase
//
// The base class for the taskbar implementations
// This is the basic interface that all the external classes should see
//

// Please note that Qt3 moc skips the  *_SKIP_BEGIN -> *_SKIP_END blocks
// while the Qt4 moc skips the Q_MOC_RUN ifdef block.. so...

// Qt4 version

// If you get failures (undefined references) in some non-autotools based
// build system that uses Qt4 then you must add the -DCOMPILE_USE_QT4
// commandline parameter to moc (at leat) when parsing this file.

class KVIRC_API KviTaskBarBase : public QDockWidget
{
	Q_OBJECT
public:
	KviTaskBarBase();
	virtual ~KviTaskBarBase();
protected:
	KviFrame * m_pFrm;
	QTimer   * m_pActivityMeterTimer;
public:
	virtual KviTaskBarItem * addItem(KviWindow *){ return 0; };
	virtual bool removeItem(KviTaskBarItem *){ return false; };
	virtual void setActiveItem(KviTaskBarItem *){};
	virtual KviTaskBarItem * firstItem(){ return 0; };
	virtual KviTaskBarItem * lastItem(void) { return 0; }
	virtual KviTaskBarItem * nextItem(){ return 0; };
	virtual KviTaskBarItem * prevItem(void) { return 0; }
	virtual KviTaskBarItem * item(int number);
	virtual bool setIterationPointer(KviTaskBarItem * it){ return false; };
	virtual void switchWindow(bool bNext,bool bInContextOnly);
	virtual void updatePseudoTransparency(){};
	virtual void applyOptions(){};
	static void getTextForConsole(QString &szText,KviConsole * pConsole);
protected slots:
	virtual void updateActivityMeter();
};

//
// Implementation details: the following classes should be
// never used directly (with just the exception of KviFrame
// that creates the taskbar)
//

class KviClassicTaskBar;

class KVIRC_API KviTaskBarButton : public QPushButton , KviTaskBarItem
{
	friend class KviClassicTaskBar;
	Q_OBJECT
public:
	KviTaskBarButton(QWidget * par,KviWindow * wnd,const char * name);
	~KviTaskBarButton();
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


class KVIRC_API KviClassicTaskBar : public KviTaskBarBase
{
	Q_OBJECT
public:
	KviClassicTaskBar();
	~KviClassicTaskBar();
protected:
	KviPointerList<KviTaskBarButton> * m_pButtonList;
	int                       m_iButtonHeight;
	QWidget                 * m_pBase;
protected:
	void calcButtonHeight();
	void doLayout(); // called by KviFrame to adjust a bug on WIndows
	void insertButton(KviTaskBarButton * b);
public:
//	virtual void fontChange(const QFont & old);
	virtual void resizeEvent(QResizeEvent *e);
public:
	virtual KviTaskBarItem * addItem(KviWindow *);
	virtual bool removeItem(KviTaskBarItem *);
	virtual void setActiveItem(KviTaskBarItem *);
	virtual KviTaskBarItem * firstItem();
	virtual KviTaskBarItem * lastItem(void);
	virtual KviTaskBarItem * nextItem();
	virtual KviTaskBarItem * prevItem(void);
	virtual bool setIterationPointer(KviTaskBarItem * it);
	virtual void updateActivityMeter();
	virtual void applyOptions();
protected slots:
	void orientationChangedSlot(Qt::Orientation o);
};

class KviTreeTaskBar;
class KviTreeTaskBarItemInternal;

class KVIRC_API KviTreeTaskBarItem : public KviTalTreeWidgetItem , public KviTaskBarItem
{
	friend class KviTreeTaskBar;
	friend class KviTreeTaskBarTreeWidget;
	friend class KviTreeTaskBarItemInternal;
public:
	KviTreeTaskBarItem(KviTalTreeWidget * par,KviWindow * wnd);
	KviTreeTaskBarItem(KviTreeTaskBarItem * par,KviWindow * wnd);
	~KviTreeTaskBarItem();
protected:
	int m_iStepNumber;
	bool m_bIncreasing;
	QTimer* m_pAnimTimer;
	KviTreeTaskBarItemInternal *m_pInternal;
public:
	virtual QString key(int column,bool) const;
	virtual void captionChanged();
	virtual void highlight(int iLevel = 1);
	virtual void unhighlight();
	virtual void setProgress(int progress);
	virtual void applyOptions();
protected:
	void setActive(bool bActive);
	void mouseEnter();
	void mouseLeave();
	void timerShot();
	int calculateColor(int col1,int col2);
	QString currentCaption() const;
};

class KviTreeTaskBarItemInternal : public QObject
{
	Q_OBJECT
public:
	KviTreeTaskBarItemInternal(KviTreeTaskBarItem* pItem):m_pItem(pItem) {};
	~KviTreeTaskBarItemInternal() {};
protected:
	KviTreeTaskBarItem* m_pItem;
public slots:
	void timerShot() { m_pItem->timerShot();};
};

class KVIRC_API KviTreeTaskBarTreeWidget : public KviTalTreeWidget
{
	friend class KviTreeTaskBarItem;
	friend class KviTreeTaskBarItemDelegate;
	Q_OBJECT
	KviTreeTaskBarItem* m_pPrevItem;
public:
	KviTreeTaskBarTreeWidget(QWidget * par);
	~KviTreeTaskBarTreeWidget();
protected:
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseDoubleClickEvent(QMouseEvent * e);
	virtual void paintEvent(QPaintEvent * event);
	virtual void resizeEvent(QResizeEvent *e);
	virtual void mouseMoveEvent (QMouseEvent * e);
	virtual void leaveEvent(QEvent *);
private:
	KviTaskBarItem * lastItem();
signals:
	void leftMousePress(KviTalTreeWidgetItem * it);
	void rightMousePress(KviTalTreeWidgetItem * it);
public slots:
	void sort();
	void reverseSort();
};


class KVIRC_API KviTreeTaskBar : public KviTaskBarBase
{
	Q_OBJECT
public:
	KviTreeTaskBar();
	~KviTreeTaskBar();
private:
	KviTreeTaskBarTreeWidget * m_pTreeWidget;
	KviTreeTaskBarItem * m_pCurrentItem;
	KviDynamicToolTip  * m_pToolTip;
	QAbstractItemDelegate* m_pItemDelegate;
public:
	virtual KviTaskBarItem * addItem(KviWindow *);
	virtual bool removeItem(KviTaskBarItem *);
	virtual void setActiveItem(KviTaskBarItem *);
	virtual KviTaskBarItem * firstItem();
	virtual KviTaskBarItem * nextItem(void);
	virtual KviTaskBarItem * lastItem();
	virtual KviTaskBarItem * prevItem(void);
	virtual bool setIterationPointer(KviTaskBarItem * it);
	virtual void updatePseudoTransparency();
	virtual bool eventFilter(QObject * o,QEvent *e);
	virtual void updateActivityMeter();
	virtual void applyOptions();
protected:
	virtual void moveEvent(QMoveEvent *);
protected slots:
	void tipRequest(KviDynamicToolTip *tip,const QPoint &pnt);
};

#define KVI_TTBID_STEPNUM Qt::UserRole
#define KVI_TTBID_REDDIFF Qt::UserRole + 1
#define KVI_TTBID_GREENDIFF Qt::UserRole + 2
#define KVI_TTBID_BLUEDIFF Qt::UserRole + 3
#define KVI_TTBID_HIGHLIGHT Qt::UserRole + 4
#define KVI_TTBID_PROGRESS Qt::UserRole + 5

class KVIRC_API KviTreeTaskBarItemDelegate : public KviTalIconAndRichTextItemDelegate
{
	Q_OBJECT
public:
	KviTreeTaskBarItemDelegate(QAbstractItemView * pWidget=0)
		: KviTalIconAndRichTextItemDelegate(pWidget) {};
	~KviTreeTaskBarItemDelegate(){};
	 QSize sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const;
	void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
	int calculateColor(int col1,int col2, int iStepNumber) const;
};
#endif //_KVI_TASKBAR_H_
