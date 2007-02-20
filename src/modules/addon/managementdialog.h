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

#include <qdialog.h>

#include "kvi_listview.h"
#include "kvi_settings.h"
#include <qwidget.h>
#include <qlabel.h>
#include <qdialog.h>
#include <qcolor.h>
#include "kvi_tal_listview.h"

class QPushButton;
class QPixmap;
class KviKvsScriptAddon;

#ifdef COMPILE_USE_QT4
	#include <q3simplerichtext.h>
	#define QSimpleRichText Q3SimpleRichText
#else
	#include <qsimplerichtext.h>
#endif

class KviScriptAddonListViewItem : public KviTalListViewItem
{
public:
	KviScriptAddonListViewItem(KviTalListView * v,KviKvsScriptAddon * a);
	~KviScriptAddonListViewItem();
protected:
	KviKvsScriptAddon * m_pAddon;
	QSimpleRichText * m_pText;
	QPixmap * m_pIcon;
	KviTalListView * m_pListView;
	QString m_szKey;
public:
	KviKvsScriptAddon * addon(){ return m_pAddon; };
protected:
	virtual QString key(int,bool) const;
	virtual void paintCell(QPainter * p,const QColorGroup & cg,int column,int width,int align);
	virtual void setup();
};

class KviScriptAddonListView : public KviListView
{
	Q_OBJECT
public:
	KviScriptAddonListView(QWidget * pParent);
	~KviScriptAddonListView();
protected:
	virtual void resizeEvent(QResizeEvent * e);
};

class KviScriptManagementDialog : public QDialog
{
	Q_OBJECT
protected:
	KviScriptManagementDialog(QWidget * p);
public:
	~KviScriptManagementDialog();
protected:
	KviScriptAddonListView * m_pListView;
	static KviScriptManagementDialog * m_pInstance;
	QPushButton * m_pUninstallButton;
	QPushButton * m_pConfigureButton;
	QPushButton * m_pHelpButton;
	QPushButton * m_pInstallButton;
	QPushButton * m_pGetScriptsButton;
public:
	static KviScriptManagementDialog * instance(){ return m_pInstance; };
	static void display();
	static void cleanup();
protected:
	void fillListView();
	virtual void showEvent(QShowEvent * e);
	virtual void closeEvent(QCloseEvent *e);
protected slots:
	void currentChanged(KviTalListViewItem *i);
	void closeClicked();
	void showScriptHelp();
	void configureScript();
	void uninstallScript();
	void getMoreScripts();
	void installScript();
};


#endif //!_MANAGEMENTDIALOG_H_
