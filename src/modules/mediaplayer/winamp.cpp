//=============================================================================
//
//   File : winamp.cpp
//   Creation date : Tue Nov  6 23:01:12 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your option) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include <windows.h>
#include <process.h>

//
// This is a winamp-kvirc interface plugin
//

// This stuff is compiled only on windows, as a separate dll module

struct winampGeneralPurposePlugin
{
	int version;
	char * description;
	int (*init)();
	void (*config)();
	void (*quit)();
	HWND hwndParent;
	HINSTANCE hDllInstance;
};

#define GPPHDR_VER 0x10

extern winampGeneralPurposePlugin * gen_plugins[256];
typedef winampGeneralPurposePlugin * (*winampGeneralPurposePluginGetter)();

int init();
void quit();
void config();

winampGeneralPurposePlugin plugin = {
	GPPHDR_VER,
	"KVIrc interface plugin 1.0",
	init,
	config,
	quit,
};

#define KVIRC_WM_USER 63112

#define KVIRC_WM_USER_GETTITLE 5000
#define KVIRC_WM_USER_GETFILE 10000
#define KVIRC_WM_USER_TRANSFER 15000

#define IPC_GETPLAYLISTFILE 211
#define IPC_GETPLAYLISTTITLE 212
#define IPC_GETLISTPOS 125

#define KVIRC_WM_USER_CHECK 13123
#define KVIRC_WM_USER_CHECK_REPLY 13124

void * lpWndProcOld = nullptr;

char szBuffer[4096];

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_USER)
	{
		if(wParam == KVIRC_WM_USER)
		{
			if(lParam == KVIRC_WM_USER_CHECK)
			{
				return KVIRC_WM_USER_CHECK_REPLY;
			}
			if(lParam == KVIRC_WM_USER_GETTITLE)
			{
				int idx = (int)SendMessage(hwnd, WM_USER, 0, IPC_GETLISTPOS);
				if(idx != -1)
				{
					char * szTitle = (char *)SendMessage(hwnd, WM_USER, idx, IPC_GETPLAYLISTTITLE);
					strcpy(szBuffer, szTitle);
					return strlen(szBuffer);
				}
			}
			else if(lParam == KVIRC_WM_USER_GETFILE)
			{
				int idx = (int)SendMessage(hwnd, WM_USER, 0, IPC_GETLISTPOS);
				if(idx != -1)
				{
					char * szTitle = (char *)SendMessage(hwnd, WM_USER, idx, IPC_GETPLAYLISTFILE);
					strcpy(szBuffer, szTitle);
					return strlen(szBuffer);
				}
			}
			else if((lParam >= KVIRC_WM_USER_TRANSFER) && (lParam < (KVIRC_WM_USER_TRANSFER + 4096)))
			{
				return (LRESULT)szBuffer[lParam - KVIRC_WM_USER_TRANSFER];
			}
		}
	}
	return CallWindowProc((WNDPROC)lpWndProcOld, hwnd, message, wParam, lParam);
}

int init()
{
	lpWndProcOld = (void *)GetWindowLong(plugin.hwndParent, GWLP_WNDPROC);
	SetWindowLongPtr(plugin.hwndParent, GWLP_WNDPROC, (intptr_t)WndProc);
	return 0;
}

void quit()
{
}

void config()
{
}

extern "C" __declspec(dllexport) winampGeneralPurposePlugin * winampGetGeneralPurposePlugin()
{
	return &plugin;
}
