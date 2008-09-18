#ifndef _EVENTEDITOR_H_
#define _EVENTEDITOR_H_
//=============================================================================
//
//   File : eventeditor.h
//   Creation date : Mon Dec 23 2002 20:24:55 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2008 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the linkss of the GNU General Public License
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

#include "kvi_window.h"
#include "kvi_string.h"
#include "kvi_tal_popupmenu.h"

#include <QWidget>
#include <QLineEdit>
#include <QTreeWidget>

class KviScriptEditor;

class KviEventListViewItem : public QTreeWidgetItem
{
public:
	unsigned int m_uEventIdx;
	QString m_szName;
	QString m_szParams;
public:
	KviEventListViewItem(QTreeWidget * par,unsigned int uEvIdx,const QString &name,const QString &params);
	~KviEventListViewItem() {};
public:
	void setName(const QString &szName);
	QString name() const { return m_szName; };
};

class KviEventHandlerListViewItem : public QTreeWidgetItem
{
public:
	QString m_szName;
	QString m_szBuffer;
	bool   m_bEnabled;
public:
	KviEventHandlerListViewItem(QTreeWidgetItem * par,const QString &name,const QString &buffer,bool bEnabled);
	~KviEventHandlerListViewItem() {};
public:
	void setName(const QString &szName);
	QString name() const { return m_szName; };
	void setEnabled(const bool bEnabled);
};

class KviEventEditorListView : public QTreeWidget
{
	Q_OBJECT
public:
	KviEventEditorListView(QWidget*);
	~KviEventEditorListView(){};
protected:
	void mousePressEvent (QMouseEvent *e);
signals:
	void rightButtonPressed(QTreeWidgetItem *,QPoint);
};

class KviEventEditor : public QWidget
{
	Q_OBJECT
public:
	KviEventEditor(QWidget * par);
	~KviEventEditor();
public:
	KviScriptEditor * m_pEditor;
	KviEventEditorListView       * m_pListView;
	QLineEdit       * m_pNameEditor;
	KviTalPopupMenu      * m_pContextPopup;
	KviEventHandlerListViewItem * m_pLastEditedItem;
	bool              m_bOneTimeSetupDone;
public:
	void commit();
	void saveLastEditedItem();
	void getUniqueHandlerName(KviEventListViewItem *it,QString &buffer);
	void getExportEventBuffer(QString &szBuffer,KviEventHandlerListViewItem * it);
protected slots:
	void currentItemChanged(QTreeWidgetItem *it,QTreeWidgetItem *);
	void itemPressed(QTreeWidgetItem *it,const QPoint &pnt);
	void toggleCurrentHandlerEnabled();
	void removeCurrentHandler();
	void addHandlerForCurrentEvent();
	void exportAllEvents();
	void exportCurrentHandler();
protected:
	void showEvent(QShowEvent *e);
private:
	void oneTimeSetup();
};

class KviEventEditorWindow : public KviWindow
{
	Q_OBJECT
public:
	KviEventEditorWindow(KviFrame * lpFrm);
	~KviEventEditorWindow();
protected:
	KviEventEditor * m_pEditor;
	QWidget        * m_pBase;
protected:
	virtual QPixmap * myIconPtr();
	virtual void fillCaptionBuffers();
	virtual void resizeEvent(QResizeEvent *e);
	virtual void getConfigGroupName(KviStr &szName);
	virtual void saveProperties(KviConfig *);
	virtual void loadProperties(KviConfig *);
protected slots:
	void cancelClicked();
	void okClicked();
	void applyClicked();
};

#endif //_EVENTEDITOR_H_
