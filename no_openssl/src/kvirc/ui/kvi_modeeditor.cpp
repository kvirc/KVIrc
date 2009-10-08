//=============================================================================
//
//   File : kvi_modeeditor.cpp
//   Creation date : Sat Apr 14 2001 13:52:11 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_modeeditor.h"
#include "kvi_iconmanager.h"
#include "kvi_locale.h"
#include "kvi_qstring.h"
#include "kvi_console.h"
#include "kvi_ircconnection.h"
#include "kvi_ircconnectionserverinfo.h"
#include "kvi_channel.h"
#include "kvi_ircconnectionuserinfo.h"
#include "kvi_toolwindows_container.h"

#include <QScrollArea>
#include <QLayout>
#include <QLabel>


//////////////////////////////////////////////////////////////////////
// class KviModeEditor
//
//////////////////////////////////////////////////////////////////////

//static char checkable_modes_table[KVI_NUM_CHECKABLE_MODES] = { 'p','s','t','n','m','i'};

KviModeEditor::KviModeEditor(QWidget * par,KviWindowToolPageButton* button,const char *,KviConsole * c,const QString &mode,const QString &key,const QString &limit)
: KviWindowToolWidget(par,button)
{
	m_szMode = mode;
	m_szKey = key;
	m_szLimit = limit;
	bool isEnabled=1;

	QObject * w = parent();
	while(w)
	{
		if(w->inherits("KviChannel"))
		{
			KviChannel *chan = ((KviChannel *)w);
			if(!( chan->isMeHalfOp() || chan->isMeOp() || chan->isMeChanOwner() || chan->isMeChanAdmin() || chan->connection()->userInfo()->hasUserMode('o') || chan->connection()->userInfo()->hasUserMode('O') ) )  isEnabled=0;
			break;
		}
		w = w->parent();
	}

	QGridLayout *pMasterLayout = new QGridLayout(this);

	setFocusPolicy(Qt::ClickFocus);

	QScrollArea * pScrollArea = new QScrollArea(this);
	pScrollArea->viewport()->setBackgroundRole(QPalette::Background);
	pMasterLayout->addWidget(pScrollArea,0,0);

	pMasterLayout->setRowStretch(1,1);
	QPushButton * b;
	if(isEnabled) b = new QPushButton(__tr2qs("&Apply"),this);
	else b = new QPushButton(__tr2qs("Close"),this);

	pMasterLayout->addWidget(b,1,0);
	connect(b,SIGNAL(clicked()),this,SLOT(commit()));

	QWidget * pBackground = new QWidget(pScrollArea->viewport());

	QGridLayout *g = new QGridLayout(pBackground);

	QLabel * l = new QLabel("",pBackground);
	l->setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MODE)));
	g->addWidget(l,0,0);

	l = new QLabel(__tr2qs("Channel Modes"),pBackground);

	g->addWidget(l,0,1,1,1);

	QFrame * f = new QFrame(pBackground);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	g->addWidget(f,1,0,1,3);

	int i = 1;
	QString tmp;

	m_pCheckBoxes = new KviPointerList<QCheckBox>;
	m_pCheckBoxes->setAutoDelete(false);


	// first che basic checkable modes pstnmi
	QString szModes = "pstnmi";

	while(!szModes.isEmpty())
	{
		QChar ccc = szModes[0];
		szModes.remove(0,1);

		KviQString::sprintf(tmp,"%c: %Q",ccc.unicode(),&(c->connection()->serverInfo()->getChannelModeDescription(ccc)));
		QCheckBox * cb = new QCheckBox(tmp,pBackground);
		cb->setEnabled(isEnabled);
		m_pCheckBoxes->append(cb);
		cb->setChecked(m_szMode.contains((char)ccc.unicode()));
		i++;
		g->addWidget(cb,i,0,1,3);
	}

	KviQString::sprintf(tmp,"l: %Q",&(c->connection()->serverInfo()->getChannelModeDescription('l')));
	m_pLimitBox = new QCheckBox(tmp,pBackground);
	m_pLimitBox->setEnabled(isEnabled);
	i++;
	g->addWidget(m_pLimitBox,i,0,1,3);
	connect(m_pLimitBox,SIGNAL(toggled(bool)),this,SLOT(limitBoxToggled(bool)));
	m_pLimitEdit = new QLineEdit(pBackground);
	m_pLimitEdit->setEnabled(isEnabled);
	i++;
	g->addWidget(m_pLimitEdit,i,1,1,2);
	if(!m_szLimit.isEmpty())
	{
		m_pLimitBox->setChecked(true);
		m_pLimitEdit->setText(m_szLimit);
	} else {
		m_pLimitEdit->setEnabled(false);
	}

	KviQString::sprintf(tmp,"k: %Q",&(c->connection()->serverInfo()->getChannelModeDescription('k')));
	m_pKeyBox = new QCheckBox(tmp,pBackground);
	m_pKeyBox->setEnabled(isEnabled);
	i++;

	g->addWidget(m_pKeyBox,i,0,1,3);
	connect(m_pKeyBox,SIGNAL(toggled(bool)),this,SLOT(keyBoxToggled(bool)));
	m_pKeyEdit = new QLineEdit(pBackground);
	m_pKeyEdit->setEnabled(isEnabled);
	i++;
	g->addWidget(m_pKeyEdit,i,1,1,2);
	if(!m_szKey.isEmpty())
	{
		m_pKeyBox->setChecked(true);
		m_pKeyEdit->setText(m_szKey);
	} else {
		m_pKeyEdit->setEnabled(false);
	}

	if(c->connection())
	{
		if(c->connection()->serverInfo())
			szModes = c->connection()->serverInfo()->supportedChannelModes();
	}

	int idx = szModes.lastIndexOf(',');
	if(idx != -1)szModes.remove(0,idx+1);

	szModes.replace("p","");
	szModes.replace("s","");
	szModes.replace("t","");
	szModes.replace("n","");
	szModes.replace("m","");
	szModes.replace("i","");
	szModes.replace(",","");
	szModes.replace("b","");
	szModes.replace("k","");
	szModes.replace("l","");

	while(!szModes.isEmpty())
	{
		QChar ccc = szModes[0];
		szModes.remove(0,1);

		KviQString::sprintf(tmp,"%c: %Q",(char)ccc.unicode(),&(c->connection()->serverInfo()->getChannelModeDescription(ccc)));
		QCheckBox * cb = new QCheckBox(tmp,pBackground);
		cb->setEnabled(isEnabled);
		m_pCheckBoxes->append(cb);
		cb->setChecked(m_szMode.contains((char)ccc.unicode()));
		i++;
		g->addWidget(cb,i,0,1,3);
	}

	i++;

	g->setRowStretch(i,1);
	g->setColumnStretch(2,1);

	pScrollArea->setWidget(pBackground);
}

KviModeEditor::~KviModeEditor()
{
	delete m_pCheckBoxes;
}

void KviModeEditor::limitBoxToggled(bool bChecked)
{
	m_pLimitEdit->setEnabled(bChecked);
}

void KviModeEditor::keyBoxToggled(bool bChecked)
{
	m_pKeyEdit->setEnabled(bChecked);
}

void KviModeEditor::commit()
{
	KviStr szPlusModes;
	KviStr szMinusModes;

	if(!m_szKey.isEmpty())
	{
		// had a key before
		if(m_pKeyBox->isChecked())
		{
			// still have it
			QString tmp = m_pKeyEdit->text().trimmed();
			if(!tmp.isEmpty())
			{
				if(tmp != m_szKey)
				{
					// not the same key!
					// set the new one
					KviStr mode(KviStr::Format,"-k %s",m_szKey.toUtf8().data()); // FIXME: assuming utf8 is wrong here!
					emit setMode(mode.ptr());
					mode.sprintf("+k %s",tmp.toUtf8().data());
					emit setMode(mode.ptr());
				}
			}
		} else {
			// no key now! reset
			KviStr mode(KviStr::Format,"-k %s",m_szKey.toUtf8().data());
			emit setMode(mode.ptr());
		}
	} else {
		// there was no key before
		if(m_pKeyBox->isChecked())
		{
			QString tmp = m_pKeyEdit->text().trimmed();
			if(!tmp.isEmpty())
			{
				// new key to be set
				KviStr mode(KviStr::Format,"+k %s",tmp.toUtf8().data());
				emit setMode(mode.ptr());
			}
		}
	}

	if(!m_szLimit.isEmpty())
	{
		// had a limit before
		if(m_pLimitBox->isChecked())
		{
			// still have it
			QString tmp = m_pLimitEdit->text().trimmed();
			bool bOk;
			unsigned int uLimit = tmp.toUInt(&bOk);
			if(bOk)
			{
				if(m_szLimit != tmp)
				{
					// not the same limit!
					KviStr mode(KviStr::Format,"+l %u",uLimit);
					emit setMode(mode.ptr());
				}
			}
		} else {
			// no limit now! reset
			szMinusModes.append('l');
		}
	} else {
		// there was no limit before
		if(m_pLimitBox->isChecked())
		{
			QString tmp = m_pLimitEdit->text().trimmed();
			bool bOk;
			unsigned int uLimit = tmp.toUInt(&bOk);
			if(bOk)
			{
				// new limit to be set
				KviStr mode(KviStr::Format,"+l %u",uLimit);
				emit setMode(mode.ptr());
			}
		}
	}


	for(QCheckBox * cb = m_pCheckBoxes->first();cb;cb = m_pCheckBoxes->next())
	{
		QString sz = cb->text();
		sz.replace("&","");
		if(sz.length() > 0)
		{
			QChar ccc = sz[0];
			if(cb->isChecked())
			{
				if(!m_szMode.contains((char)ccc.unicode()))szPlusModes.append((char)ccc.unicode());
			} else {
				if(m_szMode.contains((char)ccc.unicode()))szMinusModes.append((char)ccc.unicode());
			}
		}
	}

	KviStr szModes;

	if(szMinusModes.hasData())szModes.sprintf("-%s",szMinusModes.ptr());
	if(szPlusModes.hasData())szModes.append(KviStr::Format,"+%s",szPlusModes.ptr());
	if(szModes.hasData())emit setMode(szModes.ptr());

	emit done();
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "kvi_modeeditor.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
