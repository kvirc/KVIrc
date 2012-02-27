//=============================================================================
//
//   File : CodeTesterWindow.cpp
//   Creation date : Mon Dec 23 2002 20:28:18 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2002-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "CodeTesterWindow.h"

#include "KviIconManager.h"
#include "KviOptions.h"
#include "KviLocale.h"
#include "KviConfigurationFile.h"
#include "KviFileDialog.h"
#include "KviFileUtils.h"
#include "KviScriptEditor.h"
#include "KviApplication.h"
#include "KviConsoleWindow.h"
#include "KviParameterList.h"
#include "KviKvsScript.h"
#include "KviKvsVariantList.h"

#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <QMenu>


extern KviPointerList<CodeTesterWindow> * g_pCodeTesterWindowList;


CodeTesterWidget::CodeTesterWidget(QWidget * par)
: QWidget(par)
{
	setObjectName("code_tester");
	QGridLayout * g = new QGridLayout(this);
	m_pEditor = KviScriptEditor::createInstance(this);
	g->addWidget(m_pEditor,0,0,1,4);
	//g->addMultiCellWidget(m_pEditor,0,0,0,3);
	m_pExecuteButton = new QPushButton(__tr2qs_ctx("&Execute","editor"),this);
	g->addWidget(m_pExecuteButton,1,3);
	connect(m_pExecuteButton,SIGNAL(clicked()),this,SLOT(execute()));

	m_pModeLabel = new QLabel(__tr2qs_ctx("Params:","editor"),this);
	g->addWidget(m_pModeLabel,1,1);
	m_pParams = new QLineEdit(this);
	m_pParams->setToolTip(__tr2qs_ctx("Here you can specify a semicolon-separated list of parameters that will be available in the code as $0, $1, $2, ..","editor"));
	g->addWidget(m_pParams,1,2);
}

CodeTesterWidget::~CodeTesterWidget()
{
	KviScriptEditor::destroyInstance(m_pEditor);
}

//#warning "Allow to bind the command to a specified window"

void CodeTesterWidget::execute()
{
	QString buffer;
	m_pEditor->getText(buffer);
	KviConsoleWindow * pConsole = g_pApp->activeConsole();
	QStringList * pSLParams = new QStringList(m_pParams->text().split(';'));
	KviKvsScript::run(buffer, pConsole, new KviKvsVariantList(pSLParams));
	delete pSLParams;
}

CodeTesterWindow::CodeTesterWindow()
: KviWindow(KviWindow::ScriptEditor,"codetester",0)
{
	g_pCodeTesterWindowList->append(this);

	m_pTester = new CodeTesterWidget(this);
}

CodeTesterWindow::~CodeTesterWindow()
{
	g_pCodeTesterWindowList->removeRef(this);
}

QPixmap * CodeTesterWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KviIconManager::Bomb);
}

void CodeTesterWindow::resizeEvent(QResizeEvent *)
{
	m_pTester->setGeometry(0,0,width(),height());
}

void CodeTesterWindow::fillCaptionBuffers()
{
	m_szPlainTextCaption = __tr2qs_ctx("Script Tester","editor");
}

void CodeTesterWindow::getConfigGroupName(QString &szName)
{
	szName = "codetester";
}

void CodeTesterWindow::saveProperties(KviConfigurationFile *) //cfg
{
/*
#ifdef COMPILE_SCRIPTTOOLBAR
	cfg->writeEntry("Sizes",m_pEditor->sizes());
	cfg->writeEntry("LastRaw",m_pEditor->lastEditedRaw().ptr());
	//qDebug("LAST EDITED=%s",m_pEditor->lastEditedRaw().ptr());
#endif // COMPILE_SCRIPTTOOLBAR
*/
}

void CodeTesterWindow::loadProperties(KviConfigurationFile *) //cfg
{
/*
#ifdef COMPILE_SCRIPTTOOLBAR
	QValueList<int> def;
	def.append(20);
	def.append(80);
	m_pEditor->setSizes(cfg->readIntListEntry("Sizes",def));
	KviCString tmp = cfg->readEntry("LastRaw","");
	m_pEditor->editRaw(tmp);
	//qDebug("LAST EDITED WAS %s",tmp.ptr());
#endif // COMPILE_SCRIPTTOOLBAR
*/
}
