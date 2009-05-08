//=============================================================================
//
//   File : kvi_dbusadaptor.cpp
//   Creation date : Thu May 08 2008 21:41:45 by Voker57
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2008 Voker57 (voker57 at gmail dot com)
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

#include "kvi_dbusadaptor.h"

#ifdef COMPILE_DBUS_SUPPORT

KviDbusAdaptor::KviDbusAdaptor(QObject * obj) : QDBusAbstractAdaptor(obj)
{
}

#endif

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "kvi_dbusadaptor.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
