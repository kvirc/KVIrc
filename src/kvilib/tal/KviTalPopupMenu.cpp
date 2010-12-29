//=============================================================================
//
//   File : KviTalPopupMenu.cpp
//   Creation date : Mon May 05 2008 11:25:08 by Alessandro Carbone
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2008 Alessandro Carbone (elfonol at gmail dot com)
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

#include "KviTalPopupMenu.h"

#include <QMenu>
#include <QHash>
#include <QAction>
#include <QWidgetAction>

KviTalPopupMenu::KviTalPopupMenu(QWidget * pParent,const QString &szName)
: QMenu(pParent)
{
	connect(this,SIGNAL(triggered(QAction *)),this,SLOT(slottriggered(QAction *)));
	setObjectName(szName);
	identifier = 0;

}

KviTalPopupMenu::~KviTalPopupMenu()
{
	clear();
}

QAction * KviTalPopupMenu::getAction(int id)
{
	QAction * a = actionsDict.value(id);
	return a ? a : 0;
}

int KviTalPopupMenu::insertItem(const QString &szText)
{
	QAction * a = addAction(szText);
	actionsDict[identifier++] = a;
	return identifier-1;
}

int KviTalPopupMenu::insertItem(const QPixmap &pix,const QString &szText)
{
	QAction * a = addAction(QIcon(pix),szText);
	actionsDict[identifier++] = a;
	return identifier-1;
}

int KviTalPopupMenu::insertItem(const QString &szText,int id)
{
	QAction * a = addAction(szText);
	actionsDict[id] = a;
	return id;
}

int KviTalPopupMenu::insertItem(const QPixmap &pix,const QString &szText,int id)
{
	QAction * a = addAction(QIcon(pix),szText);
	actionsDict[id] = a;
	return id;
}

int KviTalPopupMenu::insertItem(const QString &szText,const QObject * pReceiver,const char * szSlot)
{
	QAction * action = addAction(szText);

	// create a compatible slot
	signalMapper[action] = new QSignalMapper(this);
	signalMapper[action]->setMapping(action, identifier);

	connect(action, SIGNAL(triggered()), signalMapper[action], SLOT(map()));
	connect(signalMapper[action], SIGNAL(mapped(int)), pReceiver, szSlot);

	actionsDict[identifier++]=action;
	return identifier-1;
}

int KviTalPopupMenu::insertItem(const QPixmap &pix,const QString &szText,const QObject * pReceiver,const char * szSlot)
{
	QAction * action = addAction(pix, szText);

	// create a compatible slot
	signalMapper[action] = new QSignalMapper(this);
	signalMapper[action]->setMapping(action, identifier);

	connect(action, SIGNAL(triggered()), signalMapper[action], SLOT(map()));
	connect(signalMapper[action], SIGNAL(mapped(int)), pReceiver, szSlot);

	actionsDict.insert(identifier, action);
	actionsDict[identifier++] = action;
	return identifier-1;
}

int KviTalPopupMenu::insertItem(const QPixmap &pix,const QString &szText,QMenu *pMenu)
{
	QAction * a = addMenu(pMenu);
	a->setText(szText);
	a->setIcon(QIcon(pix));
	actionsDict[identifier++] = a;
	return identifier-1;
}

int KviTalPopupMenu::insertItem(const QString &szText,QMenu *pMenu)
{
	QAction * a = addMenu(pMenu);
	a->setText(szText);
	actionsDict[identifier++] = a;
	return identifier-1;
}

int KviTalPopupMenu::insertItem(QWidget * pWidget)
{
	QWidgetAction * pAct = new QWidgetAction(this);
	pAct->setDefaultWidget(pWidget);
	addAction(pAct);
	return 0;
}

void KviTalPopupMenu::setItemChecked(int id,bool check)
{
	QAction * action = actionsDict.value(id);
	action->setCheckable(true);
	action->setChecked(check);
}

void KviTalPopupMenu::setItemParameter(int id, int param)
{
	QAction * action = actionsDict.value(id);

	// take care of the slot
	if (signalMapper[action])
	{
		signalMapper[action]->removeMappings(action);
		signalMapper[action]->setMapping(action, param);
	}

	QVariant par(param);
	if (action) action->setData(par);
}

int KviTalPopupMenu::itemParameter(int id)
{
	QAction * action = actionsDict.value(id);
	if (action)
		return action->data().toInt();
	else return 0;
}

void KviTalPopupMenu::clear()
{
	QMenu::clear();
	identifier = 0;
	actionsDict.clear();
	signalMapper.clear();
}

QString KviTalPopupMenu::text(int id)
{
	QAction * action = actionsDict.value(id);
	return action->text();
}

void KviTalPopupMenu::setItemEnabled(int id,bool bFlag)
{
	QAction * action = actionsDict.value(id);
	action->setEnabled(bFlag);
}

int KviTalPopupMenu::insertSeparator()
{
	QAction * a = addSeparator();
	actionsDict[identifier++] = a;
	return -1;
}

void KviTalPopupMenu::slottriggered(QAction * a)
{
	if (signalMapper[a]) return; // When signalmapper is used we don't need this event

	int i = actionsDict.key(a, -1); // See if action exists and get ID
	if (i != -1)
	{
		emit activated(i);
	}

	return;
}

void KviTalPopupMenu::slotActionTriggered(bool)
{
	QAction * action = (QAction *)sender();
	slottriggered(action);
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "KviTalPopupMenu.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

