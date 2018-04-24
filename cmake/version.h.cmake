//=============================================================================
//
//   File : version.h
//   Creation date : Unknown by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2009 Szymon Stefanek (pragma at kvirc dot net)
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

#ifndef VERSION_H
#define VERSION_H

#define VER_FILEVERSION @CMAKE_KVIRC_VERSION_MAJOR@,@CMAKE_KVIRC_VERSION_MINOR@, @CMAKE_KVIRC_VERSION_MICRO@,0
#define VER_FILEVERSION_STR "@CMAKE_KVIRC_VERSION_RELEASE@.0\0"

#define VER_PRODUCTVERSION VER_FILEVERSION
#define VER_PRODUCTVERSION_STR VER_FILEVERSION_STR

#define IDD_ICON2 101
#define IDD_ICON3 102
#define IDD_ICON4 103
#define IDD_ICON5 104

#define VER_COMPANYNAME_STR "KVIrc Development Team"
#define VER_FILEDESCRIPTION_STR "KVIrc"
#define VER_INTERNALNAME_STR "KVIrc"
#define VER_LEGALCOPYRIGHT_STR "Copyright © 2016"
#define VER_ORIGINALFILENAME_STR "kvirc.exe"
#define VER_PRODUCTNAME_STR "K Visual IRC Client"

#define VER_COMPANYDOMAIN_STR "kvirc.net"

#endif // VERSION_H
