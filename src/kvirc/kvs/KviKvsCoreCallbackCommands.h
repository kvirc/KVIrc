#ifndef _KVI_KVS_CORECALLBACKCOMMANDS_H_
#define _KVI_KVS_CORECALLBACKCOMMANDS_H_
//=============================================================================
//
//   File : KviKvsCoreCallbackCommands.h
//   Creation date : Fri 31 Oct 2003 04:07:58 by Szymon Stefanek
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
#include "KviKvsRunTimeContext.h"
#include "KviKvsVariantList.h"
#include "KviKvsSwitchList.h"
#include "KviKvsScript.h"
#include "KviKvsParameterProcessor.h"

#define KVSCCC(_name) bool _name([[maybe_unused]] KviKvsRunTimeContext * __pContext, [[maybe_unused]] KviKvsVariantList * __pParams, [[maybe_unused]] KviKvsSwitchList * __pSwitches, const KviKvsScript * __pCallback)

#define KVSCCC_pContext __pContext
#define KVSCCC_pParams __pParams
#define KVSCCC_pSwitches __pSwitches
#define KVSCCC_pCallback __pCallback

#define KVSCCC_PARAMETER(a, b, c, d) KVS_PARAMETER(a, b, c, d)

#define KVSCCC_PARAMETERS_BEGIN \
	KVS_PARAMETERS_BEGIN(parameter_format_list)

#define KVSCCC_PARAMETERS_END                                                                      \
	KVS_PARAMETERS_END                                                                             \
	if(!KviKvsParameterProcessor::process(KVSCCC_pParams, KVSCCC_pContext, parameter_format_list)) \
		return false;

#define KVSCCC_REQUIRE_CONNECTION                    \
	if(!KVSCCC_pContext->window()->context())        \
		return KVSCCC_pContext->errorNoIrcContext(); \
	if(!KVSCCC_pContext->window()->connection())     \
		return KVSCCC_pContext->warningNoIrcConnection();

#define KVSCCC_pWindow __pContext->window()
#define KVSCCC_pConnection __pContext->window()->connection()
#define KVSCCC_pIrcContext __pContext->context()

namespace KviKvsCoreCallbackCommands
{
	KVSCCC(ahost);
	KVSCCC(awhois);
	KVSCCC(alias);
	KVSCCC(button);
	KVSCCC(event);
	KVSCCC(exec);
	KVSCCC(privateimpl);
	KVSCCC(timer);

	void init();
};

#endif //!_KVI_KVS_CORECALLBACKCOMMANDS_H_
