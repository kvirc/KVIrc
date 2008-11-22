#ifndef _KVI_WINCFG_H_
#define _KVI_WINCFG_H_

//=============================================================================
//
//   File : kvi_wincfg.h
//   Creation date : Fri Sep 21 16:21:09 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
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

//=============================================================================
// Windows configuration: do not touch
//=============================================================================

/* define if you want to compile with new parser */
#define COMPILE_NEW_KVS 1

/* define if you want to use the Qt 4.x support */
#undef COMPILE_USE_QT4

//#define COMPILE_USE_QT4

/* define if you want to use only the new KVS parser */
#define COMPILE_ONLY_NEW_KVS 1

/* define if you want to disable DCC VOICE sound support */
/* #undef COMPILE_WITH_NO_SOUND */

/* define if you have libgsm and want the GSM code to be compiled */
/* #undef COMPILE_USE_GSM */

/* define if you want the info tips to be compiled in */
#define COMPILE_INFO_TIPS 1

/* define if you want to compile the KDE 2.x support */
/* #undef COMPILE_KDE_SUPPORT */

/* define if you want the gcc memory profile */
/* #undef COMPILE_MEMORY_PROFILE */

/* define if you want the memory checks */
/* #undef COMPILE_MEMORY_CHECKS */

/* define if you trust your memmove() function */
#define COMPILE_WITH_SYSTEM_MEMMOVE 1

/* define if you want to compile the crypt support */
#define COMPILE_CRYPT_SUPPORT 1

/* define if you want the i386 asm code */
/* #undef COMPILE_ix86_ASM */

/* define if your compiler supports gotos do dynamic labels */
/* #undef COMPILE_USE_DYNAMIC_LABELS */

/* define if you want to compile the localization support */
#define COMPILE_LOCALE_STUFF 1

/* define if you want the IpV6 support */
#define COMPILE_IPV6_SUPPORT 1

/* define if you want zlib support */
#define COMPILE_ZLIB_SUPPORT 1

/* define if you want to use the pthreads library */
/* #undef COMPILE_THREADS_USE_POSIX */

/* define if you want to use the solaris libthread */
/* #undef COMPILE_THREADS_USE_SOLARIS_LIBTHREAD */

#define COMPILE_PSEUDO_TRANSPARENCY

/* define if you have the SIOCGIFADDR ioctl and the related headers */
/* #undef COMPILE_GET_INTERFACE_ADDRESS */

/* this is the build date (configure date rather) */
#define BUILD_DATE __DATE__" "__TIME__

/* these are the build flags */
#define BUILD_FLAGS "win32"

/* define if you have strings.h */
/* #undef HAVE_STRINGS_H */

/* define if you have the getenv function */
/* #undef HAVE_GETENV */

/* define if you have the strerror function */
#define HAVE_STRERROR 1

/* define if you have the inet_aton function */
/* #define HAVE_INET_ATON 1 */

/* define if you have the inet_ntoa function */
/* #define HAVE_INET_NTOA 1 */

/* define if you have the inet_pton function */
/* #undef HAVE_INET_PTON */

/* define if you have the inet_ntop function */
/* #undef HAVE_INET_NTOP */

/* define if you have the getnameinfo function */
#define HAVE_GETNAMEINFO

/* define if you have the getaddrinfo function */
#define HAVE_GETADDRINFO

/* define this to the number of average channel users */
#define AVERAGE_CHANNEL_USERS 101

/* define this if you want to ignore the SIGALARM signal */
/* #undef COMPILE_IGNORE_SIGALARM */

/* define this if you are on a big endian machine */
/* #undef BIG_ENDIAN_MACHINE_BYTE_ORDER */

/* define this if you want to use the unicode-local 8bit charset translation */
/* #undef COMPILE_USE_LOCAL_8BIT */

/* define this if you want to disable the inter-process communication support */
/* #undef COMPILE_NO_IPC */

/* Define if you have the `getenv' function. */
/* #undef HAVE_GETENV */

/* Define if you have the <linux/soundcard.h> header file. */
/* #undef HAVE_LINUX_SOUNDCARD_H */

/* Define if you have the <soundcard.h> header file. */
/* #undef HAVE_SOUNDCARD_H */

/* Define if you have the <sys/soundcard.h> header file. */
/* #undef HAVE_SYS_SOUNDCARD_H */

/* Name of package */
#define PACKAGE "kvirc"

/* Version number of package */
#define VERSION "3.4.2"

#define VERSION_BRANCH "3.4"


/* Define if the system does not provide POSIX.1 features except with this
   defined. */
/* #undef _POSIX_1_SOURCE */

/* Define if you need to in order for `stat' and other things to work. */
/* #undef _POSIX_SOURCE */

// for now we need this....dcc voice is not ported yet
#define COMPILE_WITH_NO_SOUND

#define COMPILE_DISABLE_DCC_VOICE

/* No X support */
#define COMPILE_NO_X

/* No X bell! */
#define COMPILE_NO_X_BELL

/* define if you have openssl and want the ssl support in kvirc */
#define COMPILE_SSL_SUPPORT

/* define if you want perl scripting support */
#define COMPILE_PERL_SUPPORT

/* The size of a `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of a `long int', as computed by sizeof. */
#define SIZEOF_LONG_INT 4

/* The size of a `long long int', as computed by sizeof. */
#define SIZEOF_LONG_LONG_INT 8

/* The size of a `short int', as computed by sizeof. */
#define SIZEOF_SHORT_INT 2

/* The size of a `unsigned int', as computed by sizeof. */
#define SIZEOF_UNSIGNED_INT 4

/* The size of a `unsigned long int', as computed by sizeof. */
#define SIZEOF_UNSIGNED_LONG_INT 4

/* The size of a `unsigned long long int', as computed by sizeof. */
#define SIZEOF_UNSIGNED_LONG_LONG_INT 8

/* The size of a `unsigned short int', as computed by sizeof. */
#define SIZEOF_UNSIGNED_SHORT_INT 2

#endif //_KVI_WINCFG_H_
