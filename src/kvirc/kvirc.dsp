#
# Microsoft Developer Studio Project File - Name="kvirc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=kvirc - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "kvirc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "kvirc.mak" CFG="kvirc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "kvirc - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "kvirc - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "kvirc - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\kvilib\tal" /I "..\kvilib\system" /I "..\kvilib\irc" /I "..\kvilib\file" /I "..\kvilib\ext" /I "..\kvilib\net" /I "..\kvilib\core\wstring" /I "..\kvilib\core" /I "..\kvilib\config" /I "kvs" /I "module" /I "uparser\scripttoolbar" /I "uparser" /I "sparser" /I "kernel" /I "ui" /I "$(QTDIR)\include" /I "C:\ssl" /I "C:\zlib\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "NDEBUG"
# ADD RSC /l 0x410 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 libeay32.lib SSLeay32.lib ws2_32.lib KVILIB.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imm32.lib wsock32.lib winspool.lib winmm.lib $(QTDIR)\lib\qt-mtnc321.lib $(QTDIR)\lib\qtmain.lib C:\zlib\lib\zdll.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"msvcrt.lib" /libpath:"..\kvilib\Release" /libpath:"C:\OpenSSL\lib\VC"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
OutDir=.\Release
InputPath=.\Release\kvirc.exe
SOURCE="$(InputPath)"

"..\..\win32build\bin\kvirc.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(OutDir)\kvirc.exe ..\..\win32build\bin\kvirc.exe

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /vmg /GX /Zi /Od /I "..\kvilib\tal" /I "..\kvilib\system" /I "..\kvilib\irc" /I "..\kvilib\file" /I "..\kvilib\ext" /I "..\kvilib\net" /I "..\kvilib\core\wstring" /I "..\kvilib\core" /I "..\kvilib\config" /I "kvs" /I "module" /I "uparser\scripttoolbar" /I "uparser" /I "sparser" /I "kernel" /I "ui" /I "$(QTDIR)\include" /I "C:\ssl" /I "C:\zlib\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "NDEBUG"
# ADD RSC /l 0x410 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libeay32.lib SSLeay32.lib ws2_32.lib KVILIB.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib imm32.lib wsock32.lib winspool.lib winmm.lib $(QTDIR)\lib\qt-mtnc321.lib $(QTDIR)\lib\qtmain.lib C:\zlib\lib\zdll.lib /nologo /subsystem:windows /incremental:yes /debug /machine:I386 /nodefaultlib:"msvcrt.lib" /out:".\Debug\kvirc.exe" /pdbtype:sept /libpath:"..\kvilib\Debug" /libpath:"C:\OpenSSL\lib\VC"
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
OutDir=.\Debug
InputPath=.\Debug\kvirc.exe
SOURCE="$(InputPath)"

"..\..\win32build\KviDebug\kvirc.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(OutDir)\kvirc.exe ..\..\win32build\KviDebug\kvirc.exe

# End Custom Build

!ENDIF 

# Begin Target

# Name "kvirc - Win32 Release"
# Name "kvirc - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\kernel\kvi_action.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_actiondrawer.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_actionmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\sparser\kvi_antispam.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_app.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_app_fs.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_app_setup.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_asynchronousconnectiondata.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_channel.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_colorwin.cpp
# End Source File
# Begin Source File

SOURCE=.\uparser\kvi_command.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_console.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_coreactions.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_cryptcontroller.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_ctcppagedialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_customtoolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_customtoolbardescriptor.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_customtoolbarmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_debugwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_debugwindow.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_dynamictooltip.cpp
# End Source File
# Begin Source File

SOURCE=.\uparser\kvi_exprtree.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_filedialog.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_filetransfer.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_frame.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_historywin.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_iconmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_imagedialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_input.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_internalcmd.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_ipc.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_ipeditor.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_ircconnection.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_ircconnectionantictcpflooddata.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_ircconnectionasyncwhoisdata.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_ircconnectionnetsplitdetectordata.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_ircconnectionserverinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_ircconnectionstatedata.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_ircconnectionstatistics.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_ircconnectiontarget.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_ircconnectiontargetresolver.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_ircconnectionuserinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_irccontext.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_ircdatastreammonitor.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_irclink.cpp
# End Source File
# Begin Source File

SOURCE=.\sparser\kvi_ircmessage.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_ircsocket.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_irctoolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\uparser\kvi_ircurl.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_ircview.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_ircviewtools.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_action.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_aliasmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_array.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_arraycast.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_asyncdnsoperation.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_asyncoperation.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_callbackobject.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_corecallbackcommands.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_corefunctions.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_corefunctions_af.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_corefunctions_gl.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_corefunctions_mr.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_corefunctions_sz.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_coresimplecommands.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_coresimplecommands_af.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_coresimplecommands_gl.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_coresimplecommands_mr.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_coresimplecommands_sz.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_dnsmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_event.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_eventhandler.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_eventmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_eventtable.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_hash.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_kernel.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_moduleinterface.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_object.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_object_class.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_object_controller.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_object_functioncall.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_object_functionhandler.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_object_functionhandlerimpl.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_parameterprocessor.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_parser.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_parser_command.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_parser_comment.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_parser_dollar.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_parser_expression.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_parser_lside.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_parser_specialcommands.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_popupmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_popupmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_processmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_report.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_runtimecall.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_runtimecontext.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_rwevaluationresult.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_script.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_scriptaddonmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_switchlist.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_timermanager.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_aliasfunctioncall.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_aliassimplecommand.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_arraycount.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_arrayelement.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_arrayorhashelement.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_arrayreferenceassert.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_base.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_baseobjectfunctioncall.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_callbackcommand.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_command.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_commandevaluation.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_commandwithparameters.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_compositedata.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_constantdata.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_corecallbackcommand.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_corefunctioncall.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_coresimplecommand.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_data.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_datalist.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_expression.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_expressionreturn.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_extendedscopevariable.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_functioncall.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_globalvariable.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_hashcount.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_hashelement.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_hashreferenceassert.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_indirectdata.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_instruction.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_instructionblock.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_localvariable.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_modulecallbackcommand.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_modulefunctioncall.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_modulesimplecommand.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_multipleparameteridentifier.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_objectfield.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_objectfunctioncall.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_operation.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_parameterreturn.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_rebindingswitch.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_scopeoperator.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_simplecommand.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_singleparameteridentifier.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_specialcommand.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_specialcommandbreak.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_specialcommandclass.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_specialcommanddefpopup.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_specialcommanddo.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_specialcommandfor.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_specialcommandforeach.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_specialcommandif.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_specialcommandswitch.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_specialcommandunset.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_specialcommandwhile.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_switchlist.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_thisobjectfunctioncall.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_variable.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_voidfunctioncall.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_useraction.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_variant.cpp
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_variantlist.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_lagmeter.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_listview.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_main.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_maskeditor.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_mdicaption.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_mdichild.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_mdimanager.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_menubar.cpp
# End Source File
# Begin Source File

SOURCE=.\module\kvi_mexlinkfilter.cpp
# End Source File
# Begin Source File

SOURCE=.\module\kvi_mexserverimport.cpp
# End Source File
# Begin Source File

SOURCE=.\module\kvi_mextoolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_modeeditor.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_modew.cpp
# End Source File
# Begin Source File

SOURCE=.\module\kvi_module.cpp
# End Source File
# Begin Source File

SOURCE=.\module\kvi_moduleextension.cpp
# End Source File
# Begin Source File

SOURCE=.\module\kvi_modulemanager.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_msgbox.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_notifylist.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_options.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_optionswidget.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_query.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_scriptbutton.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_scripteditor.cpp
# End Source File
# Begin Source File

SOURCE=.\uparser\kvi_scriptobject.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_selectors.cpp
# End Source File
# Begin Source File

SOURCE=.\sparser\kvi_sp_ctcp.cpp
# End Source File
# Begin Source File

SOURCE=.\sparser\kvi_sp_literal.cpp
# End Source File
# Begin Source File

SOURCE=.\sparser\kvi_sp_numeric.cpp
# End Source File
# Begin Source File

SOURCE=.\sparser\kvi_sp_tables.cpp
# End Source File
# Begin Source File

SOURCE=.\sparser\kvi_sparser.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_splash.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_sslmaster.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_statusbar.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_statusbarapplet.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_styled_controls.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_taskbar.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_texticonmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_texticonwin.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_themedlabel.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_toolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_toolwindows_container.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_topicw.cpp
# End Source File
# Begin Source File

SOURCE=.\uparser\kvi_up_cmd.cpp
# End Source File
# Begin Source File

SOURCE=.\uparser\kvi_up_expr.cpp
# End Source File
# Begin Source File

SOURCE=.\uparser\kvi_up_fnc.cpp
# End Source File
# Begin Source File

SOURCE=.\uparser\kvi_up_oper.cpp
# End Source File
# Begin Source File

SOURCE=.\uparser\kvi_up_skip.cpp
# End Source File
# Begin Source File

SOURCE=.\uparser\kvi_uparser.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_useraction.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_userinput.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_userlistview.cpp
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_window.cpp
# End Source File
# Begin Source File

SOURCE=.\kernel\moc_kvi_action.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\ui\moc_kvi_actiondrawer.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\kernel\moc_kvi_actionmanager.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\kernel\moc_kvi_coreactions.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\ui\moc_kvi_customtoolbar.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\ui\moc_kvi_debugwindow.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\kernel\moc_kvi_filetransfer.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\ui\moc_kvi_imagedialog.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\kernel\moc_kvi_ircconnection.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\kernel\moc_kvi_ircconnectiontargetresolver.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\kernel\moc_kvi_irccontext.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\kernel\moc_kvi_irclink.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\kernel\moc_kvi_ircsocket.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\ui\moc_kvi_ircviewtools.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\kvs\moc_kvi_kvs_action.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\kvs\moc_kvi_kvs_asyncdnsoperation.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\kvs\moc_kvi_kvs_asyncoperation.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\kvs\moc_kvi_kvs_dnsmanager.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\kvs\moc_kvi_kvs_object.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\kvs\moc_kvi_kvs_popupmenu.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\kvs\moc_kvi_kvs_processmanager.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\kvs\moc_kvi_kvs_timermanager.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\kvs\moc_kvi_kvs_treenode_specialcommandswitch.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\kvs\moc_kvi_kvs_treenode_specialcommandunset.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\kvs\moc_kvi_kvs_useraction.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\ui\moc_kvi_listview.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\ui\moc_kvi_mdicaption.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\ui\moc_kvi_mdichild.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\ui\moc_kvi_mdimanager.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\module\moc_kvi_mexlinkfilter.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\module\moc_kvi_mexserverimport.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\module\moc_kvi_mextoolbar.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\module\moc_kvi_modulemanager.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\ui\moc_kvi_statusbar.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\ui\moc_kvi_statusbarapplet.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# Begin Source File

SOURCE=.\ui\moc_kvi_toolwindows_container.cpp
# ADD CPP /D "__KVIRC__"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\kernel\kvi_action.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_action.h
InputName=kvi_action

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_action.h
InputName=kvi_action

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_actiondrawer.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_actiondrawer.h
InputName=kvi_actiondrawer

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_actiondrawer.h
InputName=kvi_actiondrawer

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_actionmanager.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_actionmanager.h
InputName=kvi_actionmanager

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_actionmanager.h
InputName=kvi_actionmanager

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sparser\kvi_antispam.h
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_app.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_app.h
InputName=kvi_app

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_app.h
InputName=kvi_app

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_asynchronousconnectiondata.h
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_channel.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_channel.h
InputName=kvi_channel

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_channel.h
InputName=kvi_channel

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_colorwin.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_colorwin.h
InputName=kvi_colorwin

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_colorwin.h
InputName=kvi_colorwin

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\uparser\kvi_command.h
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_console.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_console.h
InputName=kvi_console

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_console.h
InputName=kvi_console

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_coreactionnames.h
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_coreactions.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_coreactions.h
InputName=kvi_coreactions

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_coreactions.h
InputName=kvi_coreactions

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_cryptcontroller.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_cryptcontroller.h
InputName=kvi_cryptcontroller

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_cryptcontroller.h
InputName=kvi_cryptcontroller

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_ctcppagedialog.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_ctcppagedialog.h
InputName=kvi_ctcppagedialog

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_ctcppagedialog.h
InputName=kvi_ctcppagedialog

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_customtoolbar.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_customtoolbar.h
InputName=kvi_customtoolbar

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_customtoolbar.h
InputName=kvi_customtoolbar

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_customtoolbardescriptor.h
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_customtoolbarmanager.h
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_debugwindow.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_debugwindow.h
InputName=kvi_debugwindow

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_debugwindow.h
InputName=kvi_debugwindow

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_dynamictooltip.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_dynamictooltip.h
InputName=kvi_dynamictooltip

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_dynamictooltip.h
InputName=kvi_dynamictooltip

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\uparser\kvi_eventhandler.h
# End Source File
# Begin Source File

SOURCE=.\uparser\kvi_exprtree.h
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_filedialog.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_filedialog.h
InputName=kvi_filedialog

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_filedialog.h
InputName=kvi_filedialog

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_filetransfer.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_filetransfer.h
InputName=kvi_filetransfer

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_filetransfer.h
InputName=kvi_filetransfer

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_frame.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_frame.h
InputName=kvi_frame

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_frame.h
InputName=kvi_frame

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_historywin.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_historywin.h
InputName=kvi_historywin

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_historywin.h
InputName=kvi_historywin

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_iconmanager.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_iconmanager.h
InputName=kvi_iconmanager

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_iconmanager.h
InputName=kvi_iconmanager

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_imagedialog.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_imagedialog.h
InputName=kvi_imagedialog

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_imagedialog.h
InputName=kvi_imagedialog

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_input.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_input.h
InputName=kvi_input

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_input.h
InputName=kvi_input

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_internalcmd.h
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_ipc.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_ipc.h
InputName=kvi_ipc

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_ipc.h
InputName=kvi_ipc

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_ipeditor.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_ipeditor.h
InputName=kvi_ipeditor

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_ipeditor.h
InputName=kvi_ipeditor

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_ircconnection.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_ircconnection.h
InputName=kvi_ircconnection

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_ircconnection.h
InputName=kvi_ircconnection

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_ircconnectionantictcpflooddata.h
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_ircconnectionasyncwhoisdata.h
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_ircconnectionnetsplitdetectordata.h
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_ircconnectionserverinfo.h
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_ircconnectionstatedata.h
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_ircconnectionstatistics.h
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_ircconnectiontarget.h
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_ircconnectiontargetresolver.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_ircconnectiontargetresolver.h
InputName=kvi_ircconnectiontargetresolver

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_ircconnectiontargetresolver.h
InputName=kvi_ircconnectiontargetresolver

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_ircconnectionuserinfo.h
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_irccontext.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_irccontext.h
InputName=kvi_irccontext

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_irccontext.h
InputName=kvi_irccontext

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_ircdatastreammonitor.h
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_irclink.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_irclink.h
InputName=kvi_irclink

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_irclink.h
InputName=kvi_irclink

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sparser\kvi_ircmessage.h
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_ircsocket.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_ircsocket.h
InputName=kvi_ircsocket

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_ircsocket.h
InputName=kvi_ircsocket

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_irctoolbar.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_irctoolbar.h
InputName=kvi_irctoolbar

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_irctoolbar.h
InputName=kvi_irctoolbar

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\uparser\kvi_ircurl.h
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_ircview.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_ircview.h
InputName=kvi_ircview

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_ircview.h
InputName=kvi_ircview

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_ircviewprivate.h
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_ircviewtools.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_ircviewtools.h
InputName=kvi_ircviewtools

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_ircviewtools.h
InputName=kvi_ircviewtools

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_action.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\kvs
InputPath=.\kvs\kvi_kvs_action.h
InputName=kvi_kvs_action

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\kvs
InputPath=.\kvs\kvi_kvs_action.h
InputName=kvi_kvs_action

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_aliasmanager.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_array.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_arraycast.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_asyncdnsoperation.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\kvs
InputPath=.\kvs\kvi_kvs_asyncdnsoperation.h
InputName=kvi_kvs_asyncdnsoperation

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\kvs
InputPath=.\kvs\kvi_kvs_asyncdnsoperation.h
InputName=kvi_kvs_asyncdnsoperation

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_asyncoperation.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\kvs
InputPath=.\kvs\kvi_kvs_asyncoperation.h
InputName=kvi_kvs_asyncoperation

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\kvs
InputPath=.\kvs\kvi_kvs_asyncoperation.h
InputName=kvi_kvs_asyncoperation

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_callbackobject.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_corecallbackcommands.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_corefunctions.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_coresimplecommands.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_dnsmanager.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\kvs
InputPath=.\kvs\kvi_kvs_dnsmanager.h
InputName=kvi_kvs_dnsmanager

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\kvs
InputPath=.\kvs\kvi_kvs_dnsmanager.h
InputName=kvi_kvs_dnsmanager

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_event.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_eventhandler.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_eventmanager.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_eventtable.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_hash.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_kernel.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_moduleinterface.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_object.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\kvs
InputPath=.\kvs\kvi_kvs_object.h
InputName=kvi_kvs_object

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\kvs
InputPath=.\kvs\kvi_kvs_object.h
InputName=kvi_kvs_object

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_object_class.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_object_controller.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_object_functioncall.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_object_functionhandler.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_object_functionhandlerimpl.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_parameterprocessor.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_parser.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_parser_macros.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_popupmenu.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\kvs
InputPath=.\kvs\kvi_kvs_popupmenu.h
InputName=kvi_kvs_popupmenu

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\kvs
InputPath=.\kvs\kvi_kvs_popupmenu.h
InputName=kvi_kvs_popupmenu

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_processmanager.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\kvs
InputPath=.\kvs\kvi_kvs_processmanager.h
InputName=kvi_kvs_processmanager

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\kvs
InputPath=.\kvs\kvi_kvs_processmanager.h
InputName=kvi_kvs_processmanager

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_report.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_runtimecall.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_runtimecontext.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_rwevaluationresult.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_script.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_scriptaddonmanager.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_switchlist.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_timermanager.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\kvs
InputPath=.\kvs\kvi_kvs_timermanager.h
InputName=kvi_kvs_timermanager

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\kvs
InputPath=.\kvs\kvi_kvs_timermanager.h
InputName=kvi_kvs_timermanager

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_aliasfunctioncall.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_aliassimplecommand.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_arraycount.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_arrayelement.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_arrayorhashelement.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_arrayreferenceassert.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_base.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_baseobjectfunctioncall.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_callbackcommand.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_command.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_commandevaluation.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_commandwithparameters.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_compositedata.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_constantdata.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_corecallbackcommand.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_corefunctioncall.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_coresimplecommand.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_data.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_datalist.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_expression.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_expressionreturn.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_extendedscopevariable.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_functioncall.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_globalvariable.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_hashcount.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_hashelement.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_hashreferenceassert.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_indirectdata.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_instruction.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_instructionblock.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_localvariable.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_modulecallbackcommand.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_modulefunctioncall.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_modulesimplecommand.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_multipleparameteridentifier.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_objectfield.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_objectfunctioncall.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_operation.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_parameterreturn.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_rebindingswitch.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_scopeoperator.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_simplecommand.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_singleparameteridentifier.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_specialcommand.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_specialcommandbreak.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_specialcommandclass.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_specialcommanddefpopup.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_specialcommanddo.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_specialcommandfor.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_specialcommandforeach.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_specialcommandif.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_specialcommandswitch.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\kvs
InputPath=.\kvs\kvi_kvs_treenode_specialcommandswitch.h
InputName=kvi_kvs_treenode_specialcommandswitch

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_specialcommandunset.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\kvs
InputPath=.\kvs\kvi_kvs_treenode_specialcommandunset.h
InputName=kvi_kvs_treenode_specialcommandunset

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_specialcommandwhile.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_switchlist.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_thisobjectfunctioncall.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_variable.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_treenode_voidfunctioncall.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_types.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_useraction.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\kvs
InputPath=.\kvs\kvi_kvs_useraction.h
InputName=kvi_kvs_useraction

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\kvs
InputPath=.\kvs\kvi_kvs_useraction.h
InputName=kvi_kvs_useraction

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_variant.h
# End Source File
# Begin Source File

SOURCE=.\kvs\kvi_kvs_variantlist.h
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_lagmeter.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_lagmeter.h
InputName=kvi_lagmeter

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_lagmeter.h
InputName=kvi_lagmeter

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_listview.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_listview.h
InputName=kvi_listview

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_listview.h
InputName=kvi_listview

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_maskeditor.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_maskeditor.h
InputName=kvi_maskeditor

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_maskeditor.h
InputName=kvi_maskeditor

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_mdicaption.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_mdicaption.h
InputName=kvi_mdicaption

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_mdicaption.h
InputName=kvi_mdicaption

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_mdichild.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_mdichild.h
InputName=kvi_mdichild

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_mdichild.h
InputName=kvi_mdichild

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_mdimanager.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_mdimanager.h
InputName=kvi_mdimanager

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_mdimanager.h
InputName=kvi_mdimanager

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_menubar.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_menubar.h
InputName=kvi_menubar

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_menubar.h
InputName=kvi_menubar

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\module\kvi_mexlinkfilter.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\module
InputPath=.\module\kvi_mexlinkfilter.h
InputName=kvi_mexlinkfilter

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\module
InputPath=.\module\kvi_mexlinkfilter.h
InputName=kvi_mexlinkfilter

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\module\kvi_mexserverimport.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\module
InputPath=.\module\kvi_mexserverimport.h
InputName=kvi_mexserverimport

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\module
InputPath=.\module\kvi_mexserverimport.h
InputName=kvi_mexserverimport

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\module\kvi_mextoolbar.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\module
InputPath=.\module\kvi_mextoolbar.h
InputName=kvi_mextoolbar

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\module
InputPath=.\module\kvi_mextoolbar.h
InputName=kvi_mextoolbar

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_modeeditor.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_modeeditor.h
InputName=kvi_modeeditor

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_modeeditor.h
InputName=kvi_modeeditor

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_modew.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_modew.h
InputName=kvi_modew

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_modew.h
InputName=kvi_modew

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\module\kvi_module.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\module
InputPath=.\module\kvi_module.h
InputName=kvi_module

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\module\kvi_moduleextension.h
# End Source File
# Begin Source File

SOURCE=.\module\kvi_modulemanager.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\module
InputPath=.\module\kvi_modulemanager.h
InputName=kvi_modulemanager

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\module
InputPath=.\module\kvi_modulemanager.h
InputName=kvi_modulemanager

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_msgbox.h
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_notifylist.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_notifylist.h
InputName=kvi_notifylist

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_notifylist.h
InputName=kvi_notifylist

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sparser\kvi_numeric.h
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_options.h
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_optionswidget.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_optionswidget.h
InputName=kvi_optionswidget

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_optionswidget.h
InputName=kvi_optionswidget

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_out.h
# End Source File
# Begin Source File

SOURCE=.\uparser\kvi_popupmanager.h
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_query.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_query.h
InputName=kvi_query

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_query.h
InputName=kvi_query

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h  -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_scriptbutton.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_scriptbutton.h
InputName=kvi_scriptbutton

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_scriptbutton.h
InputName=kvi_scriptbutton

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_scripteditor.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_scripteditor.h
InputName=kvi_scripteditor

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_scripteditor.h
InputName=kvi_scripteditor

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\uparser\kvi_scriptobject.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\uparser
InputPath=.\uparser\kvi_scriptobject.h
InputName=kvi_scriptobject

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\uparser
InputPath=.\uparser\kvi_scriptobject.h
InputName=kvi_scriptobject

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_selectors.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_selectors.h
InputName=kvi_selectors

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_selectors.h
InputName=kvi_selectors

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\sparser\kvi_sparser.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\sparser
InputPath=.\sparser\kvi_sparser.h
InputName=kvi_sparser

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\sparser
InputPath=.\sparser\kvi_sparser.h
InputName=kvi_sparser

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_splash.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_splash.h
InputName=kvi_splash

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_splash.h
InputName=kvi_splash

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_sslmaster.h
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_statusbar.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_statusbar.h
InputName=kvi_statusbar

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_statusbar.h
InputName=kvi_statusbar

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_statusbarapplet.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_statusbarapplet.h
InputName=kvi_statusbarapplet

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_statusbarapplet.h
InputName=kvi_statusbarapplet

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_styled_controls.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_styled_controls.h
InputName=kvi_styled_controls

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_styled_controls.h
InputName=kvi_styled_controls

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_taskbar.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_taskbar.h
InputName=kvi_taskbar

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_taskbar.h
InputName=kvi_taskbar

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_texticonmanager.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_texticonmanager.h
InputName=kvi_texticonmanager

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\kernel
InputPath=.\kernel\kvi_texticonmanager.h
InputName=kvi_texticonmanager

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_texticonwin.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_texticonwin.h
InputName=kvi_texticonwin

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_texticonwin.h
InputName=kvi_texticonwin

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_themedlabel.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_themedlabel.h
InputName=kvi_themedlabel

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_themedlabel.h
InputName=kvi_themedlabel

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_toolbar.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_toolbar.h
InputName=kvi_toolbar

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_toolbar.h
InputName=kvi_toolbar

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_toolwindows_container.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_toolwindows_container.h
InputName=kvi_toolwindows_container

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_toolwindows_container.h
InputName=kvi_toolwindows_container

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_topicw.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_topicw.h
InputName=kvi_topicw

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_topicw.h
InputName=kvi_topicw

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\uparser\kvi_uparser.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\uparser
InputPath=.\uparser\kvi_uparser.h
InputName=kvi_uparser

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\uparser
InputPath=.\uparser\kvi_uparser.h
InputName=kvi_uparser

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_useraction.h
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_userinput.h
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_userlistview.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_userlistview.h
InputName=kvi_userlistview

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_userlistview.h
InputName=kvi_userlistview

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ui\kvi_window.h

!IF  "$(CFG)" == "kvirc - Win32 Release"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_window.h
InputName=kvi_window

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "kvirc - Win32 Debug"

# Begin Custom Build
InputDir=.\ui
InputPath=.\ui\kvi_window.h
InputName=kvi_window

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\data\resources\resource.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\..\data\resources\icon1.ico
# End Source File
# Begin Source File

SOURCE=..\..\data\resources\kvirc.rc
# End Source File
# End Group
# Begin Group "Generated"

# PROP Default_Filter ".moc"
# Begin Source File

SOURCE=.\kernel\kvi_app.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_channel.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_colorwin.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_console.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_cryptcontroller.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_ctcppagedialog.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_dynamictooltip.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_filedialog.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_frame.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_historywin.moc
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_iconmanager.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_input.moc
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_ipc.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_ipeditor.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_irctoolbar.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_ircview.moc
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_lagmeter.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_maskeditor.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_menubar.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_modeeditor.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_modew.moc
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_notifylist.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_optionswidget.moc
# End Source File
# Begin Source File

SOURCE=.\uparser\kvi_popupmenu.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_query.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_scriptbutton.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_scripteditor.moc
# End Source File
# Begin Source File

SOURCE=.\uparser\kvi_scriptobject.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_selector.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_selectors.moc
# End Source File
# Begin Source File

SOURCE=.\sparser\kvi_sparser.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_splash.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_taskbar.moc
# End Source File
# Begin Source File

SOURCE=.\kernel\kvi_texticonmanager.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_texticonwin.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_themedlabel.moc
# End Source File
# Begin Source File

SOURCE=.\uparser\kvi_timermanager.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_toolbar.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_topicw.moc
# End Source File
# Begin Source File

SOURCE=.\uparser\kvi_uparser.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_userlistview.moc
# End Source File
# Begin Source File

SOURCE=.\ui\kvi_window.moc
# End Source File
# End Group
# End Target
# End Project
