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
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

# Check modules first in local dir
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

# Some vars
SET(PACKAGE "kvirc")
SET(NICENAME "KVIrc")
SET(VERSION_MAJOR "4")
SET(VERSION_BRANCH "4.0")
SET(VERSION_RELEASE "4.0.0")

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
FIND_PACKAGE(Subversion)
IF(Subversion_FOUND)
	MESSAGE("-- Found Subversion ${Subversion_VERSION_SVN}")
	Subversion_WC_INFO(${PROJECT_SOURCE_DIR} KVIrc)
 	IF(DEFINED KVIrc_WC_REVISION)
		SET(CMAKE_KVIRC_BUILD_REVISION ${KVIrc_WC_REVISION})
		MESSAGE("-- Current revision is ${KVIrc_WC_REVISION}")
	ELSE()
		MESSAGE("-- No Subversion working copy found")
	ENDIF()
ELSE()
	MESSAGE("Subversion not found. No revision number will be used")
ENDIF()

# Get the build date
TRY_RUN(TEST_DID_RUN TEST_DID_COMPILE "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/datetime.cpp" OUTPUT_VARIABLE TEST_BUILD_DATE)
IF(TEST_DID_COMPILE)
	STRING(REGEX MATCH "[0-9][0-9][0-9][0-9]/[0-9][0-9]/[0-9][0-9] [0-9][0-9]:[0-9][0-9]:[0-9][0-9] [A-Z][A-Z][A-Z]" CMAKE_KVIRC_BUILD_DATE "${TEST_BUILD_DATE}")
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
		IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
			SET(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/KVIrc.app)
		ENDIF()
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

OPTION(WANT_DEBUG "Compile debug symbols" OFF)
IF(DEBUG OR WITH_DEBUG)
	SET(WANT_DEBUG ON)
ENDIF()
IF(WANT_DEBUG)
	SET(CMAKE_BUILD_TYPE Debug)
	SET(CMAKE_STATUS_DEBUG_SUPPORT "Yes")
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS DEBUG)
ELSE()
	SET(CMAKE_BUILD_TYPE Release)
	SET(CMAKE_STATUS_DEBUG_SUPPORT "No")
ENDIF()


###############################################################################
#
# Verbose Makefile
#
###############################################################################

OPTION(WANT_VERBOSE "Generate verbose Makefiles" OFF)
IF(VERBOSE)
	SET(WANT_VERBOSE ON)
ENDIF()
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

OPTION(WANT_COEXISTENCE "Append version to binaries to maintain compatibility with older versions" ON)
IF(COEXISTENCE)
	SET(WANT_COEXISTENCE ON)
ENDIF()
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
	ENDIF()
ELSEIF(WIN32)
	SET(CMAKE_KVIRC_RESOURCES_DIR "${CMAKE_INSTALL_PREFIX}/")
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

# No longer rely on this test for endianness (otherwise cross-compilation might fail)
#INCLUDE(TestBigEndian)
#TEST_BIG_ENDIAN(SYSTEM_IS_BIG_ENDIAN)

###############################################################################
# Option for precompiled headers usage (currently broken because of bad gcc support)
###############################################################################
#
#OPTION(WANT_PCH "Whether to use pre-compiled headers" ON)
#IF(USE_PCH)
#	SET(WANT_PCH ON)
#ENDIF()
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
		SET(CMAKE_STATUS_THREADS_SUPPORT "POSIX")
		LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS Threads=POSIX)
		# FIXME: Solaris threads ?
		#SET(COMPILE_THREADS_USE_SOLARIS_LIBTHREAD 1)
		#SET(CMAKE_STATUS_THREADS_SUPPORT "SOLARIS")
		#LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS Threads=SOLARIS)
	ENDIF()
ELSE()
	SET(CMAKE_STATUS_THREADS_SUPPORT "Not used")
ENDIF()

# Check for ZLib
FIND_PACKAGE(ZLIB REQUIRED)
IF(ZLIB_FOUND)
	INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})
	LIST(APPEND LIBS ${ZLIB_LIBRARIES})
	ADD_DEFINITIONS(-DCOMPILE_ZLIB_SUPPORT)
ENDIF()

############################################################################
# Check for Qt4
############################################################################

FIND_PACKAGE(Qt4 REQUIRED)
IF(QT4_FOUND)
	# We require at least Qt 4.4.x
	IF(QT_VERSION_MINOR LESS 4)
		MESSAGE(FATAL_ERROR "Upgrade your Qt GUI Toolkit to 4.4")
	ENDIF()

        SET(QT_USE_QTSQL true)
	SET(QT_USE_QTXML true)
	SET(QT_USE_QTNETWORK true)

	# Qt-DBus support
	OPTION(WANT_QTDBUS "Whether to compile Qt-DBus support" ON)
	IF(WITHOUT_QTDBUS)
		SET(WANT_QTDBUS OFF)
	ENDIF()
	IF(WANT_QTDBUS)
		IF(QT_QTDBUS_FOUND)
			SET(QT_USE_QTDBUS true)
			SET(CMAKE_STATUS_QTDBUS_SUPPORT "Yes")
			SET(COMPILE_DBUS_SUPPORT 1)
			ADD_DEFINITIONS("-DCOMPILE_DBUS_SUPPORT")
			LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS COMPILE_DBUS_SUPPORT)
		ELSE()
			SET(CMAKE_STATUS_QTDBUS_SUPPORT "Not available")
		ENDIF()
	ELSE()
		SET(CMAKE_STATUS_QTDBUS_SUPPORT "No")
	ENDIF()

	# Qt-WebKit support
	OPTION(WANT_QTWEBKIT "Whether to compile Qt-WebKit support" ON)
	IF(WITHOUT_QTWEBKIT)
		SET(WANT_QTWEBKIT OFF)
	ENDIF()
	IF(WANT_QTWEBKIT)
		IF(QT_QTWEBKIT_FOUND)
			SET(QT_USE_QTWEBKIT true)
			SET(CMAKE_STATUS_QTWEBKIT_SUPPORT "Yes")
			SET(COMPILE_WEBKIT_SUPPORT 1)
			ADD_DEFINITIONS("-DCOMPILE_WEBKIT_SUPPORT")
			LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS COMPILE_WEBKIT_SUPPORT)
		ELSE()
			SET(CMAKE_STATUS_QTWEBKIT_SUPPORT "Not available")
		ENDIF()
	ELSE()
		SET(CMAKE_STATUS_QTWEBKIT_SUPPORT "No")
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
			#STRING(REPLACE ";" " " CMAKESUX_X11_INCLUDE_DIR "${X11_INCLUDE_DIR}")
			#SET(CMAKE_INCLUDEDIRS "${CMAKE_INCLUDEDIRS} ${CMAKESUX_X11_INCLUDE_DIR}")
			#SET(CMAKE_LIBRARIES "${CMAKE_LIBRARIES} X11")
			SET(COMPILE_X11_SUPPORT 1)
			SET(CMAKE_STATUS_X11_SUPPORT "Yes")
			INCLUDE_DIRECTORIES(${X11_INCLUDE_DIR})
			LIST(APPEND LIBS ${X11_LIBRARIES})
		ELSE()
			SET(CMAKE_STATUS_X11_SUPPORT "Not available")
		ENDIF()
	ENDIF()
ELSE()
	SET(CMAKE_STATUS_X11_SUPPORT "Not needed")
ENDIF()

# Qt-Phonon support
OPTION(WANT_PHONON "Whether to compile Phonon support" ON)
IF(WITHOUT_PHONON)
	SET(WANT_PHONON OFF)
ENDIF()
IF(WANT_PHONON)
	FIND_PACKAGE(Phonon)
	IF(PHONON_FOUND)
		SET(COMPILE_PHONON_SUPPORT 1)
		SET(CMAKE_STATUS_PHONON_SUPPORT "Yes")
		LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS COMPILE_PHONON_SUPPORT)
	        LIST(APPEND LIBS ${PHONON_LIBRARY})
	ELSE()
		SET(CMAKE_STATUS_PHONON_SUPPORT "Not available")
	ENDIF()
ELSE()
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITHOUT_PHONON)
	SET(CMAKE_STATUS_PHONON_SUPPORT "No")
ENDIF()

############################################################################
# Check for KDE4
############################################################################

OPTION(WANT_KDE4 "Whether to use KDE4 support" ON)
IF(WITHOUT_KDE4)
	SET(WANT_KDE4 OFF)
ENDIF()
IF(WANT_KDE4)
	FIND_PACKAGE(KDE4)
	IF(KDE4_FOUND)
		INCLUDE_DIRECTORIES(${KDE4_INCLUDES})
		ADD_DEFINITIONS(${KDE4_DEFINITIONS})
		SET(COMPILE_KDE_SUPPORT 1)
		LIST(APPEND LIBS ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBRARY} ${KDE4_KPARTS_LIBRARY} ${KDE4_KFILE_LIBRARY} ${KDE4_KIO_LIBRARY})
		SET(CMAKE_STATUS_KDE4_SUPPORT "Yes")
		LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS COMPILE_KDE_SUPPORT)

		# The KDE4 cmake module has a VERY WEIRD behaviour: it adds -Wl,--no-undefined
		# to the CMAKE_MODULE_LINKER_FLAGS .. which is BAD
		STRING(REPLACE "-Wl,--no-undefined" "" CMAKE_MODULE_LINKER_FLAGS ${CMAKE_MODULE_LINKER_FLAGS})
		# With certain GCC versions it also adds some insane flags around
		STRING(REPLACE "-fvisibility=hidden" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
		STRING(REPLACE "-fvisibility-inlines-hidden" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	ELSE()
		SET(CMAKE_STATUS_KDE4_SUPPORT "Not available")
	ENDIF()
ELSE()
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITHOUT_KDE4)
	SET(CMAKE_STATUS_KDE4_SUPPORT "User disabled")
ENDIF()

############################################################################
# Check for AUDIOFILE
############################################################################

OPTION(WANT_AUDIOFILE "Whether to compile Audiofile support" ON)
IF(WITHOUT_AUDIOFILE)
	SET(WANT_AUDIOFILE OFF)
ENDIF()
IF(WANT_AUDIOFILE)
	FIND_LIBRARY(AUDIOFILE_LIBRARY audiofile)
	IF(NOT AUDIOFILE_LIBRARY MATCHES ".*NOTFOUND")
		FIND_FILE(AUDIOFILE_HEADER audiofile.h)
		IF(NOT AUDIOFILE_HEADER MATCHES ".*NOTFOUND")
			SET(COMPILE_AUDIOFILE_SUPPORT 1)
			LIST(APPEND LIBS ${AUDIOFILE_LIBRARY})
			SET(CMAKE_STATUS_AUDIOFILE_SUPPORT "Yes")
			LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS COMPILE_AUDIOFILE_SUPPORT)
		ELSE()
			SET(CMAKE_STATUS_AUDIOFILE_SUPPORT "Not available (no headers)")
		ENDIF()
	ELSE()
		SET(CMAKE_STATUS_AUDIOFILE_SUPPORT "Not available (no library)")
	ENDIF()
ELSE()
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITHOUT_AUDIOFILE)
	SET(CMAKE_STATUS_AUDIOFILE_SUPPORT "No")
ENDIF()

############################################################################
# Check for MacOS X universal binary
############################################################################

IF(APPLE)
	OPTION(WANT_UNIVERSAL_BINARY "Whether to compile an Universal Binary on MacOSX" OFF)
	IF(COMPILE_MACOSX_UNIVERSAL_BINARY)
		SET(WANT_UNIVERSAL_BINARY ON)
	ENDIF()
	IF(WANT_UNIVERSAL_BINARY)
		SET(CMAKE_OSX_ARCHITECTURES ppc;i386)
		SET(CMAKE_STATUS_BUILD_UNIVERSAL_BINARY "User enabled")
		LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS COMPILE_MACOSX_UNIVERSAL_BINARY)
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
# OpenSSL support (Secure Sockets, Certificates, Digests etc...)
############################################################################

OPTION(WANT_OPENSSL "Whether to use OpenSSL library, if found" ON)
IF(WITHOUT_SSL)
	SET(WANT_OPENSSL OFF)
ENDIF()
IF(WANT_OPENSSL)
	FIND_PACKAGE(OpenSSL)
	IF(OPENSSL_FOUND)
		IF(WIN32)
			# On windows/mingw we need libeay32 too...
			FIND_LIBRARY(LIB_EAY NAMES libeay32MD eay libeay libeay32)
			IF(LIB_EAY)
				INCLUDE_DIRECTORIES(${OPENSSL_INCLUDE_DIR})
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
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITHOUT_OPENSSL)
	SET(CMAKE_STATUS_SSL_SUPPORT "User disabled")
ENDIF()

############################################################################
# Crypt engines support
############################################################################

OPTION(WANT_CRYPT "Compile Cryptographic Support and Engines" ON)
IF(WITHOUT_CRYPT)
	SET(WANT_CRYPT OFF)
ENDIF()
IF(WANT_CRYPT)
	SET(COMPILE_CRYPT_SUPPORT 1)
	SET(CMAKE_STATUS_CRYPT_SUPPORT "Yes")
ELSE()
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITHOUT_CRYPT)
	SET(CMAKE_STATUS_CRYPT_SUPPORT "User disabled")
ENDIF()

############################################################################
# IPv6 support
############################################################################

OPTION(WANT_IPV6 "Compile IPv6 Support" ON)
IF(WITHOUT_IPV6)
	SET(WANT_IPV6 OFF)
ENDIF()
IF(WANT_IPV6)
	SET(COMPILE_IPV6_SUPPORT 1)
	SET(CMAKE_STATUS_IPV6_SUPPORT "Yes")
ELSE()
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITHOUT_IPV6)
	SET(CMAKE_STATUS_IPV6_SUPPORT "User disabled")
ENDIF()

############################################################################
# Do we want pseudo-transparency ?
############################################################################

OPTION(WANT_TRANSPARENCY "Compile Pseudo-Transparency Support" ON)
IF(WITHOUT_TRANSPARENCY)
	SET(WANT_TRANSPARENCY OFF)
ENDIF()
IF(WANT_TRANSPARENCY)
	SET(COMPILE_PSEUDO_TRANSPARENCY 1)
	SET(CMAKE_STATUS_TRANSPARENCY_SUPPORT "Yes")
ELSE()
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITHOUT_TRANSPARENCY)
	SET(CMAKE_STATUS_TRANSPARENCY_SUPPORT "User disabled")
ENDIF()

# Check for Perl support
# (note: needed for user documentation generation)
OPTION(WANT_PERL "Compile Perl Support" ON)
IF(WITHOUT_PERL)
	SET(WANT_PERL OFF)
ENDIF()
IF(WANT_PERL)
	FIND_PACKAGE(Perl)
	IF(PERL_FOUND)
		SET(COMPILE_PERL_SUPPORT 1)
		SET(CMAKE_STATUS_PERL_SUPPORT "Yes")

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
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITHOUT_PERL)
	SET(CMAKE_STATUS_PERL_SUPPORT "User disabled")
ENDIF()

# Check for Python support
OPTION(WANT_PYTHON "Compile Python Support" ON)
IF(WITHOUT_PYTHON)
	SET(WANT_PYTHON OFF)
ENDIF()
IF(WANT_PYTHON)
	FIND_PACKAGE(PythonInterp)
	IF(PYTHONINTERP_FOUND)
		SET(COMPILE_PYTHON_SUPPORT 1)
		SET(CMAKE_STATUS_PYTHON_SUPPORT "Yes")

		# Get the right flags to link with
		EXECUTE_PROCESS(
			COMMAND ${PYTHON_EXECUTABLE} -c "import distutils.sysconfig ; print distutils.sysconfig.get_config_var('LINKFORSHARED')"
			OUTPUT_VARIABLE PYTHON_LINK_FLAGS
		)

		# Remove new line characters
		STRING(REGEX REPLACE "(\r?\n)+$" "" PYTHON_LINK_FLAGS "${PYTHON_LINK_FLAGS}")

		SET(ADDITIONAL_LINK_FLAGS "${ADDITIONAL_LINK_FLAGS} ${PYTHON_LINK_FLAGS}")

		# Remove leading and trailing spaces (string strip)
		STRING(REGEX REPLACE "(^( )+|( )+$)" "" ADDITIONAL_LINK_FLAGS "${ADDITIONAL_LINK_FLAGS}")
	ELSE()
		SET(CMAKE_STATUS_PYTHON_SUPPORT "No")
	ENDIF()
ELSE()
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITHOUT_PYTHON)
	SET(CMAKE_STATUS_PYTHON_SUPPORT "User disabled")
ENDIF()

############################################################################
# IPC (inter-process communication) support
############################################################################
OPTION(WANT_IPC "Compile Inter-Process Communication Support" ON)
IF(WITHOUT_IPC)
	SET(WANT_IPC OFF)
ENDIF()
IF(WANT_IPC)
	SET(CMAKE_STATUS_IPC_SUPPORT "Yes")
ELSE()
	SET(COMPILE_NO_IPC 1)
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITHOUT_IPC)
	SET(CMAKE_STATUS_IPC_SUPPORT "User disabled")
ENDIF()

############################################################################
# System memmove() support
############################################################################
#OPTION(WANT_SYSTEM_MEMMOVE "Compile System memmove() Support" ON)
#IF(WITHOUT_SYSTEM_MEMMOVE)
#	SET(WANT_SYSTEM_MEMMOVE OFF)
#ENDIF()
#IF(WANT_SYSTEM_MEMMOVE)
#	SET(COMPILE_WITH_SYSTEM_MEMMOVE 1)
#	SET(CMAKE_STATUS_SYSTEM_MEMMOVE_SUPPORT "Yes")
#ELSE()
#	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITHOUT_SYSTEM_MEMMOVE)
#	SET(CMAKE_STATUS_SYSTEM_MEMMOVE_SUPPORT "User disabled")
#ENDIF()

############################################################################
# NETWORK FUNCTIONS support
############################################################################

INCLUDE(CheckFunctionExists)
CHECK_FUNCTION_EXISTS("inet_ntoa" HAVE_INET_NTOA)
# what systems lacks this!?
IF(HAVE_INET_NTOA)
	ADD_DEFINITIONS("-DHAVE_INET_NTOA")
ENDIF()

############################################################################
# GSM support
############################################################################
OPTION(WANT_GSM "Compile GSM Support" ON)
IF(WITHOUT_GSM)
	SET(WANT_GSM OFF)
ENDIF()
IF(WANT_GSM)
	SET(COMPILE_USE_GSM 1)
	SET(CMAKE_STATUS_GSM_SUPPORT "Yes")
ELSE()
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITHOUT_GSM)
	SET(CMAKE_STATUS_GSM_SUPPORT "User disabled")
ENDIF()

############################################################################
# DCC voice support
############################################################################
CHECK_INCLUDE_FILE(linux/soundcard.h CMAKE_HAVE_LINUX_SOUNDCARD_H)
CHECK_INCLUDE_FILE(sys/soundcard.h CMAKE_HAVE_SYS_SOUNDCARD_H)
CHECK_INCLUDE_FILE(soundcard.h CMAKE_HAVE_SOUNDCARD_H)

OPTION(WANT_DCC_VOICE "Compile DCC Voice Support" ON)
IF(WITHOUT_DCC_VOICE)
	SET(WANT_DCC_VOICE OFF)
ENDIF()
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
		LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITHOUT_DCC_VOICE)
		SET(CMAKE_STATUS_DCC_VOICE_SUPPORT "No, soundcard.h not found")

	ENDIF()
ELSE()
	SET(COMPILE_DISABLE_DCC_VOICE 1)
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITHOUT_DCC_VOICE)
	SET(CMAKE_STATUS_DCC_VOICE_SUPPORT "User disabled")
ENDIF()

############################################################################
# ix86 asm support
############################################################################
#OPTION(WANT_IX86_ASM "Compile ix86 asm Support OFF)
#IF(WITH_ix86_ASM)
#	SET(WANT_IX86_ASM ON)
#ENDIF()
#IF(WANT_IX86_ASM)
#	SET(COMPILE_ix86_ASM 1)
#	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITH_ix86_ASM)
#	SET(CMAKE_STATUS_ix86_ASM_SUPPORT "User enabled")
#ELSE()
#	SET(CMAKE_STATUS_ix86_ASM_SUPPORT "No")
#ENDIF()

############################################################################
# Memory profiling support
############################################################################
OPTION(WANT_MEMORY_PROFILE "Compile Memory Profiling Support" OFF)
IF(WITH_MEMORY_PROFILE)
	SET(WANT_MEMORY_PROFILE ON)
ENDIF()
IF(WANT_MEMORY_PROFILE)
	SET(COMPILE_MEMORY_PROFILE 1)
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITH_MEMORY_PROFILE)
	SET(CMAKE_STATUS_MEMORY_PROFILE_SUPPORT "User enabled")
ELSE()
	SET(CMAKE_STATUS_MEMORY_PROFILE_SUPPORT "No")
ENDIF()


############################################################################
# Memory checks support
############################################################################
OPTION(WANT_MEMORY_CHECKS "Compile Memory Checking Support" OFF)
IF(WITH_MEMORY_CHECKS)
	SET(WANT_MEMORY_CHECKS ON)
ENDIF()

IF(WANT_MEMORY_CHECKS)
	SET(COMPILE_MEMORY_CHECKS 1)
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITH_MEMORY_CHECKS)
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
        ADD_DEFINITIONS(-DMINGW -DCOMPILE_USE_STANDALONE_MOC_SOURCES -mwindows)
        LIST(APPEND LIBS -lshlwapi)
	IF(WANT_DEBUG)
                SET(ADDITIONAL_LINK_FLAGS "${ADDITIONAL_LINK_FLAGS} -Wl,--enable-auto-import,--enable-runtime-pseudo-reloc")
        ELSE()
                SET(ADDITIONAL_LINK_FLAGS "${ADDITIONAL_LINK_FLAGS} -Wl,--enable-auto-import,--enable-runtime-pseudo-reloc -mwindows")
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
IF(WITH_PIZZA)
	SET(WANT_PIZZA ON)
ENDIF()
IF(WANT_PIZZA)
	MESSAGE(STATUS "Checking for mozzarella...")
	MESSAGE(STATUS "Checking for mozzarella - found")
	MESSAGE(STATUS "Checking for tomatoes...")
	MESSAGE(STATUS "Checking for tomatoes... - found")
	MESSAGE(STATUS "Starting up the oven...")
	MESSAGE(STATUS "Starting up the oven... - done")
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITH_PIZZA)
ENDIF()

OPTION(WANT_BEER "Add some PaleALE to the compilation" OFF)
IF(WITH_BEER)
	SET(WANT_BEER ON)
ENDIF()
IF(WANT_BEER)
	MESSAGE(STATUS "Checking for beer...")
	MESSAGE(STATUS "Checking for beer... - found")
	MESSAGE(STATUS "Opening the beer...")
	MESSAGE(STATUS "Opening the beer... -  done")
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITH_BEER)
ENDIF()


###############################################################################
#
# Translation (GetText) checks
#
###############################################################################
OPTION(WANT_GETTEXT "Whether to use GetText to generate translations" ON)
IF(WITHOUT_GETTEXT)
	SET(WANT_GETTEXT OFF)
ENDIF()
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
	SET(CMAKE_STATUS_GETTEXT_SUPPORT "User disabled")
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITHOUT_GETTEXT)
ENDIF()


#Unused by now
#CMakeUnixFindMake.cmake
#FindCygwin.cmake
#CMakeFindXCode.cmake
#CheckLibraryExists.cmake

# Search for subdirectories
SUBDIRS(src data po scripts doc)

###############################################################################
#
# The API documentation target
#
###############################################################################


OPTION(WANT_DOXYGEN "Whether to use Doxygen to generate documentation" ON)
IF(WITHOUT_DOXYGEN)
	SET(WANT_DOXYGEN OFF)
ENDIF()
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
	SET(CMAKE_STATUS_DOXYGEN_SUPPORT "User disabled")
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITHOUT_DOXYGEN)
ENDIF()

###############################################################################
#
# Package target, `cmake . -DWITH_PACKAGES="<semicolon-separated formats>" && make package` to build packages
# ex. -DWITH_PACKAGES="RPM;DEB"
#
###############################################################################
IF(WITH_PACKAGES)
	INCLUDE (InstallRequiredSystemLibraries)
	SET (CPACK_CMAKE_GENERATOR "Unix Makefiles")
	SET (CPACK_PACKAGE_NAME "kvirc")
	SET (CPACK_GENERATOR ${WITH_PACKAGES})
	SET (CPACK_PACKAGE_DESCRIPTION_SUMMARY "KDE based next generation IRC client with module support")
	SET (CPACK_PACKAGE_VENDOR "KVIrc team")
	SET (CPACK_DEBIAN_PACKAGE_SECTION "net")
	SET (CPACK_DEBIAN_PACKAGE_DESCRIPTION "Extensible crossplatform download manager
	A highly configurable graphical IRC client with an MDI interface,
	built-in scripting language, support for IRC DCC, drag & drop file
	browsing, and much more. KVIrc uses the KDE widget set, can be extended
	using its own scripting language, integrates with KDE, and supports
	custom plugins.")
	SET (CPACK_DEBIAN_PACKAGE_DEPENDS "libqt4-core (>= 4.4.0), libqt4-gui (>= 4.4.0),  libacl1 (>= 2.2.11-1), libart-2.0-2 (>= 2.3.18), libattr1 (>= 2.4.41-1), libaudio2, libaudiofile0 (>= 0.2.3-4), libc6 (>= 2.7-1), libesd0 (>= 0.2.35) | libesd-alsa0 (>= 0.2.35), libfam0, libfontconfig1 (>= 2.4.0), libfreetype6 (>= 2.3.5), libgcc1 (>= 1:4.1.1-21), libice6 (>= 1:1.0.0), libidn11 (>= 0.5.18), libjpeg62, libperl5.10 (>= 5.10.0), libpng12-0 (>= 1.2.13-4), libsm6, libssl0.9.8 (>= 0.9.8f-5), libstdc++6 (>= 4.1.1-21), libx11-6, libxcursor1 (>> 1.1.2), libxext6, libxft2 (>> 2.1.1), libxi6, libxinerama1, libxrandr2 (>= 2:1.2.0), libxrender1, libxt6, zlib1g (>= 1:1.1.4)")
	SET (CPACK_PACKAGE_DESCRIPTION_FILE	"${CMAKE_CURRENT_SOURCE_DIR}/README")
	SET (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
	SET (CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
	SET (CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})

	SET (CPACK_PACKAGE_INSTALL_DIRECTORY "KVIrc")
	SET (CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")
	SET (CPACK_PACKAGE_CONTACT "voker57@gmail.com")
	SET (CPACK_DEBIAN_PACKAGE_MAINTAINER "Voker57")
	INCLUDE (CPack)
ENDIF()
###############################################################################
#
# The User documentation target (kvirc internal help)
#
###############################################################################

#currently WIP
#OPTION(WANT_USERDOCS "Whether to generate user documentation (aka internal help)" ON)
#IF(WITHOUT_USERDOCS)
#	SET(WANT_USERDOCS OFF)
#ENDIF()
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
	ADD_CUSTOM_TARGET(
		uninstall
		"${CMAKE_COMMAND}" -P "${PROJECT_BINARY_DIR}/cmake_uninstall.cmake"
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

CONFIGURE_FILE(
	${PROJECT_SOURCE_DIR}/cmake/UninstallTarget.cmake.in
	${PROJECT_BINARY_DIR}/cmake_uninstall.cmake
	IMMEDIATE @ONLY
)

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
# FINAL OUTPUT
###############################################################################

MESSAGE(STATUS " ")
MESSAGE(STATUS "Ok, this is the configuration I've worked out:")
IF(${CONFIG_IS_UPTODATE})
	MESSAGE(STATUS "(I found an old config that was exactly the same and kept it to save compilation time)")
ENDIF()
MESSAGE(STATUS " ")
MESSAGE(STATUS "  Debug                       : ${CMAKE_STATUS_DEBUG_SUPPORT}")
MESSAGE(STATUS "  Verbose Output              : ${CMAKE_STATUS_VERBOSE_SUPPORT}")
MESSAGE(STATUS "  Coexistence Support         : ${CMAKE_STATUS_COEXISTENCE_VER}")
MESSAGE(STATUS "  Threading Support           : ${CMAKE_STATUS_THREADS_SUPPORT}")
MESSAGE(STATUS "  X11 Support                 : ${CMAKE_STATUS_X11_SUPPORT}")
#MESSAGE(STATUS "  PCH Support                : ${CMAKE_STATUS_PCH_SUPPORT}")
MESSAGE(STATUS "  Qt-DBus Support             : ${CMAKE_STATUS_QTDBUS_SUPPORT}")
MESSAGE(STATUS "  Qt-WebKit Support           : ${CMAKE_STATUS_QTWEBKIT_SUPPORT}")
MESSAGE(STATUS "  Phonon Support              : ${CMAKE_STATUS_PHONON_SUPPORT}")
MESSAGE(STATUS "  KDE4 Support                : ${CMAKE_STATUS_KDE4_SUPPORT}")
MESSAGE(STATUS "  OpenSSL Support             : ${CMAKE_STATUS_SSL_SUPPORT}")
MESSAGE(STATUS "  Crypt Support And Engines   : ${CMAKE_STATUS_CRYPT_SUPPORT}")
MESSAGE(STATUS "  IPv6 Support                : ${CMAKE_STATUS_IPV6_SUPPORT}")
MESSAGE(STATUS "  Pseudo-Transparency Support : ${CMAKE_STATUS_TRANSPARENCY_SUPPORT}")
MESSAGE(STATUS "  Perl Support                : ${CMAKE_STATUS_PERL_SUPPORT}")
MESSAGE(STATUS "  Python Support              : ${CMAKE_STATUS_PYTHON_SUPPORT}")
MESSAGE(STATUS "  IPC Support                 : ${CMAKE_STATUS_IPC_SUPPORT}")
#MESSAGE(STATUS "  System memmove Support      : ${CMAKE_STATUS_SYSTEM_MEMMOVE_SUPPORT}")
MESSAGE(STATUS "  GSM Support                 : ${CMAKE_STATUS_GSM_SUPPORT}")
MESSAGE(STATUS "  DCC Voice Support           : ${CMAKE_STATUS_DCC_VOICE_SUPPORT}")
MESSAGE(STATUS "  Apple Universal Binary      : ${CMAKE_STATUS_BUILD_UNIVERSAL_BINARY}")

#MESSAGE(STATUS "  ix86 asm Support            : ${CMAKE_STATUS_ix86_ASM_SUPPORT}")
MESSAGE(STATUS "  Memory Profile Support      : ${CMAKE_STATUS_MEMORY_PROFILE_SUPPORT}")
MESSAGE(STATUS "  Memory Checks Support       : ${CMAKE_STATUS_MEMORY_CHECKS_SUPPORT}")
MESSAGE(STATUS "  GetText Support             : ${CMAKE_STATUS_GETTEXT_SUPPORT}${GETTEXT_EXTRA_STATUS}")
MESSAGE(STATUS "  GetText Messages Tidying    : ${CMAKE_STATUS_MESSAGE_TIDY}")
#MESSAGE(STATUS "  Generate int. help files    : ${CMAKE_STATUS_GEN_USERDOC}")
MESSAGE(STATUS "  Doxygen Support             : ${CMAKE_STATUS_DOXYGEN_SUPPORT}")
MESSAGE(STATUS "  Audiofile Support           : ${CMAKE_STATUS_AUDIOFILE_SUPPORT}")
MESSAGE(STATUS " ")
MESSAGE(STATUS "  Build Date                  : ${CMAKE_KVIRC_BUILD_DATE}")
MESSAGE(STATUS " ")
MESSAGE(STATUS "I'm ready to build KVIrc for you: please type 'make' now...")
MESSAGE(STATUS " ")
