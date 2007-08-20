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

#include <qsplitter.h>
#include <qlayout.h>
#include "kvi_tal_vbox.h"
#include <qtooltip.h>
#include <qpushbutton.h>
#include <qdir.h>
#include <qmessagebox.h>
#include <qinputdialog.h>
#include <qregexp.h>

extern KviAliasEditorWindow * g_pAliasEditorWindow;
extern KviModule * g_pAliasEditorModule;

KviAliasEditorListViewItem::KviAliasEditorListViewItem(KviTalListView * pListView,Type eType,const QString &szName)
: KviTalListViewItem(pListView), m_eType(eType), m_pParentNamespaceItem(0)
{
	setName(szName);
}

KviAliasEditorListViewItem::KviAliasEditorListViewItem(KviAliasNamespaceListViewItem * pParentNamespaceItem,Type eType,const QString &szName)
: KviTalListViewItem(pParentNamespaceItem), m_eType(eType), m_pParentNamespaceItem(pParentNamespaceItem)
{
	setName(szName);
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
	setPixmap(0,*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIAS)));
	m_cPos=QPoint(0,0);
}

KviAliasListViewItem::KviAliasListViewItem(KviTalListView * pListView,const QString &szName)
: KviAliasEditorListViewItem(pListView,KviAliasEditorListViewItem::Alias,szName)
{
	setPixmap(0,*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIAS)));
	m_cPos=QPoint(0,0);
}


KviAliasNamespaceListViewItem::KviAliasNamespaceListViewItem(KviTalListView * pListView,const QString &szName)
: KviAliasEditorListViewItem(pListView,KviAliasEditorListViewItem::Namespace,szName)
{
	setPixmap(0,*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NAMESPACE)));
}

KviAliasNamespaceListViewItem::KviAliasNamespaceListViewItem(KviAliasNamespaceListViewItem * pParentNamespaceItem,const QString &szName)
: KviAliasEditorListViewItem(pParentNamespaceItem,KviAliasEditorListViewItem::Namespace,szName)
{
	setPixmap(0,*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NAMESPACE)));
}


KviAliasNamespaceListViewItem * KviAliasNamespaceListViewItem::findNamespaceItem(const QString &szName)
{
	KviAliasEditorListViewItem * it = (KviAliasEditorListViewItem *)firstChild();
	while(it)
	{
		if(it->isNamespace())
		{
			if(KviQString::equalCI(szName,it->name()))return (KviAliasNamespaceListViewItem *)it;
		}
		it = (KviAliasEditorListViewItem *)it->nextSibling();
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
	KviAliasEditorListViewItem * it = (KviAliasEditorListViewItem *)firstChild();
	while(it)
	{
		if(it->isAlias())
		{
			if(KviQString::equalCI(szName,it->name()))return (KviAliasListViewItem *)it;
		}
		it = (KviAliasEditorListViewItem *)it->nextSibling();
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


KviAliasEditor::KviAliasEditor(QWidget * par)
: QWidget(par)
{
	m_pLastEditedItem = 0;
	m_pLastClickedItem = 0;
	m_szDir = QDir::homeDirPath();
  

	QGridLayout * l = new QGridLayout(this,1,1,2,2);

#ifdef COMPILE_USE_QT4
	m_pSplitter = new QSplitter(Qt::Horizontal,this);
	m_pSplitter->setOpaqueResize(false);
#else
	m_pSplitter = new QSplitter(QSplitter::Horizontal,this);
#endif

	l->addWidget(m_pSplitter,0,0);
	
	KviTalVBox * box = new KviTalVBox(m_pSplitter);
	m_pListView = new KviTalListView(box);
	m_pListView->addColumn(__tr2qs("Alias"));
	m_pListView->setSelectionMode(KviTalListView::Extended);
	m_pListView->setShowSortIndicator(true);
	m_pListView->setRootIsDecorated(true);

	box = new KviTalVBox(m_pSplitter);
	KviTalHBox * hbox = new KviTalHBox(box);
	m_pNameLabel = new QLabel(__tr2qs("No item selected"),hbox);
	m_pRenameButton = new QPushButton(__tr2qs("Rename"),hbox);
	m_pRenameButton->setEnabled(false);
	connect(m_pRenameButton,SIGNAL(clicked()),this,SLOT(renameItem()));
	hbox->setStretchFactor(m_pNameLabel,2);
	QToolTip::add(m_pRenameButton,__tr2qs("Edit the alias or namespace name"));
	m_pEditor = KviScriptEditor::createInstance(box);
	m_pEditor->setFocus();
	connect(m_pEditor,SIGNAL(find(const QString &)),this,SLOT(slotFindWord(const QString &)));
	connect(m_pEditor,SIGNAL(replaceAll(const QString &,const QString &)),this,SLOT(slotReplaceAll(const QString &,const QString &)));

	m_pContextPopup = new KviTalPopupMenu(this);

	oneTimeSetup();

	currentItemChanged(0);
}

KviAliasEditor::~KviAliasEditor()
{
	KviScriptEditor::destroyInstance(m_pEditor);
}

KviAliasNamespaceListViewItem * KviAliasEditor::findNamespaceItem(const QString &szName)
{
	KviAliasNamespaceListViewItem * it = (KviAliasNamespaceListViewItem *)m_pListView->firstChild();
	while(it)
	{
		if(KviQString::equalCI(szName,it->name()))return it;
		it = (KviAliasNamespaceListViewItem *)it->nextSibling();
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

KviAliasListViewItem * KviAliasEditor::findAliasItem(const QString &szName)
{
	KviAliasEditorListViewItem * it = (KviAliasEditorListViewItem *)m_pListView->firstChild();
	while(it)
	{
		if(it->isAlias())
		{
			if(KviQString::equalCI(szName,it->name()))return (KviAliasListViewItem *)it;
		}
		it = (KviAliasEditorListViewItem *)it->nextSibling();
	}
	return 0;
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
	KviDict<KviKvsScript> * a = KviKvsAliasManager::instance()->aliasDict();
	if(!a)return;

	KviDictIterator<KviKvsScript> it(*a);

	KviAliasListViewItem * item;
	while(it.current())
	{
		KviKvsScript * alias = it.current();
		item = createFullAliasItem(alias->name());
		item->setBuffer(alias->code());
		++it;
	}
		
	connect(m_pListView,SIGNAL(currentChanged(KviTalListViewItem *)),this,SLOT(currentItemChanged(KviTalListViewItem *)));
	connect(m_pListView,SIGNAL(rightButtonPressed(KviTalListViewItem *,const QPoint &,int)),
		this,SLOT(itemPressed(KviTalListViewItem *,const QPoint &,int)));
}

bool KviAliasEditor::hasSelectedItems(KviTalListViewItem * it)
{
	if(!it)return false;
	if(it->isSelected())return true;
	if(it->isOpen())
	{
		if(hasSelectedItems(it->firstChild()))return true;
	}
	return hasSelectedItems(it->nextSibling());
}

bool KviAliasEditor::itemExists(KviTalListViewItem *pSearchFor,KviTalListViewItem * pSearchAt)
{
	if(!pSearchFor)return false;
	if(!pSearchAt)return false;
	if(pSearchFor == pSearchAt)return true;
	if(itemExists(pSearchFor,pSearchAt->firstChild()))return true;
	return itemExists(pSearchFor,pSearchAt->nextSibling());
}


void KviAliasEditor::itemPressed(KviTalListViewItem *it,const QPoint &pnt,int col)
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
	

	bool bHasItems = m_pListView->firstChild() != 0;
	bool bHasSelected = hasSelectedItems(m_pListView->firstChild());

	m_pContextPopup->insertSeparator();
	
	id = m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_QUIT)),
			__tr2qs("Remove Selected"),
			this,SLOT(removeSelectedItems()));
	m_pContextPopup->setItemEnabled(id,bHasSelected);


	m_pContextPopup->insertSeparator();

	m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_FOLDER)),
			__tr2qs("Export Selected..."),
			this,SLOT(exportSelected()));
	m_pContextPopup->setItemEnabled(id,bHasSelected);

	m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_FOLDER)),
			__tr2qs("Export Selected in singles files..."),
			this,SLOT(exportSelectedSepFiles()));

	m_pContextPopup->setItemEnabled(id,bHasSelected);

	m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_FOLDER)),
			__tr2qs("Export All..."),
			this,SLOT(exportAll()));
	m_pContextPopup->setItemEnabled(id,bHasItems);

	m_pContextPopup->insertSeparator();

	m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_SEARCH)),
			__tr2qs("Find In Aliases..."),
			this,SLOT(slotFind()));
	m_pContextPopup->setItemEnabled(id,bHasItems);

	m_pContextPopup->insertItem(
			*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NAMESPACE)),
			__tr2qs("Collapse All Namespaces"),
			this,SLOT(slotCollapseNamespaces()));

	m_pContextPopup->setItemEnabled(id,bHasItems);


	m_pContextPopup->popup(pnt);
}

void KviAliasEditor::recursiveSearchReplace(const QString &szSearch,KviAliasEditorListViewItem * it,bool bReplace,const QString &szReplace)
{
	if(!it)return;
	if(it->isAlias())
	{
		if(((KviAliasListViewItem *)it)->buffer().find(szSearch,0,false) != -1)
		{
			it->setPixmap(0,*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIASHIGHLIGHTED)));
			if (bReplace) ((QString &)((KviAliasListViewItem *)it)->buffer()).replace(szSearch,szReplace,false);
			openParentItems(it);
		} else
			it->setPixmap(0,*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ALIAS)));
	} else {
		recursiveSearchReplace(szSearch,(KviAliasEditorListViewItem *)(it->firstChild()),bReplace,szReplace);
	}
	recursiveSearchReplace(szSearch,(KviAliasEditorListViewItem *)(it->nextSibling()),bReplace,szReplace);
}

void KviAliasEditor::slotFind()
{
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
}
void KviAliasEditor::slotFindWord(const QString &szSearch)
{
	m_pEditor->setFindText(szSearch);
	
	recursiveSearchReplace(szSearch,(KviAliasEditorListViewItem *)m_pListView->firstChild());

}
void KviAliasEditor::recursiveCollapseNamespaces(KviAliasEditorListViewItem * it)
{
	if(!it)return;
	if(it->isNamespace())
	{
		recursiveCollapseNamespaces((KviAliasEditorListViewItem *)(it->firstChild()));
	}
	recursiveCollapseNamespaces((KviAliasEditorListViewItem *)(it->nextSibling()));
	it->setOpen(false);
}

void KviAliasEditor::slotCollapseNamespaces()
{
	recursiveCollapseNamespaces((KviAliasEditorListViewItem *)m_pListView->firstChild());
}


void KviAliasEditor::slotReplaceAll(const QString &before,const QString &after)
{
	recursiveSearchReplace(before,(KviAliasEditorListViewItem *)m_pListView->firstChild(),true,after);
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

void KviAliasEditor::exportSelectionInSinglesFiles(KviPtrList<KviAliasListViewItem> *l)
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
	
	KviPtrList<KviAliasListViewItem> l;
	l.setAutoDelete(false);

	QString out;

	appendAliasItems(&l,(KviAliasEditorListViewItem *)(m_pListView->firstChild()),bSelectedOnly);
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

void KviAliasEditor::appendAliasItems(KviPtrList<KviAliasListViewItem> * l,KviAliasEditorListViewItem * pStartFrom,bool bSelectedOnly)
{
	if(!pStartFrom)return;
	if(pStartFrom->isAlias())
	{
		if(bSelectedOnly)
		{
			if(pStartFrom->isSelected())
				l->append((KviAliasListViewItem *)pStartFrom);
		} else {
			l->append((KviAliasListViewItem *)pStartFrom);
		}
	} else {
		if(bSelectedOnly)
		{
			if(pStartFrom->isSelected())
				appendAliasItems(l,(KviAliasEditorListViewItem *)(pStartFrom->firstChild()),false); // ALL the items below the selected namespace
			else
				appendAliasItems(l,(KviAliasEditorListViewItem *)(pStartFrom->firstChild()),true); // only the selected items below
		} else {
			appendAliasItems(l,(KviAliasEditorListViewItem *)(pStartFrom->firstChild()),false);
		}
	}
	appendAliasItems(l,(KviAliasEditorListViewItem *)(pStartFrom->nextSibling()),bSelectedOnly);
}

void KviAliasEditor::appendNamespaceItems(KviPtrList<KviAliasListViewItem> * l,KviAliasEditorListViewItem * pStartFrom,bool bSelectedOnly)
{
	if(!pStartFrom)return;
	if(pStartFrom->isNamespace())
	{
		if(bSelectedOnly)
		{
			if(pStartFrom->isSelected())
				l->append((KviAliasListViewItem *)pStartFrom);
		} else {
			l->append((KviAliasListViewItem *)pStartFrom);
		}
	} else {
		if(bSelectedOnly)
		{
			if(pStartFrom->isSelected())
				appendNamespaceItems(l,(KviAliasEditorListViewItem *)(pStartFrom->firstChild()),false); // ALL the items below the selected namespace
			else
				appendNamespaceItems(l,(KviAliasEditorListViewItem *)(pStartFrom->firstChild()),true); // only the selected items below
		} else {
			appendNamespaceItems(l,(KviAliasEditorListViewItem *)(pStartFrom->firstChild()),false);
		}
	}
	appendNamespaceItems(l,(KviAliasEditorListViewItem *)(pStartFrom->nextSibling()),bSelectedOnly);
}

void KviAliasEditor::appendSelectedItems(KviPtrList<KviAliasEditorListViewItem> * l,KviAliasEditorListViewItem * pStartFrom,bool bIncludeChildrenOfSelected)
{
	if(!pStartFrom)return;
	if(pStartFrom->isSelected())
	{
		l->append(pStartFrom);
		if(bIncludeChildrenOfSelected)
			appendSelectedItems(l,(KviAliasEditorListViewItem *)(pStartFrom->firstChild()),bIncludeChildrenOfSelected);
	} else {
		appendSelectedItems(l,(KviAliasEditorListViewItem *)(pStartFrom->firstChild()),bIncludeChildrenOfSelected);
	}
	appendSelectedItems(l,(KviAliasEditorListViewItem *)(pStartFrom->nextSibling()),bIncludeChildrenOfSelected);
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
	if(bDeleteEmptyTree)
	{
		while(it)
		{
			KviAliasEditorListViewItem * par = (KviAliasEditorListViewItem *)(it->parent());
			delete it;
			if(par)
			{
				if(!par->firstChild())it = par;
				else it = 0;
			} else {
				it = 0;
			}
		}
	} else {
		delete it;
	}
	return true;
}

void KviAliasEditor::removeSelectedItems()
{
	KviPtrList<KviAliasEditorListViewItem> l;
	l.setAutoDelete(false);
	appendSelectedItems(&l,(KviAliasEditorListViewItem *)(m_pListView->firstChild()),false);

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

void KviAliasEditor::openParentItems(KviTalListViewItem * it)
{
	if(it->parent())
	{
		it->parent()->setOpen(true);
		openParentItems(it->parent());
	}
}


void KviAliasEditor::selectOneItem(KviTalListViewItem * it,KviTalListViewItem *pStartFrom)
{
	if(!pStartFrom)return;
	if(pStartFrom == it)pStartFrom->setSelected(true);
	else pStartFrom->setSelected(false);
	selectOneItem(it,pStartFrom->firstChild());
	selectOneItem(it,pStartFrom->nextSibling());
}


void KviAliasEditor::activateItem(KviTalListViewItem * it)
{
	openParentItems(it);
	selectOneItem(it,m_pListView->firstChild());
	m_pListView->setCurrentItem(it);
}


void KviAliasEditor::newAlias()
{
	if(m_pLastClickedItem)
	{
		if(!itemExists(m_pLastClickedItem,m_pListView->firstChild()))return; // dead ?
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
		if(!itemExists(m_pLastClickedItem,m_pListView->firstChild()))return; // dead ?
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
	KviPtrList<KviAliasListViewItem> l;
	l.setAutoDelete(false);

	appendAliasItems(&l,(KviAliasEditorListViewItem *)(m_pListView->firstChild()),false);
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
	KviPtrList<KviAliasListViewItem> l;
	l.setAutoDelete(false);
	
	appendNamespaceItems(&l,(KviAliasEditorListViewItem *)(m_pListView->firstChild()),false);
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
	if(!itemExists(m_pLastEditedItem,m_pListView->firstChild()))return; // dead ?

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
	KviPtrList<KviTalListViewItem> lChildren;
	lChildren.setAutoDelete(false);
	KviTalListViewItem * it = m_pLastEditedItem->firstChild();
	while(it)
	{
		lChildren.append(it);
		it = it->nextSibling();
	}
	for(it = lChildren.first();it;it = lChildren.next())
		m_pLastEditedItem->takeItem(it);

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

		for(it = lChildren.first();it;it = lChildren.next())
			ait->insertItem(it);
	} else {
		KviAliasNamespaceListViewItem * nit = createFullNamespaceItem(szNewName);
		activateItem(nit);

		for(it = lChildren.first();it;it = lChildren.next())
		{
			((KviAliasEditorListViewItem*)it)->setParentNamespaceItem(nit);
			nit->insertItem(it);
		}
	}
}

// warning: this saves ONLY the body, not the name!
//          the name is changed explicitly with renameItem(), when needed
void KviAliasEditor::saveLastEditedItem()
{
	if(!m_pLastEditedItem)return;
	if(!m_pEditor->isModified())return; // nothing to save
	if(!itemExists(m_pLastEditedItem,m_pListView->firstChild()))return; // dead ?

	if(m_pLastEditedItem->isNamespace())return;

	QString newCode;
	m_pEditor->getText(newCode);
	((KviAliasListViewItem *)m_pLastEditedItem)->setBuffer(newCode);
	((KviAliasListViewItem *)m_pLastEditedItem)->setCursorPosition(m_pEditor->getCursor());
}

void KviAliasEditor::currentItemChanged(KviTalListViewItem *it)
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
	} else {
		recursiveCommit((KviAliasEditorListViewItem *)(it->firstChild()));
	}
	recursiveCommit((KviAliasEditorListViewItem *)(it->nextSibling()));
}


void KviAliasEditor::commit()
{
	saveLastEditedItem();
	KviKvsAliasManager::instance()->clear();

	recursiveCommit((KviAliasEditorListViewItem *)(m_pListView->firstChild()));

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

	g->setColStretch(0,1);
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
