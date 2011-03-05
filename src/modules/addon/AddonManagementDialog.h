#ifndef _MANAGEMENTDIALOG_H_
#define _MANAGEMENTDIALOG_H_
//=============================================================================
//
//   File : AddonManagementDialog.h
//   Creation date : Fri 08 Apr 2005 14:54:56 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2005-2010 Szymon Stefanek <pragma at kvirc dot net>
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
#include "KviTalListWidget.h"

#include <QWidget>
#include <QLabel>
#include <QColor>
#include <QTextDocument>
#include <QListWidget>
#include <QItemDelegate>
#include <QToolButton>

class QPixmap;
class KviKvsScriptAddon;

class AddonListViewItem : public KviTalListWidgetItem
{
public:
	AddonListViewItem(KviTalListWidget * v,KviKvsScriptAddon * a);
	~AddonListViewItem();
protected:
	KviKvsScriptAddon * m_pAddon;
	QTextDocument     * m_pText;
	QPixmap           * m_pIcon;
	QListWidget       * m_pListWidget;
	QString             m_szKey;
public:
	KviKvsScriptAddon * addon(){ return m_pAddon; };
};


class AddonManagementDialog : public QWidget
{
	Q_OBJECT
protected:
	AddonManagementDialog(QWidget * p);
public:
	~AddonManagementDialog();
protected:
	KviTalListWidget                 * m_pListWidget;
	static AddonManagementDialog * m_pInstance;
	QToolButton                      * m_pConfigureButton;
	QToolButton                      * m_pHelpButton;
	QToolButton                      * m_pPackButton;
	QToolButton                      * m_pUninstallButton;
public:
	static AddonManagementDialog * instance(){ return m_pInstance; };
	static void display(bool bTopLevel);
	static void cleanup();
protected:
	void fillListView();
	virtual void closeEvent(QCloseEvent * e);
protected slots:
	void currentChanged(QListWidgetItem * i, QListWidgetItem *);
	void closeClicked();
	void configureScript();
	void showScriptHelp();
	void packScript();
	void uninstallScript();
	void getMoreScripts();
	void installScript();
	virtual void reject();
};

#endif //!_MANAGEMENTDIALOG_H_
