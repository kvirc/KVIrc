#ifndef _DIALOG_H_
#define _DIALOG_H_
//=============================================================================
//
//   File : edituser.h
//   Creation date : Sun Dec 24 2000 15:16:17 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "kvi_string.h"
#include "kvi_regusersdb.h"
#include "kvi_selectors.h"
#include "kvi_pointerhashtable.h"
#include <QTreeWidget>
#include "kvi_tal_listwidget.h"
#include "kvi_tal_popupmenu.h"

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QStyledItemDelegate>
#include <QMultiHash>
#include <QTextDocument>

class  KviRegisteredUsersListView : public QTreeWidget
{
	Q_OBJECT
public:
	 KviRegisteredUsersListView(QWidget*);
	~KviRegisteredUsersListView(){};
protected:
	void mousePressEvent (QMouseEvent *e);
signals:
	void rightButtonPressed(QTreeWidgetItem *,QPoint);
};

class KviRegisteredUsersDialogItemDelegate : public QStyledItemDelegate
{
public:
	KviRegisteredUsersDialogItemDelegate(KviRegisteredUsersListView * pWidget=0)
		: QStyledItemDelegate(pWidget) {};
	~KviRegisteredUsersDialogItemDelegate(){};
	QSize sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const;
	void paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;

};

class KviRegisteredUsersDialogItemBase : public QTreeWidgetItem
{
public:
	enum Types { User,Group };
protected:
	KviRegisteredUsersDialogItemBase(Types type,KviRegisteredUsersListView * par)
	:QTreeWidgetItem(par),m_iType(type)
	{
	};
	KviRegisteredUsersDialogItemBase(Types type,QTreeWidgetItem * par)
	:QTreeWidgetItem(par),m_iType(type)
	{
	};
	~KviRegisteredUsersDialogItemBase()
	{
	};

private:
	KviRegisteredUsersDialogItemBase::Types m_iType;
public:
	KviRegisteredUsersDialogItemBase::Types type() { return m_iType; };
};

class KviRegisteredUsersGroupItem : public KviRegisteredUsersDialogItemBase
{
protected:
	KviRegisteredUserGroup * m_pGroup;
public:
	KviRegisteredUsersGroupItem(KviRegisteredUsersListView * par,KviRegisteredUserGroup * g)
	:KviRegisteredUsersDialogItemBase(Group,par), m_pGroup(g)
	{
		setText(0,m_pGroup->name());
	}
	~KviRegisteredUsersGroupItem()
	{
	}
	KviRegisteredUserGroup * group() { return m_pGroup; };
};

class KviRegisteredUsersDialogItem : public KviRegisteredUsersDialogItemBase
{
	friend class KviRegisteredUsersDialogItemDelegate;
protected:
	KviRegisteredUser * m_pUser;
	QTextDocument       m_pText;
public:
	KviRegisteredUsersDialogItem(QTreeWidgetItem * par,KviRegisteredUser * u);
	~KviRegisteredUsersDialogItem();
public:
	KviRegisteredUser * user(){ return m_pUser; };
	void setUser(KviRegisteredUser * u){ m_pUser = u; };
	virtual QString key(int,bool) const;
};

class KviRegisteredUsersDialog : public QWidget
{
	Q_OBJECT
public:
	KviRegisteredUsersDialog(QWidget * par = 0);
	~KviRegisteredUsersDialog();
public:
	KviRegisteredUsersListView   * m_pListView;
	QPushButton * m_pAddButton;
	QPushButton * m_pWizardAddButton;
	QPushButton * m_pRemoveButton;
	QPushButton * m_pEditButton;
	QPushButton * m_pImportButton;
	QPushButton * m_pExportButton;
	QPushButton * m_pAddGroupButton;
	QPushButton * m_pSelectAllButton;
	QMultiHash<int, KviRegisteredUserGroup*> m_TmpDict;
protected:
	void fillList();
	void editItem(KviRegisteredUsersDialogItem * i);
	void editGroup(KviRegisteredUserGroup* group);
	virtual void closeEvent(QCloseEvent *);
protected slots:
	void cancelClicked();
	void okClicked();
	void addClicked();
	void removeClicked();
	void editClicked();
	void selectionChanged();
	void importClicked();
	void exportClicked();
	void addWizardClicked();
	void addGroupClicked();
	void selectAllClicked();
	void itemPressed(QTreeWidgetItem *it,int c);
	void itemDoubleClicked(QTreeWidgetItem *it, int);
	void rightButtonPressed ( QTreeWidgetItem *, QPoint);
	void moveToGroupMenuClicked(int);
};

#endif
