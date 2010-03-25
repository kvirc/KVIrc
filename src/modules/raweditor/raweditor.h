#ifndef _RAWEDITOR_H_
#define _RAWEDITOR_H_
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "kvi_window.h"
#include "kvi_qstring.h"
#include <QTreeWidget>
#include "kvi_iconmanager.h"

#include <QWidget>
#include <QLineEdit>

class KviScriptEditor;
class KviTalPopupMenu;

class KviRawTreeWidget : public QTreeWidget
{
	public:
	KviRawTreeWidget(QWidget *par)
		: QTreeWidget(par){};
	void updateItem(QTreeWidgetItem *item)
	{
		update(indexFromItem(item,0));
	};
	~KviRawTreeWidget() {};
};

class KviRawTreeWidgetItem : public QTreeWidgetItem
{
public:
	int m_iIdx;
public:
	KviRawTreeWidgetItem(QTreeWidget * par,int idx,bool bEnabled);
	~KviRawTreeWidgetItem() {};
public:
	void setEnabled(bool bEnabled)
	{
		if (bEnabled) setIcon(0,QIcon(*g_pIconManager->getSmallIcon(KVI_SMALLICON_RAWEVENT)));
			else setIcon(0,QIcon(*g_pIconManager->getSmallIcon(KVI_SMALLICON_RAWEVENTNOHANDLERS)));
			((KviRawTreeWidget*)treeWidget())->updateItem(this);
	};
};

class KviRawHandlerTreeWidgetItem : public QTreeWidgetItem
{
public:
	KviRawHandlerTreeWidgetItem(QTreeWidgetItem * par,const QString & name,const QString & buffer,bool bEnabled)
	: QTreeWidgetItem(par), m_szBuffer(buffer) , m_bEnabled(bEnabled)
	{
		setText(0,name);
		setEnabled(bEnabled);
	};
	~KviRawHandlerTreeWidgetItem() {};
public:
	void setEnabled(bool bEnabled)
	{
		if (bEnabled) setIcon(0,QIcon(*g_pIconManager->getSmallIcon(KVI_SMALLICON_HANDLER)));
			else setIcon(0,QIcon(*g_pIconManager->getSmallIcon(KVI_SMALLICON_HANDLERDISABLED)));
			((KviRawTreeWidget*)treeWidget())->updateItem(this);
	};
	QString m_szBuffer;
	bool   m_bEnabled;
	void setName(const QString &szName);
};

class KviRawEditor : public QWidget
{
	Q_OBJECT
public:
	KviRawEditor(QWidget * par);
	~KviRawEditor();
public:
	KviScriptEditor * m_pEditor;
	KviRawTreeWidget       * m_pTreeWidget;
	QLineEdit       * m_pNameEditor;
	KviTalPopupMenu      * m_pContextPopup;
	KviRawHandlerTreeWidgetItem * m_pLastEditedItem;
	bool              m_bOneTimeSetupDone;
public:
	void commit();
	void saveLastEditedItem();
	void getUniqueHandlerName(KviRawTreeWidgetItem *it,QString &buffer);
	void getExportEventBuffer(QString &szBuffer,KviRawHandlerTreeWidgetItem * it);
protected slots:
	void currentItemChanged(QTreeWidgetItem *it,QTreeWidgetItem *);
	void customContextMenuRequested(const QPoint &pnt);
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
protected:
	virtual QPixmap * myIconPtr();
	virtual void fillCaptionBuffers();
	virtual void getConfigGroupName(QString &szName);
	virtual void saveProperties(KviConfig *);
	virtual void loadProperties(KviConfig *);
protected slots:
	void cancelClicked();
	void okClicked();
	void applyClicked();
};

#endif //_RAWEDITOR_H_
