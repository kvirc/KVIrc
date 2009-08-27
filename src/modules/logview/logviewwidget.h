#ifndef _LOGVIEWWIDGET_H_
#define _LOGVIEWWIDGET_H_
//=============================================================================
//
//   File : logviewwidget.h
//   Creation date : Thue Apr 23 2002 03:01:53 CET by Juanjo Alvarez
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Juanjo Alvarez
//   Copyright (C) 2002-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "logviewmdiwindow.h"

#include "kvi_window.h"
#include "kvi_scripteditor.h"
#include "kvi_tal_treewidget.h"

class KviScriptEditor;

class KviLogListViewItem : public KviTalTreeWidgetItem
{
public:
	KviLogListViewItem(KviTalTreeWidgetItem * par, KviLogFile::KviLogTypes type, KviLogFile * fileData);
	KviLogListViewItem(KviTalTreeWidget * par, KviLogFile::KviLogTypes type, KviLogFile * fileData);
	~KviLogListViewItem() {};
public:
	KviLogFile::KviLogTypes m_type;
	KviLogFile*             m_pFileData;
	virtual QString fileName() const { return QString(); };
};

class KviLogListViewItemFolder : public KviLogListViewItem
{
public:
	KviLogListViewItemFolder(KviTalTreeWidgetItem * par, const QString& label);
	~KviLogListViewItemFolder() {};
public:
};

class KviLogListViewItemType : public KviLogListViewItem
{
public:
	KviLogListViewItemType(KviTalTreeWidget * par, KviLogFile::KviLogTypes type);
	~KviLogListViewItemType() {};
};


class KviLogListViewLog : public KviLogListViewItem
{
public:
	KviLogListViewLog(KviTalTreeWidgetItem * par, KviLogFile::KviLogTypes type, KviLogFile * fileData);
	~KviLogListViewLog() {};
	virtual QString fileName() const { return m_pFileData->fileName(); };
protected:
	bool operator<(const QTreeWidgetItem &other)const
	{
		return m_pFileData->date() < ((KviLogListViewLog*)&other)->m_pFileData->date();
	}
};

#endif
