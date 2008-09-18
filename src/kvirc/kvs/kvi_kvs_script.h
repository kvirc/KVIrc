#ifndef _KVI_KVS_SCRIPT_H_
#define _KVI_KVS_SCRIPT_H_
//=============================================================================
//
//   File : kvi_kvs_script.h
//   Creation date : Thu 25 Sep 2003 05.12 CEST by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2003-2008 Szymon Stefanek (pragma at kvirc dot net)
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
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#include "kvi_settings.h"
#include "kvi_qstring.h"
#include "kvi_window.h"
#include "kvi_pointerlist.h"
#include "kvi_kvs_variant.h"
#include "kvi_kvs_variantlist.h"
#include "kvi_heapobject.h"

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

class KVIRC_API KviKvsScript : public KviHeapObject
{
	friend class KviKvsObject;
	friend class KviKvsParser;
	friend class KviKvsRunTimeContext;
public:
	enum RunStatus {
		// the script returned an error
		Error = 0,
		// the script ran succesfully
		Success = 1,
		// the script ran succesfully and halt was encountered
		HaltEncountered = 2
	};
	enum ScriptType {
		// the most common script type: a sequence of instructions
		InstructionList,
		// an expression to be evaluated as in a $() call (pRetVal should be always set!)
		Expression,
		// a parameter to be evaluated (pRetVal should be always set!)
		Parameter
	};
protected:
	// the name parameter is the name of the script context!
	KviKvsScript(const QString &szName,const QString &szBuffer,KviKvsTreeNodeInstruction * pPreparsedTree,ScriptType eType = InstructionList);
public:
	// shallow copy of the script data
	// useful when a script can be destroyed while running (like in timers)
	KviKvsScript(const KviKvsScript &src);
	KviKvsScript(const QString &szName,const QString &szBuffer,ScriptType eType=InstructionList);
	~KviKvsScript();
private:
	KviKvsScriptData * m_pData;
public:
	const QString & name() const;
	const QString & code() const;
	bool locked() const;

	void setName(const QString &szName);

	enum RunFlags {
		// do not delete the eventual parameters passed (only execute() and run())
		PreserveParams = 1,
		// assume that the variables are local unless explicitly declared (flag used only for parse())
		AssumeLocals = 2, // FIXME: This should be a global option, eventually
		// be more pedantic: spit more warnings and sometimes more errors
		Pedantic = 4, // FIXME: This should be a global option, eventually
		// don't print any errors
		Quiet = 8
	};
	// returns 0 (KviKvsScript::RunFailure) on error
	// returns a nonzero combination of RunStatus flags on success
	int run(KviWindow * pWnd,                                // window that the command has to be bound to
				KviKvsVariantList * pParams = 0,              // parameter list (0 if you don't pass params) ownership transferred if PreserverParams is not used
				KviKvsVariant * pRetVal = 0,                  // return value buffer (0 if you ignore it)
				int iRunFlags = 0,                            // a combination of run flags (usually default)
				KviKvsExtendedRunTimeData * pExtData = 0);    // extended data (usually 0) (if you need to pass extended scope variables or alias switch lists...)

	// returns 0 (KviKvsScript::RunFailure) on error
	// returns a nonzero combination of RunStatus flags on success
	// this is probably used only in /eval
	int run(KviKvsRunTimeContext * pContext,int iRunFlags = 0);

	// same as run above, but gets a QString parameter as return buffer
	// this is probably useful only for evaluating InstructionList scripts
	int run(KviWindow * pWnd,
				KviKvsVariantList * pParams,
				QString &szRetVal,
				int iRunFlags = 0,
				KviKvsExtendedRunTimeData * pExtData = 0);

	// static helpers for quick running
	// returns a combination of RunStatus flags (nonzero on no error)
	// does NOT take params ownership
	static int run(const QString &szCode,KviWindow * pWindow,KviKvsVariantList * pParams = 0,KviKvsVariant * pRetVal = 0);

	// static helper for quick evaluating parameters
	// returns a combination of RunStatus flags (nonzero on no error)
	// does NOT take params ownership
	// pRetVal CAN'T be zero here since we're evaluating stuff here
	static int evaluate(const QString &szCode,KviWindow * pWindow,KviKvsVariantList * pParams,KviKvsVariant * pRetVal);
	static int evaluateAsString(const QString &szCode,KviWindow * pWindow,KviKvsVariantList * pParams,QString &szRetVal);
public:
	void dump(const char * prefix);
protected:
	// returns true after a succesfull parsing
	// pOutput is useful only for printing errors
	// if 0 , no errors are printed
	bool parse(KviWindow * pOutput = 0,
				int iRunFlags = 0);
	// returns 0 (KviKvsScript::RunFailure) on error
	// returns a nonzero combination of RunStatus flags on success
	int execute(KviWindow * pWnd,
				KviKvsVariantList * pParams = 0,
				KviKvsVariant * pRetVal = 0,
				int iRunFlags = 0,
				KviKvsExtendedRunTimeData * pExtData = 0);
	// returns 0 (KviKvsScript::RunFailure) on error
	// returns a nonzero combination of RunStatus flags on success
	int executeInternal(KviKvsRunTimeContext * pContext);
	const QChar * buffer() const;
	// detaches this script from any other shallow copies
	void detach();
};


#endif //!_KVI_KVS_SCRIPT_H_
