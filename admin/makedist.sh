#/bin/bash

function usage()
{
	echo "Usage:"
	echo "      $0 [options] <source_tree_dir> <kvirc_version>"
	echo "Available options:"
	echo "   --gzip"
	echo "     Use gzip instead of bzip2"
	echo "   --help"
	echo "     Show this help"
}


SOURCETREEDIR=""
KVIRCVERSION=""
COMPRESSSWITCH="j"
OUTPUTEXTENSION="bz2"

for i in $*; do
	case $i in
		--gzip)
				COMPRESSSWITCH="z"
				OUTPUTEXTENSION="gz"
			;;
		--help)
				usage
				exit
			;;
		*)
			if test -z "$SOURCETREEDIR"; then
				SOURCETREEDIR=$i
			else
				KVIRCVERSION=$i
			fi
			;;
	esac
done

if [ -z "$SOURCETREEDIR" ]; then
	echo "ERROR: Source tree directory missing"
	usage
	exit
fi

if [ -z "$KVIRCVERSION" ]; then
	echo "ERROR: Kvirc version missing"
	usage
	exit
fi

if [ ! -d $SOURCETREEDIR ]; then
	echo "ERROR: The source tree parameter doesn't identify a directory"
	usage
	exit
fi

THISDIR=$(pwd)
TEMPDIR=/tmp
PKGSRCDIR=kvirc-"$KVIRCVERSION"
TEMPSRCDIR="$TEMPDIR/$PKGSRCDIR"
OUTPUTFILE="$THISDIR/kvirc-${KVIRCVERSION}.tar.${OUTPUTEXTENSION}"

if [ -d "$TEMPSRCDIR" ]; then
	echo "Removing stale target directory..."
	rm -rf "$TEMPSRCDIR"
fi

echo "Exporting svn dir into ${TEMPSRCDIR}..."
svn export "${SOURCETREEDIR}" "${TEMPSRCDIR}"

echo "Determining svn revision..."
cd "$SOURCETREEDIR"
# If we update svn here, we have a different revision than we exported above
#svn update
REVISION=$(svnversion -n .)

echo "Revision is $REVISION"
echo $REVISION > "$TEMPSRCDIR/.svnrevision"

if [ -f "$OUTPUTFILE" ]; then
	echo "Cleaning the target package file path..."
	rm -f "$OUTPUTFILE"
fi

cd "$TEMPDIR"

echo "Compressing sources into $OUTPUTFILE"

TARPARAMS="${COMPRESSSWITCH}cf"
tar -$TARPARAMS "$OUTPUTFILE" "$PKGSRCDIR"

echo "Removing target directory..."
rm -rf "$TEMPSRCDIR"

cd "$THISDIR"

ls -al $OUTPUTFILE

echo "Done."

