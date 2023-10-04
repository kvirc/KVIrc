//=============================================================================
//
//   File : KviShortcut.cpp
//   Creation date : Mon Aug 22 2011 16:24:46 CEST by Fabio Bas
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2011 Fabio Bas (ctrlaltca at gmail dot com)
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

#include "KviShortcut.h"

#include <QShortcut>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
QShortcut * KviShortcut::create(const char * key, QWidget * parent, const char * member, const char * ambiguousMember, Qt::ShortcutContext context)
#else
QShortcut * KviShortcut::create(const char * key, QObject * parent, const char * member, const char * ambiguousMember, Qt::ShortcutContext context)
#endif
{
	//qDebug("New Plain Shortcut %s\n", QKeySequence(key).toString().toUtf8().data());
	return new QShortcut(QKeySequence(key), parent, member, ambiguousMember, context);
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
QShortcut * KviShortcut::create(const QKeySequence & key, QWidget * parent, const char * member, const char * ambiguousMember, Qt::ShortcutContext context)
#else
QShortcut * KviShortcut::create(const QKeySequence & key, QObject * parent, const char * member, const char * ambiguousMember, Qt::ShortcutContext context)
#endif
{
	//qDebug("New KeySe Shortcut %s\n", key.toString().toUtf8().data());
	return new QShortcut(key, parent, member, ambiguousMember, context);
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
QShortcut * KviShortcut::create(Qt::KeyboardModifier mod, Qt::Key key, QWidget * parent, const char * member, const char * ambiguousMember, Qt::ShortcutContext context)
#else
QShortcut * KviShortcut::create(Qt::KeyboardModifier mod, Qt::Key key, QObject * parent, const char * member, const char * ambiguousMember, Qt::ShortcutContext context)
#endif
{
	//qDebug("New KeySe Shortcut %s\n", key.toString().toUtf8().data());
	return new QShortcut(QKeySequence(mod, key), parent, member, ambiguousMember, context);
}
