#ifndef _DIALOG_H_
#define _DIALOG_H_
//=============================================================================
//
//   File : dialog.h
//   Created on Fri 10 Feb 2006 18:52:18 by Szymon Stefanek
//   Based on dlg_options.h: Tue Jun 11 2000 02:39:12 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2000-2006 Szymon Stefanek <pragma at kvirc dot net>
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
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================


#include <qdialog.h>
#include "kvi_tal_listview.h"
#include <qstringlist.h>
#include "kvi_optionswidget.h"

#include "instances.h"

class QLabel;
class KviTalWidgetStack;
class QPushButton;
class QLineEdit;
class QToolButton;

class KviGeneralOptionsFrontWidget : public KviOptionsWidget
{
	Q_OBJECT
public:
	KviGeneralOptionsFrontWidget(QWidget *parent,const QString &szText);
	~KviGeneralOptionsFrontWidget();
};

class KviOptionsListViewItem : public KviTalListViewItem
{
public:
	KviOptionsListViewItem(KviTalListView *parent,KviOptionsWidgetInstanceEntry * e);
	KviOptionsListViewItem(KviTalListViewItem *parent,KviOptionsWidgetInstanceEntry * e);
	~KviOptionsListViewItem();
public:
	KviOptionsWidgetInstanceEntry * m_pInstanceEntry;
	KviOptionsWidget              * m_pOptionsWidget;
	bool m_bHighlighted;
public:
	void setHighlighted(bool b){ m_bHighlighted = b; };
protected:
	virtual void paintCell(QPainter * p,const QColorGroup & cg,int column,int width,int align);
};

class KviOptionsDialog : public QDialog
{
	Q_OBJECT
public:
	KviOptionsDialog(QWidget * par,const QString &szGroup); 
	~KviOptionsDialog();
private:
	KviTalListView    * m_pListView;
	QLabel       * m_pCategoryLabel;
	KviTalWidgetStack * m_pWidgetStack;
	KviGeneralOptionsFrontWidget* m_pFrontWidget;
	QString        m_szGroup;
	QLineEdit    * m_pSearchLineEdit;
	QToolButton  * m_pSearchButton;
private:
	void recursiveCommit(KviOptionsListViewItem *it);
	void fillListView(KviTalListViewItem * p,KviPointerList<KviOptionsWidgetInstanceEntry> * l,const QString &szGroup,bool bNotContainedOnly = false);
	//KviOptionsListViewItem * showHiddenChildren(KviTalListViewItem * p,KviPointerList<KviOptionsWidgetInstanceEntry> * l);
	KviOptionsListViewItem * findItemByPage(KviOptionsListViewItem *it,KviOptionsWidget * pPage);
private slots:
	void listViewItemSelectionChanged(KviTalListViewItem *it);
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
	bool recursiveSearch(KviOptionsListViewItem * pItem,const QStringList &lKeywords);
public:
	void search(const QString &szKeywords);
	void search(const QStringList &lKeywords);
};


#endif //!_DIALOG_H_
