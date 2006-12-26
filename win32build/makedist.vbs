Set sh = CreateObject("WScript.Shell")

Function GetEnv(szName)
	Set env = sh.Environment("USER")
	s = env(szName)
	If Len(s) < 1 Then
		Set env = sh.Environment
		s = env(szName)
	End If
	GetEnv = s
End Function

SSLLIBDIR=GetEnv("SSLLIBDIR")
PERLDIR=GetEnv("PERLDIR")
QTDIR=GetEnv("QTDIR")
MSGFMT=GetEnv("MSGFMT")

PROJECTDIR=".."
PACKAGEDIR="package"
DOCSDIR="..\..\kvirc-docs"


SUBDIRS = Array( _
	"pics", _
	"pics\coresmall", _
	"doc", _
	"doc\scriptexamples", _
	"doc\scriptexamples\mp3share", _
	"doc\scriptexamples\playing", _
	"doc\scriptexamples\antiidle", _
	"doc\scriptexamples\runmenu", _
	"modules", _
	"modules\caps", _
	"modules\caps\action", _
	"modules\caps\crypt", _
	"modules\caps\tool", _
	"modules\caps\toolbar", _
	"modules\caps\serverimport", _
	"themes", _
	"themes\silverirc", _
	"msgcolors", _
	"defscript", _
	"config", _
	"config\modules", _
	"help", _
	"help\en", _
	"license", _
	"locale" _
)

Dim objShell

Set fso = CreateObject("Scripting.FileSystemObject")

If (Not fso.FolderExists(PACKAGEDIR)) Then fso.CreateFolder(PACKAGEDIR)
For Each adir in SUBDIRS
	newdir = PACKAGEDIR + "/" + adir
	If (Not fso.FolderExists(newdir)) Then fso.CreateFolder(newdir)
Next

fso.CopyFile PROJECTDIR + "/README",  PACKAGEDIR + "/README.txt", True
fso.CopyFile PROJECTDIR + "/ChangeLog",  PACKAGEDIR + "/ChangeLog.txt", true
fso.CopyFile PROJECTDIR + "/doc/scriptexamples/mp3share/mp3share_icon.png", PACKAGEDIR + "/doc/scriptexamples/mp3share/", true
fso.CopyFile PROJECTDIR + "/doc/scriptexamples/mp3share/mp3share.kvs", PACKAGEDIR + "/doc/scriptexamples/mp3share/", true
fso.CopyFile PROJECTDIR + "/doc/scriptexamples/playing/playing_icon.png", PACKAGEDIR + "/doc/scriptexamples/playing/", true
fso.CopyFile PROJECTDIR + "/doc/scriptexamples/playing/playing.kvs", PACKAGEDIR + "/doc/scriptexamples/playing/", true
fso.CopyFile PROJECTDIR + "/doc/scriptexamples/antiidle/antiidle.kvs", PACKAGEDIR + "/doc/scriptexamples/antiidle/", true
fso.CopyFile PROJECTDIR + "/doc/scriptexamples/runmenu/runmenu.kvs", PACKAGEDIR + "/doc/scriptexamples/runmenu/", true

fso.CopyFile PROJECTDIR + "/doc/*.txt", PACKAGEDIR + "/doc/", true
fso.CopyFile PROJECTDIR + "/INSTALL", PACKAGEDIR + "/doc/", true
fso.CopyFile PROJECTDIR + "/FAQ", PACKAGEDIR + "/doc/", true
fso.CopyFile PROJECTDIR + "/TODO", PACKAGEDIR + "/doc/", true

fso.CopyFile PROJECTDIR + "/data/msgcolors/*.msgclr", PACKAGEDIR + "/msgcolors/", true

fso.CopyFile PROJECTDIR + "/data/deftheme/silverirc/*.kvc", PACKAGEDIR + "/themes/silverirc/", true
fso.CopyFile PROJECTDIR + "/data/deftheme/silverirc/*.png", PACKAGEDIR + "/themes/silverirc/", true

fso.CopyFile PROJECTDIR + "/data/config/*.kvc", PACKAGEDIR + "/config/", True
fso.CopyFile PROJECTDIR + "/data/config/preinstalled.kvc.win32-example", PACKAGEDIR + "/config/preinstalled.kvc.win32-example", true
fso.CopyFile PROJECTDIR + "/src/modules/notifier/libkvinotifier.kvc", PACKAGEDIR + "/config/modules/", true

fso.CopyFile PROJECTDIR + "/data/manifests/Microsoft.VC80.CRT.manifest", PACKAGEDIR + "/Microsoft.VC80.CRT.manifest", true
fso.CopyFile PROJECTDIR + "/data/manifests/Microsoft.VC80.CRT.modules.manifest", PACKAGEDIR + "/modules/Microsoft.VC80.CRT.manifest", true

fso.CopyFile PROJECTDIR + "/data/defscript/default.kvs", PACKAGEDIR + "/defscript/", true
fso.CopyFile PROJECTDIR + "/data/defscript/theme-install.kvs", PACKAGEDIR + "/defscript/", true
fso.CopyFile PROJECTDIR + "/data/defscript/installer.kvs", PACKAGEDIR + "/defscript/", true

Function CopyAllPNGRecursive(path,target)
	On Error Resume Next
	Dim fol
	Dim sf
	fso.CopyFile path + "/*.png", target, true
	Set fol = fso.GetFolder(path)
	Set sf = fol.SubFolders
	For Each f in sf
		CopyAllPNGRecursive path + "/" + f.name, target + "/" + f.name
	Next
End Function

Function CopyAllPNGRecursiveInSingleFolder(path,target)
	On Error Resume Next
	Dim fol
	Dim sf
	fso.CopyFile path + "/*.png", target, true
	Set fol = fso.GetFolder(path)
	Set sf = fol.SubFolders
	For Each f in sf
		CopyAllPNGRecursiveInSingleFolder path + "/" + f.name, target
	Next
End Function

CopyAllPNGRecursiveInSingleFolder PROJECTDIR + "/src/", PACKAGEDIR + "/pics/"
CopyAllPNGRecursive PROJECTDIR + "/data/pics/", PACKAGEDIR + "/pics/"
'fso.CopyFile PROJECTDIR + "/src/*.png", PACKAGEDIR + "/pics/", true




fso.CopyFile PROJECTDIR + "/doc/COPYING", PACKAGEDIR + "/license/", true
fso.CopyFile PROJECTDIR + "/src/modules/tip/libkvitip*.kvc", PACKAGEDIR + "/config/modules/", true
fso.CopyFile PROJECTDIR + "/src/modules/logview/caps/logview", PACKAGEDIR + "/modules/caps/tool/", true
fso.CopyFile PROJECTDIR + "/src/modules/lamerizer/lamerizer", PACKAGEDIR + "/modules/caps/crypt/", true
fso.CopyFile PROJECTDIR + "/src/modules/rijndael/caps/rijndael", PACKAGEDIR + "/modules/caps/crypt/", true
fso.CopyFile PROJECTDIR + "/src/modules/filetransferwindow/caps/filetransferwindow", PACKAGEDIR + "/modules/caps/tool/", true
fso.CopyFile PROJECTDIR + "/src/modules/sharedfileswindow/caps/sharedfileswindow", PACKAGEDIR + "/modules/caps/tool/", true
fso.CopyFile PROJECTDIR + "/src/modules/mircimport/caps/mircimport", PACKAGEDIR + "/modules/caps/serverimport/", true
fso.CopyFile PROJECTDIR + "/src/modules/url/caps/url", PACKAGEDIR + "/modules/caps/action/", true


fso.CopyFile PROJECTDIR + "/data/helppics/helplogoleft.png", PACKAGEDIR + "/help/en/", true
fso.CopyFile PROJECTDIR + "/data/helppics/helplogoright.png", PACKAGEDIR + "/help/en/", true

Dim docFiles
docFiles = ""

Function GenDoc(path)
	On Error Resume Next
	Dim fol
	Dim sf
	
	Set fol = fso.GetFolder(path)
	Set fileList = fol.Files
	For Each f in fileList
		If (((Right(f.name, 4) = ".cpp") and (Left(f.name, 7) <> "kvi_kvs")) or (Right(f.name, 9) = ".template") or _
			((Left(f.name, 7) = "kvi_kvs") and (Right(f.name, 4) = ".cpp"))) Then

			docFiles = docFiles + " """ + path + "\" + f.name + """"

		End If
	Next
	Set sf = fol.SubFolders
	For Each f in sf
		GenDoc (path + "/" + f.name)
	Next
End Function

Set oShell = WScript.CreateObject("WSCript.Shell")

If (MsgBox("Do you want to run gendoc.pl? (This may take a while)", vbYesNo) = vbYes) Then
	If (Not fso.FolderExists(DOCSDIR)) Then fso.CreateFolder(DOCSDIR)

	GenDoc PROJECTDIR
	
	MsgBox("""" + PERLDIR + "bin\perl.exe"" """ + PROJECTDIR + "\admin\gendoc.pl"" """ + DOCSDIR + """ " + docFiles )
	Set oExec = oShell.Exec("""" + PERLDIR + "\bin\perl.exe"" """ + PROJECTDIR + "\admin\gendoc.pl"" """ + DOCSDIR + """ " + docFiles)
	
	Do While oExec.Status = 0
	     WScript.Sleep 100
	Loop
	
	MsgBox oExec.StdOut.ReadAll
	fso.CopyFile DOCSDIR + "/*.html", PACKAGEDIR + "/help/en/", true
End If


Function makeTranslation(path)
	On Error Resume Next
	Dim fol
	Dim sf
	
	Set fol = fso.GetFolder(path)
	Set fileList = fol.Files
	For Each f in fileList
		If (Right(f.name, 3) = ".po") Then
			fileName = Mid(f.name, 1, Len(f.name) - 3)
			Set oExec = oShell.Exec("""" + MSGFMT + """ -vo """ + path + "\" + fileName + ".mo"" """ + path + "\" + f.name + """")
			Do While oExec.Status = 0
			     WScript.Sleep 50
			Loop
			fso.CopyFile path + "\" + fileName + ".mo", PACKAGEDIR + "/locale/", true
		End If
	Next
	Set sf = fol.SubFolders
	For Each f in sf
		makeTranslation(path + "\" + f.name)
	Next
End Function

If (MsgBox("Do you want to make translations?", vbYesNo) = vbYes) Then
	makeTranslation(PROJECTDIR + "\po")
End If

Set oShell = Nothing

fso.CopyFile PROJECTDIR + "\win32build\bin\*.dll", PACKAGEDIR
fso.CopyFile PROJECTDIR + "\win32build\bin\*.exe", PACKAGEDIR
fso.CopyFile PROJECTDIR + "\win32build\bin\modules\*.dll", PACKAGEDIR + "\modules\", true



Set fso = Nothing


MsgBox "Package ready. Don't forget to include msvcr71.dll if you're compiling with VS.NET2003 or msvcr80.dll for VSC++2005"
