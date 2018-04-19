#ifndef _KVI_SHORTCUT_CLASSFILE_H_
#error "Do not include kvi_shortcuts.h config file directly: include KviShortcut.h class header instead."
#endif
#ifndef _KVI_SHORTCUTS_H_
#define _KVI_SHORTCUTS_H_
//=============================================================================
//
//   File : kvi_shortcuts.h
//   Creation date : Fri Dec 11 2009 16:24:46 CEST by Elvio Basello
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2009 Elvio Basello (hell at hellvis69 dot netsons dot org)
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

/**
* \file kvi_shortcuts.h
* \author Elvio Basello
* \brief This file contains all the keyboard shortcuts used by KVIrc
*/

/**
* \brief Keyboard shortcuts
*
* \def KVI_SHORTCUTS_AWAY Go away/back
* \def KVI_SHORTCUTS_CONNECT Connect/disconnect current IRC context
* \def KVI_SHORTCUTS_CONTEXT Open new context
* \def KVI_SHORTCUTS_EXEC Open "Execute Script" dialog
* \def KVI_SHORTCUTS_HELP Help Index (Window)
* \def KVI_SHORTCUTS_IDENTITY Open Identity options
* \def KVI_SHORTCUTS_JOIN Open "Join Channels" dialog
* \def KVI_SHORTCUTS_OPTIONS Open "Options" dialog
* \def KVI_SHORTCUTS_SERVERS Open "Servers" dialog
* \def KVI_SHORTCUTS_THEME Open "Theme Options" dialog
* \def KVI_SHORTCUTS_TOGGLE_MENU_BAR Toggle visibility of the menu bar
* \def KVI_SHORTCUTS_USERS Open "Registered Users" dialog
* \def KVI_SHORTCUTS_MANAGE_THEMES Open "Manage Theme" dialog
* \def KVI_SHORTCUTS_MANAGE_ADDONS Open "Manage Theme" dialog
* \def KVI_SHORTCUTS_QUIT Quit KVIrc
*
* \def KVI_SHORTCUTS_INPUT_ALT Alt key
* \def KVI_SHORTCUTS_INPUT_ALT_2 Meta key
* \def KVI_SHORTCUTS_INPUT_BACKSPACE Backspace key
* \def KVI_SHORTCUTS_INPUT_BACKSPACE_2 Shift + Backspace key
* \def KVI_SHORTCUTS_INPUT_BOLD Insert the 'bold' mIRC text control character
* \def KVI_SHORTCUTS_INPUT_ITALIC Insert the 'italic' mIRC text control character
* \def KVI_SHORTCUTS_INPUT_COLOR Insert the 'color' mIRC text control character
* \def KVI_SHORTCUTS_INPUT_COMMANDLINE Toggle the commandline's KVS/User-friendly mode
* \def KVI_SHORTCUTS_INPUT_COPY Copy the selected text to clipboard
* \def KVI_SHORTCUTS_INPUT_CUT Cut the selected text and copy the text to clipboard
* \def KVI_SHORTCUTS_INPUT_DELETE Delete key
* \def KVI_SHORTCUTS_INPUT_DELETE_PREV_WORD Delete previous word
* \def KVI_SHORTCUTS_INPUT_DELETE_NEXT_WORD Delete next word
* \def KVI_SHORTCUTS_INPUT_END Move the cursor at the end
* \def KVI_SHORTCUTS_INPUT_END_SELECT Select until the end
* \def KVI_SHORTCUTS_INPUT_ESCAPE Escape key
* \def KVI_SHORTCUTS_INPUT_HISTORY Open the history popup
* \def KVI_SHORTCUTS_INPUT_HISTORY_NEXT Move forward in the command history
* \def KVI_SHORTCUTS_INPUT_HISTORY_PREV Move backward in the command history
* \def KVI_SHORTCUTS_INPUT_HOME Move the cursor at the beginning
* \def KVI_SHORTCUTS_INPUT_HOME_SELECT Select until the beginning
* \def KVI_SHORTCUTS_INPUT_ICON Insert the 'icon' control code and pops up the icon list box
* \def KVI_SHORTCUTS_INPUT_MULTILINE Show/hide the multiline editor
* \def KVI_SHORTCUTS_INPUT_MULTILINE_2 Show/hide the multiline editor
* \def KVI_SHORTCUTS_INPUT_NEXT_CHAR Move the cursor to the right
* \def KVI_SHORTCUTS_INPUT_NEXT_CHAR_SELECT Move the selection to the right
* \def KVI_SHORTCUTS_INPUT_NEXT_WORD Move to the end of the next word
* \def KVI_SHORTCUTS_INPUT_NEXT_WORD_SELECT Select up to the end of the next word
* \def KVI_SHORTCUTS_INPUT_PASTE Paste the clipboard contents (same as middle mouse click)
* \def KVI_SHORTCUTS_INPUT_PLAINTEXT Insert the 'non-crypt' (plain text) KVIrc control character used to disable encryption of the current text line
* \def KVI_SHORTCUTS_INPUT_PREV_CHAR Move the cursor to the left
* \def KVI_SHORTCUTS_INPUT_PREV_CHAR_SELECT Move the selection to the left
* \def KVI_SHORTCUTS_INPUT_PREV_WORD Move to the beginning of the previous word
* \def KVI_SHORTCUTS_INPUT_PREV_WORD_SELECT Select up to the beginning of the previous word
* \def KVI_SHORTCUTS_INPUT_REDO Redo last action
* \def KVI_SHORTCUTS_INPUT_RESET Insert Reset control character
* \def KVI_SHORTCUTS_INPUT_RETURN Return key
* \def KVI_SHORTCUTS_INPUT_RETURN_2 Enter key
* \def KVI_SHORTCUTS_INPUT_REVERSE Insert the 'reverse' mIRC text control character
* \def KVI_SHORTCUTS_INPUT_SELECT_ALL Select all
* \def KVI_SHORTCUTS_INPUT_SEND_PLAIN Send message as plain text, ignoring / commands
* \def KVI_SHORTCUTS_INPUT_SEND_PLAIN_2 Send message as plain text, ignoring / commands
* \def KVI_SHORTCUTS_INPUT_SEND_KVS Send message as kvs
* \def KVI_SHORTCUTS_INPUT_SEND_KVS_2 Send message as kvs
* \def KVI_SHORTCUTS_INPUT_UNDERLINE Insert the 'underline' mIRC text control character
* \def KVI_SHORTCUTS_INPUT_UNDO Undo last action
* \def KVI_SHORTCUTS_INPUT_DUMMY Dummy combo
* \def KVI_SHORTCUTS_INPUT_DUMMY_2 Dummy combo
* \def KVI_SHORTCUTS_INPUT_CORRECT_SPELLING Correct spelling of the word near the cursor
* \def KVI_SHORTCUTS_INPUT_MENU Open the context menu
*
* \def KVI_SHORTCUTS_EDITORS_ACTION Open Action editor
* \def KVI_SHORTCUTS_EDITORS_ALIAS Open Alias editor
* \def KVI_SHORTCUTS_EDITORS_CLASS Open Class editor
* \def KVI_SHORTCUTS_EDITORS_EVENT Open Event editor
* \def KVI_SHORTCUTS_EDITORS_POPUP Open Popup editor
* \def KVI_SHORTCUTS_EDITORS_RAW Open Raw events editor
* \def KVI_SHORTCUTS_EDITORS_TESTER Open Script tester
* \def KVI_SHORTCUTS_EDITORS_TOOLBAR Open "Manage Toolbars" dialog
*
* \def KVI_SHORTCUTS_WIN_NEXT Next window
* \def KVI_SHORTCUTS_WIN_NEXT_CONTEXT Next window in the same IRC context
* \def KVI_SHORTCUTS_WIN_NEXT_HIGHLIGHT Next highlighted window
* \def KVI_SHORTCUTS_WIN_NEXT_LINE Scroll the output window down one line
* \def KVI_SHORTCUTS_WIN_NEXT_PAGE Scroll the output window down one page
* \def KVI_SHORTCUTS_WIN_PREV Previous window
* \def KVI_SHORTCUTS_WIN_PREV_CONTEXT Previous window in the same IRC context
* \def KVI_SHORTCUTS_WIN_PREV_HIGHLIGHT Previous highlighted window
* \def KVI_SHORTCUTS_WIN_PREV_LINE Scroll the output window up one line
* \def KVI_SHORTCUTS_WIN_PREV_PAGE Scroll the output window up one page
* \def KVI_SHORTCUTS_WIN_HOME Scroll the output window to the top
* \def KVI_SHORTCUTS_WIN_END Scroll the output window to the bottom
* \def KVI_SHORTCUTS_WIN_SCROLL_TO_LAST_READ_LINE Scroll to the last read line
* \def KVI_SHORTCUTS_WIN_SEARCH Open "Find Text" dialog
* \def KVI_SHORTCUTS_WIN_ZOOM_IN Increase font size
* \def KVI_SHORTCUTS_WIN_ZOOM_OUT Decrease font size
*/

//
// Please keep this list ordered by shortcut key and not by name :)
//

#define KVI_SHORTCUTS_HELP QKeySequence::HelpContents          // F1
#define KVI_SHORTCUTS_INPUT_SELECT_ALL QKeySequence::SelectAll // Ctrl+A
#define KVI_SHORTCUTS_INPUT_BOLD QKeySequence::Bold            // Ctrl+B
#define KVI_SHORTCUTS_INPUT_COPY QKeySequence::Copy            // Ctrl+C
#define KVI_SHORTCUTS_WIN_SEARCH QKeySequence::Find            // Ctrl+F
#define KVI_SHORTCUTS_INPUT_ITALIC QKeySequence::Italic        // Ctrl+I
#define KVI_SHORTCUTS_JOIN "Ctrl+J"                            // Ctrl+J
#define KVI_SHORTCUTS_INPUT_COLOR "Ctrl+K"                     // Ctrl+K
#define KVI_SHORTCUTS_WIN_SCROLL_TO_LAST_READ_LINE "Ctrl+L"    // Ctrl+L
#define KVI_SHORTCUTS_TOGGLE_MENU_BAR "Ctrl+M"                 // Ctrl+M
#define KVI_SHORTCUTS_CONTEXT QKeySequence::New                // Ctrl+N
#define KVI_SHORTCUTS_INPUT_RESET "Ctrl+O"                     // Ctrl+O
#define KVI_SHORTCUTS_INPUT_PLAINTEXT "Ctrl+P"                 // Ctrl+P
#ifdef COMPILE_ON_MAC                                          // ------
#define KVI_SHORTCUTS_QUIT QString()                           // Qstring()
#else                                                          // ------
#define KVI_SHORTCUTS_QUIT "Ctrl+Q"                            // Ctrl+Q
#endif                                                         // ------
#define KVI_SHORTCUTS_INPUT_REVERSE "Ctrl+R"                   // Ctrl+R
#define KVI_SHORTCUTS_SERVERS "Ctrl+S"                         // Ctrl+S
#define KVI_SHORTCUTS_TOGGLE_TREE_LIST "Ctrl+T"                // Ctrl+T
#define KVI_SHORTCUTS_INPUT_UNDERLINE QKeySequence::Underline  // Ctrl+U
#define KVI_SHORTCUTS_INPUT_PASTE QKeySequence::Paste          // Ctrl+V
#define KVI_SHORTCUTS_WIN_CLOSE "Ctrl+W"                       // Ctrl+W QKeySequence::Close seems to be problematic
#define KVI_SHORTCUTS_INPUT_CUT QKeySequence::Cut              // Ctrl+X
#define KVI_SHORTCUTS_INPUT_COMMANDLINE "Ctrl+Y"               // Ctrl+Y
#define KVI_SHORTCUTS_INPUT_UNDO QKeySequence::Undo            // Ctrl+Z

#define KVI_SHORTCUTS_INPUT_ICON "Alt+E"                       // Alt+E
#define KVI_SHORTCUTS_USERS "Alt+U"                            // Alt+U

#define KVI_SHORTCUTS_AWAY "Ctrl+Shift+A"                      // Ctrl+Shift+A
#define KVI_SHORTCUTS_EDITORS_TOOLBAR "Ctrl+Shift+B"           // Ctrl+Shift+B
#define KVI_SHORTCUTS_CONNECT "Ctrl+Shift+C"                   // Ctrl+Shift+C
#define KVI_SHORTCUTS_EDITORS_CLASS "Ctrl+Shift+D"             // Ctrl+Shift+D
#define KVI_SHORTCUTS_EDITORS_EVENT "Ctrl+Shift+E"             // Ctrl+Shift+E
#define KVI_SHORTCUTS_IDENTITY "Ctrl+Shift+I"                  // Ctrl+Shift+I
#define KVI_SHORTCUTS_EDITORS_ALIAS "Ctrl+Shift+L"             // Ctrl+Shift+L
#define KVI_SHORTCUTS_MANAGE_THEMES "Ctrl+Shift+M"             // Ctrl+Shift+M
#define KVI_SHORTCUTS_MANAGE_ADDONS "Ctrl+Shift+N"             // Ctrl+Shift+N
#define KVI_SHORTCUTS_OPTIONS "Ctrl+Shift+O"                   // Ctrl+Shift+O
#define KVI_SHORTCUTS_EDITORS_POPUP "Ctrl+Shift+P"             // Ctrl+Shift+P
#define KVI_SHORTCUTS_EDITORS_ACTION "Ctrl+Shift+Q"            // Ctrl+Shift+Q
#define KVI_SHORTCUTS_EDITORS_RAW "Ctrl+Shift+R"               // Ctrl+Shift+R
#define KVI_SHORTCUTS_EDITORS_TESTER "Ctrl+Shift+S"            // Ctrl+Shift+S
#define KVI_SHORTCUTS_THEME "Ctrl+Shift+T"                     // Ctrl+Shift+T
#define KVI_SHORTCUTS_EXEC "Ctrl+Shift+X"                      // Ctrl+Shift+X
#define KVI_SHORTCUTS_INPUT_REDO QKeySequence::Redo            // Ctrl+Shift+Z

#define KVI_SHORTCUTS_WIN_PREV Qt::AltModifier + Qt::Key_Up                               // Qt::AltModifier + Qt::Key_Up
#define KVI_SHORTCUTS_WIN_NEXT Qt::AltModifier + Qt::Key_Down                             // Qt::AltModifier + Qt::Key_Down
#define KVI_SHORTCUTS_WIN_NEXT_TAB Qt::ControlModifier + Qt::Key_Tab                      // Ctrl+Tab
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)                              // --------------
#define KVI_SHORTCUTS_WIN_PREV_TAB Qt::ControlModifier + Qt::ShiftModifier + Qt::Key_Tab  // Ctrl+Shift+Tab
#else                                                                                     // --------------
#define KVI_SHORTCUTS_WIN_PREV_TAB Qt::ControlModifier + Qt::Key_Backtab                  // Ctrl+Shift+Tab
#endif                                                                                    // --------------
#define KVI_SHORTCUTS_WIN_PREV_CONTEXT Qt::AltModifier + Qt::ShiftModifier + Qt::Key_Up   // Qt::AltModifier + Qt::ShiftModifier + Qt::Key_Up
#define KVI_SHORTCUTS_WIN_NEXT_CONTEXT Qt::AltModifier + Qt::ShiftModifier + Qt::Key_Down // Qt::AltModifier + Qt::ShiftModifier + Qt::Key_Down
#define KVI_SHORTCUTS_WIN_PREV_HIGHLIGHT Qt::AltModifier + Qt::Key_PageUp                 // Qt::AltModifier + Qt::Key_PageUp
#define KVI_SHORTCUTS_WIN_NEXT_HIGHLIGHT Qt::AltModifier + Qt::Key_PageDown               // Qt::AltModifier + Qt::Key_PageDown
#define KVI_SHORTCUTS_WIN_PREV_PAGE QKeySequence::MoveToPreviousPage                      // Qt::Key_PageUp
#define KVI_SHORTCUTS_WIN_NEXT_PAGE QKeySequence::MoveToNextPage                          // Qt::Key_PageDown
#define KVI_SHORTCUTS_WIN_HOME QKeySequence::MoveToStartOfDocument                        // Qt::ControlModifier + Qt::Key_Home
#define KVI_SHORTCUTS_WIN_END QKeySequence::MoveToEndOfDocument                           // Qt::ControlModifier + Qt::Key_End
#define KVI_SHORTCUTS_WIN_PREV_LINE Qt::ShiftModifier + Qt::Key_PageUp                    // Qt::ShiftModifier + Qt::Key_PageUp
#define KVI_SHORTCUTS_WIN_NEXT_LINE Qt::ShiftModifier + Qt::Key_PageDown                  // Qt::ShiftModifier + Qt::Key_PageDown
#define KVI_SHORTCUTS_INPUT_PREV_WORD QKeySequence::MoveToPreviousWord                    // Qt::ControlModifier + Qt::Key_Left
#define KVI_SHORTCUTS_INPUT_NEXT_WORD QKeySequence::MoveToNextWord                        // Qt::ControlModifier + Qt::Key_Right
#define KVI_SHORTCUTS_INPUT_PREV_WORD_SELECT QKeySequence::SelectPreviousWord             // Qt::ControlModifier + Qt::ShiftModifier + Qt::Key_Left
#define KVI_SHORTCUTS_INPUT_NEXT_WORD_SELECT QKeySequence::SelectNextWord                 // Qt::ControlModifier + Qt::ShiftModifier + Qt::Key_Right
//
//Ctrl+<digit>: Script accelerators (see OnAccelKeyPressed)                               // --------------
//F2-F12, Shift+(F1-F12): Script accelerators (see OnAccelKeyPressed)                     // --------------
//
#define KVI_SHORTCUTS_INPUT_HISTORY Qt::ControlModifier + Qt::Key_PageUp                  // Qt::ControlModifier + Qt::Key_PageUp
#define KVI_SHORTCUTS_INPUT_PREV_CHAR QKeySequence::MoveToPreviousChar                    // Qt::Key_Left
#define KVI_SHORTCUTS_INPUT_NEXT_CHAR QKeySequence::MoveToNextChar                        // Qt::Key_Right
#define KVI_SHORTCUTS_INPUT_PREV_CHAR_SELECT QKeySequence::SelectPreviousChar             // Qt::ShiftModifier + Qt::Key_Left
#define KVI_SHORTCUTS_INPUT_NEXT_CHAR_SELECT QKeySequence::SelectNextChar                 // Qt::ShiftModifier + Qt::Key_Right
#define KVI_SHORTCUTS_INPUT_DELETE_PREV_WORD QKeySequence::DeleteStartOfWord              // Qt::ControlModifier + Qt::Key_Backspace
#define KVI_SHORTCUTS_INPUT_DELETE_NEXT_WORD QKeySequence::DeleteEndOfWord                // Qt::ControlModifier + Qt::Key_Delete
#define KVI_SHORTCUTS_INPUT_MULTILINE Qt::AltModifier + Qt::Key_Return                    // Qt::AltModifier + Qt::Key_Return
#define KVI_SHORTCUTS_INPUT_MULTILINE_2 Qt::AltModifier + Qt::Key_Enter                   // Qt::AltModifier + Qt::Key_Enter
#define KVI_SHORTCUTS_INPUT_SEND_PLAIN Qt::ControlModifier + Qt::Key_Return               // Qt::ControlModifier + Qt::Key_Return
#define KVI_SHORTCUTS_INPUT_SEND_PLAIN_2 Qt::ControlModifier + Qt::Key_Enter              // Qt::ControlModifier + Qt::Key_Enter
#define KVI_SHORTCUTS_INPUT_SEND_KVS Qt::ShiftModifier + Qt::Key_Return                   // Qt::ShiftModifier + Qt::Key_Return
#define KVI_SHORTCUTS_INPUT_SEND_KVS_2 Qt::ShiftModifier + Qt::Key_Enter                  // Qt::ShiftModifier + Qt::Key_Enter
#define KVI_SHORTCUTS_INPUT_HOME QKeySequence::MoveToStartOfLine                          // Qt::Key_Home
#define KVI_SHORTCUTS_INPUT_END QKeySequence::MoveToEndOfLine                             // Qt::Key_End
#define KVI_SHORTCUTS_INPUT_HOME_SELECT QKeySequence::SelectStartOfLine                   // Qt::ShiftModifier + Qt::Key_Home
#define KVI_SHORTCUTS_INPUT_END_SELECT QKeySequence::SelectEndOfLine                      // Qt::ShiftModifier + Qt::Key_End
#define KVI_SHORTCUTS_INPUT_HISTORY_PREV QKeySequence::MoveToPreviousLine                 // Qt::Key_Up
#define KVI_SHORTCUTS_INPUT_HISTORY_NEXT QKeySequence::MoveToNextLine                     // Qt::Key_Down
#define KVI_SHORTCUTS_INPUT_ESCAPE Qt::Key_Escape                                         // Qt::Key_Escape
#define KVI_SHORTCUTS_INPUT_DUMMY Qt::ShiftModifier + Qt::Key_Escape                      // Qt::ShiftModifier + Qt::Key_Escape
#define KVI_SHORTCUTS_WIN_ZOOM_IN QKeySequence::ZoomIn                                    // Ctrl++
#define KVI_SHORTCUTS_WIN_ZOOM_OUT QKeySequence::ZoomOut                                  // Ctrl+-
#define KVI_SHORTCUTS_WIN_ZOOM_DEFAULT Qt::ControlModifier + Qt::Key_0                    // Qt::ControlModifier + Qt::Key_0
#define KVI_SHORTCUTS_INPUT_CORRECT_SPELLING Qt::ControlModifier + Qt::Key_G              // Qt::ControlModifier + Qt::Key_G
#define KVI_SHORTCUTS_INPUT_MENU Qt::Key_Menu                                             // Qt::Key_Menu

/*
	@doc: keyboard
	@type:
		generic
	@title:
		Keyboard shortcuts
	@keyterms:
		Keyboard shortcuts
	@short:
		The list of the common keyboard shortcuts
	@body:
		This list contains the key sequences associated to some common actions (e.g. selecting text). Since KVIrc 4.2 the shortcuts are dynamic:
		[ul]
		[li]more shortcuts can trigger the same actions: Cut is both Ctrl+X and Shift+Del[/li]
		[li]shortcuts can be adapted to the OS: Help is F1 under Windows and Linux, ⌘? on macOS[/li]
		[/ul]
		A full list of available combinations is available at the [anchorlink:http://doc.qt.nokia.com/latest/qkeysequence.html#standard-shortcuts] Qt documentation[/anchorlink]
		[big]All the standard keys alphabetically ordered[/big]
		[b]F1:[/b] Help Index (Window)[br]
		[b]Ctrl+A:[/b] Select all[br]
		[b]Ctrl+B:[/b] Insert Bold control character[br]
		[b]Ctrl+C:[/b] Copy to clipboard[br]
		[b]Ctrl+F:[/b] Open [i]Find Text[/i] dialog[br]
		[b]Ctrl+G:[/b] Activate spell-checker[br]
		[b]Ctrl+I:[/b] Insert italic control character[br]
		[b]Ctrl+L:[/b] Scroll to the [i]last read[/i] line marker[br]
		[b]Ctrl+J:[/b] Open [i]Join Channels[/i] dialog[br]
		[b]Ctrl+K:[/b] Open [i]Insert Color[/i] dialog[br]
		[b]Ctrl+L:[/b] Scroll to the last read line[br]
		[b]Ctrl+N:[/b] Open new context[br]
		[b]Ctrl+O:[/b] Insert Reset control character[br]
		[b]Ctrl+P:[/b] Insert [i]Plain text[/i] control character[br]
		[b]Ctrl+Q:[/b] Quit [br]
		[b]Ctrl+R:[/b] Insert Reverse control character[br]
		[b]Ctrl+S:[/b] Open [i]Servers[/i] dialog [br]
		[b]Ctrl+T:[/b] Switch between [i]Tree Window List[/i] and [i]Classic Window List[/i][br]
		[b]Ctrl+U:[/b] Insert Underline control character[br]
		[b]Ctrl+V:[/b] Paste clipboard contents[br]
		[b]Ctrl+W:[/b] Close current window[br]
		[b]Ctrl+X:[/b] Cut to clipboard[br]
		[b]Ctrl+Y:[/b] Toggle the commandline's KVS/User-friendly mode[br]
		[b]Ctrl+Z:[/b] Undo last action[br]
		[b]Ctrl+"+":[/b] Increase font size[br]
		[b]Ctrl+"-":[/b] Decrease font size[br]
		[b]Ctrl+0:[/b] Restore default font (and font size)[br]

		[b]Alt+E:[/b] Open [i]Insert icon[/i] dialog[br]
		[b]Alt+U:[/b] Open [i]Registered users[/i] dialog[br]

		[b]Ctrl+Shift+A:[/b] Go away/back[br]
		[b]Ctrl+Shift+B:[/b] Open [i]Manage Toolbars[/i] dialog[br]
		[b]Ctrl+Shift+C:[/b] Connect/disconnect current irc context[br]
		[b]Ctrl+Shift+D:[/b] Open Class editor[br]
		[b]Ctrl+Shift+E:[/b] Open Event editor[br]
		[b]Ctrl+Shift+I:[/b] Open Identity options[br]
		[b]Ctrl+Shift+L:[/b] Open Alias editor[br]
		[b]Ctrl+Shift+M:[/b] Open [i]Manage Themes[/i] dialog[br]
		[b]Ctrl+Shift+N:[/b] Open [i]Manage Addons[/i] dialog[br]
		[b]Ctrl+Shift+O:[/b] Open [i]Options[/i] dialog[br]
		[b]Ctrl+Shift+P:[/b] Open Popup editor[br]
		[b]Ctrl+Shift+Q:[/b] Open Action editor[br]
		[b]Ctrl+Shift+R:[/b] Open Raw events editor[br]
		[b]Ctrl+Shift+S:[/b] Open Script tester[br]
		[b]Ctrl+Shift+T:[/b] Open [i]Theme Options[/i] dialog[br]
		[b]Ctrl+Shift+X:[/b] Open [i]Execute Script[/i] dialog[br]
		[b]Ctrl+Shift+Z:[/b] Redo last action

		[big]Window handling related keys:[/big]
		[b]Alt+UpArrow:[/b] Previous window[br]
		[b]Alt+DownArrow:[/b] Next window[br]
		[b]Alt+Shift+UpArrow:[/b] Previous window in the same IRC context[br]
		[b]Alt+Shift+DownArrow:[/b] Next window in the same IRC context[br]
		[b]Alt+PageUp:[/b] Previous highlighted window[br]
		[b]Alt+PageDown:[/b] Next highlighted window[br]
		[b]Ctrl+UpArrow:[/b] Maximizes current window[br]
		[b]Ctrl+DownArrow:[/b] Minimizes current window[br]
		[b]PageUp:[/b] Scroll the output window up one page[br]
		[b]PageDown:[/b] Scroll the output window down one page[br]
		[b]Shift+PageUp:[/b] Scroll the output window up one line[br]
		[b]Shift+PageDown:[/b] Scroll the output window down one line[br]
		[b]Ctrl+L:[/b] Scroll to the last read line[br]
		[b]Ctrl+W:[/b] Close current window

		[big]Text editing related hotkeys:[/big]
		[b]Shift+LeftArrow:[/b] Select the previous character[br]
		[b]Shift+RightArrow:[/b] Select the next character[br]
		[b]Ctrl+LeftArrow:[/b] Move to the beginning of the previous word[br]
		[b]Ctrl+RightArrow:[/b] Move to the end of the next word[br]
		[b]Ctrl+Shift+LeftArrow:[/b] Select up to the beginning of the previous word[br]
		[b]Ctrl+Shift+RightArrow:[/b] Select up to the end of the next word[br]
		[b]Ctrl+Backspace:[/b] Delete last word[br]
		[b]Ctrl+<digit>:[/b] Script accelerators (see OnAccelKeyPressed)[br]
		[b]F2-F12, Shift+(F1-F12):[/b] Script accelerators (see OnAccelKeyPressed)[br]
		[b]Ctrl+B:[/b] Insert the 'bold' mIRC text control character[br]
		[b]Ctrl+I:[/b] Insert the 'italic' mIRC text control character[br]
		[b]Ctrl+K:[/b] Insert the 'color' mIRC text control character[br]
		[b]Ctrl+R:[/b] Insert the 'reverse' mIRC text control character[br]
		[b]Ctrl+U:[/b] Insert the 'underline' mIRC text control character[br]
		[b]Ctrl+O:[/b] Insert the 'reset' mIRC text control character[br]
		[b]Ctrl+P:[/b] Insert the 'non-crypt' (plain text) KVIrc control character used to disable[br] encryption of the current text line[br]
		[b]Ctrl+C:[/b] Copy the selected text to clipboard[br]
		[b]Ctrl+X:[/b] Cut the selected text[br]
		[b]Ctrl+V:[/b] Paste the clipboard contents (same as middle mouse click)[br]
		[b]Ctrl+Alt+E:[/b] Insert the 'icon' control code and pops up the icon list box[br]
		[b]UpArrow:[/b] Move backward in the command history and in the history popup[br]
		[b]DownArrow:[/b] Move forward in the command history and in the history popup[br]
		[b]Ctrl+PageUp:[/b] Open the history popup[br]
		[b]LeftArrow:[/b] Move the cursor to the left :)[br]
		[b]RightArrow:[/b] Move the cursor to the right[br]
		[b]Shift+LeftArrow:[/b] Move the selection to the left[br]
		[b]Shift+RightArrow:[/b] Move the selection to the right[br]
		[b]Alt+Enter:[/b] Show/hide the multi-line editor[br]
		[b]Ctrl+Enter:[/b] Send message as plain text, ignoring / commands[br]
		[b]Shift+Enter:[/b] Send message as a kvs command[br]
		[b]Alt+<numeric_sequence>:[/b] Insert the character by ASCII/Unicode code
		[example]
			[b]Alt+32:[/b] Inserts ASCII/Unicode character 32: ' ' (a space)
			[b]Alt+00032:[/b] Same as above :)
			[b]Alt+13:[/b] Inserts the Carriage Return (CR) control character
			[b]Alt+77:[/b] Inserts ASCII/Unicode character 77: 'M'
			[b]Alt+23566:[/b] Inserts Unicode character 23566 (an ideogram)
		[/example]

		[big]Completion hotkeys[/big]
		[b]Tab in the first word of input:[/b] Complete nicknames in the current channel or query[br]
		[b]Shift+Tab after the first word of input:[/b] Complete nicknames with full mask in the current channel or query[br]
		[b]Tab after a leading /:[/b] Complete commands[br]
		[b]Tab after a / in the middle of input:[/b] Complete directories[br]
		[b]Tab after a $:[/b] Complete function names
*/

#endif //_KVI_SHORTCUTS_H_
