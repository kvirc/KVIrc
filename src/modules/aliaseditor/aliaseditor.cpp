//=====================================================================================
//
//   File : aliaseditor.cpp
//   Creation date : Mon Dec 23 2002 14:36:18 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2002-2005 Szymon Stefanek (pragma at kvirc dot net)
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
//=====================================================================================

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
#include "kvi_qstring.h"
#include "kvi_kvs_aliasmanager.h"
#include "kvi_filedialog.h"
#include "kvi_cmdformatter.h"
#include "kvi_module.h"
#include "kvi_valuelist.h"
#include "kvi_tal_vbox.h"

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

extern KviAliasEditorWindow * g_pAliasEditorWindow;
extern KviModule * g_pAliasEditorModule;

KviAliasEditorListViewItem::KviAliasEditorListViewItem(QTreeWidget * pListView,Type eType,const QString &szName)
: QTreeWidgetItem(pListView), m_eType(eType), m_pParentNamespaceItem(0)
{
	setName(szName);
	setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled|Qt::ItemIsSelectable);
}

KviAliasEditorListViewItem::KviAliasEditorListViewItem(KviAliasNamespaceListViewItem * pParentNamespaceItem,Type eType,const QString &szName)
: QTreeWidgetItem(pParentNamespaceItem), m_eType(eType), m_pParentNamespaceItem(pParentNamespaceItem)
{
	setName(szName);
	setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled|Qt::ItemIsSelectable);
}



QString KviAliasEditorListViewItem::key(int column,bool bAsc) const
{
	QString ret;
	if(m_eType == Namespace)ret = "1";
	else ret = "2";
	ret += m_szName;
	return ret;
}

void KviAliasEditorListViewItem::setName(const QString &szName)
{
	m_szName = szName;
	setText(0,m_szName);
}

KviAliasListViewItem::KviAliasListViewItem(KviAliasNamespaceListViewItem * pParentNamespaceItem,const QString &szName)
: KviAliasEditorListViewItem(pParentNamespaceItem,KviAliasEditorListViewItem::Alias,szName)
{
	setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIAS))));
	m_cPos=QPoint(0,0);
	
}


KviAliasListViewItem::KviAliasListViewItem(QTreeWidget * pListView,const QString &szName)
: KviAliasEditorListViewItem(pListView,KviAliasEditorListViewItem::Alias,szName)
{
	setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIAS))));
	m_cPos=QPoint(0,0);

}

KviAliasListViewItem::~KviAliasListViewItem()
{
}

KviAliasNamespaceListViewItem::KviAliasNamespaceListViewItem(QTreeWidget * pListView,const QString &szName)
: KviAliasEditorListViewItem(pListView,KviAliasEditorListViewItem::Namespace,szName)
{
	setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NAMESPACE))));
}

KviAliasNamespaceListViewItem::KviAliasNamespaceListViewItem(KviAliasNamespaceListViewItem * pParentNamespaceItem,const QString &szName)
: KviAliasEditorListViewItem(pParentNamespaceItem,KviAliasEditorListViewItem::Namespace,szName)
{
	setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NAMESPACE))));
}

KviAliasNamespaceListViewItem::~KviAliasNamespaceListViewItem()
{

}

KviAliasNamespaceListViewItem * KviAliasNamespaceListViewItem::findNamespaceItem(const QString &szName)
{
	for (int i=0;i<childCount();i++)
	{
		if (((KviAliasEditorListViewItem *)child(i))->isNamespace())
			if(KviQString::equalCI(szName,((KviAliasEditorListViewItem *)child(i))->name()))return (KviAliasNamespaceListViewItem *)child(i);
	}
	return 0;
}

KviAliasNamespaceListViewItem * KviAliasNamespaceListViewItem::getNamespaceItem(const QString &szName)
{
	KviAliasNamespaceListViewItem * it = findNamespaceItem(szName);
	if(it)return it;
	it = new KviAliasNamespaceListViewItem(this,szName);
	return it;
}

KviAliasListViewItem * KviAliasNamespaceListViewItem::findAliasItem(const QString &szName)
{
	for (int i=0;i<childCount();i++)
	{
		if (((KviAliasEditorListViewItem *)child(i))->isAlias())
			if(KviQString::equalCI(szName,((KviAliasEditorListViewItem *)child(i))->name()))return (KviAliasListViewItem *)child(i);
	}
	return 0;
}

KviAliasListViewItem * KviAliasNamespaceListViewItem::getAliasItem(const QString &szName)
{
	KviAliasListViewItem * it = findAliasItem(szName);
	if(it)return it;
	it = new KviAliasListViewItem(this,szName);
	return it;
}


KviAliasListViewItem * KviAliasNamespaceListViewItem::createFullAliasItem(const QString &szFullName)
{
	QStringList lNamespaces;
	QString szName;
	KviAliasEditor::splitFullAliasOrNamespaceName(szFullName,lNamespaces,szName);

	if(lNamespaces.isEmpty())
		return getAliasItem(szName);
	
	QStringList::Iterator it = lNamespaces.begin();
	
	KviAliasNamespaceListViewItem * nit = getNamespaceItem(*it);
	++it;
	while(it != lNamespaces.end())
	{
		nit = nit->getNamespaceItem(*it);
		++it;
	}
	
	return nit->getAliasItem(szName);
}

KviAliasNamespaceListViewItem * KviAliasNamespaceListViewItem::createFullNamespaceItem(const QString &szFullName)
{
	QStringList lNamespaces;
	QString szName;
	KviAliasEditor::splitFullAliasOrNamespaceName(szFullName,lNamespaces,szName);

	if(lNamespaces.isEmpty())
		return getNamespaceItem(szName);
	
	QStringList::Iterator it = lNamespaces.begin();
	
	KviAliasNamespaceListViewItem * nit = getNamespaceItem(*it);
	++it;
	while(it != lNamespaces.end())
	{
		nit = nit->getNamespaceItem(*it);
		++it;
	}
	
	return nit->getNamespaceItem(szName);
}
KviAliasEditorListView::KviAliasEditorListView(QWidget * par)
: QTreeWidget(par)
{
	setColumnCount (1);
	setHeaderLabel(__tr2qs("Alias"));
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setSortingEnabled(true);
	setRootIsDecorated(true);
	setAnimated(true);
}
void KviAliasEditorListView::mousePressEvent (QMouseEvent *e)
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
	m_szDir = QDir::homeDirPath();

	QGridLayout * l = new QGridLayout(this,1,1,2,2);

	m_pSplitter = new QSplitter(Qt::Horizontal,this);
	m_pSplitter->setOpaqueResize(false);
	l->addWidget(m_pSplitter,0,0);
	
	KviTalVBox * box = new KviTalVBox(m_pSplitter);
	m_pListView = new KviAliasEditorListView(box);
	

	box = new KviTalVBox(m_pSplitter);
	KviTalHBox * hbox = new KviTalHBox(box);
	m_pNameLabel = new QLabel(__tr2qs("No item selected"),hbox);
	m_pRenameButton = new QPushButton(__tr2qs("Rename"),hbox);
	m_pRenameButton->setEnabled(false);
	connect(m_pRenameButton,SIGNAL(clicked()),this,SLOT(renameItem()));
	hbox->setStretchFactor(m_pNameLabel,2);
	m_pRenameButton->setToolTip(__tr2qs("Edit the alias or namespace name"));
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

KviAliasNamespaceListViewItem * KviAliasEditor::findNamespaceItem(const QString &szName)
{

	int count=m_pListView->topLevelItemCount();
	for (int i=0;i<count;i++)
	{
		KviAliasNamespaceListViewItem * item=(KviAliasNamespaceListViewItem *)m_pListView->topLevelItem(i);
		if(KviQString::equalCI(szName,item->name()))
			return item;
	}
	return 0;
}

KviAliasNamespaceListViewItem * KviAliasEditor::getNamespaceItem(const QString &szName)
{
	KviAliasNamespaceListViewItem * it = findNamespaceItem(szName);
	if(it)return it;
	it = new KviAliasNamespaceListViewItem(m_pListView,szName);
	return it;
}
KviAliasListViewItem * KviAliasEditor::findAliasItemRecursive(KviAliasEditorListViewItem *parent,const QString &szName)
{
	for (int i=0;i<parent->childCount();i++)
	{
		KviAliasEditorListViewItem *item=(KviAliasEditorListViewItem *)parent->child(i);
		
		if (!item->isAlias())
				return findAliasItemRecursive(item,szName);
		else{
			if(KviQString::equalCI(szName,item->name())) 
				return (KviAliasListViewItem*)item;
		}
	}
	return false;
}

KviAliasListViewItem * KviAliasEditor::findAliasItem(const QString &szName)
{
	for (int i=0;i<m_pListView->topLevelItemCount();i++)
	{
		KviAliasEditorListViewItem *item=(KviAliasEditorListViewItem *)m_pListView->topLevelItem(i);
		if (!item->isAlias())
		{
			KviAliasListViewItem * it=findAliasItemRecursive(item,szName);
			if (it) return it;
		}
		else {
			if(KviQString::equalCI(szName,item->name())) return (KviAliasListViewItem*)item;
		}
	}
	return false;
}

KviAliasListViewItem * KviAliasEditor::getAliasItem(const QString &szName)
{
	KviAliasListViewItem * it = findAliasItem(szName);
	if(it)return it;
	it = new KviAliasListViewItem(m_pListView,szName);
	return it;
}

void KviAliasEditor::splitFullAliasOrNamespaceName(const QString &szFullName,QStringList &lNamespaces,QString &szName)
{
	lNamespaces = QStringList::split("::",szFullName);
	if(lNamespaces.count() < 1)
	{
		szName = "unnamed";
		lNamespaces.append(QString(""));
		return;
	}
	szName = *(lNamespaces.fromLast());
	lNamespaces.remove(lNamespaces.fromLast());
}

QString KviAliasEditor::buildFullItemName(KviAliasEditorListViewItem * it)
{
	if(!it) return QString::null;
	QString szName = it->name();
	KviAliasNamespaceListViewItem * nit = it->parentNamespaceItem();
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


KviAliasListViewItem * KviAliasEditor::createFullAliasItem(const QString &szFullName)
{
	QStringList lNamespaces;
	QString szName;
	splitFullAliasOrNamespaceName(szFullName,lNamespaces,szName);

	if(lNamespaces.isEmpty())
		return getAliasItem(szName);
	
	QStringList::Iterator it = lNamespaces.begin();
	
	KviAliasNamespaceListViewItem * nit = getNamespaceItem(*it);
	++it;
	while(it != lNamespaces.end())
	{
		nit = nit->getNamespaceItem(*it);
		++it;
	}
	
	return nit->getAliasItem(szName);
}

KviAliasNamespaceListViewItem * KviAliasEditor::createFullNamespaceItem(const QString &szFullName)
{
	QStringList lNamespaces;
	QString szName;
	splitFullAliasOrNamespaceName(szFullName,lNamespaces,szName);

	if(lNamespaces.isEmpty())
		return getNamespaceItem(szName);
	
	QStringList::Iterator it = lNamespaces.begin();
	
	KviAliasNamespaceListViewItem * nit = getNamespaceItem(*it);
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

	KviAliasListViewItem * item;
	while(it.current())
	{
		KviKvsScript * alias = it.current();
		item = createFullAliasItem(alias->name());
		item->setBuffer(alias->code());
		++it;
	}
		
	connect(m_pListView,SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)),this,SLOT(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)));
	connect(m_pListView,SIGNAL(rightButtonPressed(QTreeWidgetItem *,QPoint)),
		this,SLOT(itemPressed(QTreeWidgetItem *,QPoint)));
	connect(m_pListView,SIGNAL(itemChanged(QTreeWidgetItem *,int)),this,SLOT(itemRenamed(QTreeWidgetItem *,int)));
	
	
}


void KviAliasEditor::itemRenamed(QTreeWidgetItem *it,int col)
{
	debug("Renamed item with new text %s",it->text(col).toUtf8().data());
	((KviAliasEditorListViewItem *)it)->setName(it->text(col));
	QString szNam = buildFullItemName((KviAliasEditorListViewItem *)it);
	QString szLabelText;
	if(((KviAliasEditorListViewItem *)it)->isNamespace())
		szLabelText = __tr2qs("Namespace");
	else
		szLabelText = __tr2qs("Alias");
	szLabelText += ": <b>";
	szLabelText += szNam;
	szLabelText += "</b>";
	m_pNameLabel->setText(szLabelText);
}
bool KviAliasEditor::hasSelectedItemsRecursive(QTreeWidgetItem *it)
{
	for (int i=0;i<it->childCount();i++)
	{
		if (!it->child(i)->isSelected()) recursiveCommit((KviAliasEditorListViewItem *)it->child(i));
		else return true;
	}
	return false;
}
bool KviAliasEditor::hasSelectedItems()
{
	for (int i=0;i<m_pListView->topLevelItemCount();i++)
	{
		if (hasSelectedItemsRecursive(m_pListView->topLevelItem(i))) return true;
	}
	return false;
}
bool KviAliasEditor::itemExists(QTreeWidgetItem *pSearchFor)
{
	if(!pSearchFor)return false;
	for (int i=0;i<m_pListView->topLevelItemCount();i++)
	{
		
		if(m_pListView->topLevelItem(i)==pSearchFor) return true;
		else
		{
			if(!((KviAliasEditorListViewItem *)m_pListView->topLevelItem(i))->isAlias())
			{
				if (itemExistsRecursive(pSearchFor,m_pListView->topLevelItem(i))) return true;
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
			if(!((KviAliasEditorListViewItem *)pSearchAt->child(i))->isAlias())
				itemExistsRecursive(pSearchFor,pSearchAt->child(i));
		}
	}
	return false;
}

void KviAliasEditor::itemPressed(QTreeWidgetItem *it,QPoint pnt)
{
	m_pContextPopup->clear();
	
	m_pLastClickedItem = (KviAliasEditorListViewItem *)it;
	
	int id;

	m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIAS)),
			__tr2qs("Add Alias"),
			this,SLOT(newAlias()));

	m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIAS)),
			__tr2qs("Add Namespace"),
			this,SLOT(newNamespace()));
	
	//bool bHasItems = m_pListView->firstChild() != 0;
	bool bHasSelected = hasSelectedItems();

	m_pContextPopup->insertSeparator();

	id = m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_QUIT)),
			__tr2qs("Remove Selected"),
			this,SLOT(removeSelectedItems()));
//	m_pContextPopup->setItemEnabled(id,bHasSelected);

	m_pContextPopup->insertSeparator();

	m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_FOLDER)),
			__tr2qs("Export Selected..."),
			this,SLOT(exportSelected()));
	//m_pContextPopup->setItemEnabled(id,bHasSelected);

	m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_FOLDER)),
			__tr2qs("Export Selected in singles files..."),
			this,SLOT(exportSelectedSepFiles()));

//	m_pContextPopup->setItemEnabled(id,bHasSelected);

	m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_FOLDER)),
			__tr2qs("Export All..."),
			this,SLOT(exportAll()));
//	m_pContextPopup->setItemEnabled(id,bHasItems);

	m_pContextPopup->insertSeparator();

	m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SEARCH)),
			__tr2qs("Find In Aliases..."),
			this,SLOT(slotFind()));
//	m_pContextPopup->setItemEnabled(id,bHasItems);

	m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NAMESPACE)),
			__tr2qs("Collapse All Namespaces"),
			this,SLOT(slotCollapseNamespaces()));

//	m_pContextPopup->setItemEnabled(id,bHasItems);
	m_pContextPopup->popup(pnt);
}

void KviAliasEditor::recursiveSearchReplace(const QString &szSearch,KviAliasEditorListViewItem * it,bool bReplace,const QString &szReplace)
{
	/*
	if(!it)return;
	if(it->isAlias())
	{
		if(((KviAliasListViewItem *)it)->buffer().find(szSearch,0,false) != -1)
		{
			it->setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIASHIGHLIGHTED))));
			if (bReplace) ((QString &)((KviAliasListViewItem *)it)->buffer()).replace(szSearch,szReplace,false);
			openParentItems(it);
		} else
			it->setIcon(0,QIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIAS))));
	} else {
		recursiveSearchReplace(szSearch,(KviAliasEditorListViewItem *)(it->firstChild()),bReplace,szReplace);
	}
	recursiveSearchReplace(szSearch,(KviAliasEditorListViewItem *)(it->nextSibling()),bReplace,szReplace);
	*/
}

void KviAliasEditor::slotFind()
{
	/*
	g_pAliasEditorModule->lock();
	bool bOk;
	QString szSearch = QInputDialog::getText(
					__tr2qs("Find In Aliases"),
					__tr2qs("Please enter the text to be searched for. The matching aliases will be highlighted."),
					QLineEdit::Normal,
					"",
					&bOk,
					this);
	g_pAliasEditorModule->unlock();
	if(!bOk)return;
	if(szSearch.isEmpty())return;
	m_pEditor->setFindText(szSearch);
	
	recursiveSearchReplace(szSearch,(KviAliasEditorListViewItem *)m_pListView->firstChild());
	*/
}

void KviAliasEditor::slotFindWord(const QString &szSearch)
{
	m_pEditor->setFindText(szSearch);
	
//	recursiveSearchReplace(szSearch,(KviAliasEditorListViewItem *)m_pListView->firstChild());

}

void KviAliasEditor::recursiveCollapseNamespaces(KviAliasEditorListViewItem * it)
{
	if(!it)return;
	if(it->isNamespace())
	{
	//	recursiveCollapseNamespaces((KviAliasEditorListViewItem *)(it->firstChild()));
	}
//	recursiveCollapseNamespaces((KviAliasEditorListViewItem *)(it->nextSibling()));
	it->setExpanded(false);
}

void KviAliasEditor::slotCollapseNamespaces()
{
//	recursiveCollapseNamespaces((KviAliasEditorListViewItem *)m_pListView->firstChild());
}

void KviAliasEditor::slotReplaceAll(const QString &before,const QString &after)
{
	//recursiveSearchReplace(before,(KviAliasEditorListViewItem *)m_pListView->firstChild(),true,after);
}

void KviAliasEditor::getExportAliasBuffer(QString &buffer,KviAliasListViewItem * it)
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

void KviAliasEditor::exportSelectionInSinglesFiles(KviPointerList<KviAliasListViewItem> *l)
{
	if(!m_szDir.endsWith(QString(KVI_PATH_SEPARATOR)))m_szDir += KVI_PATH_SEPARATOR;
	debug ("dir %s",m_szDir.latin1());
	if (!l->first())
	{
		g_pAliasEditorModule->lock();
		QMessageBox::warning(this,__tr2qs("Alias Export"),__tr2qs("There is not selection!"),__tr2qs("Ok"));
		g_pAliasEditorModule->unlock();
		return;
	}
	g_pAliasEditorModule->lock();

	if(!KviFileDialog::askForDirectoryName(m_szDir,__tr2qs("Choose a Directory - KVIrc"),m_szDir)){
		g_pAliasEditorModule->unlock();
		return;
	}
	
	if(!m_szDir.endsWith(QString(KVI_PATH_SEPARATOR)))m_szDir += KVI_PATH_SEPARATOR;
	debug ("dir changed in %s",m_szDir.latin1());
	
	bool bReplaceAll=false;
	
	for(KviAliasListViewItem * it = l->first();it;it = l->next())
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
				KviQString::sprintf(szMsg,__tr2qs("The file \"%Q\" exists. Do you want to replace it ?"),&szFileName);
				int ret = QMessageBox::question(this,__tr2qs("Replace file"),szMsg,__tr2qs("Yes"),__tr2qs("Yes to All"),__tr2qs("No"));
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
	
	KviPointerList<KviAliasListViewItem> l;
	l.setAutoDelete(false);

	QString out;

	appendAliasItems(&l,bSelectedOnly);
	if (bSingleFiles)
	{
			exportSelectionInSinglesFiles(&l);
			return;

	}
	int count=0;
	KviAliasListViewItem * tempitem=0;
	for(KviAliasListViewItem * it = l.first();it;it = l.next())
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
		QMessageBox::warning(this,__tr2qs("Alias Export"),__tr2qs("There exported file would be empty: cowardly refusing to write it"),__tr2qs("Ok"));
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
	if(!KviFileDialog::askForSaveFileName(szFile,__tr2qs("Choose a Filename - KVIrc"),szName,"*.kvs",false,true,true))
	{
		g_pAliasEditorModule->unlock();		
		return;
	}
	m_szDir=QFileInfo(szFile).dirPath(TRUE);
	g_pAliasEditorModule->unlock();

	if(!KviFileUtils::writeFile(szFile,out))
	{
		g_pAliasEditorModule->lock();
		QMessageBox::warning(this,__tr2qs("Write Failed - KVIrc"),__tr2qs("Unable to write to the alias file."),__tr2qs("Ok"));
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
	KviValueList<int> def;
	def.append(20);
	def.append(80);
	m_pSplitter->setSizes(cfg->readIntListEntry("Sizes",def));
	QString tmp = cfg->readEntry("LastAlias",QString::null);

	KviAliasEditorListViewItem * it = findAliasItem(tmp);
	if(!it)it = findNamespaceItem(tmp);
	if(it)activateItem(it);
}
void KviAliasEditor::appendAliasItems(KviPointerList<KviAliasListViewItem> * l,bool bSelectedOnly)
{
	for (int i=0;i<m_pListView->topLevelItemCount();i++)
	{
		if (((KviAliasListViewItem *)m_pListView->topLevelItem(i))->isAlias())
		{
			if(bSelectedOnly)
			{
				if(m_pListView->topLevelItem(i)->isSelected())
					l->append(((KviAliasListViewItem *)m_pListView->topLevelItem(i)));
			}
			else
			{
				l->append(((KviAliasListViewItem *)m_pListView->topLevelItem(i)));
			}
		}
		else
		{
			if(bSelectedOnly)
			{
				if(m_pListView->topLevelItem(i)->isSelected())
					appendAliasItemsRecursive(l,m_pListView->topLevelItem(i),false);
				else
					appendAliasItemsRecursive(l,m_pListView->topLevelItem(i),true);
			}
			else
				appendAliasItemsRecursive(l,m_pListView->topLevelItem(i),false);
		}
	}

}
void KviAliasEditor::appendAliasItemsRecursive(KviPointerList<KviAliasListViewItem> * l,QTreeWidgetItem * pStartFrom,bool bSelectedOnly)
{
	for (int i=0;i<pStartFrom->childCount();i++)
	{
		if (((KviAliasListViewItem *)pStartFrom->child(i))->isAlias())
		{
			if(bSelectedOnly)
			{
				if(pStartFrom->child(i)->isSelected())
					l->append((KviAliasListViewItem *)pStartFrom->child(i));
			}
			else
			{
				l->append((KviAliasListViewItem *)pStartFrom->child(i));
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


void KviAliasEditor::appendNamespaceItems(KviPointerList<KviAliasListViewItem> * l,bool bSelectedOnly)
{
	for (int i=0;i<m_pListView->topLevelItemCount();i++)
	{
		if (((KviAliasListViewItem *)m_pListView->topLevelItem(i))->isNamespace())
		{
			if(bSelectedOnly)
			{
				if(m_pListView->topLevelItem(i)->isSelected())
					l->append(((KviAliasListViewItem *)m_pListView->topLevelItem(i)));
			}
			else
			{
				l->append(((KviAliasListViewItem *)m_pListView->topLevelItem(i)));
			}
		}
		else
		{
			if(bSelectedOnly)
			{
				if(m_pListView->topLevelItem(i)->isSelected())
					appendNamespaceItemsRecursive(l,m_pListView->topLevelItem(i),false);
				else
					appendNamespaceItemsRecursive(l,m_pListView->topLevelItem(i),true);
			}
			else
				appendNamespaceItemsRecursive(l,m_pListView->topLevelItem(i),false);
		}
	}

}
void KviAliasEditor::appendNamespaceItemsRecursive(KviPointerList<KviAliasListViewItem> * l,QTreeWidgetItem * pStartFrom,bool bSelectedOnly)
{
	for (int i=0;i<pStartFrom->childCount();i++)
	{
		if (((KviAliasListViewItem *)pStartFrom->child(i))->isNamespace())
		{
			if(bSelectedOnly)
			{
				if(pStartFrom->child(i)->isSelected())
					l->append((KviAliasListViewItem *)pStartFrom->child(i));
			}
			else
			{
				l->append((KviAliasListViewItem *)pStartFrom->child(i));
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

void KviAliasEditor::appendSelectedItemsRecursive(KviPointerList<KviAliasEditorListViewItem> * l,QTreeWidgetItem *it)

{
	for (int i=0;i<it->childCount();i++)
	{
		if (it->child(i)->isSelected()) 
		{
			l->append((KviAliasEditorListViewItem *)it->child(i));
		}
		else
		{
			if (it->child(i)->childCount())
					appendSelectedItemsRecursive(l,it->child(i));
		}
	}
}
void KviAliasEditor::appendSelectedItems(KviPointerList<KviAliasEditorListViewItem> * l)
{
	for (int i=0;i<m_pListView->topLevelItemCount();i++)
	{
		if (m_pListView->topLevelItem(i)->isSelected()) 
		{
			l->append((KviAliasEditorListViewItem *)m_pListView->topLevelItem(i));
		}
		else
		{
			if (m_pListView->topLevelItem(i)->childCount())
					appendSelectedItemsRecursive(l,m_pListView->topLevelItem(i));
		}
	}
}

void KviAliasEditor::removeItemChildren(KviAliasEditorListViewItem *it)
{
	for (int i=0;i<it->childCount();i++)
	{
		if (it->child(i)->childCount()) removeItemChildren((KviAliasEditorListViewItem *)it->child(i));
		delete it->child(i);
	}
}
bool KviAliasEditor::removeItem(KviAliasEditorListViewItem *it,bool * pbYesToAll,bool bDeleteEmptyTree)
{
	if(!it)return true;
	QString szMsg;
	QString szName = it->name();

	if(!*pbYesToAll)
	{
		if(it->isAlias())
			KviQString::sprintf(szMsg,__tr2qs("Do you really want to remove the alias \"%Q\" ?"),&szName);
		else {
			KviQString::sprintf(szMsg,__tr2qs("Do you really want to remove the namespace \"%Q\" ?"),&szName);
			szMsg += "<br>";
			szMsg += __tr2qs("Please note that all the children items will be deleted too.");
		}

		g_pAliasEditorModule->lock();
		int ret = QMessageBox::question(this,__tr2qs("Remove item"),szMsg,__tr2qs("Yes"),__tr2qs("Yes to All"),__tr2qs("No"));
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
			KviAliasEditorListViewItem * par = (KviAliasEditorListViewItem *)(it->parent());
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
	KviPointerList<KviAliasEditorListViewItem> l;
	l.setAutoDelete(false);
	appendSelectedItems(&l);
	bool bYesToAll = false;
	
	for(KviAliasEditorListViewItem *it = l.first();it;it = l.next())
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
		szNewName = QInputDialog::getText(
						szAction,
						szText,
						QLineEdit::Normal,
						szInitialText,
						&bOk,
						this);
		g_pAliasEditorModule->unlock();
		if(!bOk)return QString::null;
		if(szNewName.isEmpty())
		{
			g_pAliasEditorModule->lock();
			QMessageBox::warning(this,
						__tr2qs("Missing Alias Name"),
						__tr2qs("You must specify a valid name for the alias"),
						__tr2qs("Ok, Let me try again..."));
			g_pAliasEditorModule->unlock();
			continue;
		}
		
		// we allow only [\w:]+
		QRegExp re("[\\w:]+");
		if(!re.exactMatch(szNewName))
		{
			g_pAliasEditorModule->lock();
			QMessageBox::information(this,
						__tr2qs("Bad Alias Name"),
						__tr2qs("Alias names can contain only letters, digits, underscores and '::' namespace separators"),
						__tr2qs("Ok, Let me try again..."));
			g_pAliasEditorModule->unlock();
			szNewName = "";
			continue;
		}
		// make sure that we have only doubled "::" and not ":" or ":::..."
		QString tmp = szNewName;
		tmp.replace("::","@"); // @ is not allowed by the rule above
		if(tmp.find(":") != -1)
		{
			g_pAliasEditorModule->lock();
			QMessageBox::information(this,
					__tr2qs("Bad Alias Name"),
					__tr2qs("Stray ':' character in alias name: did you mean ...<namespace>::<name> ?"),
					__tr2qs("Ok, Let me try again..."));
			g_pAliasEditorModule->unlock();
			szNewName = "";
			continue;
		}
		if(tmp.find("@@") != -1)
		{
			g_pAliasEditorModule->lock();
			QMessageBox::information(this,
					__tr2qs("Bad Alias Name"),
					__tr2qs("Found an empty namespace in alias name"),
					__tr2qs("Ok, Let me try again..."));
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
		szNewName = QInputDialog::getText(
						szAction,
						szText,
						QLineEdit::Normal,
						szInitialText,
						&bOk,
						this);
		g_pAliasEditorModule->unlock();
		if(!bOk)return QString::null;
		if(szNewName.isEmpty())
		{
			g_pAliasEditorModule->lock();
			QMessageBox::warning(this,
						__tr2qs("Missing Namespace Name"),
						__tr2qs("You must specify a valid name for the namespace"),
						__tr2qs("Ok, Let me try again..."));
			g_pAliasEditorModule->unlock();
			continue;
		}
		
		// we allow only [\w:]+
		QRegExp re("[\\w:]+");
		if(!re.exactMatch(szNewName))
		{
			g_pAliasEditorModule->lock();
			QMessageBox::information(this,
						__tr2qs("Bad Namespace Name"),
						__tr2qs("Namespace names can contain only letters, digits, underscores and '::' namespace separators"),
						__tr2qs("Ok, Let me try again..."));
			g_pAliasEditorModule->unlock();
			szNewName = "";
			continue;
		}
		// make sure that we have only doubled "::" and not ":" or ":::..."
		QString tmp = szNewName;
		tmp.replace("::","@"); // @ is not allowed by the rule above
		if(tmp.find(":") != -1)
		{
			g_pAliasEditorModule->lock();
			QMessageBox::information(this,
					__tr2qs("Bad Namespace Name"),
					__tr2qs("Stray ':' character in namespace name: did you mean ...<namespace>::<name> ?"),
					__tr2qs("Ok, Let me try again..."));
			g_pAliasEditorModule->unlock();
			szNewName = "";
			continue;
		}
		if(tmp.find("@@") != -1)
		{
			g_pAliasEditorModule->lock();
			QMessageBox::information(this,
					__tr2qs("Bad Namespace Name"),
					__tr2qs("Found an empty namespace in namespace name"),
					__tr2qs("Ok, Let me try again..."));
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

void KviAliasEditor::selectOneItem(QTreeWidgetItem * it,QTreeWidgetItem *pStartFrom)
{
	/*
	if(!pStartFrom)return;
	if(pStartFrom == it)pStartFrom->setSelected(true);
	else pStartFrom->setSelected(false);
	selectOneItem(it,pStartFrom->firstChild());
	selectOneItem(it,pStartFrom->nextSibling());
	*/


}

void KviAliasEditor::activateItem(QTreeWidgetItem * it)
{
	openParentItems(it);
	//selectOneItem(it,m_pListView->child(0));
	m_pListView->setCurrentItem(it);
}

void KviAliasEditor::newAlias()
{
	if(m_pLastClickedItem)
	{
		if(!itemExists(m_pLastClickedItem))return; // dead ?
		if(m_pLastClickedItem->isAlias())
			m_pLastClickedItem = m_pLastClickedItem->parentNamespaceItem();
	}

	QString szNewName = askForAliasName(__tr2qs("Add Alias"),__tr2qs("Please enter the name for the new alias"),"myfunction");
	if(szNewName.isEmpty())return;

	getUniqueItemName(m_pLastClickedItem,szNewName,KviAliasEditorListViewItem::Alias);
	
	KviAliasListViewItem * it;
	
	if(m_pLastClickedItem)
		it = ((KviAliasNamespaceListViewItem *)m_pLastClickedItem)->createFullAliasItem(szNewName);
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
		

	QString szNewName = askForNamespaceName(__tr2qs("Add Namespace"),__tr2qs("Please enter the name for the new namespace"),"mynamespace");
	if(szNewName.isEmpty())return;

	getUniqueItemName(m_pLastClickedItem,szNewName,KviAliasEditorListViewItem::Namespace);
	
	KviAliasNamespaceListViewItem * it;
	
	if(m_pLastClickedItem)
		it = ((KviAliasNamespaceListViewItem *)m_pLastClickedItem)->createFullNamespaceItem(szNewName);
	else
		it = createFullNamespaceItem(szNewName);

	activateItem(it);
}

bool KviAliasEditor::aliasExists(QString &szFullItemName)
{
	KviPointerList<KviAliasListViewItem> l;
	l.setAutoDelete(false);

	appendAliasItems(&l,false);
	for(KviAliasListViewItem * it = l.first();it;it = l.next())
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
	KviPointerList<KviAliasListViewItem> l;
	l.setAutoDelete(false);
	
	appendNamespaceItems(&l,false);
	for(KviAliasListViewItem * it = l.first();it;it = l.next())
	{
		if (KviQString::equalCI(buildFullItemName(it),szFullItemName))
		{
			return true;
		}
	}
	return false;
}

void KviAliasEditor::renameItem()
{
	if(!m_pLastEditedItem)return;
	if(!itemExists(m_pLastEditedItem))return; // dead ?

	QString szName = buildFullItemName(m_pLastEditedItem);
	QString szNewName;

	bool bAlias = m_pLastEditedItem->isAlias();
	
	if(bAlias)
		szNewName = askForAliasName(__tr2qs("Rename Alias"),__tr2qs("Please enter the new name for the alias"),szName);
	else
		szNewName = askForNamespaceName(__tr2qs("Rename Namespace"),__tr2qs("Please enter the new name for the namespace"),szName);
	if(szNewName.isEmpty())return;

	if(szName == szNewName)return;

	// check if there is already an alias with this name
	if (bAlias)
	{
		if (aliasExists(szNewName))
		{
			g_pAliasEditorModule->lock();
			QMessageBox::information(this,
				__tr2qs("Alias already exists"),
				__tr2qs("This name is already in use. Please choose another one."),
				__tr2qs("Ok, Let me try again..."));
			g_pAliasEditorModule->unlock();
			return;
		}
	} else {
	// check if there is already a anmespace with this name
		if (namespaceExists(szNewName))
		{
			g_pAliasEditorModule->lock();
			QMessageBox::information(this,
				__tr2qs("Namespace already exists"),
				__tr2qs("This name is already in use. Please choose another one."),
				__tr2qs("Ok, let me try again..."));
			g_pAliasEditorModule->unlock();
			return;
		}
	}
	
	QString szCode;
	QPoint pntCursor;
	if(bAlias)
	{
		m_pEditor->getText(szCode);
		pntCursor = m_pEditor->getCursor();
	}

	// take child items, if any
	KviPointerList<QTreeWidgetItem> lChildren;
	lChildren.setAutoDelete(false);
//	QTreeWidgetItem * it = m_pLastEditedItem->firstChild();
	/*while(it)
	{
		lChildren.append(it);
		it = it->nextSibling();
	}
	*/
//	for(it = lChildren.first();it;it = lChildren.next())
//		m_pLastEditedItem->takeItem(it);

	bool bYesToAll = true;
	removeItem(m_pLastEditedItem,&bYesToAll,true);

	m_pLastEditedItem = 0; // make sure it's true (but it already should be because of removeItem())
	m_pLastClickedItem = 0; // make sure it's true (but it already should be because of removeItem())
	
	if(bAlias)
	{
		KviAliasListViewItem * ait = createFullAliasItem(szNewName);
		ait->setBuffer(szCode);
		ait->setCursorPosition(pntCursor);
		activateItem(ait);

//		for(it = lChildren.first();it;it = lChildren.next())
//			ait->insertItem(it);
	} else {
		KviAliasNamespaceListViewItem * nit = createFullNamespaceItem(szNewName);
		activateItem(nit);
/*
		for(it = lChildren.first();it;it = lChildren.next())
		{
			((KviAliasEditorListViewItem*)it)->setParentNamespaceItem(nit);
			nit->insertItem(it);
		}
		*/
	}
}

// warning: this saves ONLY the body, not the name!
//          the name is changed explicitly with renameItem(), when needed
void KviAliasEditor::saveLastEditedItem()
{
	if(!m_pLastEditedItem)return;
	if(!m_pEditor->isModified())return; // nothing to save
	if(!itemExists(m_pLastEditedItem))return; // dead ?

	if(m_pLastEditedItem->isNamespace())return;

	QString newCode;
	m_pEditor->getText(newCode);
	((KviAliasListViewItem *)m_pLastEditedItem)->setBuffer(newCode);
	((KviAliasListViewItem *)m_pLastEditedItem)->setCursorPosition(m_pEditor->getCursor());
}

void KviAliasEditor::currentItemChanged(QTreeWidgetItem *it,QTreeWidgetItem *prev)
{
	saveLastEditedItem();

	m_pLastEditedItem = (KviAliasEditorListViewItem *)it;

	if(!m_pLastEditedItem)
	{
		m_pNameLabel->setText(__tr2qs("No item selected"));
		m_pRenameButton->setEnabled(false);
		m_pEditor->setText("");
		m_pEditor->setEnabled(false);
		return;
	}
	
	QString szNam = buildFullItemName(m_pLastEditedItem);
	if(m_pLastEditedItem->isNamespace())
	{
		QString szLabelText = __tr2qs("Namespace");
		szLabelText += ": <b>";
		szLabelText += szNam;
		szLabelText += "</b>";
		m_pNameLabel->setText(szLabelText);
		m_pRenameButton->setEnabled(true);
		m_pEditor->setText("");
		m_pEditor->setEnabled(false);
		return;
	}

	QString szLabelText = __tr2qs("Alias");
	szLabelText += ": <b>";
	szLabelText += szNam;
	szLabelText += "</b>";
	m_pNameLabel->setText(szLabelText);
	m_pRenameButton->setEnabled(true);
	m_pEditor->setText(((KviAliasListViewItem *)it)->buffer());
	m_pEditor->setCursorPosition(((KviAliasListViewItem *)it)->cursorPosition());
	m_pEditor->setEnabled(true);
}

void KviAliasEditor::recursiveCommit(KviAliasEditorListViewItem * it)
{
	if(!it)return;
	if(it->isAlias())
	{
		QString szName = buildFullItemName(it);
		//debug("ADDING %s",szName.latin1());
		// WARNING: On MSVC operator new here is valid ONLY because
		//          KviKvsScript has a non virtual detructor!
		KviKvsScript * a = new KviKvsScript(szName,((KviAliasListViewItem *)it)->buffer());
		KviKvsAliasManager::instance()->add(szName,a);
		return;
	} else 
	{
		for (int i=0;i<it->childCount();i++)
		{
			recursiveCommit((KviAliasEditorListViewItem *) it->child(i));
		}
	}
}

void KviAliasEditor::commit()
{
	saveLastEditedItem();
	KviKvsAliasManager::instance()->clear();
	for(int i=0;i<m_pListView->topLevelItemCount();i++)
	{
		KviAliasEditorListViewItem *item=(KviAliasEditorListViewItem *)m_pListView->topLevelItem(i);
		recursiveCommit(item);
	}
	g_pApp->saveAliases();
}

void KviAliasEditor::getUniqueItemName(KviAliasEditorListViewItem *item,QString &buffer,KviAliasEditorListViewItem::Type eType)
{
	QString ret;
	
	int idx = 0;
	KviAliasEditorListViewItem * found;
	
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
			
			if(eType == KviAliasEditorListViewItem::Namespace)
				found = ((KviAliasNamespaceListViewItem *)item)->findNamespaceItem(ret);
			else
				found = ((KviAliasNamespaceListViewItem *)item)->findAliasItem(ret);
		} else {
			if(eType == KviAliasEditorListViewItem::Namespace)
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

	setFixedCaption(__tr2qs("Alias Editor"));

	m_pEditor = new KviAliasEditor(this);

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

	g->setColumnStretch(0,1);
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

void KviAliasEditorWindow::resizeEvent(QResizeEvent *e)
{
	int hght = m_pBase->sizeHint().height();
	m_pEditor->setGeometry(0,0,width(),height()- hght);
	m_pBase->setGeometry(0,height() - hght,width(),hght);
}

void KviAliasEditorWindow::getConfigGroupName(KviStr &szName)
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
