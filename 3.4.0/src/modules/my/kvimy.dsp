# Microsoft Developer Studio Project File - Name="kvimy" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=kvimy - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "kvimy.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "kvimy.mak" CFG="kvimy - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "kvimy - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "kvimy - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "kvimy - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ACTION_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "_$(QTDIR)\include" /I "$(QTDIR)\include" /I "..\..\kvilib\tal" /I "..\..\kvilib\system" /I "..\..\kvilib\irc" /I "..\..\kvilib\file" /I "..\..\kvilib\ext" /I "..\..\kvilib\net" /I "..\..\kvilib\core\wstring" /I "..\..\kvilib\core" /I "..\..\kvilib\config" /I "..\..\kvirc\kvs" /I "..\..\kvirc\module" /I "..\..\kvirc\uparser\scripttoolbar" /I "..\..\kvirc\uparser" /I "..\..\kvirc\sparser" /I "..\..\kvirc\kernel" /I "..\..\kvirc\ui" /I "C:\ssl" /D "COMPILE_ON_WINDOWS" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ACTION_EXPORTS" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /YX /FD /c
# SUBTRACT CPP /Z<none>
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 idleui.lib imm32.lib wsock32.lib winmm.lib $(QTDIR)\lib\qt-mtnc321.lib $(QTDIR)\lib\qtmain.lib libeay32.lib SSLeay32.lib ws2_32.lib kvilib.lib kvirc.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /dll /machine:I386 /out:"..\..\..\win32build\bin\modules\kvimy.dll" /libpath:"win32\Release" /libpath:"C:\OpenSSL\lib\VC" /libpath:"..\..\kvilib\Release" /libpath:"..\..\kvirc\Release"

!ELSEIF  "$(CFG)" == "kvimy - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ACTION_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /GX /Od /I "_$(QTDIR)\include" /I "$(QTDIR)\include" /I "..\..\kvilib\tal" /I "..\..\kvilib\system" /I "..\..\kvilib\irc" /I "..\..\kvilib\file" /I "..\..\kvilib\ext" /I "..\..\kvilib\net" /I "..\..\kvilib\core\wstring" /I "..\..\kvilib\core" /I "..\..\kvilib\config" /I "..\..\kvirc\kvs" /I "..\..\kvirc\module" /I "..\..\kvirc\uparser\scripttoolbar" /I "..\..\kvirc\uparser" /I "..\..\kvirc\sparser" /I "..\..\kvirc\kernel" /I "..\..\kvirc\ui" /I "C:\ssl" /D "COMPILE_ON_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ACTION_EXPORTS" /YX /FD /GZ /c
# SUBTRACT CPP /Z<none>
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 idleui.lib odbc32.lib odbccp32.lib $(QTDIR)\lib\qt-mtnc321.lib $(QTDIR)\lib\qtmain.lib libeay32.lib SSLeay32.lib ws2_32.lib kvilib.lib kvirc.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /dll /debug /machine:I386 /out:"..\..\..\win32build\KviDebug\modules\kvimy.dll" /pdbtype:sept /libpath:"win32\Debug C:\OpenSSL\lib\VC" /libpath:"..\..\kvilib\Debug" /libpath:"..\..\kvirc\Debug"

!ENDIF 

# Begin Target

# Name "kvimy - Win32 Release"
# Name "kvimy - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\idle.cpp
# ADD CPP /D "COMPILE_ON_WINDOWS"
# End Source File
# Begin Source File

SOURCE=.\idle_win.cpp
# ADD CPP /D "COMPILE_ON_WINDOWS"
# End Source File
# Begin Source File

SOURCE=.\libkvimy.cpp

!IF  "$(CFG)" == "kvimy - Win32 Release"

# ADD CPP /I "win32"

!ELSEIF  "$(CFG)" == "kvimy - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\idle.h

!IF  "$(CFG)" == "kvimy - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\idle.h
InputName=idle

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvimy - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
