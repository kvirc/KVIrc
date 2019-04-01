//=============================================================================
//
//   File : KviUserListView.cpp
//   Creation date : Tue Aug 1 2000 21:05:22 by Szymon Stefanek
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

#define _KVI_USERLISTVIEW_CPP_

#include "kvi_debug.h"
#include "KviUserListView.h"
#include "kvi_settings.h"
#include "KviLocale.h"
#include "KviOptions.h"
#include "kvi_defaults.h"
#include "KviIconManager.h"
#include "KviRegisteredUserDataBase.h"
#include "KviWindow.h"
#include "KviConsoleWindow.h"
#include "KviApplication.h"
#include "KviUserAction.h"
#include "KviQString.h"
#include "KviMainWindow.h"
#include "KviWindowStack.h"
#include "KviKvsEventTriggers.h"
#include "KviWindowToolWidget.h"
#include "KviStringConversion.h"
#include "KviIrcConnection.h"
#include "KviIrcConnectionServerInfo.h"
#include "KviPixmapUtils.h"

#include <QLabel>
#include <QScrollBar>
#include <QPainter>
#include <QPixmap>
#include <QFontMetrics>
#include <QDateTime>
#include <QEvent>
#include <QPaintEvent>
#include <QScrollBar>
#include <QRegExp>

#ifdef COMPILE_PSEUDO_TRANSPARENCY
extern QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif

// KviApplication.cpp (loaded and destroyed by KviIconManager)
extern QPixmap * g_pUserChanStatePixmap;

// Yet another really complex widget :)

#define KVI_USERLIST_BORDER_WIDTH 2
#define KVI_USERLIST_ICON_WIDTH 16
#define KVI_USERLIST_ICON_STATE_WIDTH 8
#define KVI_USERLIST_ICON_MARGIN 3

// FIXME: #warning "We want to be able to navigate the list with the keyboard!"

KviUserListToolTip::KviUserListToolTip(KviUserListView * pView, KviUserListViewArea * pArea)
    : KviTalToolTip(pArea)
{
	m_pListView = pView;
}

KviUserListToolTip::~KviUserListToolTip()
    = default;

void KviUserListToolTip::maybeTip(const QPoint & pnt)
{
	m_pListView->maybeTip(this, pnt);
}

KviUserListEntry::KviUserListEntry(KviUserListView * pParent, const QString & szNick, KviIrcUserEntry * pEntry, short int iFlags, bool bJoinTimeUnknown)
    : QObject()
{
	m_pListView = pParent;
	m_szNick = szNick;
	m_pGlobalData = pEntry;
	m_iFlags = iFlags;
	m_lastActionTime = (kvi_time_t)0;
	m_joinTime = bJoinTimeUnknown ? (kvi_time_t)0 : kvi_unixTime();
	m_iTemperature = bJoinTimeUnknown ? 0 : KVI_USERACTION_JOIN;

	m_bSelected = false;
	m_pAvatarPixmap = nullptr;

	updateAvatarData();
	recalcSize();
}

KviUserListEntry::~KviUserListEntry()
{
	if(m_pAvatarPixmap)
		detachAvatarData();
}

void KviUserListEntry::detachAvatarData()
{
	if(!m_pAvatarPixmap)
		return;

	m_pAvatarPixmap = nullptr;
}

void KviUserListEntry::updateAvatarData()
{
	detachAvatarData();

	if(!KVI_OPTION_BOOL(KviOption_boolShowAvatarsInUserlist))
		return;

	KviAvatar * pAv = m_pGlobalData->avatar();

	if(!pAv)
		return;

	if(
	    KVI_OPTION_BOOL(KviOption_boolScaleAvatars) && ((!KVI_OPTION_BOOL(KviOption_boolDoNotUpscaleAvatars)) || ((unsigned int)pAv->size().width() > KVI_OPTION_UINT(KviOption_uintAvatarScaleWidth)) || ((unsigned int)pAv->size().height() > KVI_OPTION_UINT(KviOption_uintAvatarScaleHeight))))
	{
		m_pAvatarPixmap = pAv->forSize(
		    KVI_OPTION_UINT(KviOption_uintAvatarScaleWidth),
		    KVI_OPTION_UINT(KviOption_uintAvatarScaleHeight));
	}
	else
	{
		m_pAvatarPixmap = pAv->animatedPixmap();
	}

	if(!m_pAvatarPixmap)
		return;

	if(!KVI_OPTION_BOOL(KviOption_boolEnableAnimatedAvatars))
		return;

	QObject::connect(m_pAvatarPixmap, SIGNAL(frameChanged()), this, SLOT(avatarFrameChanged()));
	QObject::connect(m_pAvatarPixmap, SIGNAL(destroyed()), this, SLOT(avatarDestroyed()));

	m_pAvatarPixmap->start();
}

void KviUserListEntry::avatarFrameChanged()
{
	m_pListView->animatedAvatarUpdated(this);
}

void KviUserListEntry::avatarDestroyed()
{
	m_pAvatarPixmap = nullptr;
}

bool KviUserListEntry::color(QColor & color)
{
	//
	// Used in KviIrcView::getTextLine
	//
	KviRegisteredUser * pRegisteredUser = m_pListView->m_pKviWindow->connection()->userDataBase()->registeredUser(m_szNick);
	if(pRegisteredUser)
	{
		if(m_pListView->m_pKviWindow->connection()->userDataBase()->haveCustomColor(m_szNick))
		{
			color = *(m_pListView->m_pKviWindow->connection()->userDataBase()->customColor(m_szNick));
			return true;
		}
	}

	if(KVI_OPTION_BOOL(KviOption_boolUseDifferentColorForOwnNick) && m_pListView->m_pKviWindow->connection())
	{
		if(m_szNick == m_pListView->m_pKviWindow->connection()->currentNickName())
		{
			color = KVI_OPTION_COLOR(KviOption_colorUserListViewOwnForeground);
			return true;
		}
	}

	if(m_iFlags == 0 && !globalData()->isIrcOp())
	{
		color = KVI_OPTION_COLOR(KviOption_colorUserListViewNormalForeground);
		return true;
	}
	else
	{
		color = KVI_OPTION_COLOR((globalData()->isIrcOp()) ? KviOption_colorUserListViewIrcOpForeground : ((m_iFlags & KviIrcUserEntry::ChanOwner) ? KviOption_colorUserListViewChanOwnerForeground : ((m_iFlags & KviIrcUserEntry::ChanAdmin) ? KviOption_colorUserListViewChanAdminForeground : ((m_iFlags & KviIrcUserEntry::Op) ? KviOption_colorUserListViewOpForeground : ((m_iFlags & KviIrcUserEntry::HalfOp) ? KviOption_colorUserListViewHalfOpForeground : ((m_iFlags & KviIrcUserEntry::Voice) ? KviOption_colorUserListViewVoiceForeground : KviOption_colorUserListViewUserOpForeground))))));
		return true;
	}
	return true;
}

void KviUserListEntry::recalcSize()
{
	m_iHeight = m_pListView->m_iFontHeight;

	if((KVI_OPTION_BOOL(KviOption_boolShowUserChannelIcons) || KVI_OPTION_BOOL(KviOption_boolDrawGenderIcons) || KVI_OPTION_BOOL(KviOption_boolShowUserChannelState)) && (m_iHeight < 20))
		m_iHeight = 20;

	if(m_pAvatarPixmap)
		m_iHeight += m_pAvatarPixmap->size().height();

	m_iHeight += 3;
}

KviUserListView::KviUserListView(QWidget * pParent, KviWindowToolPageButton * pButton, KviIrcUserDataBase * pDb, KviWindow * pWnd, int iDictSize, const QString & szTextLabel, const char * pName)
    : KviWindowToolWidget(pParent, pButton)
{
	setObjectName(pName);

	m_pKviWindow = pWnd;
	m_pEntryDict = new KviPointerHashTable<QString, KviUserListEntry>(iDictSize, false);
	m_pEntryDict->setAutoDelete(true);

	m_pUsersLabel = new QLabel(this);
	m_pUsersLabel->setObjectName("userslabel");
	KviTalToolTip::add(m_pUsersLabel, szTextLabel);

	m_pViewArea = new KviUserListViewArea(this);
	m_pToolTip = new KviUserListToolTip(this, m_pViewArea);
	m_pTopItem = nullptr;
	m_pHeadItem = nullptr;
	m_pTailItem = nullptr;
	m_iOpCount = 0;
	m_iHalfOpCount = 0;
	m_iVoiceCount = 0;
	m_iChanAdminCount = 0;
	m_iChanOwnerCount = 0;
	m_iUserOpCount = 0;
	m_pIrcUserDataBase = pDb;
	m_iTotalHeight = 0;
	m_ibEntries = 0;
	m_ieEntries = 0;
	m_iIEntries = 0;
	m_iSelectedCount = 0;

	applyOptions();
}

KviUserListView::~KviUserListView()
{
	removeAllEntries();
	delete m_pEntryDict;
	delete m_pToolTip;
	delete m_pViewArea;
	delete m_pUsersLabel;
}

void KviUserListView::emitRightClick()
{
	int iEv = -1;
	switch(m_pKviWindow->type())
	{
		case KviWindow::Channel:
			iEv = KviEvent_OnChannelNickPopupRequest;
			break;
		case KviWindow::Query:
			iEv = KviEvent_OnQueryNickPopupRequest;
			break;
		case KviWindow::Console:
			iEv = KviEvent_OnNotifyListPopupRequest;
			break;
		default:
			iEv = KviEvent_OnNickLinkPopupRequest;
			break; // this should actually never happen
	}

	if(iEv > -1)
	{
		if(KviKvsEventManager::instance()->hasAppHandlers(iEv))
		{
			QString szNicks;
			appendSelectedNicknames(szNicks);
			KviKvsVariantList vList;
			vList.append(szNicks);
			KviKvsEventManager::instance()->trigger(iEv, m_pKviWindow, &vList);
		}
		else
		{
			g_pApp->checkSuggestRestoreDefaultScript();
		}
	}
}

void KviUserListView::emitDoubleClick()
{
	int iEv = -1;
	switch(m_pKviWindow->type())
	{
		case KviWindow::Channel:
			iEv = KviEvent_OnChannelNickDefaultActionRequest;
			break;
		case KviWindow::Query:
			iEv = KviEvent_OnQueryNickDefaultActionRequest;
			break;
		case KviWindow::Console:
			iEv = KviEvent_OnNotifyListDefaultActionRequest;
			break;
		default:
			iEv = KviEvent_OnNickLinkDefaultActionRequest;
			break; // this should actually never happen
	}

	if(iEv > -1)
	{
		if(KviKvsEventManager::instance()->hasAppHandlers(iEv))
		{
			QString szNicks;
			appendSelectedNicknames(szNicks);
			KviKvsVariantList vList;
			vList.append(szNicks);
			KviKvsEventManager::instance()->trigger(iEv, m_pKviWindow, &vList);
		}
		else
		{
			g_pApp->checkSuggestRestoreDefaultScript();
		}
	}
}

void KviUserListView::updateScrollBarRange()
{
	int iMax = m_iTotalHeight - (m_pViewArea->height() - (KVI_USERLIST_BORDER_WIDTH * 2));
	m_pViewArea->m_pScrollBar->setRange(0, iMax > 0 ? iMax : 0);
	m_pViewArea->m_pScrollBar->setVisible(iMax > 0);
}

void KviUserListView::applyOptions()
{
	setFont(KVI_OPTION_FONT(KviOption_fontUserListView));
	QFontMetrics fm(font());

	m_iFontHeight = fm.lineSpacing();
	m_pViewArea->m_pScrollBar->setSingleStep(m_iFontHeight);

	if(KVI_OPTION_UINT(KviOption_uintUserListMinimumWidth) < 50)
		KVI_OPTION_UINT(KviOption_uintUserListMinimumWidth) = 50;

	setMinimumWidth(KVI_OPTION_UINT(KviOption_uintUserListMinimumWidth));

	KviUserListEntry * pEntry = m_pHeadItem;

	//reset scrollarea position and scrollbar position
	m_pTopItem = m_pHeadItem;
	m_pViewArea->m_pScrollBar->setValue(0);

	m_iTotalHeight = 0;
	while(pEntry)
	{
		pEntry->updateAvatarData();
		pEntry->recalcSize();
		m_iTotalHeight += pEntry->m_iHeight;
		pEntry = pEntry->m_pNext;
	}
	updateScrollBarRange();
	m_pUsersLabel->setFont(KVI_OPTION_FONT(KviOption_fontUserListView));
	resizeEvent(nullptr); // this will call update() too
	repaint();
}

void KviUserListView::updateArea()
{
	bool bEnable = m_pViewArea->updatesEnabled();
	if(!bEnable)
		m_pViewArea->setUpdatesEnabled(true);

	triggerUpdate();

	if(!bEnable)
		m_pViewArea->setUpdatesEnabled(false);
}

void KviUserListView::enableUpdates(bool bEnable)
{
	m_pViewArea->setUpdatesEnabled(bEnable);
	if(bEnable)
		triggerUpdate();
}

void KviUserListView::setMaskEntries(char cType, int iNum)
{
	switch(cType)
	{
		case 'b':
			m_ibEntries = iNum;
			break;
		case 'e':
			m_ieEntries = iNum;
			break;
		case 'I':
			m_iIEntries = iNum;
			break;
	}
	updateUsersLabel();
}

void KviUserListView::animatedAvatarUpdated(KviUserListEntry * e)
{
	// FIXME: This sucks
	if(!m_pTopItem)
		return;

	if(!KVI_OPTION_BOOL(KviOption_boolShowAvatarsInUserlist))
		return;

	QRect rct;
	KviUserListEntry * pEntry = m_pTopItem;
	int iCurTop = KVI_USERLIST_BORDER_WIDTH - m_pViewArea->m_iTopItemOffset;
	int iCurBottom = 0;

	int iBaseX = KVI_USERLIST_BORDER_WIDTH + KVI_USERLIST_ICON_MARGIN + 1;
	int iBaseY = 2;
	if(KVI_OPTION_BOOL(KviOption_boolDrawGenderIcons))
		iBaseX += KVI_USERLIST_ICON_WIDTH + KVI_USERLIST_ICON_MARGIN;
	if(KVI_OPTION_BOOL(KviOption_boolShowUserChannelIcons))
		iBaseX += KVI_USERLIST_ICON_WIDTH + KVI_USERLIST_ICON_MARGIN;
	if(KVI_OPTION_BOOL(KviOption_boolShowUserChannelState))
		iBaseX += KVI_USERLIST_ICON_STATE_WIDTH + KVI_USERLIST_ICON_MARGIN;
	if(KVI_OPTION_BOOL(KviOption_boolUserListViewDrawGrid))
	{
		switch(KVI_OPTION_UINT(KviOption_uintUserListViewGridType))
		{
			case KVI_USERLISTVIEW_GRIDTYPE_PLAINGRID:
			case KVI_USERLISTVIEW_GRIDTYPE_DOTGRID:
				break;
			default: // KVI_USERLISTVIEW_GRIDTYPE_3DGRID and KVI_USERLISTVIEW_GRIDTYPE_3DBUTTONS
				if(pEntry->m_bSelected)
					iBaseY++;
				break;
		}
		iBaseX += 3;
	}

	while(pEntry && (iCurTop <= m_pViewArea->height()))
	{
		iCurBottom = iCurTop + pEntry->m_iHeight;
		if(pEntry == e)
		{
			rct.setX(iBaseX);
			rct.setY(iCurTop + iBaseY);
			rct.setWidth(pEntry->m_pAvatarPixmap->pixmap()->size().width());
			rct.setHeight(pEntry->m_pAvatarPixmap->pixmap()->size().height());

			m_pViewArea->update(rct);
			break;
		}
		iCurTop = iCurBottom;
		pEntry = pEntry->m_pNext;
	}
}

void KviUserListView::completeNickBashLike(const QString & szBegin, std::vector<QString> & pList, bool bAppendMask)
{
	KviUserListEntry * pEntry = m_pHeadItem;
	while(pEntry)
	{
		bool bEqual = KviQString::equalCIN(szBegin, pEntry->m_szNick, szBegin.length());
		if(!bEqual && KVI_OPTION_BOOL(KviOption_boolIgnoreSpecialCharactersInNickCompletion))
		{
			QString szTmp = pEntry->m_szNick;
			szTmp.remove(QRegExp("[^a-zA-Z0-9]"));
			bEqual = KviQString::equalCIN(szBegin, szTmp, szBegin.length());
		}

		if(bEqual)
		{
			if(bAppendMask)
				pList.push_back(QString("%1!%2@%3").arg(pEntry->m_szNick, pEntry->m_pGlobalData->user(), pEntry->m_pGlobalData->host()));
			else
				pList.push_back(pEntry->m_szNick);
		}
		pEntry = pEntry->m_pNext;
	}
}

bool KviUserListView::completeNickLastAction(const QString & szBegin, const QString & szSkipAfter, QString & szBuffer, bool bAppendMask)
{
	KviUserListEntry * pLastMatch = findEntry(szSkipAfter);
	KviUserListEntry * pBestMatch = nullptr;

	bool bUseNextEqual = false;
	KviUserListEntry * pEntry = m_pHeadItem;
	while(pEntry)
	{
		if(pLastMatch && pEntry == pLastMatch)
			bUseNextEqual = true;
		else if(pEntry->m_szNick.length() >= szBegin.length())
		{
			bool bEqual = KviQString::equalCIN(szBegin, pEntry->m_szNick, szBegin.length());
			if(!bEqual && KVI_OPTION_BOOL(KviOption_boolIgnoreSpecialCharactersInNickCompletion))
			{
				QString szTmp = pEntry->m_szNick;
				szTmp.remove(QRegExp("[^a-zA-Z0-9]"));
				bEqual = KviQString::equalCIN(szBegin, szTmp, szBegin.length());
			}

			if(bEqual)
			{
				if(!pLastMatch && !pBestMatch)
					pBestMatch = pEntry;
				else if(!pLastMatch && pBestMatch)
				{
					if(pEntry->m_lastActionTime > pBestMatch->m_lastActionTime)
						pBestMatch = pEntry;
				}
				else if(!pBestMatch)
				{
					if(pLastMatch->m_lastActionTime > pEntry->m_lastActionTime)
						pBestMatch = pEntry;
					else if(bUseNextEqual && pLastMatch->m_lastActionTime == pEntry->m_lastActionTime)
					{
						pBestMatch = pEntry;
						bUseNextEqual = false;
					}
				}
				else
				{
					if((pLastMatch->m_lastActionTime > pEntry->m_lastActionTime) && (pEntry->m_lastActionTime > pBestMatch->m_lastActionTime))
						pBestMatch = pEntry;
					else if(bUseNextEqual && pLastMatch->m_lastActionTime == pEntry->m_lastActionTime)
					{
						pBestMatch = pEntry;
						bUseNextEqual = false;
					}
				}
			}
		}
		pEntry = pEntry->m_pNext;
	}

	if(pBestMatch)
	{
		szBuffer = pBestMatch->m_szNick;
		if(bAppendMask)
		{
			szBuffer += "!";
			szBuffer += pBestMatch->m_pGlobalData->user();
			szBuffer += "@";
			szBuffer += pBestMatch->m_pGlobalData->host();
		}
		return true;
	}
	return false;
}

bool KviUserListView::completeNickStandard(const QString & szBegin, const QString & szSkipAfter, QString & szBuffer, bool bAppendMask)
{
	if(KVI_OPTION_BOOL(KviOption_boolPrioritizeLastActionTime))
		return completeNickLastAction(szBegin, szSkipAfter, szBuffer, bAppendMask);

	KviUserListEntry * pEntry = m_pHeadItem;

	if(!szSkipAfter.isEmpty())
	{
		while(pEntry)
		{
			if(KviQString::equalCI(szSkipAfter, pEntry->m_szNick))
			{
				pEntry = pEntry->m_pNext;
				break;
			}
			pEntry = pEntry->m_pNext;
		}
	}

	// FIXME: completion should skip my own nick or place it as last entry in the chain (?)

	//	if(KviConsoleWindow * c = m_pKviWindow->console())
	//	{
	//		if(kvi_strEqualCI(entry->m_szNick.ptr(),c->currentNickName())
	//	}

	// Ok...now the real completion
	while(pEntry)
	{
		if(pEntry->m_szNick.length() >= szBegin.length())
		{
			bool bEqual = KviQString::equalCIN(szBegin, pEntry->m_szNick, szBegin.length());
			if(!bEqual && KVI_OPTION_BOOL(KviOption_boolIgnoreSpecialCharactersInNickCompletion))
			{
				QString szTmp = pEntry->m_szNick;
				szTmp.remove(QRegExp("[^a-zA-Z0-9]"));
				bEqual = KviQString::equalCIN(szBegin, szTmp, szBegin.length());
			}

			if(bEqual)
			{
				// This is ok.
				szBuffer = pEntry->m_szNick;
				if(bAppendMask)
				{
					szBuffer += "!";
					szBuffer += pEntry->m_pGlobalData->user();
					szBuffer += "@";
					szBuffer += pEntry->m_pGlobalData->host();
				}
				return true;
			}
		}
		pEntry = pEntry->m_pNext;
	}

	return false;
}

void KviUserListView::insertUserEntry(const QString & szNnick, KviUserListEntry * pUserEntry)
{
	// Complex insertion task :)
	m_pEntryDict->insert(szNnick, pUserEntry);
	m_iTotalHeight += pUserEntry->m_iHeight;

	bool bGotTopItem = false;
	bool bModeqHasPrefix = m_pKviWindow->connection()->serverInfo()->isSupportedModeFlag('q');
	bool bModeaHasPrefix = m_pKviWindow->connection()->serverInfo()->isSupportedModeFlag('a');

	int iFlag = 0;
	if(pUserEntry->m_iFlags != 0)
	{
		if(pUserEntry->m_iFlags & KviIrcUserEntry::UserOp)
		{
			iFlag = KviIrcUserEntry::UserOp;
			m_iUserOpCount++;
		}

		if(pUserEntry->m_iFlags & KviIrcUserEntry::Voice)
		{
			iFlag = KviIrcUserEntry::Voice;
			m_iVoiceCount++;
		}

		if(pUserEntry->m_iFlags & KviIrcUserEntry::HalfOp)
		{
			iFlag = KviIrcUserEntry::HalfOp;
			m_iHalfOpCount++;
		}

		if(pUserEntry->m_iFlags & KviIrcUserEntry::Op)
		{
			iFlag = KviIrcUserEntry::Op;
			m_iOpCount++;
		}

		if(pUserEntry->m_iFlags & KviIrcUserEntry::ChanAdmin)
		{
			if(bModeaHasPrefix)
				iFlag = KviIrcUserEntry::ChanAdmin;
			m_iChanAdminCount++;
		}

		if(pUserEntry->m_iFlags & KviIrcUserEntry::ChanOwner)
		{
			if(bModeqHasPrefix)
				iFlag = KviIrcUserEntry::ChanOwner;
			m_iChanOwnerCount++;
		}
	}

	//FIXME this should probably be handled in a different way (place)
	if(pUserEntry->globalData()->isIrcOp())
	{
		m_iIrcOpCount++;
	}

	if(m_pHeadItem)
	{
		KviUserListEntry * pEntry = m_pHeadItem;

		if(!(pUserEntry->m_iFlags & KviIrcUserEntry::ChanOwner) || !bModeqHasPrefix)
		{
			// the new user is not a channel owner...
			// skip the channel owners
			while(pEntry && (pEntry->m_iFlags & KviIrcUserEntry::ChanOwner) && bModeqHasPrefix)
			{
				if(pEntry == m_pTopItem)
					bGotTopItem = true;
				pEntry = pEntry->m_pNext;
			}

			if(!(pUserEntry->m_iFlags & KviIrcUserEntry::ChanAdmin) || !bModeaHasPrefix)
			{
				// the new user is not a channel admin...
				// skip chan admins
				while(pEntry && (pEntry->m_iFlags & KviIrcUserEntry::ChanAdmin) && bModeaHasPrefix)
				{
					if(pEntry == m_pTopItem)
						bGotTopItem = true;
					pEntry = pEntry->m_pNext;
				}

				// is operator ?
				if(!(pUserEntry->m_iFlags & KviIrcUserEntry::Op))
				{
					// the new user is not an op...
					// skip ops
					while(pEntry && (pEntry->m_iFlags & KviIrcUserEntry::Op))
					{
						if(pEntry == m_pTopItem)
							bGotTopItem = true;
						pEntry = pEntry->m_pNext;
					}

					// is half oped ?
					if(!(pUserEntry->m_iFlags & KviIrcUserEntry::HalfOp))
					{
						// nope, skip halfops
						while(pEntry && (pEntry->m_iFlags & KviIrcUserEntry::HalfOp))
						{
							if(pEntry == m_pTopItem)
								bGotTopItem = true;
							pEntry = pEntry->m_pNext;
						}

						// is voiced ?
						if(!(pUserEntry->m_iFlags & KviIrcUserEntry::Voice))
						{
							// nope, not voiced so skip voiced users
							while(pEntry && (pEntry->m_iFlags & KviIrcUserEntry::Voice))
							{
								if(pEntry == m_pTopItem)
									bGotTopItem = true;
								pEntry = pEntry->m_pNext;
							}

							// is userop'd?
							if(!(pUserEntry->m_iFlags & KviIrcUserEntry::UserOp))
							{
								// nope, skip userops
								while(pEntry && (pEntry->m_iFlags & KviIrcUserEntry::UserOp))
								{
									if(pEntry == m_pTopItem)
										bGotTopItem = true;
									pEntry = pEntry->m_pNext;
								}
							} // else is userop, ops, halfops, and voiced are skipped
						}         // else it is voiced, ops and halfops are skipped
					}                 // else it is halfop,  ops are skipped
				}                         // else it is op, chan admins are skipped
			}                                 // else it is chan admin, chan owners are skipped
		}                                         // else it is chan owner, so nothing to skip: they are first in the list

		// now strcmp within the current user-flag group...
		while(pEntry && (KviQString::cmpCI(pEntry->m_szNick, pUserEntry->m_szNick,
		                     KVI_OPTION_BOOL(KviOption_boolPlaceNickWithNonAlphaCharsAtEnd))
		                    < 0)
		    && ((pEntry->m_iFlags & iFlag) || (iFlag == 0)))
		{
			if(pEntry == m_pTopItem)
				bGotTopItem = true;
			pEntry = pEntry->m_pNext;
		}

		if(pEntry)
		{
			// inserting
			pUserEntry->m_pNext = pEntry;
			pUserEntry->m_pPrev = pEntry->m_pPrev;
			if(pUserEntry->m_pPrev == nullptr)
				m_pHeadItem = pUserEntry;
			else
				pUserEntry->m_pPrev->m_pNext = pUserEntry;

			pEntry->m_pPrev = pUserEntry;
			// need to adjust the item offsets now...
			// ok... if we're inserting something after
			// the top item, we move everything down
			// otherwise we only update the scrollbar values
			if(!bGotTopItem)
			{
				// Inserting BEFORE the top item
				if((pUserEntry == m_pHeadItem) && (m_pTopItem == pUserEntry->m_pNext) && (m_pViewArea->m_iTopItemOffset == 0))
				{
					// special case...the top item is the head one
					// and it has zero offset...change the top item too
					m_pTopItem = pUserEntry;
					triggerUpdate();
				}
				else
				{
					// invisible insertion
					m_pViewArea->m_bIgnoreScrollBar = true;
					m_pViewArea->m_iLastScrollBarVal += pUserEntry->m_iHeight;
					updateScrollBarRange();
					m_pViewArea->m_pScrollBar->setValue(m_pViewArea->m_iLastScrollBarVal);
					m_pViewArea->m_bIgnoreScrollBar = false;
					updateUsersLabel();
				}
			}
			else
			{
				triggerUpdate();
			}
		}
		else
		{
			// appending to the end (may be visible)
			m_pTailItem->m_pNext = pUserEntry;
			pUserEntry->m_pNext = nullptr;
			pUserEntry->m_pPrev = m_pTailItem;
			m_pTailItem = pUserEntry;
			triggerUpdate();
		}
	}
	else
	{
		// There were no items (is rather visible)
		m_pHeadItem = pUserEntry;
		m_pTailItem = pUserEntry;
		m_pTopItem = pUserEntry;
		pUserEntry->m_pNext = nullptr;
		pUserEntry->m_pPrev = nullptr;
		triggerUpdate();
	}

	if(pUserEntry->m_bSelected)
	{
		m_iSelectedCount++;
		if(m_iSelectedCount == 1)
			g_pMainWindow->childWindowSelectionStateChange(m_pKviWindow, true);
	}
}

KviUserListEntry * KviUserListView::join(const QString & szNick, const QString & szUser, const QString & szHost, int iFlags)
{
	KviUserListEntry * pEntry = m_pEntryDict->find(szNick);
	if(!pEntry)
	{
		// add an entry to the global dict
		KviIrcUserEntry * pGlobalData = m_pIrcUserDataBase->insertUser(szNick, szUser, szHost);
		// calculate the flags and update the counters
		pEntry = new KviUserListEntry(this, szNick, pGlobalData, iFlags, (szUser == QString()));
		insertUserEntry(szNick, pEntry);
	}
	else
	{
		//		if(!host.isEmpty()) - it can be UHNAMES with host or NAMEX(X) w/o it
		//		{
		// Ok...the user was already on...
		// Probably this is a NAMES(X) reply, and the user IS_ME
		// (already joined after the JOIN message)
		if(iFlags != pEntry->m_iFlags)
		{
			//// FIXME: #warning "Maybe say to the channel that we're oped : and the op is guessed from the names reply"
			if((iFlags & KviIrcUserEntry::ChanOwner) != (pEntry->m_iFlags & KviIrcUserEntry::ChanOwner))
				setChanOwner(szNick, iFlags & KviIrcUserEntry::ChanOwner);

			if((iFlags & KviIrcUserEntry::ChanAdmin) != (pEntry->m_iFlags & KviIrcUserEntry::ChanAdmin))
				setChanAdmin(szNick, iFlags & KviIrcUserEntry::ChanAdmin);

			if((iFlags & KviIrcUserEntry::Op) != (pEntry->m_iFlags & KviIrcUserEntry::Op))
				setOp(szNick, iFlags & KviIrcUserEntry::Op);

			if((iFlags & KviIrcUserEntry::HalfOp) != (pEntry->m_iFlags & KviIrcUserEntry::HalfOp))
				setHalfOp(szNick, iFlags & KviIrcUserEntry::HalfOp);

			if((iFlags & KviIrcUserEntry::Voice) != (pEntry->m_iFlags & KviIrcUserEntry::Voice))
				setVoice(szNick, iFlags & KviIrcUserEntry::Voice);

			if((iFlags & KviIrcUserEntry::UserOp) != (pEntry->m_iFlags & KviIrcUserEntry::UserOp))
				setUserOp(szNick, iFlags & KviIrcUserEntry::UserOp);
		}
		//		}
	}
	return pEntry;
}

void KviUserListView::triggerUpdate()
{
	// This stuff is useful on joins only
	if(m_pViewArea->updatesEnabled())
	{
		updateScrollBarRange();
		m_pViewArea->update();
		updateUsersLabel();
	}
}

bool KviUserListView::avatarChanged(const QString & szNick)
{
	KviUserListEntry * pUserEntry = m_pEntryDict->find(szNick);
	if(!pUserEntry)
		return false;

	int iOldHeight = pUserEntry->m_iHeight;
	m_iTotalHeight -= pUserEntry->m_iHeight;
	pUserEntry->updateAvatarData();
	pUserEntry->recalcSize();
	m_iTotalHeight += pUserEntry->m_iHeight;
	// if this was "over" the top item, we must adjust the scrollbar value
	// otherwise scroll everything down
	KviUserListEntry * pEntry = m_pHeadItem;
	bool bGotTopItem = false;
	while(pEntry != pUserEntry)
	{
		if(pEntry == m_pTopItem)
		{
			bGotTopItem = true;
			pEntry = pUserEntry;
		}
		else
			pEntry = pEntry->m_pNext;
	}

	if(!bGotTopItem && (m_pTopItem != pUserEntry))
	{
		// we're "over" the top item, so over the
		// upper side of the view...adjust the scroll bar value
		int iHeightDiff = pUserEntry->m_iHeight - iOldHeight;
		m_pViewArea->m_iLastScrollBarVal += iHeightDiff;
		m_pViewArea->m_bIgnoreScrollBar = true;
		updateScrollBarRange();
		m_pViewArea->m_pScrollBar->setValue(m_pViewArea->m_iLastScrollBarVal);
		m_pViewArea->m_bIgnoreScrollBar = false;
	}
	else
	{
		// the item may be visible!
		// the scroll bar should take care of the case
		// in that the current value runs out of the allowed
		// range.... it should set the value to a good one
		// and emit the signal
		updateScrollBarRange();
		m_pViewArea->update();
	}
	return true;
}

bool KviUserListView::userActionVerifyMask(const QString & szNick, const QString & szUser, const QString & szHost, int iActionTemperature, QString & szOldUser, QString & szOldHost)
{
	KviUserListEntry * pEntry = m_pEntryDict->find(szNick);
	if(pEntry)
	{
		pEntry->m_lastActionTime = kvi_unixTime();
		bool bChanged = false;

		if(!(szHost.isEmpty() || (KviQString::equalCS(szHost, "*"))))
		{
			if(!KviQString::equalCI(pEntry->m_pGlobalData->host(), szHost))
			{
				if(!(pEntry->m_pGlobalData->host().isEmpty() || KviQString::equalCS(pEntry->m_pGlobalData->host(), "*")))
				{
					szOldHost = pEntry->m_pGlobalData->host();
					bChanged = true;
				}
				pEntry->m_pGlobalData->setHost(szHost);
			}
		}

		if(!(szUser.isEmpty() || (KviQString::equalCS(szUser, "*"))))
		{
			if(!KviQString::equalCI(pEntry->m_pGlobalData->user(), szUser))
			{
				if(!(pEntry->m_pGlobalData->user().isEmpty() || KviQString::equalCS(pEntry->m_pGlobalData->user(), "*")))
				{
					szOldUser = pEntry->m_pGlobalData->user();
					bChanged = true;
				}
				pEntry->m_pGlobalData->setUser(szUser);
			}
		}

		pEntry->m_iTemperature += iActionTemperature;
		// Don't allow it to grow too much
		if(pEntry->m_iTemperature > 300)
			pEntry->m_iTemperature = 300;
		else if(pEntry->m_iTemperature < -300)
			pEntry->m_iTemperature = -300;

		if(itemVisible(pEntry))
			triggerUpdate();

		return !bChanged;
	}
	return true; // no such nick so no change
}

void KviUserListView::userAction(const QString & szNick, const QString & szUser, const QString & szHost, int iActionTemperature)
{
	KviUserListEntry * pEntry = m_pEntryDict->find(szNick);
	if(pEntry)
	{
		pEntry->m_lastActionTime = kvi_unixTime();
		if(!(szHost.isEmpty() || (KviQString::equalCS(szHost, "*"))))
			pEntry->m_pGlobalData->setHost(szHost);
		if(!(szUser.isEmpty() || (KviQString::equalCS(szUser, "*"))))
			pEntry->m_pGlobalData->setUser(szUser);
		pEntry->m_iTemperature += iActionTemperature;

		// Don't allow it to grow too much
		if(pEntry->m_iTemperature > 300)
			pEntry->m_iTemperature = 300;
		else if(pEntry->m_iTemperature < -300)
			pEntry->m_iTemperature = -300;

		if(itemVisible(pEntry))
			triggerUpdate();
	}
}

void KviUserListView::userAction(KviIrcMask * pUser, int iActionTemperature)
{
	KviUserListEntry * pEntry = m_pEntryDict->find(pUser->nick());
	if(pEntry)
	{
		pEntry->m_lastActionTime = kvi_unixTime();
		if(pUser->hasUser())
			pEntry->m_pGlobalData->setUser(pUser->user());
		if(pUser->hasHost())
			pEntry->m_pGlobalData->setHost(pUser->host());
		pEntry->m_iTemperature += iActionTemperature;

		// Don't allow it to grow too much
		if(pEntry->m_iTemperature > 300)
			pEntry->m_iTemperature = 300;
		else if(pEntry->m_iTemperature < -300)
			pEntry->m_iTemperature = -300;

		if(itemVisible(pEntry))
			triggerUpdate();
	}
}

void KviUserListView::userAction(const QString & szNick, int iActionTemperature)
{
	KviUserListEntry * pEntry = m_pEntryDict->find(szNick);
	if(pEntry)
	{
		pEntry->m_lastActionTime = kvi_unixTime();
		pEntry->m_iTemperature += iActionTemperature;

		if(pEntry->m_iTemperature > 300)
			pEntry->m_iTemperature = 300;
		else if(pEntry->m_iTemperature < -300)
			pEntry->m_iTemperature = -300;

		if(itemVisible(pEntry))
			triggerUpdate();
	}
}

kvi_time_t KviUserListView::getUserJoinTime(const QString & szNick)
{
	KviUserListEntry * pEntry = m_pEntryDict->find(szNick);
	if(!pEntry)
		return (kvi_time_t)0;

	return pEntry->m_joinTime;
}

kvi_time_t KviUserListView::getUserLastActionTime(const QString & szNick)
{
	KviUserListEntry * pEntry = m_pEntryDict->find(szNick);
	if(!pEntry)
		return (kvi_time_t)0;

	return pEntry->m_lastActionTime;
}

int KviUserListView::getUserModeLevel(const QString & szNick)
{
	KviUserListEntry * pEntry = m_pEntryDict->find(szNick);
	if(!pEntry)
		return 0;

	if(pEntry->m_iFlags & KviIrcUserEntry::ModeMask)
	{
		if(pEntry->m_iFlags & KviIrcUserEntry::ChanOwner)
			return 60;
		if(pEntry->m_iFlags & KviIrcUserEntry::ChanAdmin)
			return 50;
		if(pEntry->m_iFlags & KviIrcUserEntry::Op)
			return 40;
		if(pEntry->m_iFlags & KviIrcUserEntry::HalfOp)
			return 30;
		if(pEntry->m_iFlags & KviIrcUserEntry::Voice)
			return 20;
		if(pEntry->m_iFlags & KviIrcUserEntry::UserOp)
			return 10;
	}
	return 0;
}

char KviUserListView::getUserFlag(KviUserListEntry * pEntry)
{
	if(!pEntry)
		return 0;

	return (char)m_pKviWindow->connection()->serverInfo()->modePrefixChar(pEntry->m_iFlags).unicode();
}

void KviUserListView::prependUserFlag(const QString & szNick, QString & szBuffer)
{
	char uFlag = getUserFlag(szNick);
	if(uFlag)
		szBuffer.prepend(uFlag);
}

int KviUserListView::flags(const QString & szNick)
{
	KviUserListEntry * pEntry = m_pEntryDict->find(szNick);
	return pEntry ? pEntry->m_iFlags : 0;
}

#define SET_FLAG_FUNC(__funcname, __flag)                               \
	bool KviUserListView::__funcname(const QString & szNick, bool bYes) \
	{                                                                   \
		KviUserListEntry * pEntry = m_pEntryDict->find(szNick);         \
		if(!pEntry)                                                     \
			return false;                                               \
		m_pEntryDict->setAutoDelete(false);                             \
		partInternal(szNick, false);                                    \
		m_pEntryDict->setAutoDelete(true);                              \
		if(bYes)                                                        \
		{                                                               \
			if(!(pEntry->m_iFlags & __flag))                            \
				pEntry->m_iFlags |= __flag;                             \
		}                                                               \
		else                                                            \
		{                                                               \
			if(pEntry->m_iFlags & __flag)                               \
				pEntry->m_iFlags &= ~__flag;                            \
		}                                                               \
		updateScrollBarRange();                                         \
		insertUserEntry(szNick, pEntry);                                \
		m_pViewArea->update();                                          \
		return true;                                                    \
	}

SET_FLAG_FUNC(setChanOwner, KviIrcUserEntry::ChanOwner)
SET_FLAG_FUNC(setChanAdmin, KviIrcUserEntry::ChanAdmin)
SET_FLAG_FUNC(setOp, KviIrcUserEntry::Op)
SET_FLAG_FUNC(setHalfOp, KviIrcUserEntry::HalfOp)
SET_FLAG_FUNC(setUserOp, KviIrcUserEntry::UserOp)
SET_FLAG_FUNC(setVoice, KviIrcUserEntry::Voice)

#define GET_FLAG_FUNC(__funcname, __flag)                                                                \
	bool KviUserListView::__funcname(const QString & szNick, bool bAtLeast)                              \
	{                                                                                                    \
		KviUserListEntry * pEntry = m_pEntryDict->find(szNick);                                          \
		return pEntry ? (bAtLeast ? (pEntry->m_iFlags >= __flag) : (pEntry->m_iFlags & __flag)) : false; \
	}

GET_FLAG_FUNC(isChanOwner, KviIrcUserEntry::ChanOwner)
GET_FLAG_FUNC(isChanAdmin, KviIrcUserEntry::ChanAdmin)
GET_FLAG_FUNC(isOp, KviIrcUserEntry::Op)
GET_FLAG_FUNC(isVoice, KviIrcUserEntry::Voice)
GET_FLAG_FUNC(isHalfOp, KviIrcUserEntry::HalfOp)
GET_FLAG_FUNC(isUserOp, KviIrcUserEntry::UserOp)

QString * KviUserListView::firstSelectedNickname()
{
	m_pIterator = m_pHeadItem;
	while(m_pIterator)
	{
		if(m_pIterator->m_bSelected)
		{
			QString * szTmp = &(m_pIterator->m_szNick);
			m_pIterator = m_pIterator->m_pNext;
			return szTmp;
		}
		m_pIterator = m_pIterator->m_pNext;
	}
	return nullptr;
}

QString * KviUserListView::nextSelectedNickname()
{
	while(m_pIterator)
	{
		if(m_pIterator->m_bSelected)
		{
			QString * szTmp = &(m_pIterator->m_szNick);
			m_pIterator = m_pIterator->m_pNext;
			return szTmp;
		}
		m_pIterator = m_pIterator->m_pNext;
	}
	return nullptr;
}

void KviUserListView::appendSelectedNicknames(QString & szBuffer)
{
	KviUserListEntry * pEntry = m_pHeadItem;
	bool bFirst = true;
	while(pEntry)
	{
		if(pEntry->m_bSelected)
		{
			if(!bFirst)
				szBuffer.append(',');
			else
				bFirst = false;
			szBuffer.append(pEntry->m_szNick);
		}
		pEntry = pEntry->m_pNext;
	}
}

void KviUserListView::select(const QString & szNick)
{
	KviPointerHashTableIterator<QString, KviUserListEntry> it(*m_pEntryDict);
	while(it.current())
	{
		((KviUserListEntry *)it.current())->m_bSelected = false;
		++it;
	}

	KviUserListEntry * pEntry = m_pEntryDict->find(szNick);
	if(pEntry)
	{
		pEntry->m_bSelected = true;
		m_iSelectedCount = 1;
	}
	else
	{
		m_iSelectedCount = 0;
	}

	g_pMainWindow->childWindowSelectionStateChange(m_pKviWindow, true);
	m_pViewArea->update();
}

void KviUserListView::ensureVisible(const QString & szNick)
{
	KviUserListEntry * pUserEntry = m_pEntryDict->find(szNick);
	if(!pUserEntry)
		return;

	// so, first of all..check if this item is over, or below the top item
	KviUserListEntry * pEntry = m_pHeadItem;
	bool bGotTopItem = false;
	bool bGotUserEntry = false;
	int iHeight = 0;
	while(pEntry)
	{
		if(pEntry == pUserEntry)
		{
			if(bGotTopItem)
			{
				iHeight += pEntry->m_iHeight;
				break;
			}
			else
			{
				bGotUserEntry = true;
			}
		}

		if(pEntry == m_pTopItem)
		{
			if(bGotUserEntry)
			{
				iHeight -= pEntry->m_iHeight;
				break;
			}
			else
			{
				bGotTopItem = true;
			}
		}

		if(bGotTopItem)
			iHeight += pEntry->m_iHeight;
		if(bGotUserEntry)
			iHeight -= pEntry->m_iHeight;
		pEntry = pEntry->m_pNext;
	}

	if(iHeight > m_pViewArea->height())
	{
		iHeight -= m_pViewArea->height();
	}
	else if(iHeight >= 0)
	{
		//already visible
		return;
	}

	//the scrollbar will clamp this value itself
	m_pViewArea->m_pScrollBar->setValue(m_pViewArea->m_pScrollBar->value() + iHeight);
	m_pViewArea->m_iLastScrollBarVal = m_pViewArea->m_pScrollBar->value();
}

bool KviUserListView::partInternal(const QString & szNick, bool bRemoveDefinitively)
{
	KviUserListEntry * pUserEntry = m_pEntryDict->find(szNick);
	if(!pUserEntry)
		return false; // not there

	// so, first of all..check if this item is over, or below the top item
	KviUserListEntry * pEntry = m_pHeadItem;
	bool bGotTopItem = false;
	while(pEntry != pUserEntry)
	{
		if(pEntry == m_pTopItem)
		{
			bGotTopItem = true;
			pEntry = pUserEntry;
		}
		else
			pEntry = pEntry->m_pNext;
	}

	// decrease counts first
	if(pUserEntry->m_pGlobalData->isIrcOp())
		m_iIrcOpCount--;
	if(pUserEntry->m_iFlags & KviIrcUserEntry::ChanOwner)
		m_iChanOwnerCount--;
	if(pUserEntry->m_iFlags & KviIrcUserEntry::ChanAdmin)
		m_iChanAdminCount--;
	if(pUserEntry->m_iFlags & KviIrcUserEntry::Op)
		m_iOpCount--;
	if(pUserEntry->m_iFlags & KviIrcUserEntry::HalfOp)
		m_iHalfOpCount--;
	if(pUserEntry->m_iFlags & KviIrcUserEntry::Voice)
		m_iVoiceCount--;
	if(pUserEntry->m_iFlags & KviIrcUserEntry::UserOp)
		m_iUserOpCount--;

	if(bRemoveDefinitively)
	{
		pUserEntry->detachAvatarData();
		m_pIrcUserDataBase->removeUser(szNick, pUserEntry->m_pGlobalData);
	}

	if(pUserEntry->m_bSelected)
	{
		m_iSelectedCount--;
		if(m_iSelectedCount == 0)
			g_pMainWindow->childWindowSelectionStateChange(m_pKviWindow, false);
	}
	if(pUserEntry->m_pPrev)
		pUserEntry->m_pPrev->m_pNext = pUserEntry->m_pNext;
	if(pUserEntry->m_pNext)
		pUserEntry->m_pNext->m_pPrev = pUserEntry->m_pPrev;
	if(m_pTopItem == pUserEntry)
	{
		bGotTopItem = true; // !!! the previous while() does not handle it!
		m_pTopItem = pUserEntry->m_pNext;
		if(m_pTopItem == nullptr)
			m_pTopItem = pUserEntry->m_pPrev;
	}
	if(pUserEntry == m_pHeadItem)
		m_pHeadItem = pUserEntry->m_pNext;
	if(pUserEntry == m_pTailItem)
		m_pTailItem = pUserEntry->m_pPrev;
	m_iTotalHeight -= pUserEntry->m_iHeight;

	int iHeight = pUserEntry->m_iHeight;

	m_pEntryDict->remove(szNick);

	if(bGotTopItem)
	{
		// removing after (or exactly) the top item, may be visible
		if(bRemoveDefinitively)
			triggerUpdate();
	}
	else
	{
		// removing over (before) the top item...not visible
		m_pViewArea->m_bIgnoreScrollBar = true;
		m_pViewArea->m_iLastScrollBarVal -= iHeight;
		m_pViewArea->m_pScrollBar->setValue(m_pViewArea->m_iLastScrollBarVal);
		updateScrollBarRange();
		m_pViewArea->m_bIgnoreScrollBar = false;
		if(bRemoveDefinitively)
			updateUsersLabel();
	}

	return true;
}

bool KviUserListView::nickChange(const QString & szOldNick, const QString & szNewNick)
{
	KviUserListEntry * pEntry = m_pEntryDict->find(szOldNick);
	if(pEntry)
	{
		QString szUser = pEntry->m_pGlobalData->user();
		QString szHost = pEntry->m_pGlobalData->host();
		QString szReal = pEntry->m_pGlobalData->realName();
		QString szAccount = pEntry->m_pGlobalData->accountName();
		QString szServer = pEntry->m_pGlobalData->server();
		int iFlags = pEntry->m_iFlags;
		kvi_time_t joint = pEntry->m_joinTime;
		bool bSelect = pEntry->m_bSelected;
		bool bIrcOp = pEntry->m_pGlobalData->isIrcOp();
		int iHops = pEntry->m_pGlobalData->hops();

		std::unique_ptr<KviAvatar> upAv = pEntry->m_pGlobalData->forgetAvatar();

		KviIrcUserEntry::Gender gender = pEntry->m_pGlobalData->gender();
		bool bBot = pEntry->m_pGlobalData->isBot();
		part(szOldNick);
		KVI_ASSERT(!m_pEntryDict->find(szOldNick));

		pEntry = join(szNewNick, szUser, szHost, iFlags);
		pEntry->m_pGlobalData->setGender(gender);
		pEntry->m_pGlobalData->setBot(bBot);
		pEntry->m_pGlobalData->setRealName(szReal);
		pEntry->m_pGlobalData->setAccountName(szAccount);
		pEntry->m_pGlobalData->setIrcOp(bIrcOp);
		pEntry->m_pGlobalData->setServer(szServer);
		pEntry->m_pGlobalData->setHops(iHops);
		pEntry->m_joinTime = joint;
		pEntry->m_lastActionTime = kvi_unixTime();
		pEntry->m_bSelected = bSelect;
		pEntry->m_iTemperature += KVI_USERACTION_NICK;

		if(upAv)
		{
			pEntry->m_pGlobalData->setAvatar(std::move(upAv));
			avatarChanged(szNewNick);
		}
		return true;
	}
	return false;
}

void KviUserListView::updateUsersLabel()
{
	if(!KVI_OPTION_BOOL(KviOption_boolShowUserListStatisticLabel))
		return;

	KviCString tmp;
	tmp.sprintf("<b>[%u]</b>", m_pEntryDict->count());
	if(m_iChanOwnerCount)
		tmp.append(KviCString::Format, " q:%d", m_iChanOwnerCount);
	if(m_iChanAdminCount)
		tmp.append(KviCString::Format, " a:%d", m_iChanAdminCount);
	if(m_iOpCount)
		tmp.append(KviCString::Format, " o:%d", m_iOpCount);
	if(m_iHalfOpCount)
		tmp.append(KviCString::Format, " h:%d", m_iHalfOpCount);
	if(m_iVoiceCount)
		tmp.append(KviCString::Format, " v:%d", m_iVoiceCount);
	if(m_iUserOpCount)
		tmp.append(KviCString::Format, " u:%d", m_iUserOpCount);
	if(m_ibEntries)
		tmp.append(KviCString::Format, " b:%d", m_ibEntries);
	if(m_ieEntries)
		tmp.append(KviCString::Format, " e:%d", m_ieEntries);
	if(m_iIEntries)
		tmp.append(KviCString::Format, " I:%d", m_iIEntries);
	m_pUsersLabel->setText(tmp.ptr());
}

// FIXME: this could be done really better
void KviUserListView::partAllButOne(const QString & szWhoNot)
{
	QStringList list;
	KviPointerHashTableIterator<QString, KviUserListEntry> it(*m_pEntryDict);
	while(it.current())
	{
		if(!KviQString::equalCI(szWhoNot, it.currentKey()))
			list.append(it.currentKey());
		++it;
	}
	for(auto & it2 : list)
	{
		part(it2);
	}
}

void KviUserListView::removeAllEntries()
{
	KviPointerHashTableIterator<QString, KviUserListEntry> it(*m_pEntryDict);
	while(it.current())
	{
		//it.current()->resetAvatarConnection();
		m_pIrcUserDataBase->removeUser(it.currentKey(),
		    ((KviUserListEntry *)it.current())->m_pGlobalData);
		++it;
	}

	m_pEntryDict->clear();
	m_pHeadItem = nullptr;
	m_pTopItem = nullptr;
	m_iVoiceCount = 0;
	m_iHalfOpCount = 0;
	m_iChanAdminCount = 0;
	m_iChanOwnerCount = 0;
	m_iOpCount = 0;
	m_iUserOpCount = 0;

	if(m_iSelectedCount != 0)
	{
		m_iSelectedCount = 0;
		g_pMainWindow->childWindowSelectionStateChange(m_pKviWindow, false);
	}

	m_pViewArea->m_iTopItemOffset = 0;
	m_pViewArea->m_iLastScrollBarVal = 0;
	m_pViewArea->m_bIgnoreScrollBar = true;
	m_pViewArea->m_pScrollBar->setValue(0);
	m_iTotalHeight = 0;
	updateScrollBarRange();
	m_pViewArea->m_bIgnoreScrollBar = false; // gfgf
}

void KviUserListView::partAll()
{
	removeAllEntries();
	triggerUpdate();
}

void KviUserListView::resizeEvent(QResizeEvent *)
{
	int iHeight;
	if(KVI_OPTION_BOOL(KviOption_boolShowUserListStatisticLabel)) //G&N  2005
	{
		iHeight = m_pUsersLabel->sizeHint().height();
		if(iHeight < 16)
			iHeight = 16; // at least
		m_pUsersLabel->setGeometry(0, 0, width(), iHeight);
	}
	else
	{
		iHeight = 0;
	}

	m_pViewArea->setGeometry(0, iHeight, width(), height() - iHeight);

	updateScrollBarRange();
}

bool KviUserListView::itemVisible(KviUserListEntry * e)
{
	KviUserListEntry * pEntry = m_pTopItem;
	int iCurTop = KVI_USERLIST_BORDER_WIDTH - m_pViewArea->m_iTopItemOffset;
	int iHeight = height();
	while(pEntry && (iCurTop < iHeight))
	{
		if(pEntry == e)
			return true;
		iCurTop += pEntry->m_iHeight;
		pEntry = pEntry->m_pNext;
	}
	return false;
}

KviUserListEntry * KviUserListView::itemAt(const QPoint & pnt, QRect * pRect)
{
	if(!m_pTopItem)
		return nullptr;
	if(pnt.y() < 0)
		return nullptr;

	int iCurTop = KVI_USERLIST_BORDER_WIDTH - m_pViewArea->m_iTopItemOffset;
	int iCurBottom = 0;
	KviUserListEntry * pEntry = m_pTopItem;
	while(pEntry && (iCurTop <= m_pViewArea->height()))
	{
		iCurBottom = iCurTop + pEntry->m_iHeight;
		if((pnt.y() >= iCurTop) && (pnt.y() < iCurBottom))
		{
			if(pRect)
			{
				pRect->setX(0);
				pRect->setY(iCurTop);
				pRect->setWidth(m_pViewArea->width());
				pRect->setHeight(pEntry->m_iHeight);
			}
			return pEntry;
		}
		iCurTop = iCurBottom;
		pEntry = pEntry->m_pNext;
	}
	return nullptr;
}

void KviUserListView::userStats(KviUserListViewUserStats * pStats)
{
	pStats->uTotal = m_pEntryDict->count();
	pStats->uHot = 0;
	pStats->uHotOp = 0;
	pStats->uActive = 0;
	pStats->uActiveOp = 0;
	pStats->uChanAdmin = 0;
	pStats->uChanOwner = 0;
	pStats->iAvgTemperature = 0;
	pStats->uOp = 0;
	pStats->uVoiced = 0;
	pStats->uHalfOp = 0;
	pStats->uUserOp = 0;
	pStats->uIrcOp = 0;

	KviUserListEntry * pEntry = m_pHeadItem;

	kvi_time_t curTime = kvi_unixTime();

	while(pEntry)
	{
		if(pEntry->m_lastActionTime)
		{
			unsigned int uTimeDiff = (((unsigned int)(curTime - pEntry->m_lastActionTime)) >> 6);
			if(uTimeDiff < 10)
			{
				pStats->uActive++; // the user was alive in the last ~16 mins
				if(pEntry->m_iFlags & (KviIrcUserEntry::Op | KviIrcUserEntry::ChanAdmin | KviIrcUserEntry::ChanOwner))
				{
					pStats->uActiveOp++;
					if(pEntry->m_iTemperature > 0)
					{
						pStats->uHot++;
						pStats->uHotOp++;
					}
				}
				else
				{
					if(pEntry->m_iTemperature > 0)
						pStats->uHot++;
				}
				pStats->iAvgTemperature += pEntry->m_iTemperature;
			}
		}

		if(pEntry->m_pGlobalData->isIrcOp())
			pStats->uIrcOp++;

		if(pEntry->m_iFlags & KviIrcUserEntry::ChanOwner)
			pStats->uChanOwner++;
		else
		{
			if(pEntry->m_iFlags & KviIrcUserEntry::ChanAdmin)
				pStats->uChanAdmin++;
			else
			{
				if(pEntry->m_iFlags & KviIrcUserEntry::Op)
					pStats->uOp++;
				else
				{
					if(pEntry->m_iFlags & KviIrcUserEntry::HalfOp)
						pStats->uHalfOp++;
					else
					{
						if(pEntry->m_iFlags & KviIrcUserEntry::Voice)
							pStats->uVoiced++;
						else
						{
							if(pEntry->m_iFlags & KviIrcUserEntry::UserOp)
								pStats->uUserOp++;
						}
					}
				}
			}
		}
		pEntry = pEntry->m_pNext;
	}

	if(pStats->uActive > 0)
		pStats->iAvgTemperature /= ((int)pStats->uActive);
}

void KviUserListView::maybeTip(KviUserListToolTip * pTip, const QPoint & pnt)
{
	if(!KVI_OPTION_BOOL(KviOption_boolShowUserListViewToolTips))
		return;
	QRect itRect;
	KviUserListEntry * pEntry = (KviUserListEntry *)itemAt(pnt, &itRect);
	if(pEntry)
	{
		if(m_pKviWindow->console())
		{
			QString szBuffer;
			m_pKviWindow->console()->getUserTipText(pEntry->m_szNick, pEntry->m_pGlobalData, szBuffer);

			if(pEntry->m_joinTime != 0)
			{
				QString szTmp;
				QDateTime date;
				date.setTime_t(pEntry->m_joinTime);

				switch(KVI_OPTION_UINT(KviOption_uintOutputDatetimeFormat))
				{
					case 0:
						// this is the equivalent to an empty date.toString() call, but it's needed
						// to ensure qt4 will use the default() locale and not the system() one
						szTmp = QLocale().toString(date, "ddd MMM d hh:mm:ss yyyy");
						break;
					case 1:
						szTmp = date.toString(Qt::ISODate);
						break;
					case 2:
						szTmp = date.toString(Qt::SystemLocaleShortDate);
						break;
				}

				szBuffer += R"(<tr><td bgcolor="#E0E0E0"><font color="#000000">)";
				szBuffer += __tr2qs("Joined on: <b>%1</b>").arg(szTmp);
				szBuffer += "</font></td></tr>";
			}

			if(pEntry->m_lastActionTime != 0)
			{
				int iSecs = kvi_unixTime() - pEntry->m_lastActionTime;
				int iMins = iSecs / 60;
				iSecs = iSecs % 60;
				int iHours = iMins / 60;
				iMins = iMins % 60;
				szBuffer += R"(<tr><td bgcolor="#E0E0E0"><font color="#000000">)";
				szBuffer += __tr2qs("Quiet for: <b>%1h %2m %3s</b>").arg(iHours).arg(iMins).arg(iSecs);
				szBuffer += "</font></td></tr>";
			}

			if(pEntry->m_pGlobalData->isIrcOp())
			{
				szBuffer += R"(<tr><td bgcolor="#E0E0E0"><font color="#000000">)";
				szBuffer += __tr2qs("%1 is an <b>IrcOp</b>").arg(pEntry->m_szNick);
				szBuffer += "</font></td></tr>";
			}

			pTip->doTip(QRect(pnt, pnt), szBuffer);
		}
	}
}

KviUserListViewArea::KviUserListViewArea(KviUserListView * pPar)
    : QWidget(pPar)
{
	m_pListView = pPar;
	setAutoFillBackground(false);

	m_pScrollBar = new QScrollBar(Qt::Vertical, this);
	m_pScrollBar->setObjectName("scrollbar");
	m_pScrollBar->setRange(0, 0);
	m_pScrollBar->setVisible(false);
	m_pScrollBar->setValue(0);
	connect(m_pScrollBar, SIGNAL(valueChanged(int)), this, SLOT(scrollBarMoved(int)));
	m_pScrollBar->setPageStep(height());
	m_iLastScrollBarVal = 0;
	m_iTopItemOffset = 0;
	m_bIgnoreScrollBar = false;
	m_pLastEntryUnderMouse = nullptr;
}

KviUserListViewArea::~KviUserListViewArea()
    = default;

void KviUserListViewArea::scrollBarMoved(int iNewVal)
{
	if(m_bIgnoreScrollBar)
		return;
	int iDiff = iNewVal - m_iLastScrollBarVal;
	if(m_pListView->m_pTopItem)
	{
		while(iDiff > 0)
		{
			int iNextH = (m_pListView->m_pTopItem->m_iHeight - m_iTopItemOffset);
			if(iDiff >= iNextH)
			{
				// the diff is greater than the top item visible part
				iDiff -= iNextH;
				if(m_pListView->m_pTopItem->m_pNext)
				{
					// There is a next item (offset to 0)
					m_pListView->m_pTopItem = m_pListView->m_pTopItem->m_pNext;
					m_iTopItemOffset = 0;
				}
				else
				{
					// No next item (rather a bug) (offset to the top item size)
					m_iTopItemOffset = m_pListView->m_pTopItem->m_iHeight;
					iDiff = 0;
				}
			}
			else
			{
				// just offset the top item
				m_iTopItemOffset += iDiff;
				iDiff = 0;
			}
		}

		while(iDiff < 0)
		{
			if((-iDiff) <= m_iTopItemOffset)
			{
				// just move the top item
				m_iTopItemOffset += iDiff;
				iDiff = 0;
			}
			else
			{
				iDiff += m_iTopItemOffset;
				if(m_pListView->m_pTopItem->m_pPrev)
				{
					// There is a prev item (offset to 0)
					m_pListView->m_pTopItem = m_pListView->m_pTopItem->m_pPrev;
					m_iTopItemOffset = m_pListView->m_pTopItem->m_iHeight;
				}
				else
				{
					// No prev item (rather a bug) (offset to the top item size)
					m_iTopItemOffset = 0;
					iDiff = 0;
				}
			}
		}
	}
	m_iLastScrollBarVal = iNewVal;
	update();
}

void KviUserListViewArea::paintEvent(QPaintEvent * e)
{
	// update the scroll bar
	int iWidth = width();
	if(m_pScrollBar->isVisible())
		iWidth -= m_pScrollBar->width();

	QRect r = e->rect();
	if(r.right() > iWidth)
		r.setRight(iWidth);

	QPainter p(this);

	QFontMetrics fm(p.fontMetrics());

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency) && g_pApp->supportsCompositing())
	{
		p.save();
		p.setCompositionMode(QPainter::CompositionMode_Source);
		QColor col = KVI_OPTION_COLOR(KviOption_colorGlobalTransparencyFade);
		col.setAlphaF((float)((float)KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) / (float)100));
		p.fillRect(r, col);
		p.restore();
	}
	else if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt = m_pListView->window()->isDocked() ? mapTo(g_pMainWindow, r.topLeft()) : mapTo(m_pListView->window(), r.topLeft());
		p.drawTiledPixmap(r, *(g_pShadedChildGlobalDesktopBackground), pnt);
	}
	else
	{
#endif
		QPixmap * pix = KVI_OPTION_PIXMAP(KviOption_pixmapUserListViewBackground).pixmap();
		if(pix)
		{
			KviPixmapUtils::drawPixmapWithPainter(&p, pix, KVI_OPTION_UINT(KviOption_uintUserListPixmapAlign), r, r.width(), r.height());
		}
		else
		{
			p.fillRect(r, KVI_OPTION_COLOR(KviOption_colorUserListViewBackground));
		}
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	}
#endif

	p.setClipRect(r);

	KviUserListEntry * pEntry = m_pListView->m_pTopItem;

	int iTheY = KVI_USERLIST_BORDER_WIDTH - m_iTopItemOffset;

	kvi_time_t curTime = kvi_unixTime();

	bool bShowIcons = KVI_OPTION_BOOL(KviOption_boolShowUserChannelIcons);
	bool bShowState = KVI_OPTION_BOOL(KviOption_boolShowUserChannelState);
	bool bShowGender = KVI_OPTION_BOOL(KviOption_boolDrawGenderIcons);

	while(pEntry && iTheY <= r.bottom())
	{
		int iBottom = iTheY + pEntry->m_iHeight;
		// iTheY is our top line
		// iTheX is our left corner
		// iBottom is our bottom line
		// iWidth is the width of the whole widget

		if(iBottom >= r.top())
		{
			QColor * pClrFore = nullptr;
			if(pEntry->m_bSelected)
			{
				QColor col(KVI_OPTION_COLOR(KviOption_colorUserListViewSelectionBackground));

				if(style()->inherits("QWindowsXPStyle") || style()->inherits("QWindowsVistaStyle"))
				{
					// The QWindowsXP style does not honor our colors. It uses the system ones instead.
					// We can't accept it.
					p.fillRect(0, iTheY, width(), pEntry->m_iHeight, col);
				}
				else
				{
					QStyleOptionViewItem opt;
					opt.rect = QRect(0, iTheY, width(), pEntry->m_iHeight);
					opt.state = opt.state | QStyle::State_Selected;
					opt.showDecorationSelected = true;
					opt.palette.setColor(QPalette::Highlight, col);
					style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, &p, this);
				}
				pClrFore = &(KVI_OPTION_COLOR(KviOption_colorUserListViewSelectionForeground));
			}
			else if(KVI_OPTION_BOOL(KviOption_boolUseDifferentColorForOwnNick) && m_pListView->m_pKviWindow->connection())
			{
				if(pEntry->m_szNick == m_pListView->m_pKviWindow->connection()->currentNickName())
				{
					pClrFore = &(KVI_OPTION_COLOR(KviOption_colorUserListViewOwnForeground));
				}
			}

			if(!pClrFore)
			{
				// FIXME:
				//
				// this is slow... VERY slow when one has a lot of registered users.
				//   (this is NOT a simple lookup in the user db... it is a mask match)
				// if we REALLY need to use custom colors for regged users then
				// they should be updated ONCE and stored (cached) in the KviUserListEntry structure
				//
				if(m_pListView->m_pKviWindow->connection()->userDataBase()->haveCustomColor(pEntry->m_szNick))
				{
					pClrFore = m_pListView->m_pKviWindow->connection()->userDataBase()->customColor(pEntry->m_szNick);
				}

				if(!pClrFore)
				{
					if(pEntry->m_iFlags == 0 && !pEntry->globalData()->isIrcOp())
					{
						pClrFore = &(KVI_OPTION_COLOR(KviOption_colorUserListViewNormalForeground));
					}
					else
					{
						pClrFore = &(KVI_OPTION_COLOR((pEntry->globalData()->isIrcOp()) ? KviOption_colorUserListViewIrcOpForeground : ((pEntry->m_iFlags & KviIrcUserEntry::ChanOwner) ? KviOption_colorUserListViewChanOwnerForeground : ((pEntry->m_iFlags & KviIrcUserEntry::ChanAdmin) ? KviOption_colorUserListViewChanAdminForeground : ((pEntry->m_iFlags & KviIrcUserEntry::Op) ? KviOption_colorUserListViewOpForeground : ((pEntry->m_iFlags & KviIrcUserEntry::HalfOp) ? KviOption_colorUserListViewHalfOpForeground : ((pEntry->m_iFlags & KviIrcUserEntry::Voice) ? KviOption_colorUserListViewVoiceForeground : KviOption_colorUserListViewUserOpForeground)))))));
					}
				}
			}

			int iTheX = KVI_USERLIST_BORDER_WIDTH + KVI_USERLIST_ICON_MARGIN + 1;

			int iAvatarAndTextX = iTheX;

			if(bShowGender)
				iAvatarAndTextX += KVI_USERLIST_ICON_WIDTH + KVI_USERLIST_ICON_MARGIN;
			if(bShowIcons)
				iAvatarAndTextX += KVI_USERLIST_ICON_WIDTH + KVI_USERLIST_ICON_MARGIN;
			if(bShowState)
				iAvatarAndTextX += KVI_USERLIST_ICON_STATE_WIDTH + KVI_USERLIST_ICON_MARGIN;

			if(KVI_OPTION_BOOL(KviOption_boolUserListViewDrawGrid))
			{
				// the grid
				switch(KVI_OPTION_UINT(KviOption_uintUserListViewGridType))
				{
					case KVI_USERLISTVIEW_GRIDTYPE_PLAINGRID:
					case KVI_USERLISTVIEW_GRIDTYPE_DOTGRID:
						p.setPen(QPen(KVI_OPTION_COLOR(KviOption_colorUserListViewGrid), 0,
						    (KVI_OPTION_UINT(KviOption_uintUserListViewGridType) == KVI_USERLISTVIEW_GRIDTYPE_DOTGRID) ? Qt::DotLine : Qt::SolidLine));
						p.drawLine(0, iBottom - 1, iWidth, iBottom - 1);
						if(bShowState || bShowIcons)
							p.drawLine(iAvatarAndTextX, iBottom - 1, iAvatarAndTextX, iTheY);
						break;
					default: // KVI_USERLISTVIEW_GRIDTYPE_3DGRID and KVI_USERLISTVIEW_GRIDTYPE_3DBUTTONS
						if(!pEntry->m_bSelected)
						{
							p.setPen(QPen(KVI_OPTION_COLOR(KviOption_colorUserListViewGrid), 0 /*,QPen::DotLine*/));
							if((bShowState || bShowIcons) && (KVI_OPTION_UINT(KviOption_uintUserListViewGridType) == KVI_USERLISTVIEW_GRIDTYPE_3DGRID))
								p.drawLine(iAvatarAndTextX, iBottom - 1, iAvatarAndTextX, iTheY);
							p.setPen(palette().shadow().color());
							p.drawLine(0, iBottom - 1, iWidth, iBottom - 1);
							p.setPen(palette().light().color());
							p.drawLine(0, iTheY, iWidth, iTheY);
							iTheY--;
						}
						iTheY++;
						break;
				}
				iAvatarAndTextX += 3;
			}

			if(pEntry->globalData()->isAway() && KVI_OPTION_BOOL(KviOption_boolUserListViewUseAwayColor))
			{
				QRgb rgb2 = pClrFore->rgb();
				QRgb rgb1 = KVI_OPTION_COLOR(KviOption_colorUserListViewAwayForeground).rgb();
				p.setPen(QColor(
				    ((qRed(rgb1) * 2) + qRed(rgb2)) / 3,
				    ((qGreen(rgb1) * 2) + qGreen(rgb2)) / 3,
				    ((qBlue(rgb1) * 2) + qBlue(rgb2)) / 3));
			}
			else
			{
				p.setPen(*pClrFore);
			}
			iTheY += 2;

			if(KVI_OPTION_BOOL(KviOption_boolShowAvatarsInUserlist))
			{
				if(pEntry->m_pAvatarPixmap)
				{
					QPixmap * pPix = pEntry->m_pAvatarPixmap->pixmap();
					p.drawPixmap(iAvatarAndTextX, iTheY, *pPix);
					iTheY += pPix->height() + 1;
				}
			}

			if(bShowGender)
			{
				if(pEntry->globalData()->isIrcOp())
				{
					QPixmap * pIco = g_pIconManager->getSmallIcon(KviIconManager::AlienIrcOp);
					p.drawPixmap(iTheX, iTheY + (m_pListView->m_iFontHeight - 16) / 2, *pIco);
				}
				else if(pEntry->globalData()->gender() != KviIrcUserEntry::Unknown)
				{
					QPixmap * pIco = g_pIconManager->getSmallIcon((pEntry->globalData()->gender() == KviIrcUserEntry::Male) ? KviIconManager::SexMale : KviIconManager::SexFemale);
					p.drawPixmap(iTheX, iTheY + (m_pListView->m_iFontHeight - 16) / 2, *pIco);
				}
				else if(pEntry->globalData()->isBot())
				{
					QPixmap * pIco = g_pIconManager->getSmallIcon(KviIconManager::IrcBot);
					p.drawPixmap(iTheX, iTheY + (m_pListView->m_iFontHeight - 16) / 2, *pIco);
				}
				iTheX += KVI_USERLIST_ICON_WIDTH + KVI_USERLIST_ICON_MARGIN;
			}

			if(bShowState)
			{
				if(pEntry->m_lastActionTime)
				{
					// the g_pUserChanStatePixmap is 36 x 80 pixels
					// divided into 6 rows of 5 pixmaps
					// row 0 is hot, row 5 is cold
					// left is most active, right is least active
					// e->m_iTemperature is a signed short, negative values are cold
					// e->m_lastActionTime is the time of the last action (eventually 0, if not known)
					// 6 bit right shift is an aprox division for 64 : really aprox minutes
					unsigned int uTimeDiff = (((unsigned int)(curTime - pEntry->m_lastActionTime)) >> 6);
					if(uTimeDiff < 16)
					{
						//p.drawRect(iTheX,iTheY + 2,10,e->m_iHeight - 4);
						static int iXOffTable[16] = {
							0, 8, 16, 16,
							24, 24, 24, 24,
							32, 32, 32, 32,
							32, 32, 32, 32
						};
						// the temperature now
						// temp > 100 is hot (offset y = 0)
						// temp < -100 is cold (offset y = 80)
						// temp > 30 is half-hot (offset y = 16)
						// temp < -30 is half-cold (offset y = 64)
						// temp > 0 is a-bit-hot (offset y = 32)
						// temp < 0 is a-bit-cold (offset y = 48)

						if(pEntry->m_iTemperature > KVI_MID_TEMPERATURE)
						{
							if(pEntry->m_iTemperature > KVI_HALF_HOT_TEMPERATURE)
							{
								if(pEntry->m_iTemperature > KVI_HOT_TEMPERATURE)
								{
									// hot
									p.drawPixmap(iTheX, iTheY, *g_pUserChanStatePixmap, iXOffTable[uTimeDiff], 0, 8, 16);
								}
								else
								{
									// half-hot
									p.drawPixmap(iTheX, iTheY, *g_pUserChanStatePixmap, iXOffTable[uTimeDiff], 16, 8, 16);
								}
							}
							else
							{
								// bit-hot
								p.drawPixmap(iTheX, iTheY, *g_pUserChanStatePixmap, iXOffTable[uTimeDiff], 32, 8, 16);
							}
						}
						else
						{
							if(pEntry->m_iTemperature < KVI_HALF_COLD_TEMPERATURE)
							{
								if(pEntry->m_iTemperature < KVI_COLD_TEMPERATURE)
								{
									// cold
									p.drawPixmap(iTheX, iTheY, *g_pUserChanStatePixmap, iXOffTable[uTimeDiff], 80, 8, 16);
								}
								else
								{
									// half-cold
									p.drawPixmap(iTheX, iTheY, *g_pUserChanStatePixmap, iXOffTable[uTimeDiff], 64, 8, 16);
								}
							}
							else
							{
								// bit-cold
								p.drawPixmap(iTheX, iTheY, *g_pUserChanStatePixmap, iXOffTable[uTimeDiff], 48, 8, 16);
							}
						}
					}
				}
				iTheX += KVI_USERLIST_ICON_STATE_WIDTH + KVI_USERLIST_ICON_MARGIN;
			}

			if(bShowIcons)
			{
				//p.drawRect(iTheX,iTheY + 2,18,e->m_iHeight - 4);
				if(pEntry->m_iFlags != 0)
				{
					QPixmap * pIco = g_pIconManager->getSmallIcon(
					    pEntry->globalData()->isAway() ? (
					                                         (pEntry->m_iFlags & KviIrcUserEntry::ChanOwner) ? KviIconManager::ChanOwnerAway : ((pEntry->m_iFlags & KviIrcUserEntry::ChanAdmin) ? KviIconManager::ChanAdminAway : ((pEntry->m_iFlags & KviIrcUserEntry::Op) ? KviIconManager::OpAway : ((pEntry->m_iFlags & KviIrcUserEntry::HalfOp) ? KviIconManager::HalfOpAway : ((pEntry->m_iFlags & KviIrcUserEntry::Voice) ? KviIconManager::VoiceAway : KviIconManager::UserOpAway)))))
					                                   : (
					                                         (pEntry->m_iFlags & KviIrcUserEntry::ChanOwner) ? KviIconManager::ChanOwner : ((pEntry->m_iFlags & KviIrcUserEntry::ChanAdmin) ? KviIconManager::ChanAdmin : ((pEntry->m_iFlags & KviIrcUserEntry::Op) ? KviIconManager::Op : ((pEntry->m_iFlags & KviIrcUserEntry::HalfOp) ? KviIconManager::HalfOp : ((pEntry->m_iFlags & KviIrcUserEntry::Voice) ? KviIconManager::Voice : KviIconManager::UserOp))))));
					p.drawPixmap(iTheX, iTheY + (fm.lineSpacing() - 16 /*size of small icon*/) / 2, *pIco);
				}
				iTheX += KVI_USERLIST_ICON_WIDTH + KVI_USERLIST_ICON_MARGIN;
				p.drawText(iAvatarAndTextX, iTheY + fm.lineSpacing() - 1, pEntry->m_szNick);
			}
			else
			{

				char cFlag = m_pListView->getUserFlag(pEntry);
				if(cFlag)
				{
					QString szTmp = QChar(cFlag);
					szTmp += pEntry->m_szNick;
					p.drawText(iAvatarAndTextX, iTheY + fm.lineSpacing() - 1, szTmp);
				}
				else
				{
					p.drawText(iAvatarAndTextX, iTheY + fm.lineSpacing() - 1, pEntry->m_szNick);
				}
			}
		}

		iTheY = iBottom;
		pEntry = pEntry->m_pNext;
	}

	p.setPen(palette().dark().color());
	p.drawLine(0, 0, width(), 0);
	p.drawLine(0, 0, 0, height());
	p.setPen(palette().light().color());
	p.drawLine(1, height() - 1, width() - 1, height() - 1);
	p.drawLine(width() - 1, 1, width() - 1, height());
}

void KviUserListViewArea::resizeEvent(QResizeEvent *)
{
	int iScr = m_pScrollBar->sizeHint().width();
	m_pScrollBar->setGeometry(width() - iScr, 0, iScr, height());
	m_pScrollBar->setPageStep(height());
	m_pScrollBar->setSingleStep(m_pListView->m_iFontHeight - 1);
}

void KviUserListViewArea::mousePressEvent(QMouseEvent * e)
{
	setFocus();
	if(e->button() & Qt::LeftButton)
	{
		KviUserListEntry * pEntry = m_pListView->itemAt(e->pos());
		if(pEntry)
		{
			if(e->modifiers() & Qt::ShiftModifier)
			{
				// Multiselect mode
				if(!pEntry->m_bSelected)
					m_pListView->m_iSelectedCount++;
				pEntry->m_bSelected = true;

				if(m_pListView->m_iSelectedCount == 1)
					g_pMainWindow->childWindowSelectionStateChange(m_pListView->m_pKviWindow, true);
				update();
			}
			else if(e->modifiers() & Qt::ControlModifier)
			{
				// Invert mode
				if(!pEntry->m_bSelected)
					m_pListView->m_iSelectedCount++;
				else
					m_pListView->m_iSelectedCount--;

				pEntry->m_bSelected = !pEntry->m_bSelected;
				if(m_pListView->m_iSelectedCount == 0)
					g_pMainWindow->childWindowSelectionStateChange(m_pListView->m_pKviWindow, false);
				else if(m_pListView->m_iSelectedCount == 1)
					g_pMainWindow->childWindowSelectionStateChange(m_pListView->m_pKviWindow, true);
				update();
			}
			else
			{
				// Single select mode
				bool bThereWasSelection = false;
				if(m_pListView->m_iSelectedCount > 0)
				{
					KviUserListEntry * pAux = m_pListView->m_pHeadItem;
					while(pAux)
					{
						pAux->m_bSelected = false;
						pAux = pAux->m_pNext;
					}
					bThereWasSelection = true;
				}
				pEntry->m_bSelected = true;
				m_pListView->m_iSelectedCount = 1;

				if(!bThereWasSelection)
					g_pMainWindow->childWindowSelectionStateChange(m_pListView->m_pKviWindow, true);
				update();
			}
		}
		m_pLastEntryUnderMouse = pEntry;
	}
	else if(e->button() & Qt::RightButton)
	{
		KviUserListEntry * pEntry = m_pListView->itemAt(e->pos());
		if(pEntry)
		{
			if(!pEntry->m_bSelected)
			{
				pEntry->m_bSelected = true;
				m_pListView->m_iSelectedCount = 1;

				KviPointerHashTableIterator<QString, KviUserListEntry> it(*(m_pListView->m_pEntryDict));
				while(it.current())
				{
					if(it.current() != pEntry)
						((KviUserListEntry *)it.current())->m_bSelected = false;
					++it;
				}
			}
			if(m_pListView->m_iSelectedCount == 1)
				g_pMainWindow->childWindowSelectionStateChange(m_pListView->m_pKviWindow, true);
			update();
		}
		m_pListView->emitRightClick();
	}
}

void KviUserListViewArea::keyPressEvent(QKeyEvent * e)
{
	if(e->key() == Qt::Key_Escape)
	{
		if(m_pListView->m_pKviWindow->input())
			((QWidget *)(m_pListView->m_pKviWindow->input()))->setFocus();
	}
	else if(e->key() == Qt::Key_Up)
	{
		if(m_pListView->m_iSelectedCount == 0)
		{
			KviUserListEntry * pNick = m_pListView->m_pHeadItem;
			if(pNick)
			{
				pNick->m_bSelected = true;
				m_pListView->ensureVisible(pNick->nick());
			}
		}
		else
		{
			KviUserListEntry * pAux = m_pListView->m_pHeadItem;
			KviUserListEntry * pNick = nullptr;
			while(pAux)
			{
				if(pAux->m_bSelected)
				{
					if(e->modifiers() & Qt::ShiftModifier)
					{
						if(pAux != m_pListView->m_pHeadItem)
						{
							m_pListView->m_iSelectedCount++;
							pNick = pAux->m_pPrev;
						}
						break;
					}
					else
					{
						pAux->m_bSelected = false;

						if(!pNick)
						{
							if(pAux == m_pListView->m_pHeadItem)
								pNick = m_pListView->m_pTailItem;
							else
								pNick = pAux->m_pPrev;
						}
					}
				}
				pAux = pAux->m_pNext;
			}

			if(pNick)
			{
				pNick->m_bSelected = true;
				m_pListView->ensureVisible(pNick->nick());
			}
		}
		g_pMainWindow->childWindowSelectionStateChange(m_pListView->m_pKviWindow, true);
		update();
	}
	else if(e->key() == Qt::Key_Down)
	{
		if(m_pListView->m_iSelectedCount == 0)
		{
			KviUserListEntry * pNick = m_pListView->m_pHeadItem;
			if(pNick)
			{
				pNick->m_bSelected = true;
				m_pListView->ensureVisible(pNick->nick());
			}
		}
		else
		{
			KviUserListEntry * pAux = m_pListView->m_pTailItem;
			KviUserListEntry * pNick = nullptr;
			while(pAux)
			{
				if(pAux->m_bSelected)
				{
					if(e->modifiers() & Qt::ShiftModifier)
					{
						if(pAux != m_pListView->m_pTailItem)
						{
							m_pListView->m_iSelectedCount++;
							pNick = pAux->m_pNext;
						}
						break;
					}
					else
					{
						pAux->m_bSelected = false;

						if(!pNick)
						{
							if(pAux == m_pListView->m_pTailItem)
								pNick = m_pListView->m_pHeadItem;
							else
								pNick = pAux->m_pNext;
						}
					}
				}
				pAux = pAux->m_pPrev;
			}

			if(pNick)
			{
				pNick->m_bSelected = true;
				m_pListView->ensureVisible(pNick->nick());
			}
		}
		g_pMainWindow->childWindowSelectionStateChange(m_pListView->m_pKviWindow, true);
		update();
	}
	else if(e->key() == Qt::Key_PageUp)
	{
		if(m_pListView->m_iSelectedCount == 0)
		{
			KviUserListEntry * pNick = m_pListView->m_pHeadItem;
			if(pNick)
			{
				pNick->m_bSelected = true;
				m_pListView->ensureVisible(pNick->nick());
			}
		}
		else
		{
			KviUserListEntry * pAux = m_pListView->m_pHeadItem;
			KviUserListEntry * pNick = nullptr;
			while(pAux)
			{
				if(pAux->m_bSelected)
				{
					if(e->modifiers() & Qt::ShiftModifier)
					{
						int i = 10;
						pNick = pAux;
						while(pNick != m_pListView->m_pHeadItem && i > 0)
						{
							m_pListView->m_iSelectedCount++;
							pNick->m_bSelected = true;
							pNick = pNick->m_pPrev;
							i--;
						}
						if(pNick)
							pNick->m_bSelected = true;
						break;
					}
					else
					{
						pAux->m_bSelected = false;

						if(!pNick)
						{
							pNick = pAux;
							for(int i = 10; i > 0; i--)
							{
								if(pNick == m_pListView->m_pHeadItem)
								{
									pNick = m_pListView->m_pTailItem;
									i = 0;
								}
								else
								{
									pNick = pNick->m_pPrev;
								}
							}
						}
					}
					if(pNick)
						pNick->m_bSelected = true;
				}
				pAux = pAux->m_pNext;
			}

			if(pNick)
				m_pListView->ensureVisible(pNick->nick());
		}
		g_pMainWindow->childWindowSelectionStateChange(m_pListView->m_pKviWindow, true);
		update();
	}
	else if(e->key() == Qt::Key_PageDown)
	{
		if(m_pListView->m_iSelectedCount == 0)
		{
			KviUserListEntry * pNick = m_pListView->m_pHeadItem;
			if(pNick)
			{
				pNick->m_bSelected = true;
				m_pListView->ensureVisible(pNick->nick());
			}
		}
		else
		{
			KviUserListEntry * pAux = m_pListView->m_pTailItem;
			KviUserListEntry * pNick = nullptr;
			while(pAux)
			{
				if(pAux->m_bSelected)
				{
					if(e->modifiers() & Qt::ShiftModifier)
					{
						int i = 10;
						pNick = pAux;
						while(pNick != m_pListView->m_pTailItem && i > 0)
						{
							m_pListView->m_iSelectedCount++;
							pNick->m_bSelected = true;
							pNick = pNick->m_pNext;
							i--;
						}
						if(pNick)
							pNick->m_bSelected = true;
						break;
					}
					else
					{
						pAux->m_bSelected = false;

						if(!pNick)
						{
							pNick = pAux;
							for(int i = 10; i > 0; i--)
							{
								if(pNick == m_pListView->m_pTailItem)
								{
									pNick = m_pListView->m_pHeadItem;
									i = 0;
								}
								else
								{
									pNick = pNick->m_pNext;
								}
							}
						}
					}
					if(pNick)
						pNick->m_bSelected = true;
				}
				pAux = pAux->m_pPrev;
			}

			if(pNick)
				m_pListView->ensureVisible(pNick->nick());
		}
		g_pMainWindow->childWindowSelectionStateChange(m_pListView->m_pKviWindow, true);
		update();
	}
	else if(e->matches(QKeySequence::SelectAll))
	{
		KviUserListEntry * pAux = m_pListView->m_pHeadItem;
		while(pAux)
		{
			pAux->m_bSelected = true;
			pAux = pAux->m_pNext;
		}

		g_pMainWindow->childWindowSelectionStateChange(m_pListView->m_pKviWindow, true);
		update();
	}
	else
	{
		QString szKey = e->text();
		if(!szKey.isEmpty())
		{
			KviUserListEntry * pNick = nullptr;
			KviUserListEntry * pAux = m_pListView->m_pHeadItem;

			if(m_pListView->selectedCount() == 1)
			{
				bool bFoundSelected = false;
				while(pAux)
				{
					if(pAux->nick().indexOf(szKey, 0, Qt::CaseInsensitive) == 0)
					{
						//match
						if(bFoundSelected)
							break;
						if(pAux->m_bSelected)
							bFoundSelected = true;
					}
					pAux = pAux->m_pNext;
				}
				if(!pAux)
					pAux = m_pListView->m_pHeadItem;
			}

			while(pAux)
			{
				//qDebug("%s %s %i %s %i",__FILE__,__FUNCTION__,__LINE__,aux->nick().toUtf8().data(),aux->nick().find(szKey,0,0));
				if(pAux->nick().indexOf(szKey, 0, Qt::CaseInsensitive) == 0)
				{
					pNick = pAux;
					break;
				}
				pAux = pAux->m_pNext;
			}

			if(pNick)
			{
				m_pListView->select(pNick->nick());
				m_pListView->ensureVisible(pNick->nick());
			}
		}
	}
}

void KviUserListViewArea::mouseDoubleClickEvent(QMouseEvent *)
{
	m_pListView->emitDoubleClick();
}

void KviUserListViewArea::mouseMoveEvent(QMouseEvent * e)
{
	if(!(e->buttons() & Qt::LeftButton))
		return;

	KviUserListEntry * pEntry = m_pListView->itemAt(e->pos());
	if(!pEntry)
	{
		if(e->pos().y() < KVI_USERLIST_BORDER_WIDTH)
		{
			KviUserListEntry * pTop = m_pListView->m_pTopItem;
			if(pTop)
			{
				pEntry = pTop;
				m_pScrollBar->setValue(m_pScrollBar->value() - pTop->m_iHeight);
				if(m_pListView->m_pTopItem != pTop)
				{
					pEntry = m_pListView->m_pTopItem;
				}
			}
		}
		else if(e->pos().y() > (height() - KVI_USERLIST_BORDER_WIDTH))
		{
			KviUserListEntry * pBottom = m_pListView->m_pTopItem;
			if(pBottom)
			{
				int iTheY = KVI_USERLIST_BORDER_WIDTH - m_iTopItemOffset;
				while(pBottom && (iTheY < height()))
				{
					iTheY += pBottom->m_iHeight;
					pBottom = pBottom->m_pNext;
				}

				if(!pBottom)
					pBottom = m_pListView->m_pTailItem;
				if(pBottom)
				{
					pEntry = pBottom;
					m_pScrollBar->setValue(m_pScrollBar->value() + pBottom->m_iHeight);
				}
			}
		}
	}

	if(!pEntry || !m_pLastEntryUnderMouse || pEntry == m_pLastEntryUnderMouse)
		return;

	if(e->modifiers() & Qt::ControlModifier)
	{
		if(pEntry->m_bSelected)
			m_pListView->m_iSelectedCount--;
		else
			m_pListView->m_iSelectedCount++;
		pEntry->m_bSelected = !pEntry->m_bSelected;
		if(m_pListView->m_iSelectedCount == 0)
			g_pMainWindow->childWindowSelectionStateChange(m_pListView->m_pKviWindow, false);
		else if(m_pListView->m_iSelectedCount == 1)
			g_pMainWindow->childWindowSelectionStateChange(m_pListView->m_pKviWindow, true);

		m_pLastEntryUnderMouse = pEntry;
	}
	else
	{
		KviUserListEntry * pCurEntry = m_pListView->m_pHeadItem;
		for(uint uFound = 0; pCurEntry && uFound < 2; pCurEntry = pCurEntry->next())
		{
			if(pCurEntry == pEntry || pCurEntry == m_pLastEntryUnderMouse)
				uFound++;
			if(uFound)
			{
				if(!pCurEntry->m_bSelected)
				{
					m_pListView->m_iSelectedCount++;
					pCurEntry->m_bSelected = true;
					if(m_pListView->m_iSelectedCount == 1)
						g_pMainWindow->childWindowSelectionStateChange(m_pListView->m_pKviWindow, true);
				}
			}
		}
	}
	update();
}

void KviUserListViewArea::mouseReleaseEvent(QMouseEvent *)
{
	m_pLastEntryUnderMouse = nullptr;
}

void KviUserListViewArea::wheelEvent(QWheelEvent * e)
{
	static bool bHere = false; // Qt4(<= 4.2.2) has a nasty bug that makes the re-sent wheelEvent to cause infinite recursion
	if(bHere)
		return;
	bHere = true;
	g_pApp->sendEvent(m_pScrollBar, e);
	bHere = false;
}
