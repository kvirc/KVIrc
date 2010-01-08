//=============================================================================
//
//   File : aliaseditor.cpp
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


#include "aliaseditor.h"

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
#include "kvi_kvs_aliasmanager.h"
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

extern KviAliasEditorWindow * g_pAliasEditorWindow;
extern KviModule * g_pAliasEditorModule;

KviAliasEditorTreeWidgetItem::KviAliasEditorTreeWidgetItem(QTreeWidget * pTreeWidget,Type eType,const QString &szName)
: QTreeWidgetItem(pTreeWidget), KviHeapObject(), m_eType(eType), m_pParentNamespaceItem(0)
{
	setName(szName);
}

KviAliasEditorTreeWidgetItem::KviAliasEditorTreeWidgetItem(KviAliasNamespaceTreeWidgetItem * pParentNamespaceItem,Type eType,const QString &szName)
: QTreeWidgetItem(pParentNamespaceItem), m_eType(eType), m_pParentNamespaceItem(pParentNamespaceItem)
{
	setName(szName);
	setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled|Qt::ItemIsSelectable);
}



QString KviAliasEditorTreeWidgetItem::key(int, bool) const
{
	QString ret;
	if(m_eType == Namespace)ret = "1";
	else ret = "2";
	ret += m_szName;
	return ret;
}

void KviAliasEditorTreeWidgetItem::setName(const QString &szName)
{
	m_szName = szName;
	setText(0,m_szName);
}

KviAliasTreeWidgetItem::KviAliasTreeWidgetItem(KviAliasNamespaceTreeWidgetItem * pParentNamespaceItem,const QString &szName)
: KviAliasEditorTreeWidgetItem(pParentNamespaceItem,KviAliasEditorTreeWidgetItem::Alias,szName)
{
	setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIAS))));
	m_cPos=0;

}


KviAliasTreeWidgetItem::KviAliasTreeWidgetItem(QTreeWidget * pTreeWidget,const QString &szName)
: KviAliasEditorTreeWidgetItem(pTreeWidget,KviAliasEditorTreeWidgetItem::Alias,szName)
{
	setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIAS))));
	m_cPos=0;

}

KviAliasTreeWidgetItem::~KviAliasTreeWidgetItem()
{
}

KviAliasNamespaceTreeWidgetItem::KviAliasNamespaceTreeWidgetItem(QTreeWidget * pTreeWidget,const QString &szName)
: KviAliasEditorTreeWidgetItem(pTreeWidget,KviAliasEditorTreeWidgetItem::Namespace,szName)
{
	setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NAMESPACE))));
}

KviAliasNamespaceTreeWidgetItem::KviAliasNamespaceTreeWidgetItem(KviAliasNamespaceTreeWidgetItem * pParentNamespaceItem,const QString &szName)
: KviAliasEditorTreeWidgetItem(pParentNamespaceItem,KviAliasEditorTreeWidgetItem::Namespace,szName)
{
	setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NAMESPACE))));
}

KviAliasNamespaceTreeWidgetItem::~KviAliasNamespaceTreeWidgetItem()
{
}

KviAliasNamespaceTreeWidgetItem * KviAliasNamespaceTreeWidgetItem::findNamespaceItem(const QString &szName)
{
	for (int i=0;i<childCount();i++)
	{
		if (((KviAliasEditorTreeWidgetItem *)child(i))->isNamespace())
			if(KviQString::equalCI(szName,((KviAliasEditorTreeWidgetItem *)child(i))->name()))return (KviAliasNamespaceTreeWidgetItem *)child(i);
	}
	return 0;
}

KviAliasNamespaceTreeWidgetItem * KviAliasNamespaceTreeWidgetItem::getNamespaceItem(const QString &szName)
{
	KviAliasNamespaceTreeWidgetItem * it = findNamespaceItem(szName);
	if(it)return it;
	it = new KviAliasNamespaceTreeWidgetItem(this,szName);
	return it;
}

KviAliasTreeWidgetItem * KviAliasNamespaceTreeWidgetItem::findAliasItem(const QString &szName)
{
	for (int i=0;i<childCount();i++)
	{
		if (((KviAliasEditorTreeWidgetItem *)child(i))->isAlias())
			if(KviQString::equalCI(szName,((KviAliasEditorTreeWidgetItem *)child(i))->name()))return (KviAliasTreeWidgetItem *)child(i);
	}
	return 0;
}

KviAliasTreeWidgetItem * KviAliasNamespaceTreeWidgetItem::getAliasItem(const QString &szName)
{
	KviAliasTreeWidgetItem * it = findAliasItem(szName);
	if(it)return it;
	it = new KviAliasTreeWidgetItem(this,szName);
	return it;
}


KviAliasTreeWidgetItem * KviAliasNamespaceTreeWidgetItem::createFullAliasItem(const QString &szFullName)
{
	QStringList lNamespaces;
	QString szName;
	KviAliasEditor::splitFullAliasOrNamespaceName(szFullName,lNamespaces,szName);

	if(lNamespaces.isEmpty())
		return getAliasItem(szName);

	QStringList::Iterator it = lNamespaces.begin();

	KviAliasNamespaceTreeWidgetItem * nit = getNamespaceItem(*it);
	++it;
	while(it != lNamespaces.end())
	{
		nit = nit->getNamespaceItem(*it);
		++it;
	}

	return nit->getAliasItem(szName);
}

KviAliasNamespaceTreeWidgetItem * KviAliasNamespaceTreeWidgetItem::createFullNamespaceItem(const QString &szFullName)
{
	QStringList lNamespaces;
	QString szName;
	KviAliasEditor::splitFullAliasOrNamespaceName(szFullName,lNamespaces,szName);

	if(lNamespaces.isEmpty())
		return getNamespaceItem(szName);

	QStringList::Iterator it = lNamespaces.begin();

	KviAliasNamespaceTreeWidgetItem * nit = getNamespaceItem(*it);
	++it;
	while(it != lNamespaces.end())
	{
		nit = nit->getNamespaceItem(*it);
		++it;
	}

	return nit->getNamespaceItem(szName);
}
KviAliasEditorTreeWidget::KviAliasEditorTreeWidget(QWidget * par)
: QTreeWidget(par)
{
	setColumnCount (1);
	setHeaderLabel(__tr2qs_ctx("Alias","editor"));
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setSortingEnabled(true);
	setRootIsDecorated(true);
	setAnimated(true);
}

KviAliasEditorTreeWidget::~KviAliasEditorTreeWidget()
{
	//remove and delete all items
	clear();
}

void KviAliasEditorTreeWidget::mousePressEvent (QMouseEvent *e)
{
		if (e->button() == Qt::RightButton)
		{
			QTreeWidgetItem *i=itemAt(e->pos());
			if (i) emit rightButtonPressed(i,QCursor::pos());
		}
		QTreeWidget::mousePressEvent(e);
}
KviAliasEditor::KviAliasEditor(QWidget * par)
: QWidget(par)
{
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

	m_pTreeWidget = new KviAliasEditorTreeWidget(box);


	box = new KviTalVBox(m_pSplitter);
	KviTalHBox * hbox = new KviTalHBox(box);
	hbox->setSpacing(0);
	hbox->setMargin(0);
	m_pNameLabel = new QLabel(__tr2qs_ctx("No item selected","editor"),hbox);
	m_pRenameButton = new QPushButton(__tr2qs_ctx("Rename","editor"),hbox);
	m_pRenameButton->setEnabled(false);
	connect(m_pRenameButton,SIGNAL(clicked()),this,SLOT(renameItem()));
	hbox->setStretchFactor(m_pNameLabel,2);
	m_pRenameButton->setToolTip(__tr2qs_ctx("Edit the alias or namespace name","editor"));
	m_pEditor = KviScriptEditor::createInstance(box);
	m_pEditor->setFocus();
	connect(m_pEditor,SIGNAL(find(const QString &)),this,SLOT(slotFindWord(const QString &)));
	connect(m_pEditor,SIGNAL(replaceAll(const QString &,const QString &)),this,SLOT(slotReplaceAll(const QString &,const QString &)));

	m_pContextPopup = new KviTalPopupMenu(this);

	oneTimeSetup();

	currentItemChanged(0,0);
}

KviAliasEditor::~KviAliasEditor()
{
	KviScriptEditor::destroyInstance(m_pEditor);
}

KviAliasNamespaceTreeWidgetItem * KviAliasEditor::findNamespaceItem(const QString &szName)
{

	int count=m_pTreeWidget->topLevelItemCount();
	for (int i=0;i<count;i++)
	{
		KviAliasNamespaceTreeWidgetItem * item=(KviAliasNamespaceTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		if(KviQString::equalCI(szName,item->name()))
			return item;
	}
	return 0;
}

KviAliasNamespaceTreeWidgetItem * KviAliasEditor::getNamespaceItem(const QString &szName)
{
	KviAliasNamespaceTreeWidgetItem * it = findNamespaceItem(szName);
	if(it)return it;
	it = new KviAliasNamespaceTreeWidgetItem(m_pTreeWidget,szName);
	return it;
}
KviAliasTreeWidgetItem * KviAliasEditor::findAliasItemRecursive(KviAliasEditorTreeWidgetItem *parent,const QString &szName)
{
	for (int i=0;i<parent->childCount();i++)
	{
		KviAliasEditorTreeWidgetItem *item=(KviAliasEditorTreeWidgetItem *)parent->child(i);

		if (!item->isAlias()){
			KviAliasTreeWidgetItem * itemret=findAliasItemRecursive(item,szName);
			if (itemret) return (KviAliasTreeWidgetItem*)itemret;
		}
		else{
			if(KviQString::equalCI(szName,item->name()))
				return (KviAliasTreeWidgetItem*)item;
		}
	}
	return false;
}

KviAliasTreeWidgetItem * KviAliasEditor::findAliasItem(const QString &szName)
{
	for (int i=0;i<m_pTreeWidget->topLevelItemCount();i++)
	{
		KviAliasEditorTreeWidgetItem *item=(KviAliasEditorTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		if (!item->isAlias())
		{
			KviAliasTreeWidgetItem * it=findAliasItemRecursive(item,szName);
			if (it) return it;
		}
		else {
			if(KviQString::equalCI(szName,item->name())) return (KviAliasTreeWidgetItem*)item;
		}
	}
	return false;
}

KviAliasTreeWidgetItem * KviAliasEditor::getAliasItem(const QString &szName)
{
	KviAliasTreeWidgetItem * it = findAliasItem(szName);
	if(it)return it;
	it = new KviAliasTreeWidgetItem(m_pTreeWidget,szName);
	return it;
}

void KviAliasEditor::splitFullAliasOrNamespaceName(const QString &szFullName,QStringList &lNamespaces,QString &szName)
{
	lNamespaces = szFullName.split("::");
	if(lNamespaces.count() < 1)
	{
		szName = "unnamed";
		lNamespaces.append(QString(""));
		return;
	}
	szName = lNamespaces.last();
	lNamespaces.removeLast();
}

QString KviAliasEditor::buildFullItemName(KviAliasEditorTreeWidgetItem * it)
{
	if(!it) return QString();
	QString szName = it->name();
	KviAliasNamespaceTreeWidgetItem * nit = it->parentNamespaceItem();
	while(nit)
	{
		QString tmp = nit->name();
		if(!tmp.isEmpty())
		{
			szName.prepend("::");
			szName.prepend(tmp);
		}
		nit = nit->parentNamespaceItem();
	}
	return szName;
}


KviAliasTreeWidgetItem * KviAliasEditor::createFullAliasItem(const QString &szFullName)
{
	QStringList lNamespaces;
	QString szName;
	splitFullAliasOrNamespaceName(szFullName,lNamespaces,szName);

	if(lNamespaces.isEmpty())
		return getAliasItem(szName);

	QStringList::Iterator it = lNamespaces.begin();

	KviAliasNamespaceTreeWidgetItem * nit = getNamespaceItem(*it);
	++it;
	while(it != lNamespaces.end())
	{
		nit = nit->getNamespaceItem(*it);
		++it;
	}

	return nit->getAliasItem(szName);
}

KviAliasNamespaceTreeWidgetItem * KviAliasEditor::createFullNamespaceItem(const QString &szFullName)
{
	QStringList lNamespaces;
	QString szName;
	splitFullAliasOrNamespaceName(szFullName,lNamespaces,szName);

	if(lNamespaces.isEmpty())
		return getNamespaceItem(szName);

	QStringList::Iterator it = lNamespaces.begin();

	KviAliasNamespaceTreeWidgetItem * nit = getNamespaceItem(*it);
	++it;
	while(it != lNamespaces.end())
	{
		nit = nit->getNamespaceItem(*it);
		++it;
	}

	return nit->getNamespaceItem(szName);
}

void KviAliasEditor::oneTimeSetup()
{
	KviPointerHashTable<QString,KviKvsScript> * a = KviKvsAliasManager::instance()->aliasDict();
	if(!a)return;

	KviPointerHashTableIterator<QString,KviKvsScript> it(*a);

	KviAliasTreeWidgetItem * item;
	while(it.current())
	{
		KviKvsScript * alias = it.current();
		item = createFullAliasItem(alias->name());
		item->setBuffer(alias->code());
		++it;
	}

	connect(m_pTreeWidget,SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)),this,SLOT(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)));
	connect(m_pTreeWidget,SIGNAL(rightButtonPressed(QTreeWidgetItem *,QPoint)),this,SLOT(itemPressed(QTreeWidgetItem *,QPoint)));
	connect(m_pTreeWidget,SIGNAL(itemChanged(QTreeWidgetItem *,int)),this,SLOT(itemRenamed(QTreeWidgetItem *,int)));
        connect(KviKvsAliasManager::instance(),SIGNAL(aliasRefresh(const QString &)),this,SLOT(aliasRefresh(const QString &)));
	m_pTreeWidget->sortItems(0,Qt::AscendingOrder);
}
void KviAliasEditor::aliasRefresh(const QString &szName)
{
    KviAliasTreeWidgetItem * item;
    KviKvsScript * alias = KviKvsAliasManager::instance()->aliasDict()->find(szName);
    item = createFullAliasItem(szName);
    if (item!=m_pLastEditedItem)
       item->setBuffer(alias->code());
    else{
       if(QMessageBox::warning(0,__tr2qs("OverWrite Curernt Alias"),
                        __tr2qs("You are about to overwrite current alias!"),
                        QMessageBox::Yes,QMessageBox::No|QMessageBox::Default|QMessageBox::Escape) != QMessageBox::Yes)return ;
        else {
                item->setBuffer(alias->code());
                m_pEditor->setText(alias->code());
        }
    }


}

void KviAliasEditor::itemRenamed(QTreeWidgetItem *it,int col)
{
	((KviAliasEditorTreeWidgetItem *)it)->setName(it->text(col));
	QString szNam = buildFullItemName((KviAliasEditorTreeWidgetItem *)it);
	QString szLabelText;
	if(((KviAliasEditorTreeWidgetItem *)it)->isNamespace())
		szLabelText = __tr2qs_ctx("Namespace","editor");
	else
		szLabelText = __tr2qs_ctx("Alias","editor");
	szLabelText += ": <b>";
	szLabelText += szNam;
	szLabelText += "</b>";
	m_pNameLabel->setText(szLabelText);
}

bool KviAliasEditor::hasSelectedItemsRecursive(QTreeWidgetItem *it)
{
	for (int i=0;i<it->childCount();i++)
	{
		if (!it->child(i)->isSelected()) hasSelectedItemsRecursive((KviAliasEditorTreeWidgetItem *)it->child(i));
		else return true;
	}
	return false;
}
bool KviAliasEditor::hasSelectedItems()
{
	for (int i=0;i<m_pTreeWidget->topLevelItemCount();i++)
	{
		if (hasSelectedItemsRecursive(m_pTreeWidget->topLevelItem(i))) return true;
	}
	return false;
}
bool KviAliasEditor::itemExists(QTreeWidgetItem *pSearchFor)
{
	if(!pSearchFor)return false;

	for (int i=0;i<m_pTreeWidget->topLevelItemCount();i++)
	{
		if(m_pTreeWidget->topLevelItem(i)==pSearchFor)
			return true;
		else {
			if(!((KviAliasEditorTreeWidgetItem *)m_pTreeWidget->topLevelItem(i))->isAlias())
			{
				if (itemExistsRecursive(pSearchFor,m_pTreeWidget->topLevelItem(i))) return true;
			}
		}
	}
	return false;
}
bool KviAliasEditor::itemExistsRecursive(QTreeWidgetItem *pSearchFor,QTreeWidgetItem * pSearchAt)
{
	for(int i=0;i<pSearchAt->childCount();i++)
	{
		if(pSearchFor == pSearchAt->child(i))return true;
		else
		{
			if (pSearchAt->child(i)->childCount())
			{
				if (itemExistsRecursive(pSearchFor,pSearchAt->child(i))) return true;
			}
		}
	}
	return false;
}

void KviAliasEditor::itemPressed(QTreeWidgetItem *it,QPoint pnt)
{
	m_pContextPopup->clear();

	m_pLastClickedItem = (KviAliasEditorTreeWidgetItem *)it;

	int id;

	m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIAS)),
			__tr2qs_ctx("Add Alias","editor"),
			this,SLOT(newAlias()));

	m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIAS)),
			__tr2qs_ctx("Add Namespace","editor"),
			this,SLOT(newNamespace()));

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
			__tr2qs_ctx("Find In Aliases...","editor"),
			this,SLOT(slotFind()));
	m_pContextPopup->setItemEnabled(id,bHasItems);

	m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NAMESPACE)),
			__tr2qs_ctx("Collapse All Namespaces","editor"),
			this,SLOT(slotCollapseNamespaces()));

	m_pContextPopup->setItemEnabled(id,bHasItems);
	m_pContextPopup->popup(pnt);
}

void KviAliasEditor::recursiveSearchReplace(const QString &szSearch,KviAliasEditorTreeWidgetItem * it,bool bReplace,const QString &szReplace)
{

	if(!it)return;
	for(int i=0;i< it->childCount();i++)
	{
		if(((KviAliasTreeWidgetItem *)it->child(i))->isAlias())
		{
			if(((KviAliasTreeWidgetItem *)it->child(i))->buffer().indexOf(szSearch,0,Qt::CaseInsensitive) != -1)
			{
				it->child(i)->setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIASHIGHLIGHTED))));
				if (bReplace) ((QString &)((KviAliasTreeWidgetItem *)it->child(i))->buffer()).replace(szSearch,szReplace,Qt::CaseInsensitive);
				it->setExpanded(true);
			}
			else
			it->child(i)->setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIAS))));
		}
		else {
			recursiveSearchReplace(szSearch,(KviAliasEditorTreeWidgetItem *)(it->child(i)),bReplace,szReplace);
		}
	}

}

void KviAliasEditor::slotFind()
{

	g_pAliasEditorModule->lock();
	bool bOk;

	QString szSearch = QInputDialog::getText(this,
		__tr2qs_ctx("Find In Aliases","editor"),
		__tr2qs_ctx("Please enter the text to be searched for. The matching aliases will be highlighted.","editor"),
		QLineEdit::Normal,
		"",
		&bOk);


	g_pAliasEditorModule->unlock();
	if(!bOk)return;
	if(szSearch.isEmpty())return;
	m_pEditor->setFindText(szSearch);
	for (int i=0;i<m_pTreeWidget->topLevelItemCount();i++)
	{
		if(((KviAliasTreeWidgetItem *)m_pTreeWidget->topLevelItem(i))->isAlias())
		{
			if(((KviAliasTreeWidgetItem *)m_pTreeWidget->topLevelItem(i))->buffer().indexOf(szSearch,0,Qt::CaseInsensitive) != -1)
			{
				m_pTreeWidget->topLevelItem(i)->setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIASHIGHLIGHTED))));
			} else {
				m_pTreeWidget->topLevelItem(i)->setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIAS))));
			}
		} else {
			recursiveSearchReplace(szSearch,(KviAliasEditorTreeWidgetItem *)m_pTreeWidget->topLevelItem(i));
		}
	}
}

void KviAliasEditor::slotFindWord(const QString &szSearch)
{
	m_pEditor->setFindText(szSearch);

//	recursiveSearchReplace(szSearch,(KviAliasEditorTreeWidgetItem *)m_pTreeWidget->firstChild());

}

void KviAliasEditor::recursiveCollapseNamespaces(KviAliasEditorTreeWidgetItem * it)
{
	if(!it)return;
	for (int i=0;i<it->childCount();i++)
	{
		if(it->child(i)->childCount())
		{
			it->child(i)->setExpanded(false);
			recursiveCollapseNamespaces((KviAliasEditorTreeWidgetItem *)it->child(i));
		}
	}
//	recursiveCollapseNamespaces((KviAliasEditorTreeWidgetItem *)(it->nextSibling()));
//	it->setExpanded(false);
}

void KviAliasEditor::slotCollapseNamespaces()
{
	for (int i=0;i<m_pTreeWidget->topLevelItemCount();i++)
	{
		if (m_pTreeWidget->topLevelItem(i)->childCount()){
			m_pTreeWidget->topLevelItem(i)->setExpanded(false);
			recursiveCollapseNamespaces((KviAliasEditorTreeWidgetItem *)m_pTreeWidget->topLevelItem(i));
		}
	}

	//recursiveCollapseNamespaces((KviAliasEditorTreeWidgetItem *)m_pTreeWidget->firstChild());
}

void KviAliasEditor::slotReplaceAll(const QString &szFind,const QString &szReplace)
{
	m_pEditor->setFindText(szReplace);

	for (int i=0;i<m_pTreeWidget->topLevelItemCount();i++)
	{
		recursiveSearchReplace(szFind,(KviAliasEditorTreeWidgetItem *)m_pTreeWidget->topLevelItem(i),true,szReplace);
	}
}

void KviAliasEditor::getExportAliasBuffer(QString &buffer,KviAliasTreeWidgetItem * it)
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

void KviAliasEditor::exportAll()
{
	exportAliases(false);
}

void KviAliasEditor::exportSelectedSepFiles()
{
	exportAliases (true,true);
}

void KviAliasEditor::exportSelected()
{
	exportAliases(true);
}

void KviAliasEditor::exportSelectionInSinglesFiles(KviPointerList<KviAliasTreeWidgetItem> *l)
{
	if(!m_szDir.endsWith(QString(KVI_PATH_SEPARATOR)))m_szDir += KVI_PATH_SEPARATOR;
	if (!l->first())
	{
		g_pAliasEditorModule->lock();
		QMessageBox::warning(this,__tr2qs_ctx("Alias Export","editor"),__tr2qs_ctx("There is no selection!","editor"),__tr2qs_ctx("Ok","editor"));
		g_pAliasEditorModule->unlock();
		return;
	}
	g_pAliasEditorModule->lock();

	if(!KviFileDialog::askForDirectoryName(m_szDir,__tr2qs_ctx("Choose a Directory - KVIrc","editor"),m_szDir)){
		g_pAliasEditorModule->unlock();
		return;
	}

	if(!m_szDir.endsWith(QString(KVI_PATH_SEPARATOR)))m_szDir += KVI_PATH_SEPARATOR;

	bool bReplaceAll=false;

	for(KviAliasTreeWidgetItem * it = l->first();it;it = l->next())
	{
		QString tmp;
		getExportAliasBuffer(tmp,it);
		QString szFileName=buildFullItemName(it);
		szFileName += ".kvs";
		szFileName.replace("::","_");
		QString szCompletePath=m_szDir+szFileName;

		if (KviFileUtils::fileExists(szCompletePath) && !bReplaceAll)
		{
				QString szMsg;
				KviQString::sprintf(szMsg,__tr2qs_ctx("The file \"%Q\" exists. Do you want to replace it ?","editor"),&szFileName);
				int ret = QMessageBox::question(this,__tr2qs_ctx("Replace file","editor"),szMsg,__tr2qs_ctx("Yes","editor"),__tr2qs_ctx("Yes to All","editor"),__tr2qs_ctx("No","editor"));
				if (ret!=2){
					KviFileUtils::writeFile(szCompletePath,tmp);
					if (ret==1)	bReplaceAll=true;
				}

		}
		else
		KviFileUtils::writeFile(szCompletePath,tmp);

	}
	g_pAliasEditorModule->unlock();

}

void KviAliasEditor::exportAliases(bool bSelectedOnly,bool bSingleFiles)
{
	saveLastEditedItem();

	KviPointerList<KviAliasTreeWidgetItem> l;
	l.setAutoDelete(false);

	QString out;

	appendAliasItems(&l,bSelectedOnly);
	if (bSingleFiles)
	{
			exportSelectionInSinglesFiles(&l);
			return;

	}
	int count=0;
	KviAliasTreeWidgetItem * tempitem=0;
	for(KviAliasTreeWidgetItem * it = l.first();it;it = l.next())
	{
		tempitem=it;
		count ++;
		QString tmp;
		getExportAliasBuffer(tmp,it);
		out += tmp;
		out += "\n";
	}
	QString szNameFile;
	if(out.isEmpty())
	{
		g_pAliasEditorModule->lock();
		QMessageBox::warning(this,__tr2qs_ctx("Alias Export","editor"),__tr2qs_ctx("The exported file would be empty: cowardly refusing to write it","editor"),__tr2qs_ctx("Ok","editor"));
		g_pAliasEditorModule->unlock();
		return;
	}

	QString szName = m_szDir;

	if(!szName.endsWith(QString(KVI_PATH_SEPARATOR)))szName += KVI_PATH_SEPARATOR;
	QString szFile;
	g_pAliasEditorModule->lock();

	if (count==1)
	{
			QString tmp=buildFullItemName(tempitem);
			szNameFile = tmp.replace("::","_");
	}
	else szNameFile="aliases";
	szName += szNameFile;
	szName += ".kvs";
	if(!KviFileDialog::askForSaveFileName(szFile,__tr2qs_ctx("Choose a Filename - KVIrc","editor"),szName,KVI_FILTER_SCRIPT,false,true,true))
	{
		g_pAliasEditorModule->unlock();
		return;
	}
	m_szDir=QFileInfo(szFile).absolutePath();
	g_pAliasEditorModule->unlock();

	if(!KviFileUtils::writeFile(szFile,out))
	{
		g_pAliasEditorModule->lock();
		QMessageBox::warning(this,__tr2qs_ctx("Write Failed - KVIrc","editor"),__tr2qs_ctx("Unable to write to the alias file.","editor"),__tr2qs_ctx("Ok","editor"));
		g_pAliasEditorModule->unlock();
	}
}

void KviAliasEditor::saveProperties(KviConfig *cfg)
{
	cfg->writeEntry("Sizes",m_pSplitter->sizes());
	QString szName;
	if(m_pLastEditedItem)
		szName = buildFullItemName(m_pLastEditedItem);
	cfg->writeEntry("LastAlias",szName);
}

void KviAliasEditor::loadProperties(KviConfig *cfg)
{
	QList<int> def;
	def.append(20);
	def.append(80);
	m_pSplitter->setSizes(cfg->readIntListEntry("Sizes",def));
	QString tmp = cfg->readEntry("LastAlias",QString());

	KviAliasEditorTreeWidgetItem * it = findAliasItem(tmp);
	if(!it)it = findNamespaceItem(tmp);
	if(it)activateItem(it);
}

void KviAliasEditor::appendAliasItems(KviPointerList<KviAliasTreeWidgetItem> * l,bool bSelectedOnly)
{
	for (int i=0;i<m_pTreeWidget->topLevelItemCount();i++)
	{
		if (((KviAliasTreeWidgetItem *)m_pTreeWidget->topLevelItem(i))->isAlias())
		{
			if(bSelectedOnly)
			{
				if(m_pTreeWidget->topLevelItem(i)->isSelected())
					l->append(((KviAliasTreeWidgetItem *)m_pTreeWidget->topLevelItem(i)));
			}
			else
			{
				l->append(((KviAliasTreeWidgetItem *)m_pTreeWidget->topLevelItem(i)));
			}
		}
		else
		{
			if(bSelectedOnly)
			{
				if(m_pTreeWidget->topLevelItem(i)->isSelected())
					appendAliasItemsRecursive(l,m_pTreeWidget->topLevelItem(i),false);
				else
					appendAliasItemsRecursive(l,m_pTreeWidget->topLevelItem(i),true);
			}
			else
				appendAliasItemsRecursive(l,m_pTreeWidget->topLevelItem(i),false);
		}
	}

}

void KviAliasEditor::appendAliasItemsRecursive(KviPointerList<KviAliasTreeWidgetItem> * l,QTreeWidgetItem * pStartFrom,bool bSelectedOnly)
{
	for (int i=0;i<pStartFrom->childCount();i++)
	{
		if (((KviAliasTreeWidgetItem *)pStartFrom->child(i))->isAlias())
		{
			if(bSelectedOnly)
			{
				if(pStartFrom->child(i)->isSelected())
					l->append((KviAliasTreeWidgetItem *)pStartFrom->child(i));
			}
			else
			{
				l->append((KviAliasTreeWidgetItem *)pStartFrom->child(i));
			}
		}
		else
		{
			if(bSelectedOnly)
			{
				if(pStartFrom->isSelected())
					appendAliasItemsRecursive(l,pStartFrom->child(i),false);
				else
					appendAliasItemsRecursive(l,pStartFrom->child(i),true);
			}
			else
				appendAliasItemsRecursive(l,pStartFrom->child(i),false);
		}
	}

}

void KviAliasEditor::appendNamespaceItems(KviPointerList<KviAliasTreeWidgetItem> * l,bool bSelectedOnly)
{
	for (int i=0;i<m_pTreeWidget->topLevelItemCount();i++)
	{
		if (((KviAliasTreeWidgetItem *)m_pTreeWidget->topLevelItem(i))->isNamespace())
		{
			if(bSelectedOnly)
			{
				if(m_pTreeWidget->topLevelItem(i)->isSelected())
					l->append(((KviAliasTreeWidgetItem *)m_pTreeWidget->topLevelItem(i)));
			}
			else
			{
				l->append(((KviAliasTreeWidgetItem *)m_pTreeWidget->topLevelItem(i)));
			}
		}
		else
		{
			if(bSelectedOnly)
			{
				if(m_pTreeWidget->topLevelItem(i)->isSelected())
					appendNamespaceItemsRecursive(l,m_pTreeWidget->topLevelItem(i),false);
				else
					appendNamespaceItemsRecursive(l,m_pTreeWidget->topLevelItem(i),true);
			}
			else
				appendNamespaceItemsRecursive(l,m_pTreeWidget->topLevelItem(i),false);
		}
	}

}

void KviAliasEditor::appendNamespaceItemsRecursive(KviPointerList<KviAliasTreeWidgetItem> * l,QTreeWidgetItem * pStartFrom,bool bSelectedOnly)
{
	for (int i=0;i<pStartFrom->childCount();i++)
	{
		if (((KviAliasTreeWidgetItem *)pStartFrom->child(i))->isNamespace())
		{
			if(bSelectedOnly)
			{
				if(pStartFrom->child(i)->isSelected())
					l->append((KviAliasTreeWidgetItem *)pStartFrom->child(i));
			}
			else
			{
				l->append((KviAliasTreeWidgetItem *)pStartFrom->child(i));
			}
		}
		else
		{
			if(bSelectedOnly)
			{
				if(pStartFrom->isSelected())
					appendNamespaceItemsRecursive(l,pStartFrom->child(i),false);
				else
					appendNamespaceItemsRecursive(l,pStartFrom->child(i),true);
			}
			else
				appendNamespaceItemsRecursive(l,pStartFrom->child(i),false);
		}
	}

}

void KviAliasEditor::appendSelectedItemsRecursive(KviPointerList<KviAliasEditorTreeWidgetItem> * l,QTreeWidgetItem *it)

{
	for (int i=0;i<it->childCount();i++)
	{
		if (it->child(i)->isSelected())
		{
			l->append((KviAliasEditorTreeWidgetItem *)it->child(i));
		}
		else
		{
			if (it->child(i)->childCount())
					appendSelectedItemsRecursive(l,it->child(i));
		}
	}
}

void KviAliasEditor::appendSelectedItems(KviPointerList<KviAliasEditorTreeWidgetItem> * l)
{
	for (int i=0;i<m_pTreeWidget->topLevelItemCount();i++)
	{
		if (m_pTreeWidget->topLevelItem(i)->isSelected())
		{
			l->append((KviAliasEditorTreeWidgetItem *)m_pTreeWidget->topLevelItem(i));
		}
		else
		{
			if (m_pTreeWidget->topLevelItem(i)->childCount())
					appendSelectedItemsRecursive(l,m_pTreeWidget->topLevelItem(i));
		}
	}
}

void KviAliasEditor::removeItemChildren(KviAliasEditorTreeWidgetItem *it)
{
	for (int i=0;i<it->childCount();i++)
	{
		if (it->child(i)->childCount()) removeItemChildren((KviAliasEditorTreeWidgetItem *)it->child(i));
		delete it->child(i);
	}
}

bool KviAliasEditor::removeItem(KviAliasEditorTreeWidgetItem *it,bool * pbYesToAll,bool)
{
	if(!it)return true;
	QString szMsg;
	QString szName = it->name();

	if(!*pbYesToAll)
	{
		if(it->isAlias())
			KviQString::sprintf(szMsg,__tr2qs_ctx("Do you really want to remove the alias \"%Q\" ?","editor"),&szName);
		else {
			KviQString::sprintf(szMsg,__tr2qs_ctx("Do you really want to remove the namespace \"%Q\" ?","editor"),&szName);
			szMsg += "<br>";
			szMsg += __tr2qs_ctx("Please note that all the children items will be deleted too.","editor");
		}

		g_pAliasEditorModule->lock();
		int ret = QMessageBox::question(this,__tr2qs_ctx("Remove item","editor"),szMsg,__tr2qs_ctx("Yes","editor"),__tr2qs_ctx("Yes to All","editor"),__tr2qs_ctx("No","editor"));
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
		m_pLastEditedItem = 0;
	if(it == m_pLastClickedItem)
		m_pLastClickedItem = 0;
	if (it->childCount()) removeItemChildren(it);
	delete it;
	return true;
		/*
	if(bDeleteEmptyTree)
	{
		while(it)
		{
			KviAliasEditorTreeWidgetItem * par = (KviAliasEditorTreeWidgetItem *)(it->parent());
			delete it;
			if(par)
			{
				//if(!par->firstChild())it = par;
				//else it = 0;
			} else {
				it = 0;
			}
		}
	} else {
		delete it;
	}
	return true;
	*/
}

void KviAliasEditor::removeSelectedItems()
{
	KviPointerList<KviAliasEditorTreeWidgetItem> l;
	l.setAutoDelete(false);
	appendSelectedItems(&l);
	bool bYesToAll = false;

	for(KviAliasEditorTreeWidgetItem *it = l.first();it;it = l.next())
	{
		if(!removeItem(it,&bYesToAll,false))return;
	}


}

QString KviAliasEditor::askForAliasName(const QString &szAction,const QString &szText,const QString &szInitialText)
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
		if(!bOk)return QString();
		if(szNewName.isEmpty())
		{
			g_pAliasEditorModule->lock();
			QMessageBox::warning(this,
				__tr2qs_ctx("Missing Alias Name","editor"),
				__tr2qs_ctx("You must specify a valid name for the alias","editor"),
				__tr2qs_ctx("Ok, Let me try again...","editor"));
			g_pAliasEditorModule->unlock();
			continue;
		}

		// we allow only [\w:]+
		QRegExp re("[\\w:]+");
		if(!re.exactMatch(szNewName))
		{
			g_pAliasEditorModule->lock();
			QMessageBox::information(this,
				__tr2qs_ctx("Bad Alias Name","editor"),
				__tr2qs_ctx("Alias names can contain only letters, digits, underscores and '::' namespace separators","editor"),
				__tr2qs_ctx("Ok, Let me try again...","editor"));
			g_pAliasEditorModule->unlock();
			szNewName = "";
			continue;
		}
		// make sure that we have only doubled "::" and not ":" or ":::..."
		QString tmp = szNewName;
		tmp.replace("::","@"); // @ is not allowed by the rule above
		if(tmp.indexOf(":",Qt::CaseInsensitive) != -1)
		{
			g_pAliasEditorModule->lock();
			QMessageBox::information(this,
				__tr2qs_ctx("Bad Alias Name","editor"),
				__tr2qs_ctx("Stray ':' character in alias name: did you mean ...<namespace>::<name> ?","editor"),
				__tr2qs_ctx("Ok, Let me try again...","editor"));
			g_pAliasEditorModule->unlock();
			szNewName = "";
			continue;
		}
		if(tmp.indexOf("@@",Qt::CaseInsensitive) != -1)
		{
			g_pAliasEditorModule->lock();
			QMessageBox::information(this,
				__tr2qs_ctx("Bad Alias Name","editor"),
				__tr2qs_ctx("Found an empty namespace in alias name","editor"),
				__tr2qs_ctx("Ok, Let me try again...","editor"));
			g_pAliasEditorModule->unlock();
			szNewName = "";
			continue;
		}
	}
	return szNewName;
}

QString KviAliasEditor::askForNamespaceName(const QString &szAction,const QString &szText,const QString &szInitialText)
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
		if(!bOk)return QString();
		if(szNewName.isEmpty())
		{
			g_pAliasEditorModule->lock();
			QMessageBox::warning(this,
				__tr2qs_ctx("Missing Namespace Name","editor"),
				__tr2qs_ctx("You must specify a valid name for the namespace","editor"),
				__tr2qs_ctx("Ok, Let me try again...","editor"));
			g_pAliasEditorModule->unlock();
			continue;
		}

		// we allow only [\w:]+
		QRegExp re("[\\w:]+");
		if(!re.exactMatch(szNewName))
		{
			g_pAliasEditorModule->lock();
			QMessageBox::information(this,
				__tr2qs_ctx("Bad Namespace Name","editor"),
				__tr2qs_ctx("Namespace names can contain only letters, digits, underscores and '::' namespace separators","editor"),
				__tr2qs_ctx("Ok, Let me try again...","editor"));
			g_pAliasEditorModule->unlock();
			szNewName = "";
			continue;
		}
		// make sure that we have only doubled "::" and not ":" or ":::..."
		QString tmp = szNewName;
		tmp.replace("::","@"); // @ is not allowed by the rule above
		if(tmp.indexOf(":",Qt::CaseInsensitive) != -1)
		{
			g_pAliasEditorModule->lock();
			QMessageBox::information(this,
				__tr2qs_ctx("Bad Namespace Name","editor"),
				__tr2qs_ctx("Stray ':' character in namespace name: did you mean ...<namespace>::<name> ?","editor"),
				__tr2qs_ctx("Ok, Let me try again...","editor"));
			g_pAliasEditorModule->unlock();
			szNewName = "";
			continue;
		}
		if(tmp.indexOf("@@",Qt::CaseInsensitive) != -1)
		{
			g_pAliasEditorModule->lock();
			QMessageBox::information(this,
				__tr2qs_ctx("Bad Namespace Name","editor"),
				__tr2qs_ctx("Found an empty namespace in namespace name","editor"),
				__tr2qs_ctx("Ok, Let me try again...","editor"));
			g_pAliasEditorModule->unlock();
			szNewName = "";
			continue;
		}
	}
	return szNewName;
}

void KviAliasEditor::openParentItems(QTreeWidgetItem * it)
{
	if(it->parent())
	{
		it->parent()->setExpanded(true);
		openParentItems(it->parent());
	}
}

void KviAliasEditor::activateItem(QTreeWidgetItem * it)
{
	openParentItems(it);
	m_pTreeWidget->setCurrentItem(it);
}

void KviAliasEditor::newAlias()
{
	if(m_pLastClickedItem)
	{
		if(!itemExists(m_pLastClickedItem))return; // dead ?
		if(m_pLastClickedItem->isAlias())
			m_pLastClickedItem = m_pLastClickedItem->parentNamespaceItem();
	}

	QString szNewName = askForAliasName(__tr2qs_ctx("Add Alias","editor"),__tr2qs_ctx("Please enter the name for the new alias","editor"),"myfunction");
	if(szNewName.isEmpty())return;

	getUniqueItemName(m_pLastClickedItem,szNewName,KviAliasEditorTreeWidgetItem::Alias);

	KviAliasTreeWidgetItem * it;

	if(m_pLastClickedItem)
		it = ((KviAliasNamespaceTreeWidgetItem *)m_pLastClickedItem)->createFullAliasItem(szNewName);
	else
		it = createFullAliasItem(szNewName);

	activateItem(it);
}

void KviAliasEditor::newNamespace()
{
	if(m_pLastClickedItem)
	{
		if(!itemExists(m_pLastClickedItem))return; // dead ?
		if(m_pLastClickedItem->isAlias())
			m_pLastClickedItem = m_pLastClickedItem->parentNamespaceItem();
	}


	QString szNewName = askForNamespaceName(__tr2qs_ctx("Add Namespace","editor"),__tr2qs_ctx("Please enter the name for the new namespace","editor"),"mynamespace");
	if(szNewName.isEmpty())return;

	getUniqueItemName(m_pLastClickedItem,szNewName,KviAliasEditorTreeWidgetItem::Namespace);

	KviAliasNamespaceTreeWidgetItem * it;

	if(m_pLastClickedItem)
		it = ((KviAliasNamespaceTreeWidgetItem *)m_pLastClickedItem)->createFullNamespaceItem(szNewName);
	else
		it = createFullNamespaceItem(szNewName);

	activateItem(it);
}

bool KviAliasEditor::aliasExists(QString &szFullItemName)
{
	KviPointerList<KviAliasTreeWidgetItem> l;
	l.setAutoDelete(false);

	appendAliasItems(&l,false);
	for(KviAliasTreeWidgetItem * it = l.first();it;it = l.next())
	{
		if (KviQString::equalCI(buildFullItemName(it),szFullItemName))
		{
			return true;
		}
	}
	return false;
}

bool KviAliasEditor::namespaceExists(QString &szFullItemName)
{
	KviPointerList<KviAliasTreeWidgetItem> l;
	l.setAutoDelete(false);

	appendNamespaceItems(&l,false);
	for(KviAliasTreeWidgetItem * it = l.first();it;it = l.next())
	{
		if (KviQString::equalCI(buildFullItemName(it),szFullItemName))
		{
			return true;
		}
	}
	return false;
}
/*
void KviAliasEditor::appendItems(KviPointerList<KviAliasEditorTreeWidgetItem> * l,QTreeWidgetItem *it)
{
	for (int i=0;i<it->childCount();i++)
	{
		l->append((KviAliasEditorTreeWidgetItem *) it->child(i);
		if(it->child(i)->childCount()) appendItemd(it->child(i));
	}
}
*/
void KviAliasEditor::renameItem()
{
	if(!m_pLastEditedItem)return;
	if(!itemExists(m_pLastEditedItem))return; // dead ?

	QString szName = buildFullItemName(m_pLastEditedItem);
	QString szNewName;

	bool bAlias = m_pLastEditedItem->isAlias();

	if(bAlias)
		szNewName = askForAliasName(__tr2qs_ctx("Rename Alias","editor"),__tr2qs_ctx("Please enter the new name for the alias","editor"),szName);
	else
		szNewName = askForNamespaceName(__tr2qs_ctx("Rename Namespace","editor"),__tr2qs_ctx("Please enter the new name for the namespace","editor"),szName);
	if(szNewName.isEmpty())return;

	if(szName == szNewName)return;

	// check if there is already an alias with this name
	if (bAlias)
	{
		if (aliasExists(szNewName))
		{
			g_pAliasEditorModule->lock();
			QMessageBox::information(this,
				__tr2qs_ctx("Alias already exists","editor"),
				__tr2qs_ctx("This name is already in use. Please choose another one.","editor"),
				__tr2qs_ctx("Ok, Let me try again...","editor"));
			g_pAliasEditorModule->unlock();
			return;
		}
	} else {
	// check if there is already a namespace with this name
		if (namespaceExists(szNewName))
		{
			g_pAliasEditorModule->lock();
			QMessageBox::information(this,
				__tr2qs_ctx("Namespace already exists","editor"),
				__tr2qs_ctx("This name is already in use. Please choose another one.","editor"),
				__tr2qs_ctx("Ok, Let me try again...","editor"));
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
	QList<QTreeWidgetItem*> lChildren= m_pLastEditedItem->takeChildren();
	bool bYesToAll = true;
	removeItem(m_pLastEditedItem,&bYesToAll,true);

	m_pLastEditedItem = 0; // make sure it's true (but it already should be because of removeItem())
	m_pLastClickedItem = 0; // make sure it's true (but it already should be because of removeItem())

	if(bAlias)
	{
		KviAliasTreeWidgetItem * ait = createFullAliasItem(szNewName);
		ait->setBuffer(szCode);
		ait->setCursorPosition(pntCursor);
		activateItem(ait);

	} else {
		KviAliasNamespaceTreeWidgetItem * nit = createFullNamespaceItem(szNewName);
		activateItem(nit);

		for(int i=0;i<lChildren.count();i++)
		{
			((KviAliasEditorTreeWidgetItem*)lChildren.at(i))->setParentNamespaceItem(nit);
			nit->insertChild(nit->childCount(),lChildren.at(i));
		}

	}
}

// warning: this saves ONLY the body, not the name!
//          the name is changed explicitly with renameItem(), when needed
void KviAliasEditor::saveLastEditedItem()
{
	if(!m_pLastEditedItem)
		return;
	if(!itemExists(m_pLastEditedItem) || !m_pEditor->isModified() || m_pLastEditedItem->isNamespace())
		return;

	((KviAliasTreeWidgetItem *)m_pLastEditedItem)->setCursorPosition(m_pEditor->getCursor());
	QString newCode;
	m_pEditor->getText(newCode);
	((KviAliasTreeWidgetItem *)m_pLastEditedItem)->setBuffer(newCode);
}

void KviAliasEditor::currentItemChanged(QTreeWidgetItem *it,QTreeWidgetItem *)
{

	saveLastEditedItem();
	m_pLastEditedItem = (KviAliasEditorTreeWidgetItem *)it;

	if(!m_pLastEditedItem)
	{
		m_pNameLabel->setText(__tr2qs_ctx("No item selected","editor"));
		m_pRenameButton->setEnabled(false);
		m_pEditor->setText("");
		m_pEditor->setEnabled(false);
		return;
	}

	QString szNam = buildFullItemName(m_pLastEditedItem);
	if(m_pLastEditedItem->isNamespace())
	{
		QString szLabelText = __tr2qs_ctx("Namespace","editor");
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

	QString szLabelText = __tr2qs_ctx("Alias","editor");
	szLabelText += ": <b>";
	szLabelText += szNam;
	szLabelText += "</b>";
	m_pNameLabel->setText(szLabelText);
	m_pRenameButton->setEnabled(true);
	m_pEditor->setText(((KviAliasTreeWidgetItem *)it)->buffer());

	m_pEditor->setFocus();
	m_pEditor->setCursorPosition(((KviAliasTreeWidgetItem *)it)->cursorPosition());
	m_pEditor->setEnabled(true);

}

void KviAliasEditor::recursiveCommit(KviAliasEditorTreeWidgetItem * it)
{
	if(!it)return;
	if(it->isAlias())
	{
		QString szName = buildFullItemName(it);
//		debug("Commit alias %s",szName.toUtf8().data());
		//debug("ADDING %s",szName.latin1());
		// WARNING: On MSVC operator new here is valid ONLY because
		//          KviKvsScript has a non virtual detructor!
		KviKvsScript * a = new KviKvsScript(szName,((KviAliasTreeWidgetItem *)it)->buffer());
		KviKvsAliasManager::instance()->add(szName,a);
		return;
	} else
	{
		for (int i=0;i<it->childCount();i++)
		{
			recursiveCommit((KviAliasEditorTreeWidgetItem *) it->child(i));
		}
	}
}

void KviAliasEditor::commit()
{
	saveLastEditedItem();
// 	KviKvsAliasManager::instance()->clear();
	for(int i=0;i<m_pTreeWidget->topLevelItemCount();i++)
	{
		KviAliasEditorTreeWidgetItem *item=(KviAliasEditorTreeWidgetItem *)m_pTreeWidget->topLevelItem(i);
		recursiveCommit(item);
	}
	g_pApp->saveAliases();
}

void KviAliasEditor::getUniqueItemName(KviAliasEditorTreeWidgetItem *item,QString &buffer,KviAliasEditorTreeWidgetItem::Type eType)
{
	QString ret;

	int idx = 0;
	KviAliasEditorTreeWidgetItem * found;

	for(;;)
	{
		ret = buffer;
		if(idx > 0)
		{
			QString tmp;
			tmp.setNum(idx);
			ret.append(tmp);
		}

		if(item)
		{
			if(!item->isNamespace())
			{
				buffer = ret;
				return;
			}

			if(eType == KviAliasEditorTreeWidgetItem::Namespace)
				found = ((KviAliasNamespaceTreeWidgetItem *)item)->findNamespaceItem(ret);
			else
				found = ((KviAliasNamespaceTreeWidgetItem *)item)->findAliasItem(ret);
		} else {
			if(eType == KviAliasEditorTreeWidgetItem::Namespace)
				found = findNamespaceItem(ret);
			else
				found = findAliasItem(ret);
		}

		if(!found)
		{
			buffer = ret;
			return;
		}

		idx++;
	}
}


KviAliasEditorWindow::KviAliasEditorWindow(KviFrame * lpFrm)
: KviWindow(KVI_WINDOW_TYPE_SCRIPTEDITOR,lpFrm,"aliaseditor",0)
{
	g_pAliasEditorWindow = this;

	setFixedCaption(__tr2qs_ctx("Alias Editor","editor"));

	QGridLayout * g = new QGridLayout();

	m_pEditor = new KviAliasEditor(this);
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

KviAliasEditorWindow::~KviAliasEditorWindow()
{
	g_pAliasEditorWindow = 0;
}

void KviAliasEditorWindow::okClicked()
{
	m_pEditor->commit();
	close();
}

void KviAliasEditorWindow::applyClicked()
{
	m_pEditor->commit();
}

void KviAliasEditorWindow::cancelClicked()
{
	close();
}

QPixmap * KviAliasEditorWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIAS);
}

void KviAliasEditorWindow::getConfigGroupName(QString &szName)
{
	szName = "aliaseditor";
}

void KviAliasEditorWindow::saveProperties(KviConfig *cfg)
{
	m_pEditor->saveProperties(cfg);
}

void KviAliasEditorWindow::loadProperties(KviConfig *cfg)
{
	m_pEditor->loadProperties(cfg);
}
