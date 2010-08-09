//=============================================================================
//
//   File : scripteditor.cpp
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

#include "scripteditor.h"

#include "kvi_fileutils.h"
#include "kvi_fileextensions.h"
#include "kvi_locale.h"
#include "kvi_filedialog.h"
#include "kvi_qstring.h"
#include "kvi_config.h"
#include "kvi_module.h"
#include "kvi_pointerlist.h"
#include "kvi_app.h"
#include "kvi_console.h"
#include "kvi_window.h"
#include "kvi_iconmanager.h"
#include "kvi_kvs_kernel.h"
#include "kvi_modulemanager.h"
#include "kvi_config.h"
#include "kvi_tal_vbox.h"
#include "kvi_tal_hbox.h"
#include "kvi_tal_groupbox.h"

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

extern KVIRC_API KviModuleManager                    * g_pModuleManager;
extern KviPointerList<KviScriptEditorImplementation> * g_pScriptEditorWindowList;
extern KviModule                                     * g_pEditorModulePointer;

static QColor g_clrBackground(0,0,0);
static QColor g_clrNormalText(100,100,100);
static QColor g_clrBracket(255,0,0);
static QColor g_clrComment(0,120,0);
static QColor g_clrFunction(0,17,255);
static QColor g_clrKeyword(85,85,255);
static QColor g_clrVariable(255,0,0);
static QColor g_clrPunctuation(180,180,0);
static QColor g_clrFind(0,0,0);
static QFont g_fntNormal("Courier New",8);


static bool bSemaphore=false;
static bool bCompleterReady=false;


KviScriptEditorWidget::KviScriptEditorWidget(QWidget * pParent)
: QTextEdit(pParent)
{
	m_pSyntaxHighlighter=0;
	setTabStopWidth(48);
	setAcceptRichText(false);
	setWordWrapMode(QTextOption::NoWrap);
	m_pParent = pParent;
	m_szHelp = "Nothing";
	updateOptions();
	m_szFind = "";
	m_pCompleter = 0;
	QStringList szListFunctionsCommands;
	QString tmp("kvscompleter.idx");
	iModulesCount=0;
	iIndex=0;
	QString szPath;
	g_pApp->getLocalKvircDirectory(szPath,KviApp::ConfigPlugins,tmp);

	if(!KviFileUtils::fileExists(szPath))
	{
		if (!bSemaphore){
			bSemaphore=true;
			m_pStartTimer = new QTimer();
			m_pStartTimer->setInterval(1000);
			connect(m_pStartTimer,SIGNAL(timeout()),this,SLOT(asyncCompleterCreation()));
			m_pStartTimer->start(500);
		}
		else{
			m_pStartTimer = new QTimer();
			m_pStartTimer->setInterval(2000);
			connect(m_pStartTimer,SIGNAL(timeout()),this,SLOT(checkReadyCompleter()));
			m_pStartTimer->start(1000);
		}
	} else loadCompleterFromFile();
}

KviScriptEditorWidget::~KviScriptEditorWidget()
{
	if(m_pCompleter)
		delete m_pCompleter;
}

void KviScriptEditorWidget::checkReadyCompleter()
{
	if(bCompleterReady)
	{
		m_pStartTimer->stop();
		delete m_pStartTimer;
		m_pStartTimer=0;
		loadCompleterFromFile();
	}
}

void KviScriptEditorWidget::asyncCompleterCreation()
{
	//static int iIndex = 0;
	//static int iModulesCount = 0;
	if(!iIndex)
	{
		m_pListCompletition = new QStringList();
		QString szPath;

		g_pApp->getGlobalKvircDirectory(szPath,KviApp::Modules);

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
	szModuleName=szModuleName.replace("kvi","");
	szModuleName=szModuleName.replace(".dll","");
#elif defined(COMPILE_ON_MINGW)
	szModuleName=szModuleName.replace("libkvi","");
	szModuleName=szModuleName.replace(".dll","");
#else
	szModuleName=szModuleName.replace("libkvi","");
	szModuleName=szModuleName.replace(".so","");
#endif

	KviModule * pModule = g_pModuleManager->getModule(szModuleName);
	if(pModule)
	{
		pModule->getAllFunctionsCommandsModule(m_pListCompletition,szModuleName);
	}

	if(iIndex == iModulesCount)
	{
		m_pStartTimer->stop();
		m_pStartTimer->deleteLater();
		m_pStartTimer = 0;
		QString szTmp("kvscompleter.idx");
		QString szPath;
		g_pApp->getLocalKvircDirectory(szPath,KviApp::ConfigPlugins,szTmp);
		KviKvsKernel::instance()->getAllFunctionsCommandsCore(m_pListCompletition);
		QString szBuffer = m_pListCompletition->join(",");
		QFile f(szPath);
		f.open(QIODevice::WriteOnly);
		f.write(szBuffer.toUtf8());
		f.close();
		createCompleter(*m_pListCompletition);
		iIndex = 0;
		iModulesCount = 0;
		bCompleterReady=true;
		delete m_pListCompletition;
		delete m_pListModulesNames;
	}
}

void KviScriptEditorWidget::loadCompleterFromFile()
{
	QStringList szListFunctionsCommand;
	QString szTmp("kvscompleter.idx");
	QString szPath;
	g_pApp->getLocalKvircDirectory(szPath,KviApp::ConfigPlugins,szTmp);
	QString szBuffer;
	QFile f(szPath);
	f.open(QIODevice::ReadOnly);
	szBuffer = f.readAll();
	f.close();
	szListFunctionsCommand = szBuffer.split(',');
	createCompleter(szListFunctionsCommand);
}

void KviScriptEditorWidget::createCompleter(QStringList & list)
{
	m_pCompleter = new QCompleter(list);
	m_pCompleter->setCaseSensitivity(Qt::CaseInsensitive);
	m_pCompleter->setWrapAround(false);
	m_pCompleter->setWidget(this);
	m_pCompleter->setModelSorting(QCompleter::CaseSensitivelySortedModel);
	m_pCompleter->setCompletionMode(QCompleter::PopupCompletion);
	m_pCompleter->setCaseSensitivity(Qt::CaseInsensitive);
	QObject::connect(m_pCompleter, SIGNAL(activated(const QString &)),this, SLOT(insertCompletion(const QString &)));
}

void KviScriptEditorWidget::insertCompletion(const QString & szCompletion)
{
	QTextCursor tc = textCursor();
	int iExtra = szCompletion.length() - m_pCompleter->completionPrefix().length();

	tc.movePosition(QTextCursor::Left);
	tc.movePosition(QTextCursor::EndOfWord);

	QString szTmp = szCompletion.right(iExtra);
	if(szCompletion.left(1)=="$")
		szTmp.append("(");
	else
		szTmp.append(" ");

	tc.insertText(szTmp);
	setTextCursor(tc);
}

void KviScriptEditorWidget::contextMenuEvent(QContextMenuEvent * e)
{
	QMenu * pMenu = createStandardContextMenu();
	pMenu->addAction(__tr2qs_ctx("Context sensitive help","editor"),this,SLOT(slotHelp()),Qt::CTRL+Qt::Key_H);
	pMenu->addAction(__tr2qs_ctx("&Replace","editor"),this,SLOT(slotReplace()),Qt::CTRL+Qt::Key_R);
	pMenu->exec(e->globalPos());
	delete pMenu;
}

void KviScriptEditorWidget::slotFind()
{
	m_szFind = ((KviScriptEditorImplementation*)m_pParent)->findLineEdit()->text();
	setText(toPlainText());
}

void KviScriptEditorWidget::slotReplace()
{
	KviScriptEditorReplaceDialog * pDialog = new KviScriptEditorReplaceDialog(this,__tr2qs_ctx("Find & Replace","editor"));
	connect(pDialog,SIGNAL(replaceAll(const QString &,const QString &)),m_pParent,SLOT(slotReplaceAll(const QString &,const QString &)));
	connect(pDialog,SIGNAL(initFind()),m_pParent,SLOT(slotInitFind()));
	connect(pDialog,SIGNAL(nextFind(const QString &)),m_pParent,SLOT(slotNextFind(const QString &)));

	if(pDialog->exec()){};
}

void KviScriptEditorWidget::slotHelp()
{
	contextSensitiveHelp();
}

void KviScriptEditorWidget::updateOptions()
{
	QPalette p = palette();
	p.setColor(QPalette::Base,g_clrBackground);
	p.setColor(QPalette::Text,g_clrNormalText);
	setPalette(p);
	setFont(g_fntNormal);
	setTextColor(g_clrNormalText);
	// we currently delete and re-create the m_pSyntaxHighlighter
	// as a trick to ensure proper re-highlightning to occur
	if (m_pSyntaxHighlighter) delete m_pSyntaxHighlighter;
	m_pSyntaxHighlighter = new KviScriptEditorSyntaxHighlighter(this);

	p = ((KviScriptEditorImplementation*)m_pParent)->findLineEdit()->palette();
	p.setColor(foregroundRole(),g_clrFind);
	((KviScriptEditorImplementation*)m_pParent)->findLineEdit()->setPalette(p);
}

void KviScriptEditorWidget::keyPressEvent(QKeyEvent * e)
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
	if (!m_pCompleter || !isShortcut) // don't process the shortcut when we have a completer
		QTextEdit::keyPressEvent(e);
	const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
	if (!m_pCompleter || (ctrlOrShift && e->text().isEmpty()))
		return;
	static QString eow("~!@#$%^&*()_+{}|:\"<>?,/;'[]\\-="); // end of word
	bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
	QString completionPrefix = textUnderCursor();
	if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 3 || eow.contains(e->text().right(1))))
	{
		m_pCompleter->popup()->hide();
		return;
	}
	if (completionPrefix != m_pCompleter->completionPrefix())
	{
		m_pCompleter->setCompletionPrefix(completionPrefix);
		m_pCompleter->popup()->setCurrentIndex(m_pCompleter->completionModel()->index(0, 0));
	}
	QRect cr = cursorRect();
	cr.setWidth(m_pCompleter->popup()->sizeHintForColumn(0)+ m_pCompleter->popup()->verticalScrollBar()->sizeHint().width());
	m_pCompleter->complete(cr);
}

QString KviScriptEditorWidget::textUnderCursor() const
{
	QString szWord;
	QTextCursor tc = textCursor();
	if(tc.atBlockStart())
		return QString();
	tc.clearSelection();
	tc.movePosition(QTextCursor::StartOfWord,QTextCursor::KeepAnchor);
	if(tc.atBlockStart())
	{
		szWord.append(tc.selectedText());
		tc.movePosition(QTextCursor::EndOfWord,QTextCursor::KeepAnchor);
		szWord.append(tc.selectedText());
		if(tc.atBlockEnd()){
			return szWord;
		}
		tc.movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor);
		szWord.append(tc.selectedText());
		if(szWord.right(1)!=".")
			szWord.chop(1);
		return szWord;
	}

	tc.movePosition(QTextCursor::PreviousCharacter,QTextCursor::KeepAnchor);
	szWord=tc.selectedText();
	if(szWord.left(1)==".")
	{
		tc.movePosition(QTextCursor::StartOfWord);
		tc.movePosition(QTextCursor::PreviousCharacter);
		tc.movePosition(QTextCursor::PreviousWord);
		tc.movePosition(QTextCursor::EndOfWord,QTextCursor::KeepAnchor,1);
		szWord.prepend(tc.selectedText());
	} else szWord.remove(0,1);
	return szWord;
}
/*
void KviScriptEditorWidget::mouseReleaseEvent(QMouseEvent * e)
{

	return;
		QRect r = cursorRect();
		QTextCursor cur = cursorForPosition(e->pos());
	//	cur.select(


	//completelistbox->hide();
	if (e->button() == Qt::RightButton)
	{
		QString szBuffer;
		/\*
		int iPara = paragraphAt(e->pos());
		int iIndex=charAt(e->pos(),&iPara);
		szBuffer = this->text(iPara);
		getWordOnCursor(szBuffer,iIndex);
		*\/
		QRect r = cursorRect();
		QTextCursor cur = cursorForPosition(e->pos());
		cur.select(QTextCursor::WordUnderCursor);
		szBuffer = cur.selectedText();
		QString szTmp = szBuffer;
		KviPointerList<QString> * pList;
		if(szTmp.left(1) == "$")
		{
			szTmp.remove(0,1);
			pList = KviKvsKernel::instance()->completeCommandAllocateResult(szTmp);
			if (pList->count() != 1)
				szBuffer = "";
			else
				szBuffer = *(pList->at(0));
		} else {
			pList = KviKvsKernel::instance()->completeCommandAllocateResult(szTmp);
			if (pList->count() != 1)
				szBuffer = "";
			else
				szBuffer = *(pList->at(0));
		}

		for (unsigned int i=0; i < pList->count(); i++)
			QString str = *(pList->at(i));

		KviKvsKernel::instance()->freeCompletionResult(pList);
		m_szHelp = szBuffer;
	}

	QTextEdit::mouseReleaseEvent(e);
}
*/
bool KviScriptEditorWidget::contextSensitiveHelp() const
{
	/*
	QString szBuffer;
	int iPara, iIndex;
	getCursorPosition(&iPara,&iIndex);
	szBuffer = text(iPara);

	getWordOnCursor(szBuffer,iIndex);
	*/

	QRect r = cursorRect();
	QTextCursor cur = cursorForPosition(QPoint(r.x(),r.y()));
	cur.select(QTextCursor::WordUnderCursor);
	QString szText = cur.selectedText();
	QString szTmp = szText;
	/*
	KviPointerList<QString> * pList;
	if(szTmp.left(1) == "$")
	{
		szTmp.remove(0,1);
		pList = g_pUserParser->completeFunctionAllocateResult(szTmp);
	} else {
		pList = g_pUserParser->completeCommandAllocateResult(szTmp);
	}
	debug ("command or func %s",szTmp.toUtf8().data());

	bool bOk = false;
	if(pList)
	{
		for(QString * s = pList->first(); s; s = pList->next())
		{
			if(KviQString::equalCI(*s,szBuffer))
			{
				pList->last();
				bOk = true;
			}
		}
	}

	g_pUserParser->freeCompletionResult(pList);
	if(!bOk)return false;

	QString szParse;
	KviQString::sprintf(szParse,"timer -s (help,0){ help -s %Q; }",&szBuffer);
	debug ("parsing %s",szParse.toLatin1());
	KviKvsScript::run(szParse,(KviWindow*)g_pApp->activeConsole());
	*/

	return true;
}

KviScriptEditorWidgetColorOptions::KviScriptEditorWidgetColorOptions(QWidget * pParent)
: QDialog(pParent)
{
	m_pSelectorInterfaceList = new KviPointerList<KviSelectorInterface>;
	m_pSelectorInterfaceList->setAutoDelete(false);
	setWindowTitle(__tr2qs_ctx("Preferences","editor"));
	QGridLayout * g = new QGridLayout(this);
	KviTalVBox * box = new KviTalVBox(this);
	g->addWidget(box,0,0);
	box->setMargin(0);
	box->setSpacing(0);
	KviFontSelector * f = new KviFontSelector(box,__tr2qs_ctx("Font:","editor"),&g_fntNormal,true);
	m_pSelectorInterfaceList->append(f);
	KviTalGroupBox * gbox = new KviTalGroupBox(Qt::Horizontal,__tr2qs_ctx("Colors","editor"),box);
	gbox->setInsideSpacing(0);
	KviColorSelector * s = addColorSelector(gbox,__tr2qs_ctx("Background:","editor"),&g_clrBackground,true);
	s = addColorSelector(gbox,__tr2qs_ctx("Normal text:","editor"),&g_clrNormalText,true);
	s = addColorSelector(gbox,__tr2qs_ctx("Brackets:","editor"),&g_clrBracket,true);
	s = addColorSelector(gbox,__tr2qs_ctx("Comments:","editor"),&g_clrComment,true);
	s = addColorSelector(gbox,__tr2qs_ctx("Functions:","editor"),&g_clrFunction,true);
	s = addColorSelector(gbox,__tr2qs_ctx("Keywords:","editor"),&g_clrKeyword,true);
	s = addColorSelector(gbox,__tr2qs_ctx("Variables:","editor"),&g_clrVariable,true);
	s = addColorSelector(gbox,__tr2qs_ctx("Punctuation:","editor"),&g_clrPunctuation,true);
	s = addColorSelector(gbox,__tr2qs_ctx("Find:","editor"),&g_clrFind,true);

	KviTalHBox * hbox = new KviTalHBox(box);

	QPushButton * b = new QPushButton(__tr2qs_ctx("OK","editor"),hbox);
	b->setDefault(true);
	connect(b,SIGNAL(clicked()),this,SLOT(okClicked()));

	b = new QPushButton(__tr2qs_ctx("Cancel","editor"),hbox);
	connect(b,SIGNAL(clicked()),this,SLOT(reject()));
}

KviScriptEditorWidgetColorOptions::~KviScriptEditorWidgetColorOptions()
{
	delete m_pSelectorInterfaceList;
}

KviColorSelector * KviScriptEditorWidgetColorOptions::addColorSelector(QWidget * pParent,const QString & txt,QColor * pOption,bool bEnabled)
{
	KviColorSelector * s = new KviColorSelector(pParent,txt,pOption,bEnabled);
	m_pSelectorInterfaceList->append(s);
		return s;
}

void KviScriptEditorWidgetColorOptions::okClicked()
{
	for(KviSelectorInterface * i = m_pSelectorInterfaceList->first();i;i = m_pSelectorInterfaceList->next())
	{
		i->commit();
	}
	accept();
}


KviScriptEditorSyntaxHighlighter::KviScriptEditorSyntaxHighlighter(KviScriptEditorWidget * pWidget)
: QSyntaxHighlighter(pWidget),m_pTextEdit(pWidget)
{
	// code adpated from QT4 example

	// FIX-ME: "function ..." - "function internal ..."
	// FIX-ME: "@$"
	// FIX-ME: "\" escape char
	// FIX-ME: comment multiline with empty line
	updateSyntaxtTextFormat();

	KviScriptHighlightingRule rule;

	rule.pattern=QRegExp("([=()[\\]!\"?<>;:.,+-])+");
	rule.format=punctuationFormat;
	highlightingRules.append(rule);

	rule.pattern=QRegExp("[{};](|[a-zA-Z]|[a-zA-Z]+[a-zA-Z0-9_\\.:]*)");
	rule.format=keywordFormat;
	highlightingRules.append(rule);


	rule.pattern=QRegExp("[$](|[a-zA-Z0-9]+[a-zA-Z0-9_\\.:]*)");
	rule.format=functionFormat;
	highlightingRules.append(rule);

	rule.pattern=QRegExp("[%](|[a-zA-Z]|[a-zA-Z]+[a-zA-Z0-9_\\.]*)");
	rule.format=variableFormat;
	highlightingRules.append(rule);

	rule.pattern=QRegExp("([{}])+");
	rule.format=bracketFormat;
	highlightingRules.append(rule);

	rule.pattern=QRegExp("(//[^\\n]*)|(#[^\\n]*)");
	rule.format=commentFormat;
	highlightingRules.append(rule);

	commentStartExpression = QRegExp("/\\*");
	commentEndExpression = QRegExp("\\*/");
}

KviScriptEditorSyntaxHighlighter::~KviScriptEditorSyntaxHighlighter()
{
}

void KviScriptEditorSyntaxHighlighter::updateSyntaxtTextFormat()
{
	punctuationFormat.setForeground(g_clrPunctuation);
	keywordFormat.setForeground(g_clrKeyword);
	functionFormat.setForeground(g_clrFunction);
	variableFormat.setForeground(g_clrVariable);
	bracketFormat.setForeground(g_clrBracket);
	commentFormat.setForeground(g_clrComment);
	findFormat.setForeground(g_clrFind);
}

void KviScriptEditorSyntaxHighlighter::highlightBlock(const QString & szText)
{
	if(szText.isEmpty())
		return;

	int iIndexStart=0;

	if(previousBlockState() == 1)
	{
		// in comment
		int iCommentEnd = szText.indexOf(commentEndExpression);
		if(iCommentEnd < 0)
		{
			// not found: comment until the end of the block
			setCurrentBlockState(1);
			setFormat(iIndexStart,szText.length(),commentFormat);
			return;
		}
		// end of comment found
		setFormat(iIndexStart,iCommentEnd - iIndexStart,commentFormat);
		setCurrentBlockState(0);
		iIndexStart = iCommentEnd;
	}

	// skip tabulations and spaces
	while(iIndexStart < szText.size())
	{
		if(szText.at(iIndexStart).unicode()=='\t' || szText.at(iIndexStart).unicode()==' ')
		{
			iIndexStart++;
		} else {
			break;
		}
	}

	if (iIndexStart == szText.size())
		return;

	// check 'commands'
	int iCommandStart=iIndexStart;
	if (szText.at(iIndexStart).unicode()!='$' && szText.at(iIndexStart).unicode()!='{' && szText.at(iIndexStart).unicode()!='}' && szText.at(iIndexStart).unicode()!='%')
	{
		while(iIndexStart<szText.size() && (szText.at(iIndexStart).isLetterOrNumber() || (szText.at(iIndexStart).unicode() == '.') || (szText.at(iIndexStart).unicode() == '_') || (szText.at(iIndexStart).unicode()== ':') ))
		{
			iIndexStart++;
		}
		setFormat(iCommandStart,iIndexStart-iCommandStart,keywordFormat);
	}

	// code from QT4 example
	int index=0;
	foreach (KviScriptHighlightingRule rule, highlightingRules)
	{
		QRegExp expression(rule.pattern);
		QString sz=expression.pattern();

		index = szText.indexOf(expression,iIndexStart);

		while (index >= 0)
		{
			int length = expression.matchedLength();
			setFormat(index, length, rule.format);
			index = szText.indexOf(expression, index + length);
		}
	}

	setCurrentBlockState(0);

	int startIndex = 0;
	if (previousBlockState() != 1)
		startIndex = szText.indexOf(commentStartExpression);

	while (startIndex >= 0)
	{
		int endIndex = szText.indexOf(commentEndExpression, startIndex);
		int commentLength;
		if (endIndex == -1)
		{
			setCurrentBlockState(1);
			commentLength = szText.length() - startIndex;
		} else {
			commentLength = endIndex - startIndex + commentEndExpression.matchedLength();
		}
		setFormat(startIndex, commentLength, commentFormat);
		startIndex = szText.indexOf(commentStartExpression, startIndex + commentLength);
	}

	// 'found matches' highlighting
	KviScriptEditorWidget * pEditor = ((KviScriptEditorWidget *)textEdit());
	QString szFind = pEditor->m_szFind;
	if (!szFind.isEmpty())
	{
		index = szText.indexOf(szFind,0,Qt::CaseInsensitive);
		int length=szFind.length();
		while (index >= 0)
		{
			setFormat(index, length, findFormat);
			index = szText.indexOf(szFind,index + length,Qt::CaseInsensitive);
		}
	}
}


KviScriptEditorImplementation::KviScriptEditorImplementation(QWidget * par)
:KviScriptEditor(par)
{
	m_pOptionsDialog=0;
	if(g_pScriptEditorWindowList->isEmpty())loadOptions();
	g_pScriptEditorWindowList->append(this);
	m_lastCursorPos = 0;
	QGridLayout * g = new QGridLayout(this);

	m_pFindLineEdit = new QLineEdit(" ",this);
	m_pFindLineEdit->setText("");

	QPalette p = m_pFindLineEdit->palette();
	p.setColor(foregroundRole(), g_clrFind);
	m_pFindLineEdit->setPalette(p);

	m_pEditor = new KviScriptEditorWidget(this);

	g->addWidget(m_pEditor, 0, 0, 1,4);
	g->setRowStretch(0,1);

	QToolButton * b = new QToolButton(this);
	b->setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_OPTIONS)));

	b->setObjectName("ToolButtonEditor");
	b->setMinimumWidth(24);
	g->addWidget(b,1,0);

	KviTalPopupMenu * pop = new KviTalPopupMenu(b);
	pop->insertItem(__tr2qs_ctx("&Open...","editor"),this,SLOT(loadFromFile()));
	pop->insertItem(__tr2qs_ctx("&Save As...","editor"),this,SLOT(saveToFile()));
	pop->insertSeparator();
	pop->insertItem(__tr2qs_ctx("&Configure Editor...","editor"),this,SLOT(configureColors()));
	b->setMenu(pop);
	b->setPopupMode(QToolButton::InstantPopup);

	g->setColumnStretch(1,1);
	g->setColumnStretch(2,10);
	g->addWidget(m_pFindLineEdit,1,2);

	QLabel * pLab = new QLabel(this);
	pLab->setText(__tr2qs_ctx("Find","editor"));
	g->addWidget(pLab,1,1);

	m_pRowColLabel = new QLabel("0",this);
	m_pRowColLabel->setFrameStyle(QFrame::Sunken | QFrame::Panel);
	m_pRowColLabel->setMinimumWidth(80);
	g->addWidget(m_pRowColLabel,1,3);

	connect(m_pFindLineEdit,SIGNAL(returnPressed()),m_pEditor,SLOT(slotFind()));
	connect(m_pFindLineEdit,SIGNAL(returnPressed()),this,SLOT(slotFind()));
	connect(m_pEditor,SIGNAL(cursorPositionChanged()),this,SLOT(updateRowColLabel()));
	connect(m_pEditor,SIGNAL(selectionChanged()),this,SLOT(updateRowColLabel()));
	m_lastCursorPos = 0;
}

KviScriptEditorImplementation::~KviScriptEditorImplementation()
{
	if(m_pOptionsDialog)
	{
		m_pOptionsDialog->deleteLater();
		m_pOptionsDialog=0;
	}
	g_pScriptEditorWindowList->removeRef(this);
	if(g_pScriptEditorWindowList->isEmpty())
		saveOptions();
}

void KviScriptEditorImplementation::loadOptions()
{
	QString szTmp;
	g_pEditorModulePointer->getDefaultConfigFileName(szTmp);

	KviConfig cfg(szTmp,KviConfig::Read);
	g_clrBackground = cfg.readColorEntry("Background",QColor(0,0,0));;
	g_clrNormalText = cfg.readColorEntry("NormalText",QColor(100,255,0));
	g_clrBracket = cfg.readColorEntry("Bracket",QColor(255,0,0));
	g_clrComment = cfg.readColorEntry("Comment",QColor(0,120,0));
	g_clrFunction = cfg.readColorEntry("Function",QColor(255,255,0));
	g_clrKeyword = cfg.readColorEntry("Keyword",QColor(120,120,150));
	g_clrVariable = cfg.readColorEntry("Variable",QColor(200,200,200));
	g_clrPunctuation = cfg.readColorEntry("Punctuation",QColor(180,180,0));
	g_clrFind = cfg.readColorEntry("Find",QColor(255,0,0));
	g_fntNormal = cfg.readFontEntry("Font",QFont("Courier New",8));
}

bool KviScriptEditorImplementation::isModified()
{
	return m_pEditor->document()->isModified();
}
void KviScriptEditorImplementation::setModified(bool bModified)
{
	return m_pEditor->document()->setModified(bModified);
}
void KviScriptEditorImplementation::slotFind()
{
	emit find(m_pFindLineEdit->text());
}

void KviScriptEditorImplementation::slotNextFind(const QString &) //szText
{
//	emit nextFind(const QString & szText);
}

void KviScriptEditorImplementation::slotInitFind()
{
	emit initFind();
}

void KviScriptEditorImplementation::slotReplaceAll(const QString & szToReplace, const QString & szReplaceWith)
{
	emit replaceAll(szToReplace,szReplaceWith);
}

void KviScriptEditorImplementation::saveOptions()
{
	QString szTmp;
	g_pEditorModulePointer->getDefaultConfigFileName(szTmp);

	KviConfig cfg(szTmp,KviConfig::Write);
	cfg.writeEntry("Background",g_clrBackground);;
	cfg.writeEntry("NormalText",g_clrNormalText);
	cfg.writeEntry("Bracket",g_clrBracket);
	cfg.writeEntry("Comment",g_clrComment);
	cfg.writeEntry("Function",g_clrFunction);
	cfg.writeEntry("Keyword",g_clrKeyword);
	cfg.writeEntry("Variable",g_clrVariable);
	cfg.writeEntry("Punctuation",g_clrPunctuation);
	cfg.writeEntry("Find",g_clrFind);
	cfg.writeEntry("Font",g_fntNormal);
}

void KviScriptEditorImplementation::setFocus()
{
	m_pEditor->setFocus();
}

void KviScriptEditorImplementation::focusInEvent(QFocusEvent *)
{
	m_pEditor->setFocus();
}

void KviScriptEditorImplementation::setEnabled(bool bEnabled)
{
	QWidget::setEnabled(bEnabled);
	m_pEditor->setEnabled(bEnabled);
	m_pRowColLabel->setEnabled(bEnabled);
}

void KviScriptEditorImplementation::saveToFile()
{
	QString szFileName;
	if(KviFileDialog::askForSaveFileName(szFileName,
		__tr2qs_ctx("Choose a Filename - KVIrc","editor"),
		QString::null,
		QString::null,false,true,true))
	{
		QString szBuffer = m_pEditor->toPlainText();

		//if(tmp.isEmpty())tmp = "";
		//KviStr buffer = tmp.toUtf8().data();
		if(!KviFileUtils::writeFile(szFileName,szBuffer))
		{
			QString szTmp;
			QMessageBox::warning(this,
				__tr2qs_ctx("Save Failed - KVIrc","editor"),
				KviQString::sprintf(szTmp,__tr2qs_ctx("Can't open the file %s for writing.","editor"),&szFileName));
		}
	}
}

void KviScriptEditorImplementation::setText(const char * txt)
{
	setText(QByteArray(txt));
}

void KviScriptEditorImplementation::setText(const QByteArray & szText)
{
        m_pEditor->setPlainText(szText.data());
	m_pEditor->document()->setModified(false);
	updateRowColLabel();
}

void KviScriptEditorImplementation::getText(QByteArray & szText)
{
	szText = m_pEditor->toPlainText().toUtf8();
}

void KviScriptEditorImplementation::setText(const QString & szText)
{
	m_pEditor->setPlainText(szText);
	QTextCursor cur=m_pEditor->textCursor();
	cur.movePosition(QTextCursor::End);
	m_pEditor->setTextCursor(cur);
	m_pEditor->document()->setModified(false);
	updateRowColLabel();
}

void KviScriptEditorImplementation::getText(QString & szText)
{
	szText = m_pEditor->toPlainText();
}

void KviScriptEditorImplementation::setFindText(const QString & szText)
{
	m_pFindLineEdit->setText(szText);
	m_pEditor->slotFind();
}

void KviScriptEditorImplementation::updateRowColLabel()
{
	if(m_lastCursorPos==m_pEditor->textCursor().position())
		return;
	int iRow = m_pEditor->textCursor().blockNumber();
	int iCol = m_pEditor->textCursor().columnNumber();
	QString szTmp;
	KviQString::sprintf(szTmp,__tr2qs_ctx("Row: %d Col: %d","editor"),iRow,iCol);
	m_pRowColLabel->setText(szTmp);
	m_lastCursorPos = m_pEditor->textCursor().position();
}

void KviScriptEditorImplementation::setCursorPosition(int iPos)
{
	QTextCursor cur = m_pEditor->textCursor();
	cur.setPosition(iPos);
	m_pEditor->setTextCursor(cur);
	updateRowColLabel();
}

void KviScriptEditorImplementation::loadFromFile()
{
	QString szFileName;
	if(KviFileDialog::askForOpenFileName(szFileName,
		__tr2qs_ctx("Load Script File - KVIrc","editor"),
		QString::null,KVI_FILTER_SCRIPT,false,true))
	{
		QString szBuffer;
		if(KviFileUtils::loadFile(szFileName,szBuffer))
		{
                        m_pEditor->setPlainText(szBuffer);
			setCursorPosition(0);
			//m_pEditor->moveCursor(QTextEdit::MoveEnd,false);
			//updateRowColLabel();
		} else {
			QString szTmp;
			QMessageBox::warning(this,
				__tr2qs_ctx("Open Failed - KVIrc","editor"),
				KviQString::sprintf(szTmp,__tr2qs_ctx("Can't open the file %s for reading.","editor"),&szFileName));
		}
	}
}

void KviScriptEditorImplementation::configureColors()
{
	if(!m_pOptionsDialog)
	{
		m_pOptionsDialog = new KviScriptEditorWidgetColorOptions(this);
		connect(m_pOptionsDialog,SIGNAL(finished(int)),this,SLOT(optionsDialogFinished(int)));
	}
	m_pOptionsDialog->show();
}

void KviScriptEditorImplementation::optionsDialogFinished(int iResult)
{
	if(iResult == QDialog::Accepted)
	{
		m_pEditor->updateOptions();
		saveOptions();
	}
}

KviScriptEditorReplaceDialog::KviScriptEditorReplaceDialog(QWidget * pParent, const QString & szName)
: QDialog(pParent)
{
	setObjectName(szName);

	m_pParent = pParent;
	emit initFind();
	QPalette p = palette();
	p.setColor(foregroundRole(),QColor( 0, 0, 0 ));
	p.setColor(backgroundRole(),QColor( 236, 233, 216 ));
	setPalette(p);

	QGridLayout * pLayout = new QGridLayout(this);
	pLayout->setObjectName("replace layout");

	QLabel * m_pFindLabel = new QLabel(this);
	m_pFindLabel->setObjectName("findlabel");
	m_pFindLabel->setText(__tr2qs_ctx("Word to Find","editor"));
	pLayout->addWidget(m_pFindLabel,0,0);

	m_pFindLineEdit = new QLineEdit(this);
	m_pFindLineEdit->setObjectName("findlineedit");
	pLayout->addWidget(m_pFindLineEdit,0,1);

	QLabel * m_pReplaceLabel = new QLabel(this);
	m_pReplaceLabel->setObjectName("replacelabel");
	m_pReplaceLabel->setText(__tr2qs_ctx("Replace with","editor"));
	pLayout->addWidget(m_pReplaceLabel,1,0);

	m_pReplaceLineEdit = new QLineEdit(this);
	m_pReplaceLineEdit->setObjectName("replacelineedit");
	pLayout->addWidget(m_pReplaceLineEdit,1,1);


	m_pFindLineEdit->setFocus();

	m_pCheckReplaceAll = new QCheckBox(this);
	m_pCheckReplaceAll->setObjectName("replaceAll");
	m_pCheckReplaceAll->setText(__tr2qs_ctx("&Replace in all Aliases","editor"));
	pLayout->addWidget(m_pCheckReplaceAll,2,0);

	QPushButton * pCancelButton = new QPushButton(this);
	pCancelButton->setObjectName("cancelButton");
	pCancelButton->setText(__tr2qs_ctx("&Cancel","editor"));
	pLayout->addWidget(pCancelButton,3,0);

	m_pReplaceButton = new QPushButton(this);
	m_pReplaceButton->setObjectName("replacebutton");
	m_pReplaceButton->setText(__tr2qs_ctx("&Replace","editor"));
	m_pReplaceButton->setEnabled(false);
	pLayout->addWidget(m_pReplaceButton,3,1);

	setLayout(pLayout);

	// signals and slots connections
	connect(m_pReplaceButton, SIGNAL(clicked()), this, SLOT(slotReplace()));
	connect(pCancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(m_pFindLineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
}

KviScriptEditorReplaceDialog::~KviScriptEditorReplaceDialog()
{
}

void KviScriptEditorReplaceDialog::textChanged(const QString & szText)
{
	if(!szText.isEmpty())
		m_pReplaceButton->setEnabled(true);
	else
		m_pReplaceButton->setEnabled(false);
}

void KviScriptEditorReplaceDialog::slotReplace()
{
	QString szText = ((KviScriptEditorWidget *)m_pParent)->toPlainText();
	if(m_pCheckReplaceAll->isChecked())
		emit replaceAll(m_pFindLineEdit->text(),m_pReplaceLineEdit->text());

	szText.replace(m_pFindLineEdit->text(),m_pReplaceLineEdit->text(),Qt::CaseInsensitive);
	((KviScriptEditorWidget *)m_pParent)->setText(szText);
	((KviScriptEditorWidget *)m_pParent)->document()->setModified(true);
	m_pFindLineEdit->setText("");
	m_pReplaceLineEdit->setText("");
	setTabOrder(m_pFindLineEdit,m_pReplaceLineEdit);
}

void KviScriptEditorReplaceDialog::slotNextFind()
{
	emit nextFind(m_pFindLineEdit->text());
}
