//=============================================================================
//
//   File : KVIrc.xs
//   Creation date : Unknown by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2009 Szymon Stefanek (pragma at kvirc dot net)
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

MODULE = KVIrc		PACKAGE = KVIrc

PROTOTYPES: ENABLE

void echo(text,colorset = 0,windowid = 0)
	char * text
	int colorset
	char * windowid
	CODE:
		if(g_pCurrentKvsContext && text)
		{
			KviWindow * pWnd;
			if(windowid)
			{
				pWnd = g_pApp->findWindow(windowid);
				if(!pWnd)pWnd = g_pCurrentKvsContext->window();
			} else {
				pWnd = g_pCurrentKvsContext->window();
			}
			pWnd->outputNoFmt(colorset,QString::fromUtf8(text));
		}

void say(text,windowid = 0)
	char * text
	char * windowid
	CODE:
		if(g_pCurrentKvsContext && text)
		{
			KviWindow * pWnd;
			if(windowid)
			{
				pWnd = g_pApp->findWindow(windowid);
				if(!pWnd)pWnd = g_pCurrentKvsContext->window();
			} else {
				pWnd = g_pCurrentKvsContext->window();
			}
			QString tmp = QString::fromUtf8(text);
			KviUserInput::parse(tmp,pWnd);
		}

void warning(text)
	char * text
	CODE:
		if((!g_bExecuteQuiet) && g_pCurrentKvsContext)
			g_pCurrentKvsContext->warning(text);

void internalWarning(text)
	char * text
	CODE:
		if(!g_bExecuteQuiet)
			g_lWarningList.append(QString(text));


char * getLocal(varname)
	char * varname
	CODE:
		QString tmp;
		KviCString hack;
		if(g_pCurrentKvsContext)
		{
			KviKvsVariant * pVar = g_pCurrentKvsContext->localVariables()->find(varname);
			if(pVar)
			{
				pVar->asString(tmp);
				hack = tmp;
			} else {
				hack = "";
			}
			RETVAL = hack.ptr();
		}
	OUTPUT:
		RETVAL

void setLocal(varname,value)
	char * varname
	char * value
	CODE:
		if(g_pCurrentKvsContext)
		{
			if(value && *value)
			{
				KviKvsVariant * pVar = g_pCurrentKvsContext->localVariables()->get(varname);
				pVar->setString(value);
			} else {
				g_pCurrentKvsContext->localVariables()->unset(varname);
			}
		}

char * getGlobal(varname)
	char * varname
	CODE:
		QString tmp;
		KviCString hack;
		if(g_pCurrentKvsContext)
		{
			KviKvsVariant * pVar = g_pCurrentKvsContext->globalVariables()->find(varname);
			if(pVar)
			{
				pVar->asString(tmp);
				hack = tmp;
			} else {
				hack = "";
			}
			RETVAL = hack.ptr();
		}
	OUTPUT:
		RETVAL

void setGlobal(varname,value)
	char * varname
	char * value
	CODE:
		if(g_pCurrentKvsContext)
		{
			if(value && *value)
			{
				KviKvsVariant * pVar = g_pCurrentKvsContext->globalVariables()->get(varname);
				pVar->setString(value);
			} else {
				g_pCurrentKvsContext->localVariables()->unset(varname);
			}
		}


char * eval(code)
	char * code
	CODE:
		KviCString hack;
		if(g_pCurrentKvsContext && code)
		{
			KviKvsVariant ret;
			if(KviKvsScript::run(QString::fromUtf8(code),g_pCurrentKvsContext->window(),0,&ret))
			{
				QString tmp;
				ret.asString(tmp);
				g_szLastReturnValue = tmp;
			} else {
				g_szLastReturnValue = "";
			}
			hack = g_szLastReturnValue.ptr();
		} else {
			hack = "";
		}
		RETVAL = hack.ptr();
	OUTPUT:
		RETVAL

