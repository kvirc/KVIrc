#ifndef _CLASS_SQLITE_H_
#define _CLASS_SQLITE_H_
//=============================================================================
//
//   File : KvsObject_sql.h
//   Creation date : Wed Gen 28 2009 21:07:55 by Alessandro Carbone
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_settings.h"
#include "KviCString.h"
#include "KviPointerList.h"
#include "KviKvsVariant.h"
#include "object_macros.h"
#include <QSqlQuery>
#include <QHash>

class KvsObject_sql : public KviKvsObject
{
public:
	KVSO_DECLARE_OBJECT(KvsObject_sql)
protected:
	QSqlQuery * m_pCurrentSQlQuery;
	QString mSzConnectionName;

public:
	bool setConnection(KviKvsObjectFunctionCall * c);
	bool connectionNames(KviKvsObjectFunctionCall * c);
	bool features(KviKvsObjectFunctionCall * c);
	bool queryLastInsertId(KviKvsObjectFunctionCall * c);

	bool tablesList(KviKvsObjectFunctionCall * c);
	bool beginTransaction(KviKvsObjectFunctionCall * c);
	bool commit(KviKvsObjectFunctionCall * c);

	bool queryResultsSize(KviKvsObjectFunctionCall * c);
	bool queryNext(KviKvsObjectFunctionCall * c);
	bool queryPrevious(KviKvsObjectFunctionCall * c);
	bool queryFirst(KviKvsObjectFunctionCall * c);
	bool queryLast(KviKvsObjectFunctionCall * c);
	bool queryExec(KviKvsObjectFunctionCall * c);
	bool queryRecord(KviKvsObjectFunctionCall * c);
	bool queryPrepare(KviKvsObjectFunctionCall * c);
	bool queryBindValue(KviKvsObjectFunctionCall * c);
	bool queryFinish(KviKvsObjectFunctionCall * c);
	bool closeConnection(KviKvsObjectFunctionCall * c);
	bool lastError(KviKvsObjectFunctionCall * c);
};

#endif //_CLASS_SQLITE_H_
