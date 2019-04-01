#ifndef _TC_STATUSBARAPPLET_H_
#define _TC_STATUSBARAPPLET_H_
//=============================================================================
//
//   StatusBar for Torrent client.
//
//   File : StatusBarApplet.h
//   Creation date : Fri Jan 1 15:42:25 2007 GMT by Alexander Stillich
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2007-2008 Alexander Stillich (torque at pltn dot org)
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

#include "KviStatusBar.h"
#include "KviStatusBarApplet.h"

// TODO: check if it gets destroyed
// TODO: proper icon!
class StatusBarApplet : public KviStatusBarApplet
{
	Q_OBJECT
public:
	StatusBarApplet(KviStatusBar * parent, KviStatusBarAppletDescriptor * desc);
	~StatusBarApplet() override;
	static void selfRegister(KviStatusBar * bar);
private slots:
	void update();
};

#endif // _TC_STATUSBARAPPLET_H_
