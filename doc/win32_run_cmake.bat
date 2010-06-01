echo OFF

REM =======================================================================
REM Windows cmake launch helper script.
REM See INSTALL-Win32.txt for informations about this file.
REM =======================================================================

REM =======================================================================
REM INSTALLATION PATHS
REM =======================================================================

set QTPATH=C:\Devel\Qt\2010.02.1

set MINGWPATH=C:\Devel\Qt\2010.02.1\mingw

set CMAKEPATH=C:\Programmi\CMake 2.8

set PYTHONPATH=C:\Programmi\Python26

set PERLPATH=C:\Programmi\Perl

set SUBVERSIONPATH=C:\Programmi\Subversion

set ZLIBPATH=C:\Programmi\GnuWin32

set OPENSSLPATH=C:\Devel\OpenSSL\openssl-1.0.0

set GETTEXTPATH=C:\Programmi\GnuWin32

set SEDPATH=C:\Programmi\GnuWin32

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


REM The windows command prompt is hopeless...

cmake.exe -G "MinGW Makefiles" ^
	-DZLIB_LIBRARY=%ZLIBPATH%\lib\zlib.lib ^
	-DZLIB_INCLUDE_DIR=%ZLIBPATH%\include\ ^
	-DOPENSSL_LIBRARIES=%OPENSSLPATH%\bin\libssl.dll.a ^
	-DOPENSSL_INCLUDE_DIR=%OPENSSLPATH%\include ^
	..
