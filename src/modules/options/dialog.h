#ifndef _DIALOG_H_
#define _DIALOG_H_
//=============================================================================
//
//   File : dialog.h
//   Created on Fri 10 Feb 2006 18:52:18 by Szymon Stefanek
//   Based on dlg_options.h: Tue Jun 11 2000 02:39:12 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek <pragma at kvirc dot net>
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

#include "instances.h"

#include "kvi_optionswidget.h"
#include <QTreeWidget>

#include <QDialog>
#include <QStringList>
#include <QToolButton>
#include <QPainter>
#include <QColorGroup>

class QLabel;
class QPushButton;
class QLineEdit;
class QStackedWidget;

class KviGeneralOptionsFrontWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviGeneralOptionsFrontWidget(QWidget *parent,const QString &szText);
	~KviGeneralOptionsFrontWidget();
};

class KviOptionsTreeWidgetItem : public QTreeWidgetItem
{
public:
	KviOptionsTreeWidgetItem(QTreeWidget *parent,KviOptionsWidgetInstanceEntry * e);
	KviOptionsTreeWidgetItem(QTreeWidgetItem *parent,KviOptionsWidgetInstanceEntry * e);
	~KviOptionsTreeWidgetItem();
public:
	KviOptionsWidgetInstanceEntry * m_pInstanceEntry;
	KviOptionsWidget              * m_pOptionsWidget;
};

class KviOptionsDialog : public QDialog
{
	Q_OBJECT
public:
	KviOptionsDialog(QWidget * par,const QString &szGroup);
	~KviOptionsDialog();
private:
	QTreeWidget                  * m_pTreeWidget;
	QLabel                       * m_pCategoryLabel;
	QStackedWidget               * m_pWidgetStack;
	KviGeneralOptionsFrontWidget * m_pFrontWidget;
	QString                        m_szGroup;
	QLineEdit                    * m_pSearchLineEdit;
	QToolButton                  * m_pSearchButton;
private:
	void recursiveCommit(KviOptionsTreeWidgetItem *it);
	void fillTreeWidget(QTreeWidgetItem * p,KviPointerList<KviOptionsWidgetInstanceEntry> * l,const QString &szGroup,bool bNotContainedOnly = false);
	//KviOptionsTreeWidgetItem * showHiddenChildren(QTreeWidgetItem * p,KviPointerList<KviOptionsWidgetInstanceEntry> * l);
	//KviOptionsTreeWidgetItem * findItemByPage(KviOptionsTreeWidgetItem *it,KviOptionsWidget * pPage);
private slots:
	void treeWidgetItemSelectionChanged(QTreeWidgetItem* it, QTreeWidgetItem *prev);
	void applyClicked();
	void okClicked();
	void cancelClicked();
	void pageWantsToSwitchToAdvancedPage(KviOptionsWidget * pPage);
	void searchClicked();
	void searchLineEditTextChanged(const QString &);
protected:
	void apply(bool bDialogAboutToClose);
	virtual void closeEvent(QCloseEvent *e);
	virtual void keyPressEvent( QKeyEvent * e );
	virtual void showEvent(QShowEvent *e);
	bool recursiveSearch(KviOptionsTreeWidgetItem * pItem,const QStringList &lKeywords);
	bool searchInSelectors(KviOptionsWidget *pWidget,const QStringList &lKeywords);

public:
	void search(const QString &szKeywords);
	void search(const QStringList &lKeywords);
};


#endif //_DIALOG_H_
