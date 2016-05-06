//=============================================================================
//
//   File : OptionsWidget_uparser.cpp
//   Creation date : Sat Oct 27 16:32:26 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2001-2010 Szymon Stefanek (pragma at kvirc dot net)
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

#include "OptionsWidget_uparser.h"

#include "KviOptions.h"
#include "KviLocale.h"

#include <QLayout>
#include <QLabel>

OptionsWidget_uparser::OptionsWidget_uparser(QWidget * parent)
    : KviOptionsWidget(parent, "uparser_options_widget")
{
	createLayout();

	KviBoolSelector * b;
	KviBoolSelector * b1;

	addBoolSelector(0, 0, 0, 0, __tr2qs_ctx("Disable parser warnings", "options"), KviOption_boolAvoidParserWarnings);
	addBoolSelector(0, 1, 0, 1, __tr2qs_ctx("Disable broken event handlers", "options"), KviOption_boolDisableBrokenEventHandlers);
	addBoolSelector(0, 2, 0, 2, __tr2qs_ctx("Kill broken timers", "options"), KviOption_boolKillBrokenTimers);
	addBoolSelector(0, 3, 0, 3, __tr2qs_ctx("Send unknown commands as /RAW", "options"), KviOption_boolSendUnknownCommandsAsRaw);

	addSeparator(0, 4, 0, 4);

	addBoolSelector(0, 5, 0, 5, __tr2qs_ctx("Automatically unload unused modules", "options"), KviOption_boolCleanupUnusedModules);
	addBoolSelector(0, 6, 0, 6, __tr2qs_ctx("Ignore module versions (dangerous)", "options"), KviOption_boolIgnoreModuleVersions);

	addSeparator(0, 7, 0, 7);

	b = addBoolSelector(0, 8, 0, 8, __tr2qs_ctx("Relay errors and warnings to debug window", "options"), KviOption_boolScriptErrorsToDebugWindow);
	mergeTip(b, __tr2qs_ctx("This option will show the script errors and warnings "
	                        "also in the special debug window. This makes tracking of scripts that might "
	                        "be running in several windows far easier. The messages in the debug window "
	                        "also contain a deeper call stack which will help you to identify the "
	                        "scripting problems.", "options"));

	b1 = addBoolSelector(0, 9, 0, 9, __tr2qs_ctx("Create debug window without focus", "options"), KviOption_boolShowMinimizedDebugWindow);
	mergeTip(b1, __tr2qs_ctx("This option prevents the debug window "
	                         "from opening and diverting application focus.<br>"
	                         "Enable this if you don't like the debug window "
	                         "popping up while you're typing something in a channel.", "options"));

	addRowSpacer(0, 10, 0, 10);
}

OptionsWidget_uparser::~OptionsWidget_uparser()
    = default;
