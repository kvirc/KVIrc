#ifndef _KVI_IRCVIEW_H_
#define _KVI_IRCVIEW_H_
//=============================================================================
//
//   File : KviIrcView.h
//   Creation date : Fri Mar 19 1999 05:39:01 by Szymon Stefanek
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
#include "KviCString.h"

#include <QToolButton>
#include <QWidget>
#include <QPixmap> // needed
#include <QMultiHash>
#include <QDateTime>

#include <vector>

class QScrollBar;
class QLineEdit;
class QFile;
class QFontMetrics;
class QMenu;
class QScreen;

class KviWindow;
class KviMainWindow;
class KviConsoleWindow;
class KviIrcViewToolWidget;
class KviIrcViewToolTip;
class KviAnimatedPixmap;

struct KviIrcViewLineChunk;
struct KviIrcViewWrappedBlock;
struct KviIrcViewLine;
struct KviIrcViewWrappedBlockSelectionInfo;

#define KVI_IRCVIEW_INVALID_LINE_MARK_INDEX 0xffffffff

class KVIRC_API KviIrcView : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(int TransparencyCapable READ dummyRead)
	// we cannot #ifdef due to a bug in moc
	Q_PROPERTY(bool usePaintOnScreen READ getPaintOnScreen WRITE setPaintOnScreen)
public:
	friend class KviIrcViewToolTip;
	friend class KviIrcViewToolWidget;

public:
	KviIrcView(QWidget * parent, KviWindow * pWnd);
	~KviIrcView();

public:
	int dummyRead() const { return 0; };
	bool getPaintOnScreen() const { return testAttribute(Qt::WA_PaintOnScreen); };
	void setPaintOnScreen(bool bFlag) { setAttribute(Qt::WA_PaintOnScreen, bFlag); };
private:
	//	QDate                       m_lastLogDay;
	int m_iFlushTimer;
	KviIrcViewLine * m_pFirstLine;
	KviIrcViewLine * m_pCurLine; // Bottom line in the view
	KviIrcViewLine * m_pLastLine;
	KviIrcViewLine * m_pCursorLine;
	unsigned int m_uLineMarkLineIndex;
	QRect m_lineMarkArea;

	// Highliting of links
	KviIrcViewWrappedBlock * m_pLastLinkUnderMouse;
	int m_iLastLinkRectTop;
	int m_iLastLinkRectHeight;

	int m_iNumLines;
	int m_iMaxLines;

	unsigned int m_uNextLineIndex;

	QPixmap * m_pPrivateBackgroundPixmap;
	QScrollBar * m_pScrollBar;
	QToolButton * m_pToolsButton;
	QMenu * m_pToolsPopup;

	KviIrcViewToolWidget * m_pToolWidget;

	int m_iLastScrollBarValue;

	// Font related stuff (needs precalculation!)
	int m_iFontLineSpacing;
	int m_iFontLineWidth;
	int m_iFontDescent;
	int m_iFontCharacterWidth[256]; //1024 bytes fixed
	bool m_bUseRealBold;

	int m_iWrapMargin;
	int m_iMinimumPaintWidth;
	int m_iRelativePixmapY;
	int m_iIconWidth;
	int m_iIconSideSpacing;

	// Selection
	KviIrcViewLine * m_pSelectionInitLine;
	KviIrcViewLine * m_pSelectionEndLine;
	int m_iSelectionInitCharIndex;
	int m_iSelectionEndCharIndex;
	int m_iSelectTimer;

	bool m_bMouseIsDown;
	bool m_bShiftPressed;
	bool m_bCtrlPressed;

	bool m_bSkipScrollBarRepaint;
	int m_iMouseTimer;
	KviWindow * m_pKviWindow;
	KviIrcViewWrappedBlockSelectionInfo * m_pWrappedBlockSelectionInfo;
	QFile * m_pLogFile;
	KviMainWindow * m_pFrm;
	bool m_bAcceptDrops;
	int m_iUnprocessedPaintEventRequests;
	bool m_bPostedPaintEventPending;
	std::vector<KviIrcViewLine *> m_pMessagesStoppedWhileSelecting;
	KviIrcView * m_pMasterView;
	QFontMetrics * m_pFm; // assume this valid only inside a paint event (may be 0 in other circumstances)

	QMouseEvent * m_pLastEvent;
	qint64 m_iLastMouseClickTime;

	KviIrcViewToolTip * m_pToolTip;
	bool m_bHaveUnreadedHighlightedMessages;
	bool m_bHaveUnreadedMessages;

	QMultiHash<KviIrcViewLine *, KviAnimatedPixmap *> m_hAnimatedSmiles;

public:
	void clearUnreaded();
	void applyOptions();
	void enableDnd(bool bEnable);
	bool haveUnreadedMessages() { return m_bHaveUnreadedMessages; };
	bool haveUnreadedHighlightedMessages() { return m_bHaveUnreadedHighlightedMessages; };
	enum AppendTextFlags
	{
		NoRepaint = 1,
		NoTimestamp = 2,
		SetLineMark = 4,
		TriggersNotification = 8
	};
	void appendText(int msg_type, const kvi_wchar_t * data_ptr, int iFlags = 0, const QDateTime & datetime = QDateTime());
	void clearLineMark(bool bRepaint = false);
	bool hasLineMark() { return m_uLineMarkLineIndex != KVI_IRCVIEW_INVALID_LINE_MARK_INDEX; };
	void removeHeadLine(bool bRepaint = false);
	void emptyBuffer(bool bRepaint = true);
	void getTextBuffer(QString & buffer);
	void setMaxBufferSize(int maxBufSize, bool bRepaint = true);
	int maxBufferSize() { return m_iMaxLines; }; //Never used ?
	bool saveBuffer(const char * filename);
	void findNext(const QString & szText, bool bCaseS = false, bool bRegExp = false, bool bExtended = false);
	void findPrev(const QString & szText, bool bCaseS = false, bool bRegExp = false, bool bExtended = false);
	KviWindow * parentKviWindow() { return m_pKviWindow; };
	KviConsoleWindow * console();
	// A null pixmap passed here unsets the private backgrdound.
	void setPrivateBackgroundPixmap(const QPixmap & pixmap, bool bRepaint = true);
	QPixmap * getPrivateBackgroundPixmap() const { return m_pPrivateBackgroundPixmap; };
	bool hasPrivateBackgroundPixmap() { return (m_pPrivateBackgroundPixmap != nullptr); };

	// Logging
	// Stops previous logging session too...
	bool startLogging(const QString & fname = QString(), bool bPrependCurBuffer = false);
	void stopLogging();
	bool isLogging() { return (m_pLogFile != nullptr); };
	void getLogFileName(QString & buffer);
	void add2Log(const QString & szBuffer, const QDateTime & date, int iMsgType, bool bPrependDate);

	// Channel view splitting
	void setMasterView(KviIrcView * v);
	void splitMessagesTo(KviIrcView * v);
	void joinMessagesFrom(KviIrcView * v);
	void appendMessagesFrom(KviIrcView * v);

	qint64 lastMouseClickTime() const { return m_iLastMouseClickTime; }

	// Return true if the specified message type should be "split" to the user message specific view.
	bool messageShouldGoToMessageView(int iMsgType);

	void prevLine();
	void nextLine();
	void nextPage();
	void prevPage();
	void scrollTop();
	void scrollBottom();
	QSize sizeHint() const override;
	const QString & lastLineOfText();
	const QString & lastMessageText();
	void setFont(const QFont & f);
	void scrollToMarker();

protected:
	void paintEvent(QPaintEvent *) override;
	void resizeEvent(QResizeEvent *) override;
	void mousePressEvent(QMouseEvent * e) override;
	void mouseReleaseEvent(QMouseEvent *) override;
	void mouseDoubleClickEvent(QMouseEvent * e) override;
	void mouseMoveEvent(QMouseEvent * e) override;
	void timerEvent(QTimerEvent * e) override;
	void dragEnterEvent(QDragEnterEvent * e) override;
	void dropEvent(QDropEvent * e) override;
	void showEvent(QShowEvent * e) override;
	bool event(QEvent * e) override;
	void wheelEvent(QWheelEvent * e) override;
	void keyPressEvent(QKeyEvent * e) override;
	void maybeTip(const QPoint & pnt);
	void leaveEvent(QEvent *) override;

private:
	void triggerMouseRelatedKvsEvents(QMouseEvent * e);
	void setCursorLine(KviIrcViewLine * l);
	void ensureLineVisible(KviIrcViewLine * pLineToShow);
	KviIrcViewLine * getVisibleLineAt(int yPos);
	int getVisibleCharIndexAt(KviIrcViewLine * line, int xPos, int yPos);
	void getLinkEscapeCommand(QString & buffer, const QString & escape_cmd, const QString & escape_label);
	void appendLine(KviIrcViewLine * ptr, const QDateTime & date, bool bRepaint);
	void postUpdateEvent();
	void fastScroll(int lines = 1);
	const kvi_wchar_t * getTextLine(int msg_type, const kvi_wchar_t * data_ptr, KviIrcViewLine * line_ptr, bool bEnableTimeStamp = true, const QDateTime & datetime = QDateTime());
	void calculateLineWraps(KviIrcViewLine * ptr, int maxWidth);
	void recalcFontVariables(const QFont & font, const QFontInfo & fi);
	bool checkSelectionBlock(KviIrcViewLine * line, int bufIndex);
	KviIrcViewWrappedBlock * getLinkUnderMouse(int xPos, int yPos, QRect * pRect = nullptr, QString * linkCmd = nullptr, QString * linkText = nullptr);
	void doLinkToolTip(const QRect & rct, QString & linkCmd, QString & linkText);
	void doMarkerToolTip();
	bool checkMarkerArea(const QPoint & mousePos);
	void addControlCharacter(KviIrcViewLineChunk * pC, QString & szSelectionText);
	void reapplyMessageColors();
public slots:
	void flushLog();
	void showToolsPopup();
	void clearBuffer();
	void toggleToolWidget();
	void increaseFontSize();
	void decreaseFontSize();
	void resetDefaultFont();
	void chooseFont();
	void chooseBackground();
	void resetBackground();
protected slots:
	virtual void scrollBarPositionChanged(int newValue);
	void screenChanged(QScreen *);
	void masterDead();
	void animatedIconChange();
signals:
	void rightClicked();
	void dndEntered();
	void fileDropped(const QString &);
};

#endif //_KVI_IRCVIEW_H_
