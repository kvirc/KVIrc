#ifndef _MANAGEMENTDIALOG_H_
#define _MANAGEMENTDIALOG_H_
//=============================================================================
//
//   File : managementdialog.h
//   Created on Fri 08 Apr 2005 14:54:56 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2005 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_settings.h"
#include "kvi_tal_listwidget.h"

#include <QWidget>
#include <QLabel>
#include <QDialog>
#include <QColor>
#include <QTextDocument>
#include <QListWidget>
#include <QItemDelegate>

class KviStyledToolButton;
class QPixmap;
class KviKvsScriptAddon;



class KviScriptAddonListViewItem : public KviTalListWidgetItem
{
public:
	KviScriptAddonListViewItem(KviTalListWidget * v,KviKvsScriptAddon * a);
	~KviScriptAddonListViewItem();
protected:
	KviKvsScriptAddon * m_pAddon;
	QTextDocument * m_pText;
	QPixmap * m_pIcon;
	QListWidget * m_pListWidget;
	QString m_szKey;
public:
	KviKvsScriptAddon * addon(){ return m_pAddon; };

};


class KviScriptManagementDialog : public QDialog
{
	Q_OBJECT
protected:
	KviScriptManagementDialog(QWidget * p);
public:
	~KviScriptManagementDialog();
protected:
	KviTalListWidget * m_pListWidget;
	static KviScriptManagementDialog * m_pInstance;
	KviStyledToolButton * m_pConfigureButton;
	KviStyledToolButton * m_pHelpButton;
	KviStyledToolButton * m_pPackButton;
	KviStyledToolButton * m_pUninstallButton;
public:
	static KviScriptManagementDialog * instance(){ return m_pInstance; };
	static void display();
	static void cleanup();
protected:
	void fillListView();
	virtual void showEvent(QShowEvent * e);
	virtual void closeEvent(QCloseEvent *e);
protected slots:
	void currentChanged(QListWidgetItem *i,QListWidgetItem *);
	void closeClicked();
	void configureScript();
	void showScriptHelp();
	void packScript();
	void uninstallScript();
	void getMoreScripts();
	void installScript();
};

#endif //!_MANAGEMENTDIALOG_H_
