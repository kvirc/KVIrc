//=============================================================================
//
//   File : logviewwidget.cpp
//   Creation date : Thu Apr 23 2002 17:42:12 by Juanjo Alvarez
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

#include "logviewwidget.h"

//#include "kvi_debug.h"
#include "kvi_locale.h"
#include "kvi_iconmanager.h"
#include "kvi_app.h"
#include "kvi_qstring.h"

#include <QSplitter>
#include <QLayout>
#include <QDir>
#include <QTextCodec>

#ifdef COMPILE_ZLIB_SUPPORT
	#include <zlib.h>
#endif

KviLogListViewItem::KviLogListViewItem(QTreeWidgetItem * par, KviLogFile::KviLogTypes type, KviLogFile * fileData)
: QTreeWidgetItem(par), m_type(type), m_pFileData(fileData)
{
	setText(0,m_pFileData ? m_pFileData->name() : QString());
}

KviLogListViewItem::KviLogListViewItem(QTreeWidget * par, KviLogFile::KviLogTypes type, KviLogFile * fileData)
: QTreeWidgetItem(par), m_type(type), m_pFileData(fileData)
{
	setText(0,m_pFileData ? m_pFileData->name() : QString());
}

KviLogListViewItemFolder::KviLogListViewItemFolder(QTreeWidgetItem * par, const QString& label)
: KviLogListViewItem(par,KviLogFile::Other,0)
{
	setText(0,label);
}

KviLogListViewItemType::KviLogListViewItemType(QTreeWidget * par, KviLogFile::KviLogTypes type)
: KviLogListViewItem(par,type,0)
{
	QIcon icon;
	QString text;

	switch(m_type)
	{
		case KviLogFile::Channel:
			icon= QIcon(*g_pIconManager->getSmallIcon(KVI_SMALLICON_CHANNEL));
			text = __tr2qs_ctx("Channel","logview");
			break;
		case KviLogFile::Query:
			icon= QIcon(*g_pIconManager->getSmallIcon(KVI_SMALLICON_QUERY));
			text = __tr2qs_ctx("Query","logview");
			break;
		case KviLogFile::DccChat:
			icon= QIcon(*g_pIconManager->getSmallIcon(KVI_SMALLICON_DCCMSG));
			text = __tr2qs_ctx("DCC Chat","logview");
			break;
		case KviLogFile::Console:
			icon= QIcon(*g_pIconManager->getSmallIcon(KVI_SMALLICON_CONSOLE));
			text = __tr2qs_ctx("Console","logview");
			break;
		default:
			icon= QIcon(*g_pIconManager->getSmallIcon(KVI_SMALLICON_HELP));
			text = __tr2qs_ctx("Other","logview");
			break;
	}

	setIcon(0, icon);
	setText(0, text);
}

KviLogListViewLog::KviLogListViewLog(QTreeWidgetItem * par, KviLogFile::KviLogTypes type, KviLogFile * fileData)
: KviLogListViewItem(par,type,fileData)
{
	setText(0, m_pFileData->date().toString("yyyy-MM-dd"));
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "logviewwidget.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
