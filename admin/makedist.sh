#/bin/bash

usage() {
	echo "Usage:"
	echo "      $0 <source_tree_dir> <kvirc_version>"
}


if test -z "$1"; then
	usage
	exit
fi

if test -z "$2"; then
	usage
	exit
fi

THISDIR=$(pwd)
TEMPDIR=/tmp
PKGSRCDIR=kvirc-"$2"
TEMPSRCDIR="$TEMPDIR/$PKGSRCDIR"
OUTPUTFILE="$THISDIR/kvirc-$2.tar.bz2"

if test -d "$TEMPSRCDIR"; then
	echo "Removing stale target directory..."
	rm -rf "$TEMPSRCDIR"
fi

echo "Exporting svn dir ..."
svn export "${1}" "${TEMPSRCDIR}"

echo "Determining svn revision..."
cd "$1"
# If we update svn here, we have a different revision than we exported above
#svn update
REVISION=$(svnversion -n .)

echo "Revision is $REVISION"
echo $REVISION > "$TEMPSRCDIR/.svnrevision"

if test -f "$OUTPUTFILE" ; then
	echo "Cleaning the target package file path..."
	rm -f "$OUTPUTFILE"
fi

cd "$TEMPDIR"

echo "Compressing sources into $OUTPUTFILE"
tar -jcvf "$OUTPUTFILE" "$PKGSRCDIR"

echo "Removing target directory..."
rm -rf "$TEMPSRCDIR"

cd "$THISDIR"

echo "Done."
