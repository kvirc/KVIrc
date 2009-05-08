#ifndef _RAWEDITOR_H_
#define _RAWEDITOR_H_
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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//

#include "kvi_window.h"
#include "kvi_qstring.h"

#include <qwidget.h>
#include "kvi_tal_listview.h"
#include <qlineedit.h>

class KviScriptEditor;

class KviRawListViewItem : public KviTalListViewItem
{
public:
	int m_iIdx;
	QString m_szName;
public:
	KviRawListViewItem(KviTalListView * par,int idx);
	~KviRawListViewItem() {};
public:
	virtual QString text(int col) const { return m_szName; };
	virtual const QPixmap * pixmap(int col) const;
};

class KviRawHandlerListViewItem : public KviTalListViewItem
{
public:
	KviRawHandlerListViewItem(KviTalListViewItem * par,const QString & name,const QString & buffer,bool bEnabled)
	: KviTalListViewItem(par), m_szName(name) , m_szBuffer(buffer) , m_bEnabled(bEnabled) {};
	~KviRawHandlerListViewItem() {};
public:
	QString m_szName;
	QString m_szBuffer;
	bool   m_bEnabled;
	virtual QString text(int col) const { return m_szName; };
	virtual const QPixmap * pixmap(int col) const;
};

class KviTalPopupMenu;

class KviRawEditor : public QWidget
{
	Q_OBJECT
public:
	KviRawEditor(QWidget * par);
	~KviRawEditor();
public:
	KviScriptEditor * m_pEditor;
	KviTalListView       * m_pListView;
	QLineEdit       * m_pNameEditor;
	KviTalPopupMenu      * m_pContextPopup;
	KviRawHandlerListViewItem * m_pLastEditedItem;
	bool              m_bOneTimeSetupDone;
public:
	void commit();
	void saveLastEditedItem();
	void getUniqueHandlerName(KviRawListViewItem *it,QString &buffer);
	void getExportEventBuffer(QString &szBuffer,KviRawHandlerListViewItem * it);
protected slots:
	void selectionChanged(KviTalListViewItem *it);
	void itemPressed(KviTalListViewItem *it,const QPoint &pnt,int col);
	void toggleCurrentHandlerEnabled();
	void removeCurrentHandler();
	void addHandlerForCurrentRaw();
	void addRaw();
	void exportAllEvents();
	void exportCurrentHandler();
protected:
	void showEvent(QShowEvent *e);
private:
	void oneTimeSetup();
};

class KviRawEditorWindow : public KviWindow
{
	Q_OBJECT
public:
	KviRawEditorWindow(KviFrame * lpFrm);
	~KviRawEditorWindow();
protected:
	KviRawEditor * m_pEditor;
	QWidget      * m_pBase;
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

#endif //_RAWEDITOR_H_
