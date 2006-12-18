///////////////////////////////////////////////////////////////////////////////
//
//   File : kvi_osinfo.cpp
//   Creation date : 19  Jan 2006 GMT by Alexey Uzhva
//
//   This toolbar is part of the KVirc irc client distribution
//   Copyright (C) 2006 Alexey Uzhva
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
///////////////////////////////////////////////////////////////////////////////

#define __KVILIB__

#include "kvi_osinfo.h"
#include "kvi_locale.h"

#ifndef COMPILE_ON_WINDOWS
	#include <sys/utsname.h>
	#include <stdlib.h>
	#include <unistd.h>
#endif

#ifdef COMPILE_ON_WINDOWS
#include <windows.h>

typedef enum QueryInfo
{
	Os_Release,
	Os_Version,
	Os_Type
};
typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
#define SM_SERVERR2            89
#define BUFSIZE 1024

static QString queryWinInfo( QueryInfo info)
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

      // Test for specific product on Windows NT 4.0 SP6 and later.
      if( bOsVersionInfoEx )
      {
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
			 szVersion+= QString( "%1 (Build %2)").arg( osvi.szCSDVersion).arg( osvi.dwBuildNumber & 0xFFFF);
         }

         RegCloseKey( hKey );
      }
      else // not Windows NT 4.0 
      {
		  szVersion+= QString( "%1 (Build %2)").arg( osvi.szCSDVersion).arg( osvi.dwBuildNumber & 0xFFFF);
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
#ifdef COMPILE_ON_WINDOWS
		return queryWinInfo(Os_Type);
#else
	#ifdef Q_OS_MACX
		return "macosx";
	#else
		return "unix";
	#endif
#endif
	}
		
	QString name()
	{
#ifdef COMPILE_ON_WINDOWS
		return "windows";
#else
		struct utsname uts;
		if(uname(&uts) == 0)
			return QString::fromLocal8Bit(uts.sysname);
		return QString::null;
#endif
	}
	
	QString version()
	{
#ifdef COMPILE_ON_WINDOWS
		return queryWinInfo(Os_Version);
#else
		struct utsname uts;
		if(uname(&uts) == 0)
			return QString::fromLocal8Bit(uts.version);
		return QString::null;
#endif
	}
	
	QString release()
	{
#ifdef COMPILE_ON_WINDOWS
		return queryWinInfo(Os_Release);
#else
		struct utsname uts;
		if(uname(&uts) == 0)
			return QString::fromLocal8Bit(uts.release);
		return QString::null;
#endif
	}
	
	QString machine()
	{
#ifdef COMPILE_ON_WINDOWS
		QString mach = getenv("PROCESSOR_IDENTIFIER");
		return mach.section(",",0,0);
#else
		struct utsname uts;
		if(uname(&uts) == 0)
			return QString::fromLocal8Bit(uts.machine);
		return QString::null;
#endif
	}
	
	QString nodename()
	{
#ifdef COMPILE_ON_WINDOWS
		return "windows";
#else
		struct utsname uts;
		if(uname(&uts) == 0)
			return QString::fromLocal8Bit(uts.nodename);
		return QString::null;
#endif
	}
	
	QString hostname()
	{
		char hbuffer[1024];
		if(gethostname(hbuffer,1024) == 0)
			return QString::fromLocal8Bit(hbuffer);
		else
			return QString::null;
	}
}