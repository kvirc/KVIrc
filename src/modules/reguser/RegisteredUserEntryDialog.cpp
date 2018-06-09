//=============================================================================
//
//   File : RegisteredUserEntryDialog.cpp
//   Creation date : Tue Dec 26 2000 12:24:12 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#define _WANT_OPTION_FLAGS_
#define _EDITUSER_CPP_

#include "RegisteredUserEntryDialog.h"
#include "RegistrationWizard.h"
#include "RegisteredUsersDialog.h"

#include "KviRegisteredUserDataBase.h"
#include "KviLocale.h"
#include "KviIrcMask.h"
#include "kvi_debug.h"
#include "KviIconManager.h"
#include "KviApplication.h"
#include "KviOptions.h"
#include "KviFile.h"
#include "KviMessageBox.h"
#include "KviFileUtils.h"
#include "kvi_settings.h"
#include "KviStringConversion.h"
#include "KviPointerHashTable.h"
#include "KviTalHBox.h"
#include "KviTalVBox.h"

#include <QLayout>
#include <QLabel>
#include <QGroupBox>
#include <QStyle>
#include <QPainter>
#include <QInputDialog>
#include <QImage>
#include <QString>
#include <QComboBox>
#include <QToolTip>
#include <QTableWidgetItem>
#include <QStringList>
#include <QHeaderView>
#include <QEvent>
#include <QCloseEvent>

// KviApplication.cpp
extern RegisteredUsersDialog * g_pRegisteredUsersDialog;

KviRegisteredUserDataBase * g_pLocalRegisteredUserDataBase; // local copy!

RegisteredUserPropertiesDialog::RegisteredUserPropertiesDialog(QWidget * p, KviPointerHashTable<QString, QString> * dict)
    : QDialog(p)
{
	setObjectName("property_editor");
	setWindowModality(Qt::WindowModal);
	m_pPropertyDict = dict;

	setWindowTitle(__tr2qs_ctx("Property Editor - KVIrc", "register"));
	setWindowIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Linux)));

	QGridLayout * g = new QGridLayout(this);

	m_pTable = new QTableWidget(this);
	//g->addMultiCellWidget(m_pTable,0,1,0,1);
	g->addWidget(m_pTable, 0, 0);
	m_pTable->setColumnCount(2);
	m_pTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pTable->setSelectionBehavior(QAbstractItemView::SelectRows);

	QStringList header;
	header.append(__tr2qs_ctx("Property", "register"));
	header.append(__tr2qs_ctx("Value", "register"));
	m_pTable->setHorizontalHeaderLabels(header);

	m_pTable->setMinimumSize(250, 250);
	//connect(m_pTable,SIGNAL(valueChanged(int,int)),this,SLOT(propertyValueChanged(int,int)));

	KviTalVBox * vb = new KviTalVBox(this);
	vb->setSpacing(4);
	g->addWidget(vb, 0, 1, 0, 3);

	m_pAddButton = new QPushButton(__tr2qs_ctx("&New", "register"), vb);
	connect(m_pAddButton, SIGNAL(clicked()), this, SLOT(addClicked()));
	m_pAddButton->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::NewItem)));

	m_pDelButton = new QPushButton(__tr2qs_ctx("&Remove", "register"), vb);
	connect(m_pDelButton, SIGNAL(clicked()), this, SLOT(delClicked()));
	m_pDelButton->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::DeleteItem)));

	KviTalHBox * b = new KviTalHBox(this);
	b->setSpacing(4);
	//g->addMultiCellWidget(b,2,2,1,2);
	g->addWidget(b, 1, 1);
	QPushButton * pb = new QPushButton(__tr2qs_ctx("&OK", "register"), b);
	connect(pb, SIGNAL(clicked()), this, SLOT(okClicked()));
	pb->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Accept)));

	pb = new QPushButton(__tr2qs_ctx("Cancel", "register"), b);
	connect(pb, SIGNAL(clicked()), this, SLOT(reject()));
	pb->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Discard)));

	g->setRowStretch(1, 1);
	g->setColumnStretch(0, 1);

	fillData();
}

RegisteredUserPropertiesDialog::~RegisteredUserPropertiesDialog()
    = default;

void RegisteredUserPropertiesDialog::closeEvent(QCloseEvent * e)
{
	e->accept();
	//delete this;
}

void RegisteredUserPropertiesDialog::fillData()
{
	m_pTable->setRowCount(m_pPropertyDict->count());
	KviPointerHashTableIterator<QString, QString> it(*m_pPropertyDict);
	int row = 0;
	while(it.current())
	{
		QTableWidgetItem * m_pTableItem1 = new QTableWidgetItem(it.currentKey(), QTableWidgetItem::Type);
		m_pTableItem1->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);

		QTableWidgetItem * m_pTableItem2 = new QTableWidgetItem(*(it.current()), QTableWidgetItem::Type);
		m_pTableItem2->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);

		m_pTable->setItem(row, 0, m_pTableItem1);
		m_pTable->setItem(row, 1, m_pTableItem2);
		++row;
		++it;
	}
	if(m_pTable->rowCount() == 0)
		m_pDelButton->setEnabled(false);
}

void RegisteredUserPropertiesDialog::okClicked()
{
	m_pPropertyDict->clear();

	int n = m_pTable->rowCount();
	for(int i = 0; i < n; i++)
	{
		QString szName = m_pTable->item(i, 0)->text();
		QString szValue = m_pTable->item(i, 1)->text();
		if((!szName.isEmpty()) && (!szValue.isEmpty()))
		{
			m_pPropertyDict->replace(szName, new QString(szValue));
		}
	}

	accept();
}

void RegisteredUserPropertiesDialog::addClicked()
{
	m_pTable->setRowCount(m_pTable->rowCount() + 1);

	QTableWidgetItem * m_pTableItem1 = new QTableWidgetItem("", QTableWidgetItem::Type);
	m_pTableItem1->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);

	QTableWidgetItem * m_pTableItem2 = new QTableWidgetItem("", QTableWidgetItem::Type);
	m_pTableItem2->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);

	m_pTable->setItem(m_pTable->rowCount() - 1, 0, m_pTableItem1);
	m_pTable->setItem(m_pTable->rowCount() - 1, 1, m_pTableItem2);
	m_pDelButton->setEnabled(true);
}

void RegisteredUserPropertiesDialog::delClicked()
{
	int i = m_pTable->currentRow();

	if((i > -1) && (i < m_pTable->rowCount()))
	{
		// remove row i
		m_pTable->removeRow(i);
		if(m_pTable->rowCount() == 0)
			m_pDelButton->setEnabled(false);
	}
}

RegisteredUserMaskDialog::RegisteredUserMaskDialog(QWidget * p, KviIrcMask * m)
    : QDialog(p)
{
	setObjectName("reguser_mask_editor");
	setWindowModality(Qt::WindowModal);
	m_pMask = m;

	setWindowTitle(__tr2qs_ctx("Mask Editor - KVIrc", "register"));

	QGridLayout * g = new QGridLayout(this);

	QLabel * l = new QLabel(__tr2qs_ctx("Insert a mask for this user.<br>It can contain the wildcard characters '*' and '?'.", "register"), this);
	//l->setAlignment(Qt::AlignCenter);
	g->addWidget(l, 0, 0, 1, 2);

	KviTalHBox * b = new KviTalHBox(this);
	g->addWidget(b, 1, 0, 1, 2);

	m_pNickEdit = new QLineEdit(b);
	//m_pNickEdit->setMinimumWidth(120);
	m_pNickEdit->setAlignment(Qt::AlignRight);
	m_pNickEdit->setToolTip(__tr2qs_ctx("This is the <b>nickname</b> that will match this user, default value is the registered name.", "register"));

	l = new QLabel("<b>!</b>", b);
	l->setAlignment(Qt::AlignCenter);
	//l->setMinimumWidth(40);

	m_pUserEdit = new QLineEdit(b);
	//m_pUserEdit->setMinimumWidth(120);
	m_pUserEdit->setAlignment(Qt::AlignCenter);
	m_pUserEdit->setToolTip(__tr2qs_ctx("This is the <b>username</b> that will match this user. <b>*</b> will match any username.", "register"));

	l = new QLabel("<b>@</b>", b);
	l->setAlignment(Qt::AlignCenter);
	//l->setMinimumWidth(40);

	m_pHostEdit = new QLineEdit(b);
	//m_pHostEdit->setMinimumWidth(120);
	m_pHostEdit->setAlignment(Qt::AlignLeft);
	m_pHostEdit->setToolTip(__tr2qs_ctx("This is the <b>hostname</b> that will match this user. <b>*</b> will match any hostname.", "register"));

	// just a spacer
	//	l = new QLabel("<nobr>&nbsp;<nobr>",this);
	//	g->addMultiCellWidget(l,2,2,0,1);

	//	QFrame * f = new QFrame(this);
	//	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	//	g->addMultiCellWidget(f,3,3,0,1);

	b = new KviTalHBox(this);
	b->setSpacing(4);
	g->addWidget(b, 2, 1);

	QPushButton * pb = new QPushButton(__tr2qs_ctx("&OK", "register"), b);
	connect(pb, SIGNAL(clicked()), this, SLOT(okClicked()));
	//pb->setMinimumWidth(120);

	pb = new QPushButton(__tr2qs_ctx("Cancel", "register"), b);
	connect(pb, SIGNAL(clicked()), this, SLOT(reject()));
	//pb->setMinimumWidth(120);

	g->setColumnStretch(0, 1);
	g->setRowStretch(0, 1);

	m_pNickEdit->setText(m->nick());
	m_pUserEdit->setText(m->user());
	m_pHostEdit->setText(m->host());
}

RegisteredUserMaskDialog::~RegisteredUserMaskDialog()
    = default;

void RegisteredUserMaskDialog::closeEvent(QCloseEvent * e)
{
	e->accept();
	//delete this;
}

void RegisteredUserMaskDialog::okClicked()
{
	KviCString szTmp = m_pNickEdit->text();
	if(szTmp.isEmpty())
		szTmp = "*";
	m_pMask->setNick(szTmp.ptr());

	szTmp = m_pUserEdit->text();
	if(szTmp.isEmpty())
		szTmp = "*";
	m_pMask->setUsername(szTmp.ptr());

	szTmp = m_pHostEdit->text();
	if(szTmp.isEmpty())
		szTmp = "*";
	m_pMask->setHost(szTmp.ptr());

	accept();
}

RegisteredUserEntryDialog::RegisteredUserEntryDialog(QWidget * p, KviRegisteredUser * r, bool bModal)
    : KviTalTabDialog(p, "reguser_entry_editor", bModal)
{
	m_pUser = r;
	m_pCustomColor = new QColor();

	if(r)
	{
		QString col = r->getProperty("customColor");
		KviStringConversion::fromString(col, (*m_pCustomColor));
	}

	m_pPropertyDict = new KviPointerHashTable<QString, QString>(17, false);
	m_pPropertyDict->setAutoDelete(true);

	//setMinimumSize(400,450);

	setWindowIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Linux)));
	setWindowTitle(__tr2qs_ctx("Registered User Entry - KVIrc", "register"));

	QWidget * p1 = new QWidget(this);

	QGridLayout * g = new QGridLayout(p1);

	QLabel * l = new QLabel(__tr2qs_ctx("Name:", "register"), p1);
	g->addWidget(l, 0, 0);

	m_pNameEdit = new QLineEdit(p1);
	g->addWidget(m_pNameEdit, 0, 1);

	l = new QLabel(__tr2qs_ctx("Comment:", "register"), p1);
	g->addWidget(l, 1, 0);

	m_pCommentEdit = new QLineEdit(p1);
	g->addWidget(m_pCommentEdit, 1, 1);

	QFrame * f = new QFrame(p1);
	g->addWidget(f, 2, 0, 1, 2);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);

	l = new QLabel(__tr2qs_ctx("Masks:", "register"), p1);
	g->addWidget(l, 3, 0, 1, 2);

	m_pMaskListBox = new QListWidget(p1);
	connect(m_pMaskListBox, SIGNAL(itemSelectionChanged()), this, SLOT(maskCurrentChanged()));
	m_pMaskListBox->setMinimumSize(300, 200);

	g->addWidget(m_pMaskListBox, 4, 0, 1, 2);

	KviTalHBox * b = new KviTalHBox(p1);
	g->addWidget(b, 5, 0, 1, 2);
	b->setSpacing(4);

	m_pAddMaskButton = new QPushButton(__tr2qs_ctx("&Add...", "register"), b);
	connect(m_pAddMaskButton, SIGNAL(clicked()), this, SLOT(addMaskClicked()));
	m_pAddMaskButton->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::NewItem)));

	m_pDelMaskButton = new QPushButton(__tr2qs_ctx("Re&move", "register"), b);
	m_pDelMaskButton->setEnabled(false);
	connect(m_pDelMaskButton, SIGNAL(clicked()), this, SLOT(delMaskClicked()));
	m_pDelMaskButton->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::DeleteItem)));

	m_pEditMaskButton = new QPushButton(__tr2qs_ctx("&Edit", "register"), b);
	m_pEditMaskButton->setEnabled(false);
	connect(m_pEditMaskButton, SIGNAL(clicked()), this, SLOT(editMaskClicked()));
	m_pEditMaskButton->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::EditItem)));

	g->setRowStretch(4, 1);
	g->setColumnStretch(1, 1);

	addTab(p1, __tr2qs_ctx("Identity", "register"));

	QWidget * p2 = new QWidget(this);

	g = new QGridLayout(p2);

	m_pNotifyCheck = new QCheckBox(__tr2qs_ctx("Notify when user is online", "register"), p2);
	g->addWidget(m_pNotifyCheck, 0, 0, 1, 3);

	m_pNotifyLabel = new QLabel(__tr2qs_ctx("Notify nicknames:", "register"), p2);
	m_pNotifyLabel->setEnabled(m_pNotifyCheck->isChecked());
	g->addWidget(m_pNotifyLabel, 1, 0);
	m_pNotifyCheck->setToolTip(__tr2qs_ctx("You can enter a space separated list of nicknames.", "register"));

	m_pNotifyNick = new QLineEdit(p2);
	m_pNotifyNick->setEnabled(false);

	g->addWidget(m_pNotifyNick, 1, 1, 1, 2);
	connect(m_pNotifyCheck, SIGNAL(toggled(bool)), this, SLOT(notifyCheckClicked(bool)));

	f = new QFrame(p2);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	g->addWidget(f, 2, 0, 1, 3);

	m_pAvatar = nullptr;
	if(r)
	{
		const QString av = r->getProperty("avatar");
		if(!av.isEmpty())
		{
			m_pAvatar = new KviPixmap(av.toUtf8().data());
		}
	}
	if(!m_pAvatar)
		m_pAvatar = new KviPixmap();

	m_pAvatarSelector = new KviPixmapSelector(p2, __tr2qs_ctx("Avatar:", "register"), m_pAvatar, true);
	g->addWidget(m_pAvatarSelector, 3, 0, 1, 3);

	f = new QFrame(p2);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	g->addWidget(f, 4, 0, 1, 3);

	m_pCustomColorCheck = new QCheckBox(__tr2qs_ctx("Use custom color in userlist", "register"), p2);
	if(r)
		m_pCustomColorCheck->setChecked(r->getBoolProperty("useCustomColor"));
	g->addWidget(m_pCustomColorCheck, 5, 0, 1, 2);

	m_pCustomColorSelector = new KviColorSelector(p2, QString(), m_pCustomColor, true);
	g->addWidget(m_pCustomColorSelector, 5, 2);

	QPushButton * pb = new QPushButton(__tr2qs_ctx("All Properties...", "register"), p2);
	connect(pb, SIGNAL(clicked()), this, SLOT(editAllPropertiesClicked()));
	g->addWidget(pb, 6, 2);

	g->setColumnStretch(1, 1);
	g->setRowStretch(3, 1);

	addTab(p2, __tr2qs_ctx("Properties", "register"));

	// Ignore TAB
	KviTalVBox * vb = new KviTalVBox(this);
	vb->setMargin(10);

	m_pIgnoreEnabled = new QCheckBox(__tr2qs_ctx("Enable ignore for this user", "register"), vb);

	QGroupBox * gb = new QGroupBox(__tr2qs_ctx("Ignore Features", "register"), vb);
	connect(m_pIgnoreEnabled, SIGNAL(toggled(bool)), gb, SLOT(setEnabled(bool)));

	QVBoxLayout * layout = new QVBoxLayout(gb);
	layout->setMargin(20);
	layout->setSpacing(3);

	m_pIgnoreQuery = new QCheckBox(__tr2qs_ctx("Ignore query messages", "register"), gb);
	layout->addWidget(m_pIgnoreQuery);

	m_pIgnoreChannel = new QCheckBox(__tr2qs_ctx("Ignore channel messages", "register"), gb);
	layout->addWidget(m_pIgnoreChannel);

	m_pIgnoreNotice = new QCheckBox(__tr2qs_ctx("Ignore notice messages", "register"), gb);
	layout->addWidget(m_pIgnoreNotice);

	m_pIgnoreCtcp = new QCheckBox(__tr2qs_ctx("Ignore CTCP messages", "register"), gb);
	layout->addWidget(m_pIgnoreCtcp);

	m_pIgnoreInvite = new QCheckBox(__tr2qs_ctx("Ignore invites", "register"), gb);
	layout->addWidget(m_pIgnoreInvite);

	m_pIgnoreDcc = new QCheckBox(__tr2qs_ctx("Ignore DCCs", "register"), gb);
	layout->addWidget(m_pIgnoreDcc);

	m_pIgnoreHighlight = new QCheckBox(__tr2qs_ctx("Ignore highlights", "register"), gb);
	layout->addWidget(m_pIgnoreHighlight);

	QWidget * w = new QWidget(vb);
	w->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

	addTab(vb, __tr2qs_ctx("Ignore", "register"));

	setCancelButton(__tr2qs_ctx("Cancel", "register"));
	setOkButton(__tr2qs_ctx("&OK", "register"));
	connect(this, SIGNAL(applyButtonPressed()), this, SLOT(okClicked()));
	connect(this, SIGNAL(cancelButtonPressed()), this, SLOT(reject()));

	if(r)
	{
		m_pNameEdit->setText(r->name());
		m_pCommentEdit->setText(r->getProperty("comment"));
		for(KviIrcMask * m = r->maskList()->first(); m; m = r->maskList()->next())
		{
			QString mk = m->nick();
			mk += QChar('!');
			mk += m->user();
			mk += QChar('@');
			mk += m->host();
			m_pMaskListBox->addItem(mk);
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
			KviPointerHashTableIterator<QString, QString> it(*(r->propertyDict()));
			while(QString * s = it.current())
			{
				m_pPropertyDict->insert(it.currentKey(), new QString(*s));
				++it;
			}
		}

		m_pIgnoreEnabled->setChecked(r->ignoreEnabled());

		gb->setEnabled(r->ignoreEnabled());

		m_pIgnoreQuery->setChecked(r->ignoreFlags() & KviRegisteredUser::Query);
		m_pIgnoreChannel->setChecked(r->ignoreFlags() & KviRegisteredUser::Channel);
		m_pIgnoreNotice->setChecked(r->ignoreFlags() & KviRegisteredUser::Notice);
		m_pIgnoreCtcp->setChecked(r->ignoreFlags() & KviRegisteredUser::Ctcp);
		m_pIgnoreInvite->setChecked(r->ignoreFlags() & KviRegisteredUser::Invite);
		m_pIgnoreDcc->setChecked(r->ignoreFlags() & KviRegisteredUser::Dcc);
		m_pIgnoreHighlight->setChecked(r->ignoreFlags() & KviRegisteredUser::Highlight);
	}
	else
	{
		// default values
		if(!m_pIgnoreEnabled->isChecked())
		{
			gb->setEnabled(false);
		}
	}
}

void RegisteredUserEntryDialog::closeEvent(QCloseEvent * e)
{
	e->accept();
	okClicked();
	//delete this;
}

RegisteredUserEntryDialog::~RegisteredUserEntryDialog()
{
	delete m_pAvatar;
	delete m_pPropertyDict;
	delete m_pCustomColor;
}

void RegisteredUserEntryDialog::notifyCheckClicked(bool bChecked)
{
	m_pNotifyNick->setEnabled(bChecked);
	m_pNotifyLabel->setEnabled(bChecked);
	if(bChecked && m_pNotifyNick->text().isEmpty())
	{
		QString szMask;
		if(m_pUser)
		{
			for(KviIrcMask * m = m_pUser->maskList()->first(); m; m = m_pUser->maskList()->next())
			{
				QString tmp = m->nick();
				if((tmp.indexOf('*') == -1) && (tmp.indexOf('?') == -1) && (!tmp.isEmpty()))
				{
					if(!szMask.isEmpty())
						szMask.append(' ');
					szMask.append(tmp);
				}
			}
			// if the nickname list is still empty, build a dummy nick to notify
			if(szMask.isEmpty())
			{
				szMask = m_pUser->name();
				szMask.replace(" ", "");
				szMask.replace("'", "");
				szMask.replace("&", "");
				szMask.replace(",", "");
			}
			m_pNotifyNick->setText(szMask);
		}
	}
}

void RegisteredUserEntryDialog::maskCurrentChanged()
{
	bool bHaveSelected = !m_pMaskListBox->selectedItems().empty();

	m_pDelMaskButton->setEnabled(bHaveSelected);
	m_pEditMaskButton->setEnabled(bHaveSelected);
}

void RegisteredUserEntryDialog::okClicked()
{
	QString szGroup;
	if(m_pUser)
	{
		szGroup = m_pUser->group();
		g_pLocalRegisteredUserDataBase->removeUser(m_pUser->name());
	}

	QString name = m_pNameEdit->text();

	if(name.isEmpty())
		name = "user";

	KviRegisteredUser * u;

	QString szNameOk = name;

	int idx = 1;

	do
	{
		u = g_pLocalRegisteredUserDataBase->findUserByName(szNameOk);
		if(u)
		{
			szNameOk = QString("%1%2").arg(name).arg(idx);
			idx++;
		}
	} while(u);

	u = g_pLocalRegisteredUserDataBase->addUser(szNameOk);
	u->setGroup(szGroup);

	if(!u)
	{
		// ops... no way
		// FIXME: spit an error message ?
		qDebug("Oops! Something wrong with the regusers DB");
		accept();
		return;
	}

	int cnt = m_pMaskListBox->count();
	idx = 0;
	while(cnt > 0)
	{
		QString mask = m_pMaskListBox->item(idx)->text();
		KviIrcMask * mk = new KviIrcMask(mask);
		g_pLocalRegisteredUserDataBase->removeMask(*mk);
		g_pLocalRegisteredUserDataBase->addMask(u, mk);
		cnt--;
		idx++;
	}
	u->setProperty("comment", m_pCommentEdit->text());

	m_pAvatarSelector->commit();

	if(!m_pAvatar->isNull())
	{
		QString szPath = m_pAvatar->path();
		u->setProperty("avatar", szPath);
	}

	if(m_pNotifyCheck->isChecked())
	{
		QString szNicks = m_pNotifyNick->text();

		if(szNicks.isEmpty())
		{
			u->setProperty("notify", QString(""));
		}
		else
		{
			u->setProperty("notify", szNicks);
		}
	}
	else
	{
		u->setProperty("notify", QString(""));
	}

	m_pPropertyDict->remove("comment");
	m_pPropertyDict->remove("notify");
	m_pPropertyDict->remove("avatar");

	KviPointerHashTableIterator<QString, QString> it(*m_pPropertyDict);
	while(QString * s = it.current())
	{
		u->setProperty(it.currentKey(), *s);
		++it;
	}

	u->setProperty("useCustomColor", m_pCustomColorCheck->isChecked());

	QString col;
	KviStringConversion::toString(m_pCustomColorSelector->getColor(), col);
	u->setProperty("customColor", col);

	int iIgnoreFlags = 0;
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
	if(m_pIgnoreHighlight->isChecked())
		iIgnoreFlags |= KviRegisteredUser::Highlight;

	u->setIgnoreFlags(iIgnoreFlags);
	accept();
	g_pApp->optionResetUpdate(KviOption_resetUpdateGui);
}

void RegisteredUserEntryDialog::addMaskClicked()
{
	KviIrcMask mk;
	RegisteredUserMaskDialog * dlg = new RegisteredUserMaskDialog(this, &mk);
	if(dlg->exec() == QDialog::Accepted)
	{
		QString m = mk.nick();
		m += QChar('!');
		m += mk.user();
		m += QChar('@');
		m += mk.host();
		m_pMaskListBox->addItem(m);
	}
	delete dlg;
}

void RegisteredUserEntryDialog::delMaskClicked()
{
	if(m_pMaskListBox->currentItem())
	{
		delete m_pMaskListBox->currentItem();
	}
}

void RegisteredUserEntryDialog::editMaskClicked()
{
	if(!m_pMaskListBox->currentItem())
		return;
	QString szM = m_pMaskListBox->currentItem()->text();
	if(szM.isEmpty())
		return;

	KviIrcMask mk(szM);
	RegisteredUserMaskDialog * dlg = new RegisteredUserMaskDialog(this, &mk);
	if(dlg->exec() == QDialog::Accepted)
	{
		QString m = mk.nick();
		m += QChar('!');
		m += mk.user();
		m += QChar('@');
		m += mk.host();
		m_pMaskListBox->currentItem()->setText(m);
	}
	delete dlg;
}

void RegisteredUserEntryDialog::editAllPropertiesClicked()
{
	m_pAvatarSelector->commit();

	if(m_pAvatar->isNull())
	{
		m_pPropertyDict->remove("avatar");
	}
	else
	{
		KviCString szPath = m_pAvatar->path();
		if(szPath.isEmpty())
			m_pPropertyDict->remove("avatar");
		else
			m_pPropertyDict->replace("avatar", new QString(szPath));
	}

	if(m_pNotifyCheck->isChecked())
	{
		QString szNicks = m_pNotifyNick->text();

		if(!szNicks.isEmpty())
		{
			m_pPropertyDict->replace("notify", new QString(szNicks));
		}
		else
		{
			m_pPropertyDict->remove("notify");
		}
	}
	else
	{
		m_pPropertyDict->remove("notify");
	}

	RegisteredUserPropertiesDialog * dlg = new RegisteredUserPropertiesDialog(this, m_pPropertyDict);
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
	if(!bGotIt)
		m_pNotifyNick->setText("");

	QString * avatar = m_pPropertyDict->find("avatar");
	bGotIt = false;
	if(avatar)
	{
		if(!avatar->isEmpty())
			m_pAvatarSelector->setImagePath(*avatar);
	}
}
