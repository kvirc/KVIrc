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
SET(
	CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS
	true
)

SET(
	CMAKE_MODULE_PATH
	${kvirc_SOURCE_DIR}/cmake/modules
)

SET(
	CMAKE_VERBOSE_MAKEFILE
	ON CACHE BOOL "Generate verbose Makefiles"
)

# Some vars
SET(PACKAGE "kvirc")
SET(MAJOR "4")
SET(VERSION "4.0.0")
SET(VERSION_BRANCH "4.0")

SET(LIBS)

# Install path
IF(UNIX)
	IF(APPLE)
	# MacOS X
		SET(INSTALL_PREFIX .)
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

INCLUDE(TestBigEndian)
TEST_BIG_ENDIAN(SYSTEM_IS_BIG_ENDIAN)

OPTION(WANT_PCH "Whether to use pre-compiled headers" ON)
IF(WANT_PCH)
	FIND_PACKAGE(PCHSupport)
	IF(PCHSupport_FOUND)
		SET(CMAKE_STATUS_PCH_SUPPORT "yes")
	ELSE()
		SET(CMAKE_STATUS_PCH_SUPPORT "not available")
	ENDIF()
ELSE()
	ADD_DEFINITIONS( -DUSE_PCH=0 )
	SET(CMAKE_STATUS_PCH_SUPPORT "user disabled")
ENDIF()

###############################################################################
#
# Library and tool checks, in order. First the required ones
# in order of dependancy, then the optional ones.
#
###############################################################################

FIND_PACKAGE(Threads REQUIRED)
IF(CMAKE_USE_WIN32_THREADS_INIT OR CMAKE_USE_PTHREADS_INIT)
	IF(CMAKE_USE_WIN32_THREADS_INIT)
		# no need to link to any specific library
		SET(COMPILE_THREADS_USE_WIN32 1)
		SET(CMAKE_STATUS_THREADS_SUPPORT "Win32 native")
	ELSE()
		IF(CMAKE_HAVE_THREADS_LIBRARY)
			LIST(APPEND LIBS ${CMAKE_THREAD_LIBS_INIT})
			#SET(CMAKE_LIBRARIES "${CMAKE_LIBRARIES} pthread")
		ENDIF()
		SET(COMPILE_THREADS_USE_POSIX 1)
		SET(CMAKE_STATUS_THREADS_SUPPORT "POSIX")
		# FIXME: Solaris threads ?
	ENDIF()
ELSE()
	SET(CMAKE_STATUS_THREADS_SUPPORT "Not used")
ENDIF()

# Check for threads
FIND_PACKAGE(Threads REQUIRED)
#IF(Threads_FOUND)
IF(CMAKE_HAVE_THREADS_LIBRARY)
	LIST(APPEND LIBS ${CMAKE_THREAD_LIBS_INIT})
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
	ENDIF()
	ADD_DEFINITIONS(-DCOMPILE_USE_QT4)
	INCLUDE(${QT_USE_FILE})
	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ${QT_INCLUDES})
	LIST(APPEND LIBS ${QT_LIBRARIES})
ENDIF()

FIND_PACKAGE(KDE4)
IF(KDE4_FOUND)
	IF(WITH_KDE4)
		INCLUDE_DIRECTORIES(${KDE4_INCLUDES})
		ADD_DEFINITIONS(-DCOMPILE_KDE_SUPPORT)
		LIST(APPEND LIBS ${KDE4_KDEUI_LIBS} ${KDE4_KPARTS_LIBS})
		SET(CMAKE_STATUS_KDE4_SUPPORT "yes")
	ELSE()
		SET(CMAKE_STATUS_KDE4_SUPPORT "not requested")
	ENDIF()
ELSE()
	SET(CMAKE_STATUS_KDE4_SUPPORT "not available")
ENDIF()

# Check for Perl
# FIXME: This shouldn't be required (we should be able to live without it)
FIND_PACKAGE(Perl REQUIRED)

# Check for OpenSSL
FIND_PACKAGE(OpenSSL)
IF(OPENSSL_FOUND)
	INCLUDE_DIRECTORIES(${OPENSSL_INCLUDE_DIR})
	#SET(LIBS ${LIBS} ${OPENSSL_LIBRARIES})
	LIST(APPEND LIBS ${OPENSSL_LIBRARIES})
	IF(APPLE)
		LIST(APPEND LIBS -lcrypto)
	ENDIF()
	ADD_DEFINITIONS(-DCOMPILE_CRYPT_SUPPORT)
ENDIF()


#Unused by now
#CMakeUnixFindMake.cmake
#FindDoxygen.cmake
#FindCygwin.cmake
#CMakeFindXCode.cmake
#CheckLibraryExists.cmake

# Search for subdirectories
SUBDIRS(src data)

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
	COMMAND mkdir -p ${kvirc_BINARY_DIR}/doc/api
	COMMAND doxygen ${kvirc_BINARY_DIR}/admin/Doxyfile
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
		"${CMAKE_COMMAND}" -P "${kvirc_BINARY_DIR}/cmake_uninstall.cmake"
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

CONFIGURE_FILE(${kvirc_SOURCE_DIR}/cmake/kvi_sysconfig.h.cmake ${kvirc_BINARY_DIR}/kvi_sysconfig.h)
INCLUDE_DIRECTORIES(${kvirc_BINARY_DIR})

#CONFIGURE_FILE(${kvirc_SOURCE_DIR}/cmake/kvirc-config.cmake ${kvirc_BINARY_DIR}/kvirc-config @ONLY)
#CONFIGURE_FILE(${kvirc_SOURCE_DIR}/cmake/FindKVIrc.cmake.in ${kvirc_BINARY_DIR}/FindKVIrc.cmake @ONLY)
#CONFIGURE_FILE(${kvirc_SOURCE_DIR}/cmake/Doxyfile.cmake ${kvirc_BINARY_DIR}/Doxyfile @ONLY)

CONFIGURE_FILE(
	${kvirc_SOURCE_DIR}/cmake/UninstallTarget.cmake.in
	${kvirc_BINARY_DIR}/cmake_uninstall.cmake
	IMMEDIATE @ONLY
)


###############################################################################
# FINAL OUTPUT
###############################################################################

MESSAGE(STATUS " ")
MESSAGE(STATUS "Ok, this is the configuration I've worked out:")
MESSAGE(STATUS " ")
MESSAGE(STATUS "  Threading      : ${CMAKE_STATUS_THREADS_SUPPORT}")
MESSAGE(STATUS "  X11 support    : ${CMAKE_STATUS_X11_SUPPORT}")
MESSAGE(STATUS "  PCH support    : ${CMAKE_STATUS_PCH_SUPPORT}")
MESSAGE(STATUS "  KDE4 support   : ${CMAKE_STATUS_KDE4_SUPPORT}")
MESSAGE(STATUS " ")
MESSAGE(STATUS "I'm ready to build kvirc for you: please type 'make' now...")
MESSAGE(STATUS " ")

