# Microsoft Developer Studio Project File - Name="kvioptions" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=kvioptions - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "kvioptions.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "kvioptions.mak" CFG="kvioptions - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "kvioptions - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "kvioptions - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# PROP BASE Use_MFC 1
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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OBJECTS_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(QTDIR)\include" /I "..\..\kvilib\tal" /I "..\..\kvilib\system" /I "..\..\kvilib\irc" /I "..\..\kvilib\file" /I "..\..\kvilib\ext" /I "..\..\kvilib\net" /I "..\..\kvilib\core\wstring" /I "..\..\kvilib\core" /I "..\..\kvilib\config" /I "..\..\kvirc\kvs" /I "..\..\kvirc\module" /I "..\..\kvirc\uparser\scripttoolbar" /I "..\..\kvirc\uparser" /I "..\..\kvirc\sparser" /I "..\..\kvirc\kernel" /I "..\..\kvirc\ui" /I "C:\ssl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OBJECTS_EXPORTS" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 $(QTDIR)\lib\qt-mtnc321.lib $(QTDIR)\lib\qtmain.lib libeay32.lib SSLeay32.lib ws2_32.lib kvilib.lib kvirc.lib msvcrt.lib /nologo /dll /machine:I386 /out:"..\..\..\win32build\bin\modules\kvioptions.dll" /libpath:"C:\OpenSSL\lib\VC" /libpath:"..\..\kvilib\Release" /libpath:"..\..\kvirc\Release"

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OBJECTS_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "_$(QTDIR)\include" /I "$(QTDIR)\include" /I "..\..\kvilib\tal" /I "..\..\kvilib\system" /I "..\..\kvilib\irc" /I "..\..\kvilib\file" /I "..\..\kvilib\ext" /I "..\..\kvilib\net" /I "..\..\kvilib\core\wstring" /I "..\..\kvilib\core" /I "..\..\kvilib\config" /I "..\..\kvirc\kvs" /I "..\..\kvirc\module" /I "..\..\kvirc\uparser\scripttoolbar" /I "..\..\kvirc\uparser" /I "..\..\kvirc\sparser" /I "..\..\kvirc\kernel" /I "..\..\kvirc\ui" /I "C:\ssl" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OBJECTS_EXPORTS" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 $(QTDIR)\lib\qt-mtnc321.lib $(QTDIR)\lib\qtmain.lib libeay32.lib SSLeay32.lib ws2_32.lib kvilib.lib kvirc.lib msvcrt.lib /nologo /dll /debug /machine:I386 /nodefaultlib:"msvcrtd.lib" /out:"..\..\..\win32build\KviDebug\modules\kvioptions.dll" /pdbtype:sept /libpath:"C:\OpenSSL\lib\VC" /libpath:"..\..\kvilib\Debug" /libpath:"..\..\kvirc\Debug"

!ENDIF 

# Begin Target

# Name "kvioptions - Win32 Release"
# Name "kvioptions - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\container.cpp
# End Source File
# Begin Source File

SOURCE=.\dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\instances.cpp
# End Source File
# Begin Source File

SOURCE=.\libkvioptions.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_antispam.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_avatar.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_away.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_connection.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_ctcp.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_ctcpfloodprotection.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_dcc.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_generalopt.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_highlighting.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_ident.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_identity.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_ignore.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_input.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_interfacefeatures.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_interfacelookglobal.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_irc.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_ircoutput.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_ircview.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_lag.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_logging.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_mediatypes.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_messages.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_nickserv.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_notify.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_privmsg.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_protection.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_proxy.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_query.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_servers.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_sound.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_ssl.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_taskbar.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_textencoding.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_texticons.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_theme.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_tools.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_topiclabel.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_transport.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_uparser.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_urlhandlers.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_userlevel.cpp
# End Source File
# Begin Source File

SOURCE=.\optw_userlist.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\container.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\container.h
InputName=container

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\container.h
InputName=container

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dialog.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\dialog.h
InputName=dialog

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\instances.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\instances.h
InputName=instances

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\instances.h
InputName=instances

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_antispam.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_antispam.h
InputName=optw_antispam

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_antispam.h
InputName=optw_antispam

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_avatar.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_avatar.h
InputName=optw_avatar

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_away.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_away.h
InputName=optw_away

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_away.h
InputName=optw_away

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_connection.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_connection.h
InputName=optw_connection

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_connection.h
InputName=optw_connection

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_ctcp.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_ctcp.h
InputName=optw_ctcp

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_ctcp.h
InputName=optw_ctcp

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_ctcpfloodprotection.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_ctcpfloodprotection.h
InputName=optw_ctcpfloodprotection

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_ctcpfloodprotection.h
InputName=optw_ctcpfloodprotection

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_dcc.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_dcc.h
InputName=optw_dcc

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_dcc.h
InputName=optw_dcc

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_generalopt.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_generalopt.h
InputName=optw_generalopt

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_highlighting.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_highlighting.h
InputName=optw_highlighting

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_highlighting.h
InputName=optw_highlighting

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_ident.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_ident.h
InputName=optw_ident

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_ident.h
InputName=optw_ident

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_identity.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_identity.h
InputName=optw_identity

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_identity.h
InputName=optw_identity

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_ignore.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_ignore.h
InputName=optw_ignore

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_ignore.h
InputName=optw_ignore

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_input.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_input.h
InputName=optw_input

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_input.h
InputName=optw_input

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_interfacefeatures.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_interfacefeatures.h
InputName=optw_interfacefeatures

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_interfacefeatures.h
InputName=optw_interfacefeatures

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_interfacelookglobal.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_interfacelookglobal.h
InputName=optw_interfacelookglobal

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_interfacelookglobal.h
InputName=optw_interfacelookglobal

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_irc.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_irc.h
InputName=optw_irc

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_irc.h
InputName=optw_irc

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_ircoutput.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_ircoutput.h
InputName=optw_ircoutput

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_ircoutput.h
InputName=optw_ircoutput

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_ircview.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_ircview.h
InputName=optw_ircview

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_ircview.h
InputName=optw_ircview

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_lag.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_lag.h
InputName=optw_lag

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_lag.h
InputName=optw_lag

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_logging.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_logging.h
InputName=optw_logging

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_logging.h
InputName=optw_logging

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_mediatypes.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_mediatypes.h
InputName=optw_mediatypes

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_mediatypes.h
InputName=optw_mediatypes

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_messages.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_messages.h
InputName=optw_messages

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_messages.h
InputName=optw_messages

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_nickserv.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_nickserv.h
InputName=optw_nickserv

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_nickserv.h
InputName=optw_nickserv

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_notify.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_notify.h
InputName=optw_notify

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_notify.h
InputName=optw_notify

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_privmsg.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_privmsg.h
InputName=optw_privmsg

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_privmsg.h
InputName=optw_privmsg

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_protection.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_protection.h
InputName=optw_protection

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_proxy.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_proxy.h
InputName=optw_proxy

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_proxy.h
InputName=optw_proxy

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_query.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_query.h
InputName=optw_query

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_query.h
InputName=optw_query

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_servers.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_servers.h
InputName=optw_servers

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_servers.h
InputName=optw_servers

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_sound.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_sound.h
InputName=optw_sound

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_sound.h
InputName=optw_sound

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_ssl.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_ssl.h
InputName=optw_ssl

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_ssl.h
InputName=optw_ssl

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_taskbar.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_taskbar.h
InputName=optw_taskbar

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_taskbar.h
InputName=optw_taskbar

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_textencoding.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_textencoding.h
InputName=optw_textencoding

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_texticons.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_texticons.h
InputName=optw_texticons

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_texticons.h
InputName=optw_texticons

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_theme.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_theme.h
InputName=optw_theme

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_theme.h
InputName=optw_theme

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_tools.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_tools.h
InputName=optw_tools

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_topiclabel.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_topiclabel.h
InputName=optw_topiclabel

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_topiclabel.h
InputName=optw_topiclabel

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_transport.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_transport.h
InputName=optw_transport

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_transport.h
InputName=optw_transport

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_uparser.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_uparser.h
InputName=optw_uparser

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_uparser.h
InputName=optw_uparser

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_urlhandlers.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_urlhandlers.h
InputName=optw_urlhandlers

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_urlhandlers.h
InputName=optw_urlhandlers

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_userlevel.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_userlevel.h
InputName=optw_userlevel

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optw_userlist.h

!IF  "$(CFG)" == "kvioptions - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\optw_userlist.h
InputName=optw_userlist

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvioptions - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\optw_userlist.h
InputName=optw_userlist

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
