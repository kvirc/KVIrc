//=============================================================================
//
//   File : RegistrationWizard.cpp
//   Creation date : Fri Jun 26 2002 21:21:21 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "RegistrationWizard.h"

#include "KviLocale.h"
#include "KviPointerList.h"
#include "KviApplication.h"
#include "KviIrcMask.h"
#include "KviSelectors.h"
#include "KviPixmap.h"
#include "KviRegisteredUserDataBase.h"
#include "KviIconManager.h"

#include <QDesktopWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>
#include <QLayout>
#include <QVariant>
#include <QFrame>

extern KVIRC_API KviRegisteredUserDataBase * g_pRegisteredUserDataBase;
extern KviPointerList<RegistrationWizard> * g_pRegistrationWizardList;

RegistrationWizard::RegistrationWizard(const QString & startMask, KviRegisteredUserDataBase * db, QWidget * par, bool bModal)
    : KviTalWizard(par)
{
	m_pDb = db;

	setModal(bModal);

	m_bModal = bModal;

	KviIrcMask mask(!startMask.isEmpty() ? startMask.toUtf8().data() : "*!*@*");

	g_pRegistrationWizardList->append(this);

	QGridLayout * m_pPage1Layout;
	QGridLayout * m_pPage2Layout;
	QGridLayout * m_pPage3Layout;
	QGridLayout * m_pPage4Layout;
	QGridLayout * m_pPage5Layout;

	QLabel * TextLabel10_2;
	QLabel * TextLabel10;
	QLabel * TextLabel10_3;
	QLabel * TextLabel10_3_2;

	QFrame * f;

	//    resize( 490, 456 );
	setWindowTitle(__tr2qs_ctx("User Registration Wizard - KVIrc", "register"));
	setWindowIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Linux)));

	setSizeGripEnabled(true);

	m_pPage1 = new QWidget(this);
	m_pPage1Layout = new QGridLayout(m_pPage1);
	m_pPage1Layout->setSpacing(4);
	m_pPage1Layout->setMargin(8);

	m_pLabel1 = new QLabel(m_pPage1);
	m_pLabel1->setText(__tr2qs_ctx("<p>Welcome to the user registration wizard.<br>This process allows you to add an IRC user to the database and set properties for that user. KVIrc will be (hopefully) able to recognize the user, add him to the notify list, and display the associated avatar.<br><br>First of all, you must insert an <b>entry name or real name</b> for the user you're going to register. The name will be used to identify the database entry and has no specific requirements, it can be a given name, nickname, or just some text to remind you of the real person.<br>Examples: \"George W Bush\", \"Dubya\".\n</p>", "register"));
	m_pLabel1->setWordWrap(true);

	m_pPage1Layout->addWidget(m_pLabel1, 0, 0, 1, 2);

	m_pEditRealName = new QLineEdit(m_pPage1);
	//m_pEditRealName->setAlignment(int(QLineEdit::AlignHCenter));

	m_pPage1Layout->addWidget(m_pEditRealName, 3, 1);

	m_pEntryNameLabel = new QLabel(m_pPage1);
	m_pEntryNameLabel->setText(__tr2qs_ctx("Entry name:", "register"));
	m_pPage1Layout->addWidget(m_pEntryNameLabel, 3, 0);
	m_pPage1Layout->setRowStretch(1, 1);

	addPage(m_pPage1, __tr2qs_ctx("Step 1: Entry Name", "register"));

	if(mask.nick() != "*")
		m_pEditRealName->setText(mask.nick());

	connect(m_pEditRealName, SIGNAL(textChanged(const QString &)), this, SLOT(realNameChanged(const QString &)));

	// PAGE 2

	m_pPage2 = new QWidget(this);
	m_pPage2Layout = new QGridLayout(m_pPage2);
	m_pPage2Layout->setSpacing(4);
	m_pPage2Layout->setMargin(8);

	m_pLabel2 = new QLabel(m_pPage2);
	m_pLabel2->setText(__tr2qs_ctx("<p>A registered user is identified by one or more <b>IRC masks</b>.<br>A mask must be in the following form:<br><b>nickname!username@hostname</b><br>and can contain wildcard characters '*' and '?'. Be careful in choosing the masks, as they are the only way to verify the identity of a registered user.<br><br>You can enter at most two masks here, if you wish to add more masks, use the \"<b>Edit</b>\" button in the Registered Users dialog. You must enter at least one mask.</p>", "register"));
	m_pLabel2->setWordWrap(true);

	m_pPage2Layout->addWidget(m_pLabel2, 0, 0, 1, 5);

	//KviCString szMask;

	//mask.mask(szMask,11);

	//KviIrcMask masktempl(szMask.ptr());

	m_pNicknameEdit1 = new QLineEdit(m_pPage2);
	m_pNicknameEdit1->setAlignment(Qt::AlignRight);
	if(mask.nick() != "*")
		m_pNicknameEdit1->setText(mask.nick());
	connect(m_pNicknameEdit1, SIGNAL(textChanged(const QString &)), this, SLOT(maskChanged(const QString &)));
	m_pPage2Layout->addWidget(m_pNicknameEdit1, 3, 0);

	m_pUsernameEdit1 = new QLineEdit(m_pPage2);
	m_pUsernameEdit1->setAlignment(Qt::AlignHCenter);
	if(mask.hasUser())
		m_pUsernameEdit1->setText(mask.user());
	connect(m_pUsernameEdit1, SIGNAL(textChanged(const QString &)), this, SLOT(maskChanged(const QString &)));
	m_pPage2Layout->addWidget(m_pUsernameEdit1, 3, 2);

	m_pHostEdit1 = new QLineEdit(m_pPage2);
	if(mask.hasHost())
		m_pHostEdit1->setText(mask.host());
	connect(m_pHostEdit1, SIGNAL(textChanged(const QString &)), this, SLOT(maskChanged(const QString &)));
	m_pPage2Layout->addWidget(m_pHostEdit1, 3, 4);

	m_pNicknameEdit2 = new QLineEdit(m_pPage2);
	m_pNicknameEdit2->setAlignment(Qt::AlignRight);
	m_pPage2Layout->addWidget(m_pNicknameEdit2, 4, 0);

	m_pUsernameEdit2 = new QLineEdit(m_pPage2);
	m_pUsernameEdit2->setAlignment(Qt::AlignHCenter);
	m_pPage2Layout->addWidget(m_pUsernameEdit2, 4, 2);

	m_pHostEdit2 = new QLineEdit(m_pPage2);
	m_pPage2Layout->addWidget(m_pHostEdit2, 4, 4);

	TextLabel10_2 = new QLabel(m_pPage2);
	TextLabel10_2->setText("<b>!</b>");
	TextLabel10_2->setAlignment(Qt::AlignCenter);
	m_pPage2Layout->addWidget(TextLabel10_2, 4, 1);

	TextLabel10 = new QLabel(m_pPage2);
	TextLabel10->setText("<b>!</b>");
	TextLabel10->setAlignment(Qt::AlignCenter);
	m_pPage2Layout->addWidget(TextLabel10, 3, 1);

	TextLabel10_3 = new QLabel(m_pPage2);
	TextLabel10_3->setText("<b>@</b>");
	TextLabel10_3->setAlignment(Qt::AlignCenter);
	m_pPage2Layout->addWidget(TextLabel10_3, 3, 3);

	TextLabel10_3_2 = new QLabel(m_pPage2);
	TextLabel10_3_2->setText("<b>@</b>");
	TextLabel10_3_2->setAlignment(Qt::AlignCenter);
	m_pPage2Layout->addWidget(TextLabel10_3_2, 4, 3);
	m_pPage2Layout->setRowStretch(1, 1);

	addPage(m_pPage2, __tr2qs_ctx("Step 2: Mask Selection", "register"));

	// PAGE 3

	m_pPage3 = new QWidget(this);
	m_pPage3Layout = new QGridLayout(m_pPage3);
	m_pPage3Layout->setSpacing(4);
	m_pPage3Layout->setMargin(8);

	m_pLabel3 = new QLabel(m_pPage3);
	m_pLabel3->setText(__tr2qs_ctx("<p>If you want to store an avatar image for this user, you can set it here. KVIrc will show the avatar in the userlist next to the user's nickname.<br>An avatar can be in any supported image format (PNG is recommended). Keep in mind that KVIrc stores avatars in memory and has to resize them to fit in the userlist, thus it's better to use small, low-resolution images.</p>", "register"));
	m_pLabel3->setWordWrap(true);
	m_pPage3Layout->addWidget(m_pLabel3, 0, 0);

	f = new QFrame(m_pPage3);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	m_pPage3Layout->addWidget(f, 1, 0);

	m_pAvatar = new KviPixmap();
	m_pAvatarSelector = new KviPixmapSelector(m_pPage3, __tr2qs_ctx("Store an avatar for this user:", "register"), m_pAvatar, true);
	m_pPage3Layout->addWidget(m_pAvatarSelector, 2, 0);
	m_pPage3Layout->setRowStretch(2, 1);

	addPage(m_pPage3, __tr2qs_ctx("Step 3: Avatar Selection", "register"));

	// PAGE 4

	m_pPage4 = new QWidget(this);
	m_pPage4Layout = new QGridLayout(m_pPage4);
	m_pPage4Layout->setSpacing(4);
	m_pPage4Layout->setMargin(8);

	m_pLabel4 = new QLabel(m_pPage4);
	m_pLabel4->setText(__tr2qs_ctx("<p>If you want to be notified when this user is online or goes offline, you must specify the list of nicknames that KVIrc will look for.<br><br>You can enter at most two nicknames here, if you wish to add more nicknames, use the \"<b>Edit</b>\" button in the  \"Registered Users\" dialog.</p>", "register"));
	m_pLabel4->setWordWrap(true);
	m_pPage4Layout->addWidget(m_pLabel4, 0, 0, 1, 5);

	m_pNotifyCheck = new QCheckBox(m_pPage4);
	m_pNotifyCheck->setText(__tr2qs_ctx("Add this user to the notify list", "register"));
	m_pNotifyCheck->setChecked(false);
	connect(m_pNotifyCheck, SIGNAL(toggled(bool)), this, SLOT(notifyCheckToggled(bool)));
	m_pPage4Layout->addWidget(m_pNotifyCheck, 3, 0, 1, 2);

	m_pNotifyNickLabel1 = new QLabel(m_pPage4);
	m_pNotifyNickLabel1->setText(__tr2qs_ctx("Nickname:", "register"));
	m_pPage4Layout->addWidget(m_pNotifyNickLabel1, 4, 0);

	m_pNotifyNickLabel2 = new QLabel(m_pPage4);
	m_pNotifyNickLabel2->setText(__tr2qs_ctx("Nickname 2:", "register"));
	m_pPage4Layout->addWidget(m_pNotifyNickLabel2, 5, 0);

	m_pNotifyNickEdit1 = new QLineEdit(m_pPage4);
	if(mask.nick() != "*")
		m_pNotifyNickEdit1->setText(mask.nick());
	connect(m_pNotifyNickEdit1, SIGNAL(textChanged(const QString &)), this, SLOT(notifyNickChanged(const QString &)));
	m_pPage4Layout->addWidget(m_pNotifyNickEdit1, 4, 1);

	m_pNotifyNickEdit2 = new QLineEdit(m_pPage4);
	connect(m_pNotifyNickEdit2, SIGNAL(textChanged(const QString &)), this, SLOT(notifyNickChanged(const QString &)));
	m_pPage4Layout->addWidget(m_pNotifyNickEdit2, 5, 1);
	m_pPage4Layout->setRowStretch(1, 1);

	addPage(m_pPage4, __tr2qs_ctx("Step 4: Notify List", "register"));

	// PAGE 5

	m_pPage5 = new QWidget(this);
	m_pPage5Layout = new QGridLayout(m_pPage5);
	m_pPage5Layout->setSpacing(4);
	m_pPage5Layout->setMargin(8);

	m_pTextLabel5 = new QLabel(m_pPage5);
	m_pTextLabel5->setText(__tr2qs_ctx("<p>That's it. The user registration has been completed.<br><br>Click \"<b>Finish</b>\" to close this dialog.</p>", "register"));
	m_pTextLabel5->setWordWrap(true);
	m_pPage5Layout->addWidget(m_pTextLabel5, 0, 0);
	m_pPage5Layout->setRowStretch(1, 1);

	addPage(m_pPage5, __tr2qs_ctx("Registration Complete", "register"));

	setFinishEnabled(m_pPage5, true);
	QString dummy;
	maskChanged(dummy);
	realNameChanged(dummy);
	notifyCheckToggled(false);

	setMinimumSize(QSize(350, 420));

	// signals and slots connections
}

RegistrationWizard::~RegistrationWizard()
{
	delete m_pAvatar;
	g_pRegistrationWizardList->setAutoDelete(false);
	g_pRegistrationWizardList->removeRef(this);
	g_pRegistrationWizardList->setAutoDelete(true);
}

void RegistrationWizard::reject()
{
	KviTalWizard::reject();
	if(!m_bModal)
		delete this;
	//	hide();
	//	g_pApp->collectGarbage(this);
}

void RegistrationWizard::accept()
{
	bool bLocalDb = true;
	if(!m_pDb)
	{
		bLocalDb = false;
		m_pDb = g_pRegisteredUserDataBase;
	}

	QString szName = m_pEditRealName->text();
	KviRegisteredUser * u;

	if(bLocalDb)
	{
		if(szName.isEmpty())
			szName = "user";

		QString szNameOk = szName;

		int idx = 1;

		do
		{
			u = m_pDb->findUserByName(szNameOk);
			if(u)
			{
				szNameOk = QString("%1%2").arg(szNameOk).arg(idx);
				idx++;
			}
		} while(u);

		u = m_pDb->addUser(szNameOk);
	}
	else
	{
		u = m_pDb->findUserByName(szName);
		if(!u)
			u = m_pDb->addUser(szName);
	}

	if(!u)
	{
		// ops... no way
		// FIXME: spit an error message ?
		qDebug("Oops! Something wrong with the regusers DB");
		//delete this;
		return;
	}

	KviIrcMask * mk;
	QString m1 = m_pNicknameEdit1->text();
	QString m2 = m_pUsernameEdit1->text();
	QString m3 = m_pHostEdit1->text();
	if(!(m1.isEmpty() && m2.isEmpty() && m3.isEmpty()))
	{
		mk = new KviIrcMask(m1, m2, m3);
		m_pDb->removeMask(*mk);
		m_pDb->addMask(u, mk);
	}

	m1 = m_pNicknameEdit2->text();
	m2 = m_pUsernameEdit2->text();
	m3 = m_pHostEdit2->text();
	if(!(m1.isEmpty() && m2.isEmpty() && m3.isEmpty()))
	{
		if(m1.isEmpty())
			m1 = "*";
		if(m2.isEmpty())
			m2 = "*";
		if(m3.isEmpty())
			m3 = "*";
		mk = new KviIrcMask(m1, m2, m3);
		m_pDb->removeMask(*mk);
		m_pDb->addMask(u, mk);
	}

	m_pAvatarSelector->commit();

	bool bSetAvatar = false;

	if(!m_pAvatar->isNull())
	{
		QString szPath = m_pAvatar->path();
		u->setProperty("avatar", szPath);
		bSetAvatar = true;
	}

	if(m_pNotifyCheck->isChecked())
	{
		m1 = m_pNotifyNickEdit1->text();
		m2 = m_pNotifyNickEdit2->text();
		if(!m2.isEmpty())
		{
			if(!m1.isEmpty())
				m1.append(' ');
			m1.append(m2);
		}

		if(!m1.isEmpty())
		{
			u->setProperty("notify", m1);
			if(!bLocalDb)
				g_pApp->restartNotifyLists();
		}
	}

	if(bSetAvatar && !bLocalDb)
		g_pApp->resetAvatarForMatchingUsers(u);

	KviTalWizard::accept();

	//	if(!m_bModal)delete this;
	//	hide();
	//	g_pApp->collectGarbage(this);
}

void RegistrationWizard::showEvent(QShowEvent * e)
{
	if(height() < 420)
		resize(width(), 420);

	QRect rect = g_pApp->desktop()->screenGeometry(g_pApp->desktop()->primaryScreen());
	move((rect.width() - width()) / 2, (rect.height() - height()) / 2);
	KviTalWizard::showEvent(e);
}

void RegistrationWizard::maskChanged(const QString &)
{
	KviCString tmp1 = m_pNicknameEdit1->text();
	KviCString tmp2 = m_pUsernameEdit1->text();
	KviCString tmp3 = m_pHostEdit1->text();

	if(tmp1.isEmpty())
	{
		setNextEnabled(m_pPage2, false);
		return;
	}

	if(tmp2.isEmpty())
	{
		setNextEnabled(m_pPage2, false);
		return;
	}

	if(tmp3.isEmpty())
	{
		setNextEnabled(m_pPage2, false);
		return;
	}

	KviCString mask(KviCString::Format, "%s!%s@%s", tmp1.ptr(), tmp2.ptr(), tmp3.ptr());

	//KviIrcMask m(mask.ptr());

	setNextEnabled(m_pPage2, mask != "*!*@*");
}

void RegistrationWizard::realNameChanged(const QString &)
{
	QString tmp = m_pEditRealName->text();
	setNextEnabled(m_pPage1, !(tmp.isEmpty() || tmp.isNull()));
}

void RegistrationWizard::notifyNickChanged(const QString &)
{
	bool bYes = !m_pNotifyCheck->isChecked();

	if(!bYes)
	{
		// we need at least one nickname then :)
		KviCString tmp = m_pNotifyNickEdit1->text();
		if(tmp.hasData())
		{
			bYes = true;
		}
		else
		{
			tmp = m_pNotifyNickEdit2->text();
			if(tmp.hasData())
				bYes = true;
		}
	}

	setNextEnabled(m_pPage4, bYes);
}

void RegistrationWizard::notifyCheckToggled(bool)
{
	bool bYes = m_pNotifyCheck->isChecked();

	m_pNotifyNickEdit1->setEnabled(bYes);
	m_pNotifyNickEdit2->setEnabled(bYes);
	m_pNotifyNickLabel1->setEnabled(bYes);
	m_pNotifyNickLabel2->setEnabled(bYes);

	QString dummy;
	notifyNickChanged(dummy);
}
