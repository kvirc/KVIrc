#ifndef _KVI_SHORTCUT_CLASSFILE_H_
#define _KVI_SHORTCUT_CLASSFILE_H_
//=============================================================================
//
//   File : KviShortcut.h
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

#include "kvi_shortcuts.h"

#include "KviPointerList.h"

#include <QKeySequence>

class QShortcut;
class QWidget;

class KVILIB_API KviShortcut
{
private:
	KviShortcut();
	~KviShortcut();

public:
	static QShortcut * create(const char * key, QWidget * parent, const char * member = nullptr, const char * ambiguousMember = nullptr, Qt::ShortcutContext context = Qt::WindowShortcut);
	static QShortcut * create(const QKeySequence & key, QWidget * parent, const char * member = nullptr, const char * ambiguousMember = nullptr, Qt::ShortcutContext context = Qt::WindowShortcut);
	static void create(QKeySequence::StandardKey key, QWidget * parent, const char * member = nullptr, const char * ambiguousMember = nullptr, Qt::ShortcutContext context = Qt::WindowShortcut, KviPointerList<QShortcut> * pBufferList = nullptr);
};

#endif //_KVI_SHORTCUT_CLASSFILE_H_
