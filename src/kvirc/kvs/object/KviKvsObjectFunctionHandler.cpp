//=============================================================================
//
//   File : KviKvsObjectFunctionHandler.cpp
//   Creation date : Sat 23 Apr 2005 21:53:23 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviKvsObjectFunctionHandler.h"

KviKvsObjectFunctionHandler::KviKvsObjectFunctionHandler(unsigned int uFlags)
    : m_uFlags(uFlags)
{
	m_bClone = false;
}

KviKvsObjectFunctionHandler::~KviKvsObjectFunctionHandler()
    = default;

bool KviKvsObjectFunctionHandler::isScriptHandler()
{
	return false;
}

QString KviKvsObjectFunctionHandler::scriptHandlerCode()
{
	return QString();
}
QString KviKvsObjectFunctionHandler::reminder()
{
	return QString();
}
void KviKvsObjectFunctionHandler::setReminder(const QString &)
{
}

bool KviKvsObjectFunctionHandler::isClone()
{
	return m_bClone;
}
