/***************************************************************************
                          xpstyle.h  -  description
                             -------------------
    begin                : Sun Mar 17 2002
    copyright            : (C) 2002 by Vladimir Shutoff
    email                : vovan@shutoff.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _XPSTYLE_H
#define _XPSTYLE_H

#if defined(_MSC_VER) && defined(_DEBUG) && !defined(NO_CHECK_NEW)
#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#endif
#include <stdlib.h>
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#include <qwindowsstyle.h>

class QWindowsXPStylePrivate;

class QWindowsXPStyle: public QWindowsStyle
{
    Q_OBJECT
public:
    QWindowsXPStyle();
    virtual ~QWindowsXPStyle();
    void polish( QApplication *app );
    void unPolish( QApplication *app );
    void polish( QWidget *widget );
    void unPolish( QWidget *widget );
    void updateRegion( QWidget *widget );
    virtual void drawButton( QPainter *p, int x, int y, int w, int h,
                             const QColorGroup &g, bool sunken = FALSE,
                             const QBrush *fill = 0 );
    virtual void drawBevelButton( QPainter *p, int x, int y, int w, int h,
                                  const QColorGroup &g, bool sunken = FALSE,
                                  const QBrush *fill = 0 );
    virtual void drawButtonMask( QPainter *p, int x, int y, int w, int h);
    virtual void drawPushButton( QPushButton* btn, QPainter *p);
    virtual void drawToolButton( QPainter *p, int x, int y, int w, int h,
                                 const QColorGroup &g, bool sunken = FALSE,
                                 const QBrush *fill = 0 );
    virtual QSize exclusiveIndicatorSize() const;
    virtual void drawExclusiveIndicator( QPainter* p, int x, int y, int w, int h,
                                         const QColorGroup &g, bool on, bool down = FALSE, bool enabled = TRUE );
    virtual void drawExclusiveIndicatorMask( QPainter *p, int x, int y, int w, int h, bool on);
    virtual QSize indicatorSize() const ;
    virtual void drawIndicator( QPainter* p, int x, int y, int w, int h, const QColorGroup &g,
                                int state, bool down = FALSE, bool enabled = TRUE );
    virtual void drawIndicatorMask( QPainter *p, int x, int y, int w, int h, int state);
    virtual void drawScrollBarControls( QPainter*,  const QScrollBar*,
                                        int sliderStart, uint controls,
                                        uint activeControl );
    virtual void drawComboButton( QPainter *p, int x, int y, int w, int h,
                                  const QColorGroup &g, bool sunken = FALSE,
                                  bool editable = FALSE,
                                  bool enabled = TRUE,
                                  const QBrush *fill = 0 );
    virtual void drawPanel( QPainter *p, int x, int y, int w, int h,
                            const QColorGroup &, bool sunken=FALSE,
                            int lineWidth = 1, const QBrush *fill = 0 );
    virtual void drawPopupPanel( QPainter *p, int x, int y, int w, int h,
                                 const QColorGroup &,  int lineWidth = 2,
                                 const QBrush *fill = 0 );
    //    virtual void tabbarMetrics( const QTabBar*, int&, int&, int& );
    virtual void drawTab( QPainter*, const QTabBar*, QTab*, bool selected );
    virtual void drawTabMask( QPainter*, const QTabBar*, QTab*, bool selected );
    virtual void drawPopupMenuItem( QPainter* p, bool checkable,
                                    int maxpmw, int tab, QMenuItem* mi,
                                    const QPalette& pal,
                                    bool act, bool enabled,
                                    int x, int y, int w, int h);
    virtual void drawSlider ( QPainter * p, int x, int y, int w, int h, const QColorGroup & g, Orientation, bool tickAbove, bool tickBelow );
    virtual void drawSliderGroove ( QPainter * p, int x, int y, int w, int h, const QColorGroup & g, QCOORD c, Orientation );
protected slots:
    void activeTabChanged();
protected:
    bool eventFilter( QObject *o, QEvent *e );
    QWindowsXPStylePrivate *d;
    QColor colorMenu(const QColorGroup &cg) const;
    QColor colorBitmap(const QColorGroup &cg) const;
    QColor colorSel(const QColorGroup &cg) const;
    QColor darkXp(const QColor &c) const;
    QColor shiftColor( const QColor &c, int value, bool bXp=true) const;
    QColor mix(const QColor &c1, const QColor &c2, int k) const;
    void drawMenuBackground(QPainter *p,
                            const QRect &rc,
                            const QColorGroup &cg) const;
};

#endif

