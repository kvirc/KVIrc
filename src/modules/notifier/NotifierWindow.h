#ifndef _NOTIFIERWINDOW_H_
#define _NOTIFIERWINDOW_H_
//=============================================================================
//
//   File : NotifierWindow.h
//   Creation date : Tue Jul 6 2004 20:25:12 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2005-2008 Iacopo Palazzi < iakko(at)siena(dot)linux(dot)it >
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

#include "NotifierSettings.h"

#include "kvi_settings.h"
#include "KviQString.h"
#include "KviPointerList.h"
#include "KviTimeUtils.h"

#include <QBitmap>
#include <QColor>
#include <QCursor>
#include <QDateTime>
#include <QImage>
#include <QPixmap>
#include <QRect>
#include <QTimer>
#include <QWidget>
#include <QTabWidget>
#include <QProgressBar>
#include <QMenu>

class QPainter;
class KviWindow;
class NotifierMessage;
class NotifierWindowBorder;
class KviNotifierWindowTabs;
class KviThemedLineEdit;

extern kvi_time_t g_tNotifierDisabledUntil;

class NotifierWindow : public QWidget
{
	Q_OBJECT
public:
	NotifierWindow();
	~NotifierWindow();

protected:
	QTimer * m_pShowHideTimer = nullptr;
	QTimer * m_pBlinkTimer = nullptr;
	QTimer * m_pAutoHideTimer = nullptr;
	State m_eState = Hidden;
	bool m_bBlinkOn = false;
	double m_dOpacity = 0.0;

	bool m_bCloseDown = false;
	bool m_bPrevDown = false;
	bool m_bNextDown = false;
	bool m_bWriteDown = false;
	bool m_bCrashShowWorkAround;

	QRect m_wndRect;

	NotifierMessage * m_pCurrentMessage = nullptr;
	KviThemedLineEdit * m_pLineEdit = nullptr;

	bool m_bDragging = false;
	bool m_bLeftButtonIsPressed = false;
	bool m_bDiagonalResizing = false;
	bool m_bResizing = false;

	int m_whereResizing;

	QPoint m_pntDrag;
	QPoint m_pntPos;
	QPoint m_pntClick;
	int m_iBlinkCount;
	QMenu * m_pContextPopup = nullptr;
	QMenu * m_pDisablePopup = nullptr;
	KviWindow * m_pWindowToRaise = nullptr;
	kvi_time_t m_tAutoHideAt = 0;
	kvi_time_t m_tStartedAt = 0;
	QTime m_qtStartedAt;
	bool m_bDisableHideOnMainWindowGotAttention = false;

	QCursor m_cursor;

	QTabWidget * m_pWndTabs = nullptr;
	QProgressBar * m_pProgressBar = nullptr;
	NotifierWindowBorder * m_pWndBorder = nullptr;

public:
	void doShow(bool bDoAnimate);
	void doHide(bool bDoAnimate);
	int textWidth();
	void addMessage(KviWindow * pWnd, const QString & szImageId, const QString & szText, unsigned int uMessageTime);
	void setDisableHideOnMainWindowGotAttention(bool b) { m_bDisableHideOnMainWindowGotAttention = b; };
	void showLineEdit(bool bShow);
	int countTabs() const
	{
		if(m_pWndTabs)
			return m_pWndTabs->count();
		return 0;
	}
	State state() const { return m_eState; }
protected:
	void showEvent(QShowEvent * e) override;
	void hideEvent(QHideEvent * e) override;
	void paintEvent(QPaintEvent * e) override;
	void mousePressEvent(QMouseEvent * e) override;
	void mouseReleaseEvent(QMouseEvent * e) override;
	void mouseMoveEvent(QMouseEvent * e) override;
	void leaveEvent(QEvent * e) override;
	void enterEvent(QEvent * e) override;
	bool eventFilter(QObject * pEdit, QEvent * e) override;
	void keyPressEvent(QKeyEvent * e) override;
public slots:
	void hideNow();
	void toggleLineEdit();
	void slotTabCloseRequested(int index);
protected slots:
	void blink();
	void heartbeat();
	void returnPressed();
	void updateGui();
	void fillContextPopup();
	void disableFor1Minute();
	void disableFor5Minutes();
	void disableFor15Minutes();
	void disableFor30Minutes();
	void disableFor60Minutes();
	void disableUntilKVIrcRestarted();
	void disablePermanently();
	void progressUpdate();

private:
	void contextPopup(const QPoint & pos);
	void startBlinking();
	// 	void computeRect();
	void stopShowHideTimer();
	void stopBlinkTimer();
	void stopAutoHideTimer();
	void startAutoHideTimer();
	bool shouldHideIfMainWindowGotAttention();
	void setCursor(int);
	void resize(QPoint p, bool = true);
	void redrawText();
	bool checkResizing(QPoint);
};

#endif //_NOTIFIERWINDOW_H_
