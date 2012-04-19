//=============================================================================
//
//   File : KviOsInfo.cpp
//   Creation date : Thu Jan 19 2006 13:50:11 GMT by Alexey Uzhva
//
//   This toolbar is part of the KVIrc irc client distribution
//   Copyright (C) 2006-2008 Alexey Uzhva (wizard at opendoor dot ru)
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


#include "KviOsInfo.h"
#include "KviLocale.h"
#include "KviQString.h"

#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
	#include <sys/utsname.h>
	#include <stdlib.h>
	#include <unistd.h>
#endif

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
#include <windows.h>

typedef enum _QueryInfo
{
	Os_Release,
	Os_Version,
	Os_Type
} QueryInfo;

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
typedef BOOL (WINAPI *PGETPRODUCTINFO)(DWORD,DWORD,DWORD,DWORD,PDWORD);

#define SM_SERVERR2            89
#define BUFSIZE 1024

// List stolen from WinNT.h (last updated from 7.1 SDK)
// MSDN: If the product has not been activated and is no longer in
// the grace period, this parameter is set to PRODUCT_UNLICENSED (0xABCDABCD).
// Otherwise, this parameter can be one of the following values.

#define PRODUCT_UNDEFINED                           0x00000000

#define PRODUCT_ULTIMATE                            0x00000001
#define PRODUCT_HOME_BASIC                          0x00000002
#define PRODUCT_HOME_PREMIUM                        0x00000003
#define PRODUCT_ENTERPRISE                          0x00000004
#define PRODUCT_HOME_BASIC_N                        0x00000005
#define PRODUCT_BUSINESS                            0x00000006
#define PRODUCT_STANDARD_SERVER                     0x00000007
#define PRODUCT_DATACENTER_SERVER                   0x00000008
#define PRODUCT_SMALLBUSINESS_SERVER                0x00000009
#define PRODUCT_ENTERPRISE_SERVER                   0x0000000A
#define PRODUCT_STARTER                             0x0000000B
#define PRODUCT_DATACENTER_SERVER_CORE              0x0000000C
#define PRODUCT_STANDARD_SERVER_CORE                0x0000000D
#define PRODUCT_ENTERPRISE_SERVER_CORE              0x0000000E
#define PRODUCT_ENTERPRISE_SERVER_IA64              0x0000000F
#define PRODUCT_BUSINESS_N                          0x00000010
#define PRODUCT_WEB_SERVER                          0x00000011
#define PRODUCT_CLUSTER_SERVER                      0x00000012
#define PRODUCT_HOME_SERVER                         0x00000013
#define PRODUCT_STORAGE_EXPRESS_SERVER              0x00000014
#define PRODUCT_STORAGE_STANDARD_SERVER             0x00000015
#define PRODUCT_STORAGE_WORKGROUP_SERVER            0x00000016
#define PRODUCT_STORAGE_ENTERPRISE_SERVER           0x00000017
#define PRODUCT_SERVER_FOR_SMALLBUSINESS            0x00000018
#define PRODUCT_SMALLBUSINESS_SERVER_PREMIUM        0x00000019
#define PRODUCT_HOME_PREMIUM_N                      0x0000001A
#define PRODUCT_ENTERPRISE_N                        0x0000001B
#define PRODUCT_ULTIMATE_N                          0x0000001C
#define PRODUCT_WEB_SERVER_CORE                     0x0000001D
#define PRODUCT_MEDIUMBUSINESS_SERVER_MANAGEMENT    0x0000001E
#define PRODUCT_MEDIUMBUSINESS_SERVER_SECURITY      0x0000001F
#define PRODUCT_MEDIUMBUSINESS_SERVER_MESSAGING     0x00000020
#define PRODUCT_SERVER_FOUNDATION                   0x00000021
#define PRODUCT_HOME_PREMIUM_SERVER                 0x00000022
#define PRODUCT_SERVER_FOR_SMALLBUSINESS_V          0x00000023
#define PRODUCT_STANDARD_SERVER_V                   0x00000024
#define PRODUCT_DATACENTER_SERVER_V                 0x00000025
#define PRODUCT_ENTERPRISE_SERVER_V                 0x00000026
#define PRODUCT_DATACENTER_SERVER_CORE_V            0x00000027
#define PRODUCT_STANDARD_SERVER_CORE_V              0x00000028
#define PRODUCT_ENTERPRISE_SERVER_CORE_V            0x00000029
#define PRODUCT_HYPERV                              0x0000002A
#define PRODUCT_STORAGE_EXPRESS_SERVER_CORE         0x0000002B
#define PRODUCT_STORAGE_STANDARD_SERVER_CORE        0x0000002C
#define PRODUCT_STORAGE_WORKGROUP_SERVER_CORE       0x0000002D
#define PRODUCT_STORAGE_ENTERPRISE_SERVER_CORE      0x0000002E
#define PRODUCT_STARTER_N                           0x0000002F
#define PRODUCT_PROFESSIONAL                        0x00000030
#define PRODUCT_PROFESSIONAL_N                      0x00000031
#define PRODUCT_SB_SOLUTION_SERVER                  0x00000032
#define PRODUCT_SERVER_FOR_SB_SOLUTIONS             0x00000033
#define PRODUCT_STANDARD_SERVER_SOLUTIONS           0x00000034
#define PRODUCT_STANDARD_SERVER_SOLUTIONS_CORE      0x00000035
#define PRODUCT_SB_SOLUTION_SERVER_EM               0x00000036
#define PRODUCT_SERVER_FOR_SB_SOLUTIONS_EM          0x00000037
#define PRODUCT_SOLUTION_EMBEDDEDSERVER             0x00000038
#define PRODUCT_SOLUTION_EMBEDDEDSERVER_CORE        0x00000039
#define PRODUCT_SMALLBUSINESS_SERVER_PREMIUM_CORE   0x0000003F
#define PRODUCT_ESSENTIALBUSINESS_SERVER_MGMT       0x0000003B
#define PRODUCT_ESSENTIALBUSINESS_SERVER_ADDL       0x0000003C
#define PRODUCT_ESSENTIALBUSINESS_SERVER_MGMTSVC    0x0000003D
#define PRODUCT_ESSENTIALBUSINESS_SERVER_ADDLSVC    0x0000003E
#define PRODUCT_CLUSTER_SERVER_V                    0x00000040
#define PRODUCT_EMBEDDED                            0x00000041
#define PRODUCT_STARTER_E                           0x00000042
#define PRODUCT_HOME_BASIC_E                        0x00000043
#define PRODUCT_HOME_PREMIUM_E                      0x00000044
#define PRODUCT_PROFESSIONAL_E                      0x00000045
#define PRODUCT_ENTERPRISE_E                        0x00000046
#define PRODUCT_ULTIMATE_E                          0x00000047

#define PRODUCT_UNLICENSED                          0xABCDABCD

static QString queryWinInfo(QueryInfo info)
{
	QString szVersion;
	OSVERSIONINFOEX osvi;
	SYSTEM_INFO si;
	PGNSI pGNSI;
	BOOL bOsVersionInfoEx;

	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

	// Try calling GetVersionEx using the OSVERSIONINFOEX structure.
	// If that fails, try using the OSVERSIONINFO structure.

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) )
			return KviQString::Empty;
	}

	// Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.

	pGNSI = (PGNSI) GetProcAddress(
		GetModuleHandle(TEXT("kernel32.dll")),
		"GetNativeSystemInfo");
	if(NULL != pGNSI)
		pGNSI(&si);
	else GetSystemInfo(&si);

	switch (osvi.dwPlatformId)
	{
		// Test for the Windows NT product family.

	case VER_PLATFORM_WIN32_NT:

		// Test for the specific product.

		if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2 )
		{
			if( osvi.wProductType == VER_NT_WORKSTATION )
				szVersion+= "Windows 8 ";
			else szVersion+="Windows Server 2012 ";
		}

		if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1 )
		{
			if( osvi.wProductType == VER_NT_WORKSTATION )
				szVersion+= "Windows 7 ";
			else szVersion+="Windows Server 2008 R2 ";
		}

		if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 )
		{
			if( osvi.wProductType == VER_NT_WORKSTATION )
				szVersion+= "Windows Vista ";
			else szVersion+="Windows Server 2008 ";
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
		{
			if( GetSystemMetrics(SM_SERVERR2) )
				szVersion+="Windows Server 2003 \"R2\" ";
			else if( osvi.wProductType == VER_NT_WORKSTATION &&
				si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
			{
				szVersion+="Windows XP Professional x64 ";
			}
			else szVersion+="Windows Server 2003, ";
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
			szVersion+="Windows XP ";

		PGETPRODUCTINFO pGetProductInfo;
		pGetProductInfo = (PGETPRODUCTINFO) GetProcAddress(
			GetModuleHandle(TEXT("kernel32.dll")), "GetProductInfo");
		// from MSDN, Document Date 9/7/2011
		if( bOsVersionInfoEx )
		{
			DWORD dwPlatformInfo;
			if(NULL != pGetProductInfo)
				if(pGetProductInfo(osvi.dwMajorVersion,osvi.dwMinorVersion,
					osvi.wServicePackMajor,osvi.wServicePackMinor,&dwPlatformInfo))
				{
					switch(dwPlatformInfo)
					{
					case PRODUCT_BUSINESS:
						szVersion+="Business";
						break;
					case PRODUCT_BUSINESS_N:
						szVersion+="Business N";
						break;
					case PRODUCT_CLUSTER_SERVER:
						szVersion+="HPC Edition";
						break;
					case PRODUCT_DATACENTER_SERVER:
						szVersion+="Server Datacenter (full installation)";
						break;
					case PRODUCT_DATACENTER_SERVER_CORE:
						szVersion+="Server Datacenter (core installation)";
						break;
					case PRODUCT_DATACENTER_SERVER_CORE_V:
						szVersion+="Server Datacenter without Hyper-V (core installation)";
						break;
					case PRODUCT_DATACENTER_SERVER_V:
						szVersion+="Server Datacenter without Hyper-V (full installation)";
						break;
					case PRODUCT_ENTERPRISE:
						szVersion+="Enterprise";
						break;
					case PRODUCT_ENTERPRISE_N:
						szVersion+="Enterprise N";
						break;
					case PRODUCT_ENTERPRISE_SERVER:
						szVersion+="Server Enterprise (full installation)";
						break;
					case PRODUCT_ENTERPRISE_SERVER_CORE:
						szVersion+="Server Enterprise (core installation)";
						break;
					case PRODUCT_ENTERPRISE_SERVER_IA64:
						szVersion+="Server Enterprise for Itanium-based Systems";
						break;
					case PRODUCT_ENTERPRISE_SERVER_V:
						szVersion+="Server Enterprise without Hyper-V (full installation)";
						break;
					case PRODUCT_HOME_BASIC:
						szVersion+="Home Basic";
						break;
					case PRODUCT_HOME_BASIC_N:
						szVersion+="Home Basic N";
						break;
					case PRODUCT_HOME_PREMIUM:
						szVersion+="Home Premium";
						break;
					case PRODUCT_HOME_PREMIUM_N:
						szVersion+="Home Premium N";
						break;
					case PRODUCT_HYPERV:
						szVersion+="Hyper-V Server";
						break;
					case PRODUCT_MEDIUMBUSINESS_SERVER_MANAGEMENT:
						szVersion+="Essential Business Server Management Server";
						break;
					case PRODUCT_MEDIUMBUSINESS_SERVER_MESSAGING:
						szVersion+="Essential Business Server Messaging Server";
						break;
					case PRODUCT_MEDIUMBUSINESS_SERVER_SECURITY:
						szVersion+="Essential Business Server Security Server";
						break;
					case PRODUCT_PROFESSIONAL:
						szVersion+="Professional";
						break;
					case PRODUCT_PROFESSIONAL_N:
						szVersion+="Professional N";
						break;
					case PRODUCT_SERVER_FOR_SMALLBUSINESS:
						szVersion+="Server 2008 for Windows Essential Server Solutions";
						break;
					case PRODUCT_SERVER_FOR_SMALLBUSINESS_V:
						szVersion+="Server 2008 without Hyper-V for Windows Essential Server Solutions";
						break;
					case PRODUCT_SERVER_FOUNDATION:
						szVersion+="Server Foundation";
						break;
					case PRODUCT_HOME_PREMIUM_SERVER:
						szVersion+="Home Server 2011";
						break;
					case PRODUCT_SB_SOLUTION_SERVER:
						szVersion+="Small Business Server 2011 Essentials";
						break;
					case PRODUCT_HOME_SERVER:
						szVersion+="Windows Storage Server 2008 R2 Essentials";
						break;
					case PRODUCT_SMALLBUSINESS_SERVER:
						szVersion+="Small Business Server";
						break;
					case PRODUCT_SOLUTION_EMBEDDEDSERVER:
						szVersion+="Windows MultiPoint Server";
						break;
					case PRODUCT_STANDARD_SERVER:
						szVersion+="Server Standard (full installation)";
						break;
					case PRODUCT_STANDARD_SERVER_CORE:
						szVersion+="Server Standard (core installation)";
						break;
					case PRODUCT_STANDARD_SERVER_CORE_V:
						szVersion+="Server Standard without Hyper-V (core installation)";
						break;
					case PRODUCT_STANDARD_SERVER_V:
						szVersion+="Server Standard without Hyper-V (full installation)";
						break;
					case PRODUCT_STARTER:
						szVersion+="Starter";
						break;
					case PRODUCT_STARTER_N:
						szVersion+="Starter N";
						break;
					case PRODUCT_STORAGE_ENTERPRISE_SERVER:
						szVersion+="Storage Server Enterprise";
						break;
					case PRODUCT_STORAGE_EXPRESS_SERVER:
						szVersion+="Storage Server Express";
						break;
					case PRODUCT_STORAGE_STANDARD_SERVER:
						szVersion+="Storage Server Standard";
						break;
					case PRODUCT_STORAGE_WORKGROUP_SERVER:
						szVersion+="Storage Server Workgroup";
						break;
					case PRODUCT_UNDEFINED:
						szVersion+="An unknown product";
						break;
					// just use unknown here since we do not care.
					case PRODUCT_UNLICENSED:
						szVersion+="An unknown product";
						break;
					case PRODUCT_ULTIMATE:
						szVersion+="Ultimate";
						break;
					case PRODUCT_ULTIMATE_N:
						szVersion+="Ultimate N";
						break;
					case PRODUCT_WEB_SERVER:
						szVersion+="Web Server (full installation)";
						break;
					case PRODUCT_WEB_SERVER_CORE:
						szVersion+="Web Server (core installation)";
						break;

					}
					szVersion+=" ";
					if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
					{
						szVersion+="(x64) ";
					}
				} else {
					// Test for the workstation type, for XP 32 bit
					if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
					{
						if ( osvi.wProductType == VER_NT_WORKSTATION )
						{
							if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
								szVersion+= "Home Edition " ;
							else szVersion+= "Professional " ;
						}
					}

					// Test for the server type.
					else if ( osvi.wProductType == VER_NT_SERVER ||
						osvi.wProductType == VER_NT_DOMAIN_CONTROLLER )
					{
						if(osvi.dwMajorVersion==5 && osvi.dwMinorVersion==2)
						{
							if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_IA64 )
							{
								if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
									szVersion+= "Datacenter Edition for Itanium-based Systems" ;
								else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
									szVersion+= "Enterprise Edition for Itanium-based Systems" ;
							}

							else if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
							{
								if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
									szVersion+= "Datacenter x64 Edition " ;
								else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
									szVersion+= "Enterprise x64 Edition " ;
								else szVersion+= "Standard x64 Edition " ;
							}

							else
							{
								if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
									szVersion+= "Datacenter Edition " ;
								else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
									szVersion+= "Enterprise Edition " ;
								else if ( osvi.wSuiteMask == VER_SUITE_BLADE )
									szVersion+= "Web Edition " ;
								else szVersion+= "Standard Edition " ;
							}
						}
					}
				}
		}
		// Display service pack (if any) and build number.
		szVersion+= QString( "%1 (Build %2)").arg( osvi.szCSDVersion ).arg(osvi.dwBuildNumber & 0xFFFF);
	}
	if(info==Os_Release)
	{
		unsigned long major = osvi.dwMajorVersion;
		unsigned long minor = osvi.dwMinorVersion;
		unsigned long release = osvi.dwBuildNumber;
		QString szMajor, szMinor, szRelease, szVersion;
		szMajor.setNum(major);
		szMinor.setNum(minor);
		szRelease.setNum(release);
		szVersion = "Release : "+szMajor +"."+ szMinor +"."+ szRelease;
		return szVersion;
	}
	if(info==Os_Type)
	{
		if(osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) return " NT ";
		return __tr2qs("Unknown ");
	}
	if(info==Os_Version)
	{
		return szVersion;
	}

	return KviQString::Empty;
}
#endif


namespace KviOsInfo
{
	QString type()
	{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		return queryWinInfo(Os_Type);
#else
		return "UNIX";
#endif
	}

	QString name()
	{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		return "Windows";
#else
	#ifdef COMPILE_ON_MAC
		return "MacOS X";
	#else
		struct utsname uts;
		if(uname(&uts) == 0)
			return QString::fromLocal8Bit(uts.sysname);
		return KviQString::Empty;
	#endif
#endif
	}

	QString version()
	{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		return queryWinInfo(Os_Version);
#else
		struct utsname uts;
		if(uname(&uts) == 0)
			return QString::fromLocal8Bit(uts.version);
		return KviQString::Empty;
#endif
	}

	QString release()
	{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		return queryWinInfo(Os_Release);
#else
		struct utsname uts;
		if(uname(&uts) == 0)
			return QString::fromLocal8Bit(uts.release);
		return KviQString::Empty;
#endif
	}

	QString machine()
	{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		QString mach = getenv("PROCESSOR_IDENTIFIER");
		return mach.section(",",0,0);
#else
		struct utsname uts;
		if(uname(&uts) == 0)
			return QString::fromLocal8Bit(uts.machine);
		return KviQString::Empty;
#endif
	}

	QString nodename()
	{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		return "windows";
#else
		struct utsname uts;
		if(uname(&uts) == 0)
			return QString::fromLocal8Bit(uts.nodename);
		return KviQString::Empty;
#endif
	}

	QString hostname()
	{
		char hbuffer[1024];
		if(gethostname(hbuffer,1024) == 0)
			return QString::fromLocal8Bit(hbuffer);
		else
			return KviQString::Empty;
	}
}
