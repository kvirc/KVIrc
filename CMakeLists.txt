###############################################################################
## KVIrc CMake global configuration file.
###############################################################################
##
## Conventions for variables:
##
##    WANT_*           : Switches that enable/disable features
##                       Example: WANT_KDE (KDE usage)
##
##    CMAKE_STATUS_*   : Text variables used to display the final configuration
##                       at the end of the CMake script run. Their format is
##                       not fixed therefore they should be not relied upon
##                       inside this script (i.e. don't check their value).
##                       Example: CMAKE_STATUS_X11_SUPPORT (can be "Yes", "No",
##                       "Yes, with scrnsaveproto" or whatever "Blah").
##
##    CMAKE_RESULT_*   : Contain a result of a CMake script computation
##                       in a fixed format (of any type). Used only inside
##                       the CMake script.
##                       Example: CMAKE_RESULT_USING_KDE either true or false
##
##    COMPILE_*        : Variables that end up in kvi_sysconfig.h and are
##                       visible in code (usually #defined to 1 or undefined)
##                       Example: COMPILE_IPV6_SUPPORT (set to 1 or unset)
##
##    CMAKE_KVIRC_*    : Variables that end up in kvi_sysconfig.h or
##                       kvi_sysbuildinfo.h as KVIRC_* and are usually textual
##                       Example: CMAKE_KVIRC_MODULES_DIR (a textual path)
##
###############################################################################

###############################################################################
# GLOBAL DEFS
###############################################################################

# Minimum CMake version
cmake_minimum_required(VERSION 3.8)

# Name of the project
project(kvirc)

# Check modules first in local dir
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

# Some vars
set(PACKAGE "kvirc")
set(NICENAME "KVIrc")
set(VERSION_MAJOR "5")
set(VERSION_BRANCH "5.0")
set(VERSION_RELEASE "5.0.0")
set(VERSION_CODENAME "Aria")
# REMINDER: "Quasar" is a new candidate name.

string(REGEX MATCH "[0-9]+$" VERSION_MINOR ${VERSION_BRANCH})
string(REGEX MATCH "[0-9]+$" VERSION_MICRO ${VERSION_RELEASE})

set(CMAKE_KVIRC_VERSION_MAJOR ${VERSION_MAJOR})
set(CMAKE_KVIRC_VERSION_MINOR ${VERSION_MINOR})
set(CMAKE_KVIRC_VERSION_MICRO ${VERSION_MICRO})
set(CMAKE_KVIRC_VERSION_BRANCH ${VERSION_BRANCH})
set(CMAKE_KVIRC_VERSION_RELEASE ${VERSION_RELEASE})
set(CMAKE_KVIRC_VERSION_CODENAME ${VERSION_CODENAME})
set(CMAKE_KVIRC_BUILD_COMMAND ${CMAKE_COMMAND})
set(CMAKE_KVIRC_BUILD_FLAGS)
set(CMAKE_KVIRC_BUILD_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})
set(CMAKE_KVIRC_BUILD_CPU ${CMAKE_SYSTEM_PROCESSOR})
set(CMAKE_KVIRC_BUILD_COMPILER ${CMAKE_CXX_COMPILER})
set(CMAKE_KVIRC_BUILD_COMPILER_FLAGS ${CMAKE_CXX_FLAGS})

# Prefer Python 2.7 over 3.x (which is currently incompatible) - GitHub issue #2020
set(Python_ADDITIONAL_VERSIONS "2.7")

# Suffix for GNU/Linux
set(LIB_SUFFIX
	CACHE STRING "Define suffix of directory name (32/64)"
)

# Get revision number from Git
set(MANUAL_REVISION
	CACHE STRING "Manually set the revision number"
)

string(COMPARE EQUAL "${MANUAL_REVISION}" "" MANUAL_REVISION_UNSET)
if(MANUAL_REVISION_UNSET)
	# No manually forced revision number, gotta guess one now
	if(EXISTS "${CMAKE_SOURCE_DIR}/.gitrevision")
		# We're in a release package created using makedist.sh, so we should include a .gitrevision file
		file(READ "${CMAKE_SOURCE_DIR}/.gitrevision" DISTGITREVISION)
		string(STRIP "${DISTGITREVISION}" CMAKE_KVIRC_BUILD_REVISION)
		message(STATUS "Current revision is ${CMAKE_KVIRC_BUILD_REVISION} (from .gitrevision)")
	elseif(EXISTS "${CMAKE_SOURCE_DIR}/.git")
		find_package(Git)
		if(GIT_FOUND)
			message(STATUS "Found Git ${GIT_VERSION_STRING}")
			execute_process(
				COMMAND ${GIT_EXECUTABLE} describe "--dirty"
				WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
				RESULT_VARIABLE res_var
				OUTPUT_VARIABLE GIT_COMMIT_ID
			)
			execute_process(
				COMMAND ${GIT_EXECUTABLE} show -s --format=%cd --date=short
				WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
				RESULT_VARIABLE res_date
				OUTPUT_VARIABLE GIT_COMMIT_DATE
			)
			if(${res_var} EQUAL 0)
				string(REPLACE "\n" "" GIT_COMMIT_ID "${GIT_COMMIT_ID}")
				set(CMAKE_KVIRC_BUILD_REVISION ${GIT_COMMIT_ID})
				message(STATUS "Current revision is ${CMAKE_KVIRC_BUILD_REVISION}")
			else()
				message( STATUS "Git failed (not a repo, or no tags). Build will not contain Git revision info." )
			endif()
			if(${res_date} EQUAL 0)
				string(REPLACE "\n" "" GIT_COMMIT_DATE "${GIT_COMMIT_DATE}")
				string(REPLACE "-" "" GIT_COMMIT_DATE "${GIT_COMMIT_DATE}")
				set(KVIRC_SOURCES_GOT_DATE_FROM_GIT 1)
				set(CMAKE_KVIRC_SOURCES_DATE ${GIT_COMMIT_DATE})
				set(CMAKE_KVIRC_SOURCES_DATE_HEX 0x${GIT_COMMIT_DATE})
				message(STATUS "Using sources date from ${CMAKE_KVIRC_SOURCES_DATE}")
			else()
				message(STATUS "Git failed (not a repo, or no tags). Build will not contain Git date info.")
			endif()
		else()
			message( STATUS "Git not found. Build will not contain Git revision info." )
		endif()
	else()
		message( STATUS "Missing .git directory. Build will not contain Git revision info." )
	endif()
else()
	# Manually forced revision number
	set(CMAKE_KVIRC_BUILD_REVISION ${MANUAL_REVISION})
	set(CMAKE_KVIRC_SOURCES_DATE ${MANUAL_SOURCES_DATE})
	set(CMAKE_KVIRC_SOURCES_DATE_HEX 0x${MANUAL_SOURCES_DATE})
	message(STATUS "Current revision has been manually set to ${MANUAL_REVISION}")
endif()

# Set man dir
set(MANDIR
	CACHE STRING "Define man directory starting from $CMAKE_INSTALL_PREFIX"
)

string(COMPARE EQUAL "${MANDIR}" "" MANDIR_UNSET)
if(MANDIR_UNSET)
	set(MANDIR_PREFIX "share/man")
else()
	set(MANDIR_PREFIX "man")
endif()
list(APPEND CMAKE_KVIRC_BUILD_FLAGS MANDIR=${MANDIR_PREFIX})

# Get the build date
string(TIMESTAMP CMAKE_KVIRC_BUILD_DATE "%Y-%m-%d %H:%M:%S UTC" UTC)

# Set var to link with, for later use
set(LIBS)
set(ADDITIONAL_LINK_FLAGS)

# Install path
if(UNIX)
	if(APPLE)
		# macOS
		# Due to the special bundle structure ignore
		# the prefix eventually set by the user.
		set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/KVIrc.app)
	else()
		# Linux / BSD
		if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
			#fix package build
			if(PREFIX)
				set(CMAKE_INSTALL_PREFIX ${PREFIX})
			else()
				set(CMAKE_INSTALL_PREFIX /usr/local)
			endif()
		endif()
	endif()
elseif(WIN32)
	set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/release)
endif()
list(APPEND CMAKE_KVIRC_BUILD_FLAGS CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX})

###############################################################################
# Release type
###############################################################################

option(WANT_ENV_FLAGS "Don't try to set CMAKE_C(XX)_FLAGS; instead, use the ones from environment variables" OFF)

option(WANT_DEBUG "Compile debug symbols" OFF)
if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo OR DEBUG OR WITH_DEBUG)
	set(WANT_DEBUG ON CACHE BOOL "Compile debug symbols" FORCE)
	set(COMPILE_DEBUG_MODE 1)
endif()

option(WANT_STRIP "Strip binaries (discard symbols from object files)" OFF)

if(WANT_ENV_FLAGS)
	set(CMAKE_BUILD_TYPE Undefined)
	set(CMAKE_STATUS_DEBUG_SUPPORT "Environment-driven")
else()
	if(MSVC)
		# Qt5 has /Zc:wchar_t
		set(CMAKE_CXX_FLAGS "/MP /Zc:wchar_t /EHsc /source-charset:utf-8")
		set(CMAKE_C_FLAGS "/MP /Zc:wchar_t /EHsc /source-charset:utf-8")
	endif()
	if(WANT_DEBUG)
		set(WANT_STRIP OFF CACHE BOOL "Strip binaries (discard symbols from object files)" FORCE)
		set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
		set(CMAKE_STATUS_DEBUG_SUPPORT "Yes")
		if(CMAKE_COMPILER_IS_GNUCXX)
			# force gdb format of debug symbols - the release-specific
			# variable is used, as CMake concatenates CMAKE_CXX_FLAGS with
			# CMAKE_CXX_FLAGS_DEBUG before finalizing. This results in the
			# default DEBUG flag of '-g' being appended, and therefore
			# overriding what you specify here
			set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb -pipe --exec-charset=UTF-8 --input-charset=UTF-8 --no-implement-inlines --unit-at-a-time --fast-math")
			set(CMAKE_C_FLAGS_DEBUG "-O0 -ggdb -pipe --exec-charset=UTF-8 --input-charset=UTF-8 --no-implement-inlines --unit-at-a-time --fast-math")
			include(CheckCXXCompilerFlag)
			CHECK_CXX_COMPILER_FLAG("-fvisibility=hidden" CXX_HAS_VH_FLAG)
			CHECK_CXX_COMPILER_FLAG("-fvisibility-inlines-hidden" CXX_HAS_VIH_FLAG)
			#gcc doesn't support visibility on PE/win32
			if(NOT MINGW)
				if(CXX_HAS_VH_FLAG)
					set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
					set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
				endif()
				if(CXX_HAS_VIH_FLAG)
					set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
					set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
				endif()
			endif()
		else()
			if(MSVC)
				# this is the equivalent of -O0 -g according to MSDN
				set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Od /Zi /W3")
				set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Od /Zi /W3")
			else()
				set(CMAKE_CXX_FLAGS "-O0 -g")
				set(CMAKE_C_FLAGS "-O0 -g")
			endif()
		endif()
	else()
		if(WANT_STRIP)
			find_program(STRIP_EXECUTABLE NAMES strip)
			if(STRIP_EXECUTABLE)
				set(CMAKE_STATUS_DEBUG_SUPPORT "No (Stripped)")
				set(CMAKE_BUILD_TYPE Release-stripped CACHE STRING "Choose the type of build, options are: None, Debug, Release, RelWithDebInfo and MinSizeRel." FORCE)
			else()
				set(CMAKE_STATUS_DEBUG_SUPPORT "No (Release)")
				set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None, Debug, Release, RelWithDebInfo and MinSizeRel." FORCE)
			endif()
		else()
			set(CMAKE_STATUS_DEBUG_SUPPORT "No (Release)")
			set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
		endif()
		if(CMAKE_COMPILER_IS_GNUCXX)
			set(CMAKE_CXX_FLAGS "-O3 -fomit-frame-pointer -DNDEBUG --no-enforce-eh-specs -pipe --exec-charset=UTF-8 --input-charset=UTF-8 --no-implement-inlines --unit-at-a-time --fast-math")
			set(CMAKE_C_FLAGS "-O3 -fomit-frame-pointer -DNDEBUG --no-enforce-eh-specs -pipe --exec-charset=UTF-8 --input-charset=UTF-8 --no-implement-inlines --unit-at-a-time --fast-math")
			include(CheckCXXCompilerFlag)
			CHECK_CXX_COMPILER_FLAG("-fvisibility-inlines-hidden" CXX_HAS_VIH_FLAG)
			#gcc doesn't support visibility on PE/win32
			if(CXX_HAS_VIH_FLAG AND NOT MINGW)
				set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
				set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
			endif()
		else()
			if(MSVC)
				#/GL causes compilation error (mix static and dynamic link lib)
				set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /Gw")
				set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /O2 /Gw")
			else()
				set(CMAKE_CXX_FLAGS "-O3")
				set(CMAKE_C_FLAGS "-O3")
			endif()
		endif()
	endif()
endif()

###############################################################################
# Verbose Makefile
###############################################################################

option(WANT_VERBOSE "Generate verbose Makefiles" OFF)
if(WANT_VERBOSE)
	set(CMAKE_VERBOSE_MAKEFILE ON)
	set(CMAKE_STATUS_VERBOSE_SUPPORT "Yes")
	list(APPEND CMAKE_KVIRC_BUILD_FLAGS VERBOSE)
else()
	set(CMAKE_STATUS_VERBOSE_SUPPORT "No")
endif()


###############################################################################
# Version compatibility switch: appends VERSION_MAJOR to KVIrc and kvilib
###############################################################################

option(WANT_COEXISTENCE "Append version to binaries to maintain compatibility with older versions" OFF)
if(WANT_COEXISTENCE)
	set(KVIRC_BINARYNAME ${PACKAGE}${VERSION_MAJOR})
	set(KVILIB_BINARYNAME kvilib${VERSION_MAJOR})
	set(CMAKE_STATUS_COEXISTENCE_VER "Yes")
else()
	set(KVIRC_BINARYNAME ${PACKAGE})
	set(KVILIB_BINARYNAME kvilib)
	set(CMAKE_STATUS_COEXISTENCE_VER "No")
endif()

###############################################################################
# Module install path: KVIrc needs to know where they are installed
###############################################################################

if(UNIX)
	if(APPLE)
		set(CMAKE_KVIRC_RESOURCES_DIR "${CMAKE_INSTALL_PREFIX}/Contents/Resources/")
	else()
		# Assume GNU/Linux
		set(CMAKE_KVIRC_RESOURCES_DIR "${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/")
		set(CMAKE_KVIRC_MODULES_DIR "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/kvirc/${VERSION_BRANCH}/modules")
		list(APPEND CMAKE_KVIRC_BUILD_FLAGS LIB_SUFFIX=${LIB_SUFFIX})
	endif()
elseif(WIN32)
	set(CMAKE_KVIRC_RESOURCES_DIR "${CMAKE_INSTALL_PREFIX}/")
	set(CMAKE_KVIRC_MODULES_DIR "${CMAKE_INSTALL_PREFIX}/modules/")
endif()

###############################################################################
# System checks
###############################################################################

include(CheckTypeSize)
CHECK_TYPE_SIZE("short" CMAKE_SYSTEM_SIZE_OF_SHORT_INT)
CHECK_TYPE_SIZE("int" CMAKE_SYSTEM_SIZE_OF_INT)
CHECK_TYPE_SIZE("long" CMAKE_SYSTEM_SIZE_OF_LONG_INT)
CHECK_TYPE_SIZE("long long" CMAKE_SYSTEM_SIZE_OF_LONG_LONG_INT)

include(CheckIncludeFiles)
CHECK_INCLUDE_FILES(sys/types.h SYSTEM_HAS_SYS_TYPES_H)
CHECK_INCLUDE_FILES(strings.h SYSTEM_HAS_STRINGS_H)

# "dynamic labels" compiler support
try_run(TEST_DID_RUN_DYNLABELS TEST_DID_COMPILE_DYNLABELS "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/dynlabels.cpp")
if(TEST_DID_COMPILE_DYNLABELS)
	string(COMPARE EQUAL "${TEST_DID_RUN_DYNLABELS}" "0" TEST_DID_RUN_DYNLABELS_SUCCESS)
	if(TEST_DID_RUN_DYNLABELS_SUCCESS)
		set(COMPILE_USE_DYNAMIC_LABELS 1)
	else()
		set(COMPILE_USE_DYNAMIC_LABELS 0)
	endif()
else()
	set(COMPILE_USE_DYNAMIC_LABELS 0)
endif()

###############################################################################
# Library and tool checks, in order. First the required ones
# in order of dependency, then the optional ones.
###############################################################################

# ignore sigalarm
option(WANT_IGNORE_SIGALARM "Ignore SIGALARM signal" OFF)
# Check for threads
find_package(Threads REQUIRED)
if(CMAKE_USE_WIN32_THREADS_INIT OR CMAKE_USE_PTHREADS_INIT)
	if(CMAKE_USE_WIN32_THREADS_INIT)
		# No need to link to any specific library
		set(COMPILE_THREADS_USE_WIN32 1)
		set(CMAKE_STATUS_THREADS_SUPPORT "Win32")
		list(APPEND CMAKE_KVIRC_BUILD_FLAGS Threads=Win32)
	else()
		if(CMAKE_HAVE_THREADS_LIBRARY)
			list(APPEND LIBS ${CMAKE_THREAD_LIBS_INIT})
		endif()
		set(COMPILE_THREADS_USE_POSIX 1)
		list(APPEND CMAKE_KVIRC_BUILD_FLAGS Threads=POSIX)
		if(WANT_IGNORE_SIGALARM)
			set(CMAKE_STATUS_THREADS_SUPPORT "POSIX (ignoring SIGALARM)")
			set(COMPILE_IGNORE_SIGALARM 1)
		else()
			set(CMAKE_STATUS_THREADS_SUPPORT "POSIX")
		endif()
	endif()
else()
	set(CMAKE_STATUS_THREADS_SUPPORT "Not used")
endif()

# Check for ZLib
find_package(ZLIB REQUIRED)
if(ZLIB_FOUND)
	include_directories(${ZLIB_INCLUDE_DIR})
	list(APPEND LIBS ${ZLIB_LIBRARIES})
	set(COMPILE_ZLIB_SUPPORT 1)
endif()

############################################################################
# Check for Qt5
############################################################################

find_package(Qt5Widgets)
if(Qt5Widgets_FOUND)
	set(CMAKE_STATUS_QT_VERSION "${Qt5Widgets_VERSION_STRING}")
	set(qt5_kvirc_modules)
else()
	message(FATAL_ERROR "No Qt5 found!")
endif()

# automoc is used by both qt4 and qt5
set(CMAKE_AUTOMOC TRUE)

# Shared options
if(APPLE OR WIN32)
	# No Qt D-Bus on Mac and Windows
	option(WANT_QTDBUS "Whether to compile Qt D-Bus support" OFF)
else()
	option(WANT_QTDBUS "Whether to compile Qt D-Bus support" ON)
endif()

option(WANT_QTWEBKIT "Whether to compile Qt WebKit support" ON)
option(WANT_QTSVG "Whether to compile Qt SVG support" ON)

# May be re-set below, if the proper libs are found.
set(CMAKE_STATUS_QX11INFO_SUPPORT "Not available")

include_directories(${Qt5Widgets_INCLUDE_DIRS})
list(APPEND qt5_kvirc_modules Qt5::Widgets)

# Qt SQL
find_package(Qt5Sql)
if(Qt5Sql_FOUND)
	include_directories(${Qt5Sql_INCLUDE_DIRS})
	list(APPEND qt5_kvirc_modules Qt5::Sql)
endif()

# Qt XML
find_package(Qt5Xml)
if(Qt5Xml_FOUND)
	include_directories(${Qt5Xml_INCLUDE_DIRS})
	list(APPEND qt5_kvirc_modules Qt5::Xml)
endif()

# Qt Network
find_package(Qt5Network)
if(Qt5Network_FOUND)
	include_directories(${Qt5Network_INCLUDE_DIRS})
	list(APPEND qt5_kvirc_modules Qt5::Network)
endif()

# Qt PrintSupport
find_package(Qt5PrintSupport)
if(Qt5PrintSupport_FOUND)
	include_directories(${Qt5PrintSupport_INCLUDE_DIRS})
	list(APPEND qt5_kvirc_modules Qt5::PrintSupport)
endif()

# Qt Multimedia
find_package(Qt5Multimedia)
if(Qt5Multimedia_FOUND)
	include_directories(${Qt5Multimedia_INCLUDE_DIRS})
	list(APPEND qt5_kvirc_modules Qt5::Multimedia)
endif()

# Qt D-Bus support
if(WANT_QTDBUS)
	find_package(Qt5DBus)
	if(Qt5DBus_FOUND)
		list(APPEND qt5_kvirc_modules Qt5::DBus)
		include_directories(${Qt5DBus_INCLUDE_DIRS})
		set(COMPILE_DBUS_SUPPORT 1)
	endif()
endif()

# Qt WebKit support
if(WANT_QTWEBKIT)
	find_package(Qt5WebKitWidgets)
	if(Qt5WebKitWidgets_FOUND)
		list(APPEND qt5_kvirc_modules Qt5::WebKitWidgets)
		include_directories(${Qt5WebKitWidgets_INCLUDE_DIRS})
		set(COMPILE_WEBKIT_SUPPORT 1)
	endif()
endif()

# Qt SVG support
if(WANT_QTSVG)
	find_package(Qt5Svg)
	if(Qt5Svg_FOUND)
		list(APPEND qt5_kvirc_modules Qt5::Svg)
		include_directories(${Qt5Svg_INCLUDE_DIRS})
		set(COMPILE_SVG_SUPPORT 1)
	endif()
endif()

if(WIN32)
	find_package(Qt5WinExtras)
	if(Qt5WinExtras_FOUND)
		list(APPEND qt5_kvirc_modules Qt5::WinExtras)
		include_directories(${Qt5WinExtras_INCLUDE_DIRS})
	endif()
endif()

# Qt5 requires the X11Extras module for the QX11Info class (which is needed for XSS support and other stuff)
if(UNIX AND NOT APPLE)
	find_package(Qt5X11Extras)
	if(Qt5X11Extras_FOUND)
		set(COMPILE_QX11INFO_SUPPORT 1)
		set(CMAKE_STATUS_QX11INFO_SUPPORT "Yes")
		list(APPEND qt5_kvirc_modules Qt5::X11Extras)
		include_directories(${Qt5X11Extras_INCLUDE_DIRS})
	endif()
endif()

############################################################################
# Check for KDE
############################################################################

option(WANT_KDE "Whether to use KDE support" ON)

set(CMAKE_RESULT_USING_KDE false)

if(WANT_KDE)
	# Using Qt5 -> KDE5
	find_package(ECM 1.0.0 QUIET NO_MODULE)
	if(DEFINED ECM_VERSION_MAJOR)
		set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_MODULE_PATH})

		find_package(KF5 "5.2.0" COMPONENTS
				CoreAddons      # KAboutData
				I18n            # KLocalizedString
				XmlGui          # KMainWindow
				WindowSystem    # KWindowSystem
				Notifications   # KNotification
				Service         # KToolInvocation
			)

		if(KF5_FOUND)
			set(CMAKE_RESULT_USING_KDE true)
			#include_directories(${KDE4_INCLUDES})
			#add_definitions(${KDE4_DEFINITIONS})
			set(COMPILE_KDE_SUPPORT 1)
			# There seems to be no portable way to figure out if KDE version is 4 or 5
			set(COMPILE_KDE5_SUPPORT 1)
			list(APPEND LIBS KF5::CoreAddons KF5::I18n KF5::XmlGui KF5::WindowSystem KF5::Notifications KF5::Service)
			set(CMAKE_STATUS_KDE_SUPPORT "Yes")
			if(DEFINED KF5_VERSION)
				set(CMAKE_STATUS_KDE_VERSION ${KF5_VERSION})
			else()
				set(CMAKE_STATUS_KDE_VERSION "5.X")
			endif()
		else()
			set(CMAKE_STATUS_KDE_SUPPORT "Not available (KF5 not found)")
		endif()
	else()
		set(CMAKE_STATUS_KDE_SUPPORT "Not available (ECM not found)")
	endif()
else()
	set(CMAKE_STATUS_KDE_SUPPORT "Not requested")
endif()

# Check for X11
if(UNIX)
	if(APPLE)
		set(CMAKE_STATUS_X11_SUPPORT "Not used")
	else()
		# Assume Linux
		find_package(X11)
		if(X11_FOUND)
			set(COMPILE_X11_SUPPORT 1)
			# The X screensaver stuff is in the libxss-dev package (Ubuntu/Debian)
			if(X11_Xscreensaver_FOUND)
				# QX11Info is needed to interact with Xscreensaver
				if(COMPILE_QX11INFO_SUPPORT)
					set(COMPILE_XSS_SUPPORT 1)
					set(CMAKE_STATUS_X11_SUPPORT "Yes, with scrnsaveproto")
					include_directories(${X11_Xscreensaver_INCLUDE_PATH})
					list(APPEND LIBS ${X11_Xscreensaver_LIB})
				else()
					set(CMAKE_STATUS_X11_SUPPORT "Yes")
				endif()
			else()
				set(CMAKE_STATUS_X11_SUPPORT "Yes")
			endif()
			include_directories(${X11_INCLUDE_DIR})
			list(APPEND LIBS ${X11_LIBRARIES})
			# Seems like ${X11_LIBRARIES} doesn't add -lXrender, which is needed
			# for building with --no-add-needed/binutils-gold, see
			# <http://bugs.debian.org/555068> for details.
			if(X11_Xrender_FOUND)
				list(APPEND LIBS ${X11_Xrender_LIB})
			endif(X11_Xrender_FOUND)
		else()
			set(CMAKE_STATUS_X11_SUPPORT "Not available")
		endif()
	endif()
else()
	set(CMAKE_STATUS_X11_SUPPORT "Not needed")
endif()

# Phonon support
option(WANT_PHONON "Whether to compile Phonon support" ON)
if(WANT_PHONON)
	find_package(Phonon4Qt5 NO_MODULE)
	if(Phonon4Qt5_FOUND)
		set(COMPILE_PHONON_SUPPORT 1)
		list(APPEND LIBS ${PHONON_LIBRARY})
		list(APPEND CMAKE_STATUS_AUDIO_BACKENDS "Phonon")
	endif()
endif()

# this is a no-op - not nice i know, but real no point in making it optional...
if(Qt5Multimedia_FOUND)
	list(APPEND CMAKE_STATUS_AUDIO_BACKENDS "Qt5Multimedia")
endif()

# ESD
option(WANT_ESD "Whether to compile ESD support" ON)
if(WANT_ESD)
	find_package(ESD)
	if(ESD_FOUND)
		set(COMPILE_ESD_SUPPORT 1)
		list(APPEND LIBS ${ESD_LIBRARY})
		list(APPEND CMAKE_STATUS_AUDIO_BACKENDS "Esd")
	endif()
endif()

# oss + audiofile
option(WANT_OSS "Whether to compile OSS support" ON)
option(WANT_AUDIOFILE "Whether to compile audiofile support" ON)

if(WANT_OSS)
	find_package(OSS)
	if(OSS_FOUND)
		set(COMPILE_OSS_SUPPORT 1)
	endif()

	if(WANT_AUDIOFILE)
		find_package(Audiofile)
		if(AUDIOFILE_FOUND)
			set(COMPILE_AUDIOFILE_SUPPORT 1)
			list(APPEND LIBS ${AUDIOFILE_LIBRARY})
			list(APPEND CMAKE_STATUS_AUDIO_BACKENDS "Oss+AudioFile")
		endif()
	else()
		list(APPEND CMAKE_STATUS_AUDIO_BACKENDS "Oss")
	endif()
endif()

#winmm - win32
if(WIN32)
	list(APPEND CMAKE_STATUS_AUDIO_BACKENDS "WinMM")
endif()

############################################################################
# OpenSSL support (Secure Sockets, Certificates, Digests etc.)
############################################################################

option(WANT_OPENSSL "Whether to compile OpenSSL support" ON)

if(WANT_OPENSSL)
	find_package(OpenSSL)
	if(OPENSSL_FOUND)
		if(WIN32)
			# On windows/mingw we need libeay32 too...
			find_library(LIB_EAY NAMES libeay32MD eay libeay libeay32 libcrypto
									${_OPENSSL_ROOT_HINTS_AND_PATHS}
									PATH_SUFFIXES
									"lib"
									"lib/MinGW")
			if(LIB_EAY)
				include_directories(${OPENSSL_INCLUDE_DIR})
				# The following are here in order to pick the applink.c file
				# The documentation says that it should be included in OPENSSL_INCLUDE_DIR/openssl
				# but in the package I have now it's actually in OPENSSL_INCLUDE_DIR/../ms .
				include_directories(${OPENSSL_INCLUDE_DIR}/openssl)
				include_directories(${OPENSSL_INCLUDE_DIR}/../ms)
				list(APPEND LIBS ${OPENSSL_LIBRARIES} ${LIB_EAY})
				set(COMPILE_SSL_SUPPORT 1)
				set(CMAKE_STATUS_SSL_SUPPORT "Yes")
			else()
				set(CMAKE_STATUS_SSL_SUPPORT "Not available, libeay missing")
			endif()
		else()
			include_directories(${OPENSSL_INCLUDE_DIR})
			list(APPEND LIBS ${OPENSSL_LIBRARIES})
			set(COMPILE_SSL_SUPPORT 1)
			set(CMAKE_STATUS_SSL_SUPPORT "Yes")
			# CMake FindOpenSSL is incomplete and does not add -lcrypto to LIBS...
			list(APPEND LIBS -lcrypto)
		endif()
	else()
		set(CMAKE_STATUS_SSL_SUPPORT "Not available")
	endif()
else()
	set(CMAKE_STATUS_SSL_SUPPORT "No")
endif()

############################################################################
# Crypt engines support
############################################################################

option(WANT_CRYPT "Compile cryptographic support and engines" ON)
if(WANT_CRYPT)
	set(COMPILE_CRYPT_SUPPORT 1)
	set(CMAKE_STATUS_CRYPT_SUPPORT "Yes")
else()
	set(CMAKE_STATUS_CRYPT_SUPPORT "No")
endif()

############################################################################
# IPv6 support
############################################################################

option(WANT_IPV6 "Compile IPv6 support" ON)
if(WANT_IPV6)
	set(COMPILE_IPV6_SUPPORT 1)
	set(CMAKE_STATUS_IPV6_SUPPORT "Yes")
else()
	set(CMAKE_STATUS_IPV6_SUPPORT "No")
endif()

############################################################################
# Do we want pseudo-transparency?
############################################################################

option(WANT_TRANSPARENCY "Compile transparency support" ON)
if(WANT_TRANSPARENCY)
	set(COMPILE_PSEUDO_TRANSPARENCY 1)
	set(CMAKE_STATUS_TRANSPARENCY_SUPPORT "Yes")
else()
	set(CMAKE_STATUS_TRANSPARENCY_SUPPORT "No")
endif()

# Check for Perl support
# (note: needed for user documentation generation)
option(WANT_PERL "Compile Perl support" ON)

# Perl is needed for documentation, too
find_package(Perl)

if(WANT_PERL)
	if(PERL_FOUND)
		set(COMPILE_PERL_SUPPORT 1)
		set(CMAKE_STATUS_PERL_SUPPORT "Yes, used for scripting+doc")

		# Perl compilation / linking flags are handled inside perlcore module
	else()
		set(CMAKE_STATUS_PERL_SUPPORT "No")
	endif()
else()
	if(PERL_FOUND)
		set(CMAKE_STATUS_PERL_SUPPORT "Used for doc, but scripting disabled by user")
	else()
		set(CMAKE_STATUS_PERL_SUPPORT "No")
	endif()
endif()

# Check for Python support
option(WANT_PYTHON "Compile Python support" ON)
if(WANT_PYTHON)
	find_package(PythonLibs 2.7)
	if(PYTHONLIBS_FOUND)
		set(COMPILE_PYTHON_SUPPORT 1)
		set(CMAKE_STATUS_PYTHON_SUPPORT "Yes")
		list(APPEND LIBS ${PYTHON_LIBRARIES})
		include_directories(${PYTHON_INCLUDE_DIRS})
	else()
		set(CMAKE_STATUS_PYTHON_SUPPORT "No, not found.")
	endif()
else()
	set(CMAKE_STATUS_PYTHON_SUPPORT "No")
endif()

############################################################################
# Enchant (spell checker) support
############################################################################

option(WANT_SPELLCHECKER "Compile spell-checker support" ON)
if(WANT_SPELLCHECKER)
	find_package(PkgConfig)
	pkg_search_module(Enchant enchant-2 enchant)
	if(Enchant_FOUND)
		set(CMAKE_STATUS_SPELLCHECKER_SUPPORT "Yes")
		include_directories(${Enchant_INCLUDE_DIRS})
		list(APPEND LIBS ${Enchant_LDFLAGS})
		set(COMPILE_ENCHANT_SUPPORT 1)
	else()
		set(CMAKE_STATUS_SPELLCHECKER_SUPPORT "No (libenchant not found)")
	endif()
else()
	set(CMAKE_STATUS_SPELLCHECKER_SUPPORT "No")
endif()

############################################################################
# IPC (inter-process communication) support
############################################################################

option(WANT_IPC "Compile inter-process communication support" ON)
if(WANT_IPC)
	set(CMAKE_STATUS_IPC_SUPPORT "Yes")
else()
	set(COMPILE_NO_IPC 1)
	set(CMAKE_STATUS_IPC_SUPPORT "No")
endif()


############################################################################
# NETWORK FUNCTIONS support
############################################################################

include(CheckFunctionExists)
CHECK_FUNCTION_EXISTS("inet_ntoa" HAVE_INET_NTOA_EXISTS)
# what systems lack this!?
if(HAVE_INET_NTOA_EXISTS)
	set(HAVE_INET_NTOA 1)
endif()

CHECK_FUNCTION_EXISTS("inet_aton" HAVE_INET_ATON_EXISTS)
# what systems lack this!?
if(HAVE_INET_ATON_EXISTS)
	set(HAVE_INET_ATON 1)
endif()

# Check GET_INTERFACE_ADDRESS support
if(NOT WIN32)
	find_path(GET_INTERFACE_ADDRESS_INCLUDE_DIR net/if.h)
	if(GET_INTERFACE_ADDRESS_INCLUDE_DIR)
		set(COMPILE_GET_INTERFACE_ADDRESS 1)
	endif()
endif()

############################################################################
# SetEnv/PutEnv support
############################################################################

CHECK_FUNCTION_EXISTS("setenv" HAVE_SETENV_EXISTS)
# what systems lack this!?
if(HAVE_SETENV_EXISTS)
	set(HAVE_SETENV 1)
endif()

CHECK_FUNCTION_EXISTS("putenv" HAVE_PUTENV_EXISTS)
# what systems lack this!?
if(HAVE_PUTENV_EXISTS)
	set(HAVE_PUTENV 1)
endif()

############################################################################
# GSM support
############################################################################

option(WANT_GSM "Compile GSM support" ON)
if(WANT_GSM)
	set(COMPILE_USE_GSM 1)
	set(CMAKE_STATUS_GSM_SUPPORT "Yes")
else()
	set(CMAKE_STATUS_GSM_SUPPORT "No")
endif()

############################################################################
# DCC canvas support
############################################################################

#option(WANT_DCC_CANVAS "Compile DCC Canvas Support" OFF)
#if(WANT_DCC_CANVAS)
#	set(CMAKE_STATUS_DCC_CANVAS_SUPPORT "Yes")
#	set(COMPILE_DCC_CANVAS 1)
#else()
#	set(CMAKE_STATUS_DCC_CANVAS_SUPPORT "No")
#endif()

############################################################################
# DCC voice support
############################################################################

CHECK_INCLUDE_FILE(linux/soundcard.h CMAKE_HAVE_LINUX_SOUNDCARD_H)
CHECK_INCLUDE_FILE(sys/soundcard.h CMAKE_HAVE_SYS_SOUNDCARD_H)
CHECK_INCLUDE_FILE(soundcard.h CMAKE_HAVE_SOUNDCARD_H)

option(WANT_DCC_VOICE "Compile DCC voice support" ON)
if(WANT_DCC_VOICE)
	if(CMAKE_HAVE_LINUX_SOUNDCARD_H)
		set(CMAKE_STATUS_DCC_VOICE_SUPPORT "Yes, found linux/soundcard.h")
		set(HAVE_LINUX_SOUNDCARD_H 1)
	elseif(CMAKE_HAVE_SYS_SOUNDCARD_H)
		set(CMAKE_STATUS_DCC_VOICE_SUPPORT "Yes, found sys/soundcard.h")
		set(HAVE_SYS_SOUNDCARD_H 1)
	elseif(CMAKE_HAVE_SOUNDCARD_H)
		set(CMAKE_STATUS_DCC_VOICE_SUPPORT "Yes, found soundcard.h")
		set(HAVE_SOUNDCARD_H 1)
	else()
		set(COMPILE_DISABLE_DCC_VOICE 1)
		set(CMAKE_STATUS_DCC_VOICE_SUPPORT "No, soundcard.h not found")
	endif()
else()
	set(COMPILE_DISABLE_DCC_VOICE 1)
	set(CMAKE_STATUS_DCC_VOICE_SUPPORT "No")
endif()

############################################################################
# DCC video support + extended webcam support (LibV4L2)
############################################################################

option(WANT_DCC_VIDEO "Compile DCC video support" OFF)

if(WANT_DCC_VIDEO)
	# Qt MultimediaWidgets
	find_package(Qt5MultimediaWidgets)
	if(Qt5MultimediaWidgets_FOUND)
	    include_directories(${Qt5MultimediaWidgets_INCLUDE_DIRS})
	    list(APPEND qt5_kvirc_modules Qt5::MultimediaWidgets)
	    set(COMPILE_DISABLE_DCC_VIDEO 0)
	    set(CMAKE_STATUS_DCC_VIDEO_SUPPORT "Yes")
	else()
		set(COMPILE_DISABLE_DCC_VIDEO 1)
		set(CMAKE_STATUS_DCC_VIDEO_SUPPORT "Not available")
	endif()
else()
	set(COMPILE_DISABLE_DCC_VIDEO 1)
	set(CMAKE_STATUS_DCC_VIDEO_SUPPORT "No")
endif()

############################################################################
# Ogg/Vorbis and Ogg/Theora support
############################################################################

option(WANT_OGG_THEORA "Compile Ogg/Vorbis+Theora support" OFF)

if(WANT_OGG_THEORA)
	find_package(Theora)
	if(THEORA_FOUND)
		set(CMAKE_STATUS_OGG_THEORA_SUPPORT "Yes")
		list(APPEND LIBS ${THEORA_LIBRARY})
		include_directories(${THEORA_INCLUDE_DIR})
	else()
		set(COMPILE_DISABLE_OGG_THEORA 1)
		set(CMAKE_STATUS_OGG_THEORA_SUPPORT "No")
	endif()
else()
	set(COMPILE_DISABLE_OGG_THEORA 1)
	set(CMAKE_STATUS_OGG_THEORA_SUPPORT "No")
endif()

############################################################################
# Memory profiling support
############################################################################

option(WANT_MEMORY_PROFILE "Compile memory profiling support" OFF)
if(WANT_MEMORY_PROFILE)
	set(COMPILE_MEMORY_PROFILE 1)
	set(CMAKE_STATUS_MEMORY_PROFILE_SUPPORT "User enabled")
else()
	set(CMAKE_STATUS_MEMORY_PROFILE_SUPPORT "No")
endif()

############################################################################
# Memory checks support
############################################################################

option(WANT_MEMORY_CHECKS "Compile memory checking support" OFF)
if(WANT_MEMORY_CHECKS)
	set(COMPILE_MEMORY_CHECKS 1)
	set(CMAKE_STATUS_MEMORY_CHECKS_SUPPORT "User enabled")
else()
	set(CMAKE_STATUS_MEMORY_CHECKS_SUPPORT "No")
endif()

############################################################################
# Platform Specific checks
############################################################################

if(WIN32)
	# 0x0601 = Windows 7
	add_definitions(-D_WIN32_WINNT=0x0601 -DWINVER=0x0601 -DWIN32_LEAN_AND_MEAN)
endif()

if(MINGW)
	list(APPEND LIBS -lshlwapi)
	add_definitions(-DMINGW)
	set(ADDITIONAL_LINK_FLAGS "${ADDITIONAL_LINK_FLAGS} -Wl,--enable-auto-import,--enable-runtime-pseudo-reloc -fno-exceptions")
	#remove leading and trailing spaces (string strip)
	string(REGEX REPLACE "(^( )+|( )+$)" "" ADDITIONAL_LINK_FLAGS "${ADDITIONAL_LINK_FLAGS}")
endif()

if(MSVC)
	add_definitions(-D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES -DNOMINMAX)
	add_definitions(-DMSVC)
	# Support from Windows XP
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
endif()

if(${CMAKE_CXX_COMPILER_ID} MATCHES "SunPro")
	# SunPro breaks with non-C, and we can't set it here, so nag the user.
	# looks nasty, maybe there is a better way?
	if(NOT ($ENV{LANG} STREQUAL "C"))
  	message( FATAL_ERROR "Please set environment variable \"LANG\" to \"C\" and rerun CMake" )
	endif()
endif()

############################################################################
# Very important checks
############################################################################

option(WANT_PIZZA "Pizza for everyone" OFF)
if(WANT_PIZZA)
	message(STATUS "Checking for mozzarella...")
	message(STATUS "Checking for mozzarella - found")
	message(STATUS "Checking for tomatoes...")
	message(STATUS "Checking for tomatoes... - found")
	message(STATUS "Starting up the oven...")
	message(STATUS "Starting up the oven... - done")
endif()

option(WANT_BEER "Add some PaleALE to the compilation" OFF)
if(WANT_BEER)
	message(STATUS "Checking for beer...")
	message(STATUS "Checking for beer... - found")
	message(STATUS "Opening the beer...")
	message(STATUS "Opening the beer... -  done")
endif()

############################################################################
# By default we disable QGtkStyle since it messes up with our theming
# Some users want it anyway, and we sometimes need to check it
############################################################################

option(WANT_GTKSTYLE "Enable QGtkStyle" OFF)
if(WANT_GTKSTYLE)
	set(COMPILE_ENABLE_GTKSTYLE 1)
	set(CMAKE_STATUS_GTK_SUPPORT "Yes")
else()
	set(CMAKE_STATUS_GTK_SUPPORT "No")
endif()

###############################################################################
# Translation (gettext) checks
###############################################################################

option(WANT_GETTEXT "Whether to use gettext to generate translations" ON)
if(WANT_GETTEXT)
	find_program(GETTEXT_MSGMERGE_EXECUTABLE msgmerge)
	find_program(GETTEXT_MSGFMT_EXECUTABLE msgfmt)
	find_program(GETTEXT_XGETTEXT_EXECUTABLE xgettext)
	find_program(SED_EXECUTABLE sed)

	if(GETTEXT_MSGFMT_EXECUTABLE)
		add_custom_target("kvirc-translations" ALL)
		set(CMAKE_STATUS_GETTEXT_SUPPORT "Yes")
		set(USE_GETTEXT_TRANSLATIONS 1)
	else()
		set(CMAKE_STATUS_GETTEXT_SUPPORT "No")
		set(USE_GETTEXT_TRANSLATIONS 0)
	endif()
	if(GETTEXT_MSGMERGE_EXECUTABLE AND GETTEXT_XGETTEXT_EXECUTABLE)
		# messages-update target;
		add_custom_target(messages-update)
		# messages-extract target;
		add_custom_target(messages-extract)
		add_dependencies(messages-update messages-extract)
		set(USE_GETTEXT_EXTRA 1)
		set(GETTEXT_EXTRA_STATUS ", + extra")
	else()
		set(USE_GETTEXT_EXTRA 0)
		set(GETTEXT_EXTRA_STATUS)
	endif()

	if(SED_EXECUTABLE)
		# wipes absolute paths from .pot? files, by mren
		add_custom_target(messages-tidy)
		file(GLOB_RECURSE FILES_TO_TIDY ${CMAKE_SOURCE_DIR}/po/*.po)
		file(GLOB_RECURSE POTS_TO_TIDY ${CMAKE_SOURCE_DIR}/po/*.pot)
		list(APPEND FILES_TO_TIDY ${POTS_TO_TIDY})
		if(APPLE)
			foreach(curFile ${FILES_TO_TIDY})
				add_custom_command(TARGET messages-tidy
				COMMENT "Tidying ${curFile}..."
				COMMAND ${SED_EXECUTABLE} -i "" -e "s|${CMAKE_SOURCE_DIR}/||g" "${curFile}"
				VERBATIM
				DEPENDS ${curFile}
				)
			endforeach()
		else()
			foreach(curFile ${FILES_TO_TIDY})
				add_custom_command(TARGET messages-tidy
				COMMENT "Tidying ${curFile}..."
				COMMAND ${SED_EXECUTABLE} -i -e "s|${CMAKE_SOURCE_DIR}/||g" "${curFile}"
				VERBATIM
				DEPENDS ${curFile}
				)
			endforeach()
		endif()
		if(GETTEXT_MSGMERGE_EXECUTABLE AND GETTEXT_XGETTEXT_EXECUTABLE)
			add_dependencies(messages-update messages-tidy)
		endif()
		set(CMAKE_STATUS_MESSAGE_TIDY "Yes")
	else()
		set(CMAKE_STATUS_MESSAGE_TIDY "Sed not found")
	endif()

else()
	set(CMAKE_STATUS_GETTEXT_SUPPORT "No")
endif()

# Search for subdirectories; under macOS, data _MUST_ be before src (to get Info.plist installed before the main executable's fixup_bundle step)
subdirs(data doc po scripts src)

#install additional resources for win32
if(WIN32)
	add_subdirectory(dist/windows)
endif()

###############################################################################
# The API documentation target
###############################################################################

option(WANT_DOXYGEN "Whether to use Doxygen to generate documentation" ON)
if(WANT_DOXYGEN)
	find_package(Doxygen)
	if(DOXYGEN_FOUND)
		set(CMAKE_STATUS_DOXYGEN_SUPPORT "Yes")
		add_custom_target(
			devdocs
			@echo Generating api documentation
		)

		add_custom_command(
			COMMENT "generate the API documentation"
			COMMAND mkdir -p ${CMAKE_BINARY_DIR}/doc/api
			COMMAND cd ${CMAKE_BINARY_DIR}/admin && ${DOXYGEN_EXECUTABLE} Doxyfile
			TARGET devdocs
		)
	else()
		set(CMAKE_STATUS_DOXYGEN_SUPPORT "No")
	endif()
else()
	set(CMAKE_STATUS_DOXYGEN_SUPPORT "No")
endif()

###############################################################################
# The User documentation target (KVIrc internal help)
###############################################################################

#currently WIP
#option(WANT_USERDOCS "Whether to generate user documentation (aka internal help)" ON)
#if(WANT_USERDOCS)
#	set(CMAKE_STATUS_GEN_USERDOC "Yes")
#else()
#	set(CMAKE_STATUS_GEN_USERDOC "User disabled")
#endif()

###############################################################################
# We want the "distclean" target
###############################################################################

if(UNIX)
	add_custom_target(
		distclean
		@echo Cleaning for source distribution
	)
	set(
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
	add_custom_command(
		TARGET  distclean POST_BUILD
		DEPENDS clean
		COMMENT "distribution clean"
		COMMAND rm
		ARGS    -Rf CMakeTmp CMakeFiles doc/api ${DISTCLEANED}
	)
endif()


###############################################################################
# CONFIGURE FILES
###############################################################################

# System configuration is now split into 2 files
# kvi_sysconfig.h contains the "saveable" configuration: stuff that
# may be preserved between multiple CMake runs (and thus may save compilation time)
# kvi_sysbuildinfo.h contains the configuration that changes at every
# CMake run and must be recompiled every time. Extra care is taken
# inside the sources to include kvi_sysbuildinfo.h in only ONE file (so only
# linking of kvilib is done if only kvi_sysbuildinfo.h changes.

if(EXISTS ${PROJECT_BINARY_DIR}/kvi_sysconfig.h)
	# an old config exists; let's check if it's actual:
	configure_file(${PROJECT_SOURCE_DIR}/cmake/kvi_sysconfig.h.cmake ${PROJECT_BINARY_DIR}/kvi_sysconfig.h.new)
	file(READ ${PROJECT_BINARY_DIR}/kvi_sysconfig.h OLD_CONFIG_CONTENTS)
	file(READ ${PROJECT_BINARY_DIR}/kvi_sysconfig.h.new NEW_CONFIG_CONTENTS)
	string(COMPARE EQUAL "${OLD_CONFIG_CONTENTS}" "${NEW_CONFIG_CONTENTS}" CONFIG_IS_UPTODATE)
	if(${CONFIG_IS_UPTODATE})
		# we don't overwrite the kvi_sysconfig.h file to gain compilation time
		file(REMOVE ${PROJECT_BINARY_DIR}/kvi_sysconfig.h.new)
	else()
		# let's overwrite the kvi_sysconfig.h file (config has changed)
		# FIXME: this is tricky, better use ${CMAKE_COMMAND} -E
		file(REMOVE ${PROJECT_BINARY_DIR}/kvi_sysconfig.h.new)
		configure_file(${PROJECT_SOURCE_DIR}/cmake/kvi_sysconfig.h.cmake ${PROJECT_BINARY_DIR}/kvi_sysconfig.h)
	endif()
else()
	# no old config
	configure_file(${PROJECT_SOURCE_DIR}/cmake/kvi_sysconfig.h.cmake ${PROJECT_BINARY_DIR}/kvi_sysconfig.h)
endif()

configure_file(${PROJECT_SOURCE_DIR}/cmake/kvi_sysbuildinfo.h.cmake ${PROJECT_BINARY_DIR}/kvi_sysbuildinfo.h)

if(WIN32)
	configure_file(${PROJECT_SOURCE_DIR}/cmake/version.h.cmake ${PROJECT_SOURCE_DIR}/data/resources_win32/version.h)
endif()

include_directories(${PROJECT_BINARY_DIR})


if(WANT_COEXISTENCE)
	configure_file(${kvirc_SOURCE_DIR}/cmake/kvirc-config.cmake ${CMAKE_CURRENT_BINARY_DIR}/scripts/config/kvirc${VERSION_MAJOR}-config @ONLY)
else()
	configure_file(${kvirc_SOURCE_DIR}/cmake/kvirc-config.cmake ${CMAKE_CURRENT_BINARY_DIR}/scripts/config/kvirc-config @ONLY)
endif()

#configure_file(${kvirc_SOURCE_DIR}/cmake/FindKVIrc.cmake.in ${kvirc_BINARY_DIR}/FindKVIrc.cmake @ONLY)

if(WANT_DOXYGEN)
	if(DOXYGEN_FOUND)
		configure_file(${CMAKE_SOURCE_DIR}/cmake/Doxyfile.cmake ${CMAKE_BINARY_DIR}/admin/Doxyfile @ONLY)
	endif()
endif()

###############################################################################
# Mark as advanced some variables we really don't want to see in "simple" mode
# All of them *should* be auto-detected.
###############################################################################

mark_as_advanced(FORCE QT_QMAKE_EXECUTABLE)
mark_as_advanced(FORCE GETTEXT_MSGFMT_EXECUTABLE)
mark_as_advanced(FORCE GETTEXT_MSGMERGE_EXECUTABLE)
mark_as_advanced(FORCE LIBRARY_OUTPUT_PATH)
mark_as_advanced(FORCE CMAKE_BACKWARDS_COMPATIBILITY)
mark_as_advanced(FORCE EXECUTABLE_OUTPUT_PATH)

###############################################################################
# ADD LINK FLAGS TO LIBS: THIS HAS TO BE DONE LAST
###############################################################################

list(APPEND LIBS "${ADDITIONAL_LINK_FLAGS}")

###############################################################################
# FINAL OUTPUT
###############################################################################

# Binary path
if(UNIX)
	if(APPLE)
		set(KVIRC_BIN_PATH ${CMAKE_INSTALL_PREFIX}/Contents/MacOS/)
	else()
		# Assume Linux
		set(KVIRC_BIN_PATH ${CMAKE_INSTALL_PREFIX}/bin/)
	endif()
elseif(WIN32)
	set(KVIRC_BIN_PATH ${CMAKE_INSTALL_PREFIX}/)
endif()

# KviLib path
if(UNIX)
	if(APPLE)
		set(KVIRC_LIB_PATH ${CMAKE_INSTALL_PREFIX}/Contents/MacOS/)
	else()
		# Assume Linux
		set(KVIRC_LIB_PATH ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/)
	endif()
elseif(WIN32)
	set(KVIRC_LIB_PATH ${CMAKE_INSTALL_PREFIX}/)
endif()

# Modules path
if(UNIX)
	if(APPLE)
		set(KVIRC_MOD_PATH ${CMAKE_INSTALL_PREFIX}/Contents/Resources/modules/)
	else()
		# Assume GNU/Linux
		set(KVIRC_MOD_PATH ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/kvirc/${VERSION_BRANCH}/modules/)
	endif()
elseif(WIN32)
	set(KVIRC_MOD_PATH ${CMAKE_INSTALL_PREFIX}/modules/)
endif()

# Man path
if(UNIX)
	if(APPLE)
		# does macOS use man pages? <= yes!
	else()
		# Assume GNU/Linux
		set(KVIRC_MAN_PATH ${CMAKE_INSTALL_PREFIX}/${MANDIR_PREFIX}/)
	endif()
else()
		set(KVIRC_MAN_PATH "Not available")
endif()

message(STATUS " ")
message(STATUS "OK, this is the configuration I've worked out:")
if(${CONFIG_IS_UPTODATE})
	message(STATUS "(I found an old config that was exactly the same and kept it to save compilation time)")
endif()
message(STATUS " ")
message(STATUS "Paths:")
message(STATUS "   Binary path                 : ${KVIRC_BIN_PATH}")
message(STATUS "   Library path                : ${KVIRC_LIB_PATH}")
message(STATUS "   Modules path                : ${KVIRC_MOD_PATH}")
message(STATUS "   Man path                    : ${KVIRC_MAN_PATH}")
message(STATUS "Compilation:")
message(STATUS "   Build type                  : ${CMAKE_BUILD_TYPE}")
message(STATUS "   Debug                       : ${CMAKE_STATUS_DEBUG_SUPPORT}")
message(STATUS "   Verbose output              : ${CMAKE_STATUS_VERBOSE_SUPPORT}")
message(STATUS "   Coexistence support         : ${CMAKE_STATUS_COEXISTENCE_VER}")
message(STATUS "   Threading support           : ${CMAKE_STATUS_THREADS_SUPPORT}")
message(STATUS "   Memory profile support      : ${CMAKE_STATUS_MEMORY_PROFILE_SUPPORT}")
message(STATUS "   Memory checks support       : ${CMAKE_STATUS_MEMORY_CHECKS_SUPPORT}")
message(STATUS "Features:")
message(STATUS "   X11 support                 : ${CMAKE_STATUS_X11_SUPPORT}")
message(STATUS "   Qt version                  : ${CMAKE_STATUS_QT_VERSION}")
message(STATUS "   Qt modules                  : ${qt5_kvirc_modules}")
message(STATUS "   Qt/X11 interface            : ${CMAKE_STATUS_QX11INFO_SUPPORT}")
message(STATUS "   KDE support                 : ${CMAKE_STATUS_KDE_SUPPORT}")
if(CMAKE_RESULT_USING_KDE)
	message(STATUS "   KDE version                 : ${CMAKE_STATUS_KDE_VERSION}")
endif()
message(STATUS "   GTK support                 : ${CMAKE_STATUS_GTK_SUPPORT}")
message(STATUS "   IPv6 support                : ${CMAKE_STATUS_IPV6_SUPPORT}")
message(STATUS "   Pseudo-transparency support : ${CMAKE_STATUS_TRANSPARENCY_SUPPORT}")
message(STATUS "   Spell-checker support       : ${CMAKE_STATUS_SPELLCHECKER_SUPPORT}")
message(STATUS "   IPC support                 : ${CMAKE_STATUS_IPC_SUPPORT}")
message(STATUS "   Audio backends              : ${CMAKE_STATUS_AUDIO_BACKENDS}")
message(STATUS "Cryptography:")
message(STATUS "   Crypt support and engines   : ${CMAKE_STATUS_CRYPT_SUPPORT}")
message(STATUS "   OpenSSL support             : ${CMAKE_STATUS_SSL_SUPPORT}")
message(STATUS "Scripting:")
message(STATUS "   Perl support                : ${CMAKE_STATUS_PERL_SUPPORT}")
message(STATUS "   Python support              : ${CMAKE_STATUS_PYTHON_SUPPORT}")
message(STATUS "DCC Features:")
message(STATUS "   GSM support                 : ${CMAKE_STATUS_GSM_SUPPORT}")
#message(STATUS "   DCC Canvas Support          : ${CMAKE_STATUS_DCC_CANVAS_SUPPORT}")
message(STATUS "   DCC voice support           : ${CMAKE_STATUS_DCC_VOICE_SUPPORT}")
message(STATUS "   DCC video support           : ${CMAKE_STATUS_DCC_VIDEO_SUPPORT}")
message(STATUS "   Ogg/Theora support          : ${CMAKE_STATUS_OGG_THEORA_SUPPORT}")
message(STATUS "Documentation:")
message(STATUS "   gettext support             : ${CMAKE_STATUS_GETTEXT_SUPPORT}${GETTEXT_EXTRA_STATUS}")
message(STATUS "   gettext messages tidying    : ${CMAKE_STATUS_MESSAGE_TIDY}")
#message(STATUS "   Generate int. help files    : ${CMAKE_STATUS_GEN_USERDOC}")
message(STATUS "   Doxygen support             : ${CMAKE_STATUS_DOXYGEN_SUPPORT}")
message(STATUS " ")
message(STATUS "Build date                     : ${CMAKE_KVIRC_BUILD_DATE}")
message(STATUS "Build version                  : ${CMAKE_KVIRC_VERSION_RELEASE}")
message(STATUS "Build codename                 : ${CMAKE_KVIRC_VERSION_CODENAME}")
message(STATUS "Build revision                 : ${CMAKE_KVIRC_BUILD_REVISION}")
message(STATUS "Sources date                   : ${CMAKE_KVIRC_SOURCES_DATE}")

message(STATUS " ")
message(STATUS "I'm ready to build KVIrc for you: please type ${CMAKE_MAKE_PROGRAM} now...")
message(STATUS " ")
