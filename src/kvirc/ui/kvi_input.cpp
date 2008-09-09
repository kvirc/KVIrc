//=============================================================================
//
//   File : kvi_input.cpp
//   Creation date : Sun Jan 3 1999 23:11:50 by Szymon Stefanek
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
//=============================================================================


#define _KVI_DEBUG_CHECK_RANGE_
#include "kvi_debug.h"

#define _KVI_INPUT_CPP_

#include "kvi_input.h"
#include "kvi_input_editor.h"
#include "kvi_options.h"
#include "kvi_app.h"
#include "kvi_colorwin.h"
#include "kvi_texticonwin.h"
#include "kvi_window.h"
#include "kvi_locale.h"
#include "kvi_scripteditor.h"
#include "kvi_historywin.h"
#include "kvi_userinput.h"
#include "kvi_tal_popupmenu.h"
#include "kvi_tal_hbox.h"
#include "kvi_tal_tooltip.h"

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

#include <ctype.h>
#include <stdlib.h>

//This comes from kvi_app.cpp
extern KviColorWindow          * g_pColorWindow;
extern KviTextIconWindowWidget * g_pTextIconWindow;
extern KviHistoryWindowWidget  * g_pHistoryWindow;
extern KviTalPopupMenu         * g_pInputPopup;

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	extern QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif

QFontMetrics * g_pLastFontMetrics = 0;

KviInput::KviInput(KviWindow * pPar, KviUserListView * pView)
: QWidget(pPar)
{
	setObjectName("input_widget");
	m_pLayout=new QHBoxLayout(this);
	m_pLayout->setDirection(QBoxLayout::RightToLeft);

	m_pLayout->setMargin(0);
	m_pLayout->setSpacing(0);

	m_pWindow = pPar;
	m_pMultiLineEditor = 0;

	m_pHideToolsButton = new QToolButton(this);
	m_pHideToolsButton->setObjectName("hide_container_button");

	m_pHideToolsButton->setIconSize(QSize(22,22));
	m_pHideToolsButton->setFixedWidth(10);

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
		is1.addPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TIME)));
		m_pHistoryButton->setIcon(is1);
		KviTalToolTip::add(m_pHistoryButton,__tr2qs("Show History<br>&lt;Ctrl+PageUp&gt;"));
		connect(m_pHistoryButton,SIGNAL(clicked()),this,SLOT(historyButtonClicked()));
	} else {
		is1.addPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_QUITSPLIT)));
		m_pHistoryButton->setIcon(is1);
		KviTalToolTip::add(m_pHistoryButton,__tr2qs("Input History Disabled"));
	}

	m_pIconButton = new QToolButton(m_pButtonContainer);
	m_pIconButton->setObjectName("iconbutton");

	m_pIconButton->setIconSize(QSize(22,22));
	QIcon is3;
	is3.addPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_BIGGRIN)));
	m_pIconButton->setIcon(is3);
	KviTalToolTip::add(m_pIconButton,__tr2qs("Show Icons Popup<br>&lt;Ctrl+I&gt;<br>See also /help texticons"));
	connect(m_pIconButton,SIGNAL(clicked()),this,SLOT(iconButtonClicked()));

	m_pCommandlineModeButton = new QToolButton(m_pButtonContainer);
	m_pCommandlineModeButton->setObjectName("commandlinemodebutton");

	m_pCommandlineModeButton->setIconSize(QSize(22,22));
	m_pCommandlineModeButton->setCheckable(true);
	QIcon is0;
	is0.addPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SAYSMILE)),QIcon::Normal,QIcon::On);
	is0.addPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SAYKVS)),QIcon::Normal,QIcon::Off);
	m_pCommandlineModeButton->setIcon(is0);
	KviTalToolTip::add(m_pCommandlineModeButton,__tr2qs("User friendly commandline mode<br>See also /help commandline"));
	if(KVI_OPTION_BOOL(KviOption_boolCommandlineInUserFriendlyModeByDefault))
		m_pCommandlineModeButton->setChecked(true);


	m_pMultiEditorButton = new QToolButton(m_pButtonContainer);
	m_pMultiEditorButton->setObjectName("multieditorbutton");

	m_pMultiEditorButton->setCheckable(true);
	m_pMultiEditorButton->setIconSize(QSize(22,22));
	QIcon is2;
	is2.addPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TERMINAL)),QIcon::Normal,QIcon::On);
	is2.addPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TERMINAL)),QIcon::Normal,QIcon::Off);
	m_pMultiEditorButton->setIcon(is2);
	QString szTip = __tr2qs("Multi-line Editor<br>&lt;Alt+Backspace&gt;");
	szTip += " - &lt;Ctrl+Backspace&gt;";
	KviTalToolTip::add(m_pMultiEditorButton,szTip);

	connect(m_pMultiEditorButton,SIGNAL(toggled(bool)),this,SLOT(multilineEditorButtonToggled(bool)));

	m_pInputEditor = new KviInputEditor(this,pPar,pView);
	connect(m_pInputEditor,SIGNAL(enterPressed()),this,SLOT(inputEditorEnterPressed()));
	m_pInputEditor->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Ignored));


	m_pMultiEditorButton->setAutoRaise(true);
	m_pCommandlineModeButton->setAutoRaise(true);
	m_pIconButton->setAutoRaise(true);
	m_pHistoryButton->setAutoRaise(true);
	m_pHideToolsButton->setAutoRaise(true);
	
	m_pLayout->addWidget(m_pHideToolsButton,0);
	m_pLayout->addWidget(m_pButtonContainer,0);
	m_pLayout->addWidget(m_pInputEditor,10000);
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
	KviUserInput::parse(szText,m_pWindow,QString::null,m_pCommandlineModeButton->isChecked());
	m_pInputEditor->setText("");
}

void KviInput::keyPressEvent(QKeyEvent *e)
{
	//debug("KviInput::keyPressEvent(key:%d,state:%d,text:%s)",e->key(),e->state(),e->text().isEmpty() ? "empty" : e->text().toUtf8().data());

	if((e->modifiers() & Qt::ControlModifier) || (e->modifiers() & Qt::AltModifier) || (e->modifiers() & Qt::MetaModifier))
	{
		switch(e->key())
		{
			case Qt::Key_Backspace:
				multilineEditorButtonToggled(!m_pMultiLineEditor);
			break;
		}
	}

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
					KviUserInput::parse(szText,m_pWindow,QString::null,m_pCommandlineModeButton->isChecked());
					m_pMultiLineEditor->setText("");
				}
			}
			break;
			case Qt::Key_PageUp:
				historyButtonClicked();
			break;
		}
	}
}

void KviInput::multiLinePaste(const QString & szText)
{
	if(!m_pMultiLineEditor)
		multilineEditorButtonToggled(true);
	m_pMultiLineEditor->setText(szText);
}

void KviInput::multilineEditorButtonToggled(bool bOn)
{
	if(m_pMultiLineEditor)
	{
		if(bOn) return;
		QString szTmp;
		m_pMultiLineEditor->getText(szTmp);
		m_pLayout->removeWidget(m_pMultiLineEditor);
		KviScriptEditor::destroyInstance(m_pMultiLineEditor);
		m_pMultiLineEditor = 0;
		szTmp.replace(QRegExp("[\a\f\n\r\t\v]"), QString(" "));
		m_pInputEditor->setText(szTmp);
		m_pInputEditor->show();
		m_pWindow->childrenTreeChanged(0);
		m_pInputEditor->setFocus();
		m_pMultiEditorButton->setChecked(false);
	} else {
		if(!bOn) return;
		m_pMultiLineEditor = KviScriptEditor::createInstance(this);
		QString szText = __tr2qs("<Ctrl+Return>; submits, <Ctrl+Backspace>; hides this editor");
		m_pMultiLineEditor->setFindText(szText);
		m_pMultiLineEditor->setFindLineeditReadOnly(true);
		m_pMultiLineEditor->setText(m_pInputEditor->text());
		m_pInputEditor->hide();
		m_pLayout->addWidget(m_pMultiLineEditor);
		m_pWindow->childrenTreeChanged(m_pMultiLineEditor);
		m_pMultiLineEditor->setFocus();
		m_pMultiEditorButton->setChecked(true);
	}
}

void KviInput::iconButtonClicked()
{
	if(!g_pTextIconWindow)g_pTextIconWindow = new KviTextIconWindowWidget();
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

#define BUTTON_WIDTH 20

/*void KviInput::resizeEvent(QResizeEvent *e)
{
	//m_pButtonContainer
	m_pInputEditor->setGeometry(0,0,m_pButtonContainer->isVisible() ? width() - (BUTTON_WIDTH * 4)-10 : width() - 10,height());
	if(m_pMultiLineEditor)m_pMultiLineEditor->setGeometry(0,0,m_pButtonContainer->isVisible() ? width() - (BUTTON_WIDTH * 4)-10 : width() - 10,height());
	if(m_pButtonContainer->isVisible()) m_pButtonContainer->setGeometry(width() - (BUTTON_WIDTH * 4)-10,0,BUTTON_WIDTH*4,height());

	m_pHideToolsButton->setGeometry(width() - 10,0,10,height());

	QWidget::resizeEvent(e);
}*/

void KviInput::setFocus()
{
	// redirect setFocus() to the right children
	if(m_pMultiLineEditor)m_pMultiLineEditor->setFocus();
	else m_pInputEditor->setFocus();
}

void KviInput::focusInEvent(QFocusEvent * e)
{
	// if we get a focus in event , redirect the focus to the children
	if(m_pMultiLineEditor)m_pMultiLineEditor->setFocus();
	else m_pInputEditor->setFocus();
}

int KviInput::heightHint() const
{
	return m_pMultiLineEditor ? 120 : m_pInputEditor->heightHint();
}

void KviInput::setText(const QString & szText)
{
	// FIXME: Latin1 -> QString ?
	if(m_pMultiLineEditor)
		m_pMultiLineEditor->setText(szText);
	else m_pInputEditor->setText(szText);
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
	if(g_pLastFontMetrics) delete g_pLastFontMetrics;
	g_pLastFontMetrics = 0;

	if(KVI_OPTION_BOOL(KviOption_boolEnableInputHistory))//G&N mar 2005
	{
		QIcon is1;
		is1.addPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TIME)));
		m_pHistoryButton->setIcon(is1);
		KviTalToolTip::add(m_pHistoryButton,__tr2qs("Show History<br>&lt;Ctrl+PageUp&gt;"));
		connect(m_pHistoryButton,SIGNAL(clicked()),this,SLOT(historyButtonClicked()));
	} else {
		QIcon is1;
		is1.addPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_QUITSPLIT)));
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

//const QString & KviInput::text()
QString KviInput::text()
{
	QString szText;
	if(m_pMultiLineEditor)
		m_pMultiLineEditor->getText(szText);
	else
		szText = m_pInputEditor->text();
	return szText;
}
#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "kvi_input.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
