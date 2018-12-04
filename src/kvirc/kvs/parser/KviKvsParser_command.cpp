//=============================================================================
//
//   File : KviKvsParser_command.cpp
//   Creation date : Thu 03 Nov 2003 13.23 CEST by Szymon Stefanek
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

#include "KviKvsParser.h"
#include "KviKvsTreeNode.h"
#include "KviKvsReport.h"
#include "KviKvsKernel.h"
#include "KviKvsParserMacros.h"
#include "KviLocale.h"
#include "KviKvsScript.h"
#include "KviCommandFormatter.h"

KviKvsTreeNodeCommand * KviKvsParser::parseCommand()
{
	KVSP_ASSERT(KVSP_curCharIsLetter || (KVSP_curCharUnicode == '_'));

	const QChar * pIdentifier = KVSP_curCharPointer;

	while(KVSP_curCharIsLetterOrNumber || (KVSP_curCharUnicode == '_'))
		KVSP_skipChar;

	int iIdentifierLen = KVSP_curCharPointer - pIdentifier;

	const QChar * pSecondPart = nullptr;
	int iSecondPartLen = 0;

	bool bHasNamespaceSoMustBeAlias = false;

	if(KVSP_curCharUnicode == '.')
	{
		// a module command
		KVSP_skipChar;

		pSecondPart = KVSP_curCharPointer;

		if(!KVSP_curCharIsLetter)
		{
			warning(KVSP_curCharPointer - 1, __tr2qs_ctx("Stray dot ('.') character or invalid following module command name", "kvs"));
			error(KVSP_curCharPointer, __tr2qs_ctx("Syntax error: malformed module command identifier", "kvs"));
			return nullptr;
		}

		KVSP_skipChar;
		while(KVSP_curCharIsLetterOrNumber || (KVSP_curCharUnicode == '_'))
			KVSP_skipChar;

		iSecondPartLen = KVSP_curCharPointer - pSecondPart;
	}
	else
		while(KVSP_curCharUnicode == ':')
		{
			// an alias with namespace(s) ?

			// here we allow the syntax of the form
			// <namespace>::{<namespace>::}<alias_name>

			bHasNamespaceSoMustBeAlias = true;

			KVSP_skipChar;
			if(KVSP_curCharUnicode == ':')
			{
				KVSP_skipChar;

				if(!KVSP_curCharIsLetter)
				{
					warning(KVSP_curCharPointer - 1, __tr2qs_ctx("Stray '::' sequence or invalid following alias name", "kvs"));
					error(KVSP_curCharPointer, __tr2qs_ctx("Syntax error: malformed alias identifier", "kvs"));
					return nullptr;
				}

				KVSP_skipChar;
				while(KVSP_curCharIsLetterOrNumber || (KVSP_curCharUnicode == '_'))
					KVSP_skipChar;
			}
			else
			{
				warning(KVSP_curCharPointer - 1, __tr2qs_ctx("Stray ':' character: did you mean '...<namespace>::<alias_name>'?", "kvs"));
				error(KVSP_curCharPointer, __tr2qs_ctx("Syntax error: malformed (alias?) command identifier", "kvs"));
				return nullptr;
			}

			iIdentifierLen = KVSP_curCharPointer - pIdentifier;
		}

	QString szIdentifier(pIdentifier, iIdentifierLen);

	skipSpaces();

	KviKvsTreeNodeSwitchList * sw = nullptr;
	KviKvsTreeNodeData * pRebindData;

	if(KVSP_curCharUnicode == '-')
	{
		// extract the switch
		sw = parseCommandSwitchList();
		if(!sw)
		{
			if(error())
				return nullptr;
			// else it might be a negative number or something that does not seem
			// to be a switch anyway
			pRebindData = nullptr;
		}
		else
		{
			pRebindData = sw->getStandardRebindingSwitch();
		}
	}
	else
	{
		pRebindData = nullptr;
	}

	KviKvsTreeNodeCommand * cmd = nullptr;

	if(!bHasNamespaceSoMustBeAlias)
	{
		// We're lookin' for interpreters like perl and python
		// perl.begin and python.begin have a *really* half special
		// parsing routine
		if((iIdentifierLen == 4) || (iIdentifierLen == 6))
		{
			if(pIdentifier->toLower().unicode() == 'p')
			{
				// Set the interpreter name
				QString szInterpreter;
				if(KviQString::equalCI(szIdentifier, "perl"))
				{
					szInterpreter = "perl";
				}
				else if(KviQString::equalCI(szIdentifier, "python"))
				{
					szInterpreter = "python";
				}

				if((szInterpreter == "perl") || (szInterpreter == "python"))
				{
					if(pSecondPart)
					{
						QString szSecondPart(pSecondPart, iSecondPartLen);
						if(KviQString::equalCI(szSecondPart, "begin"))
						{
							if(szInterpreter == "perl")
							{
								// yep, that's perl.begin
								cmd = parseSpecialCommandPerlBegin();
							}
							else if(szInterpreter == "python")
							{
								// yep, that's python.begin
								cmd = parseSpecialCommandPythonBegin();
							}

							if(!cmd)
							{
								// might be an error, but might be not...
								// it is an error only if error() returns true
								// but since the caller will take care of it
								// we just return nullptr
								if(sw)
									delete sw;
								if(pRebindData)
									delete pRebindData;
								return nullptr;
							}
							cmd->setLocation(pIdentifier);
							if(sw)
							{
								cmd->setSwitchList(sw);
								// cmd becomes child of the rebinding switch
								if(pRebindData)
									return new KviKvsTreeNodeRebindingSwitch(pRebindData->location(), pRebindData, cmd);
							}
							return cmd;
						}
					}
				}
			}
		}

		if(!pSecondPart)
		{
			// is this a special command ?

			// Here theoretically we could also lookup special commands composed of two parts but we actually don't need it.

			// Looking up only the first part if there is a second part, instead,
			// is dangerous since it may generate infinite loops (help.open vs help)

			KviKvsSpecialCommandParsingRoutine * ccpr = KviKvsKernel::instance()->findSpecialCommandParsingRoutine(szIdentifier);

			if(ccpr)
			{
				cmd = (this->*(ccpr->proc))();
				if(!cmd)
				{
					// might be an error, but might be not...
					// it is an error only if error() returns true
					// but since the caller will take care of it
					// we just return nullptr
					if(sw)
						delete sw;
					if(pRebindData)
						delete pRebindData;
					return nullptr;
				}
				cmd->setLocation(pIdentifier);
				if(sw)
				{
					cmd->setSwitchList(sw);
					// cmd becomes child of the rebinding switch
					if(pRebindData)
						return new KviKvsTreeNodeRebindingSwitch(pRebindData->location(), pRebindData, cmd);
				}
				return cmd;
			}
		}

		// is it a callback command ?
		if(KVSP_curCharUnicode == '(')
		{
			// core callback command
			// module callback command
			KviKvsTreeNodeDataList * dl = parseCommaSeparatedParameterList();
			if(!dl)
			{
				if(sw)
					delete sw;
				if(pRebindData)
					delete pRebindData;
				return nullptr;
			}
			if(!skipSpacesAndNewlines())
			{
				if(sw)
					delete sw;
				if(pRebindData)
					delete pRebindData;
				delete dl;
				return nullptr;
			}

			const QChar * pClbkBegin = KVSP_curCharPointer;

			KviKvsTreeNodeInstruction * ins = parseInstruction();
			if(!ins)
			{
				if(error())
				{
					if(sw)
						delete sw;
					if(pRebindData)
						delete pRebindData;
					return nullptr;
				}
			}
			else
			{
				delete ins; // in fact we don't need it, it will be reparsed the first time it is called
				            // Q: Couldn't we actually use the already parsed tree ?
				            // A: No: the tree must be reparsed in a new parser context
				            //    since we're keeping track of global and local variables...
				            //    The locals of this context are NOT the same as the locals
				            //    of the other context.
			}

			QString szCallbackName = szIdentifier;
			szCallbackName += " callback";

			QString szBlock(pClbkBegin, KVSP_curCharPointer - pClbkBegin);
			KviCommandFormatter::bufferFromBlock(szBlock);

			KviKvsScript * clbk = new KviKvsScript(szCallbackName, szBlock);

			if(pSecondPart)
			{
				cmd = new KviKvsTreeNodeModuleCallbackCommand(pIdentifier, szIdentifier, QString(pSecondPart, iSecondPartLen), dl, clbk);
			}
			else
			{
				KviKvsCoreCallbackCommandExecRoutine * r = KviKvsKernel::instance()->findCoreCallbackCommandExecRoutine(szIdentifier);
				if(r)
				{
					cmd = new KviKvsTreeNodeCoreCallbackCommand(pIdentifier, szIdentifier, dl, r, clbk);
				}
				else
				{
					error(KVSP_curCharPointer, __tr2qs_ctx("Unknown callback command \"%Q\"", "kvs"), &szIdentifier);
					if(sw)
						delete sw;
					if(pRebindData)
						delete pRebindData;
					delete dl;
					delete clbk;
					return nullptr;
				}
			}

			if(sw)
			{
				cmd->setSwitchList(sw);
				// cmd becomes child of the rebinding switch
				if(pRebindData)
					return new KviKvsTreeNodeRebindingSwitch(pRebindData->location(), pRebindData, cmd);
			}

			return cmd;
		}
	}

	// must be core simple command, module simple command or alias
	KviKvsTreeNodeDataList * pl = parseCommandParameterList();
	if(!pl)
	{
		if(sw)
			delete sw;
		if(pRebindData)
			delete pRebindData;
		return nullptr; // this MUST be an error
	}

	if(bHasNamespaceSoMustBeAlias)
	{
		// alias for sure, bind at runtime
		cmd = new KviKvsTreeNodeAliasSimpleCommand(pIdentifier, szIdentifier, pl);
	}
	else
	{
		if(pSecondPart)
		{
			cmd = new KviKvsTreeNodeModuleSimpleCommand(pIdentifier, szIdentifier, QString(pSecondPart, iSecondPartLen), pl);
		}
		else
		{
			KviKvsCoreSimpleCommandExecRoutine * r = KviKvsKernel::instance()->findCoreSimpleCommandExecRoutine(szIdentifier);
			if(r)
			{
				cmd = new KviKvsTreeNodeCoreSimpleCommand(pIdentifier, szIdentifier, pl, r);
			}
			else
			{
				// must be an alias in root namespace, bind at runtime
				cmd = new KviKvsTreeNodeAliasSimpleCommand(pIdentifier, szIdentifier, pl);
			}
		}
	}

	if(sw)
	{
		cmd->setSwitchList(sw);
		// cmd becomes child of the rebinding switch
		if(pRebindData)
			return new KviKvsTreeNodeRebindingSwitch(pRebindData->location(), pRebindData, cmd);
	}

	return cmd;
}
