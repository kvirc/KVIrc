@echo OFF

REM =======================================================================
REM Windows cmake launch helper script.
REM See INSTALL-Win32.txt for more information about this file.
REM =======================================================================

REM =======================================================================
REM INSTALLATION PATHS
REM =======================================================================

set QTPATH=C:\Qt\2010.05

set MINGWPATH=C:\Qt\2010.05\mingw

set CMAKEPATH=C:\Program Files\CMake 2.8

set PYTHONPATH=C:\Python26

set PERLPATH=C:\Perl

set SUBVERSIONPATH=C:\Program Files\Subversion

set ZLIBPATH=C:\Program Files\GnuWin32

set OPENSSLPATH=C:\OpenSSL

set GETTEXTPATH=C:\Program Files\GnuWin32

set SEDPATH=C:\Program Files\GnuWin32


REM =======================================================================
REM REAL WORKS BEGIN HERE
REM =======================================================================

set PATH=%QTPATH%\qt\bin;%QTPATH%\bin
set PATH=%PATH%;%MINGWPATH%\bin
set PATH=%PATH%;%CMAKEPATH%\bin
set PATH=%PATH%;%CMAKEPATH%\bin
set PATH=%PATH%;%PERLPATH%\bin;%PERLPATH%\site\bin
set PATH=%PATH%;%PYTHONPATH%
set PATH=%PATH%;%SUBVERSIONPATH%\bin
set PATH=%PATH%;%GETTEXTPATH%\bin
set PATH=%PATH%;%SEDPATH%\bin
set PATH=%PATH%;C:\Windows\System32

SET CMAKE_INCLUDE_PATH=%CMAKE_INCLUDE_DIR%;%OPENSSLPATH%\include;%ZLIBPATH%\include
SET CMAKE_LIBRARY_PATH=%CMAKE_LIB_DIR%;%OPENSSLPATH%\lib;%OPENSSLPATH%\lib\mingw;%ZLIBPATH%\lib

REM The windows command prompt is hopeless...

cmake.exe -G "MinGW Makefiles" ..
