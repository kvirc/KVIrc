//=============================================================================
//
//   File : KviInput.cpp
//   Creation date : Sun Jan 3 1999 23:11:50 by Szymon Stefanek
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
#include <QPushButton>
#include <QFontMetrics>

#include <cctype>
#include <cstdlib>

//This comes from KviApplication.cpp
extern KviColorWindow * g_pColorWindow;
extern KviTextIconWindow * g_pTextIconWindow;
extern KviHistoryWindowWidget * g_pHistoryWindow;
extern QMenu * g_pInputPopup;

KviInput::KviInput(KviWindow * pPar, KviUserListView * pView)
    : QWidget(pPar)
{
	setObjectName("input_widget");
	m_pLayout = new QGridLayout(this);

	m_pLayout->setMargin(0);
	m_pLayout->setSpacing(0);

	m_pWindow = pPar;
	m_pMultiLineEditor = nullptr;

	m_pHideToolsButton = new QToolButton(this);
	m_pHideToolsButton->setObjectName("hide_container_button");

	m_pHideToolsButton->setIconSize(QSize(22, 22));
	m_pHideToolsButton->setFixedWidth(16);

	if(g_pIconManager->getBigIcon("kvi_horizontal_left.png"))
		m_pHideToolsButton->setIcon(QIcon(*(g_pIconManager->getBigIcon("kvi_horizontal_left.png"))));

	connect(m_pHideToolsButton, SIGNAL(clicked()), this, SLOT(toggleToolButtons()));

	m_pButtonContainer = new KviTalHBox(this);
	m_pButtonContainer->setSpacing(0);
	m_pButtonContainer->setMargin(0);

	m_pButtonContainer->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));

	m_pHistoryButton = new QToolButton(m_pButtonContainer);
	m_pHistoryButton->setObjectName("historybutton");

	m_pHistoryButton->setIconSize(QSize(22, 22));

	QIcon is1;
	if(KVI_OPTION_BOOL(KviOption_boolEnableInputHistory)) //G&N mar 2005
	{
		is1.addPixmap(*(g_pIconManager->getSmallIcon(KviIconManager::History)));
		m_pHistoryButton->setIcon(is1);
		KviTalToolTip::add(m_pHistoryButton, __tr2qs("Show history Ctrl+PageUp"));
		connect(m_pHistoryButton, SIGNAL(clicked()), this, SLOT(historyButtonClicked()));
	}
	else
	{
		is1.addPixmap(*(g_pIconManager->getSmallIcon(KviIconManager::HistoryOff)));
		m_pHistoryButton->setIcon(is1);
		m_pHistoryButton->setEnabled(false);
		KviTalToolTip::add(m_pHistoryButton, __tr2qs("Input history disabled"));
	}

	m_pIconButton = new QToolButton(m_pButtonContainer);
	m_pIconButton->setObjectName("iconbutton");

	m_pIconButton->setIconSize(QSize(22, 22));
	QIcon is3;
	is3.addPixmap(*(g_pIconManager->getSmallIcon(KviIconManager::BigGrin)));
	m_pIconButton->setIcon(is3);
	KviTalToolTip::add(m_pIconButton, __tr2qs("Show icons popup Alt+E<br>See also /help texticons"));
	connect(m_pIconButton, SIGNAL(clicked()), this, SLOT(iconButtonClicked()));

	m_pCommandlineModeButton = new QToolButton(m_pButtonContainer);
	m_pCommandlineModeButton->setObjectName("commandlinemodebutton");

	m_pCommandlineModeButton->setIconSize(QSize(22, 22));
	m_pCommandlineModeButton->setCheckable(true);
	QIcon is0;
	is0.addPixmap(*(g_pIconManager->getSmallIcon(KviIconManager::SaySmile)), QIcon::Normal, QIcon::On);
	is0.addPixmap(*(g_pIconManager->getSmallIcon(KviIconManager::SayKvs)), QIcon::Normal, QIcon::Off);
	m_pCommandlineModeButton->setIcon(is0);
	KviTalToolTip::add(m_pCommandlineModeButton, __tr2qs("User friendly command-line mode Ctrl+Y<br>See also /help commandline"));

	if(KVI_OPTION_BOOL(KviOption_boolCommandlineInUserFriendlyModeByDefault))
		m_pCommandlineModeButton->setChecked(true);

	m_pMultiEditorButton = new QToolButton(m_pButtonContainer);
	m_pMultiEditorButton->setObjectName("multieditorbutton");

	m_pMultiEditorButton->setCheckable(true);
	m_pMultiEditorButton->setIconSize(QSize(22, 22));
	QIcon is2;
	is2.addPixmap(*(g_pIconManager->getSmallIcon(KviIconManager::IrcView)), QIcon::Normal, QIcon::On);
	is2.addPixmap(*(g_pIconManager->getSmallIcon(KviIconManager::Terminal)), QIcon::Normal, QIcon::Off);
	m_pMultiEditorButton->setIcon(is2);

	QString szTip = __tr2qs("Multi-line editor Alt+Return");
#ifdef COMPILE_ON_MAC
	szTip.replace(QString("Alt+Return;"), QString("⌥↩"));
#endif
	KviTalToolTip::add(m_pMultiEditorButton, szTip);

	connect(m_pMultiEditorButton, SIGNAL(toggled(bool)), this, SLOT(multiLineEditorButtonToggled(bool)));

	m_pInputEditor = new KviInputEditor(this, pPar, pView);
	connect(m_pInputEditor, SIGNAL(enterPressed()), this, SLOT(inputEditorEnterPressed()));
	m_pInputEditor->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored));

	m_pMultiEditorButton->setAutoRaise(true);
	m_pCommandlineModeButton->setAutoRaise(true);
	m_pIconButton->setAutoRaise(true);
	m_pHistoryButton->setAutoRaise(true);
	m_pHideToolsButton->setAutoRaise(true);

	m_pLayout->addWidget(m_pHideToolsButton, 0, 2, 2, 1);
	m_pLayout->addWidget(m_pButtonContainer, 0, 1, 2, 1);
	m_pLayout->addWidget(m_pInputEditor, 0, 0, 2, 1);

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
	if(bHidden == m_pButtonContainer->isHidden())
		return;
	m_pButtonContainer->setHidden(bHidden);
	QPixmap * pix = bHidden ? g_pIconManager->getBigIcon("kvi_horizontal_right.png") : g_pIconManager->getBigIcon("kvi_horizontal_left.png");
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
	KviUserInput::parse(szText, m_pWindow, QString(), m_pCommandlineModeButton->isChecked());
	m_pInputEditor->setText("");
	m_pInputEditor->clearUndoStack();
}

void KviInput::installShortcuts()
{
	KviShortcut::create(KVI_SHORTCUTS_INPUT_MULTILINE, this, SLOT(toggleMultiLine()), nullptr, Qt::WidgetWithChildrenShortcut);
	KviShortcut::create(KVI_SHORTCUTS_INPUT_MULTILINE_2, this, SLOT(toggleMultiLine()), nullptr, Qt::WidgetWithChildrenShortcut);
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
					if(szText.isEmpty())
						return;
					if(KVI_OPTION_BOOL(KviOption_boolWarnAboutPastingMultipleLines))
					{
						if(szText.length() > 256)
						{
							if(szText[0] != '/')
							{
								int nLines = szText.count('\n') + 1;
								if(nLines > 15)
								{
									QMessageBox pMsgBox;
									pMsgBox.setText(__tr2qs("You're about to send a message with %1 lines of text.<br><br>"
									                        "This warning is here to prevent you from accidentally "
									                        "pasting and sending a really large, potentially unedited message from your clipboard.<br><br>"
									                        "Some IRC servers may also consider %1 lines of text a flood, "
									                        "in which case you will be disconnected from said server.<br><br>"
									                        "Do you still want the message to be sent?").arg(nLines));

									pMsgBox.setWindowTitle(__tr2qs("Confirm Sending a Large Message - KVIrc"));
									pMsgBox.setIcon(QMessageBox::Question);
									QAbstractButton * pAlwaysButton = pMsgBox.addButton(__tr2qs("Always"), QMessageBox::YesRole);
									/* QAbstractButton *pYesButton = */ pMsgBox.addButton(__tr2qs("Yes"), QMessageBox::YesRole);
									QAbstractButton * pNoButton = pMsgBox.addButton(__tr2qs("No"), QMessageBox::NoRole);
									pMsgBox.setDefaultButton(qobject_cast<QPushButton *>(pNoButton));
									pMsgBox.exec();
									if(pMsgBox.clickedButton() == pAlwaysButton)
									{
										KVI_OPTION_BOOL(KviOption_boolWarnAboutPastingMultipleLines) = false;
									}
									else if(pMsgBox.clickedButton() == pNoButton || pMsgBox.clickedButton() == nullptr)
									{
										return;
									}
								}
							}
						}
					}
					szText.replace('\t', QString(KVI_OPTION_UINT(KviOption_uintSpacesToExpandTabulationInput), ' ')); //expand tabs to spaces
					KviUserInput::parse(szText, m_pWindow, QString(), m_pCommandlineModeButton->isChecked());
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
		if(bOn)
			return;
		QString szTmp;
		m_pMultiLineEditor->getText(szTmp);
		m_pLayout->removeWidget(m_pMultiLineEditor);
		m_pLayout->removeWidget(m_pHelpLabel);

		KviScriptEditor::destroyInstance(m_pMultiLineEditor);
		m_pMultiLineEditor = nullptr;

		delete m_pHelpLabel;
		m_pHelpLabel = nullptr;

		szTmp.replace(QRegExp("[\a\f\n\r\v]"), QString(" "));
		szTmp.replace('\t', QString(KVI_OPTION_UINT(KviOption_uintSpacesToExpandTabulationInput), ' ')); //expand tabs to spaces
		m_pInputEditor->setText(szTmp);
		m_pInputEditor->show();
		m_pWindow->childrenTreeChanged(nullptr);
		m_pInputEditor->setFocus();
		m_pMultiEditorButton->setChecked(false);
	}
	else
	{
		if(!bOn)
			return;
		m_pInputEditor->hide();

		m_pHelpLabel = new QLabel();
		m_pHelpLabel->setContentsMargins(15, 5, 0, 0);

		QString tmpHelpLabel = __tr2qs("Ctrl+Return; submits contents, Alt+Return; hides this editor");
#ifdef COMPILE_ON_MAC
		tmpHelpLabel.replace(QString("Ctrl+Return;"), QString("⌘↩"));
		tmpHelpLabel.replace(QString("Alt+Return;"), QString("⌥↩"));
#endif
		m_pHelpLabel->setText(tmpHelpLabel);
		m_pLayout->addWidget(m_pHelpLabel, 0, 0, 1, 1);

		m_pMultiLineEditor = KviScriptEditor::createInstance(this);
		m_pMultiLineEditor->setText(m_pInputEditor->text());
		m_pLayout->addWidget(m_pMultiLineEditor, 1, 0, 1, 1);

		m_pWindow->childrenTreeChanged(m_pMultiLineEditor);
		m_pMultiLineEditor->setFocus();
		m_pMultiEditorButton->setChecked(true);
	}
}

void KviInput::iconButtonClicked()
{
	if(!g_pTextIconWindow)
		g_pTextIconWindow = new KviTextIconWindow();
	QPoint pnt = m_pIconButton->mapToGlobal(QPoint(m_pIconButton->width(), 0));
	g_pTextIconWindow->move(pnt.x() - g_pTextIconWindow->width(), pnt.y() - g_pTextIconWindow->height());
	g_pTextIconWindow->popup(this, true);
}

void KviInput::historyButtonClicked()
{
	if(!g_pHistoryWindow)
		g_pHistoryWindow = new KviHistoryWindowWidget();

	QPoint pnt = mapToGlobal(QPoint(0, 0));

	g_pHistoryWindow->setGeometry(pnt.x(), pnt.y() - KVI_HISTORY_WIN_HEIGHT, width(), KVI_HISTORY_WIN_HEIGHT);
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
	return m_pMultiLineEditor ? (m_pInputEditor->heightHint() * 6) : m_pInputEditor->heightHint();
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
		if(!m_pHistoryButton->isEnabled())
		{
			QIcon is1;
			is1.addPixmap(*(g_pIconManager->getSmallIcon(KviIconManager::History)));
			m_pHistoryButton->setIcon(is1);
			m_pHistoryButton->setEnabled(true);
			KviTalToolTip::add(m_pHistoryButton, __tr2qs("Show history Ctrl+PageUp"));
			connect(m_pHistoryButton, SIGNAL(clicked()), this, SLOT(historyButtonClicked()));
		}
	}
	else
	{
		if(m_pHistoryButton->isEnabled())
		{
			QIcon is1;
			is1.addPixmap(*(g_pIconManager->getSmallIcon(KviIconManager::HistoryOff)));
			m_pHistoryButton->setIcon(is1);
			m_pHistoryButton->setEnabled(false);
			KviTalToolTip::add(m_pHistoryButton, __tr2qs("Input history disabled"));
			m_pHistoryButton->disconnect(SIGNAL(clicked()));
		}
	}

	m_pInputEditor->applyOptions();
}

void KviInput::setFocusProxy(QWidget *)
{
}/* do nothing */

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
		[br]
		The idea is simple: anything that starts with a slash [b]/[/b] character
		is interpreted as a command. Anything else is plain text that is
		sent to the target of the window (channel, query, DCC chat etc.).
		[big]The two operating modes[/big]
		[br]
		The commandline input has two operating modes: the [i]user friendly mode[/i] and
		the [i]KVS mode[/i]. In the user friendly mode all the parameters of the commands
		are interpreted exactly like you type them. There is no special interpretation
		of [b]$[/b], [b]%[/b], [b]-[/b], [b]([/b] and [b];[/b] characters. This allows you to type [i]/me is happy ;)[/i], for example.
		In the KVS mode the full parameter interpretation is enabled and the commands
		work just like in any other script editor. This means that anything that
		starts with a [b]$[/b] is a function call, anything that starts with a % is a variable,
		the dash characters after command names are interpreted as switches and [b];[/b] is the
		command separator. This in turn does [b]not[/b] allow you to type [i]/me is happy ;)[/i]
		because [b];[/b] is the command separator and ) will be interpreted as the beginning
		of the next command. In KVS mode you obviously have to escape the ; character
		by typing [i]/me is happy \;)[/i]. The user friendly mode is good for everyday chatting
		and for novice users while the KVS mode is for experts that know that minimum about
		scripting languages. Please note that in the user-friendly mode you're not allowed
		to type multiple commands at once :).
		[br]
		Also look at the [doc:keyboard]keyboard shortcuts[/doc] reference.[br]
		If you drop a file on this widget, a <a href="parse.kvihelp">/PARSE &lt;filename&gt;</a> will be executed.[br]
		You can enable word substitution in the preferences dialog.[br]
		For example, if you choose to substitute [b]afaik[/b] with [b]A[/b]s [b]f[/b]ar [b]a[/b]s [b]I[/b] [b]k[/b]now,[br]
		when you will type [b]afaik[/b] somewhere in the command line, and then
		press Space or Return, that word will be replaced with [i]As far as I know[/i].[br]
		Experiment with it :)[br]
		The Tab key activates the completion of the current word.[br]
		If a word is prefixed with a [b]/[/b], it is treated as a command to be completed,
		if it begins with [b]$[/b], it is treated as a function or identifier to be completed,
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
		[br]
*/
