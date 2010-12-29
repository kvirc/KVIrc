#ifndef _KVI_TAL_POPUPMENU_H_
#define _KVI_TAL_POPUPMENU_H_

//=============================================================================
//
//   File : KviTalPopupMenu.h
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
#include "kvi_settings.h"
#include "KviQString.h"

#include <QMenu>
#include <QHash>
#include <QSignalMapper>

class KVILIB_API KviTalPopupMenu : public QMenu
{
	Q_OBJECT
protected:
	QHash<int, QAction*> actionsDict;
	// We need QSignalMapper to keep slot system for insertItem()
	QHash<QAction*, QSignalMapper*> signalMapper;
	int identifier;
public:
	KviTalPopupMenu(QWidget * pParent=0,const QString &szName = KviQString::Empty);
	virtual ~KviTalPopupMenu() ;
	int insertItem(const QString &szText);
	int insertItem(const QPixmap &pix,const QString &szText);
	int insertItem(const QString &szText,int id);
	int insertItem(const QPixmap &pix,const QString &szText,int id);
	int insertItem(const QString &szText,const QObject * pReceiver,const char * szSlot);
	int insertItem(const QPixmap &pix,const QString &szText,const QObject * pReceiver,const char * szSlot);
	int insertItem(const QPixmap &pix,const QString &szText,QMenu *pMenu);
	int insertItem(const QString &szText,QMenu *pMenu);
	int insertItem(QWidget * pWidget);
	void setItemChecked(int id,bool check);
	void setItemEnabled(int id,bool bFlag);
	void setItemParameter(int id, int param);
	int itemParameter(int id);
	QAction * getAction(int id);
	void clear();
	QString text(int);
	int insertSeparator();
protected slots:
	void slottriggered(QAction *);
	void slotActionTriggered(bool);
signals:
	void activated(int);
};
#endif // _KVI_TAL_POPUPMENU_H_
