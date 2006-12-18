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

#include <qlistview.h>


class KviScriptEditor;

class KviLogListViewItem : public QListViewItem
{
public:
	KviLogListViewItem(QListViewItem * par, const QString & typeName, const QString & name)
	: QListViewItem(par), m_szTypeName(typeName), m_szName(name) {};
	KviLogListViewItem(QListView * par, const QString & typeName, const QString & name)
	: QListViewItem(par), m_szTypeName(typeName), m_szName(name) {};
	~KviLogListViewItem() {};
public:
	QString m_szTypeName;
	QString m_szName;
	virtual QString fileName(int col) const { return QString::null; };
	virtual QString text(int col) const { return m_szName; };
};


class KviLogListViewItemType : public KviLogListViewItem
{
public:
	KviLogListViewItemType(QListView * par, const QString & typeName)
	: KviLogListViewItem(par,typeName,typeName) {};
	~KviLogListViewItemType() {};
public:
	virtual const QPixmap * pixmap(int col) const;
};


class KviLogListViewLog : public KviLogListViewItem
{
public:
	KviLogListViewLog(QListViewItem * par,const QString & date, const QString & fileName, const QString & owner)
	: KviLogListViewItem(par,"",date), m_szFileName(fileName), m_szOwner(owner) {};
	~KviLogListViewLog() {};
public:
	QString m_szFileName;
	QString m_szOwner;
	virtual QString fileName(int col) const { return m_szFileName; };
	virtual QString owner(int col) const { return m_szOwner; };
};


#endif

