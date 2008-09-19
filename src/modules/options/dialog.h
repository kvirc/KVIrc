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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#include "instances.h"

#include "kvi_optionswidget.h"
#include "kvi_tal_treewidget.h"

#include <QDialog>
#include <QStringList>
#include <QToolButton>
#include <QPainter>
#include <QColorGroup>

class QLabel;
class QPushButton;
class QLineEdit;
class KviTalWidgetStack;

class KviGeneralOptionsFrontWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviGeneralOptionsFrontWidget(QWidget *parent,const QString &szText);
	~KviGeneralOptionsFrontWidget();
};

class KviOptionsTreeWidgetItem : public KviTalTreeWidgetItem
{
public:
	KviOptionsTreeWidgetItem(KviTalTreeWidget *parent,KviOptionsWidgetInstanceEntry * e);
	KviOptionsTreeWidgetItem(KviTalTreeWidgetItem *parent,KviOptionsWidgetInstanceEntry * e);
	~KviOptionsTreeWidgetItem();
public:
	KviOptionsWidgetInstanceEntry * m_pInstanceEntry;
	KviOptionsWidget              * m_pOptionsWidget;
	bool m_bHighlighted;
public:
	void setHighlighted(bool b){ m_bHighlighted = b; };
protected:
// 	virtual void paintCell(QPainter * p,const QColorGroup & cg,int column,int width,int align);
};

class KviOptionsDialog : public QDialog
{
	Q_OBJECT
public:
	KviOptionsDialog(QWidget * par,const QString &szGroup);
	~KviOptionsDialog();
private:
	KviTalTreeWidget    * m_pTreeWidget;
	QLabel       * m_pCategoryLabel;
	KviTalWidgetStack * m_pWidgetStack;
	KviGeneralOptionsFrontWidget* m_pFrontWidget;
	QString        m_szGroup;
	QLineEdit    * m_pSearchLineEdit;
	QToolButton  * m_pSearchButton;
private:
	void recursiveCommit(KviOptionsTreeWidgetItem *it);
	void fillTreeWidget(KviTalTreeWidgetItem * p,KviPointerList<KviOptionsWidgetInstanceEntry> * l,const QString &szGroup,bool bNotContainedOnly = false);
	//KviOptionsTreeWidgetItem * showHiddenChildren(KviTalTreeWidgetItem * p,KviPointerList<KviOptionsWidgetInstanceEntry> * l);
// 	KviOptionsTreeWidgetItem * findItemByPage(KviOptionsTreeWidgetItem *it,KviOptionsWidget * pPage);
private slots:
	void treeWidgetItemSelectionChanged(KviTalTreeWidgetItem* it, KviTalTreeWidgetItem *prev);
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
	bool recursiveSearch(KviOptionsTreeWidgetItem * pItem,const QStringList &lKeywords);
public:
	void search(const QString &szKeywords);
	void search(const QStringList &lKeywords);
};


#endif //!_DIALOG_H_
