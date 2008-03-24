#ifndef _LOGVIEWWIDGET_H_
#define _LOGVIEWWIDGET_H_

//
//   File : logviewwidget.h
//   Creation date : Thue Apr 23 2002 03:01:53 CET by Juanjo �varez
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2003 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_window.h"
#include "kvi_scripteditor.h"
#include "logviewmdiwindow.h"

#include "kvi_tal_listview.h"


class KviScriptEditor;

class KviLogListViewItem : public KviTalListViewItem
{
public:
	KviLogListViewItem(KviTalListViewItem * par, KviLogFile::KviLogTypes type, KviLogFile * fileData)
	: KviTalListViewItem(par), m_type(type), m_pFileData(fileData) {};
	KviLogListViewItem(KviTalListView * par, KviLogFile::KviLogTypes type, KviLogFile * fileData)
	: KviTalListViewItem(par), m_type(type), m_pFileData(fileData) {};
	~KviLogListViewItem() {};
public:
	KviLogFile::KviLogTypes m_type;
	KviLogFile*             m_pFileData;

	virtual QString fileName(int col) const { return m_pFileData ? m_pFileData->name() : QString::null; };
	virtual QString text(int col) const { return m_pFileData ? m_pFileData->name() : QString::null; };
};

class KviLogListViewItemFolder : public KviLogListViewItem
{
public:
	KviLogListViewItemFolder(KviTalListViewItem * par, const QString& label)
	: KviLogListViewItem(par,KviLogFile::Other,0),m_szLabel(label) {};
	~KviLogListViewItemFolder() {};
public:
	QString m_szLabel;
	virtual QString text(int col) const { return m_szLabel; };
};

class KviLogListViewItemType : public KviLogListViewItem
{
public:
	KviLogListViewItemType(KviTalListView * par, KviLogFile::KviLogTypes type)
	: KviLogListViewItem(par,type,0) {};
	~KviLogListViewItemType() {};
public:
	virtual const QPixmap * pixmap(int col) const;
	virtual QString text(int col) const;
};


class KviLogListViewLog : public KviLogListViewItem
{
public:
	KviLogListViewLog(KviTalListViewItem * par, KviLogFile::KviLogTypes type, KviLogFile * fileData)
	: KviLogListViewItem(par,type,fileData){};
	~KviLogListViewLog() {};
	virtual QString key ( int column, bool ascending ) const { return m_pFileData->date().toString("yyyy.MM.dd"); };
	virtual QString text(int col) const { return m_pFileData->date().toString("dd.MM.yyyy"); };
};


#endif

