#include <windows.h>
#include <tchar.h>
#include <shlwapi.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance,
                     LPSTR lpszCmdLine,int nCmdShow)
{
#define PATH_LEN 2048
	DWORD dwSize;
	HKEY hKey;
	DWORD err;
	LPWCH szModuleName = (LPWCH) malloc(PATH_LEN*sizeof(WCHAR));
	LPWCH szModulePath = (LPWCH) malloc(PATH_LEN*sizeof(WCHAR));
	LPWCH szBuffer = (LPWCH) malloc(PATH_LEN*sizeof(WCHAR));
	dwSize = GetModuleFileName(0,szModuleName,PATH_LEN);
	int iFilePos=dwSize;

	for(iFilePos=dwSize;szModuleName[iFilePos]!='\\';iFilePos--);

	wcscpy(szModuleName+iFilePos+1,_TEXT("kvirc.exe"));
	wcsncpy(szModulePath,szModuleName,iFilePos);
	szModulePath[iFilePos]=0;



	//IRC://
	SHDeleteKey(HKEY_CLASSES_ROOT,_T("irc"));

	err=RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("irc"),0,0,0,KEY_WRITE,0,&hKey,0);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)(_T("URL:IRC Protocol")),16*2);
	RegSetValueEx( hKey,_T("URL Protocol"),0,REG_SZ,(LPBYTE)"",0);

	RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("irc\\DefaultIcon"),0,0,0,KEY_WRITE,0,&hKey,0);
	wsprintf(szBuffer,_T("%s,0"),szModuleName);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)szBuffer,wcslen(szBuffer)*2);

	RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("irc\\Shell\\open"),0,0,0,KEY_WRITE,0,&hKey,0);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)(_T("Open with KVIrc")),strlen("Open with KVIrc")*2);

	RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("irc\\Shell\\open\\command"),0,0,0,KEY_WRITE,0,&hKey,0);
	wsprintf(szBuffer,_T("%s --external \"%%1\""),szModuleName);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)szBuffer,wcslen(szBuffer)*2);

	//IRCS://
	SHDeleteKey(HKEY_CLASSES_ROOT,_T("ircs"));

	err=RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("ircs"),0,0,0,KEY_WRITE,0,&hKey,0);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)(_T("URL:IRC Protocol")),16*2);
	RegSetValueEx( hKey,_T("URL Protocol"),0,REG_SZ,(LPBYTE)"",0);

	RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("ircs\\DefaultIcon"),0,0,0,KEY_WRITE,0,&hKey,0);
	wsprintf(szBuffer,_T("%s,0"),szModuleName);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)szBuffer,wcslen(szBuffer)*2);

	RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("ircs\\Shell\\open"),0,0,0,KEY_WRITE,0,&hKey,0);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)(_T("Open with KVIrc")),strlen("Open with KVIrc")*2);

	RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("ircs\\Shell\\open\\command"),0,0,0,KEY_WRITE,0,&hKey,0);
	wsprintf(szBuffer,_T("%s --external \"%%1\""),szModuleName);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)szBuffer,wcslen(szBuffer)*2);

	//IRC6://
	SHDeleteKey(HKEY_CLASSES_ROOT,_T("irc6"));

	err=RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("irc6"),0,0,0,KEY_WRITE,0,&hKey,0);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)(_T("URL:IRC Protocol")),16*2);
	RegSetValueEx( hKey,_T("URL Protocol"),0,REG_SZ,(LPBYTE)"",0);

	RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("irc6\\DefaultIcon"),0,0,0,KEY_WRITE,0,&hKey,0);
	wsprintf(szBuffer,_T("%s,0"),szModuleName);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)szBuffer,wcslen(szBuffer)*2);

	RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("irc6\\Shell\\open"),0,0,0,KEY_WRITE,0,&hKey,0);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)(_T("Open with KVIrc")),strlen("Open with KVIrc")*2);

	RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("irc6\\Shell\\open\\command"),0,0,0,KEY_WRITE,0,&hKey,0);
	wsprintf(szBuffer,_T("%s --external \"%%1\""),szModuleName);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)szBuffer,wcslen(szBuffer)*2);

	//IRCS6://
	SHDeleteKey(HKEY_CLASSES_ROOT,_T("ircs6"));

	err=RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("ircs6"),0,0,0,KEY_WRITE,0,&hKey,0);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)(_T("URL:IRC Protocol")),16*2);
	RegSetValueEx( hKey,_T("URL Protocol"),0,REG_SZ,(LPBYTE)"",0);

	RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("ircs6\\DefaultIcon"),0,0,0,KEY_WRITE,0,&hKey,0);
	wsprintf(szBuffer,_T("%s,0"),szModuleName);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)szBuffer,wcslen(szBuffer)*2);

	RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("ircs6\\Shell\\open"),0,0,0,KEY_WRITE,0,&hKey,0);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)(_T("Open with KVIrc")),strlen("Open with KVIrc")*2);

	RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("ircs6\\Shell\\open\\command"),0,0,0,KEY_WRITE,0,&hKey,0);
	wsprintf(szBuffer,_T("%s --external \"%%1\""),szModuleName);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)szBuffer,wcslen(szBuffer)*2);

//KVS


	SHDeleteKey(HKEY_CLASSES_ROOT,_T(".kvs"));

	err=RegCreateKeyEx(HKEY_CLASSES_ROOT,_T(".kvs"),0,0,0,KEY_WRITE,0,&hKey,0);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)_T("KVIrcScript"),11*2);


	SHDeleteKey(HKEY_CLASSES_ROOT,_T("KVIrcScript"));
	RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("KVIrcScript"),0,0,0,KEY_WRITE,0,&hKey,0);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)_T("KVIrc KVS Script"),wcslen(_T("KVIrc KVS Script"))*2);

	RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("KVIrcScript\\DefaultIcon"),0,0,0,KEY_WRITE,0,&hKey,0);
	wsprintf(szBuffer,_T("%s,1"),szModuleName);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)szBuffer,wcslen(szBuffer)*2);

	RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("KVIrcScript\\Shell\\Parse"),0,0,0,KEY_WRITE,0,&hKey,0);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)_T("Run KVS Script"),wcslen(_T("Run KVS Script"))*2);

	RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("KVIrcScript\\Shell\\Parse\\command"),0,0,0,KEY_WRITE,0,&hKey,0);
	wsprintf(szBuffer,_T("%s \"%%1\""),szModuleName);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)szBuffer,wcslen(szBuffer)*2);

	//Configs
	SHDeleteKey(HKEY_CLASSES_ROOT,_T(".kvc"));

	err=RegCreateKeyEx(HKEY_CLASSES_ROOT,_T(".kvc"),0,0,0,KEY_WRITE,0,&hKey,0);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)_T("KVIrcConfig"),11*2);


	SHDeleteKey(HKEY_CLASSES_ROOT,_T("KVIrcConfig"));
	RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("KVIrcConfig"),0,0,0,KEY_WRITE,0,&hKey,0);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)_T("KVIrc Configuration File"),wcslen(_T("KVIrc Configuration File"))*2);

	RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("KVIrcConfig\\DefaultIcon"),0,0,0,KEY_WRITE,0,&hKey,0);
	wsprintf(szBuffer,_T("%s,2"),szModuleName);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)szBuffer,wcslen(szBuffer)*2);

	// Themes

	SHDeleteKey(HKEY_CLASSES_ROOT,_T(".kvt"));

	err=RegCreateKeyEx(HKEY_CLASSES_ROOT,_T(".kvt"),0,0,0,KEY_WRITE,0,&hKey,0);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)_T("KVIrcTheme"),11*2);


	SHDeleteKey(HKEY_CLASSES_ROOT,_T("KVIrcTheme"));
	RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("KVIrcTheme"),0,0,0,KEY_WRITE,0,&hKey,0);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)_T("KVIrc Theme Package"),wcslen(_T("KVIrc Theme Package"))*2);

	RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("KVIrcTheme\\DefaultIcon"),0,0,0,KEY_WRITE,0,&hKey,0);
	wsprintf(szBuffer,_T("%s,3"),szModuleName);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)szBuffer,wcslen(szBuffer)*2);

	RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("KVIrcTheme\\Shell\\Install"),0,0,0,KEY_WRITE,0,&hKey,0);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)_T("Install Theme Package"),wcslen(_T("Install Theme Package"))*2);

	RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("KVIrcTheme\\Shell\\Install\\command"),0,0,0,KEY_WRITE,0,&hKey,0);
	wsprintf(szBuffer,_T("%s \"%%1\""),szModuleName);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)szBuffer,wcslen(szBuffer)*2);

	//Addons

	SHDeleteKey(HKEY_CLASSES_ROOT,_T(".kva"));

	err=RegCreateKeyEx(HKEY_CLASSES_ROOT,_T(".kva"),0,0,0,KEY_WRITE,0,&hKey,0);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)_T("KVIrcAddon"),11*2);


	SHDeleteKey(HKEY_CLASSES_ROOT,_T("KVIrcAddon"));
	RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("KVIrcAddon"),0,0,0,KEY_WRITE,0,&hKey,0);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)_T("KVIrc Addon Package"),wcslen(_T("KVIrc Addon Package"))*2);

	RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("KVIrcAddon\\DefaultIcon"),0,0,0,KEY_WRITE,0,&hKey,0);
	wsprintf(szBuffer,_T("%s,4"),szModuleName);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)szBuffer,wcslen(szBuffer)*2);

	RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("KVIrcAddon\\Shell\\Install"),0,0,0,KEY_WRITE,0,&hKey,0);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)_T("Install Package"),wcslen(_T("Install Package"))*2);

	RegCreateKeyEx(HKEY_CLASSES_ROOT,_T("KVIrcAddon\\Shell\\Install\\command"),0,0,0,KEY_WRITE,0,&hKey,0);
	wsprintf(szBuffer,_T("%s \"%%1\""),szModuleName);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)szBuffer,wcslen(szBuffer)*2);

	//Path

	SHDeleteKey(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\KVIRC.EXE"));
	RegCreateKeyEx(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\KVIRC.EXE"),0,0,0,KEY_WRITE,0,&hKey,0);
	RegSetValueEx( hKey,0,0,REG_SZ,(LPBYTE)szModuleName,wcslen(szModuleName)*2);
	RegSetValueEx( hKey,_T("Path"),0,REG_SZ,(LPBYTE)szModulePath,wcslen(szModulePath)*2);

    return 0;
}

