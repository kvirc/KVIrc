#ifndef _ALIASEDITOR_H_
#define _ALIASEDITOR_H_
//=============================================================================
//
//   File : aliaseditor.h
//   Creation date : Mon Dec 23 2002 14:35:55 CEST by Szymon Stefanek
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
#include "kvi_pointerlist.h"
#include "kvi_tal_popupmenu.h"
#include "kvi_heapobject.h"

#include <QWidget>
#include <QLineEdit>
#include <QStringList>
#include <QPushButton>
#include <QLabel>
#include <QTreeWidget>

class QSplitter;
class KviScriptEditor;
class KviAliasNamespaceTreeWidgetItem;
class KviAliasTreeWidgetItem;

class KviAliasEditorTreeWidgetItem : public QTreeWidgetItem, public KviHeapObject
{
public:
	enum Type { Alias, Namespace };
public:
	KviAliasEditorTreeWidgetItem(QTreeWidget * pTreeWidget,Type eType,const QString &szName);
	KviAliasEditorTreeWidgetItem(KviAliasNamespaceTreeWidgetItem * pParentNamespaceItem,Type eType,const QString &szName);
	~KviAliasEditorTreeWidgetItem(){};
protected:
	Type m_eType;
	KviAliasNamespaceTreeWidgetItem * m_pParentNamespaceItem;
	QString m_szName;
public:
	const QString & name(){ return m_szName; };
	void setName(const QString &szName);
	Type type(){ return m_eType; };
	bool isAlias(){ return m_eType == Alias; };
	bool isNamespace(){ return m_eType == Namespace; };
	void setParentNamespaceItem(KviAliasNamespaceTreeWidgetItem* it){ m_pParentNamespaceItem=it; };
	KviAliasNamespaceTreeWidgetItem * parentNamespaceItem(){ return m_pParentNamespaceItem; };
	virtual QString key(int column,bool bAsc) const;
};


class KviAliasNamespaceTreeWidgetItem : public KviAliasEditorTreeWidgetItem
{
public:
	KviAliasNamespaceTreeWidgetItem(QTreeWidget * pTreeWidget,const QString &szName);
	KviAliasNamespaceTreeWidgetItem(KviAliasNamespaceTreeWidgetItem * pParentNamespace,const QString &szName);
	~KviAliasNamespaceTreeWidgetItem();
public:
	KviAliasNamespaceTreeWidgetItem * findNamespaceItem(const QString &szName);
	KviAliasNamespaceTreeWidgetItem * getNamespaceItem(const QString &szName);
	KviAliasTreeWidgetItem * findAliasItem(const QString &szName);
	KviAliasTreeWidgetItem * getAliasItem(const QString &szName);
	KviAliasTreeWidgetItem * createFullAliasItem(const QString &szFullName);
	KviAliasNamespaceTreeWidgetItem * createFullNamespaceItem(const QString &szFullName);
};


class KviAliasEditorTreeWidget : public QTreeWidget
{
	Q_OBJECT
public:
	KviAliasEditorTreeWidget(QWidget*);
	~KviAliasEditorTreeWidget(){};
protected:
	void mousePressEvent (QMouseEvent *e);
signals:
	void rightButtonPressed(QTreeWidgetItem *,QPoint);


};

class KviAliasTreeWidgetItem : public KviAliasEditorTreeWidgetItem
{
public:
	KviAliasTreeWidgetItem(KviAliasNamespaceTreeWidgetItem * pParentNamespace,const QString &szName);
	KviAliasTreeWidgetItem(QTreeWidget *pTreeWidget,const QString &szName);
	~KviAliasTreeWidgetItem();
public:
	QString m_szBuffer;
	int  m_cPos;
public:
	const QString & buffer(){ return m_szBuffer; };
	const int & cursorPosition(){return m_cPos; };
	void setBuffer(const QString &szBuffer){ m_szBuffer = szBuffer; };
	void setCursorPosition(const int &cPos){ m_cPos = cPos; };
};

class KviAliasEditor : public QWidget
{
	Q_OBJECT
public:
	KviAliasEditor(QWidget * par);
	~KviAliasEditor();
public:
	KviScriptEditor            * m_pEditor;
	KviAliasEditorTreeWidget   * m_pTreeWidget;
	QLabel                     * m_pNameLabel;
	QPushButton                * m_pRenameButton;
	KviAliasEditorTreeWidgetItem * m_pLastEditedItem;
	KviAliasEditorTreeWidgetItem * m_pLastClickedItem;
	//KviAliasTreeWidgetItem * m_pLastEditedItem;
	KviTalPopupMenu                 * m_pContextPopup;
	QSplitter                  * m_pSplitter;
	QString						m_szDir;
public:
	//bool modified(){ return m_bModified; };
	void commit();
	void exportAliases(bool,bool=false);
	void exportSelectionInSinglesFiles(KviPointerList<KviAliasTreeWidgetItem> *l);

	void saveProperties(KviConfig *);
	void loadProperties(KviConfig *);
	static void splitFullAliasOrNamespaceName(const QString &szFullName,QStringList &lNamespaces,QString &szName);
protected slots:
	void currentItemChanged(QTreeWidgetItem *it,QTreeWidgetItem *);
	void newAlias();
	void newNamespace();
	void exportAll();
	void exportSelectedSepFiles();
	void exportSelected();
	void removeSelectedItems();
	void itemPressed(QTreeWidgetItem *,QPoint);
	void renameItem();
	void slotFind();
	void slotCollapseNamespaces();
	void slotFindWord(const QString &);
	void slotReplaceAll(const QString &before,const QString &after);
	void itemRenamed(QTreeWidgetItem *it,int col);
protected:
	void recursiveCollapseNamespaces(KviAliasEditorTreeWidgetItem * it);
	void recursiveSearchReplace(const QString &szSearch,KviAliasEditorTreeWidgetItem * it,bool bReplace=false,const QString &szReplace="n");

	void recursiveCommit(KviAliasEditorTreeWidgetItem * it);
	void getExportAliasBuffer(QString &buffer,KviAliasTreeWidgetItem * it);
	void oneTimeSetup();
	void saveLastEditedItem();
	void getUniqueItemName(KviAliasEditorTreeWidgetItem *item,QString &buffer,KviAliasEditorTreeWidgetItem::Type eType);
	void appendSelectedItems(KviPointerList<KviAliasEditorTreeWidgetItem> * l);
	void appendSelectedItemsRecursive(KviPointerList<KviAliasEditorTreeWidgetItem> * l,QTreeWidgetItem *it);

	void appendAliasItems(KviPointerList<KviAliasTreeWidgetItem> * l,bool bSelectedOnly);
	void appendAliasItemsRecursive(KviPointerList<KviAliasTreeWidgetItem> * l,QTreeWidgetItem * pStartFrom,bool bSelectedOnly);
	void appendNamespaceItemsRecursive(KviPointerList<KviAliasTreeWidgetItem> * l,QTreeWidgetItem * pStartFrom,bool bSelectedOnly);
	void appendNamespaceItems(KviPointerList<KviAliasTreeWidgetItem> * l,bool bSelectedOnly);

	//void appendNamespaceItems(KviPointerList<KviAliasTreeWidgetItem> * l,KviAliasEditorTreeWidgetItem * pStartFrom,bool bSelectedOnly);
	bool removeItem(KviAliasEditorTreeWidgetItem *it,bool * pbYesToAll,bool bDeleteEmptyTree);
	void removeItemChildren(KviAliasEditorTreeWidgetItem *it);

	void openParentItems(QTreeWidgetItem * it);
	void activateItem(QTreeWidgetItem * it);
	QString askForAliasName(const QString &szAction,const QString &szText,const QString &szInitialText);
	QString askForNamespaceName(const QString &szAction,const QString &szText,const QString &szInitialText);
	bool itemExistsRecursive(QTreeWidgetItem *pSearchFor,QTreeWidgetItem * pSearchAt);
	bool itemExists(QTreeWidgetItem *pSearchFor);
	bool namespaceExists(QString &szFullItemName);
	bool aliasExists(QString &szFullItemName);
	bool hasSelectedItems();
	bool hasSelectedItemsRecursive(QTreeWidgetItem *);
	KviAliasNamespaceTreeWidgetItem * findNamespaceItem(const QString &szName);
	KviAliasNamespaceTreeWidgetItem * getNamespaceItem(const QString &szName);
	KviAliasTreeWidgetItem * findAliasItem(const QString &szName);
	KviAliasTreeWidgetItem * findAliasItemRecursive(KviAliasEditorTreeWidgetItem *it,const QString &szName);
	KviAliasTreeWidgetItem * getAliasItem(const QString &szName);
	KviAliasTreeWidgetItem * createFullAliasItem(const QString &szFullName);
	KviAliasNamespaceTreeWidgetItem * createFullNamespaceItem(const QString &szFullName);
	QString buildFullItemName(KviAliasEditorTreeWidgetItem * it);
};


class KviAliasEditorWindow : public KviWindow
{
	Q_OBJECT
public:
	KviAliasEditorWindow(KviFrame * lpFrm);
	~KviAliasEditorWindow();
protected:
	KviAliasEditor * m_pEditor;
	QWidget        * m_pBase;
protected:
	virtual QPixmap * myIconPtr();
	virtual void resizeEvent(QResizeEvent *e);
	virtual void getConfigGroupName(QString &szName);
	virtual void saveProperties(KviConfig *);
	virtual void loadProperties(KviConfig *);
protected slots:
	void cancelClicked();
	void okClicked();
	void applyClicked();
};

#endif //_ALIASEDITOR_H_
