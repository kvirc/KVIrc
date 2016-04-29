#ifndef _KVI_KVS_RUNTIMECALL_H_
#define _KVI_KVS_RUNTIMECALL_H_
//=============================================================================
//
//   File : KviKvsRunTimeCall.h
//   Creation date : Sat 23 Apr 2005 18:19:38 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2005-2010 Szymon Stefanek <pragma at kvirc dot net>
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
#include "KviKvsRunTimeContext.h"
#include "KviKvsVariantList.h"
#include "KviKvsVariant.h"
#include "KviWindow.h"

class KVIRC_API KviKvsRunTimeCall
{
protected:
	KviKvsRunTimeContext * m_pContext; // shallow
	KviKvsVariantList * m_pParams;     // shallow
public:
	KviKvsRunTimeCall(KviKvsRunTimeContext * pContext, KviKvsVariantList * pParams)
	    : m_pContext(pContext), m_pParams(pParams){};
	virtual ~KviKvsRunTimeCall(){};

public:
	// the script runtime context, never zero
	KviKvsRunTimeContext * context() { return m_pContext; };

	KviWindow * window() { return m_pContext->window(); };

	// parameter list access & helpers.. almost unused
	// because of the KVSM_PARAMETER macros
	KviKvsVariantList * parameterList() { return m_pParams; };
	KviKvsVariantList * params() { return m_pParams; }; // the short version
	unsigned int paramCount() { return m_pParams->count(); };
	unsigned int parameterCount() { return m_pParams->count(); };
	KviKvsVariant * firstParam() { return m_pParams->first(); };
	KviKvsVariant * nextParam() { return m_pParams->next(); };

	// This is virtual and returns false by default
	// only few derived classes allow the parameter code to be
	// extracted. This is mainly useful in module callback commands
	// where a special parsing routine can not be implemented
	// but parameter code is needed for later evaluation. (see /addon.register for example)
	// Returns false if the parameter code cannot be extracted and true otherwise.
	virtual bool getParameterCode(unsigned int uParamIdx, QString & szParamBuffer);

	// forwarders from the context
	// this MUST be called before any blocking call that might return to the main event loop
	// and eventually quit kvirc or close the window that this command is associated to
	// Actually this is a NO-OP but later may really do something that avoids kvirc to crash
	void enterBlockingSection() { m_pContext->enterBlockingSection(); };
	// this MUST be called after exiting the blocking section above
	// if this function returns false your parsing code MUST "return false" immediately:
	// it means that something rather critical happened and the script
	// execution cannot continue
	bool leaveBlockingSection() { return m_pContext->leaveBlockingSection(); };

	// forwarders for context: they ease the porting
	void warning(QString szFmt, ...);
	bool error(QString szFmt, ...); // this ALWAYS returns false
};

#endif //!_KVI_KVS_RUNTIMECALL_H_
