//=============================================================================
//
//   File : KvsObject_sql.cpp
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
//============================================================================

#include "kvi_debug.h"
#include "KviMemory.h"
#include "KviLocale.h"
#include "KvsObject_sql.h"
#include "KvsObject_memoryBuffer.h"
#include <cstdlib>
#include <QHash>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlRecord>

#define CHECK_QUERY_IS_INIT                              \
	if(!m_pCurrentSQlQuery)                              \
	{                                                    \
		c->error("No connection has been initialized!"); \
		return false;                                    \
	}

/*
	@doc: sql
	@keyterms:
		Sql database.
	@title:
		sql class
	@type:
		class
	@short:
		A SQL database interface.
	@inherits:
		[class]object[/class]
	@description:
		This class permits KVIrc to have an interface with a SQL database supported by Qt library drivers.
	@functions:
		!fn: <boolean> $setConnection(<database_name:string>,<connection_name:string>,[<user:string>,<host_name_string>,<password:string>,<database_driver:string>])
		Connects to the DBMS using the connection <connection_name> and selecting the database <database_name>.[br]
		If the optional parameter <database_driver> is passed, it will be used the corresponding driver (if present), otherwise SQLite will be used.
		Returns true if the operation is successful, false otherwise.
		!fn:: <array or string> $connectionNames([<stringreturnflag>:'s'])
		Returns as array or, if the flag '[b]s[/b] is passed, as a comma separate string all the database active connection's names.
		!fn: <array> $tablesList(<connection_name:string>)
		Returns as array the database tables list.
		!fn: $transaction()
		Begin a transaction.
		!fn: $commit()
		Commit the transaction.
		!fn: $setCurrentQuery(<connection_name:string>)
		Sets the query for the database connection <connection_name>, which has to be already connected, as current query.
		!fn: <connection_name:string> $currentQuery()
		Returns the name of the database connection for the current query, or an empty string if there aren't any initialized queries.
		!fn: $closeConnection(<connection_name:string>)
		Closes the connection <connection_name>.
		!fn: <size:integer> $queryResultsSize()
		Returns the query size in rows or -1 if the query is empty or the database driver doesn't support the function.
		!fn: <error:string> $lastError(<more_details:boolean>)
		Returns last error occurred. Use the more_details flag for more info about the error.
		!fn: <ok:boolean> $queryExec([<query:string>])
		Execs the current query <query>. The string must follow the right syntax against the database in use.
		If there are no parameters, it will exec the query previously done.
		After the execution, the query will positioned on the first resulting record.
		Returns true if the operation is successful, false otherwise.
		See also [classfnc]$queryPrepare[/classfnc]()
		!fn: <ok:boolean> $queryPrepare(<query_string>)
		Prepare the query <query> to execute. The string must follow the right syntax against the database in use.
		It's possible to use the placeholders. It's supported either the identifier [b]:[/b] and [b]?[/b] but it's not possible to use them together.
		Returns true if the operation is successful, false otherwise.
		See also [classfnc]$queryExec[/classfnc]and[classfnc]$queryBindValue[/classfnc].
		!fn: $queryBindValue()
		Sets the placeholder <placeholder> to be bound to the value <val> in the prepared statement.
		Note that the placeholder mark (e.g [b]:[/b]) must be included when specifying the placeholder name.
		!fn: <boolean> $queryPrevious()
		Sets the current query position to the previous resulting record.
		Returns true if the operation is successful, false otherwise.
		!fn: <boolean> $queryNext()
		Sets the current query position to the next resulting record.
		Returns true if the operation is successful, false otherwise.
		!fn: <boolean> $queryLast()
		Sets the current query position to the last resulting record.
		Returns true if the operation is successful, false otherwise.
		!fn: <boolean> $queryFirst()
		Sets the current query position to the first resulting record.
		Returns true if the operation is successful, false otherwise.
		!fn: <record:hash> $queryRecord()
		Returns a hash containing the current query's record fields.
		!fn: $queryFinish()
		Sets the current query to inactive.
*/

KVSO_BEGIN_REGISTERCLASS(KvsObject_sql, "sql", "object")
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_sql, queryLastInsertId)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_sql, commit)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_sql, beginTransaction)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_sql, setConnection)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_sql, connectionNames)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_sql, tablesList)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_sql, closeConnection)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_sql, queryFinish)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_sql, queryResultsSize)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_sql, queryExec)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_sql, queryPrepare)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_sql, queryBindValue)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_sql, queryPrevious)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_sql, queryNext)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_sql, queryLast)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_sql, queryFirst)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_sql, queryRecord)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_sql, lastError)
KVSO_REGISTER_HANDLER_BY_NAME(KvsObject_sql, features)
KVSO_END_REGISTERCLASS(KvsObject_sql)

KVSO_BEGIN_CONSTRUCTOR(KvsObject_sql, KviKvsObject)

m_pCurrentSQlQuery = nullptr;
KVSO_END_CONSTRUCTOR(KvsObject_sql)

KVSO_BEGIN_DESTRUCTOR(KvsObject_sql)
if(m_pCurrentSQlQuery)
	delete m_pCurrentSQlQuery;
m_pCurrentSQlQuery = nullptr;
KVSO_END_DESTRUCTOR(KvsObject_sql)

KVSO_CLASS_FUNCTION(sql, setConnection)
{
	QString szConnectionName, szDbName, szDbDriver, szUserName, szHostName, szPassword;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("database_name", KVS_PT_STRING, 0, szDbName)
	KVSO_PARAMETER("connection_name", KVS_PT_STRING, KVS_PF_OPTIONAL, szConnectionName)
	KVSO_PARAMETER("user_name", KVS_PT_STRING, KVS_PF_OPTIONAL, szUserName)
	KVSO_PARAMETER("host_name", KVS_PT_STRING, KVS_PF_OPTIONAL, szHostName)
	KVSO_PARAMETER("password", KVS_PT_STRING, KVS_PF_OPTIONAL, szPassword)
	KVSO_PARAMETER("database_type", KVS_PT_STRING, KVS_PF_OPTIONAL, szDbDriver)
	KVSO_PARAMETERS_END(c)
	if(!szDbDriver.isEmpty())
	{
		QStringList drivers = QSqlDatabase::drivers();
		if(!drivers.contains(szDbDriver))
		{
			c->error(__tr2qs_ctx("Missing Qt plugin for database %Q", "objects"), &szDbDriver);
			return false;
		}
	}
	else
		szDbDriver = "QSQLITE";
	QSqlDatabase db;
	db = QSqlDatabase::addDatabase(szDbDriver, szConnectionName);
	mSzConnectionName = szConnectionName;
	db.setDatabaseName(szDbName);
	db.setHostName(szHostName);
	db.setUserName(szUserName);
	db.setPassword(szPassword);
	bool bOk = db.open();
	if(bOk)
	{
		if(m_pCurrentSQlQuery)
			delete m_pCurrentSQlQuery;
		m_pCurrentSQlQuery = new QSqlQuery(db);
	}
	else
		m_pCurrentSQlQuery = nullptr;
	c->returnValue()->setBoolean(bOk);
	return true;
}

KVSO_CLASS_FUNCTION(sql, connectionNames)
{
	QString szFlag;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("stringreturnflag", KVS_PT_STRING, KVS_PF_OPTIONAL, szFlag)
	KVSO_PARAMETERS_END(c)
	QStringList szConnectionsList = QSqlDatabase::connectionNames();
	if(szFlag.indexOf('s', 0, Qt::CaseInsensitive) != -1)
	{
		QString szConnections = szConnectionsList.join(",");
		c->returnValue()->setString(szConnections);
	}
	else
	{
		KviKvsArray * pArray = new KviKvsArray();
		for(int i = 0; i < szConnectionsList.count(); i++)
		{
			pArray->set(i, new KviKvsVariant(szConnectionsList.at(i)));
		}
		c->returnValue()->setArray(pArray);
	}
	return true;
}

KVSO_CLASS_FUNCTION(sql, queryLastInsertId)
{
	CHECK_QUERY_IS_INIT
	QVariant value = m_pCurrentSQlQuery->lastInsertId();
	if(value.type() == QVariant::LongLong)
		c->returnValue()->setInteger((kvs_int_t)value.toLongLong());
	qDebug("type %i", value.type());
	return true;
}
KVSO_CLASS_FUNCTION(sql, features)
{
	QString szConnectionName;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("connectionName", KVS_PT_STRING, 0, szConnectionName)
	KVSO_PARAMETERS_END(c)

	QStringList connections = QSqlDatabase::connectionNames();
	if(!connections.contains(szConnectionName))
	{
		c->warning(__tr2qs_ctx("Connection %Q doesn't exist", "objects"), &szConnectionName);
		return true;
	}
	QSqlDatabase db = QSqlDatabase::database(szConnectionName);
	QSqlDriver * sqlDriver = db.driver();
	QStringList features;
	if(sqlDriver->hasFeature(QSqlDriver::Transactions))
		features.append("transactions");
	if(sqlDriver->hasFeature(QSqlDriver::QuerySize))
		features.append("querysize");
	if(sqlDriver->hasFeature(QSqlDriver::BLOB))
		features.append("blob");
	if(sqlDriver->hasFeature(QSqlDriver::PreparedQueries))
		features.append("preparedqueries");
	if(sqlDriver->hasFeature(QSqlDriver::NamedPlaceholders))
		features.append("namedplaceholders");
	if(sqlDriver->hasFeature(QSqlDriver::PositionalPlaceholders))
		features.append("positionaplaceholders");
	if(sqlDriver->hasFeature(QSqlDriver::LastInsertId))
		features.append("lastinsertid");
	if(sqlDriver->hasFeature(QSqlDriver::BatchOperations))
		features.append("batchoperations");
	if(sqlDriver->hasFeature(QSqlDriver::SimpleLocking))
		features.append("simplelocking");
	if(sqlDriver->hasFeature(QSqlDriver::LowPrecisionNumbers))
		features.append("lowprecisionnumbers");
	if(sqlDriver->hasFeature(QSqlDriver::EventNotifications))
		features.append("eventnotifications");
	if(sqlDriver->hasFeature(QSqlDriver::FinishQuery))
		features.append("finishquery");
	if(sqlDriver->hasFeature(QSqlDriver::MultipleResultSets))
		features.append("multipleresults");
	c->returnValue()->setString(features.join(","));
	return true;
}

KVSO_CLASS_FUNCTION(sql, beginTransaction)
{
	QSqlDatabase db = QSqlDatabase::database(mSzConnectionName);
	if(!db.isValid())
	{
		c->error("No connection has been initialized!");
		return false;
	}
	db.transaction();
	return true;
}
KVSO_CLASS_FUNCTION(sql, commit)
{
	QSqlDatabase db = QSqlDatabase::database(mSzConnectionName);
	if(!db.isValid())
	{
		c->error("No connection has been initialized!");
		return false;
	}
	db.commit();
	return true;
}
KVSO_CLASS_FUNCTION(sql, closeConnection)
{
	QString szConnectionName;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("connection_name", KVS_PT_STRING, KVS_PF_OPTIONAL, szConnectionName)
	KVSO_PARAMETERS_END(c)

	if(!szConnectionName.isEmpty())
	{
		QStringList connections = QSqlDatabase::connectionNames();
		if(!connections.contains(szConnectionName))
		{
			c->warning(__tr2qs_ctx("Connection %Q doesn't exist", "objects"), &szConnectionName);
			return true;
		}
		if(m_pCurrentSQlQuery)
		{
			delete m_pCurrentSQlQuery;
			m_pCurrentSQlQuery = nullptr;
		}
		QSqlDatabase::removeDatabase(szConnectionName);
		return true;
	}
	if(m_pCurrentSQlQuery)
	{
		delete m_pCurrentSQlQuery;
		m_pCurrentSQlQuery = nullptr;
	}
	QSqlDatabase::removeDatabase(mSzConnectionName);
	return true;
}
KVSO_CLASS_FUNCTION(sql, tablesList)
{
	QSqlDatabase db = QSqlDatabase::database(mSzConnectionName);
	if(!db.isValid())
	{
		c->error("No connection has been initialized!");
		return false;
	}
	QStringList tables = db.tables();
	KviKvsArray * pArray = new KviKvsArray();
	for(int i = 0; i < tables.count(); i++)
	{
		pArray->set(i, new KviKvsVariant(tables.at(i)));
	}
	c->returnValue()->setArray(pArray);
	return true;
}

KVSO_CLASS_FUNCTION(sql, queryFinish)
{
	CHECK_QUERY_IS_INIT
	m_pCurrentSQlQuery->finish();
	return true;
}

KVSO_CLASS_FUNCTION(sql, queryPrepare)
{
	CHECK_QUERY_IS_INIT
	QString szQuery;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("query", KVS_PT_STRING, 0, szQuery)
	KVSO_PARAMETERS_END(c)
	c->returnValue()->setBoolean(m_pCurrentSQlQuery->prepare(szQuery));
	return true;
}
KVSO_CLASS_FUNCTION(sql, queryBindValue)
{
	CHECK_QUERY_IS_INIT
	QString szFieldName;
	KviKvsVariant * v;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("bindName", KVS_PT_STRING, 0, szFieldName)
	KVSO_PARAMETER("value", KVS_PT_VARIANT, 0, v)
	KVSO_PARAMETERS_END(c)
	QString szType;
	v->getTypeName(szType);
	if(v->isString() || v->isNothing())
	{
		QString szText;
		v->asString(szText);
		m_pCurrentSQlQuery->bindValue(szFieldName, QVariant(szText));
	}
	else if(v->isReal())
	{
		kvs_real_t i;
		v->asReal(i);
		m_pCurrentSQlQuery->bindValue(szFieldName, QVariant((double)i));
	}
	else if(v->isInteger())
	{
		kvs_int_t i;
		v->asInteger(i);
		m_pCurrentSQlQuery->bindValue(szFieldName, QVariant((int)i));
	}
	else if(v->isBoolean())
	{
		bool b = v->asBoolean();
		m_pCurrentSQlQuery->bindValue(szFieldName, QVariant(b));
	}
	else if(v->isHObject())
	{
		kvs_hobject_t hOb;
		v->asHObject(hOb);
		KviKvsObject * pObject;
		pObject = KviKvsKernel::instance()->objectController()->lookupObject(hOb);
		if(pObject->inheritsClass("memorybuffer"))
			m_pCurrentSQlQuery->bindValue(szFieldName, QVariant(*((KvsObject_memoryBuffer *)pObject)->pBuffer()));
		else
			c->warning(__tr2qs_ctx("Only memorybuffer class object is supported", "objects"));
	}
	else
	{
		QString szTypeName;
		v->getTypeName(szTypeName);
		c->warning(__tr2qs_ctx("Type value %Q not supported", "objects"), &szTypeName);
	}
	return true;
}

KVSO_CLASS_FUNCTION(sql, queryExec)
{
	CHECK_QUERY_IS_INIT
	QString szQuery;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("query", KVS_PT_STRING, KVS_PF_OPTIONAL, szQuery)
	KVSO_PARAMETERS_END(c)
	bool bOk;
	if(szQuery.isEmpty())
		bOk = m_pCurrentSQlQuery->exec();
	else
		bOk = m_pCurrentSQlQuery->exec(szQuery.toLatin1());
	c->returnValue()->setBoolean(bOk);
	return true;
}
KVSO_CLASS_FUNCTION(sql, queryNext)
{
	CHECK_QUERY_IS_INIT
	if(m_pCurrentSQlQuery->isActive() && m_pCurrentSQlQuery->isSelect())
		c->returnValue()->setBoolean(m_pCurrentSQlQuery->next());
	else
		c->returnValue()->setNothing();
	return true;
}
KVSO_CLASS_FUNCTION(sql, queryPrevious)
{
	CHECK_QUERY_IS_INIT
	if(m_pCurrentSQlQuery->isActive() && m_pCurrentSQlQuery->isSelect())
		c->returnValue()->setBoolean(m_pCurrentSQlQuery->previous());
	else
		c->returnValue()->setNothing();
	return true;
}
KVSO_CLASS_FUNCTION(sql, queryResultsSize)
{
	CHECK_QUERY_IS_INIT
	c->returnValue()->setInteger(m_pCurrentSQlQuery->size());
	return true;
}
KVSO_CLASS_FUNCTION(sql, queryFirst)
{
	CHECK_QUERY_IS_INIT
	if(m_pCurrentSQlQuery->isActive() && m_pCurrentSQlQuery->isSelect())
		c->returnValue()->setBoolean(m_pCurrentSQlQuery->first());
	return true;
}
KVSO_CLASS_FUNCTION(sql, queryLast)
{
	CHECK_QUERY_IS_INIT
	if(m_pCurrentSQlQuery->isActive() && m_pCurrentSQlQuery->isSelect())
		c->returnValue()->setBoolean(m_pCurrentSQlQuery->last());
	return true;
}
KVSO_CLASS_FUNCTION(sql, queryRecord)
{
	CHECK_QUERY_IS_INIT
	KviKvsHash * pHash = new KviKvsHash();
	QSqlRecord record = m_pCurrentSQlQuery->record();
	for(int i = 0; i < record.count(); i++)
	{
		KviKvsVariant * pValue = nullptr;
		QVariant value = record.value(i);
		if(value.type() == QVariant::LongLong)
			pValue = new KviKvsVariant((kvs_int_t)value.toLongLong());
		else if(value.type() == QVariant::String)
			pValue = new KviKvsVariant(value.toString());
		else if(value.type() == QVariant::ByteArray)
		{
			KviKvsObjectClass * pClass = KviKvsKernel::instance()->objectController()->lookupClass("memoryBuffer");
			KviKvsVariantList params(new KviKvsVariant(QString()));
			KviKvsObject * pObject = pClass->allocateInstance(nullptr, "", c->context(), &params);
			*((KvsObject_memoryBuffer *)pObject)->pBuffer() = value.toByteArray();
			pValue = new KviKvsVariant(pObject->handle());
		}
		else
			pValue = new KviKvsVariant(QString());
		pHash->set(record.fieldName(i), pValue);
		(void)pHash->get(record.fieldName(i));
	}
	c->returnValue()->setHash(pHash);
	return true;
}

KVSO_CLASS_FUNCTION(sql, lastError)
{
	CHECK_QUERY_IS_INIT
	bool bMoreErrorDetails;
	KVSO_PARAMETERS_BEGIN(c)
	KVSO_PARAMETER("more", KVS_PT_BOOLEAN, KVS_PF_OPTIONAL, bMoreErrorDetails)
	KVSO_PARAMETERS_END(c)
	QString szError;
	QSqlError error = m_pCurrentSQlQuery->lastError();
	if(bMoreErrorDetails)
		szError = error.text();
	else
	{
		if(error.type() == QSqlError::StatementError)
			szError = "SyntaxError";
		else if(error.type() == QSqlError::ConnectionError)
			szError = "ConnectionError";
		else if(error.type() == QSqlError::TransactionError)
			szError = "TransactionError";
		else
			szError = "UnkonwnError";
	}
	c->returnValue()->setString(szError);
	return true;
}
