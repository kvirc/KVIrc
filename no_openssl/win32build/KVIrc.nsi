;NSIS Install Script for KVIrc
;Created by Alexey, based on script contributed by Alvis

!include "MUI.nsh"
!include "WinMessages.nsh"
!include "LogicLib.nsh"

Name "KVIrc"
!define VERSION '4.0.0-rc1'
!define RELEASE_NAME 'Insomnia (wip)'
!define /date RELEASE_VERSION '%Y%m%d'
!define URL_ABOUT 'http://www.kvirc.net/'
!define URL_SUPPORT 'http://www.kvirc.net/'
!define URL_UPDATE 'http://www.kvirc.net/'
!define PUBLISHER 'Szymon Stefanek and The KVIrc Development Team'

OutFile KVIrc-${VERSION}-dev-${RELEASE_VERSION}.exe
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
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\README.txt"
!define MUI_FINISHPAGE_RUN "$INSTDIR\kvirc.exe"

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
LangString MsgUninstallOldInstaller ${LANG_ENGLISH} "Previous versions of KVIrc must to be uninstalled."
LangString KVIrcIsRunning ${LANG_ENGLISH} "An instance of KVIrc is currently running. Exit KVIrc and then try again."

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
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc" "UninstallString" '"$INSTDIR\uninstall.exe"'
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc" "URLInfoAbout" "${URL_ABOUT}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc" "HelpLink" "${URL_SUPPORT}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc" "URLUpdateInfo" "${URL_UPDATE}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc" "Publisher" "${PUBLISHER}"
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc" "NoRepair" 1

	WriteUninstaller "uninstall.exe"

	ExecWait "$INSTDIR\win32registrar.exe"

SectionEnd

; Optional section (can be disabled by the user)
Section $(StartMenuSection) StartMenuSection_IDX
  SetShellVarContext all
  CreateDirectory "$SMPROGRAMS\KVIrc"
  CreateShortCut "$SMPROGRAMS\KVIrc\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\KVIrc\KVIrc.lnk" "$INSTDIR\KVIrc.exe" "" "$INSTDIR\KVIrc.exe" 0 "" "" $(ProgramDescription)

SectionEnd

Section $(DesktopSection) DesktopSection_IDX
  SetShellVarContext all
  CreateShortCut "$DESKTOP\KVIrc.lnk" "$INSTDIR\KVIrc.exe" "" "$INSTDIR\KVIrc.exe" 0 "" "" $(ProgramDescription)
SectionEnd

Section $(TraySection) TraySection_IDX
  SetShellVarContext all
  CreateShortCut "$QUICKLAUNCH\KVIrc.lnk" "$INSTDIR\KVIrc.exe" "" "$INSTDIR\KVIrc.exe" 0 "" "" $(ProgramDescription)
SectionEnd

Section /o $(AutostartSection) AutostartSection_IDX
  Call RemoveAutostart
  Call AddAutostart
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
!insertmacro MUI_FUNCTION_DESCRIPTION_END


Function .onInit
	!insertmacro MUI_LANGDLL_DISPLAY

	Call CloseKVIrcInstances

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
	DeleteRegKey  HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc"
    RMDir "$SMPROGRAMS\KVIrc"  
 
done:

FunctionEnd

;Function .onInstSuccess
;FunctionEnd

;--------------------------------
; Uninstaller

Section !un.$(UnGeneralFiles)
  SetShellVarContext all
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc"
  DeleteRegKey HKLM SOFTWARE\KVIrc

  ; Remove autostart entry
  Call un.RemoveAutostart

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\KVIrc\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\KVIrc"
  RMDir /r "$INSTDIR"
SectionEnd

;Remove local data dir
Section /o un.$(UnLocalData)

	DetailPrint $(UnLocalDataDescr)
  ReadINIStr $LocalDir "$PROFILE\kvirc.ini" Main LocalKvircDirectory
  ${un.WordReplace} $LocalDir "%20" " " "+" "$LocalDir"
  ${un.WordReplace} $LocalDir "/" "\" "+" "$LocalDir"
  RMDir /r "$LocalDir"
  Delete "$PROFILE\kvirc.ini"

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
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "KVIrc" "$INSTDIR\kvirc.exe"
  ${ElseIf} $R0 == "HKCU"
    WriteRegStr HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "KVIrc" "$INSTDIR\kvirc.exe"
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
