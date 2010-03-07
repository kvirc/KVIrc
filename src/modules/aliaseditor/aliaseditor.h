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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
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



class KviAliasEditorTreeWidgetItem : public QTreeWidgetItem, public KviHeapObject
{
public:
	enum Type { Alias, Namespace };
public:
	KviAliasEditorTreeWidgetItem(QTreeWidget * pTreeWidget,Type eType,const QString &szName);
	KviAliasEditorTreeWidgetItem(KviAliasEditorTreeWidgetItem * pParentItem,Type eType,const QString &szName);
	~KviAliasEditorTreeWidgetItem(){};
protected:
	Type m_eType;
	KviAliasEditorTreeWidgetItem * m_pParentItem;
	QString m_szName;
	QString m_szBuffer;
	int  m_cPos;
public:
	const QString & name(){ return m_szName; };
	void setName(const QString &szName);
	Type type(){ return m_eType; };
	void setType(Type t);
	bool isAlias(){ return m_eType == Alias; };
	bool isNamespace(){ return m_eType == Namespace; };
	void setParentItem(KviAliasEditorTreeWidgetItem* it){ m_pParentItem=it; };
	KviAliasEditorTreeWidgetItem * parentItem(){ return m_pParentItem; };
	void setBuffer(const QString &szBuffer){ m_szBuffer = szBuffer; };
	const QString & buffer(){ return m_szBuffer; };
	const int & cursorPosition(){return m_cPos; };

	void setCursorPosition(const int &cPos){ m_cPos = cPos; };
};



class KviAliasEditorTreeWidget : public QTreeWidget
{
	Q_OBJECT
public:
	KviAliasEditorTreeWidget(QWidget*);
	~KviAliasEditorTreeWidget();
};

class KviAliasEditor : public QWidget
{
	Q_OBJECT
public:
	KviAliasEditor(QWidget * par);
	~KviAliasEditor();
public:
	KviScriptEditor              * m_pEditor;
	KviAliasEditorTreeWidget     * m_pTreeWidget;
	QLabel                       * m_pNameLabel;
	QPushButton                  * m_pRenameButton;
	KviAliasEditorTreeWidgetItem * m_pLastEditedItem;
	KviAliasEditorTreeWidgetItem * m_pLastClickedItem;
	KviTalPopupMenu              * m_pContextPopup;
	QSplitter                    * m_pSplitter;
	QString                        m_szDir;
	bool                           m_bSaving;
public:
	//bool modified(){ return m_bModified; };
	void commit();
	void exportAliases(bool,bool=false);
	void exportSelectionInSinglesFiles(KviPointerList<KviAliasEditorTreeWidgetItem> *l);

	void saveProperties(KviConfig *);
	void loadProperties(KviConfig *);
	static void splitFullAliasOrNamespaceName(const QString &szFullName,QStringList &lNamespaces,QString &szName);
protected slots:
	void currentItemChanged(QTreeWidgetItem *it,QTreeWidgetItem *);
	void renameItem();
	void newAlias();
	void customContextMenuRequested(const QPoint pnt);
	void newNamespace();
	void newItem(QString &szName,KviAliasEditorTreeWidgetItem::Type eType);

	void exportAll();
	void exportSelectedSepFiles();
	void exportSelected();

	void removeSelectedItems();
	void slotFind();
	void slotCollapseNamespaces();
	void slotFindWord(const QString &);
	void slotReplaceAll(const QString &before,const QString &after);
	void itemRenamed(QTreeWidgetItem *it,int col);
	void aliasRefresh(const QString &szName);

protected:
	KviPointerList<KviAliasEditorTreeWidgetItem> *m_pAliases;
	void recursiveCollapseNamespaces(KviAliasEditorTreeWidgetItem * it);
	void searchReplace(const QString &szSearch,bool bReplace=false,const QString &szReplace="n");

	void getExportAliasBuffer(QString &buffer,KviAliasEditorTreeWidgetItem * it);
	void oneTimeSetup();
	void saveLastEditedItem();
	void getUniqueItemName(KviAliasEditorTreeWidgetItem *item,QString &buffer,KviAliasEditorTreeWidgetItem::Type eType);
	void appendSelectedAliasItems(KviPointerList<KviAliasEditorTreeWidgetItem> * l);
	void appendSelectedAliasItemsRecursive(KviPointerList<KviAliasEditorTreeWidgetItem> * l,QTreeWidgetItem * pStartFrom);
	void appendSelectedItems(KviPointerList<KviAliasEditorTreeWidgetItem> * l);
	void appendSelectedItemsRecursive(KviPointerList<KviAliasEditorTreeWidgetItem> * l,QTreeWidgetItem * pStartFrom);

	void openParentItems(QTreeWidgetItem * it);
	void activateItem(QTreeWidgetItem * it);

	bool removeItem(KviAliasEditorTreeWidgetItem *it,bool * pbYesToAll,bool bDeleteEmptyTree);
	void removeItemChildren(KviAliasEditorTreeWidgetItem *it);

	QString askForAliasName(const QString &szAction,const QString &szText,const QString &szInitialText);
	QString askForNamespaceName(const QString &szAction,const QString &szText,const QString &szInitialText);

	bool itemExists(QTreeWidgetItem *pSearchFor);
	bool namespaceExists(QString &szFullItemName);
	bool aliasExists(QString &szFullItemName);
	void appendAllItems(KviPointerList<KviAliasEditorTreeWidgetItem> * l,KviAliasEditorTreeWidgetItem::Type);
	void appendAllItemsRecursive(KviPointerList<KviAliasEditorTreeWidgetItem> * l,QTreeWidgetItem * pStartFrom,KviAliasEditorTreeWidgetItem::Type);
	bool hasSelectedItems();
	void buildAliasesFile(KviPointerList<KviAliasEditorTreeWidgetItem> *l, QString & szBuffer);

	void buildFullItemPath(KviAliasEditorTreeWidgetItem * it,QString &szBuffer);
	KviAliasEditorTreeWidgetItem * findTopLevelItem(const QString &szName);
	KviAliasEditorTreeWidgetItem * createFullItem(const QString &szFullName);
	KviAliasEditorTreeWidgetItem * findItem(const QString &szName);
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
protected:
	virtual QPixmap * myIconPtr();
	virtual void getConfigGroupName(QString &szName);
	virtual void saveProperties(KviConfig *);
	virtual void loadProperties(KviConfig *);
protected slots:
	void cancelClicked();
	void okClicked();
	void applyClicked();
};

#endif //_ALIASEDITOR_H_
