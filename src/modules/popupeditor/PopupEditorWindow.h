#ifndef _POPUPEDITOR_H_
#define _POPUPEDITOR_H_
//=============================================================================
//
//   File : PopupEditorWindow.h
//   Creation date : Mon Dec 23 2002 20:24:55 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviWindow.h"
#include "KviCString.h"

#include <QTreeWidget>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QMenu>

class KviKvsPopupMenu;
class KviKvsPopupMenuItem;
class KviScriptEditor;
class MenuTreeWidgetItem;

// PopupTreeWidgetItem
class PopupTreeWidgetItem : public QTreeWidgetItem
{
public:
	enum Type
	{
		Item,
		Menu,
		Separator,
		Label,
		Epilogue,
		Prologue,
		ExtMenu
	};

public:
	PopupTreeWidgetItem(QTreeWidget * pTreeWidget, PopupTreeWidgetItem * after, Type t);
	PopupTreeWidgetItem(PopupTreeWidgetItem * parent, PopupTreeWidgetItem * after, Type t);

public:
	Type m_type;
	QString m_szText;
	QString m_szCondition;
	QString m_szIcon;
	QString m_szCode;
	QString m_szId;

private:
	void init();

public:
	void setItemText(const QString & szText);
	void setCondition(const QString & szCondition);
	void setIcon(const QString & szIcon);
	void setCode(const QString & szCode);
	void setId(const QString & szId);
};

class SinglePopupEditor final : public QWidget
{
	Q_OBJECT
public:
	SinglePopupEditor(QWidget * par);
	~SinglePopupEditor();

protected:
	QPushButton * m_pMenuButton;
	KviKvsPopupMenu * m_pClipboard;
	KviKvsPopupMenu * m_pTestPopup;
	PopupTreeWidgetItem * m_pLastSelectedItem;
	QTreeWidget * m_pTreeWidget;
	QLineEdit * m_pNameEditor;
	KviScriptEditor * m_pEditor;
	QLineEdit * m_pTextEditor;
	QLineEdit * m_pIdEditor;
	QLineEdit * m_pIconEditor;
	QLineEdit * m_pConditionEditor;
	QLineEdit * m_pExtNameEditor;
	QMenu * m_pContextPopup;

public:
	void edit(MenuTreeWidgetItem * it);
	KviKvsPopupMenu * getMenu();

protected:
	// theItem is the item above the first item that has to be inserted
	void populateMenu(KviKvsPopupMenu * pop, PopupTreeWidgetItem * par, PopupTreeWidgetItem * theItem = nullptr);
	void saveLastSelectedItem();
	void addItemToMenu(KviKvsPopupMenu * pop, PopupTreeWidgetItem * par);
	PopupTreeWidgetItem * newItem(PopupTreeWidgetItem * par, PopupTreeWidgetItem * after, PopupTreeWidgetItem::Type t);
	PopupTreeWidgetItem * newItemBelow(PopupTreeWidgetItem * it, PopupTreeWidgetItem::Type t);
	PopupTreeWidgetItem * newItemAbove(PopupTreeWidgetItem * it, PopupTreeWidgetItem::Type t);
	PopupTreeWidgetItem * newItemInside(PopupTreeWidgetItem * it, PopupTreeWidgetItem::Type t);
	void createNewItemAboveLastSelected(PopupTreeWidgetItem::Type t);
	void createNewItemBelowLastSelected(PopupTreeWidgetItem::Type t);
	void createNewItemInsideLastSelected(PopupTreeWidgetItem::Type t);
	PopupTreeWidgetItem * findMatchingItem(KviKvsPopupMenuItem * it, PopupTreeWidgetItem * item);
protected slots:
	void contextCut();
	void contextCopy();
	void contextRemove();
	void contextPasteBelow();
	void contextPasteAbove();
	void contextPasteInside();
	void contextNewSeparatorBelow();
	void contextNewSeparatorAbove();
	void contextNewSeparatorInside();
	void contextNewItemBelow();
	void contextNewItemAbove();
	void contextNewItemInside();
	void contextNewMenuBelow();
	void contextNewMenuAbove();
	void contextNewMenuInside();
	void contextNewExtMenuBelow();
	void contextNewExtMenuAbove();
	void contextNewExtMenuInside();
	void contextNewLabelBelow();
	void contextNewLabelAbove();
	void contextNewLabelInside();
	void contextNewPrologue();
	void contextNewEpilogue();
	void selectionChanged();
	void customContextMenuRequested(const QPoint & pnt);
	void testPopup();
	void testModeMenuItemClicked(KviKvsPopupMenuItem * it);
};

class MenuTreeWidgetItem final : public QTreeWidgetItem
{
public:
	MenuTreeWidgetItem(QTreeWidget * par, KviKvsPopupMenu * popup);
	~MenuTreeWidgetItem();

public:
	KviKvsPopupMenu * m_pPopup;

public:
	KviKvsPopupMenu * popup() { return m_pPopup; };
public:
	void replacePopup(KviKvsPopupMenu * popup);
};

class PopupEditorWidget final : public QWidget
{
	Q_OBJECT
public:
	PopupEditorWidget(QWidget * par);

public:
	SinglePopupEditor * m_pEditor;
	QTreeWidget * m_pTreeWidget;
	MenuTreeWidgetItem * m_pLastEditedItem;
	bool m_bOneTimeSetupDone;
	QMenu * m_pContextPopup;
	QMenu * m_pEmptyContextPopup;
	bool m_bSaving;

public:
	void commit();
	void exportPopups(bool);
protected slots:
	void currentItemChanged(QTreeWidgetItem * it, QTreeWidgetItem * prev);
	void customContextMenuRequested(const QPoint & pnt);
	void newPopup();
	void exportAll();
	void exportSelected();
	void exportCurrentPopup();
	void removeCurrentPopup();
	void popupRefresh(const QString & szName);

protected:
	void showEvent(QShowEvent * e) override;
	void getExportPopupBuffer(QString & buffer, MenuTreeWidgetItem * it);

private:
	void oneTimeSetup();
	void saveLastEditedItem();
	void getUniquePopupName(MenuTreeWidgetItem * it, QString & buffer);
};

class PopupEditorWindow final : public KviWindow
{
	Q_OBJECT
public:
	PopupEditorWindow();
	~PopupEditorWindow();

private:
	PopupEditorWidget * m_pEditor;
	QPixmap * myIconPtr() override;
	void fillCaptionBuffers() override;
	void getConfigGroupName(QString & szName) override { szName = "popupeditor"; };
	void saveProperties(KviConfigurationFile *) override {};
	void loadProperties(KviConfigurationFile *) override {};
private slots:
	void cancelClicked() { close(); };
	void okClicked();
	void applyClicked() { m_pEditor->commit(); };
};

#endif //_POPUPEDITOR_H_
