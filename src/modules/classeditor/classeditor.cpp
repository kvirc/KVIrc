//=============================================================================
//
//   File : classeditor.cpp
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
#include "kvi_modulemanager.h"

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
	else setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CLASS))));
	m_bClassModified=false;
}

KviClassEditorTreeWidgetItem::KviClassEditorTreeWidgetItem(KviClassEditorTreeWidgetItem * pParentItem,Type eType,const QString &szName)
: QTreeWidgetItem(pParentItem), m_eType(eType), m_pParentItem(pParentItem)
{
	setName(szName);
	m_cPos=0;
	setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled|Qt::ItemIsSelectable);
	if(eType==KviClassEditorTreeWidgetItem::Namespace)
		setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NAMESPACE))));
	else if(eType==KviClassEditorTreeWidgetItem::Class)
		setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CLASS))));
	else setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_FUNCTION))));
	m_bClassModified=false;
}

void KviClassEditorTreeWidgetItem::setClassNotBuilt(bool bModified)
{
	m_bClassModified=bModified;
	if (bModified)
		setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CLASSNOTBUILT))));
	else setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CLASS))));
}

void KviClassEditorTreeWidgetItem::setName(const QString & szName)
{
	m_szName = szName;
	setText(0,m_szName);
}

void KviClassEditorTreeWidgetItem::setType(Type t)
{
	m_eType=t;
	if(t==KviClassEditorTreeWidgetItem::Namespace)
		setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NAMESPACE))));
	else if(t==KviClassEditorTreeWidgetItem::Class)
		setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIAS))));
	else setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SPAM))));
}

KviClassEditorTreeWidget::KviClassEditorTreeWidget(QWidget * par)
: QTreeWidget(par)
{
	setColumnCount (1);
	setHeaderLabel(__tr2qs_ctx("Class","editor"));
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

void KviClassEditorTreeWidget::mousePressEvent (QMouseEvent * e)
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

	m_pInheritsClassNameLabel=new QLabel(__tr2qs_ctx("No item selected","editor"),hbox);

	hbox = new KviTalHBox(box);
	hbox->setSpacing(0);
	hbox->setMargin(0);
	m_pMemberFunctionNameLabel = new QLabel(__tr2qs_ctx("No item selected","editor"),hbox);
	m_pMemberFunctionNameRenameButton = new QPushButton(__tr2qs_ctx("Rename","editor"),hbox);
	m_pMemberFunctionNameRenameButton->setEnabled(false);
	connect(m_pMemberFunctionNameRenameButton,SIGNAL(clicked()),this,SLOT(renameItem()));
	hbox->setStretchFactor(m_pMemberFunctionNameLabel,2);
	m_pMemberFunctionNameRenameButton->setToolTip(__tr2qs_ctx("Edit the function member name","editor"));



	m_pEditor = KviScriptEditor::createInstance(box);
	m_pEditor->setFocus();

	/*  connect(m_pEditor,SIGNAL(find(const QString &)),this,SLOT(slotFindWord(const QString &)));
	connect(m_pEditor,SIGNAL(replaceAll(const QString &,const QString &)),this,SLOT(slotReplaceAll(const QString &,const QString &)));
	*/
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

void KviClassEditor::buildFullItemPath(KviClassEditorTreeWidgetItem * it, QString & szBuffer)
{
	if (!it) return;
	debug("base %s",szBuffer.toUtf8().data());
	szBuffer.prepend(it->name()+"::");
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

KviClassEditorTreeWidgetItem * KviClassEditor::findTopLevelItem(const QString & szName)
{
	for(int i=0;i<m_pTreeWidget->topLevelItemCount();i++)
	{
	if (KviQString::equalCI(m_pTreeWidget->topLevelItem(i)->text(0),szName))
		return (KviClassEditorTreeWidgetItem*)m_pTreeWidget->topLevelItem(i);
	}
	return 0;
}

KviClassEditorTreeWidgetItem * KviClassEditor::findItem(const QString & szFullName)
{
	QStringList lNamespaces=szFullName.split("::");
	if(!lNamespaces.count()) return 0;
	KviClassEditorTreeWidgetItem *pItem=findTopLevelItem(lNamespaces.at(0));
	if (!pItem) return 0;
	bool bFound;
	for(int i=1;i<lNamespaces.count();i++)
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

bool KviClassEditor::itemExists(const QString & szFullName)
{
	if (findItem(szFullName)) return true;
	else return false;
}

KviClassEditorTreeWidgetItem * KviClassEditor::createFullItem(const QString & szFullName)
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
				m_pClasses->insert(szClassName,pClassItem);
				KviKvsObjectClass *pParentClass=pClass->parentClass();
				pClassItem->setInheritsClass(pParentClass->name());
				while(it.current())
				{
					QString szCode;
					KviKvsObjectFunctionHandler *handler=pClass->lookupFunctionHandler(it.currentKey());
					if (pClass->isScriptHandler(it.currentKey()))
					{
						pFunctionItem=new KviClassEditorTreeWidgetItem(pClassItem,KviClassEditorTreeWidgetItem::Method,it.currentKey());
						pClass->getFunctionCode(szCode,*handler);
						pFunctionItem->setBuffer(szCode);
					}
					++it;
				}
			}
		}
	}

	g_pModuleManager->getModule("objects");
	KviPointerHashTableIterator<QString,KviKvsObjectClass> it(*KviKvsKernel::instance()->objectController()->classDict());
	while(KviKvsObjectClass * pClass=it.current())
	{
		if (pClass->isBuiltin())m_pClasses->insert(it.currentKey(),0);
		++it;
	}
	loadNotBuiltClasses();
	connect(m_pTreeWidget,SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)),this,SLOT(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)));
	//connect(m_pTreeWidget,SIGNAL(rightButtonPressed(QTreeWidgetItem *,QPoint)),this,SLOT(itemPressed(QTreeWidgetItem *,QPoint)));
	m_pTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_pTreeWidget,SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(customContextMenuRequested(const QPoint &)));
}

/*
void KviClassEditor::classRefresh(const QString & szName)
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

void KviClassEditor::itemRenamed(QTreeWidgetItem * it, int col)
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
	return m_pTreeWidget->selectedItems().count() ? 1 : 0;
}

bool KviClassEditor::itemExists(QTreeWidgetItem * pSearchFor)
{
	if(!pSearchFor) return false;
	if(m_pClasses->findRef((KviClassEditorTreeWidgetItem*) pSearchFor)) return true;
	else return false;
}

/*
void KviClassEditor::appendAllItems(KviPointerList<KviClassEditorTreeWidgetItem> * l, KviClassEditorTreeWidgetItem::Type eType)
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

void KviClassEditor::appendAllItemsRecursive(KviPointerList<KviClassEditorTreeWidgetItem> * l, QTreeWidgetItem * pStartFrom, KviClassEditorTreeWidgetItem::Type eType)
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
*/
bool KviClassEditor::classExists(QString & szFullItemName)
{
	if (m_pClasses->find(szFullItemName)) return true;
	else return false;


	/*
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
	return false;*/
}

bool KviClassEditor::namespaceExists(QString & szFullItemName)
{
	/*KviPointerList<KviClassEditorTreeWidgetItem> l;
	l.setAutoDelete(false);

	appendAllItems(&l,KviClassEditorTreeWidgetItem::Namespace);
	for(KviClassEditorTreeWidgetItem * it = l.first();it;it = l.next())
	{
		if (KviQString::equalCI(buildFullItemName(it),szFullItemName))
		{
			return true;
		}
	}*/
	return false;
}
void KviClassEditor::renameFunction()
{
	KviClassEditorTreeWidgetItem *pFunction=m_pLastEditedItem;
	QString szClassName = pFunction->parentItem()->name();
	QString szFunctionName = pFunction->name();
	KviClassEditorTreeWidgetItem *pParentClass =  pFunction->parentItem();

	QString szNewFunctionName = szFunctionName;
	bool bInternal;
	bool bOldinternal=bInternal;
	askForFunction(szNewFunctionName, &bInternal,szClassName);
	if (KviQString::equalCI(szFunctionName,szNewFunctionName) && (bInternal==bOldinternal)) return;
	if (functionExists(szNewFunctionName,pParentClass))
	{
		g_pClassEditorModule->lock();
		QMessageBox::information(this,
			__tr2qs_ctx("Function already exists","editor"),
			__tr2qs_ctx("This name is already in use. Please choose another one.","editor"),
			__tr2qs_ctx("Ok, Let me try again...","editor"));
		g_pClassEditorModule->unlock();
		return;
	}
	pFunction->setName(szNewFunctionName);

	pParentClass->setClassNotBuilt(true);
	KviPointerList<KviClassEditorTreeWidgetItem> pInheritsedClasses;
	searchInheritsedClasses(szClassName,pInheritsedClasses);
	for(unsigned int i=0;i<pInheritsedClasses.count();i++)
		pInheritsedClasses.at(i)->setClassNotBuilt(true);
	activateItem(pFunction);
	KviKvsObjectClass *pClass;
	pClass = KviKvsKernel::instance()->objectController()->lookupClass(szClassName);
	if (pClass) KviKvsKernel::instance()->objectController()->deleteClass(pClass);

}
bool KviClassEditor::functionExists(const QString &szFunctionName, KviClassEditorTreeWidgetItem *pClass)
{
	for(int i=0;i<pClass->childCount();i++)
	{
		if(KviQString::equalCI(szFunctionName,((KviClassEditorTreeWidgetItem *)pClass->child(i))->name())) return true;
	}
	return false;
}

void KviClassEditor::renameItem()
{
	if(!m_pLastEditedItem)return;
	//if(!itemExists(m_pLastEditedItem))return; // dead ?
	if(m_pLastEditedItem->isClass()) renameClass(m_pLastEditedItem);
	else if(m_pLastEditedItem->isNamespace()) renameNamespace(m_pLastEditedItem);
	else renameFunction();
}
void KviClassEditor::renameClass(KviClassEditorTreeWidgetItem *pClassItem)
{
	QString szClassName = buildFullClassName(pClassItem);
	QString szNewClassName = szClassName;

	QString szInheritsClassName = pClassItem->InheritsClass();
	QString szNewInheritsClassName = szInheritsClassName;
	bool bOk=askForClassName(szNewClassName,szNewInheritsClassName,true);
	if (!bOk) return;
	if (KviQString::equalCI(szClassName,szNewClassName) && KviQString::equalCI(szInheritsClassName,szNewInheritsClassName)) return;
	if (classExists(szNewClassName) && KviQString::equalCI(szInheritsClassName,szNewInheritsClassName))
	{
		g_pClassEditorModule->lock();
		QMessageBox::information(this,
			__tr2qs_ctx("Class already exists","editor"),
			__tr2qs_ctx("This name is already in use. Please choose another one.","editor"),
			__tr2qs_ctx("Ok, Let me try again...","editor"));
		g_pClassEditorModule->unlock();
		return;
	}
	/*if(szNewClassName.tmp.indexOf("@@",Qt::CaseInsensitive) != -1)
	{
		g_pClassEditorModule->lock();
		QMessageBox::information(this,
			__tr2qs_ctx("Bad Namespace Name","editor"),
			__tr2qs_ctx("Found an empty namespace in namespace name","editor"),
			__tr2qs_ctx("Ok, Let me try again...","editor"));
		g_pClassEditorModule->unlock();
		szNewName = "";
		continue;
	}*/
	KviClassEditorTreeWidgetItem *pNewItem=0;
	m_pClasses->remove(szClassName);
	if(szNewClassName.contains("::"))
	{

		int iIdx=m_pTreeWidget->indexOfTopLevelItem(pClassItem);
		if (iIdx!=-1)
			m_pTreeWidget->takeTopLevelItem(iIdx);
		else
		{
			KviClassEditorTreeWidgetItem *pParent=pClassItem->parentItem();
			pParent->removeChild(pClassItem);
		}
		pNewItem=createFullItem(szNewClassName.left(szNewClassName.lastIndexOf("::")));
		pNewItem->setType(KviClassEditorTreeWidgetItem::Namespace);
		pClassItem->setName(szNewClassName.section("::",-1,-1));
		pNewItem->addChild(pClassItem);
		m_pClasses->insert(szNewClassName,pNewItem);
	}
	else
	{
		pClassItem->setName(szNewClassName);
		m_pClasses->insert(szNewClassName,pClassItem);
	}
	KviKvsObjectClass *pClass;
	pClass = KviKvsKernel::instance()->objectController()->lookupClass(szClassName);
	if (pClass) KviKvsKernel::instance()->objectController()->deleteClass(pClass);
	pClassItem->setInheritsClass(szNewInheritsClassName);
	pClassItem->setClassNotBuilt(true);
	KviPointerList<KviClassEditorTreeWidgetItem> pInheritsedClasses;
	searchInheritsedClasses(szClassName,pInheritsedClasses);
	for(unsigned int i=0;i<pInheritsedClasses.count();i++)
	{
		pInheritsedClasses.at(i)->setClassNotBuilt(true);
		pInheritsedClasses.at(i)->setInheritsClass(szNewClassName);
	}
	if (pNewItem) activateItem(pNewItem);
	else activateItem(pClassItem);
}
/*void KviClassEditor::setInheritedClassesNotBuilt(const QString &szInheritsClassName)
{

}
*/
void KviClassEditor::renameNamespace(KviClassEditorTreeWidgetItem *pOldNamespaceItem)
{
	QString szName = buildFullClassName(m_pLastEditedItem);
	QString szNewNameSpaceName;
	if (!askForNamespaceName(__tr2qs_ctx("Rename Namespace","editor"),__tr2qs_ctx("Please enter the new name for the namespace","editor"),szName,szNewNameSpaceName)) return;
	if (KviQString::equalCI(szName,szNewNameSpaceName)) return;
	KviClassEditorTreeWidgetItem *pItem=findItem(szNewNameSpaceName);
	if (pItem)
	{
		g_pClassEditorModule->lock();
		if (pItem->isClass())
		{
			QMessageBox::information(this,
				__tr2qs_ctx("Name already exists as Class name","editor"),
				__tr2qs_ctx("This name is already in use as Class name. Please choose another one.","editor"),
				__tr2qs_ctx("Ok, Let me try again...","editor"));
		}
		else
		{
			QMessageBox::information(this,
			__tr2qs_ctx("Namespace already exists","editor"),
			__tr2qs_ctx("This name is already in use. Please choose another one.","editor"),
			__tr2qs_ctx("Ok, Let me try again...","editor"));
		}
		g_pClassEditorModule->unlock();
//		renameNamespace(pNamespaceItem);
	}

/*	if(szNewNameSpaceName.contains("::"))
	{

		int iIdx=m_pTreeWidget->indexOfTopLevelItem(pOldNamespaceItem);
		if (iIdx!=-1)
			m_pTreeWidget->takeTopLevelItem(iIdx);
		else
		{
			KviClassEditorTreeWidgetItem *pParent=pOldNamespaceItem->parentItem();
			pParent->removeChild(pOldNamespaceItem);
		}
		pNewItem=createFullItem(szNewNameSpaceName.left(szNewClassName.lastIndexOf("::")));
		pNewItem->setType(KviClassEditorTreeWidgetItem::Namespace);
		pOldNamespaceItem->setName(szNewNameSpaceName.section("::",-1,-1));
		pNewItem->addChild(pOldNamespaceItem);
		//m_pClasses->insert(szNewClassName,pNewItem);
	}
	else
	{
		pClassItem->setName(szNewClassName);
		m_pClasses->insert(szNewClassName,pClassItem);
	}
	KviKvsObjectClass *pClass;
	pClass = KviKvsKernel::instance()->objectController()->lookupClass(szClassName);
	if (pClass) KviKvsKernel::instance()->objectController()->deleteClass(pClass);
	pClassItem->setInheritsClass(szNewInheritsClassName);
	pClassItem->setClassNotBuilt(true);
	KviPointerList<KviClassEditorTreeWidgetItem> pInheritsedClasses;
	searchInheritsedClasses(szClassName,pInheritsedClasses);
	for(unsigned int i=0;i<pInheritsedClasses.count();i++)
	{
		pInheritsedClasses.at(i)->setClassNotBuilt(true);
		pInheritsedClasses.at(i)->setInheritsClass(szNewClassName);
	}
	if (pNewItem) activateItem(pNewItem);
	else activateItem(pClassItem);
/*	QString szName = buildFullItemName(pNamespaceItem);
	QString szNewName;
	szNewName = askForNamespaceName(__tr2qs_ctx("Rename Namespace","editor"),__tr2qs_ctx("Please enter the new name for the namespace","editor"),szName);
	if(szName == szNewName)return;
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
	QString szCode;
	int pntCursor;
	QList<QTreeWidgetItem*> lChildren= m_pLastEditedItem->takeChildren();
	bool bYesToAll = true;
	removeItem(m_pLastEditedItem,&bYesToAll,true);
	m_pLastEditedItem = 0; // make sure it's true (but it already should be because of removeItem())
	m_pLastClickedItem = 0; // make sure it's true (but it already should be because of removeItem())
	KviClassEditorTreeWidgetItem * pItem = createFullItem(szNewName);
	for(int i=0;i<lChildren.count();i++)
	{
		((KviClassEditorTreeWidgetItem*)lChildren.at(i))->setParentItem(pItem);
		pItem->insertChild(pItem->childCount(),lChildren.at(i));
	}
	activateItem(pItem);
	*/
}
/*
void KviClassEditor::renameAliasOrNamespace()
{
	if(!m_pLastEditedItem)return;
	if(!itemExists(m_pLastEditedItem))return; // dead ?


	QString szNewName;

	bool bClass = m_pLastEditedItem->isClass();
	QString szClassName,szInheritsClassName;

	if(bClass)
		askForClassName(szClassName,szInheritsClassName);
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
}

// warning: this saves ONLY the body, not the name!
//          the name is changed explicitly with renameItem(), when needed
*/
void KviClassEditor::saveLastEditedItem()
{
	if(!m_pLastEditedItem)
		return;
	if(!m_pEditor->isModified() || m_pLastEditedItem->isNamespace() || m_pLastEditedItem->isClass())
		return;

	((KviClassEditorTreeWidgetItem *)m_pLastEditedItem)->setCursorPosition(m_pEditor->getCursor());
	QString newCode;
	m_pEditor->getText(newCode);
	((KviClassEditorTreeWidgetItem *)m_pLastEditedItem)->setBuffer(newCode);
	((KviClassEditorTreeWidgetItem *)m_pLastEditedItem)->parentItem()->setClassNotBuilt(true);
}

void KviClassEditor::currentItemChanged(QTreeWidgetItem * it, QTreeWidgetItem *)
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
		m_pMemberFunctionNameRenameButton->setEnabled(false);
		m_pInheritsClassNameLabel->setText("");
		m_pMemberFunctionNameLabel->setText("");
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

	szLabelText = __tr2qs_ctx("Inherits Class","editor");
	szLabelText += ": <b>";
	szLabelText += pClassItem->InheritsClass();
	szLabelText += "</b>";
	m_pInheritsClassNameLabel->setText(szLabelText);

	szLabelText = __tr2qs_ctx("Member Function:","editor");
	if(m_pLastEditedItem->isMethod())
	{
		szLabelText += ": <b>";
		szLabelText += m_pLastEditedItem->text(0);
		szLabelText += "</b>";
		m_pMemberFunctionNameRenameButton->setEnabled(true);
	}
	else m_pClassNameRenameButton->setEnabled(true);
	m_pMemberFunctionNameLabel->setText(szLabelText);
	if(m_pLastEditedItem->isClass())
	{
		m_pMemberFunctionNameRenameButton->setEnabled(false);
		m_pEditor->setText("");
		m_pEditor->setEnabled(false);
		m_pTreeWidget->setFocus();
		return;
	}

	m_pEditor->setText(((KviClassEditorTreeWidgetItem *)it)->buffer());
	m_pEditor->setFocus();
	m_pEditor->setCursorPosition(((KviClassEditorTreeWidgetItem *)it)->cursorPosition());
	m_pEditor->setEnabled(true);
}

void KviClassEditor::customContextMenuRequested(QPoint pnt)
{
	m_pContextPopup->clear();

	m_pLastClickedItem = (KviClassEditorTreeWidgetItem *)m_pTreeWidget->itemAt(pnt);

	int id;

	id = m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NAMESPACE)),
			__tr2qs_ctx("Add Namespace","editor"),
			this,SLOT(newNamespace()));
	if (!m_pLastClickedItem)m_pContextPopup->setItemEnabled(id,true);
	else m_pContextPopup->setItemEnabled(id,m_pLastClickedItem->isNamespace());

	id = m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CLASS)),
			__tr2qs_ctx("Add Class","editor"),
			this,SLOT(newClass()));
	if (!m_pLastClickedItem)m_pContextPopup->setItemEnabled(id,true);
	else m_pContextPopup->setItemEnabled(id,m_pLastClickedItem->isNamespace());


	id = m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_FUNCTION)),
			__tr2qs_ctx("Add Member Function","editor"),
			this,SLOT(newMemberFunction()));
	if (!m_pLastClickedItem)m_pContextPopup->setItemEnabled(id,false);
	else m_pContextPopup->setItemEnabled(id,m_pLastClickedItem->isClass()| m_pLastClickedItem->isMethod());


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
	m_pContextPopup->popup( m_pTreeWidget->mapToGlobal(pnt));
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
*/
void KviClassEditor::getExportClassBuffer(QString &buffer,KviClassEditorTreeWidgetItem * it)
{
	QString szBuf = it->buffer();
	KviCommandFormatter::blockFromBuffer(szBuf);
	QString szNam = buildFullClassName(it);

	buffer = "class(\"";
	buffer += szNam;
	if(!it->InheritsClass().isEmpty())
	{
		buffer+="\",\"";
		buffer+=it->InheritsClass();
	}
	buffer += "\")\n{\n";
	for(int i=0;i<it->childCount();i++)
	{
		KviClassEditorTreeWidgetItem * pFunction= (KviClassEditorTreeWidgetItem *)it->child(i);
		if(pFunction->isMethod())
		{
			buffer += "\t";
			if (pFunction->isInternalFunction()) buffer+="internal ";
			buffer += "function ";
			buffer += pFunction->name();
			buffer += "\n\t{\n";
			buffer += pFunction->buffer();
			buffer += "\n\t}\n";
		}
		i++;
	}
	buffer += "}\n";
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
		QString szFileName=buildFullClassName(it);
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

void KviClassEditor::exportClasses(bool bSelectedOnly, bool bSingleFiles)
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
			QString tmp=buildFullClassName(tempitem);
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

void KviClassEditor::saveProperties(KviConfig * cfg)
{
	cfg->writeEntry("Sizes",m_pSplitter->sizes());
	QString szName;
	if(m_pLastEditedItem)
		szName = buildFullClassName(m_pLastEditedItem);
	cfg->writeEntry("LastClass",szName);
}

void KviClassEditor::loadProperties(KviConfig * cfg)
{
	QList<int> def;
	def.append(20);
	def.append(80);
	m_pSplitter->setSizes(cfg->readIntListEntry("Sizes",def));
	QString tmp = cfg->readEntry("LastClass",QString());

	KviClassEditorTreeWidgetItem * it = findItem(tmp);
	activateItem(it);
}

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
/*
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

        if(!*pbYesToAll)saveLastEditedItem();
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
*/

bool KviClassEditor::askForClassName(QString &szClassName,QString &szInheritsClassName, bool bEdit)
{
	KviClassEditorDialog *pDialog=new KviClassEditorDialog(this,"classdialog",m_pClasses,szClassName,szInheritsClassName,bEdit);
	szClassName="";
	g_pClassEditorModule->lock();
	bool bOk=pDialog->exec();
	g_pClassEditorModule->unlock();
	if(bOk)
	{
		szClassName=pDialog->getClassName();
		szInheritsClassName=pDialog->getInheritsClassName();
		delete pDialog;
		return true;
	}
        delete pDialog;
	return false;
}

bool KviClassEditor::askForFunction(QString &szFunctionName,bool * bInternal,const QString &szClassName)
{
	KviClassEditorFunctionDialog *pDialog=new KviClassEditorFunctionDialog(this,"function",szClassName,szFunctionName);
	szFunctionName="";
	g_pClassEditorModule->lock();
	bool bOk=pDialog->exec();
	g_pClassEditorModule->unlock();
	if(bOk)
	{
		szFunctionName=pDialog->getFunctionName();
		*bInternal=pDialog->isInternalFunction();
		delete pDialog;
		return true;
	}
	delete pDialog;
	return false;
}

bool KviClassEditor::askForNamespaceName(const QString &szAction,const QString &szText,const QString &szInitialText, QString &szNewName)
{
        bool bOk = false;
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
		if(!bOk)return false;
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
	return true;
}

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
       QString szClassName,szInheritsClassName;
       askForClassName(szClassName,szInheritsClassName,false);
       if(szClassName.isEmpty())return;
       KviClassEditorTreeWidgetItem *it=newItem(szClassName,KviClassEditorTreeWidgetItem::Class);
       QString szClass="class\(";
       szClass+=szClassName+","+szInheritsClassName;
       szClass+="){}\n";
       it->setInheritsClass(szInheritsClassName);
       activateItem(it);
       m_pClasses->insert(szClassName,it);
       debug("run class script");
       KviKvsScript::run(szClass,g_pActiveWindow);
}
void KviClassEditor::newNamespace()
{
       QString szName;
       if(!askForNamespaceName(__tr2qs_ctx("Add Namespace","editor"),__tr2qs_ctx("Please enter the name for the new namespace","editor"),"mynamespace",szName)) return;
       if(szName.isEmpty())return;
       KviClassEditorTreeWidgetItem *it=newItem(szName,KviClassEditorTreeWidgetItem::Namespace);
       activateItem(it);
}

void KviClassEditor::newMemberFunction()
{
       QString szFunctionName,szClassName;
       if(m_pLastClickedItem->isMethod()) m_pLastClickedItem=m_pLastClickedItem->parentItem();
       szClassName=buildFullClassName(m_pLastClickedItem);
       bool bInternal;
       askForFunction(szFunctionName, &bInternal,szClassName);
       if(szFunctionName.isEmpty())return;
       KviClassEditorTreeWidgetItem *it=newItem(szFunctionName,KviClassEditorTreeWidgetItem::Method);
       it->setInternalFunction(bInternal);
       activateItem(it);
       it->parentItem()->setClassNotBuilt(true);
}

KviClassEditorTreeWidgetItem *  KviClassEditor::newItem(QString &szName,KviClassEditorTreeWidgetItem::Type eType)
{
        if(m_pLastClickedItem)
            buildFullItemPath(m_pLastClickedItem,szName);
        QString szTmp;
        if(findItem(szName)) szName.append("1");
        int idx=2;
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

        return it;
}

void KviClassEditor::build()
{
    saveLastEditedItem();
    KviPointerHashTableIterator<QString,KviClassEditorTreeWidgetItem> it (*m_pClasses);
    KviPointerList<KviClassEditorTreeWidgetItem> pLinkedClasses;
    pLinkedClasses.setAutoDelete(false);
    KviPointerList<KviClassEditorTreeWidgetItem> pSkipClasses;
    pSkipClasses.setAutoDelete(false);
    while(it.current())
    {
        KviClassEditorTreeWidgetItem *pClass=it.current();
        if (pSkipClasses.findRef(it.current())!=-1)
	{
		++it;
		continue;
        }
        if (pClass->classNotBuilt())
        {
		KviClassEditorTreeWidgetItem *pParentClass=m_pClasses->find(pClass->InheritsClass());
		debug("append to build last class %s",pClass->name().toUtf8().data());


		pLinkedClasses.append(pClass);
		while(pParentClass)
		{
			if (pParentClass->classNotBuilt())
			{
				debug("append to build parent class %s",pParentClass->name().toUtf8().data());
				pLinkedClasses.append(pParentClass);
			}
			pParentClass=m_pClasses->find(pParentClass->InheritsClass());
		}
		for(int i=pLinkedClasses.count()-1;i>=0;i--)
		{
			KviKvsObjectClass *pClass;
			debug("parsing class %s",pLinkedClasses.at(i)->name().toUtf8().data());
			pClass = KviKvsKernel::instance()->objectController()->lookupClass(pLinkedClasses.at(i)->name());
			if (pClass) KviKvsKernel::instance()->objectController()->deleteClass(pClass);
			QString szClass="class\(";
			szClass+=pLinkedClasses.at(i)->name()+","+pLinkedClasses.at(i)->InheritsClass()+")\n{\n";
			for(int j=0;j<pLinkedClasses.at(i)->childCount();j++)
			{
				QString szFunction;
				KviClassEditorTreeWidgetItem *pFunction=( KviClassEditorTreeWidgetItem *)pLinkedClasses.at(i)->child(j);
				if (pFunction->isInternalFunction()) szFunction+="internal ";
				szFunction+=pFunction->name()+"()\n{\n";
				szFunction+=pFunction->buffer()+"\n}\n";
				szClass+=szFunction;
			}
			szClass+="}\n";
			debug ("class %s",szClass.toUtf8().data());
			KviKvsScript::run(szClass,g_pActiveWindow);
			pClass = KviKvsKernel::instance()->objectController()->lookupClass(pLinkedClasses.at(i)->name());
			if (!pClass)
			{
				QString szError=__tr2qs_ctx("Unable to compile class: ","editor");
				szError+=pLinkedClasses.at(i)->name()+"\n";
				KviPointerList<KviClassEditorTreeWidgetItem> pInheritsedClasses;
				searchInheritsedClasses(pLinkedClasses.at(i)->name(),pInheritsedClasses);
				if (pInheritsedClasses.count())
				{
					szError+=__tr2qs_ctx("These Inheritsed classes will be not compiled too:","editor");
					szError+="\n";
					for(unsigned int j=0;j<pInheritsedClasses.count();j++)
					{
						szError+=pInheritsedClasses.at(j)->name()+"\n";
						pInheritsedClasses.at(j)->setClassNotBuilt(true);
						pSkipClasses.append(pInheritsedClasses.at(j));
					}
				}
				QMessageBox::critical(this,__tr2qs_ctx("Compilation error - KVIrc","editor"),szError,
				QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
				break;
			}
			pLinkedClasses.at(i)->setClassNotBuilt(false);
		}

        }
         ++it;
    }
    KviKvsKernel::instance()->objectController()->flushUserClasses();

}
void KviClassEditor::loadNotBuiltClasses()
{
        QString szFileName = "libkviclasseditortmp.kvc";
        QString szBuffer;
        g_pApp->getLocalKvircDirectory(szBuffer,KviApp::ConfigPlugins,szFileName);
        KviConfig cfg(szBuffer,KviConfig::Read);
        KviConfigIterator it(*(cfg.dict()));

        KviPointerList<QString> l;
        l.setAutoDelete(true);

        while(it.current())
        {
                l.append(new QString(it.currentKey()));
                ++it;
        }

        for(QString * s = l.first();s;s = l.next())
        {
                cfg.setGroup(*s);
                KviClassEditorTreeWidgetItem *pClassItem = createFullItem(*s);
                 m_pClasses->insert(*s,pClassItem);
                pClassItem->setClassNotBuilt(true);
                KviConfigGroup * pDict = cfg.dict()->find(*s);
                if(pDict)
                {
                        KviConfigGroupIterator it(*pDict);
                        KviPointerList<QString> names;
                        names.setAutoDelete(true);

                        while(it.current())
                        {
                                names.append(new QString(it.currentKey()));
                                ++it;
                        }
                        for(QString * s = names.first(); s; s = names.next())
                        {
				if (KviQString::equalCI(*s,"@Inherits"))
				{
					pClassItem->setInheritsClass(cfg.readQStringEntry(*s,""));
					continue;
				}
				QString szCode = cfg.readQStringEntry(*s,"");
                                KviClassEditorTreeWidgetItem *pFunctionItem=new KviClassEditorTreeWidgetItem(pClassItem,KviClassEditorTreeWidgetItem::Method,*s);
                                pFunctionItem->setBuffer(szCode);
                        }
                }
        }
}

void KviClassEditor::saveNotBuiltClasses()
{
        saveLastEditedItem();
        KviPointerHashTableIterator<QString,KviClassEditorTreeWidgetItem> it (*m_pClasses);
        QString szFileName = "libkviclasseditortmp.kvc";
        QString szBuffer;
        g_pApp->getLocalKvircDirectory(szBuffer,KviApp::ConfigPlugins,szFileName);
        KviConfig cfg(szBuffer,KviConfig::Write);
	while(it.current())
        {
		if (it.current()->classNotBuilt())
		{
			debug("saving class %s",it.currentKey().toUtf8().data());
			cfg.setGroup(it.currentKey());
			cfg.writeEntry("@Inherits",it.current()->InheritsClass());
			for(int i=0;i<it.current()->childCount();i++)
				cfg.writeEntry(((KviClassEditorTreeWidgetItem*)it.current()->child(i))->name(),((KviClassEditorTreeWidgetItem*)it.current()->child(i))->buffer());
		}
		++it;
        }
}

void KviClassEditor::searchInheritsedClasses(const QString szClass,KviPointerList<KviClassEditorTreeWidgetItem> & pInheritsedClasses)
{
        KviPointerHashTableIterator<QString,KviClassEditorTreeWidgetItem> it (*m_pClasses);
	debug("into searchInherits");
        while(it.current())
        {
		//debug("class %s",it.current()->name().toUtf8().data());
		//debug("inerits class %s",it.current()->InheritsClass().toUtf8().data());

	    if (KviQString::equalCI(szClass,it.current()->InheritsClass())) pInheritsedClasses.append(it.current());
            ++it;
        }
}

KviClassEditorWindow::KviClassEditorWindow(KviFrame * lpFrm)
: KviWindow(KVI_WINDOW_TYPE_SCRIPTEDITOR,lpFrm,"classeditor",0)
{
        g_pClassEditorWindow = this;

        setFixedCaption(__tr2qs_ctx("Class Editor","editor"));

        QGridLayout * g = new QGridLayout();

        m_pEditor = new KviClassEditor(this);
        g->addWidget(m_pEditor,0,0,1,4);

        QPushButton * btn = new QPushButton(__tr2qs_ctx("&Build","editor"),this);
        connect(btn,SIGNAL(clicked()),this,SLOT(buildClicked()));
        btn->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)));
        g->addWidget(btn,1,1);

        btn = new QPushButton(__tr2qs_ctx("&Save","editor"),this);
        connect(btn,SIGNAL(clicked()),this,SLOT(saveClicked()));
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

void KviClassEditorWindow::buildClicked()
{
        m_pEditor->build();
}

void KviClassEditorWindow::saveClicked()
{
        m_pEditor->saveNotBuiltClasses();
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

KviClassEditorDialog::KviClassEditorDialog(QWidget * pParent, const QString & szName,KviPointerHashTable<QString,KviClassEditorTreeWidgetItem> *pClasses,const QString &szClassName, const QString &szInheritsClassName,bool bRenameMode)
: QDialog(pParent)
{
        setObjectName(szName);

        m_pParent = pParent;
        QPalette p = palette();
        p.setColor(foregroundRole(),QColor( 0, 0, 0 ));
        p.setColor(backgroundRole(),QColor( 236, 233, 216 ));
        setPalette(p);

        QGridLayout * pLayout = new QGridLayout(this);

        KviTalHBox * hbox = new KviTalHBox(this);
        hbox->setSpacing(0);
        hbox->setMargin(0);
        pLayout->addWidget(hbox,0,0);

        QLabel * pClassNameLabel = new QLabel(hbox);
        pClassNameLabel->setObjectName("classnamelabel");

        pClassNameLabel->setText(__tr2qs_ctx("Please enter the name for the new class:","editor"));

        m_pClassNameLineEdit = new QLineEdit(hbox);
        m_pClassNameLineEdit->setObjectName("classnameineedit");
	m_pClassNameLineEdit->setText(szClassName);
	if (!bRenameMode)
	{
		debug(" normal mode");
		// we allow only [\w:]+ class name
		QRegExp re("[\\w]+");
		QRegExpValidator *pValidator=new QRegExpValidator(re,this);
		m_pClassNameLineEdit->setValidator(pValidator);
		m_pClassNameLineEdit->setObjectName("functionameineedit");
		m_pClassNameLineEdit->setToolTip(__tr2qs_ctx("Class names can contain only letters, digits and underscores","editor"));
	}
	else
	{
		debug(" rename mode");
		// in editor mode we allow [\w:]+ and namespace separator
		QRegExp re("[\\w]+(::[\\w]+)+");
		QRegExpValidator *pValidator=new QRegExpValidator(re,this);
		m_pClassNameLineEdit->setValidator(pValidator);
		m_pClassNameLineEdit->setObjectName("functionameineedit");
		m_pClassNameLineEdit->setToolTip(__tr2qs_ctx("In rename mode class names can contain only letters, digits and underscores and namespaces :: separator","editor"));
	}
        hbox = new KviTalHBox(this);
        hbox->setSpacing(0);
        hbox->setMargin(0);
        pLayout->addWidget(hbox,1,0);

	QLabel * pInheritsClassLabel = new QLabel(hbox);
	pInheritsClassLabel->setObjectName("Inheritsclasslabel");
	pInheritsClassLabel->setText(__tr2qs_ctx("Inherits Class:","editor"));

	m_pInheritsClassComboBox=new QComboBox(hbox);
        KviPointerHashTableIterator<QString,KviClassEditorTreeWidgetItem> pClassesIt(*pClasses);

      /*  m_pInheritsClassComboBox->addItem("Object");
	m_pInheritsClassComboBox->addItem("Widget");
*/
        QStringList szClasses;
        while(pClassesIt.current())
        {
            szClasses.append(pClassesIt.currentKey());
            ++pClassesIt;
        }
        KviPointerHashTableIterator<QString,KviKvsObjectClass> pBuiltinClasses(*KviKvsKernel::instance()->objectController()->classDict());
        while(pBuiltinClasses.current())
        {
             if (pBuiltinClasses.current()->isBuiltin())
                  szClasses.append(pBuiltinClasses.currentKey());
             ++pBuiltinClasses;
        }
        szClasses.sort();
        for(int i=0;i<szClasses.count();i++)
	    m_pInheritsClassComboBox->addItem(szClasses.at(i));
	if (!szInheritsClassName.isEmpty())
	{
		int iCurrentIdx=m_pInheritsClassComboBox->findText(szInheritsClassName);
		m_pInheritsClassComboBox->setCurrentIndex(iCurrentIdx);
	}
	m_pClassNameLineEdit->setFocus();

        hbox = new KviTalHBox(this);
        hbox->setSpacing(0);
        hbox->setMargin(0);
        pLayout->addWidget(hbox,2,0);

        m_pNewClassButton = new QPushButton(hbox);
        m_pNewClassButton->setObjectName("newclassbutton");
	if (!bRenameMode)
	{
		m_pNewClassButton->setText(__tr2qs_ctx("&Create Class","editor"));
		m_pNewClassButton->setEnabled(false);
	}
	else m_pNewClassButton->setText(__tr2qs_ctx("&Rename Class","editor"));

        QPushButton * pCancelButton = new QPushButton(hbox);
        pCancelButton->setObjectName("cancelButton");
        pCancelButton->setText(__tr2qs_ctx("&Cancel","editor"));

        setLayout(pLayout);

        // signals and slots connections
        connect(pCancelButton, SIGNAL(clicked()), this, SLOT(reject()));
        connect(m_pNewClassButton, SIGNAL(clicked()), this, SLOT(accept()));
        connect(m_pClassNameLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
}

KviClassEditorDialog::~KviClassEditorDialog()
{
}

void KviClassEditorDialog::textChanged(const QString & szText)
{
    m_pNewClassButton->setEnabled(!szText.isEmpty());
}

KviClassEditorFunctionDialog::KviClassEditorFunctionDialog(QWidget * pParent, const QString & szName, const QString &szClassName, const QString &szFunctionName)
: QDialog(pParent)
{
        setObjectName(szName);

        m_pParent = pParent;
        QPalette p = palette();
        p.setColor(foregroundRole(),QColor( 0, 0, 0 ));
        p.setColor(backgroundRole(),QColor( 236, 233, 216 ));
        setPalette(p);

        QGridLayout * pLayout = new QGridLayout(this);

        KviTalHBox * hbox = new KviTalHBox(this);
        hbox->setSpacing(0);
        hbox->setMargin(0);
        pLayout->addWidget(hbox,0,0);

        QLabel * pClassNameLabel = new QLabel(hbox);
        pClassNameLabel->setObjectName("classnamelabel");
        pClassNameLabel->setText("Class: <b>"+szClassName+"</b>");

        hbox = new KviTalHBox(this);
        hbox->setSpacing(0);
        hbox->setMargin(0);
        pLayout->addWidget(hbox,1,0);

        QLabel * pFunctionNameLabel = new QLabel(hbox);
        pFunctionNameLabel->setObjectName("functionnamelabel");
        pFunctionNameLabel->setText(__tr2qs_ctx("Please enter the name for the new member function:","editor"));

        m_pFunctionNameLineEdit = new QLineEdit(hbox);

	// we allow only [\w:]+ function name
	QRegExp re("[\\w]+");
	QRegExpValidator *pValidator=new QRegExpValidator(re,this);
	m_pFunctionNameLineEdit->setValidator(pValidator);
        m_pFunctionNameLineEdit->setObjectName("functionameineedit");
	m_pFunctionNameLineEdit->setToolTip(__tr2qs_ctx("Function names can contain only letters, digits and underscores","editor"));
	m_pFunctionNameLineEdit->setText(szFunctionName);

        hbox = new KviTalHBox(this);
        hbox->setSpacing(0);
        hbox->setMargin(0);
        pLayout->addWidget(hbox,2,0);


        QLabel * pFunctionInternalLabel = new QLabel(hbox);
        pFunctionInternalLabel->setObjectName("functionnamelabel");
        pFunctionInternalLabel->setText(__tr2qs_ctx("Set as <b>Internal</b> Function: ","editor"));

        m_pInternalCheckBox=new QCheckBox(hbox);
        m_pInternalCheckBox->setChecked(false);
        m_pFunctionNameLineEdit->setFocus();
        hbox->setAlignment(m_pInternalCheckBox,Qt::AlignLeft);
        hbox->setStretchFactor(m_pInternalCheckBox,70);
        hbox->setStretchFactor(pFunctionInternalLabel,30);


        hbox = new KviTalHBox(this);
        hbox->setSpacing(0);
        hbox->setMargin(0);
        pLayout->addWidget(hbox,3,0);

        m_pNewFunctionButton = new QPushButton(hbox);
        m_pNewFunctionButton->setObjectName("newfunctionbutton");
        m_pNewFunctionButton->setText(__tr2qs_ctx("&Add","editor"));
	if (szFunctionName.isEmpty()) m_pNewFunctionButton->setEnabled(false);

        QPushButton * pCancelButton = new QPushButton(hbox);
        pCancelButton->setObjectName("cancelButton");
        pCancelButton->setText(__tr2qs_ctx("&Cancel","editor"));

        setLayout(pLayout);

        // signals and slots connections
        connect(pCancelButton, SIGNAL(clicked()), this, SLOT(reject()));
        connect(m_pNewFunctionButton, SIGNAL(clicked()), this, SLOT(accept()));
        connect(m_pFunctionNameLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
}

KviClassEditorFunctionDialog::~KviClassEditorFunctionDialog()
{
}

void KviClassEditorFunctionDialog::textChanged(const QString & szText)
{
    m_pNewFunctionButton->setEnabled(!szText.isEmpty());
}
