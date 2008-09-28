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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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
#include "kvi_tal_scrollview.h"

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






KviScriptEditorWidget::KviScriptEditorWidget(QWidget * pParent)
: QTextEdit(pParent)
{
	setTabStopWidth(48);
	setWordWrapMode(QTextOption::NoWrap);
	m_pParent = pParent;
	m_szHelp = "Nothing";
	updateOptions();
	m_szFind = "";
	m_pCompleter = 0;
	QStringList szListFunctionsCommands;
	QString tmp("kvscompleter.idx");

	QString szPath;
	g_pApp->getLocalKvircDirectory(szPath,KviApp::ConfigPlugins,tmp);
	if(!KviFileUtils::fileExists(szPath))
	{
		m_pStartTimer = new QTimer();
		m_pStartTimer->setInterval(500);
		connect(m_pStartTimer,SIGNAL(timeout()),this,SLOT(asyncCompleterCreation()));
		m_pStartTimer->start(500);
	} else loadCompleterFromFile();
}

KviScriptEditorWidget::~KviScriptEditorWidget()
{
	if(m_pCompleter)
		delete m_pCompleter;
}

void KviScriptEditorWidget::asyncCompleterCreation()
{
	static int iIndex = 0;
	static int iModulesCount = 0;
	if(!iIndex)
	{
		m_pListCompletition = new QStringList();
		QString szPath;
		g_pApp->getGlobalKvircDirectory(szPath,KviApp::Plugins);
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
		QString szBuffer = m_pListCompletition->join(",");
		QFile f(szPath);
		f.open(QIODevice::WriteOnly);
		f.write(szBuffer.toUtf8());
		f.close();
		KviKvsKernel::instance()->getAllFunctionsCommandsCore(m_pListCompletition);
		createCompleter(*m_pListCompletition);
		iIndex = 0;
		iModulesCount = 0;
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
	debug ("append %s to %s",szTmp.toUtf8().data(),m_pCompleter->completionPrefix().toUtf8().data());
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
	pMenu->addAction(__tr2qs("Context sensitive help"),this,SLOT(slotHelp()),Qt::CTRL+Qt::Key_H);
	pMenu->addAction(__tr2qs("&Replace"),this,SLOT(slotReplace()),Qt::CTRL+Qt::Key_R);
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
	KviScriptEditorReplaceDialog * pDialog = new KviScriptEditorReplaceDialog(this,__tr("Find & Replace"));
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
	// this will rehighlight everything
	setText(toPlainText()); // an "hack" to ensure Update all in the editor
	KviScriptEditorSyntaxHighlighter *h = new KviScriptEditorSyntaxHighlighter(this);
	(void)h;
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
			case Qt::Key_Backspace:
			case Qt::Key_PageUp:
				 e->ignore(); // allow the parent to process it
				 return;
			break;
		}
	}
// Adapted from QT4 QCompleter example
	bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
	if (!m_pCompleter || !isShortcut) // dont process the shortcut when we have a completer
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
    if (completionPrefix != m_pCompleter->completionPrefix()) {
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
	tc.movePosition(QTextCursor::StartOfWord);
	if(tc.atBlockStart())
	{
		tc.movePosition(QTextCursor::EndOfWord,QTextCursor::KeepAnchor);
		if(tc.atBlockEnd())
			return tc.selectedText();

		tc.movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor);
		szWord = tc.selectedText();
		if(szWord.right(1)!=".")
			szWord.chop(1);

		return szWord;
	}

	tc.movePosition(QTextCursor::PreviousCharacter);
	tc.movePosition(QTextCursor::EndOfWord,QTextCursor::KeepAnchor);
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

void KviScriptEditorWidget::mouseReleaseEvent(QMouseEvent * e)
{
	return;
	//completelistbox->hide();
	if (e->button() == Qt::RightButton)
	{
		QString szBuffer;
		/*
		int iPara = paragraphAt(e->pos());
		int iIndex=charAt(e->pos(),&iPara);
		szBuffer = this->text(iPara);
		getWordOnCursor(szBuffer,iIndex);
		*/
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

		for (int i=0; i < pList->count(); i++)
			QString str = *(pList->at(i));

		KviKvsKernel::instance()->freeCompletionResult(pList);
		m_szHelp = szBuffer;
	}

	QTextEdit::mouseReleaseEvent(e);
}

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
	KviTalGroupBox * gbox = new KviTalGroupBox(Qt::Horizontal,__tr2qs("Colors" ),box);
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

	QPushButton * b = new QPushButton(__tr2qs_ctx("&OK","editor"),hbox);
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
}

KviScriptEditorSyntaxHighlighter::~KviScriptEditorSyntaxHighlighter()
{
}

#define IN_COMMENT 1
#define IN_LINE 2
#define IN_STRING 4

void KviScriptEditorSyntaxHighlighter::highlightBlock(const QString & szText)
{
	const QChar * pBuf = (const QChar *)szText.utf16();
	const QChar * c = pBuf;

	QTextCharFormat commentFormat;
	commentFormat.setForeground(g_clrComment);
	commentFormat.setFont(g_fntNormal);

	QTextCharFormat bracketFormat;
	bracketFormat.setForeground(g_clrBracket);
	bracketFormat.setFont(g_fntNormal);

	QTextCharFormat punctuationFormat;
	punctuationFormat.setForeground(g_clrPunctuation);
	punctuationFormat.setFont(g_fntNormal);

	QTextCharFormat keywordFormat;
	keywordFormat.setForeground(g_clrKeyword);
	keywordFormat.setFont(g_fntNormal);

	QTextCharFormat variableFormat;
	variableFormat.setForeground(g_clrVariable);
	variableFormat.setFont(g_fntNormal);

	QTextCharFormat normaltextFormat;
	normaltextFormat.setForeground(g_clrNormalText);
	normaltextFormat.setFont(g_fntNormal);

	QTextCharFormat functionFormat;
	functionFormat.setForeground(g_clrFunction);
	functionFormat.setFont(g_fntNormal);

	QTextCharFormat findFormat;
	findFormat.setForeground(g_clrFind);
	findFormat.setFont(g_fntNormal);

	int iEndStateOfLastPara = previousBlockState();
	if(iEndStateOfLastPara < 0)
		iEndStateOfLastPara = 0;
	/*
	QRegExp reg("[$](|[a-zA-Z]|[a-zA-Z]+[a-zA-Z0-9_\.:]*)");
	int index = text.indexOf(reg);
	while (index >= 0)
	{
		int length = reg.matchedLength();
		setFormat(index, length, functionFormat);
		index = text.indexOf(reg, index + length);
	}

	QRegExp reg1("[{}]+");
	index = text.indexOf(reg1);
	while (index >= 0)
	{
		int length = reg1.matchedLength();
		setFormat(index, length, bracketFormat);
		index = text.indexOf(reg1, index + length);
	}
	*/

	bool bNewCommand = !(iEndStateOfLastPara & IN_LINE);
	bool bInComment = iEndStateOfLastPara & IN_COMMENT;
	bool bInString = iEndStateOfLastPara & IN_STRING;
	if (bInComment) debug ("start incomment %s",szText.toUtf8().data());
	const QChar * pBegin;

	while(c->unicode())
	{
		if(bInComment)
		{
			pBegin = c;
			while(c->unicode() && (c->unicode() != '*'))c++;
			if(!c->unicode())
			{
				setFormat(pBegin - pBuf,c - pBegin,commentFormat);
				setCurrentBlockState(IN_COMMENT);
				return;
			}
			c++;
			if(c->unicode() == '/')
			{
				// end of the comment!
				c++;
				setFormat(pBegin - pBuf,c - pBegin,commentFormat);
				bInComment = false;
				bNewCommand = true;
					setCurrentBlockState(0);
			}
			continue;
		}

		if(c->isSpace())
		{
			while(c->unicode() && c->isSpace())c++;
			if(!c->unicode())continue;
		}

		pBegin = c;

		// this does not break the bNewCommand flag
		if((c->unicode() == '{') || (c->unicode() == '}'))
		{
			c++;
			setFormat(pBegin - pBuf,1,bracketFormat);
			continue;
		}

		if(bNewCommand)
		{
			bNewCommand = false;

			if(c->unicode() == '#')
			{
				if(c > pBuf)
				{
					const QChar * prev = c - 1;
					if((prev->unicode() == ']') || (prev->unicode() == '}'))
					{
						// array or hash count
						c++;
						setFormat(pBegin - pBuf,c - pBegin,punctuationFormat);
						continue;
					}
				}
				// comment until the end of the line
				while(c->unicode())c++;
				setFormat(pBegin - pBuf,c - pBegin,commentFormat);
				continue;
			}

			if(c->unicode() == '/')
			{
				c++;
				if(c->unicode() == '/')
				{
					while(c->unicode())c++;
					setFormat(pBegin - pBuf,c - pBegin,commentFormat);
					continue;
				} else if(c->unicode() == '*')
				{
					c++;
					setFormat(pBegin - pBuf,c - pBegin,commentFormat);
					bInComment = true;
					continue;
				}
				c--;
			}

			if(c->unicode() && (c->isLetterOrNumber() || (c->unicode() == '_')))
			{
				c++;
				while(c->unicode() && (c->isLetterOrNumber() || (c->unicode() == '.') || (c->unicode() == '_') || (c->unicode() == ':')))c++;
				setFormat(pBegin - pBuf,c - pBegin,keywordFormat);
				// special processing for callbacks and magic commands
				if(pBegin->unicode() == 'e')
				{
					if(c - pBegin == 4)
					{
						// might be "else"
						QString tmp(pBegin,4);
						if(tmp.toLower() == "else")bNewCommand = true;
						continue;
					}
				}
				else
				if(pBegin->unicode() == 'f')
				{
					if(c - pBegin == 8)
					{
						// might be "function"
						QString tmp(pBegin,8);
						if(tmp.toLower() == "function")bNewCommand = true;
						continue;
					}
				}

				if(pBegin->unicode() == 'i')
				{
					if(c - pBegin == 8)
					{
						// might be "internal"
						QString tmp(pBegin,8);
						if(tmp.toLower() == "internal")bNewCommand = true;
						continue;
					}
				}

				// not an else or special command function... FIXME: should check for callbacks.. but that's prolly too difficult :)
				continue;
			}
		}

		if(c->unicode() == '$')
		{
			c++;
			if(c->unicode() == '$')
			{
				c++;
				setFormat(pBegin - pBuf,c - pBegin,keywordFormat);
			} else {
				while(c->unicode() && (c->isLetterOrNumber() || (c->unicode() == '.') || (c->unicode() == '_') || (c->unicode() == ':')))c++;
				setFormat(pBegin - pBuf,c - pBegin,functionFormat);
			}
			continue;
		}

		if(c->unicode() == '-')
		{
			QChar * pTmp =(QChar *) c;
			c++;
			if(c->unicode() == '-')	c++;
			if(c->isLetter())
			{
				while(c->unicode() && (c->isLetterOrNumber() || (c->unicode() == '_')))c++;
				setFormat(pBegin - pBuf,c - pBegin,keywordFormat);
				continue;
			} else {
				while(c!=pTmp) c--;
			}
		}

		if(c->unicode() == '%')
		{
			c++;
			if(c->unicode() && (c->isLetterOrNumber() || (c->unicode() == ':') || (c->unicode() == '_')))
			{
				while(c->unicode() && (c->isLetterOrNumber() || (c->unicode() == ':') || (c->unicode() == '_')))c++;
				setFormat(pBegin - pBuf,c - pBegin,variableFormat);
				continue;
			}
			c--;
		}

		if(!c->unicode())continue;

		if(c->isLetterOrNumber() || c->unicode() == '_')
		{
			c++;
			while(c->unicode() && c->isLetterOrNumber() || (c->unicode() == '_'))c++;
			setFormat(pBegin - pBuf,c - pBegin,normaltextFormat);
			continue;
		}

		if(c->unicode() == '\\')
		{
			c++;
			setFormat(pBegin - pBuf,c - pBegin,punctuationFormat);
			// the next char is to be interpreted as normal text
			pBegin = c;
			if(c->unicode() && (c->unicode() != '\n'))
			{
				c++;
				setFormat(pBegin - pBuf,c - pBegin,normaltextFormat);
				continue;
			}
			// this is never returned since Qt sux in string processing
			// it sets the newlines to spaces and we have no secure way to undestand that this was the end of a line
			setCurrentBlockState(IN_LINE);
			return;
		}

		if(c->unicode() == '"')
		{
			bInString = !bInString;
			c++;
			setFormat(pBegin - pBuf,c - pBegin,punctuationFormat);
			continue;
		} else if(c->unicode() == ';')
		{
			if(!bInString)	bNewCommand = true; // the next will be a new command
		}

		c++;
		if(bInString)
		{
			setFormat(pBegin - pBuf,c - pBegin,normaltextFormat);
		} else {
			setFormat(pBegin - pBuf,c - pBegin,punctuationFormat);
		}
	}

	bool bCheck = true;
	KviScriptEditorWidget * pEditor = ((KviScriptEditorWidget *)textEdit());
	QString szFind = pEditor->m_szFind;
	if (!szFind.isEmpty())
	{
		int iIndex = 0;
		while(bCheck)
		{
			iIndex = szText.indexOf(szFind,iIndex,Qt::CaseInsensitive);
			if (iIndex != -1)
			{
				setFormat(iIndex,szFind.length(),findFormat);
				iIndex += szFind.length();
			} else bCheck = false;
		}
	}

	if(bInString)
		setCurrentBlockState(IN_LINE | IN_STRING);
}


KviScriptEditorImplementation::KviScriptEditorImplementation(QWidget * par)
:KviScriptEditor(par)
{
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
	b->setArrowType(Qt::DownArrow);
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

	QLabel *lab= new QLabel("find",this);
	lab->setText(tr("Find"));
	g->addWidget(lab,1,1);

	m_pRowColLabel = new QLabel("0",this);
	m_pRowColLabel->setFrameStyle(QFrame::Sunken | QFrame::Panel);
	m_pRowColLabel->setMinimumWidth(80);
	g->addWidget(m_pRowColLabel,1,3);

	connect(m_pFindLineEdit,SIGNAL(returnPressed()),m_pEditor,SLOT(slotFind()));
	connect(m_pFindLineEdit,SIGNAL(returnPressed()),this,SLOT(slotFind()));
	connect(m_pEditor,SIGNAL(keyPressed()),this,SLOT(updateRowColLabel()));
	connect(m_pEditor,SIGNAL(textChanged()),this,SLOT(updateRowColLabel()));
	connect(m_pEditor,SIGNAL(selectionChanged()),this,SLOT(updateRowColLabel()));
	m_lastCursorPos = 0;
}

KviScriptEditorImplementation::~KviScriptEditorImplementation()
{
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
	g_fntNormal = cfg.readFontEntry("Font",QFont("Fixed",12));
}

bool KviScriptEditorImplementation::isModified()
{
	return m_pEditor->document()->isModified();
}

void KviScriptEditorImplementation::slotFind()
{
	emit find(m_pFindLineEdit->text());
}

void KviScriptEditorImplementation::slotNextFind(const QString & szText)
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

void KviScriptEditorImplementation::focusInEvent(QFocusEvent * e)
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

void KviScriptEditorImplementation::setText(const KviQCString & szText)
{
	m_pEditor->setText(szText.data());
	m_pEditor->document()->setModified(false);
	updateRowColLabel();
}

void KviScriptEditorImplementation::getText(KviQCString & szText)
{
	szText = m_pEditor->toPlainText().toUtf8();
}

void KviScriptEditorImplementation::setText(const QString & szText)
{
	m_pEditor->setText(szText);
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

void KviScriptEditorImplementation::setFindLineEditReadOnly(bool b)
{
	m_pFindLineEdit->setReadOnly(b);
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
	debug ("Moving to %d",iPos);
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
			m_pEditor->setText(szBuffer);
			setCursorPosition(0);
			//m_pEditor->moveCursor(KviTalTextEdit::MoveEnd,false);
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
	KviScriptEditorWidgetColorOptions dlg(this);
	if(dlg.exec() == QDialog::Accepted)
	{
		m_pEditor->updateOptions();
		saveOptions();
	}
}

KviScriptEditorReplaceDialog::KviScriptEditorReplaceDialog(QWidget * parent, const char * name)
: QDialog(parent)
{
	m_pParent = parent;
	emit initFind();
	QPalette p = palette();
	p.setColor(foregroundRole(),QColor( 0, 0, 0 ));
	p.setColor(backgroundRole(),QColor( 236, 233, 216 ));
	setPalette(p);

	QGridLayout * pLayout = new QGridLayout(this);
	pLayout->setObjectName("replace layout");

	QLabel * m_pFindLabel = new QLabel(this);
	m_pFindLabel->setObjectName("findlabel");
	m_pFindLabel->setText(tr("Word to Find"));
	pLayout->addWidget(m_pFindLabel,0,0);

	m_pFindLineEdit = new QLineEdit(this);
	m_pFindLineEdit->setObjectName("findlineedit");
	pLayout->addWidget(m_pFindLineEdit,0,1);

	QPushButton * pFindNext = new QPushButton(this);
	pFindNext->setObjectName("findnext");
	pFindNext->setText(tr("&Find Next(WIP)"));
	pFindNext->setEnabled(false);
	pLayout->addWidget(pFindNext,0,2);

	QLabel * m_pReplaceLabel = new QLabel(this);
	m_pReplaceLabel->setObjectName("replacelabel");
	m_pReplaceLabel->setText(tr("Replace with"));
	pLayout->addWidget(m_pReplaceLabel,1,0);

	m_pReplaceLineEdit = new QLineEdit(this);
	m_pReplaceLineEdit->setObjectName("replacelineedit");
	pLayout->addWidget(m_pReplaceLineEdit,1,1);

	QPushButton * pReplace = new QPushButton(this);
	pReplace->setObjectName("replace");
	pReplace->setText(tr("&Replace(WIP)"));
	pReplace->setEnabled(false);
	pLayout->addWidget(pReplace,1,2);

	m_pFindLineEdit->setFocus();

	m_pCheckReplaceAll = new QCheckBox(this);
	m_pCheckReplaceAll->setObjectName("replaceAll");
	m_pCheckReplaceAll->setText(tr("&Replace in all Aliases"));
	pLayout->addWidget(m_pCheckReplaceAll,2,0);

	QPushButton * pCancelButton = new QPushButton(this);
	pCancelButton->setObjectName("cancelButton");
	pCancelButton->setText(tr("&Cancel"));
	pLayout->addWidget(pCancelButton,3,1);

	m_pReplaceButton = new QPushButton(this);
	m_pReplaceButton->setObjectName("replacebutton");
	m_pReplaceButton->setText(tr("&Replace"));
	m_pReplaceButton->setEnabled(false);
	pLayout->addWidget(m_pReplaceButton,3,2);

	setLayout(pLayout);

	// signals and slots connections
	connect(m_pReplaceButton, SIGNAL(clicked()), this, SLOT(slotReplace()));
	connect(pFindNext, SIGNAL(clicked()),this,SLOT(slotNextFind()));
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
