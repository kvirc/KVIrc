#ifndef _KVI_INPUT_EDITOR_H_
#define _KVI_INPUT_EDITOR_H_
//============================================================================
//
//   File : KviInputEditor.h
//   Creation date : Fri Sep 5 2008 17:26:34 by Elvio Basello
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Elvio Basello (hell at hellvis69 dot netsons dot org)
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
//============================================================================

//
//   This file was originally part of KviInput.h
//

/**
* \file KviInputEditor.h
* \author Elvio Basello
* \brief Input editor
*
* \def KVI_INPUT_MAX_BUFFER_SIZE Default maximum buffer size
* \def KVI_INPUT_MARGIN Default margin
* \def KVI_INPUT_BLINK_TIME Cursor blink time...just don't set it to a value less than 100 if you don't want to be lagged by your cursors :)
* \def KVI_INPUT_DRAG_TIMEOUT Drag scroll speed...(smaller values = faster)
* \def KVI_INPUT_DEF_BACK Default background color
* \def KVI_INPUT_DEF_FORE Default foreground color
* \def ACCEL_KEY Accelerator key
*/

#include "kvi_settings.h"
#include "KviCString.h"
#include "KviWindow.h"

#include <QMenu>
#include <QString>
#include <QWidget>

#include <vector>

class KviUserListView;
class QDragEnterEvent;
class QKeyEvent;
class QFontMetricsF;

#define KVI_INPUT_MAX_BUFFER_SIZE 400
#define KVI_INPUT_XTRAPADDING 1
#define KVI_INPUT_MARGIN 4
#define KVI_INPUT_BLINK_TIME 800
#define KVI_INPUT_DRAG_TIMEOUT 30
#define KVI_INPUT_DEF_BACK 100
#define KVI_INPUT_DEF_FORE 101

#ifndef ACCEL_KEY
#define ACCEL_KEY(k) "\t" + QKeySequence(Qt::ControlModifier | Qt::Key_##k).toString()
#endif

class KviInputEditorSpellCheckerBlock
{
public:
	QString szText;
	int iStart;
	int iLength;
	bool bSpellCheckable;
	bool bCorrect;
};

class KviInputEditorPrivate;

/**
* \class KviInputEditor
* \brief Input editor class
*/
class KVIRC_API KviInputEditor : public QWidget
{
	//Q_PROPERTY(int KviProperty_FocusOwner READ heightHint)
	Q_PROPERTY(int TransparencyCapable READ heightHint)
	Q_OBJECT
	friend class KviTextIconWindow; // it uses insetIconCode(const QString &)
	friend class KviTopicWidget;    // it uses home()
public:
	/**
	* \brief Constructs the input editor object
	* \param pPar The parent widget
	* \param pWnd The KviWindow object
	* \param pView The userlist
	* \return KviInputEditor
	*/
	KviInputEditor(QWidget * pPar, KviWindow * pWnd, KviUserListView * pView = nullptr);

	/**
	* \brief Destroys the Input editor object
	*/
	~KviInputEditor();

protected:
	static int g_iInputFontCharWidth[256];
	static QFontMetricsF * g_pLastFontMetrics;
	static int g_iInputInstances;
	static int g_iCachedHeight;
	QString m_szTextBuffer; // original buffer
	int m_iCursorPosition;
	int m_iSelectionBegin;
	int m_iSelectionEnd;
	int m_iMaxBufferSize;
	bool m_bSpSlowFlag; // Slow paste status flag
	int m_iCursorWidth;

	// members for supporting input methods
	QString m_szIMText;
	int m_iIMStart;
	int m_iIMLength;
	int m_iIMSelectionBegin;
	int m_iIMSelectionLength;
	bool m_bIMComposing;

	bool m_bCursorOn;

	int m_iCursorTimer;
	int m_iDragTimer;

	int m_iSelectionAnchorChar;

	// History stuff
	std::vector<QString> m_History;
	int m_iCurHistoryIdx;

	KviCString m_szSaveTextBuffer;

	// Nick completion
	KviUserListView * m_pUserListView;
	QString m_szLastCompletedNick;
	QString m_szLastCompletionBuffer;
	int m_iLastCompletionCursorPosition;
	bool m_bLastCompletionFinished;

	bool m_bUpdatesEnabled;
	KviCString m_szAltKeyCode;
	KviWindow * m_pKviWindow;
	QWidget * m_pInputParent;
	QMenu * m_pIconMenu;
	QMenu m_SpellCheckerPopup;
	bool m_bReadOnly;

	/**
	* \class EditCommand
	* \brief Holds the command used in the {un,re}do operations
	*/
	class EditCommand
	{
	public:
		/**
		* \enum Type
		* \brief Defines the type of the command
		*/
		enum Type
		{
			InsertText, /**< Text insertion. Has text, position previous to the insert and length of the insert */
			RemoveText  /**< Text removal, Has text, position previous to the removal and length of the removed data */
		};

	private:
		/**
		* \var m_eType
		* \brief The type of the command
		*/
		Type m_eType;

		/**
		* \var m_szText
		* \brief The text of the command
		*/
		QString m_szText;

		/**
		* \var m_iStartPosition
		* \brief The start position
		*/
		int m_iStartPosition;

	public:
		/**
		* \brief Constructs the command object
		* \param eType The type of the command
		* \param szText The text of the command
		* \param iStartPosition The start position of the command
		* \return EditCommand
		*/
		EditCommand(Type eType, const QString & szText, int iStartPosition)
		    : m_eType(eType), m_szText(szText), m_iStartPosition(iStartPosition)
		{
		}

		/**
		* \brief Sets the start position
		* \param iStartPosition The position
		* \return void
		*/
		void setStartPosition(int iStartPosition) { m_iStartPosition = iStartPosition; }

		/**
		* \brief Returns the start position
		* \return int
		*/
		int startPosition() const { return m_iStartPosition; }

		/**
		* \brief Sets the text of the command
		* \param szText The text to set
		*/
		void setText(const QString & szText) { m_szText = szText; }

		/**
		* \brief Returns the text of the command
		* \return const QString &
		*/
		const QString & text() const { return m_szText; }

		/**
		* \brief Sets the type of the command
		* \param eType The type of the command
		* \return void
		*/
		void setType(const Type & eType) { m_eType = eType; }

		/**
		* \brief Returns the type of the command
		* \return const Type &
		*/
		const Type & type() const { return m_eType; }
	};

	/**
	* \var m_UndoStack
	* \brief The undo stack.
	*
	* Contains owned pointers and has autodelete set to true. The most recent command
	* is at the end. Null when no undo is available.
	*/
	std::vector<EditCommand *> m_UndoStack;

	/**
	* \var m_RedoStack
	* \brief The redo stack.
	*
	* Contains owned pointers and has autodelete set to true. The most recently undone
	* command is at the end. Null when no redo is available.
	*/
	std::vector<EditCommand *> m_RedoStack;

	KviInputEditorPrivate * m_p;

public:
	/**
	* \brief Returns the height of the editor
	* \return int
	*/
	int heightHint() const;

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
	QString text() const { return m_szTextBuffer; }

	/**
	* \brief Returns the text before the cursor
	*/
	QString textBeforeCursor() const;

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
	void applyOptions(bool bRefreshCachedMetrics = true);

	/**
	* \brief Sets the maximum buffer size
	* \param iSize The size of the buffer to set
	* \return void
	*/
	void setMaxBufferSize(int iSize) { m_iMaxBufferSize = iSize; }

	/**
	* \brief Sets the read only state for the input line
	* \param bReadOnly Whether to set the read only state
	* \return void
	*/
	void setReadOnly(bool bReadOnly) { m_bReadOnly = bReadOnly; }

	/**
	* \brief Returns true if the input line is in read only state
	* \return bool
	*/
	bool isReadOnly() const { return m_bReadOnly; }

	/**
	* \brief Clears the undo stack.
	*/
	void clearUndoStack();

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
	* \brief Gets the substitute character for control codes
	* \param uControlCode The control code inserted
	* \return QChar
	*/
	static QChar getSubstituteChar(unsigned short uControlCode);

	/**
	* \brief Makes sure that the cursor is visible.
	*/
	void ensureCursorVisible();

	/**
	* \brief Repaints the input line with the visible cursor
	* \return void
	*/
	void repaintWithCursorOn();

	void clearSelection();

	/**
	* \brief Returns the current character from a given position
	* \param iXPos The position to grab
	* \return int
	*/
	int charIndexFromXPosition(qreal fXPos);

	/**
	* \brief Returns the current position from a given character
	* \param iChIdx Th index of the character
	* \return int
	*/
	qreal xPositionFromCharIndex(int iChIdx);

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
	* \brief Submits input
	*
	* Triggered when the user press return
	* \param bRepaint Whether to repain the input line
	* \return void
	*/
	void returnPressed(bool bRepaint = true);

	/**
	* \brief Adds the text to the history
	*
	* Called when the user is done editing the current
	* input line (either by submitting or clearing it).
	* Adds the content (if any) to the history,
	* and hides helper windows.
	* \return void
	*/
	void finishInput();

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
	* \brief Completes the nickname specified by szWord (inserts the completed text to the input line)
	* \param bAddMask Whether to complete with the mask of the nickname
	* \param szWord The nickname to complete
	* \param bFirstWordInLine Whether the word to complete is the first word in the text line (so the suffix from option panel is added)
	* \param bInCommand Whether the completion happens inside a kvs command (and thus the completed word should be kvs-escaped)
	* \return void
	*/
	void standardNickCompletion(bool bAddMask, QString & szWord, bool bFirstWordInLine, bool bInCommand);

	/**
	* \brief Internal helper for standardNickCompletion()
	* \param szReplacedWord The word replaced before the cursor
	* \param szCompletedText The text to be completed
	* \param bFirstWordInLine Whether the word to complete is the first word in the text line (so the suffix from option panel is added)
	* \param bInCommand Whether the completion happens inside a kvs command (and thus the completed word should be kvs-escaped)
	* \return void
	*/
	void standardNickCompletionInsertCompletedText(const QString & szReplacedWord, QString szCompletedText, bool bFirstWordInLine, bool bInCommand);

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
	bool isUndoAvailable() const { return !m_bReadOnly && !m_UndoStack.empty(); }

	/**
	* \brief Returns true is there are some action in the redo stack
	* \return bool
	*/
	bool isRedoAvailable() const { return !m_bReadOnly && !m_RedoStack.empty(); }

	/**
	* \brief Inserts one action in the undo stack
	* \param pCommand The command representing the action
	* \return void
	*/
	void addUndo(EditCommand * pCommand);

	/**
	* \brief Returns the current input editor font metrics (globally shared)
	* \param font The current input editor font
	* \return QFontMetricsF *
	*/
	QFontMetricsF * getLastFontMetrics(const QFont & font);

public slots:
	/**
	* \brief Opens the icon popup with icon description
	* \param iId The id of the icon
	* \return void
	*/
	void iconPopupActivated(QAction * pAction);

	/**
	* \brief Copies the text to the system clipboard
	*
	* If the system supports the mouse clipboard, the text will be
	* copied there
	* \param bDonNotCopyToClipboard Whether to copy the text to the system clipboard
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
	* \return void
	*/
	void undo();

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
private slots:
	/**
	* \brief Opens the history popup
	* \return void
	*/
	void openHistory();

	/**
	* \brief Toggles the multiline editor
	* \return void
	*/
	void toggleMultiLineEditor();

	/**
	* \brief Moves to the previous character
	* \return void
	*/
	void previousChar();

	/**
	* \brief Moves to the next character
	* \return void
	*/
	void nextChar();

	/**
	* \brief Selects up to the previous character
	* \return void
	*/
	void previousCharSelection();

	/**
	* \brief Selects up to the next character
	* \return void
	*/
	void nextCharSelection();

	/**
	* \brief Moves to the beginning of the previous word
	* \return void
	*/
	void previousWord();

	/**
	* \brief Moves to the end of the next word
	* \return void
	*/
	void nextWord();

	/**
	* \brief Selects up to the beginning of the previous word
	* \return void
	*/
	void previousWordSelection();

	/**
	* \brief Selects up to the end of the next word
	* \return void
	*/
	void nextWordSelection();

	/**
	* \brief Inserts bold control character
	* \return void
	*/
	void insertBold();

	/**
	* \brief Inserts italic control character
	* \return void
	*/
	void insertItalic();
	/**
	* \brief Inserts reset control character
	* \return void
	*/
	void insertReset();

	/**
	* \brief Inserts underline control character
	* \return void
	*/
	void insertUnderline();

	/**
	* \brief Inserts reverse control character
	* \return void
	*/
	void insertReverse();

	/**
	* \brief Inserts non-crypt (plain text) control character
	* \return void
	*/
	void insertPlainText();

	/**
	* \brief Inserts icon control character
	* \return void
	*/
	void insertIcon();

	/**
	* \brief Inserts color control character
	* \return void
	*/
	void insertColor();

	/**
	* \brief Copies to clipboard
	* \return void
	*/
	void copyInternal();

	/**
	* \brief Cuts to clipboard
	* \return void
	*/
	void cutInternal();

	/**
	* \brief Pastes clipboard contents
	* \return void
	*/
	void pasteInternal();

	/**
	* \brief Undoes last action
	* \return void
	*/
	void undoInternal();

	/**
	* \brief Redoes last action
	* \return void
	*/
	void redoInternal();

	/**
	* \brief Selects all
	* \return void
	*/
	void selectAllInternal();

	/**
	* \brief Deletes previous word
	* \return void
	*/
	void deletePreviousWord();

	/**
	* \brief Deletes next word
	* \return void
	*/
	void deleteNextWord();

	/**
	* \brief Scrolls the output window up one line
	* \return void
	*/
	void previousLine();

	/**
	* \brief Scrolls the output window down one line
	* \return void
	*/
	void nextLine();

	/**
	* \brief Scrolls the output window up one page
	* \return void
	*/
	void previousPage();

	/**
	* \brief Scrolls the output window down one page
	* \return void
	*/
	void nextPage();

	/**
	* \brief Scrolls the output window to the top
	* \return void
	*/
	void scrollTop();

	/**
	* \brief Scrolls the output window to the bottom
	* \return void
	*/
	void scrollBottom();

	/**
	* \brief Opens the search window
	* \return void
	*/
	void search();

	/**
	* \brief Scrolls the ircview to the last read line
	* \return void
	*/
	void scrollToLastReadLine();

	/**
	* \brief Sends plain text, ignoring initial token starting with / (a command)
	* \return void
	*/
	void sendPlain();

	/**
	* \brief Sends the message assuming it's kvs code
	* \return void
	*/
	void sendKvs();

	/**
	* \brief Moves the cursor at the beginning
	* \return void
	*/
	void homeInternal();

	/**
	* \brief Moves the cursor at the end
	* \return void
	*/
	void endInternal();

	/**
	* \brief Selects until the beginning
	* \return void
	*/
	void homeInternalSelection();

	/**
	* \brief Selects until the end
	* \return void
	*/
	void endInternalSelection();

	/**
	* \brief Moves backward in the command history and in the history popup
	* \return void
	*/
	void historyPrev();

	/**
	* \brief Moves forward in the command history and in the history popup
	* \return void
	*/
	void historyNext();

	/**
	* \brief Run when return/enter key is pressed
	* \return void
	*/
	void returnHit();

	/**
	* \brief Run when backspace key is pressed
	* \return void
	*/
	void backspaceHit();

	/**
	* \brief Run when delete key is pressed
	* \return void
	*/
	void deleteHit();

	/**
	* \brief Run when escape key is pressed
	* \return void
	*/
	void escapeHit();

	/**
	* \brief Toggles the commandline's KVS/User-friendly mode
	* \return void
	*/
	void toggleCommandMode();

	/**
	* \brief Does nothing but grabs some keys combinations
	* \warning This is an hack. The keys combinations grabbed by this function write a non-printable chars which cause some troubles, eg duplication of chars opening/closing more times the multiline
	* \return void
	*/
	void dummy();

	/**
	* \brief Increase the ircview font size
	* \return void
	*/
	void zoomIn();

	/**
	* \brief Decrease the ircview font size
	* \return void
	*/
	void zoomOut();

	/**
	* \brief Restore the ircview font (and its size)
	* \return void
	*/
	void zoomDefault();

	/**
	* Shows the text icon window at cursor position
	*/
	void popupTextIconWindow();

	/**
	* Shows the contextual popup menu at the global position pos
	*/
	void showContextPopup(const QPoint & pos);

	/**
	* Shows the context popup menu at the current cursor position
	*/
	void showContextPopupHere();

	/**
	* Show the spell checker corrections popup.
	*/
	void showSpellCheckerCorrectionsPopup();

	/**
	* Handles spellchecker suggestions popup
	*/
	void spellCheckerPopupCorrectionActionTriggered();

	/**
	* Adds line to input history
	*/
	void addToHistory(const QString & szString);

protected:
	void insertIconCode(const QString & szCode);
	void completionEscapeUnsafeToken(QString & szToken);
	void installShortcuts();
	virtual void drawContents(QPainter *);
	//void resizeEvent(QResizeEvent * e) override;
	void timerEvent(QTimerEvent * e) override;
	void focusInEvent(QFocusEvent * e) override;
	void focusOutEvent(QFocusEvent * e) override;
	void keyPressEvent(QKeyEvent * e) override;
	void keyReleaseEvent(QKeyEvent * e) override;
	void mousePressEvent(QMouseEvent * e) override;
	void mouseReleaseEvent(QMouseEvent * e) override;
	void mouseDoubleClickEvent(QMouseEvent * e) override;
	void dragEnterEvent(QDragEnterEvent * e) override;
	void dropEvent(QDropEvent * e) override;
	void inputMethodEvent(QInputMethodEvent * e) override;
	QVariant inputMethodQuery(Qt::InputMethodQuery query) const override;
	void paintEvent(QPaintEvent * e) override;
	bool checkWordSpelling(const QString & szWord);
	void splitTextIntoSpellCheckerBlocks(const QString & szText, std::vector<KviInputEditorSpellCheckerBlock *> & lBuffer);
	KviInputEditorSpellCheckerBlock * findSpellCheckerBlockAtCursor(std::vector<KviInputEditorSpellCheckerBlock *> & lBlocks);
	void fillSpellCheckerCorrectionsPopup();

	void rebuildTextBlocks();

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
};

#endif //_KVI_INPUT_EDITOR_H_
