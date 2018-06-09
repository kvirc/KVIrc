//=============================================================================
//
//   File : ClassEditorWindow.cpp
//   Creation date : Mon Feb 15 2010 14:35:55 CEST by Carbone Alessandro
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2010 Alessandro Carbone (elfonol at gmail dot com)
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

#include "ClassEditorWindow.h"

#include "KviIconManager.h"
#include "KviOptions.h"
#include "KviLocale.h"
#include "KviImageDialog.h"
#include "KviConfigurationFile.h"
#include "KviFileDialog.h"
#include "KviFileUtils.h"
#include "KviScriptEditor.h"
#include "kvi_debug.h"
#include "KviApplication.h"
#include "KviMainWindow.h"
#include "KviQString.h"
#include "KviKvsKernel.h"
#include "KviKvsObjectClass.h"
#include "KviKvsObjectController.h"
#include "KviCommandFormatter.h"
#include "KviModule.h"
#include "KviTalVBox.h"
#include "kvi_fileextensions.h"
#include "KviModuleManager.h"

#include <QList>
#include <QSplitter>
#include <QLayout>
#include <QToolTip>
#include <QPushButton>
#include <QDir>
#include <QMessageBox>
#include <QInputDialog>
#include <QRegExp>
#include <QMouseEvent>
#include <QPainter>
#include <QBrush>
#include <QStringList>
#include <QLabel>

extern ClassEditorWindow * g_pClassEditorWindow;
extern KviModule * g_pClassEditorModule;

ClassEditorTreeWidget::ClassEditorTreeWidget(QWidget * pParent)
    : QTreeWidget(pParent)
{
	setColumnCount(1);
	setHeaderLabel(__tr2qs_ctx("Class", "editor"));
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setRootIsDecorated(true);
	setAnimated(true);
}

ClassEditorTreeWidget::~ClassEditorTreeWidget()
{
	//remove and delete all items
	clear();
}

void ClassEditorTreeWidget::mousePressEvent(QMouseEvent * e)
{
	if(e->button() == Qt::RightButton)
	{
		QTreeWidgetItem * pItem = itemAt(e->pos());
		if(pItem)
			emit rightButtonPressed(pItem, QCursor::pos());
	}
	QTreeWidget::mousePressEvent(e);
}

ClassEditorTreeWidgetItem::ClassEditorTreeWidgetItem(QTreeWidget * pTreeWidget, Type eType, const QString & szName)
    : QTreeWidgetItem(pTreeWidget), KviHeapObject(), m_eType(eType)
{
	setName(szName);
	m_szInheritsClassName = "";
	m_iPos = 0;
	m_bInternal = false;
	m_bClassModified = false;
	QPixmap * pIcon = nullptr;

	if(eType == ClassEditorTreeWidgetItem::Namespace)
		pIcon = g_pIconManager->getSmallIcon(KviIconManager::NameSpace);
	else
		pIcon = g_pIconManager->getSmallIcon(KviIconManager::Class);

	setIcon(0, QIcon(*pIcon));
}

ClassEditorTreeWidgetItem::ClassEditorTreeWidgetItem(ClassEditorTreeWidgetItem * pParentItem, Type eType, const QString & szName)
    : QTreeWidgetItem(pParentItem), m_eType(eType)
{
	setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	setName(szName);
	m_szInheritsClassName = "";
	m_bInternal = false;
	m_iPos = 0;
	m_bClassModified = false;
	QPixmap * pIcon = nullptr;

	if(eType == ClassEditorTreeWidgetItem::Namespace)
		pIcon = g_pIconManager->getSmallIcon(KviIconManager::NameSpace);
	else if(eType == ClassEditorTreeWidgetItem::Class)
		pIcon = g_pIconManager->getSmallIcon(KviIconManager::Class);
	else
		pIcon = g_pIconManager->getSmallIcon(KviIconManager::Function);

	setIcon(0, QIcon(*pIcon));
}

void ClassEditorTreeWidgetItem::setClassNotBuilt(bool bModified)
{
	m_bClassModified = bModified;
	QPixmap * pIcon = nullptr;

	if(bModified)
		pIcon = g_pIconManager->getSmallIcon(KviIconManager::ClassNotBuilt);
	else
		pIcon = g_pIconManager->getSmallIcon(KviIconManager::Class);

	setIcon(0, QIcon(*pIcon));
}

void ClassEditorTreeWidgetItem::setName(const QString & szName)
{
	m_szName = szName;
	setText(0, m_szName);
}

void ClassEditorTreeWidgetItem::setType(Type eType)
{
	m_eType = eType;
	QPixmap * pIcon = nullptr;

	if(eType == ClassEditorTreeWidgetItem::Namespace)
		pIcon = g_pIconManager->getSmallIcon(KviIconManager::NameSpace);
	else if(eType == ClassEditorTreeWidgetItem::Class)
		pIcon = g_pIconManager->getSmallIcon(KviIconManager::Class);
	else
		pIcon = g_pIconManager->getSmallIcon(KviIconManager::Function);

	setIcon(0, QIcon(*pIcon));
}

ClassEditorWidget::ClassEditorWidget(QWidget * pParent)
    : QWidget(pParent)
{
	m_pClasses = new KviPointerHashTable<QString, ClassEditorTreeWidgetItem>(100, false);
	m_pClasses->setAutoDelete(false);
	m_pLastEditedItem = nullptr;
	m_pLastClickedItem = nullptr;
	m_szDir = QDir::homePath();

	QGridLayout * pLayout = new QGridLayout(this);

	m_pSplitter = new QSplitter(Qt::Horizontal, this);
	m_pSplitter->setChildrenCollapsible(false);
	pLayout->addWidget(m_pSplitter, 0, 0);

	KviTalVBox * pVBox = new KviTalVBox(m_pSplitter);
	pVBox->setSpacing(0);
	pVBox->setMargin(0);
	m_pTreeWidget = new ClassEditorTreeWidget(pVBox);

	pVBox = new KviTalVBox(m_pSplitter);

	KviTalHBox * pHBox = new KviTalHBox(pVBox);
	pHBox->setSpacing(0);
	pHBox->setMargin(0);
	m_pClassNameLabel = new QLabel(__tr2qs_ctx("No item selected", "editor"), pHBox);
	pHBox->setStretchFactor(m_pClassNameLabel, 2);
	m_pClassNameLabel->setWordWrap(true);
	m_pClassNameRenameButton = new QPushButton(__tr2qs_ctx("Rename", "editor"), pHBox);
	m_pClassNameRenameButton->setToolTip(__tr2qs_ctx("Edit the class or namespace name", "editor"));
	m_pClassNameRenameButton->setEnabled(false);
	connect(m_pClassNameRenameButton, SIGNAL(clicked()), this, SLOT(renameItem()));

	pHBox = new KviTalHBox(pVBox);
	pHBox->setSpacing(0);
	pHBox->setMargin(0);

	pHBox = new KviTalHBox(pVBox);
	pHBox->setSpacing(0);
	pHBox->setMargin(0);
	m_pFunctionNameLabel = new QLabel(__tr2qs_ctx("No item selected", "editor"), pHBox);
	pHBox->setStretchFactor(m_pFunctionNameLabel, 2);
	m_pFunctionNameRenameButton = new QPushButton(__tr2qs_ctx("Rename", "editor"), pHBox);
	m_pFunctionNameRenameButton->setToolTip(__tr2qs_ctx("Edit the function member name", "editor"));
	m_pFunctionNameRenameButton->setEnabled(false);
	connect(m_pFunctionNameRenameButton, SIGNAL(clicked()), this, SLOT(renameFunction()));

	m_pReminderLabel = new QLabel(__tr2qs_ctx("No item selected", "editor"), pVBox);
	m_pReminderLabel->hide();
	m_pReminderLabel->setWordWrap(true);

	m_pEditor = KviScriptEditor::createInstance(pVBox);
	m_pEditor->setFocus();
	connect(m_pEditor, SIGNAL(find(const QString &)), this, SLOT(slotFindWord(const QString &)));
	connect(m_pEditor, SIGNAL(replaceAll(const QString &, const QString &)), this, SLOT(slotReplaceAll(const QString &, const QString &)));
	m_pContextPopup = new QMenu(this);

	oneTimeSetup();
	m_pTreeWidget->setSortingEnabled(true);
	currentItemChanged(nullptr, nullptr);
}

ClassEditorWidget::~ClassEditorWidget()
{
	m_pClasses->clear();
	delete m_pClasses;
}

void ClassEditorWidget::buildFullItemPath(ClassEditorTreeWidgetItem * pItem, QString & szBuffer)
{
	if(!pItem)
		return;
	szBuffer.prepend(pItem->name() + "::");
	pItem = (ClassEditorTreeWidgetItem *)pItem->parent();
	while(pItem)
	{
		QString szTmp = pItem->name();
		if(!szTmp.isEmpty())
		{
			szBuffer.prepend("::");
			szBuffer.prepend(szTmp);
		}
		pItem = (ClassEditorTreeWidgetItem *)pItem->parent();
	}
}

QString ClassEditorWidget::buildFullClassName(ClassEditorTreeWidgetItem * pItem)
{
	if(!pItem)
		return QString();
	QString szName = pItem->name();
	ClassEditorTreeWidgetItem * pNItem = (ClassEditorTreeWidgetItem *)pItem->parent();
	while(pNItem)
	{
		QString szTmp = pNItem->name();
		if(!szTmp.isEmpty())
		{
			szName.prepend("::");
			szName.prepend(szTmp);
		}
		pNItem = (ClassEditorTreeWidgetItem *)pNItem->parent();
	}
	return szName;
}

ClassEditorTreeWidgetItem * ClassEditorWidget::findTopLevelItem(const QString & szName)
{
	for(int i = 0; i < m_pTreeWidget->topLevelItemCount(); i++)
	{
		if(KviQString::equalCI(m_pTreeWidget->topLevelItem(i)->text(0), szName))
			return (ClassEditorTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
	}
	return nullptr;
}

ClassEditorTreeWidgetItem * ClassEditorWidget::findItem(const QString & szFullName)
{
	bool bFound;
	QStringList lNamespaces = szFullName.split("::");
	if(lNamespaces.count() == 0)
		return nullptr;
	ClassEditorTreeWidgetItem * pItem = findTopLevelItem(lNamespaces.at(0));
	if(!pItem)
		return nullptr;

	for(int i = 1; i < lNamespaces.count(); i++)
	{
		bFound = false;
		for(int j = 0; j < pItem->childCount(); j++)
		{
			if(KviQString::equalCI(pItem->child(j)->text(0), lNamespaces.at(i)))
			{
				pItem = (ClassEditorTreeWidgetItem *)pItem->child(j);
				bFound = true;
				break;
			}
		}
		if(!bFound)
			return nullptr;
	}
	return (ClassEditorTreeWidgetItem *)pItem;
}

ClassEditorTreeWidgetItem * ClassEditorWidget::createFullItem(const QString & szFullName)
{
	bool bFound;
	int i;

	QStringList lNamespaces = szFullName.split("::");
	if(lNamespaces.count() == 0)
		return nullptr;
	if(lNamespaces.count() == 1)
	{
		ClassEditorTreeWidgetItem * pItem = findTopLevelItem(lNamespaces.at(0));
		if(pItem)
			return pItem;
		return new ClassEditorTreeWidgetItem(m_pTreeWidget, ClassEditorTreeWidgetItem::Class, lNamespaces.at(0));
	}
	ClassEditorTreeWidgetItem * pItem = findTopLevelItem(lNamespaces.at(0));
	if(!pItem)
		pItem = new ClassEditorTreeWidgetItem(m_pTreeWidget, ClassEditorTreeWidgetItem::Namespace, lNamespaces.at(0));

	for(i = 1; i < lNamespaces.count() - 1; i++)
	{
		bFound = false;
		for(int j = 0; j < pItem->childCount(); j++)
		{
			if(KviQString::equalCI(pItem->child(j)->text(0), lNamespaces.at(i)))
			{
				pItem = (ClassEditorTreeWidgetItem *)pItem->child(j);
				bFound = true;
				break;
			}
		}
		if(!bFound)
			pItem = new ClassEditorTreeWidgetItem(pItem, ClassEditorTreeWidgetItem::Namespace, lNamespaces.at(i));
	}
	return new ClassEditorTreeWidgetItem(pItem, ClassEditorTreeWidgetItem::Class, lNamespaces.at(i));
}

ClassEditorTreeWidgetItem * ClassEditorWidget::createFullNamespace(const QString & szFullName)
{
	bool bFound;
	int i;

	QStringList lNamespaces = szFullName.split("::");
	if(lNamespaces.count() == 0)
		return nullptr;
	ClassEditorTreeWidgetItem * pItem = findTopLevelItem(lNamespaces.at(0));
	if(!pItem)
		pItem = new ClassEditorTreeWidgetItem(m_pTreeWidget, ClassEditorTreeWidgetItem::Namespace, lNamespaces.at(0));
	if(lNamespaces.count() == 1)
		return pItem;

	for(i = 1; i < lNamespaces.count(); i++)
	{
		bFound = false;
		for(int j = 0; j < pItem->childCount(); j++)
		{
			if(KviQString::equalCI(pItem->child(j)->text(0), lNamespaces.at(i)) && ((ClassEditorTreeWidgetItem *)pItem->child(j))->isNamespace())
			{
				pItem = (ClassEditorTreeWidgetItem *)pItem->child(j);
				bFound = true;
				break;
			}
		}
		if(!bFound)
			pItem = new ClassEditorTreeWidgetItem(pItem, ClassEditorTreeWidgetItem::Namespace, lNamespaces.at(i));
	}
	return pItem;
}

void ClassEditorWidget::oneTimeSetup()
{
	QStringList sl;
	QString szClassName;
	QString szPath;
	g_pApp->getLocalKvircDirectory(szPath, KviApplication::Classes);
	QDir d(szPath);

	QString szExtension = QString("*%1").arg(KVI_FILEEXTENSION_SCRIPT); // *.kvs
	sl = d.entryList(QStringList(szExtension), QDir::Files | QDir::NoDotAndDotDot);

	g_pModuleManager->getModule("objects");
	KviPointerHashTableIterator<QString, KviKvsObjectClass> it(*KviKvsKernel::instance()->objectController()->classDict());

	KviKvsObjectClass * pClass = nullptr;
	ClassEditorTreeWidgetItem * pClassItem = nullptr;
	while(KviKvsObjectClass * pClass = it.current())
	{
		if(pClass->isBuiltin())
			m_pClasses->insert(it.currentKey(), nullptr);
		else
		{
			QString szTmp;
			szTmp = it.currentKey();
			szTmp.replace("::", "--");
			szTmp.append(".kvs");
			if(sl.indexOf(szTmp) == -1)
			{
				szClassName = it.currentKey();
				pClassItem = createFullItem(szClassName);
				createFullClass(it.current(), pClassItem, szClassName);
			}
		}
		++it;
	}

	for(int i = 0; i < sl.count(); i++)
	{
		szClassName = sl.at(i);
		szClassName.replace("--", "::");
		szClassName.chop(4);
		pClassItem = createFullItem(szClassName);
		pClass = KviKvsKernel::instance()->objectController()->lookupClass(szClassName);
		if(pClass)
			createFullClass(pClass, pClassItem, szClassName);
	}
	loadNotBuiltClasses();
	connect(m_pTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));
	m_pTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_pTreeWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(customContextMenuRequested(const QPoint &)));
}

void ClassEditorWidget::createFullClass(KviKvsObjectClass * pClass, ClassEditorTreeWidgetItem * pClassItem, const QString & szClassName)
{
	KviPointerHashTableIterator<QString, KviKvsObjectFunctionHandler> it(*pClass->getHandlers());
	QStringList szFunctionsList;
	while(it.current())
	{
		szFunctionsList.append(it.currentKey());
		++it;
	}
	szFunctionsList.sort();
	ClassEditorTreeWidgetItem * pFunctionItem;
	m_pClasses->insert(szClassName, pClassItem);
	KviKvsObjectClass * pParentClass = pClass->parentClass();
	pClassItem->setInheritsClass(pParentClass->name());
	for(int i = 0; i < szFunctionsList.count(); i++)
	{
		QString szCode;
		KviKvsObjectFunctionHandler * pHandler = pClass->lookupFunctionHandler(szFunctionsList.at(i));
		if(pClass->isScriptHandler(szFunctionsList.at(i)) && !pHandler->isClone())
		{
			pFunctionItem = findFunction(szFunctionsList.at(i), pClassItem);
			if(!pFunctionItem)
				pFunctionItem = new ClassEditorTreeWidgetItem(pClassItem, ClassEditorTreeWidgetItem::Method, szFunctionsList.at(i));
			pClass->getFunctionCode(szCode, *pHandler);
			pFunctionItem->setBuffer(szCode);
			pFunctionItem->setReminder(pClass->reminder(pHandler));
			if(pHandler->flags() & KviKvsObjectFunctionHandler::Internal)
				pFunctionItem->setInternalFunction(true);
		}
	}
}

bool ClassEditorWidget::hasSelectedItems()
{
	return m_pTreeWidget->selectedItems().count() ? true : false;
}

bool ClassEditorWidget::classExists(QString & szFullItemName)
{
	if(m_pClasses->find(szFullItemName))
		return true;
	else
		return false;
}

void ClassEditorWidget::renameFunction()
{
	if(!m_pLastEditedItem)
		return;
	ClassEditorTreeWidgetItem * pFunction = m_pLastEditedItem;
	QString szClassName = buildFullClassName((ClassEditorTreeWidgetItem *)pFunction->parent());
	QString szFunctionName = pFunction->name();
	QString szReminder = pFunction->reminder();
	QString szNewReminder = szReminder;
	ClassEditorTreeWidgetItem * pParentClass = (ClassEditorTreeWidgetItem *)pFunction->parent();

	QString szNewFunctionName = szFunctionName;
	bool bInternal = pFunction->isInternalFunction();
	if(!askForFunction(szNewFunctionName, szNewReminder, &bInternal, szClassName, true))
		return;
	if(KviQString::equalCS(szFunctionName, szNewFunctionName) && bInternal == pFunction->isInternalFunction())
	{
		if(!KviQString::equalCS(szNewReminder, szReminder))
		{
			pFunction->setReminder(szNewReminder);
			KviKvsObjectClass * pClass = KviKvsKernel::instance()->objectController()->lookupClass(szClassName);
			if(pClass)
			{
				KviKvsObjectFunctionHandler * pHandler = pClass->lookupFunctionHandler(szFunctionName);
				if(pHandler)
				{
					pClass->setReminder(szNewReminder, pHandler);
					QString szPath;
					QString szFileName = szClassName.toLower();
					szFileName += ".kvs";
					szFileName.replace("::", "--");
					g_pApp->getLocalKvircDirectory(szPath, KviApplication::Classes, szFileName);
					pClass->save(szPath);
				}
			}
			currentItemChanged(pFunction, pFunction);
		}
		else
		{
			g_pClassEditorModule->lock();
			QMessageBox::information(this,
			    __tr2qs_ctx("Name Already Exists - KVIrc", "editor"),
			    __tr2qs_ctx("This function name is already in use. Please choose another one.", "editor"),
			    __tr2qs_ctx("OK, Let me try again...", "editor"));
			g_pClassEditorModule->unlock();
			return;
		}
	}

	pFunction->setName(szNewFunctionName);
	pFunction->setReminder(szNewReminder);
	currentItemChanged(pFunction, pFunction);
	pFunction->setInternalFunction(bInternal);
	pParentClass->setClassNotBuilt(true);

	KviPointerList<ClassEditorTreeWidgetItem> lInheritedClasses;
	lInheritedClasses.setAutoDelete(false);
	searchInheritedClasses(szClassName, lInheritedClasses);
	for(unsigned int i = 0; i < lInheritedClasses.count(); i++)
		lInheritedClasses.at(i)->setClassNotBuilt(true);
	activateItem(pFunction);
}

ClassEditorTreeWidgetItem * ClassEditorWidget::findFunction(const QString & szFunctionName, ClassEditorTreeWidgetItem * pClass)
{
	for(int i = 0; i < pClass->childCount(); i++)
	{
		if(KviQString::equalCI(szFunctionName, ((ClassEditorTreeWidgetItem *)pClass->child(i))->name()))
			return (ClassEditorTreeWidgetItem *)pClass->child(i);
	}
	return nullptr;
}

void ClassEditorWidget::renameItem()
{
	if(!m_pLastEditedItem)
		return;
	if(m_pLastEditedItem->isClass())
		renameClass(m_pLastEditedItem);
	else if(m_pLastEditedItem->isNamespace())
		renameNamespace(m_pLastEditedItem);
	else
	{
		// is function
		ClassEditorTreeWidgetItem * pParent = (ClassEditorTreeWidgetItem *)m_pLastEditedItem->parent();
		if(pParent->isClass())
			renameClass(pParent);
	}
}

void ClassEditorWidget::renameClass(ClassEditorTreeWidgetItem * pClassItem)
{
	QString szClassName = buildFullClassName(pClassItem);
	QString szNewClassName = szClassName;

	QString szInheritsClassName = pClassItem->inheritsClass();
	QString szNewInheritsClassName = szInheritsClassName;
	bool bOk = askForClassName(szNewClassName, szNewInheritsClassName, true);
	if(!bOk)
		return;
	if(classExists(szNewClassName) && KviQString::equalCS(szClassName, szNewClassName) && KviQString::equalCS(szInheritsClassName, szNewInheritsClassName))
	{
		g_pClassEditorModule->lock();
		QMessageBox::information(this,
		    __tr2qs_ctx("Name Already Exists - KVIrc", "editor"),
		    __tr2qs_ctx("This class name is already in use. Please choose another one.", "editor"),
		    __tr2qs_ctx("OK, Let me try again...", "editor"));
		g_pClassEditorModule->unlock();
		return;
	}
	/*if(szNewClassName.tmp.indexOf("@@",Qt::CaseInsensitive) != -1)
	{
		g_pClassEditorModule->lock();
		QMessageBox::information(this,
			__tr2qs_ctx("Bad Namespace Name","editor"),
			__tr2qs_ctx("Found an empty namespace in namespace name","editor"),
			__tr2qs_ctx("OK, Let me try again...","editor"));
		g_pClassEditorModule->unlock();
		szNewName = "";
		continue;
	}*/
	ClassEditorTreeWidgetItem * pNewItem = nullptr;
	m_pClasses->removeRef(pClassItem);
	cutItem(pClassItem);
	if(szNewClassName.contains("::"))
	{
		pNewItem = createFullNamespace(szNewClassName.left(szNewClassName.lastIndexOf("::")));
		pClassItem->setName(szNewClassName.section("::", -1, -1));
		pNewItem->addChild(pClassItem);
	}
	else
	{
		pClassItem->setName(szNewClassName);
		m_pTreeWidget->addTopLevelItem(pClassItem);
	}
	m_pClasses->insert(szNewClassName, pClassItem);
	pClassItem->setInheritsClass(szNewInheritsClassName);
	pClassItem->setClassNotBuilt(true);

	KviPointerList<ClassEditorTreeWidgetItem> lInheritedClasses;
	lInheritedClasses.setAutoDelete(false);
	searchInheritedClasses(szClassName, lInheritedClasses);
	for(unsigned int i = 0; i < lInheritedClasses.count(); i++)
	{
		lInheritedClasses.at(i)->setClassNotBuilt(true);
		lInheritedClasses.at(i)->setExpanded(true);
		lInheritedClasses.at(i)->setInheritsClass(szNewClassName);
	}

	if(pNewItem)
	{
		activateItem(pNewItem);
		pNewItem->setExpanded(true);
	}
	else
	{
		activateItem(pClassItem);
		pClassItem->setExpanded(true);
	}

	qDebug("delete class %s caused by rename", szClassName.toUtf8().data());
	KviKvsObjectClass * pClass = KviKvsKernel::instance()->objectController()->lookupClass(szClassName);
	if(pClass)
		KviKvsKernel::instance()->objectController()->deleteClass(pClass);
}

void ClassEditorWidget::cutItem(ClassEditorTreeWidgetItem * pItem)
{
	int iIdx = m_pTreeWidget->indexOfTopLevelItem(pItem);
	if(iIdx != -1)
		m_pTreeWidget->takeTopLevelItem(iIdx);
	else
	{
		ClassEditorTreeWidgetItem * pParent = (ClassEditorTreeWidgetItem *)pItem->parent();
		pParent->removeChild(pItem);
	}
}

void ClassEditorWidget::renameNamespace(ClassEditorTreeWidgetItem * pOldNamespaceItem)
{
	QString szOldNameSpaceName = buildFullClassName(m_pLastEditedItem);
	QString szNewNameSpaceName;
	if(!askForNamespaceName(__tr2qs_ctx("Enter a New Name - KVIrc", "editor"), __tr2qs_ctx("Please enter the new name for the namespace.", "editor"), szOldNameSpaceName, szNewNameSpaceName))
		return;
	if(KviQString::equalCI(szOldNameSpaceName, szNewNameSpaceName))
		return;
	ClassEditorTreeWidgetItem * pNewItem = findItem(szNewNameSpaceName);
	if(pNewItem)
	{
		g_pClassEditorModule->lock();
		if(pNewItem->isClass())
		{
			QMessageBox::information(this,
			    __tr2qs_ctx("Name Already Exists - KVIrc", "editor"),
			    __tr2qs_ctx("This class name is already in use. Please choose another one.", "editor"),
			    __tr2qs_ctx("OK, Let me try again...", "editor"));
		}
		else
		{
			QMessageBox::information(this,
			    __tr2qs_ctx("Name Already Exists - KVIrc", "editor"),
			    __tr2qs_ctx("This namespace name is already in use. Please choose another one.", "editor"),
			    __tr2qs_ctx("OK, Let me try again...", "editor"));
		}
		g_pClassEditorModule->unlock();
		return;
	}
	KviPointerList<ClassEditorTreeWidgetItem> pList;
	pList.setAutoDelete(false);
	appendAllClassItemsRecursive(&pList, pOldNamespaceItem);
	cutItem(pOldNamespaceItem);
	if(szNewNameSpaceName.contains("::"))
	{
		pNewItem = createFullNamespace(szNewNameSpaceName.left(szNewNameSpaceName.lastIndexOf("::")));
		pOldNamespaceItem->setName(szNewNameSpaceName.section("::", -1, -1));
		pNewItem->addChild(pOldNamespaceItem);
	}
	else
	{
		m_pTreeWidget->addTopLevelItem(pOldNamespaceItem);
		pOldNamespaceItem->setName(szNewNameSpaceName);
	}
	for(unsigned int u = 0; u < pList.count(); u++)
	{
		KviPointerHashTableEntry<QString, ClassEditorTreeWidgetItem> * pEntry = m_pClasses->findRef(pList.at(u));
		if(pEntry)
		{
			KviPointerList<ClassEditorTreeWidgetItem> lInheritedClasses;
			lInheritedClasses.setAutoDelete(false);

			QString szOldName = pEntry->key();
			QString szNewName = buildFullClassName(pList.at(u));
			searchInheritedClasses(szOldName, lInheritedClasses);
			for(unsigned int v = 0; v < lInheritedClasses.count(); v++)
			{
				lInheritedClasses.at(v)->setClassNotBuilt(true);
				lInheritedClasses.at(v)->setExpanded(true);
				lInheritedClasses.at(v)->setInheritsClass(szNewName);
			}
			m_pClasses->removeRef(pList.at(u));
			m_pClasses->insert(szNewName, pList.at(u));
			pList.at(u)->setClassNotBuilt(true);

			KviKvsObjectClass * pClass = KviKvsKernel::instance()->objectController()->lookupClass(szOldName);
			if(pClass)
				KviKvsKernel::instance()->objectController()->deleteClass(pClass);
		}
	}

	if(pNewItem)
	{
		activateItem(pNewItem);
		pNewItem->setExpanded(true);
	}
	else
	{
		activateItem(pOldNamespaceItem);
		pOldNamespaceItem->setExpanded(true);
	}
	//searchReplace(szOldNameSpaceName+"::",true,szNewNameSpaceName+"::");
}

void ClassEditorWidget::saveLastEditedItem()
{
	if(!m_pLastEditedItem)
		return;
	if(!m_pEditor->isModified() || m_pLastEditedItem->isNamespace() || m_pLastEditedItem->isClass())
		return;
	((ClassEditorTreeWidgetItem *)m_pLastEditedItem)->setCursorPosition(m_pEditor->getCursor());
	QString newCode;
	m_pEditor->getText(newCode);
	((ClassEditorTreeWidgetItem *)m_pLastEditedItem)->setBuffer(newCode);
	((ClassEditorTreeWidgetItem *)m_pLastEditedItem->parent())->setClassNotBuilt(true);
}

void ClassEditorWidget::currentItemChanged(QTreeWidgetItem * pTree, QTreeWidgetItem *)
{
	saveLastEditedItem();
	m_pLastEditedItem = (ClassEditorTreeWidgetItem *)pTree;
	if(!m_pLastEditedItem)
	{
		m_pClassNameLabel->setText(__tr2qs_ctx("No item selected", "editor"));
		m_pClassNameRenameButton->setEnabled(false);
		m_pEditor->setText("");
		m_pEditor->setEnabled(false);
		return;
	}

	ClassEditorTreeWidgetItem * pClassItem = nullptr;
	if(m_pLastEditedItem->isMethod())
		pClassItem = (ClassEditorTreeWidgetItem *)m_pLastEditedItem->parent();
	else
		pClassItem = m_pLastEditedItem;

	QString szClassName = buildFullClassName(pClassItem);

	if(m_pLastEditedItem->isNamespace())
	{
		QString szLabelText = __tr2qs_ctx("Namespace", "editor");
		szLabelText += ": <b>";
		szLabelText += szClassName;
		szLabelText += "</b>";
		m_pClassNameLabel->setText(szLabelText);
		m_pClassNameRenameButton->setEnabled(true);
		m_pFunctionNameRenameButton->setEnabled(false);
		//m_pinheritsClassNameLabel->setText("");
		m_pFunctionNameLabel->setText("");
		m_pEditor->setText("");
		m_pEditor->setEnabled(false);
		m_pTreeWidget->setFocus();
		return;
	}

	QString szLabelText = __tr2qs_ctx("Class", "editor");
	szLabelText += ": <b>";
	szLabelText += szClassName;
	szLabelText += "</b>, ";
	szLabelText += __tr2qs_ctx("inherits from class ", "editor");
	szLabelText += ": <b>";
	szLabelText += pClassItem->inheritsClass();
	szLabelText += "</b>";
	m_pClassNameLabel->setText(szLabelText);

	szLabelText = __tr2qs_ctx("Member Function", "editor");
	if(m_pLastEditedItem->isMethod())
	{
		szLabelText += ": <b>";
		szLabelText += m_pLastEditedItem->text(0);
		szLabelText += "</b>";
		m_pFunctionNameRenameButton->setEnabled(true);
		if(m_pLastEditedItem->reminder().isEmpty())
			m_pReminderLabel->hide();
		else
		{
			QString szReminderText = __tr2qs_ctx("Reminder text.", "editor");
			szReminderText += ": <b>";
			szReminderText += m_pLastEditedItem->reminder();
			szReminderText += "</b>";
			m_pReminderLabel->setText(szReminderText);
			m_pReminderLabel->show();
		}

		m_pFunctionNameLabel->setText(szLabelText);
		m_pFunctionNameLabel->show();
		m_pFunctionNameRenameButton->show();
	}
	else
	{
		m_pReminderLabel->hide();
		m_pFunctionNameLabel->hide();
		m_pClassNameRenameButton->setEnabled(true);
		m_pFunctionNameRenameButton->hide();
	}
	if(m_pLastEditedItem->isClass())
	{
		m_pFunctionNameRenameButton->setEnabled(false);
		m_pEditor->setText("");
		m_pEditor->setEnabled(true);
		m_pTreeWidget->setFocus();
		QString szBuffer;
		QStringList szFunctionsList;
		KviPointerHashTable<QString, ClassEditorTreeWidgetItem> lFunctions;
		lFunctions.setAutoDelete(false);

		ClassEditorTreeWidgetItem * pItem = nullptr;
		for(int i = 0; i < pTree->childCount(); i++)
		{
			pItem = ((ClassEditorTreeWidgetItem *)pTree->child(i));
			szFunctionsList.append(pItem->name());
			lFunctions.insert(pItem->name(), pItem);
		}
		szFunctionsList.sort();
		for(int i = 0; i < szFunctionsList.count(); i++)
		{
			szBuffer += "Member Function: <b>$" + szFunctionsList.at(i) + "</b><br>";
			if(!lFunctions.find(szFunctionsList.at(i))->reminder().isEmpty())
				szBuffer += "Parameters reminder: " + lFunctions.find(szFunctionsList.at(i))->reminder() + "<br>";
			szBuffer += "<br>";
		}
		m_pEditor->setUnHighlightedText(szBuffer);
		m_pEditor->setReadOnly(true);
		return;
	}
	m_pEditor->setReadOnly(false);
	m_pEditor->setText(((ClassEditorTreeWidgetItem *)pTree)->buffer());
	m_pEditor->setFocus();
	m_pEditor->setCursorPosition(((ClassEditorTreeWidgetItem *)pTree)->cursorPosition());
	m_pEditor->setEnabled(true);
}

void ClassEditorWidget::customContextMenuRequested(QPoint pnt)
{
	m_pContextPopup->clear();

	m_pLastClickedItem = (ClassEditorTreeWidgetItem *)m_pTreeWidget->itemAt(pnt);

	QAction * pAction = m_pContextPopup->addAction(
	    *(g_pIconManager->getSmallIcon(KviIconManager::NameSpace)),
	    __tr2qs_ctx("Add Namespace", "editor"),
	    this, SLOT(newNamespace()));
	if(!m_pLastClickedItem)
		pAction->setEnabled(true);
	else
		pAction->setEnabled(m_pLastClickedItem->isNamespace());

	pAction = m_pContextPopup->addAction(
	    *(g_pIconManager->getSmallIcon(KviIconManager::Class)),
	    __tr2qs_ctx("Add Class", "editor"),
	    this, SLOT(newClass()));
	if(!m_pLastClickedItem)
		pAction->setEnabled(true);
	else
		pAction->setEnabled(m_pLastClickedItem->isNamespace());

	pAction = m_pContextPopup->addAction(
	    *(g_pIconManager->getSmallIcon(KviIconManager::Function)),
	    __tr2qs_ctx("Add Member Function", "editor"),
	    this, SLOT(newMemberFunction()));
	if(!m_pLastClickedItem)
		pAction->setEnabled(false);
	else
		pAction->setEnabled(m_pLastClickedItem->isClass() | m_pLastClickedItem->isMethod());

	bool bHasItems = m_pTreeWidget->topLevelItemCount();
	bool bHasSelected = hasSelectedItems();

	m_pContextPopup->addSeparator();

	pAction = m_pContextPopup->addAction(
	    *(g_pIconManager->getSmallIcon(KviIconManager::Discard)),
	    __tr2qs_ctx("Remove Selected", "editor"),
	    this, SLOT(removeSelectedItems()));
	pAction->setEnabled(bHasSelected);

	m_pContextPopup->addSeparator();

	m_pContextPopup->addAction(
	    *(g_pIconManager->getSmallIcon(KviIconManager::Save)),
	    __tr2qs_ctx("Export Selected...", "editor"),
	    this, SLOT(exportSelected()));
	pAction->setEnabled(bHasSelected);

	m_pContextPopup->addAction(
	    *(g_pIconManager->getSmallIcon(KviIconManager::Save)),
	    __tr2qs_ctx("Export Selected in Single Files...", "editor"),
	    this, SLOT(exportSelectedSepFiles()));

	pAction->setEnabled(bHasSelected);

	m_pContextPopup->addAction(
	    *(g_pIconManager->getSmallIcon(KviIconManager::Save)),
	    __tr2qs_ctx("Export All...", "editor"),
	    this, SLOT(exportAll()));
	pAction->setEnabled(bHasItems);

	m_pContextPopup->addSeparator();

	m_pContextPopup->addAction(
	    *(g_pIconManager->getSmallIcon(KviIconManager::Search)),
	    __tr2qs_ctx("Find in Classes...", "editor"),
	    this, SLOT(slotFind()));
	pAction->setEnabled(bHasItems);

	m_pContextPopup->addAction(
	    *(g_pIconManager->getSmallIcon(KviIconManager::NameSpace)),
	    __tr2qs_ctx("Collapse All Items", "editor"),
	    this, SLOT(slotCollapseItems()));

	pAction->setEnabled(bHasItems);
	m_pContextPopup->popup(m_pTreeWidget->mapToGlobal(pnt));
}

void ClassEditorWidget::searchReplace(const QString & szSearch, bool bReplace, const QString & szReplace)
{
	KviPointerHashTableIterator<QString, ClassEditorTreeWidgetItem> it(*m_pClasses);
	while(it.current())
	{
		ClassEditorTreeWidgetItem * pItem = it.current();
		for(int j = 0; j < pItem->childCount(); j++)
		{
			bool bOpened = false;
			if(((ClassEditorTreeWidgetItem *)pItem->child(j))->buffer().indexOf(szSearch, 0, Qt::CaseInsensitive) != -1)
			{
				pItem->child(j)->setBackground(0, QColor(255, 0, 0, 128));
				if(bReplace)
				{
					QString & buffer = (QString &)((ClassEditorTreeWidgetItem *)pItem->child(j))->buffer();
					pItem->setClassNotBuilt(true);
					buffer.replace(szSearch, szReplace, Qt::CaseInsensitive);
				}
				if(!bOpened)
				{
					openParentItems(pItem->child(j));
					bOpened = true;
				}
			}
			else
			{
				pItem->child(j)->setBackground(0, QColor(255, 255, 255));
			}
		}
		++it;
	}
}

void ClassEditorWidget::slotFind()
{
	g_pClassEditorModule->lock();
	bool bOk;

	QString szSearch = QInputDialog::getText(this,
	    __tr2qs_ctx("Find In Classes", "editor"),
	    __tr2qs_ctx("Please enter the text to be searched for. The matching function will be highlighted.", "editor"),
	    QLineEdit::Normal,
	    "",
	    &bOk);

	g_pClassEditorModule->unlock();
	if(!bOk)
		return;
	if(szSearch.isEmpty())
		return;
	m_pEditor->setFindText(szSearch);
	searchReplace(szSearch);
}

void ClassEditorWidget::slotFindWord(const QString & szSearch)
{
	m_pEditor->setFindText(szSearch);
}

void ClassEditorWidget::recursiveCollapseItems(ClassEditorTreeWidgetItem * pItem)
{
	if(!pItem)
		return;
	for(int i = 0; i < pItem->childCount(); i++)
	{
		if(pItem->child(i)->childCount())
		{
			pItem->child(i)->setExpanded(false);
			recursiveCollapseItems((ClassEditorTreeWidgetItem *)pItem->child(i));
		}
	}
}

void ClassEditorWidget::slotCollapseItems()
{
	for(int i = 0; i < m_pTreeWidget->topLevelItemCount(); i++)
	{
		if(m_pTreeWidget->topLevelItem(i)->childCount())
		{
			m_pTreeWidget->topLevelItem(i)->setExpanded(false);
			recursiveCollapseItems((ClassEditorTreeWidgetItem *)m_pTreeWidget->topLevelItem(i));
		}
	}
}

void ClassEditorWidget::slotReplaceAll(const QString & szFind, const QString & szReplace)
{
	m_pEditor->setFindText(szReplace);
	searchReplace(szFind, true, szReplace);
}

void ClassEditorWidget::exportClassBuffer(QString & szBuffer, ClassEditorTreeWidgetItem * pItem)
{
	QString szTmp = pItem->buffer();
	KviCommandFormatter::blockFromBuffer(szTmp);
	QString szName = buildFullClassName(pItem);

	szBuffer = "class(\"";
	szBuffer += szName;
	if(!pItem->inheritsClass().isEmpty())
	{
		szBuffer += "\",\"";
		szBuffer += pItem->inheritsClass();
	}
	szBuffer += "\")\n{\n";
	for(int i = 0; i < pItem->childCount(); i++)
	{
		ClassEditorTreeWidgetItem * pFunction = (ClassEditorTreeWidgetItem *)pItem->child(i);
		if(pFunction->isMethod())
		{
			szBuffer += "\t";
			if(pFunction->isInternalFunction())
				szBuffer += "internal ";
			szBuffer += "function ";
			szBuffer += pFunction->name();
			szBuffer += "(" + pFunction->reminder() + ")\n";
			QString szCode = pFunction->buffer();
			KviCommandFormatter::blockFromBuffer(szCode);
			KviCommandFormatter::indent(szCode);

			szBuffer += szCode;
		}
	}
	szBuffer += "}\n";
}

void ClassEditorWidget::exportAll()
{
	exportClasses(false);
}

void ClassEditorWidget::exportSelectedSepFiles()
{
	exportClasses(true, true);
}

void ClassEditorWidget::exportSelected()
{
	exportClasses(true);
}

void ClassEditorWidget::exportSelectionInSinglesFiles(KviPointerList<ClassEditorTreeWidgetItem> * pList)
{
	bool bReplaceAll = false;

	if(!m_szDir.endsWith(QString(KVI_PATH_SEPARATOR)))
		m_szDir += KVI_PATH_SEPARATOR;
	if(!pList->first())
	{
		g_pClassEditorModule->lock();
		QMessageBox::warning(this, __tr2qs_ctx("Warning While Exporting - KVIrc", "editor"), __tr2qs_ctx("Must select an entry from the list to export!", "editor"), __tr2qs_ctx("OK", "editor"));
		g_pClassEditorModule->unlock();
		return;
	}
	g_pClassEditorModule->lock();

	if(!KviFileDialog::askForDirectoryName(
	       m_szDir,
	       __tr2qs_ctx("Choose a Directory - KVIrc", "editor"),
	       m_szDir,
	       QString(),
	       false,
	       true,
	       this))
	{
		g_pClassEditorModule->unlock();
		return;
	}

	if(!m_szDir.endsWith(QString(KVI_PATH_SEPARATOR)))
		m_szDir += KVI_PATH_SEPARATOR;

	for(ClassEditorTreeWidgetItem * pItem = pList->first(); pItem; pItem = pList->next())
	{
		QString szTmp;
		exportClassBuffer(szTmp, pItem);
		QString szFileName = buildFullClassName(pItem);
		szFileName += ".kvs";
		szFileName.replace("::", "_");
		QString szCompletePath = m_szDir + szFileName;
		if(KviFileUtils::fileExists(szCompletePath) && !bReplaceAll)
		{
			QString szMsg = __tr2qs_ctx("The file \"%1\" exists. Do you want to replace it?", "editor").arg(szFileName);
			int iRet = QMessageBox::question(this, __tr2qs_ctx("Confirm Replacing File - KVIrc", "editor"), szMsg, __tr2qs_ctx("Yes", "editor"), __tr2qs_ctx("Yes to All", "editor"), __tr2qs_ctx("No", "editor"));
			if(iRet != 2)
			{
				KviFileUtils::writeFile(szCompletePath, szTmp);
				if(iRet == 1)
					bReplaceAll = true;
			}
		}
		else
			KviFileUtils::writeFile(szCompletePath, szTmp);
	}
	g_pClassEditorModule->unlock();
}

void ClassEditorWidget::exportClasses(bool bSelectedOnly, bool bSingleFiles)
{
	QString szOut;
	QString szNameFile;
	QString szFile;
	int iCount = 0;

	saveLastEditedItem();

	KviPointerList<ClassEditorTreeWidgetItem> list;
	list.setAutoDelete(false);

	if(bSelectedOnly)
		appendSelectedClassItems(&list);
	else
		appendAllClassItems(&list);

	if(bSingleFiles)
	{
		exportSelectionInSinglesFiles(&list);
		return;
	}

	ClassEditorTreeWidgetItem * pTempItem = nullptr;
	KviPointerList<ClassEditorTreeWidgetItem> skiplist;
	skiplist.setAutoDelete(false);
	for(ClassEditorTreeWidgetItem * pItem = list.first(); pItem; pItem = list.next())
	{
		pTempItem = pItem;
		if(skiplist.findRef(pItem) != -1)
			continue;
		ClassEditorTreeWidgetItem * pParentClass = m_pClasses->find(pItem->inheritsClass());
		KviPointerList<ClassEditorTreeWidgetItem> linkedClasses;
		linkedClasses.setAutoDelete(false);
		linkedClasses.append(pItem);
		while(pParentClass)
		{
			if(skiplist.findRef(pParentClass) != -1)
				break;
			linkedClasses.append(pParentClass);
			pParentClass = m_pClasses->find(pParentClass->inheritsClass());
		}
		for(int i = linkedClasses.count() - 1; i >= 0; i--)
		{
			iCount++;
			QString szTmp;
			exportClassBuffer(szTmp, linkedClasses.at(i));
			skiplist.append(linkedClasses.at(i));
			szOut += szTmp;
			szOut += "\n";
		}
	}

	if(szOut.isEmpty())
	{
		g_pClassEditorModule->lock();
		QMessageBox::warning(this, __tr2qs_ctx("Warning While Exporting - KVIrc", "editor"), __tr2qs_ctx("The exported class file could be empty: cowardly refusing to write it", "editor"), __tr2qs_ctx("OK", "editor"));
		g_pClassEditorModule->unlock();
		return;
	}

	QString szName = m_szDir;

	if(!szName.endsWith(QString(KVI_PATH_SEPARATOR)))
		szName += KVI_PATH_SEPARATOR;

	g_pClassEditorModule->lock();

	if(iCount != 1)
		szNameFile = "classes";
	else
	{
		QString szTmp = buildFullClassName(pTempItem);
		szNameFile = szTmp.replace("::", "_");
	}

	szName += szNameFile;
	szName += ".kvs";
	if(!KviFileDialog::askForSaveFileName(
	       szFile,
	       __tr2qs_ctx("Choose a Filename - KVIrc", "editor"),
	       szName,
	       KVI_FILTER_SCRIPT,
	       false,
	       true,
	       true,
	       this))
	{
		g_pClassEditorModule->unlock();
		return;
	}
	m_szDir = QFileInfo(szFile).absolutePath();
	g_pClassEditorModule->unlock();

	if(!KviFileUtils::writeFile(szFile, szOut))
	{
		g_pClassEditorModule->lock();
		QMessageBox::warning(this, __tr2qs_ctx("Write to Classes File Failed - KVIrc", "editor"), __tr2qs_ctx("Unable to write to the classes file.", "editor"), __tr2qs_ctx("OK", "editor"));
		g_pClassEditorModule->unlock();
	}
}

void ClassEditorWidget::saveProperties(KviConfigurationFile * pCfg)
{
	pCfg->writeEntry("Sizes", m_pSplitter->sizes());
	QString szName;
	if(m_pLastEditedItem)
		szName = buildFullClassName(m_pLastEditedItem);
	pCfg->writeEntry("LastClass", szName);
}

void ClassEditorWidget::loadProperties(KviConfigurationFile * pCfg)
{
	QList<int> def;
	def.append(20);
	def.append(80);
	m_pSplitter->setSizes(pCfg->readIntListEntry("Sizes", def));
	QString szTmp = pCfg->readEntry("LastClass", QString());

	ClassEditorTreeWidgetItem * pItem = findItem(szTmp);
	activateItem(pItem);
}

void ClassEditorWidget::appendSelectedClassItems(KviPointerList<ClassEditorTreeWidgetItem> * pList)
{
	QList<QTreeWidgetItem *> list = m_pTreeWidget->selectedItems();
	for(int i = 0; i < list.count(); i++)
	{
		if(((ClassEditorTreeWidgetItem *)list.at(i))->isClass())
			pList->append((ClassEditorTreeWidgetItem *)list.at(i));
		else
			appendSelectedClassItemsRecursive(pList, list.at(i));
	}
}

void ClassEditorWidget::appendSelectedClassItemsRecursive(KviPointerList<ClassEditorTreeWidgetItem> * pList, QTreeWidgetItem * pStartFrom)
{
	for(int i = 0; i < pStartFrom->childCount(); i++)
	{
		if(((ClassEditorTreeWidgetItem *)pStartFrom->child(i))->isClass())
			pList->append(((ClassEditorTreeWidgetItem *)pStartFrom->child(i)));
		else
			appendSelectedClassItemsRecursive(pList, pStartFrom->child(i));
	}
}

void ClassEditorWidget::appendAllClassItems(KviPointerList<ClassEditorTreeWidgetItem> * pList)
{
	KviPointerHashTableIterator<QString, ClassEditorTreeWidgetItem> it(*m_pClasses);
	while(it.current())
	{
		pList->append(it.current());
		++it;
	}
}

void ClassEditorWidget::appendAllClassItemsRecursive(KviPointerList<ClassEditorTreeWidgetItem> * pList, QTreeWidgetItem * pStartFrom)
{
	for(int i = 0; i < pStartFrom->childCount(); i++)
	{
		if(((ClassEditorTreeWidgetItem *)pStartFrom->child(i))->isClass())
		{
			pList->append((ClassEditorTreeWidgetItem *)pStartFrom->child(i));
		}
		else
		{
			appendAllClassItemsRecursive(pList, pStartFrom->child(i));
		}
	}
}

void ClassEditorWidget::removeItemChildren(ClassEditorTreeWidgetItem * pItem, KviPointerList<ClassEditorTreeWidgetItem> & lRemovedItems)
{
	if(pItem->isClass())
	{
		KviPointerList<ClassEditorTreeWidgetItem> lInheritedClasses;
		lInheritedClasses.setAutoDelete(false);
		searchInheritedClasses(buildFullClassName(pItem), lInheritedClasses);
		for(unsigned int u = 0; u < lInheritedClasses.count(); u++)
		{
			lInheritedClasses.at(u)->setClassNotBuilt(true);
			lInheritedClasses.at(u)->setExpanded(true);
			lInheritedClasses.at(u)->setInheritsClass("object");
		}
	}

	while(pItem->childCount() > 0)
	{
		ClassEditorTreeWidgetItem * pChild = (ClassEditorTreeWidgetItem *)(pItem->child(0));
		if(pChild->childCount())
			removeItemChildren(pChild, lRemovedItems);
		if(pChild->isClass())
		{
			m_pClasses->removeRef(pChild);
			KviKvsObjectClass * pClass = KviKvsKernel::instance()->objectController()->lookupClass(buildFullClassName(pChild));
			if(pClass)
				KviKvsKernel::instance()->objectController()->deleteClass(pClass);
			qDebug("removing class %s %p", buildFullClassName(pChild).toUtf8().data(), pClass);
		}
		pItem->removeChild(pChild);
		lRemovedItems.append(pItem);
		delete pChild;
	}
}

bool ClassEditorWidget::removeItem(ClassEditorTreeWidgetItem * pItem, KviPointerList<ClassEditorTreeWidgetItem> & lRemovedItems, bool * pbYesToAll)
{
	if(!pItem)
		return true;
	QString szMsg;
	QString szName = pItem->name();

	if(!*pbYesToAll)
	{
		saveLastEditedItem();
		if(pItem->isClass())
		{
			szMsg = QString(__tr2qs_ctx("Do you really want to remove the class \"%1\"?", "editor")).arg(szName);
		}
		else if(pItem->isNamespace())
		{
			szMsg = QString(__tr2qs_ctx("Do you really want to remove the namespace \"%1\"?", "editor")).arg(szName);
			szMsg += "<br>";
			szMsg += __tr2qs_ctx("Please note that all the child classes/functions will also be deleted.", "editor");
		}
		else if(pItem->isMethod())
		{
			szMsg = QString(__tr2qs_ctx("Do you really want to remove the function \"%1\"?", "editor")).arg(szName);
		}

		g_pClassEditorModule->lock();
		int iRet = QMessageBox::question(this, __tr2qs_ctx("Confirm Removing Item - KVIrc", "editor"), szMsg, __tr2qs_ctx("Yes", "editor"), __tr2qs_ctx("Yes to All", "editor"), __tr2qs_ctx("No", "editor"));
		g_pClassEditorModule->unlock();
		switch(iRet)
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

	if(pItem == m_pLastEditedItem)
		m_pLastEditedItem = nullptr;
	if(pItem == m_pLastClickedItem)
		m_pLastClickedItem = nullptr;
	if(pItem->childCount())
		removeItemChildren(pItem, lRemovedItems);
	if(pItem->isClass())
	{
		m_pClasses->removeRef(pItem);
		KviKvsObjectClass * pClass = KviKvsKernel::instance()->objectController()->lookupClass(buildFullClassName(pItem));
		qDebug("removing class %s %p", buildFullClassName(pItem).toUtf8().data(), pClass);
		if(pClass)
			KviKvsKernel::instance()->objectController()->deleteClass(pClass);
		else
		{
			QString szFileName = buildFullClassName(pItem);
			szFileName.replace("::", "--");
			szFileName.append(KVI_FILEEXTENSION_SCRIPT);
			QString szPath;
			g_pApp->getLocalKvircDirectory(szPath, KviApplication::Classes);
			QDir d(szPath);
			if(d.exists(szFileName))
			{
				qDebug("Removing file %s from disk", szFileName.toUtf8().data());
				d.remove(szFileName);
			}
		}
	}
	if(pItem->isMethod())
	{
		updateClassHierarchy((ClassEditorTreeWidgetItem *)pItem->parent());
	}
	lRemovedItems.append(pItem);
	delete pItem;
	return true;
}

void ClassEditorWidget::updateClassHierarchy(ClassEditorTreeWidgetItem * pClass)
{
	pClass->setClassNotBuilt(true);
	KviPointerList<ClassEditorTreeWidgetItem> lInheritedClasses;
	lInheritedClasses.setAutoDelete(false);
	searchInheritedClasses(pClass->name(), lInheritedClasses);
	for(unsigned int u = 0; u < lInheritedClasses.count(); u++)
	{
		lInheritedClasses.at(u)->setClassNotBuilt(true);
		lInheritedClasses.at(u)->setInheritsClass(pClass->name());
		lInheritedClasses.at(u)->setExpanded(true);
	}
}

void ClassEditorWidget::removeSelectedItems()
{
	KviPointerList<ClassEditorTreeWidgetItem> lRemovedItems;
	lRemovedItems.setAutoDelete(false);
	QList<QTreeWidgetItem *> list = m_pTreeWidget->selectedItems();
	bool bYesToAll = false;
	for(int i = 0; i < list.count(); i++)
	{
		if(lRemovedItems.findRef((ClassEditorTreeWidgetItem *)list.at(i)) != -1)
			continue;
		if(!removeItem((ClassEditorTreeWidgetItem *)list.at(i), lRemovedItems, &bYesToAll))
			return;
	}
}

bool ClassEditorWidget::askForClassName(QString & szClassName, QString & szInheritsClassName, bool bEdit)
{
	KviClassEditorDialog * pDialog = new KviClassEditorDialog(this, "classdialog", m_pClasses, szClassName, szInheritsClassName, bEdit);
	szClassName = "";
	g_pClassEditorModule->lock();
	bool bOk = pDialog->exec();
	g_pClassEditorModule->unlock();
	if(bOk)
	{
		szClassName = pDialog->className();
		szInheritsClassName = pDialog->inheritsClassName();
		delete pDialog;
		return true;
	}
	delete pDialog;
	return false;
}

bool ClassEditorWidget::askForFunction(QString & szFunctionName, QString & szReminder, bool * bInternal, const QString & szClassName, bool bRenameMode)
{
	KviClassEditorFunctionDialog * pDialog = new KviClassEditorFunctionDialog(this, "function", szClassName, szFunctionName, szReminder, *bInternal, bRenameMode);
	szFunctionName = "";
	g_pClassEditorModule->lock();
	bool bOk = pDialog->exec();
	g_pClassEditorModule->unlock();
	if(bOk)
	{
		szFunctionName = pDialog->functionName();
		szReminder = pDialog->reminder();
		*bInternal = pDialog->isInternalFunction();
		delete pDialog;
		return true;
	}
	delete pDialog;
	return false;
}

bool ClassEditorWidget::askForNamespaceName(const QString & szAction, const QString & szText, const QString & szInitialText, QString & szNewName)
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
		if(!bOk)
			return false;
		if(szNewName.isEmpty())
		{
			g_pClassEditorModule->lock();
			QMessageBox::warning(this,
			    __tr2qs_ctx("Invalid or Missing Name - KVIrc", "editor"),
			    __tr2qs_ctx("You must specify a valid name for the namespace.", "editor"),
			    __tr2qs_ctx("OK, Let me try again...", "editor"));
			g_pClassEditorModule->unlock();
			continue;
		}

		// we allow only [\w:]+
		QRegExp re("[\\w:]+");
		if(!re.exactMatch(szNewName))
		{
			g_pClassEditorModule->lock();
			QMessageBox::information(this,
			    __tr2qs_ctx("Invalid Name - KVIrc", "editor"),
			    __tr2qs_ctx("Namespace names can contain only letters, digits, underscores and '::' namespace separators.", "editor"),
			    __tr2qs_ctx("OK, Let me try again...", "editor"));
			g_pClassEditorModule->unlock();
			szNewName = "";
			continue;
		}
		// make sure that we have only doubled "::" and not ":" or ":::..."
		QString szTmp = szNewName;
		szTmp.replace("::", "@"); // @ is not allowed by the rule above
		if(szTmp.indexOf(":", Qt::CaseInsensitive) != -1)
		{
			g_pClassEditorModule->lock();
			QMessageBox::information(this,
			    __tr2qs_ctx("Invalid Name - KVIrc", "editor"),
			    __tr2qs_ctx("Stray ':' character in namespace name: did you mean ...<namespace>::<name>?", "editor"),
			    __tr2qs_ctx("OK, Let me try again...", "editor"));
			g_pClassEditorModule->unlock();
			szNewName = "";
			continue;
		}
		if(szTmp.indexOf("@@", Qt::CaseInsensitive) != -1)
		{
			g_pClassEditorModule->lock();
			QMessageBox::information(this,
			    __tr2qs_ctx("Invalid Name - KVIrc", "editor"),
			    __tr2qs_ctx("Found an empty namespace in namespace name.", "editor"),
			    __tr2qs_ctx("OK, Let me try again...", "editor"));
			g_pClassEditorModule->unlock();
			szNewName = "";
			continue;
		}
	}
	return true;
}

void ClassEditorWidget::openParentItems(QTreeWidgetItem * pItem)
{
	if(pItem->parent())
	{
		pItem->parent()->setExpanded(true);
		openParentItems(pItem->parent());
	}
}

void ClassEditorWidget::activateItem(QTreeWidgetItem * pItem)
{
	if(!pItem)
		return;
	openParentItems(pItem);
	m_pTreeWidget->setCurrentItem(pItem);
}

void ClassEditorWidget::newClass()
{
	QString szClassName;
	QString szinheritsClassName;
	askForClassName(szClassName, szinheritsClassName, false);
	if(szClassName.isEmpty())
		return;
	ClassEditorTreeWidgetItem * pItem = newItem(szClassName, ClassEditorTreeWidgetItem::Class);

	KviQString::escapeKvs(&szClassName, KviQString::EscapeSpace);
	KviQString::escapeKvs(&szinheritsClassName, KviQString::EscapeSpace);

	QString szClass = "class(";
	szClass += szClassName + "," + szinheritsClassName;
	szClass += "){}\n";
	pItem->setInheritsClass(szinheritsClassName);
	activateItem(pItem);
	m_pClasses->insert(szClassName, pItem);
	KviKvsScript::run(szClass, g_pActiveWindow);
}

void ClassEditorWidget::newNamespace()
{
	QString szName;
	if(!askForNamespaceName(__tr2qs_ctx("Enter a Filename - KVIrc", "editor"), __tr2qs_ctx("Please enter the name for the new namespace.", "editor"), "mynamespace", szName))
		return;
	if(szName.isEmpty())
		return;
	ClassEditorTreeWidgetItem * pItem = newItem(szName, ClassEditorTreeWidgetItem::Namespace);
	activateItem(pItem);
}

void ClassEditorWidget::newMemberFunction()
{
	QString szFunctionName;
	QString szClassName;
	QString szReminder;

	if(m_pLastClickedItem->isMethod())
		m_pLastClickedItem = (ClassEditorTreeWidgetItem *)m_pLastClickedItem->parent();
	szClassName = buildFullClassName(m_pLastClickedItem);
	bool bInternal = false;
	if(!askForFunction(szFunctionName, szReminder, &bInternal, szClassName, false))
		return;
	if(szFunctionName.isEmpty())
		return;

	ClassEditorTreeWidgetItem * pItem = newItem(szFunctionName, ClassEditorTreeWidgetItem::Method);
	pItem->setInternalFunction(bInternal);
	if(!szReminder.isEmpty())
		pItem->setReminder(szReminder);
	activateItem(pItem);
	((ClassEditorTreeWidgetItem *)pItem->parent())->setClassNotBuilt(true);
}

ClassEditorTreeWidgetItem * ClassEditorWidget::newItem(QString & szName, ClassEditorTreeWidgetItem::Type eType)
{
	if(m_pLastClickedItem)
		buildFullItemPath(m_pLastClickedItem, szName);

	QString szTmp;
	if(findItem(szName))
		szName.append("1");

	int iIdx = 2;
	while(findItem(szName))
	{
		szTmp.setNum(iIdx);
		szName.chop(szTmp.length());
		szName.append(szTmp);
		iIdx++;
	}
	ClassEditorTreeWidgetItem * pItem;
	pItem = createFullItem(szName);
	pItem->setType(eType);
	return pItem;
}

void ClassEditorWidget::build()
{
	saveLastEditedItem();
	KviPointerHashTableIterator<QString, ClassEditorTreeWidgetItem> it(*m_pClasses);
	KviPointerList<ClassEditorTreeWidgetItem> linkedClasses;
	linkedClasses.setAutoDelete(false);
	KviPointerList<ClassEditorTreeWidgetItem> skipClasses;
	skipClasses.setAutoDelete(false);
	bool bErrorWhilecompiling = false;
	while(it.current())
	{
		ClassEditorTreeWidgetItem * pClass = it.current();
		if(skipClasses.findRef(it.current()) != -1)
		{
			++it;
			continue;
		}
		if(pClass->classNotBuilt())
		{
			//qDebug("compiling %s",pClass->name().toUtf8().data());
			ClassEditorTreeWidgetItem * pParentClass = m_pClasses->find(pClass->inheritsClass());

			linkedClasses.append(pClass);
			//if (!pParentClass) qDebug("no parent class");
			while(pParentClass)
			{
				if(pParentClass->classNotBuilt())
					linkedClasses.append(pParentClass);
				pParentClass = m_pClasses->find(pParentClass->inheritsClass());
			}
			for(int i = linkedClasses.count() - 1; i >= 0; i--)
			{
				QString szFullClassName = buildFullClassName(linkedClasses.at(i));
				KviKvsObjectClass * pClass = KviKvsKernel::instance()->objectController()->lookupClass(szFullClassName);
				if(pClass)
					KviKvsKernel::instance()->objectController()->deleteClass(pClass);
				QString szClass;
				exportClassBuffer(szClass, linkedClasses.at(i));
				KviKvsScript::run(szClass, g_pActiveWindow);
				pClass = KviKvsKernel::instance()->objectController()->lookupClass(szFullClassName);
				if(!pClass)
				{
					bErrorWhilecompiling = true;
					QString szError = __tr2qs_ctx("Unable to compile class: ", "editor");
					szError += szFullClassName + "\n";
					KviPointerList<ClassEditorTreeWidgetItem> lInheritedClasses;
					lInheritedClasses.setAutoDelete(false);
					searchInheritedClasses(szFullClassName, lInheritedClasses);
					if(lInheritedClasses.count())
					{
						szError += __tr2qs_ctx("These inherited classes will be not compiled too:", "editor");
						szError += "\n";
						for(unsigned int u = 0; u < lInheritedClasses.count(); u++)
						{
							szError += buildFullClassName(lInheritedClasses.at(u)) + "\n";
							lInheritedClasses.at(u)->setClassNotBuilt(true);
							skipClasses.append(lInheritedClasses.at(u));
						}
					}
					QMessageBox::critical(this, __tr2qs_ctx("Compilation Error - KVIrc", "editor"), szError,
					    QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
					break;
				}
				//else qDebug("class compiled %s :\n",szClass.toUtf8().data());
				linkedClasses.at(i)->setClassNotBuilt(false);
				m_pEditor->setModified(false);
			}
		}
		++it;
	}
	if(bErrorWhilecompiling)
		saveNotBuiltClasses();
	else
	{
		QString szFileName = "libkviclasseditortmp.kvc";
		QString szBuffer;
		g_pApp->getLocalKvircDirectory(szBuffer, KviApplication::ConfigPlugins, szFileName);
		KviConfigurationFile cfg(szBuffer, KviConfigurationFile::Write);
		cfg.clear();
		cfg.sync();
	}
	KviKvsKernel::instance()->objectController()->flushUserClasses();
}

void ClassEditorWidget::loadNotBuiltClasses()
{
	QString szFileName = "libkviclasseditortmp.kvc";
	QString szBuffer;
	g_pApp->getLocalKvircDirectory(szBuffer, KviApplication::ConfigPlugins, szFileName);
	KviConfigurationFile cfg(szBuffer, KviConfigurationFile::Read);
	KviConfigurationFileIterator it(*(cfg.dict()));

	KviPointerList<QString> l;
	l.setAutoDelete(true);

	while(it.current())
	{
		l.append(new QString(it.currentKey()));
		++it;
	}

	for(QString * pszTmp = l.first(); pszTmp; pszTmp = l.next())
	{
		cfg.setGroup(*pszTmp);
		ClassEditorTreeWidgetItem * pClassItem = createFullItem(*pszTmp);
		m_pClasses->insert(*pszTmp, pClassItem);
		pClassItem->setClassNotBuilt(true);
		KviConfigurationFileGroup * pDict = cfg.dict()->find(*pszTmp);
		if(pDict)
		{
			KviConfigurationFileGroupIterator it(*pDict);
			KviPointerList<QString> names;
			names.setAutoDelete(true);
			while(it.current())
			{
				names.append(new QString(it.currentKey()));
				++it;
			}
			for(QString * pszTmp = names.first(); pszTmp; pszTmp = names.next())
			{

				if(KviQString::equalCI(*pszTmp, "@Inherits"))
				{
					pClassItem->setInheritsClass(cfg.readEntry(*pszTmp, ""));
					continue;
				}
				if((*pszTmp).left(9) == "@Reminder")
					continue;
				QString szCode = cfg.readEntry(*pszTmp, "");
				ClassEditorTreeWidgetItem * pFunctionItem = findFunction(*pszTmp, pClassItem);
				if(!pFunctionItem)
					pFunctionItem = new ClassEditorTreeWidgetItem(pClassItem, ClassEditorTreeWidgetItem::Method, *pszTmp);
				pFunctionItem->setBuffer(szCode);
				QString szEntry = "@Reminder|" + (*pszTmp);
				QString szReminder = cfg.readEntry(szEntry, "");
				pFunctionItem->setReminder(szReminder);
			}
		}
	}
}

void ClassEditorWidget::saveNotBuiltClasses()
{
	saveLastEditedItem();
	KviPointerHashTableIterator<QString, ClassEditorTreeWidgetItem> it(*m_pClasses);
	QString szFileName = "libkviclasseditortmp.kvc";
	QString szBuffer;
	g_pApp->getLocalKvircDirectory(szBuffer, KviApplication::ConfigPlugins, szFileName);
	KviConfigurationFile cfg(szBuffer, KviConfigurationFile::Write);
	cfg.clear();

	while(it.current())
	{
		if(it.current()->classNotBuilt())
		{
			KviKvsObjectClass * pClass = KviKvsKernel::instance()->objectController()->lookupClass(it.currentKey());
			if(pClass)
				KviKvsKernel::instance()->objectController()->deleteClass(pClass);

			cfg.setGroup(it.currentKey());
			cfg.writeEntry("@Inherits", it.current()->inheritsClass());
			QString szReminderEntry;
			for(int i = 0; i < it.current()->childCount(); i++)
			{
				if(!((ClassEditorTreeWidgetItem *)it.current()->child(i))->reminder().isEmpty())
				{
					szReminderEntry = "@Reminder|" + ((ClassEditorTreeWidgetItem *)it.current()->child(i))->name();
					cfg.writeEntry(szReminderEntry, ((ClassEditorTreeWidgetItem *)it.current()->child(i))->reminder());
				}
				cfg.writeEntry(((ClassEditorTreeWidgetItem *)it.current()->child(i))->name(), ((ClassEditorTreeWidgetItem *)it.current()->child(i))->buffer());
			}
		}
		++it;
	}
	cfg.sync();
}

void ClassEditorWidget::searchInheritedClasses(const QString szClass, KviPointerList<ClassEditorTreeWidgetItem> & lInheritedClasses)
{
	KviPointerHashTableIterator<QString, ClassEditorTreeWidgetItem> it(*m_pClasses);
	while(it.current())
	{
		if(KviQString::equalCI(szClass, it.current()->inheritsClass()))
			lInheritedClasses.append(it.current());
		++it;
	}
}

ClassEditorWindow::ClassEditorWindow()
    : KviWindow(KviWindow::ScriptEditor, "classeditor", nullptr)
{
	g_pClassEditorWindow = this;

	setFixedCaption(__tr2qs_ctx("Class Editor", "editor"));

	QGridLayout * pLayout = new QGridLayout();

	m_pEditor = new ClassEditorWidget(this);
	pLayout->addWidget(m_pEditor, 0, 0, 1, 4);

	QPushButton * pBtn = new QPushButton(__tr2qs_ctx("&Build", "editor"), this);
	pBtn->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Accept)));
	pLayout->addWidget(pBtn, 1, 1);
	connect(pBtn, SIGNAL(clicked()), this, SLOT(buildClicked()));

	pBtn = new QPushButton(__tr2qs_ctx("&Save", "editor"), this);
	pBtn->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Accept)));
	pLayout->addWidget(pBtn, 1, 2);
	connect(pBtn, SIGNAL(clicked()), this, SLOT(saveClicked()));

	pBtn = new QPushButton(__tr2qs_ctx("Close", "editor"), this);
	pBtn->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Discard)));
	pLayout->addWidget(pBtn, 1, 3);
	connect(pBtn, SIGNAL(clicked()), this, SLOT(cancelClicked()));

	pLayout->setRowStretch(0, 1);
	pLayout->setColumnStretch(0, 1);
	setLayout(pLayout);
}

ClassEditorWindow::~ClassEditorWindow()
{
	g_pClassEditorWindow = nullptr;
}

void ClassEditorWindow::buildClicked()
{
	m_pEditor->build();
}

void ClassEditorWindow::saveClicked()
{
	m_pEditor->saveNotBuiltClasses();
}

void ClassEditorWindow::cancelClicked()
{
	close();
}

QPixmap * ClassEditorWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KviIconManager::ClassEditor);
}

void ClassEditorWindow::configGroupName(QString & szName)
{
	szName = "classeditor";
}

void ClassEditorWindow::saveProperties(KviConfigurationFile * pCfg)
{
	m_pEditor->saveProperties(pCfg);
}

void ClassEditorWindow::loadProperties(KviConfigurationFile * pCfg)
{
	m_pEditor->loadProperties(pCfg);
}

KviClassEditorDialog::KviClassEditorDialog(QWidget * pParent, const QString & szName, KviPointerHashTable<QString, ClassEditorTreeWidgetItem> * pClasses, const QString & szClassName, const QString & szInheritsClassName, bool bRenameMode)
    : QDialog(pParent)
{
	setObjectName(szName);

	QGridLayout * pLayout = new QGridLayout(this);

	KviTalHBox * pHBox = new KviTalHBox(this);
	pHBox->setSpacing(0);
	pHBox->setMargin(0);
	pLayout->addWidget(pHBox, 0, 0);

	QLabel * pLabel = new QLabel(pHBox);
	pLabel->setObjectName("classnamelabel");
	pLabel->setText(__tr2qs_ctx("Please enter the name for the class:", "editor"));

	m_pClassNameLineEdit = new QLineEdit(pHBox);
	m_pClassNameLineEdit->setObjectName("classnameineedit");
	m_pClassNameLineEdit->setText(szClassName);
	pLabel->setBuddy(m_pClassNameLineEdit);
	QRegExp re;
	if(!bRenameMode)
	{
		// we allow only [\w:]+ class name
		re.setPattern("[\\w]+");
		m_pClassNameLineEdit->setToolTip(__tr2qs_ctx("Class names can contain only letters, digits and underscores", "editor"));
	}
	else
	{
		// in editor mode we allow [\w:]+ and namespace separator
		re.setPattern("[\\w]+(::[\\w]+)+");
		m_pClassNameLineEdit->setToolTip(__tr2qs_ctx("In rename mode class names can contain only letters, digits and underscores and namespaces :: separator", "editor"));
	}
	QRegExpValidator * pValidator = new QRegExpValidator(re, this);
	m_pClassNameLineEdit->setValidator(pValidator);
	m_pClassNameLineEdit->setObjectName("functionameineedit");

	pHBox = new KviTalHBox(this);
	pHBox->setSpacing(0);
	pHBox->setMargin(0);
	pLayout->addWidget(pHBox, 1, 0);

	pLabel = new QLabel(pHBox);
	pLabel->setObjectName("Inheritsclasslabel");
	pLabel->setText(__tr2qs_ctx("Inherits class:", "editor"));

	m_pInheritsClassComboBox = new QComboBox(pHBox);
	pLabel->setBuddy(m_pInheritsClassComboBox);

	KviPointerHashTableIterator<QString, ClassEditorTreeWidgetItem> pClassesIt(*pClasses);

	QStringList szClasses;
	while(pClassesIt.current())
	{
		if(!KviQString::equalCI(pClassesIt.currentKey(), szClassName))
			szClasses.append(pClassesIt.currentKey());
		++pClassesIt;
	}
	KviPointerHashTableIterator<QString, KviKvsObjectClass> pBuiltinClasses(*KviKvsKernel::instance()->objectController()->classDict());
	while(pBuiltinClasses.current())
	{
		if(pBuiltinClasses.current()->isBuiltin())
			szClasses.append(pBuiltinClasses.currentKey());
		++pBuiltinClasses;
	}

	int iCurrentIdx;
	szClasses.sort();
	for(int i = 0; i < szClasses.count(); i++)
		m_pInheritsClassComboBox->addItem(szClasses.at(i));
	if(szInheritsClassName.isEmpty())
		iCurrentIdx = m_pInheritsClassComboBox->findText("object");
	else
	{
		iCurrentIdx = m_pInheritsClassComboBox->findText(szInheritsClassName);
		if(iCurrentIdx == -1)
			iCurrentIdx = m_pInheritsClassComboBox->findText("object");
	}
	m_pInheritsClassComboBox->setCurrentIndex(iCurrentIdx);
	m_pClassNameLineEdit->setFocus();
	connect(m_pClassNameLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));

	pHBox = new KviTalHBox(this);
	pHBox->setSpacing(0);
	pHBox->setMargin(0);
	pLayout->addWidget(pHBox, 2, 0);

	m_pNewClassButton = new QPushButton(pHBox);
	m_pNewClassButton->setObjectName("newclassbutton");
	if(bRenameMode)
		m_pNewClassButton->setText(__tr2qs_ctx("&Rename Class", "editor"));
	else
	{
		m_pNewClassButton->setText(__tr2qs_ctx("&Create Class", "editor"));
		m_pNewClassButton->setEnabled(false);
	}
	connect(m_pNewClassButton, SIGNAL(clicked()), this, SLOT(accept()));

	QPushButton * pCancelButton = new QPushButton(pHBox);
	pCancelButton->setObjectName("cancelButton");
	pCancelButton->setText(__tr2qs_ctx("&Cancel", "editor"));
	connect(pCancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	setLayout(pLayout);
}

KviClassEditorDialog::~KviClassEditorDialog()
    = default;

void KviClassEditorDialog::textChanged(const QString & szText)
{
	m_pNewClassButton->setEnabled(!szText.isEmpty());
}

KviClassEditorFunctionDialog::KviClassEditorFunctionDialog(QWidget * pParent, const QString & szName, const QString & szClassName, const QString & szFunctionName, const QString & szReminder, bool bIsInternal, bool bRenameMode)
    : QDialog(pParent)
{
	setObjectName(szName);

	QGridLayout * pLayout = new QGridLayout(this);

	KviTalHBox * pHBox = new KviTalHBox(this);
	pHBox->setSpacing(0);
	pHBox->setMargin(0);
	pLayout->addWidget(pHBox, 0, 0);

	QLabel * pLabel = new QLabel(pHBox);
	pLabel->setObjectName("classnamelabel");
	pLabel->setText("Class: <b>" + szClassName + "</b>");

	pHBox = new KviTalHBox(this);
	pHBox->setSpacing(0);
	pHBox->setMargin(0);
	pLayout->addWidget(pHBox, 1, 0);

	pLabel = new QLabel(pHBox);
	pLabel->setObjectName("functionnamelabel");
	pLabel->setText(__tr2qs_ctx("Please enter the name for the member function:", "editor"));

	m_pFunctionNameLineEdit = new QLineEdit(pHBox);

	// we allow only [\w:]+ function name
	QRegExp re("[\\w]+");
	QRegExpValidator * pValidator = new QRegExpValidator(re, this);
	m_pFunctionNameLineEdit->setValidator(pValidator);
	m_pFunctionNameLineEdit->setObjectName("functionameineedit");
	m_pFunctionNameLineEdit->setToolTip(__tr2qs_ctx("Function names can contain only letters, digits and underscores", "editor"));
	m_pFunctionNameLineEdit->setText(szFunctionName);
	pLabel->setBuddy(m_pFunctionNameLineEdit);

	pHBox = new KviTalHBox(this);
	pHBox->setSpacing(0);
	pHBox->setMargin(0);
	pLayout->addWidget(pHBox, 2, 0);

	pLabel = new QLabel(pHBox);
	pLabel->setObjectName("reminderlabel");
	pLabel->setWordWrap(true);
	pLabel->setText(__tr2qs_ctx("Please enter the optional reminder string for the member function:", "editor"));

	m_pReminderLineEdit = new QLineEdit(pHBox);
	m_pReminderLineEdit->setText(szReminder);
	pLabel->setBuddy(m_pReminderLineEdit);

	pHBox = new KviTalHBox(this);
	pHBox->setSpacing(0);
	pHBox->setMargin(0);
	pLayout->addWidget(pHBox, 3, 0);

	pLabel = new QLabel(pHBox);
	pLabel->setObjectName("functionnamelabel");
	pLabel->setText(__tr2qs_ctx("Set as <b>Internal</b> Function: ", "editor"));

	m_pInternalCheckBox = new QCheckBox(pHBox);
	m_pInternalCheckBox->setChecked(bIsInternal);
	m_pFunctionNameLineEdit->setFocus();
	pLabel->setBuddy(m_pInternalCheckBox);
	connect(m_pFunctionNameLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));

	pHBox->setAlignment(m_pInternalCheckBox, Qt::AlignLeft);
	pHBox->setStretchFactor(m_pInternalCheckBox, 70);
	pHBox->setStretchFactor(pLabel, 30);

	pHBox = new KviTalHBox(this);
	pHBox->setSpacing(0);
	pHBox->setMargin(0);
	pLayout->addWidget(pHBox, 4, 0);

	m_pNewFunctionButton = new QPushButton(pHBox);
	m_pNewFunctionButton->setObjectName("newfunctionbutton");
	if(!bRenameMode)
		m_pNewFunctionButton->setText(__tr2qs_ctx("&Add", "editor"));
	else
		m_pNewFunctionButton->setText(__tr2qs_ctx("&Rename", "editor"));

	if(szFunctionName.isEmpty())
		m_pNewFunctionButton->setEnabled(false);
	connect(m_pNewFunctionButton, SIGNAL(clicked()), this, SLOT(accept()));

	QPushButton * pCancelButton = new QPushButton(pHBox);
	pCancelButton->setObjectName("cancelButton");
	pCancelButton->setText(__tr2qs_ctx("&Cancel", "editor"));
	connect(pCancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	setLayout(pLayout);
}

KviClassEditorFunctionDialog::~KviClassEditorFunctionDialog()
    = default;

void KviClassEditorFunctionDialog::textChanged(const QString & szText)
{
	m_pNewFunctionButton->setEnabled(!szText.isEmpty());
}
