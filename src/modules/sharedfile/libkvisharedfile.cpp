//=============================================================================
//
//   File : libkvisharedfile.cpp (orig : libkvioffer.cpp)
//   Creation date : Wed Sep 27 2000 20:59:02 CEST by Szymon Stefanek
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
//=============================================================================

#include "KviModule.h"
#include "KviSharedFilesManager.h"
#include "KviIrcMask.h"
#include "KviFileUtils.h"
#include "KviLocale.h"
#include "kvi_out.h"
#include "KviControlCodes.h"
#include "KviWindow.h"
#include "KviMainWindow.h"
#include "KviPointerHashTable.h"

#include <ctime>

extern KVIRC_API KviSharedFilesManager * g_pSharedFilesManager;

/*
	@doc: sharedfile.add
	@type:
		command
	@title:
		sharedfile.add
	@keyterms:
		trading files by DCC
	@short:
		Adds a file sharedfile
	@syntax:
		sharedfile.add [-t=<timeout:integer>] [-n=<visible name>] <filename> [user_mask]
	@switches:
		!sw: -t=<timeout> | --timeout=<timeout>
		Specified the timeout after that the share will be automatically removed
		!sw: -n=<visible name> | --name=<visible name>
		Explicitly specifies the visible name of the share
	@description:
		Adds <filename> to the list of the active shared files.
		The users will be able to request the file via [cmd]dcc.get[/cmd].
		If [user_mask] is specified, the access to the file is limited
		to the users that match this mask, otherwise the mask will
		be automatically set to '*!*@*'.[br]
		If the 't' switch is used, the sharedfile will be removed after
		<timeout> seconds. If the 'n' switch is used, the sharedfile
		will be visible to the other users as <visible name> instead
		of the real <filename> (stripped of the leading path).
		<filename> must be an absolute path.
	@seealso:
		[cmd]sharedfile.remove[/cmd], [cmd]sharedfile.list[/cmd],
		[cmd]dcc.get[/cmd]
*/

static bool sharedfile_kvs_cmd_add(KviKvsModuleCommandCall * c)
{
	QString szFileName, szUserMask;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("filename", KVS_PT_NONEMPTYSTRING, 0, szFileName)
	KVSM_PARAMETER("user_mask", KVS_PT_NONEMPTYSTRING, KVS_PF_OPTIONAL, szUserMask)
	KVSM_PARAMETERS_END(c)

	if(!KviFileUtils::isReadable(szFileName))
	{
		c->warning(__tr2qs_ctx("The file '%Q' is not readable", "sharedfileswindow"), &szFileName);
		return true;
	}

	if(szUserMask.isEmpty())
		szUserMask = "*!*@*";
	KviIrcMask u(szUserMask);

	QString szm;
	u.mask(szm);

	kvs_int_t timeout = 0;

	if(KviKvsVariant * v = c->switches()->find('t', "timeout"))
	{
		if(!v->asInteger(timeout))
		{
			c->warning(__tr2qs_ctx("Invalid timeout, ignoring", "sharedfileswindow"));
			timeout = 0;
		}
	}

	QString szVisibleName = szFileName;
	KviQString::cutToLast(szVisibleName, '/');

	if(KviKvsVariant * n = c->switches()->find('n', "name"))
	{
		QString tmp;
		n->asString(tmp);
		if(tmp.isEmpty())
		{
			c->warning(__tr2qs_ctx("Invalid visible name: using default", "sharedfileswindow"));
		}
		else
			szVisibleName = tmp;
	}

	if(!g_pSharedFilesManager->addSharedFile(szVisibleName, szFileName, szUserMask, timeout))
	{
		c->warning(__tr2qs_ctx("Oops! Failed to add the sharedfile...", "sharedfileswindow"));
	}

	return true;
}

/*
	@doc: sharedfile.remove
	@type:
		command
	@title:
		sharedfile.remove
	@keyterms:
		trading files by DCC
	@short:
		Removes a shared file
	@syntax:
		sharedfile.remove <visible name:string> <user mask:string> [filesize:integer]
	@description:
		Removes the shared file that matches <visible name> and <user mask>.
		If [filesize] is specified, then it must be matched by the entry
		to be removed.
	@seealso:
		[cmd]sharedfile.add[/cmd], [cmd]sharedfile.list[/cmd], [cmd]sharedfile.clear[/cmd]
*/

static bool sharedfile_kvs_cmd_remove(KviKvsModuleCommandCall * c)
{
	QString szVisibleName, szUserMask;
	kvs_uint_t uSize;
	KVSM_PARAMETERS_BEGIN(c)
	KVSM_PARAMETER("visible_name", KVS_PT_NONEMPTYSTRING, 0, szVisibleName)
	KVSM_PARAMETER("user_mask", KVS_PT_NONEMPTYSTRING, 0, szUserMask)
	KVSM_PARAMETER("filesize", KVS_PT_UINT, KVS_PF_OPTIONAL, uSize)
	KVSM_PARAMETERS_END(c)

	if(!g_pSharedFilesManager->removeSharedFile(szVisibleName, szUserMask, uSize))
		c->warning(__tr2qs_ctx("No sharedfile with visible name '%Q' and user mask '%Q'", "sharedfileswindow"), &szVisibleName, &szUserMask);

	return true;
}

/*
	@doc: sharedfile.clear
	@type:
		command
	@title:
		sharedfile.clear
	@keyterms:
		trading files by DCC, shared files
	@short:
		Clears the shared files list
	@syntax:
		sharedfile.clear
	@description:
		Clears the shared files list
	@seealso:
		[cmd]sharedfile.add[/cmd], [cmd]sharedfile.list[/cmd], [cmd]sharedfile.remove[/cmd]
*/

static bool sharedfile_kvs_cmd_clear(KviKvsModuleCommandCall *)
{
	g_pSharedFilesManager->clear();
	return true;
}

/*
	@doc: sharedfile.list
	@type:
		command
	@title:
		sharedfile.list
	@keyterms:
		trading files by DCC
	@short:
		Lists the active file sharedfile
	@syntax:
		sharedfile.list
	@description:
		Lists the active file sharedfile.
	@seealso:
		[cmd]sharedfile.add[/cmd], [cmd]sharedfile.remove[/cmd]
*/

static bool sharedfile_kvs_cmd_list(KviKvsModuleCommandCall * c)
{
	KviPointerHashTableIterator<QString, KviSharedFileList> it(*(g_pSharedFilesManager->sharedFileListDict()));

	int idx = 0;

	while(KviSharedFileList * l = it.current())
	{
		for(KviSharedFile * o = l->first(); o; o = l->next())
		{
			c->window()->output(KVI_OUT_NONE, "%c%d. %s",
			    KviControlCodes::Bold, idx + 1, it.currentKey().toUtf8().data());
			c->window()->output(KVI_OUT_NONE, __tr2qs_ctx("File: %s (%u bytes)", "sharedfileswindow"),
			    o->absFilePath().toUtf8().data(), o->fileSize());
			c->window()->output(KVI_OUT_NONE, __tr2qs_ctx("Mask: %s", "sharedfileswindow"),
			    o->userMask().toUtf8().data());
			if(o->expireTime() > 0)
			{
				int secs = ((int)(o->expireTime())) - ((int)(time(nullptr)));
				int hour = secs / 3600;
				secs = secs % 3600;
				int mins = secs / 60;
				secs = secs % 60;
				c->window()->output(KVI_OUT_NONE, __tr2qs_ctx("Expires in %d hours %d minutes %d seconds", "sharedfileswindow"),
				    hour, mins, secs);
			}
			++idx;
		}
		++it;
	}

	//#warning "FIND A BETTER KVI_OUT_*"

	if(idx == 0)
		c->window()->outputNoFmt(KVI_OUT_NONE, __tr2qs_ctx("No active file sharedfile", "sharedfileswindow"));
	else
		c->window()->output(KVI_OUT_NONE, __tr2qs_ctx("Total: %d sharedfile", "sharedfileswindow"), idx);

	return true;
}

static bool sharedfile_module_init(KviModule * m)
{

	KVSM_REGISTER_SIMPLE_COMMAND(m, "add", sharedfile_kvs_cmd_add);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "remove", sharedfile_kvs_cmd_remove);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "list", sharedfile_kvs_cmd_list);
	KVSM_REGISTER_SIMPLE_COMMAND(m, "clear", sharedfile_kvs_cmd_clear);

	return true;
}

static bool sharedfile_module_can_unload(KviModule *)
{
	return true;
}

static bool sharedfile_module_cleanup(KviModule *)
{
	return true;
}

KVIRC_MODULE(
    "SharedFiles",                                                       // module name
    "4.0.0",                                                             // module version
    "Copyright (C) 2000-2003 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
    "User interface to the file sharing system",
    sharedfile_module_init,
    sharedfile_module_can_unload,
    0,
    sharedfile_module_cleanup,
    "sharedfileswindow")
