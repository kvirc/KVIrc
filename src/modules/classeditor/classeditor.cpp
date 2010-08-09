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
: QTreeWidgetItem(pTreeWidget), KviHeapObject(), m_eType(eType)
{
	setName(szName);
	m_szInheritsClassName="";
	m_cPos=0;
	m_bInternal=false;
	if(eType==KviClassEditorTreeWidgetItem::Namespace) setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NAMESPACE))));
	else setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CLASS))));
	m_bClassModified=false;
}

KviClassEditorTreeWidgetItem::KviClassEditorTreeWidgetItem(KviClassEditorTreeWidgetItem * pParentItem,Type eType,const QString &szName)
: QTreeWidgetItem(pParentItem), m_eType(eType)
{
	setName(szName);
	m_szInheritsClassName="";
	m_bInternal=false;
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
		setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CLASS))));
	else setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_FUNCTION))));
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
	m_pSplitter->setChildrenCollapsible(false);
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
	connect(m_pMemberFunctionNameRenameButton,SIGNAL(clicked()),this,SLOT(renameFunction()));
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
	m_pClasses->clear();
	delete m_pClasses;
}

void KviClassEditor::buildFullItemPath(KviClassEditorTreeWidgetItem * it, QString & szBuffer)
{
	if (!it) return;
	szBuffer.prepend(it->name()+"::");
	it=(KviClassEditorTreeWidgetItem*)it->parent();
	while(it)
	{
		QString tmp = it->name();
		if(!tmp.isEmpty())
		{
			szBuffer.prepend("::");
			szBuffer.prepend(tmp);
		}
		it = (KviClassEditorTreeWidgetItem*)it->parent();
	}
}

QString KviClassEditor::buildFullClassName(KviClassEditorTreeWidgetItem * it)
{
	if(!it) return QString();
	QString szName = it->name();
	KviClassEditorTreeWidgetItem * nit =(KviClassEditorTreeWidgetItem *)it->parent();
	while(nit)
	{
		QString tmp = nit->name();
		if(!tmp.isEmpty())
		{
			szName.prepend("::");
			szName.prepend(tmp);
		}
		nit = (KviClassEditorTreeWidgetItem *)nit->parent();
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

KviClassEditorTreeWidgetItem * KviClassEditor::createFullItem(const QString & szFullName)
{
	QStringList lNamespaces=szFullName.split("::");
	if(!lNamespaces.count()) return 0;
	if (lNamespaces.count()==1)
	{
		KviClassEditorTreeWidgetItem *pItem=findTopLevelItem(lNamespaces.at(0));
		if(pItem) return pItem;
		return new KviClassEditorTreeWidgetItem(m_pTreeWidget,KviClassEditorTreeWidgetItem::Class,lNamespaces.at(0));
	}
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

KviClassEditorTreeWidgetItem * KviClassEditor::createFullNamespace(const QString & szFullName)
{
	QStringList lNamespaces=szFullName.split("::");
	if(!lNamespaces.count()) return 0;
	KviClassEditorTreeWidgetItem *pItem=findTopLevelItem(lNamespaces.at(0));
	if (!pItem) pItem=new KviClassEditorTreeWidgetItem(m_pTreeWidget,KviClassEditorTreeWidgetItem::Namespace,lNamespaces.at(0));
	if (lNamespaces.count()==1) return pItem;
	bool bFound;
	int i;
	for(i=1;i<lNamespaces.count();i++)
	{
		bFound=false;
		for(int j=0;j<pItem->childCount();j++)
		{
			if (KviQString::equalCI(pItem->child(j)->text(0),lNamespaces.at(i)) && (( KviClassEditorTreeWidgetItem *)pItem->child(j))->isNamespace())
			{
				pItem=( KviClassEditorTreeWidgetItem *)pItem->child(j);
				bFound=true;
				break;
			}
		}
		if (!bFound)
			pItem=new KviClassEditorTreeWidgetItem(pItem,KviClassEditorTreeWidgetItem::Namespace,lNamespaces.at(i));
	}
	return pItem;
}

void KviClassEditor::oneTimeSetup()
{
	QString szPath;
	g_pApp->getLocalKvircDirectory(szPath,KviApp::Classes);
	QDir d(szPath);
	QStringList sl;
	QString extension = QString("*%1").arg(KVI_FILEEXTENSION_SCRIPT); // *.kvs
	sl = d.entryList(QStringList(extension), QDir::Files | QDir::NoDotAndDotDot);
	KviKvsObjectClass *pClass;
	KviClassEditorTreeWidgetItem *pClassItem;
	g_pModuleManager->getModule("objects");
	KviPointerHashTableIterator<QString,KviKvsObjectClass> it(*KviKvsKernel::instance()->objectController()->classDict());
	QString szClassName;
	while(KviKvsObjectClass * pClass=it.current())
	{
		if (pClass->isBuiltin())m_pClasses->insert(it.currentKey(),0);
		else
		{
			QString szTmp;
			szTmp=it.currentKey();
			szTmp.replace("::","--");
			szTmp.append(".kvs");
			if (sl.indexOf(szTmp)==-1)
			{
				szClassName=it.currentKey();
				pClassItem = createFullItem(szClassName);
				createFullClass(it.current(),pClassItem,szClassName);
			}
		}
		++it;
	}

	for(int i=0;i<sl.count();i++)
	{
		szClassName=sl.at(i);
		szClassName.replace("--","::");
		szClassName.chop(4);
		pClassItem = createFullItem(szClassName);
		pClass = KviKvsKernel::instance()->objectController()->lookupClass(szClassName);
		if (pClass) createFullClass(pClass, pClassItem, szClassName);
	}
	loadNotBuiltClasses();
	connect(m_pTreeWidget,SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)),this,SLOT(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)));
	m_pTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_pTreeWidget,SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(customContextMenuRequested(const QPoint &)));
}
void KviClassEditor::createFullClass(KviKvsObjectClass *pClass, KviClassEditorTreeWidgetItem *pClassItem, const QString &szClassName)
{
	KviPointerHashTableIterator<QString,KviKvsObjectFunctionHandler>  it(* pClass->getHandlers());
	KviClassEditorTreeWidgetItem *pFunctionItem;
	m_pClasses->insert(szClassName,pClassItem);
	KviKvsObjectClass *pParentClass=pClass->parentClass();
	pClassItem->setInheritsClass(pParentClass->name());
	while(it.current())
	{
		QString szCode;
		KviKvsObjectFunctionHandler *handler=pClass->lookupFunctionHandler(it.currentKey());

		if (pClass->isScriptHandler(it.currentKey()) && !handler->isClone())
		{
			pFunctionItem=findFunction(it.currentKey(), pClassItem);
			if(!pFunctionItem) pFunctionItem = new KviClassEditorTreeWidgetItem(pClassItem,KviClassEditorTreeWidgetItem::Method,it.currentKey());
			pClass->getFunctionCode(szCode,*handler);
			pFunctionItem->setBuffer(szCode);
			if(handler->flags() & KviKvsObjectFunctionHandler::Internal) pFunctionItem->setInternalFunction(true);
		}
		++it;
	}

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


*/

bool KviClassEditor::hasSelectedItems()
{
	return m_pTreeWidget->selectedItems().count() ? 1 : 0;
}

bool KviClassEditor::classExists(QString & szFullItemName)
{
	if (m_pClasses->find(szFullItemName)) return true;
	else return false;
}

void KviClassEditor::renameFunction()
{
	if(!m_pLastEditedItem)return;
	KviClassEditorTreeWidgetItem *pFunction=m_pLastEditedItem;
	QString szClassName = ((KviClassEditorTreeWidgetItem*)pFunction->parent())->name();
	QString szFunctionName = pFunction->name();
	KviClassEditorTreeWidgetItem *pParentClass = (KviClassEditorTreeWidgetItem*) pFunction->parent();

	QString szNewFunctionName = szFunctionName;
	bool bInternal=pFunction->isInternalFunction();
	if(!askForFunction(szNewFunctionName, &bInternal,szClassName, true)) return;
	if(KviQString::equalCI(szFunctionName,szNewFunctionName) && bInternal==pFunction->isInternalFunction()) return;
	if (findFunction(szNewFunctionName,pParentClass) && !KviQString::equalCI(szFunctionName,szNewFunctionName))
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
	currentItemChanged(pFunction,pFunction);
	pFunction->setInternalFunction(bInternal);
	pParentClass->setClassNotBuilt(true);
	KviPointerList<KviClassEditorTreeWidgetItem> lInheritedClasses;
	lInheritedClasses.setAutoDelete(false);
	searchInheritedClasses(szClassName,lInheritedClasses);
	for(unsigned int i=0;i<lInheritedClasses.count();i++)
		lInheritedClasses.at(i)->setClassNotBuilt(true);
	activateItem(pFunction);
}

KviClassEditorTreeWidgetItem * KviClassEditor::findFunction(const QString &szFunctionName, KviClassEditorTreeWidgetItem *pClass)
{
	for(int i=0;i<pClass->childCount();i++)
	{
		if(KviQString::equalCI(szFunctionName,((KviClassEditorTreeWidgetItem *)pClass->child(i))->name()))
			return (KviClassEditorTreeWidgetItem*) pClass->child(i);
	}
	return 0;
}

void KviClassEditor::renameItem()
{
	if(!m_pLastEditedItem)return;
	if(m_pLastEditedItem->isClass()) renameClass(m_pLastEditedItem);
	else if(m_pLastEditedItem->isNamespace()) renameNamespace(m_pLastEditedItem);
	else {
		// is function
		KviClassEditorTreeWidgetItem *pParent = (KviClassEditorTreeWidgetItem*)m_pLastEditedItem->parent();
		if(pParent->isClass()) renameClass(pParent);
	}
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
	m_pClasses->removeRef(pClassItem);
	cutItem(pClassItem);
	if(szNewClassName.contains("::"))
	{
		pNewItem=createFullNamespace(szNewClassName.left(szNewClassName.lastIndexOf("::")));
		pClassItem->setName(szNewClassName.section("::",-1,-1));
		pNewItem->addChild(pClassItem);
	} else {
		pClassItem->setName(szNewClassName);
		m_pTreeWidget->addTopLevelItem(pClassItem);
	}
	m_pClasses->insert(szNewClassName,pClassItem);
	pClassItem->setInheritsClass(szNewInheritsClassName);
	pClassItem->setClassNotBuilt(true);
	KviPointerList<KviClassEditorTreeWidgetItem> lInheritedClasses;
	lInheritedClasses.setAutoDelete(false);
	searchInheritedClasses(szClassName,lInheritedClasses);
	for(unsigned int i=0;i<lInheritedClasses.count();i++)
	{
		lInheritedClasses.at(i)->setClassNotBuilt(true);
		lInheritedClasses.at(i)->setExpanded(true);
		lInheritedClasses.at(i)->setInheritsClass(szNewClassName);
	}
	if (pNewItem)
	{
		activateItem(pNewItem);
		pNewItem->setExpanded(true);
	} else {
		activateItem(pClassItem);
		pClassItem->setExpanded(true);
	}
	qDebug("causa rename deleto classe %s",szClassName.toUtf8().data());
	KviKvsObjectClass *pClass = KviKvsKernel::instance()->objectController()->lookupClass(szClassName);
	if (pClass) KviKvsKernel::instance()->objectController()->deleteClass(pClass);
}

void KviClassEditor::cutItem(KviClassEditorTreeWidgetItem *pItem)
{
	int iIdx=m_pTreeWidget->indexOfTopLevelItem(pItem);
	if (iIdx!=-1)
	{
		m_pTreeWidget->takeTopLevelItem(iIdx);
	} else {
		KviClassEditorTreeWidgetItem *pParent = (KviClassEditorTreeWidgetItem*)pItem->parent();
		pParent->removeChild(pItem);
	}
}

void KviClassEditor::renameNamespace(KviClassEditorTreeWidgetItem *pOldNamespaceItem)
{
	QString szOldNameSpaceName = buildFullClassName(m_pLastEditedItem);
	QString szNewNameSpaceName;
	if (!askForNamespaceName(__tr2qs_ctx("Rename Namespace","editor"),__tr2qs_ctx("Please enter the new name for the namespace","editor"),szOldNameSpaceName,szNewNameSpaceName)) return;
	if (KviQString::equalCI(szOldNameSpaceName,szNewNameSpaceName)) return;
	KviClassEditorTreeWidgetItem *pNewItem=findItem(szNewNameSpaceName);
	if (pNewItem)
	{
		g_pClassEditorModule->lock();
		if (pNewItem->isClass())
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
		return;
	}
	KviPointerList<KviClassEditorTreeWidgetItem> pList;
	pList.setAutoDelete(false);
	appendAllClassItemsRecursive(&pList,pOldNamespaceItem);
	cutItem(pOldNamespaceItem);
	if(szNewNameSpaceName.contains("::"))
	{
		pNewItem=createFullNamespace(szNewNameSpaceName.left(szNewNameSpaceName.lastIndexOf("::")));
		pOldNamespaceItem->setName(szNewNameSpaceName.section("::",-1,-1));
		pNewItem->addChild(pOldNamespaceItem);
	} else {
		m_pTreeWidget->addTopLevelItem(pOldNamespaceItem);
		pOldNamespaceItem->setName(szNewNameSpaceName);
	}
	for(unsigned int i=0;i<pList.count();i++)
	{
		KviPointerHashTableEntry<QString,KviClassEditorTreeWidgetItem> *entry=m_pClasses->findRef(pList.at(i));
		if (entry)
		{
			KviPointerList<KviClassEditorTreeWidgetItem> lInheritedClasses;
			lInheritedClasses.setAutoDelete(false);

			QString szOldName = entry->key();
			QString szNewName = buildFullClassName(pList.at(i));
			searchInheritedClasses(szOldName,lInheritedClasses);
			for(unsigned int j=0;j<lInheritedClasses.count();j++)
			{
				lInheritedClasses.at(j)->setClassNotBuilt(true);
				lInheritedClasses.at(j)->setExpanded(true);
				lInheritedClasses.at(j)->setInheritsClass(szNewName);
			}
			m_pClasses->removeRef(pList.at(i));
			m_pClasses->insert(szNewName,pList.at(i));
			pList.at(i)->setClassNotBuilt(true);

			KviKvsObjectClass *pClass = KviKvsKernel::instance()->objectController()->lookupClass(szOldName);
			if (pClass) KviKvsKernel::instance()->objectController()->deleteClass(pClass);
		}
	}

	if (pNewItem)
	{
		activateItem(pNewItem);
		pNewItem->setExpanded(true);
	} else {
		activateItem(pOldNamespaceItem);
		pOldNamespaceItem->setExpanded(true);
	}
}
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
	((KviClassEditorTreeWidgetItem *)m_pLastEditedItem->parent())->setClassNotBuilt(true);
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
	if(m_pLastEditedItem->isMethod()) pClassItem=(KviClassEditorTreeWidgetItem *)m_pLastEditedItem->parent();
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
			__tr2qs_ctx("Collapse All Items","editor"),
			this,SLOT(slotCollapseItems()));

	m_pContextPopup->setItemEnabled(id,bHasItems);
	m_pContextPopup->popup( m_pTreeWidget->mapToGlobal(pnt));
}

void KviClassEditor::searchReplace(const QString &szSearch,bool bReplace,const QString &szReplace)
{
	KviPointerHashTableIterator<QString,KviClassEditorTreeWidgetItem> it (*m_pClasses);
	while(it.current())
	{
		KviClassEditorTreeWidgetItem *pItem=it.current();
		for(int j=0;j<pItem->childCount();j++)
		{
			bool bOpened=false;
			if(((KviClassEditorTreeWidgetItem *)pItem->child(j))->buffer().indexOf(szSearch,0,Qt::CaseInsensitive) != -1)
			{
				pItem->child(j)->setBackground(0, QColor(255,0,0,128));
				if (bReplace){
					QString &buffer=(QString &)((KviClassEditorTreeWidgetItem *)pItem->child(j))->buffer();
					buffer.replace(szSearch,szReplace,Qt::CaseInsensitive);
				}
				if (!bOpened)
				{
					openParentItems(pItem->child(j));
					bOpened=true;
				}
			} else {
				pItem->child(j)->setBackground(0, QColor(255,255,255));
			}
		}
		++it;
	}
}
void KviClassEditor::slotFind()
{

	g_pClassEditorModule->lock();
	bool bOk;

	QString szSearch = QInputDialog::getText(this,
		__tr2qs_ctx("Find In Classes","editor"),
		__tr2qs_ctx("Please enter the text to be searched for. The matching function will be highlighted.","editor"),
		QLineEdit::Normal,
		"",
		&bOk);

	g_pClassEditorModule->unlock();
	if(!bOk)return;
	if(szSearch.isEmpty())return;
	m_pEditor->setFindText(szSearch);
	searchReplace(szSearch);
}
void KviClassEditor::slotFindWord(const QString &szSearch)
{
	m_pEditor->setFindText(szSearch);
}

void KviClassEditor::recursiveCollapseItems(KviClassEditorTreeWidgetItem * it)
{
	if(!it)return;
	for (int i=0;i<it->childCount();i++)
	{
		if(it->child(i)->childCount())
		{
			it->child(i)->setExpanded(false);
			recursiveCollapseItems((KviClassEditorTreeWidgetItem *)it->child(i));
		}
	}
}

void KviClassEditor::slotCollapseItems()
{
	for (int i=0;i<m_pTreeWidget->topLevelItemCount();i++)
	{
		if (m_pTreeWidget->topLevelItem(i)->childCount()){
			m_pTreeWidget->topLevelItem(i)->setExpanded(false);
			recursiveCollapseItems((KviClassEditorTreeWidgetItem *)m_pTreeWidget->topLevelItem(i));
		}
	}
}
/*
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
		else appendSelectedClassItemsRecursive(l,list.at(i));
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
	KviPointerHashTableIterator<QString,KviClassEditorTreeWidgetItem> it (*m_pClasses);
	while(it.current())
	{
		l->append(it.current());
		++it;
	}
}

void KviClassEditor::appendAllClassItemsRecursive(KviPointerList<KviClassEditorTreeWidgetItem> * l,QTreeWidgetItem * pStartFrom)
{
	for (int i=0;i<pStartFrom->childCount();i++)
	{
		if (((KviClassEditorTreeWidgetItem *)pStartFrom->child(i))->isClass())
		{
			l->append((KviClassEditorTreeWidgetItem *)pStartFrom->child(i));
		} else {
			appendAllClassItemsRecursive(l,pStartFrom->child(i));
		}
	}
}

void KviClassEditor::removeItemChildren(KviClassEditorTreeWidgetItem *it,KviPointerList <KviClassEditorTreeWidgetItem> &lRemovedItems)
{
	if(it->isClass())
	{
		KviPointerList<KviClassEditorTreeWidgetItem> lInheritedClasses;
		lInheritedClasses.setAutoDelete(false);
		searchInheritedClasses(buildFullClassName(it),lInheritedClasses);
		for(unsigned int i=0;i<lInheritedClasses.count();i++)
		{
			lInheritedClasses.at(i)->setClassNotBuilt(true);
			lInheritedClasses.at(i)->setExpanded(true);
			lInheritedClasses.at(i)->setInheritsClass("object");
		}
	}
	
	while(it->childCount() > 0)
	{
		KviClassEditorTreeWidgetItem * pChild = (KviClassEditorTreeWidgetItem *)(it->child(0));
		if(pChild->childCount())
			removeItemChildren(pChild,lRemovedItems);
		if(pChild->isClass())
		{
			m_pClasses->removeRef(pChild);
			KviKvsObjectClass *pClass = KviKvsKernel::instance()->objectController()->lookupClass(buildFullClassName(pChild));
			if (pClass) KviKvsKernel::instance()->objectController()->deleteClass(pClass);
			qDebug("rimuovo class %s %p",buildFullClassName(pChild).toUtf8().data(), pClass);
		}
		it->removeChild(pChild);
		lRemovedItems.append(it);
		delete pChild;
	}
}

bool KviClassEditor::removeItem(KviClassEditorTreeWidgetItem *it,KviPointerList <KviClassEditorTreeWidgetItem> &lRemovedItems,bool * pbYesToAll,bool)
{
	if(!it)return true;
	QString szMsg;
	QString szName = it->name();

	if(!*pbYesToAll)
	{
		saveLastEditedItem();
		if(it->isClass())
		{
			KviQString::sprintf(szMsg,__tr2qs_ctx("Do you really want to remove the class \"%Q\" ?","editor"),&szName);
		} else if(it->isNamespace()) {
			KviQString::sprintf(szMsg,__tr2qs_ctx("Do you really want to remove the namespace \"%Q\" ?","editor"),&szName);
			szMsg += "<br>";
			szMsg += __tr2qs_ctx("Please note that all the children classes/functions will be deleted too.","editor");
		} else if(it->isMethod()) {
			KviQString::sprintf(szMsg,__tr2qs_ctx("Do you really want to remove the function \"%Q\" ?","editor"),&szName);
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
		removeItemChildren(it,lRemovedItems);
	if(it->isClass())
	{
		m_pClasses->removeRef(it);
		KviKvsObjectClass *pClass = KviKvsKernel::instance()->objectController()->lookupClass(buildFullClassName(it));
		qDebug("rimuovo class %s %p",buildFullClassName(it).toUtf8().data(), pClass);
		if (pClass) KviKvsKernel::instance()->objectController()->deleteClass(pClass);
		else
		{
			QString szFileName=buildFullClassName(it);
			szFileName.replace("::","--");
			szFileName.append(KVI_FILEEXTENSION_SCRIPT);
			QString szPath;
			g_pApp->getLocalKvircDirectory(szPath,KviApp::Classes);
			QDir d(szPath);
			if (d.exists(szFileName))
			{
				qDebug("rimuovo dal disco il file %s",szFileName.toUtf8().data());
				d.remove(szFileName);
			}
		}
	}
	if(it->isMethod())
	{
		updateClassHierarchy((KviClassEditorTreeWidgetItem*)it->parent());
	}
	lRemovedItems.append(it);
	delete it;
	return true;
}
void KviClassEditor::updateClassHierarchy(KviClassEditorTreeWidgetItem* pClass)
{
	pClass->setClassNotBuilt(true);
	KviPointerList<KviClassEditorTreeWidgetItem> lInheritedClasses;
	lInheritedClasses.setAutoDelete(false);
	searchInheritedClasses(pClass->name(),lInheritedClasses);
	for(unsigned int i=0;i<lInheritedClasses.count();i++)
	{
		lInheritedClasses.at(i)->setClassNotBuilt(true);
		lInheritedClasses.at(i)->setInheritsClass(pClass->name());
		lInheritedClasses.at(i)->setExpanded(true);
	}
}

void KviClassEditor::removeSelectedItems()
{
	KviPointerList<KviClassEditorTreeWidgetItem> lRemovedItems;
	lRemovedItems.setAutoDelete(false);
	QList<QTreeWidgetItem *> list=m_pTreeWidget->selectedItems();
	bool bYesToAll = false;
	for(int i=0;i<list.count();i++)
	{
		if (lRemovedItems.findRef((KviClassEditorTreeWidgetItem *) list.at(i))!=-1) continue;
		if(!removeItem((KviClassEditorTreeWidgetItem *) list.at(i),lRemovedItems,&bYesToAll,false))return;
	}
}

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

bool KviClassEditor::askForFunction(QString &szFunctionName,bool * bInternal,const QString &szClassName, bool bRenameMode)
{
	KviClassEditorFunctionDialog *pDialog=new KviClassEditorFunctionDialog(this,"function",szClassName,szFunctionName, *bInternal, bRenameMode);
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
	if(m_pLastClickedItem->isMethod()) m_pLastClickedItem= (KviClassEditorTreeWidgetItem*)m_pLastClickedItem->parent();
	szClassName=buildFullClassName(m_pLastClickedItem);
	bool bInternal= false;
	if(!askForFunction(szFunctionName, &bInternal,szClassName, false)) return;
	if(szFunctionName.isEmpty())return;
	KviClassEditorTreeWidgetItem *it=newItem(szFunctionName,KviClassEditorTreeWidgetItem::Method);
	it->setInternalFunction(bInternal);
	activateItem(it);
	((KviClassEditorTreeWidgetItem*)it->parent())->setClassNotBuilt(true);
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
	bool bErrorWhilecompiling=false;
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
		//	debug("compiling %s",pClass->name().toUtf8().data());
			KviClassEditorTreeWidgetItem *pParentClass=m_pClasses->find(pClass->InheritsClass());

			pLinkedClasses.append(pClass);
			//if (!pParentClass) debug("no parent class");
			while(pParentClass)
			{
				if (pParentClass->classNotBuilt()) pLinkedClasses.append(pParentClass);
				pParentClass=m_pClasses->find(pParentClass->InheritsClass());
			}
			for(int i=pLinkedClasses.count()-1;i>=0;i--)
			{
				QString szFullClassName = buildFullClassName(pLinkedClasses.at(i));
				KviKvsObjectClass *pClass = KviKvsKernel::instance()->objectController()->lookupClass(szFullClassName);
				if (pClass) KviKvsKernel::instance()->objectController()->deleteClass(pClass);
				QString szClass;
				getExportClassBuffer(szClass, pLinkedClasses.at(i));
				KviKvsScript::run(szClass,g_pActiveWindow);
				pClass = KviKvsKernel::instance()->objectController()->lookupClass(szFullClassName);
				if (!pClass)
				{
					bErrorWhilecompiling=true;
					QString szError=__tr2qs_ctx("Unable to compile class: ","editor");
					szError+=szFullClassName+"\n";
					KviPointerList<KviClassEditorTreeWidgetItem> lInheritedClasses;
					lInheritedClasses.setAutoDelete(false);
					searchInheritedClasses(szFullClassName,lInheritedClasses);
					if (lInheritedClasses.count())
					{
						szError+=__tr2qs_ctx("These Inherited classes will be not compiled too:","editor");
						szError+="\n";
						for(unsigned int j=0;j<lInheritedClasses.count();j++)
						{
							szError+=buildFullClassName(lInheritedClasses.at(j))+"\n";
							lInheritedClasses.at(j)->setClassNotBuilt(true);
							pSkipClasses.append(lInheritedClasses.at(j));
						}
					}
					QMessageBox::critical(this,__tr2qs_ctx("Compilation error - KVIrc","editor"),szError,
					QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
					break;
				}
				//else debug("class compiled %s :\n",szClass.toUtf8().data());
				pLinkedClasses.at(i)->setClassNotBuilt(false);
				m_pEditor->setModified(false);
			}
		}
		++it;
	}
	if (bErrorWhilecompiling) saveNotBuiltClasses();
	else{
		QString szFileName = "libkviclasseditortmp.kvc";
		QString szBuffer;
		g_pApp->getLocalKvircDirectory(szBuffer,KviApp::ConfigPlugins,szFileName);
		KviConfig cfg(szBuffer,KviConfig::Write);
		cfg.clear();
		cfg.sync();
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
				KviClassEditorTreeWidgetItem *pFunctionItem=findFunction(*s, pClassItem);
				if(!pFunctionItem) pFunctionItem = new KviClassEditorTreeWidgetItem(pClassItem,KviClassEditorTreeWidgetItem::Method,*s);
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
	cfg.clear();

	while(it.current())
	{
		if (it.current()->classNotBuilt())
		{
			KviKvsObjectClass *pClass = KviKvsKernel::instance()->objectController()->lookupClass(it.currentKey());
			if (pClass) KviKvsKernel::instance()->objectController()->deleteClass(pClass);

			cfg.setGroup(it.currentKey());
			cfg.writeEntry("@Inherits",it.current()->InheritsClass());
			for(int i=0;i<it.current()->childCount();i++)
				cfg.writeEntry(((KviClassEditorTreeWidgetItem*)it.current()->child(i))->name(),((KviClassEditorTreeWidgetItem*)it.current()->child(i))->buffer());
		}
		++it;
        }
        cfg.sync();
}

void KviClassEditor::searchInheritedClasses(const QString szClass,KviPointerList<KviClassEditorTreeWidgetItem> & lInheritedClasses)
{
	KviPointerHashTableIterator<QString,KviClassEditorTreeWidgetItem> it (*m_pClasses);
	while(it.current())
	{
		if (KviQString::equalCI(szClass,it.current()->InheritsClass())) lInheritedClasses.append(it.current());
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

	btn = new QPushButton(__tr2qs_ctx("Close","editor"),this);
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
	return g_pIconManager->getSmallIcon(KVI_SMALLICON_CLASSEDITOR);
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

	QGridLayout * pLayout = new QGridLayout(this);

	KviTalHBox * hbox = new KviTalHBox(this);
	hbox->setSpacing(0);
	hbox->setMargin(0);
	pLayout->addWidget(hbox,0,0);

	QLabel * pClassNameLabel = new QLabel(hbox);
	pClassNameLabel->setObjectName("classnamelabel");

	pClassNameLabel->setText(__tr2qs_ctx("Please enter the name for the class:","editor"));

	m_pClassNameLineEdit = new QLineEdit(hbox);
	m_pClassNameLineEdit->setObjectName("classnameineedit");
	m_pClassNameLineEdit->setText(szClassName);
	if (!bRenameMode)
	{
		// we allow only [\w:]+ class name
		QRegExp re("[\\w]+");
		QRegExpValidator *pValidator=new QRegExpValidator(re,this);
		m_pClassNameLineEdit->setValidator(pValidator);
		m_pClassNameLineEdit->setObjectName("functionameineedit");
		m_pClassNameLineEdit->setToolTip(__tr2qs_ctx("Class names can contain only letters, digits and underscores","editor"));
	}
	else
	{
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

	QStringList szClasses;
	while(pClassesIt.current())
	{
		if(!KviQString::equalCI(pClassesIt.currentKey(),szClassName))
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
		if (iCurrentIdx==-1) iCurrentIdx=m_pInheritsClassComboBox->findText("object");
		m_pInheritsClassComboBox->setCurrentIndex(iCurrentIdx);
	}
	else
	{
		int iCurrentIdx=m_pInheritsClassComboBox->findText("object");
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

KviClassEditorFunctionDialog::KviClassEditorFunctionDialog(QWidget * pParent, const QString & szName, const QString &szClassName, const QString &szFunctionName, bool bIsInternal, bool bRenameMode)
: QDialog(pParent)
{
	setObjectName(szName);

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
	pFunctionNameLabel->setText(__tr2qs_ctx("Please enter the name for the member function:","editor"));

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
	m_pInternalCheckBox->setChecked(bIsInternal);
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
	if (!bRenameMode) m_pNewFunctionButton->setText(__tr2qs_ctx("&Add","editor"));
	else m_pNewFunctionButton->setText(__tr2qs_ctx("&Rename","editor"));
	
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
