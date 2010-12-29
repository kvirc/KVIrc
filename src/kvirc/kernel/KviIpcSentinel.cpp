//=============================================================================
//
//   File : KviIpcSentinel.hcpp
//   Creation date : Tue Apr 10 2001 15:04:45 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================



#include <qmetatype.h>
#include "KviIpcSentinel.h"


#ifndef COMPILE_NO_IPC

	#include "KviMemory.h"
	#include "KviApplication.h"
	#include "KviQString.h"

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)

	static HWND kvi_win_findIpcSentinel()
	{
		HWND hWnd = ::FindWindow("QWidget","[Non-Commercial] - kvirc4_ipc_sentinel");
		if(hWnd)return hWnd;
		hWnd = ::FindWindow("QWidget","[Freeware] - kvirc4_ipc_sentinel");
		if(hWnd)return hWnd;
		hWnd = ::FindWindow("QWidget","kvirc4_ipc_sentinel");
		return hWnd;
	}

#else

	#ifdef COMPILE_X11_SUPPORT
		#include <X11/Xatom.h>

		#include <unistd.h>    // for getuid
		#include <sys/types.h> // for getuid

		//#include <qx11info_x11.h>
		#include <QX11Info>
		#define kvi_ipc_get_xdisplay QX11Info::display
		#define kvi_ipc_get_xrootwin QX11Info::appRootWindow

		static Atom kvi_atom_ipc_sentinel_window;
		static Atom kvi_atom_ipc_remote_command;
		static Atom kvi_atom_ipc_remote_message;
		static KviCString kvi_sentinel_id;
		static bool g_bIpcAtomsLoaded = false;

		static void kvi_ipcLoadAtoms()
		{
			if(g_bIpcAtomsLoaded)return;
			g_bIpcAtomsLoaded = true;
			kvi_sentinel_id.sprintf("pane tonno e mistero - %d",getuid());
			kvi_atom_ipc_sentinel_window = XInternAtom(kvi_ipc_get_xdisplay(),"XA_KVI4_IPC_SENTINEL_WINDOW",False);
			kvi_atom_ipc_remote_command = XInternAtom(kvi_ipc_get_xdisplay(),"XA_KVI4_IPC_REMOTE_COMMAND",False);
			kvi_atom_ipc_remote_message = XInternAtom(kvi_ipc_get_xdisplay(),"XA_KVI4_IPC_REMOTE_MESSAGE",False);
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

#endif


	#define KVI_WINDOWS_IPC_MESSAGE 0x2FACE5

	bool kvi_sendIpcMessage(const char * message)
	{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)

		HWND hSentinel = kvi_win_findIpcSentinel();
		if(hSentinel != NULL)
		{
			COPYDATASTRUCT cpd;
			cpd.cbData = strlen(message)+1;
			cpd.dwData = KVI_WINDOWS_IPC_MESSAGE;
			cpd.lpData = (void *)message;
			DWORD dwResult;
			//qDebug(message);
#if defined(COMPILE_ON_WINDOWS) && !(defined(MINGW))
			if(!::SendMessageTimeout(hSentinel,WM_COPYDATA,(WPARAM)NULL,(LPARAM)&cpd,SMTO_BLOCK,1000,(PDWORD_PTR)&dwResult))
#else
			if(!::SendMessageTimeout(hSentinel,WM_COPYDATA,(WPARAM)NULL,(LPARAM)&cpd,SMTO_BLOCK,1000,&dwResult))
#endif
			{
				qDebug("Failed to send IPC message: error code 0x%x",::GetLastError());
			}
			return true;
		}
#else
	#ifdef COMPILE_X11_SUPPORT

		kvi_ipcLoadAtoms();

		Window sentinel = kvi_x11_findIpcSentinel(kvi_ipc_get_xrootwin());
		if(sentinel != 0)
		{
//			XChangeProperty(kvi_ipc_get_xdisplay(),sentinel,kvi_atom_ipc_remote_command,XA_STRING,8,
//				PropModeReplace,(const unsigned char *)message,kvi_strLen(message));
			kvi_ipcSetRemoteCommand(sentinel,message);

			XEvent e;
			KviMemory::set(&e,0,sizeof(XEvent));
			e.type = ClientMessage;
			e.xclient.display = kvi_ipc_get_xdisplay();
			e.xclient.window = sentinel;
			e.xclient.message_type = kvi_atom_ipc_remote_message;
			e.xclient.format = 8;

			XSendEvent(kvi_ipc_get_xdisplay(),sentinel,False,0,&e);

			return true;
		}
	#endif //!COMPILE_NO_X
#endif
		return false;
	}


	//////////////////////////////////////////////////////////////////////
	// class KviIpcSentinel
	//
	//    hidden sentinel of ipc messages
	//
	//////////////////////////////////////////////////////////////////////

	KviIpcSentinel::KviIpcSentinel()
	: QWidget(0)
	{
		setObjectName("kvirc4_ipc_sentinel");
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		setWindowTitle("kvirc4_ipc_sentinel");
		setWindowFlags(Qt::FramelessWindowHint);
#else
	#ifdef COMPILE_X11_SUPPORT
		kvi_ipcLoadAtoms();

		XChangeProperty(kvi_ipc_get_xdisplay(),winId(),kvi_atom_ipc_sentinel_window,XA_STRING,8,
			PropModeReplace,(const unsigned char *)kvi_sentinel_id.ptr(),kvi_sentinel_id.len());

		kvi_ipcSetRemoteCommand(winId(),"");
	#endif //!COMPILE_NO_X
#endif

		move(-50,-50);
		resize(1,1);
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		// we need to show the window once otherwise it will never get "realized"
		// and we will not be able to find it via FindWindow()
		show();
#endif
		hide();

	}

	KviIpcSentinel::~KviIpcSentinel()
	{
	}

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)

	bool KviIpcSentinel::winEvent(MSG * msg, long * result)
	{
		if(msg->message == WM_COPYDATA)
		{
			COPYDATASTRUCT * cpd = (COPYDATASTRUCT *)msg->lParam;
			if(cpd)
			{
				if(cpd->dwData == KVI_WINDOWS_IPC_MESSAGE)
				{
					qDebug((char *)(cpd->lpData));
					if(g_pApp)
						g_pApp->ipcMessage((char *)(cpd->lpData));
					if(result)
						*result = 0;
					return true;
				}
			}
		}
		return false;
	}

#else
	#ifdef COMPILE_X11_SUPPORT
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
					KviCString szData;
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
#endif
	#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
		#include "KviIpcSentinel.moc"
	#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

#endif
