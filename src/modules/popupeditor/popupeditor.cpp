//=============================================================================
//
//   File : popupeditor.cpp
//   Creation date : Mon Dec 23 2002 20:28:18 by Szymon Stefanek
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

#include "popupeditor.h"

#include "kvi_iconmanager.h"
#include "kvi_options.h"
#include "kvi_locale.h"
#include "kvi_imagedialog.h"
#include "kvi_config.h"
#include "kvi_filedialog.h"
#include "kvi_fileutils.h"
#include "kvi_scripteditor.h"
#include "kvi_debug.h"
#include "kvi_locale.h"
#include "kvi_app.h"
#include "kvi_kvs_popupmanager.h"
#include "kvi_kvs_popupmenu.h"
#include "kvi_kvs_variantlist.h"
#include "kvi_pointerhashtable.h"
#include "kvi_tal_vbox.h"
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


extern KviPopupEditorWindow * g_pPopupEditorWindow;



//KviPopupEntryItem
KviPopupTreeWidgetItem::KviPopupTreeWidgetItem(QTreeWidget * pTreeWidget,KviPopupTreeWidgetItem * after,Type t)
: QTreeWidgetItem(pTreeWidget, after)
{
	m_type = t;
	init();
}

KviPopupTreeWidgetItem::KviPopupTreeWidgetItem(KviPopupTreeWidgetItem * parent,KviPopupTreeWidgetItem * after,Type t)
: QTreeWidgetItem(parent, after)
{
	m_type = t;
	init();
}

void KviPopupTreeWidgetItem::init()
{
	switch(m_type)
	{
		case Item:
			setText(1,__tr2qs_ctx("Item","editor"));
		break;
		case Menu:
			setText(1,__tr2qs_ctx("Submenu","editor"));
		break;
		case ExtMenu:
			setText(1,__tr2qs_ctx("External Menu","editor"));
		break;
		case Separator:
			setText(0,"-----------------------");
			setText(1,__tr2qs_ctx("Separator","editor"));
		break;
		case Label:
			setText(1,__tr2qs_ctx("Label","editor"));
		break;
		case Epilogue:
			setText(0,__tr2qs_ctx("### Epilogue ###","editor"));
			setText(1,__tr2qs_ctx("Epilogue","editor"));
		break;
		case Prologue:
			setText(0,__tr2qs_ctx("### Prologue ###","editor"));
			setText(1,__tr2qs_ctx("Prologue","editor"));
		break;
		default:
		break;
	}
}

void KviPopupTreeWidgetItem::setItemText(const QString & szText)
{
	switch(m_type)
	{
		case Item:
		case Menu:
		case Label:
		case ExtMenu:
			m_szText = szText;
			setText(0,szText);
		break;
		default:
		break;
	}
}

void KviPopupTreeWidgetItem::setCondition(const QString & szCondition)
{
	switch(m_type)
	{
		case Item:
		case Menu:
		case Label:
		case ExtMenu:
			m_szCondition = szCondition;
		break;
		default:
		break;
	}
}

void KviPopupTreeWidgetItem::setCode(const QString & szCode)
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

void KviPopupTreeWidgetItem::setId(const QString & szId)
{
	m_szId = szId;
}

void KviPopupTreeWidgetItem::setIcon(const QString & szIcon)
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
					QTreeWidgetItem::setIcon(0,QIcon(*pix));
				else
					QTreeWidgetItem::setIcon(0,QIcon());
			}
		break;
		default:
		break;
	}
}


KviSinglePopupEditor::KviSinglePopupEditor(QWidget * par)
: QWidget(par)
{
	m_pLastSelectedItem = 0;
	m_pContextPopup = new KviTalPopupMenu(this);
	m_pClipboard = 0;
	m_pTestPopup = 0;

	QGridLayout * g = new QGridLayout(this);
	g->setMargin(0);
	g->setSpacing(2);


	m_pNameEditor = new QLineEdit(this);
	m_pNameEditor->setToolTip(__tr2qs_ctx("Popup name","editor"));

	g->addWidget(m_pNameEditor,0,0,1,2);

	m_pMenuButton = new QPushButton(__tr2qs_ctx("Test","editor"),this);
	g->addWidget(m_pMenuButton,0,2);
	connect(m_pMenuButton,SIGNAL(clicked()),this,SLOT(testPopup()));
	QSplitter * spl = new QSplitter(Qt::Vertical,this);
	spl->setObjectName("popupeditor_vertical_splitter");
	spl->setChildrenCollapsible(false);

	m_pTreeWidget = new QTreeWidget(spl);
	m_pTreeWidget->setColumnCount(2);
	QStringList labels;
	labels << __tr2qs_ctx("Item","editor") << __tr2qs_ctx("Type","editor");
	m_pTreeWidget->setHeaderLabels(labels);
	m_pTreeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pTreeWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pTreeWidget->setAllColumnsShowFocus(true);
	m_pTreeWidget->setRootIsDecorated(true);
	m_pTreeWidget->header()->setSortIndicatorShown(false);
	m_pTreeWidget->setSortingEnabled(false);
	m_pTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(m_pTreeWidget,SIGNAL(itemSelectionChanged()),this,SLOT(selectionChanged()));
	connect(m_pTreeWidget,SIGNAL(customContextMenuRequested(const QPoint &)),
		this,SLOT(customContextMenuRequested(const QPoint &)));

	m_pEditor = KviScriptEditor::createInstance(spl);

	g->addWidget(spl,1,0,1,3);

	QLabel * l = new QLabel(__tr2qs_ctx("Text:","editor"),this);
	g->addWidget(l,2,0);

	m_pTextEditor = new QLineEdit(this);
	m_pTextEditor->setToolTip(
		__tr2qs_ctx("<center><b>Visible text</b><br>May contain identifiers that will be evaluated at popup call time.<br>For labels, this text can contain also limited HTML tags.</center>","editor"));
	g->addWidget(m_pTextEditor,2,1,1,2);

	l = new QLabel(__tr2qs_ctx("Condition:","editor"),this);
	l->setMargin(2);
	g->addWidget(l,3,0);

	m_pConditionEditor = new QLineEdit(this);
	m_pConditionEditor->setToolTip(
		__tr2qs_ctx("<center><b>Boolean condition</b><br>Will be evaluated at popup call time in order to decide if this entry has to be shown.<br>An empty condition evaluates to true.</center>","editor"));
	g->addWidget(m_pConditionEditor,3,1,1,2);

	l = new QLabel(__tr2qs_ctx("Icon:","editor"),this);
	l->setMargin(2);
	g->addWidget(l,4,0);

	m_pIconEditor = new QLineEdit(this);
	m_pIconEditor->setToolTip(
		__tr2qs_ctx("<center><b>Icon identifier</b><br>May be an internal icon ID, an absolute path or a relative path.<br>Portable scripts should never use absolute paths.</center>","editor"));
	g->addWidget(m_pIconEditor,4,1,1,2);

	l = new QLabel(__tr2qs_ctx("External menu:","editor"),this);
	l->setMargin(2);
	g->addWidget(l,5,0);

	m_pExtNameEditor = new QLineEdit(this);
	m_pExtNameEditor->setToolTip(
		__tr2qs_ctx("<center><b>External menu name</b><br>This allows to nest externally defined popup menus. The popup menu with the specified name will be looked up at menu setup time.</center>","editor"));
	g->addWidget(m_pExtNameEditor,5,1,1,2);

	l = new QLabel(__tr2qs_ctx("Item Id:","editor"),this);
	l->setMargin(2);
	g->addWidget(l,6,0);


	m_pIdEditor = new QLineEdit(this);
	m_pIdEditor->setToolTip(
		__tr2qs_ctx("<center><b>Item id</b><br>This will allow you to use delpopupitem later.</center>","editor"));
	g->addWidget(m_pIdEditor,6,1,1,2);
	g->setColumnStretch(1,1);
	g->setRowStretch(1,1);
}

KviSinglePopupEditor::~KviSinglePopupEditor()
{
	if(m_pClipboard)delete m_pClipboard;
	if(m_pTestPopup)delete m_pTestPopup;
	KviScriptEditor::destroyInstance(m_pEditor);
}

void KviSinglePopupEditor::testPopup()
{

	if(m_pTestPopup)delete m_pTestPopup;
	m_pTestPopup = getMenu();
	if(!m_pTestPopup)return;
	connect(m_pTestPopup,SIGNAL(testModeItemClicked(KviKvsPopupMenuItem *)),this,SLOT(testModeMenuItemClicked(KviKvsPopupMenuItem *)));
	QPoint pnt = m_pMenuButton->mapToGlobal(QPoint(0,m_pMenuButton->height()));
	KviKvsVariantList * parms = new KviKvsVariantList();
	parms->append(new KviKvsVariant(QString("test1")));
	parms->append(new KviKvsVariant(QString("test2")));
	parms->append(new KviKvsVariant(QString("test3")));
	parms->append(new KviKvsVariant(QString("test4")));
	m_pTestPopup->doPopup(pnt,g_pActiveWindow,parms,true);
}

KviPopupTreeWidgetItem * KviSinglePopupEditor::findMatchingItem(KviKvsPopupMenuItem * it,KviPopupTreeWidgetItem * item)
{

	if(it->type() != KviKvsPopupMenuItem::Item)goto not_this_one;
	if(item->m_type != KviPopupTreeWidgetItem::Item)goto not_this_one;
	if(it->name() != item->m_szId)goto not_this_one;
	if(it->kvsText())
	{
		if(it->kvsText()->code() != item->m_szText)goto not_this_one;
	} else {
		if(!item->m_szText.isEmpty())goto not_this_one;
	}
	if(it->kvsCode())
	{
		if(it->kvsCode()->code() != item->m_szCode)goto not_this_one;
	} else {
		if(!item->m_szCode.isEmpty())goto not_this_one;
	}
	if(it->kvsIcon())
	{
		if(it->kvsIcon()->code() != item->m_szIcon)goto not_this_one;
	} else {
		if(!item->m_szIcon.isEmpty())goto not_this_one;
	}
	if(it->kvsCondition())
	{
		if(it->kvsCondition()->code() != item->m_szCondition)goto not_this_one;
	} else {
		if(!item->m_szCondition.isEmpty())goto not_this_one;
	}

	return item;

not_this_one:
	int count=item->childCount();
	for(int i=0; i<count; i++)
	{
		item = (KviPopupTreeWidgetItem *)item->child(i);
		KviPopupTreeWidgetItem * found = findMatchingItem(it,item);
		if(found)return found;
	}

	return 0;
}

void KviSinglePopupEditor::testModeMenuItemClicked(KviKvsPopupMenuItem * it)
{

	saveLastSelectedItem(); // that's the first thingie
	// find the matching item and set it as current
	int count=m_pTreeWidget->topLevelItemCount();
	for(int i=0; i<count; i++)
	{
		KviPopupTreeWidgetItem * item = (KviPopupTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		KviPopupTreeWidgetItem * found = findMatchingItem(it,item);
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

void KviSinglePopupEditor::customContextMenuRequested(const QPoint &pos)
{
	QTreeWidgetItem *it=m_pTreeWidget->itemAt(pos);

	m_pContextPopup->clear();

	KviPopupTreeWidgetItem * parentMenu = 0;
	bool bIsMenu = false;

	if(it)
	{
		parentMenu = (KviPopupTreeWidgetItem *) (((KviPopupTreeWidgetItem *)it)->parent());
		bIsMenu = ((KviPopupTreeWidgetItem *)it)->m_type == KviPopupTreeWidgetItem::Menu;
	}

	m_pContextPopup->insertItem(__tr2qs_ctx("New Separator Below","editor"),this,SLOT(contextNewSeparatorBelow()));
	m_pContextPopup->setItemEnabled(
	m_pContextPopup->insertItem(__tr2qs_ctx("New Separator Above","editor"),this,SLOT(contextNewSeparatorAbove())),it);
	m_pContextPopup->setItemEnabled(
	m_pContextPopup->insertItem(__tr2qs_ctx("New Separator Inside","editor"),this,SLOT(contextNewSeparatorInside())),it && bIsMenu);

	m_pContextPopup->insertSeparator();

	m_pContextPopup->insertItem(__tr2qs_ctx("New Label Below","editor"),this,SLOT(contextNewLabelBelow()));
	m_pContextPopup->setItemEnabled(
	m_pContextPopup->insertItem(__tr2qs_ctx("New Label Above","editor"),this,SLOT(contextNewLabelAbove())),it);
	m_pContextPopup->setItemEnabled(
	m_pContextPopup->insertItem(__tr2qs_ctx("New Label Inside","editor"),this,SLOT(contextNewLabelInside())),it && bIsMenu);

	m_pContextPopup->insertSeparator();

	m_pContextPopup->insertItem(__tr2qs_ctx("New Item Below","editor"),this,SLOT(contextNewItemBelow()));
	m_pContextPopup->setItemEnabled(
	m_pContextPopup->insertItem(__tr2qs_ctx("New Item Above","editor"),this,SLOT(contextNewItemAbove())),it);
	m_pContextPopup->setItemEnabled(
	m_pContextPopup->insertItem(__tr2qs_ctx("New Item Inside","editor"),this,SLOT(contextNewItemInside())),it && bIsMenu);

	m_pContextPopup->insertSeparator();

	m_pContextPopup->insertItem(__tr2qs_ctx("New Menu Below","editor"),this,SLOT(contextNewMenuBelow()));
	m_pContextPopup->setItemEnabled(
	m_pContextPopup->insertItem(__tr2qs_ctx("New Menu Above","editor"),this,SLOT(contextNewMenuAbove())),it);
	m_pContextPopup->setItemEnabled(
	m_pContextPopup->insertItem(__tr2qs_ctx("New Menu Inside","editor"),this,SLOT(contextNewMenuInside())),it && bIsMenu);

	m_pContextPopup->insertSeparator();

	m_pContextPopup->insertItem(__tr2qs_ctx("New External Menu Below","editor"),this,SLOT(contextNewExtMenuBelow()));
	m_pContextPopup->setItemEnabled(
	m_pContextPopup->insertItem(__tr2qs_ctx("New External Menu Above","editor"),this,SLOT(contextNewExtMenuAbove())),it);
	m_pContextPopup->setItemEnabled(
	m_pContextPopup->insertItem(__tr2qs_ctx("New External Menu Inside","editor"),this,SLOT(contextNewExtMenuInside())),it && bIsMenu);

	m_pContextPopup->insertSeparator();

	m_pContextPopup->setItemEnabled(
	m_pContextPopup->insertItem(
		*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CUT)),
		__tr2qs_ctx("Cu&t","editor"),
		this,SLOT(contextCut())),it);
	m_pContextPopup->setItemEnabled(
	m_pContextPopup->insertItem(
		*(g_pIconManager->getSmallIcon(KVI_SMALLICON_COPY)),
		__tr2qs_ctx("&Copy","editor"),
		this,SLOT(contextCopy())),it);
	m_pContextPopup->setItemEnabled(
	m_pContextPopup->insertItem(
		*(g_pIconManager->getSmallIcon(KVI_SMALLICON_PASTE)),
		__tr2qs_ctx("&Paste Below","editor"),this,SLOT(contextPasteBelow())),
		m_pClipboard);
	m_pContextPopup->setItemEnabled(
	m_pContextPopup->insertItem(
		*(g_pIconManager->getSmallIcon(KVI_SMALLICON_PASTE)),
		__tr2qs_ctx("Paste Above","editor"),this,SLOT(contextPasteAbove())),it && m_pClipboard);
	m_pContextPopup->setItemEnabled(
	m_pContextPopup->insertItem(
		*(g_pIconManager->getSmallIcon(KVI_SMALLICON_PASTE)),
		__tr2qs_ctx("Paste Inside","editor"),this,SLOT(contextPasteInside())),it && bIsMenu && m_pClipboard);

	bool bSeparatorInserted = false;

//	if(!findPrologue(parentMenu))
//	{
		m_pContextPopup->insertSeparator();
		bSeparatorInserted = true;
		m_pContextPopup->insertItem(
				*(g_pIconManager->getSmallIcon(KVI_SMALLICON_PROLOGUE)),
				__tr2qs_ctx("New Menu Prologue","editor"),this,SLOT(contextNewPrologue()));
//	}

//	if(!findEpilogue(parentMenu))
//	{
		if(!bSeparatorInserted)m_pContextPopup->insertSeparator();
		m_pContextPopup->insertItem(
				*(g_pIconManager->getSmallIcon(KVI_SMALLICON_EPILOGUE)),
				__tr2qs_ctx("New Menu Epilogue","editor"),this,SLOT(contextNewEpilogue()));
//	}

	m_pContextPopup->popup(QCursor::pos());
}

void KviSinglePopupEditor::createNewItemAboveLastSelected(KviPopupTreeWidgetItem::Type t)
{
	m_pTreeWidget->setCurrentItem(newItemAbove(m_pLastSelectedItem,t));
}

void KviSinglePopupEditor::createNewItemBelowLastSelected(KviPopupTreeWidgetItem::Type t)
{
	m_pTreeWidget->setCurrentItem(newItemBelow(m_pLastSelectedItem,t));
}

void KviSinglePopupEditor::createNewItemInsideLastSelected(KviPopupTreeWidgetItem::Type t)
{
	if(m_pLastSelectedItem)m_pLastSelectedItem->setExpanded(true);
	m_pTreeWidget->setCurrentItem(newItemInside(m_pLastSelectedItem,t));
}

void KviSinglePopupEditor::contextNewSeparatorAbove()
{
	createNewItemAboveLastSelected(KviPopupTreeWidgetItem::Separator);
}

void KviSinglePopupEditor::contextNewSeparatorBelow()
{
	createNewItemBelowLastSelected(KviPopupTreeWidgetItem::Separator);
}

void KviSinglePopupEditor::contextNewSeparatorInside()
{
	createNewItemInsideLastSelected(KviPopupTreeWidgetItem::Separator);
}

void KviSinglePopupEditor::contextNewItemBelow()
{
	createNewItemBelowLastSelected(KviPopupTreeWidgetItem::Item);
}

void KviSinglePopupEditor::contextNewItemAbove()
{
	createNewItemAboveLastSelected(KviPopupTreeWidgetItem::Item);
}

void KviSinglePopupEditor::contextNewItemInside()
{
	createNewItemInsideLastSelected(KviPopupTreeWidgetItem::Item);
}

void KviSinglePopupEditor::contextNewMenuBelow()
{
	createNewItemBelowLastSelected(KviPopupTreeWidgetItem::Menu);
}

void KviSinglePopupEditor::contextNewMenuAbove()
{
	createNewItemAboveLastSelected(KviPopupTreeWidgetItem::Menu);
}

void KviSinglePopupEditor::contextNewMenuInside()
{
	createNewItemInsideLastSelected(KviPopupTreeWidgetItem::Menu);
}

void KviSinglePopupEditor::contextNewExtMenuBelow()
{
	createNewItemBelowLastSelected(KviPopupTreeWidgetItem::ExtMenu);
}

void KviSinglePopupEditor::contextNewExtMenuAbove()
{
	createNewItemAboveLastSelected(KviPopupTreeWidgetItem::ExtMenu);
}
void KviSinglePopupEditor::contextNewExtMenuInside()
{
	createNewItemInsideLastSelected(KviPopupTreeWidgetItem::ExtMenu);
}

void KviSinglePopupEditor::contextNewLabelBelow()
{
	createNewItemBelowLastSelected(KviPopupTreeWidgetItem::Label);
}

void KviSinglePopupEditor::contextNewLabelAbove()
{
	createNewItemAboveLastSelected(KviPopupTreeWidgetItem::Label);
}

void KviSinglePopupEditor::contextNewLabelInside()
{
	createNewItemInsideLastSelected(KviPopupTreeWidgetItem::Label);
}

KviPopupTreeWidgetItem * KviSinglePopupEditor::newItem(KviPopupTreeWidgetItem * par,KviPopupTreeWidgetItem * after,KviPopupTreeWidgetItem::Type t)
{
	if(par)return new KviPopupTreeWidgetItem(par,after,t);
	return new KviPopupTreeWidgetItem(m_pTreeWidget,after,t);
}

KviPopupTreeWidgetItem * KviSinglePopupEditor::newItemBelow(KviPopupTreeWidgetItem * it,KviPopupTreeWidgetItem::Type t)
{
	if(!it)return newItem(0,0,t);
	return newItem((KviPopupTreeWidgetItem *)it->parent(),it,t);
}

KviPopupTreeWidgetItem * KviSinglePopupEditor::newItemAbove(KviPopupTreeWidgetItem * it,KviPopupTreeWidgetItem::Type t)
{
	if(!it)return newItem(0,0,t);
	return newItem((KviPopupTreeWidgetItem *)it->parent(),(KviPopupTreeWidgetItem *)m_pTreeWidget->itemAbove(it),t);
}

KviPopupTreeWidgetItem * KviSinglePopupEditor::newItemInside(KviPopupTreeWidgetItem * it,KviPopupTreeWidgetItem::Type t)
{
	if(it)
	{
		if(it->m_type != KviPopupTreeWidgetItem::Menu)
		{
			return newItemBelow(it,t);
		}
	}
	return newItem(it,it,t);
}

void KviSinglePopupEditor::contextNewPrologue()
{
	KviPopupTreeWidgetItem * it = m_pLastSelectedItem ? (KviPopupTreeWidgetItem *)m_pLastSelectedItem->parent() : 0;
//	if(!findPrologue(it))
//	{
		m_pTreeWidget->setCurrentItem(newItem(it,it,KviPopupTreeWidgetItem::Prologue));
//	}
}

void KviSinglePopupEditor::contextNewEpilogue()
{
	KviPopupTreeWidgetItem * it = m_pLastSelectedItem ? (KviPopupTreeWidgetItem *)m_pLastSelectedItem->parent() : 0;
//	if(!findEpilogue(it))
//	{
		KviPopupTreeWidgetItem * after = it ? (KviPopupTreeWidgetItem *)it->child(0) : (KviPopupTreeWidgetItem *)m_pTreeWidget->topLevelItem(0);
		if(after)
		{
			while(m_pTreeWidget->itemAbove(after))
			{
				if(after->parent()==m_pTreeWidget->itemAbove(after)->parent())
					after = (KviPopupTreeWidgetItem *)m_pTreeWidget->itemAbove(after);
			}
		} else {
			after = it;
		}
		m_pTreeWidget->setCurrentItem(newItem(it,after,KviPopupTreeWidgetItem::Epilogue));
//	}
}

void KviSinglePopupEditor::contextCopy()
{
	if(!m_pLastSelectedItem)return;
	if(m_pClipboard)delete m_pClipboard;
	m_pClipboard = new KviKvsPopupMenu("clipboard");
	addItemToMenu(m_pClipboard,m_pLastSelectedItem);
}

void KviSinglePopupEditor::contextCut()
{
	if(!m_pLastSelectedItem)return;
	contextCopy();

	KviPopupTreeWidgetItem * it = m_pLastSelectedItem;
	m_pLastSelectedItem = 0;
	delete it;
	if(!m_pLastSelectedItem)selectionChanged();
}

void KviSinglePopupEditor::contextPasteBelow()
{
	if(!m_pClipboard)return;
	KviPopupTreeWidgetItem * par = m_pLastSelectedItem ? (KviPopupTreeWidgetItem *)m_pLastSelectedItem->parent() : 0;
	populateMenu(m_pClipboard,par,m_pLastSelectedItem);
}

void KviSinglePopupEditor::contextPasteAbove()
{
	if(!m_pClipboard)return;
	KviPopupTreeWidgetItem * par = m_pLastSelectedItem ? (KviPopupTreeWidgetItem *)m_pLastSelectedItem->parent() : 0;
	KviPopupTreeWidgetItem * above = m_pLastSelectedItem ? (KviPopupTreeWidgetItem *)m_pTreeWidget->itemAbove(m_pLastSelectedItem) : 0;
	populateMenu(m_pClipboard,par,above);
}

void KviSinglePopupEditor::contextPasteInside()
{
	if(!m_pClipboard)return;
	if(m_pLastSelectedItem)
	{
		if(m_pLastSelectedItem->m_type != KviPopupTreeWidgetItem::Menu)
		{
			contextPasteBelow();
			return;
		}
		m_pLastSelectedItem->setExpanded(true);
	}
	populateMenu(m_pClipboard,m_pLastSelectedItem,0);
}

void KviSinglePopupEditor::saveLastSelectedItem()
{
	if(!m_pLastSelectedItem)return;

	QString tmp;

	switch(m_pLastSelectedItem->m_type)
	{
		case KviPopupTreeWidgetItem::Prologue:
		case KviPopupTreeWidgetItem::Epilogue:
		case KviPopupTreeWidgetItem::Item:
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
		case KviPopupTreeWidgetItem::Menu:
		case KviPopupTreeWidgetItem::ExtMenu:
		case KviPopupTreeWidgetItem::Item:
			m_pLastSelectedItem->setIcon(m_pIconEditor->text());
		break;
		default:
		break;
	}

	switch(m_pLastSelectedItem->m_type)
	{
		case KviPopupTreeWidgetItem::Menu:
		case KviPopupTreeWidgetItem::Item:
		case KviPopupTreeWidgetItem::Label:
		case KviPopupTreeWidgetItem::ExtMenu:
			m_pLastSelectedItem->setItemText(m_pTextEditor->text());
			m_pLastSelectedItem->setCondition(m_pConditionEditor->text());
		break;
		default:
		break;
	}

	if(m_pLastSelectedItem->m_type == KviPopupTreeWidgetItem::ExtMenu)
	{
		m_pLastSelectedItem->setCode(m_pExtNameEditor->text());
	}
}

void KviSinglePopupEditor::addItemToMenu(KviKvsPopupMenu * p,KviPopupTreeWidgetItem *it)
{
	it->m_szId.trimmed();
	switch(it->m_type)
	{
		case KviPopupTreeWidgetItem::Prologue:
			it->m_szCode.trimmed();
			p->addPrologue(it->m_szId,it->m_szCode);
		break;
		case KviPopupTreeWidgetItem::Epilogue:
			it->m_szCode.trimmed();
			p->addEpilogue(it->m_szId,it->m_szCode);
		break;
		case KviPopupTreeWidgetItem::Separator:
			it->m_szCondition.trimmed();
			p->addSeparator(it->m_szId,it->m_szCondition);
		break;
		case KviPopupTreeWidgetItem::Label:
			it->m_szText.trimmed();
			it->m_szCondition.trimmed();
			it->m_szIcon.trimmed();
			p->addLabel(it->m_szId,it->m_szText,it->m_szIcon,it->m_szCondition);
		break;
		case KviPopupTreeWidgetItem::Item:
			it->m_szText.trimmed();
			it->m_szIcon.trimmed();
			it->m_szCondition.trimmed();
			it->m_szCode.trimmed();
			p->addItem(it->m_szId,it->m_szCode,it->m_szText,it->m_szIcon,it->m_szCondition);
		break;
		case KviPopupTreeWidgetItem::ExtMenu:
			it->m_szText.trimmed();
			it->m_szIcon.trimmed();
			it->m_szCondition.trimmed();
			it->m_szCode.trimmed();// <-- this is the ext name in fact
			p->addExtPopup(it->m_szId,it->m_szCode,it->m_szText,it->m_szIcon,it->m_szCondition);
		break;
		case KviPopupTreeWidgetItem::Menu:
		{
			it->m_szText.trimmed();
			it->m_szIcon.trimmed();
			it->m_szCondition.trimmed();
			KviKvsPopupMenu * menu = p->addPopup(it->m_szId,it->m_szText,it->m_szIcon,it->m_szCondition);
			int count= it->childCount();
			for(int i=0; i<count; i++)
			{
				addItemToMenu(menu,(KviPopupTreeWidgetItem *)it->child(i));
			}
		}
		break;
		default:
		break;
	}
}

KviKvsPopupMenu * KviSinglePopupEditor::getMenu()
{
	saveLastSelectedItem();

	QString tmp = m_pNameEditor->text();
	tmp.trimmed();

	KviKvsPopupMenu * p = new KviKvsPopupMenu(tmp);

	int count= m_pTreeWidget->topLevelItemCount();
	for(int i=0; i<count; i++)
	{
		addItemToMenu(p,(KviPopupTreeWidgetItem *)m_pTreeWidget->topLevelItem(i));
	}
	return p;
}

void KviSinglePopupEditor::selectionChanged()
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
		it=0;
	} else {
		it= (QTreeWidgetItem*)m_pTreeWidget->selectedItems().first();
	}

	if(it)
	{
		m_pIdEditor->setText(((KviPopupTreeWidgetItem *)it)->m_szId);

		switch(((KviPopupTreeWidgetItem *)it)->m_type)
		{
			case KviPopupTreeWidgetItem::Prologue:
			case KviPopupTreeWidgetItem::Epilogue:
			case KviPopupTreeWidgetItem::Item:
				m_pEditor->setText(((KviPopupTreeWidgetItem *)it)->m_szCode);
				bEditorEnabled = true;
			break;
			default:
			break;
		}

		switch(((KviPopupTreeWidgetItem *)it)->m_type)
		{
			case KviPopupTreeWidgetItem::Menu:
			case KviPopupTreeWidgetItem::Item:
			case KviPopupTreeWidgetItem::Label:
			case KviPopupTreeWidgetItem::ExtMenu:
				m_pIconEditor->setText(((KviPopupTreeWidgetItem *)it)->m_szIcon);
				bIconEditorEnabled = true;
			break;
			default:
			break;		}

		switch(((KviPopupTreeWidgetItem *)it)->m_type)
		{
			case KviPopupTreeWidgetItem::Menu:
			case KviPopupTreeWidgetItem::Item:
			case KviPopupTreeWidgetItem::Label:
			case KviPopupTreeWidgetItem::ExtMenu:
				m_pConditionEditor->setText(((KviPopupTreeWidgetItem *)it)->m_szCondition);
				bConditionEditorEnabled = true;
				m_pTextEditor->setText(((KviPopupTreeWidgetItem *)it)->m_szText);
				bTextEditorEnabled = true;
			break;
			default:
			break;
		}

		if(((KviPopupTreeWidgetItem *)it)->m_type == KviPopupTreeWidgetItem::ExtMenu)
		{
			m_pExtNameEditor->setText(((KviPopupTreeWidgetItem *)it)->m_szCode);
			bNameEditorEnabled =true;
		}
	}

	m_pLastSelectedItem = (KviPopupTreeWidgetItem *)it;

	if(!bEditorEnabled)m_pEditor->setText("");
	m_pEditor->setEnabled(bEditorEnabled);
	if(!bIconEditorEnabled)m_pIconEditor->setText("");
	m_pIconEditor->setEnabled(bIconEditorEnabled);
	if(!bConditionEditorEnabled)m_pConditionEditor->setText("");
	m_pConditionEditor->setEnabled(bConditionEditorEnabled);
	if(!bTextEditorEnabled)m_pTextEditor->setText("");
	m_pTextEditor->setEnabled(bTextEditorEnabled);
	m_pExtNameEditor->setEnabled(bNameEditorEnabled);
	if(!bNameEditorEnabled)m_pExtNameEditor->setText("");
	if(!it)m_pIdEditor->setText("");
	m_pIdEditor->setEnabled(it);
}

void KviSinglePopupEditor::populateMenu(KviKvsPopupMenu * pop,KviPopupTreeWidgetItem * par,KviPopupTreeWidgetItem * theItem)
{
	if(!pop)return;

	for(KviKvsScript * sp = pop->prologues()->first();sp;sp = pop->prologues()->next())
	{
		if(par)theItem = new KviPopupTreeWidgetItem(par,theItem,KviPopupTreeWidgetItem::Prologue);
		else theItem = new KviPopupTreeWidgetItem(m_pTreeWidget,theItem,KviPopupTreeWidgetItem::Prologue);
		theItem->setCode(sp->code());
		theItem->setId(sp->name());
	}

	for(KviKvsPopupMenuItem * item = pop->m_pItemList->first();item;item = pop->m_pItemList->next())
	{
		switch(item->type())
		{
			case KviKvsPopupMenuItem::Item:
				if(par)theItem = new KviPopupTreeWidgetItem(par,theItem,KviPopupTreeWidgetItem::Item);
				else theItem = new KviPopupTreeWidgetItem(m_pTreeWidget,theItem,KviPopupTreeWidgetItem::Item);
				theItem->setIcon(item->kvsIcon() ? item->kvsIcon()->code() : QString());
				theItem->setItemText(item->kvsText() ? item->kvsText()->code() : QString());
				theItem->setCondition(item->kvsCondition() ? item->kvsCondition()->code() : QString());
				theItem->setCode(item->kvsCode() ? item->kvsCode()->code() : QString());
				theItem->setId(item->name());
			break;
			case KviKvsPopupMenuItem::ExtMenu:
				if(par)theItem = new KviPopupTreeWidgetItem(par,theItem,KviPopupTreeWidgetItem::ExtMenu);
				else theItem = new KviPopupTreeWidgetItem(m_pTreeWidget,theItem,KviPopupTreeWidgetItem::ExtMenu);
				theItem->setIcon(item->kvsIcon() ? item->kvsIcon()->code() : QString());
				theItem->setItemText(item->kvsText() ? item->kvsText()->code() : QString());
				theItem->setCondition(item->kvsCondition() ? item->kvsCondition()->code() : QString());
				theItem->setCode(((KviKvsPopupMenuItemExtMenu *)item)->extName());
				theItem->setId(item->name());
			break;
			case KviKvsPopupMenuItem::Label:
				if(par)theItem = new KviPopupTreeWidgetItem(par,theItem,KviPopupTreeWidgetItem::Label);
				else theItem = new KviPopupTreeWidgetItem(m_pTreeWidget,theItem,KviPopupTreeWidgetItem::Label);
				theItem->setIcon(item->kvsIcon() ? item->kvsIcon()->code() : QString());
				theItem->setItemText(item->kvsText() ? item->kvsText()->code() : QString());
				theItem->setCondition(item->kvsCondition() ? item->kvsCondition()->code() : QString());
				theItem->setId(item->name());
			break;
			case KviKvsPopupMenuItem::Separator:
				if(par)theItem = new KviPopupTreeWidgetItem(par,theItem,KviPopupTreeWidgetItem::Separator);
				else theItem = new KviPopupTreeWidgetItem(m_pTreeWidget,theItem,KviPopupTreeWidgetItem::Separator);
				theItem->setId(item->name());
			break;
			case KviKvsPopupMenuItem::Menu:
				if(par)theItem = new KviPopupTreeWidgetItem(par,theItem,KviPopupTreeWidgetItem::Menu);
				else theItem = new KviPopupTreeWidgetItem(m_pTreeWidget,theItem,KviPopupTreeWidgetItem::Menu);
				theItem->setIcon(item->kvsIcon() ? item->kvsIcon()->code() : QString());
				theItem->setItemText(item->kvsText() ? item->kvsText()->code() : QString());
				theItem->setCondition(item->kvsCondition() ? item->kvsCondition()->code() : QString());
				theItem->setId(item->name());
				populateMenu(((KviKvsPopupMenuItemMenu *)item)->menu(),theItem,0);
			break;
			default:
			break;
		}
	}

	for(KviKvsScript * se = pop->epilogues()->first();se;se = pop->epilogues()->next())
	{
		if(par)theItem = new KviPopupTreeWidgetItem(par,theItem,KviPopupTreeWidgetItem::Epilogue);
		else theItem = new KviPopupTreeWidgetItem(m_pTreeWidget,theItem,KviPopupTreeWidgetItem::Epilogue);
		theItem->setCode(se->code());
		theItem->setId(se->name());
	}

	m_pTreeWidget->resizeColumnToContents(0);
}

void KviSinglePopupEditor::edit(KviMenuTreeWidgetItem * it)
{
	saveLastSelectedItem();

	m_pLastSelectedItem = 0;

	m_pTreeWidget->clear();

	selectionChanged();

	if(it)
	{
		m_pNameEditor->setText(it->m_pPopup->popupName());
		populateMenu(it->m_pPopup,0,0);
	} else {
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


KviMenuTreeWidgetItem::KviMenuTreeWidgetItem(QTreeWidget * par,KviKvsPopupMenu * popup)
: QTreeWidgetItem(par)
{
	setIcon(0,*(g_pIconManager->getSmallIcon(KVI_SMALLICON_POPUP)));
	setText(0,popup->popupName());
	m_pPopup = popup;
}

KviMenuTreeWidgetItem::~KviMenuTreeWidgetItem()
{
	delete m_pPopup;
}

void KviMenuTreeWidgetItem::replacePopup(KviKvsPopupMenu * popup)
{
	delete m_pPopup;
	m_pPopup = popup;
}

KviPopupEditor::KviPopupEditor(QWidget * par)
: QWidget(par)
{
	QGridLayout * l = new QGridLayout(this);
	QSplitter * spl = new QSplitter(Qt::Horizontal,this);
	spl->setObjectName("popupeditor_horizontal_splitter");
	spl->setChildrenCollapsible(false);
	l->addWidget(spl,0,0);

	KviTalVBox * box = new KviTalVBox(spl);

	m_pTreeWidget = new QTreeWidget(box);
	m_pTreeWidget->setHeaderLabel(__tr2qs_ctx("Popup","editor"));
	m_pTreeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_pTreeWidget->header()->setSortIndicatorShown(true);

	QPushButton * pb = new QPushButton(__tr2qs_ctx("&Export All To...","editor"),box);
	connect(pb,SIGNAL(clicked()),this,SLOT(exportAll()));
	QPushButton * gn = new QPushButton(__tr2qs_ctx("&Export selected To...","editor"),box);
	connect(gn,SIGNAL(clicked()),this,SLOT(exportSelected()));

	m_pEditor = new KviSinglePopupEditor(spl);

	m_bOneTimeSetupDone = false;
	m_pLastEditedItem = 0;

	m_pContextPopup = new KviTalPopupMenu(this);
	m_pEmptyContextPopup = new KviTalPopupMenu(this);
	
	spl->setStretchFactor (0,20);
	spl->setStretchFactor (1,80);

	currentItemChanged(0,0);
}

KviPopupEditor::~KviPopupEditor()
{
}

void KviPopupEditor::oneTimeSetup()
{
	if(m_bOneTimeSetupDone)return;
	m_bOneTimeSetupDone = true;

	const KviPointerHashTable<QString,KviKvsPopupMenu> * a = KviKvsPopupManager::instance()->popupDict();
	if(!a)return;

	KviPointerHashTableIterator<QString,KviKvsPopupMenu> it(*a);

	KviMenuTreeWidgetItem * item;

	while(it.current())
	{
		KviKvsPopupMenu * popup = it.current();
		KviKvsPopupMenu * copy = new KviKvsPopupMenu(popup->popupName());
		copy->copyFrom(popup);
		item = new KviMenuTreeWidgetItem(m_pTreeWidget,copy);
		++it;
	}

	m_pTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_pTreeWidget,SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)),this,SLOT(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)));
	connect(m_pTreeWidget,SIGNAL(customContextMenuRequested(const QPoint &)),
		this,SLOT(customContextMenuRequested(const QPoint &)));
}

void KviPopupEditor::customContextMenuRequested(const QPoint &pos)
{
	QTreeWidgetItem *it=m_pTreeWidget->itemAt(pos);
	__range_valid(m_bOneTimeSetupDone);

	if(it)
	{
		m_pContextPopup->clear();

		m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_POPUP)),
			__tr2qs_ctx("&New Popup","editor"),
			this,SLOT(newPopup()));

		m_pContextPopup->setItemEnabled(
		m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_QUIT)),
			__tr2qs_ctx("Re&move Popup","editor"),
			this,SLOT(removeCurrentPopup())),it);

		m_pContextPopup->setItemEnabled(
		m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_FOLDER)),
			__tr2qs_ctx("&Export Popup To...","editor"),
			this,SLOT(exportCurrentPopup())),it);

		m_pContextPopup->popup(QCursor::pos());
	} else {
		m_pEmptyContextPopup->clear();

		m_pEmptyContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_POPUP)),
			__tr2qs_ctx("&New Popup","editor"),
			this,SLOT(newPopup()));
		m_pEmptyContextPopup->popup(QCursor::pos());
	}
}

void KviPopupEditor::exportCurrentPopup()
{
	if(!m_pLastEditedItem)return;
	saveLastEditedItem();
	if(!m_pLastEditedItem)return;

	QString szName = QDir::homePath();
	if(!szName.endsWith(QString(KVI_PATH_SEPARATOR)))szName += KVI_PATH_SEPARATOR;
	szName += m_pLastEditedItem->popup()->popupName();
	szName += ".kvs";

	QString szFile;

	if(!KviFileDialog::askForSaveFileName(szFile,__tr2qs_ctx("Choose a Filename - KVIrc","editor"),szName,KVI_FILTER_SCRIPT,true,true,true))return;

	QString szOut;
	m_pLastEditedItem->popup()->generateDefPopup(szOut);

	if(!KviFileUtils::writeFile(szFile,szOut))
	{
		QMessageBox::warning(this,__tr2qs_ctx("Write Failed - KVIrc","editor"),__tr2qs_ctx("Unable to write to the popup file.","editor"),__tr2qs_ctx("OK","editor"));
	}
}

void KviPopupEditor::exportAll()
{
	bool bSelectedOnly=true;
	exportPopups(bSelectedOnly);
}

void KviPopupEditor::exportSelected()
{
	bool bSelectedOnly=false;
	exportPopups(bSelectedOnly);
}

void KviPopupEditor::exportPopups(bool bSelectedOnly)
{
	saveLastEditedItem();

	QString out;
	int count=0, topcount=m_pTreeWidget->topLevelItemCount();

	for(int i=0; i<topcount;i++)
	{
		KviMenuTreeWidgetItem * it = (KviMenuTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		if ( (it->isSelected()) || (bSelectedOnly == true) )
		{
			count++;
			QString tmp;
			it->popup()->generateDefPopup(tmp);
			out += tmp;
			out += "\n";
		}
	}
	if (!count && bSelectedOnly==false) return;

	QString szName = QDir::homePath();
	if(!szName.endsWith(QString(KVI_PATH_SEPARATOR)))szName += KVI_PATH_SEPARATOR;
	szName += "popups.kvs";

	QString szFile;

	if(!KviFileDialog::askForSaveFileName(szFile,__tr2qs_ctx("Choose a Filename - KVIrc","editor"),szName,KVI_FILTER_SCRIPT,true,true,true))return;

	if(!KviFileUtils::writeFile(szFile,out))
	{
		QMessageBox::warning(this,__tr2qs_ctx("Write Failed - KVIrc","editor"),__tr2qs_ctx("Unable to write to the alias file.","editor"),__tr2qs_ctx("OK","editor"));
	}
}

void KviPopupEditor::removeCurrentPopup()
{
	if(m_pLastEditedItem)
	{
		KviMenuTreeWidgetItem * it = m_pLastEditedItem;
		m_pLastEditedItem = 0;
		delete it;
		if(!m_pLastEditedItem)currentItemChanged(0,0);
	}
}

void KviPopupEditor::newPopup()
{
	QString newName;
	getUniquePopupName(0,newName);
	KviMenuTreeWidgetItem * it = new KviMenuTreeWidgetItem(m_pTreeWidget,new KviKvsPopupMenu(newName));
	m_pTreeWidget->setCurrentItem(it);
}

void KviPopupEditor::saveLastEditedItem()
{
	if(m_pLastEditedItem == 0)return;

	KviKvsPopupMenu * m = m_pEditor->getMenu();
	QString tmp = m->popupName();
	QString old = m_pLastEditedItem->m_pPopup->popupName();
	if(!KviQString::equalCI(tmp,old))
	{
		getUniquePopupName(m_pLastEditedItem,tmp);
		m->setPopupName(tmp);
	}

	m_pLastEditedItem->replacePopup(m);
	m_pLastEditedItem->setText(0,m->popupName());
}

void KviPopupEditor::currentItemChanged(QTreeWidgetItem *it, QTreeWidgetItem *)
{
	saveLastEditedItem();

	m_pLastEditedItem = (KviMenuTreeWidgetItem *)it;

	m_pEditor->edit(m_pLastEditedItem);
}

void KviPopupEditor::showEvent(QShowEvent *e)
{
	oneTimeSetup();
	QWidget::showEvent(e);
}

void KviPopupEditor::commit()
{
	if(!m_bOneTimeSetupDone)return;

	saveLastEditedItem();

	//KviKvsPopupManager::instance()->clear();

	int topcount=m_pTreeWidget->topLevelItemCount();

	// Copy the original popup dict
	KviPointerHashTable<QString,KviKvsPopupMenu> copy(*(KviKvsPopupManager::instance()->popupDict()));
	copy.setAutoDelete(false);

	for(int i=0; i<topcount;i++)
	{
		KviMenuTreeWidgetItem * it = (KviMenuTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		KviKvsPopupMenu * p = KviKvsPopupManager::instance()->get(it->m_pPopup->popupName());
		p->doClear();
		p->copyFrom(it->m_pPopup);
		// remove it from the original copy
		copy.remove(p->popupName());
	}

	// the remaining elements in the copy need to be removed from
	// the "new" dictionary (they are no longer used)
	KviPointerHashTableIterator<QString,KviKvsPopupMenu> iter(copy);

	while(iter.current())
	{
		KviKvsPopupManager::instance()->remove(iter.currentKey());
		++iter;
	}


	g_pApp->savePopups();
}

void KviPopupEditor::getUniquePopupName(KviMenuTreeWidgetItem *item,QString &buffer)
{
	__range_valid(m_bOneTimeSetupDone);

	if(buffer.isEmpty())buffer = __tr2qs_ctx("unnamed","editor");
	QString newName = buffer;

	bool bFound = true;
	int idx = 1;
	int topcount=m_pTreeWidget->topLevelItemCount();

	while(bFound)
	{
		bFound = false;

		for(int i=0; i<topcount;i++)
		{
			KviMenuTreeWidgetItem * ch = (KviMenuTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);

			if(KviQString::equalCI(newName,ch->m_pPopup->popupName()) && (ch != item))
			{
				bFound = true;
				KviQString::sprintf(newName,"%Q.%d",&buffer,idx);
				idx++;
				break;
			}
		}

	}

	buffer = newName;
}


KviPopupEditorWindow::KviPopupEditorWindow(KviFrame * lpFrm)
: KviWindow(KVI_WINDOW_TYPE_SCRIPTEDITOR,lpFrm,"popupeditor",0)
{
	g_pPopupEditorWindow = this;

	QGridLayout * g = new QGridLayout();
	m_pEditor = new KviPopupEditor(this);
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

KviPopupEditorWindow::~KviPopupEditorWindow()
{
	g_pPopupEditorWindow = 0;
}

void KviPopupEditorWindow::okClicked()
{
	m_pEditor->commit();
	close();
}

void KviPopupEditorWindow::applyClicked()
{
	m_pEditor->commit();
}

void KviPopupEditorWindow::cancelClicked()
{
	close();
}

QPixmap * KviPopupEditorWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KVI_SMALLICON_POPUP);
}

void KviPopupEditorWindow::fillCaptionBuffers()
{
	m_szPlainTextCaption = __tr2qs_ctx("Popup Editor","editor");
}

void KviPopupEditorWindow::getConfigGroupName(QString &szName)
{
	szName = "popupeditor";
}

void KviPopupEditorWindow::saveProperties(KviConfig *) //cfg
{
/*
#ifdef COMPILE_SCRIPTTOOLBAR
	cfg->writeEntry("Sizes",m_pEditor->sizes());
	cfg->writeEntry("LastPopup",m_pEditor->lastEditedPopup().ptr());
	//debug("LAST EDITED=%s",m_pEditor->lastEditedPopup().ptr());
#endif // COMPILE_SCRIPTTOOLBAR
*/
}

void KviPopupEditorWindow::loadProperties(KviConfig *) // cfg
{
/*
#ifdef COMPILE_SCRIPTTOOLBAR
	QValueList<int> def;
	def.append(20);
	def.append(80);
	m_pEditor->setSizes(cfg->readIntListEntry("Sizes",def));
	KviStr tmp = cfg->readEntry("LastPopup","");
	m_pEditor->editPopup(tmp);
	//debug("LAST EDITED WAS %s",tmp.ptr());
#endif // COMPILE_SCRIPTTOOLBAR
*/
}
