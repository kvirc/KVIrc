###############################################################################
##
## KVIrc CMake global configuration file.
##
###############################################################################

###############################################################################
# GLOBAL DEFS
###############################################################################

# Name of the project
PROJECT(kvirc)

# Set conventional loops
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

# Minimum cmake version
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.4)

if(POLICY CMP0017)
	cmake_policy(SET CMP0017 NEW)
endif(POLICY CMP0017)

# Check modules first in local dir
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

# Some vars
SET(PACKAGE "kvirc")
SET(NICENAME "KVIrc")
SET(VERSION_MAJOR "4")
SET(VERSION_BRANCH "4.1")
SET(VERSION_RELEASE "4.1.1")

STRING(REGEX MATCH "[0-9]+$" VERSION_MINOR ${VERSION_BRANCH})

SET(CMAKE_KVIRC_VERSION_MAJOR ${VERSION_MAJOR})
SET(CMAKE_KVIRC_VERSION_BRANCH ${VERSION_BRANCH})
SET(CMAKE_KVIRC_VERSION_RELEASE ${VERSION_RELEASE})
SET(CMAKE_KVIRC_BUILD_COMMAND ${CMAKE_COMMAND})
SET(CMAKE_KVIRC_BUILD_FLAGS)
SET(CMAKE_KVIRC_BUILD_SYSTEM ${CMAKE_SYSTEM})
SET(CMAKE_KVIRC_BUILD_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})
SET(CMAKE_KVIRC_BUILD_SYSTEM_VERSION ${CMAKE_SYSTEM_VERSION})
SET(CMAKE_KVIRC_BUILD_CPU ${CMAKE_SYSTEM_PROCESSOR})
SET(CMAKE_KVIRC_BUILD_COMPILER ${CMAKE_CXX_COMPILER})
SET(CMAKE_KVIRC_BUILD_COMPILER_FLAGS ${CMAKE_CXX_FLAGS})

# Suffix for Linux
SET(LIB_SUFFIX
	CACHE STRING "Define suffix of directory name (32/64)"
)

# Get revision number from SVN
SET(MANUAL_REVISION
	CACHE STRING "Manually set the revision number"
)

STRING(COMPARE EQUAL "${MANUAL_REVISION}" "" MANUAL_REVISION_UNSET)
IF(MANUAL_REVISION_UNSET)
	# No manually forced revision number, gotta guess one now
	IF(EXISTS "${CMAKE_SOURCE_DIR}/.svn" AND IS_DIRECTORY "${CMAKE_SOURCE_DIR}/.svn")
		# We're in a svn structure, let's get the revision number using subversion
		FIND_PACKAGE(Subversion)
		IF(Subversion_FOUND)
			MESSAGE(STATUS "Found Subversion ${Subversion_VERSION_SVN}")
			Subversion_WC_INFO(${PROJECT_SOURCE_DIR} KVIrc)
			IF(DEFINED KVIrc_WC_REVISION)
				SET(CMAKE_KVIRC_BUILD_REVISION ${KVIrc_WC_REVISION})
				MESSAGE(STATUS "Current revision is ${KVIrc_WC_REVISION}")
			ELSE()
				MESSAGE(STATUS "No Subversion working copy found")
			ENDIF()
		ELSE()
			MESSAGE("Subversion not found. No revision number will be used")
		ENDIF()
	ELSE()
		IF(EXISTS "${CMAKE_SOURCE_DIR}/.svnrevision")
			# We're in a release package created using makedist.sh, so we should include a .svnrevision file
			FILE(READ "${CMAKE_SOURCE_DIR}/.svnrevision" DISTSVNREVISION)
			STRING(STRIP "${DISTSVNREVISION}" CMAKE_KVIRC_BUILD_REVISION)
			MESSAGE(STATUS "SVN revision is ${CMAKE_KVIRC_BUILD_REVISION}")
		ELSE()
			MESSAGE(STATUS "No revision number found.")
		ENDIF()
	ENDIF()

ELSE()
	# Manually forced revision number
	SET(CMAKE_KVIRC_BUILD_REVISION ${MANUAL_REVISION})
	MESSAGE(STATUS "Current revision has been manually set to ${MANUAL_REVISION}")
ENDIF()


# Simplify eventual mixed revisions and strange tweaks with letters in it
STRING(REGEX REPLACE ":[0-9a-zA-Z]+" "" CMAKE_KVIRC_BUILD_REVISION "${CMAKE_KVIRC_BUILD_REVISION}")
STRING(REGEX REPLACE "[^0-9]+" "" CMAKE_KVIRC_BUILD_REVISION "${CMAKE_KVIRC_BUILD_REVISION}")


# Set man dir
SET(MANDIR
	CACHE STRING "Define man directory starting from $CMAKE_INSTALL_PREFIX"
)

STRING(COMPARE EQUAL "${MANDIR}" "" MANDIR_UNSET)
IF(MANDIR_UNSET)
	SET(MANDIR_PREFIX "share/man")
ELSE()
	SET(MANDIR_PREFIX "man")
ENDIF()
LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS MANDIR=${MANDIR_PREFIX})

# Get the build date
TRY_RUN(TEST_DID_RUN TEST_DID_COMPILE "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/datetime.cpp" OUTPUT_VARIABLE TEST_BUILD_DATE)
IF(TEST_DID_COMPILE)
	STRING(REGEX MATCH "[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9] [0-9][0-9]:[0-9][0-9]:[0-9][0-9] [A-Z][A-Z][A-Z]" CMAKE_KVIRC_BUILD_DATE "${TEST_BUILD_DATE}")
ELSE()
	SET(CMAKE_KVIRC_BUILD_DATE "ERROR")
ENDIF()

# Set var to link with, for later use
SET(LIBS)
SET(ADDITIONAL_LINK_FLAGS)

# Install path
IF(UNIX)
	IF(APPLE)
		# MacOS X
		# Due to the special bundle structure ignore
		# the prefix eventually set by the user.
		SET(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/KVIrc.app)
	ELSE()
		# Linux / BSD
		IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
			#fix package build
			IF(PREFIX)
				SET(CMAKE_INSTALL_PREFIX ${PREFIX})
			ELSE()
				SET(CMAKE_INSTALL_PREFIX /usr/local)
			ENDIF()
		ENDIF()
	ENDIF()
ELSEIF(WIN32)
	SET(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/release)
ENDIF()
LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX})


###############################################################################
#
# Release type
#
###############################################################################
OPTION(WANT_ENV_FLAGS "Don't try to set CMAKE_C(XX)_FLAGS; instead, use the ones from environment variables" OFF)

OPTION(WANT_DEBUG "Compile debug symbols" OFF)
IF(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo OR DEBUG OR WITH_DEBUG)
	SET(WANT_DEBUG ON CACHE BOOL "Compile debug symbols" FORCE)
	SET(COMPILE_DEBUG_MODE 1)
ENDIF()

OPTION(WANT_STRIP "Strip binaries (discard symbols from object files)" OFF)

IF(WANT_ENV_FLAGS)
	SET(CMAKE_BUILD_TYPE Undefined)
	SET(CMAKE_STATUS_DEBUG_SUPPORT "Environment-driven")
ELSE()
	IF(WANT_DEBUG)
		SET(WANT_STRIP OFF CACHE BOOL "Strip binaries (discard symbols from object files)" FORCE)
		SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
		SET(CMAKE_STATUS_DEBUG_SUPPORT "Yes")
		IF(CMAKE_COMPILER_IS_GNUCXX)
			#force gdb format of debug symbols
			SET(CMAKE_CXX_FLAGS "-O0 -ggdb -pipe --exec-charset=UTF-8 --input-charset=UTF-8 --no-implement-inlines --unit-at-a-time --fast-math")
			SET(CMAKE_C_FLAGS "-O0 -ggdb -pipe --exec-charset=UTF-8 --input-charset=UTF-8 --no-implement-inlines --unit-at-a-time --fast-math")
			INCLUDE(CheckCXXCompilerFlag)
			CHECK_CXX_COMPILER_FLAG("-fvisibility=hidden" CXX_HAS_VH_FLAG)
			CHECK_CXX_COMPILER_FLAG("-fvisibility-inlines-hidden" CXX_HAS_VIH_FLAG)
			#gcc doesn't support visibility on PE/win32
			IF(NOT MINGW)
				IF(CXX_HAS_VH_FLAG)
					SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
					SET(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
				ENDIF()
				IF(CXX_HAS_VIH_FLAG)
					SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
					SET(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
				ENDIF()
			ENDIF()
		ELSE()
			IF(MSVC)
				#CMake default options are conflicting for some modules, such as perl
				STRING(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG_INIT ${CMAKE_CXX_FLAGS_DEBUG_INIT})
				STRING(REPLACE "/RTC1" "" CMAKE_C_FLAGS_DEBUG_INIT ${CMAKE_C_FLAGS_DEBUG_INIT})
				STRING(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
				STRING(REPLACE "/RTC1" "" CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
				#this is the equivalent of -O0 -g according to msdn
				SET(CMAKE_CXX_FLAGS "/Od /Zi")
				SET(CMAKE_C_FLAGS "/Od /Zi")
			ELSE()
				SET(CMAKE_CXX_FLAGS "-O0 -g")
				SET(CMAKE_C_FLAGS "-O0 -g")
			ENDIF()
		ENDIF()
	ELSE()
		IF(WANT_STRIP)
			FIND_PROGRAM(STRIP_EXECUTABLE NAMES strip)
			IF(STRIP_EXECUTABLE)
				SET(CMAKE_STATUS_DEBUG_SUPPORT "No (Stripped)")
				SET(CMAKE_BUILD_TYPE Release-stripped CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
			ELSE()
				SET(CMAKE_STATUS_DEBUG_SUPPORT "No (Release)")
				SET(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
			ENDIF()
		ELSE()
			SET(CMAKE_STATUS_DEBUG_SUPPORT "No (Release)")
			SET(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
		ENDIF()
		IF(CMAKE_COMPILER_IS_GNUCXX)
			#force gdb options
			SET(CMAKE_CXX_FLAGS "-O3 -fomit-frame-pointer -DNDEBUG --no-enforce-eh-specs -pipe --exec-charset=UTF-8 --input-charset=UTF-8 --no-implement-inlines --unit-at-a-time --fast-math")
			SET(CMAKE_C_FLAGS "-O3 -fomit-frame-pointer -DNDEBUG --no-enforce-eh-specs -pipe --exec-charset=UTF-8 --input-charset=UTF-8 --no-implement-inlines --unit-at-a-time --fast-math")
			INCLUDE(CheckCXXCompilerFlag)
			CHECK_CXX_COMPILER_FLAG("-fvisibility-inlines-hidden" CXX_HAS_VIH_FLAG)
			#gcc doesn't support visibility on PE/win32
			IF(CXX_HAS_VIH_FLAG AND NOT MINGW)
				SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
				SET(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
			ENDIF()
		ELSE()
			IF(MSVC)
				#CMake default options are conflicting for some modules, such as perl
				STRING(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG_INIT ${CMAKE_CXX_FLAGS_DEBUG_INIT})
				STRING(REPLACE "/RTC1" "" CMAKE_C_FLAGS_DEBUG_INIT ${CMAKE_C_FLAGS_DEBUG_INIT})
				STRING(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
				STRING(REPLACE "/RTC1" "" CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
				SET(CMAKE_CXX_FLAGS "/Ox")
				SET(CMAKE_C_FLAGS "/Ox")
			ELSE()
				SET(CMAKE_CXX_FLAGS "-O3")
				SET(CMAKE_C_FLAGS "-O3")
			ENDIF()
		ENDIF()
	ENDIF()
ENDIF()

############################################################################
# Check for KDE4
# This has been moved to the top of CMakeLists.txt, since FindKde4 overwrite
# cmake's module search paths, forcing us to use their version of these
# modules (eg: FindOpenSSL, see bug #655)
############################################################################

OPTION(WANT_KDE4 "Whether to use KDE4 support" ON)
IF(WANT_KDE4)
	FIND_PACKAGE(KDE4)
	IF(KDE4_FOUND)
		INCLUDE_DIRECTORIES(${KDE4_INCLUDES})
		ADD_DEFINITIONS(${KDE4_DEFINITIONS})
		SET(COMPILE_KDE_SUPPORT 1)
		LIST(APPEND LIBS ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBRARY} ${KDE4_KPARTS_LIBRARY} ${KDE4_KFILE_LIBRARY} ${KDE4_KIO_LIBRARY} ${KDE4_SOLID_LIBRARY})
		SET(CMAKE_STATUS_KDE4_SUPPORT "Yes")

		# The KDE4 cmake module has a VERY WEIRD behaviour: it adds -Wl,--no-undefined
		# to the CMAKE_MODULE_LINKER_FLAGS .. which is BAD
		STRING(REPLACE "-Wl,--no-undefined" "" CMAKE_MODULE_LINKER_FLAGS ${CMAKE_MODULE_LINKER_FLAGS})
	ELSE()
		SET(CMAKE_STATUS_KDE4_SUPPORT "Not available")
	ENDIF()
ELSE()
	SET(CMAKE_STATUS_KDE4_SUPPORT "No")
ENDIF()

###############################################################################
#
# Verbose Makefile
#
###############################################################################

OPTION(WANT_VERBOSE "Generate verbose Makefiles" OFF)
IF(WANT_VERBOSE)
	SET(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "Generate verbose Makefiles")
	SET(CMAKE_STATUS_VERBOSE_SUPPORT "Yes")
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS VERBOSE)
ELSE()
	SET(CMAKE_STATUS_VERBOSE_SUPPORT "No")
ENDIF()


###############################################################################
#
# Version compatibility switch: appends VERSION_MAJOR to kvirc and kvilib
#
###############################################################################

OPTION(WANT_COEXISTENCE "Append version to binaries to maintain compatibility with older versions" OFF)
IF(WANT_COEXISTENCE)
	SET(KVIRC_BINARYNAME ${PACKAGE}${VERSION_MAJOR})
	SET(KVILIB_BINARYNAME kvilib${VERSION_MAJOR})
	SET(CMAKE_STATUS_COEXISTENCE_VER "Yes")
ELSE()
	SET(KVIRC_BINARYNAME ${PACKAGE})
	SET(KVILIB_BINARYNAME kvilib)
	SET(CMAKE_STATUS_COEXISTENCE_VER "No")
ENDIF()

###############################################################################
#
# Module install path: kvirc needs to know where they are installed
#
###############################################################################

IF(UNIX)
	IF(APPLE)
		SET(CMAKE_KVIRC_RESOURCES_DIR "${CMAKE_INSTALL_PREFIX}/Contents/Resources/")
	ELSE()
		# Assume linux
		SET(CMAKE_KVIRC_RESOURCES_DIR "${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/")
		SET(CMAKE_KVIRC_MODULES_DIR "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/kvirc/${VERSION_BRANCH}/modules")
		LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS LIB_SUFFIX=${LIB_SUFFIX})
	ENDIF()
ELSEIF(WIN32)
	SET(CMAKE_KVIRC_RESOURCES_DIR "${CMAKE_INSTALL_PREFIX}/")
	SET(CMAKE_KVIRC_MODULES_DIR "${CMAKE_INSTALL_PREFIX}/modules/")
ENDIF()

###############################################################################
#
# System checks
#
###############################################################################

INCLUDE(CheckTypeSize)
CHECK_TYPE_SIZE("short" CMAKE_SYSTEM_SIZE_OF_SHORT_INT)
CHECK_TYPE_SIZE("int" CMAKE_SYSTEM_SIZE_OF_INT)
CHECK_TYPE_SIZE("long" CMAKE_SYSTEM_SIZE_OF_LONG_INT)
CHECK_TYPE_SIZE("long long" CMAKE_SYSTEM_SIZE_OF_LONG_LONG_INT)

INCLUDE(CheckIncludeFiles)
CHECK_INCLUDE_FILES(sys/types.h SYSTEM_HAS_SYS_TYPES_H)
CHECK_INCLUDE_FILES(strings.h SYSTEM_HAS_STRINGS_H)

# "dynamic labels" compiler support
TRY_RUN(TEST_DID_RUN_DYNLABELS TEST_DID_COMPILE_DYNLABELS "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/dynlabels.cpp")
IF(TEST_DID_COMPILE_DYNLABELS)
	STRING(COMPARE EQUAL "${TEST_DID_RUN_DYNLABELS}" "0" TEST_DID_RUN_DYNLABELS_SUCCESS)
	IF(TEST_DID_RUN_DYNLABELS_SUCCESS)
		SET(COMPILE_USE_DYNAMIC_LABELS 1)
	ELSE()
		SET(COMPILE_USE_DYNAMIC_LABELS 0)
	ENDIF()
ELSE()
	SET(COMPILE_USE_DYNAMIC_LABELS 0)
ENDIF()

###############################################################################
# Option for precompiled headers usage (currently broken because of bad gcc support)
###############################################################################
#
#OPTION(WANT_PCH "Whether to use pre-compiled headers" ON)
#IF(WANT_PCH)
#	FIND_PACKAGE(PCHSupport)
#	IF(PCHSupport_FOUND)
#		SET(CMAKE_STATUS_PCH_SUPPORT "Yes")
#		LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS -DUSE_PCH=1)
#	ELSE()
#		SET(CMAKE_STATUS_PCH_SUPPORT "Not available")
#	ENDIF()
#ELSE()
#	ADD_DEFINITIONS(-DUSE_PCH=0)
#	SET(CMAKE_STATUS_PCH_SUPPORT "User disabled")
#ENDIF()
#

###############################################################################
#
# Library and tool checks, in order. First the required ones
# in order of dependancy, then the optional ones.
#
###############################################################################

# ingore sigalarm (useful with solaris threads)
OPTION(WANT_IGNORE_SIGALARM "Ignore SIGALARM signal" OFF)
# Check for threads
FIND_PACKAGE(Threads REQUIRED)
IF(CMAKE_USE_WIN32_THREADS_INIT OR CMAKE_USE_PTHREADS_INIT)
	IF(CMAKE_USE_WIN32_THREADS_INIT)
		# No need to link to any specific library
		SET(COMPILE_THREADS_USE_WIN32 1)
		SET(CMAKE_STATUS_THREADS_SUPPORT "Win32")
		LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS Threads=Win32)
	ELSE()
		IF(CMAKE_HAVE_THREADS_LIBRARY)
			LIST(APPEND LIBS ${CMAKE_THREAD_LIBS_INIT})
		ENDIF()
		SET(COMPILE_THREADS_USE_POSIX 1)
		LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS Threads=POSIX)
		# FIXME: Solaris threads ?
		#SET(COMPILE_THREADS_USE_SOLARIS_LIBTHREAD 1)
		#SET(CMAKE_STATUS_THREADS_SUPPORT "SOLARIS")
		#LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS Threads=SOLARIS)
		IF(WANT_IGNORE_SIGALARM)
			SET(CMAKE_STATUS_THREADS_SUPPORT "POSIX (ignoring SIGALARM)")
			SET(COMPILE_IGNORE_SIGALARM 1)
		ELSE()
			SET(CMAKE_STATUS_THREADS_SUPPORT "POSIX")
		ENDIF()
	ENDIF()
ELSE()
	SET(CMAKE_STATUS_THREADS_SUPPORT "Not used")
ENDIF()

# Check for ZLib
FIND_PACKAGE(ZLIB REQUIRED)
IF(ZLIB_FOUND)
	INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})
	LIST(APPEND LIBS ${ZLIB_LIBRARIES})
	SET(COMPILE_ZLIB_SUPPORT 1)
ENDIF()

############################################################################
# Check for Qt4
############################################################################

# We require at least Qt 4.6.x
FIND_PACKAGE(Qt4 4.6.0 REQUIRED)
IF(QT4_FOUND)
	SET(QT_USE_QTSQL true)
	SET(QT_USE_QTXML true)
	SET(QT_USE_QTNETWORK true)

	# Qt-DBus support
	OPTION(WANT_QTDBUS "Whether to compile Qt-DBus support" ON)
	IF(WANT_QTDBUS)
		IF(QT_QTDBUS_FOUND)
			SET(QT_USE_QTDBUS true)
			SET(CMAKE_STATUS_QTDBUS_SUPPORT "Yes")
			SET(COMPILE_DBUS_SUPPORT 1)
		ELSE()
			SET(CMAKE_STATUS_QTDBUS_SUPPORT "Not available")
		ENDIF()
	ELSE()
		SET(CMAKE_STATUS_QTDBUS_SUPPORT "No")
	ENDIF()

	# Qt-WebKit support
	OPTION(WANT_QTWEBKIT "Whether to compile Qt-WebKit support" ON)
	IF(WANT_QTWEBKIT)
		IF(QT_QTWEBKIT_FOUND)
			SET(QT_USE_QTWEBKIT true)
			SET(CMAKE_STATUS_QTWEBKIT_SUPPORT "Yes")
			SET(COMPILE_WEBKIT_SUPPORT 1)
		ELSE()
			SET(CMAKE_STATUS_QTWEBKIT_SUPPORT "Not available")
		ENDIF()
	ELSE()
		SET(CMAKE_STATUS_QTWEBKIT_SUPPORT "No")
	ENDIF()
	# Qt-WebKit support
	OPTION(WANT_QTSVG "Whether to compile Qt-Svg support" ON)
	IF(WANT_QTSVG)
		IF(QT_QTSVG_FOUND)
			SET(QT_USE_QTSVG true)
			SET(CMAKE_STATUS_QTSVG_SUPPORT "Yes")
			SET(COMPILE_SVG_SUPPORT 1)
		ELSE()
			SET(CMAKE_STATUS_QTSVG_SUPPORT "Not available")
		ENDIF()
	ELSE()
		SET(CMAKE_STATUS_QTSVG_SUPPORT "No")
	ENDIF()



	INCLUDE(${QT_USE_FILE})
	INCLUDE_DIRECTORIES(${QT_INCLUDES})
	LIST(APPEND LIBS ${QT_LIBRARIES})
ENDIF()

# Check for X11
IF(UNIX)
	IF(APPLE)
		SET(CMAKE_STATUS_X11_SUPPORT "Not used")
	ELSE()
		# Assume linux
		FIND_PACKAGE(X11)
		IF(X11_FOUND)
			SET(COMPILE_X11_SUPPORT 1)
			IF(X11_Xscreensaver_FOUND)
				SET(COMPILE_XSS_SUPPORT 1)
				SET(CMAKE_STATUS_X11_SUPPORT "Yes, with scrnsaverproto")
				INCLUDE_DIRECTORIES(${X11_Xscreensaver_INCLUDE_PATH})
				LIST(APPEND LIBS ${X11_Xscreensaver_LIB})
			ELSE()
				SET(CMAKE_STATUS_X11_SUPPORT "Yes")
			ENDIF()

			INCLUDE_DIRECTORIES(${X11_INCLUDE_DIR})
			LIST(APPEND LIBS ${X11_LIBRARIES})
			# Seems like ${X11_LIBRARIES} doesn't add -lXrender, which is needed
			# for building with --no-add-needed/binutils-gold, see
			# <http://bugs.debian.org/555068> for details.
			IF(X11_Xrender_FOUND)
				LIST(APPEND LIBS ${X11_Xrender_LIB})
			ENDIF(X11_Xrender_FOUND)
		ELSE()
			SET(CMAKE_STATUS_X11_SUPPORT "Not available")
		ENDIF()
	ENDIF()
ELSE()
	SET(CMAKE_STATUS_X11_SUPPORT "Not needed")
ENDIF()

# Phonon support
OPTION(WANT_PHONON "Whether to compile Phonon support" ON)
IF(WANT_PHONON)
	FIND_PACKAGE(Phonon)
	IF(PHONON_FOUND)
		SET(COMPILE_PHONON_SUPPORT 1)
	        LIST(APPEND LIBS ${PHONON_LIBRARY})
		LIST(APPEND CMAKE_STATUS_AUDIO_BACKENDS "Phonon")
	ENDIF()
ENDIF()

# esd
OPTION(WANT_ESD "Whether to compile ESD support" ON)
IF(WANT_ESD)
	FIND_PACKAGE(ESD)
	IF(ESD_FOUND)
		SET(COMPILE_ESD_SUPPORT 1)
	        LIST(APPEND LIBS ${ESD_LIBRARY})
		LIST(APPEND CMAKE_STATUS_AUDIO_BACKENDS "Esd")
	ENDIF()
ENDIF()

# oss + audiofile
OPTION(WANT_OSS "Whether to compile OSS support" ON)
OPTION(WANT_AUDIOFILE "Whether to compile Audiofile support" ON)

IF(WANT_OSS)
	FIND_PACKAGE(OSS)
	IF(OSS_FOUND)
		SET(COMPILE_OSS_SUPPORT 1)
	ENDIF()

	IF(WANT_AUDIOFILE)
		FIND_PACKAGE(Audiofile)
		IF(AUDIOFILE_FOUND)
			SET(COMPILE_AUDIOFILE_SUPPORT 1)
			LIST(APPEND LIBS ${AUDIOFILE_LIBRARY})
			LIST(APPEND CMAKE_STATUS_AUDIO_BACKENDS "Oss+AudioFile")
		ENDIF()
	ELSE()
		LIST(APPEND CMAKE_STATUS_AUDIO_BACKENDS "Oss")
	ENDIF()
ENDIF()

#winmm - win32
IF(WIN32)
	LIST(APPEND CMAKE_STATUS_AUDIO_BACKENDS "WinMM")
ENDIF()

############################################################################
# Check for MacOS X universal binary
############################################################################

IF(APPLE)
	OPTION(WANT_UNIVERSAL_BINARY "Whether to compile an Universal Binary on MacOSX" OFF)
	IF(WANT_UNIVERSAL_BINARY)
		SET(CMAKE_OSX_ARCHITECTURES ppc;i386)
		SET(CMAKE_STATUS_BUILD_UNIVERSAL_BINARY "User enabled")
		#FIXME: This should be handled with CMAKE_OSX_SYSROOT (TOM)
		#SET(CMAKE_SHARED_LINKER_FLAGS "-mmacosx-version-min=10.4")
		#SET(CMAKE_EXE_LINKER_FLAGS "-mmacosx-version-min=10.4")
	ELSE()
		SET(CMAKE_STATUS_BUILD_UNIVERSAL_BINARY "No")
	ENDIF()
ELSE()
	SET(CMAKE_STATUS_BUILD_UNIVERSAL_BINARY "Not available")
ENDIF()

############################################################################
# Crypto++ support: an alternative to openssl
############################################################################

OPTION(WANT_CRYPTOPP "Whether to use Crypto++ library" OFF)
OPTION(WANT_OPENSSL "Whether to use OpenSSL library" ON)

IF(WANT_CRYPTOPP)
	FIND_PACKAGE(PkgConfig)

	# for every os/distribution
	pkg_check_modules(CRYPTOPP cryptopp>=5.6.0)

	# for Debian and derivates
	pkg_check_modules(CRYPTOPP libcrypto++>=5.6.0)
	IF(CRYPTOPP_FOUND)
		LIST(APPEND LIBS "-l${CRYPTOPP_LIBRARIES}")
		INCLUDE_DIRECTORIES(${CRYPTOPP_INCLUDE_DIR})
		SET(COMPILE_CRYPTOPP_SUPPORT 1)
		SET(CMAKE_STATUS_CRYPTOPP_SUPPORT "Yes")
	ELSE()
		LIST(APPEND LIBS "-lcryptopp")
		MESSAGE("     WARNING: YOU ENABLED WITH_CRYPTOPP BUT YOUR SYSTEM DOESN'T PROVIDE")
		MESSAGE("              A .pc FILE FOR Crypto++.")
		MESSAGE("              THIS BUILD MIGHT FAIL, MAKE SURE Crypto++ IS INSTALLED AND IT CAN")
		MESSAGE("              BE LINKED AGAINST IT WITH -lcryptopp.")
		SET(COMPILE_CRYPTOPP_SUPPORT 1)
		SET(CMAKE_STATUS_CRYPTOPP_SUPPORT "Yes (library not found)")
	ENDIF()
	ADD_DEFINITIONS("-fexceptions")
ELSE()
	SET(CMAKE_STATUS_CRYPTOPP_SUPPORT "No")
ENDIF()

############################################################################
# OpenSSL support (Secure Sockets, Certificates, Digests etc...)
############################################################################

IF(WANT_OPENSSL)
	FIND_PACKAGE(OpenSSL)
	IF(OPENSSL_FOUND)
		IF(WIN32)
			# On windows/mingw we need libeay32 too...
			FIND_LIBRARY(LIB_EAY NAMES libeay32MD eay libeay libeay32)
			IF(LIB_EAY)
				INCLUDE_DIRECTORIES(${OPENSSL_INCLUDE_DIR})
				# The following are here in order to pick the applink.c file
				# The documentation says that it should be included in OPENSSL_INCLUDE_DIR/openssl
				# but in the package I have now it's actually in OPENSSL_INCLUDE_DIR/../ms .
				INCLUDE_DIRECTORIES(${OPENSSL_INCLUDE_DIR}/openssl)
				INCLUDE_DIRECTORIES(${OPENSSL_INCLUDE_DIR}/../ms)
				LIST(APPEND LIBS ${OPENSSL_LIBRARIES} ${LIB_EAY})
				SET(COMPILE_SSL_SUPPORT 1)
				SET(CMAKE_STATUS_SSL_SUPPORT "Yes")
			ELSE()
				SET(CMAKE_STATUS_SSL_SUPPORT "Not available, libeay missing")
			ENDIF()
		ELSE()
			INCLUDE_DIRECTORIES(${OPENSSL_INCLUDE_DIR})
			LIST(APPEND LIBS ${OPENSSL_LIBRARIES})
			SET(COMPILE_SSL_SUPPORT 1)
			SET(CMAKE_STATUS_SSL_SUPPORT "Yes")
			# CMake FindOpenSSL is incomplete and does not add -lcrypto to LIBS...
			LIST(APPEND LIBS -lcrypto)
		ENDIF()
	ELSE()
		SET(CMAKE_STATUS_SSL_SUPPORT "Not available")
	ENDIF()
ELSE()
	SET(CMAKE_STATUS_SSL_SUPPORT "No")
ENDIF()

############################################################################
# Crypt engines support
############################################################################

OPTION(WANT_CRYPT "Compile Cryptographic Support and Engines" ON)
IF(WANT_CRYPT)
	SET(COMPILE_CRYPT_SUPPORT 1)
	SET(CMAKE_STATUS_CRYPT_SUPPORT "Yes")
ELSE()
	SET(CMAKE_STATUS_CRYPT_SUPPORT "No")
ENDIF()

############################################################################
# IPv6 support
############################################################################

OPTION(WANT_IPV6 "Compile IPv6 Support" ON)
IF(WANT_IPV6)
	SET(COMPILE_IPV6_SUPPORT 1)
	SET(CMAKE_STATUS_IPV6_SUPPORT "Yes")
ELSE()
	SET(CMAKE_STATUS_IPV6_SUPPORT "No")
ENDIF()

############################################################################
# Do we want pseudo-transparency ?
############################################################################

OPTION(WANT_TRANSPARENCY "Compile Transparency Support" ON)
IF(WANT_TRANSPARENCY)
	SET(COMPILE_PSEUDO_TRANSPARENCY 1)
	SET(CMAKE_STATUS_TRANSPARENCY_SUPPORT "Yes")
ELSE()
	SET(CMAKE_STATUS_TRANSPARENCY_SUPPORT "No")
ENDIF()

# Check for Perl support
# (note: needed for user documentation generation)
OPTION(WANT_PERL "Compile Perl Support" ON)

#perl is needed for documentation, too
FIND_PACKAGE(Perl)

IF(WANT_PERL)
	IF(PERL_FOUND)
		SET(COMPILE_PERL_SUPPORT 1)
		SET(CMAKE_STATUS_PERL_SUPPORT "Yes, used for scripting+doc")

		# Get the right flags to link with
		EXECUTE_PROCESS(
			COMMAND ${PERL_EXECUTABLE} -MExtUtils::Embed -e ldopts
			OUTPUT_VARIABLE PERL_LINK_FLAGS
		)

                # Remove new line characters
		STRING(REGEX REPLACE "(\r?\n)+$" "" PERL_LINK_FLAGS "${PERL_LINK_FLAGS}")
		SET(ADDITIONAL_LINK_FLAGS "${ADDITIONAL_LINK_FLAGS} ${PERL_LINK_FLAGS}")
                # Remove leading and trailing spaces (string strip)
		STRING(REGEX REPLACE "(^( )+|( )+$)" "" ADDITIONAL_LINK_FLAGS "${ADDITIONAL_LINK_FLAGS}")
	ELSE()
		SET(CMAKE_STATUS_PERL_SUPPORT "No")
	ENDIF()
ELSE()
	IF(PERL_FOUND)
		SET(CMAKE_STATUS_PERL_SUPPORT "Used for doc, but scripting disabled by user")
	ELSE()
		SET(CMAKE_STATUS_PERL_SUPPORT "No")
	ENDIF()
ENDIF()

# Check for Python support
OPTION(WANT_PYTHON "Compile Python Support" ON)
IF(WANT_PYTHON)
	FIND_PACKAGE(Python)
	IF(PYTHON_FOUND)
		SET(COMPILE_PYTHON_SUPPORT 1)
		SET(CMAKE_STATUS_PYTHON_SUPPORT "Yes")

		#under win32/mingw, if no link flag is needed ${PYTHON_LFLAGS} is set to a braindead "None"
		STRING(REPLACE "None" "" PYTHON_LFLAGS "${PYTHON_LFLAGS}")
		SET(ADDITIONAL_LINK_FLAGS "${ADDITIONAL_LINK_FLAGS} ${PYTHON_LFLAGS}")
                # Remove leading and trailing spaces (string strip)
		STRING(REGEX REPLACE "(^( )+|( )+$)" "" ADDITIONAL_LINK_FLAGS "${ADDITIONAL_LINK_FLAGS}")
		LIST(APPEND LIBS ${PYTHON_LIBRARY})

		INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
	ELSE()
		SET(CMAKE_STATUS_PYTHON_SUPPORT "No, not found.")
	ENDIF()
ELSE()
	SET(CMAKE_STATUS_PYTHON_SUPPORT "No")
ENDIF()

############################################################################
# IPC (inter-process communication) support
############################################################################
OPTION(WANT_IPC "Compile Inter-Process Communication Support" ON)
IF(WANT_IPC)
	SET(CMAKE_STATUS_IPC_SUPPORT "Yes")
ELSE()
	SET(COMPILE_NO_IPC 1)
	SET(CMAKE_STATUS_IPC_SUPPORT "No")
ENDIF()


############################################################################
# NETWORK FUNCTIONS support
############################################################################

INCLUDE(CheckFunctionExists)
CHECK_FUNCTION_EXISTS("inet_ntoa" HAVE_INET_NTOA_EXISTS)
# what systems lacks this!?
IF(HAVE_INET_NTOA_EXISTS)
	SET(HAVE_INET_NTOA 1)
ENDIF()

CHECK_FUNCTION_EXISTS("inet_aton" HAVE_INET_ATON_EXISTS)
# what systems lacks this!?
IF(HAVE_INET_ATON_EXISTS)
	SET(HAVE_INET_ATON 1)
ENDIF()

# Check GET_INTERFACE_ADDRESS support
FIND_PATH(GET_INTERFACE_ADDRESS_INCLUDE_DIR net/if.h)
IF(GET_INTERFACE_ADDRESS_INCLUDE_DIR)
	SET(COMPILE_GET_INTERFACE_ADDRESS 1)
ENDIF()

############################################################################
# SetEnv/PutEnv support
############################################################################
CHECK_FUNCTION_EXISTS("setenv" HAVE_SETENV_EXISTS)
# what systems lacks this!?
IF(HAVE_SETENV_EXISTS)
	SET(HAVE_SETENV 1)
ENDIF()

CHECK_FUNCTION_EXISTS("putenv" HAVE_PUTENV_EXISTS)
# what systems lacks this!?
IF(HAVE_PUTENV_EXISTS)
	SET(HAVE_PUTENV 1)
ENDIF()

############################################################################
# GSM support
############################################################################
OPTION(WANT_GSM "Compile GSM Support" ON)
IF(WANT_GSM)
	SET(COMPILE_USE_GSM 1)
	SET(CMAKE_STATUS_GSM_SUPPORT "Yes")
ELSE()
	SET(CMAKE_STATUS_GSM_SUPPORT "No")
ENDIF()

############################################################################
# DCC canvas support
############################################################################

OPTION(WANT_DCC_CANVAS "Compile DCC Canvas Support" OFF)
IF(WANT_DCC_CANVAS)
	SET(CMAKE_STATUS_DCC_CANVAS_SUPPORT "Yes")
	SET(COMPILE_DCC_CANVAS 1)
ELSE()
	SET(CMAKE_STATUS_DCC_CANVAS_SUPPORT "No")
ENDIF()

############################################################################
# DCC voice support
############################################################################
CHECK_INCLUDE_FILE(linux/soundcard.h CMAKE_HAVE_LINUX_SOUNDCARD_H)
CHECK_INCLUDE_FILE(sys/soundcard.h CMAKE_HAVE_SYS_SOUNDCARD_H)
CHECK_INCLUDE_FILE(soundcard.h CMAKE_HAVE_SOUNDCARD_H)

OPTION(WANT_DCC_VOICE "Compile DCC Voice Support" ON)
IF(WANT_DCC_VOICE)
	IF(CMAKE_HAVE_LINUX_SOUNDCARD_H)
		SET(CMAKE_STATUS_DCC_VOICE_SUPPORT "Yes, found linux/soundcard.h")
		SET(HAVE_LINUX_SOUNDCARD_H 1)
	ELSEIF(CMAKE_HAVE_SYS_SOUNDCARD_H)
		SET(CMAKE_STATUS_DCC_VOICE_SUPPORT "Yes, found sys/soundcard.h")
		SET(HAVE_SYS_SOUNDCARD_H 1)
	ELSEIF(CMAKE_HAVE_SOUNDCARD_H)
		SET(CMAKE_STATUS_DCC_VOICE_SUPPORT "Yes, found soundcard.h")
		SET(HAVE_SOUNDCARD_H 1)
	ELSE()
		SET(COMPILE_DISABLE_DCC_VOICE 1)
		SET(CMAKE_STATUS_DCC_VOICE_SUPPORT "No, soundcard.h not found")
	ENDIF()
ELSE()
	SET(COMPILE_DISABLE_DCC_VOICE 1)
	SET(CMAKE_STATUS_DCC_VOICE_SUPPORT "No")
ENDIF()

############################################################################
# Dcc video support + extended webcam support (LibV4L2)
############################################################################
OPTION(WANT_DCC_VIDEO "Compile DCC Video Support" OFF)

IF(WANT_DCC_VIDEO)
	FIND_PATH(V4L_DEV1_INCLUDE_DIR NAMES linux/videodev.h)
	IF(V4L_DEV1_INCLUDE_DIR)
		SET(COMPILE_V4L1_CODE 1)
		SET(CMAKE_STATUS_DCC_VIDEO_SUPPORT "Yes")
	ELSE()
		FIND_PATH(V4L_DEV2_INCLUDE_DIR NAMES linux/videodev2.h)
		IF(V4L_DEV2_INCLUDE_DIR)
			SET(CMAKE_STATUS_DCC_VIDEO_SUPPORT "Yes")
			SET(COMPILE_V4L1_CODE 0)
		ELSE()
			SET(COMPILE_DISABLE_AVDEVICE 1)
			SET(COMPILE_DISABLE_DCC_VIDEO 1)
			SET(COMPILE_V4L1_CODE 0)
			SET(HAVE_LIBV4L2 0)
			SET(CMAKE_STATUS_DCC_VIDEO_SUPPORT "Not available")
		ENDIF()
	ENDIF()

	IF(NOT COMPILE_DISABLE_AVDEVICE)
		FIND_PACKAGE(LibV4L2)
		IF(LIBV4L2_FOUND)
			SET(CMAKE_STATUS_DCC_VIDEO_SUPPORT "Yes, with LibV4L2")
			SET(HAVE_LIBV4L2 1)
			SET(V4L2_CAP_VIDEO_CAPTURE 1)
			LIST(APPEND LIBS ${LIBV4L2_LIBRARY})
		ENDIF()
	ENDIF()
ELSE()
	SET(COMPILE_DISABLE_AVDEVICE 1)
	SET(COMPILE_DISABLE_DCC_VIDEO 1)
	SET(COMPILE_V4L1_CODE 0)
	SET(HAVE_LIBV4L2 0)
	SET(CMAKE_STATUS_DCC_VIDEO_SUPPORT "No")
ENDIF()

############################################################################
# Ogg/Vorbis and Ogg/Theora support
############################################################################
OPTION(WANT_OGG_THEORA "Compile Ogg/Vorbis+Theora Support" OFF)

IF(WANT_OGG_THEORA)
	FIND_PACKAGE(Theora)
	IF(THEORA_FOUND)
		SET(CMAKE_STATUS_OGG_THEORA_SUPPORT "Yes")
		LIST(APPEND LIBS ${THEORA_LIBRARY})
		INCLUDE_DIRECTORIES(${THEORA_INCLUDE_DIR})
	ELSE()
		SET(COMPILE_DISABLE_OGG_THEORA 1)
		SET(CMAKE_STATUS_OGG_THEORA_SUPPORT "No")
	ENDIF()
ELSE()
	SET(COMPILE_DISABLE_OGG_THEORA 1)
	SET(CMAKE_STATUS_OGG_THEORA_SUPPORT "No")
ENDIF()

############################################################################
# Memory profiling support
############################################################################
OPTION(WANT_MEMORY_PROFILE "Compile Memory Profiling Support" OFF)
IF(WANT_MEMORY_PROFILE)
	SET(COMPILE_MEMORY_PROFILE 1)
	SET(CMAKE_STATUS_MEMORY_PROFILE_SUPPORT "User enabled")
ELSE()
	SET(CMAKE_STATUS_MEMORY_PROFILE_SUPPORT "No")
ENDIF()


############################################################################
# Memory checks support
############################################################################
OPTION(WANT_MEMORY_CHECKS "Compile Memory Checking Support" OFF)
IF(WANT_MEMORY_CHECKS)
	SET(COMPILE_MEMORY_CHECKS 1)
	SET(CMAKE_STATUS_MEMORY_CHECKS_SUPPORT "User enabled")
ELSE()
	SET(CMAKE_STATUS_MEMORY_CHECKS_SUPPORT "No")
ENDIF()

############################################################################
# Win32 mingw/cmake build checks
############################################################################
IF(WIN32)
	ADD_DEFINITIONS(-D_WIN32_WINNT=0x501 -DWINVER=0x501)
ENDIF()

IF(MINGW)
        LIST(APPEND LIBS -lshlwapi)
	IF(WANT_DEBUG)
                SET(ADDITIONAL_LINK_FLAGS "${ADDITIONAL_LINK_FLAGS} -Wl,--enable-auto-import,--enable-runtime-pseudo-reloc -fno-exceptions")
				ADD_DEFINITIONS(-DMINGW)
        ELSE()
                SET(ADDITIONAL_LINK_FLAGS "${ADDITIONAL_LINK_FLAGS} -Wl,--enable-auto-import,--enable-runtime-pseudo-reloc -mwindows -fno-exceptions")
				ADD_DEFINITIONS(-DMINGW -mwindows)
	ENDIF()
        #remove leading and trailing spaces (string strip)
        STRING(REGEX REPLACE "(^( )+|( )+$)" "" ADDITIONAL_LINK_FLAGS "${ADDITIONAL_LINK_FLAGS}")
ENDIF()

IF(MSVC)
	ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ENDIF()

############################################################################
# Very important checks
############################################################################
OPTION(WANT_PIZZA "Pizza for everyone" OFF)
IF(WANT_PIZZA)
	MESSAGE(STATUS "Checking for mozzarella...")
	MESSAGE(STATUS "Checking for mozzarella - found")
	MESSAGE(STATUS "Checking for tomatoes...")
	MESSAGE(STATUS "Checking for tomatoes... - found")
	MESSAGE(STATUS "Starting up the oven...")
	MESSAGE(STATUS "Starting up the oven... - done")
ENDIF()

OPTION(WANT_BEER "Add some PaleALE to the compilation" OFF)
IF(WANT_BEER)
	MESSAGE(STATUS "Checking for beer...")
	MESSAGE(STATUS "Checking for beer... - found")
	MESSAGE(STATUS "Opening the beer...")
	MESSAGE(STATUS "Opening the beer... -  done")
ENDIF()


###############################################################################
#
# Translation (GetText) checks
#
###############################################################################
OPTION(WANT_GETTEXT "Whether to use GetText to generate translations" ON)
IF(WANT_GETTEXT)
	FIND_PROGRAM(GETTEXT_MSGMERGE_EXECUTABLE msgmerge)
	FIND_PROGRAM(GETTEXT_MSGFMT_EXECUTABLE msgfmt)
	FIND_PROGRAM(GETTEXT_XGETTEXT_EXECUTABLE xgettext)
	FIND_PROGRAM(SED_EXECUTABLE sed)

	IF(GETTEXT_MSGFMT_EXECUTABLE)
		ADD_CUSTOM_TARGET("kvirc-translations" ALL)
		SET(CMAKE_STATUS_GETTEXT_SUPPORT "Yes")
		SET(USE_GETTEXT_TRANSLATIONS 1)
	ELSE()
		SET(CMAKE_STATUS_GETTEXT_SUPPORT "No")
		SET(USE_GETTEXT_TRANSLATIONS 0)
	ENDIF()
	IF(GETTEXT_MSGMERGE_EXECUTABLE AND GETTEXT_XGETTEXT_EXECUTABLE)
		# messages-update target;
		ADD_CUSTOM_TARGET(messages-update)
		# messages-extract target;
		ADD_CUSTOM_TARGET(messages-extract)
		ADD_DEPENDENCIES(messages-update messages-extract)
		SET(USE_GETTEXT_EXTRA 1)
		SET(GETTEXT_EXTRA_STATUS ", + extra")
	ELSE()
		SET(USE_GETTEXT_EXTRA 0)
		SET(GETTEXT_EXTRA_STATUS)
	ENDIF()

	IF(SED_EXECUTABLE)
		# wipes absolute paths from .pot? files, by mren
		ADD_CUSTOM_TARGET(messages-tidy)
		FILE(GLOB_RECURSE FILES_TO_TIDY ${CMAKE_SOURCE_DIR}/po/*.po)
		FILE(GLOB_RECURSE POTS_TO_TIDY ${CMAKE_SOURCE_DIR}/po/*.pot)
		LIST(APPEND FILES_TO_TIDY ${POTS_TO_TIDY})
		FOREACH(curFile ${FILES_TO_TIDY})
			ADD_CUSTOM_COMMAND(TARGET messages-tidy
			COMMENT "Tidying ${curFile}..."
			COMMAND ${SED_EXECUTABLE} -i -e "s|^#:.*/\\(src/.*\\)$|#: \\1|g" "${curFile}"
			VERBATIM
			DEPENDS ${curFile}
			)
		ENDFOREACH()
		IF(GETTEXT_MSGMERGE_EXECUTABLE AND GETTEXT_XGETTEXT_EXECUTABLE)
			ADD_DEPENDENCIES(messages-update messages-tidy)
		ENDIF()
		SET(CMAKE_STATUS_MESSAGE_TIDY "Yes")
	ELSE()
		SET(CMAKE_STATUS_MESSAGE_TIDY "Sed not found")
	ENDIF()

ELSE()
	SET(CMAKE_STATUS_GETTEXT_SUPPORT "No")
ENDIF()


#Unused by now
#CMakeUnixFindMake.cmake
#FindCygwin.cmake
#CMakeFindXCode.cmake
#CheckLibraryExists.cmake

# Search for subdirectories
SUBDIRS(src data po scripts doc)

#install additional resources for win32
IF(WIN32)
	ADD_SUBDIRECTORY(win32build)
ENDIF()
###############################################################################
#
# The API documentation target
#
###############################################################################


OPTION(WANT_DOXYGEN "Whether to use Doxygen to generate documentation" ON)
IF(WANT_DOXYGEN)
	FIND_PACKAGE(Doxygen)
	IF(DOXYGEN_FOUND)
		SET(CMAKE_STATUS_DOXYGEN_SUPPORT "Yes")
		ADD_CUSTOM_TARGET(
			devdocs
			@echo Generating api documentation
		)

		ADD_CUSTOM_COMMAND(
			COMMENT "generate the api documentation"
			COMMAND mkdir -p ${CMAKE_BINARY_DIR}/doc/api
			COMMAND cd ${CMAKE_BINARY_DIR}/admin && ${DOXYGEN_EXECUTABLE} Doxyfile
			TARGET devdocs
		)
	ELSE()
		SET(CMAKE_STATUS_DOXYGEN_SUPPORT "No")
	ENDIF()
ELSE()
	SET(CMAKE_STATUS_DOXYGEN_SUPPORT "No")
ENDIF()

###############################################################################
#
# The User documentation target (kvirc internal help)
#
###############################################################################

#currently WIP
#OPTION(WANT_USERDOCS "Whether to generate user documentation (aka internal help)" ON)
#IF(WANT_USERDOCS)
#	SET(CMAKE_STATUS_GEN_USERDOC "Yes")
#ELSE()
#	SET(CMAKE_STATUS_GEN_USERDOC "User disabled")
#ENDIF()

###############################################################################
#
# We want the "distclean" target
#
###############################################################################

IF(UNIX)
	ADD_CUSTOM_TARGET(
		distclean
		@echo Cleaning for source distribution
	)
	SET(
		DISTCLEANED
		cmake.depends
		cmake.check_depends
		CMakeCache.txt
		cmake.check_cache
		progress.make
		install_manifest.txt
		#*.cmake
		Makefile
		core core.*
		gmon.out
		*~
	)
	ADD_CUSTOM_COMMAND(
		TARGET  distclean POST_BUILD
		DEPENDS clean
		COMMENT "distribution clean"
		COMMAND rm
		ARGS    -Rf CMakeTmp CMakeFiles doc/api ${DISTCLEANED}
	)
ENDIF()


###############################################################################
# CONFIGURE FILES
###############################################################################

# System configuration is now splitted in 2 files
# kvi_sysconfig.h contains the "saveable" configuration: stuff that
# may be preserved between multiple cmake runs (and thus may save compilation time)
# kvi_sysbuildinfo.h contains the configuration that changes at every
# cmake run and must be recompiled every time. Extra care is taken
# inside the sources to include kvi_sysbuildinfo.h in only ONE file (so only
# linking of kvilib is done if only kvi_sysbuildinfo.h changes.

IF(EXISTS ${PROJECT_BINARY_DIR}/kvi_sysconfig.h)
	# an old config exists; let's check if it's actual:
	CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/cmake/kvi_sysconfig.h.cmake ${PROJECT_BINARY_DIR}/kvi_sysconfig.h.new)
	FILE(READ ${PROJECT_BINARY_DIR}/kvi_sysconfig.h OLD_CONFIG_CONTENTS)
	FILE(READ ${PROJECT_BINARY_DIR}/kvi_sysconfig.h.new NEW_CONFIG_CONTENTS)
	STRING(COMPARE EQUAL "${OLD_CONFIG_CONTENTS}" "${NEW_CONFIG_CONTENTS}" CONFIG_IS_UPTODATE)
	IF(${CONFIG_IS_UPTODATE})
		# we don't overwrite the kvi_sysconfig.h file to gain compilation time
		FILE(REMOVE ${PROJECT_BINARY_DIR}/kvi_sysconfig.h.new)
	ELSE()
		# let's overwrite the kvi_sysconfig.h file (config has changed)
		# FIXME: this is tricky, better use ${CMAKE_COMMAND} -E
		FILE(REMOVE ${PROJECT_BINARY_DIR}/kvi_sysconfig.h.new)
		CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/cmake/kvi_sysconfig.h.cmake ${PROJECT_BINARY_DIR}/kvi_sysconfig.h)
	ENDIF()
ELSE()
	# no old config
	CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/cmake/kvi_sysconfig.h.cmake ${PROJECT_BINARY_DIR}/kvi_sysconfig.h)
ENDIF()

CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/cmake/kvi_sysbuildinfo.h.cmake ${PROJECT_BINARY_DIR}/kvi_sysbuildinfo.h)


INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})



IF(WANT_COEXISTENCE)
	CONFIGURE_FILE(${kvirc_SOURCE_DIR}/cmake/kvirc-config.cmake ${CMAKE_CURRENT_BINARY_DIR}/scripts/config/kvirc${VERSION_MAJOR}-config @ONLY)
ELSE()
	CONFIGURE_FILE(${kvirc_SOURCE_DIR}/cmake/kvirc-config.cmake ${CMAKE_CURRENT_BINARY_DIR}/scripts/config/kvirc-config @ONLY)
ENDIF()

#CONFIGURE_FILE(${kvirc_SOURCE_DIR}/cmake/FindKVIrc.cmake.in ${kvirc_BINARY_DIR}/FindKVIrc.cmake @ONLY)

IF(WANT_DOXYGEN)
	IF(DOXYGEN_FOUND)
		CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/cmake/Doxyfile.cmake ${CMAKE_BINARY_DIR}/admin/Doxyfile @ONLY)
	ENDIF()
ENDIF()

###############################################################################
# Mark as advanced some variables we really don't want to see in "simple" mode
# All of them *should* be auto-detected.
###############################################################################

MARK_AS_ADVANCED(FORCE QT_QMAKE_EXECUTABLE)
MARK_AS_ADVANCED(FORCE GETTEXT_MSGFMT_EXECUTABLE)
MARK_AS_ADVANCED(FORCE GETTEXT_MSGMERGE_EXECUTABLE)
MARK_AS_ADVANCED(FORCE LIBRARY_OUTPUT_PATH)
MARK_AS_ADVANCED(FORCE CMAKE_BACKWARDS_COMPATIBILITY)
MARK_AS_ADVANCED(FORCE EXECUTABLE_OUTPUT_PATH)

###############################################################################
# ADD LINK FLAGS TO LIBS: THIS HAVE TO BE DONE AT LAST
###############################################################################

LIST(APPEND LIBS "${ADDITIONAL_LINK_FLAGS}")
###############################################################################
# FINAL OUTPUT
###############################################################################

# Binary path
IF(UNIX)
	IF(APPLE)
		SET(KVIRC_BIN_PATH ${CMAKE_INSTALL_PREFIX}/Contents/MacOS/${KVIRC_BINARYNAME})
	ELSE()
		# Assume linux
		SET(KVIRC_BIN_PATH ${CMAKE_INSTALL_PREFIX}/bin/${KVIRC_BINARYNAME})
	ENDIF()
ELSEIF(WIN32)
	SET(KVIRC_BIN_PATH ${CMAKE_INSTALL_PREFIX}/${KVIRC_BINARYNAME})
ENDIF()

# KviLib path
IF(UNIX)
	IF(APPLE)
		SET(KVIRC_LIB_PATH ${CMAKE_INSTALL_PREFIX}/Contents/Frameworks/)
	ELSE()
		# Assume linux
		SET(KVIRC_LIB_PATH ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/)
	ENDIF()
ELSEIF(WIN32)
	SET(KVIRC_LIB_PATH ${CMAKE_INSTALL_PREFIX}/)
ENDIF()

# Modules path
IF(UNIX)
	IF(APPLE)
		SET(KVIRC_MOD_PATH ${CMAKE_INSTALL_PREFIX}/Contents/Frameworks/${KVIRC_BINARYNAME}/${VERSION_BRANCH}/)
	ELSE()
		# Assume linux
		SET(KVIRC_MOD_PATH ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/${KVIRC_BINARYNAME}/${VERSION_BRANCH}/)
	ENDIF()
ELSEIF(WIN32)
	SET(KVIRC_MOD_PATH ${CMAKE_INSTALL_PREFIX}/${KVIRC_BINARYNAME}/${VERSION_BRANCH}/)
ENDIF()

# Man path
IF(UNIX)
	IF(APPLE)
		# does MacOS X use man pages?
	ELSE()
		# Assume linux
		SET(KVIRC_MAN_PATH ${CMAKE_INSTALL_PREFIX}/${MANDIR_PREFIX}/)
	ENDIF()
ELSE()
		SET(KVIRC_MAN_PATH "Not available")
ENDIF()

MESSAGE(STATUS " ")
MESSAGE(STATUS "Ok, this is the configuration I've worked out:")
IF(${CONFIG_IS_UPTODATE})
	MESSAGE(STATUS "(I found an old config that was exactly the same and kept it to save compilation time)")
ENDIF()
MESSAGE(STATUS " ")
MESSAGE(STATUS "Paths:")
MESSAGE(STATUS "   Binary Path                 : ${KVIRC_BIN_PATH}")
MESSAGE(STATUS "   Library Path                : ${KVIRC_LIB_PATH}")
MESSAGE(STATUS "   Modules Path                : ${KVIRC_MOD_PATH}")
MESSAGE(STATUS "   Man Path                    : ${KVIRC_MAN_PATH}")
MESSAGE(STATUS "Compilation:")
MESSAGE(STATUS "   Debug                       : ${CMAKE_STATUS_DEBUG_SUPPORT}")
MESSAGE(STATUS "   Verbose Output              : ${CMAKE_STATUS_VERBOSE_SUPPORT}")
MESSAGE(STATUS "   Coexistence Support         : ${CMAKE_STATUS_COEXISTENCE_VER}")
MESSAGE(STATUS "   Threading Support           : ${CMAKE_STATUS_THREADS_SUPPORT}")
MESSAGE(STATUS "   Apple Universal Binary      : ${CMAKE_STATUS_BUILD_UNIVERSAL_BINARY}")
MESSAGE(STATUS "   Memory Profile Support      : ${CMAKE_STATUS_MEMORY_PROFILE_SUPPORT}")
MESSAGE(STATUS "   Memory Checks Support       : ${CMAKE_STATUS_MEMORY_CHECKS_SUPPORT}")
MESSAGE(STATUS "Features:")
MESSAGE(STATUS "   X11 Support                 : ${CMAKE_STATUS_X11_SUPPORT}")
#MESSAGE(STATUS "   PCH Support                : ${CMAKE_STATUS_PCH_SUPPORT}")
MESSAGE(STATUS "   Qt-DBus Support             : ${CMAKE_STATUS_QTDBUS_SUPPORT}")
MESSAGE(STATUS "   Qt-WebKit Support           : ${CMAKE_STATUS_QTWEBKIT_SUPPORT}")
MESSAGE(STATUS "   Qt-Svg Support           : ${CMAKE_STATUS_QTSVG_SUPPORT}")
MESSAGE(STATUS "   KDE4 Support                : ${CMAKE_STATUS_KDE4_SUPPORT}")
MESSAGE(STATUS "   IPv6 Support                : ${CMAKE_STATUS_IPV6_SUPPORT}")
MESSAGE(STATUS "   Pseudo-Transparency Support : ${CMAKE_STATUS_TRANSPARENCY_SUPPORT}")
MESSAGE(STATUS "   IPC Support                 : ${CMAKE_STATUS_IPC_SUPPORT}")
MESSAGE(STATUS "   Audio Backends              : ${CMAKE_STATUS_AUDIO_BACKENDS}")
MESSAGE(STATUS "Cryptography:")
MESSAGE(STATUS "   Crypt Support And Engines   : ${CMAKE_STATUS_CRYPT_SUPPORT}")
MESSAGE(STATUS "   OpenSSL Support             : ${CMAKE_STATUS_SSL_SUPPORT}")
MESSAGE(STATUS "   Crypto++ Support            : ${CMAKE_STATUS_CRYPTOPP_SUPPORT}")
MESSAGE(STATUS "Scripting:")
MESSAGE(STATUS "   Perl Support                : ${CMAKE_STATUS_PERL_SUPPORT}")
MESSAGE(STATUS "   Python Support              : ${CMAKE_STATUS_PYTHON_SUPPORT}")
MESSAGE(STATUS "DCC features:")
MESSAGE(STATUS "   DCC Canvas Support          : ${CMAKE_STATUS_DCC_CANVAS_SUPPORT}")
MESSAGE(STATUS "   DCC Voice Support           : ${CMAKE_STATUS_DCC_VOICE_SUPPORT}")
MESSAGE(STATUS "   GSM Support                 : ${CMAKE_STATUS_GSM_SUPPORT}")
MESSAGE(STATUS "   DCC Video Support           : ${CMAKE_STATUS_DCC_VIDEO_SUPPORT}")
MESSAGE(STATUS "   Ogg/Theora Support          : ${CMAKE_STATUS_OGG_THEORA_SUPPORT}")
MESSAGE(STATUS "Documentation:")
MESSAGE(STATUS "   GetText Support             : ${CMAKE_STATUS_GETTEXT_SUPPORT}${GETTEXT_EXTRA_STATUS}")
MESSAGE(STATUS "   GetText Messages Tidying    : ${CMAKE_STATUS_MESSAGE_TIDY}")
#MESSAGE(STATUS "   Generate int. help files    : ${CMAKE_STATUS_GEN_USERDOC}")
MESSAGE(STATUS "   Doxygen Support             : ${CMAKE_STATUS_DOXYGEN_SUPPORT}")
MESSAGE(STATUS " ")
MESSAGE(STATUS "Build Date                  : ${CMAKE_KVIRC_BUILD_DATE}")
MESSAGE(STATUS " ")
IF(MSVC)
	MESSAGE(STATUS "I'm ready to build KVIrc for you: please type 'nmake' now...")
ELSEIF(MINGW)
	MESSAGE(STATUS "I'm ready to build KVIrc for you: please type 'mingw32-make' now...")
ELSE()
	MESSAGE(STATUS "I'm ready to build KVIrc for you: please type 'make' now...")
ENDIF()
MESSAGE(STATUS " ")
