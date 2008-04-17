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
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.6)

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
SET(CMAKE_KVIRC_BUILD_CPU ${CMAKE_SYSTEM_PROCESSOR})
SET(CMAKE_KVIRC_BUILD_COMPILER ${CMAKE_CXX_COMPILER})
SET(CMAKE_KVIRC_BUILD_COMPILER_FLAGS ${CMAKE_CXX_FLAGS})

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
		SET(INSTALL_PREFIX /usr/local)
	ENDIF()
ELSEIF(WIN32)
	SET(INSTALL_PREFIX .)
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

OPTION(WANT_PCH "Whether to use pre-compiled headers" ON)
IF(WANT_PCH)
	FIND_PACKAGE(PCHSupport)
	IF(PCHSupport_FOUND)
		SET(CMAKE_STATUS_PCH_SUPPORT "yes")
		LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS -DUSE_PCH=1)
	ELSE()
		SET(CMAKE_STATUS_PCH_SUPPORT "not available")
	ENDIF()
ELSE()
	ADD_DEFINITIONS(-DUSE_PCH=0)
	SET(CMAKE_STATUS_PCH_SUPPORT "user disabled")
ENDIF()

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
		# no need to link to any specific library
		SET(COMPILE_THREADS_USE_WIN32 1)
		SET(CMAKE_STATUS_THREADS_SUPPORT "Win32 native")
		LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS Threads=Win32)
	ELSE()
		IF(CMAKE_HAVE_THREADS_LIBRARY)
			LIST(APPEND LIBS ${CMAKE_THREAD_LIBS_INIT})
		ENDIF()
		SET(COMPILE_THREADS_USE_POSIX 1)
		SET(CMAKE_STATUS_THREADS_SUPPORT "POSIX")
		LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS Threads=POSIX)
		# FIXME: Solaris threads ?
	ENDIF()
ELSE()
	SET(CMAKE_STATUS_THREADS_SUPPORT "Not used")
ENDIF()

# Check for ZLib
FIND_PACKAGE(ZLIB REQUIRED)
IF(ZLIB_FOUND)
	INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})
	LIST(APPEND LIBS ${ZLIB_LIBRARIES})
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
			LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS -DCOMPILE_X11_SUPPORT=1)
		ELSE()
			SET(CMAKE_STATUS_X11_SUPPORT "not available")
		ENDIF()
	ENDIF()
ELSE()
	SET(CMAKE_STATUS_X11_SUPPORT "not needed")
ENDIF()

# Check for Qt4
FIND_PACKAGE(Qt4 REQUIRED)
IF(QT4_FOUND)
	SET(QT_USE_QT3SUPPORT true)
	IF(APPLE)
		SET(QT_USE_QTSQL true)
		SET(QT_USE_QTXML true)
		SET(QT_USE_QTNETWORK true)
	ELSE()
		SET(QT_USE_QTDBUS true)
	ENDIF()
	IF(QT_PHONON_FOUND)
		ADD_DEFINITIONS(-DCOMPILE_PHONON_SUPPORT)
		SET(CMAKE_STATUS_PHONON_SUPPORT "yes")
		LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS -DCOMPILE_PHONON_SUPPORT=1)
		SET(QT_USE_PHONON true)
	ELSE()
		SET(CMAKE_STATUS_PHONON_SUPPORT "not available")
	ENDIF()
	ADD_DEFINITIONS(-DCOMPILE_USE_QT4)
	INCLUDE(${QT_USE_FILE})
	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ${QT_INCLUDES})
	LIST(APPEND LIBS ${QT_LIBRARIES})
ENDIF()

# Check for KDE4
IF(WITH_KDE4)
	FIND_PACKAGE(KDE4)
	IF(KDE4_FOUND)
		INCLUDE_DIRECTORIES(${KDE4_INCLUDES})
		ADD_DEFINITIONS(-DCOMPILE_KDE_SUPPORT)
		LIST(APPEND LIBS ${KDE4_KDEUI_LIBS} ${KDE4_KPARTS_LIBS})
		SET(CMAKE_STATUS_KDE4_SUPPORT "yes")
		LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS -DCOMPILE_KDE_SUPPORT=1)
	ELSE()
		SET(CMAKE_STATUS_KDE4_SUPPORT "not available")
	ENDIF()
ELSE()
	SET(CMAKE_STATUS_KDE4_SUPPORT "not requested")
ENDIF()

# Check for MacOS X universal binary
IF(COMPILE_UNIVERSAL)
	IF(APPLE)
		SET(CMAKE_OSX_ARCHITECTURES ppc;i386)
		SET(CMAKE_STATUS_UNIVERSAL_SUPPORT "yes")
		LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS -DCOMPILE_UNIVERSAL=1)
		#FIXME: This should be handled with CMAKE_OSX_SYSROOT (TOM)
		#SET(CMAKE_SHARED_LINKER_FLAGS "-mmacosx-version-min=10.4")
		#SET(CMAKE_EXE_LINKER_FLAGS "-mmacosx-version-min=10.4")
	ENDIF()
ELSE()
	SET(CMAKE_STATUS_UNIVERSAL_SUPPORT "not available")
ENDIF()

# Check for Perl
# FIXME: This shouldn't be required (we should be able to live without it)
FIND_PACKAGE(Perl REQUIRED)

# Check for OpenSSL
FIND_PACKAGE(OpenSSL)
IF(OPENSSL_FOUND)
	INCLUDE_DIRECTORIES(${OPENSSL_INCLUDE_DIR})
	LIST(APPEND LIBS ${OPENSSL_LIBRARIES})
	ADD_DEFINITIONS(-DCOMPILE_SSL_SUPPORT)
	LIST(APPEND CMAKE_KVIRC_BUILD_FLAGS -DCOMPILE_SSL_SUPPORT=1)
	IF(APPLE)
		LIST(APPEND LIBS -lcrypto)
	ENDIF()
	ADD_DEFINITIONS(-DCOMPILE_CRYPT_SUPPORT)
ENDIF()

# Check for gettext
# FindGettext.cmake - atm implemented in ./cmake/modules/
# Use system-provided module from CMake 2.5
FIND_PACKAGE(GetText)

#Unused by now
#CMakeUnixFindMake.cmake
#FindDoxygen.cmake
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

ADD_CUSTOM_TARGET(
	devdocs
	@echo Generating api documentation
)

ADD_CUSTOM_COMMAND(
	COMMENT "generate the api documentation"
	COMMAND doxygen -u
	COMMAND mkdir -p ${PROJECT_SOURCE_DIR}/doc/api
	COMMAND doxygen ${PROJECT_SOURCE_DIR}/admin/Doxyfile
	TARGET docs
)

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
# FINAL OUTPUT
###############################################################################

MESSAGE(STATUS " ")
MESSAGE(STATUS "Ok, this is the configuration I've worked out:")
IF(${CONFIG_IS_UPTODATE})
	MESSAGE(STATUS "(I found an old config that was exactly the same and kept it to save compilation time)")
ENDIF()
MESSAGE(STATUS " ")
MESSAGE(STATUS "  Threading           : ${CMAKE_STATUS_THREADS_SUPPORT}")
MESSAGE(STATUS "  X11 support         : ${CMAKE_STATUS_X11_SUPPORT}")
MESSAGE(STATUS "  PCH support         : ${CMAKE_STATUS_PCH_SUPPORT}")
MESSAGE(STATUS "  Phonon support      : ${CMAKE_STATUS_PHONON_SUPPORT}")
MESSAGE(STATUS "  KDE4 support        : ${CMAKE_STATUS_KDE4_SUPPORT}")
MESSAGE(STATUS "  Universal support   : ${CMAKE_STATUS_UNIVERSAL_SUPPORT}")
MESSAGE(STATUS " ")
MESSAGE(STATUS "  Build date          : ${CMAKE_KVIRC_BUILD_DATE}")
MESSAGE(STATUS " ")
MESSAGE(STATUS "I'm ready to build kvirc for you: please type 'make' now...")
MESSAGE(STATUS " ")
