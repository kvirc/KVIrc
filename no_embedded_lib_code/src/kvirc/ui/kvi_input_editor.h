#ifndef _KVI_INPUT_EDITOR_H_
#define _KVI_INPUT_EDITOR_H_
//============================================================================
//
//   File : kvi_input_editor.h
//   Creation date : Fri Sep 5 2008 17:26:34 by Elvio Basello
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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
//============================================================================
//   This file was originally part of kvi_input.h
//============================================================================

/**
* \file kvi_input_editor.h
* \author Elvio Basello
* \brief Input editor
*
* \def KVI_INPUT_MAX_BUFFER_SIZE Default maximum buffer size
* \def KVI_INPUT_BORDER Default border
* \def KVI_INPUT_MARGIN Default margin
* \def KVI_INPUT_BLINK_TIME Cursor blink time...just don't set it to a value less than 100 if you don't want to be lagged by your cursors :)
* \def KVI_INPUT_DRAG_TIMEOUT Drag scroll speed...(smaller values = faster)
* \def KVI_INPUT_DEF_BACK Default background color
* \def KVI_INPUT_DEF_FORE Default foreground color
* \def ACCEL_KEY Accelerator key
*/

#include "kvi_settings.h"
#include "kvi_pointerlist.h"
#include "kvi_string.h"
#include "kvi_window.h"

#include <QFrame>
#include <QString>
#include <QWidget>

class QKeyEvent;
class QDragEnterEvent;
class KviUserListView;
class KviTalPopupMenu;

#define KVI_INPUT_MAX_BUFFER_SIZE 400
#define KVI_INPUT_BORDER 1
#define KVI_INPUT_MARGIN 2
#define KVI_INPUT_BLINK_TIME 800
#define KVI_INPUT_DRAG_TIMEOUT 80
#define KVI_INPUT_DEF_BACK 100
#define KVI_INPUT_DEF_FORE 101

#ifndef ACCEL_KEY
#define ACCEL_KEY(k) "\t" + QString(QKeySequence( Qt::CTRL | Qt::Key_ ## k ))
#endif

/**
* \class KviInputEditor
* \brief Input editor class
*/
class KVIRC_API KviInputEditor : public QFrame
{
	//Q_PROPERTY( int KviProperty_FocusOwner READ heightHint )
	Q_PROPERTY( int TransparencyCapable READ heightHint )
	Q_OBJECT
	friend class KviTopicWidget; // it uses home()
public:
	/**
	* \brief Constructs the input editor object
	* \param pPar The parent widget
	* \param pWnd The KviWindow object
	* \param pView The userlist
	* \return KviInputEditor
	*/
	KviInputEditor(QWidget * pPar, KviWindow * pWnd, KviUserListView * pView = 0);

	/**
	* \brief Destroys the Input editor object
	*/
	~KviInputEditor();
protected:
	QString                   m_szTextBuffer;
	int                       m_iCursorPosition;
	int                       m_iFirstVisibleChar;
	int                       m_iSelectionBegin;
	int                       m_iSelectionEnd;
	int                       m_iMaxBufferSize;
	bool                      m_bSpSlowFlag; // <-- what is this ?

	// members for supporting input methods
	QString                   m_szIMText;
	int                       m_iIMStart;
	int                       m_iIMLength;
	int                       m_iIMSelectionBegin;
	int                       m_iIMSelectionLength;
	bool                      m_bIMComposing;

	unsigned char             m_iCurFore;
	unsigned char             m_iCurBack;
	bool                      m_bCurBold;
	bool                      m_bCurUnderline;

	int                       m_iBlockLen;
	int                       m_iBlockWidth;
	bool                      m_bControlBlock;

	bool                      m_bCursorOn;

	int                       m_iCursorTimer;
	int                       m_iDragTimer;

	int                       m_iLastCursorXPosition;
	int                       m_iSelectionAnchorChar;

	// History stuff
	KviPointerList<QString> * m_pHistory;
	int                       m_iCurHistoryIdx;

	KviStr                    m_szSaveTextBuffer;

	// Nick completion
	KviUserListView         * m_pUserListView;
	QString                   m_szLastCompletedNick;
	QString                   m_szLastCompletionBuffer;
	int                       m_iLastCompletionCursorPosition;
	int                       m_iLastCompletionCursorXPosition;
	int                       m_iLastCompletionFirstVisibleChar;
	bool                      m_bLastCompletionFinished;

	bool                      m_bUpdatesEnabled;
	KviStr                    m_szAltKeyCode;
	KviWindow               * m_pKviWindow;
	QWidget                 * m_pInputParent;
	KviTalPopupMenu         * m_pIconMenu;
	bool                      m_bReadOnly;

	// undo/redo handling
	enum CommandType { Separator, Insert, Remove, Delete, RemoveSelection, DeleteSelection, SetSelection };
	struct Command {
		inline Command() {}
		inline Command(CommandType t, int p, QString c, int ss, int se) : type(t),us(c),pos(p),selStart(ss),selEnd(se) {}
		uint type : 4;
		QString us;
		int pos, selStart, selEnd;
	};
	QVector<Command>          history;
	int                       undoState;
	bool                      separator;
public:
	/**
	* \brief Returns the height of the editor
	* \return int
	*/
	int heightHint() const;

	/**
	* \brief Returns the size of the editor
	* \return QSize
	*/
	virtual QSize sizeHint() const;

	/**
	* \brief Sets the text in the input line
	* \param szText The text to set
	* \return void
	*/
	void setText(const QString szText);

	/**
	* \brief Returns the text in the buffer
	* \return QString
	*/
	QString text() { return m_szTextBuffer; };

	/**
	* \brief Inserts a character at the current position
	* \param c The character to insert
	* \return void
	*/
	void insertChar(QChar c);

	/**
	* \brief Inserts the text at the current position
	* \param szTxt The text to insert
	* \return void
	*/
	void insertText(const QString & szTxt);

	/**
	* \brief Applies the options and update
	* \return void
	*/
	void applyOptions();

	/**
	* \brief Sets the maximum buffer size
	* \param iSize The size of the buffer to set
	* \return void
	*/
	void setMaxBufferSize(int iSize) { m_iMaxBufferSize = iSize; };

	/**
	* \brief Sets the read only state for the input line
	* \param bReadOnly Whether to set the read only state
	* \return void
	*/
	void setReadOnly(bool bReadOnly) { m_bReadOnly = bReadOnly; };

	/**
	* \brief Returns true if the input line is in read only state
	* \return bool
	*/
	bool readOnly() { return m_bReadOnly; };
private:
	/**
	* \brief Replaces the word before the cursor
	* \param szWord The word to replace
	* \param szReplacement The new word to use
	* \param bRepaint Whether to repain the input line
	* \return void
	*/
	void replaceWordBeforeCursor(const QString & szWord, const QString & szReplacement, bool bRepaint = true);

	/**
	* \brief Replace characters in the buffer
	*
	* Returns the length of the text inserted (different from text.length() only if the buffer was truncated.
	* \param iStart The starting character
	* \param iLength The length of the string to replace
	* \param szString The string to replace with
	* \return int
	*/
	int replaceSegment(int iStart, int iLength, const QString & szString);

	/**
	* \brief Gets the word before the cursor and puts it to the buffer
	* \param szBuffer The buffer where to put the word got
	* \param bIsFirstWordInLine Returns true if the word is the first
	* \return void
	*/
	void getWordBeforeCursor(QString & szBuffer, bool * bIsFirstWordInLine);

	/**
	* \brief Moves the cursor to the first visible character
	* \return void
	*/
	void runUpToTheFirstVisibleChar();

	/**
	* \brief Extracts the next block of text
	* \param iIdx The index of the character to check
	* \param fm The font metrics to use
	* \param iCurXPos The current position
	* \param iMaxXPos The maximum position of the text
	*/
	void extractNextBlock(int iIdx, QFontMetrics & fm, int iCurXPos, int iMaxXPos);

	/**
	* \brief Draws a block of text
	* \param pa The pointer to the painter
	* \param fm The font metrics to use
	* \param iCurXPos The current X posistion
	* \param iTextBaseline The current Y position
	* \param iIdx The index of the character to start crop
	* \param iLen The length of the string to crop
	* \param bSelected Whether the text is selected
	* \return void
	*/
	void drawTextBlock(QPainter * pa, QFontMetrics & fm, int iCurXPos, int iTextBaseline, int iIdx, int iLen, bool bSelected = false);

	/**
	* \brief Gets the substitute character for control codes
	* \param uControlCode The control code inserted
	* \return QChar
	*/
	QChar getSubstituteChar(unsigned short uControlCode);

	/**
	* \brief Moves the internal cursor to the first visibile char to the right
	* \return void
	*/
	void moveRightFirstVisibleCharToShowCursor();

	/**
	* \brief Repaints the input line with the visible cursor
	* \return void
	*/
	void repaintWithCursorOn();

	/**
	* \brief Selects one character at the given position
	* \param iPos The position of the character to select
	* \return void
	*/
	void selectOneChar(int iPos);

	/**
	* \brief Returns the current character from a given position
	* \param iXPos The position to grab
	* \return int
	*/
	int charIndexFromXPosition(int iXPos);

	/**
	* \brief Returns the current position from a given character
	* \param fm The font metrics
	* \param iChIdx Th index of the character
	* \param bContentsCoords Whether to use the frame width in addiction to the margin
	* \return int
	*/
	int xPositionFromCharIndex(QFontMetrics & fm, int iChIdx, bool bContentsCoords = false);

	/**
	* \brief Returns the current position from a given character
	* \param iChIdx Th index of the character
	* \param bContentsCoords Whether to use the frame width in addiction to the margin
	* \return int
	*/
	int xPositionFromCharIndex(int iChIdx, bool bContentsCoords = false);

	/**
	* \brief Kills the drag timer
	* \return void
	*/
	void killDragTimer();

	/**
	* \brief Handles the drag selection
	* \return void
	*/
	void handleDragSelection();

	/**
	* \brief Moves the cursor to the end of the input line
	* \return void
	*/
	void end();

	/**
	* \brief Moves the cursor to the start of the input line
	* \return void
	*/
	void home();

	/**
	* \brief Returns true if the input line has focus
	* \return bool
	*/
	bool hasSelection();

	/**
	* \brief Moves the cursor to the specified position
	* \param iIdx The index of the character to move the cursor to
	* \param bRepaint Whether to repaint the input line
	* \return void
	*/
	void moveCursorTo(int iIdx, bool bRepaint = true);

	/**
	* \brief Adds the text to the history
	*
	* Triggered when the user press return
	* \param bRepaint Whether to repain the input line
	* \return void
	*/
	void returnPressed(bool bRepaint = true);

	/**
	* \brief Autocompletion function
	*
	* To autocomplete, this function looks for a nickname, a directory, or a
	* nickname.
	* \param bShift Whether to complete with the mask of the nickname
	* \return void
	*/
	void completion(bool bShift);

	/**
	* \brief Completes the nickname
	* \param bAddMask Whether to complete with the mask of the nickname
	* \param szWord The nickname to complete
	* \param bFirstWordInLine Whether to add the suffix from option panel
	* \return void
	*/
	void standardNickCompletion(bool bAddMask, QString & szWord, bool bFirstWordInLine);

	/**
	* \brief Recalculates the font metrics
	* \return void
	*/
	void recalcFontMetrics();

	/**
	* \brief Moves the cursor one character to the right
	* \param bShift Whether to shift the selection
	* \return void
	*/
	void internalCursorRight(bool bShift);

	/**
	* \brief Moves the cursor one character to the left
	* \param bShift Whether to shift the selection
	* \return void
	*/
	void internalCursorLeft(bool bShift);

	/**
	* \brief Returns true is there are some action in the undo stack
	* \return bool
	*/
	inline bool isUndoAvailable() const { return !m_bReadOnly && undoState; }

	/**
	* \brief Returns true is there are some action in the redo stack
	* \return bool
	*/
	inline bool isRedoAvailable() const { return !m_bReadOnly && undoState < (int)history.size(); }

	/**
	* \brief Inserts an action separator in the undo stack
	* \return void
	*/
	inline void separate() { separator = true; }

	/**
	* \brief Inserts one action in the undo stack
	* \param cmd The command struct representing the action
	* \return void
	*/
	void addCommand(const Command& cmd);

public slots:
	/**
	* \brief Opens the icon popup with icon description
	* \param iId The id of the icon
	* \return void
	*/
	void iconPopupActivated(int iId);

	/**
	* \brief Copies the text to the system clipboard
	*
	* If the system supports the mouse clipboard, the text will be
	* copied there
	* \return void
	*/
	void copyToSelection(bool bDonNotCopyToClipboard = true);

	/**
	* \brief Copies the text to the system clipboard
	* \return void
	*/
	void copyToClipboard();

	/**
	* \brief Removes the selected text
	* \return void
	*/
	void removeSelected();

	/**
	* \brief Cuts the selection from the input line
	* \return void
	*/
	void cut();

	/**
	* \brief Undo the last action
	* \param iUntil Undo all actions up to the one with id iUntil
	* \return void
	*/
	void undo(int iUntil = -1);

	/**
	* \brief Redo the last undo-ed action
	* \return void
	*/
	void redo();

	/**
	* \brief Pastes the text to the system clipboard
	* \return void
	*/
	void pasteClipboardWithConfirmation();

	/**
	* \brief Pastes the text to the system clipboard
	*
	* If the system supports the mouse clipboard, the text will be
	* copied there
	* \return void
	*/
	void pasteSelectionWithConfirmation();

	/**
	* \brief Starts a slow paste in the buffer
	* \return void
	*/
	void pasteSlow();

	/**
	* \brief Stops a slow paste in the buffer
	* \return void
	*/
	void stopPasteSlow();

	/**
	* \brief Pastes a file to the buffer
	* \return void
	*/
	void pasteFile();

	/**
	* \brief Selects all text in the input line
	* \return void
	*/
	void selectAll();

	/**
	* \brief Clears the input line
	* \return void
	*/
	void clear();
signals:
	/**
	* \brief Called when the user press escape
	* \return void
	*/
	void escapePressed();

	/**
	* \brief Called when the user press enter/return
	* \return void
	*/
	void enterPressed();
protected:
	virtual void drawContents(QPainter *);
	//virtual void resizeEvent(QResizeEvent * e);
	virtual void timerEvent(QTimerEvent * e);
	virtual void focusInEvent(QFocusEvent * e);
	virtual void focusOutEvent(QFocusEvent * e);
	virtual void keyPressEvent(QKeyEvent * e);
	virtual void keyReleaseEvent(QKeyEvent * e);
	virtual void mousePressEvent(QMouseEvent * e);
	virtual void mouseReleaseEvent(QMouseEvent * e);
	virtual void dragEnterEvent(QDragEnterEvent * e);
	virtual void dropEvent(QDropEvent * e);
	virtual void inputMethodEvent(QInputMethodEvent * e) ;
	virtual void paintEvent(QPaintEvent * e);
};

#endif //_KVI_INPUT_EDITOR_H_
