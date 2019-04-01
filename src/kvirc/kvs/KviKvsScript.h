#ifndef _KVI_KVS_SCRIPT_H_
#define _KVI_KVS_SCRIPT_H_
//=============================================================================
//
//   File : KviKvsScript.h
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

/**
* \file KviKvsScript.h
* \author Szymon Stefanek
* \brief KVIrc Script manager
*/

#include "kvi_settings.h"
#include "KviQString.h"
#include "KviWindow.h"
#include "KviPointerList.h"
#include "KviKvsVariant.h"
#include "KviKvsVariantList.h"
#include "KviHeapObject.h"

class KviKvsTreeNodeInstruction;
class KviKvsExtendedRunTimeData;
class KviKvsScriptData;
class KviKvsReport;
class KviKvsRunTimeContext;

// X defines this stuff: ugly :/
#ifdef Error
#undef Error
#endif
#ifdef Success
#undef Success
#endif

/**
* \class KviKvsScript
* \brief The KVIrc Script class
*/
class KVIRC_API KviKvsScript : public KviHeapObject
{
	friend class KviKvsObject;
	friend class KviKvsParser;
	friend class KviKvsRunTimeContext;

public:
	/**
	* \enum RunStatus
	* \brief Holds the run status of the script
	*/
	enum RunStatus
	{
		Error = 0,          /**< The script returned an error */
		Success = 1,        /**< The script ran successfully */
		HaltEncountered = 2 /**< The script ran successfully and halt was encountered */
	};

	/**
	* \enum ScriptType
	* \brief Holds the type of the script
	*/
	enum ScriptType
	{
		InstructionList, /**< The most common script type: a sequence of instructions */
		Expression,      /**< An expression to be evaluated as in a $() call (pRetVal should be always set!) */
		Parameter        /**< A parameter to be evaluated (pRetVal should be always set!) */
	};

	/**
	* \enum RunFlags
	* \brief Holds the run flags of the script
	*/
	enum RunFlags
	{
		PreserveParams = 1, /**< Do not delete the eventual parameters passed (only execute() and run()) */
		// FIXME: This should be a global option, eventually
		AssumeLocals = 2, /**< Assume that the variables are local unless explicitly declared (flag used only for parse()) */
		// FIXME: This should be a global option, eventually
		Pedantic = 4, /**< Be more pedantic: spit more warnings and sometimes more errors */
		Quiet = 8     /**< Don't print any errors */
	};

public:
	/**
	* \brief Constructs a KVIrc Script object
	*
	* This is a shallow copy of the script data useful when a script can
	* be destroyed while running (like in timers)
	* \param src The source script
	* \return KviKvsScript
	*/
	KviKvsScript(const KviKvsScript & src);

	/**
	* \brief Constructs a KVIrc Script object
	* \param szName The name of the context
	* \param szBuffer The buffer :)
	* \param eType The type of the code in the buffer
	* \return KviKvsScript
	*/
	KviKvsScript(const QString & szName, const QString & szBuffer, ScriptType eType = InstructionList);

	/**
	* \brief Destroys a KVIrc Script object
	*/
	~KviKvsScript();

protected:
	/**
	* \brief Constructs a KVIrc Script object
	* \param szName The name of the context
	* \param szBuffer The buffer :)
	* \param pPreparsedTree The synthax tree
	* \param eType The type of the code in the buffer
	* \return KviKvsScript
	*/
	KviKvsScript(const QString & szName, const QString & szBuffer, KviKvsTreeNodeInstruction * pPreparsedTree, ScriptType eType = InstructionList);

private:
	KviKvsScriptData * m_pData;

public:
	/**
	* \brief Returns the name of the script context
	* \return const QString &
	*/
	const QString & name() const;

	/**
	* \brief Returns the code of the script
	* \return const QString &
	*/
	const QString & code() const;

	/**
	* \brief Returns true if the script is locked, false otherwise
	*
	* The lock is set while the script is being executed
	* \return bool
	*/
	bool locked() const;

	/**
	* \brief Sets the name of the script context
	* \param szName The name of the context
	* \return void
	*/
	void setName(const QString & szName);

	/**
	* \brief Runs the script
	*
	* Returns 0 (KviKvsScript::RunFailure) on error
	* Returns a nonzero combination of RunStatus flags on success
	*
	* If PreserverParams is not used, the ownership is transferred.
	* Extended data is used if you need to pass extended scope variables or alias switch lists...)
	* \param pWnd The window that the command has to be bound to
	* \param pParams The parameter list (0 if you don't pass params)
	* \param pRetVal Return value buffer (0 if you ignore it)
	* \param iRunFlags A combination of run flags (usually default)
	* \param pExtData Extended data (usually 0)
	* \return int
	*/
	int run(KviWindow * pWnd, KviKvsVariantList * pParams = nullptr, KviKvsVariant * pRetVal = nullptr, int iRunFlags = 0, KviKvsExtendedRunTimeData * pExtData = nullptr);

	/**
	* \brief Runs the script
	*
	* Returns 0 (KviKvsScript::RunFailure) on error
	* Returns a nonzero combination of RunStatus flags on success
	*
	* The QString return buffer is useful only for evaluating InstructionList scripts.
	* If PreserverParams is not used, the ownership is transferred.
	* Extended data is used if you need to pass extended scope variables or alias switch lists...)
	* \param pWnd The window that the command has to be bound to
	* \param pParams The parameter list (0 if you don't pass params)
	* \param szRetVal Return value buffer
	* \param iRunFlags A combination of run flags (usually default)
	* \param pExtData Extended data (usually 0)
	* \return int
	*/
	int run(KviWindow * pWnd, KviKvsVariantList * pParams, QString & szRetVal, int iRunFlags = 0, KviKvsExtendedRunTimeData * pExtData = nullptr);

	/**
	* \brief Runs the script
	*
	* Returns 0 (KviKvsScript::RunFailure) on error
	* Returns a nonzero combination of RunStatus flags on success
	*
	* This is probably used only in /eval
	* \param pContext The context where the script is bound to
	* \param iRunFlags A combination of run flags (usually default)
	* \return int
	*/
	int run(KviKvsRunTimeContext * pContext, int iRunFlags = 0);

	/**
	* \brief Static helper for quick running
	*
	* Returns a combination of RunStatus flags (nonzero on no error)
	* It does NOT take params ownership
	* \param szCode The source code to run
	* \param pWindow The window that the command has to be bound to
	* \param pParams The parameter list (0 if you don't pass params)
	* \param pRetVal Return value buffer (0 if you ignore it)
	* \return int
	*/
	static int run(const QString & szCode, KviWindow * pWindow, KviKvsVariantList * pParams = nullptr, KviKvsVariant * pRetVal = nullptr);

	/**
	* \brief Static helper for quick evaluating parameters
	*
	* Returns a combination of RunStatus flags (nonzero on no error)
	* It does NOT take params ownership.
	* pRetVal CAN'T be zero here since we're evaluating stuff here
	* \param szCode The source code to run
	* \param pWindow The window that the command has to be bound to
	* \param pParams The parameter list
	* \param pRetVal Return value buffer
	* \return int
	*/
	static int evaluate(const QString & szCode, KviWindow * pWindow, KviKvsVariantList * pParams, KviKvsVariant * pRetVal);

	/**
	* \brief Static helper for quick evaluating parameters
	*
	* Returns a combination of RunStatus flags (nonzero on no error)
	* It does NOT take params ownership.
	* The QString return buffer CAN'T be zero here since we're evaluating stuff here
	* \param szCode The source code to run
	* \param pWindow The window that the command has to be bound to
	* \param pParams The parameter list
	* \param szRetVal Return value buffer
	* \return int
	*/
	static int evaluateAsString(const QString & szCode, KviWindow * pWindow, KviKvsVariantList * pParams, QString & szRetVal);

	/**
	* \brief Dumps the instructions tree
	* \param prefix The prefix of the instruction
	* \return void
	*/
	void dump(const char * prefix);

protected:
	/**
	* \brief Returns true after a successful parsing, false otherwise
	*
	* pOutput is useful only for printing errors; if 0, no errors are printed
	* \param pOutput The output window for errors
	* \param iRunFlags A combination of run flags (usually default)
	* \return bool
	*/
	bool parse(KviWindow * pOutput = nullptr, int iRunFlags = 0);

	/**
	* \brief Runs the script
	*
	* Returns 0 (KviKvsScript::RunFailure) on error
	* Returns a nonzero combination of RunStatus flags on success
	* \param pWnd The window that the command has to be bound to
	* \param pParams The parameter list (0 if you don't pass params)
	* \param pRetVal Return value buffer (0 if you ignore it)
	* \param iRunFlags A combination of run flags (usually default)
	* \param pExtData Extended data (usually 0)
	* \return int
	*/
	int execute(KviWindow * pWnd, KviKvsVariantList * pParams = nullptr, KviKvsVariant * pRetVal = nullptr, int iRunFlags = 0, KviKvsExtendedRunTimeData * pExtData = nullptr);

	/**
	* \brief Runs the script
	*
	* Returns 0 (KviKvsScript::RunFailure) on error
	* Returns a nonzero combination of RunStatus flags on success
	* \param pContext The context where the script is bound to
	* \return int
	*/
	int executeInternal(KviKvsRunTimeContext * pContext);

	/**
	* \brief Returns the data of the script
	* \return const QChar *
	*/
	const QChar * buffer() const;

	/**
	* \brief Detaches this script from any other shallow copies
	* \return void
	*/
	void detach();
};

/**
* \class KviKvsScriptData
* \brief Holds the data of the script
*/
class KVIRC_API KviKvsScriptData
{
	friend class KviKvsScript;

protected:
	unsigned int m_uRefs; // Reference count for this structure

	QString m_szName;        // script context name
	QString m_szBuffer;      // NEVER TOUCH THIS
	const QChar * m_pBuffer; // this points to m_szBuffer: use it to extract string data

	KviKvsScript::ScriptType m_eType; // the type of the code in m_szBuffer

	KviKvsTreeNodeInstruction * m_pTree; // syntax tree
	unsigned int m_uLock;                // this is increased while the script is being executed
};

#endif //_KVI_KVS_SCRIPT_H_
