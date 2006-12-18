MODULE = KVIrc		PACKAGE = KVIrc
		
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
		KviStr hack;
		if(g_pCurrentKvsContext)
		{
			KviKvsVariant * pVar = g_pCurrentKvsContext->localVariables()->find(varname);
			if(pVar)
			{
				pVar->asString(tmp);
				hack = tmp;
				RETVAL = hack.ptr();
			} else RETVAL = "";
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
		KviStr hack;
		if(g_pCurrentKvsContext)
		{
			KviKvsVariant * pVar = g_pCurrentKvsContext->globalVariables()->find(varname);
			if(pVar)
			{
				pVar->asString(tmp);
				hack = tmp;
				RETVAL = hack.ptr();
			} else RETVAL = "";
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
			RETVAL = g_szLastReturnValue.ptr();
		} else {
			RETVAL = "";
		}
	OUTPUT:
		RETVAL

