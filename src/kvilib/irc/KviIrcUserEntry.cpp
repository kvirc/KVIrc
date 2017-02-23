//=============================================================================
//
//   File : KviIrcUserEntry.cpp
//   Creation date : Tue Jan 04 2010 22:45:12 by Elvio Basello
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2010 Elvio Basello (hellvis69 at gmail dot com)
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

#include "KviIrcUserEntry.h"
#include "KviControlCodes.h"
#include "KviNickColors.h"

KviIrcUserEntry::KviIrcUserEntry(const QString & szUser, const QString & szHost)
{
	m_szUser = szUser;
	m_szHost = szHost;
	m_nRefs = 1;
	m_iHops = -1;
	m_bAway = false;
	m_bIrcOp = false;
	m_eGender = Unknown;
	m_bBot = false;
	m_bNotFoundRegUserLookup = false;
	m_bUseCustomColor = false;
	m_bAvatarRequested = false;
	m_iSmartNickColor = -1;
	m_szAccountName = QString();
}

void KviIrcUserEntry::setRealName(const QString & szReal)
{
	m_szRealName = szReal.trimmed();
	if(m_szRealName.length() >= 3)
	{
		if((m_szRealName[0].unicode() == KviControlCodes::Color) && (m_szRealName[2].unicode() == KviControlCodes::Reset))
		{
			// hum.. encoded as hidden color code eh ? publish is somewhere, so others might implement this...
			// for backwards compatibility, 3=bot
			if(m_szRealName[1].unicode() & 1 && m_szRealName[1].unicode() & 2)
			{
				setBot(true); //3
			}
			else
			{
				if(m_szRealName[1].unicode() & 1)
				{
					setGender(Male); //1
				}
				else
				{
					if(m_szRealName[1].unicode() & 2)
					{
						setGender(Female); //2
					}
				}
			}
			m_szRealName.remove(0, 3);
		}
	}

	/*
	 * smart nick color code: min length=5, max length=7
	 * 1 - ^K, 2 - digit, 3 - coma, 4 - digit, 5 - KVI_TEXT_RESET, 6 - start of realname.
	 */
	if(m_szRealName.length() >= 5)
	{
		if(m_szRealName[0].unicode() == KviControlCodes::Color)
		{
			unsigned char cFore, cBack;
			int iPos = KviControlCodes::getUnicodeColorBytes(m_szRealName, 1, &cFore, &cBack);
			// extract smart nick color code
			if(iPos > 1 && m_szRealName[iPos] == KviControlCodes::Reset)
			{
				m_szRealName.truncate(iPos);
				int iColor = KviNickColors::getSmartColorIntByMircColor(cFore, cBack);
				if(iColor >= 0)
					setSmartNickColor(iColor);
				m_szRealName.remove(0, iPos + 1);
			}
		}
	}
}

void KviIrcUserEntry::setAvatar(std::unique_ptr<KviAvatar> upAvatar)
{
	m_upAvatar = std::move(upAvatar);
}

// Overload function for backwards compatibility
void KviIrcUserEntry::setAvatar(KviAvatar * pAvatar)
{
	m_upAvatar.reset(pAvatar);
}

std::unique_ptr<KviAvatar> KviIrcUserEntry::forgetAvatar()
{
	return std::move(m_upAvatar);
}
