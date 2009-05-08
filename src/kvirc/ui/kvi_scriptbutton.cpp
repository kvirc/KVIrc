//=============================================================================
//
//   File : kvi_scriptbutton.cpp
//   Creation date : Wed Nov 14 15:43:41 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_scriptbutton.h"
#include "kvi_irctoolbar.h"
#include "kvi_window.h"
#include "kvi_console.h"
#include "kvi_app.h"
#include "kvi_parameterlist.h"
#include "kvi_kvs_variantlist.h"
#include "kvi_kvs_script.h"


KviScriptUserButton::KviScriptUserButton(QWidget * par,const char * name)
: QToolButton(par)
{
	m_pScript = 0;
	setObjectName(name);
//	setAutoRaise(true);
	connect(this,SIGNAL(clicked()),this,SLOT(btnClicked()));
	setAutoRaise(true);
}

KviScriptUserButton::~KviScriptUserButton()
{
	if(m_pScript)delete m_pScript;
}

KviWindow * KviScriptUserButton::window()
{
	return g_pActiveWindow;
}

void KviScriptUserButton::setButtonCode(KviKvsScript * pScript)
{
	if(m_pScript)delete m_pScript;
	m_pScript = pScript;
}


void KviScriptUserButton::btnClicked()
{
	KviWindow * pWnd = window();
	if(!pWnd)return; // ops...
	QPoint pos = mapToGlobal(QPoint(0,height()));

	KviKvsVariantList vList;
	vList.append((kvs_int_t)pos.x());
	vList.append((kvs_int_t)pos.y());
	m_pScript->run(pWnd,&vList,0,KviKvsScript::PreserveParams);
}


KviWindowScriptButton::KviWindowScriptButton(QWidget * p,KviWindow * wnd,const char * name)
: KviScriptUserButton(p,name)
{
	m_pWnd = wnd;
}

KviWindowScriptButton::~KviWindowScriptButton()
{
}

#ifndef COMPILE_USE_STANDALONE_MOC_SOURCES
#include "kvi_scriptbutton.moc"
#endif //!COMPILE_USE_STANDALONE_MOC_SOURCES
