#ifndef _CLASSEDITOR_H_
#define _CLASSEDITOR_H_
//=============================================================================
//
//   File : classeditor.h
//   Creation date : Mon Feb 15 2010 14:35:55 CEST by Carbone Alessandro
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2010 Alessandro Carbone (elfonol at gmail dot com)
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
#include "kvi_kvs_object_class.h"

#include <QWidget>
#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QStringList>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QTreeWidget>

class QSplitter;
class KviScriptEditor;
class KviClassEditorTreeWidget;

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
	QString m_szName;
        QString m_szBuffer;
        bool m_bClassModified, m_bInternal;
	QString m_szInheritsClassName;
        int  m_cPos;
public:
	void setInheritsClass(QString szInheritsClassName){m_szInheritsClassName=szInheritsClassName;};
	QString InheritsClass(){return m_szInheritsClassName;};
	const QString & name(){ return m_szName; };
	void setName(const QString &szName);
        void setClassNotBuilt(bool bModified);
        bool classNotBuilt(){return m_bClassModified;};
        void setInternalFunction(bool bInternal){m_bInternal=bInternal;};
        bool isInternalFunction(){return m_bInternal;};
	Type type(){ return m_eType; };
        void setType(Type t);
	bool isClass(){ return m_eType == Class; };
	bool isNamespace(){ return m_eType == Namespace; };
        bool isMethod(){ return m_eType == Method;};
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
	QLabel                       * m_pInheritsClassNameLabel;
        QPushButton                  * m_pClassNameRenameButton;
        QLabel                       * m_pMemberFunctionNameLabel;
        QPushButton                  * m_pMemberFunctionNameRenameButton;

	KviClassEditorTreeWidgetItem * m_pLastEditedItem;
	KviClassEditorTreeWidgetItem * m_pLastClickedItem;
	KviTalPopupMenu              * m_pContextPopup;
	QSplitter                    * m_pSplitter;
	QString                        m_szDir;
	bool                           m_bSaving;
public:
        void saveProperties(KviConfig *);
        void loadProperties(KviConfig *);
        void loadUnbuildedFunctions();
        KviClassEditorTreeWidgetItem * findHandler(KviClassEditorTreeWidgetItem *pItemClass,const QString &szFunctionName);
        void build();
        void saveNotBuiltClasses();
        void loadNotBuiltClasses();
	KviClassEditorTreeWidgetItem * findFunction(const QString &szFunctionName, KviClassEditorTreeWidgetItem *pClass);

        void exportClasses(bool,bool=false);
        void exportSelectionInSinglesFiles(KviPointerList<KviClassEditorTreeWidgetItem> *l);
	void cutItem(KviClassEditorTreeWidgetItem *pItem);

protected slots:

	void currentItemChanged(QTreeWidgetItem *it,QTreeWidgetItem *);
	void customContextMenuRequested(QPoint);
	void newClass();
	void newNamespace();
	void newMemberFunction();
	void renameFunction();
	KviClassEditorTreeWidgetItem * newItem(QString &szName,KviClassEditorTreeWidgetItem::Type eType);

	void renameItem();

	void exportAll();
	void exportSelectedSepFiles();
	void exportSelected();
	void slotFind();
	void slotFindWord(const QString &);
	void searchReplace(const QString &szSearch,bool bReplace=false,const QString &szReplace="n");
	void slotCollapseItems();
	void recursiveCollapseItems(KviClassEditorTreeWidgetItem * it);

	void removeSelectedItems();
/*
	void slotFind();

	void slotReplaceAll(const QString &before,const QString &after);
	void classRefresh(const QString &szName);
*/

protected:
       KviPointerHashTable<QString,KviClassEditorTreeWidgetItem> *m_pClasses;
	//void recursiveSearchReplace(const QString &szSearch,KviClassEditorTreeWidgetItem * it,bool bReplace=false,const QString &szReplace="n");

	void getExportClassBuffer(QString &buffer,KviClassEditorTreeWidgetItem * it);

	void oneTimeSetup();
	void createFullClass(KviKvsObjectClass *pClass, KviClassEditorTreeWidgetItem *pItemClass, const QString &szClassName);

	void saveLastEditedItem();
	void appendSelectedClassItems(KviPointerList<KviClassEditorTreeWidgetItem> * l);
	void appendSelectedClassItemsRecursive(KviPointerList<KviClassEditorTreeWidgetItem> * l,QTreeWidgetItem * pStartFrom);
	

	
	void appendAllClassItems(KviPointerList<KviClassEditorTreeWidgetItem> * l);
	void appendAllClassItemsRecursive(KviPointerList<KviClassEditorTreeWidgetItem> * l,QTreeWidgetItem * pStartFrom);

	void openParentItems(QTreeWidgetItem * it);
	void activateItem(QTreeWidgetItem * it);
        bool hasSelectedItems();
	bool askForClassName(QString &szClassName,QString &szInheritsClassName,bool bEdit);
	bool askForNamespaceName(const QString &szAction,const QString &szText,const QString &szInitialText, QString &szNameBuffer);
	bool askForFunction(QString &szFunctionName,bool * bInternal, const QString &szClassName, bool bRenameMode);
	void searchInheritedClasses(const QString szClass,KviPointerList<KviClassEditorTreeWidgetItem> & lInheritsedClasses);
	bool classExists(QString &szFullItemName);
	void renameClass(KviClassEditorTreeWidgetItem *pClassItem);
	void renameNamespace(KviClassEditorTreeWidgetItem *pNamespaceItem);

	bool removeItem(KviClassEditorTreeWidgetItem *it,KviPointerList <KviClassEditorTreeWidgetItem> &lRemovedItems,bool * pbYesToAll,bool);
	void updateClassHierarchy(KviClassEditorTreeWidgetItem* pClass);

	void removeItemChildren(KviClassEditorTreeWidgetItem *it,KviPointerList <KviClassEditorTreeWidgetItem> &lRemovedItems);
        void buildFullItemPath(KviClassEditorTreeWidgetItem * it,QString &szBuffer);
        QString buildFullClassName(KviClassEditorTreeWidgetItem * it);
        KviClassEditorTreeWidgetItem * createFullItem(const QString &szFullName);
	KviClassEditorTreeWidgetItem * createFullNamespace(const QString & szFullName);

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
        void buildClicked();
        void saveClicked();
};

class KviClassEditorDialog: public QDialog
{
        Q_OBJECT
public:
	KviClassEditorDialog(QWidget * pParent, const QString & szName, KviPointerHashTable<QString,KviClassEditorTreeWidgetItem> *pClasses, const QString &szClassName, const QString &szInheritsClassName,bool bRenameMode=false);

        ~KviClassEditorDialog();
public:
        QString getClassName(){return m_pClassNameLineEdit->text();};
	QString getInheritsClassName(){return m_pInheritsClassComboBox->currentText();};
protected:
        QPushButton * m_pNewClassButton;
        QLineEdit * m_pClassNameLineEdit;
	QComboBox * m_pInheritsClassComboBox;
protected slots:
        void textChanged(const QString &);
};

class KviClassEditorFunctionDialog: public QDialog
{
        Q_OBJECT
public:
	KviClassEditorFunctionDialog(QWidget * pParent, const QString & szName, const QString & szClassName,const QString &szFunctionName, bool bIsInternal, bool bRenameMode=false);

        ~KviClassEditorFunctionDialog();
public:
        QString getFunctionName(){return m_pFunctionNameLineEdit->text();};
        bool isInternalFunction(){return m_pInternalCheckBox->isChecked();};
protected:
        QPushButton * m_pNewFunctionButton;
        QLineEdit * m_pFunctionNameLineEdit;
        QCheckBox * m_pInternalCheckBox;
protected slots:
        void textChanged(const QString &);
};
#endif //_CLASSEDITOR_H_
