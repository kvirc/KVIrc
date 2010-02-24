#ifndef _CLASSEDITOR_H_
#define _CLASSEDITOR_H_
//=============================================================================
//
//   File : classeditor.h
//   Creation date : Mon Feb 15 2010 14:35:55 CEST by Carbone Alessandro
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QStringList>
#include <QPushButton>
#include <QLabel>
#include <QTreeWidget>
//#include <QItemDelegate>
#include <QAbstractItemView>

class QSplitter;
class KviScriptEditor;
class KviClassEditorTreeWidget;
/*
class KVIRC_API KviClassEditorTreeWidgetItemDelegate : public QItemDelegate//QStyledItemDelegate
{
public:
        KviClassEditorTreeWidgetItemDelegate(QAbstractItemView * pWidget=0)
                : QItemDelegate(pWidget) {};
        ~KviClassEditorTreeWidgetItemDelegate(){};
       // QSize sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const;
        void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};
*/

class KviClassEditorTreeWidgetItem : public QTreeWidgetItem, public KviHeapObject
{
public:
        enum Type { Class, Namespace, Method };
public:
	KviClassEditorTreeWidgetItem(QTreeWidget * pTreeWidget,Type eType,const QString &szName);
        KviClassEditorTreeWidgetItem(KviClassEditorTreeWidgetItem * pParentItem,Type eType,const QString &szName);
	~KviClassEditorTreeWidgetItem(){};
protected:
	Type m_eType;
        KviClassEditorTreeWidgetItem * m_pParentItem;
	QString m_szName;
        QString m_szBuffer;
        bool m_bClassModified;
        int  m_cPos;
public:
	const QString & name(){ return m_szName; };
	void setName(const QString &szName);
        void setClassModified(bool bModified){m_bClassModified=bModified;};
        bool classIsModified(){return m_bClassModified;};

	Type type(){ return m_eType; };
        void setType(Type t);
	bool isClass(){ return m_eType == Class; };
	bool isNamespace(){ return m_eType == Namespace; };
        bool isMethod(){ return m_eType == Method;};
        void setParentItem(KviClassEditorTreeWidgetItem* it){ m_pParentItem=it; };
        KviClassEditorTreeWidgetItem * parentItem(){ return m_pParentItem; };
        void setBuffer(const QString &szBuffer){ m_szBuffer = szBuffer; };
        const QString & buffer(){ return m_szBuffer; };
        const int & cursorPosition(){return m_cPos; };

        void setCursorPosition(const int &cPos){ m_cPos = cPos; };
};



class KviClassEditorTreeWidget : public QTreeWidget
{
	Q_OBJECT
public:
	KviClassEditorTreeWidget(QWidget*);
	~KviClassEditorTreeWidget();
protected:
	void mousePressEvent (QMouseEvent *e);
signals:
	void rightButtonPressed(QTreeWidgetItem *,QPoint);


};

class KviClassEditor : public QWidget
{
	Q_OBJECT
public:
	KviClassEditor(QWidget * par);
	~KviClassEditor();
public:
	KviScriptEditor              * m_pEditor;
	KviClassEditorTreeWidget     * m_pTreeWidget;
        QLabel                       * m_pClassNameLabel;
        QPushButton                  * m_pClassNameRenameButton;
        QLabel                       * m_pFunctionMemberNameLabel;
        QPushButton                  * m_pFunctionMemberNameRenameButton;

	KviClassEditorTreeWidgetItem * m_pLastEditedItem;
	KviClassEditorTreeWidgetItem * m_pLastClickedItem;
	KviTalPopupMenu              * m_pContextPopup;
	QSplitter                    * m_pSplitter;
	QString                        m_szDir;
	bool                           m_bSaving;
public:
	//bool modified(){ return m_bModified; };
        void saveProperties(KviConfig *);
        void loadProperties(KviConfig *);
        void loadUnbuildedFunctions();
        KviClassEditorTreeWidgetItem * findHandler(KviClassEditorTreeWidgetItem *pItemClass,const QString &szFunctionName);
        void commit();

    /*
        void exportClasses(bool,bool=false);
        void exportSelectionInSinglesFiles(KviPointerList<KviClassEditorTreeWidgetItem> *l);


	static void splitFullClassOrNamespaceName(const QString &szFullName,QStringList &lNamespaces,QString &szName);
        */
protected slots:

       void currentItemChanged(QTreeWidgetItem *it,QTreeWidgetItem *);
       void itemPressed(QTreeWidgetItem *,QPoint);
       void newClass();
       void newNamespace();
       void newFunctionMember();

       void newItem(QString &szName,KviClassEditorTreeWidgetItem::Type eType);
     /*   void renameItem();


	void exportAll();
	void exportSelectedSepFiles();
	void exportSelected();
        void removeSelectedItems();



	void slotFind();
	void slotCollapseNamespaces();
	void slotFindWord(const QString &);
	void slotReplaceAll(const QString &before,const QString &after);
	void itemRenamed(QTreeWidgetItem *it,int col);
        void classRefresh(const QString &szName);
        */

protected:
       KviPointerHashTable<QString,KviClassEditorTreeWidgetItem> *m_pClasses;
     //   KviClassEditorTreeWidgetItemDelegate *m_pItemDelegate;
      /*  void recursiveCollapseNamespaces(KviClassEditorTreeWidgetItem * it);
	void recursiveSearchReplace(const QString &szSearch,KviClassEditorTreeWidgetItem * it,bool bReplace=false,const QString &szReplace="n");

	void recursiveCommit(KviClassEditorTreeWidgetItem * it);
        void getExportClassBuffer(QString &buffer,KviClassEditorTreeWidgetItem * it);
        */
	void oneTimeSetup();
	void saveLastEditedItem();
        /*void getUniqueItemName(KviClassEditorTreeWidgetItem *item,QString &buffer,KviClassEditorTreeWidgetItem::Type eType);
        void appendSelectedClassItems(KviPointerList<KviClassEditorTreeWidgetItem> * l);
        void appendSelectedClassItemsRecursive(KviPointerList<KviClassEditorTreeWidgetItem> * l,QTreeWidgetItem * pStartFrom);
        void appendAllClassItems(KviPointerList<KviClassEditorTreeWidgetItem> * l);
        void appendAllClassItemsRecursive(KviPointerList<KviClassEditorTreeWidgetItem> * l,QTreeWidgetItem * pStartFrom);
*/
	void openParentItems(QTreeWidgetItem * it);
	void activateItem(QTreeWidgetItem * it);
        bool hasSelectedItems();
/*
        bool removeItem(KviClassEditorTreeWidgetItem *it,bool * pbYesToAll,bool bDeleteEmptyTree);
        void removeItemChildren(KviClassEditorTreeWidgetItem *it);

        QString askForClassName(const QString &szAction,const QString &szText,const QString &szInitialText);
	QString askForNamespaceName(const QString &szAction,const QString &szText,const QString &szInitialText);


        bool namespaceExists(QString &szFullItemName);
	bool classExists(QString &szFullItemName);
        void appendAllItems(KviPointerList<KviClassEditorTreeWidgetItem> * l,KviClassEditorTreeWidgetItem::Type);
        void appendAllItemsRecursive(KviPointerList<KviClassEditorTreeWidgetItem> * l,QTreeWidgetItem * pStartFrom,KviClassEditorTreeWidgetItem::Type);



        KviClassEditorTreeWidgetItem * createFullItemRecursive(QTreeWidgetItem *pCurrent,QStringList & lNameSpaces,int & iLevel);
        KviClassEditorTreeWidgetItem * findItemRecursive(QTreeWidgetItem *it,QStringList & lNameSpaces,int & iLevel);

        */
     //   bool itemExistsRecursive(QTreeWidgetItem *pSearchFor,QTreeWidgetItem * pSearchAt);
        bool itemExists(QTreeWidgetItem *pSearchFor);
        bool itemExists(const QString &szFullName);

        void buildFullItemPath(KviClassEditorTreeWidgetItem * it,QString &szBuffer);
        QString buildFullClassName(KviClassEditorTreeWidgetItem * it);
        KviClassEditorTreeWidgetItem * createFullItem(const QString &szFullName);
        KviClassEditorTreeWidgetItem * findItem(const QString &szName);
        KviClassEditorTreeWidgetItem * findTopLevelItem(const QString &szName);
        KviClassEditorTreeWidgetItem * createFullItemRecursive(QTreeWidgetItem *pCurrent,QStringList & lNameSpaces,int & iLevel,bool bCreateNameSpace=0);
};


class KviClassEditorWindow : public KviWindow
{
	Q_OBJECT
public:
	KviClassEditorWindow(KviFrame * lpFrm);
	~KviClassEditorWindow();
protected:
	KviClassEditor * m_pEditor;
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
/*
class KviClassEditorDialog: public QDialog
{
        Q_OBJECT
public:
        KviClassEditorDialog(QWidget * pParent, const QString & szName, KviPointerHashTable<QString,KviClassEditorTreeWidgetItem> *pClasses);

        ~KviClassEditorDialog();
public:
        QLineEdit * m_pClassNameLineEdit;
        QComboBox * m_pInerithClassComboBox;
protected:
        QPushButton * m_pNewClassButton;

        QWidget     * m_pParent;
protected slots:
        void textChanged(const QString &);
        void newClass();
/*signals:
        void replaceAll(const QString &, const QString &);
        void initFind();
        void nextFind(const QString &);

};*/

#endif //_ALIASEDITOR_H_
