//=============================================================================
//
//   File : kvi_userlistview.cpp
//   Creation date : Tue Aug 1 2000 21:05:22 by Szymon Stefanek
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

#define __KVIRC__
#define _KVI_USERLISTVIEW_CPP_

#include "kvi_debug.h"
#include "kvi_userlistview.h"
#include "kvi_settings.h"
#include "kvi_locale.h"
#include "kvi_options.h"
#include "kvi_defaults.h"
#include "kvi_iconmanager.h"
#include "kvi_regusersdb.h"
#include "kvi_parameterlist.h"
#include "kvi_window.h"
#include "kvi_console.h"
#include "kvi_app.h"
#include "kvi_useraction.h"
#include "kvi_qstring.h"
#include "kvi_frame.h"
#include "kvi_mdimanager.h"
#include "kvi_kvs_eventtriggers.h"
#include "kvi_toolwindows_container.h"
#include "kvi_doublebuffer.h"
#include "kvi_stringconversion.h"
#include "kvi_ircconnection.h"
#include "kvi_ircconnectionserverinfo.h"

#include <qlabel.h>
#include <qscrollbar.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qfontmetrics.h>
#include <qdatetime.h>
#include <qmime.h>

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	extern QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif

// kvi_app.cpp
extern KviRegisteredUserDataBase * g_pRegisteredUserDataBase;

// kvi_app.cpp (loaded and destroyed by KviIconManager)
extern QPixmap * g_pUserChanStatePixmap;

// Yet another really complex widget :)

#define KVI_USERLIST_BORDER_WIDTH 2

// FIXME: #warning "Button to show/hide avatars"
// FIXME: #warning "We want to be able to navigate the list with the keyboard!"

KviUserListToolTip::KviUserListToolTip(KviUserListView * v,KviUserListViewArea * a)
: KviTalToolTip(a)
{
	m_pListView = v;
}

KviUserListToolTip::~KviUserListToolTip()
{
}

void KviUserListToolTip::maybeTip(const QPoint &pnt)
{
	m_pListView->maybeTip(this,pnt);
}




KviUserListEntry::KviUserListEntry(KviUserListView * parent,const QString &nick,
	KviIrcUserEntry * e,short int iFlags,bool bJoinTimeUnknown)
{
	m_pListView      = parent;
	m_szNick         = nick;
	m_pGlobalData    = e;
	m_iFlags         = iFlags;
	m_lastActionTime = (kvi_time_t)0;
	m_joinTime       = bJoinTimeUnknown ? (kvi_time_t)0 : kvi_unixTime();
	m_iTemperature   = bJoinTimeUnknown ? 0 : KVI_USERACTION_JOIN;

	m_bSelected      = false;
	
	recalcSize();
}

KviUserListEntry::~KviUserListEntry()
{
}

bool KviUserListEntry::color(QColor& color)
{
	//
	// FIXME: Unused ?
	//
	KviRegisteredUser* pRegisteredUser=g_pRegisteredUserDataBase->findMatchingUser(m_szNick,m_pGlobalData->user(),m_pGlobalData->host());
	if(pRegisteredUser)
	{
		if(pRegisteredUser->getBoolProperty("useCustomColor"))
		{
			QString szTmp=pRegisteredUser->getProperty("customColor");
			KviStringConversion::fromString(szTmp,color);
			return true;
		}
	}
	if(KVI_OPTION_BOOL(KviOption_boolUseDifferentColorForOwnNick) && m_pListView->m_pKviWindow->connection())
	{
		if(m_szNick==m_pListView->m_pKviWindow->connection()->currentNickName())
		{
			color = KVI_OPTION_COLOR(KviOption_colorUserListViewOwnForeground);
			return true;
		}
	} 
	if(m_iFlags == 0)
	{
		color=KVI_OPTION_COLOR(KviOption_colorUserListViewNormalForeground);
		return true;
	} else {
		color = KVI_OPTION_COLOR((m_iFlags & KVI_USERFLAG_CHANOWNER) ? \
			KviOption_colorUserListViewChanOwnerForeground : ((m_iFlags & KVI_USERFLAG_CHANADMIN) ? \
			KviOption_colorUserListViewChanAdminForeground : ((m_iFlags & KVI_USERFLAG_OP) ? \
			KviOption_colorUserListViewOpForeground : ((m_iFlags & KVI_USERFLAG_HALFOP) ? \
			KviOption_colorUserListViewHalfOpForeground : ((m_iFlags & KVI_USERFLAG_VOICE) ? \
			KviOption_colorUserListViewVoiceForeground : KviOption_colorUserListViewUserOpForeground)))));
			return true;
	}
	return true;
}

void KviUserListEntry::recalcSize()
{
	KviAvatar * av = m_pGlobalData->avatar();
	m_iHeight = m_pListView->m_iFontHeight;
	if(KVI_OPTION_BOOL(KviOption_boolShowUserChannelIcons) && (m_iHeight < 22))m_iHeight = 22;
	
	if(!KVI_OPTION_BOOL(KviOption_boolDisableAvatars))//G&N  2005
	{
		if(av)
		{
			if( KVI_OPTION_BOOL(KviOption_boolScaleAvatars) &&
					(
						!KVI_OPTION_BOOL(KviOption_boolDoNotStretchAvatars) ||
						(av->pixmap()->width() > KVI_OPTION_UINT(KviOption_uintAvatarScaleWidth)) ||
						(av->pixmap()->height() > KVI_OPTION_UINT(KviOption_uintAvatarScaleHeight))
					)
				)
			{
				QPixmap * pix = av->scaledPixmap(KVI_OPTION_UINT(KviOption_uintAvatarScaleWidth), KVI_OPTION_UINT(KviOption_uintAvatarScaleHeight));
			
				m_iHeight += pix->height();
			}
			else
			{
				m_iHeight += av->pixmap()->height();
			}
		}
	}
	m_iHeight += 3;
}

///////////////////////////////////////////////



KviUserListView::KviUserListView(QWidget * parent,KviWindowToolPageButton* button,KviIrcUserDataBase * db,KviWindow * pWnd,int dictSize,const QString & label_text,const char * name)
: KviWindowToolWidget(parent,button)
{
	setAutoDelete(0);
	m_pKviWindow  = pWnd;
	m_pEntryDict  = new KviDict<KviUserListEntry>(dictSize,false);
	m_pEntryDict->setAutoDelete(true);

	m_pUsersLabel = new QLabel(this,"userslabel");
	KviTalToolTip::add(m_pUsersLabel,label_text);

	m_pViewArea   = new KviUserListViewArea(this);
	m_pToolTip    = new KviUserListToolTip(this,m_pViewArea);
	m_pTopItem    = 0;
	m_pHeadItem   = 0;
	m_pTailItem   = 0;
	m_iOpCount    = 0;
	m_iHalfOpCount = 0;
	m_iVoiceCount = 0;
	m_iChanAdminCount = 0;
	m_iChanOwnerCount = 0;
	m_iUserOpCount = 0;
	m_pIrcUserDataBase = db;
	m_iTotalHeight = 0;
	m_ibEntries   = 0;
	m_ieEntries   = 0;
	m_iIEntries   = 0;
	m_iSelectedCount = 0;
//	setBackgroundMode(QWidget::NoBackground);
	applyOptions();
	registerSelf();
}

KviUserListView::~KviUserListView()
{
	removeAllEntries();
	delete m_pEntryDict;
	delete m_pToolTip;
}

void KviUserListView::emitRightClick()
{
	int ev = -1;
	switch(m_pKviWindow->type())
	{
		case KVI_WINDOW_TYPE_CHANNEL: ev = KviEvent_OnChannelNickPopupRequest;  break;
		case KVI_WINDOW_TYPE_QUERY:   ev = KviEvent_OnQueryNickPopupRequest;    break;
		case KVI_WINDOW_TYPE_CONSOLE: ev = KviEvent_OnNotifyListPopupRequest;   break;
		default:                      ev = KviEvent_OnNickLinkPopupRequest;     break; // this should actually never happen
	}
	if(ev > -1)
	{
		if(KviKvsEventManager::instance()->hasAppHandlers(ev))
		{
			QString nicks;
			appendSelectedNicknames(nicks);
			KviKvsVariantList vList;
			vList.append(nicks);
			KviKvsEventManager::instance()->trigger(ev,m_pKviWindow,&vList);
		} else {
			g_pApp->checkSuggestRestoreDefaultScript();
		}
	}
}

void KviUserListView::emitDoubleClick()
{
	int ev = -1;
	switch(m_pKviWindow->type())
	{
		case KVI_WINDOW_TYPE_CHANNEL: ev = KviEvent_OnChannelNickDefaultActionRequest;  break;
		case KVI_WINDOW_TYPE_QUERY:   ev = KviEvent_OnQueryNickDefaultActionRequest;    break;
		case KVI_WINDOW_TYPE_CONSOLE: ev = KviEvent_OnNotifyListDefaultActionRequest;   break;
		default:                      ev = KviEvent_OnNickLinkDefaultActionRequest;     break; // this should actually never happen
	}
	if(ev > -1)
	{
		if(KviKvsEventManager::instance()->hasAppHandlers(ev))
		{
			QString nicks;
			appendSelectedNicknames(nicks);
			KviKvsVariantList vList;
			vList.append(nicks);
			KviKvsEventManager::instance()->trigger(ev,m_pKviWindow,&vList);
		} else {
			g_pApp->checkSuggestRestoreDefaultScript();
		}
	}
}

void KviUserListView::updateScrollBarRange()
{
	int max = m_iTotalHeight - (m_pViewArea->height() - (KVI_USERLIST_BORDER_WIDTH * 2));
	m_pViewArea->m_pScrollBar->setRange(0,max > 0 ? max : 0);
}

void KviUserListView::applyOptions()
{
/*	if(!KVI_OPTION_BOOL(KviOption_boolDisableUserListLabel)){ 
		m_pUsersLabel->hide();
		m_pViewArea->setGeometry(0,0,width(),height());
	}
	if(KVI_OPTION_BOOL(KviOption_boolDisableUserListLabel)){ 
		m_pUsersLabel->show();
		updateUsersLabel();
	}*/
	setFont(KVI_OPTION_FONT(KviOption_fontUserListView));
	QFontMetrics fm(KVI_OPTION_FONT(KviOption_fontUserListView));
	m_iFontHeight = fm.lineSpacing();
	KviUserListEntry * e = m_pHeadItem;
	m_iTotalHeight = 0;
	while(e)
	{
		e->recalcSize();
		m_iTotalHeight += e->m_iHeight;
		e = e->m_pNext;
	}
	updateScrollBarRange();
	m_pUsersLabel->setFont(KVI_OPTION_FONT(KviOption_fontUserListView));
	setMinimumWidth(100);
	resizeEvent(0); // this will call update() too
	repaint();
}
void KviUserListView::updateArea()
{
	bool bEnable = m_pViewArea->isUpdatesEnabled();
	if(!bEnable) m_pViewArea->setUpdatesEnabled(true);
	triggerUpdate();
	if(!bEnable) m_pViewArea->setUpdatesEnabled(false);
}

void KviUserListView::enableUpdates(bool bEnable)
{
	m_pViewArea->setUpdatesEnabled(bEnable);
	if(bEnable)triggerUpdate();
}

void KviUserListView::setMaskEntries(char type, int num)
{
	switch (type)
	{
		case 'b':
			m_ibEntries = num;
		break;
		case 'e':
			m_ieEntries = num;
		break;
		case 'I':
			m_iIEntries = num;
		break;
	}
	updateUsersLabel();
}

void KviUserListView::completeNickBashLike(const QString &begin,KviPtrList<QString> *l,bool bAppendMask)
{
	KviUserListEntry * entry = m_pHeadItem;

	while(entry)
	{
		if(KviQString::equalCIN(begin,entry->m_szNick,begin.length()))
		{
			if(bAppendMask)
			{
				QString * s = new QString();
				KviQString::sprintf(*s,"%Q!%Q@%Q",&(entry->m_szNick),&(entry->m_pGlobalData->user()),&(entry->m_pGlobalData->host()));
				l->append(s);
			} else
				l->append(new QString(entry->m_szNick));
		}
		entry = entry->m_pNext;
	}
}

bool KviUserListView::completeNickStandard(const QString &begin,const QString &skipAfter,QString &buffer,bool bAppendMask)
{
	KviUserListEntry * entry = m_pHeadItem;

	if(!skipAfter.isEmpty())
	{
		while(entry)
		{
			if(KviQString::equalCI(skipAfter,entry->m_szNick))
			{
				entry = entry->m_pNext;
				break;
			}
			entry = entry->m_pNext;
		}
	}

	// FIXME: completion should skip my own nick or place it as last entry in the chain (?)

	//	if(KviConsole * c = m_pKviWindow->console())
	//	{
	//		if(kvi_strEqualCI(entry->m_szNick.ptr(),c->currentNickName())
	//	}

	// Ok...now the real completion
	while(entry)
	{
		if(entry->m_szNick.length() >= begin.length())
		{
			int result = KviQString::cmpCIN(begin,entry->m_szNick,begin.length());
			if(result == 0)
			{
				// This is ok.
				buffer = entry->m_szNick;
				if(bAppendMask)
				{
					buffer += entry->m_pGlobalData->user();
					buffer += entry->m_pGlobalData->host();
				}
				return true;
			} else if(result < 0)
			{
				// No match...begin is lower than the current entry
				if(entry->m_iFlags == 0)return false;
				else {
					int flags = entry->m_iFlags;
					// skip the current flag
					while(entry)
					{
						if(entry->m_iFlags != flags)break;
						entry = entry->m_pNext;
					}
					continue;
				}
			}
		}
		entry = entry->m_pNext;
	}

	return false;
}


void KviUserListView::insertUserEntry(const QString &nick,KviUserListEntry * e)
{
	// Complex insertion task :)

	m_pEntryDict->insert(nick,e);
	m_iTotalHeight += e->m_iHeight;

	bool bGotTopItem = false;

	int flag = 0;
	if(e->m_iFlags != 0)
	{
		if(e->m_iFlags & KVI_USERFLAG_USEROP)
		{
			flag = KVI_USERFLAG_USEROP;
			m_iUserOpCount++;
		}
		if(e->m_iFlags & KVI_USERFLAG_VOICE)
		{
			flag = KVI_USERFLAG_VOICE;
			m_iVoiceCount++;
		}
		if(e->m_iFlags & KVI_USERFLAG_HALFOP)
		{
			flag = KVI_USERFLAG_HALFOP;
			m_iHalfOpCount++;
		}
		if(e->m_iFlags & KVI_USERFLAG_OP)
		{
			flag = KVI_USERFLAG_OP;
			m_iOpCount++;
		}
		if(e->m_iFlags & KVI_USERFLAG_CHANADMIN)
		{
			flag = KVI_USERFLAG_CHANADMIN;
			m_iChanAdminCount++;
		}
		if(e->m_iFlags & KVI_USERFLAG_CHANOWNER)
		{
			flag = KVI_USERFLAG_CHANOWNER;
			m_iChanOwnerCount++;
		}
	}


	if(m_pHeadItem)
	{
		KviUserListEntry * entry = m_pHeadItem;

		if(!(e->m_iFlags & KVI_USERFLAG_CHANOWNER))
		{
			// the new user is not a channel owner...
			// skip the channel owners
			while(entry && (entry->m_iFlags & KVI_USERFLAG_CHANOWNER))
			{
				if(entry == m_pTopItem)bGotTopItem = true;
				entry = entry->m_pNext;
			}

			if(!(e->m_iFlags & KVI_USERFLAG_CHANADMIN))
			{
				// the new user is not a channel admin...
				// skip chan admins
				while(entry && (entry->m_iFlags & KVI_USERFLAG_CHANADMIN))
				{
					if(entry == m_pTopItem)bGotTopItem = true;
					entry = entry->m_pNext;
				}
	
				// is operator ?
				if(!(e->m_iFlags & KVI_USERFLAG_OP))
				{
					// the new user is not an op...
					// skip ops
					while(entry && (entry->m_iFlags & KVI_USERFLAG_OP))
					{
						if(entry == m_pTopItem)bGotTopItem = true;
						entry = entry->m_pNext;
					}
	
					// is half oped ?
					if(!(e->m_iFlags & KVI_USERFLAG_HALFOP))
					{
						// nope , skip halfops
						while(entry && (entry->m_iFlags & KVI_USERFLAG_HALFOP))
						{
							if(entry == m_pTopItem)bGotTopItem = true;
							entry = entry->m_pNext;
						}
	
						// is voiced ?
						if(!(e->m_iFlags & KVI_USERFLAG_VOICE))
						{
							// nope , not voiced so skip voiced users
							while(entry && (entry->m_iFlags & KVI_USERFLAG_VOICE))
							{
								if(entry == m_pTopItem)bGotTopItem = true;
								entry = entry->m_pNext;
							}
	
							// is userop'd?
							if(!(e->m_iFlags & KVI_USERFLAG_USEROP))
							{
								// nope , skip userops
								while(entry && (entry->m_iFlags & KVI_USERFLAG_USEROP))
								{
									if(entry == m_pTopItem)bGotTopItem = true;
									entry = entry->m_pNext;
								}
							} // else is userop, ops, halfops, and voiced are skipped
						} // else it is voiced , ops and halfops are skipped
					} // else it is halfop ,  ops are skipped
				} // else it is op , chan admins are skipped
			} // else it is chan admin , chan owners are skipped
		} // else it is chan owner, so nothing to skip: the chan owners are first in the list
		
		// now strcmp within the current user-flag group...
		while(entry && (KviQString::cmpCI(entry->m_szNick,e->m_szNick) < 0) &&
				((entry->m_iFlags & flag) || (flag == 0)))
		{
			if(entry == m_pTopItem)bGotTopItem = true;
			entry = entry->m_pNext;
		}
		if(entry)
		{
			// inserting
			e->m_pNext = entry;
			e->m_pPrev = entry->m_pPrev;
			if(e->m_pPrev == 0)m_pHeadItem = e;
			else e->m_pPrev->m_pNext = e;
			entry->m_pPrev = e;
			// need to adjust the item offsets now...
			// ok... if we're inserting something after
			// the top item, we move everything down
			// otherwise we only update the scrollbar values
			if(!bGotTopItem)
			{
				// Inserting BEFORE the top item
				if((e == m_pHeadItem) && (m_pTopItem == e->m_pNext) && (m_pViewArea->m_iTopItemOffset == 0))
				{
					// special case...the top item is the head one
					// and it has zero offset...change the top item too
					m_pTopItem = e;
					triggerUpdate();
				} else {
					// invisible insertion
					m_pViewArea->m_bIgnoreScrollBar = true;
					m_pViewArea->m_iLastScrollBarVal += e->m_iHeight;
					updateScrollBarRange();
					m_pViewArea->m_pScrollBar->setValue(m_pViewArea->m_iLastScrollBarVal);
					m_pViewArea->m_bIgnoreScrollBar = false;
					updateUsersLabel();
				}
			} else {
				triggerUpdate();
			}
		} else {
			// appending to the end (may be visible)
			m_pTailItem->m_pNext = e;
			e->m_pNext = 0;
			e->m_pPrev = m_pTailItem;
			m_pTailItem = e;
			triggerUpdate();
		}
	} else {
		// There were no items (is rather visible)
		m_pHeadItem = e;
		m_pTailItem = e;
		m_pTopItem = e;
		e->m_pNext = 0;
		e->m_pPrev = 0;
		triggerUpdate();
	}
	if(e->m_bSelected)
	{
		m_iSelectedCount++;
		if(m_iSelectedCount == 1)g_pFrame->childWindowSelectionStateChange(m_pKviWindow,true);
	}
}

KviUserListEntry * KviUserListView::join(const QString &nick,const QString &user,
		const QString &host,int iFlags)
{
	// Ok..an user joins the channel
	KviUserListEntry * it = m_pEntryDict->find(nick);
	if(it == 0)
	{
		// add an entry to the global dict
		KviIrcUserEntry * pGlobalData = m_pIrcUserDataBase->insertUser(nick,user,host);
		// calculate the flags and update the counters
		it = new KviUserListEntry(this,nick,pGlobalData,iFlags,(user == QString::null));
		insertUserEntry(nick,it);
	} else {
//		if(!host.isEmpty()) - it can be UHNAMES with host or NAMEX(X) w/o it
//		{
			// Ok...the user was already on...
			// Probably this is a NAMES(X) reply , and the user IS_ME
			// (already joined after the JOIN message)
			if(iFlags != it->m_iFlags)
			{
//// FIXME: #warning "Maybe say to the channel that we're oped : and the op is guessed from the names reply"
				if((iFlags & KVI_USERFLAG_CHANOWNER) != (it->m_iFlags & KVI_USERFLAG_CHANOWNER))setChanOwner(nick,iFlags & KVI_USERFLAG_CHANOWNER);
				if((iFlags & KVI_USERFLAG_CHANADMIN) != (it->m_iFlags & KVI_USERFLAG_CHANADMIN))setChanAdmin(nick,iFlags & KVI_USERFLAG_CHANADMIN);
				if((iFlags & KVI_USERFLAG_OP) != (it->m_iFlags & KVI_USERFLAG_OP))op(nick,iFlags & KVI_USERFLAG_OP);
				if((iFlags & KVI_USERFLAG_HALFOP) != (it->m_iFlags & KVI_USERFLAG_HALFOP))halfop(nick,iFlags & KVI_USERFLAG_HALFOP);
				if((iFlags & KVI_USERFLAG_VOICE) != (it->m_iFlags & KVI_USERFLAG_VOICE))voice(nick,iFlags & KVI_USERFLAG_VOICE);
				if((iFlags & KVI_USERFLAG_USEROP) != (it->m_iFlags & KVI_USERFLAG_USEROP))userop(nick,iFlags & KVI_USERFLAG_USEROP);
			}
//		}
	}
	return it;
}

void KviUserListView::triggerUpdate()
{
	// This stuff is useful on joins only
	if(m_pViewArea->isUpdatesEnabled())
	{
		//m_pViewArea->m_pScrollBar->setRange(0,m_iTotalHeight);
		updateScrollBarRange();
		m_pViewArea->update();
		updateUsersLabel();
	}
}

bool KviUserListView::avatarChanged(const QString &nick)
{
	KviUserListEntry * it = m_pEntryDict->find(nick);
	if(it)
	{
		int oldH = it->m_iHeight;
		m_iTotalHeight -= it->m_iHeight;
		it->recalcSize();
		m_iTotalHeight += it->m_iHeight;
		// if this was "over" the top item , we must adjust the scrollbar value
		// otherwise scroll everything down
		KviUserListEntry * e = m_pHeadItem;
		bool bGotTopItem = false;
		while(e != it)
		{
			if(e == m_pTopItem)
			{
				bGotTopItem = true;
				e = it;
			} else e = e->m_pNext;
		}
		if(!bGotTopItem && (m_pTopItem != it))
		{
			// we're "over" the top item , so over the
			// upper side of the view...adjust the scroll bar value
			int hDiff = it->m_iHeight - oldH;
			m_pViewArea->m_iLastScrollBarVal += hDiff;
			m_pViewArea->m_bIgnoreScrollBar = true;
//			m_pViewArea->m_pScrollBar->setRange(0,m_iTotalHeight);
			updateScrollBarRange();
			m_pViewArea->m_pScrollBar->setValue(m_pViewArea->m_iLastScrollBarVal);
			m_pViewArea->m_bIgnoreScrollBar = false;
		} else {
			// the item may be visible!
			// the scroll bar should take care of the case
			// in that the current value runs out of the allowed
			// range.... it should set the value to a good one
			// and emit the signal
			updateScrollBarRange();
//			m_pViewArea->m_pScrollBar->setRange(0,m_iTotalHeight);
			m_pViewArea->update();
		}
		return true;
	}
	return false;
}


bool KviUserListView::userActionVerifyMask(const QString &nick,const QString &user,const QString &host,int actionTemperature,QString &oldUser,QString &oldHost)
{
	// This is called when an user "acts" in some visible way
	// on the channel, so we can keep track of his channeel
	// idle time.
	// This particular function version of userAction
	// will return false if there was a user or hostname
	// change (unless they were not known at all)
	// This will also update the username and hostname
	// if needed.
	KviUserListEntry * it = m_pEntryDict->find(QString(nick));
	if(it)
	{
		it->m_lastActionTime = kvi_unixTime();
		bool bChanged = false;
		if(!(host.isEmpty() || (KviQString::equalCS(host,"*"))))
		{
			if(!KviQString::equalCI(it->m_pGlobalData->host(),host))
			{
				if(!(it->m_pGlobalData->host().isEmpty() || KviQString::equalCS(it->m_pGlobalData->host(),"*")))
				{
					oldHost = it->m_pGlobalData->host();
					bChanged = true;
				}
				it->m_pGlobalData->setHost(host);
			}
		}
		if(!(user.isEmpty() || (KviQString::equalCS(user,"*"))))
		{
			if(!KviQString::equalCI(it->m_pGlobalData->user(),user))
			{
				if(!(it->m_pGlobalData->user().isEmpty() || KviQString::equalCS(it->m_pGlobalData->user(),"*")))
				{
					oldUser = it->m_pGlobalData->user();
					bChanged = true;
				}
				it->m_pGlobalData->setUser(user);
			} 
		}
		it->m_iTemperature += actionTemperature;
		// Don't allow it to grow too much
		if(it->m_iTemperature > 300)it->m_iTemperature = 300;
		else if(it->m_iTemperature < -300)it->m_iTemperature = -300;
		if(itemVisible(it))triggerUpdate();
		return !bChanged;
	}
	return true; // no such nick so no change
}


void KviUserListView::userAction(const QString &nick,const QString &user,const QString &host,int actionTemperature)
{
	// This is called when an user "acts" in some visible way
	// on the channel, so we can keep track of his channeel
	// idle time. This will also update the username and hostname
	// if needed.
	KviUserListEntry * it = m_pEntryDict->find(QString(nick));
	if(it)
	{
		it->m_lastActionTime = kvi_unixTime();
		if(!(host.isEmpty() || (KviQString::equalCS(host,"*"))))
			it->m_pGlobalData->setHost(host);
		if(!(user.isEmpty() || (KviQString::equalCS(user,"*"))))
			it->m_pGlobalData->setUser(user);
		it->m_iTemperature += actionTemperature;
		// Don't allow it to grow too much
		if(it->m_iTemperature > 300)it->m_iTemperature = 300;
		else if(it->m_iTemperature < -300)it->m_iTemperature = -300;
		if(itemVisible(it))triggerUpdate();
	}
}


void KviUserListView::userAction(KviIrcMask *user,int actionTemperature)
{
	// This is called when an user "acts" in some visible way
	// on the channel, so we can keep track of his channeel
	// idle time. This will also update the username and hostname
	// if needed.
	KviUserListEntry * it = m_pEntryDict->find(QString(user->nick()));
	if(it)
	{
		it->m_lastActionTime = kvi_unixTime();
		if(user->hasUser())it->m_pGlobalData->setUser(user->user());
		if(user->hasHost())it->m_pGlobalData->setHost(user->host());
		it->m_iTemperature += actionTemperature;
		// Don't allow it to grow too much
		if(it->m_iTemperature > 300)it->m_iTemperature = 300;
		else if(it->m_iTemperature < -300)it->m_iTemperature = -300;
		if(itemVisible(it))triggerUpdate();
	}
}

void KviUserListView::userAction(const QString &nick,int actionTemperature)
{
	// This is called when an user "acts" in some visible way
	// on the channel, so we can keep track of his channeel
	// idle time. This will also update the username and hostname
	// if needed.
	KviUserListEntry * it = m_pEntryDict->find(nick);
	if(it)
	{
		it->m_lastActionTime = kvi_unixTime();
		it->m_iTemperature += actionTemperature;
		if(it->m_iTemperature > 300)it->m_iTemperature = 300;
		else if(it->m_iTemperature < -300)it->m_iTemperature = -300;
		if(itemVisible(it))triggerUpdate();
	}
}

kvi_time_t KviUserListView::getUserJoinTime(const QString &szNick)
{
	KviUserListEntry * e = m_pEntryDict->find(szNick);
	if(!e)return (kvi_time_t)0;
	return e->m_joinTime;
}

kvi_time_t KviUserListView::getUserLastActionTime(const QString &szNick)
{
	KviUserListEntry * e = m_pEntryDict->find(szNick);
	if(!e)return (kvi_time_t)0;
	return e->m_lastActionTime;
}


int KviUserListView::getUserModeLevel(const QString &szNick)
{
	KviUserListEntry * e = m_pEntryDict->find(szNick);
	if(!e)return 0;
	if(e->m_iFlags & KVI_USERFLAG_MODEMASK)
	{
		if(e->m_iFlags & KVI_USERFLAG_CHANOWNER)return 60;
		if(e->m_iFlags & KVI_USERFLAG_CHANADMIN)return 50;
		if(e->m_iFlags & KVI_USERFLAG_OP)return 40;
		if(e->m_iFlags & KVI_USERFLAG_HALFOP)return 30;
		if(e->m_iFlags & KVI_USERFLAG_VOICE)return 20;
		if(e->m_iFlags & KVI_USERFLAG_USEROP)return 10;
	}
	return 0;
}

char KviUserListView::getUserFlag(KviUserListEntry * e)
{
	if(!e)return 0;
	return m_pKviWindow->connection()->serverInfo()->modePrefixChar(e->m_iFlags);
}

void KviUserListView::prependUserFlag(const QString &nick,QString &buffer)
{
	char uFlag = getUserFlag(nick);
	if(uFlag)buffer.prepend(uFlag);
}

int KviUserListView::flags(const QString &nick)
{
	KviUserListEntry * it = m_pEntryDict->find(nick);
	return it ? it->m_iFlags : 0;
}

#define SET_FLAG_FUNC(__funcname,__flag) \
	bool KviUserListView::__funcname(const QString &nick,bool bYes) \
	{ \
		KviUserListEntry * it = m_pEntryDict->find(nick); \
		if(!it)return false; \
		m_pEntryDict->setAutoDelete(false); \
		partInternal(nick,false); \
		m_pEntryDict->setAutoDelete(true); \
		if(bYes) \
		{ \
			if(!(it->m_iFlags & __flag))it->m_iFlags |= __flag; \
		} else { \
			if(it->m_iFlags & __flag)it->m_iFlags &= ~__flag; \
		} \
		updateScrollBarRange(); \
		insertUserEntry(nick,it); \
		m_pViewArea->update(); \
		return true; \
	}

SET_FLAG_FUNC(setChanOwner,KVI_USERFLAG_CHANOWNER)
SET_FLAG_FUNC(setChanAdmin,KVI_USERFLAG_CHANADMIN)
SET_FLAG_FUNC(op,KVI_USERFLAG_OP)
SET_FLAG_FUNC(halfop,KVI_USERFLAG_HALFOP)
SET_FLAG_FUNC(userop,KVI_USERFLAG_USEROP)
SET_FLAG_FUNC(voice,KVI_USERFLAG_VOICE)

#define GET_FLAG_FUNC(__funcname,__flag) \
	bool KviUserListView::__funcname(const QString &nick,bool bAtLeast) \
	{ \
		KviUserListEntry * it = m_pEntryDict->find(nick); \
		return it ? (bAtLeast ? (it->m_iFlags >= __flag) : (it->m_iFlags & __flag)) : false; \
	}

GET_FLAG_FUNC(isChanOwner,KVI_USERFLAG_CHANOWNER)
GET_FLAG_FUNC(isChanAdmin,KVI_USERFLAG_CHANADMIN)
GET_FLAG_FUNC(isOp,KVI_USERFLAG_OP)
GET_FLAG_FUNC(isVoice,KVI_USERFLAG_VOICE)
GET_FLAG_FUNC(isHalfOp,KVI_USERFLAG_HALFOP)
GET_FLAG_FUNC(isUserOp,KVI_USERFLAG_USEROP)


QString * KviUserListView::firstSelectedNickname()
{
	m_pIterator = m_pHeadItem;
	while(m_pIterator)
	{
		if(m_pIterator->m_bSelected)
		{
			QString * s = &(m_pIterator->m_szNick);
			m_pIterator = m_pIterator->m_pNext;
			return s;
		}
		m_pIterator = m_pIterator->m_pNext;
	}
	return 0;
}

QString * KviUserListView::nextSelectedNickname()
{
	while(m_pIterator)
	{
		if(m_pIterator->m_bSelected)
		{
			QString * s = &(m_pIterator->m_szNick);
			m_pIterator = m_pIterator->m_pNext;
			return s;
		}
		m_pIterator = m_pIterator->m_pNext;
	}
	return 0;
}

void KviUserListView::appendSelectedNicknames(QString &buffer)
{
	KviUserListEntry * aux = m_pHeadItem;
	bool bFirst = true;
	while(aux)
	{
		if(aux->m_bSelected)
		{
			if(!bFirst)buffer.append(',');
			else bFirst = false;
			buffer.append(aux->m_szNick);
		}
		aux = aux->m_pNext;
	}
}

void KviUserListView::select(const QString& nick){
	KviDictIterator<KviUserListEntry> it(*m_pEntryDict);
	while(it.current())
	{
		((KviUserListEntry *)it.current())->m_bSelected = false;
		++it;
	}

	KviUserListEntry * entry = m_pEntryDict->find(nick);
	if(entry)
	{
		entry->m_bSelected = true;
		m_iSelectedCount = 1;
	} else {
		m_iSelectedCount = 0;
	}
	g_pFrame->childWindowSelectionStateChange(m_pKviWindow,true);
	m_pViewArea->update();
}

bool KviUserListView::partInternal(const QString &nick,bool bRemove)
{
	KviUserListEntry * it = m_pEntryDict->find(nick);
	if(it)
	{
		// so, first of all..check if this item is over, or below the top item
		KviUserListEntry * e = m_pHeadItem;
		bool bGotTopItem = false;
		while(e != it)
		{
			if(e == m_pTopItem)
			{
				bGotTopItem = true;
				e = it;
			} else e = e->m_pNext;
		}
		if(bRemove)m_pIrcUserDataBase->removeUser(nick,it->m_pGlobalData);

		// now just remove it
		if(it->m_iFlags & KVI_USERFLAG_OP)m_iOpCount--;
		if(it->m_iFlags & KVI_USERFLAG_VOICE)m_iVoiceCount--;
		if(it->m_iFlags & KVI_USERFLAG_HALFOP)m_iHalfOpCount--;
		if(it->m_iFlags & KVI_USERFLAG_CHANADMIN)m_iChanAdminCount--;
		if(it->m_iFlags & KVI_USERFLAG_CHANOWNER)m_iChanOwnerCount--;
		if(it->m_iFlags & KVI_USERFLAG_USEROP)m_iUserOpCount--;
		if(it->m_bSelected)
		{
			m_iSelectedCount--;
			if(m_iSelectedCount == 0)g_pFrame->childWindowSelectionStateChange(m_pKviWindow,false);
		}
		if(it->m_pPrev)it->m_pPrev->m_pNext = it->m_pNext;
		if(it->m_pNext)it->m_pNext->m_pPrev = it->m_pPrev;
		if(m_pTopItem == it)
		{
			bGotTopItem = true; //!!! the previous while() does not handle it!
			m_pTopItem = it->m_pNext;
			if(m_pTopItem == 0)m_pTopItem = it->m_pPrev;
		}
		if(it == m_pHeadItem)m_pHeadItem = it->m_pNext;
		if(it == m_pTailItem)m_pTailItem = it->m_pPrev;
		m_iTotalHeight -= it->m_iHeight;

		int iHeight = it->m_iHeight;

		m_pEntryDict->remove(nick);

		if(bGotTopItem)
		{
			// removing after (or exactly) the top item, may be visible
			if(bRemove)triggerUpdate();
		} else {
			// removing over (before) the top item...not visible
			m_pViewArea->m_bIgnoreScrollBar = true;
			m_pViewArea->m_iLastScrollBarVal -= iHeight;
			m_pViewArea->m_pScrollBar->setValue(m_pViewArea->m_iLastScrollBarVal);
//			m_pViewArea->m_pScrollBar->setRange(0,m_iTotalHeight);
			updateScrollBarRange();
			m_pViewArea->m_bIgnoreScrollBar = false;
			if(bRemove)updateUsersLabel();
		}

		return true;
	}
	return false;
}

bool KviUserListView::nickChange(const QString &oldNick,const QString &newNick)
{
	KviUserListEntry * it = m_pEntryDict->find(oldNick);
	if(it)
	{
		QString user      = it->m_pGlobalData->user();
		QString host      = it->m_pGlobalData->host();
		int iFlags        = it->m_iFlags;
		kvi_time_t joint  = it->m_joinTime;
		bool bSelect      = it->m_bSelected;
		KviAvatar * av    = it->m_pGlobalData->forgetAvatar();
		KviIrcUserEntry::Gender gender = it->m_pGlobalData->gender();
		part(oldNick);
		__range_invalid(m_pEntryDict->find(oldNick));
		it = join(newNick,user,host,iFlags);
		it->m_pGlobalData->setGender(gender);
		it->m_joinTime = joint;
		it->m_lastActionTime = kvi_unixTime();
		it->m_bSelected = bSelect;
		it->m_iTemperature += KVI_USERACTION_NICK;
		if(av)
		{
			it->m_pGlobalData->setAvatar(av);
			avatarChanged(newNick);
		}
		return true;
	}
	return false;
}


void KviUserListView::updateUsersLabel()
{
	if(!KVI_OPTION_BOOL(KviOption_boolDisableUserListLabel))//G&N  2005
	{
		KviStr tmp;
		tmp.sprintf("<nobr><b>[%u]</b>",m_pEntryDict->count());
		if(m_iChanOwnerCount)tmp.append(KviStr::Format," q:%d",m_iChanOwnerCount);
		if(m_iChanAdminCount)tmp.append(KviStr::Format," a:%d",m_iChanAdminCount);
		if(m_iOpCount)tmp.append(KviStr::Format," o:%d",m_iOpCount);
		if(m_iHalfOpCount)tmp.append(KviStr::Format," h:%d",m_iHalfOpCount);
		if(m_iVoiceCount)tmp.append(KviStr::Format," v:%d",m_iVoiceCount);
		if(m_iUserOpCount)tmp.append(KviStr::Format," u:%d",m_iUserOpCount);
		if(m_ibEntries)tmp.append(KviStr::Format," b:%d",m_ibEntries);
		if(m_ieEntries)tmp.append(KviStr::Format," e:%d",m_ieEntries);
		if(m_iIEntries)tmp.append(KviStr::Format," I:%d",m_iIEntries);
		tmp.append("</nobr>");
		m_pUsersLabel->setText(tmp.ptr());
	}
}

// FIXME: this could be done really better
void KviUserListView::partAllButOne(const QString &whoNot)
{
	QStringList ll;
	KviDictIterator<KviUserListEntry> it(*m_pEntryDict);
	while(it.current())
	{
		if(!KviQString::equalCI(whoNot,it.currentKey()))
			ll.append(it.currentKey());
		++it;
	}
	for(QStringList::Iterator it2 = ll.begin();it2 != ll.end();it2++)
	{
		part(*it2);
	}
}

void KviUserListView::removeAllEntries()
{
	KviDictIterator<KviUserListEntry> it(*m_pEntryDict);
	while(it.current())
	{
		m_pIrcUserDataBase->removeUser(it.currentKey(),
			((KviUserListEntry *)it.current())->m_pGlobalData);
		++it;
	}
	m_pEntryDict->clear();
	m_pHeadItem = 0;
	m_pTopItem = 0;
	m_iVoiceCount = 0;
	m_iHalfOpCount = 0;
	m_iChanAdminCount = 0;
	m_iChanOwnerCount = 0;
	m_iOpCount = 0;
	m_iUserOpCount = 0;
	if(m_iSelectedCount != 0)
	{
		m_iSelectedCount = 0;
		g_pFrame->childWindowSelectionStateChange(m_pKviWindow,false);
	}
	m_pViewArea->m_iTopItemOffset = 0;
	m_pViewArea->m_iLastScrollBarVal = 0;
	m_pViewArea->m_bIgnoreScrollBar = true;
	m_pViewArea->m_pScrollBar->setValue(0);
	m_iTotalHeight = 0;
	updateScrollBarRange();
	m_pViewArea->m_bIgnoreScrollBar = false;  // gfgf
}

void KviUserListView::partAll()
{
	removeAllEntries();
	triggerUpdate();
}

void KviUserListView::resizeEvent(QResizeEvent *)
{
	int hght;
	if(!KVI_OPTION_BOOL(KviOption_boolDisableUserListLabel))//G&N  2005
	{
		hght = m_pUsersLabel->sizeHint().height();
		if(hght < 16)hght = 16; // at least
		m_pUsersLabel->setGeometry(0,0,width(),hght);
	}
	else
	{ 
		hght =0;
	}

	m_pViewArea->setGeometry(0,hght,width(),height() - hght);

	updateScrollBarRange();
}

bool KviUserListView::itemVisible(KviUserListEntry * e)
{
	KviUserListEntry * le = m_pTopItem;
	int curTop = KVI_USERLIST_BORDER_WIDTH - m_pViewArea->m_iTopItemOffset;
	int hght   = height();
	while(le && (curTop < hght))
	{
		if(le == e)return true;
		curTop += le->m_iHeight;
		le = le->m_pNext;
	}
	return false;
}

KviUserListEntry * KviUserListView::itemAt(const QPoint &pnt,QRect * rct)
{
	if(!m_pTopItem)return 0;
	if(pnt.y() < 0)return 0;
	int curTop = KVI_USERLIST_BORDER_WIDTH - m_pViewArea->m_iTopItemOffset;
	KviUserListEntry * e = m_pTopItem;
	int curBottom = 0;
	while(e && (curTop <= m_pViewArea->height()))
	{
		curBottom = curTop + e->m_iHeight;
		if((pnt.y() >= curTop) && (pnt.y() < curBottom))
		{
			if(rct)
			{
				rct->setX(0);
				rct->setY(curTop);
				rct->setWidth(m_pViewArea->width());
				rct->setHeight(e->m_iHeight);
			}
			return e;
		}
		curTop = curBottom;
		e = e->m_pNext;
	}
	return 0;
}



void KviUserListView::userStats(KviUserListViewUserStats * s)
{
	s->uTotal = m_pEntryDict->count();
	s->uHot = 0;
	s->uHotOp = 0;
	s->uActive = 0;
	s->uActiveOp = 0;
	s->uChanAdmin = 0;
	s->uChanOwner = 0;
	s->iAvgTemperature = 0;
	s->uOp = 0;
	s->uVoiced = 0;
	s->uHalfOp = 0;
	s->uUserOp = 0;

	KviUserListEntry * e = m_pHeadItem;

	kvi_time_t curTime = kvi_unixTime();

	while(e)
	{
		if(e->m_lastActionTime)
		{
			unsigned int uTimeDiff = (((unsigned int)(curTime - e->m_lastActionTime)) >> 6);
			if(uTimeDiff < 10)
			{
				s->uActive++; // the user was alive in the last ~16 mins
				if(e->m_iFlags & (KVI_USERFLAG_OP | KVI_USERFLAG_CHANADMIN | KVI_USERFLAG_CHANOWNER))
				{
					s->uActiveOp++;
					if(e->m_iTemperature > 0)
					{
						s->uHot++;
						s->uHotOp++;
					}
				} else {
					if(e->m_iTemperature > 0)s->uHot++;
				}
				s->iAvgTemperature += e->m_iTemperature;
			}
		}
		if(e->m_iFlags & KVI_USERFLAG_CHANOWNER)s->uChanOwner++;
		else {
			if(e->m_iFlags & KVI_USERFLAG_CHANADMIN)s->uChanAdmin++;
			else {
				if(e->m_iFlags & KVI_USERFLAG_OP)s->uOp++;
				else {
					if(e->m_iFlags & KVI_USERFLAG_HALFOP)s->uHalfOp++;
					else {
						if(e->m_iFlags & KVI_USERFLAG_VOICE)s->uVoiced++;
						else {
							if(e->m_iFlags & KVI_USERFLAG_USEROP)s->uUserOp++;
						}
					}
				}
			}
		}
		e = e->m_pNext;
	}

	if(s->uActive > 0)s->iAvgTemperature /= ((int)s->uActive);
}


void KviUserListView::maybeTip(KviUserListToolTip * tip,const QPoint &pnt)
{
	if(!KVI_OPTION_BOOL(KviOption_boolShowUserListViewToolTips))return;
	QRect itRect;
	KviUserListEntry * it = (KviUserListEntry *)itemAt(pnt,&itRect);
	if(it)
	{
		if(m_pKviWindow->console())
		{
			QString buffer;
			m_pKviWindow->console()->getUserTipText(it->m_szNick,it->m_pGlobalData,buffer);

			buffer += "<table width=\"100%\">";

			if(it->m_joinTime != 0)
			{
				QDateTime dt;
				dt.setTime_t(it->m_joinTime);
				buffer += "<tr><td bgcolor=\"#F0F0F0\"><nobr>";
				buffer += __tr2qs("Joined on <b>%1</b>").arg(dt.toString());
				buffer += "</nobr></td></tr>";
			}
	
			if(it->m_lastActionTime != 0)
			{
				int secs = kvi_unixTime() - it->m_lastActionTime;
				int mins = secs / 60;
				secs = secs % 60;
				int hors = mins / 60;
				mins = mins % 60;
				buffer += "<tr><td bgcolor=\"#F0F0F0\"><nobr>";
				buffer += __tr2qs("Quiet for <b>%1h %2m %3s</b>").arg(hors).arg(mins).arg(secs);
				buffer += "</nobr></td></tr>";
			}
			buffer += "</table>";
	
			tip->doTip(itRect,buffer);
		}
	}
}

///////////////////////////////////////////////////////////////

KviUserListViewArea::KviUserListViewArea(KviUserListView * par)
: QWidget(par)
{
	m_pListView = par;
	setBackgroundMode(QWidget::NoBackground);
	m_pScrollBar = new QScrollBar(QScrollBar::Vertical,this,"scrollbar");
	m_pScrollBar->setRange(0,0);
	m_pScrollBar->setValue(0);
	connect(m_pScrollBar,SIGNAL(valueChanged(int)),this,SLOT(scrollBarMoved(int)));
	m_pScrollBar->setPageStep(height());
	m_pScrollBar->setLineStep(m_pListView->m_iFontHeight);
	m_iLastScrollBarVal = 0;
	m_iTopItemOffset = 0;
	m_bIgnoreScrollBar = false;
	m_pLastEntryUnderMouse = 0;
}

KviUserListViewArea::~KviUserListViewArea()
{
}

void KviUserListViewArea::scrollBarMoved(int newVal)
{
	if(m_bIgnoreScrollBar)return;
	int diff = newVal - m_iLastScrollBarVal;
	if(m_pListView->m_pTopItem)
	{
		while(diff > 0)
		{
			int nextH = (m_pListView->m_pTopItem->m_iHeight - m_iTopItemOffset);
			if(diff >= nextH)
			{
				// the diff is greater than the top item visible part
				diff -= nextH;
				if(m_pListView->m_pTopItem->m_pNext)
				{
					// There is a next item (offset to 0)
					m_pListView->m_pTopItem = m_pListView->m_pTopItem->m_pNext;
					m_iTopItemOffset = 0;
				} else {
					// No next item (rather a bug) (offset to the top item size)
					m_iTopItemOffset = m_pListView->m_pTopItem->m_iHeight;
					diff = 0;
				}
			} else {
				// just offset the top item
				m_iTopItemOffset += diff;
				diff = 0;
			}
		}
		while(diff < 0)
		{
			if((-diff) <= m_iTopItemOffset)
			{
				// just move the top item
				m_iTopItemOffset += diff;
				diff = 0;
			} else {
				diff += m_iTopItemOffset;
				if(m_pListView->m_pTopItem->m_pPrev)
				{
					// There is a prev item (offset to 0)
					m_pListView->m_pTopItem = m_pListView->m_pTopItem->m_pPrev;
					m_iTopItemOffset = m_pListView->m_pTopItem->m_iHeight;
				} else {
					// No prev item (rather a bug) (offset to the top item size)
					m_iTopItemOffset = 0;
					diff = 0;
				}
			}
		}
	}
	m_iLastScrollBarVal = newVal;
	update();

}

void KviUserListViewArea::paintEvent(QPaintEvent *ev)
{
	// update the scroll bar

	if(!isVisible())return;

	// if the mdiManager is in SDI mode
	// and this window is attacched but is not the toplevel one
	// then it is hidden completely behind the other windows
	// and we can avoid to paint it :)
	if(g_pFrame->mdiManager()->isInSDIMode() &&
		(m_pListView->window()->mdiParent() != g_pFrame->mdiManager()->topChild()) &&
		(m_pListView->window()->mdiParent()))
	{
		return; // totally hidden behind other windows
	}

	int wdth = width() - m_pScrollBar->width();

	QRect r = ev->rect();
	if(r.right() > wdth)r.setRight(wdth);

	//debug("PAINT EVENT %d,%d,%d,%d",r.left(),r.top(),r.width(),r.height());

	KviDoubleBuffer db(width(),height());
	QPixmap * pMemBuffer = db.pixmap();

	QPainter p(pMemBuffer);
	p.setFont(KVI_OPTION_FONT(KviOption_fontUserListView));

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt = mapToGlobal(QPoint(r.left(),r.top()));
		p.drawTiledPixmap(r.left(),r.top(),r.width(),r.height(),*g_pShadedChildGlobalDesktopBackground,pnt.x(),pnt.y());
	} else {
#endif
		p.fillRect(r.left(),r.top(),r.width(),r.height(),KVI_OPTION_COLOR(KviOption_colorUserListViewBackground));
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	}
#endif

	QPixmap *pix=KVI_OPTION_PIXMAP(KviOption_pixmapUserListViewBackground).pixmap();
	if(pix)
	{
		KviPixmapUtils::drawPixmapWithPainter(&p,pix,(Qt::AlignmentFlags)(KVI_OPTION_UINT(KviOption_uintUserListPixmapAlign)),r,width(),height());
	}

	KviUserListEntry * e = m_pListView->m_pTopItem;

	int theY = KVI_USERLIST_BORDER_WIDTH - m_iTopItemOffset;

	kvi_time_t curTime = kvi_unixTime();

	bool bShowIcons = KVI_OPTION_BOOL(KviOption_boolShowUserChannelIcons);
	bool bShowState = KVI_OPTION_BOOL(KviOption_boolShowUserChannelState);
	bool bShowGender = KVI_OPTION_BOOL(KviOption_boolDrawGenderIcons);
	
	while(e && theY <= r.bottom())
	{
		int bottom = theY + e->m_iHeight;
		// theY is our top line
		// theX is our left corner
		// bottom is our bottom line
		// wdth is the width of the whole widget

		if(bottom >= r.top())
		{
			QColor * pClrFore = 0;
			bool bColorAllocated=0; // FIXME: bool is true or false
			if(e->m_bSelected)
			{
				p.fillRect(0,theY,wdth,e->m_iHeight,KVI_OPTION_COLOR(KviOption_colorUserListViewSelectionBackground));
				pClrFore = &(KVI_OPTION_COLOR(KviOption_colorUserListViewSelectionForeground));
			} else if(KVI_OPTION_BOOL(KviOption_boolUseDifferentColorForOwnNick) && m_pListView->m_pKviWindow->connection())
			{
				if(e->m_szNick==m_pListView->m_pKviWindow->connection()->currentNickName())
					pClrFore = &(KVI_OPTION_COLOR(KviOption_colorUserListViewOwnForeground));
			} 
			 
			if(!pClrFore){
				// FIXME:
				//
				// this is slow... VERY slow when one has a lot of registered users.
				//   (this is NOT a simple lookup in the user db... it is a mask match)
				// if we REALLY need to use custom colors for regged users then
				// they should be updated ONCE and stored (cached) in the KviUserListEntry structure
				//
				KviRegisteredUser* pRegisteredUser=g_pRegisteredUserDataBase->findMatchingUser(e->m_szNick,e->m_pGlobalData->user(),e->m_pGlobalData->host());
				if(pRegisteredUser)
				{
					if(pRegisteredUser->getBoolProperty("useCustomColor"))
					{
						pClrFore = new QColor();
						QString szTmp=pRegisteredUser->getProperty("customColor");
						KviStringConversion::fromString(szTmp,(*pClrFore));
						bColorAllocated=1; // bool is true or false
					}
				}
				if(!pClrFore)
				{
					if(e->m_iFlags == 0)
					{
						pClrFore = &(KVI_OPTION_COLOR(KviOption_colorUserListViewNormalForeground));
					} else {
						pClrFore = &(KVI_OPTION_COLOR((e->m_iFlags & KVI_USERFLAG_CHANOWNER) ? \
							KviOption_colorUserListViewChanOwnerForeground : ((e->m_iFlags & KVI_USERFLAG_CHANADMIN) ? \
							KviOption_colorUserListViewChanAdminForeground : ((e->m_iFlags & KVI_USERFLAG_OP) ? \
							KviOption_colorUserListViewOpForeground : ((e->m_iFlags & KVI_USERFLAG_HALFOP) ? \
							KviOption_colorUserListViewHalfOpForeground : ((e->m_iFlags & KVI_USERFLAG_VOICE) ? \
							KviOption_colorUserListViewVoiceForeground : KviOption_colorUserListViewUserOpForeground))))));
					}
				}
			}
			
			int theX = KVI_USERLIST_BORDER_WIDTH + 1;
	
			int iAvatarAndTextX = theX;
	
			if(bShowGender)iAvatarAndTextX += 11;
			if(bShowIcons)iAvatarAndTextX += 18;
			if(bShowState)iAvatarAndTextX += 11;

			if(KVI_OPTION_BOOL(KviOption_boolUserListViewDrawGrid))
			{
				// the grid
				switch(KVI_OPTION_UINT(KviOption_uintUserListViewGridType))
				{
					case KVI_USERLISTVIEW_GRIDTYPE_PLAINGRID:
					case KVI_USERLISTVIEW_GRIDTYPE_DOTGRID:
						p.setPen(QPen(KVI_OPTION_COLOR(KviOption_colorUserListViewGrid),0,
							(KVI_OPTION_UINT(KviOption_uintUserListViewGridType) == KVI_USERLISTVIEW_GRIDTYPE_DOTGRID) ? QPen::DotLine : QPen::SolidLine));
						p.drawLine(0,bottom - 1,wdth,bottom - 1);
						if(bShowState || bShowIcons)
							p.drawLine(iAvatarAndTextX,bottom - 1,iAvatarAndTextX,theY);
					break;
					default: // KVI_USERLISTVIEW_GRIDTYPE_3DGRID and KVI_USERLISTVIEW_GRIDTYPE_3DBUTTONS
						if(!e->m_bSelected)
						{
							p.setPen(QPen(KVI_OPTION_COLOR(KviOption_colorUserListViewGrid),0 /*,QPen::DotLine*/));
							if((bShowState || bShowIcons) && (KVI_OPTION_UINT(KviOption_uintUserListViewGridType) == KVI_USERLISTVIEW_GRIDTYPE_3DGRID))
								p.drawLine(iAvatarAndTextX,bottom - 1,iAvatarAndTextX,theY);
							p.setPen(colorGroup().shadow());
							p.drawLine(0,bottom - 1,wdth,bottom - 1);
							p.setPen(colorGroup().light());
							p.drawLine(0,theY,wdth,theY);
							theY--;
						}
						theY++;
					break;
				}
				iAvatarAndTextX += 3;
			} else {
				iAvatarAndTextX += 1;
			}

			if(e->globalData()->isAway())
			{
				QRgb rgb2 = pClrFore->rgb();
				QRgb rgb1 = KVI_OPTION_COLOR(KviOption_colorUserListViewAwayForeground).rgb();
				p.setPen(QColor(((qRed(rgb1)*2) + qRed(rgb2)) / 3,((qGreen(rgb1)*2) + qGreen(rgb2)) / 3,((qBlue(rgb1)*2) + qBlue(rgb2)) / 3));
			} else {
				p.setPen(*pClrFore);
			}
			theY+=2;
		
			if(!KVI_OPTION_BOOL(KviOption_boolDisableAvatars))//G&N  2005
			{
				KviAvatar * av = e->m_pGlobalData->avatar();
				if(av && KVI_OPTION_UINT(KviOption_uintAvatarScaleWidth) && KVI_OPTION_UINT(KviOption_uintAvatarScaleHeight))
				{
					QPixmap * pix;
					if( KVI_OPTION_BOOL(KviOption_boolScaleAvatars) &&
						(
							!KVI_OPTION_BOOL(KviOption_boolDoNotStretchAvatars) ||
							(av->pixmap()->width() > KVI_OPTION_UINT(KviOption_uintAvatarScaleWidth)) ||
							(av->pixmap()->height() > KVI_OPTION_UINT(KviOption_uintAvatarScaleHeight))
						)
					)
					pix=av->scaledPixmap(KVI_OPTION_UINT(KviOption_uintAvatarScaleWidth),KVI_OPTION_UINT(KviOption_uintAvatarScaleHeight));
					else 
						pix=av->pixmap();
					p.drawPixmap(iAvatarAndTextX,theY,*pix);
					theY += pix->height() + 1;
				}
			}

			if(bShowGender && e->globalData()->gender()!=KviIrcUserEntry::Unknown)
			{
				QPixmap * ico = g_pIconManager->getBigIcon((e->globalData()->gender()==KviIrcUserEntry::Male) ? "kvi_icon_male.png" : "kvi_icon_female.png");
				p.drawPixmap(theX,theY+(m_pListView->m_iFontHeight-11)/2,*ico);
				theX +=11;
			}

			if(bShowState)
			{
				if(e->m_lastActionTime)
				{
					// the g_pUserChanStatePixmap is 36 x 80 pixels
					// divided into 6 rows of 5 pixmaps
					// row 0 is hot , row 5 is cold
					// left is most active , right is least active
					// e->m_iTemperature is a signed short , negative values are cold
					// e->m_lastActionTime is the time of the last action (eventually 0 , if not known)
					// 6 bit right shift is an aprox division for 64 : really aprox minutes
					unsigned int uTimeDiff = (((unsigned int)(curTime - e->m_lastActionTime)) >> 6);
					if(uTimeDiff < 16)
					{
						//p.drawRect(theX,theY + 2,10,e->m_iHeight - 4);
						static int xOffTable[16] =
						{
							0  , 8  , 16 , 16 ,
							24 , 24 , 24 , 24 ,
							32 , 32 , 32 , 32 ,
							32 , 32 , 32 , 32
						};
						// the temperature now
						// temp > 100 is hot (offset y = 0)
						// temp < -100 is cold (offset y = 80)
						// temp > 30 is half-hot (offset y = 16)
						// temp < -30 is half-cold (offset y = 64)
						// temp > 0 is a-bit-hot (offset y = 32)
						// temp < 0 is a-bit-cold (offset y = 48)

						if(e->m_iTemperature > KVI_MID_TEMPERATURE)
						{
							if(e->m_iTemperature > KVI_HALF_HOT_TEMPERATURE)
							{
								if(e->m_iTemperature > KVI_HOT_TEMPERATURE)
								{
									// hot
									p.drawPixmap(theX,theY,*g_pUserChanStatePixmap,xOffTable[uTimeDiff],0,8,16);
								} else {
									// half-hot
									p.drawPixmap(theX,theY,*g_pUserChanStatePixmap,xOffTable[uTimeDiff],16,8,16);
								}
							} else {
								// bit-hot
								p.drawPixmap(theX,theY,*g_pUserChanStatePixmap,xOffTable[uTimeDiff],32,8,16);
							}
						} else {
							if(e->m_iTemperature < KVI_HALF_COLD_TEMPERATURE)
							{
								if(e->m_iTemperature < KVI_COLD_TEMPERATURE)
								{
									// cold
									p.drawPixmap(theX,theY,*g_pUserChanStatePixmap,xOffTable[uTimeDiff],80,8,16);
								} else {
									// half-cold
									p.drawPixmap(theX,theY,*g_pUserChanStatePixmap,xOffTable[uTimeDiff],64,8,16);
								}
							} else {
								// bit-cold
								p.drawPixmap(theX,theY,*g_pUserChanStatePixmap,xOffTable[uTimeDiff],48,8,16);
							}
						} 
					}
				}
				theX += 11;
			}
			
			if(bShowIcons)
			{
				//p.drawRect(theX,theY + 2,18,e->m_iHeight - 4);
				if(e->m_iFlags != 0)
				{
					QPixmap * ico = g_pIconManager->getSmallIcon( \
											e->globalData()->isAway() ? \
												( \
													(e->m_iFlags & KVI_USERFLAG_CHANOWNER) ? \
													KVI_SMALLICON_CHANOWNERAWAY : ((e->m_iFlags & KVI_USERFLAG_CHANADMIN) ? \
													KVI_SMALLICON_CHANADMINAWAY : ((e->m_iFlags & KVI_USERFLAG_OP) ? \
													KVI_SMALLICON_OPAWAY : ((e->m_iFlags & KVI_USERFLAG_HALFOP) ? \
													KVI_SMALLICON_HALFOPAWAY : ((e->m_iFlags & KVI_USERFLAG_VOICE) ? \
													KVI_SMALLICON_VOICEAWAY : KVI_SMALLICON_USEROPAWAY))))
												) \
											: \
												( \
													(e->m_iFlags & KVI_USERFLAG_CHANOWNER) ? \
													KVI_SMALLICON_CHANOWNER : ((e->m_iFlags & KVI_USERFLAG_CHANADMIN) ? \
													KVI_SMALLICON_CHANADMIN : ((e->m_iFlags & KVI_USERFLAG_OP) ? \
													KVI_SMALLICON_OP : ((e->m_iFlags & KVI_USERFLAG_HALFOP) ? \
													KVI_SMALLICON_HALFOP : ((e->m_iFlags & KVI_USERFLAG_VOICE) ? \
													KVI_SMALLICON_VOICE : KVI_SMALLICON_USEROP)))) \
												) \
										);
					p.drawPixmap(theX,theY,*ico);
				}
				theX +=18;
				p.drawText(iAvatarAndTextX,theY,wdth - theX,m_pListView->m_iFontHeight,AlignLeft|AlignVCenter,e->m_szNick);
			} else {

				char flag = m_pListView->getUserFlag(e);
				if(flag)
				{
					QString ttt = QChar(flag);
					ttt += e->m_szNick;
					p.drawText(iAvatarAndTextX,theY,wdth - theX,m_pListView->m_iFontHeight,AlignLeft|AlignVCenter,ttt);
				} else {
					p.drawText(iAvatarAndTextX,theY,wdth - theX,m_pListView->m_iFontHeight,AlignLeft|AlignVCenter,e->m_szNick);
				}
			}
			if(bColorAllocated) delete pClrFore;
		}

		theY = bottom;
		e = e->m_pNext;
	}

	p.setPen(colorGroup().dark());
	p.drawLine(0,0,wdth,0);
	p.drawLine(0,0,0,height());
	p.setPen(colorGroup().light());
	p.drawLine(1,height()-1,wdth,height()-1);
	p.drawLine(wdth - 1,1,wdth - 1,height());

	bitBlt(this,r.left(),r.top(),pMemBuffer,r.left(),r.top(),r.width(),r.height(),Qt::CopyROP,false);
}

void KviUserListViewArea::resizeEvent(QResizeEvent *)
{
	int iScr = m_pScrollBar->sizeHint().width();
	m_pScrollBar->setGeometry(width() - iScr,0,iScr,height());
	m_pScrollBar->setPageStep(height());
	m_pScrollBar->setLineStep(m_pListView->m_iFontHeight - 1);
}

void KviUserListViewArea::mousePressEvent(QMouseEvent *e)
{
	setFocus();
	if(e->button() & LeftButton)
	{
		KviUserListEntry * entry = m_pListView->itemAt(e->pos());
		if(entry)
		{
			if(e->state() & ShiftButton)
			{
				// Multiselect mode
				if(!entry->m_bSelected)m_pListView->m_iSelectedCount++;
				entry->m_bSelected = true;
				if(m_pListView->m_iSelectedCount == 1)g_pFrame->childWindowSelectionStateChange(m_pListView->m_pKviWindow,true);
				update();
			} else if(e->state() & ControlButton)
			{
				// Invert mode
				if(!entry->m_bSelected)m_pListView->m_iSelectedCount++;
				else m_pListView->m_iSelectedCount--;
				entry->m_bSelected = ! entry->m_bSelected;
				if(m_pListView->m_iSelectedCount == 0)g_pFrame->childWindowSelectionStateChange(m_pListView->m_pKviWindow,false);
				else if(m_pListView->m_iSelectedCount == 1)g_pFrame->childWindowSelectionStateChange(m_pListView->m_pKviWindow,true);
				update();
			} else {
				// Single select mode
				bool bThereWasSelection = false;
				if(m_pListView->m_iSelectedCount > 0)
				{
					KviUserListEntry * aux = m_pListView->m_pHeadItem;
					while(aux)
					{
						aux->m_bSelected = false;
						aux = aux->m_pNext;
					}
					bThereWasSelection = true;
				}
				entry->m_bSelected = true;
				m_pListView->m_iSelectedCount = 1;
				if(!bThereWasSelection)g_pFrame->childWindowSelectionStateChange(m_pListView->m_pKviWindow,true);
				update();
			}
		}
		m_pLastEntryUnderMouse = entry;
	} else if(e->button() & RightButton)
	{
		KviUserListEntry * entry = m_pListView->itemAt(e->pos());
		if(entry)
		{
			if(!entry->m_bSelected)m_pListView->m_iSelectedCount++;
			entry->m_bSelected = true;
			if(m_pListView->m_iSelectedCount == 1)g_pFrame->childWindowSelectionStateChange(m_pListView->m_pKviWindow,true);
			update();
		}
		m_pListView->emitRightClick();
	}
}
void KviUserListViewArea::keyPressEvent( QKeyEvent * e )
{
	if(e->key()==Qt::Key_Escape)
	{
		if(m_pListView->m_pKviWindow->input())
			((QWidget*)(m_pListView->m_pKviWindow->input()))->setFocus();
	} else {
		QString szKey=e->text();
		if(!szKey.isEmpty())
		{
			KviUserListEntry * nick=0;
			KviUserListEntry * aux = m_pListView->m_pHeadItem;
			while(aux)
			{
				//debug("%s %s %i %s %i",__FILE__,__FUNCTION__,__LINE__,aux->nick().utf8().data(),aux->nick().find(szKey,0,0));
				if(aux->nick().find(szKey,0,0)==0) 
				{
					nick=aux;
					break;
				}
				aux = aux->m_pNext;
			}
			if(nick)
			{
				bool bThereWasSelection = false;
				if(m_pListView->m_iSelectedCount > 0)
				{
					aux = m_pListView->m_pHeadItem;
					while(aux)
					{
						aux->m_bSelected = false;
						aux = aux->m_pNext;
					}
					bThereWasSelection = true;
				}
				nick->m_bSelected = true;
				m_pListView->m_iSelectedCount = 1;
				if(!bThereWasSelection)g_pFrame->childWindowSelectionStateChange(m_pListView->m_pKviWindow,true);
				update();
			}
		}
	}
}

void KviUserListViewArea::mouseDoubleClickEvent(QMouseEvent *e)
{
	m_pListView->emitDoubleClick();
}

void KviUserListViewArea::mouseMoveEvent(QMouseEvent *e)
{
	if(e->state() & LeftButton)
	{
		KviUserListEntry * entry = m_pListView->itemAt(e->pos());
		if(entry && (entry != m_pLastEntryUnderMouse))
		{
			if(e->state() & ControlButton)
			{
				if(entry->m_bSelected)m_pListView->m_iSelectedCount--;
				else m_pListView->m_iSelectedCount++;
				entry->m_bSelected = ! entry->m_bSelected;
				if(m_pListView->m_iSelectedCount == 0)g_pFrame->childWindowSelectionStateChange(m_pListView->m_pKviWindow,false);
				else if(m_pListView->m_iSelectedCount == 1)g_pFrame->childWindowSelectionStateChange(m_pListView->m_pKviWindow,true);
			} else {
				if(!entry->m_bSelected)m_pListView->m_iSelectedCount++;
				entry->m_bSelected = true;
				if(m_pListView->m_iSelectedCount == 1)g_pFrame->childWindowSelectionStateChange(m_pListView->m_pKviWindow,true);
			}
			update();
			m_pLastEntryUnderMouse = entry;
		} else {
			// out of the widget ?
			if(entry == m_pLastEntryUnderMouse)return;
			if(e->pos().y() < KVI_USERLIST_BORDER_WIDTH)
			{
				KviUserListEntry * top = m_pListView->m_pTopItem;
				if(top)
				{
					m_pScrollBar->setValue(m_pScrollBar->value() - top->m_iHeight);
					if(m_pListView->m_pTopItem != top)
					{
						if(e->state() & ControlButton)
						{
							if(m_pListView->m_pTopItem->m_bSelected)m_pListView->m_iSelectedCount--;
							else m_pListView->m_iSelectedCount++;
							m_pListView->m_pTopItem->m_bSelected = ! m_pListView->m_pTopItem->m_bSelected;
							if(m_pListView->m_iSelectedCount == 0)g_pFrame->childWindowSelectionStateChange(m_pListView->m_pKviWindow,false);
							else if(m_pListView->m_iSelectedCount == 1)g_pFrame->childWindowSelectionStateChange(m_pListView->m_pKviWindow,true);
						} else {
							if(!m_pListView->m_pTopItem->m_bSelected)m_pListView->m_iSelectedCount++;
							m_pListView->m_pTopItem->m_bSelected = true;
							if(m_pListView->m_iSelectedCount == 1)g_pFrame->childWindowSelectionStateChange(m_pListView->m_pKviWindow,true);
						}
						update();
					}
				}
				m_pLastEntryUnderMouse = top;
			} else if(e->pos().y() > (height() - KVI_USERLIST_BORDER_WIDTH))
			{
				KviUserListEntry * bottom = m_pListView->m_pTopItem;
				if(bottom)
				{
					int theY = KVI_USERLIST_BORDER_WIDTH - m_iTopItemOffset;
					while(bottom && (theY < height()))
					{
						theY+= bottom->m_iHeight;
						bottom = bottom->m_pNext;
					}
					if(!bottom)bottom = m_pListView->m_pTailItem;
					if(bottom)
					{
						m_pScrollBar->setValue(m_pScrollBar->value() + bottom->m_iHeight);
						if(bottom != m_pLastEntryUnderMouse)
						{
							if(e->state() & ControlButton)
							{
								if(bottom->m_bSelected)m_pListView->m_iSelectedCount--;
								else m_pListView->m_iSelectedCount++;
								bottom->m_bSelected = ! bottom->m_bSelected;
								if(m_pListView->m_iSelectedCount == 0)g_pFrame->childWindowSelectionStateChange(m_pListView->m_pKviWindow,false);
								else if(m_pListView->m_iSelectedCount == 1)g_pFrame->childWindowSelectionStateChange(m_pListView->m_pKviWindow,true);
							} else {
								if(!bottom->m_bSelected)m_pListView->m_iSelectedCount++;
								bottom->m_bSelected = true;
								if(m_pListView->m_iSelectedCount == 1)g_pFrame->childWindowSelectionStateChange(m_pListView->m_pKviWindow,true);
							}
							update();
						}
					}
				}
				m_pLastEntryUnderMouse = bottom;
			} else m_pLastEntryUnderMouse = 0;
		}
	}
}

void KviUserListViewArea::mouseReleaseEvent(QMouseEvent *)
{
	m_pLastEntryUnderMouse = 0;
}

void KviUserListViewArea::wheelEvent(QWheelEvent *e)
{
	g_pApp->sendEvent(m_pScrollBar,e);
}

#include "kvi_userlistview.moc"
