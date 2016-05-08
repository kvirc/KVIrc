//=============================================================================
//
//   File : KviKvsRunTimeContext.cpp
//   Creation date : Tue 07 Oct 2003 01:49:40 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003-2010 Szymon Stefanek <pragma at kvirc dot net>
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

#include "KviKvsRunTimeContext.h"
#include "KviKvsScript.h"
#include "KviKvsKernel.h"
#include "KviKvsReport.h"
#include "KviConsoleWindow.h"
#include "KviKvsTreeNodeBase.h"
#include "KviLocale.h"
#include "KviApplication.h"
#include "KviKvsObject.h"

KviKvsExtendedRunTimeData::~KviKvsExtendedRunTimeData()
{
	if(m_bAutoDelete)
	{
		if(m_pExtendedScopeVariables)
			delete m_pExtendedScopeVariables;
		if(m_pAliasSwitchList)
			delete m_pAliasSwitchList;
		if(m_pThisObject)
			delete m_pThisObject;
		if(m_pScriptFilePath)
			delete m_pScriptFilePath;
		// don't delete m_pPopupId;
	}
}

void KviKvsExtendedRunTimeData::setPopupId(QString * pPopupId)
{
	m_pPopupId = pPopupId;
};

KviKvsRunTimeContext::KviKvsRunTimeContext(KviKvsScript * pScript, KviWindow * pWnd, KviKvsVariantList * pParams, KviKvsVariant * pRetVal, KviKvsExtendedRunTimeData * pExtData)
{
	m_bError = false;
	m_pScript = pScript;
	m_pParameterList = pParams;
	m_pWindow = pWnd;
	m_pLocalVariables = new KviKvsHash();
	m_pReturnValue = pRetVal;
	m_uRunTimeFlags = 0;
	m_pExtendedData = pExtData;
	m_pDefaultReportLocation = nullptr;
}

KviKvsRunTimeContext::~KviKvsRunTimeContext()
{
	delete m_pLocalVariables;
}

KviKvsHash * KviKvsRunTimeContext::globalVariables()
{
	return KviKvsKernel::instance()->globalVariables();
}

void KviKvsRunTimeContext::enterBlockingSection()
{
	// actually a NO-OP
}

bool KviKvsRunTimeContext::leaveBlockingSection()
{
	if(g_pApp->kviClosingDown())
		return false; // application quitting
	if(!g_pApp->windowExists(m_pWindow))
		return false; // window lost
	return true;
}

KviKvsVariant * KviKvsRunTimeContext::swapReturnValuePointer(KviKvsVariant * pNewPointer)
{
	KviKvsVariant * pAux = m_pReturnValue;
	m_pReturnValue = pNewPointer;
	return pAux;
}

void KviKvsRunTimeContext::report(bool bError, KviKvsTreeNode * pNode, const QString & szMsgFmt, kvi_va_list va)
{
	QString szMsg;
	KviQString::vsprintf(szMsg, szMsgFmt, va);

	KviPointerList<QString> * pCodeListing = nullptr;
	KviPointerList<QString> * pCallStack = nullptr;
	QString szLocation;

	if(pNode)
	{
		if(pNode->location() && m_pScript)
		{
			pCodeListing = new KviPointerList<QString>;
			pCodeListing->setAutoDelete(true);

			int iLine, iCol;

			KviKvsReport::findLineColAndListing(m_pScript->buffer(), pNode->location(), iLine, iCol, pCodeListing);

			szLocation = QString(__tr2qs_ctx("line %1, near character %2", "kvs")).arg(iLine).arg(iCol);
		}

		// create the call stack
		int iFrame = 0;

		pCallStack = new KviPointerList<QString>;
		pCallStack->setAutoDelete(true);

		while(pNode && (iFrame < 12))
		{
			QString * pString = new QString();
			QString szTmp;
			pNode->contextDescription(szTmp);
			*pString = QString("#%1 %2").arg(iFrame).arg(szTmp);
			if(pNode->location())
			{
				int iLine, iCol;
				KviKvsReport::findLineAndCol(m_pScript->buffer(), pNode->location(), iLine, iCol);
				QString tmpi = QString(" [line %1, near character %2]").arg(iLine).arg(iCol);
				*pString += tmpi;
			}
			pCallStack->append(pString);
			iFrame++;
			pNode = pNode->parent();
		}
		if(pNode)
			pCallStack->append(new QString("#12 ..."));
	}

	QString szContext = m_pScript ? m_pScript->name() : "kvirc core code";
	KviKvsReport rep(bError ? KviKvsReport::RunTimeError : KviKvsReport::RunTimeWarning, szContext, szMsg, szLocation, m_pWindow);
	if(pCodeListing)
		rep.setCodeListing(pCodeListing);
	if(pCallStack)
		rep.setCallStack(pCallStack);

	KviKvsReport::report(&rep, m_pWindow);
}

void KviKvsRunTimeContext::error(KviKvsTreeNode * pNode, QString szMsgFmt, ...)
{
	m_bError = true;

	kvi_va_list va;
	kvi_va_start(va, szMsgFmt);
	report(true, pNode, szMsgFmt, va);
	kvi_va_end(va);
}

void KviKvsRunTimeContext::warning(KviKvsTreeNode * pNode, QString szMsgFmt, ...)
{
	kvi_va_list va;
	kvi_va_start(va, szMsgFmt);
	report(false, pNode, szMsgFmt, va);
	kvi_va_end(va);
}

void KviKvsRunTimeContext::error(QString szMsgFmt, ...)
{
	m_bError = true;

	kvi_va_list va;
	kvi_va_start(va, szMsgFmt);
	report(true, m_pDefaultReportLocation, szMsgFmt, va);
	kvi_va_end(va);
}

void KviKvsRunTimeContext::warning(QString szMsgFmt, ...)
{
	kvi_va_list va;
	kvi_va_start(va, szMsgFmt);
	report(false, m_pDefaultReportLocation, szMsgFmt, va);
	kvi_va_end(va);
}

bool KviKvsRunTimeContext::errorNoIrcContext()
{
	error(m_pDefaultReportLocation, __tr2qs_ctx("This command can be used only in windows bound to an IRC context", "kvs"));
	return false;
}

bool KviKvsRunTimeContext::warningNoIrcConnection()
{
	warning(m_pDefaultReportLocation, __tr2qs_ctx("You're not connected to an IRC server", "kvs"));
	return true;
}

bool KviKvsRunTimeContext::warningMissingParameter()
{
	warning(m_pDefaultReportLocation, __tr2qs_ctx("Missing parameter", "kvs"));
	return true;
}

void KviKvsRunTimeContext::setDefaultReportLocation(KviKvsTreeNode * pNode)
{
	m_pDefaultReportLocation = pNode;
}
