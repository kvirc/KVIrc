//=============================================================================
//
//   File : optw_servers.cpp
//   Creation date : Wed Jun 12 2000 03:29:51 by Szymon Stefanek
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

#include "optw_servers.h"
#include "optw_proxy.h"
#include "kvi_query.h"
#include "kvi_channel.h"
#include "kvi_locale.h"
#include "kvi_iconmanager.h"
#include "kvi_ircserverdb.h"
#include "kvi_ipeditor.h"
#include "kvi_netutils.h"
#include "kvi_settings.h"
#include "kvi_filedialog.h"
#include "kvi_config.h"
#include "kvi_scripteditor.h"
#include "kvi_options.h"

#include "kvi_app.h"
#include "kvi_console.h"
#include "kvi_ircurl.h"
#include "kvi_module.h"
#include "kvi_modulemanager.h"
#include "kvi_moduleextension.h"
#include "kvi_msgbox.h"
#include "kvi_mexserverimport.h"
#include "kvi_nickserv.h"
#include "optw_nickserv.h" // for the NickServ rule editor
#include "kvi_proxydb.h"
#include "kvi_kvs_script.h"
#include "kvi_styled_controls.h"

#include <qtoolbutton.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include "kvi_tal_popupmenu.h"
#include <qcursor.h>
#include <qtooltip.h>
#include <qvalidator.h>
#include <qmessagebox.h>
#include "kvi_asciidict.h"
#include <qcombobox.h>
#include <qbuttongroup.h>
#include <qmessagebox.h>
#include <qinputdialog.h>
#include <qmessagebox.h>


KviNetworkDetailsWidget::KviNetworkDetailsWidget(QWidget * par,KviIrcNetwork * n)
	: QDialog(par,"network_details",true)
{
	m_pOnConnectEditor=0;
	m_pOnLoginEditor=0;
	
	m_pUserEditor=0;
	m_pPassEditor=0;
	m_pNickEditor=0;
	m_pRealEditor=0;
	m_pDescEditor=0;
	
	m_pEncodingEditor=0;
		
	m_pAutoConnectCheck=0;
		
	m_pNickServListView=0;
	m_pNickServCheck=0;
	m_pAddRuleButton=0;
	m_pDelRuleButton=0;
	m_pEditRuleButton=0;
	
	m_pChannelListSelector=0;
	
	QGridLayout * g = new QGridLayout(this,4,4,10,4);

	setCaption(__tr2qs_ctx("Network Details","options"));
	setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_WORLD)));

	QString szTmp = "<font size=\"+1\"><b>";
	szTmp += n->name();
	szTmp += "</b></font>";

	QLabel * l = new QLabel(szTmp,this);
	l->setFrameStyle(QFrame::Raised | QFrame::StyledPanel);
	l->setAlignment(Qt::AlignCenter);
	l->setMargin(10);
	g->addMultiCellWidget(l,0,0,0,3);
	l->setMinimumWidth(200);

	l = new QLabel(__tr2qs_ctx("Description:","options"),this);
	g->addWidget(l,1,0);
	m_pDescEditor = new QLineEdit(this);
	g->addMultiCellWidget(m_pDescEditor,1,1,1,3);
	m_pDescEditor->setText(n->description());
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pDescEditor,__tr2qs_ctx("<center>Put here a brief description of the network.</center>","options"));
#endif

	// Identity tab

	QTabWidget * tw = new QTabWidget(this);
	g->addMultiCellWidget(tw,2,2,0,3);

	QWidget * tab = new QWidget(tw);
	QGridLayout * gl = new QGridLayout(tab,4,2,10,4);

	KviTalGroupBox *gbox = new KviTalGroupBox(2,KviTalGroupBox::Horizontal,__tr2qs_ctx("Properties","options"),tab);
	gl->addMultiCellWidget(gbox,0,0,0,1);

	l = new QLabel(__tr2qs_ctx("Username:","options"),gbox);
	m_pUserEditor = new QLineEdit(gbox);
	m_pUserEditor->setText(n->userName());
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pUserEditor,__tr2qs_ctx("<center>This is the <b>username</b> that KVIrc will use to login to servers on this network.\n" \
										"If this field is left empty (most common case), KVIrc will use the default username " \
										"specified in the \"Identity\" options tab.</center>","options"));
#endif

	l = new QLabel(__tr2qs_ctx("Nickname:","options"),gbox);
	m_pNickEditor = new QLineEdit(gbox);
	QValidator * v = new QRegExpValidator(QRegExp("[^-0-9 ][^ ]*","options"),gbox);
	m_pNickEditor->setValidator(v);
	m_pNickEditor->setText(n->nickName());
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pNickEditor,__tr2qs_ctx("<center>You can specify a \"special\" <b>nickname</b> that will be used to log in to the servers on this network.<br>" \
									"If this field is left empty (most common case), the default nickname (specified in the \"Identity\" settings) will be used.</center>","options"));
#endif

	l = new QLabel(__tr2qs_ctx("Real name:","options"),gbox);
	m_pRealEditor = new QLineEdit(gbox);
	m_pRealEditor->setText(n->realName());
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pRealEditor,__tr2qs_ctx("<center>You can specify a \"special\" <b>real name</b> that will be used to login with the servers on this network.<br>" \
									"If you leave this field empty (most common case), the default \"real name\" (specified in the \"Identity\" settings) will be used.</center>","options"));
#endif


	l = new QLabel(__tr2qs_ctx("Encoding:","options"),tab);
	gl->addWidget(l,1,0);
	m_pEncodingEditor = new QComboBox(false,tab);
	m_pEncodingEditor->setDuplicatesEnabled(false);
	gl->addWidget(m_pEncodingEditor,1,1);
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pEncodingEditor,__tr2qs_ctx("<center>This box allows you to choose the preferred encoding for the servers in this network. " \
									"If you choose \"Use System Encoding\" then the encoding will be set to the systemwide " \
									"value that you choose in the \"Encoding\" page of the options dialog.</center>","options"));
#endif

	int i = 0;
	int current = 0;
	KviLocale::EncodingDescription * d = KviLocale::encodingDescription(i);
	QString tmp;
	m_pEncodingEditor->insertItem(__tr2qs_ctx("Use System Encoding","options"));
	while(d->szName)
	{
		KviQString::sprintf(tmp,"%s (%s)",d->szName,d->szDescription);
		m_pEncodingEditor->insertItem(tmp);
		if(KviQString::equalCI(d->szName,n->encoding()))current = i + 1;
		i = i + 1;
		d = KviLocale::encodingDescription(i);
	}

	m_pEncodingEditor->setCurrentItem(current);

	m_pAutoConnectCheck = new KviStyledCheckBox(__tr2qs_ctx("Connect to this network at startup","options"),tab);
	m_pAutoConnectCheck->setChecked(n->autoConnect());
	gl->addMultiCellWidget(m_pAutoConnectCheck,2,2,0,1);
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pAutoConnectCheck,__tr2qs_ctx("<center>This option will cause KVIrc to automatically connect to this network at startup</center>","options"));
#endif


	l = new QLabel("",tab);
	gl->addWidget(l,3,0);

	gl->setRowStretch(3,1);
	gl->setColStretch(1,1);

	tw->addTab(tab,__tr2qs_ctx("General","options"));

	// after login join channels
	tab = new QWidget(tw);

	gl = new QGridLayout(tab,1,1,10,4);

	if(n->autoJoinChannelList())
		m_lstChannels = *(n->autoJoinChannelList());
	m_pChannelListSelector = new KviCahnnelListSelector(tab,
		__tr2qs_ctx("Channels to join automatically upon connect:","options"),&m_lstChannels,true);
#ifdef COMPILE_INFO_TIPS
  QToolTip::add(m_pChannelListSelector,__tr2qs_ctx("<center>Here you can set a list of channels to be joined automatically " \
		"after a connection to a server in this network has been established. To add a channel, type its name in the " \
		"text input below and click \"<b>Add</b>\".</center>","options"));
#endif
	gl->addWidget(m_pChannelListSelector,0,0);

	tw->addTab(tab,__tr2qs_ctx("Join Channels","options"));

	// after connect execute
	tab = new QWidget(tw);
	gl = new QGridLayout(tab,1,1,10,4);
	
	m_pOnConnectEditor = KviScriptEditor::createInstance(tab);
	gl->addWidget(m_pOnConnectEditor,0,0);
	m_pOnConnectEditor->setText(n->onConnectCommand());
	m_pOnConnectEditor->setMinimumHeight(150);
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pOnConnectEditor,__tr2qs_ctx("<center>The following commands will be executed after a connection to a server in this network has been established.<br>" \
				"<b>Important:</b> Enter commands <b>without</b> a preceding slash (e.g. <tt>quote pass secret</tt> instead of <tt>/quote pass secret</tt>).<br>"\
				"KVIrc will first send the USER command, then eventually PASS and NICK and then execute this " \
				"command sequence.</center>","options"));
#endif
	
	tw->addTab(tab,__tr2qs_ctx("On Connect","options"));
	

	// after login execute
	tab = new QWidget(tw);
	gl = new QGridLayout(tab,1,1,10,4);

	m_pOnLoginEditor = KviScriptEditor::createInstance(tab);
	gl->addWidget(m_pOnLoginEditor,0,0);
	m_pOnLoginEditor->setText(n->onLoginCommand());
	
	m_pOnLoginEditor->setMinimumHeight(150);
	
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pOnLoginEditor,
				__tr2qs_ctx("<center>The following commands will be executed after a successful login to a server in this network.<br>" \
						"<b>Important:</b> Enter commands <b>without</b> a preceding slash (e.g. <tt>quote privatelog</tt> instead of <tt>/quote privatelog</tt>).<br>"
						"This is useful for automatically opening queries, setting variables, etc.</center>","options"));
#endif
	
	tw->addTab(tab,__tr2qs_ctx("On Login","options"));


	// nick serv rules
	
	tab = new QWidget(tw);
	gl = new QGridLayout(tab,3,3,10,5);
	
	KviNickServRuleSet * rs = n->nickServRuleSet();
	bool bNickServEnabled = rs ? (rs->isEnabled() && !rs->isEmpty()) : false;
	
	m_pNickServCheck = new KviStyledCheckBox(__tr2qs_ctx("Enable NickServ Identification","options"),tab);
	gl->addMultiCellWidget(m_pNickServCheck,0,0,0,2);
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pNickServCheck,
				__tr2qs_ctx("This check enables the automatic identification with NickServ","options"));
#endif
	m_pNickServCheck->setChecked(bNickServEnabled);
	
	m_pNickServListView = new KviTalListView(tab);
	m_pNickServListView->setSelectionMode(KviTalListView::Single);
	m_pNickServListView->setAllColumnsShowFocus(true);
	m_pNickServListView->addColumn(__tr2qs_ctx("Nickname","options"));
	m_pNickServListView->addColumn(__tr2qs_ctx("NickServ Mask","options"));
	m_pNickServListView->addColumn(__tr2qs_ctx("NickServ Request Mask","options"));
	m_pNickServListView->addColumn(__tr2qs_ctx("Identify Command","options"));
	connect(m_pNickServListView,SIGNAL(selectionChanged()),this,SLOT(enableDisableNickServControls()));
	
	gl->addMultiCellWidget(m_pNickServListView,1,1,0,2);
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pNickServListView,
		__tr2qs_ctx("<center>This is a list of NickServ identification rules. " \
				"KVIrc will use them to model its automatic interaction with NickServ on this network.<br>" \
				"Please be aware that this feature can cause your NickServ passwords to be stolen " \
				"if used improperly. Make sure that you fully understand the NickServ authentication protocol.<br>" \
				"In other words, be sure to know what you're doing.<br>" \
				"Also note that the password that you provide is stored as <b>PLAIN TEXT</b>.</center>","options"));
#endif
	
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
		KviPtrList<KviNickServRule> * ll = rs->rules();
		for(KviNickServRule * rule = ll->first();rule;rule = ll->next())
		{
			(void)new KviTalListViewItem(m_pNickServListView,rule->registeredNick(),rule->nickServMask(),rule->messageRegexp(),rule->identifyCommand());
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
	g->setColStretch(1,1);

	tw->setMinimumWidth(375);
}



KviNetworkDetailsWidget::~KviNetworkDetailsWidget()
{
	if(m_pOnConnectEditor) KviScriptEditor::destroyInstance(m_pOnConnectEditor);
	if(m_pOnLoginEditor) KviScriptEditor::destroyInstance(m_pOnLoginEditor);
}

void KviNetworkDetailsWidget::editNickServRule()
{
	KviTalListViewItem * it = m_pNickServListView->currentItem();
	if(!it)return;
	KviNickServRule r(it->text(0),it->text(1),it->text(2),it->text(3));
	KviNickServRuleEditor ed(this,false);
	if(ed.editRule(&r))
	{
		it->setText(0,r.registeredNick());
		it->setText(1,r.nickServMask());
		it->setText(2,r.messageRegexp());
		it->setText(3,r.identifyCommand());
	}
}

void KviNetworkDetailsWidget::addNickServRule()
{
	KviNickServRule r;
	KviNickServRuleEditor ed(this,false);
	if(ed.editRule(&r))
		(void)new KviTalListViewItem(m_pNickServListView,r.registeredNick(),r.nickServMask(),r.messageRegexp(),r.identifyCommand());
}

void KviNetworkDetailsWidget::delNickServRule()
{
	KviTalListViewItem * it = m_pNickServListView->currentItem();
	if(!it)return;
	delete it;
	enableDisableNickServControls();
}

void KviNetworkDetailsWidget::enableDisableNickServControls()
{
	bool bEnabled = m_pNickServCheck->isChecked();
	m_pNickServListView->setEnabled(bEnabled);
	m_pAddRuleButton->setEnabled(bEnabled);
	bEnabled = bEnabled && (m_pNickServListView->childCount() > 0) && m_pNickServListView->currentItem();
	m_pDelRuleButton->setEnabled(bEnabled);
	m_pEditRuleButton->setEnabled(bEnabled);
}

void KviNetworkDetailsWidget::fillData(KviIrcNetwork * n)
{
	n->setUserName(m_pUserEditor->text());
	n->setNickName(m_pNickEditor->text());
	n->setRealName(m_pRealEditor->text());
	n->setDescription(m_pDescEditor->text());
	if(m_pAutoConnectCheck)
		n->setAutoConnect(m_pAutoConnectCheck->isChecked());
	if(m_pEncodingEditor)
		if(m_pEncodingEditor->currentItem() <= 0)n->setEncoding(QString::null);
		else {
			KviLocale::EncodingDescription * d = KviLocale::encodingDescription(m_pEncodingEditor->currentItem() - 1);
			n->setEncoding(d->szName);
		}
	if(m_pChannelListSelector)
		m_pChannelListSelector->commit();
	if(m_lstChannels.isEmpty())n->setAutoJoinChannelList(0);
	else n->setAutoJoinChannelList(new QStringList(m_lstChannels));
	if(m_pNickServListView)
	{
		if(m_pNickServListView->childCount() > 0)
		{
			KviNickServRuleSet * rs = KviNickServRuleSet::createInstance();
			rs->setEnabled(m_pNickServCheck->isChecked());
			KviTalListViewItem * it = m_pNickServListView->firstChild();
			while(it)
			{
				rs->addRule(KviNickServRule::createInstance(it->text(0),it->text(1),it->text(2),it->text(3)));
				it = it->nextSibling();
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







KviServerDetailsWidget::KviServerDetailsWidget(QWidget * par,KviIrcServer * s)
    : QDialog(par,"server_details",true)
{
	m_szHostname = s->hostName();
	QGridLayout * g = new QGridLayout(this,4,4,10,4);

	setCaption(__tr2qs_ctx("Server Details","options"));
	setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SERVER)));

	m_pHeaderLabel = new QLabel("",this); // the text will be set later
	m_pHeaderLabel->setFrameStyle(QFrame::Raised | QFrame::StyledPanel);
	m_pHeaderLabel->setAlignment(Qt::AlignCenter);
	m_pHeaderLabel->setMargin(10);
	g->addMultiCellWidget(m_pHeaderLabel,0,0,0,3);
	m_pHeaderLabel->setMinimumWidth(200);

	QLabel * l = new QLabel(__tr2qs_ctx("Description:","options"),this);
	g->addWidget(l,1,0);
	m_pDescEditor = new QLineEdit(this);
	g->addMultiCellWidget(m_pDescEditor,1,1,1,3);
	m_pDescEditor->setText(s->description());
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pDescEditor,__tr2qs_ctx("<center>This is a brief description of this server. " \
								"This field has no restrictions but many server lists use it to describe the server's physical location</center>","options"));
#endif

	// Identity tab

	QTabWidget * tw = new QTabWidget(this);
	g->addMultiCellWidget(tw,2,2,0,3);

	QWidget * tab = new QWidget(tw);
	QGridLayout * gl = new QGridLayout(tab,3,1,10,4);

	KviTalGroupBox *gbox = new KviTalGroupBox(2,KviTalGroupBox::Horizontal,__tr2qs_ctx("Properties","options"),tab);
	gl->addWidget(gbox,0,0);

	l = new QLabel(__tr2qs_ctx("Username:","options"),gbox);
	m_pUserEditor = new QLineEdit(gbox);
	m_pUserEditor->setText(s->userName());
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pUserEditor,__tr2qs_ctx("<center>This is the <b>username</b> that KVIrc will use to login to this server.\n" \
										"If this field is left empty (most common case), KVIrc will first look if an username is specified " \
										"for the network that this server belongs to, and if that is empty then KVIrc will use the default username " \
										"specified in the \"Identity\" options tab.</center>","options"));
#endif

	l = new QLabel(__tr2qs_ctx("Password:","options"),gbox);
	m_pPassEditor = new QLineEdit(gbox);
	m_pPassEditor->setEchoMode(QLineEdit::Password);
	m_pPassEditor->setText(s->password());
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pPassEditor,__tr2qs_ctx("<center>If this server requires a password, put it in this field, otherwise leave it empty.</center>","options"));
#endif

	l = new QLabel(__tr2qs_ctx("Nickname:","options"),gbox);
	m_pNickEditor = new QLineEdit(gbox);
	QValidator * v = new QRegExpValidator(QRegExp("[^-0-9 ][^ ]*"),gbox);
	m_pNickEditor->setValidator(v);
	m_pNickEditor->setText(s->nickName());
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pNickEditor,__tr2qs_ctx("<center>You can specify a \"special\" <b>nickname</b> that will be used to log in to this server.<br>" \
									"If this field is left empty (most common case), KVIrc will first look if a nickname is specified " \
									"for the network that this server belongs to, and if that is empty then the default nickname (specified in the \"Identity\" settings) will be used.</center>","options"));
#endif

	l = new QLabel(__tr2qs_ctx("Real name:","options"),gbox);
	m_pRealEditor = new QLineEdit(gbox);
	m_pRealEditor->setText(s->realName());
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pRealEditor,__tr2qs_ctx("<center>You can specify a \"special\" <b>real name</b> that will be used to login with this server.<br>" \
									"If you leave this field empty (most common case), KVIrc will first look if a real name is specified " \
									"for the network that this server belongs to, and if that is empty the default \"real name\" (specified in the \"Identity\" settings) will be used.</center>","options"));
#endif
	gbox = new KviTalGroupBox(1,KviTalGroupBox::Horizontal,__tr2qs_ctx("User Mode","options"),tab);
	gl->addWidget(gbox,1,0);
	
	m_pUseDefaultInitUMode = new KviStyledCheckBox(__tr2qs_ctx("Use default user mode","options"),gbox);
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pUseDefaultInitUMode,__tr2qs_ctx("<center>If this is enabled, the global initial <b>user mode</b> (configured from"\
					" the identity dialog) will be used. If disabled, you can configure an initial user mode for this server","options"));
#endif
	bool bHasUmode = !(s->initUMode().isEmpty());
	KviStr szDefUMode = KVI_OPTION_STRING(KviOption_stringDefaultUserMode);
	m_pUseDefaultInitUMode->setChecked(!bHasUmode);
	connect(m_pUseDefaultInitUMode,SIGNAL(toggled(bool)),this,SLOT(useDefaultInitUModeToggled(bool)));
	
	m_pIMode = new KviStyledCheckBox(__tr2qs_ctx("Invisible (+i)","options"),gbox);
	m_pIMode->setEnabled(bHasUmode);
	m_pIMode->setChecked(bHasUmode ? s->initUMode().contains('i',false) : szDefUMode.contains('i',false));
	
	m_pSMode = new KviStyledCheckBox(__tr2qs_ctx("Server notices (+s)","options"),gbox);
	m_pSMode->setEnabled(bHasUmode);
	m_pSMode->setChecked(bHasUmode ? s->initUMode().contains('s',false) : szDefUMode.contains('s',false));
	
	m_pWMode = new KviStyledCheckBox(__tr2qs_ctx("Wallops (+w)","options"),gbox);
	m_pWMode->setEnabled(bHasUmode);
	m_pWMode->setChecked(bHasUmode ? s->initUMode().contains('w',false) : szDefUMode.contains('w',false));

	l = new QLabel("",tab);
	gl->addWidget(l,2,0);

	gl->setRowStretch(2,1);
	//gl->setColStretch(1,1);

	tw->addTab(tab,*(g_pIconManager->getSmallIcon(KVI_SMALLICON_WHO)),__tr2qs_ctx("Identity","options"));

	// Connection tab
	
	tab = new QWidget(tw);
	gl = new QGridLayout(tab,11,1,10,4);
	
	l = new QLabel(__tr2qs_ctx("Port:","options"),tab);
	gl->addWidget(l,0,0);
	

	m_pPortEditor = new QLineEdit(tab);
	gl->addWidget(m_pPortEditor,0,1);
	QString tmpz;
	tmpz.setNum(s->port());
	m_pPortEditor->setText(tmpz);
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pPortEditor,__tr2qs_ctx("<center>This is the default <b>port</b> that this server will be contacted on.<br>Usually <b>6667</b> is OK.</center>","options"));
#endif
	connect(m_pPortEditor,SIGNAL(textChanged(const QString &)),this,SLOT(portEditorTextChanged(const QString &)));



	l = new QLabel(__tr2qs_ctx("IP address:","options"),tab);
	gl->addWidget(l,1,0);
	m_pIpEditor = new KviIpEditor(tab,KviIpEditor::IpV4);
	gl->addWidget(m_pIpEditor,1,1);
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pIpEditor,__tr2qs_ctx("<center>This is the <b>IP address</b> of this server, it is here for caching purposes.<br>" \
										"If you leave this field empty, KVIrc will fill it in the first time it connects to the server. " \
										"If you enable the \"cache IP address\" option below, KVIrc will use it as a \"cached result\" " \
										"and avoid looking it up again.</center>","options"));
#endif
#ifdef COMPILE_IPV6_SUPPORT
	m_pIpEditor->setAddressType(s->isIpV6() ? KviIpEditor::IpV6 : KviIpEditor::IpV4);
#else
	m_pIpEditor->setAddressType(KviIpEditor::IpV4);
#endif
	
	if(!m_pIpEditor->setAddress(s->m_szIp))
	{
#ifdef COMPILE_IPV6_SUPPORT
		m_pIpEditor->setAddress(s->isIpV6() ? "0:0:0:0:0:0:0:0" : "0.0.0.0");
#else
		m_pIpEditor->setAddress("0.0.0.0");
#endif
	}
	
	m_pCacheIpCheck = new KviStyledCheckBox(__tr2qs_ctx("Cache IP address","options"),tab);
	gl->addMultiCellWidget(m_pCacheIpCheck,2,2,0,1);
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pCacheIpCheck,__tr2qs_ctx("<center>This check will enable <b>IP address caching</b> for this server:<br>" \
												"DNS lookups can be time-consuming and might be blocking on some platforms; " \
												"this option will cause KVIrc to look up the server hostname only once.<br><br> " \
												"Advanced: you can also use this option to force a certain server name to resolve " \
												"to a fixed ip address when either the dns for that server is temporairly " \
												"unreachable or you want to avoid the round-robin lookups.</center>","options"));
#endif
	m_pCacheIpCheck->setChecked(s->cacheIp());


	m_pUseIPV6Check = new KviStyledCheckBox(__tr2qs_ctx("Use IPv6 protocol","options"),tab);
	gl->addMultiCellWidget(m_pUseIPV6Check,3,3,0,1);
#ifdef COMPILE_IPV6_SUPPORT
	m_pUseIPV6Check->setChecked(s->isIpV6());
#else
	m_pUseIPV6Check->setEnabled(false);
#endif
	connect(m_pUseIPV6Check,SIGNAL(toggled(bool)),this,SLOT(useIPV6CheckToggled(bool)));

#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pUseIPV6Check,__tr2qs_ctx("<center>This check identifies IPv6 servers.<br>If enabled, KVIrc will attempt to use the IPv6 protocol " \
						"(thus your OS <b>must</b> have a working IPv6 stack and you <b>must</b> have an IPv6 connection).</center>","options"));
#endif


	m_pUseSSLCheck = new KviStyledCheckBox(__tr2qs_ctx("Use SSL protocol","options"),tab);
	gl->addMultiCellWidget(m_pUseSSLCheck,4,4,0,1);
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pUseSSLCheck,__tr2qs_ctx("<center>This check will cause the connection to use the <b>Secure Socket Layer</b> " \
						"encryption support. Obviously, this server must have support for this, too. :)</center>","options"));
#endif
#ifndef COMPILE_SSL_SUPPORT
	m_pUseSSLCheck->setEnabled(false);
#endif
	m_pUseSSLCheck->setChecked(s->useSSL());
	
	
	m_pUseAutoConnect = new KviStyledCheckBox(__tr2qs_ctx("Connect to this server at startup","options"),tab);
	m_pUseAutoConnect->setChecked(s->autoConnect());
	
	gl->addMultiCellWidget(m_pUseAutoConnect,5,5,0,1);
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pUseAutoConnect,__tr2qs_ctx("<center>This option will cause KVIrc to connect to the IRC server when it is started.</center>","options"));
#endif
	
	l = new QLabel(__tr2qs_ctx("Encoding:","options"),tab);
	gl->addWidget(l,6,0);
	m_pEncodingEditor = new QComboBox(false,tab);
	m_pEncodingEditor->setDuplicatesEnabled(false);
	gl->addWidget(m_pEncodingEditor,6,1);
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pEncodingEditor,__tr2qs_ctx("<center>This box allows you to choose the preferred encoding for this sever. " \
									"If you choose \"Use Network Encoding\" then the encoding will be inherited from the " \
									"network that this server belongs to.</center>","options"));
#endif

	int i = 0;
	int current = 0;
	KviLocale::EncodingDescription * d = KviLocale::encodingDescription(i);
	QString tmp;
	m_pEncodingEditor->insertItem(__tr2qs_ctx("Use Network Encoding","options"));
	while(d->szName)
	{
		KviQString::sprintf(tmp,"%s (%s)",d->szName,d->szDescription);
		m_pEncodingEditor->insertItem(tmp);
		if(KviQString::equalCI(d->szName,s->encoding()))current = i + 1;
		i = i + 1;
		d = KviLocale::encodingDescription(i);
	}

	m_pEncodingEditor->setCurrentItem(current);


	l = new QLabel(__tr2qs_ctx("Link filter:","options"),tab);
	gl->addWidget(l,7,0);
	m_pLinkFilterEditor = new QComboBox(true,tab);
	m_pLinkFilterEditor->setDuplicatesEnabled(false);
	gl->addWidget(m_pLinkFilterEditor,7,1);

	m_pLinkFilterEditor->insertItem("");

	g_pModuleManager->loadModulesByCaps("linkfilter");
	KviModuleExtensionDescriptorList * mexl = KviModuleExtensionManager::instance()->getExtensionList("linkfilter");



	if(mexl)
	{
		for(KviModuleExtensionDescriptor * d = mexl->first();d;d = mexl->next())
			m_pLinkFilterEditor->insertItem(d->name().ptr());
	} else {
		if(!s->linkFilter().isEmpty())
			m_pLinkFilterEditor->insertItem(s->linkFilter());
	}

	if(!s->linkFilter().isEmpty())
		m_pLinkFilterEditor->setCurrentText(s->linkFilter());
	else
		m_pLinkFilterEditor->setCurrentText("");


#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pLinkFilterEditor,__tr2qs_ctx("<center>This field specifies the name of a module that exports a link filter for this type of server.<br>" \
		"For plain IRC connections, you don't need any link filters; this is used for incompatible protocols.</center>","options"));
#endif

	l = new QLabel(__tr2qs_ctx("Id:","options"),tab);
	gl->addWidget(l,8,0);
	m_pIdEditor = new QLineEdit(tab);
	if(s->id().isEmpty())s->generateUniqueId();
	m_pIdEditor->setText(s->id());
	gl->addWidget(m_pIdEditor,8,1);

#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pIdEditor,__tr2qs_ctx("<center>This field allows you to specify a really unique id for this server. " \
		"You will then be able to use /server -x &lt;this_id&gt; to make the connection. This is especially " \
		"useful when you have multiple server entries with the same hostname and port in different networks (bouncers?)</center>","options"));
#endif


	l = new QLabel(__tr2qs_ctx("Proxy server:","options"),tab);
	gl->addWidget(l,9,0);
	m_pProxyEditor = new QComboBox(tab);
	gl->addWidget(m_pProxyEditor,9,1);
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pProxyEditor,__tr2qs_ctx("<center>This is the <b>proxy</b> that KVIrc will use to connect to thos server.\n" \
										"If this field is set in \"Default\" KVirc will use global proxy settings, if it is set in \"Direct connection\" " \
										"KVirc will connect to this server without proxy. You can define new proxy server in global options' \"Proxy servers\" menu.</center>","options"));
#endif	
	
	m_pProxyEditor->insertItem(__tr2qs_ctx("Default","options"));
	m_pProxyEditor->insertItem(__tr2qs_ctx("Direct connection","options"));	
	
	KviPtrList<KviProxy> * proxylist = g_pProxyDataBase->proxyList();
	for(KviProxy * p = proxylist->first();p;p = proxylist->next())
	{
		m_pProxyEditor->insertItem(QString("%1:%2").arg(p->hostname()).arg(p->port()));
	}
	if(m_pProxyEditor->count() > (s->proxy()+2))
		m_pProxyEditor->setCurrentItem(s->proxy()+2);


	l = new QLabel("",tab);
	gl->addMultiCellWidget(l,10,10,0,1);

	gl->setRowStretch(10,1);

	tw->addTab(tab,*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SOCKETWARNING)),__tr2qs_ctx("Connection","options"));

	// after login join channels
	tab = new QWidget(tw);

	gl = new QGridLayout(tab,1,1,10,4);

	if(s->autoJoinChannelList())
		m_lstChannels = *(s->autoJoinChannelList());
	m_pChannelListSelector = new KviCahnnelListSelector(tab,
		__tr2qs_ctx("Channels to join automatically upon connect:","options"),&m_lstChannels,true);
#ifdef COMPILE_INFO_TIPS
  QToolTip::add(m_pChannelListSelector,__tr2qs_ctx("<center>Here you can set a list of channels to be joined automatically " \
		"after a connection to this server has been established. To add a channel, type its name in the " \
		"text input below and click \"<b>Add</b>\".</center>","options"));
#endif
	gl->addWidget(m_pChannelListSelector,0,0);

	tw->addTab(tab,*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CHANNEL)),__tr2qs_ctx("Join Channels","options"));

	// after connect execute
	tab = new QWidget(tw);
	gl = new QGridLayout(tab,1,1,10,4);


	m_pOnConnectEditor = KviScriptEditor::createInstance(tab);
	gl->addWidget(m_pOnConnectEditor,0,0);
	m_pOnConnectEditor->setText(s->onConnectCommand());
	m_pOnConnectEditor->setMinimumHeight(150);
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pOnConnectEditor,__tr2qs_ctx("<center>The following commands will be executed after a connection has been established.<br>" \
			"<b>Important:</b> Enter commands <b>without</b> a preceding slash (e.g. <tt>quote pass secret</tt> instead of <tt>/quote pass secret</tt>).<br>"\
			"KVIrc will first send the USER command, then eventually PASS and NICK and then execute this " \
			"command sequence.<br>This is particularly useful for IRC bouncers that require login commands.</center>","options"));
#endif

	tw->addTab(tab,__tr2qs_ctx("On Connect","options"));
	// after login execute

	tab = new QWidget(tw);
	gl = new QGridLayout(tab,1,1,10,4);

	m_pOnLoginEditor = KviScriptEditor::createInstance(tab);
	gl->addWidget(m_pOnLoginEditor,0,0);
	m_pOnLoginEditor->setText(s->onLoginCommand());

	m_pOnLoginEditor->setMinimumHeight(150);

#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pOnLoginEditor,
			__tr2qs_ctx("<center>The following commands will be executed after a successful login to this server.<br>" \
					"<b>Important:</b> Enter commands <b>without</b> a preceding slash (e.g. <tt>quote privatelog</tt> instead of <tt>/quote privatelog</tt>).<br>"
					"This is useful for automatically opening queries, setting variables, etc.</center>","options"));
#endif

	tw->addTab(tab,__tr2qs_ctx("On Login","options"));

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
	g->setColStretch(1,1);

	tw->setMinimumWidth(390);
	
	setHeaderLabelText();
}

KviServerDetailsWidget::~KviServerDetailsWidget()
{
	if(m_pOnConnectEditor) KviScriptEditor::destroyInstance(m_pOnConnectEditor);
	if(m_pOnLoginEditor) KviScriptEditor::destroyInstance(m_pOnLoginEditor);
}

void KviServerDetailsWidget::useIPV6CheckToggled(bool)
{
#ifdef COMPILE_IPV6_SUPPORT
	m_pIpEditor->setAddressType(m_pUseIPV6Check->isChecked() ? KviIpEditor::IpV6 : KviIpEditor::IpV4);
#else
	m_pIpEditor->setAddressType(KviIpEditor::IpV4);
#endif
	setHeaderLabelText();
}

void KviServerDetailsWidget::portEditorTextChanged(const QString &)
{
	setHeaderLabelText();
}

void KviServerDetailsWidget::setHeaderLabelText()
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

void KviServerDetailsWidget::fillData(KviIrcServer * s)
{
	s->setUserName(m_pUserEditor->text());
	s->setPassword(m_pPassEditor->text());
	s->setNickName(m_pNickEditor->text());
	s->setRealName(m_pRealEditor->text());
	if(m_pDescEditor)
		s->setDescription(m_pDescEditor->text());
	if(m_pLinkFilterEditor)
		s->setLinkFilter(m_pLinkFilterEditor->lineEdit()->text());
	if(m_pEncodingEditor)
		if(m_pEncodingEditor->currentItem() <= 0)s->m_szEncoding = "";
		else {
			KviLocale::EncodingDescription * d = KviLocale::encodingDescription(m_pEncodingEditor->currentItem() - 1);
			s->m_szEncoding = d->szName;
		}
	s->setIpAddress("");
	if(m_pCacheIpCheck)
		s->setCacheIp(m_pCacheIpCheck->isChecked());
	if(m_pUseSSLCheck)
		s->setUseSSL(m_pUseSSLCheck->isChecked());
	if(m_pIdEditor)
		s->setId(m_pIdEditor->text());
	if(s->id().isEmpty())s->generateUniqueId();

	if(m_pUseDefaultInitUMode)
	{
		KviStr szUMode;
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
	s->m_uPort = uPort;
#ifdef COMPILE_IPV6_SUPPORT
	s->setIpV6(m_pUseIPV6Check->isChecked());
#else
	s->setIpV6(false);
#endif

	if(m_pIpEditor)
	{
		KviStr tmpAddr = m_pIpEditor->address();
	
		if(!m_pIpEditor->hasEmptyFields())
		{
#ifdef COMPILE_IPV6_SUPPORT
			if(s->isIpV6())
			{
				if((!kvi_strEqualCI(tmpAddr.ptr(),"0:0:0:0:0:0:0:0")) && kvi_isValidStringIp_V6(tmpAddr.ptr()))
				{
					s->setIpAddress(tmpAddr.ptr());
				} else {
					s->setCacheIp(false);
					s->setIpAddress("");
				}
			} else {
#endif
				if((!kvi_strEqualCI(tmpAddr.ptr(),"0.0.0.0")) && kvi_isValidStringIp(tmpAddr.ptr()))
				{
					s->setIpAddress(tmpAddr.ptr());
				} else {
					s->setCacheIp(false);
					s->setIpAddress("");
				}
#ifdef COMPILE_IPV6_SUPPORT
			}
#endif
		} else {
			s->setCacheIp(false);
			s->setIpAddress("");
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
		s->setProxy(m_pProxyEditor->currentItem()-2);
}

void KviServerDetailsWidget::useDefaultInitUModeToggled(bool b)
{
	m_pIMode->setEnabled(!b);
	m_pWMode->setEnabled(!b);
	m_pSMode->setEnabled(!b);
}

// kvi_app.cpp
extern KVIRC_API KviIrcServerDataBase * g_pIrcServerDataBase;

KviServerOptionsListViewItem::KviServerOptionsListViewItem(KviTalListView *parent,const QPixmap &pm,const KviIrcNetwork *n)
    : KviTalListViewItem(parent)
{
	setPixmap(0,pm);
	m_pServerData = 0;
	m_pNetworkData = new KviIrcNetwork(*n);
	setText(0,n->name());
	setText(1,n->description());
}

KviServerOptionsListViewItem::KviServerOptionsListViewItem(KviTalListViewItem *parent,const QPixmap &pm,const KviIrcServer *s)
    : KviTalListViewItem(parent)
{
	setPixmap(0,pm);
	m_pServerData = new KviIrcServer(*s);
	setText(0,s->hostName());
	setText(1,s->description());
	m_pNetworkData = 0;
}

KviServerOptionsListViewItem::~KviServerOptionsListViewItem()
{
	if(m_pServerData)delete m_pServerData;
	if(m_pNetworkData)delete m_pNetworkData;
}

void KviServerOptionsListViewItem::updateVisibleStrings()
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








KviServerOptionsWidget::KviServerOptionsWidget(QWidget * parent)
    : KviOptionsWidget(parent,"server_options_widget")
{
	createLayout(4,2);

	m_pContextPopup = new KviTalPopupMenu(this);
	m_pImportPopup = new KviTalPopupMenu(this);

	connect(m_pImportPopup,SIGNAL(aboutToShow()),this,SLOT(importPopupAboutToShow()));
	connect(m_pImportPopup,SIGNAL(activated(int)),this,SLOT(importPopupActivated(int)));

	m_pServerDetailsDialog = 0;
	m_pNetworkDetailsDialog = 0;
	m_pImportFilter = 0;

	m_pListView = new KviTalListView(this);
	addWidgetToLayout(m_pListView,0,0,0,0);
	m_pListView->addColumn(__tr2qs_ctx("Server","options"));
	m_pListView->addColumn(__tr2qs_ctx("Description","options"));
	m_pListView->setRootIsDecorated(true);
	m_pListView->setAllColumnsShowFocus(true);
	m_pListView->setSelectionMode(KviTalListView::Single);
	connect(m_pListView,SIGNAL(selectionChanged(KviTalListViewItem *)),
		this,SLOT(listViewItemSelectionChanged(KviTalListViewItem *)));
	connect(m_pListView,SIGNAL(rightButtonPressed(KviTalListViewItem *,const QPoint &,int)),
		this,SLOT(listViewRightButtonPressed(KviTalListViewItem *,const QPoint &,int)));
	connect(m_pListView,SIGNAL(doubleClicked(KviTalListViewItem*, const QPoint&, int )),
		this,SLOT(detailsClicked()));

#ifdef COMPILE_INFO_TIPS
	QString tiptxt = __tr2qs_ctx("<center>This is the list of available IRC servers.<br>" \
							"Right-click on the list to add or remove servers and perform other actions.<br>"\
							"Double-click on a item for advanced options.</center>","options");
	QToolTip::add(m_pListView,tiptxt);
	QToolTip::add(m_pListView->viewport(),tiptxt);
#endif

	KviTalVBox * vbox = new KviTalVBox(this);
	addWidgetToLayout(vbox,1,0,1,0);

	m_pNewNetworkButton = new KviStyledToolButton(vbox);
	m_pNewNetworkButton->setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_WORLD)));
	m_pNewNetworkButton->setAutoRaise(true);
	connect(m_pNewNetworkButton,SIGNAL(clicked()),this,SLOT(newNetwork()));
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pNewNetworkButton,__tr2qs_ctx("New Network","options"));
#endif

	m_pNewServerButton = new KviStyledToolButton(vbox);
	m_pNewServerButton->setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SERVER)));
	m_pNewServerButton->setAutoRaise(true);
	connect(m_pNewServerButton,SIGNAL(clicked()),this,SLOT(newServer()));
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pNewServerButton,__tr2qs_ctx("New Server","options"));
#endif

	m_pRemoveButton = new KviStyledToolButton(vbox);
	m_pRemoveButton->setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CUT)));
	m_pRemoveButton->setEnabled(false);
	m_pRemoveButton->setAutoRaise(true);
	connect(m_pRemoveButton,SIGNAL(clicked()),this,SLOT(removeCurrent()));
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pRemoveButton,__tr2qs_ctx("Remove Network/Server","options"));
#endif

	QFrame * f = new QFrame(vbox);
	f->setFrameStyle(QFrame::Sunken | QFrame::HLine);

	m_pCopyServerButton = new KviStyledToolButton(vbox);
	m_pCopyServerButton->setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_COPY)));
	m_pCopyServerButton->setEnabled(false);
	m_pCopyServerButton->setAutoRaise(true);
	connect(m_pCopyServerButton,SIGNAL(clicked()),this,SLOT(copyServer()));
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pCopyServerButton,__tr2qs_ctx("Copy Server","options"));
#endif

	m_pPasteServerButton = new KviStyledToolButton(vbox);
	m_pPasteServerButton->setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_PASTE)));
	m_pPasteServerButton->setEnabled(false);
	m_pPasteServerButton->setAutoRaise(true);
	connect(m_pPasteServerButton,SIGNAL(clicked()),this,SLOT(pasteServer()));
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pPasteServerButton,__tr2qs_ctx("Paste Server","options"));
#endif

	f = new QFrame(vbox);
	f->setFrameStyle(QFrame::Sunken | QFrame::HLine);

	m_pImportButton = new KviStyledToolButton(vbox);
	m_pImportButton->setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_FOLDER)));
	m_pImportButton->setAutoRaise(true);
	m_pImportButton->setPopup(m_pImportPopup);
	m_pImportButton->setPopupDelay(1);
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pImportButton,__tr2qs_ctx("Import List","options"));
#endif

	QFrame * lll = new QFrame(vbox);
	vbox->setStretchFactor(lll,100);
	
	KviTalGroupBox *gbox = addGroupBox(0,1,1,1,3,KviTalGroupBox::Horizontal,__tr2qs_ctx("Active Configuration","options"));
	m_pSrvNetLabel = new QLabel(__tr2qs_ctx("Server:","options"),gbox);

	m_pSrvNetEdit = new QLineEdit(gbox);
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pSrvNetEdit,__tr2qs_ctx("<center>This is the name of the currently selected server or network</center>","options"));
#endif

/*
	m_pIpV6Check = new KviStyledCheckBox(__tr2qs_ctx("Use IPv6 protocol","options"),gbox);

#ifndef COMPILE_IPV6_SUPPORT
	m_pIpV6Check->setEnabled(false);
#endif

#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pIpV6Check,__tr2qs_ctx("<center>This check identifies IPv6 servers.<br>If enabled, KVIrc will attempt to use the IPv6 protocol " \
						"(thus your OS <b>must</b> have a working IPv6 stack and you <b>must</b> have an IPv6 connection).</center>","options"));
#endif

	m_pPortLabel = new QLabel(__tr2qs_ctx("Port:","options"),gbox);

	m_pPortEdit = new QLineEdit(gbox);
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pPortEdit,__tr2qs_ctx("<center>This is the default <b>port</b> that this server will be contacted on.<br>Usually <b>6667</b> is OK.</center>","options"));
#endif
*/

	m_pDetailsButton = new QPushButton(__tr2qs_ctx("Advanced...","options"),gbox);
	connect(m_pDetailsButton,SIGNAL(clicked()),this,SLOT(detailsClicked()));
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pDetailsButton,__tr2qs_ctx("<center>Click here to edit advanced options for this entry</center>","options"));
#endif

	m_pConnectCurrent = new QPushButton(__tr2qs_ctx("Connect &Now","options"),this);
	addWidgetToLayout(m_pConnectCurrent,0,2,0,2);
	connect(m_pConnectCurrent,SIGNAL(clicked()),this,SLOT(connectCurrentClicked()));

#ifdef COMPILE_INFO_TIPS
	QToolTip::add(m_pConnectCurrent,__tr2qs_ctx("<center>Hit this button to connect to the currently selected server.</center>","options"));
#endif

	m_pRecentPopup = new KviTalPopupMenu(this);
	connect(m_pRecentPopup,SIGNAL(aboutToShow()),this,SLOT(recentServersPopupAboutToShow()));
	connect(m_pRecentPopup,SIGNAL(activated(int)),this,SLOT(recentServersPopupClicked(int)));

	QToolButton * tb = new KviStyledToolButton(this);
	addWidgetToLayout(tb,1,2,1,2);
	tb->setPixmap(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_TIME)));
	tb->setPopup(m_pRecentPopup);
	tb->setPopupDelay(1);
#ifdef COMPILE_INFO_TIPS
	QToolTip::add(tb,__tr2qs_ctx("<center>This button shows a list of recently used servers. It allows you to quickly find them in the list.</center>","options"));
#endif


	KviBoolSelector * b = addBoolSelector(0,3,1,3,__tr2qs_ctx("Show this dialog at startup","options"),KviOption_boolShowServersConnectDialogOnStart);

#ifdef COMPILE_INFO_TIPS
	QToolTip::add(b,__tr2qs_ctx("<center>If this option is enabled, the Servers dialog will appear every time you start KVIrc</center>","options"));
#endif

	// KviBoolSelector * c = addBoolSelector(0,6,2,6,__tr("Close after connection","options"),KviOption_boolCloseServerWidgetAfterConnect);

	m_pLastEditedItem = 0;

	m_pClipboard = 0;

	fillServerList();

	layout()->setRowStretch(0,1);
	//layout()->setColStretch(1,5);
	//layout()->setColStretch(2,2);
	layout()->setColStretch(0,1);
	setMinimumWidth(320);
}

KviServerOptionsWidget::~KviServerOptionsWidget()
{
	if(m_pImportFilter)
	{
		disconnect(m_pImportFilter,0,this,0);
		m_pImportFilter->die();
		m_pImportFilter = 0;
	}

	if(m_pClipboard)delete m_pClipboard;
	if(m_pServerDetailsDialog)delete m_pServerDetailsDialog;
	if(m_pNetworkDetailsDialog)delete m_pNetworkDetailsDialog;
}

void KviServerOptionsWidget::recentServersPopupAboutToShow()
{
	g_pApp->fillRecentServersPopup(m_pRecentPopup);
}

void KviServerOptionsWidget::recentServersPopupClicked(int id)
{
	KviStr data = m_pRecentPopup->text(id);
	data.cutToFirst('/');
	while(data.firstCharIs('/'))data.cutLeft(1);
	data.cutFromLast(':');
	// we should have a full server name now , with no port
	
	KviStr port = m_pRecentPopup->text(id);
	port.cutToLast(':');
	bool bOk;
	kvi_u32_t uPort = port.toUInt(&bOk);
	// we have the port too

	KviTalListViewItem * pFoundNet = 0;
	KviTalListViewItem * pFoundSrv = 0;

	for(KviTalListViewItem * net = m_pListView->firstChild();net;net = net->nextSibling())
	{
		for(KviTalListViewItem * srv = net->firstChild();srv;srv = srv->nextSibling())
		{
			KviStr tmp = ((KviServerOptionsListViewItem *)srv)->m_pServerData->hostName();
			if(kvi_strEqualCI(tmp.ptr(),data.ptr()))
			{
				if(((KviServerOptionsListViewItem *)srv)->m_pServerData->port() == uPort)
				{
					net->setOpen(true);
					m_pListView->setCurrentItem(srv);
					m_pListView->ensureItemVisible(srv);
					return;
				} else {
					if(!pFoundNet)
					{
						// the port doesn't match.. check for
						// another entry with the correct port
						// but keep track of this entry in case we don't find it
						pFoundNet = srv;
						pFoundSrv = net;
					}
				}
			}
		}
	}
	
	// fallback to the server with the wrong port
	if(pFoundNet)
	{
		pFoundNet->setOpen(true);
		m_pListView->setCurrentItem(pFoundSrv);
		m_pListView->ensureItemVisible(pFoundSrv);
	}
}


void KviServerOptionsWidget::connectCurrentClicked()
{
	saveLastItem();
	commit();
	KviStr szServer = m_pSrvNetEdit->text();
	if(szServer.isEmpty())return;
	KviKvsScript::run("server -u",g_pApp->activeConsole());
}


void KviServerOptionsWidget::fillServerList()
{
	KviServerOptionsListViewItem * net;
	KviServerOptionsListViewItem * srv;
	KviServerOptionsListViewItem * cur = 0;

	KviDictIterator<KviIrcServerDataBaseRecord> it(*(g_pIrcServerDataBase->recordDict()));

	while(KviIrcServerDataBaseRecord * r = it.current())
	{
		net = new KviServerOptionsListViewItem(m_pListView,*(g_pIconManager->getSmallIcon(KVI_SMALLICON_WORLD)),r->network());
		KviPtrList<KviIrcServer> * sl = r->serverList();
		bool bCurrent = r->network()->name() == g_pIrcServerDataBase->currentNetworkName().utf8().data();
		net->setOpen(bCurrent);
		for(KviIrcServer * s = sl->first();s;s = sl->next())
		{
			srv = new KviServerOptionsListViewItem(net,*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SERVER)),s);

			if((s == r->currentServer()) && bCurrent)
			{
				m_pListView->setSelected(srv,true);
				cur = srv;
			}
		}
		++it;
	}
	if(cur)m_pListView->ensureItemVisible(cur);
}

void KviServerOptionsWidget::listViewItemSelectionChanged(KviTalListViewItem *it)
{
	saveLastItem();
	m_pLastEditedItem = (KviServerOptionsListViewItem *)it;

	if(m_pLastEditedItem)
	{
		m_pSrvNetLabel->setEnabled(true);
		m_pSrvNetEdit->setEnabled(true);
		m_pDetailsButton->setEnabled(true);
		m_pConnectCurrent->setEnabled(m_pLastEditedItem->m_pServerData);

		m_pRemoveButton->setEnabled(true);
		m_pCopyServerButton->setEnabled(m_pLastEditedItem->m_pServerData);

		if(m_pLastEditedItem->m_pServerData)
		{
			m_pSrvNetLabel->setText(__tr2qs_ctx("Server:","options"));
			m_pSrvNetEdit->setText(m_pLastEditedItem->m_pServerData->m_szHostname);
		} else {
			m_pSrvNetLabel->setText(__tr2qs_ctx("Network:","options"));
			m_pSrvNetEdit->setText(it->text(0));
		}
	} else {
		m_pRemoveButton->setEnabled(false);
		m_pCopyServerButton->setEnabled(true);
		
		m_pConnectCurrent->setEnabled(false);
		m_pSrvNetLabel->setEnabled(false);
		m_pSrvNetEdit->setEnabled(false);
		m_pSrvNetEdit->setText(__tr2qs_ctx("No selection","options"));
		m_pDetailsButton->setEnabled(false);
	}
}



void KviServerOptionsWidget::saveLastItem()
{
	if(!m_pLastEditedItem)return;
	if(m_pLastEditedItem->m_pServerData)
	{
		KviStr tmp = m_pSrvNetEdit->text();
		if(tmp.isEmpty())tmp = "irc.unknown.net";
		m_pLastEditedItem->m_pServerData->m_szHostname = tmp;
		m_pLastEditedItem->updateVisibleStrings();
	} else if(m_pLastEditedItem->m_pNetworkData)
	{
		QString tmp = m_pSrvNetEdit->text();
		if(tmp.isEmpty())tmp = __tr2qs_ctx("UnknownNet","options");
		m_pLastEditedItem->m_pNetworkData->setName(tmp);
		m_pLastEditedItem->updateVisibleStrings();
	}
}

void KviServerOptionsWidget::commit()
{
	saveLastItem();
	g_pIrcServerDataBase->clear();
	KviServerOptionsListViewItem * it = (KviServerOptionsListViewItem *)m_pListView->firstChild();
	while(it)
	{
		QString tmp = it->m_pNetworkData ? it->m_pNetworkData->name() : QString::null;
		if(!tmp.isEmpty())
		{
			KviIrcNetwork * net = 0;
			KviIrcServerDataBaseRecord * r = g_pIrcServerDataBase->findRecord(tmp);
			if(r)
			{
				net = r->network();
				net->copyFrom(*(it->m_pNetworkData));
			} else {
				net = new KviIrcNetwork(tmp);
				net->copyFrom(*(it->m_pNetworkData));
				r = g_pIrcServerDataBase->insertNetwork(net);
			}
			if(it == m_pLastEditedItem)g_pIrcServerDataBase->setCurrentNetwork(net->name());

			KviServerOptionsListViewItem * ch = (KviServerOptionsListViewItem *)it->firstChild();
			KviIrcServer *srv;
			
			while(ch)
			{
				if(ch->m_pServerData)
				{
					if(!ch->m_pServerData->m_szHostname.isEmpty())
					{
						srv = r->findServer(ch->m_pServerData);
						if(!srv)
						{
							srv = new KviIrcServer(*(ch->m_pServerData));
							r->insertServer(srv);
						} else *srv = *(ch->m_pServerData);
						if(srv->id().isEmpty())srv->generateUniqueId();
						if(ch == m_pLastEditedItem)
						{
							g_pIrcServerDataBase->setCurrentNetwork(net->name());
							r->setCurrentServer(srv);
						}
					}
				}
				ch = (KviServerOptionsListViewItem *)ch->nextSibling();
			}
		}
		it = (KviServerOptionsListViewItem *)it->nextSibling();
	}

	KviOptionsWidget::commit();
}

void KviServerOptionsWidget::listViewRightButtonPressed(KviTalListViewItem *it,const QPoint &pnt,int col)
{
	int id;
	bool bServer = (it && ((KviServerOptionsListViewItem *)it)->m_pServerData);
	m_pContextPopup->clear();
	m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_WORLD)),__tr2qs_ctx("New Network","options"),this,SLOT(newNetwork()));
	id = m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CUT)),__tr2qs_ctx("Remove Network","options"),this,SLOT(removeCurrent()));
	m_pContextPopup->setItemEnabled(id,!bServer);
	m_pContextPopup->insertSeparator();
	id = m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SERVER)),__tr2qs_ctx("&New Server","options"),this,SLOT(newServer()));
	id = m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CUT)),__tr2qs_ctx("Re&move Server","options"),this,SLOT(removeCurrent()));
	m_pContextPopup->setItemEnabled(id,bServer);
	id = m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_COPY)),__tr2qs_ctx("&Copy Server","options"),this,SLOT(copyServer()));
	m_pContextPopup->setItemEnabled(id,bServer);
	id = m_pContextPopup->insertItem(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_PASTE)),__tr2qs_ctx("&Paste Server","options"),this,SLOT(pasteServer()));
	m_pContextPopup->setItemEnabled(id,m_pClipboard);

	m_pContextPopup->insertSeparator();
	//	m_pContextPopup->insertItem(__c2q(__tr("Merge list from server.ini","options")),this,SLOT(importFromIni()));
	m_pContextPopup->insertItem(__tr2qs_ctx("Clear List","options"),this,SLOT(clearList()));
	m_pContextPopup->insertSeparator();
	m_pContextPopup->insertItem(__tr2qs_ctx("Import List","options"),m_pImportPopup);
	m_pContextPopup->popup(QCursor::pos());
}

void KviServerOptionsWidget::importPopupAboutToShow()
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

void KviServerOptionsWidget::importPopupActivated(int id)
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

	connect(m_pImportFilter,SIGNAL(server(const KviIrcServer &,const char *)),this,SLOT(importServer(const KviIrcServer &,const char *)));
	connect(m_pImportFilter,SIGNAL(destroyed()),this,SLOT(importerDead()));

	m_pImportFilter->start();
}

void KviServerOptionsWidget::importerDead()
{
	m_pImportFilter = 0;
}

void KviServerOptionsWidget::importServer(const KviIrcServer &s,const char * network)
{
	KviServerOptionsListViewItem * net = findNetItem(network);
	if(!net)
	{
		KviIrcNetwork d(network);
		net = new KviServerOptionsListViewItem(m_pListView,*(g_pIconManager->getSmallIcon(KVI_SMALLICON_WORLD)),&d);
		net->setOpen(true);
	}

	for(KviServerOptionsListViewItem * srv = (KviServerOptionsListViewItem *)net->firstChild();srv;srv = (KviServerOptionsListViewItem *)srv->nextSibling())
	{
		if((srv)->m_pServerData->useSSL() == s.useSSL())
		{
			if(srv->m_pServerData->isIpV6() == s.isIpV6())
			{
				if(KviQString::equalCI(srv->m_pServerData->hostName(),s.hostName()))
				{
					// update the port
					srv->m_pServerData->setPort(s.port());
					if(!s.ipAddress().isEmpty())srv->m_pServerData->setIpAddress(s.ipAddress());
					if(!s.password().isEmpty())srv->m_pServerData->setPassword(s.password());
					if(!s.nickName().isEmpty())srv->m_pServerData->setNickName(s.nickName());
					m_pListView->setCurrentItem(srv);
					m_pListView->ensureItemVisible(srv);
					return;
				}
			}
		}
	}

	// not found : add it!
	KviServerOptionsListViewItem * newServer = new KviServerOptionsListViewItem(net,*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SERVER)),&s);
	m_pListView->setCurrentItem(newServer);
	m_pListView->ensureItemVisible(newServer);
}

void KviServerOptionsWidget::newNetwork()
{
	KviIrcNetwork d(__tr2qs_ctx("New Network","options"));
	KviServerOptionsListViewItem * it = new KviServerOptionsListViewItem(m_pListView,*(g_pIconManager->getSmallIcon(KVI_SMALLICON_WORLD)),&d);
	it->setOpen(true);
	m_pListView->setSelected(it,true);
	m_pListView->ensureItemVisible(it);
}

void KviServerOptionsWidget::newServer()
{
	if(m_pLastEditedItem)
	{
		KviServerOptionsListViewItem * net;
		if(m_pLastEditedItem->m_pServerData)
		{
			net = (KviServerOptionsListViewItem *)m_pLastEditedItem->parent();
			if(!net)return;
		}
		else net = m_pLastEditedItem;

		KviIrcServer tmp;
		tmp.m_szHostname = __tr2qs_ctx("irc.unknown.net","options");
		tmp.setCacheIp(false);
		tmp.generateUniqueId();

		KviServerOptionsListViewItem * it = new KviServerOptionsListViewItem(net,
									*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SERVER)),&tmp);

		net->setOpen(true);

		m_pListView->setSelected(it,true);
		m_pListView->ensureItemVisible(it);
	}
}

void KviServerOptionsWidget::copyServer()
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

void KviServerOptionsWidget::pasteServer()
{
	if(m_pClipboard)
	{
		if(m_pLastEditedItem)
		{
			KviServerOptionsListViewItem * net;
			if(m_pLastEditedItem->m_pServerData)
			{
				net = (KviServerOptionsListViewItem *)m_pLastEditedItem->parent();
				if(!net)return;
			}
			else net = m_pLastEditedItem;

			KviServerOptionsListViewItem * it = new KviServerOptionsListViewItem(net,
							*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SERVER)),m_pClipboard);

			net->setOpen(true);

			m_pListView->setSelected(it,true);
			m_pListView->ensureItemVisible(it);
		}
	}
}

void KviServerOptionsWidget::removeCurrent()
{
	if(m_pLastEditedItem)
	{
		KviTalListViewItem * it = m_pLastEditedItem->itemAbove();
		if(!it)it = m_pLastEditedItem->firstChild() ? m_pLastEditedItem->nextSibling() : m_pLastEditedItem->itemBelow();
		delete m_pLastEditedItem;
		m_pLastEditedItem = 0;
		if(!it)it = m_pListView->firstChild();
		if(it)
		{
			m_pListView->setSelected(it,true);
			m_pListView->ensureItemVisible(it);
		}
	}
}

KviServerOptionsListViewItem * KviServerOptionsWidget::findNetItem(const char * netname)
{
	KviServerOptionsListViewItem * it = (KviServerOptionsListViewItem *)m_pListView->firstChild();
	while(it)
	{
		KviStr szNetName = it->text(0);
		if(kvi_strEqualCI(szNetName.ptr(),netname))return it;
		it = (KviServerOptionsListViewItem *)it->nextSibling();
	}
	return 0;
}

void KviServerOptionsWidget::clearList()
{
	m_pListView->clear();
	m_pLastEditedItem = 0;
	listViewItemSelectionChanged(0);
}

void KviServerOptionsWidget::detailsClicked()
{
	if(!m_pLastEditedItem)return;
	if(m_pLastEditedItem->m_pServerData)
	{
		saveLastItem();

		m_pServerDetailsDialog = new KviServerDetailsWidget(this,m_pLastEditedItem->m_pServerData);

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

		m_pNetworkDetailsDialog = new KviNetworkDetailsWidget(this,m_pLastEditedItem->m_pNetworkData);

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

#include "m_optw_servers.moc"
