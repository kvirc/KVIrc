/*
 * Idle_win.cpp - detect desktop idle time
 * Copyright (C) 2003  Justin Karneges
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)

#include "Idle.h"

#include <QLibrary>

struct LASTINPUTINFO
{
	UINT cbSize;
	DWORD dwTime;
};
using PLASTINPUTINFO = LASTINPUTINFO *;

class IdlePlatform::Private
{
public:
	Private()
	{
		GetLastInputInfo = 0;
		IdleUIGetLastInputTime = 0;
		lib = 0;
	}

	BOOL(__stdcall * GetLastInputInfo)
	(PLASTINPUTINFO);
	DWORD(__stdcall * IdleUIGetLastInputTime)
	(void);
	QLibrary * lib;
};

IdlePlatform::IdlePlatform()
{
	d = new Private;
}

IdlePlatform::~IdlePlatform()
{
	delete d->lib;
	delete d;
}

bool IdlePlatform::init()
{
	if(d->lib)
		return true;
	void * p;

	// try to find the built-in Windows 2000 function
	d->lib = new QLibrary("user32");
	if(d->lib->load() && (p = d->lib->resolve("GetLastInputInfo")))
	{
		d->GetLastInputInfo = (BOOL(__stdcall *)(PLASTINPUTINFO))p;
		return true;
	}
	else
	{
		delete d->lib;
		d->lib = 0;
	}

	// fall back on idleui
	d->lib = new QLibrary("idleui");
	if(d->lib->load() && (p = d->lib->resolve("IdleUIGetLastInputTime")))
	{
		d->IdleUIGetLastInputTime = (DWORD(__stdcall *)(void))p;
		return true;
	}
	else
	{
		delete d->lib;
		d->lib = 0;
	}

	return false;
}

int IdlePlatform::secondsIdle()
{
	int i;
	if(d->GetLastInputInfo)
	{
		LASTINPUTINFO li;
		li.cbSize = sizeof(LASTINPUTINFO);
		bool ok = d->GetLastInputInfo(&li);
		if(!ok)
			return 0;
		i = li.dwTime;
	}
	else if(d->IdleUIGetLastInputTime)
	{
		i = d->IdleUIGetLastInputTime();
	}
	else
		return 0;

	return (GetTickCount() - i) / 1000;
}

#endif
