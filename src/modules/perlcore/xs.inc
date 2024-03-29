/*
 * This file was generated automatically by ExtUtils::ParseXS version 3.51 from the
 * contents of KVIrc.xs. Do not edit this file, edit KVIrc.xs instead.
 *
 *    ANY CHANGES MADE HERE WILL BE LOST!
 *
 */

#line 1 "KVIrc.xs"
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

#line 35 "KVIrc.c"
#ifndef PERL_UNUSED_VAR
#  define PERL_UNUSED_VAR(var) if (0) var = var
#endif

#ifndef dVAR
#  define dVAR		dNOOP
#endif


/* This stuff is not part of the API! You have been warned. */
#ifndef PERL_VERSION_DECIMAL
#  define PERL_VERSION_DECIMAL(r,v,s) (r*1000000 + v*1000 + s)
#endif
#ifndef PERL_DECIMAL_VERSION
#  define PERL_DECIMAL_VERSION \
	  PERL_VERSION_DECIMAL(PERL_REVISION,PERL_VERSION,PERL_SUBVERSION)
#endif
#ifndef PERL_VERSION_GE
#  define PERL_VERSION_GE(r,v,s) \
	  (PERL_DECIMAL_VERSION >= PERL_VERSION_DECIMAL(r,v,s))
#endif
#ifndef PERL_VERSION_LE
#  define PERL_VERSION_LE(r,v,s) \
	  (PERL_DECIMAL_VERSION <= PERL_VERSION_DECIMAL(r,v,s))
#endif

/* XS_INTERNAL is the explicit static-linkage variant of the default
 * XS macro.
 *
 * XS_EXTERNAL is the same as XS_INTERNAL except it does not include
 * "STATIC", ie. it exports XSUB symbols. You probably don't want that
 * for anything but the BOOT XSUB.
 *
 * See XSUB.h in core!
 */


/* TODO: This might be compatible further back than 5.10.0. */
#if PERL_VERSION_GE(5, 10, 0) && PERL_VERSION_LE(5, 15, 1)
#  undef XS_EXTERNAL
#  undef XS_INTERNAL
#  if defined(__CYGWIN__) && defined(USE_DYNAMIC_LOADING)
#    define XS_EXTERNAL(name) __declspec(dllexport) XSPROTO(name)
#    define XS_INTERNAL(name) STATIC XSPROTO(name)
#  endif
#  if defined(__SYMBIAN32__)
#    define XS_EXTERNAL(name) EXPORT_C XSPROTO(name)
#    define XS_INTERNAL(name) EXPORT_C STATIC XSPROTO(name)
#  endif
#  ifndef XS_EXTERNAL
#    if defined(HASATTRIBUTE_UNUSED) && !defined(__cplusplus)
#      define XS_EXTERNAL(name) void name(pTHX_ CV* cv __attribute__unused__)
#      define XS_INTERNAL(name) STATIC void name(pTHX_ CV* cv __attribute__unused__)
#    else
#      ifdef __cplusplus
#        define XS_EXTERNAL(name) extern "C" XSPROTO(name)
#        define XS_INTERNAL(name) static XSPROTO(name)
#      else
#        define XS_EXTERNAL(name) XSPROTO(name)
#        define XS_INTERNAL(name) STATIC XSPROTO(name)
#      endif
#    endif
#  endif
#endif

/* perl >= 5.10.0 && perl <= 5.15.1 */


/* The XS_EXTERNAL macro is used for functions that must not be static
 * like the boot XSUB of a module. If perl didn't have an XS_EXTERNAL
 * macro defined, the best we can do is assume XS is the same.
 * Dito for XS_INTERNAL.
 */
#ifndef XS_EXTERNAL
#  define XS_EXTERNAL(name) XS(name)
#endif
#ifndef XS_INTERNAL
#  define XS_INTERNAL(name) XS(name)
#endif

/* Now, finally, after all this mess, we want an ExtUtils::ParseXS
 * internal macro that we're free to redefine for varying linkage due
 * to the EXPORT_XSUB_SYMBOLS XS keyword. This is internal, use
 * XS_EXTERNAL(name) or XS_INTERNAL(name) in your code if you need to!
 */

#undef XS_EUPXS
#if defined(PERL_EUPXS_ALWAYS_EXPORT)
#  define XS_EUPXS(name) XS_EXTERNAL(name)
#else
   /* default to internal */
#  define XS_EUPXS(name) XS_INTERNAL(name)
#endif

#ifndef PERL_ARGS_ASSERT_CROAK_XS_USAGE
#define PERL_ARGS_ASSERT_CROAK_XS_USAGE assert(cv); assert(params)

/* prototype to pass -Wmissing-prototypes */
STATIC void
S_croak_xs_usage(const CV *const cv, const char *const params);

STATIC void
S_croak_xs_usage(const CV *const cv, const char *const params)
{
    const GV *const gv = CvGV(cv);

    PERL_ARGS_ASSERT_CROAK_XS_USAGE;

    if (gv) {
        const char *const gvname = GvNAME(gv);
        const HV *const stash = GvSTASH(gv);
        const char *const hvname = stash ? HvNAME(stash) : NULL;

        if (hvname)
	    Perl_croak_nocontext("Usage: %s::%s(%s)", hvname, gvname, params);
        else
	    Perl_croak_nocontext("Usage: %s(%s)", gvname, params);
    } else {
        /* Pants. I don't think that it should be possible to get here. */
	Perl_croak_nocontext("Usage: CODE(0x%" UVxf ")(%s)", PTR2UV(cv), params);
    }
}
#undef  PERL_ARGS_ASSERT_CROAK_XS_USAGE

#define croak_xs_usage        S_croak_xs_usage

#endif

/* NOTE: the prototype of newXSproto() is different in versions of perls,
 * so we define a portable version of newXSproto()
 */
#ifdef newXS_flags
#define newXSproto_portable(name, c_impl, file, proto) newXS_flags(name, c_impl, file, proto, 0)
#else
#define newXSproto_portable(name, c_impl, file, proto) (PL_Sv=(SV*)newXS(name, c_impl, file), sv_setpv(PL_Sv, proto), (CV*)PL_Sv)
#endif /* !defined(newXS_flags) */

#if PERL_VERSION_LE(5, 21, 5)
#  define newXS_deffile(a,b) Perl_newXS(aTHX_ a,b,file)
#else
#  define newXS_deffile(a,b) Perl_newXS_deffile(aTHX_ a,b)
#endif

#line 179 "KVIrc.c"

XS_EUPXS(XS_KVIrc_echo); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_KVIrc_echo)
{
    dVAR; dXSARGS;
    if (items < 1 || items > 3)
       croak_xs_usage(cv,  "text, colorset = 0, windowid = 0");
    {
	char *	text = (char *)SvPV_nolen(ST(0))
;
	int	colorset;
	char *	windowid;

	if (items < 2)
	    colorset = 0;
	else {
	    colorset = (int)SvIV(ST(1))
;
	}

	if (items < 3)
	    windowid = 0;
	else {
	    windowid = (char *)SvPV_nolen(ST(2))
;
	}
#line 34 "KVIrc.xs"
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
#line 219 "KVIrc.c"
    }
    XSRETURN_EMPTY;
}


XS_EUPXS(XS_KVIrc_say); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_KVIrc_say)
{
    dVAR; dXSARGS;
    if (items < 1 || items > 2)
       croak_xs_usage(cv,  "text, windowid = 0");
    {
	char *	text = (char *)SvPV_nolen(ST(0))
;
	char *	windowid;

	if (items < 2)
	    windowid = 0;
	else {
	    windowid = (char *)SvPV_nolen(ST(1))
;
	}
#line 51 "KVIrc.xs"
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
#line 256 "KVIrc.c"
    }
    XSRETURN_EMPTY;
}


XS_EUPXS(XS_KVIrc_warning); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_KVIrc_warning)
{
    dVAR; dXSARGS;
    if (items != 1)
       croak_xs_usage(cv,  "text");
    {
	char *	text = (char *)SvPV_nolen(ST(0))
;
#line 68 "KVIrc.xs"
		if((!g_bExecuteQuiet) && g_pCurrentKvsContext)
			g_pCurrentKvsContext->warning(text);
#line 274 "KVIrc.c"
    }
    XSRETURN_EMPTY;
}


XS_EUPXS(XS_KVIrc_internalWarning); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_KVIrc_internalWarning)
{
    dVAR; dXSARGS;
    if (items != 1)
       croak_xs_usage(cv,  "text");
    {
	char *	text = (char *)SvPV_nolen(ST(0))
;
#line 74 "KVIrc.xs"
		if(!g_bExecuteQuiet)
			g_lWarningList.append(QString(text));
#line 292 "KVIrc.c"
    }
    XSRETURN_EMPTY;
}


XS_EUPXS(XS_KVIrc_getLocal); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_KVIrc_getLocal)
{
    dVAR; dXSARGS;
    if (items != 1)
       croak_xs_usage(cv,  "varname");
    {
	char *	varname = (char *)SvPV_nolen(ST(0))
;
	char *	RETVAL;
	dXSTARG;
#line 81 "KVIrc.xs"
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
#line 324 "KVIrc.c"
	sv_setpv(TARG, RETVAL);
	XSprePUSH;
	PUSHTARG;
    }
    XSRETURN(1);
}


XS_EUPXS(XS_KVIrc_setLocal); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_KVIrc_setLocal)
{
    dVAR; dXSARGS;
    if (items != 2)
       croak_xs_usage(cv,  "varname, value");
    {
	char *	varname = (char *)SvPV_nolen(ST(0))
;
	char *	value = (char *)SvPV_nolen(ST(1))
;
#line 102 "KVIrc.xs"
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
#line 355 "KVIrc.c"
    }
    XSRETURN_EMPTY;
}


XS_EUPXS(XS_KVIrc_getGlobal); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_KVIrc_getGlobal)
{
    dVAR; dXSARGS;
    if (items != 1)
       croak_xs_usage(cv,  "varname");
    {
	char *	varname = (char *)SvPV_nolen(ST(0))
;
	char *	RETVAL;
	dXSTARG;
#line 116 "KVIrc.xs"
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
#line 387 "KVIrc.c"
	sv_setpv(TARG, RETVAL);
	XSprePUSH;
	PUSHTARG;
    }
    XSRETURN(1);
}


XS_EUPXS(XS_KVIrc_setGlobal); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_KVIrc_setGlobal)
{
    dVAR; dXSARGS;
    if (items != 2)
       croak_xs_usage(cv,  "varname, value");
    {
	char *	varname = (char *)SvPV_nolen(ST(0))
;
	char *	value = (char *)SvPV_nolen(ST(1))
;
#line 137 "KVIrc.xs"
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
#line 418 "KVIrc.c"
    }
    XSRETURN_EMPTY;
}


XS_EUPXS(XS_KVIrc_eval); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_KVIrc_eval)
{
    dVAR; dXSARGS;
    if (items != 1)
       croak_xs_usage(cv,  "code");
    {
	char *	code = (char *)SvPV_nolen(ST(0))
;
	char *	RETVAL;
	dXSTARG;
#line 152 "KVIrc.xs"
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
#line 453 "KVIrc.c"
	sv_setpv(TARG, RETVAL);
	XSprePUSH;
	PUSHTARG;
    }
    XSRETURN(1);
}

#ifdef __cplusplus
extern "C" {
#endif
XS_EXTERNAL(boot_KVIrc); /* prototype to pass -Wmissing-prototypes */
XS_EXTERNAL(boot_KVIrc)
{
#if PERL_VERSION_LE(5, 21, 5)
    dVAR; dXSARGS;
#else
    dVAR; dXSBOOTARGSXSAPIVERCHK;
#endif
#if PERL_VERSION_LE(5, 8, 999) /* PERL_VERSION_LT is 5.33+ */
    char* file = __FILE__;
#else
    const char* file = __FILE__;
#endif

    PERL_UNUSED_VAR(file);

    PERL_UNUSED_VAR(cv); /* -W */
    PERL_UNUSED_VAR(items); /* -W */
#if PERL_VERSION_LE(5, 21, 5)
    XS_VERSION_BOOTCHECK;
#  ifdef XS_APIVERSION_BOOTCHECK
    XS_APIVERSION_BOOTCHECK;
#  endif
#endif

        (void)newXSproto_portable("KVIrc::echo", XS_KVIrc_echo, file, "$;$$");
        (void)newXSproto_portable("KVIrc::say", XS_KVIrc_say, file, "$;$");
        (void)newXSproto_portable("KVIrc::warning", XS_KVIrc_warning, file, "$");
        (void)newXSproto_portable("KVIrc::internalWarning", XS_KVIrc_internalWarning, file, "$");
        (void)newXSproto_portable("KVIrc::getLocal", XS_KVIrc_getLocal, file, "$");
        (void)newXSproto_portable("KVIrc::setLocal", XS_KVIrc_setLocal, file, "$$");
        (void)newXSproto_portable("KVIrc::getGlobal", XS_KVIrc_getGlobal, file, "$");
        (void)newXSproto_portable("KVIrc::setGlobal", XS_KVIrc_setGlobal, file, "$$");
        (void)newXSproto_portable("KVIrc::eval", XS_KVIrc_eval, file, "$");
#if PERL_VERSION_LE(5, 21, 5)
#  if PERL_VERSION_GE(5, 9, 0)
    if (PL_unitcheckav)
        call_list(PL_scopestack_ix, PL_unitcheckav);
#  endif
    XSRETURN_YES;
#else
    Perl_xs_boot_epilog(aTHX_ ax);
#endif
}

#ifdef __cplusplus
}
#endif