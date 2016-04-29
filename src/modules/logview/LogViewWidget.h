#ifndef _LOGVIEWWIDGET_H_
#define _LOGVIEWWIDGET_H_
//=============================================================================
//
//   File : LogViewWidget.h
//   Creation date : Thue Apr 23 2002 03:01:53 CET by Juanjo Alvarez
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002 Juanjo Alvarez
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "LogViewWindow.h"

#include "KviWindow.h"

#include <QTreeWidget>

class LogListViewItem : public QTreeWidgetItem
{
public:
	LogListViewItem(QTreeWidgetItem * pPar, LogFile::Type eType, LogFile * pLog);
	LogListViewItem(QTreeWidget * pPar, LogFile::Type eType, LogFile * pLog);
	~LogListViewItem(){};

public:
	LogFile::Type m_eType;
	LogFile * m_pFileData;

public:
	LogFile * log() { return m_pFileData; };
	virtual QString fileName() const { return QString(); };
};

class LogListViewItemFolder : public LogListViewItem
{
public:
	LogListViewItemFolder(QTreeWidgetItem * pPar, const QString & szLabel);
	~LogListViewItemFolder(){};

public:
};

class LogListViewItemType : public LogListViewItem
{
public:
	LogListViewItemType(QTreeWidget * pPar, LogFile::Type eType);
	~LogListViewItemType(){};
};

class LogListViewLog : public LogListViewItem
{
public:
	LogListViewLog(QTreeWidgetItem * pPar, LogFile::Type eType, LogFile * pLog);
	~LogListViewLog(){};
	virtual QString fileName() const { return m_pFileData->fileName(); };
protected:
	bool operator<(const QTreeWidgetItem & other) const
	{
		return m_pFileData->date() < ((LogListViewLog *)&other)->m_pFileData->date();
	}
};

#endif
