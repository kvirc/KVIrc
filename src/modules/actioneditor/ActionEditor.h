#ifndef _ACTIONEDITOR_H_
#define _ACTIONEDITOR_H_
//=============================================================================
//
//   File : ActionEditor.h
//   Creation date : Tue Dec 29 2004 02:40:55 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek (pragma at kvirc dot net)
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
#include <QTreeWidget>

#include <QWidget>
#include <QTextDocument>
#include <QToolButton>
#include <QCheckBox>

class QLineEdit;
class QComboBox;
class QSimpleRichText;
class QPushButton;
class QSplitter;
class ActionEditorTreeWidgetItem;
class KviScriptEditor;
class ActionEditor;

class ActionData
{
public:
	QString m_szName;
	QString m_szScriptCode;
	QString m_szVisibleName;
	QString m_szDescription;
	QString m_szCategory;
	QString m_szBigIcon;
	QString m_szSmallIcon;
	QString m_szKeySequence;
	unsigned int m_uFlags;
	ActionEditorTreeWidgetItem * m_pItem; // ummigarba tanto...
public:
	ActionData(const QString & szName,
	    const QString & szScriptCode,
	    const QString & szVisibleName,
	    const QString & szDescription,
	    const QString & szCategory,
	    const QString & szBigIcon,
	    const QString & szSmallIcon,
	    unsigned int uFlags,
	    const QString & szKeySequence,
	    ActionEditorTreeWidgetItem * pItem)
	    : m_szName(szName), m_szScriptCode(szScriptCode), m_szVisibleName(szVisibleName),
	      m_szDescription(szDescription), m_szCategory(szCategory), m_szBigIcon(szBigIcon),
	      m_szSmallIcon(szSmallIcon), m_szKeySequence(szKeySequence), m_uFlags(uFlags),
	      m_pItem(pItem){};
};

class ActionEditorTreeWidgetItem : public QTreeWidgetItem
{
public:
	ActionEditorTreeWidgetItem(QTreeWidget * v, ActionData * a);
	~ActionEditorTreeWidgetItem();

protected:
	ActionData * m_pActionData;
	QTreeWidget * m_pTreeWidget;
	QString m_szKey;

public:
	ActionData * actionData() { return m_pActionData; };
	//void setupForActionData();
public:
	//virtual void paintCell(QPainter * p,const QColorGroup & cg,int column,int width,int align);
	//virtual void setup();
	virtual QString key(int, bool) const;
};

class ActionEditorTreeView : public QTreeWidget
{
	Q_OBJECT
public:
	ActionEditorTreeView(QWidget * pParent);
	~ActionEditorTreeView();

protected:
	void resizeEvent(QResizeEvent * e) override;
};

class SingleActionEditor : public QWidget
{
	Q_OBJECT
public:
	SingleActionEditor(QWidget * par, ActionEditor * ed);
	~SingleActionEditor();

protected:
	ActionEditor * m_pActionEditor;
	ActionData * m_pActionData;
	QLineEdit * m_pNameEdit;
	QLineEdit * m_pVisibleNameEdit;
	QComboBox * m_pCategoryCombo;
	KviScriptEditor * m_pScriptEditor;
	QLineEdit * m_pKeySequenceEdit;
	QLineEdit * m_pDescriptionEdit;
	QLineEdit * m_pSmallIconEdit;
	QLineEdit * m_pBigIconEdit;
	QToolButton * m_pSmallIconButton;
	QToolButton * m_pBigIconButton;
	QCheckBox * m_pNeedsContextCheck;
	QCheckBox * m_pNeedsConnectionCheck;
	QCheckBox * m_pEnableAtLoginCheck;
	QCheckBox * m_pSpecificWindowsCheck;
	QCheckBox * m_pWindowConsoleCheck;
	QCheckBox * m_pWindowChannelCheck;
	QCheckBox * m_pWindowQueryCheck;
	QCheckBox * m_pWindowDccChatCheck;
	QCheckBox * m_pConsoleOnlyIfUsersSelectedCheck;
	QCheckBox * m_pChannelOnlyIfUsersSelectedCheck;
	QCheckBox * m_pQueryOnlyIfUsersSelectedCheck;

public:
	ActionData * actionData() { return m_pActionData; };
	void setActionData(ActionData * d);
	void commit();
protected slots:
	void chooseSmallIcon();
	void chooseBigIcon();
	void displaySmallIcon(const QString & szIconId);
	void displayBigIcon(const QString & szIconId);
	void needsContextCheckToggled(bool);
	void needsConnectionCheckToggled(bool);
	void specificWindowsCheckToggled(bool);
	void channelWindowsCheckToggled(bool);
	void consoleWindowsCheckToggled(bool);
	void queryWindowsCheckToggled(bool);
};

class ActionEditor : public QWidget
{
	friend class ActionEditorWindow;
	Q_OBJECT
public:
	ActionEditor(QWidget * par);
	~ActionEditor();

protected:
	ActionEditorTreeView * m_pTreeWidget;
	SingleActionEditor * m_pSingleActionEditor;
	QSplitter * m_pSplitter;
	QPushButton * m_pNewActionButton;
	QPushButton * m_pDeleteActionsButton;
	QPushButton * m_pExportActionsButton;

public:
	void commit();
	bool actionExists(const QString & szName);

protected:
	int selectedCount();
	QString nameForAutomaticAction(const QString & szTemplate);
protected slots:
	void currentItemChanged(QTreeWidgetItem * it, QTreeWidgetItem * prev);
	void newAction();
	void deleteActions();
	void exportActions();
};

class ActionEditorWindow : public KviWindow
{
	Q_OBJECT
public:
	ActionEditorWindow();
	~ActionEditorWindow();

protected:
	ActionEditor * m_pEditor;

protected:
	QPixmap * myIconPtr() override;
	void getConfigGroupName(QString & szName) override;
	void saveProperties(KviConfigurationFile *) override;
	void loadProperties(KviConfigurationFile *) override;
protected slots:
	void cancelClicked();
	void okClicked();
	void applyClicked();
};

#endif //_ACTIONEDITOR_H_
