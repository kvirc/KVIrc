//=============================================================================
//
//   File : KviKvsPopupMenu.cpp
//   Creation date : Wed 07 Jan 2004 05:02:57 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviKvsPopupMenu.h"
#include "KviLocale.h"
#include "KviIconManager.h"
#include "kvi_out.h"
#include "KviApplication.h"
#include "KviKvsPopupManager.h"
#include "KviConsoleWindow.h"
#include "KviConfigurationFile.h"
#include "KviCommandFormatter.h"
#include "KviOptions.h"

#include <QWidgetAction>
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
		pData->window()->output(KVI_OUT_PARSERWARNING,__tr2qs_ctx("Broken condition in menu setup: Assuming false","kvs"));
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
    ((QMenu*)pMenu)->addSeparator();
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
		pData->window()->output(KVI_OUT_PARSERWARNING,__tr2qs_ctx("Broken icon parameter: Ignoring","kvs"));
		return 0;
	}

	QString szRet;
	vRet.asString(szRet);

	QPixmap * p = g_pIconManager->getImage(szRet);
	if(!p)pData->window()->output(KVI_OUT_PARSERWARNING,__tr2qs_ctx("Can't find the icon '%Q': Ignoring","kvs"),&szRet);
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
		pData->window()->output(KVI_OUT_PARSERWARNING,__tr2qs_ctx("Broken text parameter: Assuming empty string","kvs"));
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

	//QPalette p;
	//m_pLabel->setStyleSheet("background-color: " + p.color(QPalette::Normal, QPalette::Mid).name());

	m_pLabel->setFrameStyle(QFrame::Raised | QFrame::StyledPanel);
	m_pLabel->setMargin(5);

	if(pPix)m_pLabel->setPixmap(*pPix);
	QWidgetAction *pAction = new QWidgetAction(pMenu);
	pAction->setDefaultWidget(m_pLabel);
	pMenu->addAction(pAction);
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
    QAction *pAction;
    if(pPix)
        pAction = pMenu->addAction(*pPix,szText);
    else
        pAction = pMenu->addAction(szText);
    pAction->setData(iIdx);
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
	KviKvsPopupMenu * copy = new KviKvsPopupMenu(m_pMenu->popupName());
	copy->copyFrom(m_pMenu);
	return new KviKvsPopupMenuItemMenu(m_szItemName,copy,m_pKvsText,m_pKvsIcon,m_pKvsCondition);
}


void KviKvsPopupMenuItemMenu::fill(KviKvsPopupMenu * pMenu,KviKvsPopupMenuTopLevelData * pData,int iIdx)
{
	if(!evaluateCondition(pData))return;
	QString szText = evaluateText(pData);
	QPixmap * pPix = evaluateIcon(pData);
    QAction *pAction;
	m_pMenu->setParentPopup(pMenu);
    if(pPix)
        pAction = pMenu->addAction(*pPix,szText);
    else
        pAction = pMenu->addAction(szText);
    pAction->setMenu(m_pMenu);
    pAction->setData(iIdx);
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
		if(source->isHardLocked())
		{
			pData->window()->output(KVI_OUT_PARSERWARNING,__tr2qs_ctx("Recursive definition detected for popup '%Q': Ignoring","kvs"),&(pMenu->popupName()));
			return;
		}
		QString tmp = QString("%1.%2").arg(pMenu->popupName(),m_szMenuName);
		if(m_pMenu)delete m_pMenu;
		m_pMenu = new KviKvsPopupMenu(tmp);
		m_pMenu->copyFrom(source);
		m_pMenu->setParentPopup(pMenu);

        QAction *pAction;
        if(pPix)
            pAction = pMenu->addAction(*pPix,szText);
        else
            pAction = pMenu->addAction(szText);
        pAction->setMenu(m_pMenu);
        pAction->setData(iIdx);
	} else {
		pData->window()->output(KVI_OUT_PARSERWARNING,__tr2qs_ctx("Can't find the external popup '%Q': Ignoring","kvs"),&m_szMenuName);
	}
}


KviKvsPopupMenuTopLevelData::KviKvsPopupMenuTopLevelData(KviKvsVariantList * pParameters,KviWindow * pWindow)
{
	m_pExtendedRunTimeData = new KviKvsExtendedRunTimeData(new KviKvsHash(),true);
	m_pParameters = pParameters;
	m_pWindow = pWindow;
	m_bTestMode = false;
	m_eLocked = Unlocked;
}

KviKvsPopupMenuTopLevelData::~KviKvsPopupMenuTopLevelData()
{
	delete m_pExtendedRunTimeData;
	delete m_pParameters;
}


KviKvsPopupMenu::KviKvsPopupMenu(const QString &szName)
:QMenu(szName, 0)
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
    connect(this,SIGNAL(triggered(QAction*)),this,SLOT(itemClicked(QAction *)));
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
		szName = QString("prologue_%1_for_%2").arg(m_pPrologues->count()).arg(m_szName);
	m_pPrologues->append(new KviKvsScript(szName,szCode));
}

void KviKvsPopupMenu::addEpilogue(const QString &szItemName,const QString &szCode)
{
	// FIXME: translate this or not ?
	QString szName = szItemName;
	if(szName.isEmpty())
		szName = QString("epilogue_%1_for_%2").arg(m_pPrologues->count()).arg(m_szName);
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

bool KviKvsPopupMenu::isSoftLocked()
{
	if(topLevelPopup()->isVisible())return true;
	KviKvsPopupMenuTopLevelData * d = topLevelData();
	return d ? d->isSoftLocked() : false;
}

bool KviKvsPopupMenu::isHardLocked()
{
	if(topLevelPopup()->isVisible())return true;
	KviKvsPopupMenuTopLevelData * d = topLevelData();
	return d ? d->isHardLocked() : false;
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
		szName = QString("%1.subpopup%2").arg(m_szName).arg(m_pItemList->count());
	KviKvsPopupMenu * pNew = new KviKvsPopupMenu(szName);
	pNew->setParentPopup(this);
	addItemInternal(new KviKvsPopupMenuItemMenu(szName,pNew,szText,szIcon,szCondition));
	return pNew;
}

void KviKvsPopupMenu::addSeparator(const QString &szItemName,const QString &szCondition)
{
	QString szName = szItemName;
	if(szName.isEmpty())
		szName = QString("%1.separator%2").arg(m_szName).arg(m_pItemList->count());
	addItemInternal(new KviKvsPopupMenuItemSeparator(szName,szCondition));
}

void KviKvsPopupMenu::addLabel(const QString &szItemName,const QString &szText,const QString &szIcon,const QString &szCondition)
{
	QString szName = szItemName;
	if(szName.isEmpty())
		szName = QString("%1.label%2").arg(m_szName).arg(m_pItemList->count());
	addItemInternal(new KviKvsPopupMenuItemLabel(szName,szText,szIcon,szCondition));
}

void KviKvsPopupMenu::addItem(const QString &szItemName,const QString &szCode,const QString &szText,const QString &szIcon,const QString &szCondition)
{
	QString szName = szItemName;
	if(szName.isEmpty())
		szName = QString("%1.item%2").arg(m_szName).arg(m_pItemList->count());
	addItemInternal(new KviKvsPopupMenuItemItem(szName,szCode,szText,szIcon,szCondition));

}

void KviKvsPopupMenu::addExtPopup(const QString &szItemName,const QString &szPopupName,const QString &szText,const QString &szIcon,const QString &szCondition)
{
	QString szName = szItemName;
	if(szName.isEmpty())
		szName = QString("%1.%2").arg(m_szName,szName);
	addItemInternal(new KviKvsPopupMenuItemExtMenu(szName,szPopupName,szText,szIcon,szCondition));

}

void KviKvsPopupMenu::addItemInternal(KviKvsPopupMenuItem * it)
{
	if(isHardLocked())
		qDebug("Ooops... KviKvsPopupMenu is locked in ::addItem()");
	m_pItemList->append(it);
}

void KviKvsPopupMenu::doPopup(const QPoint & pnt,KviWindow * wnd,KviKvsVariantList * pParams,bool bTestMode)
{
	// This might be a compat problem later :(((((
	// it is also an ugly trick
	clearMenuContents();
	m_pTempTopLevelData = new KviKvsPopupMenuTopLevelData(pParams,wnd);
	m_pTempTopLevelData->setTestMode(bTestMode);
    QMenu::popup(pnt);
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


void KviKvsPopupMenu::lock(KviKvsPopupMenuTopLevelData::LockStatus eLock)
{
	KviKvsPopupMenuTopLevelData * d = topLevelData();
	if(!d)
		return;
	d->setLocked(eLock);
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
			if(m_pTopLevelData)qDebug("Ops.. Something got messed in KviKvsPopupMenu activation system");
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
		qDebug("Ops... Menu contents changed behind my back!");
		return;
	}

	if(!g_pApp->windowExists(d->window()))d->setWindow(g_pApp->activeConsole());

	lock(KviKvsPopupMenuTopLevelData::SoftLocked);

	if(!d->testMode())
		executePrologues(d);

	lock(KviKvsPopupMenuTopLevelData::HardLocked);

	// Fill this menu contents
	int idx = 0;
	for(KviKvsPopupMenuItem * it = m_pItemList->first();it;it = m_pItemList->next())
	{
		it->fill(this,d,idx);
		++idx;
	}

	lock(KviKvsPopupMenuTopLevelData::SoftLocked);

	if(!d->testMode())
		executeEpilogues(d);

	lock(KviKvsPopupMenuTopLevelData::Unlocked);
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
			pData->window()->output(KVI_OUT_PARSERWARNING,__tr2qs_ctx("Broken prologue in popup menu '%Q': Ignoring","kvs"),&m_szName);
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
			pData->window()->output(KVI_OUT_PARSERWARNING,__tr2qs_ctx("Broken epilogue in popup menu '%Q': Ignoring","kvs"),&m_szName);
		}
	}
}


void KviKvsPopupMenu::itemClicked(QAction *pAction)
{
	// Avoid the event propagation of QMenu::triggered() from submenus
	if(pAction->parent()!=this)
		return;
    bool bOk=false;
    int param = pAction->data().toInt(&bOk);
    if(!bOk)
        return;
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
				lock(KviKvsPopupMenuTopLevelData::HardLocked);
				if(d->extendedRunTimeData())
					d->extendedRunTimeData()->setPopupId(new QString(it->name()));
				((KviKvsPopupMenuItemItem *)it)->kvsCode()->run(
						d->window(),
						d->parameters(),
						0,
						KviKvsScript::PreserveParams,
						d->extendedRunTimeData());
				// FIXME: should we print somethng if run() returns false ?
				lock(KviKvsPopupMenuTopLevelData::Unlocked);
			}
		} else qDebug("Oops.... Clicked something that is not an item at position %d",param);
		// FIXME: #warning "Maybe tell that the window has changed"
	} else qDebug("Oops.... No menu item at position %d",param);
}


void KviKvsPopupMenu::load(const QString &prefix,KviConfigurationFile * cfg)
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
			tmp = QString("%1_Prologue%2").arg(prefix).arg(idx);
			QString pr = cfg->readEntry(tmp,"");
			tmp = QString("%1_PrologueId%2").arg(prefix).arg(idx);
			QString itemName = cfg->readEntry(tmp,"");
			if(!pr.isEmpty())addPrologue(itemName,pr);
		}
	} else {
		// Might be old version!
		tmp = QString("%1_Prologue").arg(prefix);
		QString pr = cfg->readEntry(tmp,"");
		if(!pr.isEmpty())addPrologue(QString(),pr);
	}

	tmp = QString("%1_EpilogueCount").arg(prefix);
	cnt = cfg->readIntEntry(tmp,0);

	if(cnt > 0)
	{
		for(idx = 0;idx < cnt;idx++)
		{
			tmp = QString("%1_Epilogue%2").arg(prefix).arg(idx);
			QString ep = cfg->readEntry(tmp,"");
			tmp = QString("%1_EpilogueId%2").arg(prefix).arg(idx);
			QString itemName = cfg->readEntry(tmp,"");
			if(!ep.isEmpty())addEpilogue(itemName,ep);
		}
	} else {
		// Might be old version!
		tmp = QString("%1_Epilogue").arg(prefix);
		QString ep = cfg->readEntry(tmp,"");
		if(!ep.isEmpty())addEpilogue(QString(),ep);
	}


	tmp = QString("%1_Count").arg(prefix);

	cnt = cfg->readIntEntry(tmp,0);

	for(idx = 0;idx < cnt;idx++)
	{
		QString pre = QString("%1_%2").arg(prefix).arg(idx);

		tmp = QString("%1_Id").arg(pre);
		QString itemName = cfg->readEntry(tmp,QString());
		tmp = QString("%1_Type").arg(pre);

		int type = cfg->readIntEntry(tmp,3);
		switch(type)
		{
			case 0: // separator
			{
				QString expr;
				tmp = QString("%1_Expr").arg(pre);
				expr = cfg->readEntry(tmp,"");
				addSeparator(itemName,expr);
			}
			break;
			case 1: // item
			{
				QString text,icon,code,expr;
				tmp = QString("%1_Text").arg(pre);
				text = cfg->readEntry(tmp,"Unnamed");
				tmp = QString("%1_Icon").arg(pre);
				icon = cfg->readEntry(tmp,"");
				tmp = QString("%1_Code").arg(pre);
				code = cfg->readEntry(tmp,"");
				tmp = QString("%1_Expr").arg(pre);
				expr = cfg->readEntry(tmp,"");
				addItem(itemName,code,text,icon,expr);
			}
			break;
			case 2: // menu
			{
				QString text,icon,expr;
				tmp = QString("%1_Text").arg(pre);
				text = cfg->readEntry(tmp,"Unnamed");
				tmp = QString("%1_Icon").arg(pre);
				icon = cfg->readEntry(tmp,"");
				tmp = QString("%1_Expr").arg(pre);
				expr = cfg->readEntry(tmp,"");
				KviKvsPopupMenu * pop = addPopup(itemName,text,icon,expr);
				pop->load(pre,cfg);
			}
			break;
			case 3: // label
			{
				QString text,icon,expr;
				tmp = QString("%1_Text").arg(pre);
				text = cfg->readEntry(tmp,"Unnamed");
				tmp = QString("%1_Icon").arg(pre);
				icon = cfg->readEntry(tmp,"");
				tmp = QString("%1_Expr").arg(pre);
				expr = cfg->readEntry(tmp,"");
				addLabel(itemName,text,icon,expr);
			}
			break;
			case 4: // extmenu
			{
				QString text,icon,code,expr;
				tmp = QString("%1_Text").arg(pre);
				text = cfg->readEntry(tmp,"Unnamed");
				tmp = QString("%1_Icon").arg(pre);
				icon = cfg->readEntry(tmp,"");
				tmp = QString("%1_ExtName").arg(pre);
				code = cfg->readEntry(tmp,"");
				tmp = QString("%1_Expr").arg(pre);
				expr = cfg->readEntry(tmp,"");
				addExtPopup(itemName,code,text,icon,expr);
			}
			break;
			default: // ignore
			break;
		}
	}

}

// FIXME: #warning "NOBODY EDITS THE POPUPS IN THE CONFIG!...A binary config would be faster and work better for sure here"

void KviKvsPopupMenu::save(const QString & prefix,KviConfigurationFile * cfg)
{
	int idx;

	KviKvsScript * s;
	QString tmp;

	tmp = QString("%1_PrologueCount").arg(prefix);
	cfg->writeEntry(tmp,m_pPrologues->count());

	idx = 0;
	for(s = m_pPrologues->first();s;s = m_pPrologues->next())
	{
		tmp = QString("%1_Prologue%2").arg(prefix).arg(idx);
		cfg->writeEntry(tmp,s->code());
		tmp = QString("%1_PrologueId%2").arg(prefix).arg(idx);
		cfg->writeEntry(tmp,s->name());
		idx++;
	}

	tmp = QString("%1_EpilogueCount").arg(prefix);
	cfg->writeEntry(tmp,m_pEpilogues->count());

	idx = 0;
	for(s = m_pEpilogues->first();s;s = m_pEpilogues->next())
	{
		tmp = QString("%1_Epilogue%2").arg(prefix).arg(idx);
		cfg->writeEntry(tmp,s->code());
		tmp = QString("%1_EpilogueId%2").arg(prefix).arg(idx);
		cfg->writeEntry(tmp,s->name());
		idx++;
	}

	tmp = QString("%1_Count").arg(prefix);
	cfg->writeEntry(tmp,m_pItemList->count());
	idx = 0;


	for(KviKvsPopupMenuItem * it = m_pItemList->first();it;it = m_pItemList->next())
	{
		QString pre = QString("%1_%2").arg(prefix).arg(idx);
		tmp = QString("%1_Type").arg(pre);
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

		tmp = QString("%1_Id").arg(pre);
		cfg->writeEntry(tmp,it->name());

		s = it->kvsCondition();
		if(s)
		{
			tmp = QString("%1_Expr").arg(pre);
			cfg->writeEntry(tmp,s->code());
		}

		s = it->kvsIcon();
		if(s)
		{
			tmp = QString("%1_Icon").arg(pre);
			cfg->writeEntry(tmp,s->code());
		}

		s = it->kvsText();
		if(s)
		{
			tmp = QString("%1_Text").arg(pre);
			cfg->writeEntry(tmp,s->code());
		}

		s = it->kvsCode();
		if(s)
		{
			tmp = QString("%1_Code").arg(pre);
			cfg->writeEntry(tmp,s->code());
		}

		if(it->isMenu())
		{
			((KviKvsPopupMenuItemMenu *)it)->menu()->save(pre,cfg);
		} else if(it->isExtMenu())
		{
			tmp = QString("%1_ExtName").arg(pre);
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
		tmp = s->code().trimmed();
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
		tmp = s->code().trimmed();
		KviCommandFormatter::blockFromBuffer(tmp);
		buffer.append(tmp);
		buffer.append('\n');
	}

}

void KviKvsPopupMenu::generateDefPopup(QString &buffer)
{
	buffer = QString("defpopup(%1)\n").arg(m_szName);
	QString core;

	generateDefPopupCore(core);
	KviCommandFormatter::blockFromBuffer(core);
	buffer.append(core);
}
