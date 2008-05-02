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

# Minimum cmake version
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.8)

# Set conventional loops
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

# Check modules first in local dir
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)


# Some vars
SET(PACKAGE "kvirc")
SET(VERSION_MAJOR "4")
SET(VERSION_BRANCH "4.0")
SET(VERSION_RELEASE "4.0.0")

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


# Get revision number from SVN
# FIXME: this will break if no connection is available
#FIND_PACKAGE(Subversion)
#IF(Subversion_FOUND)
#	MESSAGE("Found Subversion ${Subversion_VERSION_SVN}")
#	Subversion_WC_INFO(${PROJECT_SOURCE_DIR} KVIrc)
#	SET(CMAKE_KVIRC_BUILD_REVISION ${KVIrc_WC_REVISION})
#	MESSAGE("Current revision is ${KVIrc_WC_REVISION}")
#ELSE()
#	MESSAGE("Subversion not found. No revision number will be used")
#ENDIF()

# Get the build date
TRY_RUN(TEST_DID_RUN TEST_DID_COMPILE "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/datetime.cpp" OUTPUT_VARIABLE TEST_BUILD_DATE)
IF(TEST_DID_COMPILE)
	STRING(REGEX MATCH "[0-9][0-9][0-9][0-9]/[0-9][0-9]/[0-9][0-9] [0-9][0-9]:[0-9][0-9]:[0-9][0-9] [A-Z][A-Z][A-Z]" CMAKE_KVIRC_BUILD_DATE "${TEST_BUILD_DATE}")
ELSE()
	SET(CMAKE_KVIRC_BUILD_DATE "ERROR")
ENDIF()

# Set var to link with, for later use
SET(LIBS)

# Set verbose makefiles
SET(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "Generate verbose Makefiles")

# Install path
IF(UNIX)
	IF(APPLE)
		# MacOS X
		IF(NOT DEFINED INSTALL_PREFIX)
			SET(INSTALL_PREFIX ${CMAKE_BINARY_DIR}/KVirc.app)
		ENDIF()
	ELSE()
		# Linux
		IF(NOT DEFINED INSTALL_PREFIX)
			SET(INSTALL_PREFIX /usr/local)
		ELSE()
			SET(INSTALL_PREFIX ${INSTALL_PREFIX})
			LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS -DINSTALL_PREFIX=${INSTALL_PREFIX})
		ENDIF()
	ENDIF()
ELSEIF(WIN32)
	SET(INSTALL_PREFIX .)
ENDIF()

###############################################################################
#
# Release type
#
###############################################################################

OPTION(WANT_DEBUG "Compile debug symbols" OFF)
IF(DEBUG)
	SET(WANT_DEBUG ON)
ENDIF()
IF(WANT_DEBUG)
	SET(CMAKE_BUILD_TYPE Debug)
	SET(CMAKE_STATUS_DEBUG_SUPPORT "yes")
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS DEBUG)
ELSE()
	SET(CMAKE_BUILD_TYPE Release)
	SET(CMAKE_STATUS_DEBUG_SUPPORT "user disabled")
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
#		SET(CMAKE_STATUS_PCH_SUPPORT "yes")
#		LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS -DUSE_PCH=1)
#	ELSE()
#		SET(CMAKE_STATUS_PCH_SUPPORT "not available")
#	ENDIF()
#ELSE()
#	ADD_DEFINITIONS(-DUSE_PCH=0)
#	SET(CMAKE_STATUS_PCH_SUPPORT "user disabled")
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

# Check for X11
IF(UNIX)
	IF(APPLE)
		SET(CMAKE_STATUS_X11_SUPPORT "not used")
	ELSE()
		# Assume linux
		FIND_PACKAGE(X11)
		IF(X11_FOUND)
			#STRING(REPLACE ";" " " CMAKESUX_X11_INCLUDE_DIR "${X11_INCLUDE_DIR}")
			#SET(CMAKE_INCLUDEDIRS "${CMAKE_INCLUDEDIRS} ${CMAKESUX_X11_INCLUDE_DIR}")
			#SET(CMAKE_LIBRARIES "${CMAKE_LIBRARIES} X11")
			SET(COMPILE_X11_SUPPORT 1)
			SET(CMAKE_STATUS_X11_SUPPORT "yes")
			INCLUDE_DIRECTORIES(${X11_INCLUDE_DIR})
			LIST(APPEND LIBS ${X11_LIBRARIES})
		ELSE()
			SET(CMAKE_STATUS_X11_SUPPORT "not available")
		ENDIF()
	ENDIF()
ELSE()
	SET(CMAKE_STATUS_X11_SUPPORT "not needed")
ENDIF()

############################################################################
# Check for Qt4
############################################################################

FIND_PACKAGE(Qt4 REQUIRED)
IF(QT4_FOUND)
	SET(QT_USE_QT3SUPPORT true)
	IF(NOT WIN32)
		IF(APPLE)
			SET(QT_USE_QTSQL true)
			SET(QT_USE_QTXML true)
			SET(QT_USE_QTNETWORK true)
		ELSE()
			# Assume linux
			SET(QT_USE_QTDBUS true)
		ENDIF()
	ENDIF()

	# Phonon support
	IF(QT_PHONON_FOUND AND QT_PHONON_INCLUDE_DIR)
		SET(COMPILE_PHONON_SUPPORT 1)
		SET(CMAKE_STATUS_PHONON_SUPPORT "qt")
		LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS COMPILE_PHONON_SUPPORT)
		SET(QT_USE_PHONON true)
	ELSE()
		SET(CMAKE_STATUS_PHONON_SUPPORT "not available")
	ENDIF()

	SET(COMPILE_USE_QT4 1)
	INCLUDE(${QT_USE_FILE})
	IF(QT_USE_QTDBUS)
		SET(CMAKE_STATUS_QTDBUS "yes")
	ELSE()
		SET(CMAKE_STATUS_QTDBUS "Not used")
	ENDIF()
	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ${QT_INCLUDES})
	LIST(APPEND LIBS ${QT_LIBRARIES})
ENDIF()

############################################################################
# Check for KDE4
############################################################################

OPTION(WANT_KDE4 "Whether to use KDE4 support (experimental)" OFF)
IF(WITH_KDE4)
	SET(WANT_KDE4 ON)
ENDIF()
IF(WANT_KDE4)
	FIND_PACKAGE(KDE4)
	IF(KDE4_FOUND)
		INCLUDE_DIRECTORIES(${KDE4_INCLUDES})
		ADD_DEFINITIONS(${KDE4_DEFINITIONS})
		SET(COMPILE_KDE_SUPPORT 1)
		LIST(APPEND LIBS ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBRARY} ${KDE4_KPARTS_LIBRARY} ${KDE4_KFILE_LIBRARY} ${KDE4_KIO_LIBRARY})
		SET(CMAKE_STATUS_KDE4_SUPPORT "yes")
		LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS COMPILE_KDE_SUPPORT)
		SET(CMAKE_STATUS_PHONON_SUPPORT "kde")
		
		# The KDE4 cmake module has a VERY WEIRD behaviour: it adds -Wl,--no-undefined
		# to the CMAKE_MODULE_LINKER_FLAGS .. which is BAD
		STRING(REPLACE "-Wl,--no-undefined" "" CMAKE_MODULE_LINKER_FLAGS ${CMAKE_MODULE_LINKER_FLAGS})
	ELSE()
		SET(CMAKE_STATUS_KDE4_SUPPORT "not available")
	ENDIF()
ELSE()
	SET(CMAKE_STATUS_KDE4_SUPPORT "user disabled")
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
		SET(CMAKE_STATUS_BUILD_UNIVERSAL_BINARY "user enabled")
		LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS COMPILE_MACOSX_UNIVERSAL_BINARY)
		#FIXME: This should be handled with CMAKE_OSX_SYSROOT (TOM)
		#SET(CMAKE_SHARED_LINKER_FLAGS "-mmacosx-version-min=10.4")
		#SET(CMAKE_EXE_LINKER_FLAGS "-mmacosx-version-min=10.4")
	ELSE()
		SET(CMAKE_STATUS_BUILD_UNIVERSAL_BINARY "no")
	ENDIF()
ELSE()
	SET(CMAKE_STATUS_BUILD_UNIVERSAL_BINARY "not applicable")
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
		INCLUDE_DIRECTORIES(${OPENSSL_INCLUDE_DIR})
		LIST(APPEND LIBS ${OPENSSL_LIBRARIES})
		SET(COMPILE_SSL_SUPPORT 1)
		SET(CMAKE_STATUS_SSL_SUPPORT "yes")
		# CMake FindOpenSSL is incomplete and does not add -lcrypto to LIBS...
		LIST(APPEND LIBS -lcrypto)
	ELSE()
		SET(CMAKE_STATUS_SSL_SUPPORT "not available")
	ENDIF()
ELSE()
	SET(CMAKE_STATUS_SSL_SUPPORT "user disabled")
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
	SET(CMAKE_STATUS_CRYPT_SUPPORT "yes")
ELSE()
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITHOUT_CRYPT)
	SET(CMAKE_STATUS_CRYPT_SUPPORT "user disabled")
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
	SET(CMAKE_STATUS_IPV6_SUPPORT "yes")
ELSE()
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITHOUT_IPV6)
	SET(CMAKE_STATUS_IPV6_SUPPORT "user disabled")
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
	SET(CMAKE_STATUS_TRANSPARENCY_SUPPORT "yes")
ELSE()
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITHOUT_TRANSPARENCY)
	SET(CMAKE_STATUS_TRANSPARENCY_SUPPORT "user disabled")
ENDIF()



# Check for Perl support
# FIXME: This shouldn't be required (we should be able to live without it) (note: needed for user documentation generation)
FIND_PACKAGE(Perl REQUIRED)
OPTION(WANT_PERL "Compile Perl Support" ON)
IF(WITHOUT_PERL)
	SET(WANT_PERL OFF)
ENDIF()
IF(WANT_PERL)
	SET(COMPILE_PERL_SUPPORT 1)
	SET(CMAKE_STATUS_PERL_SUPPORT "yes")
ELSE()
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITHOUT_PERL)
	SET(CMAKE_STATUS_PERL_SUPPORT "user disabled")
ENDIF()

############################################################################
# IPC (inter-process communication) support
############################################################################
OPTION(WANT_IPC "Compile Inter-Process Communication Support" ON)
IF(WITHOUT_IPC)
	SET(WANT_IPC OFF)
ENDIF()
IF(WANT_IPC)
	SET(CMAKE_STATUS_IPC_SUPPORT "yes")
ELSE()
	SET(COMPILE_NO_IPC 1)
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITHOUT_IPC)
	SET(CMAKE_STATUS_IPC_SUPPORT "user disabled")
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
#	SET(CMAKE_STATUS_SYSTEM_MEMMOVE_SUPPORT "yes")
#ELSE()
#	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITHOUT_SYSTEM_MEMMOVE)
#	SET(CMAKE_STATUS_SYSTEM_MEMMOVE_SUPPORT "user disabled")
#ENDIF()

############################################################################
# GSM support
############################################################################
OPTION(WANT_GSM "Compile GSM Support" ON)
IF(WITHOUT_GSM)
	SET(WANT_GSM OFF)
ENDIF()
IF(WANT_GSM)
	SET(COMPILE_USE_GSM 1)
	SET(CMAKE_STATUS_GSM_SUPPORT "yes")
ELSE()
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITHOUT_GSM)
	SET(CMAKE_STATUS_GSM_SUPPORT "user disabled")
ENDIF()

############################################################################
# DCC voice support
############################################################################
OPTION(WANT_DCC_VOICE "Compile DCC Voice Support" ON)
IF(WITHOUT_DCC_VOICE)
	SET(WANT_DCC_VOICE OFF)
ENDIF()
IF(WANT_DCC_VOICE)
	SET(COMPILE_DISABLE_DCC_VOICE 1)
	SET(CMAKE_STATUS_DCC_VOICE_SUPPORT "yes")
ELSE()
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITHOUT_DCC_VOICE)
	SET(CMAKE_STATUS_DCC_VOICE_SUPPORT "user disabled")
ENDIF()

############################################################################
# Dynamic labels support
############################################################################
OPTION(WANT_DYN_LABELS "Compile Dynamic Labels Support" ON)
IF(WITHOUT_DYN_LABELS)
	SET(WANT_DYN_LABELS OFF)
ENDIF()
IF(WANT_DYN_LABELS)
	SET(COMPILE_USE_DYNAMIC_LABELS 1)
	SET(CMAKE_STATUS_DYN_LABELS_SUPPORT "yes")
ELSE()
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITHOUT_DYN_LABELS)
	SET(CMAKE_STATUS_DYN_LABELS_SUPPORT "user disabled")
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
#	SET(CMAKE_STATUS_ix86_ASM_SUPPORT "user enabled")
#ELSE()
#	SET(CMAKE_STATUS_ix86_ASM_SUPPORT "no")
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
	SET(CMAKE_STATUS_MEMORY_PROFILE_SUPPORT "user enabled")
ELSE()
	SET(CMAKE_STATUS_MEMORY_PROFILE_SUPPORT "no")
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
	SET(CMAKE_STATUS_MEMORY_CHECKS_SUPPORT "user enabled")
ELSE()
	SET(CMAKE_STATUS_MEMORY_CHECKS_SUPPORT "no")
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
	MESSAGE(STATUS "Checking for ice...")
	MESSAGE(STATUS "Checking for ice... - found")
	MESSAGE(STATUS "Opening the beer...")
	MESSAGE(STATUS "Opening the beer... -  done")
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITH_BEER)
ENDIF()

# Check for gettext
# FindGettext.cmake - atm implemented in ./cmake/modules/
# Use system-provided module from CMake 2.5
FIND_PACKAGE(GetText)

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
		SET(CMAKE_STATUS_DOXYGEN_SUPPORT "yes")
		ADD_CUSTOM_TARGET(
			devdocs
			@echo Generating api documentation
		)

		ADD_CUSTOM_COMMAND(
			COMMENT "generate the api documentation"
			COMMAND ${DOXYGEN_EXECUTABLE} -u
			COMMAND mkdir -p ${PROJECT_SOURCE_DIR}/doc/api
			COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_SOURCE_DIR}/admin/Doxyfile
			TARGET devdocs
		)
	ELSE()
		SET(CMAKE_STATUS_DOXYGEN_SUPPORT "no")
		LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS WITHOUT_DOXYGEN)
	ENDIF()
ELSE()
	SET(CMAKE_STATUS_DOXYGEN_SUPPORT "user disabled")
ENDIF()


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

CONFIGURE_FILE(${kvirc_SOURCE_DIR}/cmake/kvirc-config.cmake ${CMAKE_CURRENT_BINARY_DIR}/scripts/config/kvirc${VERSION_MAJOR}-config @ONLY)
#CONFIGURE_FILE(${kvirc_SOURCE_DIR}/cmake/FindKVIrc.cmake.in ${kvirc_BINARY_DIR}/FindKVIrc.cmake @ONLY)
#CONFIGURE_FILE(${kvirc_SOURCE_DIR}/cmake/Doxyfile.cmake ${kvirc_BINARY_DIR}/Doxyfile @ONLY)

CONFIGURE_FILE(
	${PROJECT_SOURCE_DIR}/cmake/UninstallTarget.cmake.in
	${PROJECT_BINARY_DIR}/cmake_uninstall.cmake
	IMMEDIATE @ONLY
)

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
MESSAGE(STATUS "  Threading                   : ${CMAKE_STATUS_THREADS_SUPPORT}")
MESSAGE(STATUS "  X11 support                 : ${CMAKE_STATUS_X11_SUPPORT}")
#MESSAGE(STATUS "  PCH support                : ${CMAKE_STATUS_PCH_SUPPORT}")
MESSAGE(STATUS "  Phonon support              : ${CMAKE_STATUS_PHONON_SUPPORT}")
MESSAGE(STATUS "  KDE4 support                : ${CMAKE_STATUS_KDE4_SUPPORT}")
MESSAGE(STATUS "  OpenSSL Support             : ${CMAKE_STATUS_SSL_SUPPORT}")
MESSAGE(STATUS "  Crypt Support and Engines   : ${CMAKE_STATUS_CRYPT_SUPPORT}")
MESSAGE(STATUS "  IPv6 support                : ${CMAKE_STATUS_IPV6_SUPPORT}")
MESSAGE(STATUS "  Pseudo-Transparency Support : ${CMAKE_STATUS_TRANSPARENCY_SUPPORT}")
MESSAGE(STATUS "  Perl support                : ${CMAKE_STATUS_PERL_SUPPORT}")
MESSAGE(STATUS "  IPC support                 : ${CMAKE_STATUS_IPC_SUPPORT}")
#MESSAGE(STATUS "  System memmove support      : ${CMAKE_STATUS_SYSTEM_MEMMOVE_SUPPORT}")
MESSAGE(STATUS "  GSM support                 : ${CMAKE_STATUS_GSM_SUPPORT}")
MESSAGE(STATUS "  DCC voice support           : ${CMAKE_STATUS_DCC_VOICE_SUPPORT}")
MESSAGE(STATUS "  Dynamic labels support      : ${CMAKE_STATUS_DYN_LABELS_SUPPORT}")

MESSAGE(STATUS "  Build Universal Binary      : ${CMAKE_STATUS_BUILD_UNIVERSAL_BINARY}")

#MESSAGE(STATUS "  ix86 asm support            : ${CMAKE_STATUS_ix86_ASM_SUPPORT}")
MESSAGE(STATUS "  Memory profile support      : ${CMAKE_STATUS_MEMORY_PROFILE_SUPPORT}")
MESSAGE(STATUS "  Memory checks support       : ${CMAKE_STATUS_MEMORY_CHECKS_SUPPORT}")
MESSAGE(STATUS "  Doxygen support             : ${CMAKE_STATUS_DOXYGEN_SUPPORT}")
MESSAGE(STATUS "  QtDBus                      : ${CMAKE_STATUS_QTDBUS}")
MESSAGE(STATUS " ")
MESSAGE(STATUS "  Build date                  : ${CMAKE_KVIRC_BUILD_DATE}")
MESSAGE(STATUS " ")
MESSAGE(STATUS "I'm ready to build kvirc for you: please type 'make' now...")
MESSAGE(STATUS " ")
