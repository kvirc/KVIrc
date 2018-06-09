//=============================================================================
//
//   File : AliasEditorWindow.cpp
//   Creation date : Mon Dec 23 2002 14:36:18 by Szymon Stefanek
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

#include "AliasEditorWindow.h"

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
#include "KviKvsAliasManager.h"
#include "KviCommandFormatter.h"
#include "KviModule.h"
#include "KviTalVBox.h"
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

extern AliasEditorWindow * g_pAliasEditorWindow;
extern KviModule * g_pAliasEditorModule;

AliasEditorTreeWidgetItem::AliasEditorTreeWidgetItem(QTreeWidget * pTreeWidget, Type eType, const QString & szName)
    : QTreeWidgetItem(pTreeWidget), KviHeapObject(), m_eType(eType), m_pParentItem(nullptr)
{
	setName(szName);
	m_cPos = 0;
	if(eType == AliasEditorTreeWidgetItem::Namespace)
		setIcon(0, QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::NameSpace))));
	else
		setIcon(0, QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Alias))));
}

AliasEditorTreeWidgetItem::AliasEditorTreeWidgetItem(AliasEditorTreeWidgetItem * pParentItem, Type eType, const QString & szName)
    : QTreeWidgetItem(pParentItem), m_eType(eType), m_pParentItem(pParentItem)
{
	setName(szName);
	m_cPos = 0;
	setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	if(eType == AliasEditorTreeWidgetItem::Namespace)
		setIcon(0, QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::NameSpace))));
	else
		setIcon(0, QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Alias))));
}

void AliasEditorTreeWidgetItem::setName(const QString & szName)
{
	m_szName = szName;
	setText(0, m_szName);
}
void AliasEditorTreeWidgetItem::setType(Type t)
{
	m_eType = t;
	if(t == AliasEditorTreeWidgetItem::Namespace)
		setIcon(0, QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::NameSpace))));
	else
		setIcon(0, QIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Alias))));
}

AliasEditorTreeWidget::AliasEditorTreeWidget(QWidget * par)
    : QTreeWidget(par)
{
	setColumnCount(1);
	setHeaderLabel(__tr2qs_ctx("Alias", "editor"));
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setSortingEnabled(true);
	setRootIsDecorated(true);
	setAnimated(true);
}

AliasEditorTreeWidget::~AliasEditorTreeWidget()
{
	//remove and delete all items
	clear();
}

AliasEditorWidget::AliasEditorWidget(QWidget * par)
    : QWidget(par)
{
	m_pAliases = new KviPointerList<AliasEditorTreeWidgetItem>;
	m_pAliases->setAutoDelete(false);

	m_bSaving = false;
	m_pLastEditedItem = nullptr;
	m_pLastClickedItem = nullptr;
	m_szDir = QDir::homePath();

	QGridLayout * l = new QGridLayout(this);

	m_pSplitter = new QSplitter(Qt::Horizontal, this);
	m_pSplitter->setChildrenCollapsible(false);
	l->addWidget(m_pSplitter, 0, 0);

	KviTalVBox * box = new KviTalVBox(m_pSplitter);
	box->setSpacing(0);
	box->setMargin(0);

	m_pTreeWidget = new AliasEditorTreeWidget(box);

	QPushButton * btn = new QPushButton(__tr2qs_ctx("&Export All to...", "editor"), box);
	connect(btn, SIGNAL(clicked()), this, SLOT(exportAll()));

	box = new KviTalVBox(m_pSplitter);
	KviTalHBox * hbox = new KviTalHBox(box);
	hbox->setSpacing(0);
	hbox->setMargin(0);
	m_pNameLabel = new QLabel(__tr2qs_ctx("No item selected", "editor"), hbox);
	m_pRenameButton = new QPushButton(__tr2qs_ctx("Rename", "editor"), hbox);
	m_pRenameButton->setEnabled(false);
	connect(m_pRenameButton, SIGNAL(clicked()), this, SLOT(renameItem()));
	hbox->setStretchFactor(m_pNameLabel, 2);
	m_pRenameButton->setToolTip(__tr2qs_ctx("Edit the alias or namespace name", "editor"));
	m_pEditor = KviScriptEditor::createInstance(box);
	m_pEditor->setFocus();

	connect(m_pEditor, SIGNAL(find(const QString &)), this, SLOT(slotFindWord(const QString &)));
	connect(m_pEditor, SIGNAL(replaceAll(const QString &, const QString &)), this, SLOT(slotReplaceAll(const QString &, const QString &)));

	m_pContextPopup = new QMenu(this);

	oneTimeSetup();

	currentItemChanged(nullptr, nullptr);
}

AliasEditorWidget::~AliasEditorWidget()
{
	KviScriptEditor::destroyInstance(m_pEditor);
	delete m_pAliases;
}

void AliasEditorWidget::buildFullItemPath(AliasEditorTreeWidgetItem * it, QString & szBuffer)
{
	if(it->isAlias())
		it = it->parentItem();
	if(!it)
		return;
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

QString AliasEditorWidget::buildFullItemName(AliasEditorTreeWidgetItem * it)
{
	if(!it)
		return QString();
	QString szName = it->name();
	AliasEditorTreeWidgetItem * nit = it->parentItem();

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

//FIXME this should be called something like "findNamespace"
AliasEditorTreeWidgetItem * AliasEditorWidget::findTopLevelItem(const QString & szName)
{
	AliasEditorTreeWidgetItem * pItem = nullptr;
	for(int i = 0; i < m_pTreeWidget->topLevelItemCount(); i++)
	{
		pItem = (AliasEditorTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		if(pItem->text(0) == szName && pItem->isNamespace())
			return (AliasEditorTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
	}
	return nullptr;
}

AliasEditorTreeWidgetItem * AliasEditorWidget::findItem(const QString & szFullName)
{
	QStringList lNamespaces = szFullName.split("::");
	if(!lNamespaces.count())
		return nullptr;
	AliasEditorTreeWidgetItem * pItem = findTopLevelItem(lNamespaces.at(0));
	if(!pItem)
		return nullptr;
	bool bFound;
	int i;
	int j;
	for(i = 1; i < lNamespaces.count(); i++)
	{
		bFound = false;
		for(j = 0; j < pItem->childCount(); j++)
		{
			if(KviQString::equalCI(pItem->child(j)->text(0), lNamespaces.at(i)))
			{
				pItem = (AliasEditorTreeWidgetItem *)pItem->child(j);
				bFound = true;
				break;
			}
		}
		if(!bFound)
			return nullptr;
	}
	return (AliasEditorTreeWidgetItem *)pItem;
}

AliasEditorTreeWidgetItem * AliasEditorWidget::createFullItem(const QString & szFullName)
{
	QStringList lNamespaces = szFullName.split("::");
	if(!lNamespaces.count())
		return nullptr;
	if(lNamespaces.count() == 1)
		return new AliasEditorTreeWidgetItem(m_pTreeWidget, AliasEditorTreeWidgetItem::Alias, lNamespaces.at(0));
	AliasEditorTreeWidgetItem * pItem = findTopLevelItem(lNamespaces.at(0));
	if(!pItem)
		pItem = new AliasEditorTreeWidgetItem(m_pTreeWidget, AliasEditorTreeWidgetItem::Namespace, lNamespaces.at(0));
	bool bFound;
	int i;
	for(i = 1; i < lNamespaces.count() - 1; i++)
	{
		bFound = false;
		for(int j = 0; j < pItem->childCount(); j++)
		{
			if(KviQString::equalCI(pItem->child(j)->text(0), lNamespaces.at(i)))
			{
				pItem = (AliasEditorTreeWidgetItem *)pItem->child(j);
				bFound = true;
				break;
			}
		}
		if(!bFound)
			pItem = new AliasEditorTreeWidgetItem(pItem, AliasEditorTreeWidgetItem::Namespace, lNamespaces.at(i));
	}
	return new AliasEditorTreeWidgetItem(pItem, AliasEditorTreeWidgetItem::Alias, lNamespaces.at(i));
}

void AliasEditorWidget::oneTimeSetup()
{
	KviPointerHashTable<QString, KviKvsScript> * a = KviKvsAliasManager::instance()->aliasDict();
	if(!a)
		return;

	KviPointerHashTableIterator<QString, KviKvsScript> it(*a);

	AliasEditorTreeWidgetItem * item;
	while(it.current())
	{
		KviKvsScript * alias = it.current();
		item = createFullItem(alias->name());
		item->setBuffer(alias->code());
		m_pAliases->append(item);
		++it;
	}

	connect(m_pTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));
	m_pTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_pTreeWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(customContextMenuRequested(const QPoint &)));
	connect(m_pTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(itemRenamed(QTreeWidgetItem *, int)));
	connect(KviKvsAliasManager::instance(), SIGNAL(aliasRefresh(const QString &)), this, SLOT(aliasRefresh(const QString &)));
	m_pTreeWidget->sortItems(0, Qt::AscendingOrder);
}

void AliasEditorWidget::aliasRefresh(const QString & szName)
{
	if(m_bSaving)
		return;
	AliasEditorTreeWidgetItem * item = nullptr;
	KviKvsScript * alias = KviKvsAliasManager::instance()->aliasDict()->find(szName);

	// search for old alias with same name
	KviPointerList<AliasEditorTreeWidgetItem> l;
	l.setAutoDelete(false);
	appendAllItems(&l, AliasEditorTreeWidgetItem::Alias);
	for(AliasEditorTreeWidgetItem * it = l.first(); it; it = l.next())
	{
		if(KviQString::equalCI(buildFullItemName(it), szName))
		{
			item = it;
			break;
		}
	}
	if(!item)
	{
		item = createFullItem(szName);
		m_pAliases->append(item);
	}

	if(item != m_pLastEditedItem)
	{
		item->setBuffer(alias->code());
		return;
	}
	if(
	    QMessageBox::warning(nullptr, __tr2qs_ctx("Confirm Overwriting Current - KVIrc", "editor"),
	        __tr2qs_ctx("An external script has changed the alias you are currently editing. Do you want to accept the external changes?", "editor"),
	        QMessageBox::Yes, QMessageBox::No | QMessageBox::Default | QMessageBox::Escape)
	    != QMessageBox::Yes)
		return;
	item->setBuffer(alias->code());
	m_pEditor->setText(alias->code());
}
void AliasEditorWidget::itemRenamed(QTreeWidgetItem * it, int col)
{
	if(it != m_pLastEditedItem)
		return;

	((AliasEditorTreeWidgetItem *)it)->setName(it->text(col));
	QString szNam = buildFullItemName((AliasEditorTreeWidgetItem *)it);
	QString szLabelText;
	if(((AliasEditorTreeWidgetItem *)it)->isNamespace())
		szLabelText = __tr2qs_ctx("Namespace", "editor");
	else
		szLabelText = __tr2qs_ctx("Alias", "editor");
	szLabelText += ": <b>";
	szLabelText += szNam;
	szLabelText += "</b>";
	m_pNameLabel->setText(szLabelText);
}

bool AliasEditorWidget::hasSelectedItems()
{
	return m_pTreeWidget->selectedItems().count() ? true : false;
}

bool AliasEditorWidget::itemExists(QTreeWidgetItem * pSearchFor)
{
	if(!pSearchFor)
		return false;
	if(m_pAliases->findRef((AliasEditorTreeWidgetItem *)pSearchFor) != -1)
		return true;
	else
		return false;
}

void AliasEditorWidget::appendAllItems(KviPointerList<AliasEditorTreeWidgetItem> * l, AliasEditorTreeWidgetItem::Type eType)
{
	for(int i = 0; i < m_pTreeWidget->topLevelItemCount(); i++)
	{
		if(((AliasEditorTreeWidgetItem *)m_pTreeWidget->topLevelItem(i))->type() == eType)
		{
			l->append(((AliasEditorTreeWidgetItem *)m_pTreeWidget->topLevelItem(i)));
		}
		else
		{
			appendAllItemsRecursive(l, m_pTreeWidget->topLevelItem(i), eType);
		}
	}
}

void AliasEditorWidget::appendAllItemsRecursive(KviPointerList<AliasEditorTreeWidgetItem> * l, QTreeWidgetItem * pStartFrom, AliasEditorTreeWidgetItem::Type eType)
{
	for(int i = 0; i < pStartFrom->childCount(); i++)
	{
		if(((AliasEditorTreeWidgetItem *)pStartFrom->child(i))->type() == eType)
		{
			l->append((AliasEditorTreeWidgetItem *)pStartFrom->child(i));
		}
		else
		{
			appendAllItemsRecursive(l, pStartFrom->child(i), eType);
		}
	}
}

bool AliasEditorWidget::aliasExists(QString & szFullItemName)
{
	KviPointerList<AliasEditorTreeWidgetItem> l;
	l.setAutoDelete(false);
	appendAllItems(&l, AliasEditorTreeWidgetItem::Alias);
	for(AliasEditorTreeWidgetItem * it = l.first(); it; it = l.next())
	{
		if(KviQString::equalCI(buildFullItemName(it), szFullItemName))
		{
			return true;
		}
	}
	return false;
}

bool AliasEditorWidget::namespaceExists(QString & szFullItemName)
{
	KviPointerList<AliasEditorTreeWidgetItem> l;
	l.setAutoDelete(false);

	appendAllItems(&l, AliasEditorTreeWidgetItem::Namespace);
	for(AliasEditorTreeWidgetItem * it = l.first(); it; it = l.next())
	{
		if(KviQString::equalCI(buildFullItemName(it), szFullItemName))
		{
			return true;
		}
	}
	return false;
}
void AliasEditorWidget::renameItem()
{
	if(!m_pLastEditedItem)
		return;
	//if(itemExists(m_pLastEditedItem))return; // dead ?
	QString szName = buildFullItemName(m_pLastEditedItem);
	QString szNewName;

	bool bAlias = m_pLastEditedItem->isAlias();

	if(bAlias)
		szNewName = askForAliasName(__tr2qs_ctx("Enter a New Name - KVIrc", "editor"), __tr2qs_ctx("Please enter the new name for the alias.", "editor"), szName);
	else
		szNewName = askForNamespaceName(__tr2qs_ctx("Enter a New Name - KVIrc", "editor"), __tr2qs_ctx("Please enter the new name for the namespace.", "editor"), szName);
	if(szNewName.isEmpty())
		return;

	if(szName == szNewName)
		return;

	// check if there is already an alias with this name
	if(bAlias)
	{
		if(aliasExists(szNewName))
		{
			g_pAliasEditorModule->lock();
			QMessageBox::information(this,
			    __tr2qs_ctx("Name Already Exists - KVIrc", "editor"),
			    __tr2qs_ctx("This name is already in use. Please choose another one.", "editor"),
			    __tr2qs_ctx("OK, Let me try again...", "editor"));
			g_pAliasEditorModule->unlock();
			return;
		}
	}
	else
	{
		// check if there is already a namespace with this name
		if(namespaceExists(szNewName))
		{
			g_pAliasEditorModule->lock();
			QMessageBox::information(this,
			    __tr2qs_ctx("Name Already Exists - KVIrc", "editor"),
			    __tr2qs_ctx("This name is already in use. Please choose another one.", "editor"),
			    __tr2qs_ctx("OK, Let me try again...", "editor"));
			g_pAliasEditorModule->unlock();
			return;
		}
	}

	QString szCode;
	int pntCursor;
	if(bAlias)
	{
		m_pEditor->getText(szCode);
		pntCursor = m_pEditor->getCursor();
	}
	QList<QTreeWidgetItem *> lChildren = m_pLastEditedItem->takeChildren();
	bool bYesToAll = true;
	removeItem(m_pLastEditedItem, &bYesToAll, true);

	m_pLastEditedItem = nullptr;  // make sure it's true (but it already should be because of removeItem())
	m_pLastClickedItem = nullptr; // make sure it's true (but it already should be because of removeItem())
	AliasEditorTreeWidgetItem * pItem = createFullItem(szNewName);
	if(bAlias)
	{
		pItem->setBuffer(szCode);
		pItem->setCursorPosition(pntCursor);
	}
	else
	{
		for(int i = 0; i < lChildren.count(); i++)
		{
			((AliasEditorTreeWidgetItem *)lChildren.at(i))->setParentItem(pItem);
			pItem->insertChild(pItem->childCount(), lChildren.at(i));
		}
	}
	activateItem(pItem);
	m_pAliases->append(pItem);
}

// warning: this saves ONLY the body, not the name!
//          the name is changed explicitly with renameItem(), when needed

void AliasEditorWidget::saveLastEditedItem()
{
	if(!m_pLastEditedItem)
		return;
	if(!itemExists(m_pLastEditedItem) || !m_pEditor->isModified() || m_pLastEditedItem->isNamespace())
		return;
	((AliasEditorTreeWidgetItem *)m_pLastEditedItem)->setCursorPosition(m_pEditor->getCursor());
	QString newCode;
	m_pEditor->getText(newCode);
	((AliasEditorTreeWidgetItem *)m_pLastEditedItem)->setBuffer(newCode);
}

void AliasEditorWidget::currentItemChanged(QTreeWidgetItem * it, QTreeWidgetItem *)
{
	saveLastEditedItem();
	m_pLastEditedItem = (AliasEditorTreeWidgetItem *)it;

	if(!m_pLastEditedItem)
	{
		m_pNameLabel->setText(__tr2qs_ctx("No item selected", "editor"));
		m_pRenameButton->setEnabled(false);
		m_pEditor->setText("");
		m_pEditor->setEnabled(false);
		return;
	}

	QString szNam = buildFullItemName(m_pLastEditedItem);
	if(m_pLastEditedItem->isNamespace())
	{
		QString szLabelText = __tr2qs_ctx("Namespace", "editor");
		szLabelText += ": <b>";
		szLabelText += szNam;
		szLabelText += "</b>";
		m_pNameLabel->setText(szLabelText);
		m_pRenameButton->setEnabled(true);
		m_pEditor->setText("");
		m_pEditor->setEnabled(false);
		m_pTreeWidget->setFocus();
		return;
	}

	QString szLabelText = __tr2qs_ctx("Alias", "editor");
	szLabelText += ": <b>";
	szLabelText += szNam;
	szLabelText += "</b>";
	m_pNameLabel->setText(szLabelText);
	m_pRenameButton->setEnabled(true);
	m_pEditor->setText(((AliasEditorTreeWidgetItem *)it)->buffer());

	m_pEditor->setFocus();
	m_pEditor->setCursorPosition(((AliasEditorTreeWidgetItem *)it)->cursorPosition());
	m_pEditor->setEnabled(true);
}

void AliasEditorWidget::customContextMenuRequested(const QPoint pnt)
{
	m_pContextPopup->clear();
	m_pLastClickedItem = (AliasEditorTreeWidgetItem *)m_pTreeWidget->itemAt(pnt);

	m_pContextPopup->addAction(
	    *(g_pIconManager->getSmallIcon(KviIconManager::Alias)),
	    __tr2qs_ctx("Add Alias", "editor"),
	    this, SLOT(newAlias()));

	m_pContextPopup->addAction(
	    *(g_pIconManager->getSmallIcon(KviIconManager::NameSpace)),
	    __tr2qs_ctx("Add Namespace", "editor"),
	    this, SLOT(newNamespace()));

	bool bHasItems = m_pTreeWidget->topLevelItemCount();
	bool bHasSelected = hasSelectedItems();

	m_pContextPopup->addSeparator();

	QAction * pAction = m_pContextPopup->addAction(
	    *(g_pIconManager->getSmallIcon(KviIconManager::Discard)),
	    __tr2qs_ctx("Remove Selected", "editor"),
	    this, SLOT(removeSelectedItems()));
	pAction->setEnabled(bHasSelected);

	m_pContextPopup->addSeparator();

	m_pContextPopup->addAction(
	                   *(g_pIconManager->getSmallIcon(KviIconManager::Save)),
	                   __tr2qs_ctx("Export Selected...", "editor"),
	                   this, SLOT(exportSelected()))
	    ->setEnabled(bHasSelected);

	m_pContextPopup->addAction(
	                   *(g_pIconManager->getSmallIcon(KviIconManager::Save)),
	                   __tr2qs_ctx("Export Selected into Single Files...", "editor"),
	                   this, SLOT(exportSelectedSepFiles()))
	    ->setEnabled(bHasSelected);

	m_pContextPopup->addAction(
	                   *(g_pIconManager->getSmallIcon(KviIconManager::Save)),
	                   __tr2qs_ctx("Export All...", "editor"),
	                   this, SLOT(exportAll()))
	    ->setEnabled(bHasItems);

	m_pContextPopup->addSeparator();

	m_pContextPopup->addAction(
	                   *(g_pIconManager->getSmallIcon(KviIconManager::Search)),
	                   __tr2qs_ctx("Find in Aliases...", "editor"),
	                   this, SLOT(slotFind()))
	    ->setEnabled(bHasItems);

	m_pContextPopup->addAction(
	                   *(g_pIconManager->getSmallIcon(KviIconManager::NameSpace)),
	                   __tr2qs_ctx("Collapse All Namespaces", "editor"),
	                   this, SLOT(slotCollapseNamespaces()))
	    ->setEnabled(bHasItems);
	m_pContextPopup->popup(m_pTreeWidget->mapToGlobal(pnt));
}

void AliasEditorWidget::searchReplace(const QString & szSearch, bool bReplace, const QString & szReplace)
{
	for(unsigned int i = 0; i < m_pAliases->count(); i++)
	{
		AliasEditorTreeWidgetItem * pItem = m_pAliases->at(i);
		if(pItem->buffer().indexOf(szSearch, 0, Qt::CaseInsensitive) != -1)
		{
			pItem->setBackground(0, QColor(255, 0, 0, 128));
			if(bReplace)
				((QString &)pItem->buffer()).replace(szSearch, szReplace, Qt::CaseInsensitive);
			openParentItems(pItem);
		}
		else
		{
			pItem->setBackground(0, QColor(255, 255, 255));
		}
	}
}

void AliasEditorWidget::slotFind()
{

	g_pAliasEditorModule->lock();
	bool bOk;

	QString szSearch = QInputDialog::getText(this,
	    __tr2qs_ctx("Find in Aliases", "editor"),
	    __tr2qs_ctx("Please enter the text to be searched for. The matching aliases will be highlighted.", "editor"),
	    QLineEdit::Normal,
	    "",
	    &bOk);

	g_pAliasEditorModule->unlock();
	if(!bOk)
		return;
	if(szSearch.isEmpty())
		return;
	m_pEditor->setFindText(szSearch);
	searchReplace(szSearch);
}

void AliasEditorWidget::slotFindWord(const QString & szSearch)
{
	m_pEditor->setFindText(szSearch);
}

void AliasEditorWidget::recursiveCollapseNamespaces(AliasEditorTreeWidgetItem * it)
{
	if(!it)
		return;
	for(int i = 0; i < it->childCount(); i++)
	{
		if(it->child(i)->childCount())
		{
			it->child(i)->setExpanded(false);
			recursiveCollapseNamespaces((AliasEditorTreeWidgetItem *)it->child(i));
		}
	}
}

void AliasEditorWidget::slotCollapseNamespaces()
{
	for(int i = 0; i < m_pTreeWidget->topLevelItemCount(); i++)
	{
		if(m_pTreeWidget->topLevelItem(i)->childCount())
		{
			m_pTreeWidget->topLevelItem(i)->setExpanded(false);
			recursiveCollapseNamespaces((AliasEditorTreeWidgetItem *)m_pTreeWidget->topLevelItem(i));
		}
	}
}

void AliasEditorWidget::slotReplaceAll(const QString & szFind, const QString & szReplace)
{
	m_pEditor->setFindText(szReplace);
	searchReplace(szFind, true, szReplace);
}

void AliasEditorWidget::getExportAliasBuffer(QString & buffer, AliasEditorTreeWidgetItem * it)
{
	QString szBuf = it->buffer();
	KviCommandFormatter::blockFromBuffer(szBuf);
	QString szNam = buildFullItemName(it);

	buffer = "alias(";
	buffer += szNam;
	buffer += ")\n";
	buffer += szBuf;
	buffer += "\n";
}

void AliasEditorWidget::exportAll()
{
	exportAliases(false);
}

void AliasEditorWidget::exportSelectedSepFiles()
{
	exportAliases(true, true);
}

void AliasEditorWidget::exportSelected()
{
	exportAliases(true);
}

void AliasEditorWidget::exportSelectionInSinglesFiles(KviPointerList<AliasEditorTreeWidgetItem> * l)
{
	if(!m_szDir.endsWith(QString(KVI_PATH_SEPARATOR)))
		m_szDir += KVI_PATH_SEPARATOR;
	if(!l->first())
	{
		g_pAliasEditorModule->lock();
		QMessageBox::warning(this, __tr2qs_ctx("Warning While Exporting - KVIrc", "editor"), __tr2qs_ctx("Must select an entry from the list to export!", "editor"), __tr2qs_ctx("OK", "editor"));
		g_pAliasEditorModule->unlock();
		return;
	}
	g_pAliasEditorModule->lock();

	if(!KviFileDialog::askForDirectoryName(
	       m_szDir,
	       __tr2qs_ctx("Choose a Directory - KVIrc", "editor"),
	       m_szDir,
	       QString(),
	       false,
	       true,
	       this))
	{
		g_pAliasEditorModule->unlock();
		return;
	}

	if(!m_szDir.endsWith(QString(KVI_PATH_SEPARATOR)))
		m_szDir += KVI_PATH_SEPARATOR;

	bool bReplaceAll = false;

	for(AliasEditorTreeWidgetItem * it = l->first(); it; it = l->next())
	{
		QString tmp;
		getExportAliasBuffer(tmp, it);
		QString szFileName = buildFullItemName(it);
		szFileName += ".kvs";
		szFileName.replace("::", "_");
		QString szCompletePath = m_szDir + szFileName;
		if(KviFileUtils::fileExists(szCompletePath) && !bReplaceAll)
		{
			QString szMsg;
			szMsg = QString(__tr2qs_ctx("The file \"%1\" exists. Do you want to replace it?", "editor")).arg(szFileName);
			int ret = QMessageBox::question(this, __tr2qs_ctx("Confirm Replacing File - KVIrc", "editor"), szMsg, __tr2qs_ctx("Yes", "editor"), __tr2qs_ctx("Yes to All", "editor"), __tr2qs_ctx("No", "editor"));
			if(ret != 2)
			{
				KviFileUtils::writeFile(szCompletePath, tmp);
				if(ret == 1)
					bReplaceAll = true;
			}
		}
		else
		{
			KviFileUtils::writeFile(szCompletePath, tmp);
		}
	}
	g_pAliasEditorModule->unlock();
}

void AliasEditorWidget::exportAliases(bool bSelectedOnly, bool bSingleFiles)
{
	saveLastEditedItem();

	KviPointerList<AliasEditorTreeWidgetItem> l;
	l.setAutoDelete(false);

	if(bSelectedOnly)
		appendSelectedAliasItems(&l);
	if(bSingleFiles)
	{
		exportSelectionInSinglesFiles(&l);
		return;
	}
	QString szOut;
	int count;

	if(bSelectedOnly)
	{
		count = l.count();
		buildAliasesFile(&l, szOut);
	}
	else
	{
		count = m_pAliases->count();
		buildAliasesFile(m_pAliases, szOut);
	}
	QString szNameFile;
	if(szOut.isEmpty())
	{
		g_pAliasEditorModule->lock();
		QMessageBox::warning(this, __tr2qs_ctx("Warning While Exporting - KVIrc", "editor"), __tr2qs_ctx("The exported file could be empty: cowardly refusing to write it.", "editor"), __tr2qs_ctx("OK", "editor"));
		g_pAliasEditorModule->unlock();
		return;
	}

	QString szName = m_szDir;

	if(!szName.endsWith(QString(KVI_PATH_SEPARATOR)))
		szName += KVI_PATH_SEPARATOR;
	QString szFile;
	g_pAliasEditorModule->lock();

	if(count == 1)
	{
		QString tmp = buildFullItemName(l.at(0));
		szNameFile = tmp.replace("::", "_");
	}
	else
	{
		szNameFile = "aliases";
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
		g_pAliasEditorModule->unlock();
		return;
	}
	m_szDir = QFileInfo(szFile).absolutePath();
	g_pAliasEditorModule->unlock();

	if(!KviFileUtils::writeFile(szFile, szOut))
	{
		g_pAliasEditorModule->lock();
		QMessageBox::warning(this, __tr2qs_ctx("Writing to File Failed - KVIrc", "editor"), __tr2qs_ctx("Unable to write to the aliases file.", "editor"), __tr2qs_ctx("OK", "editor"));
		g_pAliasEditorModule->unlock();
	}
}
void AliasEditorWidget::buildAliasesFile(KviPointerList<AliasEditorTreeWidgetItem> * l, QString & szBuffer)
{
	for(AliasEditorTreeWidgetItem * it = l->first(); it; it = l->next())
	{
		QString tmp;
		getExportAliasBuffer(tmp, it);
		szBuffer += tmp;
		szBuffer += "\n";
	}
}

void AliasEditorWidget::saveProperties(KviConfigurationFile * cfg)
{
	cfg->writeEntry("Sizes", m_pSplitter->sizes());
	QString szName;
	if(m_pLastEditedItem)
		szName = buildFullItemName(m_pLastEditedItem);
	cfg->writeEntry("LastAlias", szName);
}

void AliasEditorWidget::loadProperties(KviConfigurationFile * cfg)
{
	QList<int> def;
	def.append(20);
	def.append(80);
	m_pSplitter->setSizes(cfg->readIntListEntry("Sizes", def));
	QString tmp = cfg->readEntry("LastAlias", QString());

	AliasEditorTreeWidgetItem * it = findItem(tmp);
	activateItem(it);
}

void AliasEditorWidget::appendSelectedAliasItems(KviPointerList<AliasEditorTreeWidgetItem> * l)
{
	QList<QTreeWidgetItem *> list = m_pTreeWidget->selectedItems();
	for(int i = 0; i < list.count(); i++)
	{
		if(((AliasEditorTreeWidgetItem *)list.at(i))->isAlias())
		{
			l->append((AliasEditorTreeWidgetItem *)list.at(i));
		}
		else
		{
			appendSelectedAliasItemsRecursive(l, list.at(i));
		}
	}
}

void AliasEditorWidget::appendSelectedAliasItemsRecursive(KviPointerList<AliasEditorTreeWidgetItem> * l, QTreeWidgetItem * pStartFrom)
{
	for(int i = 0; i < pStartFrom->childCount(); i++)
	{
		if(((AliasEditorTreeWidgetItem *)pStartFrom->child(i))->isAlias())
			l->append(((AliasEditorTreeWidgetItem *)pStartFrom->child(i)));
		else
			appendSelectedAliasItemsRecursive(l, pStartFrom->child(i));
	}
}

void AliasEditorWidget::appendSelectedItems(KviPointerList<AliasEditorTreeWidgetItem> * l)
{
	QList<QTreeWidgetItem *> list = m_pTreeWidget->selectedItems();
	for(int i = 0; i < list.count(); i++)
	{
		l->append((AliasEditorTreeWidgetItem *)list.at(i));
	}
}

void AliasEditorWidget::appendSelectedItemsRecursive(KviPointerList<AliasEditorTreeWidgetItem> * l, QTreeWidgetItem * pStartFrom)
{
	for(int i = 0; i < pStartFrom->childCount(); i++)
	{
		l->append(((AliasEditorTreeWidgetItem *)pStartFrom->child(i)));
		if(!((AliasEditorTreeWidgetItem *)pStartFrom->child(i))->isAlias())
			appendSelectedAliasItemsRecursive(l, pStartFrom->child(i));
	}
}

void AliasEditorWidget::removeItemChildren(AliasEditorTreeWidgetItem * it)
{
	while(it->childCount() > 0)
	{
		AliasEditorTreeWidgetItem * pChild = (AliasEditorTreeWidgetItem *)(it->child(0));
		if(pChild->childCount())
			removeItemChildren(pChild);
		delete pChild;
		m_pAliases->removeRef(pChild);
	}
}

bool AliasEditorWidget::removeItem(AliasEditorTreeWidgetItem * it, bool * pbYesToAll, bool)
{
	if(!it)
		return true;
	QString szMsg;
	QString szName = it->name();

	if(!*pbYesToAll)
	{
		if(it->isAlias())
			szMsg = QString(__tr2qs_ctx("Do you really want to remove the alias \"%1\"?", "editor")).arg(szName);
		else
		{
			szMsg = QString(__tr2qs_ctx("Do you really want to remove the namespace \"%1\"?", "editor")).arg(szName);
			szMsg += "<br>";
			szMsg += __tr2qs_ctx("Please note that all the child items will be deleted too.", "editor");
		}

		g_pAliasEditorModule->lock();
		int ret = QMessageBox::question(this, __tr2qs_ctx("Confirm Removing Item - KVIrc", "editor"), szMsg, __tr2qs_ctx("Yes", "editor"), __tr2qs_ctx("Yes to All", "editor"), __tr2qs_ctx("No", "editor"));
		g_pAliasEditorModule->unlock();
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
		m_pLastEditedItem = nullptr;
	if(it == m_pLastClickedItem)
		m_pLastClickedItem = nullptr;
	if(it->childCount())
		removeItemChildren(it);
	delete it;
	m_pAliases->removeRef(it);
	return true;
}

void AliasEditorWidget::removeSelectedItems()
{
	KviPointerList<AliasEditorTreeWidgetItem> l;
	l.setAutoDelete(false);
	appendSelectedItems(&l);
	bool bYesToAll = false;
	for(AliasEditorTreeWidgetItem * it = l.first(); it; it = l.next())
	{
		if(!removeItem(it, &bYesToAll, false))
			return;
	}
}

QString AliasEditorWidget::askForAliasName(const QString & szAction, const QString & szText, const QString & szInitialText)
{
	bool bOk = false;
	QString szNewName;
	while(szNewName.isEmpty())
	{
		g_pAliasEditorModule->lock();
		szNewName = QInputDialog::getText(this,
		    szAction,
		    szText,
		    QLineEdit::Normal,
		    szInitialText,
		    &bOk);
		g_pAliasEditorModule->unlock();
		if(!bOk)
			return QString();
		if(szNewName.isEmpty())
		{
			g_pAliasEditorModule->lock();
			QMessageBox::warning(this,
			    __tr2qs_ctx("Invalid or Missing Name - KVIrc", "editor"),
			    __tr2qs_ctx("You must specify a valid name for the alias.", "editor"),
			    __tr2qs_ctx("OK, Let me try again...", "editor"));
			g_pAliasEditorModule->unlock();
			continue;
		}

		// we allow only [\w:]+
		QRegExp re("[\\w:]+");
		if(!re.exactMatch(szNewName))
		{
			g_pAliasEditorModule->lock();
			QMessageBox::information(this,
			    __tr2qs_ctx("Invalid Name - KVIrc", "editor"),
			    __tr2qs_ctx("Aliases names can contain only letters, digits, underscores and '::' namespace separators.", "editor"),
			    __tr2qs_ctx("OK, Let me try again...", "editor"));
			g_pAliasEditorModule->unlock();
			szNewName = "";
			continue;
		}
		// make sure that we have only doubled "::" and not ":" or ":::..."
		QString tmp = szNewName;
		tmp.replace("::", "@"); // @ is not allowed by the rule above
		if(tmp.indexOf(":", Qt::CaseInsensitive) != -1)
		{
			g_pAliasEditorModule->lock();
			QMessageBox::information(this,
			    __tr2qs_ctx("Invalid Name - KVIrc", "editor"),
			    __tr2qs_ctx("Stray ':' character in alias name: did you mean ...<namespace>::<name>?", "editor"),
			    __tr2qs_ctx("OK, Let me try again...", "editor"));
			g_pAliasEditorModule->unlock();
			szNewName = "";
			continue;
		}
		if(tmp.indexOf("@@", Qt::CaseInsensitive) != -1)
		{
			g_pAliasEditorModule->lock();
			QMessageBox::information(this,
			    __tr2qs_ctx("Invalid Name - KVIrc", "editor"),
			    __tr2qs_ctx("Found an empty namespace in alias name.", "editor"),
			    __tr2qs_ctx("OK, Let me try again...", "editor"));
			g_pAliasEditorModule->unlock();
			szNewName = "";
			continue;
		}
	}
	return szNewName;
}

QString AliasEditorWidget::askForNamespaceName(const QString & szAction, const QString & szText, const QString & szInitialText)
{
	bool bOk = false;
	QString szNewName;
	while(szNewName.isEmpty())
	{
		g_pAliasEditorModule->lock();
		szNewName = QInputDialog::getText(this,
		    szAction,
		    szText,
		    QLineEdit::Normal,
		    szInitialText,
		    &bOk);
		g_pAliasEditorModule->unlock();
		if(!bOk)
			return QString();
		if(szNewName.isEmpty())
		{
			g_pAliasEditorModule->lock();
			QMessageBox::warning(this,
			    __tr2qs_ctx("Invalid or Missing Name - KVIrc", "editor"),
			    __tr2qs_ctx("You must specify a valid name for the namespace.", "editor"),
			    __tr2qs_ctx("OK, Let me try again...", "editor"));
			g_pAliasEditorModule->unlock();
			continue;
		}

		// we allow only [\w:]+
		QRegExp re("[\\w:]+");
		if(!re.exactMatch(szNewName))
		{
			g_pAliasEditorModule->lock();
			QMessageBox::information(this,
			    __tr2qs_ctx("Invalid Name - KVIrc", "editor"),
			    __tr2qs_ctx("Namespace names can contain only letters, digits, underscores and '::' namespace separators.", "editor"),
			    __tr2qs_ctx("OK, Let me try again...", "editor"));
			g_pAliasEditorModule->unlock();
			szNewName = "";
			continue;
		}
		// make sure that we have only doubled "::" and not ":" or ":::..."
		QString tmp = szNewName;
		tmp.replace("::", "@"); // @ is not allowed by the rule above
		if(tmp.indexOf(":", Qt::CaseInsensitive) != -1)
		{
			g_pAliasEditorModule->lock();
			QMessageBox::information(this,
			    __tr2qs_ctx("Invalid Name - KVIrc", "editor"),
			    __tr2qs_ctx("Stray ':' character in namespace name: did you mean... <namespace>::<name>?", "editor"),
			    __tr2qs_ctx("OK, Let me try again...", "editor"));
			g_pAliasEditorModule->unlock();
			szNewName = "";
			continue;
		}
		if(tmp.indexOf("@@", Qt::CaseInsensitive) != -1)
		{
			g_pAliasEditorModule->lock();
			QMessageBox::information(this,
			    __tr2qs_ctx("Invalid Name - KVIrc", "editor"),
			    __tr2qs_ctx("Found an empty namespace in namespace name.", "editor"),
			    __tr2qs_ctx("OK, Let me try again...", "editor"));
			g_pAliasEditorModule->unlock();
			szNewName = "";
			continue;
		}
	}
	return szNewName;
}

void AliasEditorWidget::openParentItems(QTreeWidgetItem * it)
{
	if(it->parent())
	{
		it->parent()->setExpanded(true);
		openParentItems(it->parent());
	}
}

void AliasEditorWidget::activateItem(QTreeWidgetItem * it)
{
	if(!it)
		return;
	openParentItems(it);
	m_pTreeWidget->setCurrentItem(it);
}

void AliasEditorWidget::newAlias()
{
	QString szNewName = askForAliasName(__tr2qs_ctx("Enter a Filename - KVIrc", "editor"), __tr2qs_ctx("Please enter the new name for the alias.", "editor"), "myfunction");
	if(szNewName.isEmpty())
		return;
	newItem(szNewName, AliasEditorTreeWidgetItem::Alias);
}

void AliasEditorWidget::newNamespace()
{
	QString szName = askForNamespaceName(__tr2qs_ctx("Enter a Filename - KVIrc", "editor"), __tr2qs_ctx("Please enter the new name for the namespace.", "editor"), "mynamespace");
	if(szName.isEmpty())
		return;
	newItem(szName, AliasEditorTreeWidgetItem::Namespace);
}

void AliasEditorWidget::newItem(QString & szName, AliasEditorTreeWidgetItem::Type eType)
{
	if(m_pLastClickedItem)
		if(!itemExists(m_pLastClickedItem))
			m_pLastClickedItem = nullptr;
	if(m_pLastClickedItem)
		buildFullItemPath(m_pLastClickedItem, szName);
	int idx = 1;
	QString szTmp;
	if(findItem(szName))
		szName.append("1");
	while(findItem(szName))
	{
		szTmp.setNum(idx);
		szName.chop(szTmp.length());
		szName.append(szTmp);
		idx++;
	}
	AliasEditorTreeWidgetItem * it;
	it = createFullItem(szName);
	it->setType(eType);
	m_pAliases->append(it);
	activateItem(it);
}

void AliasEditorWidget::commit()
{
	m_bSaving = true;
	saveLastEditedItem();
	KviKvsAliasManager::instance()->clear();
	for(unsigned int i = 0; i < m_pAliases->count(); i++)
	{
		//don't save empty aliases or namespace names
		if(m_pAliases->at(i)->buffer().isEmpty())
			continue;
		QString szName = buildFullItemName(m_pAliases->at(i));
		KviKvsScript * a = new KviKvsScript(szName, m_pAliases->at(i)->buffer());
		KviKvsAliasManager::instance()->add(szName, a);
	}
	g_pApp->saveAliases();
	m_bSaving = false;
}

AliasEditorWindow::AliasEditorWindow()
    : KviWindow(KviWindow::ScriptEditor, "aliaseditor", nullptr)
{
	g_pAliasEditorWindow = this;

	setFixedCaption(__tr2qs_ctx("Alias Editor", "editor"));

	QGridLayout * g = new QGridLayout();

	m_pEditor = new AliasEditorWidget(this);
	g->addWidget(m_pEditor, 0, 0, 1, 4);

	QPushButton * btn = new QPushButton(__tr2qs_ctx("&OK", "editor"), this);
	connect(btn, SIGNAL(clicked()), this, SLOT(okClicked()));
	btn->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Accept)));
	g->addWidget(btn, 1, 1);

	btn = new QPushButton(__tr2qs_ctx("&Apply", "editor"), this);
	connect(btn, SIGNAL(clicked()), this, SLOT(applyClicked()));
	btn->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Accept)));
	g->addWidget(btn, 1, 2);

	btn = new QPushButton(__tr2qs_ctx("Cancel", "editor"), this);
	connect(btn, SIGNAL(clicked()), this, SLOT(cancelClicked()));
	btn->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Discard)));
	g->addWidget(btn, 1, 3);

	g->setRowStretch(0, 1);
	g->setColumnStretch(0, 1);
	setLayout(g);
}

AliasEditorWindow::~AliasEditorWindow()
{
	g_pAliasEditorWindow = nullptr;
}

void AliasEditorWindow::okClicked()
{
	m_pEditor->commit();
	close();
}

void AliasEditorWindow::applyClicked()
{
	m_pEditor->commit();
}

void AliasEditorWindow::cancelClicked()
{
	close();
}

QPixmap * AliasEditorWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KviIconManager::AliasEditor);
}

void AliasEditorWindow::getConfigGroupName(QString & szName)
{
	szName = "aliaseditor";
}

void AliasEditorWindow::saveProperties(KviConfigurationFile * cfg)
{
	m_pEditor->saveProperties(cfg);
}

void AliasEditorWindow::loadProperties(KviConfigurationFile * cfg)
{
	m_pEditor->loadProperties(cfg);
}
