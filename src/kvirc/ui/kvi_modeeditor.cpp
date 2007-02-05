//
//   File : kvi_modeeditor.cpp
//   Creation date : Sat Apr 14 2001 13:52:11 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
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
#define __KVIRC__
#include "kvi_modeeditor.h"
#include "kvi_iconmanager.h"
#include "kvi_locale.h"
#include "kvi_qstring.h"
#include "kvi_console.h"
#include "kvi_ircconnection.h"
#include "kvi_ircconnectionserverinfo.h"
#include "kvi_channel.h"
#include "kvi_ircconnectionuserinfo.h"
#include "kvi_styled_controls.h"
#include "kvi_toolwindows_container.h"
#include "kvi_tal_scrollview.h"

#include <qlayout.h>
#include <qlabel.h>


//////////////////////////////////////////////////////////////////////
// class KviModeEditor
//
//////////////////////////////////////////////////////////////////////

//static char checkable_modes_table[KVI_NUM_CHECKABLE_MODES] = { 'p','s','t','n','m','i'};

// FIXME: This widget should use a KviTalScrollView!

KviModeEditor::KviModeEditor(QWidget * par,KviWindowToolPageButton* button,const char * nam,KviConsole * c,const char * mode,const char * key,const char * limit)
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
	
	QGridLayout *pMasterLayout = new QGridLayout(this,2,1,2,2);

#ifdef COMPILE_USE_QT4
	setFocusPolicy(Qt::ClickFocus);
#else
	setFocusPolicy(QWidget::ClickFocus);
#endif
	
	KviTalScrollView *pScrollView = new KviTalScrollView(this);
#ifdef COMPILE_USE_QT4
	pScrollView->viewport()->setBackgroundRole(QPalette::Background);
#else
	pScrollView->viewport()->setBackgroundMode(QWidget::PaletteBackground);
#endif
	
	pMasterLayout->addWidget(pScrollView,0,0);

	pMasterLayout->setRowStretch(1,1);
	QPushButton * b;
	if(isEnabled) b = new QPushButton(__tr2qs("&Apply"),this);
	else b = new QPushButton(__tr2qs("Close"),this);
	
	pMasterLayout->addWidget(b,1,0);
	connect(b,SIGNAL(clicked()),this,SLOT(commit()));

	QWidget * pBackground = new QWidget(pScrollView->viewport());

	QGridLayout *g = new QGridLayout(pBackground,20,3,2,2);

	QLabel * l = new QLabel("",pBackground);
	l->setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_MODE)));
	g->addWidget(l,0,0);

	l = new QLabel(__tr2qs("Channel Modes"),pBackground);
	g->addMultiCellWidget(l,0,0,1,2);

	QFrame * f = new QFrame(pBackground);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	g->addMultiCellWidget(f,1,1,0,2);

	int i = 1;
	QString tmp;

	m_pCheckBoxes = new KviPtrList<KviStyledCheckBox>;
	m_pCheckBoxes->setAutoDelete(false);

	
	// first che basic checkable modes pstnmi
	QString szModes = "pstnmi";

	while(!szModes.isEmpty())
	{
		QChar ccc = szModes[0];
		szModes.remove(0,1);

		KviQString::sprintf(tmp,"%c: %Q",ccc.unicode(),&(c->connection()->serverInfo()->getChannelModeDescription(ccc)));
		KviStyledCheckBox * cb = new KviStyledCheckBox(tmp,pBackground);
		cb->setEnabled(isEnabled);
		m_pCheckBoxes->append(cb);
		cb->setChecked(m_szMode.contains((char)ccc.unicode()));
		i++;
		g->addMultiCellWidget(cb,i,i,0,2);
	}

	KviQString::sprintf(tmp,"l: %Q",&(c->connection()->serverInfo()->getChannelModeDescription('l')));
	m_pLimitBox = new KviStyledCheckBox(tmp,pBackground);
	m_pLimitBox->setEnabled(isEnabled);
	i++;
	g->addMultiCellWidget(m_pLimitBox,i,i,0,2);
	connect(m_pLimitBox,SIGNAL(toggled(bool)),this,SLOT(limitBoxToggled(bool)));
	m_pLimitEdit = new QLineEdit(pBackground);
	m_pLimitEdit->setEnabled(isEnabled);
	i++;
	g->addMultiCellWidget(m_pLimitEdit,i,i,1,2);
	if(m_szLimit.hasData())
	{
		m_pLimitBox->setChecked(true);
//		m_pLimitEdit->setEnabled(true);
		m_pLimitEdit->setText(m_szLimit.ptr());
	} else {
		m_pLimitEdit->setEnabled(false);
	}

	KviQString::sprintf(tmp,"k: %Q",&(c->connection()->serverInfo()->getChannelModeDescription('k')));
	m_pKeyBox = new KviStyledCheckBox(tmp,pBackground);
	m_pKeyBox->setEnabled(isEnabled);
	i++;
	g->addMultiCellWidget(m_pKeyBox,i,i,0,2);
	connect(m_pKeyBox,SIGNAL(toggled(bool)),this,SLOT(keyBoxToggled(bool)));
	m_pKeyEdit = new QLineEdit(pBackground);
	m_pKeyEdit->setEnabled(isEnabled);
	i++;
	g->addMultiCellWidget(m_pKeyEdit,i,i,1,2);
	if(m_szKey.hasData())
	{
		m_pKeyBox->setChecked(true);
//		m_pLimitEdit->setEnabled(true);
		m_pKeyEdit->setText(m_szKey.ptr());
	} else {
		m_pKeyEdit->setEnabled(false);
	}

	if(c->connection())
	{
		if(c->connection()->serverInfo())
			szModes = c->connection()->serverInfo()->supportedChannelModes();
	}

	int idx = szModes.findRev(',');
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
		KviStyledCheckBox * cb = new KviStyledCheckBox(tmp,pBackground);
		cb->setEnabled(isEnabled);
		m_pCheckBoxes->append(cb);
		cb->setChecked(m_szMode.contains((char)ccc.unicode()));
		i++;
		g->addMultiCellWidget(cb,i,i,0,2);
	}

	i++;

	g->setRowStretch(i,1);
	g->setColStretch(2,1);

	pScrollView->addChild(pBackground,0,0);
	registerSelf();
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

	if(m_szKey.hasData())
	{
		// had a key before
		if(m_pKeyBox->isChecked())
		{
			// still have it
			KviStr tmp = m_pKeyEdit->text();
			tmp.stripWhiteSpace();
			if(tmp.hasData())
			{
				if(!kvi_strEqualCI(tmp.ptr(),m_szKey.ptr()))
				{
					// not the same key!
					// set the new one
					KviStr mode(KviStr::Format,"-k %s",m_szKey.ptr());
					emit setMode(mode.ptr());
					mode.sprintf("+k %s",tmp.ptr());
					emit setMode(mode.ptr());
				}
			}
		} else {
			// no key now! reset
			KviStr mode(KviStr::Format,"-k %s",m_szKey.ptr());
			emit setMode(mode.ptr());
		}
	} else {
		// there was no key before
		if(m_pKeyBox->isChecked())
		{
			KviStr tmp = m_pKeyEdit->text();
			tmp.stripWhiteSpace();
			if(tmp.hasData())
			{
				// new key to be set
				KviStr mode(KviStr::Format,"+k %s",tmp.ptr());
				emit setMode(mode.ptr());
			}
		}
	}

	if(m_szLimit.hasData())
	{
		// had a limit before
		if(m_pLimitBox->isChecked())
		{
			// still have it
			KviStr tmp = m_pLimitEdit->text();
			tmp.stripWhiteSpace();
			if(tmp.hasData() && tmp.isUnsignedNum())
			{
				if(!kvi_strEqualCI(tmp.ptr(),m_szLimit.ptr()))
				{
					// not the same limit!
					KviStr mode(KviStr::Format,"+l %s",tmp.ptr());
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
			KviStr tmp = m_pLimitEdit->text();
			tmp.stripWhiteSpace();
			if(tmp.hasData() && tmp.isUnsignedNum())
			{
				// new limit to be set
				KviStr mode(KviStr::Format,"+l %s",tmp.ptr());
				emit setMode(mode.ptr());
			}
		}
	}


	for(KviStyledCheckBox * cb = m_pCheckBoxes->first();cb;cb = m_pCheckBoxes->next())
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

#include "kvi_modeeditor.moc"
