#ifndef _CLASS_WIDGET_H_
#define _CLASS_WIDGET_H_
//=============================================================================
//
//   File : KvsObject_widget.h
//   Creation date : Mon Sep 11 16:35:32 CET 2000 by Krzysztof Godlewski
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000 Krzysztof Godlewski
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#include "KviCString.h"
#include "KviTalToolTip.h"
#include "object_macros.h"

#include <QGraphicsEffect>

#ifdef COMPILE_WEBKIT_SUPPORT
#include <QtWebKitWidgets/QWebView>
#endif

class KvsObject_widget : public KviKvsObject
{
	Q_OBJECT
public:
	KVSO_DECLARE_OBJECT(KvsObject_widget)

public:
	QWidget * widget() { return (QWidget *)object(); };

protected:
	bool init(KviKvsRunTimeContext * pContext, KviKvsVariantList * pParams) override;

#ifdef COMPILE_WEBKIT_SUPPORT
	QWebView * m_pWebview = nullptr;
#endif
	bool eventFilter(QObject * o, QEvent * e) override;
	KviKvsRunTimeContext * m_pContext = nullptr;
	QGraphicsDropShadowEffect * pGraphicsEffect = nullptr;
	// ok, it is clear that we're messing with the naming conventions for the
	// object classes :D
	// let's try to use this one:
	// isEnabled
	// setEnabled..
	// ... etc
	// so the name of the C++ function matches exactly the name of the KVS function at this point

	// let's also try to keep alphabetic order for the functions here.. so one can find them quickly

	bool addWidgetToWrappedLayout(KviKvsObjectFunctionCall * c);
	bool backgroundColor(KviKvsObjectFunctionCall * c);
	bool centerToScreen(KviKvsObjectFunctionCall * c);
	bool colorPalette(KviKvsObjectFunctionCall * c);
	bool fontAscent(KviKvsObjectFunctionCall * c);
	bool fontDescent(KviKvsObjectFunctionCall * c);
	bool fontMetricsHeight(KviKvsObjectFunctionCall * c);
	bool fontMetricsWidth(KviKvsObjectFunctionCall * c);
	bool fontMetricsLineSpacing(KviKvsObjectFunctionCall * c);
	bool foregroundColor(KviKvsObjectFunctionCall * c);
	bool geometry(KviKvsObjectFunctionCall * c);
	bool globalCursorX(KviKvsObjectFunctionCall * c);
	bool globalCursorY(KviKvsObjectFunctionCall * c);
	bool hasFocus(KviKvsObjectFunctionCall * c);
	bool height(KviKvsObjectFunctionCall *);
	bool hide(KviKvsObjectFunctionCall *);
	bool isEnabled(KviKvsObjectFunctionCall * c);
	bool isTopLevel(KviKvsObjectFunctionCall * c);
	bool isVisible(KviKvsObjectFunctionCall * c);
	bool loadInterface(KviKvsObjectFunctionCall * c);
	bool lower(KviKvsObjectFunctionCall *);
	bool mapFromGlobal(KviKvsObjectFunctionCall * c);
	bool mapToGlobal(KviKvsObjectFunctionCall * c);
	bool move(KviKvsObjectFunctionCall * c);
	bool parentWidget(KviKvsObjectFunctionCall * c);
	bool raise(KviKvsObjectFunctionCall *);
	bool repaint(KviKvsObjectFunctionCall * c);
	bool insertIntoStatusBar(KviKvsObjectFunctionCall * c);
	bool removeFromStatusBar(KviKvsObjectFunctionCall * c);
	bool setParent(KviKvsObjectFunctionCall * c);
	bool resize(KviKvsObjectFunctionCall * c);
	bool screenResolution(KviKvsObjectFunctionCall * c);
	bool setAttribute(KviKvsObjectFunctionCall * c);
	bool setBackgroundColor(KviKvsObjectFunctionCall * c);
	bool setBackgroundImage(KviKvsObjectFunctionCall * c);
	bool setEnabled(KviKvsObjectFunctionCall * c);
	bool setFixedHeight(KviKvsObjectFunctionCall * c);
	bool setFixedSize(KviKvsObjectFunctionCall * c);
	bool setFixedWidth(KviKvsObjectFunctionCall * c);
	bool setFocus(KviKvsObjectFunctionCall * c);
	bool setFocusPolicy(KviKvsObjectFunctionCall * c);
	bool setFont(KviKvsObjectFunctionCall * c);
	bool setGeometry(KviKvsObjectFunctionCall * c);
	bool setKeyShortcut(KviKvsObjectFunctionCall * c);
	bool setMask(KviKvsObjectFunctionCall * c);
	bool setMaximumHeight(KviKvsObjectFunctionCall * c);
	bool setMaximumWidth(KviKvsObjectFunctionCall * c);
	bool setMinimumHeight(KviKvsObjectFunctionCall * c);
	bool setMinimumWidth(KviKvsObjectFunctionCall * c);
	bool setMouseTracking(KviKvsObjectFunctionCall * c);
	bool setForegroundColor(KviKvsObjectFunctionCall * c);
	bool setSizePolicy(KviKvsObjectFunctionCall * c);
	bool setStyleSheet(KviKvsObjectFunctionCall * c);
	bool setToolTip(KviKvsObjectFunctionCall *);
	bool setWindowIcon(KviKvsObjectFunctionCall * c);
	bool setWindowTitle(KviKvsObjectFunctionCall * c);
	bool setWFlags(KviKvsObjectFunctionCall * c);
	bool show(KviKvsObjectFunctionCall * c);
	bool sizeHint(KviKvsObjectFunctionCall * c);
	bool update(KviKvsObjectFunctionCall * c);
	bool width(KviKvsObjectFunctionCall *);
	bool windowTitle(KviKvsObjectFunctionCall * c);
	bool x(KviKvsObjectFunctionCall *);
	bool y(KviKvsObjectFunctionCall *);

	bool grab(KviKvsObjectFunctionCall *);
#ifdef COMPILE_WEBKIT_SUPPORT
	bool setWebView(KviKvsObjectFunctionCall * c);
#endif

signals:
	void aboutToDie();
};

class KviKvsWidget : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QSize sizeHint READ sizeHint)
public:
	KviKvsWidget(KvsObject_widget * ob, QWidget * par);
	~KviKvsWidget();

protected:
	KvsObject_widget * m_pObject;

public:
	QSize sizeHint() const override;
};
#endif //_CLASS_WIDGET_H_
