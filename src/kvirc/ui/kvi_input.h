#ifndef _KVI_INPUT_H_
#define _KVI_INPUT_H_
//============================================================================
//
//   File : kvi_input.h
//   Creation date : Sun Jan 3 1999 23:04:10 by Szymon Stefanek
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//============================================================================

#include "kvi_settings.h"
#include "kvi_input_history.h"

#include <QWidget>
#include <QToolButton>

class QHBoxLayout;
class KviUserListView;
class KviWindow;
class KviInputEditor;
class KviTalHBox;
class KviTalPopupMenu;
class KviScriptEditor;

class KVIRC_API KviInput : public QWidget
{
	Q_OBJECT
public:
	KviInput(KviWindow * par, KviUserListView * view = 0);
	~KviInput();
public:
	KviWindow       * m_pWindow;
	KviInputEditor  * m_pInputEditor;
	KviScriptEditor * m_pMultiLineEditor;
	KviTalHBox      * m_pButtonContainer;
	QToolButton     * m_pMultiEditorButton;
	QToolButton     * m_pHistoryButton;
	QToolButton     * m_pIconButton;
	QToolButton     * m_pCommandlineModeButton;
	QToolButton     * m_pHideToolsButton;
	QHBoxLayout     * m_pLayout;
protected:
	//virtual void resizeEvent(QResizeEvent * e);
	virtual void focusInEvent(QFocusEvent * e);
	virtual void setFocusProxy(QWidget * w);
	virtual void keyPressEvent(QKeyEvent * e);
public slots:
	void multilineEditorButtonToggled(bool bOn);
	void historyButtonClicked();
	void iconButtonClicked();
	void inputEditorEnterPressed();
	void toggleToolButtons();
public:
	virtual void setFocus();
	void multiLinePaste(const QString & szText);
	bool isUserFriendly() { return m_pCommandlineModeButton->isChecked(); };
	void setUserFriendly(bool bSet) { m_pCommandlineModeButton->setChecked(bSet); };
	int heightHint() const;
	void setText(const QString & szText);
	void insertChar(char c);
	void insertText(const QString & szText);
	void applyOptions();
	bool isButtonsHidden();
	void setButtonsHidden(bool bHidden);
	//const QString & text();
	QString text();
	inline KviInputEditor * editor(){ return m_pInputEditor; };
	inline KviInputHistory * history(){ return KviInputHistory::instance(); };
};

#endif //_KVI_INPUT_H_
