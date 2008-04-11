#!/bin/sh
#
#   A script for retrieving the latest HUMOR build configuration
#   Mainly used for building plugins out of the source tree
#   The idea is "stolen" from the gtk-config and xmms-config scripts :)
#
#   09-04-2000 Szymon Stefanek (Initial release)
#   31-01-2002 Szymon Stefanek (Revised for KVIrc 3.0.0)
#   27-07-2002 Szymon Stefanek (Revised again for KVIrc 3.0.0)
#   01-06-2007 Szymon Stefanek (Revised for SST)
#   ??-10-2007 Szymon Stefanek (Revised for Humor)
#
#   This program is FREE software. You can redistribute it and/or
#   modify it under the terms of the GNU General Public License
#   as published by the Free Software Foundation; either version 2
#   of the License, or (at your opinion) any later version.
#
#   This program is distributed in the HOPE that it will be USEFUL,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#   See the GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program. If not, write to the Free Software Foundation,
#   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#

set -e

print_syntax()
{
	echo "humor-config (HuMoR @HUMOR_CONFIG_VERSION@)"
	echo "	A script for retrieving the latest HuMoR build configuration"
	echo ""
	echo "Syntax : humor-config [OPTIONS]"
	echo "  options:"
	echo "    --version         : Version of the HuMoR toolkit."
	echo "    --prefix          : The prefix where the HuMoR toolkit was installed."
	echo "    --includedirs     : The include directories you should use to compile stuff with HuMoR."
	echo "    --includeflags    : The include directories above with -I prepended."
	echo "    --librarydirs     : The library directories you should use to compile stuff with HuMoR."
	echo "    --libraries       : The libraries you should use to compile stuff with HuMoR."
	echo "    --libraryflags    : The library directories above with -L prepended "
	echo "                        and the libraries above with -l prepended."
	echo "    --definitions     : The compiler definitions you should use to compile stuff with HuMoR."
	exit 0
}

if test $# -eq 0; then
	print_syntax 1 1>&2
fi

HUMOR_STUFF_TO_ECHO=""

while test $# -gt 0; do
	case "$1" in
		-*=*) optarg=`echo "$1" | sed 's/[-_a-zA-Z0-9]*=//'` ;;
		*) optarg= ;;
	esac

	case $1 in
		--version)
		HUMOR_STUFF_TO_ECHO="$HUMOR_STUFF_TO_ECHO @HUMOR_CONFIG_VERSION@"
		;;
		--prefix)
		HUMOR_STUFF_TO_ECHO="$HUMOR_STUFF_TO_ECHO @HUMOR_CONFIG_PREFIX@"
		;;
		--includedirs)
		HUMOR_STUFF_TO_ECHO="$HUMOR_STUFF_TO_ECHO @HUMOR_CONFIG_INCLUDEDIRS@"
		;;
		--includeflags)
		for d in @HUMOR_CONFIG_INCLUDEDIRS@; do
			HUMOR_STUFF_TO_ECHO="$HUMOR_STUFF_TO_ECHO -I$d"
		done
		;;
		--librarydirs)
		HUMOR_STUFF_TO_ECHO="$HUMOR_STUFF_TO_ECHO @HUMOR_CONFIG_LIBRARYDIRS@"
		;;
		--libraries)
		HUMOR_STUFF_TO_ECHO="$HUMOR_STUFF_TO_ECHO @HUMOR_CONFIG_LIBRARIES@"
		;;
		--libraryflags)
		for d in @HUMOR_CONFIG_LIBRARYDIRS@; do
			HUMOR_STUFF_TO_ECHO="$HUMOR_STUFF_TO_ECHO -L$d"
		done
		for d in @HUMOR_CONFIG_LIBRARIES@; do
			HUMOR_STUFF_TO_ECHO="$HUMOR_STUFF_TO_ECHO -l$d"
		done
		;;
		--definitions)
		HUMOR_STUFF_TO_ECHO="$HUMOR_STUFF_TO_ECHO @HUMOR_CONFIG_DEFINITIONS@"
		;;
		*)
		print_syntax 1 1>&2
		;;
	esac
	shift
done

if test -n "$HUMOR_STUFF_TO_ECHO"; then
	echo -n $HUMOR_STUFF_TO_ECHO
fi
