//=============================================================================
//
//   File : KviKvsScript.cpp
//   Creation date : Thu 25 Sep 2003 05.12 CEST by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_out.h"
#include "KviKvsScript.h"
#include "KviKvsParser.h"
#include "KviKvsReport.h"
#include "KviKvsRunTimeContext.h"
#include "KviKvsTreeNodeInstruction.h"
#include "KviKvsVariantList.h"
#include "KviKvsKernel.h"
#include "KviLocale.h"
#include "KviWindow.h"
#include "KviApplication.h"

//#warning "THERE IS SOME MESS WITH m_szBuffer and m_pBuffer : with some script copying we may get errors with negative char indexes!"

KviKvsScript::KviKvsScript(const QString & szName, const QString & szBuffer, ScriptType eType)
{
	m_pData = new KviKvsScriptData;
	m_pData->m_uRefs = 1;
	m_pData->m_szName = szName;
	m_pData->m_eType = eType;
	m_pData->m_szBuffer = szBuffer;
	if(m_pData->m_szBuffer.isNull())
		m_pData->m_szBuffer = "";

	m_pData->m_pBuffer = m_pData->m_szBuffer.constData(); // never 0
	m_pData->m_uLock = 0;
	m_pData->m_pTree = nullptr;
}

KviKvsScript::KviKvsScript(const QString & szName, const QString & szBuffer, KviKvsTreeNodeInstruction * pPreparsedTree, ScriptType eType)
{
	m_pData = new KviKvsScriptData;
	m_pData->m_uRefs = 1;
	m_pData->m_szName = szName;
	m_pData->m_szBuffer = szBuffer;
	m_pData->m_eType = eType;
	if(m_pData->m_szBuffer.isNull())
		m_pData->m_szBuffer = "";

	m_pData->m_pBuffer = m_pData->m_szBuffer.constData(); // never 0
	m_pData->m_uLock = 0;
	m_pData->m_pTree = pPreparsedTree;
}

KviKvsScript::KviKvsScript(const KviKvsScript & src)
{
	m_pData = src.m_pData;
	m_pData->m_uRefs++;
}

KviKvsScript::~KviKvsScript()
{
	if(m_pData->m_uRefs < 2)
	{
		if(m_pData->m_uLock)
			qDebug("WARNING: destroying a locked KviKvsScript");
		if(m_pData->m_pTree)
			delete m_pData->m_pTree;
		delete m_pData;
	}
	else
	{
		m_pData->m_uRefs--;
	}
}

void KviKvsScript::setName(const QString & szName)
{
	if(m_pData->m_uRefs > 1)
		detach();
	m_pData->m_szName = szName;
}

const QString & KviKvsScript::name() const
{
	return m_pData->m_szName;
}

const QString & KviKvsScript::code() const
{
	return m_pData->m_szBuffer;
}

bool KviKvsScript::locked() const
{
	return m_pData->m_uLock > 0;
}

void KviKvsScript::dump(const char * prefix)
{
	if(m_pData->m_pTree)
		m_pData->m_pTree->dump(prefix);
	else
		qDebug("%s KviKvsScript : no tree to dump", prefix);
}

void KviKvsScript::detach()
{
	if(m_pData->m_uRefs <= 1)
		return;

	m_pData->m_uRefs--;
	KviKvsScriptData * d = new KviKvsScriptData;
	d->m_uRefs = 1;
	d->m_eType = m_pData->m_eType;
	d->m_szBuffer = m_pData->m_szBuffer;
	if(d->m_szBuffer.isNull())
		d->m_szBuffer = "";

	d->m_szBuffer.resize(d->m_szBuffer.length());
	d->m_pBuffer = d->m_szBuffer.constData(); // never 0
	d->m_uLock = 0;
	d->m_pTree = nullptr;
	m_pData = d;
}

const QChar * KviKvsScript::buffer() const
{
	return m_pData->m_pBuffer;
}

int KviKvsScript::run(const QString & szCode, KviWindow * pWindow, KviKvsVariantList * pParams, KviKvsVariant * pRetVal)
{
	// static helper
	KviKvsScript s("kvirc::corecall(run)", szCode);
	return s.run(pWindow, pParams, pRetVal, PreserveParams);
}

int KviKvsScript::evaluate(const QString & szCode, KviWindow * pWindow, KviKvsVariantList * pParams, KviKvsVariant * pRetVal)
{
	// static helper
	KviKvsScript s("kvirc::corecall(evaluate)", szCode, Parameter);
	return s.run(pWindow, pParams, pRetVal, PreserveParams);
}

int KviKvsScript::evaluateAsString(const QString & szCode, KviWindow * pWindow, KviKvsVariantList * pParams, QString & szRetVal)
{
	// static helper
	KviKvsVariant ret;
	KviKvsScript s("kvirc::corecall(evaluate)", szCode, Parameter);
	int iRet = s.run(pWindow, pParams, &ret, PreserveParams);
	ret.asString(szRetVal);
	return iRet;
}

int KviKvsScript::run(KviWindow * pWnd, KviKvsVariantList * pParams, QString & szRetVal, int iRunFlags, KviKvsExtendedRunTimeData * pExtData)
{
	KviKvsVariant retVal;
	int iRet = run(pWnd, pParams, &retVal, iRunFlags, pExtData);
	retVal.asString(szRetVal);
	return iRet;
}

//static long int g_iTreeCacheHits = 0;
//static long int g_iTreeCacheMisses = 0;

int KviKvsScript::run(KviWindow * pWnd, KviKvsVariantList * pParams, KviKvsVariant * pRetVal, int iRunFlags, KviKvsExtendedRunTimeData * pExtData)
{
	if(!m_pData->m_pTree)
	{
		//g_iTreeCacheMisses++;
		//qDebug("CREATING TREE FOR SCRIPT %s",name().latin1());
		//qDebug("TREE CACHE STATS: HITS=%d, MISSES=%d",g_iTreeCacheHits,g_iTreeCacheMisses);
		if(!parse(pWnd, iRunFlags))
		{
			if(pParams && !(iRunFlags & PreserveParams))
				delete pParams;
			return Error;
		}
	}
	else
	{
		//g_iTreeCacheHits++;
		//qDebug("USING A CACHED TREE FOR SCRIPT %s",name().latin1());
		//qDebug("TREE CACHE STATS: HITS=%d, MISSES=%d",g_iTreeCacheHits,g_iTreeCacheMisses);
	}

	return execute(pWnd, pParams, pRetVal, iRunFlags, pExtData);
}

int KviKvsScript::run(KviKvsRunTimeContext * pContext, int iRunFlags)
{
	if(!m_pData->m_pTree)
	{
		//g_iTreeCacheMisses++;
		//qDebug("CREATING TREE FOR SCRIPT %s",name().latin1());
		//qDebug("TREE CACHE STATS: HITS=%d, MISSES=%d",g_iTreeCacheHits,g_iTreeCacheMisses);
		if(!parse(pContext->window(), iRunFlags))
			return Error;
	}
	else
	{
		//g_iTreeCacheHits++;
		//qDebug("USING A CACHED TREE FOR SCRIPT %s",name().latin1());
		//qDebug("TREE CACHE STATS: HITS=%d, MISSES=%d",g_iTreeCacheHits,g_iTreeCacheMisses);
	}

	int iRet;

	if(iRunFlags & Quiet)
	{
		bool bMustReEnable = !(pContext->reportingDisabled());
		pContext->disableReporting();
		iRet = executeInternal(pContext);
		if(bMustReEnable)
			pContext->enableReporting();
	}
	else
	{
		iRet = executeInternal(pContext);
	}

	return iRet;
}

bool KviKvsScript::parse(KviWindow * pOutput, int iRunFlags)
{
	if(m_pData->m_pTree)
	{
		// there is already a tree
		// if we have more than one ref, detach!
		if(m_pData->m_uRefs > 1)
		{
			// mmmh.. more than one ref! .. detach
			detach();
		}
		else
		{
			// only a single ref: we're the owner of the tree
			if(m_pData->m_uLock)
			{
				// oops... someone is locked in THIS script object
				qDebug("WARNING: trying to reparse a locked KviKvsScript!");
				return false;
			}
			if(m_pData->m_pTree)
				delete m_pData->m_pTree;

			m_pData->m_pTree = nullptr;
		}
	} // else there is no tree at all, nobody can be locked inside

	KviKvsParser p(this, (iRunFlags & Quiet) ? nullptr : pOutput);
	// parse never blocks

	int iFlags = iRunFlags & AssumeLocals ? KviKvsParser::AssumeLocals : 0;
	if(iRunFlags & Pedantic)
		iFlags |= KviKvsParser::Pedantic;

	switch(m_pData->m_eType)
	{
		case Expression:
			m_pData->m_pTree = p.parseAsExpression(m_pData->m_pBuffer, iFlags);
			break;
		case Parameter:
			m_pData->m_pTree = p.parseAsParameter(m_pData->m_pBuffer, iFlags);
			break;
		case InstructionList:
		default:
			m_pData->m_pTree = p.parse(m_pData->m_pBuffer, iFlags);
			break;
	}

	//qDebug("\n\nDUMPING SCRIPT");
	//dump("");
	//qDebug("END OF SCRIPT DUMP\n\n");

	return !p.error();
}

int KviKvsScript::executeInternal(KviKvsRunTimeContext * pContext)
{
	// lock this script
	m_pData->m_uLock++;

	int iRunStatus = Success;

	if(!m_pData->m_pTree->execute(pContext))
	{
		if(pContext->error())
			iRunStatus = Error;
		else
		{
			// else just a halt, return or sth like that
			if(pContext->haltCalled())
				iRunStatus |= HaltEncountered;
		}
	}

	// we can't block any longer: unlock
	m_pData->m_uLock--;

	return iRunStatus;
}

int KviKvsScript::execute(KviWindow * pWnd, KviKvsVariantList * pParams, KviKvsVariant * pRetVal, int iRunFlags, KviKvsExtendedRunTimeData * pExtData)
{
	bool bDeleteParams = !(iRunFlags & PreserveParams);

	// do we have a parsed tree ?
	if(!m_pData->m_pTree)
	{
		if(pParams && bDeleteParams)
			delete pParams;

		// this is intended for developers only
		pWnd->outputNoFmt(KVI_OUT_PARSERERROR, "[developer error]: you must successfully call KviKvsScript::parse() before KviKvsScript::execute()");
		return Error;
	}

	// do we need to pass dummy params ?
	if(!pParams)
	{
		pParams = KviKvsKernel::instance()->emptyParameterList();
		bDeleteParams = false;
	}

	bool bDeleteRetVal = false;

	if(!pRetVal)
	{
		pRetVal = new KviKvsVariant();
		bDeleteRetVal = true;
	}

	KviKvsRunTimeContext ctx(this, pWnd, pParams, pRetVal, pExtData);

	if(iRunFlags & Quiet)
		ctx.disableReporting();

	int iRunStatus = executeInternal(&ctx);

	// don't forget to delete the params
	if(bDeleteParams)
		delete pParams;
	if(bDeleteRetVal)
		delete pRetVal;
	return iRunStatus;
}
