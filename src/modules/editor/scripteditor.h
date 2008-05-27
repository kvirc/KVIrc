#ifndef _SCRIPTEDITOR_H_
#define _SCRIPTEDITOR_H_

//=============================================================================
//
//   File : scripteditor.h
//   Created on Sun Mar 28 1999 16:11:48 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2004 Szymon Stefanek <pragma at kvirc dot net>
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

#include "kvi_scripteditor.h"
#include "kvi_pointerlist.h"
#include "kvi_selectors.h"
#include "kvi_qcstring.h"
#include <kvi_tal_textedit.h>
#include "kvi_tal_popupmenu.h"
#include "kvi_tal_listbox.h"

#include <QLabel>
#include <QDialog>
#include "kvi_styled_controls.h"
#include <QEvent>
#include <QTextEdit>
#include <QListWidget>

// Q3PopupMenu

#include <QSyntaxHighlighter>

class QTimer;

typedef KviPointerList<int> ColumnList;

class KviCompletionBox: public QListWidget
{
	Q_OBJECT
public:
	KviCompletionBox(QTextEdit * parent);
	~KviCompletionBox(){};
	
	void updateContents(QString word);
protected:
	QTextEdit *m_pTextEdit;
	virtual void keyPressEvent(QKeyEvent * e);
};

class KviScriptEditorWidget : public QTextEdit
{
	Q_OBJECT
	Q_PROPERTY(bool contextSensitiveHelp READ contextSensitiveHelp)
public:
	KviScriptEditorWidget(QWidget * pParent);
	virtual ~KviScriptEditorWidget();
public:
	void updateOptions();
	void find1();
	QString m_szFind;
	KviCompletionBox *completelistbox;
	void completition(bool bCanComplete = 1);
	void getWordBeforeCursor(QString &buffer,int index,bool *);
	void getWordOnCursor(QString &buffer,int index) const;
	bool contextSensitiveHelp() const;
public slots:
	void slotFind();
	void slotHelp();
	void slotReplace();
	void slotComplete(QListWidgetItem *);

signals:
	void keyPressed();
protected:
	void contextMenuEvent(QContextMenuEvent *event);
	virtual void keyPressEvent(QKeyEvent * e);
	void mouseReleaseEvent(QMouseEvent *);
	QWidget *m_pParent;
	QString m_szHelp;
};

class KviScriptSyntaxHighlighter : public QSyntaxHighlighter
{
public:
	KviScriptSyntaxHighlighter(KviScriptEditorWidget * pWidget);
	virtual ~KviScriptSyntaxHighlighter();
public:
	QTextEdit *m_pTextEdit;
	QTextEdit *textEdit(){return m_pTextEdit;}
	void highlightBlock(const QString &text);
};

class KviScriptEditorWidgetColorOptions : public QDialog
{
	Q_OBJECT
public:
	KviScriptEditorWidgetColorOptions(QWidget * pParent);
	~KviScriptEditorWidgetColorOptions();
private:
	KviPointerList<KviSelectorInterface> * m_pSelectorInterfaceList;
protected:
	KviColorSelector * addColorSelector(QWidget * pParent,const QString & txt,QColor * pOption,bool bEnabled);

protected slots:
	void okClicked();
};

class KviScriptEditorImplementation : public KviScriptEditor
{
	Q_OBJECT
public:
	KviScriptEditorImplementation(QWidget * par);
	virtual ~KviScriptEditorImplementation();
protected:
	KviScriptEditorWidget * m_pEditor;
	QLabel                * m_pRowColLabel;
	int                  m_lastCursorPos;
public:
	virtual void setText(const QString &txt);
	virtual void getText(QString &txt);
	virtual void setText(const KviQCString &txt);
	virtual void getText(KviQCString &txt);
	virtual void setFindText(const QString & text);
	virtual void setEnabled(bool bEnabled);
	virtual void setFocus();
	virtual bool isModified();
	void setFindLineeditReadOnly(bool b);
	void setCursorPosition(int);
	int getCursor();
	QLineEdit *m_pFindlineedit;
	QLineEdit * getFindlineedit();
protected:
	virtual void focusInEvent(QFocusEvent *e);
	void loadOptions();
	void saveOptions();
protected slots:
	void saveToFile();
	void loadFromFile();
	void configureColors();
	void updateRowColLabel();
	void slotFind();
	void slotReplaceAll(const QString &,const QString &);
	void slotInitFind();
	void slotNextFind(const QString &);
signals:
	void find( const QString &);
	void replaceAll( const QString &, const QString &);
	void initFind();
	void nextFind(const QString &);
};

class KviScriptEditorReplaceDialog: public QDialog
{
	Q_OBJECT
public:
	KviScriptEditorReplaceDialog( QWidget* parent = 0, const char* name = 0);
	~KviScriptEditorReplaceDialog();
	QLineEdit *m_pFindlineedit;
	QLineEdit *m_pReplacelineedit;
protected:
	QLabel *findlabel;
	QLabel *replacelabel;
	QPushButton *replacebutton;
	QPushButton *replace;
	QPushButton *findNext;

//	QPushButton *cancelbutton;
	KviStyledCheckBox *checkReplaceAll;
	QWidget *m_pParent;
protected slots:
	void textChanged(const QString &);
	void slotReplace();
	void slotNextFind();
signals:
	void replaceAll( const QString &,const QString &);
	void initFind();
	void nextFind(const QString &);
};


#endif //!_SCRIPTEDITOR_H_
