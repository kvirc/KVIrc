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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//============================================================================


#include "kvi_themedlabel.h"
#include "kvi_channel.h"
#include "kvi_options.h"
#include "kvi_ircconnectionserverinfo.h"
#include "kvi_ircconnectionuserinfo.h"
#include "kvi_qcstring.h"
#include "kvi_tal_hbox.h"

#include <QFrame>
#include <QLineEdit>
#include <QEvent>
#include <QResizeEvent>

KviModeWidget::KviModeWidget(QWidget * par,KviChannel* chan,const char * name)
:QFrame(par)
{
	setObjectName(name);
	m_pChannel=chan;
	m_pLabel=0;
	m_pLineEdit=0;
	setAutoFillBackground(false);
	reset();
}

KviModeWidget::~KviModeWidget()
{
}

void KviModeWidget::reset()
{
	if(m_pLineEdit)
	{
		delete m_pLineEdit;
		m_pLineEdit=0;
	}
	if(!m_pLabel)
		m_pLabel=new KviThemedLabel(this,0);
	refreshModes();
	m_pLabel->show();
	connect(m_pLabel,SIGNAL(doubleClicked()),this,SLOT(labelDoubleClick()));
	QResizeEvent* ev=new QResizeEvent(size(),size());
	resizeEvent(ev);
	delete ev;
	if(m_pChannel->input()) m_pChannel->setFocus();
}

void KviModeWidget::refreshModes()
{
	QString szMode=m_pChannel->channelMode();
	if(!m_pChannel->channelKey().isEmpty())
		szMode+=QString(" k:%1").arg(m_pChannel->channelKey());
	if(!m_pChannel->channelLimit().isEmpty())
		szMode+=QString(" l:%1").arg(m_pChannel->channelLimit().toUtf8().data());
	if(m_pLabel)
		m_pLabel->setText(szMode);
}

void KviModeWidget::applyOptions()
{
	if(m_pLabel) m_pLabel->applyOptions();
}

void KviModeWidget::resizeEvent(QResizeEvent *e)
{
	if(e)QFrame::resizeEvent(e);
	if(m_pLabel) m_pLabel->setGeometry(0,0,width(),height());
	if(m_pLineEdit) m_pLineEdit->setGeometry(0,0,width(),height());
}

void KviModeWidget::labelDoubleClick()
{
	if(m_pLabel && ( m_pChannel->isMeHalfOp() || m_pChannel->isMeOp() || m_pChannel->isMeChanOwner() || m_pChannel->isMeChanAdmin() || m_pChannel->connection()->userInfo()->hasUserMode('o') || m_pChannel->connection()->userInfo()->hasUserMode('O')) )
	{
		delete m_pLabel;
		m_pLabel=0;
		m_pLineEdit = new QLineEdit(this);
		m_pLineEdit->setText(m_pChannel->channelMode());
		m_pLineEdit->show();
		m_pLineEdit->setFocus();
		resizeEvent(new QResizeEvent(size(),size()));
		m_pLineEdit->installEventFilter( this );
		connect(m_pLineEdit,SIGNAL(textChanged ( const QString & ) ),this,SLOT(editorTextChanged( const QString & )));
	}
}

bool KviModeWidget::eventFilter( QObject *obj, QEvent *ev )
{
	if( (obj==m_pLineEdit) && ( ev->type() == QEvent::KeyPress ) )
	{
		QKeyEvent *keyEvent = (QKeyEvent*)ev;
		switch(keyEvent->key())
		{
			case Qt::Key_Return:
			case Qt::Key_Enter:
				editorReturnPressed();
				return TRUE;
			case Qt::Key_Escape:
				reset();
				return TRUE;
		}
	}
	return QFrame::eventFilter( obj, ev );
}

void KviModeWidget::editorReturnPressed()
{
	QString szCurModes=m_pChannel->channelMode();
	QString szNewModes=m_pLineEdit->text();
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
		KviQCString chan = m_pChannel->connection()->encodeText(m_pChannel->objectName());
		m_pChannel->connection()->sendFmtData("MODE %s %s",chan.data(),mode.toUtf8().data());
	}
	reset();
}

void KviModeWidget::editorTextChanged( const QString & text)
{
	int i = 0;
	QString szText=text;
	for(i=0;i<szText.length();i++)
	{
		if( !m_pChannel->connection()->serverInfo()->supportedPlainModes().contains(szText[i]) || 
			szText.indexOf(szText[i])<i,Qt::CaseInsensitive )
			szText.remove(i,1);
	}
	m_pLineEdit->setText(szText);
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "kvi_modew.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
