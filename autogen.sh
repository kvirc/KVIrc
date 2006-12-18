#! /bin/bash

###############################################################################
#
#	Run this to generate the Makefile templates, etc.
#
###############################################################################

THIS_PROGGIE_EXECUTABLE="./autogen.sh"
THIS_PROGGIE_NAME="KVIrc autogen.sh"
THIS_PROGGIE_VERSION="1.0.0"
PKG_NAME="kvirc"
DIE=0
GEN_FILES="configure.in"

USE_BUNDLED_LIBTOOL=0

# Handle arguments


while test "$#" -gt 0
do
	arg="$1"
	shift

	case $arg in
		--help)
			echo $THIS_PROGGIE_NAME $THIS_PROGGIE_VERSION
			echo "Usage:"
			echo $THIS_PROGGIE_EXECUTABLE "[options]"
			echo "Options:"
			echo "   --bundled-libtool   : use the bundled libtool instead"
			echo "                         of the system one"
			exit 0
	    ;;

		--version)
			echo $THIS_PROGGIE_NAME $THIS_PROGGIE_VERSION
			exit 0
	    ;;

		--bundled-libtool)
			USE_BUNDLED_LIBTOOL=1
		;;

		*)
			echo "Unrecognized option $arg"
			echo "See $THIS_PROGGIE_EXECUTABLE --help"
		    exit -1
		;;
	esac
done


set -e

function RUN_TOOL()
{
	if [ -z "$*" ]; then return; fi
	echo "... Running $*"
	$*
}


###############################################################################
#
#


echo "... Generating build files for $PKG_NAME"

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
	echo
	echo "**Error**: You must have GNU \`autoconf' installed."
	DIE=1
}

(test "$USE_BUNDLED_LIBTOOL" -eq 0) && {
	(libtool --version) < /dev/null > /dev/null 2>&1 || {
		echo "*Warning*: You don't have GNU \`libtool' installed."
		echo "*Warning*: Using the buindled libtool instead."
		USE_BUNDLED_LIBTOOL=1
	}
}

(automake --version) < /dev/null > /dev/null 2>&1 || {
	echo
	echo "**Error**: You must have GNU \`automake' installed."
	DIE=1
	NO_AUTOMAKE=yes
}

# if no automake, do not bother testing for aclocal
test -n "$NO_AUTOMAKE" || (aclocal --version) < /dev/null > /dev/null 2>&1 || {
	echo
	echo "**Error**: Missing \`aclocal'. The version of \`automake'"
	echo "installed does not appear recent enough."
	DIE=1
}

if test "$DIE" -eq 1; then
	exit 1
fi

case $CC in
xlc)
	am_opt=--include-deps;;
esac

for coin in $GEN_FILES
do
	dr=`dirname $coin`
	if test -f $dr/NO-AUTO-GEN; then
		echo "... skipping $dr -- flagged as no auto-gen"
	else
		echo "... processing \"$dr\""
		macrodirs=`sed -n -e 's,AM_ACLOCAL_INCLUDE(\(.*\)),\1,gp' < $coin`
		(
			cd $dr
			aclocalinclude="$ACLOCAL_FLAGS"
			for k in $macrodirs; do
				if test -d $k; then
					aclocalinclude="$aclocalinclude -I $k"
				fi
			done

			MACRO_FILES="admin/acinclude.m4.in admin/libtool.m4.in"

			cat $MACRO_FILES > acinclude.m4

			if test "$USE_BUNDLED_LIBTOOL" -eq 0; then
				echo "... Using system libtool"
				echo "...   Re-run with --bundled-libtool to use"
				echo "...   the libtool shipped with the KVIrc cvs"
				RUN_TOOL libtoolize --copy --force
			else
				echo "... Using bundled libtool"
				rm -f admin/ltmain.sh
				rm -f admin/config.guess
				rm -f admin/config.sub
				cp admin/libtool/ltmain.sh admin/ltmain.sh
				cp admin/libtool/config.guess admin/config.guess
				cp admin/libtool/config.sub admin/config.sub
			fi

			RUN_TOOL aclocal $aclocalinclude
			if grep "^AM_CONFIG_HEADER" $coin > /dev/null; then
				RUN_TOOL autoheader
			fi
			RUN_TOOL automake --add-missing --force-missing --copy $am_opt
			RUN_TOOL autoconf
			touch stamp-h.in
		)
	fi
done

echo "###"
echo "### Done. The next step is ./configure --help"
echo "### Take a look at the options (eventually read the INSTALL file)"
echo "### and run ./configure <your_options>"
echo "###"

#
###############################################################################
