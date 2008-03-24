#ifndef _ACTIONEDITOR_H_
#define _ACTIONEDITOR_H_
//=============================================================================
//
//   File : actioneditor.h
//   Creation date : Tue Dec 29 2004 02:40:55 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2004 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#include "kvi_window.h"

#include <qwidget.h>
#include "kvi_tal_listview.h"
#include "kvi_styled_controls.h"

class KviActionEditorListViewItem;

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
	KviActionEditorListViewItem * m_pItem; // ummigarba tanto...
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
		KviActionEditorListViewItem * pItem)
	: m_szName(szName), m_szScriptCode(szScriptCode), m_szVisibleName(szVisibleName),
		m_szDescription(szDescription), m_szCategory(szCategory), m_szBigIcon(szBigIcon),
		m_szSmallIcon(szSmallIcon), m_szKeySequence(szKeySequence), m_uFlags(uFlags),
		m_pItem(pItem)
	{};
};


class QLineEdit;
class QComboBox;
class KviScriptEditor;
class QToolButton;
class QSimpleRichText;
class QPushButton;

#ifdef COMPILE_USE_QT4
	#include <q3simplerichtext.h>
	#define QSimpleRichText Q3SimpleRichText
#else
	#include <qsimplerichtext.h>
#endif



class KviActionEditorListViewItem : public KviTalListViewItem
{
public:
	KviActionEditorListViewItem(KviTalListView * v,KviActionData * a);
	~KviActionEditorListViewItem();
protected:
	KviActionData * m_pActionData;
	QSimpleRichText * m_pText;
	QPixmap * m_pIcon;
	KviTalListView * m_pListView;
	QString m_szKey;
public:
	KviActionData * actionData(){ return m_pActionData; };
	void setupForActionData();
public:
	virtual void paintCell(QPainter * p,const QColorGroup & cg,int column,int width,int align);
	virtual void setup();
	virtual QString key(int,bool) const;
};


class KviActionEditorListView : public KviTalListView
{
	Q_OBJECT
public:
	KviActionEditorListView(QWidget * pParent);
	~KviActionEditorListView();
protected:
	virtual void resizeEvent(QResizeEvent * e);
};

class KviActionEditor;

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
	KviStyledCheckBox * m_pNeedsContextCheck;
	KviStyledCheckBox * m_pNeedsConnectionCheck;
	KviStyledCheckBox * m_pEnableAtLoginCheck;
	KviStyledCheckBox * m_pSpecificWindowsCheck;
	KviStyledCheckBox * m_pWindowConsoleCheck;
	KviStyledCheckBox * m_pWindowChannelCheck;
	KviStyledCheckBox * m_pWindowQueryCheck;
	KviStyledCheckBox * m_pWindowDccChatCheck;
	KviStyledCheckBox * m_pConsoleOnlyIfUsersSelectedCheck;
	KviStyledCheckBox * m_pChannelOnlyIfUsersSelectedCheck;
	KviStyledCheckBox * m_pQueryOnlyIfUsersSelectedCheck;
public:
	KviActionData * actionData(){ return m_pActionData; };
	void setActionData(KviActionData * d);
	void commit();
protected slots:
	void chooseSmallIcon();
	void chooseBigIcon();
	void needsContextCheckToggled(bool);
	void needsConnectionCheckToggled(bool);
	void specificWindowsCheckToggled(bool);
	void channelQueryOrConsoleWindowCheckToggled(bool);
};

class QSplitter;

class KviActionEditor : public QWidget
{
	friend class KviActionEditorWindow;
	Q_OBJECT
public:
	KviActionEditor(QWidget * par);
	~KviActionEditor();
protected:
	KviActionEditorListView * m_pListView;
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
	void currentChanged(KviTalListViewItem * it);
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
	QWidget         * m_pBase;
protected:
	virtual QPixmap * myIconPtr();
	virtual void resizeEvent(QResizeEvent *e);
	virtual void getConfigGroupName(KviStr &szName);
	virtual void saveProperties(KviConfig *);
	virtual void loadProperties(KviConfig *);
protected slots:
	void cancelClicked();
	void okClicked();
	void applyClicked();
};

#endif //_ACTIONEDITOR_H_
