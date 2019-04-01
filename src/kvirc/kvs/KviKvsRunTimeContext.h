#ifndef _KVI_KVS_RUNTIMECONTEXT_H_
#define _KVI_KVS_RUNTIMECONTEXT_H_
//=============================================================================
//
//   File : KviKvsRunTimeContext.h
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

#include "kvi_settings.h"
#include "KviWindow.h"

#include "KviKvsVariant.h"
#include "KviKvsHash.h"
#include "KviKvsVariantList.h"
#include "KviKvsSwitchList.h"

class KviKvsScript;
class KviConsoleWindow;
class KviIrcContext;
class KviIrcConnection;
class KviKvsTreeNode;
class KviKvsObject;
class KviKvsReportHandler;

class KVIRC_API KviKvsExtendedRunTimeData
{
	friend class KviKvsRunTimeContext;

protected:
	KviKvsHash * m_pExtendedScopeVariables; // extended scope vars, shallow, may be 0
	KviKvsSwitchList * m_pAliasSwitchList;  // switches for the aliases, shallow, may be 0
	KviKvsObject * m_pThisObject;           // the current object for object function calls
	QString * m_pScriptFilePath;            // the current script file path, shallow, may be 0
	bool m_bAutoDelete;
	QString * m_pPopupId;

public:
	// all shallow data, all may be 0
	KviKvsExtendedRunTimeData()
	    : m_pExtendedScopeVariables(0),
	      m_pAliasSwitchList(0),
	      m_pThisObject(0),
	      m_pScriptFilePath(0),
	      m_bAutoDelete(false),
	      m_pPopupId(0){};
	KviKvsExtendedRunTimeData(KviKvsHash * pExtScopeVariables, bool autoDelete = false)
	    : m_pExtendedScopeVariables(pExtScopeVariables),
	      m_pAliasSwitchList(0),
	      m_pThisObject(0),
	      m_pScriptFilePath(0),
	      m_bAutoDelete(autoDelete),
	      m_pPopupId(0){};
	KviKvsExtendedRunTimeData(KviKvsSwitchList * pAliasSwitchList, bool autoDelete = false)
	    : m_pExtendedScopeVariables(0),
	      m_pAliasSwitchList(pAliasSwitchList),
	      m_pThisObject(0),
	      m_pScriptFilePath(0),
	      m_bAutoDelete(autoDelete),
	      m_pPopupId(0){};
	KviKvsExtendedRunTimeData(KviKvsObject * pThisObject, bool autoDelete = false)
	    : m_pExtendedScopeVariables(0),
	      m_pAliasSwitchList(0),
	      m_pThisObject(pThisObject),
	      m_pScriptFilePath(0),
	      m_bAutoDelete(autoDelete),
	      m_pPopupId(0){};
	KviKvsExtendedRunTimeData(QString * pScriptFilePath, bool autoDelete = false)
	    : m_pExtendedScopeVariables(0),
	      m_pAliasSwitchList(0),
	      m_pThisObject(0),
	      m_pScriptFilePath(pScriptFilePath),
	      m_bAutoDelete(autoDelete),
	      m_pPopupId(0){};
	~KviKvsExtendedRunTimeData();

public:
	KviKvsHash * extendedScopeVariables() { return m_pExtendedScopeVariables; };
	KviKvsSwitchList * aliasSwitchList() { return m_pAliasSwitchList; };
	KviKvsObject * thisObject() { return m_pThisObject; };
	QString * scriptFilePath() { return m_pScriptFilePath; };
	QString * popupId() { return m_pPopupId; };
	void setPopupId(QString * pPopupId);
};

class KVIRC_API KviKvsRunTimeContext
{
	friend class KviKvsScript;
	friend class KviKvsObject;
	friend class KviKvsRunTimeCall;
	friend class KviKvsEventManager;

protected:
	// only KviKvsScript, KviKvsEventManager and KviKvsObject can instantiate this class
	KviKvsRunTimeContext(KviKvsScript * pScript,
	    KviWindow * pWnd,
	    KviKvsVariantList * pParams,
	    KviKvsVariant * pRetVal,
	    KviKvsExtendedRunTimeData * pExtData = nullptr);

public:
	~KviKvsRunTimeContext();

protected:
	// stuff that is fixed in the whole script context
	KviKvsScript * m_pScript;             // shallow, may be 0!
	KviKvsHash * m_pLocalVariables;       // owned, never 0
	KviKvsVariantList * m_pParameterList; // shallow, never 0
	KviKvsVariant * m_pReturnValue;       // shallow, never 0

	// stuff that is generally global but sometimes may change
	// during the execution of the script
	KviWindow * m_pWindow; // shallow, never 0

	enum RunTimeFlags
	{
		BreakPending = 1,
		ContinuePending = 2,
		HaltCalled = 4,
		DisableReporting = 8
	};
	unsigned int m_uRunTimeFlags; // a combination of RunTimeFlags

	KviKvsExtendedRunTimeData * m_pExtendedData; // shallow, may be 0

	// error handling
	bool m_bError;                             // was error() ever called ?
	KviKvsTreeNode * m_pDefaultReportLocation; // default report location for error()
public:
	// the window that this script is bound to (it MAY change during the script parsing)
	KviWindow * window()
	{
		return m_pWindow;
	};
	// quick access to the irc context (it MAY be zero!)
	KviIrcContext * context()
	{
		return m_pWindow->context();
	};
	// quick access to the console that this script is bound to (it MAY be zero, if there is no console)
	KviConsoleWindow * console()
	{
		return m_pWindow->console();
	};
	// quick access to the connection: it MAY be zero!
	KviIrcConnection * connection()
	{
		return m_pWindow->connection();
	};

	// the local variables of this script
	KviKvsHash * localVariables()
	{
		return m_pLocalVariables;
	};
	// the global application-wide variables
	KviKvsHash * globalVariables();
	// the parameters passed to this script
	KviKvsVariantList * parameterList()
	{
		return m_pParameterList;
	};

	// parent script, may be 0!
	KviKvsScript * script()
	{
		return m_pScript;
	};

	KviKvsVariant * returnValue()
	{
		return m_pReturnValue;
	};

	// this is the default error reporting location
	// it is also used by rfc2812wrapper to find out the current command name!
	KviKvsTreeNode * defaultReportLocation()
	{
		return m_pDefaultReportLocation;
	};

	// the extended stuff
	KviKvsHash * extendedScopeVariables()
	{
		return m_pExtendedData ? m_pExtendedData->extendedScopeVariables() : 0;
	};
	KviKvsSwitchList * aliasSwitchList()
	{
		return m_pExtendedData ? m_pExtendedData->aliasSwitchList() : 0;
	};
	KviKvsObject * thisObject()
	{
		return m_pExtendedData ? m_pExtendedData->thisObject() : 0;
	};
	QString * scriptFilePath()
	{
		return m_pExtendedData ? m_pExtendedData->scriptFilePath() : 0;
	};
	QString * popupId()
	{
		return m_pExtendedData ? m_pExtendedData->popupId() : 0;
	};

	// this MUST be called before any blocking call that might return to the main event loop
	// and eventually quit kvirc or close the window that this command is associated to
	// Actually this is a NO-OP but later may really do something that avoids kvirc to crash
	void enterBlockingSection();
	// this MUST be called after exiting the blocking section above
	// if this function returns false your parsing code MUST "return false" immediately:
	// it means that something rather critical happened and the script
	// execution cannot continue
	bool leaveBlockingSection();

	// Error handling

	// was error(...) ever called ?
	bool error() const { return m_bError; };

	void error(KviKvsTreeNode * pNode, QString szMsgFmt, ...);
	void warning(KviKvsTreeNode * pNode, QString szMsgFmt, ...);
	void error(QString szMsgFmt, ...);
	void warning(QString szMsgFmt, ...);
	void setDefaultReportLocation(KviKvsTreeNode * pNode);
	bool errorNoIrcContext();
	bool warningNoIrcConnection();
	bool warningMissingParameter();

	//PRIVATE:
	// tricky
	// don't use it: it's only for the parser
	void setWindow(KviWindow * pWnd)
	{
		m_pWindow = pWnd;
	};

	// returns the old pointer
	KviKvsVariant * swapReturnValuePointer(KviKvsVariant * pNewPointer);
	// the old pointer MUST be reset!

	// this is called by the parser when a break is encountered
	// the parser calls setBreakPending() and returns false
	// the contexts that can handle a break should check breakPending()
	// after an internal evaluation returned false.
	// if breakPending() they should handle it with handleBreak() and
	// return true instead.
	void setBreakPending()
	{
		m_uRunTimeFlags |= BreakPending;
	};
	// this tells if a break command has been called
	// it may return true if an execution() command has returned false
	// (if breakPending() there should be no error()
	bool breakPending()
	{
		return (m_uRunTimeFlags & BreakPending);
	};
	// this is called by the commands that can handle a break
	void handleBreak()
	{
		m_uRunTimeFlags &= ~BreakPending;
	};

	// this is called by the parser when a continue is encountered
	void setContinuePending()
	{
		m_uRunTimeFlags |= ContinuePending;
	};
	// this tells if a continue command has been called
	bool continuePending()
	{
		return (m_uRunTimeFlags & ContinuePending);
	};
	// this is called by the commands that can handle a continue
	void handleContinue()
	{
		m_uRunTimeFlags &= ~ContinuePending;
	};

	// this is called by the parser when a halt is encountered
	// the parser then returns false and all the stack frames
	// above should do the same
	void setHaltCalled()
	{
		m_uRunTimeFlags |= HaltCalled;
	};
	bool haltCalled()
	{
		return (m_uRunTimeFlags & HaltCalled);
	};

	// this is used by KviKvsScript to disable reporting when the Quiet flag is used
	void disableReporting()
	{
		m_uRunTimeFlags |= DisableReporting;
	};
	bool reportingDisabled()
	{
		return (m_uRunTimeFlags & DisableReporting);
	};
	void enableReporting()
	{
		m_uRunTimeFlags &= ~DisableReporting;
	};

	// clears the error status on this context
	// this is useful to continue execution after an error
	// and probably used only in /eval
	void clearError()
	{
		m_bError = false;
	};

protected:
	void report(bool bError, KviKvsTreeNode * pNode, const QString & szMsgFmt, kvi_va_list va);
};

#endif //!_KVI_KVS_RUNTIMECONTEXT_H_
