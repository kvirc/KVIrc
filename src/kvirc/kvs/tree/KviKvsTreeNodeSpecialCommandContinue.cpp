//=============================================================================
//
//   File : KviKvsTreeNodeSpecialCommandContinue.cpp
//   Creation date : Fri 13 Jun 2008 01:25:00 by Elvio Basello
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2008 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviKvsTreeNodeSpecialCommandContinue.h"
#include "KviKvsRunTimeContext.h"
#include "KviLocale.h"

KviKvsTreeNodeSpecialCommandContinue::KviKvsTreeNodeSpecialCommandContinue(const QChar * pLocation)
    : KviKvsTreeNodeSpecialCommand(pLocation, "continue")
{
}

KviKvsTreeNodeSpecialCommandContinue::~KviKvsTreeNodeSpecialCommandContinue()
    = default;

void KviKvsTreeNodeSpecialCommandContinue::contextDescription(QString & szBuffer)
{
	szBuffer = "Special Command \"continue\"";
}

void KviKvsTreeNodeSpecialCommandContinue::dump(const char * prefix)
{
	qDebug("%s SpecialCommandContinue", prefix);
}

bool KviKvsTreeNodeSpecialCommandContinue::execute(KviKvsRunTimeContext * c)
{
	c->setContinuePending();
	return false;
}
