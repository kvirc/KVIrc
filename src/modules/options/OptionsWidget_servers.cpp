//=============================================================================
//
//   File : OptionsWidget_servers.cpp
//   Creation date : Wed Jun 12 2000 03:29:51 by Szymon Stefanek
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

#include "OptionsWidget_servers.h"
#include "OptionsWidget_proxy.h"
#include "OptionsWidget_nickserv.h" // for the NickServ rule editor

#include "OptionsWidgetContainer.h"

#include "KviQueryWindow.h"
#include "KviChannelWindow.h"
#include "KviLocale.h"
#include "KviIconManager.h"
#include "KviIrcServerDataBase.h"
#include "KviIpEditor.h"
#include "KviNetUtils.h"
#include "kvi_settings.h"
#include "KviFileDialog.h"
#include "KviConfigurationFile.h"
#include "KviScriptEditor.h"
#include "KviOptions.h"
#include "KviApplication.h"
#include "KviConsoleWindow.h"
#include "KviIrcUrl.h"
#include "KviModule.h"
#include "KviModuleManager.h"
#include "KviModuleExtension.h"
#include "KviMessageBox.h"
#include "KviMexServerImport.h"
#include "KviNickServRuleSet.h"
#include "KviProxy.h"
#include "KviProxyDataBase.h"
#include "KviKvsScript.h"
#include "KviPointerHashTable.h"
#include "KviTalToolTip.h"
#include "KviIrcNetwork.h"
#include "KviSASL.h"

#include <QLineEdit>
#include <QCursor>
#include <QValidator>
#include <QMessageBox>
#include <QComboBox>
#include <QButtonGroup>
#include <QInputDialog>
#include <QHeaderView>
#include <QIcon>
#include <QShortcut>
#include <QMenu>

#include <memory>
#include <vector>

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW) || defined(COMPILE_ON_MAC)
#include <QStyleFactory>
#endif //COMPILE_ON_WINDOWS || COMPILE_ON_MINGW

IrcNetworkDetailsWidget::IrcNetworkDetailsWidget(QWidget * par, KviIrcNetwork * n)
    : QDialog(par)
{
	setModal(true);
	setObjectName("network_details");

	m_pOnConnectEditor = nullptr;
	m_pOnLoginEditor = nullptr;

	m_pUserEditor = nullptr;
	m_pPassEditor = nullptr;
	m_pNickEditor = nullptr;
	m_pAlternativeNickEditor = nullptr;
	m_pRealEditor = nullptr;
	m_pDescEditor = nullptr;

	m_pEncodingEditor = nullptr;
	m_pTextEncodingEditor = nullptr;

	m_pAutoConnectCheck = nullptr;

	m_pNickServTreeWidget = nullptr;
	m_pNickServCheck = nullptr;
	m_pAddRuleButton = nullptr;
	m_pDelRuleButton = nullptr;
	m_pEditRuleButton = nullptr;

	m_pChannelListSelector = nullptr;

	QGridLayout * g = new QGridLayout(this);

	setWindowTitle(__tr2qs_ctx("Network Details and Configuration - KVIrc", "options"));
	setWindowIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::World))));

	QString szTmp = "<font size=\"+1\"><b>";
	szTmp += n->name();
	szTmp += "</b></font>";

	QLabel * l = new QLabel(szTmp, this);
	l->setFrameStyle(QFrame::Raised | QFrame::StyledPanel);
	l->setAlignment(Qt::AlignCenter);
	g->addWidget(l, 0, 0, 1, 4);

	l->setMinimumWidth(200);

	l = new QLabel(__tr2qs_ctx("Description:", "options"), this);
	g->addWidget(l, 1, 0);
	m_pDescEditor = new QLineEdit(this);
	g->addWidget(m_pDescEditor, 1, 1, 1, 3);

	m_pDescEditor->setText(n->description());
	KviTalToolTip::add(m_pDescEditor, __tr2qs_ctx("Put here a brief description of the network.", "options"));

	QTabWidget * tw = new QTabWidget(this);
	g->addWidget(tw, 2, 0, 1, 4);

	QWidget * tab = new QWidget(tw);
	QGridLayout * gl = new QGridLayout(tab);

	KviTalGroupBox * gbox = new KviTalGroupBox(Qt::Horizontal, __tr2qs_ctx("Properties", "options"), tab);
	gl->addWidget(gbox, 0, 0, 1, 2);

	QGridLayout * pPropertiesBoxLayout = new QGridLayout();
	gbox->setLayout(pPropertiesBoxLayout);

	l = new QLabel(__tr2qs_ctx("Username:", "options"), gbox);
	pPropertiesBoxLayout->addWidget(l, 0, 0);

	m_pUserEditor = new QLineEdit(gbox);
	m_pUserEditor->setText(n->userName());
	pPropertiesBoxLayout->addWidget(m_pUserEditor, 0, 1);

	KviTalToolTip::add(m_pUserEditor, __tr2qs_ctx("This is the <b>username</b> that KVIrc will use to login to servers on this network.\n"
	                                              "If this field is left empty (most common case), KVIrc will use the default username "
	                                              "specified in the \"Identity\" options tab.", "options"));

	l = new QLabel(__tr2qs_ctx("Password:", "options"), gbox);
	pPropertiesBoxLayout->addWidget(l, 1, 0);

	m_pPassEditor = new KviPasswordLineEdit(gbox);
	m_pPassEditor->setText(n->password());
	pPropertiesBoxLayout->addWidget(m_pPassEditor, 1, 1);

	KviTalToolTip::add(m_pPassEditor, __tr2qs_ctx("If this network requires a password, put it in this field, otherwise leave it empty.", "options"));

	l = new QLabel(__tr2qs_ctx("Nickname:", "options"), gbox);
	pPropertiesBoxLayout->addWidget(l, 2, 0);

	m_pNickEditor = new QLineEdit(gbox);
	QValidator * v = new QRegExpValidator(QRegExp("[^-0-9 ][^ ]*", Qt::CaseSensitive), gbox);
	m_pNickEditor->setValidator(v);
	m_pNickEditor->setText(n->nickName());
	pPropertiesBoxLayout->addWidget(m_pNickEditor, 2, 1);

	KviTalToolTip::add(m_pNickEditor, __tr2qs_ctx("You can specify a special nickname that will be used to login to the servers on this network.<br>"
	                                              "If this field is left empty (most common case), the default nickname (specified in the \"Identity\" settings) will be used.", "options"));

	l = new QLabel(__tr2qs_ctx("Alt. nickname:", "options"), gbox);
	pPropertiesBoxLayout->addWidget(l, 3, 0);

	m_pAlternativeNickEditor = new QLineEdit(gbox);
	v = new QRegExpValidator(QRegExp("[^-0-9 ][^ ]*", Qt::CaseSensitive), gbox);
	m_pAlternativeNickEditor->setValidator(v);
	m_pAlternativeNickEditor->setText(n->alternativeNickName());
	pPropertiesBoxLayout->addWidget(m_pAlternativeNickEditor, 3, 1);

	KviTalToolTip::add(m_pAlternativeNickEditor, __tr2qs_ctx("This nickname will be tried if the primary nickname for this network is already in use.<br>"
	                                                         "If you leave it empty then the default nicknames specified in the identity options will be tried instead.", "options"));

	l = new QLabel(__tr2qs_ctx("Real name:", "options"), gbox);
	pPropertiesBoxLayout->addWidget(l, 4, 0);

	m_pRealEditor = new QLineEdit(gbox);
	m_pRealEditor->setText(n->realName());
	pPropertiesBoxLayout->addWidget(m_pRealEditor, 4, 1);

	KviTalToolTip::add(m_pRealEditor, __tr2qs_ctx("You can specify a special <b>real name</b> that will be used to login with the servers on this network."
	                                              "If you leave this field empty (most common case), the default \"real name\" (specified in the \"Identity\" settings) will be used.", "options"));

	l = new QLabel(__tr2qs_ctx("Server encoding:", "options"), tab);
	gl->addWidget(l, 1, 0);
	m_pEncodingEditor = new QComboBox(tab);
	m_pEncodingEditor->setDuplicatesEnabled(false);
	gl->addWidget(m_pEncodingEditor, 1, 1);
	KviTalToolTip::add(m_pEncodingEditor, __tr2qs_ctx("This box allows you to choose the preferred encoding for the servers in this network. "
	                                                  "This encoding will be used for server specific needs, like referencing nicknames and channel names.<br>"
	                                                  "If you choose \"Use System Encoding\" then the encoding will be set to the system-wide "
	                                                  "value that you choose in the \"Encoding\" page of the options dialog.", "options"));

	l = new QLabel(__tr2qs_ctx("Text encoding:", "options"), tab);
	gl->addWidget(l, 2, 0);
	m_pTextEncodingEditor = new QComboBox(tab);
	m_pTextEncodingEditor->setDuplicatesEnabled(false);
	gl->addWidget(m_pTextEncodingEditor, 2, 1);
	KviTalToolTip::add(m_pTextEncodingEditor, __tr2qs_ctx("This box allows you to choose the preferred encoding for the servers in this network. "
	                                                      "This encoding will be used as the default for text messages.<br>"
	                                                      "If you choose \"Use System Encoding\" then the encoding will be set to the system-wide "
	                                                      "value that you choose in the \"Encoding\" page of the options dialog.", "options"));
	int i = 0;
	int srvcurrent = 0, txtcurrent = 0;
	KviLocale::EncodingDescription * d = KviLocale::instance()->encodingDescription(i);
	QString tmp;
	m_pEncodingEditor->addItem(__tr2qs_ctx("Use System Encoding", "options"));
	m_pTextEncodingEditor->addItem(__tr2qs_ctx("Use System Encoding", "options"));

	while(d->pcName)
	{
		tmp = QString("%1 (%2)").arg(d->pcName, d->pcDescription);
		m_pEncodingEditor->insertItem(m_pEncodingEditor->count(), tmp);
		m_pTextEncodingEditor->insertItem(m_pTextEncodingEditor->count(), tmp);
		if(KviQString::equalCI(d->pcName, n->encoding()))
			srvcurrent = i + 1;
		if(KviQString::equalCI(d->pcName, n->textEncoding()))
			txtcurrent = i + 1;
		i = i + 1;
		d = KviLocale::instance()->encodingDescription(i);
	}

	m_pEncodingEditor->setCurrentIndex(srvcurrent);
	m_pTextEncodingEditor->setCurrentIndex(txtcurrent);

	m_pAutoConnectCheck = new QCheckBox(__tr2qs_ctx("Connect to this network at startup", "options"), tab);
	m_pAutoConnectCheck->setChecked(n->autoConnect());
	gl->addWidget(m_pAutoConnectCheck, 3, 0, 1, 2);

	KviTalToolTip::add(m_pAutoConnectCheck, __tr2qs_ctx("This option will cause KVIrc to automatically connect to this network at startup", "options"));

	l = new QLabel("", tab);
	gl->addWidget(l, 4, 0);

	gl->setRowStretch(4, 1);
	gl->setColumnStretch(1, 1);

	tw->addTab(tab, __tr2qs_ctx("General", "options"));

	tab = new QWidget(tw);

	gl = new QGridLayout(tab);

	if(n->autoJoinChannelList())
		m_lstChannels = *(n->autoJoinChannelList());
	m_pChannelListSelector = new KviChannelListSelector(tab, __tr2qs_ctx("Channels to Join Automatically upon Connect", "options"), &m_lstChannels, true);

	KviTalToolTip::add(m_pChannelListSelector, __tr2qs_ctx("Here you can set a list of channels to be joined automatically "
	                                                       "after a connection to a server in this network has been established. To add a channel, type its name in the "
	                                                       "text input below and click \"<b>Add</b>\".", "options"));

	gl->addWidget(m_pChannelListSelector, 0, 0);

	tw->addTab(tab, __tr2qs_ctx("Join Channels", "options"));

	// after connect execute
	tab = new QWidget(tw);
	gl = new QGridLayout(tab);

	m_pOnConnectEditor = KviScriptEditor::createInstance(tab);
	gl->addWidget(m_pOnConnectEditor, 0, 0);
	m_pOnConnectEditor->setText(n->onConnectCommand());
	m_pOnConnectEditor->setMinimumHeight(150);
	KviTalToolTip::add(m_pOnConnectEditor, __tr2qs_ctx("The following commands will be executed after a connection to a server in this network has been established.<br>"
	                                                   "<b>Important:</b> Enter commands <b>without</b> a preceding slash (e.g. <tt>quote pass secret</tt> instead of <tt>/quote pass secret</tt>).<br>"
	                                                   "KVIrc will first send the USER command, then eventually PASS and NICK and then execute this "
	                                                   "command sequence.", "options"));

	tw->addTab(tab, __tr2qs_ctx("On Connect", "options"));

	tab = new QWidget(tw);
	gl = new QGridLayout(tab);

	m_pOnLoginEditor = KviScriptEditor::createInstance(tab);
	gl->addWidget(m_pOnLoginEditor, 0, 0);
	m_pOnLoginEditor->setText(n->onLoginCommand());

	m_pOnLoginEditor->setMinimumHeight(150);

	KviTalToolTip::add(m_pOnLoginEditor, __tr2qs_ctx("The following commands will be executed after a successful login to a server in this network.<br>"
	                                                 "<b>Important:</b> Enter commands <b>without</b> a preceding slash (e.g. <tt>quote privatelog</tt> instead of <tt>/quote privatelog</tt>).<br>"
	                                                 "This is useful for automatically opening queries, setting variables, etc.", "options"));

	tw->addTab(tab, __tr2qs_ctx("On Login", "options"));

	tab = new QWidget(tw);
	gl = new QGridLayout(tab);

	KviNickServRuleSet * rs = n->nickServRuleSet();
	bool bNickServEnabled = rs ? (rs->isEnabled() && !rs->isEmpty()) : false;

	m_pNickServCheck = new QCheckBox(__tr2qs_ctx("Enable NickServ identification", "options"), tab);
	gl->addWidget(m_pNickServCheck, 0, 0, 1, 3);

	KviTalToolTip::add(m_pNickServCheck, __tr2qs_ctx("This check enables the automatic identification with NickServ", "options"));
	m_pNickServCheck->setChecked(bNickServEnabled);

	m_pNickServTreeWidget = new QTreeWidget(tab);
	m_pNickServTreeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pNickServTreeWidget->setAllColumnsShowFocus(true);
	m_pNickServTreeWidget->setColumnCount(4);
	QStringList columnLabels;
	columnLabels.append(__tr2qs_ctx("Nickname", "options"));
	columnLabels.append(__tr2qs_ctx("NickServ Mask", "options"));
	columnLabels.append(__tr2qs_ctx("NickServ Request Mask", "options"));
	columnLabels.append(__tr2qs_ctx("Identify Command", "options"));
	m_pNickServTreeWidget->setHeaderLabels(columnLabels);
	m_pNickServTreeWidget->setColumnWidth(0, 130);
	m_pNickServTreeWidget->setColumnWidth(1, 150);
	m_pNickServTreeWidget->setColumnWidth(2, 190);
	m_pNickServTreeWidget->setColumnWidth(3, 130);
	connect(m_pNickServTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(enableDisableNickServControls()));
	gl->addWidget(m_pNickServTreeWidget, 1, 0, 1, 3);

	KviTalToolTip::add(m_pNickServTreeWidget, __tr2qs_ctx("This is a list of NickServ identification rules. "
	                                                      "KVIrc will use them to model its automatic interaction with NickServ on this network.<br>"
	                                                      "Please be aware that this feature can cause your NickServ passwords to be stolen "
	                                                      "if used improperly. Make sure that you fully understand the NickServ authentication protocol.<br>"
	                                                      "In other words, be sure to know what you're doing.<br>"
	                                                      "Also note that the password that you provide is stored as <b>PLAIN TEXT</b>.", "options"));

	m_pAddRuleButton = new QPushButton(__tr2qs_ctx("Add Rule", "options"), tab);
	connect(m_pAddRuleButton, SIGNAL(clicked()), this, SLOT(addNickServRule()));
	gl->addWidget(m_pAddRuleButton, 2, 0);

	m_pEditRuleButton = new QPushButton(__tr2qs_ctx("Edit Rule", "options"), tab);
	connect(m_pEditRuleButton, SIGNAL(clicked()), this, SLOT(editNickServRule()));
	gl->addWidget(m_pEditRuleButton, 2, 1);

	m_pDelRuleButton = new QPushButton(__tr2qs_ctx("Delete Rule", "options"), tab);
	connect(m_pDelRuleButton, SIGNAL(clicked()), this, SLOT(delNickServRule()));
	gl->addWidget(m_pDelRuleButton, 2, 2);

	connect(m_pNickServCheck, SIGNAL(toggled(bool)), this, SLOT(enableDisableNickServControls()));

	if(rs && rs->rules())
	{
		KviPointerList<KviNickServRule> * ll = rs->rules();
		for(KviNickServRule * rule = ll->first(); rule; rule = ll->next())
		{
			QTreeWidgetItem * it = new QTreeWidgetItem(m_pNickServTreeWidget);
			it->setText(0, rule->registeredNick());
			it->setText(1, rule->nickServMask());
			it->setText(2, rule->messageRegexp());
			it->setText(3, rule->identifyCommand());
		}
	}

	enableDisableNickServControls();

	gl->setRowStretch(1, 1);

	tw->addTab(tab, __tr2qs_ctx("NickServ", "options"));

	QPushButton * b = new QPushButton(__tr2qs_ctx("&OK", "options"), this);
	b->setMinimumWidth(80);
	g->addWidget(b, 3, 2);
	b->setDefault(true);
	connect(b, SIGNAL(clicked()), this, SLOT(accept()));

	b = new QPushButton(__tr2qs_ctx("Cancel", "options"), this);
	g->addWidget(b, 3, 3);
	b->setMinimumWidth(80);
	connect(b, SIGNAL(clicked()), this, SLOT(reject()));

	g->setRowStretch(2, 1);
	g->setColumnStretch(1, 1);

	tw->setMinimumWidth(375);
}

IrcNetworkDetailsWidget::~IrcNetworkDetailsWidget()
{
	if(m_pOnConnectEditor)
		KviScriptEditor::destroyInstance(m_pOnConnectEditor);
	if(m_pOnLoginEditor)
		KviScriptEditor::destroyInstance(m_pOnLoginEditor);
}

void IrcNetworkDetailsWidget::editNickServRule()
{
	QTreeWidgetItem * it = (QTreeWidgetItem *)m_pNickServTreeWidget->currentItem();
	if(!it)
		return;
	KviNickServRule r(it->text(0), it->text(1), it->text(2), it->text(3));
	NickServRuleEditor ed(this, false);
	if(ed.editRule(&r))
	{
		it->setText(0, r.registeredNick());
		it->setText(1, r.nickServMask());
		it->setText(2, r.messageRegexp());
		it->setText(3, r.identifyCommand());
	}
}

void IrcNetworkDetailsWidget::addNickServRule()
{
	KviNickServRule r;
	NickServRuleEditor ed(this, false);
	if(ed.editRule(&r))
	{
		QTreeWidgetItem * it = new QTreeWidgetItem(m_pNickServTreeWidget);
		it->setText(0, r.registeredNick());
		it->setText(1, r.nickServMask());
		it->setText(2, r.messageRegexp());
		it->setText(3, r.identifyCommand());
	}
}

void IrcNetworkDetailsWidget::delNickServRule()
{
	QTreeWidgetItem * it = (QTreeWidgetItem *)m_pNickServTreeWidget->currentItem();
	if(!it)
		return;
	delete it;
	enableDisableNickServControls();
}

void IrcNetworkDetailsWidget::enableDisableNickServControls()
{
	bool bEnabled = m_pNickServCheck->isChecked();
	m_pNickServTreeWidget->setEnabled(bEnabled);
	m_pAddRuleButton->setEnabled(bEnabled);
	bEnabled = bEnabled && (m_pNickServTreeWidget->topLevelItemCount()) && m_pNickServTreeWidget->currentItem();
	m_pDelRuleButton->setEnabled(bEnabled);
	m_pEditRuleButton->setEnabled(bEnabled);
}

void IrcNetworkDetailsWidget::fillData(KviIrcNetwork * n)
{
	n->setUserName(m_pUserEditor->text());
	n->setPassword(m_pPassEditor->text());
	n->setNickName(m_pNickEditor->text());
	n->setAlternativeNickName(m_pAlternativeNickEditor->text());
	n->setRealName(m_pRealEditor->text());
	n->setDescription(m_pDescEditor->text());
	if(m_pAutoConnectCheck)
		n->setAutoConnect(m_pAutoConnectCheck->isChecked());

	if(m_pEncodingEditor)
	{
		if(m_pEncodingEditor->currentIndex() <= 0)
		{
			n->setEncoding(QString());
		}
		else
		{
			KviLocale::EncodingDescription * d = KviLocale::instance()->encodingDescription(m_pEncodingEditor->currentIndex() - 1);
			n->setEncoding(d->pcName);
		}
	}

	if(m_pTextEncodingEditor)
	{
		if(m_pTextEncodingEditor->currentIndex() <= 0)
		{
			n->setTextEncoding(QString());
		}
		else
		{
			KviLocale::EncodingDescription * dd = KviLocale::instance()->encodingDescription(m_pTextEncodingEditor->currentIndex() - 1);
			n->setTextEncoding(dd->pcName);
		}
	}

	if(m_pChannelListSelector)
		m_pChannelListSelector->commit();

	if(m_lstChannels.isEmpty())
		n->setAutoJoinChannelList(nullptr);
	else
		n->setAutoJoinChannelList(new QStringList(m_lstChannels));

	if(m_pNickServTreeWidget)
	{
		if(m_pNickServTreeWidget->topLevelItemCount() > 0)
		{
			KviNickServRuleSet * rs = KviNickServRuleSet::createInstance();
			rs->setEnabled(m_pNickServCheck->isChecked());
			QTreeWidgetItem * it;
			for(int i = 0; i < m_pNickServTreeWidget->topLevelItemCount(); i++)
			{
				it = (QTreeWidgetItem *)m_pNickServTreeWidget->topLevelItem(i);
				rs->addRule(KviNickServRule::createInstance(it->text(0), it->text(1), it->text(2), it->text(3)));
			}
			n->setNickServRuleSet(rs);
		}
		else
			n->setNickServRuleSet(nullptr);
	}

	if(m_pOnConnectEditor)
	{
		QString tmp;
		m_pOnConnectEditor->getText(tmp);
		n->setOnConnectCommand(tmp);
	}

	if(m_pOnLoginEditor)
	{
		QString tmp;
		m_pOnLoginEditor->getText(tmp);
		n->setOnLoginCommand(tmp);
	}
}

IrcServerDetailsWidget::IrcServerDetailsWidget(QWidget * par, KviIrcServer * s)
    : QDialog(par)
{
	setModal(true);
	setObjectName("server_details");

	m_szHostname = s->hostName();
	QGridLayout * g = new QGridLayout(this);

	setWindowTitle(__tr2qs_ctx("Server Details and Configuration - KVIrc", "options"));
	setWindowIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Server))));

	m_pHeaderLabel = new QLabel("", this); // the text will be set later
	m_pHeaderLabel->setFrameStyle(QFrame::Raised | QFrame::StyledPanel);
	m_pHeaderLabel->setAlignment(Qt::AlignCenter);
	g->addWidget(m_pHeaderLabel, 0, 0, 1, 4);
	m_pHeaderLabel->setMinimumWidth(200);

	QLabel * l = new QLabel(__tr2qs_ctx("Description:", "options"), this);
	g->addWidget(l, 1, 0);
	m_pDescEditor = new QLineEdit(this);
	g->addWidget(m_pDescEditor, 1, 1, 1, 3);

	m_pDescEditor->setText(s->description());
	KviTalToolTip::add(m_pDescEditor, __tr2qs_ctx("This is a brief description of this server. "
	                                              "This field has no restrictions but many server lists use it to describe the server's physical location", "options"));

	QTabWidget * tw = new QTabWidget(this);
	g->addWidget(tw, 2, 0, 1, 4);

	QWidget * tab = new QWidget(tw);
	QGridLayout * gl = new QGridLayout(tab);

	KviTalGroupBox * gbox = new KviTalGroupBox(Qt::Horizontal, __tr2qs_ctx("Properties", "options"), tab);
	gl->addWidget(gbox, 0, 0);

	QGridLayout * pPropertiesBoxLayout = new QGridLayout();
	gbox->setLayout(pPropertiesBoxLayout);

	l = new QLabel(__tr2qs_ctx("Username:", "options"), gbox);
	pPropertiesBoxLayout->addWidget(l, 0, 0);

	m_pUserEditor = new QLineEdit(gbox);
	m_pUserEditor->setText(s->userName());
	pPropertiesBoxLayout->addWidget(m_pUserEditor, 0, 1);

	KviTalToolTip::add(m_pUserEditor, __tr2qs_ctx("This is the <b>username</b> that KVIrc will use to login to this server.\n"
	                                              "If this field is left empty (most common case), KVIrc will first look if a username is specified "
	                                              "for the network that this server belongs to, and if that is empty then KVIrc will use the default username "
	                                              "specified in the \"Identity\" options tab.", "options"));

	l = new QLabel(__tr2qs_ctx("Password:", "options"), gbox);
	pPropertiesBoxLayout->addWidget(l, 1, 0);

	m_pPassEditor = new KviPasswordLineEdit(gbox);
	m_pPassEditor->setText(s->password());
	pPropertiesBoxLayout->addWidget(m_pPassEditor, 1, 1);

	KviTalToolTip::add(m_pPassEditor, __tr2qs_ctx("If this server requires a password, put it in this field, otherwise leave it empty.", "options"));

	l = new QLabel(__tr2qs_ctx("Nickname:", "options"), gbox);
	pPropertiesBoxLayout->addWidget(l, 2, 0);

	m_pNickEditor = new QLineEdit(gbox);
	QValidator * v = new QRegExpValidator(QRegExp("[^-0-9 ][^ ]*"), gbox);
	m_pNickEditor->setValidator(v);
	m_pNickEditor->setText(s->nickName());
	pPropertiesBoxLayout->addWidget(m_pNickEditor, 2, 1);

	KviTalToolTip::add(m_pNickEditor, __tr2qs_ctx("You can specify a special <b>nickname</b> that will be used to login to this server.<br>"
	                                              "If this field is left empty (most common case), KVIrc will first look if a nickname is specified "
	                                              "for the network that this server belongs to, and if that is empty then the default nickname "
						      "(specified in the \"Identity\" settings) will be used.", "options"));

	l = new QLabel(__tr2qs_ctx("Alt. nickname:", "options"), gbox);
	pPropertiesBoxLayout->addWidget(l, 3, 0);

	m_pAlternativeNickEditor = new QLineEdit(gbox);
	v = new QRegExpValidator(QRegExp("[^-0-9 ][^ ]*"), gbox);
	m_pAlternativeNickEditor->setValidator(v);
	m_pAlternativeNickEditor->setText(s->alternativeNickName());
	pPropertiesBoxLayout->addWidget(m_pAlternativeNickEditor, 3, 1);

	KviTalToolTip::add(m_pAlternativeNickEditor, __tr2qs_ctx("This nickname will be tried if the primary nickname for this server is already in use.<br>"
	                                                         "If you leave it empty then the default nicknames specified in the identity options will be tried instead.", "options"));

	l = new QLabel(__tr2qs_ctx("Real name:", "options"), gbox);
	pPropertiesBoxLayout->addWidget(l, 4, 0);

	m_pRealEditor = new QLineEdit(gbox);
	m_pRealEditor->setText(s->realName());
	pPropertiesBoxLayout->addWidget(m_pRealEditor, 4, 1);

	KviTalToolTip::add(m_pRealEditor, __tr2qs_ctx("You can specify a special <b>real name</b> that will be used to login with this server.<br>"
	                                              "If you leave this field empty (most common case), KVIrc will first look if a real name is specified "
	                                              "for the network that this server belongs to, and if that is empty the default \"real name\" "
						      "(specified in the \"Identity\" settings) will be used.", "options"));

	gbox = new KviTalGroupBox(Qt::Horizontal, __tr2qs_ctx("User Mode", "options"), tab);
	gl->addWidget(gbox, 1, 0);

	m_pUseDefaultInitUMode = new QCheckBox(__tr2qs_ctx("Use default user mode", "options"), gbox);
	KviTalToolTip::add(m_pUseDefaultInitUMode, __tr2qs_ctx("If this is enabled, the global initial <b>user mode</b> (configured from"
	                                                       " the identity dialog) will be used. If disabled, you can configure an initial user mode for this server", "options"));

	bool bHasUmode = !(s->initUMode().isEmpty());
	KviCString szDefUMode = KVI_OPTION_STRING(KviOption_stringDefaultUserMode);
	m_pUseDefaultInitUMode->setChecked(!bHasUmode);
	connect(m_pUseDefaultInitUMode, SIGNAL(toggled(bool)), this, SLOT(useDefaultInitUModeToggled(bool)));

	m_pIMode = new QCheckBox(__tr2qs_ctx("Invisible (+i)", "options"), gbox);
	m_pIMode->setEnabled(bHasUmode);
	m_pIMode->setChecked(bHasUmode ? bool(s->initUMode().contains('i', Qt::CaseInsensitive)) : bool(szDefUMode.contains('i', Qt::CaseInsensitive)));

	m_pSMode = new QCheckBox(__tr2qs_ctx("Server notices (+s)", "options"), gbox);
	m_pSMode->setEnabled(bHasUmode);
	m_pSMode->setChecked(bHasUmode ? bool(s->initUMode().contains('s', Qt::CaseInsensitive)) : bool(szDefUMode.contains('s', Qt::CaseInsensitive)));

	m_pWMode = new QCheckBox(__tr2qs_ctx("WALLOPS (+w)", "options"), gbox);
	m_pWMode->setEnabled(bHasUmode);
	m_pWMode->setChecked(bHasUmode ? bool(s->initUMode().contains('w', Qt::CaseInsensitive)) : bool(szDefUMode.contains('w', Qt::CaseInsensitive)));

	l = new QLabel("", tab);
	gl->addWidget(l, 2, 0);

	gl->setRowStretch(2, 1);

	tw->addTab(tab, *(g_pIconManager->getSmallIcon(KviIconManager::Ident)), __tr2qs_ctx("Identity", "options"));

	int iRow = 0;

	tab = new QWidget(tw);
	gl = new QGridLayout(tab);

	l = new QLabel(__tr2qs_ctx("Port:", "options"), tab);
	gl->addWidget(l, iRow, 0);

	m_pPortEditor = new QLineEdit(tab);
	gl->addWidget(m_pPortEditor, iRow, 1);
	QString tmpz;
	tmpz.setNum(s->port());
	m_pPortEditor->setText(tmpz);
	KviTalToolTip::add(m_pPortEditor, __tr2qs_ctx("This is the default <b>port</b> that this server will be contacted on.<br>Usually <b>6667</b> is OK.", "options"));
	connect(m_pPortEditor, SIGNAL(textChanged(const QString &)), this, SLOT(portEditorTextChanged(const QString &)));

	iRow++;

	l = new QLabel(__tr2qs_ctx("IP address:", "options"), tab);
	gl->addWidget(l, iRow, 0);
	m_pIpEditor = new KviIpEditor(tab, KviIpEditor::IPv4);
	gl->addWidget(m_pIpEditor, iRow, 1);
	KviTalToolTip::add(m_pIpEditor, __tr2qs_ctx("This is the <b>IP address</b> of this server, it is here for caching purposes.<br>"
	                                            "If you leave this field empty, KVIrc will fill it in the first time it connects to the server. "
	                                            "If you enable the \"cache IP address\" option below, KVIrc will use it as a \"cached result\" "
	                                            "and avoid looking it up again.", "options"));
#ifdef COMPILE_IPV6_SUPPORT
	m_pIpEditor->setAddressType(s->isIPv6() ? KviIpEditor::IPv6 : KviIpEditor::IPv4);
#else
	m_pIpEditor->setAddressType(KviIpEditor::IPv4);
#endif

	if(!m_pIpEditor->setAddress(s->ip()))
	{
#ifdef COMPILE_IPV6_SUPPORT
		m_pIpEditor->setAddress(s->isIPv6() ? "0:0:0:0:0:0:0:0" : "0.0.0.0");
#else
		m_pIpEditor->setAddress("0.0.0.0");
#endif
	}

	iRow++;

	QGridLayout * pCheckBoxLayout = new QGridLayout();
	gl->addLayout(pCheckBoxLayout, iRow, 0, 1, 2);

	m_pCacheIpCheck = new QCheckBox(__tr2qs_ctx("Cache IP address", "options"), tab);
	pCheckBoxLayout->addWidget(m_pCacheIpCheck, 0, 0);

	KviTalToolTip::add(m_pCacheIpCheck, __tr2qs_ctx("This check will enable <b>IP address caching</b> for this server:<br>"
	                                                "DNS lookups can be time-consuming and might be blocking on some platforms; "
	                                                "this option will cause KVIrc to look up the server hostname only once.<br><br> "
	                                                "Advanced: you can also use this option to force a certain server name to resolve "
	                                                "to a fixed IP address when either the DNS for that server is temporarily "
	                                                "unreachable or you want to avoid the round-robin lookups.", "options"));

	m_pCacheIpCheck->setChecked(s->cacheIp());
	m_pIpEditor->setEnabled(m_pCacheIpCheck->isChecked());
	connect(m_pCacheIpCheck, SIGNAL(toggled(bool)), this, SLOT(useCacheIpCheckToggled(bool)));

	m_pUseAutoConnect = new QCheckBox(__tr2qs_ctx("Connect to this server at startup", "options"), tab);
	m_pUseAutoConnect->setChecked(s->autoConnect());

	pCheckBoxLayout->addWidget(m_pUseAutoConnect, 0, 1);

	KviTalToolTip::add(m_pUseAutoConnect, __tr2qs_ctx("This option will cause KVIrc to connect to the IRC server when it is started.", "options"));

	m_pUseIPV6Check = new QCheckBox(__tr2qs_ctx("Use IPv6 protocol", "options"), tab);
	pCheckBoxLayout->addWidget(m_pUseIPV6Check, 1, 0);

#ifdef COMPILE_IPV6_SUPPORT
	m_pUseIPV6Check->setChecked(s->isIPv6());
	connect(m_pUseIPV6Check, SIGNAL(toggled(bool)), this, SLOT(useIPV6CheckToggled(bool)));
#else
	m_pUseIPV6Check->setEnabled(false);
#endif

	KviTalToolTip::add(m_pUseIPV6Check, __tr2qs_ctx("This check identifies IPv6 servers.<br>If enabled, KVIrc will attempt to use the IPv6 protocol "
	                                                "(thus your OS <b>must</b> have a working IPv6 stack and you <b>must</b> have an IPv6 connection).", "options"));

	m_pUseSSLCheck = new QCheckBox(__tr2qs_ctx("Use SSL protocol", "options"), tab);
	pCheckBoxLayout->addWidget(m_pUseSSLCheck, 1, 1);

	KviTalToolTip::add(m_pUseSSLCheck, __tr2qs_ctx("This check will cause the connection to use the <b>Secure Socket Layer</b> "
	                                               "encryption support. Obviously, this server must have support for this, too. :)", "options"));
#ifdef COMPILE_SSL_SUPPORT
	m_pUseSSLCheck->setChecked(s->useSSL());
	connect(m_pUseSSLCheck, SIGNAL(toggled(bool)), this, SLOT(useSSLCheckToggled(bool)));
#else
	m_pUseSSLCheck->setEnabled(false);
#endif

	iRow++;

	l = new QLabel(__tr2qs_ctx("Protocol encoding:", "options"), tab);
	gl->addWidget(l, iRow, 0);
	m_pEncodingEditor = new QComboBox(tab);
	m_pEncodingEditor->setDuplicatesEnabled(false);
	gl->addWidget(m_pEncodingEditor, iRow, 1);
	KviTalToolTip::add(m_pEncodingEditor, __tr2qs_ctx("This box allows you to choose the preferred encoding for this server. "
	                                                  "This encoding will be used for server specific needs, like referencing nicknames and channel names.<br>"
	                                                  "If you choose \"Use Network Encoding\" then the encoding will be inherited from the "
	                                                  "network that this server belongs to.", "options"));
	iRow++;

	l = new QLabel(__tr2qs_ctx("Text encoding:", "options"), tab);
	gl->addWidget(l, iRow, 0);
	m_pTextEncodingEditor = new QComboBox(tab);
	m_pTextEncodingEditor->setDuplicatesEnabled(false);
	gl->addWidget(m_pTextEncodingEditor, iRow, 1);
	KviTalToolTip::add(m_pTextEncodingEditor, __tr2qs_ctx("This box allows you to choose the preferred encoding for this server. "
	                                                      "This will be used as the default encoding for all text messages.<br>"
	                                                      "If you choose \"Use Network Encoding\" then the encoding will be inherited from the "
	                                                      "network that this server belongs to.", "options"));
	int i = 0;
	int srvcurrent = 0, txtcurrent = 0;
	KviLocale::EncodingDescription * d = KviLocale::instance()->encodingDescription(i);
	QString tmp;
	m_pEncodingEditor->addItem(__tr2qs_ctx("Use Network Encoding", "options"));
	m_pTextEncodingEditor->addItem(__tr2qs_ctx("Use Network Encoding", "options"));

	while(d->pcName)
	{
		tmp = QString("%1 (%2)").arg(d->pcName, d->pcDescription);
		m_pEncodingEditor->insertItem(m_pEncodingEditor->count(), tmp);
		m_pTextEncodingEditor->insertItem(m_pTextEncodingEditor->count(), tmp);
		if(KviQString::equalCI(d->pcName, s->encoding()))
			srvcurrent = i + 1;
		if(KviQString::equalCI(d->pcName, s->textEncoding()))
			txtcurrent = i + 1;
		i = i + 1;
		d = KviLocale::instance()->encodingDescription(i);
	}

	m_pEncodingEditor->setCurrentIndex(srvcurrent);
	m_pTextEncodingEditor->setCurrentIndex(txtcurrent);

	iRow++;

	l = new QLabel(__tr2qs_ctx("Proxy server:", "options"), tab);
	gl->addWidget(l, iRow, 0);
	m_pProxyEditor = new QComboBox(tab);
	gl->addWidget(m_pProxyEditor, iRow, 1);
	KviTalToolTip::add(m_pProxyEditor, __tr2qs_ctx("Defines the connection type that KVIrc should use to connect to this server.<br>"
	                                               "When it's set to *Default* KVIrc will use the global proxy settings, when it's set to *Direct Connection* "
	                                               "KVIrc will connect directly without using a proxy, or select a desired proxy you have previously added<br>"
	                                               "You can define or edit new proxy servers in *General Preferences* / *Connection* tab' \"Proxy hosts\" dialog.", "options"));

	m_pProxyEditor->addItem(__tr2qs_ctx("Default", "options"));
	m_pProxyEditor->addItem(__tr2qs_ctx("Direct Connection", "options"));

	std::vector<std::unique_ptr<KviProxy>> & proxylist = g_pProxyDataBase->proxyList();

	for(auto & p : proxylist)
		m_pProxyEditor->insertItem(m_pProxyEditor->count(), QString("%1:%2").arg(p->hostname()).arg(p->port()));

	if(m_pProxyEditor->count() > (s->proxy() + 2))
		m_pProxyEditor->setCurrentIndex(s->proxy() + 2);

	iRow++;

	l = new QLabel("", tab);
	gl->addWidget(l, iRow, 0, 1, 2);

	gl->setRowStretch(iRow, 1);

	tw->addTab(tab, *(g_pIconManager->getSmallIcon(KviIconManager::Socket)), __tr2qs_ctx("Connection", "options"));

	tab = new QWidget(tw);

	gl = new QGridLayout(tab);

	if(s->autoJoinChannelList())
		m_lstChannels = *(s->autoJoinChannelList());
	m_pChannelListSelector = new KviChannelListSelector(tab, __tr2qs_ctx("Channels to Join Automatically upon Connect", "options"), &m_lstChannels, true);
	KviTalToolTip::add(m_pChannelListSelector, __tr2qs_ctx("Here you can set a list of channels to be joined automatically "
	                                                       "after a connection to this server has been established. To add a channel, type its name in the "
	                                                       "text input below and click \"<b>Add</b>\".", "options"));

	gl->addWidget(m_pChannelListSelector, 0, 0);

	tw->addTab(tab, *(g_pIconManager->getSmallIcon(KviIconManager::Channel)), __tr2qs_ctx("Join Channels", "options"));

	tab = new QWidget(tw);
	gl = new QGridLayout(tab);

	m_pOnConnectEditor = KviScriptEditor::createInstance(tab);
	gl->addWidget(m_pOnConnectEditor, 0, 0);
	m_pOnConnectEditor->setText(s->onConnectCommand());
	m_pOnConnectEditor->setMinimumHeight(150);
	KviTalToolTip::add(m_pOnConnectEditor, __tr2qs_ctx("The following commands will be executed after a connection has been established.<br>"
	                                                   "<b>Important:</b> Enter commands <b>without</b> a preceding slash (e.g. <tt>quote pass secret</tt> instead of <tt>/quote pass secret</tt>).<br>"
	                                                   "KVIrc will first send the USER command, then eventually PASS and NICK and then execute this "
	                                                   "command sequence.<br>This is particularly useful for IRC bouncers that require login commands.", "options"));

	tw->addTab(tab, __tr2qs_ctx("On Connect", "options"));

	tab = new QWidget(tw);
	gl = new QGridLayout(tab);

	m_pOnLoginEditor = KviScriptEditor::createInstance(tab);
	gl->addWidget(m_pOnLoginEditor, 0, 0);
	m_pOnLoginEditor->setText(s->onLoginCommand());

	m_pOnLoginEditor->setMinimumHeight(150);

	KviTalToolTip::add(m_pOnLoginEditor, __tr2qs_ctx("The following commands will be executed after a successful login to this server.<br>"
	                                                 "<b>Important:</b> Enter commands <b>without</b> a preceding slash (e.g. <tt>quote privatelog</tt> instead of <tt>/quote privatelog</tt>).<br>"
	                                                 "This is useful for automatically opening queries, setting variables, etc.", "options"));

	tw->addTab(tab, __tr2qs_ctx("On Login", "options"));

	tab = new QWidget(tw);
	gl = new QGridLayout(tab);

	iRow = 0;

	m_pEnableSTARTTLSCheck = new QCheckBox(__tr2qs_ctx("Switch to SSL/TLS by using the STARTTLS extension", "options"), tab);
	gl->addWidget(m_pEnableSTARTTLSCheck, iRow, 0, 1, 2);
	KviTalToolTip::add(m_pEnableSTARTTLSCheck, __tr2qs_ctx("This check enables the use of the <b>Transport Layer Security</b> "
	                                                       "protocol. If you enable the Extended Capabilities below then the TLS protocol support will be detected by using "
	                                                       "a CAP LS command. Without \"Extended Capabilities\" the STARTTLS command will be forcibly sent at the beginning of the "
	                                                       "connection.", "options"));
#ifndef COMPILE_SSL_SUPPORT
	m_pEnableSTARTTLSCheck->setEnabled(false);
#endif
	m_pEnableSTARTTLSCheck->setChecked(s->enabledSTARTTLS());

	iRow++;

	QGroupBox * pCapGroup = new QGroupBox(__tr2qs_ctx("Extended Capabilities", "options"), tab);
	gl->addWidget(pCapGroup, iRow, 0, 1, 2);

	QGridLayout * pCapLayout = new QGridLayout();
	pCapGroup->setLayout(pCapLayout);

	m_pEnableCAPCheck = new QCheckBox(__tr2qs_ctx("Query extended capabilities on connect", "options"), tab);
	pCapLayout->addWidget(m_pEnableCAPCheck, 0, 0);

	KviTalToolTip::add(m_pEnableCAPCheck, __tr2qs_ctx("This check will cause the connection to use the <b>Extended Capability</b> support.", "options"));
	m_pEnableCAPCheck->setChecked(s->enabledCAP());

	QGroupBox * pSASLGroup = new QGroupBox(__tr2qs_ctx("SASL Authentication", "options"), tab);
	pCapLayout->addWidget(pSASLGroup, 2, 0);

	QGridLayout * pSASLLayout = new QGridLayout(pSASLGroup);

	m_pEnableSASLCheck = new QCheckBox(__tr2qs_ctx("Authenticate via SASL extension", "options"), pSASLGroup);
	pSASLLayout->addWidget(m_pEnableSASLCheck, 0, 0, 1, 2);
	KviTalToolTip::add(m_pEnableSASLCheck, __tr2qs_ctx("This check enables the use of the <b>SASL</b> authentication protocol "
	                                                   "If you enable the proper global option in the Connection/SSL tab and fill the SASL nickname and SASL "
	                                                   "password fields in this page, the SASL protocol will be used for this server if available.", "options"));

	m_pEnableSASLCheck->setChecked(s->enabledSASL());

	l = new QLabel(__tr2qs_ctx("SASL method:", "options"), pSASLGroup);
	pSASLLayout->addWidget(l, 1, 0);
	m_pSaslMethodComboBox = new QComboBox(pSASLGroup);
	m_pSaslMethodComboBox->setDuplicatesEnabled(false);
	for(auto&& method : KviSASL::supportedMethods())
		m_pSaslMethodComboBox->addItem(method);
	KviTalToolTip::add(m_pSaslMethodComboBox, __tr2qs_ctx("Select which SASL method you want to use to authenticate with.<br><br>"
	                                                      "EXTERNAL will fallback to PLAIN if a non-SSL connection is used or no .pem file is loaded.", "options"));
	pSASLLayout->addWidget(m_pSaslMethodComboBox, 1, 1);

	int index = m_pSaslMethodComboBox->findText(s->saslMethod());
	if(index != -1)
		m_pSaslMethodComboBox->setCurrentIndex(index);

	l = new QLabel(__tr2qs_ctx("SASL nickname:", "options"), pSASLGroup);
	pSASLLayout->addWidget(l, 2, 0);
	m_pSaslNickEditor = new QLineEdit(pSASLGroup);
	m_pSaslNickEditor->setText(s->saslNick());
	KviTalToolTip::add(m_pSaslNickEditor, __tr2qs_ctx("If you want to enable SASL authentication, insert your nickname here. (Not required for EXTERNAL)", "options"));
	pSASLLayout->addWidget(m_pSaslNickEditor, 2, 1);

	l = new QLabel(__tr2qs_ctx("SASL password:", "options"), pSASLGroup);
	pSASLLayout->addWidget(l, 3, 0);
	m_pSaslPassEditor = new KviPasswordLineEdit(pSASLGroup);
	m_pSaslPassEditor->setText(s->saslPass());
	KviTalToolTip::add(m_pSaslPassEditor, __tr2qs_ctx("If you want to enable SASL authentication, insert your password here. (Not required for EXTERNAL)", "options"));
	pSASLLayout->addWidget(m_pSaslPassEditor, 3, 1);

	pSASLGroup->setEnabled(s->enabledCAP());
	connect(m_pEnableCAPCheck, SIGNAL(toggled(bool)), pSASLGroup, SLOT(setEnabled(bool)));

	iRow++;

	l = new QLabel(__tr2qs_ctx("Link filter:", "options"), tab);
	gl->addWidget(l, iRow, 0);
	m_pLinkFilterEditor = new QComboBox(tab);
	m_pLinkFilterEditor->setEditable(true);
	m_pLinkFilterEditor->setDuplicatesEnabled(false);
	gl->addWidget(m_pLinkFilterEditor, iRow, 1);

	m_pLinkFilterEditor->addItem("");

	g_pModuleManager->loadModulesByCaps("linkfilter");
	KviModuleExtensionDescriptorList * mexl = KviModuleExtensionManager::instance()->getExtensionList("linkfilter");

	if(mexl)
	{
		for(KviModuleExtensionDescriptor * d = mexl->first(); d; d = mexl->next())
			m_pLinkFilterEditor->addItem(d->name().ptr());
	}
	else
	{
		if(!s->linkFilter().isEmpty())
			m_pLinkFilterEditor->addItem(s->linkFilter());
	}

	if(!s->linkFilter().isEmpty())
	{
		int i = m_pLinkFilterEditor->findText(s->linkFilter());
		if(i != -1)
			m_pLinkFilterEditor->setCurrentIndex(i);
		else
			m_pLinkFilterEditor->setEditText(s->linkFilter());
	}
	else
		m_pLinkFilterEditor->setCurrentIndex(0);

	KviTalToolTip::add(m_pLinkFilterEditor, __tr2qs_ctx("This field specifies the name of a module that exports a link filter for this type of server.<br>"
	                                                    "For plain IRC connections, you don't need any link filters; this is used for incompatible protocols.", "options"));

	iRow++;

	l = new QLabel(__tr2qs_ctx("ID:", "options"), tab);
	gl->addWidget(l, iRow, 0);
	m_pIdEditor = new QLineEdit(tab);
	if(s->id().isEmpty())
		s->generateUniqueId();
	m_pIdEditor->setText(s->id());
	gl->addWidget(m_pIdEditor, iRow, 1);

	KviTalToolTip::add(m_pIdEditor, __tr2qs_ctx("This field allows you to specify a really unique ID for this server. "
	                                            "You will then be able to use /server -x &lt;this_id&gt; to make the connection. This is especially "
	                                            "useful when you have multiple server entries with the same hostname and port in different networks (bouncers?)", "options"));
	iRow++;

	tw->addTab(tab, __tr2qs_ctx("Advanced", "options"));

	// Placeholder for NickServ options (text with redirect to network settings)
	tab = new QWidget(tw);
	gl = new QGridLayout(tab);
	iRow = 0;

	l = new QLabel(__tr2qs_ctx("NickServ settings are available only for the whole IRC network, not for specific servers.\n"
	                           "To open network settings choose a network (with a globe) in list of networks and servers and click Advanced.", "options"),
	    tab);
	gl->addWidget(l, iRow, 0);
	gl->setRowStretch(1, 1);

	tw->addTab(tab, __tr2qs_ctx("NickServ", "options"));

	QPushButton * b = new QPushButton(__tr2qs_ctx("OK", "options"), this);
	b->setMinimumWidth(80);
	g->addWidget(b, 3, 2);
	b->setDefault(true);
	connect(b, SIGNAL(clicked()), this, SLOT(accept()));

	b = new QPushButton(__tr2qs_ctx("Cancel", "options"), this);
	g->addWidget(b, 3, 3);
	b->setMinimumWidth(80);
	connect(b, SIGNAL(clicked()), this, SLOT(reject()));

	g->setRowStretch(2, 1);
	g->setColumnStretch(1, 1);

	tw->setMinimumWidth(390);

	setHeaderLabelText();
}

IrcServerDetailsWidget::~IrcServerDetailsWidget()
{
	if(m_pOnConnectEditor)
		KviScriptEditor::destroyInstance(m_pOnConnectEditor);
	if(m_pOnLoginEditor)
		KviScriptEditor::destroyInstance(m_pOnLoginEditor);
}

void IrcServerDetailsWidget::useCacheIpCheckToggled(bool)
{
	m_pIpEditor->setEnabled(m_pCacheIpCheck->isChecked());
}

void IrcServerDetailsWidget::useIPV6CheckToggled(bool)
{
#ifdef COMPILE_IPV6_SUPPORT
	m_pIpEditor->setAddressType(m_pUseIPV6Check->isChecked() ? KviIpEditor::IPv6 : KviIpEditor::IPv4);
#else
	m_pIpEditor->setAddressType(KviIpEditor::IPv4);
#endif
	setHeaderLabelText();
}

void IrcServerDetailsWidget::useSSLCheckToggled(bool)
{
	setHeaderLabelText();
}

void IrcServerDetailsWidget::portEditorTextChanged(const QString &)
{
	setHeaderLabelText();
}

void IrcServerDetailsWidget::setHeaderLabelText()
{
	QString szTmp = "<font size=\"+1\"><b>irc";
	if(m_pUseSSLCheck->isChecked())
		szTmp += "s";
	if(m_pUseIPV6Check->isChecked())
		szTmp += "6";
	szTmp += "://";
	szTmp += m_szHostname;
	szTmp += ":";

	bool bOk;

	QString num = m_pPortEditor->text();
	unsigned int uPort = num.toUInt(&bOk);
	if(!bOk)
		uPort = 6667;
	num.setNum(uPort);
	szTmp += num;
	szTmp += "</b></font>";

	m_pHeaderLabel->setText(szTmp);
}

void IrcServerDetailsWidget::fillData(KviIrcServer * s)
{
	s->setUserName(m_pUserEditor->text());
	s->setPassword(m_pPassEditor->text());
	s->setNickName(m_pNickEditor->text());
	s->setAlternativeNickName(m_pAlternativeNickEditor->text());
	s->setRealName(m_pRealEditor->text());
	if(m_pDescEditor)
		s->setDescription(m_pDescEditor->text());

	if(m_pLinkFilterEditor)
		s->setLinkFilter(m_pLinkFilterEditor->lineEdit()->text());

	if(m_pEncodingEditor)
	{
		if(m_pEncodingEditor->currentIndex() <= 0)
		{
			s->setEncoding(QString());
		}
		else
		{
			KviLocale::EncodingDescription * d = KviLocale::instance()->encodingDescription(m_pEncodingEditor->currentIndex() - 1);
			s->setEncoding(d->pcName);
		}
	}

	if(m_pTextEncodingEditor)
	{
		if(m_pTextEncodingEditor->currentIndex() <= 0)
		{
			s->setTextEncoding(QString());
		}
		else
		{
			KviLocale::EncodingDescription * dd = KviLocale::instance()->encodingDescription(m_pTextEncodingEditor->currentIndex() - 1);
			s->setTextEncoding(dd->pcName);
		}
	}

	s->setIp("");
	if(m_pCacheIpCheck)
		s->setCacheIp(m_pCacheIpCheck->isChecked());

	if(m_pUseSSLCheck)
		s->setUseSSL(m_pUseSSLCheck->isChecked());

	if(m_pEnableCAPCheck)
		s->setEnabledCAP(m_pEnableCAPCheck->isChecked());

	if(m_pEnableSTARTTLSCheck)
		s->setEnabledSTARTTLS(m_pEnableSTARTTLSCheck->isChecked());

	s->setSaslMethod(m_pSaslMethodComboBox->currentText());
	s->setSaslNick(m_pSaslNickEditor->text());
	s->setSaslPass(m_pSaslPassEditor->text());
	if(m_pEnableSASLCheck)
		s->setEnabledSASL(m_pEnableSASLCheck->isChecked() && ((!m_pSaslNickEditor->text().isEmpty() && !m_pSaslPassEditor->text().isEmpty()) || (m_pSaslMethodComboBox->currentText() == QStringLiteral("EXTERNAL"))));
	if(m_pIdEditor)
		s->setId(m_pIdEditor->text());
	if(s->id().isEmpty())
		s->generateUniqueId();

	if(m_pUseDefaultInitUMode)
	{
		KviCString szUMode;
		if(!m_pUseDefaultInitUMode->isChecked())
		{
			if(m_pIMode->isChecked())
				szUMode.append('i');
			if(m_pWMode->isChecked())
				szUMode.append('w');
			if(m_pSMode->isChecked())
				szUMode.append('s');
		}
		s->setInitUMode(szUMode.ptr());
	}

	QString tmp = m_pPortEditor->text();
	bool bOk;
	kvi_u32_t uPort = tmp.toUInt(&bOk);
	if(!bOk)
		uPort = 6667;
	s->setPort(uPort);
#ifdef COMPILE_IPV6_SUPPORT
	s->setIPv6(m_pUseIPV6Check->isChecked());
#else
	s->setIPv6(false);
#endif

	if(m_pIpEditor)
	{
		if(m_pIpEditor->isValid())
		{
			QString tmpAddr = m_pIpEditor->address();
#ifdef COMPILE_IPV6_SUPPORT
			if(s->isIPv6())
			{
				if(tmpAddr != "::" && KviNetUtils::isValidStringIPv6(tmpAddr))
				{
					s->setIp(tmpAddr);
				}
				else
				{
					s->setCacheIp(false);
					s->setIp("");
				}
			}
			else
			{
#endif
				if(tmpAddr != "0.0.0.0" && KviNetUtils::isValidStringIp(tmpAddr))
				{
					s->setIp(tmpAddr);
				}
				else
				{
					s->setCacheIp(false);
					s->setIp("");
				}
#ifdef COMPILE_IPV6_SUPPORT
			}
#endif
		}
		else
		{
			s->setCacheIp(false);
			s->setIp("");
		}
	}
	if(m_pOnConnectEditor)
	{
		QString tmp;
		m_pOnConnectEditor->getText(tmp);
		s->setOnConnectCommand(tmp);
	}

	if(m_pOnLoginEditor)
	{
		QString tmp;
		m_pOnLoginEditor->getText(tmp);
		s->setOnLoginCommand(tmp);
	}

	if(m_pChannelListSelector)
		m_pChannelListSelector->commit();
	if(m_lstChannels.isEmpty())
		s->setAutoJoinChannelList(nullptr);
	else
		s->setAutoJoinChannelList(new QStringList(m_lstChannels));

	if(m_pUseAutoConnect)
		s->setAutoConnect(m_pUseAutoConnect->isChecked());

	if(m_pProxyEditor)
		s->setProxy(m_pProxyEditor->currentIndex() - 2);
}

void IrcServerDetailsWidget::useDefaultInitUModeToggled(bool b)
{
	m_pIMode->setEnabled(!b);
	m_pWMode->setEnabled(!b);
	m_pSMode->setEnabled(!b);
}

// KviApplication.cpp
extern KVIRC_API KviIrcServerDataBase * g_pServerDataBase;

IrcServerOptionsTreeWidgetItem::IrcServerOptionsTreeWidgetItem(QTreeWidget * parent, const QPixmap & pm, const KviIrcNetwork * n)
    : QTreeWidgetItem(parent)
{
	setIcon(0, QIcon(pm));
	m_pServerData = nullptr;
	m_pNetworkData = new KviIrcNetwork(*n);
	setText(0, n->name());
	setText(1, n->description());
}

IrcServerOptionsTreeWidgetItem::IrcServerOptionsTreeWidgetItem(QTreeWidgetItem * parent, const QPixmap & pm, const KviIrcServer * s)
    : QTreeWidgetItem(parent)
{
	setIcon(0, QIcon(pm));
	m_pServerData = new KviIrcServer(*s);
	setText(0, s->hostName());
	setText(1, s->description());
	m_pNetworkData = nullptr;
}

IrcServerOptionsTreeWidgetItem::~IrcServerOptionsTreeWidgetItem()
{
	delete m_pServerData;
	delete m_pNetworkData;
}

void IrcServerOptionsTreeWidgetItem::updateVisibleStrings()
{
	if(m_pNetworkData)
	{
		setText(0, m_pNetworkData->name());
		setText(1, m_pNetworkData->description());
		return;
	}

	if(m_pServerData)
	{
		setText(0, m_pServerData->hostName());
		setText(1, m_pServerData->description());
	}
}

OptionsWidget_servers::OptionsWidget_servers(QWidget * parent)
    : KviOptionsWidget(parent)
{
	setObjectName("server_options_widget");
	createLayout();

	m_pContextPopup = new QMenu(this);
	m_pImportPopup = new QMenu(this);

	connect(m_pImportPopup, SIGNAL(aboutToShow()), this, SLOT(importPopupAboutToShow()));
	connect(m_pImportPopup, SIGNAL(triggered(QAction *)), this, SLOT(importPopupActivated(QAction *)));

	m_pServerDetailsDialog = nullptr;
	m_pNetworkDetailsDialog = nullptr;
	m_pImportFilter = nullptr;

	m_pFilterLabel = new QLabel(__tr2qs_ctx("Filter:", "options"), this);
	addWidgetToLayout(m_pFilterLabel, 0, 0, 0, 0);

	m_pFilterEdit = new QLineEdit(this);
	connect(m_pFilterEdit, SIGNAL(textEdited(const QString &)), this, SLOT(filterTextEdited(const QString &)));
	KviTalToolTip::add(m_pFilterEdit, __tr2qs_ctx("If you are searching for a specific server or network, you can insert its name to filter the servers in the list", "options"));
	addWidgetToLayout(m_pFilterEdit, 1, 0, 1, 0);

	m_pShowFavoritesOnlyButton = new QToolButton(this);
	m_pShowFavoritesOnlyButton->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::FavoriteOff)));
	m_pShowFavoritesOnlyButton->setCheckable(true);
	m_pShowFavoritesOnlyButton->setChecked(KVI_OPTION_BOOL(KviOption_boolShowFavoriteServersOnly));
	KviTalToolTip::add(m_pShowFavoritesOnlyButton, __tr2qs_ctx("If this option is enabled, only servers you have favorited will be displayed", "options"));
	addWidgetToLayout(m_pShowFavoritesOnlyButton, 3, 0, 3, 0);
	connect(m_pShowFavoritesOnlyButton, SIGNAL(toggled(bool)), this, SLOT(updateFavoritesFilter(bool))); // Sets the server to a favorite

	m_pTreeWidget = new QTreeWidget(this);
	addWidgetToLayout(m_pTreeWidget, 0, 1, 1, 1);
	m_pTreeWidget->setColumnCount(2);
	QStringList columLabels;
	columLabels.append(__tr2qs_ctx("Server", "options"));
	columLabels.append(__tr2qs_ctx("Description", "options"));
	m_pTreeWidget->setColumnWidth(0, 300);
	m_pTreeWidget->setColumnWidth(1, 300);

	m_pTreeWidget->setSortingEnabled(true);
	m_pTreeWidget->sortByColumn(0, Qt::AscendingOrder);
	m_pTreeWidget->setHeaderLabels(columLabels);
	m_pTreeWidget->setRootIsDecorated(true);
	m_pTreeWidget->setAllColumnsShowFocus(true);
	m_pTreeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_pTreeWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(customContextMenuRequested(const QPoint &)));
	connect(m_pTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));
	connect(m_pTreeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem *, int)));

	QString tiptxt = __tr2qs_ctx("This is the list of available IRC servers.<br>"
	                             "Right-click on the list to add or remove servers and perform other actions.<br>"
	                             "Double-click on a item for advanced options.", "options");

	KviTalToolTip::add(m_pTreeWidget, tiptxt);
	KviTalToolTip::add(m_pTreeWidget->viewport(), tiptxt);

	KviTalVBox * vbox = new KviTalVBox(this);
	vbox->setSpacing(0);
	addWidgetToLayout(vbox, 3, 1, 3, 1);

	m_pNewNetworkButton = new QToolButton(vbox);
	m_pNewNetworkButton->setIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::NewNetwork))));
	m_pNewNetworkButton->setAutoRaise(true);
	connect(m_pNewNetworkButton, SIGNAL(clicked()), this, SLOT(newNetwork()));
	KviTalToolTip::add(m_pNewNetworkButton, __tr2qs_ctx("New network", "options"));

	QFrame * f = new QFrame(vbox);
	f->setFrameStyle(QFrame::Sunken | QFrame::HLine);

	m_pNewServerButton = new QToolButton(vbox);
	m_pNewServerButton->setIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::NewServer))));
	m_pNewServerButton->setAutoRaise(true);
	connect(m_pNewServerButton, SIGNAL(clicked()), this, SLOT(newServer()));
	KviTalToolTip::add(m_pNewServerButton, __tr2qs_ctx("New server", "options"));

	m_pFavoriteServerButton = new QToolButton(vbox);
	m_pFavoriteServerButton->setIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::ServerFavorite))));
	m_pFavoriteServerButton->setAutoRaise(true);
	connect(m_pFavoriteServerButton, SIGNAL(clicked()), this, SLOT(favoriteServer()));
	KviTalToolTip::add(m_pFavoriteServerButton, __tr2qs_ctx("Favorite / Unfavorite server", "options"));

	m_pRemoveButton = new QToolButton(vbox);
	m_pRemoveButton->setIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Remove))));
	m_pRemoveButton->setEnabled(false);
	m_pRemoveButton->setAutoRaise(true);
	connect(m_pRemoveButton, SIGNAL(clicked()), this, SLOT(removeCurrent()));
	KviTalToolTip::add(m_pRemoveButton, __tr2qs_ctx("Remove network / server", "options"));

	m_pCopyServerButton = new QToolButton(vbox);
	m_pCopyServerButton->setIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Copy))));
	m_pCopyServerButton->setEnabled(false);
	m_pCopyServerButton->setAutoRaise(true);
	connect(m_pCopyServerButton, SIGNAL(clicked()), this, SLOT(copyServer()));
	KviTalToolTip::add(m_pCopyServerButton, __tr2qs_ctx("Copy server", "options"));

	m_pPasteServerButton = new QToolButton(vbox);
	m_pPasteServerButton->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Paste)));
	m_pPasteServerButton->setEnabled(false);
	m_pPasteServerButton->setAutoRaise(true);
	connect(m_pPasteServerButton, SIGNAL(clicked()), this, SLOT(pasteServer()));
	KviTalToolTip::add(m_pPasteServerButton, __tr2qs_ctx("Paste server", "options"));

	f = new QFrame(vbox);
	f->setFrameStyle(QFrame::Sunken | QFrame::HLine);

	m_pImportButton = new QToolButton(vbox);
	m_pImportButton->setIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Folder))));
	m_pImportButton->setAutoRaise(true);
	m_pImportButton->setMenu(m_pImportPopup);
	m_pImportButton->setPopupMode(QToolButton::InstantPopup);

	KviTalToolTip::add(m_pImportButton, __tr2qs_ctx("Import server list", "options"));

	QFrame * lll = new QFrame(vbox);
	vbox->setStretchFactor(lll, 100);

	KviTalGroupBox * gbox = addGroupBox(0, 2, 3, 2, Qt::Vertical, __tr2qs_ctx("Active Configuration", "options"));
	m_pSrvNetLabel = new QLabel(__tr2qs_ctx("Server:", "options"), gbox);

	m_pSrvNetEdit = new QLineEdit(gbox);
	connect(m_pSrvNetEdit, SIGNAL(textEdited(const QString &)), this, SLOT(serverNetworkEditTextEdited(const QString &)));
	KviTalToolTip::add(m_pSrvNetEdit, __tr2qs_ctx("This is the name of the currently selected server or network", "options"));

	m_pDetailsButton = new QPushButton(__tr2qs_ctx("Advanced...", "options"), gbox);
	connect(m_pDetailsButton, SIGNAL(clicked()), this, SLOT(detailsClicked()));
	KviTalToolTip::add(m_pDetailsButton, __tr2qs_ctx("Click here to edit advanced options for this entry", "options"));

	m_pRecentPopup = new QMenu(gbox);
	connect(m_pRecentPopup, SIGNAL(aboutToShow()), this, SLOT(recentServersPopupAboutToShow()));
	connect(m_pRecentPopup, SIGNAL(triggered(QAction *)), this, SLOT(recentServersPopupClicked(QAction *)));

	QToolButton * tb = new QToolButton(gbox);
	tb->setIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::History))));
	tb->setMenu(m_pRecentPopup);
	tb->setAutoRaise(true);
	tb->setPopupMode(QToolButton::InstantPopup);

	KviTalToolTip::add(tb, __tr2qs_ctx("This button shows a list of recently used servers.<br>It allows you to quickly find them in the list.", "options"));

	m_pShowThisDialogAtStartupSelector = nullptr;

	// The "Show this dialog at startup" option is shown only when the server options widget is shown as standalone dialog
	if(parent->inherits("OptionsWidgetContainer"))
	{
		m_pConnectCurrent = new QPushButton(__tr2qs_ctx("Connect &Now", "options"), this);
		addWidgetToLayout(m_pConnectCurrent, 0, 3, 3, 3);
		connect(m_pConnectCurrent, SIGNAL(clicked()), this, SLOT(connectCurrentClicked()));

		QPalette pal(QColor(0, 0, 0));

		/* QPalette::active defines the colors of the button/text when the KVIrc window has focus
		   Keep button/text color in a spectrum that indicates a connect action is possible */
		pal.setColor(QPalette::Active, QPalette::Button, QColor(0, 135, 0));
		pal.setColor(QPalette::Active, QPalette::ButtonText, QColor(245, 245, 245));

		/* QPalette::Inactive defines the colors of the button/text when the KVIrc window looses focus
		   Keep button/text color the same as QPalette::Active but use different shades */
		pal.setColor(QPalette::Inactive, QPalette::Button, QColor(50, 135, 50));
		pal.setColor(QPalette::Inactive, QPalette::ButtonText, QColor(225, 225, 225));

		/* QPalette::Disabled defines the colors of the button/text when for instance network is selected
		   Keep button/text color in a spectrum that indicates no possible action is possible */
		pal.setColor(QPalette::Disabled, QPalette::Button, QColor(100, 100, 100));
		pal.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(180, 180, 180));

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW) || defined(COMPILE_ON_MAC)
		// Kill the WindowXP/Mac style which does not honor palette
		QStyle * pWindowsStyle = QStyleFactory::create("plastique");

		if(!pWindowsStyle)
		{
			pWindowsStyle = QStyleFactory::create("windows");
			if(!pWindowsStyle)
				pWindowsStyle = QStyleFactory::create("motif");
		}
		if(pWindowsStyle)
			m_pConnectCurrent->setStyle(pWindowsStyle);
#endif //COMPILE_ON_WINDOWS || COMPILE_ON_MINGW

		m_pConnectCurrent->setPalette(pal);

		QFont fnt = m_pConnectCurrent->font();
		fnt.setPointSizeF(fnt.pointSizeF() * 1.4);
		fnt.setBold(true);
		m_pConnectCurrent->setFont(fnt);

		KviTalToolTip::add(m_pConnectCurrent, __tr2qs_ctx("Hit this button to connect to the currently selected server.", "options"));

		OptionsWidgetContainer * pContainer = dynamic_cast<OptionsWidgetContainer *>(parent);

		if(pContainer)
		{
			m_pShowThisDialogAtStartupSelector = addBoolSelector(pContainer, __tr2qs_ctx("Show this dialog at startup", "options"), KviOption_boolShowServersConnectDialogOnStart);
			pContainer->setLeftCornerWidget(m_pShowThisDialogAtStartupSelector);
			// This selector can be destroyed upon reparenting: make sure it's removed from the selector list
			// (or we'll get a crash at commit() time...).
			connect(m_pShowThisDialogAtStartupSelector, SIGNAL(destroyed()), this, SLOT(slotShowThisDialogAtStartupSelectorDestroyed()));
			KviTalToolTip::add(m_pShowThisDialogAtStartupSelector, __tr2qs_ctx("If this option is enabled, the servers dialog will appear every time you start KVIrc", "options"));
		}

		new QShortcut(Qt::Key_Escape, parent, SLOT(close()));
	}
	else
	{
		m_pConnectCurrent = nullptr;
	}

	m_pLastEditedItem = nullptr;

	m_pClipboard = nullptr;

	m_bShowingFavoritesOnly = KVI_OPTION_BOOL(KviOption_boolShowFavoriteServersOnly);

	fillServerList();
	updateFavoritesFilter(KVI_OPTION_BOOL(KviOption_boolShowFavoriteServersOnly));

	layout()->setRowStretch(1, 1);
	layout()->setColumnStretch(1, 1);
	setMinimumWidth(600);  // made this dialog taller but cant make it wider https://github.com/kvirc/KVIrc/issues/1355
	setMinimumHeight(440); // The server dilaog opens inside general prefs as well and wider makes the parent window way too large
}

OptionsWidget_servers::~OptionsWidget_servers()
{
	if(m_pImportFilter)
	{
		disconnect(m_pImportFilter, nullptr, this, nullptr);
		m_pImportFilter->die();
		m_pImportFilter = nullptr;
	}

	delete m_pClipboard;
	delete m_pServerDetailsDialog;
	delete m_pNetworkDetailsDialog;
}

void OptionsWidget_servers::slotShowThisDialogAtStartupSelectorDestroyed()
{
	KVI_ASSERT(m_pShowThisDialogAtStartupSelector);

	removeSelector(m_pShowThisDialogAtStartupSelector);
	m_pShowThisDialogAtStartupSelector = nullptr;
}

void OptionsWidget_servers::recentServersPopupAboutToShow()
{
	g_pApp->fillRecentServersPopup(m_pRecentPopup);

	m_pRecentPopup->addSeparator();
	m_pRecentPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Remove)), __tr2qs("Clear Recent Servers List"));
}

void OptionsWidget_servers::recentServersPopupClicked(QAction * pAction)
{
	if(!g_pActiveWindow)
		return; // doh

	KviConsoleWindow * c = g_pActiveWindow->console();
	if(!c)
		return;

	QString szItemText = pAction->text();
	szItemText.remove(QChar('&'));
	if(szItemText.isEmpty())
		return;

	if(szItemText == __tr2qs("Clear Recent Servers List"))
	{
		KviKvsScript::run("option stringlistRecentServers", c);
		return;
	}

	selectBestServerByUrl(szItemText);
}

void OptionsWidget_servers::selectBestServerByUrl(const QString & szUrl)
{
	if(szUrl.isEmpty())
		return;

	KviIrcUrlParts oParts;

	KviIrcUrl::split(szUrl, oParts);

	int uCount = m_pTreeWidget->topLevelItemCount();
	int uIdx = 0;

	IrcServerOptionsTreeWidgetItem * pBestCandidate = nullptr;
	kvi_u32_t uBestCandidateScore = 0;

	while(uIdx < uCount)
	{
		IrcServerOptionsTreeWidgetItem * pNet = static_cast<IrcServerOptionsTreeWidgetItem *>(m_pTreeWidget->topLevelItem(uIdx));

		uIdx++;

		if(!pNet)
			continue; // huh ?

		int uServerCount = pNet->childCount();
		int uChildIdx = 0;
		while(uChildIdx < uServerCount)
		{
			IrcServerOptionsTreeWidgetItem * pServer = static_cast<IrcServerOptionsTreeWidgetItem *>(pNet->child(uChildIdx));
			uChildIdx++;

			if(!pServer)
				continue; // huh ?

			KviIrcServer * pServerData = pServer->serverData();

			if(!pServerData)
				continue; // umphf...

			kvi_u32_t uScore = 0;

			if(pServerData->hostName().toLower() == oParts.szHost.toLower())
				uScore++;

			if(pServerData->port() == oParts.iPort)
				uScore++;

			if(pServerData->isIPv6() == oParts.bIPv6)
				uScore++;

			if(pServerData->useSSL() == oParts.bSsl)
				uScore++;

			if(uScore > uBestCandidateScore)
			{
				uBestCandidateScore = uScore;
				pBestCandidate = pServer;
				if(uScore >= 4)
				{                      // exact match
					uIdx = uCount; // break the outer loop
					break;         // break the inner loop
				}
			}
		}
	}

	if(pBestCandidate)
		m_pTreeWidget->setCurrentItem(pBestCandidate);
}

void OptionsWidget_servers::connectCurrentClicked()
{
	saveLastItem();
	commit();
	KviCString szServer = m_pSrvNetEdit->text();
	if(szServer.isEmpty())
		return;
	KviKvsScript::run("server -u", g_pApp->activeConsole());
}

void OptionsWidget_servers::fillServerList()
{
	IrcServerOptionsTreeWidgetItem * net;
	IrcServerOptionsTreeWidgetItem * srv;
	IrcServerOptionsTreeWidgetItem * cur = nullptr;

	KviPointerHashTableIterator<QString, KviIrcNetwork> it(*(g_pServerDataBase->recordDict()));

	if(m_pConnectCurrent)
		m_pConnectCurrent->setEnabled(false);

	while(KviIrcNetwork * r = it.current())
	{
		net = new IrcServerOptionsTreeWidgetItem(m_pTreeWidget, *(g_pIconManager->getSmallIcon(KviIconManager::World)), r);
		KviPointerList<KviIrcServer> * sl = r->serverList();
		bool bCurrent = r->name() == g_pServerDataBase->currentNetworkName().toUtf8().data();
		net->setExpanded(bCurrent);

		for(KviIrcServer * s = sl->first(); s; s = sl->next())
		{
			unsigned icon = s->favorite() ? KviIconManager::ServerFavorite : KviIconManager::Server;
			srv = new IrcServerOptionsTreeWidgetItem(net, *(g_pIconManager->getSmallIcon(icon)), s);

			if((s == r->currentServer()) && bCurrent)
			{
				srv->setSelected(true);
				cur = srv;
			}
		}
		++it;
	}
	if(cur)
	{
		cur->setSelected(true);
		m_pTreeWidget->setCurrentItem(cur);
		m_pTreeWidget->scrollToItem(cur);
		if(m_pConnectCurrent)
			m_pConnectCurrent->setEnabled(true);
	}
}

void OptionsWidget_servers::itemDoubleClicked(QTreeWidgetItem *, int)
{
	detailsClicked();
}

void OptionsWidget_servers::currentItemChanged(QTreeWidgetItem * it, QTreeWidgetItem *)
{
	saveLastItem();
	m_pLastEditedItem = (IrcServerOptionsTreeWidgetItem *)it;

	if(m_pLastEditedItem)
	{
		m_pSrvNetLabel->setEnabled(true);
		m_pSrvNetEdit->setEnabled(true);
		m_pDetailsButton->setEnabled(true);
		if(m_pConnectCurrent)
			m_pConnectCurrent->setEnabled(m_pLastEditedItem->m_pServerData);

		m_pRemoveButton->setEnabled(true);
		m_pCopyServerButton->setEnabled(m_pLastEditedItem->m_pServerData);
		m_pFavoriteServerButton->setEnabled(m_pLastEditedItem->m_pServerData);

		if(m_pLastEditedItem->m_pServerData)
		{
			m_pSrvNetLabel->setText(__tr2qs_ctx("Server:", "options"));
			m_pSrvNetEdit->setText(m_pLastEditedItem->m_pServerData->hostName());
		}
		else
		{
			m_pSrvNetLabel->setText(__tr2qs_ctx("Network:", "options"));
			m_pSrvNetEdit->setText(it->text(0));
		}
	}
	else
	{
		m_pRemoveButton->setEnabled(false);
		m_pCopyServerButton->setEnabled(true);
		m_pFavoriteServerButton->setEnabled(true);

		if(m_pConnectCurrent)
			m_pConnectCurrent->setEnabled(false);

		m_pSrvNetLabel->setEnabled(false);
		m_pSrvNetEdit->setEnabled(false);
		m_pSrvNetEdit->setText(__tr2qs_ctx("No selection", "options"));
		m_pDetailsButton->setEnabled(false);
	}
}

void OptionsWidget_servers::serverNetworkEditTextEdited(const QString &)
{
	if(!m_pLastEditedItem)
		return;

	saveLastItem();

	// make sure the current item is currently visible (if it still exists)
	if(m_pLastEditedItem)
		m_pTreeWidget->scrollToItem(m_pLastEditedItem, QTreeWidget::EnsureVisible);
}

void OptionsWidget_servers::filterTextEdited(const QString &)
{
	QString szFilter = m_pFilterEdit->text().trimmed();

	IrcServerOptionsTreeWidgetItem * network;
	for(int i = 0; i < m_pTreeWidget->topLevelItemCount(); i++)
	{
		network = (IrcServerOptionsTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		if(network->m_pNetworkData->name().contains(szFilter, Qt::CaseInsensitive) || network->m_pNetworkData->description().contains(szFilter, Qt::CaseInsensitive))
		{
			network->setHidden(false);
			// if the net matches, we always show all its servers
			IrcServerOptionsTreeWidgetItem * ch;
			for(int j = 0; j < network->childCount(); j++)
			{
				ch = (IrcServerOptionsTreeWidgetItem *)network->child(j);
				ch->setHidden(false);
			}
		}
		else
		{
			uint uServers = 0;

			IrcServerOptionsTreeWidgetItem * ch;
			for(int j = 0; j < network->childCount(); j++)
			{
				bool bHidden = true;
				ch = (IrcServerOptionsTreeWidgetItem *)network->child(j);
				if(ch->m_pServerData)
				{
					if(ch->m_pServerData->hostName().contains(szFilter, Qt::CaseInsensitive) || ch->m_pServerData->description().contains(szFilter, Qt::CaseInsensitive))
						bHidden = false;
				}

				if(!bHidden)
					uServers++;
				ch->setHidden(bHidden);
			}
			// if at list one server matches, we show its network
			network->setHidden(!uServers);
		}
	}
}

void OptionsWidget_servers::saveLastItem()
{
	if(!m_pLastEditedItem)
		return;

	if(m_pLastEditedItem->m_pServerData)
	{
		QString tmp = m_pSrvNetEdit->text();
		if(tmp.isEmpty())
			tmp = QString::fromLatin1("irc.unknown.net");

		m_pLastEditedItem->m_pServerData->setHostName(tmp);
		m_pLastEditedItem->updateVisibleStrings();
	}
	else if(m_pLastEditedItem->m_pNetworkData)
	{
		QString tmp = m_pSrvNetEdit->text();
		if(tmp.isEmpty())
			tmp = __tr2qs_ctx("UnknownNet", "options");

		m_pLastEditedItem->m_pNetworkData->setName(tmp);
		m_pLastEditedItem->updateVisibleStrings();
	}
}

void OptionsWidget_servers::commit()
{
	saveLastItem();

	g_pServerDataBase->clear();

	IrcServerOptionsTreeWidgetItem * network;
	for(int i = 0; i < m_pTreeWidget->topLevelItemCount(); i++)
	{
		network = (IrcServerOptionsTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		QString tmp = network->m_pNetworkData->name();
		KviIrcNetwork * pNetwork = g_pServerDataBase->findNetwork(tmp);

		if(pNetwork)
		{
			pNetwork->copyFrom(*(network->m_pNetworkData));
		}
		else
		{
			pNetwork = new KviIrcNetwork(tmp);
			pNetwork->copyFrom(*(network->m_pNetworkData));
			g_pServerDataBase->addNetwork(pNetwork);
		}

		if(network == m_pLastEditedItem)
			g_pServerDataBase->setCurrentNetwork(pNetwork->name());

		IrcServerOptionsTreeWidgetItem * ch;
		for(int j = 0; j < network->childCount(); j++)
		{
			KviIrcServer * srv;
			ch = (IrcServerOptionsTreeWidgetItem *)network->child(j);

			if(ch->m_pServerData)
			{
				if(!ch->m_pServerData->hostName().isEmpty())
				{
					srv = pNetwork->findServer(ch->m_pServerData);

					if(!srv)
					{
						srv = new KviIrcServer(*(ch->m_pServerData));
						pNetwork->insertServer(srv);
					}
					else
						*srv = *(ch->m_pServerData);
					if(srv->id().isEmpty())
						srv->generateUniqueId();

					if(ch == m_pLastEditedItem)
					{
						g_pServerDataBase->setCurrentNetwork(pNetwork->name());
						pNetwork->setCurrentServer(srv);
					}
				}
			}
		}
	}

	KVI_OPTION_BOOL(KviOption_boolShowFavoriteServersOnly) = m_pShowFavoritesOnlyButton->isChecked();

	KviOptionsWidget::commit();

	g_pApp->saveIrcServerDataBase(); // Ensure saving of the server DB
}

void OptionsWidget_servers::customContextMenuRequested(const QPoint & pnt)
{
	QTreeWidgetItem * it = static_cast<QTreeWidgetItem *>(m_pTreeWidget->itemAt(pnt));
	bool bServer = (it && static_cast<IrcServerOptionsTreeWidgetItem *>(it)->m_pServerData);
	bool bFavorite = (bServer && static_cast<IrcServerOptionsTreeWidgetItem *>(it)->m_pServerData->favorite());
	m_pContextPopup->clear();
	m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::NewNetwork)), __tr2qs_ctx("New Network", "options"), this, SLOT(newNetwork()));
	m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Remove)), __tr2qs_ctx("Remove Network", "options"), this, SLOT(removeCurrent()))
	    ->setEnabled(!bServer);

	m_pContextPopup->addSeparator();

	m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::NewServer)), __tr2qs_ctx("&New Server", "options"), this, SLOT(newServer()));
	m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::ServerFavorite)),
	    __tr2qs_ctx(bFavorite ? "Unfavorite Server" : "Favorite Server", "options"), this, SLOT(favoriteServer()));
	m_pContextPopup->setEnabled(bServer);
	m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Remove)), __tr2qs_ctx("Re&move Server", "options"), this, SLOT(removeCurrent()))
	    ->setEnabled(bServer);
	m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Copy)), __tr2qs_ctx("&Copy Server", "options"), this, SLOT(copyServer()));
	m_pContextPopup->setEnabled(bServer);
	m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Paste)), __tr2qs_ctx("&Paste Server", "options"), this, SLOT(pasteServer()))
	    ->setEnabled(m_pClipboard);

	m_pContextPopup->addSeparator();

	m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Folder)), __tr2qs_ctx("Import Server List", "options"))->setMenu(m_pImportPopup);
	m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Remove)), __tr2qs_ctx("Clear Server List", "options"), this, SLOT(clearList()));
	m_pContextPopup->popup(QCursor::pos());
}

void OptionsWidget_servers::importPopupAboutToShow()
{
	g_pModuleManager->loadModulesByCaps("serverimport");
	m_pImportPopup->clear();

	KviModuleExtensionDescriptorList * l = KviModuleExtensionManager::instance()->getExtensionList("serverimport");

	if(!l)
		return;

	QAction * pAction;

	for(KviModuleExtensionDescriptor * d = l->first(); d; d = l->next())
	{
		if(d->icon())
			pAction = m_pImportPopup->addAction(*(d->icon()), d->visibleName());
		else
			pAction = m_pImportPopup->addAction(d->visibleName());
		pAction->setData(d->id());
	}
}

void OptionsWidget_servers::importPopupActivated(QAction * pAction)
{
	// ensure that we have all the modules : they could have been unloaded while the popup was displayed
	g_pModuleManager->loadModulesByCaps("serverimport");

	KviModuleExtensionDescriptorList * l = KviModuleExtensionManager::instance()->getExtensionList("serverimport");

	if(!l)
	{
		// Oops! Internal error: I thought to have a module capable of importing servers
		// but actually it's not the case.. something weird happened (in the best case
		// the user has just unloaded the module and removed it from disk?)
		KviMessageBox::warning(__tr2qs_ctx("Oops! Something weird happened:<br>Can't find any module capable of importing servers.", "options"));
		return;
	}

	if(m_pImportFilter)
	{
		disconnect(m_pImportFilter, nullptr, this, nullptr);
		m_pImportFilter->die();
		m_pImportFilter = nullptr;
	}

	bool bOk = false;
	int id = pAction->data().toInt(&bOk);
	if(!bOk)
		return;

	m_pImportFilter = (KviMexServerImport *)KviModuleExtensionManager::instance()->allocateExtension("serverimport", id, nullptr);

	if(!m_pImportFilter)
	{
		KviMessageBox::warning(__tr2qs_ctx("Oops! Something weird happened:<br>Can't find the module that was capable of this import action. :(", "options"));
		return;
	}

	connect(m_pImportFilter, SIGNAL(server(const KviIrcServer &, const QString &)), this, SLOT(importServer(const KviIrcServer &, const QString &)));
	connect(m_pImportFilter, SIGNAL(destroyed()), this, SLOT(importerDead()));

	m_pImportFilter->start();
}

void OptionsWidget_servers::importerDead()
{
	m_pImportFilter = nullptr;
}

void OptionsWidget_servers::importServer(const KviIrcServer & s, const QString & network)
{
	IrcServerOptionsTreeWidgetItem * net = findNetItem(network);

	if(!net)
	{
		KviIrcNetwork d(network);
		net = new IrcServerOptionsTreeWidgetItem(m_pTreeWidget, *(g_pIconManager->getSmallIcon(KviIconManager::World)), &d);
		net->setExpanded(true);
	}

	IrcServerOptionsTreeWidgetItem * srv;
	for(int i = 0; i < net->childCount(); i++)
	{
		srv = (IrcServerOptionsTreeWidgetItem *)net->child(i);
		if((srv)->m_pServerData->useSSL() == s.useSSL())
		{
			if(srv->m_pServerData->isIPv6() == s.isIPv6())
			{
				if(KviQString::equalCI(srv->m_pServerData->hostName(), s.hostName()))
				{
					// update the port
					srv->m_pServerData->setPort(s.port());
					if(!s.ip().isEmpty())
						srv->m_pServerData->setIp(s.ip());
					if(!s.password().isEmpty())
						srv->m_pServerData->setPassword(s.password());
					if(!s.nickName().isEmpty())
						srv->m_pServerData->setNickName(s.nickName());
					m_pTreeWidget->setCurrentItem(srv);
					m_pTreeWidget->scrollToItem(srv);
					return;
				}
			}
		}
	}

	// not found : add it!
	IrcServerOptionsTreeWidgetItem * newServer = new IrcServerOptionsTreeWidgetItem(net, *(g_pIconManager->getSmallIcon(KviIconManager::Server)), &s);
	m_pTreeWidget->setCurrentItem(newServer);
	m_pTreeWidget->scrollToItem(newServer);
}

void OptionsWidget_servers::newNetwork()
{
	KviIrcNetwork d(__tr2qs_ctx("New Network", "options"));
	IrcServerOptionsTreeWidgetItem * it = new IrcServerOptionsTreeWidgetItem(m_pTreeWidget, *(g_pIconManager->getSmallIcon(KviIconManager::World)), &d);
	it->setExpanded(true);
	it->setSelected(true);
	m_pTreeWidget->setCurrentItem(it);
	m_pTreeWidget->scrollToItem(it);
}

void OptionsWidget_servers::newServer()
{
	if(m_pLastEditedItem)
	{
		IrcServerOptionsTreeWidgetItem * net;

		if(m_pLastEditedItem->m_pServerData)
		{
			net = (IrcServerOptionsTreeWidgetItem *)m_pLastEditedItem->parent();
			if(!net)
				return;
		}
		else
			net = m_pLastEditedItem;

		KviIrcServer tmp;
		tmp.setHostName(__tr2qs_ctx("irc.unknown.net", "options"));
		tmp.setCacheIp(false);
		tmp.generateUniqueId();

		IrcServerOptionsTreeWidgetItem * it = new IrcServerOptionsTreeWidgetItem(net, *(g_pIconManager->getSmallIcon(KviIconManager::Server)), &tmp);

		net->setExpanded(true);

		it->setSelected(true);
		m_pTreeWidget->setCurrentItem(it);
		m_pTreeWidget->scrollToItem(it);
	}
}

void OptionsWidget_servers::favoriteServer()
{
	if(!m_pLastEditedItem)
		return;

	if(m_pLastEditedItem->m_pServerData->favorite())
		m_pLastEditedItem->m_pServerData->setFavorite(false);
	else
		m_pLastEditedItem->m_pServerData->setFavorite(true);

	unsigned icon = m_pLastEditedItem->m_pServerData->favorite() ? KviIconManager::ServerFavorite : KviIconManager::Server;
	m_pLastEditedItem->setIcon(0, *(g_pIconManager->getSmallIcon(icon)));

	if(m_bShowingFavoritesOnly)
		updateFavoritesFilter(true);
}

void OptionsWidget_servers::updateFavoritesFilter(bool bSet)
{
	m_bShowingFavoritesOnly = bSet;
	IrcServerOptionsTreeWidgetItem * network;
	for(int i = 0; i < m_pTreeWidget->topLevelItemCount(); i++)
	{
		network = static_cast<IrcServerOptionsTreeWidgetItem *>(m_pTreeWidget->topLevelItem(i));
		uint uServers = 0;

		IrcServerOptionsTreeWidgetItem * ch;
		for(int j = 0; j < network->childCount(); j++)
		{
			bool bHidden = bSet ? true : false;
			ch = static_cast<IrcServerOptionsTreeWidgetItem *>(network->child(j));
			if(ch->m_pServerData && bSet)
			{
				if(ch->m_pServerData->favorite())
					bHidden = false;
			}
			if(!bHidden)
				uServers++;
			ch->setHidden(bHidden);
		}
		network->setHidden(!uServers);
	}

	m_pShowFavoritesOnlyButton->setIcon(*(g_pIconManager->getSmallIcon(bSet ? KviIconManager::Favorite : KviIconManager::FavoriteOff)));
}

void OptionsWidget_servers::copyServer()
{
	if(m_pLastEditedItem)
	{
		if(m_pLastEditedItem->m_pServerData)
		{
			if(!m_pClipboard)
				m_pClipboard = new KviIrcServer();
			*m_pClipboard = *(m_pLastEditedItem->m_pServerData);
			m_pPasteServerButton->setEnabled(true);
		}
	}
}

void OptionsWidget_servers::pasteServer()
{
	if(m_pClipboard)
	{
		if(m_pLastEditedItem)
		{
			IrcServerOptionsTreeWidgetItem * net;
			if(m_pLastEditedItem->m_pServerData)
			{
				net = (IrcServerOptionsTreeWidgetItem *)m_pLastEditedItem->parent();
				if(!net)
					return;
			}
			else
				net = m_pLastEditedItem;

			IrcServerOptionsTreeWidgetItem * it = new IrcServerOptionsTreeWidgetItem(net, *(g_pIconManager->getSmallIcon(KviIconManager::Server)), m_pClipboard);

			it->m_pServerData->generateUniqueId(); // FIXME: This isn't necessarily unique...

			net->setExpanded(true);

			it->setSelected(true);
			m_pTreeWidget->setCurrentItem(it);
			m_pTreeWidget->scrollToItem(it);
		}
	}
}

void OptionsWidget_servers::removeCurrent()
{
	if(m_pLastEditedItem)
	{
		QTreeWidgetItem * it = m_pTreeWidget->itemAbove(m_pLastEditedItem);

		int index = m_pTreeWidget->indexOfTopLevelItem(m_pLastEditedItem);
		if(index > -1)
		{
			// top level item
			m_pTreeWidget->takeTopLevelItem(m_pTreeWidget->indexOfTopLevelItem(m_pLastEditedItem));
		}
		else
		{
			QTreeWidgetItem * tmp = m_pLastEditedItem->parent();
			index = tmp->indexOfChild(m_pLastEditedItem);
			if(index > -1)
				tmp->takeChild(index);
		}

		if(!it)
			it = m_pTreeWidget->topLevelItem(0);
		if(it)
		{
			it->setSelected(true);
			m_pTreeWidget->setCurrentItem(it);
			m_pTreeWidget->scrollToItem(it);
		}
	}
}

IrcServerOptionsTreeWidgetItem * OptionsWidget_servers::findNetItem(const QString & netname)
{

	IrcServerOptionsTreeWidgetItem * it;
	for(int i = 0; i < m_pTreeWidget->topLevelItemCount(); i++)
	{
		it = (IrcServerOptionsTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		if(KviQString::equalCI(it->text(0), netname))
			return it;
	}
	return nullptr;
}

void OptionsWidget_servers::clearList()
{
	QString txt = __tr2qs_ctx("If you click <b>Yes</b>, all of your saved networks, servers, settings, and passwords will be lost.<br>"
	                          "Would you like to continue?", "options");

	if(QMessageBox::question(this,__tr2qs_ctx("Confirm Clearing Server List - KVIrc", "options"), txt,
	                              __tr2qs_ctx("Yes", "options"), __tr2qs_ctx("No", "options"), nullptr, 1) != 0) return;

	m_pTreeWidget->clear();
	m_pLastEditedItem = nullptr;
	currentItemChanged(nullptr, nullptr);
}

void OptionsWidget_servers::detailsClicked()
{
	if(!m_pLastEditedItem)
		return;
	if(m_pLastEditedItem->m_pServerData)
	{
		saveLastItem();

		m_pServerDetailsDialog = new IrcServerDetailsWidget(this, m_pLastEditedItem->m_pServerData);

		int retCode = m_pServerDetailsDialog->exec();
		if(retCode == QDialog::Accepted)
		{
			if(m_pLastEditedItem)
			{
				if(m_pLastEditedItem->m_pServerData)
				{
					m_pServerDetailsDialog->fillData(m_pLastEditedItem->m_pServerData);
					saveLastItem();
				}
			}
		}

		delete m_pServerDetailsDialog;
		m_pServerDetailsDialog = nullptr;
		return;
	}
	if(m_pLastEditedItem->m_pNetworkData)
	{
		saveLastItem();

		m_pNetworkDetailsDialog = new IrcNetworkDetailsWidget(this, m_pLastEditedItem->m_pNetworkData);

		int retCode = m_pNetworkDetailsDialog->exec();
		if(retCode == QDialog::Accepted)
		{
			if(m_pLastEditedItem)
			{
				if(m_pLastEditedItem->m_pNetworkData)
				{
					m_pNetworkDetailsDialog->fillData(m_pLastEditedItem->m_pNetworkData);
					saveLastItem();
				}
			}
		}

		delete m_pNetworkDetailsDialog;
		m_pNetworkDetailsDialog = nullptr;
	}
}
