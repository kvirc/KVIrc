#ifndef _EVENTEDITOR_H_
#define _EVENTEDITOR_H_
//=============================================================================
//
//   File : EventEditorWindow.h
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
#include "KviCString.h"

#include <QWidget>
#include <QLineEdit>
#include <QTreeWidget>
#include <QMenu>
#include <QCheckBox>

class KviScriptEditor;

class EventEditorEventTreeWidgetItem : public QTreeWidgetItem
{
public:
	unsigned int m_uEventIdx;
	QString m_szName;
	QString m_szParams;

public:
	EventEditorEventTreeWidgetItem(QTreeWidget * par, unsigned int uEvIdx, const QString & name, QString params);
	~EventEditorEventTreeWidgetItem(){};

public:
	void setName(const QString & szName);
	QString name() const { return m_szName; };
};

class EventEditorHandlerTreeWidgetItem : public QTreeWidgetItem
{
public:
	QString m_szName;
	QString m_szBuffer;
	bool m_bEnabled;
	int m_cPos;

public:
	EventEditorHandlerTreeWidgetItem(QTreeWidgetItem * par, const QString & name, QString buffer, bool bEnabled);
	~EventEditorHandlerTreeWidgetItem(){};

public:
	const int & cursorPosition() { return m_cPos; };
	void setCursorPosition(const int & cPos)
	{
		m_cPos = cPos;
	};

	void setName(const QString & szName);
	QString name() const { return m_szName; };
	void setEnabled(const bool bEnabled);
	bool isEnabled() { return m_bEnabled; };
};

class EventEditorTreeWidget : public QTreeWidget
{
	Q_OBJECT
public:
	EventEditorTreeWidget(QWidget *);
	~EventEditorTreeWidget(){};

protected:
	void mousePressEvent(QMouseEvent * e) override;
signals:
	void rightButtonPressed(QTreeWidgetItem *, QPoint);
};

class EventEditor : public QWidget
{
	Q_OBJECT
public:
	EventEditor(QWidget * par);
	~EventEditor();

public:
	KviScriptEditor * m_pEditor;
	EventEditorTreeWidget * m_pTreeWidget;
	QLineEdit * m_pNameEditor;
	QCheckBox * m_pIsEnabled;
	QMenu * m_pContextPopup = nullptr;
	EventEditorHandlerTreeWidgetItem * m_pLastEditedItem;
	bool m_bOneTimeSetupDone;

public:
	void commit();
	void saveLastEditedItem();
	void getUniqueHandlerName(EventEditorEventTreeWidgetItem * it, QString & buffer);
	void getExportEventBuffer(QString & szBuffer, EventEditorHandlerTreeWidgetItem * it);
protected slots:
	void currentItemChanged(QTreeWidgetItem * it, QTreeWidgetItem *);
	void itemPressed(QTreeWidgetItem * it, const QPoint & pnt);
	void toggleCurrentHandlerEnabled();
	void removeCurrentHandler();
	void addHandlerForCurrentEvent();
	void exportAllEvents();
	void exportCurrentHandler();
	void eventHandlerDisabled(const QString & szName);

protected:
	void showEvent(QShowEvent * e) override;

private:
	void oneTimeSetup();
};

class EventEditorWindow : public KviWindow
{
	Q_OBJECT
public:
	EventEditorWindow();
	~EventEditorWindow();

protected:
	EventEditor * m_pEditor;

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

#endif //_EVENTEDITOR_H_
