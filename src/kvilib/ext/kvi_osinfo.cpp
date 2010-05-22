//=============================================================================
//
//   File : kvi_osinfo.cpp
//   Creation date : Thu Jan 19 2006 13:50:11 GMT by Alexey Uzhva
//
//   This toolbar is part of the KVirc irc client distribution
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


#include "kvi_osinfo.h"
#include "kvi_locale.h"
#include "kvi_qstring.h"

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

//Vista :/

#define PRODUCT_BUSINESS 0x00000006
#define PRODUCT_BUSINESS_N 0x00000010
#define PRODUCT_CLUSTER_SERVER 0x00000012
#define PRODUCT_DATACENTER_SERVER 0x00000008
#define PRODUCT_DATACENTER_SERVER_CORE 0x0000000C
#define PRODUCT_ENTERPRISE 0x00000004
#define PRODUCT_ENTERPRISE_SERVER 0x0000000A
#define PRODUCT_ENTERPRISE_SERVER_CORE 0x0000000E
#define PRODUCT_ENTERPRISE_SERVER_IA64 0x0000000F
#define PRODUCT_HOME_BASIC 0x00000002
#define PRODUCT_HOME_BASIC_N 0x00000005
#define PRODUCT_HOME_PREMIUM 0x00000003
#define PRODUCT_HOME_SERVER 0x00000013
#define PRODUCT_SERVER_FOR_SMALLBUSINESS 0x00000018
#define PRODUCT_SMALLBUSINESS_SERVER 0x00000009
#define PRODUCT_SMALLBUSINESS_SERVER_PREMIUM 0x00000019
#define PRODUCT_STANDARD_SERVER 0x00000007
#define PRODUCT_STANDARD_SERVER_CORE 0x0000000D
#define PRODUCT_STARTER 0x0000000B
#define PRODUCT_STORAGE_ENTERPRISE_SERVER 0x00000017
#define PRODUCT_STORAGE_EXPRESS_SERVER 0x00000014
#define PRODUCT_STORAGE_STANDARD_SERVER 0x00000015
#define PRODUCT_STORAGE_WORKGROUP_SERVER 0x00000016
#define PRODUCT_UNDEFINED 0x00000000
#define PRODUCT_ULTIMATE 0x00000001
#define PRODUCT_WEB_SERVER 0x00000011

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
			return FALSE;
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

		if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 )
		{
			if( osvi.wProductType == VER_NT_WORKSTATION )
				szVersion+= "Windows Vista ";
			else szVersion+="Windows Server \"Longhorn\" ";
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
		{
			if( GetSystemMetrics(SM_SERVERR2) )
				szVersion+="Windows Server 2003 \"R2\" ";
			else if( osvi.wProductType == VER_NT_WORKSTATION &&
				si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
			{
				szVersion+="Windows XP Professional x64 Edition ";
			}
			else szVersion+="Windows Server 2003, ";
		}

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
			szVersion+="Windows XP ";

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
			szVersion+="Windows 2000 ";

		if ( osvi.dwMajorVersion <= 4 )
			szVersion+="Windows NT ";

		PGETPRODUCTINFO pGetProductInfo;
		pGetProductInfo = (PGETPRODUCTINFO) GetProcAddress(
			GetModuleHandle(TEXT("kernel32.dll")), "GetProductInfo");
		// Test for specific product on Windows NT 4.0 SP6 and later.
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
						szVersion+="Business Edition";
						break;
					case PRODUCT_BUSINESS_N:
						szVersion+="Business N Edition";
						break;
					case PRODUCT_CLUSTER_SERVER:
						szVersion+="Cluster Server Edition";
						break;
					case PRODUCT_DATACENTER_SERVER:
						szVersion+="Server Datacenter Edition (full installation)";
						break;
					case PRODUCT_DATACENTER_SERVER_CORE:
						szVersion+="Server Datacenter Edition (core installation)";
						break;
					case PRODUCT_ENTERPRISE:
						szVersion+="Enterprise Edition";
						break;
					case PRODUCT_ENTERPRISE_SERVER:
						szVersion+="Server Enterprise Edition (full installation)";
						break;
					case PRODUCT_ENTERPRISE_SERVER_CORE:
						szVersion+="Server Enterprise Edition (core installation)";
						break;
					case PRODUCT_ENTERPRISE_SERVER_IA64:
						szVersion+="Server Enterprise Edition for Itanium-based Systems";
						break;
					case PRODUCT_HOME_BASIC:
						szVersion+="Home Basic Edition";
						break;
					case PRODUCT_HOME_BASIC_N:
						szVersion+="Home Basic N Edition";
						break;
					case PRODUCT_HOME_PREMIUM:
						szVersion+="Home Premium Edition";
						break;
					case PRODUCT_HOME_SERVER:
						szVersion+="Home Server Edition";
						break;
					case PRODUCT_SERVER_FOR_SMALLBUSINESS:
						szVersion+="Server for Small Business Edition";
						break;
					case PRODUCT_SMALLBUSINESS_SERVER:
						szVersion+="Small Business Server";
						break;
					case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
						szVersion+="Small Business Server Premium Edition";
						break;
					case PRODUCT_STANDARD_SERVER:
						szVersion+="Server Standard Edition (full installation)";
						break;
					case PRODUCT_STANDARD_SERVER_CORE:
						szVersion+="Server Standard Edition (core installation)";
						break;
					case PRODUCT_STARTER:
						szVersion+="Starter Edition";
						break;
					case PRODUCT_STORAGE_ENTERPRISE_SERVER:
						szVersion+="Storage Server Enterprise Edition";
						break;
					case PRODUCT_STORAGE_EXPRESS_SERVER:
						szVersion+="Storage Server Express Edition";
						break;
					case PRODUCT_STORAGE_STANDARD_SERVER:
						szVersion+="Storage Server Standard Edition";
						break;
					case PRODUCT_STORAGE_WORKGROUP_SERVER:
						szVersion+="Storage Server Workgroup Edition";
						break;
					case PRODUCT_UNDEFINED:
						szVersion+="An unknown product";
						break;
					case PRODUCT_ULTIMATE:
						szVersion+="Ultimate Edition";
						break;
					case PRODUCT_WEB_SERVER:
						szVersion+="Web Server Edition";
						break;

					}
					szVersion+=" ";
					if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
					{
						szVersion+="(x64) ";
					}
				} else {
					// Test for the workstation type.
					if ( osvi.wProductType == VER_NT_WORKSTATION &&
						si.wProcessorArchitecture!=PROCESSOR_ARCHITECTURE_AMD64)
					{
						if( osvi.dwMajorVersion == 4 )
							szVersion+= "Workstation 4.0 " ;
						else if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
							szVersion+= "Home Edition " ;
						else szVersion+= "Professional " ;
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
						else if(osvi.dwMajorVersion==5 && osvi.dwMinorVersion==0)
						{
							if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
								szVersion+= "Datacenter Server " ;
							else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
								szVersion+= "Advanced Server " ;
							else szVersion+= "Server " ;
						}
						else  // Windows NT 4.0
						{
							if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
								szVersion+="Server 4.0, Enterprise Edition " ;
							else szVersion+= "Server 4.0 " ;
						}
					}
				}
		}
		// Test for specific product on Windows NT 4.0 SP5 and earlier
		else
		{
			HKEY hKey;
			TCHAR szProductType[BUFSIZE];
			DWORD dwBufLen=BUFSIZE*sizeof(TCHAR);
			LONG lRet;

			lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
				TEXT("SYSTEM\\CurrentControlSet\\Control\\ProductOptions"),
				0, KEY_QUERY_VALUE, &hKey );
			if( lRet != ERROR_SUCCESS )
				return FALSE;

			lRet = RegQueryValueEx( hKey, TEXT("ProductType"), NULL, NULL,
				(LPBYTE) szProductType, &dwBufLen);
			RegCloseKey( hKey );

			if( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE*sizeof(TCHAR)) )
				return FALSE;

			if ( lstrcmpi( TEXT("WINNT"), szProductType) == 0 )
				szVersion+= "Workstation " ;
			if ( lstrcmpi( TEXT("LANMANNT"), szProductType) == 0 )
				szVersion+= "Server " ;
			if ( lstrcmpi( TEXT("SERVERNT"), szProductType) == 0 )
				szVersion+= "Advanced Server " ;
		}


		// Display service pack (if any) and build number.

		if( osvi.dwMajorVersion == 4 &&
			lstrcmpi( osvi.szCSDVersion, TEXT("Service Pack 6") ) == 0 )
		{
			HKEY hKey;
			LONG lRet;

			// Test for SP6 versus SP6a.
			lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
				TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Hotfix\\Q246009"),
				0, KEY_QUERY_VALUE, &hKey );
			if( lRet == ERROR_SUCCESS )
				szVersion+= QString("Service Pack 6a (Build %1)").arg( osvi.dwBuildNumber & 0xFFFF );
			else // Windows NT 4.0 prior to SP6a
			{
				szVersion+= QString( "%1 (Build %2)").arg( osvi.szCSDVersion ).arg(osvi.dwBuildNumber & 0xFFFF);
			}

			RegCloseKey( hKey );
		}
		else // not Windows NT 4.0
		{
			szVersion+= QString( "%1 (Build %2)").arg( osvi.szCSDVersion ).arg(osvi.dwBuildNumber & 0xFFFF);
		}

		break;

		// Test for the Windows Me/98/95.
	case VER_PLATFORM_WIN32_WINDOWS:

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
		{
			szVersion+="Windows 95 ";
			if (osvi.szCSDVersion[1]=='C' || osvi.szCSDVersion[1]=='B')
				szVersion+="OSR2 ";
		}

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
		{
			szVersion+="Windows 98 ";
			if ( osvi.szCSDVersion[1]=='A' || osvi.szCSDVersion[1]=='B')
				szVersion+="SE ";
		}

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
		{
			szVersion+="Windows Millennium Edition";
		}
		break;

	case VER_PLATFORM_WIN32s:

		szVersion+="Win32s";
		break;
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
		if(osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) return " Windows ";
		if(osvi.dwPlatformId == VER_PLATFORM_WIN32s) return " Win32s ";
		return __tr2qs("Unknown ");
	}
	if(info==Os_Version)
	{
		return szVersion;
	}

	return "what's wrong? o.O";
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
