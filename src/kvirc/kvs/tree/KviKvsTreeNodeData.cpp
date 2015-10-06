//=============================================================================
//
//   File : KviKvsTreeNodeData.cpp
//   Creation date : Sun 05 Oct 2003 21.52 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "KviKvsTreeNodeData.h"
#include "KviLocale.h"


KviKvsTreeNodeData::KviKvsTreeNodeData(const QChar * pLocation)
: KviKvsTreeNode(pLocation), m_pEndingLocation(0)
{
}


KviKvsTreeNodeData::~KviKvsTreeNodeData()
{
}

void KviKvsTreeNodeData::contextDescription(QString &szBuffer)
{
	szBuffer = "Data evaluation";
}

void KviKvsTreeNodeData::dump(const char * prefix)
{
	qDebug("%s Data",prefix);
}

bool KviKvsTreeNodeData::isReadOnly()
{
	return true;
}

bool KviKvsTreeNodeData::canEvaluateToObjectReference()
{
	return false;
}

bool KviKvsTreeNodeData::canEvaluateInObjectScope()
{
	return false;
}

bool KviKvsTreeNodeData::isFunctionCall()
{
	return false;
}

bool KviKvsTreeNodeData::convertStringConstantToNumeric()
{
	return false;
}

/*
bool KviKvsTreeNodeData::canReleaseResult()
{
	return false;
}
*/

bool KviKvsTreeNodeData::evaluateReadOnly(KviKvsRunTimeContext * c,KviKvsVariant *)
{
	c->error(this,__tr2qs_ctx("Internal error: Pure virtual evaluateReadOnly called","kvs"));
	return false;
}

KviKvsRWEvaluationResult * KviKvsTreeNodeData::evaluateReadWrite(KviKvsRunTimeContext * c)
{
	c->error(this,__tr2qs_ctx("Internal error: Trying to evaluate as read-write a read-only data location","kvs"));
	return 0;
}


bool KviKvsTreeNodeData::evaluateReadOnlyInObjectScope(KviKvsObject *,KviKvsRunTimeContext * c,KviKvsVariant * )
{
	c->error(this,__tr2qs_ctx("Internal error: Pure virtual evaluateReadOnlyInObjectScope called","kvs"));
	return false;
}

KviKvsRWEvaluationResult * KviKvsTreeNodeData::evaluateReadWriteInObjectScope(KviKvsObject *,KviKvsRunTimeContext * c)
{
	c->error(this,__tr2qs_ctx("Internal error: Trying to evaluate as read-write a read-only data location","kvs"));
	return 0;
}
