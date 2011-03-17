//=============================================================================
//
//   File : OptionsWidget_servers.cpp
//   Creation date : Wed Jun 12 2000 03:29:51 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#include "KviTalPopupMenu.h"
#include "KviTalToolTip.h"

#include <QLineEdit>
#include <QCursor>
#include <QValidator>
#include <QMessageBox>
#include <QComboBox>
#include <QButtonGroup>
#include <QInputDialog>
#include <QHeaderView>
#include <QIcon>

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW) || defined(COMPILE_ON_MAC)
	#include <QStyleFactory>
	#include <QWindowsStyle>
#endif //COMPILE_ON_WINDOWS || COMPILE_ON_MINGW

IrcNetworkDetailsWidget::IrcNetworkDetailsWidget(QWidget * par,KviIrcNetwork * n)
	: QDialog(par)
{
	setModal(true);
	setObjectName("network_details");

	m_pOnConnectEditor = NULL;
	m_pOnLoginEditor = NULL;

	m_pUserEditor = NULL;
	m_pPassEditor = NULL;
	m_pNickEditor = NULL;
	m_pAlternativeNickEditor = NULL;
	m_pRealEditor = NULL;
	m_pDescEditor = NULL;

	m_pEncodingEditor = NULL;
	m_pTextEncodingEditor = NULL;

	m_pAutoConnectCheck = NULL;

	m_pNickServTreeWidget = NULL;
	m_pNickServCheck = NULL;
	m_pAddRuleButton = NULL;
	m_pDelRuleButton = NULL;
	m_pEditRuleButton = NULL;

	m_pChannelListSelector = NULL;

	QGridLayout * g = new QGridLayout(this);

	setWindowTitle(__tr2qs_ctx("Network Details","options"));
	setWindowIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::World))));

	QString szTmp = "<font size=\"+1\"><b>";
	szTmp += n->name();
	szTmp += "</b></font>";

	QLabel * l = new QLabel(szTmp,this);
	l->setFrameStyle(QFrame::Raised | QFrame::StyledPanel);
	l->setAlignment(Qt::AlignCenter);
	g->addWidget(l,0,0,1,4);

	l->setMinimumWidth(200);

	l = new QLabel(__tr2qs_ctx("Description:","options"),this);
	g->addWidget(l,1,0);
	m_pDescEditor = new QLineEdit(this);
	g->addWidget(m_pDescEditor,1,1,1,3);

	m_pDescEditor->setText(n->description());
	KviTalToolTip::add(m_pDescEditor,__tr2qs_ctx("<center>Put here a brief description of the network.</center>","options"));

	// Identity tab

	QTabWidget * tw = new QTabWidget(this);
	g->addWidget(tw,2,0,1,4);

	QWidget * tab = new QWidget(tw);
	QGridLayout * gl = new QGridLayout(tab);

	KviTalGroupBox *gbox = new KviTalGroupBox(Qt::Horizontal,__tr2qs_ctx("Properties","options"),tab);
	gl->addWidget(gbox,0,0,1,2);

	QGridLayout * pPropertiesBoxLayout = new QGridLayout();
	gbox->setLayout(pPropertiesBoxLayout);

	l = new QLabel(__tr2qs_ctx("Username:","options"),gbox);
	pPropertiesBoxLayout->addWidget(l,0,0);

	m_pUserEditor = new QLineEdit(gbox);
	m_pUserEditor->setText(n->userName());
	pPropertiesBoxLayout->addWidget(m_pUserEditor,0,1);

	KviTalToolTip::add(m_pUserEditor,__tr2qs_ctx("<center>This is the <b>username</b> that KVIrc will use to login to servers on this network.\n" \
		"If this field is left empty (most common case), KVIrc will use the default username " \
		"specified in the \"Identity\" options tab.</center>","options"));

	l = new QLabel(__tr2qs_ctx("Password:","options"),gbox);
	pPropertiesBoxLayout->addWidget(l,1,0);

	m_pPassEditor = new KviPasswordLineEdit(gbox);
	m_pPassEditor->setText(n->password());
	pPropertiesBoxLayout->addWidget(m_pPassEditor,1,1);

	KviTalToolTip::add(m_pPassEditor,__tr2qs_ctx("<center>If this network requires a password, put it in this field, otherwise leave it empty.</center>","options"));

	l = new QLabel(__tr2qs_ctx("Nickname:","options"),gbox);
	pPropertiesBoxLayout->addWidget(l,2,0);

	m_pNickEditor = new QLineEdit(gbox);
	QValidator * v = new QRegExpValidator(QRegExp("[^-0-9 ][^ ]*",Qt::CaseSensitive),gbox);
	m_pNickEditor->setValidator(v);
	m_pNickEditor->setText(n->nickName());
	pPropertiesBoxLayout->addWidget(m_pNickEditor,2,1);


	KviTalToolTip::add(m_pNickEditor,__tr2qs_ctx("<center>You can specify a \"special\" <b>nickname</b> that will be used to log in to the servers on this network.<br>" \
		"If this field is left empty (most common case), the default nickname (specified in the \"Identity\" settings) will be used.</center>","options"));

	l = new QLabel(__tr2qs_ctx("Alt.Nickname:","options"),gbox);
	pPropertiesBoxLayout->addWidget(l,3,0);

	m_pAlternativeNickEditor = new QLineEdit(gbox);
	v = new QRegExpValidator(QRegExp("[^-0-9 ][^ ]*",Qt::CaseSensitive),gbox);
	m_pAlternativeNickEditor->setValidator(v);
	m_pAlternativeNickEditor->setText(n->alternativeNickName());
	pPropertiesBoxLayout->addWidget(m_pAlternativeNickEditor,3,1);

	KviTalToolTip::add(m_pAlternativeNickEditor,__tr2qs_ctx("<center>This nickname will be tried if the primary nickname for this network is already in use.<br>" \
			"If you leave it empty then the default nicknames specified in the identity options will be tried instead.</center>","options"));


	l = new QLabel(__tr2qs_ctx("Real name:","options"),gbox);
	pPropertiesBoxLayout->addWidget(l,4,0);

	m_pRealEditor = new QLineEdit(gbox);
	m_pRealEditor->setText(n->realName());
	pPropertiesBoxLayout->addWidget(m_pRealEditor,4,1);

	KviTalToolTip::add(m_pRealEditor,__tr2qs_ctx("<center>You can specify a \"special\" <b>real name</b> that will be used to login with the servers on this network.<br>" \
		"If you leave this field empty (most common case), the default \"real name\" (specified in the \"Identity\" settings) will be used.</center>","options"));

	//server encoding

	l = new QLabel(__tr2qs_ctx("Server Encoding:","options"),tab);
	gl->addWidget(l,1,0);
	m_pEncodingEditor = new QComboBox(tab);
	m_pEncodingEditor->setDuplicatesEnabled(false);
	gl->addWidget(m_pEncodingEditor,1,1);
	KviTalToolTip::add(m_pEncodingEditor,__tr2qs_ctx("<center>This box allows you to choose the preferred encoding for the servers in this network. " \
		"This encoding will be used for server specific needs, like referencing nicknames and channel names." \
		"If you choose \"Use System Encoding\" then the encoding will be set to the systemwide " \
		"value that you choose in the \"Encoding\" page of the options dialog.</center>","options"));

	//text encoding

	l = new QLabel(__tr2qs_ctx("Text Encoding:","options"),tab);
	gl->addWidget(l,2,0);
	m_pTextEncodingEditor = new QComboBox(tab);
	m_pTextEncodingEditor->setDuplicatesEnabled(false);
	gl->addWidget(m_pTextEncodingEditor,2,1);
	KviTalToolTip::add(m_pTextEncodingEditor,__tr2qs_ctx("<center>This box allows you to choose the preferred encoding for the servers in this network. " \
		"This encoding will be used as the default for text messages." \
		"If you choose \"Use System Encoding\" then the encoding will be set to the systemwide " \
		"value that you choose in the \"Encoding\" page of the options dialog.</center>","options"));

	//common between server encoding and text encoding

	int i = 0;
	int srvcurrent = 0, txtcurrent=0;
	KviLocale::EncodingDescription * d = KviLocale::encodingDescription(i);
	QString tmp;
	m_pEncodingEditor->addItem(__tr2qs_ctx("Use System Encoding","options"));
	m_pTextEncodingEditor->addItem(__tr2qs_ctx("Use System Encoding","options"));
	while(d->szName)
	{
		tmp = QString("%1 (%2)").arg(d->szName,d->szDescription);
		m_pEncodingEditor->insertItem(m_pEncodingEditor->count(),tmp);
		m_pTextEncodingEditor->insertItem(m_pTextEncodingEditor->count(),tmp);
		if(KviQString::equalCI(d->szName,n->encoding()))srvcurrent = i + 1;
		if(KviQString::equalCI(d->szName,n->textEncoding()))txtcurrent = i + 1;
		i = i + 1;
		d = KviLocale::encodingDescription(i);
	}

	m_pEncodingEditor->setCurrentIndex(srvcurrent);
	m_pTextEncodingEditor->setCurrentIndex(txtcurrent);


	m_pAutoConnectCheck = new QCheckBox(__tr2qs_ctx("Connect to this network at startup","options"),tab);
	m_pAutoConnectCheck->setChecked(n->autoConnect());
	gl->addWidget(m_pAutoConnectCheck,3,0,1,2);

	KviTalToolTip::add(m_pAutoConnectCheck,__tr2qs_ctx("<center>This option will cause KVIrc to automatically connect to this network at startup</center>","options"));


	l = new QLabel("",tab);
	gl->addWidget(l,4,0);

	gl->setRowStretch(4,1);
	gl->setColumnStretch(1,1);

	tw->addTab(tab,__tr2qs_ctx("General","options"));

	// after login join channels
	tab = new QWidget(tw);

	gl = new QGridLayout(tab);

	if(n->autoJoinChannelList())
		m_lstChannels = *(n->autoJoinChannelList());
	m_pChannelListSelector = new KviChannelListSelector(tab,
		__tr2qs_ctx("Channels to join automatically upon connect:","options"),&m_lstChannels,true);

	KviTalToolTip::add(m_pChannelListSelector,__tr2qs_ctx("<center>Here you can set a list of channels to be joined automatically " \
		"after a connection to a server in this network has been established. To add a channel, type its name in the " \
		"text input below and click \"<b>Add</b>\".</center>","options"));

	gl->addWidget(m_pChannelListSelector,0,0);

	tw->addTab(tab,__tr2qs_ctx("Join Channels","options"));

	// after connect execute
	tab = new QWidget(tw);
	gl = new QGridLayout(tab);

	m_pOnConnectEditor = KviScriptEditor::createInstance(tab);
	gl->addWidget(m_pOnConnectEditor,0,0);
	m_pOnConnectEditor->setText(n->onConnectCommand());
	m_pOnConnectEditor->setMinimumHeight(150);
	KviTalToolTip::add(m_pOnConnectEditor,__tr2qs_ctx("<center>The following commands will be executed after a connection to a server in this network has been established.<br>" \
		"<b>Important:</b> Enter commands <b>without</b> a preceding slash (e.g. <tt>quote pass secret</tt> instead of <tt>/quote pass secret</tt>).<br>"\
		"KVIrc will first send the USER command, then eventually PASS and NICK and then execute this " \
		"command sequence.</center>","options"));

	tw->addTab(tab,__tr2qs_ctx("On Connect","options"));


	// after login execute
	tab = new QWidget(tw);
	gl = new QGridLayout(tab);

	m_pOnLoginEditor = KviScriptEditor::createInstance(tab);
	gl->addWidget(m_pOnLoginEditor,0,0);
	m_pOnLoginEditor->setText(n->onLoginCommand());

	m_pOnLoginEditor->setMinimumHeight(150);

	KviTalToolTip::add(m_pOnLoginEditor,
			__tr2qs_ctx("<center>The following commands will be executed after a successful login to a server in this network.<br>" \
				"<b>Important:</b> Enter commands <b>without</b> a preceding slash (e.g. <tt>quote privatelog</tt> instead of <tt>/quote privatelog</tt>).<br>"
				"This is useful for automatically opening queries, setting variables, etc.</center>","options"));

	tw->addTab(tab,__tr2qs_ctx("On Login","options"));


	// nick serv rules

	tab = new QWidget(tw);
	gl = new QGridLayout(tab);

	KviNickServRuleSet * rs = n->nickServRuleSet();
	bool bNickServEnabled = rs ? (rs->isEnabled() && !rs->isEmpty()) : false;

	m_pNickServCheck = new QCheckBox(__tr2qs_ctx("Enable NickServ Identification","options"),tab);
	gl->addWidget(m_pNickServCheck,0,0,1,3);

	KviTalToolTip::add(m_pNickServCheck,
				__tr2qs_ctx("This check enables the automatic identification with NickServ","options"));
	m_pNickServCheck->setChecked(bNickServEnabled);

	m_pNickServTreeWidget = new QTreeWidget(tab);
	m_pNickServTreeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pNickServTreeWidget->setAllColumnsShowFocus(true);
	m_pNickServTreeWidget->setColumnCount(4);
	QStringList columnLabels;
	columnLabels.append(__tr2qs_ctx("Nickname","options"));
	columnLabels.append(__tr2qs_ctx("NickServ Mask","options"));
	columnLabels.append(__tr2qs_ctx("NickServ Request Mask","options"));
	columnLabels.append(__tr2qs_ctx("Identify Command","options"));
	m_pNickServTreeWidget->setHeaderLabels(columnLabels);
	connect(m_pNickServTreeWidget,SIGNAL(itemSelectionChanged()),this,SLOT(enableDisableNickServControls()));
	gl->addWidget(m_pNickServTreeWidget,1,0,1,3);

	KviTalToolTip::add(m_pNickServTreeWidget,
		__tr2qs_ctx("<center>This is a list of NickServ identification rules. " \
				"KVIrc will use them to model its automatic interaction with NickServ on this network.<br>" \
				"Please be aware that this feature can cause your NickServ passwords to be stolen " \
				"if used improperly. Make sure that you fully understand the NickServ authentication protocol.<br>" \
				"In other words, be sure to know what you're doing.<br>" \
				"Also note that the password that you provide is stored as <b>PLAIN TEXT</b>.</center>","options"));

	m_pAddRuleButton = new QPushButton(__tr2qs_ctx("Add Rule","options"),tab);
	connect(m_pAddRuleButton,SIGNAL(clicked()),this,SLOT(addNickServRule()));
	gl->addWidget(m_pAddRuleButton,2,0);


	m_pEditRuleButton = new QPushButton(__tr2qs_ctx("Edit Rule","options"),tab);
	connect(m_pEditRuleButton,SIGNAL(clicked()),this,SLOT(editNickServRule()));
	gl->addWidget(m_pEditRuleButton,2,1);

	m_pDelRuleButton = new QPushButton(__tr2qs_ctx("Delete Rule","options"),tab);
	connect(m_pDelRuleButton,SIGNAL(clicked()),this,SLOT(delNickServRule()));
	gl->addWidget(m_pDelRuleButton,2,2);

	connect(m_pNickServCheck,SIGNAL(toggled(bool)),this,SLOT(enableDisableNickServControls()));

	if(rs && rs->rules())
	{
		KviPointerList<KviNickServRule> * ll = rs->rules();
		for(KviNickServRule * rule = ll->first();rule;rule = ll->next())
		{
			QTreeWidgetItem* it = new QTreeWidgetItem(m_pNickServTreeWidget);
			it->setText(0,rule->registeredNick());
			it->setText(1,rule->nickServMask());
			it->setText(2,rule->messageRegexp());
			it->setText(3,rule->identifyCommand());
		}
	}

	enableDisableNickServControls();

	gl->setRowStretch(1,1);

	tw->addTab(tab,__tr2qs_ctx("NickServ","options"));

	QPushButton * b = new QPushButton(__tr2qs_ctx("&OK","options"),this);
	b->setMinimumWidth(80);
	g->addWidget(b,3,2);
	b->setDefault(true);
	connect(b,SIGNAL(clicked()),this,SLOT(accept()));

	b = new QPushButton(__tr2qs_ctx("Cancel","options"),this);
	g->addWidget(b,3,3);
	b->setMinimumWidth(80);
	connect(b,SIGNAL(clicked()),this,SLOT(reject()));

	g->setRowStretch(2,1);
	g->setColumnStretch(1,1);

	tw->setMinimumWidth(375);
}


IrcNetworkDetailsWidget::~IrcNetworkDetailsWidget()
{
	if(m_pOnConnectEditor) KviScriptEditor::destroyInstance(m_pOnConnectEditor);
	if(m_pOnLoginEditor) KviScriptEditor::destroyInstance(m_pOnLoginEditor);
}

void IrcNetworkDetailsWidget::editNickServRule()
{
	QTreeWidgetItem * it = (QTreeWidgetItem *) m_pNickServTreeWidget->currentItem();
	if(!it)return;
	KviNickServRule r(it->text(0),it->text(1),it->text(2),it->text(3));
	NickServRuleEditor ed(this,false);
	if(ed.editRule(&r))
	{
		it->setText(0,r.registeredNick());
		it->setText(1,r.nickServMask());
		it->setText(2,r.messageRegexp());
		it->setText(3,r.identifyCommand());
	}
}

void IrcNetworkDetailsWidget::addNickServRule()
{
	KviNickServRule r;
	NickServRuleEditor ed(this,false);
	if(ed.editRule(&r))
	{
		QTreeWidgetItem *it = new QTreeWidgetItem(m_pNickServTreeWidget);
		it->setText(0,r.registeredNick());
		it->setText(1,r.nickServMask());
		it->setText(2,r.messageRegexp());
		it->setText(3,r.identifyCommand());
	}
}

void IrcNetworkDetailsWidget::delNickServRule()
{
	QTreeWidgetItem * it = (QTreeWidgetItem *)m_pNickServTreeWidget->currentItem();
	if(!it)return;
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
		} else {
			KviLocale::EncodingDescription * d = KviLocale::encodingDescription(m_pEncodingEditor->currentIndex() - 1);
			n->setEncoding(d->szName);
		}
	}
	if(m_pTextEncodingEditor)
	{
		if(m_pTextEncodingEditor->currentIndex() <= 0)
		{
			n->setTextEncoding(QString());
		} else {
			KviLocale::EncodingDescription * dd = KviLocale::encodingDescription(m_pTextEncodingEditor->currentIndex() - 1);
			n->setTextEncoding(dd->szName);
		}
	}
	if(m_pChannelListSelector)
		m_pChannelListSelector->commit();
	if(m_lstChannels.isEmpty())n->setAutoJoinChannelList(0);
	else n->setAutoJoinChannelList(new QStringList(m_lstChannels));
	if(m_pNickServTreeWidget)
	{
		if(m_pNickServTreeWidget->topLevelItemCount() > 0)
		{
			KviNickServRuleSet * rs = KviNickServRuleSet::createInstance();
			rs->setEnabled(m_pNickServCheck->isChecked());
			QTreeWidgetItem * it;
			for (int i=0;i<m_pNickServTreeWidget->topLevelItemCount();i++)
			{
				it=(QTreeWidgetItem *) m_pNickServTreeWidget->topLevelItem(i);
				rs->addRule(KviNickServRule::createInstance(it->text(0),it->text(1),it->text(2),it->text(3)));
			}
			n->setNickServRuleSet(rs);
		} else n->setNickServRuleSet(0);
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


IrcServerDetailsWidget::IrcServerDetailsWidget(QWidget * par,KviIrcServer * s)
: QDialog(par)
{
	setModal(true);
	setObjectName("server_details");

	m_szHostname = s->hostName();
	QGridLayout * g = new QGridLayout(this);

	setWindowTitle(__tr2qs_ctx("Server Details","options"));
	setWindowIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Server))));

	m_pHeaderLabel = new QLabel("",this); // the text will be set later
	m_pHeaderLabel->setFrameStyle(QFrame::Raised | QFrame::StyledPanel);
	m_pHeaderLabel->setAlignment(Qt::AlignCenter);
	g->addWidget(m_pHeaderLabel,0,0,1,4);
	m_pHeaderLabel->setMinimumWidth(200);

	QLabel * l = new QLabel(__tr2qs_ctx("Description:","options"),this);
	g->addWidget(l,1,0);
	m_pDescEditor = new QLineEdit(this);
	g->addWidget(m_pDescEditor,1,1,1,3);

	m_pDescEditor->setText(s->description());
	KviTalToolTip::add(m_pDescEditor,__tr2qs_ctx("<center>This is a brief description of this server. " \
		"This field has no restrictions but many server lists use it to describe the server's physical location</center>","options"));

	// Identity tab

	QTabWidget * tw = new QTabWidget(this);
	g->addWidget(tw,2,0,1,4);

	QWidget * tab = new QWidget(tw);
	QGridLayout * gl = new QGridLayout(tab);

	KviTalGroupBox *gbox = new KviTalGroupBox(Qt::Horizontal,__tr2qs_ctx("Properties","options"),tab);
	gl->addWidget(gbox,0,0);

	QGridLayout * pPropertiesBoxLayout = new QGridLayout();
	gbox->setLayout(pPropertiesBoxLayout);

	l = new QLabel(__tr2qs_ctx("Username:","options"),gbox);
	pPropertiesBoxLayout->addWidget(l,0,0);

	m_pUserEditor = new QLineEdit(gbox);
	m_pUserEditor->setText(s->userName());
	pPropertiesBoxLayout->addWidget(m_pUserEditor,0,1);

	KviTalToolTip::add(m_pUserEditor,__tr2qs_ctx("<center>This is the <b>username</b> that KVIrc will use to login to this server.\n" \
		"If this field is left empty (most common case), KVIrc will first look if an username is specified " \
		"for the network that this server belongs to, and if that is empty then KVIrc will use the default username " \
		"specified in the \"Identity\" options tab.</center>","options"));

	l = new QLabel(__tr2qs_ctx("Password:","options"),gbox);
	pPropertiesBoxLayout->addWidget(l,1,0);

	m_pPassEditor = new KviPasswordLineEdit(gbox);
	m_pPassEditor->setText(s->password());
	pPropertiesBoxLayout->addWidget(m_pPassEditor,1,1);

	KviTalToolTip::add(m_pPassEditor,__tr2qs_ctx("<center>If this server requires a password, put it in this field, otherwise leave it empty.</center>","options"));

	l = new QLabel(__tr2qs_ctx("Nickname:","options"),gbox);
	pPropertiesBoxLayout->addWidget(l,2,0);

	m_pNickEditor = new QLineEdit(gbox);
	QValidator * v = new QRegExpValidator(QRegExp("[^-0-9 ][^ ]*"),gbox);
	m_pNickEditor->setValidator(v);
	m_pNickEditor->setText(s->nickName());
	pPropertiesBoxLayout->addWidget(m_pNickEditor,2,1);

	KviTalToolTip::add(m_pNickEditor,__tr2qs_ctx("<center>You can specify a \"special\" <b>nickname</b> that will be used to log in to this server.<br>" \
		"If this field is left empty (most common case), KVIrc will first look if a nickname is specified " \
		"for the network that this server belongs to, and if that is empty then the default nickname (specified in the \"Identity\" settings) will be used.</center>","options"));

	l = new QLabel(__tr2qs_ctx("Alt.Nickname:","options"),gbox);
	pPropertiesBoxLayout->addWidget(l,3,0);

	m_pAlternativeNickEditor = new QLineEdit(gbox);
	v = new QRegExpValidator(QRegExp("[^-0-9 ][^ ]*"),gbox);
	m_pAlternativeNickEditor->setValidator(v);
	m_pAlternativeNickEditor->setText(s->alternativeNickName());
	pPropertiesBoxLayout->addWidget(m_pAlternativeNickEditor,3,1);

	KviTalToolTip::add(m_pAlternativeNickEditor,__tr2qs_ctx("<center>This nickname will be tried if the primary nickname for this server is already in use.<br>" \
			"If you leave it empty then the default nicknames specified in the identity options will be tried instead.</center>","options"));


	l = new QLabel(__tr2qs_ctx("Real name:","options"),gbox);
	pPropertiesBoxLayout->addWidget(l,4,0);

	m_pRealEditor = new QLineEdit(gbox);
	m_pRealEditor->setText(s->realName());
	pPropertiesBoxLayout->addWidget(m_pRealEditor,4,1);

	KviTalToolTip::add(m_pRealEditor,__tr2qs_ctx("<center>You can specify a \"special\" <b>real name</b> that will be used to login with this server.<br>" \
		"If you leave this field empty (most common case), KVIrc will first look if a real name is specified " \
		"for the network that this server belongs to, and if that is empty the default \"real name\" (specified in the \"Identity\" settings) will be used.</center>","options"));



	gbox = new KviTalGroupBox(Qt::Horizontal,__tr2qs_ctx("User Mode","options"),tab);
	gl->addWidget(gbox,1,0);

	m_pUseDefaultInitUMode = new QCheckBox(__tr2qs_ctx("Use default user mode","options"),gbox);
	KviTalToolTip::add(m_pUseDefaultInitUMode,__tr2qs_ctx("<center>If this is enabled, the global initial <b>user mode</b> (configured from"\
		" the identity dialog) will be used. If disabled, you can configure an initial user mode for this server","options"));
	bool bHasUmode = !(s->initUMode().isEmpty());
	KviCString szDefUMode = KVI_OPTION_STRING(KviOption_stringDefaultUserMode);
	m_pUseDefaultInitUMode->setChecked(!bHasUmode);
	connect(m_pUseDefaultInitUMode,SIGNAL(toggled(bool)),this,SLOT(useDefaultInitUModeToggled(bool)));

	m_pIMode = new QCheckBox(__tr2qs_ctx("Invisible (+i)","options"),gbox);
	m_pIMode->setEnabled(bHasUmode);
	m_pIMode->setChecked(bHasUmode ? s->initUMode().contains('i',Qt::CaseInsensitive) : szDefUMode.contains('i',Qt::CaseInsensitive));

	m_pSMode = new QCheckBox(__tr2qs_ctx("Server notices (+s)","options"),gbox);
	m_pSMode->setEnabled(bHasUmode);
	m_pSMode->setChecked(bHasUmode ? s->initUMode().contains('s',Qt::CaseInsensitive) : szDefUMode.contains('s',Qt::CaseInsensitive));

	m_pWMode = new QCheckBox(__tr2qs_ctx("Wallops (+w)","options"),gbox);
	m_pWMode->setEnabled(bHasUmode);
	m_pWMode->setChecked(bHasUmode ? s->initUMode().contains('w',Qt::CaseInsensitive) : szDefUMode.contains('w',Qt::CaseInsensitive));

	l = new QLabel("",tab);
	gl->addWidget(l,2,0);

	gl->setRowStretch(2,1);

	tw->addTab(tab,*(g_pIconManager->getSmallIcon(KviIconManager::Who)),__tr2qs_ctx("Identity","options"));

	// Connection tab

	int iRow = 0;

	tab = new QWidget(tw);
	gl = new QGridLayout(tab);

	l = new QLabel(__tr2qs_ctx("Port:","options"),tab);
	gl->addWidget(l,iRow,0);


	m_pPortEditor = new QLineEdit(tab);
	gl->addWidget(m_pPortEditor,iRow,1);
	QString tmpz;
	tmpz.setNum(s->port());
	m_pPortEditor->setText(tmpz);
	KviTalToolTip::add(m_pPortEditor,__tr2qs_ctx("<center>This is the default <b>port</b> that this server will be contacted on.<br>Usually <b>6667</b> is OK.</center>","options"));
	connect(m_pPortEditor,SIGNAL(textChanged(const QString &)),this,SLOT(portEditorTextChanged(const QString &)));

	iRow++;

	l = new QLabel(__tr2qs_ctx("IP address:","options"),tab);
	gl->addWidget(l,iRow,0);
	m_pIpEditor = new KviIpEditor(tab,KviIpEditor::IPv4);
	gl->addWidget(m_pIpEditor,iRow,1);
	KviTalToolTip::add(m_pIpEditor,__tr2qs_ctx("<center>This is the <b>IP address</b> of this server, it is here for caching purposes.<br>" \
		"If you leave this field empty, KVIrc will fill it in the first time it connects to the server. " \
		"If you enable the \"cache IP address\" option below, KVIrc will use it as a \"cached result\" " \
		"and avoid looking it up again.</center>","options"));
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
	gl->addLayout(pCheckBoxLayout,iRow,0,1,2);

	m_pCacheIpCheck = new QCheckBox(__tr2qs_ctx("Cache IP address","options"),tab);
	pCheckBoxLayout->addWidget(m_pCacheIpCheck,0,0);

	KviTalToolTip::add(m_pCacheIpCheck,__tr2qs_ctx("<center>This check will enable <b>IP address caching</b> for this server:<br>" \
		"DNS lookups can be time-consuming and might be blocking on some platforms; " \
		"this option will cause KVIrc to look up the server hostname only once.<br><br> " \
		"Advanced: you can also use this option to force a certain server name to resolve " \
		"to a fixed ip address when either the dns for that server is temporairly " \
		"unreachable or you want to avoid the round-robin lookups.</center>","options"));
	m_pCacheIpCheck->setChecked(s->cacheIp());




	m_pUseAutoConnect = new QCheckBox(__tr2qs_ctx("Connect to this server at startup","options"),tab);
	m_pUseAutoConnect->setChecked(s->autoConnect());

	pCheckBoxLayout->addWidget(m_pUseAutoConnect,0,1);

	KviTalToolTip::add(m_pUseAutoConnect,__tr2qs_ctx("<center>This option will cause KVIrc to connect to the IRC server when it is started.</center>","options"));



	m_pUseIPV6Check = new QCheckBox(__tr2qs_ctx("Use IPv6 protocol","options"),tab);
	pCheckBoxLayout->addWidget(m_pUseIPV6Check,1,0);

#ifdef COMPILE_IPV6_SUPPORT
	m_pUseIPV6Check->setChecked(s->isIPv6());
#else
	m_pUseIPV6Check->setEnabled(false);
#endif
	connect(m_pUseIPV6Check,SIGNAL(toggled(bool)),this,SLOT(useIPV6CheckToggled(bool)));

	KviTalToolTip::add(m_pUseIPV6Check,__tr2qs_ctx("<center>This check identifies IPv6 servers.<br>If enabled, KVIrc will attempt to use the IPv6 protocol " \
		"(thus your OS <b>must</b> have a working IPv6 stack and you <b>must</b> have an IPv6 connection).</center>","options"));




	m_pUseSSLCheck = new QCheckBox(__tr2qs_ctx("Use SSL protocol","options"),tab);
	pCheckBoxLayout->addWidget(m_pUseSSLCheck,1,1);

	KviTalToolTip::add(m_pUseSSLCheck,__tr2qs_ctx("<center>This check will cause the connection to use the <b>Secure Socket Layer</b> " \
		"encryption support. Obviously, this server must have support for this, too. :)</center>","options"));
#ifndef COMPILE_SSL_SUPPORT
	m_pUseSSLCheck->setEnabled(false);
#endif
	m_pUseSSLCheck->setChecked(s->useSSL());


	iRow++;


	//server encoding

	l = new QLabel(__tr2qs_ctx("Protocol Encoding:","options"),tab);
	gl->addWidget(l,iRow,0);
	m_pEncodingEditor = new QComboBox(tab);
	m_pEncodingEditor->setDuplicatesEnabled(false);
	gl->addWidget(m_pEncodingEditor,iRow,1);
	KviTalToolTip::add(m_pEncodingEditor,__tr2qs_ctx("<center>This box allows you to choose the preferred encoding for this server. " \
		"This encoding will be used for server specific needs, like referencing nicknames and channel names." \
		"If you choose \"Use Network Encoding\" then the encoding will be inherited from the " \
		"network that this server belongs to.</center>","options"));

	iRow++;

	//text encoding

	l = new QLabel(__tr2qs_ctx("Text Encoding:","options"),tab);
	gl->addWidget(l,iRow,0);
	m_pTextEncodingEditor = new QComboBox(tab);
	m_pTextEncodingEditor->setDuplicatesEnabled(false);
	gl->addWidget(m_pTextEncodingEditor,iRow,1);
	KviTalToolTip::add(m_pTextEncodingEditor,__tr2qs_ctx("<center>This box allows you to choose the preferred encoding for this server. " \
			"This encoding will be used as the default for text messages." \
			"If you choose \"Use Network Encoding\" then the encoding will be inherited from the " \
			"network that this server belongs to.</center>","options"));

	//common between server encoding and text encoding

	int i = 0;
	int srvcurrent = 0, txtcurrent=0;
	KviLocale::EncodingDescription * d = KviLocale::encodingDescription(i);
	QString tmp;
	m_pEncodingEditor->addItem(__tr2qs_ctx("Use Network Encoding","options"));
	m_pTextEncodingEditor->addItem(__tr2qs_ctx("Use Network Encoding","options"));
	while(d->szName)
	{
		tmp = QString("%1 (%2)").arg(d->szName,d->szDescription);
		m_pEncodingEditor->insertItem(m_pEncodingEditor->count(),tmp);
		m_pTextEncodingEditor->insertItem(m_pTextEncodingEditor->count(),tmp);
		if(KviQString::equalCI(d->szName,s->encoding()))srvcurrent = i + 1;
		if(KviQString::equalCI(d->szName,s->textEncoding()))txtcurrent = i + 1;
		i = i + 1;
		d = KviLocale::encodingDescription(i);
	}

	m_pEncodingEditor->setCurrentIndex(srvcurrent);
	m_pTextEncodingEditor->setCurrentIndex(txtcurrent);

	iRow++;

	l = new QLabel(__tr2qs_ctx("Proxy server:","options"),tab);
	gl->addWidget(l,iRow,0);
	m_pProxyEditor = new QComboBox(tab);
	gl->addWidget(m_pProxyEditor,iRow,1);
	KviTalToolTip::add(m_pProxyEditor,__tr2qs_ctx("<center>This is the <b>proxy</b> that KVIrc will use to connect to this server.\n" \
		"If this field is set in \"Default\" KVirc will use global proxy settings, if it is set in \"Direct connection\" " \
		"KVirc will connect to this server without proxy. You can define new proxy server in global options' \"Proxy servers\" menu.</center>","options"));

	m_pProxyEditor->addItem(__tr2qs_ctx("Default","options"));
	m_pProxyEditor->addItem(__tr2qs_ctx("Direct connection","options"));

	KviPointerList<KviProxy> * proxylist = g_pProxyDataBase->proxyList();
	for(KviProxy * p = proxylist->first();p;p = proxylist->next())
	{
		m_pProxyEditor->insertItem(m_pProxyEditor->count(),QString("%1:%2").arg(p->hostName()).arg(p->port()));
	}
	if(m_pProxyEditor->count() > (s->proxy()+2))
		m_pProxyEditor->setCurrentIndex(s->proxy()+2);

	iRow++;




	l = new QLabel("",tab);
	gl->addWidget(l,iRow,0,1,2);

	gl->setRowStretch(iRow,1);

	tw->addTab(tab,*(g_pIconManager->getSmallIcon(KviIconManager::SocketWarning)),__tr2qs_ctx("Connection","options"));

	// after login join channels
	tab = new QWidget(tw);

	gl = new QGridLayout(tab);

	if(s->autoJoinChannelList())
		m_lstChannels = *(s->autoJoinChannelList());
	m_pChannelListSelector = new KviChannelListSelector(tab,
		__tr2qs_ctx("Channels to join automatically upon connect:","options"),&m_lstChannels,true);
	KviTalToolTip::add(m_pChannelListSelector,__tr2qs_ctx("<center>Here you can set a list of channels to be joined automatically " \
		"after a connection to this server has been established. To add a channel, type its name in the " \
		"text input below and click \"<b>Add</b>\".</center>","options"));
	gl->addWidget(m_pChannelListSelector,0,0);

	tw->addTab(tab,*(g_pIconManager->getSmallIcon(KviIconManager::Channel)),__tr2qs_ctx("Join Channels","options"));

	// after connect execute
	tab = new QWidget(tw);
	gl = new QGridLayout(tab);


	m_pOnConnectEditor = KviScriptEditor::createInstance(tab);
	gl->addWidget(m_pOnConnectEditor,0,0);
	m_pOnConnectEditor->setText(s->onConnectCommand());
	m_pOnConnectEditor->setMinimumHeight(150);
	KviTalToolTip::add(m_pOnConnectEditor,__tr2qs_ctx("<center>The following commands will be executed after a connection has been established.<br>" \
		"<b>Important:</b> Enter commands <b>without</b> a preceding slash (e.g. <tt>quote pass secret</tt> instead of <tt>/quote pass secret</tt>).<br>"\
		"KVIrc will first send the USER command, then eventually PASS and NICK and then execute this " \
		"command sequence.<br>This is particularly useful for IRC bouncers that require login commands.</center>","options"));

	tw->addTab(tab,__tr2qs_ctx("On Connect","options"));
	// after login execute

	tab = new QWidget(tw);
	gl = new QGridLayout(tab
		);

	m_pOnLoginEditor = KviScriptEditor::createInstance(tab);
	gl->addWidget(m_pOnLoginEditor,0,0);
	m_pOnLoginEditor->setText(s->onLoginCommand());

	m_pOnLoginEditor->setMinimumHeight(150);

	KviTalToolTip::add(m_pOnLoginEditor,
		__tr2qs_ctx("<center>The following commands will be executed after a successful login to this server.<br>" \
			"<b>Important:</b> Enter commands <b>without</b> a preceding slash (e.g. <tt>quote privatelog</tt> instead of <tt>/quote privatelog</tt>).<br>"
			"This is useful for automatically opening queries, setting variables, etc.</center>","options"));

	tw->addTab(tab,__tr2qs_ctx("On Login","options"));



	// Advanced tab


	tab = new QWidget(tw);
	gl = new QGridLayout(tab);

	iRow = 0;

	QGroupBox * pCapGroup = new QGroupBox(__tr2qs_ctx("Extended Capabilities","options"),tab);
	gl->addWidget(pCapGroup,iRow,0,1,2);

	QGridLayout * pCapLayout = new QGridLayout();
	pCapGroup->setLayout(pCapLayout);

	m_pEnableCAPCheck = new QCheckBox(__tr2qs_ctx("Query extended capabilities on connect","options"),tab);
	pCapLayout->addWidget(m_pEnableCAPCheck,0,0);

	KviTalToolTip::add(m_pEnableCAPCheck,__tr2qs_ctx("<center>This check will cause the connection to use the <b>Extended Capability</b> " \
		"support. Obviously, this server must have support for this, too. Disable this for irc bouncers.</center>","options"));
	m_pEnableCAPCheck->setChecked(s->enabledCAP());



	m_pEnableSTARTTLSCheck = new QCheckBox(__tr2qs_ctx("Switch to SSL/TLS by using the STARTTLS extension","options"),tab);
	pCapLayout->addWidget(m_pEnableSTARTTLSCheck,1,0);

	m_pEnableSTARTTLSCheck->setEnabled(s->enabledCAP());
	QObject::connect(m_pEnableCAPCheck,SIGNAL(toggled(bool)),m_pEnableSTARTTLSCheck,SLOT(setEnabled(bool)));
	KviTalToolTip::add(m_pEnableSTARTTLSCheck,__tr2qs_ctx("<center>This check enables the use of the <b>Transport Layer Security</b> " \
		"protocol. If you enable the proper global option in the Connection/SSL tab, the TLS protocol will be used for this server if available.</center>","options"));
#ifndef COMPILE_SSL_SUPPORT
	m_pEnableSTARTTLSCheck->setEnabled(false);
#endif
	m_pEnableSTARTTLSCheck->setChecked(s->enabledCAP() && s->enabledSTARTTLS());


	QGroupBox * pSASLGroup = new QGroupBox(__tr2qs_ctx("SASL Authentication","options"),tab);
	pCapLayout->addWidget(pSASLGroup,2,0);

	QGridLayout * pSASLLayout = new QGridLayout(pSASLGroup);

	m_pEnableSASLCheck = new QCheckBox(__tr2qs_ctx("Authenticate via SASL extension","options"),pSASLGroup);
	pSASLLayout->addWidget(m_pEnableSASLCheck,0,0,1,2);
	KviTalToolTip::add(m_pEnableSASLCheck,__tr2qs_ctx("<center>This check enables the use of the <b>SASL</b> authentication procotol " \
		"If you enable the proper global option in the Connection/SSL tab and fill the Sasl Nickname and Sasl Password fields in this page, the SASL protocol will be used for this server if available.</center>","options"));
	m_pEnableSASLCheck->setChecked(s->enabledSASL());

	l = new QLabel(__tr2qs_ctx("Sasl Nickname:","options"),pSASLGroup);
	pSASLLayout->addWidget(l,1,0);
	m_pSaslNickEditor = new QLineEdit(pSASLGroup);
	m_pSaslNickEditor->setText(s->saslNick());
	KviTalToolTip::add(m_pSaslNickEditor,__tr2qs_ctx("<center>If you want to enable SASL authentication, insert your nickname here.</center>","options"));
	pSASLLayout->addWidget(m_pSaslNickEditor,1,1);

	l = new QLabel(__tr2qs_ctx("Sasl Password:","options"),pSASLGroup);
	pSASLLayout->addWidget(l,2,0);
	m_pSaslPassEditor = new KviPasswordLineEdit(pSASLGroup); // <---- ?????
	m_pSaslPassEditor->setText(s->saslPass());
	KviTalToolTip::add(m_pSaslPassEditor,__tr2qs_ctx("<center>If you want to enable SASL authentication, insert your password here.</center>","options"));
	pSASLLayout->addWidget(m_pSaslPassEditor,2,1);

	pSASLGroup->setEnabled(s->enabledCAP());
	QObject::connect(m_pEnableCAPCheck,SIGNAL(toggled(bool)),pSASLGroup,SLOT(setEnabled(bool)));

	iRow++;


	l = new QLabel(__tr2qs_ctx("Link filter:","options"),tab);
	gl->addWidget(l,iRow,0);
	m_pLinkFilterEditor = new QComboBox(tab);
	m_pLinkFilterEditor->setEditable(true);
	m_pLinkFilterEditor->setDuplicatesEnabled(false);
	gl->addWidget(m_pLinkFilterEditor,iRow,1);

	m_pLinkFilterEditor->addItem("");

	g_pModuleManager->loadModulesByCaps("linkfilter");
	KviModuleExtensionDescriptorList * mexl = KviModuleExtensionManager::instance()->getExtensionList("linkfilter");

	if(mexl)
	{
		for(KviModuleExtensionDescriptor * d = mexl->first();d;d = mexl->next())
			m_pLinkFilterEditor->addItem(d->name().ptr());
	} else {
		if(!s->linkFilter().isEmpty())
			m_pLinkFilterEditor->addItem(s->linkFilter());
	}

	if(!s->linkFilter().isEmpty())
	{
		int i = m_pLinkFilterEditor->findText(s->linkFilter());
		if (i != -1)
			m_pLinkFilterEditor->setCurrentIndex(i);
		else
			m_pLinkFilterEditor->setEditText(s->linkFilter());
	}
	else
		m_pLinkFilterEditor->setCurrentIndex(0);


	KviTalToolTip::add(m_pLinkFilterEditor,__tr2qs_ctx("<center>This field specifies the name of a module that exports a link filter for this type of server.<br>" \
		"For plain IRC connections, you don't need any link filters; this is used for incompatible protocols.</center>","options"));

	iRow++;

	l = new QLabel(__tr2qs_ctx("Id:","options"),tab);
	gl->addWidget(l,iRow,0);
	m_pIdEditor = new QLineEdit(tab);
	if(s->id().isEmpty())s->generateUniqueId();
	m_pIdEditor->setText(s->id());
	gl->addWidget(m_pIdEditor,iRow,1);

	KviTalToolTip::add(m_pIdEditor,__tr2qs_ctx("<center>This field allows you to specify a really unique id for this server. " \
		"You will then be able to use /server -x &lt;this_id&gt; to make the connection. This is especially " \
		"useful when you have multiple server entries with the same hostname and port in different networks (bouncers?)</center>","options"));


	iRow++;

	tw->addTab(tab,__tr2qs_ctx("Advanced","options"));



	QPushButton * b = new QPushButton(__tr2qs_ctx("OK","options"),this);
	b->setMinimumWidth(80);
	g->addWidget(b,3,2);
	b->setDefault(true);
	connect(b,SIGNAL(clicked()),this,SLOT(accept()));

	b = new QPushButton(__tr2qs_ctx("Cancel","options"),this);
	g->addWidget(b,3,3);
	b->setMinimumWidth(80);
	connect(b,SIGNAL(clicked()),this,SLOT(reject()));

	g->setRowStretch(2,1);
	g->setColumnStretch(1,1);


	tw->setMinimumWidth(390);

	setHeaderLabelText();
}

IrcServerDetailsWidget::~IrcServerDetailsWidget()
{
	if(m_pOnConnectEditor) KviScriptEditor::destroyInstance(m_pOnConnectEditor);
	if(m_pOnLoginEditor) KviScriptEditor::destroyInstance(m_pOnLoginEditor);
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

void IrcServerDetailsWidget::portEditorTextChanged(const QString &)
{
	setHeaderLabelText();
}

void IrcServerDetailsWidget::setHeaderLabelText()
{
	QString szTmp = "<font size=\"+1\"><b>irc";
	if(m_pUseIPV6Check->isChecked())szTmp += "6";
	szTmp += "://";
	szTmp += m_szHostname;
	szTmp += ":";

	bool bOk;

	QString num = m_pPortEditor->text();
	unsigned int uPort = num.toUInt(&bOk);
	if(!bOk)uPort = 6667;
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
		} else {
			KviLocale::EncodingDescription * d = KviLocale::encodingDescription(m_pEncodingEditor->currentIndex() - 1);
			s->setEncoding(d->szName);
		}
	}
	if(m_pTextEncodingEditor)
	{
		if(m_pTextEncodingEditor->currentIndex() <= 0)
		{
			s->setTextEncoding(QString());
		} else {
			KviLocale::EncodingDescription * dd = KviLocale::encodingDescription(m_pTextEncodingEditor->currentIndex() - 1);
			s->setTextEncoding(dd->szName);
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

	s->setSaslNick(m_pSaslNickEditor->text());
	s->setSaslPass(m_pSaslPassEditor->text());
	if(m_pEnableSASLCheck)
		s->setEnabledSASL( m_pEnableSASLCheck->isChecked() &&
				  !m_pSaslNickEditor->text().isEmpty() &&
				  !m_pSaslPassEditor->text().isEmpty()
				  );

	if(m_pIdEditor)
		s->setId(m_pIdEditor->text());
	if(s->id().isEmpty())s->generateUniqueId();

	if(m_pUseDefaultInitUMode)
	{
		KviCString szUMode;
		if(!m_pUseDefaultInitUMode->isChecked())
		{
			if(m_pIMode->isChecked())szUMode.append('i');
			if(m_pWMode->isChecked())szUMode.append('w');
			if(m_pSMode->isChecked())szUMode.append('s');
		}
		s->setInitUMode(szUMode.ptr());
	}

	QString tmp = m_pPortEditor->text();
	bool bOk;
	kvi_u32_t uPort = tmp.toUInt(&bOk);
	if(!bOk)uPort = 6667;
	s->setPort(uPort);
#ifdef COMPILE_IPV6_SUPPORT
	s->setIPv6(m_pUseIPV6Check->isChecked());
#else
	s->setIPv6(false);
#endif

	if(m_pIpEditor)
	{
		QString tmpAddr = m_pIpEditor->address();

		if(!m_pIpEditor->hasEmptyFields())
		{
#ifdef COMPILE_IPV6_SUPPORT
			if(s->isIPv6())
			{
				if((!KviQString::equalCI(tmpAddr,"0:0:0:0:0:0:0:0")) && KviNetUtils::isValidStringIp(tmpAddr))
				{
					s->setIp(tmpAddr);
				} else {
					s->setCacheIp(false);
					s->setIp("");
				}
			} else {
#endif
				if((!KviQString::equalCI(tmpAddr,"0.0.0.0")) && KviNetUtils::isValidStringIp(tmpAddr))
				{
					s->setIp(tmpAddr);
				} else {
					s->setCacheIp(false);
					s->setIp("");
				}
#ifdef COMPILE_IPV6_SUPPORT
			}
#endif
		} else {
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
	if(m_lstChannels.isEmpty())s->setAutoJoinChannelList(0);
	else s->setAutoJoinChannelList(new QStringList(m_lstChannels));
	if(m_pUseAutoConnect)
		s->setAutoConnect(m_pUseAutoConnect->isChecked());
	if(m_pProxyEditor)
		s->setProxy(m_pProxyEditor->currentIndex()-2);
}

void IrcServerDetailsWidget::useDefaultInitUModeToggled(bool b)
{
	m_pIMode->setEnabled(!b);
	m_pWMode->setEnabled(!b);
	m_pSMode->setEnabled(!b);
}

// KviApplication.cpp
extern KVIRC_API KviIrcServerDataBase * g_pServerDataBase;

IrcServerOptionsTreeWidgetItem::IrcServerOptionsTreeWidgetItem(QTreeWidget *parent,const QPixmap &pm,const KviIrcNetwork *n)
    : QTreeWidgetItem(parent)
{
	setIcon(0,QIcon(pm));
	m_pServerData = 0;
	m_pNetworkData = new KviIrcNetwork(*n);
	setText(0,n->name());
	setText(1,n->description());
}

IrcServerOptionsTreeWidgetItem::IrcServerOptionsTreeWidgetItem(QTreeWidgetItem *parent,const QPixmap &pm,const KviIrcServer *s)
    : QTreeWidgetItem(parent)
{
	setIcon(0,QIcon(pm));
	m_pServerData = new KviIrcServer(*s);
	setText(0,s->hostName());
	setText(1,s->description());
	m_pNetworkData = 0;
}

IrcServerOptionsTreeWidgetItem::~IrcServerOptionsTreeWidgetItem()
{
	if(m_pServerData)delete m_pServerData;
	if(m_pNetworkData)delete m_pNetworkData;
}

void IrcServerOptionsTreeWidgetItem::updateVisibleStrings()
{
	if(m_pNetworkData)
	{
		setText(0,m_pNetworkData->name());
		setText(1,m_pNetworkData->description());
		return;
	}
	if(m_pServerData)
	{
		setText(0,m_pServerData->hostName());
		setText(1,m_pServerData->description());
	}
}


OptionsWidget_servers::OptionsWidget_servers(QWidget * parent)
: KviOptionsWidget(parent)
{
	setObjectName("server_options_widget");

	createLayout();

	m_pContextPopup = new KviTalPopupMenu(this);
	m_pImportPopup = new KviTalPopupMenu(this);

	connect(m_pImportPopup,SIGNAL(aboutToShow()),this,SLOT(importPopupAboutToShow()));
	connect(m_pImportPopup,SIGNAL(activated(int)),this,SLOT(importPopupActivated(int)));

	m_pServerDetailsDialog = 0;
	m_pNetworkDetailsDialog = 0;
	m_pImportFilter = 0;

	m_pFilterLabel = new QLabel(__tr2qs_ctx("Filter:","options"),this);
	addWidgetToLayout(m_pFilterLabel,0,0,0,0);

	m_pFilterEdit = new QLineEdit(this);
	connect(m_pFilterEdit,SIGNAL(textEdited(const QString &)),this,SLOT(filterTextEdited(const QString &)));
	KviTalToolTip::add(m_pFilterEdit,__tr2qs_ctx("<center>If you are searching for a specific server or network, you can insert its name to filter the servers in the list</center>","options"));
	addWidgetToLayout(m_pFilterEdit,1,0,1,0);

	m_pTreeWidget = new QTreeWidget(this);
	addWidgetToLayout(m_pTreeWidget,0,1,1,1);
	m_pTreeWidget->setColumnCount(2);
	QStringList columLabels;
	columLabels.append(__tr2qs_ctx("Server","options"));
	columLabels.append(__tr2qs_ctx("Description","options"));
	m_pTreeWidget->setColumnWidth(0,250);
	m_pTreeWidget->setColumnWidth(1,250);

	m_pTreeWidget->setSortingEnabled(true);
        m_pTreeWidget->sortByColumn(0,Qt::AscendingOrder);
	m_pTreeWidget->setHeaderLabels(columLabels);
	m_pTreeWidget->setRootIsDecorated(true);
	m_pTreeWidget->setAllColumnsShowFocus(true);
	m_pTreeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_pTreeWidget,SIGNAL(customContextMenuRequested(const QPoint &)),
		this,SLOT(customContextMenuRequested(const QPoint &)));
	connect(m_pTreeWidget,SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)),
		this,SLOT(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)));

	connect(m_pTreeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int )),
		this,SLOT(itemDoubleClicked(QTreeWidgetItem*, int )));

	QString tiptxt = __tr2qs_ctx("<center>This is the list of available IRC servers.<br>" \
			"Right-click on the list to add or remove servers and perform other actions.<br>"\
			"Double-click on a item for advanced options.</center>","options");
	KviTalToolTip::add(m_pTreeWidget,tiptxt);
	KviTalToolTip::add(m_pTreeWidget->viewport(),tiptxt);

	KviTalVBox * vbox = new KviTalVBox(this);
	vbox->setSpacing(0);
	addWidgetToLayout(vbox,3,1,3,1);

	m_pNewNetworkButton = new QToolButton(vbox);
	m_pNewNetworkButton->setIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::World))));
	m_pNewNetworkButton->setAutoRaise(true);
	connect(m_pNewNetworkButton,SIGNAL(clicked()),this,SLOT(newNetwork()));
	KviTalToolTip::add(m_pNewNetworkButton,__tr2qs_ctx("New Network","options"));

	m_pNewServerButton = new QToolButton(vbox);
	m_pNewServerButton->setIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Server))));
	m_pNewServerButton->setAutoRaise(true);
	connect(m_pNewServerButton,SIGNAL(clicked()),this,SLOT(newServer()));
	KviTalToolTip::add(m_pNewServerButton,__tr2qs_ctx("New Server","options"));

	m_pRemoveButton = new QToolButton(vbox);
	m_pRemoveButton->setIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Cut))));
	m_pRemoveButton->setEnabled(false);
	m_pRemoveButton->setAutoRaise(true);
	connect(m_pRemoveButton,SIGNAL(clicked()),this,SLOT(removeCurrent()));
	KviTalToolTip::add(m_pRemoveButton,__tr2qs_ctx("Remove Network/Server","options"));

	QFrame * f = new QFrame(vbox);
	f->setFrameStyle(QFrame::Sunken | QFrame::HLine);

	m_pCopyServerButton = new QToolButton(vbox);
	m_pCopyServerButton->setIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Copy))));
	m_pCopyServerButton->setEnabled(false);
	m_pCopyServerButton->setAutoRaise(true);
	connect(m_pCopyServerButton,SIGNAL(clicked()),this,SLOT(copyServer()));
	KviTalToolTip::add(m_pCopyServerButton,__tr2qs_ctx("Copy Server","options"));

	m_pPasteServerButton = new QToolButton(vbox);
	m_pPasteServerButton->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Paste)));
	m_pPasteServerButton->setEnabled(false);
	m_pPasteServerButton->setAutoRaise(true);
	connect(m_pPasteServerButton,SIGNAL(clicked()),this,SLOT(pasteServer()));
	KviTalToolTip::add(m_pPasteServerButton,__tr2qs_ctx("Paste Server","options"));

	f = new QFrame(vbox);
	f->setFrameStyle(QFrame::Sunken | QFrame::HLine);

	m_pImportButton = new QToolButton(vbox);
	m_pImportButton->setIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Folder))));
	m_pImportButton->setAutoRaise(true);
	m_pImportButton->setMenu(m_pImportPopup);
	m_pImportButton->setPopupMode(QToolButton::InstantPopup);

	KviTalToolTip::add(m_pImportButton,__tr2qs_ctx("Import List","options"));

	QFrame * lll = new QFrame(vbox);
	vbox->setStretchFactor(lll,100);

	KviTalGroupBox *gbox = addGroupBox(0,2,3,2,Qt::Vertical,__tr2qs_ctx("Active Configuration","options"));
	m_pSrvNetLabel = new QLabel(__tr2qs_ctx("Server:","options"),gbox);

	m_pSrvNetEdit = new QLineEdit(gbox);
	connect(m_pSrvNetEdit,SIGNAL(textEdited(const QString &)),this,SLOT(serverNetworkEditTextEdited(const QString &)));
	KviTalToolTip::add(m_pSrvNetEdit,__tr2qs_ctx("<center>This is the name of the currently selected server or network</center>","options"));

	m_pDetailsButton = new QPushButton(__tr2qs_ctx("Advanced...","options"),gbox);
	connect(m_pDetailsButton,SIGNAL(clicked()),this,SLOT(detailsClicked()));
	KviTalToolTip::add(m_pDetailsButton,__tr2qs_ctx("<center>Click here to edit advanced options for this entry</center>","options"));

	m_pRecentPopup = new KviTalPopupMenu(gbox);
	connect(m_pRecentPopup,SIGNAL(aboutToShow()),this,SLOT(recentServersPopupAboutToShow()));
	connect(m_pRecentPopup,SIGNAL(activated(int)),this,SLOT(recentServersPopupClicked(int)));

	QToolButton * tb = new QToolButton(gbox);
	tb->setIcon(QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Time))));
	tb->setMenu(m_pRecentPopup);
	tb->setAutoRaise(true);
	tb->setPopupMode(QToolButton::InstantPopup);

	KviTalToolTip::add(tb,__tr2qs_ctx("<center>This button shows a list of recently used servers. It allows you to quickly find them in the list.</center>","options"));

	m_pShowThisDialogAtStartupSelector = NULL;

	// The "Show this dialog at startup" option is shown only when the server options widget is shown as standalone dialog
	if(parent->inherits("OptionsWidgetContainer"))
	{
		m_pConnectCurrent = new QPushButton(__tr2qs_ctx("Connect &Now","options"),this);
		addWidgetToLayout(m_pConnectCurrent,0,3,3,3);
		connect(m_pConnectCurrent,SIGNAL(clicked()),this,SLOT(connectCurrentClicked()));

		QPalette pal(QColor(0,0,128));

		pal.setColor(QPalette::Active,QPalette::Button,QColor(0,0,220));
		pal.setColor(QPalette::Active,QPalette::ButtonText,QColor(245,245,245));

		pal.setColor(QPalette::Disabled,QPalette::Button,QColor(98,98,180));
		pal.setColor(QPalette::Disabled,QPalette::ButtonText,QColor(128,128,245));

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

		KviTalToolTip::add(m_pConnectCurrent,__tr2qs_ctx("<center>Hit this button to connect to the currently selected server.</center>","options"));

		OptionsWidgetContainer * pContainer = dynamic_cast<OptionsWidgetContainer *>(parent);
		if(pContainer)
		{
			m_pShowThisDialogAtStartupSelector = addBoolSelector(pContainer,__tr2qs_ctx("Show this dialog at startup","options"),KviOption_boolShowServersConnectDialogOnStart);
			pContainer->setLeftCornerWidget(m_pShowThisDialogAtStartupSelector);
			// This selector can be destroyed upon reparenting: make sure it's removed from the selector list
			// (or we'll get a crash at commit() time...).
			QObject::connect(m_pShowThisDialogAtStartupSelector,SIGNAL(destroyed()),this,SLOT(slotShowThisDialogAtStartupSelectorDestroyed()));

			KviTalToolTip::add(m_pShowThisDialogAtStartupSelector,__tr2qs_ctx("<center>If this option is enabled, the Servers dialog will appear every time you start KVIrc</center>","options"));
		}
	} else {
		m_pConnectCurrent = NULL;
	}

	m_pLastEditedItem = 0;

	m_pClipboard = 0;

	fillServerList();

	layout()->setRowStretch(1,1);
	layout()->setColumnStretch(1,1);
	setMinimumWidth(600);
}

OptionsWidget_servers::~OptionsWidget_servers()
{
	if(m_pImportFilter)
	{
		disconnect(m_pImportFilter,0,this,0);
		m_pImportFilter->die();
		m_pImportFilter = 0;
	}

	if(m_pClipboard)
		delete m_pClipboard;
	if(m_pServerDetailsDialog)
		delete m_pServerDetailsDialog;
	if(m_pNetworkDetailsDialog)
		delete m_pNetworkDetailsDialog;
}

void OptionsWidget_servers::slotShowThisDialogAtStartupSelectorDestroyed()
{
	KVI_ASSERT(m_pShowThisDialogAtStartupSelector);

	removeSelector(m_pShowThisDialogAtStartupSelector);
	m_pShowThisDialogAtStartupSelector = NULL;
}

void OptionsWidget_servers::recentServersPopupAboutToShow()
{
	g_pApp->fillRecentServersPopup(m_pRecentPopup);

	m_pRecentPopup->insertSeparator();
	m_pRecentPopup->insertItem(__tr2qs("Clear Recent Servers List"));
}

void OptionsWidget_servers::recentServersPopupClicked(int id)
{
	if(!g_pActiveWindow)
		return; // doh

	KviConsoleWindow * c = g_pActiveWindow->console();
	if(!c)
		return;

	QString szItemText = m_pRecentPopup->text(id);
	szItemText.remove(QChar('&'));
	if(szItemText.isEmpty())
		return;
	if(szItemText == __tr2qs("Clear Recent Servers List"))
	{
		KviKvsScript::run("option stringlistRecentServers",c);
		return;
	}

	selectBestServerByUrl(szItemText);
}

void OptionsWidget_servers::selectBestServerByUrl(const QString &szUrl)
{
	if(szUrl.isEmpty())
		return;

	KviIrcUrlParts oParts;

	KviIrcUrl::split(szUrl,oParts);

	int uCount = m_pTreeWidget->topLevelItemCount();
	int uIdx = 0;

	IrcServerOptionsTreeWidgetItem * pBestCandidate = NULL;
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
				{
					// exact match
					uIdx = uCount; // break the outer loop
					break; // break the inner loop
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
	if(szServer.isEmpty())return;
	KviKvsScript::run("server -u",g_pApp->activeConsole());
}

void OptionsWidget_servers::fillServerList()
{
	IrcServerOptionsTreeWidgetItem * net;
	IrcServerOptionsTreeWidgetItem * srv;
	IrcServerOptionsTreeWidgetItem * cur = 0;

	KviPointerHashTableIterator<QString,KviIrcNetwork> it(*(g_pServerDataBase->recordDict()));

	if(m_pConnectCurrent)
		m_pConnectCurrent->setEnabled(false);

	while(KviIrcNetwork * r = it.current())
	{
		net = new IrcServerOptionsTreeWidgetItem(m_pTreeWidget,*(g_pIconManager->getSmallIcon(KviIconManager::World)),r);
		KviPointerList<KviIrcServer> * sl = r->serverList();
		bool bCurrent = r->name() == g_pServerDataBase->currentNetworkName().toUtf8().data();
		net->setExpanded(bCurrent);
	
		for(KviIrcServer * s = sl->first();s;s = sl->next())
		{
			srv = new IrcServerOptionsTreeWidgetItem(net,*(g_pIconManager->getSmallIcon(KviIconManager::Server)),s);
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

void OptionsWidget_servers::itemDoubleClicked(QTreeWidgetItem*, int )
{
	detailsClicked();
}

void OptionsWidget_servers::currentItemChanged(QTreeWidgetItem *it,QTreeWidgetItem *)
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

		if(m_pLastEditedItem->m_pServerData)
		{
			m_pSrvNetLabel->setText(__tr2qs_ctx("Server:","options"));
			m_pSrvNetEdit->setText(m_pLastEditedItem->m_pServerData->hostName());
		} else {
			m_pSrvNetLabel->setText(__tr2qs_ctx("Network:","options"));
			m_pSrvNetEdit->setText(it->text(0));
		}
	} else {
		m_pRemoveButton->setEnabled(false);
		m_pCopyServerButton->setEnabled(true);

		if(m_pConnectCurrent)
			m_pConnectCurrent->setEnabled(false);
		m_pSrvNetLabel->setEnabled(false);
		m_pSrvNetEdit->setEnabled(false);
		m_pSrvNetEdit->setText(__tr2qs_ctx("No selection","options"));
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
		m_pTreeWidget->scrollToItem(m_pLastEditedItem,QTreeWidget::EnsureVisible);
}

void OptionsWidget_servers::filterTextEdited(const QString &)
{
	QString szFilter=m_pFilterEdit->text().trimmed();

	IrcServerOptionsTreeWidgetItem * network;
	for (int i=0;i<m_pTreeWidget->topLevelItemCount();i++)
	{
		network = (IrcServerOptionsTreeWidgetItem *) m_pTreeWidget->topLevelItem(i);
		if(network->m_pNetworkData->name().contains(szFilter, Qt::CaseInsensitive) || 
				network->m_pNetworkData->description().contains(szFilter, Qt::CaseInsensitive))
		{
			network->setHidden(false);
			// if the net matches, we always show all its servers
			IrcServerOptionsTreeWidgetItem * ch;
			for (int j=0;j<network->childCount();j++)
			{
				ch=(IrcServerOptionsTreeWidgetItem *)network->child(j);
				ch->setHidden(false);
			}
		} else {
			uint uServers=0;
			
			IrcServerOptionsTreeWidgetItem * ch;
			for (int j=0;j<network->childCount();j++)
			{
				bool bHidden=true;
				ch=(IrcServerOptionsTreeWidgetItem *)network->child(j);
				if(ch->m_pServerData)
				{
					if(ch->m_pServerData->hostName().contains(szFilter, Qt::CaseInsensitive) || 
						ch->m_pServerData->description().contains(szFilter, Qt::CaseInsensitive))
							bHidden=false;
				}
				if(!bHidden)
					uServers++;
				ch->setHidden(bHidden);
			}
			//if at list one server matches, we show its network
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
			tmp = QString::fromAscii("irc.unknown.net");
		m_pLastEditedItem->m_pServerData->setHostName(tmp);
		m_pLastEditedItem->updateVisibleStrings();
	} else if(m_pLastEditedItem->m_pNetworkData)
	{
		QString tmp = m_pSrvNetEdit->text();
		if(tmp.isEmpty())
			tmp = __tr2qs_ctx("UnknownNet","options");
		m_pLastEditedItem->m_pNetworkData->setName(tmp);
		m_pLastEditedItem->updateVisibleStrings();
	}
}

void OptionsWidget_servers::commit()
{
	saveLastItem();

	g_pServerDataBase->clear();

	IrcServerOptionsTreeWidgetItem * network;
	for (int i=0;i<m_pTreeWidget->topLevelItemCount();i++)
	{
		network=(IrcServerOptionsTreeWidgetItem *) m_pTreeWidget->topLevelItem(i);
		QString tmp = network->m_pNetworkData->name();
		KviIrcNetwork * pNetwork = g_pServerDataBase->findNetwork(tmp);
		if(pNetwork)
		{
			pNetwork->copyFrom(*(network->m_pNetworkData));
		} else {
			pNetwork = new KviIrcNetwork(tmp);
			pNetwork->copyFrom(*(network->m_pNetworkData));
			g_pServerDataBase->addNetwork(pNetwork);
		}
		if(network == m_pLastEditedItem)g_pServerDataBase->setCurrentNetwork(pNetwork->name());

		IrcServerOptionsTreeWidgetItem * ch;
		for (int j=0;j<network->childCount();j++)
		{
			KviIrcServer *srv;
			ch=(IrcServerOptionsTreeWidgetItem *)network->child(j);
			if(ch->m_pServerData)
			{
				if(!ch->m_pServerData->hostName().isEmpty())
				{
					srv = pNetwork->findServer(ch->m_pServerData);
					if(!srv)
					{
						srv = new KviIrcServer(*(ch->m_pServerData));
						pNetwork->insertServer(srv);
					} else *srv = *(ch->m_pServerData);
					if(srv->id().isEmpty())srv->generateUniqueId();
					if(ch == m_pLastEditedItem)
					{
						g_pServerDataBase->setCurrentNetwork(pNetwork->name());
						pNetwork->setCurrentServer(srv);
					}
				}
			}
		}

	}

	KviOptionsWidget::commit();

	// Ensure saving of the server DB
	g_pApp->saveIrcServerDataBase();
}

void OptionsWidget_servers::customContextMenuRequested(const QPoint &pnt)
{
	int id;
	QTreeWidgetItem *it=(QTreeWidgetItem *) m_pTreeWidget->itemAt(pnt);
	bool bServer = (it && ((IrcServerOptionsTreeWidgetItem *)it)->m_pServerData);
	m_pContextPopup->clear();
	m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::World)),__tr2qs_ctx("New Network","options"),this,SLOT(newNetwork()));
	id = m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::Cut)),__tr2qs_ctx("Remove Network","options"),this,SLOT(removeCurrent()));
	m_pContextPopup->setItemEnabled(id,!bServer);
	m_pContextPopup->insertSeparator();
	id = m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::Server)),__tr2qs_ctx("&New Server","options"),this,SLOT(newServer()));
	id = m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::Cut)),__tr2qs_ctx("Re&move Server","options"),this,SLOT(removeCurrent()));
	m_pContextPopup->setItemEnabled(id,bServer);
	id = m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::Copy)),__tr2qs_ctx("&Copy Server","options"),this,SLOT(copyServer()));
	m_pContextPopup->setItemEnabled(id,bServer);
	id = m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KviIconManager::Paste)),__tr2qs_ctx("&Paste Server","options"),this,SLOT(pasteServer()));
	m_pContextPopup->setItemEnabled(id,m_pClipboard);

	m_pContextPopup->insertSeparator();
	//	m_pContextPopup->insertItem(__c2q(__tr("Merge list from server.ini","options")),this,SLOT(importFromIni()));
	m_pContextPopup->insertItem(__tr2qs_ctx("Clear List","options"),this,SLOT(clearList()));
	m_pContextPopup->insertSeparator();
	m_pContextPopup->insertItem(__tr2qs_ctx("Import List","options"),m_pImportPopup);
	m_pContextPopup->popup(QCursor::pos());
}

void OptionsWidget_servers::importPopupAboutToShow()
{
	g_pModuleManager->loadModulesByCaps("serverimport");
	m_pImportPopup->clear();

	KviModuleExtensionDescriptorList * l = KviModuleExtensionManager::instance()->getExtensionList("serverimport");

	if(!l)return;

	int id;

	for(KviModuleExtensionDescriptor * d = l->first();d;d = l->next())
	{
		if(d->icon())
			id = m_pImportPopup->insertItem(*(d->icon()),d->visibleName());
		else
			id = m_pImportPopup->insertItem(d->visibleName());
		m_pImportPopup->setItemParameter(id,d->id());
	}
}

void OptionsWidget_servers::importPopupActivated(int id)
{
	// ensure that we have all the modules : they could have been unloaded while the popup was displayed
	g_pModuleManager->loadModulesByCaps("serverimport");

	KviModuleExtensionDescriptorList * l = KviModuleExtensionManager::instance()->getExtensionList("serverimport");

	if(!l)
	{
		// ops.. internal error: I thought to have a module capable of importing servers
		// but actually it's not the case.. something weird happened (in the best case
		// the user has just unloaded the module and removed it from disk ?)
		KviMessageBox::warning(__tr2qs_ctx("Oops... something weird happened:<br>Can't find any module capable of importing servers.","options"));
		return;
	}

	if(m_pImportFilter)
	{
		disconnect(m_pImportFilter,0,this,0);
		m_pImportFilter->die();
		m_pImportFilter = 0;
	}

	id = m_pImportPopup->itemParameter(id);

	m_pImportFilter = (KviMexServerImport *)KviModuleExtensionManager::instance()->allocateExtension("serverimport",id,0);

	if(!m_pImportFilter)
	{
		KviMessageBox::warning(__tr2qs_ctx("Oops... something weird happened:<br>Can't find the module that was capable of this import action. :(","options"));
		return;
	}

	connect(m_pImportFilter,SIGNAL(server(const KviIrcServer &,const QString&)),this,SLOT(importServer(const KviIrcServer &,const QString&)));
	connect(m_pImportFilter,SIGNAL(destroyed()),this,SLOT(importerDead()));

	m_pImportFilter->start();
}

void OptionsWidget_servers::importerDead()
{
	m_pImportFilter = 0;
}

void OptionsWidget_servers::importServer(const KviIrcServer &s,const QString &network)
{
	IrcServerOptionsTreeWidgetItem * net = findNetItem(network);
	if(!net)
	{
		KviIrcNetwork d(network);
		net = new IrcServerOptionsTreeWidgetItem(m_pTreeWidget,*(g_pIconManager->getSmallIcon(KviIconManager::World)),&d);
		net->setExpanded(true);
	}

	IrcServerOptionsTreeWidgetItem * srv ;
	for(int i=0;i<net->childCount();i++)
	{
		srv=(IrcServerOptionsTreeWidgetItem *) net->child(i);
		if((srv)->m_pServerData->useSSL() == s.useSSL())
		{
			if(srv->m_pServerData->isIPv6() == s.isIPv6())
			{
				if(KviQString::equalCI(srv->m_pServerData->hostName(),s.hostName()))
				{
					// update the port
					srv->m_pServerData->setPort(s.port());
					if(!s.ip().isEmpty())srv->m_pServerData->setIp(s.ip());
					if(!s.password().isEmpty())srv->m_pServerData->setPassword(s.password());
					if(!s.nickName().isEmpty())srv->m_pServerData->setNickName(s.nickName());
					m_pTreeWidget->setCurrentItem(srv);
					m_pTreeWidget->scrollToItem(srv);
					return;
				}
			}
		}
	}

	// not found : add it!
	IrcServerOptionsTreeWidgetItem * newServer = new IrcServerOptionsTreeWidgetItem(net,*(g_pIconManager->getSmallIcon(KviIconManager::Server)),&s);
	m_pTreeWidget->setCurrentItem(newServer);
	m_pTreeWidget->scrollToItem(newServer);
}

void OptionsWidget_servers::newNetwork()
{
	KviIrcNetwork d(__tr2qs_ctx("New Network","options"));
	IrcServerOptionsTreeWidgetItem * it = new IrcServerOptionsTreeWidgetItem(m_pTreeWidget,*(g_pIconManager->getSmallIcon(KviIconManager::World)),&d);
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
			if(!net)return;
		}
		else net = m_pLastEditedItem;

		KviIrcServer tmp;
		tmp.setHostName(__tr2qs_ctx("irc.unknown.net","options"));
		tmp.setCacheIp(false);
		tmp.generateUniqueId();

		IrcServerOptionsTreeWidgetItem * it = new IrcServerOptionsTreeWidgetItem(net,*(g_pIconManager->getSmallIcon(KviIconManager::Server)),&tmp);

		net->setExpanded(true);

		it->setSelected(true);
		m_pTreeWidget->setCurrentItem(it);
		m_pTreeWidget->scrollToItem(it);
	}
}

void OptionsWidget_servers::copyServer()
{
	if(m_pLastEditedItem)
	{
		if(m_pLastEditedItem->m_pServerData)
		{
			if(!m_pClipboard)m_pClipboard = new KviIrcServer();
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
				if(!net)return;
			}
			else net = m_pLastEditedItem;

			IrcServerOptionsTreeWidgetItem * it = new IrcServerOptionsTreeWidgetItem(net,
							*(g_pIconManager->getSmallIcon(KviIconManager::Server)),m_pClipboard);

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
		} else {
			QTreeWidgetItem * tmp = m_pLastEditedItem->parent();
			index = tmp->indexOfChild(m_pLastEditedItem);
			if(index > -1) tmp->takeChild(index);
		}

		if(!it)it = m_pTreeWidget->topLevelItem(0);
		if(it)
		{
			it->setSelected(true);
			m_pTreeWidget->setCurrentItem(it);
			m_pTreeWidget->scrollToItem(it);
		}
	}
}

IrcServerOptionsTreeWidgetItem * OptionsWidget_servers::findNetItem(const QString &netname)
{

	IrcServerOptionsTreeWidgetItem * it;
	for(int i=0;i<m_pTreeWidget->topLevelItemCount();i++)
	{
		it=(IrcServerOptionsTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		if(KviQString::equalCI(it->text(0),netname))return it;
	}
	return 0;
}

void OptionsWidget_servers::clearList()
{
	m_pTreeWidget->clear();
	m_pLastEditedItem = 0;
	currentItemChanged(0,0);
}

void OptionsWidget_servers::detailsClicked()
{
	if(!m_pLastEditedItem)return;
	if(m_pLastEditedItem->m_pServerData)
	{
		saveLastItem();

		m_pServerDetailsDialog = new IrcServerDetailsWidget(this,m_pLastEditedItem->m_pServerData);

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
		m_pServerDetailsDialog = 0;
		return;
	}
	if(m_pLastEditedItem->m_pNetworkData)
	{
		saveLastItem();

		m_pNetworkDetailsDialog = new IrcNetworkDetailsWidget(this,m_pLastEditedItem->m_pNetworkData);

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
		m_pNetworkDetailsDialog = 0;
	}
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "m_OptionsWidget_servers.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
