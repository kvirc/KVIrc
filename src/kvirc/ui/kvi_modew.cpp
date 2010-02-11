//============================================================================
//
//   File : kvi_modew.cpp
//   Creation date : Sat Nov 12 2005 23:50:12 by Alexey Uzhva
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2005-2008 Alexey Uzhva (wizard at opendoor dot ru)
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

#include "kvi_channel.h"
#include "kvi_options.h"
#include "kvi_ircconnectionserverinfo.h"
#include "kvi_ircconnectionuserinfo.h"
#include "kvi_tal_hbox.h"

#include <QEvent>
#include <QResizeEvent>
#include <QByteArray>

KviModeWidget::KviModeWidget(QWidget * par,KviChannel* chan,const char * name)
:KviThemedLineEdit(par, chan, name)
{
	m_pChannel=chan;
	connect(this,SIGNAL(textEdited ( const QString & ) ),this,SLOT(editorTextEdited( const QString & )));
	reset();
}

KviModeWidget::~KviModeWidget()
{
}

void KviModeWidget::reset()
{
	setText(m_pChannel->channelMode());
	setReadOnly(true);

	refreshModes();
	

	if(m_pChannel->input()) m_pChannel->setFocus();
}

void KviModeWidget::refreshModes()
{
	QString szMode=m_pChannel->channelMode();
	if(!m_pChannel->channelKey().isEmpty())
		szMode+=QString(" k:%1").arg(m_pChannel->channelKey());
	if(!m_pChannel->channelLimit().isEmpty())
		szMode+=QString(" l:%1").arg(m_pChannel->channelLimit().toUtf8().data());
	setText(szMode);
}

void KviModeWidget::mouseDoubleClickEvent(QMouseEvent *)
{
	if(m_pChannel->isMeHalfOp() ||
		m_pChannel->isMeOp() ||
		m_pChannel->isMeChanOwner() ||
		m_pChannel->isMeChanAdmin() ||
		m_pChannel->connection()->userInfo()->hasUserMode('o') ||
		m_pChannel->connection()->userInfo()->hasUserMode('O'))
	{
		setReadOnly(false);
	}
}

void KviModeWidget::keyReleaseEvent (QKeyEvent * e)
{
	switch(e->key())
	{
		case Qt::Key_Return:
		case Qt::Key_Enter:
			editorReturnPressed();
			break;
		case Qt::Key_Escape:
			reset();
			break;
		default:
			KviThemedLineEdit::keyReleaseEvent(e);
			break;
	}
}

void KviModeWidget::editorReturnPressed()
{
	QString szCurModes=m_pChannel->channelMode();
	QString szNewModes=text();
	QString szMinusModes;
	for(int i=0; i<szCurModes.length(); i++)
	{
		if(szNewModes.contains(szCurModes[i]))
			szNewModes.remove(szCurModes[i]);
		else
			szMinusModes+=szCurModes[i];
	}
	QString mode;
	if(!szMinusModes.isEmpty()) mode+=QString("-"+szMinusModes);
	if(!szNewModes.isEmpty()) mode+=QString("+"+szNewModes);
	if(!mode.isEmpty())
	{
		QByteArray chan = m_pChannel->connection()->encodeText(m_pChannel->target());
		m_pChannel->connection()->sendFmtData("MODE %s %s",chan.data(),mode.toUtf8().data());
	}
	reset();
}

void KviModeWidget::editorTextEdited(const QString & text)
{
	int i = 0;
	QString szText=text;
	for(i=0;i<szText.length();i++)
	{
		if( !m_pChannel->connection()->serverInfo()->supportedPlainModes().contains(szText[i]) ||
			szText.indexOf(szText[i])<i,Qt::CaseInsensitive )
			szText.remove(i,1);
	}
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "kvi_modew.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
