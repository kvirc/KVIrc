/* Look at .appveyor.yml to understand what is this */
#ifndef kvirc_perl_workaround_h_
#define kvirc_perl_workaround_h_
#ifdef PERL_STATIC_INLINE
#undef PERL_STATIC_INLINE
#endif
#endif
/* Work around for using MSVC and Strawberry Perl 5.32. */
#ifdef _MSC_VER
# define __builtin_expect(expr,val) (expr)
#endif