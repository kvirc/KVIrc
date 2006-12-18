//
//   File : wizard.cpp
//   Creation date : Fri Jun 26 2002 21:21:21 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)
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


#include "wizard.h"

#include "kvi_locale.h"
#include "kvi_list.h"
#include "kvi_app.h"
#include "kvi_ircmask.h"
#include "kvi_selectors.h"
#include "kvi_pixmap.h"
#include "kvi_regusersdb.h"
#include "kvi_iconmanager.h"

#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qframe.h>


extern KVIRC_API KviRegisteredUserDataBase * g_pRegisteredUserDataBase;
extern KviPtrList<KviRegistrationWizard> * g_pRegistrationWizardList;

KviRegistrationWizard::KviRegistrationWizard(const char * startMask,KviRegisteredUserDataBase * db,QWidget * par,bool bModal)
: QWizard(par,"regusers_wizard",bModal)
{
	m_pDb = db;
	
	m_bModal = bModal;

	KviIrcMask mask(startMask ? startMask : "*!*@*");

	g_pRegistrationWizardList->append(this);

    QGridLayout* m_pPage1Layout;
    QGridLayout* m_pPage2Layout;
    QGridLayout* m_pPage3Layout;
    QGridLayout* m_pPage4Layout;
    QGridLayout* m_pPage5Layout;

    QLabel* TextLabel10_2;
    QLabel* TextLabel10;
    QLabel* TextLabel10_3;
    QLabel* TextLabel10_3_2;

	QFrame * f;

//    resize( 490, 456 ); 
    setCaption(__tr2qs("User Registration Wizard - KVIrc"));
	setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_LINUX)));

    setSizeGripEnabled( TRUE );

    m_pPage1 = new QWidget(this);
    m_pPage1Layout = new QGridLayout(m_pPage1); 
    m_pPage1Layout->setSpacing(4);
    m_pPage1Layout->setMargin(8);

    m_pLabel1 = new QLabel(m_pPage1);
    m_pLabel1->setText(__tr2qs("<p>Welcome to the user registration wizard.<br>This process allows you to add an IRC user to the database and set properties for that user. KVIrc will be (hopefully) able to recognize the user, add him to the notify list, and display the associated avatar.<br><br>First of all, you must insert an <b>entry name or real name</b> for the user you're going to register. The name will be used to identify the database entry and has no specific requirements, it can be a given name, nickname, or just some text to remind you of the real person.<br>Examples: \"George W Bush\", \"Dubya\".\n</p>" ));

    m_pPage1Layout->addWidget(m_pLabel1, 0, 0);

    m_pEditRealName = new QLineEdit(m_pPage1);
    //m_pEditRealName->setAlignment(int(QLineEdit::AlignHCenter));

    m_pPage1Layout->addWidget(m_pEditRealName,1,0);
    addPage( m_pPage1,__tr2qs("Step 1: Entry Name"));

	if(mask.nick() != "*")m_pEditRealName->setText(mask.nick());

	connect(m_pEditRealName,SIGNAL(textChanged(const QString &)),this,SLOT(realNameChanged(const QString &)));

	// PAGE 2


    m_pPage2 = new QWidget(this);
    m_pPage2Layout = new QGridLayout( m_pPage2 ); 
    m_pPage2Layout->setSpacing( 4 );
    m_pPage2Layout->setMargin( 8 );

    m_pLabel2 = new QLabel( m_pPage2);
    m_pLabel2->setText(__tr2qs("<p>A registered user is identified by one or more <b>IRC masks</b>.<br>A mask must be in the following form:<br><b>nickname!username@hostname</b><br>and can contain wildcard characters '*' and '?'. Be careful in choosing the masks, as they are the only way to verify the identity of a registered user.<br><br>You can enter at most two masks here, if you wish to add more masks, use the \"<b>Edit</b>\" button in the Registered Users dialog. You must enter at least one mask.</p>"));

    m_pPage2Layout->addMultiCellWidget(m_pLabel2,0,0,0,4);

	//KviStr szMask;

	//mask.mask(szMask,11);

	//KviIrcMask masktempl(szMask.ptr());

    m_pNicknameEdit1 = new QLineEdit(m_pPage2);
    m_pNicknameEdit1->setAlignment(int(QLineEdit::AlignRight));
	if(mask.nick() != "*")m_pNicknameEdit1->setText(mask.nick());
	connect(m_pNicknameEdit1,SIGNAL(textChanged(const QString &)),this,SLOT(maskChanged(const QString &)));
    m_pPage2Layout->addWidget( m_pNicknameEdit1, 1, 0 );

    m_pUsernameEdit1 = new QLineEdit(m_pPage2);
    m_pUsernameEdit1->setAlignment(int(QLineEdit::AlignHCenter));
	if(mask.hasUser())m_pUsernameEdit1->setText(mask.user());
	connect(m_pUsernameEdit1,SIGNAL(textChanged(const QString &)),this,SLOT(maskChanged(const QString &)));
    m_pPage2Layout->addWidget(m_pUsernameEdit1,1,2);

	m_pHostEdit1 = new QLineEdit(m_pPage2);
	if(mask.hasHost())m_pHostEdit1->setText(mask.host());
	connect(m_pHostEdit1,SIGNAL(textChanged(const QString &)),this,SLOT(maskChanged(const QString &)));
	m_pPage2Layout->addWidget(m_pHostEdit1,1,4);

    m_pNicknameEdit2 = new QLineEdit(m_pPage2);
    m_pNicknameEdit2->setAlignment(int(QLineEdit::AlignRight));
    m_pPage2Layout->addWidget(m_pNicknameEdit2,2,0);

    m_pUsernameEdit2 = new QLineEdit(m_pPage2);
    m_pUsernameEdit2->setAlignment(int(QLineEdit::AlignHCenter));
    m_pPage2Layout->addWidget(m_pUsernameEdit2,2,2);

    m_pHostEdit2 = new QLineEdit(m_pPage2);
    m_pPage2Layout->addWidget(m_pHostEdit2,2,4);

    TextLabel10_2 = new QLabel(m_pPage2);
    TextLabel10_2->setText("<center><b>!</b></center>");
    TextLabel10_2->setAlignment(int(QLabel::AlignCenter));
    m_pPage2Layout->addWidget(TextLabel10_2,2,1);

    TextLabel10 = new QLabel(m_pPage2);
    TextLabel10->setText("<center><b>!</b></center>");
    TextLabel10->setAlignment(int(QLabel::AlignCenter));
    m_pPage2Layout->addWidget(TextLabel10,1,1);

    TextLabel10_3 = new QLabel(m_pPage2);
    TextLabel10_3->setText("<center><b>@</b></center>");
    TextLabel10_3->setAlignment(int(QLabel::AlignCenter));
    m_pPage2Layout->addWidget(TextLabel10_3,1,3);

    TextLabel10_3_2 = new QLabel(m_pPage2);
    TextLabel10_3_2->setText("<center><b>@</b></center>");
    TextLabel10_3_2->setAlignment(int(QLabel::AlignCenter));
    m_pPage2Layout->addWidget(TextLabel10_3_2,2,3);

    addPage(m_pPage2,__tr2qs("Step 2: Mask Selection"));





    m_pPage3 = new QWidget(this);
    m_pPage3Layout = new QGridLayout(m_pPage3); 
    m_pPage3Layout->setSpacing(4);
    m_pPage3Layout->setMargin(8);

    m_pLabel3 = new QLabel(m_pPage3);
    m_pLabel3->setText(__tr2qs("<p>If you want to store an avatar image for this user, you can set it here. KVIrc will show the avatar in the userlist next to the user's nickname.<br>An avatar can be in any supported image format (PNG is recommended). Keep in mind that KVIrc stores avatars in memory and has to resize them to fit in the userlist, thus it's better to use small, low-resolution images.</p>"));
    m_pPage3Layout->addWidget(m_pLabel3,0,0);

	f = new QFrame(m_pPage3);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	m_pPage3Layout->addWidget(f,1,0);

	m_pAvatar = new KviPixmap();
	m_pAvatarSelector = new KviPixmapSelector(m_pPage3,__tr2qs("Store an avatar for this user"),m_pAvatar,true);
	m_pPage3Layout->addWidget(m_pAvatarSelector,2,0);

	m_pPage3Layout->setRowStretch(0,1);

    addPage(m_pPage3,__tr2qs( "Step 3: Avatar Selection"));




    m_pPage4 = new QWidget(this);
    m_pPage4Layout = new QGridLayout(m_pPage4); 
    m_pPage4Layout->setSpacing(4);
    m_pPage4Layout->setMargin(8);

    m_pLabel4 = new QLabel(m_pPage4);
    m_pLabel4->setText(__tr2qs("<p>If you want to be notified when this user is online or goes offline, you must specify the list of nicknames that KVIrc will look for.<br><br>You can enter at most two nicknames here, if you wish to add more nicknames, use the \"<b>Edit</b>\" button in the Registered Users dialog.</p>"));
    m_pPage4Layout->addMultiCellWidget(m_pLabel4,0,0,0,1);


	f = new QFrame(m_pPage4);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	m_pPage4Layout->addMultiCellWidget(f,1,1,0,1);

    m_pNotifyCheck = new QCheckBox(m_pPage4);
    m_pNotifyCheck->setText(__tr2qs("Add this user to the notify list"));
	m_pNotifyCheck->setChecked(false);
	connect(m_pNotifyCheck,SIGNAL(toggled(bool)),this,SLOT(notifyCheckToggled(bool)));
    m_pPage4Layout->addMultiCellWidget(m_pNotifyCheck,2,2,0,1);

    m_pNotifyNickLabel1 = new QLabel(m_pPage4);
    m_pNotifyNickLabel1->setText(__tr2qs("Nickname:"));
    m_pPage4Layout->addWidget(m_pNotifyNickLabel1,3,0);

    m_pNotifyNickLabel2 = new QLabel(m_pPage4);
    m_pNotifyNickLabel2->setText(__tr2qs("Nickname 2:"));
    m_pPage4Layout->addWidget(m_pNotifyNickLabel2,4,0);

    m_pNotifyNickEdit1 = new QLineEdit(m_pPage4);
	if(mask.nick() != "*")m_pNotifyNickEdit1->setText(mask.nick());
	connect(m_pNotifyNickEdit1,SIGNAL(textChanged(const QString &)),this,SLOT(notifyNickChanged(const QString &)));
    m_pPage4Layout->addWidget(m_pNotifyNickEdit1,3,1);

    m_pNotifyNickEdit2 = new QLineEdit(m_pPage4);
	connect(m_pNotifyNickEdit2,SIGNAL(textChanged(const QString &)),this,SLOT(notifyNickChanged(const QString &)));
    m_pPage4Layout->addWidget(m_pNotifyNickEdit2,4,1);

	m_pPage4Layout->setRowStretch(0,1);

    addPage(m_pPage4,__tr2qs("Step 4: Notify List"));





    m_pPage5 = new QWidget(this);
    m_pPage5Layout = new QGridLayout(m_pPage5); 
    m_pPage5Layout->setSpacing(4);
    m_pPage5Layout->setMargin(8);

    m_pTextLabel5 = new QLabel(m_pPage5);
    m_pTextLabel5->setText(__tr2qs("<p>That's it. The user registration has been completed.<br><br>Click \"<b>Finish</b>\" to close this dialog.</p>"));
    m_pPage5Layout->addWidget(m_pTextLabel5,0,0);

    addPage(m_pPage5,__tr2qs("Registration Complete"));

	setFinishEnabled(m_pPage5,true);
	QString dummy;
	maskChanged(dummy);
	realNameChanged(dummy);
	notifyCheckToggled(false);

	setMinimumSize(QSize(350,420));

    // signals and slots connections
}

KviRegistrationWizard::~KviRegistrationWizard()
{
	delete m_pAvatar;
	g_pRegistrationWizardList->setAutoDelete(false);
	g_pRegistrationWizardList->removeRef(this);
	g_pRegistrationWizardList->setAutoDelete(true);
}

void KviRegistrationWizard::reject()
{
	QWizard::reject();
	if(!m_bModal)delete this;
//	hide();
//	g_pApp->collectGarbage(this);
}

void KviRegistrationWizard::accept()
{
	QWizard::accept();
	bool bLocalDb = true;
	if(!m_pDb)
	{
		bLocalDb = false;
		m_pDb = g_pRegisteredUserDataBase;
	}

	KviStr name = m_pEditRealName->text();
	KviRegisteredUser * u;


	if(bLocalDb)
	{
		if(name.isEmpty())name = "user";

		KviStr szNameOk = name;

		int idx = 1;

		do {
			u = m_pDb->findUserByName(szNameOk.ptr());
			if(u)
			{
				szNameOk.sprintf("%s%d",name.ptr(),idx);
				idx++;
			}
		} while(u);

		u = m_pDb->addUser(szNameOk.ptr());

	} else {
		u = m_pDb->findUserByName(name.ptr());
		if(!u)u = m_pDb->addUser(name.ptr());
	}

	if(!u)
	{
		// ops... no way
		// FIXME: spit an error message ?
		debug("Ops.. something wrong with the regusers db");
		delete this;
		return;
	}

	KviStr m1 = m_pNicknameEdit1->text();
	KviStr m2 = m_pUsernameEdit1->text();
	KviStr m3 = m_pHostEdit1->text();
	KviStr mask(KviStr::Format,"%s!%s@%s",m1.ptr(),m2.ptr(),m3.ptr());
	KviIrcMask * mk = new KviIrcMask(mask.ptr());
	m_pDb->removeMask(*mk);
	m_pDb->addMask(u,mk);


	m1 = m_pNicknameEdit2->text();
	m2 = m_pUsernameEdit2->text();
	if(m2.isEmpty())m2 = "*";
	if(m3.isEmpty())m3 = "*";
	m3 = m_pHostEdit2->text();
	KviStr mask2(KviStr::Format,"%s!%s@%s",m1.ptr(),m2.ptr(),m3.ptr());
	mk = new KviIrcMask(mask2.ptr());
	if(mk->nick() != "*")
	{
		m_pDb->removeMask(*mk);
		m_pDb->addMask(u,mk);
	} else {
		delete mk;
	}

	m_pAvatarSelector->commit();

	bool bSetAvatar = false;

	if(!m_pAvatar->isNull())
	{
		KviStr szPath = m_pAvatar->path();
		u->setProperty("avatar",szPath.ptr());
		bSetAvatar = true;
	}

	if(m_pNotifyCheck->isChecked())
	{
		m1 = m_pNotifyNickEdit1->text();
		m2 = m_pNotifyNickEdit2->text();
		if(m2.hasData())
		{
			if(m1.hasData())m1.append(' ');
			m1.append(m2);
		}
	
		if(m1.hasData())
		{
			u->setProperty("notify",m1.ptr());
			if(!bLocalDb)g_pApp->restartNotifyLists();
		}
	}

	if(bSetAvatar && !bLocalDb)g_pApp->resetAvatarForMatchingUsers(u);


	if(!m_bModal)delete this;
//	hide();
//	g_pApp->collectGarbage(this);
}


void KviRegistrationWizard::showEvent(QShowEvent *e)
{
	if(height() < 420)resize(width(),420);
	move((g_pApp->desktop()->width() - width())/2,(g_pApp->desktop()->height() - height())/2);
	QWizard::showEvent(e);
}

void KviRegistrationWizard::maskChanged(const QString &)
{
	KviStr tmp1 = m_pNicknameEdit1->text();
	KviStr tmp2 = m_pUsernameEdit1->text();
	KviStr tmp3 = m_pHostEdit1->text();

	if(tmp1.isEmpty())
	{
		setNextEnabled(m_pPage2,false);
		return;
	}
	
	if(tmp2.isEmpty())
	{
		setNextEnabled(m_pPage2,false);
		return;
	}

	if(tmp3.isEmpty())
	{
		setNextEnabled(m_pPage2,false);
		return;
	}

	KviStr mask(KviStr::Format,"%s!%s@%s",tmp1.ptr(),tmp2.ptr(),tmp3.ptr());

	//KviIrcMask m(mask.ptr());

	setNextEnabled(m_pPage2,mask!="*!*@*");
}

void KviRegistrationWizard::realNameChanged(const QString &)
{
	QString tmp = m_pEditRealName->text();
	setNextEnabled(m_pPage1,!(tmp.isEmpty() || tmp.isNull()));
}

void KviRegistrationWizard::notifyNickChanged(const QString &)
{
	bool bYes = !m_pNotifyCheck->isChecked();

	if(!bYes)
	{
		// we need at least one nickname then :)
		KviStr tmp = m_pNotifyNickEdit1->text();
		if(tmp.hasData())
		{
			bYes = true;
		} else {
			tmp = m_pNotifyNickEdit2->text();
			if(tmp.hasData())bYes = true;
		}
	}

	setNextEnabled(m_pPage4,bYes);
}

void KviRegistrationWizard::notifyCheckToggled(bool)
{
	bool bYes = m_pNotifyCheck->isChecked();

	m_pNotifyNickEdit1->setEnabled(bYes);
	m_pNotifyNickEdit2->setEnabled(bYes);
	m_pNotifyNickLabel1->setEnabled(bYes);
	m_pNotifyNickLabel2->setEnabled(bYes);

	QString dummy;
	notifyNickChanged(dummy);
}

#include "wizard.moc"
