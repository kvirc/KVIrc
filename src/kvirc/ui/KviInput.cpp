//=============================================================================
//
//   File : KviInput.cpp
//   Creation date : Sun Jan 3 1999 23:11:50 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 1999 Szymon Stefanek (pragma at kvirc dot net)
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
//=============================================================================


#define _KVI_DEBUG_CHECK_RANGE_
#include "kvi_debug.h"

#define _KVI_INPUT_CPP_

#include "KviInput.h"
#include "KviInputEditor.h"
#include "KviOptions.h"
#include "KviApplication.h"
#include "KviColorSelectionWindow.h"
#include "KviTextIconWindow.h"
#include "KviWindow.h"
#include "KviLocale.h"
#include "KviScriptEditor.h"
#include "KviHistoryWindow.h"
#include "KviUserInput.h"
#include "KviShortcut.h"
#include "KviTalHBox.h"
#include "KviTalToolTip.h"

#include <QLabel>
#include <QFileDialog>
#include <QPainter>
#include <QClipboard>
#include <QStringList>
#include <QApplication>
#include <QMessageBox>
#include <QLayout>
#include <QStyle>
#include <QEvent>
#include <QMouseEvent>
#include <QUrl>
#include <QHBoxLayout>
#include <QMenu>

#include <ctype.h>
#include <stdlib.h>

//This comes from KviApplication.cpp
extern KviColorWindow          * g_pColorWindow;
extern KviTextIconWindow       * g_pTextIconWindow;
extern KviHistoryWindowWidget  * g_pHistoryWindow;
extern QMenu         * g_pInputPopup;

KviInput::KviInput(KviWindow * pPar, KviUserListView * pView)
: QWidget(pPar)
{
	setObjectName("input_widget");
	m_pLayout=new QGridLayout(this);

	m_pLayout->setMargin(0);
	m_pLayout->setSpacing(0);

	m_pWindow = pPar;
	m_pMultiLineEditor = 0;

	m_pHideToolsButton = new QToolButton(this);
	m_pHideToolsButton->setObjectName("hide_container_button");

	m_pHideToolsButton->setIconSize(QSize(22,22));
	m_pHideToolsButton->setFixedWidth(16);

	if(g_pIconManager->getBigIcon("kvi_horizontal_left.png"))
		m_pHideToolsButton->setIcon(QIcon(*(g_pIconManager->getBigIcon("kvi_horizontal_left.png"))));

	connect(m_pHideToolsButton,SIGNAL(clicked()),this,SLOT(toggleToolButtons()));

	m_pButtonContainer = new KviTalHBox(this);
	m_pButtonContainer->setSpacing(0);
	m_pButtonContainer->setMargin(0);

	m_pButtonContainer->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));
	//if(m_pButtonContainer->layout())
	// m_pButtonContainer->layout()->setSizeConstraint(QLayout::SetMinimumSize);

	m_pHistoryButton = new QToolButton(m_pButtonContainer);
	m_pHistoryButton->setObjectName("historybutton");

	m_pHistoryButton->setIconSize(QSize(22,22));
	//m_pHistoryButton->setUpdatesEnabled(TRUE); ???
	QIcon is1;
	if(KVI_OPTION_BOOL(KviOption_boolEnableInputHistory))//G&N mar 2005
	{
		is1.addPixmap(*(g_pIconManager->getSmallIcon(KviIconManager::Time)));
		m_pHistoryButton->setIcon(is1);
		KviTalToolTip::add(m_pHistoryButton,__tr2qs("Show History<br>&lt;Ctrl+PageUp&gt;"));
		connect(m_pHistoryButton,SIGNAL(clicked()),this,SLOT(historyButtonClicked()));
	} else {
		is1.addPixmap(*(g_pIconManager->getSmallIcon(KviIconManager::QuitSplit)));
		m_pHistoryButton->setIcon(is1);
		KviTalToolTip::add(m_pHistoryButton,__tr2qs("Input History Disabled"));
	}

	m_pIconButton = new QToolButton(m_pButtonContainer);
	m_pIconButton->setObjectName("iconbutton");

	m_pIconButton->setIconSize(QSize(22,22));
	QIcon is3;
	is3.addPixmap(*(g_pIconManager->getSmallIcon(KviIconManager::BigGrin)));
	m_pIconButton->setIcon(is3);
	KviTalToolTip::add(m_pIconButton,__tr2qs("Show Icons Popup<br>&lt;Ctrl+I&gt;<br>See also /help texticons"));
	connect(m_pIconButton,SIGNAL(clicked()),this,SLOT(iconButtonClicked()));

	m_pCommandlineModeButton = new QToolButton(m_pButtonContainer);
	m_pCommandlineModeButton->setObjectName("commandlinemodebutton");

	m_pCommandlineModeButton->setIconSize(QSize(22,22));
	m_pCommandlineModeButton->setCheckable(true);
	QIcon is0;
	is0.addPixmap(*(g_pIconManager->getSmallIcon(KviIconManager::SaySmile)),QIcon::Normal,QIcon::On);
	is0.addPixmap(*(g_pIconManager->getSmallIcon(KviIconManager::SayKvs)),QIcon::Normal,QIcon::Off);
	m_pCommandlineModeButton->setIcon(is0);
	KviTalToolTip::add(m_pCommandlineModeButton,__tr2qs("User friendly commandline mode<br>See also /help commandline"));
	if(KVI_OPTION_BOOL(KviOption_boolCommandlineInUserFriendlyModeByDefault))
		m_pCommandlineModeButton->setChecked(true);


	m_pMultiEditorButton = new QToolButton(m_pButtonContainer);
	m_pMultiEditorButton->setObjectName("multieditorbutton");

	m_pMultiEditorButton->setCheckable(true);
	m_pMultiEditorButton->setIconSize(QSize(22,22));
	QIcon is2;
	is2.addPixmap(*(g_pIconManager->getSmallIcon(KviIconManager::Terminal)),QIcon::Normal,QIcon::On);
	is2.addPixmap(*(g_pIconManager->getSmallIcon(KviIconManager::Terminal)),QIcon::Normal,QIcon::Off);
	m_pMultiEditorButton->setIcon(is2);
	QString szTip = __tr2qs("Multi-line Editor<br>&lt;Alt+Return&gt;");
	KviTalToolTip::add(m_pMultiEditorButton,szTip);

	connect(m_pMultiEditorButton,SIGNAL(toggled(bool)),this,SLOT(multiLineEditorButtonToggled(bool)));

	m_pInputEditor = new KviInputEditor(this,pPar,pView);
	connect(m_pInputEditor,SIGNAL(enterPressed()),this,SLOT(inputEditorEnterPressed()));
	m_pInputEditor->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Ignored));


	m_pMultiEditorButton->setAutoRaise(true);
	m_pCommandlineModeButton->setAutoRaise(true);
	m_pIconButton->setAutoRaise(true);
	m_pHistoryButton->setAutoRaise(true);
	m_pHideToolsButton->setAutoRaise(true);

	m_pLayout->addWidget(m_pHideToolsButton,0,2,2,1);
	m_pLayout->addWidget(m_pButtonContainer,0,1,2,1);
	m_pLayout->addWidget(m_pInputEditor,0,0,2,1);

	installShortcuts();
}

KviInput::~KviInput()
{
	if(m_pMultiLineEditor)
		KviScriptEditor::destroyInstance(m_pMultiLineEditor);
}

bool KviInput::isButtonsHidden()
{
	return m_pButtonContainer->isHidden();
}

void KviInput::setButtonsHidden(bool bHidden)
{
	if(!m_pHideToolsButton || !m_pButtonContainer)
		return;
	if(bHidden==m_pButtonContainer->isHidden())
		return;
	m_pButtonContainer->setHidden(bHidden);
	QPixmap * pix= bHidden ?
		g_pIconManager->getBigIcon("kvi_horizontal_right.png") :
		g_pIconManager->getBigIcon("kvi_horizontal_left.png");
	if(pix)
		m_pHideToolsButton->setIcon(QIcon(*pix));
}

void KviInput::toggleToolButtons()
{
	setButtonsHidden(!isButtonsHidden());
}

void KviInput::inputEditorEnterPressed()
{
	QString szText = m_pInputEditor->text();
	KviUserInput::parse(szText,m_pWindow,QString(),m_pCommandlineModeButton->isChecked());
	m_pInputEditor->setText("");
	m_pInputEditor->clearUndoStack();
}

void KviInput::installShortcuts()
{
	KviShortcut::create(KVI_SHORTCUTS_INPUT_MULTILINE,this,SLOT(toggleMultiLine()),0,Qt::WidgetWithChildrenShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_MULTILINE_2,this,SLOT(toggleMultiLine()),0,Qt::WidgetWithChildrenShortcut);
}

void KviInput::keyPressEvent(QKeyEvent * e)
{
	if(e->modifiers() & Qt::ControlModifier)
	{
		switch(e->key())
		{
			case Qt::Key_Enter:
			case Qt::Key_Return:
			{
				if(m_pMultiLineEditor)
				{
					QString szText;
					m_pMultiLineEditor->getText(szText);
					if(szText.isEmpty())return;
					if(KVI_OPTION_BOOL(KviOption_boolWarnAboutPastingMultipleLines))
					{
						if(szText.length() > 256)
						{
							if(szText[0] != '/')
							{
								int nLines = szText.count('\n') + 1;
								if(nLines > 15)
								{
									int nRet = QMessageBox::question(
										this,
										__tr2qs("Confirm Multiline Message"),
										__tr2qs("You're about to send a message with %1 lines of text.<br><br>" \
											"There is nothing wrong with it, this warning is<br>" \
											"here to prevent you from accidentally sending<br>" \
											"a really large message just because you didn't edit it<br>" \
											"properly after pasting text from the clipboard.<br><br>" \
											"Do you want the message to be sent?").arg(nLines),
										__tr2qs("Yes, always"),
										__tr2qs("Yes"),
										__tr2qs("No"),
										1,2);
									switch(nRet)
									{
										case 0:
											KVI_OPTION_BOOL(KviOption_boolWarnAboutPastingMultipleLines) = false;
										break;
										case 2:
											return;
										break;
										default: // also case 1
										break;
									}
								}
							}
						}
					}
					szText.replace('\t',QString(KVI_OPTION_UINT(KviOption_uintSpacesToExpandTabulationInput),' ')); //expand tabs to spaces
					KviUserInput::parse(szText,m_pWindow,QString(),m_pCommandlineModeButton->isChecked());
					m_pMultiLineEditor->setText("");
				}
			}
			break;
		}
	}
}

void KviInput::multiLinePaste(const QString & szText)
{
	if(!m_pMultiLineEditor)
		multiLineEditorButtonToggled(true);
	QString szCompleteText = m_pInputEditor->text();
	szCompleteText.append(szText);
	m_pMultiLineEditor->setText(szCompleteText);
}

void KviInput::multiLineEditorButtonToggled(bool bOn)
{
	if(m_pMultiLineEditor)
	{
		if(bOn) return;
		QString szTmp;
		m_pMultiLineEditor->getText(szTmp);
		m_pLayout->removeWidget(m_pMultiLineEditor);
		m_pLayout->removeWidget(m_pHelpLabel);

		KviScriptEditor::destroyInstance(m_pMultiLineEditor);
		m_pMultiLineEditor = 0;

		delete m_pHelpLabel;
		m_pHelpLabel = 0;

		szTmp.replace(QRegExp("[\a\f\n\r\v]"), QString(" "));
		szTmp.replace('\t',QString(KVI_OPTION_UINT(KviOption_uintSpacesToExpandTabulationInput),' ')); //expand tabs to spaces
		m_pInputEditor->setText(szTmp);
		m_pInputEditor->show();
		m_pWindow->childrenTreeChanged(0);
		m_pInputEditor->setFocus();
		m_pMultiEditorButton->setChecked(false);
	} else {
		if(!bOn) return;
		m_pInputEditor->hide();

		m_pHelpLabel = new QLabel();
		m_pHelpLabel->setIndent(5); // we only want a left margin here
		m_pHelpLabel->setText(__tr2qs("<Ctrl+Return>; submits, <Alt+Return>; hides this editor"));
		m_pLayout->addWidget(m_pHelpLabel,0,0,1,1);

		m_pMultiLineEditor = KviScriptEditor::createInstance(this);
		m_pMultiLineEditor->setText(m_pInputEditor->text());
		m_pLayout->addWidget(m_pMultiLineEditor,1,0,1,1);

		m_pWindow->childrenTreeChanged(m_pMultiLineEditor);
		m_pMultiLineEditor->setFocus();
		m_pMultiEditorButton->setChecked(true);
	}
}

void KviInput::iconButtonClicked()
{
	if(!g_pTextIconWindow)
		g_pTextIconWindow = new KviTextIconWindow();
	QPoint pnt = m_pIconButton->mapToGlobal(QPoint(m_pIconButton->width(),0));
 	g_pTextIconWindow->move(pnt.x()-g_pTextIconWindow->width(),pnt.y() - g_pTextIconWindow->height());
	g_pTextIconWindow->popup(this,true);
}

void KviInput::historyButtonClicked()
{
	if(!g_pHistoryWindow)
		g_pHistoryWindow = new KviHistoryWindowWidget();

	QPoint pnt = mapToGlobal(QPoint(0,0));

	g_pHistoryWindow->setGeometry(pnt.x(),pnt.y() - KVI_HISTORY_WIN_HEIGHT,width(),KVI_HISTORY_WIN_HEIGHT);
	g_pHistoryWindow->popup(this);
}

void KviInput::setFocus()
{
	// redirect setFocus() to the right children
	if(m_pMultiLineEditor)
		m_pMultiLineEditor->setFocus();
	else
		m_pInputEditor->setFocus();
}

void KviInput::focusInEvent(QFocusEvent *)
{
	// if we get a focus in event, redirect the focus to the children
	if(m_pMultiLineEditor)
		m_pMultiLineEditor->setFocus();
	else
		m_pInputEditor->setFocus();
}

int KviInput::heightHint() const
{
	return m_pMultiLineEditor ? 120 : m_pInputEditor->heightHint();
}

void KviInput::setText(const QString & szText)
{
	if(m_pMultiLineEditor)
		m_pMultiLineEditor->setText(szText);
	else
		m_pInputEditor->setText(szText);
}

void KviInput::insertChar(char c)
{
	m_pInputEditor->insertChar(c);
}

void KviInput::insertText(const QString & szText)
{
	m_pInputEditor->insertText(szText);
}

void KviInput::applyOptions()
{
	if(KVI_OPTION_BOOL(KviOption_boolEnableInputHistory))
	{
		QIcon is1;
		is1.addPixmap(*(g_pIconManager->getSmallIcon(KviIconManager::Time)));
		m_pHistoryButton->setIcon(is1);
		KviTalToolTip::add(m_pHistoryButton,__tr2qs("Show History<br>&lt;Ctrl+PageUp&gt;"));
		connect(m_pHistoryButton,SIGNAL(clicked()),this,SLOT(historyButtonClicked()));
	} else {
		QIcon is1;
		is1.addPixmap(*(g_pIconManager->getSmallIcon(KviIconManager::QuitSplit)));
		m_pHistoryButton->setIcon(is1);
		KviTalToolTip::add(m_pHistoryButton,__tr2qs("Input History Disabled"));
		m_pHistoryButton->disconnect(SIGNAL(clicked()));
	}

	m_pInputEditor->applyOptions();
}

void KviInput::setFocusProxy(QWidget *)
{
	/* do nothing */
}

QString KviInput::text()
{
	QString szText;
	if(m_pMultiLineEditor)
		m_pMultiLineEditor->getText(szText);
	else
		szText = m_pInputEditor->text();
	return szText;
}

void KviInput::toggleMultiLine()
{
	multiLineEditorButtonToggled(!m_pMultiLineEditor);
}

/*
	@doc: commandline
	@title:
		The Commandline Input Features
	@type:
		generic
	@short:
		Commandline input features
	@body:
		[big]Principles of operation[/big]
		[p]
		The idea is simple: anything that starts with a slash (/) character
		is interpreted as a command. Anything else is plain text that is
		sent to the target of the window (channel, query, dcc chat etc..).
		[/p]
		[big]The two operating modes[/big]
		[p]
		The commandline input has two operating modes: the "user friendly mode" and
		the "kvs mode". In the user friendly mode all the parameters of the commands
		are interpreted exactly like you type them. There is no special interpretation
		of $,%,-,( and ; characters. This allows you to type "/me is happy ;)", for example.
		In the kvs mode the full parameter interpretation is enabled and the commands
		work just like in any other script editor. This means that anything that
		starts with a $ is a function call, anything that starts with a % is a variable,
		the dash characters after command names are interpreted as switches and ; is the
		command separator. This in turn does NOT allow you to type "/me is happy ;)"
		because ; is the command separator and ) will be interpreted as the beginning
		of the next command. In KVS mode you obviously have to escape the ; character
		by typing "/me is happy \;)". The user friendly mode is good for everyday chatting
		and for novice users while the KVS mode is for experts that know that minimum about
		scripting languages. Please note that in the user-friendly mode you're not allowed
		to type multiple commands at once :).
		[/p]
		[p]
		Also look at the [doc:keyboard]keyboard shortcuts[/doc] reference.[br]
		If you drop a file on this widget, a <a href="parse.kvihelp">/PARSE &lt;filename&gt;</a> will be executed.[br]
		You can enable word substitution in the preferences dialog.[br]
		For example, if you choose to substitute "afaik" with "As far as I know",[br]
		when you will type "afaik" somewhere in the command line, and then
		press Space or Return, that word will be replaced with "As far as I know".[br]
		Experiment with it :)[br]
		The Tab key activates the completion of the current word.[br]
		If a word is prefixed with a '/', it is treated as a command to be completed,
		if it begins with '$', it is treated as a function or identifier to be completed,
		otherwise it is treated as a nickname or filename to be completed.
		[example]
			/ec&lt;Tab&gt; will produce /echo&lt;space&gt;
			/echo $loca&lt;Tab&gt; will produce /echo $localhost
		[/example]
		Multiple matches are listed in the view window and the word is completed
		to the common part of all the matches.
		[example]
			$sel&lt;Tab;&gt; will find multiple matches and produce $selected
		[/example]
		Experiment with that too :)
		[/p]
*/

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "KviInput.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
