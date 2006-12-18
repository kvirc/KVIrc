# Microsoft Developer Studio Project File - Name="kvifiletransferwindow" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=kvifiletransferwindow - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "kvifiletransferwindow.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "kvifiletransferwindow.mak" CFG="kvifiletransferwindow - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "kvifiletransferwindow - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "kvifiletransferwindow - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "kvifiletransferwindow - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FILETRANSFERWINDOW_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\modules\dcc" /I "_$(QTDIR)\include" /I "$(QTDIR)\include" /I "..\..\kvilib\tal" /I "..\..\kvilib\system" /I "..\..\kvilib\irc" /I "..\..\kvilib\file" /I "..\..\kvilib\ext" /I "..\..\kvilib\net" /I "..\..\kvilib\core\wstring" /I "..\..\kvilib\core" /I "..\..\kvilib\config" /I "..\..\kvirc\kvs" /I "..\..\kvirc\module" /I "..\..\kvirc\uparser\scripttoolbar" /I "..\..\kvirc\uparser" /I "..\..\kvirc\sparser" /I "..\..\kvirc\kernel" /I "..\..\kvirc\ui" /I "C:\ssl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FILETRANSFERWINDOW_EXPORTS" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 $(QTDIR)\lib\qt-mtnc321.lib $(QTDIR)\lib\qtmain.lib libeay32.lib SSLeay32.lib ws2_32.lib kvilib.lib kvirc.lib msvcrt.lib Shell32.lib /nologo /dll /machine:I386 /out:"..\..\..\win32build\bin\modules\kvifiletransferwindow.dll" /libpath:"C:\OpenSSL\lib\VC" /libpath:"..\..\kvilib\Release" /libpath:"..\..\kvirc\Release"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "kvifiletransferwindow - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FILETRANSFERWINDOW_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "_$(QTDIR)\include" /I "$(QTDIR)\include" /I "..\..\kvilib\tal" /I "..\..\kvilib\system" /I "..\..\kvilib\irc" /I "..\..\kvilib\file" /I "..\..\kvilib\ext" /I "..\..\kvilib\net" /I "..\..\kvilib\core\wstring" /I "..\..\kvilib\core" /I "..\..\kvilib\config" /I "..\..\kvirc\kvs" /I "..\..\kvirc\module" /I "..\..\kvirc\uparser\scripttoolbar" /I "..\..\kvirc\uparser" /I "..\..\kvirc\sparser" /I "..\..\kvirc\kernel" /I "..\..\kvirc\ui" /I "C:\ssl" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FILETRANSFERWINDOW_EXPORTS" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 $(QTDIR)\lib\qt-mtnc321.lib $(QTDIR)\lib\qtmain.lib libeay32.lib SSLeay32.lib ws2_32.lib kvilib.lib kvirc.lib msvcrt.lib Shell32.lib. /nologo /dll /debug /machine:I386 /nodefaultlib:"msvcrtd.lib" /out:"..\..\..\win32build\KviDebug\modules\kvifiletransferwindow.dll" /pdbtype:sept /libpath:"C:\OpenSSL\lib\VC" /libpath:"..\..\kvilib\Debug" /libpath:"..\..\kvirc\Debug"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "kvifiletransferwindow - Win32 Release"
# Name "kvifiletransferwindow - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\filetransferwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\libkvifiletransferwindow.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\filetransferwindow.h

!IF  "$(CFG)" == "kvifiletransferwindow - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\filetransferwindow.h
InputName=filetransferwindow

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvifiletransferwindow - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\filetransferwindow.h
InputName=filetransferwindow

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
