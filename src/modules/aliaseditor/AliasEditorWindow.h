#ifndef _ALIASEDITOR_H_
#define _ALIASEDITOR_H_
//=============================================================================
//
//   File : AliasEditorWindow.h
//   Creation date : Mon Dec 23 2002 14:35:55 CEST by Szymon Stefanek
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
#include "KviPointerList.h"
#include "KviHeapObject.h"

#include <QWidget>
#include <QLineEdit>
#include <QStringList>
#include <QPushButton>
#include <QLabel>
#include <QTreeWidget>
#include <QMenu>

class QSplitter;
class KviScriptEditor;

class AliasEditorTreeWidgetItem : public QTreeWidgetItem, public KviHeapObject
{
public:
	enum Type
	{
		Alias,
		Namespace
	};

public:
	AliasEditorTreeWidgetItem(QTreeWidget * pTreeWidget, Type eType, const QString & szName);
	AliasEditorTreeWidgetItem(AliasEditorTreeWidgetItem * pParentItem, Type eType, const QString & szName);
	~AliasEditorTreeWidgetItem(){};

protected:
	Type m_eType;
	AliasEditorTreeWidgetItem * m_pParentItem;
	QString m_szName;
	QString m_szBuffer;
	int m_cPos;

public:
	const QString & name() const { return m_szName; }
	void setName(const QString & szName);
	Type type() const { return m_eType; }
	void setType(Type t);
	bool isAlias() const { return m_eType == Alias; }
	bool isNamespace() const { return m_eType == Namespace; }
	void setParentItem(AliasEditorTreeWidgetItem * it) { m_pParentItem = it; };
	AliasEditorTreeWidgetItem * parentItem() { return m_pParentItem; };
	void setBuffer(const QString & szBuffer) { m_szBuffer = szBuffer; }
	const QString & buffer() const { return m_szBuffer; }
	const int & cursorPosition() const { return m_cPos; }

	void setCursorPosition(const int & cPos) { m_cPos = cPos; };
};

class AliasEditorTreeWidget : public QTreeWidget
{
	Q_OBJECT
public:
	AliasEditorTreeWidget(QWidget *);
	~AliasEditorTreeWidget();
};

class AliasEditorWidget : public QWidget
{
	Q_OBJECT
public:
	AliasEditorWidget(QWidget * par);
	~AliasEditorWidget();

public:
	KviScriptEditor * m_pEditor;
	AliasEditorTreeWidget * m_pTreeWidget;
	QLabel * m_pNameLabel;
	QPushButton * m_pRenameButton;
	AliasEditorTreeWidgetItem * m_pLastEditedItem;
	AliasEditorTreeWidgetItem * m_pLastClickedItem;
	QMenu * m_pContextPopup;
	QSplitter * m_pSplitter;
	QString m_szDir;
	bool m_bSaving;

public:
	//bool modified(){ return m_bModified; };
	void commit();
	void exportAliases(bool, bool = false);
	void exportSelectionInSinglesFiles(KviPointerList<AliasEditorTreeWidgetItem> * l);

	void saveProperties(KviConfigurationFile *);
	void loadProperties(KviConfigurationFile *);
	static void splitFullAliasOrNamespaceName(const QString & szFullName, QStringList & lNamespaces, QString & szName);
protected slots:
	void currentItemChanged(QTreeWidgetItem * it, QTreeWidgetItem *);
	void renameItem();
	void newAlias();
	void customContextMenuRequested(const QPoint pnt);
	void newNamespace();
	void newItem(QString & szName, AliasEditorTreeWidgetItem::Type eType);

	void exportAll();
	void exportSelectedSepFiles();
	void exportSelected();

	void removeSelectedItems();
	void slotFind();
	void slotCollapseNamespaces();
	void slotFindWord(const QString &);
	void slotReplaceAll(const QString & before, const QString & after);
	void itemRenamed(QTreeWidgetItem * it, int col);
	void aliasRefresh(const QString & szName);

protected:
	KviPointerList<AliasEditorTreeWidgetItem> * m_pAliases;
	void recursiveCollapseNamespaces(AliasEditorTreeWidgetItem * it);
	void searchReplace(const QString & szSearch, bool bReplace = false, const QString & szReplace = "n");

	void getExportAliasBuffer(QString & buffer, AliasEditorTreeWidgetItem * it);
	void oneTimeSetup();
	void saveLastEditedItem();
	void getUniqueItemName(AliasEditorTreeWidgetItem * item, QString & buffer, AliasEditorTreeWidgetItem::Type eType);
	void appendSelectedAliasItems(KviPointerList<AliasEditorTreeWidgetItem> * l);
	void appendSelectedAliasItemsRecursive(KviPointerList<AliasEditorTreeWidgetItem> * l, QTreeWidgetItem * pStartFrom);
	void appendSelectedItems(KviPointerList<AliasEditorTreeWidgetItem> * l);
	void appendSelectedItemsRecursive(KviPointerList<AliasEditorTreeWidgetItem> * l, QTreeWidgetItem * pStartFrom);

	void openParentItems(QTreeWidgetItem * it);
	void activateItem(QTreeWidgetItem * it);

	bool removeItem(AliasEditorTreeWidgetItem * it, bool * pbYesToAll, bool bDeleteEmptyTree);
	void removeItemChildren(AliasEditorTreeWidgetItem * it);

	QString askForAliasName(const QString & szAction, const QString & szText, const QString & szInitialText);
	QString askForNamespaceName(const QString & szAction, const QString & szText, const QString & szInitialText);
	//
	bool itemExists(QTreeWidgetItem * pSearchFor);
	bool namespaceExists(QString & szFullItemName);
	bool aliasExists(QString & szFullItemName);
	void appendAllItems(KviPointerList<AliasEditorTreeWidgetItem> * l, AliasEditorTreeWidgetItem::Type);
	void appendAllItemsRecursive(KviPointerList<AliasEditorTreeWidgetItem> * l, QTreeWidgetItem * pStartFrom, AliasEditorTreeWidgetItem::Type);
	bool hasSelectedItems();
	void buildAliasesFile(KviPointerList<AliasEditorTreeWidgetItem> * l, QString & szBuffer);

	void buildFullItemPath(AliasEditorTreeWidgetItem * it, QString & szBuffer);
	AliasEditorTreeWidgetItem * findTopLevelItem(const QString & szName);
	AliasEditorTreeWidgetItem * createFullItem(const QString & szFullName);
	AliasEditorTreeWidgetItem * findItem(const QString & szName);
	QString buildFullItemName(AliasEditorTreeWidgetItem * it);
};

class AliasEditorWindow : public KviWindow
{
	Q_OBJECT
public:
	AliasEditorWindow();
	~AliasEditorWindow();

protected:
	AliasEditorWidget * m_pEditor;

protected:
	QPixmap * myIconPtr() override;
	void getConfigGroupName(QString & szName) override;
	void saveProperties(KviConfigurationFile *) override;
	void loadProperties(KviConfigurationFile *) override;
protected slots:
	void cancelClicked();
	void okClicked();
	void applyClicked();
};

#endif //_ALIASEDITOR_H_
