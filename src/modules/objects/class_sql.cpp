//=============================================================================
//
//   File : class_sql.cpp
//   Creation date : Wed Gen 28 2009 21:07:55 by Alessandro Carbone
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
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
//============================================================================

#include "kvi_debug.h"
#include "kvi_malloc.h"
#include "kvi_locale.h"
#include "class_sql.h"
#include <QtSql>
#include "class_memorybuffer.h"
#include <stdlib.h>
#include <QHash>

static KviPointerList <KviKvsObject_sql> sql_instances;

static bool checkDuplicatedConnection(QString &szConnectionName)
{
    if(!sql_instances.count()) return false;
    bool bFound=false;
    for(KviKvsObject_sql *i = sql_instances.first(); i; i = sql_instances.next())
    {
        QSqlQuery *q=i->getQueryConnectionsDict().value(szConnectionName);
        if (q)
        {
            i->closeQueryConnection(q);
            bFound=true;
            break;
        }
    }
    return bFound;
}
static void KviKvsSqlInstanceRegister(KviKvsObject_sql *instance)
{
    sql_instances.setAutoDelete(false);
    sql_instances.append(instance);
}
static void KviKvsSqlInstanceUnregister(KviKvsObject_sql *instance)
{
    int idx=sql_instances.findRef(instance);
    sql_instances.remove(idx);
}

#define CHECK_QUERY_IS_INIT if (!m_pCurrentSQlQuery)\
        {\
            c->error("No query has been initialized!");\
            return false;}


/*
        @doc:           sql
        @keyterms:
                        Sql database.
            @title:
                        sql class
            @type:
                        class
            @short:
                        A sql database interface.
            @inherits:
                        [class]object[/class]
            @description:
                      This class permits KVIrc to have an interface with a SQL database supported by Qt library drivers.
            @functions:
                      !fn: <boolean> $setConnection(<connection_name:string>,<database_name:string>,[<user:string>,<host_name_string>,<password:string>,<database_driver:string>])
                      Connects to the DBMS using the connection <connection_name> and selecting the database <database_name>.[br]
                      If the optional parameter <database_driver> is passed, it will be used the corresponding driver (if present), otherwise Sqlite will be used.
                      Returns true if the operation is successful, false otherwise.
                      !fn: <array> $tablesList(<connection_name:string>)
                      Returns as array the database tables list.
                      !fn: $queryInit(<connection_name:string>)
                      Initialize the query for the database <connection_name> which has to be already connected.
                      !fn: $transaction()
                      Begin a transaction.
                      !fn: $commit()
                      Commit the transaction.
                      !fn: $setCurrentQuery(<connection_name:string>)
                      Sets the query for the database connection <connection_name> , which has to be already connected, as current query.
                      !fn: <connection_name:string> $currentQuery()
                      Returns the name of the database connection for the current query, or an empty string if there aren't any initialized queries.
                      !fn: $closeConnection(<connection_name:string>)
                      Closes the connection <connection_name>.
                      !fn: <size:integer> $queryResultsSize()
                      Returns the query size in rows or -1 if the query is empty or the database driver doesn' support the function.
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
                      It's possible to use the placeholders. It's supported either the identifier ':' and '?' but it's not possible to use them together.
                      Returns true if the operation is successful, false otherwise.
                      See also [classfnc]$queryExec[/classfnc]and[classfnc]$queryBindValue[/classfnc.
                      !fn: $queryBindValue()
                      Sets the placeholder <placeholder> to be bound to the value <val> in the prepared statement.
                      Note that the placeholder mark (e.g :) must be included when specifing the placeholder name.
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



KVSO_BEGIN_REGISTERCLASS(KviKvsObject_sql,"sql","object")
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_sql,queryLastInsertId)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_sql,commit)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_sql,beginTransaction)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_sql,setConnection)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_sql,setCurrentQuery)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_sql,tablesList)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_sql,currentQuery)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_sql,closeConnection)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_sql,queryInit)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_sql,queryFinish)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_sql,queryResultsSize)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_sql,queryExec)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_sql,queryPrepare)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_sql,queryBindValue)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_sql,queryPrevious)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_sql,queryNext)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_sql,queryLast)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_sql,queryFirst)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_sql,queryRecord)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_sql,lastError)
        KVSO_REGISTER_HANDLER_BY_NAME(KviKvsObject_sql,features)
KVSO_END_REGISTERCLASS(KviKvsObject_sql)


KVSO_BEGIN_CONSTRUCTOR(KviKvsObject_sql,KviKvsObject)
m_pCurrentSQlQuery=0;
KviKvsSqlInstanceRegister(this);
KVSO_END_CONSTRUCTOR(KviKvsObject_sql)


KVSO_BEGIN_DESTRUCTOR(KviKvsObject_sql)
QHashIterator<QString,QSqlQuery *> t(queryConnectionsDict);
while (t.hasNext())
{
    t.next();
    QString szConnectionName=t.key();
    queryConnectionsDict.value(szConnectionName)->finish();
    delete queryConnectionsDict.value(szConnectionName);
    QSqlDatabase::removeDatabase(szConnectionName);
}
KviKvsSqlInstanceUnregister(this);
queryConnectionsDict.clear();

KVSO_END_DESTRUCTOR(KviKvsObject_sql)

KVSO_CLASS_FUNCTION(sql,setConnection)
{
        QString szConnectionName,szDbName,szDbDriver,szUserName,szHostName,szPassword;
        KVSO_PARAMETERS_BEGIN(c)
                KVSO_PARAMETER("connection_name",KVS_PT_STRING,0,szConnectionName)
                KVSO_PARAMETER("database_name",KVS_PT_STRING,0,szDbName)
                KVSO_PARAMETER("user_name",KVS_PT_STRING,KVS_PF_OPTIONAL,szUserName)
                KVSO_PARAMETER("host_name",KVS_PT_STRING,KVS_PF_OPTIONAL,szHostName)
                KVSO_PARAMETER("password",KVS_PT_STRING,KVS_PF_OPTIONAL,szPassword)
                KVSO_PARAMETER("database_type",KVS_PT_STRING,KVS_PF_OPTIONAL,szDbDriver)
        KVSO_PARAMETERS_END(c)
        if(!szDbDriver.isEmpty())
        {
            QStringList drivers = QSqlDatabase::drivers();
            if (!drivers.contains(szDbDriver))
            {
                c->error(__tr2qs_ctx("Missing Qt plugin for database %Q","objects"),&szDbDriver);
                return false;
            }
        }
        else szDbDriver="QSQLITE";
        if (checkDuplicatedConnection(szConnectionName))
        {
            c->warning(__tr2qs_ctx("Duplicate connection name '%Q', old query connection removed","objects"),&szConnectionName);
        }
        QSqlDatabase db=QSqlDatabase::addDatabase(szDbDriver,szConnectionName);
        db.setDatabaseName(szDbName);
        db.setHostName(szHostName);
        db.setUserName(szUserName);
        db.setPassword(szPassword);

        c->returnValue()->setBoolean(db.open());
        return true;
}
KVSO_CLASS_FUNCTION(sql,queryLastInsertId)
{
        CHECK_QUERY_IS_INIT
        QVariant value=m_pCurrentSQlQuery->lastInsertId();
        if (value.type()==QVariant::LongLong){
            c->returnValue()->setInteger((kvs_int_t) value.toLongLong());
        }
        return true;
    }
KVSO_CLASS_FUNCTION(sql,features)
{
        QString szConnectionName;
        KVSO_PARAMETERS_BEGIN(c)
                KVSO_PARAMETER("connectionName",KVS_PT_STRING,0,szConnectionName)
        KVSO_PARAMETERS_END(c)

        QStringList connections = QSqlDatabase::connectionNames();
        if (!connections.contains(szConnectionName))
        {
             c->warning(__tr2qs_ctx("Connection %Q does not exists","objects"),&szConnectionName);
             return true;
        }
        QSqlDatabase db=QSqlDatabase::database(szConnectionName);
        QSqlDriver *sqlDriver=db.driver();
        QStringList features;
        if (sqlDriver->hasFeature(QSqlDriver::Transactions)) features.append("transactions");
        if (sqlDriver->hasFeature(QSqlDriver::QuerySize)) features.append("querysize");
        if (sqlDriver->hasFeature(QSqlDriver::BLOB)) features.append("blob");
        if (sqlDriver->hasFeature(QSqlDriver::PreparedQueries)) features.append("preparedqueries");
        if (sqlDriver->hasFeature(QSqlDriver::NamedPlaceholders)) features.append("namedplaceholders");
        if (sqlDriver->hasFeature(QSqlDriver::PositionalPlaceholders)) features.append("positionaplaceholders");
        if (sqlDriver->hasFeature(QSqlDriver::LastInsertId)) features.append("lastinsertid");
        if (sqlDriver->hasFeature(QSqlDriver::BatchOperations)) features.append("batchoperations");
        if (sqlDriver->hasFeature(QSqlDriver::SimpleLocking)) features.append("simplelocking");
        if (sqlDriver->hasFeature(QSqlDriver::LowPrecisionNumbers)) features.append("lowprecisionnumbers");
        if (sqlDriver->hasFeature(QSqlDriver::EventNotifications)) features.append("eventnotifications");
        if (sqlDriver->hasFeature(QSqlDriver::FinishQuery)) features.append("finishquery");
        if (sqlDriver->hasFeature(QSqlDriver::MultipleResultSets)) features.append("multipleresults");
        c->returnValue()->setString(features.join(","));
        return true;
}

KVSO_CLASS_FUNCTION(sql,beginTransaction)
{
        QString szConnectionName;
        KVSO_PARAMETERS_BEGIN(c)
                KVSO_PARAMETER("connectionName",KVS_PT_STRING,0,szConnectionName)
        KVSO_PARAMETERS_END(c)

        QStringList connections = QSqlDatabase::connectionNames();
        if (!connections.contains(szConnectionName))
        {
             c->warning(__tr2qs_ctx("Connection %Q does not exists","objects"),&szConnectionName);
             return true;
        }
        QSqlDatabase db=QSqlDatabase::database(szConnectionName);
        db.transaction();
        return true;
}
KVSO_CLASS_FUNCTION(sql,commit)
{
        QString szConnectionName;
        KVSO_PARAMETERS_BEGIN(c)
                KVSO_PARAMETER("connectionName",KVS_PT_STRING,0,szConnectionName)
        KVSO_PARAMETERS_END(c)

        QStringList connections = QSqlDatabase::connectionNames();
        if (!connections.contains(szConnectionName))
        {
             c->warning(__tr2qs_ctx("Connection %Q does not exists","objects"),&szConnectionName);
             return true;
        }
        QSqlDatabase db=QSqlDatabase::database(szConnectionName);
        db.commit();
        return true;
}
KVSO_CLASS_FUNCTION(sql,closeConnection)
{
        QString szConnectionName;
        KVSO_PARAMETERS_BEGIN(c)
                KVSO_PARAMETER("connectionName",KVS_PT_STRING,0,szConnectionName)
        KVSO_PARAMETERS_END(c)

        QStringList connections = QSqlDatabase::connectionNames();
        if (!connections.contains(szConnectionName))
        {
             c->warning(__tr2qs_ctx("Connection %Q does not exists","objects"),&szConnectionName);
             return true;
        }
        QSqlQuery *q=queryConnectionsDict.value(szConnectionName);
        if (q) closeQueryConnection(q);
        QSqlDatabase::removeDatabase(szConnectionName);
        return true;
}
KVSO_CLASS_FUNCTION(sql,tablesList)
{
        QString szConnectionName;
        KVSO_PARAMETERS_BEGIN(c)
                KVSO_PARAMETER("connectionName",KVS_PT_STRING,0,szConnectionName)
        KVSO_PARAMETERS_END(c)
        QStringList connections = QSqlDatabase::connectionNames();
        if (!connections.contains(szConnectionName))
        {
             c->warning(__tr2qs_ctx("Connection %Q does not exists","objects"),&szConnectionName);
             return true;
        }
        QSqlDatabase db=QSqlDatabase::database(szConnectionName);
        QStringList tables=db.tables();
        KviKvsArray *pArray=new KviKvsArray();
        for(int i=0;i<tables.count();i++)
        {
            pArray->set(i,new KviKvsVariant(tables.at(i)));
        }
        c->returnValue()->setArray(pArray);
        return true;
}
KVSO_CLASS_FUNCTION(sql,setCurrentQuery)
{
      QString szConnectionName;
      KVSO_PARAMETERS_BEGIN(c)
              KVSO_PARAMETER("connection_name",KVS_PT_STRING,0,szConnectionName)
      KVSO_PARAMETERS_END(c)
      QSqlQuery *q=queryConnectionsDict.value(szConnectionName);
      if(!q)
      {
          c->warning(__tr2qs_ctx("Connection query %Q does not exists","objects"),&szConnectionName);
          return true;
      }
      m_pCurrentSQlQuery=q;
      return true;
}
KVSO_CLASS_FUNCTION(sql,queryFinish)
{
      QString szConnectionName;
      KVSO_PARAMETERS_BEGIN(c)
              KVSO_PARAMETER("connection_name",KVS_PT_STRING,KVS_PF_OPTIONAL,szConnectionName)
      KVSO_PARAMETERS_END(c)
      QSqlQuery *q;
      if(szConnectionName.isEmpty())
      {
          q=m_pCurrentSQlQuery;
          if (!q)
          {
              c->warning(__tr2qs_ctx("No query connection is open","objects"));
                return true;
          }
      }
      else
      {
          q=queryConnectionsDict.value(szConnectionName);
          if(!q)
          {
              c->warning(__tr2qs_ctx("Query for connection %Q does not exists","objects"),&szConnectionName);
              return true;
          }
      }
      q->finish();
      return true;
}
KVSO_CLASS_FUNCTION(sql,currentQuery)
{
    if (!m_pCurrentSQlQuery) c->returnValue()->setString(QString());
    else
    {
        QString szKey=queryConnectionsDict.key(m_pCurrentSQlQuery);
        c->returnValue()->setString(szKey);
    }
    return true;
}
KVSO_CLASS_FUNCTION(sql,queryInit)
{
        QString szConnectionName;
        KVSO_PARAMETERS_BEGIN(c)
                KVSO_PARAMETER("connection_name",KVS_PT_STRING,0,szConnectionName)
        KVSO_PARAMETERS_END(c)
        if (queryConnectionsDict.value(szConnectionName))
        {
           c->warning(__tr2qs_ctx("Query %Q already initialized","objects"),&szConnectionName);
           return true;
        }
        if(!QSqlDatabase::connectionNames().contains(szConnectionName))
        {
            c->error(__tr2qs_ctx("Connection %Q is not open!","objects"),&szConnectionName);
            return false;
        }
        m_pCurrentSQlQuery=new QSqlQuery(QSqlDatabase::database(szConnectionName));
        queryConnectionsDict[szConnectionName]=m_pCurrentSQlQuery;
        return true;
}


KVSO_CLASS_FUNCTION(sql,queryPrepare)
{
        CHECK_QUERY_IS_INIT
        QString szQuery;
        KVSO_PARAMETERS_BEGIN(c)
                KVSO_PARAMETER("query",KVS_PT_STRING,0,szQuery)
        KVSO_PARAMETERS_END(c)
        c->returnValue()->setBoolean(m_pCurrentSQlQuery->prepare(szQuery));
        return true;
}
KVSO_CLASS_FUNCTION(sql,queryBindValue)
{
        CHECK_QUERY_IS_INIT
        QString szFieldName;
        KviKvsVariant *v;
        KVSO_PARAMETERS_BEGIN(c)
                KVSO_PARAMETER("bindName",KVS_PT_STRING,0,szFieldName)
                KVSO_PARAMETER("value",KVS_PT_VARIANT,0,v)
        KVSO_PARAMETERS_END(c)
        QString szType;
        v->getTypeName(szType);
        if (v->isString()|| v->isNothing())
        {
            QString szText;
            v->asString(szText);
            m_pCurrentSQlQuery->bindValue(szFieldName,QVariant(szText));
        }
        else if (v->isReal())
        {
            kvs_real_t i;
            v->asReal(i);
            m_pCurrentSQlQuery->bindValue(szFieldName,QVariant((double)i));
        }
        else if (v->isInteger())
        {
            kvs_int_t i;
            v->asInteger(i);
            m_pCurrentSQlQuery->bindValue(szFieldName,QVariant((int)i));
        }
        else if (v->isBoolean())
        {
            bool b=v->asBoolean();
            m_pCurrentSQlQuery->bindValue(szFieldName,QVariant(b));
        }
        else if (v->isHObject())
        {
            kvs_hobject_t hOb;
            v->asHObject(hOb);
            KviKvsObject *pObject;
            pObject=KviKvsKernel::instance()->objectController()->lookupObject(hOb);
            if (pObject->inheritsClass("memorybuffer"))
                m_pCurrentSQlQuery->bindValue(szFieldName,QVariant(*((KviKvsObject_memorybuffer *)pObject)->pBuffer()));
            else c->warning(__tr2qs_ctx("Only memorybuffer class object is supported","objects"));
        }
        else
        {
            QString szTypeName;
            v->getTypeName(szTypeName);
            c->warning(__tr2qs_ctx("Type value %Q not supported","objects"),&szTypeName);
        }
        return true;
}

KVSO_CLASS_FUNCTION(sql,queryExec)
{
        CHECK_QUERY_IS_INIT
        QString szQuery;
        KVSO_PARAMETERS_BEGIN(c)
                KVSO_PARAMETER("query",KVS_PT_STRING,KVS_PF_OPTIONAL,szQuery)
        KVSO_PARAMETERS_END(c)
        bool bOk;
        if (szQuery.isEmpty()) bOk=m_pCurrentSQlQuery->exec();
        else bOk=m_pCurrentSQlQuery->exec(szQuery.toLatin1());
        c->returnValue()->setBoolean(bOk);
        return true;
}
KVSO_CLASS_FUNCTION(sql,queryNext)
{
        CHECK_QUERY_IS_INIT
        if (m_pCurrentSQlQuery->isActive() && m_pCurrentSQlQuery->isSelect()) c->returnValue()->setBoolean(m_pCurrentSQlQuery->next());
        else c->returnValue()->setNothing();
        return true;
}
KVSO_CLASS_FUNCTION(sql,queryPrevious)
{
        CHECK_QUERY_IS_INIT
        if (m_pCurrentSQlQuery->isActive() && m_pCurrentSQlQuery->isSelect()) c->returnValue()->setBoolean(m_pCurrentSQlQuery->previous());
        else c->returnValue()->setNothing();
        return true;
}
KVSO_CLASS_FUNCTION(sql,queryResultsSize)
{
        CHECK_QUERY_IS_INIT
        c->returnValue()->setInteger(m_pCurrentSQlQuery->size());
        return true;
}
KVSO_CLASS_FUNCTION(sql,queryFirst)
{
        CHECK_QUERY_IS_INIT
        if (m_pCurrentSQlQuery->isActive() && m_pCurrentSQlQuery->isSelect()) c->returnValue()->setBoolean(m_pCurrentSQlQuery->first());
        return true;
}
KVSO_CLASS_FUNCTION(sql,queryLast)
{
        CHECK_QUERY_IS_INIT
        if (m_pCurrentSQlQuery->isActive() && m_pCurrentSQlQuery->isSelect()) c->returnValue()->setBoolean(m_pCurrentSQlQuery->last());
        return true;
}
KVSO_CLASS_FUNCTION(sql,queryRecord)
{
        CHECK_QUERY_IS_INIT
        KviKvsHash *pHash=new KviKvsHash();
        QSqlRecord record=m_pCurrentSQlQuery->record();
        for(int i=0;i<record.count();i++)
        {
            KviKvsVariant *pValue=0;
            QVariant value=record.value(i);
            if (value.type()==QVariant::LongLong) pValue=new KviKvsVariant((kvs_int_t) value.toLongLong());
            else if (value.type()==QVariant::String) pValue=new KviKvsVariant(value.toString());
            else if (value.type()==QVariant::ByteArray)
            {
               KviKvsObjectClass * pClass = KviKvsKernel::instance()->objectController()->lookupClass("memoryBuffer");
               KviKvsVariantList params(new KviKvsVariant(QString()));
               KviKvsObject * pObject = pClass->allocateInstance(0,"",c->context(),&params);
               *((KviKvsObject_memorybuffer *)pObject)->pBuffer()=value.toByteArray();
               pValue=new KviKvsVariant(pObject->handle());
            }
            else pValue=new KviKvsVariant(QString());
            pHash->set(record.fieldName(i),pValue);
            KviKvsVariant *value2=pHash->get(record.fieldName(i));
            value2->type();
        }
        c->returnValue()->setHash(pHash);
        return true;
}

KVSO_CLASS_FUNCTION(sql,lastError)
{
        CHECK_QUERY_IS_INIT
	bool bMoreErrorDetails;
	KVSO_PARAMETERS_BEGIN(c)
		KVSO_PARAMETER("more",KVS_PT_BOOLEAN,KVS_PF_OPTIONAL,bMoreErrorDetails)
	KVSO_PARAMETERS_END(c)
	QString szError;
        QSqlError error=m_pCurrentSQlQuery->lastError();
	if (bMoreErrorDetails) szError=error.text();
	else
	{
		if (error.type()==QSqlError::StatementError) szError="SyntaxError";
		else if (error.type()==QSqlError::ConnectionError) szError="ConnectionError";
		else if (error.type()==QSqlError::TransactionError) szError="TransactionError";
		else szError="UnkonwnError";
	}
        c->returnValue()->setString(szError);
        return true;
}
void KviKvsObject_sql::closeQueryConnection(QSqlQuery * query)
{
        if (query==m_pCurrentSQlQuery) m_pCurrentSQlQuery=0;
        query->finish();
        queryConnectionsDict.remove(queryConnectionsDict.key(query));
        delete query;
 }

