#ifndef _DIALOG_H_
#define _DIALOG_H_
//=============================================================================
//
//   File : OptionsDialog.h
//   Created on Fri 10 Feb 2006 18:52:18 by Szymon Stefanek
//   Based on dlg_options.h: Tue Jun 11 2000 02:39:12 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "OptionsInstanceManager.h"

#include "KviOptionsWidget.h"
#include <QTreeWidget>

#include <QWidget>
#include <QDialog>
#include <QStringList>
#include <QToolButton>
#include <QPainter>

class QLabel;
class QPushButton;
class QLineEdit;
class QStackedWidget;

class OptionsDialogFrontWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	OptionsDialogFrontWidget(QWidget * parent, const QString & szText);
	~OptionsDialogFrontWidget();
};

class OptionsDialogTreeWidgetItem : public QTreeWidgetItem
{
public:
	OptionsDialogTreeWidgetItem(QTreeWidget * parent, OptionsWidgetInstanceEntry * e);
	OptionsDialogTreeWidgetItem(QTreeWidgetItem * parent, OptionsWidgetInstanceEntry * e);
	~OptionsDialogTreeWidgetItem();

public:
	OptionsWidgetInstanceEntry * m_pInstanceEntry;
	KviOptionsWidget * m_pOptionsWidget;
};

class OptionsDialog : public QDialog
{
	Q_OBJECT
public:
	OptionsDialog(QWidget * par, const QString & szGroup, bool bModal);
	~OptionsDialog();

private:
	QTreeWidget * m_pTreeWidget;
	QLabel * m_pCategoryLabel;
	QStackedWidget * m_pWidgetStack;
	OptionsDialogFrontWidget * m_pFrontWidget;
	QString m_szGroup;
	QLineEdit * m_pSearchLineEdit;
	QToolButton * m_pSearchButton;

private:
	void recursiveCommit(OptionsDialogTreeWidgetItem * it);
	void fillTreeWidget(QTreeWidgetItem * p, KviPointerList<OptionsWidgetInstanceEntry> * l, const QString & szGroup, bool bNotContainedOnly = false);
	//OptionsDialogTreeWidgetItem * showHiddenChildren(QTreeWidgetItem * p,KviPointerList<OptionsWidgetInstanceEntry> * l);
	//OptionsDialogTreeWidgetItem * findItemByPage(OptionsDialogTreeWidgetItem *it,KviOptionsWidget * pPage);
private slots:
	void treeWidgetItemSelectionChanged(QTreeWidgetItem * it, QTreeWidgetItem * prev);
	void applyClicked();
	void okClicked();
	void cancelClicked();
	void pageWantsToSwitchToAdvancedPage(KviOptionsWidget * pPage);
	void searchClicked();
	void searchLineEditTextChanged(const QString &);

protected:
	void apply(bool bDialogAboutToClose);
	void closeEvent(QCloseEvent * e) override;
	void keyPressEvent(QKeyEvent * e) override;
	void showEvent(QShowEvent * e) override;
	bool recursiveSearch(OptionsDialogTreeWidgetItem * pItem, const QStringList & lKeywords);
	void clearSearch();
	bool searchInSelectors(KviOptionsWidget * pWidget, const QStringList & lKeywords);

public:
	void search(const QString & szKeywords);
	void search(const QStringList & lKeywords);
};

#endif //_DIALOG_H_
