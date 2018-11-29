//=============================================================================
//
//   File : KviTranslator.cpp
//   Creation date : Wed 23 Mar 2011 03:32:12
//   Originally in KviLocale.cpp
//      Created on: Sat Jan 16 1999 19:08:41 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 1999-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviTranslator.h"
#include "KviLocale.h"

KviTranslator::KviTranslator(QObject * pParent)
    : QTranslator(pParent)
{
}

KviTranslator::~KviTranslator()
    = default;

QString KviTranslator::translate(const char *, const char * pcMessage, const char *, int n) const
{
	// We currently ignore contexts and comments for qt translations
	// FIXME: Could use the context and lookup in the context catalogue first, then in the main one.
	return g_pMainCatalogue->translateToQString(pcMessage);
}
