#!/bin/sh
#
#   A script for retrieving the latest KVIrc build configuration
#   Mainly used for building plugins out of the source tree
#   The idea is "stolen" from the gtk-config and xmms-config scripts :)
#
#
#   09-04-2000 Szymon Stefanek (Initial release)
#   31-01-2002 Szymon Stefanek (Revised for KVIrc 3.0.0)
#   27-07-2002 Szymon Stefanek (Revised again for KVIrc 3.0.0)
#   01-06-2007 Szymon Stefanek (Revised for SST)
#   ??-10-2007 Szymon Stefanek (Revised for Humor)
#   14-04-2008 CtrlAltCa (adapted to cmake branch)
#
#   This program is FREE software. You can redistribute it and/or
#   modify it under the terms of the GNU General Public License
#   as published by the Free Software Foundation; either version 2
#   of the License, or (at your option) any later version.
#
#   This program is distributed in the HOPE that it will be USEFUL,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#   See the GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program. If not, write to the Free Software Foundation,
#   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#

set -e

print_syntax()
{
	echo "@PACKAGE@@VERSION_MAJOR@-config (KVIrc @VERSION_RELEASE@)"
	echo "	A script for retrieving the latest KVIrc build configuration"
	echo ""
	echo "Syntax : @PACKAGE@@VERSION_MAJOR@-config [OPTIONS]"
	echo "  options:"
	echo "    --version         : Version of the KVIrc toolkit."
	echo "    --prefix          : The prefix where KVIrc was installed."
	echo "    --lib-suffix      : The suffix (if any) for the library directory where KVIrc was installed."
	echo "    --revision        : The KVIrc revision number."
	echo "    --date            : The KVIrc build date."
	echo "    --type            : The KVIrc build type (Debug, Release, ..)."
	exit 0
}

if test $# -eq 0; then
	print_syntax 1 1>&2
fi

KVIRC_STUFF_TO_ECHO=""

while test $# -gt 0; do
	case "$1" in
		-*=*) optarg=`echo "$1" | sed 's/[-_a-zA-Z0-9]*=//'` ;;
		*) optarg= ;;
	esac

	case $1 in
		--version)
		KVIRC_STUFF_TO_ECHO="$KVIRC_STUFF_TO_ECHO @VERSION_RELEASE@"
		;;
		--prefix)
		KVIRC_STUFF_TO_ECHO="$KVIRC_STUFF_TO_ECHO @CMAKE_INSTALL_PREFIX@"
		;;
		--lib-suffix)
		KVIRC_STUFF_TO_ECHO="$KVIRC_STUFF_TO_ECHO @LIB_SUFFIX@"
		;;
		--revision)
		KVIRC_STUFF_TO_ECHO="$KVIRC_STUFF_TO_ECHO @CMAKE_KVIRC_BUILD_REVISION@"
		;;
		--date)
		KVIRC_STUFF_TO_ECHO="$KVIRC_STUFF_TO_ECHO @CMAKE_KVIRC_BUILD_DATE@"
		;;
		--type)
		KVIRC_STUFF_TO_ECHO="$KVIRC_STUFF_TO_ECHO @CMAKE_BUILD_TYPE@"
		;;
		*)
		print_syntax 1 1>&2
		;;
	esac
	shift
done

if test -n "$KVIRC_STUFF_TO_ECHO"; then
	echo -n $KVIRC_STUFF_TO_ECHO
fi
