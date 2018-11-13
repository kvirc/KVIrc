#if 0
<<'SKIP';
#endif
/*
----------------------------------------------------------------------

    ppport.h -- Perl/Pollution/Portability Version 3.42

    Automatically created by Devel::PPPort running under perl 5.028000.

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
  if (eval $Devel::PPPort::VERSION < 3.42) {
    die "ppport.h was originally generated with Devel::PPPort 3.42.\n"
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
#ifndef PERL_REVISION
#if !defined(__PATCHLEVEL_H_INCLUDED__) && !(defined(PATCHLEVEL) && defined(SUBVERSION))
#define PERL_PATCHLEVEL_H_IMPLICIT
#include <patchlevel.h>
#endif
#if !(defined(PERL_VERSION) || (defined(SUBVERSION) && defined(PATCHLEVEL)))
#include <could_not_find_Perl_patchlevel.h>
#endif
#ifndef PERL_REVISION
#define PERL_REVISION (5)
#define PERL_VERSION PATCHLEVEL
#define PERL_SUBVERSION SUBVERSION
#endif
#endif
#define D_PPP_DEC2BCD(dec) ((((dec)/100)<<8)|((((dec)%100)/10)<<4)|((dec)%10))
#define PERL_BCDVERSION ((D_PPP_DEC2BCD(PERL_REVISION)<<24)|(D_PPP_DEC2BCD(PERL_VERSION)<<12)|D_PPP_DEC2BCD(PERL_SUBVERSION))
#if PERL_REVISION != 5
#error ppport.h only works with Perl version 5
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
#ifndef sv_2uv
#define sv_2uv(sv) ((PL_Sv = (sv)), (UV) (SvNOK(PL_Sv) ? SvNV(PL_Sv) : sv_2nv(PL_Sv)))
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
#ifndef SvUVx
#define SvUVx(sv) ((PL_Sv = (sv)), SvUV(PL_Sv))
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
#ifndef SvRX
#if defined(NEED_SvRX)
static void * DPPP_(my_SvRX)(pTHX_ SV *rv);
static
#else
extern void * DPPP_(my_SvRX)(pTHX_ SV *rv);
#endif
#if defined(NEED_SvRX) || defined(NEED_SvRX_GLOBAL)
#ifdef SvRX
#undef SvRX
#endif
#define SvRX(a) DPPP_(my_SvRX)(aTHX_ a)
void *
DPPP_(my_SvRX)(pTHX_ SV *rv)
{
if (SvROK(rv)) {
SV *sv = SvRV(rv);
if (SvMAGICAL(sv)) {
MAGIC *mg = mg_find(sv, PERL_MAGIC_qr);
if (mg && mg->mg_obj) {
return mg->mg_obj;
}
}
}
return 0;
}
#endif
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
#ifndef dNOOP
#define dNOOP extern int  Perl___notused PERL_UNUSED_DECL
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
#if defined(PERL_GCC_PEDANTIC)
#ifndef PERL_GCC_BRACE_GROUPS_FORBIDDEN
#define PERL_GCC_BRACE_GROUPS_FORBIDDEN
#endif
#endif
#if defined(__GNUC__) && !defined(PERL_GCC_BRACE_GROUPS_FORBIDDEN) && !defined(__cplusplus)
#ifndef PERL_USE_GCC_BRACE_GROUPS
#define PERL_USE_GCC_BRACE_GROUPS
#endif
#endif
#undef STMT_START
#undef STMT_END
#ifdef PERL_USE_GCC_BRACE_GROUPS
#define STMT_START (void)(
#define STMT_END )
#else
#if defined(VOIDFLAGS) && (VOIDFLAGS) && (defined(sun) || defined(__sun__)) && !defined(__GNUC__)
#define STMT_START if (1)
#define STMT_END else (void)0
#else
#define STMT_START do
#define STMT_END while (0)
#endif
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
register SV ** const mark = PL_stack_base + ax++
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
#ifndef UTF8_MAXBYTES
#define UTF8_MAXBYTES UTF8_MAXLEN
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
#ifndef isPSXSPC
#define isPSXSPC(c) (isSPACE(c) || (c) == '\v')
#endif
#ifndef isBLANK
#define isBLANK(c) ((c) == ' ' || (c) == '\t')
#endif
#ifdef EBCDIC
#ifndef isALNUMC
#define isALNUMC(c) isalnum(c)
#endif
#ifndef isASCII
#define isASCII(c) isascii(c)
#endif
#ifndef isCNTRL
#define isCNTRL(c) iscntrl(c)
#endif
#ifndef isGRAPH
#define isGRAPH(c) isgraph(c)
#endif
#ifndef isPRINT
#define isPRINT(c) isprint(c)
#endif
#ifndef isPUNCT
#define isPUNCT(c) ispunct(c)
#endif
#ifndef isXDIGIT
#define isXDIGIT(c) isxdigit(c)
#endif
#else
#if (PERL_BCDVERSION < 0x5010000)
#undef isPRINT
#endif
#ifndef WIDEST_UTYPE
#ifdef QUADKIND
#ifdef U64TYPE
#define WIDEST_UTYPE U64TYPE
#else
#define WIDEST_UTYPE Quad_t
#endif
#else
#define WIDEST_UTYPE U32
#endif
#endif
#ifndef isALNUMC
#define isALNUMC(c) (isALPHA(c) || isDIGIT(c))
#endif
#ifndef isASCII
#define isASCII(c) ((WIDEST_UTYPE) (c) <= 127)
#endif
#ifndef isCNTRL
#define isCNTRL(c) ((WIDEST_UTYPE) (c) < ' ' || (c) == 127)
#endif
#ifndef isGRAPH
#define isGRAPH(c) (isALNUM(c) || isPUNCT(c))
#endif
#ifndef isPRINT
#define isPRINT(c) (((c) >= 32 && (c) < 127))
#endif
#ifndef isPUNCT
#define isPUNCT(c) (((c) >= 33 && (c) <= 47) || ((c) >= 58 && (c) <= 64) || ((c) >= 91 && (c) <= 96) || ((c) >= 123 && (c) <= 126))
#endif
#ifndef isXDIGIT
#define isXDIGIT(c) (isDIGIT(c) || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))
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
#ifndef MUTABLE_PTR
#if defined(__GNUC__) && !defined(PERL_GCC_BRACE_GROUPS_FORBIDDEN)
#define MUTABLE_PTR(p) ({ void *_p = (p); _p; })
#else
#define MUTABLE_PTR(p) ((void *) (p))
#endif
#endif
#ifndef MUTABLE_SV
#define MUTABLE_SV(p) ((SV *)MUTABLE_PTR(p))
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
#define D_PPP_FIX_UTF8_ERRSV(errsv, sv) \
STMT_START { \
if (sv != ERRSV) \
SvFLAGS(ERRSV) = (SvFLAGS(ERRSV) & ~SVf_UTF8) | \
(SvFLAGS(sv) & SVf_UTF8); \
} STMT_END
#else
#define D_PPP_FIX_UTF8_ERRSV(errsv, sv) STMT_START {} STMT_END
#endif
#define croak_sv(sv) \
STMT_START { \
if (SvROK(sv)) { \
sv_setsv(ERRSV, sv); \
croak(NULL); \
} else { \
D_PPP_FIX_UTF8_ERRSV(ERRSV, sv); \
croak("%" SVf, SVfARG(sv)); \
} \
} STMT_END
#elif (PERL_BCDVERSION >= 0x5004000)
#define croak_sv(sv) croak("%" SVf, SVfARG(sv))
#else
#define croak_sv(sv) croak("%s", SvPV_nolen(sv))
#endif
#endif
#ifndef die_sv
#if defined(NEED_die_sv)
static OP * DPPP_(my_die_sv)(pTHX_ SV *sv);
static
#else
extern OP * DPPP_(my_die_sv)(pTHX_ SV *sv);
#endif
#if defined(NEED_die_sv) || defined(NEED_die_sv_GLOBAL)
#ifdef die_sv
#undef die_sv
#endif
#define die_sv(a) DPPP_(my_die_sv)(aTHX_ a)
#define Perl_die_sv DPPP_(my_die_sv)
OP *
DPPP_(my_die_sv)(pTHX_ SV *sv)
{
croak_sv(sv);
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
#ifndef vmess
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
#if (PERL_BCDVERSION < 0x5006000)
#undef mess
#endif
#if !defined(mess_nocontext) && !defined(Perl_mess_nocontext)
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
#ifndef mess_sv
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
#endif
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
#ifndef PERL_LOADMOD_DENY
#define PERL_LOADMOD_DENY 0x1
#endif
#ifndef PERL_LOADMOD_NOIMPORT
#define PERL_LOADMOD_NOIMPORT 0x2
#endif
#ifndef PERL_LOADMOD_IMPORT_OPS
#define PERL_LOADMOD_IMPORT_OPS 0x4
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
#ifndef eval_pv
#if defined(NEED_eval_pv)
static SV* DPPP_(my_eval_pv)(char *p, I32 croak_on_error);
static
#else
extern SV* DPPP_(my_eval_pv)(char *p, I32 croak_on_error);
#endif
#if defined(NEED_eval_pv) || defined(NEED_eval_pv_GLOBAL)
#ifdef eval_pv
#undef eval_pv
#endif
#define eval_pv(a,b) DPPP_(my_eval_pv)(aTHX_ a,b)
#define Perl_eval_pv DPPP_(my_eval_pv)
SV*
DPPP_(my_eval_pv)(char *p, I32 croak_on_error)
{
dSP;
SV* sv = newSVpv(p, 0);
PUSHMARK(sp);
eval_sv(sv, G_SCALAR);
SvREFCNT_dec(sv);
SPAGAIN;
sv = POPs;
PUTBACK;
if (croak_on_error && SvTRUEx(ERRSV))
croak_sv(ERRSV);
return sv;
}
#endif
#endif
#ifndef vload_module
#if defined(NEED_vload_module)
static void DPPP_(my_vload_module)(U32 flags, SV *name, SV *ver, va_list *args);
static
#else
extern void DPPP_(my_vload_module)(U32 flags, SV *name, SV *ver, va_list *args);
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
#if (PERL_BCDVERSION >= 0x5004000)
utilize(!(flags & PERL_LOADMOD_DENY), start_subparse(FALSE, 0),
veop, modname, imop);
#elif (PERL_BCDVERSION > 0x5003000)
utilize(!(flags & PERL_LOADMOD_DENY), start_subparse(),
veop, modname, imop);
#else
utilize(!(flags & PERL_LOADMOD_DENY), start_subparse(),
modname, imop);
#endif
PL_expect = oexpect;
PL_copline = ocopline;
PL_curcop = ocurcop;
}
}
#endif
#endif
#ifndef load_module
#if defined(NEED_load_module)
static void DPPP_(my_load_module)(U32 flags, SV *name, SV *ver, ...);
static
#else
extern void DPPP_(my_load_module)(U32 flags, SV *name, SV *ver, ...);
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
#if defined(NEED_newRV_noinc)
static SV * DPPP_(my_newRV_noinc)(SV *sv);
static
#else
extern SV * DPPP_(my_newRV_noinc)(SV *sv);
#endif
#if defined(NEED_newRV_noinc) || defined(NEED_newRV_noinc_GLOBAL)
#ifdef newRV_noinc
#undef newRV_noinc
#endif
#define newRV_noinc(a) DPPP_(my_newRV_noinc)(aTHX_ a)
#define Perl_newRV_noinc DPPP_(my_newRV_noinc)
SV *
DPPP_(my_newRV_noinc)(SV *sv)
{
SV *rv = (SV *)newRV(sv);
SvREFCNT_dec(sv);
return rv;
}
#endif
#endif
#if (PERL_BCDVERSION < 0x5004063) && (PERL_BCDVERSION != 0x5004005)
#if defined(NEED_newCONSTSUB)
static void DPPP_(my_newCONSTSUB)(HV *stash, const char *name, SV *sv);
static
#else
extern void DPPP_(my_newCONSTSUB)(HV *stash, const char *name, SV *sv);
#endif
#if defined(NEED_newCONSTSUB) || defined(NEED_newCONSTSUB_GLOBAL)
#ifdef newCONSTSUB
#undef newCONSTSUB
#endif
#define newCONSTSUB(a,b,c) DPPP_(my_newCONSTSUB)(aTHX_ a,b,c)
#define Perl_newCONSTSUB DPPP_(my_newCONSTSUB)
#define D_PPP_PL_copline PL_copline
void
DPPP_(my_newCONSTSUB)(HV *stash, const char *name, SV *sv)
{
U32 oldhints = PL_hints;
HV *old_cop_stash = PL_curcop->cop_stash;
HV *old_curstash = PL_curstash;
line_t oldline = PL_curcop->cop_line;
PL_curcop->cop_line = D_PPP_PL_copline;
PL_hints &= ~HINT_BLOCK_SCOPE;
if (stash)
PL_curstash = PL_curcop->cop_stash = stash;
newSUB(
#if (PERL_BCDVERSION < 0x5003022)
start_subparse(),
#elif (PERL_BCDVERSION == 0x5003022)
start_subparse(0),
#else
start_subparse(FALSE, 0),
#endif
newSVOP(OP_CONST, 0, newSVpv((char *) name, 0)),
newSVOP(OP_CONST, 0, &PL_sv_no),
newSTATEOP(0, Nullch, newSVOP(OP_CONST, 0, sv))
);
PL_hints = oldhints;
PL_curcop->cop_stash = old_cop_stash;
PL_curstash = old_curstash;
PL_curcop->cop_line = oldline;
}
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
#if defined(NEED_newSV_type)
static SV* DPPP_(my_newSV_type)(pTHX_ svtype const t);
static
#else
extern SV* DPPP_(my_newSV_type)(pTHX_ svtype const t);
#endif
#if defined(NEED_newSV_type) || defined(NEED_newSV_type_GLOBAL)
#ifdef newSV_type
#undef newSV_type
#endif
#define newSV_type(a) DPPP_(my_newSV_type)(aTHX_ a)
#define Perl_newSV_type DPPP_(my_newSV_type)
SV*
DPPP_(my_newSV_type)(pTHX_ svtype const t)
{
SV* const sv = newSV(0);
sv_upgrade(sv, t);
return sv;
}
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
#if defined(NEED_newSVpvn_flags)
static SV * DPPP_(my_newSVpvn_flags)(pTHX_ const char *s, STRLEN len, U32 flags);
static
#else
extern SV * DPPP_(my_newSVpvn_flags)(pTHX_ const char *s, STRLEN len, U32 flags);
#endif
#if defined(NEED_newSVpvn_flags) || defined(NEED_newSVpvn_flags_GLOBAL)
#ifdef newSVpvn_flags
#undef newSVpvn_flags
#endif
#define newSVpvn_flags(a,b,c) DPPP_(my_newSVpvn_flags)(aTHX_ a,b,c)
#define Perl_newSVpvn_flags DPPP_(my_newSVpvn_flags)
SV *
DPPP_(my_newSVpvn_flags)(pTHX_ const char *s, STRLEN len, U32 flags)
{
SV *sv = newSVpvn(D_PPP_CONSTPV_ARG(s), len);
SvFLAGS(sv) |= (flags & SVf_UTF8);
return (flags & SVs_TEMP) ? sv_2mortal(sv) : sv;
}
#endif
#endif
#if !defined(NEED_sv_2pv_flags) && defined(NEED_sv_2pv_nolen)
#define NEED_sv_2pv_flags
#endif
#if !defined(NEED_sv_2pv_flags_GLOBAL) && defined(NEED_sv_2pv_nolen_GLOBAL)
#define NEED_sv_2pv_flags_GLOBAL
#endif
#ifndef sv_2pv_nolen
#define sv_2pv_nolen(sv) SvPV_nolen(sv)
#endif
#ifdef SvPVbyte
#if (PERL_BCDVERSION < 0x5007000)
#if defined(NEED_sv_2pvbyte)
static char * DPPP_(my_sv_2pvbyte)(pTHX_ SV *sv, STRLEN *lp);
static
#else
extern char * DPPP_(my_sv_2pvbyte)(pTHX_ SV *sv, STRLEN *lp);
#endif
#if defined(NEED_sv_2pvbyte) || defined(NEED_sv_2pvbyte_GLOBAL)
#ifdef sv_2pvbyte
#undef sv_2pvbyte
#endif
#define sv_2pvbyte(a,b) DPPP_(my_sv_2pvbyte)(aTHX_ a,b)
#define Perl_sv_2pvbyte DPPP_(my_sv_2pvbyte)
char *
DPPP_(my_sv_2pvbyte)(pTHX_ SV *sv, STRLEN *lp)
{
sv_utf8_downgrade(sv,0);
return SvPV(sv,*lp);
}
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
#ifndef SV_NOSTEAL
#define SV_NOSTEAL 0
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
#if (PERL_BCDVERSION < 0x5007002)
#if defined(NEED_sv_2pv_flags)
static char * DPPP_(my_sv_2pv_flags)(pTHX_ SV *sv, STRLEN *lp, I32 flags);
static
#else
extern char * DPPP_(my_sv_2pv_flags)(pTHX_ SV *sv, STRLEN *lp, I32 flags);
#endif
#if defined(NEED_sv_2pv_flags) || defined(NEED_sv_2pv_flags_GLOBAL)
#ifdef sv_2pv_flags
#undef sv_2pv_flags
#endif
#define sv_2pv_flags(a,b,c) DPPP_(my_sv_2pv_flags)(aTHX_ a,b,c)
#define Perl_sv_2pv_flags DPPP_(my_sv_2pv_flags)
char *
DPPP_(my_sv_2pv_flags)(pTHX_ SV *sv, STRLEN *lp, I32 flags)
{
STRLEN n_a = (STRLEN) flags;
return sv_2pv(sv, lp ? lp : &n_a);
}
#endif
#if defined(NEED_sv_pvn_force_flags)
static char * DPPP_(my_sv_pvn_force_flags)(pTHX_ SV *sv, STRLEN *lp, I32 flags);
static
#else
extern char * DPPP_(my_sv_pvn_force_flags)(pTHX_ SV *sv, STRLEN *lp, I32 flags);
#endif
#if defined(NEED_sv_pvn_force_flags) || defined(NEED_sv_pvn_force_flags_GLOBAL)
#ifdef sv_pvn_force_flags
#undef sv_pvn_force_flags
#endif
#define sv_pvn_force_flags(a,b,c) DPPP_(my_sv_pvn_force_flags)(aTHX_ a,b,c)
#define Perl_sv_pvn_force_flags DPPP_(my_sv_pvn_force_flags)
char *
DPPP_(my_sv_pvn_force_flags)(pTHX_ SV *sv, STRLEN *lp, I32 flags)
{
STRLEN n_a = (STRLEN) flags;
return sv_pvn_force(sv, lp ? lp : &n_a);
}
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
#if (PERL_BCDVERSION >= 0x5004000) && !defined(vnewSVpvf)
#if defined(NEED_vnewSVpvf)
static SV * DPPP_(my_vnewSVpvf)(pTHX_ const char *pat, va_list *args);
static
#else
extern SV * DPPP_(my_vnewSVpvf)(pTHX_ const char *pat, va_list *args);
#endif
#if defined(NEED_vnewSVpvf) || defined(NEED_vnewSVpvf_GLOBAL)
#ifdef vnewSVpvf
#undef vnewSVpvf
#endif
#define vnewSVpvf(a,b) DPPP_(my_vnewSVpvf)(aTHX_ a,b)
#define Perl_vnewSVpvf DPPP_(my_vnewSVpvf)
SV *
DPPP_(my_vnewSVpvf)(pTHX_ const char *pat, va_list *args)
{
register SV *sv = newSV(0);
sv_vsetpvfn(sv, pat, strlen(pat), args, Null(SV**), 0, Null(bool*));
return sv;
}
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
static void DPPP_(my_sv_catpvf_mg)(pTHX_ SV *sv, const char *pat, ...);
static
#else
extern void DPPP_(my_sv_catpvf_mg)(pTHX_ SV *sv, const char *pat, ...);
#endif
#if defined(NEED_sv_catpvf_mg) || defined(NEED_sv_catpvf_mg_GLOBAL)
#define Perl_sv_catpvf_mg DPPP_(my_sv_catpvf_mg)
void
DPPP_(my_sv_catpvf_mg)(pTHX_ SV *sv, const char *pat, ...)
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
static void DPPP_(my_sv_catpvf_mg_nocontext)(SV *sv, const char *pat, ...);
static
#else
extern void DPPP_(my_sv_catpvf_mg_nocontext)(SV *sv, const char *pat, ...);
#endif
#if defined(NEED_sv_catpvf_mg_nocontext) || defined(NEED_sv_catpvf_mg_nocontext_GLOBAL)
#define sv_catpvf_mg_nocontext DPPP_(my_sv_catpvf_mg_nocontext)
#define Perl_sv_catpvf_mg_nocontext DPPP_(my_sv_catpvf_mg_nocontext)
void
DPPP_(my_sv_catpvf_mg_nocontext)(SV *sv, const char *pat, ...)
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
static void DPPP_(my_sv_setpvf_mg)(pTHX_ SV *sv, const char *pat, ...);
static
#else
extern void DPPP_(my_sv_setpvf_mg)(pTHX_ SV *sv, const char *pat, ...);
#endif
#if defined(NEED_sv_setpvf_mg) || defined(NEED_sv_setpvf_mg_GLOBAL)
#define Perl_sv_setpvf_mg DPPP_(my_sv_setpvf_mg)
void
DPPP_(my_sv_setpvf_mg)(pTHX_ SV *sv, const char *pat, ...)
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
static void DPPP_(my_sv_setpvf_mg_nocontext)(SV *sv, const char *pat, ...);
static
#else
extern void DPPP_(my_sv_setpvf_mg_nocontext)(SV *sv, const char *pat, ...);
#endif
#if defined(NEED_sv_setpvf_mg_nocontext) || defined(NEED_sv_setpvf_mg_nocontext_GLOBAL)
#define sv_setpvf_mg_nocontext DPPP_(my_sv_setpvf_mg_nocontext)
#define Perl_sv_setpvf_mg_nocontext DPPP_(my_sv_setpvf_mg_nocontext)
void
DPPP_(my_sv_setpvf_mg_nocontext)(SV *sv, const char *pat, ...)
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
#ifndef newSVpvn_share
#if defined(NEED_newSVpvn_share)
static SV * DPPP_(my_newSVpvn_share)(pTHX_ const char *src, I32 len, U32 hash);
static
#else
extern SV * DPPP_(my_newSVpvn_share)(pTHX_ const char *src, I32 len, U32 hash);
#endif
#if defined(NEED_newSVpvn_share) || defined(NEED_newSVpvn_share_GLOBAL)
#ifdef newSVpvn_share
#undef newSVpvn_share
#endif
#define newSVpvn_share(a,b,c) DPPP_(my_newSVpvn_share)(aTHX_ a,b,c)
#define Perl_newSVpvn_share DPPP_(my_newSVpvn_share)
SV *
DPPP_(my_newSVpvn_share)(pTHX_ const char *src, I32 len, U32 hash)
{
SV *sv;
if (len < 0)
len = -len;
if (!hash)
PERL_HASH(hash, (char*) src, len);
sv = newSVpvn((char *) src, len);
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
#ifndef gv_fetchpvn_flags
#if defined(NEED_gv_fetchpvn_flags)
static GV* DPPP_(my_gv_fetchpvn_flags)(pTHX_ const char* name, STRLEN len, int flags, int types);
static
#else
extern GV* DPPP_(my_gv_fetchpvn_flags)(pTHX_ const char* name, STRLEN len, int flags, int types);
#endif
#if defined(NEED_gv_fetchpvn_flags) || defined(NEED_gv_fetchpvn_flags_GLOBAL)
#ifdef gv_fetchpvn_flags
#undef gv_fetchpvn_flags
#endif
#define gv_fetchpvn_flags(a,b,c,d) DPPP_(my_gv_fetchpvn_flags)(aTHX_ a,b,c,d)
#define Perl_gv_fetchpvn_flags DPPP_(my_gv_fetchpvn_flags)
GV*
DPPP_(my_gv_fetchpvn_flags)(pTHX_ const char* name, STRLEN len, int flags, int types) {
char *namepv = savepvn(name, len);
GV* stash = gv_fetchpv(namepv, TRUE, SVt_PVHV);
Safefree(namepv);
return stash;
}
#endif
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
#ifndef ckWARN
#ifdef G_WARN_ON
#define ckWARN(a) (PL_dowarn & G_WARN_ON)
#else
#define ckWARN(a) PL_dowarn
#endif
#endif
#if (PERL_BCDVERSION >= 0x5004000) && !defined(warner)
#if defined(NEED_warner)
static void DPPP_(my_warner)(U32 err, const char *pat, ...);
static
#else
extern void DPPP_(my_warner)(U32 err, const char *pat, ...);
#endif
#if defined(NEED_warner) || defined(NEED_warner_GLOBAL)
#define Perl_warner DPPP_(my_warner)
void
DPPP_(my_warner)(U32 err, const char *pat, ...)
{
SV *sv;
va_list args;
PERL_UNUSED_ARG(err);
va_start(args, pat);
sv = vnewSVpvf(pat, &args);
va_end(args);
sv_2mortal(sv);
warn("%s", SvPV_nolen(sv));
}
#define warner Perl_warner
#define Perl_warner_nocontext Perl_warner
#endif
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
#ifndef SvGETMAGIC
#define SvGETMAGIC(x) STMT_START { if (SvGMAGICAL(x)) mg_get(x); } STMT_END
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
#ifndef SvIV_nomg
#define SvIV_nomg SvIV
#endif
#ifndef SvUV_nomg
#define SvUV_nomg SvUV
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
static MAGIC * DPPP_(my_mg_findext)(SV * sv, int type, const MGVTBL *vtbl);
static
#else
extern MAGIC * DPPP_(my_mg_findext)(SV * sv, int type, const MGVTBL *vtbl);
#endif
#if defined(NEED_mg_findext) || defined(NEED_mg_findext_GLOBAL)
#define mg_findext DPPP_(my_mg_findext)
#define Perl_mg_findext DPPP_(my_mg_findext)
MAGIC *
DPPP_(my_mg_findext)(SV * sv, int type, const MGVTBL *vtbl) {
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
static int DPPP_(my_sv_unmagicext)(pTHX_ SV * const sv, const int type, MGVTBL * vtbl);
static
#else
extern int DPPP_(my_sv_unmagicext)(pTHX_ SV * const sv, const int type, MGVTBL * vtbl);
#endif
#if defined(NEED_sv_unmagicext) || defined(NEED_sv_unmagicext_GLOBAL)
#ifdef sv_unmagicext
#undef sv_unmagicext
#endif
#define sv_unmagicext(a,b,c) DPPP_(my_sv_unmagicext)(aTHX_ a,b,c)
#define Perl_sv_unmagicext DPPP_(my_sv_unmagicext)
int
DPPP_(my_sv_unmagicext)(pTHX_ SV *const sv, const int type, MGVTBL *vtbl)
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
register const PERL_CONTEXT * const cx = &cxstk[i];
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
static const PERL_CONTEXT * DPPP_(my_caller_cx)(pTHX_ I32 count, const PERL_CONTEXT **dbcxp);
static
#else
extern const PERL_CONTEXT * DPPP_(my_caller_cx)(pTHX_ I32 count, const PERL_CONTEXT **dbcxp);
#endif
#if defined(NEED_caller_cx) || defined(NEED_caller_cx_GLOBAL)
#ifdef caller_cx
#undef caller_cx
#endif
#define caller_cx(a,b) DPPP_(my_caller_cx)(aTHX_ a,b)
#define Perl_caller_cx DPPP_(my_caller_cx)
const PERL_CONTEXT *
DPPP_(my_caller_cx)(pTHX_ I32 count, const PERL_CONTEXT **dbcxp)
{
register I32 cxix = DPPP_dopoptosub_at(cxstack, cxstack_ix);
register const PERL_CONTEXT *cx;
register const PERL_CONTEXT *ccstack = cxstack;
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
count++;
if (!count--)
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
static bool DPPP_(my_grok_numeric_radix)(pTHX_ const char ** sp, const char * send);
static
#else
extern bool DPPP_(my_grok_numeric_radix)(pTHX_ const char ** sp, const char * send);
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
#define Perl_my_sprintf DPPP_(my_my_sprintf)
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
#if defined(is_utf8_string) && defined(utf8_to_uvchr)
bool isuni = flags & PERL_PV_ESCAPE_UNI ? 1 : 0;
#endif
const char *pv = str;
const char * const end = pv + count;
octbuf[0] = esc;
if (!(flags & PERL_PV_ESCAPE_NOCLEAR))
sv_setpvs(dsv, "");
#if defined(is_utf8_string) && defined(utf8_to_uvchr)
if ((flags & PERL_PV_ESCAPE_UNI_DETECT) && is_utf8_string((U8*)pv, count))
isuni = 1;
#endif
for (; pv < end && (!max || wrote < max) ; pv += readsize) {
const UV u =
#if defined(is_utf8_string) && defined(utf8_to_uvchr)
isuni ? utf8_to_uvchr((U8*)pv, &readsize) :
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
#endif
