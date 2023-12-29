#if 0
my $void = <<'SKIP';
#endif
/*
----------------------------------------------------------------------

    ppport.h -- Perl/Pollution/Portability Version 3.71

    Automatically created by Devel::PPPort running under perl 5.038000.

    Version 3.x, Copyright (c) 2004-2013, Marcus Holland-Moritz.

    Version 2.x, Copyright (C) 2001, Paul Marquess.

    Version 1.x, Copyright (C) 1999, Kenneth Albanowski.

    This program is free software; you can redistribute it and/or
    modify it under the same terms as Perl itself.

----------------------------------------------------------------------

SKIP
if (@ARGV && $ARGV[0] eq '--unstrip') {
  eval { require Devel::PPPort };
  $@ and die "Cannot require Devel::PPPort, please install.\n";
  if (eval $Devel::PPPort::VERSION < 3.71) {
    die "ppport.h was originally generated with Devel::PPPort 3.71.\n"
      . "Your Devel::PPPort is only version $Devel::PPPort::VERSION.\n"
      . "Please install a newer version, or --unstrip will not work.\n";
  }
  Devel::PPPort::WriteFile($0);
  exit 0;
}
print <<END;

Sorry, but this is a stripped version of $0.

To be able to use its original script and doc functionality,
please try to regenerate this file using:

  $^X $0 --unstrip

END
__DATA__*/
#ifndef _P_P_PORTABILITY_H_
#define _P_P_PORTABILITY_H_
#ifndef DPPP_NAMESPACE
#define DPPP_NAMESPACE DPPP_
#endif
#define DPPP_CAT2(x,y) CAT2(x,y)
#define DPPP_(name) DPPP_CAT2(DPPP_NAMESPACE, name)
#define D_PPP_RELEASE_DATE 1647561600
#if ! defined(PERL_REVISION) && ! defined(PERL_VERSION_MAJOR)
#if ! defined(__PATCHLEVEL_H_INCLUDED__) \
&& ! ( defined(PATCHLEVEL) && defined(SUBVERSION))
#define PERL_PATCHLEVEL_H_IMPLICIT
#include <patchlevel.h>
#endif
#if ! defined(PERL_VERSION) \
&& ! defined(PERL_VERSION_MAJOR) \
&& ( ! defined(SUBVERSION) || ! defined(PATCHLEVEL) )
#include <could_not_find_Perl_patchlevel.h>
#endif
#endif
#ifdef PERL_VERSION_MAJOR
#define D_PPP_MAJOR PERL_VERSION_MAJOR
#elif defined(PERL_REVISION)
#define D_PPP_MAJOR PERL_REVISION
#else
#define D_PPP_MAJOR 5
#endif
#ifdef PERL_VERSION_MINOR
#define D_PPP_MINOR PERL_VERSION_MINOR
#elif defined(PERL_VERSION)
#define D_PPP_MINOR PERL_VERSION
#elif defined(PATCHLEVEL)
#define D_PPP_MINOR PATCHLEVEL
#define PERL_VERSION PATCHLEVEL
#else
#error Could not find a source for PERL_VERSION_MINOR
#endif
#ifdef PERL_VERSION_PATCH
#define D_PPP_PATCH PERL_VERSION_PATCH
#elif defined(PERL_SUBVERSION)
#define D_PPP_PATCH PERL_SUBVERSION
#elif defined(SUBVERSION)
#define D_PPP_PATCH SUBVERSION
#define PERL_SUBVERSION SUBVERSION
#else
#error Could not find a source for PERL_VERSION_PATCH
#endif
#if D_PPP_MAJOR < 5 || D_PPP_MAJOR == 6
#error Devel::PPPort works only on Perl 5, Perl 7, ...
#elif D_PPP_MAJOR != 5
#undef PERL_REVISION
#undef PERL_VERSION
#undef PERL_SUBVERSION
#define D_PPP_REVISION 5
#define D_PPP_VERSION 201
#define D_PPP_SUBVERSION 201
#if (defined(__clang__)  \
&& ( (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) \
|| defined(_STDC_C99) \
|| defined(__c99)))
#define D_PPP_STRINGIFY(x) #x
#define D_PPP_deprecate(xyz) _Pragma(D_PPP_STRINGIFY(GCC warning(D_PPP_STRINGIFY(xyz) " is deprecated")))
#define PERL_REVISION (D_PPP_REVISION D_PPP_deprecate(PERL_REVISION))
#define PERL_VERSION (D_PPP_REVISION D_PPP_deprecate(PERL_VERSION))
#define PERL_SUBVERSION (D_PPP_SUBVERSION D_PPP_deprecate(PERL_SUBVERSION))
#else
#define PERL_REVISION D_PPP_REVISION
#define PERL_VERSION D_PPP_REVISION
#define PERL_SUBVERSION D_PPP_SUBVERSION
#endif
#endif
#define D_PPP_DEC2BCD(dec) ((((dec)/100)<<8)|((((dec)%100)/10)<<4)|((dec)%10))
#define D_PPP_JNP_TO_BCD(j,n,p) ((D_PPP_DEC2BCD(j)<<24)|(D_PPP_DEC2BCD(n)<<12)|D_PPP_DEC2BCD(p))
#define PERL_BCDVERSION D_PPP_JNP_TO_BCD(D_PPP_MAJOR, \
D_PPP_MINOR, \
D_PPP_PATCH)
#undef PERL_VERSION_EQ
#undef PERL_VERSION_NE
#undef PERL_VERSION_LT
#undef PERL_VERSION_GE
#undef PERL_VERSION_LE
#undef PERL_VERSION_GT
#ifndef PERL_VERSION_EQ
#define PERL_VERSION_EQ(j,n,p) \
(((p) == '*') ? ( (j) == D_PPP_VERSION_MAJOR \
&& (n) == D_PPP_VERSION_MINOR) \
: (PERL_BCDVERSION == D_PPP_JNP_TO_BCD(j,n,p)))
#endif
#ifndef PERL_VERSION_NE
#define PERL_VERSION_NE(j,n,p) (! PERL_VERSION_EQ(j,n,p))
#endif
#ifndef PERL_VERSION_LT
#define PERL_VERSION_LT(j,n,p)  \
(PERL_BCDVERSION < D_PPP_JNP_TO_BCD( (j), \
(n), \
(((p) == '*') ? 0 : (p))))
#endif
#ifndef PERL_VERSION_GE
#define PERL_VERSION_GE(j,n,p) (! PERL_VERSION_LT(j,n,p))
#endif
#ifndef PERL_VERSION_LE
#define PERL_VERSION_LE(j,n,p)  \
(PERL_BCDVERSION < D_PPP_JNP_TO_BCD( (j), \
(((p) == '*') ? ((n)+1) : (n)), \
(((p) == '*') ? 0 : (p))))
#endif
#ifndef PERL_VERSION_GT
#define PERL_VERSION_GT(j,n,p) (! PERL_VERSION_LE(j,n,p))
#endif
#ifndef dTHR
#define dTHR dNOOP
#endif
#ifndef dTHX
#define dTHX dNOOP
#endif
#ifndef dTHXa
#define dTHXa(x) dNOOP
#endif
#ifndef pTHX
#define pTHX void
#endif
#ifndef pTHX_
#define pTHX_
#endif
#ifndef aTHX
#define aTHX
#endif
#ifndef aTHX_
#define aTHX_
#endif
#if (PERL_BCDVERSION < 0x5006000)
#ifdef USE_THREADS
#define aTHXR thr
#define aTHXR_ thr,
#else
#define aTHXR
#define aTHXR_
#endif
#define dTHXR dTHR
#else
#define aTHXR aTHX
#define aTHXR_ aTHX_
#define dTHXR dTHX
#endif
#ifndef dTHXoa
#define dTHXoa(x) dTHXa(x)
#endif
#ifdef I_LIMITS
#include <limits.h>
#endif
#ifndef PERL_UCHAR_MIN
#define PERL_UCHAR_MIN ((unsigned char)0)
#endif
#ifndef PERL_UCHAR_MAX
#ifdef UCHAR_MAX
#define PERL_UCHAR_MAX ((unsigned char)UCHAR_MAX)
#else
#ifdef MAXUCHAR
#define PERL_UCHAR_MAX ((unsigned char)MAXUCHAR)
#else
#define PERL_UCHAR_MAX ((unsigned char)~(unsigned)0)
#endif
#endif
#endif
#ifndef PERL_USHORT_MIN
#define PERL_USHORT_MIN ((unsigned short)0)
#endif
#ifndef PERL_USHORT_MAX
#ifdef USHORT_MAX
#define PERL_USHORT_MAX ((unsigned short)USHORT_MAX)
#else
#ifdef MAXUSHORT
#define PERL_USHORT_MAX ((unsigned short)MAXUSHORT)
#else
#ifdef USHRT_MAX
#define PERL_USHORT_MAX ((unsigned short)USHRT_MAX)
#else
#define PERL_USHORT_MAX ((unsigned short)~(unsigned)0)
#endif
#endif
#endif
#endif
#ifndef PERL_SHORT_MAX
#ifdef SHORT_MAX
#define PERL_SHORT_MAX ((short)SHORT_MAX)
#else
#ifdef MAXSHORT
#define PERL_SHORT_MAX ((short)MAXSHORT)
#else
#ifdef SHRT_MAX
#define PERL_SHORT_MAX ((short)SHRT_MAX)
#else
#define PERL_SHORT_MAX ((short) (PERL_USHORT_MAX >> 1))
#endif
#endif
#endif
#endif
#ifndef PERL_SHORT_MIN
#ifdef SHORT_MIN
#define PERL_SHORT_MIN ((short)SHORT_MIN)
#else
#ifdef MINSHORT
#define PERL_SHORT_MIN ((short)MINSHORT)
#else
#ifdef SHRT_MIN
#define PERL_SHORT_MIN ((short)SHRT_MIN)
#else
#define PERL_SHORT_MIN (-PERL_SHORT_MAX - ((3 & -1) == 3))
#endif
#endif
#endif
#endif
#ifndef PERL_UINT_MAX
#ifdef UINT_MAX
#define PERL_UINT_MAX ((unsigned int)UINT_MAX)
#else
#ifdef MAXUINT
#define PERL_UINT_MAX ((unsigned int)MAXUINT)
#else
#define PERL_UINT_MAX (~(unsigned int)0)
#endif
#endif
#endif
#ifndef PERL_UINT_MIN
#define PERL_UINT_MIN ((unsigned int)0)
#endif
#ifndef PERL_INT_MAX
#ifdef INT_MAX
#define PERL_INT_MAX ((int)INT_MAX)
#else
#ifdef MAXINT
#define PERL_INT_MAX ((int)MAXINT)
#else
#define PERL_INT_MAX ((int)(PERL_UINT_MAX >> 1))
#endif
#endif
#endif
#ifndef PERL_INT_MIN
#ifdef INT_MIN
#define PERL_INT_MIN ((int)INT_MIN)
#else
#ifdef MININT
#define PERL_INT_MIN ((int)MININT)
#else
#define PERL_INT_MIN (-PERL_INT_MAX - ((3 & -1) == 3))
#endif
#endif
#endif
#ifndef PERL_ULONG_MAX
#ifdef ULONG_MAX
#define PERL_ULONG_MAX ((unsigned long)ULONG_MAX)
#else
#ifdef MAXULONG
#define PERL_ULONG_MAX ((unsigned long)MAXULONG)
#else
#define PERL_ULONG_MAX (~(unsigned long)0)
#endif
#endif
#endif
#ifndef PERL_ULONG_MIN
#define PERL_ULONG_MIN ((unsigned long)0L)
#endif
#ifndef PERL_LONG_MAX
#ifdef LONG_MAX
#define PERL_LONG_MAX ((long)LONG_MAX)
#else
#ifdef MAXLONG
#define PERL_LONG_MAX ((long)MAXLONG)
#else
#define PERL_LONG_MAX ((long) (PERL_ULONG_MAX >> 1))
#endif
#endif
#endif
#ifndef PERL_LONG_MIN
#ifdef LONG_MIN
#define PERL_LONG_MIN ((long)LONG_MIN)
#else
#ifdef MINLONG
#define PERL_LONG_MIN ((long)MINLONG)
#else
#define PERL_LONG_MIN (-PERL_LONG_MAX - ((3 & -1) == 3))
#endif
#endif
#endif
#if defined(HAS_QUAD) && (defined(convex) || defined(uts))
#ifndef PERL_UQUAD_MAX
#ifdef ULONGLONG_MAX
#define PERL_UQUAD_MAX ((unsigned long long)ULONGLONG_MAX)
#else
#ifdef MAXULONGLONG
#define PERL_UQUAD_MAX ((unsigned long long)MAXULONGLONG)
#else
#define PERL_UQUAD_MAX (~(unsigned long long)0)
#endif
#endif
#endif
#ifndef PERL_UQUAD_MIN
#define PERL_UQUAD_MIN ((unsigned long long)0L)
#endif
#ifndef PERL_QUAD_MAX
#ifdef LONGLONG_MAX
#define PERL_QUAD_MAX ((long long)LONGLONG_MAX)
#else
#ifdef MAXLONGLONG
#define PERL_QUAD_MAX ((long long)MAXLONGLONG)
#else
#define PERL_QUAD_MAX ((long long) (PERL_UQUAD_MAX >> 1))
#endif
#endif
#endif
#ifndef PERL_QUAD_MIN
#ifdef LONGLONG_MIN
#define PERL_QUAD_MIN ((long long)LONGLONG_MIN)
#else
#ifdef MINLONGLONG
#define PERL_QUAD_MIN ((long long)MINLONGLONG)
#else
#define PERL_QUAD_MIN (-PERL_QUAD_MAX - ((3 & -1) == 3))
#endif
#endif
#endif
#endif
#ifdef HAS_QUAD
#ifdef cray
#ifndef IVTYPE
#define IVTYPE int
#endif
#ifndef IV_MIN
#define IV_MIN PERL_INT_MIN
#endif
#ifndef IV_MAX
#define IV_MAX PERL_INT_MAX
#endif
#ifndef UV_MIN
#define UV_MIN PERL_UINT_MIN
#endif
#ifndef UV_MAX
#define UV_MAX PERL_UINT_MAX
#endif
#ifdef INTSIZE
#ifndef IVSIZE
#define IVSIZE INTSIZE
#endif
#endif
#else
#if defined(convex) || defined(uts)
#ifndef IVTYPE
#define IVTYPE long long
#endif
#ifndef IV_MIN
#define IV_MIN PERL_QUAD_MIN
#endif
#ifndef IV_MAX
#define IV_MAX PERL_QUAD_MAX
#endif
#ifndef UV_MIN
#define UV_MIN PERL_UQUAD_MIN
#endif
#ifndef UV_MAX
#define UV_MAX PERL_UQUAD_MAX
#endif
#ifdef LONGLONGSIZE
#ifndef IVSIZE
#define IVSIZE LONGLONGSIZE
#endif
#endif
#else
#ifndef IVTYPE
#define IVTYPE long
#endif
#ifndef IV_MIN
#define IV_MIN PERL_LONG_MIN
#endif
#ifndef IV_MAX
#define IV_MAX PERL_LONG_MAX
#endif
#ifndef UV_MIN
#define UV_MIN PERL_ULONG_MIN
#endif
#ifndef UV_MAX
#define UV_MAX PERL_ULONG_MAX
#endif
#ifdef LONGSIZE
#ifndef IVSIZE
#define IVSIZE LONGSIZE
#endif
#endif
#endif
#endif
#ifndef IVSIZE
#define IVSIZE 8
#endif
#ifndef LONGSIZE
#define LONGSIZE 8
#endif
#ifndef PERL_QUAD_MIN
#define PERL_QUAD_MIN IV_MIN
#endif
#ifndef PERL_QUAD_MAX
#define PERL_QUAD_MAX IV_MAX
#endif
#ifndef PERL_UQUAD_MIN
#define PERL_UQUAD_MIN UV_MIN
#endif
#ifndef PERL_UQUAD_MAX
#define PERL_UQUAD_MAX UV_MAX
#endif
#else
#ifndef IVTYPE
#define IVTYPE long
#endif
#ifndef LONGSIZE
#define LONGSIZE 4
#endif
#ifndef IV_MIN
#define IV_MIN PERL_LONG_MIN
#endif
#ifndef IV_MAX
#define IV_MAX PERL_LONG_MAX
#endif
#ifndef UV_MIN
#define UV_MIN PERL_ULONG_MIN
#endif
#ifndef UV_MAX
#define UV_MAX PERL_ULONG_MAX
#endif
#endif
#ifndef IVSIZE
#ifdef LONGSIZE
#define IVSIZE LONGSIZE
#else
#define IVSIZE 4
#endif
#endif
#ifndef UVTYPE
#define UVTYPE unsigned IVTYPE
#endif
#ifndef UVSIZE
#define UVSIZE IVSIZE
#endif
#ifndef PERL_SIGNALS_UNSAFE_FLAG
#define PERL_SIGNALS_UNSAFE_FLAG 0x0001
#if (PERL_BCDVERSION < 0x5008000)
#define D_PPP_PERL_SIGNALS_INIT PERL_SIGNALS_UNSAFE_FLAG
#else
#define D_PPP_PERL_SIGNALS_INIT 0
#endif
#if defined(NEED_PL_signals)
static U32 DPPP_(my_PL_signals) = D_PPP_PERL_SIGNALS_INIT;
#elif defined(NEED_PL_signals_GLOBAL)
U32 DPPP_(my_PL_signals) = D_PPP_PERL_SIGNALS_INIT;
#else
extern U32 DPPP_(my_PL_signals);
#endif
#define PL_signals DPPP_(my_PL_signals)
#endif
#if (PERL_BCDVERSION <= 0x5005005)
#define PL_ppaddr ppaddr
#define PL_no_modify no_modify
#endif
#if (PERL_BCDVERSION <= 0x5004005)
#define PL_DBsignal DBsignal
#define PL_DBsingle DBsingle
#define PL_DBsub DBsub
#define PL_DBtrace DBtrace
#define PL_Sv Sv
#define PL_Xpv Xpv
#define PL_bufend bufend
#define PL_bufptr bufptr
#define PL_compiling compiling
#define PL_copline copline
#define PL_curcop curcop
#define PL_curstash curstash
#define PL_debstash debstash
#define PL_defgv defgv
#define PL_diehook diehook
#define PL_dirty dirty
#define PL_dowarn dowarn
#define PL_errgv errgv
#define PL_error_count error_count
#define PL_expect expect
#define PL_hexdigit hexdigit
#define PL_hints hints
#define PL_in_my in_my
#define PL_laststatval laststatval
#define PL_lex_state lex_state
#define PL_lex_stuff lex_stuff
#define PL_linestr linestr
#define PL_na na
#define PL_perl_destruct_level perl_destruct_level
#define PL_perldb perldb
#define PL_rsfp_filters rsfp_filters
#define PL_rsfp rsfp
#define PL_stack_base stack_base
#define PL_stack_sp stack_sp
#define PL_statcache statcache
#define PL_stdingv stdingv
#define PL_sv_arenaroot sv_arenaroot
#define PL_sv_no sv_no
#define PL_sv_undef sv_undef
#define PL_sv_yes sv_yes
#define PL_tainted tainted
#define PL_tainting tainting
#define PL_tokenbuf tokenbuf
#define PL_mess_sv mess_sv
#endif
#if (PERL_BCDVERSION >= 0x5009005)
#ifdef DPPP_PL_parser_NO_DUMMY
#define D_PPP_my_PL_parser_var(var) ((PL_parser ? PL_parser : \
(croak("panic: PL_parser == NULL in %s:%d", \
__FILE__, __LINE__), (yy_parser *) NULL))->var)
#else
#ifdef DPPP_PL_parser_NO_DUMMY_WARNING
#define D_PPP_parser_dummy_warning(var)
#else
#define D_PPP_parser_dummy_warning(var) \
warn("warning: dummy PL_" #var " used in %s:%d", __FILE__, __LINE__),
#endif
#define D_PPP_my_PL_parser_var(var) ((PL_parser ? PL_parser : \
(D_PPP_parser_dummy_warning(var) &DPPP_(dummy_PL_parser)))->var)
#if defined(NEED_PL_parser)
static yy_parser DPPP_(dummy_PL_parser);
#elif defined(NEED_PL_parser_GLOBAL)
yy_parser DPPP_(dummy_PL_parser);
#else
extern yy_parser DPPP_(dummy_PL_parser);
#endif
#endif
#define PL_expect D_PPP_my_PL_parser_var(expect)
#define PL_copline D_PPP_my_PL_parser_var(copline)
#define PL_rsfp D_PPP_my_PL_parser_var(rsfp)
#define PL_rsfp_filters D_PPP_my_PL_parser_var(rsfp_filters)
#define PL_linestr D_PPP_my_PL_parser_var(linestr)
#define PL_bufptr D_PPP_my_PL_parser_var(bufptr)
#define PL_bufend D_PPP_my_PL_parser_var(bufend)
#define PL_lex_state D_PPP_my_PL_parser_var(lex_state)
#define PL_lex_stuff D_PPP_my_PL_parser_var(lex_stuff)
#define PL_tokenbuf D_PPP_my_PL_parser_var(tokenbuf)
#define PL_in_my D_PPP_my_PL_parser_var(in_my)
#define PL_in_my_stash D_PPP_my_PL_parser_var(in_my_stash)
#define PL_error_count D_PPP_my_PL_parser_var(error_count)
#else
#define PL_parser ((void *) 1)
#endif
#if (PERL_BCDVERSION <= 0x5003022)
#undef start_subparse
#if (PERL_BCDVERSION < 0x5003022)
#ifndef start_subparse
#define start_subparse(a, b) Perl_start_subparse()
#endif
#else
#ifndef start_subparse
#define start_subparse(a, b) Perl_start_subparse(b)
#endif
#endif
#if (PERL_BCDVERSION < 0x5003007)
foo
#endif
#endif
#if (PERL_BCDVERSION < 0x5004063) && (PERL_BCDVERSION != 0x5004005)
#define NEED_newCONSTSUB
#if defined(NEED_newCONSTSUB)
static CV * DPPP_(my_newCONSTSUB)(HV * stash, const char * name, SV * sv);
static
#else
extern CV * DPPP_(my_newCONSTSUB)(HV * stash, const char * name, SV * sv);
#endif
#if defined(NEED_newCONSTSUB) || defined(NEED_newCONSTSUB_GLOBAL)
#ifdef newCONSTSUB
#undef newCONSTSUB
#endif
#define newCONSTSUB(a,b,c) DPPP_(my_newCONSTSUB)(aTHX_ a,b,c)
#define Perl_newCONSTSUB DPPP_(my_newCONSTSUB)
#define D_PPP_PL_copline PL_copline
CV *
DPPP_(my_newCONSTSUB)(HV *stash, const char *name, SV *sv)
{
CV *cv;
U32 oldhints = PL_hints;
HV *old_cop_stash = PL_curcop->cop_stash;
HV *old_curstash = PL_curstash;
line_t oldline = PL_curcop->cop_line;
PL_curcop->cop_line = D_PPP_PL_copline;
PL_hints &= ~HINT_BLOCK_SCOPE;
if (stash)
PL_curstash = PL_curcop->cop_stash = stash;
cv = newSUB(
start_subparse(FALSE, 0),
newSVOP(OP_CONST, 0, newSVpv((char *) name, 0)),
newSVOP(OP_CONST, 0, &PL_sv_no),
newSTATEOP(0, Nullch, newSVOP(OP_CONST, 0, sv))
);
PL_hints = oldhints;
PL_curcop->cop_stash = old_cop_stash;
PL_curstash = old_curstash;
PL_curcop->cop_line = oldline;
return cv;
}
#endif
#endif
#ifndef PERL_MAGIC_sv
#define PERL_MAGIC_sv '\0'
#endif
#ifndef PERL_MAGIC_overload
#define PERL_MAGIC_overload 'A'
#endif
#ifndef PERL_MAGIC_overload_elem
#define PERL_MAGIC_overload_elem 'a'
#endif
#ifndef PERL_MAGIC_overload_table
#define PERL_MAGIC_overload_table 'c'
#endif
#ifndef PERL_MAGIC_bm
#define PERL_MAGIC_bm 'B'
#endif
#ifndef PERL_MAGIC_regdata
#define PERL_MAGIC_regdata 'D'
#endif
#ifndef PERL_MAGIC_regdatum
#define PERL_MAGIC_regdatum 'd'
#endif
#ifndef PERL_MAGIC_env
#define PERL_MAGIC_env 'E'
#endif
#ifndef PERL_MAGIC_envelem
#define PERL_MAGIC_envelem 'e'
#endif
#ifndef PERL_MAGIC_fm
#define PERL_MAGIC_fm 'f'
#endif
#ifndef PERL_MAGIC_regex_global
#define PERL_MAGIC_regex_global 'g'
#endif
#ifndef PERL_MAGIC_isa
#define PERL_MAGIC_isa 'I'
#endif
#ifndef PERL_MAGIC_isaelem
#define PERL_MAGIC_isaelem 'i'
#endif
#ifndef PERL_MAGIC_nkeys
#define PERL_MAGIC_nkeys 'k'
#endif
#ifndef PERL_MAGIC_dbfile
#define PERL_MAGIC_dbfile 'L'
#endif
#ifndef PERL_MAGIC_dbline
#define PERL_MAGIC_dbline 'l'
#endif
#ifndef PERL_MAGIC_mutex
#define PERL_MAGIC_mutex 'm'
#endif
#ifndef PERL_MAGIC_shared
#define PERL_MAGIC_shared 'N'
#endif
#ifndef PERL_MAGIC_shared_scalar
#define PERL_MAGIC_shared_scalar 'n'
#endif
#ifndef PERL_MAGIC_collxfrm
#define PERL_MAGIC_collxfrm 'o'
#endif
#ifndef PERL_MAGIC_tied
#define PERL_MAGIC_tied 'P'
#endif
#ifndef PERL_MAGIC_tiedelem
#define PERL_MAGIC_tiedelem 'p'
#endif
#ifndef PERL_MAGIC_tiedscalar
#define PERL_MAGIC_tiedscalar 'q'
#endif
#ifndef PERL_MAGIC_qr
#define PERL_MAGIC_qr 'r'
#endif
#ifndef PERL_MAGIC_sig
#define PERL_MAGIC_sig 'S'
#endif
#ifndef PERL_MAGIC_sigelem
#define PERL_MAGIC_sigelem 's'
#endif
#ifndef PERL_MAGIC_taint
#define PERL_MAGIC_taint 't'
#endif
#ifndef PERL_MAGIC_uvar
#define PERL_MAGIC_uvar 'U'
#endif
#ifndef PERL_MAGIC_uvar_elem
#define PERL_MAGIC_uvar_elem 'u'
#endif
#ifndef PERL_MAGIC_vstring
#define PERL_MAGIC_vstring 'V'
#endif
#ifndef PERL_MAGIC_vec
#define PERL_MAGIC_vec 'v'
#endif
#ifndef PERL_MAGIC_utf8
#define PERL_MAGIC_utf8 'w'
#endif
#ifndef PERL_MAGIC_substr
#define PERL_MAGIC_substr 'x'
#endif
#ifndef PERL_MAGIC_defelem
#define PERL_MAGIC_defelem 'y'
#endif
#ifndef PERL_MAGIC_glob
#define PERL_MAGIC_glob '*'
#endif
#ifndef PERL_MAGIC_arylen
#define PERL_MAGIC_arylen '#'
#endif
#ifndef PERL_MAGIC_pos
#define PERL_MAGIC_pos '.'
#endif
#ifndef PERL_MAGIC_backref
#define PERL_MAGIC_backref '<'
#endif
#ifndef PERL_MAGIC_ext
#define PERL_MAGIC_ext '~'
#endif
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#ifndef PERL_STATIC_INLINE
#define PERL_STATIC_INLINE static inline
#endif
#else
#ifndef PERL_STATIC_INLINE
#define PERL_STATIC_INLINE static
#endif
#endif
#ifndef cBOOL
#define cBOOL(cbool) ((cbool) ? (bool)1 : (bool)0)
#endif
#ifndef OpHAS_SIBLING
#define OpHAS_SIBLING(o) (cBOOL((o)->op_sibling))
#endif
#ifndef OpSIBLING
#define OpSIBLING(o) (0 + (o)->op_sibling)
#endif
#ifndef OpMORESIB_set
#define OpMORESIB_set(o, sib) ((o)->op_sibling = (sib))
#endif
#ifndef OpLASTSIB_set
#define OpLASTSIB_set(o, parent) ((o)->op_sibling = NULL)
#endif
#ifndef OpMAYBESIB_set
#define OpMAYBESIB_set(o, sib, parent) ((o)->op_sibling = (sib))
#endif
#ifndef HEf_SVKEY
#define HEf_SVKEY -2
#endif
#if defined(DEBUGGING) && !defined(__COVERITY__)
#ifndef __ASSERT_
#define __ASSERT_(statement) assert(statement),
#endif
#else
#ifndef __ASSERT_
#define __ASSERT_(statement)
#endif
#endif
#ifndef __has_builtin
#define __has_builtin(x) 0
#endif
#if __has_builtin(__builtin_unreachable)
#define D_PPP_HAS_BUILTIN_UNREACHABLE
#elif (defined(__GNUC__) && ( __GNUC__ > 4 \
|| __GNUC__ == 4 && __GNUC_MINOR__ >= 5))
#define D_PPP_HAS_BUILTIN_UNREACHABLE
#endif
#ifndef ASSUME
#ifdef DEBUGGING
#define ASSUME(x) assert(x)
#elif defined(_MSC_VER)
#define ASSUME(x) __assume(x)
#elif defined(__ARMCC_VERSION)
#define ASSUME(x) __promise(x)
#elif defined(D_PPP_HAS_BUILTIN_UNREACHABLE)
#define ASSUME(x) ((x) ? (void) 0 : __builtin_unreachable())
#else
#define ASSUME(x) assert(x)
#endif
#endif
#ifndef NOT_REACHED
#ifdef D_PPP_HAS_BUILTIN_UNREACHABLE
#define NOT_REACHED \
STMT_START { \
ASSUME(!"UNREACHABLE"); __builtin_unreachable(); \
} STMT_END
#elif ! defined(__GNUC__) && (defined(__sun) || defined(__hpux))
#define NOT_REACHED
#else
#define NOT_REACHED ASSUME(!"UNREACHABLE")
#endif
#endif
#ifndef WIDEST_UTYPE
#ifdef QUADKIND
#ifdef U64TYPE
#define WIDEST_UTYPE U64TYPE
#else
#define WIDEST_UTYPE unsigned Quad_t
#endif
#else
#define WIDEST_UTYPE U32
#endif
#endif
#ifndef withinCOUNT
#define withinCOUNT(c, l, n) \
(((WIDEST_UTYPE) (((c)) - ((l) | 0))) <= (((WIDEST_UTYPE) ((n) | 0))))
#endif
#ifndef inRANGE
#define inRANGE(c, l, u) \
( (sizeof(c) == sizeof(U8)) ? withinCOUNT(((U8) (c)), (l), ((u) - (l))) \
: (sizeof(c) == sizeof(U32)) ? withinCOUNT(((U32) (c)), (l), ((u) - (l))) \
: (withinCOUNT(((WIDEST_UTYPE) (c)), (l), ((u) - (l)))))
#endif
#undef FITS_IN_8_BITS
#ifndef FITS_IN_8_BITS
#define FITS_IN_8_BITS(c) ( (sizeof(c) == 1) \
|| !(((WIDEST_UTYPE)((c) | 0)) & ~0xFF))
#endif
#define D_PPP_IS_GENERIC_UTF8_SAFE(s, e, macro) \
(((e) - (s)) <= 0 \
? 0 \
: UTF8_IS_INVARIANT((s)[0]) \
? is ## macro ## _L1((s)[0]) \
: (((e) - (s)) < UTF8SKIP(s)) \
? 0 \
: UTF8_IS_DOWNGRADEABLE_START((s)[0]) \
\
? is ## macro ## _L1((WIDEST_UTYPE) LATIN1_TO_NATIVE( \
UTF8_ACCUMULATE(NATIVE_UTF8_TO_I8((s)[0]) \
& UTF_START_MASK(2), \
(s)[1]))) \
: is ## macro ## _utf8(s))
#define D_PPP_IS_GENERIC_LC_UTF8_SAFE(s, e, macro) \
(((e) - (s)) <= 0 \
? 0 \
: UTF8_IS_INVARIANT((s)[0]) \
? is ## macro ## _LC((s)[0]) \
: (((e) - (s)) < UTF8SKIP(s)) \
? 0 \
: UTF8_IS_DOWNGRADEABLE_START((s)[0]) \
\
? is ## macro ## _LC((WIDEST_UTYPE) LATIN1_TO_NATIVE( \
UTF8_ACCUMULATE(NATIVE_UTF8_TO_I8((s)[0]) \
& UTF_START_MASK(2), \
(s)[1]))) \
: is ## macro ## _utf8(s))
#define D_PPP_IS_GENERIC_LC_UTF8_SAFE_BROKEN(s, e, macro) \
(((e) - (s)) <= 0 \
? 0 \
: UTF8_IS_INVARIANT((s)[0]) \
? is ## macro ## _LC((s)[0]) \
: (((e) - (s)) < UTF8SKIP(s)) \
? 0 \
: UTF8_IS_DOWNGRADEABLE_START((s)[0]) \
\
? is ## macro ## _LC((WIDEST_UTYPE) LATIN1_TO_NATIVE( \
UTF8_ACCUMULATE(NATIVE_UTF8_TO_I8((s)[0]) \
& UTF_START_MASK(2), \
(s)[1]))) \
: is ## macro ## _utf8_safe(s, e))
#ifndef SvRX
#define SvRX(rv) (SvROK((rv)) ? (SvMAGICAL(SvRV((rv))) ? (mg_find(SvRV((rv)), PERL_MAGIC_qr) ? mg_find(SvRV((rv)), PERL_MAGIC_qr)->mg_obj : NULL) : NULL) : NULL)
#endif
#ifndef SvRXOK
#define SvRXOK(sv) (!!SvRX(sv))
#endif
#ifndef PERL_UNUSED_DECL
#ifdef HASATTRIBUTE
#if (defined(__GNUC__) && defined(__cplusplus)) || defined(__INTEL_COMPILER)
#define PERL_UNUSED_DECL
#else
#define PERL_UNUSED_DECL __attribute__((unused))
#endif
#else
#define PERL_UNUSED_DECL
#endif
#endif
#ifndef PERL_UNUSED_ARG
#if defined(lint) && defined(S_SPLINT_S)
#include <note.h>
#define PERL_UNUSED_ARG(x) NOTE(ARGUNUSED(x))
#else
#define PERL_UNUSED_ARG(x) ((void)x)
#endif
#endif
#ifndef PERL_UNUSED_VAR
#define PERL_UNUSED_VAR(x) ((void)x)
#endif
#ifndef PERL_UNUSED_CONTEXT
#ifdef USE_ITHREADS
#define PERL_UNUSED_CONTEXT PERL_UNUSED_ARG(my_perl)
#else
#define PERL_UNUSED_CONTEXT
#endif
#endif
#ifndef PERL_UNUSED_RESULT
#if defined(__GNUC__) && defined(HASATTRIBUTE_WARN_UNUSED_RESULT)
#define PERL_UNUSED_RESULT(v) STMT_START { __typeof__(v) z = (v); (void)sizeof(z); } STMT_END
#else
#define PERL_UNUSED_RESULT(v) ((void)(v))
#endif
#endif
#ifndef NOOP
#define NOOP (void)0
#endif
#if (PERL_BCDVERSION < 0x5006001) && (PERL_BCDVERSION < 0x5027007)
#undef dNOOP
#ifndef dNOOP
#define dNOOP struct Perl___notused_struct
#endif
#endif
#ifndef NVTYPE
#if defined(USE_LONG_DOUBLE) && defined(HAS_LONG_DOUBLE)
#define NVTYPE long double
#else
#define NVTYPE double
#endif
typedef NVTYPE NV;
#endif
#ifndef INT2PTR
#if (IVSIZE == PTRSIZE) && (UVSIZE == PTRSIZE)
#define PTRV UV
#define INT2PTR(any,d) (any)(d)
#else
#if PTRSIZE == LONGSIZE
#define PTRV unsigned long
#else
#define PTRV unsigned
#endif
#define INT2PTR(any,d) (any)(PTRV)(d)
#endif
#endif
#ifndef PTR2ul
#if PTRSIZE == LONGSIZE
#define PTR2ul(p) (unsigned long)(p)
#else
#define PTR2ul(p) INT2PTR(unsigned long,p)
#endif
#endif
#ifndef PTR2nat
#define PTR2nat(p) (PTRV)(p)
#endif
#ifndef NUM2PTR
#define NUM2PTR(any,d) (any)PTR2nat(d)
#endif
#ifndef PTR2IV
#define PTR2IV(p) INT2PTR(IV,p)
#endif
#ifndef PTR2UV
#define PTR2UV(p) INT2PTR(UV,p)
#endif
#ifndef PTR2NV
#define PTR2NV(p) NUM2PTR(NV,p)
#endif
#undef START_EXTERN_C
#undef END_EXTERN_C
#undef EXTERN_C
#ifdef __cplusplus
#define START_EXTERN_C extern "C" {
#define END_EXTERN_C }
#define EXTERN_C extern "C"
#else
#define START_EXTERN_C
#define END_EXTERN_C
#define EXTERN_C extern
#endif
#if (PERL_BCDVERSION < 0x5004000) || defined(PERL_GCC_PEDANTIC)
#ifndef PERL_GCC_BRACE_GROUPS_FORBIDDEN
#ifndef PERL_GCC_BRACE_GROUPS_FORBIDDEN
#define PERL_GCC_BRACE_GROUPS_FORBIDDEN
#endif
#endif
#endif
#if ! defined(__GNUC__) || defined(PERL_GCC_BRACE_GROUPS_FORBIDDEN) || defined(__cplusplus)
#undef PERL_USE_GCC_BRACE_GROUPS
#else
#ifndef PERL_USE_GCC_BRACE_GROUPS
#define PERL_USE_GCC_BRACE_GROUPS
#endif
#endif
#undef STMT_START
#undef STMT_END
#if defined(VOIDFLAGS) && (VOIDFLAGS) && (defined(sun) || defined(__sun__)) && !defined(__GNUC__)
#define STMT_START if (1)
#define STMT_END else (void)0
#else
#define STMT_START do
#define STMT_END while (0)
#endif
#ifndef boolSV
#define boolSV(b) ((b) ? &PL_sv_yes : &PL_sv_no)
#endif
#ifndef DEFSV
#define DEFSV GvSV(PL_defgv)
#endif
#ifndef SAVE_DEFSV
#define SAVE_DEFSV SAVESPTR(GvSV(PL_defgv))
#endif
#ifndef DEFSV_set
#define DEFSV_set(sv) (DEFSV = (sv))
#endif
#ifndef AvFILLp
#define AvFILLp AvFILL
#endif
#ifndef av_tindex
#define av_tindex AvFILL
#endif
#ifndef av_top_index
#define av_top_index AvFILL
#endif
#ifndef av_count
#define av_count(av) (AvFILL(av)+1)
#endif
#ifndef ERRSV
#define ERRSV get_sv("@",FALSE)
#endif
#ifndef gv_stashpvn
#define gv_stashpvn(str,len,create) gv_stashpv(str,create)
#endif
#ifndef get_cv
#define get_cv perl_get_cv
#endif
#ifndef get_sv
#define get_sv perl_get_sv
#endif
#ifndef get_av
#define get_av perl_get_av
#endif
#ifndef get_hv
#define get_hv perl_get_hv
#endif
#ifndef dUNDERBAR
#define dUNDERBAR dNOOP
#endif
#ifndef UNDERBAR
#define UNDERBAR DEFSV
#endif
#ifndef dAX
#define dAX I32 ax = MARK - PL_stack_base + 1
#endif
#ifndef dITEMS
#define dITEMS I32 items = SP - MARK
#endif
#ifndef dXSTARG
#define dXSTARG SV * targ = sv_newmortal()
#endif
#ifndef dAXMARK
#define dAXMARK I32 ax = POPMARK; \
SV ** const mark = PL_stack_base + ax++
#endif
#ifndef XSprePUSH
#define XSprePUSH (sp = PL_stack_base + ax - 1)
#endif
#if (PERL_BCDVERSION < 0x5005000)
#undef XSRETURN
#define XSRETURN(off) \
STMT_START { \
PL_stack_sp = PL_stack_base + ax + ((off) - 1); \
return; \
} STMT_END
#endif
#ifndef XSPROTO
#define XSPROTO(name) void name(pTHX_ CV* cv)
#endif
#ifndef SVfARG
#define SVfARG(p) ((void*)(p))
#endif
#ifndef PERL_ABS
#define PERL_ABS(x) ((x) < 0 ? -(x) : (x))
#endif
#ifndef dVAR
#define dVAR dNOOP
#endif
#ifndef SVf
#define SVf "_"
#endif
#ifndef CPERLscope
#define CPERLscope(x) x
#endif
#ifndef PERL_HASH
#define PERL_HASH(hash,str,len) \
STMT_START { \
const char *s_PeRlHaSh = str; \
I32 i_PeRlHaSh = len; \
U32 hash_PeRlHaSh = 0; \
while (i_PeRlHaSh--) \
hash_PeRlHaSh = hash_PeRlHaSh * 33 + *s_PeRlHaSh++; \
(hash) = hash_PeRlHaSh; \
} STMT_END
#endif
#ifndef PERLIO_FUNCS_DECL
#ifdef PERLIO_FUNCS_CONST
#define PERLIO_FUNCS_DECL(funcs) const PerlIO_funcs funcs
#define PERLIO_FUNCS_CAST(funcs) (PerlIO_funcs*)(funcs)
#else
#define PERLIO_FUNCS_DECL(funcs) PerlIO_funcs funcs
#define PERLIO_FUNCS_CAST(funcs) (funcs)
#endif
#endif
#if (PERL_BCDVERSION < 0x5009003)
#ifdef ARGSproto
typedef OP* (CPERLscope(*Perl_ppaddr_t))(ARGSproto);
#else
typedef OP* (CPERLscope(*Perl_ppaddr_t))(pTHX);
#endif
typedef OP* (CPERLscope(*Perl_check_t)) (pTHX_ OP*);
#endif
#if defined(EBCDIC) && defined(NATIVE_TO_ASCI)
#ifndef NATIVE_TO_LATIN1
#define NATIVE_TO_LATIN1(c) NATIVE_TO_ASCII(c)
#endif
#ifndef LATIN1_TO_NATIVE
#define LATIN1_TO_NATIVE(c) ASCII_TO_NATIVE(c)
#endif
#ifndef NATIVE_TO_UNI
#define NATIVE_TO_UNI(c) ((c) > 255 ? (c) : NATIVE_TO_LATIN1(c))
#endif
#ifndef UNI_TO_NATIVE
#define UNI_TO_NATIVE(c) ((c) > 255 ? (c) : LATIN1_TO_NATIVE(c))
#endif
#else
#ifndef NATIVE_TO_LATIN1
#define NATIVE_TO_LATIN1(c) (c)
#endif
#ifndef LATIN1_TO_NATIVE
#define LATIN1_TO_NATIVE(c) (c)
#endif
#ifndef NATIVE_TO_UNI
#define NATIVE_TO_UNI(c) (c)
#endif
#ifndef UNI_TO_NATIVE
#define UNI_TO_NATIVE(c) (c)
#endif
#endif
#undef isPSXSPC
#undef isPSXSPC_A
#undef isPSXSPC_L1
#ifdef EBCDIC
#if (PERL_BCDVERSION < 0x5022000)
#undef isALNUM
#undef isALNUM_A
#undef isALNUM_L1
#undef isALNUMC
#undef isALNUMC_A
#undef isALNUMC_L1
#undef isALPHA
#undef isALPHA_A
#undef isALPHA_L1
#undef isALPHANUMERIC
#undef isALPHANUMERIC_A
#undef isALPHANUMERIC_L1
#undef isASCII
#undef isASCII_A
#undef isASCII_L1
#undef isBLANK
#undef isBLANK_A
#undef isBLANK_L1
#undef isCNTRL
#undef isCNTRL_A
#undef isCNTRL_L1
#undef isDIGIT
#undef isDIGIT_A
#undef isDIGIT_L1
#undef isGRAPH
#undef isGRAPH_A
#undef isGRAPH_L1
#undef isIDCONT
#undef isIDCONT_A
#undef isIDCONT_L1
#undef isIDFIRST
#undef isIDFIRST_A
#undef isIDFIRST_L1
#undef isLOWER
#undef isLOWER_A
#undef isLOWER_L1
#undef isOCTAL
#undef isOCTAL_A
#undef isOCTAL_L1
#undef isPRINT
#undef isPRINT_A
#undef isPRINT_L1
#undef isPUNCT
#undef isPUNCT_A
#undef isPUNCT_L1
#undef isSPACE
#undef isSPACE_A
#undef isSPACE_L1
#undef isUPPER
#undef isUPPER_A
#undef isUPPER_L1
#undef isWORDCHAR
#undef isWORDCHAR_A
#undef isWORDCHAR_L1
#undef isXDIGIT
#undef isXDIGIT_A
#undef isXDIGIT_L1
#endif
#ifndef isASCII
#define isASCII(c) (isCNTRL(c) || isPRINT(c))
#endif
#ifndef isCNTRL
#define isCNTRL(c) ( (c) == '\0' || (c) == '\a' || (c) == '\b' \
|| (c) == '\f' || (c) == '\n' || (c) == '\r' \
|| (c) == '\t' || (c) == '\v' \
|| ((c) <= 3 && (c) >= 1)  \
|| (c) == 7  \
|| ((c) <= 0x13 && (c) >= 0x0E)  \
\
|| (c) == 0x18  \
|| (c) == 0x19  \
|| ((c) <= 0x1F && (c) >= 0x1C)  \
|| (c) == 0x26  \
|| (c) == 0x27  \
|| (c) == 0x2D  \
|| (c) == 0x2E  \
|| (c) == 0x32  \
|| (c) == 0x37  \
|| (c) == 0x3C  \
|| (c) == 0x3D  \
|| (c) == 0x3F  \
)
#endif
#if '^' == 106
#define D_PPP_OUTLIER_CONTROL 0x5F
#else
#define D_PPP_OUTLIER_CONTROL 0xFF
#endif
#ifndef isCNTRL_L1
#define isCNTRL_L1(c) ((WIDEST_UTYPE) (c) < ' ' \
|| (WIDEST_UTYPE) (c) == D_PPP_OUTLIER_CONTROL)
#endif
#ifndef isLOWER
#define isLOWER(c) ( (c) >= 'a' && (c) <= 'z' \
&& ( (c) <= 'i' \
|| ((c) >= 'j' && (c) <= 'r') \
|| (c) >= 's'))
#endif
#ifndef isUPPER
#define isUPPER(c) ( (c) >= 'A' && (c) <= 'Z' \
&& ( (c) <= 'I' \
|| ((c) >= 'J' && (c) <= 'R') \
|| (c) >= 'S'))
#endif
#else
#if (PERL_BCDVERSION < 0x5004000)
#undef isALNUM
#undef isALNUM_A
#undef isALPHA
#undef isALPHA_A
#undef isDIGIT
#undef isDIGIT_A
#undef isIDFIRST
#undef isIDFIRST_A
#undef isLOWER
#undef isLOWER_A
#undef isUPPER
#undef isUPPER_A
#endif
#if (PERL_BCDVERSION == 0x5007000)
#undef isGRAPH
#endif
#if (PERL_BCDVERSION < 0x5008000)
#undef isCNTRL
#endif
#if (PERL_BCDVERSION < 0x5010000)
#undef isPRINT
#undef isPRINT_A
#endif
#if (PERL_BCDVERSION < 0x5014000)
#undef isASCII
#undef isASCII_A
#endif
#if (PERL_BCDVERSION < 0x5017008)
#undef isPUNCT_L1
#endif
#if (PERL_BCDVERSION < 0x5013007)
#undef isALNUMC_L1
#endif
#if (PERL_BCDVERSION < 0x5020000)
#undef isSPACE
#undef isSPACE_A
#undef isSPACE_L1
#endif
#ifndef isASCII
#define isASCII(c) ((WIDEST_UTYPE) (c) <= 127)
#endif
#ifndef isCNTRL
#define isCNTRL(c) ((WIDEST_UTYPE) (c) < ' ' || (c) == 127)
#endif
#ifndef isCNTRL_L1
#define isCNTRL_L1(c) ( (WIDEST_UTYPE) (c) < ' ' \
|| inRANGE((c), 0x7F, 0x9F))
#endif
#ifndef isLOWER
#define isLOWER(c) inRANGE((c), 'a', 'z')
#endif
#ifndef isUPPER
#define isUPPER(c) inRANGE((c), 'A', 'Z')
#endif
#endif
#ifndef isASCII_L1
#define isASCII_L1(c) isASCII(c)
#endif
#ifndef isASCII_LC
#define isASCII_LC(c) isASCII(c)
#endif
#ifndef isALNUM
#define isALNUM(c) isWORDCHAR(c)
#endif
#ifndef isALNUMC
#define isALNUMC(c) isALPHANUMERIC(c)
#endif
#ifndef isALNUMC_L1
#define isALNUMC_L1(c) isALPHANUMERIC_L1(c)
#endif
#ifndef isALPHA
#define isALPHA(c) (isUPPER(c) || isLOWER(c))
#endif
#ifndef isALPHA_L1
#define isALPHA_L1(c) (isUPPER_L1(c) || isLOWER_L1(c))
#endif
#ifndef isALPHANUMERIC
#define isALPHANUMERIC(c) (isALPHA(c) || isDIGIT(c))
#endif
#ifndef isALPHANUMERIC_L1
#define isALPHANUMERIC_L1(c) (isALPHA_L1(c) || isDIGIT(c))
#endif
#ifndef isALPHANUMERIC_LC
#define isALPHANUMERIC_LC(c) (isALPHA_LC(c) || isDIGIT_LC(c))
#endif
#ifndef isBLANK
#define isBLANK(c) ((c) == ' ' || (c) == '\t')
#endif
#ifndef isBLANK_L1
#define isBLANK_L1(c) ( isBLANK(c) \
|| ( FITS_IN_8_BITS(c) \
&& NATIVE_TO_LATIN1((U8) c) == 0xA0))
#endif
#ifndef isBLANK_LC
#define isBLANK_LC(c) isBLANK(c)
#endif
#ifndef isDIGIT
#define isDIGIT(c) inRANGE(c, '0', '9')
#endif
#ifndef isDIGIT_L1
#define isDIGIT_L1(c) isDIGIT(c)
#endif
#ifndef isGRAPH
#define isGRAPH(c) (isWORDCHAR(c) || isPUNCT(c))
#endif
#ifndef isGRAPH_L1
#define isGRAPH_L1(c) ( isPRINT_L1(c) \
&& (c) != ' ' \
&& NATIVE_TO_LATIN1((U8) c) != 0xA0)
#endif
#ifndef isIDCONT
#define isIDCONT(c) isWORDCHAR(c)
#endif
#ifndef isIDCONT_L1
#define isIDCONT_L1(c) isWORDCHAR_L1(c)
#endif
#ifndef isIDCONT_LC
#define isIDCONT_LC(c) isWORDCHAR_LC(c)
#endif
#ifndef isIDFIRST
#define isIDFIRST(c) (isALPHA(c) || (c) == '_')
#endif
#ifndef isIDFIRST_L1
#define isIDFIRST_L1(c) (isALPHA_L1(c) || (U8) (c) == '_')
#endif
#ifndef isIDFIRST_LC
#define isIDFIRST_LC(c) (isALPHA_LC(c) || (U8) (c) == '_')
#endif
#ifndef isLOWER_L1
#define isLOWER_L1(c) ( isLOWER(c) \
|| ( FITS_IN_8_BITS(c) \
&& ( ( NATIVE_TO_LATIN1((U8) c) >= 0xDF \
&& NATIVE_TO_LATIN1((U8) c) != 0xF7) \
|| NATIVE_TO_LATIN1((U8) c) == 0xAA \
|| NATIVE_TO_LATIN1((U8) c) == 0xBA \
|| NATIVE_TO_LATIN1((U8) c) == 0xB5)))
#endif
#ifndef isOCTAL
#define isOCTAL(c) (((WIDEST_UTYPE)((c)) & ~7) == '0')
#endif
#ifndef isOCTAL_L1
#define isOCTAL_L1(c) isOCTAL(c)
#endif
#ifndef isPRINT
#define isPRINT(c) (isGRAPH(c) || (c) == ' ')
#endif
#ifndef isPRINT_L1
#define isPRINT_L1(c) (FITS_IN_8_BITS(c) && ! isCNTRL_L1(c))
#endif
#ifndef isPSXSPC
#define isPSXSPC(c) isSPACE(c)
#endif
#ifndef isPSXSPC_L1
#define isPSXSPC_L1(c) isSPACE_L1(c)
#endif
#ifndef isPUNCT
#define isPUNCT(c) ( (c) == '-' || (c) == '!' || (c) == '"' \
|| (c) == '#' || (c) == '$' || (c) == '%' \
|| (c) == '&' || (c) == '\'' || (c) == '(' \
|| (c) == ')' || (c) == '*' || (c) == '+' \
|| (c) == ',' || (c) == '.' || (c) == '/' \
|| (c) == ':' || (c) == ';' || (c) == '<' \
|| (c) == '=' || (c) == '>' || (c) == '?' \
|| (c) == '@' || (c) == '[' || (c) == '\\' \
|| (c) == ']' || (c) == '^' || (c) == '_' \
|| (c) == '`' || (c) == '{' || (c) == '|' \
|| (c) == '}' || (c) == '~')
#endif
#ifndef isPUNCT_L1
#define isPUNCT_L1(c) ( isPUNCT(c) \
|| ( FITS_IN_8_BITS(c) \
&& ( NATIVE_TO_LATIN1((U8) c) == 0xA1 \
|| NATIVE_TO_LATIN1((U8) c) == 0xA7 \
|| NATIVE_TO_LATIN1((U8) c) == 0xAB \
|| NATIVE_TO_LATIN1((U8) c) == 0xB6 \
|| NATIVE_TO_LATIN1((U8) c) == 0xB7 \
|| NATIVE_TO_LATIN1((U8) c) == 0xBB \
|| NATIVE_TO_LATIN1((U8) c) == 0xBF)))
#endif
#ifndef isSPACE
#define isSPACE(c) ( isBLANK(c) || (c) == '\n' || (c) == '\r' \
|| (c) == '\v' || (c) == '\f')
#endif
#ifndef isSPACE_L1
#define isSPACE_L1(c) ( isSPACE(c) \
|| (FITS_IN_8_BITS(c) \
&& ( NATIVE_TO_LATIN1((U8) c) == 0x85 \
|| NATIVE_TO_LATIN1((U8) c) == 0xA0)))
#endif
#ifndef isUPPER_L1
#define isUPPER_L1(c) ( isUPPER(c) \
|| (FITS_IN_8_BITS(c) \
&& ( NATIVE_TO_LATIN1((U8) c) >= 0xC0 \
&& NATIVE_TO_LATIN1((U8) c) <= 0xDE \
&& NATIVE_TO_LATIN1((U8) c) != 0xD7)))
#endif
#ifndef isWORDCHAR
#define isWORDCHAR(c) (isALPHANUMERIC(c) || (c) == '_')
#endif
#ifndef isWORDCHAR_L1
#define isWORDCHAR_L1(c) (isIDFIRST_L1(c) || isDIGIT(c))
#endif
#ifndef isWORDCHAR_LC
#define isWORDCHAR_LC(c) (isIDFIRST_LC(c) || isDIGIT_LC(c))
#endif
#ifndef isXDIGIT
#define isXDIGIT(c) ( isDIGIT(c) \
|| inRANGE((c), 'a', 'f') \
|| inRANGE((c), 'A', 'F'))
#endif
#ifndef isXDIGIT_L1
#define isXDIGIT_L1(c) isXDIGIT(c)
#endif
#ifndef isXDIGIT_LC
#define isXDIGIT_LC(c) isxdigit(c)
#endif
#ifndef isALNUM_A
#define isALNUM_A(c) isALNUM(c)
#endif
#ifndef isALNUMC_A
#define isALNUMC_A(c) isALNUMC(c)
#endif
#ifndef isALPHA_A
#define isALPHA_A(c) isALPHA(c)
#endif
#ifndef isALPHANUMERIC_A
#define isALPHANUMERIC_A(c) isALPHANUMERIC(c)
#endif
#ifndef isASCII_A
#define isASCII_A(c) isASCII(c)
#endif
#ifndef isBLANK_A
#define isBLANK_A(c) isBLANK(c)
#endif
#ifndef isCNTRL_A
#define isCNTRL_A(c) isCNTRL(c)
#endif
#ifndef isDIGIT_A
#define isDIGIT_A(c) isDIGIT(c)
#endif
#ifndef isGRAPH_A
#define isGRAPH_A(c) isGRAPH(c)
#endif
#ifndef isIDCONT_A
#define isIDCONT_A(c) isIDCONT(c)
#endif
#ifndef isIDFIRST_A
#define isIDFIRST_A(c) isIDFIRST(c)
#endif
#ifndef isLOWER_A
#define isLOWER_A(c) isLOWER(c)
#endif
#ifndef isOCTAL_A
#define isOCTAL_A(c) isOCTAL(c)
#endif
#ifndef isPRINT_A
#define isPRINT_A(c) isPRINT(c)
#endif
#ifndef isPSXSPC_A
#define isPSXSPC_A(c) isPSXSPC(c)
#endif
#ifndef isPUNCT_A
#define isPUNCT_A(c) isPUNCT(c)
#endif
#ifndef isSPACE_A
#define isSPACE_A(c) isSPACE(c)
#endif
#ifndef isUPPER_A
#define isUPPER_A(c) isUPPER(c)
#endif
#ifndef isWORDCHAR_A
#define isWORDCHAR_A(c) isWORDCHAR(c)
#endif
#ifndef isXDIGIT_A
#define isXDIGIT_A(c) isXDIGIT(c)
#endif
#ifndef isASCII_utf8_safe
#define isASCII_utf8_safe(s,e) (((e) - (s)) <= 0 ? 0 : isASCII(*(s)))
#endif
#ifndef isASCII_uvchr
#define isASCII_uvchr(c) (FITS_IN_8_BITS(c) ? isASCII_L1(c) : 0)
#endif
#if (PERL_BCDVERSION >= 0x5006000)
#ifdef isALPHA_uni
#define D_PPP_is_ctype(upper, lower, c) \
(FITS_IN_8_BITS(c) \
? is ## upper ## _L1(c) \
: is ## upper ## _uni((UV) (c)))
#else
#define D_PPP_is_ctype(upper, lower, c) \
(FITS_IN_8_BITS(c) \
? is ## upper ## _L1(c) \
: is_uni_ ## lower((UV) (c)))
#endif
#ifndef isALPHA_uvchr
#define isALPHA_uvchr(c) D_PPP_is_ctype(ALPHA, alpha, c)
#endif
#ifndef isALPHANUMERIC_uvchr
#define isALPHANUMERIC_uvchr(c) (isALPHA_uvchr(c) || isDIGIT_uvchr(c))
#endif
#ifdef is_uni_blank
#ifndef isBLANK_uvchr
#define isBLANK_uvchr(c) D_PPP_is_ctype(BLANK, blank, c)
#endif
#else
#ifndef isBLANK_uvchr
#define isBLANK_uvchr(c) (FITS_IN_8_BITS(c) \
? isBLANK_L1(c) \
: ( (UV) (c) == 0x1680  \
|| inRANGE((UV) (c), 0x2000, 0x200A) \
|| (UV) (c) == 0x202F \
|| (UV) (c) == 0x205F \
|| (UV) (c) == 0x3000))
#endif
#endif
#ifndef isCNTRL_uvchr
#define isCNTRL_uvchr(c) D_PPP_is_ctype(CNTRL, cntrl, c)
#endif
#ifndef isDIGIT_uvchr
#define isDIGIT_uvchr(c) D_PPP_is_ctype(DIGIT, digit, c)
#endif
#ifndef isGRAPH_uvchr
#define isGRAPH_uvchr(c) D_PPP_is_ctype(GRAPH, graph, c)
#endif
#ifndef isIDCONT_uvchr
#define isIDCONT_uvchr(c) isWORDCHAR_uvchr(c)
#endif
#ifndef isIDFIRST_uvchr
#define isIDFIRST_uvchr(c) D_PPP_is_ctype(IDFIRST, idfirst, c)
#endif
#ifndef isLOWER_uvchr
#define isLOWER_uvchr(c) D_PPP_is_ctype(LOWER, lower, c)
#endif
#ifndef isPRINT_uvchr
#define isPRINT_uvchr(c) D_PPP_is_ctype(PRINT, print, c)
#endif
#ifndef isPSXSPC_uvchr
#define isPSXSPC_uvchr(c) isSPACE_uvchr(c)
#endif
#ifndef isPUNCT_uvchr
#define isPUNCT_uvchr(c) D_PPP_is_ctype(PUNCT, punct, c)
#endif
#ifndef isSPACE_uvchr
#define isSPACE_uvchr(c) D_PPP_is_ctype(SPACE, space, c)
#endif
#ifndef isUPPER_uvchr
#define isUPPER_uvchr(c) D_PPP_is_ctype(UPPER, upper, c)
#endif
#ifndef isXDIGIT_uvchr
#define isXDIGIT_uvchr(c) D_PPP_is_ctype(XDIGIT, xdigit, c)
#endif
#ifndef isWORDCHAR_uvchr
#define isWORDCHAR_uvchr(c) (FITS_IN_8_BITS(c) \
? isWORDCHAR_L1(c) : isALPHANUMERIC_uvchr(c))
#endif
#ifndef isALPHA_utf8_safe
#define isALPHA_utf8_safe(s,e) D_PPP_IS_GENERIC_UTF8_SAFE(s, e, ALPHA)
#endif
#ifdef isALPHANUMERIC_utf8
#ifndef isALPHANUMERIC_utf8_safe
#define isALPHANUMERIC_utf8_safe(s,e) \
D_PPP_IS_GENERIC_UTF8_SAFE(s, e, ALPHANUMERIC)
#endif
#else
#ifndef isALPHANUMERIC_utf8_safe
#define isALPHANUMERIC_utf8_safe(s,e) \
(isALPHA_utf8_safe(s,e) || isDIGIT_utf8_safe(s,e))
#endif
#endif
#if 'A' == 65
#ifndef isBLANK_utf8_safe
#define isBLANK_utf8_safe(s,e) \
( ( LIKELY((e) > (s)) ) ?  \
( ( 0x09 == ((const U8*)s)[0] || 0x20 == ((const U8*)s)[0] ) ? 1 \
: ( LIKELY(((e) - (s)) >= UTF8SKIP(s)) ) ? \
( ( 0xC2 == ((const U8*)s)[0] ) ? \
( ( 0xA0 == ((const U8*)s)[1] ) ? 2 : 0 ) \
: ( 0xE1 == ((const U8*)s)[0] ) ? \
( ( ( 0x9A == ((const U8*)s)[1] ) && ( 0x80 == ((const U8*)s)[2] ) ) ? 3 : 0 )\
: ( 0xE2 == ((const U8*)s)[0] ) ? \
( ( 0x80 == ((const U8*)s)[1] ) ? \
( ( inRANGE(((const U8*)s)[2], 0x80, 0x8A ) || 0xAF == ((const U8*)s)[2] ) ? 3 : 0 )\
: ( ( 0x81 == ((const U8*)s)[1] ) && ( 0x9F == ((const U8*)s)[2] ) ) ? 3 : 0 )\
: ( ( ( 0xE3 == ((const U8*)s)[0] ) && ( 0x80 == ((const U8*)s)[1] ) ) && ( 0x80 == ((const U8*)s)[2] ) ) ? 3 : 0 )\
: 0 ) \
: 0 )
#endif
#elif 'A' == 193 && '^' == 95
#ifndef isBLANK_utf8_safe
#define isBLANK_utf8_safe(s,e) \
( ( LIKELY((e) > (s)) ) ? \
( ( 0x05 == ((const U8*)s)[0] || 0x40 == ((const U8*)s)[0] ) ? 1 \
: ( LIKELY(((e) - (s)) >= UTF8SKIP(s)) ) ? \
( ( 0x80 == ((const U8*)s)[0] ) ? \
( ( 0x41 == ((const U8*)s)[1] ) ? 2 : 0 ) \
: ( 0xBC == ((const U8*)s)[0] ) ? \
( ( ( 0x63 == ((const U8*)s)[1] ) && ( 0x41 == ((const U8*)s)[2] ) ) ? 3 : 0 )\
: ( 0xCA == ((const U8*)s)[0] ) ? \
( ( 0x41 == ((const U8*)s)[1] ) ? \
( ( inRANGE(((const U8*)s)[2], 0x41, 0x4A ) || 0x51 == ((const U8*)s)[2] ) ? 3 : 0 )\
: ( 0x42 == ((const U8*)s)[1] ) ? \
( ( 0x56 == ((const U8*)s)[2] ) ? 3 : 0 ) \
: ( ( 0x43 == ((const U8*)s)[1] ) && ( 0x73 == ((const U8*)s)[2] ) ) ? 3 : 0 )\
: ( ( ( 0xCE == ((const U8*)s)[0] ) && ( 0x41 == ((const U8*)s)[1] ) ) && ( 0x41 == ((const U8*)s)[2] ) ) ? 3 : 0 )\
: 0 ) \
: 0 )
#endif
#elif 'A' == 193 && '^' == 176
#ifndef isBLANK_utf8_safe
#define isBLANK_utf8_safe(s,e) \
( ( LIKELY((e) > (s)) ) ? \
( ( 0x05 == ((const U8*)s)[0] || 0x40 == ((const U8*)s)[0] ) ? 1 \
: ( LIKELY(((e) - (s)) >= UTF8SKIP(s)) ) ? \
( ( 0x78 == ((const U8*)s)[0] ) ? \
( ( 0x41 == ((const U8*)s)[1] ) ? 2 : 0 ) \
: ( 0xBD == ((const U8*)s)[0] ) ? \
( ( ( 0x62 == ((const U8*)s)[1] ) && ( 0x41 == ((const U8*)s)[2] ) ) ? 3 : 0 )\
: ( 0xCA == ((const U8*)s)[0] ) ? \
( ( 0x41 == ((const U8*)s)[1] ) ? \
( ( inRANGE(((const U8*)s)[2], 0x41, 0x4A ) || 0x51 == ((const U8*)s)[2] ) ? 3 : 0 )\
: ( 0x42 == ((const U8*)s)[1] ) ? \
( ( 0x56 == ((const U8*)s)[2] ) ? 3 : 0 ) \
: ( ( 0x43 == ((const U8*)s)[1] ) && ( 0x72 == ((const U8*)s)[2] ) ) ? 3 : 0 )\
: ( ( ( 0xCE == ((const U8*)s)[0] ) && ( 0x41 == ((const U8*)s)[1] ) ) && ( 0x41 == ((const U8*)s)[2] ) ) ? 3 : 0 )\
: 0 ) \
: 0 )
#endif
#else
#error Unknown character set
#endif
#ifndef isCNTRL_utf8_safe
#define isCNTRL_utf8_safe(s,e) D_PPP_IS_GENERIC_UTF8_SAFE(s, e, CNTRL)
#endif
#ifndef isDIGIT_utf8_safe
#define isDIGIT_utf8_safe(s,e) D_PPP_IS_GENERIC_UTF8_SAFE(s, e, DIGIT)
#endif
#ifndef isGRAPH_utf8_safe
#define isGRAPH_utf8_safe(s,e) D_PPP_IS_GENERIC_UTF8_SAFE(s, e, GRAPH)
#endif
#ifdef isIDCONT_utf8
#ifndef isIDCONT_utf8_safe
#define isIDCONT_utf8_safe(s,e) D_PPP_IS_GENERIC_UTF8_SAFE(s, e, IDCONT)
#endif
#else
#ifndef isIDCONT_utf8_safe
#define isIDCONT_utf8_safe(s,e) isWORDCHAR_utf8_safe(s,e)
#endif
#endif
#ifndef isIDFIRST_utf8_safe
#define isIDFIRST_utf8_safe(s,e) D_PPP_IS_GENERIC_UTF8_SAFE(s, e, IDFIRST)
#endif
#ifndef isLOWER_utf8_safe
#define isLOWER_utf8_safe(s,e) D_PPP_IS_GENERIC_UTF8_SAFE(s, e, LOWER)
#endif
#ifndef isPRINT_utf8_safe
#define isPRINT_utf8_safe(s,e) D_PPP_IS_GENERIC_UTF8_SAFE(s, e, PRINT)
#endif
#undef isPSXSPC_utf8_safe
#ifndef isPSXSPC_utf8_safe
#define isPSXSPC_utf8_safe(s,e) isSPACE_utf8_safe(s,e)
#endif
#ifndef isPUNCT_utf8_safe
#define isPUNCT_utf8_safe(s,e) D_PPP_IS_GENERIC_UTF8_SAFE(s, e, PUNCT)
#endif
#ifndef isSPACE_utf8_safe
#define isSPACE_utf8_safe(s,e) D_PPP_IS_GENERIC_UTF8_SAFE(s, e, SPACE)
#endif
#ifndef isUPPER_utf8_safe
#define isUPPER_utf8_safe(s,e) D_PPP_IS_GENERIC_UTF8_SAFE(s, e, UPPER)
#endif
#ifdef isWORDCHAR_utf8
#ifndef isWORDCHAR_utf8_safe
#define isWORDCHAR_utf8_safe(s,e) D_PPP_IS_GENERIC_UTF8_SAFE(s, e, WORDCHAR)
#endif
#else
#ifndef isWORDCHAR_utf8_safe
#define isWORDCHAR_utf8_safe(s,e) \
(isALPHANUMERIC_utf8_safe(s,e) || (*(s)) == '_')
#endif
#endif
#if 'A' == 65
#ifndef isXDIGIT_utf8_safe
#define isXDIGIT_utf8_safe(s,e) \
( ( LIKELY((e) > (s)) ) ? \
( ( inRANGE(((const U8*)s)[0], 0x30, 0x39 ) || inRANGE(((const U8*)s)[0], 0x41, 0x46 ) || inRANGE(((const U8*)s)[0], 0x61, 0x66 ) ) ? 1\
: ( ( LIKELY(((e) - (s)) >= UTF8SKIP(s)) ) && ( 0xEF == ((const U8*)s)[0] ) ) ? ( ( 0xBC == ((const U8*)s)[1] ) ?\
( ( inRANGE(((const U8*)s)[2], 0x90, 0x99 ) || inRANGE(((const U8*)s)[2], 0xA1, 0xA6 ) ) ? 3 : 0 )\
: ( ( 0xBD == ((const U8*)s)[1] ) && ( inRANGE(((const U8*)s)[2], 0x81, 0x86 ) ) ) ? 3 : 0 ) : 0 )\
: 0 )
#endif
#elif 'A' == 193 && '^' == 95
#ifndef isXDIGIT_utf8_safe
#define isXDIGIT_utf8_safe(s,e) \
( ( LIKELY((e) > (s)) ) ? \
( ( inRANGE(((const U8*)s)[0], 0x81, 0x86 ) || inRANGE(((const U8*)s)[0], 0xC1, 0xC6 ) || inRANGE(((const U8*)s)[0], 0xF0, 0xF9 ) ) ? 1\
: ( ( ( LIKELY(((e) - (s)) >= UTF8SKIP(s)) ) && ( 0xDD == ((const U8*)s)[0] ) ) && ( 0x73 == ((const U8*)s)[1] ) ) ? ( ( 0x67 == ((const U8*)s)[2] ) ?\
( ( inRANGE(((const U8*)s)[3], 0x57, 0x59 ) || inRANGE(((const U8*)s)[3], 0x62, 0x68 ) ) ? 4 : 0 )\
: ( ( inRANGE(((const U8*)s)[2], 0x68, 0x69 ) ) && ( inRANGE(((const U8*)s)[3], 0x42, 0x47 ) ) ) ? 4 : 0 ) : 0 )\
: 0 )
#endif
#elif 'A' == 193 && '^' == 176
#ifndef isXDIGIT_utf8_safe
#define isXDIGIT_utf8_safe(s,e) \
( ( LIKELY((e) > (s)) ) ? \
( ( inRANGE(((const U8*)s)[0], 0x81, 0x86 ) || inRANGE(((const U8*)s)[0], 0xC1, 0xC6 ) || inRANGE(((const U8*)s)[0], 0xF0, 0xF9 ) ) ? 1\
: ( ( ( LIKELY(((e) - (s)) >= UTF8SKIP(s)) ) && ( 0xDD == ((const U8*)s)[0] ) ) && ( 0x72 == ((const U8*)s)[1] ) ) ? ( ( 0x66 == ((const U8*)s)[2] ) ?\
( ( inRANGE(((const U8*)s)[3], 0x57, 0x59 ) || 0x5F == ((const U8*)s)[3] || inRANGE(((const U8*)s)[3], 0x62, 0x67 ) ) ? 4 : 0 )\
: ( ( inRANGE(((const U8*)s)[2], 0x67, 0x68 ) ) && ( inRANGE(((const U8*)s)[3], 0x42, 0x47 ) ) ) ? 4 : 0 ) : 0 )\
: 0 )
#endif
#else
#error Unknown character set
#endif
#ifndef isALPHA_LC_utf8_safe
#define isALPHA_LC_utf8_safe(s,e) D_PPP_IS_GENERIC_LC_UTF8_SAFE(s, e, ALPHA)
#endif
#ifdef isALPHANUMERIC_utf8
#ifndef isALPHANUMERIC_LC_utf8_safe
#define isALPHANUMERIC_LC_utf8_safe(s,e) \
D_PPP_IS_GENERIC_LC_UTF8_SAFE(s, e, ALPHANUMERIC)
#endif
#else
#ifndef isALPHANUMERIC_LC_utf8_safe
#define isALPHANUMERIC_LC_utf8_safe(s,e) \
(isALPHA_LC_utf8_safe(s,e) || isDIGIT_LC_utf8_safe(s,e))
#endif
#endif
#ifndef isBLANK_LC_utf8_safe
#define isBLANK_LC_utf8_safe(s,e) \
D_PPP_IS_GENERIC_LC_UTF8_SAFE_BROKEN(s, e, BLANK)
#endif
#ifndef isCNTRL_LC_utf8_safe
#define isCNTRL_LC_utf8_safe(s,e) D_PPP_IS_GENERIC_LC_UTF8_SAFE(s, e, CNTRL)
#endif
#ifndef isDIGIT_LC_utf8_safe
#define isDIGIT_LC_utf8_safe(s,e) D_PPP_IS_GENERIC_LC_UTF8_SAFE(s, e, DIGIT)
#endif
#ifndef isGRAPH_LC_utf8_safe
#define isGRAPH_LC_utf8_safe(s,e) D_PPP_IS_GENERIC_LC_UTF8_SAFE(s, e, GRAPH)
#endif
#ifdef isIDCONT_utf8
#ifndef isIDCONT_LC_utf8_safe
#define isIDCONT_LC_utf8_safe(s,e) D_PPP_IS_GENERIC_LC_UTF8_SAFE(s, e, IDCONT)
#endif
#else
#ifndef isIDCONT_LC_utf8_safe
#define isIDCONT_LC_utf8_safe(s,e) isWORDCHAR_LC_utf8_safe(s,e)
#endif
#endif
#ifndef isIDFIRST_LC_utf8_safe
#define isIDFIRST_LC_utf8_safe(s,e) D_PPP_IS_GENERIC_LC_UTF8_SAFE(s, e, IDFIRST)
#endif
#ifndef isLOWER_LC_utf8_safe
#define isLOWER_LC_utf8_safe(s,e) D_PPP_IS_GENERIC_LC_UTF8_SAFE(s, e, LOWER)
#endif
#ifndef isPRINT_LC_utf8_safe
#define isPRINT_LC_utf8_safe(s,e) D_PPP_IS_GENERIC_LC_UTF8_SAFE(s, e, PRINT)
#endif
#undef isPSXSPC_LC_utf8_safe
#ifndef isPSXSPC_LC_utf8_safe
#define isPSXSPC_LC_utf8_safe(s,e) isSPACE_LC_utf8_safe(s,e)
#endif
#ifndef isPUNCT_LC_utf8_safe
#define isPUNCT_LC_utf8_safe(s,e) D_PPP_IS_GENERIC_LC_UTF8_SAFE(s, e, PUNCT)
#endif
#ifndef isSPACE_LC_utf8_safe
#define isSPACE_LC_utf8_safe(s,e) D_PPP_IS_GENERIC_LC_UTF8_SAFE(s, e, SPACE)
#endif
#ifndef isUPPER_LC_utf8_safe
#define isUPPER_LC_utf8_safe(s,e) D_PPP_IS_GENERIC_LC_UTF8_SAFE(s, e, UPPER)
#endif
#ifdef isWORDCHAR_utf8
#ifndef isWORDCHAR_LC_utf8_safe
#define isWORDCHAR_LC_utf8_safe(s,e) D_PPP_IS_GENERIC_LC_UTF8_SAFE(s, e, WORDCHAR)
#endif
#else
#ifndef isWORDCHAR_LC_utf8_safe
#define isWORDCHAR_LC_utf8_safe(s,e) \
(isALPHANUMERIC_LC_utf8_safe(s,e) || (*(s)) == '_')
#endif
#endif
#ifndef isXDIGIT_LC_utf8_safe
#define isXDIGIT_LC_utf8_safe(s,e) \
D_PPP_IS_GENERIC_LC_UTF8_SAFE_BROKEN(s, e, XDIGIT)
#endif
#endif
#define D_PPP_TOO_SHORT_MSG "Malformed UTF-8 character starting with:" \
" \\x%02x (too short; %d bytes available, need" \
" %d)\n"
#if (PERL_BCDVERSION >= 0x5007003)
#ifndef toLOWER_uvchr
#define toLOWER_uvchr(c, s, l) UNI_TO_NATIVE(to_uni_lower(NATIVE_TO_UNI(c), s, l))
#endif
#ifndef toUPPER_uvchr
#define toUPPER_uvchr(c, s, l) UNI_TO_NATIVE(to_uni_upper(NATIVE_TO_UNI(c), s, l))
#endif
#ifndef toTITLE_uvchr
#define toTITLE_uvchr(c, s, l) UNI_TO_NATIVE(to_uni_title(NATIVE_TO_UNI(c), s, l))
#endif
#ifndef toFOLD_uvchr
#define toFOLD_uvchr(c, s, l) UNI_TO_NATIVE(to_uni_fold( NATIVE_TO_UNI(c), s, l))
#endif
#if (PERL_BCDVERSION != 0x5015006)
#if defined toLOWER_utf8
#define D_PPP_TO_LOWER_CALLEE(s,r,l) toLOWER_utf8(s,r,l)
#else
#define D_PPP_TO_LOWER_CALLEE(s,r,l) to_utf8_lower(s,r,l)
#endif
#if defined toTITLE_utf8
#define D_PPP_TO_TITLE_CALLEE(s,r,l) toTITLE_utf8(s,r,l)
#else
#define D_PPP_TO_TITLE_CALLEE(s,r,l) to_utf8_title(s,r,l)
#endif
#if defined toUPPER_utf8
#define D_PPP_TO_UPPER_CALLEE(s,r,l) toUPPER_utf8(s,r,l)
#else
#define D_PPP_TO_UPPER_CALLEE(s,r,l) to_utf8_upper(s,r,l)
#endif
#if defined toFOLD_utf8
#define D_PPP_TO_FOLD_CALLEE(s,r,l) toFOLD_utf8(s,r,l)
#else
#define D_PPP_TO_FOLD_CALLEE(s,r,l) to_utf8_fold(s,r,l)
#endif
#else
#define D_PPP_TO_LOWER_CALLEE(s,r,l) \
Perl__to_utf8_lower_flags(aTHX_ s, r, l, 0, NULL)
#define D_PPP_TO_TITLE_CALLEE(s,r,l) \
Perl__to_utf8_title_flags(aTHX_ s, r, l, 0, NULL)
#define D_PPP_TO_UPPER_CALLEE(s,r,l) \
Perl__to_utf8_upper_flags(aTHX_ s, r, l, 0, NULL)
#define D_PPP_TO_FOLD_CALLEE(s,r,l) \
Perl__to_utf8_fold_flags(aTHX_ s, r, l, FOLD_FLAGS_FULL, NULL)
#endif
#define D_PPP_GENERIC_MULTI_ARG_TO(name, s, e,r,l) \
(((((e) - (s)) <= 0) \
\
? (croak("Attempting case change on zero length string"), \
0)  \
: ((e) - (s)) < UTF8SKIP(s)) \
? (croak(D_PPP_TOO_SHORT_MSG, \
s[0], (int) ((e) - (s)), (int) UTF8SKIP(s)), \
0) \
: D_PPP_TO_ ## name ## _CALLEE(s,r,l))
#ifndef toUPPER_utf8_safe
#define toUPPER_utf8_safe(s,e,r,l) \
D_PPP_GENERIC_MULTI_ARG_TO(UPPER,s,e,r,l)
#endif
#ifndef toLOWER_utf8_safe
#define toLOWER_utf8_safe(s,e,r,l) \
D_PPP_GENERIC_MULTI_ARG_TO(LOWER,s,e,r,l)
#endif
#ifndef toTITLE_utf8_safe
#define toTITLE_utf8_safe(s,e,r,l) \
D_PPP_GENERIC_MULTI_ARG_TO(TITLE,s,e,r,l)
#endif
#ifndef toFOLD_utf8_safe
#define toFOLD_utf8_safe(s,e,r,l) \
D_PPP_GENERIC_MULTI_ARG_TO(FOLD,s,e,r,l)
#endif
#elif (PERL_BCDVERSION >= 0x5006000)
#ifdef uvchr_to_utf8
#define D_PPP_UV_TO_UTF8 uvchr_to_utf8
#else
#define D_PPP_UV_TO_UTF8 uv_to_utf8
#endif
#define D_PPP_GENERIC_SINGLE_ARG_TO_UVCHR(name, c, s, l) \
(*(l) = (D_PPP_UV_TO_UTF8(s, \
UNI_TO_NATIVE(to_uni_ ## name(NATIVE_TO_UNI(c)))) - (s)), \
UNI_TO_NATIVE(to_uni_ ## name(NATIVE_TO_UNI(c))))
#ifndef toLOWER_uvchr
#define toLOWER_uvchr(c, s, l) \
D_PPP_GENERIC_SINGLE_ARG_TO_UVCHR(lower, c, s, l)
#endif
#ifndef toUPPER_uvchr
#define toUPPER_uvchr(c, s, l) \
D_PPP_GENERIC_SINGLE_ARG_TO_UVCHR(upper, c, s, l)
#endif
#ifndef toTITLE_uvchr
#define toTITLE_uvchr(c, s, l) \
D_PPP_GENERIC_SINGLE_ARG_TO_UVCHR(title, c, s, l)
#endif
#ifndef toFOLD_uvchr
#define toFOLD_uvchr(c, s, l) toLOWER_uvchr(c, s, l)
#endif
#define D_PPP_GENERIC_SINGLE_ARG_TO_UTF8(name, s, e, r, l) \
(((((e) - (s)) <= 0) \
? (croak("Attempting case change on zero length string"), \
0)  \
: ((e) - (s)) < UTF8SKIP(s)) \
? (croak(D_PPP_TOO_SHORT_MSG, \
s[0], (int) ((e) - (s)), (int) UTF8SKIP(s)), \
0) \
\
: D_PPP_UV_TO_UTF8(r, to_utf8_ ## name(s)), \
\
*(l) = UTF8SKIP(r), to_utf8_ ## name(r))
#ifndef toUPPER_utf8_safe
#define toUPPER_utf8_safe(s,e,r,l) \
D_PPP_GENERIC_SINGLE_ARG_TO_UTF8(upper, s, e, r, l)
#endif
#ifndef toLOWER_utf8_safe
#define toLOWER_utf8_safe(s,e,r,l) \
D_PPP_GENERIC_SINGLE_ARG_TO_UTF8(lower, s, e, r, l)
#endif
#ifndef toTITLE_utf8_safe
#define toTITLE_utf8_safe(s,e,r,l) \
D_PPP_GENERIC_SINGLE_ARG_TO_UTF8(title, s, e, r, l)
#endif
#ifndef toFOLD_utf8_safe
#define toFOLD_utf8_safe(s,e,r,l) toLOWER_utf8_safe(s,e,r,l)
#endif
#endif
#if (PERL_BCDVERSION >= 0x5008000)
#ifndef HeUTF8
#define HeUTF8(he) ((HeKLEN(he) == HEf_SVKEY) ? \
SvUTF8(HeKEY_sv(he)) : \
(U32)HeKUTF8(he))
#endif
#endif
#ifndef C_ARRAY_LENGTH
#define C_ARRAY_LENGTH(a) (sizeof(a)/sizeof((a)[0]))
#endif
#ifndef C_ARRAY_END
#define C_ARRAY_END(a) ((a) + C_ARRAY_LENGTH(a))
#endif
#ifndef LIKELY
#define LIKELY(x) (x)
#endif
#ifndef UNLIKELY
#define UNLIKELY(x) (x)
#endif
#ifndef MUTABLE_PTR
#if defined(PERL_USE_GCC_BRACE_GROUPS)
#define MUTABLE_PTR(p) ({ void *_p = (p); _p; })
#else
#define MUTABLE_PTR(p) ((void *) (p))
#endif
#endif
#ifndef MUTABLE_AV
#define MUTABLE_AV(p) ((AV *)MUTABLE_PTR(p))
#endif
#ifndef MUTABLE_CV
#define MUTABLE_CV(p) ((CV *)MUTABLE_PTR(p))
#endif
#ifndef MUTABLE_GV
#define MUTABLE_GV(p) ((GV *)MUTABLE_PTR(p))
#endif
#ifndef MUTABLE_HV
#define MUTABLE_HV(p) ((HV *)MUTABLE_PTR(p))
#endif
#ifndef MUTABLE_IO
#define MUTABLE_IO(p) ((IO *)MUTABLE_PTR(p))
#endif
#ifndef MUTABLE_SV
#define MUTABLE_SV(p) ((SV *)MUTABLE_PTR(p))
#endif
#if (PERL_BCDVERSION >= 0x5004000) && !defined(vnewSVpvf)
#if defined(PERL_USE_GCC_BRACE_GROUPS)
#define vnewSVpvf(pat, args) ({ SV *_sv = newSV(0); sv_vsetpvfn(_sv, (pat), strlen((pat)), (args), Null(SV**), 0, Null(bool*)); _sv; })
#else
#define vnewSVpvf(pat, args) ((PL_Sv = newSV(0)), sv_vsetpvfn(PL_Sv, (pat), strlen((pat)), (args), Null(SV**), 0, Null(bool*)), PL_Sv)
#endif
#endif
#if (PERL_BCDVERSION >= 0x5004000) && !defined(sv_vcatpvf)
#define sv_vcatpvf(sv, pat, args) sv_vcatpvfn(sv, pat, strlen(pat), args, Null(SV**), 0, Null(bool*))
#endif
#if (PERL_BCDVERSION >= 0x5004000) && !defined(sv_vsetpvf)
#define sv_vsetpvf(sv, pat, args) sv_vsetpvfn(sv, pat, strlen(pat), args, Null(SV**), 0, Null(bool*))
#endif
#if (PERL_BCDVERSION >= 0x5004000) && !defined(sv_catpvf_mg)
#if defined(NEED_sv_catpvf_mg)
static void DPPP_(my_sv_catpvf_mg)(pTHX_ SV * const sv, const char * const pat, ...);
static
#else
extern void DPPP_(my_sv_catpvf_mg)(pTHX_ SV * const sv, const char * const pat, ...);
#endif
#if defined(NEED_sv_catpvf_mg) || defined(NEED_sv_catpvf_mg_GLOBAL)
#define Perl_sv_catpvf_mg DPPP_(my_sv_catpvf_mg)
void
DPPP_(my_sv_catpvf_mg)(pTHX_ SV * const sv, const char * const pat, ...)
{
va_list args;
va_start(args, pat);
sv_vcatpvfn(sv, pat, strlen(pat), &args, Null(SV**), 0, Null(bool*));
SvSETMAGIC(sv);
va_end(args);
}
#endif
#endif
#ifdef PERL_IMPLICIT_CONTEXT
#if (PERL_BCDVERSION >= 0x5004000) && !defined(sv_catpvf_mg_nocontext)
#if defined(NEED_sv_catpvf_mg_nocontext)
static void DPPP_(my_sv_catpvf_mg_nocontext)(SV * const sv, const char * const pat, ...);
static
#else
extern void DPPP_(my_sv_catpvf_mg_nocontext)(SV * const sv, const char * const pat, ...);
#endif
#if defined(NEED_sv_catpvf_mg_nocontext) || defined(NEED_sv_catpvf_mg_nocontext_GLOBAL)
#define sv_catpvf_mg_nocontext DPPP_(my_sv_catpvf_mg_nocontext)
#define Perl_sv_catpvf_mg_nocontext DPPP_(my_sv_catpvf_mg_nocontext)
void
DPPP_(my_sv_catpvf_mg_nocontext)(SV * const sv, const char * const pat, ...)
{
dTHX;
va_list args;
va_start(args, pat);
sv_vcatpvfn(sv, pat, strlen(pat), &args, Null(SV**), 0, Null(bool*));
SvSETMAGIC(sv);
va_end(args);
}
#endif
#endif
#endif
#ifndef sv_catpvf_mg
#ifdef PERL_IMPLICIT_CONTEXT
#define sv_catpvf_mg Perl_sv_catpvf_mg_nocontext
#else
#define sv_catpvf_mg Perl_sv_catpvf_mg
#endif
#endif
#if (PERL_BCDVERSION >= 0x5004000) && !defined(sv_vcatpvf_mg)
#define sv_vcatpvf_mg(sv, pat, args) \
STMT_START { \
sv_vcatpvfn(sv, pat, strlen(pat), args, Null(SV**), 0, Null(bool*)); \
SvSETMAGIC(sv); \
} STMT_END
#endif
#if (PERL_BCDVERSION >= 0x5004000) && !defined(sv_setpvf_mg)
#if defined(NEED_sv_setpvf_mg)
static void DPPP_(my_sv_setpvf_mg)(pTHX_ SV * const sv, const char * const pat, ...);
static
#else
extern void DPPP_(my_sv_setpvf_mg)(pTHX_ SV * const sv, const char * const pat, ...);
#endif
#if defined(NEED_sv_setpvf_mg) || defined(NEED_sv_setpvf_mg_GLOBAL)
#define Perl_sv_setpvf_mg DPPP_(my_sv_setpvf_mg)
void
DPPP_(my_sv_setpvf_mg)(pTHX_ SV * const sv, const char * const pat, ...)
{
va_list args;
va_start(args, pat);
sv_vsetpvfn(sv, pat, strlen(pat), &args, Null(SV**), 0, Null(bool*));
SvSETMAGIC(sv);
va_end(args);
}
#endif
#endif
#ifdef PERL_IMPLICIT_CONTEXT
#if (PERL_BCDVERSION >= 0x5004000) && !defined(sv_setpvf_mg_nocontext)
#if defined(NEED_sv_setpvf_mg_nocontext)
static void DPPP_(my_sv_setpvf_mg_nocontext)(SV * const sv, const char * const pat, ...);
static
#else
extern void DPPP_(my_sv_setpvf_mg_nocontext)(SV * const sv, const char * const pat, ...);
#endif
#if defined(NEED_sv_setpvf_mg_nocontext) || defined(NEED_sv_setpvf_mg_nocontext_GLOBAL)
#define sv_setpvf_mg_nocontext DPPP_(my_sv_setpvf_mg_nocontext)
#define Perl_sv_setpvf_mg_nocontext DPPP_(my_sv_setpvf_mg_nocontext)
void
DPPP_(my_sv_setpvf_mg_nocontext)(SV * const sv, const char * const pat, ...)
{
dTHX;
va_list args;
va_start(args, pat);
sv_vsetpvfn(sv, pat, strlen(pat), &args, Null(SV**), 0, Null(bool*));
SvSETMAGIC(sv);
va_end(args);
}
#endif
#endif
#endif
#ifndef sv_setpvf_mg
#ifdef PERL_IMPLICIT_CONTEXT
#define sv_setpvf_mg Perl_sv_setpvf_mg_nocontext
#else
#define sv_setpvf_mg Perl_sv_setpvf_mg
#endif
#endif
#if (PERL_BCDVERSION >= 0x5004000) && !defined(sv_vsetpvf_mg)
#define sv_vsetpvf_mg(sv, pat, args) \
STMT_START { \
sv_vsetpvfn(sv, pat, strlen(pat), args, Null(SV**), 0, Null(bool*)); \
SvSETMAGIC(sv); \
} STMT_END
#endif
#ifndef sv_2pv_nolen
#define sv_2pv_nolen(sv) SvPV_nolen(sv)
#endif
#ifdef SvPVbyte
#if (PERL_BCDVERSION < 0x5007000)
#ifndef sv_2pvbyte
#define sv_2pvbyte(sv, lp) (sv_utf8_downgrade((sv), 0), SvPV((sv), *(lp)))
#endif
#undef SvPVbyte
#define SvPVbyte(sv, lp) \
((SvFLAGS(sv) & (SVf_POK|SVf_UTF8)) == (SVf_POK) \
? ((lp = SvCUR(sv)), SvPVX(sv)) : sv_2pvbyte(sv, &lp))
#endif
#else
#define SvPVbyte SvPV
#define sv_2pvbyte sv_2pv
#endif
#ifndef sv_2pvbyte_nolen
#define sv_2pvbyte_nolen(sv) sv_2pv_nolen(sv)
#endif
#ifndef SV_IMMEDIATE_UNREF
#define SV_IMMEDIATE_UNREF 0
#endif
#ifndef SV_GMAGIC
#define SV_GMAGIC 0
#endif
#ifndef SV_COW_DROP_PV
#define SV_COW_DROP_PV 0
#endif
#ifndef SV_UTF8_NO_ENCODING
#define SV_UTF8_NO_ENCODING 0
#endif
#ifndef SV_CONST_RETURN
#define SV_CONST_RETURN 0
#endif
#ifndef SV_MUTABLE_RETURN
#define SV_MUTABLE_RETURN 0
#endif
#ifndef SV_SMAGIC
#define SV_SMAGIC 0
#endif
#ifndef SV_HAS_TRAILING_NUL
#define SV_HAS_TRAILING_NUL 0
#endif
#ifndef SV_COW_SHARED_HASH_KEYS
#define SV_COW_SHARED_HASH_KEYS 0
#endif
#if defined(PERL_USE_GCC_BRACE_GROUPS)
#ifndef sv_2pv_flags
#define sv_2pv_flags(sv, lp, flags) ({ SV *_sv = (sv); const I32 _flags = (flags); STRLEN *_lp = lp; _lp = _lp ? : &PL_na; (!(_flags & SV_GMAGIC) && SvGMAGICAL(_sv)) ? ({ char *_pv; SvGMAGICAL_off(_sv); _pv = sv_2pv(_sv, _lp); SvGMAGICAL_on(_sv); _pv; }) : sv_2pv(_sv, _lp); })
#endif
#ifndef sv_pvn_force_flags
#define sv_pvn_force_flags(sv, lp, flags) ({ SV *_sv = (sv); const I32 _flags = (flags); STRLEN *_lp = lp; _lp = _lp ? : &PL_na; (!(_flags & SV_GMAGIC) && SvGMAGICAL(_sv)) ? ({ char *_pv; SvGMAGICAL_off(_sv); _pv = sv_pvn_force(_sv, _lp); SvGMAGICAL_on(_sv); _pv; }) : sv_pvn_force(_sv, _lp); })
#endif
#else
#ifndef sv_2pv_flags
#define sv_2pv_flags(sv, lp, flags) ((PL_Sv = (sv)), (!((flags) & SV_GMAGIC) && SvGMAGICAL(PL_Sv)) ? (SvGMAGICAL_off(PL_Sv), (PL_Xpv = (XPV *)sv_2pv(PL_Sv, (lp) ? (lp) : &PL_na)), SvGMAGICAL_on(PL_Sv), (char *)PL_Xpv) : sv_2pv(PL_Sv, (lp) ? (lp) : &PL_na))
#endif
#ifndef sv_pvn_force_flags
#define sv_pvn_force_flags(sv, lp, flags) ((PL_Sv = (sv)), (!((flags) & SV_GMAGIC) && SvGMAGICAL(PL_Sv)) ? (SvGMAGICAL_off(PL_Sv), (PL_Xpv = (XPV *)sv_pvn_force(PL_Sv, (lp) ? (lp) : &PL_na)), SvGMAGICAL_on(PL_Sv), (char *)PL_Xpv) : sv_pvn_force(PL_Sv, (lp) ? (lp) : &PL_na))
#endif
#endif
#if (PERL_BCDVERSION < 0x5008008) || ( (PERL_BCDVERSION >= 0x5009000) && (PERL_BCDVERSION < 0x5009003) )
#define D_PPP_SVPV_NOLEN_LP_ARG &PL_na
#else
#define D_PPP_SVPV_NOLEN_LP_ARG 0
#endif
#ifndef SvPV_const
#define SvPV_const(sv, lp) SvPV_flags_const(sv, lp, SV_GMAGIC)
#endif
#ifndef SvPV_mutable
#define SvPV_mutable(sv, lp) SvPV_flags_mutable(sv, lp, SV_GMAGIC)
#endif
#ifndef SvPV_flags
#define SvPV_flags(sv, lp, flags) \
((SvFLAGS(sv) & (SVf_POK)) == SVf_POK \
? ((lp = SvCUR(sv)), SvPVX(sv)) : sv_2pv_flags(sv, &lp, flags))
#endif
#ifndef SvPV_flags_const
#define SvPV_flags_const(sv, lp, flags) \
((SvFLAGS(sv) & (SVf_POK)) == SVf_POK \
? ((lp = SvCUR(sv)), SvPVX_const(sv)) : \
(const char*) sv_2pv_flags(sv, &lp, flags|SV_CONST_RETURN))
#endif
#ifndef SvPV_flags_const_nolen
#define SvPV_flags_const_nolen(sv, flags) \
((SvFLAGS(sv) & (SVf_POK)) == SVf_POK \
? SvPVX_const(sv) : \
(const char*) sv_2pv_flags(sv, D_PPP_SVPV_NOLEN_LP_ARG, flags|SV_CONST_RETURN))
#endif
#ifndef SvPV_flags_mutable
#define SvPV_flags_mutable(sv, lp, flags) \
((SvFLAGS(sv) & (SVf_POK)) == SVf_POK \
? ((lp = SvCUR(sv)), SvPVX_mutable(sv)) : \
sv_2pv_flags(sv, &lp, flags|SV_MUTABLE_RETURN))
#endif
#ifndef SvPV_force
#define SvPV_force(sv, lp) SvPV_force_flags(sv, lp, SV_GMAGIC)
#endif
#ifndef SvPV_force_nolen
#define SvPV_force_nolen(sv) SvPV_force_flags_nolen(sv, SV_GMAGIC)
#endif
#ifndef SvPV_force_mutable
#define SvPV_force_mutable(sv, lp) SvPV_force_flags_mutable(sv, lp, SV_GMAGIC)
#endif
#ifndef SvPV_force_nomg
#define SvPV_force_nomg(sv, lp) SvPV_force_flags(sv, lp, 0)
#endif
#ifndef SvPV_force_nomg_nolen
#define SvPV_force_nomg_nolen(sv) SvPV_force_flags_nolen(sv, 0)
#endif
#ifndef SvPV_force_flags
#define SvPV_force_flags(sv, lp, flags) \
((SvFLAGS(sv) & (SVf_POK|SVf_THINKFIRST)) == SVf_POK \
? ((lp = SvCUR(sv)), SvPVX(sv)) : sv_pvn_force_flags(sv, &lp, flags))
#endif
#ifndef SvPV_force_flags_nolen
#define SvPV_force_flags_nolen(sv, flags) \
((SvFLAGS(sv) & (SVf_POK|SVf_THINKFIRST)) == SVf_POK \
? SvPVX(sv) : sv_pvn_force_flags(sv, D_PPP_SVPV_NOLEN_LP_ARG, flags))
#endif
#ifndef SvPV_force_flags_mutable
#define SvPV_force_flags_mutable(sv, lp, flags) \
((SvFLAGS(sv) & (SVf_POK|SVf_THINKFIRST)) == SVf_POK \
? ((lp = SvCUR(sv)), SvPVX_mutable(sv)) \
: sv_pvn_force_flags(sv, &lp, flags|SV_MUTABLE_RETURN))
#endif
#ifndef SvPV_nolen
#define SvPV_nolen(sv) \
((SvFLAGS(sv) & (SVf_POK)) == SVf_POK \
? SvPVX(sv) : sv_2pv_flags(sv, D_PPP_SVPV_NOLEN_LP_ARG, SV_GMAGIC))
#endif
#ifndef SvPV_nolen_const
#define SvPV_nolen_const(sv) \
((SvFLAGS(sv) & (SVf_POK)) == SVf_POK \
? SvPVX_const(sv) : sv_2pv_flags(sv, D_PPP_SVPV_NOLEN_LP_ARG, SV_GMAGIC|SV_CONST_RETURN))
#endif
#if defined(PERL_USE_GCC_BRACE_GROUPS)
#ifndef SvPVx_nolen_const
#define SvPVx_nolen_const(sv) ({SV *sV_ = (sv); SvPV_nolen_const(sV_); })
#endif
#else
#ifndef SvPVx_nolen_const
#define SvPVx_nolen_const(sv) (PL_Sv = sv, SvPV_nolen_const(PL_Sv))
#endif
#endif
#ifndef SvPV_nomg
#define SvPV_nomg(sv, lp) SvPV_flags(sv, lp, 0)
#endif
#ifndef SvPV_nomg_const
#define SvPV_nomg_const(sv, lp) SvPV_flags_const(sv, lp, 0)
#endif
#ifndef SvPV_nomg_const_nolen
#define SvPV_nomg_const_nolen(sv) SvPV_flags_const_nolen(sv, 0)
#endif
#ifndef SvPV_nomg_nolen
#define SvPV_nomg_nolen(sv) ((SvFLAGS(sv) & (SVf_POK)) == SVf_POK \
? SvPVX(sv) : sv_2pv_flags(sv, D_PPP_SVPV_NOLEN_LP_ARG, 0))
#endif
#ifndef SvPV_renew
#define SvPV_renew(sv,n) STMT_START { SvLEN_set(sv, n); \
SvPV_set((sv), (char *) saferealloc( \
(Malloc_t)SvPVX(sv), (MEM_SIZE)((n)))); \
} STMT_END
#endif
#ifndef SvPVCLEAR
#define SvPVCLEAR(sv) sv_setpvs((sv), "")
#endif
#ifndef WARN_ALL
#define WARN_ALL 0
#endif
#ifndef WARN_CLOSURE
#define WARN_CLOSURE 1
#endif
#ifndef WARN_DEPRECATED
#define WARN_DEPRECATED 2
#endif
#ifndef WARN_EXITING
#define WARN_EXITING 3
#endif
#ifndef WARN_GLOB
#define WARN_GLOB 4
#endif
#ifndef WARN_IO
#define WARN_IO 5
#endif
#ifndef WARN_CLOSED
#define WARN_CLOSED 6
#endif
#ifndef WARN_EXEC
#define WARN_EXEC 7
#endif
#ifndef WARN_LAYER
#define WARN_LAYER 8
#endif
#ifndef WARN_NEWLINE
#define WARN_NEWLINE 9
#endif
#ifndef WARN_PIPE
#define WARN_PIPE 10
#endif
#ifndef WARN_UNOPENED
#define WARN_UNOPENED 11
#endif
#ifndef WARN_MISC
#define WARN_MISC 12
#endif
#ifndef WARN_NUMERIC
#define WARN_NUMERIC 13
#endif
#ifndef WARN_ONCE
#define WARN_ONCE 14
#endif
#ifndef WARN_OVERFLOW
#define WARN_OVERFLOW 15
#endif
#ifndef WARN_PACK
#define WARN_PACK 16
#endif
#ifndef WARN_PORTABLE
#define WARN_PORTABLE 17
#endif
#ifndef WARN_RECURSION
#define WARN_RECURSION 18
#endif
#ifndef WARN_REDEFINE
#define WARN_REDEFINE 19
#endif
#ifndef WARN_REGEXP
#define WARN_REGEXP 20
#endif
#ifndef WARN_SEVERE
#define WARN_SEVERE 21
#endif
#ifndef WARN_DEBUGGING
#define WARN_DEBUGGING 22
#endif
#ifndef WARN_INPLACE
#define WARN_INPLACE 23
#endif
#ifndef WARN_INTERNAL
#define WARN_INTERNAL 24
#endif
#ifndef WARN_MALLOC
#define WARN_MALLOC 25
#endif
#ifndef WARN_SIGNAL
#define WARN_SIGNAL 26
#endif
#ifndef WARN_SUBSTR
#define WARN_SUBSTR 27
#endif
#ifndef WARN_SYNTAX
#define WARN_SYNTAX 28
#endif
#ifndef WARN_AMBIGUOUS
#define WARN_AMBIGUOUS 29
#endif
#ifndef WARN_BAREWORD
#define WARN_BAREWORD 30
#endif
#ifndef WARN_DIGIT
#define WARN_DIGIT 31
#endif
#ifndef WARN_PARENTHESIS
#define WARN_PARENTHESIS 32
#endif
#ifndef WARN_PRECEDENCE
#define WARN_PRECEDENCE 33
#endif
#ifndef WARN_PRINTF
#define WARN_PRINTF 34
#endif
#ifndef WARN_PROTOTYPE
#define WARN_PROTOTYPE 35
#endif
#ifndef WARN_QW
#define WARN_QW 36
#endif
#ifndef WARN_RESERVED
#define WARN_RESERVED 37
#endif
#ifndef WARN_SEMICOLON
#define WARN_SEMICOLON 38
#endif
#ifndef WARN_TAINT
#define WARN_TAINT 39
#endif
#ifndef WARN_THREADS
#define WARN_THREADS 40
#endif
#ifndef WARN_UNINITIALIZED
#define WARN_UNINITIALIZED 41
#endif
#ifndef WARN_UNPACK
#define WARN_UNPACK 42
#endif
#ifndef WARN_UNTIE
#define WARN_UNTIE 43
#endif
#ifndef WARN_UTF8
#define WARN_UTF8 44
#endif
#ifndef WARN_VOID
#define WARN_VOID 45
#endif
#ifndef WARN_ASSERTIONS
#define WARN_ASSERTIONS 46
#endif
#ifndef packWARN
#define packWARN(a) (a)
#endif
#ifndef packWARN2
#define packWARN2(a,b) (packWARN(a) << 8 | (b))
#endif
#ifndef packWARN3
#define packWARN3(a,b,c) (packWARN2(a,b) << 8 | (c))
#endif
#ifndef packWARN4
#define packWARN4(a,b,c,d) (packWARN3(a,b,c) << 8 | (d))
#endif
#ifndef ckWARN
#ifdef G_WARN_ON
#define ckWARN(a) (PL_dowarn & G_WARN_ON)
#else
#define ckWARN(a) PL_dowarn
#endif
#endif
#ifndef ckWARN2
#define ckWARN2(a,b) (ckWARN(a) || ckWARN(b))
#endif
#ifndef ckWARN3
#define ckWARN3(a,b,c) (ckWARN(c) || ckWARN2(a,b))
#endif
#ifndef ckWARN4
#define ckWARN4(a,b,c,d) (ckWARN(d) || ckWARN3(a,b,c))
#endif
#ifndef ckWARN_d
#ifdef isLEXWARN_off
#define ckWARN_d(a) (isLEXWARN_off || ckWARN(a))
#else
#define ckWARN_d(a) 1
#endif
#endif
#ifndef ckWARN2_d
#define ckWARN2_d(a,b) (ckWARN_d(a) || ckWARN_d(b))
#endif
#ifndef ckWARN3_d
#define ckWARN3_d(a,b,c) (ckWARN_d(c) || ckWARN2_d(a,b))
#endif
#ifndef ckWARN4_d
#define ckWARN4_d(a,b,c,d) (ckWARN_d(d) || ckWARN3_d(a,b,c))
#endif
#ifndef vwarner
#define vwarner(err, pat, argsp) \
STMT_START { SV *sv; \
PERL_UNUSED_ARG(err); \
sv = vnewSVpvf(pat, argsp); \
sv_2mortal(sv); \
warn("%s", SvPV_nolen(sv)); \
} STMT_END
#endif
#if (PERL_BCDVERSION >= 0x5004000) && !defined(warner)
#if defined(NEED_warner)
static void DPPP_(my_warner)(U32 err, const char * pat, ...);
static
#else
extern void DPPP_(my_warner)(U32 err, const char * pat, ...);
#endif
#if defined(NEED_warner) || defined(NEED_warner_GLOBAL)
#define Perl_warner DPPP_(my_warner)
void
DPPP_(my_warner)(U32 err, const char *pat, ...)
{
va_list args;
va_start(args, pat);
vwarner(err, pat, &args);
va_end(args);
}
#define warner Perl_warner
#define Perl_warner_nocontext Perl_warner
#endif
#endif
#if (PERL_BCDVERSION >= 0x5004000) && !defined(ck_warner)
#if defined(NEED_ck_warner)
static void DPPP_(my_ck_warner)(pTHX_ U32 err, const char * pat, ...);
static
#else
extern void DPPP_(my_ck_warner)(pTHX_ U32 err, const char * pat, ...);
#endif
#if defined(NEED_ck_warner) || defined(NEED_ck_warner_GLOBAL)
#define Perl_ck_warner DPPP_(my_ck_warner)
void
DPPP_(my_ck_warner)(pTHX_ U32 err, const char *pat, ...)
{
va_list args;
if ( ! ckWARN((err ) & 0xFF)
&& ! ckWARN((err >> 8) & 0xFF)
&& ! ckWARN((err >> 16) & 0xFF)
&& ! ckWARN((err >> 24) & 0xFF))
{
return;
}
va_start(args, pat);
vwarner(err, pat, &args);
va_end(args);
}
#define ck_warner Perl_ck_warner
#endif
#endif
#if (PERL_BCDVERSION >= 0x5004000) && !defined(ck_warner_d)
#if defined(NEED_ck_warner_d)
static void DPPP_(my_ck_warner_d)(pTHX_ U32 err, const char * pat, ...);
static
#else
extern void DPPP_(my_ck_warner_d)(pTHX_ U32 err, const char * pat, ...);
#endif
#if defined(NEED_ck_warner_d) || defined(NEED_ck_warner_d_GLOBAL)
#define Perl_ck_warner_d DPPP_(my_ck_warner_d)
void
DPPP_(my_ck_warner_d)(pTHX_ U32 err, const char *pat, ...)
{
va_list args;
if ( ! ckWARN_d((err ) & 0xFF)
&& ! ckWARN_d((err >> 8) & 0xFF)
&& ! ckWARN_d((err >> 16) & 0xFF)
&& ! ckWARN_d((err >> 24) & 0xFF))
{
return;
}
va_start(args, pat);
vwarner(err, pat, &args);
va_end(args);
}
#define ck_warner_d Perl_ck_warner_d
#endif
#endif
#ifndef IVdf
#if IVSIZE == LONGSIZE
#define IVdf "ld"
#define UVuf "lu"
#define UVof "lo"
#define UVxf "lx"
#define UVXf "lX"
#elif IVSIZE == INTSIZE
#define IVdf "d"
#define UVuf "u"
#define UVof "o"
#define UVxf "x"
#define UVXf "X"
#else
#error "cannot define IV/UV formats"
#endif
#endif
#ifndef NVef
#if defined(USE_LONG_DOUBLE) && defined(HAS_LONG_DOUBLE) && \
defined(PERL_PRIfldbl) && (PERL_BCDVERSION != 0x5006000)
#define NVef PERL_PRIeldbl
#define NVff PERL_PRIfldbl
#define NVgf PERL_PRIgldbl
#else
#define NVef "e"
#define NVff "f"
#define NVgf "g"
#endif
#endif
#ifndef sv_setuv
#define sv_setuv(sv, uv) \
STMT_START { \
UV TeMpUv = uv; \
if (TeMpUv <= IV_MAX) \
sv_setiv(sv, TeMpUv); \
else \
sv_setnv(sv, (double)TeMpUv); \
} STMT_END
#endif
#ifndef newSVuv
#define newSVuv(uv) ((uv) <= IV_MAX ? newSViv((IV)uv) : newSVnv((NV)uv))
#endif
#if defined(PERL_USE_GCC_BRACE_GROUPS)
#ifndef sv_2uv
#define sv_2uv(sv) ({ SV *_sv = (sv); (UV) (SvNOK(_sv) ? SvNV(_sv) : sv_2nv(_sv)); })
#endif
#else
#ifndef sv_2uv
#define sv_2uv(sv) ((PL_Sv = (sv)), (UV) (SvNOK(PL_Sv) ? SvNV(PL_Sv) : sv_2nv(PL_Sv)))
#endif
#endif
#ifndef SvUVX
#define SvUVX(sv) ((UV)SvIVX(sv))
#endif
#ifndef SvUVXx
#define SvUVXx(sv) SvUVX(sv)
#endif
#ifndef SvUV
#define SvUV(sv) (SvIOK(sv) ? SvUVX(sv) : sv_2uv(sv))
#endif
#if defined(PERL_USE_GCC_BRACE_GROUPS)
#ifndef SvUVx
#define SvUVx(sv) ({ SV *_sv = (sv)); SvUV(_sv); })
#endif
#else
#ifndef SvUVx
#define SvUVx(sv) ((PL_Sv = (sv)), SvUV(PL_Sv))
#endif
#endif
#ifndef sv_uv
#define sv_uv(sv) SvUVx(sv)
#endif
#if !defined(SvUOK) && defined(SvIOK_UV)
#define SvUOK(sv) SvIOK_UV(sv)
#endif
#ifndef XST_mUV
#define XST_mUV(i,v) (ST(i) = sv_2mortal(newSVuv(v)) )
#endif
#ifndef XSRETURN_UV
#define XSRETURN_UV(v) STMT_START { XST_mUV(0,v); XSRETURN(1); } STMT_END
#endif
#ifndef PUSHu
#define PUSHu(u) STMT_START { sv_setuv(TARG, (UV)(u)); PUSHTARG; } STMT_END
#endif
#ifndef XPUSHu
#define XPUSHu(u) STMT_START { sv_setuv(TARG, (UV)(u)); XPUSHTARG; } STMT_END
#endif
#if !defined(my_strnlen)
#if defined(NEED_my_strnlen)
static Size_t DPPP_(my_my_strnlen)(const char * str, Size_t maxlen);
static
#else
extern Size_t DPPP_(my_my_strnlen)(const char * str, Size_t maxlen);
#endif
#if defined(NEED_my_strnlen) || defined(NEED_my_strnlen_GLOBAL)
#define my_strnlen DPPP_(my_my_strnlen)
#define Perl_my_strnlen DPPP_(my_my_strnlen)
Size_t
DPPP_(my_my_strnlen)(const char *str, Size_t maxlen)
{
const char *p = str;
while(maxlen-- && *p)
p++;
return p - str;
}
#endif
#endif
#ifdef HAS_MEMCMP
#ifndef memNE
#define memNE(s1,s2,l) (memcmp(s1,s2,l))
#endif
#ifndef memEQ
#define memEQ(s1,s2,l) (!memcmp(s1,s2,l))
#endif
#else
#ifndef memNE
#define memNE(s1,s2,l) (bcmp(s1,s2,l))
#endif
#ifndef memEQ
#define memEQ(s1,s2,l) (!bcmp(s1,s2,l))
#endif
#endif
#ifndef memEQs
#define memEQs(s1, l, s2) \
(sizeof(s2)-1 == l && memEQ(s1, (s2 ""), (sizeof(s2)-1)))
#endif
#ifndef memNEs
#define memNEs(s1, l, s2) !memEQs(s1, l, s2)
#endif
#ifndef memCHRs
#define memCHRs(s, c) ((const char *) memchr("" s "" , c, sizeof(s)-1))
#endif
#ifndef MoveD
#define MoveD(s,d,n,t) memmove((char*)(d),(char*)(s), (n) * sizeof(t))
#endif
#ifndef CopyD
#define CopyD(s,d,n,t) memcpy((char*)(d),(char*)(s), (n) * sizeof(t))
#endif
#ifdef HAS_MEMSET
#ifndef ZeroD
#define ZeroD(d,n,t) memzero((char*)(d), (n) * sizeof(t))
#endif
#else
#ifndef ZeroD
#define ZeroD(d,n,t) ((void)memzero((char*)(d), (n) * sizeof(t)), d)
#endif
#endif
#ifndef PoisonWith
#define PoisonWith(d,n,t,b) (void)memset((char*)(d), (U8)(b), (n) * sizeof(t))
#endif
#ifndef PoisonNew
#define PoisonNew(d,n,t) PoisonWith(d,n,t,0xAB)
#endif
#ifndef PoisonFree
#define PoisonFree(d,n,t) PoisonWith(d,n,t,0xEF)
#endif
#ifndef Poison
#define Poison(d,n,t) PoisonFree(d,n,t)
#endif
#ifndef Newx
#define Newx(v,n,t) New(0,v,n,t)
#endif
#ifndef Newxc
#define Newxc(v,n,t,c) Newc(0,v,n,t,c)
#endif
#ifndef Newxz
#define Newxz(v,n,t) Newz(0,v,n,t)
#endif
#ifdef NEED_mess_sv
#define NEED_mess
#endif
#ifdef NEED_mess
#define NEED_mess_nocontext
#define NEED_vmess
#endif
#ifndef croak_sv
#if (PERL_BCDVERSION >= 0x5007003) || ( (PERL_BCDVERSION >= 0x5006001) && (PERL_BCDVERSION < 0x5007000) )
#if ( (PERL_BCDVERSION >= 0x5008000) && (PERL_BCDVERSION < 0x5008009) ) || ( (PERL_BCDVERSION >= 0x5009000) && (PERL_BCDVERSION < 0x5010001) )
#define D_PPP_FIX_UTF8_ERRSV_FOR_SV(sv) \
STMT_START { \
SV *_errsv = ERRSV; \
SvFLAGS(_errsv) = (SvFLAGS(_errsv) & ~SVf_UTF8) | \
(SvFLAGS(sv) & SVf_UTF8); \
} STMT_END
#else
#define D_PPP_FIX_UTF8_ERRSV_FOR_SV(sv) STMT_START {} STMT_END
#endif
PERL_STATIC_INLINE void D_PPP_croak_sv(SV *sv) {
dTHX;
SV *_sv = (sv);
if (SvROK(_sv)) {
sv_setsv(ERRSV, _sv);
croak(NULL);
} else {
D_PPP_FIX_UTF8_ERRSV_FOR_SV(_sv);
croak("%" SVf, SVfARG(_sv));
}
}
#define croak_sv(sv) D_PPP_croak_sv(sv)
#elif (PERL_BCDVERSION >= 0x5004000)
#define croak_sv(sv) croak("%" SVf, SVfARG(sv))
#else
#define croak_sv(sv) croak("%s", SvPV_nolen(sv))
#endif
#endif
#ifndef die_sv
#if defined(NEED_die_sv)
static OP * DPPP_(my_die_sv)(pTHX_ SV * baseex);
static
#else
extern OP * DPPP_(my_die_sv)(pTHX_ SV * baseex);
#endif
#if defined(NEED_die_sv) || defined(NEED_die_sv_GLOBAL)
#ifdef die_sv
#undef die_sv
#endif
#define die_sv(a) DPPP_(my_die_sv)(aTHX_ a)
#define Perl_die_sv DPPP_(my_die_sv)
OP *
DPPP_(my_die_sv)(pTHX_ SV *baseex)
{
croak_sv(baseex);
return (OP *)NULL;
}
#endif
#endif
#ifndef warn_sv
#if (PERL_BCDVERSION >= 0x5004000)
#define warn_sv(sv) warn("%" SVf, SVfARG(sv))
#else
#define warn_sv(sv) warn("%s", SvPV_nolen(sv))
#endif
#endif
#if ! defined vmess && (PERL_BCDVERSION >= 0x5004000)
#if defined(NEED_vmess)
static SV * DPPP_(my_vmess)(pTHX_ const char * pat, va_list * args);
static
#else
extern SV * DPPP_(my_vmess)(pTHX_ const char * pat, va_list * args);
#endif
#if defined(NEED_vmess) || defined(NEED_vmess_GLOBAL)
#ifdef vmess
#undef vmess
#endif
#define vmess(a,b) DPPP_(my_vmess)(aTHX_ a,b)
#define Perl_vmess DPPP_(my_vmess)
SV*
DPPP_(my_vmess)(pTHX_ const char* pat, va_list* args)
{
mess(pat, args);
return PL_mess_sv;
}
#endif
#endif
#if (PERL_BCDVERSION < 0x5006000) && (PERL_BCDVERSION >= 0x5004000)
#undef mess
#endif
#if !defined(mess_nocontext) && !defined(Perl_mess_nocontext) && (PERL_BCDVERSION >= 0x5004000)
#if defined(NEED_mess_nocontext)
static SV * DPPP_(my_mess_nocontext)(const char * pat, ...);
static
#else
extern SV * DPPP_(my_mess_nocontext)(const char * pat, ...);
#endif
#if defined(NEED_mess_nocontext) || defined(NEED_mess_nocontext_GLOBAL)
#define mess_nocontext DPPP_(my_mess_nocontext)
#define Perl_mess_nocontext DPPP_(my_mess_nocontext)
SV*
DPPP_(my_mess_nocontext)(const char* pat, ...)
{
dTHX;
SV *sv;
va_list args;
va_start(args, pat);
sv = vmess(pat, &args);
va_end(args);
return sv;
}
#endif
#endif
#ifndef mess
#if defined(NEED_mess)
static SV * DPPP_(my_mess)(pTHX_ const char * pat, ...);
static
#else
extern SV * DPPP_(my_mess)(pTHX_ const char * pat, ...);
#endif
#if defined(NEED_mess) || defined(NEED_mess_GLOBAL)
#define Perl_mess DPPP_(my_mess)
SV*
DPPP_(my_mess)(pTHX_ const char* pat, ...)
{
SV *sv;
va_list args;
va_start(args, pat);
sv = vmess(pat, &args);
va_end(args);
return sv;
}
#ifdef mess_nocontext
#define mess mess_nocontext
#else
#define mess Perl_mess_nocontext
#endif
#endif
#endif
#if ! defined mess_sv && (PERL_BCDVERSION >= 0x5004000)
#if defined(NEED_mess_sv)
static SV * DPPP_(my_mess_sv)(pTHX_ SV * basemsg, bool consume);
static
#else
extern SV * DPPP_(my_mess_sv)(pTHX_ SV * basemsg, bool consume);
#endif
#if defined(NEED_mess_sv) || defined(NEED_mess_sv_GLOBAL)
#ifdef mess_sv
#undef mess_sv
#endif
#define mess_sv(a,b) DPPP_(my_mess_sv)(aTHX_ a,b)
#define Perl_mess_sv DPPP_(my_mess_sv)
SV *
DPPP_(my_mess_sv)(pTHX_ SV *basemsg, bool consume)
{
SV *tmp;
SV *ret;
if (SvPOK(basemsg) && SvCUR(basemsg) && *(SvEND(basemsg)-1) == '\n') {
if (consume)
return basemsg;
ret = mess("");
SvSetSV_nosteal(ret, basemsg);
return ret;
}
if (consume) {
sv_catsv(basemsg, mess(""));
return basemsg;
}
ret = mess("");
tmp = newSVsv(ret);
SvSetSV_nosteal(ret, basemsg);
sv_catsv(ret, tmp);
sv_dec(tmp);
return ret;
}
#endif
#endif
#ifndef warn_nocontext
#define warn_nocontext warn
#endif
#ifndef croak_nocontext
#define croak_nocontext croak
#endif
#ifndef croak_no_modify
#define croak_no_modify() croak_nocontext("%s", PL_no_modify)
#define Perl_croak_no_modify() croak_no_modify()
#endif
#ifndef croak_memory_wrap
#if (PERL_BCDVERSION >= 0x5009002) || ( (PERL_BCDVERSION >= 0x5008006) && (PERL_BCDVERSION < 0x5009000) )
#define croak_memory_wrap() croak_nocontext("%s", PL_memory_wrap)
#else
#define croak_memory_wrap() croak_nocontext("panic: memory wrap")
#endif
#endif
#ifndef croak_xs_usage
#if defined(NEED_croak_xs_usage)
static void DPPP_(my_croak_xs_usage)(const CV * const cv, const char * const params);
static
#else
extern void DPPP_(my_croak_xs_usage)(const CV * const cv, const char * const params);
#endif
#if defined(NEED_croak_xs_usage) || defined(NEED_croak_xs_usage_GLOBAL)
#define croak_xs_usage DPPP_(my_croak_xs_usage)
#define Perl_croak_xs_usage DPPP_(my_croak_xs_usage)
#ifndef PERL_ARGS_ASSERT_CROAK_XS_USAGE
#define PERL_ARGS_ASSERT_CROAK_XS_USAGE assert(cv); assert(params)
void
DPPP_(my_croak_xs_usage)(const CV *const cv, const char *const params)
{
dTHX;
const GV *const gv = CvGV(cv);
PERL_ARGS_ASSERT_CROAK_XS_USAGE;
if (gv) {
const char *const gvname = GvNAME(gv);
const HV *const stash = GvSTASH(gv);
const char *const hvname = stash ? HvNAME(stash) : NULL;
if (hvname)
croak("Usage: %s::%s(%s)", hvname, gvname, params);
else
croak("Usage: %s(%s)", gvname, params);
} else {
croak("Usage: CODE(0x%" UVxf ")(%s)", PTR2UV(cv), params);
}
}
#endif
#endif
#endif
#ifndef mPUSHs
#define mPUSHs(s) PUSHs(sv_2mortal(s))
#endif
#ifndef PUSHmortal
#define PUSHmortal PUSHs(sv_newmortal())
#endif
#ifndef mPUSHp
#define mPUSHp(p,l) sv_setpvn(PUSHmortal, (p), (l))
#endif
#ifndef mPUSHn
#define mPUSHn(n) sv_setnv(PUSHmortal, (NV)(n))
#endif
#ifndef mPUSHi
#define mPUSHi(i) sv_setiv(PUSHmortal, (IV)(i))
#endif
#ifndef mPUSHu
#define mPUSHu(u) sv_setuv(PUSHmortal, (UV)(u))
#endif
#ifndef mXPUSHs
#define mXPUSHs(s) XPUSHs(sv_2mortal(s))
#endif
#ifndef XPUSHmortal
#define XPUSHmortal XPUSHs(sv_newmortal())
#endif
#ifndef mXPUSHp
#define mXPUSHp(p,l) STMT_START { EXTEND(sp,1); sv_setpvn(PUSHmortal, (p), (l)); } STMT_END
#endif
#ifndef mXPUSHn
#define mXPUSHn(n) STMT_START { EXTEND(sp,1); sv_setnv(PUSHmortal, (NV)(n)); } STMT_END
#endif
#ifndef mXPUSHi
#define mXPUSHi(i) STMT_START { EXTEND(sp,1); sv_setiv(PUSHmortal, (IV)(i)); } STMT_END
#endif
#ifndef mXPUSHu
#define mXPUSHu(u) STMT_START { EXTEND(sp,1); sv_setuv(PUSHmortal, (UV)(u)); } STMT_END
#endif
#ifndef call_sv
#define call_sv perl_call_sv
#endif
#ifndef call_pv
#define call_pv perl_call_pv
#endif
#ifndef call_argv
#define call_argv perl_call_argv
#endif
#ifndef call_method
#define call_method perl_call_method
#endif
#ifndef eval_sv
#define eval_sv perl_eval_sv
#endif
#if (PERL_BCDVERSION >= 0x5003098) && (PERL_BCDVERSION < 0x5006000)
#ifndef eval_pv
#define eval_pv perl_eval_pv
#endif
#endif
#if (PERL_BCDVERSION < 0x5006000)
#ifndef Perl_eval_sv
#define Perl_eval_sv perl_eval_sv
#endif
#if (PERL_BCDVERSION >= 0x5003098)
#ifndef Perl_eval_pv
#define Perl_eval_pv perl_eval_pv
#endif
#endif
#endif
#ifndef G_LIST
#define G_LIST G_ARRAY
#endif
#ifndef PERL_LOADMOD_DENY
#define PERL_LOADMOD_DENY 0x1
#endif
#ifndef PERL_LOADMOD_NOIMPORT
#define PERL_LOADMOD_NOIMPORT 0x2
#endif
#ifndef PERL_LOADMOD_IMPORT_OPS
#define PERL_LOADMOD_IMPORT_OPS 0x4
#endif
#if defined(PERL_USE_GCC_BRACE_GROUPS)
#define D_PPP_CROAK_IF_ERROR(cond) ({ \
SV *_errsv; \
( (cond) \
&& (_errsv = ERRSV) \
&& (SvROK(_errsv) || SvTRUE(_errsv)) \
&& (croak_sv(_errsv), 1)); \
})
#else
PERL_STATIC_INLINE void D_PPP_CROAK_IF_ERROR(int cond) {
dTHX;
SV *errsv;
if (!cond) return;
errsv = ERRSV;
if (SvROK(errsv) || SvTRUE(errsv)) croak_sv(errsv);
}
#define D_PPP_CROAK_IF_ERROR(cond) D_PPP_CROAK_IF_ERROR(cond)
#endif
#ifndef G_METHOD
#define G_METHOD 64
#ifdef call_sv
#undef call_sv
#endif
#if (PERL_BCDVERSION < 0x5006000)
#define call_sv(sv, flags) ((flags) & G_METHOD ? perl_call_method((char *) SvPV_nolen_const(sv), \
(flags) & ~G_METHOD) : perl_call_sv(sv, flags))
#else
#define call_sv(sv, flags) ((flags) & G_METHOD ? Perl_call_method(aTHX_ (char *) SvPV_nolen_const(sv), \
(flags) & ~G_METHOD) : Perl_call_sv(aTHX_ sv, flags))
#endif
#endif
#ifndef G_RETHROW
#define G_RETHROW 8192
#ifdef eval_sv
#undef eval_sv
#endif
#if defined(PERL_USE_GCC_BRACE_GROUPS)
#define eval_sv(sv, flags) ({ I32 _flags = (flags); I32 _ret = Perl_eval_sv(aTHX_ sv, (_flags & ~G_RETHROW)); D_PPP_CROAK_IF_ERROR(_flags & G_RETHROW); _ret; })
#else
#define eval_sv(sv, flags) ((PL_na = Perl_eval_sv(aTHX_ sv, ((flags) & ~G_RETHROW))), D_PPP_CROAK_IF_ERROR((flags) & G_RETHROW), (I32)PL_na)
#endif
#endif
#if (PERL_BCDVERSION < 0x5031002)
#ifdef eval_pv
#undef eval_pv
#if defined(PERL_USE_GCC_BRACE_GROUPS)
#define eval_pv(p, croak_on_error) ({ SV *_sv = Perl_eval_pv(aTHX_ p, 0); D_PPP_CROAK_IF_ERROR(croak_on_error); _sv; })
#else
#define eval_pv(p, croak_on_error) ((PL_Sv = Perl_eval_pv(aTHX_ p, 0)), D_PPP_CROAK_IF_ERROR(croak_on_error), PL_Sv)
#endif
#endif
#endif
#ifndef eval_pv
#if defined(NEED_eval_pv)
static SV * DPPP_(my_eval_pv)(const char * p, I32 croak_on_error);
static
#else
extern SV * DPPP_(my_eval_pv)(const char * p, I32 croak_on_error);
#endif
#if defined(NEED_eval_pv) || defined(NEED_eval_pv_GLOBAL)
#ifdef eval_pv
#undef eval_pv
#endif
#define eval_pv(a,b) DPPP_(my_eval_pv)(aTHX_ a,b)
#define Perl_eval_pv DPPP_(my_eval_pv)
SV*
DPPP_(my_eval_pv)(const char *p, I32 croak_on_error)
{
dSP;
SV* sv = newSVpv(p, 0);
PUSHMARK(sp);
eval_sv(sv, G_SCALAR);
SvREFCNT_dec(sv);
SPAGAIN;
sv = POPs;
PUTBACK;
D_PPP_CROAK_IF_ERROR(croak_on_error);
return sv;
}
#endif
#endif
#if ! defined(vload_module) && defined(start_subparse)
#if defined(NEED_vload_module)
static void DPPP_(my_vload_module)(U32 flags, SV * name, SV * ver, va_list * args);
static
#else
extern void DPPP_(my_vload_module)(U32 flags, SV * name, SV * ver, va_list * args);
#endif
#if defined(NEED_vload_module) || defined(NEED_vload_module_GLOBAL)
#ifdef vload_module
#undef vload_module
#endif
#define vload_module(a,b,c,d) DPPP_(my_vload_module)(aTHX_ a,b,c,d)
#define Perl_vload_module DPPP_(my_vload_module)
void
DPPP_(my_vload_module)(U32 flags, SV *name, SV *ver, va_list *args)
{
dTHR;
dVAR;
OP *veop, *imop;
OP * const modname = newSVOP(OP_CONST, 0, name);
SvREADONLY_off(((SVOP*)modname)->op_sv);
modname->op_private |= OPpCONST_BARE;
if (ver) {
veop = newSVOP(OP_CONST, 0, ver);
}
else
veop = NULL;
if (flags & PERL_LOADMOD_NOIMPORT) {
imop = sawparens(newNULLLIST());
}
else if (flags & PERL_LOADMOD_IMPORT_OPS) {
imop = va_arg(*args, OP*);
}
else {
SV *sv;
imop = NULL;
sv = va_arg(*args, SV*);
while (sv) {
imop = append_elem(OP_LIST, imop, newSVOP(OP_CONST, 0, sv));
sv = va_arg(*args, SV*);
}
}
{
const line_t ocopline = PL_copline;
COP * const ocurcop = PL_curcop;
const int oexpect = PL_expect;
utilize(!(flags & PERL_LOADMOD_DENY), start_subparse(FALSE, 0),
#if (PERL_BCDVERSION > 0x5003000)
veop,
#endif
modname, imop);
PL_expect = oexpect;
PL_copline = ocopline;
PL_curcop = ocurcop;
}
}
#endif
#endif
#ifndef load_module
#if defined(NEED_load_module)
static void DPPP_(my_load_module)(U32 flags, SV * name, SV * ver, ...);
static
#else
extern void DPPP_(my_load_module)(U32 flags, SV * name, SV * ver, ...);
#endif
#if defined(NEED_load_module) || defined(NEED_load_module_GLOBAL)
#ifdef load_module
#undef load_module
#endif
#define load_module DPPP_(my_load_module)
#define Perl_load_module DPPP_(my_load_module)
void
DPPP_(my_load_module)(U32 flags, SV *name, SV *ver, ...)
{
va_list args;
va_start(args, ver);
vload_module(flags, name, ver, &args);
va_end(args);
}
#endif
#endif
#ifndef newRV_inc
#define newRV_inc(sv) newRV(sv)
#endif
#ifndef newRV_noinc
#if defined(PERL_USE_GCC_BRACE_GROUPS)
#define newRV_noinc(sv) ({ SV *_sv = (SV *)newRV((sv)); SvREFCNT_dec((sv)); _sv; })
#else
#define newRV_noinc(sv) ((PL_Sv = (SV *)newRV((sv))), SvREFCNT_dec((sv)), PL_Sv)
#endif
#endif
#if defined(MULTIPLICITY) || defined(PERL_OBJECT) || \
defined(PERL_CAPI) || defined(PERL_IMPLICIT_CONTEXT)
#ifndef START_MY_CXT
#define START_MY_CXT
#if (PERL_BCDVERSION < 0x5004068)
#define dMY_CXT_SV \
SV *my_cxt_sv = get_sv(MY_CXT_KEY, FALSE)
#else
#define dMY_CXT_SV \
SV *my_cxt_sv = *hv_fetch(PL_modglobal, MY_CXT_KEY, \
sizeof(MY_CXT_KEY)-1, TRUE)
#endif
#define dMY_CXT \
dMY_CXT_SV; \
my_cxt_t *my_cxtp = INT2PTR(my_cxt_t*,SvUV(my_cxt_sv))
#define MY_CXT_INIT \
dMY_CXT_SV; \
\
my_cxt_t *my_cxtp = (my_cxt_t*)SvPVX(newSV(sizeof(my_cxt_t)-1));\
Zero(my_cxtp, 1, my_cxt_t); \
sv_setuv(my_cxt_sv, PTR2UV(my_cxtp))
#define MY_CXT (*my_cxtp)
#define pMY_CXT my_cxt_t *my_cxtp
#define pMY_CXT_ pMY_CXT,
#define _pMY_CXT ,pMY_CXT
#define aMY_CXT my_cxtp
#define aMY_CXT_ aMY_CXT,
#define _aMY_CXT ,aMY_CXT
#endif
#ifndef MY_CXT_CLONE
#define MY_CXT_CLONE \
dMY_CXT_SV; \
my_cxt_t *my_cxtp = (my_cxt_t*)SvPVX(newSV(sizeof(my_cxt_t)-1));\
Copy(INT2PTR(my_cxt_t*, SvUV(my_cxt_sv)), my_cxtp, 1, my_cxt_t);\
sv_setuv(my_cxt_sv, PTR2UV(my_cxtp))
#endif
#else
#ifndef START_MY_CXT
#define START_MY_CXT static my_cxt_t my_cxt;
#define dMY_CXT_SV dNOOP
#define dMY_CXT dNOOP
#define MY_CXT_INIT NOOP
#define MY_CXT my_cxt
#define pMY_CXT void
#define pMY_CXT_
#define _pMY_CXT
#define aMY_CXT
#define aMY_CXT_
#define _aMY_CXT
#endif
#ifndef MY_CXT_CLONE
#define MY_CXT_CLONE NOOP
#endif
#endif
#ifndef SvREFCNT_inc
#ifdef PERL_USE_GCC_BRACE_GROUPS
#define SvREFCNT_inc(sv) \
({ \
SV * const _sv = (SV*)(sv); \
if (_sv) \
(SvREFCNT(_sv))++; \
_sv; \
})
#else
#define SvREFCNT_inc(sv) \
((PL_Sv=(SV*)(sv)) ? (++(SvREFCNT(PL_Sv)),PL_Sv) : NULL)
#endif
#endif
#ifndef SvREFCNT_inc_simple
#ifdef PERL_USE_GCC_BRACE_GROUPS
#define SvREFCNT_inc_simple(sv) \
({ \
if (sv) \
(SvREFCNT(sv))++; \
(SV *)(sv); \
})
#else
#define SvREFCNT_inc_simple(sv) \
((sv) ? (SvREFCNT(sv)++,(SV*)(sv)) : NULL)
#endif
#endif
#ifndef SvREFCNT_inc_NN
#ifdef PERL_USE_GCC_BRACE_GROUPS
#define SvREFCNT_inc_NN(sv) \
({ \
SV * const _sv = (SV*)(sv); \
SvREFCNT(_sv)++; \
_sv; \
})
#else
#define SvREFCNT_inc_NN(sv) \
(PL_Sv=(SV*)(sv),++(SvREFCNT(PL_Sv)),PL_Sv)
#endif
#endif
#ifndef SvREFCNT_inc_void
#ifdef PERL_USE_GCC_BRACE_GROUPS
#define SvREFCNT_inc_void(sv) \
({ \
SV * const _sv = (SV*)(sv); \
if (_sv) \
(void)(SvREFCNT(_sv)++); \
})
#else
#define SvREFCNT_inc_void(sv) \
(void)((PL_Sv=(SV*)(sv)) ? ++(SvREFCNT(PL_Sv)) : 0)
#endif
#endif
#ifndef SvREFCNT_inc_simple_void
#define SvREFCNT_inc_simple_void(sv) STMT_START { if (sv) SvREFCNT(sv)++; } STMT_END
#endif
#ifndef SvREFCNT_inc_simple_NN
#define SvREFCNT_inc_simple_NN(sv) (++SvREFCNT(sv), (SV*)(sv))
#endif
#ifndef SvREFCNT_inc_void_NN
#define SvREFCNT_inc_void_NN(sv) (void)(++SvREFCNT((SV*)(sv)))
#endif
#ifndef SvREFCNT_inc_simple_void_NN
#define SvREFCNT_inc_simple_void_NN(sv) (void)(++SvREFCNT((SV*)(sv)))
#endif
#ifndef newSV_type
#if defined(PERL_USE_GCC_BRACE_GROUPS)
#define newSV_type(t) ({ SV *_sv = newSV(0); sv_upgrade(_sv, (t)); _sv; })
#else
#define newSV_type(t) ((PL_Sv = newSV(0)), sv_upgrade(PL_Sv, (t)), PL_Sv)
#endif
#endif
#if (PERL_BCDVERSION < 0x5006000)
#define D_PPP_CONSTPV_ARG(x) ((char *) (x))
#else
#define D_PPP_CONSTPV_ARG(x) (x)
#endif
#ifndef newSVpvn
#define newSVpvn(data,len) ((data) \
? ((len) ? newSVpv((data), (len)) : newSVpv("", 0)) \
: newSV(0))
#endif
#ifndef newSVpvn_utf8
#define newSVpvn_utf8(s, len, u) newSVpvn_flags((s), (len), (u) ? SVf_UTF8 : 0)
#endif
#ifndef SVf_UTF8
#define SVf_UTF8 0
#endif
#ifndef newSVpvn_flags
#if defined(PERL_USE_GCC_BRACE_GROUPS)
#define newSVpvn_flags(s, len, flags) \
({ \
SV * sv = newSVpvn(D_PPP_CONSTPV_ARG(s), (len)); \
SvFLAGS(sv) |= ((flags) & SVf_UTF8); \
if ((flags) & SVs_TEMP) sv = sv_2mortal(sv); \
sv; \
})
#else
PERL_STATIC_INLINE SV* D_PPP_newSVpvn_flags(const char *const s, const STRLEN len, const U32 flags)
{
dTHX;
SV * sv = newSVpvn(s, len);
SvFLAGS(sv) |= (flags & SVf_UTF8);
if (flags & SVs_TEMP) return sv_2mortal(sv);
return sv;
}
#define newSVpvn_flags(s, len, flags) D_PPP_newSVpvn_flags((s), (len), (flags))
#endif
#endif
#ifndef SV_NOSTEAL
#define SV_NOSTEAL 16
#endif
#if ( (PERL_BCDVERSION >= 0x5007003) && (PERL_BCDVERSION < 0x5008007) ) || ( (PERL_BCDVERSION >= 0x5009000) && (PERL_BCDVERSION < 0x5009002) )
#undef sv_setsv_flags
#if defined(PERL_USE_GCC_BRACE_GROUPS)
#define sv_setsv_flags(dstr, sstr, flags) \
STMT_START { \
if (((flags) & SV_NOSTEAL) && (sstr) && (SvFLAGS((SV *)(sstr)) & SVs_TEMP)) { \
SvTEMP_off((SV *)(sstr)); \
Perl_sv_setsv_flags(aTHX_ (dstr), (sstr), (flags) & ~SV_NOSTEAL); \
SvTEMP_on((SV *)(sstr)); \
} else { \
Perl_sv_setsv_flags(aTHX_ (dstr), (sstr), (flags) & ~SV_NOSTEAL); \
} \
} STMT_END
#else
#define sv_setsv_flags(dstr, sstr, flags) \
( \
(((flags) & SV_NOSTEAL) && (sstr) && (SvFLAGS((SV *)(sstr)) & SVs_TEMP)) ? ( \
SvTEMP_off((SV *)(sstr)), \
Perl_sv_setsv_flags(aTHX_ (dstr), (sstr), (flags) & ~SV_NOSTEAL), \
SvTEMP_on((SV *)(sstr)), \
1 \
) : ( \
Perl_sv_setsv_flags(aTHX_ (dstr), (sstr), (flags) & ~SV_NOSTEAL), \
1 \
) \
)
#endif
#endif
#if defined(PERL_USE_GCC_BRACE_GROUPS)
#ifndef sv_setsv_flags
#define sv_setsv_flags(dstr, sstr, flags) \
STMT_START { \
if (((flags) & SV_NOSTEAL) && (sstr) && (SvFLAGS((SV *)(sstr)) & SVs_TEMP)) { \
SvTEMP_off((SV *)(sstr)); \
if (!((flags) & SV_GMAGIC) && (sstr) && SvGMAGICAL((SV *)(sstr))) { \
SvGMAGICAL_off((SV *)(sstr)); \
sv_setsv((dstr), (sstr)); \
SvGMAGICAL_on((SV *)(sstr)); \
} else { \
sv_setsv((dstr), (sstr)); \
} \
SvTEMP_on((SV *)(sstr)); \
} else { \
if (!((flags) & SV_GMAGIC) && (sstr) && SvGMAGICAL((SV *)(sstr))) { \
SvGMAGICAL_off((SV *)(sstr)); \
sv_setsv((dstr), (sstr)); \
SvGMAGICAL_on((SV *)(sstr)); \
} else { \
sv_setsv((dstr), (sstr)); \
} \
} \
} STMT_END
#endif
#else
#ifndef sv_setsv_flags
#define sv_setsv_flags(dstr, sstr, flags) \
( \
(((flags) & SV_NOSTEAL) && (sstr) && (SvFLAGS((SV *)(sstr)) & SVs_TEMP)) ? ( \
SvTEMP_off((SV *)(sstr)), \
(!((flags) & SV_GMAGIC) && (sstr) && SvGMAGICAL((SV *)(sstr))) ? ( \
SvGMAGICAL_off((SV *)(sstr)), \
sv_setsv((dstr), (sstr)), \
SvGMAGICAL_on((SV *)(sstr)), \
1 \
) : ( \
sv_setsv((dstr), (sstr)), \
1 \
), \
SvTEMP_on((SV *)(sstr)), \
1 \
) : ( \
(!((flags) & SV_GMAGIC) && (sstr) && SvGMAGICAL((SV *)(sstr))) ? ( \
SvGMAGICAL_off((SV *)(sstr)), \
sv_setsv((dstr), (sstr)), \
SvGMAGICAL_on((SV *)(sstr)), \
1 \
) : ( \
sv_setsv((dstr), (sstr)), \
1 \
) \
) \
)
#endif
#endif
#ifndef newSVsv_flags
#if defined(PERL_USE_GCC_BRACE_GROUPS)
#define newSVsv_flags(sv, flags) \
({ \
SV *n= newSV(0); \
sv_setsv_flags(n, (sv), (flags)); \
n; \
})
#else
PERL_STATIC_INLINE SV* D_PPP_newSVsv_flags(SV *const old, I32 flags)
{
dTHX;
SV *n= newSV(0);
sv_setsv_flags(n, old, flags);
return n;
}
#define newSVsv_flags(sv, flags) D_PPP_newSVsv_flags(sv, flags)
#endif
#endif
#ifndef newSVsv_nomg
#define newSVsv_nomg(sv) newSVsv_flags((sv), SV_NOSTEAL)
#endif
#if (PERL_BCDVERSION >= 0x5017005)
#ifndef sv_mortalcopy_flags
#define sv_mortalcopy_flags(sv, flags) Perl_sv_mortalcopy_flags(aTHX_ (sv), (flags))
#endif
#else
#ifndef sv_mortalcopy_flags
#define sv_mortalcopy_flags(sv, flags) sv_2mortal(newSVsv_flags((sv), (flags)))
#endif
#endif
#ifndef SvMAGIC_set
#define SvMAGIC_set(sv, val) \
STMT_START { assert(SvTYPE(sv) >= SVt_PVMG); \
(((XPVMG*) SvANY(sv))->xmg_magic = (val)); } STMT_END
#endif
#if (PERL_BCDVERSION < 0x5009003)
#ifndef SvPVX_const
#define SvPVX_const(sv) ((const char*) (0 + SvPVX(sv)))
#endif
#ifndef SvPVX_mutable
#define SvPVX_mutable(sv) (0 + SvPVX(sv))
#endif
#ifndef SvRV_set
#define SvRV_set(sv, val) \
STMT_START { assert(SvTYPE(sv) >= SVt_RV); \
(((XRV*) SvANY(sv))->xrv_rv = (val)); } STMT_END
#endif
#else
#ifndef SvPVX_const
#define SvPVX_const(sv) ((const char*)((sv)->sv_u.svu_pv))
#endif
#ifndef SvPVX_mutable
#define SvPVX_mutable(sv) ((sv)->sv_u.svu_pv)
#endif
#ifndef SvRV_set
#define SvRV_set(sv, val) \
STMT_START { assert(SvTYPE(sv) >= SVt_RV); \
((sv)->sv_u.svu_rv = (val)); } STMT_END
#endif
#endif
#ifndef SvSTASH_set
#define SvSTASH_set(sv, val) \
STMT_START { assert(SvTYPE(sv) >= SVt_PVMG); \
(((XPVMG*) SvANY(sv))->xmg_stash = (val)); } STMT_END
#endif
#if (PERL_BCDVERSION < 0x5004000)
#ifndef SvUV_set
#define SvUV_set(sv, val) \
STMT_START { assert(SvTYPE(sv) == SVt_IV || SvTYPE(sv) >= SVt_PVIV); \
(((XPVIV*) SvANY(sv))->xiv_iv = (IV) (val)); } STMT_END
#endif
#else
#ifndef SvUV_set
#define SvUV_set(sv, val) \
STMT_START { assert(SvTYPE(sv) == SVt_IV || SvTYPE(sv) >= SVt_PVIV); \
(((XPVUV*) SvANY(sv))->xuv_uv = (val)); } STMT_END
#endif
#endif
#ifndef newSVpvn_share
#if defined(NEED_newSVpvn_share)
static SV * DPPP_(my_newSVpvn_share)(pTHX_ const char * s, I32 len, U32 hash);
static
#else
extern SV * DPPP_(my_newSVpvn_share)(pTHX_ const char * s, I32 len, U32 hash);
#endif
#if defined(NEED_newSVpvn_share) || defined(NEED_newSVpvn_share_GLOBAL)
#ifdef newSVpvn_share
#undef newSVpvn_share
#endif
#define newSVpvn_share(a,b,c) DPPP_(my_newSVpvn_share)(aTHX_ a,b,c)
#define Perl_newSVpvn_share DPPP_(my_newSVpvn_share)
SV *
DPPP_(my_newSVpvn_share)(pTHX_ const char *s, I32 len, U32 hash)
{
SV *sv;
if (len < 0)
len = -len;
if (!hash)
PERL_HASH(hash, (char*) s, len);
sv = newSVpvn((char *) s, len);
sv_upgrade(sv, SVt_PVIV);
SvIVX(sv) = hash;
SvREADONLY_on(sv);
SvPOK_on(sv);
return sv;
}
#endif
#endif
#ifndef SvSHARED_HASH
#define SvSHARED_HASH(sv) (0 + SvUVX(sv))
#endif
#ifndef HvNAME_get
#define HvNAME_get(hv) HvNAME(hv)
#endif
#ifndef HvNAMELEN_get
#define HvNAMELEN_get(hv) (HvNAME_get(hv) ? (I32)strlen(HvNAME_get(hv)) : 0)
#endif
#if (PERL_BCDVERSION >= 0x5009002) && (PERL_BCDVERSION <= 0x5009003)
#undef gv_fetchpvn_flags
#endif
#ifdef GV_NOADD_MASK
#define D_PPP_GV_NOADD_MASK GV_NOADD_MASK
#else
#define D_PPP_GV_NOADD_MASK 0xE0
#endif
#ifndef gv_fetchpvn_flags
#define gv_fetchpvn_flags(name, len, flags, sv_type) gv_fetchpv(SvPVX(sv_2mortal(newSVpvn((name), (len)))), ((flags) & D_PPP_GV_NOADD_MASK) ? FALSE : TRUE, (I32)(sv_type))
#endif
#ifndef GvSVn
#define GvSVn(gv) GvSV(gv)
#endif
#ifndef isGV_with_GP
#define isGV_with_GP(gv) isGV(gv)
#endif
#ifndef gv_fetchsv
#define gv_fetchsv(name, flags, svt) gv_fetchpv(SvPV_nolen_const(name), flags, svt)
#endif
#ifndef get_cvn_flags
#define get_cvn_flags(name, namelen, flags) get_cv(name, flags)
#endif
#ifndef gv_init_pvn
#define gv_init_pvn(gv, stash, ptr, len, flags) gv_init(gv, stash, ptr, len, flags & GV_ADDMULTI ? TRUE : FALSE)
#endif
#ifndef STR_WITH_LEN
#define STR_WITH_LEN(s) (s ""), (sizeof(s)-1)
#endif
#ifndef newSVpvs
#define newSVpvs(str) newSVpvn(str "", sizeof(str) - 1)
#endif
#ifndef newSVpvs_flags
#define newSVpvs_flags(str, flags) newSVpvn_flags(str "", sizeof(str) - 1, flags)
#endif
#ifndef newSVpvs_share
#define newSVpvs_share(str) newSVpvn_share(str "", sizeof(str) - 1, 0)
#endif
#ifndef sv_catpvs
#define sv_catpvs(sv, str) sv_catpvn(sv, str "", sizeof(str) - 1)
#endif
#ifndef sv_setpvs
#define sv_setpvs(sv, str) sv_setpvn(sv, str "", sizeof(str) - 1)
#endif
#ifndef hv_fetchs
#define hv_fetchs(hv, key, lval) hv_fetch(hv, key "", sizeof(key) - 1, lval)
#endif
#ifndef hv_stores
#define hv_stores(hv, key, val) hv_store(hv, key "", sizeof(key) - 1, val, 0)
#endif
#ifndef gv_fetchpvs
#define gv_fetchpvs(name, flags, svt) gv_fetchpvn_flags(name "", sizeof(name) - 1, flags, svt)
#endif
#ifndef gv_stashpvs
#define gv_stashpvs(name, flags) gv_stashpvn(name "", sizeof(name) - 1, flags)
#endif
#ifndef get_cvs
#define get_cvs(name, flags) get_cvn_flags(name "", sizeof(name)-1, flags)
#endif
#undef SvGETMAGIC
#ifndef SvGETMAGIC
#define SvGETMAGIC(x) ((void)(UNLIKELY(SvGMAGICAL(x)) && mg_get(x)))
#endif
#ifndef sv_catpvn_nomg
#define sv_catpvn_nomg sv_catpvn
#endif
#ifndef sv_catsv_nomg
#define sv_catsv_nomg sv_catsv
#endif
#ifndef sv_setsv_nomg
#define sv_setsv_nomg sv_setsv
#endif
#ifndef sv_pvn_nomg
#define sv_pvn_nomg sv_pvn
#endif
#ifdef SVf_IVisUV
#if defined(PERL_USE_GCC_BRACE_GROUPS)
#ifndef SvIV_nomg
#define SvIV_nomg(sv) (!SvGMAGICAL((sv)) ? SvIV((sv)) : ({ SV *_sviv = sv_mortalcopy_flags((sv), SV_NOSTEAL); IV _iv = SvIV(_sviv); SvFLAGS((sv)) = (SvFLAGS((sv)) & ~SVf_IVisUV) | (SvFLAGS(_sviv) & SVf_IVisUV); _iv; }))
#endif
#ifndef SvUV_nomg
#define SvUV_nomg(sv) (!SvGMAGICAL((sv)) ? SvUV((sv)) : ({ SV *_svuv = sv_mortalcopy_flags((sv), SV_NOSTEAL); UV _uv = SvUV(_svuv); SvFLAGS((sv)) = (SvFLAGS((sv)) & ~SVf_IVisUV) | (SvFLAGS(_svuv) & SVf_IVisUV); _uv; }))
#endif
#else
#ifndef SvIV_nomg
#define SvIV_nomg(sv) (!SvGMAGICAL((sv)) ? SvIV((sv)) : ((PL_Sv = sv_mortalcopy_flags((sv), SV_NOSTEAL)), sv_upgrade(PL_Sv, SVt_PVIV), (SvIVX(PL_Sv) = SvIV(PL_Sv)), (SvFLAGS((sv)) = (SvFLAGS((sv)) & ~SVf_IVisUV) | (SvFLAGS(PL_Sv) & SVf_IVisUV)), SvIVX(PL_Sv)))
#endif
#ifndef SvUV_nomg
#define SvUV_nomg(sv) (!SvGMAGICAL((sv)) ? SvIV((sv)) : ((PL_Sv = sv_mortalcopy_flags((sv), SV_NOSTEAL)), sv_upgrade(PL_Sv, SVt_PVIV), (SvUVX(PL_Sv) = SvUV(PL_Sv)), (SvFLAGS((sv)) = (SvFLAGS((sv)) & ~SVf_IVisUV) | (SvFLAGS(PL_Sv) & SVf_IVisUV)), SvUVX(PL_Sv)))
#endif
#endif
#else
#ifndef SvIV_nomg
#define SvIV_nomg(sv) (!SvGMAGICAL((sv)) ? SvIV((sv)) : SvIVx(sv_mortalcopy_flags((sv), SV_NOSTEAL)))
#endif
#ifndef SvUV_nomg
#define SvUV_nomg(sv) (!SvGMAGICAL((sv)) ? SvUV((sv)) : SvUVx(sv_mortalcopy_flags((sv), SV_NOSTEAL)))
#endif
#endif
#ifndef SvNV_nomg
#define SvNV_nomg(sv) (!SvGMAGICAL((sv)) ? SvNV((sv)) : SvNVx(sv_mortalcopy_flags((sv), SV_NOSTEAL)))
#endif
#ifndef SvTRUE_nomg
#define SvTRUE_nomg(sv) (!SvGMAGICAL((sv)) ? SvTRUE((sv)) : SvTRUEx(sv_mortalcopy_flags((sv), SV_NOSTEAL)))
#endif
#ifndef sv_catpv_mg
#define sv_catpv_mg(sv, ptr) \
STMT_START { \
SV *TeMpSv = sv; \
sv_catpv(TeMpSv,ptr); \
SvSETMAGIC(TeMpSv); \
} STMT_END
#endif
#ifndef sv_catpvn_mg
#define sv_catpvn_mg(sv, ptr, len) \
STMT_START { \
SV *TeMpSv = sv; \
sv_catpvn(TeMpSv,ptr,len); \
SvSETMAGIC(TeMpSv); \
} STMT_END
#endif
#ifndef sv_catsv_mg
#define sv_catsv_mg(dsv, ssv) \
STMT_START { \
SV *TeMpSv = dsv; \
sv_catsv(TeMpSv,ssv); \
SvSETMAGIC(TeMpSv); \
} STMT_END
#endif
#ifndef sv_setiv_mg
#define sv_setiv_mg(sv, i) \
STMT_START { \
SV *TeMpSv = sv; \
sv_setiv(TeMpSv,i); \
SvSETMAGIC(TeMpSv); \
} STMT_END
#endif
#ifndef sv_setnv_mg
#define sv_setnv_mg(sv, num) \
STMT_START { \
SV *TeMpSv = sv; \
sv_setnv(TeMpSv,num); \
SvSETMAGIC(TeMpSv); \
} STMT_END
#endif
#ifndef sv_setpv_mg
#define sv_setpv_mg(sv, ptr) \
STMT_START { \
SV *TeMpSv = sv; \
sv_setpv(TeMpSv,ptr); \
SvSETMAGIC(TeMpSv); \
} STMT_END
#endif
#ifndef sv_setpvn_mg
#define sv_setpvn_mg(sv, ptr, len) \
STMT_START { \
SV *TeMpSv = sv; \
sv_setpvn(TeMpSv,ptr,len); \
SvSETMAGIC(TeMpSv); \
} STMT_END
#endif
#ifndef sv_setsv_mg
#define sv_setsv_mg(dsv, ssv) \
STMT_START { \
SV *TeMpSv = dsv; \
sv_setsv(TeMpSv,ssv); \
SvSETMAGIC(TeMpSv); \
} STMT_END
#endif
#ifndef sv_setuv_mg
#define sv_setuv_mg(sv, i) \
STMT_START { \
SV *TeMpSv = sv; \
sv_setuv(TeMpSv,i); \
SvSETMAGIC(TeMpSv); \
} STMT_END
#endif
#ifndef sv_usepvn_mg
#define sv_usepvn_mg(sv, ptr, len) \
STMT_START { \
SV *TeMpSv = sv; \
sv_usepvn(TeMpSv,ptr,len); \
SvSETMAGIC(TeMpSv); \
} STMT_END
#endif
#ifndef SvVSTRING_mg
#define SvVSTRING_mg(sv) (SvMAGICAL(sv) ? mg_find(sv, PERL_MAGIC_vstring) : NULL)
#endif
#if (PERL_BCDVERSION < 0x5004000)
#elif (PERL_BCDVERSION < 0x5008000)
#define sv_magic_portable(sv, obj, how, name, namlen) \
STMT_START { \
SV *SvMp_sv = (sv); \
char *SvMp_name = (char *) (name); \
I32 SvMp_namlen = (namlen); \
if (SvMp_name && SvMp_namlen == 0) \
{ \
MAGIC *mg; \
sv_magic(SvMp_sv, obj, how, 0, 0); \
mg = SvMAGIC(SvMp_sv); \
mg->mg_len = -42;  \
mg->mg_ptr = SvMp_name; \
} \
else \
{ \
sv_magic(SvMp_sv, obj, how, SvMp_name, SvMp_namlen); \
} \
} STMT_END
#else
#define sv_magic_portable(a, b, c, d, e) sv_magic(a, b, c, d, e)
#endif
#if !defined(mg_findext)
#if defined(NEED_mg_findext)
static MAGIC * DPPP_(my_mg_findext)(const SV * sv, int type, const MGVTBL * vtbl);
static
#else
extern MAGIC * DPPP_(my_mg_findext)(const SV * sv, int type, const MGVTBL * vtbl);
#endif
#if defined(NEED_mg_findext) || defined(NEED_mg_findext_GLOBAL)
#define mg_findext DPPP_(my_mg_findext)
#define Perl_mg_findext DPPP_(my_mg_findext)
MAGIC *
DPPP_(my_mg_findext)(const SV * sv, int type, const MGVTBL *vtbl) {
if (sv) {
MAGIC *mg;
#ifdef AvPAD_NAMELIST
assert(!(SvTYPE(sv) == SVt_PVAV && AvPAD_NAMELIST(sv)));
#endif
for (mg = SvMAGIC (sv); mg; mg = mg->mg_moremagic) {
if (mg->mg_type == type && mg->mg_virtual == vtbl)
return mg;
}
}
return NULL;
}
#endif
#endif
#if !defined(sv_unmagicext)
#if defined(NEED_sv_unmagicext)
static int DPPP_(my_sv_unmagicext)(pTHX_ SV * const sv, const int type, const MGVTBL * vtbl);
static
#else
extern int DPPP_(my_sv_unmagicext)(pTHX_ SV * const sv, const int type, const MGVTBL * vtbl);
#endif
#if defined(NEED_sv_unmagicext) || defined(NEED_sv_unmagicext_GLOBAL)
#ifdef sv_unmagicext
#undef sv_unmagicext
#endif
#define sv_unmagicext(a,b,c) DPPP_(my_sv_unmagicext)(aTHX_ a,b,c)
#define Perl_sv_unmagicext DPPP_(my_sv_unmagicext)
int
DPPP_(my_sv_unmagicext)(pTHX_ SV *const sv, const int type, const MGVTBL *vtbl)
{
MAGIC* mg;
MAGIC** mgp;
if (SvTYPE(sv) < SVt_PVMG || !SvMAGIC(sv))
return 0;
mgp = &(SvMAGIC(sv));
for (mg = *mgp; mg; mg = *mgp) {
const MGVTBL* const virt = mg->mg_virtual;
if (mg->mg_type == type && virt == vtbl) {
*mgp = mg->mg_moremagic;
if (virt && virt->svt_free)
virt->svt_free(aTHX_ sv, mg);
if (mg->mg_ptr && mg->mg_type != PERL_MAGIC_regex_global) {
if (mg->mg_len > 0)
Safefree(mg->mg_ptr);
else if (mg->mg_len == HEf_SVKEY)
SvREFCNT_dec(MUTABLE_SV(mg->mg_ptr));
else if (mg->mg_type == PERL_MAGIC_utf8)
Safefree(mg->mg_ptr);
}
if (mg->mg_flags & MGf_REFCOUNTED)
SvREFCNT_dec(mg->mg_obj);
Safefree(mg);
}
else
mgp = &mg->mg_moremagic;
}
if (SvMAGIC(sv)) {
if (SvMAGICAL(sv))
mg_magical(sv);
}
else {
SvMAGICAL_off(sv);
SvFLAGS(sv) |= (SvFLAGS(sv) & (SVp_IOK|SVp_NOK|SVp_POK)) >> PRIVSHIFT;
}
return 0;
}
#endif
#endif
#ifdef USE_ITHREADS
#ifndef CopFILE
#define CopFILE(c) ((c)->cop_file)
#endif
#ifndef CopFILEGV
#define CopFILEGV(c) (CopFILE(c) ? gv_fetchfile(CopFILE(c)) : Nullgv)
#endif
#ifndef CopFILE_set
#define CopFILE_set(c,pv) ((c)->cop_file = savepv(pv))
#endif
#ifndef CopFILESV
#define CopFILESV(c) (CopFILE(c) ? GvSV(gv_fetchfile(CopFILE(c))) : Nullsv)
#endif
#ifndef CopFILEAV
#define CopFILEAV(c) (CopFILE(c) ? GvAV(gv_fetchfile(CopFILE(c))) : Nullav)
#endif
#ifndef CopSTASHPV
#define CopSTASHPV(c) ((c)->cop_stashpv)
#endif
#ifndef CopSTASHPV_set
#define CopSTASHPV_set(c,pv) ((c)->cop_stashpv = ((pv) ? savepv(pv) : Nullch))
#endif
#ifndef CopSTASH
#define CopSTASH(c) (CopSTASHPV(c) ? gv_stashpv(CopSTASHPV(c),GV_ADD) : Nullhv)
#endif
#ifndef CopSTASH_set
#define CopSTASH_set(c,hv) CopSTASHPV_set(c, (hv) ? HvNAME(hv) : Nullch)
#endif
#ifndef CopSTASH_eq
#define CopSTASH_eq(c,hv) ((hv) && (CopSTASHPV(c) == HvNAME(hv) \
|| (CopSTASHPV(c) && HvNAME(hv) \
&& strEQ(CopSTASHPV(c), HvNAME(hv)))))
#endif
#else
#ifndef CopFILEGV
#define CopFILEGV(c) ((c)->cop_filegv)
#endif
#ifndef CopFILEGV_set
#define CopFILEGV_set(c,gv) ((c)->cop_filegv = (GV*)SvREFCNT_inc(gv))
#endif
#ifndef CopFILE_set
#define CopFILE_set(c,pv) CopFILEGV_set((c), gv_fetchfile(pv))
#endif
#ifndef CopFILESV
#define CopFILESV(c) (CopFILEGV(c) ? GvSV(CopFILEGV(c)) : Nullsv)
#endif
#ifndef CopFILEAV
#define CopFILEAV(c) (CopFILEGV(c) ? GvAV(CopFILEGV(c)) : Nullav)
#endif
#ifndef CopFILE
#define CopFILE(c) (CopFILESV(c) ? SvPVX(CopFILESV(c)) : Nullch)
#endif
#ifndef CopSTASH
#define CopSTASH(c) ((c)->cop_stash)
#endif
#ifndef CopSTASH_set
#define CopSTASH_set(c,hv) ((c)->cop_stash = (hv))
#endif
#ifndef CopSTASHPV
#define CopSTASHPV(c) (CopSTASH(c) ? HvNAME(CopSTASH(c)) : Nullch)
#endif
#ifndef CopSTASHPV_set
#define CopSTASHPV_set(c,pv) CopSTASH_set((c), gv_stashpv(pv,GV_ADD))
#endif
#ifndef CopSTASH_eq
#define CopSTASH_eq(c,hv) (CopSTASH(c) == (hv))
#endif
#endif
#if (PERL_BCDVERSION >= 0x5006000)
#ifndef caller_cx
#if defined(NEED_caller_cx) || defined(NEED_caller_cx_GLOBAL)
static I32
DPPP_dopoptosub_at(const PERL_CONTEXT *cxstk, I32 startingblock)
{
I32 i;
for (i = startingblock; i >= 0; i--) {
const PERL_CONTEXT * const cx = &cxstk[i];
switch (CxTYPE(cx)) {
default:
continue;
case CXt_EVAL:
case CXt_SUB:
case CXt_FORMAT:
return i;
}
}
return i;
}
#endif
#if defined(NEED_caller_cx)
static const PERL_CONTEXT * DPPP_(my_caller_cx)(pTHX_ I32 level, const PERL_CONTEXT * * dbcxp);
static
#else
extern const PERL_CONTEXT * DPPP_(my_caller_cx)(pTHX_ I32 level, const PERL_CONTEXT * * dbcxp);
#endif
#if defined(NEED_caller_cx) || defined(NEED_caller_cx_GLOBAL)
#ifdef caller_cx
#undef caller_cx
#endif
#define caller_cx(a,b) DPPP_(my_caller_cx)(aTHX_ a,b)
#define Perl_caller_cx DPPP_(my_caller_cx)
const PERL_CONTEXT *
DPPP_(my_caller_cx)(pTHX_ I32 level, const PERL_CONTEXT **dbcxp)
{
I32 cxix = DPPP_dopoptosub_at(cxstack, cxstack_ix);
const PERL_CONTEXT *cx;
const PERL_CONTEXT *ccstack = cxstack;
const PERL_SI *top_si = PL_curstackinfo;
for (;;) {
while (cxix < 0 && top_si->si_type != PERLSI_MAIN) {
top_si = top_si->si_prev;
ccstack = top_si->si_cxstack;
cxix = DPPP_dopoptosub_at(ccstack, top_si->si_cxix);
}
if (cxix < 0)
return NULL;
if (PL_DBsub && GvCV(PL_DBsub) && cxix >= 0 &&
ccstack[cxix].blk_sub.cv == GvCV(PL_DBsub))
level++;
if (!level--)
break;
cxix = DPPP_dopoptosub_at(ccstack, cxix - 1);
}
cx = &ccstack[cxix];
if (dbcxp) *dbcxp = cx;
if (CxTYPE(cx) == CXt_SUB || CxTYPE(cx) == CXt_FORMAT) {
const I32 dbcxix = DPPP_dopoptosub_at(ccstack, cxix - 1);
if (PL_DBsub && GvCV(PL_DBsub) && dbcxix >= 0 && ccstack[dbcxix].blk_sub.cv == GvCV(PL_DBsub))
cx = &ccstack[dbcxix];
}
return cx;
}
#endif
#endif
#endif
#ifndef IN_PERL_COMPILETIME
#define IN_PERL_COMPILETIME (PL_curcop == &PL_compiling)
#endif
#ifndef IN_LOCALE_RUNTIME
#define IN_LOCALE_RUNTIME (PL_curcop->op_private & HINT_LOCALE)
#endif
#ifndef IN_LOCALE_COMPILETIME
#define IN_LOCALE_COMPILETIME (PL_hints & HINT_LOCALE)
#endif
#ifndef IN_LOCALE
#define IN_LOCALE (IN_PERL_COMPILETIME ? IN_LOCALE_COMPILETIME : IN_LOCALE_RUNTIME)
#endif
#ifndef IS_NUMBER_IN_UV
#define IS_NUMBER_IN_UV 0x01
#endif
#ifndef IS_NUMBER_GREATER_THAN_UV_MAX
#define IS_NUMBER_GREATER_THAN_UV_MAX 0x02
#endif
#ifndef IS_NUMBER_NOT_INT
#define IS_NUMBER_NOT_INT 0x04
#endif
#ifndef IS_NUMBER_NEG
#define IS_NUMBER_NEG 0x08
#endif
#ifndef IS_NUMBER_INFINITY
#define IS_NUMBER_INFINITY 0x10
#endif
#ifndef IS_NUMBER_NAN
#define IS_NUMBER_NAN 0x20
#endif
#ifndef GROK_NUMERIC_RADIX
#define GROK_NUMERIC_RADIX(sp, send) grok_numeric_radix(sp, send)
#endif
#ifndef PERL_SCAN_GREATER_THAN_UV_MAX
#define PERL_SCAN_GREATER_THAN_UV_MAX 0x02
#endif
#ifndef PERL_SCAN_SILENT_ILLDIGIT
#define PERL_SCAN_SILENT_ILLDIGIT 0x04
#endif
#ifndef PERL_SCAN_ALLOW_UNDERSCORES
#define PERL_SCAN_ALLOW_UNDERSCORES 0x01
#endif
#ifndef PERL_SCAN_DISALLOW_PREFIX
#define PERL_SCAN_DISALLOW_PREFIX 0x02
#endif
#ifndef grok_numeric_radix
#if defined(NEED_grok_numeric_radix)
static bool DPPP_(my_grok_numeric_radix)(pTHX_ const char * * sp, const char * send);
static
#else
extern bool DPPP_(my_grok_numeric_radix)(pTHX_ const char * * sp, const char * send);
#endif
#if defined(NEED_grok_numeric_radix) || defined(NEED_grok_numeric_radix_GLOBAL)
#ifdef grok_numeric_radix
#undef grok_numeric_radix
#endif
#define grok_numeric_radix(a,b) DPPP_(my_grok_numeric_radix)(aTHX_ a,b)
#define Perl_grok_numeric_radix DPPP_(my_grok_numeric_radix)
bool
DPPP_(my_grok_numeric_radix)(pTHX_ const char **sp, const char *send)
{
#ifdef USE_LOCALE_NUMERIC
#ifdef PL_numeric_radix_sv
if (PL_numeric_radix_sv && IN_LOCALE) {
STRLEN len;
char* radix = SvPV(PL_numeric_radix_sv, len);
if (*sp + len <= send && memEQ(*sp, radix, len)) {
*sp += len;
return TRUE;
}
}
#else
#include <locale.h>
dTHR;
struct lconv *lc = localeconv();
char *radix = lc->decimal_point;
if (radix && IN_LOCALE) {
STRLEN len = strlen(radix);
if (*sp + len <= send && memEQ(*sp, radix, len)) {
*sp += len;
return TRUE;
}
}
#endif
#endif
if (*sp < send && **sp == '.') {
++*sp;
return TRUE;
}
return FALSE;
}
#endif
#endif
#ifndef grok_number
#if defined(NEED_grok_number)
static int DPPP_(my_grok_number)(pTHX_ const char * pv, STRLEN len, UV * valuep);
static
#else
extern int DPPP_(my_grok_number)(pTHX_ const char * pv, STRLEN len, UV * valuep);
#endif
#if defined(NEED_grok_number) || defined(NEED_grok_number_GLOBAL)
#ifdef grok_number
#undef grok_number
#endif
#define grok_number(a,b,c) DPPP_(my_grok_number)(aTHX_ a,b,c)
#define Perl_grok_number DPPP_(my_grok_number)
int
DPPP_(my_grok_number)(pTHX_ const char *pv, STRLEN len, UV *valuep)
{
const char *s = pv;
const char *send = pv + len;
const UV max_div_10 = UV_MAX / 10;
const char max_mod_10 = UV_MAX % 10;
int numtype = 0;
int sawinf = 0;
int sawnan = 0;
while (s < send && isSPACE(*s))
s++;
if (s == send) {
return 0;
} else if (*s == '-') {
s++;
numtype = IS_NUMBER_NEG;
}
else if (*s == '+')
s++;
if (s == send)
return 0;
if (isDIGIT(*s)) {
UV value = *s - '0';
if (++s < send) {
int digit = *s - '0';
if (digit >= 0 && digit <= 9) {
value = value * 10 + digit;
if (++s < send) {
digit = *s - '0';
if (digit >= 0 && digit <= 9) {
value = value * 10 + digit;
if (++s < send) {
digit = *s - '0';
if (digit >= 0 && digit <= 9) {
value = value * 10 + digit;
if (++s < send) {
digit = *s - '0';
if (digit >= 0 && digit <= 9) {
value = value * 10 + digit;
if (++s < send) {
digit = *s - '0';
if (digit >= 0 && digit <= 9) {
value = value * 10 + digit;
if (++s < send) {
digit = *s - '0';
if (digit >= 0 && digit <= 9) {
value = value * 10 + digit;
if (++s < send) {
digit = *s - '0';
if (digit >= 0 && digit <= 9) {
value = value * 10 + digit;
if (++s < send) {
digit = *s - '0';
if (digit >= 0 && digit <= 9) {
value = value * 10 + digit;
if (++s < send) {
digit = *s - '0';
while (digit >= 0 && digit <= 9
&& (value < max_div_10
|| (value == max_div_10
&& digit <= max_mod_10))) {
value = value * 10 + digit;
if (++s < send)
digit = *s - '0';
else
break;
}
if (digit >= 0 && digit <= 9
&& (s < send)) {
do {
s++;
} while (s < send && isDIGIT(*s));
numtype |=
IS_NUMBER_GREATER_THAN_UV_MAX;
goto skip_value;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
numtype |= IS_NUMBER_IN_UV;
if (valuep)
*valuep = value;
skip_value:
if (GROK_NUMERIC_RADIX(&s, send)) {
numtype |= IS_NUMBER_NOT_INT;
while (s < send && isDIGIT(*s))
s++;
}
}
else if (GROK_NUMERIC_RADIX(&s, send)) {
numtype |= IS_NUMBER_NOT_INT | IS_NUMBER_IN_UV;
if (s < send && isDIGIT(*s)) {
do {
s++;
} while (s < send && isDIGIT(*s));
if (valuep) {
*valuep = 0;
}
}
else
return 0;
} else if (*s == 'I' || *s == 'i') {
s++; if (s == send || (*s != 'N' && *s != 'n')) return 0;
s++; if (s == send || (*s != 'F' && *s != 'f')) return 0;
s++; if (s < send && (*s == 'I' || *s == 'i')) {
s++; if (s == send || (*s != 'N' && *s != 'n')) return 0;
s++; if (s == send || (*s != 'I' && *s != 'i')) return 0;
s++; if (s == send || (*s != 'T' && *s != 't')) return 0;
s++; if (s == send || (*s != 'Y' && *s != 'y')) return 0;
s++;
}
sawinf = 1;
} else if (*s == 'N' || *s == 'n') {
s++; if (s == send || (*s != 'A' && *s != 'a')) return 0;
s++; if (s == send || (*s != 'N' && *s != 'n')) return 0;
s++;
sawnan = 1;
} else
return 0;
if (sawinf) {
numtype &= IS_NUMBER_NEG;
numtype |= IS_NUMBER_INFINITY | IS_NUMBER_NOT_INT;
} else if (sawnan) {
numtype &= IS_NUMBER_NEG;
numtype |= IS_NUMBER_NAN | IS_NUMBER_NOT_INT;
} else if (s < send) {
if (*s == 'e' || *s == 'E') {
numtype &= IS_NUMBER_NEG;
numtype |= IS_NUMBER_NOT_INT;
s++;
if (s < send && (*s == '-' || *s == '+'))
s++;
if (s < send && isDIGIT(*s)) {
do {
s++;
} while (s < send && isDIGIT(*s));
}
else
return 0;
}
}
while (s < send && isSPACE(*s))
s++;
if (s >= send)
return numtype;
if (len == 10 && memEQ(pv, "0 but true", 10)) {
if (valuep)
*valuep = 0;
return IS_NUMBER_IN_UV;
}
return 0;
}
#endif
#endif
#ifndef grok_bin
#if defined(NEED_grok_bin)
static UV DPPP_(my_grok_bin)(pTHX_ const char * start, STRLEN * len_p, I32 * flags, NV * result);
static
#else
extern UV DPPP_(my_grok_bin)(pTHX_ const char * start, STRLEN * len_p, I32 * flags, NV * result);
#endif
#if defined(NEED_grok_bin) || defined(NEED_grok_bin_GLOBAL)
#ifdef grok_bin
#undef grok_bin
#endif
#define grok_bin(a,b,c,d) DPPP_(my_grok_bin)(aTHX_ a,b,c,d)
#define Perl_grok_bin DPPP_(my_grok_bin)
UV
DPPP_(my_grok_bin)(pTHX_ const char *start, STRLEN *len_p, I32 *flags, NV *result)
{
const char *s = start;
STRLEN len = *len_p;
UV value = 0;
NV value_nv = 0;
const UV max_div_2 = UV_MAX / 2;
bool allow_underscores = *flags & PERL_SCAN_ALLOW_UNDERSCORES;
bool overflowed = FALSE;
if (!(*flags & PERL_SCAN_DISALLOW_PREFIX)) {
if (len >= 1) {
if (s[0] == 'b') {
s++;
len--;
}
else if (len >= 2 && s[0] == '0' && s[1] == 'b') {
s+=2;
len-=2;
}
}
}
for (; len-- && *s; s++) {
char bit = *s;
if (bit == '0' || bit == '1') {
redo:
if (!overflowed) {
if (value <= max_div_2) {
value = (value << 1) | (bit - '0');
continue;
}
warn("Integer overflow in binary number");
overflowed = TRUE;
value_nv = (NV) value;
}
value_nv *= 2.0;
value_nv += (NV)(bit - '0');
continue;
}
if (bit == '_' && len && allow_underscores && (bit = s[1])
&& (bit == '0' || bit == '1'))
{
--len;
++s;
goto redo;
}
if (!(*flags & PERL_SCAN_SILENT_ILLDIGIT))
warn("Illegal binary digit '%c' ignored", *s);
break;
}
if ( ( overflowed && value_nv > 4294967295.0)
#if UVSIZE > 4
|| (!overflowed && value > 0xffffffff )
#endif
) {
warn("Binary number > 0b11111111111111111111111111111111 non-portable");
}
*len_p = s - start;
if (!overflowed) {
*flags = 0;
return value;
}
*flags = PERL_SCAN_GREATER_THAN_UV_MAX;
if (result)
*result = value_nv;
return UV_MAX;
}
#endif
#endif
#ifndef grok_hex
#if defined(NEED_grok_hex)
static UV DPPP_(my_grok_hex)(pTHX_ const char * start, STRLEN * len_p, I32 * flags, NV * result);
static
#else
extern UV DPPP_(my_grok_hex)(pTHX_ const char * start, STRLEN * len_p, I32 * flags, NV * result);
#endif
#if defined(NEED_grok_hex) || defined(NEED_grok_hex_GLOBAL)
#ifdef grok_hex
#undef grok_hex
#endif
#define grok_hex(a,b,c,d) DPPP_(my_grok_hex)(aTHX_ a,b,c,d)
#define Perl_grok_hex DPPP_(my_grok_hex)
UV
DPPP_(my_grok_hex)(pTHX_ const char *start, STRLEN *len_p, I32 *flags, NV *result)
{
const char *s = start;
STRLEN len = *len_p;
UV value = 0;
NV value_nv = 0;
const UV max_div_16 = UV_MAX / 16;
bool allow_underscores = *flags & PERL_SCAN_ALLOW_UNDERSCORES;
bool overflowed = FALSE;
const char *xdigit;
if (!(*flags & PERL_SCAN_DISALLOW_PREFIX)) {
if (len >= 1) {
if (s[0] == 'x') {
s++;
len--;
}
else if (len >= 2 && s[0] == '0' && s[1] == 'x') {
s+=2;
len-=2;
}
}
}
for (; len-- && *s; s++) {
xdigit = strchr((char *) PL_hexdigit, *s);
if (xdigit) {
redo:
if (!overflowed) {
if (value <= max_div_16) {
value = (value << 4) | ((xdigit - PL_hexdigit) & 15);
continue;
}
warn("Integer overflow in hexadecimal number");
overflowed = TRUE;
value_nv = (NV) value;
}
value_nv *= 16.0;
value_nv += (NV)((xdigit - PL_hexdigit) & 15);
continue;
}
if (*s == '_' && len && allow_underscores && s[1]
&& (xdigit = strchr((char *) PL_hexdigit, s[1])))
{
--len;
++s;
goto redo;
}
if (!(*flags & PERL_SCAN_SILENT_ILLDIGIT))
warn("Illegal hexadecimal digit '%c' ignored", *s);
break;
}
if ( ( overflowed && value_nv > 4294967295.0)
#if UVSIZE > 4
|| (!overflowed && value > 0xffffffff )
#endif
) {
warn("Hexadecimal number > 0xffffffff non-portable");
}
*len_p = s - start;
if (!overflowed) {
*flags = 0;
return value;
}
*flags = PERL_SCAN_GREATER_THAN_UV_MAX;
if (result)
*result = value_nv;
return UV_MAX;
}
#endif
#endif
#ifndef grok_oct
#if defined(NEED_grok_oct)
static UV DPPP_(my_grok_oct)(pTHX_ const char * start, STRLEN * len_p, I32 * flags, NV * result);
static
#else
extern UV DPPP_(my_grok_oct)(pTHX_ const char * start, STRLEN * len_p, I32 * flags, NV * result);
#endif
#if defined(NEED_grok_oct) || defined(NEED_grok_oct_GLOBAL)
#ifdef grok_oct
#undef grok_oct
#endif
#define grok_oct(a,b,c,d) DPPP_(my_grok_oct)(aTHX_ a,b,c,d)
#define Perl_grok_oct DPPP_(my_grok_oct)
UV
DPPP_(my_grok_oct)(pTHX_ const char *start, STRLEN *len_p, I32 *flags, NV *result)
{
const char *s = start;
STRLEN len = *len_p;
UV value = 0;
NV value_nv = 0;
const UV max_div_8 = UV_MAX / 8;
bool allow_underscores = *flags & PERL_SCAN_ALLOW_UNDERSCORES;
bool overflowed = FALSE;
for (; len-- && *s; s++) {
int digit = *s - '0';
if (digit >= 0 && digit <= 7) {
redo:
if (!overflowed) {
if (value <= max_div_8) {
value = (value << 3) | digit;
continue;
}
warn("Integer overflow in octal number");
overflowed = TRUE;
value_nv = (NV) value;
}
value_nv *= 8.0;
value_nv += (NV)digit;
continue;
}
if (digit == ('_' - '0') && len && allow_underscores
&& (digit = s[1] - '0') && (digit >= 0 && digit <= 7))
{
--len;
++s;
goto redo;
}
if (digit == 8 || digit == 9) {
if (!(*flags & PERL_SCAN_SILENT_ILLDIGIT))
warn("Illegal octal digit '%c' ignored", *s);
}
break;
}
if ( ( overflowed && value_nv > 4294967295.0)
#if UVSIZE > 4
|| (!overflowed && value > 0xffffffff )
#endif
) {
warn("Octal number > 037777777777 non-portable");
}
*len_p = s - start;
if (!overflowed) {
*flags = 0;
return value;
}
*flags = PERL_SCAN_GREATER_THAN_UV_MAX;
if (result)
*result = value_nv;
return UV_MAX;
}
#endif
#endif
#if !defined(my_snprintf)
#if defined(NEED_my_snprintf)
static int DPPP_(my_my_snprintf)(char * buffer, const Size_t len, const char * format, ...);
static
#else
extern int DPPP_(my_my_snprintf)(char * buffer, const Size_t len, const char * format, ...);
#endif
#if defined(NEED_my_snprintf) || defined(NEED_my_snprintf_GLOBAL)
#define my_snprintf DPPP_(my_my_snprintf)
#define Perl_my_snprintf DPPP_(my_my_snprintf)
int
DPPP_(my_my_snprintf)(char *buffer, const Size_t len, const char *format, ...)
{
dTHX;
int retval;
va_list ap;
va_start(ap, format);
#ifdef HAS_VSNPRINTF
retval = vsnprintf(buffer, len, format, ap);
#else
retval = vsprintf(buffer, format, ap);
#endif
va_end(ap);
if (retval < 0 || (len > 0 && (Size_t)retval >= len))
Perl_croak(aTHX_ "panic: my_snprintf buffer overflow");
return retval;
}
#endif
#endif
#if !defined(my_sprintf)
#if defined(NEED_my_sprintf)
static int DPPP_(my_my_sprintf)(char * buffer, const char * pat, ...);
static
#else
extern int DPPP_(my_my_sprintf)(char * buffer, const char * pat, ...);
#endif
#if defined(NEED_my_sprintf) || defined(NEED_my_sprintf_GLOBAL)
#define my_sprintf DPPP_(my_my_sprintf)
int
DPPP_(my_my_sprintf)(char *buffer, const char* pat, ...)
{
va_list args;
va_start(args, pat);
vsprintf(buffer, pat, args);
va_end(args);
return strlen(buffer);
}
#endif
#endif
#ifdef NO_XSLOCKS
#ifdef dJMPENV
#define dXCPT dJMPENV; int rEtV = 0
#define XCPT_TRY_START JMPENV_PUSH(rEtV); if (rEtV == 0)
#define XCPT_TRY_END JMPENV_POP;
#define XCPT_CATCH if (rEtV != 0)
#define XCPT_RETHROW JMPENV_JUMP(rEtV)
#else
#define dXCPT Sigjmp_buf oldTOP; int rEtV = 0
#define XCPT_TRY_START Copy(top_env, oldTOP, 1, Sigjmp_buf); rEtV = Sigsetjmp(top_env, 1); if (rEtV == 0)
#define XCPT_TRY_END Copy(oldTOP, top_env, 1, Sigjmp_buf);
#define XCPT_CATCH if (rEtV != 0)
#define XCPT_RETHROW Siglongjmp(top_env, rEtV)
#endif
#endif
#if !defined(my_strlcat)
#if defined(NEED_my_strlcat)
static Size_t DPPP_(my_my_strlcat)(char * dst, const char * src, Size_t size);
static
#else
extern Size_t DPPP_(my_my_strlcat)(char * dst, const char * src, Size_t size);
#endif
#if defined(NEED_my_strlcat) || defined(NEED_my_strlcat_GLOBAL)
#define my_strlcat DPPP_(my_my_strlcat)
#define Perl_my_strlcat DPPP_(my_my_strlcat)
Size_t
DPPP_(my_my_strlcat)(char *dst, const char *src, Size_t size)
{
Size_t used, length, copy;
used = strlen(dst);
length = strlen(src);
if (size > 0 && used < size - 1) {
copy = (length >= size - used) ? size - used - 1 : length;
memcpy(dst + used, src, copy);
dst[used + copy] = '\0';
}
return used + length;
}
#endif
#endif
#if !defined(my_strlcpy)
#if defined(NEED_my_strlcpy)
static Size_t DPPP_(my_my_strlcpy)(char * dst, const char * src, Size_t size);
static
#else
extern Size_t DPPP_(my_my_strlcpy)(char * dst, const char * src, Size_t size);
#endif
#if defined(NEED_my_strlcpy) || defined(NEED_my_strlcpy_GLOBAL)
#define my_strlcpy DPPP_(my_my_strlcpy)
#define Perl_my_strlcpy DPPP_(my_my_strlcpy)
Size_t
DPPP_(my_my_strlcpy)(char *dst, const char *src, Size_t size)
{
Size_t length, copy;
length = strlen(src);
if (size > 0) {
copy = (length >= size) ? size - 1 : length;
memcpy(dst, src, copy);
dst[copy] = '\0';
}
return length;
}
#endif
#endif
#ifdef SVf_UTF8
#ifndef SvUTF8
#define SvUTF8(sv) (SvFLAGS(sv) & SVf_UTF8)
#endif
#endif
#if (PERL_BCDVERSION == 0x5019001)
#undef UTF8f
#endif
#ifdef SVf_UTF8
#ifndef UTF8f
#define UTF8f SVf
#endif
#ifndef UTF8fARG
#define UTF8fARG(u,l,p) newSVpvn_flags((p), (l), ((u) ? SVf_UTF8 : 0) | SVs_TEMP)
#endif
#endif
#define D_PPP_MIN(a,b) (((a) <= (b)) ? (a) : (b))
#ifndef UNICODE_REPLACEMENT
#define UNICODE_REPLACEMENT 0xFFFD
#endif
#ifdef UTF8_MAXLEN
#ifndef UTF8_MAXBYTES
#define UTF8_MAXBYTES UTF8_MAXLEN
#endif
#endif
#ifndef UTF_START_MARK
#define UTF_START_MARK(len) \
(((len) > 7) ? 0xFF : (0xFF & (0xFE << (7-(len)))))
#endif
#if (PERL_BCDVERSION < 0x5018000)
#undef UTF8_MAXBYTES_CASE
#endif
#if 'A' == 65
#define D_PPP_BYTE_INFO_BITS 6
#ifndef UTF8_MAXBYTES_CASE
#define UTF8_MAXBYTES_CASE 13
#endif
#else
#define D_PPP_BYTE_INFO_BITS 5
#ifndef UTF8_MAXBYTES_CASE
#define UTF8_MAXBYTES_CASE 15
#endif
#endif
#ifndef UTF_ACCUMULATION_SHIFT
#define UTF_ACCUMULATION_SHIFT D_PPP_BYTE_INFO_BITS
#endif
#ifdef NATIVE_TO_UTF
#ifndef NATIVE_UTF8_TO_I8
#define NATIVE_UTF8_TO_I8(c) NATIVE_TO_UTF(c)
#endif
#else
#ifndef NATIVE_UTF8_TO_I8
#define NATIVE_UTF8_TO_I8(c) (c)
#endif
#endif
#ifdef UTF_TO_NATIVE
#ifndef I8_TO_NATIVE_UTF8
#define I8_TO_NATIVE_UTF8(c) UTF_TO_NATIVE(c)
#endif
#else
#ifndef I8_TO_NATIVE_UTF8
#define I8_TO_NATIVE_UTF8(c) (c)
#endif
#endif
#ifndef UTF_START_MASK
#define UTF_START_MASK(len) \
(((len) >= 7) ? 0x00 : (0x1F >> ((len)-2)))
#endif
#ifndef UTF_IS_CONTINUATION_MASK
#define UTF_IS_CONTINUATION_MASK \
((U8) (0xFF << UTF_ACCUMULATION_SHIFT))
#endif
#ifndef UTF_CONTINUATION_MARK
#define UTF_CONTINUATION_MARK \
(UTF_IS_CONTINUATION_MASK & 0xB0)
#endif
#ifndef UTF_MIN_START_BYTE
#define UTF_MIN_START_BYTE \
((UTF_CONTINUATION_MARK >> UTF_ACCUMULATION_SHIFT) | UTF_START_MARK(2))
#endif
#ifndef UTF_MIN_ABOVE_LATIN1_BYTE
#define UTF_MIN_ABOVE_LATIN1_BYTE \
((0x100 >> UTF_ACCUMULATION_SHIFT) | UTF_START_MARK(2))
#endif
#if (PERL_BCDVERSION < 0x5007000)
#undef UTF8_IS_DOWNGRADEABLE_START
#endif
#ifndef UTF8_IS_DOWNGRADEABLE_START
#define UTF8_IS_DOWNGRADEABLE_START(c) \
inRANGE(NATIVE_UTF8_TO_I8(c), \
UTF_MIN_START_BYTE, UTF_MIN_ABOVE_LATIN1_BYTE - 1)
#endif
#ifndef UTF_CONTINUATION_MASK
#define UTF_CONTINUATION_MASK \
((U8) ((1U << UTF_ACCUMULATION_SHIFT) - 1))
#endif
#ifndef UTF8_ACCUMULATE
#define UTF8_ACCUMULATE(base, added) \
(((base) << UTF_ACCUMULATION_SHIFT) \
| ((NATIVE_UTF8_TO_I8(added)) \
& UTF_CONTINUATION_MASK))
#endif
#ifndef UTF8_ALLOW_ANYUV
#define UTF8_ALLOW_ANYUV 0
#endif
#ifndef UTF8_ALLOW_EMPTY
#define UTF8_ALLOW_EMPTY 0x0001
#endif
#ifndef UTF8_ALLOW_CONTINUATION
#define UTF8_ALLOW_CONTINUATION 0x0002
#endif
#ifndef UTF8_ALLOW_NON_CONTINUATION
#define UTF8_ALLOW_NON_CONTINUATION 0x0004
#endif
#ifndef UTF8_ALLOW_SHORT
#define UTF8_ALLOW_SHORT 0x0008
#endif
#ifndef UTF8_ALLOW_LONG
#define UTF8_ALLOW_LONG 0x0010
#endif
#ifndef UTF8_ALLOW_OVERFLOW
#define UTF8_ALLOW_OVERFLOW 0x0080
#endif
#ifndef UTF8_ALLOW_ANY
#define UTF8_ALLOW_ANY ( UTF8_ALLOW_CONTINUATION \
|UTF8_ALLOW_NON_CONTINUATION \
|UTF8_ALLOW_SHORT \
|UTF8_ALLOW_LONG \
|UTF8_ALLOW_OVERFLOW)
#endif
#if defined UTF8SKIP
#undef UTF8_SAFE_SKIP
#undef UTF8_CHK_SKIP
#ifndef UTF8_SAFE_SKIP
#define UTF8_SAFE_SKIP(s, e) ( \
((((e) - (s)) <= 0) \
? 0 \
: D_PPP_MIN(((e) - (s)), UTF8SKIP(s))))
#endif
#ifndef UTF8_CHK_SKIP
#define UTF8_CHK_SKIP(s) \
(s[0] == '\0' ? 1 : ((U8) D_PPP_MIN(my_strnlen((char *) (s), UTF8SKIP(s)), \
UTF8SKIP(s))))
#endif
#ifndef UTF8_SKIP
#define UTF8_SKIP(s) UTF8SKIP(s)
#endif
#endif
#if 'A' == 65
#ifndef UTF8_IS_INVARIANT
#define UTF8_IS_INVARIANT(c) isASCII(c)
#endif
#else
#ifndef UTF8_IS_INVARIANT
#define UTF8_IS_INVARIANT(c) (isASCII(c) || isCNTRL_L1(c))
#endif
#endif
#ifndef UVCHR_IS_INVARIANT
#define UVCHR_IS_INVARIANT(c) UTF8_IS_INVARIANT(c)
#endif
#ifdef UVCHR_IS_INVARIANT
#if 'A' != 65 || UVSIZE < 8
#define D_PPP_UVCHR_SKIP_UPPER(c) 7
#else
#define D_PPP_UVCHR_SKIP_UPPER(c) \
(((WIDEST_UTYPE) (c)) < \
(((WIDEST_UTYPE) 1) << (6 * D_PPP_BYTE_INFO_BITS)) ? 7 : 13)
#endif
#ifndef UVCHR_SKIP
#define UVCHR_SKIP(c) \
UVCHR_IS_INVARIANT(c) ? 1 : \
(WIDEST_UTYPE) (c) < (32 * (1U << ( D_PPP_BYTE_INFO_BITS))) ? 2 : \
(WIDEST_UTYPE) (c) < (16 * (1U << (2 * D_PPP_BYTE_INFO_BITS))) ? 3 : \
(WIDEST_UTYPE) (c) < ( 8 * (1U << (3 * D_PPP_BYTE_INFO_BITS))) ? 4 : \
(WIDEST_UTYPE) (c) < ( 4 * (1U << (4 * D_PPP_BYTE_INFO_BITS))) ? 5 : \
(WIDEST_UTYPE) (c) < ( 2 * (1U << (5 * D_PPP_BYTE_INFO_BITS))) ? 6 : \
D_PPP_UVCHR_SKIP_UPPER(c)
#endif
#endif
#ifdef is_ascii_string
#ifndef is_invariant_string
#define is_invariant_string(s,l) is_ascii_string(s,l)
#endif
#ifndef is_utf8_invariant_string
#define is_utf8_invariant_string(s,l) is_ascii_string(s,l)
#endif
#endif
#ifdef ibcmp_utf8
#ifndef foldEQ_utf8
#define foldEQ_utf8(s1,pe1,l1,u1,s2,pe2,l2,u2) \
cBOOL(! ibcmp_utf8(s1,pe1,l1,u1,s2,pe2,l2,u2))
#endif
#endif
#if defined(is_utf8_string) && defined(UTF8SKIP)
#ifndef isUTF8_CHAR
#define isUTF8_CHAR(s, e) ( \
(e) <= (s) || ! is_utf8_string(s, UTF8_SAFE_SKIP(s, e)) \
? 0 \
: UTF8SKIP(s))
#endif
#endif
#if 'A' == 65
#ifndef BOM_UTF8
#define BOM_UTF8 "\xEF\xBB\xBF"
#endif
#ifndef REPLACEMENT_CHARACTER_UTF8
#define REPLACEMENT_CHARACTER_UTF8 "\xEF\xBF\xBD"
#endif
#elif '^' == 95
#ifndef BOM_UTF8
#define BOM_UTF8 "\xDD\x73\x66\x73"
#endif
#ifndef REPLACEMENT_CHARACTER_UTF8
#define REPLACEMENT_CHARACTER_UTF8 "\xDD\x73\x73\x71"
#endif
#elif '^' == 176
#ifndef BOM_UTF8
#define BOM_UTF8 "\xDD\x72\x65\x72"
#endif
#ifndef REPLACEMENT_CHARACTER_UTF8
#define REPLACEMENT_CHARACTER_UTF8 "\xDD\x72\x72\x70"
#endif
#else
#error Unknown character set
#endif
#if (PERL_BCDVERSION < 0x5035010)
#undef utf8_to_uvchr_buf
#endif
#if (PERL_BCDVERSION >= 0x5006001) && ! defined(utf8_to_uvchr_buf)
#if defined(utf8n_to_uvchr) || defined(utf8_to_uvchr) || defined(utf8_to_uv)
#if defined(utf8n_to_uvchr)
#define D_PPP_utf8_to_uvchr_buf_callee utf8n_to_uvchr
#elif  \
defined(utf8_to_uv) && defined(utf8_to_uv_simple)
#define D_PPP_utf8_to_uvchr_buf_callee utf8_to_uv
#elif defined(utf8_to_uvchr)
#define D_PPP_utf8_to_uvchr_buf_callee(s, curlen, retlen, flags) \
utf8_to_uvchr((U8 *)(s), (retlen))
#else
#define D_PPP_utf8_to_uvchr_buf_callee(s, curlen, retlen, flags) \
utf8_to_uv((U8 *)(s), (retlen))
#endif
#endif
#if defined(NEED_utf8_to_uvchr_buf)
static UV DPPP_(my_utf8_to_uvchr_buf)(pTHX_ const U8 * s, const U8 * send, STRLEN * retlen);
static
#else
extern UV DPPP_(my_utf8_to_uvchr_buf)(pTHX_ const U8 * s, const U8 * send, STRLEN * retlen);
#endif
#if defined(NEED_utf8_to_uvchr_buf) || defined(NEED_utf8_to_uvchr_buf_GLOBAL)
#ifdef utf8_to_uvchr_buf
#undef utf8_to_uvchr_buf
#endif
#define utf8_to_uvchr_buf(a,b,c) DPPP_(my_utf8_to_uvchr_buf)(aTHX_ a,b,c)
#define Perl_utf8_to_uvchr_buf DPPP_(my_utf8_to_uvchr_buf)
UV
DPPP_(my_utf8_to_uvchr_buf)(pTHX_ const U8 *s, const U8 *send, STRLEN *retlen)
{
#if (PERL_BCDVERSION >= 0x5031004)
#if (PERL_BCDVERSION != 0x5035009)
if (send <= s) s = send = (U8 *) "?";
return Perl_utf8_to_uvchr_buf_helper(aTHX_ s, send, retlen);
#else
if (send > s) return Perl_utf8_to_uvchr_buf_helper(aTHX_ s, send, retlen);
if (! ckWARN_d(WARN_UTF8)) {
if (retlen) *retlen = 0;
return UNICODE_REPLACEMENT;
}
else {
s = send = (U8 *) "?";
(void) Perl__utf8n_to_uvchr_msgs_helper(s, 0, NULL, 0, NULL, NULL);
if (retlen) *retlen = (STRLEN) -1;
return 0;
}
#endif
#else
UV ret;
STRLEN curlen;
bool overflows = 0;
const U8 *cur_s = s;
const bool do_warnings = ckWARN_d(WARN_UTF8);
#if (PERL_BCDVERSION < 0x5026000) && ! defined(EBCDIC)
STRLEN overflow_length = 0;
#endif
if (send > s) {
curlen = send - s;
}
else {
assert(0);
curlen = 0;
if (! do_warnings) {
if (retlen) *retlen = 0;
return UNICODE_REPLACEMENT;
}
}
#if (PERL_BCDVERSION < 0x5026000) && ! defined(EBCDIC)
if (curlen > 0 && UNLIKELY(*s >= 0xFE)) {
if (sizeof(ret) < 8) {
overflows = 1;
overflow_length = (*s == 0xFE) ? 7 : 13;
}
else {
const U8 highest[] =
"\xFF\x80\x87\xBF\xBF\xBF\xBF\xBF\xBF\xBF\xBF\xBF\xBF";
const U8 *cur_h = highest;
for (cur_s = s; cur_s < send; cur_s++, cur_h++) {
if (UNLIKELY(*cur_s == *cur_h)) {
continue;
}
overflows = *cur_s > *cur_h;
break;
}
overflow_length = 13;
}
}
if (UNLIKELY(overflows)) {
ret = 0;
if (! do_warnings && retlen) {
*retlen = overflow_length;
}
}
else
#endif
ret = D_PPP_utf8_to_uvchr_buf_callee(
(U8 *)
s, curlen, retlen, (UTF8_ALLOW_ANYUV
& ~(UTF8_ALLOW_LONG|UTF8_ALLOW_EMPTY)));
#if (PERL_BCDVERSION >= 0x5026000) && (PERL_BCDVERSION < 0x5028000)
if (UNLIKELY(ret > IV_MAX)) {
overflows = 1;
}
#endif
if (UNLIKELY(overflows)) {
if (! do_warnings) {
if (retlen) {
*retlen = D_PPP_MIN(*retlen, UTF8SKIP(s));
*retlen = D_PPP_MIN(*retlen, curlen);
}
return UNICODE_REPLACEMENT;
}
else {
Perl_warner(aTHX_ packWARN(WARN_UTF8),
"Malformed UTF-8 character (overflow at 0x%" UVxf
", byte 0x%02x, after start byte 0x%02x)",
ret, *cur_s, *s);
if (retlen) {
*retlen = (STRLEN) -1;
}
return 0;
}
}
if (UNLIKELY(ret == 0 && (curlen == 0 || *s != '\0'))) {
if (do_warnings) {
if (retlen) {
*retlen = (STRLEN) -1;
}
}
else {
ret = D_PPP_utf8_to_uvchr_buf_callee(
(U8 *)
s, curlen, retlen, UTF8_ALLOW_ANY);
ret = UNICODE_REPLACEMENT;
#if (PERL_BCDVERSION < 0x5016000)
if (retlen && (IV) *retlen >= 0) {
unsigned int i = 1;
*retlen = D_PPP_MIN(*retlen, curlen);
*retlen = D_PPP_MIN(*retlen, UTF8SKIP(s));
do {
#ifdef UTF8_IS_CONTINUATION
if (! UTF8_IS_CONTINUATION(s[i]))
#else
if (s[i] < 0x80 || s[i] > 0xBF)
#endif
{
*retlen = i;
break;
}
} while (++i < *retlen);
}
#endif
}
}
return ret;
#endif
}
#endif
#endif
#if defined(UTF8SKIP) && defined(utf8_to_uvchr_buf)
#undef utf8_to_uvchr
#ifndef utf8_to_uvchr
#define utf8_to_uvchr(s, lp) \
((*(s) == '\0') \
? utf8_to_uvchr_buf(s,((s)+1), lp)  \
: utf8_to_uvchr_buf(s, (s) + UTF8_CHK_SKIP(s), (lp)))
#endif
#endif
#ifdef sv_len_utf8
#if (PERL_BCDVERSION >= 0x5017005)
#ifndef sv_len_utf8_nomg
#if defined(PERL_USE_GCC_BRACE_GROUPS)
#define sv_len_utf8_nomg(sv) \
({ \
SV *sv_ = (sv); \
sv_len_utf8(!SvGMAGICAL(sv_) \
? sv_ \
: sv_mortalcopy_flags(sv_, SV_NOSTEAL)); \
})
#else
PERL_STATIC_INLINE STRLEN D_PPP_sv_len_utf8_nomg(SV * sv)
{
dTHX;
if (SvGMAGICAL(sv))
return sv_len_utf8(sv_mortalcopy_flags(sv,
SV_NOSTEAL));
else return sv_len_utf8(sv);
}
#define sv_len_utf8_nomg(sv) D_PPP_sv_len_utf8_nomg(sv)
#endif
#endif
#else
#undef sv_len_utf8
#if defined(PERL_USE_GCC_BRACE_GROUPS)
#define sv_len_utf8_nomg(sv) \
({ \
SV *sv2 = (sv); \
STRLEN len; \
if (SvUTF8(sv2)) { \
if (SvGMAGICAL(sv2)) \
len = Perl_sv_len_utf8(aTHX_ \
sv_mortalcopy_flags(sv2, \
SV_NOSTEAL));\
else \
len = Perl_sv_len_utf8(aTHX_ sv2); \
} \
else SvPV_nomg(sv2, len); \
len; \
})
#define sv_len_utf8(sv) ({ SV *_sv1 = (sv); \
SvGETMAGIC(_sv1); \
sv_len_utf8_nomg(_sv1); \
})
#else
PERL_STATIC_INLINE STRLEN D_PPP_sv_len_utf8_nomg(SV * sv)
{
dTHX;
STRLEN len;
if (SvUTF8(sv)) {
if (SvGMAGICAL(sv))
len = Perl_sv_len_utf8(aTHX_
sv_mortalcopy_flags(sv,
SV_NOSTEAL));
else
len = Perl_sv_len_utf8(aTHX_ sv);
}
else SvPV_nomg(sv, len);
return len;
}
#define sv_len_utf8_nomg(sv) D_PPP_sv_len_utf8_nomg(sv)
PERL_STATIC_INLINE STRLEN D_PPP_sv_len_utf8(SV * sv)
{
dTHX;
SvGETMAGIC(sv);
return sv_len_utf8_nomg(sv);
}
#define sv_len_utf8(sv) D_PPP_sv_len_utf8(sv)
#endif
#endif
#endif
#ifndef PERL_PV_ESCAPE_QUOTE
#define PERL_PV_ESCAPE_QUOTE 0x0001
#endif
#ifndef PERL_PV_PRETTY_QUOTE
#define PERL_PV_PRETTY_QUOTE PERL_PV_ESCAPE_QUOTE
#endif
#ifndef PERL_PV_PRETTY_ELLIPSES
#define PERL_PV_PRETTY_ELLIPSES 0x0002
#endif
#ifndef PERL_PV_PRETTY_LTGT
#define PERL_PV_PRETTY_LTGT 0x0004
#endif
#ifndef PERL_PV_ESCAPE_FIRSTCHAR
#define PERL_PV_ESCAPE_FIRSTCHAR 0x0008
#endif
#ifndef PERL_PV_ESCAPE_UNI
#define PERL_PV_ESCAPE_UNI 0x0100
#endif
#ifndef PERL_PV_ESCAPE_UNI_DETECT
#define PERL_PV_ESCAPE_UNI_DETECT 0x0200
#endif
#ifndef PERL_PV_ESCAPE_ALL
#define PERL_PV_ESCAPE_ALL 0x1000
#endif
#ifndef PERL_PV_ESCAPE_NOBACKSLASH
#define PERL_PV_ESCAPE_NOBACKSLASH 0x2000
#endif
#ifndef PERL_PV_ESCAPE_NOCLEAR
#define PERL_PV_ESCAPE_NOCLEAR 0x4000
#endif
#ifndef PERL_PV_ESCAPE_RE
#define PERL_PV_ESCAPE_RE 0x8000
#endif
#ifndef PERL_PV_PRETTY_NOCLEAR
#define PERL_PV_PRETTY_NOCLEAR PERL_PV_ESCAPE_NOCLEAR
#endif
#ifndef PERL_PV_PRETTY_DUMP
#define PERL_PV_PRETTY_DUMP PERL_PV_PRETTY_ELLIPSES|PERL_PV_PRETTY_QUOTE
#endif
#ifndef PERL_PV_PRETTY_REGPROP
#define PERL_PV_PRETTY_REGPROP PERL_PV_PRETTY_ELLIPSES|PERL_PV_PRETTY_LTGT|PERL_PV_ESCAPE_RE
#endif
#ifndef pv_escape
#if defined(NEED_pv_escape)
static char * DPPP_(my_pv_escape)(pTHX_ SV * dsv, char const * const str, const STRLEN count, const STRLEN max, STRLEN * const escaped, const U32 flags);
static
#else
extern char * DPPP_(my_pv_escape)(pTHX_ SV * dsv, char const * const str, const STRLEN count, const STRLEN max, STRLEN * const escaped, const U32 flags);
#endif
#if defined(NEED_pv_escape) || defined(NEED_pv_escape_GLOBAL)
#ifdef pv_escape
#undef pv_escape
#endif
#define pv_escape(a,b,c,d,e,f) DPPP_(my_pv_escape)(aTHX_ a,b,c,d,e,f)
#define Perl_pv_escape DPPP_(my_pv_escape)
char *
DPPP_(my_pv_escape)(pTHX_ SV *dsv, char const * const str,
const STRLEN count, const STRLEN max,
STRLEN * const escaped, const U32 flags)
{
const char esc = flags & PERL_PV_ESCAPE_RE ? '%' : '\\';
const char dq = flags & PERL_PV_ESCAPE_QUOTE ? '"' : esc;
char octbuf[32] = "%123456789ABCDF";
STRLEN wrote = 0;
STRLEN chsize = 0;
STRLEN readsize = 1;
#if defined(is_utf8_string) && defined(utf8_to_uvchr_buf)
bool isuni = flags & PERL_PV_ESCAPE_UNI ? 1 : 0;
#endif
const char *pv = str;
const char * const end = pv + count;
octbuf[0] = esc;
if (!(flags & PERL_PV_ESCAPE_NOCLEAR))
sv_setpvs(dsv, "");
#if defined(is_utf8_string) && defined(utf8_to_uvchr_buf)
if ((flags & PERL_PV_ESCAPE_UNI_DETECT) && is_utf8_string((U8*)pv, count))
isuni = 1;
#endif
for (; pv < end && (!max || wrote < max) ; pv += readsize) {
const UV u =
#if defined(is_utf8_string) && defined(utf8_to_uvchr_buf)
isuni ? utf8_to_uvchr_buf((U8*)pv, end, &readsize) :
#endif
(U8)*pv;
const U8 c = (U8)u & 0xFF;
if (u > 255 || (flags & PERL_PV_ESCAPE_ALL)) {
if (flags & PERL_PV_ESCAPE_FIRSTCHAR)
chsize = my_snprintf(octbuf, sizeof octbuf,
"%" UVxf, u);
else
chsize = my_snprintf(octbuf, sizeof octbuf,
"%cx{%" UVxf "}", esc, u);
} else if (flags & PERL_PV_ESCAPE_NOBACKSLASH) {
chsize = 1;
} else {
if (c == dq || c == esc || !isPRINT(c)) {
chsize = 2;
switch (c) {
case '\\' :
case '%' : if (c == esc)
octbuf[1] = esc;
else
chsize = 1;
break;
case '\v' : octbuf[1] = 'v'; break;
case '\t' : octbuf[1] = 't'; break;
case '\r' : octbuf[1] = 'r'; break;
case '\n' : octbuf[1] = 'n'; break;
case '\f' : octbuf[1] = 'f'; break;
case '"' : if (dq == '"')
octbuf[1] = '"';
else
chsize = 1;
break;
default: chsize = my_snprintf(octbuf, sizeof octbuf,
pv < end && isDIGIT((U8)*(pv+readsize))
? "%c%03o" : "%c%o", esc, c);
}
} else {
chsize = 1;
}
}
if (max && wrote + chsize > max) {
break;
} else if (chsize > 1) {
sv_catpvn(dsv, octbuf, chsize);
wrote += chsize;
} else {
char tmp[2];
my_snprintf(tmp, sizeof tmp, "%c", c);
sv_catpvn(dsv, tmp, 1);
wrote++;
}
if (flags & PERL_PV_ESCAPE_FIRSTCHAR)
break;
}
if (escaped != NULL)
*escaped= pv - str;
return SvPVX(dsv);
}
#endif
#endif
#ifndef pv_pretty
#if defined(NEED_pv_pretty)
static char * DPPP_(my_pv_pretty)(pTHX_ SV * dsv, char const * const str, const STRLEN count, const STRLEN max, char const * const start_color, char const * const end_color, const U32 flags);
static
#else
extern char * DPPP_(my_pv_pretty)(pTHX_ SV * dsv, char const * const str, const STRLEN count, const STRLEN max, char const * const start_color, char const * const end_color, const U32 flags);
#endif
#if defined(NEED_pv_pretty) || defined(NEED_pv_pretty_GLOBAL)
#ifdef pv_pretty
#undef pv_pretty
#endif
#define pv_pretty(a,b,c,d,e,f,g) DPPP_(my_pv_pretty)(aTHX_ a,b,c,d,e,f,g)
#define Perl_pv_pretty DPPP_(my_pv_pretty)
char *
DPPP_(my_pv_pretty)(pTHX_ SV *dsv, char const * const str, const STRLEN count,
const STRLEN max, char const * const start_color, char const * const end_color,
const U32 flags)
{
const U8 dq = (flags & PERL_PV_PRETTY_QUOTE) ? '"' : '%';
STRLEN escaped;
if (!(flags & PERL_PV_PRETTY_NOCLEAR))
sv_setpvs(dsv, "");
if (dq == '"')
sv_catpvs(dsv, "\"");
else if (flags & PERL_PV_PRETTY_LTGT)
sv_catpvs(dsv, "<");
if (start_color != NULL)
sv_catpv(dsv, D_PPP_CONSTPV_ARG(start_color));
pv_escape(dsv, str, count, max, &escaped, flags | PERL_PV_ESCAPE_NOCLEAR);
if (end_color != NULL)
sv_catpv(dsv, D_PPP_CONSTPV_ARG(end_color));
if (dq == '"')
sv_catpvs(dsv, "\"");
else if (flags & PERL_PV_PRETTY_LTGT)
sv_catpvs(dsv, ">");
if ((flags & PERL_PV_PRETTY_ELLIPSES) && escaped < count)
sv_catpvs(dsv, "...");
return SvPVX(dsv);
}
#endif
#endif
#ifndef pv_display
#if defined(NEED_pv_display)
static char * DPPP_(my_pv_display)(pTHX_ SV * dsv, const char * pv, STRLEN cur, STRLEN len, STRLEN pvlim);
static
#else
extern char * DPPP_(my_pv_display)(pTHX_ SV * dsv, const char * pv, STRLEN cur, STRLEN len, STRLEN pvlim);
#endif
#if defined(NEED_pv_display) || defined(NEED_pv_display_GLOBAL)
#ifdef pv_display
#undef pv_display
#endif
#define pv_display(a,b,c,d,e) DPPP_(my_pv_display)(aTHX_ a,b,c,d,e)
#define Perl_pv_display DPPP_(my_pv_display)
char *
DPPP_(my_pv_display)(pTHX_ SV *dsv, const char *pv, STRLEN cur, STRLEN len, STRLEN pvlim)
{
pv_pretty(dsv, pv, cur, pvlim, NULL, NULL, PERL_PV_PRETTY_DUMP);
if (len > cur && pv[cur] == '\0')
sv_catpvs(dsv, "\\0");
return SvPVX(dsv);
}
#endif
#endif
#if PERL_VERSION_LT(5,27,9)
#ifndef LC_NUMERIC_LOCK
#define LC_NUMERIC_LOCK
#endif
#ifndef LC_NUMERIC_UNLOCK
#define LC_NUMERIC_UNLOCK
#endif
#if PERL_VERSION_LT(5,19,0)
#undef STORE_LC_NUMERIC_SET_STANDARD
#undef RESTORE_LC_NUMERIC
#undef DECLARATION_FOR_LC_NUMERIC_MANIPULATION
#ifdef USE_LOCALE
#ifndef DECLARATION_FOR_LC_NUMERIC_MANIPULATION
#define DECLARATION_FOR_LC_NUMERIC_MANIPULATION char *LoC_
#endif
#ifndef STORE_NUMERIC_SET_STANDARD
#define STORE_NUMERIC_SET_STANDARD() \
LoC_ = savepv(setlocale(LC_NUMERIC, NULL)); \
SAVEFREEPV(LoC_); \
setlocale(LC_NUMERIC, "C");
#endif
#ifndef RESTORE_LC_NUMERIC
#define RESTORE_LC_NUMERIC() \
setlocale(LC_NUMERIC, LoC_);
#endif
#else
#ifndef DECLARATION_FOR_LC_NUMERIC_MANIPULATION
#define DECLARATION_FOR_LC_NUMERIC_MANIPULATION
#endif
#ifndef STORE_LC_NUMERIC_SET_STANDARD
#define STORE_LC_NUMERIC_SET_STANDARD()
#endif
#ifndef RESTORE_LC_NUMERIC
#define RESTORE_LC_NUMERIC()
#endif
#endif
#endif
#endif
#ifndef LOCK_NUMERIC_STANDARD
#define LOCK_NUMERIC_STANDARD()
#endif
#ifndef UNLOCK_NUMERIC_STANDARD
#define UNLOCK_NUMERIC_STANDARD()
#endif
#ifndef LOCK_LC_NUMERIC_STANDARD
#define LOCK_LC_NUMERIC_STANDARD LOCK_NUMERIC_STANDARD
#endif
#ifndef UNLOCK_LC_NUMERIC_STANDARD
#define UNLOCK_LC_NUMERIC_STANDARD UNLOCK_NUMERIC_STANDARD
#endif
#ifndef switch_to_global_locale
#define switch_to_global_locale()
#endif
#ifdef sync_locale
#if (PERL_BCDVERSION < 0x5027009)
#if (PERL_BCDVERSION >= 0x5021003)
#undef sync_locale
#define sync_locale() (Perl_sync_locale(aTHX), 1)
#elif defined(sync_locale)
#undef sync_locale
#define sync_locale() (new_ctype(setlocale(LC_CTYPE, NULL)), \
new_collate(setlocale(LC_COLLATE, NULL)), \
set_numeric_local(), \
new_numeric(setlocale(LC_NUMERIC, NULL)), \
1)
#elif defined(new_ctype) && defined(LC_CTYPE)
#define sync_locale() (new_ctype(setlocale(LC_CTYPE, NULL)), 1)
#endif
#endif
#endif
#ifndef sync_locale
#define sync_locale() 1
#endif
#endif
