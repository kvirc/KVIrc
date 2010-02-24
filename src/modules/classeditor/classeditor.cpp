//=============================================================================
//
//   File : classeditor.cpp
//   Creation date : Mon Dec 23 2002 14:36:18 by Szymon Stefanek
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


#include "classeditor.h"

#include "kvi_iconmanager.h"
#include "kvi_options.h"
#include "kvi_locale.h"
#include "kvi_imagedialog.h"
#include "kvi_config.h"
#include "kvi_filedialog.h"
#include "kvi_fileutils.h"
#include "kvi_scripteditor.h"
#include "kvi_debug.h"
#include "kvi_app.h"
#include "kvi_frame.h"
#include "kvi_qstring.h"

#include "kvi_kvs_kernel.h"
#include "kvi_kvs_object_class.h"
#include "kvi_kvs_object_controller.h"
#include "kvi_filedialog.h"
#include "kvi_cmdformatter.h"
#include "kvi_module.h"
#include "kvi_tal_vbox.h"
#include "kvi_fileextensions.h"

#include <QList>
#include <QSplitter>
#include <QLayout>
#include <QToolTip>
#include <QPushButton>
#include <QDir>
#include <QMessageBox>
#include <QInputDialog>
#include <QRegExp>
#include <QTreeWidget>
#include <QMouseEvent>
#include <QList>
#include <QPainter>
#include <QBrush>
extern KviClassEditorWindow * g_pClassEditorWindow;
extern KviModule * g_pClassEditorModule;

KviClassEditorTreeWidgetItem::KviClassEditorTreeWidgetItem(QTreeWidget * pTreeWidget,Type eType,const QString &szName)
: QTreeWidgetItem(pTreeWidget), KviHeapObject(), m_eType(eType), m_pParentItem(0)
{
	setName(szName);
        m_cPos=0;

        if(eType==KviClassEditorTreeWidgetItem::Namespace) setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NAMESPACE))));
           else setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIAS))));
        m_bClassModified=false;

}

KviClassEditorTreeWidgetItem::KviClassEditorTreeWidgetItem(KviClassEditorTreeWidgetItem * pParentItem,Type eType,const QString &szName)
: QTreeWidgetItem(pParentItem), m_eType(eType), m_pParentItem(pParentItem)
{
	setName(szName);
        m_cPos=0;
	setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled|Qt::ItemIsSelectable);
        if(eType==KviClassEditorTreeWidgetItem::Namespace) setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NAMESPACE))));
           else if(eType==KviClassEditorTreeWidgetItem::Class) setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIAS))));
        else setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SPAM))));
        m_bClassModified=false;
     }

void KviClassEditorTreeWidgetItem::setName(const QString &szName)
{
	m_szName = szName;
	setText(0,m_szName);
}
void KviClassEditorTreeWidgetItem::setType(Type t)
{
     m_eType=t;
     if(t==KviClassEditorTreeWidgetItem::Namespace) setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NAMESPACE))));
        else if(t==KviClassEditorTreeWidgetItem::Class) setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIAS))));
     else setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SPAM))));

     
}

KviClassEditorTreeWidget::KviClassEditorTreeWidget(QWidget * par)
: QTreeWidget(par)
{
        setColumnCount (1);
        setHeaderLabel(__tr2qs_ctx("Class","classeditor"));
        setSelectionMode(QAbstractItemView::ExtendedSelection);
        setSortingEnabled(true);
        setRootIsDecorated(true);
        setAnimated(true);
}

KviClassEditorTreeWidget::~KviClassEditorTreeWidget()
{
        //remove and delete all items
        clear();
}

void KviClassEditorTreeWidget::mousePressEvent (QMouseEvent *e)
{
        if (e->button() == Qt::RightButton)
        {
                QTreeWidgetItem *i=itemAt(e->pos());
                if (i) emit rightButtonPressed(i,QCursor::pos());
        }
        QTreeWidget::mousePressEvent(e);
}
KviClassEditor::KviClassEditor(QWidget * par)
: QWidget(par)
{
   m_pClasses=new KviPointerHashTable<QString,KviClassEditorTreeWidgetItem>(100,false);
    m_pClasses->setAutoDelete(false);
   // m_bSaving = false;
    m_pLastEditedItem = 0;
    m_pLastClickedItem = 0;
    m_szDir = QDir::homePath();

    QGridLayout * l = new QGridLayout(this);

    m_pSplitter = new QSplitter(Qt::Horizontal,this);
    m_pSplitter->setOpaqueResize(false);
    l->addWidget(m_pSplitter,0,0);

    KviTalVBox * box = new KviTalVBox(m_pSplitter);
    box->setSpacing(0);
    box->setMargin(0);
    m_pTreeWidget = new KviClassEditorTreeWidget(box);

    box = new KviTalVBox(m_pSplitter);

    KviTalHBox * hbox = new KviTalHBox(box);
    hbox->setSpacing(0);
    hbox->setMargin(0);
    m_pClassNameLabel = new QLabel(__tr2qs_ctx("No item selected","editor"),hbox);
    m_pClassNameRenameButton = new QPushButton(__tr2qs_ctx("Rename","editor"),hbox);
    m_pClassNameRenameButton->setEnabled(false);
    connect(m_pClassNameRenameButton,SIGNAL(clicked()),this,SLOT(renameItem()));
    hbox->setStretchFactor(m_pClassNameLabel,2);
    m_pClassNameRenameButton->setToolTip(__tr2qs_ctx("Edit the class or namespace name","editor"));

    hbox = new KviTalHBox(box);
    hbox->setSpacing(0);
    hbox->setMargin(0);
    m_pFunctionMemberNameLabel = new QLabel(__tr2qs_ctx("No item selected","editor"),hbox);
    m_pFunctionMemberNameRenameButton = new QPushButton(__tr2qs_ctx("Rename","editor"),hbox);
    m_pFunctionMemberNameRenameButton->setEnabled(false);
    connect(m_pFunctionMemberNameRenameButton,SIGNAL(clicked()),this,SLOT(renameMember()));
    hbox->setStretchFactor(m_pFunctionMemberNameLabel,2);
    m_pFunctionMemberNameRenameButton->setToolTip(__tr2qs_ctx("Edit the function member name","editor"));



    m_pEditor = KviScriptEditor::createInstance(box);
    m_pEditor->setFocus();

    connect(m_pEditor,SIGNAL(find(const QString &)),this,SLOT(slotFindWord(const QString &)));
    connect(m_pEditor,SIGNAL(replaceAll(const QString &,const QString &)),this,SLOT(slotReplaceAll(const QString &,const QString &)));

    m_pContextPopup = new KviTalPopupMenu(this);

    oneTimeSetup();

    currentItemChanged(0,0);
}
KviClassEditor::~KviClassEditor()
{
   // KviScriptEditor::destroyInstance(m_pEditor);
    //m_pClasses->clear();
    //delete m_pClasses;
}

void KviClassEditor::buildFullItemPath(KviClassEditorTreeWidgetItem * it,QString &szBuffer)
{
        if (!it) return;
        it=it->parentItem();
        while(it)
        {
            QString tmp = it->name();
            if(!tmp.isEmpty())
            {
                    szBuffer.prepend("::");
                    szBuffer.prepend(tmp);
            }
            it = it->parentItem();
        }
}

QString KviClassEditor::buildFullClassName(KviClassEditorTreeWidgetItem * it)
{
        if(!it) return QString();
        QString szName = it->name();
        KviClassEditorTreeWidgetItem * nit = it->parentItem();

        while(nit)
        {
                QString tmp = nit->name();
                if(!tmp.isEmpty())
                {
                        szName.prepend("::");
                        szName.prepend(tmp);
                }
                nit = nit->parentItem();
        }
        return szName;
}

KviClassEditorTreeWidgetItem * KviClassEditor::findTopLevelItem(const QString &szName)
{
        for(int i=0;i<m_pTreeWidget->topLevelItemCount();i++)
        {
            if (KviQString::equalCI(m_pTreeWidget->topLevelItem(i)->text(0),szName))
                return (KviClassEditorTreeWidgetItem*)m_pTreeWidget->topLevelItem(i);
        }
        return 0;
}

KviClassEditorTreeWidgetItem * KviClassEditor::findItem(const QString &szFullName)
{
        QStringList lNamespaces=szFullName.split("::");
        if(!lNamespaces.count()) return 0;
        KviClassEditorTreeWidgetItem *pItem=findTopLevelItem(lNamespaces.at(0));
        if (!pItem) return 0;
        bool bFound;
        for(int i=1;i<lNamespaces.count()-1;i++)
        {
            bFound=false;
            for(int j=0;j<pItem->childCount();j++)
            {
                if (KviQString::equalCI(pItem->child(j)->text(0),lNamespaces.at(i)))
                {
                    pItem=( KviClassEditorTreeWidgetItem *)pItem->child(j);
                    bFound=true;
                    break;
                }
            }
            if (!bFound) return 0;
        }
        return  (KviClassEditorTreeWidgetItem *)pItem;
}
bool KviClassEditor::itemExists(const QString &szFullName)
{

    if (findItem(szFullName)) return true;
    else return false;
}

KviClassEditorTreeWidgetItem * KviClassEditor::createFullItem(const QString &szFullName)
{
    QStringList lNamespaces=szFullName.split("::");
    if(!lNamespaces.count()) return 0;
    if (lNamespaces.count()==1) return new KviClassEditorTreeWidgetItem(m_pTreeWidget,KviClassEditorTreeWidgetItem::Class,lNamespaces.at(0));
    KviClassEditorTreeWidgetItem *pItem=findTopLevelItem(lNamespaces.at(0));
    if (!pItem) pItem=new KviClassEditorTreeWidgetItem(m_pTreeWidget,KviClassEditorTreeWidgetItem::Namespace,lNamespaces.at(0));
    bool bFound;
    int i;
    for(i=1;i<lNamespaces.count()-1;i++)
    {
        bFound=false;
        for(int j=0;j<pItem->childCount();j++)
        {
            if (KviQString::equalCI(pItem->child(j)->text(0),lNamespaces.at(i)))
            {
                 pItem=( KviClassEditorTreeWidgetItem *)pItem->child(j);
                 bFound=true;
                 break;
            }
        }
        if (!bFound)
             pItem=new KviClassEditorTreeWidgetItem(pItem,KviClassEditorTreeWidgetItem::Namespace,lNamespaces.at(i));
    }
    return new KviClassEditorTreeWidgetItem(pItem,KviClassEditorTreeWidgetItem::Class,lNamespaces.at(i));
}
void KviClassEditor::oneTimeSetup()
{
    QString szPath;
    g_pApp->getLocalKvircDirectory(szPath,KviApp::Classes);
    QDir d(szPath);
    QStringList sl;
    sl = d.entryList(QDir::Files);
    KviKvsObjectClass *pClass;
    KviClassEditorTreeWidgetItem *pClassItem;
    for(int i=0;i<sl.count();i++)
    {
            QString szClassName=sl.at(i);
            szClassName.replace("--","::");
            if (szClassName!=".." && szClassName!=".")
            {
                    szClassName.chop(4);
                    pClassItem = createFullItem(szClassName);
                    pClass = KviKvsKernel::instance()->objectController()->lookupClass(szClassName);
                    KviClassEditorTreeWidgetItem *pFunctionItem;
                    if (pClass)
                    {
                          KviPointerHashTableIterator<QString,KviKvsObjectFunctionHandler>  it(* pClass->getHandlers());
                          while(it.current())
                          {
                                  QString szCode;
                                  KviKvsObjectFunctionHandler *handler=pClass->lookupFunctionHandler(it.currentKey());
                                  if (pClass->isScriptHandler(it.currentKey()))
                                  {
                                      pFunctionItem=new KviClassEditorTreeWidgetItem(pClassItem,KviClassEditorTreeWidgetItem::Method,it.currentKey());
                                      pClass->getFunctionCode(szCode,*handler);
                                      m_pClasses->insert(szClassName,pClassItem);
                                      pFunctionItem->setBuffer(szCode);
                                  }
                                  ++it;
                          }
                     }
             }
    }

    KviPointerHashTableIterator<QString,KviKvsObjectClass> it(*KviKvsKernel::instance()->objectController()->classDict());
    while(KviKvsObjectClass * pClass=it.current())
    {
            if (pClass->isBuiltin())m_pClasses->insert(it.currentKey(),0);
            ++it;
    }
    connect(m_pTreeWidget,SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)),this,SLOT(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)));
    connect(m_pTreeWidget,SIGNAL(rightButtonPressed(QTreeWidgetItem *,QPoint)),this,SLOT(itemPressed(QTreeWidgetItem *,QPoint)));

}
/*
void KviClassEditor::classRefresh(const QString &szName)
{
        if(m_bSaving)
                return;
        KviClassEditorTreeWidgetItem * item;
        KviKvsScript * class = KviKvsClassManager::instance()->classDict()->find(szName);
        item = createFullItem(szName);
        if(item!=m_pLastEditedItem)
        {
                item->setBuffer(class->code());
                return;
        }
        if(
                QMessageBox::warning(0,__tr2qs_ctx("OverWrite Current Class","editor"),
                                __tr2qs_ctx("An external script has changed the class you are currently editing. Do you want to accept the external changes?","editor"),
                                QMessageBox::Yes,QMessageBox::No|QMessageBox::Default|QMessageBox::Escape) != QMessageBox::Yes
                )
                return;
        item->setBuffer(class->code());
        m_pEditor->setText(class->code());
}
void KviClassEditor::itemRenamed(QTreeWidgetItem *it,int col)
{
        if(it!=m_pLastEditedItem) return;

        ((KviClassEditorTreeWidgetItem *)it)->setName(it->text(col));
        QString szNam = buildFullItemName((KviClassEditorTreeWidgetItem *)it);
        QString szLabelText;
        if(((KviClassEditorTreeWidgetItem *)it)->isNamespace())
                szLabelText = __tr2qs_ctx("Namespace","editor");
        else
                szLabelText = __tr2qs_ctx("Class","editor");
        szLabelText += ": <b>";
        szLabelText += szNam;
        szLabelText += "</b>";
        m_pClassNameLabel->setText(szLabelText);
}

*/

bool KviClassEditor::hasSelectedItems()
{
    return m_pTreeWidget->selectedItems().count()?1:0;
}

bool KviClassEditor::itemExists(QTreeWidgetItem *pSearchFor)
{
    if(!pSearchFor)return false;
    if(m_pClasses->findRef((KviClassEditorTreeWidgetItem*) pSearchFor)) return true;
    else return false;
}

/*
void KviClassEditor::appendAllItems(KviPointerList<KviClassEditorTreeWidgetItem> * l,KviClassEditorTreeWidgetItem::Type eType)
{
        for (int i=0;i<m_pTreeWidget->topLevelItemCount();i++)
        {
                if (((KviClassEditorTreeWidgetItem *)m_pTreeWidget->topLevelItem(i))->type()==eType)
                {
                        l->append(((KviClassEditorTreeWidgetItem *)m_pTreeWidget->topLevelItem(i)));
                }
                else
                {
                        appendAllItemsRecursive(l,m_pTreeWidget->topLevelItem(i),eType);
                }
        }

}

void KviClassEditor::appendAllItemsRecursive(KviPointerList<KviClassEditorTreeWidgetItem> * l,QTreeWidgetItem * pStartFrom,KviClassEditorTreeWidgetItem::Type eType)
{
        for (int i=0;i<pStartFrom->childCount();i++)
        {
                if (((KviClassEditorTreeWidgetItem *)pStartFrom->child(i))->type()==eType)
                {
                        l->append((KviClassEditorTreeWidgetItem *)pStartFrom->child(i));
                }
                else
                {
                        appendAllItemsRecursive(l,pStartFrom->child(i),eType);
                }
        }

}

bool KviClassEditor::classExists(QString &szFullItemName)
{
        KviPointerList<KviClassEditorTreeWidgetItem> l;
        l.setAutoDelete(false);
        appendAllItems(&l,KviClassEditorTreeWidgetItem::Class);
        for(KviClassEditorTreeWidgetItem * it = l.first();it;it = l.next())
        {
                if (KviQString::equalCI(buildFullItemName(it),szFullItemName))
                {
                        return true;
                }
        }
        return false;
}

bool KviClassEditor::namespaceExists(QString &szFullItemName)
{
        KviPointerList<KviClassEditorTreeWidgetItem> l;
        l.setAutoDelete(false);

        appendAllItems(&l,KviClassEditorTreeWidgetItem::Namespace);
        for(KviClassEditorTreeWidgetItem * it = l.first();it;it = l.next())
        {
                if (KviQString::equalCI(buildFullItemName(it),szFullItemName))
                {
                        return true;
                }
        }
        return false;
}
void KviClassEditor::renameItem()
{
        if(!m_pLastEditedItem)return;
        if(!itemExists(m_pLastEditedItem))return; // dead ?

        QString szName = buildFullItemName(m_pLastEditedItem);
        QString szNewName;

        bool bClass = m_pLastEditedItem->isClass();

        if(bClass)
                szNewName = askForClassName(__tr2qs_ctx("Rename Class","editor"),__tr2qs_ctx("Please enter the new name for the class","editor"),szName);
        else
                szNewName = askForNamespaceName(__tr2qs_ctx("Rename Namespace","editor"),__tr2qs_ctx("Please enter the new name for the namespace","editor"),szName);
        if(szNewName.isEmpty())return;

        if(szName == szNewName)return;

        // check if there is already an class with this name
        if (bClass)
        {
                if (classExists(szNewName))
                {
                        g_pClassEditorModule->lock();
                        QMessageBox::information(this,
                                __tr2qs_ctx("Class already exists","editor"),
                                __tr2qs_ctx("This name is already in use. Please choose another one.","editor"),
                                __tr2qs_ctx("Ok, Let me try again...","editor"));
                        g_pClassEditorModule->unlock();
                        return;
                }
        } else {
        // check if there is already a namespace with this name
                if (namespaceExists(szNewName))
                {
                        g_pClassEditorModule->lock();
                        QMessageBox::information(this,
                                __tr2qs_ctx("Namespace already exists","editor"),
                                __tr2qs_ctx("This name is already in use. Please choose another one.","editor"),
                                __tr2qs_ctx("Ok, Let me try again...","editor"));
                        g_pClassEditorModule->unlock();
                        return;
                }
        }

        QString szCode;
        int pntCursor;
        if(bClass)
        {
                m_pEditor->getText(szCode);
                pntCursor = m_pEditor->getCursor();
        }
        QList<QTreeWidgetItem*> lChildren= m_pLastEditedItem->takeChildren();
        bool bYesToAll = true;
        removeItem(m_pLastEditedItem,&bYesToAll,true);

        m_pLastEditedItem = 0; // make sure it's true (but it already should be because of removeItem())
        m_pLastClickedItem = 0; // make sure it's true (but it already should be because of removeItem())
         KviClassEditorTreeWidgetItem * pItem = createFullItem(szNewName);
        if(bClass)
        {
                pItem->setBuffer(szCode);
                pItem->setCursorPosition(pntCursor);

        } else {
                for(int i=0;i<lChildren.count();i++)
                {
                        ((KviClassEditorTreeWidgetItem*)lChildren.at(i))->setParentItem(pItem);
                        pItem->insertChild(pItem->childCount(),lChildren.at(i));
                }

        }
        activateItem(pItem);

}*/

// warning: this saves ONLY the body, not the name!
//          the name is changed explicitly with renameItem(), when needed

void KviClassEditor::saveLastEditedItem()
{
       if(!m_pLastEditedItem)
                return;
       if(!itemExists(m_pLastEditedItem) || !m_pEditor->isModified() || m_pLastEditedItem->isNamespace())
                return;

        ((KviClassEditorTreeWidgetItem *)m_pLastEditedItem)->setCursorPosition(m_pEditor->getCursor());
        QString newCode;
        m_pEditor->getText(newCode);
        ((KviClassEditorTreeWidgetItem *)m_pLastEditedItem)->setBuffer(newCode);
}

void KviClassEditor::currentItemChanged(QTreeWidgetItem *it,QTreeWidgetItem *)
{
        saveLastEditedItem();
        m_pLastEditedItem = (KviClassEditorTreeWidgetItem *)it;

        if(!m_pLastEditedItem)
        {
                m_pClassNameLabel->setText(__tr2qs_ctx("No item selected","editor"));
                m_pClassNameRenameButton->setEnabled(false);
                m_pEditor->setText("");
                m_pEditor->setEnabled(false);
                return;
        }
        KviClassEditorTreeWidgetItem * pClassItem;
        if(m_pLastEditedItem->isMethod()) pClassItem=m_pLastEditedItem->parentItem();
        else pClassItem=m_pLastEditedItem;
        QString szClassName = buildFullClassName(pClassItem);
        if(m_pLastEditedItem->isNamespace())
        {
                QString szLabelText = __tr2qs_ctx("Namespace","editor");
                szLabelText += ": <b>";
                szLabelText += szClassName;
                szLabelText += "</b>";
                m_pClassNameLabel->setText(szLabelText);
                m_pClassNameRenameButton->setEnabled(true);
                m_pFunctionMemberNameRenameButton->setEnabled(false);
                m_pEditor->setText("");
                m_pEditor->setEnabled(false);
                m_pTreeWidget->setFocus();
                return;
        }

        QString szLabelText = __tr2qs_ctx("Class","editor");
        szLabelText += ": <b>";
        szLabelText += szClassName;
        szLabelText += "</b>";
        m_pClassNameLabel->setText(szLabelText);
        m_pClassNameRenameButton->setEnabled(true);

        szLabelText = __tr2qs_ctx("Function Member:","editor");
        if(m_pLastEditedItem->isMethod()){
            szLabelText += ": <b>";
            szLabelText += m_pLastEditedItem->text(0);
            szLabelText += "</b>";
            m_pFunctionMemberNameRenameButton->setEnabled(true);
        }
        else
            m_pFunctionMemberNameRenameButton->setEnabled(false);
        m_pFunctionMemberNameLabel->setText(szLabelText);



        m_pEditor->setText(((KviClassEditorTreeWidgetItem *)it)->buffer());

        m_pEditor->setFocus();
        m_pEditor->setCursorPosition(((KviClassEditorTreeWidgetItem *)it)->cursorPosition());
        m_pEditor->setEnabled(true);

}


void KviClassEditor::itemPressed(QTreeWidgetItem *it,QPoint pnt)
{
        m_pContextPopup->clear();

        m_pLastClickedItem = (KviClassEditorTreeWidgetItem *)it;

        int id;

        id = m_pContextPopup->insertItem(
                        *(g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIAS)),
                        __tr2qs_ctx("Add Namespace","editor"),
                        this,SLOT(newNamespace()));
        m_pContextPopup->setItemEnabled(id,m_pLastClickedItem->isNamespace());

        id = m_pContextPopup->insertItem(
                        *(g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIAS)),
                        __tr2qs_ctx("Add Class","editor"),
                        this,SLOT(newClass()));
        m_pContextPopup->setItemEnabled(id,m_pLastClickedItem->isNamespace());


        id = m_pContextPopup->insertItem(
                        *(g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIAS)),
                        __tr2qs_ctx("Add Function Member","editor"),
                        this,SLOT(newFunctionMember()));
        m_pContextPopup->setItemEnabled(id,m_pLastClickedItem->isClass()| m_pLastClickedItem->isMethod());


        bool bHasItems = m_pTreeWidget->topLevelItemCount();
        bool bHasSelected = hasSelectedItems();

        m_pContextPopup->insertSeparator();

        id = m_pContextPopup->insertItem(
                        *(g_pIconManager->getSmallIcon(KVI_SMALLICON_QUIT)),
                        __tr2qs_ctx("Remove Selected","editor"),
                        this,SLOT(removeSelectedItems()));
        m_pContextPopup->setItemEnabled(id,bHasSelected);

        m_pContextPopup->insertSeparator();

        m_pContextPopup->insertItem(
                        *(g_pIconManager->getSmallIcon(KVI_SMALLICON_FOLDER)),
                        __tr2qs_ctx("Export Selected...","editor"),
                        this,SLOT(exportSelected()));
        m_pContextPopup->setItemEnabled(id,bHasSelected);

        m_pContextPopup->insertItem(
                        *(g_pIconManager->getSmallIcon(KVI_SMALLICON_FOLDER)),
                        __tr2qs_ctx("Export Selected in singles files...","editor"),
                        this,SLOT(exportSelectedSepFiles()));

        m_pContextPopup->setItemEnabled(id,bHasSelected);

        m_pContextPopup->insertItem(
                        *(g_pIconManager->getSmallIcon(KVI_SMALLICON_FOLDER)),
                        __tr2qs_ctx("Export All...","editor"),
                        this,SLOT(exportAll()));
        m_pContextPopup->setItemEnabled(id,bHasItems);

        m_pContextPopup->insertSeparator();

        m_pContextPopup->insertItem(
                        *(g_pIconManager->getSmallIcon(KVI_SMALLICON_SEARCH)),
                        __tr2qs_ctx("Find In Classes...","editor"),
                        this,SLOT(slotFind()));
        m_pContextPopup->setItemEnabled(id,bHasItems);

        m_pContextPopup->insertItem(
                        *(g_pIconManager->getSmallIcon(KVI_SMALLICON_NAMESPACE)),
                        __tr2qs_ctx("Collapse All Namespaces","editor"),
                        this,SLOT(slotCollapseNamespaces()));

        m_pContextPopup->setItemEnabled(id,bHasItems);
        m_pContextPopup->popup(pnt);

}
/*
void KviClassEditor::recursiveSearchReplace(const QString &szSearch,KviClassEditorTreeWidgetItem * it,bool bReplace,const QString &szReplace)
{

        if(!it)return;
        for(int i=0;i< it->childCount();i++)
        {
                if(((KviClassEditorTreeWidgetItem *)it->child(i))->isClass())
                {
                        if(((KviClassEditorTreeWidgetItem *)it->child(i))->buffer().indexOf(szSearch,0,Qt::CaseInsensitive) != -1)
                        {
                                it->child(i)->setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIASHIGHLIGHTED))));
                                if (bReplace) ((QString &)((KviClassEditorTreeWidgetItem *)it->child(i))->buffer()).replace(szSearch,szReplace,Qt::CaseInsensitive);
                                it->setExpanded(true);
                        }
                        else
                        it->child(i)->setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIAS))));
                }
                else {
                        recursiveSearchReplace(szSearch,(KviClassEditorTreeWidgetItem *)(it->child(i)),bReplace,szReplace);
                }
        }

}

void KviClassEditor::slotFind()
{

        g_pClassEditorModule->lock();
        bool bOk;

        QString szSearch = QInputDialog::getText(this,
                __tr2qs_ctx("Find In Classes","editor"),
                __tr2qs_ctx("Please enter the text to be searched for. The matching classes will be highlighted.","editor"),
                QLineEdit::Normal,
                "",
                &bOk);


        g_pClassEditorModule->unlock();
        if(!bOk)return;
        if(szSearch.isEmpty())return;
        m_pEditor->setFindText(szSearch);
        for (int i=0;i<m_pTreeWidget->topLevelItemCount();i++)
        {
                if(((KviClassEditorTreeWidgetItem *)m_pTreeWidget->topLevelItem(i))->isClass())
                {
                        if(((KviClassEditorTreeWidgetItem *)m_pTreeWidget->topLevelItem(i))->buffer().indexOf(szSearch,0,Qt::CaseInsensitive) != -1)
                        {
                                m_pTreeWidget->topLevelItem(i)->setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIASHIGHLIGHTED))));
                        } else {
                                m_pTreeWidget->topLevelItem(i)->setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIAS))));
                        }
                } else {
                        recursiveSearchReplace(szSearch,(KviClassEditorTreeWidgetItem *)m_pTreeWidget->topLevelItem(i));
                }
        }
}

void KviClassEditor::slotFindWord(const QString &szSearch)
{
        m_pEditor->setFindText(szSearch);
}

void KviClassEditor::recursiveCollapseNamespaces(KviClassEditorTreeWidgetItem * it)
{
        if(!it)return;
        for (int i=0;i<it->childCount();i++)
        {
                if(it->child(i)->childCount())
                {
                        it->child(i)->setExpanded(false);
                        recursiveCollapseNamespaces((KviClassEditorTreeWidgetItem *)it->child(i));
                }
        }
}

void KviClassEditor::slotCollapseNamespaces()
{
        for (int i=0;i<m_pTreeWidget->topLevelItemCount();i++)
        {
                if (m_pTreeWidget->topLevelItem(i)->childCount()){
                        m_pTreeWidget->topLevelItem(i)->setExpanded(false);
                        recursiveCollapseNamespaces((KviClassEditorTreeWidgetItem *)m_pTreeWidget->topLevelItem(i));
                }
        }
}

void KviClassEditor::slotReplaceAll(const QString &szFind,const QString &szReplace)
{
        m_pEditor->setFindText(szReplace);

        for (int i=0;i<m_pTreeWidget->topLevelItemCount();i++)
        {
                recursiveSearchReplace(szFind,(KviClassEditorTreeWidgetItem *)m_pTreeWidget->topLevelItem(i),true,szReplace);
        }
}

void KviClassEditor::getExportClassBuffer(QString &buffer,KviClassEditorTreeWidgetItem * it)
{
        QString szBuf = it->buffer();
        KviCommandFormatter::blockFromBuffer(szBuf);
        QString szNam = buildFullItemName(it);

        buffer = "class(";
        buffer += szNam;
        buffer += ")\n";
        buffer += szBuf;
        buffer += "\n";
}

void KviClassEditor::exportAll()
{
        exportClasses(false);
}

void KviClassEditor::exportSelectedSepFiles()
{
        exportClasses (true,true);
}

void KviClassEditor::exportSelected()
{
        exportClasses(true);
}

void KviClassEditor::exportSelectionInSinglesFiles(KviPointerList<KviClassEditorTreeWidgetItem> *l)
{
        if(!m_szDir.endsWith(QString(KVI_PATH_SEPARATOR)))m_szDir += KVI_PATH_SEPARATOR;
        if (!l->first())
        {
                g_pClassEditorModule->lock();
                QMessageBox::warning(this,__tr2qs_ctx("Class Export","editor"),__tr2qs_ctx("There is no selection!","editor"),__tr2qs_ctx("OK","editor"));
                g_pClassEditorModule->unlock();
                return;
        }
        g_pClassEditorModule->lock();

        if(!KviFileDialog::askForDirectoryName(m_szDir,__tr2qs_ctx("Choose a Directory - KVIrc","editor"),m_szDir)){
                g_pClassEditorModule->unlock();
                return;
        }

        if(!m_szDir.endsWith(QString(KVI_PATH_SEPARATOR)))m_szDir += KVI_PATH_SEPARATOR;

        bool bReplaceAll=false;

        for(KviClassEditorTreeWidgetItem * it = l->first();it;it = l->next())
        {
                QString tmp;
                getExportClassBuffer(tmp,it);
                QString szFileName=buildFullItemName(it);
                szFileName += ".kvs";
                szFileName.replace("::","_");
                QString szCompletePath=m_szDir+szFileName;
                if (KviFileUtils::fileExists(szCompletePath) && !bReplaceAll)
                {
                    QString szMsg;
                    KviQString::sprintf(szMsg,__tr2qs_ctx("The file \"%Q\" exists. Do you want to replace it ?","editor"),&szFileName);
                    int ret = QMessageBox::question(this,__tr2qs_ctx("Replace file","editor"),szMsg,__tr2qs_ctx("Yes","editor"),__tr2qs_ctx("Yes to All","editor"),__tr2qs_ctx("No","editor"));
                    if (ret!=2)
                    {
                        KviFileUtils::writeFile(szCompletePath,tmp);
                        if (ret==1) bReplaceAll=true;
                    }

                }
                else
                    KviFileUtils::writeFile(szCompletePath,tmp);
          }
          g_pClassEditorModule->unlock();

}

void KviClassEditor::exportClasses(bool bSelectedOnly,bool bSingleFiles)
{
        saveLastEditedItem();

        KviPointerList<KviClassEditorTreeWidgetItem> l;
        l.setAutoDelete(false);

        QString out;
        if (bSelectedOnly) appendSelectedClassItems(&l);
        else appendAllClassItems(&l);
        if (bSingleFiles)
        {
                exportSelectionInSinglesFiles(&l);
                return;

        }
        int count=0;
        KviClassEditorTreeWidgetItem * tempitem=0;
        for(KviClassEditorTreeWidgetItem * it = l.first();it;it = l.next())
        {
                tempitem=it;
                count ++;
                QString tmp;
                getExportClassBuffer(tmp,it);
                out += tmp;
                out += "\n";
        }
        QString szNameFile;
        if(out.isEmpty())
        {
                g_pClassEditorModule->lock();
                QMessageBox::warning(this,__tr2qs_ctx("Class Export","editor"),__tr2qs_ctx("The exported file would be empty: cowardly refusing to write it","editor"),__tr2qs_ctx("OK","editor"));
                g_pClassEditorModule->unlock();
                return;
        }

        QString szName = m_szDir;

        if(!szName.endsWith(QString(KVI_PATH_SEPARATOR)))szName += KVI_PATH_SEPARATOR;
        QString szFile;
        g_pClassEditorModule->lock();

        if (count==1)
        {
                        QString tmp=buildFullItemName(tempitem);
                        szNameFile = tmp.replace("::","_");
        }
        else szNameFile="classes";
        szName += szNameFile;
        szName += ".kvs";
        if(!KviFileDialog::askForSaveFileName(szFile,__tr2qs_ctx("Choose a Filename - KVIrc","editor"),szName,KVI_FILTER_SCRIPT,false,true,true))
        {
                g_pClassEditorModule->unlock();
                return;
        }
        m_szDir=QFileInfo(szFile).absolutePath();
        g_pClassEditorModule->unlock();

        if(!KviFileUtils::writeFile(szFile,out))
        {
                g_pClassEditorModule->lock();
                QMessageBox::warning(this,__tr2qs_ctx("Write Failed - KVIrc","editor"),__tr2qs_ctx("Unable to write to the class file.","editor"),__tr2qs_ctx("OK","editor"));
                g_pClassEditorModule->unlock();
        }
}
*/
void KviClassEditor::saveProperties(KviConfig *cfg)
{
        cfg->writeEntry("Sizes",m_pSplitter->sizes());
        QString szName;
        if(m_pLastEditedItem)
                szName = buildFullClassName(m_pLastEditedItem);
        cfg->writeEntry("LastClass",szName);

}

void KviClassEditor::loadProperties(KviConfig *cfg)
{
       QList<int> def;
        def.append(20);
        def.append(80);
        m_pSplitter->setSizes(cfg->readIntListEntry("Sizes",def));
        QString tmp = cfg->readEntry("LastClass",QString());

        KviClassEditorTreeWidgetItem * it = findItem(tmp);
        activateItem(it);
}
/*
void KviClassEditor::appendSelectedClassItems(KviPointerList<KviClassEditorTreeWidgetItem> * l)
{
        QList<QTreeWidgetItem *> list=m_pTreeWidget->selectedItems();
        for(int i=0;i<list.count();i++)
        {
            if (((KviClassEditorTreeWidgetItem *)list.at(i))->isClass())
                   l->append((KviClassEditorTreeWidgetItem *)list.at(i));
            else
            {
                   appendSelectedClassItemsRecursive(l,list.at(i));
            }
        }

}
void KviClassEditor::appendSelectedClassItemsRecursive(KviPointerList<KviClassEditorTreeWidgetItem> * l,QTreeWidgetItem * pStartFrom)
{
        for (int i=0;i<pStartFrom->childCount();i++)
        {

            if (((KviClassEditorTreeWidgetItem *)pStartFrom->child(i))->isClass())
                    l->append(((KviClassEditorTreeWidgetItem *)pStartFrom->child(i)));
            else
                    appendSelectedClassItemsRecursive(l,pStartFrom->child(i));
        }
}

void KviClassEditor::appendAllClassItems(KviPointerList<KviClassEditorTreeWidgetItem> * l)
{
        for (int i=0;i<m_pTreeWidget->topLevelItemCount();i++)
        {
                if (((KviClassEditorTreeWidgetItem *)m_pTreeWidget->topLevelItem(i))->isClass())
                {
                        l->append(((KviClassEditorTreeWidgetItem *)m_pTreeWidget->topLevelItem(i)));
                }
                else
                {
                        appendAllClassItemsRecursive(l,m_pTreeWidget->topLevelItem(i));
                }
        }

}

void KviClassEditor::appendAllClassItemsRecursive(KviPointerList<KviClassEditorTreeWidgetItem> * l,QTreeWidgetItem * pStartFrom)
{
        for (int i=0;i<pStartFrom->childCount();i++)
        {
                if (((KviClassEditorTreeWidgetItem *)pStartFrom->child(i))->isClass())
                {
                        l->append((KviClassEditorTreeWidgetItem *)pStartFrom->child(i));
                }
                else
                {
                        appendAllClassItemsRecursive(l,pStartFrom->child(i));
                }
        }

}

void KviClassEditor::removeItemChildren(KviClassEditorTreeWidgetItem *it)
{
        while(it->childCount() > 0)
        {
                KviClassEditorTreeWidgetItem * pChild = (KviClassEditorTreeWidgetItem *)(it->child(0));
                if(pChild->childCount())
                        removeItemChildren(pChild);
                delete pChild;
        }
}

bool KviClassEditor::removeItem(KviClassEditorTreeWidgetItem *it,bool * pbYesToAll,bool)
{
        if(!it)return true;
        QString szMsg;
        QString szName = it->name();

        if(!*pbYesToAll)
        {
                if(it->isClass())
                        KviQString::sprintf(szMsg,__tr2qs_ctx("Do you really want to remove the class \"%Q\" ?","editor"),&szName);
                else {
                        KviQString::sprintf(szMsg,__tr2qs_ctx("Do you really want to remove the namespace \"%Q\" ?","editor"),&szName);
                        szMsg += "<br>";
                        szMsg += __tr2qs_ctx("Please note that all the children items will be deleted too.","editor");
                }

                g_pClassEditorModule->lock();
                int ret = QMessageBox::question(this,__tr2qs_ctx("Remove item","editor"),szMsg,__tr2qs_ctx("Yes","editor"),__tr2qs_ctx("Yes to All","editor"),__tr2qs_ctx("No","editor"));
                g_pClassEditorModule->unlock();
                switch(ret)
                {
                        case 0:
                                // nothing
                        break;
                        case 1:
                                *pbYesToAll = true;
                        break;
                        default:
                                return false;
                        break;
                }
        }

        if(it == m_pLastEditedItem)
                m_pLastEditedItem = 0;
        if(it == m_pLastClickedItem)
                m_pLastClickedItem = 0;
        if (it->childCount())
                removeItemChildren(it);
        delete it;
        return true;
}

void KviClassEditor::removeSelectedItems()
{
        KviPointerList<KviClassEditorTreeWidgetItem> l;
        l.setAutoDelete(false);
        appendSelectedClassItems(&l);

        bool bYesToAll = false;

        for(KviClassEditorTreeWidgetItem *it = l.first();it;it = l.next())
        {
                if(!removeItem(it,&bYesToAll,false))return;
        }


}

QString KviClassEditor::askForClassName(const QString &szAction,const QString &szText,const QString &szInitialText)
{
        bool bOk = false;
        QString szNewName;
        while(szNewName.isEmpty())
        {
                g_pClassEditorModule->lock();
                szNewName = QInputDialog::getText(this,
                                                szAction,
                                                szText,
                                                QLineEdit::Normal,
                                                szInitialText,
                                                &bOk);
                g_pClassEditorModule->unlock();
                if(!bOk)return QString();
                if(szNewName.isEmpty())
                {
                        g_pClassEditorModule->lock();
                        QMessageBox::warning(this,
                                __tr2qs_ctx("Missing Class Name","editor"),
                                __tr2qs_ctx("You must specify a valid name for the class","editor"),
                                __tr2qs_ctx("Ok, Let me try again...","editor"));
                        g_pClassEditorModule->unlock();
                        continue;
                }

                // we allow only [\w:]+
                QRegExp re("[\\w:]+");
                if(!re.exactMatch(szNewName))
                {
                        g_pClassEditorModule->lock();
                        QMessageBox::information(this,
                                __tr2qs_ctx("Bad Class Name","editor"),
                                __tr2qs_ctx("Class names can contain only letters, digits, underscores and '::' namespace separators","editor"),
                                __tr2qs_ctx("Ok, Let me try again...","editor"));
                        g_pClassEditorModule->unlock();
                        szNewName = "";
                        continue;
                }
                // make sure that we have only doubled "::" and not ":" or ":::..."
                QString tmp = szNewName;
                tmp.replace("::","@"); // @ is not allowed by the rule above
                if(tmp.indexOf(":",Qt::CaseInsensitive) != -1)
                {
                        g_pClassEditorModule->lock();
                        QMessageBox::information(this,
                                __tr2qs_ctx("Bad Class Name","editor"),
                                __tr2qs_ctx("Stray ':' character in class name: did you mean ...<namespace>::<name> ?","editor"),
                                __tr2qs_ctx("Ok, Let me try again...","editor"));
                        g_pClassEditorModule->unlock();
                        szNewName = "";
                        continue;
                }
                if(tmp.indexOf("@@",Qt::CaseInsensitive) != -1)
                {
                        g_pClassEditorModule->lock();
                        QMessageBox::information(this,
                                __tr2qs_ctx("Bad Class Name","editor"),
                                __tr2qs_ctx("Found an empty namespace in class name","editor"),
                                __tr2qs_ctx("Ok, Let me try again...","editor"));
                        g_pClassEditorModule->unlock();
                        szNewName = "";
                        continue;
                }
        }
        return szNewName;
}

QString KviClassEditor::askForNamespaceName(const QString &szAction,const QString &szText,const QString &szInitialText)
{
        bool bOk = false;
        QString szNewName;
        while(szNewName.isEmpty())
        {
                g_pClassEditorModule->lock();
                szNewName = QInputDialog::getText(this,
                                                szAction,
                                                szText,
                                                QLineEdit::Normal,
                                                szInitialText,
                                                &bOk);
                g_pClassEditorModule->unlock();
                if(!bOk)return QString();
                if(szNewName.isEmpty())
                {
                        g_pClassEditorModule->lock();
                        QMessageBox::warning(this,
                                __tr2qs_ctx("Missing Namespace Name","editor"),
                                __tr2qs_ctx("You must specify a valid name for the namespace","editor"),
                                __tr2qs_ctx("Ok, Let me try again...","editor"));
                        g_pClassEditorModule->unlock();
                        continue;
                }

                // we allow only [\w:]+
                QRegExp re("[\\w:]+");
                if(!re.exactMatch(szNewName))
                {
                        g_pClassEditorModule->lock();
                        QMessageBox::information(this,
                                __tr2qs_ctx("Bad Namespace Name","editor"),
                                __tr2qs_ctx("Namespace names can contain only letters, digits, underscores and '::' namespace separators","editor"),
                                __tr2qs_ctx("Ok, Let me try again...","editor"));
                        g_pClassEditorModule->unlock();
                        szNewName = "";
                        continue;
                }
                // make sure that we have only doubled "::" and not ":" or ":::..."
                QString tmp = szNewName;
                tmp.replace("::","@"); // @ is not allowed by the rule above
                if(tmp.indexOf(":",Qt::CaseInsensitive) != -1)
                {
                        g_pClassEditorModule->lock();
                        QMessageBox::information(this,
                                __tr2qs_ctx("Bad Namespace Name","editor"),
                                __tr2qs_ctx("Stray ':' character in namespace name: did you mean ...<namespace>::<name> ?","editor"),
                                __tr2qs_ctx("Ok, Let me try again...","editor"));
                        g_pClassEditorModule->unlock();
                        szNewName = "";
                        continue;
                }
                if(tmp.indexOf("@@",Qt::CaseInsensitive) != -1)
                {
                        g_pClassEditorModule->lock();
                        QMessageBox::information(this,
                                __tr2qs_ctx("Bad Namespace Name","editor"),
                                __tr2qs_ctx("Found an empty namespace in namespace name","editor"),
                                __tr2qs_ctx("Ok, Let me try again...","editor"));
                        g_pClassEditorModule->unlock();
                        szNewName = "";
                        continue;
                }
        }
        return szNewName;
}
*/
void KviClassEditor::openParentItems(QTreeWidgetItem * it)
{
        if(it->parent())
        {
                it->parent()->setExpanded(true);
                openParentItems(it->parent());
        }
}

void KviClassEditor::activateItem(QTreeWidgetItem * it)
{
        if (!it) return;
        openParentItems(it);
        m_pTreeWidget->setCurrentItem(it);
}

void KviClassEditor::newClass()
{
        /*QString szNewName = askForClassName(__tr2qs_ctx("Add Member Function","editor"),__tr2qs_ctx("Please enter the name for the new function","editor"),"myfunction");
        if(szNewName.isEmpty())return;
        newItem(szNewName,KviClassEditorTreeWidgetItem::Class);*/
}

void KviClassEditor::newNamespace()
{
/*        QString szName = askForNamespaceName(__tr2qs_ctx("Add Namespace","editor"),__tr2qs_ctx("Please enter the name for the new namespace","editor"),"mynamespace");
        if(szName.isEmpty())return;
        newItem(szName,KviClassEditorTreeWidgetItem::Namespace);*/
}

void KviClassEditor::newFunctionMember()
{
/*        QString szName = askForNamespaceName(__tr2qs_ctx("Add Namespace","editor"),__tr2qs_ctx("Please enter the name for the new namespace","editor"),"mynamespace");
        if(szName.isEmpty())return;
        newItem(szName,KviClassEditorTreeWidgetItem::Namespace);*/
}

void  KviClassEditor::newItem(QString &szName,KviClassEditorTreeWidgetItem::Type eType)
{
        if(m_pLastClickedItem)
                if(!itemExists(m_pLastClickedItem)) m_pLastClickedItem=0;
        if(m_pLastClickedItem) buildFullItemPath(m_pLastClickedItem,szName);
        int idx=1;
        QString szTmp;
        if(findItem(szName)) szName.append("1");
        while(findItem(szName))
        {
              szTmp.setNum(idx);
              szName.chop(szTmp.length());
              szName.append(szTmp);
              idx++;
       }
        KviClassEditorTreeWidgetItem * it;
        it=createFullItem(szName);
        it->setType(eType);
        activateItem(it);
}
/*
void KviClassEditor::recursiveCommit(KviClassEditorTreeWidgetItem * it)
{
        if(!it)
                return;
        if(it->isClass())
        {
                        QString szName = buildFullItemName(it);
//                      debug("Commit class %s",szName.toUtf8().data());
                        //debug("ADDING %s",szName.latin1());
                        // WARNING: On MSVC operator new here is valid ONLY because
                        // KviKvsScript has a non virtual detructor!
                        KviKvsScript * a = new KviKvsScript(szName,((KviClassEditorTreeWidgetItem *)it)->buffer());
                        KviKvsClassManager::instance()->add(szName,a);
                return;
        }

        for (int i=0;i<it->childCount();i++)
                recursiveCommit((KviClassEditorTreeWidgetItem *) it->child(i));

}*/

void KviClassEditor::commit()
{
    /*    m_bSaving = true;
        saveLastEditedItem();

        KviKvsClassManager::instance()->clear();

        for(int i=0;i<m_pTreeWidget->topLevelItemCount();i++)
        {
                KviClassEditorTreeWidgetItem *item=(KviClassEditorTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
                recursiveCommit(item);
        }
        g_pApp->saveClasses();

        m_bSaving = false;*/
}


KviClassEditorWindow::KviClassEditorWindow(KviFrame * lpFrm)
: KviWindow(KVI_WINDOW_TYPE_SCRIPTEDITOR,lpFrm,"classeditor",0)
{
        g_pClassEditorWindow = this;

        setFixedCaption(__tr2qs_ctx("Class Editor","editor"));

        QGridLayout * g = new QGridLayout();

        m_pEditor = new KviClassEditor(this);
        g->addWidget(m_pEditor,0,0,1,4);

        QPushButton * btn = new QPushButton(__tr2qs_ctx("&OK","editor"),this);
        connect(btn,SIGNAL(clicked()),this,SLOT(okClicked()));
        btn->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)));
        g->addWidget(btn,1,1);

        btn = new QPushButton(__tr2qs_ctx("&Apply","editor"),this);
        connect(btn,SIGNAL(clicked()),this,SLOT(applyClicked()));
        btn->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)));
        g->addWidget(btn,1,2);

        btn = new QPushButton(__tr2qs_ctx("Cancel","editor"),this);
        connect(btn,SIGNAL(clicked()),this,SLOT(cancelClicked()));
        btn->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DISCARD)));
        g->addWidget(btn,1,3);

        g->setRowStretch(0,1);
        g->setColumnStretch(0,1);
        setLayout(g);
}

KviClassEditorWindow::~KviClassEditorWindow()
{
        g_pClassEditorWindow = 0;
}

void KviClassEditorWindow::okClicked()
{
        m_pEditor->commit();
        close();
}

void KviClassEditorWindow::applyClicked()
{
        m_pEditor->commit();
}

void KviClassEditorWindow::cancelClicked()
{
        close();
}

QPixmap * KviClassEditorWindow::myIconPtr()
{
        return g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIAS);
}

void KviClassEditorWindow::getConfigGroupName(QString &szName)
{
        szName = "classeditor";
}

void KviClassEditorWindow::saveProperties(KviConfig *cfg)
{
        m_pEditor->saveProperties(cfg);
}

void KviClassEditorWindow::loadProperties(KviConfig *cfg)
{
        m_pEditor->loadProperties(cfg);
}
