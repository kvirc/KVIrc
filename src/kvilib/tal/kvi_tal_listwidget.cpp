//=============================================================================
//
//   File : kvi_tal_listbox.cpp
//   Creation date : Mon Jan 22 2007 11:25:08 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2007 Szymon Stefanek (pragma at kvirc dot net)
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
//=============================================================================

#include "kvi_tal_listwidget.h"
#include "kvi_tal_itemdelegates.h"

#include <QListWidget>
#include <QEvent>
#include <QHelpEvent>

KviTalListWidget::KviTalListWidget(QWidget * pParent,QString name,Qt::WFlags f)
: QListWidget(pParent)
{
	setObjectName(name);
	setWindowFlags(f);
}
bool KviTalListWidget::event(QEvent * e)
{

	if (e->type() == QEvent::ToolTip)
	{
	    QHelpEvent *helpEvent = static_cast<QHelpEvent *>(e);
		const QPoint &p=helpEvent->pos();
		QListWidgetItem *item=itemAt(p);
		if (item){
			debug ("Emit tip request in listwidget");
			emit tipRequest(item,p);
		}
	}
	return QListWidget::event(e);
}



#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "kvi_tal_listwidget.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
