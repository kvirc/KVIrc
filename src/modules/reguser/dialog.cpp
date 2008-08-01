//=============================================================================
//
//   File : edituser.cpp
//   Creation date : Tue Dec 26 2000 12:24:12 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2004 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
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
#define _WANT_OPTION_FLAGS_

#include "edituser.h"
#include "wizard.h"
#include "dialog.h"

#include "kvi_regusersdb.h"
#include "kvi_locale.h"
#include "kvi_ircmask.h"
#include "kvi_debug.h"
#include "kvi_iconmanager.h"
#include "kvi_app.h"
#include "kvi_options.h"
#include "kvi_file.h"
#include "kvi_filedialog.h"
#include "kvi_msgbox.h"
#include "kvi_fileutils.h"
#include "kvi_settings.h"
#include "kvi_stringconversion.h"
#include "kvi_options.h"
#include "kvi_pointerhashtable.h"
#include "kvi_tal_hbox.h"
#include "kvi_tal_vbox.h"

#include <QLayout>
#include <QLabel>
#include <QImageWriter>
#include <QImageReader>
#include <QImage>
#include <QString>
#include <QComboBox>
#include <QToolTip>
#include <QStyle>
#include <QPainter>
#include <QInputDialog>
#include <QEvent>
#include <QCloseEvent>
#include <QAbstractItemView>

#define LVI_ICON_SIZE 32
#define LVI_BORDER 4
#define LVI_SPACING 8
#define LVI_MINIMUM_TEXT_WIDTH 300
#define LVI_MINIMUM_CELL_WIDTH (LVI_MINIMUM_TEXT_WIDTH + LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING + LVI_BORDER)


extern KviRegisteredUsersDialog * g_pRegisteredUsersDialog;

KviRegisteredUsersDialogItem::KviRegisteredUsersDialogItem(KviTalListViewItem * par,KviRegisteredUser * u)
: KviRegisteredUsersDialogItemBase(User,par), m_pUser(u)
{
	QString szTmp;
	QString t = "<nobr><b>";
	t += u->name();
	t += "</b> [";
	szTmp = m_pUser->getProperty("notify");
	if(szTmp.isEmpty())
	{
		t += __tr2qs("Notify disabled");
	} else {
		t += __tr2qs("Notify as: ");
		t += szTmp;

	}
	t += "]</nobr>";
	t += "<br><nobr><font size=\"-1\">";
	szTmp = m_pUser->getProperty("comment");
	if(szTmp.isEmpty())
	{
		t += __tr2qs("No comment set");
	} else {
		t += __tr2qs("Comment: ");
		t += m_pUser->getProperty("comment");
	}
	t += "</font></nobr>";
	m_pText = new QTextDocument();
	m_pText->setHtml(t);
	m_pText->setDefaultFont(listView()->font());
	//setText(0,u->name());
}

KviRegisteredUsersDialogItem::~KviRegisteredUsersDialogItem()
{
	delete m_pText;
}

QString KviRegisteredUsersDialogItem::key(int,bool) const
{
	return m_pUser->name();
}

void KviRegisteredUsersDialogItem::paintCell(QPainter * p,const QColorGroup &cg,int column,int width,int align)
{
	KviTalListViewItem::paintCell(p,cg,column,width,align);
	if(column==0)
	{
		if (isSelected())
		{
			QColor col(listView()->palette().highlight());
			col.setAlpha(127);
			p->setBrush(col);
			p->drawRect(0, 0, listView()->visibleWidth(), height());
		}
		p->drawPixmap(LVI_BORDER,LVI_BORDER, *(g_pIconManager->getBigIcon(QString(KVI_BIGICON_REGUSERS))) );
		int afterIcon = LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING;
		int www = listView()->visibleWidth() - (afterIcon + LVI_BORDER);
	p->translate(afterIcon,LVI_BORDER);
	m_pText->setPageSize(QSizeF(www,height() - (LVI_BORDER * 2)));
	m_pText->drawContents(p);
	} else {
		if(m_pUser)
		{
			if(!m_pUser->getProperty("notify").isEmpty())
				p->drawPixmap(LVI_BORDER,LVI_BORDER,*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NOTIFYONLINE)));
			if(m_pUser->ignoreEnagled())
				p->drawPixmap(LVI_BORDER,2*LVI_BORDER+16,*(g_pIconManager->getSmallIcon(KVI_SMALLICON_IGNORE)));
		}
	}
}

void KviRegisteredUsersDialogItem::setup()
{
	KviTalListViewItem::setup();
	int iWidth = listView()->visibleWidth();
	if(iWidth < LVI_MINIMUM_CELL_WIDTH)iWidth = LVI_MINIMUM_CELL_WIDTH;
	iWidth -= LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING + LVI_BORDER;
	//m_pText->setWidth(iWidth);
	//int iHeight = m_pText->height() + (2 * LVI_BORDER);
	int iHeight = m_pText->size().height() + (2 * LVI_BORDER);
	if(iHeight < (LVI_ICON_SIZE + (2 * LVI_BORDER)))iHeight = LVI_ICON_SIZE + (2 * LVI_BORDER);
	setHeight(iHeight+2);
}


KviRegisteredUsersDialog::KviRegisteredUsersDialog(QWidget * par)
: QWidget(par)
{
	g_pRegisteredUsersDialog = this;

	g_pLocalRegisteredUserDataBase = new KviRegisteredUserDataBase();
	g_pLocalRegisteredUserDataBase->copyFrom(g_pRegisteredUserDataBase);

	setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_REGUSERS)));
	setWindowTitle(__tr2qs("Registered Users - KVIrc"));

	QGridLayout * g = new QGridLayout(this);


	m_pListView = new KviTalListView(this);

	m_pListView->addColumn(__tr2qs("Name"),200);
	m_pListView->addColumn(__tr2qs("Flags"),20);

	m_pListView->setAllColumnsShowFocus(true);

	m_pListView->setSelectionMode(Q3ListView::Extended);
	m_pListView->setRootIsDecorated(TRUE);

	QToolTip::add(m_pListView,__tr2qs("<center>This is the list of registered users. " \
		"KVIrc can automatically recognize and associate properties to them.<br>" \
		"Use the buttons on the right to add, edit and remove entries. " \
		"The \"notify\" column allows you to quickly add users to the notify list. " \
		"Notify list fine-tuning can be performed by editing the entry properties.</center>"));

	connect(m_pListView,SIGNAL(pressed(KviTalListViewItem *,const QPoint &,int)),this,SLOT(itemPressed(KviTalListViewItem *,const QPoint &,int)));
	connect(m_pListView,SIGNAL(doubleClicked(KviTalListViewItem *)),this,SLOT(itemDoubleClicked(KviTalListViewItem *)));

	g->addMultiCellWidget(m_pListView,0,1,0,1);

	KviTalVBox * vbox = new KviTalVBox(this);
	vbox->setSpacing(4);
	g->addWidget(vbox,0,2);

	m_pWizardAddButton = new QPushButton(__tr2qs("Add (Wizard)..."),vbox);
	connect(m_pWizardAddButton,SIGNAL(clicked()),this,SLOT(addWizardClicked()));
	QToolTip::add(m_pWizardAddButton,__tr2qs("Add a registered user by means of a user-friendly wizard."));
	m_pWizardAddButton->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NEWITEMBYWIZARD)));


	m_pAddButton = new QPushButton(__tr2qs("&Add..."),vbox);
	connect(m_pAddButton,SIGNAL(clicked()),this,SLOT(addClicked()));
	QToolTip::add(m_pAddButton,__tr2qs("Open the edit dialog to create a new user entry."));
	m_pAddButton->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NEWITEM)));
	
	m_pAddGroupButton = new QPushButton(__tr2qs("&Add Group..."),vbox);
	connect(m_pAddGroupButton,SIGNAL(clicked()),this,SLOT(addGroupClicked()));
	QToolTip::add(m_pAddGroupButton,__tr2qs("Adds a new group"));
	m_pAddGroupButton->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_NEWITEM)));

	m_pRemoveButton = new QPushButton(__tr2qs("Re&move"),vbox);
	connect(m_pRemoveButton,SIGNAL(clicked()),this,SLOT(removeClicked()));
	m_pRemoveButton->setEnabled(false);
	QToolTip::add(m_pRemoveButton,__tr2qs("Remove the currently selected entries."));
	m_pRemoveButton->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DELETEITEM)));


	m_pEditButton = new QPushButton(__tr2qs("&Edit..."),vbox);
	connect(m_pEditButton,SIGNAL(clicked()),this,SLOT(editClicked()));
	m_pEditButton->setEnabled(false);
	QToolTip::add(m_pEditButton,__tr2qs("Edit the first selected entry."));
	m_pEditButton->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_EDITITEM)));

	QFrame * f = new QFrame(vbox);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);

	m_pExportButton = new QPushButton(__tr("Export To..."),vbox);
	m_pExportButton->setEnabled(false);
	connect(m_pExportButton,SIGNAL(clicked()),this,SLOT(exportClicked()));
	QToolTip::add(m_pExportButton,__tr2qs("Export the selected entries to a file.<br>All the data associated with the selected registered users will be exported.<br>You (or anyone else) can later import the entries by using the \"Import\" button."));
	m_pExportButton->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_FLOPPY)));


	m_pImportButton = new QPushButton(__tr("Import From..."),vbox);
	connect(m_pImportButton,SIGNAL(clicked()),this,SLOT(importClicked()));
	QToolTip::add(m_pImportButton,__tr2qs("Import entries from a file exported earlier by the \"export\" function of this dialog."));
	m_pImportButton->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_FOLDER)));


	KviTalHBox * hbox = new KviTalHBox(this);
	hbox->setSpacing(4);
	g->addMultiCellWidget(hbox,3,3,1,2);

	QPushButton * b;


	b = new QPushButton(__tr2qs("&OK"),hbox);
	connect(b,SIGNAL(clicked()),this,SLOT(okClicked()));
	b->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ACCEPT)));
	//b->setMinimumWidth(120);

	b = new QPushButton(__tr2qs("Cancel"),hbox);
	connect(b,SIGNAL(clicked()),this,SLOT(cancelClicked()));
	b->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_DISCARD)));
	//b->setMinimumWidth(120);

	g->addRowSpacing(2,15);
	g->setColStretch(0,1);
	g->setRowStretch(1,1);

	connect(m_pListView,SIGNAL(selectionChanged()),this,SLOT(selectionChanged()));
	connect(m_pListView,SIGNAL(rightButtonClicked ( KviTalListViewItem *, const QPoint &, int ) ),this,SLOT(listViewRightButtonClicked ( KviTalListViewItem *, const QPoint &, int )));

	fillList();

	if(!parent())
	{
		if(KVI_OPTION_RECT(KviOption_rectRegisteredUsersDialogGeometry).y() < 5)
		{
			KVI_OPTION_RECT(KviOption_rectRegisteredUsersDialogGeometry).setY(5);
		}
		//setGeometry(KVI_OPTION_RECT(KviOption_rectRegisteredUsersDialogGeometry));
		resize(KVI_OPTION_RECT(KviOption_rectRegisteredUsersDialogGeometry).width(),
			KVI_OPTION_RECT(KviOption_rectRegisteredUsersDialogGeometry).height());
		move(KVI_OPTION_RECT(KviOption_rectRegisteredUsersDialogGeometry).x(),
			KVI_OPTION_RECT(KviOption_rectRegisteredUsersDialogGeometry).y());
	}
}

KviRegisteredUsersDialog::~KviRegisteredUsersDialog()
{
#ifndef COMPILE_ON_MAC
	if(!parent())KVI_OPTION_RECT(KviOption_rectRegisteredUsersDialogGeometry) = QRect(pos().x(),pos().y(),size().width(),size().height());
#else
	if(!parent())KVI_OPTION_RECT(KviOption_rectRegisteredUsersDialogGeometry) = geometry();
#endif

	g_pRegisteredUsersDialog = 0;

	delete g_pLocalRegisteredUserDataBase;
	g_pLocalRegisteredUserDataBase = 0;
}

void KviRegisteredUsersDialog::itemPressed(KviTalListViewItem *it,const QPoint &pnt,int c)
{
	if(!it)return;
	KviRegisteredUsersDialogItemBase* b=(KviRegisteredUsersDialogItemBase*)it;
	if(b->type()==KviRegisteredUsersDialogItemBase::User)
	{
		KviRegisteredUsersDialogItem *i = (KviRegisteredUsersDialogItem *)it;
	
		QRect r = m_pListView->itemRect(i);
		int daw = m_pListView->columnWidth(0);
	
		QPoint ppp = m_pListView->mapFromGlobal(pnt);
	
		if((c == 1) && (ppp.x() < (r.height() + 5 + daw)))
		{
			// notify list toggle
			if(i->user()->getProperty("notify").isEmpty())
			{
				i->user()->setProperty("notify",""); // kill that
			} else {
				// try to find the nicknames to be notified
				QString szMask;
	
				for(KviIrcMask * m = i->user()->maskList()->first();m;m = i->user()->maskList()->next())
				{
					QString tmp = m->nick();
					if((tmp.indexOf('*') == -1) && (tmp.indexOf('?') == -1) && (!tmp.isEmpty()))
					{
						if(!szMask.isEmpty())szMask.append(' ');
						szMask.append(tmp);
					}
				}
				// if the nickname list is still empty , build a dummy nick to notify
				szMask = i->user()->name();
				szMask.replace(" ","");
				szMask.replace("'","");
				szMask.replace("&","");
				szMask.replace(",","");
	
				i->user()->setProperty("notify",szMask);
			}
			m_pListView->update();
		}
	}
}

void KviRegisteredUsersDialog::itemDoubleClicked(KviTalListViewItem *it)
{
	if(!it)return;
	KviRegisteredUsersDialogItemBase* b=(KviRegisteredUsersDialogItemBase*)it;
	if(b->type()==KviRegisteredUsersDialogItemBase::User)
	{
		editItem((KviRegisteredUsersDialogItem *)it);
	} else	{
		KviRegisteredUsersGroupItem *i = (KviRegisteredUsersGroupItem *)b;
		editGroup(i->group());
	}
}

void KviRegisteredUsersDialog::addGroupClicked()
{
	bool ok;
	QString text = QInputDialog::getText(
		"KVIrc", __tr("Group name:"), QLineEdit::Normal,
		QString::null, &ok, this );
	if ( ok && !text.isEmpty() ) {
		g_pLocalRegisteredUserDataBase->addGroup(text);
		fillList();
	}
}

void KviRegisteredUsersDialog::editGroup(KviRegisteredUserGroup* group)
{
	bool ok;
	
	QString text = QInputDialog::getText(
		"KVIrc", __tr("Group name:"), QLineEdit::Normal,
		group->name(), &ok, this );
	if ( ok && !text.isEmpty() ) {
		QString szOldGroup=group->name();
		g_pLocalRegisteredUserDataBase->groupDict()->setAutoDelete(0);
		g_pLocalRegisteredUserDataBase->groupDict()->remove(szOldGroup);
		g_pLocalRegisteredUserDataBase->groupDict()->setAutoDelete(1);
		group->setName(text);
		g_pLocalRegisteredUserDataBase->groupDict()->insert(text,group);
		
		KviPointerHashTable<QString,KviRegisteredUser> * d = g_pLocalRegisteredUserDataBase->userDict();
	
		for(KviRegisteredUser * u = d->first();u;u = d->next())
		{
			if(u->group()==szOldGroup)
				u->setGroup(text);
		}
		
		fillList();
	}
}

void KviRegisteredUsersDialog::listViewRightButtonClicked ( KviTalListViewItem * pItem, const QPoint & point, int )
{
	if(pItem)
	{
		KviRegisteredUsersDialogItemBase* b=(KviRegisteredUsersDialogItemBase*)pItem;
		if(b->type()==KviRegisteredUsersDialogItemBase::User)
		{
			KviTalPopupMenu *groups = new KviTalPopupMenu;
			
			KviPointerHashTable<QString,KviRegisteredUserGroup> * pGroups = g_pLocalRegisteredUserDataBase->groupDict();
			m_TmpDict.clear();
			for(KviPointerHashTableEntry<QString,KviRegisteredUserGroup> * g = pGroups->firstEntry();g;g = pGroups->nextEntry())
			{
				int id=groups->insertItem(g->key());
				m_TmpDict.replace(id,g->data());
			}
			
			connect(groups,SIGNAL(activated ( int )),this,SLOT(moveToGroupMenuClicked(int)));
			
			KviTalPopupMenu *mainPopup = new KviTalPopupMenu;
			mainPopup->insertItem(__tr("Move to group"),groups);
			mainPopup->exec(point);
		}
	}
}

void KviRegisteredUsersDialog::moveToGroupMenuClicked(int id)
{
	QString szGroup=m_TmpDict.find(id)->name();
	KviTalListViewItemIterator it( m_pListView,  KviTalListViewItemIterator::Selected );
	while ( it.current() ) {
		KviRegisteredUsersDialogItemBase* b=(KviRegisteredUsersDialogItemBase*)(it.current());
		if(b->type()==KviRegisteredUsersDialogItemBase::User)
		{
			((KviRegisteredUsersDialogItem *)(it.current()))->user()->setGroup(szGroup);
		} 
		++it;
	}
	fillList();
}

void KviRegisteredUsersDialog::fillList()
{
	m_pListView->clear();
	KviPointerHashTable<QString,KviRegisteredUsersGroupItem> groupItems(5,false);
	groupItems.setAutoDelete(false);
	
	KviPointerHashTable<QString,KviRegisteredUserGroup> * pGroups = g_pLocalRegisteredUserDataBase->groupDict();
	for(KviRegisteredUserGroup * g = pGroups->first();g;g = pGroups->next())
	{
		KviRegisteredUsersGroupItem* pCur = new KviRegisteredUsersGroupItem(m_pListView,g);
		groupItems.insert(g->name(),pCur);
		pCur->setOpen(TRUE);
	}
	
	KviPointerHashTable<QString,KviRegisteredUser> * d = g_pLocalRegisteredUserDataBase->userDict();
	KviRegisteredUsersDialogItem * item;

	for(KviRegisteredUser * u = d->first();u;u = d->next())
	{
		if(u->group().isEmpty())
			u->setGroup(__tr("Default"));
		if(groupItems.find(u->group()))
			item = new KviRegisteredUsersDialogItem(groupItems.find(u->group()),u);
		else if(groupItems.find(__tr("Default")))
			item = new KviRegisteredUsersDialogItem(groupItems.find(__tr("Default")),u);
		else { //should never be called
			KviRegisteredUserGroup* pGroup = g_pLocalRegisteredUserDataBase->addGroup(__tr("Default"));
			KviRegisteredUsersGroupItem* pCur = new KviRegisteredUsersGroupItem(m_pListView,pGroup);
			groupItems.insert(__tr("Default"),pCur);
			item = new KviRegisteredUsersDialogItem(pCur,u);
		}
	}
	if(m_pListView->firstChild())
	{
		m_pListView->setSelected(m_pListView->firstChild(),true);
		m_pListView->setCurrentItem(m_pListView->firstChild());
	}
}

void KviRegisteredUsersDialog::closeEvent(QCloseEvent *e)
{
	m_pListView->clear();
	e->accept();
	okClicked();
	//delete this;
}

void KviRegisteredUsersDialog::okClicked()
{
	m_pListView->clear();
	g_pRegisteredUserDataBase->copyFrom(g_pLocalRegisteredUserDataBase);
	g_pApp->restartNotifyLists();
	delete this;
}

void KviRegisteredUsersDialog::cancelClicked()
{
	m_pListView->clear();
	delete this;
}

void KviRegisteredUsersDialog::addClicked()
{
	KviRegisteredUserEntryDialog * dlg = new KviRegisteredUserEntryDialog(this,0);
	int ret = dlg->exec();
	delete dlg;

	if(!g_pRegisteredUsersDialog)return; // we have been deleted!

	if(ret == QDialog::Accepted)
	{
		fillList();
	}
}

void KviRegisteredUsersDialog::addWizardClicked()
{
	KviRegistrationWizard * w = new KviRegistrationWizard("",g_pLocalRegisteredUserDataBase,this,true);
	int ret = w->exec();
	delete w;
	if(!g_pRegisteredUsersDialog)return; // we have been deleted!
	if(ret == QDialog::Accepted)
	{
		fillList();
	}
}

void KviRegisteredUsersDialog::removeClicked()
{
	KviTalListViewItemIterator it( m_pListView,  KviTalListViewItemIterator::Selected );
	while ( it.current() ) {
		KviRegisteredUsersDialogItemBase* b=(KviRegisteredUsersDialogItemBase*)(it.current());
		if(b->type()==KviRegisteredUsersDialogItemBase::User)
		{
			g_pLocalRegisteredUserDataBase->removeUser(((KviRegisteredUsersDialogItem *)(it.current()))->user()->name());
		} else {
			g_pLocalRegisteredUserDataBase->removeGroup(((KviRegisteredUsersGroupItem *)(it.current()))->group()->name());
		}
		++it;
	}
	fillList();
// 	KviRegisteredUsersDialogItem *it = (KviRegisteredUsersDialogItem *)m_pListView->firstChild();
// 	KviRegisteredUsersDialogItemBase* b=(KviRegisteredUsersDialogItemBase*)it;
// 	if(b->type()==KviRegisteredUsersDialogItemBase::User)
// 	{
// 		KviPointerList<KviRegisteredUsersDialogItem> l;
// 		l.setAutoDelete(false);
// 		while(it)
// 		{
// 			if(it->isSelected())l.append(it);
// 			it = (KviRegisteredUsersDialogItem *)it->nextSibling();
// 		}
// 	
// 		for(KviRegisteredUsersDialogItem * i = l.first();i;i = l.next())
// 		{
// 			//g_pLocalRegisteredUserDataBase->removeUser(i->user()->name());
// 			delete i;
// 		}
// 	} else {
// 		
// 	}
}

void KviRegisteredUsersDialog::editClicked()
{
	KviRegisteredUsersDialogItemBase* b=(KviRegisteredUsersDialogItemBase*)(m_pListView->currentItem());
	if(!b)return;
	if(b->type()==KviRegisteredUsersDialogItemBase::User)
	{
		KviRegisteredUsersDialogItem *i = (KviRegisteredUsersDialogItem *)b;
		editItem(i);
	} else {
		KviRegisteredUsersGroupItem *i = (KviRegisteredUsersGroupItem *)b;
		editGroup(i->group());
	}
}

void KviRegisteredUsersDialog::editItem(KviRegisteredUsersDialogItem * i)
{
	KviRegisteredUser * u = i->user();

	i->setUser(0);

	QString szName = u->name();

	KviRegisteredUserEntryDialog * dlg = new KviRegisteredUserEntryDialog(this,u);
	int res = dlg->exec();
	delete dlg;

	if(!g_pRegisteredUsersDialog)return; // we have been deleted!

	if(res == QDialog::Accepted)
	{
		fillList();

		// select the last edited item
		KviTalListViewItem * i = m_pListView->firstChild();
		while(i)
		{
			QString szTmp = i->text(0);
			if(KviQString::equalCI(szTmp,szName))
			{
				m_pListView->setSelected(i,true);
				m_pListView->setCurrentItem(i);
				break;
			}
			i = i->nextSibling();
		}

		return;
	}

	i->setUser(u);
	m_pListView->update();
}

void KviRegisteredUsersDialog::selectionChanged()
{
	bool bHaveSelected = !m_pListView->selectedItem();

	m_pEditButton->setEnabled(bHaveSelected);
	m_pRemoveButton->setEnabled(bHaveSelected);
	m_pExportButton->setEnabled(bHaveSelected);
}


#define KVI_REGUSER_DB_FILE_MAGIC 0x5334DBDB
#define KVI_REGUSER_DB_FILE_VERSION 1

typedef struct _KviReguserDbFileHeader
{
	unsigned int magic;
	unsigned int version;
	unsigned int nentries;
} KviReguserDbFileHeader;

void KviRegisteredUsersDialog::exportClicked()
{
	unsigned int nEntries = 0;

	KviTalListViewItemIterator it( m_pListView, KviTalListViewItemIterator::Selected );
	KviTalListViewItemIterator cit( m_pListView, KviTalListViewItemIterator::Selected );
	while ( cit.current() ) {
		if(((KviRegisteredUsersDialogItemBase *)(cit.current()))->type() == KviRegisteredUsersDialogItemBase::User)
			nEntries++;
		++cit;
	}

	if(nEntries < 1)
	{
		KviMessageBox::warning(__tr("No entries selected."));
		return;
	}

	QString buffer;

	if(!KviFileDialog::askForSaveFileName(buffer,__tr("Choose a Filename - KVIrc"),0,0,true,true))return;

	if(!g_pRegisteredUsersDialog)return; // we have been deleted!

	KviFile f(buffer);
	if(!f.open(IO_WriteOnly | IO_Truncate))
	{
		KviMessageBox::warning(__tr("Can't open file %Q for writing."),&buffer);
		return;
	}

	KviReguserDbFileHeader hf;
	hf.magic = KVI_REGUSER_DB_FILE_MAGIC;
	hf.version = KVI_REGUSER_DB_FILE_VERSION;
	hf.nentries = nEntries;

	if(f.writeBlock((const char *)&hf,sizeof(KviReguserDbFileHeader)) != sizeof(KviReguserDbFileHeader))goto write_error;

	while ( it.current() ) {
		KviRegisteredUsersDialogItemBase *pBase = (KviRegisteredUsersDialogItemBase *)(it.current());
		if(pBase->type()!=KviRegisteredUsersDialogItemBase::User) continue;
		QString szName = it.current()->text(0);
		KviRegisteredUser * u = ((KviRegisteredUsersDialogItem *)(it.current()))->user();
		if(u)
		{
			if(!f.save(szName))goto write_error;
			KviPointerHashTable<QString,QString> * pd = u->propertyDict();
			if(pd)
			{
				if(!f.save(pd->count()))goto write_error;
				for(KviPointerHashTableEntry<QString,QString> * pCur = pd->firstEntry();pCur;pCur = pd->nextEntry())
				{
					QString key = pCur->key();
					if(!f.save(key))goto write_error;
					if(!f.save(*(pCur->data())))goto write_error;
				}
			} else {
				if(!f.save(0))goto write_error;
			}

			KviPointerList<KviIrcMask> * ml = u->maskList();
			if(ml)
			{
				if(!f.save(ml->count()))goto write_error;
				for(KviIrcMask * m = ml->first();m;m = ml->next())
				{
					QString fullMask;
					m->mask(fullMask,KviIrcMask::NickUserHost);
					if(!f.save(fullMask))goto write_error;
				}
			} else {
				if(!f.save(0))goto write_error;
			}

			QString avatar;
			if(u->getProperty("avatar",avatar))
			{
				KviAvatar * av = g_pIconManager->getAvatar(QString::null,avatar);
				if(av)
				{
					if(!av->pixmap()->isNull())
					{
						if(!f.save(1))goto write_error;
						QImageWriter io;
						io.setDevice(&f);
						io.setFormat("PNG");						
						if(!io.write(av->pixmap()->convertToImage()))goto write_error;
					} else {
						if(!f.save(0))goto write_error;
					}
				} else {
					if(!f.save(0))goto write_error;
				}
			} else {
				if(!f.save(0))goto write_error;
			}
		}
		++it;
	}

	goto succesfull_export;


write_error:
	KviMessageBox::warning(__tr("Can't export the registered users database: Write error."));
	f.close();
	return;

succesfull_export:

	f.close();
}

void KviRegisteredUsersDialog::importClicked()
{
	//KviStr buffer;
	QString buffer;

	if(!KviFileDialog::askForOpenFileName(buffer,__tr("Choose a Filename - KVIrc")))return;

	if(!g_pRegisteredUsersDialog)return; // we have been deleted!

	KviFile f(buffer);
	if(!f.open(IO_ReadOnly))
	{
		KviMessageBox::warning(__tr2qs("Can't open file %s for reading."),&buffer);
		return;
	}

	KviReguserDbFileHeader hf;
	unsigned int idx;

	if(f.readBlock((char *)&hf,sizeof(KviReguserDbFileHeader)) != sizeof(KviReguserDbFileHeader))goto read_error;

	if(hf.magic != KVI_REGUSER_DB_FILE_MAGIC)
	{
		KviMessageBox::warning(__tr2qs("The file %s doesn't appear to be a valid registered users database."),&buffer);
		f.close();
		return;
	}

	if(hf.version != KVI_REGUSER_DB_FILE_VERSION)
	{
		KviMessageBox::warning(__tr2qs("The file %s contains an invalid registered users database version."),&buffer);
		f.close();
		return;
	}

	for(idx = 0;idx < hf.nentries;idx++)
	{
		QString szName;
		if(!f.load(szName))goto read_error;
		KviRegisteredUser * u = g_pLocalRegisteredUserDataBase->getUser(szName);
		unsigned int count = 0;
		if(!f.load(count))goto read_error;
		for(unsigned int up = 0;up < count;up++)
		{
			QString szKey,szValue;
			if(!f.load(szKey))goto read_error;
			if(!f.load(szValue))goto read_error;
			u->setProperty(szKey,szValue);
		}
		if(!f.load(count))goto read_error;
		for(unsigned int um = 0;um < count;um++)
		{
			QString szMask;
			if(!f.load(szMask))goto read_error;
			if(!szMask.isEmpty())
			{
				KviIrcMask * m = new KviIrcMask(szMask);
				g_pLocalRegisteredUserDataBase->addMask(u,m);
			}
		}
		if(!f.load(count))goto read_error;
		if(count)
		{
			// there is an avatar
			QImage img;
			QImageReader io;
			io.setDevice(&f);
			io.setFormat("PNG");
			img=io.read();
//			if(io.read())goto read_error;

			if(img.isNull())debug("Ops.. readed a null image ?");

			KviStr fName = u->name();
			kvi_encodeFileName(fName);

			KviStr fPath;
			int rnm = 0 ;
			do
			{
				g_pApp->getLocalKvircDirectory(fPath,KviApp::Avatars,fName.ptr());
				fPath.append(KviStr::Format,"%d.png",rnm);
				rnm++;
			} while(KviFileUtils::fileExists(fPath.ptr()));

			if(!img.save(fPath.ptr(),"PNG"))
			{
				debug("Can't save image %s",fPath.ptr());
			} else {
				u->setProperty("avatar",fPath.ptr());
			}
		}
	}

	goto succesfull_import;


read_error:
	KviMessageBox::warning(__tr("Can't import the registered users database: Read error."));
	f.close();
	return;

succesfull_import:

	f.close();
	fillList();
}


