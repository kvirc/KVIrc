//=============================================================================
//
//   File : kvi_tal_popupmenu.cpp
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

#define __KVILIB__
#include "kvi_tal_popupmenu.h"

#include <qwidgetaction.h>
#include <QMenu>
#include <QHash>


KviTalPopupMenu::KviTalPopupMenu(QWidget * pParent,const QString &szName)
: QMenu(pParent)
{
	connect(this,SIGNAL(triggered(QAction *)),this,SLOT(slottriggered(QAction *)));
	setName(szName);
	identifier=0;
}
KviTalPopupMenu::~KviTalPopupMenu() 
{
}
	
	
int KviTalPopupMenu::insertItem(const QString &szText)
{
	QAction *a=addAction(szText);
	actionsDict[identifier++]=a;
	return identifier-1;
}
int KviTalPopupMenu::insertItem(const QPixmap &pix,const QString &szText)
{
	//return QMenu::insertItem(QIcon(pix),szText,-1,-1);
	QAction *a=addAction(QIcon(pix),szText);
	actionsDict[identifier++]=a;
	return identifier-1;
}
int KviTalPopupMenu::insertItem(const QString &szText,int id)
{
	//return QMenu::insertItem(szText,id);
	
	QAction *a=addAction(szText);
	addAction(a);
	actionsDict[id]=a;
	return id;
}

int KviTalPopupMenu::insertItem(const QPixmap &pix,const QString &szText,int id)
{
	//return QMenu::insertItem(QIcon(pix),szText,id,-1);
	QAction *a=addAction(QIcon(pix),szText);
	actionsDict[id]=a;
	return id;
}


int KviTalPopupMenu::insertItem(const QString &szText,const QObject * pReceiver,const char * szSlot)
{
	//return QMenu::insertItem(szText,pReceiver,szSlot);
	QAction *a=addAction(szText,pReceiver,szSlot);
	actionsDict[identifier++]=a;
	return identifier-1;
}
int KviTalPopupMenu::insertItem(const QPixmap &pix,const QString &szText,const QObject * pReceiver,const char * szSlot)
{
	//return QMenu::insertItem(QIcon(pix),szText,pReceiver,szSlot);
	QAction *a=addAction(QIcon(pix),szText,pReceiver,szSlot);
	actionsDict[identifier++]=a;
	return identifier-1;
}
	
int KviTalPopupMenu::insertItem(const QPixmap &pix,const QString &szText,QMenu *pMenu)
{
	//return QMenu::insertItem(QIcon(pix),szText,pMenu,-1,-1);
	QAction *a=addMenu(pMenu);
	a->setText(szText);
	a->setIcon(QIcon(pix));
	actionsDict[identifier++]=a;
	return identifier-1;
}
	
int KviTalPopupMenu::insertItem(const QString &szText,QMenu *pMenu)
{
	//return QMenu::insertItem(szText,pMenu,-1,-1);
	QAction *a=addMenu(pMenu);
	a->setText(szText);
	actionsDict[identifier++]=a;
	return identifier-1;
}
	

int KviTalPopupMenu::insertItem(QWidget * pWidget)
{
	// needs Qt 4.2
	QWidgetAction * pAct = new QWidgetAction(this);
	pAct->setDefaultWidget(pWidget);
	addAction(pAct);
	return 0;
}
void KviTalPopupMenu::setItemChecked(int id,bool check)
{
	QAction * action=actionsDict.value(id);
	action->setCheckable(true);
    action->setChecked(check);
}
void KviTalPopupMenu::setItemParameter(int id, int param)
{
	QAction * action=actionsDict.value(id);
	QVariant par(param);
	action->setData(par);
}
int KviTalPopupMenu::itemParameter(int id)
{
	QAction * action=actionsDict.value(id);
	return action->data().asInt();
}
void KviTalPopupMenu::clear()
{
	QMenu::clear();
	identifier=0;
	actionsDict.clear();
}
QString KviTalPopupMenu::text(int id)
{
	QAction * action=actionsDict.value(id);
	return action->text();
}
void KviTalPopupMenu::setItemEnabled(int id,bool bFlag)
{
	QAction * action=actionsDict.value(id);
	action->setEnabled(bFlag);
}

int KviTalPopupMenu::insertSeparator()
{
	QAction *a=addSeparator();
	actionsDict[identifier++]=a;
	return identifier-1;
}
void KviTalPopupMenu::slottriggered(QAction *a)
{
	QHashIterator<int, QAction *> i(actionsDict);
	int count=0;
	bool found=false;
	while (i.hasNext()) 
	{
	     i.next();
		 if (i.value()!= a) count++;
		 else break;
	}
	emit activated(count);
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
	#include "kvi_tal_popupmenu.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES

