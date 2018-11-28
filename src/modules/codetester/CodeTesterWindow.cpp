//=============================================================================
//
//   File : CodeTesterWindow.cpp
//   Creation date : Mon Dec 23 2002 20:28:18 by Szymon Stefanek
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

#include "CodeTesterWindow.h"

#include "KviIconManager.h"
#include "KviOptions.h"
#include "KviLocale.h"
#include "KviConfigurationFile.h"
#include "KviFileUtils.h"
#include "KviScriptEditor.h"
#include "KviApplication.h"
#include "KviConsoleWindow.h"
#include "KviKvsScript.h"
#include "KviKvsVariantList.h"
#include "KviIrcView.h"

#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <QMenu>

#include <unordered_set>

extern std::unordered_set<CodeTesterWindow *> g_pCodeTesterWindowList;

CodeTesterWindow::CodeTesterWindow()
    : KviWindow(KviWindow::ScriptEditor, "codetester", nullptr)
{
	g_pCodeTesterWindowList.insert(this);
	setObjectName("code_tester");

	m_pSplitter = new KviTalSplitter(Qt::Horizontal, this);
	m_pSplitter->setObjectName("main_splitter");
	m_pSplitter->setChildrenCollapsible(false);

	// layouts can't be added to splitters directly, so we embed the layout in a widget.
	QWidget * l = new QWidget(this);
	QGridLayout * g = new QGridLayout(l);
	m_pEditor = KviScriptEditor::createInstance(this);
	g->addWidget(m_pEditor, 0, 0, 1, 4);

	m_pExecuteButton = new QPushButton(__tr2qs_ctx("&Execute", "editor"), this);
	g->addWidget(m_pExecuteButton, 1, 3);
	connect(m_pExecuteButton, SIGNAL(clicked()), this, SLOT(execute()));

	m_pModeLabel = new QLabel(__tr2qs_ctx("Params:", "editor"), this);
	g->addWidget(m_pModeLabel, 1, 1);
	m_pParams = new QLineEdit(this);
	m_pParams->setToolTip(__tr2qs_ctx("Here you can specify a semicolon-separated list of parameters that will be available in the code as $0, $1, $2, ..", "editor"));
	g->addWidget(m_pParams, 1, 2);

	m_pSplitter->addWidget(l);

	m_pIrcView = new KviIrcView(m_pSplitter, this);

	QList<int> li { width() / 2, width() / 2 };
	m_pSplitter->setSizes(li);
}

CodeTesterWindow::~CodeTesterWindow()
{
	KviScriptEditor::destroyInstance(m_pEditor);
	g_pCodeTesterWindowList.erase(this);
}

void CodeTesterWindow::execute()
{
	QString buffer;
	m_pEditor->getText(buffer);
	QStringList slParams = m_pParams->text().split(';');
	KviKvsVariantList params{&slParams};
	KviKvsScript::run(buffer, this, &params);
}

QPixmap * CodeTesterWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KviIconManager::Bomb);
}

void CodeTesterWindow::resizeEvent(QResizeEvent *)
{
	m_pSplitter->setGeometry(0, 0, width(), height());
}

void CodeTesterWindow::fillCaptionBuffers()
{
	m_szPlainTextCaption = __tr2qs_ctx("Script Tester", "editor");
}

void CodeTesterWindow::getConfigGroupName(QString & szName)
{
	szName = "codetester";
}
