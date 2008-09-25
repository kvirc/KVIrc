#ifndef _KVI_WINCFG_H_
#define _KVI_WINCFG_H_
//=============================================================================
//
//   File : kvi_wincfg.h
//   Creation date : Fri Sep 21 16:21:09 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001-2008 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your opinion) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

/**
* \file kvi_wincfg.h
* \author Szymon Stefanek
* \brief Windows configuration: do not touch
*/

/** \def COMPILE_NEW_KVS Define if you want to compile with new parser */
#define COMPILE_NEW_KVS 1

/** \def COMPILE_ONLY_NEW_KVS Define if you want to use only the new KVS parser */
#define COMPILE_ONLY_NEW_KVS 1

/** \def COMPILE_WITH_NO_SOUND Define if you want to disable DCC VOICE sound support */
/* #undef COMPILE_WITH_NO_SOUND */

/** \def COMPILE_USE_GSM Define if you have libgsm and want the GSM code to be compiled */
/* #undef COMPILE_USE_GSM */

/** \def COMPILE_INFO_TIPS Define if you want the info tips to be compiled in */
/* #define COMPILE_INFO_TIPS 1 */

/** \def COMPILE_KDE_SUPPORT Define if you want to compile the KDE 2.x support */
/* #undef COMPILE_KDE_SUPPORT */

/** \def COMPILE_MEMORY_PROFILE Define if you want the gcc memory profile */
/* #undef COMPILE_MEMORY_PROFILE */

/** \def COMPILE_MEMORY_CHECKS Define if you want the memory checks */
/* #undef COMPILE_MEMORY_CHECKS */

/** \def COMPILE_WITH_SYSTEM_MEMMOVE Define if you trust your memmove() function */
#define COMPILE_WITH_SYSTEM_MEMMOVE 1

/** \def COMPILE_CRYPT_SUPPORT Define if you want to compile the crypt support */
#define COMPILE_CRYPT_SUPPORT 1

/** \def COMPILE_ix86_ASM Define if you want the i386 asm code */
/* #undef COMPILE_ix86_ASM */

/** \def COMPILE_USE_DYNAMIC_LABELS Define if your compiler supports gotos do dynamic labels */
/* #undef COMPILE_USE_DYNAMIC_LABELS */

/** \def COMPILE_LOCALE_STUFF Define if you want to compile the localization support */
#define COMPILE_LOCALE_STUFF 1

/** \def COMPILE_IPV6_SUPPORT Define if you want the IPv6 support */
#define COMPILE_IPV6_SUPPORT 1

/** \def COMPILE_ZLIB_SUPPORT Define if you want zlib support */
#define COMPILE_ZLIB_SUPPORT 1

/** \def COMPILE_THREADS_USE_POSIX Define if you want to use the pthreads library */
/* #undef COMPILE_THREADS_USE_POSIX */

/** \def COMPILE_THREADS_USE_SOLARIS_LIBTHREAD Define if you want to use the solaris libthread */
/* #undef COMPILE_THREADS_USE_SOLARIS_LIBTHREAD */

/** \def COMPILE_PSEUDO_TRANSPARENCY Define if you want the pseudo transparency */
#define COMPILE_PSEUDO_TRANSPARENCY

/** \def COMPILE_GET_INTERFACE_ADDRESS Define if you have the SIOCGIFADDR ioctl and the related headers */
/* #undef COMPILE_GET_INTERFACE_ADDRESS */

/** \def KVIRC_BUILD_DATE This is the build date (configure date rather) */
#define KVIRC_BUILD_DATE __DATE__" "__TIME__

/** \def KVIRC_BUILD_FLAGS This is a build flag */
#define KVIRC_BUILD_FLAGS "win32"
/** \def KVIRC_BUILD_COMMAND This is a build flag */
#define KVIRC_BUILD_COMMAND "win32"
/** \def KVIRC_BUILD_SYSTEM This is a build flag */
#define KVIRC_BUILD_SYSTEM "win32"
/** \def KVIRC_BUILD_CPU This is a build flag */
#define KVIRC_BUILD_CPU "win32"
/** \def KVIRC_BUILD_COMPILER This is a build flag */
#define KVIRC_BUILD_COMPILER "win32"
/** \def KVIRC_BUILD_COMPILER_FLAGS This is a build flag */
#define KVIRC_BUILD_COMPILER_FLAGS "win32"

/** \def HAVE_STRINGS_H Define if you have strings.h */
/* #undef HAVE_STRINGS_H */

/** \def HAVE_GETENV Define if you have the getenv function */
/* #undef HAVE_GETENV */

/** \def HAVE_STRERROR Define if you have the strerror function */
#define HAVE_STRERROR 1

/** \def HAVE_INET_ATON Define if you have the inet_aton function */
/* #define HAVE_INET_ATON 1 */

/** \def HAVE_INET_NTOA Define if you have the inet_ntoa function */
/* #define HAVE_INET_NTOA 1 */

/** \def HAVE_INET_PTON Define if you have the inet_pton function */
/* #undef HAVE_INET_PTON */

/** \def HAVE_INET_NTOP Define if you have the inet_ntop function */
/* #undef HAVE_INET_NTOP */

/** \def HAVE_GETNAMEINFO Define if you have the getnameinfo function */
#define HAVE_GETNAMEINFO

/** \def HAVE_GETADDRINFO Define if you have the getaddrinfo function */
#define HAVE_GETADDRINFO

/** \def AVERAGE_CHANNEL_USERS Define this to the number of average channel users */
#define AVERAGE_CHANNEL_USERS 101

/** \def COMPILE_IGNORE_SIGALARM Define this if you want to ignore the SIGALARM signal */
/* #undef COMPILE_IGNORE_SIGALARM */

/** \def BIG_ENDIAN_MACHINE_BYTE_ORDER Define this if you are on a big endian machine */
/* #undef BIG_ENDIAN_MACHINE_BYTE_ORDER */

/** \def COMPILE_USE_LOCAL_8BIT Define this if you want to use the unicode-local 8bit charset translation */
/* #undef COMPILE_USE_LOCAL_8BIT */

/** \def COMPILE_NO_IPC Define this if you want to disable the inter-process communication support */
/* #undef COMPILE_NO_IPC */

/** \def HAVE_GETENV Define if you have the `getenv' function. */
/* #undef HAVE_GETENV */

/** \def HAVE_LINUX_SOUNDCARD_H Define if you have the <linux/soundcard.h> header file. */
/* #undef HAVE_LINUX_SOUNDCARD_H */

/** \def HAVE_SOUNDCARD_H Define if you have the <soundcard.h> header file. */
/* #undef HAVE_SOUNDCARD_H */

/** \def HAVE_SYS_SOUNDCARD_H Define if you have the <sys/soundcard.h> header file. */
/* #undef HAVE_SYS_SOUNDCARD_H */

/** \def PACKAGE Name of package */
#define PACKAGE "kvirc"

/** \def VERSION Version number of package */
#define VERSION "4.0.0"

/** \def VERSION_BRANCH Version branch number of package */
#define VERSION_BRANCH "4.0"

/** \def _POSIX_1_SOURCE Define if the system does not provide POSIX.1 features except with this defined. */
/* #undef _POSIX_1_SOURCE */

/** \def _POSIX_SOURCE Define if you need to in order for `stat' and other things to work. */
/* #undef _POSIX_SOURCE */

/** \def COMPILE_WITH_NO_SOUND Define if you want DCC Voice support */
// for now we need this....dcc voice is not ported yet
#define COMPILE_WITH_NO_SOUND

/** \def COMPILE_DISABLE_DCC_VOICE Define if you don't want DCC Voice support */
#define COMPILE_DISABLE_DCC_VOICE

/** \def COMPILE_NO_X No X support */
#define COMPILE_NO_X

/** \def COMPILE_NO_X_BELL No X bell! */
#define COMPILE_NO_X_BELL

/** \def COMPILE_SSL_SUPPORT Define if you have openssl and want the ssl support in kvirc */
#define COMPILE_SSL_SUPPORT

/** \def COMPILE_PERL_SUPPORT Define if you want perl scripting support */
#define COMPILE_PERL_SUPPORT

/** \def SIZEOF_INT The size of a `int', as computed by sizeof. */
#define SIZEOF_INT 4

/** \def SIZEOF_LONG_INT The size of a `long int', as computed by sizeof. */
#define SIZEOF_LONG_INT 4

/** \def SIZEOF_LONG_LONG_INT The size of a `long long int', as computed by sizeof. */
#define SIZEOF_LONG_LONG_INT 8

/** \def SIZEOF_SHORT_INT The size of a `short int', as computed by sizeof. */
#define SIZEOF_SHORT_INT 2

/** \def SIZEOF_UNSIGNED_INT The size of a `unsigned int', as computed by sizeof. */
#define SIZEOF_UNSIGNED_INT 4

/** \def SIZEOF_UNSIGNED_LONG_INT The size of a `unsigned long int', as computed by sizeof. */
#define SIZEOF_UNSIGNED_LONG_INT 4

/** \def SIZEOF_UNSIGNED_LONG_LONG_INT The size of a `unsigned long long int', as computed by sizeof. */
#define SIZEOF_UNSIGNED_LONG_LONG_INT 8

/** \def SIZEOF_UNSIGNED_SHORT_INT The size of a `unsigned short int', as computed by sizeof. */
#define SIZEOF_UNSIGNED_SHORT_INT 2

#endif //_KVI_WINCFG_H_
