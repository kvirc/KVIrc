//=============================================================================
//
//   File : DccUtils.cpp
//   Creation date : Tue Jul 23 02:54:44 2002 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#define _UTILS_CPP_

#include "DccUtils.h"

#include "KviOptions.h"
#include "KviLocale.h"
#include "KviNetUtils.h"
#include "KviIrcSocket.h"
#include "KviIrcLink.h"
#include "KviQString.h"

/*
bool dcc_module_get_listen_ip_address(KviCommand *c,KviConsoleWindow * pConsole,QString &szListenIp)
{
	//
	// Find an interface suitable for listening....
	// Either from user options or from the current connection...
	//

	if(KVI_OPTION_BOOL(KviOption_boolDccListenOnSpecifiedInterfaceByDefault))
	{
		KVI_OPTION_STRING(KviOption_stringDccListenDefaultInterface).trimmed();
		if(!KVI_OPTION_STRING(KviOption_stringDccListenDefaultInterface).isEmpty())
		{
			if(kvi_isValidStringIp(KVI_OPTION_STRING(KviOption_stringDccListenDefaultInterface).toUtf8().data()))
			{
				if(KviQString::equalCI(KVI_OPTION_STRING(KviOption_stringDccListenDefaultInterface),"0.0.0.0"))
				{
					// Try to find the first available IPv4 interface
					if(!kvi_getLocalHostAddress(szListenIp))
					{
						if(c)c->warning(__tr2qs_ctx("Can't retrieve a suitable local IPV4 address","dcc"),
								KVI_OPTION_STRING(KviOption_stringDccListenDefaultInterface).toUtf8().data());
						return false;
					}
				} else {
					szListenIp = KVI_OPTION_STRING(KviOption_stringDccListenDefaultInterface);
				}
				return true;
			}
#ifdef COMPILE_IPV6_SUPPORT
			if(kvi_isValidStringIPv6(KVI_OPTION_STRING(KviOption_stringDccListenDefaultInterface).toUtf8().data()))
			{
				szListenIp = KVI_OPTION_STRING(KviOption_stringDccListenDefaultInterface);
			} else {
#endif
				if(!kvi_getInterfaceAddress(KVI_OPTION_STRING(KviOption_stringDccListenDefaultInterface).toUtf8().data(),szListenIp))
				{
					KVI_OPTION_BOOL(KviOption_boolDccListenOnSpecifiedInterfaceByDefault) = false;
					if(c)c->warning(__tr2qs_ctx("Can't listen on default interface '%s': fix it in the options dialog, disabling the option (so the next dcc will work)","dcc"),
							KVI_OPTION_STRING(KviOption_stringDccListenDefaultInterface).toUtf8().data());
					return false;
				}
#ifdef COMPILE_IPV6_SUPPORT
			}
#endif
			return true;
		} else {
			// the option was empty.. disable it
			KVI_OPTION_BOOL(KviOption_boolDccListenOnSpecifiedInterfaceByDefault) = false;
		}
	}

	if(pConsole)
	{
		if(pConsole->isConnected())
		{
			//#warning "The IPV6 choice is not OK here.... and maybe allow to bind to specified ports"
			pConsole->socket()->getLocalHostIp(szListenIp,pConsole->isIPv6Connection());
		} else {
			szListenIp = "0.0.0.0"; // huh ? :)
		}
	} else {
		szListenIp = "0.0.0.0";
	}

	return true;
}
*/

bool dcc_kvs_get_listen_ip_address(KviKvsModuleCommandCall * c, KviConsoleWindow * pConsole, QString & szListenIp)
{
	//
	// Find an interface suitable for listening....
	// Either from user options or from the current connection...
	//

	if(KVI_OPTION_BOOL(KviOption_boolDccListenOnSpecifiedInterfaceByDefault))
	{
		KVI_OPTION_STRING(KviOption_stringDccListenDefaultInterface) = KVI_OPTION_STRING(KviOption_stringDccListenDefaultInterface).trimmed();
		if(!KVI_OPTION_STRING(KviOption_stringDccListenDefaultInterface).isEmpty())
		{
			if(KviNetUtils::isValidStringIp(KVI_OPTION_STRING(KviOption_stringDccListenDefaultInterface).toUtf8().data()))
			{
				if(KviQString::equalCI(KVI_OPTION_STRING(KviOption_stringDccListenDefaultInterface), "0.0.0.0"))
				{
					// Try to find the first available IPv4 interface
					if(!kvi_getLocalHostAddress(szListenIp))
					{
						if(c)
							c->warning(__tr2qs_ctx("Can't retrieve a suitable local IPv4 address", "dcc"),
							    KVI_OPTION_STRING(KviOption_stringDccListenDefaultInterface).toUtf8().data());
						return false;
					}
				}
				else
				{
					szListenIp = KVI_OPTION_STRING(KviOption_stringDccListenDefaultInterface);
				}
				return true;
			}
#ifdef COMPILE_IPV6_SUPPORT
			if(KviNetUtils::isValidStringIPv6(KVI_OPTION_STRING(KviOption_stringDccListenDefaultInterface).toUtf8().data()))
			{
				szListenIp = KVI_OPTION_STRING(KviOption_stringDccListenDefaultInterface);
			}
			else
			{
#endif
				if(!KviNetUtils::getInterfaceAddress(KVI_OPTION_STRING(KviOption_stringDccListenDefaultInterface), szListenIp))
				{
					KVI_OPTION_BOOL(KviOption_boolDccListenOnSpecifiedInterfaceByDefault) = false;
					if(c)
						c->warning(__tr2qs_ctx("Can't listen on default interface '%s': fix it in the options dialog, disabling the option (so the next DCC will work)", "dcc"),
						    KVI_OPTION_STRING(KviOption_stringDccListenDefaultInterface).toUtf8().data());
					return false;
				}
#ifdef COMPILE_IPV6_SUPPORT
			}
#endif
			return true;
		}
		else
		{
			// the option was empty.. disable it
			KVI_OPTION_BOOL(KviOption_boolDccListenOnSpecifiedInterfaceByDefault) = false;
		}
	}

	if(pConsole)
	{
		if(pConsole->isConnected())
		{
			//#warning "The IPV6 choice is not OK here.... and maybe allow to bind to specified ports"
			pConsole->connection()->link()->socket()->getLocalHostIp(szListenIp, pConsole->isIPv6Connection());
		}
		else
		{
			szListenIp = "0.0.0.0"; // huh ? :)
		}
	}
	else
	{
		szListenIp = "0.0.0.0";
	}

	return true;
}
