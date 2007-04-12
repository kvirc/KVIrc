//=============================================================================
//
//   File : popupeditor.cpp
//   Creation date : Mon Dec 23 2002 20:28:18 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)
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
//#include "kvi_parameterlist.h"

#include <qmessagebox.h>
#include <qdir.h>
#include <qsplitter.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qtooltip.h>
#include "kvi_asciidict.h"
#include <qpushbutton.h>
#include "kvi_tal_vbox.h"

extern KviPopupEditorWindow * g_pPopupEditorWindow;



//KviPopupEntryItem
KviPopupListViewItem::KviPopupListViewItem(KviTalListView * pListView,KviPopupListViewItem * after,Type t)
: KviTalListViewItem(pListView,after)
{
	m_type = t;
	init();
}

KviPopupListViewItem::KviPopupListViewItem(KviPopupListViewItem * parent,KviPopupListViewItem * after,Type t)
: KviTalListViewItem(parent,after)
{
	m_type = t;
	init();
}

void KviPopupListViewItem::init()
{
	switch(m_type)
	{
		case Item:
			setText(1,__tr2qs("Item"));
		break;
		case Menu:
			setText(1,__tr2qs("Submenu"));
		break;
		case ExtMenu:
			setText(1,__tr2qs("External Menu"));
		break;
		case Separator:
			setText(0,"-----------------------");
			setText(1,__tr2qs("Separator"));
		break;
		case Label:
			setText(1,__tr2qs("Label"));
		break;
		case Epilogue:
			setText(0,__tr2qs("### Epilogue ###"));
			setText(1,__tr2qs("Epilogue"));
		break;
		case Prologue:
			setText(0,__tr2qs("### Prologue ###"));
			setText(1,__tr2qs("Prologue"));
		break;
		default:
		break;
	}
}

void KviPopupListViewItem::setItemText(const QString & szText)
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

void KviPopupListViewItem::setCondition(const QString & szCondition)
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

void KviPopupListViewItem::setCode(const QString & szCode)
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

void KviPopupListViewItem::setId(const QString & szId)
{
	m_szId = szId;
}

void KviPopupListViewItem::setIcon(const QString & szIcon)
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
				if(pix)setPixmap(0,*pix);
				else setPixmap(0,QPixmap());
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

	QGridLayout * g = new QGridLayout(this,7,3,2,2);

	m_pNameEditor = new QLineEdit(this);
	QToolTip::add(m_pNameEditor,__tr2qs("Popup name"));

	g->addMultiCellWidget(m_pNameEditor,0,0,0,1);

	m_pMenuButton = new QPushButton(__tr2qs("Test"),this);
	g->addWidget(m_pMenuButton,0,2);
	connect(m_pMenuButton,SIGNAL(clicked()),this,SLOT(testPopup()));
#ifdef COMPILE_USE_QT4
	QSplitter * spl = new QSplitter(Qt::Vertical,this,"popupeditor");
	spl->setOpaqueResize(false);
#else
	QSplitter * spl = new QSplitter(QSplitter::Vertical,this);
#endif

	m_pListView = new KviTalListView(spl);
	m_pListView->addColumn(__tr2qs("Item"));
	m_pListView->addColumn(__tr2qs("Type"));
	m_pListView->setMultiSelection(false);
	m_pListView->setAllColumnsShowFocus(true);
	m_pListView->setRootIsDecorated(true);
	m_pListView->setShowSortIndicator(false);
	m_pListView->setSorting(-1);


	connect(m_pListView,SIGNAL(selectionChanged(KviTalListViewItem *)),this,SLOT(selectionChanged(KviTalListViewItem *)));
	connect(m_pListView,SIGNAL(rightButtonPressed(KviTalListViewItem *,const QPoint &,int)),
		this,SLOT(itemPressed(KviTalListViewItem *,const QPoint &,int)));

	m_pEditor = KviScriptEditor::createInstance(spl);

	g->addMultiCellWidget(spl,1,1,0,2);

	QLabel * l = new QLabel(__tr2qs("Text:"),this);
	l->setMargin(2);
	g->addWidget(l,2,0);

	m_pTextEditor = new QLineEdit(this);
	QToolTip::add(m_pTextEditor,
		__tr2qs("<center><b>Visible text</b><br>May contain identifiers that will be evaluated at popup call time.<br>For labels, this text can contain also limited HTML tags.</center>"));
	g->addMultiCellWidget(m_pTextEditor,2,2,1,2);

	l = new QLabel(__tr2qs("Condition:"),this);
	l->setMargin(2);
	g->addWidget(l,3,0);

	m_pConditionEditor = new QLineEdit(this);
	QToolTip::add(m_pConditionEditor,
		__tr2qs("<center><b>Boolean condition</b><br>Will be evaluated at popup call time in order to decide if this entry has to be shown.<br>An empty condition evaluates to true.</center>"));
	g->addMultiCellWidget(m_pConditionEditor,3,3,1,2);

	l = new QLabel(__tr2qs("Icon:"),this);
	l->setMargin(2);
	g->addWidget(l,4,0);

	m_pIconEditor = new QLineEdit(this);
	QToolTip::add(m_pIconEditor,
		__tr2qs("<center><b>Icon identifier</b><br>May be an internal icon ID, an absolute path or a relative path.<br>Portable scripts should never use absolute paths.</center>"));
	g->addMultiCellWidget(m_pIconEditor,4,4,1,2);

	l = new QLabel(__tr2qs("External menu:"),this);
	l->setMargin(2);
	g->addWidget(l,5,0);

	m_pExtNameEditor = new QLineEdit(this);
	QToolTip::add(m_pExtNameEditor,
		__tr2qs("<center><b>External menu name</b><br>This allows to nest externally defined popup menus. The popup menu with the specified name will be looked up at menu setup time.</center>"));
	g->addMultiCellWidget(m_pExtNameEditor,5,5,1,2);

	l = new QLabel(__tr2qs("Item Id:"),this);
	l->setMargin(2);
	g->addWidget(l,6,0);


	m_pIdEditor = new QLineEdit(this);
	QToolTip::add(m_pIdEditor,
		__tr2qs("<center><b>Item id</b><br>This will allow you to use delpopupitem later.</center>"));
	g->addMultiCellWidget(m_pIdEditor,6,6,1,2);


	g->setColStretch(1,1);
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

KviPopupListViewItem * KviSinglePopupEditor::findMatchingItem(KviKvsPopupMenuItem * it,KviPopupListViewItem * item)
{
	
	if(it->type() != KviKvsPopupMenuItem::Item)goto not_this_one;
	if(item->m_type != KviPopupListViewItem::Item)goto not_this_one;
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
	item = (KviPopupListViewItem *)item->firstChild();
	while(item)
	{
		KviPopupListViewItem * found = findMatchingItem(it,item);
		if(found)return found;
		item = (KviPopupListViewItem *)item->nextSibling();
	}

	return 0;
}

void KviSinglePopupEditor::testModeMenuItemClicked(KviKvsPopupMenuItem * it)
{

	saveLastSelectedItem(); // that's the first thingie
	// find the matching item and set it as current
	KviPopupListViewItem * item = (KviPopupListViewItem *)m_pListView->firstChild();
	while(item)
	{
		KviPopupListViewItem * found = findMatchingItem(it,item);
		if(found)
		{
			// set the item as current
			m_pListView->ensureItemVisible(found);
			m_pListView->setCurrentItem(found);
			selectionChanged(found);
			return;
		}
		item = (KviPopupListViewItem *)item->nextSibling();
	}

}


void KviSinglePopupEditor::itemPressed(KviTalListViewItem *it,const QPoint &pnt,int col)
{
	m_pContextPopup->clear();

	KviPopupListViewItem * parentMenu = 0;
	bool bIsMenu = false;

	if(it)
	{
		parentMenu = (KviPopupListViewItem *) (((KviPopupListViewItem *)it)->parent());
		bIsMenu = ((KviPopupListViewItem *)it)->m_type == KviPopupListViewItem::Menu;
	}

	m_pContextPopup->insertItem(__tr2qs("New Separator Below"),this,SLOT(contextNewSeparatorBelow()));
	m_pContextPopup->setItemEnabled(
		m_pContextPopup->insertItem(__tr2qs("New Separator Above"),this,SLOT(contextNewSeparatorAbove())),
		it);
	m_pContextPopup->setItemEnabled(
		m_pContextPopup->insertItem(__tr2qs("New Separator Inside"),this,SLOT(contextNewSeparatorInside())),
		it && bIsMenu);

	m_pContextPopup->insertSeparator();

	m_pContextPopup->insertItem(__tr2qs("New Label Below"),this,SLOT(contextNewLabelBelow()));
	m_pContextPopup->setItemEnabled(
		m_pContextPopup->insertItem(__tr2qs("New Label Above"),this,SLOT(contextNewLabelAbove())),
		it);
	m_pContextPopup->setItemEnabled(
		m_pContextPopup->insertItem(__tr2qs("New Label Inside"),this,SLOT(contextNewLabelInside())),
		it && bIsMenu);

	m_pContextPopup->insertSeparator();

	m_pContextPopup->insertItem(__tr2qs("New Item Below"),this,SLOT(contextNewItemBelow()));
	m_pContextPopup->setItemEnabled(
		m_pContextPopup->insertItem(__tr2qs("New Item Above"),this,SLOT(contextNewItemAbove())),
		it);
	m_pContextPopup->setItemEnabled(
		m_pContextPopup->insertItem(__tr2qs("New Item Inside"),this,SLOT(contextNewItemInside())),
		it && bIsMenu);

	m_pContextPopup->insertSeparator();

	m_pContextPopup->insertItem(__tr2qs("New Menu Below"),this,SLOT(contextNewMenuBelow()));
	m_pContextPopup->setItemEnabled(
		m_pContextPopup->insertItem(__tr2qs("New Menu Above"),this,SLOT(contextNewMenuAbove())),
		it);
	m_pContextPopup->setItemEnabled(
		m_pContextPopup->insertItem(__tr2qs("New Menu Inside"),this,SLOT(contextNewMenuInside())),
		it && bIsMenu);

		m_pContextPopup->insertSeparator();

	m_pContextPopup->insertItem(__tr2qs("New External Menu Below"),this,SLOT(contextNewExtMenuBelow()));
	m_pContextPopup->setItemEnabled(
		m_pContextPopup->insertItem(__tr2qs("New External Menu Above"),this,SLOT(contextNewExtMenuAbove())),
		it);
	m_pContextPopup->setItemEnabled(
		m_pContextPopup->insertItem(__tr2qs("New External Menu Inside"),this,SLOT(contextNewExtMenuInside())),
		it && bIsMenu);

	m_pContextPopup->insertSeparator();

	m_pContextPopup->setItemEnabled(
		m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_CUT)),
			__tr2qs("Cu&t"),
			this,SLOT(contextCut())),
		it);
	m_pContextPopup->setItemEnabled(
		m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_COPY)),
			__tr2qs("&Copy"),
			this,SLOT(contextCopy())),
		it);
	m_pContextPopup->setItemEnabled(
		m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_PASTE)),
			__tr2qs("&Paste Below"),this,SLOT(contextPasteBelow())),
		m_pClipboard);
	m_pContextPopup->setItemEnabled(
		m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_PASTE)),
			__tr2qs("Paste Above"),this,SLOT(contextPasteAbove())),
		it && m_pClipboard);
	m_pContextPopup->setItemEnabled(
		m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_PASTE)),
			__tr2qs("Paste Inside"),this,SLOT(contextPasteInside())),
		it && bIsMenu && m_pClipboard);


	bool bSeparatorInserted = false;

//	if(!findPrologue(parentMenu))
//	{
		m_pContextPopup->insertSeparator();
		bSeparatorInserted = true;
		m_pContextPopup->insertItem(
				*(g_pIconManager->getSmallIcon(KVI_SMALLICON_PROLOGUE)),
				__tr2qs("New Menu Prologue"),this,SLOT(contextNewPrologue()));
//	}

//	if(!findEpilogue(parentMenu))
//	{
		if(!bSeparatorInserted)m_pContextPopup->insertSeparator();
		m_pContextPopup->insertItem(
				*(g_pIconManager->getSmallIcon(KVI_SMALLICON_EPILOGUE)),
				__tr2qs("New Menu Epilogue"),this,SLOT(contextNewEpilogue()));
//	}

	m_pContextPopup->popup(pnt);
}

void KviSinglePopupEditor::createNewItemAboveLastSelected(KviPopupListViewItem::Type t)
{
	m_pListView->setCurrentItem(newItemAbove(m_pLastSelectedItem,t));
}

void KviSinglePopupEditor::createNewItemBelowLastSelected(KviPopupListViewItem::Type t)
{
	m_pListView->setCurrentItem(newItemBelow(m_pLastSelectedItem,t));
}

void KviSinglePopupEditor::createNewItemInsideLastSelected(KviPopupListViewItem::Type t)
{
	if(m_pLastSelectedItem)m_pLastSelectedItem->setOpen(true);
	m_pListView->setCurrentItem(newItemInside(m_pLastSelectedItem,t));
}

void KviSinglePopupEditor::contextNewSeparatorAbove()
{
	createNewItemAboveLastSelected(KviPopupListViewItem::Separator);
}

void KviSinglePopupEditor::contextNewSeparatorBelow()
{
	createNewItemBelowLastSelected(KviPopupListViewItem::Separator);
}

void KviSinglePopupEditor::contextNewSeparatorInside()
{
	createNewItemInsideLastSelected(KviPopupListViewItem::Separator);
}

void KviSinglePopupEditor::contextNewItemBelow()
{
	createNewItemBelowLastSelected(KviPopupListViewItem::Item);
}

void KviSinglePopupEditor::contextNewItemAbove()
{
	createNewItemAboveLastSelected(KviPopupListViewItem::Item);
}

void KviSinglePopupEditor::contextNewItemInside()
{
	createNewItemInsideLastSelected(KviPopupListViewItem::Item);
}

void KviSinglePopupEditor::contextNewMenuBelow()
{
	createNewItemBelowLastSelected(KviPopupListViewItem::Menu);
}

void KviSinglePopupEditor::contextNewMenuAbove()
{
	createNewItemAboveLastSelected(KviPopupListViewItem::Menu);
}

void KviSinglePopupEditor::contextNewMenuInside()
{
	createNewItemInsideLastSelected(KviPopupListViewItem::Menu);
}

void KviSinglePopupEditor::contextNewExtMenuBelow()
{
	createNewItemBelowLastSelected(KviPopupListViewItem::ExtMenu);
}

void KviSinglePopupEditor::contextNewExtMenuAbove()
{
	createNewItemAboveLastSelected(KviPopupListViewItem::ExtMenu);
}
void KviSinglePopupEditor::contextNewExtMenuInside()
{
	createNewItemInsideLastSelected(KviPopupListViewItem::ExtMenu);
}


void KviSinglePopupEditor::contextNewLabelBelow()
{
	createNewItemBelowLastSelected(KviPopupListViewItem::Label);
}

void KviSinglePopupEditor::contextNewLabelAbove()
{
	createNewItemAboveLastSelected(KviPopupListViewItem::Label);
}

void KviSinglePopupEditor::contextNewLabelInside()
{
	createNewItemInsideLastSelected(KviPopupListViewItem::Label);
}

KviPopupListViewItem * KviSinglePopupEditor::newItem(KviPopupListViewItem * par,KviPopupListViewItem * after,KviPopupListViewItem::Type t)
{
	if(par)return new KviPopupListViewItem(par,after,t);
	return new KviPopupListViewItem(m_pListView,after,t);
}

KviPopupListViewItem * KviSinglePopupEditor::newItemBelow(KviPopupListViewItem * it,KviPopupListViewItem::Type t)
{
	if(!it)return newItem(0,0,t);
	return newItem((KviPopupListViewItem *)it->parent(),it,t);
}

KviPopupListViewItem * KviSinglePopupEditor::newItemAbove(KviPopupListViewItem * it,KviPopupListViewItem::Type t)
{
	if(!it)return newItem(0,0,t);
	return newItem((KviPopupListViewItem *)it->parent(),(KviPopupListViewItem *)it->itemAbove(),t);
}

KviPopupListViewItem * KviSinglePopupEditor::newItemInside(KviPopupListViewItem * it,KviPopupListViewItem::Type t)
{
	if(it)
	{
		if(it->m_type != KviPopupListViewItem::Menu)
		{
			return newItemBelow(it,t);
		}
	}
	return newItem(it,it,t);
}

void KviSinglePopupEditor::contextNewPrologue()
{
	KviPopupListViewItem * it = m_pLastSelectedItem ? (KviPopupListViewItem *)m_pLastSelectedItem->parent() : 0;
//	if(!findPrologue(it))
//	{
		m_pListView->setCurrentItem(newItem(it,it,KviPopupListViewItem::Prologue));
//	}
}

void KviSinglePopupEditor::contextNewEpilogue()
{
	KviPopupListViewItem * it = m_pLastSelectedItem ? (KviPopupListViewItem *)m_pLastSelectedItem->parent() : 0;
//	if(!findEpilogue(it))
//	{
		KviPopupListViewItem * after = it ? (KviPopupListViewItem *)it->firstChild() : (KviPopupListViewItem *)m_pListView->firstChild();
		if(after)
		{
			while(after->nextSibling())after = (KviPopupListViewItem *)after->nextSibling();
		} else {
			after = it;
		}
		m_pListView->setCurrentItem(newItem(it,after,KviPopupListViewItem::Epilogue));
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

	KviPopupListViewItem * it = m_pLastSelectedItem;
	m_pLastSelectedItem = 0;
	delete it;
	if(!m_pLastSelectedItem)selectionChanged(0);
}


void KviSinglePopupEditor::contextPasteBelow()
{
	if(!m_pClipboard)return;
	KviPopupListViewItem * par = m_pLastSelectedItem ? (KviPopupListViewItem *)m_pLastSelectedItem->parent() : 0;
	populateMenu(m_pClipboard,par,m_pLastSelectedItem);
}

void KviSinglePopupEditor::contextPasteAbove()
{
	if(!m_pClipboard)return;
	KviPopupListViewItem * par = m_pLastSelectedItem ? (KviPopupListViewItem *)m_pLastSelectedItem->parent() : 0;
	KviPopupListViewItem * above = m_pLastSelectedItem ? (KviPopupListViewItem *)m_pLastSelectedItem->itemAbove() : 0;
	populateMenu(m_pClipboard,par,above);

}

void KviSinglePopupEditor::contextPasteInside()
{
	if(!m_pClipboard)return;
	if(m_pLastSelectedItem)
	{
		if(m_pLastSelectedItem->m_type != KviPopupListViewItem::Menu)
		{
			contextPasteBelow();
			return;
		}
		m_pLastSelectedItem->setOpen(true);
	}
	populateMenu(m_pClipboard,m_pLastSelectedItem,0);
}

void KviSinglePopupEditor::saveLastSelectedItem()
{
	if(!m_pLastSelectedItem)return;

	QString tmp;

	switch(m_pLastSelectedItem->m_type)
	{
		case KviPopupListViewItem::Prologue:
		case KviPopupListViewItem::Epilogue:
		case KviPopupListViewItem::Item:
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
		case KviPopupListViewItem::Menu:
		case KviPopupListViewItem::ExtMenu:
		case KviPopupListViewItem::Item:
			m_pLastSelectedItem->setIcon(m_pIconEditor->text());
		break;
		default:
		break;
	}

	switch(m_pLastSelectedItem->m_type)
	{
		case KviPopupListViewItem::Menu:
		case KviPopupListViewItem::Item:
		case KviPopupListViewItem::Label:
		case KviPopupListViewItem::ExtMenu:
			m_pLastSelectedItem->setItemText(m_pTextEditor->text());
			m_pLastSelectedItem->setCondition(m_pConditionEditor->text());
		break;
		default:
		break;
	}

	if(m_pLastSelectedItem->m_type == KviPopupListViewItem::ExtMenu)
	{
		m_pLastSelectedItem->setCode(m_pExtNameEditor->text());
	}
}

void KviSinglePopupEditor::addItemToMenu(KviKvsPopupMenu * p,KviPopupListViewItem *it)
{
	it->m_szId.stripWhiteSpace();
	switch(it->m_type)
	{
		case KviPopupListViewItem::Prologue:
			it->m_szCode.stripWhiteSpace();
			p->addPrologue(it->m_szId,it->m_szCode);
		break;
		case KviPopupListViewItem::Epilogue:
			it->m_szCode.stripWhiteSpace();
			p->addEpilogue(it->m_szId,it->m_szCode);
		break;
		case KviPopupListViewItem::Separator:
			it->m_szCondition.stripWhiteSpace();
			p->addSeparator(it->m_szId,it->m_szCondition);
		break;
		case KviPopupListViewItem::Label:
			it->m_szText.stripWhiteSpace();
			it->m_szCondition.stripWhiteSpace();
			it->m_szIcon.stripWhiteSpace();
			p->addLabel(it->m_szId,it->m_szText,it->m_szIcon,it->m_szCondition);
		break;
		case KviPopupListViewItem::Item:
			it->m_szText.stripWhiteSpace();
			it->m_szIcon.stripWhiteSpace();
			it->m_szCondition.stripWhiteSpace();
			it->m_szCode.stripWhiteSpace();
			p->addItem(it->m_szId,it->m_szCode,it->m_szText,it->m_szIcon,it->m_szCondition);
		break;
		case KviPopupListViewItem::ExtMenu:
			it->m_szText.stripWhiteSpace();
			it->m_szIcon.stripWhiteSpace();
			it->m_szCondition.stripWhiteSpace();
			it->m_szCode.stripWhiteSpace();// <-- this is the ext name in fact
			p->addExtPopup(it->m_szId,it->m_szCode,it->m_szText,it->m_szIcon,it->m_szCondition);
		break;
		case KviPopupListViewItem::Menu:
		{
			it->m_szText.stripWhiteSpace();
			it->m_szIcon.stripWhiteSpace();
			it->m_szCondition.stripWhiteSpace();
			KviKvsPopupMenu * menu = p->addPopup(it->m_szId,it->m_szText,it->m_szIcon,it->m_szCondition);
			for(KviPopupListViewItem * item = (KviPopupListViewItem *)it->firstChild();item;item = (KviPopupListViewItem *)item->nextSibling())
			{
				addItemToMenu(menu,item);
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
	tmp.stripWhiteSpace();

	KviKvsPopupMenu * p = new KviKvsPopupMenu(tmp);
	for(KviPopupListViewItem * it = (KviPopupListViewItem *)m_pListView->firstChild();it;it = (KviPopupListViewItem *)it->nextSibling())
	{
		addItemToMenu(p,it);
	}
	return p;
}

void KviSinglePopupEditor::selectionChanged(KviTalListViewItem * it)
{
	saveLastSelectedItem();

	bool bEditorEnabled = false;
	bool bIconEditorEnabled = false;
	bool bConditionEditorEnabled = false;
	bool bTextEditorEnabled = false;
	bool bNameEditorEnabled = false;

	if(it)
	{
		m_pIdEditor->setText(((KviPopupListViewItem *)it)->m_szId);

		switch(((KviPopupListViewItem *)it)->m_type)
		{
			case KviPopupListViewItem::Prologue:
			case KviPopupListViewItem::Epilogue:
			case KviPopupListViewItem::Item:
				m_pEditor->setText(((KviPopupListViewItem *)it)->m_szCode);
				bEditorEnabled = true;
			break;
			default:
			break;
		}

		switch(((KviPopupListViewItem *)it)->m_type)
		{
			case KviPopupListViewItem::Menu:
			case KviPopupListViewItem::Item:
			case KviPopupListViewItem::Label:
			case KviPopupListViewItem::ExtMenu:
				m_pIconEditor->setText(((KviPopupListViewItem *)it)->m_szIcon);
				bIconEditorEnabled = true;
			break;
			default:
			break;		}

		switch(((KviPopupListViewItem *)it)->m_type)
		{
			case KviPopupListViewItem::Menu:
			case KviPopupListViewItem::Item:
			case KviPopupListViewItem::Label:
			case KviPopupListViewItem::ExtMenu:
				m_pConditionEditor->setText(((KviPopupListViewItem *)it)->m_szCondition);
				bConditionEditorEnabled = true;
				m_pTextEditor->setText(((KviPopupListViewItem *)it)->m_szText);
				bTextEditorEnabled = true;
			break;
			default:
			break;
		}

		if(((KviPopupListViewItem *)it)->m_type == KviPopupListViewItem::ExtMenu)
		{
			m_pExtNameEditor->setText(((KviPopupListViewItem *)it)->m_szCode);
			bNameEditorEnabled =true;
		}
	}

	m_pLastSelectedItem = (KviPopupListViewItem *)it;

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

void KviSinglePopupEditor::populateMenu(KviKvsPopupMenu * pop,KviPopupListViewItem * par,KviPopupListViewItem * theItem)
{
	if(!pop)return;

	for(KviKvsScript * sp = pop->prologues()->first();sp;sp = pop->prologues()->next())
	{
		if(par)theItem = new KviPopupListViewItem(par,theItem,KviPopupListViewItem::Prologue);
		else theItem = new KviPopupListViewItem(m_pListView,theItem,KviPopupListViewItem::Prologue);
		theItem->setCode(sp->code());
		theItem->setId(sp->name());
	}

	for(KviKvsPopupMenuItem * item = pop->m_pItemList->first();item;item = pop->m_pItemList->next())
	{
		switch(item->type())
		{
			case KviKvsPopupMenuItem::Item:
				if(par)theItem = new KviPopupListViewItem(par,theItem,KviPopupListViewItem::Item);
				else theItem = new KviPopupListViewItem(m_pListView,theItem,KviPopupListViewItem::Item);
				theItem->setIcon(item->kvsIcon() ? item->kvsIcon()->code() : QString::null);
				theItem->setItemText(item->kvsText() ? item->kvsText()->code() : QString::null);
				theItem->setCondition(item->kvsCondition() ? item->kvsCondition()->code() : QString::null);
				theItem->setCode(item->kvsCode() ? item->kvsCode()->code() : QString::null);
				theItem->setId(item->name());
			break;
			case KviKvsPopupMenuItem::ExtMenu:
				if(par)theItem = new KviPopupListViewItem(par,theItem,KviPopupListViewItem::ExtMenu);
				else theItem = new KviPopupListViewItem(m_pListView,theItem,KviPopupListViewItem::ExtMenu);
				theItem->setIcon(item->kvsIcon() ? item->kvsIcon()->code() : QString::null);
				theItem->setItemText(item->kvsText() ? item->kvsText()->code() : QString::null);
				theItem->setCondition(item->kvsCondition() ? item->kvsCondition()->code() : QString::null);
				theItem->setCode(((KviKvsPopupMenuItemExtMenu *)item)->extName());
				theItem->setId(item->name());
			break;
			case KviKvsPopupMenuItem::Label:
				if(par)theItem = new KviPopupListViewItem(par,theItem,KviPopupListViewItem::Label);
				else theItem = new KviPopupListViewItem(m_pListView,theItem,KviPopupListViewItem::Label);
				theItem->setIcon(item->kvsIcon() ? item->kvsIcon()->code() : QString::null);
				theItem->setItemText(item->kvsText() ? item->kvsText()->code() : QString::null);
				theItem->setCondition(item->kvsCondition() ? item->kvsCondition()->code() : QString::null);
				theItem->setId(item->name());
			break;
			case KviKvsPopupMenuItem::Separator:
				if(par)theItem = new KviPopupListViewItem(par,theItem,KviPopupListViewItem::Separator);
				else theItem = new KviPopupListViewItem(m_pListView,theItem,KviPopupListViewItem::Separator);
				theItem->setId(item->name());
			break;
			case KviKvsPopupMenuItem::Menu:
				if(par)theItem = new KviPopupListViewItem(par,theItem,KviPopupListViewItem::Menu);
				else theItem = new KviPopupListViewItem(m_pListView,theItem,KviPopupListViewItem::Menu);
				theItem->setIcon(item->kvsIcon() ? item->kvsIcon()->code() : QString::null);
				theItem->setItemText(item->kvsText() ? item->kvsText()->code() : QString::null);
				theItem->setCondition(item->kvsCondition() ? item->kvsCondition()->code() : QString::null);
				theItem->setId(item->name());
				populateMenu(((KviKvsPopupMenuItemMenu *)item)->menu(),theItem,0);
			break;
			default:
			break;
		}
	}

	for(KviKvsScript * se = pop->epilogues()->first();se;se = pop->epilogues()->next())
	{
		if(par)theItem = new KviPopupListViewItem(par,theItem,KviPopupListViewItem::Epilogue);
		else theItem = new KviPopupListViewItem(m_pListView,theItem,KviPopupListViewItem::Epilogue);
		theItem->setCode(se->code());
		theItem->setId(se->name());
	}
}



void KviSinglePopupEditor::edit(KviMenuListViewItem * it)
{
	saveLastSelectedItem();

	m_pLastSelectedItem = 0;

	m_pListView->clear();

	selectionChanged(0);

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
	m_pListView->setEnabled(it);
	m_pNameEditor->setEnabled(it);
	m_pMenuButton->setEnabled(it);

}

















KviMenuListViewItem::KviMenuListViewItem(KviTalListView * par,KviKvsPopupMenu * popup)
: KviTalListViewItem(par)
{
	setPixmap(0,*(g_pIconManager->getSmallIcon(KVI_SMALLICON_POPUP)));
	setText(0,popup->name());
	m_pPopup = popup;
}

KviMenuListViewItem::~KviMenuListViewItem()
{
	delete m_pPopup;
}

void KviMenuListViewItem::replacePopup(KviKvsPopupMenu * popup)
{
	delete m_pPopup;
	m_pPopup = popup;
}


KviPopupEditor::KviPopupEditor(QWidget * par)
: QWidget(par)
{
	QGridLayout * l = new QGridLayout(this,1,1,0,2);
#ifdef COMPILE_USE_QT4
	QSplitter * spl = new QSplitter(Qt::Horizontal,this,"popupeditor");
	spl->setOpaqueResize(false);
#else
	QSplitter * spl = new QSplitter(QSplitter::Horizontal,this);
#endif
	l->addWidget(spl,0,0);
	
	KviTalVBox * box = new KviTalVBox(spl);

	m_pListView = new KviTalListView(box);
	m_pListView->addColumn(__tr2qs("Popup"));
	m_pListView->setSelectionMode(KviTalListView::Extended);
	m_pListView->setShowSortIndicator(true);

	QPushButton * pb = new QPushButton(__tr2qs("&Export All To..."),box);
	connect(pb,SIGNAL(clicked()),this,SLOT(exportAll()));
	QPushButton * gn = new QPushButton(__tr2qs("&Export selected To..."),box);
	connect(gn,SIGNAL(clicked()),this,SLOT(exportSelected()));

	m_pEditor = new KviSinglePopupEditor(spl);

	m_bOneTimeSetupDone = false;
	m_pLastEditedItem = 0;

	m_pContextPopup = new KviTalPopupMenu(this);

	currentItemChanged(0);
}

KviPopupEditor::~KviPopupEditor()
{
}

void KviPopupEditor::oneTimeSetup()
{
	if(m_bOneTimeSetupDone)return;
	m_bOneTimeSetupDone = true;

	const KviDict<KviKvsPopupMenu> * a = KviKvsPopupManager::instance()->popupDict();
	if(!a)return;

	KviDictIterator<KviKvsPopupMenu> it(*a);

	KviMenuListViewItem * item;

	while(it.current())
	{
		KviKvsPopupMenu * popup = it.current();
		KviKvsPopupMenu * copy = new KviKvsPopupMenu(popup->popupName());
		copy->copyFrom(popup);
		item = new KviMenuListViewItem(m_pListView,copy);
		++it;
	}

	connect(m_pListView,SIGNAL(currentChanged(KviTalListViewItem *)),this,SLOT(currentItemChanged(KviTalListViewItem *)));
	connect(m_pListView,SIGNAL(rightButtonPressed(KviTalListViewItem *,const QPoint &,int)),
		this,SLOT(itemPressed(KviTalListViewItem *,const QPoint &,int)));

}

void KviPopupEditor::itemPressed(KviTalListViewItem *it,const QPoint &pnt,int col)
{
	__range_valid(m_bOneTimeSetupDone);
	m_pContextPopup->clear();

	m_pContextPopup->insertItem(
		*(g_pIconManager->getSmallIcon(KVI_SMALLICON_POPUP)),
		__tr2qs("&New Popup"),
		this,SLOT(newPopup()));

	m_pContextPopup->setItemEnabled(
		m_pContextPopup->insertItem(
				*(g_pIconManager->getSmallIcon(KVI_SMALLICON_QUIT)),
				__tr2qs("Re&move Popup"),
				this,SLOT(removeCurrentPopup())),
	it);
	
	m_pContextPopup->setItemEnabled(
		m_pContextPopup->insertItem(
				*(g_pIconManager->getSmallIcon(KVI_SMALLICON_FOLDER)),
				__tr2qs("&Export Popup To..."),
				this,SLOT(exportCurrentPopup())),
	it);

	m_pContextPopup->popup(pnt);
}


void KviPopupEditor::exportCurrentPopup()
{
	if(!m_pLastEditedItem)return;
	saveLastEditedItem();
	if(!m_pLastEditedItem)return;

	QString szName = QDir::homeDirPath();
	if(!szName.endsWith(QString(KVI_PATH_SEPARATOR)))szName += KVI_PATH_SEPARATOR;
	szName += m_pLastEditedItem->popup()->name();
	szName += ".kvs";
	
	QString szFile;
	
	if(!KviFileDialog::askForSaveFileName(szFile,__tr2qs("Choose a Filename - KVIrc"),szName,"*.kvs",true,true,true))return;

	QString szOut;
	m_pLastEditedItem->popup()->generateDefPopup(szOut);
	
	if(!KviFileUtils::writeFile(szFile,szOut))
	{
		QMessageBox::warning(this,__tr2qs("Write Failed - KVIrc"),__tr2qs("Unable to write to the popup file."),__tr2qs("Ok"));
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

	KviMenuListViewItem * it = (KviMenuListViewItem *)m_pListView->firstChild();

	QString out;

	int count=0;
	KviTalListViewItemIterator itv( m_pListView );
	while( itv.current() )
	{
		if ( (itv.current()->isSelected()) || (bSelectedOnly == true) )
		{
			count ++;
			QString tmp;
			it->popup()->generateDefPopup(tmp);
			out += tmp;
			out += "\n";
		}
		it = (KviMenuListViewItem *)it->nextSibling();
		++itv;
	}
	if (!count && bSelectedOnly==false) return;

	QString szName = QDir::homeDirPath();
	if(!szName.endsWith(QString(KVI_PATH_SEPARATOR)))szName += KVI_PATH_SEPARATOR;
	szName += "popups.kvs";
	
	QString szFile;
	
	if(!KviFileDialog::askForSaveFileName(szFile,__tr2qs("Choose a Filename - KVIrc"),szName,"*.kvs",true,true,true))return;

	if(!KviFileUtils::writeFile(szFile,out))
	{
		QMessageBox::warning(this,__tr2qs("Write Failed - KVIrc"),__tr2qs("Unable to write to the alias file."),__tr2qs("Ok"));
	}
}


void KviPopupEditor::removeCurrentPopup()
{
	if(m_pLastEditedItem)
	{
		KviMenuListViewItem * it = m_pLastEditedItem;
		m_pLastEditedItem = 0;
		delete it;
		if(!m_pLastEditedItem)currentItemChanged(0);
	}
}

void KviPopupEditor::newPopup()
{
	QString newName;
	getUniquePopupName(0,newName);
	KviMenuListViewItem * it = new KviMenuListViewItem(m_pListView,new KviKvsPopupMenu(newName));
	m_pListView->setCurrentItem(it);
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

void KviPopupEditor::currentItemChanged(KviTalListViewItem *it)
{
	saveLastEditedItem();

	m_pLastEditedItem = (KviMenuListViewItem *)it;

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

	KviMenuListViewItem * it = (KviMenuListViewItem *)m_pListView->firstChild();

	// Copy the original popup dict
	KviDict<KviKvsPopupMenu> copy(*(KviKvsPopupManager::instance()->popupDict()));
	copy.setAutoDelete(false);

	while(it)
	{
		KviKvsPopupMenu * p = KviKvsPopupManager::instance()->get(it->m_pPopup->popupName());
		p->doClear();
		p->copyFrom(it->m_pPopup);
		it = (KviMenuListViewItem *)it->nextSibling();
		// remove it from the original copy
		copy.remove(p->popupName());
	}

	// the remaining elements in the copy need to be removed from
	// the "new" dictionary (they are no longer used)
	KviDictIterator<KviKvsPopupMenu> iter(copy);

	while(iter.current())
	{
		KviKvsPopupManager::instance()->remove(iter.currentKey());
		++iter;
	}
	

	g_pApp->savePopups();
}


void KviPopupEditor::getUniquePopupName(KviMenuListViewItem *item,QString &buffer)
{
	__range_valid(m_bOneTimeSetupDone);

	if(buffer.isEmpty())buffer = __tr2qs("unnamed");
	QString newName = buffer;

	bool bFound = true;
	int idx = 1;

	while(bFound)
	{
		bFound = false;

		for(KviMenuListViewItem * ch = (KviMenuListViewItem *)(m_pListView->firstChild());ch;ch = (KviMenuListViewItem *)ch->nextSibling())
		{
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

	m_pEditor = new KviPopupEditor(this);

	m_pBase = new QWidget(this);
	QGridLayout * g = new QGridLayout(m_pBase,1,4,4,4);

	QPushButton * btn = new QPushButton(__tr2qs("&OK"),m_pBase);
	connect(btn,SIGNAL(clicked()),this,SLOT(okClicked()));
	btn->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)));
	g->addWidget(btn,0,1);

	btn = new QPushButton(__tr2qs("&Apply"),m_pBase);
	connect(btn,SIGNAL(clicked()),this,SLOT(applyClicked()));
	btn->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)));
	g->addWidget(btn,0,2);

	btn = new QPushButton(__tr2qs("Cancel"),m_pBase);
	connect(btn,SIGNAL(clicked()),this,SLOT(cancelClicked()));
	btn->setIconSet(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DISCARD)));
	g->addWidget(btn,0,3);

	g->setColStretch(0,1);
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

void KviPopupEditorWindow::resizeEvent(QResizeEvent *e)
{
	int hght = m_pBase->sizeHint().height();
	m_pEditor->setGeometry(0,0,width(),height()- hght);
	m_pBase->setGeometry(0,height() - hght,width(),hght);
}

void KviPopupEditorWindow::fillCaptionBuffers()
{
	m_szPlainTextCaption = __tr2qs("Popup Editor");

	static QString p1("<nobr><font color=\"");
	static QString p2("\"><b>");
	static QString p3("</b></font></nobr>");

	m_szHtmlActiveCaption = p1;
	m_szHtmlActiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextActive).name();
	m_szHtmlActiveCaption += p2;
	m_szHtmlActiveCaption += m_szPlainTextCaption;
	m_szHtmlActiveCaption += p3;

	m_szHtmlInactiveCaption = p1;
	m_szHtmlInactiveCaption += KVI_OPTION_COLOR(KviOption_colorCaptionTextInactive).name();
	m_szHtmlInactiveCaption += p2;
	m_szHtmlInactiveCaption += m_szPlainTextCaption;
	m_szHtmlInactiveCaption += p3;

}


void KviPopupEditorWindow::getConfigGroupName(KviStr &szName)
{
	szName = "popupeditor";
}


void KviPopupEditorWindow::saveProperties(KviConfig *cfg)
{
/*
#ifdef COMPILE_SCRIPTTOOLBAR
	cfg->writeEntry("Sizes",m_pEditor->sizes());
	cfg->writeEntry("LastPopup",m_pEditor->lastEditedPopup().ptr());
	//debug("LAST EDITED=%s",m_pEditor->lastEditedPopup().ptr());
#endif // COMPILE_SCRIPTTOOLBAR
*/
}

void KviPopupEditorWindow::loadProperties(KviConfig *cfg)
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
