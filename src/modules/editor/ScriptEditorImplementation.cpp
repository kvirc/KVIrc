//=============================================================================
//
//   File : ScriptEditorImplementation.cpp
//   Creation date : Sun Mar 28 1999 16:11:48 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999 Szymon Stefanek <pragma at kvirc dot net>
//   Copyright (C) 2005 Tonino Imbesi
//   Copyright (C) 2005-2008 Alessandro Carbone (elfonol at gmail dot com)
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

#include "ScriptEditorImplementation.h"

#include "KviFileUtils.h"
#include "kvi_fileextensions.h"
#include "KviLocale.h"
#include "KviFileDialog.h"
#include "KviQString.h"
#include "KviConfigurationFile.h"
#include "KviModule.h"
#include "KviApplication.h"
#include "KviConsoleWindow.h"
#include "KviWindow.h"
#include "KviIconManager.h"
#include "KviKvsKernel.h"
#include "KviModuleManager.h"
#include "KviConfigurationFile.h"
#include "KviTalVBox.h"
#include "KviTalHBox.h"
#include "KviTalGroupBox.h"
#include "KviOptions.h"

#include <QScrollBar>
#include <QLayout>
#include <QMessageBox>
#include <QTimer>
#include <QCursor>
#include <QFont>
#include <QRect>
#include <QEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QListWidget>
#include <QPainter>

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
#include <windows.h>
#endif //COMPILE_ON_WINDOWS || COMPILE_ON_MINGW

extern KVIRC_API KviModuleManager * g_pModuleManager;
extern std::set<ScriptEditorImplementation *> g_pScriptEditorWindowList;
extern KviModule * g_pEditorModulePointer;

static QColor g_clrBackground(0, 0, 0);
static QColor g_clrNormalText(100, 255, 0);
static QColor g_clrBracket(255, 0, 0);
static QColor g_clrComment(0, 120, 0);
static QColor g_clrFunction(255, 255, 0);
static QColor g_clrKeyword(120, 120, 150);
static QColor g_clrVariable(200, 200, 200);
static QColor g_clrPunctuation(180, 180, 0);
static QColor g_clrFind(255, 0, 0);

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
#define KVI_SCRIPTEDITOR_DEFAULT_FONT "Courier New"
#define KVI_SCRIPTEDITOR_DEFAULT_FONT_SIZE 8
#elif defined(COMPILE_ON_MAC)
#define KVI_SCRIPTEDITOR_DEFAULT_FONT "Monaco"
#define KVI_SCRIPTEDITOR_DEFAULT_FONT_SIZE 10
#else
#define KVI_SCRIPTEDITOR_DEFAULT_FONT "Monospace"
#define KVI_SCRIPTEDITOR_DEFAULT_FONT_SIZE 8
#endif
static QFont g_fntNormal(KVI_SCRIPTEDITOR_DEFAULT_FONT, KVI_SCRIPTEDITOR_DEFAULT_FONT_SIZE);

static bool bSemaphore = false;
static bool bCompleterReady = false;

ScriptEditorWidget::ScriptEditorWidget(QWidget * pParent)
    : QTextEdit(pParent)
{
	m_pSyntaxHighlighter = nullptr;
	setTabStopWidth(48);
	setAcceptRichText(false);
	setWordWrapMode(QTextOption::NoWrap);
	m_pParent = pParent;
	m_szHelp = "Nothing";
	updateOptions();
	m_szFind = "";
	m_pCompleter = nullptr;
	QStringList szListFunctionsCommands;
	QString tmp("kvscompleter.idx");
	iModulesCount = 0;
	iIndex = 0;
	QString szPath;
	g_pApp->getLocalKvircDirectory(szPath, KviApplication::ConfigPlugins, tmp);

	if(!KviFileUtils::fileExists(szPath))
	{
		if(!bSemaphore)
		{
			bSemaphore = true;
			m_pStartTimer = new QTimer();
			m_pStartTimer->setInterval(1000);
			connect(m_pStartTimer, SIGNAL(timeout()), this, SLOT(asyncCompleterCreation()));
			m_pStartTimer->start(500);
		}
		else
		{
			m_pStartTimer = new QTimer();
			m_pStartTimer->setInterval(2000);
			connect(m_pStartTimer, SIGNAL(timeout()), this, SLOT(checkReadyCompleter()));
			m_pStartTimer->start(1000);
		}
	}
	else
		loadCompleterFromFile();
}

ScriptEditorWidget::~ScriptEditorWidget()
{
	if(m_pCompleter)
		delete m_pCompleter;
	disableSyntaxHighlighter();
}

void ScriptEditorWidget::checkReadyCompleter()
{
	if(bCompleterReady)
	{
		m_pStartTimer->stop();
		delete m_pStartTimer;
		m_pStartTimer = nullptr;
		loadCompleterFromFile();
	}
}

void ScriptEditorWidget::asyncCompleterCreation()
{
	//static int iIndex = 0;
	//static int iModulesCount = 0;
	if(!iIndex)
	{
		m_pListCompletition = new QStringList();
		QString szPath;

		g_pApp->getGlobalKvircDirectory(szPath, KviApplication::Modules);

		QDir d(szPath);

#if defined(COMPILE_ON_WINDOWS)
		d.setNameFilters(QStringList("kvi*.dll"));
#elif defined(COMPILE_ON_MINGW)
		d.setNameFilters(QStringList("libkvi*.dll"));
#else
		d.setNameFilters(QStringList("libkvi*.so"));
#endif
		m_pListModulesNames = new QStringList(d.entryList(QDir::Files | QDir::Readable));
		iModulesCount = m_pListModulesNames->count();
	}
	QString szModuleName = m_pListModulesNames->at(iIndex);
	iIndex++;

#if defined(COMPILE_ON_WINDOWS)
	szModuleName = szModuleName.replace("kvi", "");
	szModuleName = szModuleName.replace(".dll", "");
#elif defined(COMPILE_ON_MINGW)
	szModuleName = szModuleName.replace("libkvi", "");
	szModuleName = szModuleName.replace(".dll", "");
#else
	szModuleName = szModuleName.replace("libkvi", "");
	szModuleName = szModuleName.replace(".so", "");
#endif

	KviModule * pModule = g_pModuleManager->getModule(szModuleName);
	if(pModule)
	{
		pModule->getAllFunctionsCommandsModule(m_pListCompletition, szModuleName);
	}

	if(iIndex == iModulesCount)
	{
		m_pStartTimer->stop();
		m_pStartTimer->deleteLater();
		m_pStartTimer = nullptr;
		QString szTmp("kvscompleter.idx");
		QString szPath;
		g_pApp->getLocalKvircDirectory(szPath, KviApplication::ConfigPlugins, szTmp);
		KviKvsKernel::instance()->getAllFunctionsCommandsCore(m_pListCompletition);
		QString szBuffer = m_pListCompletition->join(",");
		QFile f(szPath);
		f.open(QIODevice::WriteOnly);
		f.write(szBuffer.toUtf8());
		f.close();
		createCompleter(*m_pListCompletition);
		iIndex = 0;
		iModulesCount = 0;
		bCompleterReady = true;
		delete m_pListCompletition;
		delete m_pListModulesNames;
	}
}

void ScriptEditorWidget::loadCompleterFromFile()
{
	QStringList szListFunctionsCommand;
	QString szTmp("kvscompleter.idx");
	QString szPath;
	g_pApp->getLocalKvircDirectory(szPath, KviApplication::ConfigPlugins, szTmp);
	QString szBuffer;
	QFile f(szPath);
	f.open(QIODevice::ReadOnly);
	szBuffer = f.readAll();
	f.close();
	szListFunctionsCommand = szBuffer.split(',');
	createCompleter(szListFunctionsCommand);
}

void ScriptEditorWidget::createCompleter(QStringList & list)
{
	m_pCompleter = new QCompleter(list);
	m_pCompleter->setCaseSensitivity(Qt::CaseInsensitive);
	m_pCompleter->setWrapAround(false);
	m_pCompleter->setWidget(this);
	m_pCompleter->setModelSorting(QCompleter::CaseSensitivelySortedModel);
	m_pCompleter->setCompletionMode(QCompleter::PopupCompletion);
	m_pCompleter->setCaseSensitivity(Qt::CaseInsensitive);
	QObject::connect(m_pCompleter, SIGNAL(activated(const QString &)), this, SLOT(insertCompletion(const QString &)));
}

void ScriptEditorWidget::insertCompletion(const QString & szCompletion)
{
	QTextCursor tc = textCursor();
	int iExtra = szCompletion.length() - m_pCompleter->completionPrefix().length();

	tc.movePosition(QTextCursor::Left);
	tc.movePosition(QTextCursor::EndOfWord);

	QString szTmp = szCompletion.right(iExtra);
	if(szCompletion.left(1) == "$")
		szTmp.append("(");
	else
		szTmp.append(" ");

	tc.insertText(szTmp);
	setTextCursor(tc);
}

void ScriptEditorWidget::contextMenuEvent(QContextMenuEvent * e)
{
	QMenu * pMenu = createStandardContextMenu();
	pMenu->addAction(__tr2qs_ctx("Context Sensitive Help", "editor"), this, SLOT(slotHelp()), Qt::CTRL + Qt::Key_H);
	pMenu->addAction(__tr2qs_ctx("&Replace", "editor"), this, SLOT(slotReplace()), Qt::CTRL + Qt::Key_R);
	pMenu->exec(e->globalPos());
	delete pMenu;
}

void ScriptEditorWidget::slotFind()
{
	m_szFind = ((ScriptEditorImplementation *)m_pParent)->findLineEdit()->text();
	setText(toPlainText());
}

void ScriptEditorWidget::slotReplace()
{
	ScriptEditorReplaceDialog * pDialog = new ScriptEditorReplaceDialog(this, __tr2qs_ctx("Find & Replace", "editor"));
	connect(pDialog, SIGNAL(replaceAll(const QString &, const QString &)), m_pParent, SLOT(slotReplaceAll(const QString &, const QString &)));
	connect(pDialog, SIGNAL(initFind()), m_pParent, SLOT(slotInitFind()));
	connect(pDialog, SIGNAL(nextFind(const QString &)), m_pParent, SLOT(slotNextFind(const QString &)));

	if(pDialog->exec())
	{
	};
}

void ScriptEditorWidget::slotHelp()
{
	contextSensitiveHelp();
}
void ScriptEditorWidget::enableSyntaxHighlighter()
{
	// we currently delete and re-create the m_pSyntaxHighlighter
	// as a trick to ensure proper re-highlighting to occur
	if(!m_pSyntaxHighlighter)
		m_pSyntaxHighlighter = new ScriptEditorSyntaxHighlighter(this);
}
void ScriptEditorWidget::disableSyntaxHighlighter()
{
	if(m_pSyntaxHighlighter)
		delete m_pSyntaxHighlighter;
	m_pSyntaxHighlighter = nullptr;
}

void ScriptEditorWidget::updateOptions()
{
	QPalette p = palette();
	p.setColor(QPalette::Base, g_clrBackground);
	p.setColor(QPalette::Text, g_clrNormalText);
	setPalette(p);
	setFont(g_fntNormal);
	setTextColor(g_clrNormalText);
	disableSyntaxHighlighter();
	enableSyntaxHighlighter();

	//set cursor custom width
	if(KVI_OPTION_BOOL(KviOption_boolEnableCustomCursorWidth))
	{
		setCursorWidth(KVI_OPTION_UINT(KviOption_uintCustomCursorWidth));
	}
	else
	{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
		int iCursorWidth = 1;
		SystemParametersInfo(SPI_GETCARETWIDTH, 0, &iCursorWidth, 0);
		setCursorWidth(iCursorWidth);
#endif //COMPILE_ON_WINDOWS || COMPILE_ON_MINGW
	}
}

void ScriptEditorWidget::keyPressEvent(QKeyEvent * e)
{
	if(m_pCompleter && m_pCompleter->popup()->isVisible())
	{
		switch(e->key())
		{
			case Qt::Key_Enter:
			case Qt::Key_Return:
			case Qt::Key_Escape:
			case Qt::Key_Tab:
			case Qt::Key_Backtab:
				e->ignore();
				return;
			default:
				break;
		}
	}

	if(e->modifiers() == Qt::ControlModifier)
	{
		switch(e->key())
		{
			case Qt::Key_B:
				insertPlainText("$b");
				return;
			case Qt::Key_I:
				insertPlainText("$i");
				return;
			case Qt::Key_K:
				insertPlainText("$k");
				return;
			case Qt::Key_O:
				insertPlainText("$o");
				return;
			case Qt::Key_U:
				insertPlainText("$u");
				return;
			case Qt::Key_Enter:
			case Qt::Key_Return:
			case Qt::Key_PageUp:
				e->ignore(); // allow the parent to process it
				return;
				break;
		}
	}
	// Adapted from QT4 QCompleter example
	bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
	if(!m_pCompleter || !isShortcut)                                                     // don't process the shortcut when we have a completer
		QTextEdit::keyPressEvent(e);
	const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
	if(!m_pCompleter || (ctrlOrShift && e->text().isEmpty()))
		return;
	static QString eow("~!@#$%^&*()_+{}|:\"<>?,/;'[]\\-="); // end of word
	bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
	QString completionPrefix = textUnderCursor();
	if(!isShortcut && (hasModifier || e->text().isEmpty() || completionPrefix.length() < 3 || eow.contains(e->text().right(1))))
	{
		m_pCompleter->popup()->hide();
		return;
	}
	if(completionPrefix != m_pCompleter->completionPrefix())
	{
		m_pCompleter->setCompletionPrefix(completionPrefix);
		m_pCompleter->popup()->setCurrentIndex(m_pCompleter->completionModel()->index(0, 0));
	}
	QRect cr = cursorRect();
	cr.setWidth(m_pCompleter->popup()->sizeHintForColumn(0) + m_pCompleter->popup()->verticalScrollBar()->sizeHint().width());
	m_pCompleter->complete(cr);
}

QString ScriptEditorWidget::textUnderCursor() const
{
	QString szWord;
	QTextCursor tc = textCursor();
	if(tc.atBlockStart())
		return QString();
	tc.clearSelection();
	tc.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);
	if(tc.atBlockStart())
	{
		szWord.append(tc.selectedText());
		tc.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
		szWord.append(tc.selectedText());
		if(tc.atBlockEnd())
		{
			return szWord;
		}
		tc.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
		szWord.append(tc.selectedText());
		if(szWord.right(1) != ".")
			szWord.chop(1);
		return szWord;
	}

	tc.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
	szWord = tc.selectedText();
	if(szWord.left(1) == ".")
	{
		tc.movePosition(QTextCursor::StartOfWord);
		tc.movePosition(QTextCursor::PreviousCharacter);
		tc.movePosition(QTextCursor::PreviousWord);
		tc.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor, 1);
		szWord.prepend(tc.selectedText());
	}
	else
		szWord.remove(0, 1);
	return szWord;
}

bool ScriptEditorWidget::contextSensitiveHelp() const
{

	QRect r = cursorRect();
	QTextCursor cur = cursorForPosition(QPoint(r.x(), r.y()));
	cur.select(QTextCursor::WordUnderCursor);
	QString szText = cur.selectedText();
	QString szTmp = szText;

	return true;
}

ScriptEditorWidgetColorOptions::ScriptEditorWidgetColorOptions(QWidget * pParent)
    : QDialog(pParent)
{
	setWindowTitle(__tr2qs_ctx("Editor Configuration - KVIrc", "editor"));

	QGridLayout * g = new QGridLayout(this);
	KviTalVBox * box = new KviTalVBox(this);
	g->addWidget(box, 0, 0);
	box->setMargin(0);
	box->setSpacing(0);
	box->setMinimumWidth(280);

	KviFontSelector * f = new KviFontSelector(box, __tr2qs_ctx("Font:", "editor"), &g_fntNormal, true);
	m_pSelectorInterfaceList.push_back(f);
	KviTalGroupBox * gbox = new KviTalGroupBox(Qt::Horizontal, __tr2qs_ctx("Colors", "editor"), box);
	gbox->setInsideSpacing(0);

	addColorSelector(gbox, __tr2qs_ctx("Background:", "editor"), &g_clrBackground, true);
	addColorSelector(gbox, __tr2qs_ctx("Normal text:", "editor"), &g_clrNormalText, true);
	addColorSelector(gbox, __tr2qs_ctx("Brackets:", "editor"), &g_clrBracket, true);
	addColorSelector(gbox, __tr2qs_ctx("Comments:", "editor"), &g_clrComment, true);
	addColorSelector(gbox, __tr2qs_ctx("Functions:", "editor"), &g_clrFunction, true);
	addColorSelector(gbox, __tr2qs_ctx("Keywords:", "editor"), &g_clrKeyword, true);
	addColorSelector(gbox, __tr2qs_ctx("Variables:", "editor"), &g_clrVariable, true);
	addColorSelector(gbox, __tr2qs_ctx("Punctuation:", "editor"), &g_clrPunctuation, true);
	addColorSelector(gbox, __tr2qs_ctx("Find:", "editor"), &g_clrFind, true);

	KviTalHBox * hbox = new KviTalHBox(box);

	QPushButton * b = new QPushButton(__tr2qs_ctx("OK", "editor"), hbox);
	b->setDefault(true);
	connect(b, SIGNAL(clicked()), this, SLOT(okClicked()));

	b = new QPushButton(__tr2qs_ctx("Cancel", "editor"), hbox);
	connect(b, SIGNAL(clicked()), this, SLOT(reject()));
}

KviColorSelector * ScriptEditorWidgetColorOptions::addColorSelector(QWidget * pParent, const QString & txt, QColor * pOption, bool bEnabled)
{
	KviColorSelector * s = new KviColorSelector(pParent, txt, pOption, bEnabled);
	m_pSelectorInterfaceList.push_back(s);
	return s;
}

void ScriptEditorWidgetColorOptions::okClicked()
{
	for(const auto & i : m_pSelectorInterfaceList)
		i->commit();

	accept();
}

ScriptEditorSyntaxHighlighter::ScriptEditorSyntaxHighlighter(ScriptEditorWidget * pWidget)
    : QSyntaxHighlighter(pWidget), m_pTextEdit(pWidget)
{
	// code adpated from QT4 example

	// FIX-ME: "function ..." - "function internal ..."
	// FIX-ME: "@$"
	// FIX-ME: "\" escape char
	updateSyntaxtTextFormat();

	KviScriptHighlightingRule rule;

	rule.pattern = QRegExp("([=()[\\]!\"?<>;:.,+-])+");
	rule.format = punctuationFormat;
	highlightingRules.append(rule);

	rule.pattern = QRegExp("[{};](|[a-zA-Z]|[a-zA-Z]+[a-zA-Z0-9_\\.:]*)");
	rule.format = keywordFormat;
	highlightingRules.append(rule);

	rule.pattern = QRegExp("[$](|[a-zA-Z0-9]+[a-zA-Z0-9_\\.:]*)");
	rule.format = functionFormat;
	highlightingRules.append(rule);

	rule.pattern = QRegExp("[%](|[a-zA-Z]|[a-zA-Z]+[a-zA-Z0-9_\\.]*)");
	rule.format = variableFormat;
	highlightingRules.append(rule);

	rule.pattern = QRegExp("([{}])+");
	rule.format = bracketFormat;
	highlightingRules.append(rule);
}

ScriptEditorSyntaxHighlighter::~ScriptEditorSyntaxHighlighter()
    = default;

void ScriptEditorSyntaxHighlighter::updateSyntaxtTextFormat()
{
	punctuationFormat.setForeground(g_clrPunctuation);
	keywordFormat.setForeground(g_clrKeyword);
	functionFormat.setForeground(g_clrFunction);
	variableFormat.setForeground(g_clrVariable);
	bracketFormat.setForeground(g_clrBracket);
	commentFormat.setForeground(g_clrComment);
	findFormat.setForeground(g_clrFind);
}

#define SKIP_SPACES                                                                             \
	while(iIndexStart < szText.size())                                                          \
	{                                                                                           \
		if(szText.at(iIndexStart).unicode() == '\t' || szText.at(iIndexStart).unicode() == ' ') \
		{                                                                                       \
			iIndexStart++;                                                                      \
		}                                                                                       \
		else                                                                                    \
		{                                                                                       \
			break;                                                                              \
		}                                                                                       \
	}

void ScriptEditorSyntaxHighlighter::highlightBlock(const QString & szText)
{
	if(szText.isEmpty())
	{
		setCurrentBlockState(previousBlockState());
		return;
	}

	int iIndexStart = 0;
	setCurrentBlockState(0);

	if(previousBlockState() == 1)
	{
		// inside a multiline comment
		int iCommentEnd = szText.indexOf("*/");
		if(iCommentEnd < 0)
		{
			// not found: comment until the end of the block
			setCurrentBlockState(1);
			setFormat(iIndexStart, szText.length(), commentFormat);
			return;
		}
		// skip 2 chars
		iCommentEnd += 2;
		// end of comment found
		setFormat(iIndexStart, iCommentEnd - iIndexStart, commentFormat);
		iIndexStart = iCommentEnd;
	}

	SKIP_SPACES

	if(iIndexStart == szText.size())
		return;

	// check 'commands' and comments
	int iCommandStart = iIndexStart;
	if(szText.at(iIndexStart).unicode() != '$' && szText.at(iIndexStart).unicode() != '{' && szText.at(iIndexStart).unicode() != '}' && szText.at(iIndexStart).unicode() != '%')
	{
		switch(szText.at(iIndexStart).unicode())
		{
			case '#':
				// single line comment, bash style
				while(iIndexStart < szText.size() && szText.at(iIndexStart).unicode() != '\n')
					iIndexStart++;
				setFormat(iCommandStart, iIndexStart - iCommandStart, commentFormat);
				break;
			case '/':
				if((iIndexStart + 1) < szText.size())
				{
					if(szText.at(iIndexStart + 1).unicode() == '/')
					{
						// single line comment, c++ style
						iIndexStart++;
						while(iIndexStart < szText.size() && szText.at(iIndexStart).unicode() != '\n')
							iIndexStart++;
						setFormat(iCommandStart, iIndexStart - iCommandStart, commentFormat);
						break;
					}

					if(szText.at(iIndexStart + 1).unicode() == '*')
					{
						// multi line comment, c style
						iIndexStart++;
						setCurrentBlockState(1);
						while(iIndexStart < szText.size())
						{
							if((iIndexStart + 1) < szText.size())
							{
								if(szText.at(iIndexStart).unicode() == '*' && szText.at(iIndexStart + 1).unicode() == '/')
								{
									iIndexStart += 2;
									setCurrentBlockState(0);
									break;
								}
							}
							iIndexStart++;
						}
						setFormat(iCommandStart, iIndexStart - iCommandStart, commentFormat);

						SKIP_SPACES

						if(currentBlockState() == 0)
						{
							iCommandStart = iIndexStart;
						}
						else
						{
							break;
						}
						// else fallback to command (a command can start at the end of a /* comment */)
					}
					// else not a comment, fallback to command
				}
			default:
				//command
				while(iIndexStart < szText.size() && (szText.at(iIndexStart).isLetterOrNumber() || (szText.at(iIndexStart).unicode() == '.') || (szText.at(iIndexStart).unicode() == '_') || (szText.at(iIndexStart).unicode() == ':')))
				{
					iIndexStart++;
				}
				setFormat(iCommandStart, iIndexStart - iCommandStart, keywordFormat);
				break;
		}
	}

	// code from QT4 example
	int index = 0;
	foreach(KviScriptHighlightingRule rule, highlightingRules)
	{
		QRegExp expression(rule.pattern);
		QString sz = expression.pattern();

		index = szText.indexOf(expression, iIndexStart);

		while(index >= 0)
		{
			int length = expression.matchedLength();
			setFormat(index, length, rule.format);
			index = szText.indexOf(expression, index + length);
		}
	}

	// 'found matches' highlighting
	ScriptEditorWidget * pEditor = ((ScriptEditorWidget *)textEdit());
	QString szFind = pEditor->m_szFind;
	if(!szFind.isEmpty())
	{
		index = szText.indexOf(szFind, 0, Qt::CaseInsensitive);
		int length = szFind.length();
		while(index >= 0)
		{
			setFormat(index, length, findFormat);
			index = szText.indexOf(szFind, index + length, Qt::CaseInsensitive);
		}
	}
}

ScriptEditorImplementation::ScriptEditorImplementation(QWidget * par)
    : KviScriptEditor(par)
{
	m_pOptionsDialog = nullptr;
	if(g_pScriptEditorWindowList.empty())
		loadOptions();
	g_pScriptEditorWindowList.insert(this);
	m_lastCursorPos = 0;
	QGridLayout * g = new QGridLayout(this);

	m_pEditor = new ScriptEditorWidget(this);

	m_pFindLineEdit = new QLineEdit(" ", this);
	m_pFindLineEdit->setText("");

	QPalette p = m_pFindLineEdit->palette();
	p.setColor(QPalette::Text, g_clrFind);
	m_pFindLineEdit->setPalette(p);

	g->addWidget(m_pEditor, 0, 0, 1, 5);
	g->setRowStretch(0, 1);

	QToolButton * b = new QToolButton(this);
	b->setIcon(*(g_pIconManager->getSmallIcon(KviIconManager::Options)));

	b->setObjectName("ToolButtonEditor");
	b->setMinimumWidth(24);
	b->setText(__tr2qs_ctx("Options", "editor"));
	b->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	g->addWidget(b, 1, 0);

	QMenu * pop = new QMenu(b);
	pop->addAction(__tr2qs_ctx("&Open...", "editor"), this, SLOT(loadFromFile()));
	pop->addAction(__tr2qs_ctx("&Save As...", "editor"), this, SLOT(saveToFile()));
	pop->addSeparator();
	pop->addAction(__tr2qs_ctx("&Configure Editor...", "editor"), this, SLOT(configureColors()));
	b->setMenu(pop);
	b->setPopupMode(QToolButton::InstantPopup);

	g->setColumnStretch(1, 1);
	g->setColumnStretch(2, 10);
	g->addWidget(m_pFindLineEdit, 1, 2);

	QLabel * pLab = new QLabel(this);
	pLab->setText(__tr2qs_ctx("Find:", "editor"));
	pLab->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	g->addWidget(pLab, 1, 1);

	m_pFindButton = new QPushButton(QString(__tr2qs_ctx("&Find", "editor")), this);
	g->addWidget(m_pFindButton, 1, 3);

	m_pRowColLabel = new QLabel(QString(__tr2qs_ctx("Line: %1 Col: %2", "editor")).arg(1).arg(1), this);
	m_pRowColLabel->setFrameStyle(QFrame::Sunken | QFrame::Panel);
	m_pRowColLabel->setMinimumWidth(80);
	g->addWidget(m_pRowColLabel, 1, 4);

	connect(m_pFindLineEdit, SIGNAL(returnPressed()), m_pEditor, SLOT(slotFind()));
	connect(m_pFindLineEdit, SIGNAL(returnPressed()), this, SLOT(slotFind()));
	connect(m_pFindButton, SIGNAL(clicked()), m_pEditor, SLOT(slotFind()));
	connect(m_pFindButton, SIGNAL(clicked()), this, SLOT(slotFind()));
	connect(m_pEditor, SIGNAL(cursorPositionChanged()), this, SLOT(updateRowColLabel()));
	connect(m_pEditor, SIGNAL(selectionChanged()), this, SLOT(updateRowColLabel()));
	m_lastCursorPos = 0;
}

ScriptEditorImplementation::~ScriptEditorImplementation()
{
	if(m_pOptionsDialog)
	{
		m_pOptionsDialog->deleteLater();
		m_pOptionsDialog = nullptr;
	}
	g_pScriptEditorWindowList.erase(this);
	if(g_pScriptEditorWindowList.empty())
		saveOptions();
}

void ScriptEditorImplementation::loadOptions()
{
	QString szTmp;
	g_pEditorModulePointer->getDefaultConfigFileName(szTmp);

	KviConfigurationFile cfg(szTmp, KviConfigurationFile::Read);
	g_clrBackground = cfg.readColorEntry("Background", QColor(0, 0, 0));
	g_clrNormalText = cfg.readColorEntry("NormalText", QColor(100, 255, 0));
	g_clrBracket = cfg.readColorEntry("Bracket", QColor(255, 0, 0));
	g_clrComment = cfg.readColorEntry("Comment", QColor(0, 120, 0));
	g_clrFunction = cfg.readColorEntry("Function", QColor(255, 255, 0));
	g_clrKeyword = cfg.readColorEntry("Keyword", QColor(120, 120, 150));
	g_clrVariable = cfg.readColorEntry("Variable", QColor(200, 200, 200));
	g_clrPunctuation = cfg.readColorEntry("Punctuation", QColor(180, 180, 0));
	g_clrFind = cfg.readColorEntry("Find", QColor(255, 0, 0));
	g_fntNormal = cfg.readFontEntry("Font", QFont(KVI_SCRIPTEDITOR_DEFAULT_FONT, KVI_SCRIPTEDITOR_DEFAULT_FONT_SIZE));
}

bool ScriptEditorImplementation::isModified()
{
	return m_pEditor->document()->isModified();
}
void ScriptEditorImplementation::setModified(bool bModified)
{
	return m_pEditor->document()->setModified(bModified);
}
void ScriptEditorImplementation::slotFind()
{
	emit find(m_pFindLineEdit->text());
}

void ScriptEditorImplementation::slotNextFind(const QString &) //szText
{
	//	emit nextFind(const QString & szText);
}

void ScriptEditorImplementation::slotInitFind()
{
	emit initFind();
}

void ScriptEditorImplementation::slotReplaceAll(const QString & szToReplace, const QString & szReplaceWith)
{
	emit replaceAll(szToReplace, szReplaceWith);
}

void ScriptEditorImplementation::saveOptions()
{
	QString szTmp;
	g_pEditorModulePointer->getDefaultConfigFileName(szTmp);

	KviConfigurationFile cfg(szTmp, KviConfigurationFile::Write);
	cfg.writeEntry("Background", g_clrBackground);
	cfg.writeEntry("NormalText", g_clrNormalText);
	cfg.writeEntry("Bracket", g_clrBracket);
	cfg.writeEntry("Comment", g_clrComment);
	cfg.writeEntry("Function", g_clrFunction);
	cfg.writeEntry("Keyword", g_clrKeyword);
	cfg.writeEntry("Variable", g_clrVariable);
	cfg.writeEntry("Punctuation", g_clrPunctuation);
	cfg.writeEntry("Find", g_clrFind);
	cfg.writeEntry("Font", g_fntNormal);
}

void ScriptEditorImplementation::setFocus()
{
	m_pEditor->setFocus();
}

void ScriptEditorImplementation::focusInEvent(QFocusEvent *)
{
	m_pEditor->setFocus();
}

void ScriptEditorImplementation::setEnabled(bool bEnabled)
{
	QWidget::setEnabled(bEnabled);
	m_pEditor->setEnabled(bEnabled);
	m_pRowColLabel->setEnabled(bEnabled);
}

void ScriptEditorImplementation::saveToFile()
{
	QString szFileName;
	if(KviFileDialog::askForSaveFileName(szFileName,
	       __tr2qs_ctx("Choose a Filename - KVIrc", "editor"),
	       QString::null,
	       QString::null, false, true, true, this))
	{
		QString szBuffer = m_pEditor->toPlainText();

		if(!KviFileUtils::writeFile(szFileName, szBuffer))
		{
			QString szTmp;
			QMessageBox::warning(this,
			    __tr2qs_ctx("Writing to File Failed - KVIrc", "editor"),
			    szTmp = QString(__tr2qs_ctx("Can't open file %1 for writing.", "editor")).arg(szFileName));
		}
	}
}

void ScriptEditorImplementation::setText(const char * txt)
{
	setText(QByteArray(txt));
}
void ScriptEditorImplementation::setUnHighlightedText(const QString & szText)
{
	m_pEditor->disableSyntaxHighlighter();
	m_pEditor->setText(szText);
}
void ScriptEditorImplementation::setText(const QByteArray & szText)
{
	m_pEditor->setPlainText(szText.data());
	m_pEditor->document()->setModified(false);
	updateRowColLabel();
}
void ScriptEditorImplementation::setReadOnly(bool bReadOnly)
{
	m_pEditor->setReadOnly(bReadOnly);
}
void ScriptEditorImplementation::getText(QByteArray & szText)
{
	szText = m_pEditor->toPlainText().toUtf8();
}

void ScriptEditorImplementation::setText(const QString & szText)
{
	m_pEditor->enableSyntaxHighlighter();
	m_pEditor->setPlainText(szText);
	QTextCursor cur = m_pEditor->textCursor();
	cur.movePosition(QTextCursor::End);
	m_pEditor->setTextCursor(cur);
	m_pEditor->document()->setModified(false);
	updateRowColLabel();
}

void ScriptEditorImplementation::getText(QString & szText)
{
	szText = m_pEditor->toPlainText();
}

void ScriptEditorImplementation::setFindText(const QString & szText)
{
	m_pFindLineEdit->setText(szText);
	m_pEditor->slotFind();
}

void ScriptEditorImplementation::updateRowColLabel()
{
	if(m_lastCursorPos == m_pEditor->textCursor().position())
		return;
	int iRow = m_pEditor->textCursor().blockNumber() + 1;
	int iCol = m_pEditor->textCursor().columnNumber() + 1;
	QString szTmp = QString(__tr2qs_ctx("Line: %1 Col: %2", "editor")).arg(iRow).arg(iCol);
	m_pRowColLabel->setText(szTmp);
	m_lastCursorPos = m_pEditor->textCursor().position();
}

void ScriptEditorImplementation::setCursorPosition(int iPos)
{
	QTextCursor cur = m_pEditor->textCursor();
	cur.setPosition(iPos);
	m_pEditor->setTextCursor(cur);
	updateRowColLabel();
}

void ScriptEditorImplementation::loadFromFile()
{
	QString szFileName;
	if(KviFileDialog::askForOpenFileName(szFileName,
	       __tr2qs_ctx("Select a File - KVIrc", "editor"),
	       QString::null, KVI_FILTER_SCRIPT, false, true, this))
	{
		QString szBuffer;
		if(KviFileUtils::loadFile(szFileName, szBuffer))
		{
			m_pEditor->setPlainText(szBuffer);
			setCursorPosition(0);
		}
		else
		{
			QString szTmp;
			QMessageBox::warning(this,
			    __tr2qs_ctx("Opening File Failed - KVIrc", "editor"),
			    szTmp = QString(__tr2qs_ctx("Can't open file %1 for reading.", "editor")).arg(szFileName));
		}
	}
}

void ScriptEditorImplementation::configureColors()
{
	if(!m_pOptionsDialog)
	{
		m_pOptionsDialog = new ScriptEditorWidgetColorOptions(this);
		connect(m_pOptionsDialog, SIGNAL(finished(int)), this, SLOT(optionsDialogFinished(int)));
	}
	m_pOptionsDialog->show();
}

void ScriptEditorImplementation::optionsDialogFinished(int iResult)
{
	if(iResult == QDialog::Accepted)
	{
		m_pEditor->updateOptions();
		saveOptions();
	}
}

ScriptEditorReplaceDialog::ScriptEditorReplaceDialog(QWidget * pParent, const QString & szName)
    : QDialog(pParent)
{
	setObjectName(szName);
	setWindowTitle(__tr2qs("Find and Replace - KVIrc"));

	m_pParent = pParent;
	emit initFind();
	QPalette p = palette();
	p.setColor(foregroundRole(), QColor(0, 0, 0));
	p.setColor(backgroundRole(), QColor(255, 255, 255));
	setPalette(p);

	QGridLayout * pLayout = new QGridLayout(this);
	pLayout->setObjectName("replace layout");

	QLabel * m_pFindLabel = new QLabel(this);
	m_pFindLabel->setObjectName("findlabel");
	m_pFindLabel->setText(__tr2qs_ctx("Word to find:", "editor"));
	pLayout->addWidget(m_pFindLabel, 0, 0);

	m_pFindLineEdit = new QLineEdit(this);
	m_pFindLineEdit->setObjectName("findlineedit");
	pLayout->addWidget(m_pFindLineEdit, 0, 1);

	QLabel * m_pReplaceLabel = new QLabel(this);
	m_pReplaceLabel->setObjectName("replacelabel");
	m_pReplaceLabel->setText(__tr2qs_ctx("Replace with:", "editor"));
	pLayout->addWidget(m_pReplaceLabel, 1, 0);

	m_pReplaceLineEdit = new QLineEdit(this);
	m_pReplaceLineEdit->setObjectName("replacelineedit");
	pLayout->addWidget(m_pReplaceLineEdit, 1, 1);

	m_pFindLineEdit->setFocus();

	m_pCheckReplaceAll = new QCheckBox(this);
	m_pCheckReplaceAll->setObjectName("replaceAll");
	m_pCheckReplaceAll->setText(__tr2qs_ctx("&Replace in all editor's items", "editor"));
	pLayout->addWidget(m_pCheckReplaceAll, 2, 0);

	QPushButton * pCancelButton = new QPushButton(this);
	pCancelButton->setObjectName("cancelButton");
	pCancelButton->setText(__tr2qs_ctx("&Cancel", "editor"));
	pLayout->addWidget(pCancelButton, 3, 0);

	m_pReplaceButton = new QPushButton(this);
	m_pReplaceButton->setObjectName("replacebutton");
	m_pReplaceButton->setText(__tr2qs_ctx("&Replace", "editor"));
	m_pReplaceButton->setEnabled(false);
	pLayout->addWidget(m_pReplaceButton, 3, 1);

	setLayout(pLayout);

	// signals and slots connections
	connect(m_pReplaceButton, SIGNAL(clicked()), this, SLOT(slotReplace()));
	connect(pCancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(m_pFindLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
}

void ScriptEditorReplaceDialog::textChanged(const QString & szText)
{
	if(!szText.isEmpty())
		m_pReplaceButton->setEnabled(true);
	else
		m_pReplaceButton->setEnabled(false);
}

void ScriptEditorReplaceDialog::slotReplace()
{
	QString szText = ((ScriptEditorWidget *)m_pParent)->toPlainText();
	if(m_pCheckReplaceAll->isChecked())
		emit replaceAll(m_pFindLineEdit->text(), m_pReplaceLineEdit->text());

	szText.replace(m_pFindLineEdit->text(), m_pReplaceLineEdit->text(), Qt::CaseInsensitive);
	((ScriptEditorWidget *)m_pParent)->setText(szText);
	((ScriptEditorWidget *)m_pParent)->document()->setModified(true);
	m_pFindLineEdit->setText("");
	m_pReplaceLineEdit->setText("");
	setTabOrder(m_pFindLineEdit, m_pReplaceLineEdit);
}

void ScriptEditorReplaceDialog::slotNextFind()
{
	emit nextFind(m_pFindLineEdit->text());
}
