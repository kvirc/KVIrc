//=============================================================================
//
//   File : edituser.cpp
//   Creation date : Tue Dec 26 2000 12:24:12 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2004 Szymon Stefanek (pragma at kvirc dot net)
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
#define _WANT_OPTION_FLAGS_

#include "edituser.h"
#include "kvi_regusersdb.h"

#include "kvi_locale.h"
#include "kvi_ircmask.h"
#include "kvi_debug.h"
#include "kvi_iconmanager.h"
#include "kvi_app.h"
#include "kvi_options.h"
#include "kvi_file.h"
#include "kvi_filedialog.h"
#include "kvi_msgbox.h"
#include "kvi_fileutils.h"
#include "kvi_settings.h"
#include "kvi_stringconversion.h"
#include "kvi_options.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qheader.h>
#include "kvi_asciidict.h"
#include <qimage.h>
#include <qstring.h>
#include <qcombobox.h>


#include <qstyle.h>
#include <qpainter.h>
#include "kvi_tal_hbox.h"
#include "kvi_tal_vbox.h"
#include <qinputdialog.h>

#include "wizard.h"


#ifdef COMPILE_INFO_TIPS
	#include <qtooltip.h>
#endif // COMPILE_INFO_TIPS

// kvi_app.cpp
extern KVIRC_API KviRegisteredUserDataBase * g_pRegisteredUserDataBase;
extern KviRegisteredUsersDialog * g_pRegisteredUsersDialog;

static KviRegisteredUserDataBase * g_pLocalRegisteredUserDataBase = 0; // local copy!



KviReguserPropertiesDialog::KviReguserPropertiesDialog(QWidget * p,KviDict<QString> * dict)
: QDialog(p,"property_editor",true)
{
	m_pPropertyDict = dict;

	setCaption(__tr2qs("Property Editor"));
	setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_LINUX)));

	QGridLayout * g = new QGridLayout(this,3,3,4,4);

	m_pTable = new QTable(this);
	g->addMultiCellWidget(m_pTable,0,1,0,1);

	m_pTable->setNumCols(2);
	m_pTable->setSelectionMode(QTable::NoSelection);

	m_pTable->horizontalHeader()->setLabel(0,__tr2qs("Property"));
	m_pTable->horizontalHeader()->setLabel(1,__tr2qs("Value"));

	m_pTable->setMinimumSize(250,250);
	//connect(m_pTable,SIGNAL(valueChanged(int,int)),this,SLOT(propertyValueChanged(int,int)));

	KviTalVBox * vb = new KviTalVBox(this);
	vb->setSpacing(4);
	g->addWidget(vb,0,2);

	m_pAddButton = new QPushButton(__tr2qs("&New"),vb);
	connect(m_pAddButton,SIGNAL(clicked()),this,SLOT(addClicked()));
	m_pAddButton->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NEWITEM)));

	m_pDelButton = new QPushButton(__tr2qs("&Remove"),vb);
	connect(m_pDelButton,SIGNAL(clicked()),this,SLOT(delClicked()));
	m_pDelButton->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DELETEITEM)));

	KviTalHBox * b = new KviTalHBox(this);
	b->setSpacing(4);
	g->addMultiCellWidget(b,2,2,1,2);

	QPushButton * pb = new QPushButton(__tr2qs("&OK"),b);
	connect(pb,SIGNAL(clicked()),this,SLOT(okClicked()));
	pb->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)));


	pb = new QPushButton(__tr2qs("Cancel"),b);
	connect(pb,SIGNAL(clicked()),this,SLOT(reject()));
	pb->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DISCARD)));

	g->setRowStretch(1,1);
	g->setColStretch(0,1);

	fillData();
}

KviReguserPropertiesDialog::~KviReguserPropertiesDialog()
{
}

void KviReguserPropertiesDialog::closeEvent(QCloseEvent *e)
{
	e->accept();
	//delete this;
}

void KviReguserPropertiesDialog::fillData()
{
	m_pTable->setNumRows(m_pPropertyDict->count());
	KviDictIterator<QString> it(*m_pPropertyDict);
	int row = 0;
	while(it.current())
	{
		m_pTable->setItem(row,0,new QTableItem(m_pTable,QTableItem::OnTyping,it.currentKey()));
		m_pTable->setItem(row,1,new QTableItem(m_pTable,QTableItem::OnTyping,*(it.current())));
		++row;
		++it;
	}
	if(m_pTable->numRows() == 0)m_pDelButton->setEnabled(false);
}

void KviReguserPropertiesDialog::okClicked()
{
	m_pPropertyDict->clear();

	int n = m_pTable->numRows();
	for(int i=0;i<n;i++)
	{
		QString szName = m_pTable->text(i,0);
		QString szValue = m_pTable->text(i,1);
		if((!szName.isEmpty()) && (!szValue.isEmpty()))
		{
			m_pPropertyDict->replace(szName,new QString(szValue));
		}
	}

	accept();
}


void KviReguserPropertiesDialog::addClicked()
{
	m_pTable->setNumRows(m_pTable->numRows() + 1);
	m_pTable->setItem(m_pTable->numRows() - 1,0,new QTableItem(m_pTable,QTableItem::OnTyping,""));
	m_pTable->setItem(m_pTable->numRows() - 1,1,new QTableItem(m_pTable,QTableItem::OnTyping,""));
	m_pDelButton->setEnabled(true);
}

void KviReguserPropertiesDialog::delClicked()
{
	int i = m_pTable->currentRow();

	if((i > -1) && (i < m_pTable->numRows()))
	{
		// remove row i
		m_pTable->clearCell(i,0);
		m_pTable->clearCell(i,1);

		for(;i < (m_pTable->numRows() - 1);i++)
		{
			m_pTable->swapRows(i,i+1);
		}
		m_pTable->setNumRows(m_pTable->numRows() - 1);
		if(m_pTable->numRows() == 0)m_pDelButton->setEnabled(false);
	}
}


KviReguserMaskDialog::KviReguserMaskDialog(QWidget * p,KviIrcMask * m)
: QDialog(p,"reguser_mask_editor",true)
{
	m_pMask = m;

	setCaption(__tr2qs("Mask Editor"));

	QGridLayout * g = new QGridLayout(this,3,2,4,4);

	QLabel * l = new QLabel(__tr2qs("Insert a mask for this user.<br>It can contain the wildcard characters '*' and '?'."),this);
	//l->setAlignment(Qt::AlignCenter);
	g->addMultiCellWidget(l,0,0,0,1);

	KviTalHBox * b = new KviTalHBox(this);
	g->addMultiCellWidget(b,1,1,0,1);

	m_pNickEdit = new QLineEdit(b);
	//m_pNickEdit->setMinimumWidth(120);
	m_pNickEdit->setAlignment(Qt::AlignRight);
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pNickEdit,__tr2qs("<center>This the <b>nickname</b> that will match this user, default value is the registered name.</center>"));
#endif

	l = new QLabel("<center><b>!</b></center>",b);
	l->setAlignment(Qt::AlignCenter);
	//l->setMinimumWidth(40);

	m_pUserEdit = new QLineEdit(b);
	//m_pUserEdit->setMinimumWidth(120);
	m_pUserEdit->setAlignment(Qt::AlignCenter);
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pUserEdit,__tr2qs("<center>This the <b>username</b> that will match this user. <b>*</b> will match any username.</center>"));
#endif

	l = new QLabel("<center><b>@</b></center>",b);
	l->setAlignment(Qt::AlignCenter);
	//l->setMinimumWidth(40);

	m_pHostEdit = new QLineEdit(b);
	//m_pHostEdit->setMinimumWidth(120);
	m_pHostEdit->setAlignment(Qt::AlignLeft);
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pHostEdit,__tr2qs("<center>This the <b>hostname</b> that will match this user. <b>*</b> will match any hostname.</center>"));
#endif

	// just a spacer
//	l = new QLabel("<nobr>&nbsp;<nobr>",this);
//	g->addMultiCellWidget(l,2,2,0,1);

//	QFrame * f = new QFrame(this);
//	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);
//	g->addMultiCellWidget(f,3,3,0,1);

	b = new KviTalHBox(this);
	b->setSpacing(4);
	g->addWidget(b,2,1);

	QPushButton * pb = new QPushButton(__tr2qs("&OK"),b);
	connect(pb,SIGNAL(clicked()),this,SLOT(okClicked()));
	//pb->setMinimumWidth(120);

	pb = new QPushButton(__tr2qs("Cancel"),b);
	connect(pb,SIGNAL(clicked()),this,SLOT(reject()));
	//pb->setMinimumWidth(120);



	g->setColStretch(0,1);
	g->setRowStretch(0,1);

	m_pNickEdit->setText(m->nick());
	m_pUserEdit->setText(m->user());
	m_pHostEdit->setText(m->host());
}

KviReguserMaskDialog::~KviReguserMaskDialog()
{
}

void KviReguserMaskDialog::closeEvent(QCloseEvent *e)
{
	e->accept();
	//delete this;
}

void KviReguserMaskDialog::okClicked()
{
	KviStr szTmp = m_pNickEdit->text();
	if(szTmp.isEmpty())szTmp = "*";
	m_pMask->setNick(szTmp.ptr());

	szTmp = m_pUserEdit->text();
	if(szTmp.isEmpty())szTmp = "*";
	m_pMask->setUsername(szTmp.ptr());

	szTmp = m_pHostEdit->text();
	if(szTmp.isEmpty())szTmp = "*";
	m_pMask->setHost(szTmp.ptr());

	accept();
}




KviRegisteredUserEntryDialog::KviRegisteredUserEntryDialog(QWidget *p,KviRegisteredUser * r,bool bModal)
: KviTalTabDialog(p,"reguser_entry_editor",bModal)
{
	m_pUser = r;
	m_pCustomColor = new QColor();
	
	if(r)
	{
		QString col=r->getProperty("customColor");
		KviStringConversion::fromString(col,(*m_pCustomColor));
	}
	
	m_pPropertyDict = new KviDict<QString>(17,false);
	m_pPropertyDict->setAutoDelete(true);

	//setMinimumSize(400,450);

	setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_LINUX)));
	setCaption(__tr2qs("Registered User Entry"));

	QWidget * p1 = new QWidget(this);

	QGridLayout * g = new QGridLayout(p1,6,2,4,4);

	QLabel * l = new QLabel(__tr2qs("Name:"),p1);
	g->addWidget(l,0,0);

	m_pNameEdit = new QLineEdit(p1);
	g->addWidget(m_pNameEdit,0,1);

	l = new QLabel(__tr2qs("Comment:"),p1);
	g->addWidget(l,1,0);
	
	m_pCommentEdit = new QLineEdit(p1);
	g->addWidget(m_pCommentEdit,1,1);

	QFrame * f = new QFrame(p1);
	g->addMultiCellWidget(f,2,2,0,1);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);

	l = new QLabel(__tr2qs("Masks:"),p1);
	g->addMultiCellWidget(l,3,3,0,1);

	m_pMaskListBox = new KviTalListBox(p1);
	connect(m_pMaskListBox,SIGNAL(currentChanged(KviTalListBoxItem *)),this,SLOT(maskCurrentChanged(KviTalListBoxItem *)));
	m_pMaskListBox->setMinimumSize(300,200);

	g->addMultiCellWidget(m_pMaskListBox,4,4,0,1);

	KviTalHBox * b = new KviTalHBox(p1);
	g->addMultiCellWidget(b,5,5,0,1);
	b->setSpacing(4);

	m_pAddMaskButton = new QPushButton(__tr2qs("&Add..."),b);
	connect(m_pAddMaskButton,SIGNAL(clicked()),this,SLOT(addMaskClicked()));
	m_pAddMaskButton->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NEWITEM)));

	m_pDelMaskButton = new QPushButton(__tr2qs("Re&move"),b);
	m_pDelMaskButton->setEnabled(false);
	connect(m_pDelMaskButton,SIGNAL(clicked()),this,SLOT(delMaskClicked()));
	m_pDelMaskButton->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DELETEITEM)));

	m_pEditMaskButton = new QPushButton(__tr2qs("&Edit"),b);
	m_pEditMaskButton->setEnabled(false);
	connect(m_pEditMaskButton,SIGNAL(clicked()),this,SLOT(editMaskClicked()));
	m_pEditMaskButton->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_EDITITEM)));

	g->setRowStretch(4,1);
	g->setColStretch(1,1);

	addTab(p1,__tr2qs("Identity"));



	QWidget * p2 = new QWidget(this);

	g = new QGridLayout(p2,6,3,5,2);

	m_pNotifyCheck = new KviStyledCheckBox(__tr2qs("Notify when user is online"),p2);
	g->addMultiCellWidget(m_pNotifyCheck,0,0,0,2);

	l = new QLabel(__tr2qs("Notify nicknames:"),p2);
	l->setEnabled(m_pNotifyCheck->isChecked());
	g->addWidget(l,1,0);
	connect(m_pNotifyCheck,SIGNAL(toggled(bool)),l,SLOT(setEnabled(bool)));
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pNotifyCheck,__tr2qs("<center>You can enter a space separated list of nicknames.</center>"));
#endif


	m_pNotifyNick = new QLineEdit(p2);
	m_pNotifyNick->setEnabled(false);
	g->addMultiCellWidget(m_pNotifyNick,1,1,1,2);
	connect(m_pNotifyCheck,SIGNAL(toggled(bool)),m_pNotifyNick,SLOT(setEnabled(bool)));


	f = new QFrame(p2);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	g->addMultiCellWidget(f,2,2,0,2);

	m_pAvatar = 0;
	if(r)
	{
		const char * av = r->getProperty("avatar");
		if(av)
		{
			m_pAvatar = new KviPixmap(av);
		}
	}
	if(!m_pAvatar)m_pAvatar = new KviPixmap();

	m_pAvatarSelector = new KviPixmapSelector(p2,__tr2qs("Avatar"),m_pAvatar,true);
	g->addMultiCellWidget(m_pAvatarSelector,3,3,0,2);

	f = new QFrame(p2);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	g->addMultiCellWidget(f,4,4,0,2);

	m_pCustomColorCheck = new KviStyledCheckBox(__tr2qs("Use custom color in userlist"),p2);
	if(r)
		m_pCustomColorCheck->setChecked(r->getBoolProperty("useCustomColor"));
	g->addMultiCellWidget(m_pCustomColorCheck,5,5,0,1);

	m_pCustomColorSelector = new KviColorSelector(p2,QString::null,m_pCustomColor,1);
	g->addWidget(m_pCustomColorSelector,5,2);
	
	QPushButton * pb = new QPushButton(__tr2qs("All Properties..."),p2);
	connect(pb,SIGNAL(clicked()),this,SLOT(editAllPropertiesClicked()));
	g->addWidget(pb,6,2);

	g->setColStretch(1,1);
	g->setRowStretch(3,1);

	addTab(p2,__tr2qs("Properties"));

	// Ignore TAB
	QVBox * vb = new QVBox(this);
	vb->setMargin(10);

	m_pIgnoreEnabled = new KviStyledCheckBox(__tr2qs("Enable ignore for this user"),vb);

	m_pIgnoreQuery = new KviStyledCheckBox(__tr2qs("Ignore query-messages"),vb);
	connect(m_pIgnoreEnabled,SIGNAL(toggled(bool)),m_pIgnoreQuery,SLOT(setEnabled(bool)));

	m_pIgnoreChannel = new KviStyledCheckBox(__tr2qs("Ignore channel-messages"),vb);
	connect(m_pIgnoreEnabled,SIGNAL(toggled(bool)),m_pIgnoreChannel,SLOT(setEnabled(bool)));

	m_pIgnoreNotice = new KviStyledCheckBox(__tr2qs("Ignore notice-messages"),vb);
	connect(m_pIgnoreEnabled,SIGNAL(toggled(bool)),m_pIgnoreNotice,SLOT(setEnabled(bool)));

	m_pIgnoreCtcp = new KviStyledCheckBox(__tr2qs("Ignore ctcp-messages"),vb);
	connect(m_pIgnoreEnabled,SIGNAL(toggled(bool)),m_pIgnoreCtcp,SLOT(setEnabled(bool)));

	m_pIgnoreInvite = new KviStyledCheckBox(__tr2qs("Ignore invites"),vb);
	connect(m_pIgnoreEnabled,SIGNAL(toggled(bool)),m_pIgnoreInvite,SLOT(setEnabled(bool)));

	m_pIgnoreDcc = new KviStyledCheckBox(__tr2qs("Ignore DCCs"),vb);
	connect(m_pIgnoreEnabled,SIGNAL(toggled(bool)),m_pIgnoreDcc,SLOT(setEnabled(bool)));

	addTab(vb,__tr2qs("Ignore"));

	setCancelButton(__tr2qs("Cancel"));
	setOkButton(__tr2qs("&OK"));
	connect(this,SIGNAL(applyButtonPressed()),this,SLOT(okClicked()));
	connect(this,SIGNAL(cancelButtonPressed()),this,SLOT(reject()));

	if(r)
	{
		m_pNameEdit->setText(r->name());
		m_pCommentEdit->setText(r->getProperty("comment"));
		for(KviIrcMask * m = r->maskList()->first();m;m = r->maskList()->next())
		{
			QString mk = m->nick();
			mk += QChar('!');
			mk += m->user();
			mk += QChar('@');
			mk += m->host();
			m_pMaskListBox->insertItem(mk);
		}

		QString szNotifyNicks = r->getProperty("notify");
		if(!szNotifyNicks.isEmpty())
		{
			m_pNotifyCheck->setChecked(true);
			m_pNotifyNick->setText(szNotifyNicks);
			m_pNotifyNick->setEnabled(true);
		}
		
		if(r->propertyDict())
		{
			KviDictIterator<QString> it(*(r->propertyDict()));
			while(QString *s = it.current())
			{
				m_pPropertyDict->insert(it.currentKey(),new QString(*s));
				++it;
			}
		}

		m_pIgnoreEnabled->setChecked(r->ignoreEnagled());
		m_pIgnoreQuery->setChecked(r->ignoreFlags() & KviRegisteredUser::Query);
		m_pIgnoreChannel->setChecked(r->ignoreFlags() & KviRegisteredUser::Channel);
		m_pIgnoreNotice->setChecked(r->ignoreFlags() & KviRegisteredUser::Notice);
		m_pIgnoreCtcp->setChecked(r->ignoreFlags() & KviRegisteredUser::Ctcp);
		m_pIgnoreInvite->setChecked(r->ignoreFlags() & KviRegisteredUser::Invite);
		m_pIgnoreDcc->setChecked(r->ignoreFlags() & KviRegisteredUser::Dcc);
	}
}

void KviRegisteredUserEntryDialog::closeEvent(QCloseEvent *e)
{
	e->accept();
	okClicked();
	//delete this;
}

KviRegisteredUserEntryDialog::~KviRegisteredUserEntryDialog()
{
	delete m_pAvatar;
	delete m_pPropertyDict;
	delete m_pCustomColor;
}

void KviRegisteredUserEntryDialog::maskCurrentChanged(KviTalListBoxItem *it)
{
	m_pDelMaskButton->setEnabled(it);
	m_pEditMaskButton->setEnabled(it);
}

void KviRegisteredUserEntryDialog::okClicked()
{
	QString szGroup;
	if(m_pUser)
	{
		szGroup=m_pUser->group();
		g_pLocalRegisteredUserDataBase->removeUser(m_pUser->name());
	}

	QString name = m_pNameEdit->text();

	if(name.isEmpty())name = "user";

	KviRegisteredUser * u;

	QString szNameOk = name;

	int idx = 1;

	do {
		u = g_pLocalRegisteredUserDataBase->findUserByName(szNameOk);
		if(u)
		{
			KviQString::sprintf(szNameOk,"%Q%d",&name,idx);
			idx++;
		}
	} while(u);


	u = g_pLocalRegisteredUserDataBase->addUser(szNameOk);
	u->setGroup(szGroup);
	
	if(!u)
	{
		// ops... no way
		// FIXME: spit an error message ?
		debug("Ops.. something wrong with the regusers db");
		accept();
		return;
	}

	int cnt = m_pMaskListBox->count();
	idx = 0;
	while(cnt > 0)
	{
		QString mask = m_pMaskListBox->text(idx);
		KviIrcMask * mk = new KviIrcMask(mask);
		g_pLocalRegisteredUserDataBase->removeMask(*mk);
		g_pLocalRegisteredUserDataBase->addMask(u,mk);
		cnt--;
		idx++;
	}
	u->setProperty("comment",m_pCommentEdit->text());
	
	m_pAvatarSelector->commit();

	if(!m_pAvatar->isNull())
	{
		QString szPath = m_pAvatar->path();
		u->setProperty("avatar",szPath);
	}

	if(m_pNotifyCheck->isChecked())
	{
		QString szNicks = m_pNotifyNick->text();
	
		if(!szNicks.isEmpty())
		{
			u->setProperty("notify",szNicks);
		}
	}
	
	m_pPropertyDict->remove("notify");
	m_pPropertyDict->remove("avatar");

	KviDictIterator<QString> it(*m_pPropertyDict);
	while(QString *s = it.current())
	{
		u->setProperty(it.currentKey(),*s);
		++it;
	}

	u->setProperty("useCustomColor",m_pCustomColorCheck->isChecked());
	
	QString col;
	KviStringConversion::toString(m_pCustomColorSelector->getColor(),col);
	u->setProperty("customColor",col);

	
	int iIgnoreFlags=0;
	u->setIgnoreEnabled(m_pIgnoreEnabled->isChecked());
	if(m_pIgnoreQuery->isChecked())
		iIgnoreFlags |= KviRegisteredUser::Query;
	if(m_pIgnoreChannel->isChecked())
		iIgnoreFlags |= KviRegisteredUser::Channel;
	if(m_pIgnoreNotice->isChecked())
		iIgnoreFlags |= KviRegisteredUser::Notice;
	if(m_pIgnoreCtcp->isChecked())
		iIgnoreFlags |= KviRegisteredUser::Ctcp;
	if(m_pIgnoreInvite->isChecked())
		iIgnoreFlags |= KviRegisteredUser::Invite;
	if(m_pIgnoreDcc->isChecked())
		iIgnoreFlags |= KviRegisteredUser::Dcc;

	u->setIgnoreFlags(iIgnoreFlags);
	accept();
	g_pApp->optionResetUpdate(KviOption_resetUpdateGui);
}


void KviRegisteredUserEntryDialog::addMaskClicked()
{
	KviIrcMask mk;
	KviReguserMaskDialog * dlg = new KviReguserMaskDialog(this,&mk);
	if(dlg->exec() == QDialog::Accepted)
	{
		QString m = mk.nick();
		m += QChar('!');
		m += mk.user();
		m += QChar('@');
		m += mk.host();
		m_pMaskListBox->insertItem(m);
	}
	delete dlg;
}

void KviRegisteredUserEntryDialog::delMaskClicked()
{
	int idx = m_pMaskListBox->currentItem();
	if(idx == -1)return;
	m_pMaskListBox->removeItem(idx);
}

void KviRegisteredUserEntryDialog::editMaskClicked()
{
	int idx = m_pMaskListBox->currentItem();
	if(idx == -1)return;
	KviStr szM = m_pMaskListBox->text(idx);
	if(szM.isEmpty())return;

	KviIrcMask mk(szM.ptr());
	KviReguserMaskDialog * dlg = new KviReguserMaskDialog(this,&mk);
	if(dlg->exec() == QDialog::Accepted)
	{
		QString m = mk.nick();
		m += QChar('!');
		m += mk.user();
		m += QChar('@');
		m += mk.host();
		m_pMaskListBox->changeItem(m,idx);
	}
	delete dlg;
}

void KviRegisteredUserEntryDialog::editAllPropertiesClicked()
{
	m_pAvatarSelector->commit();

	if(m_pAvatar->isNull())
	{
		m_pPropertyDict->remove("avatar");
	} else {
		KviStr szPath = m_pAvatar->path();
		if(szPath.isEmpty())m_pPropertyDict->remove("avatar");
		else m_pPropertyDict->replace("avatar",new QString(szPath));
	}

	if(m_pNotifyCheck->isChecked())
	{
		QString szNicks = m_pNotifyNick->text();
	
		if(!szNicks.isEmpty())
		{
			m_pPropertyDict->replace("notify",new QString(szNicks));
		} else {
			m_pPropertyDict->remove("notify");
		}
	} else {
		m_pPropertyDict->remove("notify");
	}


	KviReguserPropertiesDialog * dlg = new KviReguserPropertiesDialog(this,m_pPropertyDict);
	if(dlg->exec() != QDialog::Accepted)
	{
		delete dlg;
		return;
	}
	delete dlg;

	QString * notify = m_pPropertyDict->find("notify");
	bool bGotIt = false;
	if(notify)
	{
		if(!notify->isEmpty())
		{
			bGotIt = true;
			m_pNotifyNick->setText(*notify);
		}
	}
	m_pNotifyCheck->setChecked(bGotIt);
	m_pNotifyNick->setEnabled(bGotIt);
	if(!bGotIt)m_pNotifyNick->setText("");

	QString * avatar = m_pPropertyDict->find("avatar");
	bGotIt = false;
	if(avatar)
	{
		if(!avatar->isEmpty())
			m_pAvatarSelector->setImagePath(*avatar);
	}

}

KviRegisteredUsersDialogItem::KviRegisteredUsersDialogItem(KviTalListViewItem * par,KviRegisteredUser * u)
: KviRegisteredUsersDialogItemBase(User,par), m_pUser(u)
{
	setText(0,u->name());
}

void KviRegisteredUsersDialogItem::paintCell(QPainter * p,const QColorGroup &cg,int column,int width,int align)
{
	if(column == 1)
	{
		KviTalListView *lv = (KviTalListView *)listView();
		const BackgroundMode bgmode = lv->viewport()->backgroundMode();
		const QColorGroup::ColorRole crole = QPalette::backgroundRoleFromMode( bgmode );
		p->fillRect(0,0,width,height(),isSelected() ? cg.brush(QColorGroup::Highlight) : cg.brush(crole));
		int marg = lv->itemMargin();

//		int st = QStyle::Style_Enabled;
		QString szTmp;
		if(m_pUser)szTmp = m_pUser->getProperty("notify");
		QRect r = lv->itemRect(this);

		p->drawRect(1,1,r.height() - 2,r.height() - 2);

		if(!szTmp.isEmpty())
		{
			p->fillRect(3,3,r.height() - 6,r.height() - 6,Qt::black);

			p->setPen(Qt::gray);
	
			QString sz = "(";
			sz += szTmp;
			sz += ")";
	
			p->drawText(r.height() + 3,0,lv->columnWidth(1) - (marg + r.height() + 3),r.height(),Qt::AlignLeft | Qt::SingleLine,sz);
		}
		return;
	}
	KviTalListViewItem::paintCell(p,cg,column,width,align);
}


KviRegisteredUsersDialog::KviRegisteredUsersDialog(QWidget * par)
: QWidget(par)
{
	g_pRegisteredUsersDialog = this;

	g_pLocalRegisteredUserDataBase = new KviRegisteredUserDataBase();
	g_pLocalRegisteredUserDataBase->copyFrom(g_pRegisteredUserDataBase);

	setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_REGUSERS)));
	setCaption(__tr2qs("Registered Users - KVIrc"));

	QGridLayout * g = new QGridLayout(this,4,3,4,4);


	m_pListView = new KviTalListView(this);

	m_pListView->addColumn(__tr2qs("Name"),200);
	m_pListView->addColumn(__tr2qs("Notify"),130);

	m_pListView->setAllColumnsShowFocus(true);

	m_pListView->setSelectionMode(KviTalListView::Extended);
	m_pListView->setRootIsDecorated(TRUE);

#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pListView,__tr2qs("<center>This is the list of registered users. " \
		"KVIrc can automatically recognize and associate properties to them.<br>" \
		"Use the buttons on the right to add, edit and remove entries. " \
		"The \"notify\" column allows you to quickly add users to the notify list. " \
		"Notify list fine-tuning can be performed by editing the entry properties.</center>"));
#endif // COMPILE_INFO_TIPS

	connect(m_pListView,SIGNAL(pressed(KviTalListViewItem *,const QPoint &,int)),this,SLOT(itemPressed(KviTalListViewItem *,const QPoint &,int)));
	connect(m_pListView,SIGNAL(doubleClicked(KviTalListViewItem *)),this,SLOT(itemDoubleClicked(KviTalListViewItem *)));

	g->addMultiCellWidget(m_pListView,0,1,0,1);

	KviTalVBox * vbox = new KviTalVBox(this);
	vbox->setSpacing(4);
	g->addWidget(vbox,0,2);

	m_pWizardAddButton = new QPushButton(__tr2qs("Add (Wizard)..."),vbox);
	connect(m_pWizardAddButton,SIGNAL(clicked()),this,SLOT(addWizardClicked()));
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pWizardAddButton,__tr2qs("Add a registered user by means of a user-friendly wizard."));
#endif // COMPILE_INFO_TIPS
	m_pWizardAddButton->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NEWITEMBYWIZARD)));


	m_pAddButton = new QPushButton(__tr2qs("&Add..."),vbox);
	connect(m_pAddButton,SIGNAL(clicked()),this,SLOT(addClicked()));
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pAddButton,__tr2qs("Open the edit dialog to create a new user entry."));
#endif // COMPILE_INFO_TIPS
	m_pAddButton->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NEWITEM)));
	
	m_pAddGroupButton = new QPushButton(__tr2qs("&Add Group..."),vbox);
	connect(m_pAddGroupButton,SIGNAL(clicked()),this,SLOT(addGroupClicked()));
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pAddGroupButton,__tr2qs("Adds a new group"));
#endif // COMPILE_INFO_TIPS
	m_pAddGroupButton->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NEWITEM)));

	m_pRemoveButton = new QPushButton(__tr2qs("Re&move"),vbox);
	connect(m_pRemoveButton,SIGNAL(clicked()),this,SLOT(removeClicked()));
	m_pRemoveButton->setEnabled(false);
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pRemoveButton,__tr2qs("Remove the currently selected entries."));
#endif // COMPILE_INFO_TIPS
	m_pRemoveButton->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DELETEITEM)));


	m_pEditButton = new QPushButton(__tr2qs("&Edit..."),vbox);
	connect(m_pEditButton,SIGNAL(clicked()),this,SLOT(editClicked()));
	m_pEditButton->setEnabled(false);
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pEditButton,__tr2qs("Edit the first selected entry."));
#endif // COMPILE_INFO_TIPS
	m_pEditButton->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_EDITITEM)));

	QFrame * f = new QFrame(vbox);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);

	m_pExportButton = new QPushButton(__tr("Export To..."),vbox);
	m_pExportButton->setEnabled(false);
	connect(m_pExportButton,SIGNAL(clicked()),this,SLOT(exportClicked()));
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pExportButton,__tr2qs("Export the selected entries to a file.<br>All the data associated with the selected registered users will be exported.<br>You (or anyone else) can later import the entries by using the \"Import\" button."));
#endif // COMPILE_INFO_TIPS
	m_pExportButton->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_FLOPPY)));


	m_pImportButton = new QPushButton(__tr("Import From..."),vbox);
	connect(m_pImportButton,SIGNAL(clicked()),this,SLOT(importClicked()));
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pImportButton,__tr2qs("Import entries from a file exported earlier by the \"export\" function of this dialog."));
#endif // COMPILE_INFO_TIPS
	m_pImportButton->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_FOLDER)));


	KviTalHBox * hbox = new KviTalHBox(this);
	hbox->setSpacing(4);
	g->addMultiCellWidget(hbox,3,3,1,2);

	QPushButton * b;


	b = new QPushButton(__tr2qs("&OK"),hbox);
	connect(b,SIGNAL(clicked()),this,SLOT(okClicked()));
	b->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)));
	//b->setMinimumWidth(120);

	b = new QPushButton(__tr2qs("Cancel"),hbox);
	connect(b,SIGNAL(clicked()),this,SLOT(cancelClicked()));
	b->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DISCARD)));
	//b->setMinimumWidth(120);


	g->addRowSpacing(2,15);

	g->setColStretch(0,1);
	g->setRowStretch(1,1);

	connect(m_pListView,SIGNAL(selectionChanged()),this,SLOT(selectionChanged()));
	connect(m_pListView,SIGNAL(rightButtonClicked ( KviTalListViewItem *, const QPoint &, int ) ),this,SLOT(listViewRightButtonClicked ( KviTalListViewItem *, const QPoint &, int )));

	fillList();

	if(!parent())
	{
		if(KVI_OPTION_RECT(KviOption_rectRegisteredUsersDialogGeometry).y() < 5)
		{
			KVI_OPTION_RECT(KviOption_rectRegisteredUsersDialogGeometry).setY(5);
		}
		//setGeometry(KVI_OPTION_RECT(KviOption_rectRegisteredUsersDialogGeometry));
		resize(KVI_OPTION_RECT(KviOption_rectRegisteredUsersDialogGeometry).width(),
			KVI_OPTION_RECT(KviOption_rectRegisteredUsersDialogGeometry).height());
		move(KVI_OPTION_RECT(KviOption_rectRegisteredUsersDialogGeometry).x(),
			KVI_OPTION_RECT(KviOption_rectRegisteredUsersDialogGeometry).y());
	}
}

KviRegisteredUsersDialog::~KviRegisteredUsersDialog()
{
#ifndef Q_OS_MACX
	if(!parent())KVI_OPTION_RECT(KviOption_rectRegisteredUsersDialogGeometry) = QRect(pos().x(),pos().y(),
			size().width(),size().height());
#else
	if(!parent())KVI_OPTION_RECT(KviOption_rectRegisteredUsersDialogGeometry) = geometry();
#endif

	g_pRegisteredUsersDialog = 0;

	delete g_pLocalRegisteredUserDataBase;
	g_pLocalRegisteredUserDataBase = 0;
}

void KviRegisteredUsersDialog::itemPressed(KviTalListViewItem *it,const QPoint &pnt,int c)
{
	if(!it)return;
	KviRegisteredUsersDialogItemBase* b=(KviRegisteredUsersDialogItemBase*)it;
	if(b->type()==KviRegisteredUsersDialogItemBase::User)
	{
		KviRegisteredUsersDialogItem *i = (KviRegisteredUsersDialogItem *)it;
	
		QRect r = m_pListView->itemRect(i);
		int daw = m_pListView->columnWidth(0);
	
		QPoint ppp = m_pListView->mapFromGlobal(pnt);
	
		if((c == 1) && (ppp.x() < (r.height() + 5 + daw)))
		{
			// notify list toggle
			if(i->user()->getProperty("notify"))
			{
				i->user()->setProperty("notify",""); // kill that
			} else {
				// try to find the nicknames to be notified
				QString szMask;
	
				for(KviIrcMask * m = i->user()->maskList()->first();m;m = i->user()->maskList()->next())
				{
					QString tmp = m->nick();
					if((tmp.find('*') == -1) && (tmp.find('?') == -1) && (!tmp.isEmpty()))
					{
						if(!szMask.isEmpty())szMask.append(' ');
						szMask.append(tmp);
					}
				}
				// if the nickname list is still empty , build a dummy nick to notify
				szMask = i->user()->name();
				szMask.replace(" ","");
				szMask.replace("'","");
				szMask.replace("&","");
				szMask.replace(",","");
	
				i->user()->setProperty("notify",szMask);
			}
			m_pListView->update();
		}
	}
}

void KviRegisteredUsersDialog::itemDoubleClicked(KviTalListViewItem *it)
{
	if(!it)return;
	KviRegisteredUsersDialogItemBase* b=(KviRegisteredUsersDialogItemBase*)it;
	if(b->type()==KviRegisteredUsersDialogItemBase::User)
	{
		editItem((KviRegisteredUsersDialogItem *)it);
	} else	{
		KviRegisteredUsersGroupItem *i = (KviRegisteredUsersGroupItem *)b;
		editGroup(i->group());
	}
}

void KviRegisteredUsersDialog::addGroupClicked()
{
	bool ok;
	QString text = QInputDialog::getText(
		"KVIrc", __tr("Group name:"), QLineEdit::Normal,
		QString::null, &ok, this );
	if ( ok && !text.isEmpty() ) {
		g_pLocalRegisteredUserDataBase->addGroup(text);
		fillList();
	}
}
void KviRegisteredUsersDialog::editGroup(KviRegisteredUserGroup* group)
{
	bool ok;
	
	QString text = QInputDialog::getText(
		"KVIrc", __tr("Group name:"), QLineEdit::Normal,
		group->name(), &ok, this );
	if ( ok && !text.isEmpty() ) {
		QString szOldGroup=group->name();
		g_pLocalRegisteredUserDataBase->groupDict()->setAutoDelete(0);
		g_pLocalRegisteredUserDataBase->groupDict()->remove(szOldGroup);
		g_pLocalRegisteredUserDataBase->groupDict()->setAutoDelete(1);
		group->setName(text);
		g_pLocalRegisteredUserDataBase->groupDict()->insert(text,group);
		
		KviDict<KviRegisteredUser> * d = g_pLocalRegisteredUserDataBase->userDict();
		KviDictIterator<KviRegisteredUser> it(*d);
	
		while(KviRegisteredUser * u = it.current())
		{
			if(u->group()==szOldGroup)
				u->setGroup(text);
			++it;
		}
		
		
		fillList();
	}
}

void KviRegisteredUsersDialog::listViewRightButtonClicked ( KviTalListViewItem * pItem, const QPoint & point, int )
{
	if(pItem)
	{
		KviRegisteredUsersDialogItemBase* b=(KviRegisteredUsersDialogItemBase*)pItem;
		if(b->type()==KviRegisteredUsersDialogItemBase::User)
		{
			KviTalPopupMenu *groups = new KviTalPopupMenu;
			
			KviDict<KviRegisteredUserGroup> * pGroups = g_pLocalRegisteredUserDataBase->groupDict();
			KviDictIterator<KviRegisteredUserGroup> git(*pGroups);
			m_TmpDict.clear();
			while(KviRegisteredUserGroup * g = git.current())
			{
				int id=groups->insertItem(git.currentKey());
				m_TmpDict.replace(id,g);
				++git;
			}
			
			connect(groups,SIGNAL(activated ( int )),this,SLOT(moveToGroupMenuClicked(int)));
			
			KviTalPopupMenu *mainPopup = new KviTalPopupMenu;
			mainPopup->insertItem(__tr("Move to group"),groups);
			mainPopup->exec(point);
		}
	}
}

void KviRegisteredUsersDialog::moveToGroupMenuClicked(int id)
{
	QString szGroup=m_TmpDict.find(id)->name();
	KviTalListViewItemIterator it( m_pListView,  KviTalListViewItemIterator::Selected );
	while ( it.current() ) {
		KviRegisteredUsersDialogItemBase* b=(KviRegisteredUsersDialogItemBase*)(it.current());
		if(b->type()==KviRegisteredUsersDialogItemBase::User)
		{
			((KviRegisteredUsersDialogItem *)(it.current()))->user()->setGroup(szGroup);
		} 
		++it;
	}
	fillList();
}

void KviRegisteredUsersDialog::fillList()
{
	m_pListView->clear();
	KviDict<KviRegisteredUsersGroupItem> groupItems(5,false);
	
	KviDict<KviRegisteredUserGroup> * pGroups = g_pLocalRegisteredUserDataBase->groupDict();
	KviDictIterator<KviRegisteredUserGroup> git(*pGroups);
	while(KviRegisteredUserGroup * g = git.current())
	{
		KviRegisteredUsersGroupItem* pCur = new KviRegisteredUsersGroupItem(m_pListView,g);
		groupItems.insert(g->name(),pCur);
		pCur->setOpen(TRUE);
		++git;
	}
	
	KviDict<KviRegisteredUser> * d = g_pLocalRegisteredUserDataBase->userDict();
	KviDictIterator<KviRegisteredUser> it(*d);
	KviRegisteredUsersDialogItem * item;

	while(KviRegisteredUser * u = it.current())
	{
		if(u->group().isEmpty())
			u->setGroup(__tr("Default"));
		if(groupItems.find(u->group()))
			item = new KviRegisteredUsersDialogItem(groupItems.find(u->group()),u);
		else if(groupItems.find(__tr("Default")))
			item = new KviRegisteredUsersDialogItem(groupItems.find(__tr("Default")),u);
		else { //should never be called
			KviRegisteredUserGroup* pGroup = g_pLocalRegisteredUserDataBase->addGroup(__tr("Default"));
			KviRegisteredUsersGroupItem* pCur = new KviRegisteredUsersGroupItem(m_pListView,pGroup);
			groupItems.insert(__tr("Default"),pCur);
			item = new KviRegisteredUsersDialogItem(pCur,u);
		}
		item->setPixmap(0,*(g_pIconManager->getSmallIcon(KVI_SMALLICON_LINUX)));
		++it;
	}
	if(m_pListView->firstChild())
	{
		m_pListView->setSelected(m_pListView->firstChild(),true);
		m_pListView->setCurrentItem(m_pListView->firstChild());
	}
}

void KviRegisteredUsersDialog::closeEvent(QCloseEvent *e)
{
	m_pListView->clear();
	e->accept();
	okClicked();
	//delete this;
}

void KviRegisteredUsersDialog::okClicked()
{
	m_pListView->clear();
	g_pRegisteredUserDataBase->copyFrom(g_pLocalRegisteredUserDataBase);
	g_pApp->restartNotifyLists();
	delete this;
}

void KviRegisteredUsersDialog::cancelClicked()
{
	m_pListView->clear();
	delete this;
}

void KviRegisteredUsersDialog::addClicked()
{
	KviRegisteredUserEntryDialog * dlg = new KviRegisteredUserEntryDialog(this,0);
	int ret = dlg->exec();
	delete dlg;

	if(!g_pRegisteredUsersDialog)return; // we have been deleted!

	if(ret == QDialog::Accepted)
	{
		fillList();
	}
}

void KviRegisteredUsersDialog::addWizardClicked()
{
	KviRegistrationWizard * w = new KviRegistrationWizard("",g_pLocalRegisteredUserDataBase,this,true);
	int ret = w->exec();
	delete w;
	if(!g_pRegisteredUsersDialog)return; // we have been deleted!
	if(ret == QDialog::Accepted)
	{
		fillList();
	}
}

void KviRegisteredUsersDialog::removeClicked()
{
	KviTalListViewItemIterator it( m_pListView,  KviTalListViewItemIterator::Selected );
	while ( it.current() ) {
		KviRegisteredUsersDialogItemBase* b=(KviRegisteredUsersDialogItemBase*)(it.current());
		if(b->type()==KviRegisteredUsersDialogItemBase::User)
		{
			g_pLocalRegisteredUserDataBase->removeUser(((KviRegisteredUsersDialogItem *)(it.current()))->user()->name());
		} else {
			g_pLocalRegisteredUserDataBase->removeGroup(((KviRegisteredUsersGroupItem *)(it.current()))->group()->name());
		}
		++it;
	}
	fillList();
// 	KviRegisteredUsersDialogItem *it = (KviRegisteredUsersDialogItem *)m_pListView->firstChild();
// 	KviRegisteredUsersDialogItemBase* b=(KviRegisteredUsersDialogItemBase*)it;
// 	if(b->type()==KviRegisteredUsersDialogItemBase::User)
// 	{
// 		KviPtrList<KviRegisteredUsersDialogItem> l;
// 		l.setAutoDelete(false);
// 		while(it)
// 		{
// 			if(it->isSelected())l.append(it);
// 			it = (KviRegisteredUsersDialogItem *)it->nextSibling();
// 		}
// 	
// 		for(KviRegisteredUsersDialogItem * i = l.first();i;i = l.next())
// 		{
// 			//g_pLocalRegisteredUserDataBase->removeUser(i->user()->name());
// 			delete i;
// 		}
// 	} else {
// 		
// 	}
}

void KviRegisteredUsersDialog::editClicked()
{
	KviRegisteredUsersDialogItemBase* b=(KviRegisteredUsersDialogItemBase*)(m_pListView->currentItem());
	if(!b)return;
	if(b->type()==KviRegisteredUsersDialogItemBase::User)
	{
		KviRegisteredUsersDialogItem *i = (KviRegisteredUsersDialogItem *)b;
		editItem(i);
	} else {
		KviRegisteredUsersGroupItem *i = (KviRegisteredUsersGroupItem *)b;
		editGroup(i->group());
	}
}

void KviRegisteredUsersDialog::editItem(KviRegisteredUsersDialogItem * i)
{
	KviRegisteredUser * u = i->user();

	i->setUser(0);

	QString szName = u->name();

	KviRegisteredUserEntryDialog * dlg = new KviRegisteredUserEntryDialog(this,u);
	int res = dlg->exec();
	delete dlg;

	if(!g_pRegisteredUsersDialog)return; // we have been deleted!

	if(res == QDialog::Accepted)
	{
		fillList();

		// select the last edited item
		KviTalListViewItem * i = m_pListView->firstChild();
		while(i)
		{
			QString szTmp = i->text(0);
			if(KviQString::equalCI(szTmp,szName))
			{
				m_pListView->setSelected(i,true);
				m_pListView->setCurrentItem(i);
				break;
			}
			i = i->nextSibling();
		}

		return;
	}

	i->setUser(u);
	m_pListView->update();
}


void KviRegisteredUsersDialog::selectionChanged()
{
	bool bHaveSelected = !m_pListView->selectedItem();

	m_pEditButton->setEnabled(bHaveSelected);
	m_pRemoveButton->setEnabled(bHaveSelected);
	m_pExportButton->setEnabled(bHaveSelected);
}


#define KVI_REGUSER_DB_FILE_MAGIC 0x5334DBDB
#define KVI_REGUSER_DB_FILE_VERSION 1

typedef struct _KviReguserDbFileHeader
{
	unsigned int magic;
	unsigned int version;
	unsigned int nentries;
} KviReguserDbFileHeader;

void KviRegisteredUsersDialog::exportClicked()
{
	unsigned int nEntries = 0;

	KviTalListViewItemIterator it( m_pListView, QListViewItemIterator::Selected );
	KviTalListViewItemIterator cit( m_pListView, QListViewItemIterator::Selected );
    while ( cit.current() ) {
		if(((KviRegisteredUsersDialogItemBase *)(cit.current()))->type() == KviRegisteredUsersDialogItemBase::User)
			nEntries++;
        ++cit;
    }

	if(nEntries < 1)
	{
		KviMessageBox::warning(__tr("No entries selected."));
		return;
	}

	QString buffer;

	if(!KviFileDialog::askForSaveFileName(buffer,__tr("Choose a Filename - KVIrc"),0,0,true,true))return;

	if(!g_pRegisteredUsersDialog)return; // we have been deleted!

	KviFile f(buffer);
	if(!f.open(IO_WriteOnly | IO_Truncate))
	{
		KviMessageBox::warning(__tr("Can't open file %Q for writing."),&buffer);
		return;
	}

	KviReguserDbFileHeader hf;
	hf.magic = KVI_REGUSER_DB_FILE_MAGIC;
	hf.version = KVI_REGUSER_DB_FILE_VERSION;
	hf.nentries = nEntries;

	if(f.writeBlock((const char *)&hf,sizeof(KviReguserDbFileHeader)) != sizeof(KviReguserDbFileHeader))goto write_error;

    while ( it.current() ) {
		KviRegisteredUsersDialogItemBase *pBase = (KviRegisteredUsersDialogItemBase *)(it.current());
		if(pBase->type()!=KviRegisteredUsersDialogItemBase::User) continue;
		QString szName = it.current()->text(0);
		KviRegisteredUser * u = ((KviRegisteredUsersDialogItem *)(it.current()))->user();
		if(u)
		{
			if(!f.save(szName))goto write_error;
			KviDict<QString> * pd = u->propertyDict();
			if(pd)
			{
				if(!f.save(pd->count()))goto write_error;
				KviDictIterator<QString> it(*pd);
				while(it.current())
				{
					QString key = it.currentKey();
					if(!f.save(key))goto write_error;
					if(!f.save(*(it.current())))goto write_error;
					++it;
				}
			} else {
				if(!f.save(0))goto write_error;
			}

			KviPtrList<KviIrcMask> * ml = u->maskList();
			if(ml)
			{
				if(!f.save(ml->count()))goto write_error;
				for(KviIrcMask * m = ml->first();m;m = ml->next())
				{
					QString fullMask;
					m->mask(fullMask,KviIrcMask::NickUserHost);
					if(!f.save(fullMask))goto write_error;
				}
			} else {
				if(!f.save(0))goto write_error;
			}

			QString avatar;
			if(u->getProperty("avatar",avatar))
			{
				KviAvatar * av = g_pIconManager->getAvatar(QString::null,avatar);
				if(av)
				{
					if(!av->pixmap()->isNull())
					{
						if(!f.save(1))goto write_error;
						QImageIO io;
						io.setImage(av->pixmap()->convertToImage());
						io.setIODevice(&f);
						io.setFormat("PNG");
						if(!io.write())goto write_error;
					} else {
						if(!f.save(0))goto write_error;
					}
				} else {
					if(!f.save(0))goto write_error;
				}
			} else {
				if(!f.save(0))goto write_error;
			}
		}
		++it;
	}

	goto succesfull_export;


write_error:
	KviMessageBox::warning(__tr("Can't export the registered users database: Write error."));
	f.close();
	return;

succesfull_export:

	f.close();
}

void KviRegisteredUsersDialog::importClicked()
{
	//KviStr buffer;
	QString buffer;

	if(!KviFileDialog::askForOpenFileName(buffer,__tr("Choose a Filename - KVIrc")))return;

	if(!g_pRegisteredUsersDialog)return; // we have been deleted!

	KviFile f(buffer);
	if(!f.open(IO_ReadOnly))
	{
		KviMessageBox::warning(__tr2qs("Can't open file %s for reading."),&buffer);
		return;
	}

	KviReguserDbFileHeader hf;
	unsigned int idx;

	if(f.readBlock((char *)&hf,sizeof(KviReguserDbFileHeader)) != sizeof(KviReguserDbFileHeader))goto read_error;

	if(hf.magic != KVI_REGUSER_DB_FILE_MAGIC)
	{
		KviMessageBox::warning(__tr2qs("The file %s doesn't appear to be a valid registered users database."),&buffer);
		f.close();
		return;
	}

	if(hf.version != KVI_REGUSER_DB_FILE_VERSION)
	{
		KviMessageBox::warning(__tr2qs("The file %s contains an invalid registered users database version."),&buffer);
		f.close();
		return;
	}



	for(idx = 0;idx < hf.nentries;idx++)
	{
		QString szName;
		if(!f.load(szName))goto read_error;
		KviRegisteredUser * u = g_pLocalRegisteredUserDataBase->getUser(szName);
		unsigned int count = 0;
		if(!f.load(count))goto read_error;
		for(unsigned int up = 0;up < count;up++)
		{
			QString szKey,szValue;
			if(!f.load(szKey))goto read_error;
			if(!f.load(szValue))goto read_error;
			u->setProperty(szKey,szValue);
		}
		if(!f.load(count))goto read_error;
		for(unsigned int um = 0;um < count;um++)
		{
			QString szMask;
			if(!f.load(szMask))goto read_error;
			if(!szMask.isEmpty())
			{
				KviIrcMask * m = new KviIrcMask(szMask);
				g_pLocalRegisteredUserDataBase->addMask(u,m);
			}
		}
		if(!f.load(count))goto read_error;
		if(count)
		{
			// there is an avatar
			QImageIO io;
			QImage img;
			io.setImage(img);
			io.setIODevice(&f);
			io.setFormat("PNG");

			if(!io.read())goto read_error;

			img = io.image();

			if(img.isNull())debug("Ops.. readed a null image ?");

			KviStr fName = u->name();
			kvi_encodeFileName(fName);

			KviStr fPath;
			int rnm = 0 ;
			do
			{
				g_pApp->getLocalKvircDirectory(fPath,KviApp::Avatars,fName.ptr());
				fPath.append(KviStr::Format,"%d.png",rnm);
				rnm++;
			} while(KviFileUtils::fileExists(fPath.ptr()));

			if(!img.save(fPath.ptr(),"PNG"))
			{
				debug("Can't save image %s",fPath.ptr());
			} else {
				u->setProperty("avatar",fPath.ptr());
			}
		}
	}

	goto succesfull_import;


read_error:
	KviMessageBox::warning(__tr("Can't import the registered users database: Read error."));
	f.close();
	return;

succesfull_import:

	f.close();
	fillList();
}



#include "edituser.moc"
