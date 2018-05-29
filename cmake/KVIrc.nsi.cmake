;NSIS Install Script for KVIrc
;Created by Alexey, based on script contributed by Alvis

!include "MUI.nsh"
!include "WinMessages.nsh"
!include "LogicLib.nsh"
!include "WinVer.nsh"
!include "x64.nsh"
!include "StdUtils.nsh"

Name "@NICENAME@"
!define VERSION '@CMAKE_KVIRC_VERSION_RELEASE@'
!define RELEASE_NAME '@CMAKE_KVIRC_VERSION_CODENAME@'
!define /date GIT_VERSION '@CMAKE_KVIRC_BUILD_REVISION@'
!define KVIRC_BUILD_REVISION '@CMAKE_KVIRC_BUILD_REVISION@'
!define URL_ABOUT 'http://www.kvirc.net/'
!define URL_SUPPORT 'http://www.kvirc.net/'
!define URL_UPDATE 'http://www.kvirc.net/'
!define PUBLISHER 'Szymon Stefanek and The KVIrc Development Team'

; Git release, e.g. KVIrc-4.3.2-dev-git-5465-ge352609.exe
OutFile KVIrc-${KVIRC_BUILD_REVISION}.exe
; Stable version, e.g. KVIrc-4.2.0-Equilibrium.exe
;OutFile KVIrc-${VERSION}-${RELEASE_NAME}.exe

Unicode true
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

!define MUI_FINISHPAGE_RUN
!define MUI_FINISHPAGE_RUN_TEXT "Start KVIrc"
!define MUI_FINISHPAGE_RUN_FUNCTION "LaunchKVIrc"

; Pages
!insertmacro MUI_PAGE_LICENSE "release\License\ABOUT-LICENSE"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_COMPONENTS
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"
LangString UnLocalDataDescr ${LANG_ENGLISH} "This will delete your settings, themes and logs. Keep it unchecked if you plan to reinstall KVIrc later."
LangString UnLocalData ${LANG_ENGLISH} "Settings and logs"
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
LangString MsgUninstallOldInstaller ${LANG_ENGLISH} "Previous versions of KVIrc must be uninstalled."
LangString KVIrcIsRunning ${LANG_ENGLISH} "An instance of KVIrc is currently running. Exit KVIrc and then try again."
LangString WinVerUnsupported ${LANG_ENGLISH} "KVIrc does not support the currently running Windows version.$\r$\nWindows 7 or higher is required."

!include ".\translations\*.nsi"

;--------------------------------
; Sections

Section !$(KVIrc) KVIrc_IDX

	SectionIn RO
	SetDetailsPrint textonly
	DetailPrint $(KVIrcDescr)
	SetDetailsPrint listonly

	SetOutPath "$INSTDIR"
	File /r release\*.*

	; Write the installation path into the registry
	WriteRegStr HKLM SOFTWARE\KVIrc "Install_Dir" "$INSTDIR"

	; Write the uninstall keys for Windows
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc" "DisplayName" "KVIrc"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc" "DisplayIcon" "$INSTDIR\@KVIRC_BINARYNAME@.exe"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc" "DisplayVersion" "${VERSION}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc" "URLInfoAbout" "${URL_ABOUT}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc" "HelpLink" "${URL_SUPPORT}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc" "URLUpdateInfo" "${URL_UPDATE}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc" "Publisher" "${PUBLISHER}"
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc" "NoRepair" 1


	WriteUninstaller "$INSTDIR\uninstall.exe"

	IfFileExists "$INSTDIR\vc_redist.x86.exe" VcRedist86Exists PastVcRedist86Check
	VcRedist86Exists:
		ExecWait '"$INSTDIR\vc_redist.x86.exe"  /quiet /norestart'
	PastVcRedist86Check:

	IfFileExists "$INSTDIR\vc_redist.x64.exe" VcRedist64Exists PastVcRedist64Check
	VcRedist64Exists:
		ExecWait '"$INSTDIR\vc_redist.x64.exe"  /quiet /norestart'
	PastVcRedist64Check:

SectionEnd

; Optional section (can be disabled by the user)
Section $(StartMenuSection) StartMenuSection_IDX
  SetShellVarContext all
  CreateShortCut "$SMPROGRAMS\KVIrc.lnk" "$INSTDIR\@KVIRC_BINARYNAME@.exe" "" "$INSTDIR\@KVIRC_BINARYNAME@.exe" 0 "" "" $(ProgramDescription)
SectionEnd

Section $(DesktopSection) DesktopSection_IDX
  SetShellVarContext all
  CreateShortCut "$DESKTOP\KVIrc.lnk" "$INSTDIR\@KVIRC_BINARYNAME@.exe" "" "$INSTDIR\@KVIRC_BINARYNAME@.exe" 0 "" "" $(ProgramDescription)
SectionEnd

Section $(TraySection) TraySection_IDX
  SetShellVarContext all
  CreateShortCut "$QUICKLAUNCH\KVIrc.lnk" "$INSTDIR\@KVIRC_BINARYNAME@.exe" "" "$INSTDIR\@KVIRC_BINARYNAME@.exe" 0 "" "" $(ProgramDescription)
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
!insertmacro MUI_FUNCTION_DESCRIPTION_END


Function .onInit
    ${IfNot} ${AtLeastWin7}
      MessageBox MB_OK|MB_ICONSTOP "$(WinVerUnsupported)"
      Quit
    ${EndIf}
    !insertmacro MUI_LANGDLL_DISPLAY

    Call CloseKVIrcInstances

    ${If} ${RunningX64}
        ; disable registry redirection (enable access to 64-bit portion of registry)
        SetRegView 64
        ; change install dir
        StrCpy $INSTDIR $PROGRAMFILES64\KVIrc
    ${EndIf}

    SetShellVarContext all
    ; Remove old installer, check for 32-bit first, we don't want both installed
    ${If} ${RunningX64}
        SetRegView 32
        ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc" "UninstallString"
        StrCmp $R0 "" check64
    ${Else}
        ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc" "UninstallString"
        StrCmp $R0 "" done
    ${EndIf}


    MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION "$(MsgUninstallOldInstaller)" IDOK uninst
    Abort

check64:
    SetRegView 64
    ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc" "UninstallString"
    StrCmp $R0 "" done
    MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION "$(MsgUninstallOldInstaller)" IDOK uninst

    ;Run the uninstaller
uninst:
    ClearErrors
    ExecWait "$R0"

done:

FunctionEnd

;--------------------------------
; Uninstaller

Section !un.$(UnGeneralFiles)
    Delete "$INSTDIR\uninstall.exe"

    SetShellVarContext all

    ; Remove registry keys
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KVIrc"
    DeleteRegKey HKLM SOFTWARE\KVIrc

    ; Remove shortcuts, if any
    Delete "$SMPROGRAMS\KVIrc.lnk"
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

Function LaunchKVIrc
  ${StdUtils.ExecShellAsUser} $0 "$INSTDIR\@KVIRC_BINARYNAME@.exe" "open" ""
FunctionEnd

Function CloseKVIrcInstances
; Waits for all running instances of KVIrc to close
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
; Waits for all running instances of KVIrc to close
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
