//=============================================================================
//
//   File : KviRuntimeInfo.cpp
//   Creation date : Thu Jan 19 2006 13:50:11 GMT by Alexey Uzhva
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2006-2008 Alexey Uzhva (wizard at opendoor dot ru)
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

#include "KviRuntimeInfo.h"
#include "KviLocale.h"
#include "KviQString.h"

#include <QApplication>
#include <QString>
#include <QStyle>

#if !defined(COMPILE_ON_WINDOWS) && !defined(COMPILE_ON_MINGW)
#include <sys/utsname.h>
#include <cstdlib>
#include <unistd.h>
#endif

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
#include <winsock2.h>

typedef enum _QueryInfo {
	Os_Release,
	Os_Version,
	Os_Type
} QueryInfo;

typedef void(WINAPI * PGNSI)(LPSYSTEM_INFO);
typedef BOOL(WINAPI * PGETPRODUCTINFO)(DWORD, DWORD, DWORD, DWORD, PDWORD);

#define BUFSIZE 1024

// stolen from WinNT.h (last updated from 10.0.17763.0 SDK)
//
// Product types
// This list grows with each OS release.
//
// There is no ordering of values to ensure callers
// do an equality test i.e. greater-than and less-than
// comparisons are not useful.
//
// NOTE: Values in this list should never be deleted.
//       When a product-type 'X' gets dropped from a
//       OS release onwards, the value of 'X' continues
//       to be used in the mapping table of GetProductInfo.
//
// clang-format off
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
#define PRODUCT_PROFESSIONAL_EMBEDDED               0x0000003A
#define PRODUCT_ESSENTIALBUSINESS_SERVER_MGMT       0x0000003B
#define PRODUCT_ESSENTIALBUSINESS_SERVER_ADDL       0x0000003C
#define PRODUCT_ESSENTIALBUSINESS_SERVER_MGMTSVC    0x0000003D
#define PRODUCT_ESSENTIALBUSINESS_SERVER_ADDLSVC    0x0000003E
#define PRODUCT_SMALLBUSINESS_SERVER_PREMIUM_CORE   0x0000003F
#define PRODUCT_CLUSTER_SERVER_V                    0x00000040
#define PRODUCT_EMBEDDED                            0x00000041
#define PRODUCT_STARTER_E                           0x00000042
#define PRODUCT_HOME_BASIC_E                        0x00000043
#define PRODUCT_HOME_PREMIUM_E                      0x00000044
#define PRODUCT_PROFESSIONAL_E                      0x00000045
#define PRODUCT_ENTERPRISE_E                        0x00000046
#define PRODUCT_ULTIMATE_E                          0x00000047
#define PRODUCT_ENTERPRISE_EVALUATION               0x00000048
#define PRODUCT_MULTIPOINT_STANDARD_SERVER          0x0000004C
#define PRODUCT_MULTIPOINT_PREMIUM_SERVER           0x0000004D
#define PRODUCT_STANDARD_EVALUATION_SERVER          0x0000004F
#define PRODUCT_DATACENTER_EVALUATION_SERVER        0x00000050
#define PRODUCT_ENTERPRISE_N_EVALUATION             0x00000054
#define PRODUCT_EMBEDDED_AUTOMOTIVE                 0x00000055
#define PRODUCT_EMBEDDED_INDUSTRY_A                 0x00000056
#define PRODUCT_THINPC                              0x00000057
#define PRODUCT_EMBEDDED_A                          0x00000058
#define PRODUCT_EMBEDDED_INDUSTRY                   0x00000059
#define PRODUCT_EMBEDDED_E                          0x0000005A
#define PRODUCT_EMBEDDED_INDUSTRY_E                 0x0000005B
#define PRODUCT_EMBEDDED_INDUSTRY_A_E               0x0000005C
#define PRODUCT_STORAGE_WORKGROUP_EVALUATION_SERVER 0x0000005F
#define PRODUCT_STORAGE_STANDARD_EVALUATION_SERVER  0x00000060
#define PRODUCT_CORE_ARM                            0x00000061
#define PRODUCT_CORE_N                              0x00000062
#define PRODUCT_CORE_COUNTRYSPECIFIC                0x00000063
#define PRODUCT_CORE_SINGLELANGUAGE                 0x00000064
#define PRODUCT_CORE                                0x00000065
#define PRODUCT_PROFESSIONAL_WMC                    0x00000067
#define PRODUCT_EMBEDDED_INDUSTRY_EVAL              0x00000069
#define PRODUCT_EMBEDDED_INDUSTRY_E_EVAL            0x0000006A
#define PRODUCT_EMBEDDED_EVAL                       0x0000006B
#define PRODUCT_EMBEDDED_E_EVAL                     0x0000006C
#define PRODUCT_NANO_SERVER                         0x0000006D
#define PRODUCT_CLOUD_STORAGE_SERVER                0x0000006E
#define PRODUCT_CORE_CONNECTED                      0x0000006F
#define PRODUCT_PROFESSIONAL_STUDENT                0x00000070
#define PRODUCT_CORE_CONNECTED_N                    0x00000071
#define PRODUCT_PROFESSIONAL_STUDENT_N              0x00000072
#define PRODUCT_CORE_CONNECTED_SINGLELANGUAGE       0x00000073
#define PRODUCT_CORE_CONNECTED_COUNTRYSPECIFIC      0x00000074
#define PRODUCT_CONNECTED_CAR                       0x00000075
#define PRODUCT_INDUSTRY_HANDHELD                   0x00000076
#define PRODUCT_PPI_PRO                             0x00000077
#define PRODUCT_ARM64_SERVER                        0x00000078
#define PRODUCT_EDUCATION                           0x00000079
#define PRODUCT_EDUCATION_N                         0x0000007A
#define PRODUCT_IOTUAP                              0x0000007B
#define PRODUCT_CLOUD_HOST_INFRASTRUCTURE_SERVER    0x0000007C
#define PRODUCT_ENTERPRISE_S                        0x0000007D
#define PRODUCT_ENTERPRISE_S_N                      0x0000007E
#define PRODUCT_PROFESSIONAL_S                      0x0000007F
#define PRODUCT_PROFESSIONAL_S_N                    0x00000080
#define PRODUCT_ENTERPRISE_S_EVALUATION             0x00000081
#define PRODUCT_ENTERPRISE_S_N_EVALUATION           0x00000082
#define PRODUCT_HOLOGRAPHIC                         0x00000087
#define PRODUCT_PRO_SINGLE_LANGUAGE                 0x0000008A
#define PRODUCT_PRO_CHINA                           0x0000008B
#define PRODUCT_ENTERPRISE_SUBSCRIPTION             0x0000008C
#define PRODUCT_ENTERPRISE_SUBSCRIPTION_N           0x0000008D
#define PRODUCT_DATACENTER_NANO_SERVER              0x0000008F
#define PRODUCT_STANDARD_NANO_SERVER                0x00000090
#define PRODUCT_DATACENTER_A_SERVER_CORE            0x00000091
#define PRODUCT_STANDARD_A_SERVER_CORE              0x00000092
#define PRODUCT_DATACENTER_WS_SERVER_CORE           0x00000093
#define PRODUCT_STANDARD_WS_SERVER_CORE             0x00000094
#define PRODUCT_UTILITY_VM                          0x00000095
#define PRODUCT_DATACENTER_EVALUATION_SERVER_CORE   0x0000009F
#define PRODUCT_STANDARD_EVALUATION_SERVER_CORE     0x000000A0
#define PRODUCT_PRO_WORKSTATION                     0x000000A1
#define PRODUCT_PRO_WORKSTATION_N                   0x000000A2
#define PRODUCT_PRO_FOR_EDUCATION                   0x000000A4
#define PRODUCT_PRO_FOR_EDUCATION_N                 0x000000A5
#define PRODUCT_AZURE_SERVER_CORE                   0x000000A8
#define PRODUCT_AZURE_NANO_SERVER                   0x000000A9
#define PRODUCT_ENTERPRISEG                         0x000000AB
#define PRODUCT_ENTERPRISEGN                        0x000000AC
#define PRODUCT_SERVERRDSH                          0x000000AF
#define PRODUCT_CLOUD                               0x000000B2
#define PRODUCT_CLOUDN                              0x000000B3
#define PRODUCT_HUBOS                               0x000000B4
#define PRODUCT_ONECOREUPDATEOS                     0x000000B6
#define PRODUCT_CLOUDE                              0x000000B7
#define PRODUCT_ANDROMEDA                           0x000000B8
#define PRODUCT_IOTOS                               0x000000B9
#define PRODUCT_CLOUDEN                             0x000000BA

#define PRODUCT_UNLICENSED                          0xABCDABCD
// clang-format on

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

	if(!(bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO *)&osvi)))
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if(!GetVersionEx((OSVERSIONINFO *)&osvi))
			return KviQString::Empty;
	}

	// Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.

	pGNSI = (PGNSI)GetProcAddress(
	    GetModuleHandle(TEXT("kernel32.dll")),
	    "GetNativeSystemInfo");
	if(nullptr != pGNSI)
		pGNSI(&si);
	else
		GetSystemInfo(&si);

	switch(osvi.dwPlatformId)
	{
		// Test for the Windows NT product family.

		case VER_PLATFORM_WIN32_NT:

			// Test for the specific product.
			if(osvi.dwMajorVersion == 10 && osvi.dwMinorVersion == 0)
			{
				if(osvi.wProductType == VER_NT_WORKSTATION)
					szVersion += "Windows 10 ";
				else if(osvi.wProductType == VER_NT_SERVER || osvi.wProductType == VER_NT_DOMAIN_CONTROLLER )
					szVersion += "Windows Server ";
						if(osvi.dwBuildNumber <= 14393)
							szVersion += "2016 ";
						else if(osvi.dwBuildNumber <= 17763)
							szVersion += "2019 ";
						else
							szVersion += "vNext ";
			}

			if(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 3)
			{
				if(osvi.wProductType == VER_NT_WORKSTATION)
					szVersion += "Windows 8.1 ";
				else
					szVersion += "Windows Server 2012 R2 ";
			}

			if(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2)
			{
				if(osvi.wProductType == VER_NT_WORKSTATION)
					szVersion += "Windows 8 ";
				else
					szVersion += "Windows Server 2012 ";
			}

			if(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1)
			{
				if(osvi.wProductType == VER_NT_WORKSTATION)
					szVersion += "Windows 7 ";
				else
					szVersion += "Windows Server 2008 R2 ";
			}

			PGETPRODUCTINFO pGetProductInfo;
			pGetProductInfo = (PGETPRODUCTINFO)GetProcAddress(
			    GetModuleHandle(TEXT("kernel32.dll")), "GetProductInfo");
			// from MSDN, Document Date 12/05/2018
			// https://docs.microsoft.com/en-us/windows/desktop/api/sysinfoapi/nf-sysinfoapi-getproductinfo
			// the entire PRODUCT_CORE group has the base Windows version in the
			// returned value. rip out "Windows" of all PRODUCT values as well
			if(bOsVersionInfoEx)
			{
				DWORD dwPlatformInfo;
				if(nullptr != pGetProductInfo)
					if(pGetProductInfo(osvi.dwMajorVersion, osvi.dwMinorVersion,
					       osvi.wServicePackMajor, osvi.wServicePackMinor, &dwPlatformInfo))
					{
						switch(dwPlatformInfo)
						{
							case PRODUCT_BUSINESS:
								szVersion += "Business";
								break;
							case PRODUCT_BUSINESS_N:
								szVersion += "Business N";
								break;
							case PRODUCT_CLUSTER_SERVER:
								szVersion += "HPC Edition";
								break;
							case PRODUCT_CLUSTER_SERVER_V:
								szVersion += "Server Hyper Core V";
								break;
							case PRODUCT_CORE:
								//szVersion+="10 Home";
								break;
							case PRODUCT_CORE_COUNTRYSPECIFIC:
								szVersion += "China";
								break;
							case PRODUCT_CORE_N:
								szVersion += "N";
								break;
							case PRODUCT_CORE_SINGLELANGUAGE:
								szVersion += "Single Language";
								break;
							case PRODUCT_DATACENTER_EVALUATION_SERVER:
								szVersion += "Server Datacenter (evaluation installation)";
								break;
							case PRODUCT_DATACENTER_A_SERVER_CORE:
								szVersion += "Server Datacenter, Semi-Annual Channel (core installation)";
								break;
							case PRODUCT_STANDARD_A_SERVER_CORE:
								szVersion += "Server Standard, Semi-Annual Channel (core installation)";
								break;
							case PRODUCT_DATACENTER_SERVER:
								szVersion += "Server Datacenter (full installation)";
								break;
							case PRODUCT_DATACENTER_SERVER_CORE:
								szVersion += "Server Datacenter (core installation)";
								break;
							case PRODUCT_DATACENTER_SERVER_CORE_V:
								szVersion += "Server Datacenter without Hyper-V (core installation)";
								break;
							case PRODUCT_DATACENTER_SERVER_V:
								szVersion += "Server Datacenter without Hyper-V (full installation)";
								break;
							case PRODUCT_EDUCATION:
								szVersion += "Education";
								break;
							case PRODUCT_EDUCATION_N:
								szVersion += "Education N";
								break;
							case PRODUCT_ENTERPRISE:
								szVersion += "Enterprise";
								break;
							case PRODUCT_ENTERPRISE_E:
								szVersion+= "Enterprise E";
								break;
							case PRODUCT_ENTERPRISE_EVALUATION:
								szVersion += "Enterprise Evaluation";
								break;
							case PRODUCT_ENTERPRISE_N:
								szVersion += "Enterprise N";
								break;
							case PRODUCT_ENTERPRISE_N_EVALUATION:
								szVersion += "Enterprise N (evaluation installation)";
								break;
							case PRODUCT_ENTERPRISE_S:
								szVersion+= "Enterprise 2015 LTSB";
								break;
							case PRODUCT_ENTERPRISE_S_EVALUATION:
								szVersion += "Enterprise 2015 LTSB Evaluation";
								break;
							case PRODUCT_ENTERPRISE_S_N:
								szVersion += "Windows 10 Enterprise 2015 LTSB N";
								break;
							case PRODUCT_ENTERPRISE_S_N_EVALUATION:
								szVersion += "Windows 10 Enterprise 2015 LTSB N Evaluation";
								break;
							case PRODUCT_ENTERPRISE_SERVER:
								szVersion += "Server Enterprise (full installation)";
								break;
							case PRODUCT_ENTERPRISE_SERVER_CORE:
								szVersion += "Server Enterprise (core installation)";
								break;
							case PRODUCT_ENTERPRISE_SERVER_CORE_V:
								szVersion += "Server Enterprise without Hyper-V (core installation)";
								break;
							case PRODUCT_ENTERPRISE_SERVER_IA64:
								szVersion += "Server Enterprise for Itanium-based Systems";
								break;
							case PRODUCT_ENTERPRISE_SERVER_V:
								szVersion += "Server Enterprise without Hyper-V (full installation)";
								break;
							case PRODUCT_ESSENTIALBUSINESS_SERVER_ADDL:
								szVersion += "Essential Server Solution Additional";
								break;
							case PRODUCT_ESSENTIALBUSINESS_SERVER_ADDLSVC:
								szVersion += "Essential Server Solution Additional SVC";
								break;
							case PRODUCT_ESSENTIALBUSINESS_SERVER_MGMT:
								szVersion += "Essential Server Solution Management";
								break;
							case PRODUCT_ESSENTIALBUSINESS_SERVER_MGMTSVC:
								szVersion += "Essential Server Solution Management SVC";
								break;
							case PRODUCT_HOME_BASIC:
								szVersion += "Home Basic";
								break;
							case PRODUCT_HOME_BASIC_E:
								//szVersion+="Not supported";
								break;
							case PRODUCT_HOME_BASIC_N:
								szVersion += "Home Basic N";
								break;
							case PRODUCT_HOME_PREMIUM:
								szVersion += "Home Premium";
								break;
							case PRODUCT_HOME_PREMIUM_E:
								//szVersion+="Not supported";
								break;
							case PRODUCT_HOME_PREMIUM_N:
								szVersion += "Home Premium N";
								break;
							case PRODUCT_HOME_PREMIUM_SERVER:
								szVersion += "Home Server 2011";
								break;
							case PRODUCT_HOME_SERVER:
								szVersion += "Storage Server 2008 R2 Essentials";
								break;
							case PRODUCT_HYPERV:
								szVersion += "Hyper-V Server";
								break;
							case PRODUCT_IOTUAP:
								szVersion += "IoT Core";
								break;
							//case PRODUCT_IOTUAPCOMMERCIAL:
								//szVersion += "IoT Core Commercial";
								//break;
							case PRODUCT_MEDIUMBUSINESS_SERVER_MANAGEMENT:
								szVersion += "Essential Business Server Management Server";
								break;
							case PRODUCT_MEDIUMBUSINESS_SERVER_MESSAGING:
								szVersion += "Essential Business Server Messaging Server";
								break;
							case PRODUCT_MEDIUMBUSINESS_SERVER_SECURITY:
								szVersion += "Essential Business Server Security Server";
								break;
							//case PRODUCT_MOBILE_CORE:
								//szVersion += "Mobile";
								//break;
							//case PRODUCT_MOBILE_ENTERPRISE:
								//szVersion += "Mobile Enterprise";
								//break;
							case PRODUCT_MULTIPOINT_PREMIUM_SERVER:
								szVersion += "MultiPoint Server Premium (full installation)";
								break;
							case PRODUCT_MULTIPOINT_STANDARD_SERVER:
								szVersion += "MultiPoint Server Standard (full installation)";
								break;
							case PRODUCT_PRO_WORKSTATION:
								szVersion += "Pro for Workstations";
								break;
							case PRODUCT_PRO_WORKSTATION_N:
								szVersion += "Pro for Workstations N";
								break;
							case PRODUCT_PROFESSIONAL:
								szVersion += "Pro";
								break;
							case PRODUCT_PROFESSIONAL_E:
								//szVersion+="Not supported";
								break;
							case PRODUCT_PROFESSIONAL_N:
								szVersion += "Pro N";
								break;
							case PRODUCT_PROFESSIONAL_WMC:
								szVersion += "Professional with Media Center";
								break;
							case PRODUCT_SB_SOLUTION_SERVER:
								szVersion += "Small Business Server 2011 Essentials";
								break;
							case PRODUCT_SB_SOLUTION_SERVER_EM:
								szVersion += "Server For SB Solutions EM";
								break;
							case PRODUCT_SERVER_FOR_SB_SOLUTIONS:
								szVersion += "Server For SB Solutions";
								break;
							case PRODUCT_SERVER_FOR_SB_SOLUTIONS_EM:
								szVersion += "Server For SB Solutions EM";
								break;
							case PRODUCT_SERVER_FOR_SMALLBUSINESS:
								szVersion += "Server 2008 for Windows Essential Server Solutions";
								break;
							case PRODUCT_SERVER_FOR_SMALLBUSINESS_V:
								szVersion += "Server 2008 without Hyper-V for Windows Essential Server Solutions";
								break;
							case PRODUCT_SERVER_FOUNDATION:
								szVersion += "Server Foundation";
								break;
							case PRODUCT_SMALLBUSINESS_SERVER:
								szVersion += "Small Business Server";
								break;
							case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
								szVersion += "Small Business Server Premium";
								break;
							case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM_CORE:
								szVersion += "Small Business Server Premium (core installation)";
								break;
							case PRODUCT_SOLUTION_EMBEDDEDSERVER:
								szVersion += "MultiPoint Server";
								break;
							case PRODUCT_STANDARD_EVALUATION_SERVER:
								szVersion += "Server Standard (evaluation installation)";
								break;
							case PRODUCT_STANDARD_SERVER:
								szVersion += "Server Standard";
								break;
							case PRODUCT_STANDARD_SERVER_CORE:
								szVersion += "Server Standard (core installation)";
								break;
							case PRODUCT_STANDARD_SERVER_V:
								szVersion += "Server Standard without Hyper-V";
								break;
							case PRODUCT_STANDARD_SERVER_CORE_V:
								szVersion += "Server Standard without Hyper-V (core installation)";
								break;
							case PRODUCT_STANDARD_SERVER_SOLUTIONS:
								szVersion += "Server Solutions Premium";
								break;
							case PRODUCT_STANDARD_SERVER_SOLUTIONS_CORE:
								szVersion += "Server Solutions Premium (core installation)";
								break;
							case PRODUCT_STARTER:
								szVersion += "Starter";
								break;
							case PRODUCT_STARTER_E:
								//szVersion+="Not supported";
								break;
							case PRODUCT_STARTER_N:
								szVersion += "Starter N";
								break;
							case PRODUCT_STORAGE_ENTERPRISE_SERVER:
								szVersion += "Storage Server Enterprise";
								break;
							case PRODUCT_STORAGE_ENTERPRISE_SERVER_CORE:
								szVersion += "Storage Server Enterprise (core installation)";
								break;
							case PRODUCT_STORAGE_EXPRESS_SERVER:
								szVersion += "Storage Server Express";
								break;
							case PRODUCT_STORAGE_EXPRESS_SERVER_CORE:
								szVersion += "Storage Server Express (core installation)";
								break;
							case PRODUCT_STORAGE_STANDARD_EVALUATION_SERVER:
								szVersion += "Storage Server Standard (evaluation installation)";
								break;
							case PRODUCT_STORAGE_STANDARD_SERVER:
								szVersion += "Storage Server Standard";
								break;
							case PRODUCT_STORAGE_STANDARD_SERVER_CORE:
								szVersion += "Storage Server Standard (core installation)";
								break;
							case PRODUCT_STORAGE_WORKGROUP_EVALUATION_SERVER:
								szVersion += "Storage Server Workgroup (evaluation installation)";
								break;
							case PRODUCT_STORAGE_WORKGROUP_SERVER:
								szVersion += "Storage Server Workgroup";
								break;
							case PRODUCT_STORAGE_WORKGROUP_SERVER_CORE:
								szVersion += "Storage Server Workgroup (core installation)";
								break;
							case PRODUCT_ULTIMATE:
								szVersion += "Ultimate";
								break;
							case PRODUCT_ULTIMATE_E:
								//szVersion+="Not supported";
								break;
							case PRODUCT_ULTIMATE_N:
								szVersion += "Ultimate N";
								break;
							case PRODUCT_UNDEFINED:
								szVersion += "An unknown product";
								break;
							// just use unknown here since we do not care.
							case PRODUCT_UNLICENSED:
								szVersion += "An unknown product";
								break;
							case PRODUCT_WEB_SERVER:
								szVersion += "Web Server (full installation)";
								break;
							case PRODUCT_WEB_SERVER_CORE:
								szVersion += "Web Server (core installation)";
								break;
						}
						szVersion += " ";
						if(si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
						{
							szVersion += "(x64) ";
						}
					}
			}
			// Display service pack (if any) and build number.
			szVersion += QString("%1 (Build %2)").arg(QString::fromWCharArray(osvi.szCSDVersion)).arg(osvi.dwBuildNumber & 0xFFFF);
	}
	if(info == Os_Release)
	{
		unsigned long major = osvi.dwMajorVersion;
		unsigned long minor = osvi.dwMinorVersion;
		unsigned long release = osvi.dwBuildNumber;
		QString szMajor, szMinor, szRelease, szVersion;
		szMajor.setNum(major);
		szMinor.setNum(minor);
		szRelease.setNum(release);
		szVersion = "Release : " + szMajor + "." + szMinor + "." + szRelease;
		return szVersion;
	}
	if(info == Os_Type)
	{
		if(osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
			return " NT ";
		return __tr2qs("Unknown ");
	}
	if(info == Os_Version)
	{
		return szVersion;
	}

	return KviQString::Empty;
}
#endif

namespace KviRuntimeInfo
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
#elif defined(COMPILE_ON_MAC)
		return "macOS";
#else
		struct utsname uts;
		if(uname(&uts) == 0)
			return QString::fromLocal8Bit(uts.sysname);
		return KviQString::Empty;
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
		return mach.section(",", 0, 0);
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
		if(gethostname(hbuffer, 1024) == 0)
			return QString::fromLocal8Bit(hbuffer);
		else
			return KviQString::Empty;
	}

	QString qtVersion()
	{
		return QString(qVersion());
	}

	QString qtTheme()
	{
		static QString theme{qApp->style()->objectName().isEmpty() ?
		                         __tr2qs("Overridden with a stylesheet") :
		                         qApp->style()->objectName()};
		return theme;
	}
}
