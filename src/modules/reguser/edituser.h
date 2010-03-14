#ifndef _EDITUSER_H_
#define _EDITUSER_H_
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
#include "kvi_tal_popupmenu.h"
#include <kvi_tal_tabdialog.h>

#include <QListWidget>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QCheckBox>

class KviRegisteredUser;

#ifndef _EDITUSER_CPP_
	extern KviRegisteredUserDataBase * g_pLocalRegisteredUserDataBase;
#endif

class KviReguserPropertiesDialog : public QDialog
{
	Q_OBJECT
public:
	KviReguserPropertiesDialog(QWidget *p,KviPointerHashTable<QString,QString> * dict);
	~KviReguserPropertiesDialog();
protected:
	QTableWidget *m_pTable;

	KviPointerHashTable<QString,QString>     * m_pPropertyDict;
	QPushButton        * m_pDelButton;
	QPushButton        * m_pAddButton;
protected:
	void fillData();
	virtual void closeEvent(QCloseEvent *);
protected slots:
	void okClicked();
	void addClicked();
	void delClicked();
};


class KviReguserMaskDialog : public QDialog
{
	Q_OBJECT
public:
	KviReguserMaskDialog(QWidget * p,KviIrcMask * m);
	~KviReguserMaskDialog();
protected:
	KviIrcMask * m_pMask;

	QLineEdit * m_pNickEdit;
	QLineEdit * m_pUserEdit;
	QLineEdit * m_pHostEdit;

	virtual void closeEvent(QCloseEvent *);
protected slots:
	void okClicked();
};


class KviRegisteredUserEntryDialog : public KviTalTabDialog
{
	Q_OBJECT
public:
	KviRegisteredUserEntryDialog(QWidget * p,KviRegisteredUser * r,bool bModal = true);
	virtual ~KviRegisteredUserEntryDialog();
protected:
	KviRegisteredUser  * m_pUser;

	QLineEdit          * m_pNameEdit;
	QLineEdit          * m_pCommentEdit;

	QListWidget        * m_pMaskListBox;

	QPushButton        * m_pDelMaskButton;
	QPushButton        * m_pEditMaskButton;
	QPushButton        * m_pAddMaskButton;

	QCheckBox          * m_pNotifyCheck;
	QLineEdit          * m_pNotifyNick;
	QLabel             * m_pNotifyLabel;
	
	KviPixmap          * m_pAvatar;
	KviPixmapSelector  * m_pAvatarSelector;

	KviPointerHashTable<QString,QString>     * m_pPropertyDict;

	QCheckBox          * m_pCustomColorCheck;
	QColor             * m_pCustomColor;
	KviColorSelector   * m_pCustomColorSelector;

	//Ignore TAB
	QCheckBox          * m_pIgnoreEnabled;
	QCheckBox          * m_pIgnoreQuery;
	QCheckBox          * m_pIgnoreChannel;
	QCheckBox          * m_pIgnoreNotice;
	QCheckBox          * m_pIgnoreCtcp;
	QCheckBox          * m_pIgnoreInvite;
	QCheckBox          * m_pIgnoreDcc;

	virtual void closeEvent(QCloseEvent *);
protected slots:
	void okClicked();
	void addMaskClicked();
	void delMaskClicked();
	void editMaskClicked();
	void editAllPropertiesClicked();
	void maskCurrentChanged();
	void notifyCheckClicked(bool bChecked);
};

#endif //_EDITUSER_H_
