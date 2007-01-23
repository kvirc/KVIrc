#ifndef _EVENTEDITOR_H_
#define _EVENTEDITOR_H_
//
//   File : eventeditor.h
//   Creation date : Mon Dec 23 2002 20:24:55 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_window.h"
#include "kvi_string.h"

#include <qwidget.h>
#include <qlistview.h>
#include <qlineedit.h>

class KviScriptEditor;

class KviEventListViewItem : public QListViewItem
{
public:
	unsigned int m_uEventIdx;
	QString m_szName;
	QString m_szParams;
public:
	KviEventListViewItem(QListView * par,unsigned int uEvIdx,const QString &name,const QString &params)
	: QListViewItem(par), m_uEventIdx(uEvIdx), m_szName(name), m_szParams(params) {};
	~KviEventListViewItem() {};
public:
	virtual QString text(int col) const { return m_szName; };
	virtual const QPixmap * pixmap(int col) const;
};

class KviEventHandlerListViewItem : public QListViewItem
{
public:
	QString m_szName;
	QString m_szBuffer;
	bool   m_bEnabled;
public:
	KviEventHandlerListViewItem(QListViewItem * par,const QString &name,const QString &buffer,bool bEnabled)
	: QListViewItem(par), m_szName(name) , m_szBuffer(buffer) , m_bEnabled(bEnabled) {};
	~KviEventHandlerListViewItem() {};
public:
	virtual QString text(int col) const { return m_szName; };
	virtual const QPixmap * pixmap(int col) const;
};

class KviTalPopupMenu;

class KviEventEditor : public QWidget
{
	Q_OBJECT
public:
	KviEventEditor(QWidget * par);
	~KviEventEditor();
public:
	KviScriptEditor * m_pEditor;
	QListView       * m_pListView;
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
	void selectionChanged(QListViewItem *it);
	void itemPressed(QListViewItem *it,const QPoint &pnt,int col);
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
