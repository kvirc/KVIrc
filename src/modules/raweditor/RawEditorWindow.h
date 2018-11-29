#ifndef _RAWEDITOR_H_
#define _RAWEDITOR_H_
//=============================================================================
//
//   File : RawEditorWindow.h
//   Creation date : Mon Dec 23 2002 20:24:55 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your option) any later version.
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

#include "KviWindow.h"
#include "KviQString.h"
#include <QTreeWidget>
#include "KviIconManager.h"

#include <QWidget>
#include <QLineEdit>

class KviScriptEditor;
class QMenu;

class RawTreeWidget : public QTreeWidget
{
public:
	RawTreeWidget(QWidget * par)
	    : QTreeWidget(par){};
	void updateItem(QTreeWidgetItem * item)
	{
		update(indexFromItem(item, 0));
	};
	~RawTreeWidget(){};
};

class RawTreeWidgetItem : public QTreeWidgetItem
{
public:
	int m_iIdx;

public:
	RawTreeWidgetItem(QTreeWidget * par, int idx, bool bEnabled);
	~RawTreeWidgetItem(){};

public:
	void setEnabled(bool bEnabled)
	{
		if(bEnabled)
			setIcon(0, QIcon(*g_pIconManager->getSmallIcon(KviIconManager::RawEvent)));
		else
			setIcon(0, QIcon(*g_pIconManager->getSmallIcon(KviIconManager::RawEventNoHandlers)));
		((RawTreeWidget *)treeWidget())->updateItem(this);
	};
};

class RawHandlerTreeWidgetItem : public QTreeWidgetItem
{
public:
	RawHandlerTreeWidgetItem(QTreeWidgetItem * par, const QString & name, const QString & buffer, bool bEnabled)
	    : QTreeWidgetItem(par), m_szBuffer(buffer), m_bEnabled(bEnabled)
	{
		setText(0, name);
		setEnabled(bEnabled);
	};
	~RawHandlerTreeWidgetItem(){};

public:
	void setEnabled(bool bEnabled)
	{
		if(bEnabled)
			setIcon(0, QIcon(*g_pIconManager->getSmallIcon(KviIconManager::Handler)));
		else
			setIcon(0, QIcon(*g_pIconManager->getSmallIcon(KviIconManager::HandlerDisabled)));
		((RawTreeWidget *)treeWidget())->updateItem(this);
	};
	QString m_szBuffer;
	bool m_bEnabled;
	void setName(const QString & szName);
};

class RawEditorWidget : public QWidget
{
	Q_OBJECT
public:
	RawEditorWidget(QWidget * par);
	~RawEditorWidget();

public:
	KviScriptEditor * m_pEditor;
	RawTreeWidget * m_pTreeWidget;
	QLineEdit * m_pNameEditor;
	QMenu * m_pContextPopup;
	RawHandlerTreeWidgetItem * m_pLastEditedItem;
	bool m_bOneTimeSetupDone;

public:
	void commit();
	void saveLastEditedItem();
	void getUniqueHandlerName(RawTreeWidgetItem * it, QString & buffer);
	void getExportEventBuffer(QString & szBuffer, RawHandlerTreeWidgetItem * it);
protected slots:
	void currentItemChanged(QTreeWidgetItem * it, QTreeWidgetItem *);
	void customContextMenuRequested(const QPoint & pnt);
	void toggleCurrentHandlerEnabled();
	void removeCurrentHandler();
	void addHandlerForCurrentRaw();
	void addRaw();
	void exportAllEvents();
	void exportCurrentHandler();

protected:
	void showEvent(QShowEvent * e) override;

private:
	void oneTimeSetup();
};

class RawEditorWindow : public KviWindow
{
	Q_OBJECT
public:
	RawEditorWindow();
	~RawEditorWindow();

protected:
	RawEditorWidget * m_pEditor;

protected:
	QPixmap * myIconPtr() override;
	void fillCaptionBuffers() override;
	void getConfigGroupName(QString & szName) override;
	void saveProperties(KviConfigurationFile *) override;
	void loadProperties(KviConfigurationFile *) override;
protected slots:
	void cancelClicked();
	void okClicked();
	void applyClicked();
};

#endif //_RAWEDITOR_H_
