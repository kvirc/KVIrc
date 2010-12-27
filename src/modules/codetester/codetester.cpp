//=============================================================================
//
//   File : codetester.cpp
//   Creation date : Mon Dec 23 2002 20:28:18 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
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

#include "codetester.h"

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
#include "KviTalPopupMenu.h"

#include <QPushButton>
#include <QLayout>
#include <QLabel>


extern KviPointerList<KviCodeTesterWindow> * g_pCodeTesterWindowList;


KviCodeTester::KviCodeTester(QWidget * par)
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
	g->addWidget(m_pParams,1,2);
}

KviCodeTester::~KviCodeTester()
{
	KviScriptEditor::destroyInstance(m_pEditor);
}

//#warning "Allow to bind the command to a specified window"

void KviCodeTester::execute()
{
	QString parms = m_pParams->text();
	QString buffer;
	m_pEditor->getText(buffer);
	KviConsole * con = g_pApp->activeConsole();
	//KviParameterList * l = new KviParameterList(parms.ptr());
	KviKvsScript::run(buffer,con,new KviKvsVariantList(new QString(parms)));
}

KviCodeTesterWindow::KviCodeTesterWindow(KviMainWindow * lpFrm)
: KviWindow(KVI_WINDOW_TYPE_SCRIPTEDITOR,lpFrm,"codetester",0)
{
	g_pCodeTesterWindowList->append(this);

	m_pTester = new KviCodeTester(this);
}

KviCodeTesterWindow::~KviCodeTesterWindow()
{
	g_pCodeTesterWindowList->removeRef(this);
}

QPixmap * KviCodeTesterWindow::myIconPtr()
{
	return g_pIconManager->getSmallIcon(KVI_SMALLICON_BOMB);
}

void KviCodeTesterWindow::resizeEvent(QResizeEvent *)
{
	m_pTester->setGeometry(0,0,width(),height());
}

void KviCodeTesterWindow::fillCaptionBuffers()
{
	m_szPlainTextCaption = __tr2qs_ctx("Script Tester","editor");
}

void KviCodeTesterWindow::getConfigGroupName(QString &szName)
{
	szName = "codetester";
}

void KviCodeTesterWindow::saveProperties(KviConfigurationFile *) //cfg
{
/*
#ifdef COMPILE_SCRIPTTOOLBAR
	cfg->writeEntry("Sizes",m_pEditor->sizes());
	cfg->writeEntry("LastRaw",m_pEditor->lastEditedRaw().ptr());
	//qDebug("LAST EDITED=%s",m_pEditor->lastEditedRaw().ptr());
#endif // COMPILE_SCRIPTTOOLBAR
*/
}

void KviCodeTesterWindow::loadProperties(KviConfigurationFile *) //cfg
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
