//=============================================================================
//
//   File : PopupEditorWindow.cpp
//   Creation date : Mon Dec 23 2002 20:28:18 by Szymon Stefanek
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

#include "PopupEditorWindow.h"

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
#include "KviKvsPopupManager.h"
#include "KviKvsPopupMenu.h"
#include "KviKvsVariantList.h"
#include "KviPointerHashTable.h"
#include "KviTalVBox.h"
#include "kvi_fileextensions.h"

#include <QMessageBox>
#include <QDir>
#include <QHeaderView>
#include <QSplitter>
#include <QLayout>
#include <QLabel>
#include <QToolTip>
#include <QPushButton>
#include <QAbstractItemView>

extern PopupEditorWindow * g_pPopupEditorWindow;

//KviPopupEntryItem
PopupTreeWidgetItem::PopupTreeWidgetItem(QTreeWidget * pTreeWidget, PopupTreeWidgetItem * after, Type t)
    : QTreeWidgetItem(pTreeWidget, after), m_type(t)
{
	init();
}

PopupTreeWidgetItem::PopupTreeWidgetItem(PopupTreeWidgetItem * parent, PopupTreeWidgetItem * after, Type t)
    : QTreeWidgetItem(parent, after), m_type(t)
{
	init();
}

void PopupTreeWidgetItem::init()
{
	switch(m_type)
	{
		case Item:
			setText(1, __tr2qs_ctx("Item", "editor"));
			break;
		case Menu:
			setText(1, __tr2qs_ctx("Submenu", "editor"));
			break;
		case ExtMenu:
			setText(1, __tr2qs_ctx("External Menu", "editor"));
			break;
		case Separator:
			setText(0, "-----------------------");
			setText(1, __tr2qs_ctx("Separator", "editor"));
			break;
		case Label:
			setText(1, __tr2qs_ctx("Label", "editor"));
			break;
		case Epilogue:
			setText(0, __tr2qs_ctx("### Epilogue ###", "editor"));
			setText(1, __tr2qs_ctx("Epilogue", "editor"));
			break;
		case Prologue:
			setText(0, __tr2qs_ctx("### Prologue ###", "editor"));
			setText(1, __tr2qs_ctx("Prologue", "editor"));
			break;
		default:
			break;
	}
}

void PopupTreeWidgetItem::setItemText(const QString & szText)
{
	switch(m_type)
	{
		case Item:
		case Menu:
		case Label:
		case ExtMenu:
			m_szText = szText;
			setText(0, szText);
			break;
		default:
			break;
	}
}

void PopupTreeWidgetItem::setCondition(const QString & szCondition)
{
	switch(m_type)
	{
		case Item:
		case Menu:
		case Label:
		case ExtMenu:
		case Separator:
			m_szCondition = szCondition;
			break;
		default:
			break;
	}
}

void PopupTreeWidgetItem::setCode(const QString & szCode)
{
	switch(m_type)
	{
		case Item:
		case Epilogue:
		case Prologue:
		case ExtMenu:
			m_szCode = szCode;
			break;
		default:
			break;
	}
}

void PopupTreeWidgetItem::setId(const QString & szId)
{
	m_szId = szId;
}

void PopupTreeWidgetItem::setIcon(const QString & szIcon)
{
	switch(m_type)
	{
		case Item:
		case Menu:
		case Label:
		case ExtMenu:
			m_szIcon = szIcon;
			{
				QPixmap * pix = g_pIconManager->getImage(szIcon);
				if(pix)
					QTreeWidgetItem::setIcon(0, QIcon(*pix));
				else
					QTreeWidgetItem::setIcon(0, QIcon());
			}
			break;
		default:
			break;
	}
}

SinglePopupEditor::SinglePopupEditor(QWidget * par)
    : QWidget(par)
{
	m_pLastSelectedItem = nullptr;
	m_pContextPopup = new QMenu(this);
	m_pClipboard = nullptr;
	m_pTestPopup = nullptr;

	QGridLayout * g = new QGridLayout(this);
	g->setMargin(0);
	g->setSpacing(2);

	m_pNameEditor = new QLineEdit(this);
	m_pNameEditor->setToolTip(__tr2qs_ctx("Popup name", "editor"));

	g->addWidget(m_pNameEditor, 0, 0, 1, 2);

	m_pMenuButton = new QPushButton(__tr2qs_ctx("Test", "editor"), this);
	g->addWidget(m_pMenuButton, 0, 2);
	connect(m_pMenuButton, SIGNAL(clicked()), this, SLOT(testPopup()));
	QSplitter * spl = new QSplitter(Qt::Vertical, this);
	spl->setObjectName("popupeditor_vertical_splitter");
	spl->setChildrenCollapsible(false);

	m_pTreeWidget = new QTreeWidget(spl);
	m_pTreeWidget->setColumnCount(2);
	QStringList labels;
	labels << __tr2qs_ctx("Item", "editor") << __tr2qs_ctx("Type", "editor");
	m_pTreeWidget->setHeaderLabels(labels);
	m_pTreeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pTreeWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pTreeWidget->setAllColumnsShowFocus(true);
	m_pTreeWidget->setRootIsDecorated(true);
	m_pTreeWidget->header()->setSortIndicatorShown(false);
	m_pTreeWidget->setSortingEnabled(false);
	m_pTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(m_pTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
	connect(m_pTreeWidget, SIGNAL(customContextMenuRequested(const QPoint &)),
	    this, SLOT(customContextMenuRequested(const QPoint &)));

	m_pEditor = KviScriptEditor::createInstance(spl);

	g->addWidget(spl, 1, 0, 1, 3);

	QLabel * l = new QLabel(__tr2qs_ctx("Text:", "editor"), this);
	g->addWidget(l, 2, 0);

	m_pTextEditor = new QLineEdit(this);
	m_pTextEditor->setToolTip(
	    __tr2qs_ctx("<b>Visible text</b><br>May contain identifiers that will be evaluated at popup call time.<br>For labels, this text can contain also limited HTML tags.", "editor"));
	g->addWidget(m_pTextEditor, 2, 1, 1, 2);

	l = new QLabel(__tr2qs_ctx("Condition:", "editor"), this);
	l->setMargin(2);
	g->addWidget(l, 3, 0);

	m_pConditionEditor = new QLineEdit(this);
	m_pConditionEditor->setToolTip(
	    __tr2qs_ctx("<b>Boolean condition</b><br>Will be evaluated at popup call time in order to decide if this entry has to be shown.<br>An empty condition evaluates to true.", "editor"));
	g->addWidget(m_pConditionEditor, 3, 1, 1, 2);

	l = new QLabel(__tr2qs_ctx("Icon:", "editor"), this);
	l->setMargin(2);
	g->addWidget(l, 4, 0);

	m_pIconEditor = new QLineEdit(this);
	m_pIconEditor->setToolTip(
	    __tr2qs_ctx("<b>Icon identifier</b><br>May be an internal icon ID, an absolute path or a relative path.<br>Portable scripts should never use absolute paths.", "editor"));
	g->addWidget(m_pIconEditor, 4, 1, 1, 2);

	l = new QLabel(__tr2qs_ctx("External menu:", "editor"), this);
	l->setMargin(2);
	g->addWidget(l, 5, 0);

	m_pExtNameEditor = new QLineEdit(this);
	m_pExtNameEditor->setToolTip(
	    __tr2qs_ctx("<b>External menu name</b><br>This allows one to nest externally defined popup menus. The popup menu with the specified name will be looked up at menu setup time.", "editor"));
	g->addWidget(m_pExtNameEditor, 5, 1, 1, 2);

	l = new QLabel(__tr2qs_ctx("Item ID:", "editor"), this);
	l->setMargin(2);
	g->addWidget(l, 6, 0);

	m_pIdEditor = new QLineEdit(this);
	m_pIdEditor->setToolTip(
	    __tr2qs_ctx("<b>Item ID</b><br>This will allow you to use delpopupitem later.", "editor"));
	g->addWidget(m_pIdEditor, 6, 1, 1, 2);
	g->setColumnStretch(1, 1);
	g->setRowStretch(1, 1);
}

SinglePopupEditor::~SinglePopupEditor()
{
	delete m_pClipboard;
	delete m_pTestPopup;
	KviScriptEditor::destroyInstance(m_pEditor);
}

void SinglePopupEditor::testPopup()
{

	if(m_pTestPopup)
		delete m_pTestPopup;
	m_pTestPopup = getMenu();
	if(!m_pTestPopup)
		return;
	connect(m_pTestPopup, SIGNAL(testModeItemClicked(KviKvsPopupMenuItem *)), this, SLOT(testModeMenuItemClicked(KviKvsPopupMenuItem *)));
	QPoint pnt = m_pMenuButton->mapToGlobal(QPoint(0, m_pMenuButton->height()));
	KviKvsVariantList * parms = new KviKvsVariantList();
	parms->append(new KviKvsVariant(QString("test1")));
	parms->append(new KviKvsVariant(QString("test2")));
	parms->append(new KviKvsVariant(QString("test3")));
	parms->append(new KviKvsVariant(QString("test4")));
	m_pTestPopup->doPopup(pnt, g_pActiveWindow, parms, true);
}

PopupTreeWidgetItem * SinglePopupEditor::findMatchingItem(KviKvsPopupMenuItem * it, PopupTreeWidgetItem * item)
{

	if(it->type() != KviKvsPopupMenuItem::Item)
		goto not_this_one;
	if(item->m_type != PopupTreeWidgetItem::Item)
		goto not_this_one;
	if(it->name() != item->m_szId)
		goto not_this_one;
	if(it->kvsText())
	{
		if(it->kvsText()->code() != item->m_szText)
			goto not_this_one;
	}
	else
	{
		if(!item->m_szText.isEmpty())
			goto not_this_one;
	}
	if(it->kvsCode())
	{
		if(it->kvsCode()->code() != item->m_szCode)
			goto not_this_one;
	}
	else
	{
		if(!item->m_szCode.isEmpty())
			goto not_this_one;
	}
	if(it->kvsIcon())
	{
		if(it->kvsIcon()->code() != item->m_szIcon)
			goto not_this_one;
	}
	else
	{
		if(!item->m_szIcon.isEmpty())
			goto not_this_one;
	}
	if(it->kvsCondition())
	{
		if(it->kvsCondition()->code() != item->m_szCondition)
			goto not_this_one;
	}
	else
	{
		if(!item->m_szCondition.isEmpty())
			goto not_this_one;
	}

	return item;

not_this_one:
	int count = item->childCount();
	for(int i = 0; i < count; i++)
	{
		PopupTreeWidgetItem * testItem = (PopupTreeWidgetItem *)item->child(i);
		KVI_ASSERT(testItem);
		PopupTreeWidgetItem * found = findMatchingItem(it, testItem);
		if(found)
			return found;
	}

	return nullptr;
}

void SinglePopupEditor::testModeMenuItemClicked(KviKvsPopupMenuItem * it)
{

	saveLastSelectedItem(); // that's the first thingie
	// find the matching item and set it as current
	int count = m_pTreeWidget->topLevelItemCount();
	for(int i = 0; i < count; i++)
	{
		PopupTreeWidgetItem * item = (PopupTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		PopupTreeWidgetItem * found = findMatchingItem(it, item);
		if(found)
		{
			// set the item as current
			m_pTreeWidget->scrollToItem(found);
			m_pTreeWidget->setCurrentItem(found);
			found->setSelected(true);
			selectionChanged();
			return;
		}
	}
}

void SinglePopupEditor::customContextMenuRequested(const QPoint & pos)
{
	QTreeWidgetItem * it = m_pTreeWidget->itemAt(pos);

	m_pContextPopup->clear();

	bool bIsMenu = false;

	if(it)
	{
		bIsMenu = ((PopupTreeWidgetItem *)it)->m_type == PopupTreeWidgetItem::Menu;
	}

	m_pContextPopup->addAction(__tr2qs_ctx("New Separator Below", "editor"), this, SLOT(contextNewSeparatorBelow()));
	m_pContextPopup->addAction(__tr2qs_ctx("New Separator Above", "editor"), this, SLOT(contextNewSeparatorAbove()))
	    ->setEnabled(it);
	m_pContextPopup->addAction(__tr2qs_ctx("New Separator Inside", "editor"), this, SLOT(contextNewSeparatorInside()))
	    ->setEnabled(it && bIsMenu);

	m_pContextPopup->addSeparator();

	m_pContextPopup->addAction(__tr2qs_ctx("New Label Below", "editor"), this, SLOT(contextNewLabelBelow()));
	m_pContextPopup->addAction(__tr2qs_ctx("New Label Above", "editor"), this, SLOT(contextNewLabelAbove()))
	    ->setEnabled(it);
	m_pContextPopup->addAction(__tr2qs_ctx("New Label Inside", "editor"), this, SLOT(contextNewLabelInside()))
	    ->setEnabled(it && bIsMenu);

	m_pContextPopup->addSeparator();

	m_pContextPopup->addAction(__tr2qs_ctx("New Item Below", "editor"), this, SLOT(contextNewItemBelow()));
	m_pContextPopup->addAction(__tr2qs_ctx("New Item Above", "editor"), this, SLOT(contextNewItemAbove()))
	    ->setEnabled(it);
	m_pContextPopup->addAction(__tr2qs_ctx("New Item Inside", "editor"), this, SLOT(contextNewItemInside()))
	    ->setEnabled(it && bIsMenu);

	m_pContextPopup->addSeparator();

	m_pContextPopup->addAction(__tr2qs_ctx("New Menu Below", "editor"), this, SLOT(contextNewMenuBelow()));
	m_pContextPopup->addAction(__tr2qs_ctx("New Menu Above", "editor"), this, SLOT(contextNewMenuAbove()))
	    ->setEnabled(it);
	m_pContextPopup->addAction(__tr2qs_ctx("New Menu Inside", "editor"), this, SLOT(contextNewMenuInside()))
	    ->setEnabled(it && bIsMenu);

	m_pContextPopup->addSeparator();

	m_pContextPopup->addAction(__tr2qs_ctx("New External Menu Below", "editor"), this, SLOT(contextNewExtMenuBelow()));
	m_pContextPopup->addAction(__tr2qs_ctx("New External Menu Above", "editor"), this, SLOT(contextNewExtMenuAbove()))
	    ->setEnabled(it);
	m_pContextPopup->addAction(__tr2qs_ctx("New External Menu Inside", "editor"), this, SLOT(contextNewExtMenuInside()))
	    ->setEnabled(it && bIsMenu);

	m_pContextPopup->addSeparator();

	m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Cut)),
	    __tr2qs_ctx("Cu&t", "editor"), this, SLOT(contextCut()))
	    ->setEnabled(it);
	m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Copy)),
	    __tr2qs_ctx("&Copy", "editor"), this, SLOT(contextCopy()))
	    ->setEnabled(it);
	m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Discard)),
	    __tr2qs_ctx("Re&move", "editor"), this, SLOT(contextRemove()))
	    ->setEnabled(it);
	m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Paste)),
	    __tr2qs_ctx("&Paste Below", "editor"), this, SLOT(contextPasteBelow()))
	    ->setEnabled(m_pClipboard);
	m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Paste)),
	    __tr2qs_ctx("Paste Above", "editor"), this, SLOT(contextPasteAbove()))
	    ->setEnabled(it && m_pClipboard);
	m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Paste)),
	    __tr2qs_ctx("Paste Inside", "editor"), this, SLOT(contextPasteInside()))
	    ->setEnabled(it && bIsMenu && m_pClipboard);

	m_pContextPopup->addSeparator();
	m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Prologue)),
	    __tr2qs_ctx("New Menu Prologue", "editor"), this, SLOT(contextNewPrologue()));

	m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Epilogue)),
	    __tr2qs_ctx("New Menu Epilogue", "editor"), this, SLOT(contextNewEpilogue()));

	m_pContextPopup->popup(QCursor::pos());
}

void SinglePopupEditor::createNewItemAboveLastSelected(PopupTreeWidgetItem::Type t)
{
	m_pTreeWidget->setCurrentItem(newItemAbove(m_pLastSelectedItem, t));
}

void SinglePopupEditor::createNewItemBelowLastSelected(PopupTreeWidgetItem::Type t)
{
	m_pTreeWidget->setCurrentItem(newItemBelow(m_pLastSelectedItem, t));
}

void SinglePopupEditor::createNewItemInsideLastSelected(PopupTreeWidgetItem::Type t)
{
	if(m_pLastSelectedItem)
		m_pLastSelectedItem->setExpanded(true);
	m_pTreeWidget->setCurrentItem(newItemInside(m_pLastSelectedItem, t));
}

void SinglePopupEditor::contextNewSeparatorAbove()
{
	createNewItemAboveLastSelected(PopupTreeWidgetItem::Separator);
}

void SinglePopupEditor::contextNewSeparatorBelow()
{
	createNewItemBelowLastSelected(PopupTreeWidgetItem::Separator);
}

void SinglePopupEditor::contextNewSeparatorInside()
{
	createNewItemInsideLastSelected(PopupTreeWidgetItem::Separator);
}

void SinglePopupEditor::contextNewItemBelow()
{
	createNewItemBelowLastSelected(PopupTreeWidgetItem::Item);
}

void SinglePopupEditor::contextNewItemAbove()
{
	createNewItemAboveLastSelected(PopupTreeWidgetItem::Item);
}

void SinglePopupEditor::contextNewItemInside()
{
	createNewItemInsideLastSelected(PopupTreeWidgetItem::Item);
}

void SinglePopupEditor::contextNewMenuBelow()
{
	createNewItemBelowLastSelected(PopupTreeWidgetItem::Menu);
}

void SinglePopupEditor::contextNewMenuAbove()
{
	createNewItemAboveLastSelected(PopupTreeWidgetItem::Menu);
}

void SinglePopupEditor::contextNewMenuInside()
{
	createNewItemInsideLastSelected(PopupTreeWidgetItem::Menu);
}

void SinglePopupEditor::contextNewExtMenuBelow()
{
	createNewItemBelowLastSelected(PopupTreeWidgetItem::ExtMenu);
}

void SinglePopupEditor::contextNewExtMenuAbove()
{
	createNewItemAboveLastSelected(PopupTreeWidgetItem::ExtMenu);
}
void SinglePopupEditor::contextNewExtMenuInside()
{
	createNewItemInsideLastSelected(PopupTreeWidgetItem::ExtMenu);
}

void SinglePopupEditor::contextNewLabelBelow()
{
	createNewItemBelowLastSelected(PopupTreeWidgetItem::Label);
}

void SinglePopupEditor::contextNewLabelAbove()
{
	createNewItemAboveLastSelected(PopupTreeWidgetItem::Label);
}

void SinglePopupEditor::contextNewLabelInside()
{
	createNewItemInsideLastSelected(PopupTreeWidgetItem::Label);
}

PopupTreeWidgetItem * SinglePopupEditor::newItem(PopupTreeWidgetItem * par, PopupTreeWidgetItem * after, PopupTreeWidgetItem::Type t)
{
	if(par)
		return new PopupTreeWidgetItem(par, after, t);
	return new PopupTreeWidgetItem(m_pTreeWidget, after, t);
}

PopupTreeWidgetItem * SinglePopupEditor::newItemBelow(PopupTreeWidgetItem * it, PopupTreeWidgetItem::Type t)
{
	if(!it)
		return newItem(nullptr, nullptr, t);
	return newItem((PopupTreeWidgetItem *)it->parent(), it, t);
}

PopupTreeWidgetItem * SinglePopupEditor::newItemAbove(PopupTreeWidgetItem * it, PopupTreeWidgetItem::Type t)
{
	if(!it)
		return newItem(nullptr, nullptr, t);
	return newItem((PopupTreeWidgetItem *)it->parent(), (PopupTreeWidgetItem *)m_pTreeWidget->itemAbove(it), t);
}

PopupTreeWidgetItem * SinglePopupEditor::newItemInside(PopupTreeWidgetItem * it, PopupTreeWidgetItem::Type t)
{
	if(it && it->m_type != PopupTreeWidgetItem::Menu)
		return newItemBelow(it, t);
	return newItem(it, it, t);
}

void SinglePopupEditor::contextNewPrologue()
{
	PopupTreeWidgetItem * it = m_pLastSelectedItem ? (PopupTreeWidgetItem *)m_pLastSelectedItem->parent() : nullptr;
	m_pTreeWidget->setCurrentItem(newItem(it, it, PopupTreeWidgetItem::Prologue));
}

void SinglePopupEditor::contextNewEpilogue()
{
	PopupTreeWidgetItem * it = m_pLastSelectedItem ? (PopupTreeWidgetItem *)m_pLastSelectedItem->parent() : nullptr;
	PopupTreeWidgetItem * after = it ? (PopupTreeWidgetItem *)it->child(0) : (PopupTreeWidgetItem *)m_pTreeWidget->topLevelItem(0);
	if(after)
	{
		while(m_pTreeWidget->itemAbove(after))
		{
			if(after->parent() == m_pTreeWidget->itemAbove(after)->parent())
				after = (PopupTreeWidgetItem *)m_pTreeWidget->itemAbove(after);
		}
	}
	else
	{
		after = it;
	}
	m_pTreeWidget->setCurrentItem(newItem(it, after, PopupTreeWidgetItem::Epilogue));
}

void SinglePopupEditor::contextCopy()
{
	if(!m_pLastSelectedItem)
		return;
	delete m_pClipboard;
	m_pClipboard = new KviKvsPopupMenu("clipboard");
	addItemToMenu(m_pClipboard, m_pLastSelectedItem);
}

void SinglePopupEditor::contextCut()
{
	if(!m_pLastSelectedItem)
		return;

	saveLastSelectedItem();
	contextCopy();

	auto * lastSelectedCache = m_pLastSelectedItem;

	m_pTreeWidget->selectionModel()->clearSelection();
	selectionChanged();
	delete lastSelectedCache;
}

void SinglePopupEditor::contextRemove()
{
	if(!m_pLastSelectedItem)
		return;

	auto * lastSelectedCache = m_pLastSelectedItem;

	m_pTreeWidget->selectionModel()->clearSelection();
	selectionChanged();
	delete lastSelectedCache;
}

void SinglePopupEditor::contextPasteBelow()
{
	if(!m_pClipboard)
		return;

	PopupTreeWidgetItem * par = m_pLastSelectedItem ? (PopupTreeWidgetItem *)m_pLastSelectedItem->parent() : nullptr;
	populateMenu(m_pClipboard, par, m_pLastSelectedItem);
}

void SinglePopupEditor::contextPasteAbove()
{
	if(!m_pClipboard)
		return;

	PopupTreeWidgetItem * par = m_pLastSelectedItem ? (PopupTreeWidgetItem *)m_pLastSelectedItem->parent() : nullptr;
	PopupTreeWidgetItem * above = m_pLastSelectedItem ? (PopupTreeWidgetItem *)m_pTreeWidget->itemAbove(m_pLastSelectedItem) : nullptr;
	populateMenu(m_pClipboard, par, above);
}

void SinglePopupEditor::contextPasteInside()
{
	if(!m_pClipboard)
		return;

	if(m_pLastSelectedItem)
	{
		if(m_pLastSelectedItem->m_type != PopupTreeWidgetItem::Menu)
		{
			contextPasteBelow();
			return;
		}
		m_pLastSelectedItem->setExpanded(true);
	}
	populateMenu(m_pClipboard, m_pLastSelectedItem, nullptr);
}

void SinglePopupEditor::saveLastSelectedItem()
{
	if(!m_pLastSelectedItem)
		return;

	QString tmp;

	switch(m_pLastSelectedItem->m_type)
	{
		case PopupTreeWidgetItem::Prologue:
		case PopupTreeWidgetItem::Epilogue:
		case PopupTreeWidgetItem::Item:
		{
			QString tmpx;
			m_pEditor->getText(tmpx);
			m_pLastSelectedItem->setCode(tmpx);
		}
		break;
		default:
			break;
	}

	m_pLastSelectedItem->setId(m_pIdEditor->text());

	switch(m_pLastSelectedItem->m_type)
	{
		case PopupTreeWidgetItem::Menu:
		case PopupTreeWidgetItem::ExtMenu:
		case PopupTreeWidgetItem::Item:
			m_pLastSelectedItem->setIcon(m_pIconEditor->text());
			break;
		default:
			break;
	}

	switch(m_pLastSelectedItem->m_type)
	{
		case PopupTreeWidgetItem::Menu:
		case PopupTreeWidgetItem::Item:
		case PopupTreeWidgetItem::Label:
		case PopupTreeWidgetItem::ExtMenu:
			m_pLastSelectedItem->setItemText(m_pTextEditor->text());
			break;
		default:
			break;
	}

	switch(m_pLastSelectedItem->m_type)
	{
		case PopupTreeWidgetItem::Menu:
		case PopupTreeWidgetItem::Item:
		case PopupTreeWidgetItem::Label:
		case PopupTreeWidgetItem::ExtMenu:
		case PopupTreeWidgetItem::Separator:
			m_pLastSelectedItem->setCondition(m_pConditionEditor->text());
			break;
		default:
			break;
	}

	if(m_pLastSelectedItem->m_type == PopupTreeWidgetItem::ExtMenu)
	{
		m_pLastSelectedItem->setCode(m_pExtNameEditor->text());
	}
}

void SinglePopupEditor::addItemToMenu(KviKvsPopupMenu * p, PopupTreeWidgetItem * it)
{
	it->m_szId = it->m_szId.trimmed();
	switch(it->m_type)
	{
		case PopupTreeWidgetItem::Prologue:
			it->m_szCode = it->m_szCode.trimmed();
			p->addPrologue(it->m_szId, it->m_szCode);
			break;
		case PopupTreeWidgetItem::Epilogue:
			it->m_szCode = it->m_szCode.trimmed();
			p->addEpilogue(it->m_szId, it->m_szCode);
			break;
		case PopupTreeWidgetItem::Separator:
			it->m_szCondition = it->m_szCondition.trimmed();
			p->addSeparator(it->m_szId, it->m_szCondition);
			break;
		case PopupTreeWidgetItem::Label:
			it->m_szText = it->m_szText.trimmed();
			it->m_szCondition = it->m_szCondition.trimmed();
			it->m_szIcon = it->m_szIcon.trimmed();
			p->addLabel(it->m_szId, it->m_szText, it->m_szIcon, it->m_szCondition);
			break;
		case PopupTreeWidgetItem::Item:
			it->m_szText = it->m_szText.trimmed();
			it->m_szIcon = it->m_szIcon.trimmed();
			it->m_szCondition = it->m_szCondition.trimmed();
			it->m_szCode = it->m_szCode.trimmed();
			p->addItem(it->m_szId, it->m_szCode, it->m_szText, it->m_szIcon, it->m_szCondition);
			break;
		case PopupTreeWidgetItem::ExtMenu:
			it->m_szText = it->m_szText.trimmed();
			it->m_szIcon = it->m_szIcon.trimmed();
			it->m_szCondition = it->m_szCondition.trimmed();
			it->m_szCode = it->m_szCode.trimmed(); // <-- this is the ext name in fact
			p->addExtPopup(it->m_szId, it->m_szCode, it->m_szText, it->m_szIcon, it->m_szCondition);
			break;
		case PopupTreeWidgetItem::Menu:
		{
			it->m_szText = it->m_szText.trimmed();
			it->m_szIcon = it->m_szIcon.trimmed();
			it->m_szCondition = it->m_szCondition.trimmed();
			KviKvsPopupMenu * menu = p->addPopup(it->m_szId, it->m_szText, it->m_szIcon, it->m_szCondition);
			int count = it->childCount();
			for(int i = 0; i < count; i++)
			{
				addItemToMenu(menu, (PopupTreeWidgetItem *)it->child(i));
			}
		}
		break;
		default:
			break;
	}
}

KviKvsPopupMenu * SinglePopupEditor::getMenu()
{
	saveLastSelectedItem();

	QString tmp = m_pNameEditor->text().trimmed();

	KviKvsPopupMenu * p = new KviKvsPopupMenu(tmp);

	int count = m_pTreeWidget->topLevelItemCount();
	for(int i = 0; i < count; i++)
	{
		addItemToMenu(p, (PopupTreeWidgetItem *)m_pTreeWidget->topLevelItem(i));
	}
	return p;
}

void SinglePopupEditor::selectionChanged()
{
	saveLastSelectedItem();

	bool bEditorEnabled = false;
	bool bIconEditorEnabled = false;
	bool bConditionEditorEnabled = false;
	bool bTextEditorEnabled = false;
	bool bNameEditorEnabled = false;

	QTreeWidgetItem * it;

	if(m_pTreeWidget->selectedItems().empty())
	{
		it = nullptr;
	}
	else
	{
		it = (QTreeWidgetItem *)m_pTreeWidget->selectedItems().first();
	}

	if(it)
	{
		m_pIdEditor->setText(((PopupTreeWidgetItem *)it)->m_szId);

		switch(((PopupTreeWidgetItem *)it)->m_type)
		{
			case PopupTreeWidgetItem::Prologue:
			case PopupTreeWidgetItem::Epilogue:
			case PopupTreeWidgetItem::Item:
				m_pEditor->setText(((PopupTreeWidgetItem *)it)->m_szCode);
				bEditorEnabled = true;
				break;
			default:
				break;
		}

		switch(((PopupTreeWidgetItem *)it)->m_type)
		{
			case PopupTreeWidgetItem::Menu:
			case PopupTreeWidgetItem::Item:
			case PopupTreeWidgetItem::Label:
			case PopupTreeWidgetItem::ExtMenu:
				m_pIconEditor->setText(((PopupTreeWidgetItem *)it)->m_szIcon);
				bIconEditorEnabled = true;
				break;
			default:
				break;
		}

		switch(((PopupTreeWidgetItem *)it)->m_type)
		{
			case PopupTreeWidgetItem::Menu:
			case PopupTreeWidgetItem::Item:
			case PopupTreeWidgetItem::Label:
			case PopupTreeWidgetItem::ExtMenu:
				m_pTextEditor->setText(((PopupTreeWidgetItem *)it)->m_szText);
				bTextEditorEnabled = true;
				break;
			default:
				break;
		}

		switch(((PopupTreeWidgetItem *)it)->m_type)
		{
			case PopupTreeWidgetItem::Menu:
			case PopupTreeWidgetItem::Item:
			case PopupTreeWidgetItem::Label:
			case PopupTreeWidgetItem::ExtMenu:
			case PopupTreeWidgetItem::Separator:
				m_pConditionEditor->setText(((PopupTreeWidgetItem *)it)->m_szCondition);
				bConditionEditorEnabled = true;
				break;
			default:
				break;
		}

		if(((PopupTreeWidgetItem *)it)->m_type == PopupTreeWidgetItem::ExtMenu)
		{
			m_pExtNameEditor->setText(((PopupTreeWidgetItem *)it)->m_szCode);
			bNameEditorEnabled = true;
		}
	}

	m_pLastSelectedItem = (PopupTreeWidgetItem *)it;

	if(!bEditorEnabled)
		m_pEditor->setText("");
	m_pEditor->setEnabled(bEditorEnabled);
	if(!bIconEditorEnabled)
		m_pIconEditor->setText("");
	m_pIconEditor->setEnabled(bIconEditorEnabled);
	if(!bConditionEditorEnabled)
		m_pConditionEditor->setText("");
	m_pConditionEditor->setEnabled(bConditionEditorEnabled);
	if(!bTextEditorEnabled)
		m_pTextEditor->setText("");
	m_pTextEditor->setEnabled(bTextEditorEnabled);
	m_pExtNameEditor->setEnabled(bNameEditorEnabled);
	if(!bNameEditorEnabled)
		m_pExtNameEditor->setText("");
	if(!it)
		m_pIdEditor->setText("");
	m_pIdEditor->setEnabled(it);
}

void SinglePopupEditor::populateMenu(KviKvsPopupMenu * pop, PopupTreeWidgetItem * par, PopupTreeWidgetItem * theItem)
{
	if(!pop)
		return;

	for(KviKvsScript * sp = pop->prologues()->first(); sp; sp = pop->prologues()->next())
	{
		if(par)
			theItem = new PopupTreeWidgetItem(par, theItem, PopupTreeWidgetItem::Prologue);
		else
			theItem = new PopupTreeWidgetItem(m_pTreeWidget, theItem, PopupTreeWidgetItem::Prologue);
		theItem->setCode(sp->code());
		theItem->setId(sp->name());
	}

	for(KviKvsPopupMenuItem * item = pop->itemList()->first(); item; item = pop->itemList()->next())
	{
		switch(item->type())
		{
			case KviKvsPopupMenuItem::Item:
				if(par)
					theItem = new PopupTreeWidgetItem(par, theItem, PopupTreeWidgetItem::Item);
				else
					theItem = new PopupTreeWidgetItem(m_pTreeWidget, theItem, PopupTreeWidgetItem::Item);
				theItem->setIcon(item->kvsIcon() ? item->kvsIcon()->code() : QString());
				theItem->setItemText(item->kvsText() ? item->kvsText()->code() : QString());
				theItem->setCondition(item->kvsCondition() ? item->kvsCondition()->code() : QString());
				theItem->setCode(item->kvsCode() ? item->kvsCode()->code() : QString());
				theItem->setId(item->name());
				break;
			case KviKvsPopupMenuItem::ExtMenu:
				if(par)
					theItem = new PopupTreeWidgetItem(par, theItem, PopupTreeWidgetItem::ExtMenu);
				else
					theItem = new PopupTreeWidgetItem(m_pTreeWidget, theItem, PopupTreeWidgetItem::ExtMenu);
				theItem->setIcon(item->kvsIcon() ? item->kvsIcon()->code() : QString());
				theItem->setItemText(item->kvsText() ? item->kvsText()->code() : QString());
				theItem->setCondition(item->kvsCondition() ? item->kvsCondition()->code() : QString());
				theItem->setCode(((KviKvsPopupMenuItemExtMenu *)item)->extName());
				theItem->setId(item->name());
				break;
			case KviKvsPopupMenuItem::Label:
				if(par)
					theItem = new PopupTreeWidgetItem(par, theItem, PopupTreeWidgetItem::Label);
				else
					theItem = new PopupTreeWidgetItem(m_pTreeWidget, theItem, PopupTreeWidgetItem::Label);
				theItem->setIcon(item->kvsIcon() ? item->kvsIcon()->code() : QString());
				theItem->setItemText(item->kvsText() ? item->kvsText()->code() : QString());
				theItem->setCondition(item->kvsCondition() ? item->kvsCondition()->code() : QString());
				theItem->setId(item->name());
				break;
			case KviKvsPopupMenuItem::Separator:
				if(par)
					theItem = new PopupTreeWidgetItem(par, theItem, PopupTreeWidgetItem::Separator);
				else
					theItem = new PopupTreeWidgetItem(m_pTreeWidget, theItem, PopupTreeWidgetItem::Separator);
				theItem->setCondition(item->kvsCondition() ? item->kvsCondition()->code() : QString());
				theItem->setId(item->name());
				break;
			case KviKvsPopupMenuItem::Menu:
				if(par)
					theItem = new PopupTreeWidgetItem(par, theItem, PopupTreeWidgetItem::Menu);
				else
					theItem = new PopupTreeWidgetItem(m_pTreeWidget, theItem, PopupTreeWidgetItem::Menu);
				theItem->setIcon(item->kvsIcon() ? item->kvsIcon()->code() : QString());
				theItem->setItemText(item->kvsText() ? item->kvsText()->code() : QString());
				theItem->setCondition(item->kvsCondition() ? item->kvsCondition()->code() : QString());
				theItem->setId(item->name());
				populateMenu(((KviKvsPopupMenuItemMenu *)item)->menu(), theItem, nullptr);
				break;
			default:
				break;
		}
	}

	for(KviKvsScript * se = pop->epilogues()->first(); se; se = pop->epilogues()->next())
	{
		if(par)
			theItem = new PopupTreeWidgetItem(par, theItem, PopupTreeWidgetItem::Epilogue);
		else
			theItem = new PopupTreeWidgetItem(m_pTreeWidget, theItem, PopupTreeWidgetItem::Epilogue);
		theItem->setCode(se->code());
		theItem->setId(se->name());
	}

	m_pTreeWidget->resizeColumnToContents(0);
}

void SinglePopupEditor::edit(MenuTreeWidgetItem * it)
{
	saveLastSelectedItem();

	m_pLastSelectedItem = nullptr;

	m_pTreeWidget->clear();

	selectionChanged();

	if(it)
	{
		m_pNameEditor->setText(it->m_pPopup->popupName());
		populateMenu(it->m_pPopup, nullptr, nullptr);
	}
	else
	{
		m_pIconEditor->setText("");
		m_pIconEditor->setEnabled(false);
		m_pIdEditor->setText("");
		m_pIdEditor->setEnabled(false);
		m_pConditionEditor->setText("");
		m_pConditionEditor->setEnabled(false);
		m_pTextEditor->setText("");
		m_pTextEditor->setEnabled(false);
		m_pEditor->setText("");
		m_pEditor->setEnabled(false);
		m_pNameEditor->setText("");
		m_pExtNameEditor->setText("");
		m_pExtNameEditor->setEnabled(false);
	}
	m_pTreeWidget->setEnabled(it);
	m_pNameEditor->setEnabled(it);
	m_pMenuButton->setEnabled(it);
}

MenuTreeWidgetItem::MenuTreeWidgetItem(QTreeWidget * par, KviKvsPopupMenu * popup)
    : QTreeWidgetItem(par)
{
	setIcon(0, *(g_pIconManager->getSmallIcon(KviIconManager::Popup)));
	setText(0, popup->popupName());
	m_pPopup = popup;
}

MenuTreeWidgetItem::~MenuTreeWidgetItem()
{
	delete m_pPopup;
}

void MenuTreeWidgetItem::replacePopup(KviKvsPopupMenu * popup)
{
	delete m_pPopup;
	m_pPopup = popup;
}

PopupEditorWidget::PopupEditorWidget(QWidget * par)
    : QWidget(par)
{
	m_bSaving = false;

	QGridLayout * l = new QGridLayout(this);
	QSplitter * spl = new QSplitter(Qt::Horizontal, this);
	spl->setObjectName("popupeditor_horizontal_splitter");
	spl->setChildrenCollapsible(false);
	l->addWidget(spl, 0, 0);

	KviTalVBox * box = new KviTalVBox(spl);

	m_pTreeWidget = new QTreeWidget(box);
	m_pTreeWidget->setHeaderLabel(__tr2qs_ctx("Popup", "editor"));
	m_pTreeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_pTreeWidget->header()->setSortIndicatorShown(true);

	QPushButton * pb = new QPushButton(__tr2qs_ctx("&Export All to...", "editor"), box);
	connect(pb, SIGNAL(clicked()), this, SLOT(exportAll()));
	QPushButton * gn = new QPushButton(__tr2qs_ctx("&Export Selected to...", "editor"), box);
	connect(gn, SIGNAL(clicked()), this, SLOT(exportSelected()));

	m_pEditor = new SinglePopupEditor(spl);

	m_bOneTimeSetupDone = false;
	m_pLastEditedItem = nullptr;

	m_pContextPopup = new QMenu(this);
	m_pEmptyContextPopup = new QMenu(this);

	spl->setStretchFactor(0, 20);
	spl->setStretchFactor(1, 80);

	currentItemChanged(nullptr, nullptr);
}

void PopupEditorWidget::oneTimeSetup()
{
	if(m_bOneTimeSetupDone)
		return;
	m_bOneTimeSetupDone = true;

	const KviPointerHashTable<QString, KviKvsPopupMenu> * a = KviKvsPopupManager::instance()->popupDict();
	if(!a)
		return;

	KviPointerHashTableIterator<QString, KviKvsPopupMenu> it(*a);

	while(it.current())
	{
		KviKvsPopupMenu * popup = it.current();
		KviKvsPopupMenu * copy = new KviKvsPopupMenu(popup->popupName());
		copy->copyFrom(popup);
		new MenuTreeWidgetItem(m_pTreeWidget, copy);
		++it;
	}

	m_pTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_pTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));
	connect(m_pTreeWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(customContextMenuRequested(const QPoint &)));
	connect(KviKvsPopupManager::instance(), SIGNAL(popupRefresh(const QString &)), this, SLOT(popupRefresh(const QString &)));
}

void PopupEditorWidget::popupRefresh(const QString & szName)
{
	if(m_bSaving)
		return;

	KviPointerHashTable<QString, KviKvsPopupMenu> * pTable = KviKvsPopupManager::instance()->popupDict();
	if(!pTable)
		return;
	KviKvsPopupMenu * pPopup = pTable->find(szName);
	if(!pPopup)
		return;

	// if it already exists, replace its popup
	for(int i = 0; i < m_pTreeWidget->topLevelItemCount(); i++)
	{
		MenuTreeWidgetItem * ch = (MenuTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);

		if(KviQString::equalCI(szName, ch->m_pPopup->popupName()))
		{
			if(ch == m_pLastEditedItem)
			{
				if(
				    QMessageBox::warning(nullptr, __tr2qs_ctx("Confirm Overwriting Current - KVIrc", "editor"),
				        __tr2qs_ctx("An external script has changed the popup you are currently editing. Do you want to accept the external changes?", "editor"),
				        QMessageBox::Yes, QMessageBox::No | QMessageBox::Default | QMessageBox::Escape) != QMessageBox::Yes)
					return;
			}
			KviKvsPopupMenu * pCopy = new KviKvsPopupMenu(szName);
			pCopy->copyFrom(pPopup);
			ch->replacePopup(pCopy);

			// refresh current item
			if(ch == m_pLastEditedItem)
				m_pEditor->edit(m_pLastEditedItem);
			return;
		}
	}
	// create it
	KviKvsPopupMenu * pCopy = new KviKvsPopupMenu(szName);
	pCopy->copyFrom(pPopup);
	new MenuTreeWidgetItem(m_pTreeWidget, pCopy);
}

void PopupEditorWidget::customContextMenuRequested(const QPoint & pos)
{
	QTreeWidgetItem * it = m_pTreeWidget->itemAt(pos);
	KVI_ASSERT(m_bOneTimeSetupDone);

	if(it)
	{
		m_pContextPopup->clear();

		m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Popup)),
		    __tr2qs_ctx("&New Popup", "editor"), this, SLOT(newPopup()));

		m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Discard)),
		    __tr2qs_ctx("Re&move Popup", "editor"), this, SLOT(removeCurrentPopup()))
		    ->setEnabled(it);

		m_pContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Save)),
		    __tr2qs_ctx("&Export Popup to...", "editor"), this, SLOT(exportCurrentPopup()))
		    ->setEnabled(it);

		m_pContextPopup->popup(QCursor::pos());
	}
	else
	{
		m_pEmptyContextPopup->clear();

		m_pEmptyContextPopup->addAction(*(g_pIconManager->getSmallIcon(KviIconManager::Popup)),
		    __tr2qs_ctx("&New Popup", "editor"), this, SLOT(newPopup()));
		m_pEmptyContextPopup->popup(QCursor::pos());
	}
}

void PopupEditorWidget::exportCurrentPopup()
{
	if(!m_pLastEditedItem)
		return;
	saveLastEditedItem();
	if(!m_pLastEditedItem)
		return;

	QString szName = QDir::homePath();
	if(!szName.endsWith(QString(KVI_PATH_SEPARATOR)))
		szName += KVI_PATH_SEPARATOR;
	szName += m_pLastEditedItem->popup()->popupName();
	szName += ".kvs";

	QString szFile;

	if(!KviFileDialog::askForSaveFileName(szFile, __tr2qs_ctx("Enter a Filename - KVIrc", "editor"), szName, KVI_FILTER_SCRIPT, true, true, true, this))
		return;

	QString szOut;
	m_pLastEditedItem->popup()->generateDefPopup(szOut);

	if(!KviFileUtils::writeFile(szFile, szOut))
	{
		QMessageBox::warning(this, __tr2qs_ctx("Writing to File Failed - KVIrc", "editor"), __tr2qs_ctx("Unable to write to the popups file.", "editor"), __tr2qs_ctx("OK", "editor"));
	}
}

void PopupEditorWidget::exportAll()
{
	bool bSelectedOnly = true;
	exportPopups(bSelectedOnly);
}

void PopupEditorWidget::exportSelected()
{
	bool bSelectedOnly = false;
	exportPopups(bSelectedOnly);
}

void PopupEditorWidget::exportPopups(bool bSelectedOnly)
{
	saveLastEditedItem();

	QString out;
	int count = 0, topcount = m_pTreeWidget->topLevelItemCount();

	for(int i = 0; i < topcount; i++)
	{
		MenuTreeWidgetItem * it = (MenuTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		if((it->isSelected()) || (bSelectedOnly == true))
		{
			count++;
			QString tmp;
			it->popup()->generateDefPopup(tmp);
			out += tmp;
			out += "\n";
		}
	}
	if(!count && !bSelectedOnly)
		return;

	QString szName = QDir::homePath();
	if(!szName.endsWith(QString(KVI_PATH_SEPARATOR)))
		szName += KVI_PATH_SEPARATOR;
	szName += "popups.kvs";

	QString szFile;

	if(!KviFileDialog::askForSaveFileName(szFile, __tr2qs_ctx("Enter a Filename - KVIrc", "editor"), szName, KVI_FILTER_SCRIPT, true, true, true, this))
		return;

	if(!KviFileUtils::writeFile(szFile, out))
	{
		QMessageBox::warning(this, __tr2qs_ctx("Writing to File Failed - KVIrc", "editor"), __tr2qs_ctx("Unable to write to the popups file.", "editor"), __tr2qs_ctx("OK", "editor"));
	}
}

void PopupEditorWidget::removeCurrentPopup()
{
	if(m_pLastEditedItem)
	{
		MenuTreeWidgetItem * it = m_pLastEditedItem;
		m_pLastEditedItem = nullptr;
		delete it;
		if(!m_pLastEditedItem)
			currentItemChanged(nullptr, nullptr);
	}
}

void PopupEditorWidget::newPopup()
{
	QString newName;
	getUniquePopupName(nullptr, newName);
	MenuTreeWidgetItem * it = new MenuTreeWidgetItem(m_pTreeWidget, new KviKvsPopupMenu(newName));
	m_pTreeWidget->setCurrentItem(it);
}

void PopupEditorWidget::saveLastEditedItem()
{
	if(!m_pLastEditedItem)
		return;

	KviKvsPopupMenu * m = m_pEditor->getMenu();
	QString tmp = m->popupName();
	QString old = m_pLastEditedItem->m_pPopup->popupName();
	if(!KviQString::equalCI(tmp, old))
	{
		getUniquePopupName(m_pLastEditedItem, tmp);
		m->setPopupName(tmp);
	}

	m_pLastEditedItem->replacePopup(m);
	m_pLastEditedItem->setText(0, m->popupName());
}

void PopupEditorWidget::currentItemChanged(QTreeWidgetItem * it, QTreeWidgetItem *)
{
	saveLastEditedItem();

	m_pLastEditedItem = (MenuTreeWidgetItem *)it;
	m_pEditor->edit(m_pLastEditedItem);
}

void PopupEditorWidget::showEvent(QShowEvent * e)
{
	oneTimeSetup();
	QWidget::showEvent(e);
}

void PopupEditorWidget::commit()
{
	m_bSaving = true;
	if(!m_bOneTimeSetupDone)
		return;

	saveLastEditedItem();

	int topcount = m_pTreeWidget->topLevelItemCount();

	// Copy the original popup dict
	KviPointerHashTable<QString, KviKvsPopupMenu> copy(*(KviKvsPopupManager::instance()->popupDict()));
	copy.setAutoDelete(false);

	for(int i = 0; i < topcount; i++)
	{
		MenuTreeWidgetItem * it = (MenuTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		KviKvsPopupMenu * p = KviKvsPopupManager::instance()->get(it->m_pPopup->popupName());
		p->doClear();
		p->copyFrom(it->m_pPopup);
		// remove it from the original copy
		copy.remove(p->popupName());
	}

	// the remaining elements in the copy need to be removed from
	// the "new" dictionary (they are no longer used)
	KviPointerHashTableIterator<QString, KviKvsPopupMenu> iter(copy);

	while(iter.current())
	{
		KviKvsPopupManager::instance()->remove(iter.currentKey());
		++iter;
	}

	g_pApp->savePopups();
	m_bSaving = false;
}

void PopupEditorWidget::getUniquePopupName(MenuTreeWidgetItem * item, QString & buffer)
{
	KVI_ASSERT(m_bOneTimeSetupDone);

	if(buffer.isEmpty())
		buffer = __tr2qs_ctx("unnamed", "editor");
	QString newName = buffer;

	bool bFound = true;
	int idx = 1;
	int topcount = m_pTreeWidget->topLevelItemCount();

	while(bFound)
	{
		bFound = false;

		for(int i = 0; i < topcount; i++)
		{
			MenuTreeWidgetItem * ch = (MenuTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);

			if(KviQString::equalCI(newName, ch->m_pPopup->popupName()) && (ch != item))
			{
				bFound = true;
				newName = QString("%1.%2").arg(buffer).arg(idx);
				idx++;
				break;
			}
		}
	}

	buffer = newName;
}

PopupEditorWindow::PopupEditorWindow()
    : KviWindow(KviWindow::ScriptEditor, "popupeditor", nullptr)
{
	g_pPopupEditorWindow = this;

	QGridLayout * g = new QGridLayout();
	m_pEditor = new PopupEditorWidget(this);
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

PopupEditorWindow::~PopupEditorWindow()
{
	g_pPopupEditorWindow = nullptr;
}

void PopupEditorWindow::okClicked()
{
	m_pEditor->commit();
	close();
}

QPixmap * PopupEditorWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KviIconManager::PopupEditor);
}

void PopupEditorWindow::fillCaptionBuffers()
{
	m_szPlainTextCaption = __tr2qs_ctx("Popup Editor", "editor");
}
