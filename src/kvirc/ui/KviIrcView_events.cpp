//===========================================================================
//
//   File : KviIrcView_events.cpp
//   Creation date : Wed Oct 1 2008 17:18:20 by Fabio Bas
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2008 Fabio Bas (ctrlaltca at gmail dot com)
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
//===========================================================================
//
// This file was originally part of KviIrcView.cpp
//
//===========================================================================

#include "KviChannelWindow.h"
#include "KviKvsEventTriggers.h"
#include "KviIconManager.h"
#include "KviIrcConnectionTarget.h"
#include "KviIrcUrl.h"
#include "KviIrcView.h"
#include "KviIrcView_private.h"
#include "KviIrcView_tools.h"
#include "KviLocale.h"
#include "KviControlCodes.h"
#include "KviOptions.h"
#include "kvi_out.h"
#include "KviTopicWidget.h"
#include "KviQueryWindow.h"
#include "KviWindow.h"

#include <QClipboard>
#include <QEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QUrl>

#define KVI_IRCVIEW_SELECT_REPAINT_INTERVAL 100

/*
	@doc: escape_sequences
	@title:
		Escape sequences and clickable links
	@type:
		generic
	@body:
		The KVIrc view widgets support clickable links.[br]
		The links can be created using special escape sequences in the text
		passed to the [cmd]echo[/cmd] command.[br]
		KVIrc uses some escape sequences in the text "echoed" internally.[br]
		The simplest way to explain it is to use an example:[br]
		[example]
			[cmd]echo[/cmd] This is a [fnc]$cr[/fnc]![!dbl][cmd]echo[/cmd] You have clicked it![fnc]$cr[/fnc]\clickable link$cr !
		[/example]
		The example above will show the following text line: "This is a clickable link".
		If you move the mouse over the words "clickable link", you will see the text highlighted.[br]
		Once you double-click one of that words, the command "[cmd]echo[/cmd] You have clicked it!" will be executed.[br]
		The format looks complex ?... it is not...just read on.[br]

		<cr>!<link_type><cr><visible text><cr>
		<cr>!<escape_command><cr><visible text><cr>

		[big]Escape format[/big]
		The whole escape sequence format is the following:[br]
		[b]<cr>!<escape_command><cr><visible text><cr>[/b][br]
		<cr> is the carriage return character. You can obtain it by using the [fnc]]$cr[/fnc] function.[br]
		<visible text> is the text that will appear as "link" when you move the mouse over it.[br]
		<escape_command> is the description of the actions to be taken when the user interacts with the link.[br]
		<escape_command> has the two following syntactic forms:[br]
		[b]<escape_command> ::= <user_defined_commands>[/b][br]
		[b]<escape_command> ::= <builtin_link_description>[/b]

		[big]User defined links[/big][br]
		The user defined links allow you to perform arbitrary commands when the user interacts with the link.[br]
		The commands are specified in the <escape_command> part by using the following syntax:[br]
		<escape_command> ::= <user_defined_commands>[br]
		<user_defined_commands> ::= <command_rule> [<user_defined_commands>][br]
		<command_rule> ::= <action_tag><command>[br]
		<action_tag> ::= "[!" <action> "]"[br]
		<action> ::= "rbt" | "mbt" | "dbl" | "txt"[br]
		<command> ::= any kvirc command (see notes below)[br]

		[big]A shortcut[/big]
		You may have a look at the [fnc]$fmtlink[/fnc] function: it does automatically some of the job explained
		in this document.[br]

*/

// FIXME: #warning "Finish the doc above!! Maybe some examples ?!"


//mouse events
void KviIrcView::mouseDoubleClickEvent(QMouseEvent *e)
{
	QString szKvsCommand;
	QString szLinkCommandPart;
	QString szLinkTextPart;

	if(m_iMouseTimer)
	{
		killTimer(m_iMouseTimer);
		m_iMouseTimer=0;
		delete m_pLastEvent;
		m_pLastEvent = 0;
	}

	getLinkUnderMouse(e->pos().x(),e->pos().y(),0,&szLinkCommandPart,&szLinkTextPart);

	if(szLinkCommandPart.isEmpty())
	{
		KVS_TRIGGER_EVENT_0(KviEvent_OnTextViewDoubleClicked,m_pKviWindow);
		return;
	}

	KviKvsVariantList lParams;
	lParams.append(szLinkTextPart);

	switch(szLinkCommandPart[0].unicode())
	{
		case 'n':
		{
			switch(m_pKviWindow->type())
			{
				case KviWindow::Channel:
					if(((KviChannelWindow *)m_pKviWindow)->isOn(szLinkTextPart))
					{
						KVS_TRIGGER_EVENT(KviEvent_OnChannelNickDefaultActionRequest,m_pKviWindow,&lParams);
						return;
					}
				break;
				case KviWindow::Query:
					if(KviQString::equalCI(((KviQueryWindow *)m_pKviWindow)->windowName(),szLinkTextPart))
					{
						KVS_TRIGGER_EVENT(KviEvent_OnQueryNickDefaultActionRequest,m_pKviWindow,&lParams);
						return;
					}
				break;
				default:
					return; // unhandled window type (FIXME: Let it go anyway ?)
				break;
			}
			if(console())
				KVS_TRIGGER_EVENT(KviEvent_OnNickLinkDefaultActionRequest,m_pKviWindow,&lParams);
			return;
		}
		break;
		case 'm': // m+X / m-X (used to quickly undo mode changes)
		{
			if(szLinkCommandPart.length() < 3)
				return; // malformed
			if(m_pKviWindow->type() != KviWindow::Channel)
				return; // must be on a channel to apply it
			if(!(((KviChannelWindow *)m_pKviWindow)->isMeOp()))
				return; // i'm not op, can't do mode changes

			QChar cPlusOrMinus = szLinkCommandPart[1];
			if((cPlusOrMinus.unicode() != '+') && (cPlusOrMinus.unicode() != '-'))
				return; // malformed

			QChar cFlag = szLinkCommandPart[2];
			switch(cFlag.unicode())
			{
				case 'o':
				case 'v':
					return; // We can do nothing here... (FIXME: Can't remember why...)
				break;

				case 'b':
				case 'I':
				case 'e':
				case 'q':
				case 'f':
				case 'k':
				default:
					szKvsCommand = QString("mode $chan.name %1%2 $0").arg(cPlusOrMinus.toLatin1()).arg(cFlag.toLatin1());
				break;
			}
		}
		break;
		case 'h':
			m_pKviWindow->output(KVI_OUT_HOSTLOOKUP,__tr2qs("Looking up host %Q..."),&szLinkTextPart);
			szKvsCommand = "host -a $0";
		break;
		case 'u':
			if(KVI_OPTION_UINT(KviOption_uintUrlMouseClickNum) == 2) // <-- ??????????
			{
				KVS_TRIGGER_EVENT(KviEvent_OnURLLinkClick,m_pKviWindow,&lParams);
				return;
			}
		break;
		case 'c':
		{
			if(!console())
				return;
			if(!console()->connection())
				return;

			if(KviChannelWindow * c = console()->connection()->findChannel(szLinkTextPart))
			{
				// already there
				c->raise();
				c->setFocus();
				return;
			}

			szKvsCommand = "join $0";
		}
		break;
		case 's':
			szKvsCommand = "motd $0";
		break;
		default:
		{
			// extract the user-supplied double click command
			getLinkEscapeCommand(szKvsCommand,szLinkCommandPart,"[!dbl]");
			if(szKvsCommand.isEmpty())
			{
				KVS_TRIGGER_EVENT_0(KviEvent_OnTextViewDoubleClicked,m_pKviWindow);
				return;
			}
		}
		break;
	}

	if(!szKvsCommand.isEmpty())
		KviKvsScript::run(szKvsCommand,m_pKviWindow,&lParams);
}

void KviIrcView::mousePressEvent(QMouseEvent * e)
{
	if(m_pKviWindow->input())
		m_pKviWindow->input()->setFocus();

	if(!(e->button() & Qt::LeftButton))
	{
		triggerMouseRelatedKvsEvents(e);
		return;
	}
	
	// Left button handler

	// We are inside the line marker
	if(checkMarkerArea(m_lineMarkArea,e->pos()))
	{
		scrollToMarker();
	}

	// This is the beginning of a selection...
	// We just set the mouse to be "down" and
	// await mouseMove events...

	m_pSelectionInitLine = getVisibleLineAt(e->pos().y());
	m_pSelectionEndLine = m_pSelectionInitLine;
	if(m_pSelectionInitLine)
	{
		m_iSelectionInitCharIndex = getVisibleCharIndexAt(m_pSelectionInitLine, e->pos().x(), e->pos().y());
		m_iSelectionEndCharIndex=m_iSelectionInitCharIndex;
	}

	if(m_pToolWidget)
	{
		m_pCursorLine = m_pSelectionInitLine;
		repaint();
	}

	m_mousePressPos   = e->pos();
	m_mouseCurrentPos = e->pos();

	m_bMouseIsDown = true;

	m_bShiftPressed = (e->modifiers() & Qt::ShiftModifier);

	if(m_iMouseTimer) // clicked at least twice within the doubleClickInterval(): this is a double click then...
	{
		killTimer(m_iMouseTimer);
		m_iMouseTimer=0;
		delete m_pLastEvent;
		m_pLastEvent = 0;
	} else {
		// This is used to avoid triggering the single click KVS events
		// if a double click is issued instead...
		// We actually trigger the click event after the double click interval
		// is elapsed without a second click.
		m_iMouseTimer = startTimer(QApplication::doubleClickInterval());
		m_pLastEvent = new QMouseEvent(*e);
	}
}

void KviIrcView::triggerMouseRelatedKvsEvents(QMouseEvent *e)
{
	QString linkCmd;
	QString linkText;
	getLinkUnderMouse(e->pos().x(),e->pos().y(),0,&linkCmd,&linkText);

	QString szCmd(linkCmd);
	szCmd.remove(0,1);

	KviKvsVariantList * pParams = new KviKvsVariantList();

	if(!szCmd.isEmpty()) pParams->append(szCmd); // <-- FIXME: why we do this ?
	else pParams->append(linkText); // <-- FIXME: why we do this ?

	pParams->append(linkText);
	pParams->append(szCmd);

	if(!(e->modifiers() & Qt::ControlModifier))//(e->button() & Qt::RightButton) && (
	{
		if(!linkCmd.isEmpty())
		{
			switch(linkCmd[0].unicode())
			{
				case 'n':
					{
						bool bTrigger = false;
						switch(m_pKviWindow->type())
						{
							case KviWindow::Channel:
								if(((KviChannelWindow *)m_pKviWindow)->isOn(linkText))
								{
									if(e->button() & Qt::RightButton)
										KVS_TRIGGER_EVENT(KviEvent_OnChannelNickPopupRequest,m_pKviWindow,pParams);
									if(e->button() & Qt::LeftButton) {
										KVS_TRIGGER_EVENT(KviEvent_OnChannelNickLinkClick,m_pKviWindow,pParams);
									}
								} else bTrigger = true;
							break;
							case KviWindow::Query:
								if(KviQString::equalCI(((KviQueryWindow *)m_pKviWindow)->windowName(),linkText))
								{
									if(e->button() & Qt::RightButton)
										KVS_TRIGGER_EVENT(KviEvent_OnQueryNickPopupRequest,m_pKviWindow,pParams);
									if(e->button() & Qt::LeftButton)
										KVS_TRIGGER_EVENT(KviEvent_OnQueryNickLinkClick,m_pKviWindow,pParams);
								} else bTrigger = true;
							break;
							default:
								bTrigger = true;
						break;
						}
						if(bTrigger)
						{
							if(console())
							{
								if(e->button() & Qt::RightButton)
									KVS_TRIGGER_EVENT(KviEvent_OnNickLinkPopupRequest,m_pKviWindow,pParams);
								if(e->button() & Qt::LeftButton)
									KVS_TRIGGER_EVENT(KviEvent_OnConsoleNickLinkClick,m_pKviWindow,pParams);
							} else emit rightClicked();
						}
					}
				break;
				case 'h':
					if(e->button() & Qt::RightButton)
						KVS_TRIGGER_EVENT(KviEvent_OnHostLinkPopupRequest,m_pKviWindow,pParams);
					if(e->button() & Qt::LeftButton)
						KVS_TRIGGER_EVENT(KviEvent_OnHostLinkClick,m_pKviWindow,pParams);
				break;
				case 'u':
					if(e->button() & Qt::RightButton)
						KVS_TRIGGER_EVENT(KviEvent_OnURLLinkPopupRequest,m_pKviWindow,pParams);
					if(e->button() & Qt::LeftButton && KVI_OPTION_UINT(KviOption_uintUrlMouseClickNum) == 1)
						KVS_TRIGGER_EVENT(KviEvent_OnURLLinkClick,m_pKviWindow,pParams);
				break;
				case 'c':
					if(e->button() & Qt::RightButton)
						KVS_TRIGGER_EVENT(KviEvent_OnChannelLinkPopupRequest,m_pKviWindow,pParams);
					if(e->button() & Qt::LeftButton)
						KVS_TRIGGER_EVENT(KviEvent_OnChannelLinkClick,m_pKviWindow,pParams);
				break;
				case 's':
					if(e->button() & Qt::RightButton)
						KVS_TRIGGER_EVENT(KviEvent_OnServerLinkPopupRequest,m_pKviWindow,pParams);
					if(e->button() & Qt::LeftButton)
						KVS_TRIGGER_EVENT(KviEvent_OnServerLinkClick,m_pKviWindow,pParams);
				break;
				default:
				{
					if(e->button() & Qt::RightButton)
					{
						QString tmp;
						getLinkEscapeCommand(tmp,linkCmd,"[!rbt]");
						if(!tmp.isEmpty())
						{
							KviKvsScript::run(tmp,m_pKviWindow,pParams);
						} else emit rightClicked();
					}
				}
				break;
			}
		} else if(e->button() & Qt::RightButton) emit rightClicked();

	} else if((e->button() & Qt::MidButton) || ((e->button() & Qt::RightButton) && (e->modifiers() & Qt::ControlModifier)))
	{
		QString tmp;
		getLinkEscapeCommand(tmp,linkCmd,QString("[!mbt]"));
		if(!tmp.isEmpty())
		{
			KviKvsScript::run(tmp,m_pKviWindow,pParams);
		} else {
			KVS_TRIGGER_EVENT_0(KviEvent_OnWindowPopupRequest,m_pKviWindow);
		}
	}
	delete pParams;
}

void KviIrcView::addControlCharacter(KviIrcViewLineChunk *pC, QString & szSelectionText)
{
	switch(pC->type)
	{
		case KviControlCodes::Bold:
		case KviControlCodes::Underline:
		case KviControlCodes::Reverse:
		case KviControlCodes::Reset:
			szSelectionText.append(QChar(pC->type));
		break;
		case KviControlCodes::Color:
			szSelectionText.append(QChar(pC->type));
			if((pC->colors.fore != KviControlCodes::NoChange) && (pC->colors.fore != KviControlCodes::Transparent))
			{
				if(pC->colors.fore > 9)szSelectionText.append(QChar('1'));
				szSelectionText.append(QChar((pC->colors.fore%10)+'0'));
			}
			if((pC->colors.back != KviControlCodes::NoChange) && (pC->colors.back != KviControlCodes::Transparent) )
			{
				szSelectionText.append(QChar(','));
				if(pC->colors.back > 9)szSelectionText.append(QChar('1'));
				szSelectionText.append(QChar((pC->colors.back%10)+'0'));
			}
		break;
	}
}

void KviIrcView::mouseReleaseEvent(QMouseEvent *e)
{
	if(m_pSelectionInitLine)
	{
		killTimer(m_iSelectTimer);
		m_iSelectTimer = 0;

		KviIrcViewLine *tempLine=getVisibleLineAt(e->pos().y());
		if(tempLine)
		{
			m_pSelectionEndLine = tempLine;
			int iTmp=getVisibleCharIndexAt(m_pSelectionEndLine, e->pos().x(), e->pos().y());
			if(iTmp > -1)
				m_iSelectionEndCharIndex = iTmp;
		}

		//check if selection is bottom to top or viceversa
		KviIrcViewLine *init, *end;
		int initChar, endChar;
		if(m_pSelectionInitLine->uIndex == m_pSelectionEndLine->uIndex)
		{
			init=m_pSelectionInitLine;
			end=m_pSelectionEndLine;
			if(m_iSelectionInitCharIndex<=m_iSelectionEndCharIndex)
			{
				//one line ltor selection
				initChar=m_iSelectionInitCharIndex;
				endChar=m_iSelectionEndCharIndex;
			} else {
				//one line rtol selection
				initChar=m_iSelectionEndCharIndex;
				endChar=m_iSelectionInitCharIndex;
			}
		} else if(m_pSelectionInitLine->uIndex < m_pSelectionEndLine->uIndex)
		{
			//multi line uptobottom selection
			init=m_pSelectionInitLine;
			end=m_pSelectionEndLine;
			initChar=m_iSelectionInitCharIndex;
			endChar=m_iSelectionEndCharIndex;
		} else {
			//multi line bottomtotop selection
			end=m_pSelectionInitLine;
			init=m_pSelectionEndLine;
			initChar=m_iSelectionEndCharIndex;
			endChar=m_iSelectionInitCharIndex;
		}

		tempLine = init;
		QString szSelectionText;
		while(tempLine)
		{
			if(tempLine->uIndex == init->uIndex)
			{
				if(tempLine->uIndex == end->uIndex)
				{
					//selection starts and ends in this line
					if(m_bShiftPressed)
					{
						bool bStarted=false;
						KviIrcViewLineChunk *pC;
						for(unsigned int i=0;i<tempLine->uChunkCount; i++)
						{
							pC = &tempLine->pChunks[i];
							if(bStarted)
							{
								if(endChar >= (pC->iTextStart + pC->iTextLen))
								{
									//the entire chunk is included
									addControlCharacter(pC, szSelectionText);
									szSelectionText.append(tempLine->szText.mid(pC->iTextStart, pC->iTextLen));
								} else {
									//ends in this chunk
									addControlCharacter(pC, szSelectionText);
									szSelectionText.append(tempLine->szText.mid(pC->iTextStart, endChar-pC->iTextStart));
									break;
								}
							} else {
								if(initChar <= (pC->iTextStart + pC->iTextLen))
								{
									//starts in this chunk
									addControlCharacter(pC, szSelectionText);
									if((endChar-initChar) > pC->iTextLen)
									{
										//don't end in this chunk
										szSelectionText.append(tempLine->szText.mid(initChar, pC->iTextLen-(initChar-pC->iTextStart)));
										bStarted=true;
									} else {
										//ends in this chunk
										szSelectionText.append(tempLine->szText.mid(initChar, endChar-initChar));
										break;
									}
								}
							}
						}
					} else {
						szSelectionText.append(tempLine->szText.mid(initChar, endChar-initChar));
					}
					break;
				} else {
					// the first line of a multi line selection
					if(m_bShiftPressed)
					{
						bool bStarted=false;
						KviIrcViewLineChunk *pC;
						for(unsigned int i=0;i<tempLine->uChunkCount; i++)
						{
							pC = &tempLine->pChunks[i];
							if(bStarted)
							{
								//the entire chunk is included
								addControlCharacter(pC, szSelectionText);
								szSelectionText.append(tempLine->szText.mid(pC->iTextStart, pC->iTextLen));
							} else {
								if(initChar <= (pC->iTextStart + pC->iTextLen))
								{
									//starts in this chunk
									addControlCharacter(pC, szSelectionText);
									szSelectionText.append(tempLine->szText.mid(initChar, pC->iTextLen-(initChar-pC->iTextStart)));
									bStarted=true;
								}
							}
						}
					} else {
						szSelectionText.append(tempLine->szText.mid(initChar));
					}
					szSelectionText.append("\n");
				}
			} else {
				if(tempLine->uIndex == end->uIndex)
				{
					// the last line of a multi line selection
					if(m_bShiftPressed)
					{
						KviIrcViewLineChunk *pC;
						for(unsigned int i=0;i<tempLine->uChunkCount; i++)
						{
							pC = &tempLine->pChunks[i];
							if(endChar >= (pC->iTextStart + pC->iTextLen))
							{
								//the entire chunk is included
								addControlCharacter(pC, szSelectionText);
								szSelectionText.append(tempLine->szText.mid(pC->iTextStart, pC->iTextLen));
							} else {
								//ends in this chunk
								addControlCharacter(pC, szSelectionText);
								szSelectionText.append(tempLine->szText.mid(pC->iTextStart, endChar-pC->iTextStart));
								break;
							}
						}
					} else {
						szSelectionText.append(tempLine->szText.left(endChar));
					}
					break;
				} else {
					//a middle line of a multi line selection
					if(m_bShiftPressed)
					{
						KviIrcViewLineChunk *pC;
						for(unsigned int i=0;i<tempLine->uChunkCount; i++)
						{
							pC = &tempLine->pChunks[i];
							//the entire chunk is included
							addControlCharacter(pC, szSelectionText);
							szSelectionText.append(tempLine->szText.mid(pC->iTextStart, pC->iTextLen));
						}
					} else {
						szSelectionText.append(tempLine->szText);
					}
					szSelectionText.append("\n");
				}
			}
			tempLine = tempLine->pNext;
		}

		QClipboard * c = QApplication::clipboard();
		if(c && !szSelectionText.isEmpty())
		{
			// copy to both!
			c->setText(szSelectionText,QClipboard::Clipboard);
			if(c->supportsSelection())
				c->setText(szSelectionText,QClipboard::Selection);
		}
		m_pSelectionInitLine = 0;
		m_pSelectionEndLine = 0;
		m_iSelectionInitCharIndex=0;
		m_iSelectionEndCharIndex=0;
	}

	if(m_bMouseIsDown)
	{
		m_bMouseIsDown = false;
		m_bShiftPressed = false;
		// Insert the lines blocked while selecting
		while(KviIrcViewLine * l = m_pMessagesStoppedWhileSelecting->first())
		{
			m_pMessagesStoppedWhileSelecting->removeFirst();
			appendLine(l,false);
		}
		repaint();
	}
}

// FIXME: #warning "The tooltip timeout should be small, because the view scrolls!"

void KviIrcView::mouseMoveEvent(QMouseEvent * e)
{
	bool bCursorOverMarker = checkMarkerArea(m_lineMarkArea,e->pos());

	if(m_bMouseIsDown && (e->buttons() & Qt::LeftButton)) // m_bMouseIsDown MUST BE true...(otherwise the mouse entered the window with the button pressed ?)
	{
		if(m_iSelectTimer == 0)
			m_iSelectTimer = startTimer(KVI_IRCVIEW_SELECT_REPAINT_INTERVAL);

		//scroll the ircview if the user is trying to extend a selection near the ircview borders
		int curY = e->pos().y();
		if(curY < KVI_IRCVIEW_VERTICAL_BORDER)
		{
			prevLine();
		} else if(curY > (height() - KVI_IRCVIEW_VERTICAL_BORDER))
		{
			nextLine();
		}

		KviIrcViewLine *tempLine=getVisibleLineAt(e->pos().y());
		if(tempLine)
		{
			m_pSelectionEndLine = tempLine;
			int iTmp=getVisibleCharIndexAt(m_pSelectionEndLine, e->pos().x(), e->pos().y());
			if(iTmp > -1)
				m_iSelectionEndCharIndex = iTmp;
		}

		return;
	}

	if(m_iSelectTimer)
	{
		killTimer(m_iSelectTimer);
		m_iSelectTimer = 0;
	}

	int yPos = e->pos().y();
	int rectTop;
	int rectHeight;
	QRect rctLink;
	KviIrcViewWrappedBlock * newLinkUnderMouse = getLinkUnderMouse(e->pos().x(),yPos,&rctLink);

	rectTop = rctLink.y();
	rectHeight = rctLink.height();

	if(newLinkUnderMouse != m_pLastLinkUnderMouse)
	{
		m_pLastLinkUnderMouse = newLinkUnderMouse;
		if(m_pLastLinkUnderMouse)
		{
			if(rectTop < 0)rectTop = 0;
			if((rectTop + rectHeight) > height())rectHeight = height() - rectTop;

			if(m_iLastLinkRectHeight > -1)
			{
				// prev link
				int top = (rectTop < m_iLastLinkRectTop) ? rectTop : m_iLastLinkRectTop;
				int lastBottom = m_iLastLinkRectTop + m_iLastLinkRectHeight;
				int thisBottom = rectTop + rectHeight;
				QRect r(0,top,width(),((lastBottom > thisBottom) ? lastBottom : thisBottom) - top);
				repaint(r);
			} else {
				// no prev link
				QRect r(0,rectTop,width(),rectHeight);
				repaint(r);
			}
			m_iLastLinkRectTop = rectTop;
			m_iLastLinkRectHeight = rectHeight;
		} else {
			if(m_iLastLinkRectHeight > -1)
			{
				// There was a previous bottom rect
				QRect r(0,m_iLastLinkRectTop,width(),m_iLastLinkRectHeight);
				repaint(r);
				m_iLastLinkRectTop = -1;
				m_iLastLinkRectHeight = -1;
			}
		}
	}

	if(bCursorOverMarker || m_pLastLinkUnderMouse)
		setCursor(Qt::PointingHandCursor);
	else
		setCursor(Qt::ArrowCursor);
}

void KviIrcView::leaveEvent(QEvent * )
{
	if(m_pLastLinkUnderMouse)
	{
		 m_pLastLinkUnderMouse = 0;
		 update();
	}
}

void KviIrcView::timerEvent(QTimerEvent *e)
{
	m_mouseCurrentPos = mapFromGlobal(QCursor::pos());

	if(e->timerId() == m_iSelectTimer)
	{
		repaint();
		return;
	}

	if(e->timerId() == m_iMouseTimer)
	{
		killTimer(m_iMouseTimer);
		m_iMouseTimer=0;
		triggerMouseRelatedKvsEvents(m_pLastEvent);
		delete m_pLastEvent;
		m_pLastEvent=0;
		return;
	}

	if(e->timerId() == m_iFlushTimer)
	{
		flushLog();
		return;
	}
}

//not exactly events, but event-related

void KviIrcView::maybeTip(const QPoint &pnt)
{
	QString linkCmd;
	QString linkText;
	QRect rctLink;
	QRect markerArea;

	// Check if the mouse is over the marker icon
	// 16(width) + 5(border) = 21
	int widgetWidth = width()-m_pScrollBar->width();
	int x = widgetWidth - 21;
	int y = KVI_IRCVIEW_VERTICAL_BORDER;

	markerArea = QRect(QPoint(x,y),QSize(16,16));
	if(checkMarkerArea(markerArea,pnt))
		doMarkerToolTip(markerArea);

	// Check if the mouse is over a link
	KviIrcViewWrappedBlock * linkUnderMouse = getLinkUnderMouse(pnt.x(),pnt.y(),&rctLink,&linkCmd,&linkText);

	if((linkUnderMouse == m_pLastLinkUnderMouse) && linkUnderMouse)
		doLinkToolTip(rctLink,linkCmd,linkText);
	else m_pLastLinkUnderMouse = 0;
}

void KviIrcView::doMarkerToolTip(const QRect &rct)
{
	QString tip;
	tip = "<table width=\"100%\">" \
		"<tr><td valign=\"center\"><img src=\"" + g_pIconManager->getSmallIconResourceName(KviIconManager::UnreadText) + "\"> <u><font color=\"blue\"><nowrap>";
	tip += __tr2qs("Scroll up to read from the last read line");
	tip += "</nowrap></font></u></td></tr><tr><td>";
	tip += "</td></tr></table>";

	if(tip.isEmpty())return;

	m_pToolTip->doTip(rct,tip);
}

void KviIrcView::doLinkToolTip(const QRect &rct,QString &linkCmd,QString &linkText)
{
	if(linkCmd.isEmpty())return;

	QString szCmd(linkCmd);
	szCmd.remove(0,1);

	QString tip;

	switch(linkCmd[0].unicode())
	{
		case 'u': // url link
		{
			if(!KVI_OPTION_BOOL(KviOption_boolEnableUrlLinkToolTip))
				return;
			tip = "<table width=\"100%\">" \
				"<tr><td valign=\"center\"><img src=\"" + g_pIconManager->getSmallIconResourceName(KviIconManager::Url) + "\"> <u><font color=\"blue\"><nowrap>";
			if(linkText.length() > 50)
			{
				tip += linkText.left(47);
				tip += "...";
			} else {
				tip += linkText;
			}
			tip+="</nowrap></font></u></td></tr><tr><td>";

			// Check clicks' number
			if(KVI_OPTION_UINT(KviOption_uintUrlMouseClickNum) == 1)
				tip += __tr2qs("Click to open this link");
			else
				tip += __tr2qs("Double-click to open this link");
			tip += "</td></tr></table>";
		}
		break;
		case 'h': // host link
		{
			if(!KVI_OPTION_BOOL(KviOption_boolEnableHostLinkToolTip))
				return;
			tip = "<table width=\"100%\">" \
				"<tr><td valign=\"center\"><img src=\"" + g_pIconManager->getSmallIconResourceName(KviIconManager::Server) + "\"> <u><font color=\"blue\"><nowrap>";
			if(linkText.length() > 50)
			{
				tip += linkText.left(47);
				tip += "...";
			} else {
				tip += linkText;
			}
			tip+="</nowrap></font></u></td></tr><tr><td>";

			if(linkText.indexOf('*') != -1)
			{
				if(linkText.length() > 1)tip += __tr2qs("Unable to look it up hostname: Hostname appears to be masked");
				else tip += __tr2qs("Unable to look it up hostname: Unknown host");
			} else {
				tip += __tr2qs("Double-click to look up this hostname<br>Right-click to view other options");
			}
			tip += "</td></tr></table>";
		}
		break;
		case 's': // server link
		{
			if(!KVI_OPTION_BOOL(KviOption_boolEnableServerLinkToolTip))
				return;
			// FIXME: #warning "Spit out some server info...hub ?...registered ?"

			tip = "<table width=\"100%\">" \
				"<tr><td valign=\"center\"><img src=\"" + g_pIconManager->getSmallIconResourceName(KviIconManager::Irc) + "\"> <u><font color=\"blue\"><nowrap>";

			if(linkText.length() > 50)
			{
				tip += linkText.left(47);
				tip += "...";
			} else {
				tip += linkText;
			}
			tip+="</nowrap></font></u></td></tr><tr><td>";

			if(linkText.indexOf('*') != -1)
			{
				if(linkText.length() > 1)tip += __tr2qs("Server appears to be a network hub<br>");
				else tip += __tr2qs("Unknown server<br>"); // might happen...
			}

			tip.append(__tr2qs("Double-click to read the MOTD<br>Right-click to view other options"));
			tip += "</td></tr></table>";
		}
		break;
		case 'm': // mode link
		{
			if(!KVI_OPTION_BOOL(KviOption_boolEnableModeLinkToolTip))
				return;
			if((linkCmd.length() > 2) && (m_pKviWindow->type() == KviWindow::Channel))
			{
				if(((KviChannelWindow *)m_pKviWindow)->isMeOp())
				{
					QChar plmn = linkCmd[1];
					if((plmn.unicode() == '+') || (plmn.unicode() == '-'))
					{
						tip = __tr2qs("Double-click to set<br>");
						QChar flag = linkCmd[2];
						switch(flag.unicode())
						{
							case 'o':
							case 'v':
								// We can do nothing here...
								tip = "";
							break;
							case 'b':
							case 'I':
							case 'e':
							case 'q':
							case 'f':
							case 'k':
								KviQString::appendFormatted(tip,QString("<b>mode %Q %c%c %Q</b>"),&(m_pKviWindow->windowName()),plmn.toLatin1(),flag.toLatin1(),&linkText);
							break;
							default:
								KviQString::appendFormatted(tip,QString("<b>mode %Q %c%c</b>"),&(m_pKviWindow->windowName()),plmn.toLatin1(),flag.toLatin1());
							break;
						}
					}
				} else {
					// I'm not op...no way
					tip = __tr2qs("You're not an operator: You may not change channel modes");
				}
			}
		}
		break;
		case 'n': // nick link
		{
			if(!KVI_OPTION_BOOL(KviOption_boolEnableNickLinkToolTip))
				return;
			if(console())
			{
				if(console()->connection())
				{
					KviIrcUserEntry * e = console()->connection()->userDataBase()->find(linkText);
					if(e)
					{
						QString buffer;
						console()->getUserTipText(linkText,e,buffer);
						tip = buffer;
					} else tip = QString(__tr2qs("Nothing known about %1")).arg(linkText);
				} else tip = QString(__tr2qs("Nothing known about %1 (no connection)")).arg(linkText);
			}
		}
		break;
		case 'c': // channel link
		{
			if(!KVI_OPTION_BOOL(KviOption_boolEnableChannelLinkToolTip))
				return;
			if(console() && console()->connection())
			{
				QString szChan = linkText;
				QString buf;
				tip = "<img src=\"" + g_pIconManager->getSmallIconResourceName(KviIconManager::Channel) + "\"> ";

				if(szCmd.length()>0) szChan=szCmd;
				KviChannelWindow * c = console()->connection()->findChannel(szChan);
				QString szUrl;
				if(c)
				{
					QString chanMode;
					c->getChannelModeString(chanMode);
					QString topic = KviControlCodes::stripControlBytes(c->topicWidget()->topic());
					topic.replace("<","&lt;");
					topic.replace(">","&gt;");
					KviIrcUrl::join(szUrl,console()->connection()->target()->server());
					szUrl.append(szChan);
					buf = QString(__tr2qs("<b>%1</b> (<u><font color=\"blue\"><nowrap>"
						"%2</nowrap></font></u>): <br><nowrap>+%3 (%4 users)<hr>%5</nowrap>")).arg(szChan,szUrl,chanMode).arg(c->count()).arg(topic);
				} else {
					KviIrcUrl::join(szUrl,console()->connection()->target()->server());
					szUrl.append(szChan);
					buf = QString(__tr2qs("<b>%1</b> (<u><font color=\"blue\"><nowrap>"
						"%2</nowrap></font></u>)<hr>Double-click to join %3<br>Right click to view other options")).arg(szChan,szUrl,szChan);
				}

				tip += buf;
			}
		}
		break;
		default:
		{
			if(!KVI_OPTION_BOOL(KviOption_boolEnableEscapeLinkToolTip))
				return;
			QString dbl,rbt,txt,mbt;
			getLinkEscapeCommand(dbl,linkCmd,"[!dbl]");
			getLinkEscapeCommand(rbt,linkCmd,"[!rbt]");
			getLinkEscapeCommand(txt,linkCmd,"[!txt]");
			getLinkEscapeCommand(mbt,linkCmd,"[!mbt]");

			if(!txt.isEmpty())tip = txt;
			if(tip.isEmpty() && (!dbl.isEmpty()))
			{
				if(!tip.isEmpty())tip.append("<hr>");
				KviQString::appendFormatted(tip,__tr2qs("<b>Double-click:</b><br>%Q"),&dbl);
			}
			if(tip.isEmpty() && (!mbt.isEmpty()))
			{
				if(!tip.isEmpty())tip.append("<hr>");
				KviQString::appendFormatted(tip,__tr2qs("<b>Middle-click:</b><br>%Q"),&mbt);
			}
			if(tip.isEmpty() && (!rbt.isEmpty()))
			{
				if(!tip.isEmpty())tip.append("<hr>");
				KviQString::appendFormatted(tip,__tr2qs("<b>Right-click:</b><br>%Q"),&rbt);
			}
		}
		break;
	}

	if(tip.isEmpty())return;

	m_pToolTip->doTip(rct,tip);
}

//keyboard events
void KviIrcView::keyPressEvent(QKeyEvent *e)
{
	switch(e->key())
	{
		case Qt::Key_PageUp:
			prevPage();
			e->accept();
			break;
		case Qt::Key_PageDown:
			nextPage();
			e->accept();
			break;
		default:
			e->ignore();
	}
}

//drag&drop events
void KviIrcView::dragEnterEvent(QDragEnterEvent *e)
{
	if(!m_bAcceptDrops)return;
	if(e->mimeData()->hasUrls()) e->acceptProposedAction();
	emit dndEntered();
}

void KviIrcView::dropEvent(QDropEvent *e)
{
	if(!m_bAcceptDrops)return;
	QList<QUrl> list;
	if(e->mimeData()->hasUrls())
	{
		list = e->mimeData()->urls();
		if(!list.isEmpty())
		{
			QList<QUrl>::Iterator it = list.begin();
			for( ; it != list.end(); ++it )
			{
				QUrl url = *it;
				QString path = url.toLocalFile();
				emit fileDropped(path);
			}
		}
	}
}

//

bool KviIrcView::event(QEvent *e)
{
	if(e->type() == QEvent::User)
	{
		KVI_ASSERT(m_bPostedPaintEventPending);
		if(m_iUnprocessedPaintEventRequests)
			repaint();
		// else we just had a pointEvent that did the job
		m_bPostedPaintEventPending = false;
		return true;
	}
	return QWidget::event(e);
}

void KviIrcView::wheelEvent(QWheelEvent *e)
{
	static bool bHere = false;
	if(bHere)return;
	bHere = true; // Qt4 tends to jump into infinite recursion here
	g_pApp->sendEvent(m_pScrollBar,e);
	bHere = false;
}

