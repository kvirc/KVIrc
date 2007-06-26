#ifndef _DIALOG_H_
#define _DIALOG_H_
//=============================================================================
//
//   File : edituser.h
//   Creation date : Sun Dec 24 2000 15:16:17 CEST by Szymon Stefanek
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
#include "kvi_string.h"
#include "kvi_regusersdb.h"
#include "kvi_selectors.h"

#include <qwidget.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include "kvi_tal_listview.h"
#include "kvi_tal_listbox.h"
#include "kvi_dict.h"
#include "kvi_tal_popupmenu.h"
#ifdef COMPILE_USE_QT4
	#include <q3intdict.h>
	#include <q3simplerichtext.h>
	#define QSimpleRichText Q3SimpleRichText
#else
	#include <qsimplerichtext.h>
	#include <qintdict.h>


#endif

class KviRegisteredUsersDialogItemBase : public KviTalListViewItem
{
public:
	enum Types { User,Group };
protected:
	KviRegisteredUsersDialogItemBase(Types type,KviTalListView * par)
	:KviTalListViewItem(par),m_iType(type)
	{
	};
	KviRegisteredUsersDialogItemBase(Types type,KviTalListViewItem * par)
	:KviTalListViewItem(par),m_iType(type)
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
	KviRegisteredUsersGroupItem(KviTalListView * par,KviRegisteredUserGroup * g)
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
protected:
	KviRegisteredUser * m_pUser;
	QSimpleRichText * m_pText;
public:
	KviRegisteredUsersDialogItem(KviTalListViewItem * par,KviRegisteredUser * u);
	~KviRegisteredUsersDialogItem();
public:
	KviRegisteredUser * user(){ return m_pUser; };
	void setUser(KviRegisteredUser * u){ m_pUser = u; };
	virtual void paintCell(QPainter * p,const QColorGroup &cg,int column,int width,int align);
	virtual void setup();
	virtual QString key(int,bool) const;
};

class KviRegisteredUsersDialog : public QWidget
{
	Q_OBJECT
public:
	KviRegisteredUsersDialog(QWidget * par = 0);
	~KviRegisteredUsersDialog();
public:
	KviTalListView   * m_pListView;
	QPushButton * m_pAddButton;
	QPushButton * m_pWizardAddButton;
	QPushButton * m_pRemoveButton;
	QPushButton * m_pEditButton;
	QPushButton * m_pImportButton;
	QPushButton * m_pExportButton;
	QPushButton * m_pAddGroupButton;
#ifdef COMPILE_USE_QT4
	Q3IntDict<KviRegisteredUserGroup> m_TmpDict;
#else
	QIntDict<KviRegisteredUserGroup> m_TmpDict;
#endif

	protected slots:
	void itemPressed(KviTalListViewItem *it,const QPoint &pnt,int c);
	void itemDoubleClicked(KviTalListViewItem *it);
protected:
	void fillList();
protected:
	virtual void closeEvent(QCloseEvent *);
	void editItem(KviRegisteredUsersDialogItem * i);
	void editGroup(KviRegisteredUserGroup* group);
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
	void listViewRightButtonClicked ( KviTalListViewItem *, const QPoint &, int );
	void moveToGroupMenuClicked(int);
};

#endif
