//=============================================================================
//
//   File : kvi_kvs_popupmenu.cpp
//   Creation date : Wed 07 Jan 2004 05:02:57 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2008 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_kvs_popupmenu.h"
#include "kvi_locale.h"
#include "kvi_iconmanager.h"
#include "kvi_out.h"
#include "kvi_app.h"
#include "kvi_kvs_popupmanager.h"
#include "kvi_console.h"
#include "kvi_config.h"
#include "kvi_cmdformatter.h"
#include "kvi_options.h"

// popup names

// rootname : the root popup
//       rootname.anonymousmenuX : child popups with no name
//       rootname.externalrootname : child popups copied from external menus
//       rootname.itemX : child items
//       rootname.separatorX : child separators
//       rootname.labelX : child labels


KviKvsPopupMenuItem::KviKvsPopupMenuItem(Type t,const QString &szItemName,const QString &szCondition)
{
	m_szItemName = szItemName;
	m_eType = t;
	if(szCondition.isEmpty())
	{
		// true by default
		m_pKvsCondition = 0;
	} else {
		QString szName = "condition callback for ";
		szName += szItemName;
		m_pKvsCondition = new KviKvsScript(szName,szCondition,KviKvsScript::Expression);
	}
}

KviKvsPopupMenuItem::KviKvsPopupMenuItem(Type t,const QString &szItemName,const KviKvsScript * pCondition)
{
	m_szItemName = szItemName;
	m_eType = t;
	if(!pCondition)
	{
		// true by default
		m_pKvsCondition = 0;
	} else {
		m_pKvsCondition = new KviKvsScript(*pCondition);
	}
}


KviKvsPopupMenuItem::~KviKvsPopupMenuItem()
{
	if(m_pKvsCondition)delete m_pKvsCondition;
}

void KviKvsPopupMenuItem::clear()
{
}

KviKvsScript * KviKvsPopupMenuItem::kvsIcon()
{
	return 0;
}

KviKvsScript * KviKvsPopupMenuItem::kvsText()
{
	return 0;
}

KviKvsScript * KviKvsPopupMenuItem::kvsCode()
{
	return 0;
}

bool KviKvsPopupMenuItem::evaluateCondition(KviKvsPopupMenuTopLevelData * pData)
{
	if(!m_pKvsCondition)return true;
	KviKvsVariant vRet;

	if(!m_pKvsCondition->run(pData->window(),
			pData->parameters(),
			&vRet,
			KviKvsScript::PreserveParams,
			pData->extendedRunTimeData()))
	{
		// broken condition
		pData->window()->output(KVI_OUT_PARSERWARNING,__tr2qs_ctx("Broken condition in menu setup: assuming false","kvs"));
		return false;
	}
	return vRet.asBoolean();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

KviKvsPopupMenuItemSeparator::KviKvsPopupMenuItemSeparator(const QString &szItemName,const QString &szCondition)
: KviKvsPopupMenuItem(KviKvsPopupMenuItem::Separator,szItemName,szCondition)
{
}

KviKvsPopupMenuItemSeparator::KviKvsPopupMenuItemSeparator(const QString &szItemName,const KviKvsScript * pCondition)
: KviKvsPopupMenuItem(KviKvsPopupMenuItem::Separator,szItemName,pCondition)
{
}

KviKvsPopupMenuItemSeparator::~KviKvsPopupMenuItemSeparator()
{
}

void KviKvsPopupMenuItemSeparator::fill(KviKvsPopupMenu * pMenu,KviKvsPopupMenuTopLevelData * pData,int)
{
	if(!evaluateCondition(pData))return;
	pMenu->insertSeparator();
}

KviKvsPopupMenuItem * KviKvsPopupMenuItemSeparator::clone() const
{
	return new KviKvsPopupMenuItemSeparator(m_szItemName,m_pKvsCondition);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////


KviKvsPopupMenuItemWithTextAndIcon::KviKvsPopupMenuItemWithTextAndIcon(KviKvsPopupMenuItem::Type t,const QString &szItemName,const QString &szText,const QString &szIcon,const QString &szCondition)
: KviKvsPopupMenuItem(t,szItemName,szCondition)
{
	QString szName = "text callback for ";
	szName += szItemName;
	m_pKvsText = new KviKvsScript(szName,szText,KviKvsScript::Parameter);

	if(szIcon.isEmpty())
	{
		m_pKvsIcon = 0;
	} else {
		szName = "icon callback for ";
		szName += szItemName;
		m_pKvsIcon = new KviKvsScript(szName,szIcon,KviKvsScript::Parameter);
	}
}

KviKvsPopupMenuItemWithTextAndIcon::KviKvsPopupMenuItemWithTextAndIcon(KviKvsPopupMenuItem::Type t,const QString &szItemName,const KviKvsScript * pText,const KviKvsScript * pIcon,const KviKvsScript * pCondition)
: KviKvsPopupMenuItem(t,szItemName,pCondition)
{
	if(pText)
	{
		m_pKvsText = new KviKvsScript(*pText);
	} else {
		// hum.. this should never happen anyway
		QString szName = "text callback for ";
		szName += szItemName;
		m_pKvsText = new KviKvsScript(szName,"",KviKvsScript::Parameter);
	}

	if(!pIcon)
	{
		m_pKvsIcon = 0;
	} else {
		m_pKvsIcon = new KviKvsScript(*pIcon);
	}
}


KviKvsPopupMenuItemWithTextAndIcon::~KviKvsPopupMenuItemWithTextAndIcon()
{
	delete m_pKvsText;
	if(m_pKvsIcon)delete m_pKvsIcon;
}


KviKvsScript * KviKvsPopupMenuItemWithTextAndIcon::kvsIcon()
{
	return m_pKvsIcon;
}

KviKvsScript * KviKvsPopupMenuItemWithTextAndIcon::kvsText()
{
	return m_pKvsText;
}

QPixmap * KviKvsPopupMenuItemWithTextAndIcon::evaluateIcon(KviKvsPopupMenuTopLevelData * pData)
{
	if(!KVI_OPTION_BOOL(KviOption_boolShowIconsInPopupMenus))return 0;
	if(!m_pKvsIcon)return 0;

	KviKvsVariant vRet;
	if(!m_pKvsIcon->run(pData->window(),
			pData->parameters(),
			&vRet,
			KviKvsScript::PreserveParams,
			pData->extendedRunTimeData()))
	{
		// broken text
		pData->window()->output(KVI_OUT_PARSERWARNING,__tr2qs_ctx("Broken icon parameter: ignoring","kvs"));
		return 0;
	}

	QString szRet;
	vRet.asString(szRet);

	QPixmap * p = g_pIconManager->getImage(szRet);
	if(!p)pData->window()->output(KVI_OUT_PARSERWARNING,__tr2qs_ctx("Can't find the icon '%Q': ignoring","kvs"),&szRet);
	return p;
}

QString KviKvsPopupMenuItemWithTextAndIcon::evaluateText(KviKvsPopupMenuTopLevelData *pData)
{
	QString szRet;
	if(!m_pKvsText)return szRet;
	KviKvsVariant vRet;
	if(!m_pKvsText->run(pData->window(),
			pData->parameters(),
			&vRet,
			KviKvsScript::PreserveParams,
			pData->extendedRunTimeData()))
	{
		// broken text
		pData->window()->output(KVI_OUT_PARSERWARNING,__tr2qs_ctx("Broken text parameter: assuming empty string","kvs"));
		return szRet;
	}
	vRet.asString(szRet);
	return szRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


KviKvsPopupMenuItemLabelHelper::KviKvsPopupMenuItemLabelHelper(KviKvsPopupMenuItemLabel * pItem)
: QObject()
{
	m_pItem = pItem;
}

KviKvsPopupMenuItemLabelHelper::~KviKvsPopupMenuItemLabelHelper()
{
}

void KviKvsPopupMenuItemLabelHelper::labelDestroyed()
{
	m_pItem->labelDestroyed();
}

KviKvsPopupMenuItemLabel::KviKvsPopupMenuItemLabel(const QString &szItemName,const QString &szText,const QString &szIcon,const QString &szCondition)
: KviKvsPopupMenuItemWithTextAndIcon(KviKvsPopupMenuItem::Label,szItemName,szText,szIcon,szCondition)
{
	m_pLabel = 0;
	m_pSignalRelay = new KviKvsPopupMenuItemLabelHelper(this);
}

KviKvsPopupMenuItemLabel::KviKvsPopupMenuItemLabel(const QString &szItemName,const KviKvsScript * pText,const KviKvsScript * pIcon,const KviKvsScript * pCondition)
: KviKvsPopupMenuItemWithTextAndIcon(KviKvsPopupMenuItem::Label,szItemName,pText,pIcon,pCondition)
{
	m_pLabel = 0;
	m_pSignalRelay = new KviKvsPopupMenuItemLabelHelper(this);
}


KviKvsPopupMenuItemLabel::~KviKvsPopupMenuItemLabel()
{
	if(m_pLabel)
	{
		QObject::disconnect(m_pLabel,SIGNAL(destroyed()),m_pSignalRelay,SLOT(labelDestroyed()));
		delete m_pLabel;
	}
	delete m_pSignalRelay;
}

void KviKvsPopupMenuItemLabel::labelDestroyed()
{
	m_pLabel = 0; // someone (maybe qt, maybe us) has destroyed the label
}


KviKvsPopupMenuItem * KviKvsPopupMenuItemLabel::clone() const
{
	return new KviKvsPopupMenuItemLabel(m_szItemName,m_pKvsText,m_pKvsIcon,m_pKvsCondition);
}

void KviKvsPopupMenuItemLabel::clear()
{
	if(m_pLabel)
	{
		QObject::disconnect(m_pLabel,SIGNAL(destroyed()),m_pSignalRelay,SLOT(labelDestroyed()));
		delete m_pLabel;
		m_pLabel = 0;
	}
}

void KviKvsPopupMenuItemLabel::fill(KviKvsPopupMenu * pMenu,KviKvsPopupMenuTopLevelData * pData,int)
{
	if(!evaluateCondition(pData))return;
	QString szText = evaluateText(pData);
	QPixmap * pPix = evaluateIcon(pData);
	if(m_pLabel)
	{
		QObject::disconnect(m_pLabel,SIGNAL(destroyed()),m_pSignalRelay,SLOT(labelDestroyed()));
		delete m_pLabel;
	}
	m_pLabel = new QLabel(szText,pMenu);
	QObject::connect(m_pLabel,SIGNAL(destroyed()),m_pSignalRelay,SLOT(labelDestroyed()));

	QPalette p;
	m_pLabel->setStyleSheet("background-color: " + p.color(QPalette::Normal, QPalette::Mid).name());

	if(pPix)m_pLabel->setPixmap(*pPix);
	pMenu->insertItem(m_pLabel);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////

KviKvsPopupMenuItemItem::KviKvsPopupMenuItemItem(const QString &szItemName,const QString &szCode,const QString &szText,const QString &szIcon,const QString &szCondition)
: KviKvsPopupMenuItemWithTextAndIcon(KviKvsPopupMenuItem::Item,szItemName,szText,szIcon,szCondition)
{
	QString szName = "click callback for ";
	szName += szItemName;
	m_pKvsCode = new KviKvsScript(szName,szCode);
}

KviKvsPopupMenuItemItem::KviKvsPopupMenuItemItem(const QString &szItemName,const KviKvsScript * pCode,const KviKvsScript * pText,const KviKvsScript * pIcon,const KviKvsScript * pCondition)
: KviKvsPopupMenuItemWithTextAndIcon(KviKvsPopupMenuItem::Item,szItemName,pText,pIcon,pCondition)
{
	m_pKvsCode = new KviKvsScript(*pCode);
}


KviKvsPopupMenuItemItem::~KviKvsPopupMenuItemItem()
{
	delete m_pKvsCode;
}

void KviKvsPopupMenuItemItem::fill(KviKvsPopupMenu * pMenu,KviKvsPopupMenuTopLevelData * pData,int iIdx)
{
	if(!evaluateCondition(pData))return;
	QString szText = evaluateText(pData);
	QPixmap * pPix = evaluateIcon(pData);
	int id;
	if(pPix)id = pMenu->insertItem(*pPix,szText);
	else id = pMenu->insertItem(szText);
	pMenu->setItemParameter(id,iIdx);
}

KviKvsPopupMenuItem * KviKvsPopupMenuItemItem::clone() const
{
	return new KviKvsPopupMenuItemItem(m_szItemName,m_pKvsCode,m_pKvsText,m_pKvsIcon,m_pKvsCondition);
}

KviKvsScript * KviKvsPopupMenuItemItem::kvsCode()
{
	return m_pKvsCode;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////

KviKvsPopupMenuItemMenu::KviKvsPopupMenuItemMenu(const QString &szItemName,KviKvsPopupMenu * pMenu,const QString &szText,const QString &szIcon,const QString &szCondition)
: KviKvsPopupMenuItemWithTextAndIcon(KviKvsPopupMenuItem::Menu,szItemName,szText,szIcon,szCondition)
{
	m_pMenu = pMenu;
}

KviKvsPopupMenuItemMenu::KviKvsPopupMenuItemMenu(const QString &szItemName,KviKvsPopupMenu * pMenu,const KviKvsScript * pText,const KviKvsScript * pIcon,const KviKvsScript * pCondition)
: KviKvsPopupMenuItemWithTextAndIcon(KviKvsPopupMenuItem::Menu,szItemName,pText,pIcon,pCondition)
{
	m_pMenu = pMenu;
}


KviKvsPopupMenuItemMenu::~KviKvsPopupMenuItemMenu()
{
	delete m_pMenu;
}

KviKvsPopupMenuItem * KviKvsPopupMenuItemMenu::clone() const
{
	KviKvsPopupMenu * copy = new KviKvsPopupMenu(m_pMenu->objectName().toLatin1());
	copy->copyFrom(m_pMenu);
	return new KviKvsPopupMenuItemMenu(m_szItemName,copy,m_pKvsText,m_pKvsIcon,m_pKvsCondition);
}


void KviKvsPopupMenuItemMenu::fill(KviKvsPopupMenu * pMenu,KviKvsPopupMenuTopLevelData * pData,int iIdx)
{
	if(!evaluateCondition(pData))return;
	QString szText = evaluateText(pData);
	QPixmap * pPix = evaluateIcon(pData);
	int id;
	m_pMenu->setParentPopup(pMenu);
	if(pPix)id = pMenu->insertItem(*pPix,szText,m_pMenu);
	else id = pMenu->insertItem(szText,m_pMenu);
	pMenu->setItemParameter(id,iIdx);
}

void KviKvsPopupMenuItemMenu::clear()
{
	m_pMenu->clearMenuContents();
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////


KviKvsPopupMenuItemExtMenu::KviKvsPopupMenuItemExtMenu(const QString &szItemName,const QString &szMenuName,const QString &szText,const QString &szIcon,const QString &szCondition)
: KviKvsPopupMenuItemWithTextAndIcon(KviKvsPopupMenuItem::ExtMenu,szItemName,szText,szIcon,szCondition)
{
	m_szMenuName = szMenuName;
	if(m_szMenuName[0]=='"' && m_szMenuName[(int)(m_szMenuName.length()-1)]=='"')
	{
		m_szMenuName.remove(0,1);
		m_szMenuName.remove(m_szMenuName.length()-1,1);
	}
	m_pMenu = 0;
}

KviKvsPopupMenuItemExtMenu::KviKvsPopupMenuItemExtMenu(const QString &szItemName,const QString &szMenuName,const KviKvsScript * pText,const KviKvsScript * pIcon,const KviKvsScript * pCondition)
: KviKvsPopupMenuItemWithTextAndIcon(KviKvsPopupMenuItem::ExtMenu,szItemName,pText,pIcon,pCondition)
{
	m_szMenuName = szMenuName;
	if(m_szMenuName[0]=='"' && m_szMenuName[(int)(m_szMenuName.length()-1)]=='"')
	{
		m_szMenuName.remove(0,1);
		m_szMenuName.remove(m_szMenuName.length()-1,1);
	}
	m_pMenu = 0;
}


KviKvsPopupMenuItemExtMenu::~KviKvsPopupMenuItemExtMenu()
{
	if(m_pMenu)delete m_pMenu;
}

void KviKvsPopupMenuItemExtMenu::clear()
{
	if(m_pMenu)
	{
		delete m_pMenu;
		m_pMenu = 0;
	}
}

KviKvsPopupMenuItem * KviKvsPopupMenuItemExtMenu::clone() const
{
	return new KviKvsPopupMenuItemExtMenu(m_szItemName,m_szMenuName,m_pKvsText,m_pKvsIcon,m_pKvsCondition);
}


void KviKvsPopupMenuItemExtMenu::fill(KviKvsPopupMenu * pMenu,KviKvsPopupMenuTopLevelData * pData,int iIdx)
{
	if(!evaluateCondition(pData))return;
	QString szText = evaluateText(pData);
	QPixmap * pPix = evaluateIcon(pData);

	KviKvsPopupMenu * source = KviKvsPopupManager::instance()->lookup(m_szMenuName);
	if(source)
	{
		if(source->isLocked())
		{
			pData->window()->output(KVI_OUT_PARSERWARNING,__tr2qs_ctx("Recursive definition detected for popup '%Q': ignoring","kvs"),&(pMenu->popupName()));
			return;
		}
		QString tmp;
		KviQString::sprintf(tmp,"%Q.%Q",&(pMenu->popupName()),&m_szMenuName);
		if(m_pMenu)delete m_pMenu;
		m_pMenu = new KviKvsPopupMenu(tmp);
		m_pMenu->copyFrom(source);
		m_pMenu->setParentPopup(pMenu);
		int id;
		if(pPix)id = pMenu->insertItem(*pPix,szText,m_pMenu);
		else id = pMenu->insertItem(szText,m_pMenu);
		pMenu->setItemParameter(id,iIdx);
	} else {
		pData->window()->output(KVI_OUT_PARSERWARNING,__tr2qs_ctx("Can't find the external popup '%Q': ignoring","kvs"),&m_szMenuName);
	}
}


KviKvsPopupMenuTopLevelData::KviKvsPopupMenuTopLevelData(KviKvsVariantList * pParameters,KviWindow * pWindow)
{
	m_pExtendedRunTimeData = new KviKvsExtendedRunTimeData(new KviKvsHash(),TRUE);
	m_pParameters = pParameters;
	m_pWindow = pWindow;
	m_bTestMode = false;
	m_bLocked = false;
}

KviKvsPopupMenuTopLevelData::~KviKvsPopupMenuTopLevelData()
{
	delete m_pExtendedRunTimeData;
	delete m_pParameters;
}


KviKvsPopupMenu::KviKvsPopupMenu(const QString &szName)
:KviTalPopupMenu(0,szName)
{
	m_szName = szName;
	m_pItemList = new KviPointerList<KviKvsPopupMenuItem>;
	m_pItemList->setAutoDelete(true);
	m_pPrologues = new KviPointerList<KviKvsScript>;
	m_pPrologues->setAutoDelete(true);
	m_pEpilogues = new KviPointerList<KviKvsScript>;
	m_pEpilogues->setAutoDelete(true);
	m_pParentPopup = 0;
	m_pTopLevelData = 0;
	m_pTempTopLevelData = 0;
	m_bSetupDone = false;
	connect(this,SIGNAL(activated(int)),this,SLOT(itemClicked(int)));
	connect(this,SIGNAL(aboutToShow()),this,SLOT(setupMenuContents()));
}


KviKvsPopupMenu::~KviKvsPopupMenu()
{
	clearMenuContents();
	delete m_pItemList;
	delete m_pPrologues;
	delete m_pEpilogues;
	if(m_pTopLevelData)delete m_pTopLevelData;
	if(m_pTempTopLevelData)delete m_pTempTopLevelData;
}


void KviKvsPopupMenu::copyFrom(const KviKvsPopupMenu * src)
{
	doClear();

	for(KviKvsScript * se = src->m_pEpilogues->first();se;se = src->m_pEpilogues->next())
	{
		m_pEpilogues->append(new KviKvsScript(*se));
	}

	for(KviKvsScript * sp = src->m_pPrologues->first();sp;sp = src->m_pPrologues->next())
	{
		m_pPrologues->append(new KviKvsScript(*sp));
	}


	for(const KviKvsPopupMenuItem * it = src->m_pItemList->first();it;it = src->m_pItemList->next())
	{
		addItemInternal(it->clone());
	}
}

void KviKvsPopupMenu::addPrologue(const QString &szItemName,const QString &szCode)
{
	// FIXME: translate this or not ?
	QString szName = szItemName;
	if(szName.isEmpty())
		KviQString::sprintf(szName,"prologue_%u_for_%Q",m_pPrologues->count(),&m_szName);
	m_pPrologues->append(new KviKvsScript(szName,szCode));
}

void KviKvsPopupMenu::addEpilogue(const QString &szItemName,const QString &szCode)
{
	// FIXME: translate this or not ?
	QString szName = szItemName;
	if(szName.isEmpty())
		KviQString::sprintf(szName,"epilogue_%u_for_%Q",m_pPrologues->count(),&m_szName);
	m_pEpilogues->append(new KviKvsScript(szName,szCode));
}

KviKvsPopupMenuTopLevelData * KviKvsPopupMenu::topLevelData()
{
	if(parentPopup())return parentPopup()->topLevelData();
	return m_pTopLevelData;
}

KviKvsPopupMenu * KviKvsPopupMenu::findChildPopupByName(const QString &szItemName)
{
	for(KviKvsPopupMenuItem * it = m_pItemList->first();it;it = m_pItemList->next())
	{
		if(!it->isMenu())
			continue;
		if(!KviQString::equalCI(szItemName,it->name()))
			continue;
		KviKvsPopupMenu * pMenu = ((KviKvsPopupMenuItemMenu *)it)->menu();
		if(!pMenu)
			continue;
		return pMenu;
	}
	return NULL;
}

bool KviKvsPopupMenu::removeItemByName(const QString &szItemName,bool bRecursive)
{
	KviKvsScript * se;

	for(se = m_pEpilogues->first();se;se = m_pEpilogues->next())
	{
		if(KviQString::equalCI(szItemName,se->name()))
		{
			m_pEpilogues->removeRef(se);
			return true;
		}
	}

	for(se = m_pPrologues->first();se;se = m_pPrologues->next())
	{
		if(KviQString::equalCI(szItemName,se->name()))
		{
			m_pPrologues->removeRef(se);
			return true;
		}
	}

	for(KviKvsPopupMenuItem * it = m_pItemList->first();it;it = m_pItemList->next())
	{
		if(KviQString::equalCI(szItemName,it->name()))
		{
			m_pItemList->removeRef(it); // bye :)
			return true;
		}
	}

	if(bRecursive)
	{
		for(KviKvsPopupMenuItem * ii = m_pItemList->first();ii;ii = m_pItemList->next())
		{
			if(ii->isMenu())
			{
				if(((KviKvsPopupMenuItemMenu *)ii)->menu())
				{
					bool bRet = ((KviKvsPopupMenuItemMenu *)ii)->menu()->removeItemByName(szItemName,true);
					if(bRet)return true;
				}
			}
		}
	}

	return false;
}

bool KviKvsPopupMenu::isLocked()
{
	if(topLevelPopup()->isVisible())return true;
	KviKvsPopupMenuTopLevelData * d = topLevelData();
	return d ? d->isLocked() : false;
}


KviKvsPopupMenu * KviKvsPopupMenu::topLevelPopup()
{
	if(parentPopup())return parentPopup();
	return this;
}


KviKvsPopupMenu * KviKvsPopupMenu::addPopup(const QString &szItemName,const QString &szText,const QString &szIcon,const QString &szCondition)
{
	QString szName = szItemName;
	if(szName.isEmpty())
		KviQString::sprintf(szName,"%Q.subpopup%d",&m_szName,m_pItemList->count());
	KviKvsPopupMenu * pNew = new KviKvsPopupMenu(szName);
	pNew->setParentPopup(this);
	addItemInternal(new KviKvsPopupMenuItemMenu(szName,pNew,szText,szIcon,szCondition));
	return pNew;
}

void KviKvsPopupMenu::addSeparator(const QString &szItemName,const QString &szCondition)
{
	QString szName = szItemName;
	if(szName.isEmpty())
		KviQString::sprintf(szName,"%Q.separator%d",&m_szName,m_pItemList->count());
	addItemInternal(new KviKvsPopupMenuItemSeparator(szName,szCondition));
}

void KviKvsPopupMenu::addLabel(const QString &szItemName,const QString &szText,const QString &szIcon,const QString &szCondition)
{
	QString szName = szItemName;
	if(szName.isEmpty())
		KviQString::sprintf(szName,"%Q.label%d",&m_szName,m_pItemList->count());
	addItemInternal(new KviKvsPopupMenuItemLabel(szName,szText,szIcon,szCondition));
}

void KviKvsPopupMenu::addItem(const QString &szItemName,const QString &szCode,const QString &szText,const QString &szIcon,const QString &szCondition)
{
	QString szName = szItemName;
	if(szName.isEmpty())
		KviQString::sprintf(szName,"%Q.item%d",&m_szName,m_pItemList->count());
	addItemInternal(new KviKvsPopupMenuItemItem(szName,szCode,szText,szIcon,szCondition));

}

void KviKvsPopupMenu::addExtPopup(const QString &szItemName,const QString &szPopupName,const QString &szText,const QString &szIcon,const QString &szCondition)
{
	QString szName = szItemName;
	if(szName.isEmpty())
		KviQString::sprintf(szName,"%Q.%Q",&m_szName,&szName);
	addItemInternal(new KviKvsPopupMenuItemExtMenu(szName,szPopupName,szText,szIcon,szCondition));

}

void KviKvsPopupMenu::addItemInternal(KviKvsPopupMenuItem * it)
{
	if(isLocked())debug("Ooops... KviKvsPopupMenu is locked in ::addItem()");
	m_pItemList->append(it);
}

void KviKvsPopupMenu::doPopup(const QPoint & pnt,KviWindow * wnd,KviKvsVariantList * pParams,bool bTestMode)
{
	// This might be a compat problem later :(((((
	// it is also an ugly trick
	clearMenuContents();
	m_pTempTopLevelData = new KviKvsPopupMenuTopLevelData(pParams,wnd);
	m_pTempTopLevelData->setTestMode(bTestMode);
	KviTalPopupMenu::popup(pnt);
}

void KviKvsPopupMenu::clearMenuContents()
{
	m_bSetupDone = false;

	clear();

	for(KviKvsPopupMenuItem * it = m_pItemList->first();it;it = m_pItemList->next())
	{
		it->clear();
	}

	if(m_pTopLevelData)
	{
		delete m_pTopLevelData;
		m_pTopLevelData = 0;
	}
	if(m_pTempTopLevelData)
	{
		delete m_pTempTopLevelData;
		m_pTempTopLevelData = 0;
	}
}

void KviKvsPopupMenu::doClear()
{
	clear();
	if(m_pTopLevelData)
	{
		delete m_pTopLevelData;
		m_pTopLevelData = 0;
	}
	if(m_pTempTopLevelData)
	{
		delete m_pTempTopLevelData;
		m_pTempTopLevelData = 0;
	}
	m_bSetupDone = false;
	m_pItemList->clear();

	m_pPrologues->clear();
	m_pEpilogues->clear();
}


void KviKvsPopupMenu::lock(bool bLock)
{
	KviKvsPopupMenuTopLevelData * d = topLevelData();
	if(!d)return;
	d->setLocked(bLock);
}


void KviKvsPopupMenu::setupMenuContents()
{
	// This might be a compat problem later :((((
	if(parentPopup() == 0)
	{
		if(m_pTempTopLevelData == 0)
		{
			// We have been called by a KviMenuBar!
			// m_bSetupDone is not valid here
			clearMenuContents();
			m_pTopLevelData = new KviKvsPopupMenuTopLevelData(new KviKvsVariantList(),g_pActiveWindow);
		} else {
			if(m_bSetupDone)return;
			// we have been called by doPopup
			// the menu contents have been already cleared
			if(m_pTopLevelData)debug("Ops.. something got messed in KviKvsPopupMenu activation system");
			// Swap the top level data from temporary to the permanent
			m_pTopLevelData = m_pTempTopLevelData;
			m_pTempTopLevelData = 0;
		}
	} else {
		if(m_bSetupDone)return;
	}

	m_bSetupDone = true;


	// HACK...this is to remove the separator inserted by Qt when popup() is called and the popup is empty
	clear();

	KviKvsPopupMenuTopLevelData * d = topLevelData();
	if(!d)
	{
		qDebug("Ops...menu contents changed behind my back!");
		return;
	}


	lock(true);


	if(!g_pApp->windowExists(d->window()))d->setWindow(g_pApp->activeConsole());

	if(!d->testMode())
		executePrologues(d);

	// Fill this menu contents
	int idx = 0;
	for(KviKvsPopupMenuItem * it = m_pItemList->first();it;it = m_pItemList->next())
	{
		it->fill(this,d,idx);
		++idx;
	}

	if(!d->testMode())
		executeEpilogues(d);

	lock(false);

}

void KviKvsPopupMenu::executePrologues(KviKvsPopupMenuTopLevelData * pData)
{
	for(KviKvsScript * s = m_pPrologues->first();s;s = m_pPrologues->next())
	{
		if(!s->run(pData->window(),
			pData->parameters(),
			0,
			KviKvsScript::PreserveParams,
			pData->extendedRunTimeData()))
		{
			pData->window()->output(KVI_OUT_PARSERWARNING,__tr2qs_ctx("Broken prologue in popup menu '%Q': ignoring","kvs"),&m_szName);
		}
	}
}

void KviKvsPopupMenu::executeEpilogues(KviKvsPopupMenuTopLevelData * pData)
{
	for(KviKvsScript * s = m_pEpilogues->first();s;s = m_pEpilogues->next())
	{
		if(!s->run(pData->window(),
			pData->parameters(),
			0,
			KviKvsScript::PreserveParams,
			pData->extendedRunTimeData()))
		{
			pData->window()->output(KVI_OUT_PARSERWARNING,__tr2qs_ctx("Broken epilogue in popup menu '%Q': ignoring","kvs"),&m_szName);
		}
	}
}


void KviKvsPopupMenu::itemClicked(int itemId)
{
	int param = itemParameter(itemId);
	KviKvsPopupMenuItem * it = m_pItemList->at(param);
	KviKvsPopupMenuTopLevelData * d = topLevelData();
	if(it && d)
	{
		if(it->isItem())
		{
			if(d->testMode())
			{
				emit testModeItemClicked(it);
			} else {
				// rebind if window is lost
				if(!g_pApp->windowExists(d->window()))d->setWindow(g_pApp->activeConsole());
				// FIXME: we could avoid locking since scripts can be shared now!
				// see KviKvsTimerManager implementation
				lock(true);
				((KviKvsPopupMenuItemItem *)it)->kvsCode()->run(
						d->window(),
						d->parameters(),
						0,
						KviKvsScript::PreserveParams,
						d->extendedRunTimeData());
				// FIXME: should we print somethng if run() returns false ?
				lock(false);
			}
		} else qDebug("oops....clicked something that is not an item at position %d",param);
		// FIXME: #warning "Maybe tell that the window has changed"
	} else qDebug("oops....no menu item at position %d",param);
}


void KviKvsPopupMenu::load(const QString &prefix,KviConfig * cfg)
{
	doClear();

	int cnt;
	int idx;

	QString tmp = prefix;
	tmp.append("_PrologueCount");

	cnt = cfg->readIntEntry(tmp,0);

	if(cnt > 0)
	{
		for(idx = 0;idx < cnt;idx++)
		{
			KviQString::sprintf(tmp,"%Q_Prologue%d",&(prefix),idx);
			QString pr = cfg->readQStringEntry(tmp,"");
			KviQString::sprintf(tmp,"%Q_PrologueId%d",&(prefix),idx);
			QString itemName = cfg->readQStringEntry(tmp,"");
			if(!pr.isEmpty())addPrologue(itemName,pr);
		}
	} else {
		// Might be old version!
		KviQString::sprintf(tmp,"%Q_Prologue",&(prefix));
		QString pr = cfg->readQStringEntry(tmp,"");
		if(!pr.isEmpty())addPrologue(QString(),pr);
	}

	KviQString::sprintf(tmp,"%Q_EpilogueCount",&prefix);
	cnt = cfg->readIntEntry(tmp,0);

	if(cnt > 0)
	{
		for(idx = 0;idx < cnt;idx++)
		{
			KviQString::sprintf(tmp,"%Q_Epilogue%d",&prefix,idx);
			QString ep = cfg->readQStringEntry(tmp,"");
			KviQString::sprintf(tmp,"%Q_PrologueId%d",&(prefix),idx);
			QString itemName = cfg->readQStringEntry(tmp,"");
			if(!ep.isEmpty())addEpilogue(itemName,ep);
		}
	} else {
		// Might be old version!
		KviQString::sprintf(tmp,"%Q_Epilogue",&prefix);
		QString ep = cfg->readQStringEntry(tmp,"");
		if(!ep.isEmpty())addEpilogue(QString(),ep);
	}


	KviQString::sprintf(tmp,"%Q_Count",&prefix);

	cnt = cfg->readIntEntry(tmp,0);

	for(idx = 0;idx < cnt;idx++)
	{
		QString pre;
		KviQString::sprintf(pre,"%Q_%d",&prefix,idx);

		KviQString::sprintf(tmp,"%Q_Id",&pre);
		QString itemName = cfg->readQStringEntry(tmp,QString());
		KviQString::sprintf(tmp,"%Q_Type",&pre);

		int type = cfg->readIntEntry(tmp,3);
		switch(type)
		{
			case 0: // separator
			{
				QString expr;
				KviQString::sprintf(tmp,"%Q_Expr",&pre);
				expr = cfg->readQStringEntry(tmp,"");
				addSeparator(itemName,expr);
			}
			break;
			case 1: // item
			{
				QString text,icon,code,expr;
				KviQString::sprintf(tmp,"%Q_Text",&pre);
				text = cfg->readQStringEntry(tmp,"Unnamed");
				KviQString::sprintf(tmp,"%Q_Icon",&pre);
				icon = cfg->readQStringEntry(tmp,"");
				KviQString::sprintf(tmp,"%Q_Code",&pre);
				code = cfg->readQStringEntry(tmp,"");
				KviQString::sprintf(tmp,"%Q_Expr",&pre);
				expr = cfg->readQStringEntry(tmp,"");
				addItem(itemName,code,text,icon,expr);
			}
			break;
			case 2: // menu
			{
				QString text,icon,expr;
				KviQString::sprintf(tmp,"%Q_Text",&pre);
				text = cfg->readQStringEntry(tmp,"Unnamed");
				KviQString::sprintf(tmp,"%Q_Icon",&pre);
				icon = cfg->readQStringEntry(tmp,"");
				KviQString::sprintf(tmp,"%Q_Expr",&pre);
				expr = cfg->readQStringEntry(tmp,"");
				KviKvsPopupMenu * pop = addPopup(itemName,text,icon,expr);
				pop->load(pre,cfg);
			}
			break;
			case 3: // label
			{
				QString text,icon,expr;
				KviQString::sprintf(tmp,"%Q_Text",&pre);
				text = cfg->readQStringEntry(tmp,"Unnamed");
				KviQString::sprintf(tmp,"%Q_Icon",&pre);
				icon = cfg->readQStringEntry(tmp,"");
				KviQString::sprintf(tmp,"%Q_Expr",&pre);
				expr = cfg->readQStringEntry(tmp,"");
				addLabel(itemName,text,icon,expr);
			}
			break;
			case 4: // extmenu
			{
				QString text,icon,code,expr;
				KviQString::sprintf(tmp,"%Q_Text",&pre);
				text = cfg->readQStringEntry(tmp,"Unnamed");
				KviQString::sprintf(tmp,"%Q_Icon",&pre);
				icon = cfg->readQStringEntry(tmp,"");
				KviQString::sprintf(tmp,"%Q_ExtName",&pre);
				code = cfg->readQStringEntry(tmp,"");
				KviQString::sprintf(tmp,"%Q_Expr",&pre);
				expr = cfg->readQStringEntry(tmp,"");
				addExtPopup(itemName,code,text,icon,expr);
			}
			break;
			default: // ignore
			break;
		}
	}

}

// FIXME: #warning "NOBODY EDITS THE POPUPS IN THE CONFIG!...A binary config would be faster and work better for sure here"

void KviKvsPopupMenu::save(const QString & prefix,KviConfig * cfg)
{
	int idx;

	KviKvsScript * s;
	QString tmp;

	KviQString::sprintf(tmp,"%Q_PrologueCount",&prefix);
	cfg->writeEntry(tmp,m_pPrologues->count());

	idx = 0;
	for(s = m_pPrologues->first();s;s = m_pPrologues->next())
	{
		KviQString::sprintf(tmp,"%Q_Prologue%d",&prefix,idx);
		cfg->writeEntry(tmp,s->code());
		KviQString::sprintf(tmp,"%Q_PrologueId%d",&prefix,idx);
		cfg->writeEntry(tmp,s->name());
		idx++;
	}

	KviQString::sprintf(tmp,"%Q_EpilogueCount",&prefix);
	cfg->writeEntry(tmp,m_pEpilogues->count());

	idx = 0;
	for(s = m_pEpilogues->first();s;s = m_pEpilogues->next())
	{
		KviQString::sprintf(tmp,"%Q_Epilogue%d",&prefix,idx);
		cfg->writeEntry(tmp,s->code());
		KviQString::sprintf(tmp,"%Q_EpilogueId%d",&prefix,idx);
		cfg->writeEntry(tmp,s->name());
		idx++;
	}

	KviQString::sprintf(tmp,"%Q_Count",&prefix);
	cfg->writeEntry(tmp,m_pItemList->count());
	idx = 0;


	for(KviKvsPopupMenuItem * it = m_pItemList->first();it;it = m_pItemList->next())
	{
		QString pre;
		KviQString::sprintf(pre,"%Q_%d",&prefix,idx);
		KviQString::sprintf(tmp,"%Q_Type",&pre);
		int typeCode = 0;
		switch(it->type())
		{
			case KviKvsPopupMenuItem::Label:        typeCode = 3; break;
			case KviKvsPopupMenuItem::Separator:    typeCode = 0; break;
			case KviKvsPopupMenuItem::Menu:         typeCode = 2; break;
			case KviKvsPopupMenuItem::Item:         typeCode = 1; break;
			case KviKvsPopupMenuItem::ExtMenu:      typeCode = 4; break;
		}

		cfg->writeEntry(tmp,typeCode);

		KviQString::sprintf(tmp,"%Q_Id",&pre);
		cfg->writeEntry(tmp,it->name());

		s = it->kvsCondition();
		if(s)
		{
			KviQString::sprintf(tmp,"%Q_Expr",&pre);
			cfg->writeEntry(tmp,s->code());
		}

		s = it->kvsIcon();
		if(s)
		{
			KviQString::sprintf(tmp,"%Q_Icon",&pre);
			cfg->writeEntry(tmp,s->code());
		}

		s = it->kvsText();
		if(s)
		{
			KviQString::sprintf(tmp,"%Q_Text",&pre);
			cfg->writeEntry(tmp,s->code());
		}

		s = it->kvsCode();
		if(s)
		{
			KviQString::sprintf(tmp,"%Q_Code",&pre);
			cfg->writeEntry(tmp,s->code());
		}

		if(it->isMenu())
		{
			((KviKvsPopupMenuItemMenu *)it)->menu()->save(pre,cfg);
		} else if(it->isExtMenu())
		{
			KviQString::sprintf(tmp,"%Q_ExtName",&pre);
			cfg->writeEntry(tmp,((KviKvsPopupMenuItemExtMenu *)it)->extName());
		}

		++idx;
	}
}

void KviKvsPopupMenu::generateDefPopupCore(QString &buffer)
{

	QString tmp;

	buffer = "";

	KviKvsScript * s;

	for(s = m_pPrologues->first();s;s = m_pPrologues->next())
	{
		buffer.append("prologue\n");
		tmp = s->code();
		tmp.trimmed();
		KviCommandFormatter::blockFromBuffer(tmp);
		buffer.append(tmp);
		buffer.append('\n');
	}

	for(KviKvsPopupMenuItem * it = m_pItemList->first();it;it = m_pItemList->next())
	{
		switch(it->type())
		{
			case KviKvsPopupMenuItem::Item:
				if(it->kvsIcon())KviQString::appendFormatted(buffer,"item(%Q,%Q)",&(it->kvsText()->code()),&(it->kvsIcon()->code()));
				else KviQString::appendFormatted(buffer,"item(%Q)",&(it->kvsText()->code()));
				if(it->kvsCondition())KviQString::appendFormatted(buffer," (%Q)",&(it->kvsCondition()->code()));
				buffer.append("\n");
				tmp = it->kvsCode()->code();
				KviCommandFormatter::blockFromBuffer(tmp);
				buffer.append(tmp);
				buffer.append("\n");
			break;
			case KviKvsPopupMenuItem::Menu:
				if(it->kvsIcon())KviQString::appendFormatted(buffer,"popup(%Q,%Q)",&(it->kvsText()->code()),&(it->kvsIcon()->code()));
				else KviQString::appendFormatted(buffer,"popup(%Q)",&(it->kvsText()->code()));
				if(it->kvsCondition())KviQString::appendFormatted(buffer," (%Q)",&(it->kvsCondition()->code()));
				buffer.append("\n");
				((KviKvsPopupMenuItemMenu *)it)->menu()->generateDefPopupCore(tmp);
				KviCommandFormatter::blockFromBuffer(tmp);
				buffer.append(tmp);
				buffer.append("\n");
			break;
			case KviKvsPopupMenuItem::Separator:
				if(it->kvsCondition())KviQString::appendFormatted(buffer,"separator(%Q)\n\n",&(it->kvsCondition()->code()));
				else buffer.append("separator\n\n");
			break;
			case KviKvsPopupMenuItem::Label:
				if(it->kvsIcon())KviQString::appendFormatted(buffer,"label(%Q,%Q)",&(it->kvsText()->code()),&(it->kvsIcon()->code()));
				else KviQString::appendFormatted(buffer,"label(%Q)",&(it->kvsText()->code()));
				if(it->kvsCondition())KviQString::appendFormatted(buffer," (%Q)",&(it->kvsCondition()->code()));
				buffer.append("\n\n");
			break;
			case KviKvsPopupMenuItem::ExtMenu:
				if(it->kvsIcon())KviQString::appendFormatted(buffer,"extpopup(%Q,%Q,%Q)",&(it->kvsText()->code()),&(((KviKvsPopupMenuItemExtMenu *)it)->extName()),&(it->kvsIcon()->code()));
				else KviQString::appendFormatted(buffer,"extpopup(%Q)",&(it->kvsText()->code()));
				if(it->kvsCondition())KviQString::appendFormatted(buffer," (%Q)",&(it->kvsCondition()->code()));
				buffer.append("\n\n");
			break;
		}
	}

	for(s = m_pEpilogues->first();s;s = m_pEpilogues->next())
	{
		buffer.append("epilogue\n");
		tmp = s->code();
		tmp.trimmed();
		KviCommandFormatter::blockFromBuffer(tmp);
		buffer.append(tmp);
		buffer.append('\n');
	}

}

void KviKvsPopupMenu::generateDefPopup(QString &buffer)
{
	KviQString::sprintf(buffer,"defpopup(%s)\n",objectName().toLatin1().data());
	QString core;

	generateDefPopupCore(core);
	KviCommandFormatter::blockFromBuffer(core);
	buffer.append(core);
}
