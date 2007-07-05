//=============================================================================
//
//   File : kvi_ipc.hcpp
//   Creation date : Tue Apr 10 2001 15:04:45 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2007 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your opinion) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#define __KVIRC__


#include "kvi_ipc.h"


#ifndef COMPILE_NO_IPC

	#include "kvi_memmove.h"
	#include "kvi_app.h"
	#include "kvi_qstring.h"

#ifdef COMPILE_ON_WINDOWS

	static HWND kvi_win_findIpcSentinel()
	{
		HWND hWnd = ::FindWindow("QWidget","[Non-Commercial] - kvirc_ipc_sentinel");
		if(hWnd)return hWnd;
		hWnd = ::FindWindow("QWidget","[Freeware] - kvirc_ipc_sentinel");
		if(hWnd)return hWnd;
		hWnd = ::FindWindow("QWidget","kvirc_ipc_sentinel");
		return hWnd;
	}

#else //!COMPILE_ON_WINDOWS

	#ifndef COMPILE_NO_X
		#include <X11/Xatom.h>

		#include <unistd.h>    // for getuid
		#include <sys/types.h> // for getuid

		#ifdef COMPILE_USE_QT4
			#include <qx11info_x11.h>
			#define kvi_ipc_get_xdisplay QX11Info::display
			#define kvi_ipc_get_xrootwin QX11Info::appRootWindow
		#else
			#define kvi_ipc_get_xdisplay qt_xdisplay
			#define kvi_ipc_get_xrootwin qt_xrootwin
		#endif

		static Atom kvi_atom_ipc_sentinel_window;
		static Atom kvi_atom_ipc_remote_command;
		static Atom kvi_atom_ipc_remote_message;
		static KviStr kvi_sentinel_id;
		static bool g_bIpcAtomsLoaded = false;
	
		static void kvi_ipcLoadAtoms()
		{
			if(g_bIpcAtomsLoaded)return;
			g_bIpcAtomsLoaded = true;
			kvi_sentinel_id.sprintf("tonno e mistero - %d",getuid());
			kvi_atom_ipc_sentinel_window = XInternAtom(kvi_ipc_get_xdisplay(),"XA_KVI_IPC_SENTINEL_WINDOW",False);
			kvi_atom_ipc_remote_command = XInternAtom(kvi_ipc_get_xdisplay(),"XA_KVI_IPC_REMOTE_COMMAND",False);
			kvi_atom_ipc_remote_message = XInternAtom(kvi_ipc_get_xdisplay(),"XA_KVI_IPC_REMOTE_MESSAGE",False);
		}
	
		static void kvi_ipcSetRemoteCommand(Window w,const char * command)
		{
			XChangeProperty(kvi_ipc_get_xdisplay(),w,kvi_atom_ipc_remote_command,
				XA_STRING,8,PropModeReplace,(const unsigned char *)command,kvi_strLen(command) + 1);
		}
	
		static Window kvi_x11_findIpcSentinel(Window win)
		{
			Atom type;
			int format;
			unsigned long nItems,after;
			unsigned char * data = 0;
			if(XGetWindowProperty(kvi_ipc_get_xdisplay(),win,kvi_atom_ipc_sentinel_window,0,32,FALSE,XA_STRING,
				&type,&format,&nItems,&after,&data) == Success)
			{
				if((type == XA_STRING) && (format == 8))
				{
					if((nItems == ((unsigned long)(kvi_sentinel_id.len()))) && data)
					{
						if(kvi_strEqualCSN((const char *)data,kvi_sentinel_id.ptr(),kvi_sentinel_id.len()))
						{
							XFree((char *)data);
							return win;
						}
					}
				}
			}
	
			Window root,parent;
			Window * children;
			unsigned int nChildren;
	
			if(!XQueryTree(kvi_ipc_get_xdisplay(),win,&root,&parent,&children,&nChildren))
			{
				if(children)XFree((char *)children);
				return 0;
			}
	
			Window found = 0;
	
		    for(int i=nChildren-1;(!found) && (i >= 0);i--)
				found = kvi_x11_findIpcSentinel(children[i]);
	
	 		if(children)XFree((char *)children);
	
			return found;
		}
	#endif //!COMPILE_NO_X
	
#endif //!COMPILE_ON_WINDOWS


	#define KVI_WINDOWS_IPC_MESSAGE 0x2FACE5

	bool kvi_sendIpcMessage(const char * message)
	{
#ifdef COMPILE_ON_WINDOWS
		HWND hSentinel = kvi_win_findIpcSentinel();
		if(hSentinel != NULL)
		{
			COPYDATASTRUCT cpd;
			cpd.cbData = strlen(message)+1;
			cpd.dwData = KVI_WINDOWS_IPC_MESSAGE;
			cpd.lpData = (void *)message;
			DWORD dwResult;
			debug(message);
			::SendMessageTimeout(hSentinel,WM_COPYDATA,(WPARAM)NULL,(LPARAM)&cpd,SMTO_BLOCK,1000,&dwResult);
			return true;
		}
#else //!COMPILE_ON_WINDOWS
	#ifndef COMPILE_NO_X
	
		kvi_ipcLoadAtoms();

		Window sentinel = kvi_x11_findIpcSentinel(kvi_ipc_get_xrootwin());
		if(sentinel != 0)
		{
//			XChangeProperty(kvi_ipc_get_xdisplay(),sentinel,kvi_atom_ipc_remote_command,XA_STRING,8,
//				PropModeReplace,(const unsigned char *)message,kvi_strLen(message));
			kvi_ipcSetRemoteCommand(sentinel,message);

			XEvent e;
			kvi_memset(&e,0,sizeof(XEvent));
			e.type = ClientMessage;
			e.xclient.display = kvi_ipc_get_xdisplay();
			e.xclient.window = sentinel;
			e.xclient.message_type = kvi_atom_ipc_remote_message;
			e.xclient.format = 8;

			XSendEvent(kvi_ipc_get_xdisplay(),sentinel,False,0,&e);

			return true;
		}
	#endif //!COMPILE_NO_X
#endif //!COMPILE_ON_WINDOWS
		return false;
	}


	//////////////////////////////////////////////////////////////////////
	// class KviIpcSentinel
	//
	//    hidden sentinel of ipc messages
	//
	//////////////////////////////////////////////////////////////////////

	KviIpcSentinel::KviIpcSentinel()
	: QWidget(0,"kvirc_ipc_sentinel")
	{
#ifdef COMPILE_ON_WINDOWS
		setCaption("kvirc_ipc_sentinel");
#else //!COMPILE_ON_WINDOWS
	#ifndef COMPILE_NO_X
		kvi_ipcLoadAtoms();

		XChangeProperty(kvi_ipc_get_xdisplay(),winId(),kvi_atom_ipc_sentinel_window,XA_STRING,8,
			PropModeReplace,(const unsigned char *)kvi_sentinel_id.ptr(),kvi_sentinel_id.len());

		kvi_ipcSetRemoteCommand(winId(),"");
	#endif //!COMPILE_NO_X
#endif //!COMPILE_ON_WINDOWS

		move(-50,-50);
		resize(1,1);
		hide();

	}
	
	KviIpcSentinel::~KviIpcSentinel()
	{
	}

#ifdef COMPILE_ON_WINDOWS

	bool KviIpcSentinel::winEvent(MSG * msg)
	{
		if(msg->message == WM_COPYDATA)
		{
			COPYDATASTRUCT * cpd = (COPYDATASTRUCT *)msg->lParam;
			if(cpd)
			{
				if(cpd->dwData == KVI_WINDOWS_IPC_MESSAGE)
				{
					debug((char *)(cpd->lpData));
					if(g_pApp)g_pApp->ipcMessage((char *)(cpd->lpData));
					return true;
				}
			}
		}
		return false;
	}

#else //!COMPILE_ON_WINDOWS
	#ifndef COMPILE_NO_X
		bool KviIpcSentinel::x11Event(XEvent *e)
		{
			if(e->type == ClientMessage)
			{
				if((e->xclient.message_type = kvi_atom_ipc_remote_message) && (e->xclient.format == 8))
				{
					Atom type;
					int format;
					unsigned long nItems,after;
					unsigned char * data = 0;
					KviStr szData;
					if(XGetWindowProperty(kvi_ipc_get_xdisplay(),winId(),kvi_atom_ipc_remote_command,0,1024,FALSE,XA_STRING,
						&type,&format,&nItems,&after,&data) == Success)
					{
						if((type == XA_STRING) && (format == 8) && (nItems > 0) && data)
						{
							szData = (char *)data;
							XFree((char *)data);
						}
					}
					kvi_ipcSetRemoteCommand(winId(),"");
	
					if(g_pApp)g_pApp->ipcMessage(szData.ptr());
					return true;
				}
			}
			return false;
		}
	#endif //!COMPILE_NO_X
#endif //!COMPILE_ON_WINDOWS

	#include "kvi_ipc.moc"

#endif
