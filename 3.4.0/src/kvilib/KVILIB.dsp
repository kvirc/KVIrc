# Microsoft Developer Studio Project File - Name="KVILIB" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=KVILIB - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "KVILIB.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "KVILIB.mak" CFG="KVILIB - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "KVILIB - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "KVILIB - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "KVILIB - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KVILIB_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "config" /I "tal" /I "system" /I "irc" /I "file" /I "ext" /I "net" /I "core\wstring" /I "core" /I "..\kernel" /I "$(QTDIR)\include" /I "c:\ssl" /I "c:\ipv6" /D "NDEBUG" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KVILIB_EXPORTS" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "__LIBKVILIB__" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "NDEBUG"
# ADD RSC /l 0x410 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 libeay32.lib SSLeay32.lib ws2_32.lib msvcrt.lib $(QTDIR)\lib\qt-mtnc321.lib $(QTDIR)\lib\qtmain.lib /nologo /dll /machine:I386 /nodefaultlib:"LIBCMT.lib" /out:"..\..\win32build\bin\kvilib.dll" /libpath:"C:\OpenSSL\lib\VC"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "KVILIB - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KVILIB_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "C:\src\kvilib\config" /I "C:\src\kvilib\tal" /I "C:\src\kvilib\system" /I "C:\src\kvilib\irc" /I "C:\src\kvilib\file" /I "C:\src\kvilib\ext" /I "C:\src\kvilib\net" /I "C:\src\kvilib\core\wstring" /I "C:\src\kvilib\core" /I "C:\src\kvirc\kernel" /I "_$(QTDIR)\include" /I "$(QTDIR)\include" /I "config" /I "tal" /I "system" /I "irc" /I "file" /I "ext" /I "net" /I "core\wstring" /I "core" /I "c:\ssl" /I "c:\ipv6" /I "..\kernel" /D "_DEBUG" /D "__LIBKVILIB__" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KVILIB_EXPORTS" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "NDEBUG"
# ADD RSC /l 0x410 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libeay32.lib SSLeay32.lib ws2_32.lib msvcrt.lib $(QTDIR)\lib\qt-mtnc321.lib $(QTDIR)\lib\qtmain.lib /nologo /dll /incremental:yes /debug /machine:I386 /nodefaultlib:"LIBCMT.lib" /out:"..\..\win32build\KviDebug\kvilib.dll" /pdbtype:sept /libpath:"C:\OpenSSL\lib\VC"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "KVILIB - Win32 Release"
# Name "KVILIB - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\irc\kvi_avatar.cpp
# End Source File
# Begin Source File

SOURCE=.\irc\kvi_avatarcache.cpp
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_cmdformatter.cpp
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_config.cpp
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_crypt.cpp
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_databuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_datacontainer.cpp
# End Source File
# Begin Source File

SOURCE=.\net\kvi_dns.cpp
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_doublebuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\system\kvi_env.cpp
# End Source File
# Begin Source File

SOURCE=.\core\kvi_error.cpp
# End Source File
# Begin Source File

SOURCE=.\file\kvi_file.cpp
# End Source File
# Begin Source File

SOURCE=.\file\kvi_fileutils.cpp
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_garbage.cpp
# End Source File
# Begin Source File

SOURCE=.\core\kvi_heapobject.cpp
# End Source File
# Begin Source File

SOURCE=.\net\kvi_http.cpp
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_imagelib.cpp
# End Source File
# Begin Source File

SOURCE=.\irc\kvi_ircmask.cpp
# End Source File
# Begin Source File

SOURCE=.\irc\kvi_ircserver.cpp
# End Source File
# Begin Source File

SOURCE=.\irc\kvi_ircserverdb.cpp
# End Source File
# Begin Source File

SOURCE=.\irc\kvi_ircuserdb.cpp
# End Source File
# Begin Source File

SOURCE=.\system\kvi_locale.cpp
# End Source File
# Begin Source File

SOURCE=.\core\kvi_malloc.cpp
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_md5.cpp
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_mediatype.cpp
# End Source File
# Begin Source File

SOURCE=.\core\kvi_memmove.cpp
# End Source File
# Begin Source File

SOURCE=.\irc\kvi_mirccntrl.cpp
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_msgtype.cpp
# End Source File
# Begin Source File

SOURCE=.\net\kvi_netutils.cpp
# End Source File
# Begin Source File

SOURCE=.\irc\kvi_nickserv.cpp
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_osinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_parameterlist.cpp
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_pixmap.cpp
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_proxydb.cpp
# End Source File
# Begin Source File

SOURCE=.\core\kvi_qstring.cpp
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_regchan.cpp
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_regusersdb.cpp
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_sharedfiles.cpp
# End Source File
# Begin Source File

SOURCE=.\net\kvi_socket.cpp
# End Source File
# Begin Source File

SOURCE=.\net\kvi_ssl.cpp
# End Source File
# Begin Source File

SOURCE=.\core\kvi_string.cpp
# End Source File
# Begin Source File

SOURCE=.\core\kvi_stringarray.cpp
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_stringconversion.cpp
# End Source File
# Begin Source File

SOURCE=.\tal\kvi_tal_application.cpp
# End Source File
# Begin Source File

SOURCE=.\tal\kvi_tal_filedialog.cpp
# End Source File
# Begin Source File

SOURCE=.\tal\kvi_tal_mainwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\tal\kvi_tal_menubar.cpp
# End Source File
# Begin Source File

SOURCE=.\tal\kvi_tal_toolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\system\kvi_thread.cpp
# End Source File
# Begin Source File

SOURCE=.\system\kvi_time.cpp
# End Source File
# Begin Source File

SOURCE=.\net\kvi_url.cpp
# End Source File
# Begin Source File

SOURCE=.\ext\moc_kvi_crypt.cpp
# ADD CPP /D "__KVILIB__"
# End Source File
# Begin Source File

SOURCE=.\net\moc_kvi_dns.cpp
# ADD CPP /D "__KVILIB__"
# End Source File
# Begin Source File

SOURCE=.\ext\moc_kvi_garbage.cpp
# ADD CPP /D "__KVILIB__"
# End Source File
# Begin Source File

SOURCE=.\net\moc_kvi_http.cpp
# ADD CPP /D "__KVILIB__"
# End Source File
# Begin Source File

SOURCE=.\system\moc_kvi_locale.cpp
# ADD CPP /D "__KVILIB__"
# End Source File
# Begin Source File

SOURCE=.\ext\moc_kvi_sharedfiles.cpp
# ADD CPP /D "__KVILIB__"
# End Source File
# Begin Source File

SOURCE=.\system\moc_kvi_thread.cpp

!IF  "$(CFG)" == "KVILIB - Win32 Release"

# ADD CPP /D "__KVILIB__"

!ELSEIF  "$(CFG)" == "KVILIB - Win32 Debug"

# ADD CPP /D "NDEBUG" /D "__KVILIB__"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\irc\kvi_avatar.h
# End Source File
# Begin Source File

SOURCE=.\irc\kvi_avatarcache.h
# End Source File
# Begin Source File

SOURCE=.\core\kvi_bswap.h
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_cmdformatter.h
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_config.h
# End Source File
# Begin Source File

SOURCE=.\config\kvi_confignames.h
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_crypt.h

!IF  "$(CFG)" == "KVILIB - Win32 Release"

# Begin Custom Build
InputDir=.\ext
InputPath=.\ext\kvi_crypt.h
InputName=kvi_crypt

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "KVILIB - Win32 Debug"

# Begin Custom Build
InputDir=.\ext
InputPath=.\ext\kvi_crypt.h
InputName=kvi_crypt

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ext\kvi_databuffer.h
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_datacontainer.h
# End Source File
# Begin Source File

SOURCE=.\net\kvi_dns.h

!IF  "$(CFG)" == "KVILIB - Win32 Release"

# Begin Custom Build
InputDir=.\net
InputPath=.\net\kvi_dns.h
InputName=kvi_dns

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "KVILIB - Win32 Debug"

# Begin Custom Build
InputDir=.\net
InputPath=.\net\kvi_dns.h
InputName=kvi_dns

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ext\kvi_doublebuffer.h
# End Source File
# Begin Source File

SOURCE=.\system\kvi_env.h
# End Source File
# Begin Source File

SOURCE=.\core\kvi_error.h
# End Source File
# Begin Source File

SOURCE=.\file\kvi_file.h
# End Source File
# Begin Source File

SOURCE=.\file\kvi_fileutils.h
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_garbage.h

!IF  "$(CFG)" == "KVILIB - Win32 Release"

# Begin Custom Build
InputDir=.\ext
InputPath=.\ext\kvi_garbage.h
InputName=kvi_garbage

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "KVILIB - Win32 Debug"

# Begin Custom Build
InputDir=.\ext
InputPath=.\ext\kvi_garbage.h
InputName=kvi_garbage

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\core\kvi_heapobject.h
# End Source File
# Begin Source File

SOURCE=.\net\kvi_http.h

!IF  "$(CFG)" == "KVILIB - Win32 Release"

# Begin Custom Build
InputDir=.\net
InputPath=.\net\kvi_http.h
InputName=kvi_http

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "KVILIB - Win32 Debug"

# Begin Custom Build
InputDir=.\net
InputPath=.\net\kvi_http.h
InputName=kvi_http

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ext\kvi_imagelib.h
# End Source File
# Begin Source File

SOURCE=.\core\kvi_inttypes.h
# End Source File
# Begin Source File

SOURCE=.\irc\kvi_ircmask.h
# End Source File
# Begin Source File

SOURCE=.\irc\kvi_ircserver.h
# End Source File
# Begin Source File

SOURCE=.\irc\kvi_ircserverdb.h
# End Source File
# Begin Source File

SOURCE=.\irc\kvi_ircuserdb.h
# End Source File
# Begin Source File

SOURCE=.\system\kvi_library.h
# End Source File
# Begin Source File

SOURCE=.\core\kvi_list.h
# End Source File
# Begin Source File

SOURCE=.\system\kvi_locale.h

!IF  "$(CFG)" == "KVILIB - Win32 Release"

# Begin Custom Build
InputDir=.\system
InputPath=.\system\kvi_locale.h
InputName=kvi_locale

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "KVILIB - Win32 Debug"

# Begin Custom Build
InputDir=.\system
InputPath=.\system\kvi_locale.h
InputName=kvi_locale

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\core\kvi_malloc.h
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_md5.h
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_mediatype.h
# End Source File
# Begin Source File

SOURCE=.\core\kvi_memmove.h
# End Source File
# Begin Source File

SOURCE=.\irc\kvi_mirccntrl.h
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_msgtype.h
# End Source File
# Begin Source File

SOURCE=.\net\kvi_netutils.h
# End Source File
# Begin Source File

SOURCE=.\irc\kvi_nickserv.h
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_osinfo.h
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_parameterlist.h
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_pixmap.h
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_proxydb.h
# End Source File
# Begin Source File

SOURCE=.\core\kvi_qstring.h
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_regchan.h
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_regusersdb.h
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_sharedfiles.h

!IF  "$(CFG)" == "KVILIB - Win32 Release"

# Begin Custom Build
InputDir=.\ext
InputPath=.\ext\kvi_sharedfiles.h
InputName=kvi_sharedfiles

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "KVILIB - Win32 Debug"

# Begin Custom Build
InputDir=.\ext
InputPath=.\ext\kvi_sharedfiles.h
InputName=kvi_sharedfiles

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\net\kvi_socket.h
# End Source File
# Begin Source File

SOURCE=.\net\kvi_sockettype.h
# End Source File
# Begin Source File

SOURCE=.\net\kvi_ssl.h
# End Source File
# Begin Source File

SOURCE=.\system\kvi_stdarg.h
# End Source File
# Begin Source File

SOURCE=.\core\kvi_strasm.h
# End Source File
# Begin Source File

SOURCE=.\core\kvi_string.h
# End Source File
# Begin Source File

SOURCE=.\core\kvi_stringarray.h
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_stringconversion.h
# End Source File
# Begin Source File

SOURCE=.\tal\kvi_tal_application.h

!IF  "$(CFG)" == "KVILIB - Win32 Release"

# Begin Custom Build
InputDir=.\tal
InputPath=.\tal\kvi_tal_application.h
InputName=kvi_tal_application

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "KVILIB - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\tal\kvi_tal_application_kde.h
# End Source File
# Begin Source File

SOURCE=.\tal\kvi_tal_application_qt.h

!IF  "$(CFG)" == "KVILIB - Win32 Release"

# Begin Custom Build
InputDir=.\tal
InputPath=.\tal\kvi_tal_application_qt.h
InputName=kvi_tal_application_qt

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "KVILIB - Win32 Debug"

# Begin Custom Build
InputDir=.\tal
InputPath=.\tal\kvi_tal_application_qt.h
InputName=kvi_tal_application_qt

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\tal\kvi_tal_filedialog.h
# End Source File
# Begin Source File

SOURCE=.\tal\kvi_tal_filedialog_kde.h
# End Source File
# Begin Source File

SOURCE=.\tal\kvi_tal_filedialog_qt.h

!IF  "$(CFG)" == "KVILIB - Win32 Release"

# Begin Custom Build
InputDir=.\tal
InputPath=.\tal\kvi_tal_filedialog_qt.h
InputName=kvi_tal_filedialog_qt

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "KVILIB - Win32 Debug"

# Begin Custom Build
InputDir=.\tal
InputPath=.\tal\kvi_tal_filedialog_qt.h
InputName=kvi_tal_filedialog_qt

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\tal\kvi_tal_mainwindow.h
# End Source File
# Begin Source File

SOURCE=.\tal\kvi_tal_mainwindow_kde.h
# End Source File
# Begin Source File

SOURCE=.\tal\kvi_tal_mainwindow_qt.h

!IF  "$(CFG)" == "KVILIB - Win32 Release"

# Begin Custom Build
InputDir=.\tal
InputPath=.\tal\kvi_tal_mainwindow_qt.h
InputName=kvi_tal_mainwindow_qt

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "KVILIB - Win32 Debug"

# Begin Custom Build
InputDir=.\tal
InputPath=.\tal\kvi_tal_mainwindow_qt.h
InputName=kvi_tal_mainwindow_qt

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\tal\kvi_tal_menubar.h
# End Source File
# Begin Source File

SOURCE=.\tal\kvi_tal_menubar_kde.h
# End Source File
# Begin Source File

SOURCE=.\tal\kvi_tal_menubar_qt.h

!IF  "$(CFG)" == "KVILIB - Win32 Release"

# Begin Custom Build
InputDir=.\tal
InputPath=.\tal\kvi_tal_menubar_qt.h
InputName=kvi_tal_menubar_qt

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "KVILIB - Win32 Debug"

# Begin Custom Build
InputDir=.\tal
InputPath=.\tal\kvi_tal_menubar_qt.h
InputName=kvi_tal_menubar_qt

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\tal\kvi_tal_toolbar.h
# End Source File
# Begin Source File

SOURCE=.\tal\kvi_tal_toolbar_kde.h
# End Source File
# Begin Source File

SOURCE=.\tal\kvi_tal_toolbar_qt.h

!IF  "$(CFG)" == "KVILIB - Win32 Release"

# Begin Custom Build
InputDir=.\tal
InputPath=.\tal\kvi_tal_toolbar_qt.h
InputName=kvi_tal_toolbar_qt

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "KVILIB - Win32 Debug"

# Begin Custom Build
InputDir=.\tal
InputPath=.\tal\kvi_tal_toolbar_qt.h
InputName=kvi_tal_toolbar_qt

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\tal\kvi_tal_toolbardocktype.h
# End Source File
# Begin Source File

SOURCE=.\system\kvi_thread.h

!IF  "$(CFG)" == "KVILIB - Win32 Release"

# Begin Custom Build
InputDir=.\system
InputPath=.\system\kvi_thread.h
InputName=kvi_thread

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "KVILIB - Win32 Debug"

# Begin Custom Build
InputDir=.\system
InputPath=.\system\kvi_thread.h
InputName=kvi_thread

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\system\kvi_time.h
# End Source File
# Begin Source File

SOURCE=.\net\kvi_url.h
# End Source File
# Begin Source File

SOURCE=.\config\kvi_wincfg.h
# End Source File
# Begin Source File

SOURCE=.\ext\kvi_xlib.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Generated"

# PROP Default_Filter ".moc"
# Begin Source File

SOURCE=.\tal\kvi_tal_application_qt.moc
# End Source File
# Begin Source File

SOURCE=.\tal\kvi_tal_filedialog_qt.moc
# End Source File
# Begin Source File

SOURCE=.\tal\kvi_tal_mainwindow_qt.moc
# End Source File
# Begin Source File

SOURCE=.\tal\kvi_tal_menubar_qt.moc
# End Source File
# Begin Source File

SOURCE=.\tal\kvi_tal_toolbar_qt.moc
# End Source File
# End Group
# End Target
# End Project
