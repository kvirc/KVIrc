//=============================================================================
//
//   File : OptionsWidget_nickserv.cpp
//   Creation date : Fri Aug 10 2001 03:38:10 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "OptionsWidget_nickserv.h"

#include "KviQString.h"
#include "KviOptions.h"
#include "KviLocale.h"
#include "KviNickServRuleSet.h"
#include "KviIrcMask.h"
#include "KviTalToolTip.h"
#include <QTreeWidget>

#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

// KviApplication.cpp
extern KVIRC_API KviNickServRuleSet * g_pNickServRuleSet;

NickServRuleEditor::NickServRuleEditor(QWidget * par, bool bUseServerMaskField)
    : QDialog(par)
{
	setWindowTitle(__tr2qs_ctx("NickServ Authentication Rule - KVIrc", "options"));

	QGridLayout * gl = new QGridLayout(this); //,bUseServerMaskField ? 7 : 6,4,10,5);

	QLabel * l = new QLabel(__tr2qs_ctx("Registered nickname:", "options"), this);
	gl->addWidget(l, 0, 0);

	m_pRegisteredNickEdit = new QLineEdit(this);
	KviTalToolTip::add(m_pRegisteredNickEdit, __tr2qs_ctx("Put here the nickname that you have registered with NickServ", "options"));
	gl->addWidget(m_pRegisteredNickEdit, 0, 1, 1, 3);
	//	gl->addMultiCellWidget(m_pRegisteredNickEdit,0,0,1,3);

	l = new QLabel(__tr2qs_ctx("NickServ mask:", "options"), this);
	gl->addWidget(l, 1, 0);

	m_pNickServMaskEdit = new QLineEdit(this);
	// xgettext:no-c-format
	KviTalToolTip::add(m_pNickServMaskEdit, __tr2qs_ctx("This is the mask that NickServ must match to be correctly identified as the NickServ service. "
	                                                    "This usually will be something like <b>NickServ!service@services.dalnet</b>.<br>"
	                                                    "You can use wildcards for this field, but generally it is a security flaw. "
	                                                    "If you're 100% sure that NO user on the network can use the nickname \"NickServ\", "
	                                                    "the mask <b>NickServ!*@*</b> may be safe to use in this field.", "options"));

	gl->addWidget(m_pNickServMaskEdit, 1, 1, 1, 3);
	//	gl->addMultiCellWidget(m_pNickServMaskEdit,1,1,1,3);

	l = new QLabel(__tr2qs_ctx("Message regexp:", "options"), this);
	gl->addWidget(l, 2, 0);

	m_pMessageRegexpEdit = new QLineEdit(this);
	gl->addWidget(m_pMessageRegexpEdit, 2, 1, 1, 3);
	//	gl->addMultiCellWidget(m_pMessageRegexpEdit,2,2,1,3);

	KviTalToolTip::add(m_pMessageRegexpEdit, __tr2qs_ctx("This is the simple regular expression that the identification request message "
	                                                     "from NickServ must match in order to be correctly recognized.<br>"
	                                                     "The message is usually something like \"To identify yourself please use /ns IDENTIFY password\" "
	                                                     "and it is sent when the NickServ wants you to authenticate yourself. "
	                                                     "You can use the * and ? wildcards.", "options"));
	l = new QLabel(__tr2qs_ctx("Identify command:", "options"), this);
	gl->addWidget(l, 3, 0);

	m_pIdentifyCommandEdit = new QLineEdit(this);
	KviTalToolTip::add(m_pIdentifyCommandEdit, __tr2qs_ctx("This is the command that will be executed when NickServ requests authentication "
	                                                       "for the nickname described in this rule (if the both server and NickServ mask are matched). "
	                                                       "This usually will be something like <b>msg NickServ identify &lt;yourpassword&gt;</b>.<br>"
	                                                       "You can use <b>msg -q</b> if you don't want the password echoed on the screen. "
	                                                       "Please note that there is no leading slash in this command.", "options"));
	gl->addWidget(m_pIdentifyCommandEdit, 3, 1, 1, 3);
	//	gl->addMultiCellWidget(m_pIdentifyCommandEdit,3,3,1,3);

	int iNextLine = 4;

	if(bUseServerMaskField)
	{
		l = new QLabel(__tr2qs_ctx("Server mask:", "options"), this);
		gl->addWidget(l, 4, 0);

		m_pServerMaskEdit = new QLineEdit(this);
		KviTalToolTip::add(m_pServerMaskEdit, __tr2qs_ctx("This is the mask that the current server must match in order "
		                                                  "for this rule to apply. It can contain * and ? wildcards.<br>Do NOT use simply \"*\" here...", "options"));

		gl->addWidget(m_pServerMaskEdit, 4, 1, 1, 3);
		//		gl->addMultiCellWidget(m_pServerMaskEdit,4,4,1,3);
		iNextLine++;
	}
	else
	{
		m_pServerMaskEdit = nullptr;
	}

	l = new QLabel(__tr2qs_ctx("Hint: Move the mouse cursor over the fields to get help", "options"), this);
	l->setMargin(10);
	gl->addWidget(l, iNextLine, 0, 1, 4);
	//	gl->addMultiCellWidget(l,iNextLine,iNextLine,0,3);

	iNextLine++;

	QPushButton * p = new QPushButton(__tr2qs_ctx("Cancel", "options"), this);
	p->setMinimumWidth(100);
	connect(p, SIGNAL(clicked()), this, SLOT(reject()));
	gl->addWidget(p, iNextLine, 2);

	m_pOkButton = new QPushButton(__tr2qs_ctx("OK", "options"), this);
	m_pOkButton->setMinimumWidth(100);
	m_pOkButton->setDefault(true);
	connect(m_pOkButton, SIGNAL(clicked()), this, SLOT(okPressed()));
	gl->addWidget(m_pOkButton, iNextLine, 3);

	gl->setColumnStretch(1, 1);
	gl->setRowStretch(bUseServerMaskField ? 5 : 4, 1);

	setMinimumWidth(250);
}

NickServRuleEditor::~NickServRuleEditor()
    = default;

bool NickServRuleEditor::validate()
{
	QString s = m_pRegisteredNickEdit->text();

	QString m = __tr2qs_ctx("Invalid NickServ Rule - KVIrc", "options");
	QString o = __tr2qs_ctx("OK", "options");

	if(s.isEmpty())
	{
		QMessageBox::warning(this, m, __tr2qs_ctx("The nickname field can't be empty!", "options"), o);
		return false;
	}

	if(s.indexOf(QChar(' ')) != -1)
	{
		QMessageBox::warning(this, m, __tr2qs_ctx("The nickname field can't contain spaces!", "options"), o);
		return false;
	}

	s = m_pNickServMaskEdit->text();

	if(s.isEmpty())
	{
		QMessageBox::warning(this, m, __tr2qs_ctx("The NickServ mask can't be empty!<br>You must put at least * there.", "options"), o);
		return false;
	}

	s = m_pMessageRegexpEdit->text();

	if(s.isEmpty())
	{
		QMessageBox::warning(this, m, __tr2qs_ctx("The message regexp can't be empty!<br>You must put at least * there.", "options"), o);
		return false;
	}

	s = m_pIdentifyCommandEdit->text();

	if(s.isEmpty())
	{
		QMessageBox::warning(this, m, __tr2qs_ctx("The IDENTIFY command can't be empty!", "options"), o);
		return false;
	}

	return true;
}

void NickServRuleEditor::okPressed()
{
	if(!validate())
		return;
	accept();
}

bool NickServRuleEditor::editRule(KviNickServRule * r)
{
	m_pRegisteredNickEdit->setText(r->registeredNick());
	m_pNickServMaskEdit->setText(r->nickServMask().isEmpty() ? QString("NickServ!*@*") : r->nickServMask());
	m_pMessageRegexpEdit->setText(r->messageRegexp().isEmpty() ? QString("*IDENTIFY*") : r->messageRegexp());
	m_pIdentifyCommandEdit->setText(r->identifyCommand().isEmpty() ? QString("raw -q NickServ IDENTIFY <password>") : r->identifyCommand());
	if(m_pServerMaskEdit)
		m_pServerMaskEdit->setText(r->serverMask().isEmpty() ? QString("irc.yourserver.org") : r->serverMask());
	m_pRegisteredNickEdit->selectAll();
	if(exec() != QDialog::Accepted)
		return false;
	r->setRegisteredNick(m_pRegisteredNickEdit->text());
	r->setNickServMask(m_pNickServMaskEdit->text());
	r->setMessageRegexp(m_pMessageRegexpEdit->text());
	r->setIdentifyCommand(m_pIdentifyCommandEdit->text());
	if(m_pServerMaskEdit)
		r->setServerMask(m_pServerMaskEdit->text());
	return true;
}

OptionsWidget_nickServ::OptionsWidget_nickServ(QWidget * parent)
    : KviOptionsWidget(parent)
{
	createLayout();
	setObjectName("nickserv_options_widget");

	QGridLayout * gl = layout();

	KviNickServRuleSet * rs = g_pNickServRuleSet;
	bool bNickServEnabled = rs ? (rs->isEnabled() && !rs->isEmpty()) : false;

	m_pNickServCheck = new QCheckBox(__tr2qs_ctx("Enable NickServ identification", "options"), this);
	gl->addWidget(m_pNickServCheck, 0, 0, 1, 3);
	//	gl->addMultiCellWidget(m_pNickServCheck,0,0,0,2);
	KviTalToolTip::add(m_pNickServCheck, __tr2qs_ctx("This check enables the automatic identification with NickServ", "options"));
	m_pNickServCheck->setChecked(bNickServEnabled);

	m_pNickServTreeWidget = new QTreeWidget(this);
	m_pNickServTreeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pNickServTreeWidget->setAllColumnsShowFocus(true);
	QStringList columnLabels;

	columnLabels.append(__tr2qs_ctx("Nickname", "options"));
	columnLabels.append(__tr2qs_ctx("Server Mask", "options"));
	columnLabels.append(__tr2qs_ctx("NickServ Mask", "options"));
	columnLabels.append(__tr2qs_ctx("NickServ Request Mask", "options"));
	columnLabels.append(__tr2qs_ctx("Identify Command", "options"));
	m_pNickServTreeWidget->setHeaderLabels(columnLabels);
	m_pNickServTreeWidget->setColumnWidth(0, 150);
	m_pNickServTreeWidget->setColumnWidth(1, 150);
	m_pNickServTreeWidget->setColumnWidth(2, 150);
	m_pNickServTreeWidget->setColumnWidth(3, 150);
	m_pNickServTreeWidget->setColumnWidth(4, 150);
	connect(m_pNickServTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(enableDisableNickServControls()));

	gl->addWidget(m_pNickServTreeWidget, 1, 0, 1, 3);
	//	gl->addMultiCellWidget(m_pNickServTreeWidget,1,1,0,2);
	KviTalToolTip::add(m_pNickServTreeWidget, __tr2qs_ctx("This is a list of NickServ identification rules. "
	                                                      "KVIrc will use them to model its automatic interaction with NickServ on all the networks.<br>"
	                                                      "Please be aware that this feature can cause your NickServ passwords to be stolen "
	                                                      "if used improperly. Make sure that you fully understand the NickServ authentication protocol.<br>"
	                                                      "In other words, be sure to know what you're doing.<br>"
	                                                      "Also note that the password that you provide is stored as <b>PLAIN TEXT</b>.<br>"
	                                                      "KVIrc supports also per-network NickServ authentication rules that can be "
	                                                      "created in the \"Advanced...\" network options (accessible from the servers dialog).", "options"));

	m_pAddRuleButton = new QPushButton(__tr2qs_ctx("Add Rule", "options"), this);
	connect(m_pAddRuleButton, SIGNAL(clicked()), this, SLOT(addNickServRule()));
	gl->addWidget(m_pAddRuleButton, 2, 0);

	m_pEditRuleButton = new QPushButton(__tr2qs_ctx("Edit Rule", "options"), this);
	connect(m_pEditRuleButton, SIGNAL(clicked()), this, SLOT(editNickServRule()));
	gl->addWidget(m_pEditRuleButton, 2, 1);

	m_pDelRuleButton = new QPushButton(__tr2qs_ctx("Delete Rule", "options"), this);
	connect(m_pDelRuleButton, SIGNAL(clicked()), this, SLOT(delNickServRule()));
	gl->addWidget(m_pDelRuleButton, 2, 2);

	connect(m_pNickServCheck, SIGNAL(toggled(bool)), this, SLOT(enableDisableNickServControls()));

	if(rs && rs->rules())
	{
		QTreeWidgetItem * it;
		KviPointerList<KviNickServRule> * ll = rs->rules();
		for(KviNickServRule * rule = ll->first(); rule; rule = ll->next())
		{
			it = new QTreeWidgetItem(m_pNickServTreeWidget);
			it->setText(0, rule->registeredNick());
			it->setText(1, rule->serverMask());
			it->setText(2, rule->nickServMask());
			it->setText(3, rule->messageRegexp());
			it->setText(4, rule->identifyCommand());
		}
	}

	enableDisableNickServControls();

	gl->setRowStretch(1, 1);
}

OptionsWidget_nickServ::~OptionsWidget_nickServ()
    = default;

void OptionsWidget_nickServ::editNickServRule()
{
	QTreeWidgetItem * it = (QTreeWidgetItem *)m_pNickServTreeWidget->currentItem();
	if(!it)
		return;
	KviNickServRule r(it->text(0), it->text(2), it->text(3), it->text(4), it->text(1));
	NickServRuleEditor ed(this, true);
	if(ed.editRule(&r))
	{
		it->setText(0, r.registeredNick());
		it->setText(1, r.serverMask());
		it->setText(2, r.nickServMask());
		it->setText(3, r.messageRegexp());
		it->setText(4, r.identifyCommand());
	}
}

void OptionsWidget_nickServ::addNickServRule()
{
	KviNickServRule r;
	NickServRuleEditor ed(this, true);
	if(ed.editRule(&r))
	{
		QTreeWidgetItem * it = new QTreeWidgetItem(m_pNickServTreeWidget);
		it->setText(0, r.registeredNick());
		it->setText(1, r.serverMask());
		it->setText(2, r.nickServMask());
		it->setText(3, r.messageRegexp());
		it->setText(4, r.identifyCommand());
	}
}

void OptionsWidget_nickServ::delNickServRule()
{
	QTreeWidgetItem * it = (QTreeWidgetItem *)m_pNickServTreeWidget->currentItem();
	if(!it)
		return;
	delete it;
	enableDisableNickServControls();
}

void OptionsWidget_nickServ::enableDisableNickServControls()
{
	bool bEnabled = m_pNickServCheck->isChecked();
	m_pNickServTreeWidget->setEnabled(bEnabled);
	m_pAddRuleButton->setEnabled(bEnabled);
	bEnabled = bEnabled && (m_pNickServTreeWidget->topLevelItemCount()) && m_pNickServTreeWidget->currentItem();
	m_pDelRuleButton->setEnabled(bEnabled);
	m_pEditRuleButton->setEnabled(bEnabled);
}

void OptionsWidget_nickServ::commit()
{
	g_pNickServRuleSet->clear();
	if(m_pNickServTreeWidget->topLevelItemCount())
	{
		g_pNickServRuleSet->setEnabled(m_pNickServCheck->isChecked());
		QTreeWidgetItem * it; // = m_pNickServTreeWidget->firstChild();
		for(int i = 0; i < m_pNickServTreeWidget->topLevelItemCount(); i++)
		//		while(it)
		{
			it = (QTreeWidgetItem *)m_pNickServTreeWidget->topLevelItem(i);
			g_pNickServRuleSet->addRule(KviNickServRule::createInstance(it->text(0), it->text(2), it->text(3), it->text(4), it->text(1)));
			//it = it->nextSibling();
		}
	}
	KviOptionsWidget::commit();
}
