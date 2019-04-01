#ifndef _KVI_INPUT_H_
#define _KVI_INPUT_H_
//============================================================================
//
//   File : KviInput.h
//   Creation date : Sun Jan 3 1999 23:04:10 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999 Szymon Stefanek (pragma at kvirc dot net)
//   Copyright (C) 2008 Elvio Basello (hellvis69 at netsons dot org)
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

/**
* \file KviInput.h
* \author Szymon Stefanek
* \brief Input handling
*/

#include "kvi_settings.h"
#include "KviInputHistory.h"

#include <QWidget>
#include <QToolButton>
#include <QLabel>

class QGridLayout;
class KviUserListView;
class KviWindow;
class KviInputEditor;
class KviTalHBox;
class QMenu;
class KviScriptEditor;

/**
* \class KviInput
* \brief Input handling class
*/
class KVIRC_API KviInput : public QWidget
{
	friend class KviTextIconWindow;
	Q_OBJECT
public:
	/**
	* \brief Constructs the input object
	* \param pPar The parent window
	* \param pView The userlist
	* \return KviInput
	*/
	KviInput(KviWindow * pPar, KviUserListView * pView = nullptr);

	/**
	* \brief Destroys the input object
	*/
	~KviInput();

public:
	KviWindow * m_pWindow;
	KviInputEditor * m_pInputEditor;
	KviScriptEditor * m_pMultiLineEditor;
	KviTalHBox * m_pButtonContainer;
	QToolButton * m_pMultiEditorButton;
	QToolButton * m_pHistoryButton;
	QToolButton * m_pIconButton;
	QToolButton * m_pCommandlineModeButton;
	QToolButton * m_pHideToolsButton;
	QGridLayout * m_pLayout;
	QLabel * m_pHelpLabel;

public:
	/**
	* \brief Sets the focus to the input line
	* \return void
	*/
	virtual void setFocus();

	/**
	* \brief Opens the multiline editor and pastes the given text
	* \param szText The text to paste
	* \return void
	*/
	void multiLinePaste(const QString & szText);

	/**
	* \brief Returns true if the input line is in user-friendly mode
	* \return bool
	*/
	bool isUserFriendly() { return m_pCommandlineModeButton->isChecked(); };

	/**
	* \brief Sets the user-friendly mode to the input line
	* \param bSet Whether to set the user-friendly mode
	* \return void
	*/
	void setUserFriendly(bool bSet) { m_pCommandlineModeButton->setChecked(bSet); };

	/**
	* \brief Returns the height of the input line
	* \return int
	*/
	int heightHint() const;

	/**
	* \brief Sets the given text
	* \param szText The text to set in the input line
	* \return void
	*/
	void setText(const QString & szText);

	/**
	* \brief Inserts the given character
	* \param c The character to insert
	* \return void
	*/
	void insertChar(char c);

	/**
	* \brief Inserts the given text
	* \param szText The text to insert
	* \return void
	*/
	void insertText(const QString & szText);

	/**
	* \brief Saves and applies the options
	* \return void
	*/
	void applyOptions();

	/**
	* \brief Returns true if the buttons on the right are hidden
	* \return bool
	*/
	bool isButtonsHidden();

	/**
	* \brief Sets the hidden status of the button on the right
	* \param bHidden Whether to hide the buttons
	* \return void
	*/
	void setButtonsHidden(bool bHidden);

	/**
	* \brief Returns the text written in the input line
	* \return QString
	*/
	QString text();
	//const QString & text();

	/**
	* \brief Return the instance of the input editor
	* \return KviInputEditor *
	*/
	KviInputEditor * editor() const { return m_pInputEditor; }

	/**
	* \brief Return the instance of the input history
	* \return KviInputHistory *
	*/
	KviInputHistory * history() { return KviInputHistory::instance(); }
protected:
	void installShortcuts();
	void focusInEvent(QFocusEvent * e) override;
	void setFocusProxy(QWidget * w);
	void keyPressEvent(QKeyEvent * e) override;
public slots:
	/**
	* \brief Toggles the multiline editor
	* \param bOn Whether to activate the multiline editor
	* \return void
	*/
	void multiLineEditorButtonToggled(bool bOn);

	/**
	* \brief Shows the history popup
	* \return void
	*/
	void historyButtonClicked();

	/**
	* \brief Shows the icon popup
	* \return void
	*/
	void iconButtonClicked();

	/**
	* \brief Parses the text written and clears the input line
	* \return void
	*/
	void inputEditorEnterPressed();

	/**
	* \brief Toggles the buttons
	* \return void
	*/
	void toggleToolButtons();
private slots:
	/**
	* \brief Toggles multiline
	* \return void
	*/
	void toggleMultiLine();
};

#endif //_KVI_INPUT_H_
