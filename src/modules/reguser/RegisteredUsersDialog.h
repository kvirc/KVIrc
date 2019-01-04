#ifndef _DIALOG_H_
#define _DIALOG_H_
//=============================================================================
//
//   File : RegisteredUsersDialog.h
//   Creation date : Sun Dec 24 2000 15:16:17 CEST by Szymon Stefanek
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

#include "KviCString.h"
#include "KviRegisteredUserDataBase.h"
#include "KviSelectors.h"
#include "KviPointerHashTable.h"
#include <QTreeWidget>
#include "KviTalListWidget.h"

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QStyledItemDelegate>
#include <QTextDocument>
#include <QMenu>

class KviRegisteredUsersListView : public QTreeWidget
{
	Q_OBJECT
public:
	KviRegisteredUsersListView(QWidget *);
	~KviRegisteredUsersListView(){};

protected:
	void mousePressEvent(QMouseEvent * e) override;
signals:
	void rightButtonPressed(QTreeWidgetItem *, QPoint);
};

class RegisteredUsersDialogItemDelegate : public QStyledItemDelegate
{
public:
	RegisteredUsersDialogItemDelegate(KviRegisteredUsersListView * pWidget = nullptr)
	    : QStyledItemDelegate(pWidget){};
	~RegisteredUsersDialogItemDelegate(){};
	QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const override;
	void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const override;
};

class RegisteredUsersDialogItemBase : public QTreeWidgetItem
{
public:
	enum Types
	{
		User,
		Group
	};

protected:
	RegisteredUsersDialogItemBase(Types type, KviRegisteredUsersListView * par)
	    : QTreeWidgetItem(par), m_iType(type){};
	RegisteredUsersDialogItemBase(Types type, QTreeWidgetItem * par)
	    : QTreeWidgetItem(par), m_iType(type){};
	~RegisteredUsersDialogItemBase(){};

private:
	RegisteredUsersDialogItemBase::Types m_iType;

public:
	RegisteredUsersDialogItemBase::Types type() { return m_iType; };
};

class RegisteredUsersGroupItem : public RegisteredUsersDialogItemBase
{
protected:
	KviRegisteredUserGroup * m_pGroup;

public:
	RegisteredUsersGroupItem(KviRegisteredUsersListView * par, KviRegisteredUserGroup * g)
	    : RegisteredUsersDialogItemBase(Group, par), m_pGroup(g)
	{
		setText(0, m_pGroup->name());
	}
	~RegisteredUsersGroupItem()
	{
	}
	KviRegisteredUserGroup * group() { return m_pGroup; };
};

class RegisteredUsersDialogItem : public RegisteredUsersDialogItemBase
{
	friend class RegisteredUsersDialogItemDelegate;

protected:
	KviRegisteredUser * m_pUser;
	QTextDocument m_pText;

public:
	RegisteredUsersDialogItem(QTreeWidgetItem * par, KviRegisteredUser * u);
	~RegisteredUsersDialogItem();

public:
	KviRegisteredUser * user() { return m_pUser; };
	void setUser(KviRegisteredUser * u) { m_pUser = u; };
	virtual QString key(int, bool) const;
};

class RegisteredUsersDialog : public QWidget
{
	Q_OBJECT
public:
	RegisteredUsersDialog(QWidget * par = nullptr);
	~RegisteredUsersDialog();

public:
	KviRegisteredUsersListView * m_pListView;
	QPushButton * m_pAddButton;
	QPushButton * m_pWizardAddButton;
	QPushButton * m_pRemoveButton;
	QPushButton * m_pEditButton;
	QPushButton * m_pImportButton;
	QPushButton * m_pExportButton;
	QPushButton * m_pAddGroupButton;
	QPushButton * m_pSelectAllButton;

protected:
	void fillList();
	void editItem(RegisteredUsersDialogItem * i);
	void editGroup(KviRegisteredUserGroup * group);
	void closeEvent(QCloseEvent *) override;
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
	void itemPressed(QTreeWidgetItem * it, int c);
	void itemDoubleClicked(QTreeWidgetItem * it, int);
	void rightButtonPressed(QTreeWidgetItem *, QPoint);
	void moveToGroupMenuClicked(QAction * pAction);
};

#endif
