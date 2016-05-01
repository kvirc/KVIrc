//=============================================================================
//
//   File : KviKvsEvent.cpp
//   Creation date : Mon 23 Feb 2004 03:00:02 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2004-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviKvsEvent.h"

KviKvsEvent::~KviKvsEvent()
{
	clear();
}

void KviKvsEvent::clear()
{
	if(m_pHandlers)
		delete m_pHandlers;
	m_pHandlers = nullptr;
}

void KviKvsEvent::removeHandler(KviKvsEventHandler * h)
{
	m_pHandlers->removeRef(h);
	if(m_pHandlers->isEmpty())
	{
		delete m_pHandlers;
		m_pHandlers = nullptr;
	}
}

void KviKvsEvent::addHandler(KviKvsEventHandler * h)
{
	if(!m_pHandlers)
	{
		m_pHandlers = new KviPointerList<KviKvsEventHandler>();
		m_pHandlers->setAutoDelete(true);
	}
	m_pHandlers->append(h);
}

void KviKvsEvent::clearScriptHandlers()
{
	if(!m_pHandlers)
		return;
	KviPointerList<KviKvsEventHandler> dl;
	dl.setAutoDelete(false);
	KviKvsEventHandler * e;
	for(e = m_pHandlers->first(); e; e = m_pHandlers->next())
	{
		if(e->type() == KviKvsEventHandler::Script)
			dl.append(e);
	}
	for(e = dl.first(); e; e = dl.next())
	{
		m_pHandlers->removeRef(e);
	}
	if(m_pHandlers->isEmpty())
	{
		delete m_pHandlers;
		m_pHandlers = nullptr;
	}
}
