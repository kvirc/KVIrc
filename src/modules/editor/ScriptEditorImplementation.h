#ifndef _SCRIPTEDITOR_H_
#define _SCRIPTEDITOR_H_

//=============================================================================
//
//   File : ScriptEditorImplementation.h
//   Creation date : Sun Mar 28 1999 16:11:48 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviScriptEditor.h"
#include "KviPointerList.h"
#include "KviSelectors.h"

#include <QCompleter>
#include <QLabel>
#include <QDialog>
#include <QEvent>
#include <QTextEdit>
#include <QListWidget>
#include <QCheckBox>
#include <QSyntaxHighlighter>
#include <QByteArray>
#include <QMenu>

class QTimer;

//typedef KviPointerList<int> ColumnList;
class ScriptEditorSyntaxHighlighter;
class ScriptEditorWidget : public QTextEdit
{
	Q_OBJECT
	Q_PROPERTY(bool contextSensitiveHelp READ contextSensitiveHelp)
public:
	ScriptEditorWidget(QWidget * pParent);
	virtual ~ScriptEditorWidget();
public:
	QString m_szFind;
protected:
	ScriptEditorSyntaxHighlighter * m_pSyntaxHighlighter;
	QCompleter  * m_pCompleter;
	QStringList * m_pListModulesNames;
	QStringList * m_pListCompletition;
	QTimer      * m_pStartTimer;
	QWidget     * m_pParent;
	int iIndex,iModulesCount;
	QString       m_szHelp;
public:
	ScriptEditorSyntaxHighlighter * syntaxHighlighter(){return m_pSyntaxHighlighter;};
	void disableSyntaxHighlighter();
	void enableSyntaxHighlighter();

	void createCompleter(QStringList &list);

	void loadCompleterFromFile();
	QCompleter * completer() const { return m_pCompleter; };
	QString textUnderCursor() const;
	void updateOptions();
	//void find1();
	bool contextSensitiveHelp() const;
public slots:
	void checkReadyCompleter();
	void insertCompletion(const QString & szCompletion);
	void slotFind();
	void slotHelp();
	void slotReplace();
protected slots:
	void asyncCompleterCreation();
signals:
	void keyPressed();
protected:
	void contextMenuEvent(QContextMenuEvent * e);
	virtual void keyPressEvent(QKeyEvent * e);
// 	void mouseReleaseEvent(QMouseEvent * e);
};

class ScriptEditorWidgetColorOptions : public QDialog
{
	Q_OBJECT
public:
	ScriptEditorWidgetColorOptions(QWidget * pParent);
	~ScriptEditorWidgetColorOptions();
private:
	KviPointerList<KviSelectorInterface> * m_pSelectorInterfaceList;
protected:
	KviColorSelector * addColorSelector(QWidget * pParent,const QString & txt,QColor * pOption,bool bEnabled);

protected slots:
	void okClicked();

};

class ScriptEditorSyntaxHighlighter : public QSyntaxHighlighter
{
public:
	ScriptEditorSyntaxHighlighter(ScriptEditorWidget * pWidget);
	virtual ~ScriptEditorSyntaxHighlighter();
public:
	QTextEdit * m_pTextEdit;
public:
	QTextEdit * textEdit(){ return m_pTextEdit; }
	void highlightBlock(const QString & szText);
	void updateSyntaxtTextFormat();
private:
	struct KviScriptHighlightingRule
	{
		QRegExp pattern;
		QTextCharFormat format;
	};

	QVector<KviScriptHighlightingRule> highlightingRules;
	QRegExp commentStartExpression;
	QRegExp commentEndExpression;

	QTextCharFormat bracketFormat;
	QTextCharFormat punctuationFormat;
	QTextCharFormat keywordFormat;
	QTextCharFormat variableFormat;
	QTextCharFormat normaltextFormat;
	QTextCharFormat findFormat;
	QTextCharFormat functionFormat;
	QTextCharFormat commentFormat;
};

class ScriptEditorImplementation : public KviScriptEditor
{
	Q_OBJECT
public:
	ScriptEditorImplementation(QWidget * par);
	virtual ~ScriptEditorImplementation();
public:
	QLineEdit * m_pFindLineEdit;
protected:
	ScriptEditorWidgetColorOptions * m_pOptionsDialog;
	ScriptEditorWidget * m_pEditor;
	QLabel                * m_pRowColLabel;
	int                     m_lastCursorPos;
public:
	virtual void setText(const char * txt);
	virtual void setText(const QString & szText);
	virtual void setText(const QByteArray & szText);
	virtual void getText(QString & szText);
	virtual void getText(QByteArray & szText);
	virtual void setFindText(const QString & szText);
	virtual void setEnabled(bool bEnabled);
	virtual void setReadOnly(bool bReadOnly);
	void setUnHighlightedText(const QString & szText);


	virtual void setFocus();
	virtual bool isModified();
	virtual void setModified(bool);

	int getCursor(){return m_pEditor->textCursor().position();};
	void setCursorPosition(int iPos);
	int cursor(){ return m_lastCursorPos; };
	QLineEdit * findLineEdit(){ return m_pFindLineEdit; };
protected:
	virtual void focusInEvent(QFocusEvent * e);
	void loadOptions();
	void saveOptions();
protected slots:
	void saveToFile();
	void loadFromFile();
	void configureColors();
	void updateRowColLabel();
	void slotFind();
	void slotReplaceAll(const QString & szToReplace, const QString & szReplaceWith);
	void slotInitFind();
	void slotNextFind(const QString &);
	void optionsDialogFinished(int iResult);
signals:
	void find(const QString &);
	void replaceAll(const QString & szToReplace, const QString & szReplaceWith);
	void initFind();
	void nextFind(const QString & szText);
};

class ScriptEditorReplaceDialog: public QDialog
{
	Q_OBJECT
public:
	ScriptEditorReplaceDialog(QWidget * parent = 0, const QString & szName = QString());
	~ScriptEditorReplaceDialog();
public:
	QLineEdit * m_pFindLineEdit;
	QLineEdit * m_pReplaceLineEdit;
protected:
	QPushButton * m_pReplaceButton;
	QCheckBox   * m_pCheckReplaceAll;
	QWidget     * m_pParent;
protected slots:
	void textChanged(const QString &);
	void slotReplace();
	void slotNextFind();
signals:
	void replaceAll(const QString &, const QString &);
	void initFind();
	void nextFind(const QString &);
};

#endif // _SCRIPTEDITOR_H_
