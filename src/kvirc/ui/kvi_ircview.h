#ifndef _KVI_IRCVIEW_H_
#define _KVI_IRCVIEW_H_
//=============================================================================
//
//   File : kvi_ircview.h
//   Creation date : Fri Mar 19 1999 05:39:01 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2008 Szymon Stefanek (pragma at kvirc dot net)
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
#include "kvi_string.h"
#include "kvi_pointerlist.h"

#include <QToolButton>
#include <QWidget>
#include <QPixmap>      // needed
#include <QMultiHash>

class QScrollBar;
class QLineEdit;
class QFile;
class QFontMetrics;
class KviTalPopupMenu;

class KviWindow;
class KviFrame;
class KviConsole;
class KviIrcViewToolWidget;
class KviIrcViewToolTip;
class KviAnimatedPixmap;

typedef struct _KviIrcViewLineChunk KviIrcViewLineChunk;
typedef struct _KviIrcViewWrappedBlock KviIrcViewWrappedBlock;
typedef struct _KviIrcViewLine KviIrcViewLine;
typedef struct _KviIrcViewWrappedBlockSelectionInfoTag KviIrcViewWrappedBlockSelectionInfo;

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
	KviIrcView(QWidget *parent,KviFrame *pFrm,KviWindow *pWnd);
	~KviIrcView();
public:
	int dummyRead() const { return 0; };
	bool getPaintOnScreen() const { return testAttribute(Qt::WA_PaintOnScreen);};
	void setPaintOnScreen(bool bFlag){setAttribute(Qt::WA_PaintOnScreen,bFlag);} ;
private:
//	QDate                       m_lastLogDay;
	int		 	                m_iFlushTimer;
	KviIrcViewLine            * m_pFirstLine;
	KviIrcViewLine            * m_pCurLine;    // Bottom line in the view
	KviIrcViewLine            * m_pLastLine;
	KviIrcViewLine            * m_pCursorLine;
	unsigned int                m_uLineMarkLineIndex;

	// Highliting of links
	KviIrcViewWrappedBlock    * m_pLastLinkUnderMouse;
	int                         m_iLastLinkRectTop;
	int                         m_iLastLinkRectHeight;

	int                         m_iNumLines;
	int                         m_iMaxLines;

	unsigned int                m_uNextLineIndex;

	QPixmap                   * m_pPrivateBackgroundPixmap;
	QScrollBar                * m_pScrollBar;
	QToolButton               * m_pToolsButton;
	KviTalPopupMenu           * m_pToolsPopup;

	KviIrcViewToolWidget      * m_pToolWidget;

	int                         m_iLastScrollBarValue;

	// Font related stuff (needs precalculation!)
	int                         m_iFontLineSpacing;
	int                         m_iFontLineWidth;
	int                         m_iFontDescent;
	int                         m_iFontCharacterWidth[256];    //1024 bytes fixed

	int                         m_iWrapMargin;
	int                         m_iMinimumPaintWidth;
	int                         m_iRelativePixmapY;
	int                         m_iIconWidth;
	int                         m_iIconSideSpacing;

	QPoint                      m_mousePressPos;
	QPoint                      m_mouseCurrentPos;

	// Selection
	KviIrcViewLine            * m_pSelectionInitLine;
	KviIrcViewLine            * m_pSelectionEndLine;
	int                         m_iSelectionInitCharIndex;
	int                         m_iSelectionEndCharIndex;
	int                         m_iSelectTimer;

	bool                        m_bMouseIsDown;
	bool                        m_bShiftPressed;

	bool                        m_bSkipScrollBarRepaint;
	int                         m_iMouseTimer;
	KviWindow                 * m_pKviWindow;
	KviIrcViewWrappedBlockSelectionInfo * m_pWrappedBlockSelectionInfo;
	QFile                     * m_pLogFile;
	KviFrame                  * m_pFrm;
	bool                        m_bAcceptDrops;
	int                         m_iUnprocessedPaintEventRequests;
	bool                        m_bPostedPaintEventPending;
	KviPointerList<KviIrcViewLine> * m_pMessagesStoppedWhileSelecting;
	KviIrcView                * m_pMasterView;
	QFontMetrics              * m_pFm;               // assume this valid only inside a paint event (may be 0 in other circumstances)

	QMouseEvent              *  m_pLastEvent;

	KviIrcViewToolTip         * m_pToolTip;
	bool m_bHaveUnreadedHighlightedMessages;
	bool m_bHaveUnreadedMessages;

	QMultiHash<KviIrcViewLine*,KviAnimatedPixmap*>  m_hAnimatedSmiles;
public:
	void clearUnreaded();
	void applyOptions();
	void enableDnd(bool bEnable);
	bool haveUnreadedMessages() { return m_bHaveUnreadedMessages; };
	bool haveUnreadedHighlightedMessages() { return m_bHaveUnreadedHighlightedMessages; };
	enum AppendTextFlags { NoRepaint = 1, NoTimestamp = 2, SetLineMark = 4 };
	void appendText(int msg_type,const kvi_wchar_t *data_ptr,int iFlags = 0);
	void clearLineMark(bool bRepaint=false);
	bool hasLineMark(){ return m_uLineMarkLineIndex != KVI_IRCVIEW_INVALID_LINE_MARK_INDEX; };
	void removeHeadLine(bool bRepaint=false);
	void emptyBuffer(bool bRepaint=true);
	void getTextBuffer(QString &buffer);
	void setMaxBufferSize(int maxBufSize,bool bRepaint=true);
	int  maxBufferSize(){ return m_iMaxLines; }; //Never used ?
	bool saveBuffer(const char *filename);
	void findNext(const QString& szText,bool bCaseS = false,bool bRegExp = false,bool bExtended = false);
	void findPrev(const QString& szText,bool bCaseS = false,bool bRegExp = false,bool bExtended = false);
	KviWindow * parentKviWindow(){ return m_pKviWindow; };
	KviConsole * console();
	// A null pixmap passed here unsets the private backgrdound.
	void setPrivateBackgroundPixmap(const QPixmap &pixmap,bool bRepaint=true);
	bool hasPrivateBackgroundPixmap(){ return (m_pPrivateBackgroundPixmap != 0); };

	//==============================================================================================
	// Logging
	// Stops previous logging session too...
	bool startLogging(const QString& fname = QString(),bool bPrependCurBuffer = false);
	void stopLogging();
	bool isLogging(){ return (m_pLogFile != 0); };
	void getLogFileName(QString &buffer);
	void add2Log(const QString &szBuffer,int iMsgType = 0,bool bPrependDate = false);

	//==============================================================================================
	// Channel view splitting
	void setMasterView(KviIrcView * v);
	void splitMessagesTo(KviIrcView * v);
	void joinMessagesFrom(KviIrcView * v);
	void appendMessagesFrom(KviIrcView * v);

	void prevLine();
	void nextLine();
	void nextPage();
	void prevPage();
	virtual QSize sizeHint() const;
	const QString & lastLineOfText();
	const QString & lastMessageText();
	virtual void setFont(const QFont &f);
public slots:
	void flushLog();
	void showToolsPopup();
	void clearBuffer();
	void toggleToolWidget();
	void increaseFontSize();
	void decreaseFontSize();
	void chooseFont();
	void chooseBackground();
	void resetBackground();
signals:
	void rightClicked();
	void dndEntered();
	void fileDropped(const QString &);
private:
	void setCursorLine(KviIrcViewLine * l);
	KviIrcViewLine * getVisibleLineAt(int yPos);
	int getVisibleCharIndexAt(KviIrcViewLine * line, int xPos, int yPos);
	void getLinkEscapeCommand(QString &buffer,const QString &escape_cmd,const QString &escape_label);
	void appendLine(KviIrcViewLine *ptr,bool bRepaint);
	void postUpdateEvent();
	void fastScroll(int lines = 1);
	const kvi_wchar_t * getTextLine(int msg_type,const kvi_wchar_t * data_ptr,KviIrcViewLine *line_ptr,bool bEnableTimeStamp = true);
	void calculateLineWraps(KviIrcViewLine *ptr,int maxWidth);
	void recalcFontVariables(const QFontMetrics &fm,const QFontInfo &fi);
	bool checkSelectionBlock(KviIrcViewLine * line,int bufIndex);
	KviIrcViewWrappedBlock * getLinkUnderMouse(int xPos,int yPos,QRect * pRect = 0,QString * linkCmd = 0,QString * linkText = 0);
	void doLinkToolTip(const QRect &rct,QString &linkCmd,QString &linkText);
	void doMarkerToolTip(const QRect &rct);
	bool checkMarkerArea(const QRect & area, const QPoint & mousePos);
	void addControlCharacter(KviIrcViewLineChunk *pC, QString & szSelectionText);
protected:
	virtual void paintEvent(QPaintEvent *);
	virtual void resizeEvent(QResizeEvent *);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseRealPressEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void mouseDoubleClickEvent(QMouseEvent *e);
	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void timerEvent(QTimerEvent *e);
	virtual void dragEnterEvent(QDragEnterEvent *e);
	virtual void dropEvent(QDropEvent *e);
	virtual bool event(QEvent *e);
	virtual void wheelEvent(QWheelEvent *e);
	virtual void keyPressEvent(QKeyEvent *e);
	void maybeTip(const QPoint &pnt);
	virtual void leaveEvent(QEvent *);
protected slots:
	virtual void scrollBarPositionChanged(int newValue);
	void masterDead();
	void animatedIconChange();
};

#endif //_KVI_IRCVIEW_H_
