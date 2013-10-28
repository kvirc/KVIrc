###############################################################################
##
## KVIrc CMake global configuration file.
##
###############################################################################

###############################################################################
# GLOBAL DEFS
###############################################################################

# Name of the project
project(kvirc)

# Set conventional loops
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

# Minimum cmake version
cmake_minimum_required(VERSION 2.8.6)

if(POLICY CMP0017)
	cmake_policy(SET CMP0017 NEW)
endif()

# Check modules first in local dir
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

# Some vars
set(PACKAGE "kvirc")
set(NICENAME "KVIrc")
set(VERSION_MAJOR "4")
set(VERSION_BRANCH "4.3")
set(VERSION_RELEASE "4.3.1")
set(VERSION_CODENAME "Aria")
# REMINDER: "Quasar" is a new candidate.

string(REGEX MATCH "[0-9]+$" VERSION_MINOR ${VERSION_BRANCH})

set(CMAKE_KVIRC_VERSION_MAJOR ${VERSION_MAJOR})
set(CMAKE_KVIRC_VERSION_BRANCH ${VERSION_BRANCH})
set(CMAKE_KVIRC_VERSION_RELEASE ${VERSION_RELEASE})
set(CMAKE_KVIRC_VERSION_CODENAME ${VERSION_CODENAME})
set(CMAKE_KVIRC_BUILD_COMMAND ${CMAKE_COMMAND})
set(CMAKE_KVIRC_BUILD_FLAGS)
set(CMAKE_KVIRC_BUILD_SYSTEM ${CMAKE_SYSTEM})
set(CMAKE_KVIRC_BUILD_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})
set(CMAKE_KVIRC_BUILD_SYSTEM_VERSION ${CMAKE_SYSTEM_VERSION})
set(CMAKE_KVIRC_BUILD_CPU ${CMAKE_SYSTEM_PROCESSOR})
set(CMAKE_KVIRC_BUILD_COMPILER ${CMAKE_CXX_COMPILER})
set(CMAKE_KVIRC_BUILD_COMPILER_FLAGS ${CMAKE_CXX_FLAGS})

# Suffix for Linux
set(LIB_SUFFIX
	CACHE STRING "Define suffix of directory name (32/64)"
)

# Get revision number from SVN
set(MANUAL_REVISION
	CACHE STRING "Manually set the revision number"
)



string(COMPARE EQUAL "${MANUAL_REVISION}" "" MANUAL_REVISION_UNSET)
if(MANUAL_REVISION_UNSET)
	# No manually forced revision number, gotta guess one now
	if(EXISTS "${CMAKE_SOURCE_DIR}/.svnrevision")
		# We're in a release package created using makedist.sh, so we should include a .svnrevision file
		file(READ "${CMAKE_SOURCE_DIR}/.svnrevision" DISTSVNREVISION)
		string(STRIP "${DISTSVNREVISION}" CMAKE_KVIRC_BUILD_REVISION)
		message(STATUS "SVN revision is ${CMAKE_KVIRC_BUILD_REVISION}")
	else()
		find_package(Subversion)
		if(Subversion_FOUND)
			message(STATUS "Found Subversion ${Subversion_VERSION_SVN}")
			
			# Subversion_WC_INFO is buggy and will croak if ran on a directory that is not checked out from svn

			# SVN info does not like .. as paths, so be sure that we use a full path
			get_filename_component(ABS_FOLDER ${CMAKE_SOURCE_DIR} ABSOLUTE)
			execute_process(
					COMMAND ${Subversion_SVN_EXECUTABLE} info ${ABS_FOLDER}
					RESULT_VARIABLE INFO_RESULT
					OUTPUT_VARIABLE IGNORED
					ERROR_VARIABLE IGNORED
				)
			if(INFO_RESULT EQUAL 0)
				Subversion_WC_INFO(${PROJECT_SOURCE_DIR} KVIrc) 
				if(DEFINED KVIrc_WC_REVISION)
					set(CMAKE_KVIRC_BUILD_REVISION ${KVIrc_WC_REVISION})
					message(STATUS "Current revision is ${KVIrc_WC_REVISION}")
				else()
					message(STATUS "No revision number found (no clue why though).")
				endif()
			else()
				message(STATUS "No revision number found (not a subversion repository).")
			endif()
		else()
			message(STATUS "No revision number found (subversion not available).")
		endif()
	endif()

else()
	# Manually forced revision number
	set(CMAKE_KVIRC_BUILD_REVISION ${MANUAL_REVISION})
	message(STATUS "Current revision has been manually set to ${MANUAL_REVISION}")
endif()


# Simplify eventual mixed revisions and strange tweaks with letters in it
string(REGEX REPLACE ":[0-9a-zA-Z]+" "" CMAKE_KVIRC_BUILD_REVISION "${CMAKE_KVIRC_BUILD_REVISION}")
string(REGEX REPLACE "[^0-9]+" "" CMAKE_KVIRC_BUILD_REVISION "${CMAKE_KVIRC_BUILD_REVISION}")


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
try_run(TEST_DID_RUN TEST_DID_COMPILE "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/datetime.cpp" OUTPUT_VARIABLE TEST_BUILD_DATE)
if(TEST_DID_COMPILE)
	string(REGEX MATCH "[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9] [0-9][0-9]:[0-9][0-9]:[0-9][0-9] [A-Z][A-Z][A-Z]" CMAKE_KVIRC_BUILD_DATE "${TEST_BUILD_DATE}")
else()
	message(STATUS " Test datetime program failed to compile:")
	message(STATUS " ----------------------------------------")
	message(STATUS "${TEST_BUILD_DATE}")
	message(STATUS " ----------------------------------------")
	set(CMAKE_KVIRC_BUILD_DATE "ERROR ")
endif()

# Set var to link with, for later use
set(LIBS)
set(ADDITIONAL_LINK_FLAGS)

# Install path
if(UNIX)
	if(APPLE)
		# MacOS X
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
#
# Release type
#
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
	if(WANT_DEBUG)
		set(WANT_STRIP OFF CACHE BOOL "Strip binaries (discard symbols from object files)" FORCE)
		set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
		set(CMAKE_STATUS_DEBUG_SUPPORT "Yes")
		if(CMAKE_COMPILER_IS_GNUCXX)
			# force gdb format of debug symbols
			set(CMAKE_CXX_FLAGS "-O0 -ggdb -pipe --exec-charset=UTF-8 --input-charset=UTF-8 --no-implement-inlines --unit-at-a-time --fast-math")
			set(CMAKE_C_FLAGS "-O0 -ggdb -pipe --exec-charset=UTF-8 --input-charset=UTF-8 --no-implement-inlines --unit-at-a-time --fast-math")
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
				# this is the equivalent of -O0 -g according to MSDN, /Zc:wchar_t- is default for QT 4.x but not 5; reevaluate then.
				set(CMAKE_CXX_FLAGS "/Od /Zi /Zc:wchar_t-")
				set(CMAKE_C_FLAGS "/Od /Zi /Zc:wchar_t-")
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
				set(CMAKE_BUILD_TYPE Release-stripped CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
			else()
				set(CMAKE_STATUS_DEBUG_SUPPORT "No (Release)")
				set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
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
				# /Ox is -O3, /Zc:wchar_t- is default for QT 4.x but not 5; reevaluate then.
				set(CMAKE_CXX_FLAGS "/Ox /Zc:wchar_t-")
				set(CMAKE_C_FLAGS "/Ox /Zc:wchar_t-")
			else()
				set(CMAKE_CXX_FLAGS "-O3")
				set(CMAKE_C_FLAGS "-O3")
			endif()
		endif()
	endif()
endif()

###############################################################################
#
# Verbose Makefile
#
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
#
# Version compatibility switch: appends VERSION_MAJOR to kvirc and kvilib
#
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
#
# Module install path: kvirc needs to know where they are installed
#
###############################################################################

if(UNIX)
	if(APPLE)
		set(CMAKE_KVIRC_RESOURCES_DIR "${CMAKE_INSTALL_PREFIX}/Contents/Resources/")
	else()
		# Assume linux
		set(CMAKE_KVIRC_RESOURCES_DIR "${CMAKE_INSTALL_PREFIX}/share/kvirc/${VERSION_BRANCH}/")
		set(CMAKE_KVIRC_MODULES_DIR "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/kvirc/${VERSION_BRANCH}/modules")
		list(APPEND CMAKE_KVIRC_BUILD_FLAGS LIB_SUFFIX=${LIB_SUFFIX})
	endif()
elseif(WIN32)
	set(CMAKE_KVIRC_RESOURCES_DIR "${CMAKE_INSTALL_PREFIX}/")
	set(CMAKE_KVIRC_MODULES_DIR "${CMAKE_INSTALL_PREFIX}/modules/")
endif()

###############################################################################
#
# System checks
#
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
#
# Library and tool checks, in order. First the required ones
# in order of dependancy, then the optional ones.
#
###############################################################################

# ingore sigalarm
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
# Check for Qt{4,5}
############################################################################

option(WANT_QT4 "Whether to prefer Qt4 over Qt5" OFF)

if(WANT_QT4)
	find_package(Qt4 4.6.0)
	if(NOT Qt4_FOUND)
		message(FATAL_ERROR "The use of Qt4 has been forced, but no Qt4 found on the system (WANT_QT4=ON)")
		set(CMAKE_STATUS_QT_VERSION "None")
	else()
		set(CMAKE_STATUS_QT_VERSION "${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}")
	endif()
else()
	find_package(Qt5Widgets)
	if(Qt5Widgets_FOUND)
		set(CMAKE_STATUS_QT_VERSION "${Qt5Widgets_VERSION_STRING}")
		set(qt5_kvirc_modules)
	else()
		find_package(Qt4 4.6.0)
		if(Qt4_FOUND)
			set(CMAKE_STATUS_QT_VERSION "${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}")
			set(WANT_QT4 ON CACHE BOOL "Whether to prefer Qt4 over Qt5" FORCE)
		else()
			message(FATAL_ERROR "No Qt4 or Qt5 found!")
		endif()
	endif()
endif()

# automoc is used by both qt4 and qt5
set(CMAKE_AUTOMOC TRUE)

# Shared options
if(APPLE OR WIN32)
	# No Qt-DBus on Mac and Windows
	option(WANT_QTDBUS "Whether to compile Qt-DBus support" OFF)
else()
	option(WANT_QTDBUS "Whether to compile Qt-DBus support" ON)
endif()

option(WANT_QTWEBKIT "Whether to compile Qt-WebKit support" ON)
option(WANT_QTSVG "Whether to compile Qt-Svg support" ON)

# Qt4 stuff
if(Qt4_FOUND)
	set(QT_USE_QTSQL true)
	set(QT_USE_QTXML true)
	set(QT_USE_QTNETWORK true)

	# qtmain for non-debug MSVC (/subsystem:windows), MinGW can live without it.
	if(MSVC AND NOT WANT_DEBUG)
		set(QT_USE_QTMAIN true)
	endif()

	# Qt-DBus support	
	if(WANT_QTDBUS)
		if(QT_QTDBUS_FOUND)
			set(QT_USE_QTDBUS true)
			set(CMAKE_STATUS_QTDBUS_SUPPORT "Yes")
			set(COMPILE_DBUS_SUPPORT 1)
		else()
			set(CMAKE_STATUS_QTDBUS_SUPPORT "Not available")
		endif()
	else()
		set(CMAKE_STATUS_QTDBUS_SUPPORT "No")
	endif()

	# Qt-WebKit support
	if(WANT_QTWEBKIT)
		if(QT_QTWEBKIT_FOUND)
			set(QT_USE_QTWEBKIT true)
			set(CMAKE_STATUS_QTWEBKIT_SUPPORT "Yes")
			set(COMPILE_WEBKIT_SUPPORT 1)
		else()
			set(CMAKE_STATUS_QTWEBKIT_SUPPORT "Not available")
		endif()
	else()
		set(CMAKE_STATUS_QTWEBKIT_SUPPORT "No")
	endif()
	# Qt-Svg support
	if(WANT_QTSVG)
		if(QT_QTSVG_FOUND)
			set(QT_USE_QTSVG true)
			set(CMAKE_STATUS_QTSVG_SUPPORT "Yes")
			set(COMPILE_SVG_SUPPORT 1)
		else()
			set(CMAKE_STATUS_QTSVG_SUPPORT "Not available")
		endif()
	else()
		set(CMAKE_STATUS_QTSVG_SUPPORT "No")
	endif()

	include(${QT_USE_FILE})
	include_directories(${QT_INCLUDES})
	list(APPEND LIBS ${QT_LIBRARIES})
endif()

if(Qt5Widgets_FOUND)
	include_directories(${Qt5Widgets_INCLUDE_DIRS})
	list(APPEND qt5_kvirc_modules Widgets)

	# qtmain for non-debug MSVC (/subsystem:windows), MinGW can live without it.
	if(MSVC AND NOT WANT_DEBUG)
		set(QT_USE_QTMAIN true)
	endif()

	# QtSql
	find_package(Qt5Sql)
	if(Qt5Sql_FOUND)
	    include_directories(${Qt5Sql_INCLUDE_DIRS})
	    list(APPEND qt5_kvirc_modules Sql)
	endif()

	# QtXml
	find_package(Qt5Xml)
	if(Qt5Xml_FOUND)
	    include_directories(${Qt5Xml_INCLUDE_DIRS})
	    list(APPEND qt5_kvirc_modules Xml)
	endif()

	# QtNetwork
	find_package(Qt5Network)
	if(Qt5Network_FOUND)
	    include_directories(${Qt5Network_INCLUDE_DIRS})
	    list(APPEND qt5_kvirc_modules Network)
	endif()

	# QtPrintSupport
	find_package(Qt5PrintSupport)
	if(Qt5PrintSupport_FOUND)
	    include_directories(${Qt5PrintSupport_INCLUDE_DIRS})
	    list(APPEND qt5_kvirc_modules PrintSupport)
	endif()

	# QtMultimedia
	find_package(Qt5Multimedia)
	if(Qt5Multimedia_FOUND)
	    include_directories(${Qt5Multimedia_INCLUDE_DIRS})
	    list(APPEND qt5_kvirc_modules Multimedia)
	endif()

	# Qt-DBus support	
	if(WANT_QTDBUS)
	    find_package(Qt5DBus)
	    if(Qt5DBus_FOUND)
			list(APPEND qt5_kvirc_modules DBus)
			include_directories(${Qt5DBus_INCLUDE_DIRS})
			set(COMPILE_DBUS_SUPPORT 1)
		endif()
	endif()

	# Qt-WebKit support
	if(WANT_QTWEBKIT)
	    find_package(Qt5WebKitWidgets)
	    if(Qt5WebKitWidgets_FOUND)
			list(APPEND qt5_kvirc_modules WebKitWidgets)
			include_directories(${Qt5WebKitWidgets_INCLUDE_DIRS})
			set(COMPILE_WEBKIT_SUPPORT 1)
		endif()
	endif()
	# Qt-Svg support
	if(WANT_QTSVG)
	    find_package(Qt5Svg)
	    if(Qt5Svg_FOUND)
		list(APPEND qt5_kvirc_modules Svg)
		include_directories(${Qt5Svg_INCLUDE_DIRS})
		set(COMPILE_SVG_SUPPORT 1)
		endif()
	endif()
endif()

############################################################################
# Check for KDE4
############################################################################

option(WANT_KDE4 "Whether to use KDE4 support" ON)
if(WANT_KDE4 AND WANT_QT4)
	find_package(Qt4 4.6.0)
	if(Qt4_FOUND)
		find_package(KDE4)
		if(KDE4_FOUND)
			include_directories(${KDE4_INCLUDES})
			add_definitions(${KDE4_DEFINITIONS})
			set(COMPILE_KDE_SUPPORT 1)
			list(APPEND LIBS ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBRARY} ${KDE4_KPARTS_LIBRARY} ${KDE4_KFILE_LIBRARY} ${KDE4_KIO_LIBRARY} ${KDE4_SOLID_LIBRARY})
			set(CMAKE_STATUS_KDE4_SUPPORT "Yes")
			# The FindKDE4Internal cmake module adds -Wl,--no-undefined
			# to the CMAKE_MODULE_LINKER_FLAGS .. which is not good for us
			string(REPLACE "-Wl,--no-undefined" "" CMAKE_MODULE_LINKER_FLAGS ${CMAKE_MODULE_LINKER_FLAGS})
			# and for kvilib...
			string(REPLACE "-Wl,--no-undefined" "" CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS})
		else()
			set(CMAKE_STATUS_KDE4_SUPPORT "Not available")
		endif()
	endif()
elseif(NOT WANT_QT4)
	# Must be Qt4
	set(CMAKE_STATUS_KDE4_SUPPORT "Unsupported with Qt5")
else()
	set(CMAKE_STATUS_KDE4_SUPPORT "No")
endif()

# Check for X11
if(UNIX)
	if(APPLE)
		set(CMAKE_STATUS_X11_SUPPORT "Not used")
	else()
		# Assume linux
		find_package(X11)
		if(X11_FOUND)
			set(COMPILE_X11_SUPPORT 1)
			if(X11_Xscreensaver_FOUND)
				set(COMPILE_XSS_SUPPORT 1)
				set(CMAKE_STATUS_X11_SUPPORT "Yes, with scrnsaverproto")
				include_directories(${X11_Xscreensaver_INCLUDE_PATH})
				list(APPEND LIBS ${X11_Xscreensaver_LIB})
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
	find_package(Phonon)
	if(PHONON_FOUND AND NOT Qt5Multimedia_FOUND)
		set(COMPILE_PHONON_SUPPORT 1)
		list(APPEND LIBS ${PHONON_LIBRARY})
		list(APPEND CMAKE_STATUS_AUDIO_BACKENDS "Phonon")
	elseif(Qt5Multimedia_FOUND)
		list(APPEND CMAKE_STATUS_AUDIO_BACKENDS "Qt5Multimedia")
	endif()
endif()

# esd
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
option(WANT_AUDIOFILE "Whether to compile Audiofile support" ON)

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
# Check for MacOS X universal binary
############################################################################

if(APPLE)
	option(WANT_UNIVERSAL_BINARY "Whether to compile an Universal Binary on MacOSX" ON)
	if(WANT_UNIVERSAL_BINARY)
		set(CMAKE_OSX_ARCHITECTURES x86_64)
		set(CMAKE_STATUS_BUILD_UNIVERSAL_BINARY "User enabled")
		#FIXME: This should be handled with CMAKE_OSX_SYSROOT (TOM)
		set(CMAKE_SHARED_LINKER_FLAGS "-mmacosx-version-min=10.6")
		set(CMAKE_EXE_LINKER_FLAGS "-mmacosx-version-min=10.6")
	else()
		set(CMAKE_STATUS_BUILD_UNIVERSAL_BINARY "No")
	endif()
else()
	set(CMAKE_STATUS_BUILD_UNIVERSAL_BINARY "Not available")
endif()

############################################################################
# OpenSSL support (Secure Sockets, Certificates, Digests etc...)
############################################################################

option(WANT_OPENSSL "Whether to compile OpenSSL support" ON)

if(WANT_OPENSSL)
	find_package(OpenSSL)
	if(OPENSSL_FOUND)
		if(WIN32)
			# On windows/mingw we need libeay32 too...
			find_library(LIB_EAY NAMES libeay32MD eay libeay libeay32
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

option(WANT_CRYPT "Compile Cryptographic Support and Engines" ON)
if(WANT_CRYPT)
	set(COMPILE_CRYPT_SUPPORT 1)
	set(CMAKE_STATUS_CRYPT_SUPPORT "Yes")
else()
	set(CMAKE_STATUS_CRYPT_SUPPORT "No")
endif()

############################################################################
# IPv6 support
############################################################################

option(WANT_IPV6 "Compile IPv6 Support" ON)
if(WANT_IPV6)
	set(COMPILE_IPV6_SUPPORT 1)
	set(CMAKE_STATUS_IPV6_SUPPORT "Yes")
else()
	set(CMAKE_STATUS_IPV6_SUPPORT "No")
endif()

############################################################################
# Do we want pseudo-transparency ?
############################################################################

option(WANT_TRANSPARENCY "Compile Transparency Support" ON)
if(WANT_TRANSPARENCY)
	set(COMPILE_PSEUDO_TRANSPARENCY 1)
	set(CMAKE_STATUS_TRANSPARENCY_SUPPORT "Yes")
else()
	set(CMAKE_STATUS_TRANSPARENCY_SUPPORT "No")
endif()

# Check for Perl support
# (note: needed for user documentation generation)
option(WANT_PERL "Compile Perl Support" ON)

#perl is needed for documentation, too
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
option(WANT_PYTHON "Compile Python Support" ON)
if(WANT_PYTHON)
	# this should ensure that cmake prefers python 2.7 over python 3
	# it's not neede from cmake 2.8.8 onwards
	set(Python_ADDITIONAL_VERSIONS 2.7)
	find_package(PythonLibs)
	if(PYTHONLIBS_FOUND)
		set(COMPILE_PYTHON_SUPPORT 1)
		set(CMAKE_STATUS_PYTHON_SUPPORT "Yes")

		if(WANT_DEBUG)
			list(APPEND LIBS ${PYTHON_DEBUG_LIBRARIES})
		else()
			list(APPEND LIBS ${PYTHON_LIBRARIES})
		endif()

		if(DEFINED PYTHON_INCLUDE_DIRS)
			include_directories(${PYTHON_INCLUDE_DIRS})
		else()
			include_directories(${PYTHON_INCLUDE_PATH})
		endif()
	else()
		set(CMAKE_STATUS_PYTHON_SUPPORT "No, not found.")
	endif()
else()
	set(CMAKE_STATUS_PYTHON_SUPPORT "No")
endif()

############################################################################
# IPC (inter-process communication) support
############################################################################
option(WANT_IPC "Compile Inter-Process Communication Support" ON)
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
# what systems lacks this!?
if(HAVE_INET_NTOA_EXISTS)
	set(HAVE_INET_NTOA 1)
endif()

CHECK_FUNCTION_EXISTS("inet_aton" HAVE_INET_ATON_EXISTS)
# what systems lacks this!?
if(HAVE_INET_ATON_EXISTS)
	set(HAVE_INET_ATON 1)
endif()

# Check GET_INTERFACE_ADDRESS support
find_path(GET_INTERFACE_ADDRESS_INCLUDE_DIR net/if.h)
if(GET_INTERFACE_ADDRESS_INCLUDE_DIR)
	set(COMPILE_GET_INTERFACE_ADDRESS 1)
endif()

############################################################################
# SetEnv/PutEnv support
############################################################################
CHECK_FUNCTION_EXISTS("setenv" HAVE_SETENV_EXISTS)
# what systems lacks this!?
if(HAVE_SETENV_EXISTS)
	set(HAVE_SETENV 1)
endif()

CHECK_FUNCTION_EXISTS("putenv" HAVE_PUTENV_EXISTS)
# what systems lacks this!?
if(HAVE_PUTENV_EXISTS)
	set(HAVE_PUTENV 1)
endif()

############################################################################
# GSM support
############################################################################
option(WANT_GSM "Compile GSM Support" ON)
if(WANT_GSM)
	set(COMPILE_USE_GSM 1)
	set(CMAKE_STATUS_GSM_SUPPORT "Yes")
else()
	set(CMAKE_STATUS_GSM_SUPPORT "No")
endif()

############################################################################
# DCC canvas support
############################################################################

option(WANT_DCC_CANVAS "Compile DCC Canvas Support" OFF)
if(WANT_DCC_CANVAS)
	set(CMAKE_STATUS_DCC_CANVAS_SUPPORT "Yes")
	set(COMPILE_DCC_CANVAS 1)
else()
	set(CMAKE_STATUS_DCC_CANVAS_SUPPORT "No")
endif()

############################################################################
# DCC voice support
############################################################################
CHECK_INCLUDE_FILE(linux/soundcard.h CMAKE_HAVE_LINUX_SOUNDCARD_H)
CHECK_INCLUDE_FILE(sys/soundcard.h CMAKE_HAVE_SYS_SOUNDCARD_H)
CHECK_INCLUDE_FILE(soundcard.h CMAKE_HAVE_SOUNDCARD_H)

option(WANT_DCC_VOICE "Compile DCC Voice Support" ON)
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
# Dcc video support + extended webcam support (LibV4L2)
############################################################################
option(WANT_DCC_VIDEO "Compile DCC Video Support" OFF)

if(WANT_DCC_VIDEO AND Qt5Widgets_FOUND)
	# QtMultimediaWidgets
	find_package(Qt5MultimediaWidgets)
	if(Qt5MultimediaWidgets_FOUND)
	    include_directories(${Qt5MultimediaWidgets_INCLUDE_DIRS})
	    list(APPEND qt5_kvirc_modules MultimediaWidgets)
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
option(WANT_OGG_THEORA "Compile Ogg/Vorbis+Theora Support" OFF)

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
option(WANT_MEMORY_PROFILE "Compile Memory Profiling Support" OFF)
if(WANT_MEMORY_PROFILE)
	set(COMPILE_MEMORY_PROFILE 1)
	set(CMAKE_STATUS_MEMORY_PROFILE_SUPPORT "User enabled")
else()
	set(CMAKE_STATUS_MEMORY_PROFILE_SUPPORT "No")
endif()


############################################################################
# Memory checks support
############################################################################
option(WANT_MEMORY_CHECKS "Compile Memory Checking Support" OFF)
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
	add_definitions(-D_WIN32_WINNT=0x501 -DWINVER=0x501)
endif()

if(MINGW)
	list(APPEND LIBS -lshlwapi)
	add_definitions(-DMINGW)
	set(ADDITIONAL_LINK_FLAGS "${ADDITIONAL_LINK_FLAGS} -Wl,--enable-auto-import,--enable-runtime-pseudo-reloc -fno-exceptions")
	#remove leading and trailing spaces (string strip)
	string(REGEX REPLACE "(^( )+|( )+$)" "" ADDITIONAL_LINK_FLAGS "${ADDITIONAL_LINK_FLAGS}")
endif()

if(MSVC)
	add_definitions(-D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES)
	add_definitions(-DMSVC)
endif()

if(${CMAKE_CXX_COMPILER_ID} MATCHES "SunPro")
	# SunPro breaks with non-C, and we can't set it here, so nag the user.
	# looks nasty, maybe there is a better way?
	if(NOT ($ENV{LANG} STREQUAL "C"))
  	message( FATAL_ERROR "Please set enviroment variable \"LANG\" to \"C\" and rerun CMake" )
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
# Some users wants it anyway, and we sometimes needs to check it
############################################################################

option(WANT_GTKSTYLE "Enable QGtkStyle" OFF)
if(WANT_GTKSTYLE)
	set(COMPILE_ENABLE_GTKSTYLE 1)
endif()

###############################################################################
#
# Translation (GetText) checks
#
###############################################################################
option(WANT_GETTEXT "Whether to use GetText to generate translations" ON)
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
		foreach(curFile ${FILES_TO_TIDY})
			add_custom_command(TARGET messages-tidy
			COMMENT "Tidying ${curFile}..."
			COMMAND ${SED_EXECUTABLE} -i -e "s|^#:.*/\\(src/.*\\)$|#: \\1|g" "${curFile}"
			VERBATIM
			DEPENDS ${curFile}
			)
		endforeach()
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


#Unused by now
#CMakeUnixFindMake.cmake
#FindCygwin.cmake
#CMakeFindXCode.cmake
#CheckLibraryExists.cmake

# Search for subdirectories; under osx, data _MUST_ be before src (to get Info.plist installed before the main executable's fixup_bundle step)
subdirs(data doc po scripts src)

#install additional resources for win32
if(WIN32)
	add_subdirectory(win32build)
endif()
###############################################################################
#
# The API documentation target
#
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
			COMMENT "generate the api documentation"
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
#
# The User documentation target (kvirc internal help)
#
###############################################################################

#currently WIP
#option(WANT_USERDOCS "Whether to generate user documentation (aka internal help)" ON)
#if(WANT_USERDOCS)
#	set(CMAKE_STATUS_GEN_USERDOC "Yes")
#else()
#	set(CMAKE_STATUS_GEN_USERDOC "User disabled")
#endif()

###############################################################################
#
# We want the "distclean" target
#
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

# System configuration is now splitted in 2 files
# kvi_sysconfig.h contains the "saveable" configuration: stuff that
# may be preserved between multiple cmake runs (and thus may save compilation time)
# kvi_sysbuildinfo.h contains the configuration that changes at every
# cmake run and must be recompiled every time. Extra care is taken
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
# ADD LINK FLAGS TO LIBS: THIS HAVE TO BE DONE AT LAST
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
		# Assume linux
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
		# Assume linux
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
		# Assume linux
		set(KVIRC_MOD_PATH ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/kvirc/${VERSION_BRANCH}/modules/)
	endif()
elseif(WIN32)
	set(KVIRC_MOD_PATH ${CMAKE_INSTALL_PREFIX}/modules/)
endif()

# Man path
if(UNIX)
	if(APPLE)
		# does MacOS X use man pages? <= yes!
	else()
		# Assume linux
		set(KVIRC_MAN_PATH ${CMAKE_INSTALL_PREFIX}/${MANDIR_PREFIX}/)
	endif()
else()
		set(KVIRC_MAN_PATH "Not available")
endif()

message(STATUS " ")
message(STATUS "Ok, this is the configuration I've worked out:")
if(${CONFIG_IS_UPTODATE})
	message(STATUS "(I found an old config that was exactly the same and kept it to save compilation time)")
endif()
message(STATUS " ")
message(STATUS "Paths:")
message(STATUS "   Binary Path                 : ${KVIRC_BIN_PATH}")
message(STATUS "   Library Path                : ${KVIRC_LIB_PATH}")
message(STATUS "   Modules Path                : ${KVIRC_MOD_PATH}")
message(STATUS "   Man Path                    : ${KVIRC_MAN_PATH}")
message(STATUS "Compilation:")
message(STATUS "   Debug                       : ${CMAKE_STATUS_DEBUG_SUPPORT}")
message(STATUS "   Verbose Output              : ${CMAKE_STATUS_VERBOSE_SUPPORT}")
message(STATUS "   Coexistence Support         : ${CMAKE_STATUS_COEXISTENCE_VER}")
message(STATUS "   Threading Support           : ${CMAKE_STATUS_THREADS_SUPPORT}")
message(STATUS "   Apple Universal Binary      : ${CMAKE_STATUS_BUILD_UNIVERSAL_BINARY}")
message(STATUS "   Memory Profile Support      : ${CMAKE_STATUS_MEMORY_PROFILE_SUPPORT}")
message(STATUS "   Memory Checks Support       : ${CMAKE_STATUS_MEMORY_CHECKS_SUPPORT}")
message(STATUS "Features:")
message(STATUS "   X11 Support                 : ${CMAKE_STATUS_X11_SUPPORT}")
message(STATUS "   Qt Version                  : ${CMAKE_STATUS_QT_VERSION}")
if(Qt5Widgets_FOUND)
    message(STATUS "   Qt Modules                  : ${qt5_kvirc_modules}")
else()
    message(STATUS "   Qt-DBus Support             : ${CMAKE_STATUS_QTDBUS_SUPPORT}")
    message(STATUS "   Qt-WebKit Support           : ${CMAKE_STATUS_QTWEBKIT_SUPPORT}")
    message(STATUS "   Qt-Svg Support              : ${CMAKE_STATUS_QTSVG_SUPPORT}")
endif()
message(STATUS "   KDE4 Support                : ${CMAKE_STATUS_KDE4_SUPPORT}")
message(STATUS "   IPv6 Support                : ${CMAKE_STATUS_IPV6_SUPPORT}")
message(STATUS "   Pseudo-Transparency Support : ${CMAKE_STATUS_TRANSPARENCY_SUPPORT}")
message(STATUS "   IPC Support                 : ${CMAKE_STATUS_IPC_SUPPORT}")
message(STATUS "   Audio Backends              : ${CMAKE_STATUS_AUDIO_BACKENDS}")
message(STATUS "Cryptography:")
message(STATUS "   Crypt Support And Engines   : ${CMAKE_STATUS_CRYPT_SUPPORT}")
message(STATUS "   OpenSSL Support             : ${CMAKE_STATUS_SSL_SUPPORT}")
message(STATUS "Scripting:")
message(STATUS "   Perl Support                : ${CMAKE_STATUS_PERL_SUPPORT}")
message(STATUS "   Python Support              : ${CMAKE_STATUS_PYTHON_SUPPORT}")
message(STATUS "DCC features:")
message(STATUS "   DCC Canvas Support          : ${CMAKE_STATUS_DCC_CANVAS_SUPPORT}")
message(STATUS "   DCC Voice Support           : ${CMAKE_STATUS_DCC_VOICE_SUPPORT}")
message(STATUS "   GSM Support                 : ${CMAKE_STATUS_GSM_SUPPORT}")
message(STATUS "   DCC Video Support           : ${CMAKE_STATUS_DCC_VIDEO_SUPPORT}")
message(STATUS "   Ogg/Theora Support          : ${CMAKE_STATUS_OGG_THEORA_SUPPORT}")
message(STATUS "Documentation:")
message(STATUS "   GetText Support             : ${CMAKE_STATUS_GETTEXT_SUPPORT}${GETTEXT_EXTRA_STATUS}")
message(STATUS "   GetText Messages Tidying    : ${CMAKE_STATUS_MESSAGE_TIDY}")
#message(STATUS "   Generate int. help files    : ${CMAKE_STATUS_GEN_USERDOC}")
message(STATUS "   Doxygen Support             : ${CMAKE_STATUS_DOXYGEN_SUPPORT}")
message(STATUS " ")
message(STATUS "Build Date                     : ${CMAKE_KVIRC_BUILD_DATE}")
message(STATUS " ")
if(MSVC)
	message(STATUS "I'm ready to build KVIrc for you: please type 'nmake' now...")
elseif(MINGW)
	message(STATUS "I'm ready to build KVIrc for you: please type 'mingw32-make' now...")
else()
	message(STATUS "I'm ready to build KVIrc for you: please type 'make' now...")
endif()

if(COMPILE_ENABLE_GTKSTYLE)
	message(STATUS "Warning: you forcibly enabled the GtkStyle engine, which is known to break KVIrc theming.")
endif()
message(STATUS " ")
