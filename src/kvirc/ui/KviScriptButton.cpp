//=============================================================================
//
//   File : KviScriptButton.cpp
//   Creation date : Wed Nov 14 15:43:41 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviScriptButton.h"
#include "KviApplication.h"
#include "KviConsoleWindow.h"
#include "KviIrcToolBar.h"
#include "KviKvsScript.h"
#include "KviKvsVariantList.h"
#include "KviWindow.h"

#include <QString>

KviScriptUserButton::KviScriptUserButton(QWidget * par, const char * name)
    : QToolButton(par)
{
	m_pScript = nullptr;
	setObjectName(name);
	//	setAutoRaise(true);
	connect(this, SIGNAL(clicked()), this, SLOT(btnClicked()));
	setAutoRaise(true);
}

KviScriptUserButton::~KviScriptUserButton()
{
	if(m_pScript)
		delete m_pScript;
}

KviWindow * KviScriptUserButton::window()
{
	return g_pActiveWindow;
}

void KviScriptUserButton::setButtonCode(KviKvsScript * pScript)
{
	if(m_pScript)
		delete m_pScript;
	m_pScript = pScript;
}

void KviScriptUserButton::btnClicked()
{
	KviWindow * pWnd = window();
	if(!pWnd)
		return; // ops...
	QPoint pos = mapToGlobal(QPoint(0, height()));

	KviKvsVariantList vList;
	vList.append((kvs_int_t)pos.x());
	vList.append((kvs_int_t)pos.y());
	m_pScript->run(pWnd, &vList, nullptr, KviKvsScript::PreserveParams);
}

KviWindowScriptButton::KviWindowScriptButton(QWidget * p, KviWindow * wnd, const char * name)
    : KviScriptUserButton(p, name)
{
	m_pWnd = wnd;
}

KviWindowScriptButton::~KviWindowScriptButton()
    = default;
