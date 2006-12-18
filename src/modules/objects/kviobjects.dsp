# Microsoft Developer Studio Project File - Name="kviobjects" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=kviobjects - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "kviobjects.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "kviobjects.mak" CFG="kviobjects - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "kviobjects - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "kviobjects - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "kviobjects - Win32 Release"

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
# ADD LINK32 $(QTDIR)\lib\qt-mtnc321.lib $(QTDIR)\lib\qtmain.lib libeay32.lib SSLeay32.lib ws2_32.lib kvilib.lib kvirc.lib msvcrt.lib /nologo /dll /machine:I386 /out:"..\..\..\win32build\bin\modules\kviobjects.dll" /libpath:"C:\OpenSSL\lib\VC" /libpath:"..\..\kvilib\Release" /libpath:"..\..\kvirc\Release"

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

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
# ADD LINK32 $(QTDIR)\lib\qt-mtnc321.lib $(QTDIR)\lib\qtmain.lib libeay32.lib SSLeay32.lib ws2_32.lib kvilib.lib kvirc.lib msvcrt.lib /nologo /dll /debug /machine:I386 /nodefaultlib:"msvcrtd.lib" /out:"..\..\..\win32build\KviDebug\modules\kviobjects.dll" /pdbtype:sept /libpath:"C:\OpenSSL\lib\VC" /libpath:"..\..\kvilib\Debug" /libpath:"..\..\kvirc\Debug"

!ENDIF 

# Begin Target

# Name "kviobjects - Win32 Release"
# Name "kviobjects - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\class_button.cpp
# End Source File
# Begin Source File

SOURCE=.\class_checkbox.cpp
# End Source File
# Begin Source File

SOURCE=.\class_combobox.cpp
# End Source File
# Begin Source File

SOURCE=.\class_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\class_dockwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\class_file.cpp
# End Source File
# Begin Source File

SOURCE=.\class_groupbox.cpp
# End Source File
# Begin Source File

SOURCE=.\class_hbox.cpp
# End Source File
# Begin Source File

SOURCE=.\class_image.cpp
# End Source File
# Begin Source File

SOURCE=.\class_label.cpp
# End Source File
# Begin Source File

SOURCE=.\class_layout.cpp
# End Source File
# Begin Source File

SOURCE=.\class_lcd.cpp
# End Source File
# Begin Source File

SOURCE=.\class_lineedit.cpp
# End Source File
# Begin Source File

SOURCE=.\class_list.cpp
# End Source File
# Begin Source File

SOURCE=.\class_listbox.cpp
# End Source File
# Begin Source File

SOURCE=.\class_listview.cpp
# End Source File
# Begin Source File

SOURCE=.\class_listviewitem.cpp
# End Source File
# Begin Source File

SOURCE=.\class_mainwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\class_menubar.cpp
# End Source File
# Begin Source File

SOURCE=.\class_multilineedit.cpp
# End Source File
# Begin Source File

SOURCE=.\class_painter.cpp
# End Source File
# Begin Source File

SOURCE=.\class_pixmap.cpp
# End Source File
# Begin Source File

SOURCE=.\class_popupmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\class_process.cpp
# End Source File
# Begin Source File

SOURCE=.\class_progressbar.cpp
# End Source File
# Begin Source File

SOURCE=.\class_radiobutton.cpp
# End Source File
# Begin Source File

SOURCE=.\class_slider.cpp
# End Source File
# Begin Source File

SOURCE=.\class_socket.cpp
# End Source File
# Begin Source File

SOURCE=.\class_spinbox.cpp
# End Source File
# Begin Source File

SOURCE=.\class_tabwidget.cpp
# End Source File
# Begin Source File

SOURCE=.\class_tbrowser.cpp
# End Source File
# Begin Source File

SOURCE=.\class_toolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\class_toolbutton.cpp
# End Source File
# Begin Source File

SOURCE=.\class_urllabel.cpp
# End Source File
# Begin Source File

SOURCE=.\class_vbox.cpp
# End Source File
# Begin Source File

SOURCE=.\class_widget.cpp
# End Source File
# Begin Source File

SOURCE=.\class_window.cpp
# End Source File
# Begin Source File

SOURCE=.\class_wizard.cpp
# End Source File
# Begin Source File

SOURCE=.\class_workspace.cpp
# End Source File
# Begin Source File

SOURCE=.\class_wrapper.cpp
# End Source File
# Begin Source File

SOURCE=.\class_xmlreader.cpp
# End Source File
# Begin Source File

SOURCE=.\libkviobjects.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\class_button.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\class_button.h
InputName=class_button

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_button.h
InputName=class_button

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\class_checkbox.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\class_checkbox.h
InputName=class_checkbox

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_checkbox.h
InputName=class_checkbox

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\class_combobox.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\class_combobox.h
InputName=class_combobox

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_combobox.h
InputName=class_combobox

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\class_dialog.h
# End Source File
# Begin Source File

SOURCE=.\class_dockwindow.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_dockwindow.h
InputName=class_dockwindow

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\class_file.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\class_file.h
InputName=class_file

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_file.h
InputName=class_file

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\class_groupbox.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_groupbox.h
InputName=class_groupbox

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\class_hbox.h
# End Source File
# Begin Source File

SOURCE=.\class_image.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_image.h
InputName=class_image

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\class_label.h
# End Source File
# Begin Source File

SOURCE=.\class_layout.h
# End Source File
# Begin Source File

SOURCE=.\class_lcd.h
# End Source File
# Begin Source File

SOURCE=.\class_lineedit.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\class_lineedit.h
InputName=class_lineedit

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_lineedit.h
InputName=class_lineedit

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\class_list.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_list.h
InputName=class_list

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\class_listbox.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\class_listbox.h
InputName=class_listbox

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_listbox.h
InputName=class_listbox

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\class_listview.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\class_listview.h
InputName=class_listview

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_listview.h
InputName=class_listview

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\class_listviewitem.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\class_listviewitem.h
InputName=class_listviewitem

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_listviewitem.h
InputName=class_listviewitem

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\class_mainwindow.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_mainwindow.h
InputName=class_mainwindow

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\class_menubar.h
# End Source File
# Begin Source File

SOURCE=.\class_multilineedit.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\class_multilineedit.h
InputName=class_multilineedit

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_multilineedit.h
InputName=class_multilineedit

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\class_painter.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\class_painter.h
InputName=class_painter

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_painter.h
InputName=class_painter

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\class_pixmap.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\class_pixmap.h
InputName=class_pixmap

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_pixmap.h
InputName=class_pixmap

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\class_popupmenu.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\class_popupmenu.h
InputName=class_popupmenu

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_popupmenu.h
InputName=class_popupmenu

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\class_process.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\class_process.h
InputName=class_process

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_process.h
InputName=class_process

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\class_progressbar.h
# End Source File
# Begin Source File

SOURCE=.\class_radiobutton.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\class_radiobutton.h
InputName=class_radiobutton

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_radiobutton.h
InputName=class_radiobutton

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\class_slider.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\class_slider.h
InputName=class_slider

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_slider.h
InputName=class_slider

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\class_socket.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\class_socket.h
InputName=class_socket

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_socket.h
InputName=class_socket

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\class_spinbox.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\class_spinbox.h
InputName=class_spinbox

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_spinbox.h
InputName=class_spinbox

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\class_tabwidget.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\class_tabwidget.h
InputName=class_tabwidget

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_tabwidget.h
InputName=class_tabwidget

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\class_tbrowser.h
# End Source File
# Begin Source File

SOURCE=.\class_toolbar.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\class_toolbar.h
InputName=class_toolbar

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_toolbar.h
InputName=class_toolbar

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\class_toolbutton.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\class_toolbutton.h
InputName=class_toolbutton

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_toolbutton.h
InputName=class_toolbutton

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\class_urllabel.h
# End Source File
# Begin Source File

SOURCE=.\class_vbox.h
# End Source File
# Begin Source File

SOURCE=.\class_widget.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\class_widget.h
InputName=class_widget

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_widget.h
InputName=class_widget

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\class_window.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\class_window.h
InputName=class_window

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_window.h
InputName=class_window

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\class_wizard.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\class_wizard.h
InputName=class_wizard

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_wizard.h
InputName=class_wizard

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\class_workspace.h
# End Source File
# Begin Source File

SOURCE=.\class_wrapper.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\class_wrapper.h
InputName=class_wrapper

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_wrapper.h
InputName=class_wrapper

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\class_xmlreader.h

!IF  "$(CFG)" == "kviobjects - Win32 Release"

# Begin Custom Build
InputDir=.
InputPath=.\class_xmlreader.h
InputName=class_xmlreader

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kviobjects - Win32 Debug"

# Begin Custom Build
InputDir=.
InputPath=.\class_xmlreader.h
InputName=class_xmlreader

"$(InputDir)\m_$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\m_$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\object_macros.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "generated"

# PROP Default_Filter ".moc"
# End Group
# End Target
# End Project
