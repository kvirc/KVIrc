//=============================================================================
//
//   File : kvi_kvs_treenode_specialcommanddefpopup.cpp
//   Created on Fri 02 Jan 2004 14:09:00 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004 Szymon Stefanek <pragma at kvirc dot net>
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

#define __KVIRC__

#include "kvi_kvs_treenode_specialcommanddefpopup.h"
#include "kvi_kvs_treenode_expression.h"
#include "kvi_kvs_treenode_instruction.h"
#include "kvi_kvs_runtimecontext.h"
#include "kvi_locale.h"
#include "kvi_kvs_popupmanager.h"
#include "kvi_kvs_popupmenu.h"
#include "kvi_kvs_script.h"

void KviKvsTreeNodeSpecialCommandDefpopupLabelExtpopup::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Label \"extpopup\" For Special Command \"defpopup\"";
#endif
}

void KviKvsTreeNodeSpecialCommandDefpopupLabelExtpopup::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s SpecialCommandDefpopupLabelExtpopup",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	QString x = tmp;
	x += "CONDITION: ";
	x += m_szCondition;
	debug("%s",x.utf8().data());
	x = tmp;
	x += "TEXT: ";
	x += m_szText;
	debug("%s",x.utf8().data());
	x = tmp;
	x += "NAME: ";
	x += m_szName;
	debug("%s",x.utf8().data());
	x = tmp;
	x += "ICON: ";
	x += m_szIcon;
	debug("%s",x.utf8().data());
#endif
}

bool KviKvsTreeNodeSpecialCommandDefpopupLabelExtpopup::execute(KviKvsRunTimeContext * c,KviKvsPopupMenu * p)
{
#ifdef COMPILE_NEW_KVS
	p->addExtPopup(m_szItemName,m_szName,m_szText,m_szIcon,m_szCondition);
#endif
	return true;
}


void KviKvsTreeNodeSpecialCommandDefpopupLabelItem::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Label \"item\" For Special Command \"defpopup\"";
#endif
}


void KviKvsTreeNodeSpecialCommandDefpopupLabelItem::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s SpecialCommandDefpopupLabelItem",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	QString x = tmp;
	x += "CONDITION: ";
	x += m_szCondition;
	debug("%s",x.utf8().data());
	x = tmp;
	x += "TEXT: ";
	x += m_szText;
	debug("%s",x.utf8().data());
	x = tmp;
	x += "ICON: ";
	x += m_szIcon;
	debug("%s",x.utf8().data());
	x = tmp;
	x += "INSTRUCTION: ";
	x += m_szInstruction;
	debug("%s",x.utf8().data());
#endif
}

bool KviKvsTreeNodeSpecialCommandDefpopupLabelItem::execute(KviKvsRunTimeContext * c,KviKvsPopupMenu * p)
{
#ifdef COMPILE_NEW_KVS
	p->addItem(m_szItemName,m_szInstruction,m_szText,m_szIcon,m_szCondition);
#endif
	return true;
}

void KviKvsTreeNodeSpecialCommandDefpopupLabelLabel::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Label \"label\" For Special Command \"defpopup\"";
#endif
}


void KviKvsTreeNodeSpecialCommandDefpopupLabelLabel::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s SpecialCommandDefpopupLabelLabel",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	QString x = tmp;
	x += "CONDITION: ";
	x += m_szCondition;
	debug("%s",x.utf8().data());
	x = tmp;
	x += "TEXT: ";
	x += m_szText;
	debug("%s",x.utf8().data());
	x = tmp;
	x += "ICON: ";
	x += m_szIcon;
	debug("%s",x.utf8().data());
#endif
}

bool KviKvsTreeNodeSpecialCommandDefpopupLabelLabel::execute(KviKvsRunTimeContext * c,KviKvsPopupMenu * p)
{
#ifdef COMPILE_NEW_KVS
	p->addLabel(m_szItemName,m_szText,m_szIcon,m_szCondition);
#endif
	return true;
}




void KviKvsTreeNodeSpecialCommandDefpopupLabelSeparator::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Label \"separator\" For Special Command \"defpopup\"";
#endif
}


void KviKvsTreeNodeSpecialCommandDefpopupLabelSeparator::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s SpecialCommandDefpopupLabelSeparator",prefix);
	QString tmp = prefix;
	tmp.append("  CONDITION:");
	tmp.append(m_szCondition);
	debug("%s",tmp.utf8().data());
#endif
}

bool KviKvsTreeNodeSpecialCommandDefpopupLabelSeparator::execute(KviKvsRunTimeContext * c,KviKvsPopupMenu * p)
{
#ifdef COMPILE_NEW_KVS
	p->addSeparator(m_szItemName,m_szCondition);
#endif
	return true;
}



void KviKvsTreeNodeSpecialCommandDefpopupLabelEpilogue::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Label \"epilogue\" For Special Command \"defpopup\"";
#endif
}


void KviKvsTreeNodeSpecialCommandDefpopupLabelEpilogue::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s SpecialCommandDefpopupLabelEpilogue",prefix);
	QString tmp = prefix;
	tmp.append("  INSTRUCTION: ");
	tmp += m_szInstruction;
	debug("%s",tmp.utf8().data());
#endif
}

bool KviKvsTreeNodeSpecialCommandDefpopupLabelEpilogue::execute(KviKvsRunTimeContext * c,KviKvsPopupMenu * p)
{
#ifdef COMPILE_NEW_KVS
	p->addEpilogue(m_szItemName,m_szInstruction);
#endif
	return true;
}


void KviKvsTreeNodeSpecialCommandDefpopupLabelPrologue::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Label \"prologue\" For Special Command \"defpopup\"";
#endif
}



void KviKvsTreeNodeSpecialCommandDefpopupLabelPrologue::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s SpecialCommandDefpopupLabelPrologue",prefix);
	QString tmp = prefix;
	tmp.append("  INSTRUCTION: ");
	tmp += m_szInstruction;
	debug("%s",tmp.utf8().data());
#endif
}

bool KviKvsTreeNodeSpecialCommandDefpopupLabelPrologue::execute(KviKvsRunTimeContext * c,KviKvsPopupMenu * p)
{
#ifdef COMPILE_NEW_KVS
	p->addPrologue(m_szItemName,m_szInstruction);
#endif
	return true;
}









KviKvsTreeNodeSpecialCommandDefpopupLabelPopup::KviKvsTreeNodeSpecialCommandDefpopupLabelPopup(const QChar * pLocation)
: KviKvsTreeNodeSpecialCommandDefpopupConditionalLabelWithTextAndIcon(pLocation,QString::null,QString::null,QString::null,QString::null)
{
#ifdef COMPILE_NEW_KVS
	m_pLabels = new KviPointerList<KviKvsTreeNodeSpecialCommandDefpopupLabel>;
	m_pLabels->setAutoDelete(true);
#endif
}

KviKvsTreeNodeSpecialCommandDefpopupLabelPopup::~KviKvsTreeNodeSpecialCommandDefpopupLabelPopup()
{
#ifdef COMPILE_NEW_KVS
	delete m_pLabels;
#endif
}

void KviKvsTreeNodeSpecialCommandDefpopupLabelPopup::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Label \"popup\" For Special Command \"defpopup\"";
#endif
}


void KviKvsTreeNodeSpecialCommandDefpopupLabelPopup::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s SpecialCommandDefpopupLabelPopup",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	QString x = tmp;
	x += "CONDITION: ";
	x += m_szCondition;
	debug(x.utf8().data());
	x = tmp;
	x += "TEXT: ";
	x += m_szText;
	debug(x.utf8().data());
	x = tmp;
	x += "ICON: ";
	x += m_szIcon;
	debug(x.utf8().data());
	for(KviKvsTreeNodeSpecialCommandDefpopupLabel * l = m_pLabels->first();l;l = m_pLabels->next())
		l->dump(tmp.utf8().data());
#endif
}

void KviKvsTreeNodeSpecialCommandDefpopupLabelPopup::addLabel(KviKvsTreeNodeSpecialCommandDefpopupLabel * pLabel)
{
#ifdef COMPILE_NEW_KVS
	pLabel->setParent(this);
	m_pLabels->append(pLabel);
#endif
}

bool KviKvsTreeNodeSpecialCommandDefpopupLabelPopup::execute(KviKvsRunTimeContext * c,KviKvsPopupMenu * p)
{
#ifdef COMPILE_NEW_KVS
	// the name will be reset anyway
	KviKvsPopupMenu * pNew = p->addPopup(m_szItemName,m_szText,m_szIcon,m_szCondition);
	return fill(c,pNew);
#else
	return false;
#endif
}

bool KviKvsTreeNodeSpecialCommandDefpopupLabelPopup::fill(KviKvsRunTimeContext* c,KviKvsPopupMenu * p)
{
#ifdef COMPILE_NEW_KVS
	for(KviKvsTreeNodeSpecialCommandDefpopupLabel * l = m_pLabels->first();l;l = m_pLabels->next())
	{
		if(!l->execute(c,p))return false;
	}
#endif
	return true;
}








KviKvsTreeNodeSpecialCommandDefpopup::KviKvsTreeNodeSpecialCommandDefpopup(const QChar * pLocation,KviKvsTreeNodeData * pPopupName,KviKvsTreeNodeSpecialCommandDefpopupLabelPopup * pMainPopup)
: KviKvsTreeNodeSpecialCommand(pLocation,"defpopup")
{
#ifdef COMPILE_NEW_KVS
	m_pPopupName = pPopupName;
	m_pPopupName->setParent(this);
	m_pMainPopup = pMainPopup;
	m_pMainPopup->setParent(this);
#endif
}

KviKvsTreeNodeSpecialCommandDefpopup::~KviKvsTreeNodeSpecialCommandDefpopup()
{
#ifdef COMPILE_NEW_KVS
	delete m_pPopupName;
	delete m_pMainPopup;
#endif
}

void KviKvsTreeNodeSpecialCommandDefpopup::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Special Command \"defpopup\"";
#endif
}


void KviKvsTreeNodeSpecialCommandDefpopup::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s SpecialCommandDefpopup",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	m_pPopupName->dump(tmp.utf8().data());
	m_pMainPopup->dump(tmp.utf8().data());
#endif
}

bool KviKvsTreeNodeSpecialCommandDefpopup::execute(KviKvsRunTimeContext * c)
{
#ifdef COMPILE_NEW_KVS
	KviKvsVariant v;
	if(!m_pPopupName->evaluateReadOnly(c,&v))return false;
	QString szName;
	v.asString(szName);

	KviKvsSwitchList swl;
	if(m_pSwitches)
	{
		if(!(m_pSwitches->evaluate(c,&swl)))return false;
	}

	KviKvsPopupMenu * pPopup = KviKvsPopupManager::instance()->lookup(szName);
	bool bWasAlreadyThere;
	if(!pPopup)
	{
		if(m_pMainPopup->isEmpty())return true; // we wanted to remove it anyway: exit silently
		// we want to create it
		pPopup = new KviKvsPopupMenu(szName);
		KviKvsPopupManager::instance()->add(szName,pPopup);
		bWasAlreadyThere = false;
	} else {	
		bWasAlreadyThere = true;

		if(pPopup->isLocked())
		{
			c->error(__tr2qs("The popup '%s' is actually locked: 'self-modifications' are not allowed"),&szName);
			return false;
		}

		if(m_pMainPopup->isEmpty())
		{
			// we want to remove it
			KviKvsPopupManager::instance()->remove(szName);
			return true;
		}
		// we want to (re)create it

		if(!swl.find('m',"--merge"))pPopup->doClear();
	}


	if(!m_pMainPopup->fill(c,pPopup))
	{
		if(!bWasAlreadyThere)KviKvsPopupManager::instance()->remove(szName);
		return false;
	}
#endif
	return true;
}
