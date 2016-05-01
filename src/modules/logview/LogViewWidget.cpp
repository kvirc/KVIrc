//=============================================================================
//
//   File : LogViewWidget.cpp
//   Creation date : Thu Apr 23 2002 17:42:12 by Juanjo Alvarez
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

#include "LogViewWidget.h"

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

LogListViewItem::LogListViewItem(QTreeWidgetItem * pPar, LogFile::Type eType, LogFile * pLog)
    : QTreeWidgetItem(pPar), m_eType(eType), m_pFileData(pLog)
{
	setText(0, m_pFileData ? m_pFileData->name() : QString());
}

LogListViewItem::LogListViewItem(QTreeWidget * pPar, LogFile::Type eType, LogFile * pLog)
    : QTreeWidgetItem(pPar), m_eType(eType), m_pFileData(pLog)
{
	setText(0, m_pFileData ? m_pFileData->name() : QString());
}

LogListViewItemFolder::LogListViewItemFolder(QTreeWidgetItem * pPar, const QString & szLabel)
    : LogListViewItem(pPar, LogFile::Other, nullptr)
{
	setText(0, szLabel);
}

LogListViewItemType::LogListViewItemType(QTreeWidget * pPar, LogFile::Type eType)
    : LogListViewItem(pPar, eType, nullptr)
{
	QIcon icon;
	QString szText;

	switch(m_eType)
	{
		case LogFile::Channel:
			icon = QIcon(*g_pIconManager->getSmallIcon(KviIconManager::Channel));
			szText = __tr2qs_ctx("Channel", "log");
			break;
		case LogFile::Query:
			icon = QIcon(*g_pIconManager->getSmallIcon(KviIconManager::Query));
			szText = __tr2qs_ctx("Query", "log");
			break;
		case LogFile::DccChat:
			icon = QIcon(*g_pIconManager->getSmallIcon(KviIconManager::DccMsg));
			szText = __tr2qs_ctx("DCC Chat", "log");
			break;
		case LogFile::Console:
			icon = QIcon(*g_pIconManager->getSmallIcon(KviIconManager::Console));
			szText = __tr2qs_ctx("Console", "log");
			break;
		default:
			icon = QIcon(*g_pIconManager->getSmallIcon(KviIconManager::Log));
			szText = __tr2qs_ctx("Other", "log");
	}

	setIcon(0, icon);
	setText(0, szText);
}

LogListViewLog::LogListViewLog(QTreeWidgetItem * pPar, LogFile::Type eType, LogFile * pLog)
    : LogListViewItem(pPar, eType, pLog)
{
	setText(0, m_pFileData->date().toString("yyyy-MM-dd"));
}
