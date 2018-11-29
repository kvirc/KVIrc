//=============================================================================
//
//   File : RegisteredUsersDialog.cpp
//   Creation date : Tue Dec 26 2000 12:24:12 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#define _WANT_OPTION_FLAGS_

#include "RegisteredUserEntryDialog.h"
#include "RegistrationWizard.h"
#include "RegisteredUsersDialog.h"

#include "KviRegisteredUserDataBase.h"
#include "KviLocale.h"
#include "KviIrcMask.h"
#include "kvi_debug.h"
#include "KviMainWindow.h"
#include "KviIconManager.h"
#include "KviApplication.h"
#include "KviOptions.h"
#include "KviFile.h"
#include "KviFileDialog.h"
#include "KviMessageBox.h"
#include "KviFileUtils.h"
#include "kvi_settings.h"
#include "KviStringConversion.h"
#include "KviOptions.h"
#include "KviPointerHashTable.h"
#include "KviTalHBox.h"
#include "KviTalVBox.h"

#include <QLayout>
#include <QLabel>
#include <QImageWriter>
#include <QImageReader>
#include <QImage>
#include <QString>
#include <QDesktopWidget>
#include <QComboBox>
#include <QToolTip>
#include <QStyle>
#include <QPainter>
#include <QInputDialog>
#include <QEvent>
#include <QCloseEvent>
#include <QAbstractItemView>
#include <QAbstractTextDocumentLayout>
#include <QShortcut>

#include <memory>

#define LVI_ICON_SIZE 32
#define LVI_BORDER 4
#define LVI_SPACING 8
#define LVI_MINIMUM_TEXT_WIDTH 300
#define LVI_MINIMUM_CELL_WIDTH (LVI_MINIMUM_TEXT_WIDTH + LVI_BORDER + LVI_ICON_SIZE + LVI_SPACING + LVI_BORDER)

extern RegisteredUsersDialog * g_pRegisteredUsersDialog;

KviRegisteredUsersListView::KviRegisteredUsersListView(QWidget * par)
    : QTreeWidget(par)
{
	setColumnCount(2);

	QStringList labels;
	labels << __tr2qs_ctx("Name", "register") << __tr2qs_ctx("Flags", "register");
	setHeaderLabels(labels);
	setColumnWidth(0, 360);
	setColumnWidth(1, 20);

	setAllColumnsShowFocus(true);

	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setRootIsDecorated(true);
	setSortingEnabled(true);

	setToolTip(__tr2qs_ctx("This is the list of registered users. "
	                       "KVIrc can automatically recognize and associate properties to them.<br>"
	                       "Use the buttons on the right to add, edit and remove entries. "
	                       "The \"notify\" column allows you to quickly add users to the notify list. "
	                       "Notify list fine-tuning can be performed by editing the entry properties.", "register"));
}

void KviRegisteredUsersListView::mousePressEvent(QMouseEvent * e)
{
	if(e->button() == Qt::RightButton)
	{
		QTreeWidgetItem * i = itemAt(e->pos());
		if(i)
			emit rightButtonPressed(i, QCursor::pos());
	}
	QTreeWidget::mousePressEvent(e);
}

RegisteredUsersDialogItem::RegisteredUsersDialogItem(QTreeWidgetItem * par, KviRegisteredUser * u)
    : RegisteredUsersDialogItemBase(User, par), m_pUser(u)
{
	QString szTmp;
	QString t = "<b>";
	t += u->name();
	t += "</b> [";
	szTmp = m_pUser->getProperty("notify");
	if(szTmp.isEmpty())
	{
		t += __tr2qs_ctx("Notify disabled", "register");
	}
	else
	{
		t += __tr2qs_ctx("Notify as:", "register");
		t += " ";
		t += szTmp;
	}
	t += "]";
	t += "<br><font size=\"-1\">";
	szTmp = m_pUser->getProperty("comment");
	if(szTmp.isEmpty())
	{
		t += __tr2qs_ctx("No comment set", "register");
	}
	else
	{
		t += __tr2qs_ctx("Comment:", "register");
		t += " ";
		t += m_pUser->getProperty("comment");
	}
	t += "</font>";
	m_pText.setHtml(t);
	m_pText.setTextWidth(-1);
	// 	setText(0,t);
}

RegisteredUsersDialogItem::~RegisteredUsersDialogItem()
    = default;

QString RegisteredUsersDialogItem::key(int, bool) const
{
	return m_pUser->name();
}

void RegisteredUsersDialogItemDelegate::paint(QPainter * p, const QStyleOptionViewItem & option, const QModelIndex & index) const
{

	RegisteredUsersDialogItemBase * item = static_cast<RegisteredUsersDialogItemBase *>(index.internalPointer());

	if(item->type() == RegisteredUsersDialogItemBase::Group)
	{
		//groups
		QStyledItemDelegate::paint(p, option, index);
	}
	else
	{
		QStyleOptionViewItem opt = option;
		initStyleOption(&opt, index);

		if(opt.state & QStyle::State_Selected)
			QApplication::style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, p);

		RegisteredUsersDialogItem * it = (RegisteredUsersDialogItem *)item;

		if(index.column() == 0)
		{
			QPoint pt = opt.rect.topLeft();
			pt.setX(pt.x() + LVI_BORDER);
			pt.setY(pt.y() + LVI_BORDER);

			p->drawPixmap(pt, *(g_pIconManager->getBigIcon(KVI_BIGICON_REGUSERS)));
			QRect cliprect = QRect(QPoint(0, 0), QSize(opt.rect.width() - (LVI_ICON_SIZE + LVI_SPACING + LVI_BORDER), opt.rect.height()));
			p->save();

			p->translate(opt.rect.x() + LVI_ICON_SIZE + LVI_SPACING, opt.rect.y());
			it->m_pText.drawContents(p, cliprect);
			p->restore();
		}
		else
		{
			if(it->user())
			{
				if(!it->user()->getProperty("notify").isEmpty())
					p->drawPixmap(opt.rect.topLeft().x() + LVI_BORDER, opt.rect.topLeft().y() + LVI_BORDER, *(g_pIconManager->getSmallIcon(KviIconManager::NotifyOnLine)));
				else
					p->drawPixmap(opt.rect.topLeft().x() + LVI_BORDER, opt.rect.topLeft().y() + LVI_BORDER, *(g_pIconManager->getSmallIcon(KviIconManager::NotifyOffLine)));
				if(it->user()->ignoreEnabled())
					p->drawPixmap(opt.rect.topLeft().x() + LVI_BORDER, opt.rect.topLeft().y() + 2 * LVI_BORDER + 16, *(g_pIconManager->getSmallIcon(KviIconManager::Ignore)));
			}
		}
	}
}

QSize RegisteredUsersDialogItemDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	RegisteredUsersDialogItemBase * item = static_cast<RegisteredUsersDialogItemBase *>(index.internalPointer());

	if(item->type() == RegisteredUsersDialogItemBase::Group)
	{
		//groups
		return QStyledItemDelegate::sizeHint(option, index);
	}
	else
	{
		//users
		// 		RegisteredUsersDialogItem *it=(RegisteredUsersDialogItem*)item;

		return { 300, LVI_ICON_SIZE + 2 * LVI_BORDER };
	}
}

RegisteredUsersDialog::RegisteredUsersDialog(QWidget * par)
    : QWidget(par)
{
	g_pRegisteredUsersDialog = this;

	g_pLocalRegisteredUserDataBase = new KviRegisteredUserDataBase();
	g_pLocalRegisteredUserDataBase->copyFrom(g_pRegisteredUserDataBase);

	setWindowIcon(*(g_pIconManager->getSmallIcon(KviIconManager::RegUsers)));
	setWindowTitle(__tr2qs_ctx("Registered Users and Configuration - KVIrc", "register"));

	QGridLayout * g = new QGridLayout(this);

	m_pListView = new KviRegisteredUsersListView(this);
	m_pListView->setItemDelegate(new RegisteredUsersDialogItemDelegate());

	connect(m_pListView, SIGNAL(itemPressed(QTreeWidgetItem *, int)), this, SLOT(itemPressed(QTreeWidgetItem *, int)));
	connect(m_pListView, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem *, int)));

	g->addWidget(m_pListView, 0, 0, 2, 2);

	KviTalVBox * vbox = new KviTalVBox(this);
	vbox->setSpacing(4);
	g->addWidget(vbox, 0, 2);

	m_pWizardAddButton = new QPushButton(__tr2qs_ctx("Add Wizard...", "register"), vbox);
	connect(m_pWizardAddButton, SIGNAL(clicked()), this, SLOT(addWizardClicked()));
	m_pWizardAddButton->setToolTip(__tr2qs_ctx("Add a registered user by means of a user-friendly wizard.", "register"));
	//m_pWizardAddButton->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::NewItemByWizard)));

	m_pAddButton = new QPushButton(__tr2qs_ctx("&Add...", "register"), vbox);
	connect(m_pAddButton, SIGNAL(clicked()), this, SLOT(addClicked()));
	m_pAddButton->setToolTip(__tr2qs_ctx("Open the edit dialog to create a new user entry.", "register"));
	//m_pAddButton->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::NewItem)));

	m_pAddGroupButton = new QPushButton(__tr2qs_ctx("&Add Group...", "register"), vbox);
	connect(m_pAddGroupButton, SIGNAL(clicked()), this, SLOT(addGroupClicked()));
	m_pAddGroupButton->setToolTip(__tr2qs_ctx("Adds a new group", "register"));
	//m_pAddGroupButton->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::NewItem)));

	m_pRemoveButton = new QPushButton(__tr2qs_ctx("Re&move", "register"), vbox);
	connect(m_pRemoveButton, SIGNAL(clicked()), this, SLOT(removeClicked()));
	m_pRemoveButton->setEnabled(false);
	m_pRemoveButton->setToolTip(__tr2qs_ctx("Remove the currently selected entries.", "register"));
	//m_pRemoveButton->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::DeleteItem)));

	m_pEditButton = new QPushButton(__tr2qs_ctx("&Edit...", "register"), vbox);
	connect(m_pEditButton, SIGNAL(clicked()), this, SLOT(editClicked()));
	m_pEditButton->setEnabled(false);
	m_pEditButton->setToolTip(__tr2qs_ctx("Edit the first selected entry.", "register"));
	//m_pEditButton->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::EditItem)));

	QFrame * f = new QFrame(vbox);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);

	m_pSelectAllButton = new QPushButton(__tr2qs_ctx("Select All", "register"), vbox);
	connect(m_pSelectAllButton, SIGNAL(clicked()), this, SLOT(selectAllClicked()));
	m_pSelectAllButton->setToolTip(__tr2qs_ctx("Select all the entries", "register"));
	//m_pSelectAllButton->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Plus)));

	m_pExportButton = new QPushButton(__tr2qs_ctx("Export to...", "register"), vbox);
	m_pExportButton->setEnabled(false);
	connect(m_pExportButton, SIGNAL(clicked()), this, SLOT(exportClicked()));
	m_pExportButton->setToolTip(__tr2qs_ctx("Export the selected entries to a file.<br>All the data associated with the selected registered users will be exported.<br>You (or anyone else) can later import the entries by using the \"Import\" button.", "register"));
	//m_pExportButton->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Save)));

	m_pImportButton = new QPushButton(__tr2qs_ctx("Import from...", "register"), vbox);
	connect(m_pImportButton, SIGNAL(clicked()), this, SLOT(importClicked()));
	m_pImportButton->setToolTip(__tr2qs_ctx("Import entries from a file exported earlier by the \"export\" function of this dialog.", "register"));
	//m_pImportButton->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Folder)));

	KviTalHBox * hbox = new KviTalHBox(this);
	hbox->setSpacing(4);
	g->addWidget(hbox, 3, 1, 1, 2);

	QPushButton * b;

	b = new QPushButton(__tr2qs_ctx("&OK", "register"), hbox);
	connect(b, SIGNAL(clicked()), this, SLOT(okClicked()));
	b->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Accept)));
	//b->setMinimumWidth(120);

	b = new QPushButton(__tr2qs_ctx("Cancel", "register"), hbox);
	connect(b, SIGNAL(clicked()), this, SLOT(cancelClicked()));
	b->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Discard)));
	//b->setMinimumWidth(120);

	g->addItem(new QSpacerItem(0, 15), 2, 0);
	g->setColumnStretch(0, 1);
	g->setRowStretch(1, 1);

	connect(m_pListView, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
	connect(m_pListView, SIGNAL(rightButtonPressed(QTreeWidgetItem *, QPoint)), this, SLOT(rightButtonPressed(QTreeWidgetItem *, QPoint)));

	new QShortcut(Qt::Key_Escape, this, SLOT(cancelClicked()));

	fillList();

	if(!parent())
	{
		if(KVI_OPTION_RECT(KviOption_rectRegisteredUsersDialogGeometry).y() < 5)
			KVI_OPTION_RECT(KviOption_rectRegisteredUsersDialogGeometry)
			    .setY(5);

		//setGeometry(KVI_OPTION_RECT(KviOption_rectRegisteredUsersDialogGeometry));
		resize(KVI_OPTION_RECT(KviOption_rectRegisteredUsersDialogGeometry).width(),
		    KVI_OPTION_RECT(KviOption_rectRegisteredUsersDialogGeometry).height());

		QRect rect = g_pApp->desktop()->screenGeometry(g_pMainWindow);
		move(rect.x() + ((rect.width() - KVI_OPTION_RECT(KviOption_rectRegisteredUsersDialogGeometry).width()) / 2), rect.y() + ((rect.height() - KVI_OPTION_RECT(KviOption_rectRegisteredUsersDialogGeometry).height()) / 2));
	}
}

RegisteredUsersDialog::~RegisteredUsersDialog()
{
#ifndef COMPILE_ON_MAC
	if(!parent())
		KVI_OPTION_RECT(KviOption_rectRegisteredUsersDialogGeometry) = QRect(pos().x(), pos().y(), size().width(), size().height());
#else
	if(!parent())
		KVI_OPTION_RECT(KviOption_rectRegisteredUsersDialogGeometry) = geometry();
#endif

	g_pRegisteredUsersDialog = nullptr;

	delete g_pLocalRegisteredUserDataBase;
	g_pLocalRegisteredUserDataBase = nullptr;
}

void RegisteredUsersDialog::itemPressed(QTreeWidgetItem * it, int c)
{
	if(!it)
		return;
	RegisteredUsersDialogItemBase * b = (RegisteredUsersDialogItemBase *)it;
	if(b->type() == RegisteredUsersDialogItemBase::User)
	{
		RegisteredUsersDialogItem * i = (RegisteredUsersDialogItem *)it;

		QRect r = m_pListView->visualItemRect(i);
		int daw = m_pListView->columnWidth(0);

		QPoint ppp = m_pListView->mapFromGlobal(QCursor::pos());

		if((c == 1) && (ppp.x() < (r.height() + 5 + daw)))
		{
			// notify list toggle
			if(i->user()->getProperty("notify").isEmpty())
			{
				// try to find the nicknames to be notified
				QString szMask;

				for(KviIrcMask * m = i->user()->maskList()->first(); m; m = i->user()->maskList()->next())
				{
					QString tmp = m->nick();
					if((tmp.indexOf('*') == -1) && (tmp.indexOf('?') == -1) && (!tmp.isEmpty()))
					{
						if(!szMask.isEmpty())
							szMask.append(' ');
						szMask.append(tmp);
					}
				}
				// if the nickname list is still empty, build a dummy nick to notify
				if(szMask.isEmpty())
				{
					szMask = i->user()->name();
					szMask.replace(" ", "");
					szMask.replace("'", "");
					szMask.replace("&", "");
					szMask.replace(",", "");
				}

				i->user()->setProperty("notify", szMask);
			}
			else
			{
				i->user()->setProperty("notify", QString("")); // kill that
			}

			m_pListView->repaint(r);
		}
	}
}

void RegisteredUsersDialog::itemDoubleClicked(QTreeWidgetItem * it, int)
{
	if(!it)
		return;
	RegisteredUsersDialogItemBase * b = (RegisteredUsersDialogItemBase *)it;
	if(b->type() == RegisteredUsersDialogItemBase::User)
	{
		editItem((RegisteredUsersDialogItem *)it);
	}
	else
	{
		RegisteredUsersGroupItem * i = (RegisteredUsersGroupItem *)b;
		editGroup(i->group());
	}
}

void RegisteredUsersDialog::addGroupClicked()
{
	bool ok;
	QString text = QInputDialog::getText(this, "Add Group Name - KVIrc",
	    __tr2qs_ctx("Group name:", "register"), QLineEdit::Normal, QString(), &ok);

	if(ok && !text.isEmpty())
	{
		g_pLocalRegisteredUserDataBase->addGroup(text);
		fillList();
	}
}

void RegisteredUsersDialog::editGroup(KviRegisteredUserGroup * group)
{
	bool ok;

	QString text = QInputDialog::getText(this, "Change Group Name - KVIrc",
	    __tr2qs_ctx("Group name:", "register"), QLineEdit::Normal, group->name(), &ok);

	if(ok && !text.isEmpty())
	{
		QString szOldGroup = group->name();
		g_pLocalRegisteredUserDataBase->groupDict()->setAutoDelete(false);
		g_pLocalRegisteredUserDataBase->groupDict()->remove(szOldGroup);
		g_pLocalRegisteredUserDataBase->groupDict()->setAutoDelete(true);
		group->setName(text);
		g_pLocalRegisteredUserDataBase->groupDict()->insert(text, group);

		KviPointerHashTable<QString, KviRegisteredUser> * d = g_pLocalRegisteredUserDataBase->userDict();

		for(KviRegisteredUser * u = d->first(); u; u = d->next())
		{
			if(u->group() == szOldGroup)
				u->setGroup(text);
		}

		fillList();

		// select the last edited item
		for(int c = 0; c < m_pListView->topLevelItemCount(); c++)
		{
			QTreeWidgetItem * pGroup = m_pListView->topLevelItem(c);
			if(KviQString::equalCI(pGroup->text(0), text))
			{
				pGroup->setSelected(true);
				m_pListView->setCurrentItem(pGroup);
				break;
			}
		}
	}
}

void RegisteredUsersDialog::rightButtonPressed(QTreeWidgetItem * pItem, QPoint pnt)
{
	if(pItem)
	{
		RegisteredUsersDialogItemBase * b = (RegisteredUsersDialogItemBase *)pItem;
		if(b->type() == RegisteredUsersDialogItemBase::User)
		{
			QMenu * groups = new QMenu;

			KviPointerHashTable<QString, KviRegisteredUserGroup> * pGroups = g_pLocalRegisteredUserDataBase->groupDict();
			for(KviPointerHashTableEntry<QString, KviRegisteredUserGroup> * g = pGroups->firstEntry(); g; g = pGroups->nextEntry())
			{
				groups->addAction(g->key())
				    ->setData(g->data()->name());
			}

			connect(groups, SIGNAL(triggered(QAction *)), this, SLOT(moveToGroupMenuClicked(QAction *)));

			QMenu * mainPopup = new QMenu;
			mainPopup->addAction(__tr2qs_ctx("Move to Group", "register"))->setMenu(groups);
			mainPopup->exec(pnt);
		}
	}
}

void RegisteredUsersDialog::moveToGroupMenuClicked(QAction * pAction)
{
	QString szGroup = pAction->data().toString();

	QList<QTreeWidgetItem *> list = m_pListView->selectedItems();
	for(int i = 0; i < list.count(); i++)
	{
		RegisteredUsersDialogItemBase * b = (RegisteredUsersDialogItemBase *)(list.at(i));
		if(b->type() == RegisteredUsersDialogItemBase::User)
		{
			((RegisteredUsersDialogItem *)list.at(i))->user()->setGroup(szGroup);
		}
	}
	fillList();
}

void RegisteredUsersDialog::fillList()
{
	m_pListView->clear();
	KviPointerHashTable<QString, RegisteredUsersGroupItem> groupItems(5, false);
	groupItems.setAutoDelete(false);

	KviPointerHashTable<QString, KviRegisteredUserGroup> * pGroups = g_pLocalRegisteredUserDataBase->groupDict();
	for(KviRegisteredUserGroup * g = pGroups->first(); g; g = pGroups->next())
	{
		RegisteredUsersGroupItem * pCur = new RegisteredUsersGroupItem(m_pListView, g);
		groupItems.insert(g->name(), pCur);
		pCur->setExpanded(true);
	}

	KviPointerHashTable<QString, KviRegisteredUser> * d = g_pLocalRegisteredUserDataBase->userDict();
	for(KviRegisteredUser * u = d->first(); u; u = d->next())
	{
		if(u->group().isEmpty())
			u->setGroup(__tr2qs_ctx("Default", "register"));
		if(groupItems.find(u->group()))
		{
			(void)new RegisteredUsersDialogItem(groupItems.find(u->group()), u);
		}
		else if(groupItems.find(__tr2qs_ctx("Default", "register")))
		{
			(void)new RegisteredUsersDialogItem(groupItems.find(__tr2qs_ctx("Default", "register")), u);
		}
		else
		{ //should never be called
			KviRegisteredUserGroup * pGroup = g_pLocalRegisteredUserDataBase->addGroup(__tr2qs_ctx("Default", "register"));
			RegisteredUsersGroupItem * pCur = new RegisteredUsersGroupItem(m_pListView, pGroup);
			groupItems.insert(__tr2qs_ctx("Default", "register"), pCur);
			(void)new RegisteredUsersDialogItem(pCur, u);
		}
	}
	if(m_pListView->topLevelItem(0))
	{
		m_pListView->topLevelItem(0)->setSelected(true);
		m_pListView->setCurrentItem(m_pListView->topLevelItem(0));
	}
}

void RegisteredUsersDialog::closeEvent(QCloseEvent * e)
{
	m_pListView->clear();
	e->accept();
	okClicked();
	//delete this;
}

void RegisteredUsersDialog::okClicked()
{
	m_pListView->clear();
	g_pRegisteredUserDataBase->copyFrom(g_pLocalRegisteredUserDataBase);
	g_pApp->restartNotifyLists();
	delete this;
}

void RegisteredUsersDialog::cancelClicked()
{
	m_pListView->clear();
	delete this;
}

void RegisteredUsersDialog::addClicked()
{
	RegisteredUserEntryDialog * dlg = new RegisteredUserEntryDialog(this, nullptr);
	int ret = dlg->exec();
	delete dlg;

	if(!g_pRegisteredUsersDialog)
		return; // we have been deleted!

	if(ret == QDialog::Accepted)
	{
		fillList();
	}
}

void RegisteredUsersDialog::addWizardClicked()
{
	RegistrationWizard * w = new RegistrationWizard("", g_pLocalRegisteredUserDataBase, this, true);
	int ret = w->exec();
	delete w;
	if(!g_pRegisteredUsersDialog)
		return; // we have been deleted!
	if(ret == QDialog::Accepted)
	{
		fillList();
	}
}

void RegisteredUsersDialog::removeClicked()
{
	QList<QTreeWidgetItem *> list = m_pListView->selectedItems();
	for(int i = 0; i < list.count(); i++)
	{
		RegisteredUsersDialogItemBase * b = (RegisteredUsersDialogItemBase *)(list.at(i));
		if(b->type() == RegisteredUsersDialogItemBase::User)
		{
			g_pLocalRegisteredUserDataBase->removeUser(((RegisteredUsersDialogItem *)list.at(i))->user()->name());
		}
		else
		{
			g_pLocalRegisteredUserDataBase->removeGroup(((RegisteredUsersGroupItem *)list.at(i))->group()->name());
		}
	}
	fillList();
}

void RegisteredUsersDialog::editClicked()
{
	RegisteredUsersDialogItemBase * b = (RegisteredUsersDialogItemBase *)(m_pListView->currentItem());
	if(!b)
		return;
	if(b->type() == RegisteredUsersDialogItemBase::User)
	{
		RegisteredUsersDialogItem * i = (RegisteredUsersDialogItem *)b;
		editItem(i);
	}
	else
	{
		RegisteredUsersGroupItem * i = (RegisteredUsersGroupItem *)b;
		editGroup(i->group());
	}
}

void RegisteredUsersDialog::editItem(RegisteredUsersDialogItem * i)
{
	KviRegisteredUser * u = i->user();

	i->setUser(nullptr);

	QString szName = u->name();

	RegisteredUserEntryDialog * dlg = new RegisteredUserEntryDialog(this, u);
	int res = dlg->exec();
	delete dlg;

	if(!g_pRegisteredUsersDialog)
		return; // we have been deleted!

	if(res == QDialog::Accepted)
	{
		fillList();

		// select the last edited item
		for(int c = 0; c < m_pListView->topLevelItemCount(); c++)
		{
			QTreeWidgetItem * pGroup = m_pListView->topLevelItem(c);
			for(int d = 0; d < pGroup->childCount(); d++)
			{
				RegisteredUsersDialogItem * pUser = (RegisteredUsersDialogItem *)pGroup->child(d);
				if(KviQString::equalCI(pUser->user()->name(), szName))
				{
					pUser->setSelected(true);
					m_pListView->setCurrentItem(pUser);
					break;
				}
			}
		}

		return;
	}

	i->setUser(u);
	m_pListView->update();
}

void RegisteredUsersDialog::selectionChanged()
{
	bool bHaveSelected = !m_pListView->selectedItems().empty();

	m_pEditButton->setEnabled(bHaveSelected);
	m_pRemoveButton->setEnabled(bHaveSelected);
	m_pExportButton->setEnabled(bHaveSelected);
}

#define KVI_REGUSER_DB_FILE_MAGIC 0x5334DBDB
#define KVI_REGUSER_DB_FILE_VERSION 1

struct KviReguserDbFileHeader
{
	kvi_u32_t magic;
	kvi_u32_t version;
	kvi_u32_t nentries;
};

void RegisteredUsersDialog::exportClicked()
{
	unsigned int nEntries = 0;

	QList<QTreeWidgetItem *> list = m_pListView->selectedItems();

	//only "user" entries matters, discard groups from count
	for(int i = 0; i < list.count(); i++)
	{
		if(((RegisteredUsersDialogItemBase *)list.at(i))->type() == RegisteredUsersDialogItemBase::User)
			nEntries++;
	}

	if(nEntries < 1)
	{
		KviMessageBox::warning(__tr2qs_ctx("No entries selected.", "register"));
		return;
	}

	QString buffer;

	if(!KviFileDialog::askForSaveFileName(buffer, __tr2qs_ctx("Enter a Filename - KVIrc", "register"), nullptr, nullptr, true, true, true, this))
		return;

	if(!g_pRegisteredUsersDialog)
		return; // we have been deleted!

	KviFile f(buffer);
	if(!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		KviMessageBox::warning(__tr2qs_ctx("Can't open file %Q for writing.", "register"), &buffer);
		return;
	}

	KviReguserDbFileHeader hf;
	hf.magic = KVI_REGUSER_DB_FILE_MAGIC;
	hf.version = KVI_REGUSER_DB_FILE_VERSION;
	hf.nentries = nEntries;

	if(f.write((const char *)&hf, sizeof(KviReguserDbFileHeader)) != sizeof(KviReguserDbFileHeader))
		goto write_error;

	for(int i = 0; i < list.count(); i++)
	{
		RegisteredUsersDialogItemBase * pBase = (RegisteredUsersDialogItemBase *)list.at(i);
		if(pBase->type() != RegisteredUsersDialogItemBase::User)
			continue;

		KviRegisteredUser * u = ((RegisteredUsersDialogItem *)list.at(i))->user();
		if(u)
		{
			if(!f.save(u->name()))
				goto write_error;
			KviPointerHashTable<QString, QString> * pd = u->propertyDict();
			if(pd)
			{
				if(!f.save(pd->count()))
					goto write_error;
				for(KviPointerHashTableEntry<QString, QString> * pCur = pd->firstEntry(); pCur; pCur = pd->nextEntry())
				{
					QString key = pCur->key();
					if(!f.save(key))
						goto write_error;
					if(!f.save(*(pCur->data())))
						goto write_error;
				}
			}
			else
			{
				if(!f.save(0))
					goto write_error;
			}

			KviPointerList<KviIrcMask> * ml = u->maskList();
			if(ml)
			{
				if(!f.save(ml->count()))
					goto write_error;
				for(KviIrcMask * m = ml->first(); m; m = ml->next())
				{
					QString fullMask;
					m->mask(fullMask, KviIrcMask::NickUserHost);
					if(!f.save(fullMask))
						goto write_error;
				}
			}
			else
			{
				if(!f.save(0))
					goto write_error;
			}

			QString avatar;
			if(u->getProperty("avatar", avatar))
			{
				std::unique_ptr<KviAvatar> av{g_pIconManager->getAvatar(QString(), avatar)};
				if(av)
				{
					if(!av->pixmap()->isNull())
					{
						if(!f.save(1))
							goto write_error;
						QImageWriter io;
						io.setDevice(&f);
						io.setFormat("PNG");
						if(!io.write(av->pixmap()->toImage()))
							goto write_error;
					}
					else
					{
						if(!f.save(0))
							goto write_error;
					}
				}
				else
				{
					if(!f.save(0))
						goto write_error;
				}
			}
			else
			{
				if(!f.save(0))
					goto write_error;
			}
		}
	}

	goto succesfull_export;

write_error:
	KviMessageBox::warning(__tr2qs_ctx("Can't export the registered users database: write error.", "register"));
	f.close();
	return;

succesfull_export:

	f.close();
}

void RegisteredUsersDialog::selectAllClicked()
{
	m_pListView->selectAll();
}

void RegisteredUsersDialog::importClicked()
{
	//KviCString buffer;
	QString buffer;

	if(!KviFileDialog::askForOpenFileName(buffer, __tr2qs_ctx("Select a File - KVIrc", "register"), QString(), QString(), false, true, this))
		return;

	if(!g_pRegisteredUsersDialog)
		return; // we have been deleted!

	KviFile f(buffer);
	if(!f.open(QIODevice::ReadOnly))
	{
		KviMessageBox::warning(__tr2qs_ctx("Can't open file %Q for reading.", "register"), &buffer);
		return;
	}

	KviReguserDbFileHeader hf;
	unsigned int idx;

	if(f.read((char *)&hf, sizeof(KviReguserDbFileHeader)) != sizeof(KviReguserDbFileHeader))
		goto read_error;

	if(hf.magic != KVI_REGUSER_DB_FILE_MAGIC)
	{
		KviMessageBox::warning(__tr2qs_ctx("The file %Q doesn't appear to be a valid registered users database.", "register"), &buffer);
		f.close();
		return;
	}

	if(hf.version != KVI_REGUSER_DB_FILE_VERSION)
	{
		KviMessageBox::warning(__tr2qs_ctx("The file %Q contains an invalid registered users database version.", "register"), &buffer);
		f.close();
		return;
	}

	for(idx = 0; idx < hf.nentries; idx++)
	{
		QString szName;
		if(!f.load(szName))
			goto read_error;
		KviRegisteredUser * u = g_pLocalRegisteredUserDataBase->getUser(szName);
		if(!u)
			goto read_error;
		unsigned int count = 0;
		if(!f.load(count))
			goto read_error;
		for(unsigned int up = 0; up < count; up++)
		{
			QString szKey, szValue;
			if(!f.load(szKey))
				goto read_error;
			if(!f.load(szValue))
				goto read_error;
			u->setProperty(szKey, szValue);
		}
		if(!f.load(count))
			goto read_error;
		for(unsigned int um = 0; um < count; um++)
		{
			QString szMask;
			if(!f.load(szMask))
				goto read_error;
			if(!szMask.isEmpty())
			{
				KviIrcMask * m = new KviIrcMask(szMask);
				g_pLocalRegisteredUserDataBase->addMask(u, m);
			}
		}
		if(!f.load(count))
			goto read_error;
		if(count)
		{
			// there is an avatar
			QImage img;
			QImageReader io;
			io.setDevice(&f);
			io.setFormat("PNG");
			img = io.read();
			//			if(io.read())goto read_error;

			if(img.isNull())
				qDebug("Oops! Read a null image?");

			QString fName = u->name();
			KviFileUtils::adjustFilePath(fName);

			QString fPath;
			int rnm = 0;
			do
			{
				g_pApp->getLocalKvircDirectory(fPath, KviApplication::Avatars, fName);
				fPath.append(QString("%1.png").arg(rnm));
				rnm++;
			} while(KviFileUtils::fileExists(fPath));

			if(!img.save(fPath, "PNG"))
			{
				qDebug("Can't save image %s", fPath.toUtf8().data());
			}
			else
			{
				u->setProperty("avatar", fPath);
			}
		}
	}

	goto succesfull_import;

read_error:
	KviMessageBox::warning(__tr2qs_ctx("Can't import the registered users database: read error.", "register"));
	f.close();
	return;

succesfull_import:

	f.close();
	fillList();
}
