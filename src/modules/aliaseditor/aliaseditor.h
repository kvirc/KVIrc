#ifndef _ALIASEDITOR_H_
#define _ALIASEDITOR_H_
//=====================================================================================
//
//   File : aliaseditor.h
//   Creation date : Mon Dec 23 2002 14:35:55 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2005 Szymon Stefanek (pragma at kvirc dot net)
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
//=====================================================================================

#include "kvi_window.h"
#include "kvi_string.h"
#include "kvi_pointerlist.h"
#include "kvi_tal_popupmenu.h"
#include "kvi_tal_listview.h"

#include <QWidget>
#include <QLineEdit>
#include <QStringList>
#include <QPushButton>
#include <QLabel>

class QSplitter;
class KviScriptEditor;
class KviAliasNamespaceListViewItem;


class KviAliasEditorListViewItem : public KviTalListViewItem
{
public:
	enum Type { Alias, Namespace };
public:
	KviAliasEditorListViewItem(KviTalListView * pListView,Type eType,const QString &szName);
	KviAliasEditorListViewItem(KviAliasNamespaceListViewItem * pParentNamespaceItem,Type eType,const QString &szName);
	~KviAliasEditorListViewItem(){};
protected:
	Type m_eType;
	KviAliasNamespaceListViewItem * m_pParentNamespaceItem;
	QString m_szName;
public:
	const QString & name(){ return m_szName; };
	void setName(const QString &szName);
	Type type(){ return m_eType; };
	bool isAlias(){ return m_eType == Alias; };
	bool isNamespace(){ return m_eType == Namespace; };
	void setParentNamespaceItem(KviAliasNamespaceListViewItem* it){ m_pParentNamespaceItem=it; };
	KviAliasNamespaceListViewItem * parentNamespaceItem(){ return m_pParentNamespaceItem; };
	virtual QString key(int column,bool bAsc) const;
};

class KviAliasListViewItem;

class KviAliasNamespaceListViewItem : public KviAliasEditorListViewItem
{
public:
	KviAliasNamespaceListViewItem(KviTalListView * pListView,const QString &szName);
	KviAliasNamespaceListViewItem(KviAliasNamespaceListViewItem * pParentNamespace,const QString &szName);
	~KviAliasNamespaceListViewItem(){};
public:
	KviAliasNamespaceListViewItem * findNamespaceItem(const QString &szName);
	KviAliasNamespaceListViewItem * getNamespaceItem(const QString &szName);
	KviAliasListViewItem * findAliasItem(const QString &szName);
	KviAliasListViewItem * getAliasItem(const QString &szName);
	KviAliasListViewItem * createFullAliasItem(const QString &szFullName);
	KviAliasNamespaceListViewItem * createFullNamespaceItem(const QString &szFullName);
};


class KviAliasListViewItem : public KviAliasEditorListViewItem
{
public:
	KviAliasListViewItem(KviAliasNamespaceListViewItem * pParentNamespace,const QString &szName);
	KviAliasListViewItem(KviTalListView *pListView,const QString &szName);
	~KviAliasListViewItem(){};
public:
	QString m_szBuffer;
	QPoint  m_cPos;
public:
	const QString & buffer(){ return m_szBuffer; };
	const QPoint & cursorPosition(){ return m_cPos; };
	void setBuffer(const QString &szBuffer){ m_szBuffer = szBuffer; };
	void setCursorPosition(const QPoint &cPos){ m_cPos = cPos; };
};


class KviAliasEditor : public QWidget
{
	Q_OBJECT
public:
	KviAliasEditor(QWidget * par);
	~KviAliasEditor();
public:
	KviScriptEditor            * m_pEditor;
	KviTalListView                  * m_pListView;
	QLabel                     * m_pNameLabel;
	QPushButton                * m_pRenameButton;
	KviAliasEditorListViewItem * m_pLastEditedItem;
	KviAliasEditorListViewItem * m_pLastClickedItem;
	//KviAliasListViewItem * m_pLastEditedItem;
	KviTalPopupMenu                 * m_pContextPopup;
	QSplitter                  * m_pSplitter;
	QString						m_szDir;
public:
	//bool modified(){ return m_bModified; };
	void commit();
	void exportAliases(bool,bool=false);
	void exportSelectionInSinglesFiles(KviPointerList<KviAliasListViewItem> *l);

	void saveProperties(KviConfig *);
	void loadProperties(KviConfig *);
	static void splitFullAliasOrNamespaceName(const QString &szFullName,QStringList &lNamespaces,QString &szName);
protected slots:
	void currentItemChanged(KviTalListViewItem *it);
	void newAlias();
	void newNamespace();
	void exportAll();
	void exportSelectedSepFiles();
	void exportSelected();
	void removeSelectedItems();
	void itemPressed(KviTalListViewItem *it,const QPoint &pnt,int col);
	void renameItem();
	void slotFind();
	void slotCollapseNamespaces();
	void slotFindWord(const QString &);
	void slotReplaceAll(const QString &before,const QString &after);
protected:
//	QString const & findWord(const QString &txt,bool);
	void recursiveCollapseNamespaces(KviAliasEditorListViewItem * it);
	void recursiveSearchReplace(const QString &szSearch,KviAliasEditorListViewItem * it,bool bReplace=false,const QString &szReplace="n");
	
	void recursiveCommit(KviAliasEditorListViewItem * it);
	void getExportAliasBuffer(QString &buffer,KviAliasListViewItem * it);
	void oneTimeSetup();
	void selectOneItem(KviTalListViewItem * it,KviTalListViewItem *pStartFrom);
	void saveLastEditedItem();
	void getUniqueItemName(KviAliasEditorListViewItem *item,QString &buffer,KviAliasEditorListViewItem::Type eType);
	void appendSelectedItems(KviPointerList<KviAliasEditorListViewItem> * l,KviAliasEditorListViewItem * pStartFrom,bool bIncludeChildrenOfSelected = false);
	void appendAliasItems(KviPointerList<KviAliasListViewItem> * l,KviAliasEditorListViewItem * pStartFrom,bool bSelectedOnly);
	void appendNamespaceItems(KviPointerList<KviAliasListViewItem> * l,KviAliasEditorListViewItem * pStartFrom,bool bSelectedOnly);
	bool removeItem(KviAliasEditorListViewItem *it,bool * pbYesToAll,bool bDeleteEmptyTree);
	void openParentItems(KviTalListViewItem * it);
	void activateItem(KviTalListViewItem * it);
	QString askForAliasName(const QString &szAction,const QString &szText,const QString &szInitialText);
	QString askForNamespaceName(const QString &szAction,const QString &szText,const QString &szInitialText);
	bool itemExists(KviTalListViewItem *pSearchFor,KviTalListViewItem * pSearchAt);
	bool namespaceExists(QString &szFullItemName);
	bool aliasExists(QString &szFullItemName);
	bool hasSelectedItems(KviTalListViewItem * it);
	KviAliasNamespaceListViewItem * findNamespaceItem(const QString &szName);
	KviAliasNamespaceListViewItem * getNamespaceItem(const QString &szName);
	KviAliasListViewItem * findAliasItem(const QString &szName);
	KviAliasListViewItem * getAliasItem(const QString &szName);
	KviAliasListViewItem * createFullAliasItem(const QString &szFullName);
	KviAliasNamespaceListViewItem * createFullNamespaceItem(const QString &szFullName);
	QString buildFullItemName(KviAliasEditorListViewItem * it);
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
	virtual void getConfigGroupName(KviStr &szName);
	virtual void saveProperties(KviConfig *);
	virtual void loadProperties(KviConfig *);
protected slots:
	void cancelClicked();
	void okClicked();
	void applyClicked();
};

#endif //_ALIASEDITOR_H_
