//=============================================================================
//
//   File : LogViewWidget.cpp
//   Creation date : Thu Apr 23 2002 17:42:12 by Juanjo Alvarez
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2002 Juanjo Alvarez
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "LogViewWidget.h"

//#include "kvi_debug.h"
#include "KviLocale.h"
#include "KviIconManager.h"
#include "KviApplication.h"
#include "KviQString.h"

#include <QLayout>
#include <QDir>
#include <QTextCodec>

#ifdef COMPILE_ZLIB_SUPPORT
	#include <zlib.h>
#endif

LogListViewItem::LogListViewItem(QTreeWidgetItem * par, LogFile::KviLogTypes type, LogFile * fileData)
: QTreeWidgetItem(par), m_type(type), m_pFileData(fileData)
{
	setText(0,m_pFileData ? m_pFileData->name() : QString());
}

LogListViewItem::LogListViewItem(QTreeWidget * par, LogFile::KviLogTypes type, LogFile * fileData)
: QTreeWidgetItem(par), m_type(type), m_pFileData(fileData)
{
	setText(0,m_pFileData ? m_pFileData->name() : QString());
}

LogListViewItemFolder::LogListViewItemFolder(QTreeWidgetItem * par, const QString& label)
: LogListViewItem(par,LogFile::Other,0)
{
	setText(0,label);
}

LogListViewItemType::LogListViewItemType(QTreeWidget * par, LogFile::KviLogTypes type)
: LogListViewItem(par,type,0)
{
	QIcon icon;
	QString text;

	switch(m_type)
	{
		case LogFile::Channel:
			icon= QIcon(*g_pIconManager->getSmallIcon(KviIconManager::Channel));
			text = __tr2qs_ctx("Channel","logview");
			break;
		case LogFile::Query:
			icon= QIcon(*g_pIconManager->getSmallIcon(KviIconManager::Query));
			text = __tr2qs_ctx("Query","logview");
			break;
		case LogFile::DccChat:
			icon= QIcon(*g_pIconManager->getSmallIcon(KviIconManager::DccMsg));
			text = __tr2qs_ctx("DCC Chat","logview");
			break;
		case LogFile::Console:
			icon= QIcon(*g_pIconManager->getSmallIcon(KviIconManager::Console));
			text = __tr2qs_ctx("Console","logview");
			break;
		default:
			icon= QIcon(*g_pIconManager->getSmallIcon(KviIconManager::Help));
			text = __tr2qs_ctx("Other","logview");
			break;
	}

	setIcon(0, icon);
	setText(0, text);
}

LogListViewLog::LogListViewLog(QTreeWidgetItem * par, LogFile::KviLogTypes type, LogFile * fileData)
: LogListViewItem(par,type,fileData)
{
	setText(0, m_pFileData->date().toString("yyyy-MM-dd"));
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "LogViewWidget.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
