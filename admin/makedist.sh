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
PKGSRCDIR=kvirc-$2
TEMPSRCDIR=$TEMPDIR/$PKGSRCDIR
OUTPUTFILE=$THISDIR/kvirc-$2.tar.bz2

if test -d "$TEMPSRCDIR"; then
	echo "Removing stale target directory..."
	rm -rf $TEMPSRCDIR
fi

echo "Copying sources..."
cp -rf $1 $TEMPSRCDIR

echo "Determining svn revision..."
cd $1
svn update
REVISION=$(svnversion -n .)

echo "Revision is $REVISION"
echo $REVISION > $TEMPSRCDIR/.svnrevision

if test -f $OUTPUTFILE; then
	echo "Cleaning the target package file path..."
	rm -f $OUTPUTFILE
fi

cd $TEMPDIR

echo "Removing .svn directories.."
rm -fr $(find ./$PKGSRCDIR -name ".svn")

echo "Compressing sources into $OUTPUTFILE"
tar -jcvf $OUTPUTFILE $PKGSRCDIR

echo "Removing target directory..."
rm -rf $TEMPSRCDIR

cd $THISDIR

echo "Done."
