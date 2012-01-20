;NSIS Install Script for KVIrc
;Created by Alexey, based on script contributed by Alvis

!include "MUI.nsh"
!include "WinMessages.nsh"
!include "LogicLib.nsh"
!include "WinVer.nsh"

Name "@NICENAME@"
!define VERSION '@CMAKE_KVIRC_VERSION_RELEASE@'
!define RELEASE_NAME 'Equilibrium (wip)'
!define /date RELEASE_VERSION 'r@CMAKE_KVIRC_BUILD_REVISION@'
!define URL_ABOUT 'http://www.kvirc.net/'
!define URL_SUPPORT 'http://www.kvirc.net/'
!define URL_UPDATE 'http://www.kvirc.net/'
!define PUBLISHER 'Szymon Stefanek and The KVIrc Development Team'

; Svn release, eg: KVIrc-4.0.0-rc3-dev-r4300.exe
OutFile KVIrc-${VERSION}-${RELEASE_VERSION}-dev.exe
; Stable version, eg: KVIrc-4.0.0-Insomnia.exe
;OutFile KVIrc-${VERSION}-${RELEASE_NAME}.exe

SetCompressor /SOLID lzma
XPStyle on
InstallDir $PROGRAMFILES\KVIrc
InstallDirRegKey HKLM "Software\KVIrc" "Install_Dir"
Icon ..\data\resources_win32\icon1.ico

;--------------------------------
!include "WordFunc.nsh"
!insertmacro un.WordReplace #we need this function for uninstall local data dir
Var LocalDir
;--------------------------------

!define MUI_ICON ..\data\resources_win32\icon1.ico
!define MUI_UNICON ..\data\resources_win32\icon1.ico
!define MUI_LANGDLL_REGISTRY_ROOT "HKCU"
!define MUI_LANGDLL_REGISTRY_KEY "Software\KVIrc"
!define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"
!define MUI_LANGDLL_ALWAYSSHOW
!define MUI_FINISHPAGE_RUN "$INSTDIR\@KVIRC_BINARYNAME@.exe"

; Pages
!insertmacro MUI_PAGE_LICENSE "release\License\COPYING"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_COMPONENTS
!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
!insertmacro MUI_RESERVEFILE_LANGDLL
ReserveFile "${NSISDIR}\Plugins\UserInfo.dll"

!insertmacro MUI_LANGUAGE "English"
LangString UnLocalDataDescr ${LANG_ENGLISH} "This will delete your settings, themes, logs. Keep it unchecked if you plan to reinstall KVIrc later."
LangString UnLocalData ${LANG_ENGLISH} "Settings"
LangString UnGeneralFiles ${LANG_ENGLISH} "Program files"
LangString ProgramDescription ${LANG_ENGLISH} "Visual IRC client"
LangString TraySection ${LANG_ENGLISH} "Quick launch icon"
LangString TraySectionDescr ${LANG_ENGLISH} "Create quick launch icon"
LangString DesktopSection ${LANG_ENGLISH} "Desktop shortcut"
LangString DesktopSectionDescr ${LANG_ENGLISH} "Create desktop shortcut"
LangString KVIrc ${LANG_ENGLISH} "KVIrc (required)"
LangString KVIrcDescr ${LANG_ENGLISH} "KVIrc program files"
LangString StartMenuSection ${LANG_ENGLISH} "Start menu"
LangString StartMenuSectionDescr ${LANG_ENGLISH} "Create start menu icon"
LangString AutostartSection ${LANG_ENGLISH} "Autostart"
LangString AutostartSectionDescr ${LANG_ENGLISH} "Start program when user login"
LangString MsgUninstallOldInstaller ${LANG_ENGLISH} "Previous versions of KVIrc must be uninstalled."
LangString KVIrcIsRunning ${LANG_ENGLISH} "An instance of KVIrc is currently running. Exit KVIrc and then try again."
LangString WinampSection ${LANG_ENGLISH} "Winamp plugin"
LangString WinampSectionDescr ${LANG_ENGLISH} "Install Winamp plugin"
LangString WinVerUnsupported ${LANG_ENGLISH} "KVIrc does not support the currently running Windows Version.$\r$\nWindows XP or higher is required." 

!include ".\translations\*.nsi"

;--------------------------------
; Sections

Section !$(KVIrc) KVIrc_IDX

	SectionIn RO
	SetDetailsPrint textonly
	DetailPrint $(KVIrcDescr)
	SetDetailsPrint listonly

	SetOutPath "$INSTDIR"
	File release\*.*
	SetOutPath "$INSTDIR\config"
	File /r release\config\*.*
	SetOutPath "$INSTDIR\qt-plugins"
	File /r release\qt-plugins\*.*
	SetOutPath "$INSTDIR\defscript"
	File /r release\defscript\*.*
	SetOutPath "$INSTDIR\doc"
	File /r release\doc\*.*
	SetOutPath "$INSTDIR\help"
	File /r release\help\*.*
	SetOutPath "$INSTDIR\license"
	File release\license\COPYING
	SetOutPath "$INSTDIR\locale"
	File release\locale\*.mo
	SetOutPath "$INSTDIR\audio"
	File release\audio\*.wav
	SetOutPath "$INSTDIR\modules"
	File /r release\modules\*.*
	SetOutPath "$INSTDIR\msgcolors"
	File /r release\msgcolors\*.*
	SetOutPath "$INSTDIR\pics"
	File release\pics\*.png
	SetOutPath "$INSTDIR\pics\coresmall\"
	File release\pics\coresmall\*.png
	SetOutPath "$INSTDIR\themes"
	File /r release\themes\*.*


	; Write the installation path into the registry
	WriteRegStr HKLM SOFTWARE\KVIrc "Install_Dir" "$INSTDIR"

	; Write the uninstall keys for Windows
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc" "DisplayName" "KVIrc"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc" "URLInfoAbout" "${URL_ABOUT}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc" "HelpLink" "${URL_SUPPORT}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc" "URLUpdateInfo" "${URL_UPDATE}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc" "Publisher" "${PUBLISHER}"
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc" "NoRepair" 1


	WriteUninstaller "$INSTDIR\uninstall.exe"

	ExecWait "$INSTDIR\win32registrar.exe"

SectionEnd

; Optional section (can be disabled by the user)
Section $(StartMenuSection) StartMenuSection_IDX
  SetShellVarContext all
  CreateDirectory "$SMPROGRAMS\KVIrc"
  CreateShortCut "$SMPROGRAMS\KVIrc\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\KVIrc\KVIrc.lnk" "$INSTDIR\@KVIRC_BINARYNAME@.exe" "" "$INSTDIR\@KVIRC_BINARYNAME@.exe" 0 "" "" $(ProgramDescription)

SectionEnd

Section $(DesktopSection) DesktopSection_IDX
  SetShellVarContext all
  CreateShortCut "$DESKTOP\KVIrc.lnk" "$INSTDIR\@KVIRC_BINARYNAME@.exe" "" "$INSTDIR\@KVIRC_BINARYNAME@.exe" 0 "" "" $(ProgramDescription)
SectionEnd

Section $(TraySection) TraySection_IDX
  SetShellVarContext all
  CreateShortCut "$QUICKLAUNCH\KVIrc.lnk" "$INSTDIR\@KVIRC_BINARYNAME@.exe" "" "$INSTDIR\@KVIRC_BINARYNAME@.exe" 0 "" "" $(ProgramDescription)
SectionEnd

Section /o $(AutostartSection) AutostartSection_IDX
  Call RemoveAutostart
  Call AddAutostart
SectionEnd

Section $(WinampSection) WinampSection_IDX
  ReadRegStr $R0 HKCU Software\Winamp ""
  IfFileExists "$R0\winamp.exe" 0 +2
  Rename "$INSTDIR\modules\gen_kvirc.dll" "$R0\Plugins\gen_kvirc.dll"
SectionEnd

;--------------------------------
; Descriptions

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
!insertmacro MUI_DESCRIPTION_TEXT ${KVIrc_IDX} \
        $(KVIrcDescr)
!insertmacro MUI_DESCRIPTION_TEXT ${StartMenuSection_IDX} \
        $(StartMenuSectionDescr)
!insertmacro MUI_DESCRIPTION_TEXT ${DesktopSection_IDX} \
        $(DesktopSectionDescr)
!insertmacro MUI_DESCRIPTION_TEXT ${TraySection_IDX} \
        $(TraySectionDescr)
!insertmacro MUI_DESCRIPTION_TEXT ${AutostartSection_IDX} \
        $(AutostartSectionDescr)
!insertmacro MUI_DESCRIPTION_TEXT ${WinampSection_IDX} \
        $(WinampSectionDescr)
!insertmacro MUI_FUNCTION_DESCRIPTION_END


Function .onInit
    ${IfNot} ${AtLeastWinXP}
      MessageBox MB_OK|MB_ICONSTOP "$(WinVerUnsupported)"
      Quit
    ${EndIf}
    !insertmacro MUI_LANGDLL_DISPLAY

    Call CloseKVIrcInstances

    ReadRegStr $R0 HKCU Software\Winamp ""
    IfFileExists "$R0\winamp.exe" continue 0
        SectionSetFlags ${WinampSection_IDX} 16 # 10000 in binary: disabled+unchecked
continue:


    SetShellVarContext all
    ; Remove old installer
    ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc" "UninstallString"
    StrCmp $R0 "" done

    MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION "$(MsgUninstallOldInstaller)" IDOK uninst
    Abort
 
    ;Run the uninstaller
uninst:
    ClearErrors
    ExecWait "$R0"
 
done:

FunctionEnd

;Function .onInstSuccess
;FunctionEnd

;--------------------------------
; Uninstaller

Section !un.$(UnGeneralFiles)
    Delete "$INSTDIR\uninstall.exe"

    SetShellVarContext all

    ; Remove registry keys
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc"
    DeleteRegKey HKLM SOFTWARE\KVIrc

    ; Remove autostart entry
    Call un.RemoveAutostart

    ; Remove shortcuts, if any
    Delete "$SMPROGRAMS\KVIrc\*.*"
    RMDir "$SMPROGRAMS\KVIrc"
    Delete "$DESKTOP\KVIrc.lnk"
    Delete "$QUICKLAUNCH\KVIrc.lnk"
  
    ; Delete installed trees of files
    RMDir /r "$INSTDIR\config"
    RMDir /r "$INSTDIR\audio"
    RMDir /r "$INSTDIR\qt-plugins"
    RMDir /r "$INSTDIR\defscript"
    RMDir /r "$INSTDIR\doc"
    RMDir /r "$INSTDIR\help"
    RMDir /r "$INSTDIR\license"
    RMDir /r "$INSTDIR\locale"
    RMDir /r "$INSTDIR\modules"
    RMDir /r "$INSTDIR\msgcolors"
    RMDir /r "$INSTDIR\pics"
    RMDir /r "$INSTDIR\themes"
    Delete "$INSTDIR\*.dll"
    Delete "$INSTDIR\*.exe"
    Delete "$INSTDIR\*.ini"
    RMDir "$INSTDIR"

    ReadRegStr $R0 HKCU Software\Winamp ""
        IfFileExists "$R0\Plugins\gen_kvirc.dll" 0 +2
        Delete "$R0\Plugins\gen_kvirc.dll"

SectionEnd

;Remove local data dir
Section /o un.$(UnLocalData)

    DetailPrint $(UnLocalDataDescr)
    IfFileExists "$INSTDIR\portable" portable notportable

portable:
    RMDir /r "$INSTDIR\Settings"
    Delete "$INSTDIR\portable"
    Goto done

notportable:
	; test kvirc4 file
    ReadINIStr $LocalDir "$PROFILE\kvirc4.ini" Main LocalKvircDirectory
	; not found => don't delete anything
	StrCmp $LocalDir "" done

    ${un.WordReplace} $LocalDir "%20" " " "+" "$LocalDir"
    ${un.WordReplace} $LocalDir "/" "\" "+" "$LocalDir"
    RMDir /r "$LocalDir"
    Delete "$PROFILE\kvirc4.ini"

done:

SectionEnd

Function un.onInit
  !insertmacro MUI_UNGETLANGUAGE
  Call un.CloseKVIrcInstances
FunctionEnd

;--------------------------------
; Functions

Function AddAutostart
  Call CheckUserInstallRights
  Pop $R0
  ${If} $R0 == "HKLM"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "KVIrc" "$INSTDIR\@KVIRC_BINARYNAME@.exe"
  ${ElseIf} $R0 == "HKCU"
    WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "KVIrc" "$INSTDIR\@KVIRC_BINARYNAME@.exe"
  ${EndIf}
FunctionEnd

Function RemoveAutostart
  Call RemoveAutostartShortcuts

  ; Remove registry keys
  DeleteRegValue HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "KVIrc"
  DeleteRegValue HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "KVIrc"
FunctionEnd

Function un.RemoveAutostart
  Call un.RemoveAutostartShortcuts

  ; Remove registry keys
  DeleteRegValue HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "KVIrc"
  DeleteRegValue HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "KVIrc"
FunctionEnd

Function RemoveAutostartShortcuts
  ; Remove user created sturtup shortcuts
  ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders" "Common Startup"
  StrLen $R1 "$R0"
  ${If} $R1 > 0
    StrCpy $R0 "\KVIrc.lnk" $R1
    Delete "$R0"
  ${EndIf}

  ReadRegStr $R0 HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders" "Common Startup"
  StrLen $R1 "$R0"
  ${If} $R1 > 0
    StrCpy $R0 "\KVIrc.lnk" $R1
    Delete "$R0"
  ${EndIf}
FunctionEnd

Function un.RemoveAutostartShortcuts
  ; Remove user created sturtup shortcuts
  ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders" "Common Startup"
  StrLen $R1 "$R0"
  ${If} $R1 > 0
    StrCpy $R0 "\KVIrc.lnk" $R1
    Delete "$R0"
  ${EndIf}

  ReadRegStr $R0 HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders" "Common Startup"
  StrLen $R1 "$R0"
  ${If} $R1 > 0
    StrCpy $R0 "\KVIrc.lnk" $R1
    Delete "$R0"
  ${EndIf}
FunctionEnd

Function CheckUserInstallRights
  ClearErrors
  UserInfo::GetName
  IfErrors Win9x
  Pop $0
  UserInfo::GetAccountType
  Pop $1

  StrCmp $1 "Admin" 0 +3
    StrCpy $1 "HKLM"
  Goto done
  StrCmp $1 "Power" 0 +3
    StrCpy $1 "HKLM"
  Goto done
  StrCmp $1 "User" 0 +3
    StrCpy $1 "HKCU"
  Goto done
  StrCmp $1 "Guest" 0 +3
    StrCpy $1 "NONE"
  Goto done
  ; Unknown error
    StrCpy $1 "NONE"
  Goto done

  Win9x:
    StrCpy $1 "HKLM"

  done:
    Push $1
FunctionEnd

Function CloseKVIrcInstances
; Waits for all running instances of KVirc to close
    Push $0 ;saving stack
  newcheck:
    FindWindow $0 "QWidget" "Kvirc" 0
    IntCmp $0 0 done
    MessageBox MB_RETRYCANCEL|MB_ICONEXCLAMATION "$(KVIrcIsRunning)" IDRETRY newcheck
    ; cancel
    Quit
  done:
    Pop $0 ; restoring stack
FunctionEnd

Function un.CloseKVIrcInstances
; Waits for all running instances of KVirc to close
    Push $0 ;saving stack
  newcheck:
    FindWindow $0 "QWidget" "Kvirc" 0
    IntCmp $0 0 done
    MessageBox MB_RETRYCANCEL|MB_ICONEXCLAMATION "$(KVIrcIsRunning)" IDRETRY newcheck
    ; cancel
    Quit
  done:
    Pop $0 ; restoring stack
FunctionEnd
