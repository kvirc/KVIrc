#ifndef _ACTIONEDITOR_H_
#define _ACTIONEDITOR_H_
//=============================================================================
//
//   File : actioneditor.h
//   Creation date : Tue Dec 29 2004 02:40:55 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2004-2008 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the linkss of the GNU General Public License
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

#include "kvi_window.h"
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
class KviActionEditorTreeWidgetItem;
class KviScriptEditor;
class KviActionEditor;


class KviActionData
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
	KviActionEditorTreeWidgetItem * m_pItem; // ummigarba tanto...
public:
	KviActionData(const QString &szName,
		const QString &szScriptCode,
		const QString &szVisibleName,
		const QString &szDescription,
		const QString &szCategory,
		const QString &szBigIcon,
		const QString &szSmallIcon,
		unsigned int uFlags,
		const QString &szKeySequence,
		KviActionEditorTreeWidgetItem * pItem)
	: m_szName(szName), m_szScriptCode(szScriptCode), m_szVisibleName(szVisibleName),
		m_szDescription(szDescription), m_szCategory(szCategory), m_szBigIcon(szBigIcon),
		m_szSmallIcon(szSmallIcon), m_szKeySequence(szKeySequence), m_uFlags(uFlags),
		m_pItem(pItem)
	{};
};


class KviActionEditorTreeWidgetItem : public QTreeWidgetItem
{
public:
	KviActionEditorTreeWidgetItem(QTreeWidget * v,KviActionData * a);
	~KviActionEditorTreeWidgetItem();
protected:
	KviActionData * m_pActionData;
	//QSimpleRichText * m_pText;
	QTextDocument * m_pText;
	QPixmap * m_pIcon;
	QTreeWidget * m_pTreeWidget;
	QString m_szKey;
public:
	KviActionData * actionData(){ return m_pActionData; };
	//void setupForActionData();
public:
	//virtual void paintCell(QPainter * p,const QColorGroup & cg,int column,int width,int align);
	//virtual void setup();
	virtual QString key(int,bool) const;
};


class KviActionEditorTreeView : public QTreeWidget
{
	Q_OBJECT
public:
	KviActionEditorTreeView(QWidget * pParent);
	~KviActionEditorTreeView();
protected:
	virtual void resizeEvent(QResizeEvent * e);
};


class KviSingleActionEditor : public QWidget
{
	Q_OBJECT
public:
	KviSingleActionEditor(QWidget * par,KviActionEditor * ed);
	~KviSingleActionEditor();
protected:
	KviActionEditor * m_pActionEditor;
	KviActionData * m_pActionData;
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
	KviActionData * actionData(){ return m_pActionData; };
	void setActionData(KviActionData * d);
	void commit();
protected slots:
	void chooseSmallIcon();
	void chooseBigIcon();
	void displaySmallIcon(const QString &szIconId);
	void displayBigIcon(const QString &szIconId);
	void needsContextCheckToggled(bool);
	void needsConnectionCheckToggled(bool);
	void specificWindowsCheckToggled(bool);
	void channelQueryOrConsoleWindowCheckToggled(bool);
};


class KviActionEditor : public QWidget
{
	friend class KviActionEditorWindow;
	Q_OBJECT
public:
	KviActionEditor(QWidget * par);
	~KviActionEditor();
protected:
	KviActionEditorTreeView * m_pTreeWidget;
	KviSingleActionEditor * m_pSingleActionEditor;
	QSplitter * m_pSplitter;
	QPushButton * m_pNewActionButton;
	QPushButton * m_pDeleteActionsButton;
	QPushButton * m_pExportActionsButton;
public:
	void commit();
	bool actionExists(const QString &szName);
protected:
	int selectedCount();
	QString nameForAutomaticAction(const QString &szTemplate);
protected slots:
	void currentItemChanged(QTreeWidgetItem * it,QTreeWidgetItem *prev);
	void newAction();
	void deleteActions();
	void exportActions();
};


class KviActionEditorWindow : public KviWindow
{
	Q_OBJECT
public:
	KviActionEditorWindow(KviFrame * lpFrm);
	~KviActionEditorWindow();
protected:
	KviActionEditor * m_pEditor;
protected:
	virtual QPixmap * myIconPtr();
	virtual void getConfigGroupName(QString &szName);
	virtual void saveProperties(KviConfig *);
	virtual void loadProperties(KviConfig *);
protected slots:
	void cancelClicked();
	void okClicked();
	void applyClicked();
};

#endif //_ACTIONEDITOR_H_
